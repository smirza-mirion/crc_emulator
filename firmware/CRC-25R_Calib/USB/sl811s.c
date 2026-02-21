/**
 * \file
 * \details This file contains functions, which calls the Cypress USB Chip (SL811) as slave and implements call to PC Communications.
 */

//*****************************************************************************************
// Cypress Semiconductor - Customer Design Center
//*****************************************************************************************
// Project		: (SL811S + Audio Control + Internet Control) Demo Code
// File			: sl811s.c
// H/W Target	: SL811HST(Rev 1.5) + EZUSB DEV KIT
// DCE/FAE		: Shawn Ng (cxn)
// Last Update	: 12/10/04  -- Marcia Suzuki
//                04/07/05 --  enable_reset_interrupt routine added
//                06/21/05 --  decoupled sending output packets from directly writing to SL811
//                02/20/07 -- routine for CapMca -- USB_Send_Data_Num  for use with non-ascii
//		  06/05/07 -- Split the clear_to_send flag into a frontend flag and a backend flag
//        05/20/8 -- change startup_811
//*****************************************************************************************
// Include files
//*****************************************************************************************
#include "crc.h"
#include "cs.h"
#include "pit.h"
#include "coldfire.h"
#include <string.h>
#include "sl811s.h"
#include "descp.h"
#include "keyboard.h"
#include "i2c.h"

#define END_CHAR  '#'
#define START_CHAR '$'
#define INPACKETBUFFERLEN	129
#define OUTPACKETBUFFERLEN	139
#define SOFMULTIP	1
short cdc_send_length = EP1_LEN;
char InPacketCircularBuffer[INPACKETBUFFERLEN];
unsigned short InPacketCircularHead = 0;
unsigned short InPacketCircularTail = 0;
char OutPacketCircularBuffer[OUTPACKETBUFFERLEN];
unsigned short OutPacketCircularHead = 0;
unsigned short OutPacketCircularTail = 0;
extern unsigned short InCDCCircularHead;
extern unsigned short InCDCCircularTail;
bool m_flgDTR, m_flgCarrier;
static unsigned long m_ulDataRate;
static unsigned char m_ucLineCoding[] = {0x80, 0x25, 0x00, 0x00, 0x00, 0x00, 0x08};
static unsigned char m_ucStop, m_ucParity, m_ucData;

static void enable_reset_interrupt(void);
//short InBufferUsed(void);
short OutBufferUsed(void);
//short InBufferFree(void);
short OutBufferFree(void);
bool InBufferPush(char *Input, short Length);
bool OutBufferPush(char *Input, short Length);
//bool InBufferPop(char *Output, short Length);
bool OutBufferPop(char *Output, short Length);
unsigned short OutBufferPop1(char *Output);
//char InBufferPeep(short BufferIndex);
//char OutBufferPeep(short BufferIndex);
//short InBufferScan(char testFor);
//short OutBufferScan(char testFor);
//void InBufferStrip(char testFor);
//void OutBufferStrip(char testFor);
extern CURRENT current;

#define USB_OFF_SET  4      // set register address
#define USB_OFF_RW   5      // reads from /writes to register

#define MAXIFCNUM 8
#define RESETINTSHUTDOWN 10	// number of consecutive reset interrupts before shutting down the reset interrupt


//*****************************************************************************************
// Variables Define
//*****************************************************************************************
typedef struct{
    BYTE bmRequest;					// SETUP Token Protocol
    BYTE bRequest;
    WORD wValue;
    WORD wIndex;
    WORD wLength;
} 	SetupPKG, *pSetupPKG;

//----------------------------------
// SL811S Variables
//----------------------------------
extern volatile uchar *pUSBSlaveAddress;
extern volatile uchar *pUSBSlaveData;

static char InPacket[EP2_LEN];
static bool valid_inpacket;

static BYTE	Slave_USBaddr;			// USB device address
static BYTE	Slave_ConfigVal;		// Device configuration value
static BYTE	Slave_Protocol;			// HID device protocol status
static BYTE	Slave_IdleRate;			// HID device idle rate value
static BYTE	Slave_RemoteWU;			// Device remote wakeup stats
static BYTE	Slave_inEPstall;		// EP0 ~ EP7's IN stall status
static BYTE	Slave_outEPstall;		// EP0 ~ EP7's OUT stall status
static BYTE	Slave_IfcAlt[MAXIFCNUM];// 8 interface(Ep0~7) contain alternate setting value

static BYTE DisableResetInterruptCountdown; // Counts down to zero and disables Reset Interrupt

static SetupPKG 	dReq;					// Setup token struct
static WORD		len_req;				// length of data for EP0
static BYTE		ep1_toggle;				// EP1 DATA toggle state
static BYTE		ep2_toggle;				// EP2 DATA toggle state
static BYTE		ep3_toggle;				// EP2 DATA toggle state
static BYTE		in_buffer_idx;			// EP0 IN data buffer tracking

static BYTE dev_first;

static BOOL 		IN_NULL;				// EP0's IN null packet transmission
static BOOL 		IN_EXACT;				// EP0's IN data length requested is extact of EP0_LEN
static BOOL 		BUS_POWERED;			// Bus powered device

static uchar OutPacket[EP1_LEN];
bool clear_to_send1 = TRUE;
static bool clear_to_send2 = TRUE;

static uchar running_counter = 0;
bool flgEP1A_AckReceived;
long SOF_counter_zero;

static void uC_init(void);

static BYTE SL811Read(BYTE a);
static void SL811Write(BYTE a, BYTE d);

static void SL811BufRead(BYTE addr, BYTE *s, BYTE c);
static void SL811BufWrite(BYTE addr, BYTE *s, BYTE c);

static void EP0A_IN_Arm(BYTE buf_adr, BYTE len, BYTE seq);
static void EP0A_OUT_Arm(BYTE len);
static void EP0A_ReArm(void);
static void EP1A_IN_Arm(BYTE buf_adr, BYTE len, BYTE seq);
static void EP2A_OUT_Arm(BYTE buf_adr, BYTE len, BYTE seq);
static WORD WordSwap(WORD input);

static void ep0_isr(void);
static void ep1_isr(void);
static void ep2_isr(void);
static void sl811s_init(void);
static void sof_isr(void);

static void ep1_isr_cdc(void);
static void ep2_isr_cdc(void);
static void ep3_isr_cdc(void);
static BYTE SerialString[] = {18,3,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,'0',0};

/**
 * \details Read calibrator serial number and place it into the SerialString array
 * \returns None
 */
static void set_usb_serial_number(void){
	char sn[10];
	short i;

    ReadSN(sn);

	SerialString[0] = 22;
	for(i = 0; i < 10; i++)
	{
		SerialString[2 + 2 * i] = sn[i];
	}
}
/**
 * \details Power on the USB Slave connector and call uC_init
 * \returns None
 */
void startup_811(void){
	set_usb_serial_number();
	set_usb_power(FALSE);
	delay_msec(500);
	set_usb_power(TRUE);
	delay_msec(10);

	DisableResetInterruptCountdown = RESETINTSHUTDOWN;
	uC_init();
}
/**
 * \details Initialize the SL811 Slave registers
 * \returns None
 */
static void uC_init(void){
	*pUSBSlaveAddress = 0x00;
	*pUSBSlaveData = 0x00;

	SL811Write(cDATASet, 0x00);		// use "A" side p.14 5.4.5
	SL811Write(IntEna, 0x40);		// enable USB Reset interrupt p.13 5.4.2
	SL811Write(CtrlReg, 0x01);		// enable USB, FullSpeed p.12 5.4.1
	SL811Write(IntStatus, 0xFF);		// clear all interrupts p.13 5.4.4
}
/**
 * \details Interrupt handler for SL811 Slave. This routine handles calls for all the USB endpoints.
 * \returns None
 */
void isr_811(void){
	BYTE int_status, int_enabled;

	int_status = SL811Read(IntStatus);	// read interrupt status p.13 5.4.4
	if(int_status & USB_RESET_DONE){	// wait for USB Reset interrupt
		sl811s_init();
		// Decrement countdown if not zero, When countdown reaches zero then disable Reset Interrupt
		if(DisableResetInterruptCountdown){
			DisableResetInterruptCountdown--;
		}else{
			int_enabled = SL811Read(IntEna);	// read the currently enable interrupts p.13 5.4.2
			int_enabled = int_enabled & 0xBF;	// turn off USB Reset interrupt
			SL811Write(IntEna, int_enabled);	// write masked interrupt setting p.13 5.4.2
		}
	}else if(int_status & EP0_DONE){				// wait for EP0 interrupt
		SL811Write(IntStatus, EP0_DONE);			// clear EP0 interrupt p.13 5.4.4
		ep0_isr();
		enable_reset_interrupt();						// Enable the Reset Interrupt if countdown is zero and resets the countdown
	}else if(int_status & EP1_DONE){						// wait for EP1 interrupt
		SL811Write(IntStatus,EP1_DONE);				// clear EP1 interrupt
		if(current.usb_device_protocol == 0){
			ep1_isr();
		}else if(current.usb_device_protocol == 1){
			ep1_isr_cdc();
		}
		enable_reset_interrupt();						// Enable the Reset Interrupt if countdown is zero and resets the countdown
	}else if(int_status & EP2_DONE){						// wait for EP2 interrupt
		SL811Write(IntStatus,EP2_DONE);				// clear EP2 interrupt
		if(current.usb_device_protocol == 0){
			ep2_isr();
		}else if(current.usb_device_protocol == 1){
			ep2_isr_cdc();
		}
		enable_reset_interrupt();						// Enable the Reset Interrupt if countdown is zero and resets the countdown
	}else if(int_status & EP3_DONE){						// wait for EP3 interrupt
		SL811Write(IntStatus,EP3_DONE);				// clear EP3 interrupt
		enable_reset_interrupt();					// Enable the Reset Interrupt if countdown is zero and resets the countdown
	}else if(int_status & SOF_DONE){						// wait for SOF interrupt
		SL811Write(IntStatus,SOF_DONE);				// clear SOF interrupt
		sof_isr();
	}
}
/**
 * \details Setup the SL811 on chip buffer and registers
 * \returns None
 */
static void sl811s_init(void){
	int i;

	dev_first = 0;
	ep1_toggle = 0;				// ep1 toggle state
	ep2_toggle = 0;
	ep3_toggle = 0;

	//----------------------------
	// SL811S-Specific
	//----------------------------
	BUS_POWERED = 1;								// define as a bus powered device
	Slave_USBaddr = 0;								// set to default USB address zero
	Slave_ConfigVal = 0;								// default device config value
	Slave_Protocol = 0;									// HID class default boot protocol
	Slave_IdleRate = 0;								// HID class default idle rate
	Slave_RemoteWU = 0;								// device remote wakeup support
	Slave_inEPstall = 0;									// EP0 ~ EP7's IN
	Slave_outEPstall = 0;								// EP0 ~ EP7's OUT
	for(i=0; i<MAXIFCNUM; i++) Slave_IfcAlt[i] = 0;				// reset alternate setting
	for(i=0; i<EP0_LEN; i++) SL811Write(EP0A_Slave_Buf + i, 0);		// clear EP0 Buffer
	for(i=0; i<EP1_LEN; i++) SL811Write(EP1A_Slave_Buf + i, 0);		// clear EP1 Buffer
	for(i=0; i<EP2_LEN; i++) SL811Write(EP2A_Slave_Buf + i, 0);		// clear EP2 Buffer
	for(i=0; i<EP3_LEN; i++) SL811Write(EP3A_Slave_Buf + i, 0);		// clear EP2 Buffer

	valid_inpacket = FALSE;
	SL811Write(USBAddress, 0x00);							// set usb address p.13 5.4.3
	if(current.usb_device_protocol == 0){
		SL811Write(IntEna, 0x67);		// enable SOF, EP0, EP1, EP2, USB Reset interrupts p.13 5.4.2
	}else if(current.usb_device_protocol == 1){
		SL811Write(IntEna, 0x67);	// enable SOF, EP0, EP1, EP2, USB Reset interrupts p.13 5.4.2
	}
	SL811Write(IntStatus, 0xFF);							// clear all interrupts p.13 5.4.4
	EP0A_OUT_Arm(EP0_LEN);							// ready to receive from host
}

//*****************************************************************************************
// EP0 interrupt service routine
//*****************************************************************************************
/**
 * \details Routine to handle calls to Endpoint 0. Responses to USB enumeration come from here.
 * \returns None
 */
static void ep0_isr(void){
	BYTE status, byte_rx, len_xfr;
	BYTE data_seq, req_type;

	status = SL811Read(EP0AStatus);						// get packet status p.11 5.3.3

	if(status & 0x22){
		EP0A_ReArm();
		return;
	}

	byte_rx = SL811Read(EP0AXferLen) - SL811Read(EP0ACounter);		// get no. of bytes received p.11 5.3.2, 5.3.4
												// for OUT data from host
	//----------------------------------------------------------------------------------------
	// ACK received
	//----------------------------------------------------------------------------------------
	if(status & EP_ACK){
		//----------------------------------------------------------------
		// Set newly assigned USB address
		//----------------------------------------------------------------
		if(Slave_USBaddr){								// if new USB address was assigned,
			SL811Write(USBAddress, Slave_USBaddr);			// communicate all USB transaction p.13 5.4.3
			Slave_USBaddr = 0;						// using this new address.
		}

		//================================================================
		// SETUP's ACKed
		//================================================================
		if(status & EP_SETUP){
			SL811BufRead(EP0A_Slave_Buf, (BYTE*)&dReq, byte_rx);	// capture SETUP data request
			len_req = WordSwap(dReq.wLength);				// len_req = actual requested length
			in_buffer_idx = 0;							// reset buffer locatio indexing
			IN_NULL = FALSE;							// these are for IN-NULL packet
			IN_EXACT = FALSE;						// transfer condition
			req_type = (dReq.bmRequest & 0x60) >> 5;			// decode for Std,Class,Vendor type

			switch(req_type){							// Parse bmRequest Type
				//---------------------------------------------------------------------
				// Standard USB Requests
				//---------------------------------------------------------------------
				case STD_REQUEST:
					switch(dReq.bRequest){					// Parse bRequest
						case GET_DESCRIPTOR:
							switch ((BYTE)dReq.wValue){		// Parse wValue
								case DEVICE:
									if(current.usb_device_protocol == 0){
										SL811BufWrite(EP0A_Slave_Buf, (BYTE*)Dev_Descp, DEV_LEN);			// load Device Descp for HID
									}else if(current.usb_device_protocol == 1){
										SL811BufWrite(EP0A_Slave_Buf, (BYTE*)Dev_Descp_CDC, DEV_LEN);		// load Device Descp for CDC
									}
									len_req = (len_req >= DEV_LEN) ? DEV_LEN : len_req;					// get exact data length
									break;

								case CONFIGURATION:
									if(current.usb_device_protocol == 0){
										SL811BufWrite(EP0A_Slave_Buf, (BYTE*)Cfg_Descp, CFG_LEN);			// load Config Descp for HID
										len_req = (len_req >= CFG_LEN) ? CFG_LEN : len_req;				// get exact data length
									}else if(current.usb_device_protocol == 1){
										SL811BufWrite(EP0A_Slave_Buf, (BYTE*)Cfg_Descp_CDC, CFG_LEN_CDC);	// load Config Descp for CDC
										len_req = (len_req >= CFG_LEN_CDC) ? CFG_LEN_CDC : len_req;			// get exact data length
									}
									break;

								case HID_DEV:
									SL811BufWrite(EP0A_Slave_Buf, (BYTE*)Cfg_Descp+18, HID_LEN);			// load HID Class Descp
									len_req = (len_req >= HID_LEN) ? HID_LEN:len_req;						// get exact data length
									break;

								case HID_REPORT:
									SL811BufWrite(EP0A_Slave_Buf, (BYTE*)Rep_Descp, REP_LEN);				// load Report Descp
									len_req = (len_req>=REP_LEN) ? REP_LEN : len_req;					// get exact data length
									break;

								case STRING:
									switch(dReq.wValue >> 8){									// get string index
										case 0x00:
											SL811BufWrite(EP0A_Slave_Buf, (BYTE*)LangString, LangString[0]);
											len_req = (len_req>=LangString[0]) ? LangString[0]:len_req;
											break;

										case 0x01:
											SL811BufWrite(EP0A_Slave_Buf, (BYTE*)MfgString, MfgString[0]);
											len_req = (len_req >= MfgString[0]) ? MfgString[0] : len_req;
											break;

										case 0x02:
											SL811BufWrite(EP0A_Slave_Buf, (BYTE*)ProdString, ProdString[0]);
											len_req = (len_req >= ProdString[0]) ? ProdString[0] : len_req;
											break;

										case 0x03:
											SL811BufWrite(EP0A_Slave_Buf, (BYTE*)SerialString, SerialString[0]);
											len_req = (len_req >= SerialString[0]) ? SerialString[0] : len_req;
											break;
									}
									break;
							}

							if(len_req == WordSwap(dReq.wLength)) IN_EXACT = TRUE;						// if requested length is equal to the
																						// exact length of descriptor, set IN_EXACT
																						// is use during IN-NULL pkt trasnmission

							len_xfr = (len_req >= EP0_LEN) ? EP0_LEN : (BYTE)len_req;						// get current IN transfer length
							EP0A_IN_Arm(EP0A_Slave_Buf, len_xfr, 1);									// Arm IN response, start with DATA1 seq
							in_buffer_idx += len_xfr;												// update to next muliple buffer location
							len_req -= len_xfr;													// update data length for current transfer
							break;

						case GET_CONFIG:
							SL811Write(EP0A_Slave_Buf, Slave_ConfigVal);								// load current configuration value
							EP0A_IN_Arm(EP0A_Slave_Buf, 1, 1);										// send 1 byte data back to host
							len_req = 0;
							break;

						case GET_INTERFACE:
							SL811Write(EP0A_Slave_Buf, Slave_IfcAlt[dReq.wIndex>>8]);						// load current alternate setting
							EP0A_IN_Arm(EP0A_Slave_Buf, 1, 1);										// send 1 byte data back to host
							len_req = 0;
							break;

						case GET_STATUS:
							switch(dReq.bmRequest&0x03){											// check for recipients
								case RECIPIENT_DEV:											// load current device status
									SL811Write(EP0A_Slave_Buf, (Slave_RemoteWU << 1) | BUS_POWERED);
									break;

								case RECIPIENT_IFC:
									SL811Write(EP0A_Slave_Buf, 0);									// first byte = 0
									break;

								case RECIPIENT_ENP:
									if((dReq.wIndex >> 8) & 0x80){												// for IN direction endpoint
										if(Slave_inEPstall & (0x01 << ((dReq.wIndex >> 8) & 0x0F))) SL811Write(EP0A_Slave_Buf, 1);	// first byte = 1 (IN endpoint stall)
										else SL811Write(EP0A_Slave_Buf, 0);										// first byte = 0 (IN endpoint not stall)
									}else{																// for OUT direction endpoint
										if(Slave_outEPstall & (0x01 << ((dReq.wIndex >> 8) & 0x0F))) SL811Write(EP0A_Slave_Buf, 1);	// first byte = 1 (OUT endpoint stall)
										else SL811Write(EP0A_Slave_Buf, 0);										// first byte = 0 (OUT endpoint not stall)
									}
									break;
							}
							SL811Write(EP0A_Slave_Buf + 1, 0);										// second byte = 0
							EP0A_IN_Arm(EP0A_Slave_Buf, 2, 1);										// send 2 bytes data back to host
							len_req = 0;														// reset request length to zero
							break;

						case SET_FEATURE:
						case CLEAR_FEATURE:
							switch(dReq.bmRequest&0x03){											// check for recipients
								case RECIPIENT_DEV:
									if((dReq.wValue>>8) == 1){									// feature selector = 1 (remote wakeup)
										if(dReq.bRequest == SET_FEATURE) Slave_RemoteWU = 1;			// set remote wake up status
										if(dReq.bRequest == CLEAR_FEATURE) Slave_RemoteWU = 0;			// clear remote wake up status
										EP0A_IN_Arm(EP0A_Slave_Buf, 0, 1);									// IN status stage
									}else SL811Write(EP0AControl, SEND_STALL);							// Stall unsupported requests
									break;

								case RECIPIENT_ENP:
									if((dReq.wValue >> 8) == 0){														// feature selector = 0 (endpoint stall)
										if((dReq.wIndex>>8) & 0x80){													// for IN direction endpoint
											if(dReq.bRequest == SET_FEATURE) Slave_inEPstall |= (0x01 << ((dReq.wIndex >> 8) & 0x0F));		// set endpoint stall (limit to 7 IN's data endpoint)
											if(dReq.bRequest == CLEAR_FEATURE) Slave_inEPstall &= ~(0x01 << ((dReq.wIndex >> 8) & 0x0F));	// clear endpoint stall (limit to 7 IN's data endpoint)
										}else{																	// for OUT direction endpoint
											if(dReq.bRequest == SET_FEATURE) Slave_outEPstall |= (0x01 << ((dReq.wIndex >> 8) & 0x0F));		// set endpoint stall (limit to 7 OUT's data endpoint)
											if(dReq.bRequest == CLEAR_FEATURE) Slave_outEPstall &= ~(0x01 << ((dReq.wIndex >> 8) & 0x0F));	// clear endpoint stall (limit to 7 OUT's data endpoint)
										}
										EP0A_IN_Arm(EP0A_Slave_Buf, 0, 1);									// IN status stage
									}
									else SL811Write(EP0AControl, SEND_STALL);							// Stall unsupported requests
									break;

								default:
									SL811Write(EP0AControl, SEND_STALL);							// Stall all unsupported requests
									break;
							}
							break;

						case SET_ADDRESS:
							Slave_USBaddr = dReq.wValue >> 8;										// update new USB address assigned by host
							EP0A_IN_Arm(EP0A_Slave_Buf, 0, 1);												// IN status stage
							break;

						case SET_CONFIG:
							Slave_ConfigVal = dReq.wValue >> 8;										// update configuration value
							ep1_toggle = 0;													// ep1 toggle state
							ep2_toggle = 0;
							ep3_toggle = 0;
							InCDCCircularHead = InCDCCircularTail = OutPacketCircularHead = OutPacketCircularTail = InPacketCircularHead = InPacketCircularTail = 0;
							EP0A_IN_Arm(EP0A_Slave_Buf, 0, 1);												// IN status stage
							if(Slave_ConfigVal){
								if(current.usb_device_protocol == 0){
									SL811Write(EP1AControl,DATA0_IN_ENABLE);						// Enable EP1 (reponse with NAK)
								}else if(current.usb_device_protocol == 1){
									SL811Write(EP1AControl, DATA0_IN_ENABLE);						// Enable EP1 (reponse with NAK)
									SL811Write(EP2AAddress, EP2A_Slave_Buf);
									SL811Write(EP2AXferLen, EP2_LEN);
									SL811Write(EP2AControl, DATA0_OUT);							// Enable and Arm EP2
									SL811Write(EP3AControl, DATA0_IN_ENABLE);						// Enable EP3 (reponse with NAK)
								}
							}else{
								if(current.usb_device_protocol == 0){
									SL811Write(EP1AControl, DATA0_IN_DISABLE);						// Disable EP1
								}else if(current.usb_device_protocol == 1){
									SL811Write(EP1AControl, DATA0_IN_DISABLE);						// Disable EP1
									SL811Write(EP2AAddress, EP2A_Slave_Buf);
									SL811Write(EP2AXferLen, EP2_LEN);
									SL811Write(EP2AControl, DATA0_OUT_DISABLE);						// Disable EP2
									SL811Write(EP3AControl, DATA0_IN_DISABLE);						// Disable EP2
								}
							}
							break;

						case SET_INTERFACE:													// update alternate setting for
							Slave_IfcAlt[dReq.wIndex >> 8] = dReq.wValue >> 8;							// selected interface number
							EP0A_IN_Arm(EP0A_Slave_Buf, 0, 1);												// IN status stage
							break;

						default:
							SL811Write(EP0AControl, SEND_STALL);									// Stall all unsupported requests
							break;
					}
					break;

				//---------------------------------------------------------------------
				// Specific Class Requests (HID)
				//---------------------------------------------------------------------
				case CLASS_REQUEST:
					 switch(dReq.bRequest){														// Parse bRequest
						case GET_IDLE:
							SL811Write(EP0A_Slave_Buf, Slave_IdleRate);								// load current idle rate value
							EP0A_IN_Arm(EP0A_Slave_Buf, 1, 1);										// send 1 byte data back to host
							len_req = 0;
							break;

						case GET_PROTOCOL:
							SL811Write(EP0A_Slave_Buf, Slave_Protocol);								// load current protocol state
							EP0A_IN_Arm(EP0A_Slave_Buf, 1, 1);										// send 1 byte data back to host
							len_req = 0;
							break;

						case SET_IDLE:
							Slave_IdleRate = (BYTE)dReq.wValue;										// update Idle Rate (upper byte of wValue)
							EP0A_IN_Arm(EP0A_Slave_Buf, 0, 1);												// IN status stage
							break;

						case SET_PROTOCOL:
							Slave_Protocol = dReq.wValue >> 8;										// update protocol value, 0=Boot, 1=report
							EP0A_IN_Arm(EP0A_Slave_Buf, 0, 1);												// IN status stage
							break;

						case SET_CONTROL_LINE_STATE: // 0x22
							if(current.usb_device_protocol == 0){
								SL811Write(EP0AControl, SEND_STALL);								// Stall all unsupported requests
							}else if(current.usb_device_protocol == 1){
								EP0A_IN_Arm(EP0A_Slave_Buf, 0, 1);											// IN status stage
								while(!(SL811Read(EP0AControl) & 0x01)); 	// Wait for Arm bit to set
								while(SL811Read(EP0AControl) & 0x01);		// Wait for Arm bit to clear

								SL811Write(IntStatus, EP0_DONE);			// Clear Interrupt

								EP0A_OUT_Arm(EP0_LEN);						// Arm next SETUP requests
								while(SL811Read(IntStatus) & EP0_DONE);		// Wait for Interrupt Status to clear
							}
							break;

						case GET_CONTROL_LINE_CODING: // 0x21
							if(current.usb_device_protocol == 0){
								SL811Write(EP0AControl, SEND_STALL);								// Stall all unsupported requests
							}else if(current.usb_device_protocol == 1){
								len_req = 0;
								len_xfr = 7;
								SL811BufWrite(EP0A_Slave_Buf, (BYTE*)m_ucLineCoding, len_xfr);

								EP0A_IN_Arm(EP0A_Slave_Buf, len_xfr, 1);
								while(!(SL811Read(EP0AControl) & 0x01));		// Wait for Arm bit to set
								while(SL811Read(EP0AControl) & 0x01);			// Wait for Arm bit to clear

								EP0A_OUT_Arm(EP0_LEN);
								while(!(SL811Read(EP0AControl) & 0x01));		// Wait for Arm bit to set
								while(SL811Read(EP0AControl) & 0x01);			// Wait for Arm bit to clear

								SL811Write(IntStatus, EP0_DONE);				// Clear Interrupt

								EP0A_OUT_Arm(EP0_LEN);							// Arm next SETUP requests
								while(SL811Read(IntStatus) & EP0_DONE);			// Wait for Interrupt Status to clear
							}
							break;

						case SET_CONTROL_LINE_CODING: //0x20;
							if(current.usb_device_protocol == 0){
								SL811Write(EP0AControl, SEND_STALL);								// Stall all unsupported requests
							}else if(current.usb_device_protocol == 1){
								EP0A_OUT_Arm(len_req);
							}
							break;

						default:
							SL811Write(EP0AControl, SEND_STALL);									// Stall all unsupported requests
							break;
					}
					break;

				//---------------------------------------------------------------------
				// Specific Vendor Requests
				//---------------------------------------------------------------------
				case VENDOR_REQUEST:
					SL811Write(EP0AControl, SEND_STALL);							// Stall all unsupported requests
					break;

				//---------------------------------------------------------------------
				// Unsupported Requests
				//---------------------------------------------------------------------
				default:
					SL811Write(EP0AControl, SEND_STALL);							// Stall all unsupported requests
					break;
			}
		}else{
			//---------------------------------
			// IN's ACKed
			//---------------------------------
			if(SL811Read(EP0AControl) & DIRECTION){
				if(dev_first == 0){												// happens on the first get device descp
					len_req = 0;												// the host terminate IN transfer prematurely
					dev_first = 1;											// (if EP0 maxpktsize is only 8 bytes)
				}														// reset len_req, end with OUT status stage

				//--------------------------------------
				// Continue with next IN if needed
				//--------------------------------------
				if(len_req){
					data_seq = (((SL811Read(EP0AControl) & DATAX) == 0) ? 1 : 0);				// toggle DATA sequence
					len_xfr = (len_req >= EP0_LEN) ? EP0_LEN : (BYTE)len_req;				// get transfer length for EP0

					if(!IN_NULL) EP0A_IN_Arm(EP0A_Slave_Buf + in_buffer_idx, len_xfr, data_seq);	// point to correct buffer location
					else EP0A_IN_Arm(EP0A_Slave_Buf, 0, data_seq);								// transfer zero Null packet

					in_buffer_idx += len_xfr;										// update buffer location
					len_req -= len_xfr;											// update remaining data length

					if(len_req==0 && len_xfr == EP0_LEN && !IN_EXACT){					// handles null packet of data length
						len_req = 1;											// to host is multiple of EP0_LEN, and
						IN_NULL = TRUE;										// only if host request more than this
					}													// value of length, otherwise, the extra
				}														// null pkt is not require.
				//--------------------------------------
				// Arm Status Stage OUT or new SETUP
				//--------------------------------------
				else														// Arm status OUT or next SETUP requests
					EP0A_OUT_Arm(EP0_LEN);									// i.e. end of IN status stage.
			}else{
				//---------------------------------
				// OUT's ACKed
				//---------------------------------
				if(byte_rx == 0){												// zero data packet received
					EP0A_OUT_Arm(EP0_LEN);									// get ready for next SETUP token
					if((BYTE)dReq.wValue == HID_REPORT){							// end of a report descp, indicate
						valid_inpacket = FALSE;
						EP2A_OUT_Arm(EP2A_Slave_Buf, EP2_LEN, ep2_toggle);
					}
				}else if(byte_rx == 7){
					if(current.usb_device_protocol == 1){
						SL811BufRead(EP0A_Slave_Buf, (BYTE*)&m_ucLineCoding, byte_rx);
						len_req = 0;

						EP0A_IN_Arm(EP0A_Slave_Buf, len_req, 1);						// IN status stage
						while(!(SL811Read(EP0AControl) & 0x01));						// Wait for Arm bit to set
						while(SL811Read(EP0AControl) & 0x01);							// Wait for Arm bit to clear

						SL811Write(IntStatus, EP0_DONE);								// Clear Interrupt

						EP0A_OUT_Arm(EP0_LEN);											// Arm next SETUP requests
						while(SL811Read(IntStatus) & EP0_DONE);							// Wait for Interrupt Status to clear
					}
				}
			}
		}
	}
	//----------------------------------------------------------------------------------------
	// End of ACK received
	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------
	// If a STALL was sent out by device, it will still interrupt, but
	// STALL bit in status register does not reflect this when in slave mode.
	//----------------------------------------------------------------------------------------
	else if( SL811Read(EP0AControl)&0x20 )											// check for previous stall sent
		EP0A_OUT_Arm(EP0_LEN);												// get ready for next SETUP token
	return;
}

//*****************************************************************************************
// EP1 interrupt service routine
//*****************************************************************************************
/**
 * \details Routine to handle a HID Endpoint 1 call
 * \returns None
 */
static void ep1_isr(void){
	BYTE bStatus;

	bStatus = SL811Read(EP1AStatus);
	if(bStatus & EP_ACK){
		ep1_toggle = (((SL811Read(EP1AControl)&DATAX)==0) ? 1:0);							// toggle DATA sequence
		flgEP1A_AckReceived = TRUE;
	}else{
		//printf ("Endpoint 1 Error: %Xh\n", bStatus);
		EP1A_IN_Arm(EP1A_Slave_Buf, EP1_LEN, ep1_toggle);
		SOF_counter_zero = 10000;
	}
	return;
}
/**
 * \details Routine to handle a CDC Endpoint 1 call
 */
static void ep1_isr_cdc(void){
	BYTE bStatus;

	bStatus = SL811Read(EP1AStatus);
	if(bStatus & EP_ACK){
		ep1_toggle = (((SL811Read(EP1AControl) & DATAX) == 0) ? 1 : 0);						// toggle DATA sequence
		flgEP1A_AckReceived = TRUE;
	}else{
		//printf ("Endpoint 1 Error: %Xh\n", bStatus);
		EP1A_IN_Arm(EP1A_Slave_Buf, cdc_send_length, ep1_toggle);
		SOF_counter_zero = 10000;
	}
}

//*****************************************************************************************
// EP2 interrupt service routine
//*****************************************************************************************
/**
 * \details Routine to handle a HID Endpoint 2 call
 * \returns None
 */
static void ep2_isr(void){
	BYTE bStatus;

	bStatus = SL811Read(EP2AStatus);
//	if(SL811Read(EP2AStatus) & EP_ACK)												// check for ACK bit set
	if(bStatus & EP_ACK){
		ep2_toggle = (((SL811Read(EP2AControl)&DATAX)==0) ? 1:0);				// toggle DATA sequence
		SL811BufRead(EP2A_Slave_Buf, (BYTE *)InPacket, EP2_LEN);
		EP2A_OUT_Arm(EP2A_Slave_Buf, EP2_LEN, ep2_toggle);
		valid_inpacket = TRUE;
	}else{
		//printf ("Endpoint 2 Error: %Xh\n", bStatus);
		EP2A_OUT_Arm(EP2A_Slave_Buf, EP2_LEN, ep2_toggle);
	}
	return;
}
/**
 * \details Routine to handle a CDC Endpoint 2 call
 * \returns None
 */
static void ep2_isr_cdc(void){
	BYTE bStatus, bReceivedCount;
	char tempBuffer[64];
	//short index, bufferCount, commandLength;

	bStatus = SL811Read(EP2AStatus);
	if(bStatus & EP_ACK){
		ep2_toggle = (((SL811Read(EP2AControl) & DATAX) == 0) ? 1:0);							// toggle DATA sequence
		bReceivedCount = SL811Read(EP2AXferLen) - SL811Read(EP2ACounter);
		EP2A_OUT_Arm(EP2A_Slave_Buf, EP2_LEN, ep2_toggle);
		if(bReceivedCount > 0){
			SL811BufRead(EP2A_Slave_Buf, (BYTE *)tempBuffer, bReceivedCount);
			if(*tempBuffer == '@'){
				if((SL811Read(EP1AControl) & 0x01) == 0x00){
					tempBuffer[0] = START_CHAR;
					tempBuffer[1] = 'O';
					tempBuffer[2] = END_CHAR;
					tempBuffer[3] = '\0';
					cdc_send_length = 3;
					SL811BufWrite(EP1A_Slave_Buf, (BYTE *)tempBuffer, cdc_send_length);
					EP1A_IN_Arm(EP1A_Slave_Buf, cdc_send_length, ep1_toggle);
				}else{
					InBufferPush(tempBuffer, bReceivedCount);
				}
			}else{
				InBufferPush(tempBuffer, bReceivedCount);
			}
		}
		//EP2A_OUT_Arm(EP2A_Slave_Buf, EP2_LEN, ep2_toggle);
	}else{
		//printf ("Endpoint 2 Error: %Xh\n", bStatus);
		EP2A_OUT_Arm(EP2A_Slave_Buf, EP2_LEN, ep2_toggle);
	}
}

//******************************************************************************************
// EP3 interrupt service routine
//******************************************************************************************
static void ep3_isr_cdc(void){
	//BYTE bStatus;

	//printf("Endpoint 3 Interrupt\n");
	/*bStatus = SL811Read(EP2AStatus);
	SL811Write(IntStatus,EP2_DONE);					// clear EP2 interrupt
	if(bStatus & EP_ACK){
		ep2_toggle = (((SL811Read(EP2AControl)&DATAX)==0) ? 1:0);
													// toggle DATA sequence
		SL811BufRead(EP2A_Slave_Buf, (BYTE *)InPacket, EP2_LEN);
		EP2A_OUT_Arm(EP2A_Slave_Buf, EP2_LEN, ep2_toggle);
		valid_inpacket = TRUE;
	}else{
		//printf ("Endpoint 2 Error: %Xh\n", bStatus);
	}*/
}

//*****************************************************************************************
// SOF interrupt service routine (act as 1ms timer)
//*****************************************************************************************
/**
 * \details Routine to handle the SOF (Start Of Frame) call. This routine is called each millisecond and is used to monitor the Endpoint 1 process
 * \returns None
 */
static void sof_isr(void){
	static short block = 0;
	short length;
	BYTE singleChar[EP1_LEN];

	if(SL811Read(USBAddress) != 0){
		if(current.usb_device_protocol == 0){
			if(!clear_to_send2){
				if((SL811Read(EP1AControl) &  0x01) == 0x00){
					clear_to_send2 = TRUE;
					SL811BufWrite(EP1A_Slave_Buf, OutPacket, EP1_LEN);
					EP1A_IN_Arm(EP1A_Slave_Buf, EP1_LEN, ep1_toggle);

					flgEP1A_AckReceived = FALSE;
					while ((SL811Read(EP1AControl) & 0x01)== 0x00);
					SOF_counter_zero = 10000;
					while ((!((SL811Read(EP1AControl) & 0x01)== 0x00)) && (SOF_counter_zero != 0)) SOF_counter_zero-=1;

					if (SOF_counter_zero != 0){
						SOF_counter_zero = 10000;
						while ((flgEP1A_AckReceived == FALSE) && (SOF_counter_zero != 0)) SOF_counter_zero-=1;
					}


					clear_to_send1 = TRUE;
				}
			}
		}else if(current.usb_device_protocol == 1){
			if((SL811Read(EP1AControl) &  0x01) == 0x00){
				if(!block){
					block = 1;
					length = OutBufferUsed();
					if(length){
						if(length > EP1_LEN) length = EP1_LEN;
						cdc_send_length = length;
						OutBufferPop((char *) singleChar, cdc_send_length);
						SL811BufWrite(EP1A_Slave_Buf, singleChar, cdc_send_length);
						EP1A_IN_Arm(EP1A_Slave_Buf, cdc_send_length, ep1_toggle);

						flgEP1A_AckReceived = FALSE;
						while ((SL811Read(EP1AControl) & 0x01)== 0x00);
						SOF_counter_zero = 10000;
						while ((!((SL811Read(EP1AControl) & 0x01)== 0x00)) && (SOF_counter_zero != 0)) SOF_counter_zero-=1;

						if (SOF_counter_zero != 0){
							SOF_counter_zero = 10000;
							while ((flgEP1A_AckReceived == FALSE) && (SOF_counter_zero != 0)) SOF_counter_zero-=1;
						}
					}
					block = 0;
				}
			}
		}
	}
}

//==========================================================================================================================================================

//*****************************************************************************************
// Byte Read from SL811H
// a = register address
// return = data in register
//*****************************************************************************************
/**
 * \details Read byte from SL811
 * \param a Register address
 * \returns Data in register
 */
static BYTE SL811Read(BYTE a){
	*pUSBSlaveAddress = a;
	return *pUSBSlaveData;
}

//*****************************************************************************************
// Byte Write to SL811H
// a = register address
// d = data to be written to this register address
//*****************************************************************************************
/**
 * \details Write byte to SL811
 * \param a Register address
 * \param d Data to be written
 */
static void SL811Write(BYTE a, BYTE d){
	*pUSBSlaveAddress = a;
	*pUSBSlaveData = d;
}

//*****************************************************************************************
// Buffer Read from SL811H
// addr = buffer start address
// s    = return buffer address where data are to be save/read
// c	= buffer data length
//*****************************************************************************************
/**
 * \details Buffer read from SL811
 * \param addr Buffer start address
 * \param s Pointer to where data is saved
 * \param c Buffer data length
 * \returns None
 */
static void SL811BufRead(BYTE addr, BYTE *s, BYTE c){
   	while(c--){
   		*pUSBSlaveAddress = addr++;
   		*(s++) = *pUSBSlaveData;
   	}
}

//*****************************************************************************************
// Buffer Write  to SL811H
// addr = buffer start address
// s    = buffer address where data are to be written
// c	= buffer data length
//*****************************************************************************************
/**
 * \details Buffer write to SL811
 * \param addr Buffer start address
 * \param s Pointer to data to be written
 * \param c Buffer data length
 * \returns None
 */
static void SL811BufWrite(BYTE addr, BYTE *s, BYTE c){
   	while(c--){
   		*pUSBSlaveAddress = addr++;
   		*pUSBSlaveData = *(s++);
   	}
}

//*****************************************************************************************
// EP0's IN Token Arming (using Set A)
//*****************************************************************************************
/**
 * \details Configure SL811 Endpoint 0 for IN operation
 * \param buf_adr Buffer start address
 * \param len Max transfer length allowed
 * \param seq Sequence status
 * \returns None
 */
static void EP0A_IN_Arm(BYTE buf_adr, BYTE len, BYTE seq){
	SL811Write(EP0AAddress, buf_adr);			// ep0 address buffer start adress
	SL811Write(EP0AXferLen, len);				// max length of transfer allowed
	if(seq) SL811Write(EP0AControl, DATA1_IN);		// armed to transmit to host, DATA1
	else SL811Write(EP0AControl, DATA0_IN);		// armed to transmit to host, DATA0
}

//*****************************************************************************************
// EP0's SETUP/OUT Token Arming (using Set A)
//*****************************************************************************************
/**
 * \details Configure SL811 Endpoint 0 for OUT operation
 * \param len Max transfer length allowed
 * \returns None
 */
static void EP0A_OUT_Arm(BYTE len){
	SL811Write(EP0AAddress, EP0A_Slave_Buf);		// ep0 address buffer start adress
 	SL811Write(EP0AXferLen, len);				// max length of transfer allowed
 	SL811Write(EP0AControl, DATA0_OUT);		// armed to receive from host
}

static void EP0A_ReArm(void){
	BYTE current;
	current = SL811Read(EP0AControl);
	current |= 0x01;
	SL811Write(EP0AControl, current);
}

//*****************************************************************************************
// EP1's IN Token Arming (using Set A)
//*****************************************************************************************
/**
 * \details Configure SL811 Endpoint 1 for IN operation
 * \param buf_adr Buffer start address
 * \param len Max transfer length allowed
 * \param seq Sequence status
 * \returns None
 */
static void EP1A_IN_Arm(BYTE buf_adr, BYTE len, BYTE seq){
    SL811Write(EP1AAddress,buf_adr); 				// ep1 address buffer start adress
	SL811Write(EP1AXferLen,len);					// max length of transfer allowed
	if(seq) SL811Write(EP1AControl,DATA1_IN);			// armed to transmit to host, DATA1
	else SL811Write(EP1AControl,DATA0_IN);			// armed to transmit to host, DATA0
}

//*****************************************************************************************
// EP2's OUT Token Arming (using Set A)
//*****************************************************************************************
/**
 * \details Configure SL811 Endpoint 2 for OUT operation
 * \param buf_adr Buffer start address
 * \param len Max transfer length allowed
 * \param seq Sequence status
 * \returns None
 */
static void EP2A_OUT_Arm(BYTE buf_adr, BYTE len, BYTE seq){
	SL811Write(EP2AAddress,buf_adr);				// ep2 address buffer start adress
	SL811Write(EP2AXferLen,len);					// max length of transfer allowed
	if(seq) SL811Write(EP2AControl,DATA1_OUT);		// armed to transmit to host, DATA1
	else SL811Write(EP2AControl,DATA0_OUT);			// armed to transmit to host, DATA0
}

//*****************************************************************************************
// Swap high and low byte
//*****************************************************************************************
/**
 * \details Swap high and low bytes
 * \param input Word value for swapping
 * \returns Swapped value
 */
static WORD WordSwap(WORD input){
	return(((input & 0x00FF) << 8)|((input & 0xFF00) >> 8));
}
/**
 * \details Enable SL811 Reset interrupt
 * \returns None
 */
static void enable_reset_interrupt(void){
	BYTE int_enabled;
	if(DisableResetInterruptCountdown == 0){
		int_enabled = SL811Read(IntEna);
		int_enabled = int_enabled | 0x40;
		SL811Write(IntEna, int_enabled);
	}
	DisableResetInterruptCountdown = RESETINTSHUTDOWN;
}

/*short InBufferUsed(void){
	short returnValue;

	returnValue = InPacketCircularTail - InPacketCircularHead;
	if(returnValue < 0) returnValue += INPACKETBUFFERLEN;
	return returnValue;
}*/
/**
 * \details Returns the number of items in the out packet buffer
 * \returns Number of items
 */
short OutBufferUsed(void){
	short returnValue;

	returnValue = OutPacketCircularTail - OutPacketCircularHead;
	if(returnValue < 0) returnValue += OUTPACKETBUFFERLEN;
	return returnValue;
}

/*short InBufferFree(void){
	return INPACKETBUFFERLEN - InBufferUsed() - 1;
}*/
/**
 * \details Get the number of spaces available in the out packet buffer
 * \returns Spaces available
 */
short OutBufferFree(void){
	return OUTPACKETBUFFERLEN - OutBufferUsed() - 1;
}

/*char InBufferPeep(short BufferIndex){
	short relativeIndex;

	relativeIndex = InPacketCircularHead + BufferIndex;

	while(relativeIndex >= INPACKETBUFFERLEN) relativeIndex -= INPACKETBUFFERLEN;
	return InPacketCircularBuffer[relativeIndex];
}*/

/*char OutBufferPeep(short BufferIndex){
	short relativeIndex;

	relativeIndex = OutPacketCircularHead + BufferIndex;

	while(relativeIndex >= OUTPACKETBUFFERLEN) relativeIndex -= OUTPACKETBUFFERLEN;
	return OutPacketCircularBuffer[relativeIndex];
}*/

/*short InBufferScan(char testFor){
	short i, returnValue;

	if(InBufferUsed() != 0){
		returnValue = -1;
		for(i=0; i<InBufferUsed(); i++){
			if(InBufferPeep(i) == testFor){
				returnValue = i;
				break;
			}
}

	}else{
		returnValue = -1;
	}

	return returnValue;
}*/

/*short OutBufferScan(char testFor){
	short i, returnValue;

	if(OutBufferUsed() != 0){
		returnValue = -1;
		for(i=0; i<OutBufferUsed(); i++){
			if(OutBufferPeep(i) == testFor){
				returnValue = i;
				break;
			}
		}
	}else{
		returnValue = -1;
	}

	return returnValue;
}*/

/*void InBufferStrip(char testFor){
	short location;
	char dump[INPACKETBUFFERLEN + 1];

	location = InBufferScan(testFor);
	if(location > 0){
		InBufferPop(dump, location);
	}
}*/

/*void OutBufferStrip(char testFor){
	short location;
	char dump[OUTPACKETBUFFERLEN + 1];

	location = OutBufferScan(testFor);
	if(location > 0){
		OutBufferPop(dump, location);
	}
}*/
//==========================================================================================================================================================

// Returns 1 if successful, Returns 0 if unsuccessful because there is no data in the buffer
/**
 * \details Move data from InPacket into the Input_Array
 * \param Input_Array Pointer to byte array, which receives the data
 * \returns None
 */
void USB_Get_Data(uchar Input_Array[]){
	short i;

    for (i = 0; i < EP2_LEN; i++) Input_Array[i] = InPacket[i];
    valid_inpacket = FALSE;
}

// Returns TRUE if successful in loading the output buffer
// Returns FALSE if unsuccessful because it has not sent the previous packet
/**
 * \details Load the output buffer in the SL811 with a Null terminated string
 * \param Output_Array Pointer to Null terminated string for output
 * \returns True = Success, False = Previous packet has not been sent and output buffer has not been loaded
 */
bool USB_Send_Data(uchar Output_Array[]){
    short i, len;

    if(current.usb_device_protocol == 0){
		if(clear_to_send1){
			clear_to_send1 = FALSE;
			len = strlen((char *)Output_Array);
			for(i = 0; i < len; i++) OutPacket[i] = Output_Array[i];
			for(i = len; i < EP1_LEN; i++) OutPacket[i] = '\0';
			clear_to_send2 = FALSE;
			return TRUE;
		}else{
			return FALSE;
		}
    }else if(current.usb_device_protocol == 1){
    	len = strlen((char *)Output_Array);
    	if(len > OutBufferFree()){
    		return FALSE;
    	}else{
    		OutBufferPush((char *) Output_Array, len);
    		return TRUE;
    	}
    }else{
    	return TRUE;
    }
}

/**
 * \details Load the output buffer in the SL811 with data
 * \param Output_Array Pointer to data for output
 * \param num Number of bytes for output
 * \returns True = Success, False = Previous packet has not been sent and output buffer has not been loaded
 */
bool USB_Send_Data_num(uchar Output_Array[], short num){
    short i;
    short len;

    if(current.usb_device_protocol == 0){
		if(clear_to_send1){
			clear_to_send1 = FALSE;
			len = num;
			for(i=0; i<len; i++) OutPacket[i] = Output_Array[i];
			for(i=len; i<EP1_LEN; i++) OutPacket[i] = '\0';
			OutPacket[63] = running_counter++;
			clear_to_send2 = FALSE;
			return TRUE;
		}else{
			return FALSE;
		}
    }else if(current.usb_device_protocol == 1){
    	if(num > OutBufferFree()){
    		return FALSE;
    	}else{
    		OutBufferPush((char *) Output_Array, num);
    		return TRUE;
    	}
    }else{
    	return TRUE;
    }
}
/**
 * \details Load the output buffer in the SL811 with data and append a checksum at the end of the ENDPOINT buffer
 * \param Output_Array Pointer to data for output
 * \param num Numbe of bytes for output
 * \returns True = Success, False = Previous packet has not been sent and output buffer has not been loader
 */
bool USB_Send_Data_LowLevel_num(uchar Output_Array[],short num){
	unsigned char chksum;
    short i, len;

    if(current.usb_device_protocol == 0){
		if(clear_to_send1){
			clear_to_send1 = FALSE;
			len = num;
			chksum = 0;

			for(i=0; i<len; i++){
				OutPacket[i] = Output_Array[i];
				chksum += Output_Array[i];
			}
			for(i=len; i<EP1_LEN; i++) OutPacket[i] = 0;
			OutPacket[EP1_LEN - 1] = (~chksum) + 1;
			clear_to_send2 = FALSE;
			return TRUE;
		}else{
			return FALSE;
		}
    }else if(current.usb_device_protocol == 1){
    	return TRUE;
    }else{
    	return TRUE;
    }
}
/**
 * \details Test if an inpacket is waiting
 * \returns True = Pending in packet, False = No pending in packet
 */
bool is_usb_char_waiting(void){
	service_watchdog();
	if(valid_inpacket) return TRUE;
	else return FALSE;
}

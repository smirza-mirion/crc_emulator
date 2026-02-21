/**
 * \file
 * \details This file contains function, which calls the Cypress USB Chip (SL811) as host and implements communications with USB Printers or USB Flash Drive
 */
// Project      : Capintec Calibrator USB Printer driver
// File         : sl811h.c
// Created By   : Winston Lam
// Last Update  : 7/16/05 -- Created Timed Polled Code
//                7/19/05 -- Added Setup packet after the Setup Configuration
//                7/20/05 -- Fixed printer problem, Added HotPlug detection, Added Out of Paper detection, Added Printer Selected detection, Added Printer Error detection
//                11/02/05 -- Add NAK detection code
//                12/2/05 -- Add Enumeration Code in Polled Mode
//				   1/5/06 -- Added Keep Alive Signal
//                04/10/06 -- Added timeout to usb_write, to prevent a lockup on 'OUT OF PAPER' or 'PRINTER OFF'
//                 04/28/06 -- changed timeout to accomodate HP needing more than 1 sec
//                 05/22/06 -- changed timeout variables to longs
//		  10/15/08 -- added read packet size from enumeration for USB 2.0 printers
#include <string.h>
#include <stdlib.h>
#include "crc.h"
#include "coldfire.h"
#include "pit.h"
#include "keyboard.h"   //for service_watchdog
#include "ff.h"
#include "sl811h_int.h"

#define USB_NAK_TIMEOUT 200 // 200 * 1 centisec = 2sec

extern ushort g_Init_USB_PCL;
//
extern volatile uchar *pCS1;

void USBClearNAKCounters(void);

// Function prototypes for functions outside this file
void set_host_usb_power(bool on);

ushort get_minute_counter(void);       // 1 ms counter, rollover at 60000
ushort m_flgFromResetUSBBUS;
short  m_uiUsbWriteBytesRemaining;

// function prototypes
void wr811(uchar ucRegister, uchar ucValue);       // write a value into a sl811 register
uchar rd811(uchar ucRegister);                     // read a value from a sl811 register
void wr811buffer(uchar *pucData, uchar ucLength);  // write a sequence of bytes to sl811 starting a location VAL_BUFFER_OFFSET
ushort IncCirIndex(ushort uiIndex);                // Returns the next index for the circular buffer, this takes care of wrap around
ushort CirLen(void);                                // Returns the length of the data in the circular buffer

uchar VAL_BUFFER_OFFSET_B;
unsigned long m_ulTimeOut;
uchar m_ucLastPID_ENDPOINT;
uchar m_ucLastCommand;
uchar m_ucOutEndpoint;
uchar m_ucOutBufferSize;
uchar m_ucInEndpoint;
uchar m_ucInBufferSize;
uchar m_ucMaxPacketSize;
bool m_flgFirstDesc;
volatile uchar m_ucData_0_1;
volatile uchar m_ucData_0_1_B;
volatile uchar m_ucData_Out_0_1 = 0;
volatile uchar m_ucData_In_0_1 = 0;
volatile bool m_flgStallDirectionIn;
volatile uchar m_ucStallEndpoint;
volatile uchar m_ucStallLastPID_ENDPOINT;
volatile uchar m_ucStallLastCommand;
volatile uchar m_ucStallBufferLength;
volatile uchar m_ucStallData_0_1;
bool m_flgEnablePoll = FALSE;
bool m_flgFullSpeed = FALSE;
uchar m_ucDescriptor[256];
uchar m_ucDescriptorStartIndex;
uchar m_ucTotalInterfaces;
uchar m_ucConfiguration;
uchar m_ucInterfaceNum;
uchar m_ucAltInterfaceNum;
uchar m_ucAttachedDevice; // 0=Unknown, 1=Printer, 2=Flash Drive
uchar m_ucMaxLun;
volatile bool m_boolCircularBufferValid = FALSE;

// Circular Buffer Variables
volatile uchar m_ucEnumerated = 0; // 0 - Not Enumerated, 1 - Enumerated Printer, 2 - Enumerated Disk
volatile uchar m_ucCircularBuffer[CIR_BUF_SIZE];
volatile ushort m_uiHead = 0;
volatile ushort m_uiTail = 0;

volatile unsigned int m_uiNAKReplyDelay = NAK_REPLY_INSIDE_INTERRUPT;
																// 0 - Reply to NAK inside interrupt routine,
																// 1 - Counting Delay inside PIT Timer before sending Reply to NAK,
																// 2 - Reply to NAK sent from PIT Timer

volatile unsigned long m_ulNAKReplyDelayCountMilliSecs = 0;		// millisec counter inside PIT used to determine when to send NAK reply

volatile unsigned long m_ulNAKReplyDelayThresholdMilliSecs = 0; // NAK reply is sent from PIT when
																// m_ulNAKReplyDelayCountMilliSecs exceeds m_ulNAKReplyDelayThresholdMilliSecs
																// Each consecutive iteration that a NAK reply is sent from the PIT the
																// m_ulNAKReplyDelayThresholdMilliSecs is advanced by 1ms, until it reaches
																// USB_NAK_DELAY_TIMEOUT_UPPER_LIMIT. For 1000ms terminal delay, this works out to
																// be a cumulative time of 8.34 minutes

volatile unsigned long m_ulConsecutiveNAKS = 0;					// A Number of NAKs sent from inside the interrupt routine (NAK_REPLY_INSIDE_INTERRUPT)

volatile unsigned long m_ulConsecutiveNAKSPrevious = 0;			// Number of m_ulConsecutiveNAKS from the previous centisec PIT, if the current
																// m_ulConsecutiveNAKS matches the m_ulConsecutiveNAKS then no new NAKs were sent
																// between the current PIT interrupt and the previous interrupt.

volatile unsigned long m_ulConsecutiveNAKSCentiSecs = 0;		// Number of centisecs of continuous NAK activity from inside the interrupt routine (NAK_REPLY_INSIDE_INTERRUPT)

uchar m_ucSCSIReceive[512];
int m_SCSIReceiveEnd;
int m_SCSIReceiveLength;

volatile int SCSIPending = 0; // 0 = None, 1 = Read, 2 = Write
volatile bool SCSIError;
volatile unsigned char *SCSIWriteBufferBegin;
volatile unsigned char *SCSIWriteBufferPtr;
volatile unsigned long int SCSIWriteSectorAddress;
volatile unsigned short int SCSIWriteSectors;
volatile unsigned long int SCSIWriteBytes;

volatile unsigned char *SCSIReadBufferBegin;
volatile unsigned char *SCSIReadBufferPtr;
volatile unsigned long int SCSIReadInCount;
volatile unsigned long int SCSIReadSectorAddress;
volatile unsigned short int SCSIReadSectors;
volatile unsigned long int SCSIReadBytes;
static FATFS *usbFileSystemObject;

long runningtag = 0;
uchar m_ucPDT;
unsigned long int m_ulLastLogicalBlock;
unsigned long int m_ulLogicalBlockSize;
unsigned long int m_ulTransferNumOfBytes;

// Packet to get USB Device Descriptor
uchar GET_DESC_DEV [] = {
                         0x80,              // bmRequestType (h<-d, std request, device is recipient)
                         0x06,              // bRequest (GET_DESCRIPTOR)
                         0x00,              // wValueL (DESCRIPTOR INDEX)
                         0x01,              // wValueH (DESCRIPTOR TYPE = 1 (DEVICE))
                         0x00,              // wIndexL (zero)
                         0x00,              // wIndexH (zero)
                         0x40,              // wLengthL (64)
                         0x00               // wLengthH (zero)
                         };

uchar GET_DESC_CONFIG [] = {
                            0x80,
                            0x06,
                            0x00,
                            0x02,
                            0x00,
                            0x00,
                            0x09,
                            0x00
                           };

uchar GET_DESC_CONFIG_FULL [] = {
                                 0x80,
                                 0x06,
                                 0x00,
                                 0x02,
                                 0x00,
                                 0x00,
                                 0xFF,
                                 0xFF
                                };


// Packet to set USB device address to 1
uchar SET_ADDRESS_TO_1 [] = {
                             0x00,          // bmRequestType (h->d,std request,device is recipient)
                             0x05,          // bRequest (SET_ADDRESS)
                             0x01,          // wValueL  (device address)--we're setting it to ONE
                             0x00,          // wValueH  (zero)
                             0x00,          // wIndexL  (zero)
                             0x00,          // wIndexH  (zero)
                             0x00,          // wLengthL (zero)
                             0x00           // wLengthH (zero)
                            };

// Packet to set the device configuration to 1
uchar SET_CONFIGURATION [] = {
                              0x00,    // bmRequestType (h->d,std request,device is recipient)
                              0x09,    // bRequest (SET_CONFIGURATION)
                              0x01,    // wValueL  (configuration = 1)
                              0x00,    // wValueH  (zero)
                              0x00,    // wIndexL  (zero)
                              0x00,    // wIndexH  (zero)
                              0x00,    // wLengthL (zero)
                              0x00     // wLengthH (zero)
                             };

uchar SET_INTERFACE [] = {
                          0x00,    // bmRequestType (h->d,std request,device is recipient)
                          0x0B,    // bRequest (SET_INTERFACE)
                          0x00,    // wValueL  (Alt Interface Num)
                          0x00,    // wValueH  (zero)
                          0x00,    // wIndexL  (Interface Num)
                          0x00,    // wIndexH  (zero)
                          0x00,    // wLengthL (zero)
                          0x00     // wLengthH (zero)
                         };

uchar GET_DESC_CLASS [] = {
                           0xA1,
                           0x00,
                           0x00,
                           0x00,
                           0x00,
                           0x00,
                           0xF1,
                           0x03
                          };

uchar GET_DESC_MAX_LUN [] = {
                             0xA1,
                             0xFE,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x01,
                             0x00
						 	};

uchar MAGIC_CODES2 [] = {
                         0x80,
                         0x06,
                         0x00,
                         0x10,
                         0x00,
                         0x00,
                         0x12,
                         0x00
                        };

uchar GET_PORT_STATUS [] = {
                            0xA1,
                            0x01,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x01,
                            0x00
                           };

uchar CLEAR_FEATURE [] = {
						  0x02,	// Host->Dev, Endpoint
						  0x01, // Request: Clear Feature
						  0x00, // Halt
						  0x00, // Halt
						  0x00, // Bit 7 = 1-In, 0-Out, Bit 0-3=Endpoint
						  0x00,
						  0x00, // Length
						  0x00  // Length
						 };

uchar CBW [] = {
				0x55, // 0 Signature
				0x53, // 1 Signature
				0x42, // 2 Signature
				0x43, // 3 Signature
				0x00, // 4 Tag
				0x00, // 5 Tag
				0x00, // 6 Tag
				0x00, // 7 Tag
				0x00, // 8 Data Transfer Length
				0x00, // 9 Data Transfer Length
				0x00, // 10 Data Transfer Length
				0x00, // 11 Data Transfer Length
				0x00, // 12 Direction
				0x00, // 13 LUN
				0x00, // 14 Command Block Length
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 // 30 Command Block
				};

uchar SCSI_INQUIRY [] = {
						 0x12,	// INQUIRY
						 0x00,  // EVPD
						 0x00,  // Code Page
						 0x00,  // Allocation Length MSB
						 0x24,  // Allocation Length LSB
						 0x00	// Control
						};

uchar SCSI_TEST_UNIT_READY [] = {
								 0x00,	// TEST UNIT READY
								 0x00,
								 0x00,
								 0x00,
								 0x00,
								 0x00
								};

uchar SCSI_READ_CAPACITY [] = {
							   0x25,
							   0x00,
							   0x00,
							   0x00,
							   0x00,
							   0x00,
							   0x00,
							   0x00,
							   0x00,
							   0x00
							  };

uchar SCSI_READ_CAPACITY16 [] = {
							     0x9E,
							     0x10,
							     0x00,
							     0x00,
							     0x00,
							     0x00,
							     0x00,
							     0x00,
							     0x00,
							     0x00,
							     0x00,
							     0x00,
							     0x00,
							     0x0C,
							     0x00,
							     0x00
							    };

uchar SCSI_REQUEST_SENSE [] = {
							   0x03,
							   0x00,
							   0x00,
							   0x00,
							   0x12,
							   0x00
							  };

static unsigned long int m_ulTimerThreshold;

volatile STATE m_state[MAX_STATE];
volatile bool m_flgPrinterBlocked;

static void enablePrinterInterrupt(void);
static void disablePrinterInterrupt(void);
static void StartDelay(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, int centiSecs);
static void StartUsbA(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucCommand);
static void StartSetAddr(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall);
static void StartGetDesc(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar *pucData, bool First);
static void StartSetConfig(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucConfig);
static void StartSetIntface(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucInterface, uchar ucAltInterface);
static void StartScsiInquiry(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucLun);
static void StartScsiTestUnitReady(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucLun);
static void StartScsiReadCapacity(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucLun);
static void StartScsiInInt(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall);
static void StartScsiOutInt(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall);
static void StartClearFeature(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, bool In, int Endpoint);
static void StartRequestSense(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall);
static void RunPrinter(void);
void RunPrinterFore(void);

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: InitializeSL811H                                                                          */
/*       Input: None                                                                                      */
/*      Output: None                                                                                      */
/* Description: This function powers up the SL811 host chip and tries to initiate a printer               */
/*                                                                                                        */
/**********************************************************************************************************/
/**
 * \details This function powers up the SL811 host chip and tries to initiate a printer
 * \returns None
 */
void InitializeSL811H(void){
	int index;
    // Reset the SL811H chip
	for(index=0; index<MAX_STATE; index++){
		m_state[index].Phase = 0;
		m_state[index].ReturnState = 0;
		m_state[index].ReturnPhase = 0;
		m_state[index].ReturnError = 0;
	}
	m_state[MASTER].Phase = MASTER_RESET_USB_BUS;
    set_host_usb_power(TRUE);
    delay_msec(2000);
	m_flgEnablePoll = TRUE;
    // ReinitializeSL811H();
    // delay_msec(1000);
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: USBMaster                                                                                 */
/*       Input: None                                                                                      */
/*      Output: None                                                                                      */
/* Description: This function is executed by a 10 ms timer. This function sends OUT packets to endpoint 2 */
/*              It retrieves data from the circular buffer and sends it to the USB printer. If there is   */
/*              no data, it will send a packet filled with zeros                                          */
/**********************************************************************************************************/
/**
 * \details This function is executed by a 10 ms timer. This function sends OUT packets to endpoint 2. It retrieves data from the circular buffer and sends it to the USB printer. If there is no data, it will send a packet filled with zeros
 * \returns None
 */
static void USBMaster(void){
	uchar i, ucSize, ucDescriptorType, ucCurrentConfiguration;
	bool flgFoundOutEndpoint, flgFoundInEndpoint, flgCorrectInterface;

	switch(m_state[MASTER].Phase){
		case MASTER_WAIT:
			break;

		case MASTER_RESET_USB_BUS:
			disablePrinterInterrupt();
			m_ucAttachedDevice = ATTACHED_UNKNOWN;
			m_ucEnumerated = 0;
			m_flgFromResetUSBBUS = 1;
			m_uiNAKReplyDelay = NAK_REPLY_INSIDE_INTERRUPT;
			USBClearNAKCounters();

			// Clear USB-A Registers
			wr811(REG_CONTROL, 0x00);
			wr811(REG_BUFFER_ADDRESS, 0x00);
			wr811(REG_BUFFER_LENGTH, 0x00);
			wr811(REG_PID_ENDPOINT, 0x00);
			wr811(REG_DEVICE_ADDRESS, 0x00);
			wr811(REG_CONTROL_1, 0x00);
			wr811(REG_INT_ENABLE, 0x00);
			wr811(REG_INT_STATUS, 0xFF);
			wr811(REG_SOF_COUNT_LO, 0x00);
			wr811(REG_SOF_COUNT_HI, 0x00);

			// Clear USB-B Registers
			wr811(REG_CONTROL_B, 0x00);
			wr811(REG_BUFFER_ADDRESS_B, 0x00);
			wr811(REG_BUFFER_LENGTH_B, 0x00);
			wr811(REG_PID_ENDPOINT_B, 0x00);
			wr811(REG_DEVICE_ADDRESS_B, 0x00);

			/******************************************************************************************************/
			/* Test for connected device and device speed                                                         */
			/******************************************************************************************************/
			wr811(REG_SOF_COUNT_HI, 0xAE);				// Set SOF high counter, no change D+/D-, host mode

			// Reset USB Bus
			wr811(REG_CONTROL_1, USB_RESET);			// Reset USB engine, full-speed setup, suspend disable

			StartDelay(MASTER, MASTER_ENABLE_USB_BUS, MASTER_ENABLE_USB_BUS, MASTER_ENABLE_USB_BUS, 500);	// Wait for 500 ms
			break;

		case MASTER_ENABLE_USB_BUS:
			wr811(REG_CONTROL_1, USB_OPERATE_HI_SPEED_WITHOUT_SOF);										// Set to normal operation
			wr811(REG_INT_ENABLE, 0x00);
			wr811(REG_INT_STATUS, 0xFF);																	// Clear Interrupt enable status
			StartDelay(MASTER, MASTER_CHK_ATTACHED_DEVICE, MASTER_CHK_ATTACHED_DEVICE, MASTER_CHK_ATTACHED_DEVICE, 10);							// Wait for 100 ms
			break;

		case MASTER_CHK_ATTACHED_DEVICE:
			if(rd811(REG_INT_STATUS) & BIT_INT_DEVICE_STATUS){
				wr811(REG_INT_STATUS, 0xFF);			// Clear Interrupt enable status
				StartDelay(MASTER, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, 100); // Jump to beginning after 1 sec delay
			}else{
				// Checking full or low speed
				if((rd811(REG_INT_STATUS) & BIT_INT_D_PLUS_STATUS) == 0) m_flgFullSpeed = FALSE; // ** Low Speed is detected ** //
				else m_flgFullSpeed = TRUE;														 // ** Full Speed is detected ** //

				// Reset USB Bus
				wr811(REG_CONTROL_1, USB_RESET);			// Reset USB engine, full-speed setup, suspend disable

				StartDelay(MASTER, MASTER_ASK_DESC_DEV_INIT, MASTER_ASK_DESC_DEV_INIT, MASTER_ASK_DESC_DEV_INIT, 1); // Wait for 10 ms
			}
			break;

		case MASTER_ASK_DESC_DEV_INIT:
			if(m_flgFullSpeed){
				wr811(REG_SOF_COUNT_HI, 0xAE);                           // Set up host & full speed direct and SOF cnt
				wr811(REG_SOF_COUNT_LO, 0xE0);                           // SOF Counter Low = 0xE0; 1ms interval
				wr811(REG_CONTROL_1, USB_OPERATE_HI_SPEED);
			}else{
				wr811(REG_SOF_COUNT_HI, 0xEE);                            // Set up host and low speed direct and SOF cnt
				wr811(REG_SOF_COUNT_LO, 0xE0);                            // SOF Counter Low = 0xE0; 1ms interval
				wr811(REG_CONTROL_1, USB_OPERATE_LO_SPEED);
			}
			wr811(REG_PID_ENDPOINT, BIT_SOF_PID | BIT_EP0);
			wr811(REG_DEVICE_ADDRESS, 0x00);                              // set device address to zero
			wr811(REG_CONTROL, 0x01);                                     // start generate SOF or EOP

			wr811(REG_INT_STATUS, 0xFF);									// Clear Interrupt status
			m_ucMaxPacketSize = 8;
			StartGetDesc(MASTER, MASTER_RESET_USB_BUS2, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, GET_DESC_DEV, TRUE);
			break;

		case MASTER_RESET_USB_BUS2:
			m_ucMaxPacketSize = m_ucDescriptor[7];
			wr811(REG_CONTROL_1, USB_RESET);
			StartDelay(MASTER, MASTER_ISSUE_SET_USB_ADDRESS, MASTER_ISSUE_SET_USB_ADDRESS, MASTER_ISSUE_SET_USB_ADDRESS, 1); // Wait for 10 ms
			break;

		case MASTER_ISSUE_SET_USB_ADDRESS:
			if(m_flgFullSpeed){
				wr811(REG_SOF_COUNT_HI, 0xAE);                           // Set up host & full speed direct and SOF cnt
				wr811(REG_SOF_COUNT_LO, 0xE0);                           // SOF Counter Low = 0xE0; 1ms interval
				wr811(REG_CONTROL_1, USB_OPERATE_HI_SPEED);
			}else{
				wr811(REG_SOF_COUNT_HI, 0xEE);                            // Set up host and low speed direct and SOF cnt
				wr811(REG_SOF_COUNT_LO, 0xE0);                            // SOF Counter Low = 0xE0; 1ms interval
				wr811(REG_CONTROL_1, USB_OPERATE_LO_SPEED);
			}
			wr811(REG_PID_ENDPOINT, BIT_SOF_PID | BIT_EP0);
			wr811(REG_DEVICE_ADDRESS, 0x00);                              // set device address to zero
			wr811(REG_CONTROL, 0x01);                                     // start generate SOF or EOP

			wr811(REG_INT_STATUS, 0xFF);                   // Clear Interrupt status

			StartSetAddr(MASTER, MASTER_GET_DESCRIPTOR, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS);
			break;

		case MASTER_GET_DESCRIPTOR:
			StartGetDesc(MASTER, MASTER_READ_FULL_DEVICE_DESCRIPTOR, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, GET_DESC_DEV, FALSE);
			break;

		case MASTER_READ_FULL_DEVICE_DESCRIPTOR:
			StartGetDesc(MASTER, MASTER_READ_INIT_CONFIG_DESCRIPTOR, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, GET_DESC_CONFIG, FALSE);
			break;

		case MASTER_READ_INIT_CONFIG_DESCRIPTOR:
			StartGetDesc(MASTER, MASTER_READ_FULL_CONFIG_DESCRIPTOR, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, GET_DESC_CONFIG_FULL, FALSE);
			break;

		case MASTER_READ_FULL_CONFIG_DESCRIPTOR:
			// Search for Out Endpoint and buffer Size
			i = 0;
			flgFoundOutEndpoint = FALSE;
			flgFoundInEndpoint = FALSE;
			flgCorrectInterface = FALSE;
			while(i < m_ucDescriptorStartIndex){
				ucSize = m_ucDescriptor[i];
				ucDescriptorType = m_ucDescriptor[i + 1];

				switch(ucDescriptorType){
					case DESC_TYPE_CONFIGURATION:
						m_ucTotalInterfaces = m_ucDescriptor[i + 4];
						ucCurrentConfiguration = m_ucDescriptor[i + 5];
						break;

					case DESC_TYPE_INTERFACE:
						if(!flgCorrectInterface){
							if((m_ucDescriptor[i + 5] == 7 && m_ucDescriptor[i + 6] == 1) || (m_ucDescriptor[i + 5] == 0xFF && m_ucDescriptor[i + 6] == 0xFF)){
								m_ucInterfaceNum = m_ucDescriptor[i + 2];
								m_ucAltInterfaceNum = m_ucDescriptor[i + 3];
								m_ucAttachedDevice = ATTACHED_PRINTER;
								flgCorrectInterface = TRUE;
							}else if(m_ucDescriptor[i + 5] == 8 && ((m_ucDescriptor[i + 6] == 5) || (m_ucDescriptor[i + 6] == 6)) && m_ucDescriptor[i + 7] == 80){ // Mass Storage, SCSI Command Set, Bulk Only Transport
								m_ucInterfaceNum = m_ucDescriptor[i + 2];
								m_ucAltInterfaceNum = m_ucDescriptor[i + 3];
								m_ucAttachedDevice = ATTACHED_FLASH;
								flgCorrectInterface = TRUE;
							}
						}
						break;

					case DESC_TYPE_ENDPOINT:
						if(flgCorrectInterface){
							// Test for OUT Endpoint descriptor
							if(((m_ucDescriptor[i + 2]) & 0x80) == 0x00){
								if(flgFoundOutEndpoint == FALSE){
									if((m_ucDescriptor[i + 3] & 3) == 2){ // Bulk Endpoint
										flgFoundOutEndpoint = TRUE;
										m_ucConfiguration = ucCurrentConfiguration;
										m_ucOutEndpoint = ((m_ucDescriptor[i + 2]) & 0x7F);
										m_ucOutBufferSize = m_ucDescriptor[i + 4];
									}else{								   // Not Bulk Endpoint
										flgCorrectInterface = FALSE;		// Reset to try next interface
									}
								}
							}else{
								if(flgFoundInEndpoint == FALSE){
									if((m_ucDescriptor[i + 3] & 3) == 2){	// Bulk Endpoint
										flgFoundInEndpoint = TRUE;
										m_ucConfiguration = ucCurrentConfiguration;
										m_ucInEndpoint = ((m_ucDescriptor[i + 2]) & 0x7F);
										m_ucInBufferSize = m_ucDescriptor[i + 4];
									}else{
										flgCorrectInterface = FALSE;
									}
								}
							}
						}
						break;
				}
				i += ucSize;
			}

			if(flgFoundOutEndpoint && flgFoundInEndpoint) m_state[MASTER].Phase = MASTER_SET_CONFIGURATION;
			else m_state[MASTER].Phase = MASTER_POLL_STALL;
			break;

		case MASTER_SET_CONFIGURATION:
			StartSetConfig(MASTER, MASTER_SET_INTERFACE, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, m_ucConfiguration);
			break;

		case MASTER_SET_INTERFACE:
			if (m_ucTotalInterfaces == 1){
				if(m_ucAttachedDevice == ATTACHED_PRINTER) m_state[MASTER].Phase = MASTER_READ_PRINTER_CLASS_DESCRIPTOR;
				else m_state[MASTER].Phase = MASTER_SCSI_MAX_LUN;
			}else{
				StartSetIntface(MASTER, MASTER_READ_PRINTER_CLASS_DESCRIPTOR, MASTER_RESET_USB_BUS, MASTER_READ_PRINTER_CLASS_DESCRIPTOR, m_ucInterfaceNum, m_ucAltInterfaceNum);
			}
			break;

		case MASTER_READ_PRINTER_CLASS_DESCRIPTOR:
			StartGetDesc(MASTER, MASTER_PRINTER, MASTER_RESET_USB_BUS, MASTER_PRINTER, GET_DESC_CLASS, FALSE);
			break;

		case MASTER_PRINTER:
			m_ucEnumerated = 1;
			m_flgPrinterBlocked = TRUE;
			enablePrinterInterrupt();
			g_Init_USB_PCL = 1;
			m_state[MASTER].Phase = MASTER_WAIT;
			break;

		case MASTER_SCSI_MAX_LUN:
			StartGetDesc(MASTER, MASTER_SCSI_INQUIRY, MASTER_RESET_USB_BUS, MASTER_SCSI_MAX_LUN_STALL, GET_DESC_MAX_LUN, FALSE);
			break;

		case MASTER_SCSI_MAX_LUN_STALL:
			StartClearFeature(MASTER, MASTER_SCSI_MAX_LUN_SENSE, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, m_flgStallDirectionIn, m_ucStallEndpoint);
			break;

		case MASTER_SCSI_MAX_LUN_SENSE:
			StartRequestSense(MASTER, MASTER_SCSI_INQUIRY, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS);
			break;

		case MASTER_SCSI_INQUIRY:
			m_ucMaxLun = m_ucDescriptor[0];
			StartScsiInquiry(MASTER, MASTER_SCSI_READ_CAPACITY, MASTER_RESET_USB_BUS, MASTER_SCSI_INQUIRY_STALL, 0);
			break;

		case MASTER_SCSI_INQUIRY_STALL:
			StartClearFeature(MASTER, MASTER_SCSI_INQUIRY_SENSE, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, m_flgStallDirectionIn, m_ucStallEndpoint);
			break;

		case MASTER_SCSI_INQUIRY_SENSE:
			StartRequestSense(MASTER, MASTER_SCSI_READ_CAPACITY, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS);
			break;

		case MASTER_SCSI_READ_CAPACITY:
			StartScsiReadCapacity(MASTER, MASTER_SCSI_TEST_UNIT_READY, MASTER_RESET_USB_BUS, MASTER_SCSI_READ_CAPACITY_STALL, 0);
			break;

		case MASTER_SCSI_READ_CAPACITY_STALL:
			StartClearFeature(MASTER, MASTER_SCSI_READ_CAPACITY_SENSE, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, m_flgStallDirectionIn, m_ucStallEndpoint);
			break;

		case MASTER_SCSI_READ_CAPACITY_SENSE:
			StartRequestSense(MASTER, MASTER_SCSI_READ_CAPACITY, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS);
			break;

		case MASTER_SCSI_TEST_UNIT_READY:
			StartScsiTestUnitReady(MASTER, MASTER_SCSI, MASTER_SCSI_TEST_UNIT_READY, MASTER_SCSI_TEST_UNIT_READY_STALL, 0);
			break;

		case MASTER_SCSI_TEST_UNIT_READY_STALL:
			StartClearFeature(MASTER, MASTER_SCSI_TEST_UNIT_READY_SENSE, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS, m_flgStallDirectionIn, m_ucStallEndpoint);
			break;

		case MASTER_SCSI_TEST_UNIT_READY_SENSE:
			StartRequestSense(MASTER, MASTER_SCSI_TEST_UNIT_READY, MASTER_RESET_USB_BUS, MASTER_RESET_USB_BUS);
			break;

		case MASTER_SCSI:
			m_ucEnumerated = 2;
			SCSIPending = 0;
			SCSIError = 0;
			enablePrinterInterrupt();
			usbFileSystemObject = malloc(sizeof(FATFS));
			f_mount(1, usbFileSystemObject);
			m_state[MASTER].Phase = MASTER_WAIT;
			break;

		case MASTER_POLL_STALL:
			if(rd811(REG_INT_STATUS) & BIT_INT_DEVICE_STATUS){
				disablePrinterInterrupt();
				SCSIPending = 0;
				m_ucEnumerated = 0;
				m_uiNAKReplyDelay = NAK_REPLY_INSIDE_INTERRUPT;
				USBClearNAKCounters();
				m_state[MASTER].Phase = MASTER_RESET_USB_BUS;
			}
			break;

		default:
			break;
	}
}
/**
 * \details Returns the value in m_uiUsbWriteBytesRemaining
 */
short bytes_remaining(void){
	return m_uiUsbWriteBytesRemaining;
}
/**
 * \details Write a Null terminated string to the usb printer using the Outbound circular Queue
 * \param cData Pointer to Null terminated string
 * \returns None
 */
void usb_write (char *cData){
	long uiLen, uiNextIndex;
	long uiTimeoutCount, uiInitialCount, uiCurrentCount;
	bool flgStartTimeout, flgRollover;

	if(!(m_ucEnumerated == 1)){            // Only load circular buffer if device is connected
		m_uiUsbWriteBytesRemaining = strlen(cData);
		return;
	}

	uiLen = strlen(cData);
	while(uiLen > 0){
		flgStartTimeout = FALSE;
		do{
			service_watchdog();
			uiNextIndex = IncCirIndex(m_uiTail);

			if(uiNextIndex == m_uiHead){
				if((m_ucEnumerated == 1) && (m_state[RUNPRINTER].Phase == RUNPRINTER_WAIT) && (m_state[USBA].Phase == USBA_WAIT)) RunPrinterFore();

				if(!(m_ucEnumerated == 1)){
					m_uiUsbWriteBytesRemaining = uiLen;
					return;
				}

				if(flgStartTimeout){
					uiCurrentCount = get_minute_counter();

					if(flgRollover){
						if((uiCurrentCount >= uiTimeoutCount) && (uiCurrentCount < uiInitialCount)){
							m_uiUsbWriteBytesRemaining = uiLen;
							return;
						}
					}else{
						if((uiCurrentCount >= uiTimeoutCount) || (uiCurrentCount < uiInitialCount)){
							m_uiUsbWriteBytesRemaining = uiLen;
							return;
						}
					}
				}else{
					flgStartTimeout = TRUE;
					uiInitialCount = get_minute_counter();
					uiTimeoutCount = uiInitialCount + USB_WRITE_TIMEOUT;
                    if(uiTimeoutCount >= 60000){
                    	uiTimeoutCount -= 60000;
                    	flgRollover = TRUE;
                    }else{
                    	flgRollover = FALSE;
                    }
				}
			}
		}while(uiNextIndex == m_uiHead);          // Loop until circular buffer is not full

		m_ucCircularBuffer [m_uiTail] = *cData++;   // Load one character into circular buffer
		m_uiTail = IncCirIndex (m_uiTail);          // Move the tail to the next position
		uiLen--;                                    // Countdown remaining bytes left
	}
	m_uiUsbWriteBytesRemaining = 0;
	if((m_ucEnumerated == 1) && (m_state[RUNPRINTER].Phase == RUNPRINTER_WAIT) && (m_state[USBA].Phase == USBA_WAIT) && (m_uiHead != m_uiTail)) RunPrinterFore();
}

/**********************************************************************************************************
/
/    Function: usb_write_num
/       Input: string to send to the USB printer, # of characters to send
/               used for sending control strings to printer which might have NULL as a parameter
/      Output: None
/ Description: Places data into the circular buffer for the polled routine to send to the USB printer
/
**********************************************************************************************************/
/**
 * \details Places data into the circular buffer for the polled routine to send to the USB printer
 * \param cData Pointer to the beginning of the data array
 * \param num Number of bytes to send
 * \returns None
 */
void usb_write_num (char *cData, short num){
	long uiLen, uiNextIndex;
	long uiTimeoutCount, uiInitialCount, uiCurrentCount;
	bool flgStartTimeout, flgRollover;

	if(!(m_ucEnumerated == 1)){            // Only load circular buffer if device is connected
		m_uiUsbWriteBytesRemaining = num;
		return;
	}

	uiLen = num;
	while(uiLen > 0){
		flgStartTimeout = FALSE;
		do{
			service_watchdog();
			uiNextIndex = IncCirIndex(m_uiTail);

			if(uiNextIndex == m_uiHead){
				if((m_ucEnumerated == 1) && (m_state[RUNPRINTER].Phase == RUNPRINTER_WAIT) && (m_state[USBA].Phase == USBA_WAIT)) RunPrinterFore();

				if(!(m_ucEnumerated == 1)){
					m_uiUsbWriteBytesRemaining = uiLen;
					return;
				}

				if(flgStartTimeout){
					uiCurrentCount = get_minute_counter();

					if(flgRollover){
						if((uiCurrentCount >= uiTimeoutCount) && (uiCurrentCount < uiInitialCount)){
							m_uiUsbWriteBytesRemaining = uiLen;
							return;
						}
                    }else{
                    	if((uiCurrentCount >= uiTimeoutCount) || (uiCurrentCount < uiInitialCount)){
                    		m_uiUsbWriteBytesRemaining = uiLen;
                    		return;
                    	}
                    }
				}else{
					flgStartTimeout = TRUE;
					uiInitialCount = get_minute_counter();
					uiTimeoutCount = uiInitialCount + USB_WRITE_TIMEOUT;
					if(uiTimeoutCount >= 60000){
						uiTimeoutCount -= 60000;
						flgRollover = TRUE;
					}else{
						flgRollover = FALSE;
					}
				}
			}
		}while(uiNextIndex == m_uiHead);          // Loop until circular buffer is not full

		m_ucCircularBuffer [m_uiTail] = *cData++;   // Load one character into circular buffer
		m_uiTail = IncCirIndex (m_uiTail);          // Move the tail to the next position
		uiLen--;                                    // Countdown remaining bytes left
	}
	m_uiUsbWriteBytesRemaining = 0;
	if((m_ucEnumerated == 1) && (m_state[RUNPRINTER].Phase == RUNPRINTER_WAIT) && (m_state[USBA].Phase == USBA_WAIT) && (m_uiHead != m_uiTail)) RunPrinterFore();
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: wr811                                                                                     */
/*       Input: Register, Value                                                                           */
/*      Output: None                                                                                      */
/* Description: Writes a value to a sl811 register                                                        */
/*                                                                                                        */
/**********************************************************************************************************/
/**
 * \details Writes a value to a sl811 register
 * \param ucRegister SL811 register
 * \param ucValue Value to write to register
 * \returns None
 */
void wr811 (uchar ucRegister, uchar ucValue)
{
    pCS1 [USB_HOST_OFF_SET] = ucRegister;
    pCS1 [USB_HOST_OFF_RW] = ucValue;
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: rd811                                                                                     */
/*       Input: Register                                                                                  */
/*      Output: Data Value                                                                                */
/* Description: Reads data from a sl811 register                                                          */
/*                                                                                                        */
/**********************************************************************************************************/
/**
 * \details Reads data from a sl811 register
 * \param ucRegister SL811 register
 * \returns Value in the register
 */
uchar rd811 (uchar ucRegister)
{
    pCS1 [USB_HOST_OFF_SET] = ucRegister;
    return pCS1 [USB_HOST_OFF_RW];
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: wr811buffer                                                                               */
/*       Input: Pointer to data, Length of data                                                           */
/*      Output: None                                                                                      */
/* Description: Writes a string of data to consecutive locations on the sl811                             */
/*                                                                                                        */
/**********************************************************************************************************/
/**
 * \details Writes a string of data to consecutive locations on the sl811
 * \param pucData Pointer to data
 * \param ucLength Number of data bytes
 * \returns None
 */
void wr811buffer (uchar *pucData, uchar ucLength)
{
    uchar ucIndex;
    pCS1 [USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;

    if (ucLength > 0)
    {
        for (ucIndex = 0; ucIndex < ucLength; ucIndex++)
            pCS1 [USB_HOST_OFF_RW] = *pucData++;
    }
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: IncCirIndex                                                                               */
/*       Input: Index Value                                                                               */
/*      Output: Index Value                                                                               */
/* Description: Calculates the incremented index value of the circular buffer                             */
/*                                                                                                        */
/**********************************************************************************************************/
/**
 * \details Calculates the incremented index value of the circular buffer
 * \param uiIndex Current index value
 * \returns Next index value
 */
ushort IncCirIndex (ushort uiIndex)
{
    if (uiIndex == CIR_BUF_SIZE - 1)
        return 0;
    else
        return uiIndex + 1;
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: CirLen                                                                                    */
/*       Input: None                                                                                      */
/*      Output: Length of data in circular buffer                                                         */
/* Description: Calculates the length of the data in the circular buffer                                  */
/*                                                                                                        */
/**********************************************************************************************************/
/**
 * \details Calculates the length of the data in the circular buffer
 * \returns Length of data in the circular buffer
 */
ushort CirLen (void)
{
    if (m_uiTail == m_uiHead)
    {
        return 0;
    }
    else if (m_uiTail > m_uiHead)
    {
        return (m_uiTail - m_uiHead);
    }
    else
    {
        return (CIR_BUF_SIZE + m_uiTail - m_uiHead);
    }
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: IsUSBPrinterConnected                                                                     */
/*       Input: None                                                                                      */
/*      Output: Flag indicating the presence of a USB printer                                             */
/* Description: This function checks m_ucEnumerated if USB PORT has been enumerated                       */
/*                                                                                                        */
/**********************************************************************************************************/
/**
 * \details This function checks m_ucEnumerated if USB PORT has been enumerated
 * \returns True = USB Printer is connected, False = USB Printer is not connected
 */
bool IsUSBPrinterConnected (void)
{
	if(m_ucEnumerated == 1) return TRUE;
	else return FALSE;
}
/**
 * \details Convert little endian 16 bit number to a Coldfire big endian 16 bit number
 * \param start Pointer to little 16 bit endian number
 * \returns Big endian 16 bit number
 */
unsigned short int TransposeShortInt(unsigned char *start){
	unsigned short int returnValue;
	unsigned char *ptr1;
	unsigned char *ptr2;

	ptr1 = start;
	ptr2 = (unsigned char *) &returnValue;

	ptr2[1] = ptr1[0];
	ptr2[0] = ptr1[1];

	return returnValue;
}
/**
 * \details Convert little endian 32 bit number to a Coldfire big endian 32 bit number
 * \param start Pointer to little endian 32 bit number
 * \returns Big endian 32 bit number
 */
unsigned long int TransposeLongInt(unsigned char *start){
	unsigned long int returnValue;
	unsigned char *ptr1;
	unsigned char *ptr2;

	ptr1 = start;
	ptr2 = (unsigned char *) &returnValue;

	ptr2[3] = ptr1[0];
	ptr2[2] = ptr1[1];
	ptr2[1] = ptr1[2];
	ptr2[0] = ptr1[3];

	return returnValue;
}
/**
 * \details Create a CBW (Command Block Wrapper) for Mass Storage Class (USB Flash Drive)
 * \param cbw Pointer to the new CBW structure
 * \param TransferSize Transfer Size
 * \param TransferInDir Transfer Direction
 * \param CommandBlockSize Command Block Size
 * \param CommandBlock Command Block Data
 * \param LUN Logical Unit Number
 * \returns None
 */
void CreateCBW(uchar *cbw, unsigned long int TransferSize, bool TransferInDir, unsigned char CommandBlockSize, uchar *CommandBlock, unsigned char LUN){
	unsigned long int TransposedTransferSize;

	memcpy(cbw, CBW, sizeof(CBW));
	runningtag++;
	memcpy(&(cbw[4]), &runningtag, 4);
	TransposedTransferSize = TransposeLongInt((unsigned char *) &TransferSize);
	memcpy(&(cbw[8]), &TransposedTransferSize, 4);
	if(TransferInDir) cbw[12] = 0x80;
	else cbw[12] = 0x00;
	cbw[13] = LUN;
	cbw[14] = CommandBlockSize;
	memcpy(&(cbw[15]), CommandBlock, CommandBlockSize);
}
/**
 * \details Checks a CSW (Command Status Wrapper) for Mass Storage Class (USB Flash Drive)
 * \param residue Pointer to 32 bit, which receives the residue
 * \returns Status value
 */
static int CheckCSW(unsigned long int *residue){ // Bit 0 = Signature, Bit 1 = Tag, Bit 2 = Status
	int returnValue;
	long int tag;

	returnValue = 0;

	if((m_ucSCSIReceive[0] != 0x55) || (m_ucSCSIReceive[1] != 0x53) || (m_ucSCSIReceive[2] != 0x42) || (m_ucSCSIReceive[3] != 0x53)) returnValue += 1;

	memcpy(&tag, &(m_ucSCSIReceive[4]), 4);
	if(tag != runningtag) returnValue += 2;

	if(m_ucSCSIReceive[12] != 0) returnValue += 4;

	if(residue != NULL) *residue = TransposeLongInt(&(m_ucSCSIReceive[8]));
	return returnValue;
}
/**
 * \details Setup the SL811 registers
 * \param buffer Pointer to buffer on the SL811 chip
 * \param buffersize Size of the buffer on the SL811 chip
 * \returns None
 */
static void Setup811Init(uchar *buffer, int buffersize){
	wr811(REG_BUFFER_ADDRESS, VAL_BUFFER_OFFSET);
	wr811(REG_BUFFER_LENGTH, buffersize);
	wr811buffer(buffer, buffersize);
	m_ucLastPID_ENDPOINT = BIT_SETUP_PID | BIT_EP0;
	wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
	m_ucData_0_1 = 0;
}
/**
 * \details Configure the SL811 registers as an out endpoint
 * \param buffer Pointer to buffer on the SL811 chip
 * \param buffersize Size of the buffer on the SL811 chip
 * \returns None
 */
void Out811Init(uchar *buffer, int buffersize){
	wr811(REG_BUFFER_ADDRESS, VAL_BUFFER_OFFSET);
	wr811(REG_BUFFER_LENGTH, buffersize);
	wr811buffer(buffer, buffersize);
	m_ucLastPID_ENDPOINT = BIT_OUT_PID | m_ucOutEndpoint;
	wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
}
/**
 * \details Initialize the USB state variable
 * \param CurrentState State variable to set
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \returns None
 */
void Start(int CurrentState, int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall){
	m_state[ReturnState].Phase = 0;
	m_state[CurrentState].ReturnState = ReturnState;
	m_state[CurrentState].ReturnPhase = ReturnPhase;
	m_state[CurrentState].ReturnError = ReturnError;
	m_state[CurrentState].ReturnStall = ReturnStall;
}
/**
 * \details Set the phase to the ReturnState with the ReturnPhase from the Current Phase
 * \param CurrentState Current State
 * \returns None
 */
static void Return(int CurrentState){
	m_state[m_state[CurrentState].ReturnState].Phase = m_state[CurrentState].ReturnPhase;
	m_state[CurrentState].Phase = 0;
}
/**
 * \details Set the phase to the ReturnState with the ReturnError from the Current Phase
 * \returns None
 */
static void ReturnError(int CurrentState){
	m_state[m_state[CurrentState].ReturnState].Phase = m_state[CurrentState].ReturnError;
	m_state[CurrentState].Phase = 0;
}
/**
 * \details Set the phase to the ReturnState with the ReturnStall from the Current Phase
 * \returns None
 */
static void ReturnStall(int CurrentState){
	m_state[m_state[CurrentState].ReturnState].Phase = m_state[CurrentState].ReturnStall;
	m_state[CurrentState].Phase = 0;
}
/**
 * \details Set the MASTER state to MASTER_POLL_STALL
 */
static void ReturnStallMaster(int CurrentState){
	m_state[MASTER].Phase = MASTER_POLL_STALL;
	m_state[CurrentState].Phase = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Set USB TIMER State Variable
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \param centiSecs Number of 10 ms
 * \returns None
 */
static void StartDelay(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, int centiSecs){
	Start(TIMER, ReturnState, ReturnPhase, ReturnError, ReturnStall);
	m_ulTimerThreshold = g_csec_tstamp + centiSecs;
	m_state[TIMER].Phase = TIMER_WAIT_FOR_TIMER;
}
/**
 * \details Do nothing until m_ulTimerThreshold times out, then Execute the Return State
 * \returns None
 */
static void USBDelay(void){
	switch(m_state[TIMER].Phase){
		case TIMER_WAIT:
			break;

		case TIMER_WAIT_FOR_TIMER:
			if(g_csec_tstamp > m_ulTimerThreshold){
				Return(TIMER);
			}
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Setup USBA state variable and SL811 registers
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \param ucCommand Command to the SL811 Chip
 * \returns None
 */
static void StartUsbA(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucCommand){
	Start(USBA, ReturnState, ReturnPhase, ReturnError, ReturnStall);
	m_ucLastCommand = ucCommand;
	m_ulTimeOut = g_csec_tstamp + WAIT_FOR_INTA_TIMEOUT;
	m_state[USBA].Phase = USBA_WAIT_FOR_USBA;

	wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);                   // clear the DONE bit
	if((m_ucLastPID_ENDPOINT & 0x0F) == 0){
		if (m_ucData_0_1){
			m_ucData_0_1 = 0;
			wr811 (REG_CONTROL, 0x40 | ucCommand);                     // start the operation
		}else{
			m_ucData_0_1 = 1;
			wr811 (REG_CONTROL, 0xBF & ucCommand);                     // start the operation
		}
	}else{
		if((m_ucLastPID_ENDPOINT & 0xF0) == 0x10){
			if (m_ucData_Out_0_1){
				m_ucData_Out_0_1 = 0;
				wr811 (REG_CONTROL, 0x40 | ucCommand);                     // start the operation
			}else{
				m_ucData_Out_0_1 = 1;
				wr811 (REG_CONTROL, 0xBF & ucCommand);                     // start the operation
			}
		}
		if((m_ucLastPID_ENDPOINT & 0xF0) == 0x90){
			if (m_ucData_In_0_1){
				m_ucData_In_0_1 = 0;
				wr811 (REG_CONTROL, 0x40 | ucCommand);                     // start the operation
			}else{
				m_ucData_In_0_1 = 1;
				wr811 (REG_CONTROL, 0xBF & ucCommand);                     // start the operation
			}
		}
	}
}
/**
 * \details Clear all the NAK timeout counters
 * \returns None
 */
void USBClearNAKCounters(void){
	m_ulNAKReplyDelayCountMilliSecs = 0;
	m_ulNAKReplyDelayThresholdMilliSecs = 0;
	m_ulConsecutiveNAKS = 0;
	m_ulConsecutiveNAKSPrevious = 0;
	m_ulConsecutiveNAKSCentiSecs = 0;
}
/**
 * \details Send a reply by setting the registers on the SL811 for a NAK packet
 * \returns None
 */
void USBUsbAReplyToNAK(void){
	if((m_ucLastPID_ENDPOINT & 0x0F) == 0){
		if(m_ucData_0_1) wr811 (REG_CONTROL, 0xBF & m_ucLastCommand);
		else wr811 (REG_CONTROL, 0x40 | m_ucLastCommand);
	}else{
		if((m_ucLastPID_ENDPOINT & 0xF0) == 0x10){
			if(m_ucData_Out_0_1) wr811 (REG_CONTROL, 0xBF & m_ucLastCommand);
			else wr811 (REG_CONTROL, 0x40 | m_ucLastCommand);
		}

		if((m_ucLastPID_ENDPOINT & 0xF0) == 0x90){
			if(m_ucData_In_0_1) wr811 (REG_CONTROL, 0xBF & m_ucLastCommand);
			else wr811 (REG_CONTROL, 0x40 | m_ucLastCommand);
		}
	}
	m_ulTimeOut = g_csec_tstamp + WAIT_FOR_INTA_TIMEOUT; //Reset timeout
}
/**
 * \details Respond to the SL811 chip with the state variable
 * \returns None
 */
static void USBUsbA(void){
	uchar ucStatus;

	switch(m_state[USBA].Phase){
		case USBA_WAIT:
			break;

		case USBA_WAIT_FOR_USBA:
			if((rd811(REG_INT_STATUS) & BIT_INT_USB_A_STATUS) == BIT_INT_USB_A_STATUS){
				// Test for NAK
				ucStatus = rd811(REG_PACKET_STATUS);
				if((ucStatus & BIT_STATUS_NAK) == BIT_STATUS_NAK){
					wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
					wr811(REG_INT_STATUS, BIT_INT_USB_A_STATUS);   // Clear the DONE bit

					if(m_ucEnumerated == 1){
						if(m_uiNAKReplyDelay == NAK_REPLY_INSIDE_INTERRUPT){
							if(m_ulConsecutiveNAKSCentiSecs > USB_NAK_TIMEOUT){
								m_uiNAKReplyDelay = NAK_DELAY_REPLY_INSIDE_PIT;	// NAK reply is delayed in the PIT Routine by m_ulNAKReplyDelayThresholdMilliSecs
								USBClearNAKCounters();
							}else{
								USBUsbAReplyToNAK();
								m_ulConsecutiveNAKS++;
							}
						}else if(m_uiNAKReplyDelay == NAK_REPLY_SENT_FROM_PIT){
							// Received a NAK from a NAK reply sent from PIT
							m_uiNAKReplyDelay = NAK_DELAY_REPLY_INSIDE_PIT;	// NAK reply is delayed in the PIT Routine by m_ulNAKReplyDelayThresholdMilliSecs
							m_ulNAKReplyDelayCountMilliSecs = 0;
							m_ulConsecutiveNAKS = 0;
							m_ulConsecutiveNAKSPrevious = 0;
							m_ulConsecutiveNAKSCentiSecs = 0;
						}
					}else{
						m_uiNAKReplyDelay = NAK_REPLY_INSIDE_INTERRUPT;
						USBClearNAKCounters();
						USBUsbAReplyToNAK();
					}
				}else if((ucStatus & BIT_STATUS_ERROR) == BIT_STATUS_ERROR){
					//printf("Error\n");
					m_uiNAKReplyDelay = NAK_REPLY_INSIDE_INTERRUPT;
					USBClearNAKCounters();
					wr811(REG_INT_STATUS, BIT_INT_USB_A_STATUS);
					ReturnError(USBA);
				}else if((ucStatus & BIT_STATUS_TIMEOUT) == BIT_STATUS_TIMEOUT){
					//printf("Timeout\n");
					m_uiNAKReplyDelay = NAK_REPLY_INSIDE_INTERRUPT;
					USBClearNAKCounters();
					wr811(REG_INT_STATUS, BIT_INT_USB_A_STATUS);
					ReturnError(USBA);
				}else if((ucStatus & BIT_STATUS_OVERFLOW) == BIT_STATUS_OVERFLOW){
					m_uiNAKReplyDelay = NAK_REPLY_INSIDE_INTERRUPT;
					USBClearNAKCounters();
					if((ucStatus & BIT_STATUS_ACK) == BIT_STATUS_ACK){
						//printf("Overflow & Ack\n");
						//wr811(REG_INT_STATUS, BIT_INT_USB_A_STATUS);
						Return(USBA);
					}else{
						//printf("Overflow\n");
						wr811(REG_INT_STATUS, BIT_INT_USB_A_STATUS);
						ReturnError(USBA);
					}
				}else if((ucStatus & BIT_STATUS_STALL) == BIT_STATUS_STALL){
					m_uiNAKReplyDelay = NAK_REPLY_INSIDE_INTERRUPT;
					USBClearNAKCounters();
					if((m_ucLastPID_ENDPOINT & 0xF0) == 0x90) m_flgStallDirectionIn = TRUE;
					else m_flgStallDirectionIn = FALSE;
					m_ucStallEndpoint = m_ucLastPID_ENDPOINT & 0x0F;
					m_ucStallLastCommand = m_ucLastCommand;
					m_ucStallLastPID_ENDPOINT = m_ucLastPID_ENDPOINT;
					m_ucStallBufferLength = rd811(REG_BUFFER_LENGTH);
					m_ucStallData_0_1 = m_ucData_In_0_1;
					//printf("Stall\n");
					wr811(REG_INT_STATUS, BIT_INT_USB_A_STATUS);
					ReturnStall(USBA);
				}else if((ucStatus & BIT_STATUS_ACK) == BIT_STATUS_ACK){
					m_uiNAKReplyDelay = NAK_REPLY_INSIDE_INTERRUPT;
					USBClearNAKCounters();
					Return(USBA);
				}
			}else{
				// Test for timeout
				if(g_csec_tstamp > m_ulTimeOut){
					//printf("Timeout\n");
					ReturnError(USBA);
				}
			}
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start Set Address USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \returns None
 */
static void StartSetAddr(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall){
	Start(SETADDR, ReturnState, ReturnPhase, ReturnError, ReturnStall);

	wr811(REG_INT_STATUS, 0xFF);                        // Clear Interrupt status
	Setup811Init(SET_ADDRESS_TO_1, sizeof(SET_ADDRESS_TO_1));
	StartUsbA(SETADDR, SETADDR_IN, SETADDR_ERROR, SETADDR_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}
/**
 * \details Response to Set Address USB Process
 * \returns None
 */
static void USBSetAddr(void){
	switch(m_state[SETADDR].Phase){
		case SETADDR_WAIT:
			break;

		case SETADDR_ERROR:
			ReturnError(SETADDR);
			break;

		case SETADDR_STALL:
			ReturnStall(SETADDR);
			break;

		case SETADDR_IN:
			m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
			wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
			m_ucData_0_1 = 0;
			StartUsbA(SETADDR, SETADDR_FINISHED, SETADDR_ERROR, SETADDR_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
			break;

		case SETADDR_FINISHED:
			wr811(REG_DEVICE_ADDRESS, 0x01); // Set USB Address to 1
			wr811(REG_DEVICE_ADDRESS_B, 0x01);
			Return(SETADDR);
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start Get Description USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \param pucData Pointer to 8 byte array of the description to get
 * \param First True = First description, False = Not first description
 * \returns None
 */
static void StartGetDesc(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar *pucData, bool First){
	m_flgFirstDesc = First;
	Start(GETDESC, ReturnState, ReturnPhase, ReturnError, ReturnStall);

	Setup811Init(pucData, 8);
	StartUsbA(GETDESC, GETDESC_INIT_IN, GETDESC_ERROR, GETDESC_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}
/**
 * \details Response to Get Description USB Process
 * \returns None
 */
static void USBGetDesc(void){
	uchar index, ucTransferCount;
	switch(m_state[GETDESC].Phase){
		case GETDESC_WAIT:
			break;

		case GETDESC_ERROR:
			ReturnError(GETDESC);
			break;

		case GETDESC_STALL:
			ReturnStall(GETDESC);
			break;

		case GETDESC_INIT_IN:
			m_ucDescriptorStartIndex = 0;

			wr811(REG_BUFFER_LENGTH, m_ucMaxPacketSize);
			m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
			wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
			m_ucData_0_1 = 0;

			StartUsbA(GETDESC, GETDESC_IN, GETDESC_ERROR, GETDESC_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
			break;

		case GETDESC_IN:
			ucTransferCount = rd811(REG_BUFFER_LENGTH) - rd811(REG_TRANSFER_COUNT);

			// Read the size of the descriptor
			if(ucTransferCount > 0){
				pCS1[USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
				for(index=0; index<ucTransferCount; index++){
					m_ucDescriptor[m_ucDescriptorStartIndex++] = pCS1[USB_HOST_OFF_RW];
					//m_ucDescriptor[m_ucDescriptorStartIndex] = rd811(VAL_BUFFER_OFFSET + index);
					//m_ucDescriptorStartIndex++;
				}
			}

//			printf("Transfer Count: %u, DescriptorStartIndex: %u\n", ucTransferCount, m_ucDescriptorStartIndex);

			if((ucTransferCount == 0) || (ucTransferCount<m_ucMaxPacketSize) || m_flgFirstDesc){
				// Send Final OUT PID
				wr811 (REG_BUFFER_LENGTH, 0);
				m_ucLastPID_ENDPOINT = BIT_OUT_PID | BIT_EP0;
				wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				m_ucData_0_1 = 1;
				StartUsbA(GETDESC, GETDESC_FINISHED, GETDESC_ERROR, GETDESC_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
			}else{
				wr811 (REG_BUFFER_LENGTH, m_ucMaxPacketSize);
				m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
				wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				StartUsbA(GETDESC, GETDESC_IN, GETDESC_ERROR, GETDESC_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
			}
			break;

		case GETDESC_FINISHED:
			Return(GETDESC);
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start Set Config USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \param ucConfig USB Configuration
 * \returns None
 */
static void StartSetConfig(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucConfig){
	uchar ucSetConfig[8];

	Start(SETCONFIG, ReturnState, ReturnPhase, ReturnError, ReturnStall);

	memcpy(ucSetConfig, SET_CONFIGURATION, sizeof(SET_CONFIGURATION));
	ucSetConfig[2] = ucConfig;
	Setup811Init(ucSetConfig, sizeof(ucSetConfig));
	StartUsbA(SETCONFIG, SETCONFIG_IN, SETCONFIG_ERROR, SETCONFIG_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}
/**
 * \details Response to Set Config USB Process
 * \returns None
 */
static void USBSetConfig(void){
	switch(m_state[SETCONFIG].Phase){
		case SETCONFIG_WAIT:
			break;

		case SETCONFIG_ERROR:
			ReturnError(SETCONFIG);
			break;

		case SETCONFIG_STALL:
			ReturnStall(SETCONFIG);
			break;

		case SETCONFIG_IN:
			wr811(REG_BUFFER_LENGTH, 8);
			m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
			wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
			m_ucData_0_1 = 0;
			StartUsbA(SETCONFIG, SETCONFIG_FINISHED, SETCONFIG_ERROR, SETCONFIG_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
			break;

		case SETCONFIG_FINISHED:
			m_ucData_0_1 = 0;
			m_ucData_Out_0_1 = 0;
			m_ucData_In_0_1 = 0;
			Return(SETCONFIG);
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start Set Interface USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \param ucInterface Interface number
 * \param ucAltInterface Alt Interface number
 * \returns None
 */
static void StartSetIntface(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucInterface, uchar ucAltInterface){
	Start(SETINTFACE, ReturnState, ReturnPhase, ReturnError, ReturnStall);

	SET_INTERFACE[2] = ucAltInterface;
	SET_INTERFACE[4] = ucInterface;
	Setup811Init(SET_INTERFACE, sizeof(SET_INTERFACE));
	StartUsbA(SETINTFACE, SETINTFACE_IN, SETINTFACE_ERROR, SETINTFACE_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}
/**
 * \details Response to Set Interface USB Process
 * \returns None
 */
static void USBSetIntface(void){
	switch(m_state[SETINTFACE].Phase){
		case SETINTFACE_WAIT:
			break;

		case SETINTFACE_ERROR:
			ReturnError(SETINTFACE);
			break;

		case SETINTFACE_STALL:
			ReturnStall(SETINTFACE);
			break;

		case SETINTFACE_IN:
			wr811(REG_BUFFER_LENGTH, 8);
			m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
			wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
			m_ucData_0_1 = 0;
			StartUsbA(SETINTFACE, SETINTFACE_FINISH, SETINTFACE_ERROR, SETINTFACE_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
			break;

		case SETINTFACE_FINISH:
			Return(SETINTFACE);
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*static void StartRunPrinter(int ReturnState, int ReturnPhase, int ReturnError){
	Start(RUNPRINTER, ReturnState, ReturnPhase, ReturnError);

	m_state[RUNPRINTER].Phase = RUNPRINTER_INIT;
}

static void USBRunPrinter(void){
	uchar ucIndex;
	static ushort s_uiKeepAliveCountDown;

	switch(m_state[RUNPRINTER].Phase){
		case RUNPRINTER_WAIT:
			break;

		case RUNPRINTER_ERROR:
			ReturnError(RUNPRINTER);
			break;

		case RUNPRINTER_INIT:
			if(m_flgFromResetUSBBUS == 1){
				g_Init_USB_PCL = 1;
				m_flgFromResetUSBBUS = 0;
			}
			m_state[RUNPRINTER].Phase = RUNPRINTER_SEND;
			break;

		case RUNPRINTER_SEND:
			if(rd811(REG_INT_STATUS) & BIT_INT_DEVICE_STATUS){
				m_flgEnumerated = FALSE;
				m_state[m_state[RUNPRINTER].ReturnState].Phase = m_state[RUNPRINTER].ReturnError;
				m_state[RUNPRINTER].Phase = RUNPRINTER_WAIT;
			}else{
				if(m_uiHead == m_uiTail){
					// Keep Alive Signal
					if(s_uiKeepAliveCountDown < 1) s_uiKeepAliveCountDown = KEEP_ALIVE_COUNT; //removed sending keep alive -- made Epson hang up
					else s_uiKeepAliveCountDown--;
				}else{
					if(CirLen() >= m_ucOutBufferSize) {
						wr811(REG_BUFFER_LENGTH, m_ucOutBufferSize);
						pCS1[USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
						for(ucIndex=0; ucIndex<m_ucOutBufferSize; ucIndex++){
							if (m_uiHead != m_uiTail){
								pCS1[USB_HOST_OFF_RW] = m_ucCircularBuffer[m_uiHead];
								m_uiHead = IncCirIndex(m_uiHead);
							}else{
								pCS1[USB_HOST_OFF_RW] = 0;
							}
						}
					}else{
						wr811(REG_BUFFER_LENGTH, CirLen());
						pCS1[USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
						while(m_uiHead != m_uiTail){
							pCS1[USB_HOST_OFF_RW] = m_ucCircularBuffer[m_uiHead];
							m_uiHead = IncCirIndex(m_uiHead);
						}
					}
					m_ucLastPID_ENDPOINT = BIT_OUT_PID | m_ucOutEndpoint;
					wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);          // Write an out packet to endpoint 2
					s_uiKeepAliveCountDown = KEEP_ALIVE_COUNT;
					StartUsbA(RUNPRINTER, RUNPRINTER_SEND, RUNPRINTER_ERROR, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
				}
			}
			break;
	}
}*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start SCSI In USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \param length Size of inbound packet
 * \returns None
 */
static void StartScsiIn(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, int length){
	Start(SCSIIN, ReturnState, ReturnPhase, ReturnError, ReturnStall);
	m_SCSIReceiveEnd = 0;
	m_SCSIReceiveLength = length;
	if(m_SCSIReceiveLength > m_ucInBufferSize) wr811(REG_BUFFER_LENGTH, m_ucInBufferSize);
	else wr811(REG_BUFFER_LENGTH, m_SCSIReceiveLength);
	m_ucLastPID_ENDPOINT = BIT_IN_PID | m_ucInEndpoint;
	wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
	StartUsbA(SCSIIN, SCSIIN_IN, SCSIIN_ERROR, SCSIIN_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
}
/**
 * \details Response to SCSI In USB process
 * \returns None
 */
static void USBScsiIn(void){
	uchar index, ucTransferCount;

	switch(m_state[SCSIIN].Phase){
		case SCSIIN_WAIT:
			break;

		case SCSIIN_ERROR:
			ReturnError(SCSIIN);
			break;

		case SCSIIN_STALL:
			ReturnStall(SCSIIN);
			break;

		case SCSIIN_IN:
			ucTransferCount = rd811(REG_BUFFER_LENGTH) - rd811(REG_TRANSFER_COUNT);
			if(ucTransferCount > 0){
				pCS1[USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
				for(index=0; index<ucTransferCount; index++) {
					if(m_SCSIReceiveEnd < 1024){
						m_ucSCSIReceive[m_SCSIReceiveEnd++] = pCS1[USB_HOST_OFF_RW];
						//m_ucSCSIReceive[m_SCSIReceiveEnd++] = rd811(VAL_BUFFER_OFFSET + index);
					}
					else m_SCSIReceiveEnd++;
				}
			}

			if(m_SCSIReceiveEnd < m_SCSIReceiveLength){
				if(m_SCSIReceiveLength > m_ucInBufferSize) wr811(REG_BUFFER_LENGTH, m_ucInBufferSize);
				else wr811(REG_BUFFER_LENGTH, m_SCSIReceiveLength);
				m_ucLastPID_ENDPOINT = BIT_IN_PID | m_ucInEndpoint;
				wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				StartUsbA(SCSIIN, SCSIIN_IN, SCSIIN_ERROR, SCSIIN_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
			}else{
				Return(SCSIIN);
			}
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*static void StartScsiOut(int ReturnState, int ReturnPhase, int ReturnError, int length){
	Start(SCSIOUT, ReturnState, ReturnPhase, ReturnError);
	m_SCSITransmitStart = 0;
	m_SCSITransmitLength = length;
	if(m_SCSITransmitLength > m_ucOutBufferSize){
		wr811(REG_BUFFER_LENGTH, m_ucOutBufferSize);
		wr811buffer(&(m_ucSCSITransmit[m_SCSITransmitStart]), m_ucOutBufferSize);
		m_SCSITransmitStart += m_ucOutBufferSize;

	}else{
		wr811(REG_BUFFER_LENGTH, m_SCSITransmitLength);
		wr811buffer(&(m_ucSCSITransmit[m_SCSITransmitStart]), m_SCSITransmitLength);
		m_SCSITransmitStart += m_SCSITransmitLength;
	}
	m_ucLastPID_ENDPOINT = BIT_OUT_PID | m_ucOutEndpoint;
	wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
	StartUsbA(SCSIOUT, SCSIOUT_OUT, SCSIOUT_ERROR, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}

static void USBScsiOut(void){
	int remaining;

	switch(m_state[SCSIOUT].Phase){
		case SCSIOUT_WAIT:
			break;

		case SCSIOUT_ERROR:
			ReturnError(SCSIOUT);
			break;

		case SCSIOUT_OUT:
			if(m_SCSITransmitStart < m_SCSITransmitLength){
				remaining = m_SCSITransmitLength - m_SCSITransmitStart;
				if(remaining > m_ucOutBufferSize){
					wr811(REG_BUFFER_LENGTH, m_ucOutBufferSize);
					wr811buffer(&(m_ucSCSITransmit[m_SCSITransmitStart]), m_ucOutBufferSize);
					m_SCSITransmitStart += m_ucOutBufferSize;
				}else{
					wr811(REG_BUFFER_LENGTH, remaining);
					wr811buffer(&(m_ucSCSITransmit[m_SCSITransmitStart]), remaining);
					m_SCSITransmitStart += remaining;
				}
				m_ucLastPID_ENDPOINT = BIT_OUT_PID | m_ucOutEndpoint;
				wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				StartUsbA(SCSIOUT, SCSIOUT_OUT, SCSIOUT_ERROR, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
			}else{
				Return(SCSIOUT);
			}
			break;
	}
}*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start SCSI Inquiry USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \param ucLun Logical Unit Number
 * \returns None
 */
static void StartScsiInquiry(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucLun){
	uchar cbw[31];

	Start(SCSIINQUIRY, ReturnState, ReturnPhase, ReturnError, ReturnStall);
	CreateCBW(cbw, 36, TRUE, sizeof(SCSI_INQUIRY), SCSI_INQUIRY, 0);
	Out811Init(cbw, sizeof(cbw));
	StartUsbA(SCSIINQUIRY, SCSIINQUIRY_IN, SCSIINQUIRY_ERROR, SCSIINQUIRY_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}
/**
 * \details Response to SCSI Inquiry USB process
 * \returns None
 */
static void USBScsiInquiry(void){
	unsigned long int residue;
	bool flgStatusOK;

	switch(m_state[SCSIINQUIRY].Phase){
		case SCSIINQUIRY_WAIT:
			break;

		case SCSIINQUIRY_ERROR:
			ReturnError(SCSIINQUIRY);
			break;

		case SCSIINQUIRY_STALL:
			ReturnStall(SCSIINQUIRY);
			break;

		case SCSIINQUIRY_IN:
			StartScsiIn(SCSIINQUIRY, SCSIINQUIRY_CSW, SCSIINQUIRY_ERROR, SCSIINQUIRY_STALL, 36);
			break;

		case SCSIINQUIRY_CSW:
			m_ucPDT = m_ucSCSIReceive[0] & 0x0F; // Get Device Type
			StartScsiIn(SCSIINQUIRY, SCSIINQUIRY_FINISH, SCSIINQUIRY_ERROR, SCSIINQUIRY_STALL, 13);
			break;

		case SCSIINQUIRY_FINISH:
			if(CheckCSW(&residue) == 0){
				if(residue == 0) flgStatusOK = TRUE;
				else flgStatusOK = FALSE;
			}else flgStatusOK = FALSE;

			if(flgStatusOK){
				if(m_ucPDT == 0) Return(SCSIINQUIRY);
				else ReturnStallMaster(SCSIINQUIRY);
			}else ReturnError(SCSIINQUIRY);
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start SCSI Test Unit Ready USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \param ucLun Logical Unit Number
 * \returns None
 */
static void StartScsiTestUnitReady(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucLun){
	uchar cbw[31];

	Start(SCSITESTUNITREADY, ReturnState, ReturnPhase, ReturnError, ReturnStall);
	CreateCBW(cbw, 0, FALSE, sizeof(SCSI_TEST_UNIT_READY), SCSI_TEST_UNIT_READY, 0);
	Out811Init(cbw, sizeof(cbw));
	//m_ucSCSIReceive[0] = 0;
	StartUsbA(SCSITESTUNITREADY, SCSITESTUNITREADY_CSW, SCSITESTUNITREADY_ERROR, SCSITESTUNITREADY_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}
/**
 * \details Response to SCSI Test Unit Ready USB Process
 * \returns None
 */
static void USBScsiTestUnitReady(void){
	switch(m_state[SCSITESTUNITREADY].Phase){
		case SCSITESTUNITREADY_WAIT:
			break;

		case SCSITESTUNITREADY_ERROR:
			ReturnError(SCSITESTUNITREADY);
			break;

		case SCSITESTUNITREADY_STALL:
			ReturnStall(SCSITESTUNITREADY);
			break;

		case SCSITESTUNITREADY_CSW:
			StartScsiIn(SCSITESTUNITREADY, SCSITESTUNITREADY_FINISH, SCSITESTUNITREADY_ERROR, SCSITESTUNITREADY_STALL, 13);
			break;

		case SCSITESTUNITREADY_FINISH:
			if(CheckCSW(NULL)) ReturnError(SCSITESTUNITREADY);
			else Return(SCSITESTUNITREADY);
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start SCSI Read Capacity USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \param ucLun Logical Unit Number
 * \returns None
 */
static void StartScsiReadCapacity(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucLun){
	uchar cbw[31];

	Start(SCSIREADCAPACITY, ReturnState, ReturnPhase, ReturnError, ReturnStall);
	CreateCBW(cbw, 8, TRUE, sizeof(SCSI_READ_CAPACITY), SCSI_READ_CAPACITY, 0);
	Out811Init(cbw, sizeof(cbw));
	StartUsbA(SCSIREADCAPACITY, SCSIREADCAPACITY_IN, SCSIREADCAPACITY_ERROR, SCSIREADCAPACITY_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}
/**
 * \details Response to SCSI Read Capacity USB process
 * \returns None
 */
static void USBScsiReadCapacity(void){
	bool flgStatusOK;
	unsigned long int residue, *ptr;

	switch(m_state[SCSIREADCAPACITY].Phase){
		case SCSIREADCAPACITY_WAIT:
			break;

		case SCSIREADCAPACITY_ERROR:
			ReturnError(SCSIREADCAPACITY);
			break;

		case SCSIREADCAPACITY_STALL:
			ReturnStall(SCSIREADCAPACITY);
			break;

		case SCSIREADCAPACITY_IN:
			StartScsiIn(SCSIREADCAPACITY, SCSIREADCAPACITY_CSW, SCSIREADCAPACITY_ERROR, SCSIREADCAPACITY_STALL, 8);
			break;

		case SCSIREADCAPACITY_CSW:
			// Process LBA and Block Length
			ptr = (unsigned long int *) &(m_ucSCSIReceive[0]);
			m_ulLastLogicalBlock = *ptr;
			ptr = (unsigned long int *) &(m_ucSCSIReceive[4]);
			m_ulLogicalBlockSize = *ptr;
			StartScsiIn(SCSIREADCAPACITY, SCSIREADCAPACITY_FINISH, SCSIREADCAPACITY_ERROR, SCSIREADCAPACITY_STALL, 13);
			break;

		case SCSIREADCAPACITY_FINISH:
			if(CheckCSW(&residue) == 0){
				if(residue == 0) flgStatusOK = TRUE;
				else flgStatusOK = FALSE;
			}else flgStatusOK = FALSE;

			if(flgStatusOK) Return(SCSIREADCAPACITY);
			else ReturnError(SCSIREADCAPACITY);
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*static void StartScsiReadBlock(int ReturnState, int ReturnPhase, int ReturnError, unsigned long int LBA, unsigned short int NumOfBlocks, uchar ucLun){
	uchar cbw[31], readlba_command[10];

	Start(SCSIREADBLOCK, ReturnState, ReturnPhase, ReturnError);
	memcpy(readlba_command, SCSI_READ_LBA, sizeof(SCSI_READ_LBA));
	memcpy(&(readlba_command[2]), &LBA, 4);
	memcpy(&(readlba_command[7]), &NumOfBlocks, 2);
	m_ulTransferNumOfBytes = m_ulLogicalBlockSize * NumOfBlocks;
	CreateCBW(cbw, m_ulTransferNumOfBytes, TRUE, sizeof(SCSI_READ_LBA), readlba_command, 0);
	Out811Init(cbw, sizeof(cbw));
	StartUsbAInt(SCSIREADBLOCK, SCSIREADBLOCK_IN, SCSIREADBLOCK_ERROR, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}*/
/**
 * \details Response to SCSI Read Block USB process
 * \returns None
 */
static void USBScsiReadBlock(void){
	bool flgStatusOK;
	unsigned long int residue;

	switch(m_state[SCSIREADBLOCK].Phase){
		case SCSIREADBLOCK_WAIT:
			break;

		case SCSIREADBLOCK_ERROR:
			SCSIPending = 0;
			SCSIError = 1;
			m_state[SCSIREADBLOCK].Phase = SCSIREADBLOCK_WAIT;
			break;

		case SCSIREADBLOCK_STALL:
			SCSIPending = 0;
			SCSIError = 1;
			m_state[SCSIREADBLOCK].Phase = SCSIREADBLOCK_WAIT;
			break;

		case SCSIREADBLOCK_IN:
			StartScsiInInt(SCSIREADBLOCK, SCSIREADBLOCK_FINISH, SCSIREADBLOCK_ERROR, SCSIREADBLOCK_STALL);
			break;

		case SCSIREADBLOCK_FINISH:
			if(CheckCSW(&residue) == 0){
				if(residue == 0) flgStatusOK = TRUE;
				else flgStatusOK = FALSE;
			}else flgStatusOK = FALSE;

			if(flgStatusOK) SCSIError = 0;
			else SCSIError = 1;

			SCSIPending = 0;
			m_state[SCSIREADBLOCK].Phase = SCSIREADBLOCK_WAIT;
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*static void StartScsiWriteBlock(int ReturnState, int ReturnPhase, int ReturnError, unsigned long int LBA, unsigned short int NumOfBlocks, uchar ucLun){
	uchar cbw[31], writelba_command[10];

	Start(SCSIWRITEBLOCK, ReturnState, ReturnPhase, ReturnError);
	memcpy(writelba_command, SCSI_WRITE_LBA, sizeof(SCSI_WRITE_LBA));
	memcpy(&(writelba_command[2]), &LBA, 4);
	memcpy(&(writelba_command[7]), &NumOfBlocks, 2);
	m_ulTransferNumOfBytes = m_ulLogicalBlockSize * NumOfBlocks;
	CreateCBW(cbw, m_ulTransferNumOfBytes, FALSE, sizeof(SCSI_WRITE_LBA), writelba_command, 0);
	Out811Init(cbw, sizeof(cbw));
	StartUsbA(SCSIWRITEBLOCK, SCSIWRITEBLOCK_OUT, SCSIWRITEBLOCK_ERROR, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}*/
/**
 * \details Process SCSI Write Block USB process
 * \returns None
 */
static void USBScsiWriteBlock(void){
	bool flgStatusOK;
	unsigned long int residue;

	switch(m_state[SCSIWRITEBLOCK].Phase){
		case SCSIWRITEBLOCK_WAIT:
			break;

		case SCSIWRITEBLOCK_ERROR:
			SCSIPending = 0;
			SCSIError = 1;
			m_state[SCSIWRITEBLOCK].Phase = SCSIWRITEBLOCK_WAIT;
			break;

		case SCSIWRITEBLOCK_STALL:
			SCSIPending = 0;
			SCSIError = 1;
			m_state[SCSIWRITEBLOCK].Phase = SCSIWRITEBLOCK_WAIT;
			break;

		case SCSIWRITEBLOCK_OUT:
			StartScsiOutInt(SCSIWRITEBLOCK, SCSIWRITEBLOCK_CSW, SCSIWRITEBLOCK_ERROR, SCSIWRITEBLOCK_STALL);
			break;

		case SCSIWRITEBLOCK_CSW:
			StartScsiIn(SCSIWRITEBLOCK, SCSIWRITEBLOCK_FINISH, SCSIWRITEBLOCK_ERROR, SCSIWRITEBLOCK_STALL, 13);
			break;

		case SCSIWRITEBLOCK_FINISH:
			if(CheckCSW(&residue) == 0){
				if(residue == 0) flgStatusOK = TRUE;
				else flgStatusOK = FALSE;
			}else flgStatusOK = FALSE;

			if(flgStatusOK) SCSIError = 0;
			else SCSIError = 1;

			SCSIPending = 0;
			m_state[SCSIWRITEBLOCK].Phase = SCSIWRITEBLOCK_WAIT;
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start SCSI In Int USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \returns None
 */
static void StartScsiInInt(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall){
	Start(SCSIININT, ReturnState, ReturnPhase, ReturnError, ReturnStall);
	if(SCSIReadBytes > m_ucInBufferSize) wr811(REG_BUFFER_LENGTH, m_ucInBufferSize);
	else wr811(REG_BUFFER_LENGTH, SCSIReadBytes);
	m_ucLastPID_ENDPOINT = BIT_IN_PID | m_ucInEndpoint;
	wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
	StartUsbA(SCSIININT, SCSIININT_IN, SCSIININT_ERROR, SCSIININT_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
}
/**
 * \details Response to SCSI In Int USB process
 * \returns None
 */
static void USBScsiInInt(void){
	uchar index, ucTransferCount;
	unsigned long int remaining;

	switch(m_state[SCSIININT].Phase){
		case SCSIININT_WAIT:
			break;

		case SCSIININT_ERROR:
			ReturnError(SCSIININT);
			break;

		case SCSIININT_STALL:
			ReturnStall(SCSIININT);
			break;

		case SCSIININT_IN:
			ucTransferCount = rd811(REG_BUFFER_LENGTH) - rd811(REG_TRANSFER_COUNT);
			if(ucTransferCount > 0){
				pCS1[USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
				if(SCSIReadInCount){
					for(index=0; index<ucTransferCount; index++) {
						*SCSIReadBufferPtr = pCS1[USB_HOST_OFF_RW];
						SCSIReadBufferPtr++;
					}
					SCSIReadInCount--;

					if(ucTransferCount == 13){
						if(*(SCSIReadBufferPtr-13) == 'U' && *(SCSIReadBufferPtr-12) == 'S' &&  *(SCSIReadBufferPtr-11) == 'B' && *(SCSIReadBufferPtr-10) == 'S'){
							ReturnError(SCSIININT);
							wr811(REG_INT_STATUS, BIT_INT_USB_A_STATUS);
							return;
						}
					}

				}else{
					for(index=0; index<ucTransferCount; index++){
						m_ucSCSIReceive[index] = pCS1[USB_HOST_OFF_RW];
					}
					Return(SCSIININT);
					wr811(REG_INT_STATUS, BIT_INT_USB_A_STATUS);
					return;
				}
			}

			if(SCSIReadInCount){
				remaining = SCSIReadBytes - (SCSIReadBufferPtr - SCSIReadBufferBegin);
				if(remaining > m_ucInBufferSize) wr811(REG_BUFFER_LENGTH, m_ucInBufferSize);
				else wr811(REG_BUFFER_LENGTH, remaining);

				m_ucLastPID_ENDPOINT = BIT_IN_PID | m_ucInEndpoint;
				wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				StartUsbA(SCSIININT, SCSIININT_IN, SCSIININT_ERROR, SCSIININT_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
			}else{
				wr811(REG_BUFFER_LENGTH, 13);
				m_ucLastPID_ENDPOINT = BIT_IN_PID | m_ucInEndpoint;
				wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				StartUsbA(SCSIININT, SCSIININT_IN, SCSIININT_ERROR, SCSIININT_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
			}
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start SCSI Out Int USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \returns None
 */
static void StartScsiOutInt(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall){
	Start(SCSIOUTINT, ReturnState, ReturnPhase, ReturnError, ReturnStall);

	if(SCSIWriteBytes > m_ucOutBufferSize){
		wr811(REG_BUFFER_LENGTH, m_ucOutBufferSize);
		wr811buffer((uchar *) SCSIWriteBufferPtr, m_ucOutBufferSize);
		SCSIWriteBufferPtr += m_ucOutBufferSize;

	}else{
		wr811(REG_BUFFER_LENGTH, SCSIWriteBytes);
		wr811buffer((uchar *) SCSIWriteBufferPtr, SCSIWriteBytes);
		SCSIWriteBufferPtr += SCSIWriteBytes;
	}
	m_ucLastPID_ENDPOINT = BIT_OUT_PID | m_ucOutEndpoint;
	wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
	StartUsbA(SCSIOUTINT, SCSIOUTINT_OUT, SCSIOUTINT_ERROR, SCSIOUTINT_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}
/**
 * \details Response to SCSI Out Int USB process
 * \returns None
 */
static void USBScsiOutInt(void){
	unsigned long int remaining;

	switch(m_state[SCSIOUTINT].Phase){
		case SCSIOUTINT_WAIT:
			break;

		case SCSIOUTINT_ERROR:
			ReturnError(SCSIOUTINT);
			break;

		case SCSIOUTINT_STALL:
			ReturnStall(SCSIOUTINT);
			break;

		case SCSIOUTINT_OUT:
			if(SCSIWriteBufferPtr < (SCSIWriteBufferBegin + SCSIWriteBytes)){
				remaining = SCSIWriteBytes - (SCSIWriteBufferPtr - SCSIWriteBufferBegin);
				if(remaining > m_ucOutBufferSize){
					wr811(REG_BUFFER_LENGTH, m_ucOutBufferSize);
					wr811buffer((uchar *) SCSIWriteBufferPtr, m_ucOutBufferSize);
					SCSIWriteBufferPtr += m_ucOutBufferSize;
				}else{
					wr811(REG_BUFFER_LENGTH, remaining);
					wr811buffer((uchar *) SCSIWriteBufferPtr, remaining);
					SCSIWriteBufferPtr += remaining;
				}
				m_ucLastPID_ENDPOINT = BIT_OUT_PID | m_ucOutEndpoint;
				wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				StartUsbA(SCSIOUTINT, SCSIOUTINT_OUT, SCSIOUTINT_ERROR, SCSIOUTINT_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
			}else{
				Return(SCSIOUTINT);
			}
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start Clear Feature USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \param In True = Inbound, False = Outbound
 * \param Endpoint Endpoint to clear
 * \returns None
 */
static void StartClearFeature(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, bool In, int Endpoint){
	uchar temp[8];
	Start(CLEARFEATURE, ReturnState, ReturnPhase, ReturnError, ReturnStall);

	memcpy(temp, CLEAR_FEATURE, sizeof(CLEAR_FEATURE));
	if(In) temp[4] = 0x80;
	temp[4] += Endpoint;
	Setup811Init(temp, sizeof(temp));
	StartUsbA(CLEARFEATURE, CLEARFEATURE_IN, CLEARFEATURE_ERROR, CLEARFEATURE_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}
/**
 * \details Response to Clear Feature USB process
 * \returns None
 */
static void USBClearFeature(void){
	switch(m_state[CLEARFEATURE].Phase){
		case CLEARFEATURE_WAIT:
			break;

		case CLEARFEATURE_ERROR:
			ReturnError(CLEARFEATURE);
			break;

		case CLEARFEATURE_STALL:
			ReturnStall(CLEARFEATURE);
			break;

		case CLEARFEATURE_IN:
			wr811(REG_BUFFER_LENGTH, 8);
			m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
			wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
			m_ucData_0_1 = 0;
			StartUsbA(CLEARFEATURE, CLEARFEATURE_RERUNLAST, CLEARFEATURE_ERROR, CLEARFEATURE_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
			break;

		case CLEARFEATURE_RERUNLAST:
			wr811(REG_BUFFER_LENGTH, m_ucStallBufferLength);
			m_ucLastPID_ENDPOINT = m_ucStallLastPID_ENDPOINT;
			wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
			if(m_ucStallData_0_1) m_ucData_0_1 = 0;
			else m_ucData_0_1 = 1;
			StartUsbA(CLEARFEATURE, CLEARFEATURE_FINISH, CLEARFEATURE_ERROR, CLEARFEATURE_STALL, m_ucStallLastCommand);
			break;

		case CLEARFEATURE_FINISH:
			Return(CLEARFEATURE);
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Start Request Sense USB process
 * \param ReturnState Return State
 * \param ReturnPhase Return Phase
 * \param ReturnError Return Error Phase
 * \param ReturnStall Return Stall Phase
 * \returns None
 */
static void StartRequestSense(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall){
	uchar cbw[31];

	Start(REQUESTSENSE, ReturnState, ReturnPhase, ReturnError, ReturnStall);
	CreateCBW(cbw, 18, TRUE, sizeof(SCSI_REQUEST_SENSE), SCSI_REQUEST_SENSE, 0);
	Out811Init(cbw, sizeof(cbw));
	StartUsbA(REQUESTSENSE, REQUESTSENSE_IN, REQUESTSENSE_ERROR, REQUESTSENSE_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
}
/**
 * \details Response to Request Sense USB process
 * \returns None
 */
static void USBRequestSense(void){
		switch(m_state[REQUESTSENSE].Phase){
			case REQUESTSENSE_WAIT:
				break;

			case REQUESTSENSE_ERROR:
				ReturnError(REQUESTSENSE);
				break;

			case REQUESTSENSE_STALL:
				ReturnStall(REQUESTSENSE);
				break;

			case REQUESTSENSE_IN:
				StartScsiIn(REQUESTSENSE, REQUESTSENSE_CSW, REQUESTSENSE_ERROR, REQUESTSENSE_STALL, 18);
				break;

			case REQUESTSENSE_CSW:
				StartScsiIn(REQUESTSENSE, REQUESTSENSE_FINISH, REQUESTSENSE_ERROR, REQUESTSENSE_STALL, 13);
				break;

			case REQUESTSENSE_FINISH:
				Return(REQUESTSENSE);
				break;
		}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Remove bytes from outbound circular queue and places them in the SL811 buffer to be send out to the printer
 * \returns None
 */
static void RunPrinter(void){
	uchar index;
	ushort uCirLen;

	if(m_ucEnumerated == 1){
		if(m_uiHead != m_uiTail){
			if(m_state[USBA].Phase != USBA_WAIT_FOR_USBA){
				uCirLen = CirLen();
				if(uCirLen >= m_ucOutBufferSize){
					wr811(REG_BUFFER_LENGTH, m_ucOutBufferSize);
					pCS1[USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
					for(index=0; index<m_ucOutBufferSize; index++){
						if(m_uiHead != m_uiTail){
							pCS1[USB_HOST_OFF_RW] = m_ucCircularBuffer[m_uiHead];
							//m_uiHead = IncCirIndex(m_uiHead);
							if(m_uiHead == CIR_BUF_SIZE -1) m_uiHead = 0;
							else m_uiHead++;
						}else{
							pCS1[USB_HOST_OFF_RW] = 0;
						}
					}
				}else{
					wr811(REG_BUFFER_LENGTH, uCirLen);
					pCS1[USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
					while(m_uiHead != m_uiTail){
						pCS1[USB_HOST_OFF_RW] = m_ucCircularBuffer[m_uiHead];
						//m_uiHead = IncCirIndex(m_uiHead);
						if(m_uiHead == CIR_BUF_SIZE -1) m_uiHead = 0;
						else m_uiHead++;
					}
				}
				m_ucLastPID_ENDPOINT = BIT_OUT_PID | m_ucOutEndpoint;
				wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);          // Write an out packet to endpoint 2
				StartUsbA(RUNPRINTER, RUNPRINTER_SEND, RUNPRINTER_ERROR, RUNPRINTER_SEND, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
			}
		}else{
			wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);                   // clear the DONE bit
			m_state[RUNPRINTER].Phase = RUNPRINTER_WAIT;
		}
	}else{
		wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);                   // clear the DONE bit
	}
}
/**
 * \details Kick off RunPrinter, if outbound circular queue is not empty
 * \returns None
 */
static void USBRunPrinter(void){
	switch(m_state[RUNPRINTER].Phase){
		case RUNPRINTER_WAIT:
			break;

		case RUNPRINTER_ERROR:
			break;

		case RUNPRINTER_STALL:
			break;

		case RUNPRINTER_SEND:
			if(m_uiHead != m_uiTail){
				RunPrinter();
			}else{
				wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);                   // clear the DONE bit
				m_state[RUNPRINTER].Phase = RUNPRINTER_WAIT;
			}
			break;

		case RUNPRINTER_FINISH:
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Kick off USB process from the timer interrupt
 * \returns None
 */
void USBTimedPoll(void){
	if(m_flgEnablePoll){
		if(m_ucEnumerated == 0){
			if(m_state[TIMER].Phase) USBDelay();
			if(m_state[USBA].Phase) USBUsbA();
			if(m_state[SETADDR].Phase) USBSetAddr();
			if(m_state[GETDESC].Phase) USBGetDesc();
			if(m_state[SETCONFIG].Phase) USBSetConfig();
			if(m_state[SETINTFACE].Phase) USBSetIntface();
			if(m_state[SCSIIN].Phase) USBScsiIn();
			if(m_state[SCSIINQUIRY].Phase) USBScsiInquiry();
			if(m_state[SCSITESTUNITREADY].Phase) USBScsiTestUnitReady();
			if(m_state[SCSIREADCAPACITY].Phase) USBScsiReadCapacity();
			if(m_state[CLEARFEATURE].Phase) USBClearFeature();
			if(m_state[REQUESTSENSE].Phase) USBRequestSense();
			if(m_state[MASTER].Phase) USBMaster();
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \details Enable the interrupt to the SL811 chip
 * \returns None
 */
static void enablePrinterInterrupt(void){
	// Enable USBA Int
	wr811(REG_INT_ENABLE, BIT_INT_INSERT_ENABLE | BIT_INT_USB_A_ENABLE);

	// Clear All 811 interrupts
	wr811(REG_INT_STATUS, 0xFF);

	cf.intc[0].imrl &= 0xffffffbe;													// Unmask IRQ 6
}
/**
 * \details Disable the interrupt to the SL811 chip
 * \returns None
 */
static void disablePrinterInterrupt(void){
	// Disable All USB Interrupts
	wr811(REG_INT_ENABLE, 0x00);

	// Clear All 811 interrupts
	wr811(REG_INT_STATUS, 0xFF);

	cf.intc[0].imrl = (cf.intc[0].imrl | 0x00000040) & 0xfffffffe;					// Mask IRQ 6 and write 0 to bit0
}
/**
 * \details Interrupt handler for the SL811 chip
 * \returns None
 */
__interrupt void printer_interrupt(void){
	if((rd811(REG_INT_STATUS) & BIT_INT_INSERT_ENABLE) == BIT_INT_INSERT_ENABLE){
		f_mount(1, NULL);
		free(usbFileSystemObject);
		SCSIPending = 0;
		SCSIError = TRUE;
		disablePrinterInterrupt();
		m_state[MASTER].Phase = MASTER_RESET_USB_BUS;
		m_ucEnumerated = 0;
		m_uiNAKReplyDelay = NAK_REPLY_INSIDE_INTERRUPT;
		USBClearNAKCounters();
		m_flgPrinterBlocked = FALSE;
	}else{
		if(m_ucEnumerated == 1){
			do{
				if(m_state[USBA].Phase) USBUsbA();
				if(m_state[RUNPRINTER].Phase) USBRunPrinter();
			}while((m_uiHead != m_uiTail) && (m_state[USBA].Phase != USBA_WAIT_FOR_USBA));

			if((m_uiHead == m_uiTail) && (m_state[USBA].Phase == USBA_WAIT)) m_flgPrinterBlocked = TRUE;
			else m_flgPrinterBlocked = FALSE;
		}else if(m_ucEnumerated == 2){
			if(SCSIPending == 1){
				do{
					if(m_state[USBA].Phase) USBUsbA();
					if(m_state[SCSIININT].Phase) USBScsiInInt();
					if(m_state[SCSIREADBLOCK].Phase) USBScsiReadBlock();
				}while((SCSIPending != 0) && (m_state[USBA].Phase != USBA_WAIT_FOR_USBA));
			}else if(SCSIPending == 2){
				do{
					if(m_state[USBA].Phase) USBUsbA();
					if(m_state[SCSIOUTINT].Phase) USBScsiOutInt();
					if(m_state[SCSIIN].Phase) USBScsiIn();
					if(m_state[SCSIWRITEBLOCK].Phase) USBScsiWriteBlock();
				}while((SCSIPending != 0) && (m_state[USBA].Phase != USBA_WAIT_FOR_USBA));
			}

			m_flgPrinterBlocked = FALSE;
		}
	}
}

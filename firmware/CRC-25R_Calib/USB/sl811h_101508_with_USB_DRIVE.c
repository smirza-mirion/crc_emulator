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
#include "crc.h"
#include "pit.h"
#include "keyboard.h"   //for service_watchdog

extern ushort g_Init_USB_PCL;
//
extern volatile uchar *pCS1;

// Function prototypes for functions outside this file
void set_host_usb_power(bool on);

ushort get_minute_counter(void);       // 1 ms counter, rollover at 60000
ushort from_Reset_USB_BUS;

#define USB_WRITE_TIMEOUT               50000     //= 50 sec
short  m_uiUsbWriteBytesRemaining;

// USB Polling State
#define P_RESET_USB_BUS                 0x0000  // Send
#define P_ENABLE_USB_BUS                0x0001  //
#define P_CHK_ATTACHED_DEVICE           0x0002
#define P_ASK_DESC_DEV_INIT             0x0003
#define P_READ_DESC_DEV_INIT            0x0004
#define P_READ2_DESC_DEV_INIT           0x0005
#define P_RESET_USB_BUS2                0x0006
#define P_ENABLE_USB_BUS2               0x0007
#define P_CHK_ATTACHED_DEVICE2          0x0008
#define P_ISSUE_SET_USB_ADDRESS         0x0009
#define P_SEND_SET_USB_ADDRESS_IN_PID   0x000A
#define P_SET_USB_ADDRESS_TO_1          0x000B
#define P_READ_FULL_DEVICE_DESCRIPTOR   0x000C
#define P_READ_INIT_CONFIG_DESCRIPTOR	0x000D
#define P_READ_FULL_CONFIG_DESCRIPTOR	0x000E
#define P_SET_CONFIGURATION				0x000F
#define P_SET_CONFIGURATION_SEND_IN_PACKET 0x0010
#define P_SET_INTERFACE					0x0011
#define P_SET_INTERFACE_SEND_IN_PACKET	0x0012
#define P_READ_CLASS_DESCRIPTOR			0x0013
#define P_ENUMERATED					0x0014
#define P_READ_MAX_LUN					0x0015
#define P_ENUMERATED_FLASH_SEND_CBW		0x0016
#define P_ENUMERATED_FLASH_SEND_DATA	0x0017
#define P_ENUMERATED_FLASH_RECEIVE_DATA	0x0018
#define P_ENUMERATED_FLASH_RECEIVE_CSW	0x0019
#define P_READ_CLASS_DESCRIPTOR_MAX_LUN 0x001A
#define P_POLL_STALL                    0xFFFF
#define P_POLL_DELAY                    0xFFFE
#define P_POLL_INT_USB_A                0xFFFD
#define P_SEND_IN_PID                   0xFFFC

// Offsets from pCS1 to access sl811 registers
#define USB_HOST_OFF_SET            6       // set register address
#define USB_HOST_OFF_RW             7       // reads from /writes to register

// sl811 host registers addresses
#define	REG_CONTROL                 0x00    // write this register to kick off a transfer
#define REG_CONTROL_B				0x08	// write this register to kick off a transfer
#define	REG_BUFFER_ADDRESS          0x01    // start of internal data buffer
#define REG_BUFFER_ADDRESS_B		0x09	// start of internal data buffer
#define	REG_BUFFER_LENGTH           0x02    // length of internal buffer
#define REG_BUFFER_LENGTH_B			0x0A	// length of internal buffer
#define	REG_PID_ENDPOINT            0x03    // name when written--PID and Endpoint for next xfr
#define REG_PID_ENDPOINT_B			0x0B	// name when written--PID and Endpoint for next xfr
#define	REG_PACKET_STATUS           0x03    // name when read--status of last transfer
#define REG_PACKET_STATUS_B			0x0B	// name when read--status of last transfer
#define	REG_DEVICE_ADDRESS          0x04    // name when written--USB function address
#define REG_DEVICE_ADDRESS_B		0x0C	// name when written--USB function address
#define REG_TRANSFER_COUNT			0x04	// name when read--number of bytes received or transfered
#define REG_TRANSFER_COUNT_B		0x0C	// name when read--number of bytes received or transfered
#define	REG_CONTROL_1               0x05    // more control stuff
#define REG_INT_ENABLE              0x06    //
#define	REG_INT_STATUS              0x0D    // Interrupt request status bits. We use DONE and SOF.
#define	REG_SOF_COUNT_LO            0x0E    // SOF (EOP) time constant low byte
#define	REG_SOF_COUNT_HI            0x0F    // name when written--EOP time constant high byte

// sl811 buffer values
#define VAL_BUFFER_OFFSET           0x10    // Base location for sl811 data buffer
// #define VAL_BUFFER_LENGTH           64      // Hardcoded buffer size for out end point for a HP inkjet printer, will need to generalize by discovery

// sl811 PID (Packet Identifier) bit settings (must be ORed with endpoint in the low nibble)
#define	BIT_SETUP_PID               0xD0    // for the 'set address' request
#define	BIT_IN_PID                  0x90    // IN transactions
#define BIT_OUT_PID                 0x10    // OUT transactions
#define	BIT_SOF_PID                 0x50    // start of frame marker
#define BIT_PREAMBLE_PID            0xC0
#define BIT_NAK_PID                 0xA0
#define BIT_STALL_PID               0xE0
#define BIT_DATA0_PID               0x30
#define BIT_DATA1_PID               0xB0

// Endpoint must be ORed with PID in the high nibble
#define BIT_EP0                     0x00    // Endpoint 0
#define BIT_EP1                     0x01    // Endpoint 1
#define BIT_EP2                     0x02    // Endpoint 2

// Control Bit Settings
#define BIT_CTRL_ARM                0x01
#define BIT_CTRL_ENABLE             0x02
#define BIT_CTRL_IN                 0x00
#define BIT_CTRL_OUT                0x04

// constants for 811 CONTROL 1 register
#define	USB_RESET                           0x08    // Reset USB bus
#define	USB_OPERATE_LO_SPEED                0x21    // Low Speed=1(b5) and SOF(EOP)EN=1(b0)
#define USB_OPERATE_HI_SPEED                0x01    // Low Speed=0(b5) and SOF(EOP)EN=1(b0)
#define USB_OPERATE_HI_SPEED_WITHOUT_SOF    0x00    // Low Speed=0(b5) and SOF(EOP)EN=0(b0)

// sl811 Interrupt Enable bit settings
#define BIT_INT_USB_A_ENABLE        0x01    // Enables USB A interrupt
#define BIT_INT_USB_B_ENABLE        0x02    // Enables USB B interrupt
#define BIT_INT_SOF_ENABLE          0x10    // Enables the Start Of Frame interrupt
#define BIT_INT_INSERT_ENABLE       0x20    // Enables the Slave Insertion / Removal interrupt
#define BIT_INT_DEVICE_ENABLE       0x40    // Enables the Device Detect interrupt

// sl811 Interrupt Status bit settings
#define BIT_INT_USB_A_STATUS        0x01    // USB A interrupt status
#define BIT_INT_USB_B_STATUS        0x02    // USB B interrupt status
#define BIT_INT_SOF_STATUS          0x10    // Start Of Frame interrupt status
#define BIT_INT_INSERT_STATUS       0x20    // Slave Insertion / Removal interrupt status
#define BIT_INT_DEVICE_STATUS       0x40    // Device Detect state
#define BIT_INT_D_PLUS_STATUS       0x80    // D+ state

// USB printer port bit settings
#define BIT_PORT_STATUS_EMPTY       0x20    // Paper Empty Condition on printer (1 = empty)
#define BIT_PORT_STATUS_SELECT      0x10    // Selected (1 = selected)
#define BIT_PORT_STATUS_NOT_ERROR   0x08    // No Error (0 = Error)

// sl811 Packet Status bit settings
#define BIT_STATUS_ACK              0x01
#define BIT_STATUS_ERROR            0x02
#define BIT_STATUS_TIMEOUT          0x04
#define BIT_STATUS_SEQ              0x08
#define BIT_STATUS_SETUP            0x10
#define BIT_STATUS_OVERFLOW         0x20
#define BIT_STATUS_NAK              0x40
#define BIT_STATUS_STALL            0x80

#define KEEP_ALIVE_COUNT			500

// USB DESCRIPTOR
#define DESC_TYPE_CONFIGURATION		0x02
#define DESC_TYPE_INTERFACE 		0x04
#define DESC_TYPE_ENDPOINT 			0x05

#define ATTACHED_UNKNOWN	0x00
#define ATTACHED_PRINTER	0x01
#define ATTACHED_FLASH		0x02

// function prototypes
void wr811 (uchar ucRegister, uchar ucValue);       // write a value into a sl811 register
uchar rd811 (uchar ucRegister);                     // read a value from a sl811 register
void wr811buffer (uchar *pucData, uchar ucLength);  // write a sequence of bytes to sl811 starting a location VAL_BUFFER_OFFSET
ushort IncCirIndex (ushort uiIndex);                // Returns the next index for the circular buffer, this takes care of wrap around
ushort CirLen(void);                                // Returns the length of the data in the circular buffer
void Poll_Go(uchar ucCommand);

#define WAIT_FOR_INTA_TIMEOUT	100                 // Number of 10 ms cycles to wait for a response before jumping to P_RESET_USB_BUS
void PollDelay(ushort uiTenMSCountDown);
void PollDelayJump(ushort uiTenMSCountDown, ushort uiNextPollPhase);
uchar VAL_BUFFER_OFFSET_B;
ushort m_uiCountDown;
ushort m_uiNextPollPhase;
ushort m_uiDescriptorFinishPhase;
uchar m_ucLastPID_ENDPOINT;
uchar m_ucLastCommand;
uchar m_ucOutEndpoint;
uchar m_ucOutBufferSize;
uchar m_ucInEndpoint;
uchar m_ucInBufferSize;

// CHANGED 10/15/2008
uchar m_ucMaxPacketSize;

volatile uchar m_ucData_0_1;
volatile uchar m_ucData_0_1_B;
bool m_flgEnablePoll = FALSE;
bool m_flgFullSpeed = FALSE;
bool m_flgSendFirstINPID;
uchar m_ucDescriptor[256];
uchar m_ucDescriptorStartIndex;
uchar m_ucDescriptorSize;
ushort m_uiPollPhase = P_RESET_USB_BUS;
uchar m_ucMultiplier;
uchar m_ucTotalInterfaces;
uchar m_ucConfiguration;
uchar m_ucInterfaceNum;
uchar m_ucAltInterfaceNum;
uchar m_ucAttachedDevice; // 0=Unknown, 1=Printer, 2=Flash Drive
uchar m_ucMaxLun;
volatile ushort m_uiUsbDiskStatus = USB_DISK_NOT_ATTACHED;
volatile bool m_boolCircularBufferValid = FALSE;

// Circular Buffer Variables
#define CIR_BUF_SIZE 1000
bool m_flgEnumerated = FALSE;
volatile uchar m_ucCircularBuffer [CIR_BUF_SIZE];
volatile ushort m_uiHead = 0;
volatile ushort m_uiTail = 0;

void GetDescriptor(uchar *pucData);

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
uchar SET_CONFIGURATION_TO_1 [] = {
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

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: InitializeSL811H                                                                          */
/*       Input: None                                                                                      */
/*      Output: None                                                                                      */
/* Description: This function powers up the SL811 host chip and tries to initiate a printer               */
/*                                                                                                        */
/**********************************************************************************************************/
void InitializeSL811H (void)
{
    // Reset the SL811H chip
    set_host_usb_power (TRUE);
    delay_msec (2000);
	m_flgEnablePoll = TRUE;
    // ReinitializeSL811H();
    // delay_msec (1000);
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: USBTimedPoll                                                                              */
/*       Input: None                                                                                      */
/*      Output: None                                                                                      */
/* Description: This function is executed by a 10 ms timer. This function sends OUT packets to endpoint 2 */
/*              It retrieves data from the circular buffer and sends it to the USB printer. If there is   */
/*              no data, it will send a packet filled with zeros                                          */
/**********************************************************************************************************/
void USBTimedPoll (void)
{
	uchar i, ucTransferCount, ucSize, ucIndex, ucDescriptorType;
	uchar ucCommand, ucCurrentConfiguration;
	bool flgFoundOutEndpoint;
	bool flgFoundInEndpoint;
	bool flgCorrectInterface;
	uchar ucSetConfig[10];
	static ushort s_uiKeepAliveCountDown;

	if (m_flgEnablePoll)
	{
		switch (m_uiPollPhase)
		{
			case P_RESET_USB_BUS:
				m_uiUsbDiskStatus = USB_DISK_NOT_ATTACHED;
				m_ucAttachedDevice = ATTACHED_UNKNOWN;
				m_flgEnumerated = FALSE;
				from_Reset_USB_BUS = 1;
				// Clear USB-A Registers
				wr811 (REG_CONTROL, 0x00);
				wr811 (REG_BUFFER_ADDRESS, 0x00);
				wr811 (REG_BUFFER_LENGTH, 0x00);
				wr811 (REG_PID_ENDPOINT, 0x00);
				wr811 (REG_DEVICE_ADDRESS, 0x00);
				wr811 (REG_CONTROL_1, 0x00);
				wr811 (REG_INT_ENABLE, 0x00);
				wr811 (REG_INT_STATUS, 0xFF);
				wr811 (REG_SOF_COUNT_LO, 0x00);
				wr811 (REG_SOF_COUNT_HI, 0x00);

				// Clear USB-B Registers
				wr811 (REG_CONTROL_B, 0x00);
				wr811 (REG_BUFFER_ADDRESS_B, 0x00);
				wr811 (REG_BUFFER_LENGTH_B, 0x00);
				wr811 (REG_PID_ENDPOINT_B, 0x00);
				wr811 (REG_DEVICE_ADDRESS_B, 0x00);

				/******************************************************************************************************/
				/* Test for connected device and device speed                                                         */
				/******************************************************************************************************/

				wr811 (REG_SOF_COUNT_HI, 0xAE);				// Set SOF high counter, no change D+/D-, host mode

				// Reset USB Bus
				wr811 (REG_CONTROL_1, USB_RESET);			// Reset USB engine, full-speed setup, suspend disable
				PollDelay(10);								// Wait for 100 ms
				break;

			case P_ENABLE_USB_BUS:
				wr811 (REG_CONTROL_1, USB_OPERATE_HI_SPEED_WITHOUT_SOF);										// Set to normal operation
				wr811 (REG_INT_ENABLE, BIT_INT_DEVICE_ENABLE | BIT_INT_INSERT_ENABLE | BIT_INT_USB_A_ENABLE);
				wr811 (REG_INT_STATUS, 0xFF);																	// Clear Interrupt enable status
				PollDelay(10);																					// Wait for 100 ms
				break;

			case P_CHK_ATTACHED_DEVICE:
				if (rd811 (REG_INT_STATUS) & BIT_INT_DEVICE_STATUS)
				{
					wr811 (REG_INT_STATUS, 0xFF);			// Clear Interrupt enable status
					PollDelayJump(100, P_RESET_USB_BUS);	// Jump to beginning after 1 sec delay
				}
				else
				{
					// Checking full or low speed
					if ((rd811 (REG_INT_STATUS) & BIT_INT_D_PLUS_STATUS) == 0)
					{                                       // ** Low Speed is detected ** //S
						wr811 (REG_SOF_COUNT_HI, 0xEE);     // Set up host and low speed direct and SOF cnt
						wr811 (REG_SOF_COUNT_LO, 0xE0);     // SOF Counter Low = 0xE0; 1ms interval
						wr811 (REG_CONTROL_1, USB_OPERATE_LO_SPEED);
						m_flgFullSpeed = FALSE;               // low speed device flag
					}
					else
					{                                       // ** Full Speed is detected ** //
						wr811 (REG_SOF_COUNT_HI, 0xAE);     // Set up host & full speed direct and SOF cnt
						wr811 (REG_SOF_COUNT_LO, 0xE0);     // SOF Counter Low = 0xE0; 1ms interval
						wr811 (REG_CONTROL_1, USB_OPERATE_HI_SPEED);
						m_flgFullSpeed = TRUE;
					}

					wr811 (REG_PID_ENDPOINT, BIT_SOF_PID | BIT_EP0);
					wr811 (REG_DEVICE_ADDRESS, 0x00);       // set device address to zero
					wr811 (REG_CONTROL, 0x01);              // start generate SOF or EOP
					PollDelay(10);							// Wait for 100 ms
				}
				break;

			case P_ASK_DESC_DEV_INIT:
				wr811 (REG_INT_STATUS, 0xFF);                   // Clear Interrupt status

				// SL811 initialization
				wr811 (REG_BUFFER_ADDRESS, VAL_BUFFER_OFFSET);  // start of SETUP/IN internal data buffer
				wr811 (REG_BUFFER_LENGTH, 0x08);                // reserve 8 bytes

				wr811buffer(GET_DESC_DEV, sizeof(GET_DESC_DEV));
				m_ucLastPID_ENDPOINT = BIT_SETUP_PID | BIT_EP0;
				wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				m_ucData_0_1 = 0;
				Poll_Go(BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
				break;

			case P_READ_DESC_DEV_INIT:
				m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
				wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				m_ucData_0_1 = 0;
				Poll_Go (BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
				break;

			case P_READ2_DESC_DEV_INIT:

				// CHANGED 10/15/2008
				// Read Max Packet Size
				m_ucMaxPacketSize = rd811 (VAL_BUFFER_OFFSET + 7);

				wr811 (REG_BUFFER_LENGTH, 0);
				m_ucLastPID_ENDPOINT = BIT_OUT_PID | BIT_EP0;
				wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				Poll_Go(BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
				break;

			case P_RESET_USB_BUS2:
				wr811 (REG_CONTROL_1, USB_RESET);
				PollDelay(2);				// Wait for 20 ms
				break;

			case P_ENABLE_USB_BUS2:
				wr811 (REG_CONTROL_1, USB_OPERATE_HI_SPEED_WITHOUT_SOF);            							// Set to normal operation
				wr811 (REG_INT_ENABLE, BIT_INT_DEVICE_ENABLE | BIT_INT_INSERT_ENABLE | BIT_INT_USB_A_ENABLE);
 				wr811 (REG_INT_STATUS, 0xFF);                                                                   // Clear Interrupt
                PollDelay(10);                                                                                  // Wait for 100 ms
				break;

			case P_CHK_ATTACHED_DEVICE2:
				if (rd811 (REG_INT_STATUS) & BIT_INT_DEVICE_STATUS)
				{
					wr811 (REG_INT_STATUS, 0xFF);                   // Clear Interrupt enable status
					PollDelayJump(100, P_RESET_USB_BUS);    // Jump to beginning after 1 sec delay
				}
				else
				{
					// Checking full or low speed
					if ((rd811 (REG_INT_STATUS) & BIT_INT_D_PLUS_STATUS) == 0)
					{                                       // ** Low Speed is detected ** //
						wr811 (REG_SOF_COUNT_HI, 0xEE);     // Set up host and low speed direct and SOF cnt
						wr811 (REG_SOF_COUNT_LO, 0xE0);     // SOF Counter Low = 0xE0; 1ms interval
						wr811 (REG_CONTROL_1, USB_OPERATE_LO_SPEED);
						m_flgFullSpeed = FALSE;             // low speed device flag
					}
					else
					{                                       // ** Full Speed is detected ** //
						wr811 (REG_SOF_COUNT_HI, 0xAE);     // Set up host & full speed direct and SOF cnt
						wr811 (REG_SOF_COUNT_LO, 0xE0);     // SOF Counter Low = 0xE0; 1ms interval
						wr811 (REG_CONTROL_1, USB_OPERATE_HI_SPEED);
						m_flgFullSpeed = TRUE;
					}

					wr811 (REG_PID_ENDPOINT, BIT_SOF_PID | BIT_EP0);
					wr811 (REG_DEVICE_ADDRESS, 0x00);       // set device address to zero
					wr811 (REG_CONTROL, 0x01);              // start generate SOF or EOP
					PollDelay(10);                          // Wait for 100 ms
				}
				break;

			case P_ISSUE_SET_USB_ADDRESS:
				wr811 (REG_INT_STATUS, 0xFF);		// Clear Interrupt status
				wr811 (REG_BUFFER_ADDRESS, VAL_BUFFER_OFFSET);	// Start of SETUP/IN internal data buffer
				wr811 (REG_BUFFER_LENGTH, 0x08);
				wr811buffer (SET_ADDRESS_TO_1, sizeof(SET_ADDRESS_TO_1));
				m_ucLastPID_ENDPOINT = BIT_SETUP_PID | BIT_EP0;
				wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				m_ucData_0_1 = 0;
				Poll_Go (BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
				break;

			case P_SEND_SET_USB_ADDRESS_IN_PID:
				m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
				wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				m_ucData_0_1 = 0;
				Poll_Go (BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN);
				break;

			case P_SET_USB_ADDRESS_TO_1:
				wr811 (REG_DEVICE_ADDRESS, 0x01); // Set USB Address to 1
				wr811 (REG_DEVICE_ADDRESS_B, 0x01);
				GetDescriptor (GET_DESC_DEV);
				break;

			case P_READ_FULL_DEVICE_DESCRIPTOR:
				GetDescriptor (GET_DESC_CONFIG);
				break;

			case P_READ_INIT_CONFIG_DESCRIPTOR:
				GetDescriptor (GET_DESC_CONFIG_FULL);
				break;

			case P_READ_FULL_CONFIG_DESCRIPTOR:
				// Search for Out Endpoint and buffer Size
				i = 0;
				flgFoundOutEndpoint = FALSE;
				flgFoundInEndpoint = FALSE;
				flgCorrectInterface = FALSE;
				while (i < m_ucDescriptorStartIndex)
				{
					ucSize = m_ucDescriptor [i];
					ucDescriptorType = m_ucDescriptor[i + 1];

					switch (ucDescriptorType)
					{
						case DESC_TYPE_CONFIGURATION:
							m_ucTotalInterfaces = m_ucDescriptor[i + 4];
							ucCurrentConfiguration = m_ucDescriptor[i + 5];
							//if (m_ucTotalInterfaces == 1) flgCorrectInterface = TRUE;
							//else flgCorrectInterface = FALSE;
							break;
						case DESC_TYPE_INTERFACE:
							//if (m_ucTotalInterfaces > 1)
							//{
							if(!flgCorrectInterface){
								if (m_ucDescriptor[i + 5] == 7 && m_ucDescriptor[i + 6] == 1){
									m_ucInterfaceNum = m_ucDescriptor[i + 2];
									m_ucAltInterfaceNum = m_ucDescriptor[i + 3];
									m_ucAttachedDevice = ATTACHED_PRINTER;
									flgCorrectInterface = TRUE;
								}
								else if(m_ucDescriptor[i + 5] == 8 && m_ucDescriptor[i + 6] == 6 && m_ucDescriptor[i + 7] == 80){ // Mass Storage, SCSI Command Set, Bulk Only Transport
									m_ucInterfaceNum = m_ucDescriptor[i + 2];
									m_ucAltInterfaceNum = m_ucDescriptor[i + 3];
									m_ucAttachedDevice = ATTACHED_FLASH;
									flgCorrectInterface = TRUE;
								}
							}
							break;
							
						case DESC_TYPE_ENDPOINT:
							if (flgCorrectInterface)
							{
								// Test for OUT Endpoint descriptor
								if (((m_ucDescriptor [i + 2]) & 0x80) == 0x00)
								{
									if(flgFoundOutEndpoint == FALSE){
										if((m_ucDescriptor [i + 3] & 3) == 2){ // Bulk Endpoint
											flgFoundOutEndpoint = TRUE;
											m_ucConfiguration = ucCurrentConfiguration;
											m_ucOutEndpoint = ((m_ucDescriptor [i + 2]) & 0x7F);
											m_ucOutBufferSize = m_ucDescriptor [i + 4];
										}else{								   // Not Bulk Endpoint
											flgCorrectInterface = FALSE;		// Reset to try next interface
										}
									}
								}
								else
								{
									if (flgFoundInEndpoint == FALSE){
										if((m_ucDescriptor [i + 3] & 3) == 2){	// Bulk Endpoint
											flgFoundInEndpoint = TRUE;
											m_ucConfiguration = ucCurrentConfiguration;
											m_ucInEndpoint = ((m_ucDescriptor [i + 2]) & 0x7F);
											m_ucInBufferSize = m_ucDescriptor [i + 4];
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

				if (flgFoundOutEndpoint && flgFoundInEndpoint) m_uiPollPhase = P_SET_CONFIGURATION;
				else m_uiPollPhase = P_POLL_STALL;
				break;

			case P_SET_CONFIGURATION:
				// Send a CONTROL transfer to select configuration m_ucConfiguration.
				memcpy(ucSetConfig, SET_CONFIGURATION_TO_1, sizeof(SET_CONFIGURATION_TO_1));
				ucSetConfig[2] = m_ucConfiguration;
				wr811buffer (ucSetConfig, sizeof (SET_CONFIGURATION_TO_1));
				wr811 (REG_BUFFER_LENGTH, 8);
				m_ucLastPID_ENDPOINT = BIT_SETUP_PID | BIT_EP0;
		        wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);          // OR in the endpoint (zero)
				m_ucData_0_1 = 0;
		        Poll_Go (0x07);                                       // DIREC=1(out), ENAB=1, ARM=1
				break;

			case P_SET_CONFIGURATION_SEND_IN_PACKET:
				wr811 (REG_BUFFER_LENGTH, 8);
				m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
				wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				m_ucData_0_1 = 0;
				Poll_Go (0x03);
				break;

		    case P_SET_INTERFACE:
				if (m_ucTotalInterfaces == 1){
					if(m_ucAttachedDevice == ATTACHED_PRINTER) m_uiPollPhase = P_READ_CLASS_DESCRIPTOR;
					else m_uiPollPhase = P_READ_CLASS_DESCRIPTOR_MAX_LUN;
					//else m_uiPollPhase = P_ENUMERATED_FLASH_SEND_CBW;
				}else{
					SET_INTERFACE[2] = m_ucAltInterfaceNum;
					SET_INTERFACE[4] = m_ucInterfaceNum;
					wr811buffer(SET_INTERFACE, sizeof(SET_INTERFACE));
					wr811 (REG_BUFFER_LENGTH, 8);
					m_ucLastPID_ENDPOINT = BIT_SETUP_PID | BIT_EP0;
					wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);          // OR in the endpoint (zero)
					m_ucData_0_1 = 0;
					Poll_Go (0x07);
				}
				break;
				
			case P_SET_INTERFACE_SEND_IN_PACKET:
				wr811 (REG_BUFFER_LENGTH, 8);
				m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
				wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
				m_ucData_0_1 = 0;
				if(m_ucAttachedDevice == ATTACHED_FLASH) m_uiPollPhase = P_READ_CLASS_DESCRIPTOR_MAX_LUN - 1;
				//if(m_ucAttachedDevice == ATTACHED_FLASH) m_uiPollPhase = P_ENUMERATED_FLASH_SEND_CBW - 1;
				Poll_Go (0x03);
				break;

			case P_READ_CLASS_DESCRIPTOR:
				s_uiKeepAliveCountDown = KEEP_ALIVE_COUNT;
				GetDescriptor (GET_DESC_CLASS);
				break;

			case P_READ_CLASS_DESCRIPTOR_MAX_LUN:
				s_uiKeepAliveCountDown = KEEP_ALIVE_COUNT;
				m_uiPollPhase = P_READ_MAX_LUN - 1;
				GetDescriptor (GET_DESC_MAX_LUN);
				break;

			case P_ENUMERATED:
				m_flgEnumerated = TRUE;

				if (from_Reset_USB_BUS == 1)
				{
					g_Init_USB_PCL = 1;
					from_Reset_USB_BUS = 0;
				}

				if (rd811 (REG_INT_STATUS) & BIT_INT_DEVICE_STATUS)
        		{
            		m_flgEnumerated = FALSE;
					m_uiPollPhase = P_RESET_USB_BUS;
        		}
        		else
        		{
            		if ((rd811 (REG_INT_STATUS) & BIT_INT_USB_A_STATUS) == BIT_INT_USB_A_STATUS)   // The previous command had finished
            		{
						// Test for NAK
						if ((rd811 (REG_PACKET_STATUS) & BIT_STATUS_NAK) == BIT_STATUS_NAK)
						{
							// Resend the last packet
							wr811 (REG_PID_ENDPOINT, BIT_OUT_PID | m_ucOutEndpoint);
							wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);   // Clear the DONE bit
							if (m_ucData_0_1)
							{
								wr811 (REG_CONTROL, 0x07);
							}
							else
							{
								wr811 (REG_CONTROL, 0x47);
							}
						}
						else
						{
           				    if (m_uiHead == m_uiTail)
							{
								// Keep Alive Signal
								if (s_uiKeepAliveCountDown < 1)
								{
                                    //removed sending keep alive -- made Epson hang up
									s_uiKeepAliveCountDown = KEEP_ALIVE_COUNT;
								}
								else
								{
									s_uiKeepAliveCountDown--;
								}
							}
               				else
               				{
								if (CirLen () >= m_ucOutBufferSize)
                   				{
									//wr811 (REG_BUFFER_LENGTH, VAL_BUFFER_LENGTH);
									wr811 (REG_BUFFER_LENGTH, m_ucOutBufferSize);
									pCS1 [USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
									//for (ucIndex = 0; ucIndex < VAL_BUFFER_LENGTH; ucIndex++)
									for (ucIndex = 0; ucIndex < m_ucOutBufferSize; ucIndex++)
									{
										if (m_uiHead != m_uiTail)
                           				{
                   				            pCS1 [USB_HOST_OFF_RW] = m_ucCircularBuffer [m_uiHead];
                   				            m_uiHead = IncCirIndex (m_uiHead);
                           				}
                           				else
                           				{
				                            pCS1 [USB_HOST_OFF_RW] = 0;
                           				}
                       				}
                   				}
                   				else
                   				{
			                        wr811 (REG_BUFFER_LENGTH, CirLen());
               				        pCS1 [USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
			                        while (m_uiHead != m_uiTail)
               				        {
               				            pCS1 [USB_HOST_OFF_RW] = m_ucCircularBuffer [m_uiHead];
               				            m_uiHead = IncCirIndex (m_uiHead);
               				        }
                   				}
								wr811 (REG_PID_ENDPOINT, BIT_OUT_PID | m_ucOutEndpoint);   // Write an out packet to endpoint 2
			                    wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);   // Clear the DONE bit

               				    if (m_ucData_0_1)
			                    {
			                        wr811 (REG_CONTROL, 0x47);
			                        m_ucData_0_1 = 0;
			                    }
			                    else
			                    {
			                        wr811 (REG_CONTROL, 0x07);
			                        m_ucData_0_1 = 1;
			                    }
								s_uiKeepAliveCountDown = KEEP_ALIVE_COUNT;
			                }
       					}
					}

					//if (rd811 (REG_INT_STATUS) & BIT_INT_USB_B_STATUS == BIT_INT_USB_B_STATUS)
					//{
					//	if (m_ucMultiplier > 99)
					//	{
					//		wr811 (REG_PID_ENDPOINT_B, BIT_IN_PID | m_ucInEndpoint);
					//		wr811 (REG_INT_STATUS, BIT_INT_USB_B_STATUS);
					//		m_ucData_0_1_B = 1;
					//		if (m_ucData_0_1_B)
					//		{
					//			wr811 (REG_CONTROL_B, 0x43);
					//			m_ucData_0_1_B = 0;
					//		}
					//		else
					//		{
					//			wr811 (REG_CONTROL_B, 0x03);
					//			m_ucData_0_1_B = 1;
					//		}
					//		m_ucMultiplier = 0;
					//	}
					//	m_ucMultiplier++;
					//}
       			}
				break;

			case P_READ_MAX_LUN:
				m_ucMaxLun = m_ucDescriptor[0];
				//printf("Out Endpoint: %u, Size: %u; In Endpoint: %u, Size: %u\n", m_ucOutEndpoint, m_ucOutBufferSize, m_ucInEndpoint, m_ucInBufferSize);
				m_flgEnumerated = FALSE;
				m_uiPollPhase = P_RESET_USB_BUS;
				//m_uiPollPhase = P_ENUMERATED_FLASH_SEND_CBW;
				break;

			case P_ENUMERATED_FLASH_SEND_CBW:
				if(rd811(REG_INT_STATUS) & BIT_INT_DEVICE_STATUS){
            		m_flgEnumerated = FALSE;
					m_uiPollPhase = P_RESET_USB_BUS;
        		}else{
        			if((rd811(REG_INT_STATUS) & BIT_INT_USB_A_STATUS) == BIT_INT_USB_A_STATUS){   // The previous command had finished
						// Test for NAK
						if((rd811(REG_PACKET_STATUS) & BIT_STATUS_NAK) == BIT_STATUS_NAK){
							// Resend the last packet
							wr811(REG_PID_ENDPOINT, BIT_OUT_PID | m_ucOutEndpoint);
							wr811(REG_INT_STATUS, BIT_INT_USB_A_STATUS);   // Clear the DONE bit
							if(m_ucData_0_1){
								wr811(REG_CONTROL, 0x07);
							}else{
								wr811(REG_CONTROL, 0x47);
							}
						}else{
							if(m_boolCircularBufferValid){
								if(m_uiHead != m_uiTail){
									if(CirLen() >= m_ucOutBufferSize){
										wr811(REG_BUFFER_LENGTH, m_ucOutBufferSize);
										pCS1[USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
										for(ucIndex=0; ucIndex<m_ucOutBufferSize; ucIndex++){
											if(m_uiHead != m_uiTail){
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
									wr811(REG_PID_ENDPOINT, BIT_OUT_PID | m_ucOutEndpoint);   // Write an out packet to endpoint 2
			                    	wr811(REG_INT_STATUS, BIT_INT_USB_A_STATUS);   // Clear the DONE bit

               				    	if(m_ucData_0_1){
			                	        wr811(REG_CONTROL, 0x47);
			            	            m_ucData_0_1 = 0;
			        	            }else{
				                        wr811(REG_CONTROL, 0x07);
				                        m_ucData_0_1 = 1;
				                    }

               				    	m_uiPollPhase = P_ENUMERATED_FLASH_SEND_DATA;
								}
							}
						}
					}
        		}
				break;

			case P_ENUMERATED_FLASH_SEND_DATA:
				break;

			case P_ENUMERATED_FLASH_RECEIVE_DATA:
				break;

			case P_ENUMERATED_FLASH_RECEIVE_CSW:
				break;

			//==================================================================================================================================================

			case P_POLL_DELAY:
				if (m_uiCountDown != 0) m_uiCountDown--;
				if (m_uiCountDown == 0)	m_uiPollPhase = m_uiNextPollPhase;
				break;

			case P_POLL_INT_USB_A:
				if (m_uiCountDown != 0) m_uiCountDown--;
				if ((rd811 (REG_INT_STATUS) & BIT_INT_USB_A_STATUS) == BIT_INT_USB_A_STATUS)
				{
					// Test for NAK
					if ((rd811 (REG_PACKET_STATUS) & BIT_STATUS_NAK) == BIT_STATUS_NAK)
					{
						// Resend the last packet
						wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
						wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);   // Clear the DONE bit
						if (m_ucData_0_1)
						{
							wr811 (REG_CONTROL, 0xBF & m_ucLastCommand);
						}
						else
						{
							wr811 (REG_CONTROL, 0x40 | m_ucLastCommand);
						}
						m_uiCountDown = WAIT_FOR_INTA_TIMEOUT;
					}
					else
					{
						m_uiPollPhase = m_uiNextPollPhase;
					}
				}
				else
				{
					if (m_uiCountDown == 0)
					{
						m_uiPollPhase = P_RESET_USB_BUS;
					}
				}
				break;

			case P_SEND_IN_PID:
				if (m_flgSendFirstINPID)
				{
					m_flgSendFirstINPID = FALSE;
					m_ucDescriptorSize = 0;
					m_ucDescriptorStartIndex = 0;

					// Send Initial IN PID

					// CHANGED 10/15/2008
					//wr811 (REG_BUFFER_LENGTH, 8);
					wr811 (REG_BUFFER_LENGTH, m_ucMaxPacketSize);

					m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
					wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);

					m_ucData_0_1 = 0;

					ucCommand = BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN;
					if (m_ucData_0_1)
					{
						wr811 (REG_CONTROL, 0x40 | ucCommand);
						m_ucData_0_1 = 0;
					}
					else
					{
						wr811 (REG_CONTROL, 0xBF & ucCommand);
						m_ucData_0_1 = 1;
					}
					m_ucLastCommand = ucCommand;
					m_uiCountDown = WAIT_FOR_INTA_TIMEOUT;
					m_uiNextPollPhase = m_uiPollPhase;
					m_uiPollPhase = P_POLL_INT_USB_A;
				}
				else
				{
//					ucStatus = rd811 (REG_PACKET_STATUS);
//					ucLength = rd811 (REG_BUFFER_LENGTH);
					ucTransferCount = rd811(REG_BUFFER_LENGTH) - rd811 (REG_TRANSFER_COUNT);

					// Read the size of the descriptor
//					if (m_ucDescriptorSize == 0) m_ucDescriptorSize = rd811 (VAL_BUFFER_OFFSET);
//
//					if (m_ucDescriptorSize != m_ucDescriptorStartIndex)
//					{
//						i = 0;
//						do
//						{
//							m_ucDescriptor[m_ucDescriptorStartIndex] = rd811 (VAL_BUFFER_OFFSET + i);
//							m_ucDescriptorStartIndex++;
//							i++;
//						} while ((m_ucDescriptorStartIndex < m_ucDescriptorSize) && (i < 8));
//					}

					if (ucTransferCount > 0)
					{
						for (i = 0; i < ucTransferCount; i++)
						{
							m_ucDescriptor[m_ucDescriptorStartIndex] = rd811 (VAL_BUFFER_OFFSET + i);
							m_ucDescriptorStartIndex++;
						}
					}

//					printf("Transfer Count: %u, DescriptorStartIndex: %u\n", ucTransferCount, m_ucDescriptorStartIndex);

//					if (m_ucDescriptorSize == m_ucDescriptorStartIndex)

					//if ((ucTransferCount == 0) || (ucTransferCount < 8))
					// CHANGED 10/15/2008
					if ((ucTransferCount == 0) || (ucTransferCount < m_ucMaxPacketSize))
					{
						// Send Final OUT PID
						wr811 (REG_BUFFER_LENGTH, 0);
						m_ucLastPID_ENDPOINT = BIT_OUT_PID | BIT_EP0;
						wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);

						ucCommand = BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT;
						m_ucData_0_1 = 1;
						if (m_ucData_0_1)
						{
							wr811 (REG_CONTROL, 0x40 | ucCommand);
							m_ucData_0_1 = 0;
						}
						else
						{
							wr811 (REG_CONTROL, 0xBF & ucCommand);
							m_ucData_0_1 = 1;
						}
						m_ucLastCommand = ucCommand;
						m_uiCountDown = WAIT_FOR_INTA_TIMEOUT;
						m_uiNextPollPhase = m_uiDescriptorFinishPhase;
						m_uiPollPhase = P_POLL_INT_USB_A;
					}
					else
					{
						// Send subsequent IN PID
						// CHANGED 10/15/2008
						//wr811 (REG_BUFFER_LENGTH, 8);
						wr811 (REG_BUFFER_LENGTH, m_ucMaxPacketSize);

						m_ucLastPID_ENDPOINT = BIT_IN_PID | BIT_EP0;
						wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);

						ucCommand = BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_IN;
						if (m_ucData_0_1)
						{
							wr811 (REG_CONTROL, 0x40 | ucCommand);
							m_ucData_0_1 = 0;
						}
						else
						{
							wr811 (REG_CONTROL, 0xBF & ucCommand);
							m_ucData_0_1 = 1;
						}
						m_ucLastCommand = ucCommand;
						m_uiCountDown = WAIT_FOR_INTA_TIMEOUT;
						m_uiNextPollPhase = m_uiPollPhase;
						m_uiPollPhase = P_POLL_INT_USB_A;
					}
				}
				break;

			case P_POLL_STALL:
				break;

			default:
				break;
		}
	}
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: usb_write                                                                                 */
/*       Input: Null terminated string to send to the USB printer                                         */
/*      Output: None                                                                                      */
/* Description: Places data into the circular buffer for the polled routine to send to the USB printer    */
/*                                                                                                        */
/**********************************************************************************************************/
/*void usb_write (char *cData)
 - {
 -     unsigned short int uiLen, uiNextIndex;
 -
 -     if (m_flgEnumerated)                                // Only load circular buffer if device is connected
 -     {
 -         uiLen = strlen (cData);
 -         while (uiLen > 0)
 -         {
 -             do
 -             {
 -                 uiNextIndex = IncCirIndex (m_uiTail);
 -             } while (uiNextIndex == m_uiHead);          // Loop until circular buffer is not full
 -
 -             m_ucCircularBuffer [m_uiTail] = *cData++;   // Load one character into circular buffer
 -             m_uiTail = IncCirIndex (m_uiTail);          // Move the tail to the next position
 -             uiLen--;                                    // Countdown remaining bytes left
 -         }
 -     }
 - }*/

short bytes_remaining(void)
{

    return m_uiUsbWriteBytesRemaining;
}

void usb_write (char *cData)
{
    //ushort uiLen, uiNextIndex;
    //ushort uiTimeoutCount, uiInitialCount, uiCurrentCount;
    long uiLen, uiNextIndex;
    long uiTimeoutCount, uiInitialCount, uiCurrentCount;
    bool flgStartTimeout, flgRollover;

    if (!m_flgEnumerated)            // Only load circular buffer if device is connected
    {

         m_uiUsbWriteBytesRemaining = strlen(cData);
         return;
     }

    uiLen = strlen (cData);
    while (uiLen > 0)
    {
        flgStartTimeout = FALSE;
        do
        {
            service_watchdog();
            uiNextIndex = IncCirIndex (m_uiTail);

            if (uiNextIndex == m_uiHead)
            {
                if (!m_flgEnumerated)
                {
                    m_uiUsbWriteBytesRemaining = uiLen;
                    return;
                }

                if (flgStartTimeout)
                {
                    uiCurrentCount = get_minute_counter();

                    if (flgRollover)
                    {
                        if ((uiCurrentCount >= uiTimeoutCount) && (uiCurrentCount < uiInitialCount))
                        {
                            m_uiUsbWriteBytesRemaining = uiLen;
                            return;
                        }
                    }
                    else
                    {
                        if ((uiCurrentCount >= uiTimeoutCount) || (uiCurrentCount < uiInitialCount))
                        {
                            m_uiUsbWriteBytesRemaining = uiLen;
                            return;
                        }
                    }
                }
                else
                {
                    flgStartTimeout = TRUE;
                    uiInitialCount = get_minute_counter();
                    uiTimeoutCount = uiInitialCount + USB_WRITE_TIMEOUT;
                    if (uiTimeoutCount >= 60000)
                    {
                        uiTimeoutCount -= 60000;
                        flgRollover = TRUE;
                    }
                    else
                    {
                        flgRollover = FALSE;
                    }
                }
            }
        } while (uiNextIndex == m_uiHead);          // Loop until circular buffer is not full

        m_ucCircularBuffer [m_uiTail] = *cData++;   // Load one character into circular buffer
        m_uiTail = IncCirIndex (m_uiTail);          // Move the tail to the next position
        uiLen--;                                    // Countdown remaining bytes left
    }
    m_uiUsbWriteBytesRemaining = 0;
}

int SCSI_Inquiry(void){
	return 0;
}

int SCSI_TestUnitReady(void){
	return 0;
}

int SCSI_ReadCapacity(void){
	return 0;
}

int SCSI_ModeSense(void){
	return 0;
}

int SCSI_RequestSense(void){
	return 0;
}

int SCSI_Read(void){
	return 0;
}

int SCSI_Write(void){
	return 0;
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
/*void usb_write_num (char *cData, short num)
 - {
 -     ushort uiLen, uiNextIndex;
 -
 -     if (m_flgEnumerated)                                // Only load circular buffer if device is connected
 -     {
 -         uiLen = num;
 -         while (uiLen > 0)
 -         {
 -             do
 -             {
 -                 uiNextIndex = IncCirIndex (m_uiTail);
 -             } while (uiNextIndex == m_uiHead);          // Loop until circular buffer is not full
 -
 -             m_ucCircularBuffer [m_uiTail] = *cData++;   // Load one character into circular buffer
 -             m_uiTail = IncCirIndex (m_uiTail);          // Move the tail to the next position
 -             uiLen--;                                    // Countdown remaining bytes left
 -         }
 -     }
 - }*/

void usb_write_num (char *cData, short num)
{
    //unsigned short int uiLen, uiNextIndex;
    //unsigned short int uiTimeoutCount, uiInitialCount, uiCurrentCount;
    long uiLen, uiNextIndex;
    long uiTimeoutCount, uiInitialCount, uiCurrentCount;
    bool flgStartTimeout, flgRollover;

    if (!m_flgEnumerated)            // Only load circular buffer if device is connected
    {

         m_uiUsbWriteBytesRemaining = num;
         return;
     }

    uiLen = num;
    while (uiLen > 0)
    {
        flgStartTimeout = FALSE;
        do
        {

            service_watchdog();
            uiNextIndex = IncCirIndex (m_uiTail);

            if (uiNextIndex == m_uiHead)
            {
                if (!m_flgEnumerated)
                {
                    m_uiUsbWriteBytesRemaining = uiLen;
                    return;
                }

                if (flgStartTimeout)
                {
                    uiCurrentCount = get_minute_counter();

                    if (flgRollover)
                    {
                        if ((uiCurrentCount >= uiTimeoutCount) && (uiCurrentCount < uiInitialCount))
                        {
                            m_uiUsbWriteBytesRemaining = uiLen;
                            return;
                        }
                    }
                    else
                    {
                        if ((uiCurrentCount >= uiTimeoutCount) || (uiCurrentCount < uiInitialCount))
                        {
                            m_uiUsbWriteBytesRemaining = uiLen;
                            return;
                        }
                    }
                }
                else
                {
                    flgStartTimeout = TRUE;
                    uiInitialCount = get_minute_counter();
                    uiTimeoutCount = uiInitialCount + USB_WRITE_TIMEOUT;
                    if (uiTimeoutCount >= 60000)
                    {
                        uiTimeoutCount -= 60000;
                        flgRollover = TRUE;
                    }
                    else
                    {
                        flgRollover = FALSE;
                    }
                }
            }
        } while (uiNextIndex == m_uiHead);          // Loop until circular buffer is not full

        m_ucCircularBuffer [m_uiTail] = *cData++;   // Load one character into circular buffer
        m_uiTail = IncCirIndex (m_uiTail);          // Move the tail to the next position
        uiLen--;                                    // Countdown remaining bytes left
    }
    m_uiUsbWriteBytesRemaining = 0;
}





void Poll_Go(uchar ucCommand)
{
	wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);                   // clear the DONE bit
	if (m_ucData_0_1)
	{
		wr811 (REG_CONTROL, 0x40 | ucCommand);                     // start the operation
		m_ucData_0_1 = 0;
	}
	else
	{
		wr811 (REG_CONTROL, 0xBF & ucCommand);                     // start the operation
		m_ucData_0_1 = 1;
	}

	m_ucLastCommand = ucCommand;
	m_uiCountDown = WAIT_FOR_INTA_TIMEOUT;
	m_uiNextPollPhase = m_uiPollPhase + 1;
	m_uiPollPhase = P_POLL_INT_USB_A;
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: wr811                                                                                     */
/*       Input: Register, Value                                                                           */
/*      Output: None                                                                                      */
/* Description: Writes a value to a sl811 register                                                        */
/*                                                                                                        */
/**********************************************************************************************************/
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
/* Description: This function returns the m_flgEnumerated, which is the variable used to determine a USB  */
/*              printer                                                                                   */
/*                                                                                                        */
/**********************************************************************************************************/
bool IsUSBPrinterConnected (void)
{
    return m_flgEnumerated;
}

void PollDelay(ushort uiTenMSCountDown)
{
	m_uiCountDown = uiTenMSCountDown;
	m_uiNextPollPhase = m_uiPollPhase + 1;
	m_uiPollPhase = P_POLL_DELAY;
}

void PollDelayJump(ushort uiTenMSCountDown, ushort uiNextPollPhase)
{
	m_uiCountDown = uiTenMSCountDown;
	m_uiNextPollPhase = uiNextPollPhase;
	m_uiPollPhase = P_POLL_DELAY;
}

void AdvanceToNextPhase(void)
{
	if ((m_uiPollPhase & 0x8000) == 0x0000) // Only advance phase for phases 0 to 7FFF, we are reserving 8000 to FFFF for delays or conditional waits
	{
		m_uiPollPhase++;
	}
}

void GetDescriptor(uchar *pucData)
{
	uchar ucCommand;

	// SL811 initialization
	wr811 (REG_BUFFER_ADDRESS, VAL_BUFFER_OFFSET);  // start of SETUP/IN internal data buffer
	wr811 (REG_BUFFER_LENGTH, 0x08);                // reserve 8 bytes
	wr811buffer(pucData, 8);
	m_ucLastPID_ENDPOINT = BIT_SETUP_PID | BIT_EP0;
	wr811 (REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);
	m_ucData_0_1 = 0;

	ucCommand = BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT;

	wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);                   // clear the DONE bit
	if (m_ucData_0_1)
	{
		wr811 (REG_CONTROL, 0x40 | ucCommand);                     // start the operation
		m_ucData_0_1 = 0;
	}
	else
	{
		wr811 (REG_CONTROL, 0xBF & ucCommand);                     // start the operation
		m_ucData_0_1 = 1;
	}
	m_ucLastCommand = ucCommand;
	m_uiCountDown = WAIT_FOR_INTA_TIMEOUT;
	m_uiDescriptorFinishPhase = m_uiPollPhase + 1;
	m_uiNextPollPhase = P_SEND_IN_PID;
	m_flgSendFirstINPID = TRUE;
	m_uiPollPhase = P_POLL_INT_USB_A;
}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: GetPortStatus                                                                             */
/*       Input: None                                                                                      */
/*      Output: The port status byte of the printer                                                       */
/* Description: This function returns the port status byte of the printer. With this status you can tell  */
/*              1) Out of paper condition                                                                 */
/*              2) Printer Selected                                                                       */
/*              3) Error condition                                                                        */
/*                                                                                                        */
/**********************************************************************************************************/
//uchar GetPortStatus()
//{
//    uchar ucStatus,ucResult;
//
//    m_flgEnablePoll = FALSE;
//    while (rd811 (REG_INT_STATUS) & BIT_INT_USB_A_STATUS == 0){}
//
//    /* wr811 (REG_BUFFER_LENGTH, 0x08);
//    wr811buffer (GET_PORT_STATUS, sizeof(GET_PORT_STATUS));
//    wr811 (REG_PID_ENDPOINT, BIT_SETUP_PID | BIT_EP0);
//    m_ucData_0_1 = 0;
//    ucResult = go (0x07);
//
//    wr811 (REG_BUFFER_LENGTH, 0x01);
//    wr811 (REG_PID_ENDPOINT, BIT_IN_PID | BIT_EP0);
//    m_ucData_0_1 = 0;
//    ucResult = go (0x03);
//
//    wr811 (REG_BUFFER_LENGTH, 0x00);
//    wr811 (REG_PID_ENDPOINT, BIT_OUT_PID | BIT_EP0);
//    ucResult = go (0x07);
//
//    ucStatus = rd811 (VAL_BUFFER_OFFSET); */
//
//    wr811 (REG_BUFFER_LENGTH, 0x01);
//    wr811 (REG_PID_ENDPOINT, BIT_IN_PID | 0x01);
//    ucResult = go (0x03);
//    ucStatus = rd811 (VAL_BUFFER_OFFSET);
//
//    if (m_ucData_0_1) m_ucData_0_1 = 0;
//    else m_ucData_0_1 = 1;
//
//    m_flgEnablePoll = TRUE;
//
//    printf("status: %x\r\n",ucStatus);
//    return ucStatus;
//}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: IsUSBPrinterPaperEmpty                                                                    */
/*       Input: None                                                                                      */
/*      Output: Returns TRUE, if there is no paper in the printer                                         */
/* Description: Uses GetPortStatus and looks at bit 5 to determine the paper status                       */
/*                                                                                                        */
/**********************************************************************************************************/
//bool IsUSBPrinterPaperEmpty (void)
//{
//    uchar ucStatus;
//
//    if (m_flgEnumerated)
//    {
//        ucStatus = GetPortStatus ();
//
//        if (ucStatus & BIT_PORT_STATUS_EMPTY)
//            return TRUE;
//        else
//            return FALSE;
//    }
//    else
//    {
//        return FALSE;
//    }
//}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: IsUSBPrinterError                                                                         */
/*       Input: None                                                                                      */
/*      Output: Returns TRUE, if there is a printer error                                                 */
/* Description: Uses GetPortStatus and looks at bit 3 to determine an error. If the bit is set, then      */
/*              there is no error.                                                                        */
/*                                                                                                        */
/**********************************************************************************************************/
//bool IsUSBPrinterError (void)
//{
//    uchar ucStatus;
//
//    if (m_flgEnumerated)
//    {
//        ucStatus = GetPortStatus ();
//
//        if (ucStatus & BIT_PORT_STATUS_NOT_ERROR)
//            return FALSE;
//        else
//            return TRUE;
//    }
//    else
//    {
//        return FALSE;
//    }
//}

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: IsUSBPrinterSelected                                                                      */
/*       Input: None                                                                                      */
/*      Output: Returns TRUE, if the printer is selected                                                  */
/* Description: Uses GetPortStatus and looks at bit 4 to determine the selection status of the printer    */
/*                                                                                                        */
/**********************************************************************************************************/
//bool IsUSBPrinterSelected (void)
//{
//    uchar ucStatus;
//
//    if (m_flgEnumerated)
//    {
//        ucStatus = GetPortStatus ();
//
//
//        if (ucStatus & BIT_PORT_STATUS_SELECT)
//            return TRUE;
//        else
//            return FALSE;
//    }
//    else
//    {
//        return TRUE;
//    }
//}

//    uchar ucIndex;
//
//    if (m_flgEnumerated)                            // Only write OUTS after usb has been configured
//    {
//        if (rd811 (REG_INT_STATUS) & BIT_INT_USB_A_STATUS == BIT_INT_USB_A_STATUS)   // The previous command had finished
//        {
//            wr811 (REG_BUFFER_LENGTH, VAL_BUFFER_LENGTH);
//            pCS1 [USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
//            for (ucIndex = 0; ucIndex < VAL_BUFFER_LENGTH; ucIndex++)
//            {
//                if (m_uiHead != m_uiTail)
//                {
//                    pCS1 [USB_HOST_OFF_RW] = m_ucCircularBuffer [m_uiHead];
//                    m_uiHead = IncCirIndex (m_uiHead);
//                }
//                else
//                {
//                    pCS1 [USB_HOST_OFF_RW] = 0;
//                }
//            }
//            wr811 (REG_PID_ENDPOINT, BIT_OUT_PID | 0x02);   // Write an out packet to endpoint 2
//            wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);   // Clear the DONE bit
//            wr811 (REG_CONTROL, 0x07);
//        }
//    }

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: ReinitializeSL811H                                                                        */
/*       Input: None                                                                                      */
/*      Output: None                                                                                      */
/* Description: This function initializes the USB printer if there is one. This version is to be executed */
/*              from the foreground and not inside an interrupt, because it contains delays               */
/*                                                                                                        */
/**********************************************************************************************************/
//void ReinitializeSL811H (void)
//{
//    uchar ucResult;
//    //bool flgFullSpeed;
//    // unsigned short int uiIndex;
//
//    if (! m_flgEnumerated)
//    {
//        // Clear USB-A Registers
//        wr811 (REG_CONTROL, 0x00);
//        wr811 (REG_BUFFER_ADDRESS, 0x00);
//        wr811 (REG_BUFFER_LENGTH, 0x00);
//        wr811 (REG_PID_ENDPOINT, 0x00);
//        wr811 (REG_DEVICE_ADDRESS, 0x00);
//        wr811 (REG_CONTROL_1, 0x00);
//        wr811 (REG_INT_ENABLE, 0x00);
//        wr811 (REG_INT_STATUS, 0xFF);
//        wr811 (REG_SOF_COUNT_LO, 0x00);
//        wr811 (REG_SOF_COUNT_HI, 0x00);
//
//        // Clear USB-B Registers not used but initialized
//        wr811 (0x08, 0x00);
//        wr811 (0x09, 0x00);
//        wr811 (0x0A, 0x00);
//        wr811 (0x0B, 0x00);
//        wr811 (0x0C, 0x00);
//
//        /******************************************************************************************************/
//        /* Test for connected device and device speed                                                         */
//        /******************************************************************************************************/
//
//        wr811 (REG_SOF_COUNT_HI, 0xAE);         // Set SOF high counter, no change D+/D-, host mode
//
//        // Reset USB Bus
//        wr811 (REG_CONTROL_1, USB_RESET);                           // Reset USB engine, full-speed setup, suspend disable
//        delay_msec (100);                                           // Delay for HW stablize
//        wr811 (REG_CONTROL_1, USB_OPERATE_HI_SPEED_WITHOUT_SOF);    // Set to normal operation
//
//        wr811 (REG_INT_ENABLE, BIT_INT_DEVICE_ENABLE | BIT_INT_INSERT_ENABLE | BIT_INT_USB_A_ENABLE);
//        wr811 (REG_INT_STATUS, 0xFF);           // Clear Interrupt enable status
//        delay_msec (100);                       // Delay for HW stablize
//
//        if (rd811 (REG_INT_STATUS) & BIT_INT_DEVICE_STATUS)
//        {
//            wr811 (REG_INT_STATUS, 0xFF);       // Clear Interrupt enable status
//            return;
//        }
//
//        // Checking full or low speed
//        if ((rd811 (REG_INT_STATUS) & BIT_INT_D_PLUS_STATUS) == 0)
//        {                                       // ** Low Speed is detected ** //
//            wr811 (REG_SOF_COUNT_HI, 0xEE);     // Set up host and low speed direct and SOF cnt
//            wr811 (REG_SOF_COUNT_LO, 0xE0);     // SOF Counter Low = 0xE0; 1ms interval
//            wr811 (REG_CONTROL_1, USB_OPERATE_LO_SPEED);
//            //flgFullSpeed = FALSE;               // low speed device flag
//        }
//        else
//        {                                       // ** Full Speed is detected ** //
//            wr811 (REG_SOF_COUNT_HI, 0xAE);     // Set up host & full speed direct and SOF cnt
//            wr811 (REG_SOF_COUNT_LO, 0xE0);     // SOF Counter Low = 0xE0; 1ms interval
//            wr811 (REG_CONTROL_1, USB_OPERATE_HI_SPEED);
//            //flgFullSpeed = TRUE;
//        }
//
//        wr811 (REG_PID_ENDPOINT, BIT_SOF_PID | BIT_EP0);
//        wr811 (REG_DEVICE_ADDRESS, 0x00);       // set device address to zero
//        wr811 (REG_CONTROL, 0x01);              // start generate SOF or EOP
//
//        delay_msec (100);                       // Hub required approx. 24.1mS
//        wr811 (REG_INT_STATUS, 0xFF);           // Clear Interrupt status
//        /******************************************************************************************************/
//
//        // SL811 initialization
//        wr811 (REG_BUFFER_ADDRESS, VAL_BUFFER_OFFSET);  // start of SETUP/IN internal data buffer
//        wr811 (REG_BUFFER_LENGTH, 0x08);                // reserve 8 bytes
//
//        // Issue a SET_ADDRESS USB request, setting the peripheral address to 1
//        wr811buffer (SET_ADDRESS_TO_1, sizeof (SET_ADDRESS_TO_1));
//        wr811 (REG_PID_ENDPOINT, BIT_SETUP_PID | BIT_EP0);          // SETUP PID, EP0
//        m_ucData_0_1 = 0;
//        ucResult = go (0x07);                                       // DIREC=1(out), ENAB=1, ARM=1
//
//        // STATUS stage is a no-data IN to EP0
//        wr811 (REG_PID_ENDPOINT, BIT_IN_PID | BIT_EP0);             // IN PID, EP0
//        m_ucData_0_1 = 0;
//        ucResult = go (0x03);                                       // Don't sync to SOF, DIREC=0(in), ENAB, ARM
//
//        // Send a CONTROL transfer to select configuration #1.
//        wr811buffer (SET_CONFIGURATION_TO_1, sizeof (SET_CONFIGURATION_TO_1));
//        wr811 (REG_DEVICE_ADDRESS, 0x01);                           // now talking to USB device at address 1
//        wr811 (REG_PID_ENDPOINT, BIT_SETUP_PID | BIT_EP0);          // OR in the endpoint (zero)
//        m_ucData_0_1 = 0;
//        ucResult = go (0x07);                                       // DIREC=1(out), ENAB=1, ARM=1
//
//        // STATUS stage is a no-data IN to EP0
//        wr811 (REG_PID_ENDPOINT, BIT_IN_PID | BIT_EP0);              // IN PID, EP0
//        m_ucData_0_1 = 0;
//        ucResult = go (0x03);                                        // Don't sync to SOF, DIREC=0(in), ENAB, ARM
//
//        /* wr811buffer (MAGIC_CODES2, sizeof(MAGIC_CODES2));
//        wr811 (REG_PID_ENDPOINT, BIT_SETUP_PID | BIT_EP0);
//        m_ucData_0_1 = 0;
//        ucResult = go (0x07);
//        m_ucData_0_1 = 0;
//        for (uiIndex = 0; uiIndex < 3; uiIndex++)
//        {
//            wr811 (REG_PID_ENDPOINT, BIT_IN_PID | BIT_EP0);
//            ucResult = go (0x03);
//        }
//        wr811 (REG_BUFFER_LENGTH, 0);
//        wr811 (REG_PID_ENDPOINT, BIT_OUT_PID | BIT_EP0);
//        ucResult = go (0x07);
//
//        wr811 (REG_BUFFER_LENGTH, 0x08);
//        wr811buffer (MAGIC_CODES, sizeof(MAGIC_CODES));
//        wr811 (REG_PID_ENDPOINT, BIT_SETUP_PID |BIT_EP0);
//        m_ucData_0_1 = 0;
//        ucResult = go (0x07);
//        m_ucData_0_1 = 0;
//        for (uiIndex = 0; uiIndex < 24; uiIndex++)
//        {
//            wr811 (REG_PID_ENDPOINT, BIT_IN_PID | BIT_EP0);
//            ucResult = go (0x03);
//        }
//        wr811 (REG_BUFFER_LENGTH, 0);
//        wr811 (REG_PID_ENDPOINT, BIT_OUT_PID | BIT_EP0);
//        ucResult = go (0x07); */
//
//        delay_msec (100);
//        m_ucData_0_1 = 0;
//        m_flgEnumerated = TRUE;
//    }
//}

//	POLLED CODE TO SEND DATA TO PRINTER
//	uchar ucIndex;
//    if (m_flgEnumerated && m_flgEnablePoll)                            // Only write OUTS after usb has been configured
//    {
//        if (rd811 (REG_INT_STATUS) & BIT_INT_DEVICE_STATUS)
//        {
//            m_flgEnumerated = FALSE;
//        }
//        else
//        {
//            if (rd811 (REG_INT_STATUS) & BIT_INT_USB_A_STATUS == BIT_INT_USB_A_STATUS)   // The previous command had finished
//            {
//				// Test for NAK
//				if ((rd811 (REG_PACKET_STATUS) & BIT_STATUS_NAK) == BIT_STATUS_NAK)
//				{
//					// Resend the last packet
//					wr811 (REG_PID_ENDPOINT, BIT_OUT_PID | 0x02);   // Write an out packet to endpoint 2
//					wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);   // Clear the DONE bit
//					if (m_ucData_0_1)
//					{
//						wr811 (REG_CONTROL, 0x07);
//					}
//					else
//					{
//						wr811 (REG_CONTROL, 0x47);
//					}
//				}
//				else
//				{
//                if (m_uiHead == m_uiTail)
//                {
//                   /* wr811 (REG_BUFFER_LENGTH, VAL_BUFFER_LENGTH);
//                    pCS1 [USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
//                    for (ucIndex = 0; ucIndex < VAL_BUFFER_LENGTH; ucIndex++)
//                    {
//                        pCS1 [USB_HOST_OFF_RW] = 0;
//                    }
//                    wr811 (REG_PID_ENDPOINT, BIT_OUT_PID | 0x02);   // Write an out packet to endpoint 2
//                    wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);   // Clear the DONE bit
//                    if (m_ucData_0_1)
//                    {
//                        wr811 (REG_CONTROL, 0x47);
//                        m_ucData_0_1 = 0;
//                    }
//                    else
//                    {
//                        wr811 (REG_CONTROL, 0x07);
//                        m_ucData_0_1 = 1;
//                    } */
//                }
//                else
//                {
//                    if (CirLen () >= VAL_BUFFER_LENGTH)
//                    {
//                        wr811 (REG_BUFFER_LENGTH, VAL_BUFFER_LENGTH);
//                        pCS1 [USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
//                        for (ucIndex = 0; ucIndex < VAL_BUFFER_LENGTH; ucIndex++)
//                        {
//                            if (m_uiHead != m_uiTail)
//                            {
//                                pCS1 [USB_HOST_OFF_RW] = m_ucCircularBuffer [m_uiHead];
//                                m_uiHead = IncCirIndex (m_uiHead);
//                            }
//                            else
//                            {
//                            pCS1 [USB_HOST_OFF_RW] = 0;
//                            }
//                        }
//                    }
//                    else
//                    {
//                        wr811 (REG_BUFFER_LENGTH, CirLen());
//                        pCS1 [USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
//                        while (m_uiHead != m_uiTail)
//                        {
//                            pCS1 [USB_HOST_OFF_RW] = m_ucCircularBuffer [m_uiHead];
//                            m_uiHead = IncCirIndex (m_uiHead);
//                        }
//                    }
//
//                    wr811 (REG_PID_ENDPOINT, BIT_OUT_PID | 0x02);   // Write an out packet to endpoint 2
//                    wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);   // Clear the DONE bit
//                    if (m_ucData_0_1)
//                    {
//                        wr811 (REG_CONTROL, 0x47);
//                        m_ucData_0_1 = 0;
//                    }
//                    else
//                    {
//                        wr811 (REG_CONTROL, 0x07);
//                        m_ucData_0_1 = 1;
//                    }
//                }
//            }
//			}
//        }
//    }

/**********************************************************************************************************/
/*                                                                                                        */
/*    Function: go                                                                                        */
/*       Input: Value to the Control Register                                                             */
/*      Output: Packet Status                                                                             */
/* Description: This function sends a packet to the device. This function executes in the foreground,     */
/*              because it contains delays and loops until the sl811 returns with a status.               */
/*                                                                                                        */
/**********************************************************************************************************/
//uchar go (uchar ucCommand)                                          // Launch an 811 operation.
//{
//    wr811 (REG_INT_STATUS, BIT_INT_USB_A_STATUS);                   // clear the DONE bit
//    if (m_ucData_0_1)
//    {
//        wr811 (REG_CONTROL, 0x40 | ucCommand);                      // start the operation
//        m_ucData_0_1 = 0;
//    }
//    else
//    {
//        wr811 (REG_CONTROL, 0xBF & ucCommand);                      // start the operation
//        m_ucData_0_1 = 1;
//    }
//
//    delay_msec (10);
//
//    while ((rd811 (REG_INT_STATUS) & BIT_INT_USB_A_STATUS) == 0){}    // spin while "done" bit is low
//    return rd811 (REG_PACKET_STATUS);                               // return the status of this transfer
//}


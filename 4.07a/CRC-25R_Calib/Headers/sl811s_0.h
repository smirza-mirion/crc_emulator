//*****************************************************************************************
// Cypress Semiconductor - Customer Design Center 
//*****************************************************************************************
// Project		: (SL811S + Audio Control + Internet Control) Demo Code
// File			: sl811s.h (Include files for SL811S)
// DCE/FAE		: Shawn Ng (cxn)
// Last Update	: 03/01/02
//
//*****************************************************************************************
// Include files
//*****************************************************************************************
#include "ezusb.h"
#include "ezregs.h"

//*****************************************************************************************
// Constant Defines
//*****************************************************************************************
// #define NULL		0
#define TRUE   		1
#define FALSE  		0

//----------------------------------
// I/O Ports Bit Masks
//----------------------------------
#define	ACTIVE_BLINK		0x40	// Port B bit 6 - Active LED, start of Report Descriptor (slave)
#define	ACTIVE_LED			0x40	// Port B bit 6 - Active LED, blink when no device attach (host)
#define	IRQ_INPUT			0x10	// Port B bit 4 - Active high interrupt input from SL811HST
#define	nHOST_SLAVE_MODE	0x02	// Port B bit 1 - f/w control of nHost/Slave pin
#define	nRESET				0x01	// Port B bit 0 - f/w control of nReset pin

#define	PORTX_LED			0x1F	// Turn off all Port C LEDs
#define	PORT0_LED			0x01	// Port C bit 0 - Port #0
#define	PORT1_LED			0x02	// Port C bit 1 - Port #1
#define	PORT2_LED			0x04	// Port C bit 2 - Port #2
#define	PORT3_LED			0x08	// Port C bit 3 - Port #3
#define	PORT4_LED			0x10	// Port C bit 4 - Port #4

#define	AUDIO_KEYS			0x07	// PC2, PC1, PC0
#define	MUTE_KEY			0x01	// Mute Button - (PC0)
#define	VOL_UP_KEY			0x02	// Volume Up button - (PC1)
#define	VOL_DN_KEY			0x04	// Volume Down button - (PC2)

//----------------------------------
// USB Report Bit Mask
//----------------------------------
#define	USB_MUTE			0x01	// Mute Button
#define	USB_VOLUME_UP		0x02	// Volume Up button
#define	USB_VOLUME_DN		0x04	// Volume Down button
#define	USB_PLAY			0x08	// Play button
#define	USB_STOP			0x10	// Stop button (Audio)
#define	USB_BASSB			0x20	// Bass Boost button
#define	USB_SCAN_PREV		0x40	// Scan Previous Track button
#define	USB_SCAN_NEXT		0x80	// Scan Next Track button

#define	USB_AC_SEARCH		0x01	// Search button
#define	USB_AC_HOME			0x02	// Home button
#define	USB_AC_BACK			0x04	// Back button
#define	USB_AC_FORWARD		0x08	// Forward button
#define	USB_AC_STOP			0x10	// Stop button (Broswer)
#define	USB_AC_REFRESH		0x20	// Refresh button
#define	USB_AC_BOOKMARK		0x40	// Bookmark button
#define	USB_AC_EMAIL		0x80	// Email button

//----------------------------------
// SL811S Slave Device Specific
//----------------------------------
#define EP0_LEN 			8		// EP0 MaxPktSize 
#define EP1_LEN 			64		// EP1 MaxPktSize
#define EP2_LEN				64		// EP2 MaxPktSize
#define MAXIFCNUM			8		// Maximum of 8 interfaces
#define TIME_OUT			5		// Task time out

//----------------------------------
// SL811S Registers Address
//----------------------------------
#define CtrlReg         	0x05	// USB Control Registers
#define IntEna         	 	0x06
#define USBAddress			0x07
#define IntStatus       	0x0D
#define cDATASet        	0x0E
#define cSOFcnt         	0x0F

#define EP0AControl    		0x00	// EP0 Endpoint Registers (set A)
#define EP0AAddress    		0x01
#define EP0AXferLen    		0x02
#define EP0AStatus     		0x03
#define EP0ACounter    		0x04

#define EP1AControl    		0x10	// EP1 Endpoint Registers (set A)
#define EP1AAddress    		0x11
#define EP1AXferLen    		0x12
#define EP1AStatus     		0x13
#define EP1ACounter    		0x14

#define EP2AControl    		0x20	// EP2 Endpoint Registers (set A)
#define EP2AAddress    		0x21
#define EP2AXferLen    		0x22
#define EP2AStatus     		0x23
#define EP2ACounter    		0x24

#define EP0A_Slave_Buf		0x40					// Start address for EP0 buffer
#define EP1A_Slave_Buf		EP0A_Slave_Buf+EP0_LEN	// Start address for EP1A buffer
#define EP2A_Slave_Buf		EP1A_Slave_Buf+EP1_LEN  // Start address for EP2A buffer

//----------------------------------
// Interrupt Register Bit Masks
//----------------------------------
#define EP0_DONE			0x01
#define EP1_DONE			0x02
#define EP2_DONE			0x04
#define EP3_DONE			0x08
#define DMA_DONE			0x10
#define SOF_DONE			0x20
#define USB_RESET_DONE		0x40

//----------------------------------
// EPx Status Register Bit Masks
//----------------------------------
#define EP_ACK				0x01
#define EP_ERROR			0x02
#define EP_TIMEOUT			0x04
#define EP_SEQUENCE			0x08
#define EP_SETUP			0x10
#define EP_OVERFLOW			0x20
#define EP_NAK				0x40
#define EP_STALL			0x80

//----------------------------------
// EPx Control Register Bit Mask
//----------------------------------
#define DATA0_OUT    		0x03   	// (Data0 + OUT/SETUP + Enable + Arm)
#define DATA1_OUT    		0x43   	// (Data1 + OUT/SETUP + Enable + Arm)
#define DATA0_IN    		0x07   	// (Data0 + IN + Enable + Arm)
#define DATA1_IN    		0x47   	// (Data1 + IN + Enable + Arm)
#define DATAX	    		0x40   	// Data sequence bits
#define ENABLE_ISO    		0x10   	// Enable ISO on endpoint
#define SEND_STALL    		0x27   	// (STALL + IN + Enable + Arm)
#define DIRECTION 			0x04   	// OUT - '0', receive from host
									// IN  - '1', transmit to host	

#define DATA0_IN_ENABLE		0x06   	// (Data0 + IN + Enable)
#define DATA0_IN_DISABLE	0x04   	// (Data0 + IN + Disable)

//----------------------------------
// Standard Chapter 9 definition
//----------------------------------
#define STD_REQUEST			0x00
#define CLASS_REQUEST		0x01
#define VENDOR_REQUEST		0x02
#define RECIPIENT_DEV		0x00
#define RECIPIENT_IFC		0x01
#define RECIPIENT_ENP		0x02

#define GET_STATUS      	0x00																  
#define CLEAR_FEATURE   	0x01
#define SET_FEATURE    		0x03
#define SET_ADDRESS    		0x05
#define GET_DESCRIPTOR 		0x06
#define SET_DESCRIPTOR 		0x07
#define GET_CONFIG     		0x08
#define SET_CONFIG    		0x09
#define GET_INTERFACE  		0x0A
#define SET_INTERFACE  		0x0B
#define SYNCH_FRAME     	0x0C

#define DEVICE          	0x01
#define CONFIGURATION   	0x02
#define STRING          	0x03
#define INTERFACE       	0x04
#define ENDPOINT        	0x05

#define STDCLASS        	0x00
#define HIDCLASS        	0x03
#define HUBCLASS	 		0x09  

//----------------------------------
// Class Descriptor for HID 
//----------------------------------
#define GET_REPORT      	0x01
#define GET_IDLE        	0x02
#define GET_PROTOCOL    	0x03

#define SET_REPORT      	0x09
#define SET_IDLE        	0x0A
#define SET_PROTOCOL    	0x0B

#define HID_DEV         	0x21
#define HID_REPORT      	0x22
#define HID_PHYSICAL    	0x23

#define INPUT        		0x01
#define OUPUT     	    	0x02
#define FEATURE    	   		0x03

#define NONE	        	0x00
#define KEYBOARD        	0x01
#define MOUSE	        	0x02

//*****************************************************************************************
// Function Prototypes
//*****************************************************************************************

BYTE SL811Read(BYTE a);
void SL811Write(BYTE a, BYTE d);
void SL811BufRead(BYTE addr, BYTE *s, BYTE c);
void SL811BufWrite(BYTE addr, BYTE *s, BYTE c);
void uC8051_init(void);
void sl811s_init(void);
void EP0A_OUT_Arm(BYTE len);
void EP0A_IN_Arm(BYTE buf_adr, BYTE len, BYTE seq);
void EP1A_IN_Arm(BYTE buf_adr, BYTE len, BYTE seq);
void EP2A_OUT_Arm(BYTE buf_adr, BYTE len, BYTE seq);
void ep0_isr(void);
void ep1_isr(void);
void ep2_isr(void);
void sof_isr(void);

//----------------------------------
// Application-Specific functions
//----------------------------------
void audio_key_scan(void);
void internet_key_scan(void);



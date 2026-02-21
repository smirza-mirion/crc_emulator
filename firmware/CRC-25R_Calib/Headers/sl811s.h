#pragma once
//*****************************************************************************************
// Cypress Semiconductor - Customer Design Center 
//*****************************************************************************************
// Project		: (SL811S + Audio Control + Internet Control) Demo Code
// File			: sl811s.h (Include files for SL811S)
// DCE/FAE		: Shawn Ng (cxn)
// Last Update	: 04/07/05  -- Marcia Suzuki
//
//*****************************************************************************************
// Include files
//*****************************************************************************************
#include "ezusb.h"
#include "ezregs.h"

//----------------------------------
// SL811S Slave Device Specific
//----------------------------------
#define EP0_LEN		8		// EP0 MaxPktSize 
#define EP1_LEN		64		// EP1 MaxPktSize
#define EP2_LEN		64		// EP2 MaxPktSize
#define EP3_LEN		56		// EP3 MaxPktSize
#define MAXIFCNUM	8		// Maximum of 8 interfaces
#define TIME_OUT		5		// Task time out

//----------------------------------
// SL811S Registers Address
//----------------------------------
#define CtrlReg			(char)0x05	// USB Control Registers
#define IntEna			(char)0x06
#define USBAddress		(char)0x07
#define IntStatus			(char)0x0D
#define cDATASet			(char)0x0E
#define cSOFcnt			(char)0x0F

#define EP0AControl		(char)0x00	// EP0 Endpoint Registers (set A)
#define EP0AAddress		(char)0x01
#define EP0AXferLen		(char)0x02
#define EP0AStatus		(char)0x03
#define EP0ACounter		(char)0x04

#define EP1AControl		(char)0x10	// EP1 Endpoint Registers (set A)
#define EP1AAddress		(char)0x11
#define EP1AXferLen		(char)0x12
#define EP1AStatus		(char)0x13
#define EP1ACounter		(char)0x14

#define EP2AControl		(char)0x20	// EP2 Endpoint Registers (set A)
#define EP2AAddress		(char)0x21
#define EP2AXferLen		(char)0x22
#define EP2AStatus		(char)0x23
#define EP2ACounter		(char)0x24

#define EP0A_Slave_Buf		(char)0x40					// Start address for EP0 buffer
#define EP1A_Slave_Buf		(char)EP0A_Slave_Buf+EP0_LEN		// Start address for EP1A buffer
#define EP2A_Slave_Buf		(char)EP1A_Slave_Buf+EP1_LEN		// Start address for EP2A buffer
#define EP3A_Slave_Buf		(char)EP2A_Slave_Buf+EP2_LEN		// Start address for EP3A buffer

#define EP3AControl		(char)0x30					// EP3 Endpoint Registers (set A)
#define EP3AAddress		(char)0x31
#define EP3AXferLen		(char)0x32
#define EP3AStatus		(char)0x33
#define EP3ACounter		(char)0x34

//----------------------------------
// Interrupt Register Bit Masks
//----------------------------------
#define EP0_DONE			(char)0x01
#define EP1_DONE			(char)0x02
#define EP2_DONE			(char)0x04
#define EP3_DONE			(char)0x08
#define DMA_DONE		(char)0x10
#define SOF_DONE			(char)0x20
#define USB_RESET_DONE	(char)0x40

//----------------------------------
// EPx Status Register Bit Masks
//----------------------------------
#define EP_ACK			(char)0x01
#define EP_ERROR			(char)0x02
#define EP_TIMEOUT		(char)0x04
#define EP_SEQUENCE		(char)0x08
#define EP_SETUP			(char)0x10
#define EP_OVERFLOW		(char)0x20
#define EP_NAK			(char)0x40
#define EP_STALL			(char)0x80

//----------------------------------
// EPx Control Register Bit Mask
//----------------------------------
#define DATA0_OUT		(char)0x03	// (Data0 + OUT/SETUP + Enable + Arm)
#define DATA1_OUT		(char)0x43	// (Data1 + OUT/SETUP + Enable + Arm)
#define DATA0_IN			(char)0x07	// (Data0 + IN + Enable + Arm)
#define DATA1_IN			(char)0x47	// (Data1 + IN + Enable + Arm)
#define DATAX			(char)0x40	// Data sequence bits
#define ENABLE_ISO		(char)0x10	// Enable ISO on endpoint
#define SEND_STALL		(char)0x27	// (STALL + IN + Enable + Arm)
#define DIRECTION		(char)0x04	// OUT - '0', receive from host
							// IN  - '1', transmit to host	

#define DATA0_IN_ENABLE	(char)0x06	// (Data0 + IN + Enable)
#define DATA0_IN_DISABLE	(char)0x04	// (Data0 + IN + Disable)
#define DATA0_OUT_ENABLE	(char)0x02	// (Data0 + OUT + Enable)
#define DATA0_OUT_DISABLE	(char)0x00	// (Data0 + OUT + Disable)

//----------------------------------
// Standard Chapter 9 definition
//----------------------------------
#define STD_REQUEST		(char)0x00
#define CLASS_REQUEST		(char)0x01
#define VENDOR_REQUEST	(char)0x02
#define RECIPIENT_DEV		(char)0x00
#define RECIPIENT_IFC		(char)0x01
#define RECIPIENT_ENP		(char)0x02

#define GET_STATUS		(char)0x00																  
#define CLEAR_FEATURE		(char)0x01
#define SET_FEATURE		(char)0x03
#define SET_ADDRESS		(char)0x05
#define GET_DESCRIPTOR	(char)0x06
#define SET_DESCRIPTOR	(char)0x07
#define GET_CONFIG		(char)0x08
#define SET_CONFIG		(char)0x09
#define GET_INTERFACE		(char)0x0A
#define SET_INTERFACE		(char)0x0B
#define SYNCH_FRAME		(char)0x0C

#define DEVICE			(char)0x01
#define CONFIGURATION		(char)0x02
#define STRING			(char)0x03
#define INTERFACE		(char)0x04
#define ENDPOINT			(char)0x05

#define STDCLASS			(char)0x00
#define HIDCLASS			(char)0x03
#define HUBCLASS			(char)0x09  

//----------------------------------
// Class Descriptor for HID 
//----------------------------------
#define GET_REPORT		(char)0x01
#define GET_IDLE			(char)0x02
#define GET_PROTOCOL		(char)0x03

#define SET_REPORT		(char)0x09
#define SET_IDLE			(char)0x0A
#define SET_PROTOCOL		(char)0x0B

//----------------------------------
// Class Descriptor for CDC
//----------------------------------
#define SET_CONTROL_LINE_STATE		(char)0x22
#define GET_CONTROL_LINE_CODING		(char)0x21
#define SET_CONTROL_LINE_CODING		(char)0x20

#define HID_DEV		(char)0x21
#define HID_REPORT	(char)0x22
#define HID_PHYSICAL	(char)0x23

#define INPUT		(char)0x01
#define OUPUT		(char)0x02
#define FEATURE		(char)0x03

#define NONE		(char)0x00
#define KEYBOARD		(char)0x01
#define MOUSE		(char)0x02

//routines to read & write USB data
void USB_Get_Data(uchar Input_Array[]);
bool USB_Send_Data(uchar Output_Array[]);
bool USB_Send_Data_num(uchar Output_Array[], short num);
bool USB_Send_Data_LowLevel_num(uchar Output_Array[],short num);
bool is_usb_char_waiting(void);

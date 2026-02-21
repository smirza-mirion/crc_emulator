#pragma once
//*****************************************************************************************
// Cypress Semiconductor - Customer Design Center 
//*****************************************************************************************
// Project		: (SL811S + Audio Control + Internet Control) Demo Code
// File			: descp.h (USB Descriptors ROM code)
// DCE/FAE		: Shawn Ng (cxn)
// Last Update	: 03/01/02
//
//*****************************************************************************************
// Constant specific to USB Slave Descriptors
//*****************************************************************************************
#define DEV_LEN		18			// Device Descriptor Length
#define HID_LEN		9			// HID-Class Descriptor Length
#define CFG_LEN		41			// Configuration Descriptor Length
#define CFG_LEN_CDC	67			// Configuration Descriptor Length
#define REP_LEN		34			// HID Report Descriptor Length

//---------------------------------------------------------------------------------------------------------------------
// DEVICE DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
BYTE	Dev_Descp[] = {	DEV_LEN,			// bLength: Size of descriptor
						0x01,				// bDescriptorType: Device	
						0x10,0x01,			// bcdUSB: USB 1.1
						0x00,				// bDeviceClass: none
						0x00,				// bDeviceSubClass: none
						0x00,				// bDeviceProtocol: none
						EP0_LEN,			// bMaxPacketSize0: 8/64 bytes
						0xB4,0x04,			// idVendor: 0x04B4 - Cypress
						0x11,0x08,			// idProduct: 0x1111 - SL811S
						0x00,0x01,			// bcdDevice: version 1.0
						0x01,				// iManufacturer: Cypress
						0x02,				// iProduct: Cypress SL811S - HID Consumer Control
						0x00,				// iSerialNumber: none
						0x01};				// bNumConfigurations: 1

BYTE	Dev_Descp_CDC[] = {	DEV_LEN,			// bLength: Size of descriptor
							0x01,				// bDescriptorType: Device
							0x10,0x01,			// bcdUSB: USB 1.1
							0x02,				// bDeviceClass: none
							0x00,				// bDeviceSubClass: none
							0x00,				// bDeviceProtocol: none
							EP0_LEN,			// bMaxPacketSize0: 8/64 bytes
							0xB4,0x04,			// idVendor: 0x04B4 - Cypress
							0x11,0x08,			// idProduct: 0x0811 - SL811S
							0x00,0x01,			// bcdDevice: version 1.0
							0x01,				// iManufacturer: Capintec
							0x02,				// iProduct: Capintec
							0x03,				// iSerialNumber: 1234567890
							0x01};				// bNumConfigurations: 1

//---------------------------------------------------------------------------------------------------------------------
// CONFIGURATION DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
BYTE	Cfg_Descp[] = {	0x09,				// bLength: Size of descriptor
						0x02,				// bDescriptorType: Configuration	
						CFG_LEN,0x00,		// wTotalLength: Cfg+Ifc+Class+Ep = 34 bytes
						0x01,				// bNumInterfaces: 1 interface
						0x01,				// bConfigurationValue: 1
						0x00,				// iConfiguration: none
						0xC0,				// bmAttributes: self-powered
						0x32,				// MaxPower: 100mA

//---------------------------------------------------------------------------------------------------------------------
// INTERFACE DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
						0x09,				// bLength: Size of descriptor
						0x04,				// bDescriptorType: Interface
						0x00,				// bInterfaceNumber: #0
						0x00,				// bAlternateSetting: #0
						0x02,				// bNumEndpoints: 2
						0x03,				// bInterfaceClass: HID-class
						0x00,				// bInterfaceSubClass: none
						0x00,				// bInterfaceProtocol: none
						0x00,				// iInterface: none

//---------------------------------------------------------------------------------------------------------------------
// HID-CLASS DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
						0x09,				// bLength: Size of descriptor
						0x21,				// bDescriptorType: HID Class
						0x00,0x01,			// bcdHID: HID Class version 1.1
						0x00,				// bCountryCode: none
						0x01,				// bNumDescriptors: 1 report descp
						0x22,				// bDescriptorType: report descriptor type
						REP_LEN,0x00,		// wDescriptorLength: 31 bytes

//---------------------------------------------------------------------------------------------------------------------
// IN ENDPOINT DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
						0x07,				// bLength: Size of descriptor
						0x05,				// bDescriptorType: Endpoint
						0x81,				// bEndpointAddress: IN, EP1
						0x03,				// bmAttributes: Interrupt
						EP1_LEN,0x00,		// wMaxPacketSize: 
						0x0A,				// bInterval: 10ms

//---------------------------------------------------------------------------------------------------------------------
// OUT ENDPOINT DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
						0x07,				// bLength: Size of descriptor
						0x05,				// bDescriptorType: Endpoint
						0x02,				// bEndpointAddress: OUT, EP2
						0x03,				// bmAttributes: Interrupt
						EP2_LEN,0x00,		// wMaxPacketSize: 
						0x0A};				// bInterval: 10ms
						
//---------------------------------------------------------------------------------------------------------------------
// REPORT DESCRIPTOR
//--------------------------------------------------------------------------------------------------------------------------
// Bytes |  Bit 7  |  Bit 6  |  Bit 5  |  Bit 4  |  Bit 3  |  Bit 2  |  Bit 1  |  Bit 0  | 
//--------------------------------------------------------------------------------------------------------------------------
//   0   |  ScanNT |  ScanPT |  BassB   |  Stop   |  Play   |  VolDn  |  VolUp  |  Mute	 | << MultiMedia Audio Control
//--------------------------------------------------------------------------------------------------------------------------
//   1   |  Email  |  BookM  | Refresh |  Stop   | Forward |  Back   |   Home  |  Search | << Internet Application Control
//--------------------------------------------------------------------------------------------------------------------------
/*BYTE	Rep_Descp[] = {	0x05, 0x0C,			// USAGE_PAGE (Consumer Devices)
							0x09, 0x01,			// USAGE (Consumer Control)
							0xA1, 0x01,			// COLLECTION (Application)
							0x15, 0x00,			//  LOGICAL_MINIMUM (0)
							0x25, 0x01,			//  LOGICAL_MAXIMUM (1)
							0x09, 0xE2,			//    USAGE (Mute)					// 	 MultiMedia Audio Control
							0x09, 0xE9,			//    USAGE (Volume Up)
							0x09, 0xEA,			//    USAGE (Volume Down)
							0x09, 0xCD,			//    USAGE (Play/Pause)
							0x09, 0xB7,			//    USAGE (Stop)
							0x09, 0xB6,			//    USAGE (Scan Previous Track)
							0x09, 0xB5,			//    USAGE (Scan Next Track)
							0x09, 0xE5, 		//    USAGE (Bass Boost)								
							0x0A, 0x21, 0x02,	//    USAGE (AC Search)				// 	 Internet Application Control
							0x0A, 0x23, 0x02,	//    USAGE (AC Home)
							0x0A, 0x24, 0x02,	//    USAGE (AC Back)
							0x0A, 0x25, 0x02,	//    USAGE (AC Forward)
							0x0A, 0x26, 0x02,	//    USAGE (AC Stop)
							0x0A, 0x27, 0x02,	//    USAGE (AC Refresh)
							0x0A, 0x2A, 0x02,	//    USAGE (AC Favorites/Bookmark)
							0x0A, 0x8A, 0x01,	//    USAGE (AL Email Reader)
							0x75, 0x01,      	//  REPORT_SIZE (1)
							0x95, 0x10, 		//  REPORT_COUNT (16)
							0x81, 0x06,			//  INPUT (Data,Var,Rel,Prf)
							0xC0};				// END_COLLECTION */

BYTE Rep_Descp[] = {0x06, 0xA0, 0xFF,		//USAGE_PAGE
					0x09, 0xA5,				//USAGE
					0xA1, 0x01,				//COLLECTION
					0x09, 0xA6,				//USAGE
					0x09, 0xA7,				//USAGE
					0x15, 0x80,				//LOGICAL_MINIMUM (-127)
					0x25, 0x7F,				//LOGICAL_MAXIMUM (128)
					0x75, 0x08,				//REPORT_SIZE (8)
					0x95, 0x40,				//REPORT_COUNT (64)
					0x81, 0x02,				//INPUT (Data,Var,Abs)
					0x09, 0xA9,				//USAGE
					0x15, 0x80,				//LOGICAL_MINIMUM (-127)
					0x25, 0x7F,				//LOGICAL_MAXIMUM (128)
					0x75, 0x08,				//REPORT_SIZE (8)
					0x95, 0x40,				//REPORT_COUNT (64)
					0x91, 0x02,				//OUTPUT (Data,Var,Abs)
					0xC0}; // END_COLLECTION

//---------------------------------------------------------------------------------------------------------------------
// CONFIGURATION DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
BYTE Cfg_Descp_CDC[]= {	0x09,				// bLength: Size of descriptor
						0x02,				// bDescriptorType: Configuration
						CFG_LEN_CDC,0x00,	// wTotalLength: Cfg+Ifc+Class+Ep = 67 bytes
						0x02,				// bNumInterfaces: 2 interface
						0x01,				// bConfigurationValue: 1
						0x00,				// iConfiguration: none
						0xC0,				// bmAttributes: self-powered
						0x32,				// MaxPower: 100mA

//---------------------------------------------------------------------------------------------------------------------
// INTERFACE DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
						0x09,				// bLength: Size of descriptor
						0x04,				// bDescriptorType: Interface
						0x00,				// bInterfaceNumber: #0
						0x00,				// bAlternateSetting: #0
						0x01,				// bNumEndpoints: 1
						0x02,				// bInterfaceClass: CDC Communications
						0x02,				// bInterfaceSubClass: Abstract Control Model
						0x01,				// bInterfaceProtocol: Common AT commands
						0x00,				// iInterface: none

//---------------------------------------------------------------------------------------------------------------------
// HEADER FUNCTIONAL DESCRIPTOR p.34 Universal Serial Bus Class Definitions for Communication Devices
//---------------------------------------------------------------------------------------------------------------------
						0x05,
						0x24,
						0x00,
						0x10,
						0x01,

//---------------------------------------------------------------------------------------------------------------------
// ABSTRACT CONTROL MANAGEMENT FUNCTIONAL DESCRIPTOR p.35 Universal Serial Bus Class Definitions for Communication Devices
//---------------------------------------------------------------------------------------------------------------------
						0x04,
						0x24,
						0x02,
						//0x00,
						0x02,

//---------------------------------------------------------------------------------------------------------------------
// UNION FUNCTIONAL DESCRIPTOR p.40 Universal Serial Bus Class Definitions for Communication Devices
//---------------------------------------------------------------------------------------------------------------------
						0x05,
						0x24,
						0x06,
						0x00,
						0x01,

//---------------------------------------------------------------------------------------------------------------------
// CALL MANAGEMENT FUNCTIONAL DESCRIPTOR p.34 Universal Serial Bus Class Definitions for Communication Devices
//---------------------------------------------------------------------------------------------------------------------
						0x05,
						0x24,
						0x01,
						0x00,
						0x01,

//---------------------------------------------------------------------------------------------------------------------
// IN ENDPOINT DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
						0x07,				// bLength: Size of descriptor
						0x05,				// bDescriptorType: Endpoint
						0x83,				// bEndpointAddress: IN, EP3
						0x03,				// bmAttributes: Interrupt
						EP3_LEN,0x00,		// wMaxPacketSize:
						0x0A,				// bInterval: 10ms

//---------------------------------------------------------------------------------------------------------------------
// INTERFACE DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
						0x09,				// bLength: Size of descriptor
						0x04,				// bDescriptorType: Interface
						0x01,				// bInterfaceNumber: #1
						0x00,				// bAlternateSetting: #0
						0x02,				// bNumEndpoints: 2
						0x0A,				// bInterfaceClass: CDC Data
						0x00,				// bInterfaceSubClass: Abstract Control Model
						0x00,				// bInterfaceProtocol: Common AT commands
						0x00,				// iInterface: none

//---------------------------------------------------------------------------------------------------------------------
// IN ENDPOINT DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
						0x07,				// bLength: Size of descriptor
						0x05,				// bDescriptorType: Endpoint
						0x81,				// bEndpointAddress: IN, EP1
						0x02,				// bmAttributes: Bulk
						EP1_LEN,0x00,		// wMaxPacketSize:
						0x00,

//---------------------------------------------------------------------------------------------------------------------
// OUT ENDPOINT DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
						0x07,				// bLength: Size of descriptor
						0x05,				// bDescriptorType: Endpoint
						0x02,				// bEndpointAddress: OUT, EP2
						0x02,				// bmAttributes: Bulk
						EP2_LEN,0x00,		// wMaxPacketSize:
						0x00};


//---------------------------------------------------------------------------------------------------------------------
// STRING DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
BYTE LangString[] = {4,3,9,4};
BYTE MfgString[]  = {18,3,'C',0,'a',0,'p',0,'i',0,'n',0,'t',0,'e',0,'c',0};
BYTE ProdString[] = {18,3,'C',0,'a',0,'p',0,'i',0,'n',0,'t',0,'e',0,'c',0};
//BYTE SerialString[] = {18,3,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,'0',0};

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
#define CFG_LEN		34			// Configuration Descriptor Length
#define REP_LEN		57			// HID Report Descriptor Length

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

//---------------------------------------------------------------------------------------------------------------------
// CONFIGURATION DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
BYTE	Cfg_Descp[] = {	0x09,				// bLength: Size of descriptor
							0x02,				// bDescriptorType: Configuration	
							CFG_LEN,0x00,		// wTotalLength: Cfg+Ifc+Class+Ep = 34 bytes
							0x01,				// bNumInterfaces: 1 interface
							0x01,				// bConfigurationValue: 1
							0x00,				// iConfiguration: none
							0xA0,				// bmAttributes: bus-powered, remote-wakeup
							0x32,				// MaxPower: 100mA

//---------------------------------------------------------------------------------------------------------------------
// INTERFACE DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
							0x09,				// bLength: Size of descriptor
							0x04,				// bDescriptorType: Interface
							0x00,				// bInterfaceNumber: #0
							0x00,				// bAlternateSetting: #0
							0x01,				// bNumEndpoints: 1
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
// ENDPOINT DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
							0x07,				// bLength: Size of descriptor
							0x05,				// bDescriptorType: Endpoint
							0x81,				// bEndpointAddress: IN, EP1
							0x03,				// bmAttributes: Interrupt
							EP1_LEN,0x00,		// wMaxPacketSize: 
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
BYTE	Rep_Descp[] = {	0x05, 0x0C,			// USAGE_PAGE (Consumer Devices)
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
							0xC0};				// END_COLLECTION

//---------------------------------------------------------------------------------------------------------------------
// STRING DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
BYTE LangString[] = {4,3,9,4};
BYTE MfgString[]  = {16,3,'C',0,'y',0,'p',0,'r',0,'e',0,'s',0,'s',0};
BYTE ProdString[] = {76,3,'C',0,'y',0,'p',0,'r',0,'e',0,'s',0,'s',0,' ',0,'S',0,'L',0,'8',0,'1',0,'1',0,'S',0,' ',0,'-',0,' ',0,'H',0,'I',0,'D',0,' ',0,'C',0,'o',0,'n',0,'s',0,'u',0,'m',0,'e',0,'r',0,' ',0,'C',0,'o',0,'n',0,'t',0,'r',0,'o',0,'l',0};

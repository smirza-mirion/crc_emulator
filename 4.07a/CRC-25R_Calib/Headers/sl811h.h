//
// header for sl811h.c  host software
// 07/20/05
//

bool IsUSBPrinterConnected (void);
bool IsUSBPrinterError (void);
bool IsUSBPrinterPaperEmpty (void);
bool IsUSBPrinterSelected (void);
void InitializeSL811H (void);
void usb_write (char *cData);
void ReinitializeSL811H (void);


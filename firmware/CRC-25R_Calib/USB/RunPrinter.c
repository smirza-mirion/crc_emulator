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

extern volatile bool m_flgPrinterBlocked;
extern unsigned long m_ulTimeOut;
extern uchar m_ucLastPID_ENDPOINT;
extern uchar m_ucLastCommand;
extern uchar m_ucOutEndpoint;
extern uchar m_ucOutBufferSize;
extern uchar m_ucInBufferSize;
extern volatile uchar m_ucData_0_1;
extern volatile uchar m_ucData_Out_0_1;
extern volatile uchar m_ucData_In_0_1;
extern volatile STATE m_state[MAX_STATE];
extern volatile uchar *pCS1;
extern volatile uchar m_ucEnumerated;
extern volatile uchar m_ucCircularBuffer[CIR_BUF_SIZE];
extern volatile ushort m_uiHead;
extern volatile ushort m_uiTail;
extern volatile int SCSIPending; // 0 = None, 1 = Read, 2 = Write
extern volatile bool SCSIError;
extern volatile unsigned char *SCSIWriteBufferBegin;
extern volatile unsigned char *SCSIWriteBufferPtr;
extern volatile unsigned long int SCSIWriteSectorAddress;
extern volatile unsigned short int SCSIWriteSectors;
extern volatile unsigned long int SCSIWriteBytes;

extern volatile unsigned char *SCSIReadBufferBegin;
extern volatile unsigned char *SCSIReadBufferPtr;
extern volatile unsigned long int SCSIReadInCount;
extern volatile unsigned long int SCSIReadSectorAddress;
extern volatile unsigned short int SCSIReadSectors;
extern volatile unsigned long int SCSIReadBytes;

extern unsigned long int m_ulLogicalBlockSize;

uchar SCSI_READ_LBA [] = {
						  0x28,
						  0x00,
						  0x00, // LBA
						  0x00, // LBA
						  0x00, // LBA
						  0x00, // LBA
						  0x00,
						  0x00, // Length
						  0x00, // Length
						  0x00
						 };

uchar SCSI_WRITE_LBA [] = {
						   0x2A,
						   0x00,
						   0x00, // LBA
						   0x00, // LBA
						   0x00, // LBA
						   0x00, // LBA
						   0x00,
						   0x00, // Length
						   0x00, // Length
						   0x00
						  };

void Start(int CurrentState, int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall);
void wr811(uchar ucRegister, uchar ucValue);       // write a value into a sl811 register
ushort CirLen(void);
void CreateCBW(uchar *cbw, unsigned long int TransferSize, bool TransferInDir, unsigned char CommandBlockSize, uchar *CommandBlock, unsigned char LUN);
void Out811Init(uchar *buffer, int buffersize);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void StartUsbAFore(int ReturnState, int ReturnPhase, int ReturnError, int ReturnStall, uchar ucCommand){
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

void RunPrinterFore(void){
	uchar index;
	ushort uCirLen;

	if(m_flgPrinterBlocked){
		uCirLen = CirLen();
		if(uCirLen > 0){
			if(uCirLen >= m_ucOutBufferSize){
				wr811(REG_BUFFER_LENGTH, m_ucOutBufferSize);
				pCS1[USB_HOST_OFF_SET] = VAL_BUFFER_OFFSET;
				for(index=0; index<m_ucOutBufferSize; index++){
					if(m_uiHead != m_uiTail){
						pCS1[USB_HOST_OFF_RW] = m_ucCircularBuffer[m_uiHead];
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
					if(m_uiHead == CIR_BUF_SIZE -1) m_uiHead = 0;
					else m_uiHead++;
				}
			}
			m_ucLastPID_ENDPOINT = BIT_OUT_PID | m_ucOutEndpoint;
			wr811(REG_PID_ENDPOINT, m_ucLastPID_ENDPOINT);          // Write an out packet to endpoint 2
			StartUsbAFore(RUNPRINTER, RUNPRINTER_SEND, RUNPRINTER_ERROR, RUNPRINTER_SEND, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
		}
	}
}

bool scsi_read(unsigned char *buffer, unsigned long int LBA, unsigned short int count){
	int countdown;
	bool retry;
	uchar cbw[31], readlba_command[10];

	if(m_ucEnumerated == 2){
		while(SCSIPending != 0) service_watchdog();

		countdown = 10;
		do{
			SCSIReadBufferBegin = buffer;
			SCSIReadBufferPtr = buffer;
			SCSIReadSectorAddress = LBA;
			SCSIReadSectors = count;
			SCSIReadBytes = count;
			SCSIReadBytes *= m_ulLogicalBlockSize;
			SCSIReadInCount = SCSIReadBytes / m_ucInBufferSize;
			if(SCSIReadBytes % m_ucInBufferSize) SCSIReadInCount++;

			Start(SCSIREADBLOCK, SCSIREADBLOCK, SCSIREADBLOCK_WAIT, SCSIREADBLOCK_ERROR, SCSIREADBLOCK_STALL);
			memcpy(readlba_command, SCSI_READ_LBA, sizeof(SCSI_READ_LBA));
			memcpy(&(readlba_command[2]), (const void *) &SCSIReadSectorAddress, 4);
			memcpy(&(readlba_command[7]), (const void *) &SCSIReadSectors, 2);
			CreateCBW(cbw, SCSIReadBytes, TRUE, sizeof(SCSI_READ_LBA), readlba_command, 0);
			Out811Init(cbw, sizeof(cbw));
			SCSIPending = 1;
			SCSIError = 0;
			StartUsbAFore(SCSIREADBLOCK, SCSIREADBLOCK_IN, SCSIREADBLOCK_ERROR, SCSIREADBLOCK_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
			while(SCSIPending == 1) service_watchdog();
			if(SCSIError){
				if(m_ucEnumerated == 2){
					countdown--;
					if(countdown) retry = TRUE;
					else retry = FALSE;
				}
			}else{
				retry = FALSE;
			}
		}while(retry);

		return SCSIError;
	}else{
		return TRUE;
	}
}

bool scsi_write(unsigned char *buffer, unsigned long int LBA, unsigned short int count){
	uchar cbw[31], writelba_command[10];

    if(m_ucEnumerated == 2){
    	while(SCSIPending != 0) service_watchdog();

    	SCSIWriteBufferBegin = buffer;
    	SCSIWriteBufferPtr = buffer;
    	SCSIWriteSectorAddress = LBA;
    	SCSIWriteSectors = count;
    	SCSIWriteBytes = count;
    	SCSIWriteBytes *= m_ulLogicalBlockSize;

    	Start(SCSIWRITEBLOCK, SCSIWRITEBLOCK, SCSIWRITEBLOCK_WAIT, SCSIWRITEBLOCK_ERROR, SCSIWRITEBLOCK_STALL);
    	memcpy(writelba_command, SCSI_WRITE_LBA, sizeof(SCSI_WRITE_LBA));
    	memcpy(&(writelba_command[2]), (const void *) &SCSIWriteSectorAddress, 4);
    	memcpy(&(writelba_command[7]), (const void *) &SCSIWriteSectors, 2);
    	CreateCBW(cbw, SCSIWriteBytes, FALSE, sizeof(SCSI_WRITE_LBA), writelba_command, 0);
    	Out811Init(cbw, sizeof(cbw));
    	SCSIPending = 2;
    	SCSIError = 0;
    	StartUsbAFore(SCSIWRITEBLOCK, SCSIWRITEBLOCK_OUT, SCSIWRITEBLOCK_ERROR, SCSIWRITEBLOCK_STALL, BIT_CTRL_ARM | BIT_CTRL_ENABLE | BIT_CTRL_OUT);
    	while(SCSIPending == 2) service_watchdog();
    	return SCSIError;
    }else{
    	return TRUE;
    }
}

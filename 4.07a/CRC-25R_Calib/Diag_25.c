/*********************************************************************
  MODULE:		DIAGNOSTICS -- System Test for CRC-15R

  FILE:		Diag_15.c

  DATE: 	04/07/06

  ANALYSIS:	controls flow of diagnostics

  CALLED BY:
		main menu

  ***********************************************************************/
#include "crc.h"
#include "i2c.h"
#include "screen.h"
#include "qspi.h"
#include "message.h"
#include "pit.h"
#include "cs.h"
#include "keyboard.h"
#include "coldfire.h"
#include "uart.h"
#include "amulet.h"
//#include "mmcapi.h"
#include "ff.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char m_acInfo_Diagnostics1[26];
extern char m_acInfo_Diagnostics2[26];
extern char m_cInfo_DiagnosticsFailed;
extern volatile uchar m_ucEnumerated;

static int read_memdata_from_mmc_card(char *data, int bufsize);
static bool program_memory_test(ushort *val);
ushort CirLen(void);

    bool system_test(void)
	{

        ushort crcval;
        bool mem_ok;
        short rflag;
        char status_str[8];
        char str[6];

        stop_tone();

        //disable all interrupts
        disable_all_interrupts();
        cf.uarts[2].ir = 0;
        cf.uarts[2].ucr = UART_UCR_RESET_TX;											// Reset Tx
        cf.uarts[2].ucr = UART_UCR_RESET_RX;											// Reset Rx

        display_text(0,14,"SYSTEM TEST",0,MEDIUM,NORMAL);

        mem_ok = program_memory_test(&crcval);
        sprintf(str,"%4x",crcval);
        if(mem_ok)
        {
            rflag = NORMAL;
            strcpy(status_str,"PASS: ");
        }    
        else
        {
            rflag = REV;
            strcpy(status_str,"FAIL: ");
        }   

        display_text(16,28,status_str,0,MEDIUM,rflag);
        display_text(64,28,str,0,MEDIUM,rflag);
        

        //re-initialize interrupts
        //initialize_interrupts();
        cf.intc[0].imrl &= 0xffff7f5e;													// Unmask IRQ 5, 7, 15
        cf.intc[0].imrh &= 0xfdffffff;													// Unmask IRQ 57

		// Clear UART Structures
		ClearUart1Rx();
		ClearUart1Tx();
		ClearUart2Rx();
		ClearUart2Tx();
		ClearMasterMessage();
		ClearSlaveMessage();
		ClearCmdQueue();

		// Enable UART
		cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
		cf.uarts[2].ucr = UART_UCR_RX_ENABLED;
		cf.uarts[2].ir = 3;
        
        //initialize QSPI for communicating with chamber
        //init_qspi();
        return (mem_ok);
	}

bool amulet_system_test(void){
	ushort crcval;
	bool mem_ok;
	char sCRCString[10];
	unsigned long int lowermask, uppermask;

	stop_tone();

	if(m_ucEnumerated == 1){
		while(CirLen() != 0) service_watchdog();
	}

	//disable all interrupts
	lowermask = cf.intc[0].imrl;
	uppermask = cf.intc[0].imrh;
	disable_all_interrupts();

	cf.uarts[2].ir = 0;
	cf.uarts[2].ucr = UART_UCR_RESET_TX;											// Reset Tx
	cf.uarts[2].ucr = UART_UCR_RESET_RX;											// Reset Rx

	//strcpy(m_acInfo_Diagnostics1, "PROGRAM INTEGRITY");
	get_amulet_message(L_PROGRAM_INTEGRITY,m_acInfo_Diagnostics1);    // "PROGRAM INTEGRITY"

	mem_ok = program_memory_test(&crcval);
	sprintf(sCRCString,"%4x",crcval);

	if(mem_ok){
		//strcpy(m_acInfo_Diagnostics2, "PASS: ");
		get_amulet_message(L_PASS,m_acInfo_Diagnostics2);    // "PASS: "
		m_cInfo_DiagnosticsFailed = 0;
	}else{
		//strcpy(m_acInfo_Diagnostics2, "FAIL: ");
		get_amulet_message(L_FAIL,m_acInfo_Diagnostics2);    // "FAIL: "
		m_cInfo_DiagnosticsFailed = 1;
	}
	strcat(m_acInfo_Diagnostics2, sCRCString);

	//re-initialize interrupts
	//initialize_interrupts();
	//cf.intc[0].imrl &= 0xffff7f5e;													// Unmask IRQ 5, 7, 15
	//cf.intc[0].imrh &= 0xfdffffff;													// Unmask IRQ 57
	cf.intc[0].imrl = lowermask;
	cf.intc[0].imrh = uppermask;

	// Clear UART Structures
	ClearUart1Rx();
	ClearUart1Tx();
	ClearUart2Rx();
	ClearUart2Tx();
	ClearMasterMessage();
	ClearSlaveMessage();
	ClearCmdQueue();

	// Enable UART
	cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
	cf.uarts[2].ucr = UART_UCR_RX_ENABLED;
	cf.uarts[2].ir = 3;

	//initialize QSPI for communicating with chamber
	//init_qspi();
	return (mem_ok);
}

#define  BASE_ADDRESS 0x02000000  
    static bool program_memory_test(ushort *val)
    {
        uchar *addr;
        ulong i;
        ushort crc;
        char ch;
        bool ok;
        char memdata[20];
        ulong nbytes;
        ulong crcval;
        char *where;
        short index;
        int filesize;
            
        generate_crc_table();

        //read program length and CRC value from mmc card
        filesize = read_memdata_from_mmc_card(memdata,20);
        if(filesize == 0)
            return FALSE;
        
        nbytes = strtoul(memdata,NULL,10);
        where = strchr(memdata,'$');
        index = where - memdata + 1;
        crcval = strtoul(&memdata[index],NULL,16);
        
        //start at beginning of program 0x02000000
        addr = (uchar *)BASE_ADDRESS;
    
        crc = 0;

        for(i = 0; i < nbytes; i++)
        {
            ch = *addr++;
            crc = Crc16(ch,crc);
            if((i%100000) == 0) service_watchdog();
        }
    
        ok = (crc == crcval);

        //return the CRC value
        *val = crc;
        
        return (ok);
    
    
    }

static int read_memdata_from_mmc_card(char *data, int bufsize){
	int filesize=0;
	FIL fileObject;
	FILINFO fileInfo;
	char longFileName[100];
	UINT bytesRead;

	//mmc_change_dir("\\");       //Make sure we're at Root
	//mmc_change_dir("images");
	f_chdrive(0);
	f_chdir("/images");

	//if(mmc_file_exists("memdata.txt") == 0)
	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	if(f_stat("memdata.txt", &fileInfo) == FR_OK){
		//filesize = mmc_read_file("memdata.txt", data, bufsize);
		f_open(&fileObject, "memdata.txt", FA_READ);
		f_read(&fileObject, data, bufsize, &bytesRead);
		f_close(&fileObject);
		filesize = bytesRead;
	}
#if SYSCALLS
	else
		printf("\nmemdata.txt Not found!");
#endif  
	return(filesize);
}

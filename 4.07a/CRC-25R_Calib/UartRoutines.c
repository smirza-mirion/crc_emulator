/**
 * \file
 * \details This file contains functions, which calls the Coldfire UART Module.
 */
/********************************************************************
  MODULE: UART Routines

  FILE:   UartRoutines.c

  DATE;   06/14/05
          02/11/08 -- added watchdog to uart_char_waiting
  ********************************************************************/
#include "coldfire.h"
#include "crc.h"
#include "uart.h"
#include "pit.h"
#include "printer.h"
#include "keyboard.h"
#include "amulet.h"
#include <string.h>
#include <stdlib.h>

volatile unsigned char g_flgAmuletMode = 1;

static bool pc_char_waiting;
static void slip_xon_xoff(void);

volatile static unsigned char ucXON = 0xFF;
volatile static short unsigned int uiUart1RxHead;
volatile static short unsigned int uiUart1RxTail;
volatile static short unsigned int uiUart1TxHead;
volatile static short unsigned int uiUart1TxTail;
volatile static char acUart1Rx[UART_BUFFER_SIZE];
volatile static char acUart1Tx[UART_BUFFER_SIZE];

volatile static short unsigned int uiUart2RxHead;
volatile static short unsigned int uiUart2RxTail;
volatile static short unsigned int uiUart2TxHead;
volatile static short unsigned int uiUart2TxTail;
volatile static char acUart2Rx[UART_BUFFER_SIZE];
volatile static char acUart2Tx[UART_BUFFER_SIZE];
volatile static char acUart2TxRaw[UART_BUFFER_SIZE];

/**
 * \details Initialize Coldfire UART registers
 * \param BaudRate Baud rate
 * \param iu UART channel, 0 = Printer, 1 = PC communications
 * \returns None
 */
    void init_uart(int BaudRate, int iu)
    {

        ushort baud;
        uchar baud_hi,baud_lo;
        uchar cfbyte;

        //set port iu for primary function of Tx,Rx
        switch(iu)
        {
        case 0:
            cf.gpio.puapar |= 0x03;
            break;
        case 1:
            cf.gpio.puapar |= 0x0c;
            break;
        case 2:  //to be decided
        	cf.gpio.paspar &= 0xffaf;
        	cf.gpio.paspar |= 0x00a0;
        	baud = cf.gpio.paspar;
        	printf("%x\n", baud);
            break;
        }

        //printer
        if(iu == U_PR)
        {
            //set pin 1 of port tc to 0 for gpio -- bits 2-3
            cfbyte = cf.gpio.ptcpar;
            cfbyte &= 0xf3;
            cf.gpio.ptcpar = cfbyte;

            //set for input, bit 1
            cfbyte = cf.gpio.ddrtc;
            cfbyte &= 0xfd;
            cf.gpio.ddrtc = cfbyte;
        }

        cf.uarts[iu].ucr 	= UART_UCR_RESET_TX;
        cf.uarts[iu].ucr 	= UART_UCR_RESET_RX;
        cf.uarts[iu].ir 	= 0;			// Disable all interrupts

        cf.uarts[iu].ucr 	= UART_UCR_RESET_MR;	// Point to UMR1
        cf.uarts[iu].umr	= UART_UMR1_PM_NONE | UART_UMR1_BC_8;
        cf.uarts[iu].umr	= UART_UMR2_CM_NORMAL   // UMR2
                                       | UART_UMR2_STOP_BITS_1;

        cf.uarts[iu].usr 	= UART_UCSR_TIMER;

        //set the Baud Rate
        baud = (ushort)(SYSCLK / (32 * BaudRate));
        baud_hi = (uchar)(baud >> 8);
        baud_lo = (uchar)(baud & 0xff);
        cf.uarts[iu].dur	= baud_hi;
        cf.uarts[iu].dlr = (uchar)baud_lo;

        cf.uarts[iu].ucr 	= UART_UCR_TX_ENABLED;
        cf.uarts[iu].ucr 	= UART_UCR_RX_ENABLED;

        if(iu == U_PC)
            pc_char_waiting = FALSE;
    }

    //send 1 character
/**
 * \details Send a single character out on the UART
 * \param c Character value
 * \param iu UART channel, 0 = Printer, 1 = PC communications
 * \returns None
 */
    void uart_putchar(int c, int iu)
    {

        //service watchdog
        service_watchdog();

        //wait till ready to transmit
        while(!(cf.uarts[iu].usr & UART_USR_TXRDY))
        ;

        cf.uarts[iu].udb = (uchar)c;
    }

    //send string
/**
 * \details Send a string out on the UART
 * \param str Pointer to Null string
 * \param iu UART channel, 0 = Printer, 1 = PC communications
 * \returns None
 */
    void uart_write(char *str, int iu)
    {
        int len = strlen(str);

        while(len--)
        {

            uart_putchar(*str++, iu);
            //if printer is SLIP PRINTER or ROLL PRINTER, do XON/XOFF
            //if(current.printer == SLIP_PRINTER || current.printer == ROLL_PRINTER)
                slip_xon_xoff();
        }
    }

    //send specified number of characters (needed for case of string containing NULL)
/**
 * \details Send specific number of characters out on the UART
 * \param str Pointer to byte array
 * \param num Number of bytes
 * \param iu UART channel, 0 = Printer, 1 = PC communications
 * \returns None
 */
    void uart_write_num(char *str, short num, int iu)
    {

        int len = num;

        while(len--)
        {
            uart_putchar(*str++, iu);
            //if printer is SLIP PRINTER or ROLL PRINTER, do XON/XOFF
            //if(current.printer == SLIP_PRINTER || current.printer == ROLL_PRINTER)
                slip_xon_xoff();
        }
    }


    //is there a character waiting to be received?
/**
 * \details Is there a character waiting to be received
 * \param iu UART channel, 0 = Printer, 1 = PC communications
 * \returns True = Character is pending in the Coldfire UART register, False = No pending UART character
 */
    bool uart_char_waiting(int iu)
    {
        bool ch_flag;

        service_watchdog();


        ch_flag = (cf.uarts[iu].usr & UART_USR_RXRDY)!=0;
        if(iu == U_PC)
            pc_char_waiting = ch_flag;
        return ch_flag;
    }

    //return pc character waiting
/**
 * \details Get the pc_char_waiting status
 * \returns Status value
 */
    bool is_pc_char_waiting(void)
    {
        return(pc_char_waiting);
    }


    /* get a character
       if wait_flag set, waits until there is a character to be received
       called with wait_flag FALSE if already checked to see if there is a
       character
    */
/**
 * \details Get a UART character with wait
 * \param wait_flag True = Wait until character is received, False = Do not wait
 * \param iu UART Channel, 0 = Printer, 1 = PC communications
 * \returns Character value
 */
    int uart_getchar(bool wait_flag, int iu)
    {

        //if wait_flag, wait until a character is received
        if(wait_flag)
        {
            while(!uart_char_waiting(iu))
            ;
        }

        return (int)cf.uarts[iu].udb;
    }

    //xon / xoff for Slip printer, to avoid overrun
#define  XON  (char)0x11
#define  XOFF (char)0x13
/**
 * \details Stall process when XOFF and resume when XON to avoid overrun in Printer
 * \returns None
 */
    static void slip_xon_xoff(void)
    {
        if(!uart_char_waiting(U_PR))
            return;

        //character waiting from slip printer
        if(uart_getchar(FALSE,U_PR) == XOFF)
        {
            //wait for XON
            for(;;)
            {
                delay_msec(5);
                if(uart_char_waiting(U_PR))
                {
                    if(uart_getchar(FALSE,U_PR) == XON)
                        return;
                }
            }
        }
    }

/**
 * \details Clear UART 1 circular receive buffer
 * \returns None
 */
void ClearUart1Rx(void){
	uiUart1RxHead = uiUart1RxTail = 0;
}

/**
 * \details Clear UART 2 circular receive buffer
 * \returns None
 */
void ClearUart2Rx(void){
	uiUart2RxHead = uiUart2RxTail = 0;
}

/**
 * \details Clear UART 1 circular transmit buffer
 * \returns None
 */
void ClearUart1Tx(void){
	uiUart1TxHead = uiUart1TxTail = 0;
}

/**
 * \details Clear UART 2 circular transmit buffer
 * \returns None
 */
void ClearUart2Tx(void){
	uiUart2TxHead = uiUart2TxTail = 0;
}

/**
 * \details Get number of items in the UART 1 circular receive buffer
 * \returns Number of items
 */
short unsigned int QueryPendingUart1Rx(void) {
	short unsigned int uiSize;

	if (uiUart1RxHead >= uiUart1RxTail) {
		uiSize = uiUart1RxHead - uiUart1RxTail;
	}
	else {
		uiSize = UART_BUFFER_SIZE - uiUart1RxTail + uiUart1RxHead;
	}

	return uiSize;
}

/**
 * \details Get number of items in the UART 2 circular receive buffer
 * \returns Number of items
 */
short unsigned int QueryPendingUart2Rx(void) {
	short unsigned int uiSize;

	if (uiUart2RxHead >= uiUart2RxTail) {
		uiSize = uiUart2RxHead - uiUart2RxTail;
	}
	else {
		uiSize = UART_BUFFER_SIZE - uiUart2RxTail + uiUart2RxHead;
	}

	return uiSize;
}

/**
 * \details Get number of items in the UART 1 circular transmit buffer
 * \returns Number of items
 */
short unsigned int QueryPendingUart1Tx(void) {
	short unsigned int uiSize;

	if (uiUart1TxHead >= uiUart1TxTail) {
		uiSize = uiUart1TxHead - uiUart1TxTail;
	}
	else {
		uiSize = UART_BUFFER_SIZE - uiUart1TxTail + uiUart1TxHead;
	}

	return uiSize;
}

/**
 * \details Get number of items in the UART2 circular transmit buffer
 * \returns Number of items
 */
short unsigned int QueryPendingUart2Tx(void) {
	short unsigned int uiSize;

	if (uiUart2TxHead >= uiUart2TxTail) {
		uiSize = uiUart2TxHead - uiUart2TxTail;
	}
	else {
		uiSize = UART_BUFFER_SIZE - uiUart2TxTail + uiUart2TxHead;
	}

	return uiSize;
}

/**
 * \details Push a single byte into the UART 1 circular receive buffer
 * \param cInput Byte value
 * \returns 0 = Success, -1 = Buffer is full
 */
char PushUart1Rx(char cInput) {
	char cError;
	short unsigned int uiNextHead;

	uiNextHead = uiUart1RxHead + 1;
	if(uiNextHead == UART_BUFFER_SIZE) uiNextHead = 0;

	if(uiUart1RxTail == uiNextHead) {
		cError = -1;
	}
	else {
		acUart1Rx[uiUart1RxHead] = cInput;
		uiUart1RxHead = uiNextHead;
		cError = 0;
	}

	return cError;
}

/**
 * \details Push a single byte into the UART 2 circular receive buffer
 * \param cInput Byte value
 * \returns 0 = Success, -1 = Buffer is full
 */
char PushUart2Rx(char cInput) {
	char cError;
	short unsigned int uiNextHead;

	uiNextHead = uiUart2RxHead + 1;
	if(uiNextHead == UART_BUFFER_SIZE) uiNextHead = 0;

	if(uiUart2RxTail == uiNextHead) {
		cError = -1;
	}
	else {
		acUart2Rx[uiUart2RxHead] = cInput;
		uiUart2RxHead = uiNextHead;
		cError = 0;
	}

	return cError;
}

/**
 * \details Push a single byte into the UART 1 circular transmit buffer
 * \param cInput Byte value
 * \returns 0 = Success, -1 = Buffer is full
 */
char PushUart1Tx(char cInput) {
	char cError;
	short unsigned int uiNextHead;

	uiNextHead = uiUart1TxHead + 1;
	if(uiNextHead == UART_BUFFER_SIZE) uiNextHead = 0;

	if(uiUart1TxTail == uiNextHead) {
		cError = -1;
	}
	else {
		acUart1Tx[uiUart1TxHead] = cInput;
		uiUart1TxHead = uiNextHead;
		cError = 0;
	}

	return cError;
}

/**
 * \details Push a single byte into the UART 2 circular transmit buffer
 * \param cInput Byte value
 * \param raw Raw byte value
 * \returns 0 = Success, -1 = Buffer is full
 */
char PushUart2Tx(char cInput, char raw) {
	char cError;
	short unsigned int uiNextHead;

	uiNextHead = uiUart2TxHead + 1;
	if(uiNextHead == UART_BUFFER_SIZE) uiNextHead = 0;

	if(uiUart2TxTail == uiNextHead) {
		cError = -1;
	}
	else {
		acUart2Tx[uiUart2TxHead] = cInput;
		acUart2TxRaw[uiUart2TxHead] = raw;
		uiUart2TxHead = uiNextHead;
		cError = 0;
	}

	return cError;
}

/**
 * \details Pop a single byte from the UART 1 circular receive buffer
 * \param cOutput Pointer to character to receive data
 * \returns 0 = Success, -1 = Buffer is empty
 */
char PopUart1Rx(char *cOutput) {
	char cError;

	if(uiUart1RxTail == uiUart1RxHead) {
		cError = -1;
	}
	else {
		*cOutput = acUart1Rx[uiUart1RxTail];
		if((uiUart1RxTail + 1) == UART_BUFFER_SIZE) uiUart1RxTail = 0;
		else uiUart1RxTail++;
		cError = 0;
	}

	return cError;
}

/**
 * \details Pop a single byte from the UART 2 circular receive buffer
 * \param cOutput Pointer to character to receive data
 * \returns 0 = Success, -1 = Buffer is empty
 */
char PopUart2Rx(char *cOutput) {
	char cError;

	if(uiUart2RxTail == uiUart2RxHead) {
		cError = -1;
	}
	else {
		*cOutput = acUart2Rx[uiUart2RxTail];
		if((uiUart2RxTail + 1) == UART_BUFFER_SIZE) uiUart2RxTail = 0;
		else uiUart2RxTail++;
		cError = 0;
	}

	return cError;
}

/**
 * \details Pop a single byte from the UART 1 circular transmit buffer
 * \param cOutput Pointer to character to receive data
 * \returns 0 = Success, -1 = Buffer is empty
 */
char PopUart1Tx(char *cOutput) {
	char cError;

	if(uiUart1TxTail == uiUart1TxHead) {
		cError = -1;
	}
	else {
		*cOutput = acUart1Tx[uiUart1TxTail];
		if((uiUart1TxTail + 1) == UART_BUFFER_SIZE) uiUart1TxTail = 0;
		else uiUart1TxTail++;
		cError = 0;
	}

	return cError;
}

/**
 * \details Pop a single byte from the UART 2 circular transmit buffer
 * \param cOutput Pointer to character to receive data
 * \param raw Pointer to character to receive raw data
 * \returns 0 = Success, -1 = Buffer is empty
 */
char PopUart2Tx(char *cOutput, char *raw) {
	char cError;

	if(uiUart2TxTail == uiUart2TxHead) {
		cError = -1;
	}
	else {
		*cOutput = acUart2Tx[uiUart2TxTail];
		*raw = acUart2TxRaw[uiUart2TxTail];
		if((uiUart2TxTail + 1) == UART_BUFFER_SIZE) uiUart2TxTail = 0;
		else uiUart2TxTail++;
		cError = 0;
	}

	return cError;
}

/**
 * \details Put a Null terminated string onto the UART 1 circular transmit buffer
 * \param cOutput Pointer to Null terminated string
 * \returns 0 = Success, -1 = Not enough space on the buffer
 */
char SendUart1Tx(char *cOutput){
	short unsigned int uiLength;
	short unsigned int uiFree;
	char cStatus;
	char cBinary[5];
	unsigned char first, second;
	unsigned char *pucOutput;

	if(g_flgAmuletMode){
		pucOutput = (unsigned char *) cOutput;
		first = *pucOutput;
		pucOutput++;
		second = *pucOutput;

		if((first == 0xA0) && (second == 0x02)){
			cBinary[0] = 0xA0;
			cBinary[1] = 0x02;
			cBinary[2] = ConvertByte(cOutput + 2);
			cBinary[3] = ConvertByte(cOutput + 4);
			cBinary[4] = ConvertByte(cOutput + 6);

			uiLength = 6;

			uiFree = UART_BUFFER_SIZE - 1;
			uiFree = uiFree - QueryPendingUart1Tx();

			if (uiFree >= uiLength) {
				PushUart1Tx(cBinary[0]);
				PushUart1Tx(cBinary[1]);
				PushUart1Tx(cBinary[2]);
				PushUart1Tx(cBinary[3]);
				PushUart1Tx(cBinary[4]);
				PushUart1Tx(0);
				cf.uarts[1].ucr = UART_UCR_TX_ENABLED;
				cStatus = 0;
			}else{
				cStatus = -1;
			}
		}else{
			uiLength = strlen(cOutput) + 1;

			uiFree = UART_BUFFER_SIZE - 1;
			uiFree = uiFree - QueryPendingUart1Tx();

			if (uiFree >= uiLength) {
				while((*cOutput) != 0){
					PushUart1Tx(*cOutput);
					cOutput++;
				}

				PushUart1Tx(0);
				if (uiLength > 27) {
					PushUart1Tx(XOFF);
				}

				cf.uarts[1].ucr = UART_UCR_TX_ENABLED;
				cStatus = 0;
			}else {
				cStatus = -1;
			}
		}

		return cStatus;
	}else{
		return 0;
	}

}

/**
 * \details Put a Null terminated string onto the UART 2 circular transmit buffer
 * \param cOutput Pointer to Null terminated string
 * \returns 0 = Success, -1 = Not enough space on the buffer
 */
char SendUart2Tx(char *cOutput){
	short unsigned int uiLength;
	short unsigned int uiFree;
	char cStatus;
	char cBinary[5];
	unsigned char first, second;
	unsigned char *pucOutput;

	if(g_flgAmuletMode){
		pucOutput = (unsigned char *) cOutput;
		first = *pucOutput;
		pucOutput++;
		second = *pucOutput;

		if((first == 0xA0) && (second == 0x02)){
			cBinary[0] = 0xA0;
			cBinary[1] = 0x02;
			cBinary[2] = ConvertByte(cOutput + 2);
			cBinary[3] = ConvertByte(cOutput + 4);
			cBinary[4] = ConvertByte(cOutput + 6);

			uiLength = 6;

			uiFree = UART_BUFFER_SIZE - 1;
			uiFree = uiFree - QueryPendingUart2Tx();

			if (uiFree >= uiLength) {
				PushUart2Tx(cBinary[0], -1);
				PushUart2Tx(cBinary[1], -1);
				PushUart2Tx(cBinary[2], -1);
				PushUart2Tx(cBinary[3], -1);
				PushUart2Tx(cBinary[4], -1);
				PushUart2Tx(0, -1);
				cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
				cStatus = 0;
			}else{
				cStatus = -1;
			}
		}else{
			uiLength = strlen(cOutput) + 1;

			uiFree = UART_BUFFER_SIZE - 1;
			uiFree = uiFree - QueryPendingUart2Tx();

			if (uiFree >= uiLength) {
				while((*cOutput) != 0){
					PushUart2Tx(*cOutput, 0);
					cOutput++;
				}

				PushUart2Tx(0, 0);
				if (uiLength > 27) {
					PushUart2Tx(XOFF, 0);
				}

				cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
				cStatus = 0;
			}else {
				cStatus = -1;
			}
		}

		return cStatus;
	}else{
		return 0;
	}

}

/**
 * \details Transmit NAK on UART 1
 * \returns None
 */
void SendNAK1(void) {
	if(g_flgAmuletMode){
		PushUart1Tx((char) 0xF1);
		cf.uarts[1].ucr = UART_UCR_TX_ENABLED;
	}
}

/**
 * \details Transmit NAK on UART 2
 * \returns None
 */
void SendNAK2(void) {
	if(g_flgAmuletMode){
		PushUart2Tx((char) 0xF1, 0);
		cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
	}
}

/**
 * \details Send Amulet wake up command on UART 2
 * \returns None
 */
void SendWakeUp(void){
	PushUart2Tx(0xA0, -1);
	PushUart2Tx(0x02, -1);
	PushUart2Tx(0x00, -1);
	PushUart2Tx(0x16, -1);
	PushUart2Tx(0x48, -1);
	cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
}

/**
 * \details Interrupt handler for receive and transmit for UART 1. Receives go into the receive circular and transmits pull from the transmit circular buffer
 * \returns None
 */
__interrupt void uart1_interrupt(void){
	char cStatus;
	char cByte;

	cStatus = cf.uarts[1].usr;

	// Test Received Break
	if ((cStatus & UART_USR_RB) != 0) printf("Received Break (1)\n");

	// Test Framing Error
	if ((cStatus & UART_USR_FE) != 0) printf("Framing Error (1)\n");

	// Test Parity Error
	if ((cStatus & UART_USR_PE) != 0) printf("Parity Error (1)\n");

	// Test Overrun Error
	if ((cStatus & UART_USR_OE) != 0) printf("Overrun Error (1)\n");

	if((cStatus & 0xF0) != 0) cf.uarts[1].ucr = UART_UCR_RESET_ERROR;

	// Test if there is a byte has been received
	while((cf.uarts[1].usr & UART_USR_RXRDY) != 0){
		cByte = cf.uarts[1].udb;
		PushUart1Rx(cByte);
	}

	// Test if Transmitter Ready
	if ((cStatus & UART_USR_TXRDY) != 0) {
		if (QueryPendingUart1Tx() == 0) {
			if((cStatus & UART_USR_TXEMP) != 0) cf.uarts[1].ucr = UART_UCR_TX_DISABLED;
		}else{
			while((cf.uarts[1].usr & UART_USR_TXRDY) != 0) {
				if(QueryPendingUart1Tx() == 0) break;
				else{
					if(!PopUart1Tx(&cByte)) {
						cf.uarts[1].udb = cByte;
					}else break;
				}
			}
		}
	}
}

/**
 * \details Interrupt handler for receive and transmit for UART 2. Receives go into the receive circular and transmits pull from the transmit circular buffer
 * \returns None
 */
__interrupt void amulet_interrupt(void) {
	char cStatus;
	char cByte, cRaw;
	unsigned char ucByte;
	char cPacketArray[257];
	int i;

	cStatus = cf.uarts[2].usr;

	// Test Received Break
	if ((cStatus & UART_USR_RB) != 0) printf("Received Break (2)\n");

	// Test Framing Error
	if ((cStatus & UART_USR_FE) != 0) printf("Framing Error (2)\n");

	// Test Parity Error
	if ((cStatus & UART_USR_PE) != 0) printf("Parity Error (2)\n");

	// Test Overrun Error
	if ((cStatus & UART_USR_OE) != 0) printf("Overrun Error (2)\n");

	if((cStatus & 0xF0) != 0) cf.uarts[2].ucr = UART_UCR_RESET_ERROR;

	if(g_flgAmuletMode){
		// Test if there is a byte has been received
		while((cf.uarts[2].usr & UART_USR_RXRDY) != 0){
			cByte = cf.uarts[2].udb;
			ucByte = (unsigned char) cByte;

			if (cByte == XON){
				ucXON = 0xFF;
				cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
			}else{
				if (QueryPendingUart2Rx() == 0){
					if ((ucByte == 0x00) || (ucByte == 0xF0)){
						// Do Nothing, Ignore Null or ACK
					}else if(IsMSOM(ucByte) || IsSSOM(ucByte)) PushUart2Rx(cByte); // Place into Queue
					else SendNAK2(); // Send NAK
				}else {
					if (IsMSOM(ucByte) || IsSSOM(ucByte) || (ucByte == 0xF0)) {
						ClearUart2Rx();
						if (IsMSOM(ucByte) || IsSSOM(ucByte)) PushUart2Rx(cByte);
					}else if (ucByte == 0x00) {
						i = 0;
						while((!PopUart2Rx(&cByte)) && (i < 256)) {
							cPacketArray[i] = cByte;
							i++;
						}
						cPacketArray[i] = 0;

						if (CheckPacket(cPacketArray)) 	PushPacket(cPacketArray);
						else SendNAK2();
						ClearUart2Rx();
					}else {
/*						if(ucByte > 0x7F) {
 - 							ClearUart2Rx();
 - 							SendNAK2();
 - 						}else PushUart2Rx(cByte);*/
						PushUart2Rx(cByte);
					}
				}
			}
		}

		// Test if Transmitter Ready
		if ((cStatus & UART_USR_TXRDY) != 0) {
			if (ucXON) {
				if (QueryPendingUart2Tx() == 0) {
					if((cStatus & UART_USR_TXEMP) != 0) cf.uarts[2].ucr = UART_UCR_TX_DISABLED;
				}else{
					while((cf.uarts[2].usr & UART_USR_TXRDY) != 0) {
						if(QueryPendingUart2Tx() == 0) break;
						else{
							if(!PopUart2Tx(&cByte, &cRaw)) {
								if ((cByte==XOFF) && (cRaw==0x00)) {
									ucXON = 0;
									cf.uarts[2].udb = cByte;
									break;
								}else cf.uarts[2].udb = cByte;
							}else break;
						}
					}
				}
			}
		//else {
		//	if((cStatus & UART_USR_TXEMP) != 0) {
		//		cf.uarts[1].ucr = UART_UCR_TX_DISABLED;
		//	}
		//}
		}
	}else{
		// Test if there is a byte has been received
		while((cf.uarts[2].usr & UART_USR_RXRDY) != 0){
			cByte = cf.uarts[2].udb;
			ucByte = (unsigned char) cByte;
			PushUart2Rx(cByte);
		}

		// Test if Transmitter Ready
		if ((cStatus & UART_USR_TXRDY) != 0) {
			if (QueryPendingUart2Tx() == 0) {
				if((cStatus & UART_USR_TXEMP) != 0) cf.uarts[2].ucr = UART_UCR_TX_DISABLED;
			}else{
				while((cf.uarts[2].usr & UART_USR_TXRDY) != 0) {
					if(QueryPendingUart2Tx() == 0) break;
					else{
						if(!PopUart2Tx(&cByte, &cRaw)) {
							cf.uarts[2].udb = cByte;
						}else break;
					}
				}
			}
		}
	}
}

/**
 * \details Set UART 1 to start transmission
 * \returns None
 */
void EnableTx1(void){
	cf.uarts[1].ucr = UART_UCR_TX_ENABLED;
}

/**
 * \details Set UART 2 to start transmission
 * \returns None
 */
void EnableTx2(void){
	cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
}

void printUart2(void){
	printf("uiUart2RxHead: %u\n", uiUart2RxHead);
	printf("uiUart2RxTail: %u\n", uiUart2RxTail);
	printf("uiUart2TxHead: %u\n", uiUart2TxHead);
	printf("uiUart2TxTail: %u\n\n", uiUart2TxTail);
}

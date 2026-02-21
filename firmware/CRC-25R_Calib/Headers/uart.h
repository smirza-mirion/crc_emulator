#pragma once

//uart header
//08/29/05


 /***********************************************************************/
 /*									*/
 /*  In the first register, UMR1 is for reads and UMR2 is for writes.	*/
 /*									*/
 /***********************************************************************/

#define UART_UMR1_RXRTS			(0x80)	/* Receive Request-to-Send	*/
#define UART_UMR1_RXIRQ			(0x40)	/* Receive Interrupt Select	*/
#define UART_UMR1_ERR			(0x20)	/* Error Mode			*/
#define UART_UMR1_PM_MULTI_ADDR		(0x1C)	/* Parity: Multidrop Adr Char	*/
#define UART_UMR1_PM_MULTI_DATA		(0x18)	/* Parity: Multidrop Data Char	*/
#define UART_UMR1_PM_NONE		(0x10)	/* Parity: None			*/
#define UART_UMR1_PM_FORCE_HI		(0x0C)	/* Parity: Force High		*/
#define UART_UMR1_PM_FORCE_LO		(0x08)	/* Parity: Force Low		*/
#define UART_UMR1_PM_ODD		(0x04)	/* Parity: Odd Parity		*/
#define UART_UMR1_PM_EVEN		(0x00)	/* Parity: Even Parity		*/
#define UART_UMR1_BC_5			(0x00)	/* 5 Bits Per Character		*/
#define UART_UMR1_BC_6			(0x01)	/* 6 Bits Per Character		*/
#define UART_UMR1_BC_7			(0x02)	/* 7 Bits Per Character		*/
#define UART_UMR1_BC_8			(0x03)	/* 8 Bits Per Character		*/

#define UART_UMR2_CM_NORMAL		(0x00)	/* Normal Channel Mode		*/
#define UART_UMR2_CM_ECHO		(0x40)	/* Automatic Echo Channel Mode	*/
#define UART_UMR2_CM_LOCAL_LOOP		(0x80)	/* Local Loopback Channel Mode	*/
#define UART_UMR2_CM_REMOTE_LOOP	(0xC0)	/* Remote Loopback Channel Mode	*/
#define UART_UMR2_TXRTS			(0x20)	/* Transmitter Ready-to-Send	*/
#define UART_UMR2_TXCTS			(0x10)	/* Transmitter Clear-to-Send	*/
#define UART_UMR2_STOP_BITS_1		(0x07)	/* 1 Stop Bits			*/
#define UART_UMR2_STOP_BITS_2		(0x0F)	/* 2 Stop Bits			*/
#define UART_UMR2_STOP_BITS(a)		((a)&0x0f)	/* Stop Bit Length	*/

 /***********************************************************************/
 /*									*/
 /*  In this next register, USR is for reads and UCSR is for writes.	*/
 /*									*/
 /***********************************************************************/

#define UART_USR_RB			(0x80)	/* Received Break		*/
#define UART_USR_FE			(0x40)	/* Framing Error		*/
#define UART_USR_PE			(0x20)	/* Parity Error			*/
#define UART_USR_OE			(0x10)	/* Overrun Error		*/
#define UART_USR_TXEMP			(0x08)	/* Transmitter Empty		*/
#define UART_USR_TXRDY			(0x04)	/* Transmitter Ready		*/
#define UART_USR_FFULL			(0x02)	/* FIFO Full			*/
#define UART_USR_RXRDY			(0x01)	/* Receiver Ready		*/

#define UART_UCSR_9600_BPS		(0xBB)	/* 9600 Baud w/ 3.6864 MHz clk	*/
#define UART_UCSR_TIMER			(0xDD)	/* Timer mode */
#define UART_UCSR_RCS(a)		(((a)&0x0f)<<4) /* Receiver Clk Select	*/
#define UART_UCSR_TCS(a)		((a)&0x0f) /* Transmitter Clock Select	*/

 /***********************************************************************/
 /*									*/
 /*  In this next register, there is only UCR is for writes.		*/
 /*									*/
 /***********************************************************************/

#define UART_UCR_NONE			(0x00)	/* No Command			*/
#define UART_UCR_STOP_BREAK		(0x70)	/* Stop Break			*/
#define UART_UCR_START_BREAK		(0x60)	/* Start Break			*/
#define UART_UCR_RESET_BKCHGINT		(0x50)	/* Reset Break-Change Interrupt	*/
#define UART_UCR_RESET_ERROR		(0x40)	/* Reset Error Status		*/
#define UART_UCR_RESET_TX		(0x30)	/* Reset Transmitter		*/
#define UART_UCR_RESET_RX		(0x20)	/* Reset Receiver		*/
#define UART_UCR_RESET_MR		(0x10)	/* Reset Mode Register Pointer	*/
#define UART_UCR_TX_DISABLED		(0x08)	/* Transmitter Disabled		*/
#define UART_UCR_TX_ENABLED		(0x04)	/* Transmitter Enabled		*/
#define UART_UCR_RX_DISABLED		(0x02)	/* Receiver Disabled		*/
#define UART_UCR_RX_ENABLED		(0x01)	/* Receiver Enabled		*/

 /***********************************************************************/
 /*									*/
 /*  In this next register, UIPCR is for reads and UACR is for writes.	*/
 /*									*/
 /***********************************************************************/

#define UART_UIPCR_COS			(0x10)	/* Change-of-State at IPx input	*/
#define UART_UIPCR_CTS			(0x01)	/* Current State of CTS pin	*/

#define UART_UACR_BRG			(0x80)	/* Set 2 of Baud Rate Generator	*/
#define UART_UACR_CTMS_TIMER		(0x60)	/* Timer Mode and Source Select	*/
							/* Must set this mode and src	*/
#define UART_UACR_IEC			(0x01)	/* Input Enable Control		*/

 /***********************************************************************/
 /*									*/
 /*  In this next register, UISR is for reads and UIMR is for writes.	*/
 /*									*/
 /***********************************************************************/

#define UART_UISR_COS			(0x80)	/* Change-of-State at CTS input	*/
#define UART_UISR_DB			(0x04)	/* Receiver Has Detected Break	*/
#define UART_UISR_RXRDY			(0x02)	/* Receiver Ready or FIFO Full	*/
#define UART_UISR_TXRDY			(0x01)	/* Transmitter Ready		*/

#define UART_UIMR_COS			(0x80)	/* Enable Change-of-State Intpt	*/
#define UART_UIMR_DB			(0x04)	/* Enable Delta Break Interrupt	*/
#define UART_UIMR_FFULL			(0x02)	/* Enable FIFO Full Interrupt	*/
#define UART_UIMR_TXRDY			(0x01)	/* Enable Transmitter Rdy Intpt	*/

 /*******************************************************************************/
 /*										*/
 /*  The last few registers can only be accessed as a read or write register.	*/
 /*										*/
 /*******************************************************************************/

#define UART_UIP_CTS			(0x01) /* Current State of CTS Input	*/

#define UART_UOP_RTS			(0x01) /* Sets All Bits on OP Bit Set	*/
#define UART_UOP0_RTS			(0x01) /* Clears All Bits on OP Bit Rst	*/

#define UART_BUFFER_SIZE 1024

//assigns usage for each uart
enum
{
    U_PR,       //printer
    U_PC,       //pc communications
};

#define PC_START_CHAR '@'

//returns from PC Communication
enum
{
    PC_RET_NOT_SET,
    PC_NO_ACTION,
    PC_START_FLAG,
    PC_CONTINUE,
    PC_ACCEPT,
    PC_NUM_DAILY,
    PC_ACC_SOURCES,
};

extern volatile unsigned char g_ucRelease;

void init_uart(int BaudRate, int iu);
void uart_putchar(int c, int iu);
void uart_write(char *str, int iu);
void uart_write_num(char *str, short num,int iu);
bool uart_char_waiting(int iu);
int uart_getchar(bool wait_flag, int iu);
bool is_pc_char_waiting(void);
void from_pc(bool ok_flag);
void from_pc_cdc(void);
bool rs_enabled(void);
short get_pc_ret(void);
void clear_pc_ret(void);
void set_pc_start_flag(void);
#ifdef TERMINAL
void pf(const char *format, ...);
void from_terminal(char key);
#endif // #ifdef TERMINAL

//simulation of remote sending character
#define REMOTE_START_CHAR '#'
void from_remote(void);

void ClearUart1Rx(void);
void ClearUart1Tx(void);
void ClearUart2Rx(void);
void ClearUart2Tx(void);

short unsigned int QueryPendingUart1Rx(void);
short unsigned int QueryPendingUart1Tx(void);
short unsigned int QueryPendingUart2Rx(void);
short unsigned int QueryPendingUart2Tx(void);

char PushUart1Rx(char cInput);
char PushUart1Tx(char cInput);
char PushUart2Rx(char cInput);
char PushUart2Tx(char cInput, char raw);

char PopUart1Rx(char *cOutput);
char PopUart1Tx(char *cOutput);
char PopUart2Rx(char *cOutput);
char PopUart2Tx(char *cOutput, char *raw);

char SendUart1Tx(char *cOutput);
char SendUart2Tx(char *cOutput);

void SendNAK1(void);
void SendNAK2(void);

void EnableTx1(void);
void EnableTx2(void);

void SendWakeUp(void);
void printUart2(void);

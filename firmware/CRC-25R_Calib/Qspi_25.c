/**
 * \file
 * \details This file contains functions, which calls the Coldfire SPI Module and implements calls to communicate with the iometer board.
 */
/**********************************************************

    MODULE: QSPI for CRC-25

    FILE: Qspi_25.c

    DATE: 08/25/06

This module contains all routines that use the QSPI (except SD card)

Chip selects:
            3   2   1   0
Chamber     1   0   0   0
HV          1   0   0   1
SD          1   1   1   0

Chamber:    Max132, uc_EEPROM, Remote
HV:         High Voltage for 6 channel MCA

*********************************************************************/
#include "crc.h"
#include "coldfire.h"
#include "qspi.h"
#include "pit.h"
#include "cs.h"
#include "lowlevelsnoop.h"
#include <string.h>
#ifdef SIMULATE_DECAY_SOURCE_ADC
	#include <stdlib.h>
#endif

volatile long QSPI_BUFFER0_PTR = -1;
volatile long QSPI_BUFFER0[500];
volatile long QSPI_BUFFER1_PTR = -1;
volatile long QSPI_BUFFER1[500];

extern volatile unsigned long int g_ulTickCounter;
extern volatile mcf5282 cf;
extern CHAMBER chamber[];
extern MEASUREMENT measurement[];
extern CURRENT current;
extern REMOTE remote[];
extern short max_chambers;

#ifdef SIMULATE_DECAY_SOURCE_ADC
	extern time_t clock_time;
	short NuclideData_getIndexFromNameIncludingUser(char *name);
	float NuclideData_getHalflife(short index);
	short NuclideData_getHalflifeUnit(short index);
	#define SIM_ADC_INITIAL_VALUE	255999.0
	time_t initial_clock_time = 0;
	int nuclideIndex;
#endif

static short iStartAlarm = 1;
static uchar rxbuffer[QSPI_MAX_BUFFER_SIZE];

static short adc_zero=160;					// adc zero offset
static long adc_value;						// adc value result
static ushort conv_type=ADC_READ_VOLTS;	// default to value, not zero
static short chamber_number = 0;
static short remote_counter;
static bool remote_done = FALSE;
bool adc_pinset = FALSE;     //output pins being set

ushort uiChamberErrorCountdown;

static uchar adc_status;
static uchar adc_result1;
static uchar adc_result0;
static uchar pol=0;							// adc "polarity" bit
static unsigned char flgFirstADCValue = 5;
static unsigned char m_ucGain[8] = {0,0,0,0,0,0,0,0};
static void get_next_chamber(void);
static volatile uchar disconnected_countdownFF[9] = {100,100,100,100,100,100,100,100,100};
//static volatile uchar disconnected_countdown00[9] = {100,100,100,100,100,100,100,100,100};

bool chamber_mcp3550_chip(short ch_num);
static char tx_data_remote(char ch);
void service_watchdog(void);

///////////////////////////////////////////////////////////////////////////////////////
	ushort adc_tick_time=0;
	enum adc_status
	{
	adc_idle,
	adc_max132_start_conversion,
	adc_max132_read_status,
	adc_max132_wait_for_eoc,
	adc_max132_start_read_result1,
	adc_max132_end_read_result1,
	adc_max132_start_read_result0,
	adc_max132_end_read_result0,
	adc_mcp3550_conversion,
	adc_max132_conversion,
    adc_service_remote
	};
	static enum adc_status adc_state;
//////////////////////////////////////////////

static bool adc_enabled;
#ifdef TERMINAL
int qspi_sim_adc_gain[2] = {-1, -1};
int qspi_sim_adc_value[2] = {0, 0};
#endif // #ifdef TERMINAL
/**
 * \details Set adc_state to idle
 * \returns None
 */
void SetADCStateToIdle(void){
	adc_state = adc_idle;
}

//enable or disable ADC
/**
 * \details Set adc_enabled variable
 * \param on Value to set to adc_enabled
 * \returns None
 */
void set_adc_enabled(bool on){

	adc_enabled = on;
}

//set enabled state of ADC to calling program
/**
 * \details Get adc_enabled value
 * \returns adc_enabled value
 */
bool get_adc_enabled(void){
	return(adc_enabled);
}
/**
 * \details Set the conv_type field in the MEASUREMENT structure
 * \param ch_num Chamber number
 * \param mode Value to set conv_type
 * \returns None
 */
void set_adc_mode(short ch_num, ushort mode){
	measurement[ch_num].conv_type = (mode == ADC_READ_ZERO) ? ADC_READ_ZERO : ADC_READ_VOLTS;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Set MAX132 Output Pins State
//
//	Input Parameters:	(uchar) pinstate - BITS0-3 are significant
//						(P3=Bit3....P0=bit0), BITS4-7 ignored
//	Output Parameters:	None
//	Return value:		None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 * \details Set MAX132 Output Pins State. This is used to switch the gain stage.
 * \param pinstate BITS0-3 are significant
 * \returns None
 */
void max132_set_output_pins(uchar pinstate){
    
        //set flag for pins being set
        //adc_pinset = TRUE;

    // Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

  // Setting the command Ram FIFO for using QSPI_CS transfers on this channel
  // Command Ram is set through the QDR
	cf.qspi.qdr = MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
						//MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD) , BIT = 0
						MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS); 		// CS Active Low

    //  First Write QSPI Address Register QAR See 22.5.5 in User's manual
	cf.qspi.qar = MCF5282_QSPI_QTR0 ; // Selects the first Ram Command entry offset

	// Write Single Data Value = MAX132 Command Input Register 1, To Set Output Pin State
	cf.qspi.qdr = MAX132_CR1_WRITE | ((pinstate & 0x0F) << 4);

    // See Table 22-6 in the users's manual
	// Halt after a Single Command Transaction, CS Inactive Level=HIGH
	cf.qspi.qwr = MCF5282_QSPI_QWR_HALT |			// Halt when Current Command Complete
						MCF5282_QSPI_QWR_CSIV |		// CS Returns to Inactive=HIGH
						MCF5282_QSPI_QWR_ENDQP(0);	// Last Transfer Position = 0

	//reset SPIF
	cf.qspi.qir |= 0x1;

	// Preserve QCD and DTL Values in QDLR Register
	// SET SPE bit 15 in QDLYR to start the QSPI Communication transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

    // Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF) );

	//reset SPIF
	//cf.qspi.qir |= 0x1;

	//turn off flag for pins being set
	//adc_pinset = FALSE;


}	// End max132_set_output_pins



//////////////////////////////////////////////////////////////////////////////////////////
//
//	Set MCP3550 Output Pins State
//
//	Input Parameters:	(uchar) pinstate - BITS0-3 are significant
//						(P3=Bit3....P0=bit0), BITS4-7 ignored
//	Output Parameters:	None
//	Return value:		None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 * \details Set MCP3550 Output Pins State. This is used to switch the gain stage.
 * \param ch Chamber number
 * \param pinstate BITS0-3 are significant
 * \returns None
 */
void mcp3550_set_output_pins(short ch, uchar pinstate){
		m_ucGain[ch] = pinstate;

        //set flag for pins being set
        //adc_pinset = TRUE;

    // Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
		cf.qspi.qar =	MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

  // Setting the command Ram FIFO for using QSPI_CS transfers on this channel
  // Command Ram is set through the QDR
		cf.qspi.qdr = MCF5282_QSPI_QCR_CONT |							// Chip Select remains selected after transfer is complete
					  MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |	// Delay after xfer enable set in QDLYR(DTL)
					  //MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD) , BIT = 0
					  MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS); 			// CS Active Low

		cf.qspi.qdr = MCF5282_QSPI_QCR_CONT |							// Chip Select remains selected after transfer is complete
					  MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |	// Delay after xfer enable set in QDLYR(DTL)
					//MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD) , BIT = 0
					  MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

		cf.qspi.qdr = MCF5282_QSPI_QCR_CONT |							// Chip Select remains selected after transfer is complete
					  MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |	// Delay after xfer enable set in QDLYR(DTL)
					//MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD) , BIT = 0
					  MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

		cf.qspi.qdr = MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |	// Delay after xfer enable set in QDLYR(DTL)
					//MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD) , BIT = 0
					  MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

		cf.qspi.qdr = MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |	// Delay after xfer enable set in QDLYR(DTL)
					//MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD) , BIT = 0
					  MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

    //  First Write QSPI Address Register QAR See 22.5.5 in User's manual
    	cf.qspi.qar =	MCF5282_QSPI_QTR0 ; // Selects the first Ram Command entry offset

	// Write Single Data Value = MAX132 Command Input Register 1, To Set Output Pin State
    	cf.qspi.qdr = m_ucGain[ch];
    	cf.qspi.qdr = m_ucGain[ch];
    	cf.qspi.qdr = m_ucGain[ch];
    	cf.qspi.qdr = m_ucGain[ch];
    	cf.qspi.qdr = m_ucGain[ch];

    // See Table 22-6 in the users's manual
	// Halt after a Single Command Transaction, CS Inactive Level=HIGH
	cf.qspi.qwr = MCF5282_QSPI_QWR_CSIV |		// CS Returns to Inactive=HIGH
						MCF5282_QSPI_QWR_ENDQP(4);	// Last Transfer Position = 0

	//reset SPIF
	cf.qspi.qir |= 0x1;

	// Preserve QCD and DTL Values in QDLR Register
	// SET SPE bit 15 in QDLYR to start the QSPI Communication transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

    // Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF) );

	//reset SPIF
	//cf.qspi.qir |= 0x1;

	//turn off flag for pins being set
	//adc_pinset = FALSE;
}	// End mcp3550_set_output_pins


//////////////////////////////////////////////////////////////////////////////////////////
//
//	Start adc Conversion
//
//	Input Parameters:	[uchar conv_type (0=Read Zero, 1=Read Vin))]
//	Output Parameters:	None
//	Return value:		None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/*static void max132_start_adc_conv(void){
   uchar i;
   ushort cmd,data;

	for (i=0; i<QSPI_MAX_BUFFER_SIZE; i++) rxbuffer[i]=0;

  // Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

  // Setting the command Ram FIFO for using QSPI_CS transfers on this channel
  // Command Ram is set through the QDR
	cmd = MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |     // Delay after xfer enable set in QDLYR(DTL)
                       // MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |   // CS to Clock Delay in QDLYR(QCD) bit = 0
                        MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);       // CS Active Low
        cf.qspi.qdr = cmd;
    
	// First Write QSPI Address Register QAR See 22.5.5 in User's manual
    	cf.qspi.qar =	MCF5282_QSPI_QTR0 ; // Selects the first Ram Command entry offset

	// Write Single Data Value = MAX132 Command Input Register 0, To Start Conversion

	data = MAX132_CR0_START_CONV |
                        MAX132_CR0_60HZ |   //  Default to 60Hz
                        (conv_type==0 ? MAX132_CR0_READ0 : MAX132_CR0_READVIN) |
                        MAX132_CR0_AWAKE |
                        MAX132_CR0_READSTATUS;
        cf.qspi.qdr = data;

    // See Table 22-6 in the users's manual
	// Halt after a Single Command Transaction, CS Inactive Level=HIGH
	cf.qspi.qwr = MCF5282_QSPI_QWR_HALT |	// Halt when Current Command Complete
						MCF5282_QSPI_QWR_CSIV |	// CS Returns to Inactive=HIGH
						MCF5282_QSPI_QWR_ENDQP(0); // Last Transfer Position = 0

	//reset SPIF
	cf.qspi.qir |= 0x1;

	// Preserve QCD and DTL Values in QDLR Register
	// SET SPE bit 15 in QDLYR to start the QSPI Communication transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

    // Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF) );

	//reset SPIF
	//cf.qspi.qir |= 0x1;

	// MAX132 has received the Command and will Output the Requested Status After Next Command



}	// End of max132_start_adc_conv */


//////////////////////////////////////////////////////////////////////////////////////////
//
//	max132_send_read_status_to_adc
//
//	Input Parameters:	None
//	Output Parameters:	None
//	Return value:		None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/* static void max132_send_read_status_to_adc(void){
	// Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

	// Setting the command Ram FIFO for using QSPI_CS transfers on this channel
	// Command Ram is set through the QDR

	cf.qspi.qdr = MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
						MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS); 		// CS Active Low

	// First Write QSPI Address Register QAR See 22.5.5 in User's manual
	cf.qspi.qar = MCF5282_QSPI_QTR0 ; // Selects the first Ram Command entry offset

	// Write Single Data Value = MAX132 Command Input Register 0
	cf.qspi.qdr = MAX132_CR0_60HZ |
           				MAX132_CR0_AWAKE |
           				MAX132_CR0_READVIN |
           				MAX132_CR0_READSTATUS;

	// See Table 22-6 in the users's manual
	// Halt after a Single Command Transaction, CS Inactive Level=HIGH
	cf.qspi.qwr = MCF5282_QSPI_QWR_HALT |		// Halt when Current Command Complete
						MCF5282_QSPI_QWR_CSIV |		// CS Returns to Inactive=HIGH
						MCF5282_QSPI_QWR_ENDQP(0);	// Last Transfer Position = 0`

	//reset SPIF
	cf.qspi.qir |= 0x1;

	// Preserve QCD and DTL Values in QDLR Register
	// SET SPE bit 15 in QDLYR to start the transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

	// Wait for spi transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF) );

	//reset SPIF
	//cf.qspi.qir |= 0x1;

	// MAX132 has received the Command and will Output the Requested Status After Next Command
}	// end max132_send_read_status_to_adc */




//////////////////////////////////////////////////////////////////////////////////////////
//
//	Reads adc Conversion Status
//
//	Input Parameters:	None
//	Output Parameters:	None
//	Return value:		None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/*static void max132_read_status_from_adc(void){
	ushort cmd,data;

	// Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar =	MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

	// Setting the command Ram FIFO for using QSPI_CS transfers on this channel
	// Command Ram is set through the QDR

	cmd = MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |     // Delay after xfer enable set in QDLYR(DTL)
                        MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);       // CS Active Low
	cf.qspi.qdr = cmd;

	// First Write QSPI Address Register QAR See 22.5.5 in User's manual
	cf.qspi.qar = MCF5282_QSPI_QTR0 ; // Selects the first Ram Command entry offset

	// Write Single Data Value = MAX132 Command Input Register 0
        
	data = MAX132_CR0_60HZ |
                        MAX132_CR0_AWAKE |
                        MAX132_CR0_READVIN |
                        MAX132_CR0_READSTATUS;
	cf.qspi.qdr = data;
        
	// See Table 22-6 in the users's manual
	// Halt after a Single Command Transaction, CS Inactive Level=HIGH
	cf.qspi.qwr = MCF5282_QSPI_QWR_HALT |		// Halt when Current Command Complete
						MCF5282_QSPI_QWR_CSIV |		// CS Returns to Inactive=HIGH
						MCF5282_QSPI_QWR_ENDQP(0);	// Last Transfer Position = 0`

	//reset SPIF
	cf.qspi.qir |= 0x1;

	// SET SPE bit 15 in QDLYR to start the transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

// ===== NOTE =====
// Could have another state to wait for this QSPI transfer to complete - depends
// on the 5282 qspi latency - measure it!! At the slowest qspi rate the transfer
// takes c.62uS - so waiting here should not present a problem
// ===== NOTE =====

	// Wait for QSPI transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF) );

	//reset SPIF
	//cf.qspi.qir |= 0x1;

	// Now Read the Status Register Value
	// set up pointer in RX ram
	cf.qspi.qar = MCF5282_QSPI_QRR0; // Selects the first Ram RX data offset

	// Read the Data transfered in
	adc_status = cf.qspi.qdr;
}	// end max132_read_status_from_adc() */


//////////////////////////////////////////////////////////////////////////////////////////
//
//	max132_start_read_result
//
//	Input Parameters:	[uchar conv_type (0=Read Zero, 1=Read Vin), uchar registerNum]
//	Output Parameters:	None
//	Return value:		None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/* static void max132_start_read_result(ushort registerNum){
	ushort cmd,data;
    
	// Sends Commands to MAX132 to Read Results. When EOC is Set, Send Commands to
	// Read the Output Register Values

	// Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

	// Set the command Ram FIFO for 1 transfer
	// Command Ram is set through the QDR
	cmd = MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |     // Delay after xfer enable set in QDLYR(DTL)
                        MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);       // CS Active Low
	cf.qspi.qdr = cmd;

	// First Write QSPI Address Register QAR See 22.5.5 in User's manual
	cf.qspi.qar = 	MCF5282_QSPI_QTR0 ; // Selects the first Ram Command entry offset

	// Send Command to Read OUTPUT Register 0

	data = MAX132_CR0_60HZ |
                MAX132_CR0_AWAKE |
                (conv_type==0 ? MAX132_CR0_READ0 : MAX132_CR0_READVIN) |
                registerNum;
	cf.qspi.qdr = data;

    // See Table 22-6 in the users's manual
	// Halt after 1 Command execution, CS Inactive Level=HIGH
	cf.qspi.qwr = MCF5282_QSPI_QWR_HALT |		// Halt when Current Command Complete
						MCF5282_QSPI_QWR_CSIV |		// CS Returns to Inactive=HIGH
						MCF5282_QSPI_QWR_ENDQP(0);	// Last Transfer Position=0

	//reset SPIF
	cf.qspi.qir |= 0x1;

	// Preserve QCD and DTL Values in QDLR Register
	// SET SPE bit 15 in QDLYR to start the transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

	// Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF) );

	//reset SPIF
	cf.qspi.qir |= 0x1;
}	// end start_read_result */


//////////////////////////////////////////////////////////////////////////////////////////
//
//	max132_end_read_result
//
//	Input Parameters:	None
//	Output Parameters:	None
//	Return value:		None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/*static void max132_end_read_result(ushort registerNum){

    ushort cmd,data;
    
	// REPEAT Command Because Results Delayed by 1 cycle
		cf.qspi.qar =	MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

	cmd = MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |     // Delay after xfer enable set in QDLYR(DTL)
                        //MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |   // dsck BIT = 0
                        MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);       // CS Active Low
        cf.qspi.qdr = cmd;

	// First Write QSPI Address Register QAR See 22.5.5 in User's manual
	cf.qspi.qar = MCF5282_QSPI_QTR0 ; // Selects the first Ram Command entry offset
    

	// Send Command to Read OUTPUT Register

	data = MAX132_CR0_60HZ |
                        MAX132_CR0_AWAKE |
                        (conv_type==0 ? MAX132_CR0_READ0 : MAX132_CR0_READVIN) |
                        registerNum;
        cf.qspi.qdr = data;

    // Halt after 1 Command execution, CS Inactive Level=HIGH
	cf.qspi.qwr = MCF5282_QSPI_QWR_HALT |     // Halt when Current Command Complete
                        MCF5282_QSPI_QWR_CSIV |     // CS Returns to Inactive=HIGH
                        MCF5282_QSPI_QWR_ENDQP(0);  // Last Transfer Position=0
        
	//reset SPIF
	cf.qspi.qir |= 0x1;

	// SET SPE bit 15 in QDLYR to start the transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

    // Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF) );

	//reset SPIF
	//cf.qspi.qir |= 0x1;

} // end max132_end_read_result */
/**
 * \details Start ADC conversion on the MAX132 chip
 * \returns True = OK, False = Error
 */
static bool max132_read_and_start(void){
	ushort cmd1;
	bool returnvalue;

	// Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset
	cmd1 =	MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
			MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low
	cf.qspi.qdr = cmd1;
	cf.qspi.qdr = cmd1;
	cf.qspi.qdr = cmd1;
	cf.qspi.qdr = cmd1;

	cf.qspi.qar = MCF5282_QSPI_QTR0 ; // Selects the first Ram Transmit entry offset
	cf.qspi.qdr = 0x02 | (conv_type==0 ? MAX132_CR0_READ0 : MAX132_CR0_READVIN);
	cf.qspi.qdr = 0x00 | (conv_type==0 ? MAX132_CR0_READ0 : MAX132_CR0_READVIN);
	cf.qspi.qdr = 0x04 | (conv_type==0 ? MAX132_CR0_READ0 : MAX132_CR0_READVIN);
	cf.qspi.qdr = 0x82 | (conv_type==0 ? MAX132_CR0_READ0 : MAX132_CR0_READVIN);

	cf.qspi.qwr =	MCF5282_QSPI_QWR_CSIV |	// CS Returns to Inactive=HIGH
					MCF5282_QSPI_QWR_ENDQP(3); // Last Transfer Position = 0

	//reset SPIF
	cf.qspi.qir |= 0x1;

	// Preserve QCD and DTL Values in QDLR Register
	// SET SPE bit 15 in QDLYR to start the QSPI Communication transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

	// Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF));

	cf.qspi.qar = MCF5282_QSPI_QRR0 + 1; // Selects the first Ram RX data offset

	if((adc_status & MAX132_SR_EOC) == 0){
		adc_status = 0x40;
		adc_result0 = 0x00;
		adc_result1 = 0x00;
		returnvalue = FALSE;
	}else{
		adc_status = cf.qspi.qdr;
		adc_result0 = cf.qspi.qdr;
		adc_result1 = cf.qspi.qdr;
		returnvalue = TRUE;
	}
	return returnvalue;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Set MAX132 Output Pins To Select Sensitivity and Analog Output Source
//
//	Input Parameters:	(uchar) Sensitivity (P1,P0) in Bits0-3, Bits 4-7 Ignored
//						(uchar) Analog Output Select (P3,P2) in Bits 0-3, Bits 4-7 ignored
//	Output Parameters:	None
//	Return value:		None
//
//////////////////////////////////////////////////////////////////////////////////////////
//

//////////////////////////////////////////////////////////////////////////////////////////
//
//	max132_service_adc - 	this function manages the state machine for starting
//							adc conversions and reading results back
//
//	QSPI Transfer Rate ~= 1 Bytes every 62uS at 291KHz
//	Don't need to test form MCF5282_QSPI_QIR_SPIF as this time has
//	expired well before this function is called by the 10mS timer tick
//
//	Input Parameters:	None
//	Output Parameters:	None
//	Return value:		None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 * \details Routine to read the ADC from either the Max132 chip or the MCP3550 chip. This is called from the PIT interrupt every 500 msec
 * \param msec_tstamp timestamp when the reading takes place
 * \returns None
 */
void service_adc(unsigned long int msec_tstamp){
	unsigned char ucChamber, ucByte0, ucByte1, ucByte2, ucByte3;
	//unsigned char ucDR, ucOH, ucOL;
	unsigned char ucOH, ucOL;
	long int lADCZero, lADC;
	short ch = chamber_number;
#ifdef SIMULATE_DECAY_SOURCE_ADC
	int value;
	float floatvalue;
	float decay_factor;
	float decay_adc;
#endif

	ucChamber = ch;

	//if output pins being set, just exit
	if(adc_pinset) return;

	conv_type = measurement[ch].conv_type;

	if(adc_state != adc_service_remote)
		//set up for ADC measurement of the chamber
		set_chamber_spi(ch, QSPI_ADC);

	switch (adc_state){
		case adc_idle:	// waiting for action!
			//only change state if connected
			if(chamber[ch].connected_flag){
				if(chamber_mcp3550_chip(ch)){
					adc_state = adc_mcp3550_conversion;
				}else{
					//adc_state = adc_max132_start_conversion;
					adc_state = adc_max132_conversion;
				}
			}else{
				//not connected, go to next chamber
				get_next_chamber();
			}
			break;

		/* case adc_max132_start_conversion:
			if(!current.demo_mode){
				max132_start_adc_conv();
			}
			adc_state = adc_max132_read_status;
			break;

		case adc_max132_read_status:
			if(!current.demo_mode){
				max132_send_read_status_to_adc();	// status read is a 2 part process
			}
			adc_state = adc_max132_wait_for_eoc;
			break;

		case adc_max132_wait_for_eoc:
			if(current.demo_mode){
				adc_state = adc_max132_start_read_result1;
				break;
			}

			max132_read_status_from_adc();		// 2nd part of Read Status
			if((adc_status & MAX132_SR_EOC) != 0){
				//if status = 0xff, chamber is not connected, set connected flag to False
				// and return state to idle
				if(adc_status == 0xff){
					chamber[ch].connected_flag = FALSE;
					adc_state = adc_idle;
					break;
				}

				// Save Polarity for Later
				// EOC set so can read results now...
				pol = adc_status & MAX132_SR_POLARITY;
				measurement[ch].polarity = pol;
				max132_start_read_result(MAX132_CR0_READOUTPUT1); // 2 part process
				adc_state = adc_max132_start_read_result1;
			}
			//else: EOC not set, read status again
			break;

		case adc_max132_start_read_result1:
			if(!current.demo_mode){
				max132_end_read_result(MAX132_CR0_READOUTPUT1);
				// set up pointer in RX ram
				cf.qspi.qar = MCF5282_QSPI_QRR0; // Selects the first Ram RX data offset

				// read QSPI  [cf.qspi.qdr] result data
				// Store Result1 in Global for Later Use
				adc_result1 = cf.qspi.qdr;	//single byte value returned from adc
			}
			adc_state = adc_max132_end_read_result1;
			break;

		case adc_max132_end_read_result1:
			if(current.demo_mode){
				adc_state=adc_max132_start_read_result0;
				break;
			}
			max132_start_read_result(MAX132_CR0_READOUTPUT0);	// 2 part process
			// now get Register 0
			adc_state=adc_max132_start_read_result0;
			break;

		case adc_max132_start_read_result0:
			if(!current.demo_mode){
				max132_end_read_result(MAX132_CR0_READOUTPUT0);		// Wait for 10mS before reading value
				// set up pointer in RX ram
				cf.qspi.qar = MCF5282_QSPI_QRR0; // Selects the first Ram RX data offset

				// Store Result0 in Global for Later Use
				adc_result0 = cf.qspi.qdr;	//single byte value returned from adc
			}
			adc_state = adc_max132_end_read_result0;
			break;

		case adc_max132_end_read_result0:
			if(current.demo_mode){
				switch(conv_type) {
					case ADC_READ_ZERO:
						adc_zero = 0;
						if(flgADCZeroRead_lADCZero[ch]) adc_zero = ADCZeroRead_lADCZero[ch];
						chamber[ch].adc_zero = adc_zero;
						lADCZero = adc_zero;
						PushADCZero(ucChamber, lADCZero);
						break;

					case ADC_READ_VOLTS:
						//adc_value = measurement[ch].adc_value + 1;
						adc_value = 0;
						if(flgADCRead_lADC[ch]) adc_value = ADCRead_lADC[ch];
						measurement[ch].adc_value = adc_value;
						lADC = adc_value;
						PushADCRead(ucChamber, lADC);
						break;
				}
				measurement[ch].valid_flag = TRUE;
				get_next_chamber();
				adc_state=adc_idle;
				break;
			}

			switch(conv_type){
				case ADC_READ_ZERO:
					adc_zero = (ushort)(adc_result1 << 8) + (ushort)adc_result0;
					if(flgADCZeroRead_lADCZero[ch]) adc_zero = ADCZeroRead_lADCZero[ch];
					chamber[ch].adc_zero = adc_zero;
					lADCZero = adc_zero;
					PushADCZero(ucChamber, lADCZero);
					break;

				case ADC_READ_VOLTS:
					adc_value = (ushort)(adc_result1 << 8) + (ushort)adc_result0;
					if(flgADCRead_lADC[ch]) adc_value = ADCRead_lADC[ch];
					measurement[ch].adc_value = adc_value;
					lADC = adc_value;
					PushADCRead(ucChamber, lADC);
					break;
			}

			//measurement now valid
			measurement[ch].valid_flag = TRUE;

			//set up to service remote
			if(remote[ch].exists && remote[ch].valid_flag){
				adc_state = adc_service_remote;
				remote_counter = 0;
				break;
			}

			//no remote to service, do next chamger
			get_next_chamber();
			adc_state=adc_idle;
			break; */

		case adc_max132_conversion:
			if(current.demo_mode){
				switch(conv_type) {
					case ADC_READ_ZERO:
						adc_zero = 0;
						if(flgADCZeroRead_lADCZero[ch]) adc_zero = ADCZeroRead_lADCZero[ch];
						chamber[ch].adc_zero = adc_zero;
						lADCZero = adc_zero;
						PushADCZero(ucChamber, lADCZero);
						break;

					case ADC_READ_VOLTS:
						//adc_value = measurement[ch].adc_value + 1;
						adc_value = 0;
						if(flgADCRead_lADC[ch]) adc_value = ADCRead_lADC[ch];
						measurement[ch].adc_value = adc_value;
						lADC = adc_value;
						PushADCRead(ucChamber, lADC);
						break;
				}
				measurement[ch].ulMilliSecStamp = msec_tstamp;
				if(measurement[ch].gainchangecountdown) measurement[ch].gainchangecountdown--;
				measurement[ch].valid_flag = TRUE;
				get_next_chamber();
				adc_state=adc_idle;
				break;
			}

			if(max132_read_and_start()){
				pol = adc_status & MAX132_SR_POLARITY;
				measurement[ch].polarity = pol;
				switch(conv_type){
					case ADC_READ_ZERO:
						adc_zero = (ushort)(adc_result1 << 8) + (ushort)adc_result0;
						if(flgADCZeroRead_lADCZero[ch]) adc_zero = ADCZeroRead_lADCZero[ch];
						chamber[ch].adc_zero = adc_zero;
						lADCZero = adc_zero;
						PushADCZero(ucChamber, lADCZero);
						break;

					case ADC_READ_VOLTS:
						adc_value = (ushort)(adc_result1 << 8) + (ushort)adc_result0;
						if(flgADCRead_lADC[ch]) adc_value = ADCRead_lADC[ch];
						measurement[ch].adc_value = adc_value;
						lADC = adc_value;
						PushADCRead(ucChamber, lADC);
						if(ch==0){
							if(QSPI_BUFFER0_PTR >= 0){
								QSPI_BUFFER0[QSPI_BUFFER0_PTR] = lADC;
								QSPI_BUFFER0_PTR++;
							}
						}else{
							if(QSPI_BUFFER1_PTR >= 0){
								QSPI_BUFFER1[QSPI_BUFFER1_PTR] = lADC;
								QSPI_BUFFER1_PTR++;
							}
						}
						break;
				}

				//measurement now valid
				measurement[ch].ulMilliSecStamp = msec_tstamp;
				if(measurement[ch].gainchangecountdown) measurement[ch].gainchangecountdown--;
				measurement[ch].valid_flag = TRUE;
			}else{
				if(ch==0){
					if(QSPI_BUFFER0_PTR >= 0){
						QSPI_BUFFER0[QSPI_BUFFER0_PTR] = -1234567;
						QSPI_BUFFER0_PTR++;
					}
				}else{
					if(QSPI_BUFFER1_PTR >= 0){
						QSPI_BUFFER1[QSPI_BUFFER1_PTR] = -1234567;
						QSPI_BUFFER1_PTR++;
					}
				}
			}

			//set up to service remote
			if(remote[ch].exists && remote[ch].valid_flag){
				adc_state = adc_service_remote;
				remote_counter = 0;
				break;
			}

			//no remote to service, do next chamger
			get_next_chamber();
			adc_state=adc_idle;
			break;

		case adc_mcp3550_conversion:
			if(!current.demo_mode){
				mcp3550_set_output_pins(ch, m_ucGain[ch]);
				switch(conv_type) {
					case ADC_READ_ZERO:
						adc_zero = 0;
						if (flgADCZeroRead_lADCZero[ch]) adc_zero = ADCZeroRead_lADCZero[ch];
						chamber[ch].adc_zero = adc_zero;
						lADCZero = adc_zero;
						PushADCZero(ucChamber, lADCZero);
						measurement[ch].ulMilliSecStamp = msec_tstamp;
						if(measurement[ch].gainchangecountdown) measurement[ch].gainchangecountdown--;
						measurement[ch].valid_flag = TRUE;
						break;

					case ADC_READ_VOLTS:
						cf.qspi.qar = MCF5282_QSPI_QRR0;
						ucByte3 = cf.qspi.qdr;
						ucByte2 = cf.qspi.qdr;
						ucByte1 = cf.qspi.qdr;
						ucByte0 = cf.qspi.qdr;

						lADC = ucByte0;
						lADC = lADC >> 7;
						adc_value = 0;
						adc_value = lADC;

						lADC = ucByte1;
						lADC = lADC << 1;
						adc_value = adc_value + lADC;

						lADC = ucByte2;
						lADC = lADC << 9;
						adc_value = adc_value + lADC;

						lADC = (ucByte3 & 0x1F);
						lADC = lADC << 17;
						adc_value = adc_value + lADC;

						adc_value = adc_value >> 4;

						//if ((ucByte3 & 0x80) == 0) ucDR = 0;
						//else ucDR = 1;

						if ((ucByte3 & 0x40) == 0) ucOL = 0;
						else ucOL = 1;

						if ((ucByte3 & 0x20) == 0) ucOH = 0;
						else ucOH = 1;

						if ((ucOH == 0) && (ucOL == 0) && ((ucByte3 & 0x10) == 0x10)) adc_value = adc_value | 0xFFFC0000;
						else if ((ucOH == 0) && (ucOL == 1)) adc_value = adc_value | 0xFFFC0000;
						adc_value = adc_value + 131072;

						if(flgADCRead_lADC[ch]) adc_value = ADCRead_lADC[ch];

						if (flgFirstADCValue) {
							flgFirstADCValue--;
						}else {
							lADC = adc_value;
							if ((ucOH == 1) && (ucOL == 1)) {
								//printf ("Illegal\n");

								if((ucByte3 == 0xFF) && (ucByte2 == 0xFF) && (ucByte1 == 0xFF) && (ucByte0 == 0xFF)){
									(disconnected_countdownFF[ch])--;
									//printf("FF countdown: %u\n", disconnected_countdownFF[ch]);
									if((disconnected_countdownFF[ch]) == 0){
										chamber[ch].connected_flag = FALSE;
										adc_state = adc_idle;
										//printf("ucByte3 = %X\n", ucByte3);
										//printf("ucByte2 = %X\n", ucByte2);
										//printf("ucByte1 = %X\n", ucByte1);
										//printf("ucByte0 = %X\n", ucByte0);
										//printf("chamber disconnected\n");
									}
								}
							}else {
//								if((ucByte3 ==0x00) && (ucByte2 == 0x00) && (ucByte1 == 0x00) && (ucByte0 == 0x00)){
//									(disconnected_countdown00[ch])--;
//									if(disconnected_countdown00[ch] == 0){
//										chamber[ch].connected_flag = FALSE;
//										adc_state = adc_idle;
//										//printf("ucByte3 = %X\n", ucByte3);
//										//printf("ucByte2 = %X\n", ucByte2);
//										//printf("ucByte1 = %X\n", ucByte1);
//										//printf("ucByte0 = %X\n", ucByte0);
//										//printf("chamber disconnected\n");
//									}
//								}else{
#ifndef SIMULATE_DECAY_SOURCE_ADC
									measurement[ch].adc_value = adc_value;
									if (adc_value < 0) measurement[ch].polarity = 1;
									else measurement[ch].polarity = 0;
#else
									if(initial_clock_time == 0){
										if(clock_time > 0){
											initial_clock_time = clock_time;
											nuclideIndex = NuclideData_getIndexFromNameIncludingUser("C 11  ");
										}
										measurement[ch].adc_value = adc_value;
										if (adc_value < 0) measurement[ch].polarity = 1;
										else measurement[ch].polarity = 0;
									}else{
										if(clock_time > initial_clock_time){
											decay_factor = nucdecay(1.0, initial_clock_time, clock_time, NuclideData_getHalflife(nuclideIndex),NuclideData_getHalflifeUnit(nuclideIndex));
											srand(clock_time);
											value = rand();
											value = value % 65535;
											value = value - 32768;
											floatvalue = value;
											floatvalue = floatvalue / 32768.0 * .15;
											floatvalue = 1.0 + floatvalue;
											decay_factor *= floatvalue;
											decay_adc = decay_factor * SIM_ADC_INITIAL_VALUE;
											if((decay_adc >=0) && (decay_adc <= SIM_ADC_INITIAL_VALUE)){
												measurement[ch].adc_value = decay_adc;
												measurement[ch].polarity = 0;
											}else{
												measurement[ch].adc_value = adc_value;
												if (adc_value < 0) measurement[ch].polarity = 1;
												else measurement[ch].polarity = 0;
											}
										}else{
											measurement[ch].adc_value = adc_value;
											if (adc_value < 0) measurement[ch].polarity = 1;
											else measurement[ch].polarity = 0;
										}
									}
#endif

									PushADCRead(ucChamber, lADC);
									//measurement now valid
									measurement[ch].ulMilliSecStamp = msec_tstamp;
									if(measurement[ch].gainchangecountdown) measurement[ch].gainchangecountdown--;
									measurement[ch].valid_flag = TRUE;
									//disconnected_countdown00[ch] = 2;
									disconnected_countdownFF[ch] = 10;

									if(ch==0){
										if(QSPI_BUFFER0_PTR >= 0){
											QSPI_BUFFER0[QSPI_BUFFER0_PTR] = lADC;
											QSPI_BUFFER0_PTR++;
										}
									}else{
										if(QSPI_BUFFER1_PTR >= 0){
											QSPI_BUFFER1[QSPI_BUFFER1_PTR] = lADC;
											QSPI_BUFFER1_PTR++;
										}
									}
//								}
							}
						}
						break;
				}
			}else{
				switch(conv_type){
					case ADC_READ_ZERO:
						adc_zero = 0;
						if (flgADCZeroRead_lADCZero[ch]) adc_zero = ADCZeroRead_lADCZero[ch];
						chamber[ch].adc_zero = adc_zero;
						lADCZero = adc_zero;
						PushADCZero(ucChamber, lADCZero);
						break;

					case ADC_READ_VOLTS:
#ifndef TERMINAL
						//adc_value = measurement[ch].adc_value + 1;
						adc_value = 0;
#else // #ifndef TERMINAL
						if(qspi_sim_adc_gain[ch] == -1){
							measurement[ch].gain = 0;
							adc_value = 0;
						}else{
							if(qspi_sim_adc_gain[ch] == 0){
								if(measurement[ch].gain == 0) adc_value = qspi_sim_adc_value[ch];
								else if(measurement[ch].gain == 2) adc_value = 199;
								else adc_value = 0;
							}else if(qspi_sim_adc_gain[ch] == 1){
								if(measurement[ch].gain == 0) adc_value = 256001;
								else if(measurement[ch].gain == 2) adc_value = qspi_sim_adc_value[ch];
								else adc_value = 0;
							}else{
								measurement[ch].gain = 0;
								adc_value = 0;
							}
						}
#endif // #ifndef TERMINAL
						if(flgADCRead_lADC[ch]) adc_value = ADCRead_lADC[ch];
						measurement[ch].adc_value = adc_value;
						lADC = adc_value;
						PushADCRead(ucChamber, lADC);
						break;
				}
				measurement[ch].ulMilliSecStamp = msec_tstamp;
				if(measurement[ch].gainchangecountdown) measurement[ch].gainchangecountdown--;
				measurement[ch].valid_flag = TRUE;
			}

			//set up to service remote
			if(remote[ch].exists && remote[ch].valid_flag){
				adc_state = adc_service_remote;
				remote_counter = 0;
				break;
			}

			//no remote to service, do next chamger
			get_next_chamber();
			adc_state=adc_idle;
			break;

		case adc_service_remote:
			//service the remote
			service_remote(ch);

			if(remote_done){
				//reset remote_done
				remote_done = FALSE;

				//do next chamber
				get_next_chamber();
				adc_state=adc_idle;
			}
			//remote not done, stay in same state
			adc_tick_time = 2;  //extra millisecond
			break;

		default:
			break;
	}
} // end service_adc()



//initialize qspi for adc
/*void  init_qspi(void)
{

   // See Chapter 22, TABLE 22-3 of the MCF5282UM
   // Note: Clearing LSByte disabes QSPI
   cf.qspi.qmr = 		MCF5282_QSPI_QMR_MSTR |			// Master Mode
   						MCF5282_QSPI_QMR_DOHIE |		// High Impedance between Transfers
   						MCF5282_QSPI_QMR_BITS_8 |		// 8 Bit Transfers
	// Data changes on FALLING clock edge,Sampled on LEADING clock edge - See MCF5282UM, Section 22.5.1
   						MCF5282_QSPI_QMR_BAUD(0xFF);	// 129.4KHz Rate - Divide by 255

    // Write the desired delay values  See Chapter 22 TABLE 22-5 of MCF5282UM
    // 7-0 are delay times in SPI clk periods after Xfer
    // no clocks before and after CS selected here
    cf.qspi.qdlyr= MCF5282_QSPI_QDLYR_QCD(0x20)
    					| MCF5282_QSPI_QDLYR_DTL(0x20);	  // Bit 15 starts transfers

   // QSPI Interupt Register  See table 22-6 in the manual
   // Set up QIR to enable write collision, abort bus errors and clear any interrupts
   // Writing a 1 to the LSB clears the SPI finished flag



    //initialize adc state
    adc_state = adc_idle;


}  // End of QSPI INIT  */



/**
 * \details Use Chip Selects 0-3 in the Coldfire register
 * \returns None
 */
void init_gpio_primary_funct_QSPI(void)
{
//	GPIO primary functions See MCF5282UM Section 26
//  cf.gpio.pqspar =  USE_PRIMARY_FUNCTION_QSPI_ONLY_CS0;

//	Use Chip Selects 0-3
	cf.gpio.pqspar =  USE_PRIMARY_FUNCTION_QSPI_CS0_3;
}


//////////////////////////////////////////////////////////////////////////////////////////
//  This Function consists of a number of read operations, executed (num_bytes) times,
//  and described as follows:-
//  This Read Operation sends two bytes on the SDO Pin to the eeprom. The First byte
//  includes the Read Command and A8 address bit. The 2nd byte is the Lower address
//  bits A0-A7.The Bytes are transmitted highest bit first. This is followed by a read
//  operation (with a dummy output from the 5282 QSPI) used to clock in
//  the data byte from the the Eeprom.
//
//  Input Parameters:   ushort address, ushort num_bytes
//  Output Parameters:  None
//  Return value:       uchar* to received data buffer
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 * \details Read from the EEPROM on the Chamber
 * \param address Starting Address of the EEPROM read
 * \param num_bytes Number of bytes to read
 * \returns Pointer to bytes array holding the result
 */
uchar *uc_eeprom_read(ushort address, ushort num_bytes){
	static uchar eeram[256];
    uchar address_bit8;
    uchar address_bits0_7;
    ushort count=0;
    uchar i;
    uchar rdaddr;
    ushort cmd1,cmd2;

    //NOTE: ***** !! This Code Assumes num_bytes does not cross A8 boundary !! ****
    address_bit8 = (address & 0x80) ? UC_EEPROM_A8 : 0;
    address_bits0_7 = (uchar)address & 0xff;

    for (i=0; i<255; i++) eeram[i] = 0;

    // Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

	// Set the command Ram FIFO for 1 transfer
	// Command Ram is set through the QDR
//	cmd1 =	MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
//			MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD)
//			MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_CONT) |	// Hold CS Active Between Transfers
//			MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low
//
//	cmd2 =	MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
//			MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD)
//			MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

	cmd1 = 0xb800;
	cmd2 = 0xa800;

	cf.qspi.qdr = cmd1;
	cf.qspi.qdr = cmd2;
	cf.qspi.qdr = cmd2;

	//cf.qspi.qdr = cmd1;
	//cf.qspi.qdr = cmd1;
	//cf.qspi.qdr = cmd2;

    for(count=0; count<num_bytes; count++){
    	rdaddr = address_bits0_7 + (uchar)count;

        // First Write QSPI Address Register QAR See 22.5.5 in User's manual
        cf.qspi.qar = MCF5282_QSPI_QTR0;							// Selects the first Ram Command entry offset

        // Send Command to Read Byte at address
        cf.qspi.qdr = UC_EEPROM_READ_COMMAND | address_bit8;
        cf.qspi.qdr = rdaddr;
        cf.qspi.qdr = address_bits0_7;								//dummy value, eeprom don't care

        // See Table 22-6 in the users's manual
        // Halt after 3 Command executed, CS Inactive Level=HIGH
        cf.qspi.qwr =	MCF5282_QSPI_QWR_CSIV |						// CS Returns to Inactive=HIGH
        				MCF5282_QSPI_QWR_ENDQP(2);					// Last Transfer Position=2,ie 3 commands

        // Reset SPIF
        cf.qspi.qir |= 0x1;

        // Preserve QCD and DTL Values in QDLR Register
        // SET SPE bit 15 in QDLYR to start the transfer
        cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

        // Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
        while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF) ) service_watchdog();

        // Reset SPIF
        //cf.qspi.qir |= 0x1;

        // Set up pointer in RX ram
        //cf.qspi.qar = MCF5282_QSPI_QRR0; // Selects the first Ram RX data offset
        // Read the Data transfered in
        //for ( i=0; i<QSPI_MAX_BUFFER_SIZE; i++)
        //{
        //    rxbuffer[i] = cf.qspi.qdr;
        //}
        //eeram[count] = rxbuffer[2];     //rxbuffer[2] is the memory byte!

        cf.qspi.qar = MCF5282_QSPI_QRR0 + 2;
        eeram[count] = cf.qspi.qdr;
    }
    return eeram;
}   // end uc_eeprom_read



//////////////////////////////////////////////////////////////////////////////////////////
//  This Function consists of a number of write operations, executed (num_bytes) times,
//  and described as follows:-
//  This Write Operstion sends two bytes on the SDO Pin to the Eeprom. The First byte
//  includes the Read Command and A8 address bit. The 2nd byte is the Lower address
//  bits A0-A7.The Bytes are transmitted highest bit first, this is followed by transmission
//  of the data byte
//
//  NOTE:   EEPROM Data Sheet Section 3.7 Data Protection: Any write operation subsequently
//          resets the write enable latch. A Write Enable Instruction must be issued
//          before any write operation. A write operation is defined as "a byte, page
//          or status register write".
//
//  Input Parameters:   ushort address, uchar * outbuffer, ushort num_bytes
//  Output Parameters:  None
//  Return value:       None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 * \details Write to the EEPROM on the Chamber
 * \param address Starting Address of the EEPROM write
 * \param outbuffer Pointer to byte array to write to EEPROM
 * \param num_bytes Number of bytes to write
 * \returns None
 */
void uc_eeprom_write(ushort address, uchar *outbuffer, ushort num_bytes){
	uchar *outbyte;
	ushort wraddr;
	uchar address_bit8;
	uchar address_bits0_7;
    ushort count=0;

    outbyte = outbuffer;

    address_bit8 = (address & 0x80) ? UC_EEPROM_A8 : 0;
    address_bits0_7 = (uchar)address & 0xff;

    for(count=0; count<num_bytes; count++){
    	wraddr = address_bits0_7 + (uchar)count;

    	// Wait if write in progress
    	// Set Write Enable Latch before any write operation!!
    	uc_eeprom_wren();

    	// Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
    	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

    	// Set the command Ram FIFO for 3 transfers
    	// Command Ram is set through the QDR

    	cf.qspi.qdr =	MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
                        MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD)
                        MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_CONT) |	// Hold CS Active Between Transfers
                        MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

        cf.qspi.qdr =   MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
                        MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD)
                        MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_CONT) |	// Hold CS Active Between Transfers
                        MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

        cf.qspi.qdr =   MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
                        MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD)
                        MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

        // First Write QSPI Address Register QAR See 22.5.5 in User's manual
        cf.qspi.qar =   MCF5282_QSPI_QTR0;									// Selects the first Ram Command entry offset

        // Send Command to Write Byte at address
        cf.qspi.qdr =   UC_EEPROM_WRITE_COMMAND | address_bit8;
        cf.qspi.qdr =   wraddr;
        cf.qspi.qdr =   *outbyte++;         //data output

        // See Table 22-6 in the users's manual
        // Halt after 3 Commands executed, CS Inactive Level=HIGH
        cf.qspi.qwr =   MCF5282_QSPI_QWR_CSIV |     // CS Returns to Inactive=HIGH
        				MCF5282_QSPI_QWR_ENDQP(2);  // Last Transfer Position=2,ie 3 commands

        // Reset SPIF
        cf.qspi.qir |= 0x1;

        // Preserve QCD and DTL Values in QDLR Register
        // SET SPE bit 15 in QDLYR to start the transfer
        cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

        // Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
        while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF) ) service_watchdog();

        // Reset SPIF
        //cf.qspi.qir |= 0x1;

        delay_msec(5);
    }
}   // end uc_eeprom_write



//////////////////////////////////////////////////////////////////////////////////////////
//  This Function sends the Write Disable command to the EEPROM
//
//  Input Parameters:   Mone
//  Output Parameters:  None
//  Return value:       None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 * \details This Function sends the Write Disable command to the EEPROM
 * \returns None
 */
void uc_eeprom_wrdi(void){
	// Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

	// Set the command Ram FIFO for 1 transfer
	// Command Ram is set through the QDR

	cf.qspi.qdr =	MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
					MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD)
					MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low


	//  First Write QSPI Address Register QAR See 22.5.5 in User's manual
	cf.qspi.qar =   MCF5282_QSPI_QTR0;									// Selects the first Ram Command entry offset

	// Send Command to Read Byte at address
	cf.qspi.qdr =   UC_EEPROM_WRITE_COMMAND | UC_EEPROM_WRITE_DISABLE;

	// See Table 22-6 in the users's manual
	// Halt after 1 Command execution, CS Inactive Level=HIGH
	cf.qspi.qwr =	MCF5282_QSPI_QWR_HALT |     // Halt when Current Command Complete
					MCF5282_QSPI_QWR_CSIV |     // CS Returns to Inactive=HIGH
					MCF5282_QSPI_QWR_ENDQP(0);  // Last Transfer Position=0, ie. 1 command

	// Reset SPIF
	cf.qspi.qir |= 0x1;

	// Preserve QCD and DTL Values in QDLR Register
	// SET SPE bit 15 in QDLYR to start the transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

	// Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF)) service_watchdog();

	// Reset SPIF
	//cf.qspi.qir |= 0x1;
}   // end uc_eeprom_wrdi


//////////////////////////////////////////////////////////////////////////////////////////
//  This Function sends the Write Enable command to the EEPROM
//
//  Input Parameters:   Mone
//  Output Parameters:  None
//  Return value:       None
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 * \details This Function sends the Write Enable command to the EEPROM
 * \returns None
 */
void uc_eeprom_wren(void){
	// Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

	// Set the command Ram FIFO for 1 transfer
	// Command Ram is set through the QDR
	cf.qspi.qdr =	MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
					MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD)
					MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

	//  First Write QSPI Address Register QAR See 22.5.5 in User's manual
	cf.qspi.qar =   MCF5282_QSPI_QTR0 ;         // Selects the first Ram Command entry offset

	// Send Command to Read Byte at address
	cf.qspi.qdr =   UC_EEPROM_WRITE_COMMAND | UC_EEPROM_WRITE_ENABLE;

	// See Table 22-6 in the users's manual
	// Halt after 1 Command executed, CS Inactive Level=HIGH
	cf.qspi.qwr =	MCF5282_QSPI_QWR_HALT |     // Halt when Current Command Complete
					MCF5282_QSPI_QWR_CSIV |     // CS Returns to Inactive=HIGH
					MCF5282_QSPI_QWR_ENDQP(0);  // Last Transfer Position=0, ie. 1 command

	// Reset SPIF
	cf.qspi.qir |= 0x1;

	// Preserve QCD and DTL Values in QDLR Register
	// SET SPE bit 15 in QDLYR to start the transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

	// Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF)) service_watchdog();

    // Reset SPIF
	//cf.qspi.qir |= 0x1;
}   // end uc_eeprom_wren


//////////////////////////////////////////////////////////////////////////////////////////
//  This Function Reads the Status Register in the EEPROM
//
//  Input Parameters:   Mone
//  Output Parameters:  None
//  Return value:       uchar status.
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 * \details This Function Reads the Status Register in the EEPROM
 * \returns Status code
 */
uchar uc_eeprom_rdsr(void){
	uchar i;

	// Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

    // Set the command Ram FIFO for 2 transfers!
    // Command Ram is set through the QDR
	cf.qspi.qdr =   MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |     // Delay after xfer enable set in QDLYR(DTL)
					MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |   // CS to Clock Delay in QDLYR(QCD)
					MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_CONT) |   // Hold CS Active Between Transfers
					MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);          // CS Active Low

	cf.qspi.qdr =   MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |     // Delay after xfer enable set in QDLYR(DTL)
					MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |   // CS to Clock Delay in QDLYR(QCD)
					MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);          // CS Active Low

	// First Write QSPI Address Register QAR See 22.5.5 in User's manual
	cf.qspi.qar =   MCF5282_QSPI_QTR0 ;         // Selects the first Ram Command entry offset

	// Send Command to Read the status register
	cf.qspi.qdr =   UC_EEPROM_READ_STATUS;
	cf.qspi.qdr =   0x7e;      //dummy command

	// See Table 22-6 in the users's manual
	// Halt after 2 Commands executed, CS Inactive Level=HIGH
	cf.qspi.qwr =   MCF5282_QSPI_QWR_CSIV |     // CS Returns to Inactive=HIGH
					MCF5282_QSPI_QWR_ENDQP(1);  // Last Transfer Position=1,ie 2 commands

	// Reset SPIF
	cf.qspi.qir |= 0x1;

	// Preserve QCD and DTL Values in QDLR Register
	// SET SPE bit 15 in QDLYR to start the transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

	// Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF)) service_watchdog();

	// Reset SPIF
	//cf.qspi.qir |= 0x1;

	// set up pointer in RX ram
	cf.qspi.qar = MCF5282_QSPI_QRR0; // Selects the first Ram RX data offset

	// Read the Data transfered in
	for ( i=0; i<QSPI_MAX_BUFFER_SIZE; i++)
	{
		rxbuffer[i] = cf.qspi.qdr;
	}

	return rxbuffer[1];
}   // end uc_eeprom_rdsr



//////////////////////////////////////////////////////////////////////////////////////////
//  This Function writes the EEPROM Status Register
//
//  Input Parameters:   uchar status
//  Output Parameters:  None
//  Return value:       uchar status.
//
//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 * \details This Function writes the EEPROM Status Register
 * \param status Value to write
 * \returns  None
 */
void uc_eeprom_wrsr(uchar status){
	// Set Write Enable Latch first!!
	uc_eeprom_wren();

	// Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
	cf.qspi.qar = MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

	// Set the command Ram FIFO for 2 transfers!
	// Command Ram is set through the QDR
	cf.qspi.qdr =   MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
					MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD)
					MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_CONT) |	// Hold CS Active Between Transfers
					MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

	cf.qspi.qdr =   MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |		// Delay after xfer enable set in QDLYR(DTL)
					MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |	// CS to Clock Delay in QDLYR(QCD)
					MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);				// CS Active Low

	// First Write QSPI Address Register QAR See 22.5.5 in User's manual
	cf.qspi.qar =   MCF5282_QSPI_QTR0 ;         // Selects the first Ram Command entry offset

	// Send Command to write status
	cf.qspi.qdr =   UC_EEPROM_WRITE_STATUS;
	cf.qspi.qdr =   status;                     //status value to write

	// See Table 22-6 in the users's manual
	// Halt after 2 Command executed, CS Inactive Level=HIGH
	// cf.qspi.qwr =   MCF5282_QSPI_QWR_HALT |     // Halt when Current Command Complete
	cf.qspi.qwr =   MCF5282_QSPI_QWR_CSIV |     // CS Returns to Inactive=HIGH
					MCF5282_QSPI_QWR_ENDQP(1);  // Last Transfer Position=0, ie. 1 command

	// Reset SPIF
	cf.qspi.qir |= 0x1;

	// Preserve QCD and DTL Values in QDLR Register
	// SET SPE bit 15 in QDLYR to start the transfer
	cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

	// Wait for transfer to complete,  QIR[SPIF] (bit 0) set to 1
	while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF)) service_watchdog();

	// Reset SPIF
	//cf.qspi.qir |= 0x1;
}   // end uc_eeprom_wrsr

#define NO_CMD      'P'
#define NO_CMD_SUB  '1'
#define REM_START_CHAR '!'
#define START_CHAR '$'
#define END_CHAR '#'
static char rec_buff[30];


void service_remote(short ch_num){
	char ch;
	uchar rec;
	short i;
	short istart;

	//select remote
	set_chamber_spi(ch_num,QSPI_REMOTE);

	//get character from remote buffer
	ch = remote[ch_num].buffer[remote_counter];

	//send the character in remote buffer & receive character back
	rec = tx_data_remote(ch);
	if(rec == 0xff) rec = '\0';     //fix ambiguous return if no remote

	//save received character in buffer
	rec_buff[remote_counter] = rec;

	//if End character, finished -- otherwise, increment counter
	if(ch == END_CHAR){
		remote_done = TRUE;

		//contents of remote buffer no longer valid
		remote[ch_num].valid_flag = FALSE;
    
		//rec_buff[remote_counter + 1] = '\0';
		//printf("rec: %s\r\n",rec_buff);

		//save command, if any
		istart = -1;
		for(i=0; i<=remote_counter; i++){
			if(rec_buff[i] == REM_START_CHAR){
				istart = i;
				break;
			}
		}
               
		if(istart != -1){   //got start character
			remote[ch_num].cmd = rec_buff[istart + 1];
			remote[ch_num].sub_cmd = rec_buff[istart + 2];
        }else{
        	remote[ch_num].cmd = 'O';   //no command
        }    
		//set chamber back to read ADC
		set_chamber_spi(ch_num,QSPI_ADC);
	}
	else ++remote_counter;
}

    

//transmit data to remote & receive back optional data from remote
static char tx_data_remote(char ch)
{
    ushort cmd,data;
    char rec_data;

  // Write QAR with 0x0020 to select the first COMMAND ADDRESS - See 22.4.1 & 22.5.5 MCF5282UM
        cf.qspi.qar =   MCF5282_QSPI_QCR0 ; // Selects the first Ram Command entry offset

  // Setting the command Ram FIFO for using QSPI_CS transfers on this channel
  // Command Ram is set through the QDR


         cmd =   MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DT) |     // Delay after xfer enable set in QDLYR(DTL)
                        //MCF5282_QSPI_QDR_COMMAND(MCF5282_QSPI_QCR_DSCK) |   // CS to Clock Delay in QDLYR(QCD) bit = 0
                        MCF5282_QSPI_QCR_CS(CHAMBER_QSPI_CS);       // CS Active Low
        cf.qspi.qdr = cmd;

    //  First Write QSPI Address Register QAR See 22.5.5 in User's manual
        cf.qspi.qar =   MCF5282_QSPI_QTR0 ; // Selects the first Ram Command entry offset

    // Write Single Data from remote
        
        
        data =   (ushort)ch;
        cf.qspi.qdr = data;
        
    // See Table 22-6 in the users's manual
    // Halt after a Single Command Transaction, CS Inactive Level=HIGH
        cf.qspi.qwr =   MCF5282_QSPI_QWR_HALT |     // Halt when Current Command Complete
                        MCF5282_QSPI_QWR_CSIV |     // CS Returns to Inactive=HIGH
                        MCF5282_QSPI_QWR_ENDQP(0);  // Last Transfer Position = 0`

    //reset SPIF
        cf.qspi.qir |= 0x1;

    // SET SPE bit 15 in QDLYR to start the transfer
        cf.qspi.qdlyr |= MCF5282_QSPI_QDLYR_SPE;

// ***** NOTE *****
    // Wait for QSPI transfer to complete,  QIR[SPIF] (bit 0) set to 1
        while (!(cf.qspi.qir & MCF5282_QSPI_QIR_SPIF) );

    //reset SPIF
        //cf.qspi.qir |= 0x1;

    // Now Read the Status Register Value
    // set up pointer in RX ram
        cf.qspi.qar = MCF5282_QSPI_QRR0; // Selects the first Ram RX data offset

    // Read the Data transfered in
        rec_data = cf.qspi.qdr;

        return(rec_data);

}    
/**
 * \details Set next chamber to read. This is executed by the service_adc routine to setup the next chamber for a read
 * \returns None
 */
static void get_next_chamber(void){
	short i, iFoundChamber;
	unsigned long ulMinTick;
	unsigned long ulStartOffset;
	unsigned long ulOffset;

	if(iStartAlarm == 1){
		iStartAlarm = 0;
		iFoundChamber = 0;
		for(i=0; i<max_chambers; i++){
			if(chamber[i].connected_flag) iFoundChamber++;
		}

		ulStartOffset = READ_CHAMBERS_IN_MSEC;
		ulStartOffset /= iFoundChamber;

		ulOffset = READ_CHAMBERS_IN_MSEC;
		ulOffset += g_ulTickCounter;

		for(i=0; i<max_chambers; i++){
			if(chamber[i].connected_flag){
				measurement[i].ulTickAlarm = ulOffset;
				ulOffset += ulStartOffset;
			}
		}

		for(i=0; i<max_chambers; i++){
			if(chamber[i].connected_flag){
				chamber_number = i;
				adc_tick_time = 0;
				break;
			}
		}
	}else{
		iFoundChamber = -1;
		ulMinTick = 0xFFFFFFFF;
		for(i=0; i<max_chambers; i++){
			if(chamber[i].connected_flag){
				if(measurement[i].ulTickAlarm < ulMinTick){
					ulMinTick = measurement[i].ulTickAlarm;
					iFoundChamber = i;
				}
			}
		}

		if(iFoundChamber != -1){
			chamber_number = iFoundChamber;
			ulMinTick = measurement[iFoundChamber].ulTickAlarm;
			if(ulMinTick <= g_ulTickCounter){
				adc_tick_time = 0;
				measurement[iFoundChamber].ulTickAlarm = g_ulTickCounter + READ_CHAMBERS_IN_MSEC;
			}else{
				adc_tick_time = ulMinTick - g_ulTickCounter;
				measurement[iFoundChamber].ulTickAlarm = ulMinTick + READ_CHAMBERS_IN_MSEC;
			}
		}else{
			adc_tick_time = READ_CHAMBERS_IN_MSEC;
		}
	}
}

/*	case adc_max132_start_conversion:
			if(!current.demo_mode){
				max132_start_adc_conv();
			}
			adc_state = adc_max132_read_status;
			break;

		case adc_max132_read_status:
			if(!current.demo_mode){
				max132_send_read_status_to_adc();	// status read is a 2 part process
			}
			adc_state = adc_max132_wait_for_eoc;
			break;

		case adc_max132_wait_for_eoc:
			if(current.demo_mode){
				adc_state = adc_max132_start_read_result1;
				break;
			}

			max132_read_status_from_adc();		// 2nd part of Read Status
			if((adc_status & MAX132_SR_EOC) != 0){
				//if status = 0xff, chamber is not connected, set connected flag to False
				// and return state to idle
				if(adc_status == 0xff){
					chamber[ch].connected_flag = FALSE;
					adc_state = adc_idle;
					break;
				}

				// Save Polarity for Later
				// EOC set so can read results now...
				pol = adc_status & MAX132_SR_POLARITY;
				measurement[ch].polarity = pol;
				max132_start_read_result(MAX132_CR0_READOUTPUT1); // 2 part process
				adc_state = adc_max132_start_read_result1;
			}
			//else: EOC not set, read status again
			break;

		case adc_max132_start_read_result1:
			if(!current.demo_mode){
				max132_end_read_result(MAX132_CR0_READOUTPUT1);
				// set up pointer in RX ram
				cf.qspi.qar = MCF5282_QSPI_QRR0; // Selects the first Ram RX data offset

				// read QSPI  [cf.qspi.qdr] result data
				// Store Result1 in Global for Later Use
				adc_result1 = cf.qspi.qdr;	//single byte value returned from adc
			}
			adc_state = adc_max132_end_read_result1;
			break;

		case adc_max132_end_read_result1:
			if(current.demo_mode){
				adc_state=adc_max132_start_read_result0;
				break;
			}
			max132_start_read_result(MAX132_CR0_READOUTPUT0);	// 2 part process
			// now get Register 0
			adc_state=adc_max132_start_read_result0;
			break;

		case adc_max132_start_read_result0:
			if(!current.demo_mode){
				max132_end_read_result(MAX132_CR0_READOUTPUT0);		// Wait for 10mS before reading value
				// set up pointer in RX ram
				cf.qspi.qar = MCF5282_QSPI_QRR0; // Selects the first Ram RX data offset

				// Store Result0 in Global for Later Use
				adc_result0 = cf.qspi.qdr;	//single byte value returned from adc
			}
			adc_state = adc_max132_end_read_result0;
			break;

		case adc_max132_end_read_result0:
			if(current.demo_mode){
				switch(conv_type) {
					case ADC_READ_ZERO:
						adc_zero = 0;
						if(flgADCZeroRead_lADCZero[ch]) adc_zero = ADCZeroRead_lADCZero[ch];
						chamber[ch].adc_zero = adc_zero;
						lADCZero = adc_zero;
						PushADCZero(ucChamber, lADCZero);
						break;

					case ADC_READ_VOLTS:
						//adc_value = measurement[ch].adc_value + 1;
						adc_value = 0;
						if(flgADCRead_lADC[ch]) adc_value = ADCRead_lADC[ch];
						measurement[ch].adc_value = adc_value;
						lADC = adc_value;
						PushADCRead(ucChamber, lADC);
						break;
				}
				measurement[ch].valid_flag = TRUE;
				get_next_chamber();
				adc_state=adc_idle;
				break;
			}

			switch(conv_type){
				case ADC_READ_ZERO:
					adc_zero = (ushort)(adc_result1 << 8) + (ushort)adc_result0;
					if(flgADCZeroRead_lADCZero[ch]) adc_zero = ADCZeroRead_lADCZero[ch];
					chamber[ch].adc_zero = adc_zero;
					lADCZero = adc_zero;
					PushADCZero(ucChamber, lADCZero);
					break;

				case ADC_READ_VOLTS:
					adc_value = (ushort)(adc_result1 << 8) + (ushort)adc_result0;
					if(flgADCRead_lADC[ch]) adc_value = ADCRead_lADC[ch];
					measurement[ch].adc_value = adc_value;
					lADC = adc_value;
					PushADCRead(ucChamber, lADC);
					break;
			}

			//measurement now valid
			measurement[ch].valid_flag = TRUE;

			//set up to service remote
			if(remote[ch].exists && remote[ch].valid_flag){
				adc_state = adc_service_remote;
				remote_counter = 0;
				break;
			}

			//no remote to service, do next chamger
			get_next_chamber();
			adc_state=adc_idle;
			break; */

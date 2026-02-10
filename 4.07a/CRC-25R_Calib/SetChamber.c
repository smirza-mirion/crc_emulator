/*********************************************************************
  MODULE:	SET CHAMBER

  FILE:		SetChamber.c

  DATE:		04/17/06

  ANALYSIS:
			setchamber:		 sets chamber's gain
			set_bias:		 sets chamber for bias measurement
			set_zero:		 sets chamber for zero adjust


  PASSED PARAMETERS:
		short ch_num			chamber number (0,1)
		short gain_num		0,1,2

  *************************************************************************/
#include "crc.h"
#include "qspi.h"
#include "chambfac.h"

// Gain0 = 0; Gain1 = 2, Gain2 = 1, TEST = 8  for two stage
// Gain0 = 0,  N/A     , Gain2 = 1, Test = 9 for one stage
#define TEST_INDEX   3
static const uchar gain_byte_one_stage[] = {0x0,0x02,0x01,0x09};
static const uchar gain_byte_two_stage[] = {0x0,0x02,0x01,0x08};

extern MEASUREMENT measurement[];
extern bool adc_pinset;     //output pins being set

void service_watchdog(void);

void setchamber(short ch_num , short gain_num){
	uchar gbyte;

	measurement[ch_num].gainchangecountdown = GAIN_CHANGE_COUNTDOWN_VALUE;

	if(chamber_one_gain_relay(ch_num)){
		gbyte = gain_byte_one_stage[gain_num];
	}else{
		gbyte = gain_byte_two_stage[gain_num];
	}

	if(measurement[ch_num].mode == TESTMODE){
		if(chamber_one_gain_relay(ch_num)){
			gbyte |= gain_byte_one_stage[TEST_INDEX];
		}else{
			gbyte |= gain_byte_two_stage[TEST_INDEX];
		}
	}

	//set flag for pins being set
	adc_pinset = TRUE;
	while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_ADC)) service_watchdog();

	//select chamber
	set_chamber_spi(ch_num, QSPI_ADC);
		
	//write the gain byte to realys by setting output pins
	if(chamber_mcp3550_chip(ch_num)){
		mcp3550_set_output_pins(ch_num, gbyte);
	}else{
		max132_set_output_pins(gbyte);
	}
	measurement[ch_num].gain = gain_num;

	//turn off flag for pins being set
	while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_ADC)) service_watchdog();
	adc_pinset = FALSE;
}

	/* set chamber to zero mode */
void set_zero(short ch_num){
	//set flag for pins being set
	adc_pinset = TRUE;
	while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_ADC)) service_watchdog();
        
	//select chamber
	set_chamber_spi(ch_num,QSPI_ADC);

	/* set to lowest gain */
	if(chamber_mcp3550_chip(ch_num)){
		if(chamber_one_gain_relay(ch_num)){
			mcp3550_set_output_pins(ch_num, gain_byte_one_stage[2]);
		}else{
			mcp3550_set_output_pins(ch_num, gain_byte_two_stage[2]);
		}
	}else{
		if(chamber_one_gain_relay(ch_num)){
			max132_set_output_pins(gain_byte_one_stage[2]);
		}else{
			max132_set_output_pins(gain_byte_two_stage[2]);
		}
	}

	/* set mode */
	measurement[ch_num].mode = ZEROMODE;
	measurement[ch_num].gain = 2;

	while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_ADC)) service_watchdog();

	//turn off flag for pins being set
	adc_pinset = FALSE;
}

	/* set chamber to do bias */
void set_bias(short ch_num){
	//set flag for pins being set
	adc_pinset = TRUE;
	while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_ADC)) service_watchdog();
        
	//select chamber
	set_chamber_spi(ch_num, QSPI_ADC);
        
	// set mode
	measurement[ch_num].mode = TESTMODE;
		
	measurement[ch_num].gain = 2;
	/* set relay to TEST */
	if(chamber_mcp3550_chip(ch_num)){
		if(chamber_one_gain_relay(ch_num)){
			mcp3550_set_output_pins(ch_num, gain_byte_one_stage[TEST_INDEX]);
		}else{
			mcp3550_set_output_pins(ch_num, gain_byte_two_stage[TEST_INDEX]);
		}
	}else{
		if(chamber_one_gain_relay(ch_num)){
			max132_set_output_pins(gain_byte_one_stage[TEST_INDEX]);
		}else{
			max132_set_output_pins((gain_byte_two_stage[TEST_INDEX] | 0x01));
		}
	}

	while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_ADC)) service_watchdog();
	//turn off flag for pins being set
	adc_pinset = FALSE;
}

/************************************************************
  MODULE:  header for QSPI   functions  for CRC-25

  FILE:  qspi.h

  DATE:  09/05/06
  **************************************************************/  

#ifndef __QSPI_H
#define __QSPI_H

#define SPI_SEMA_ACQUIRE	0
#define SPI_SEMA_RELEASE	1
#define SPI_SEMA_NULL		0
#define SPI_SEMA_ADC		1
#define SPI_SEMA_SDCARD		2
//#define SPI_SEMA_EEPROM		2
//#define SPI_SEMA_REMOTE		3

void spiClearSemaphore(void);
void spiSetBaud(int lockedBy, uchar baud);
int spiGetSemaphore(void);
void spiSemaphoreLoadValues(int lockedBy);
unsigned char spiSemaphore(int acq_release, int lockedBy);

#define  USE_PRIMARY_FUNCTION_QSPI_ONLY_CS0	0x0F
#define  USE_PRIMARY_FUNCTION_QSPI_CS0_3	0x7F
#define  SPI_DEBUG							0xFF

// Following QSPI Definitions extracted from mcf5282.h
// Bit level definitions and macros
#define MCF5282_QSPI_QMR_MSTR				(0x8000)
#define MCF5282_QSPI_QMR_DOHIE  			(0x4000)
#define MCF5282_QSPI_QMR_BITS_16  			(0x0000)
#define MCF5282_QSPI_QMR_BITS_8  			(0x2000)
#define MCF5282_QSPI_QMR_BITS_9  			(0x2400)
#define MCF5282_QSPI_QMR_BITS_10  			(0x2800)
#define MCF5282_QSPI_QMR_BITS_11  			(0x2C00)
#define MCF5282_QSPI_QMR_BITS_12  			(0x3000)
#define MCF5282_QSPI_QMR_BITS_13  			(0x3400)
#define MCF5282_QSPI_QMR_BITS_14  			(0x3800)
#define MCF5282_QSPI_QMR_BITS_15  			(0x3C00)
#define MCF5282_QSPI_QMR_CPOL   			(0x0200)
#define MCF5282_QSPI_QMR_CPHA   			(0x0100)
#define MCF5282_QSPI_QMR_BAUD(x)			(((x)&0x00FF))

#define MCF5282_QSPI_QDLYR_SPE				(0x8000)
#define MCF5282_QSPI_QDLYR_QCD(x)			(((x)&0x007F)<<8)
#define MCF5282_QSPI_QDLYR_DTL(x)			(((x)&0x00FF))

#define MCF5282_QSPI_QWR_HALT				(0x8000)
#define MCF5282_QSPI_QWR_WREN				(0x4000)
#define MCF5282_QSPI_QWR_WRTO				(0x2000)
#define MCF5282_QSPI_QWR_CSIV				(0x1000)
#define MCF5282_QSPI_QWR_ENDQP(x)			(((x)&0x000F)<<8)
#define MCF5282_QSPI_QWR_CPTQP(x)			(((x)&0x000F)<<4)
#define MCF5282_QSPI_QWR_NEWQP(x)			(((x)&0x000F))

#define MCF5282_QSPI_QIR_WCEFB				(0x8000)
#define MCF5282_QSPI_QIR_ABRTB				(0x4000)
#define MCF5282_QSPI_QIR_ABRTL				(0x1000)
#define MCF5282_QSPI_QIR_WCEFE				(0x0800)
#define MCF5282_QSPI_QIR_ABRTE				(0x0400)
#define MCF5282_QSPI_QIR_SPIFE				(0x0100)
#define MCF5282_QSPI_QIR_WCEF 				(0x0008)
#define MCF5282_QSPI_QIR_ABRT 				(0x0004)
#define MCF5282_QSPI_QIR_SPIF 				(0x0001)

#define MCF5282_QSPI_QAR_ADDR(x)			(((x)&0x003F))

#define MCF5282_QSPI_QDR_COMMAND(x)			(((x)&0xFF00))

#define MCF5282_QSPI_QCR_DATA(x)			(((x)&0x00FF)<<8)
#define MCF5282_QSPI_QCR_CONT				(0x8000)
#define MCF5282_QSPI_QCR_BITSE				(0x4000)
#define MCF5282_QSPI_QCR_DT					(0x2000)
#define MCF5282_QSPI_QCR_DSCK				(0x1000)
#define MCF5282_QSPI_QCR_CS(x)				(((x)&0x000F)<<8)
#define MCF5282_QSPI_QCR_CSBAR(x)			(((~x)&0x000F)<<8)

#define QSPI_MAX_BUFFER_SIZE	16

#define MCF5282_QSPI_QTR0	0x0000
#define MCF5282_QSPI_QRR0	0x0010
#define MCF5282_QSPI_QCR0	0x0020

#define MAX132_CR0_START_CONV	0x80	// CR0 Start Conversion Bit
#define MAX132_CR0_50HZ			0x40	// CR0 50Hz Bit
#define MAX132_CR0_60HZ			0x00	// CR0 60Hz Bit
#define MAX132_CR0_SLEEP		0x20	// CR0 Sleep Mode Bit
#define MAX132_CR0_AWAKE		0x00	// CR0 Awake Bit
#define MAX132_CR0_READ0		0x10	// CR0 Read Zero Bit
#define MAX132_CR0_READVIN		0x00	// CR0 Read VIN Bit
#define MAX132_CR0_READOUTPUT0	0x00	// CR0 Read Output Register 0
#define MAX132_CR0_READOUTPUT1	0x04	// CR0 Read Output Register 1
#define MAX132_CR0_READSTATUS	0x02	// CR0 Read Output Status Register

#define MAX132_CR1_WRITE		0x01	// CR1 Access

#define MAX132_SR_COLL			0x80	// SR Collision Signal
#define MAX132_SR_EOC			0x40	// SR End Of Conversion Signal
#define MAX132_SR_INTEGRATING	0x20	// SR Integrating Signal
#define MAX132_SR_SLEEP			0x10	// SR End Of Conversion Signal
#define MAX132_SR_POLARITY		0x08	// SR +/- Polarity Signal
#define MAX132_SR_LSBs			0x03	// SR LSBs B2/B1

#define ADC_READ_ZERO		0		// Read Zero Value
#define ADC_READ_VOLTS		1		// Read Voltage Input

//#define MAX132_CHIP_QSPI_CS     0x0f    // QSPI Chip Select Number for ADC

void    init_qspi(void);
void 	mcp3550_set_output_pins(short ch, uchar pinstate);
void	max132_set_output_pins(uchar pinstate);
void	service_adc(unsigned long int msec_tstamp);
void    set_adc_enabled(bool on);
bool    get_adc_enabled(void);
void    set_adc_mode(short ch_num, ushort mode);

void init_gpio_primary_funct_QSPI(void);
void init_qspi_dac(void);

#define UC_EEPROM_A8            0x08    // Command Bit A8

#define UC_EEPROM_READ_COMMAND  0x03    // Read Data Command
#define UC_EEPROM_WRITE_COMMAND 0x02    // Write Data Command

#define UC_EEPROM_WRITE_DISABLE 0x04    // Disables Write Operations
#define UC_EEPROM_WRITE_ENABLE  0x06    // Enables Write Operations

#define UC_EEPROM_READ_STATUS   0x05    // Read Status Register
#define UC_EEPROM_WRITE_STATUS  0x01    // Write Status Register


//#define UC_EEPROM_QSPI_CS       0x0f    // QSPI Chip Select Number for EEPROM

uchar *uc_eeprom_read(ushort address, ushort num_bytes);
void uc_eeprom_write(ushort address, uchar *outbuffer, ushort num_bytes);
void uc_eeprom_wrdi(void);
void uc_eeprom_wren(void);
uchar uc_eeprom_rdsr(void);
void uc_eeprom_wrsr(uchar status);

void set_chamber_spi(short ch_num, short mode);
void read_from_chamber(short ch_num,CHAMBERVALS *ch_vals);
void service_remote(short ch_num);

void eeprom_test(void);

enum
{
    QSPI_EEPROM,
    QSPI_ADC,
    QSPI_REMOTE,
};


//define Chip Selects
#define CHAMBER_QSPI_CS      0x08        //CS3
#define HV_QSPI_CS           0x00        //CS3,CS0
#define SD_QSPI_CS           0x0e        //CS3,CS2,CS1

// MMC-SD RAM Card definitions

// Bit 5 Read: SCR is 1 is SDRAM Card Present
#define MMC_CARD_INSERTED 0x20

// Bit 5 Write: 1 = Apply SDRAM Power
#define MMC_POWER_ON      0x20

bool init_mmc_card(void);
//void mmc_card_detect_debounce(void);

//bool test_mmc_rw(uint start, uint sector);
//void test_mmc_filesystem(void);

//definitions for Max152 Dual DAC
#define DAC_LOAD_REG_A  0x2000
#define DAC_LOAD_REG_B  0xa000
#define DAC_UPDATE_A    0x0400
#define DAC_UPDATE_B    0x1400


#endif

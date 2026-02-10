/**
 * \file
 * \details This file contains functions, which controls the access of the SPI channel between the SD Card and the IOmeter Board.
 */
#include "coldfire.h"
#include "crc.h"
#include "qspi.h"
#include "diskio.h"
#include "ff.h"

void SetADCStateToIdle(void);
//int spiGetDataBlock(unsigned long *dwaddr, unsigned long sector);
//int spiSetDataBlock(unsigned long *dwaddr, unsigned long sector);
void service_watchdog(void);

static int currentLockedBy = 0;
static int lastLoaded = 0;
static uchar spiBaud[5];
int (*spi_write_function)(void *data, unsigned long sector);
int (*spi_read_function) (void *data, unsigned long sector);
bool scsi_read(unsigned char *buffer, unsigned long int LBA, unsigned short int count);
bool scsi_write(unsigned char *buffer, unsigned long int LBA, unsigned short int count);
/**
 * \details Unlock the Semaphore state
 * \returns None
 */
void spiClearSemaphore(void){
	currentLockedBy = SPI_SEMA_NULL;
	lastLoaded = SPI_SEMA_NULL;
	spiBaud[SPI_SEMA_SDCARD] = 0xff;
	SetADCStateToIdle();
}
/**
 * \details Set the baud rate for different devices
 * \param lockedBy 1 = Chamber ADC, 2 = SD Card
 * \param baud SPI baud rate
 * \returns None
 */
void spiSetBaud(int lockedBy, uchar baud){
	spiBaud[lockedBy] = baud;
}

// Returns 0, if success at (acquire/release) semaphore, 0xff, if failed to (acquire/release) semaphore
//
// acq_release
// 0 = request to acquire semaphore
// 1 = request to release semaphore
//
// lockedBy
// 0 = invalid, will return failed (0xff)
// 1 = adc chamber
// 2 = eeprom chamber
// 3 = remote
// 4 = sd card
/**
 * \details Get Semaphore Status
 * \returns 0 = Unlocked, 1 = Chamber ADC, 2 = SD Card
 */
int spiGetSemaphore(void){
	return currentLockedBy;
}
/**
 * \details Load Coldfire SPI registers for either Chamber ADC or SD Card
 * \param lockedBy 1 = Chamber ADC, 2 = SD Card
 * \returns None
 */
void spiSemaphoreLoadValues(int lockedBy){
	int index;

	if(lastLoaded != lockedBy){
		switch(lockedBy){
			case SPI_SEMA_ADC:
				//cf.qspi.qmr = 0xE040;
				//cf.qspi.qdlyr = 0x2001;
				cf.qspi.qmr = 0xE0FF;
				cf.qspi.qdlyr = 0x2020;
				break;

			//case SPI_SEMA_EEPROM:
				//cf.qspi.qmr = 0xE040;
				//cf.qspi.qdlyr = 0x2001;
				//cf.qspi.qmr = 0xE0FF;
				//cf.qspi.qdlyr = 0x2020;
				//break;

			//case SPI_SEMA_REMOTE:
				//cf.qspi.qmr = 0xE0FF;
				//cf.qspi.qdlyr = 0x2020;
				//break;

			case SPI_SEMA_SDCARD:
				cf.qspi.qwr = 0x9000;	// Stop + active low
				cf.qspi.qmr = 0xE000 + spiBaud[lockedBy];
				cf.qspi.qdlyr = 0x1901;
				cf.qspi.qar = 0x20;
				cf.qspi.qdr = 0xde00;
				for(index=0; index<15; index++) cf.qspi.qdr = 0xce00;
				break;
		}
		lastLoaded = lockedBy;
	}
}
/**
 * \details Execute Semaphore Function
 * \param acq_release Semaphore Operation: 0 = Acquire Semaphore, 1 = Release Semaphore
 * \param lockedBy 1 = Chamber ADC, 2 = SD Card
 * \returns 0 = Success, FFh = Operation is blocked
 */
unsigned char spiSemaphore(int acq_release, int lockedBy){
	unsigned char returnvalue;
	unsigned int interrupt_state;
	int index;

	// disable interrupts
	interrupt_state = __DIR();
	if(((acq_release == SPI_SEMA_ACQUIRE) || (acq_release == SPI_SEMA_RELEASE)) && (lockedBy != SPI_SEMA_NULL)){
		if(acq_release){
			if(currentLockedBy == lockedBy){
				currentLockedBy = SPI_SEMA_NULL;
				returnvalue = 0x00;
			}else{
				returnvalue = 0xff;
			}
		}else{
			if(currentLockedBy == SPI_SEMA_NULL){
				currentLockedBy = lockedBy;
				returnvalue = 0x00;
			}else if(currentLockedBy == lockedBy){
				returnvalue = 0x00;
			}else{
				returnvalue = 0xff;
			}
		}
	}else returnvalue = 0xff;

	if((acq_release == SPI_SEMA_ACQUIRE) && (returnvalue == 0x00)){
		if(lastLoaded != lockedBy){
			switch(lockedBy){
				case SPI_SEMA_ADC:
					//cf.qspi.qmr = 0xE040;
					//cf.qspi.qdlyr = 0x2001;
					cf.qspi.qmr = 0xE0FF;
					cf.qspi.qdlyr = 0x2020;
					break;

				//case SPI_SEMA_EEPROM:
					//cf.qspi.qmr = 0xE040;
					//cf.qspi.qdlyr = 0x2001;
					//cf.qspi.qmr = 0xE0FF;
					//cf.qspi.qdlyr = 0x2020;
					//break;

				//case SPI_SEMA_REMOTE:
					//cf.qspi.qmr = 0xE0FF;
					//cf.qspi.qdlyr = 0x2020;
					//break;

				case SPI_SEMA_SDCARD:
					cf.qspi.qwr = 0x9000;	// Stop + active low
					cf.qspi.qmr = 0xE000 + spiBaud[lockedBy];
					cf.qspi.qdlyr = 0x1901;
					cf.qspi.qar = 0x20;
					cf.qspi.qdr = 0xde00;
					for(index=0; index<15; index++) cf.qspi.qdr = 0xce00;
					break;
			}
			lastLoaded = lockedBy;
		}
	}
	// enable interrupts
	__RIR(interrupt_state);
	return returnvalue;
}
/**
 * \details Read from either SD Card or USB Flash Drive
 * \param drive 0 = SD Card, 1 = USB Flash Drive
 * \param buffer Pointer to byte array, which receives the data
 * \param sectorNumber Starting sector number for read
 * \param sectorCount Number of sectors to read
 * \returns Status code
 */
DRESULT disk_read(BYTE drive, BYTE *buffer, DWORD sectorNumber, BYTE sectorCount){
	DRESULT returnvalue;
	int index, status;
	unsigned char *dataptr;

	returnvalue = RES_OK;
	returnvalue |= RES_ERROR;
	if(drive == 0){
		status = 0;
		dataptr = buffer;
		returnvalue = RES_OK;
		for(index=0; index<sectorCount; index++){
			while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_SDCARD)) service_watchdog();
			//status = spiGetDataBlock((unsigned long *) dataptr, sectorNumber + index);
			status = (*spi_read_function)((unsigned long *) dataptr, sectorNumber + index);
			while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_SDCARD)) service_watchdog();
			if(status) break;
			dataptr += 512;
		}
		if(status) returnvalue |= RES_ERROR;
	}else if(drive == 1){
		status = 0;
		returnvalue = RES_OK;
		status = scsi_read(buffer, sectorNumber, sectorCount);
		if(status) returnvalue |= RES_ERROR;
	}
	return returnvalue;
}
/**
 * \details Write to either SD Card or USB Flash Drive
 * \param drive 0 = SD Card, 1 = USB Flash Drive
 * \param buffer Pointer to data to write
 * \param sectorNumber Starting sector number for write
 * \param sectorCount Number of sectors to write
 * \returns Status code
 */
DRESULT disk_write(BYTE drive, const BYTE *buffer, DWORD sectorNumber, BYTE sectorCount){
	DRESULT returnvalue;
	int index, status;
	const BYTE *dataptr;

	returnvalue = RES_OK;
	returnvalue |= RES_ERROR;
	if(drive == 0){
		status = 0;
		dataptr = buffer;
		returnvalue = RES_OK;
		for(index=0; index<sectorCount; index++){
			while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_SDCARD)) service_watchdog();
			//status = spiSetDataBlock((unsigned long *) dataptr, sectorNumber + index);
			status = (*spi_write_function)((unsigned long *) dataptr, sectorNumber + index);
			while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_SDCARD)) service_watchdog();
			if(status) break;
			dataptr += 512;
		}
		if(status) returnvalue |= RES_ERROR;
	}else if(drive == 1){
		status = 0;
		returnvalue = RES_OK;
		status = scsi_write((unsigned char *)buffer, sectorNumber, sectorCount);
		if(status) returnvalue |= RES_ERROR;
	}
	return returnvalue;
}

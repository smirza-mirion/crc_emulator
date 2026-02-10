#define PHASE_LOWLEVEL_PRE_INIT	0
#define PHASE_LOWLEVEL_WAIT		1
#define PHASE_LOWLEVEL_OFF		2
#define PHASE_LOWLEVEL_CHAMBER	3
#define PHASE_LOWLEVEL_WELL		4
#define PHASE_LOWLEVEL_SAVE		5
#define PHASE_LOWLEVEL_CLEAR	6
#define PHASE_LOWLEVEL_MEMON	7
#define PHASE_LOWLEVEL_MEMOFF	8
#define PHASE_LOWLEVEL_TEST_PRE	9
#define PHASE_LOWLEVEL_TEST_WAIT 10
#define PHASE_LOWLEVEL_TEST_STOP 11
#define PHASE_LOWLEVEL_TEST		12

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "qspi.h"
#include "cs.h"
//#include "fat.h"
//#include "mmcapi.h"
#include "ff.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern volatile long QSPI_BUFFER0_PTR;
extern volatile long QSPI_BUFFER0[500];
extern volatile long QSPI_BUFFER1_PTR;
extern volatile long QSPI_BUFFER1[500];

void SetAmuletBackHTML(void);

void AmuletLowLevel_menu(void){
	static short currentmode;
	char message[25];
	static uchar *readptr;
	uchar *testptr;
	int index, jndex, kndex;
	uchar bytevalue;
	long filelen;
	char buffer[300];
	bool flgCheck;
	static long blockindex;
	long buf0ptr, buf1ptr;
	long buf0[500], buf1[500];
	//F_FILE *pFileHandle;
	//F_FILE *fptr, *sptr;
	FILINFO fileInfo;
	FIL fileObject, fileObject2;
	char longFileName[100];
	UINT bytesRead, bytesWritten;

	switch(m_iPhase){
		case PHASE_LOWLEVEL_PRE_INIT:
			if(m_ucClear == 49){
				currentmode = current.pccomm;
				if((currentmode != PC_COMM_USB_DEBUG_CHAMBER) && (currentmode != PC_COMM_USB_DEBUG_WELL)){
					currentmode = PC_COMM_NONE;
				}
				blockindex = 0;
				readptr = malloc(256 * 1024);
				m_ucClear = 0;
			}

			switch(currentmode){
				case PC_COMM_USB_DEBUG_CHAMBER:
					strcpy(message, "CHAMBER");
					break;

				case PC_COMM_USB_DEBUG_WELL:
					strcpy(message, "WELL");
					break;

				default:
					strcpy(message, "OFF");
					break;
			}
			SetAmuletString(100, message);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_LOWLEVEL_WAIT;
			break;

		case PHASE_LOWLEVEL_WAIT:
			break;

		case PHASE_LOWLEVEL_OFF:
			beep_amulet();
			currentmode = PC_COMM_NONE;
			m_iPhase = PHASE_LOWLEVEL_PRE_INIT;
			break;

		case PHASE_LOWLEVEL_CHAMBER:
			beep_amulet();
			currentmode = PC_COMM_USB_DEBUG_CHAMBER;
			m_iPhase = PHASE_LOWLEVEL_PRE_INIT;
			break;

		case PHASE_LOWLEVEL_WELL:
			beep_amulet();
			currentmode = PC_COMM_USB_DEBUG_WELL;
			m_iPhase = PHASE_LOWLEVEL_PRE_INIT;
			break;

		case PHASE_LOWLEVEL_SAVE:
			beep_amulet();

			//f_chdir("\\");
			f_chdrive(0);
			f_chdir("/");

			//if(f_chdir("options") != F_NO_ERROR){
			if(f_chdir("options") != FR_OK){
				//f_mkdir("options");
				f_mkdir("options");

				//f_chdir("\\");
				f_chdir("/");

				//if(f_chdir("options")!= F_NO_ERROR){
				if(f_chdir("options") != FR_OK){
					printf("Unable to make options\n");
				}else{
					//pFileHandle = f_open("lowcmb.opt", "w");
					f_open(&fileObject, "lowcmb.opt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

					//f_close(pFileHandle);
					f_close(&fileObject);
				}
			}

			//if(mmc_file_exists("lowcmb.opt") == 0) f_delete("lowcmb.opt");
			fileInfo.lfname = longFileName;
			fileInfo.lfsize = 100;
			if(f_stat("lowcmb.opt", &fileInfo) == FR_OK) f_unlink("lowcmb.opt");

			//if(mmc_file_exists("lowwell.opt") == 0) f_delete("lowwell.opt");
			fileInfo.lfname = longFileName;
			fileInfo.lfsize = 100;
			if(f_stat("lowwell.opt", &fileInfo) == FR_OK) f_unlink("lowwell.opt");

			switch(currentmode){
				case PC_COMM_USB_DEBUG_CHAMBER:
					//pFileHandle = f_open("lowcmb.opt", "w");
					f_open(&fileObject, "lowcmb.opt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

					//f_close(pFileHandle);
					f_close(&fileObject);
					break;

				case PC_COMM_USB_DEBUG_WELL:
					//pFileHandle = f_open("lowwell.opt", "w");
					f_open(&fileObject, "lowwell.opt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

					//f_close(pFileHandle);
					f_close(&fileObject);
					break;
			}

			switch(currentmode){
				case PC_COMM_USB_DEBUG_CHAMBER:
				case PC_COMM_USB_DEBUG_WELL:
					current.pccomm = currentmode;
					break;

				default:
					if(usb_pc_connected()) current.pccomm = PC_COMM_USB;
					current.pccomm = PC_COMM_RS232;
					break;
			}
			SetAmuletBackHTML();
			beep_amulet();
			break;

		case PHASE_LOWLEVEL_CLEAR:
			beep_amulet();

			//f_chdir("\\");
			//f_chdir("data");
			f_chdrive(0);
			f_chdir("/data");

			//if(mmc_file_exists("well.bin")==0) f_delete("well.bin");
			fileInfo.lfname = longFileName;
			fileInfo.lfsize = 100;
			if(f_stat("well.bin", &fileInfo) == FR_OK) f_unlink("well.bin");

			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_LOWLEVEL_WAIT;
			break;

		case PHASE_LOWLEVEL_MEMON:
			SetAmuletString(110, "Running Memory Test");
			SetAmuletByte(102, 0xFF);
			SetAmuletByte(103, 0xFF);
			m_iPhase = PHASE_LOWLEVEL_TEST_PRE;
			break;

		case PHASE_LOWLEVEL_MEMOFF:
			m_iPhase = PHASE_LOWLEVEL_WAIT;
			break;

		case PHASE_LOWLEVEL_TEST_PRE:
			__DI();
			QSPI_BUFFER0_PTR = 0;
			QSPI_BUFFER1_PTR = 0;
			__EI();
			m_iPhase = PHASE_LOWLEVEL_TEST;
			break;

		case PHASE_LOWLEVEL_TEST:
			testptr = readptr;
			bytevalue = 0;
			fileInfo.lfname = longFileName;
			fileInfo.lfsize = 100;

			for(index=0; index< 256 * 1024; index++){
				*testptr = bytevalue;
				bytevalue++;
				testptr++;
			}

			//f_delete("\\test.txt");
			f_chdrive(0);
			f_chdir("/");
			if(f_stat("test.txt", &fileInfo) == FR_OK) f_unlink("test.txt");

			//fptr = f_open("\\test.txt", "w+");
			f_open(&fileObject, "test.txt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

			for(index=0; index<40; index++){
				//f_write(readptr, 256 * 1024, 1, fptr);
				f_write(&fileObject, readptr, 256 * 1024, &bytesWritten);

				sprintf(buffer, "Wrote Block: %d\n", blockindex + index);
				SetAmuletString(110, buffer);
				SetAmuletByte(102, 0xFF);
			}
			//f_close(fptr);
			f_close(&fileObject);

			//filelen = f_filelength("\\test.txt");
			f_stat("test.txt", &fileInfo);
			filelen = fileInfo.fsize;

			if(filelen != 40 * 256 * 1024){
				//sptr = f_open("\\sd.log", "a+");
				f_open(&fileObject, "sd.log", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
				f_lseek(&fileObject, fileObject.fsize);

				sprintf(buffer, "File size is not correct, %ld bytes.\n", filelen);
				//f_write(buffer, strlen(buffer), 1, sptr);
				f_write(&fileObject, buffer, strlen(buffer), &bytesWritten);

				//f_close(sptr);
				f_close(&fileObject);
			}else{
				//fptr = f_open("\\test.txt", "r");
				f_open(&fileObject, "test.txt", FA_READ);

				for(index=0; index<40; index++){
					testptr = readptr;
					for(jndex=0; jndex<256*1024; jndex++) *testptr++ = 0;

					//f_read(readptr, 256 * 1024, 1, fptr);
					f_read(&fileObject, readptr, 256 * 1024, &bytesRead);

					flgCheck = TRUE;
					bytevalue = 0;
					testptr = readptr;
					for(jndex=0; jndex<256*1024; jndex++){
						if(*testptr != bytevalue){
							sprintf(buffer, "Error in Block: %d", index);
							SetAmuletString(110, buffer);
							SetAmuletByte(102, 0xFF);
							sprintf(buffer, "Mismatch detected, block: %ld, offset: %d (Expected = %u, Actual = %u)\n", blockindex, jndex, bytevalue, *testptr);

							//sptr = f_open("\\sd.log", "a+");
							f_open(&fileObject2, "sd.log", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
							f_lseek(&fileObject2, fileObject2.fsize);

							//f_write(buffer, strlen(buffer), 1, sptr);
							f_write(&fileObject2, buffer, strlen(buffer), &bytesWritten);

							//f_close(sptr);
							f_close(&fileObject2);
							flgCheck = FALSE;
							break;
						}
						testptr++;
						bytevalue++;
					}

					if(flgCheck){
						sprintf(buffer, "Verified Block: %ld", blockindex);
						SetAmuletString(110, buffer);
						SetAmuletByte(102, 0xFF);
						sprintf(buffer, "Block %ld is OK\n", blockindex);
						//sptr = f_open("\\sd.log", "a+");
						f_open(&fileObject2, "sd.log", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
						f_lseek(&fileObject2, fileObject2.fsize);

						//f_write(buffer, strlen(buffer), 1, sptr);
						f_write(&fileObject2, buffer, strlen(buffer), &bytesWritten);

						//f_close(sptr);
						f_close(&fileObject2);

						__DI();
						if(QSPI_BUFFER0_PTR > 0){
							buf0ptr = QSPI_BUFFER0_PTR;
							for(kndex=0; kndex<buf0ptr; kndex++) buf0[kndex] = QSPI_BUFFER0[kndex];
							QSPI_BUFFER0_PTR = 0;
						}else buf0ptr = 0;

						if(QSPI_BUFFER1_PTR > 0){
							buf1ptr = QSPI_BUFFER1_PTR;
							for(kndex=0; kndex<buf1ptr; kndex++) buf1[kndex] = QSPI_BUFFER1[kndex];
							QSPI_BUFFER1_PTR = 0;
						}else buf1ptr = 0;
						__EI();

						if(buf0ptr > 0){
							//sptr = f_open("\\channel0.log", "a+");
							f_open(&fileObject2, "channel0.log", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
							f_lseek(&fileObject2, fileObject2.fsize);

							for(kndex=0; kndex<buf0ptr; kndex++){
								sprintf(buffer, "%ld\n", buf0[kndex]);
								//f_write(buffer, strlen(buffer), 1, sptr);
								f_write(&fileObject2, buffer, strlen(buffer), &bytesWritten);
							}
							//f_close(sptr);
							f_close(&fileObject2);
						}

						if(buf1ptr > 0){
							//sptr = f_open("\\channel1.log", "a+");
							f_open(&fileObject2, "channel1.log", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
							f_lseek(&fileObject2, fileObject2.fsize);

							for(kndex=0; kndex<buf1ptr; kndex++){
								sprintf(buffer, "%ld\n", buf1[kndex]);
								//f_write(buffer, strlen(buffer), 1, sptr);
								f_write(&fileObject2, buffer, strlen(buffer), &bytesWritten);
							}
							//f_close(sptr);
							f_close(&fileObject2);
						}
					}
					blockindex++;
				}
				//f_close(fptr);
				f_close(&fileObject);
			}
			m_iPhase = PHASE_LOWLEVEL_TEST_WAIT;
			break;

		case PHASE_LOWLEVEL_TEST_WAIT:
			m_iPhase = PHASE_LOWLEVEL_TEST;
			break;

		case PHASE_LOWLEVEL_TEST_STOP:
			__DI();
			QSPI_BUFFER0_PTR = -1;
			QSPI_BUFFER1_PTR = -1;
			__EI();
			SetAmuletString(110, "Finished Test");
			SetAmuletByte(102, 0xFF);
			SetAmuletByte(104, 0xFF);
			m_iPhase = PHASE_LOWLEVEL_WAIT;
			break;

	}
}

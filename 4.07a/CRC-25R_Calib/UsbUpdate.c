#include <string.h>
#include "crc.h"
#include "sl811s.h"
#include "qspi.h"
//#include "fat.h"
//#include "mmcapi.h"
#include "ff.h"
#include "message.h"

void disable_all_interrupts(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void service_watchdog(void);
void MoveFileName(char *fname, char *lfname);
void service_amulet(void);

extern long PCComm_transferFileSize;

unsigned char UsbUpdate_seqNumber;
unsigned char UsbUpdate_buffer[1054];
short UsbUpdate_bufferEnd;
bool UsbUpdate_transferFinished = FALSE;
//F_FILE *UsbUpdate_fileHandle;
FIL UsbUpdate_fileObject;

void UsbUpdate_ReceiveFilePacket(void){
	unsigned char data[64];
	short index, bytesread;
	long checksum;
	long checksumlowbyte;
	char ok[4]={'$','O','#',0};
	char resend[4]={'$','R','#',0};
	unsigned char checksumbyte, single;
	unsigned char buffer[1024];
	long lFileSize, lCurrentPosition, chuncksize;
	unsigned char *byte0, *byte1, *byte2, *byte3;
	char sInstruction[2];
	char sFileName[13];
	char sPath[50];
	char message[104], titlestring[100], formatstring[104];
	char files[100][20];
	//F_FILE *writeHandle;
	//F_FIND find;
	DIR dirObject;
	FIL fileObject;
	FILINFO fileInfo;
	bool Error;
	UINT bytesWritten, bytesRead;
	char longFileName[100];
	char *extension;

	if(is_usb_char_waiting()){
		USB_Get_Data(data);
		if(PCComm_transferFileSize > 0){
			checksum = 0;
			for(index = 0; index< 63; index++){
				checksum = checksum + data[index];
			}
			checksumlowbyte = checksum % 256;

			if(data[63] == checksumlowbyte){
				if(data[62] == UsbUpdate_seqNumber){
					if(PCComm_transferFileSize >= 62){
						for(index=0; index<62; index++) UsbUpdate_buffer[UsbUpdate_bufferEnd + index] = data[index];
						UsbUpdate_bufferEnd = UsbUpdate_bufferEnd + 62;
						PCComm_transferFileSize = PCComm_transferFileSize - 62;
					}else{
						for(index=0; index<PCComm_transferFileSize; index++) UsbUpdate_buffer[UsbUpdate_bufferEnd + index] = data[index];
						UsbUpdate_bufferEnd = UsbUpdate_bufferEnd + PCComm_transferFileSize;
						PCComm_transferFileSize = 0;
					}

					if((PCComm_transferFileSize == 0) || (UsbUpdate_bufferEnd == 1054)){
						//f_write(UsbUpdate_buffer, UsbUpdate_bufferEnd, 1, UsbUpdate_fileHandle);
						f_write(&UsbUpdate_fileObject, UsbUpdate_buffer, UsbUpdate_bufferEnd, &bytesWritten);
						UsbUpdate_bufferEnd = 0;
					}
					UsbUpdate_seqNumber++;
					USB_Send_Data((uchar *)ok);

					if(PCComm_transferFileSize == 0){
						//f_close(UsbUpdate_fileHandle);
						f_close(&UsbUpdate_fileObject);
						UsbUpdate_transferFinished = TRUE;
						//disable_all_interrupts();
						//for(;;);
					}
				}else{
					//Send $R#
					USB_Send_Data((uchar *)resend);
				}
			}else{
				// Send $R#
				USB_Send_Data((uchar *)resend);
			}
		}
	}

	if(UsbUpdate_transferFinished){
		fileInfo.lfname = longFileName;
		fileInfo.lfsize = 100;
		f_chdrive(0);
		f_chdir("/temp");

		//if(mmc_file_exists("temp.bin") == 0){
		if(f_stat("temp.bin", &fileInfo) == FR_OK){
			// Copy current files to rollback
			// Run Checksum
			//lFileSize = f_filelength("temp.bin");
			lFileSize = fileInfo.fsize;
			lFileSize -= 5;

			//UsbUpdate_fileHandle = f_open("temp.bin", "r");
			f_open(&UsbUpdate_fileObject, "temp.bin", FA_READ);

			checksumbyte = 0;
			while(lFileSize > 0){
				if(lFileSize >= 1024){
					//f_read(buffer, 1, 1000, UsbUpdate_fileHandle);
					f_read(&UsbUpdate_fileObject, buffer, 1024, &bytesRead);
					bytesread = 1024;
					lFileSize -= 1024;
				}else{
					//f_read(buffer, 1, lFileSize, UsbUpdate_fileHandle);
					f_read(&UsbUpdate_fileObject, buffer, lFileSize, &bytesRead);
					bytesread = lFileSize;
					lFileSize = 0;
				}

				for(index=0; index<bytesread; index++) checksumbyte += buffer[index];
			}

			//f_seek(UsbUpdate_fileHandle, -5, F_SEEK_END);
			f_lseek(&UsbUpdate_fileObject, UsbUpdate_fileObject.fsize - 5);

			//f_read(buffer, 1, 1, UsbUpdate_fileHandle);
			f_read(&UsbUpdate_fileObject, buffer, 1, &bytesRead);

			if(checksumbyte == buffer[0]){
				Error = FALSE;
				// Parse and Update individual files
				fileInfo.lfname = longFileName;
				fileInfo.lfsize = 100;
				//lFileSize = f_filelength("temp.bin");
				f_stat("temp.bin", &fileInfo);
				lFileSize = fileInfo.fsize;
				lFileSize -= 5;
				lCurrentPosition = 0;

				//f_seek(UsbUpdate_fileHandle, 0, F_SEEK_SET);
				f_lseek(&UsbUpdate_fileObject, 0);

				while(lCurrentPosition<lFileSize){
					index = 0;
					single = 1;
					while(single != 0) {
						//f_read(&single, 1, 1, UsbUpdate_fileHandle);
						f_read(&UsbUpdate_fileObject, &single, 1, &bytesRead);
						buffer[index++] = single;
					}
					lCurrentPosition += index;
					//printf("%s\n", buffer);

					sInstruction[0] = buffer[0];
					sInstruction[1] = 0;
					index = strlen((char *)buffer);
					while(buffer[index] != '\\') index--;
					strcpy(sFileName, (char *) &(buffer[index+1]));
					buffer[index] = 0;
					strcpy(sPath, (char *) &(buffer[1]));
					//printf("Instruction: %s\n", sInstruction);
					//printf("Path: %s\n", sPath);
					//printf("FileName: %s\n", sFileName);

					// Convert little endian to big endian
					byte0 = (unsigned char *) &chuncksize;
					byte1 = byte0 + 1;
					byte2 = byte0 + 2;
					byte3 = byte0 + 3;
					//f_read(&single, 1, 1, UsbUpdate_fileHandle);
					f_read(&UsbUpdate_fileObject, &single, 1, &bytesRead);
					*byte3 = single;

					//f_read(&single, 1, 1, UsbUpdate_fileHandle);
					f_read(&UsbUpdate_fileObject, &single, 1, &bytesRead);
					*byte2 = single;

					//f_read(&single, 1, 1, UsbUpdate_fileHandle);
					f_read(&UsbUpdate_fileObject, &single, 1, &bytesRead);
					*byte1 = single;

					//f_read(&single, 1, 1, UsbUpdate_fileHandle);
					f_read(&UsbUpdate_fileObject, &single, 1, &bytesRead);
					*byte0 = single;

					lCurrentPosition += 4;

					//printf("Chunck Size: %ld\n", chuncksize);

					//if(f_chdir(sPath)== F_NO_ERROR) {
					if(f_chdir(sPath) == FR_OK){
						lCurrentPosition += chuncksize;
						fileInfo.lfname = longFileName;
						fileInfo.lfsize = 100;
						//if(mmc_file_exists("a") == 0) f_delete("a");
						if(f_stat("a", &fileInfo) == FR_OK) f_unlink("a");

						//writeHandle = f_open("a", "w");
						f_open(&fileObject, "a", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

						while(chuncksize>0){
							if(chuncksize>=1024){
								//f_read(buffer, 1000, 1, UsbUpdate_fileHandle);
								f_read(&UsbUpdate_fileObject, buffer, 1024, &bytesRead);

								//f_write(buffer, 1000, 1, writeHandle);
								f_write(&fileObject, buffer, 1024, &bytesWritten);
								chuncksize -= 1024;
							}else{
								//f_read(buffer, chuncksize, 1, UsbUpdate_fileHandle);
								f_read(&UsbUpdate_fileObject, buffer, chuncksize, &bytesRead);

								//f_write(buffer, chuncksize, 1, writeHandle);
								f_write(&fileObject, buffer, chuncksize, &bytesWritten);
								chuncksize = 0;
							}
						}
						//f_close(writeHandle);
						f_close(&fileObject);

						//if(mmc_file_exists("a") == 0){
						if(f_stat("a", &fileInfo) == FR_OK){
							if(sInstruction[0] == 'O'){
								for(index=0; index<100; index++) files[index][0] = 0;

								index = 0;
								//if(f_findfirst("*.dat", &find)==0){
								//	strcpy(&(files[index++][0]), find.filename);
								//	while((index<100) && (f_findnext(&find)==0)) strcpy(&(files[index++][0]), find.filename);
								//}
								fileInfo.lfname = longFileName;
								fileInfo.lfsize = 100;
								f_opendir(&dirObject, ".");
								do{
									longFileName[0] = 0;
									f_readdir(&dirObject, &fileInfo);
									MoveFileName(fileInfo.fname, longFileName);
									if(strlen(longFileName) > 4){
										extension = &(longFileName[strlen(longFileName) - 4]);
										if(strcmp(extension, ".dat") == 0 || strcmp(extension, ".DAT") == 0){
											if(index < 100){
												strcpy(&(files[index][0]), longFileName);
												index++;
											}
										}
									}
								}while(longFileName[0] != 0);

								for(index=0; index<100; index++){
									//if(strlen(&files[index][0]) != 0) f_delete(&files[index][0]);
									if(strlen(&(files[index][0])) != 0) f_unlink(&files[index][0]);
								}
							}else if(sInstruction[0] == 'R'){
								//if(mmc_file_exists(sFileName) == 0) f_delete(sFileName);
								if(f_stat(sFileName, &fileInfo) == FR_OK) f_unlink(sFileName);
							}

							//mmc_rename_file("a", sFileName);
							f_rename("a", sFileName);
						}
					}else{
						Error = TRUE;
						get_amulet_message(L_FILE_TRANSFER_ERROR, titlestring);    // "File Transfer Error"
						get_amulet_message(L_UNABLE_TO_FIND_FILE_PATH, formatstring);    // "Unable to find file path: %s\n"
						//sprintf(message, "Unable to find file path: %s\n", sPath);
						sprintf(message, formatstring, sPath);
						//Amulet_DisplayError("File Transfer Error", message,TRUE);
						Amulet_DisplayError(titlestring, message,TRUE);
						break;
					}
				}

				// reboot
				if(!Error){
					//f_chdir("\\temp");
					f_chdir("/temp");

					//f_delete("temp.bin");
					f_unlink("temp.bin");

					UsbUpdate_transferFinished = FALSE;

					get_amulet_message(L_UPDATE, titlestring);    // "Update"
					get_amulet_message(L_POWER_OFF_AND_THEN_POWER_ON_TO_COMPLETE_UPDATE, formatstring);    // "Power Off,\nand then Power On\nto complete update."
					//sprintf(message, "Power Off,\nand then Power On\nto complete update.");

					//Amulet_DisplayError("Update", message, FALSE);
					Amulet_DisplayError(titlestring, formatstring, FALSE);

					for(index=0;index<10;index++){
						service_amulet();
						delayloop(50);
					}

					disable_all_interrupts();
					for(;;) service_watchdog();
				}
			}else{
				get_amulet_message(L_FILE_TRANSFER_ERROR, titlestring);    // "File Transfer Error"
				get_amulet_message(L_CHECKSUM_HAS_FAILED, formatstring);    // "Checksum has failed."
			    //Amulet_DisplayError("File Transfer Error", "Checksum has failed.",TRUE);
				Amulet_DisplayError(titlestring, formatstring, TRUE);
			}
		}else{
			get_amulet_message(L_FILE_TRANSFER_ERROR, titlestring);    // "File Transfer Error"
			get_amulet_message(L_UNABLE_TO_FIND_TRANSFER_FILE, formatstring);    // "Unable to find transfer file."
			//Amulet_DisplayError("File Transfer Error", "Unable to find transfer file.",TRUE);
			Amulet_DisplayError(titlestring, formatstring,TRUE);
		}

		UsbUpdate_transferFinished = FALSE;
	}
}

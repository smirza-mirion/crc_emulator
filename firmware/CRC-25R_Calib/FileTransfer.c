#include <time.h>
#include "crc.h"
#include "sl811s.h"
#include "qspi.h"
//#include "fat.h"
//#include "mmcapi.h"
#include "ff.h"

//F_FILE *FileTransfer_pointer;
FIL FileTransfer_fileObject;
unsigned long FileTransfer_filesize;
bool FileTransfer_lastBlock;
short FileTransfer_pccomm;

#define LOW_LEVEL_BUF_SIZE 501
extern unsigned char ucLowLevelBuf[LOW_LEVEL_BUF_SIZE][63];
extern unsigned long ulLowLevelHead;
extern unsigned long ulLowLevelTail;
extern bool clear_to_send1;
extern CURRENT current;
//extern bool PCComm_adcstate;

int PopLowLevel(unsigned char *data);

static void PushLowLevelTransfer(unsigned char data[63]){
	short index;

	for(index=0; index<63; index++) ucLowLevelBuf[ulLowLevelTail][index] = data[index];
	ulLowLevelTail++;
	if(ulLowLevelTail == LOW_LEVEL_BUF_SIZE) ulLowLevelTail = 0;
	if(ulLowLevelTail == ulLowLevelHead){
		ulLowLevelHead++;
		if(ulLowLevelHead == LOW_LEVEL_BUF_SIZE) ulLowLevelHead = 0;
	}
}

void FileTransfer_UploadFile(void){
	unsigned char data[64];
	unsigned short int blocknum;
	unsigned long int fileoffset, remaining;
	unsigned char *ptr;
	unsigned char block[7936];
	short index, jndex;
	unsigned char blockmask;
	unsigned char find0;
	UINT bytesRead;

	if(is_usb_char_waiting()){
		USB_Get_Data(data);
		if((data[0] == 'W') && (data[1] == 'L')){
			switch(data[2]){
				case 'S':
					ptr = (unsigned char *) &blocknum;
					*ptr = data[3];
					ptr++;
					*ptr = data[4];

					if((data[4] & 0x01) == 0x01) blockmask = 0x80;
					else blockmask = 0x00;

					fileoffset = blocknum;
					fileoffset *= 7936;
					//f_seek(FileTransfer_pointer, fileoffset, F_SEEK_SET);
					f_lseek(&FileTransfer_fileObject, fileoffset);

					remaining = FileTransfer_filesize - fileoffset;
					//if(remaining > 7936) f_read(block, 7936, 1, FileTransfer_pointer);
					if(remaining > 7936) f_read(&FileTransfer_fileObject, block, 7936, &bytesRead);
					else{
						for(index=0; index<7936; index++) block[index] = 0;
						//f_read(block, remaining, 1, FileTransfer_pointer);
						f_read(&FileTransfer_fileObject, block, remaining, &bytesRead);
						FileTransfer_lastBlock = TRUE;
					}
					for(index=0; index<128; index++){
						data[0] = index;
						data[0] = data[0] | blockmask;
						for(jndex=0; jndex<62; jndex++) data[jndex+1] = block[62*index + jndex];
						PushLowLevelTransfer(data);
					}
					break;

				case 'R':
					if((data[3] & 0x01) == 0x01) find0 = 0x80;
					else find0 = 0x00;
					find0 = find0 + (data[4] & 0x7F);

					index = ulLowLevelTail;
					index --;
					if(index < 0) index = LOW_LEVEL_BUF_SIZE - 1;

					while(1){
						if(ucLowLevelBuf[index][0] == find0){
							PushLowLevelTransfer(&(ucLowLevelBuf[index][0]));
							break;
						}
						index--;
						if(index < 0) index = LOW_LEVEL_BUF_SIZE - 1;
					}
					break;
			}
		}
	}

	if(clear_to_send1){
		if(PopLowLevel(data)){
			USB_Send_Data_LowLevel_num(data, 63);
			if(FileTransfer_lastBlock){
				if(ulLowLevelHead == ulLowLevelTail){
					current.pccomm = FileTransfer_pccomm;
				}
			}
		}
	}
}


void memorycopy(uchar *destination, uchar *source, ulong size){
	ulong index;
	for(index=0; index<size; index++){
		*destination = *source;
		destination++;
		source++;
	}
}

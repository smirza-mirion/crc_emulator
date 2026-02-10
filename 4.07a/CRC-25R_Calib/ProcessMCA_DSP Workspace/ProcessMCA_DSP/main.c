/*
 * main.c
 *
 *  Created on: Dec 16, 2009
 *      Author: Administrator
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/*int main(void){
	FILE *OutStream, *InStream;
	int i, j, iLength;
	char cBufferTemp[100];
	char cBuffer[100000];
	char cBuffer2[100000];

	OutStream = fopen("MCA_DSP.TXT", "r");
	if(OutStream != NULL){
		cBuffer[0] = 0;

		while(fgets(cBufferTemp, 100, OutStream)!=NULL){
			iLength = strlen(cBufferTemp);
			if(cBufferTemp[iLength-1] == 0x0A) cBufferTemp[iLength-1] = 0;
			strcat(cBuffer, cBufferTemp);
		}

		iLength = strlen(cBuffer);
		i = j = 0;
		while(i < iLength){
			cBuffer2[j] = '0';
			j++;

			cBuffer2[j] = 'x';
			j++;

			cBuffer2[j] = cBuffer[i];
			j++;
			i++;

			cBuffer2[j] = cBuffer[i];
			j++;
			i++;

			cBuffer2[j] = ',';
			j++;

			if(i%40 == 0){
				cBuffer2[j] = 0x0A;
			}else{
				cBuffer2[j] = ' ';
			}
			j++;
		}
		fclose(OutStream);

		InStream = fopen("dsp_code.c", "w");
		fwrite(cBuffer2, 1, strlen(cBuffer2), InStream);
		fclose(InStream);
		printf("Finished");
	}else{
		printf("Unable to find MCA_DSP.TXT");
	}

	return 1;
}*/

int main(void){
	FILE *OutStream;
	int i;
	int readchar0,readchar1,readchar2,readchar3;
	char cBuffer1[20];

	OutStream = fopen("dsp_mca.bin", "r");
	if(OutStream != NULL){
		i = 0;
		do{
			readchar0 = fgetc(OutStream);
			readchar1 = fgetc(OutStream);
			readchar2 = fgetc(OutStream);
			readchar3 = fgetc(OutStream);

			if(readchar0!=EOF && readchar1!=EOF && readchar2!=EOF && readchar3!=EOF){
				sprintf("0x")
			}
		}while(readchar0 != EOF);
		fclose(OutStream);
	}else{
		printf("Unable to find dsp_mca.bin\n");
	}

	return 1;
}

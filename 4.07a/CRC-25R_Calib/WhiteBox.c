/*********************************************************************
  MODULE:       WHITEBOX TESTS

  FILE:         WhiteBox.c

  DATE:

  ANALYSIS:     White Box test routines

    *************************************************************************/

#include "crc.h"
#ifdef WHITEBOX_TESTS

#include "fat.h"
#include "stdlib.h"
#include "mmcapi.h"
#include "qspi.h"
#include <string.h>

extern time_t clock_time;

#define NEG_THRESHOLD -.001
#define POS_THRESHOLD .001

void test_cnum(void)
{
	char cal_str[20];
	float response, response2, diff;
	char cstr[20];
	int i;
	int cval;
	int print;

	printf("DisplayNucCal_getCalNumFromResponse\r\n\n");

	//*100

	for (cval = 10; cval < 1000; cval++)
	{
		
		sprintf(cal_str,"%03d",cval);
		
		strcat(cal_str, "*100");
		
		response = get_response(cal_str,0);
		//cal_from_response(cstr,response);
		DisplayNucCal_getCalNumFromResponse(response, 0,cstr);
		
		if(strcmp(cstr,cal_str)){
			strcat(cstr, " **");
/*			print = 1;
 - 		}else{
 - 			print = 0;*/
		}
		printf("%s %f %s\r\n",cal_str,response,cstr);
		
		print = 0;
		if(print){
			response2 = get_response(cstr, 0);
			diff = response - response2;
			if((diff < NEG_THRESHOLD) || (diff > POS_THRESHOLD)) printf("%s  %f  %s  %f  %f\r\n",cal_str,response,cstr, response2, diff);
		}
	}

	
	//*10


	for (cval = 10; cval < 1201; cval++)
	{
		
		sprintf(cal_str,"%03d",cval);

		strcat(cal_str, "*10");
		
		response = get_response(cal_str,0);
		//cal_from_response(cstr,response);
		DisplayNucCal_getCalNumFromResponse(response, 0,cstr);
		
		if(strcmp(cstr, cal_str)){
			strcat(cstr," **");
/*			print = 1;
 - 		}else{
 - 			print = 0;*/
		}
			
		printf("%s %f %s\r\n",cal_str,response,cstr);
		
		print = 0;
		if(print){
			response2 = get_response(cstr, 0);
			diff = response - response2;
			if((diff < NEG_THRESHOLD) || (diff > POS_THRESHOLD)) printf("%s  %f  %s  %f  %f\r\n",cal_str,response,cstr, response2, diff);
		}
	}

	//no factor


    for (cval = 10; cval < 1201; cval++)
	{
		
		sprintf(cal_str,"%03d",cval);
		response = get_response(cal_str,0);
		//cal_from_response(cstr,response);
		DisplayNucCal_getCalNumFromResponse(response, 0,cstr);
		
		if(strcmp(cstr, cal_str)){
			strcat(cstr," **");
/*			print = 1;
 - 		}else{
 - 			print = 0;*/
		}

		printf("%s %f %s\r\n",cal_str,response,cstr);
		
		print = 0;
		if(print){
			response2 = get_response(cstr, 0);
			diff = response - response2;
			if((diff < NEG_THRESHOLD) || (diff > POS_THRESHOLD))
				printf("%s  %f  %s  %f  %f\r\n",cal_str,response,cstr, response2, diff);
		}
	}
	
	//div by 2
	for (cval = 400; cval < 1201; cval++)
	{	
		
		sprintf(cal_str,"%d",cval);
		
		strcat(cal_str, "\x81");
		strcat(cal_str, "2");
		
		response = get_response(cal_str,0);
		
		//cal_from_response(cstr,response);
		DisplayNucCal_getCalNumFromResponse(response, 0,cstr);
		

		i=strlen(cstr);
		if(i){
			i--;
			do{
				if(cstr[i] == 32) cstr[i] = 0;
				else break;
				i--;
			}while(i);
		}
		
		if(strcmp(cstr, cal_str)){
			strcat(cstr," **");
/*			print = 1;
 - 		}else{
 - 			print = 0;*/
		}
		
		printf("%s %f %s\r\n",cal_str,response,cstr);
		
		print = 0;
		if(print){
			response2 = get_response(cstr, 0);
			diff = response - response2;
			if((diff < NEG_THRESHOLD) || (diff > POS_THRESHOLD)) printf("%s  %f  %s  %f  %f\r\n",cal_str,response,cstr, response2, diff);

		}
				
	}


	
}	
/*
 - void INTEGRATION_TEST_FileWriteTest(void){
 - 	F_FILE *file;
 - 	int i, j, k, filesize, writesize, startsize[16];
 - 	char filename[13];
 - 	unsigned char buffer[512], chksum;
 - 
 - 	//disable adc;
 - 	set_adc_enabled(FALSE);
 - 	init_mmc_card();
 - 
 - 	f_chdir("\\");
 - 	f_chdir("data");
 - 
 - 	startsize[0] = 2 - 1;
 - 	startsize[1] = 64 - 2;
 - 	startsize[2] = 128 - 2;
 - 	startsize[3] = 256 - 2;
 - 	startsize[4] = 512 - 2;
 - 	startsize[5] = 1024 - 2;
 - 	startsize[6] = 2*1024 - 2;
 - 	startsize[7] = 4*1024 - 2;
 - 	startsize[8] = 8*1024 - 2;
 - 	startsize[9] = 16*1024 - 2;
 - 	startsize[10] = 32*1024 - 2;
 - 	startsize[11] = 64*1024 - 2;
 - 	startsize[12] = 128*1024 - 2;
 - 	startsize[13] = 256*1024 - 2;
 - 	startsize[14] = 512*1024 - 2;
 - 	startsize[15] = 1024 *1024 - 2;
 - 
 - 	srand(clock_time);
 - 
 - 	for(i=0; i<16; i++){
 - 		for(j=0; j<3; j++){
 - 			filesize = startsize[i] + j;
 - 			sprintf(filename, "%08d.abc", filesize + 1);
 - 			if(!mmc_file_exists(filename)) f_delete(filename);
 - 			file = f_open(filename, "a");
 - 			chksum = 0;
 - 			while(filesize){
 - 				if(filesize >= 512) writesize = 512;
 - 				else writesize = filesize;
 - 				for(k=0; k<writesize; k++){
 - 					buffer[k] = rand() & 0xFF;
 - 					chksum += buffer[k];
 - 				}
 - 				f_write(buffer, writesize, 1, file);
 - 				filesize -= writesize;
 - 			}
 - 			chksum = ~chksum;
 - 			chksum += 1;
 - 			f_write(&chksum, 1, 1, file);
 - 			f_close(file);
 - 			printf("Created: %s\n", filename);
 - 		}
 - 	}
 - 
 - 	//initialize QSPI for communicating with chamber
 - 	init_qspi();
 - 
 - 	//re-enable adc
 - 	set_adc_enabled(TRUE);
 - }
 - 
 - static void test_file(char *fname){
 - 	int filelength, readlength, i;
 - 	char filename[13];
 - 	unsigned char buffer[256], checksum;
 - 	F_FILE *file;
 - 
 - 	filelength = fn_filelength(fname);
 - 	strcpy(filename, fname);
 - 	filename[8] = 0;
 - 
 - 	if(atoi(filename) == filelength){
 - 		checksum = 0;
 - 		file = fn_open(fname, "r");
 - 
 - 		while(filelength){
 - 			if(filelength >= 256) readlength = 256;
 - 			else readlength = filelength;
 - 			fn_read(buffer, readlength, 1, file);
 - 			for(i=0; i<readlength; i++) checksum += buffer[i];
 - 			filelength -= readlength;
 - 		}
 - 
 - 		f_close(file);
 - 
 - 		if(checksum) printf("File %s Checksum is bad.\n", fname);
 - 		else printf("File %s Checksum is good.\n", fname);
 - 	}else{
 - 		printf("ERROR: File %s is not the correct size.\n", fname);
 - 	}
 - }
 - 
 - void INTEGRATION_TEST_FileReadTest(void){
 - 	F_FIND find;
 - 
 - 	//disable adc;
 - 	set_adc_enabled(FALSE);
 - 	init_mmc_card();
 - 
 - 	f_chdir("\\");
 - 	f_chdir("data");
 - 
 - 	if(!fn_findfirst("*.abc", &find)){
 - 		test_file(find.filename);
 - 		while(!fn_findnext(&find)){
 - 			test_file(find.filename);
 - 		}
 - 	}
 - 
 - 	//initialize QSPI for communicating with chamber
 - 	init_qspi();
 - 
 - 	//re-enable adc
 - 	set_adc_enabled(TRUE);
 - }
 - */
/*
 *
 * C program on the PC to verify the files written by INTEGRATION_TEST_FileWriteTest()
 *
 */
/*
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

int main(void) {
	DIR *dir;
	struct dirent *ep;
	char filename[13];
	int filehandle, filesize, readsize, i;
	unsigned char buffer[1024], checksum;

	dir = opendir(".");

	while((ep = readdir(dir))){
		if(strlen(ep->d_name) == 12){
			strcpy(filename, ep->d_name);
			if(!strcmp(&(filename[8]), ".ABC") || !strcmp(&(filename[8]), ".abc")){
				filehandle = open(ep->d_name, O_RDONLY | O_BINARY);
				filesize = lseek(filehandle, 0, SEEK_END);
				lseek(filehandle, 0, SEEK_SET);

				filename[8] = 0;
				if(atoi(filename) == filesize){
					checksum = 0;
					while(filesize){
						if(filesize >= 1024) readsize = 1024;
						else readsize = filesize;

						read(filehandle, buffer, readsize);
						for(i=0; i<readsize; i++) checksum += buffer[i];
						filesize -= readsize;
					}

					if(checksum) printf("File %s Checksum is bad.\n", ep->d_name);
					else printf("File %s Checksum is good.\n", ep->d_name);
				}else{
					printf("ERROR: File %s is not the correct size.\n", ep->d_name);
				}

				close(filehandle);
			}
		}
	}

	closedir(dir);

	return EXIT_SUCCESS;
}*/

#endif

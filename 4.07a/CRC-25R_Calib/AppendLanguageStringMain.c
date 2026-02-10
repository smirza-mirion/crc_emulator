/*
 * AppendLanguageStringMain.c
 *
 *  Created on: Apr 27, 2012
 *      Author: Winston Lam
 */

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct{
	int used;
	char enumString[200];
	char englishString[200];
} MESSAGE;


char *GetSingleLine(char *beginning, char *singleLine){
	char *returnValue;

	returnValue = beginning;
	while((*returnValue != 10) && (*returnValue != 0)){
		*singleLine = *returnValue;
		singleLine++;
		returnValue++;
	}
	*singleLine = 0;
	if(*(singleLine - 1) == 13) *(singleLine - 1) = 0;
	returnValue++;

	return returnValue;
}

void StripLeadingWhiteSpaces(char *singleLine){
	int index, length;

	while((*singleLine == 0x09) || (*singleLine == 32)){
		length = strlen(singleLine);
		for(index=0; index<length; index++){
			*(singleLine + index) = *(singleLine + index + 1);
		}
	}
}

int main(int argc, char *argv[]){
	int returnValue, index, hasChanged;
	int displayMessageHandle, displayMessageLength;
	char bufferChar, displayMessagePath[300];
	int fileHandle, fileLength;
	char filePath[300];
	char *buffer1, *buffer2, *beginning, *ptr, *ptrEnd, *ptr2, *ptr3;
	char workingFolder[300], singleLine[1000], filename[100];
	int messageSize;
	MESSAGE message[2000];
	DIR *dp;
	struct dirent *ep;

	returnValue = 0;

	for(index=0; index<2000; index++) message[index].used = 0;

	strcpy(workingFolder, argv[0]);
	index = strlen(workingFolder);
	while(workingFolder[index]!='\\') index--;
	workingFolder[index] = 0;

	strcpy(displayMessagePath, workingFolder);
	strcat(displayMessagePath, "\\DisplayMessage.c");

	displayMessageHandle = open(displayMessagePath, O_RDONLY);
	lseek(displayMessageHandle, 0, SEEK_SET);
	displayMessageLength = lseek(displayMessageHandle, 0, SEEK_END);
	lseek(displayMessageHandle, 0, SEEK_SET);
	buffer1 = malloc(displayMessageLength + 1);
	displayMessageLength = read(displayMessageHandle, buffer1, displayMessageLength);
	close(displayMessageHandle);
	buffer1[displayMessageLength] = 0;

	beginning = strstr(buffer1, "/*static const AMULET_MESSAGE amulet_messages[] =");

	ptr = beginning;

	while(*ptr++ != '}');
	*ptr = 0;
	ptrEnd = ptr;

	ptr = beginning;

	ptr = GetSingleLine(ptr, singleLine);
	while(strstr(singleLine, "{") == NULL) ptr = GetSingleLine(ptr, singleLine);

	messageSize = 0;
	while(ptr < ptrEnd){
		if(ptr < ptrEnd){
			do{
				ptr = GetSingleLine(ptr, singleLine);
				StripLeadingWhiteSpaces(singleLine);
			}while(((*singleLine != 'L') || (*(singleLine + 1) != '_')) && (ptr < ptrEnd));

			if(ptr < ptrEnd){
				ptr2 = singleLine + strlen(singleLine);
				while(*ptr2 != ',') ptr2--;
				*ptr2 = 0;
				//printf("%s\n", singleLine);
				strcpy(message[messageSize].enumString, singleLine);
			}
		}

		if(ptr < ptrEnd){
			do{
				ptr = GetSingleLine(ptr, singleLine);
				StripLeadingWhiteSpaces(singleLine);
			}while((*singleLine != '"') && (ptr < ptrEnd));

			if(ptr < ptrEnd){
				ptr2 = singleLine + strlen(singleLine);
				while(*ptr2 != '"') ptr2--;
				ptr2++;
				*ptr2 = 0;
				//printf("%s\n", singleLine);
				strcpy(message[messageSize].englishString, singleLine);
			}
		}

		if(ptr < ptrEnd){
			do{
				ptr = GetSingleLine(ptr, singleLine);
				StripLeadingWhiteSpaces(singleLine);
			}while((*singleLine != '"') && (ptr < ptrEnd));

			if(ptr < ptrEnd){
				messageSize++;
			}
		}
	}
	free(buffer1);

	dp = opendir(workingFolder);
	while((ep = readdir(dp))){
		strcpy(filename, ep->d_name);
		ptr2 = filename + strlen(filename);

		if(((*(ptr2 - 1) == 'c') || (*(ptr2 - 1) == 'C')) && (*(ptr2 - 2) == '.')){
			if(strcmp(filename, "DisplayMessage.c") != 0){
				strcpy(filePath, workingFolder);
				strcat(filePath, "\\");
				strcat(filePath, filename);
				fileHandle = open(filePath, O_RDONLY);
				lseek(fileHandle, 0, SEEK_SET);
				fileLength = lseek(fileHandle, 0, SEEK_END);
				lseek(fileHandle, 0, SEEK_SET);
				buffer1 = malloc(2 * fileLength);
				buffer2 = malloc(2 * fileLength);
				fileLength = read(fileHandle, buffer1, fileLength);
				close(fileHandle);
				buffer1[fileLength] = 0;

				hasChanged = 0;
				for(index=0; index<messageSize; index++){
					ptr = buffer1;
					while((ptr = strstr(ptr, message[index].enumString)) != NULL){
						if((*(ptr - 1) == '(') || (*(ptr - 1) == ' ') || (*(ptr - 1) == ',') || (*(ptr - 1) == 0x09)){
							if((*(ptr + strlen(message[index].enumString)) == ',') || (*(ptr + strlen(message[index].enumString)) == ' ') || (*(ptr + strlen(message[index].enumString)) == 0x09) || (*(ptr + strlen(message[index].enumString)) == ')')){

								bufferChar = *ptr;
								*ptr = 0;
								strcpy(buffer2, buffer1);
								*ptr = bufferChar;
								ptr3 = NULL;
								ptr2 = ptr;
								while(*ptr2 != 10){
									if(*ptr2 == ';'){
										ptr3 = ptr2;
									}
									ptr2++;
								}
								if(ptr3 != NULL){
									*ptr3 = 0;
									strcat(buffer2, ptr);
									strcat(buffer2, ";");
									strcat(buffer2, "    // ");
									strcat(buffer2, message[index].englishString);
									strcat(buffer2, ptr2);

									strcpy(buffer1, buffer2);
									message[index].used = 1;
									hasChanged = 1;
								}
							}
						}
						ptr = ptr + strlen(message[index].enumString);
					}
				}

				if(hasChanged){
					printf("%s\n", filePath);
					fileHandle = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
					write(fileHandle, buffer1, strlen(buffer1));
					close(fileHandle);
				}

				free(buffer1);
				free(buffer2);
			}
		}
	}
	for(index=0; index<messageSize; index++){
		if(!message[index].used){
			printf("%s - Not Used\n", message[index].enumString);
		}
	}

	closedir(dp);

	return returnValue;
}



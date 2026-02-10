#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

static char *nextLine(char *ptr){
	char *returnValue;

	returnValue = ptr;
	while(*returnValue != 0x0A) returnValue++;
	returnValue++;
	return returnValue;
}

int main(int argc, char *argv[]){
	int index, length, headerHandle, sourceHandle, outputHandle, returnError, headerLength, sourceLength, bin[10], englishAllocation, frenchAllocation, stringCounter;
	char workingFolder[200], headerPath[300], sourcePath[300], outputPath[300], onechar[3], mapString[20], enumString[200], singleLine[500];
	char *headerBuffer, *sourceBuffer, *enumBuffer, *ptr, *ptrPrevious, *ptrSearch, *outputBuffer, *mapBuffer, ok, *checkBuffer;
	long charCount;

	for(index=0; index<10; index++) bin[index] = 26 * index;

	stringCounter = 0;
	returnError = 0;
	outputBuffer = malloc(100000000);
	checkBuffer = malloc(100000000);
	*outputBuffer = 0;
	*checkBuffer = 0;
	strcat(outputBuffer, "#include \"crc.h\"\n\nconst char lang_str[]={\n");
	mapBuffer = malloc(10000000);
	*mapBuffer = 0;

	onechar[1] = 0;
	strcpy(workingFolder, argv[0]);
	length = strlen(workingFolder);
	index = length;
	while(workingFolder[index] != '\\') index--;
	workingFolder[index] = 0;
	//printf("%s\n", workingFolder);

	strcpy(headerPath, workingFolder);
	strcat(headerPath, "\\Headers\\message.h");

	headerHandle = open(headerPath, O_RDONLY);
	if(headerHandle < 0){
		printf("Unable to open: %s\n", headerPath);
		returnError = 1;
	}else{
		lseek(headerHandle, 0, SEEK_SET);
		headerLength = lseek(headerHandle, 0, SEEK_END);
		headerBuffer = malloc(headerLength + 1);
		lseek(headerHandle, 0, SEEK_SET);
		headerLength = read(headerHandle, headerBuffer, headerLength);
		close(headerHandle);
		*(headerBuffer + headerLength) = 0;
		//enumBuffer = strstr(headerBuffer, "enum amulet_strings_id");
		//printf("%s", enumBuffer);

		strcpy(sourcePath, workingFolder);
		strcat(sourcePath, "\\DisplayMessage.c");
		sourceHandle = open(sourcePath, O_RDONLY);
		if(sourceHandle < 0){
			printf("Unable to open: %s\n", sourcePath);
			returnError = 1;
		}else{
			lseek(sourceHandle, 0, SEEK_SET);
			sourceLength = lseek(sourceHandle, 0, SEEK_END);
			sourceBuffer = malloc(sourceLength + 1);
			lseek(sourceHandle, 0, SEEK_SET);
			sourceLength = read(sourceHandle, sourceBuffer, sourceLength);
			close(sourceHandle);
			*(sourceBuffer + sourceLength) = 0;

			enumBuffer = strstr(headerBuffer, "enum amulet_strings_id");
			ptr = strchr(enumBuffer, '}');
			*ptr = 0;
			enumBuffer = strchr(enumBuffer, '{');
			enumBuffer = nextLine(enumBuffer);
			index = 0;
			do{
				while((*enumBuffer == 0x09) || (*enumBuffer == 32)) enumBuffer++;
				ptr = strchr(enumBuffer, ',');
				*ptr = 0;
				strcpy(enumString, enumBuffer);
				strcat(enumString, ",");

				if(!strcmp(enumBuffer, "NUM_AMULET_STRINGS")) break;

				ok = 0;
				ptrSearch = sourceBuffer;
				while(!ok){
					ptr = strstr(ptrSearch, enumString);
					if(ptr == NULL){
						printf("Unable to Find: %s\n", enumString);
						strcpy(outputPath, workingFolder);
						strcat(outputPath, "\\LanguageStrings.c");
						outputHandle = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
						write(outputHandle, outputBuffer, strlen(outputBuffer));
						close(outputHandle);
						return 1;
					}

					ptrPrevious = ptr - 1;
					if(ptr == sourceBuffer) ok = 1;
					else if(((*ptrPrevious) == 32) || ((*ptrPrevious) == 0x09)){
						ok = 1;
						ptrPrevious = ptr;
						while(*ptrPrevious != 0x0A){
							if(*ptrPrevious == '\"'){
								ok = 0;
								break;
							}
							ptrPrevious++;
						}
						if(!ok) ptrSearch = nextLine(ptr);

					}else{
						ptrSearch = nextLine(ptr);
					}
				}

				ptr = nextLine(ptr);
				while((*ptr == 0x09) || (*ptr == 32)) ptr++;
				while(*ptr != '"') ptr++;
				ptr++;
				sprintf(mapString, "%d,", index);
				strcat(mapBuffer, mapString);
				charCount = 0;
				while(*ptr != '"'){
					strcat(outputBuffer, "'");
					onechar[0] = *ptr++;
					if(onechar[0] == '\\'){
						onechar[1] = *ptr++;
						onechar[2] = 0;
					}else if(onechar[0] == '\''){
						onechar[0] = '\\';
						onechar[1] = '\'';
						onechar[2] = 0;
					}else{
						onechar[1] = 0;
					}
					strcat(outputBuffer, onechar);
					strcat(outputBuffer, "',");
					charCount++;
					if(onechar[0] < 0) charCount++;
					index++;
				}
				//strcat(outputBuffer, "0,");
				//index++;

				englishAllocation = 9;
				while((bin[englishAllocation] > charCount) && (englishAllocation != 0)) englishAllocation--;

				strcat(outputBuffer, "		// ");
				strcat(outputBuffer, enumBuffer);
				sprintf(singleLine," (english)  %ld(%d), %d\n", charCount, englishAllocation, stringCounter++);
				strcat(outputBuffer, singleLine);

				ptr = nextLine(ptr);
				while((*ptr == 0x09) || (*ptr == 32)) ptr++;
				while(*ptr != '"') ptr++;
				ptr++;
				sprintf(mapString, "%d,\n", index);
				strcat(mapBuffer, mapString);
				charCount = 0;
				while(*ptr != '"'){
					strcat(outputBuffer, "'");
					onechar[0] = *ptr++;
					if(onechar[0] == '\\'){
						onechar[1] = *ptr++;
						onechar[2] = 0;
					}else if(onechar[0] == '\''){
						onechar[0] = '\\';
						onechar[1] = '\'';
						onechar[2] = 0;
					}else{
						onechar[1] = 0;
					}
					strcat(outputBuffer, onechar);
					strcat(outputBuffer, "',");
					charCount++;
					if(onechar[0] < 0) charCount++;
					index++;
				}
				//strcat(outputBuffer, "0,");
				//index++;

				frenchAllocation = 9;
				while((bin[frenchAllocation] > charCount) && (frenchAllocation != 0)) frenchAllocation--;

				strcat(outputBuffer, "		// ");
				strcat(outputBuffer, enumBuffer);
				sprintf(singleLine," (french)  %ld(%d)\n", charCount, frenchAllocation);
				strcat(outputBuffer, singleLine);

				/*if(frenchAllocation > englishAllocation){
					englishAllocation = 26 * (englishAllocation + 1) - 1;
					frenchAllocation = 26 * (frenchAllocation + 1) - 1;
					strcat(checkBuffer, "//");
					strcat(checkBuffer, enumBuffer);
					sprintf(singleLine, " %d, %d\n", englishAllocation, frenchAllocation);
					strcat(checkBuffer, singleLine);
				}*/

				/*if(frenchAllocation == englishAllocation){
					if(frenchAllocation > 0){
						englishAllocation = 26 * (englishAllocation + 1) - 1;
						frenchAllocation = 26 * (frenchAllocation + 1) - 1;
						strcat(checkBuffer, "//");
						strcat(checkBuffer, enumBuffer);
						sprintf(singleLine, " %d, %d\n", englishAllocation, frenchAllocation);
						strcat(checkBuffer, singleLine);
					}
				}*/

				enumBuffer = nextLine(enumBuffer);

			}while(strchr(enumBuffer, 0x0A) != NULL);
			strcat(outputBuffer, "0\n};\n\n");

			strcat(outputBuffer, "const LANG_MAP lang_map[] = {\n");
			mapBuffer[strlen(mapBuffer) - 2] = 0;
			strcat(outputBuffer, mapBuffer);
			strcat(outputBuffer, "\n};\n");

			//strcat(outputBuffer, checkBuffer);
			strcpy(outputPath, workingFolder);
			strcat(outputPath, "\\LanguageStrings.c");
			outputHandle = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
			write(outputHandle, outputBuffer, strlen(outputBuffer));
			close(outputHandle);

			free(sourceBuffer);
		}

		free(headerBuffer);
	}

	free(checkBuffer);
	free(outputBuffer);
	return returnError;
}

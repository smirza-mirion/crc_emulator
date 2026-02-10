#define PHASE_ERROR_MSG_LARGE_WAIT		0x1234

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"

char AmuletErrorMsgLargeMenu_title[52];
char AmuletErrorMsgLargeMenu_message[4096];
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);

extern int m_iPhase;

void AmuletErrorMsgLarge_menu(void){
	int index, currentline, currentchar, last_index, last_currentline, last_currentchar, sentcount;
	char line[14][51];

	switch(m_iPhase){
		case PHASE_ERROR_MSG_LARGE_WAIT:
			break;

		default:
			for(index=0; index<14; index++) line[index][0] = 0;
			currentline = 0;
			last_currentline = -1;

			currentchar = 0;
			last_currentchar = 0;

			index = 0;
			last_index = 0;

			while((AmuletErrorMsgLargeMenu_message[index] != 0) && (currentline < 14)){
				if(currentchar < 50){
					if(AmuletErrorMsgLargeMenu_message[index] == '\n'){
						line[currentline][currentchar] = 0;
						currentline++;
						currentchar = 0;
						index++;
					}else{
						line[currentline][currentchar] = AmuletErrorMsgLargeMenu_message[index];
						if(AmuletErrorMsgLargeMenu_message[index] == ' '){
							last_index = index;
							last_currentline = currentline;
							last_currentchar = currentchar;
						}
						currentchar++;
						index++;
					}
				}else{
					if(last_currentline == currentline){
						// Truncate at space on current line
						line[last_currentline][last_currentchar] = 0;
						index = last_index + 1;
						currentline++;
						currentchar = 0;
					}else{
						// There is no space on current line
						line[currentline][currentchar] = 0;
						currentline++;
						currentchar = 0;
					}
				}
			}

			if((currentline < 14) && (currentchar < 51)) line[currentline][currentchar] = 0;

			send_to_amulet_string(100, AmuletErrorMsgLargeMenu_title);
			send_amulet_message(L_CAPS_OK, 102);    // "OK"
			delayloop(20);
			sentcount = 0;
			for(index=0; index<14; index++){
				if(strlen(&(line[index][0]))> 0){
					send_to_amulet_string(50 + 2*index, &(line[index][0]));
					sentcount++;
				}

				if(sentcount == 4){
					delayloop(20);
					sentcount = 0;
				}
			}
			SetAmuletByte(100, 0xFF);

			m_iPhase = PHASE_ERROR_MSG_LARGE_WAIT;
			break;
	}
}

//#define PHASE_ERROR_MSG_PRE_INIT	0
#define PHASE_ERROR_MSG_WAIT		0x1234

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"

char AmuletErrorMsgMenu_title[52];
char AmuletErrorMsgMenu_message[104];
//short AmuletErrorMsgMenu_x;
//short AmuletErrorMsgMenu_y;
char AmuletErrorMsgMenu_displayed;
bool AmuletErrorMsgMenu_showOK;
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);

extern int m_iPhase;

void AmuletErrorMsg_menu(void){
	switch(m_iPhase){
		/*case PHASE_ERROR_MSG_PRE_INIT:
			SetAmuletString(100, AmuletErrorMsgMenu_title);
			SetAmuletString(102, AmuletErrorMsgMenu_message);
			if(AmuletErrorMsgMenu_showOK) SetAmuletByte(100, 0xFF);
			else SetAmuletByte(101, 0xFF);
			AmuletErrorMsgMenu_displayed = -1;
			m_iPhase = PHASE_ERROR_MSG_WAIT;
			break; */

		case PHASE_ERROR_MSG_WAIT:
			break;

		default:
			send_to_amulet_string(100, AmuletErrorMsgMenu_title);
			send_to_amulet_string(102, AmuletErrorMsgMenu_message);
			send_amulet_message(L_CAPS_OK, 106);    // "OK"
			if(AmuletErrorMsgMenu_showOK) SetAmuletByte(100, 0xFF);
			else SetAmuletByte(101, 0xFF);
			AmuletErrorMsgMenu_displayed = -1;
			m_iPhase = PHASE_ERROR_MSG_WAIT;
			break;
	}
}

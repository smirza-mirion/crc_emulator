//#define PHASE_NOTIFICATION_MSG_PRE_INIT	0
#define PHASE_NOTIFICATION_MSG_WAIT		0x1235

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"

char AmuletNotificationMsgMenu_title[52];
char AmuletNotificationMsgMenu_message[104];
char AmuletNotificationMsgMenu_displayed;
bool AmuletNotificationMsgMenu_showOK;
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);

extern int m_iPhase;

void AmuletNotificationMsg_menu(void){
	switch(m_iPhase){
		/*case PHASE_NOTIFICATION_MSG_PRE_INIT:
			send_to_amulet_string(100, AmuletNotificationMsgMenu_title);
			send_to_amulet_string(102, AmuletNotificationMsgMenu_message);
			send_amulet_message(L_CAPS_OK, 106);    // "OK"
			if(AmuletNotificationMsgMenu_showOK) SetAmuletByte(100,0xFF);
			else SetAmuletByte(101, 0xFF);
			AmuletNotificationMsgMenu_displayed = -1;
			m_iPhase = PHASE_NOTIFICATION_MSG_WAIT;
			break;*/

		case PHASE_NOTIFICATION_MSG_WAIT:
			break;

		default:
			send_to_amulet_string(100, AmuletNotificationMsgMenu_title);
			send_to_amulet_string(102, AmuletNotificationMsgMenu_message);
			send_amulet_message(L_CAPS_OK, 106);    // "OK"
			if(AmuletNotificationMsgMenu_showOK) SetAmuletByte(100,0xFF);
			else SetAmuletByte(101, 0xFF);
			AmuletNotificationMsgMenu_displayed = -1;
			m_iPhase = PHASE_NOTIFICATION_MSG_WAIT;
			break;
	}
}

#define PHASE_SETUPPHOTKEYS_PRE_INIT	0
#define PHASE_SETUPPHOTKEYS_WAIT		1
#define PHASE_SETUPPHOTKEYS_ACCEPT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "nuc.h"
#include "i2c.h"
#include "message.h"
#include "chambfac.h"

unsigned char AmuletSetupPHotkeys_M[8];
unsigned char AmuletSetupPHotkeys_N[20];

extern int m_iPhase;
extern unsigned char m_ucClear;
extern unsigned char m_ucHotKeyNuclideID[6][8];
extern unsigned char m_ucHotKeyNuclideID2[6][20];

void trim_and_shrink(char *acByte);
void SetAmuletBackHTML(void);
//void write_HotKeys(void);
void send_accept_cancel(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void DB_WriteAllHotkey(int ch_type);

void AmuletSetupPHotkeys_menu(void){
	short index;
	char message[100];
	//USER_KEYS user_keys;

	switch(m_iPhase){
		case PHASE_SETUPPHOTKEYS_PRE_INIT:
			if(m_ucClear == 48){
				for(index=0; index<8; index++) AmuletSetupPHotkeys_M[index] = m_ucHotKeyNuclideID[1][index];
				for(index=0; index<20; index++) AmuletSetupPHotkeys_N[index] = m_ucHotKeyNuclideID2[1][index];
				m_ucClear = 0;
			}

			for(index=0; index<8; index++){
				if(AmuletSetupPHotkeys_M[index]!=255){
					NuclideData_getName(AmuletSetupPHotkeys_M[index], message);
					trim_and_shrink(message);
					send_to_amulet_string(51+index, message);
				}
			}
			delayloop(5);
			for(index=0; index<10; index++){
				if(AmuletSetupPHotkeys_N[index]!=255){
					NuclideData_getName(AmuletSetupPHotkeys_N[index], message);
					trim_and_shrink(message);
					send_to_amulet_string(61+index, message);
				}
			}
			delayloop(5);
			for(index=10; index<20; index++){
				if(AmuletSetupPHotkeys_N[index] != 255){
					NuclideData_getName(AmuletSetupPHotkeys_N[index], message);
					trim_and_shrink(message);
					send_to_amulet_string(61+index, message);
				}
			}

			send_amulet_message(L_SETUP_PET_HOTKEYS,100);    // "Setup PET Chamber Hotkeys"
			send_amulet_message(L_HOME_SCREEN,102);    // "Home\n Screen"
			send_amulet_message(L_NUCLIDE_SCREEN,103);    // "Nuclide\n Screen"
			send_amulet_message(L_PLEASE_SELECT_NUCLIDE,104);    // "Please Select Nuclide"

			send_accept_cancel();
			
			SetAmuletByte(100, 0xFF);

			m_iPhase = PHASE_SETUPPHOTKEYS_WAIT;
			break;

		case PHASE_SETUPPHOTKEYS_WAIT:
			break;

		case PHASE_SETUPPHOTKEYS_ACCEPT:
			beep_amulet();
			for(index=0;index<8;index++) m_ucHotKeyNuclideID[1][index] = AmuletSetupPHotkeys_M[index];
			for(index=0;index<20;index++) m_ucHotKeyNuclideID2[1][index] = AmuletSetupPHotkeys_N[index];
			DB_WriteAllHotkey(P_CHAMB); //write_HotKeys();
			/*if(current.comm_hotkey == 1){
				for(index=0; index<10; index++){
					if(m_ucHotKeyNuclideID2[1][index+4]==255) user_keys.keys[index] = -1;
					else user_keys.keys[index] = m_ucHotKeyNuclideID2[1][index+4];
				}
				EE_WRITE(user_keys[0], (uchar *) &user_keys);
			}*/
			SetAmuletBackHTML();
			break;
	}
}

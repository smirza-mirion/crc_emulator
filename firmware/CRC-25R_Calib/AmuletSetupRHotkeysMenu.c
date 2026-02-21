/**
 * \file
 * \details This file handles calls from the Amulet Hotkey Setup Screen
 */
#define PHASE_SETUPRHOTKEYS_PRE_INIT	0
#define PHASE_SETUPRHOTKEYS_WAIT		1
#define PHASE_SETUPRHOTKEYS_ACCEPT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "nuc.h"
#include "i2c.h"
#include "message.h"
#include "chambfac.h"

unsigned char AmuletSetupRHotkeys_M[8];
unsigned char AmuletSetupRHotkeys_N[20];

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

/**
 * \details Handles the Amulet Screen SetupRHotkeys.htm. SetupRHotkeys.htm setups the nuclide hotkeys.
 * \param Amulet_String_ID Description
 * \param 90 Accept
 * \param 91 Cancel
 * \param 51 Main Screen Nuclide 1
 * \param 52 Main Screen Nuclide 2
 * \param 53 Main Screen Nuclide 3
 * \param 54 Main Screen Nuclide 4
 * \param 55 Main Screen Nuclide 5
 * \param 56 Main Screen Nuclide 6
 * \param 57 Main Screen Nuclide 7
 * \param 58 Main Screen Nuclide 8
 * \param 61 Nuclide Screen Nuclide 1
 * \param 62 Nuclide Screen Nuclide 2
 * \param 63 Nuclide Screen Nuclide 3
 * \param 64 Nuclide Screen Nuclide 4
 * \param 65 Nuclide Screen Nuclide 5
 * \param 66 Nuclide Screen Nuclide 6
 * \param 67 Nuclide Screen Nuclide 7
 * \param 68 Nuclide Screen Nuclide 8
 * \param 69 Nuclide Screen Nuclide 9
 * \param 70 Nuclide Screen Nuclide 10
 * \param 71 Nuclide Screen Nuclide 11
 * \param 72 Nuclide Screen Nuclide 12
 * \param 73 Nuclide Screen Nuclide 13
 * \param 74 Nuclide Screen Nuclide 14
 * \param 75 Nuclide Screen Nuclide 15
 * \param 76 Nuclide Screen Nuclide 16
 * \param 77 Nuclide Screen Nuclide 17
 * \param 78 Nuclide Screen Nuclide 18
 * \param 79 Nuclide Screen Nuclide 19
 * \param 80 Nuclide Screen Nuclide 20
 * \param 100_101 Title
 * \param 102 Home Screen
 * \param 103 Nuclide Screen
 * \param 104_105 Please Select Nuclide
 * \returns None
 */
void AmuletSetupRHotkeys_menu(void){
	short index;
	char message[100];
	//USER_KEYS user_keys;

	switch(m_iPhase){
		case PHASE_SETUPRHOTKEYS_PRE_INIT:
			if(m_ucClear == 47){
				if(chamber_C()){
					for(index=0; index<8; index++) AmuletSetupRHotkeys_M[index] = m_ucHotKeyNuclideID[C_CHAMB][index];
					for(index=0; index<20; index++) AmuletSetupRHotkeys_N[index] = m_ucHotKeyNuclideID2[C_CHAMB][index];
				}else if(chamber_K()){
					for(index=0; index<8; index++) AmuletSetupRHotkeys_M[index] = m_ucHotKeyNuclideID[K_CHAMB][index];
					for(index=0; index<20; index++) AmuletSetupRHotkeys_N[index] = m_ucHotKeyNuclideID2[K_CHAMB][index];
				}else{
					for(index=0; index<8; index++) AmuletSetupRHotkeys_M[index] = m_ucHotKeyNuclideID[R_CHAMB][index];
					for(index=0; index<20; index++) AmuletSetupRHotkeys_N[index] = m_ucHotKeyNuclideID2[R_CHAMB][index];
				}
				m_ucClear = 0;
			}

			for(index=0; index<8; index++){
				if(AmuletSetupRHotkeys_M[index]!=255){
					NuclideData_getName(AmuletSetupRHotkeys_M[index], message);
					trim_and_shrink(message);
					send_to_amulet_string(51+index, message);
				}
			}
			delayloop(5);
			for(index=0; index<10; index++){
				if(AmuletSetupRHotkeys_N[index]!=255){
					NuclideData_getName(AmuletSetupRHotkeys_N[index], message);
					trim_and_shrink(message);
					send_to_amulet_string(61+index, message);
				}
			}
			delayloop(5);
			for(index=10; index<20; index++){
				if(AmuletSetupRHotkeys_N[index] != 255){
					NuclideData_getName(AmuletSetupRHotkeys_N[index], message);
					trim_and_shrink(message);
					send_to_amulet_string(61+index, message);
				}
			}

			if(chamber_77t()) send_amulet_message(L_SETUP_HOTKEYS,100);    // "Setup Hotkeys"
			else send_amulet_message(L_SETUP_R_HOTKEYS,100);    // "Setup R Chamber Hotkeys"
			send_amulet_message(L_HOME_SCREEN,102);    // "Home\n Screen"
			send_amulet_message(L_NUCLIDE_SCREEN,103);    // "Nuclide\n Screen"
			send_amulet_message(L_PLEASE_SELECT_NUCLIDE,104);    // "Please Select Nuclide"

			send_accept_cancel();
			
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUPRHOTKEYS_WAIT;
			break;

		case PHASE_SETUPRHOTKEYS_WAIT:
			break;

		case PHASE_SETUPRHOTKEYS_ACCEPT:
			beep_amulet();
			if(chamber_C()){
				for(index=0;index<8;index++) m_ucHotKeyNuclideID[C_CHAMB][index] = AmuletSetupRHotkeys_M[index];
				for(index=0;index<20;index++) m_ucHotKeyNuclideID2[C_CHAMB][index] = AmuletSetupRHotkeys_N[index];
				DB_WriteAllHotkey(C_CHAMB); //write_HotKeys();
			}else if(chamber_K()){
				for(index=0;index<8;index++) m_ucHotKeyNuclideID[K_CHAMB][index] = AmuletSetupRHotkeys_M[index];
				for(index=0;index<20;index++) m_ucHotKeyNuclideID2[K_CHAMB][index] = AmuletSetupRHotkeys_N[index];
				DB_WriteAllHotkey(K_CHAMB); //write_HotKeys();
			}else{
				for(index=0;index<8;index++) m_ucHotKeyNuclideID[R_CHAMB][index] = AmuletSetupRHotkeys_M[index];
				for(index=0;index<20;index++) m_ucHotKeyNuclideID2[R_CHAMB][index] = AmuletSetupRHotkeys_N[index];
				DB_WriteAllHotkey(R_CHAMB); //write_HotKeys();
			}

			/*if(current.comm_hotkey == 0){
				for(index=0; index<10; index++){
					if(m_ucHotKeyNuclideID2[0][index+4]==255) user_keys.keys[index] = -1;
					else user_keys.keys[index] = m_ucHotKeyNuclideID2[0][index+4];
				}
				EE_WRITE(user_keys[0], (uchar *) &user_keys);
			}*/

			SetAmuletBackHTML();
			break;
	}
}

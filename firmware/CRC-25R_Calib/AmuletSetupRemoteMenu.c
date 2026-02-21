#define PHASE_SETUP_REMOTE_PRE_INIT		0
#define PHASE_SETUP_REMOTE_WAIT			1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"
#include "amulet.h"
#include "nuc.h"
#include "message.h"

KEYDEF AmuletSetupRemoteMenu_mirrorRemoteNuc[8];

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;

void send_to_amulet_string(uchar ucIndex, char message0[]);

void AmuletSetupRemote_menu(void){
	short index, jndex, kndex;
	char newshuffle[8][8];

	switch(m_iPhase){
		case PHASE_SETUP_REMOTE_PRE_INIT:
			if(m_ucClear == 10){
				EE_READ(remote_nucs, (uchar *) &AmuletSetupRemoteMenu_mirrorRemoteNuc);
				m_ucClear = 0;
			}else{

				for(index=0; index<8; index++) newshuffle[index][0] = 0;
				jndex = 0;
				for(index=0; index<8; index++){
					if(AmuletSetupRemoteMenu_mirrorRemoteNuc[index].keyiso[0].iso_name[0] != 0){
						for(kndex=0; kndex<8; kndex++) newshuffle[jndex][kndex] = AmuletSetupRemoteMenu_mirrorRemoteNuc[index].keyiso[0].iso_name[kndex];
						jndex++;
					}
				}
				for(index=0; index<8; index++){
					for(jndex=0;jndex<8;jndex++) AmuletSetupRemoteMenu_mirrorRemoteNuc[index].keyiso[0].iso_name[jndex] = newshuffle[index][jndex];
				}

				for(index=0; index<8; index++) newshuffle[index][0] = 0;
				jndex = 0;
				for(index=0; index<8; index++){
					if(AmuletSetupRemoteMenu_mirrorRemoteNuc[index].keyiso[1].iso_name[0] != 0){
						for(kndex=0; kndex<8; kndex++) newshuffle[jndex][kndex] = AmuletSetupRemoteMenu_mirrorRemoteNuc[index].keyiso[1].iso_name[kndex];
						jndex++;
					}
				}
				for(index=0; index<8; index++){
					for(jndex=0;jndex<8;jndex++) AmuletSetupRemoteMenu_mirrorRemoteNuc[index].keyiso[1].iso_name[jndex] = newshuffle[index][jndex];
				}

				EE_WRITE(remote_nucs, (uchar *) &AmuletSetupRemoteMenu_mirrorRemoteNuc);
			}
			for(index=0; index<8; index++) send_to_amulet_string(101 + index, AmuletSetupRemoteMenu_mirrorRemoteNuc[index].keyiso[0].iso_name);
			for(index=0; index<8; index++) send_to_amulet_string(109 + index, AmuletSetupRemoteMenu_mirrorRemoteNuc[index].keyiso[1].iso_name);

			SetAmuletByte(92,current.language);
			
			send_amulet_message(L_SETUP_REMOTE_NUCLIDES,120);    // "Setup Remote Nuclides"
			send_amulet_message(L_R_CHAMBER,122);    // "R Chamber"
			send_amulet_message(L_P_CHAMBER,123);    // "P Chamber"
			send_amulet_message(L_PLEASE_SELECT_NUCLIDE,124);    // "Please Select Nuclide"

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_REMOTE_WAIT;
			break;

		case PHASE_SETUP_REMOTE_WAIT:
			break;
	}
}

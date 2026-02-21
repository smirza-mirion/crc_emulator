#define PHASE_DELETECALICHECK_PRE_INIT	0
#define PHASE_DELETECALICHECK_WAIT		1
#define PHASE_DELETECALICHECK_DELETE		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "linearity.h"
#include "i2c.h"
#include "message.h"

extern int m_iPhase;
extern CURRENT current;

void SetAmuletBackHTML(void);

void AmuletDeleteCalicheck_menu(void){
	short ch_num = current.main_chamber;
	short index;
	LINDEF linearityempty;
	//char message[50];
	
	switch(m_iPhase){
		case PHASE_DELETECALICHECK_PRE_INIT:

			send_amulet_message(L_DELETE_CALICHECK_SETTINGS,100);    // "Delete Calicheck Settings"
			send_amulet_message(L_BTN_YES,102);    // "YES"
			send_amulet_message(L_BTN_NO,103);    // "NO"
			send_amulet_message(L_DELETE_CALICHECK_SETTINGS_Q,104);    // "Delete Calicheck Settings?"
			
			
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_DELETECALICHECK_WAIT;
			break;

		case PHASE_DELETECALICHECK_WAIT:
			break;

		case PHASE_DELETECALICHECK_DELETE:
			beep_amulet();
			linearityempty.num_Cali[ch_num] = -1;
			EE_WRITE(lindef.num_Cali[ch_num], (uchar *) &linearityempty.num_Cali[ch_num]);
			for(index=0; index<7; index++){
				linearityempty.chamb_num_Cali[ch_num][index] = 0;
				EE_WRITE(lindef.chamb_num_Cali[ch_num][index], (uchar *) &linearityempty.chamb_num_Cali[ch_num][index]);
			}

			for(index=0; index<11; index++){
				linearityempty.serial_num_Cali[ch_num][index] = 0;
				EE_WRITE(lindef.serial_num_Cali[ch_num][index], (uchar *) &linearityempty.serial_num_Cali[ch_num][index]);
			}

			for(index=0; index<12; index++){
				linearityempty.factors_Cali[ch_num][index] = -999;
				EE_WRITE(lindef.factors_Cali[ch_num][index], (uchar *) &linearityempty.factors_Cali[ch_num][index]);
			}

			SetAmuletBackHTML();
			break;
	}
}

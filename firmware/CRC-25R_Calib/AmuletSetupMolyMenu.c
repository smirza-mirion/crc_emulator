#define PHASE_SETUP_MOLY_PRE_INIT 	0
#define PHASE_SETUP_MOLY_WAIT		1
#define PHASE_SETUP_MOLY_MALLINCK	2
#define PHASE_SETUP_MOLY_BRISTOL	3
#define PHASE_SETUP_MOLY_CAPINTEC	4
#define PHASE_SETUP_MOLY_STREAMLINED	5

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"
#include "message.h"
#include "mca.h"
#include "database.h"

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);

extern int m_iPhase;
extern unsigned char m_ucClear;
extern unsigned char m_ucCheckboxState;
extern CURRENT current;

static unsigned char m_aucMo[4];
float m_fSetupMoly_MoTcLimit;

void send_to_amulet_string(uchar ucIndex, char message0[]);

void AmuletSetupMoly_menu(void){
	char acMsg[100];

	switch(m_iPhase){
		case PHASE_SETUP_MOLY_PRE_INIT:
			if(m_ucClear==7){
				EE_READ(mo_choice, m_aucMo);
				EE_READ(mo_tc_lim, (unsigned char *) &m_fSetupMoly_MoTcLimit);
				m_ucClear = 0;
			}else{
				EE_WRITE(mo_tc_lim, (unsigned char *) &m_fSetupMoly_MoTcLimit);
			}

			if(m_aucMo[0]==FALSE){
				SetAmuletByte(101, 1);
			}else{
				SetAmuletByte(101, 2);
			}

			if(m_aucMo[1]==FALSE){
				SetAmuletByte(102, 3);
			}else{
				SetAmuletByte(102, 4);
			}

			if(m_aucMo[2]==FALSE){
				SetAmuletByte(103, 5);
			}else{
				SetAmuletByte(103, 6);
			}

			if(current.moly_streamlined){
				SetAmuletByte(104, 8);
			}else{
				SetAmuletByte(104, 7);
			}

			SetAmuletByte(92,current.language);			

			send_amulet_message(L_SETUP_MOLY,101);    // "Setup Moly"

			send_amulet_message(L_SETUP_MOLY_METHOD,102);    // "Moly Method:"

			send_amulet_message(L_SETUP_MOLY_LIMIT,103);    // "Mo/Tc Limit:"

			send_amulet_message(L_ENTER_MOTC_LIMIT,104);    // "Enter Mo/Tc Limit:"

			sprintf(acMsg, "%.3f", m_fSetupMoly_MoTcLimit);
			send_to_amulet_string(100, acMsg);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_MOLY_WAIT;
			break;

		case PHASE_SETUP_MOLY_WAIT:

			break;

		case PHASE_SETUP_MOLY_MALLINCK:
			if(m_ucCheckboxState==1) m_aucMo[0] = FALSE;
			else if(m_ucCheckboxState == 2) m_aucMo[0] = TRUE;
			EE_WRITE(mo_choice, m_aucMo);
			m_iPhase = PHASE_SETUP_MOLY_WAIT;
			break;

		case PHASE_SETUP_MOLY_BRISTOL:
			if(m_ucCheckboxState==3) m_aucMo[1] = FALSE;
			else if(m_ucCheckboxState==4) m_aucMo[1] = TRUE;
			EE_WRITE(mo_choice, m_aucMo);
			m_iPhase = PHASE_SETUP_MOLY_WAIT;
			break;

		case PHASE_SETUP_MOLY_CAPINTEC:
			if(m_ucCheckboxState==5) m_aucMo[2] = FALSE;
			else if(m_ucCheckboxState==6) m_aucMo[2] = TRUE;
			EE_WRITE(mo_choice, m_aucMo);
			m_iPhase = PHASE_SETUP_MOLY_WAIT;
			break;

		case PHASE_SETUP_MOLY_STREAMLINED:
			if(m_ucCheckboxState==7) current.moly_streamlined = 0;
			else if(m_ucCheckboxState==8) current.moly_streamlined = 1;
			DB_WriteMolyStreamlined(current.moly_streamlined);
			m_iPhase = PHASE_SETUP_MOLY_WAIT;
			break;
	}
}

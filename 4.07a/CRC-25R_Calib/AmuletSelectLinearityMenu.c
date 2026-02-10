/**
 * \file
 * \details This file handles calls from the Amulet Select Linearity Test Menu
 */
#define PHASE_SELECTLINEARITY_PRE_INIT	0
#define PHASE_SELECTLINEARITY_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "chambfac.h"
#include "linearity.h"
#include "i2c.h"
#include "message.h"

extern int m_iPhase;
extern CURRENT current;
extern CHAMBERVALS chamb_vals[];

void send_to_amulet_string(uchar ucIndex, char message0[]);

/**
 * \details Handles the Amulet Screen SelectLinearity.htm. SelectLinearity.htm displays the linearity test available on the calibrator.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Current Chamber
 * \param 21 Show Chamber Button
 * \param 80 Language (State)
 * \param 91 Show Standard (State)
 * \param 92 Show Lineator (State)
 * \param 93 Show Calicheck (State)
 * \param 94 Show No Linearity Setup (State)
 * \param 100 Show Page (Toggle)
 * \param Amulet_String_ID Description
 * \param 100_101 Title
 * \param 102_103 Please Enter Password:
 * \param 104_105_106_107 Standard, Lineator and Calicheck are available in setup
 * \param 108_109 AutoLinearity
 * \param 110_111 Standard
 * \param 112_113 Lineator
 * \param 114_115 Calicheck
 * \returns None
 */
void AmuletSelectLinearity_menu(void){
	char index, message[100];
	short ch_num = current.main_chamber;
	short ch_type, ch_type_mod;
	LINDEF mirror;
	char cur_chamb_num[7];
	bool found;

	switch(m_iPhase){
		case PHASE_SELECTLINEARITY_PRE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_LINEARITY, 100);    // "Linearity"
			send_amulet_message(L_ENTER_PASSWORD, 102);    // "Please Enter Password:"
			send_amulet_message(L_STD_LINEATOR_CALICHECK_AVAILABLE_IN_SETUP, 104);    // "Standard, Lineator and Calicheck are available in setup"
			send_amulet_message(L_AUTOLINEARITY, 108);    // "AutoLinearity"
			send_amulet_message(L_STANDARD, 110);    // "Standard"
			send_amulet_message(L_LINEATOR, 112);    // "Lineator"
			send_amulet_message(L_CALICHECK, 114);    // "Calicheck"

			ch_type = chamber_type(ch_num);
			for(index=0;index<6;index++) cur_chamb_num[index] = chamb_vals[ch_num].sn[index];
			cur_chamb_num[6] = 0;
			if(current.num_chambers>1) SetAmuletByte(21, 0xFF);

			if(current.num_chambers>0){
				sprintf(message, "Ch: %d", current.main_chamber + 1);
				switch(chamber_type(current.main_chamber)){
					case R_CHAMB:
						strcat(message, ", R");
						break;
					case P_CHAMB:
						strcat(message, ", PET");
						break;
					case B_CHAMB:
						strcat(message, ", BT");
						break;
					case ONE_DOT_EIGHT_CHAMB:
						strcat(message, ", 1.8 Atm");
						break;
					case C_CHAMB:
						strcat(message, ", HR");
						break;
					case K_CHAMB:
						strcat(message, ", 1K");
						break;
				}
				send_to_amulet_string(9, message);
				SetAmuletByte(20, 0xFF);
			}

			found = FALSE;
			EE_READ(lindef, (uchar *) &mirror);
			ch_type_mod = chamber_type_77t_remap(ch_type); // C_CHAMB and K_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded
			if(mirror.num_Std[ch_type_mod] != -1){
				SetAmuletByte(91, 0xFF);
				found = TRUE;
			}
			if(mirror.num_Lin[ch_num] != -1){
				if(strcmp(cur_chamb_num, &(mirror.chamb_num_Lin[ch_num][0])) == 0){
					SetAmuletByte(92, 0xFF);
					found = TRUE;
				}
			}
			if(mirror.num_Cali[ch_num] != -1){
				if(strcmp(cur_chamb_num, &(mirror.chamb_num_Cali[ch_num][0])) == 0){
					SetAmuletByte(93, 0xFF);
					found = TRUE;
				}
			}

			if(!found) SetAmuletByte(94, 0xFF);
			SetAmuletByte(100, 0xFF);

			m_iPhase = PHASE_SELECTLINEARITY_WAIT;
			break;

		case PHASE_SELECTLINEARITY_WAIT:
			break;
	}
}

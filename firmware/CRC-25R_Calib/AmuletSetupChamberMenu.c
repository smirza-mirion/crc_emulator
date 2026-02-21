/**
 * \file
 * \details This file handles calls from the Amulet Chamber Setup Screen
 */
#define PHASE_SETUPCHAMBER_PRE_INIT	0
#define PHASE_SETUPCHAMBER_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern char AmuletErrorMsgMenu_displayed;
extern char cap_str[];
extern CHAMBERVALS chamb_vals[9];
extern CURRENT current;

CHAMBERVALS AmuletSetupChamber_chvals;
char AmuletSetupChamber_serial[7];
float AmuletSetupChamber_respCorr;
float AmuletSetupChamber_nomVolts;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void read_from_chamber(short ch_num, CHAMBERVALS *ch_vals);
void write_to_chamber(short ch_num, CHAMBERVALS *ch_vals);

/**
 * \details Handles the Amulet Screen SetupChamber.htm. SetupChamber.htm setups the chamber settings.
 * \param Amulet_Byte_ID Description
 * \param 20 Display Chamber type (TOGGLE)
 * \param 100 Show display (TOGGLE)
 * \param Amulet_String_ID Description
 * \param 9 Chamber type
 * \param 101 Chamber serial number
 * \param 102 Response correction
 * \param 103 Nominal bias voltage
 * \returns None
 */
void AmuletSetupChamber_menu(void){
	char message[25];

	switch(m_iPhase){
		case PHASE_SETUPCHAMBER_PRE_INIT:
			if(m_ucClear==37){
				read_from_chamber(current.main_chamber, &AmuletSetupChamber_chvals);

				if(!strncmp(AmuletSetupChamber_chvals.init_str, cap_str, 8) == 0){
					AmuletSetupChamber_chvals.chamb_type = R_CHAMB;
					AmuletSetupChamber_chvals.resp_corr = 0.0;
					strncpy(AmuletSetupChamber_chvals.sn, "000000", 6);
					AmuletSetupChamber_chvals.nomvolts = 155;
					strncpy(AmuletSetupChamber_chvals.init_str, "Capintec", 8);
					strncpy(AmuletSetupChamber_chvals.rev_num_str, "      ", 6);
					AmuletSetupChamber_chvals.feature_flag1 = 0;
					AmuletSetupChamber_chvals.feature_flag2 = 0;
					AmuletSetupChamber_chvals.gainfactor0 = 0;
					AmuletSetupChamber_chvals.gainfactor1 = 0;
					AmuletSetupChamber_chvals.gainfactor2 = 0;
					AmuletSetupChamber_chvals.future = 0;
					write_to_chamber(current.main_chamber, &AmuletSetupChamber_chvals);
					read_from_chamber(current.main_chamber, &(chamb_vals[current.main_chamber]));
					Amulet_DisplayNotification("Chamber Initialization", "Blank Chamb EEPROM detected.\nFlashed as 3 Stage R", TRUE);
				}

				strncpy(AmuletSetupChamber_serial, AmuletSetupChamber_chvals.sn, 6);
				AmuletSetupChamber_serial[6] = 0;
				AmuletSetupChamber_respCorr = AmuletSetupChamber_chvals.resp_corr;
				AmuletSetupChamber_nomVolts = AmuletSetupChamber_chvals.nomvolts;
				AmuletErrorMsgMenu_displayed = 0;
				m_ucClear = 0;
			}

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
			SetAmuletString(9, message);
			SetAmuletByte(20, 0xFF);

			if(strlen(AmuletSetupChamber_serial)<6){
				if(AmuletErrorMsgMenu_displayed == 0){
					Amulet_DisplayError("Serial Number Error", "Serial Number should be 6 digits",TRUE);
				}

				strncpy(AmuletSetupChamber_serial, AmuletSetupChamber_chvals.sn, 6);
				AmuletSetupChamber_serial[6] = 0;
			}

			strcpy(message, AmuletSetupChamber_serial);
			SetAmuletString(101, message);

			sprintf(message, "%f", 100.0 * AmuletSetupChamber_respCorr);
			SetAmuletString(102, message);

			sprintf(message, "%.1f", AmuletSetupChamber_nomVolts);
			SetAmuletString(103, message);

			if((strncmp(AmuletSetupChamber_serial, AmuletSetupChamber_chvals.sn, 6)!=0) || (AmuletSetupChamber_respCorr!=AmuletSetupChamber_chvals.resp_corr) || (AmuletSetupChamber_nomVolts != AmuletSetupChamber_chvals.nomvolts)){
				strncpy(AmuletSetupChamber_chvals.sn, AmuletSetupChamber_serial, 6);
				AmuletSetupChamber_chvals.resp_corr = AmuletSetupChamber_respCorr;
				AmuletSetupChamber_chvals.nomvolts = AmuletSetupChamber_nomVolts;
				write_to_chamber(current.main_chamber, &AmuletSetupChamber_chvals);
				read_from_chamber(current.main_chamber, &(chamb_vals[current.main_chamber]));
			}

			SetAmuletByte(100, 0xFF);
			AmuletErrorMsgMenu_displayed = 0;
			m_iPhase = PHASE_SETUPCHAMBER_WAIT;
			break;

		case PHASE_SETUPCHAMBER_WAIT:
			break;
	}
}

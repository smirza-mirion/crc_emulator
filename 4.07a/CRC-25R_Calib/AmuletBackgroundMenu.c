/**
 * \file
 * \details This file handles call from the Amulet Background Test Screen
 */
#define PHASE_BKG_BEFORE_INIT 0
#define PHASE_BKG_AFTER_INIT 1
#define PHASE_BKG_BARGRAPH 2
#define PHASE_BKG_BARGRAPH_AFTER 3
#define PHASE_BKG_SAVE_BACK 4
#define PHASE_BKG_FINISHED 5
#define PHASE_BKG_WAIT 6

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "chambfac.h"
#include "daily.h"
#include "mca.h"
#include "database.h"
#include "screen.h"

extern CURRENT  current;
extern int m_iPhase;
extern BKGDATA bkgdata[];
extern unsigned char m_ucBargraphValue;
extern CHAMBERVALS chamb_vals[];
extern time_t clock_time;
extern CHAMBER chamber[];
extern MEASUREMENT measurement[];
extern ushort chamber_checksum[];

void send_to_amulet_string(uchar ucIndex, char message0[]);
void trim(char *acByte);
ushort calc_chamber_checksum(short chamb);
void Sysset_updateMirror(void);

/**
 * \details Handles the Amulet Screen Background.htm. Background.htm runs the background test.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 80 Language (STATE) English = 0, Spanish = 1
 * \param 108 Show Red Color
 * \param 189 State Change (TOGGLE)
 * \param 189 FF = Display Remove All Sources message
 * \param 189 FE = Display Background Test
 * \param Amulet_String_ID Description
 * \param 9 Current Chamber
 * \param 150 Background Value
 * \param 190 Background Result
 * \param 200_201 Title
 * \param 202_203 1) Remove All Sources from Chamber
 * \param 204 OK
 * \param 205_206 2) Measure Backgnd
 * \param 207 Finished
 * \param 208 Continue
 * \returns None
 */
void AmuletBackground_menu(void) {
	short ch_num = current.main_chamber;
	char acMsg[100], *acMsg2;
	static char String190[100];
	CHAMBERBACKGROUND *chamberbackground;

	switch(m_iPhase) {
		case PHASE_BKG_BEFORE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_BACKGROUND2, 200);    // "Background"
			send_amulet_message(L_REMOVE_ALL_SOURCES_FROM_CHAMBER, 202);    // "1) Remove All Sources from Chamber"
			send_amulet_message(L_CAPS_OK, 204);    // "OK"
			send_amulet_message(L_MEASURE_BACKGROUND2, 205);    // "2) Measure Backgnd"
			send_amulet_message(L_FINISHED, 207);    // "Finished"
			send_amulet_message(L_CONTINUE_BTN, 208);    // "Continue"

			if(current.num_chambers>1) SetAmuletByte(21, 0xFF);
			if(current.num_chambers>0){
				sprintf(acMsg, "Ch: %d", current.main_chamber + 1);
				switch(chamber_type(current.main_chamber)){
					case R_CHAMB:
						strcat(acMsg, ", R");
						break;
					case P_CHAMB:
						strcat(acMsg, ", PET");
						break;
					case B_CHAMB:
						strcat(acMsg, ", BT");
						break;
					case ONE_DOT_EIGHT_CHAMB:
						strcat(acMsg, ", 1.8 Atm");
						break;
					case C_CHAMB:
						strcat(acMsg, ", HR");
						break;
					case K_CHAMB:
						strcat(acMsg, ", 1K");
						break;
				}
				send_to_amulet_string(9, acMsg);
				SetAmuletByte(20, 0xFF);
			}
			m_iPhase = PHASE_BKG_WAIT;
			// Wait for user press continue
			break;

		case PHASE_BKG_WAIT:
			break;

		case PHASE_BKG_AFTER_INIT:
			if(current.num_chambers>0) setup_for_background(ch_num);
			m_iPhase = PHASE_BKG_BARGRAPH;
			break;

		case PHASE_BKG_BARGRAPH:
			if(current.num_chambers>0){
				if (bkgdata[ch_num].count < 51) {
					m_ucBargraphValue = bkgdata[ch_num].count;
				}else {
					m_ucBargraphValue = 51;
					SetAmuletByte(189, 0xFF);
					m_iPhase = PHASE_BKG_BARGRAPH_AFTER;
				}
			}
			break;

		case PHASE_BKG_BARGRAPH_AFTER:
			// Wait for OK button
			break;

		case PHASE_BKG_SAVE_BACK:
			m_ucBargraphValue = 0;

			if(current.num_chambers>0){
				chamberbackground = (CHAMBERBACKGROUND *) malloc(sizeof(CHAMBERBACKGROUND));
				chamberbackground->ChamberBackgroundID = 0;
				chamberbackground->ChamberSerialNumber[6] = 0;
				strncpy(chamberbackground->ChamberSerialNumber, chamb_vals[ch_num].sn, 6);
				chamberbackground->ChamberType = chamb_vals[ch_num].chamb_type;
				chamberbackground->TwoStageChamber = chamber_one_gain_relay(ch_num);
				chamberbackground->BackgroundStatus = bkgdata[ch_num].status;
				chamberbackground->BackgroundTextEnglish[0] = 0;
				chamberbackground->BackgroundTextSpanish[0] = 0;
				chamberbackground->BackgroundValue = 0;
				chamberbackground->MeasuredOn = clock_time;
				chamberbackground->InactiveReason[0] = 0;
				chamberbackground->Inactive = FALSE;

				if (bkgdata[ch_num].status == BKG_TOO_HIGH) {
					//SetAmuletString(190, "BACKGROUND TOO HIGH");
					get_amulet_message_with_language(L_BACKGROUND_TOO_HIGH, String190, ENGLISH);    // "BACKGROUND TOO HIGH"
					strcpy(chamberbackground->BackgroundTextEnglish, String190);
					get_amulet_message_with_language(L_BACKGROUND_TOO_HIGH, String190, SPANISH);    // "BACKGROUND TOO HIGH"
					strcpy(chamberbackground->BackgroundTextSpanish, String190);

					get_amulet_message(L_BACKGROUND_TOO_HIGH, String190);    // "BACKGROUND TOO HIGH"
					send_to_amulet_string(190, String190);
					SetAmuletByte(108, 0xFF);
					delayloop(5);
				}else{
					chamber[ch_num].bkg += measurement[ch_num].meas;
					chamberbackground->BackgroundValue = chamber[ch_num].bkg;
					strcpy(acMsg, measurement[ch_num].actstr);
					trim(acMsg);
					strcpy(String190, acMsg);
					strcpy(chamberbackground->BackgroundTextEnglish, String190);
					replace(chamberbackground->BackgroundTextEnglish, '$', 'u');
					strcpy(chamberbackground->BackgroundTextSpanish, chamberbackground->BackgroundTextEnglish);
					if(bkgdata[ch_num].status == BKG_HIGH){
						acMsg2 = malloc(100);

						strcat(chamberbackground->BackgroundTextEnglish, " ");
						get_amulet_message_with_language(L_HIGH, acMsg2, ENGLISH);    // "HIGH"
						strcat(chamberbackground->BackgroundTextEnglish, acMsg2);

						strcat(chamberbackground->BackgroundTextSpanish, " ");
						get_amulet_message_with_language(L_HIGH, acMsg2, SPANISH);    // "HIGH"
						strcat(chamberbackground->BackgroundTextSpanish, acMsg2);

						free(acMsg2);
					}
					send_to_amulet_string(190, String190);
					//SetAmuletString(190, acMsg);
					chamber_checksum[ch_num] = calc_chamber_checksum(ch_num);
					Sysset_updateMirror();
				}
				DB_CreateChamberBackground(chamberbackground, 0, TRUE);
				free(chamberbackground);
				SetAmuletByte(189, 0xFE);
				erase_screen();
				m_iPhase = PHASE_BKG_FINISHED;
			}
			break;

		case PHASE_BKG_FINISHED:
			break;

		default:
			break;
	}
}

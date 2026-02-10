/**
 * \file
 *  \details This file handles calls from the Amulet Bias Voltage Test Screen
 */
#define PHASE_CHAMBERVOLTS_BEFORE_INIT 0
#define PHASE_CHAMBERVOLTS_AFTER_INIT 1
#define PHASE_CHAMBERVOLTS_FINISHED 2
#define PHASE_CHAMBERVOLTS_WAIT_AFTER_VOLTAGE 3
#define PHASE_CHAMBERVOLTS_BEFORE_INIT_DELAY 4
#define PHASE_CHAMBERVOLTS_WAIT_2MIN_PRE 5
#define PHASE_CHAMBERVOLTS_WAIT_2MIN	 6
#define PHASE_CHAMBERVOLTS_WAIT_2MIN_POST 7

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
#include "pit.h"

extern CURRENT  current;
extern int m_iPhase;
extern unsigned char m_ucBargraphValue;
extern MEASUREMENT measurement[];
extern BIASDATA biasdata[];
extern CHAMBERVALS chamb_vals[];
extern time_t clock_time;

void send_to_amulet_string(uchar ucIndex, char message0[]);
void trim(char *acByte);

/**
 * \details Handles the Amulet Screen ChamberVolts.htm ChamberVolts.htm runs the bias voltage test.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 80 Language (STATE) English = 0, French = 1
 * \param 100 Hide Buttons
 * \param 101 Show Buttons
 * \param 109 Show Red Color
 * \param 189 State Change (TOGGLE)
 * \param 189 FF = Display Bias Voltage Test
 * \param 189 FE = Display Bias Voltage Result
 * \param Amulet_String_ID Description
 * \param 9 Current Chamber
 * \param 80_81 Title
 * \param 82_83 1) Check Chamber Voltage
 * \param 100_101_102 Stabilization Message
 * \param 189 Volts Result
 * \returns None
 */
void AmuletChamberVolts_menu(void) {
	short ch_num = current.main_chamber;
	short ch_type;
	char acMsg[100], acMsg2[100];
	static char passfail[100];
	static unsigned long int wait;
	CHAMBERVOLTAGE *chambervoltage;

	switch (m_iPhase) {
		case PHASE_CHAMBERVOLTS_BEFORE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_CHAMBER_VOLTS, 80);    // "Chamber Volts"
			send_amulet_message(L_CHECK_CHAMBER_VOLTAGE2, 82);    // "1) Check Chamber Voltage"

			if(current.num_chambers > 1) SetAmuletByte(21, 0xFF);
			erase_screen();
			m_ucBargraphValue = 0;
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

				setup_for_bias(ch_num);
				wait = g_csec_tstamp + 25;
				SetAmuletByte(189, 0xFF);
				m_iPhase = PHASE_CHAMBERVOLTS_AFTER_INIT;
			}
			break;

		case PHASE_CHAMBERVOLTS_BEFORE_INIT_DELAY:
			if(g_csec_tstamp >= wait){
				setchamber(ch_num, measurement[ch_num].gain);
				m_iPhase = PHASE_CHAMBERVOLTS_AFTER_INIT;
			}
			break;

		case PHASE_CHAMBERVOLTS_AFTER_INIT:
			if(current.num_chambers>0){
				if (biasdata[ch_num].status == BIAS_TEST_FAIL || biasdata[ch_num].status == TEST_GOOD) {
					wait = g_csec_tstamp + 500;
					m_iPhase = PHASE_CHAMBERVOLTS_WAIT_AFTER_VOLTAGE;
				}else{
					if(biasdata[ch_num].count <= 16){
						m_ucBargraphValue = biasdata[ch_num].count;
					}
				}
			}
			break;

		case PHASE_CHAMBERVOLTS_WAIT_AFTER_VOLTAGE:
			if(g_csec_tstamp >= wait){
				chambervoltage = (CHAMBERVOLTAGE *) malloc(sizeof(CHAMBERVOLTAGE));
				chambervoltage->ChamberVoltageID = 0;
				chambervoltage->ChamberSerialNumber[6] = 0;
				strncpy(chambervoltage->ChamberSerialNumber, chamb_vals[ch_num].sn, 6);
				chambervoltage->ChamberType = chamb_vals[ch_num].chamb_type;
				chambervoltage->TwoStageChamber = chamber_one_gain_relay(ch_num);
				chambervoltage->NominalVoltage = chamber_nominal_volts(ch_num);
				chambervoltage->MinVoltage = biasdata[ch_num].minvolts;
				chambervoltage->MaxVoltage = biasdata[ch_num].maxvolts;
				chambervoltage->VoltageStatus = biasdata[ch_num].status;
				chambervoltage->VoltageValue = biasdata[ch_num].measvolts;
				chambervoltage->MeasuredOn = clock_time;
				chambervoltage->InactiveReason[0] = 0;
				chambervoltage->Inactive = FALSE;

				strcpy(acMsg, &(measurement[ch_num].actstr[0]));
				strcpy(chambervoltage->VoltageTextEnglish, acMsg);
				trim(chambervoltage->VoltageTextEnglish);
				strcpy(chambervoltage->VoltageTextFrench, chambervoltage->VoltageTextEnglish);

				if (biasdata[ch_num].status == TEST_GOOD){
					get_amulet_message(L_CAPS_OK, acMsg2);    // "OK"
					strcat(acMsg, acMsg2);
				}else if (biasdata[ch_num].status == BIAS_TEST_FAIL){
					get_amulet_message_with_language(L_ERROR, acMsg2, ENGLISH);    // "ERROR"
					strcat(chambervoltage->VoltageTextEnglish, " ");
					strcat(chambervoltage->VoltageTextEnglish, acMsg2);

					get_amulet_message_with_language(L_ERROR, acMsg2, FRENCH);    // "ERROR"
					strcat(chambervoltage->VoltageTextFrench, " ");
					strcat(chambervoltage->VoltageTextFrench, acMsg2);

					get_amulet_message(L_FAIL2, acMsg2);    // "FAIL"
					strcat(acMsg, acMsg2);
					SetAmuletByte(109, 0xFF);
					delayloop(5);
				}
				DB_CreateChamberVoltage(chambervoltage, 0, TRUE);
				free(chambervoltage);

				measurement[ch_num].dogain = 1;
				erase_screen();
				measurement[ch_num].mode = MEASMODE;
				setchamber(ch_num, measurement[ch_num].gain);
				if(measurement[ch_num].gain != 0){
					delay_msec(1000);
					setchamber(ch_num, 0);
				}
				strcpy(passfail, acMsg);

				ch_type = chamber_type(ch_num);
				if(!chamber_hv_adc(ch_num)){
					if(ch_type == P_CHAMB){
						m_iPhase = PHASE_CHAMBERVOLTS_WAIT_2MIN_PRE;
					}else{
						m_iPhase = PHASE_CHAMBERVOLTS_FINISHED;
					}
				}else{
					m_iPhase = PHASE_CHAMBERVOLTS_FINISHED;
				}

				if(m_iPhase == PHASE_CHAMBERVOLTS_FINISHED){
					send_to_amulet_string(189, passfail);
					SetAmuletByte(189, 0xFE);
				}
			}
			break;

		case PHASE_CHAMBERVOLTS_FINISHED:
			break;

		case PHASE_CHAMBERVOLTS_WAIT_2MIN_PRE:
			send_to_amulet_string(189, passfail);
			SetAmuletByte(189, 0xFE);
			//SetAmuletString(100, "PLEASE WAIT 2 Minutes for Stabilization");
			send_amulet_message(L_PLEASE_WAIT_2_MINUTES_FOR_STABILIZATION, 100);    // "PLEASE WAIT 2 Minutes for Stabilization"
			SetAmuletByte(100, 0xFF);
			wait = g_csec_tstamp + 12000;
			m_iPhase = PHASE_CHAMBERVOLTS_WAIT_2MIN;
			break;

		case PHASE_CHAMBERVOLTS_WAIT_2MIN:
			if(g_csec_tstamp > wait){
				m_iPhase = PHASE_CHAMBERVOLTS_WAIT_2MIN_POST;
			}
			break;

		case PHASE_CHAMBERVOLTS_WAIT_2MIN_POST:
			send_to_amulet_string(189, passfail);
			SetAmuletByte(189, 0xFE);
			send_to_amulet_string(100, "");
			SetAmuletByte(101, 0xFF);
			if(current.num_chambers>1){
				SetAmuletByte(21, 0xFF);
			}
			beep_amulet();
			m_iPhase = PHASE_CHAMBERVOLTS_FINISHED;
			break;

		default:
			break;
	}
}

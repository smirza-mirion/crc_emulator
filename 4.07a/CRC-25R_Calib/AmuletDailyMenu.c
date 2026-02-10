/**
 * \file
 * \details This file handles calls from the Amulet Daily Test Screen
 */
#define PHASE_DAILY_BEFORE_INIT 0
#define PHASE_DAILY_AFTER_INIT 1
#define PHASE_DAILY_BEFORE_ZERO 2
#define PHASE_DAILY_DURING_ZERO 3
#define PHASE_DAILY_AFTER_ZERO 4
#define PHASE_DAILY_SAVE_ZERO 5
#define PHASE_DAILY_BEFORE_BACK 6
#define PHASE_DAILY_DURING_BACK 7
#define PHASE_DAILY_AFTER_BACK 8
#define PHASE_DAILY_SAVE_BACK 9
#define PHASE_DAILY_BEFORE_VOLTAGE 0x0A
#define PHASE_DAILY_DURING_VOLTAGE 0x0B
#define PHASE_DAILY_AFTER_VOLTAGE 0x0C
#define PHASE_DAILY_BEFORE_MEM 0x0D
#define PHASE_DAILY_AFTER_MEM 0x0E
#define PHASE_DAILY_BEFORE_ACCURACY 0x0F
#define PHASE_DAILY_ACTIVATE_MEASUREMENT_BUTTONS 0x10
#define PHASE_DAILY_DURING_ACCURACY 0x11
#define PHASE_DAILY_MEASURE_SOURCE1 0x12
#define PHASE_DAILY_MEASURE_SOURCE2 0x13
#define PHASE_DAILY_MEASURE_SOURCE3 0x14
#define PHASE_DAILY_MEASURE_SOURCE4 0x15
#define PHASE_DAILY_MEASURE_SOURCE5 0x16
#define PHASE_DAILY_SET_NUCLIDE 0x17
#define PHASE_DAILY_MEASURE_NUCLIDE 0x18
#define PHASE_DAILY_ACCEPT_PRESSED 0x19
#define PHASE_DAILY_FINISHED 100
#define PHASE_DAILY_PRINT 0x1A
#define PHASE_DAILY_WAIT_AFTER_VOLTAGE 0x1B
#define PHASE_DAILY_BEFORE_VOLTAGE_DELAY 0x1C
#define PHASE_DAILY_WAIT_2MIN_PRE		0x1D
#define PHASE_DAILY_WAIT_2MIN			0x1E
#define PHASE_DAILY_WAIT_2MIN_POST		0x1F


#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "screen.h"
#include "keyboard.h"
#include "daily.h"
#include "pit.h"
#include "i2c.h"
#include "nuc.h"
#include "chambfac.h"
#include "printer.h"
#include "message.h"

extern int m_iPhase;
extern uchar m_ucClear;
extern unsigned char m_ucBargraphValue;
extern CURRENT  current;
extern CHAMBERVALS chamb_vals[];
extern CHAMBER chamber[];
extern BKGDATA bkgdata[];
extern MEASUREMENT measurement[];
extern ZERODATA zerodata[];
extern BIASDATA biasdata[];
extern ushort chamber_checksum[];
extern short num_daily;
extern time_t clock_time;
extern bool AmuletGenericYesNo_AccuracyTestPending;

bool AmuletDailyMenu_okdata;
long long int AmuletDailyMenu_ChamberDailyTestID;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void trim(char *acByte);
void start_daily_res(short ch_num);
void setup_for_zero_adc(short ch_num);
ushort calc_chamber_checksum(short chamb);
short get_num_sources(bool daily);
void Sysset_updateMirror(void);
short prdailysec(bool okdata,short acc_ret);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void DB_CreateChamberZero(CHAMBERZERO *chamberzero, long long int *ChamberDailyTestID, bool bookEnd);
void DB_CreateChamberBackground(CHAMBERBACKGROUND *chamberbackground, long long int ChamberDailyTestID, bool bookEnd);
void DB_CreateChamberVoltage(CHAMBERVOLTAGE *chambervoltage, long long int ChamberDailyTestID, bool bookEnd);
void DB_UpdateDailyTestDataCheck(long long int ChamberDailyTestID, char *DataCheckTextEnglish, char *DataCheckTextSpanish, bool DataCheckFailed, char *DataCheckCRC, bool bookEnd);
void AmuletGenericYesNo_saveInDatabase(bool autoconstancy);

/**
 * \details Handles the Amulet Screen Daily.htm. Daily.htm runs the calibrator test.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label	(TOGGLE)
 * \param 91 Show Accuracy Button	(TOGGLE)
 * \param 92 Language		(STATE) English = 0, Spanish = 1
 * \param 189 State Change		(TOGGLE)
 * \param 189 FF = Display Remove All Sources Message
 * \param 189 FE = Display Zero Test
 * \param 189 FD = Display Background Test
 * \param 189 FC = Display Bias Voltage Test
 * \param 189 FB = Display Data Check
 * \param 189 FA = Display Accuracy Test Button
 * \param Amulet_String_ID Description
 * \param 9 Current Chamber
 * \param 80 Title
 * \param 81_82 Please Enter Password:
 * \param 83_84 1) Remove All Sources from Chamber
 * \param 85 OK
 * \param 86_87 2) Auto Zero
 * \param 88_89 3) Measure Backgnd
 * \param 90_91 4) Check Chamber Voltage
 * \param 92_93 5) Data Check
 * \param 94 Continue
 * \param 95 Accuracy
 * \param 100 Background Result
 * \param 120 Extra Line
 * \param 150 Value Zero
 * \param 174_175_176 Source Data
 * \param 189 Source Data Result 1
 * \param 190 Zero Result
 * \param 191 Chamber Volts Result
 * \param 192 Data Check Result
 * \returns None
 */
void AmuletDaily_menu(void) {
	char acMsg[100], acMsg2[100], *acMsg3;
	char hex_str[10];
	static bool okdata;
	ushort memval;
	short ch_num = current.main_chamber;
	short ch_type;
	static char String190[100];
	static char String191[100];
	static char String192[100];
	static char String100[100];
	static char String174[100];
	static bool printer_pressed;
	short iret;
	static unsigned long int wait;
	CHAMBERZERO *chamberzero;
	CHAMBERBACKGROUND *chamberbackground;
	CHAMBERVOLTAGE *chambervoltage;
	struct tm now;
	time_t new_time;

	switch(m_iPhase) {
		case PHASE_DAILY_BEFORE_INIT:
			AmuletDailyMenu_ChamberDailyTestID = -1;

			if(current.num_chambers>1) SetAmuletByte(21, 0xFF);

			if(current.num_chambers>0){
				SetAmuletByte(92, current.language);

				send_amulet_message(L_DAILY_TEST, 80);    // "Daily Test"
				send_amulet_message(L_ENTER_PASSWORD, 81);    // "Please Enter Password:"
				send_amulet_message(L_REMOVE_ALL_SOURCES_FROM_CHAMBER, 83);    // "1) Remove All Sources from Chamber"
				send_amulet_message(L_CAPS_OK, 85);    // "OK"
				send_amulet_message(L_AUTO_ZERO, 86);    // "2) Auto Zero"
				send_amulet_message(L_MEASURE_BACKGROUND, 88);    // "3) Measure Backgnd"
				send_amulet_message(L_CHECK_CHAMBER_VOLTAGE, 90);    // "4) Check Chamber Voltage"
				send_amulet_message(L_DATA_CHECK, 92);    // "5) Data Check"
				send_amulet_message(L_CONTINUE_BTN, 94);    // "Continue"
				send_amulet_message(L_ACCURACY, 95);    // "Accuracy"
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

				if(m_ucClear==27){
					m_ucClear = 0;
					if(AmuletGenericYesNo_AccuracyTestPending){
						AmuletGenericYesNo_saveInDatabase(TRUE);
						AmuletGenericYesNo_AccuracyTestPending = FALSE;
					}
					start_daily_res(ch_num);
					erase_screen();
					printer_pressed = FALSE;
					m_iPhase = PHASE_DAILY_AFTER_INIT;
				}else{
					if(printer_pressed){
						printer_pressed = FALSE;
						send_to_amulet_string(190, String190);
						send_to_amulet_string(100, String100);
						send_to_amulet_string(191, String191);
						send_to_amulet_string(192, String192);
						send_to_amulet_string(174, String174);
						SetAmuletByte(102, 0xFF);

						//if(current.printer!= NONE_PRINTER) SetAmuletByte(101, 0xFF);
						if((current.printer!= NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
							SetAmuletByte(101, 0xFF);
						num_daily = get_num_sources(TRUE);
						if(num_daily!=0) SetAmuletByte(100, 0xFF);
						m_iPhase = PHASE_DAILY_FINISHED;
					}else{
						if(AmuletGenericYesNo_AccuracyTestPending){
							AmuletGenericYesNo_saveInDatabase(TRUE);
							AmuletGenericYesNo_AccuracyTestPending = FALSE;
						}
						start_daily_res(ch_num);
						erase_screen();
						m_iPhase = PHASE_DAILY_AFTER_INIT;
					}
				}
			}
			break;

		case PHASE_DAILY_AFTER_INIT:
			// Wait for Continue Button
			break;

		case PHASE_DAILY_BEFORE_ZERO:
			if(current.num_chambers>0){
				setup_for_zero_adc(ch_num);
				set_nokey();
				measurement[ch_num].display_flag = FALSE;
				erase_screen();
				m_iPhase = PHASE_DAILY_DURING_ZERO;
			}
			break;

		case PHASE_DAILY_DURING_ZERO:
			if(current.num_chambers>0){
				if(zerodata[ch_num].barvalue < 27){
					m_ucBargraphValue = zerodata[ch_num].barvalue;
				}else{
					m_ucBargraphValue = 27;
					m_iPhase = PHASE_DAILY_AFTER_ZERO;
					SetAmuletByte(189, 0xFF);
				}
			}
			break;

		case PHASE_DAILY_AFTER_ZERO:
			// Wait for OK button
			break;

		case PHASE_DAILY_SAVE_ZERO:
			m_ucBargraphValue = 0;
			if(current.num_chambers>0){
				//new_time = clock_time;
				//new_time += 86400;
				//gmtime_r(&new_time, &now);
				//set_clock(&now);
				//read_clock(&clock_time);

				chamberzero = (CHAMBERZERO *) malloc(sizeof(CHAMBERZERO));
				chamberzero->ChamberZeroID = 0;
				chamberzero->ChamberSerialNumber[6] = 0;
				strncpy(chamberzero->ChamberSerialNumber, chamb_vals[ch_num].sn, 6);
				chamberzero->ChamberType = chamb_vals[ch_num].chamb_type;
				chamberzero->TwoStageChamber = chamber_one_gain_relay(ch_num);
				chamberzero->ZeroStatus = zerodata[ch_num].status;
				chamberzero->ZeroTextEnglish[0] = 0;
				chamberzero->ZeroTextSpanish[0] = 0;
				chamberzero->ZeroValue = zerodata[ch_num].znew;
				chamberzero->MeasuredOn = clock_time;
				chamberzero->CreatedOn = 0;
				chamberzero->InactiveReason[0] = 0;
				chamberzero->Inactive = FALSE;
				if(zerodata[ch_num].status == TEST_GOOD){
					chamber[ch_num].zero = zerodata[ch_num].znew;
					strcpy(acMsg, measurement[ch_num].actstr);
					trim(acMsg);
					strcpy(String190, acMsg);
					strcpy(chamberzero->ZeroTextEnglish, String190);
					strcpy(chamberzero->ZeroTextSpanish, chamberzero->ZeroTextEnglish);
					send_to_amulet_string(190, acMsg);
					chamber_checksum[ch_num] = calc_chamber_checksum(ch_num);
					beep_amulet();
					Sysset_updateMirror();
				}else if (zerodata[ch_num].status == ZERO_DRIFT){
					strcpy(acMsg, measurement[ch_num].actstr);
					trim(acMsg);
					strcat(acMsg, " ");
					get_amulet_message_with_language(L_ZERO_DRIFT, String190, ENGLISH);    // "Zero Drift"
					strcat(acMsg, String190);
					strcpy(chamberzero->ZeroTextEnglish, acMsg);

					strcpy(acMsg, measurement[ch_num].actstr);
					trim(acMsg);
					strcat(acMsg, " ");
					get_amulet_message_with_language(L_ZERO_DRIFT, String190, SPANISH);    // "Zero Drift"
					strcat(acMsg, String190);
					strcpy(chamberzero->ZeroTextSpanish, acMsg);

					get_amulet_message(L_ZERO_DRIFT, String190);    // "Zero Drift"
					send_to_amulet_string(190, String190);
					SetAmuletByte(107, 0xFF);

					chamber[ch_num].zero = zerodata[ch_num].znew;
					chamber_checksum[ch_num] = calc_chamber_checksum(ch_num);
					beep_amulet();
					Sysset_updateMirror();

					delayloop(5);
				}else if (zerodata[ch_num].status == ZERO_OUT_OF_RANGE){
					get_amulet_message_with_language(L_ZERO_OUT_OF_RANGE, String190, ENGLISH);    // "Zero out of Range"
					strcpy(chamberzero->ZeroTextEnglish, String190);

					get_amulet_message_with_language(L_ZERO_OUT_OF_RANGE, String190, SPANISH);    // "Zero out of Range"
					strcpy(chamberzero->ZeroTextSpanish, String190);

					get_amulet_message(L_ZERO_OUT_OF_RANGE, String190);    // "Zero out of Range"
					send_to_amulet_string(190, String190);
					SetAmuletByte(107, 0xFF);
					delayloop(5);
				}
				DB_CreateChamberZero(chamberzero, &AmuletDailyMenu_ChamberDailyTestID, TRUE);
				free(chamberzero);
				SetAmuletByte(189, 0xFE);
				measurement[ch_num].dogain = 1;
				erase_screen();
				measurement[ch_num].dogain = 1;
				setchamber(ch_num, 0);
				measurement[ch_num].mode = MEASMODE;

				m_iPhase = PHASE_DAILY_BEFORE_BACK;
			}
			break;

		case PHASE_DAILY_BEFORE_BACK:
			if(current.num_chambers>0){
				setup_for_background(ch_num);
				m_iPhase = PHASE_DAILY_DURING_BACK;
			}
			break;

		case PHASE_DAILY_DURING_BACK:
			if(current.num_chambers>0){
				if(bkgdata[ch_num].count < 51){
					m_ucBargraphValue = bkgdata[ch_num].count;
				}else{
					m_ucBargraphValue = 51;
					SetAmuletByte(189, 0xFD);
					m_iPhase = PHASE_DAILY_AFTER_BACK;
				}
			}
			break;

		case PHASE_DAILY_AFTER_BACK:
			// Wait for OK button
			break;

		case PHASE_DAILY_SAVE_BACK:
			m_ucBargraphValue = 0;
			if(current.num_chambers>0){
				bkgdata[ch_num].latched_status = bkgdata[ch_num].status;  //latch status for printing
				chamberbackground = (CHAMBERBACKGROUND *) malloc(sizeof(CHAMBERBACKGROUND));
				chamberbackground->ChamberBackgroundID = 0;
				chamberbackground->ChamberSerialNumber[6] = 0;
				strncpy(chamberbackground->ChamberSerialNumber, chamb_vals[ch_num].sn, 6);
				chamberbackground->ChamberType = chamb_vals[ch_num].chamb_type;
				chamberbackground->TwoStageChamber = chamber_one_gain_relay(ch_num);
				chamberbackground->BackgroundStatus = bkgdata[ch_num].latched_status;
				chamberbackground->BackgroundTextEnglish[0] = 0;
				chamberbackground->BackgroundTextSpanish[0] = 0;
				chamberbackground->BackgroundValue = 0;
				chamberbackground->MeasuredOn = clock_time;
				chamberbackground->InactiveReason[0] = 0;
				chamberbackground->Inactive = FALSE;
				if(bkgdata[ch_num].status == BKG_TOO_HIGH){
					get_amulet_message_with_language(L_BACKGROUND_TOO_HIGH, String100, ENGLISH);    // "BACKGROUND TOO HIGH"
					strcpy(chamberbackground->BackgroundTextEnglish, String100);
					get_amulet_message_with_language(L_BACKGROUND_TOO_HIGH, String100, SPANISH);    // "BACKGROUND TOO HIGH"
					strcpy(chamberbackground->BackgroundTextSpanish, String100);

					get_amulet_message(L_BACKGROUND_TOO_HIGH, String100);    // "BACKGROUND TOO HIGH"
					send_to_amulet_string(100, String100);
					SetAmuletByte(108, 0xFF);
					delayloop(5);
				}else{
					chamber[ch_num].bkg += measurement[ch_num].meas;
					chamberbackground->BackgroundValue = chamber[ch_num].bkg;
					strcpy(acMsg, measurement[ch_num].actstr);
					trim(acMsg);
					strcpy(String100, acMsg);
					strcpy(chamberbackground->BackgroundTextEnglish, String100);
					replace(chamberbackground->BackgroundTextEnglish, '$', 'u');
					strcpy(chamberbackground->BackgroundTextSpanish, chamberbackground->BackgroundTextEnglish);
					if(bkgdata[ch_num].status == BKG_HIGH){
						strcat(chamberbackground->BackgroundTextEnglish, " ");
						get_amulet_message_with_language(L_HIGH, acMsg2, ENGLISH);    // "HIGH"
						strcat(chamberbackground->BackgroundTextEnglish, acMsg2);

						strcat(chamberbackground->BackgroundTextSpanish, " ");
						get_amulet_message_with_language(L_HIGH, acMsg2, SPANISH);    // "HIGH"
						strcat(chamberbackground->BackgroundTextSpanish, acMsg2);
					}
					send_to_amulet_string(100, String100);
					chamber_checksum[ch_num] = calc_chamber_checksum(ch_num);
					beep_amulet();
					Sysset_updateMirror();
				}
				DB_CreateChamberBackground(chamberbackground, AmuletDailyMenu_ChamberDailyTestID, TRUE);
				free(chamberbackground);
				SetAmuletByte(189, 0xFC);
				erase_screen();
				m_iPhase = PHASE_DAILY_BEFORE_VOLTAGE;
			}
			break;

		case PHASE_DAILY_BEFORE_VOLTAGE:
			if(current.num_chambers>0){
				setup_for_bias(ch_num);
				wait = g_csec_tstamp + 25;
				m_iPhase = PHASE_DAILY_BEFORE_VOLTAGE_DELAY;
			}
			break;

		case PHASE_DAILY_BEFORE_VOLTAGE_DELAY:
			if(g_csec_tstamp > wait){
				setchamber(ch_num, measurement[ch_num].gain);
				m_iPhase = PHASE_DAILY_DURING_VOLTAGE;
			}
			break;

		case PHASE_DAILY_DURING_VOLTAGE:
			if(current.num_chambers>0){
				if(biasdata[ch_num].status == BIAS_TEST_FAIL || biasdata[ch_num].status == TEST_GOOD){
					wait = g_csec_tstamp + 500;
					m_iPhase = PHASE_DAILY_WAIT_AFTER_VOLTAGE;
				}else{
					if(biasdata[ch_num].count <= 16){
						m_ucBargraphValue = biasdata[ch_num].count;
					}
				}
			}
			break;

		case PHASE_DAILY_WAIT_AFTER_VOLTAGE:
			if(g_csec_tstamp > wait){
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
				strcpy(chambervoltage->VoltageTextSpanish, chambervoltage->VoltageTextEnglish);

				if(biasdata[ch_num].status == TEST_GOOD){
					get_amulet_message(L_CAPS_OK, acMsg2);    // "OK"
					strcat(acMsg, acMsg2);
				}else if(biasdata[ch_num].status == BIAS_TEST_FAIL){
					get_amulet_message_with_language(L_ERROR, acMsg2, ENGLISH);    // "ERROR"
					strcat(chambervoltage->VoltageTextEnglish, " ");
					strcat(chambervoltage->VoltageTextEnglish, acMsg2);

					get_amulet_message_with_language(L_ERROR, acMsg2, SPANISH);    // "ERROR"
					strcat(chambervoltage->VoltageTextSpanish, " ");
					strcat(chambervoltage->VoltageTextSpanish, acMsg2);

					get_amulet_message(L_FAIL2, acMsg2);    // "FAIL"
					strcat(acMsg, acMsg2);
					SetAmuletByte(109, 0xFF);
					delayloop(5);
				}
				DB_CreateChamberVoltage(chambervoltage, AmuletDailyMenu_ChamberDailyTestID, TRUE);
				free(chambervoltage);

				measurement[ch_num].dogain = 1;
				erase_screen();
				measurement[ch_num].mode = MEASMODE;
				setchamber(ch_num, measurement[ch_num].gain);
				if(measurement[ch_num].gain != 0){
					delay_msec(1000);
					setchamber(ch_num, 0);
				}
				strcpy(String191, acMsg);

				ch_type = chamber_type(ch_num);
				if(!chamber_hv_adc(ch_num)){
					if(ch_type==P_CHAMB){
						m_iPhase = PHASE_DAILY_WAIT_2MIN_PRE;
					}else{
						m_iPhase = PHASE_DAILY_AFTER_VOLTAGE;
					}
				}else{
					m_iPhase = PHASE_DAILY_AFTER_VOLTAGE;
				}

				if(m_iPhase == PHASE_DAILY_AFTER_VOLTAGE){
					send_to_amulet_string(191, String191);
					SetAmuletByte(189, 0xFB);
				}
			}
			break;

		case PHASE_DAILY_AFTER_VOLTAGE:
			// Wait for OK from touch screen.
			break;

		case PHASE_DAILY_BEFORE_MEM:
			if(current.num_chambers>0){
				okdata = NuclideData_testNuclideTable(&memval);
				sprintf(hex_str,"%04x",memval);
				AmuletDailyMenu_okdata = okdata;
				if(okdata){
					acMsg3 = malloc(100);
					get_amulet_message_with_language(L_CAPS_OK, String192, ENGLISH);    // "OK"
					get_amulet_message_with_language(L_CAPS_OK, acMsg3, SPANISH);    // "OK"
					DB_UpdateDailyTestDataCheck(AmuletDailyMenu_ChamberDailyTestID, String192, acMsg3, FALSE, hex_str, TRUE);
					free(acMsg3);

					get_amulet_message(L_PASSED, String192);    // "Passed"
					send_to_amulet_string(192, String192);
				}else{
					get_amulet_message_with_language(L_ERROR, String192, ENGLISH);    // "ERROR"
					strcat(String192, ", ");
					strcat(String192, hex_str);
					acMsg3 = malloc(100);
					get_amulet_message_with_language(L_ERROR, acMsg3, SPANISH);    // "ERROR"
					strcat(acMsg3, ", ");
					strcat(acMsg3, hex_str);
					DB_UpdateDailyTestDataCheck(AmuletDailyMenu_ChamberDailyTestID, String192, acMsg3, TRUE, hex_str, TRUE);
					free(acMsg3);

					get_amulet_message(L_FAILED, acMsg);    // "Failed, "
					strcat(acMsg, hex_str);
					strcpy(String192, acMsg);
					send_to_amulet_string(192, String192);
					SetAmuletByte(110, 0xFF);
					delayloop(5);
				}
				SetAmuletByte(189, 0xFA);
				m_iPhase = PHASE_DAILY_AFTER_MEM;
			}
			break;

		case PHASE_DAILY_AFTER_MEM:
			// Wait for OK from touch screen
			break;

		case PHASE_DAILY_BEFORE_ACCURACY:
			if(current.num_chambers>0){
				num_daily = get_num_sources(TRUE);
				measurement[ch_num].future.dosetime = NO_TIME;
				// Removed: System is read at the beginning
				//EE_READ(syst, &current.system);

				if(num_daily == 0){
					get_amulet_message(L_NO_DAILY_SOURCE_DATA, String174);    // "6) NO DAILY SOURCE DATA"
					send_to_amulet_string(174, String174);
				}else{
					strcpy(String174, "6)");
					send_to_amulet_string(174, String174);
					SetAmuletByte(100, 0xFF);
					m_iPhase = PHASE_DAILY_FINISHED;
				}
				SetAmuletByte(190, 0xFF);
				//if(current.printer!=NONE_PRINTER) SetAmuletByte(101, 0xFF);
				if((current.printer!=NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
					SetAmuletByte(101, 0xFF);
			}
			m_iPhase = PHASE_DAILY_FINISHED;
			break;

		case PHASE_DAILY_PRINT:
			m_iPhase = PHASE_DAILY_FINISHED;
			printer_pressed = TRUE;
			beep_amulet();

			iret = prdailysec(okdata, -1);
			if(iret==1) dosig(current.printer);
			SetAmuletByte(111,0xFF);
			break;

		case PHASE_DAILY_FINISHED:
			printer_pressed = FALSE;
			break;

		case PHASE_DAILY_WAIT_2MIN_PRE:
			send_to_amulet_string(191, String191);
			SetAmuletByte(106, 0xFF);
			send_amulet_message(L_PLEASE_WAIT_2_MINUTES_FOR, 174);    // "PLEASE WAIT 2 Minutes for"
			send_amulet_message(L_STABILIZATION, 120);    // "Stabilization"
			SetAmuletByte(103, 0xFF);
			wait = g_csec_tstamp + 12000;
			m_iPhase = PHASE_DAILY_WAIT_2MIN;
			break;

		case PHASE_DAILY_WAIT_2MIN:
			if(g_csec_tstamp > wait){
				m_iPhase = PHASE_DAILY_WAIT_2MIN_POST;
			}
			break;

		case PHASE_DAILY_WAIT_2MIN_POST:
			send_to_amulet_string(191, String191);
			SetAmuletByte(189, 0xFB);
			send_to_amulet_string(174, "");
			send_to_amulet_string(120, "");
			SetAmuletByte(104, 0xFF);
			if(current.num_chambers>1){
				SetAmuletByte(21, 0xFF);
			}
			SetAmuletByte(105, 0xFF);
			beep_amulet();
			m_iPhase = PHASE_DAILY_AFTER_VOLTAGE;
			break;

		default:
			break;
	}
}

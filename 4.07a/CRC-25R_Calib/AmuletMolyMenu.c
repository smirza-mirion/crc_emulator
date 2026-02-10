#define PHASE_MOLY_PRE_INIT 0
#define PHASE_MOLY_SELECT_METHOD 1
#define PHASE_MOLY_SELECTED_MALLINKRODT 2
#define PHASE_MOLY_SELECTED_BRISTOL 3
#define PHASE_MOLY_SELECTED_CAPINTEC 4
#define PHASE_MOLY_PRE_BKG 5
#define PHASE_MOLY_WAIT_FOR_SCREEN 6
#define PHASE_MOLY_DISPLAY_MEASURE_BKG 7
#define PHASE_MOLY_WAIT_MEASURE 8
#define PHASE_MOLY_SETUP_BKG 9
#define PHASE_MOLY_DISPLAY_BAR_BKG 10
#define PHASE_MOLY_DISPLAY_BKG 11
#define PHASE_MOLY_SET_BKG 12
#define PHASE_MOLY_ZERO_BKG 13
#define PHASE_MOLY_PRE_MOLY_ASSAY 14
#define PHASE_MOLY_DISPLAY_BAR_MOLY 15
#define PHASE_MOLY_DISPLAY_MOLY 16
#define PHASE_MOLY_SET_MOLY 17
#define PHASE_MOLY_PRE_TC_ASSAY 18
#define PHASE_MOLY_DISPLAY_TC 19
#define PHASE_MOLY_SET_TC 20
#define PHASE_MOLY_GET_VOLUME 21
#define PHASE_MOLY_AFTER_VOLUME 22
#define PHASE_MOLY_PRINT 23
#define PHASE_MOLY_WAIT 24
#define PHASE_MOLY_TERMINATE 25
#define PHASE_MOLY_DISPLAY_ELUTE_START 26
#define PHASE_MOLY_ELUTE_START_WAIT 27
#define PHASE_MOLY_DISPLAY_TC99M_START 28
#define PHASE_MOLY_TC99M_START_WAIT 29
#define PHASE_MOLY_FINISH 255

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "chambfac.h"
#include "i2c.h"
#include "nuc.h"
#include "daily.h"
#include "screen.h"
#include "printer.h"

extern CURRENT  current;
extern int m_iPhase;
extern unsigned char m_ucClear;
extern unsigned char m_ucBargraphValue;
extern float moly_volume;
extern MEASUREMENT measurement[];
extern CHAMBER chamber[];
extern BKGDATA bkgdata[];
extern float mobkg;
extern char mo_actstr[12];
extern float moact;
extern char tc_actstr[12];
extern float moptc;
extern short dosehour;
extern time_t clock_time;
extern volatile char m_acTitle[52];
extern float m_fMaxValue;
extern float m_fMinValue;
extern unsigned char m_ucLenLimit;
extern unsigned char m_ucLenFractionLimit;
extern unsigned char m_ucKeyPadConfig;
extern unsigned short int m_uiSetKeyPad;
extern bool m_flgShowPlusMinus;
extern const float motc_factor[];

void send_to_amulet_string(uchar ucIndex, char message0[]);
void trim_and_shrink(char *acByte);
void moinit(void);
void init_mo_empty(short nuc_index, short ch_num);
void init_mo_assay(short nuc_index, short ch_num);
unsigned char PopPageStack(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
void Amulet_printMoly(time_t tc_stamp, char *tcactstr, float tcact, float volume, char *moactstr, float MOPTC, short DOSEHOUR);

void AmuletMoly_menu(void) {
	char mo[4];
	short num_mo;
	short ch_num = current.main_chamber;
	short i;
	short iNucIndex;
	short iPosition;
	short ypos;
	char acMsg[100];
	char acMsg2[100];
	static short iMethod;
	static char acBkgAct[100];
	static float fMoBkg;
	static float tcact, tcact0;
	NUCDATA nucdata;
	float limit;
	char fmtstr[10];
	short ndec;
	char strng[30];
	short ihour;
	float ratio;
	short line;
	static unsigned long int wait;
	static char string197[52];
	static char string195[52];
	static char string193[52];
	static char string191[52];
	static char string189[52];
	static char string187[52];
	static char string185[52];
	static bool wasover;
	static time_t tcstamp;
	//static time_t mostamp;
	static char concstr[12];
	float conc;
	char volstr[10];
	static bool printer_pressed;
	static bool high_displayed;
	//bool foundPET;
	//short index;
	char message[50],message1[50];

	switch(m_iPhase) {
		case PHASE_MOLY_PRE_INIT:

			SetAmuletByte(92,current.language);

			//SetAmuletString(197, "Moly Assay");
			send_amulet_message(L_MOLY_ASSAY,197);    // "Moly Assay"
			send_amulet_message(L_ENTER_PASSWORD,106);    // "Please Enter Password:"
			SetAmuletByte(255, 0xFF);
			if(current.num_chambers>1){
				if(chamber_type(0) == R_CHAMB && chamber_type(1) == R_CHAMB) SetAmuletByte(21, 0xFF);
			}

			m_ucBargraphValue = 0;
			high_displayed = FALSE;
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

				if(m_ucClear == 35){
					m_ucClear = 0;
					printer_pressed = FALSE;
					moly_volume = 100;
					EE_READ(mo_choice, (uchar *)mo);
					num_mo = 0;
					for(i=0; i<3; i++){
						if(mo[i] == 1) num_mo++;
					}

					iPosition = 1;
					switch(num_mo){
						case 1:
							for (i=0; i<3; i++) {
								if (mo[i] == 1) {
									switch(i) {
										case 0:
											iMethod = 1;
											break;

										case 1:
											iMethod = 2;
											break;

										case 2:
											iMethod = 0;
											break;
									}
								}
							}
							m_iPhase = PHASE_MOLY_PRE_BKG;
							break;

						case 2:
							for (i = 0; i < 3; i++) {
								if (mo[i] == 1) {
									if (iPosition == 1) {
										ypos = 179;
										iPosition++;
									}
									else {
										ypos = 352;
									}

									switch(i) {
										case 0:
											SetAmuletWord(157, 150);
											SetAmuletWord(158, ypos);
											send_amulet_message(L_CAPMAC_FOR_MAL_GEN,100);    // "CAPMAC for Mallinckrodt Gen"
											SetAmuletByte(254, 0xFF);
											break;

										case 1:
											SetAmuletWord(159, 150);
											SetAmuletWord(160, ypos);
											send_amulet_message(L_CAPMAC_FOR_BM_GEN,102);    // "CAPMAC for Bristol Myers Gen"
											SetAmuletByte(252, 0xFF);
											break;

										case 2:
											SetAmuletWord(161, 150);
											SetAmuletWord(162, ypos);
											send_amulet_message(L_CAPINTEC_CANISTER,104);    // "Capintec Canister"
											SetAmuletByte(250, 0xFF);
											break;
									}
								}
							}
							m_iPhase = PHASE_MOLY_SELECT_METHOD;
							break;

						case 3:
						case 0:
							SetAmuletWord(157, 150);
							SetAmuletWord(158, 136);
							SetAmuletWord(159, 150);
							SetAmuletWord(160, 266);
							SetAmuletWord(161, 150);
							SetAmuletWord(162, 396);
							send_amulet_message(L_CAPMAC_FOR_MAL_GEN,100);    // "CAPMAC for Mallinckrodt Gen"
							send_amulet_message(L_CAPMAC_FOR_BM_GEN,102);    // "CAPMAC for Bristol Myers Gen"
							send_amulet_message(L_CAPINTEC_CANISTER,104);    // "Capintec Canister"
							SetAmuletByte(254, 0xFF);
							SetAmuletByte(252, 0xFF);
							SetAmuletByte(250, 0xFF);
							m_iPhase = PHASE_MOLY_SELECT_METHOD;
							break;
					}
				}else{
					if(printer_pressed){
						send_to_amulet_string(197, string197);
						send_to_amulet_string(195, string195);
						send_to_amulet_string(193, string193);
						send_to_amulet_string(191, string191);
						send_to_amulet_string(189, string189);
						send_to_amulet_string(187, string187);
						send_to_amulet_string(185, string185);
						SetAmuletByte(101, 0xFF);
						m_iPhase = PHASE_MOLY_AFTER_VOLUME;
					}else{
						moly_volume = 100;
						EE_READ(mo_choice, (uchar *)mo);
						num_mo = 0;
						for(i=0; i<3; i++){
							if(mo[i] == 1) num_mo++;
						}

						iPosition = 1;
						switch(num_mo){
							case 1:
								for (i=0; i<3; i++) {
									if (mo[i] == 1) {
										switch(i) {
											case 0:
												iMethod = 1;
												break;

											case 1:
												iMethod = 2;
												break;

											case 2:
												iMethod = 0;
												break;
										}
									}
								}
								m_iPhase = PHASE_MOLY_PRE_BKG;
								break;

							case 2:
								for (i = 0; i < 3; i++) {
									if (mo[i] == 1) {
										if (iPosition == 1) {
											ypos = 179;
											iPosition++;
										}
										else {
											ypos = 352;
										}

										switch(i) {
											case 0:
												SetAmuletWord(157, 150);
												SetAmuletWord(158, ypos);
												send_amulet_message(L_CAPMAC_FOR_MAL_GEN,100);    // "CAPMAC for Mallinckrodt Gen"
												SetAmuletByte(254, 0xFF);
												break;

											case 1:
												SetAmuletWord(159, 150);
												SetAmuletWord(160, ypos);
												send_amulet_message(L_CAPMAC_FOR_BM_GEN,102);    // "CAPMAC for Bristol Myers Gen"
												SetAmuletByte(252, 0xFF);
												break;

											case 2:
												SetAmuletWord(161, 150);
												SetAmuletWord(162, ypos);
												send_amulet_message(L_CAPINTEC_CANISTER,104);    // "Capintec Canister"
												SetAmuletByte(250, 0xFF);
												break;
										}
									}
								}
								m_iPhase = PHASE_MOLY_SELECT_METHOD;
								break;

							case 3:
							case 0:
								SetAmuletWord(157, 150);
								SetAmuletWord(158, 136);
								SetAmuletWord(159, 150);
								SetAmuletWord(160, 266);
								SetAmuletWord(161, 150);
								SetAmuletWord(162, 396);
								send_amulet_message(L_CAPMAC_FOR_MAL_GEN,100);    // "CAPMAC for Mallinckrodt Gen"
								send_amulet_message(L_CAPMAC_FOR_BM_GEN,102);    // "CAPMAC for Bristol Myers Gen"
								send_amulet_message(L_CAPINTEC_CANISTER,104);    // "Capintec Canister"
								SetAmuletByte(254, 0xFF);
								SetAmuletByte(252, 0xFF);
								SetAmuletByte(250, 0xFF);
								m_iPhase = PHASE_MOLY_SELECT_METHOD;
								break;
						}
					}
				}
			}
			break;

		case PHASE_MOLY_SELECT_METHOD:
			// Wait for user to press key
			break;

		case PHASE_MOLY_SELECTED_MALLINKRODT:
			iMethod = 1;
			m_iPhase = PHASE_MOLY_PRE_BKG;
			break;

		case PHASE_MOLY_SELECTED_BRISTOL:
			iMethod = 2;
			m_iPhase = PHASE_MOLY_PRE_BKG;
			break;

		case PHASE_MOLY_SELECTED_CAPINTEC:
			iMethod = 0;
			m_iPhase = PHASE_MOLY_PRE_BKG;
			break;

		case PHASE_MOLY_PRE_BKG:
			if(current.num_chambers>0){
				send_to_amulet_string(193, "");
				switch(iMethod){
					case 0:
						//SetAmuletString(197, "Moly Assay - Capintec Canister Method");
						//strcpy(string197, "Moly Assay - Capintec Canister Method");
						send_amulet_message(L_MOLY_ASSAY_CAPINTEC_CANISTER_METHOD,197);    // "Moly Assay - Capintec Canister Method"
						get_amulet_message(L_MOLY_ASSAY_CAPINTEC_CANISTER_METHOD,string197);    // "Moly Assay - Capintec Canister Method"
						break;
					case 1:
						//SetAmuletString(197, "Moly Assay - Mallinckrodt Gen Method");
						//strcpy(string197, "Moly Assay - Mallinckrodt Gen Method");
						send_amulet_message(L_MOLY_ASSAY_MAL_GEN_METHOD,197);    // "Moly Assay - Mallinckrodt Gen Method"
						get_amulet_message(L_MOLY_ASSAY_MAL_GEN_METHOD,string197);    // "Moly Assay - Mallinckrodt Gen Method"
						break;
					case 2:
						//SetAmuletString(197, "Moly Assay - Bristol Myers Gen Method");
						//strcpy(string197, "Moly Assay - Bristol Myers Gen Method");
						send_amulet_message(L_MOLY_ASSAY_BM_GEN_METHOD,197);    // "Moly Assay - Bristol Myers Gen Method"
						get_amulet_message(L_MOLY_ASSAY_BM_GEN_METHOD,string197);    // "Moly Assay - Bristol Myers Gen Method"
						break;
				}

				if(iMethod==0){
					//SetAmuletString(195, "1) Measure Canister Bkg:");
					//strcpy(string195, "1) Measure Canister Bkg:");
					send_amulet_message(L_MEASURE_CANISTER_BKG,195);    // "1) Measure Canister Bkg:"
					get_amulet_message(L_MEASURE_CANISTER_BKG,string195);    // "1) Measure Canister Bkg:"
				}else{
					//SetAmuletString(195, "1) Measure CAPMAC Bkg:");
					//strcpy(string195, "1) Measure CAPMAC Bkg:");
					send_amulet_message(L_MEASURE_CAPMAC_BKG,195);    // "1) Measure CAPMAC Bkg:"
					get_amulet_message(L_MEASURE_CAPMAC_BKG,string195);    // "1) Measure CAPMAC Bkg:"
				}
				send_to_amulet_string(193, "");
				SetAmuletByte(238, 0xFF);

				m_iPhase = PHASE_MOLY_WAIT_FOR_SCREEN;
			}
			break;

		case PHASE_MOLY_WAIT_FOR_SCREEN:
			break;

		case PHASE_MOLY_DISPLAY_MEASURE_BKG:
			if(current.num_chambers>0){
				send_amulet_message(L_MEASURE_BKG,108);    // "Measure Bkg"
				send_amulet_message(L_SKIP_BKG,109);    // "Skip Bkg"
				SetAmuletByte(237, 0xFF);
				m_iPhase = PHASE_MOLY_WAIT_MEASURE;
			}
			break;

		case PHASE_MOLY_WAIT_MEASURE:
			break;

		case PHASE_MOLY_SETUP_BKG:
			if(current.num_chambers>0){
				if(iMethod==0) iNucIndex = NuclideData_getIndexFromName("MoCAN");
				else iNucIndex = NuclideData_getIndexFromName("MoCAP");
				strcpy(chamber[ch_num].nucdata.name,"Mo99  ");
				disp_nuclide();
				measurement[ch_num].mode = MOBKGMODE;
				moinit();
				init_mo_empty(iNucIndex, ch_num);
				m_iPhase = PHASE_MOLY_DISPLAY_BAR_BKG;
			}
			break;

		case PHASE_MOLY_DISPLAY_BAR_BKG:
			if(current.num_chambers>0){
				if(m_ucBargraphValue != bkgdata[ch_num].count){
					m_ucBargraphValue = bkgdata[ch_num].count;
				}
				if(bkgdata[ch_num].count == 51){
					send_amulet_message(L_ACCEPT_BUTTON,110);    // "Accept"
					SetAmuletByte(236, 0xFF);
					m_iPhase = PHASE_MOLY_DISPLAY_BKG;
				}
			}
			break;

		case PHASE_MOLY_DISPLAY_BKG:
			if(current.num_chambers>0){
				if(bkgdata[ch_num].status != TEST_WAIT){
					if(measurement[ch_num].display_flag_2){
						measurement[ch_num].display_flag_2 = FALSE;
						if(!measurement[ch_num].over_flag){
							//if(measurement[ch_num].gain == 0){
							if((measurement[ch_num].meas + chamber[ch_num].bkg) < 2.95679){
								strcpy(acBkgAct, measurement[ch_num].actstr);
								fMoBkg = measurement[ch_num].meas;
								strcpy(acMsg, acBkgAct);
								trim_and_shrink(acMsg);
								strcpy(acMsg2, "Mo99 ");
								strcat(acMsg2, acMsg);
								if(bkgdata[ch_num].status == TEST_GOOD){
									//strcat (acMsg2, " OK");
									strcat(acMsg2," ");
									get_amulet_message(L_CAPS_OK,message);    // "OK"
									strcat(acMsg2,message);
								}
								if(bkgdata[ch_num].status == BKG_HIGH){
									//strcat(acMsg2, " HIGH");
									strcat(acMsg2," ");
									get_amulet_message(L_HIGH,message);    // "HIGH"
									strcat(acMsg2,message);
								}
								send_to_amulet_string(155, acMsg2);
							}else{
								PopPageStack();
								//Amulet_DisplayError("Background", "BACKGROUND\nTOO HIGH", TRUE);
								get_amulet_message(L_BACKGROUND2,message);    // "Background"
								get_amulet_message(L_BACKGROUND_TOO_HIGH2,message1);    // "BACKGROUND\n TOO HIGH"
								Amulet_DisplayError(message,message1, TRUE);
								return;
							}
						}else{
							PopPageStack();
							//Amulet_DisplayError("Moly Bkg", "OVER RANGE\nERROR", TRUE);
							get_amulet_message(L_MOLY_BKG,message);    // "Moly Bkg"
							get_amulet_message(L_OVERRANGE_ERROR,message1);    // "OVER RANGE\nERROR"
							Amulet_DisplayError(message,message1, TRUE);
							return;
						}
					}
				}
			}
			break;

		case PHASE_MOLY_SET_BKG:
			if(current.num_chambers>0){
				m_ucBargraphValue = 0;
				mobkg = fMoBkg;
				if(current.moly_streamlined) SetAmuletByte(231, 0xFF);
				strcpy(acMsg, acBkgAct);
				trim_and_shrink(acMsg);
				strcat(acMsg, " (Mo99)");
				send_to_amulet_string(193, acMsg);
				strcpy(string193, acMsg);
				if(current.moly_streamlined) m_iPhase = PHASE_MOLY_PRE_MOLY_ASSAY;
				else m_iPhase = PHASE_MOLY_DISPLAY_ELUTE_START;
			}
			break;

		case PHASE_MOLY_ZERO_BKG:
			if(current.num_chambers>0){
				mobkg = 0;
				if(current.moly_streamlined) SetAmuletByte(231, 0xFF);
				//SetAmuletString(193, "Skipped");
				send_amulet_message(L_SKIPPED,193);    // "Skipped"
				//strcpy(string193, "Skipped");
				get_amulet_message(L_SKIPPED,string193);    // "Skipped"
				if(current.moly_streamlined) m_iPhase = PHASE_MOLY_PRE_MOLY_ASSAY;
				else m_iPhase = PHASE_MOLY_DISPLAY_ELUTE_START;
			}
			break;

		case PHASE_MOLY_PRE_MOLY_ASSAY:
			if(current.num_chambers>0){
				erase_screen();
				if(iMethod == 0){
					//SetAmuletString(191, "2) Eluate in CANISTER: ");
					send_amulet_message(L_ELUATE_IN_CANISTER,191);    // "2) Eluate in CANISTER: "
					//strcpy(string191, "2) Eluate in CANISTER: ");
					get_amulet_message(L_ELUATE_IN_CANISTER,string191);    // "2) Eluate in CANISTER: "
					iNucIndex = NuclideData_getIndexFromName("MoCAN");
				}else{
					//SetAmuletString(191, "2) Eluate in CAPMAC: ");
					send_amulet_message(L_ELUATE_IN_CAPMAC,191);    // "2) Eluate in CAPMAC: "
					//strcpy(string191, "2) Eluate in CAPMAC: ");
					get_amulet_message(L_ELUATE_IN_CAPMAC,string191);    // "2) Eluate in CAPMAC: "
					iNucIndex = NuclideData_getIndexFromName("MoCAP");
				}
				strcpy(&chamber[ch_num].nucdata.name[0],"Mo99  ");
				disp_nuclide();
				display_text(80,0,"ASSAY",0,MEDIUM,NORMAL);
				moinit();
				init_mo_assay(iNucIndex, ch_num);
				m_iPhase = PHASE_MOLY_DISPLAY_BAR_MOLY;
			}
			break;

		case PHASE_MOLY_DISPLAY_BAR_MOLY:
			if(current.num_chambers>0){
				if(m_ucBargraphValue != bkgdata[ch_num].count){
					m_ucBargraphValue = bkgdata[ch_num].count;
				}

				if(bkgdata[ch_num].count == 51){
					send_amulet_message(L_ACCEPT_BUTTON,110);    // "Accept"
					SetAmuletByte(235, 0xFF);
					m_iPhase = PHASE_MOLY_DISPLAY_MOLY;
				}
			}
			break;

		case PHASE_MOLY_DISPLAY_MOLY:
			if(current.num_chambers>0){
				if(bkgdata[ch_num].status != TEST_WAIT){
					if(measurement[ch_num].display_flag_2){
						measurement[ch_num].display_flag_2 = FALSE;
						if(measurement[ch_num].act0 < -100.0e-6){
							PopPageStack();
							//Amulet_DisplayError("ERROR", "NEGATIVE ACTIVITY", TRUE);
							get_amulet_message(L_ERROR,message);    // "ERROR"
							get_amulet_message(L_NEGATIVE_ACTIVITY,message1);    // "NEGATIVE ACTIVITY"
							Amulet_DisplayError(message, message1, TRUE);
						}else if(((measurement[ch_num].act0 > 1.0e-3) || (measurement[ch_num].over_flag))&& (!high_displayed)){
							high_displayed = TRUE;
							if(!measurement[ch_num].over_flag){
								strcpy(mo_actstr, measurement[ch_num].actstr);
								strcpy(acMsg, mo_actstr);
								trim_and_shrink(acMsg);
								strcpy(acMsg2, "Mo99 ");
								strcat(acMsg2, acMsg);
								send_to_amulet_string(153, acMsg2);
							}else{
								//SetAmuletString(153, "OVER RANGE");
								send_amulet_message(L_OVERRANGE,153);    // "OVER RANGE"
							}
							//SetAmuletString(185, "MO HIGH Terminate?");
							send_amulet_message(L_MO_HIGH_TERMINATE_QUESTION,185);    // "MO HIGH Terminate?"
							send_amulet_message(L_YES,111);    // "Yes"
							send_amulet_message(L_NO,112);    // "No"
							SetAmuletByte(234, 0xFF);
							SetAmuletByte(56, 0xFF);
							m_iPhase = PHASE_MOLY_TERMINATE;
						}else{
							if(!measurement[ch_num].over_flag){
								strcpy(mo_actstr, measurement[ch_num].actstr);

								// Locked: CI
								//if(current.system == CI)
								//	moact = measurement[ch_num].act / 1.0e-6;
								//else
								//	moact = measurement[ch_num].act / 1.0e+6;
								// Changed:
								//moact = measurement[ch_num].act / 1.0e-6;
								moact = measurement[ch_num].act0;

								//mostamp = clock_time;
								strcpy(acMsg, mo_actstr);
								trim_and_shrink(acMsg);
								strcpy(acMsg2, "Mo99 ");
								strcat(acMsg2, acMsg);

								send_to_amulet_string(153, acMsg2);
							}else{
								//SetAmuletString(153, "OVER RANGE");
								send_amulet_message(L_OVERRANGE,153);    // "OVER RANGE"
								measurement[ch_num].act0 = 100.0;
							}
						}
					}
				}
			}
			break;

		case PHASE_MOLY_SET_MOLY:
			if(current.num_chambers>0){
				if(measurement[ch_num].act0 == 100.0){
					PopPageStack();
					//Amulet_DisplayError("Moly", "OVER RANGE\nERROR", TRUE);
					get_amulet_message(L_OVERRANGE_ERROR,message);    // "OVER RANGE\nERROR"
					Amulet_DisplayError("Moly", message, TRUE);
					return;
				}else{
					m_ucBargraphValue = 0;
					if(current.moly_streamlined) SetAmuletByte(230, 0xFF);
					strcpy(acMsg, mo_actstr);
					trim_and_shrink(acMsg);
					strcat(acMsg, " (Mo99)");
					send_to_amulet_string(189, acMsg);
					strcpy(string189, acMsg);
					if(current.moly_streamlined) m_iPhase = PHASE_MOLY_PRE_TC_ASSAY;
					else m_iPhase = PHASE_MOLY_DISPLAY_TC99M_START;
				}
			}
			break;

		case PHASE_MOLY_PRE_TC_ASSAY:
			if(current.num_chambers>0){
				//SetAmuletString(187, "3) Measure Tc99m Assay:");
				send_amulet_message(L_MEASURE_TC_ASSAY,187);    // "3) Measure Tc99m Assay:"
				//strcpy(string187, "3) Measure Tc99m Assay:");
				get_amulet_message(L_MEASURE_TC_ASSAY,string187);    // "3) Measure Tc99m Assay:"
				erase_screen();
				measurement[ch_num].mode = MEASMODE;
				switch(iMethod){
					case 0:
						iNucIndex = NuclideData_getIndexFromName("Tc99m");
						break;
					case 1:
						iNucIndex = NuclideData_getIndexFromName("TcCAPM");
						break;
					case 2:
						iNucIndex = NuclideData_getIndexFromName("TcCAPB");
						break;
				}
				NuclideData_getNuclide(iNucIndex, &nucdata);
				chamber[ch_num].nucdata = nucdata;
				strcpy(&chamber[ch_num].nucdata.name[0],"Tc99m ");
				measurement[ch_num].response = nucdata.response[chamber_type(ch_num)];
				disp_nuclide();

				nucinit(ch_num,TRUE);
				respcor(ch_num,&measurement[ch_num].response);
				m_iPhase = PHASE_MOLY_DISPLAY_TC;
			}
			break;

		case PHASE_MOLY_DISPLAY_TC:
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					measurement[ch_num].display_flag_2 = FALSE;
					if(!measurement[ch_num].over_flag){
						strcpy(tc_actstr, measurement[ch_num].actstr);
						tcact0 = measurement[ch_num].act0;
						wasover = measurement[ch_num].wasover;
						tcstamp = clock_time;
						strcpy(acMsg, tc_actstr);
						trim_and_shrink(acMsg);
						strcpy(acMsg2, "Tc99m ");
						strcat(acMsg2, acMsg);
						send_to_amulet_string(151, acMsg2);
					}else{
						//SetAmuletString(151, "OVER RANGE");
						send_amulet_message(L_OVERRANGE,151);    // "OVER RANGE"
						tcact0 = 100.0;
					}
				}
			}
			break;

		case PHASE_MOLY_SET_TC:
			if(current.num_chambers>0){
				if(tcact0 == 100.0){
					PopPageStack();
					//Amulet_DisplayError("Moly", "OVER RANGE\nERROR", TRUE);
					get_amulet_message(L_OVERRANGE_ERROR,message);    // "OVER RANGE\nERROR"
					Amulet_DisplayError("Moly", message, TRUE);
					return;
				}else{
					strcpy(acMsg, tc_actstr);
					trim_and_shrink(acMsg);
					strcat(acMsg, " (Tc99m)");
					send_to_amulet_string(185, acMsg);
					strcpy(string185, acMsg);
					wait = g_csec_tstamp + 100;
					m_iPhase = PHASE_MOLY_GET_VOLUME;
				}
			}
			break;

		case PHASE_MOLY_GET_VOLUME:
			if(g_csec_tstamp > wait){
				//strcpy((char *)m_acTitle, "Please Enter Volume(ml)");
				get_amulet_message(L_PLEASE_ENTER_VOLUME,(char *)m_acTitle);    // "Please Enter Volume(ml):"
				//m_uiOffsetX = 488;
				//m_uiOffsetY = 160;
				m_fMinValue = 1.0;
				m_fMaxValue = 100.0;
				m_ucLenLimit = 3;
				m_ucLenFractionLimit = 2;
				m_ucKeyPadConfig = 2;
				m_uiSetKeyPad = 63;
				m_flgShowPlusMinus = FALSE;
				SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
				PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
				printer_pressed = TRUE;
				m_iPhase = PHASE_MOLY_WAIT;
			}
			break;

		case PHASE_MOLY_AFTER_VOLUME:
			if(current.num_chambers>0){
				tcact = tcact0;
				// Locked: CI
				//if(current.system == CI)
				//	tcact /= 1.0e-3;
				//else
				//	tcact /= 1.0e+9;
				//tcact /= 1.0e-3;

				if(wasover){
					// Overrange Error
					//SetAmuletString(181, "Over Range Error");
					send_amulet_message(L_OVERRANGE_ERROR2,181);    // "Over Range Error"
					SetAmuletByte(81, 0xFF);
					delayloop(5);
				// Locked: CI
				//}else if(((current.system == CI) && (tcact < 1.0)) || ((current.system == BQ) && (tcact < 0.037))){
				}else if(tcact < .001){
					// Activity Too Low Error
					//SetAmuletString(181, "Activity Too Low Error");
					send_amulet_message(L_ACTIVITY_TOO_LOW_ERROR,181);    // "Activity Too Low Error"
					SetAmuletByte(81, 0xFF);
					delayloop(5);
				}else{
					//if(current.printer != NONE_PRINTER)
					if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
						SetAmuletByte(100, 0xFF);

					conc = tcact0 / moly_volume;
					// Locked: CI
					//format_activity(conc, current.system, concstr);
					//format_activity(conc, CI, concstr);
					format_activity_system(conc, concstr);
					trim_and_shrink(concstr);
					strcat(concstr, "/ml");
					strcpy(acMsg, "Tc99(conc): ");
					strcat(acMsg, concstr);
					send_to_amulet_string(183, acMsg);
					strcpy(acMsg, "Vol: ");
					sprintf(volstr, "%.2f ml", moly_volume);
					strcat(acMsg, volstr);
					send_to_amulet_string(179, acMsg);

					// Changed:
					//moptc = moact / tcact;
					moptc = moact / tcact;
					moptc *= 1000;

					EE_READ(mo_tc_lim, (uchar *) &limit);
					if(moptc < 0) moptc = 0.;

					strcpy(fmtstr,"%5. f");
					ndec = 3;

					if(moptc >= 1.) ndec = 0;

					fmtstr[3] = ndec + '0';
					sprintf(strng,fmtstr,moptc);

					strcpy(acMsg, "Mo99/Tc99m: ");
					strcat(acMsg, strng);
					// Locked: CI
					if(current.system == CI){
						strcat(acMsg, " (uCi/mCi)");
					}else{
						strcat(acMsg, " (MBq/GBq)");
					}
					//strcat(acMsg, " (uCi/mCi)");
					send_to_amulet_string(181, acMsg);

					dosehour = 0;
					for(ihour = 12; ihour >= 0; ihour--){
						ratio = limit / motc_factor[ihour];
						if(moptc < ratio){
							dosehour = ihour;
							break;
						}
					}

					line = 5;

					if(dosehour < 12){
						if(dosehour >= 6 && dosehour < 9){
							//display_text(10,22,"CAUTION",0,MEDIUM,NORMAL);
							//display_medium_message(MOLY_10,22,0,NORMAL);
							//SetAmuletString(177, "CAUTION");
							send_amulet_message(L_CAUTION,177);    // "CAUTION"
							SetAmuletByte(77, 0xFF);
							delayloop(5);
							line++;
						}

						if(dosehour > 2 && dosehour < 6){
							//display_text(10,22,"MO HIGH",0,MEDIUM,NORMAL);
							//display_medium_message(MOLY_11,22,0,NORMAL);
							//SetAmuletString(177, "MO HIGH");
							send_amulet_message(L_MO_HIGH,177);    // "MO HIGH"
							SetAmuletByte(77, 0xFF);
							delayloop(5);
							line++;
						}

						if(dosehour > 2 && dosehour < 12){
							//splay_text(12,40,"DO NOT USE AFTER",0,SMALL,NORMAL);
							//display_small_message(MOLY_12,40,0,NORMAL);
							//sprintf(acMsg,"DO NOT USE AFTER %d HOURS", dosehour);
							get_amulet_message(L_DO_NOT_USE_AFTER_HOURS,message);    // "DO NOT USE AFTER %d HOURS"
							sprintf(acMsg,message, dosehour);
							switch(line){
								case 5:
									send_to_amulet_string(177, acMsg);
									SetAmuletByte(77, 0xFF);
									delayloop(5);
									break;
								case 6:
									send_to_amulet_string(173, acMsg);
									SetAmuletByte(73, 0xFF);
									delayloop(5);
									break;
								case 7:
									send_to_amulet_string(169, acMsg);
									SetAmuletByte(69, 0xFF);
									delayloop(5);
									break;
								case 8:
									send_to_amulet_string(165, acMsg);
									SetAmuletByte(65, 0xFF);
									delayloop(5);
									break;
								case 9:
									send_to_amulet_string(161, acMsg);
									SetAmuletByte(61, 0xFF);
									delayloop(5);
									break;
							}
							line++;

							//display_text(18,48,strng,0,SMALL,NORMAL);
							//display_text(36,48,"HOURS",0,SMALL,NORMAL);
							//display_small_message(MOLY_13,48,0,NORMAL);
						}

						if(dosehour <= 2){
							switch(line){
								case 5:
									//SetAmuletString(177, "MO TOO HIGH");
									//SetAmuletString(173, "DO NOT USE");
									send_amulet_message(L_MO_TOO_HIGH,177);    // "MO TOO HIGH"
									send_amulet_message(L_DO_NOT_USE,173);    // "DO NOT USE"
									SetAmuletByte(77, 0xFF);
									SetAmuletByte(73, 0xFF);
									delayloop(5);
									break;
								case 6:
									//SetAmuletString(173, "MO TOO HIGH");
									//SetAmuletString(169, "DO NOT USE");
									send_amulet_message(L_MO_TOO_HIGH,173);    // "MO TOO HIGH"
									send_amulet_message(L_DO_NOT_USE,169);    // "DO NOT USE"
									SetAmuletByte(73, 0xFF);
									SetAmuletByte(69, 0xFF);
									delayloop(5);
									break;
								case 7:
									//SetAmuletString(169, "MO TOO HIGH");
									//SetAmuletString(165, "DO NOT USE");
									send_amulet_message(L_MO_TOO_HIGH,169);    // "MO TOO HIGH"
									send_amulet_message(L_DO_NOT_USE,165);    // "DO NOT USE"
									SetAmuletByte(69, 0xFF);
									SetAmuletByte(65, 0xFF);
									delayloop(5);
									break;
								case 8:
									//SetAmuletString(165, "MO TOO HIGH");
									//SetAmuletString(161, "DO NOT USE");
									send_amulet_message(L_MO_TOO_HIGH,165);    // "MO TOO HIGH"
									send_amulet_message(L_DO_NOT_USE,161);    // "DO NOT USE"
									SetAmuletByte(65, 0xFF);
									SetAmuletByte(61, 0xFF);
									delayloop(5);
									break;
								case 9:
									//SetAmuletString(161, "MO TOO HIGH");
									//SetAmuletString(157, "DO NOT USE");
									send_amulet_message(L_MO_TOO_HIGH,161);    // "MO TOO HIGH"
									send_amulet_message(L_DO_NOT_USE,157);    // "DO NOT USE"
									SetAmuletByte(61, 0xFF);
									SetAmuletByte(57, 0xFF);
									delayloop(5);
									break;
							}
							//display_text(0,22,"MO TOO HIGH",0,MEDIUM,NORMAL);
							//display_text(10,34,"DO NOT USE",0,MEDIUM,NORMAL);
							//display_medium_message(MOLY_14,22,0,NORMAL);
							//display_medium_message(MOLY_15,34,0,NORMAL);
						}
					}
				}
				m_iPhase = PHASE_MOLY_FINISH;
			}
			break;

		case PHASE_MOLY_PRINT:
			m_iPhase = PHASE_MOLY_FINISH;
			printer_pressed = TRUE;
			beep_amulet();
			Amulet_printMoly(tcstamp, tc_actstr, tcact0, moly_volume, mo_actstr, moptc, dosehour);
			SetAmuletByte(102, 0xFF);
			break;

		case PHASE_MOLY_FINISH:
			printer_pressed = FALSE;
			break;

		case PHASE_MOLY_WAIT:
			break;

		case PHASE_MOLY_TERMINATE:
			break;

		case PHASE_MOLY_DISPLAY_ELUTE_START:
			if(current.num_chambers>0){
				send_amulet_message(L_START_MEASUREMENT,199);    // "Start Measurement"
				if(iMethod == 0){
					//SetAmuletString(191, "2) Eluate in CANISTER: ");
					send_amulet_message(L_ELUATE_IN_CANISTER,191);    // "2) Eluate in CANISTER: "
					//strcpy(string191, "2) Eluate in CANISTER: ");
					get_amulet_message(L_ELUATE_IN_CANISTER,string191);    // "2) Eluate in CANISTER: "
				}else{
					//SetAmuletString(191, "2) Eluate in CAPMAC: ");
					send_amulet_message(L_ELUATE_IN_CAPMAC,191);    // "2) Eluate in CAPMAC: "
					//strcpy(string191, "2) Eluate in CAPMAC: ");
					get_amulet_message(L_ELUATE_IN_CAPMAC,string191);    // "2) Eluate in CAPMAC: "
				}
				SetAmuletByte(233, 0xFF);
				m_iPhase = PHASE_MOLY_ELUTE_START_WAIT;
			}
			break;

		case PHASE_MOLY_ELUTE_START_WAIT:
			break;

		case PHASE_MOLY_DISPLAY_TC99M_START:
			if(current.num_chambers>0){
				send_amulet_message(L_START_MEASUREMENT,199);    // "Start Measurement"
				//SetAmuletString(187, "3) Measure Tc99m Assay:");
				send_amulet_message(L_MEASURE_TC_ASSAY,187);    // "3) Measure Tc99m Assay:"
				//strcpy(string187, "3) Measure Tc99m Assay:");
				get_amulet_message(L_MEASURE_TC_ASSAY,string187);    // "3) Measure Tc99m Assay:"
				SetAmuletByte(232, 0xFF);
				m_iPhase = PHASE_MOLY_TC99M_START_WAIT;
			}
			break;

		case PHASE_MOLY_TC99M_START_WAIT:
			break;

		default:
			break;
	}
}

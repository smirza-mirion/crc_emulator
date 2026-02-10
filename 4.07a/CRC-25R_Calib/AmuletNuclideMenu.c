#define PHASE_NUCLIDE_PRE_INIT 0
#define PHASE_NUCLIDE_REFRESH_SCREEN 1
#define PHASE_NUCLIDE_WAIT 2
#define PHASE_NUCLIDE_PAGE_DOWN 3
#define PHASE_NUCLIDE_PAGE_UP 4
#define PHASE_NUCLIDE_ROW1 5
#define PHASE_NUCLIDE_ROW2 6
#define PHASE_NUCLIDE_ROW3 7
#define PHASE_NUCLIDE_ROW4 8
#define PHASE_NUCLIDE_ROW5 9
#define PHASE_NUCLIDE_ROW6 10
#define PHASE_NUCLIDE_ROW7 11
#define PHASE_NUCLIDE_ROW8 12
#define PHASE_NUCLIDE_ROW9 13
#define PHASE_NUCLIDE_ROW10 14
#define PHASE_NUCLIDE_B1_1 15
#define PHASE_NUCLIDE_B1_2 16
#define PHASE_NUCLIDE_B1_3 17
#define PHASE_NUCLIDE_B1_4 18
#define PHASE_NUCLIDE_B2_1 19
#define PHASE_NUCLIDE_B2_2 20
#define PHASE_NUCLIDE_B2_3 21
#define PHASE_NUCLIDE_B2_4 22
#define PHASE_NUCLIDE_B3_1 23
#define PHASE_NUCLIDE_B3_2 24
#define PHASE_NUCLIDE_B3_3 25
#define PHASE_NUCLIDE_B3_4 26
#define PHASE_NUCLIDE_B4_1 27
#define PHASE_NUCLIDE_B4_2 28
#define PHASE_NUCLIDE_B4_3 29
#define PHASE_NUCLIDE_B4_4 30
#define PHASE_NUCLIDE_B5_1 31
#define PHASE_NUCLIDE_B5_2 32
#define PHASE_NUCLIDE_B5_3 33
#define PHASE_NUCLIDE_B5_4 34
#define PHASE_NUCLIDE_SELECT 35
#define PHASE_NUCLIDE_SUBMIT_BLANK 36

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "chambfac.h"
#include "nuc.h"
#include "mca.h"
#include "i2c.h"
#include "message.h"

extern CURRENT  current;
extern int m_iPhase;
extern uchar m_ucNuclideClearButton;
extern uchar m_ucNuclideConfig;
extern volatile char m_acTitle[51];
extern unsigned char m_ucHotKeyNuclideID2[UPPER_LIMIT_CHAMB][20];
extern unsigned char m_ucSetNuclide;
extern CHAMBER  chamber[];
extern volatile bool DisplayActivity_SendActivity;
extern unsigned char m_ucAddInventory_Nuclide;
extern unsigned char m_ucInfo_DecayNuclide;
extern short AmuletSetupCalNumMenu_NuclideID;
extern char AmuletSetupCalNumMenu_UserRCal[10];
extern char AmuletSetupCalNumMenu_UserPCal[10];
extern KEYDEF AmuletSetupRemoteMenu_mirrorRemoteNuc[8];
extern short AmuletDoseTableMenu_nuclideID;
extern short AmuletSetupSourcesMenu_constancyCh[12];
extern short AmuletSetupLinearityStandardMenu_nuclideID;
extern WELLWIPETYPE AmuletWellSetupTypes_wellWipeType;
extern WELLWIPELOCATION AmuletWellSetupAddEditLocation_wellWipeLocation;
extern char AmuletWellMDATestMenu_nuclideID;
extern char generalNuclideID;
extern float startEV[10];
extern float endEV[10];
extern unsigned char AmuletSetupRHotkeys_M[8];
extern unsigned char AmuletSetupRHotkeys_N[20];
extern unsigned char AmuletSetupPHotkeys_M[8];
extern unsigned char AmuletSetupPHotkeys_N[20];
extern unsigned char m_ucSetWellMeasure;
extern int AmuletWellMeasurementMenu_generalDetectorTest;
extern char AmuletAutoLinearityTest_nuclideName[10];
extern HALFLIFECALC HalflifeCalc_test;

void populateNuclideHotKey(char hotkeytype, bool original);
void trim_and_shrink(char *acByte);
void GetExtendedNuclideString(unsigned char ucNuclideID, char *acMsg);
void SetAmuletBackHTML(void);
void trim(char *acByte);
void splitNuclide(char *input, char *output, bool para);
void send_to_amulet_string(uchar ucIndex, char message0[]);

unsigned char m_ucNuclideMap[ALLNUC];
unsigned char m_ucNuclideMapCount;
static unsigned char m_ucCurrentNuclide = 0xFF;

#define ITEMS_PER_PAGE 10
#define MAX_NUCLIDE 95
void AmuletNuclide_menu(void) {
	short i, j;
//	short nuc_index;
	static short first_map_item_displayed;
	static short selected_map_item;
	static char tagbothusercalnum;
	static char hotkeytype;
	static bool original;
	short ch_num = current.main_chamber;
	short ch_type;
	char nucshrink[25];
	char message[51];
	bool showHotKeys;
	unsigned char uctempIndex;
	//unsigned long int delay;

	switch(m_iPhase) {
		case PHASE_NUCLIDE_PRE_INIT:
			send_amulet_message(L_ACCEPT_BUTTON, 90);    // "Accept"
			send_amulet_message(L_CANCEL_BUTTON, 91);    // "Cancel"
			send_amulet_message(L_CLEAR_SELECTED_NUCLIDE, 95);    // "Clear Selected\nNuclide"
			send_amulet_message(L_NUCLIDE, 93);    // "Nuclide"
			send_amulet_message(L_SETUP_NUCLIDE_HALFLIFE, 94);    // "Halflife"
			if(current.num_chambers>0){
				hotkeytype = -1;
				original = TRUE;
				ch_type = chamber_type(ch_num);
				showHotKeys = TRUE;
				if(m_ucNuclideClearButton) SetAmuletByte(100, 0xFF);

				if((m_ucNuclideConfig == 8) && chamber_C()) m_ucNuclideConfig = 17; // Setup Standard Linearity and Setup HotKeys for 77t uses the R Chamber Screen
				if((m_ucNuclideConfig == 8) && chamber_K()) m_ucNuclideConfig = 18; // Setup Standard Linearity and Setup HotKeys for 77t uses the R Chamber Screen

				if(m_ucNuclideConfig!=0){
					j = 0;
					switch(m_ucNuclideConfig){
						case 1:
							for(i=USERNUC; i<(USERNUC+10); i++){
								if(NuclideData_isShow(i)){
									m_ucNuclideMap[j] = i;
									j++;
								}
							}
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i])){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							break;

						case 2:
							for(i=USERNUC; i<(USERNUC+10); i++){
								if(NuclideData_isShow(i) && (NuclideData_getEffectiveResponse(i, ch_type)!=0.0)){
									m_ucNuclideMap[j] = i;
									j++;
								}
							}
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i]) && (NuclideData_getEffectiveResponse(SortedBuiltInIndex[i], ch_type)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							break;

						case 3:
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i])){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = -1;
							break;

						case 4:
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i]) && (NuclideData_getEffectiveResponse(SortedBuiltInIndex[i], 0)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = -1;
							break;

						case 5:
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i]) && (NuclideData_getEffectiveResponse(SortedBuiltInIndex[i], 1)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = -1;
							break;

						case 6:	// Energy1 and Well Effective
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEff(i, DET_WELL)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEff(SortedBuiltInIndex[i], DET_WELL)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}

							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 7:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if(NuclideData_getPrimary(i) != 0.0){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}

							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 8:
							for(i=USERNUC; i<(USERNUC+10); i++){
								if(NuclideData_isShow(i) && (NuclideData_getEffectiveResponse(i, R_CHAMB)!=0.0)){
									m_ucNuclideMap[j] = i;
									j++;
								}
							}
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i]) && (NuclideData_getEffectiveResponse(SortedBuiltInIndex[i], R_CHAMB)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = -1;
							break;

						case 9:
							for(i=USERNUC; i<(USERNUC+10); i++){
								if(NuclideData_isShow(i) && (NuclideData_getEffectiveResponse(i, P_CHAMB)!=0.0)){
									m_ucNuclideMap[j] = i;
									j++;
								}
							}
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i]) && (NuclideData_getEffectiveResponse(SortedBuiltInIndex[i], P_CHAMB)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = -1;
							break;

						case 10:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEff(i, DET_BETA)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEff(SortedBuiltInIndex[i], DET_BETA)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 11:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEff(i, DET_PROBE700)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEff(SortedBuiltInIndex[i], DET_PROBE700)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 12:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEff(i, DET_WELL700)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEff(SortedBuiltInIndex[i], DET_WELL700)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 13:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEff(i, Mca_installedDetector)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEff(SortedBuiltInIndex[i], Mca_installedDetector)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;


						case 14: // Skew Well
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEffSkewWell(i, Mca_installedDetector)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEffSkewWell(SortedBuiltInIndex[i], Mca_installedDetector)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 15: // Skew Probe
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEffSkewProbe(i, Mca_installedDetector)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEffSkewProbe(SortedBuiltInIndex[i], Mca_installedDetector)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 16: //
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									m_ucNuclideMap[j] = i;
									j++;
								}
							}
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i])){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 17:
							for(i=USERNUC; i<(USERNUC+10); i++){
								if(NuclideData_isShow(i) && (NuclideData_getEffectiveResponse(i, C_CHAMB)!=0.0)){
									m_ucNuclideMap[j] = i;
									j++;
								}
							}
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i]) && (NuclideData_getEffectiveResponse(SortedBuiltInIndex[i], C_CHAMB)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = -1;
							break;

						case 18:
							for(i=USERNUC; i<(USERNUC+10); i++){
								if(NuclideData_isShow(i) && (NuclideData_getEffectiveResponse(i, K_CHAMB)!=0.0)){
									m_ucNuclideMap[j] = i;
									j++;
								}
							}
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i]) && (NuclideData_getEffectiveResponse(SortedBuiltInIndex[i], K_CHAMB)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = -1;
							break;
					}

					m_ucNuclideMapCount = j;

					if(showHotKeys){
						switch(m_ucNuclideConfig){
							case 1:
							case 2:
								hotkeytype = ch_type;
								original = FALSE;
								break;

							case 3:
								hotkeytype = ch_type;
								original = TRUE;
								break;

							case 4:
								hotkeytype = 0;
								original = TRUE;
								break;

							case 5:
								hotkeytype = 1;
								original = TRUE;
								break;

							case 6:
								hotkeytype = -1;
								original = TRUE;
								break;

							case 7:
								hotkeytype = -1;
								original = TRUE;
								break;

							case 8:
								hotkeytype = R_CHAMB;
								original = FALSE;
								break;

							case 9:
								hotkeytype = P_CHAMB;
								original = FALSE;
								break;

							case 10:
								hotkeytype = 0;
								original = FALSE;
								break;

							case 11:
								hotkeytype = 0;
								original = FALSE;
								break;

							case 12:
								hotkeytype = 0;
								original = FALSE;
								break;

							case 13:
								hotkeytype = 0;
								original = FALSE;
								break;

							case 14:
								hotkeytype = 0;
								original = FALSE;
								break;

							case 15:
								hotkeytype = 0;
								original = FALSE;
								break;

							case 16:
								hotkeytype = 0;
								original = FALSE;
								break;

							case 17:
								hotkeytype = C_CHAMB;
								original = FALSE;
								break;

							case 18:
								hotkeytype = K_CHAMB;
								original = FALSE;
								break;
						}

						if(hotkeytype != -1){
							populateNuclideHotKey(hotkeytype, original);
							SetAmuletByte(101, 0xFF);
						}
					}
					m_ucNuclideConfig = 0;
					delayloop(20);
				}
				send_to_amulet_string(98, (char *)m_acTitle);
				SetAmuletByte(99, 0xFF);

				first_map_item_displayed = 0;
				selected_map_item = -1;
				m_iPhase = PHASE_NUCLIDE_REFRESH_SCREEN;
			}else{
				if(m_ucNuclideConfig == 1 || m_ucNuclideConfig == 6 || m_ucNuclideConfig == 7 || m_ucNuclideConfig == 10 || m_ucNuclideConfig == 11 || m_ucNuclideConfig == 12 || m_ucNuclideConfig == 13 || m_ucNuclideConfig == 14 || m_ucNuclideConfig == 15 || m_ucNuclideConfig == 16){
					if(m_ucNuclideClearButton) SetAmuletByte(100, 0xFF);
					j = 0;
					switch(m_ucNuclideConfig){
						case 1:
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i])){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;


						case 6:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEff(i, DET_WELL)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEff(SortedBuiltInIndex[i], DET_WELL)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 7:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if(NuclideData_getPrimary(i) != 0.0){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 10:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEff(i, DET_BETA)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEff(SortedBuiltInIndex[i], DET_BETA)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 11:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEff(i, DET_PROBE700)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEff(SortedBuiltInIndex[i], DET_PROBE700)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 12:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEff(i, DET_WELL700)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEff(SortedBuiltInIndex[i], DET_WELL700)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 13:
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEff(i, Mca_installedDetector)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEff(SortedBuiltInIndex[i], Mca_installedDetector)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 14: // Skew Well
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEffSkewWell(i, Mca_installedDetector)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEffSkewWell(SortedBuiltInIndex[i], Mca_installedDetector)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 15: // Skew Probe
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									if((NuclideData_getPrimary(i)!=0.0) && (NuclideData_getEffSkewProbe(i, Mca_installedDetector)!=0.0)){
										m_ucNuclideMap[j] = i;
										j++;
									}
								}
							}

							for(i=0; i<BUILTINNUC; i++){
								if((NuclideData_getEffectivePrimary(SortedBuiltInIndex[i])!=0.0) && (NuclideData_getEffectiveEffSkewProbe(SortedBuiltInIndex[i], Mca_installedDetector)!=0.0)){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;

						case 16: //
							for(i=USERNUC+10; i<USERNUC+20; i++){
								if(NuclideData_getHalflifeUnit(i) != -1){
									m_ucNuclideMap[j] = i;
									j++;
								}
							}
							for(i=0; i<BUILTINNUC; i++){
								if(NuclideData_isShow(SortedBuiltInIndex[i])){
									m_ucNuclideMap[j] = SortedBuiltInIndex[i];
									j++;
								}
							}
							tagbothusercalnum = 0;
							showHotKeys = FALSE;
							break;
					}

					m_ucNuclideMapCount = j;
					m_ucNuclideConfig = 0;

					if(showHotKeys) SetAmuletByte(101, 0xFF);
					delayloop(10);
					send_to_amulet_string(98, (char *)m_acTitle);
					SetAmuletByte(99, 0xFF);
					delayloop(10);
					first_map_item_displayed = 0;
					selected_map_item = -1;
					m_iPhase = PHASE_NUCLIDE_REFRESH_SCREEN;
				}
			}
			break;

		case PHASE_NUCLIDE_REFRESH_SCREEN:
			display_nuclide(&first_map_item_displayed, &selected_map_item, tagbothusercalnum);
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_WAIT:
			break;

		case PHASE_NUCLIDE_PAGE_DOWN:
			i = first_map_item_displayed;
			i += ITEMS_PER_PAGE;
			if(i > (MAX_NUCLIDE - 1)){
				i = (MAX_NUCLIDE - 1) / ITEMS_PER_PAGE;
				i *= ITEMS_PER_PAGE;
			}
			first_map_item_displayed = i;
			m_iPhase = PHASE_NUCLIDE_REFRESH_SCREEN;
			break;

		case PHASE_NUCLIDE_PAGE_UP:
			i = first_map_item_displayed;
			i -= ITEMS_PER_PAGE;
			if (i < 0) i = 0;
			first_map_item_displayed = i;
			m_iPhase = PHASE_NUCLIDE_REFRESH_SCREEN;
			break;

		case PHASE_NUCLIDE_ROW1:
			i = 0;
			if(selected_map_item == -1){
				SetAmuletByte(240, 0xFF);
			}
			selected_map_item = first_map_item_displayed + i;
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_ROW2:
			i = 1;
			if(selected_map_item == -1){
				SetAmuletByte(240, 0xFF);
			}
			selected_map_item = first_map_item_displayed + i;
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_ROW3:
			i = 2;
			if (selected_map_item == -1) {
				SetAmuletByte(240, 0xFF);
			}
			selected_map_item = first_map_item_displayed + i;
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_ROW4:
			i = 3;
			if (selected_map_item == -1) {
				SetAmuletByte(240, 0xFF);
			}
			selected_map_item = first_map_item_displayed + i;
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_ROW5:
			i = 4;
			if (selected_map_item == -1) {
				SetAmuletByte(240, 0xFF);
			}
			selected_map_item = first_map_item_displayed + i;
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_ROW6:
			i = 5;
			if (selected_map_item == -1) {
				SetAmuletByte(240, 0xFF);
			}
			selected_map_item = first_map_item_displayed + i;
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_ROW7:
			i = 6;
			if (selected_map_item == -1) {
				SetAmuletByte(240, 0xFF);
			}
			selected_map_item = first_map_item_displayed + i;
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_ROW8:
			i = 7;
			if (selected_map_item == -1) {
				SetAmuletByte(240, 0xFF);
			}
			selected_map_item = first_map_item_displayed + i;
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_ROW9:
			i = 8;
			if (selected_map_item == -1) {
				SetAmuletByte(240, 0xFF);
			}
			selected_map_item = first_map_item_displayed + i;
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_ROW10:
			i = 9;
			if (selected_map_item == -1) {
				SetAmuletByte(240, 0xFF);
			}
			selected_map_item = first_map_item_displayed + i;
			m_iPhase = PHASE_NUCLIDE_WAIT;
			break;

		case PHASE_NUCLIDE_B1_1:
		case PHASE_NUCLIDE_B1_2:
		case PHASE_NUCLIDE_B1_3:
		case PHASE_NUCLIDE_B1_4:
		case PHASE_NUCLIDE_B2_1:
		case PHASE_NUCLIDE_B2_2:
		case PHASE_NUCLIDE_B2_3:
		case PHASE_NUCLIDE_B2_4:
		case PHASE_NUCLIDE_B3_1:
		case PHASE_NUCLIDE_B3_2:
		case PHASE_NUCLIDE_B3_3:
		case PHASE_NUCLIDE_B3_4:
		case PHASE_NUCLIDE_B4_1:
		case PHASE_NUCLIDE_B4_2:
		case PHASE_NUCLIDE_B4_3:
		case PHASE_NUCLIDE_B4_4:
		case PHASE_NUCLIDE_B5_1:
		case PHASE_NUCLIDE_B5_2:
		case PHASE_NUCLIDE_B5_3:
		case PHASE_NUCLIDE_B5_4:
			if((hotkeytype == R_CHAMB )||(hotkeytype == P_CHAMB)||(hotkeytype == C_CHAMB)||(hotkeytype == K_CHAMB)){
				uctempIndex = m_iPhase - PHASE_NUCLIDE_B1_1;
				if(m_ucHotKeyNuclideID2[hotkeytype][uctempIndex]!=255){
					if(original){
						if((m_ucHotKeyNuclideID2[hotkeytype][uctempIndex] < USERNUC) || ((m_ucHotKeyNuclideID2[hotkeytype][uctempIndex] >= USERNUC+20) && (m_ucHotKeyNuclideID2[hotkeytype][uctempIndex] < ALLNUC))){
							if(NuclideData_getEffectiveResponse(m_ucHotKeyNuclideID2[hotkeytype][uctempIndex], hotkeytype) != 0.0){
								selected_map_item = -2;
								m_ucCurrentNuclide = m_ucHotKeyNuclideID2[hotkeytype][uctempIndex];
								m_iPhase = PHASE_NUCLIDE_SELECT;
							}else{
								m_iPhase = PHASE_NUCLIDE_WAIT;
							}
						}else{
							m_iPhase = PHASE_NUCLIDE_WAIT;
						}
					}else{
						if(NuclideData_getEffectiveResponse(m_ucHotKeyNuclideID2[hotkeytype][uctempIndex], hotkeytype) != 0.0){
							selected_map_item = -2;
							m_ucCurrentNuclide = m_ucHotKeyNuclideID2[hotkeytype][uctempIndex];
							m_iPhase = PHASE_NUCLIDE_SELECT;
						}else{
							m_iPhase = PHASE_NUCLIDE_WAIT;
						}
					}
				}else{
					m_iPhase = PHASE_NUCLIDE_WAIT;
				}
			}else{
				m_iPhase = PHASE_NUCLIDE_WAIT;
			}
			//selected_map_item = -2;
			//m_ucCurrentNuclide = 30;
			//m_iPhase = PHASE_NUCLIDE_SELECT;
			break;

		case PHASE_NUCLIDE_SELECT:
			if(selected_map_item != -2) m_ucCurrentNuclide = m_ucNuclideMap[selected_map_item];

			switch(m_ucSetNuclide){
				case 0:
					m_iPhase = PHASE_NUCLIDE_WAIT;
					break;

				case 1:
					chamber[ch_num].nuc_index = m_ucCurrentNuclide;
					set_nuclide_data(m_ucCurrentNuclide, ch_num);
					strcpy(nucshrink, chamber[ch_num].nucdata.name);
					trim_and_shrink(nucshrink);
					if(DisplayActivity_SendActivity){
						if(nucshrink[0]!=0){
							GetExtendedNuclideString(chamber[ch_num].nuc_index, message);
							send_to_amulet_string(0, message);
						}
						else send_to_amulet_string(0, "");
						//else send_to_amulet_string(0, "*****");
					}
					EE_WRITE(nuclideID[ch_num], (uchar *) &(chamber[ch_num].nuc_index));
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 2:
					m_ucAddInventory_Nuclide = m_ucCurrentNuclide;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 3:
					m_ucInfo_DecayNuclide = m_ucCurrentNuclide;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 4:
					/*beep_amulet();
					if(NuclideData_getEffectiveResponse(m_ucCurrentNuclide, chamber_type(ch_num)) != 0.0){
						chamber[ch_num].nuc_index = m_ucCurrentNuclide;
						set_nuclide_data(m_ucCurrentNuclide, ch_num);
						strcpy(nucshrink, chamber[ch_num].nucdata.name);
						trim_and_shrink(nucshrink);
						if(DisplayActivity_SendActivity){
							if(nucshrink[0]!=0){
								GetExtendedNuclideString(chamber[ch_num].nuc_index, message);
								send_to_amulet_string(0, message);
							}
							else send_to_amulet_string(0, "*****");
						}
						EE_WRITE(nuclideID[ch_num], (uchar *) &(chamber[ch_num].nuc_index));
						m_ucHotKeyNuclideID[chamber_type(ch_num)][m_ucLastHotKeyIndex - 1] = m_ucCurrentNuclide;
						write_HotKeys();
					}

					PopPageStack();
					SetAmuletHTML(CurrentPageStack());
					m_ucSetNuclide = 0;*/
					break;

				case 5:
					AmuletSetupCalNumMenu_NuclideID = m_ucCurrentNuclide;
					AmuletSetupCalNumMenu_UserRCal[0] = 0;
					AmuletSetupCalNumMenu_UserPCal[0] = 0;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
					if(m_ucCurrentNuclide == 0xFF) AmuletSetupRemoteMenu_mirrorRemoteNuc[m_ucSetNuclide - 6].keyiso[0].iso_name[0] = 0;
					else NuclideData_getName(m_ucCurrentNuclide, AmuletSetupRemoteMenu_mirrorRemoteNuc[m_ucSetNuclide - 6].keyiso[0].iso_name);
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 14:
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
					if(m_ucCurrentNuclide==0xFF) AmuletSetupRemoteMenu_mirrorRemoteNuc[m_ucSetNuclide - 14].keyiso[1].iso_name[0] = 0;
					else NuclideData_getName(m_ucCurrentNuclide, AmuletSetupRemoteMenu_mirrorRemoteNuc[m_ucSetNuclide - 14].keyiso[1].iso_name);
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 22:
					AmuletDoseTableMenu_nuclideID = m_ucCurrentNuclide;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 23:
				case 24:
				case 25:
				case 26:
				case 27:
				case 28:
				case 29:
				case 30:
				case 31:
				case 32:
				case 33:
				case 34:
					if(m_ucCurrentNuclide == 0xFF) AmuletSetupSourcesMenu_constancyCh[m_ucSetNuclide - 23] = -1;
					else AmuletSetupSourcesMenu_constancyCh[m_ucSetNuclide - 23] = m_ucCurrentNuclide;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 35:
					AmuletSetupLinearityStandardMenu_nuclideID = m_ucCurrentNuclide;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 36:
				case 37:
				case 38:
				case 39:
				case 40:
				case 41:
				case 42:
				case 43:
				case 44:
				case 45:
					if(m_ucCurrentNuclide == 0xFF){
						AmuletWellSetupTypes_wellWipeType.NuclideID[m_ucSetNuclide - 36] = -1;
					}else{
						AmuletWellSetupTypes_wellWipeType.NuclideID[m_ucSetNuclide - 36] = m_ucCurrentNuclide;
					}
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 46:
				case 47:
				case 48:
				case 49:
				case 50:
				case 51:
				case 52:
				case 53:
				case 54:
				case 55:
					if(m_ucCurrentNuclide == 0xFF){
						AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[m_ucSetNuclide - 46] = -1;
					}else{
						AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[m_ucSetNuclide - 46] = m_ucCurrentNuclide;
					}
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 56:
				case 57:
				case 58:
				case 59:
				case 60:
				case 61:
				case 62:
				case 63:
				case 64:
				case 65:
				case 66:
				case 67:
				case 68:
				case 69:
				case 70:
				case 71:
				case 72:
				case 73:
				case 74:
				case 75:
				case 76:
				case 77:
				case 78:
				case 79:
				case 80:
				case 81:
				case 82:
				case 83:
				case 84:
				case 85:
				case 86:
				case 87:
				case 88:
				case 89:
				case 90:
				case 91:
				case 92:
				case 93:
				case 94:
				case 95:
				case 96:
				case 97:
				case 98:
				case 99:
				case 100:
				case 101:
				case 102:
				case 103:
				case 104:
				case 105:
				case 106:
				case 107:
				case 108:
				case 109:
				case 110:
				case 111:
				case 112:
				case 113:
				case 114:
				case 115:
				case 116:
				case 117:
				case 118:
				case 119:
				case 120:
				case 121:
				case 122:
				case 123:
				case 124:
				case 125:
				case 126:
				case 127:
				case 128:
				case 129:
				case 130:
				case 131:
				case 132:
				case 133:
				case 134:
				case 135:
				case 136:
				case 137:
				case 138:
				case 139:
				case 140:
				case 141:
				case 142:
				case 143:
				case 144:
				case 145:
				case 146:
				case 147:
				case 148:
				case 149:
				case 150:
				case 151:
				case 152:
				case 153:
				case 154:
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 155:
					AmuletWellMDATestMenu_nuclideID = m_ucCurrentNuclide;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 156:
					if(m_ucCurrentNuclide == 0xFF){
						generalNuclideID = -1;
						startEV[0] = -1.0;
						endEV[0] = -1.0;
					}else{
						generalNuclideID = m_ucCurrentNuclide;
						if(m_ucSetWellMeasure == 1){
							if(Mca_installedDetector == DET_DRILLEDPROBE700){
								if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_WELL) Mca_getEndPoints(NuclideData_getEffectivePrimary(generalNuclideID), &(startEV[0]), &(endEV[0]), FALSE);
								else if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_PROBE) Mca_getEndPoints(NuclideData_getEffectivePrimary(generalNuclideID), &(startEV[0]), &(endEV[0]), TRUE);
								else Mca_getEndPoints(NuclideData_getEffectivePrimary(generalNuclideID), &(startEV[0]), &(endEV[0]), FALSE);
							}else{
								Mca_getEndPoints(NuclideData_getEffectivePrimary(generalNuclideID), &(startEV[0]), &(endEV[0]), FALSE);
							}
						}else{
							Mca_getEndPoints(NuclideData_getEffectivePrimary(generalNuclideID), &(startEV[0]), &(endEV[0]), FALSE);
						}
					}
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 157:
				case 158:
				case 159:
				case 160:
				case 161:
				case 162:
				case 163:
				case 164:
					AmuletSetupRHotkeys_M[m_ucSetNuclide-157] = m_ucCurrentNuclide;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 165:
				case 166:
				case 167:
				case 168:
				case 169:
				case 170:
				case 171:
				case 172:
				case 173:
				case 174:
				case 175:
				case 176:
				case 177:
				case 178:
				case 179:
				case 180:
				case 181:
				case 182:
				case 183:
				case 184:
					AmuletSetupRHotkeys_N[m_ucSetNuclide-165] = m_ucCurrentNuclide;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 185:
				case 186:
				case 187:
				case 188:
				case 189:
				case 190:
				case 191:
				case 192:
					AmuletSetupPHotkeys_M[m_ucSetNuclide-185] = m_ucCurrentNuclide;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 193:
				case 194:
				case 195:
				case 196:
				case 197:
				case 198:
				case 199:
				case 200:
				case 201:
				case 202:
				case 203:
				case 204:
				case 205:
				case 206:
				case 207:
				case 208:
				case 209:
				case 210:
				case 211:
				case 212:
					AmuletSetupPHotkeys_N[m_ucSetNuclide-193] = m_ucCurrentNuclide;
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 213:
					NuclideData_getName(m_ucCurrentNuclide, AmuletAutoLinearityTest_nuclideName);
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

				case 214:
					if(m_ucSetNuclide == 0xFF){
						HalflifeCalc_test.NuclideName[0] = 0;
					}else{
						NuclideData_getName(m_ucCurrentNuclide, HalflifeCalc_test.NuclideName);
					}
					SetAmuletBackHTML();
					m_ucSetNuclide = 0;
					break;

			}
			break;

		case PHASE_NUCLIDE_SUBMIT_BLANK:
			selected_map_item = -2;
			m_ucCurrentNuclide = 0xFF;
			m_iPhase = PHASE_NUCLIDE_SELECT;
			break;
	}
}

void display_nuclide(short *first_map_item_displayed, short *selected_map_item, char tagbothusercalnum) {
	short last_map_item, last_displayed;
	short i, j;
	short row;
	char nuclide[10];
	char name[15];
	char acMsg[25], formatstring[25];
	float halflife;
	short hlunit;
	short item_count;
	short index_end;
	NUCDATA nuc;
	short iAmuletGridAddress;
	//unsigned long int temp_msec;
	char page_down_visible;
	char page_up_visible;
	short chambertype;

	if(current.num_chambers>0){
		chambertype = chamber_type(current.main_chamber);
	}else{
		chambertype = R_CHAMB;
	}
	last_map_item = m_ucNuclideMapCount - 1;

	item_count = ITEMS_PER_PAGE;
	last_displayed = last_map_item / item_count;
	last_displayed *= item_count;

	if (*first_map_item_displayed > last_map_item) *first_map_item_displayed = last_displayed;
	index_end = *first_map_item_displayed + (ITEMS_PER_PAGE - 1);
	if (index_end > last_map_item) index_end = last_map_item;

	row = 0;
	for (i=*first_map_item_displayed; i <= index_end; i++) {
		NuclideData_getNuclide(m_ucNuclideMap[i], &nuc);
		if(tagbothusercalnum){
			if((NuclideData_getUserResponse(m_ucNuclideMap[i], R_CHAMB) != 0.0) || (NuclideData_getUserResponse(m_ucNuclideMap[i], P_CHAMB) != 0.0)) strcpy(nuclide, "*");
			else strcpy(nuclide, "");
		}else{
			if(NuclideData_getUserResponse(m_ucNuclideMap[i], chambertype) != 0.0) strcpy(nuclide, "*");
			else strcpy(nuclide, "");
		}

		strncat(nuclide, nuc.name, 7);
		trim(nuclide);
		halflife = nuc.halflife;
		hlunit = nuc.hlunit;
		NuclideData_getFullName(m_ucNuclideMap[i], name);
		sprintf (acMsg, "%.2f", halflife);
		switch (hlunit) {
			case SEC:
				strcat(acMsg, " s");
				break;

			case MIN:
				strcat(acMsg, " m");
				break;

			case HOUR:
				strcat(acMsg, " hr");
				break;

			case DAY:
				strcat(acMsg, " dy");
				break;

			case YEAR:
				if(halflife > 1.0e+6){
					sprintf (acMsg, "%.2f", halflife / 1.0e+6);
					strcat(acMsg, " Myr");
				}else{
					strcat(acMsg, " yr");
				}
				break;
		}

		iAmuletGridAddress = (3 * row) + 100;

		send_to_amulet_string(iAmuletGridAddress, nuclide);
		send_to_amulet_string(iAmuletGridAddress + 1, name);
		send_to_amulet_string(iAmuletGridAddress + 2, acMsg);

		SetAmuletByte (row + 244, 0xFF);

		delayloop(5);
		row++;
	}


	if (row < ITEMS_PER_PAGE) {
		acMsg[0] = 0;
		for (i=row; i<ITEMS_PER_PAGE; i++) {
			iAmuletGridAddress = (3 * i) + 100;
			send_to_amulet_string(iAmuletGridAddress, acMsg);
			send_to_amulet_string(iAmuletGridAddress + 1, acMsg);
			send_to_amulet_string(iAmuletGridAddress + 2, acMsg);
			SetAmuletByte(i+244, 0);

			delayloop(2);
		}
	}

	page_up_visible = 1;
	page_down_visible = 1;

	if (*first_map_item_displayed == 0) page_up_visible = 0;
	if (*first_map_item_displayed >= last_displayed) page_down_visible = 0;

	if (page_up_visible == 1) SetAmuletByte(242, 0xFF);
	if (page_down_visible == 1) SetAmuletByte(241, 0xFF);

	i = (*first_map_item_displayed) / ITEMS_PER_PAGE;
	i++;

	j = last_map_item / ITEMS_PER_PAGE;
	j++;

	delayloop(10);
	get_amulet_message(L_PAGE_OF, formatstring);    // "Page %d of %d"
	sprintf(acMsg, formatstring, i, j);
	send_to_amulet_string(130, acMsg);
	SetAmuletByte(243, 0xFF);

	*selected_map_item = -1;
}

void populateNuclideHotKey(char hotkeytype, bool original){
	short index;
	char nuclidename[7];
	char message[26];

	for(index=0; index<20; index++){
		if(m_ucHotKeyNuclideID2[hotkeytype][index] != 255){
			if(original){
				if((m_ucHotKeyNuclideID2[hotkeytype][index] < USERNUC) || ((m_ucHotKeyNuclideID2[hotkeytype][index] >= USERNUC+20) && (m_ucHotKeyNuclideID2[hotkeytype][index] < ALLNUC))){
					NuclideData_getName(m_ucHotKeyNuclideID2[hotkeytype][index], nuclidename);
					if(NuclideData_getEffectiveResponse(m_ucHotKeyNuclideID2[hotkeytype][index], hotkeytype) != 0.0){
						splitNuclide(nuclidename, message, FALSE);
					}else{
						splitNuclide(nuclidename, message, TRUE);
					}
					send_to_amulet_string(70+index, message);
					delayloop(5);
				}
			}else
			{
				NuclideData_getName(m_ucHotKeyNuclideID2[hotkeytype][index], nuclidename);
				if(NuclideData_getEffectiveResponse(m_ucHotKeyNuclideID2[hotkeytype][index], hotkeytype)!=0.0){
					splitNuclide(nuclidename, message, FALSE);
				}else{
					splitNuclide(nuclidename, message, TRUE);
				}
				send_to_amulet_string(70+index, message);
				delayloop(5);
			}
		}
	}
}

void splitNuclide(char *input, char *output, bool para){
	char letter[3];
	char number[10];

	letter[0] = *input++; //0
	letter[1] = *input++; //1
	letter[2] = 0;

	if(letter[1] == 32) letter[1] = 0;

	number[0] = *input++; //2
	number[1] = *input++; //3
	number[2] = *input++; //4
	number[3] = *input++; //5
	number[4] = 0;

	trim_and_shrink(number);
	if(para){
		sprintf(output, "(%s)\n(%s)", letter, number);
	}else{
		sprintf(output, "%s\n%s", letter, number);
	}
}

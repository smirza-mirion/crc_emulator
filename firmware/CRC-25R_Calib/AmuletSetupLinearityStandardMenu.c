/**
 * \file
 * \details This file handles calls from the Amulet Standard Linearity Setup Screen
 */
#define PHASE_SETUP_LINEARITY_STANDARD_PRE_INIT	0
#define PHASE_SETUP_LINEARITY_STANDARD_WAIT		1
#define PHASE_SETUP_LINEARITY_STANDARD_SAVE		2
#define PHASE_SETUP_LINEARITY_STANDARD_NUCLIDE  3
#define PHASE_SETUP_LINEARITY_STANDARD_CLEAR    4

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "linearity.h"
#include "i2c.h"
#include "nuc.h"
#include "chambfac.h"
#include "amulet.h"
#include "message.h"

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void GetShortNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
unsigned char CurrentPageStack(void);
void PushPageStack(unsigned char ucPage);
void SetAmuletBackHTML(void);

static LINDEF linearityMirror;

short AmuletSetupLinearityStandardMenu_chamberType;
char AmuletSetupLinearityStandardMenu_testTotal;
short AmuletSetupLinearityStandardMenu_testHour[12];
short AmuletSetupLinearityStandardMenu_nuclideID;

extern int m_iPhase;
extern unsigned char m_ucClear;
extern volatile char m_acTitle[51];
extern uchar m_ucNuclideConfig;
extern uchar m_ucNuclideClearButton;
extern unsigned char m_ucSetNuclide;

void send_accept_cancel(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);

/**
 * \details Handles the Amulet Screen SetupLinearityStandard.htm. SetupLinearityStandard.htm setups the standard linearity test.
 * \param Amulet_Byte_ID Description
 * \param 100 Refresh Display
 * \param 101 Display Test1
 * \param 102 Display Test2
 * \param 103 Display Test3
 * \param 104 Display Test4
 * \param 105 Display Test5
 * \param 106 Display Test6
 * \param 107 Display Test7
 * \param 108 Display Test8
 * \param 109 Display Test9
 * \param 110 Display Test10
 * \param 111 Display Test11
 * \param 112 Display Test12
 * \param Amuelt_String_ID Description
 * \param 100 Num of Tests
 * \param 101 Test 1 Time
 * \param 102 Test 2 Time
 * \param 103 Test 3 Time
 * \param 104 Test 4 Time
 * \param 105 Test 5 Time
 * \param 106 Test 6 Time
 * \param 107 Test 7 Time
 * \param 108 Test 8 Time
 * \param 109 Test 9 Time
 * \param 110 Test 10 Time
 * \param 111 Test 11 Time
 * \param 112 Test 12 Time
 * \param 113_114 Nuclide
 * \param 115_116 Title
 * \param 117 Num of Measurements
 * \param 118 Nuclide
 * \param 119 Measured On
 * \param 120 Clear Button
 * \param 121 hr
 * \param 122_123 Please Select Nuclide
 * \param 124_125 Enter # of Measurements
 * \param 126_127 Enter Test 2 Time
 * \param 128_129 Enter Test 3 Time
 * \param 130_131 Enter Test 4 Time
 * \param 132_133 Enter Test 5 Time
 * \param 134_135 Enter Test 6 Time
 * \param 136_137 Enter Test 7 Time
 * \param 138_139 Enter Test 8 Time
 * \param 140_141 Enter Test 9 Time
 * \param 141_143 Enter Test 10 Time
 * \param 144_145 Enter Test 11 Time
 * \param 146_147 Enter Test 12 Time
 * \returns None
 */
void AmuletSetupLinearityStandard_menu(void){
	char index;
	bool hoursfilled, hoursgood;
	char message[101];
	char message1[101];
	short lastvalue;
	NUCDATA nuc;
	short ch_type_mod;

	switch(m_iPhase){
		case PHASE_SETUP_LINEARITY_STANDARD_PRE_INIT:
			if(m_ucClear == 12){
				EE_READ(lindef, (uchar *) &linearityMirror);

				ch_type_mod = chamber_type_77t_remap(AmuletSetupLinearityStandardMenu_chamberType); // C_CHAMB and K_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded

				AmuletSetupLinearityStandardMenu_nuclideID = linearityMirror.nuc_index_Std[ch_type_mod];

				AmuletSetupLinearityStandardMenu_testTotal = linearityMirror.num_Std[ch_type_mod];

				if(AmuletSetupLinearityStandardMenu_testTotal < 5 || AmuletSetupLinearityStandardMenu_testTotal > 12) AmuletSetupLinearityStandardMenu_testTotal = -1;

				if(AmuletSetupLinearityStandardMenu_testTotal == -1) for(index=0; index<12; index++) AmuletSetupLinearityStandardMenu_testHour[index] = -1;
				else{
					for(index=0; index<12; index++){
						if((linearityMirror.hours_Std[ch_type_mod][index] > 0) && linearityMirror.hours_Std[ch_type_mod][index] < 1000) AmuletSetupLinearityStandardMenu_testHour[index] = linearityMirror.hours_Std[ch_type_mod][index];
						else AmuletSetupLinearityStandardMenu_testHour[index] = -1;
					}
				}

				m_ucClear = 0;
			}

			if(AmuletSetupLinearityStandardMenu_chamberType == R_CHAMB){
				//SetAmuletString(115, "Setup Linearity Standard - R Chamber");
				send_amulet_message(L_SETUP_LINEARITY_STANDARD_R,115);    // "Setup Linearity Standard - R Chamber"
			}else if(AmuletSetupLinearityStandardMenu_chamberType == P_CHAMB){
				//SetAmuletString(115, "Setup Linearity Standard - PET Chamber");
				send_amulet_message(L_SETUP_LINEARITY_STANDARD_PET,115);    // "Setup Linearity Standard - PET Chamber"
			}else{
				send_amulet_message(L_SETUP_LINEARITY_STANDARD,115);    // "Setup Linearity Standard"
			}


			send_amulet_message(L_NUM_OF_MEASUREMENTS,117);    // "Num of Measurements:"

			get_amulet_message(L_NUCLIDE,message);    // "Nuclide"
			strcat(message,":");
			send_to_amulet_string(118,message);

			send_amulet_message(L_MEASURED_ON,119);    // "Measured On"
			send_amulet_message(L_CLEAR_BUTTON,120);    // "Clear"
			send_amulet_message(L_HR3,121);    // "hr"

			send_amulet_message(L_ENTER_NUM_MEASUREMENTS,124);    // "Please Enter # of Measurements:"
			send_amulet_message(L_PLEASE_SELECT_NUCLIDE,122);    // "Please Select Nuclide"

			send_amulet_message(L_ENTER_TEST_2_TIME,126);    // "Enter Test 2 Time:"
			send_amulet_message(L_ENTER_TEST_3_TIME,128);    // "Enter Test 3 Time:"
			send_amulet_message(L_ENTER_TEST_4_TIME,130);    // "Enter Test 4 Time:"
			send_amulet_message(L_ENTER_TEST_5_TIME,132);    // "Enter Test 5 Time:"
			send_amulet_message(L_ENTER_TEST_6_TIME,134);    // "Enter Test 6 Time:"
			send_amulet_message(L_ENTER_TEST_7_TIME,136);    // "Enter Test 7 Time:"
			send_amulet_message(L_ENTER_TEST_8_TIME,138);    // "Enter Test 8 Time:"
			send_amulet_message(L_ENTER_TEST_9_TIME,140);    // "Enter Test 9 Time:"
			send_amulet_message(L_ENTER_TEST_10_TIME,142);    // "Enter Test 10 Time:"
			send_amulet_message(L_ENTER_TEST_11_TIME,144);    // "Enter Test 11 Time:"
			send_amulet_message(L_ENTER_TEST_12_TIME,146);    // "Enter Test 12 Time:"

			send_accept_cancel();

			if(NuclideData_getEffectiveResponse(AmuletSetupLinearityStandardMenu_nuclideID, AmuletSetupLinearityStandardMenu_chamberType) == 0.0){
				if(AmuletSetupLinearityStandardMenu_chamberType == P_CHAMB){
					AmuletSetupLinearityStandardMenu_nuclideID = NuclideData_getIndexFromName("F 18");
				}else{
					AmuletSetupLinearityStandardMenu_nuclideID = NuclideData_getIndexFromName("Tc99m");
				}
			}
			GetShortNuclideInfo(AmuletSetupLinearityStandardMenu_nuclideID, &nuc, message);
			send_to_amulet_string(113, message);

			if(AmuletSetupLinearityStandardMenu_testTotal >= 0) {
				sprintf(message, "%d", AmuletSetupLinearityStandardMenu_testTotal);
				send_to_amulet_string(100, message);
				SetAmuletByte(100, 0xFF);

				send_to_amulet_string(101, "0");
				AmuletSetupLinearityStandardMenu_testHour[0] = 0;
				SetAmuletByte(101, 0xFF);
				delayloop(2);
				for(index=1; index<AmuletSetupLinearityStandardMenu_testTotal; index++){
					if(AmuletSetupLinearityStandardMenu_testHour[index] >= 0){
						sprintf(message, "%d", AmuletSetupLinearityStandardMenu_testHour[index]);
						send_to_amulet_string(101 + index, message);
					}
					SetAmuletByte(101+index, 0xFF);
					delayloop(2);
				}
			}else{
				SetAmuletByte(100, 0xFF);
			}

			m_iPhase = PHASE_SETUP_LINEARITY_STANDARD_WAIT;
			break;

		case PHASE_SETUP_LINEARITY_STANDARD_WAIT:
			break;

		case PHASE_SETUP_LINEARITY_STANDARD_SAVE:
			beep_amulet();
			if(AmuletSetupLinearityStandardMenu_testTotal == -1){
				ch_type_mod = chamber_type_77t_remap(AmuletSetupLinearityStandardMenu_chamberType); // C_CHAMB and K_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded
				linearityMirror.num_Std[ch_type_mod] = -1;
				EE_WRITE(lindef.num_Std[ch_type_mod], (uchar *) &linearityMirror.num_Std[ch_type_mod]);
				linearityMirror.nuc_index_Std[ch_type_mod] = AmuletSetupLinearityStandardMenu_nuclideID;
				EE_WRITE(lindef.nuc_index_Std[ch_type_mod], (uchar *) &linearityMirror.nuc_index_Std[ch_type_mod]);
				for(index=0; index<12;index++){
					linearityMirror.hours_Std[ch_type_mod][index] = -1;
					EE_WRITE(lindef.hours_Std[ch_type_mod][index], (uchar *)&linearityMirror.hours_Std[ch_type_mod][index]);
				}
				SetAmuletBackHTML();
				return;
			}else{
				hoursfilled = TRUE;
				for(index=0; index<AmuletSetupLinearityStandardMenu_testTotal; index++){
					if(AmuletSetupLinearityStandardMenu_testHour[index]==-1){
						hoursfilled = FALSE;
						break;
					}
				}

				if(!hoursfilled){
					//Amulet_DisplayError("Setup Linearity Error", "Please fill in all hours",TRUE);
					get_amulet_message(L_SETUP_LINEARITY_ERROR,message);    // "Setup Linearity Error"
					get_amulet_message(L_FILL_IN_HOURS,message1);    // "Please Fill in All Hours"
					Amulet_DisplayError(message,message1,TRUE);
					return;
				}else{
					hoursgood = TRUE;
					lastvalue = 0;
					for(index=1; index<AmuletSetupLinearityStandardMenu_testTotal; index++){
						if(lastvalue < AmuletSetupLinearityStandardMenu_testHour[index]) lastvalue = AmuletSetupLinearityStandardMenu_testHour[index];
						else{
							hoursgood = FALSE;
							break;
						}
					}

					if(!hoursgood){
						//Amulet_DisplayError("Setup Linearity Error", "Please enter hours in ascending order",TRUE);
						get_amulet_message(L_SETUP_LINEARITY_ERROR,message);    // "Setup Linearity Error"
						get_amulet_message(L_ASCENDING_ORDER,message1);    // "Please enter hours in ascending order"
						Amulet_DisplayError(message,message1,TRUE);
						return;
					}else{
						ch_type_mod = chamber_type_77t_remap(AmuletSetupLinearityStandardMenu_chamberType); // C_CHAMB and K_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded
						linearityMirror.num_Std[ch_type_mod] = AmuletSetupLinearityStandardMenu_testTotal;
						EE_WRITE(lindef.num_Std[ch_type_mod], (uchar *) &linearityMirror.num_Std[ch_type_mod]);
						linearityMirror.nuc_index_Std[ch_type_mod] = AmuletSetupLinearityStandardMenu_nuclideID;
						EE_WRITE(lindef.nuc_index_Std[ch_type_mod], (uchar *) &linearityMirror.nuc_index_Std[ch_type_mod]);
						for(index=0; index<AmuletSetupLinearityStandardMenu_testTotal; index++){
							linearityMirror.hours_Std[ch_type_mod][index] = AmuletSetupLinearityStandardMenu_testHour[index];
							EE_WRITE(lindef.hours_Std[ch_type_mod][index], (uchar *)&linearityMirror.hours_Std[ch_type_mod][index]);
						}
						if(AmuletSetupLinearityStandardMenu_testTotal < 12){
							for(index=AmuletSetupLinearityStandardMenu_testTotal; index<12;index++){
								linearityMirror.hours_Std[ch_type_mod][index] = -1;
								EE_WRITE(lindef.hours_Std[ch_type_mod][index], (uchar *)&linearityMirror.hours_Std[ch_type_mod][index]);
							}
						}
						SetAmuletBackHTML();
						return;
					}
				}
			}
			//break;

		case PHASE_SETUP_LINEARITY_STANDARD_NUCLIDE:
			if(CurrentPageStack() != AmuletHTMLIndex[NUCLIDE2_HTM]){
				//strcpy((char *)m_acTitle, "Please Select Nuclide");
				get_amulet_message(L_PLEASE_SELECT_NUCLIDE, (char *) m_acTitle);    // "Please Select Nuclide"
				if((AmuletSetupLinearityStandardMenu_chamberType == R_CHAMB) || (AmuletSetupLinearityStandardMenu_chamberType == C_CHAMB) || (AmuletSetupLinearityStandardMenu_chamberType == K_CHAMB)) m_ucNuclideConfig = 8;
				else m_ucNuclideConfig = 9;
				m_ucNuclideClearButton = 0;
				m_ucSetNuclide = 35;
				SetAmuletHTML(AmuletHTMLIndex[NUCLIDE2_HTM]);
				PushPageStack(AmuletHTMLIndex[NUCLIDE2_HTM]);
				beep_amulet();
				return;
			}
			break;

		case PHASE_SETUP_LINEARITY_STANDARD_CLEAR:
			beep_amulet();
			AmuletSetupLinearityStandardMenu_testTotal = -1;
			send_to_amulet_string(100, "");
			delayloop(2);
			for(index=0;index<12;index++){
				AmuletSetupLinearityStandardMenu_testHour[index] = -1;
				send_to_amulet_string(101+index, "");
				delayloop(2);
			}
			SetAmuletByte(113, 0xFF);
			m_iPhase = PHASE_SETUP_LINEARITY_STANDARD_WAIT;
			break;
	}
}

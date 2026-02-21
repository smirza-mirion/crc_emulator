/**
 * \file
 * \details This file handles calls from the Amulet Cal Number Setup Screen
 */
#define PHASE_SETUP_CAL_NUM_PRE_INIT	0
#define PHASE_SETUP_CAL_NUM_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "nuc.h"
#include "message.h"

short AmuletSetupCalNumMenu_NuclideID;
char AmuletSetupCalNumMenu_UserRCal[10];
char AmuletSetupCalNumMenu_UserPCal[10];
extern int m_iPhase;
extern unsigned char m_ucClear;
extern CHAMBER chamber[];
extern CURRENT current;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);

/**
 * \details Handles Amulet Screen SetupCalNum.htm. SetupCalNum.htm allows the user to change the cal num for a built-in nuclide.
 * \param Amulet_Byte_ID Description
 * \param 92 Language (STATE) English = 0, French = 1
 * \param 93 CRC-77t (STATE) Not CRC-77t = 0, CRC-77t = 1
 * \param 100 Display Data (0xFF)
 * \param Amulet_String_ID Description
 * \param 100_101 Nuclide
 * \param 102 Original R Cal
 * \param 103 Original P Cal
 * \param 104 User R Cal
 * \param 105 User P Cal
 * \param 110 Title
 * \param 111 Nuclide Label
 * \param 112 Default RCal Label
 * \param 113 Default PCal Label
 * \param 114 User RCal Label
 * \param 115 User PCal Label
 * \param 116_117 Please Enter R Cal#
 * \param 118_119 Please Enter P Cal#
 * \param 120_121 Please Select Nuclide
 * \returns None
 */
void AmuletSetupCalNum_menu(void){
	char acMsg[100];
	float fResponse;
	short ch_num = current.main_chamber;
	short ch_type;
	char err_msg[100];

	if(current.num_chambers>0) ch_type = chamber_type(ch_num);
	else{
		ch_type = R_CHAMB;
		ch_num = 0;
	}

	switch(m_iPhase){
		case PHASE_SETUP_CAL_NUM_PRE_INIT:
			if(m_ucClear == 11){
				AmuletSetupCalNumMenu_NuclideID = -1;
				AmuletSetupCalNumMenu_UserRCal[0] = 0;
				AmuletSetupCalNumMenu_UserPCal[0] = 0;
				m_ucClear = 0;
			}

			SetAmuletByte(92,current.language);
			if(chamber_77t()) SetAmuletByte(93, 0x01);
			else SetAmuletByte(93, 0x00);

			if(AmuletSetupCalNumMenu_NuclideID== -1){
				send_to_amulet_string(100, "");
				send_to_amulet_string(102, "");
				send_to_amulet_string(103, "");
				send_to_amulet_string(104, "");
				send_to_amulet_string(105, "");
			}else{
				if(strlen(AmuletSetupCalNumMenu_UserRCal) != 0){
					if(strcmp(AmuletSetupCalNumMenu_UserRCal, "EMPTY") == 0){
						if((ch_type == R_CHAMB) && (chamber[ch_num].nuc_index == AmuletSetupCalNumMenu_NuclideID) && (NuclideData_getResponse(AmuletSetupCalNumMenu_NuclideID, R_CHAMB) == 0.0)){
							chamber[ch_num].nuc_index = NuclideData_getIndexFromName("Tc99m");
							set_nuclide_data(chamber[ch_num].nuc_index, ch_num);
						}

						if((ch_type == C_CHAMB) && (chamber[ch_num].nuc_index == AmuletSetupCalNumMenu_NuclideID) && (NuclideData_getResponse(AmuletSetupCalNumMenu_NuclideID, C_CHAMB) == 0.0)){
							chamber[ch_num].nuc_index = NuclideData_getIndexFromName("Cs137");
							set_nuclide_data(chamber[ch_num].nuc_index, ch_num);
						}

						if((ch_type == K_CHAMB) && (chamber[ch_num].nuc_index == AmuletSetupCalNumMenu_NuclideID) && (NuclideData_getResponse(AmuletSetupCalNumMenu_NuclideID, K_CHAMB) == 0.0)){
							chamber[ch_num].nuc_index = NuclideData_getIndexFromName("Cs137");
							set_nuclide_data(chamber[ch_num].nuc_index, ch_num);
						}
						AmuletSetupCalNumMenu_UserRCal[0] = 0;
					}

					if(chamber_C()){
						switch(NuclideData_updateUserCalNum(AmuletSetupCalNumMenu_NuclideID, C_CHAMB, AmuletSetupCalNumMenu_UserRCal)){
							case -1:
								// Invalid Cal Num
								//DISPLAY_ERROR("Cal Number Error", "Invalid Cal Num");
								get_amulet_message(L_CAL_NUMBER_ERROR,acMsg);    // "Cal Number Error"
								get_amulet_message(L_INVALID_CAL_NUMBER,err_msg);    // "Invalid Cal Number"
								DISPLAY_ERROR(acMsg,err_msg);
								break;

							case -2:
								// User Cal Full
								//DISPLAY_ERROR("User Cal Error", "User Cal Full");
								get_amulet_message(L_CAL_NUMBER_ERROR,acMsg);    // "Cal Number Error"
								get_amulet_message(L_USER_CAL_FULL,err_msg);    // "User Cal Full"
								DISPLAY_ERROR(acMsg,err_msg);
								break;
						}
					}else if(chamber_K()){
						switch(NuclideData_updateUserCalNum(AmuletSetupCalNumMenu_NuclideID, K_CHAMB, AmuletSetupCalNumMenu_UserRCal)){
							case -1:
								// Invalid Cal Num
								//DISPLAY_ERROR("Cal Number Error", "Invalid Cal Num");
								get_amulet_message(L_CAL_NUMBER_ERROR,acMsg);    // "Cal Number Error"
								get_amulet_message(L_INVALID_CAL_NUMBER,err_msg);    // "Invalid Cal Number"
								DISPLAY_ERROR(acMsg,err_msg);
								break;

							case -2:
								// User Cal Full
								//DISPLAY_ERROR("User Cal Error", "User Cal Full");
								get_amulet_message(L_CAL_NUMBER_ERROR,acMsg);    // "Cal Number Error"
								get_amulet_message(L_USER_CAL_FULL,err_msg);    // "User Cal Full"
								DISPLAY_ERROR(acMsg,err_msg);
								break;
						}
					}else{
						switch(NuclideData_updateUserCalNum(AmuletSetupCalNumMenu_NuclideID, R_CHAMB, AmuletSetupCalNumMenu_UserRCal)){
							case -1:
								// Invalid Cal Num
								//DISPLAY_ERROR("Cal Number Error", "Invalid Cal Num");
								get_amulet_message(L_CAL_NUMBER_ERROR,acMsg);    // "Cal Number Error"
								get_amulet_message(L_INVALID_CAL_NUMBER,err_msg);    // "Invalid Cal Number"
								DISPLAY_ERROR(acMsg,err_msg);
								break;

							case -2:
								// User Cal Full
								//DISPLAY_ERROR("User Cal Error", "User Cal Full");
								get_amulet_message(L_CAL_NUMBER_ERROR,acMsg);    // "Cal Number Error"
								get_amulet_message(L_USER_CAL_FULL,err_msg);    // "User Cal Full"
								DISPLAY_ERROR(acMsg,err_msg);
								break;
						}
					}
					AmuletSetupCalNumMenu_UserRCal[0] = 0;
				}

				if(strlen(AmuletSetupCalNumMenu_UserPCal) != 0){
					if(strcmp(AmuletSetupCalNumMenu_UserPCal, "EMPTY") == 0){
						if((ch_type == P_CHAMB) && (chamber[ch_num].nuc_index == AmuletSetupCalNumMenu_NuclideID) && (NuclideData_getResponse(AmuletSetupCalNumMenu_NuclideID, P_CHAMB) == 0.0)){
							chamber[ch_num].nuc_index = NuclideData_getIndexFromName("Tc99m");
							set_nuclide_data(chamber[ch_num].nuc_index, ch_num);
						}
						AmuletSetupCalNumMenu_UserPCal[0] = 0;
					}
					switch(NuclideData_updateUserCalNum(AmuletSetupCalNumMenu_NuclideID, P_CHAMB, AmuletSetupCalNumMenu_UserPCal)){
						case -1:
							// Invalid Cal Num
							//DISPLAY_ERROR("Cal Number Error", "Invalid Cal Num");
							get_amulet_message(L_CAL_NUMBER_ERROR,acMsg);    // "Cal Number Error"
							get_amulet_message(L_INVALID_CAL_NUMBER,err_msg);    // "Invalid Cal Number"
							DISPLAY_ERROR(acMsg,err_msg);
							break;

						case -2:
							// User Cal Full
							//DISPLAY_ERROR("User Cal Error", "User Cal Full");
							get_amulet_message(L_CAL_NUMBER_ERROR,acMsg);    // "Cal Number Error"
							get_amulet_message(L_USER_CAL_FULL,err_msg);    // "User Cal Full"
							DISPLAY_ERROR(acMsg,err_msg);
							break;
					}
					AmuletSetupCalNumMenu_UserPCal[0] = 0;
				}

				NuclideData_getExtendedName(AmuletSetupCalNumMenu_NuclideID, acMsg);
				send_to_amulet_string(100, acMsg);

				if(chamber_C()){
					fResponse = NuclideData_getResponse(AmuletSetupCalNumMenu_NuclideID, C_CHAMB);
					if(fResponse == 0.0) strcpy(acMsg, "N/A");
					else DisplayNucCal_getCalNumFromResponse(fResponse, C_CHAMB, acMsg);
				}else if(chamber_K()){
					fResponse = NuclideData_getResponse(AmuletSetupCalNumMenu_NuclideID, K_CHAMB);
					if(fResponse == 0.0) strcpy(acMsg, "N/A");
					else DisplayNucCal_getCalNumFromResponse(fResponse, K_CHAMB, acMsg);
				}else{
					fResponse = NuclideData_getResponse(AmuletSetupCalNumMenu_NuclideID, R_CHAMB);
					if(fResponse == 0.0) strcpy(acMsg, "N/A");
					else DisplayNucCal_getCalNumFromResponse(fResponse, R_CHAMB, acMsg);
				}
				send_to_amulet_string(102, acMsg);

				fResponse = NuclideData_getResponse(AmuletSetupCalNumMenu_NuclideID, P_CHAMB);
				if(fResponse == 0.0) strcpy(acMsg, "N/A");
				else DisplayNucCal_getCalNumFromResponse(fResponse, P_CHAMB, acMsg);
				send_to_amulet_string(103, acMsg);

				if(chamber_C()){
					fResponse = NuclideData_getUserResponse(AmuletSetupCalNumMenu_NuclideID, C_CHAMB);
					if(fResponse == 0.0) acMsg[0] = 0;
					else DisplayNucCal_getCalNumFromResponse(fResponse, C_CHAMB, acMsg);
				}else if(chamber_K()){
					fResponse = NuclideData_getUserResponse(AmuletSetupCalNumMenu_NuclideID, K_CHAMB);
					if(fResponse == 0.0) acMsg[0] = 0;
					else DisplayNucCal_getCalNumFromResponse(fResponse, K_CHAMB, acMsg);
				}else{
					fResponse = NuclideData_getUserResponse(AmuletSetupCalNumMenu_NuclideID, R_CHAMB);
					if(fResponse == 0.0) acMsg[0] = 0;
					else DisplayNucCal_getCalNumFromResponse(fResponse, R_CHAMB, acMsg);
				}
				send_to_amulet_string(104, acMsg);

				fResponse = NuclideData_getUserResponse(AmuletSetupCalNumMenu_NuclideID, P_CHAMB);
				if(fResponse == 0.0) acMsg[0] = 0;
				else DisplayNucCal_getCalNumFromResponse(fResponse, P_CHAMB, acMsg);
				send_to_amulet_string(105, acMsg);
			}

			send_amulet_message(L_SETUP_CALNUM,110);    // "Setup CalNum"
			send_amulet_message(L_NUCLIDE_COLON,111);    // "Nuclide:"
			
			get_amulet_message(L_DEFAULT,acMsg);    // "Default"
			if(chamber_77t()) strcat(acMsg," Cal#:");
			else strcat(acMsg," R Cal#:");
			send_to_amulet_string(112,acMsg);
			
			get_amulet_message(L_DEFAULT,acMsg);    // "Default"
			strcat(acMsg," P Cal#:");
			send_to_amulet_string(113,acMsg);
			
			get_amulet_message(L_USER,acMsg);    // "User"
			if(chamber_77t()) strcat(acMsg," Cal#:");
			else strcat(acMsg," R Cal#:");
			send_to_amulet_string(114,acMsg);
			
			get_amulet_message(L_USER,acMsg);    // "User"
			strcat(acMsg," P Cal#:");
			send_to_amulet_string(115,acMsg);

			if(chamber_77t()) send_amulet_message(L_ENTER_CAL,116);    // "Please Enter Cal #:"
			else send_amulet_message(L_ENTER_RCAL,116);    // "Please Enter R Cal #:"

			send_amulet_message(L_ENTER_PCAL,118);    // "Please Enter P Cal #:"

			send_amulet_message(L_PLEASE_SELECT_NUCLIDE,120);    // "Please Select Nuclide"
			
			
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_CAL_NUM_WAIT;
			break;

		case PHASE_SETUP_CAL_NUM_WAIT:
			break;
	}
}

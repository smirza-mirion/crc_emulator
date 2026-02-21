/**
 * \file
 * \details This file handles calls from the Amulet User Nuclide Setup Screen
 */
#define PHASE_SETUP_NUCLIDE_PRE_INIT	0
#define PHASE_SETUP_NUCLIDE_WAIT		1
#define PHASE_SETUP_NUCLIDE_SAVE		2
#define PHASE_SETUP_NUCLIDE_CLEAR01		3
#define PHASE_SETUP_NUCLIDE_CLEAR02		4
#define PHASE_SETUP_NUCLIDE_CLEAR03		5
#define PHASE_SETUP_NUCLIDE_CLEAR04		6
#define PHASE_SETUP_NUCLIDE_CLEAR05		7
#define PHASE_SETUP_NUCLIDE_CLEAR06		8
#define PHASE_SETUP_NUCLIDE_CLEAR07		9
#define PHASE_SETUP_NUCLIDE_CLEAR08		10
#define PHASE_SETUP_NUCLIDE_CLEAR09		11
#define PHASE_SETUP_NUCLIDE_CLEAR10		12

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "nuc.h"
#include "i2c.h"
#include "message.h"
//#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CHAMBER chamber[];
extern const short max_chambers;
extern unsigned char m_ucHotKeyNuclideID[UPPER_LIMIT_CHAMB][8];
extern unsigned char m_ucHotKeyNuclideID2[UPPER_LIMIT_CHAMB][20];
extern KEYDEF AmuletSetupRemoteMenu_mirrorRemoteNuc[8];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void Amulet_trimTrailingZero(char *numberString);
static void clearrow(short row);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void SetAmuletBackHTML(void);
void tolower(char *c);
void trim_and_shrink(char *acByte);
//void write_HotKeys(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void send_accept_cancel(void);
void DB_WriteAllHotkey(int ch_type);

SETUP_NUCLIDE m_tSetupNuclide[10];

/**
 * \details Handles the Amulet Screen SetupNuclide.htm shows user defined nuclides.
 * \param Amulet_Byte_ID
 * \param 99 CRC-77t (STATE) Not CRC-77t = 0, CRC-77t = 1
 * \param 100 Display Screen
 * \param Amulet_String_ID Description
 * \param 90 Accept
 * \param 91 Cancel
 * \param 100  Nuclide 1
 * \param 101  Element 1
 * \param 102  Half Life 1
 * \param 103  R Cal Num 1
 * \param 104  P Cal Num 1
 * \param 105  Nuclide 2
 * \param 106  Element 2
 * \param 107  Half Life 2
 * \param 108  R Cal Num 2
 * \param 109  P Cal Num 2
 * \param 110  Nuclide 3
 * \param 111  Element 3
 * \param 112  Half Life 3
 * \param 113  R Cal Num 3
 * \param 114  P Cal Num 3
 * \param 115  Nuclide 4
 * \param 116  Element 4
 * \param 117  Half Life 4
 * \param 118  R Cal Num 4
 * \param 119  P Cal Num 4
 * \param 120  Nuclide 5
 * \param 121  Element 5
 * \param 122  Half Life 5
 * \param 123  R Cal Num 5
 * \param 124  P Cal Num 5
 * \param 125  Nuclide 6
 * \param 126  Element 6
 * \param 127  Half Life 6
 * \param 128  R Cal Num 6
 * \param 129  P Cal Num 6
 * \param 130  Nuclide 7
 * \param 131  Element 7
 * \param 132  Half Life 7
 * \param 133  R Cal Num 7
 * \param 134  P Cal Num 7
 * \param 135  Nuclide 8
 * \param 136  Element 8
 * \param 137  Half Life 8
 * \param 138  R Cal Num 8
 * \param 139  P Cal Num 8
 * \param 140  Nuclide 9
 * \param 141  Element 9
 * \param 142  Half Life 9
 * \param 143  R Cal Num 9
 * \param 144  P Cal Num 9
 * \param 145  Nuclide 10
 * \param 146  Element 10
 * \param 147  Half Life 10
 * \param 148  R Cal Num 10
 * \param 149  P Cal Num 10
 * \param 150  Title
 * \param 151  Nuclide Header
 * \param 152  Element Header
 * \param 153  Halflife Header
 * \param 154  Clear Button
 * \param 155_156 Please Enter Nuclide
 * \param 157_158 Please Enter Element
 * \param 159_160 Please Enter Halflife
 * \param 161_162 Please Enter R Cal
 * \param 163_164 Please Enter P Cal
 * \param 165 R Cal Header
 * \param 166 P Cal Header
 * \param 167_168 Please Enter Cal
 * \returns None
 */
void AmuletSetupNuclide_menu(void){
    short i, j;//, nuclideindex;
	char tempArray[20];
	//short ch_num = current.main_chamber;
	bool partial;
	char partialrow[100];
	char errormsg[100];
	char newnames[10][7];
	short newnamesnum;
	char stdnames[BUILTINNUC][7];
	short stdnamesnum;
	bool founddupnames;
	SETUP_NUCLIDE original[10];
	short removedr[10], removedp[10], removedc[10], removedk[10];
	short removedrnum, removedpnum, removedcnum, removedknum;
	bool changed, found, rhotchanged, photchanged;
	short nuc_index;
	short constch[12];
	short rlin_nuc_index, plin_nuc_index, clin_nuc_index, klin_nuc_index;
	USER_KEYS user_keys;
	char message[50];
	char tunit[6];

	//if(current.num_chambers==0) ch_num = 0;

	switch(m_iPhase){
		case PHASE_SETUP_NUCLIDE_PRE_INIT:
			if(m_ucClear==8){
				for(i=0; i<10; i++)	NuclideData_populateSetupStruct(i, &m_tSetupNuclide[i]);
				m_ucClear = 0;
			}

			if(chamber_77t()) SetAmuletByte(99, 0x01);
			else SetAmuletByte(99, 0x00);

			for(i=0; i<10; i++){
				if(strlen(m_tSetupNuclide[i].acName)!=0) send_to_amulet_string(100 + (5*i), m_tSetupNuclide[i].acName);
				if(strlen(m_tSetupNuclide[i].acElement)!=0) send_to_amulet_string(101 + (5*i), m_tSetupNuclide[i].acElement);
				if(m_tSetupNuclide[i].sHalflifeUnit==-1) tempArray[0] = 0;
				else{
					sprintf(tempArray, "%.5f", m_tSetupNuclide[i].fHalflife);
					Amulet_trimTrailingZero(tempArray);
					switch(m_tSetupNuclide[i].sHalflifeUnit){
						case SEC:
							get_amulet_message(L_SEC,tunit);    // " Sec"
							strcat(tempArray, tunit);
							break;

						case MIN:
							get_amulet_message(L_MIN,tunit);    // " Min"
							strcat(tempArray, tunit);
							//strcat(tempArray, " Min");
							break;

						case HOUR:
							get_amulet_message(L_HR,tunit);    // " Hr"
							strcat(tempArray, tunit);
							//strcat(tempArray, " Hr");
							break;

						case DAY:
							get_amulet_message(L_DAY,tunit);    // " Day"
							strcat(tempArray, tunit);
							//strcat(tempArray, " Day");
							break;

						case YEAR:
							get_amulet_message(L_YR,tunit);    // " Yr"
							strcat(tempArray, tunit);
							//strcat(tempArray, " Yr");
							break;
					}
				}
				if(strlen(tempArray)!=0)
					send_to_amulet_string(102 + (5*i), tempArray);
					//SetAmuletString(102 + (5*i), tempArray);
				if(chamber_C()){
					if(strlen(m_tSetupNuclide[i].acCCalNum)!=0) send_to_amulet_string(103 + (5*i), m_tSetupNuclide[i].acCCalNum);
					send_to_amulet_string(104 + (5*i), "");
				}else if(chamber_K()){
					if(strlen(m_tSetupNuclide[i].acKCalNum)!=0) send_to_amulet_string(103 + (5*i), m_tSetupNuclide[i].acKCalNum);
					send_to_amulet_string(104 + (5*i), "");
				}else{
					if(strlen(m_tSetupNuclide[i].acRCalNum)!=0) send_to_amulet_string(103 + (5*i), m_tSetupNuclide[i].acRCalNum);
					if(strlen(m_tSetupNuclide[i].acPCalNum)!=0) send_to_amulet_string(104 + (5*i), m_tSetupNuclide[i].acPCalNum);
				}
				delayloop(3);
			}

			get_amulet_message(L_SETUP_NUCLIDES,message);    // "Setup Nuclide"
			send_to_amulet_string(150,message);

			get_amulet_message(L_NUCLIDE,message);    // "Nuclide"
			send_to_amulet_string(151,message);

			get_amulet_message(L_SETUP_NUCLIDE_ELEMENT,message);    // "Element"
			send_to_amulet_string(152,message);

			get_amulet_message(L_SETUP_NUCLIDE_HALFLIFE,message);    // "Halflife"
			send_to_amulet_string(153,message);

			get_amulet_message(L_CLEAR_BUTTON,message);    // "Clear"
			send_to_amulet_string(154,message);

			get_amulet_message(L_ENTER_NUCLIDE,message);    // "Please Enter Nuclide"
			strcat(message,":");
			send_to_amulet_string(155,message);

			get_amulet_message(L_ENTER_ELEMENT,message);    // "Please Enter Element"
			strcat(message,":");
			send_to_amulet_string(157,message);

			get_amulet_message(L_ENTER_HALFLIFE,message);    // "Please Enter Halflife"
			strcat(message,":");
			send_to_amulet_string(159,message);

			send_amulet_message(L_ENTER_RCAL,161);    // "Please Enter R Cal #:"
			send_amulet_message(L_ENTER_PCAL,163);    // "Please Enter P Cal #:"


			if(chamber_77t()){
				send_to_amulet_string(165, "Cal#");
				send_to_amulet_string(166, "");
			}else{
				send_to_amulet_string(165, "Cal#(R)");
				send_to_amulet_string(166, "Cal#(P)");
			}

			send_amulet_message(L_ENTER_CAL,167);    // "Please Enter Cal #:"

			send_accept_cancel();

			SetAmuletByte(100, 0xFF);
			delayloop(25);
			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_SETUP_NUCLIDE_WAIT;
			break;

		case PHASE_SETUP_NUCLIDE_WAIT:
			break;

		case PHASE_SETUP_NUCLIDE_SAVE:
			beep_amulet();
			// Test For Partial Entries
			partial = FALSE;
			partialrow[0] = 0;
			for(i=0;i<10;i++){
				if((m_tSetupNuclide[i].acName[0]!=0) || (m_tSetupNuclide[i].acElement[0]!=0) || (m_tSetupNuclide[i].acRCalNum[0]!=0) || (m_tSetupNuclide[i].acPCalNum[0]!=0) || (m_tSetupNuclide[i].acCCalNum[0]!=0) || (m_tSetupNuclide[i].acKCalNum[0]!=0) || (m_tSetupNuclide[i].sHalflifeUnit!=-1)){
					if((m_tSetupNuclide[i].acName[0]==0) || (m_tSetupNuclide[i].acElement[0]==0) || (m_tSetupNuclide[i].sHalflifeUnit==-1) || ((m_tSetupNuclide[i].acRCalNum[0]==0) && (m_tSetupNuclide[i].acPCalNum[0]==0) && (m_tSetupNuclide[i].acCCalNum[0]==0) && (m_tSetupNuclide[i].acKCalNum[0]==0))){
						sprintf(tempArray, "%d, ", i+1);
						strcat(partialrow, tempArray);
						partial = TRUE;
					}
				}
			}
			if(partial){
				partialrow[strlen(partialrow) - 2] = 0;
				//strcpy(errormsg, "Please complete partial Nuclide entry.\nRow(s): ");
				get_amulet_message(L_PLEASE_COMPLETE_PARTIAL_NUCLIDE_ENTRY,errormsg);    // "Please complete partial Nuclide entry.\nRow(s): "
				strcat(errormsg, partialrow);
				//Amulet_DisplayError("Setup Nuclide Error", errormsg, TRUE);
				get_amulet_message(L_SETUP_NUCLIDE_ERROR,message);    // "Setup Nuclide Error"
				Amulet_DisplayError(message, errormsg, TRUE);
				return;
			}

			// Test For Duplicate Entries (case Insensitive and remove internal space)
			newnamesnum=0;
			for(i=0;i<10;i++){
				if(m_tSetupNuclide[i].acName[0]!=0){
					strcpy(&(newnames[newnamesnum][0]), m_tSetupNuclide[i].acName);
					trim_and_shrink(&(newnames[newnamesnum][0]));
					tolower(&(newnames[newnamesnum][0]));
					newnamesnum++;
				}
			}

			stdnamesnum = 0;
			for(i=0;i<BUILTINNUC;i++){
				if(NuclideData_isShow(SortedBuiltInIndex[i])){
					NuclideData_getName(SortedBuiltInIndex[i], &(stdnames[stdnamesnum][0]));
					trim_and_shrink(&(stdnames[stdnamesnum][0]));
					tolower(&(stdnames[stdnamesnum][0]));
					stdnamesnum++;
				}
			}

			founddupnames = FALSE;
			partialrow[0] = 0;
			for(i=0;i<newnamesnum-1;i++){
				for(j=i+1;j<newnamesnum;j++){
					if(strcmp(&(newnames[i][0]), &(newnames[j][0]))==0){
						strcat(partialrow, &(newnames[i][0]));
						strcat(partialrow, ", ");
						founddupnames = TRUE;
					}
				}
			}

			if(founddupnames){
				partialrow[strlen(partialrow) - 2] = 0;
				//strcpy(errormsg, "Duplicate Name found:\n");
				get_amulet_message(L_DUPLICATE_NAME_FOUND,errormsg);    // "Duplicate Name found:\n"
				strcat(errormsg, partialrow);
				get_amulet_message(L_SETUP_NUCLIDE_ERROR,message);    // "Setup Nuclide Error"
				//Amulet_DisplayError("Setup Nuclide Error", errormsg, TRUE);
				Amulet_DisplayError(message, errormsg, TRUE);
				return;
			}

			partialrow[0] = 0;
			for(i=0;i<newnamesnum;i++){
				for(j=0;j<stdnamesnum;j++){
					if(strcmp(&(newnames[i][0]),&(stdnames[j][0]))==0){
						strcat(partialrow, &(newnames[i][0]));
						strcat(partialrow, ", ");
						founddupnames = TRUE;
					}
				}
			}

			if(founddupnames){
				partialrow[strlen(partialrow) - 2] = 0;
				//strcpy(errormsg, "Duplicate Name found:\n");
				get_amulet_message(L_DUPLICATE_NAME_FOUND,errormsg);    // "Duplicate Name found:\n"
				strcat(errormsg, partialrow);
				get_amulet_message(L_SETUP_NUCLIDE_ERROR,message);    // "Setup Nuclide Error"
				//Amulet_DisplayError("Setup Nuclide Error", errormsg, TRUE);
				Amulet_DisplayError(message, errormsg, TRUE);
				return;
			}

			// Test For Nuclide Name Changes Or Missing RCal Or Missing PCal
			// Missing R and Missing P - from missing or name change
			// Missing R by removing RCal
			// Missing P by removing PCal
			for(i=0;i<10;i++){
				NuclideData_populateSetupStruct(i, &original[i]);
			}
			removedrnum = 0;
			removedpnum = 0;
			removedcnum = 0;
			removedknum = 0;
			for(i=0;i<10;i++){
				if(original[i].sHalflifeUnit != -1){
					if(strcmp(original[i].acName, m_tSetupNuclide[i].acName)!=0){
						if(original[i].acRCalNum[0]!=0){
							removedr[removedrnum] = i;
							removedrnum++;
						}
						if(original[i].acPCalNum[0]!=0){
							removedp[removedpnum] = i;
							removedpnum++;
						}
						if(original[i].acCCalNum[0]!=0){
							removedc[removedcnum] = i;
							removedcnum++;
						}
						if(original[i].acKCalNum[0]!=0){
							removedk[removedknum] = i;
							removedknum++;
						}
					}else{
						if(original[i].acRCalNum[0]!=0){
							if(m_tSetupNuclide[i].acRCalNum[0]==0){
								removedr[removedrnum] = i;
								removedrnum++;
							}
						}

						if(original[i].acPCalNum[0]!=0){
							if(m_tSetupNuclide[i].acPCalNum[0]==0){
								removedp[removedpnum] = i;
								removedpnum++;
							}
						}

						if(original[i].acCCalNum[0] !=0){
							if(m_tSetupNuclide[i].acCCalNum[0]==0){
								removedc[removedcnum] = i;
								removedcnum++;
							}
						}

						if(original[i].acKCalNum[0] !=0){
							if(m_tSetupNuclide[i].acKCalNum[0]==0){
								removedk[removedknum] = i;
								removedknum++;
							}
						}
					}
				}
			}

			// 1) Remove Relevant Nuclides from Chamber Nuclide
			if(chamber_77t()){
				for(i=0; i<max_chambers; i++){
					if(chamber[i].exists && chamber[i].active && (chamber[i].nuc_index >= USERNUC) && (chamber[i].nuc_index < (USERNUC+20))){
						chamber[i].nuc_index = NuclideData_getIndexFromName("Cs137");
						set_nuclide_data(chamber[i].nuc_index, i);
					}
				}
			}else{
				for(i=0; i<max_chambers; i++){
					if(chamber[i].exists && chamber[i].active && (chamber[i].nuc_index >= USERNUC) && (chamber[i].nuc_index < (USERNUC+20))){
						chamber[i].nuc_index = NuclideData_getIndexFromName("Tc99m");
						set_nuclide_data(chamber[i].nuc_index, i);
					}
				}
			}

			// 2) Remove Relevant Nuclides from Inventory Entry


			// 3) Remove Relevant Nuclides from Hotkeys
			rhotchanged = FALSE;
			for(i=0;i<8;i++){
				if(removedrnum>0){
					if((m_ucHotKeyNuclideID[R_CHAMB][i]!=255) && (m_ucHotKeyNuclideID[R_CHAMB][i]>=USERNUC) && (m_ucHotKeyNuclideID[R_CHAMB][i]<(USERNUC+20))){
						for(j=0;j<removedrnum;j++){
							if(m_ucHotKeyNuclideID[R_CHAMB][i] == removedr[j] + USERNUC){
								m_ucHotKeyNuclideID[R_CHAMB][i] = 255;
								rhotchanged = TRUE;
							}
						}
					}
				}

				if(removedpnum>0){
					if((m_ucHotKeyNuclideID[P_CHAMB][i]!=255) && (m_ucHotKeyNuclideID[P_CHAMB][i]>=USERNUC) && (m_ucHotKeyNuclideID[P_CHAMB][i]<(USERNUC+20))){
						for(j=0;j<removedpnum;j++){
							if(m_ucHotKeyNuclideID[P_CHAMB][i] == removedp[j] + USERNUC){
								m_ucHotKeyNuclideID[P_CHAMB][i] = 255;
								rhotchanged = TRUE;
							}
						}
					}
				}

				if(removedcnum>0){
					if((m_ucHotKeyNuclideID[C_CHAMB][i]!=255) && (m_ucHotKeyNuclideID[C_CHAMB][i]>=USERNUC) && (m_ucHotKeyNuclideID[C_CHAMB][i]<(USERNUC+20))){
						for(j=0;j<removedcnum;j++){
							if(m_ucHotKeyNuclideID[C_CHAMB][i] == removedc[j] + USERNUC){
								m_ucHotKeyNuclideID[C_CHAMB][i] = 255;
								rhotchanged = TRUE;
							}
						}
					}
				}

				if(removedknum>0){
					if((m_ucHotKeyNuclideID[K_CHAMB][i]!=255) && (m_ucHotKeyNuclideID[K_CHAMB][i]>=USERNUC) && (m_ucHotKeyNuclideID[K_CHAMB][i]<(USERNUC+20))){
						for(j=0;j<removedknum;j++){
							if(m_ucHotKeyNuclideID[K_CHAMB][i] == removedk[j] + USERNUC){
								m_ucHotKeyNuclideID[K_CHAMB][i] = 255;
								rhotchanged = TRUE;
							}
						}
					}
				}
			}

			photchanged = FALSE;
			for(i=0;i<20;i++){
				if(removedrnum>0){
					if((m_ucHotKeyNuclideID2[R_CHAMB][i]!=255) && (m_ucHotKeyNuclideID2[R_CHAMB][i]>=USERNUC) && (m_ucHotKeyNuclideID2[R_CHAMB][i]<(USERNUC+20))){
						for(j=0;j<removedrnum;j++){
							if(m_ucHotKeyNuclideID2[R_CHAMB][i] == removedr[j] + USERNUC){
								m_ucHotKeyNuclideID2[R_CHAMB][i] = 255;
								photchanged = TRUE;
							}
						}
					}
				}
				if(removedpnum>0){
					if((m_ucHotKeyNuclideID2[P_CHAMB][i]!=255) && (m_ucHotKeyNuclideID2[P_CHAMB][i]>=USERNUC) && (m_ucHotKeyNuclideID2[P_CHAMB][i]<(USERNUC+20))){
						for(j=0;j<removedpnum;j++){
							if(m_ucHotKeyNuclideID2[P_CHAMB][i] == removedp[j] + USERNUC){
								m_ucHotKeyNuclideID2[P_CHAMB][i] = 255;
								photchanged = TRUE;
							}
						}
					}
				}
				if(removedcnum>0){
					if((m_ucHotKeyNuclideID2[C_CHAMB][i]!=255) && (m_ucHotKeyNuclideID2[C_CHAMB][i]>=USERNUC) && (m_ucHotKeyNuclideID2[C_CHAMB][i]<(USERNUC+20))){
						for(j=0;j<removedcnum;j++){
							if(m_ucHotKeyNuclideID2[C_CHAMB][i] == removedc[j] + USERNUC){
								m_ucHotKeyNuclideID2[C_CHAMB][i] = 255;
								photchanged = TRUE;
							}
						}
					}
				}
				if(removedknum>0){
					if((m_ucHotKeyNuclideID2[K_CHAMB][i]!=255) && (m_ucHotKeyNuclideID2[K_CHAMB][i]>=USERNUC) && (m_ucHotKeyNuclideID2[K_CHAMB][i]<(USERNUC+20))){
						for(j=0;j<removedknum;j++){
							if(m_ucHotKeyNuclideID2[K_CHAMB][i] == removedk[j] + USERNUC){
								m_ucHotKeyNuclideID2[K_CHAMB][i] = 255;
								photchanged = TRUE;
							}
						}
					}
				}
			}

			if(rhotchanged || photchanged){
				DB_WriteAllHotkey(0x7fff); //write_HotKeys();
			}

			// 4) Remove Relevant Nuclides from User Cal#s


			// 5) Remove Relevant Nuclides from Remote
			EE_READ(remote_nucs, (uchar *) &AmuletSetupRemoteMenu_mirrorRemoteNuc);
			changed = FALSE;
			for(i=0;i<8;i++){
				if(AmuletSetupRemoteMenu_mirrorRemoteNuc[i].keyiso[0].iso_name[0] != 0){
					nuc_index = NuclideData_getIndexFromNameIncludingUser(AmuletSetupRemoteMenu_mirrorRemoteNuc[i].keyiso[0].iso_name);
					if(nuc_index != -1){
						if((nuc_index >= USERNUC) && (nuc_index < (USERNUC+20))){
							found = FALSE;
							for(j=0;j<10;j++){
								if(strcmp(m_tSetupNuclide[j].acName,AmuletSetupRemoteMenu_mirrorRemoteNuc[i].keyiso[0].iso_name)==0){
									if(m_tSetupNuclide[j].acRCalNum[0]!=0){
										found = TRUE;
									}
								}
							}
							if(!found){
								AmuletSetupRemoteMenu_mirrorRemoteNuc[i].keyiso[0].iso_name[0] = 0;
								changed = TRUE;
							}
						}
					}
				}

				if(AmuletSetupRemoteMenu_mirrorRemoteNuc[i].keyiso[1].iso_name[0] != 0){
					nuc_index = NuclideData_getIndexFromNameIncludingUser(AmuletSetupRemoteMenu_mirrorRemoteNuc[i].keyiso[1].iso_name);
					if(nuc_index != -1){
						if(nuc_index >= USERNUC && (nuc_index < (USERNUC+20))){
							found = FALSE;

							for(j=0;j<10;j++){
								if(strcmp(m_tSetupNuclide[j].acName,AmuletSetupRemoteMenu_mirrorRemoteNuc[i].keyiso[1].iso_name)==0){
									if(m_tSetupNuclide[j].acPCalNum[0]!=0){
										found = TRUE;
									}
								}
							}

							if(!found){
								AmuletSetupRemoteMenu_mirrorRemoteNuc[i].keyiso[1].iso_name[0] = 0;
								changed = TRUE;
							}
						}
					}
				}
			}
			if(changed){
				EE_WRITE(remote_nucs, (uchar *) &AmuletSetupRemoteMenu_mirrorRemoteNuc);
			}

			// 6) Remove Relevant Nuclides from Autoconstancy
			EE_READ(constancych, (uchar *)&constch);
			changed = FALSE;
			for(i=0;i<12;i++){
				if((constch[i]>=USERNUC) && (constch[i]<(USERNUC+20))){
					if(m_tSetupNuclide[constch[i]-USERNUC].sHalflifeUnit==-1){
						constch[i] = -1;
						changed = TRUE;
					}else{
						if(strcmp(original[constch[i]-USERNUC].acName, m_tSetupNuclide[constch[i]-USERNUC].acName)!=0){
							constch[i] = -1;
							changed = TRUE;
						}
					}
				}
			}
			if(changed){
				EE_WRITE(constancych, (uchar *)&constch);
			}

			// 7) Remove Relevant Nuclides from Standard Linearity
			changed = FALSE;
			EE_READ(lindef.nuc_index_Std[R_CHAMB], (uchar *)&rlin_nuc_index);
			if(rlin_nuc_index>=USERNUC && (rlin_nuc_index<(USERNUC+20))){
				if(m_tSetupNuclide[rlin_nuc_index - USERNUC].sHalflifeUnit == -1){
					if(chamber_77t()) rlin_nuc_index = NuclideData_getIndexFromName("Cs137");
					else rlin_nuc_index = NuclideData_getIndexFromName("Tc99m");
					changed = TRUE;
				}else{
					if(strcmp(original[rlin_nuc_index - USERNUC].acName, m_tSetupNuclide[rlin_nuc_index - USERNUC].acName)!=0){
						if(chamber_77t()) rlin_nuc_index = NuclideData_getIndexFromName("Cs137");
						else rlin_nuc_index = NuclideData_getIndexFromName("Tc99m");
						changed = TRUE;
					}else{
						if(chamber_C()){ // C_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded
							if(m_tSetupNuclide[rlin_nuc_index - USERNUC].acCCalNum[0]==0){
								rlin_nuc_index = NuclideData_getIndexFromName("Cs137");
								changed = TRUE;
							}
						}else if(chamber_K()){ // K_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded
							if(m_tSetupNuclide[rlin_nuc_index - USERNUC].acKCalNum[0]==0){
								rlin_nuc_index = NuclideData_getIndexFromName("Cs137");
								changed = TRUE;
							}
						}else{
							if(m_tSetupNuclide[rlin_nuc_index - USERNUC].acRCalNum[0]==0){
								rlin_nuc_index = NuclideData_getIndexFromName("Tc99m");
								changed = TRUE;
							}
						}
					}
				}
			}
			if(changed) EE_WRITE(lindef.nuc_index_Std[R_CHAMB], (uchar *)&rlin_nuc_index);

			changed = FALSE;
			EE_READ(lindef.nuc_index_Std[P_CHAMB], (uchar *)&plin_nuc_index);
			if(plin_nuc_index>=USERNUC && (plin_nuc_index<(USERNUC+20))){
				if(m_tSetupNuclide[plin_nuc_index - USERNUC].sHalflifeUnit == -1){
					plin_nuc_index = NuclideData_getIndexFromName("F 18");
					changed = TRUE;
				}else{
					if(strcmp(original[plin_nuc_index - USERNUC].acName, m_tSetupNuclide[plin_nuc_index - USERNUC].acName)!=0){
						plin_nuc_index = NuclideData_getIndexFromName("F 18");
						changed = TRUE;
					}else{
						if(m_tSetupNuclide[plin_nuc_index - USERNUC].acPCalNum[0]==0){
							plin_nuc_index = NuclideData_getIndexFromName("F 18");
							changed = TRUE;
						}
					}
				}
			}
			if(changed) EE_WRITE(lindef.nuc_index_Std[P_CHAMB], (uchar *)&plin_nuc_index);

			// 8) Remove Relevant Nuclides from PC Communications Nuclide
			if(photchanged){
				for(i=0; i<10; i++){
					if(m_ucHotKeyNuclideID2[1][i+4]==255) user_keys.keys[i] = -1;
					else user_keys.keys[i] = m_ucHotKeyNuclideID2[1][i+4];
				}
				EE_WRITE(user_keys[0], (uchar *) &user_keys);
			}

			// Save User Nuclide
			for(i=0;i<10;i++){
				NuclideData_updateMirror(i, &(m_tSetupNuclide[i]));
			}

			SetAmuletBackHTML();
			return;
			//break;

		case PHASE_SETUP_NUCLIDE_CLEAR01:
		case PHASE_SETUP_NUCLIDE_CLEAR02:
		case PHASE_SETUP_NUCLIDE_CLEAR03:
		case PHASE_SETUP_NUCLIDE_CLEAR04:
		case PHASE_SETUP_NUCLIDE_CLEAR05:
		case PHASE_SETUP_NUCLIDE_CLEAR06:
		case PHASE_SETUP_NUCLIDE_CLEAR07:
		case PHASE_SETUP_NUCLIDE_CLEAR08:
		case PHASE_SETUP_NUCLIDE_CLEAR09:
		case PHASE_SETUP_NUCLIDE_CLEAR10:
			beep_amulet();
			clearrow(m_iPhase - PHASE_SETUP_NUCLIDE_CLEAR01);
			m_iPhase = PHASE_SETUP_NUCLIDE_WAIT;
			break;
	}
}

static void clearrow(short row){
	m_tSetupNuclide[row].acName[0] = 0;
	m_tSetupNuclide[row].acElement[0] = 0;
	m_tSetupNuclide[row].sHalflifeUnit = -1;
	m_tSetupNuclide[row].fHalflife = -1;
	m_tSetupNuclide[row].acPCalNum[0] = 0;
	m_tSetupNuclide[row].acRCalNum[0] = 0;
	m_tSetupNuclide[row].acCCalNum[0] = 0;
	m_tSetupNuclide[row].acKCalNum[0] = 0;

	send_to_amulet_string(100 + (5*row), "");
	send_to_amulet_string(101 + (5*row), "");
	send_to_amulet_string(102 + (5*row), "");
	send_to_amulet_string(103 + (5*row), "");
	send_to_amulet_string(104 + (5*row), "");
	SetAmuletByte(102 + row, 0xFF);
}

void tolower(char *c){
	while(*c != 0){
		if(((*c)>=65) && ((*c)<=90)){
			(*c) = (*c) + 32;
		}
		c++;
	}
}

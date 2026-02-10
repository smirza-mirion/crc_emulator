#define PHASE_KIT_INVENTORY_PRE_INIT 0
#define PHASE_KIT_INVENTORY_WAIT 1
#define PHASE_KIT_INVENTORY_DELETE 2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "nuc.h"
#include "message.h"

extern int m_iPhase;
extern INVENTORY inventory[MAX_INVENTORY];
extern short int m_iSelectedMapItem;
extern short int m_iMap[MAX_INVENTORY];
//extern const char *study_type[];
extern unsigned char m_ucClear;
extern CURRENT current;

short m_iKitInventory_KitStudy;
char m_cKitInventory_KitID;
char m_acKitInventory_KitLot[17];
float m_fKitInventory_KitActivity;
float m_fKitInventory_KitVolume;
short m_iKitInventory_NucIndex;
float m_fKitInventory_MeasuredActivity;
time_t m_dtmKitInventory_MeasuredDate;
unsigned char m_ucKitInventory_MeasuredNuclide;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void trim(char *acByte);
float get_current_act(INVENTORY *inv, NUCDATA *nuc, time_t *nowtime);
float get_timepoint_act(INVENTORY *inv, NUCDATA *nuc, time_t *timepoint);
unsigned char PopPageStack(void);
char SetAmuletHTML(unsigned int uiValue);
unsigned char CurrentPageStack(void);
float update_motc(INVENTORY *inv);
float update_motc_timepoint(INVENTORY *inv, time_t *timepoint);
void GetExtendedNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
void GetExtendedTimeInfoLanguage(time_t *dtmDateTime, char *acMsg);
void write_Inventory(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void get_study_type(short istudy, char *study);
void trim(char *acByte);


void AmuletKitInv_menu(void){
	float fActivity, fVolume, fMoPerTc, fTemp;
	short i, iNuclide, iStudy;
	char cID;
//	char cSyst;
	char acLot[17];
//	time_t dtmDate,
	time_t nowtime;
	char acMsg[100], acMsg2[100];
//	char acHalflife[20];
	NUCDATA nuc;
//	short ndec, kun;
//	float act_disp;
//	bool over_flag;
	unsigned char flgValidActivity, flgValidVolume;
	float fConc, fReqVol, fDrawnVol, fRemainingVol, fRemainingActivity, fCurrentMoPerTc;
	char message[100], formatstring[104];

	switch(m_iPhase){
		case PHASE_KIT_INVENTORY_PRE_INIT:
			if(m_ucClear == 3){
				m_iKitInventory_KitStudy = -1;
				m_cKitInventory_KitID = 0;
				m_acKitInventory_KitLot[0] = 0;
				m_fKitInventory_KitActivity = -999;
				m_fKitInventory_KitVolume = -999;
				m_fKitInventory_MeasuredActivity = -999;
				m_dtmKitInventory_MeasuredDate = 0;
				m_ucKitInventory_MeasuredNuclide = 0xFF;
				m_ucClear = 0;
			}
			SetAmuletByte(92,current.language);			

			if((m_fKitInventory_MeasuredActivity != -999) && (m_dtmKitInventory_MeasuredDate != 0) && (m_ucKitInventory_MeasuredNuclide == m_iKitInventory_NucIndex)){
				iNuclide = inventory[m_iMap[m_iSelectedMapItem]].nucnum;
				NuclideData_getNuclide(iNuclide, &nuc);
				fActivity = get_timepoint_act(&(inventory[m_iMap[m_iSelectedMapItem]]), &nuc, &m_dtmKitInventory_MeasuredDate);
				fConc = fActivity / inventory[m_iMap[m_iSelectedMapItem]].vol;
				fDrawnVol = m_fKitInventory_MeasuredActivity / fConc;
				fRemainingVol = inventory[m_iMap[m_iSelectedMapItem]].vol - fDrawnVol;
				fRemainingActivity = fActivity - m_fKitInventory_MeasuredActivity;

				inventory[m_iMap[m_iSelectedMapItem]].vol = fRemainingVol;
				inventory[m_iMap[m_iSelectedMapItem]].act = fRemainingActivity;
				inventory[m_iMap[m_iSelectedMapItem]].date = m_dtmKitInventory_MeasuredDate;

				if(iNuclide == NuclideData_getIndexFromName("Tc99m")){
					if(inventory[m_iMap[m_iSelectedMapItem]].mopertc > 0){
						fCurrentMoPerTc = update_motc_timepoint(&(inventory[m_iMap[m_iSelectedMapItem]]), &m_dtmKitInventory_MeasuredDate);
						inventory[m_iMap[m_iSelectedMapItem]].mopertc = fCurrentMoPerTc;
					}else{
						fCurrentMoPerTc = inventory[m_iMap[m_iSelectedMapItem]].mopertc;
					}
				}else{
					fCurrentMoPerTc = 0;
				}

				for(i=0; i<MAX_INVENTORY; i++){
					if(inventory[i].nucnum == -1){
						inventory[i].act = m_fKitInventory_MeasuredActivity;
						inventory[i].vol = fDrawnVol;
						inventory[i].mopertc = fCurrentMoPerTc;
						inventory[i].nucnum = iNuclide;
						inventory[i].type = m_iKitInventory_KitStudy;
						inventory[i].id = m_cKitInventory_KitID;
						// Removed:
						//inventory[i].syst = inventory[m_iMap[m_iSelectedMapItem]].syst;
						strcpy(inventory[i].lot, m_acKitInventory_KitLot);
						inventory[i].date = m_dtmKitInventory_MeasuredDate;
						break;
					}
				}

				//Save SD
				write_Inventory();
				PopPageStack();
				SetAmuletHTML(CurrentPageStack());
			}else{
				fVolume = inventory[m_iMap[m_iSelectedMapItem]].vol;
				fMoPerTc = inventory[m_iMap[m_iSelectedMapItem]].mopertc;
				iNuclide = inventory[m_iMap[m_iSelectedMapItem]].nucnum;
				m_iKitInventory_NucIndex = iNuclide;
				iStudy = inventory[m_iMap[m_iSelectedMapItem]].type;
				cID = inventory[m_iMap[m_iSelectedMapItem]].id;
				//cSyst = inventory[m_iMap[m_iSelectedMapItem]].syst;
				strncpy(acLot, inventory[m_iMap[m_iSelectedMapItem]].lot, 16);
				//dtmDate = inventory[m_iMap[m_iSelectedMapItem]].date;

				GetExtendedNuclideInfo(iNuclide, &nuc, acMsg);
				send_to_amulet_string(100, acMsg);

				if (cID == 0) {
					acMsg[0] = 0;
				}else{
					sprintf(acMsg, "%d", cID);
				}
				send_to_amulet_string(103, acMsg);

				send_to_amulet_string(104, acLot);

				if(fVolume == -999){
					acMsg[0] = 0;
				}else{
					sprintf(acMsg, "%.3f ml", fVolume);
				}
				send_to_amulet_string(105, acMsg);

				fActivity = get_current_act(&(inventory[m_iMap[m_iSelectedMapItem]]), &nuc, &nowtime);
				if(fActivity < 1.e-8){
					if(current.system == CI){
						fTemp = fActivity;
						fTemp *= 1.e+6;
						sprintf(acMsg,"%.3f uCi", fTemp);
					}else{
						format_activity_system(fActivity, acMsg);
					}
				}else{
					// Removed:
					//format_activity(fActivity, inventory[m_iMap[m_iSelectedMapItem]].syst, acMsg);
					//format_activity(fActivity, CI, acMsg);
					format_activity_system(fActivity, acMsg);
					trim(acMsg);
				}

				//over_flag = FALSE;
				// Removed:
				//if(((inventory[m_iMap[m_iSelectedMapItem]].syst == CI) && (fActivity > 10.)) || ((inventory[m_iMap[m_iSelectedMapItem]].syst == BQ) && (fActivity > 1.0e+12)))
				if(fActivity > 10.)
				{
					//strcpy(acMsg, "OVER");
					get_amulet_message(L_PR_OVER,acMsg);    // "OVER"
					//over_flag = TRUE;
				}
				send_to_amulet_string(106, acMsg);

				GetExtendedTimeInfoLanguage(&(inventory[m_iMap[m_iSelectedMapItem]].date), acMsg);
				send_to_amulet_string(107, acMsg);

				if(iNuclide == NuclideData_getIndexFromName("Tc99m")){
					if(iStudy != -1){
						//acMsg[5] = 0;
						//strncpy(acMsg, study_type[iStudy], 5);
						//SetAmuletString(102, acMsg);

						get_study_type(iStudy,acMsg);
						send_to_amulet_string(102,acMsg);

						send_amulet_message(L_STUDY_LABEL,209);    // "STUDY:"
						SetAmuletByte(101, 0xFF);
					}

					if(fMoPerTc != 0){
						if(fMoPerTc < 0){
							//strcpy(acMsg, "Tc Level TOO LOW");
							get_amulet_message(L_TC_LEVEL_TOO_LOW,acMsg);    // "Tc Level TOO LOW"
						}else{
							fCurrentMoPerTc = update_motc(&(inventory[m_iMap[m_iSelectedMapItem]]));
							if(fCurrentMoPerTc > 0) sprintf(acMsg, "%.3f", fCurrentMoPerTc);
							else
								//strcpy(acMsg, "Tc Level TOO LOW");
								get_amulet_message(L_TC_LEVEL_TOO_LOW,acMsg);    // "Tc Level TOO LOW"
						}
						send_to_amulet_string(121, acMsg);
						SetAmuletByte(102, 0xFF);
					}
				}

				if(m_iKitInventory_KitStudy == -1){
					acMsg[0] = 0;
					SetAmuletByte(200, 0xFF);
				}else{
					//acMsg[5] = 0;
					//strncpy(acMsg, study_type[m_iKitInventory_KitStudy], 5);
					get_study_type(m_iKitInventory_KitStudy,acMsg);
				}
				//SetAmuletString(109, acMsg);
				send_to_amulet_string(109,acMsg);

				if(m_cKitInventory_KitID == 0){
					acMsg[0] = 0;
				}else{
					sprintf(acMsg, "%d", m_cKitInventory_KitID);
				}
				send_to_amulet_string(110, acMsg);

				send_to_amulet_string(111, m_acKitInventory_KitLot);

				flgValidActivity = FALSE;
				if(m_fKitInventory_KitActivity == -999){
					acMsg[0] = 0;
					SetAmuletByte(201, 0xFF);
				}else{
					//over_flag = FALSE;
					// Removed:
					//if(((inventory[m_iMap[m_iSelectedMapItem]].syst == CI) && (m_fKitInventory_KitActivity > 10.)) || ((inventory[m_iMap[m_iSelectedMapItem]].syst == BQ) && (m_fKitInventory_KitActivity > 1.0e+12))){
					if(m_fKitInventory_KitActivity > 10.){
						//strcpy(acMsg, "OVER");
						get_amulet_message(L_OVER, acMsg);    // " OVER "
						trim(acMsg);
						//over_flag = TRUE;
						SetAmuletByte(201, 0xFF);
					}else{
						if(m_fKitInventory_KitActivity < 1.e-8){
							if(current.system == CI){
								fTemp = m_fKitInventory_KitActivity;
								fTemp *= 1.e+6;
								sprintf(acMsg, "%.3f uCi", fTemp);
							}else{
								format_activity_system(m_fKitInventory_KitActivity, acMsg);
							}
						}else{
							// Removed:
							//format_activity(m_fKitInventory_KitActivity, inventory[m_iMap[m_iSelectedMapItem]].syst, acMsg);
							//format_activity(m_fKitInventory_KitActivity, CI, acMsg);
							format_activity_system(m_fKitInventory_KitActivity, acMsg);
							trim(acMsg);
						}

						if(m_fKitInventory_KitActivity > fActivity){
							SetAmuletByte(201, 0xFF);
							//SetAmuletString(114, "KIT ACTIVITY GREATER THAN THAT");
							send_amulet_message(L_KIT_ACTIVITY_GREATER,114);    // "KIT ACTIVITY GREATER THAN THAT"
							//SetAmuletString(116, "IN SOURCE");
							send_amulet_message(L_IN_SOURCE,116);    // "IN SOURCE"
							SetAmuletByte(103, 0xFF);
							SetAmuletByte(104, 0xFF);
						}else{
							flgValidActivity = TRUE;
						}
					}
				}
				send_to_amulet_string(112, acMsg);

				flgValidVolume = FALSE;
				if(m_fKitInventory_KitVolume == -999){
					acMsg[0] = 0;
					SetAmuletByte(202, 0xFF);
				}else{
					sprintf(acMsg, "%.3f ml", m_fKitInventory_KitVolume);
					if(flgValidActivity == TRUE){
						fConc = fActivity / inventory[m_iMap[m_iSelectedMapItem]].vol;
						fReqVol = m_fKitInventory_KitActivity / fConc;
						if(fReqVol > m_fKitInventory_KitVolume){
							SetAmuletByte(202, 0xFF);

							//SetAmuletString(114, "KIT VOLUME LESS THAN THE");
							get_amulet_message(L_KIT_VOLUME_LESS, formatstring);    // "KIT VOLUME LESS THAN THE"
							sprintf(acMsg2, formatstring, fReqVol);
							send_to_amulet_string(114, acMsg2);

							//sprintf(acMsg2, "%.3f ml DRAW FROM SOURCE", fReqVol);
							get_amulet_message(L_DRAW_FROM_SOURCE, formatstring);    // "%.3f ml DRAW FROM SOURCE"
							sprintf(acMsg2, formatstring, fReqVol);
							send_to_amulet_string(116, acMsg2);

							SetAmuletByte(103, 0xFF);
							SetAmuletByte(104, 0xFF);
						}else{
							flgValidVolume = TRUE;
						}
					}
				}
				send_to_amulet_string(113, acMsg);

				if((flgValidActivity == TRUE) && (flgValidVolume == TRUE) && (m_iKitInventory_KitStudy != -1)){
					//sprintf(acMsg, "Please withdraw: %5.1f ml,\n and Measure Activity.", fReqVol);
					get_amulet_message(L_PLEASE_WITHDRAW_AND_MEASURE_ACTIVITY,message);    // "Please withdraw: %5.1f ml,\n and Measure Activity."
					sprintf(acMsg, message, fReqVol);
					send_to_amulet_string(118, acMsg);
					send_amulet_message(L_MEASURE_ACTIVITY,214);    // "Measure Activity"
					SetAmuletByte(105, 0xFF);
				}

				send_amulet_message(L_MAKE_INVENTORY_KIT,200);    // "Make Inventory Kit"
				send_amulet_message(L_SOURCE_NUCLIDE,202);    // "Source Nuclide:"
				send_amulet_message(L_SOURCE_ID,203);    // "Source ID:"
				send_amulet_message(L_SOURCE_LOT,204);    // "Source Lot:"
				send_amulet_message(L_SOURCE_VOLUME,205);    // "Source Volume:"
				send_amulet_message(L_SOURCE_ACTIVITY,206);    // "Source Activity:"
				send_amulet_message(L_SOURCE_DATE,207);    // "Source Date:"
				send_amulet_message(L_KIT_STUDY,209);    // "Kit Study:"
				send_amulet_message(L_KIT_ID,210);    // "Kit ID:"
				send_amulet_message(L_KIT_LOT,211);    // "Kit Lot:"
				send_amulet_message(L_KIT_ACTIVITY,212);    // "Kit Activity:"
				send_amulet_message(L_KIT_VOLUME,213);    // "Kit Volume:"
				send_amulet_message(L_PLEASE_MEASURE_WITHDRAWAL,216);    // "Please Measure Withdrawal:"
				send_amulet_message(L_PLEASE_ENTER_ID,218);    // "Please Enter ID:"
				send_amulet_message(L_PLEASE_ENTER_LOT,220);    // "Please Enter Lot:"
				send_amulet_message(L_PLEASE_ENTER_ACTIVITY,222);    // "Please Enter Activity:"
				send_amulet_message(L_PLEASE_ENTER_VOLUME,224);    // "Please Enter Volume(ml):"

				SetAmuletByte(100, 0xFF);
			}
			m_iPhase = PHASE_KIT_INVENTORY_WAIT;
			break;

		case PHASE_KIT_INVENTORY_WAIT:
			break;
	}
}

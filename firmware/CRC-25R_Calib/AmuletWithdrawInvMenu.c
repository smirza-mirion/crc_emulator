#define PHASE_WITHDRAW_INVENTORY_PRE_INIT 0
#define PHASE_WITHDRAW_INVENTORY_WAIT 1

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

float m_fWithdrawInventory_WithdrawActivity;
time_t m_dtmWithdrawInventory_TimeOfUse;
short m_iWithdrawInventory_NucIndex;
float m_fWithdrawInventory_MeasuredActivity;
time_t m_dtmWithdrawInventory_MeasuredDate;
unsigned char m_ucWithdrawInventory_MeasuredNuclide;

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

void AmuletWithdrawInv_menu(void){
	float fActivity, fVolume, fMoPerTc, fTemp, fConcentration, fDrawnVol, fRemainingVol, fRemainingActivity;
	short iNuclide, iStudy;
	char cID;
	//char cSyst;
	char acLot[17];
	//time_t dtmDate
	time_t nowtime;
	char acMsg[100];
	//char acHalflife[20];
	NUCDATA nuc;
	//short ndec, kun;
	//float act_disp;
	bool over_flag;
	float fInvActivityAtTimeOfUse, fConcentrationAtTimeOfUse, fReqVol, fCurrentMoPerTc;
	char message[50];
		
	switch(m_iPhase){
		case PHASE_WITHDRAW_INVENTORY_PRE_INIT:
			if(m_ucClear == 2){
				m_fWithdrawInventory_WithdrawActivity = -999;
				m_dtmWithdrawInventory_TimeOfUse = 0;
				m_fWithdrawInventory_MeasuredActivity = -999;
				m_dtmWithdrawInventory_MeasuredDate = 0;
				m_ucWithdrawInventory_MeasuredNuclide = 0xFF;
				m_ucClear = 0;
			}

			SetAmuletByte(92,current.language);
						
			if((m_fWithdrawInventory_MeasuredActivity != -999) && (m_dtmWithdrawInventory_MeasuredDate != 0) && (m_ucWithdrawInventory_MeasuredNuclide == m_iWithdrawInventory_NucIndex)){
				iNuclide = inventory[m_iMap[m_iSelectedMapItem]].nucnum;
				NuclideData_getNuclide(iNuclide, &nuc);
				fActivity = get_timepoint_act(&(inventory[m_iMap[m_iSelectedMapItem]]), &nuc, &m_dtmWithdrawInventory_MeasuredDate);
				fConcentration = fActivity / inventory[m_iMap[m_iSelectedMapItem]].vol;
				fDrawnVol = m_fWithdrawInventory_MeasuredActivity / fConcentration;
				fRemainingVol = inventory[m_iMap[m_iSelectedMapItem]].vol - fDrawnVol;
				fRemainingActivity = fActivity - m_fWithdrawInventory_MeasuredActivity;

				inventory[m_iMap[m_iSelectedMapItem]].vol = fRemainingVol;
				inventory[m_iMap[m_iSelectedMapItem]].act = fRemainingActivity;
				inventory[m_iMap[m_iSelectedMapItem]].date = m_dtmWithdrawInventory_MeasuredDate;

				if(iNuclide == NuclideData_getIndexFromName("Tc99m")){
					if(inventory[m_iMap[m_iSelectedMapItem]].mopertc > 0){
						fCurrentMoPerTc = update_motc_timepoint(&(inventory[m_iMap[m_iSelectedMapItem]]), &m_dtmWithdrawInventory_MeasuredDate);
						inventory[m_iMap[m_iSelectedMapItem]].mopertc = fCurrentMoPerTc;
					}
				}
				// Save SD
				write_Inventory();
				PopPageStack();
				SetAmuletHTML(CurrentPageStack());
			}else{
				fVolume = inventory[m_iMap[m_iSelectedMapItem]].vol;
				fMoPerTc = inventory[m_iMap[m_iSelectedMapItem]].mopertc;
				iNuclide = inventory[m_iMap[m_iSelectedMapItem]].nucnum;
				m_iWithdrawInventory_NucIndex = iNuclide;
				iStudy = inventory[m_iMap[m_iSelectedMapItem]].type;
				cID = inventory[m_iMap[m_iSelectedMapItem]].id;
				//cSyst = inventory[m_iMap[m_iSelectedMapItem]].syst;
				strncpy(acLot, inventory[m_iMap[m_iSelectedMapItem]].lot, 16);
				//dtmDate = inventory[m_iMap[m_iSelectedMapItem]].date;

				GetExtendedNuclideInfo(iNuclide, &nuc, acMsg);
				send_to_amulet_string(100, acMsg);

				if (cID == 0) {
					acMsg[0] = 0;
				}else {
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

				over_flag = FALSE;
				// Removed:
				//if(((inventory[m_iMap[m_iSelectedMapItem]].syst == CI) && (fActivity > 10.)) || ((inventory[m_iMap[m_iSelectedMapItem]].syst == BQ) && (fActivity > 1.0e+12)))
				if(fActivity > 10.)
				{
					//strcpy(acMsg, "OVER");
					get_amulet_message(L_PR_OVER,acMsg);    // "OVER"
					over_flag = TRUE;
				}
				send_to_amulet_string(106, acMsg);

				GetExtendedTimeInfoLanguage(&(inventory[m_iMap[m_iSelectedMapItem]].date), acMsg);
				send_to_amulet_string(107, acMsg);

				if(iNuclide == NuclideData_getIndexFromName("Tc99m")){
					if(iStudy != -1){
						send_amulet_message(L_STUDY_LABEL,209);    // "STUDY:"
						//acMsg[5] = 0;
						//strncpy(acMsg, study_type[iStudy], 5);
						//SetAmuletString(102, acMsg);
						get_study_type(iStudy,acMsg);
						send_to_amulet_string(102,acMsg);
						
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
						send_to_amulet_string(117, acMsg);
						SetAmuletByte(102, 0xFF);
					}
				}

				if(m_fWithdrawInventory_WithdrawActivity == -999){
					acMsg[0]=0;
					SetAmuletByte(200, 0xFF);
				}else{
					if(m_fWithdrawInventory_WithdrawActivity < 1.e-8){
						if(current.system == CI){
							fTemp = m_fWithdrawInventory_WithdrawActivity;
							fTemp *= 1.e+6;
							sprintf(acMsg,"%.3f uCi", fTemp);
						}else{
							format_activity_system(m_fWithdrawInventory_WithdrawActivity, acMsg);
						}
					}else{
						// Removed:
						//format_activity(m_fWithdrawInventory_WithdrawActivity, inventory[m_iMap[m_iSelectedMapItem]].syst, acMsg);
						//format_activity(m_fWithdrawInventory_WithdrawActivity, CI, acMsg);
						format_activity_system(m_fWithdrawInventory_WithdrawActivity, acMsg);
						trim(acMsg);
					}

					over_flag = FALSE;
					// Removed:
					//if(((inventory[m_iMap[m_iSelectedMapItem]].syst == CI) && (m_fWithdrawInventory_WithdrawActivity > 10.)) || ((inventory[m_iMap[m_iSelectedMapItem]].syst == BQ) && (m_fWithdrawInventory_WithdrawActivity > 1.0e+12))){
					if(m_fWithdrawInventory_WithdrawActivity > 10.){
						//strcpy(acMsg, "OVER");
						get_amulet_message(L_PR_OVER,acMsg);    // "OVER"
						over_flag = TRUE;
						SetAmuletByte(200, 0xFF);
					}
				}
				send_to_amulet_string(109, acMsg);

				if(m_dtmWithdrawInventory_TimeOfUse == 0){
					acMsg[0]=0;
					SetAmuletByte(201, 0xFF);
				}else{
					GetExtendedTimeInfoLanguage(&m_dtmWithdrawInventory_TimeOfUse, acMsg);
				}
				send_to_amulet_string(110, acMsg);

				if((m_fWithdrawInventory_WithdrawActivity != -999) && (m_dtmWithdrawInventory_TimeOfUse != 0) && (over_flag == FALSE)){
					fInvActivityAtTimeOfUse = nucdecay(inventory[m_iMap[m_iSelectedMapItem]].act,
													   inventory[m_iMap[m_iSelectedMapItem]].date,
													   m_dtmWithdrawInventory_TimeOfUse,
													   nuc.halflife,
													   nuc.hlunit);
					if(m_fWithdrawInventory_WithdrawActivity > fInvActivityAtTimeOfUse){
						//SetAmuletString(111, "ACTIVITY GREATER THAN THAT IN INVENTORY");
						send_amulet_message(L_ACTIVITY_GREATER_THAN_INVENTORY,111);    // "ACTIVITY GREATER THAN THAT IN INVENTORY"
						SetAmuletByte(103, 0xFF);
						SetAmuletByte(200, 0xFF);
						SetAmuletByte(201, 0xFF);
					}else{
						fConcentrationAtTimeOfUse = fInvActivityAtTimeOfUse / inventory[m_iMap[m_iSelectedMapItem]].vol;
						fReqVol = m_fWithdrawInventory_WithdrawActivity / fConcentrationAtTimeOfUse;
						//sprintf(acMsg, "Please withdraw: %5.1f ml,", fReqVol);
						get_amulet_message(L_PLEASE_WITHDRAW_ML,message);    // "Please withdraw: %5.1f ml"
						sprintf(acMsg, message, fReqVol);
						send_to_amulet_string(113, acMsg);
						//SetAmuletString(115, "and Measure Activity.");
						send_amulet_message(L_AND_MEASURE_ACTIVITY,115);    // "and Measure Activity."
						send_amulet_message(L_MEASURE_ACTIVITY,218);    // "Measure Activity"
						send_amulet_message(L_PLEASE_MEASURE_WITHDRAWAL,220);    // "Please Measure Withdrawal:"
						SetAmuletByte(104, 0xFF);
					}
				}

				send_amulet_message(L_WITHDRAW_INVENTORY,200);    // "Withdraw Inventory"
				send_amulet_message(L_NUCLIDE_COLON,204);    // "Nuclide:"
				send_amulet_message(L_LOT_COLON,205);    // "Lot:"
				send_amulet_message(L_VOLUME,206);    // "Volume:"
				send_amulet_message(L_ACTIVITY_COLON,207);    // "Activity:"
				send_amulet_message(L_DATE_COLON,208);    // "Date:"
				send_amulet_message(L_WITHDRAW_ACTIVITY,210);    // "Withdraw Activity:"
				send_amulet_message(L_TIME_OF_USE,212);    // "Time of Use:"
				send_amulet_message(L_PLEASE_ENTER_ACTIVITY,214);    // "Please Enter Activity:"
				send_amulet_message(L_PLEASE_ENTER_TIME_OF_USE,216);    // "Please Enter Time Of Use:"
			
				
				SetAmuletByte(100, 0xFF);
			}
			m_iPhase = PHASE_WITHDRAW_INVENTORY_WAIT;
			break;

		case PHASE_WITHDRAW_INVENTORY_WAIT:
			break;
	}
}

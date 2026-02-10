#define PHASE_DELETE_INVENTORY_PRE_INIT 0
#define PHASE_DELETE_INVENTORY_WAIT 1
#define PHASE_DELETE_INVENTORY_DELETE 2

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
extern CURRENT current;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void trim(char *acByte);
float get_current_act(INVENTORY *inv, NUCDATA *nuc, time_t *nowtime);
void SetAmuletBackHTML(void);
float update_motc(INVENTORY *inv);
void GetExtendedNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
void GetExtendedTimeInfoLanguage(time_t *dtmDateTime, char *acMsg);
void write_Inventory(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void get_study_type(short istudy, char *study);

void AmuletDeleteInv_menu(void){
	float fActivity, fVolume, fMoPerTc, fTemp, fCurrentMoPerTc;
	short iNuclide, iStudy;
	char cID;
	//char cSyst;
	char acLot[17];
	//time_t dtmDate
	time_t nowtime;
	char acMsg[100];
	//bool over_flag;
	NUCDATA nuc;


	switch(m_iPhase){
		case PHASE_DELETE_INVENTORY_PRE_INIT:

			SetAmuletByte(92,current.language);
			
			fVolume = inventory[m_iMap[m_iSelectedMapItem]].vol;
			fMoPerTc = inventory[m_iMap[m_iSelectedMapItem]].mopertc;
			iNuclide = inventory[m_iMap[m_iSelectedMapItem]].nucnum;
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
					send_to_amulet_string(108, acMsg);
					SetAmuletByte(102, 0xFF);
				}
			}

			send_amulet_message(L_DELETE_INVENTORY,200);    // "Delete Inventory"
			send_amulet_message(L_DELETE_INVENTORY,202);    // "Delete Inventory"
			send_amulet_message(L_NUCLIDE_COLON,204);    // "Nuclide:"
			send_amulet_message(L_LOT_COLON,205);    // "Lot:"
			send_amulet_message(L_VOLUME,206);    // "Volume:"
			send_amulet_message(L_ACTIVITY_COLON,207);    // "Activity:"
			send_amulet_message(L_DATE_COLON,208);    // "Date:"

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_DELETE_INVENTORY_WAIT;
			break;

		case PHASE_DELETE_INVENTORY_WAIT:
			break;

		case PHASE_DELETE_INVENTORY_DELETE:
			inventory[m_iMap[m_iSelectedMapItem]].nucnum = -1;
			//Save SD
			write_Inventory();
			SetAmuletBackHTML();
			break;
	}
}

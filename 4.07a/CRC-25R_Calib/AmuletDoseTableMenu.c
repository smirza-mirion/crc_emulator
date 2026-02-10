/**
 * \file
 * \details This file handles calls from the Amulet Dose Table Screen
 */
#define PHASE_DOSE_TABLE_PRE_INIT		0
#define PHASE_DOSE_TABLE_WAIT			1
#define PHASE_DOSE_TABLE_PRINT			2

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crc.h"
#include "nuc.h"
#include "i2c.h"
#include "printer.h"
#include "chambfac.h"
#include "message.h"

short AmuletDoseTableMenu_nuclideID;
float AmuletDoseTableMenu_activity;
float AmuletDoseTableMenu_nowactivity;
//short AmuletDoseTableMenu_activityUnit;
time_t AmuletDoseTableMenu_timestamp;
time_t AmuletDoseTableMenu_nowtime;
float AmuletDoseTableMenu_volume;
float AmuletDoseTableMenu_dose;
//float AmuletDoseTableMenu_doseUnit;
short AmuletDoseTableMenu_interval;
float calcvol[30];
time_t futuretime[30];

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CHAMBER  chamber[];
extern CURRENT current;

void trim_and_shrink(char *acByte);
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void Amulet_printDoseTable(short nucindex, float act0, float dose, float vol, time_t act_time, time_t *timearray, float *volarray);
void send_to_amulet_string(uchar ucIndex, char message0[]);

/**
 * \details Handles the Amulet Screen DoseTable.htm. DoseTable.htm is decay dose table screen.
 * \param Amulet_Byte_ID Description
 * \param 92 Language (STATE) English = 0, Spanish = 1
 * \param Amulet_String_ID Description
 * \param 150_151 Title
 * \param 152 Nuclide
 * \param 153 Activity
 * \param 154 Volume
 * \param 155 Dose
 * \param 156 Interval
 * \param 157 Measure Activity
 * \param 159_160 Please enter volume
 * \param 161_162 Please enter dose
 * \param 163_164 Enter interval in minutes
 * \param 165_166 Please select nuclide
 * \returns None
 */
void AmuletDoseTable_menu(void){
	char message[52];
	char nucname[50];
	char halflife[50];
	char timestring[50];
	float activity, volume;
	char volumestring[50];
	char complete;
	short index;
	time_t reporttime;
	short ch_num = current.main_chamber;
	char err_message[104];

	switch(m_iPhase){
		case PHASE_DOSE_TABLE_PRE_INIT:
			if(current.num_chambers>1) SetAmuletByte(21, 0xFF);

			if(current.num_chambers>0){
				sprintf(message, "Ch: %d", current.main_chamber + 1);
				switch(chamber_type(current.main_chamber)){
					case R_CHAMB:
						strcat(message, ", R");
						break;
					case P_CHAMB:
						strcat(message, ", PET");
						break;
					case B_CHAMB:
						strcat(message, ", BT");
						break;
					case ONE_DOT_EIGHT_CHAMB:
						strcat(message, ", 1.8 Atm");
						break;
					case C_CHAMB:
						strcat(message, ", HR");
						break;
					case K_CHAMB:
						strcat(message, ", 1K");
						break;
				}
				send_to_amulet_string(9, message);
				SetAmuletByte(20, 0xFF);
			}

			if(m_ucClear == 15){
				AmuletDoseTableMenu_nuclideID = chamber[ch_num].nuc_index;
				AmuletDoseTableMenu_activity = 0;
				//AmuletDoseTableMenu_activityUnit = -1;
				AmuletDoseTableMenu_timestamp = (time_t) 0;
				AmuletDoseTableMenu_volume = 0;
				AmuletDoseTableMenu_dose = 0;
				//AmuletDoseTableMenu_doseUnit = -1;
				AmuletDoseTableMenu_interval = 30;
				m_ucClear = 0;
			}

			complete = -1;
			if(AmuletDoseTableMenu_nuclideID != -1){
				NuclideData_getName(AmuletDoseTableMenu_nuclideID, nucname);
				sprintf(halflife, "%.2f", NuclideData_getHalflife(AmuletDoseTableMenu_nuclideID));
				switch(NuclideData_getHalflifeUnit(AmuletDoseTableMenu_nuclideID)){
					case SEC:
						//strcat(halflife, "sec");
						get_amulet_message(L_SEC3, message);    // "sec"
						break;

					case MIN:
						//strcat(halflife, "min");
						get_amulet_message(L_MIN3,message);    // "min"
						break;

					case HOUR:
						//strcat(halflife, "hr");
						get_amulet_message(L_HR3,message);    // "hr"
						break;

					case DAY:
						//strcat(halflife, "day");
						get_amulet_message(L_DAY3,message);    // "day"
						break;

					case YEAR:
						if(NuclideData_getHalflife(AmuletDoseTableMenu_nuclideID) > 1.0e+6){
							sprintf(halflife, "%.2f", NuclideData_getHalflife(AmuletDoseTableMenu_nuclideID) / 1.0e+6);
							//strcat(halflife, " Myr");
							get_amulet_message(L_MYR,message);    // " Myr"
						}else{
							//strcat(halflife, "yr");
							get_amulet_message(L_YR3,message);    // "yr"
							
						}
						break;
				}

				strcat(halflife,message);

				trim_and_shrink(nucname);
				strcpy(message, nucname);
				strcat(message, ", ");
				strcat(message, halflife);
				//SetAmuletString(101, message);
				send_to_amulet_string(101,message);
			}else{
				complete = 0;
			}

			if(AmuletDoseTableMenu_activity!=0){
				// Removed:
				//GetExtendedActivityInfo(AmuletDoseTableMenu_activity, AmuletDoseTableMenu_activityUnit, message);
				format_activity_system(AmuletDoseTableMenu_activity, message);
				send_to_amulet_string(102, message);
			}else{
				complete = 0;
			}

			if(AmuletDoseTableMenu_volume!=0){
				sprintf(message, "%.3f ml", AmuletDoseTableMenu_volume);
				send_to_amulet_string(103, message);
			}else{
				complete = 0;
			}

			if(AmuletDoseTableMenu_dose!=0){
				// Removed:
				//GetExtendedActivityInfo(AmuletDoseTableMenu_dose, AmuletDoseTableMenu_doseUnit, message);
				format_activity_system(AmuletDoseTableMenu_dose, message);
				send_to_amulet_string(104, message);
			}else{
				complete = 0;
			}

			if(AmuletDoseTableMenu_interval!=0){
				sprintf(message, "%d min", AmuletDoseTableMenu_interval);
				send_to_amulet_string(105, message);
			}else{
				complete = 0;
			}

			if(complete){
				read_clock(&AmuletDoseTableMenu_nowtime);
				AmuletDoseTableMenu_nowactivity = nucdecay(AmuletDoseTableMenu_activity, AmuletDoseTableMenu_timestamp, AmuletDoseTableMenu_nowtime, NuclideData_getHalflife(AmuletDoseTableMenu_nuclideID), NuclideData_getHalflifeUnit(AmuletDoseTableMenu_nuclideID));
				if(AmuletDoseTableMenu_dose > AmuletDoseTableMenu_nowactivity){
					AmuletDoseTableMenu_dose = 0;
					//Amulet_DisplayError("Dose Table Error", "Dose can not be larger than initial Activity.",TRUE);
					get_amulet_message(L_DOSE_TABLE_ERROR,message);    // "Dose Table Error"
					get_amulet_message(L_DOSE_LARGER_THAN_ACTIVITY,err_message);    // "Dose can not be larger than initial Activity."
					Amulet_DisplayError(message,err_message,TRUE);
					return;
				}else{
					reporttime = AmuletDoseTableMenu_nowtime;
					for(index=0; index<30; index++){
						calcvol[index] = -999;
						futuretime[index] = (time_t) 0;
					}
					for(index=0; index<30; index++){
						futuretime[index] = reporttime;
						timeout(timestring, &reporttime);
						activity = nucdecay(AmuletDoseTableMenu_nowactivity, AmuletDoseTableMenu_nowtime, reporttime, NuclideData_getHalflife(AmuletDoseTableMenu_nuclideID), NuclideData_getHalflifeUnit(AmuletDoseTableMenu_nuclideID));
						volume = AmuletDoseTableMenu_volume * AmuletDoseTableMenu_dose / activity;
						calcvol[index] = volume;
						if(volume > AmuletDoseTableMenu_volume){
							calcvol[index] = -999;
							strcpy(message, timestring);
							//strcat(message, " - Exceeds Initial");
							get_amulet_message(L_EXCEEDS_INITIAL,err_message);    // " - Exceeds Initial"
							strcat(message,err_message);
							send_to_amulet_string(111+index, message);
							//SetAmuletString(111+index, message);
							break;
						}else{
							sprintf(volumestring, "%.2f ml", volume);
						}
						strcpy(message, timestring);
						strcat(message, " - ");
						strcat(message, volumestring);
						send_to_amulet_string(111 + index, message);
						reporttime += (60 * AmuletDoseTableMenu_interval);
					}
					SetAmuletByte(101, 0xFF);
					//if(current.printer != NONE_PRINTER) SetAmuletByte(102, 0xFF);
					if((current.printer != NONE_PRINTER)&& (current.printer != USB_EPS_LABEL_PRINTER)){
						delayloop(100);
						SetAmuletByte(102, 0xFF);
					}
				}
			}

			SetAmuletByte(92,current.language);
			
			send_amulet_message(L_DOSE_TABLE,150);    // "Dose Table"
			send_amulet_message(L_NUCLIDE,152);    // "Nuclide"
			send_amulet_message(L_ACTIVITY,153);    // "Activity"
			send_amulet_message(L_DOSE_TABLE_VOLUME,154);    // "Volume"
			send_amulet_message(L_DOSE,155);    // "Dose"
			send_amulet_message(L_INTERVAL,156);    // "Interval"
			send_amulet_message(L_MEASURE_ACTIVITY,157);    // "Measure Activity"
			send_amulet_message(L_PLEASE_ENTER_VOLUME,159);    // "Please Enter Volume(ml):"
			send_amulet_message(L_PLEASE_ENTER_DOSE,161);    // "Please Enter Dose"
			send_amulet_message(L_ENTER_INTERVAL,163);    // "Enter Interval in minutes:"
			send_amulet_message(L_PLEASE_SELECT_NUCLIDE,165);    // "Please Select Nuclide"

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_DOSE_TABLE_WAIT;
			break;

		case PHASE_DOSE_TABLE_WAIT:
			break;

		case PHASE_DOSE_TABLE_PRINT:
			beep_amulet();
			Amulet_printDoseTable(AmuletDoseTableMenu_nuclideID, AmuletDoseTableMenu_activity, AmuletDoseTableMenu_dose, AmuletDoseTableMenu_volume, AmuletDoseTableMenu_timestamp, futuretime, calcvol);
			SetAmuletByte(103, 0xFF);
			m_iPhase = PHASE_DOSE_TABLE_WAIT;
			break;
	}
}

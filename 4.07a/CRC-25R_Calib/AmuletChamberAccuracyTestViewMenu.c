/**
 * \file
 * \details This screen handles calls from the Amulet Accuracy Test View Screen
 */
#define PHASE_CHAMBER_ACCURACY_VIEW_PRE_INIT	0
#define PHASE_CHAMBER_ACCURACY_VIEW_WAIT		1
#define PHASE_CHAMBER_ACCURACY_VIEW_PRINT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "message.h"
#include "printer.h"

extern int m_iPhase;
extern CURRENT current;
extern CHAMBERDAILYTEST				AmuletChamberSearchMenu_DailyTest;
extern CHAMBERACCURACYTEST			AmuletChamberSearchMenu_AccuracyTest;
extern CHAMBERACCURACYMEASUREMENT	AmuletChamberSearchMenu_AccuracyMeasurement[6];
extern CHAMBERAUTOCONSTANCY		AmuletChamberSearchMenu_AutoConstancy[12];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void AmuletChamberSearch_SerialNumberString(char *serial_number, char *output);
void AmuletChamberSearch_DateString(time_t datetime, char *output);
void AmuletChamberSearch_DescString(int chamber_type, bool two_stage, char *output);
void AmuletChamberSearch_InactiveString(char *inactive_reason, char *output);
bool DB_CheckDailyTestForAccuracyID(long long int TestID);
void prdaily_db(void);
void praccuracy_db(void);

/**
 * \details Handles the Amulet Screen ChamberAccuracyTestView.htm. ChamberAccuracyTestView.htm shows an accuracy test from the database.
 * \param Amulet_Byte_ID
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 80 Language (STATE) English = 0, Spanish = 1
 * \param 101 Show Test1 (TOGGLE)
 * \param 102 Show Test2 (TOGGLE)
 * \param 103 Show Test3 (TOGGLE)
 * \param 104 Show Test4 (TOGGLE)
 * \param 105 Show Test5 (TOGGLE)
 * \param 106 Show Test6 (TOGGLE)
 * \param 107 Autoconstancy Button (STATE)
 * \param 108 Print Button (STATE)
 * \param 109 Show Buttons (TOGGLE)
 * \param 110 Inactivate Button (STATE) FF = Inactivate Button, FE = Inactivate Reason
 * \param 111 Display Test1 Red (STATE)
 * \param 112 Display Test2 Red (STATE)
 * \param 113 Display Test3 Red (STATE)
 * \param 114 Display Test4 Red (STATE)
 * \param 115 Display Test5 Red (STATE)
 * \param 116 Display Test5 Red (STATE)
 * \param Amulet_String_ID Description
 * \param 100_101 Title
 * \param 102 Chamber Description
 * \param 103 Chamber Serial Number
 * \param 104 Date
 * \param 105 S/N Label
 * \param 106 Calc Label
 * \param 107 Meas Label
 * \param 108 Deviation Label
 * \param 109 Nuclide 1
 * \param 110 S/N 1
 * \param 111 Calc 1
 * \param 112 Meas 1
 * \param 113 Dev 1
 * \param 114 Nuclide 2
 * \param 115 S/N 2
 * \param 116 Calc 2
 * \param 117 Meas 2
 * \param 118 Dev 2
 * \param 119 Nuclide 3
 * \param 120 S/N 3
 * \param 121 Calc 3
 * \param 122 Meas 3
 * \param 123 Dev 3
 * \param 124 Nuclide 4
 * \param 125 S/N 4
 * \param 126 Calc 4
 * \param 127 Meas 4
 * \param 128 Dev 4
 * \param 129 Nuclide 5
 * \param 130 S/N 5
 * \param 131 Calc 5
 * \param 132 Meas 5
 * \param 133 Dev 5
 * \param 134 Nuclide 6
 * \param 135 S/N 6
 * \param 136 Calc 6
 * \param 137 Meas 6
 * \param 138 Dev 6
 * \param 139 Autoconstancy
 * \param 140 Inactivate
 * \param 141_142 Inactivate Text
 * \returns None
 */
void AmuletChamberAccuracyTestView_menu(void){
	int i, j;
	char acMsg[100];

	switch(m_iPhase){
		case PHASE_CHAMBER_ACCURACY_VIEW_PRE_INIT:
			SetAmuletByte(80, current.language);

			send_amulet_message(L_ACCURACY, 100);    // "Accuracy"
			delayloop(2);

			AmuletChamberSearch_DescString(AmuletChamberSearchMenu_AccuracyTest.ChamberType, AmuletChamberSearchMenu_AccuracyTest.TwoStageChamber, acMsg);
			send_to_amulet_string(102, acMsg);
			delayloop(2);

			AmuletChamberSearch_SerialNumberString(AmuletChamberSearchMenu_AccuracyTest.ChamberSerialNumber, acMsg);
			send_to_amulet_string(103, acMsg);
			delayloop(2);

			AmuletChamberSearch_DateString(AmuletChamberSearchMenu_AccuracyTest.CreatedOn, acMsg);
			send_to_amulet_string(104, acMsg);
			delayloop(2);

			//send_amulet_message(L_SN2, 105);    // "S/N:"
			get_amulet_message(L_SN2, acMsg);    // "S/N:"
			acMsg[strlen(acMsg) - 1] = 0;
			send_to_amulet_string(105, acMsg);
			delayloop(2);

			//send_amulet_message(L_CALCULATED, 106);    // "Calculated:"
			get_amulet_message(L_CALCULATED, acMsg);    // "Calculated:"
			acMsg[strlen(acMsg) - 1] = 0;
			send_to_amulet_string(106, acMsg);
			delayloop(2);

			send_amulet_message(L_MEASURED3, 107);    // "Measured"
			//get_amulet_message(L_MEASURED, acMsg);    // "Measured:"
			//acMsg[strlen(acMsg) - 1] = 0;
			//send_to_amulet_string(107, acMsg);
			delayloop(2);

			//send_amulet_message(L_DEVIATION, 108);    // "Deviation:"
			get_amulet_message(L_DEVIATION, acMsg);    // "Deviation:"
			acMsg[strlen(acMsg) - 1] = 0;
			send_to_amulet_string(108, acMsg);
			delayloop(2);

			if(AmuletChamberSearchMenu_AutoConstancy[0].ChamberAutoConstancyID != 0){
				send_amulet_message(L_AUTOCONSTANCY, 139);    // "AutoConstancy"
				SetAmuletByte(107, 0xFF);
			}
			delayloop(2);

			if(AmuletChamberSearchMenu_AccuracyTest.Inactive){
				AmuletChamberSearch_InactiveString(AmuletChamberSearchMenu_AccuracyTest.InactiveReason, acMsg);
				send_to_amulet_string(141, acMsg);
				SetAmuletByte(110, 0xFE);
			}else{
				if(AmuletChamberSearchMenu_DailyTest.ChamberDailyTestID == 0){
					if(!DB_CheckDailyTestForAccuracyID(AmuletChamberSearchMenu_AccuracyTest.ChamberAccuracyTestID)){
						send_amulet_message(L_INACTIVATE, 140);    // "Inactivate"
						SetAmuletByte(110, 0xFF);
					}
				}
			}
			delayloop(2);

			if(current.printer != NONE_PRINTER && current.printer != USB_EPS_LABEL_PRINTER) SetAmuletByte(108, 0xFF);

			SetAmuletByte(20, 0xFF);

			j = 0;
			for(i=0; i<6; i++){
				if(AmuletChamberSearchMenu_AccuracyMeasurement[i].ChamberAccuracyMeasurementID > 0){
					send_to_amulet_string((5*j) + 109, AmuletChamberSearchMenu_AccuracyMeasurement[i].SourceNuclide);
					delayloop(2);
					send_to_amulet_string((5*j) + 110, AmuletChamberSearchMenu_AccuracyMeasurement[i].SourceSerialNumber);
					delayloop(2);
					if(current.language == ENGLISH){
						send_to_amulet_string((5*j) + 111, AmuletChamberSearchMenu_AccuracyMeasurement[i].CalculatedActivityTextEnglish);
						delayloop(2);
						send_to_amulet_string((5*j) + 112, AmuletChamberSearchMenu_AccuracyMeasurement[i].MeasuredActivityTextEnglish);
						delayloop(2);
						send_to_amulet_string((5*j) + 113, AmuletChamberSearchMenu_AccuracyMeasurement[i].DeviationTextEnglish);
						delayloop(2);
					}else if(current.language == SPANISH){
						send_to_amulet_string((5*j) + 111, AmuletChamberSearchMenu_AccuracyMeasurement[i].CalculatedActivityTextSpanish);
						delayloop(2);
						send_to_amulet_string((5*j) + 112, AmuletChamberSearchMenu_AccuracyMeasurement[i].MeasuredActivityTextSpanish);
						delayloop(2);
						send_to_amulet_string((5*j) + 113, AmuletChamberSearchMenu_AccuracyMeasurement[i].DeviationTextSpanish);
						delayloop(2);
					}
					if(AmuletChamberSearchMenu_AccuracyMeasurement[i].DeviationStatus) SetAmuletByte(111 + j, 0xFF);
					delayloop(10);
					SetAmuletByte(101 + j, 0xFF);
					delayloop(2);
					j++;
				}
			}

			m_iPhase = PHASE_CHAMBER_ACCURACY_VIEW_WAIT;
			break;

		case PHASE_CHAMBER_ACCURACY_VIEW_WAIT:
			break;

		case PHASE_CHAMBER_ACCURACY_VIEW_PRINT:
			beep_amulet();
			if(AmuletChamberSearchMenu_DailyTest.ChamberDailyTestID == 0){
				praccuracy_db();
			}else{
				prdaily_db();
			}
			SetAmuletByte(109, 0xFF);
			m_iPhase = PHASE_CHAMBER_ACCURACY_VIEW_WAIT;
			break;
	}
}

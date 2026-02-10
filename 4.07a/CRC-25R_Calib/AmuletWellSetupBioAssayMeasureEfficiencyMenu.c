#define PHASE_WELLSETUPBIOASSAYMEASUREEFF_PRE_INIT	0
#define PHASE_WELLSETUPBIOASSAYMEASUREEFF_WAIT		1
#define PHASE_WELLSETUPBIOASSAYMEASUREEFF_SAVE		2
#define PHASE_WELLSETUPBIOASSAYMEASUREEFF_CLEAR131	3
#define PHASE_WELLSETUPBIOASSAYMEASUREEFF_CLEAR125	4
#define PHASE_WELLSETUPBIOASSAYMEASUREEFF_CLEAR123	5

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "amulet.h"
#include "bioassay.h"
#include "mca.h"
#include "nuc.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;

PROBEBIOASSAYEFFICIENCY AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff;
bool AmuletWellSetupBioAssayMeasureEfficiencyMenu_backgroundMeasured;
int AmuletWellSetupBioAssayMeasureEfficiencyMenu_countTime;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);

void AmuletWellSetupBioAssayMeasureEfficiency_menu(void){
	float halfLife, activity, dpm;
	short halfLifeUnit, nucIndex;
	char message[26];

	switch(m_iPhase){
		case PHASE_WELLSETUPBIOASSAYMEASUREEFF_PRE_INIT:
			if(m_ucClear == 82){
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_backgroundMeasured = FALSE;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity = -1.0;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Date = (time_t) 0;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM = -1.0;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131SpectraID = -1;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Spectrum.SpectraID = -1;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity = -1.0;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Date = (time_t) 0;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM = -1.0;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125SpectraID = -1;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Spectrum.SpectraID = -1;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity = -1.0;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Date = (time_t) 0;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM = -1.0;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123SpectraID = -1;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Spectrum.SpectraID = -1;
				AmuletWellSetupBioAssayMeasureEfficiencyMenu_countTime = BioAssayCountTime();
				m_ucClear = 0;
			}
			Mca_switchToProbe();

			if(AmuletWellSetupBioAssayMeasureEfficiencyMenu_backgroundMeasured){
				SetAmuletByte(80, 0xFF);
				if(current.system == CI) SetAmuletString(101, "Counts(cpm)");
				else SetAmuletString(101, "Counts(cps)");

				if(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity == -1.0){
					message[0] = 0;
				}else{
					if(current.system == CI) format_activity_system2(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity, message);
					else format_activity_system_kbq(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity, message);
				}
				SetAmuletString(102, message);

				if(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Date == (time_t) 0){
					message[0] = 0;
				}else{
					GetExtendedTimeInfo(&AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Date, message);
				}
				SetAmuletString(103, message);

				if(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM == -1.0){
					message[0] = 0;
				}else{
					if(current.system == CI) sprintf(message, "%.0f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM);
					else sprintf(message, "%.0f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM / 60.0);
				}
				SetAmuletString(104, message);

				if(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity == -1.0){
					message[0] = 0;
				}else{
					if(current.system == CI) format_activity_system2(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity, message);
					else format_activity_system_kbq(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity, message);
				}
				SetAmuletString(107, message);

				if(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Date == (time_t) 0){
					message[0] = 0;
				}else{
					GetExtendedTimeInfo(&AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Date, message);
				}
				SetAmuletString(108, message);

				if(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM == -1.0){
					message[0] = 0;
				}else{
					if(current.system == CI) sprintf(message, "%.0f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM);
					else sprintf(message, "%.0f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM / 60.0);
				}
				SetAmuletString(109, message);

				if(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity == -1.0){
					message[0] = 0;
				}else{
					if(current.system == CI) format_activity_system2(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity, message);
					else format_activity_system_kbq(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity, message);
				}
				SetAmuletString(111, message);

				if(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Date == (time_t) 0){
					message[0] = 0;
				}else{
					GetExtendedTimeInfo(&AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Date, message);
				}
				SetAmuletString(112, message);

				if(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM == -1.0){
					message[0] = 0;
				}else{
					if(current.system == CI) sprintf(message, "%.0f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM);
					else sprintf(message, "%.0f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM / 60.0);
				}
				SetAmuletString(113, message);

				SetAmuletByte(81, 0xFF);

				if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity != -1.0) && (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Date != (time_t) 0) && (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM > 0)){
					nucIndex = NuclideData_getIndexFromName("I 131");
					halfLife = NuclideData_getHalflife(nucIndex);
					halfLifeUnit = NuclideData_getHalflifeUnit(nucIndex);
					activity = nucdecay(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Date, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Spectrum.MeasuredOn, halfLife, halfLifeUnit);
					dpm = Mca_convertCiToDpm(activity);
					if(dpm<1){
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Efficiency = -1.0;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125Contamination = -1.0;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Date = (time_t) 0;
						Amulet_DisplayError("BioAssay Efficiency", "Calculated I131 Activity is too low\nPlease check calibration date.", TRUE);
						return;
					}else{
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Efficiency = 100.0 * AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131NetROICPM / dpm;
						sprintf(message, "%.3f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Efficiency);
						SetAmuletString(105, message);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125Contamination = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125NetROICPM / AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131NetROICPM;
						sprintf(message, "%.5f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125Contamination);
						SetAmuletString(106, message);
						SetAmuletByte(82, 0xFF);
					}
				}else{
					AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Efficiency = -1.0;
					AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125Contamination = -1.0;
				}
				if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Date != (time_t) 0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM != -1.0))  SetAmuletByte(83, 0xFF);

				if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity != -1.0) && (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Date != (time_t) 0) && (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM > 0)){
					nucIndex = NuclideData_getIndexFromName("I 125");
					halfLife = NuclideData_getHalflife(nucIndex);
					halfLifeUnit = NuclideData_getHalflifeUnit(nucIndex);
					activity = nucdecay(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Date, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Spectrum.MeasuredOn, halfLife, halfLifeUnit);
					dpm = Mca_convertCiToDpm(activity);
					if(dpm < 1){
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Efficiency = -1.0;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Date = (time_t) 0;
						Amulet_DisplayError("BioAssay Efficiency", "Calculated I125 Activity is too low\nPlease check calibration date.", TRUE);
					}else{
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Efficiency = 100.0 * AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125NetROICPM / dpm;
						sprintf(message, "%.3f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Efficiency);
						SetAmuletString(110, message);
						SetAmuletByte(84, 0xFF);
					}
				}else{
					AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Efficiency = -1.0;
				}
				if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Date != (time_t) 0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM != -1.0))  SetAmuletByte(85, 0xFF);

				if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity != -1.0) && (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Date != (time_t) 0) && (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM > 0)){
					nucIndex = NuclideData_getIndexFromName("I 123");
					halfLife = NuclideData_getHalflife(nucIndex);
					halfLifeUnit = NuclideData_getHalflifeUnit(nucIndex);
					activity = nucdecay(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Date, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Spectrum.MeasuredOn, halfLife, halfLifeUnit);
					dpm = Mca_convertCiToDpm(activity);
					if(dpm < 1){
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Efficiency = -1.0;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125Contamination = -1.0;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Date = (time_t) 0;
						Amulet_DisplayError("BioAssay Efficiency", "Calculated I123 Activity is too low\nPlease check calibration date.", TRUE);
					}else{
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Efficiency = 100.0 * AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123NetROICPM / dpm;
						sprintf(message, "%.3f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Efficiency);
						SetAmuletString(114, message);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125Contamination = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125NetROICPM / AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123NetROICPM;
						sprintf(message, "%.5f", AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125Contamination);
						SetAmuletString(115, message);
						SetAmuletByte(86, 0xFF);
					}
				}else{
					AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Efficiency = -1.0;
					AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125Contamination = -1.0;
				}
				if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Date != (time_t) 0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM != -1.0)) SetAmuletByte(87, 0xFF);

				if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Efficiency != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Efficiency != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Efficiency != -1.0)) SetAmuletByte(89, 0xFF);
			}else{
				SetAmuletByte(80, 0xFF);
				SetAmuletByte(88, 0xFF);
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLSETUPBIOASSAYMEASUREEFF_WAIT;
			break;

		case PHASE_WELLSETUPBIOASSAYMEASUREEFF_WAIT:
			break;

		case PHASE_WELLSETUPBIOASSAYMEASUREEFF_SAVE:
			if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Efficiency == -1.0) && ((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Date != (time_t) 0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM != -1.0))){
				Amulet_DisplayError("BioAssay Efficiency", "Please complete I131 measurement", TRUE);
				return;
			}

			if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Efficiency == -1.0) && ((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Date != (time_t) 0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM != -1.0))){
				Amulet_DisplayError("BioAssay Efficiency", "Please complete I125 measurement", TRUE);
				return;
			}

			if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Efficiency == -1.0) && ((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Date != (time_t) 0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM != -1.0))){
				Amulet_DisplayError("BioAssay Efficiency", "Please complete I123 measurement", TRUE);
				return;
			}

			BioAssaySaveEfficiency(&AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff, FALSE);
			SetAmuletBackHTML();
			break;

		case PHASE_WELLSETUPBIOASSAYMEASUREEFF_CLEAR131:
			beep_amulet();
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity = -1.0;
			SetAmuletString(102, "");
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Date = (time_t) 0;
			SetAmuletString(103, "");
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM = -1.0;
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Efficiency = -1.0;
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125Contamination = -1.0;
			SetAmuletString(104, "");
			SetAmuletString(105, "");
			SetAmuletString(106, "");
			SetAmuletByte(82, 0x00);
			SetAmuletByte(83, 0x00);
			SetAmuletByte(101, 0xFF);
			if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Efficiency != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Efficiency != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Efficiency != -1.0)){
				SetAmuletByte(89, 0xFF);
				SetAmuletByte(104, 0xFF);
			}else{
				SetAmuletByte(89, 0x00);
				SetAmuletByte(105, 0xFF);
			}
			m_iPhase = PHASE_WELLSETUPBIOASSAYMEASUREEFF_WAIT;
			break;

		case PHASE_WELLSETUPBIOASSAYMEASUREEFF_CLEAR125:
			beep_amulet();
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity = -1.0;
			SetAmuletString(107, "");
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Date = (time_t) 0;
			SetAmuletString(108, "");
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM = -1.0;
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Efficiency = -1.0;
			SetAmuletString(109, "");
			SetAmuletString(110, "");
			SetAmuletByte(84, 0x00);
			SetAmuletByte(85, 0x00);
			SetAmuletByte(102, 0xFF);
			if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Efficiency != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Efficiency != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Efficiency != -1.0)){
				SetAmuletByte(89, 0xFF);
				SetAmuletByte(104, 0xFF);
			}else{
				SetAmuletByte(89, 0x00);
				SetAmuletByte(105, 0xFF);
			}
			m_iPhase = PHASE_WELLSETUPBIOASSAYMEASUREEFF_WAIT;
			break;

		case PHASE_WELLSETUPBIOASSAYMEASUREEFF_CLEAR123:
			beep_amulet();
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity = -1.0;
			SetAmuletString(111, "");
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Date = (time_t) 0;
			SetAmuletString(112, "");
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM = -1.0;
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Efficiency = -1.0;
			AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125Contamination = -1.0;
			SetAmuletString(113, "");
			SetAmuletString(114, "");
			SetAmuletString(115, "");
			SetAmuletByte(86, 0x00);
			SetAmuletByte(87, 0x00);
			SetAmuletByte(103, 0xFF);
			if((AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Efficiency != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Efficiency != -1.0) || (AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Efficiency != -1.0)){
				SetAmuletByte(89, 0xFF);
				SetAmuletByte(104, 0xFF);
			}else{
				SetAmuletByte(89, 0x00);
				SetAmuletByte(105, 0xFF);
			}
			m_iPhase = PHASE_WELLSETUPBIOASSAYMEASUREEFF_WAIT;
			break;
	}
}

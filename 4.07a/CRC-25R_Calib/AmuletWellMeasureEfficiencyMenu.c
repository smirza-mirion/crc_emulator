#define PHASE_WELLMEASUREEFFICIENCY_PRE_INIT	0
#define PHASE_WELLMEASUREEFFICIENCY_WAIT		1
#define PHASE_WELLMEASUREEFFICIENCY_SAVE		2
#define PHASE_WELLMEASUREEFFICIENCY_PRINT		3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "nuc.h"
#include "mca.h"
#include "amulet.h"
#include "printer.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern SETUP_EFFICIENCY AmuletWellEditEfficiencies_temp;
extern NUCDATA AmuletWellAddEditUserNuclides_temp;
extern CURRENT current;
extern time_t clock_time;

void GetExtendedNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void SetAmuletBackHTML(void);
void AmuletWellMeasureEfficiency_print(char *nuclidename, float halflife, short hlunit, float primary_peak, time_t calib_on, float calib_activity, float current_dpm, time_t measured_on, float measured_cpm, float efficiency);

time_t AmuletWellMeasureEfficiency_calibrationStamp;
float AmuletWellMeasureEfficiency_calibrationActivity;
DB_SPEC AmuletWellMeasureEfficiency_spectrum;
float AmuletWellMeasureEfficiency_effectiveEnergy;
short AmuletWellMeasureEfficiency_mode = 0;

void AmuletWellMeasureEfficiency_menu(void){
	static char nuclide[51], nuclidename[51];
	static NUCDATA nuclidedata;
	static short hlunit;
	static float halflife;
	char message[76], message2[76];
	char calstatus[51], calstatus2[51];
	char bkgstatus[51], bkgstatus2[51];
	short index;
	static float cpm, activity, dpm;
	static float efficiency;
	//static float effectiveEnergy;
	time_t stamp;
	ENERGYPEAK energyPeak[10];

	switch(m_iPhase){
		case PHASE_WELLMEASUREEFFICIENCY_PRE_INIT:

			switch(AmuletWellMeasureEfficiency_mode){
				case MEASURE_EFF_EDIT_PROBE:
				case MEASURE_EFF_ADD_PROBE:
					Mca_switchToProbe();
					break;

				case MEASURE_EFF_EDIT_WELL:
				case MEASURE_EFF_ADD_WELL:
					Mca_switchToWell();
					break;
			}

			if(m_ucClear == 26){
				if(AmuletWellMeasureEfficiency_mode == MEASURE_EFF_EDIT_PROBE || AmuletWellMeasureEfficiency_mode == MEASURE_EFF_EDIT_WELL){
					GetExtendedNuclideInfo(AmuletWellEditEfficiencies_temp.nuclideID, &nuclidedata, nuclide);
					strcpy(nuclidename, nuclidedata.name);
					halflife = nuclidedata.halflife;
					hlunit = nuclidedata.hlunit;
					if(AmuletWellEditEfficiencies_temp.userEnergy1 == -1.0){
						AmuletWellMeasureEfficiency_effectiveEnergy = nuclidedata.energy1;
					}else{
						AmuletWellMeasureEfficiency_effectiveEnergy = AmuletWellEditEfficiencies_temp.userEnergy1;
					}
				}else{
					strcpy(nuclide, AmuletWellAddEditUserNuclides_temp.name);
					strcpy(nuclidename, AmuletWellAddEditUserNuclides_temp.name);
					AmuletWellMeasureEfficiency_effectiveEnergy = AmuletWellAddEditUserNuclides_temp.energy1;
					halflife = AmuletWellAddEditUserNuclides_temp.halflife;
					hlunit = AmuletWellAddEditUserNuclides_temp.hlunit;
				}

				if(AmuletWellMeasureEfficiency_effectiveEnergy == 0.0){
					DISPLAY_ERROR("Measure Eff Error", "Unable to measure efficiency without Energy1\nPlease enter a value for Energy1");
					SetAmuletBackHTML();
					return;

				}else if(hlunit == -1){
					DISPLAY_ERROR("Measure Eff Error", "Unable to measure efficiency without Halflife\nPlease enter a value for Halflife");
					SetAmuletBackHTML();
					return;
				}else{
					AmuletWellMeasureEfficiency_calibrationStamp = (time_t) 0;
					AmuletWellMeasureEfficiency_calibrationActivity = -999;
					for(index=0; index<4096; index++) AmuletWellMeasureEfficiency_spectrum.DecompressedSpectra[index] = 0;
					AmuletWellMeasureEfficiency_spectrum.LiveTime = 0;
					AmuletWellMeasureEfficiency_spectrum.RealTime = 0;
					AmuletWellMeasureEfficiency_spectrum.MeasuredOn = (time_t) 0;
					efficiency = 0;
					m_ucClear = 0;
				}
			}
			Mca_sendDetectorType();

			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER)) SetAmuletByte(90, 0xFF);

			SetAmuletString(100, nuclide);

			if(AmuletWellMeasureEfficiency_calibrationStamp == (time_t)0){
			    message[0] = 0;
			}else{
			    GetExtendedTimeInfo(&AmuletWellMeasureEfficiency_calibrationStamp, message);
			}
			SetAmuletString(102, message);

			if(AmuletWellMeasureEfficiency_calibrationActivity == -999){
				message[0] = 0;
			}else{
				if(current.system == CI) format_activity_system2(AmuletWellMeasureEfficiency_calibrationActivity, message);
				else format_activity_system_kbq(AmuletWellMeasureEfficiency_calibrationActivity, message);
			}
			SetAmuletString(104, message);

			message[0] = 0;
			calstatus[0] = 0;
			bkgstatus[0] = 0;
			if(!Mca_getCalibrationStatus()){
				strcat(message, "Please Auto Calibrate. ");
				strcat(calstatus, "Status: Missing Calibration");
			}else{
				if(!Mca_getCalibratedToday()){
					strcat(message, "Calibration Expired. ");
					strcat(calstatus, "<<EXPIRED>> ");
				}
				stamp = Mca_getCalibrationStamp();
				GetExtendedTimeInfoSec(&stamp, calstatus2);
				strcat(calstatus, "Calibrated on ");
				strcat(calstatus, calstatus2);
			}
			SetAmuletString(108, calstatus);

			if(!Mca_getBackgroundStatus()){
				strcat(message, "Please Run Background. ");
				strcat(bkgstatus, "Status: Missing Background");
			}else{
				if(!Mca_getBackgroundToday()){
					strcat(message, "Background Expired. ");
					strcat(bkgstatus, "<<EXPIRED>> ");
				}

				stamp = Mca_getBackgroundStamp();
				GetExtendedTimeInfoSec(&stamp, bkgstatus2);
				strcat(bkgstatus, "Measured on ");
				strcat(bkgstatus, bkgstatus2);
			}
			SetAmuletString(110, bkgstatus);
			SetAmuletString(106, message);
			if(message[0] == 0) SetAmuletByte(101, 0xFF);

			if(Mca_getCalibrationStatus() && Mca_getBackgroundStatus() && Mca_getBackgroundToday()){
				if(AmuletWellMeasureEfficiency_spectrum.MeasuredOn > Mca_getCalibrationStamp() && AmuletWellMeasureEfficiency_spectrum.MeasuredOn > Mca_getBackgroundStamp() && AmuletWellMeasureEfficiency_spectrum.LiveTime!=0.0 && AmuletWellMeasureEfficiency_calibrationStamp!=(time_t)0 && AmuletWellMeasureEfficiency_calibrationActivity!=-999){
					for(index=0; index<10; index++){
						energyPeak[index].energy = 0;
						energyPeak[index].peakindex = -1;
						energyPeak[index].startCh = -1;
						energyPeak[index].endCh = -1;
						energyPeak[index].found = FALSE;
					}

					energyPeak[0].energy = AmuletWellMeasureEfficiency_effectiveEnergy;
					if(Mca_installedDetector == DET_DRILLEDPROBE700 && (AmuletWellMeasureEfficiency_mode == MEASURE_EFF_EDIT_PROBE || AmuletWellMeasureEfficiency_mode == MEASURE_EFF_ADD_PROBE)) Mca_findPeak(AmuletWellMeasureEfficiency_spectrum.DecompressedSpectra, energyPeak, TRUE, &AmuletWellMeasureEfficiency_spectrum.AutoCal);
					else Mca_findPeak(AmuletWellMeasureEfficiency_spectrum.DecompressedSpectra, energyPeak, FALSE, &AmuletWellMeasureEfficiency_spectrum.AutoCal);

					if(energyPeak[0].found){
						cpm = Mca_getCpm(AmuletWellMeasureEfficiency_spectrum.DecompressedSpectra, energyPeak[0].startCh, energyPeak[0].endCh, AmuletWellMeasureEfficiency_spectrum.LiveTime, TRUE);
						activity = nucdecay(AmuletWellMeasureEfficiency_calibrationActivity, AmuletWellMeasureEfficiency_calibrationStamp, AmuletWellMeasureEfficiency_spectrum.MeasuredOn, halflife, hlunit);
						dpm = Mca_convertCiToDpm(activity);
						if(dpm<1){
							DISPLAY_ERROR("Measure Eff Error", "Calculated Activity is too low\nPlease check calibration date.");
						}else{
							efficiency = cpm /dpm;
							efficiency *= 100.0;
							GetExtendedTimeInfoSec(&(AmuletWellMeasureEfficiency_spectrum.MeasuredOn), bkgstatus2);
							strcpy(message, "Source measured on ");
							strcat(message, bkgstatus2);
							sprintf(message2, ", Measured efficiency = %.3f%%", efficiency);
							strcat(message, message2);
							SetAmuletString(112, message);
							SetAmuletByte(102, 0xFF);
						}
					}else{
						DISPLAY_ERROR("Measure Eff Error", "Unable to detect Energy1 Peak.");
					}
				}else{
					if(AmuletWellMeasureEfficiency_spectrum.MeasuredOn > Mca_getCalibrationStamp() && AmuletWellMeasureEfficiency_spectrum.MeasuredOn > Mca_getBackgroundStamp() && AmuletWellMeasureEfficiency_spectrum.LiveTime != 0.0){
						GetExtendedTimeInfoSec(&(AmuletWellMeasureEfficiency_spectrum.MeasuredOn), bkgstatus2);
						strcpy(message, "Source measured on ");
						strcat(message, bkgstatus2);
						SetAmuletString(112, message);
					}
				}
			}
			SetAmuletByte(100, 0xFF);

			m_iPhase = PHASE_WELLMEASUREEFFICIENCY_WAIT;
			break;

		case PHASE_WELLMEASUREEFFICIENCY_WAIT:
			break;

		case PHASE_WELLMEASUREEFFICIENCY_SAVE:
			switch(AmuletWellMeasureEfficiency_mode){
				case MEASURE_EFF_EDIT_WELL:
					if(Mca_installedDetector == DET_WELL){
						AmuletWellEditEfficiencies_temp.userWellEff = efficiency;
						AmuletWellEditEfficiencies_temp.wellStamp = AmuletWellMeasureEfficiency_spectrum.MeasuredOn;
					}else{
						AmuletWellEditEfficiencies_temp.userWell700Eff = efficiency;
						AmuletWellEditEfficiencies_temp.well700Stamp = AmuletWellMeasureEfficiency_spectrum.MeasuredOn;
					}
					break;

				case MEASURE_EFF_EDIT_PROBE:
					AmuletWellEditEfficiencies_temp.userProbe700Eff = efficiency;
					AmuletWellEditEfficiencies_temp.probe700Stamp = AmuletWellMeasureEfficiency_spectrum.MeasuredOn;
					break;

				case MEASURE_EFF_ADD_WELL:
					if(Mca_installedDetector == DET_WELL){
						AmuletWellAddEditUserNuclides_temp.wellefficiency = efficiency;
						AmuletWellAddEditUserNuclides_temp.wellmeasuredon = AmuletWellMeasureEfficiency_spectrum.MeasuredOn;
					}else{
						AmuletWellAddEditUserNuclides_temp.well700efficiency = efficiency;
						AmuletWellAddEditUserNuclides_temp.well700measuredon = AmuletWellMeasureEfficiency_spectrum.MeasuredOn;
					}
					break;

				case MEASURE_EFF_ADD_PROBE:
					AmuletWellAddEditUserNuclides_temp.probe700efficiency = efficiency;
					AmuletWellAddEditUserNuclides_temp.probe700measuredon = AmuletWellMeasureEfficiency_spectrum.MeasuredOn;
					break;
			}
			SetAmuletBackHTML();
			break;

		case PHASE_WELLMEASUREEFFICIENCY_PRINT:
			AmuletWellMeasureEfficiency_print(nuclidename, halflife, hlunit, AmuletWellMeasureEfficiency_effectiveEnergy, AmuletWellMeasureEfficiency_calibrationStamp, AmuletWellMeasureEfficiency_calibrationActivity, dpm, AmuletWellMeasureEfficiency_spectrum.MeasuredOn, cpm, efficiency);
			SetAmuletByte(104, 0xFF);
			m_iPhase = PHASE_WELLMEASUREEFFICIENCY_WAIT;
			break;
	}
}


void AmuletWellMeasureEfficiency_print(char *nuclidename, float halflife, short hlunit, float primary_peak, time_t calib_on, float calib_activity, float current_dpm, time_t measured_on, float measured_cpm, float efficiency){
	char prtype;
	char strng[90], temp[35];

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "Measure Efficiency",  clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		switch(Mca_installedDetector){

			case DET_WELL:
				insertconst(strng, 0, 0, 0, "Detector: WELL");
				break;

			case DET_WELL700:
				insertconst(strng, 0, 0, 0, "Detector: Well");
				break;

			case DET_BETA:
				insertconst(strng, 0, 0, 0, "Detector: Beta");
				break;

			case DET_PROBE700:
				insertconst(strng, 0, 0, 0, "Detector: Probe");
				break;

			case DET_DRILLEDPROBE700:
				insertconst(strng, 0, 0, 0, "Detector: Drilled Probe");
				break;
		}
		pr_write2sec(strng, "Measure Efficiency");
		pr_write2sec(strng, "Measure Efficiency");

		insertstring(strng, 0, 0, 0, "Nuclide: %s", nuclidename);
		pr_write2sec(strng, "Measure Efficiency");

		sprintf(temp, "%.2f", halflife);
		switch(hlunit){
			case SEC:
				strcat(temp, " sec");
				break;

			case MIN:
				strcat(temp, " min");
				break;

			case HOUR:
				strcat(temp, " hr");
				break;

			case DAY:
				strcat(temp, " day");
				break;

			case YEAR:
				if(halflife > 1.0e+6){
					sprintf(temp, "%.2f", halflife / 1.0e+6);
					strcat(temp, " Myr");
				}else{
					strcat(temp, " yr");
				}
				break;
		}
		insertstring(strng, 0, 0, 0, "Halflife: %s", temp);
		pr_write2sec(strng, "Measure Efficiency");

		insertfloat(strng, 0, 0, 0, "Primary: %.2f keV", primary_peak);
		pr_write2sec(strng, "Measure Efficiency");
		pr_write2sec(strng, "Measure Efficiency");

		GetExtendedTimeInfo(&calib_on ,temp);
		insertstring(strng, 0, 0, 0, "Calib On: %s", temp);
		pr_write2sec(strng, "Measure Efficiency");

		if(current.system == CI) format_activity_system2(calib_activity, temp);
		else format_activity_system_kbq(calib_activity, temp);
		replace(temp, '$', 'u');
		insertstring(strng, 0, 0, 0, "Calib Act: %s", temp);
		pr_write2sec(strng, "Measure Efficiency");

		if(current.system == CI){
			//insertfloat(strng, 0, 0, 0, "Current Act: %.0f dpm", current_dpm);
			insertfloat(strng, 0, 0, 0, "Decayed Act: %.0f dpm", current_dpm);
		}else{
			//insertfloat(strng, 0, 0, 0, "Current Act: %.0f Bq", current_dpm / 60.0);
			format_activity_system_kbq(Mca_convertDpmToCi(current_dpm), temp);
			//insertstring(strng, 0, 0, 0, "Current Act: %s", temp);
			insertstring(strng, 0, 0, 0, "Decayed Act: %s", temp);
		}
		pr_write2sec(strng, "Measure Efficiency");
		pr_write2sec(strng, "Measure Efficiency");

		GetExtendedTimeInfoSec(&measured_on, temp);
		insertstring(strng, 0, 0, 0, "Meas. On: %s", temp);
		pr_write2sec(strng, "Measure Efficiency");

		if(current.system == CI){
			//insertfloat(strng, 0, 0, 0, "Meas. Act: %.0f cpm", measured_cpm);
			insertfloat(strng, 0, 0, 0, "Measured: %.0f cpm", measured_cpm);
		}else{
			//insertfloat(strng, 0, 0, 0, "Meas. Act: %.0f cps", measured_cpm / 60.0);
			insertfloat(strng, 0, 0, 0, "Measured: %.0f cps", measured_cpm / 60.0);
		}
		pr_write2sec(strng, "Measure Efficiency");
		pr_write2sec(strng, "Measure Efficiency");

		insertfloat(strng, 0, 0, 0, "Efficiency: %.3f%%", efficiency);
		pr_write2sec(strng, "Measure Efficiency");

		formfeed(prtype);
	}
}

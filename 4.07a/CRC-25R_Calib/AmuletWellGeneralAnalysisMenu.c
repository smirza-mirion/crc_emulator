#define PHASE_WELLGENERALANALYSIS_PRE_INIT	0
#define PHASE_WELLGENERALANALYSIS_WAIT		1
#define PHASE_WELLGENERALANALYSIS_PRINT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "nuc.h"
#include "printer.h"
#include "screen.h"

void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void AmuletWellGeneralAnalysis_print(float totalcpm, float totalcps, float totalbackcpm, float totalbackcps, float totalnetcpm, float totalnetcps, char *roiregion, float roicpm, float roicps, float roibackcpm, float roibackcps, float roinetcpm, float roinetcps, float dpm, float bq);

DB_SPEC AmuletWellGeneralAnalysis_spectrum;
char AmuletWellGeneralAnalysis_generalNuclideID;
float AmuletWellGeneralAnalysis_startEV;
float AmuletWellGeneralAnalysis_endEV;

extern int m_iPhase;
extern CURRENT current;
extern int AmuletWellMeasurementMenu_generalDetectorTest;

void AmuletWellGeneralAnalysis_menu(void){
	char message[51], message2[51];
	static float totalcpm, totalcps;
	static float totalbackcpm, totalbackcps;
	static float totalnetcpm, totalnetcps;
	static char roiregion[51];
	static float roicpm, roicps;
	static float roibackcpm, roibackcps;
	static float roinetcpm, roinetcps;
	static float dpm, bq;
	short calcStart, calcEnd;

	switch(m_iPhase){
		case PHASE_WELLGENERALANALYSIS_PRE_INIT:
			GetExtendedTimeInfoSec(&(AmuletWellGeneralAnalysis_spectrum.MeasuredOn), message);
			SetAmuletString(100, message);

			sprintf(message, "%.1f sec", AmuletWellGeneralAnalysis_spectrum.LiveTime);
			SetAmuletString(102, message);

			totalcpm = Mca_getCpm(AmuletWellGeneralAnalysis_spectrum.DecompressedSpectra, 0, AmuletWellGeneralAnalysis_spectrum.NumOfChannels - 1, AmuletWellGeneralAnalysis_spectrum.LiveTime, FALSE);
			totalcps = totalcpm / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm",  totalcpm);
			else sprintf(message, "%.0f cps", totalcps);
			SetAmuletString(103, message);

			if(Mca_getBackgroundStatus()){
				totalbackcpm = Mca_getAverageBackgroundCPM(NULL);
				totalbackcps = totalbackcpm / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", totalbackcpm);
				else sprintf(message, "%.0f cps", totalbackcps);
			}else{
				totalbackcpm = -1.0;
				totalbackcps = -1.0;
				strcpy(message, "N/A");
			}
			SetAmuletString(104, message);

			if(Mca_getBackgroundStatus()){
				totalnetcpm = Mca_getCpm(AmuletWellGeneralAnalysis_spectrum.DecompressedSpectra, 0, AmuletWellGeneralAnalysis_spectrum.NumOfChannels - 1, AmuletWellGeneralAnalysis_spectrum.LiveTime, TRUE);
				totalnetcps = totalnetcpm / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", totalnetcpm);
				else sprintf(message, "%.0f cps", totalnetcps);
			}else{
				totalnetcpm = -1.0;
				totalnetcps = -1.0;
				strcpy(message, "N/A");
			}
			SetAmuletString(105, message);

			if(AmuletWellGeneralAnalysis_generalNuclideID != -1){
				SetAmuletString(106, "ROI:");
				roiregion[0] = 0;
				if(AmuletWellGeneralAnalysis_generalNuclideID >= 0){
					NuclideData_getName(AmuletWellGeneralAnalysis_generalNuclideID, roiregion);
					strcat(roiregion, ", ");

					if(AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype == DET_DRILLEDPROBE700){
						if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_WELL){
							if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_WELL700) > 0.0){
								SetAmuletString(109, "Efficiency: ");
								sprintf(message, "%.1f %%", NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_WELL700));
								SetAmuletString(110, message);
								message[0] = 0;
								SetAmuletString(119, message);
							}
						}else if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_PROBE){
							if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_PROBE700) > 0.0){
								SetAmuletString(109, "Efficiency: ");
								sprintf(message, "%.1f %%", NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_PROBE700));
								SetAmuletString(110, message);
								message[0] = 0;
								SetAmuletString(119, message);
							}
						}
					}else{
						if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype) > 0.0){
							SetAmuletString(109, "Efficiency: ");
							sprintf(message, "%.1f %%", NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype));
							SetAmuletString(110, message);
							message[0] = 0;
							SetAmuletString(119, message);
						}
					}
				}
				sprintf(message2, "%.1f - %.1f keV", AmuletWellGeneralAnalysis_startEV, AmuletWellGeneralAnalysis_endEV);
				strcat(roiregion, message2);
				SetAmuletString(107, roiregion);

				Mca_getROIChannels(AmuletWellGeneralAnalysis_startEV, AmuletWellGeneralAnalysis_endEV, &calcStart, &calcEnd, &AmuletWellGeneralAnalysis_spectrum.AutoCal);
				SetAmuletString(111, "ROI Counts:");
				roicpm = Mca_getCpm(AmuletWellGeneralAnalysis_spectrum.DecompressedSpectra, calcStart, calcEnd, AmuletWellGeneralAnalysis_spectrum.LiveTime, FALSE);
				roicps = roicpm / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", roicpm);
				else sprintf(message, "%.0f cps", roicps);
				SetAmuletString(112, message);

				SetAmuletString(113, "ROI Background:");
				if(Mca_getBackgroundStatus()){
					roibackcpm = Mca_getROIBackgroundCPM(calcStart, calcEnd, NULL);
					roibackcps = roibackcpm / 60.0;
					if(current.system == CI) sprintf(message, "%.0f cpm", roibackcpm);
					else sprintf(message, "%.0f cps", roibackcps);
				}else{
					roibackcpm = -1.0;
					roibackcps = -1.0;
					strcpy(message, "N/A");
				}
				SetAmuletString(114, message);

				SetAmuletString(115, "ROI Net Counts:");
				if(Mca_getBackgroundStatus()){
					roinetcpm = Mca_getCpm(AmuletWellGeneralAnalysis_spectrum.DecompressedSpectra, calcStart, calcEnd, AmuletWellGeneralAnalysis_spectrum.LiveTime, TRUE);
					roinetcps = roinetcpm / 60.0;
					if(current.system == CI) sprintf(message, "%.0f cpm", roinetcpm);
					else sprintf(message, "%.0f cps", roinetcps);
					SetAmuletString(116, message);

					if(AmuletWellGeneralAnalysis_generalNuclideID >= 0){
						if(AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype == DET_DRILLEDPROBE700){
							if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_WELL){
								if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_WELL700) > 0.0){
									SetAmuletString(117, "Activity:");
									dpm = roinetcpm / NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_WELL700) * 100.0;
									bq = dpm / 60.0;
									if(current.system == CI) sprintf(message, "%.0f dpm", dpm);
									else sprintf(message, "%.0f Bq", bq);
									SetAmuletString(118, message);
									message[0] = 0;
									SetAmuletString(120, message);
								}
							}else if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_PROBE){
								if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_PROBE700) > 0.0){
									SetAmuletString(117, "Activity:");
									dpm = roinetcpm / NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_PROBE700) * 100.0;
									bq = dpm / 60.0;
									if(current.system == CI) sprintf(message, "%.0f dpm", dpm);
									else sprintf(message, "%.0f Bq", bq);
									SetAmuletString(118, message);
									message[0] = 0;
									SetAmuletString(120, message);
								}
							}
						}else{
							if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype) > 0.0){
								SetAmuletString(117, "Activity:");
								dpm = roinetcpm / NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype) * 100.0;
								bq = dpm / 60.0;
								if(current.system == CI) sprintf(message, "%.0f dpm", dpm);
								else sprintf(message, "%.0f Bq", bq);
								SetAmuletString(118, message);
								message[0] = 0;
								SetAmuletString(120, message);
							}
						}
					}
				}else{
					roinetcpm = -1.0;
					roinetcps = -1.0;
					dpm = -1.0;
					bq = -1.0;
					strcpy(message, "N/A");
					SetAmuletString(116, message);
				}
			}

			switch(AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype){
				case DET_WELL:
					SetAmuletString(9, "WELL");
					break;

				case DET_WELL700:
					SetAmuletString(9, "Well");
					break;

				case DET_BETA:
					SetAmuletString(9, "Beta");
					break;

				case DET_PROBE700:
					SetAmuletString(9, "Probe");
					break;

				case DET_DRILLEDPROBE700:
					SetAmuletString(9, "Drilled Probe");
					if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_WELL) SetAmuletString(92, "As Well");
					else if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_PROBE) SetAmuletString(92, "As Probe");
					else  SetAmuletString(92, "");
					break;

				default:
					SetAmuletString(9, "");
					break;
			}
			SetAmuletByte(100, 0xFF);

			//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_WELLGENERALANALYSIS_WAIT;
			break;

		case PHASE_WELLGENERALANALYSIS_WAIT:
			break;

		case PHASE_WELLGENERALANALYSIS_PRINT:
			beep_amulet();
			AmuletWellGeneralAnalysis_print(totalcpm, totalcps, totalbackcpm, totalbackcps, totalnetcpm, totalnetcps, roiregion, roicpm, roicps, roibackcpm, roibackcps, roinetcpm, roinetcps, dpm, bq);
			SetAmuletByte(102, 0xFF);
			m_iPhase = PHASE_WELLGENERALANALYSIS_WAIT;
			break;
	}
}

void AmuletWellGeneralAnalysis_print(float totalcpm, float totalcps, float totalbackcpm, float totalbackcps, float totalnetcpm, float totalnetcps, char *roiregion, float roicpm, float roicps, float roibackcpm, float roibackcps, float roinetcpm, float roinetcps, float dpm, float bq){
	char prtype;
	char strng[90];
	char buf[40];
	short nc;
	float combinedgain1, combinedgain2, hv, hvcode;
	float calcdpm, calcbq;
	char detectortype[40];

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		g_resetlinespacing(prtype);

		rawheadersec(prtype, "GENERAL MEASUREMENT", AmuletWellGeneralAnalysis_spectrum.MeasuredOn);

		if(AmuletWellGeneralAnalysis_generalNuclideID != -1) g_print(prtype, AmuletWellGeneralAnalysis_spectrum.DecompressedSpectra, AmuletWellGeneralAnalysis_startEV, AmuletWellGeneralAnalysis_endEV, &(AmuletWellGeneralAnalysis_spectrum.AutoCal));
		else g_print(prtype, AmuletWellGeneralAnalysis_spectrum.DecompressedSpectra, -1.0, -1.0, &(AmuletWellGeneralAnalysis_spectrum.AutoCal));


		switch(AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype){
			case DET_WELL:
				strcpy(detectortype, "WELL");
				break;

			case DET_WELL700:
				strcpy(detectortype, "Well");
				break;

			case DET_BETA:
				strcpy(detectortype, "Beta");
				break;

			case DET_PROBE700:
				strcpy(detectortype, "Probe");
				break;

			case DET_DRILLEDPROBE700:
				if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_WELL) strcpy(detectortype, "Drilled Probe As Well");
				else if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_PROBE) strcpy(detectortype, "Drilled Probe As Probe");
				break;

			default:
				strcpy(detectortype, "");
				break;
		}

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%d Ch, %s", AmuletWellGeneralAnalysis_spectrum.AutoCal.num_of_channels, detectortype);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		combinedgain1 = AmuletWellGeneralAnalysis_spectrum.AutoCal.gain1;
		combinedgain2 = AmuletWellGeneralAnalysis_spectrum.AutoCal.gain2;
		combinedgain1 /= 64.0;
		combinedgain2 /= 64.0;
		combinedgain1 *= combinedgain2;

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "      Gain: %.2f", combinedgain1);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "    Offset: %d", AmuletWellGeneralAnalysis_spectrum.AutoCal.zeroopampoffset);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "    Thresh: %d", AmuletWellGeneralAnalysis_spectrum.AutoCal.threshold);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		hvcode = AmuletWellGeneralAnalysis_spectrum.AutoCal.hv;
		if((Mca_installedDetector == DET_WELL) || (Mca_installedDetector == DET_BETA)) hv = (.158974 * hvcode) + 595.0;
		else if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)) hv = .263158 * hvcode;

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "        HV: %.0f V", hv);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "       S/N: %s", AmuletWellGeneralAnalysis_spectrum.AutoCal.serialnum);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, " Real Time: %.1f sec", AmuletWellGeneralAnalysis_spectrum.RealTime);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, " Live Time: %.1f sec", AmuletWellGeneralAnalysis_spectrum.LiveTime);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(current.system == CI) nc = sprintf(buf, "Total Cnt: %.0f cpm", totalcpm);
		else nc = sprintf(buf, "Total Cnt: %.0f cps", totalcps);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(totalbackcpm >= 0.0){
			if(current.system == CI) nc = sprintf(buf, "Total Bkg: %.0f cpm", totalbackcpm);
			else  nc = sprintf(buf, "Total Bkg: %.0f cps", totalbackcps);
		}else{
			nc = sprintf(buf, "Total Bkg: N/A");
		}
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(totalnetcpm >= 0.0){
			if(current.system == CI) nc = sprintf(buf, "Net Cnt: %.0f cpm", totalnetcpm);
			else  nc = sprintf(buf, "Net Cnt: %.0f cps", totalnetcps);
		}else{
			nc = sprintf(buf, "Net Cnt: N/A");
		}
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		if(AmuletWellGeneralAnalysis_generalNuclideID != -1){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "ROI: %s", roiregion);
			strncpy(&strng[2], buf, nc);
			pr_write(strng);

			if(AmuletWellGeneralAnalysis_generalNuclideID >= 0){
				if(AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype == DET_DRILLEDPROBE700){
					if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_WELL){
						if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_WELL700) > 0.0){
							lininit(strng, TRUE, prtype);
							nc = sprintf(buf, "Efficiency: %.1f %%", NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_WELL700));
							strncpy(&strng[2], buf, nc);
							pr_write(strng);
						}
					}else if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_PROBE){
						if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_PROBE700) > 0.0){
							lininit(strng, TRUE, prtype);
							nc = sprintf(buf, "Efficiency: %.1f %%", NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_PROBE700));
							strncpy(&strng[2], buf, nc);
							pr_write(strng);
						}
					}
				}else{
					if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype) > 0.0){
						lininit(strng, TRUE, prtype);
						nc = sprintf(buf, "Efficiency: %.1f %%", NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype));
						strncpy(&strng[2], buf, nc);
						pr_write(strng);
					}
				}
			}

			lininit(strng, TRUE, prtype);
			if(current.system == CI) nc = sprintf(buf, "ROI Cnt: %.0f cpm", roicpm);
			else  nc = sprintf(buf, "ROI Cnt: %.0f cps", roicps);
			strncpy(&strng[2], buf, nc);
			pr_write(strng);

			lininit(strng, TRUE, prtype);
			if(roibackcpm >= 0.0){
				if(current.system == CI) nc = sprintf(buf, "ROI Bkg: %.0f cpm", roibackcpm);
				else  nc = sprintf(buf, "ROI Bkg: %.0f cps", roibackcps);
			}else{
				nc = sprintf(buf, "ROI Bkg: N/A");
			}
			strncpy(&strng[2], buf, nc);
			pr_write(strng);

			lininit(strng, TRUE, prtype);
			if(roinetcpm >= 0.0){
				if(current.system == CI) nc = sprintf(buf, "ROI Net: %.0f cpm", roinetcpm);
				else  nc = sprintf(buf, "ROI Net: %.0f cps", roinetcps);
			}else{
				nc = sprintf(buf, "ROI Net: N/A");
			}
			strncpy(&strng[2], buf, nc);
			pr_write(strng);

			if(dpm >= 0.0){
				if(AmuletWellGeneralAnalysis_generalNuclideID >= 0){
					if(AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype == DET_DRILLEDPROBE700){
						if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_WELL){
							if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_WELL700) > 0.0){
								lininit(strng, TRUE, prtype);
								calcdpm = roinetcpm / NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_WELL700) * 100.0;
								calcbq = calcdpm / 60.0;
								if(current.system == CI) nc = sprintf(buf, "Activity: %.0f dpm", calcdpm);
								else  nc = sprintf(buf, "Activity: %.0f Bq", calcbq);
								strncpy(&strng[2], buf, nc);
								pr_write(strng);
							}
						}else if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_PROBE){
							if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_PROBE700) > 0.0){
								lininit(strng, TRUE, prtype);
								calcdpm = roinetcpm / NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, DET_PROBE700) * 100.0;
								calcbq = calcdpm / 60.0;
								if(current.system == CI) nc = sprintf(buf, "Activity: %.0f dpm", calcdpm);
								else  nc = sprintf(buf, "Activity: %.0f Bq", calcbq);
								strncpy(&strng[2], buf, nc);
								pr_write(strng);
							}
						}
					}else{
						if(NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype) > 0.0){
							lininit(strng, TRUE, prtype);
							calcdpm = roinetcpm / NuclideData_getEffectiveEff(AmuletWellGeneralAnalysis_generalNuclideID, AmuletWellGeneralAnalysis_spectrum.AutoCal.detectortype) * 100.0;
							calcbq = calcdpm / 60.0;
							if(current.system == CI) nc = sprintf(buf, " Activity: %.0f dpm", calcdpm);
							else  nc = sprintf(buf, " Activity: %.0f Bq", calcbq);
							strncpy(&strng[2], buf, nc);
							pr_write(strng);
						}
					}
				}
			}
		}
		formfeed(prtype);
	}
}

#define PHASE_WELLWIPEREPORT_PRE_INIT	0
#define PHASE_WELLWIPEREPORT_WAIT		1
#define PHASE_WELLWIPEREPORT_PRINT		2
#define PHASE_WELLWIPEREPORT_SPECTRUM	3
#define PHASE_WELLWIPEREPORT_BACKGND	4

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "int_mca.h"
#include "nuc.h"
#include "printer.h"
#include "amulet.h"
#include "database.h"

extern int m_iPhase;
extern CURRENT current;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;
extern BACKGND temp_backgnd;

WELLWIPE AmuletWellWipeReportMenu_wellWipe;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
float Mca_convertDpmToCi(float dpm);
float Mca_convertDpmToBq(float dpm);
void PushPageStack(unsigned char ucPage);
void AmuletWellWipeReport_print(void);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void trim_and_shrink(char *acByte);

void AmuletWellWipeReport_menu(void){
	char message[100], nuclidename[10];
	float triggerdpm, triggernci, triggerbq;
	float backgroundcpm, backgroundcps;
	float totalcpm, totalcps;
	float netcpm, netcps;
	float activitydpm, activitybq, activitynci;
	short index, jndex;
	long long int WellWipeNuclideID;
	float net_cpm, net_cps, activity_dpm, activity_bq, activity_nci;

	switch(m_iPhase){
		case PHASE_WELLWIPEREPORT_PRE_INIT:
			sprintf(message, "%s", AmuletWellWipeReportMenu_wellWipe.WipeLocation.Name);
			sprintf(message, "%s (%s)", message, AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeName);
			SetAmuletString(99, message);

			triggerdpm = AmuletWellWipeReportMenu_wellWipe.WipeLocation.Threshold;
			triggernci = Mca_convertDpmToCi(triggerdpm);
			triggernci *= 1e+9;
			triggerbq = Mca_convertDpmToBq(triggerdpm);

			if(current.system == CI){
				if(AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeGroupID == 4) sprintf(message, "%.3f nCi", triggernci);
				else sprintf(message, "%.2f dpm", triggerdpm);
			}else{
				sprintf(message, "%.3f Bq", triggerbq);
			}
			SetAmuletString(101, message);

			sprintf(message, "%.1f", AmuletWellWipeReportMenu_wellWipe.Spectrum.LiveTime);
			SetAmuletString(102, message);

			message[0] = 0;
			for(index=0; index<10; index++){
				if(AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[index].WellWipeNuclideID > 0){
					strcpy(nuclidename, AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[index].Name);
					trim_and_shrink(nuclidename);
					strcat(message, nuclidename);
					strcat(message, ", ");
				}
			}
			message[strlen(message) - 2] = 0;
			SetAmuletString(90, message);

			sprintf(message, "%.3f %%", AmuletWellWipeReportMenu_wellWipe.Efficiency);
			SetAmuletString(103, message);

			GetExtendedTimeInfoSec(&AmuletWellWipeReportMenu_wellWipe.Spectrum.MeasuredOn, message);
			SetAmuletString(110, message);

			backgroundcpm = AmuletWellWipeReportMenu_wellWipe.Background.Spectrum.TotalCPM;
			backgroundcps = backgroundcpm / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", backgroundcpm);
			else sprintf(message, "%.0f cps", backgroundcps);
			SetAmuletString(104, message);

			totalcpm = AmuletWellWipeReportMenu_wellWipe.Spectrum.TotalCPM;
			totalcps = totalcpm / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", totalcpm);
			else sprintf(message, "%.0f cps", totalcps);
			SetAmuletString(105, message);

			netcpm = AmuletWellWipeReportMenu_wellWipe.NetCPM;
			netcps = netcpm / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", netcpm);
			else sprintf(message, "%.0f cps", netcps);
			SetAmuletString(106, message);

			activitydpm = AmuletWellWipeReportMenu_wellWipe.Activity;
			activitybq = Mca_convertDpmToBq(activitydpm);
			activitynci = Mca_convertDpmToCi(activitydpm);
			activitynci *= 1e+9;
			if(current.system == CI){
				if(AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeGroupID == 4){
					sprintf(message, "%.2f nCi", activitynci);
					SetAmuletString(109, "nCi");
				}else{
					sprintf(message, "%.0f dpm", activitydpm);
					SetAmuletString(109, "dpm");
				}
				SetAmuletString(108, "cpm");
			}else{
				sprintf(message, "%.0f Bq", activitybq);
				SetAmuletString(109, "Bq");
				SetAmuletString(108, "cps");
			}
			SetAmuletString(107, message);

			if(AmuletWellWipeReportMenu_wellWipe.ExceedsThreshold) SetAmuletByte(101, 0xFF);

			SetAmuletByte(100, 0xFF);

			if(AmuletWellWipeReportMenu_wellWipe.WellWipeID > 0){
				if(AmuletWellWipeReportMenu_wellWipe.Inactive){
					SetAmuletString(97, AmuletWellWipeReportMenu_wellWipe.InactiveReason);
					SetAmuletByte(147, 0xFF);
				}else{
					SetAmuletByte(146, 0xFF);
				}
			}

			for(index=0; index<14; index++){
				if(AmuletWellWipeReportMenu_wellWipe.Peaks[index].Energy >= 0.0){
					sprintf(message, "%.1f", AmuletWellWipeReportMenu_wellWipe.Peaks[index].Energy);
					SetAmuletString((4*index)+21, message);

					net_cpm = AmuletWellWipeReportMenu_wellWipe.Peaks[index].NetROICPM;
					net_cps = net_cpm / 60.0;
					if(current.system == CI) sprintf(message, "%.0f", net_cpm);
					else sprintf(message, "%.0f", net_cps);
					SetAmuletString((4*index)+22, message);

					if(AmuletWellWipeReportMenu_wellWipe.Peaks[index].PeakType == PRIMARY){
						WellWipeNuclideID = AmuletWellWipeReportMenu_wellWipe.Peaks[index].WellWipeNuclideID;
						for(jndex=0; jndex<10; jndex++){
							if(AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[jndex].WellWipeNuclideID == WellWipeNuclideID){
								SetAmuletString((4*index)+23, AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[jndex].Name);
								break;
							}
						}
						activity_dpm = AmuletWellWipeReportMenu_wellWipe.Peaks[index].Activity;
						activity_bq = Mca_convertDpmToBq(activity_dpm);
						activity_nci = Mca_convertDpmToCi(activity_dpm);
						activity_nci *= 1e+9;

						if(current.system == CI){
							if(AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeGroupID == 4) sprintf(message, "%.2f", activity_nci);
							else sprintf(message, "%.0f", activity_dpm);
						}else{
							sprintf(message, "%.0f", activity_bq);
						}
						SetAmuletString((4*index)+24, message);

						if(AmuletWellWipeReportMenu_wellWipe.Peaks[index].ExceedsThreshold) SetAmuletByte(131+index, 0xFF);
					}else{
						if(AmuletWellWipeReportMenu_wellWipe.Peaks[index].PeakType != NOT_FOUND){
							WellWipeNuclideID = AmuletWellWipeReportMenu_wellWipe.Peaks[index].WellWipeNuclideID;
							for(jndex=0; jndex<10; jndex++){
								if(AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[jndex].WellWipeNuclideID == WellWipeNuclideID){
									SetAmuletString((4*index)+23, AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[jndex].Name);
									break;
								}
							}
						}
					}

					SetAmuletByte(111 + index, 0xFF);
					delayloop(25);
				}else{
					break;
				}
			}

			//if(current.printer != NONE_PRINTER) SetAmuletByte(145, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(145, 0xFF);

			m_iPhase = PHASE_WELLWIPEREPORT_WAIT;
			break;

		case PHASE_WELLWIPEREPORT_WAIT:
		    break;

		case PHASE_WELLWIPEREPORT_PRINT:
			beep_amulet();
			AmuletWellWipeReport_print();
			SetAmuletByte(148, 0xFF);
			m_iPhase = PHASE_WELLWIPEREPORT_WAIT;
			break;

		case PHASE_WELLWIPEREPORT_SPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &AmuletWellWipeReportMenu_wellWipe.Spectrum, sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Wipe Spectrum");
			strcpy(AmuletWellSpectrumMenu_comment2, AmuletWellWipeReportMenu_wellWipe.WipeLocation.Name);
			strcpy(AmuletWellSpectrumMenu_comment3, AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeName);
			if((AmuletWellWipeReportMenu_wellWipe.WellWipeID > 0) && (AmuletWellWipeReportMenu_wellWipe.Inactive)){
				AmuletWellSpectrumMenu_inactive = TRUE;
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellWipeReportMenu_wellWipe.InactiveReason);
			}else{
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLWIPEREPORT_BACKGND:
			temp_backgnd.WellBackgroundID = AmuletWellWipeReportMenu_wellWipe.Background.WellBackgroundID;
			DB_RetrieveBackgnd(&temp_backgnd);
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(temp_backgnd.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Wipe Background");
			strcpy(AmuletWellSpectrumMenu_comment2, AmuletWellWipeReportMenu_wellWipe.WipeLocation.Name);
			strcpy(AmuletWellSpectrumMenu_comment3, AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeName);
			if((AmuletWellWipeReportMenu_wellWipe.WellWipeID > 0) && (AmuletWellWipeReportMenu_wellWipe.Inactive)){
				AmuletWellSpectrumMenu_inactive = TRUE;
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellWipeReportMenu_wellWipe.InactiveReason);
			}else{
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		//case PHASE_WELLWIPEREPORT_INACTIVATE:
		//	beep_amulet();
		//	m_ucClear = 63;
		//	SetAmuletHTML();
		//	return;
	}
}

//void AmuletWellWipeReport_print(float triggerdpm, float triggernci, float triggerbq, float backgroundcpm, float backgroundcps, float totalcpm, float totalcps, float netcpm, float netcps, float activitydpm, float activitybq, float activitynci, bool nethigh){
void AmuletWellWipeReport_print(void){
	char prtype;
	char strng[90];
	char buf[40], buf2[40], nuclidename[10];
	short nc, index, jndex;
	float triggerdpm, triggernci, triggerbq;
	float backgroundcpm, backgroundcps;
	float totalcpm, totalcps;
	float netcpm, netcps;
	float activitydpm, activitybq, activitynci;
	float net_cpm, net_cps, activity_dpm, activity_bq, activity_nci;

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){

		sprintf(buf, "%s", AmuletWellWipeReportMenu_wellWipe.WipeLocation.Name);
		sprintf(buf2, "%s", AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeName);

		g_resetlinespacing(prtype);

		rawheader2(prtype, buf, buf2, AmuletWellWipeReportMenu_wellWipe.Spectrum.MeasuredOn);

		for(jndex=0; jndex<10; jndex++){
			//if(nucIndex[jndex]>=0){
			if(AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[jndex].WellWipeNuclideID > 0){
				lininit(strng, TRUE, prtype);
				//NuclideData_getName(nucIndex[jndex], nuclidename);
				strcpy(nuclidename, AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[jndex].Name);
				trim_and_shrink(nuclidename);
				nc = sprintf(buf, "Nuclide: %s", nuclidename);
				strncpy(&strng[2], buf, nc);
				pr_write(strng);
			}
		}

		lininit(strng, TRUE, prtype);
		triggerdpm = AmuletWellWipeReportMenu_wellWipe.WipeLocation.Threshold;
		triggernci = Mca_convertDpmToCi(triggerdpm);
		triggernci *= 1e+9;
		triggerbq = Mca_convertDpmToBq(triggerdpm);

		if(current.system==CI){
			//if(AmuletWellWipeReportMenu_reportType == 2) nc = sprintf(buf, "Trigger Level: %.3f nCi", triggernci);
			if(AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeGroupID == 4) nc = sprintf(buf, "Trigger Level: %.3f nCi", triggernci);
			else nc = sprintf(buf, "Trigger Level: %.2f dpm", triggerdpm);
		}else{
			nc = sprintf(buf, "Trigger Level: %.3f Bq", triggerbq);
		}
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Count Time: %.1f sec", AmuletWellWipeReportMenu_wellWipe.Spectrum.LiveTime);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		//nc = sprintf(buf, "Full Eff: %.2f %%", Mca_getFullEfficiency());
		nc = sprintf(buf, "Full Eff: %.3f %%", AmuletWellWipeReportMenu_wellWipe.Efficiency);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		backgroundcpm = AmuletWellWipeReportMenu_wellWipe.Background.Spectrum.TotalCPM;
		backgroundcps = backgroundcpm / 60.0;
		if(current.system == CI) nc = sprintf(buf, "Background: %.0f cpm", backgroundcpm);
		else nc = sprintf(buf, "Background: %.0f cps", backgroundcps);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		totalcpm = AmuletWellWipeReportMenu_wellWipe.Spectrum.TotalCPM;
		totalcps = totalcpm / 60.0;
		lininit(strng, TRUE, prtype);
		if(current.system == CI) nc = sprintf(buf, "Total Counts: %.0f cpm", totalcpm);
		else nc = sprintf(buf, "Total Counts: %.0f cps", totalcps);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		netcpm = AmuletWellWipeReportMenu_wellWipe.NetCPM;
		netcps = netcpm / 60.0;
		lininit(strng, TRUE, prtype);
		if(current.system == CI) nc = sprintf(buf, "Net Counts: %.0f cpm", netcpm);
		else nc = sprintf(buf, "Net Counts: %.0f cps", netcps);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		activitydpm = AmuletWellWipeReportMenu_wellWipe.Activity;
		activitybq = Mca_convertDpmToBq(activitydpm);
		activitynci = Mca_convertDpmToCi(activitydpm);
		activitynci *= 1e+9;
		lininit(strng, TRUE, prtype);
		if(current.system == CI){
			if(AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeGroupID == 4) nc = sprintf(buf, "Net Activity: %.2f nCi", activitynci);
			else nc = sprintf(buf, "Net Activity: %.0f dpm", activitydpm);
		}else{
			nc = sprintf(buf, "Net Activity: %.0f Bq", activitybq);
		}
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		if(AmuletWellWipeReportMenu_wellWipe.ExceedsThreshold){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "NET ACTIVITY HIGH");
			strncpy(&strng[2], buf, nc);
			pr_write(strng);
		}

		feed(1, prtype);

		for(index=0; index<14; index++){
			//if(specpeak[index].energy >= 0.0){
			if(AmuletWellWipeReportMenu_wellWipe.Peaks[index].Energy >= 0.0){
				lininit(strng, TRUE, prtype);
				strncpy(&strng[2], "------------------------------", 30);
				pr_write(strng);

				lininit(strng, TRUE, prtype);
				nc = sprintf(buf, "Energy: %.1f keV", AmuletWellWipeReportMenu_wellWipe.Peaks[index].Energy);
				strncpy(&strng[2], buf, nc);
				pr_write(strng);

				lininit(strng, TRUE, prtype);
				net_cpm = AmuletWellWipeReportMenu_wellWipe.Peaks[index].NetROICPM;
				net_cps = net_cpm / 60.0;
				if(current.system == CI) nc = sprintf(buf, "Net Counts: %.0f cpm", net_cpm);
				else nc = sprintf(buf, "Net Counts: %.0f cps", net_cps);
				strncpy(&strng[2], buf, nc);
				pr_write(strng);

				//if(specpeak[index].type == PRIMARY){
				if(AmuletWellWipeReportMenu_wellWipe.Peaks[index].PeakType == PRIMARY){
					lininit(strng, TRUE, prtype);
					//NuclideData_getName(specpeak[index].nuclideID, buf);
					buf[0] = 0;
					for(jndex=0; jndex<10; jndex++){
						if(AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[jndex].WellWipeNuclideID == AmuletWellWipeReportMenu_wellWipe.Peaks[index].WellWipeNuclideID){
							strcpy(buf, AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[jndex].Name);
						}
					}
					strncpy(&strng[2], "Nuclide: ", 9);
					strncpy(&strng[11], buf, strlen(buf));
					pr_write(strng);

					//activity_dpm = specpeak[index].activity;
					activity_dpm = AmuletWellWipeReportMenu_wellWipe.Peaks[index].Activity;
					activity_bq = Mca_convertDpmToBq(activity_dpm);
					activity_nci = Mca_convertDpmToCi(activity_dpm);
					activity_nci *= 1e+9;

					lininit(strng, TRUE, prtype);
					if(current.system == CI){
						if(AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeGroupID == 4) nc = sprintf(buf, "Activity: %.2f nCi", activity_nci);
						else nc = sprintf(buf, "Activity: %.0f dpm", activity_dpm);
					}else{
						nc = sprintf(buf, "Activity: %.0f Bq", activity_bq);
					}
					strncpy(&strng[2], buf, nc);
					pr_write(strng);

					//if(specpeak[index].exceedthreshold){
					if(AmuletWellWipeReportMenu_wellWipe.Peaks[index].ExceedsThreshold){
						lininit(strng, TRUE, prtype);
						strncpy(&strng[2], "ACTIVITY HIGH", 13);
						pr_write(strng);
					}
				}else{
					if(AmuletWellWipeReportMenu_wellWipe.Peaks[index].PeakType != NOT_FOUND){
						lininit(strng, TRUE, prtype);
						//NuclideData_getName(specpeak[index].nuclideID, buf);
						buf[0] = 0;
						for(jndex=0; jndex<10; jndex++){
							if(AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[jndex].WellWipeNuclideID == AmuletWellWipeReportMenu_wellWipe.Peaks[index].WellWipeNuclideID){
								strcpy(buf, AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[jndex].Name);
							}
						}
						strncpy(&strng[2], "Nuclide: ", 9);
						strncpy(&strng[11], buf, strlen(buf));
						pr_write(strng);
					}
				}
			}
		}

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "------------------------------", 30);
		pr_write(strng);

		feed(1, prtype);

		if((AmuletWellWipeReportMenu_wellWipe.WellWipeID > 0) && (AmuletWellWipeReportMenu_wellWipe.Inactive)){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "INACTIVE: %s", AmuletWellWipeReportMenu_wellWipe.InactiveReason);
			strncpy(strng, buf, nc);
			pr_write(strng);
		}

		formfeed(prtype);
	}
}

/*static void DedupNuclide(char nucIndex[10]){
	short index, jndex, end;
	char newarray[10];
	bool found;

	for(index=0; index<10; index++) newarray[index] = -1;

	end = 0;
	for(index=0; index<10; index++){
		if(nucIndex[index] >= 0){
			found = FALSE;
			if(end > 0){
				for(jndex=0; jndex<end; jndex++){
					if(newarray[jndex] == nucIndex[index]){
						found = TRUE;
						break;
					}
				}
			}

			if(!found){
				newarray[end] = nucIndex[index];
				end++;
			}
		}
	}

	for(index=0; index<10; index++) nucIndex[index] = newarray[index];
}*/

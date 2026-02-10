#define PHASE_WELLAUTOCALREPORT_PRE_INIT	0
#define PHASE_WELLAUTOCALREPORT_WAIT		1
#define PHASE_WELLAUTOCALREPORT_PRINT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "amulet.h"
#include "printer.h"

extern int m_iPhase;
extern CURRENT current;

AUTOCAL AmuletWellAutoCalReportMenu_test;

void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void AmuletWellAutoCalReport_print(void);
float GetFwhm(float sigma);

void AmuletWellAutoCalReport_menu(void){
	char message[51];
	int index;
	float hvcode, hv, combinedgain1, combinedgain2, linstd[5], kev;

	switch(m_iPhase){
		case PHASE_WELLAUTOCALREPORT_PRE_INIT:
			GetExtendedTimeInfoSec(&AmuletWellAutoCalReportMenu_test.caltstamp, message);
			SetAmuletString(100, message);

			SetAmuletString(102, AmuletWellAutoCalReportMenu_test.serialnum);

			sprintf(message, "%d", AmuletWellAutoCalReportMenu_test.threshold);
			SetAmuletString(103, message);

			hvcode = AmuletWellAutoCalReportMenu_test.hv;
			if((AmuletWellAutoCalReportMenu_test.detectortype == DET_WELL) || (AmuletWellAutoCalReportMenu_test.detectortype == DET_BETA)) hv = (.158974 * hvcode) + 595.0;
			else if((AmuletWellAutoCalReportMenu_test.detectortype == DET_PROBE700) || (AmuletWellAutoCalReportMenu_test.detectortype == DET_WELL700) || (AmuletWellAutoCalReportMenu_test.detectortype == DET_DRILLEDPROBE700)) hv = .263158 * hvcode;
			sprintf(message, "%.0f Volts", hv);
			SetAmuletString(104, message);

			combinedgain1 = AmuletWellAutoCalReportMenu_test.gain1;
			combinedgain2 = AmuletWellAutoCalReportMenu_test.gain2;
			combinedgain1 /= 64.0;
			combinedgain2 /= 64.0;
			combinedgain1 *= combinedgain2;
			sprintf(message, "%.2f", combinedgain1);
			SetAmuletString(105, message);

			sprintf(message, "%d", AmuletWellAutoCalReportMenu_test.zeroopampoffset);
			SetAmuletString(106, message);

			sprintf(message, "%.3f %%", GetFwhm(AmuletWellAutoCalReportMenu_test.sigma));
			SetAmuletString(107, message);

			switch(AmuletWellAutoCalReportMenu_test.detectortype){
				case DET_WELL:
					strcpy(message, "WELL");
					break;

				case DET_WELL700:
					strcpy(message, "Well");
					break;

				case DET_BETA:
					strcpy(message, "Beta");
					break;

				case DET_PROBE700:
					strcpy(message, "Probe");
					break;

				case DET_DRILLEDPROBE700:
					strcpy(message, "Drilled Probe");
					break;
			}
			SetAmuletString(113, message);

			sprintf(message, "%d", AmuletWellAutoCalReportMenu_test.num_of_channels);
			SetAmuletString(114, message);

			if(AmuletWellAutoCalReportMenu_test.detectortype == DET_DRILLEDPROBE700){
				sprintf(message, "%.3f %%", GetFwhm(AmuletWellAutoCalReportMenu_test.sigma_lip));
				SetAmuletString(115, message);
				SetAmuletByte(103, 0xFF);
			}

			SetAmuletByte(100, 0xFF);

			if(((AmuletWellAutoCalReportMenu_test.linmeasured[0])>0.0) && ((AmuletWellAutoCalReportMenu_test.linmeasured[1])>(AmuletWellAutoCalReportMenu_test.linmeasured[0])) && ((AmuletWellAutoCalReportMenu_test.linmeasured[2])>(AmuletWellAutoCalReportMenu_test.linmeasured[1])) && ((AmuletWellAutoCalReportMenu_test.linmeasured[3])>(AmuletWellAutoCalReportMenu_test.linmeasured[2])) && ((AmuletWellAutoCalReportMenu_test.linmeasured[4])>(AmuletWellAutoCalReportMenu_test.linmeasured[3]))){
				linstd[0] = 32.85;
				linstd[1] = 40.8;
				linstd[2] = 121.8;
				linstd[3] = 344.3;
				linstd[4] = 661.66;

				for(index=0; index<5; index++){
					kev = AmuletWellAutoCalReportMenu_test.linmeasured[index] - linstd[index];
					kev /= linstd[index];
					kev *= 100.0;
					sprintf(message, "%.3f keV = %.3f %%", AmuletWellAutoCalReportMenu_test.linmeasured[index], kev);
					SetAmuletString(index + 108, message);
				}

				SetAmuletByte(102, 0xFF);
			}

			//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(101, 0xFF);

			m_iPhase = PHASE_WELLAUTOCALREPORT_WAIT;
			break;

		case PHASE_WELLAUTOCALREPORT_WAIT:
			break;

		case PHASE_WELLAUTOCALREPORT_PRINT:
			beep_amulet();
			AmuletWellAutoCalReport_print();
			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_WELLAUTOCALREPORT_WAIT;
			break;

			}
}

void AmuletWellAutoCalReport_print(void){
	char prtype;
	char strng[90];
	char buf[40];
	short nc;
	int index;
	float hvcode, hv, combinedgain1, combinedgain2, linstd[5], kev;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "Auto Calibration", AmuletWellAutoCalReportMenu_test.caltstamp);

		lininit(strng, TRUE, prtype);
		switch(AmuletWellAutoCalReportMenu_test.detectortype){
			case DET_WELL:
				nc = sprintf(buf, "Det: %s", "WELL");
				break;

			case DET_WELL700:
				nc = sprintf(buf, "Det: %s", "Well");
				break;

			case DET_BETA:
				nc = sprintf(buf, "Det: %s", "Beta");
				break;

			case DET_PROBE700:
				nc = sprintf(buf, "Det: %s", "Probe");
				break;

			case DET_DRILLEDPROBE700:
				nc = sprintf(buf, "Det: %s", "Drilled Probe");
				break;
		}
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Channels: %d", AmuletWellAutoCalReportMenu_test.num_of_channels);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Serial #: %s", AmuletWellAutoCalReportMenu_test.serialnum);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Threshold: %d", AmuletWellAutoCalReportMenu_test.threshold);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		hvcode = AmuletWellAutoCalReportMenu_test.hv;
		if((AmuletWellAutoCalReportMenu_test.detectortype == DET_WELL) || (AmuletWellAutoCalReportMenu_test.detectortype == DET_BETA)) hv = (.158974 * hvcode) + 595.0;
		else if((AmuletWellAutoCalReportMenu_test.detectortype == DET_PROBE700) || (AmuletWellAutoCalReportMenu_test.detectortype == DET_WELL700) || (AmuletWellAutoCalReportMenu_test.detectortype == DET_DRILLEDPROBE700)) hv = .263158 * hvcode;
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "HV: %.0f Volts", hv);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		combinedgain1 = AmuletWellAutoCalReportMenu_test.gain1;
		combinedgain2 = AmuletWellAutoCalReportMenu_test.gain2;
		combinedgain1 /= 64.0;
		combinedgain2 /= 64.0;
		combinedgain1 *= combinedgain2;
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Gain: %.2f", combinedgain1);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Zero: %d", AmuletWellAutoCalReportMenu_test.zeroopampoffset);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "FWHM: %.3f %%", GetFwhm(AmuletWellAutoCalReportMenu_test.sigma));
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		if(AmuletWellAutoCalReportMenu_test.detectortype == DET_DRILLEDPROBE700){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "FWHM Perimeter: %.3f %%", GetFwhm(AmuletWellAutoCalReportMenu_test.sigma_lip));
			strncpy(&strng[2], buf, nc);
			pr_write(strng);
		}

		if(((AmuletWellAutoCalReportMenu_test.linmeasured[0])>0.0) && ((AmuletWellAutoCalReportMenu_test.linmeasured[1])>(AmuletWellAutoCalReportMenu_test.linmeasured[0])) && ((AmuletWellAutoCalReportMenu_test.linmeasured[2])>(AmuletWellAutoCalReportMenu_test.linmeasured[1])) && ((AmuletWellAutoCalReportMenu_test.linmeasured[3])>(AmuletWellAutoCalReportMenu_test.linmeasured[2])) && ((AmuletWellAutoCalReportMenu_test.linmeasured[4])>(AmuletWellAutoCalReportMenu_test.linmeasured[3]))){
			linstd[0] = 32.85;
			linstd[1] = 40.8;
			linstd[2] = 121.8;
			linstd[3] = 344.3;
			linstd[4] = 661.66;

			lininit(strng, TRUE, prtype);
			strncpy(&strng[2], "==============================", 30);
			pr_write(strng);

			feed(1, prtype);

			lininit(strng, TRUE, prtype);
			strncpy(&strng[2], "Linearity Correction", 20);
			pr_write(strng);

			for(index=0; index<5; index++){
				kev = AmuletWellAutoCalReportMenu_test.linmeasured[index] - linstd[index];
				kev /= linstd[index];
				kev *= 100.0;
				lininit(strng, TRUE, prtype);
				nc = sprintf(buf, "%.3f keV = %.3f %%", AmuletWellAutoCalReportMenu_test.linmeasured[index], kev);
				strncpy(&strng[2], buf, nc);
				pr_write(strng);
			}
		}
		formfeed(prtype);
	}
}

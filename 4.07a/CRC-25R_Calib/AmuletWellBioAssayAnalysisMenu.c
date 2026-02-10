#define PHASE_WELLBIOASSAYANALYSIS_PRE_INIT	0
#define PHASE_WELLBIOASSAYANALYSIS_WAIT		1
#define PHASE_WELLBIOASSAYANALYSIS_SAVE		2
#define PHASE_WELLBIOASSAYANALYSIS_CANCEL	3
#define PHASE_WELLBIOASSAYANALYSIS_PRINT	4
#define PHASE_WELLBIOASSAYANALYSIS_PRINT_AND_SAVE	5
#define PHASE_WELLBIOASSAYANALYSIS_BACKGROUND 6
#define PHASE_WELLBIOASSAYANALYSIS_SPECTRUM	7

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "amulet.h"
#include "database.h"
#include "bioassay.h"
#include "nuc.h"
#include "printer.h"

PROBEBIOASSAYTEST AmuletWellBioAssayAnalysis_test;

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern PROBEBIOASSAYTEST AmuletWellBioAssay_test;
extern int AmuletWellBioAssay_staffID;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;

void SetAmuletBackHTML(void);
void PushPageStack(unsigned char ucPage);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
static void AmuletWellBioAssayAnalysis_print(void);

void AmuletWellBioAssayAnalysis_menu(void){
#ifdef TIMES1000
	int index;
#endif
	char message[51];
	PROBEBIOASSAYSETTING setting;
	BACKGND background;

	switch(m_iPhase){
		case PHASE_WELLBIOASSAYANALYSIS_PRE_INIT:
			if(m_ucClear == 84){
				m_ucClear = 0;
			}

			if(AmuletWellBioAssayAnalysis_test.ProbeBioAssayTestID == 0){
				SetAmuletByte(84, 0x00);
				SetAmuletByte(85, 0xFF);
				SetAmuletByte(86, 0xFF);
				SetAmuletByte(87, 0xFF);
				SetAmuletByte(88, 0x00);
				SetAmuletByte(89, 0x00);
				SetAmuletByte(90, 0x00);
				SetAmuletByte(91, 0x00);
				//if(current.printer != NONE_PRINTER) SetAmuletByte(92, 0xFF);
				if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
					SetAmuletByte(92, 0xFF);
			}else{
				SetAmuletByte(84, 0xFF);
				SetAmuletByte(85, 0x00);
				SetAmuletByte(86, 0x00);
				SetAmuletByte(87, 0x00);
				SetAmuletByte(88, 0xFF);
				if(AmuletWellBioAssayAnalysis_test.Inactive){
					SetAmuletByte(89, 0x00);
					SetAmuletByte(90, 0xFF);
					SetAmuletString(99, AmuletWellBioAssayAnalysis_test.InactiveReason);
				}else{
					SetAmuletByte(89, 0xFF);
					SetAmuletByte(90, 0x00);
				}
				//if(current.printer != NONE_PRINTER) SetAmuletByte(91, 0xFF);
				if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))  SetAmuletByte(91, 0xFF);
				else SetAmuletByte(91, 0x00);
				SetAmuletByte(92, 0x00);
			}

			if(AmuletWellBioAssayAnalysis_test.Spectrum.SpectraID == -1){
				AmuletWellBioAssayAnalysis_test.Spectrum.SpectraID = AmuletWellBioAssayAnalysis_test.SpectraID;
				DB_ReadSpectrum(&AmuletWellBioAssayAnalysis_test.Spectrum);
			}

			DB_ReadProbeBioAssaySetting(AmuletWellBioAssayAnalysis_test.ProbeBioAssaySettingID, &setting);

			sprintf(message, "%s %s", AmuletWellBioAssayAnalysis_test.FirstName, AmuletWellBioAssayAnalysis_test.LastName);
			SetAmuletString(100, message);

			sprintf(message, "%.0f", AmuletWellBioAssayAnalysis_test.Spectrum.LiveTime);
			SetAmuletString(102, message);

			sprintf(message, "%d", AmuletWellBioAssayAnalysis_test.ProbeDistance);
			SetAmuletString(103, message);

			GetExtendedTimeInfo(&AmuletWellBioAssayAnalysis_test.CreatedOn, message);
			SetAmuletString(104, message);

			if(AmuletWellBioAssayAnalysis_test.I131Active){
				sprintf(message, "%.1f - %.1f", AmuletWellBioAssayAnalysis_test.I131StartEV, AmuletWellBioAssayAnalysis_test.I131EndEV);
				SetAmuletString(105, message);

				if(current.system == CI){
					sprintf(message, "%.0f cpm", AmuletWellBioAssayAnalysis_test.I131BackgroundROICPM);
				}else{
					sprintf(message, "%.0f cps", AmuletWellBioAssayAnalysis_test.I131BackgroundROICPM / 60.0);
				}
				SetAmuletString(108, message);

				if(current.system == CI){
					sprintf(message, "%.0f cpm", AmuletWellBioAssayAnalysis_test.I131NetROICPM);
				}else{
					sprintf(message, "%.0f cps", AmuletWellBioAssayAnalysis_test.I131NetROICPM / 60.0);
				}
				SetAmuletString(111, message);

				if(AmuletWellBioAssayAnalysis_test.I131Activity == -1.0) message[0] = 0;
				else{
					if(current.system == CI) format_activity_system2(AmuletWellBioAssayAnalysis_test.I131Activity, message);
					else format_activity_system_kbq(AmuletWellBioAssayAnalysis_test.I131Activity, message);
				}
				SetAmuletString(114, message);

				if(setting.I131Threshold == -1.0) message[0] = 0;
				else{
					if(current.system == CI) format_activity_system2(setting.I131Threshold, message);
					else format_activity_system_kbq(setting.I131Threshold, message);
				}
				SetAmuletString(117, message);

				if(AmuletWellBioAssayAnalysis_test.I131High){
					SetAmuletByte(81, 0xFF);
				}else SetAmuletByte(81, 0x00);
			}else{
				SetAmuletString(108, "");
				SetAmuletString(111, "");
				SetAmuletString(114, "");
				SetAmuletString(117, "");
				SetAmuletByte(81, 0x00);
			}

			if(AmuletWellBioAssayAnalysis_test.I125Active){
				sprintf(message, "%.1f - %.1f", AmuletWellBioAssayAnalysis_test.I125StartEV, AmuletWellBioAssayAnalysis_test.I125EndEV);
				SetAmuletString(106, message);

				if(current.system == CI){
					sprintf(message, "%.0f cpm", AmuletWellBioAssayAnalysis_test.I125BackgroundROICPM);
				}else{
					sprintf(message, "%.0f cps", AmuletWellBioAssayAnalysis_test.I125BackgroundROICPM / 60.0);
				}
				SetAmuletString(109, message);

				if(current.system == CI){
					sprintf(message, "%.0f cpm", AmuletWellBioAssayAnalysis_test.I125NetROICPM);
				}else{
					sprintf(message, "%.0f cps", AmuletWellBioAssayAnalysis_test.I125NetROICPM / 60.0);
				}
				SetAmuletString(112, message);

				if(current.system == CI){
					sprintf(message, "%.0f cpm", AmuletWellBioAssayAnalysis_test.I125NetROICPMCorrected);
				}else{
					sprintf(message, "%.0f cps", AmuletWellBioAssayAnalysis_test.I125NetROICPMCorrected / 60.0);
				}
				SetAmuletString(121, message);

				if(AmuletWellBioAssayAnalysis_test.I125Activity == -1.0) message[0] = 0;
				else{
					if(current.system == CI) format_activity_system2(AmuletWellBioAssayAnalysis_test.I125Activity, message);
					else format_activity_system_kbq(AmuletWellBioAssayAnalysis_test.I125Activity, message);
				}
				SetAmuletString(115, message);

				if(setting.I125Threshold == -1.0) message[0] = 0;
				else{
					if(current.system == CI) format_activity_system2(setting.I125Threshold, message);
					else format_activity_system_kbq(setting.I125Threshold, message);
				}
				SetAmuletString(118, message);

				if(AmuletWellBioAssayAnalysis_test.I125High) SetAmuletByte(82, 0xFF);
				else SetAmuletByte(82, 0x00);
			}else{
				SetAmuletString(109, "");
				SetAmuletString(112, "");
				SetAmuletString(115, "");
				SetAmuletString(118, "");
				SetAmuletString(121, "");
				SetAmuletByte(82, 0x00);
			}

			if(AmuletWellBioAssayAnalysis_test.I123Active){
				sprintf(message, "%.1f - %.1f", AmuletWellBioAssayAnalysis_test.I123StartEV, AmuletWellBioAssayAnalysis_test.I123EndEV);
				SetAmuletString(107, message);

				if(current.system == CI){
					sprintf(message, "%.0f cpm", AmuletWellBioAssayAnalysis_test.I123BackgroundROICPM);
				}else{
					sprintf(message, "%.0f cps", AmuletWellBioAssayAnalysis_test.I123BackgroundROICPM / 60.0);
				}
				SetAmuletString(110, message);

				if(current.system == CI){
					sprintf(message, "%.0f cpm", AmuletWellBioAssayAnalysis_test.I123NetROICPM);
				}else{
					sprintf(message, "%.0f cps", AmuletWellBioAssayAnalysis_test.I123NetROICPM / 60.0);
				}
				SetAmuletString(113, message);

				if(AmuletWellBioAssayAnalysis_test.I123Activity == -1.0) message[0] = 0;
				else{
					if(current.system == CI) format_activity_system2(AmuletWellBioAssayAnalysis_test.I123Activity, message);
					else format_activity_system_kbq(AmuletWellBioAssayAnalysis_test.I123Activity, message);
				}
				SetAmuletString(116, message);

				if(setting.I123Threshold == -1.0) message[0] = 0;
				else{
					if(current.system == CI) format_activity_system2(setting.I123Threshold, message);
					else format_activity_system_kbq(setting.I123Threshold, message);
				}
				SetAmuletString(119, message);

				if(AmuletWellBioAssayAnalysis_test.I123High) SetAmuletByte(83, 0xFF);
				else SetAmuletByte(83, 0x00);
			}else{
				SetAmuletString(110, "");
				SetAmuletString(113, "");
				SetAmuletString(116, "");
				SetAmuletString(119, "");
				SetAmuletByte(83, 0x00);
			}

			SetAmuletString(120, AmuletWellBioAssayAnalysis_test.Comment);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLBIOASSAYANALYSIS_WAIT;
			break;

		case PHASE_WELLBIOASSAYANALYSIS_WAIT:
			break;

		case PHASE_WELLBIOASSAYANALYSIS_SAVE:
			beep_amulet();
#ifdef TIMES1000
			for(index=0; index<TIMES1000; index++){
#endif
				BioAssaySaveTest(&AmuletWellBioAssayAnalysis_test);
#ifdef TIMES1000
				AmuletWellBioAssayAnalysis_test.SpectraID = 0;
				AmuletWellBioAssayAnalysis_test.Spectrum.SpectraID = 0;
				AmuletWellBioAssayAnalysis_test.Spectrum.MeasuredOn -= 86400;
				AmuletWellBioAssayAnalysis_test.CreatedOn -= 86400;
			}
#endif

			AmuletWellBioAssay_test.UserName[0] = 0;
			AmuletWellBioAssay_test.FirstName[0] = 0;
			AmuletWellBioAssay_test.LastName[0] = 0;
			AmuletWellBioAssay_staffID = -1;
			AmuletWellBioAssay_test.Spectrum.SpectraID = -1;
			SetAmuletBackHTML();
			break;

		case PHASE_WELLBIOASSAYANALYSIS_CANCEL:
			beep_amulet();
			AmuletWellBioAssay_test.UserName[0] = 0;
			AmuletWellBioAssay_test.FirstName[0] = 0;
			AmuletWellBioAssay_test.LastName[0] = 0;
			AmuletWellBioAssay_staffID = -1;
			AmuletWellBioAssay_test.Spectrum.SpectraID = -1;
			SetAmuletBackHTML();
			break;

		case PHASE_WELLBIOASSAYANALYSIS_PRINT:
			beep_amulet();
			AmuletWellBioAssayAnalysis_print();
			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_WELLBIOASSAYANALYSIS_WAIT;
			break;

		case PHASE_WELLBIOASSAYANALYSIS_PRINT_AND_SAVE:
			beep_amulet();
			BioAssaySaveTest(&AmuletWellBioAssayAnalysis_test);
			AmuletWellBioAssayAnalysis_print();
			AmuletWellBioAssay_test.UserName[0] = 0;
			AmuletWellBioAssay_test.FirstName[0] = 0;
			AmuletWellBioAssay_test.LastName[0] = 0;
			AmuletWellBioAssay_staffID = -1;
			AmuletWellBioAssay_test.Spectrum.SpectraID = -1;
			SetAmuletBackHTML();
			break;

		case PHASE_WELLBIOASSAYANALYSIS_BACKGROUND:
			beep_amulet();
			background.WellBackgroundID = AmuletWellBioAssayAnalysis_test.WellBackgroundID;
			DB_RetrieveBackgnd(&background);
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(background.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "BioAssay Background");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;

			if(AmuletWellBioAssayAnalysis_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellBioAssayAnalysis_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLBIOASSAYANALYSIS_SPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellBioAssayAnalysis_test.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "BioAssay Spectrum");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;

			if(AmuletWellBioAssayAnalysis_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellBioAssayAnalysis_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;
	}
}

static void AmuletWellBioAssayAnalysis_print(void){
	char prtype;
	char strng[90];
	char message[51];
	PROBEBIOASSAYSETTING setting;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		DB_ReadProbeBioAssaySetting(AmuletWellBioAssayAnalysis_test.ProbeBioAssaySettingID, &setting);

		rawheader(prtype, "BIOASSAY TEST", AmuletWellBioAssayAnalysis_test.CreatedOn);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);
		pr_write2(strng, "BioAssay Analysis");

		insertstring(strng, 0, 0, 0, "First: %s", AmuletWellBioAssayAnalysis_test.FirstName);
		pr_write2(strng, "BioAssay Analysis");

		insertstring(strng, 0, 0, 0, " Last: %s", AmuletWellBioAssayAnalysis_test.LastName);
		pr_write2(strng, "BioAssay Analysis");
		pr_write2(strng, "BioAssay Analysis");

		insertint(strng, 0, 0, 0, "Distance: %d cm", AmuletWellBioAssayAnalysis_test.ProbeDistance);
		pr_write2(strng, "BioAssay Analysis");

		insertfloat(strng, 0, 0, 0, "Live Time: %.0f sec", AmuletWellBioAssayAnalysis_test.Spectrum.LiveTime);
		pr_write2(strng, "BioAssay Analysis");

		insertfloat(strng, 0, 0, 0, "Real Time: %.0f sec", AmuletWellBioAssayAnalysis_test.Spectrum.RealTime);
		pr_write2(strng, "BioAssay Analysis");

		if(AmuletWellBioAssayAnalysis_test.I131Active){
			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "BioAssay Analysis");

			insertconst(strng, 0, 0, 0, " Nuclide: I131");
			pr_write2(strng, "BioAssay Analysis");

			//insertfloat(strng, 0, 0, 0, "ROI(keV): %.1f - %1.f", AmuletWellBioAssayAnalysis_test.I131StartEV, AmuletWellBioAssayAnalysis_test.I131EndEV);
			sprintf(message, "ROI(keV): %.1f - %.1f", AmuletWellBioAssayAnalysis_test.I131StartEV, AmuletWellBioAssayAnalysis_test.I131EndEV);
			insertstring(strng, 0, 0, 0, "%s", message);
			pr_write2(strng, "BioAssay Analysis");

			if(current.system == CI) insertfloat(strng, 0, 0, 0, "Background: %.0f cpm", AmuletWellBioAssayAnalysis_test.I131BackgroundROICPM);
			else insertfloat(strng, 0, 0, 0, "Background: %.0f cps", AmuletWellBioAssayAnalysis_test.I131BackgroundROICPM / 60.0);
			pr_write2(strng, "BioAssay Analysis");

			if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Counts: %.0f cpm", AmuletWellBioAssayAnalysis_test.I131NetROICPM);
			else insertfloat(strng, 0, 0, 0, "Net Counts: %.0f cps", AmuletWellBioAssayAnalysis_test.I131NetROICPM / 60.0);
			pr_write2(strng, "BioAssay Analysis");

			if(AmuletWellBioAssayAnalysis_test.I131Activity != -1.0){
				if(current.system == CI) format_activity_system2(AmuletWellBioAssayAnalysis_test.I131Activity, message);
				else format_activity_system_kbq(AmuletWellBioAssayAnalysis_test.I131Activity, message);
				replace(message, '$', 'u');
				insertstring(strng, 0, 0, 0, "Activity: %s", message);
				if(AmuletWellBioAssayAnalysis_test.I131High) insertconst(strng, 32, 1, 0, "HIGH");
				pr_write2(strng, "BioAssay Analysis");

				if(setting.I131Threshold != -1.0){
					if(current.system == CI) format_activity_system2(setting.I131Threshold, message);
					else format_activity_system_kbq(setting.I131Threshold, message);
					replace(message, '$', 'u');
					insertstring(strng, 0, 0, 0, "Threshold: %s", message);
					pr_write2(strng, "BioAssay Analysis");
				}
			}
		}

		if(AmuletWellBioAssayAnalysis_test.I125Active){
			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "BioAssay Analysis");

			insertconst(strng, 0, 0, 0, " Nuclide: I125");
			pr_write2(strng, "BioAssay Analysis");

			//insertfloat(strng, 0, 0, 0, "ROI(keV): %.1f - %1.f", AmuletWellBioAssayAnalysis_test.I125StartEV, AmuletWellBioAssayAnalysis_test.I125EndEV);
			sprintf(message, "ROI(keV): %.1f - %.1f", AmuletWellBioAssayAnalysis_test.I125StartEV, AmuletWellBioAssayAnalysis_test.I125EndEV);
			insertstring(strng, 0, 0, 0, "%s", message);
			pr_write2(strng, "BioAssay Analysis");

			if(current.system == CI) insertfloat(strng, 0, 0, 0, "Background: %.0f cpm", AmuletWellBioAssayAnalysis_test.I125BackgroundROICPM);
			else insertfloat(strng, 0, 0, 0, "Background: %.0f cps", AmuletWellBioAssayAnalysis_test.I125BackgroundROICPM / 60.0);
			pr_write2(strng, "BioAssay Analysis");

			if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Counts: %.0f cpm", AmuletWellBioAssayAnalysis_test.I125NetROICPM);
			else insertfloat(strng, 0, 0, 0, "Net Counts: %.0f cps", AmuletWellBioAssayAnalysis_test.I125NetROICPM / 60.0);
			pr_write2(strng, "BioAssay Analysis");

			insertconst(strng, 0, 0, 0, "Contamination");
			pr_write2(strng, "BioAssay Analysis");

			if(current.system == CI) insertfloat(strng, 0, 0, 0, "Adjusted Counts: %.0f cpm", AmuletWellBioAssayAnalysis_test.I125NetROICPMCorrected);
			else insertfloat(strng, 0, 0, 0, "Adjusted Counts: %.0f cps", AmuletWellBioAssayAnalysis_test.I125NetROICPMCorrected / 60.0);
			pr_write2(strng, "BioAssay Analysis");

			if(AmuletWellBioAssayAnalysis_test.I125Activity != -1.0){
				if(current.system == CI) format_activity_system2(AmuletWellBioAssayAnalysis_test.I125Activity, message);
				else format_activity_system_kbq(AmuletWellBioAssayAnalysis_test.I125Activity, message);
				replace(message, '$', 'u');
				insertstring(strng, 0, 0, 0, "Activity: %s", message);
				if(AmuletWellBioAssayAnalysis_test.I125High) insertconst(strng, 32, 1, 0, "HIGH");
				pr_write2(strng, "BioAssay Analysis");

				if(setting.I125Threshold != -1.0){
					if(current.system == CI) format_activity_system2(setting.I125Threshold, message);
					else format_activity_system_kbq(setting.I125Threshold, message);
					replace(message, '$', 'u');
					insertstring(strng, 0, 0, 0, "Threshold: %s", message);
					pr_write2(strng, "BioAssay Analysis");
				}
			}
		}

		if(AmuletWellBioAssayAnalysis_test.I123Active){
			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "BioAssay Analysis");

			insertconst(strng, 0, 0, 0, " Nuclide: I123");
			pr_write2(strng, "BioAssay Analysis");

			//insertfloat(strng, 0, 0, 0, "ROI(keV): %.1f - %1.f", AmuletWellBioAssayAnalysis_test.I123StartEV, AmuletWellBioAssayAnalysis_test.I123EndEV);
			sprintf(message, "ROI(keV): %.1f - %.1f", AmuletWellBioAssayAnalysis_test.I123StartEV, AmuletWellBioAssayAnalysis_test.I123EndEV);
			insertstring(strng, 0, 0, 0, "%s", message);
			pr_write2(strng, "BioAssay Analysis");

			if(current.system == CI) insertfloat(strng, 0, 0, 0, "Background: %.0f cpm", AmuletWellBioAssayAnalysis_test.I123BackgroundROICPM);
			else insertfloat(strng, 0, 0, 0, "Background: %.0f cps", AmuletWellBioAssayAnalysis_test.I123BackgroundROICPM / 60.0);
			pr_write2(strng, "BioAssay Analysis");

			if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Counts: %.0f cpm", AmuletWellBioAssayAnalysis_test.I123NetROICPM);
			else insertfloat(strng, 0, 0, 0, "Net Counts: %.0f cps", AmuletWellBioAssayAnalysis_test.I123NetROICPM / 60.0);
			pr_write2(strng, "BioAssay Analysis");

			if(AmuletWellBioAssayAnalysis_test.I123Activity != -1.0){
				if(current.system == CI) format_activity_system2(AmuletWellBioAssayAnalysis_test.I123Activity, message);
				else format_activity_system_kbq(AmuletWellBioAssayAnalysis_test.I123Activity, message);
				replace(message, '$', 'u');
				insertstring(strng, 0, 0, 0, "Activity: %s", message);
				if(AmuletWellBioAssayAnalysis_test.I123High) insertconst(strng, 32, 1, 0, "HIGH");
				pr_write2(strng, "BioAssay Analysis");

				if(setting.I123Threshold != -1.0){
					if(current.system == CI) format_activity_system2(setting.I123Threshold, message);
					else format_activity_system_kbq(setting.I123Threshold, message);
					replace(message, '$', 'u');
					insertstring(strng, 0, 0, 0, "Threshold: %s", message);
					pr_write2(strng, "BioAssay Analysis");
				}
			}
		}

		if(AmuletWellBioAssayAnalysis_test.Comment[0] != 0){
			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "BioAssay Analysis");
			insertstring(strng, 0, 0, 0, " COMMENT: %s", AmuletWellBioAssayAnalysis_test.Comment);
			pr_write2(strng, "BioAssay Analysis");
		}

		if(AmuletWellBioAssayAnalysis_test.Inactive){
			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "BioAssay Analysis");
			if(AmuletWellBioAssayAnalysis_test.InactiveReason[0] == 0){
				insertconst(strng, 0, 0, 0, "INACTIVE");
			}else{
				insertstring(strng, 0, 0, 0, "INACTIVE: %s", AmuletWellBioAssayAnalysis_test.InactiveReason);
			}
			pr_write2(strng, "BioAssay Analysis");
		}
		formfeed(prtype);
	}
}

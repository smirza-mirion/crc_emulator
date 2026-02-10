#define PHASE_WELLRBCANALYSIS_PRE_INIT	0
#define PHASE_WELLRBCANALYSIS_WAIT		1
#define PHASE_WELLRBCANALYSIS_PRINT		2
#define PHASE_WELLRBCANALYSIS_FINISH	3
#define PHASE_WELLRBCANALYSIS_WHOLESTDSPEC 4
#define PHASE_WELLRBCANALYSIS_WHOLESAMPSPEC 5
#define PHASE_WELLRBCANALYSIS_PLASMASTDSPEC 6
#define PHASE_WELLRBCANALYSIS_PLASMASAMPSPEC 7
#define PHASE_WELLRBCANALYSIS_BACKGND 8


#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "amulet.h"
#include "printer.h"

extern int m_iPhase;
extern CURRENT current;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;
extern WELLRBCTEST AmuletWellRBCMenu_test;

void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void PushPageStack(unsigned char ucPage);
unsigned char PopPageStack(void);
void SetAmuletBackHTML(void);
void AmuletWellRBCAnalysis_print(void);

void AmuletWellRBCAnalysis_menu(void){
	char message[51];
	float cps;

	switch(m_iPhase){
		case PHASE_WELLRBCANALYSIS_PRE_INIT:
			GetExtendedTimeInfo(&AmuletWellRBCMenu_test.CreatedOn, message);
			SetAmuletString(100, message);

			cps = AmuletWellRBCMenu_test.BackgroundROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCMenu_test.BackgroundROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(102, message);

			cps = AmuletWellRBCMenu_test.WholeBloodStandardROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCMenu_test.WholeBloodStandardROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(103, message);

			cps = AmuletWellRBCMenu_test.WholeBloodSampleROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCMenu_test.WholeBloodSampleROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(104, message);

			cps = AmuletWellRBCMenu_test.PlasmaStandardROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCMenu_test.PlasmaStandardROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(105, message);

			cps = AmuletWellRBCMenu_test.PlasmaSampleROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCMenu_test.PlasmaSampleROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(106, message);

			sprintf(message, "%.1f %%", AmuletWellRBCMenu_test.DoseHematocrit);
			SetAmuletString(107, message);

			sprintf(message, "%.1f %%", AmuletWellRBCMenu_test.PatientHematocrit);
			SetAmuletString(108, message);

			sprintf(message, "%.1f kg", AmuletWellRBCMenu_test.PatientWeight);
			SetAmuletString(109, message);

			sprintf(message, "%.0f ml", AmuletWellRBCMenu_test.RBCVolume);
			SetAmuletString(110, message);

			sprintf(message, "%.0f ml/kg", AmuletWellRBCMenu_test.RBCPerKg);
			SetAmuletString(111, message);

			sprintf(message, "%.0f ml", AmuletWellRBCMenu_test.WholeBloodVolume);
			SetAmuletString(112, message);

			sprintf(message, "%.0f ml/kg", AmuletWellRBCMenu_test.WholeBloodPerKg);
			SetAmuletString(113, message);

			sprintf(message, "%.0f ml", AmuletWellRBCMenu_test.PlasmaVolume);
			SetAmuletString(114, message);

			sprintf(message, "%.0f ml/kg", AmuletWellRBCMenu_test.PlasmaPerKg);
			SetAmuletString(115, message);

			SetAmuletByte(100, 0xFF);

			//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(101, 0xFF);

			m_iPhase = PHASE_WELLRBCANALYSIS_WAIT;
			break;

		case PHASE_WELLRBCANALYSIS_WAIT:
			break;

		case PHASE_WELLRBCANALYSIS_PRINT:
			beep_amulet();
			AmuletWellRBCAnalysis_print();
			SetAmuletByte(102, 0xFF);
			m_iPhase = PHASE_WELLRBCANALYSIS_WAIT;
			break;

		case PHASE_WELLRBCANALYSIS_FINISH:
			beep_amulet();
			PopPageStack();
			SetAmuletBackHTML();
			break;

		case PHASE_WELLRBCANALYSIS_WHOLESTDSPEC:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCMenu_test.WholeBloodStandard_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Test, Blood Standard");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLRBCANALYSIS_WHOLESAMPSPEC:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCMenu_test.WholeBloodSample_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Test, Blood Sample");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLRBCANALYSIS_PLASMASTDSPEC:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCMenu_test.PlasmaStandard_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Test, Plasma Standard");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLRBCANALYSIS_PLASMASAMPSPEC:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCMenu_test.PlasmaSample_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Test, Plasma Sample");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLRBCANALYSIS_BACKGND:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCMenu_test.Background.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Test, Background");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;
	}
}

void AmuletWellRBCAnalysis_print(void){
	char prtype;
	char strng[90];
	char buf[40];
	short nc;
	float cps;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheader(prtype, "RBC VOLUME", AmuletWellRBCMenu_test.CreatedOn);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Nuclide: Cr51", 13);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "    ROI: 100 - 500 keV", 22);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background  Time:", 17);

		nc = sprintf(buf, "%.0f sec", AmuletWellRBCMenu_test.Background.Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole   Std Time:", 17);
		nc = sprintf(buf, "%.0f sec", AmuletWellRBCMenu_test.WholeBloodStandard_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma  Std Time:", 17);
		nc = sprintf(buf, "%.0f sec", AmuletWellRBCMenu_test.PlasmaStandard_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole  Samp Time:", 17);
		nc = sprintf(buf, "%.0f sec", AmuletWellRBCMenu_test.WholeBloodSample_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma Samp Time:", 17);
		nc = sprintf(buf, "%.0f sec", AmuletWellRBCMenu_test.PlasmaSample_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Dose    Hct   :", 15);
		nc = sprintf(buf, "%.1f %% =H", AmuletWellRBCMenu_test.DoseHematocrit);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Patient Hct   :", 15);
		nc = sprintf(buf, "%.1f %% =C", AmuletWellRBCMenu_test.PatientHematocrit);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Patient Weight:", 15);
		nc = sprintf(buf, "%.1f kg", AmuletWellRBCMenu_test.PatientWeight);
		strncpy(&strng[30-nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background :", 12);

		cps = AmuletWellRBCMenu_test.BackgroundROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm=B", AmuletWellRBCMenu_test.BackgroundROICPM);
		else nc = sprintf(buf, "%.0f cps =B", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole   Std:", 12);
		cps = AmuletWellRBCMenu_test.WholeBloodStandardROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm=W", AmuletWellRBCMenu_test.WholeBloodStandardROICPM);
		else nc = sprintf(buf, "%.0f cps =W", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole  Samp:", 12);
		cps = AmuletWellRBCMenu_test.WholeBloodSampleROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm=S", AmuletWellRBCMenu_test.WholeBloodSampleROICPM);
		else nc = sprintf(buf, "%.0f cps =S", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma  Std:", 12);
		cps = AmuletWellRBCMenu_test.PlasmaStandardROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm=P", AmuletWellRBCMenu_test.PlasmaStandardROICPM);
		else nc = sprintf(buf, "%.0f cps =P", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma Samp:", 12);
		cps = AmuletWellRBCMenu_test.PlasmaSampleROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm=L", AmuletWellRBCMenu_test.PlasmaSampleROICPM);
		else nc = sprintf(buf, "%.0f cps =L", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "==============================", 30);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2],"RBC Vol:", 8);
		nc = sprintf(buf, "%5.0f ml   ", AmuletWellRBCMenu_test.RBCVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellRBCMenu_test.RBCPerKg);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], " 1000*((W-B)-((P-B)*(1-H)))*C ", 30);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "= --------------------------- ", 30);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "      (S-B) - ((L-B)*(1-C))   ", 30);
		pr_write(strng);
		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "==============================", 30);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2],"Blood Vol:", 10);

		nc = sprintf(buf, "%5.0f ml   ", AmuletWellRBCMenu_test.WholeBloodVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellRBCMenu_test.WholeBloodPerKg);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "= RBC Volume / C              ", 30);
		pr_write(strng);
		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "==============================", 30);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2],"Plasma Vol:", 11);
		nc = sprintf(buf, "%5.0f ml   ", AmuletWellRBCMenu_test.PlasmaVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellRBCMenu_test.PlasmaPerKg);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "= Whole Blood Vol - RBC Vol   ", 30);
		pr_write(strng);
		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "==============================", 30);
		pr_write(strng);

		formfeed(prtype);
	}
}

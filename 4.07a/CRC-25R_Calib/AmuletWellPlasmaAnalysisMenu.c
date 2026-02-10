#define PHASE_WELLPLASMAANALYSIS_PRE_INIT	0
#define PHASE_WELLPLASMAANALYSIS_WAIT		1
#define PHASE_WELLPLASMAANALYSIS_PRINT		2
#define PHASE_WELLPLASMAANALYSIS_FINISH		3
#define PHASE_WELLPLASMAANALYSIS_STDSPECTRUM 4
#define PHASE_WELLPLASMAANALYSIS_WHOLEBLOODSPECTRUM 5
#define PHASE_WELLPLASMAANALYSIS_PLASMASPECTRUM	6
#define PHASE_WELLPLASMAANALYSIS_BACKGND	7

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "amulet.h"
#include "printer.h"

extern int m_iPhase;
extern CURRENT current;
extern WELLPLASMATEST AmuletWellPlasmaMenu_test;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;

void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void PushPageStack(unsigned char ucPage);
unsigned char PopPageStack(void);
void SetAmuletBackHTML(void);
static void AmuletWellPlasmaAnalysis_print(void);

void AmuletWellPlasmaAnalysis_menu(void){
	float cps;
	char message[51];

	switch(m_iPhase){
		case PHASE_WELLPLASMAANALYSIS_PRE_INIT:
			GetExtendedTimeInfo(&AmuletWellPlasmaMenu_test.CreatedOn, message);
			SetAmuletString(100, message);

			cps = AmuletWellPlasmaMenu_test.BackgroundROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaMenu_test.BackgroundROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(102, message);

			cps = AmuletWellPlasmaMenu_test.StandardROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaMenu_test.StandardROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(103, message);

			cps = AmuletWellPlasmaMenu_test.WholeBloodROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaMenu_test.WholeBloodROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(104, message);

			cps = AmuletWellPlasmaMenu_test.PlasmaROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaMenu_test.PlasmaROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(105, message);

			sprintf(message, "%.1f", AmuletWellPlasmaMenu_test.DilutionFactor);
			SetAmuletString(106, message);

			sprintf(message, "%.1f ml", AmuletWellPlasmaMenu_test.SampleVolume);
			SetAmuletString(107, message);

			sprintf(message, "%.1f kg", AmuletWellPlasmaMenu_test.PatientWeight);
			SetAmuletString(108, message);

			sprintf(message, "%.1f %%", AmuletWellPlasmaMenu_test.Hematocrit);
			SetAmuletString(109, message);

			sprintf(message, "%.0f ml", AmuletWellPlasmaMenu_test.WholeBloodVolume);
			SetAmuletString(110, message);

			sprintf(message, "%.0f ml/kg", AmuletWellPlasmaMenu_test.WholeBloodPerKg);
			SetAmuletString(111, message);

			sprintf(message, "%.0f ml", AmuletWellPlasmaMenu_test.PlasmaVolume);
			SetAmuletString(112, message);

			sprintf(message, "%.0f ml/kg", AmuletWellPlasmaMenu_test.PlasmaPerKg);
			SetAmuletString(113, message);

			sprintf(message, "%.0f ml", AmuletWellPlasmaMenu_test.RBCVolume);
			SetAmuletString(114, message);

			sprintf(message, "%.0f ml/kg", AmuletWellPlasmaMenu_test.RBCPerKg);
			SetAmuletString(115, message);

			sprintf(message, "%.1f %%", AmuletWellPlasmaMenu_test.CalculatedHematocrit);
			SetAmuletString(116, message);

			SetAmuletByte(100, 0xFF);

			//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(101, 0xFF);

			m_iPhase = PHASE_WELLPLASMAANALYSIS_WAIT;
			break;

		case PHASE_WELLPLASMAANALYSIS_WAIT:
			break;

		case PHASE_WELLPLASMAANALYSIS_PRINT:
			beep_amulet();
			AmuletWellPlasmaAnalysis_print();
			SetAmuletByte(102, 0xFF);
			m_iPhase = PHASE_WELLPLASMAANALYSIS_WAIT;
			break;

		case PHASE_WELLPLASMAANALYSIS_FINISH:
			beep_amulet();
			PopPageStack();
			SetAmuletBackHTML();
			break;

		case PHASE_WELLPLASMAANALYSIS_STDSPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellPlasmaMenu_test.Standard_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Plasma Test, Standard");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLPLASMAANALYSIS_WHOLEBLOODSPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellPlasmaMenu_test.WholeBlood_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Plasma Test, Whole Blood");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLPLASMAANALYSIS_PLASMASPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellPlasmaMenu_test.Plasma_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Plasma Test, Plasma");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLPLASMAANALYSIS_BACKGND:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellPlasmaMenu_test.Background.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Plasma Test, Background");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;
	}
}

static void AmuletWellPlasmaAnalysis_print(void){
	char prtype;
	char strng[90];
	char buf[40];
	short nc;
	float cps;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheader(prtype, "PLASMA VOLUME", AmuletWellPlasmaMenu_test.CreatedOn);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Nuclide: I125", 13);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "    ROI: 15 - 80 keV", 20);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background  Time:", 17);
		nc = sprintf(buf, "%.1f sec", AmuletWellPlasmaMenu_test.Background.Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Standard    Time:", 17);
		nc = sprintf(buf, "%.1f sec", AmuletWellPlasmaMenu_test.Standard_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole Blood Time:", 17);
		nc = sprintf(buf, "%.1f sec", AmuletWellPlasmaMenu_test.WholeBlood_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma      Time:", 17);
		nc = sprintf(buf, "%.1f sec", AmuletWellPlasmaMenu_test.Plasma_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Dilution   :",12);
		nc = sprintf(buf, "%6.1f", AmuletWellPlasmaMenu_test.DilutionFactor);
		strncpy(&strng[26-nc], buf, nc);
		strncpy(&strng[30], "=D", 2);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Sample Vol :",12);
		nc = sprintf(buf, "%6.1f", AmuletWellPlasmaMenu_test.SampleVolume);
		strncpy(&strng[26-nc], buf, nc);
		strncpy(&strng[27], "ml =A", 5);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Patient Wt :",12);
		nc = sprintf(buf, "%5.1f", AmuletWellPlasmaMenu_test.PatientWeight);
		strncpy(&strng[26-nc], buf, nc);
		strncpy(&strng[27], "kg", 2);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Patient Hct:",12);
		nc = sprintf(buf, "%4.1f", AmuletWellPlasmaMenu_test.Hematocrit);
		strncpy(&strng[26-nc], buf, nc);
		strng[27] = '%';
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background :", 12);
		cps = AmuletWellPlasmaMenu_test.BackgroundROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =B", AmuletWellPlasmaMenu_test.BackgroundROICPM);
		else nc = sprintf(buf, "%.0f cps =B", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Standard   :", 12);
		cps = AmuletWellPlasmaMenu_test.StandardROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =W", AmuletWellPlasmaMenu_test.StandardROICPM);
		else nc = sprintf(buf, "%.0f cps =W", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole Blood:", 12);
		cps = AmuletWellPlasmaMenu_test.WholeBloodROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =S", AmuletWellPlasmaMenu_test.WholeBloodROICPM);
		else nc = sprintf(buf, "%.0f cps =S", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma     :", 12);
		cps = AmuletWellPlasmaMenu_test.PlasmaROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =L", AmuletWellPlasmaMenu_test.PlasmaROICPM);
		else nc = sprintf(buf, "%.0f cps =L", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "------------------------------", 30);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2],"Whole Blood Vol:", 16);
		nc = sprintf(buf, "%5.0f ml   ", AmuletWellPlasmaMenu_test.WholeBloodVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellPlasmaMenu_test.WholeBloodPerKg);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2],"((W-B) * D * A) / (S-B)", 23);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "------------------------------", 30);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2],"Plasma Vol:", 11);
		nc = sprintf(buf, "%5.0f ml   ", AmuletWellPlasmaMenu_test.PlasmaVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellPlasmaMenu_test.PlasmaPerKg);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "((W-B) * D * A) / (L-B)", 23);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "------------------------------", 30);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2],"RBC Vol:", 8);
		nc = sprintf(buf, "%5.0f ml   ", AmuletWellPlasmaMenu_test.RBCVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellPlasmaMenu_test.RBCPerKg);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "= Whole Vol - Plasma Vol", 24);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "------------------------------", 30);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Radioactive Hct:", 16);
		nc = sprintf(buf, "%4.1f %%", AmuletWellPlasmaMenu_test.CalculatedHematocrit);
		strncpy(&strng[28-nc], buf, nc);
		pr_write(strng);

		formfeed(prtype);
	}
}

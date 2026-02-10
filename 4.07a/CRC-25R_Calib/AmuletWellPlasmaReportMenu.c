#define PHASE_WELLPLASMAREPORT_PRE_INIT	0
#define PHASE_WELLPLASMAREPORT_WAIT		1
#define PHASE_WELLPLASMAREPORT_PRINT		2
#define PHASE_WELLPLASMAREPORT_STDSPECTRUM	3
#define PHASE_WELLPLASMAREPORT_WHOLEBLOODSPECTRUM 4
#define PHASE_WELLPLASMAREPORT_PLASMASPECTRUM	5
#define PHASE_WELLPLASMAREPORT_BACKGND	6

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

WELLPLASMATEST AmuletWellPlasmaReportMenu_test;

void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void PushPageStack(unsigned char ucPage);
static void AmuletWellPlasmaReport_print(void);

void AmuletWellPlasmaReport_menu(void){
	float cps;
	char message[51];

	switch(m_iPhase){
		case PHASE_WELLPLASMAREPORT_PRE_INIT:
			GetExtendedTimeInfo(&AmuletWellPlasmaReportMenu_test.CreatedOn, message);
			SetAmuletString(100, message);

			if(AmuletWellPlasmaReportMenu_test.TestIdent.DateOfBirth != 0.0){
				dateout_julian(message, AmuletWellPlasmaReportMenu_test.TestIdent.DateOfBirth);
				SetAmuletString(102, message);
			}
			if(strcmp(AmuletWellPlasmaReportMenu_test.TestIdent.Sex, "M") == 0) SetAmuletString(103, "Male");
			else if(strcmp(AmuletWellPlasmaReportMenu_test.TestIdent.Sex, "F") == 0) SetAmuletString(103, "Female");
			SetAmuletString(104, AmuletWellPlasmaReportMenu_test.TestIdent.TestID);
			SetAmuletString(105, AmuletWellPlasmaReportMenu_test.TestIdent.PatientID);
			SetAmuletString(106, AmuletWellPlasmaReportMenu_test.TestIdent.FirstName);
			SetAmuletString(107, AmuletWellPlasmaReportMenu_test.TestIdent.LastName);
			SetAmuletString(108, AmuletWellPlasmaReportMenu_test.TestIdent.Physician);
			SetAmuletString(109, AmuletWellPlasmaReportMenu_test.TestIdent.TechID);

			cps = AmuletWellPlasmaReportMenu_test.BackgroundROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaReportMenu_test.BackgroundROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(110, message);

			cps = AmuletWellPlasmaReportMenu_test.StandardROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaReportMenu_test.StandardROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(111, message);

			cps = AmuletWellPlasmaReportMenu_test.WholeBloodROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaReportMenu_test.WholeBloodROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(112, message);

			cps = AmuletWellPlasmaReportMenu_test.PlasmaROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaReportMenu_test.PlasmaROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(113, message);

			sprintf(message, "%.1f", AmuletWellPlasmaReportMenu_test.DilutionFactor);
			SetAmuletString(114, message);

			sprintf(message, "%.1f ml", AmuletWellPlasmaReportMenu_test.SampleVolume);
			SetAmuletString(115, message);

			sprintf(message, "%.1f kg", AmuletWellPlasmaReportMenu_test.PatientWeight);
			SetAmuletString(116, message);

			sprintf(message, "%.1f %%", AmuletWellPlasmaReportMenu_test.Hematocrit);
			SetAmuletString(117, message);

			sprintf(message, "%.0f ml", AmuletWellPlasmaReportMenu_test.WholeBloodVolume);
			SetAmuletString(118, message);

			sprintf(message, "%.0f ml/kg", AmuletWellPlasmaReportMenu_test.WholeBloodPerKg);
			SetAmuletString(119, message);

			sprintf(message, "%.0f ml", AmuletWellPlasmaReportMenu_test.PlasmaVolume);
			SetAmuletString(120, message);

			sprintf(message, "%.0f ml/kg", AmuletWellPlasmaReportMenu_test.PlasmaPerKg);
			SetAmuletString(121, message);

			sprintf(message, "%.0f ml", AmuletWellPlasmaReportMenu_test.RBCVolume);
			SetAmuletString(122, message);

			sprintf(message, "%.0f ml/kg", AmuletWellPlasmaReportMenu_test.RBCPerKg);
			SetAmuletString(123, message);

			sprintf(message, "%.1f %%", AmuletWellPlasmaReportMenu_test.CalculatedHematocrit);
			SetAmuletString(124, message);

			SetAmuletByte(100, 0xFF);

			//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			if((current.printer != NONE_PRINTER)&& (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(101, 0xFF);

			if(AmuletWellPlasmaReportMenu_test.Inactive){
				SetAmuletString(99, AmuletWellPlasmaReportMenu_test.InactiveReason);
				SetAmuletByte(103, 0xFF);
			}else{
				SetAmuletByte(102, 0xFF);
			}

			m_iPhase = PHASE_WELLPLASMAREPORT_WAIT;
			break;

		case PHASE_WELLPLASMAREPORT_WAIT:
			break;

		case PHASE_WELLPLASMAREPORT_PRINT:
			beep_amulet();
			AmuletWellPlasmaReport_print();
			SetAmuletByte(104, 0xFF);
			m_iPhase = PHASE_WELLPLASMAREPORT_WAIT;
			break;

		case PHASE_WELLPLASMAREPORT_STDSPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellPlasmaReportMenu_test.Standard_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Plasma Test, Standard");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellPlasmaReportMenu_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellPlasmaReportMenu_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLPLASMAREPORT_WHOLEBLOODSPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellPlasmaReportMenu_test.WholeBlood_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Plasma Test, Whole Blood");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellPlasmaReportMenu_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellPlasmaReportMenu_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLPLASMAREPORT_PLASMASPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellPlasmaReportMenu_test.Plasma_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Plasma Test, Plasma");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellPlasmaReportMenu_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellPlasmaReportMenu_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLPLASMAREPORT_BACKGND:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellPlasmaReportMenu_test.Background.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Plasma Test, Background");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellPlasmaReportMenu_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellPlasmaReportMenu_test.InactiveReason);
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

static void AmuletWellPlasmaReport_print(void){
	char prtype;
	char strng[90];
	char buf[40];
	short nc;
	float cps;
	char message[51];

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheader(prtype, "PLASMA VOLUME", AmuletWellPlasmaReportMenu_test.CreatedOn);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "   Test ID: %s", AmuletWellPlasmaReportMenu_test.TestIdent.TestID);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Patient ID: %s", AmuletWellPlasmaReportMenu_test.TestIdent.PatientID);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "     First: %s", AmuletWellPlasmaReportMenu_test.TestIdent.FirstName);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "      Last: %s", AmuletWellPlasmaReportMenu_test.TestIdent.LastName);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(AmuletWellPlasmaReportMenu_test.TestIdent.DateOfBirth == 0.0) nc = sprintf(buf, "DOB:");
		else{
			dateout_julian(message, AmuletWellPlasmaReportMenu_test.TestIdent.DateOfBirth);
			nc = sprintf(buf, "DOB: %s", message);
		}
		strncpy(&strng[7], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(strcmp(AmuletWellPlasmaReportMenu_test.TestIdent.Sex, "M") == 0) nc = sprintf(buf, "Sex: Male");
		else if(strcmp(AmuletWellPlasmaReportMenu_test.TestIdent.Sex, "F") == 0) nc = sprintf(buf, "Sex: Female");
		else nc = sprintf(buf, "Sex:");
		strncpy(&strng[7], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, " Physician: %s", AmuletWellPlasmaReportMenu_test.TestIdent.Physician);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "   Tech ID: %s", AmuletWellPlasmaReportMenu_test.TestIdent.TechID);
		strncpy(strng, buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Nuclide: I125", 13);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "    ROI: 15 - 80 keV", 20);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background  Time:", 17);
		nc = sprintf(buf, "%.1f sec", AmuletWellPlasmaReportMenu_test.Background.Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Standard    Time:", 17);
		nc = sprintf(buf, "%.1f sec", AmuletWellPlasmaReportMenu_test.Standard_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole Blood Time:", 17);
		nc = sprintf(buf, "%.1f sec", AmuletWellPlasmaReportMenu_test.WholeBlood_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma      Time:", 17);
		nc = sprintf(buf, "%.1f sec", AmuletWellPlasmaReportMenu_test.Plasma_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Dilution   :",12);
		nc = sprintf(buf, "%6.1f", AmuletWellPlasmaReportMenu_test.DilutionFactor);
		strncpy(&strng[26-nc], buf, nc);
		strncpy(&strng[30], "=D", 2);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Sample Vol :",12);
		nc = sprintf(buf, "%6.1f", AmuletWellPlasmaReportMenu_test.SampleVolume);
		strncpy(&strng[26-nc], buf, nc);
		strncpy(&strng[27], "ml =A", 5);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Patient Wt :",12);
		nc = sprintf(buf, "%5.1f", AmuletWellPlasmaReportMenu_test.PatientWeight);
		strncpy(&strng[26-nc], buf, nc);
		strncpy(&strng[27], "kg", 2);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Patient Hct:",12);
		nc = sprintf(buf, "%4.1f", AmuletWellPlasmaReportMenu_test.Hematocrit);
		strncpy(&strng[26-nc], buf, nc);
		strng[27] = '%';
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background :", 12);
		cps = AmuletWellPlasmaReportMenu_test.BackgroundROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =B", AmuletWellPlasmaReportMenu_test.BackgroundROICPM);
		else nc = sprintf(buf, "%.0f cps =B", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Standard   :", 12);
		cps = AmuletWellPlasmaReportMenu_test.StandardROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =W", AmuletWellPlasmaReportMenu_test.StandardROICPM);
		else nc = sprintf(buf, "%.0f cps =W", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole Blood:", 12);
		cps = AmuletWellPlasmaReportMenu_test.WholeBloodROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =S", AmuletWellPlasmaReportMenu_test.WholeBloodROICPM);
		else nc = sprintf(buf, "%.0f cps =S", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma     :", 12);
		cps = AmuletWellPlasmaReportMenu_test.PlasmaROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =L", AmuletWellPlasmaReportMenu_test.PlasmaROICPM);
		else nc = sprintf(buf, "%.0f cps =L", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "------------------------------", 30);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2],"Whole Blood Vol:", 16);
		nc = sprintf(buf, "%5.0f ml   ", AmuletWellPlasmaReportMenu_test.WholeBloodVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellPlasmaReportMenu_test.WholeBloodPerKg);
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
		nc = sprintf(buf, "%5.0f ml   ", AmuletWellPlasmaReportMenu_test.PlasmaVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellPlasmaReportMenu_test.PlasmaPerKg);
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
		nc = sprintf(buf, "%5.0f ml   ", AmuletWellPlasmaReportMenu_test.RBCVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellPlasmaReportMenu_test.RBCPerKg);
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
		nc = sprintf(buf, "%4.1f %%", AmuletWellPlasmaReportMenu_test.CalculatedHematocrit);
		strncpy(&strng[28-nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

        if(AmuletWellPlasmaReportMenu_test.Inactive){
        	lininit(strng,TRUE,prtype);
        	nc = sprintf(buf, "INACTIVE: %s", AmuletWellPlasmaReportMenu_test.InactiveReason);
        	strncpy(strng, buf, nc);
        	pr_write(strng);
        }

		formfeed(prtype);
	}
}

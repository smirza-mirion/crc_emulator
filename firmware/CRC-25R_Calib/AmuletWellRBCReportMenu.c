#define PHASE_WELLRBCREPORT_PRE_INIT	0
#define PHASE_WELLRBCREPORT_WAIT		1
#define PHASE_WELLRBCREPORT_PRINT		2
#define PHASE_WELLRBCREPORT_WHOLESTDSPEC	3
#define PHASE_WELLRBCREPORT_WHOLESAMPSPEC	4
#define PHASE_WELLRBCREPORT_PLASMASTDSPEC	5
#define PHASE_WELLRBCREPORT_PLASMASAMPSPEC	6
#define PHASE_WELLRBCREPORT_BACKGROUND		7

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

WELLRBCTEST AmuletWellRBCReportMenu_test;

void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void PushPageStack(unsigned char ucPage);
void AmuletWellRBCReport_print(void);

void AmuletWellRBCReport_menu(void){
	char message[51];
	float cps;

	switch(m_iPhase){
		case PHASE_WELLRBCREPORT_PRE_INIT:
			GetExtendedTimeInfo(&AmuletWellRBCReportMenu_test.CreatedOn, message);
			SetAmuletString(100, message);

			if(AmuletWellRBCReportMenu_test.TestIdent.DateOfBirth != 0.0){
				dateout_julian(message, AmuletWellRBCReportMenu_test.TestIdent.DateOfBirth);
				SetAmuletString(102, message);
			}
			if(strcmp(AmuletWellRBCReportMenu_test.TestIdent.Sex, "M") == 0) SetAmuletString(103, "Male");
			else if(strcmp(AmuletWellRBCReportMenu_test.TestIdent.Sex, "F") == 0) SetAmuletString(103, "Female");
			SetAmuletString(104, AmuletWellRBCReportMenu_test.TestIdent.TestID);
			SetAmuletString(105, AmuletWellRBCReportMenu_test.TestIdent.PatientID);
			SetAmuletString(106, AmuletWellRBCReportMenu_test.TestIdent.FirstName);
			SetAmuletString(107, AmuletWellRBCReportMenu_test.TestIdent.LastName);
			SetAmuletString(108, AmuletWellRBCReportMenu_test.TestIdent.Physician);
			SetAmuletString(109, AmuletWellRBCReportMenu_test.TestIdent.TechID);

			cps = AmuletWellRBCReportMenu_test.BackgroundROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCReportMenu_test.BackgroundROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(110, message);

			cps = AmuletWellRBCReportMenu_test.WholeBloodStandardROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCReportMenu_test.WholeBloodStandardROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(111, message);

			cps = AmuletWellRBCReportMenu_test.WholeBloodSampleROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCReportMenu_test.WholeBloodSampleROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(112, message);

			cps = AmuletWellRBCReportMenu_test.PlasmaStandardROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCReportMenu_test.PlasmaStandardROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(113, message);

			cps = AmuletWellRBCReportMenu_test.PlasmaSampleROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCReportMenu_test.PlasmaSampleROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(114, message);

			sprintf(message, "%.1f %%", AmuletWellRBCReportMenu_test.DoseHematocrit);
			SetAmuletString(115, message);

			sprintf(message, "%.1f %%", AmuletWellRBCReportMenu_test.PatientHematocrit);
			SetAmuletString(116, message);

			sprintf(message, "%.1f kg", AmuletWellRBCReportMenu_test.PatientWeight);
			SetAmuletString(117, message);

			sprintf(message, "%.0f ml", AmuletWellRBCReportMenu_test.RBCVolume);
			SetAmuletString(118, message);

			sprintf(message, "%.0f ml/kg", AmuletWellRBCReportMenu_test.RBCPerKg);
			SetAmuletString(119, message);

			sprintf(message, "%.0f ml", AmuletWellRBCReportMenu_test.WholeBloodVolume);
			SetAmuletString(120, message);

			sprintf(message, "%.0f ml/kg", AmuletWellRBCReportMenu_test.WholeBloodPerKg);
			SetAmuletString(121, message);

			sprintf(message, "%.0f ml", AmuletWellRBCReportMenu_test.PlasmaVolume);
			SetAmuletString(122, message);

			sprintf(message, "%.0f ml/kg", AmuletWellRBCReportMenu_test.PlasmaPerKg);
			SetAmuletString(123, message);

			SetAmuletByte(100, 0xFF);

			//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(101, 0xFF);

			if(AmuletWellRBCReportMenu_test.Inactive){
				SetAmuletString(99, AmuletWellRBCReportMenu_test.InactiveReason);
				SetAmuletByte(103, 0xFF);
			}else{
				SetAmuletByte(102, 0xFF);
			}

			m_iPhase = PHASE_WELLRBCREPORT_WAIT;
			break;

		case PHASE_WELLRBCREPORT_WAIT:
			break;

		case PHASE_WELLRBCREPORT_PRINT:
			beep_amulet();
			AmuletWellRBCReport_print();
			SetAmuletByte(104, 0xFF);
			m_iPhase = PHASE_WELLRBCREPORT_WAIT;
			break;

		case PHASE_WELLRBCREPORT_WHOLESTDSPEC:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCReportMenu_test.WholeBloodStandard_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Test, Blood Standard");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellRBCReportMenu_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellRBCReportMenu_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLRBCREPORT_WHOLESAMPSPEC:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCReportMenu_test.WholeBloodSample_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Test, Blood Sample");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellRBCReportMenu_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellRBCReportMenu_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLRBCREPORT_PLASMASTDSPEC:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCReportMenu_test.PlasmaStandard_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Test, Plasma Standard");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellRBCReportMenu_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellRBCReportMenu_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLRBCREPORT_PLASMASAMPSPEC:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCReportMenu_test.PlasmaSample_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Test, Plasma Sample");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellRBCReportMenu_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellRBCReportMenu_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLRBCREPORT_BACKGROUND:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCReportMenu_test.Background.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Test, Background");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellRBCReportMenu_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellRBCReportMenu_test.InactiveReason);
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

void AmuletWellRBCReport_print(void){
	char prtype;
	char strng[90];
	char buf[40];
	short nc;
	float cps;
	char message[51];

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheader(prtype, "RBC VOLUME", AmuletWellRBCReportMenu_test.CreatedOn);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "   Test ID: %s", AmuletWellRBCReportMenu_test.TestIdent.TestID);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Patient ID: %s", AmuletWellRBCReportMenu_test.TestIdent.PatientID);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "     First: %s", AmuletWellRBCReportMenu_test.TestIdent.FirstName);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "      Last: %s", AmuletWellRBCReportMenu_test.TestIdent.LastName);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(AmuletWellRBCReportMenu_test.TestIdent.DateOfBirth == 0.0) nc = sprintf(buf, "DOB:");
		else{
			dateout_julian(message, AmuletWellRBCReportMenu_test.TestIdent.DateOfBirth);
			nc = sprintf(buf, "DOB: %s", message);
		}
		strncpy(&strng[7], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(strcmp(AmuletWellRBCReportMenu_test.TestIdent.Sex, "M") == 0) nc = sprintf(buf, "Sex: Male");
		else if(strcmp(AmuletWellRBCReportMenu_test.TestIdent.Sex, "F") == 0) nc = sprintf(buf, "Sex: Female");
		else nc = sprintf(buf, "Sex:");
		strncpy(&strng[7], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, " Physician: %s", AmuletWellRBCReportMenu_test.TestIdent.Physician);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "   Tech ID: %s", AmuletWellRBCReportMenu_test.TestIdent.TechID);
		strncpy(strng, buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Nuclide: Cr51", 13);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "    ROI: 100 - 500 keV", 22);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background  Time:", 17);

		nc = sprintf(buf, "%.0f sec", AmuletWellRBCReportMenu_test.Background.Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole   Std Time:", 17);
		nc = sprintf(buf, "%.0f sec", AmuletWellRBCReportMenu_test.WholeBloodStandard_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma  Std Time:", 17);
		nc = sprintf(buf, "%.0f sec", AmuletWellRBCReportMenu_test.PlasmaStandard_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole  Samp Time:", 17);
		nc = sprintf(buf, "%.0f sec", AmuletWellRBCReportMenu_test.WholeBloodSample_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma Samp Time:", 17);
		nc = sprintf(buf, "%.0f sec", AmuletWellRBCReportMenu_test.PlasmaSample_Spectrum.LiveTime);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Dose    Hct   :", 15);
		nc = sprintf(buf, "%.1f %% =H", AmuletWellRBCReportMenu_test.DoseHematocrit);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Patient Hct   :", 15);
		nc = sprintf(buf, "%.1f %% =C", AmuletWellRBCReportMenu_test.PatientHematocrit);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Patient Weight:", 15);
		nc = sprintf(buf, "%.1f kg", AmuletWellRBCReportMenu_test.PatientWeight);
		strncpy(&strng[30-nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background :", 12);

		cps = AmuletWellRBCReportMenu_test.BackgroundROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm=B", AmuletWellRBCReportMenu_test.BackgroundROICPM);
		else nc = sprintf(buf, "%.0f cps =B", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole   Std:", 12);
		cps = AmuletWellRBCReportMenu_test.WholeBloodStandardROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm=W", AmuletWellRBCReportMenu_test.WholeBloodStandardROICPM);
		else nc = sprintf(buf, "%.0f cps =W", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Whole  Samp:", 12);
		cps = AmuletWellRBCReportMenu_test.WholeBloodSampleROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm=S", AmuletWellRBCReportMenu_test.WholeBloodSampleROICPM);
		else nc = sprintf(buf, "%.0f cps =S", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma  Std:", 12);
		cps = AmuletWellRBCReportMenu_test.PlasmaStandardROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm=P", AmuletWellRBCReportMenu_test.PlasmaStandardROICPM);
		else nc = sprintf(buf, "%.0f cps =P", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Plasma Samp:", 12);
		cps = AmuletWellRBCReportMenu_test.PlasmaSampleROICPM / 60.0;
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm=L", AmuletWellRBCReportMenu_test.PlasmaSampleROICPM);
		else nc = sprintf(buf, "%.0f cps =L", cps);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "==============================", 30);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2],"RBC Vol:", 8);
		nc = sprintf(buf, "%5.0f ml   ", AmuletWellRBCReportMenu_test.RBCVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellRBCReportMenu_test.RBCPerKg);
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

		nc = sprintf(buf, "%5.0f ml   ", AmuletWellRBCReportMenu_test.WholeBloodVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellRBCReportMenu_test.WholeBloodPerKg);
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
		nc = sprintf(buf, "%5.0f ml   ", AmuletWellRBCReportMenu_test.PlasmaVolume);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%5.0f ml/kg", AmuletWellRBCReportMenu_test.PlasmaPerKg);
		strncpy(&strng[32-nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "= Whole Blood Vol - RBC Vol   ", 30);
		pr_write(strng);
		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "==============================", 30);
		pr_write(strng);

		feed(1, prtype);

		if(AmuletWellRBCReportMenu_test.Inactive){
        	lininit(strng,TRUE,prtype);
        	nc = sprintf(buf, "INACTIVE: %s", AmuletWellRBCReportMenu_test.InactiveReason);
        	strncpy(strng, buf, nc);
        	pr_write(strng);
        }

		formfeed(prtype);
	}
}

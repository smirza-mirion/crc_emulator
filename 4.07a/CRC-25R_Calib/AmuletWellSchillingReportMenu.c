#define PHASE_WELLSCHILLINGREPORT_PRE_INIT	0
#define PHASE_WELLSCHILLINGREPORT_WAIT		1
#define PHASE_WELLSCHILLINGREPORT_PRINT		2
#define PHASE_WELLSCHILLINGREPORT_STDSPECTRUM	3
#define PHASE_WELLSCHILLINGREPORT_ALQSPECTRUM	4
#define PHASE_WELLSCHILLINGREPORT_BACKGND		5

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "printer.h"

extern int m_iPhase;
extern CURRENT current;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;

WELLSCHILLINGTEST AmuletWellSchillingReport_test;

void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
static void AmuletWellSchillingReport_print(void);
void PushPageStack(unsigned char ucPage);

void AmuletWellSchillingReport_menu(void){
	char message[51];
	float cps;

	switch(m_iPhase){
		case PHASE_WELLSCHILLINGREPORT_PRE_INIT:
			GetExtendedTimeInfo(&AmuletWellSchillingReport_test.CreatedOn, message);
			SetAmuletString(100, message);

			if(AmuletWellSchillingReport_test.TestIdent.DateOfBirth != 0.0){
				dateout_julian(message, AmuletWellSchillingReport_test.TestIdent.DateOfBirth);
				SetAmuletString(102, message);
			}
			if(strcmp(AmuletWellSchillingReport_test.TestIdent.Sex, "M") == 0) SetAmuletString(103, "Male");
			else if(strcmp(AmuletWellSchillingReport_test.TestIdent.Sex, "F") == 0) SetAmuletString(103, "Female");
			SetAmuletString(104, AmuletWellSchillingReport_test.TestIdent.TestID);
			SetAmuletString(105, AmuletWellSchillingReport_test.TestIdent.PatientID);
			SetAmuletString(106, AmuletWellSchillingReport_test.TestIdent.FirstName);
			SetAmuletString(107, AmuletWellSchillingReport_test.TestIdent.LastName);
			SetAmuletString(108, AmuletWellSchillingReport_test.TestIdent.Physician);
			SetAmuletString(109, AmuletWellSchillingReport_test.TestIdent.TechID);
			cps = AmuletWellSchillingReport_test.BackgroundROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellSchillingReport_test.BackgroundROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(110, message);

			cps = AmuletWellSchillingReport_test.StandardROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellSchillingReport_test.StandardROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(111, message);

			cps = AmuletWellSchillingReport_test.AliquotROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellSchillingReport_test.AliquotROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(112, message);

			sprintf(message, "%.1f", AmuletWellSchillingReport_test.DilutionFactor);
			SetAmuletString(113, message);

			sprintf(message, "%.1f ml", AmuletWellSchillingReport_test.UrineVolume);
			SetAmuletString(114, message);

			sprintf(message, "%.1f ml", AmuletWellSchillingReport_test.AliquotVolume);
			SetAmuletString(115, message);

			sprintf(message, "%.1f %%", AmuletWellSchillingReport_test.Excretion);
			SetAmuletString(116, message);

			SetAmuletByte(100, 0xFF);

			//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(101, 0xFF);

			if(AmuletWellSchillingReport_test.Inactive){
				SetAmuletString(99, AmuletWellSchillingReport_test.InactiveReason);
				SetAmuletByte(103, 0xFF);
			}else{
				SetAmuletByte(102, 0xFF);
			}

			m_iPhase = PHASE_WELLSCHILLINGREPORT_WAIT;
			break;

		case PHASE_WELLSCHILLINGREPORT_WAIT:
			break;

		case PHASE_WELLSCHILLINGREPORT_PRINT:
			beep_amulet();
			AmuletWellSchillingReport_print();
			SetAmuletByte(104, 0xFF);
			m_iPhase = PHASE_WELLSCHILLINGREPORT_WAIT;
			break;

		case PHASE_WELLSCHILLINGREPORT_STDSPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellSchillingReport_test.SchillingStandard_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Schilling Standard");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellSchillingReport_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellSchillingReport_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLSCHILLINGREPORT_ALQSPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellSchillingReport_test.SchillingAliquot_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Schilling Aliquot");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellSchillingReport_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellSchillingReport_test.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLSCHILLINGREPORT_BACKGND:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellSchillingReport_test.Background.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Schilling Background");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellSchillingReport_test.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellSchillingReport_test.InactiveReason);
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

static void AmuletWellSchillingReport_print(void){
	char prtype;
	char strng[90];
	char buf[40];
	short nc;
	float cps;
	char message[51];

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheader(prtype, "SCHILLING TEST", AmuletWellSchillingReport_test.CreatedOn);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "   Test ID: %s", AmuletWellSchillingReport_test.TestIdent.TestID);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Patient ID: %s", AmuletWellSchillingReport_test.TestIdent.PatientID);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "     First: %s", AmuletWellSchillingReport_test.TestIdent.FirstName);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "      Last: %s", AmuletWellSchillingReport_test.TestIdent.LastName);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(AmuletWellSchillingReport_test.TestIdent.DateOfBirth == 0.0) nc = sprintf(buf, "DOB:");
		else{
			dateout_julian(message, AmuletWellSchillingReport_test.TestIdent.DateOfBirth);
			nc = sprintf(buf, "DOB: %s", message);
		}
		strncpy(&strng[7], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(strcmp(AmuletWellSchillingReport_test.TestIdent.Sex, "M") == 0) nc = sprintf(buf, "Sex: Male");
		else if(strcmp(AmuletWellSchillingReport_test.TestIdent.Sex, "F") == 0) nc = sprintf(buf, "Sex: Female");
		else nc = sprintf(buf, "Sex:");
		strncpy(&strng[7], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, " Physician: %s", AmuletWellSchillingReport_test.TestIdent.Physician);
		strncpy(strng, buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "   Tech ID: %s", AmuletWellSchillingReport_test.TestIdent.TechID);
		strncpy(strng, buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Nuclide: Co57", 13);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "    ROI: 50 - 200 keV", 21);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background Time:", 16);
		nc = sprintf(buf, "%.1f sec", AmuletWellSchillingReport_test.Background.Spectrum.LiveTime);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Standard   Time:", 16);
		nc = sprintf(buf, "%.1f sec", AmuletWellSchillingReport_test.SchillingStandard_Spectrum.LiveTime);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Aliquot    Time:", 16);
		nc = sprintf(buf, "%.1f sec", AmuletWellSchillingReport_test.SchillingAliquot_Spectrum.LiveTime);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
        strncpy(&strng[2], "Dilution Factor: ", 17);
        nc = sprintf(buf, "%5.1f", AmuletWellSchillingReport_test.DilutionFactor);
        strncpy(&strng[26 - nc], buf, nc);
        strncpy(&strng[30], "=D", 2);
        pr_write(strng);

        lininit(strng,TRUE,prtype);
        strncpy(&strng[2],"Aliquot  Volume: ",17);
        nc = sprintf(buf, "%5.1f", AmuletWellSchillingReport_test.AliquotVolume);
        strncpy(&strng[26 - nc], buf, nc);
        strncpy(&strng[27], "ml =A", 5);
        pr_write(strng);

        lininit(strng,TRUE,prtype);
        strncpy(&strng[2], "Urine    Volume: ", 17);
        nc = sprintf(buf, "%6.1f", AmuletWellSchillingReport_test.UrineVolume);
        strncpy(&strng[26 - nc], buf, nc);
        strncpy(&strng[27], "ml =V", 5);
        pr_write(strng);

        feed(1, prtype);

		cps = AmuletWellSchillingReport_test.BackgroundROICPM / 60.0;
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background:", 11);
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =B", AmuletWellSchillingReport_test.BackgroundROICPM);
		else nc = sprintf(buf, "%.0f cps =B", cps);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		cps = AmuletWellSchillingReport_test.StandardROICPM / 60.0;
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Standard  :", 11);
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =S", AmuletWellSchillingReport_test.StandardROICPM);
		else nc = sprintf(buf, "%.0f cps =S", cps);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		cps = AmuletWellSchillingReport_test.AliquotROICPM / 60.0;
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Aliquot   :", 11);
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =U", AmuletWellSchillingReport_test.AliquotROICPM);
		else nc = sprintf(buf, "%.0f cps =U", cps);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], " Excretion:", 11);
		nc = sprintf(buf, "%.1f %%", AmuletWellSchillingReport_test.Excretion);
		strncpy(&strng[26 - nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng,TRUE,prtype);
		strncpy(&strng[20],"(U-B)*(V/A)",11);
        pr_write(strng);

        lininit(strng,TRUE,prtype);
        strncpy(&strng[2],"% Excretion = 100*-----------",29);
        pr_write(strng);

        lininit(strng,TRUE,prtype);
        strncpy(&strng[22],"(S-B)*D",7);
        pr_write(strng);

        feed(1, prtype);
        if(AmuletWellSchillingReport_test.Inactive){
        	lininit(strng,TRUE,prtype);
        	nc = sprintf(buf, "INACTIVE: %s", AmuletWellSchillingReport_test.InactiveReason);
        	strncpy(strng, buf, nc);
        	pr_write(strng);
        }

        formfeed(prtype);
	}
}




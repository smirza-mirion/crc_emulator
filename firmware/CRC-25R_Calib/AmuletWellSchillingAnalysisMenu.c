#define PHASE_WELLSCHILLINGANALYSIS_PRE_INIT	0
#define PHASE_WELLSCHILLINGANALYSIS_WAIT		1
#define PHASE_WELLSCHILLINGANALYSIS_PRINT		2
#define PHASE_WELLSCHILLINGANALYSIS_FINISH		3
#define PHASE_WELLSCHILLINGANALYSIS_STDSPECTRUM	4
#define PHASE_WELLSCHILLINGANALYSIS_ALQSPECTRUM	5
#define PHASE_WELLSCHILLINGANALYSIS_BACKGND		6

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "printer.h"

extern int m_iPhase;
extern WELLSCHILLINGTEST AmuletWellSchillingMenu_test;
extern CURRENT current;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;

void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
static void AmuletWellSchillingAnalysis_print(void);
void PushPageStack(unsigned char ucPage);
unsigned char PopPageStack(void);
void SetAmuletBackHTML(void);

void AmuletWellSchillingAnalysis_menu(void){
	char message[51];
	float cps;

	switch(m_iPhase){
		case PHASE_WELLSCHILLINGANALYSIS_PRE_INIT:
			GetExtendedTimeInfo(&AmuletWellSchillingMenu_test.CreatedOn, message);
			SetAmuletString(100, message);

			cps = AmuletWellSchillingMenu_test.BackgroundROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellSchillingMenu_test.BackgroundROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(102, message);

			cps = AmuletWellSchillingMenu_test.StandardROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellSchillingMenu_test.StandardROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(103, message);

			cps = AmuletWellSchillingMenu_test.AliquotROICPM / 60.0;
			if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellSchillingMenu_test.AliquotROICPM);
			else sprintf(message, "%.0f cps", cps);
			SetAmuletString(104, message);

			sprintf(message, "%.1f", AmuletWellSchillingMenu_test.DilutionFactor);
			SetAmuletString(105, message);

			sprintf(message, "%.1f ml", AmuletWellSchillingMenu_test.UrineVolume);
			SetAmuletString(106, message);

			sprintf(message, "%.1f ml", AmuletWellSchillingMenu_test.AliquotVolume);
			SetAmuletString(107, message);

			sprintf(message, "%.1f %%", AmuletWellSchillingMenu_test.Excretion);
			SetAmuletString(108, message);

			SetAmuletByte(100, 0xFF);

			//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(101, 0xFF);

			m_iPhase = PHASE_WELLSCHILLINGANALYSIS_WAIT;
			break;

		case PHASE_WELLSCHILLINGANALYSIS_WAIT:
			break;

		case PHASE_WELLSCHILLINGANALYSIS_PRINT:
			beep_amulet();
			AmuletWellSchillingAnalysis_print();
			SetAmuletByte(102, 0xFF);
			m_iPhase = PHASE_WELLSCHILLINGANALYSIS_WAIT;
			break;

		case PHASE_WELLSCHILLINGANALYSIS_FINISH:
			beep_amulet();
			PopPageStack();
			SetAmuletBackHTML();
			break;

		case PHASE_WELLSCHILLINGANALYSIS_STDSPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellSchillingMenu_test.SchillingStandard_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Schilling Standard");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLSCHILLINGANALYSIS_ALQSPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellSchillingMenu_test.SchillingAliquot_Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Schilling Aliquot");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLSCHILLINGANALYSIS_BACKGND:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellSchillingMenu_test.Background.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Schilling Background");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;
	}
}

static void AmuletWellSchillingAnalysis_print(void){
	char prtype;
	char strng[90];
	char buf[40];
	short nc;
	float cps;

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheader(prtype, "SCHILLING TEST", AmuletWellSchillingMenu_test.CreatedOn);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Nuclide: Co57", 13);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "    ROI: 50 - 200 keV", 21);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background Time:", 16);
		nc = sprintf(buf, "%.1f sec", AmuletWellSchillingMenu_test.Background.Spectrum.LiveTime);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Standard   Time:", 16);
		nc = sprintf(buf, "%.1f sec", AmuletWellSchillingMenu_test.SchillingStandard_Spectrum.LiveTime);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Aliquot    Time:", 16);
		nc = sprintf(buf, "%.1f sec", AmuletWellSchillingMenu_test.SchillingAliquot_Spectrum.LiveTime);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
        strncpy(&strng[2], "Dilution Factor: ", 17);
        nc = sprintf(buf, "%5.1f", AmuletWellSchillingMenu_test.DilutionFactor);
        strncpy(&strng[26 - nc], buf, nc);
        strncpy(&strng[30], "=D", 2);
        pr_write(strng);

        lininit(strng,TRUE,prtype);
        strncpy(&strng[2],"Aliquot  Volume: ",17);
        nc = sprintf(buf, "%5.1f", AmuletWellSchillingMenu_test.AliquotVolume);
        strncpy(&strng[26 - nc], buf, nc);
        strncpy(&strng[27], "ml =A", 5);
        pr_write(strng);

        lininit(strng,TRUE,prtype);
        strncpy(&strng[2], "Urine    Volume: ", 17);
        nc = sprintf(buf, "%6.1f", AmuletWellSchillingMenu_test.UrineVolume);
        strncpy(&strng[26 - nc], buf, nc);
        strncpy(&strng[27], "ml =V", 5);
        pr_write(strng);

        feed(1, prtype);

		cps = AmuletWellSchillingMenu_test.BackgroundROICPM / 60.0;
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Background:", 11);
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =B", AmuletWellSchillingMenu_test.BackgroundROICPM);
		else nc = sprintf(buf, "%.0f cps =B", cps);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		cps = AmuletWellSchillingMenu_test.StandardROICPM / 60.0;
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Standard  :", 11);
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =S", AmuletWellSchillingMenu_test.StandardROICPM);
		else nc = sprintf(buf, "%.0f cps =S", cps);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		cps = AmuletWellSchillingMenu_test.AliquotROICPM / 60.0;
		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Aliquot   :", 11);
		if(current.system == CI) nc = sprintf(buf, "%.0f cpm =U", AmuletWellSchillingMenu_test.AliquotROICPM);
		else nc = sprintf(buf, "%.0f cps =U", cps);
		strncpy(&strng[32 - nc], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], " Excretion:", 11);
		nc = sprintf(buf, "%.1f %%", AmuletWellSchillingMenu_test.Excretion);
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

        formfeed(prtype);
	}
}




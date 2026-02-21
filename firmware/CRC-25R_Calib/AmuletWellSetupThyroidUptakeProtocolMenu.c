#define PHASE_WELLSETUPTUPROTOCOL_PRE_INIT	0
#define PHASE_WELLSETUPTUPROTOCOL_WAIT		1
#define PHASE_WELLSETUPTUPROTOCOL_PGDOWN		2
#define PHASE_WELLSETUPTUPROTOCOL_PGUP		3
#define PHASE_WELLSETUPTUPROTOCOL_ROW1		4
#define PHASE_WELLSETUPTUPROTOCOL_ROW2		5
#define PHASE_WELLSETUPTUPROTOCOL_ROW3		6
#define PHASE_WELLSETUPTUPROTOCOL_ROW4		7
#define PHASE_WELLSETUPTUPROTOCOL_ROW5		8
#define PHASE_WELLSETUPTUPROTOCOL_ROW6		9
#define PHASE_WELLSETUPTUPROTOCOL_ROW7		10
#define PHASE_WELLSETUPTUPROTOCOL_ROW8		11
#define PHASE_WELLSETUPTUPROTOCOL_ROW9		12
#define PHASE_WELLSETUPTUPROTOCOL_ROW10		13
#define PHASE_WELLSETUPTUPROTOCOL_HOME		14
#define PHASE_WELLSETUPTUPROTOCOL_BACK		15
#define PHASE_WELLSETUPTUPROTOCOL_ADD		16
#define PHASE_WELLSETUPTUPROTOCOL_EDIT		17
#define PHASE_WELLSETUPTUPROTOCOL_DELETE	18
#define PHASE_WELLSETUPTUPROTOCOL_SELECT	19
#define PHASE_WELLSETUPTUPROTOCOL_PRINT		20
#define PHASE_WELLSETUPTUPROTOCOL_PRINTALL	21
#define PHASE_WELLSETUPTUPROTOCOL_POST_PRINT 22

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "mca.h"
#include "database.h"
#include "amulet.h"
#include "nuc.h"
#include "printer.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern PROBETHYROIDUPTAKEPROTOCOL AmuletAddEditThyroidUptakeProtocol_protocol;
extern PROBETHYROIDUPTAKETEST AmuletAddEditThyroidUptakeTest_test;
extern time_t clock_time;

bool AmuletWellSetupThyroidUptakeProtocol_setupMode;
static int AmuletWellSetupThyroidUptakeProtocol_count;
static PROBETHYROIDUPTAKEPROTOCOLACTIVENAME *AmuletWellSetupThyroidUptakeProtocol;
static int AmuletWellSetupThyroidUptakeProtocol_currentPage;	// Zero based
static int AmuletWellSetupThyroidUptakeProtocol_lastPage;		// Zero based
static int AmuletWellSetupThyroidUptakeProtocol_currentIndex;	// Zero based
static PROBETHYROIDUPTAKEPROTOCOL protocol;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletHomeHTML(void);
void SetAmuletBackHTML(void);
void PushPageStack(unsigned char ucPage);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void AmuletWellSetupThyroidUptakeProtocolMenu_printProtocol(long long int id);
void AmuletWellSetupThyroidUptakeProtocolMenu_printAll(void);

static void AmuletWellSetupThyroidUptakeProtocol_displayPage(void);

void AmuletWellSetupThyroidUptakeProtocol_displayInitial(void){
	if(AmuletWellSetupThyroidUptakeProtocol_setupMode){
				SetAmuletString(98, "Setup Thyroid Uptake Protocol");
				SetAmuletByte(90, 0xFF);
				SetAmuletByte(91, 0xFF);
				SetAmuletByte(92, 0xFF);
				SetAmuletByte(93, 0xFF);
				SetAmuletByte(94, 0xFF);
				SetAmuletByte(95, 0x00);
			}else{
				SetAmuletString(98, "Select Thyroid Uptake Protocol");
				SetAmuletByte(90, 0x00);
				SetAmuletByte(91, 0xFF);
				SetAmuletByte(92, 0x00);
				SetAmuletByte(93, 0x00);
				SetAmuletByte(94, 0x00);
				SetAmuletByte(95, 0xFF);
			}

			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER) && AmuletWellSetupThyroidUptakeProtocol_setupMode) SetAmuletByte(98, 0xFF);
			else SetAmuletByte(98, 0);

			AmuletWellSetupThyroidUptakeProtocol_count = DB_ActiveProbeThyroidUptakeProtocolCount();
			if(AmuletWellSetupThyroidUptakeProtocol_count == 0) AmuletWellSetupThyroidUptakeProtocol_lastPage = 0;
			else{
				AmuletWellSetupThyroidUptakeProtocol_lastPage = AmuletWellSetupThyroidUptakeProtocol_count / 10;
				if(AmuletWellSetupThyroidUptakeProtocol_count % 10) AmuletWellSetupThyroidUptakeProtocol_lastPage++;
				AmuletWellSetupThyroidUptakeProtocol_lastPage--;
			}
			if(AmuletWellSetupThyroidUptakeProtocol_count){
				AmuletWellSetupThyroidUptakeProtocol = malloc(AmuletWellSetupThyroidUptakeProtocol_count * sizeof(PROBETHYROIDUPTAKEPROTOCOLACTIVENAME));
				DB_RetrieveActiveProbeThyroidUptakeProtocolNames(AmuletWellSetupThyroidUptakeProtocol);
			}
			AmuletWellSetupThyroidUptakeProtocol_currentPage = 0;
			AmuletWellSetupThyroidUptakeProtocol_displayPage();
			SetAmuletByte(99, 0xFF);
}

void AmuletWellSetupThyroidUptakeProtocol_menu(void){
	switch(m_iPhase){
		case PHASE_WELLSETUPTUPROTOCOL_PRE_INIT:
			if(AmuletWellSetupThyroidUptakeProtocol_setupMode){
				SetAmuletString(98, "Setup Thyroid Uptake Protocol");
				SetAmuletByte(90, 0xFF);
				SetAmuletByte(91, 0xFF);
				SetAmuletByte(92, 0xFF);
				SetAmuletByte(93, 0xFF);
				SetAmuletByte(94, 0xFF);
				SetAmuletByte(95, 0x00);
			}else{
				SetAmuletString(98, "Select Thyroid Uptake Protocol");
				SetAmuletByte(90, 0x00);
				SetAmuletByte(91, 0xFF);
				SetAmuletByte(92, 0x00);
				SetAmuletByte(93, 0x00);
				SetAmuletByte(94, 0x00);
				SetAmuletByte(95, 0xFF);
			}

			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER) && AmuletWellSetupThyroidUptakeProtocol_setupMode) SetAmuletByte(98, 0xFF);
			else SetAmuletByte(98, 0);

			AmuletWellSetupThyroidUptakeProtocol_count = DB_ActiveProbeThyroidUptakeProtocolCount();
			if(AmuletWellSetupThyroidUptakeProtocol_count == 0) AmuletWellSetupThyroidUptakeProtocol_lastPage = 0;
			else{
				AmuletWellSetupThyroidUptakeProtocol_lastPage = AmuletWellSetupThyroidUptakeProtocol_count / 10;
				if(AmuletWellSetupThyroidUptakeProtocol_count % 10) AmuletWellSetupThyroidUptakeProtocol_lastPage++;
				AmuletWellSetupThyroidUptakeProtocol_lastPage--;
			}
			if(AmuletWellSetupThyroidUptakeProtocol_count){
				AmuletWellSetupThyroidUptakeProtocol = malloc(AmuletWellSetupThyroidUptakeProtocol_count * sizeof(PROBETHYROIDUPTAKEPROTOCOLACTIVENAME));
				DB_RetrieveActiveProbeThyroidUptakeProtocolNames(AmuletWellSetupThyroidUptakeProtocol);
			}
			AmuletWellSetupThyroidUptakeProtocol_currentPage = 0;
			AmuletWellSetupThyroidUptakeProtocol_displayPage();
			SetAmuletByte(99, 0xFF);

			m_iPhase = PHASE_WELLSETUPTUPROTOCOL_WAIT;
			break;

		case PHASE_WELLSETUPTUPROTOCOL_WAIT:
			break;

		case PHASE_WELLSETUPTUPROTOCOL_PGDOWN:
			if(AmuletWellSetupThyroidUptakeProtocol_currentPage < AmuletWellSetupThyroidUptakeProtocol_lastPage){
				beep_amulet();
				AmuletWellSetupThyroidUptakeProtocol_currentPage++;
				AmuletWellSetupThyroidUptakeProtocol_displayPage();
			}
			m_iPhase = PHASE_WELLSETUPTUPROTOCOL_WAIT;
			break;

		case PHASE_WELLSETUPTUPROTOCOL_PGUP:
			if(AmuletWellSetupThyroidUptakeProtocol_currentPage > 0){
				beep_amulet();
				AmuletWellSetupThyroidUptakeProtocol_currentPage--;
				AmuletWellSetupThyroidUptakeProtocol_displayPage();
			}
			m_iPhase = PHASE_WELLSETUPTUPROTOCOL_WAIT;
			break;

		case PHASE_WELLSETUPTUPROTOCOL_ROW1:
		case PHASE_WELLSETUPTUPROTOCOL_ROW2:
		case PHASE_WELLSETUPTUPROTOCOL_ROW3:
		case PHASE_WELLSETUPTUPROTOCOL_ROW4:
		case PHASE_WELLSETUPTUPROTOCOL_ROW5:
		case PHASE_WELLSETUPTUPROTOCOL_ROW6:
		case PHASE_WELLSETUPTUPROTOCOL_ROW7:
		case PHASE_WELLSETUPTUPROTOCOL_ROW8:
		case PHASE_WELLSETUPTUPROTOCOL_ROW9:
		case PHASE_WELLSETUPTUPROTOCOL_ROW10:
			beep_amulet();
			AmuletWellSetupThyroidUptakeProtocol_currentIndex = 10 * AmuletWellSetupThyroidUptakeProtocol_currentPage + (m_iPhase - PHASE_WELLSETUPTUPROTOCOL_ROW1);
			m_iPhase = PHASE_WELLSETUPTUPROTOCOL_WAIT;
			break;

		case PHASE_WELLSETUPTUPROTOCOL_HOME:
			beep_amulet();
			if(AmuletWellSetupThyroidUptakeProtocol_count) free(AmuletWellSetupThyroidUptakeProtocol);
			SetAmuletHomeHTML();
			return;

		case PHASE_WELLSETUPTUPROTOCOL_BACK:
			if(AmuletWellSetupThyroidUptakeProtocol_count) free(AmuletWellSetupThyroidUptakeProtocol);
			SetAmuletBackHTML();
			return;

		case PHASE_WELLSETUPTUPROTOCOL_ADD:
			beep_amulet();
			AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID = -1;
			m_ucClear = 88;
			SetAmuletHTML(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
			PushPageStack(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
			if(AmuletWellSetupThyroidUptakeProtocol_count) free(AmuletWellSetupThyroidUptakeProtocol);
			return;

		case PHASE_WELLSETUPTUPROTOCOL_EDIT:
			beep_amulet();
			AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID = AmuletWellSetupThyroidUptakeProtocol[AmuletWellSetupThyroidUptakeProtocol_currentIndex].ProbeTUProtocolID;
			AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID = 0;
			m_ucClear = 88;
			SetAmuletHTML(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
			PushPageStack(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
			if(AmuletWellSetupThyroidUptakeProtocol_count) free(AmuletWellSetupThyroidUptakeProtocol);
			return;

		case PHASE_WELLSETUPTUPROTOCOL_DELETE:
			beep_amulet();
			AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID = AmuletWellSetupThyroidUptakeProtocol[AmuletWellSetupThyroidUptakeProtocol_currentIndex].ProbeTUProtocolID;
			AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID = -2;
			m_ucClear = 88;
			SetAmuletHTML(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
			PushPageStack(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
			if(AmuletWellSetupThyroidUptakeProtocol_count) free(AmuletWellSetupThyroidUptakeProtocol);
			return;

		case PHASE_WELLSETUPTUPROTOCOL_SELECT:
			beep_amulet();
			AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID = AmuletWellSetupThyroidUptakeProtocol[AmuletWellSetupThyroidUptakeProtocol_currentIndex].ProbeTUProtocolID;
			protocol.ProbeTUProtocolID = AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID;
			DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
			if(protocol.ProbeTUProtocolID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
				return;
			}
			if(AmuletAddEditThyroidUptakeTest_test.CountingTime == -1) AmuletAddEditThyroidUptakeTest_test.CountingTime = protocol.DefaultCountTime;
			if(AmuletAddEditThyroidUptakeTest_test.ProbeDistance == -1) AmuletAddEditThyroidUptakeTest_test.ProbeDistance = protocol.DefaultDistance;
			switch(protocol.NuclideID){
				case 40:
					strcpy(AmuletAddEditThyroidUptakeTest_test.NuclideName, "I123");
					AmuletAddEditThyroidUptakeTest_test.StartROI = protocol.I123LowerEV;
					AmuletAddEditThyroidUptakeTest_test.EndROI = protocol.I123UpperEV;
					break;

				case 45:
					strcpy(AmuletAddEditThyroidUptakeTest_test.NuclideName, "I131");
					AmuletAddEditThyroidUptakeTest_test.StartROI = protocol.I131LowerEV;
					AmuletAddEditThyroidUptakeTest_test.EndROI = protocol.I131UpperEV;
					break;

				case 84:
					strcpy(AmuletAddEditThyroidUptakeTest_test.NuclideName, "Tc99m");
					AmuletAddEditThyroidUptakeTest_test.StartROI = protocol.Tc99mLowerEV;
					AmuletAddEditThyroidUptakeTest_test.EndROI = protocol.Tc99mUpperEV;
					break;
			}
			AmuletAddEditThyroidUptakeTest_test.HalfLife = NuclideData_getHalflife(protocol.NuclideID);
			AmuletAddEditThyroidUptakeTest_test.HalfLifeUnit = NuclideData_getHalflifeUnit(protocol.NuclideID);

			AmuletAddEditThyroidUptakeTest_test.DoseMultiplier = -1;

			if(AmuletWellSetupThyroidUptakeProtocol_count) free(AmuletWellSetupThyroidUptakeProtocol);
			SetAmuletBackHTML();
			return;

		case PHASE_WELLSETUPTUPROTOCOL_PRINT:
			beep_amulet();
			AmuletWellSetupThyroidUptakeProtocolMenu_printProtocol(AmuletWellSetupThyroidUptakeProtocol[AmuletWellSetupThyroidUptakeProtocol_currentIndex].ProbeTUProtocolID);
			//SetAmuletByte(88, 0xFF);
			//SetAmuletByte(87, 0xFF);
			AmuletWellSetupThyroidUptakeProtocol_displayPage();
			SetAmuletByte(99, 0xFF);
			m_iPhase = PHASE_WELLSETUPTUPROTOCOL_WAIT;
			break;

		case PHASE_WELLSETUPTUPROTOCOL_PRINTALL:
			beep_amulet();
			AmuletWellSetupThyroidUptakeProtocolMenu_printAll();
			//SetAmuletByte(87, 0xFF);
			//if(AmuletWellSetupThyroidUptakeProtocol_currentIndex != -1) SetAmuletByte(88, 0xFF);
			AmuletWellSetupThyroidUptakeProtocol_displayPage();
			SetAmuletByte(99, 0xFF);
			m_iPhase = PHASE_WELLSETUPTUPROTOCOL_WAIT;
			break;

		//case PHASE_WELLSETUPTUPROTOCOL_POST_PRINT:
		//	AmuletWellSetupThyroidUptakeProtocol_displayInitial();
		//	m_iPhase = PHASE_WELLSETUPTUPROTOCOL_WAIT;
		//	break;
	}
}

static void AmuletWellSetupThyroidUptakeProtocol_displayPage(void){
	int index, indexbegin;
	char message[51];

	if(AmuletWellSetupThyroidUptakeProtocol_lastPage == 0){
		SetAmuletByte(96, 0x00);
		SetAmuletByte(97, 0x00);
	}else{
		if(AmuletWellSetupThyroidUptakeProtocol_currentPage == 0){
			SetAmuletByte(96, 0x00);
			SetAmuletByte(97, 0xFF);
		}else if(AmuletWellSetupThyroidUptakeProtocol_currentPage == AmuletWellSetupThyroidUptakeProtocol_lastPage){
			SetAmuletByte(96, 0xFF);
			SetAmuletByte(97, 0x00);
		}else{
			SetAmuletByte(96, 0xFF);
			SetAmuletByte(97, 0xFF);
		}
	}

	if(AmuletWellSetupThyroidUptakeProtocol_count > 0){
		if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER) && AmuletWellSetupThyroidUptakeProtocol_setupMode) SetAmuletByte(89, 0xFF);
		else SetAmuletByte(89, 0);

		indexbegin = 10 * AmuletWellSetupThyroidUptakeProtocol_currentPage;
		for(index=indexbegin; index<indexbegin+10; index++){
			if(index < AmuletWellSetupThyroidUptakeProtocol_count){
				SetAmuletString(100 + 2*(index-indexbegin), AmuletWellSetupThyroidUptakeProtocol[index].ProtocolName);
				SetAmuletByte(101 + (index-indexbegin), 0xFF);
			}else{
				SetAmuletString(100 + 2*(index-indexbegin), "");
				SetAmuletByte(101 + (index-indexbegin), 0);
			}
		}

		message[0] = 0;
		if(AmuletWellSetupThyroidUptakeProtocol_lastPage > 0){
			sprintf(message, "%d of %d", AmuletWellSetupThyroidUptakeProtocol_currentPage + 1, AmuletWellSetupThyroidUptakeProtocol_lastPage + 1);
		}
		SetAmuletString(150, message);

		SetAmuletByte(100, 0xFF);
		SetAmuletByte(111, 0xFF);
	}

	AmuletWellSetupThyroidUptakeProtocol_currentIndex = -1;
}

void AmuletWellSetupThyroidUptakeProtocolMenu_printProtocol(long long int id){
	char prtype;
	char strng[90];
	char buff[90];

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		protocol.ProbeTUProtocolID = id;
		DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
		if(protocol.ProbeTUProtocolID > 0){
			rawheader(prtype, "Thyroid Uptake Protocol",  clock_time);
			pr_set_linecnt(5);

			lininit(strng, TRUE, prtype);

			// Thyroid Uptake Protocol Name
			insertstring(strng, 0, 0, 0, "Name: %s", protocol.ProtocolName);
			pr_write2(strng, "Thyroid Uptake Protocol");
			pr_write2(strng, "Thyroid Uptake Protocol");

			// Counting Method - Dose Decay or Reference
			if(protocol.DoseDecayCorrect){
				insertconst(strng, 0, 0, 0, "Counting Method: Decay Correct");
				pr_write2(strng, "Thyroid Uptake Protocol");
				insertconst(strng, 0, 0, 0, "Administered Dose");
				pr_write2(strng, "Thyroid Uptake Protocol");
			}else{
				insertconst(strng, 0, 0, 0, "Counting Method: Measure Same");
				pr_write2(strng, "Thyroid Uptake Protocol");
				insertconst(strng, 0, 0, 0, "Reference Dose Before Each");
				pr_write2(strng, "Thyroid Uptake Protocol");
				insertconst(strng, 0, 0, 0, "Uptake");
				pr_write2(strng, "Thyroid Uptake Protocol");
			}
			pr_write2(strng, "Thyroid Uptake Protocol");

			// Dose Measurement
			switch(protocol.DoseMeasurementMethod){
				case 1:
					insertconst(strng, 0, 0, 0, "Dose Measurement: Dose Measured");
					pr_write2(strng, "Thyroid Uptake Protocol");
					insertconst(strng, 0, 0, 0, "is Dose Administered");
					pr_write2(strng, "Thyroid Uptake Protocol");
					break;

				case 2:
					insertconst(strng, 0, 0, 0, "Dose Measurement: Measure Each");
					pr_write2(strng, "Thyroid Uptake Protocol");
					insertconst(strng, 0, 0, 0, "Dose and Add Activity");
					pr_write2(strng, "Thyroid Uptake Protocol");
					break;

				case 3:
					insertconst(strng, 0, 0, 0, "Dose Measurement: Measure One");
					pr_write2(strng, "Thyroid Uptake Protocol");
					insertconst(strng, 0, 0, 0, "Capsule and Multiply by Number");
					pr_write2(strng, "Thyroid Uptake Protocol");
					insertconst(strng, 0, 0, 0, "Administered");
					pr_write2(strng, "Thyroid Uptake Protocol");
					break;

				case 4:
					insertconst(strng, 0, 0, 0, "Dose Measurement: Measure Liquid");
					pr_write2(strng, "Thyroid Uptake Protocol");
					insertconst(strng, 0, 0, 0, "and Multiply by Factor");
					pr_write2(strng, "Thyroid Uptake Protocol");
					break;
			}
			pr_write2(strng, "Thyroid Uptake Protocol");

			// Nuclide
			switch(protocol.NuclideID){
				case 40:
					insertconst(strng, 0, 0, 0, "Nuclide: I123");
					break;
				case 45:
					insertconst(strng, 0, 0, 0, "Nuclide: I131");
					break;
				case 84:
					insertconst(strng, 0, 0, 0, "Nuclide: Tc99m");
					break;
			}
			pr_write2(strng, "Thyroid Uptake Protocol");

			// Dose Form
			switch(protocol.DoseForm){
				case 1:
					insertconst(strng, 0, 0, 0, "Dose Form: Capsule");
					break;

				case 2:
					insertconst(strng, 0, 0, 0, "Dose Form: Liquid");
					break;
			}
			pr_write2(strng, "Thyroid Uptake Protocol");

			// PreDose
			if(protocol.PreDoseMeasurement) insertconst(strng, 0, 0, 0, "Pre-Dose Measurement: Yes");
			else  insertconst(strng, 0, 0, 0, "Pre-Dose Measurement: No");
			pr_write2(strng, "Thyroid Uptake Protocol");

			// Residual
			if(protocol.ResidualMeasurement) insertconst(strng, 0, 0, 0, "Measure Residual: Yes");
			else insertconst(strng, 0, 0, 0, "Measure Residual: No");
			pr_write2(strng, "Thyroid Uptake Protocol");

			// Counting Time
			insertint(strng, 0, 0, 0, "Count Time(sec): %d", protocol.DefaultCountTime);
			pr_write2(strng, "Thyroid Uptake Protocol");

			// Probe Distance
			insertint(strng, 0, 0, 0, "Probe Distance(cm): %d", protocol.DefaultDistance);
			pr_write2(strng, "Thyroid Uptake Protocol");

			// I123 ROI
			sprintf(buff, " I123 ROI(keV): %.1f - %.1f", protocol.I123LowerEV, protocol.I123UpperEV);
			insertconst(strng, 0, 0, 0, buff);
			pr_write2(strng, "Thyroid Uptake Protocol");

			// I131 ROI
			sprintf(buff, " I131 ROI(keV): %.1f - %.1f", protocol.I131LowerEV, protocol.I131UpperEV);
			insertconst(strng, 0, 0, 0, buff);
			pr_write2(strng, "Thyroid Uptake Protocol");

			// Tc99m ROI
			sprintf(buff, "Tc99m ROI(keV): %.1f - %.1f", protocol.Tc99mLowerEV, protocol.Tc99mUpperEV);
			insertconst(strng, 0, 0, 0, buff);
			pr_write2(strng, "Thyroid Uptake Protocol");

			formfeed(prtype);
		}else{
			Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
			return;
		}
	}
}

void AmuletWellSetupThyroidUptakeProtocolMenu_printAll(void){
	char prtype;
	char strng[90];
	char buff[90];
	int index, protocol_count;
	PROBETHYROIDUPTAKEPROTOCOLACTIVENAME *ThyroidUptakeProtocolActiveName;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		protocol_count = DB_ActiveProbeThyroidUptakeProtocolCount();

		if(protocol_count > 0){
			if(prtype == ROLL_PRINTER) delayloop(1000);
			ThyroidUptakeProtocolActiveName = malloc(protocol_count * sizeof(PROBETHYROIDUPTAKEPROTOCOLACTIVENAME));
			DB_RetrieveActiveProbeThyroidUptakeProtocolNames(ThyroidUptakeProtocolActiveName);

			rawheader(prtype, "Thyroid Uptake Protocol",  clock_time);
			pr_set_linecnt(5);
			lininit(strng, TRUE, prtype);
			for(index=0; index<protocol_count; index++){
				protocol.ProbeTUProtocolID = ThyroidUptakeProtocolActiveName[index].ProbeTUProtocolID;
				DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
				if(protocol.ProbeTUProtocolID <= 0){
					Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
					return;
				}

				// Thyroid Uptake Protocol Name
				insertstring(strng, 0, 0, 0, "Name: %s", protocol.ProtocolName);
				pr_write2(strng, "Thyroid Uptake Protocol");
				pr_write2(strng, "Thyroid Uptake Protocol");

				// Counting Method - Dose Decay or Reference
				if(protocol.DoseDecayCorrect){
					insertconst(strng, 0, 0, 0, "Counting Method: Decay Correct");
					pr_write2(strng, "Thyroid Uptake Protocol");
					insertconst(strng, 0, 0, 0, "Administered Dose");
					pr_write2(strng, "Thyroid Uptake Protocol");
				}else{
					insertconst(strng, 0, 0, 0, "Counting Method: Measure Same");
					pr_write2(strng, "Thyroid Uptake Protocol");
					insertconst(strng, 0, 0, 0, "Reference Dose Before Each");
					pr_write2(strng, "Thyroid Uptake Protocol");
					insertconst(strng, 0, 0, 0, "Uptake");
					pr_write2(strng, "Thyroid Uptake Protocol");
				}
				pr_write2(strng, "Thyroid Uptake Protocol");

				// Dose Measurement
				switch(protocol.DoseMeasurementMethod){
					case 1:
						insertconst(strng, 0, 0, 0, "Dose Measurement: Dose Measured");
						pr_write2(strng, "Thyroid Uptake Protocol");
						insertconst(strng, 0, 0, 0, "is Dose Administered");
						pr_write2(strng, "Thyroid Uptake Protocol");
						break;

					case 2:
						insertconst(strng, 0, 0, 0, "Dose Measurement: Measure Each");
						pr_write2(strng, "Thyroid Uptake Protocol");
						insertconst(strng, 0, 0, 0, "Dose and Add Activity");
						pr_write2(strng, "Thyroid Uptake Protocol");
						break;

					case 3:
						insertconst(strng, 0, 0, 0, "Dose Measurement: Measure One");
						pr_write2(strng, "Thyroid Uptake Protocol");
						insertconst(strng, 0, 0, 0, "Capsule and Multiply by Number");
						pr_write2(strng, "Thyroid Uptake Protocol");
						insertconst(strng, 0, 0, 0, "Administered");
						pr_write2(strng, "Thyroid Uptake Protocol");
						break;

					case 4:
						insertconst(strng, 0, 0, 0, "Dose Measurement: Measure Liquid");
						pr_write2(strng, "Thyroid Uptake Protocol");
						insertconst(strng, 0, 0, 0, "and Multiply by Factor");
						pr_write2(strng, "Thyroid Uptake Protocol");
						break;
				}
				pr_write2(strng, "Thyroid Uptake Protocol");

				// Nuclide
				switch(protocol.NuclideID){
					case 40:
						insertconst(strng, 0, 0, 0, "Nuclide: I123");
						break;
					case 45:
						insertconst(strng, 0, 0, 0, "Nuclide: I131");
						break;
					case 84:
						insertconst(strng, 0, 0, 0, "Nuclide: Tc99m");
						break;
				}
				pr_write2(strng, "Thyroid Uptake Protocol");

				// Dose Form
				switch(protocol.DoseForm){
					case 1:
						insertconst(strng, 0, 0, 0, "Dose Form: Capsule");
						break;

					case 2:
						insertconst(strng, 0, 0, 0, "Dose Form: Liquid");
						break;
				}
				pr_write2(strng, "Thyroid Uptake Protocol");

				// PreDose
				if(protocol.PreDoseMeasurement) insertconst(strng, 0, 0, 0, "Pre-Dose Measurement: Yes");
				else  insertconst(strng, 0, 0, 0, "Pre-Dose Measurement: No");
				pr_write2(strng, "Thyroid Uptake Protocol");

				// Residual
				if(protocol.ResidualMeasurement) insertconst(strng, 0, 0, 0, "Measure Residual: Yes");
				else insertconst(strng, 0, 0, 0, "Measure Residual: No");
				pr_write2(strng, "Thyroid Uptake Protocol");

				// Counting Time
				insertint(strng, 0, 0, 0, "Count Time(sec): %d", protocol.DefaultCountTime);
				pr_write2(strng, "Thyroid Uptake Protocol");

				// Probe Distance
				insertint(strng, 0, 0, 0, "Probe Distance(cm): %d", protocol.DefaultDistance);
				pr_write2(strng, "Thyroid Uptake Protocol");

				// I123 ROI
				sprintf(buff, " I123 ROI(keV): %.1f - %.1f", protocol.I123LowerEV, protocol.I123UpperEV);
				insertconst(strng, 0, 0, 0, buff);
				pr_write2(strng, "Thyroid Uptake Protocol");

				// I131 ROI
				sprintf(buff, " I131 ROI(keV): %.1f - %.1f", protocol.I131LowerEV, protocol.I131UpperEV);
				insertconst(strng, 0, 0, 0, buff);
				pr_write2(strng, "Thyroid Uptake Protocol");

				// Tc99m ROI
				sprintf(buff, "Tc99m ROI(keV): %.1f - %.1f", protocol.Tc99mLowerEV, protocol.Tc99mUpperEV);
				insertconst(strng, 0, 0, 0, buff);
				pr_write2(strng, "Thyroid Uptake Protocol");

				insertconst(strng, 0, 0, 0, "==================================");
				pr_write2(strng, "Thyroid Uptake Protocol");
			}
			formfeed(prtype);
			free(ThyroidUptakeProtocolActiveName);
		}
	}
}

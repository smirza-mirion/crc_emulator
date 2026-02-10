#define PHASE_WELLCHITEST_PRE_INIT		0
#define PHASE_WELLCHITEST_WAIT			1
#define PHASE_WELLCHITEST_COUNTITME		2
#define PHASE_WELLCHITEST_REPS			3
#define PHASE_WELLCHITEST_MEASURE		4
#define PHASE_WELLCHITEST_PRINT			5
#define PHASE_WELLCHITEST_SPECTRUM1		6
#define PHASE_WELLCHITEST_SPECTRUM2		7
#define PHASE_WELLCHITEST_SPECTRUM3		8
#define PHASE_WELLCHITEST_SPECTRUM4		9
#define PHASE_WELLCHITEST_SPECTRUM5		10
#define PHASE_WELLCHITEST_SPECTRUM6		11
#define PHASE_WELLCHITEST_SPECTRUM7		12
#define PHASE_WELLCHITEST_SPECTRUM8		13
#define PHASE_WELLCHITEST_SPECTRUM9		14
#define PHASE_WELLCHITEST_SPECTRUM10	15
#define PHASE_WELLCHITEST_SPECTRUM11	16
#define PHASE_WELLCHITEST_SPECTRUM12	17
#define PHASE_WELLCHITEST_SPECTRUM13	18
#define PHASE_WELLCHITEST_SPECTRUM14	19
#define PHASE_WELLCHITEST_SPECTRUM15	20
#define PHASE_WELLCHITEST_SPECTRUM16	21
#define PHASE_WELLCHITEST_SPECTRUM17	22
#define PHASE_WELLCHITEST_SPECTRUM18	23
#define PHASE_WELLCHITEST_SPECTRUM19	24
#define PHASE_WELLCHITEST_SPECTRUM20	25

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "printer.h"
#include "nuc.h"
#include "database.h"
#include "i2c.h"

bool AmuletWellChiTestMenu_finished;
WELLCHITEST AmuletWellChiTestMenu_test;

extern int m_iPhase;
extern unsigned char m_ucClear;
extern volatile char m_acTitle[51];
extern unsigned char m_ucKeyPadConfig;
extern unsigned char m_ucLenLimit;
extern unsigned char m_ucLenFractionLimit;
extern float m_fMaxValue;
extern float m_fMinValue;
extern unsigned short int m_uiSetKeyPad;
extern long AmuletWellMeasurementMenu_reps;
extern unsigned char m_ucSetWellMeasure;
extern CURRENT current;
extern bool m_flgShowPlusMinus;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;
extern bool AmuletWellMeasurementMenu_unlockDetector;

void PushPageStack(unsigned char ucPage);
void AmuletWellChiTest_print(void);

void AmuletWellChiTest_menu(void){
    short index;
#ifdef TIMES1000 
    short jndex;
#endif
	char message[51];
	double sum, squared, squaredsum, num;

	switch(m_iPhase){
		case PHASE_WELLCHITEST_PRE_INIT:
			if(m_ucClear == 46){
				AmuletWellChiTestMenu_finished = FALSE;
				AmuletWellChiTestMenu_test.WellChiSquareTestID = 0;
				AmuletWellChiTestMenu_test.SampleTime = 60;
				AmuletWellChiTestMenu_test.NumberOfSamples = 5;
				strcpy(AmuletWellChiTestMenu_test.NuclideName, "Cs137");
				AmuletWellChiTestMenu_test.Energy = NuclideData_getEffectivePrimary(NuclideData_getIndexFromName("Cs137"));
				if(AmuletWellChiTestMenu_test.DetectorTest == DET_DRILLEDPROBE700_AS_PROBE) Mca_getEndPoints(AmuletWellChiTestMenu_test.Energy, &(AmuletWellChiTestMenu_test.StartROI), &(AmuletWellChiTestMenu_test.EndROI), TRUE);
				else Mca_getEndPoints(AmuletWellChiTestMenu_test.Energy, &(AmuletWellChiTestMenu_test.StartROI), &(AmuletWellChiTestMenu_test.EndROI), FALSE);
				AmuletWellChiTestMenu_test.ChiSquare = 0;
				AmuletWellChiTestMenu_test.CreatedOn = 0;
				AmuletWellChiTestMenu_test.InactiveReason[0] = 0;
				AmuletWellChiTestMenu_test.Inactive = FALSE;
				for(index=0; index<20; index++){
					AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.SpectraID = 0;
					AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.MeasuredOn = 0;
				}
				m_ucClear = 0;
			}

			switch(AmuletWellChiTestMenu_test.DetectorTest){
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

				case DET_DRILLEDPROBE700_AS_WELL:
					strcpy(message, "Drilled Probe As Well");
					break;

				case DET_DRILLEDPROBE700_AS_PROBE:
					strcpy(message, "Drilled Probe As Probe");
					break;
			}
			SetAmuletString(9, message);

			sprintf(message, "%d", AmuletWellChiTestMenu_test.SampleTime);
			SetAmuletString(101, message);

			sprintf(message, "%d", AmuletWellChiTestMenu_test.NumberOfSamples);
			SetAmuletString(102, message);

			SetAmuletByte(100, 0xFF);

			if(AmuletWellChiTestMenu_finished){
				sum = 0;
				squaredsum = 0;
				// Calculate Chi-Square
				for(index=0; index<AmuletWellChiTestMenu_test.NumberOfSamples; index++){
					sprintf(message, "%d) %ld", index+1, AmuletWellChiTestMenu_test.WellChiSquareSample[index].ROICounts);
					SetAmuletString(103+index, message);
					sum +=  AmuletWellChiTestMenu_test.WellChiSquareSample[index].ROICounts;
					squared = AmuletWellChiTestMenu_test.WellChiSquareSample[index].ROICounts;
					squared = squared * squared;
					squaredsum += squared;
					SetAmuletByte(103 + index, 0xFF);
					delayloop(20);
				}
				delayloop(50);
				num = AmuletWellChiTestMenu_test.NumberOfSamples;
				AmuletWellChiTestMenu_test.ChiSquare = (num * squaredsum / sum) - sum;
				sprintf(message, "Chi-Square = %.1f", AmuletWellChiTestMenu_test.ChiSquare);
				SetAmuletString(123, message);
				SetAmuletByte(101, 0xFF);

				if(AmuletWellChiTestMenu_test.WellChiSquareTestID == 0){
#ifdef TIMES1000
					for(index=0; index<TIMES1000; index++){
#endif
						DB_CreateChiTest(&AmuletWellChiTestMenu_test, TRUE);
#ifdef TIMES1000
						clock_time -= 86400;
						for(jndex=0; jndex<AmuletWellChiTestMenu_test.NumberOfSamples; jndex++){
							AmuletWellChiTestMenu_test.WellChiSquareSample[jndex].Spectrum.MeasuredOn = clock_time;
						}

					}
					read_clock(&clock_time);
#endif
				}

				//if(current.printer != NONE_PRINTER) SetAmuletByte(102, 0xFF);
				if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
					SetAmuletByte(102, 0xFF);
			}
			m_iPhase = PHASE_WELLCHITEST_WAIT;
			break;

		case PHASE_WELLCHITEST_WAIT:
			break;

		case PHASE_WELLCHITEST_COUNTITME:
			AmuletWellChiTestMenu_finished = FALSE;
			AmuletWellChiTestMenu_test.WellChiSquareTestID = 0;
			AmuletWellChiTestMenu_test.CreatedOn = 0;
			for(index=0; index<20; index++){
				AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.SpectraID = 0;
				AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.MeasuredOn = 0;
			}
			strcpy((char *)m_acTitle, "Please Enter Counting Time (sec):");
			m_ucLenLimit = 4;
			m_ucLenFractionLimit = 0;
			m_fMinValue = 2.0;
			m_fMaxValue = 30000.0;
			m_ucKeyPadConfig = 0;
			m_uiSetKeyPad = 92;
			m_flgShowPlusMinus = FALSE;
			SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
			PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
			beep_amulet();
			return;

		case PHASE_WELLCHITEST_REPS:
			AmuletWellChiTestMenu_finished = FALSE;
			AmuletWellChiTestMenu_test.WellChiSquareTestID = 0;
			AmuletWellChiTestMenu_test.CreatedOn = 0;
			for(index=0; index<20; index++){
				AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.SpectraID = 0;
				AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.MeasuredOn = 0;
			}
			strcpy((char *)m_acTitle, "Please Enter Number of Samples:");
			m_ucLenLimit = 2;
			m_ucLenFractionLimit = 0;
			m_fMinValue = 5.0;
			m_fMaxValue = 20.0;
			m_ucKeyPadConfig = 0;
			m_uiSetKeyPad = 93;
			m_flgShowPlusMinus = FALSE;
			SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
			PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
			beep_amulet();
			return;

		case PHASE_WELLCHITEST_MEASURE:
			beep_amulet();
			AmuletWellChiTestMenu_finished = FALSE;
			AmuletWellChiTestMenu_test.WellChiSquareTestID = 0;
			AmuletWellChiTestMenu_test.CreatedOn = 0;
			for(index=0; index<20; index++){
				AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.SpectraID = 0;
				AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.MeasuredOn = 0;
			}
			AmuletWellMeasurementMenu_reps = AmuletWellChiTestMenu_test.NumberOfSamples;
			m_ucClear = 18;
			m_ucSetWellMeasure = 18;
			AmuletWellMeasurementMenu_unlockDetector = FALSE;
			sprintf((char *)m_acTitle, "Chi-Square Test");
			SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
			return;

		case PHASE_WELLCHITEST_PRINT:
			beep_amulet();
			AmuletWellChiTest_print();
			SetAmuletByte(123, 0xFF);
			m_iPhase = PHASE_WELLCHITEST_WAIT;
			break;

		case PHASE_WELLCHITEST_SPECTRUM1:
		case PHASE_WELLCHITEST_SPECTRUM2:
		case PHASE_WELLCHITEST_SPECTRUM3:
		case PHASE_WELLCHITEST_SPECTRUM4:
		case PHASE_WELLCHITEST_SPECTRUM5:
		case PHASE_WELLCHITEST_SPECTRUM6:
		case PHASE_WELLCHITEST_SPECTRUM7:
		case PHASE_WELLCHITEST_SPECTRUM8:
		case PHASE_WELLCHITEST_SPECTRUM9:
		case PHASE_WELLCHITEST_SPECTRUM10:
		case PHASE_WELLCHITEST_SPECTRUM11:
		case PHASE_WELLCHITEST_SPECTRUM12:
		case PHASE_WELLCHITEST_SPECTRUM13:
		case PHASE_WELLCHITEST_SPECTRUM14:
		case PHASE_WELLCHITEST_SPECTRUM15:
		case PHASE_WELLCHITEST_SPECTRUM16:
		case PHASE_WELLCHITEST_SPECTRUM17:
		case PHASE_WELLCHITEST_SPECTRUM18:
		case PHASE_WELLCHITEST_SPECTRUM19:
		case PHASE_WELLCHITEST_SPECTRUM20:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellChiTestMenu_test.WellChiSquareSample[m_iPhase - PHASE_WELLCHITEST_SPECTRUM1].Spectrum), sizeof(DB_SPEC));
			sprintf(message, "Chi-Square Spectrum #%d", m_iPhase - PHASE_WELLCHITEST_SPECTRUM1 + 1);
			strcpy(AmuletWellSpectrumMenu_comment1, message);
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;
	}
}

void AmuletWellChiTest_print(void){
	char prtype;
	char strng[90], detectortype[30];
	char buf[40];
	short nc, index;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "CHI-SQUARE TEST", AmuletWellChiTestMenu_test.CreatedOn);

		switch(AmuletWellChiTestMenu_test.DetectorTest){
			case DET_WELL:
				strcpy(detectortype, "Det: WELL");
				break;

			case DET_WELL700:
				strcpy(detectortype, "Det: Well");
				break;

			case DET_BETA:
				strcpy(detectortype, "Det: Beta");
				break;

			case DET_PROBE700:
				strcpy(detectortype, "Det: Probe");
				break;

			case DET_DRILLEDPROBE700:
				strcpy(detectortype, "Det: Drilled Probe");
				break;

			case DET_DRILLEDPROBE700_AS_WELL:
				strcpy(detectortype, "Det: Drilled Probe As Well");
				break;

			case DET_DRILLEDPROBE700_AS_PROBE:
				strcpy(detectortype, "Det: Drilled Probe As Probe");
				break;
		}
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%s", detectortype);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Count Time: %d sec", AmuletWellChiTestMenu_test.SampleTime);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Test", 4);
		strncpy(&strng[26], "Counts", 6);
		pr_write(strng);

		for(index=0; index<AmuletWellChiTestMenu_test.NumberOfSamples; index++){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "%d", index+1);
			strncpy(&strng[2], buf, nc);
			nc = sprintf(buf, "%ld", AmuletWellChiTestMenu_test.WellChiSquareSample[index].ROICounts);
			strncpy(&strng[32 - nc], buf, nc);
			pr_write(strng);
		}

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Chi-Square = %.1f", AmuletWellChiTestMenu_test.ChiSquare);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		formfeed(prtype);
	}
}

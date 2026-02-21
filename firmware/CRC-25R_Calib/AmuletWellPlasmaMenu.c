#define PHASE_WELLPLASMA_PRE_INIT	0
#define PHASE_WELLPLASMA_WAIT		1
#define PHASE_WELLPLASMA_CALC		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "amulet.h"
#include "nuc.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern time_t clock_time;

WELLPLASMATEST AmuletWellPlasmaMenu_test;
bool AmuletWellPlasmaMenu_backgroundMeasured;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);

void AmuletWellPlasma_menu(void){
	char message[51], message2[51];
	float backgroundcps, standardcps, wholecps, plasmacps, netstd, netwhole, netplasma;
	time_t tstamp;
	short calcStart, calcEnd;
	int index;

	switch(m_iPhase){
		case PHASE_WELLPLASMA_PRE_INIT:
			if(m_ucClear == 42){
#ifdef DEMO_DATA
				if(DEMO_DATA){
					AmuletWellPlasmaMenu_test.DilutionFactor = 2000;
					AmuletWellPlasmaMenu_test.SampleVolume = 20;
					AmuletWellPlasmaMenu_test.Hematocrit = 40;
					AmuletWellPlasmaMenu_test.PatientWeight = 140;
					AmuletWellPlasmaMenu_test.StandardROICPM = 2400;
					AmuletWellPlasmaMenu_test.WholeBloodROICPM = 3600;
					AmuletWellPlasmaMenu_test.PlasmaROICPM = 2600;
					AmuletWellPlasmaMenu_backgroundMeasured = TRUE;
				}
#else
				AmuletWellPlasmaMenu_test.DilutionFactor = -1;
				AmuletWellPlasmaMenu_test.SampleVolume = -1;
				AmuletWellPlasmaMenu_test.Hematocrit = -1;
				AmuletWellPlasmaMenu_test.PatientWeight = -1;
				AmuletWellPlasmaMenu_test.StartROI = 15.0;
				AmuletWellPlasmaMenu_test.EndROI = 80.0;
				AmuletWellPlasmaMenu_test.StandardROICounts = 0;
				AmuletWellPlasmaMenu_test.StandardROICPM = -1;
				AmuletWellPlasmaMenu_test.WholeBloodROICounts = 0;
				AmuletWellPlasmaMenu_test.WholeBloodROICPM = -1;
				AmuletWellPlasmaMenu_test.PlasmaROICounts = 0;
				AmuletWellPlasmaMenu_test.PlasmaROICPM = -1;
				AmuletWellPlasmaMenu_test.BackgroundROICounts = 0;
				AmuletWellPlasmaMenu_test.BackgroundROICPM = -1;
				AmuletWellPlasmaMenu_test.WholeBloodVolume = 0;
				AmuletWellPlasmaMenu_test.WholeBloodPerKg = 0;
				AmuletWellPlasmaMenu_test.PlasmaVolume = 0;
				AmuletWellPlasmaMenu_test.PlasmaPerKg = 0;
				AmuletWellPlasmaMenu_test.RBCVolume = 0;
				AmuletWellPlasmaMenu_test.RBCPerKg = 0;
				AmuletWellPlasmaMenu_test.CalculatedHematocrit = 0;
				AmuletWellPlasmaMenu_test.Standard_Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellPlasmaMenu_test.Standard_Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellPlasmaMenu_test.WholeBlood_Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellPlasmaMenu_test.WholeBlood_Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellPlasmaMenu_test.Plasma_Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellPlasmaMenu_test.Plasma_Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellPlasmaMenu_test.Background.WellBackgroundID  = 0;
				AmuletWellPlasmaMenu_test.Background.Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellPlasmaMenu_test.Background.Spectrum.DecompressedSpectra[index] = 0;
				//AmuletWellPlasmaMenu_test.BackgroundTime = 0;
				AmuletWellPlasmaMenu_test.TestIdent.TestIdentID = 0;
				AmuletWellPlasmaMenu_test.TestIdent.TestIdentGroupID = 0;
				AmuletWellPlasmaMenu_test.TestIdent.TestID[0] = 0;
				AmuletWellPlasmaMenu_test.TestIdent.PatientID[0] = 0;
				AmuletWellPlasmaMenu_test.TestIdent.FirstName[0] = 0;
				AmuletWellPlasmaMenu_test.TestIdent.LastName[0] = 0;
				AmuletWellPlasmaMenu_test.TestIdent.DateOfBirth = 0.0;
				AmuletWellPlasmaMenu_test.TestIdent.Sex[0] = 0;
				AmuletWellPlasmaMenu_test.TestIdent.Physician[0] = 0;
				AmuletWellPlasmaMenu_test.TestIdent.TechID[0] = 0;
				AmuletWellPlasmaMenu_test.TestIdent.CreatedOn = 0;
				AmuletWellPlasmaMenu_test.TestIdent.LastUpdated = 0;
				AmuletWellPlasmaMenu_test.TestIdent.Inactive = FALSE;
				AmuletWellPlasmaMenu_test.CreatedOn = 0;
				AmuletWellPlasmaMenu_test.InactiveReason[0] = 0;
				AmuletWellPlasmaMenu_test.Inactive = FALSE;
				AmuletWellPlasmaMenu_backgroundMeasured = FALSE;
#endif
				m_ucClear = 0;
			}
			Mca_switchToWell();
			Mca_sendDetectorType();

			if(AmuletWellPlasmaMenu_backgroundMeasured){
				AmuletWellPlasmaMenu_test.Background.WellBackgroundID = ((BACKGND *)Mca_getBackgroundMirror())->WellBackgroundID;
				DB_RetrieveBackgnd(&(AmuletWellPlasmaMenu_test.Background));
				tstamp = Mca_getBackgroundStamp();
				GetExtendedTimeInfo(&tstamp, message2);
				strcpy(message, "Background measured ");
				strcat(message, message2);
				strcat(message, " ");
				SetAmuletString(100, message);
				Mca_getROIChannels(AmuletWellPlasmaMenu_test.StartROI, AmuletWellPlasmaMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellPlasmaMenu_test.Background.Spectrum.AutoCal);
				AmuletWellPlasmaMenu_test.BackgroundROICounts = Mca_getROIBackgroundCounts(calcStart, calcEnd, &AmuletWellPlasmaMenu_test.Background);
				AmuletWellPlasmaMenu_test.BackgroundROICPM = Mca_getROIBackgroundCPM(calcStart, calcEnd, &AmuletWellPlasmaMenu_test.Background);
				backgroundcps = AmuletWellPlasmaMenu_test.BackgroundROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaMenu_test.BackgroundROICPM);
				else sprintf(message, "%.0f cps", backgroundcps);
				SetAmuletString(102, message);
			}else{
				AmuletWellPlasmaMenu_test.BackgroundROICPM = -1;
				SetAmuletString(100, "Measured Background:");
			}

			if(AmuletWellPlasmaMenu_test.DilutionFactor != -1){
				sprintf(message, "%.1f", AmuletWellPlasmaMenu_test.DilutionFactor);
				SetAmuletString(103, message);
			}

			if(AmuletWellPlasmaMenu_test.SampleVolume != -1){
				sprintf(message, "%.1f", AmuletWellPlasmaMenu_test.SampleVolume);
				SetAmuletString(104, message);
			}

			if(AmuletWellPlasmaMenu_test.Hematocrit != -1){
				sprintf(message, "%.1f", AmuletWellPlasmaMenu_test.Hematocrit);
				SetAmuletString(105, message);
			}

			if(AmuletWellPlasmaMenu_test.PatientWeight != -1){
				sprintf(message, "%.1f", AmuletWellPlasmaMenu_test.PatientWeight);
				SetAmuletString(106, message);
			}

			if(AmuletWellPlasmaMenu_test.StandardROICPM != -1){
				standardcps = AmuletWellPlasmaMenu_test.StandardROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaMenu_test.StandardROICPM);
				else sprintf(message, "%.0f cps", standardcps);
				SetAmuletString(107, message);
			}

			if(AmuletWellPlasmaMenu_test.WholeBloodROICPM != -1){
				wholecps = AmuletWellPlasmaMenu_test.WholeBloodROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaMenu_test.WholeBloodROICPM);
				else sprintf(message, "%.0f cps", wholecps);
				SetAmuletString(108, message);
			}

			if(AmuletWellPlasmaMenu_test.PlasmaROICPM != -1){
				plasmacps = AmuletWellPlasmaMenu_test.PlasmaROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellPlasmaMenu_test.PlasmaROICPM);
				else sprintf(message, "%.0f cps", plasmacps);
				SetAmuletString(109, message);
			}

			SetAmuletByte(100, 0xFF);

			if((AmuletWellPlasmaMenu_test.BackgroundROICPM != -1) && (AmuletWellPlasmaMenu_test.DilutionFactor != -1) && (AmuletWellPlasmaMenu_test.SampleVolume != -1) && (AmuletWellPlasmaMenu_test.Hematocrit != -1) && (AmuletWellPlasmaMenu_test.PatientWeight != -1) && (AmuletWellPlasmaMenu_test.StandardROICPM != -1) && (AmuletWellPlasmaMenu_test.WholeBloodROICPM != -1) && (AmuletWellPlasmaMenu_test.PlasmaROICPM != -1)){
				SetAmuletByte(101, 0xFF);
			}
			m_iPhase = PHASE_WELLPLASMA_WAIT;
			break;

		case PHASE_WELLPLASMA_WAIT:
			break;

		case PHASE_WELLPLASMA_CALC:
			beep_amulet();

			if(AmuletWellPlasmaMenu_test.BackgroundROICPM >= AmuletWellPlasmaMenu_test.StandardROICPM){
				Amulet_DisplayError("Plasma Test Error", "Background Counts can not be greater than or equal to the Standard Counts",TRUE);
				return;
			}

			if(AmuletWellPlasmaMenu_test.BackgroundROICPM >= AmuletWellPlasmaMenu_test.WholeBloodROICPM){
				Amulet_DisplayError("Plasma Test Error", "Background Counts can not be greater than or equal to the Whole Blood Counts",TRUE);
				return;
			}

			if(AmuletWellPlasmaMenu_test.BackgroundROICPM >= AmuletWellPlasmaMenu_test.PlasmaROICPM){
				Amulet_DisplayError("Plasma Test Error", "Background Counts can not be greater than or equal to the Plasma Counts",TRUE);
				return;
			}

			netstd = AmuletWellPlasmaMenu_test.StandardROICPM - AmuletWellPlasmaMenu_test.BackgroundROICPM;
			netwhole = AmuletWellPlasmaMenu_test.WholeBloodROICPM - AmuletWellPlasmaMenu_test.BackgroundROICPM;
			netplasma = AmuletWellPlasmaMenu_test.PlasmaROICPM - AmuletWellPlasmaMenu_test.BackgroundROICPM;

			AmuletWellPlasmaMenu_test.WholeBloodVolume = AmuletWellPlasmaMenu_test.SampleVolume * netstd * AmuletWellPlasmaMenu_test.DilutionFactor / netwhole;
			AmuletWellPlasmaMenu_test.PlasmaVolume = AmuletWellPlasmaMenu_test.SampleVolume * netstd * AmuletWellPlasmaMenu_test.DilutionFactor / netplasma;
			AmuletWellPlasmaMenu_test.RBCVolume = AmuletWellPlasmaMenu_test.WholeBloodVolume - AmuletWellPlasmaMenu_test.PlasmaVolume;

			AmuletWellPlasmaMenu_test.WholeBloodPerKg = AmuletWellPlasmaMenu_test.WholeBloodVolume / AmuletWellPlasmaMenu_test.PatientWeight;
			AmuletWellPlasmaMenu_test.PlasmaPerKg = AmuletWellPlasmaMenu_test.PlasmaVolume / AmuletWellPlasmaMenu_test.PatientWeight;
			AmuletWellPlasmaMenu_test.RBCPerKg = AmuletWellPlasmaMenu_test.RBCVolume / AmuletWellPlasmaMenu_test.PatientWeight;

			AmuletWellPlasmaMenu_test.CalculatedHematocrit = 100.0 * AmuletWellPlasmaMenu_test.RBCVolume / AmuletWellPlasmaMenu_test.WholeBloodVolume;

			AmuletWellPlasmaMenu_test.CreatedOn = clock_time;
			SetAmuletHTML(AmuletHTMLIndex[WELLPLASMAANALYSIS_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLPLASMAANALYSIS_HTM]);
			return;
			//break;
	}
}

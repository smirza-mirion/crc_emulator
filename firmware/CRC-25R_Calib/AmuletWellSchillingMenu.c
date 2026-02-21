#define PHASE_WELLSCHILLING_PRE_INIT	0
#define PHASE_WELLSCHILLING_WAIT		1
#define PHASE_WELLSCHILLING_CALC		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "amulet.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern time_t clock_time;

WELLSCHILLINGTEST AmuletWellSchillingMenu_test;
bool AmuletWellSchillingMenu_backgroundMeasured;


char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);

void AmuletWellSchilling_menu(void){
	char message[51], message2[51];
	float backgroundcps, standardcps, aliqotcps;
	time_t tstamp;
	short calcStart, calcEnd;
	int index;
	float aliquotnet, standnet, num, denom;

	switch(m_iPhase){
		case PHASE_WELLSCHILLING_PRE_INIT:
			if(m_ucClear == 41){
#ifdef DEMO_DATA
				if(DEMO_DATA){
					AmuletWellSchillingMenu_test.UrineVolume = 2000;
					AmuletWellSchillingMenu_test.AliquotVolume = 20;
					AmuletWellSchillingMenu_test.DilutionFactor = 1000;
					AmuletWellSchillingMenu_test.StandardROICPM = 3000;
					AmuletWellSchillingMenu_test.AliquotROICPM = 4000;
					AmuletWellSchillingMenu_backgroundMeasured = TRUE;
				}
#else
				AmuletWellSchillingMenu_test.UrineVolume = -1;
				AmuletWellSchillingMenu_test.AliquotVolume = -1;
				AmuletWellSchillingMenu_test.DilutionFactor = -1;
				AmuletWellSchillingMenu_test.StartROI = 50.0;
				AmuletWellSchillingMenu_test.EndROI = 200.0;
				AmuletWellSchillingMenu_test.StandardROICounts = 0;
				AmuletWellSchillingMenu_test.StandardROICPM = -1;
				AmuletWellSchillingMenu_test.AliquotROICounts = 0;
				AmuletWellSchillingMenu_test.AliquotROICPM = -1;
				AmuletWellSchillingMenu_test.BackgroundROICounts = 0;
				AmuletWellSchillingMenu_test.BackgroundROICPM = -1;
				AmuletWellSchillingMenu_test.Excretion = 0;
				AmuletWellSchillingMenu_test.SchillingStandard_Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellSchillingMenu_test.SchillingStandard_Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellSchillingMenu_test.SchillingAliquot_Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellSchillingMenu_test.SchillingAliquot_Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellSchillingMenu_test.Background.WellBackgroundID = 0;
				AmuletWellSchillingMenu_test.Background.Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellSchillingMenu_test.Background.Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellSchillingMenu_test.TestIdent.TestIdentID = 0;
				AmuletWellSchillingMenu_test.TestIdent.TestIdentGroupID = 0;
				AmuletWellSchillingMenu_test.TestIdent.TestID[0] = 0;
				AmuletWellSchillingMenu_test.TestIdent.PatientID[0] = 0;
				AmuletWellSchillingMenu_test.TestIdent.FirstName[0] = 0;
				AmuletWellSchillingMenu_test.TestIdent.LastName[0] = 0;
				AmuletWellSchillingMenu_test.TestIdent.DateOfBirth = 0.0;
				AmuletWellSchillingMenu_test.TestIdent.Sex[0] = 0;
				AmuletWellSchillingMenu_test.TestIdent.Physician[0] = 0;
				AmuletWellSchillingMenu_test.TestIdent.TechID[0] = 0;
				AmuletWellSchillingMenu_test.TestIdent.CreatedOn = 0;
				AmuletWellSchillingMenu_test.TestIdent.LastUpdated = 0;
				AmuletWellSchillingMenu_test.TestIdent.Inactive = FALSE;
				AmuletWellSchillingMenu_test.CreatedOn = 0;
				AmuletWellSchillingMenu_test.InactiveReason[0] = 0;
				AmuletWellSchillingMenu_test.Inactive = FALSE;
				AmuletWellSchillingMenu_backgroundMeasured = FALSE;
#endif
				m_ucClear = 0;
			}
			Mca_switchToWell();
			Mca_sendDetectorType();

			if(AmuletWellSchillingMenu_backgroundMeasured){
				AmuletWellSchillingMenu_test.Background.WellBackgroundID = ((BACKGND *)Mca_getBackgroundMirror())->WellBackgroundID;
				DB_RetrieveBackgnd(&(AmuletWellSchillingMenu_test.Background));
				tstamp = Mca_getBackgroundStamp();
				GetExtendedTimeInfo(&tstamp, message2);
				strcpy(message, "Background measured ");
				strcat(message, message2);
				strcat(message, " ");
				SetAmuletString(100, message);
				Mca_getROIChannels(AmuletWellSchillingMenu_test.StartROI, AmuletWellSchillingMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellSchillingMenu_test.Background.Spectrum.AutoCal);
				AmuletWellSchillingMenu_test.BackgroundROICounts = Mca_getROIBackgroundCounts(calcStart, calcEnd, &AmuletWellSchillingMenu_test.Background);
				AmuletWellSchillingMenu_test.BackgroundROICPM = Mca_getROIBackgroundCPM(calcStart, calcEnd, &AmuletWellSchillingMenu_test.Background);
				backgroundcps = AmuletWellSchillingMenu_test.BackgroundROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellSchillingMenu_test.BackgroundROICPM);
				else sprintf(message, "%.0f cps", backgroundcps);
				SetAmuletString(102, message);
			}else{
				AmuletWellSchillingMenu_test.BackgroundROICPM = -1;
				SetAmuletString(100, "Measured Background:");
			}

			if(AmuletWellSchillingMenu_test.UrineVolume != -1){
				sprintf(message, "%.1f", AmuletWellSchillingMenu_test.UrineVolume);
				SetAmuletString(103, message);
			}

			if(AmuletWellSchillingMenu_test.AliquotVolume != -1){
				sprintf(message, "%.1f", AmuletWellSchillingMenu_test.AliquotVolume);
				SetAmuletString(104, message);
			}

			if(AmuletWellSchillingMenu_test.DilutionFactor != -1){
				sprintf(message, "%.1f", AmuletWellSchillingMenu_test.DilutionFactor);
				SetAmuletString(105, message);
			}

			if(AmuletWellSchillingMenu_test.StandardROICPM != -1){
				standardcps = AmuletWellSchillingMenu_test.StandardROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellSchillingMenu_test.StandardROICPM);
				else sprintf(message, "%.0f cps", standardcps);
				SetAmuletString(106, message);
			}

			if(AmuletWellSchillingMenu_test.AliquotROICPM != -1){
				aliqotcps = AmuletWellSchillingMenu_test.AliquotROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellSchillingMenu_test.AliquotROICPM);
				else sprintf(message, "%.0f cps", aliqotcps);
				SetAmuletString(107, message);
			}

			SetAmuletByte(100, 0xFF);

			if((AmuletWellSchillingMenu_test.BackgroundROICPM != -1) && (AmuletWellSchillingMenu_test.UrineVolume != -1) && (AmuletWellSchillingMenu_test.AliquotVolume != -1) && (AmuletWellSchillingMenu_test.DilutionFactor != -1) && (AmuletWellSchillingMenu_test.StandardROICPM != -1) && (AmuletWellSchillingMenu_test.AliquotROICPM != -1)){
				SetAmuletByte(101, 0xFF);
			}
			m_iPhase = PHASE_WELLSCHILLING_WAIT;
			break;

		case PHASE_WELLSCHILLING_WAIT:
			break;

		case PHASE_WELLSCHILLING_CALC:
			m_iPhase = PHASE_WELLSCHILLING_WAIT;
			beep_amulet();

			// Check Aliquot is not bigger than Urine
			if(AmuletWellSchillingMenu_test.AliquotVolume > AmuletWellSchillingMenu_test.UrineVolume){
				Amulet_DisplayError("Schilling Test Error", "Aliquot Volume can not be greater then Urine Volume",TRUE);
				return;
			}

			// Check Background is not bigger than Measured Standard Activity
			if(AmuletWellSchillingMenu_test.BackgroundROICPM >= AmuletWellSchillingMenu_test.StandardROICPM){
				Amulet_DisplayError("Schilling Test Error", "Background Counts can not be greater than or equal to the Standard Counts",TRUE);
				return;
			}

			// Check Background is not bigger than Measured Aliquot Activity
			if(AmuletWellSchillingMenu_test.BackgroundROICPM >= AmuletWellSchillingMenu_test.AliquotROICPM){
				Amulet_DisplayError("Schilling Test Error", "Background Counts can not be greater than or equal to the Aliquot Counts",TRUE);
				return;
			}

			aliquotnet = AmuletWellSchillingMenu_test.AliquotROICPM - AmuletWellSchillingMenu_test.BackgroundROICPM;
			standnet = AmuletWellSchillingMenu_test.StandardROICPM - AmuletWellSchillingMenu_test.BackgroundROICPM;
			num = aliquotnet * AmuletWellSchillingMenu_test.UrineVolume;
			denom = standnet * AmuletWellSchillingMenu_test.DilutionFactor * AmuletWellSchillingMenu_test.AliquotVolume;
			AmuletWellSchillingMenu_test.Excretion = 100 * num / denom;

			AmuletWellSchillingMenu_test.CreatedOn = clock_time;
			SetAmuletHTML(AmuletHTMLIndex[WELLSCHILLINGANALYSIS_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSCHILLINGANALYSIS_HTM]);
			break;
	}
}

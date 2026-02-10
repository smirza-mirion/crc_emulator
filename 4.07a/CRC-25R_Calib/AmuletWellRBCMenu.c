#define PHASE_WELLRBC_PRE_INIT	0
#define PHASE_WELLRBC_WAIT		1
#define PHASE_WELLRBC_CALC		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "amulet.h"
#include "database.h"

WELLRBCTEST AmuletWellRBCMenu_test;

bool AmuletWellRBCMenu_backgroundMeasured;

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern time_t clock_time;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);

void AmuletWellRBC_menu(void){
	char message[51], message2[51];
	float backgroundcps, cps, dosehema, pathema, netwholestd, netwholesamp, netplasmastd, netplasmasamp, denom;
	time_t tstamp;
	short calcStart, calcEnd;
	int index;

	switch(m_iPhase){
		case PHASE_WELLRBC_PRE_INIT:
			if(m_ucClear == 43){
#ifdef DEMO_DATA
				if(DEMO_DATA){
					AmuletWellRBCMenu_test.DoseHematocrit = 35;
					AmuletWellRBCMenu_test.PatientHematocrit = 40;
					AmuletWellRBCMenu_test.PatientWeight = 120;
					AmuletWellRBCMenu_test.WholeBloodStandardROICPM = 2000;
					AmuletWellRBCMenu_test.PlasmaStandardROICPM = 3000;
					AmuletWellRBCMenu_test.WholeBloodSampleROICPM = 4000;
					AmuletWellRBCMenu_test.PlasmaSampleROICPM = 5000;
					AmuletWellRBCMenu_backgroundMeasured = TRUE;
				}
#else
				AmuletWellRBCMenu_test.DoseHematocrit = -1;
				AmuletWellRBCMenu_test.PatientHematocrit = -1;
				AmuletWellRBCMenu_test.PatientWeight = -1;
				AmuletWellRBCMenu_test.StartROI = 100.0;
				AmuletWellRBCMenu_test.EndROI = 500.0;
				AmuletWellRBCMenu_test.WholeBloodStandardROICounts = 0;
				AmuletWellRBCMenu_test.WholeBloodStandardROICPM = -1;
				AmuletWellRBCMenu_test.PlasmaStandardROICounts = 0;
				AmuletWellRBCMenu_test.PlasmaStandardROICPM = -1;
				AmuletWellRBCMenu_test.WholeBloodSampleROICounts = 0;
				AmuletWellRBCMenu_test.WholeBloodSampleROICPM = -1;
				AmuletWellRBCMenu_test.PlasmaSampleROICounts = 0;
				AmuletWellRBCMenu_test.PlasmaSampleROICPM = -1;
				AmuletWellRBCMenu_test.BackgroundROICounts = 0;
				AmuletWellRBCMenu_test.BackgroundROICPM = -1;
				AmuletWellRBCMenu_test.RBCVolume = 0;
				AmuletWellRBCMenu_test.RBCPerKg = 0;
				AmuletWellRBCMenu_test.WholeBloodVolume = 0;
				AmuletWellRBCMenu_test.WholeBloodPerKg = 0;
				AmuletWellRBCMenu_test.PlasmaVolume = 0;
				AmuletWellRBCMenu_test.PlasmaPerKg = 0;
				AmuletWellRBCMenu_test.WholeBloodStandard_Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellRBCMenu_test.WholeBloodStandard_Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellRBCMenu_test.PlasmaStandard_Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellRBCMenu_test.PlasmaStandard_Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellRBCMenu_test.WholeBloodSample_Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellRBCMenu_test.WholeBloodSample_Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellRBCMenu_test.PlasmaSample_Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellRBCMenu_test.PlasmaSample_Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellRBCMenu_test.Background.WellBackgroundID = 0;
				AmuletWellRBCMenu_test.Background.Spectrum.SpectraID = 0;
				for(index=0; index<4096; index++) AmuletWellRBCMenu_test.Background.Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellRBCMenu_test.TestIdent.TestIdentID = 0;
				AmuletWellRBCMenu_test.TestIdent.TestIdentGroupID = 0;
				AmuletWellRBCMenu_test.TestIdent.TestID[0] = 0;
				AmuletWellRBCMenu_test.TestIdent.PatientID[0] = 0;
				AmuletWellRBCMenu_test.TestIdent.FirstName[0] = 0;
				AmuletWellRBCMenu_test.TestIdent.LastName[0] = 0;
				AmuletWellRBCMenu_test.TestIdent.DateOfBirth = 0.0;
				AmuletWellRBCMenu_test.TestIdent.Sex[0] = 0;
				AmuletWellRBCMenu_test.TestIdent.Physician[0] = 0;
				AmuletWellRBCMenu_test.TestIdent.TechID[0] = 0;
				AmuletWellRBCMenu_test.TestIdent.CreatedOn = 0;
				AmuletWellRBCMenu_test.TestIdent.LastUpdated = 0;
				AmuletWellRBCMenu_test.TestIdent.Inactive = FALSE;
				AmuletWellRBCMenu_test.CreatedOn = 0;
				AmuletWellRBCMenu_test.InactiveReason[0] = 0;
				AmuletWellRBCMenu_test.Inactive = FALSE;
				AmuletWellRBCMenu_backgroundMeasured = FALSE;
#endif
				m_ucClear = 0;
			}
			Mca_switchToWell();
			Mca_sendDetectorType();

			if(AmuletWellRBCMenu_backgroundMeasured){
				AmuletWellRBCMenu_test.Background.WellBackgroundID = ((BACKGND *)Mca_getBackgroundMirror())->WellBackgroundID;
				DB_RetrieveBackgnd(&(AmuletWellRBCMenu_test.Background));
				tstamp = Mca_getBackgroundStamp();
				GetExtendedTimeInfo(&tstamp, message2);
				strcpy(message, "Background measured ");
				strcat(message, message2);
				strcat(message, " ");
				SetAmuletString(100, message);
				Mca_getROIChannels(AmuletWellRBCMenu_test.StartROI, AmuletWellRBCMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellRBCMenu_test.Background.Spectrum.AutoCal);
				AmuletWellRBCMenu_test.BackgroundROICounts = Mca_getROIBackgroundCounts(calcStart, calcEnd, &AmuletWellRBCMenu_test.Background);
				AmuletWellRBCMenu_test.BackgroundROICPM = Mca_getROIBackgroundCPM(calcStart, calcEnd, &AmuletWellRBCMenu_test.Background);
				backgroundcps = AmuletWellRBCMenu_test.BackgroundROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCMenu_test.BackgroundROICPM);
				else sprintf(message, "%.0f cps", backgroundcps);
				SetAmuletString(102, message);
			}else{
				AmuletWellRBCMenu_test.BackgroundROICPM = -1;
				SetAmuletString(100, "Measured Background:");
			}

			if(AmuletWellRBCMenu_test.DoseHematocrit != -1){
				sprintf(message, "%.1f", AmuletWellRBCMenu_test.DoseHematocrit);
				SetAmuletString(103, message);
			}

			if(AmuletWellRBCMenu_test.PatientHematocrit != -1){
				sprintf(message, "%.1f", AmuletWellRBCMenu_test.PatientHematocrit);
				SetAmuletString(104, message);
			}

			if(AmuletWellRBCMenu_test.PatientWeight != -1){
				sprintf(message, "%.1f", AmuletWellRBCMenu_test.PatientWeight);
				SetAmuletString(105, message);
			}

			if(AmuletWellRBCMenu_test.WholeBloodStandardROICPM != -1){
				cps = AmuletWellRBCMenu_test.WholeBloodStandardROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCMenu_test.WholeBloodStandardROICPM);
				else sprintf(message, "%.0f cps", cps);
				SetAmuletString(106, message);
			}

			if(AmuletWellRBCMenu_test.PlasmaStandardROICPM != -1){
				cps = AmuletWellRBCMenu_test.PlasmaStandardROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCMenu_test.PlasmaStandardROICPM);
				else sprintf(message, "%.0f cps", cps);
				SetAmuletString(107, message);
			}

			if(AmuletWellRBCMenu_test.WholeBloodSampleROICPM != -1){
				cps = AmuletWellRBCMenu_test.WholeBloodSampleROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCMenu_test.WholeBloodSampleROICPM);
				else sprintf(message, "%.0f cps", cps);
				SetAmuletString(108, message);
			}

			if(AmuletWellRBCMenu_test.PlasmaSampleROICPM != -1){
				cps = AmuletWellRBCMenu_test.PlasmaSampleROICPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellRBCMenu_test.PlasmaSampleROICPM);
				else sprintf(message, "%.0f cps", cps);
				SetAmuletString(109, message);
			}

			SetAmuletByte(100, 0xFF);

			if((AmuletWellRBCMenu_test.BackgroundROICPM != -1) && (AmuletWellRBCMenu_test.DoseHematocrit != -1) && (AmuletWellRBCMenu_test.PatientHematocrit != -1) && (AmuletWellRBCMenu_test.PatientWeight != -1) && (AmuletWellRBCMenu_test.WholeBloodStandardROICPM != -1) && (AmuletWellRBCMenu_test.PlasmaStandardROICPM != -1) && (AmuletWellRBCMenu_test.WholeBloodSampleROICPM != -1) && (AmuletWellRBCMenu_test.PlasmaSampleROICPM != -1)){
				SetAmuletByte(101, 0xFF);
			}

			m_iPhase = PHASE_WELLRBC_WAIT;
			break;

		case PHASE_WELLRBC_WAIT:
			break;

		case PHASE_WELLRBC_CALC:
			beep_amulet();

			if(AmuletWellRBCMenu_test.BackgroundROICPM >= AmuletWellRBCMenu_test.WholeBloodStandardROICPM){
				Amulet_DisplayError("RBC Test Error", "Background can not be greater than or equal to the Whole Blood Standard",TRUE);
				return;
			}
			if(AmuletWellRBCMenu_test.BackgroundROICPM >= AmuletWellRBCMenu_test.PlasmaStandardROICPM){
				Amulet_DisplayError("RBC Test Error", "Background can not be greater than or equal to the Plasma Standard",TRUE);
				return;
			}
			if(AmuletWellRBCMenu_test.BackgroundROICPM >= AmuletWellRBCMenu_test.WholeBloodSampleROICPM){
				Amulet_DisplayError("RBC Test Error", "Background can not be greater than or equal to the Whole Blood Sample",TRUE);
				return;
			}
			if(AmuletWellRBCMenu_test.BackgroundROICPM >= AmuletWellRBCMenu_test.PlasmaSampleROICPM){
				Amulet_DisplayError("RBC Test Error", "Background can not be greater than or equal to the Plasma Sample",TRUE);
				return;
			}

			dosehema = AmuletWellRBCMenu_test.DoseHematocrit / 100.0;
			pathema = AmuletWellRBCMenu_test.PatientHematocrit / 100.0;

			netwholestd = AmuletWellRBCMenu_test.WholeBloodStandardROICPM - AmuletWellRBCMenu_test.BackgroundROICPM;
			netwholesamp = AmuletWellRBCMenu_test.WholeBloodSampleROICPM - AmuletWellRBCMenu_test.BackgroundROICPM;
			netplasmastd = AmuletWellRBCMenu_test.PlasmaStandardROICPM - AmuletWellRBCMenu_test.BackgroundROICPM;
			netplasmasamp = AmuletWellRBCMenu_test.PlasmaSampleROICPM - AmuletWellRBCMenu_test.BackgroundROICPM;

			AmuletWellRBCMenu_test.RBCVolume = 1000.0 * (netwholestd - (netplasmastd * (1.0 - dosehema))) * pathema;
			denom = netwholesamp - (netplasmasamp * (1.0 - pathema));

			AmuletWellRBCMenu_test.RBCVolume /= denom;
			AmuletWellRBCMenu_test.WholeBloodVolume = AmuletWellRBCMenu_test.RBCVolume / pathema;
			AmuletWellRBCMenu_test.PlasmaVolume = AmuletWellRBCMenu_test.WholeBloodVolume - AmuletWellRBCMenu_test.RBCVolume;

			AmuletWellRBCMenu_test.WholeBloodPerKg = AmuletWellRBCMenu_test.WholeBloodVolume / AmuletWellRBCMenu_test.PatientWeight;
			AmuletWellRBCMenu_test.PlasmaPerKg = AmuletWellRBCMenu_test.PlasmaVolume / AmuletWellRBCMenu_test.PatientWeight;
			AmuletWellRBCMenu_test.RBCPerKg = AmuletWellRBCMenu_test.RBCVolume / AmuletWellRBCMenu_test.PatientWeight;


			AmuletWellRBCMenu_test.CreatedOn = clock_time;
			SetAmuletHTML(AmuletHTMLIndex[WELLRBCANALYSIS_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLRBCANALYSIS_HTM]);
			return;
			//break;
	}
}

#define PHASE_WELLBIOASSAY_PRE_INIT	0
#define PHASE_WELLBIOASSAY_WAIT		1
#define PHASE_WELLBIOASSAY_NEXT		2
#define PHASE_WELLBIOASSAY_EXIT		3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "amulet.h"
#include "database.h"
#include "bioassay.h"
#include "nuc.h"

bool AmuletWellBioAssay_accepted;
PROBEBIOASSAYTEST AmuletWellBioAssay_test;
bool AmuletWellBioAssay_backgroundMeasured;
int AmuletWellBioAssay_staffID;
int AmuletWellBioAssay_probeDistance;

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern bool AmuletLoginMenu_blockGuest;
extern int AmuletLoginMenu_minRole;
extern float I131Lower_keV, I131Upper_keV, I125Lower_keV, I125Upper_keV, I123Lower_keV, I123Upper_keV;
extern PROBEBIOASSAYTEST AmuletWellBioAssayAnalysis_test;

void SetAmuletBackHTML(void);
void PushPageStack(unsigned char ucPage);
unsigned char PopPageStack(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);

void AmuletWellBioAssay_menu(void){
	float BackgroundCPM, NeckCPM, dpm;
	char message[52];
	short nucIndex, startCh, endCh;
	USER guestuser, user;

	switch(m_iPhase){
		case PHASE_WELLBIOASSAY_PRE_INIT:
			DB_GetUser("Guest", &guestuser);

			if(m_ucClear == 83){
				m_ucClear = 0;
				AmuletWellBioAssay_probeDistance = BioAssayProbeDistance();
				AmuletWellBioAssay_test.UserName[0] = 0;
				AmuletWellBioAssay_test.FirstName[0] = 0;
				AmuletWellBioAssay_test.LastName[0] = 0;
				AmuletWellBioAssay_staffID = -1;
				AmuletWellBioAssay_test.Spectrum.SpectraID = -1;
				AmuletWellBioAssay_backgroundMeasured = FALSE;

				if((current.security_mode == 1 && guestuser.UserID == current.user_id) || (current.security_mode == 2)){
					AmuletLoginMenu_blockGuest = TRUE;
					AmuletLoginMenu_minRole = 0;
					m_ucClear = 75;
					SetAmuletHTML(AmuletHTMLIndex[LOGIN_HTM]);
					PushPageStack(AmuletHTMLIndex[LOGIN_HTM]);
					AmuletWellBioAssay_accepted = FALSE;
					return;
				}else{
					AmuletWellBioAssay_accepted = TRUE;
				}
			}

			if(!AmuletWellBioAssay_accepted || (guestuser.UserID == current.user_id)){
				SetAmuletBackHTML();
				return;
			}

			if(!BioAssayI131Active() && !BioAssayI125Active() && !BioAssayI123Active()){
				if(current.security_mode == 0){
				DB_GetUser("root", &user);
				current.user_id = user.UserID;
				}else if(current.security_mode == 2){
					DB_GetUser("Guest", &user);
					current.user_id = user.UserID;
				}
				PopPageStack();
				Amulet_DisplayError("BioAssay", "None of the Iodine Nuclides are active", TRUE);
				return;
			}

			Mca_switchToProbe();

			DB_GetUserFromID(current.user_id, &user);
			if(user.Role < 2){
				AmuletWellBioAssay_staffID = current.user_id;
				strcpy(AmuletWellBioAssay_test.UserName, user.UserName);
				strcpy(AmuletWellBioAssay_test.FirstName, user.FirstName);
				strcpy(AmuletWellBioAssay_test.LastName, user.LastName);
				sprintf(message, "%s %s", AmuletWellBioAssay_test.FirstName, AmuletWellBioAssay_test.LastName);
				SetAmuletString(100, message);
				SetAmuletByte(80, 0x00);
				SetAmuletByte(81, 0xFF);
			}else{
				if(AmuletWellBioAssay_staffID == -1){
					AmuletWellBioAssay_test.UserName[0] = 0;
					AmuletWellBioAssay_test.FirstName[0] = 0;
					AmuletWellBioAssay_test.LastName[0] = 0;
					message[0] = 0;
				}else{
					DB_GetUserFromID(AmuletWellBioAssay_staffID, &user);
					strcpy(AmuletWellBioAssay_test.UserName, user.UserName);
					strcpy(AmuletWellBioAssay_test.FirstName, user.FirstName);
					strcpy(AmuletWellBioAssay_test.LastName, user.LastName);
					sprintf(message, "%s %s", AmuletWellBioAssay_test.FirstName, AmuletWellBioAssay_test.LastName);
				}
				SetAmuletString(100, message);
				SetAmuletByte(80, 0xFF);
				SetAmuletByte(81, 0x00);
			}

			sprintf(message, "%d cm", AmuletWellBioAssay_probeDistance);
			SetAmuletString(102, message);

			if(AmuletWellBioAssay_backgroundMeasured){
				BackgroundCPM = ((BACKGND *)Mca_getBackgroundMirror())->Spectrum.TotalCPM;
				if(current.system == CI) sprintf(message, "%.0f cpm", BackgroundCPM);
				else sprintf(message, "%.0f cps", BackgroundCPM / 60.0);
			}else{
				message[0] = 0;
			}
			SetAmuletString(103, message);

			if(AmuletWellBioAssay_test.Spectrum.SpectraID != -1){
				NeckCPM = AmuletWellBioAssay_test.Spectrum.TotalCPM;
				if(current.system == CI) sprintf(message, "%.0f cpm", NeckCPM);
				else sprintf(message, "%.0f cps", NeckCPM / 60.0);
			}else{
				message[0] = 0;
			}
			SetAmuletString(104, message);

			if((AmuletWellBioAssay_staffID != -1) && (AmuletWellBioAssay_backgroundMeasured) && (AmuletWellBioAssay_test.Spectrum.SpectraID != -1)){
				SetAmuletByte(82, 0xFF);
			}

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLBIOASSAY_WAIT;
			break;

		case PHASE_WELLBIOASSAY_WAIT:
			break;

		case PHASE_WELLBIOASSAY_NEXT:
			beep_amulet();

			AmuletWellBioAssay_test.ProbeBioAssayTestID = 0;

			AmuletWellBioAssay_test.ProbeDistance = AmuletWellBioAssay_probeDistance;

			nucIndex = NuclideData_getIndexFromName("I 131");
			AmuletWellBioAssay_test.I131PrimaryEnergy = NuclideData_getEffectivePrimary(nucIndex);
			AmuletWellBioAssay_test.I131StartEV = I131Lower_keV;
			AmuletWellBioAssay_test.I131EndEV = I131Upper_keV;

			nucIndex = NuclideData_getIndexFromName("I 125");
			AmuletWellBioAssay_test.I125PrimaryEnergy = NuclideData_getEffectivePrimary(nucIndex);
			AmuletWellBioAssay_test.I125StartEV = I125Lower_keV;
			AmuletWellBioAssay_test.I125EndEV = I125Upper_keV;

			nucIndex = NuclideData_getIndexFromName("I 123");
			AmuletWellBioAssay_test.I123PrimaryEnergy = NuclideData_getEffectivePrimary(nucIndex);
			AmuletWellBioAssay_test.I123StartEV = I123Lower_keV;
			AmuletWellBioAssay_test.I123EndEV = I123Upper_keV;

			if(BioAssayI131Active()){
				AmuletWellBioAssay_test.I131Active = TRUE;
				Mca_getROIChannels(AmuletWellBioAssay_test.I131StartEV, AmuletWellBioAssay_test.I131EndEV, &startCh, &endCh, NULL);
				AmuletWellBioAssay_test.I131ROICounts = Mca_getROICounts(AmuletWellBioAssay_test.Spectrum.DecompressedSpectra, startCh, endCh);
				AmuletWellBioAssay_test.I131ROICPM = AmuletWellBioAssay_test.I131ROICounts;
				AmuletWellBioAssay_test.I131ROICPM /= AmuletWellBioAssay_test.Spectrum.LiveTime;
				AmuletWellBioAssay_test.I131ROICPM *= 60.0;
				AmuletWellBioAssay_test.I131BackgroundROICounts = Mca_getROIBackgroundCounts(startCh, endCh, NULL);
				AmuletWellBioAssay_test.I131BackgroundROICPM = Mca_getROIBackgroundCPM(startCh, endCh, NULL);
				AmuletWellBioAssay_test.I131NetROICPM = AmuletWellBioAssay_test.I131ROICPM - AmuletWellBioAssay_test.I131BackgroundROICPM;
				if(BioAssayI131Efficiency() != -1.0){
					if(AmuletWellBioAssay_test.I131NetROICPM > 0.0){
						dpm = AmuletWellBioAssay_test.I131NetROICPM / (0.01 * BioAssayI131Efficiency());
						AmuletWellBioAssay_test.I131Activity = Mca_convertDpmToCi(dpm);
						if(AmuletWellBioAssay_test.I131Activity > BioAssayI131Threshold()) AmuletWellBioAssay_test.I131High = TRUE;
						else AmuletWellBioAssay_test.I131High = FALSE;
					}else{
						AmuletWellBioAssay_test.I131Activity = 0.0;
						AmuletWellBioAssay_test.I131High = FALSE;
					}
				}else{
					AmuletWellBioAssay_test.I131Activity = -1.0;
					AmuletWellBioAssay_test.I131High = FALSE;
				}

				if(BioAssayI131I125Contamination() != 1.0){
					AmuletWellBioAssay_test.I131I125NetROICPM = AmuletWellBioAssay_test.I131NetROICPM * BioAssayI131I125Contamination();
				}else{
					AmuletWellBioAssay_test.I131I125NetROICPM = 0.0;
				}
			}else{
				AmuletWellBioAssay_test.I131Active = FALSE;
				AmuletWellBioAssay_test.I131ROICounts = 0;
				AmuletWellBioAssay_test.I131ROICPM = 0.0;
				AmuletWellBioAssay_test.I131BackgroundROICounts = 0;
				AmuletWellBioAssay_test.I131BackgroundROICPM = 0.0;
				AmuletWellBioAssay_test.I131NetROICPM = 0.0;
				AmuletWellBioAssay_test.I131Activity = 0.0;
				AmuletWellBioAssay_test.I131High = FALSE;
				AmuletWellBioAssay_test.I131I125NetROICPM = 0.0;
			}

			if(BioAssayI123Active()){
				AmuletWellBioAssay_test.I123Active = TRUE;
				Mca_getROIChannels(AmuletWellBioAssay_test.I123StartEV, AmuletWellBioAssay_test.I123EndEV, &startCh, &endCh, NULL);
				AmuletWellBioAssay_test.I123ROICounts = Mca_getROICounts(AmuletWellBioAssay_test.Spectrum.DecompressedSpectra, startCh, endCh);
				AmuletWellBioAssay_test.I123ROICPM = AmuletWellBioAssay_test.I123ROICounts;
				AmuletWellBioAssay_test.I123ROICPM /= AmuletWellBioAssay_test.Spectrum.LiveTime;
				AmuletWellBioAssay_test.I123ROICPM *= 60.0;
				AmuletWellBioAssay_test.I123BackgroundROICounts = Mca_getROIBackgroundCounts(startCh, endCh, NULL);
				AmuletWellBioAssay_test.I123BackgroundROICPM = Mca_getROIBackgroundCPM(startCh, endCh, NULL);
				AmuletWellBioAssay_test.I123NetROICPM = AmuletWellBioAssay_test.I123ROICPM - AmuletWellBioAssay_test.I123BackgroundROICPM;
				if(BioAssayI123Efficiency() != -1.0){
					if(AmuletWellBioAssay_test.I123NetROICPM > 0.0){
						dpm = AmuletWellBioAssay_test.I123NetROICPM / (0.01 * BioAssayI123Efficiency());
						AmuletWellBioAssay_test.I123Activity = Mca_convertDpmToCi(dpm);
						if(AmuletWellBioAssay_test.I123Activity > BioAssayI123Threshold()) AmuletWellBioAssay_test.I123High = TRUE;
						else AmuletWellBioAssay_test.I123High = FALSE;
					}else{
						AmuletWellBioAssay_test.I123Activity = 0.0;
						AmuletWellBioAssay_test.I123High = FALSE;
					}
				}else{
					AmuletWellBioAssay_test.I123Activity = -1.0;
					AmuletWellBioAssay_test.I123High = FALSE;
				}

				if(BioAssayI123I125Contamination() != -1.0){
					AmuletWellBioAssay_test.I123I125NetROICPM = AmuletWellBioAssay_test.I123NetROICPM * BioAssayI123I125Contamination();
				}else{
					AmuletWellBioAssay_test.I123I125NetROICPM = 0.0;
				}
			}else{
				AmuletWellBioAssay_test.I123Active = FALSE;
				AmuletWellBioAssay_test.I123ROICounts = 0;
				AmuletWellBioAssay_test.I123ROICPM = 0.0;
				AmuletWellBioAssay_test.I123BackgroundROICounts = 0;
				AmuletWellBioAssay_test.I123BackgroundROICPM = 0.0;
				AmuletWellBioAssay_test.I123NetROICPM = 0.0;
				AmuletWellBioAssay_test.I123Activity = 0.0;
				AmuletWellBioAssay_test.I123High = FALSE;
				AmuletWellBioAssay_test.I123I125NetROICPM = 0.0;
			}

			if(BioAssayI125Active()){
				AmuletWellBioAssay_test.I125Active = TRUE;
				Mca_getROIChannels(AmuletWellBioAssay_test.I125StartEV, AmuletWellBioAssay_test.I125EndEV, &startCh, &endCh, NULL);
				AmuletWellBioAssay_test.I125ROICounts = Mca_getROICounts(AmuletWellBioAssay_test.Spectrum.DecompressedSpectra, startCh, endCh);
				AmuletWellBioAssay_test.I125ROICPM = AmuletWellBioAssay_test.I125ROICounts;
				AmuletWellBioAssay_test.I125ROICPM /= AmuletWellBioAssay_test.Spectrum.LiveTime;
				AmuletWellBioAssay_test.I125ROICPM *= 60.0;
				AmuletWellBioAssay_test.I125BackgroundROICounts = Mca_getROIBackgroundCounts(startCh, endCh, NULL);
				AmuletWellBioAssay_test.I125BackgroundROICPM = Mca_getROIBackgroundCPM(startCh, endCh, NULL);
				AmuletWellBioAssay_test.I125NetROICPM = AmuletWellBioAssay_test.I125ROICPM - AmuletWellBioAssay_test.I125BackgroundROICPM;
				AmuletWellBioAssay_test.I125NetROICPMCorrected = AmuletWellBioAssay_test.I125NetROICPM - AmuletWellBioAssay_test.I131I125NetROICPM - AmuletWellBioAssay_test.I123I125NetROICPM;
				if(BioAssayI125Efficiency() != -1.0){
					if(AmuletWellBioAssay_test.I125NetROICPMCorrected > 0.0){
						dpm = AmuletWellBioAssay_test.I125NetROICPMCorrected / (0.01 * BioAssayI125Efficiency());
						AmuletWellBioAssay_test.I125Activity = Mca_convertDpmToCi(dpm);
						if(AmuletWellBioAssay_test.I125Activity > BioAssayI125Threshold()) AmuletWellBioAssay_test.I125High = TRUE;
						else AmuletWellBioAssay_test.I125High = FALSE;
					}else{
						AmuletWellBioAssay_test.I125Activity = 0.0;
						AmuletWellBioAssay_test.I125High = FALSE;
					}
				}else{
					AmuletWellBioAssay_test.I125Activity = -1.0;
					AmuletWellBioAssay_test.I125High = FALSE;
				}
			}else{
				AmuletWellBioAssay_test.I125Active = FALSE;
				AmuletWellBioAssay_test.I125ROICounts = 0;
				AmuletWellBioAssay_test.I125ROICPM = 0.0;
				AmuletWellBioAssay_test.I125BackgroundROICounts = 0;
				AmuletWellBioAssay_test.I125BackgroundROICPM = 0.0;
				AmuletWellBioAssay_test.I125NetROICPM = 0.0;
				AmuletWellBioAssay_test.I125NetROICPMCorrected = 0.0;
				AmuletWellBioAssay_test.I125Activity = 0.0;
				AmuletWellBioAssay_test.I125High = FALSE;
			}
			AmuletWellBioAssay_test.WellBackgroundID = ((BACKGND *) Mca_getBackgroundMirror())->WellBackgroundID;
			AmuletWellBioAssay_test.SpectraID = 0;
			AmuletWellBioAssay_test.ProbeBioAssayEfficiencyID = BioAssayEfficiencyID();
			AmuletWellBioAssay_test.ProbeBioAssaySettingID = BioAssaySettingID();
			AmuletWellBioAssay_test.CreatedOn = AmuletWellBioAssay_test.Spectrum.MeasuredOn;
			AmuletWellBioAssay_test.Comment[0] = 0;
			AmuletWellBioAssay_test.InactiveReason[0] = 0;
			AmuletWellBioAssay_test.Inactive = FALSE;

			memcpy(&AmuletWellBioAssayAnalysis_test, &AmuletWellBioAssay_test, sizeof(PROBEBIOASSAYTEST));

			m_ucClear = 84;
			SetAmuletHTML(AmuletHTMLIndex[WELLBIOASSAYANALYSIS_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLBIOASSAYANALYSIS_HTM]);
			return;

		case PHASE_WELLBIOASSAY_EXIT:
			if(current.security_mode == 0){
				DB_GetUser("root", &user);
				current.user_id = user.UserID;
			}else if(current.security_mode == 2){
				DB_GetUser("Guest", &user);
				current.user_id = user.UserID;
			}
			SetAmuletBackHTML();
			break;
	}
}

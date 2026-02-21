#define PHASE_GENERICITEMS_PRE_INIT	0
#define PHASE_GENERICITEMS_WAIT		1
#define PHASE_GENERICITEMS_BUTTON1	2
#define PHASE_GENERICITEMS_BUTTON2	3
#define PHASE_GENERICITEMS_BUTTON3	4
#define PHASE_GENERICITEMS_BUTTON4	5
#define PHASE_GENERICITEMS_BUTTON5	6
#define PHASE_GENERICITEMS_BUTTON6	7
#define PHASE_GENERICITEMS_BUTTON7	8
#define PHASE_GENERICITEMS_BUTTON8	9

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "database.h"
#include "wipes.h"
#include "message.h"

#define CONFIG_WIPESEARCH_WIPETYPE	1
#define CONFIG_DETECTOR700			2
#define CONFIG_BRANDING			3
#define CONFIG_AUTOCALSEARCH_DEVTYPE	4
#define CONFIG_SYSTEMTESTSEARCH_DEVTYPE	5
#define CONFIG_MDASEARCH_DEVTYPE	6
#define CONFIG_CHISQUARE_DEVTYPE	7
#define CONFIG_TU_COUNTING_METHOD	8
#define CONFIG_TU_DOSE_MEASUREMENT	9
#define CONFIG_TU_ISOTOPE			10
#define CONFIG_TU_DOSE_FORM			11
#define CONFIG_TU_PRE_DOSE			12
#define CONFIG_TU_DELETE_TEST		13
#define CONFIG_TU_RESIDUAL			14
#define CONFIG_TU_REACTIVATE		15
#define CONFIG_LOCK_NUCLIDE_KEV		16
#define CONFIG_RBC_SURVIVAL_REACTIVATE	17
#define CONFIG_RBC_SURVIVAL_DELETE_TEST	18
#define CONFIG_CONFIRM_UPDATE			19
#define CONFIG_OVERWRITE_WITH_DEFAULT	20
#define CONFIG_DATE_FORMAT				21
#define CONFIG_USB_DRIVER				22
#define CONFIG_FUTURE_DATE_ENTRY		23
#define CONFIG_LANGUAGE					24

extern int m_iPhase;
extern CURRENT current;
extern int AmuletWellWipeSearch_wipeType;
extern char pr_crcname[];
extern char pr_crcname_large[];
extern unsigned char m_ucClear;
extern int AmuletWellAutoCalSearch_device;
extern int AmuletWellSystemTestSearch_device;
extern int AmuletWellMDASearch_device;
extern int AmuletWellChiSearch_device;
extern PROBETHYROIDUPTAKEPROTOCOL AmuletAddEditThyroidUptakeProtocol_protocol;
extern PROBETHYROIDUPTAKETEST AmuletWellThyroidUptakeTest;
extern PROBETHYROIDUPTAKETEST AmuletAddEditThyroidUptakeTest_test;
extern WELLRBCSURVIVALTEST AmuletWellRBCSurvivalTest;
extern WELLRBCSURVIVALTEST AmuletAddEditRBCSurvivalTest_test;
extern char m_acUpdateVersionRead[21];
extern char m_acUpdateVersionOK[21];
extern WELLWIPELOCATION AmuletWellSetupAddEditLocation_wellWipeLocation;

uchar AmuletGenericItems_config;

void SetAmuletBackHTML(void);
unsigned char PreviousPageStack(void);
unsigned char PopPageStack(void);
void AmuletWellThyroidUptake_clearSearch(void);
void AmuletWellRBCSurvival_clearSearch(void);
void DB_shutdown(void);
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);
void DB_SetDefaultEnergy(void);
void startup_811(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void set_study_type(void);
void set_calicheck_tube(void);
void set_time_unit(void);

void AmuletGenericItems_menu(void){
	short newDetector;
	int index;
	char wipetype[20];
	char message[100];
	char formatstring[100];
	char titlestring[50];
	WELLWIPETYPE *wipeType;

	switch(m_iPhase){
		case PHASE_GENERICITEMS_PRE_INIT:
			switch(AmuletGenericItems_config){
				case CONFIG_WIPESEARCH_WIPETYPE:
					send_amulet_message(L_SELECT_WIPE_CRITERIA, 98);    // "Please select Wipe Criteria"
					send_amulet_message(L_ALL_WIPES, 101);    // "All Wipes"
					send_amulet_message(L_WORK_AREA, 102);    // "Work Area"
					send_amulet_message(L_UNRESTRICTED_AREA, 103);    // "Unrestricted Area"
					send_amulet_message(L_SEALED_SOURCE, 104);    // "Sealed Source"
					send_amulet_message(L_PACKAGE, 105);    // "Package"
					send_amulet_message(L_HIGH_ACTIVITY, 106);    // "High Activity"

					SetAmuletByte(100, 0xFF);
					SetAmuletByte(101, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(103, 0xFF);
					SetAmuletByte(104, 0xFF);
					SetAmuletByte(105, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_DETECTOR700:
					send_amulet_message(L_SELECT_DETECTOR, 98);    // "Please select Detector"
					send_amulet_message(L_PROBE, 102);    // "Probe"
					send_amulet_message(L_WELL, 106);    // "Well"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_BRANDING:
					send_amulet_message(L_SELECT_BRANDING, 98);    // "Please select Branding"
					send_to_amulet_string(101, "CRC-55t");
					send_to_amulet_string(102, "Caprac-t");
					send_to_amulet_string(103, "Captus-700t");
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(101, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(103, 0xFF);
					break;

				case CONFIG_AUTOCALSEARCH_DEVTYPE:
				case CONFIG_SYSTEMTESTSEARCH_DEVTYPE:
				case CONFIG_MDASEARCH_DEVTYPE:
				case CONFIG_CHISQUARE_DEVTYPE:
					send_amulet_message(L_SELECT_DETECTOR, 98);    // "Please select Detector"
					send_amulet_message(L_ALL, 101);    // "All"
					send_amulet_message(L_PROBE, 102);    // "Probe"
					send_amulet_message(L_WELL, 103);    // "Well"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(101, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(103, 0xFF);
					break;

				case CONFIG_TU_COUNTING_METHOD:
					send_amulet_message(L_SELECT_TU_METHOD, 98);    // "Please select Counting Method"
					send_amulet_message(L_DECAY_CORRECT_ADMIN_DOSE, 54);    // "Decay Correct Administered Dose"
					send_amulet_message(L_MEAS_REF_DOSE, 66);    // "Measure Same Reference Dose Before Each Uptake"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(112, 0xFF);
					SetAmuletByte(116, 0xFF);
					break;

				case CONFIG_TU_DOSE_MEASUREMENT:
					send_amulet_message(L_SELECT_DOSE_MEASUREMENT, 98);    // "Select Dose Measurement"
					send_amulet_message(L_DOSE_MEAS_IS_DOSE_ADMIN, 54);    // "Dose Measured is Dose Administered"
					send_amulet_message(L_MEAS_DOSE_AND_ADD, 57);    // "Measure Each Dose and Add Activity"
					send_amulet_message(L_MEAS_DOSE_AND_MULTIP, 66);    // "Measure One Capsule and Multiply by Number Administered"
					send_amulet_message(L_MEAS_LIQ_AND_MULTIP, 69);    // "Measure Liquid and Multiply by Factor"
					//send_to_amulet_string(72, "Measure Liquid and Subtract Residual");
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(112, 0xFF);
					SetAmuletByte(113, 0xFF);
					SetAmuletByte(116, 0xFF);
					SetAmuletByte(117, 0xFF);
					//SetAmuletByte(118, 0xFF);
					break;

				case CONFIG_TU_ISOTOPE:
					send_amulet_message(L_SELECT_NUCLIDE, 98);    // "Select Nuclide"
					send_to_amulet_string(102, "I123");
					send_to_amulet_string(106, "I131");
					send_to_amulet_string(107, "Tc99m");
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					SetAmuletByte(107, 0xFF);
					break;

				case CONFIG_TU_DOSE_FORM:
					send_amulet_message(L_SELECT_DOSE_FORM, 98);    // "Select Dose Form"
					send_amulet_message(L_CAPSULE, 102);    // "Capsule"
					send_amulet_message(L_LIQUID, 106);    // "Liquid"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_TU_PRE_DOSE:
					send_amulet_message(L_MEAS_PREDOSE_PATIENT, 98);    // "Measure Pre-Dose Patient?"
					send_amulet_message(L_YES_ENGLISH, 102);    // "Yes"
					send_amulet_message(L_NO_ENGLISH, 106);    // "No"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_TU_DELETE_TEST:
					send_amulet_message(L_DELETE_THYROID, 98);    // "Delete Thyroid Uptake Test?"
					send_amulet_message(L_YES_ENGLISH, 102);    // "Yes"
					send_amulet_message(L_NO_ENGLISH, 106);    // "No"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_TU_RESIDUAL:
					send_amulet_message(L_MEAS_REDISUAL, 98);    // "Measure Residual Dose?"
					send_amulet_message(L_YES_ENGLISH, 102);    // "Yes"
					send_amulet_message(L_NO_ENGLISH, 106);    // "No"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_TU_REACTIVATE:
					send_amulet_message(L_REACTIVATE_TU, 98);    // "ReActivate Thyroid Uptake Test?"
					send_amulet_message(L_YES_ENGLISH, 102);    // "Yes"
					send_amulet_message(L_NO_ENGLISH, 106);    // "No"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_LOCK_NUCLIDE_KEV:
					send_amulet_message(L_ONLY_DEFAULT_NUC, 98);    // "Use Only Default Nuclide keV?"
					send_amulet_message(L_YES_ENGLISH, 102);    // "Yes"
					send_amulet_message(L_NO_ENGLISH, 106);    // "No"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_RBC_SURVIVAL_REACTIVATE:
					send_amulet_message(L_REACTIVATE_RBC, 98);    // "ReActivate RBC Survival Test?"
					send_amulet_message(L_YES_ENGLISH, 102);    // "Yes"
					send_amulet_message(L_NO_ENGLISH, 106);    // "No"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_RBC_SURVIVAL_DELETE_TEST:
					send_amulet_message(L_DELETE_RBC, 98);    // "Delete RBC Survival Test?"
					send_amulet_message(L_YES_ENGLISH, 102);    // "Yes"
					send_amulet_message(L_NO_ENGLISH, 106);    // "No"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_CONFIRM_UPDATE:
					if((m_acUpdateVersionRead[0] == 'W') &&
					   (m_acUpdateVersionRead[1] == 'i') &&
					   (m_acUpdateVersionRead[2] == 'n') &&
					   (m_acUpdateVersionRead[3] == 's') &&
					   (m_acUpdateVersionRead[4] == 't') &&
					   (m_acUpdateVersionRead[5] == 'o') &&
					   (m_acUpdateVersionRead[6] == 'n') &&
					   (m_acUpdateVersionRead[17] == 'L') &&
					   (m_acUpdateVersionRead[18] == 'a') &&
					   (m_acUpdateVersionRead[19] == 'm')
					  ){

						for(index=16; index>= 0; index--){
							if(m_acUpdateVersionRead[index] == 32) m_acUpdateVersionRead[index] = 0;
							else break;
						}

						get_amulet_message(L_UPDATE_TO_REV, formatstring);    // "Update to Rev: %s, Proceed?"

						sprintf(message, formatstring, &(m_acUpdateVersionRead[7]));
						send_to_amulet_string(98, message);
					}else{
						send_amulet_message(L_UPDATE_DISABLES_USB, 98);    // "This version will disable USB Update. Proceed?"
					}

					send_amulet_message(L_YES, 102);    // "Yes"
					send_amulet_message(L_NO, 106);    // "No"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_OVERWRITE_WITH_DEFAULT:
					switch(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID){
						case 2:
							get_amulet_message(L_WORK_AREA, wipetype);    // "Work Area"
							break;

						case 3:
							get_amulet_message(L_UNRESTRICTED_AREA, wipetype);    // "Unrestricted Area"
							break;

						case 4:
							get_amulet_message(L_SEALED_SOURCE, wipetype);    // "Sealed Source"
							break;

						case 5:
							get_amulet_message(L_PACKAGE, wipetype);    // "Package"
							break;
					}
					get_amulet_message(L_OVERWRITE_SETTING_WITH_DEFAULT, formatstring);    // "Overwrite settings with defaults from %s?"
					sprintf(message, formatstring, wipetype);
					send_to_amulet_string(98, message);
					send_amulet_message(L_YES_ENGLISH, 102);    // "Yes"
					send_amulet_message(L_NO_ENGLISH, 106);    // "No"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_DATE_FORMAT:
					send_amulet_message(L_SELECT_DATE_FORMAT, 98);    // "Please Select Date Format"
					if(current.language == ENGLISH){
						send_to_amulet_string(101, "mm/dd/yyyy");
						send_to_amulet_string(102, "dd/mm/yyyy");
						send_to_amulet_string(103, "yyyy/mm/dd");
					}else if(current.language == FRENCH){
						send_to_amulet_string(101, "mm/dd/aaaa");
						send_to_amulet_string(102, "dd/mm/aaaa");
						send_to_amulet_string(103, "aaaa/mm/dd");
					}else{
						send_to_amulet_string(101, "mm/dd/yyyy");
						send_to_amulet_string(102, "dd/mm/yyyy");
						send_to_amulet_string(103, "yyyy/mm/dd");
					}
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(101, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(103, 0xFF);
					break;

				case CONFIG_USB_DRIVER:
					send_amulet_message(L_SELECT_USB_DRIVER, 98);    // "Please Select USB PC Driver"
					send_amulet_message(L_LEGACY, 102);    // "Legacy"
					send_amulet_message(L_CDC, 106);    // "CDC"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_FUTURE_DATE_ENTRY:
					send_amulet_message(L_SELECT_DOSE_DECAY_ENTRY_MODE, 98);    // "Please Select Dose Decay Entry Mode"
					send_amulet_message(L_QUICK, 102);    // "Quick"
					send_amulet_message(L_FULL, 106);    // "Full"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;

				case CONFIG_LANGUAGE:
					send_amulet_message(L_SELECT_LANGUAGE, 98);    // "Please Select Language"
					send_amulet_message(L_ENGLISH, 102);    // "English"
					send_amulet_message(L_FRENCH, 106);    // "French"
					SetAmuletByte(100, 0xFF);
					SetAmuletByte(102, 0xFF);
					SetAmuletByte(106, 0xFF);
					break;
			}
			m_iPhase = PHASE_GENERICITEMS_WAIT;
			break;

		case PHASE_GENERICITEMS_WAIT:
			break;

		case PHASE_GENERICITEMS_BUTTON1:
		case PHASE_GENERICITEMS_BUTTON2:
		case PHASE_GENERICITEMS_BUTTON3:
		case PHASE_GENERICITEMS_BUTTON4:
		case PHASE_GENERICITEMS_BUTTON5:
		case PHASE_GENERICITEMS_BUTTON6:
		case PHASE_GENERICITEMS_BUTTON7:
		case PHASE_GENERICITEMS_BUTTON8:
			beep_amulet();
			switch(AmuletGenericItems_config){
				case CONFIG_WIPESEARCH_WIPETYPE:
					AmuletWellWipeSearch_wipeType = m_iPhase - 1;
					break;

				case CONFIG_DETECTOR700:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						newDetector = DET_PROBE700;
					}else{
						newDetector = DET_WELL700;
					}

					if(newDetector != Mca_installedDetector){
						if(PreviousPageStack() == AmuletHTMLIndex[WELLMEASUREMENT_HTM]) m_ucClear = 18;
						Mca_switchDetector(newDetector);
					}
					break;

				case CONFIG_BRANDING:
					if(current.branding != 3){
						if(m_iPhase == PHASE_GENERICITEMS_BUTTON1){
							current.branding = 0;
							strcpy(pr_crcname, "CRC-55t");
							get_amulet_message(L_55T_FULL_TITLE, pr_crcname_large);    // "CRC-55t      RADIOISOTOPE DOSE CALIBRATOR"
						}else if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
							current.branding = 1;
							strcpy(pr_crcname, "CAPRAC-t");
							get_amulet_message(L_CAPRAC_FULL_TITLE, pr_crcname_large);    // "CAPRAC-t        RADIOISOTOPE WELL COUNTER"
						}else if(m_iPhase == PHASE_GENERICITEMS_BUTTON3){
							current.branding = 2;
							strcpy(pr_crcname, "CAPTUS-700t");
							get_amulet_message(L_700T_FULL_TITLE, pr_crcname_large);    // "CAPTUS-700t          RADIOISOTOPE COUNTER"
						}

						DB_WriteBranding(current.branding);
					}else{
						if(m_iPhase == PHASE_GENERICITEMS_BUTTON1){
							DB_WriteBranding(0);
						}else if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
							DB_WriteBranding(1);
						}else if(m_iPhase == PHASE_GENERICITEMS_BUTTON3){
							DB_WriteBranding(2);
						}
					}
					break;

				case CONFIG_AUTOCALSEARCH_DEVTYPE:
					AmuletWellAutoCalSearch_device = m_iPhase - PHASE_GENERICITEMS_BUTTON1;
					break;

				case CONFIG_SYSTEMTESTSEARCH_DEVTYPE:
					AmuletWellSystemTestSearch_device = m_iPhase - PHASE_GENERICITEMS_BUTTON1;
					break;

				case CONFIG_MDASEARCH_DEVTYPE:
					AmuletWellMDASearch_device = m_iPhase - PHASE_GENERICITEMS_BUTTON1;
					break;

				case CONFIG_CHISQUARE_DEVTYPE:
					AmuletWellChiSearch_device = m_iPhase - PHASE_GENERICITEMS_BUTTON1;
					break;

				case CONFIG_TU_COUNTING_METHOD:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2) AmuletAddEditThyroidUptakeProtocol_protocol.DoseDecayCorrect = TRUE;
					else if(m_iPhase == PHASE_GENERICITEMS_BUTTON6) AmuletAddEditThyroidUptakeProtocol_protocol.DoseDecayCorrect = FALSE;
					break;

				case CONFIG_TU_DOSE_MEASUREMENT:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2) AmuletAddEditThyroidUptakeProtocol_protocol.DoseMeasurementMethod = 1;
					else if(m_iPhase == PHASE_GENERICITEMS_BUTTON3) AmuletAddEditThyroidUptakeProtocol_protocol.DoseMeasurementMethod = 2;
					else if(m_iPhase == PHASE_GENERICITEMS_BUTTON6) AmuletAddEditThyroidUptakeProtocol_protocol.DoseMeasurementMethod = 3;
					else if(m_iPhase == PHASE_GENERICITEMS_BUTTON7) AmuletAddEditThyroidUptakeProtocol_protocol.DoseMeasurementMethod = 4;
					//else if(m_iPhase == PHASE_GENERICITEMS_BUTTON8) AmuletAddEditThyroidUptakeProtocol_protocol.DoseMeasurementMethod = 5;
					break;

				case CONFIG_TU_ISOTOPE:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2) AmuletAddEditThyroidUptakeProtocol_protocol.NuclideID = 40;
					else if(m_iPhase == PHASE_GENERICITEMS_BUTTON6) AmuletAddEditThyroidUptakeProtocol_protocol.NuclideID = 45;
					else if(m_iPhase == PHASE_GENERICITEMS_BUTTON7) AmuletAddEditThyroidUptakeProtocol_protocol.NuclideID = 84;
					break;

				case CONFIG_TU_DOSE_FORM:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2) AmuletAddEditThyroidUptakeProtocol_protocol.DoseForm = 1;
					else if(m_iPhase == PHASE_GENERICITEMS_BUTTON6) AmuletAddEditThyroidUptakeProtocol_protocol.DoseForm = 2;
					break;

				case CONFIG_TU_PRE_DOSE:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2) AmuletAddEditThyroidUptakeProtocol_protocol.PreDoseMeasurement = TRUE;
					else if(m_iPhase == PHASE_GENERICITEMS_BUTTON6) AmuletAddEditThyroidUptakeProtocol_protocol.PreDoseMeasurement = FALSE;
					break;

				case CONFIG_TU_DELETE_TEST:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						DB_DeleteProbeThyroidUptakeTest(&AmuletAddEditThyroidUptakeTest_test);
						PopPageStack();
						SetAmuletBackHTML();
						return;
					}
					break;

				case CONFIG_TU_RESIDUAL:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2) AmuletAddEditThyroidUptakeProtocol_protocol.ResidualMeasurement = TRUE;
					else if(m_iPhase == PHASE_GENERICITEMS_BUTTON6) AmuletAddEditThyroidUptakeProtocol_protocol.ResidualMeasurement = FALSE;
					break;

				case CONFIG_TU_REACTIVATE:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						AmuletWellThyroidUptake_clearSearch();
						DB_ReactivateProbeThyroidUptakeTest(&AmuletWellThyroidUptakeTest);
						PopPageStack();
						SetAmuletBackHTML();
						return;
					}
					break;

				case CONFIG_LOCK_NUCLIDE_KEV:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						DB_SetDefaultEnergy();
						DB_WriteDefaultKEV(1);
					}else{
						DB_WriteDefaultKEV(0);
					}
					DB_shutdown();
					get_amulet_message(L_FACTORY, titlestring);    // "Factory"
					get_amulet_message(L_PLEASE_RESTART, formatstring);    // "Setting has been saved. Please restart."
					Amulet_DisplayNotification(titlestring, formatstring, FALSE);
					return;

				case CONFIG_RBC_SURVIVAL_REACTIVATE:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						AmuletWellRBCSurvival_clearSearch();
						DB_ReactivateWellRBCSurvivalTest(&AmuletWellRBCSurvivalTest);
						PopPageStack();
						SetAmuletBackHTML();
						return;
					}
					break;

				case CONFIG_RBC_SURVIVAL_DELETE_TEST:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						DB_DeleteRBCSurvivalTest(&AmuletAddEditRBCSurvivalTest_test);
						PopPageStack();
						SetAmuletBackHTML();
						return;
					}
					break;

				case CONFIG_CONFIRM_UPDATE:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						for(index=0; index<20; index++) m_acUpdateVersionOK[index] = m_acUpdateVersionRead[index];
					}
					break;

				case CONFIG_OVERWRITE_WITH_DEFAULT:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						wipeType = WipesCurrentType(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID);
						AmuletWellSetupAddEditLocation_wellWipeLocation.CountTime = wipeType->CountTime;
						for(index=0; index<10; index++) AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[index] = wipeType->NuclideID[index];
						AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold = wipeType->Threshold;
						AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeID = wipeType->WellWipeTypeID;
						strcpy(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeName, wipeType->Name);
						PopPageStack();
						get_amulet_message(L_WIPE_TYPE_DEFAULTS, titlestring);    // "Wipe Type Defaults"
						get_amulet_message(L_WIPE_OVERWRITE_STRING, formatstring);    // "Current settings have been overwritten with %s default values"
						sprintf(message, formatstring, wipetype);
						Amulet_DisplayNotification(titlestring, message, TRUE);
						return;
					}
					break;

				case CONFIG_DATE_FORMAT:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON1){
						current.time_format = 0;
					}else if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						current.time_format = 1;
					}else if(m_iPhase == PHASE_GENERICITEMS_BUTTON3){
						current.time_format = 2;
					}else{
						current.time_format = 0;
					}
					DB_WriteTimeFormat(current.time_format);
					break;

				case CONFIG_USB_DRIVER:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						if(current.usb_device_protocol != 0){
							current.usb_device_protocol = 0;
							startup_811();
							DB_WriteUSBDeviceProtocol(current.usb_device_protocol);
						}
					}else if(m_iPhase == PHASE_GENERICITEMS_BUTTON6){
						if(current.usb_device_protocol !=1){
							current.usb_device_protocol = 1;
							startup_811();
							DB_WriteUSBDeviceProtocol(current.usb_device_protocol);
						}
					}
					break;

				case CONFIG_FUTURE_DATE_ENTRY:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						current.future_date_input = 0;
						DB_WriteFutureDateInput(current.future_date_input);
					}else if(m_iPhase == PHASE_GENERICITEMS_BUTTON6){
						current.future_date_input = 1;
						DB_WriteFutureDateInput(current.future_date_input);
					}
					break;

				case CONFIG_LANGUAGE:
					if(m_iPhase == PHASE_GENERICITEMS_BUTTON2){
						current.language = 0;
						DB_WriteLanguage(current.language);
					}else if(m_iPhase == PHASE_GENERICITEMS_BUTTON6){
						current.language = 1;
						DB_WriteLanguage(current.language);
					}

					//set study type and calicheck tube and time unit for current language
					set_study_type();
					set_calicheck_tube();
					set_time_unit();
					if(current.branding == 0) get_amulet_message(L_55T_FULL_TITLE, pr_crcname_large);    // "CRC-55t      RADIOISOTOPE DOSE CALIBRATOR"
					else if(current.branding == 1) get_amulet_message(L_CAPRAC_FULL_TITLE, pr_crcname_large);    // "CAPRAC-t        RADIOISOTOPE WELL COUNTER"
					else if(current.branding == 2) get_amulet_message(L_700T_FULL_TITLE, pr_crcname_large);    // "CAPTUS-700t          RADIOISOTOPE COUNTER"
					else if(current.branding == 3) get_amulet_message(L_77T_FULL_TITLE, pr_crcname_large);    // "CRC-77t      RADIOISOTOPE DOSE CALIBRATOR"
					else strcpy(pr_crcname_large, "");
					break;
			}
			AmuletGenericItems_config = 0;
			SetAmuletBackHTML();
			return;
	}
}

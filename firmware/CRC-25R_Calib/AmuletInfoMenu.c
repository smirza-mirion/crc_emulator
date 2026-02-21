/**
 * \file
 * \details This file handles calls from the Amulet Utility Screen
 */
#define PHASE_INFO_PRE_INIT 0
#define PHASE_INFO_WAIT 1
#define PHASE_INFO_DIAGNOSTICS 2
#define PHASE_INFO_RUN_DIAGNOSTICS 3
#define PHASE_INFO_UPDATE 4
#define PHASE_INFO_UPDATE_ERROR 5

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "printer.h"
#include "i2c.h"
#include "ff.h"
#include "mca.h"
#include "database.h"
#include "Amulet.h"
#include "message.h"

typedef struct updateinfo UPDATEINFO;
struct updateinfo{
	char Command[2];
	char Path[30];
	char FileName[30];
};

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern uchar m_ucEnumerated;
extern uchar AmuletGenericItems_config;
//extern volatile bool watchdogswitch;

bool AmuletInfoMenu_DoseTable;

// Ci Bq Conversion
float m_fInfo_InputUnitlessActivity;
unsigned char m_ucInfo_InputUnit;

// Nuclide Decay
unsigned char m_ucInfo_DecayNuclide;
time_t m_dtmInfo_FromTime;
float m_fInfo_FromActivity;
unsigned char m_ucInfo_FromUnit;
time_t m_dtmInfo_ToTime;
float m_fInfo_ToActivity;
unsigned char m_ucInfo_ToUnit;

// Diagnostics
char m_acInfo_Diagnostics1[26];
char m_acInfo_Diagnostics2[26];
char m_cInfo_DiagnosticsFailed;

// Update
char m_acUpdateVersionRead[21];
char m_acUpdateVersionOK[21];

static char longFileName[40], SourcePath[40], DestPath[60], lastFileName[40], test[100], files[100][21];
static UPDATEINFO updateinfo[20];
static FILINFO fileInfo;
static FF_DIR dirObject;
static FRESULT Result;
static FIL SourceFileObject, DestFileObject;
static unsigned char *buffer, checksumnum;
static UINT BytesRead, BytesWritten;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void GetExtendedNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
void GetExtendedTimeInfoLanguageSec(time_t *dtmDateTime, char *acMsg);
void GetExtendedActivityInfoKBq(float fActivity, unsigned char ucUnit, char *acMsg);
short format_activity_kunit_kbq(float act, short syst, char *actstr);
float nucdecay(float act0, time_t time0,time_t time1, float hl, short unit);
time_t nucdecaytime(float fActivity0, float fActivity, time_t dtmTime0, float fHalflife, short iUnit);
bool amulet_print_diagnostic_data(void);
void remote_busy(bool flag);
void busy_sent(void);
void set_pc_freeze_flag(bool flag);
void MoveFileName(char *fname, char *lfname);
unsigned long int TransposeLongInt(unsigned char *start);
void toupper(char *acByte);
void PushPageStack(unsigned char ucPage);
//void disable_all_interrupts(void);
//void set_touch_reset(bool on);
void send_to_amulet_string(uchar ucIndex, char message0[]);
	
bool CompareArray(char *string1, char *string2, int size){
	bool returnvalue;
	int index;

	returnvalue = 0;
	for(index=0; index<size; index++){
		if((*string1) != (*string2)){
			returnvalue = 1;
			break;
		}
	}

	return returnvalue;
}

/**
 * \details Handles the Amulet Screen Info.htm. Info.htm is the utility screen.
 * \param Amulet_Byte_ID Description
 * \param 90 Login
 * \param 91 Update
 * \param 92 UpdateProgress
 * \param 93 Language (STATE) English = 0, French = 1
 * \param 99 DoseTable Active
 * \param 100 Display
 * \param 101 ToTime_Normal
 * \param 102 ToTime_Highlight
 * \param 103 ToActivity_Normal
 * \param 104 ToActivity_Highlight
 * \param 105 Display Diagnostic
 * \param 106 Display Diagnostics2 in Normal
 * \param 107 Display Diagnostics2 in Red
 * \param 108 Show All Buttons
 * \param 109 Hide All Buttons
 * \param 110 Show Update Message
 * \param 111 Show Update Progress
 * \param Amulet_String_ID Description
 * \param 80_81 Update Line1
 * \param 82_83 Update Line2
 * \param 84_85 Update Software
 * \param 100 Convert Activity Ci<->Bq
 * \param 101 Converted Activity
 * \param 102_103 Decay Nuclide
 * \param 104 From Time
 * \param 105 From Activity
 * \param 107 To Activity
 * \param 108 Diagnostic Line1
 * \param 109 Diagnostic Line2
 * \param 110 Serial Number
 * \param 115,116 To Time
 * \param 120 Title
 * \param 121_122 Ci,Bq Conv  Input Activity
 * \param 123_124 Decay Calculator
 * \param 125 Nuclide
 * \param 126 From
 * \param 127 To
 * \param 128 Diagnostics
 * \param 129 Dose Table
 * \param 130_131 Please Enter Activity
 * \param 132_133 Please Select Nuclide
 * \param 134_135 Please Enter Start Time
 * \param 136_137 Please Enter end time
 * \param 138 Reports
 * \returns None
 */
void AmuletInfo_menu(void){
	char sys0,sys1;
	float act0,act1;
	char acMsg[50], acMsg2[50], numstring[30];
	NUCDATA nuc;
	double dActivity0, dActivity, fProgress;
	char acToTimeMsg[50], acToActivityMsg[50];
	unsigned char ucToTimeHighlight, ucToActivityHighlight, ucProgress;
	time_t dtmCalcToTime;
	char serialnum[11];
	bool printerok;
	USER user;
	unsigned long int filecount, lastFileSize, relativeJump, bufferSize, readSize, copyFileSize, currentLoc, base, signatureLoc;
	int updateinfo_index, index, jndex;
	char message[100], formatstring[100];

	switch(m_iPhase){
		case PHASE_INFO_PRE_INIT:
			if (m_ucClear == 4){
				m_fInfo_InputUnitlessActivity = -999;
				m_ucInfo_InputUnit = 0xFF;
				m_ucInfo_DecayNuclide = 0xFF;
				m_dtmInfo_FromTime = 0;
				m_fInfo_FromActivity = -999;
				m_ucInfo_FromUnit = 0xFF;
				m_dtmInfo_ToTime = 0;
				m_fInfo_ToActivity = -999;
				m_ucInfo_ToUnit = 0xFF;
				m_acInfo_Diagnostics1[0] = 0;
				m_acInfo_Diagnostics2[0] = 0;
				m_cInfo_DiagnosticsFailed = 0;
				strcpy(m_acUpdateVersionRead, "Winston          Lam");
				strcpy(m_acUpdateVersionOK, "Lam          Winston");
				m_ucClear = 0;

				/*f_chdir("/images");
				fileInfo.lfname = longFileName;
				fileInfo.lfsize = 100;
				if(f_stat("progname.txt", &fileInfo) == FR_OK) f_unlink("progname.txt");

				f_open(&fileObject2, "progname.txt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
				strcpy(progname, "flash.mem");
				f_write(&fileObject2, progname, strlen(progname), &bytesWritten);
				f_close(&fileObject2);

				set_touch_reset(FALSE);

				disable_all_interrupts();
				for(;;); */
			}

			if(current.security_mode == 1){
				DB_GetUserFromID(current.user_id, &user);
				send_to_amulet_string(90, user.UserName);
				SetAmuletByte(90, 0xFF);
			}else SetAmuletByte(90, 0x00);

			if(AmuletInfoMenu_DoseTable) SetAmuletByte(99, 0xFF);
			else SetAmuletByte(99, 0x00);

			SetAmuletByte(93,current.language);
			

			send_amulet_message(L_UTILITY,120);    // "Utility"
			send_amulet_message(L_CONV_INPUT_ACTIVITY,121);    // "[Ci,Bq Conv] Input Activity:"
			send_amulet_message(L_DECAY_CALCULATOR,123);    // "[Decay Calculator]"

			get_amulet_message(L_NUCLIDE,message);    // "Nuclide"
			strcat(message,":");
			send_to_amulet_string(125,message);

			send_amulet_message(L_FROM,126);    // "FROM:"
			send_amulet_message(L_TO,127);    // "TO:"

			send_amulet_message(L_DIAGNOSTICS,128);    // "Diagnostics"
			send_amulet_message(L_DOSE_TABLE,129);    // "Dose Table"

			send_amulet_message(L_PLEASE_ENTER_ACTIVITY,130);    // "Please Enter Activity:"
			send_amulet_message(L_PLEASE_SELECT_NUCLIDE,132);    // "Please Select Nuclide"
			send_amulet_message(L_PLEASE_ENTER_START_TIME,134);    // "Please Enter Start Time:"
			send_amulet_message(L_PLEASE_ENTER_END_TIME,136);    // "Please Enter End Time:"
			send_amulet_message(L_UPDATE_SOFTWARE, 84);    // "Update Software"
			send_amulet_message(L_REPORTS, 138);    // "Reports"

			if(m_fInfo_InputUnitlessActivity == -999){
				send_to_amulet_string(100, "");
				send_to_amulet_string(101, "");
				//SetAmuletByte(100, 0xFF);
			}else{
				act0 = m_fInfo_InputUnitlessActivity;
				switch(m_ucInfo_InputUnit){
					case 0:
					case 1:
					case 2:
						sys0 = CI;
						sys1 = BQ;
						break;
					case 3:
					case 4:
					case 5:
						sys0 = BQ;
						sys1 = CI;
						break;
				}
				format_activity_kunit_kbq(act0, sys0, acMsg);
				strcat(acMsg, " ");
				send_to_amulet_string(100, acMsg);
				if(sys0==CI) act1 = act0 * BQFACTOR;
				else act1 = act0 / BQFACTOR;
				format_activity_kunit_kbq(act1, sys1, acMsg2);
				strcpy(acMsg, "=");
				strcat(acMsg, acMsg2);
				send_to_amulet_string(101, acMsg);
			}

			if(m_ucInfo_DecayNuclide == 0xFF){
				acMsg[0] = 0;
			}else{
				GetExtendedNuclideInfo(m_ucInfo_DecayNuclide, &nuc, acMsg);
			}
			//SetAmuletString(102, acMsg);
			send_to_amulet_string(102, acMsg);
			
			if(m_dtmInfo_FromTime == 0){
				acMsg[0] = 0;
			}else{
				GetExtendedTimeInfoLanguageSec(&m_dtmInfo_FromTime, acMsg);
			}
			send_to_amulet_string(104, acMsg);

			if(m_fInfo_FromActivity == -999){
				acMsg[0] = 0;
			}else{
				GetExtendedActivityInfoKBq(m_fInfo_FromActivity, m_ucInfo_FromUnit, acMsg);
			}
			send_to_amulet_string(105, acMsg);

			if(m_dtmInfo_ToTime == 0){
				acToTimeMsg[0] = 0;
			}else{
				GetExtendedTimeInfoLanguageSec(&m_dtmInfo_ToTime, acToTimeMsg);
			}

			if(m_fInfo_ToActivity == -999){
				acToActivityMsg[0] = 0;
			}else{
				GetExtendedActivityInfoKBq(m_fInfo_ToActivity, m_ucInfo_ToUnit, acToActivityMsg);
			}

			ucToTimeHighlight = FALSE;
			ucToActivityHighlight = FALSE;

			if((m_ucInfo_DecayNuclide != 0xFF) && (m_dtmInfo_FromTime != 0) && (m_fInfo_FromActivity != -999)){
				if((m_dtmInfo_ToTime != 0) && (m_fInfo_ToActivity == -999)){
					GetExtendedActivityInfoKBq(nucdecay(m_fInfo_FromActivity, m_dtmInfo_FromTime, m_dtmInfo_ToTime, nuc.halflife, nuc.hlunit), m_ucInfo_FromUnit, acToActivityMsg);
					if (strlen(acToActivityMsg)>11)
						//strcpy(acToActivityMsg, "TOO LARGE");
						get_amulet_message(L_TOO_LARGE,acToActivityMsg);    // "TOO LARGE"
					ucToActivityHighlight = TRUE;
				}else if((m_dtmInfo_ToTime == 0) && (m_fInfo_ToActivity != -999)){
					// Change all Activity to Curies;
					dActivity0 = m_fInfo_FromActivity;
					if(m_ucInfo_FromUnit > 2) dActivity0 /= BQFACTOR;

					dActivity = m_fInfo_ToActivity;
					if(m_ucInfo_ToUnit > 2) dActivity /= BQFACTOR;

					dtmCalcToTime = nucdecaytime(dActivity0, dActivity, m_dtmInfo_FromTime, nuc.halflife, nuc.hlunit);
					if(dtmCalcToTime == 1)
						//strcpy(acToTimeMsg, "TOO FAR INTO FUTURE");
						get_amulet_message(L_TOO_FAR_INTO_FUTURE,acToTimeMsg);    // "TOO FAR INTO FUTURE"
					else if(dtmCalcToTime == 0)
						//strcpy(acToTimeMsg, "TOO FAR INTO PAST");
						get_amulet_message(L_TOO_FAR_INTO_PAST,acToTimeMsg);    // "TOO FAR INTO PAST"
					else GetExtendedTimeInfoLanguageSec(&dtmCalcToTime, acToTimeMsg);
					ucToTimeHighlight = TRUE;
				}
			}

			send_to_amulet_string(115, acToTimeMsg);
			send_to_amulet_string(107, acToActivityMsg);

			if(ucToTimeHighlight == TRUE) SetAmuletByte(102, 0xFF);
			else SetAmuletByte(101, 0xFF);

			if(ucToActivityHighlight == TRUE) SetAmuletByte(104, 0xFF);
			else SetAmuletByte(103, 0xFF);

			ReadSN(serialnum);
			serialnum[10] = 0;
			send_to_amulet_string(110, serialnum);

			if(m_ucEnumerated == 2) SetAmuletByte(91, 0xFF);
			send_to_amulet_string(80, "");
			send_to_amulet_string(82, "");

			send_amulet_message(L_UTILITY,120);    // "Utility"

			if(CompareArray(m_acUpdateVersionRead, m_acUpdateVersionOK, 20) != 0){
				SetAmuletByte(100, 0xFF);

				send_to_amulet_string(108, m_acInfo_Diagnostics1);
				SetAmuletByte(105, 0xFF);

				send_to_amulet_string(109, m_acInfo_Diagnostics2);
				if(m_cInfo_DiagnosticsFailed == 0) SetAmuletByte(106, 0xFF);
				else SetAmuletByte(107, 0xFF);

				m_iPhase = PHASE_INFO_WAIT;
			}else{
				m_iPhase = PHASE_INFO_UPDATE;
			}
			break;

		case PHASE_INFO_WAIT:
			break;

		case PHASE_INFO_DIAGNOSTICS:
			beep_amulet();
			SetAmuletByte(109, 0xFF);
			m_iPhase = PHASE_INFO_RUN_DIAGNOSTICS;
			break;

		case PHASE_INFO_RUN_DIAGNOSTICS:
			//watchdogswitch = FALSE;

			//beep_amulet();
			remote_busy(TRUE);
			busy_sent();
			set_pc_freeze_flag(TRUE);
			printerok = amulet_print_diagnostic_data();
			remote_busy(FALSE);
			busy_sent();
			set_pc_freeze_flag(FALSE);

			if(printerok){
				send_to_amulet_string(108, m_acInfo_Diagnostics1);
				SetAmuletByte(105, 0xFF);

				send_to_amulet_string(109, m_acInfo_Diagnostics2);
				if(m_cInfo_DiagnosticsFailed == 0) SetAmuletByte(106, 0xFF);
				else SetAmuletByte(107, 0xFF);

				SetAmuletByte(108, 0xFF);
				m_iPhase = PHASE_INFO_WAIT;
			}else{
				return;
			}
			break;

		case PHASE_INFO_UPDATE:
			beep_amulet();
			SetAmuletByte(109, 0xFF);
			send_to_amulet_string(80, "");
			send_to_amulet_string(82, "");
			SetAmuletByte(110, 0xFF);
			delayloop(50);
			if(m_ucEnumerated != 2){
				//SetAmuletByte(91, 0x00);
				//SetAmuletString(82, "Missing USB Drive");
				send_amulet_message(L_MISSING_USB_DRIVE, 82);    // "Missing USB Drive"
				SetAmuletByte(110, 0xFF);
				SetAmuletByte(108, 0xFF);
				m_iPhase = PHASE_INFO_WAIT;
				return;
			}else{
				fileInfo.lfname = longFileName;
				fileInfo.lfsize = 40;

				Result = f_chdrive(1);
				if(Result){
					m_iPhase = PHASE_INFO_UPDATE_ERROR;
					return;
				}

				Result = f_chdir("/");
				if(Result){
					m_iPhase = PHASE_INFO_UPDATE_ERROR;
					return;
				}

				Result = f_opendir(&dirObject, ".");
				if(Result){
					m_iPhase = PHASE_INFO_UPDATE_ERROR;
					return;
				}

				filecount = 0;
				do{
					longFileName[0] = 0;
					Result = f_readdir(&dirObject, &fileInfo);
					if(Result){
						m_iPhase = PHASE_INFO_UPDATE_ERROR;
						return;
					}
					MoveFileName(fileInfo.fname, longFileName);
					if((longFileName[0] != 0) && !(fileInfo.fattrib & 0x1e)){
						filecount++;
						strcpy(lastFileName, longFileName);
						lastFileSize = fileInfo.fsize;
					}

				}while(longFileName[0] != 0);

				if(filecount == 0){
					//SetAmuletString(82, "Unable to find file");
					send_amulet_message(L_UNABLE_TO_FIND_FILE, 82);    // "Unable to find file"
					SetAmuletByte(110, 0xFF);
					SetAmuletByte(108, 0xFF);
					m_iPhase = PHASE_INFO_WAIT;
					return;
				}else if(filecount > 1){
					//SetAmuletString(80, "Place only one update");
					send_amulet_message(L_PLACE_ONLY_ONE_UPDATE, 80);    // "Place only one update"
					//SetAmuletString(82, "file in root folder");
					send_amulet_message(L_FILE_IN_ROOT_FOLDER, 82);    // "file in root folder"
					SetAmuletByte(110, 0xFF);
					SetAmuletByte(108, 0xFF);
					m_iPhase = PHASE_INFO_WAIT;
					return;
				}else{
					// Open file
					strcpy(SourcePath, "1:");
					strcat(SourcePath, lastFileName);
					Result = f_open(&SourceFileObject, SourcePath, FA_OPEN_EXISTING | FA_READ);
					if(Result){
						f_close(&SourceFileObject);
						m_iPhase = PHASE_INFO_UPDATE_ERROR;
						return;
					}
					if(lastFileSize >= 4){
						Result = f_lseek(&SourceFileObject, lastFileSize - 4);
						if(Result){
							f_close(&SourceFileObject);
							m_iPhase = PHASE_INFO_UPDATE_ERROR;
							return;
						}

						Result = f_read(&SourceFileObject, &relativeJump, 4, &BytesRead);
						if(Result || BytesRead == 0){
							f_close(&SourceFileObject);
							m_iPhase = PHASE_INFO_UPDATE_ERROR;
							return;
						}

						relativeJump = TransposeLongInt((unsigned char *) &relativeJump);

						if(lastFileSize > relativeJump){
							currentLoc = lastFileSize - relativeJump;
							if(currentLoc > 20){
								signatureLoc = currentLoc - 20; // New
							}else{
								currentLoc = 0;
							}
						}else{
							currentLoc = 0;
						}

						if(currentLoc > 0){
							checksumnum = 0;
							updateinfo_index = 0;

							base = currentLoc;
							copyFileSize = lastFileSize - base;
							Result = f_lseek(&SourceFileObject, signatureLoc);
							if(Result){
								f_close(&SourceFileObject);
								m_iPhase = PHASE_INFO_UPDATE_ERROR;
								return;
							}

							m_acUpdateVersionRead[20] = 0;
							Result = f_read(&SourceFileObject, m_acUpdateVersionRead, 20, &BytesRead);
							if(Result || BytesRead == 0){
								f_close(&SourceFileObject);
								m_iPhase = PHASE_INFO_UPDATE_ERROR;
								return;
							}
							if(CompareArray(m_acUpdateVersionRead, m_acUpdateVersionOK, 20) != 0){
								f_close(&SourceFileObject);
								AmuletGenericItems_config = 19;
								SetAmuletHTML(AmuletHTMLIndex[GENERICITEMS_HTM]);
								PushPageStack(AmuletHTMLIndex[GENERICITEMS_HTM]);
								return;
							}
							strcpy(m_acUpdateVersionOK, "Lam          Winston");
							SetAmuletByte(92, 0x00);
							SetAmuletByte(111, 0xFF);

							// Malloc
							bufferSize = 16 * 1024;
							buffer = malloc(bufferSize);

							do{
								// Read test buffer
								Result = f_read(&SourceFileObject, test, 100, &BytesRead);
								if(Result || BytesRead == 0){
									free(buffer);
									f_close(&SourceFileObject);
									m_iPhase = PHASE_INFO_UPDATE_ERROR;
									return;
								}

								//if((test[0]== 'O') || (test[0] == 'R')){ //Testing
								if((test[0]!= 'O') && (test[0] != 'R')){
									free(buffer);
									f_close(&SourceFileObject);
									send_to_amulet_string(80, "");
									//SetAmuletString(82, "Invalid update file");
									send_amulet_message(L_INVALID_UPDATE_FILE, 82);    // "Invalid update file"
									SetAmuletByte(110, 0xFF);
									SetAmuletByte(108, 0xFF);
									m_iPhase = PHASE_INFO_WAIT;
									return;
								}

								updateinfo[updateinfo_index].Command[0] = test[0];
								updateinfo[updateinfo_index].Command[1] = 0;

								strcpy(DestPath, &(test[1]));

								for(index=0; index < strlen(DestPath) + 6; index++) checksumnum += (unsigned char) test[index];
								currentLoc += (strlen(DestPath) + 6);
								relativeJump = TransposeLongInt((unsigned char *) &(test[strlen(DestPath) + 2]));

								for(index=strlen(DestPath) - 1; index >= 0; index--){
									if(DestPath[index] == '\\'){
										DestPath[index] = 0;
										break;
									}
								}
								strcpy(updateinfo[updateinfo_index].Path, DestPath);
								strcpy(updateinfo[updateinfo_index].FileName, &(DestPath[index+1]));

								Result = f_lseek(&SourceFileObject, currentLoc);
								if(Result){
									free(buffer);
									f_close(&SourceFileObject);
									m_iPhase = PHASE_INFO_UPDATE_ERROR;
									return;
								}

								// Create Temp File
								strcpy(DestPath, "0:");
								strcat(DestPath, updateinfo[updateinfo_index].Path);
								strcat(DestPath, "/");
								strcat(DestPath, updateinfo[updateinfo_index].FileName);
								strcat(DestPath, ".temp");
								for(index=0; index<strlen(DestPath); index++) if(DestPath[index] == '\\') DestPath[index] = '/';
								Result = f_open(&DestFileObject, DestPath, FA_CREATE_ALWAYS | FA_WRITE);
								if(Result){
									free(buffer);
									f_close(&DestFileObject);
									f_close(&SourceFileObject);
									m_iPhase = PHASE_INFO_UPDATE_ERROR;
									return;
								}

								// Loop Copy From USB to Temp File
								while(relativeJump > 0){
									sprintf(numstring, "%lu", currentLoc - base);
									get_amulet_message(L_COPYING, formatstring);    // "Copying %s"
									//sprintf(test, "Copying %s", numstring);
									sprintf(test, formatstring, numstring);
									send_to_amulet_string(80, test);
									sprintf(numstring, "%lu", copyFileSize);
									get_amulet_message(L_OF2, formatstring);    // "of %s"
									//sprintf(test, "of %s", numstring);
									sprintf(test, formatstring, numstring);
									send_to_amulet_string(82, test);
									SetAmuletByte(110, 0xFF);
									fProgress = currentLoc - base;
									fProgress /= copyFileSize;
									fProgress *= 100;
									if(fProgress <= 0) ucProgress = 0;
									else if(fProgress >= 100) ucProgress = 100;
									else ucProgress = fProgress;
									SetAmuletByte(92, ucProgress);

									if(relativeJump > bufferSize) readSize = bufferSize;
									else readSize = relativeJump;

									Result = f_read(&SourceFileObject, buffer, readSize, &BytesRead);
									if(Result || BytesRead != readSize){
										free(buffer);
										f_close(&DestFileObject);
										f_close(&SourceFileObject);
										m_iPhase = PHASE_INFO_UPDATE_ERROR;
										return;
									}

									for(index=0; index<readSize; index++) checksumnum += buffer[index];

									Result = f_write(&DestFileObject, buffer, readSize, &BytesWritten);
									if(Result || BytesWritten != readSize){
										free(buffer);
										f_close(&DestFileObject);
										f_close(&SourceFileObject);
										m_iPhase = PHASE_INFO_UPDATE_ERROR;
										return;
									}

									relativeJump -= readSize;
									currentLoc += readSize;
								}

								// Close Temp File
								Result = f_close(&DestFileObject);
								if(Result){
									free(buffer);
									f_close(&SourceFileObject);
									m_iPhase = PHASE_INFO_UPDATE_ERROR;
									return;
								}

								updateinfo_index++;
							}while(currentLoc < lastFileSize - 5);

							// Free
							free(buffer);

							// Read and Compare Checksum
							Result = f_read(&SourceFileObject, test, 1, &BytesRead);
							if(Result || BytesRead == 0){
								f_close(&SourceFileObject);
								m_iPhase = PHASE_INFO_UPDATE_ERROR;
								return;
							}

							Result= f_close(&SourceFileObject);
							if(Result){
								m_iPhase = PHASE_INFO_UPDATE_ERROR;
								return;
							}

							//if(checksumnum != (unsigned char) test[0]){ // Testing
							if(checksumnum == (unsigned char) test[0]){ // IF(CHECKSUM == OK) Run Execution Command
								f_chdrive(0);
								for(index=0; index<updateinfo_index; index++){
									strcpy(DestPath, updateinfo[index].Path);
									for(jndex=0; jndex<strlen(DestPath); jndex++) if(DestPath[jndex] == '\\') DestPath[jndex] = '/';
									f_chdir(DestPath);

									strcpy(test, updateinfo[index].FileName);
									strcpy(DestPath, test);
									strcat(test, ".temp");

									if(updateinfo[index].Command[0] == 'R'){
										// Check Existence of temp file
										if(f_stat(test, &fileInfo) == FR_OK){
											// Remove Current File
											if(f_stat(DestPath, &fileInfo) == FR_OK) f_unlink(DestPath);

											// Rename Temp File to Current File
											Result = f_rename(test, DestPath);
										}
									}else if(updateinfo[index].Command[0] == 'O'){
										// Check Existence of temp file
										if(f_stat(test, &fileInfo) == FR_OK){
											// Remove All Files except for temp file
											toupper(test);
											for(jndex=0; jndex<100; jndex++) files[jndex][0] = 0;
											f_opendir(&dirObject, ".");
											jndex = 0;
											do{
												longFileName[0] = 0;
												f_readdir(&dirObject, &fileInfo);
												MoveFileName(fileInfo.fname, longFileName);
												toupper(longFileName);
												if((longFileName[0] != 0) && (strcmp(longFileName, ".")!= 0) && (strcmp(longFileName, "..") != 0) && (strlen(longFileName) < 21)){
													if(strcmp(longFileName, test) != 0){
														if(jndex < 100){
															strcpy(&(files[jndex][0]), longFileName);
															jndex++;
														}
													}
												}
											}while(longFileName[0] != 0);

											for(jndex=0; jndex<100; jndex++){
												if(files[jndex][0] != 0){
													f_unlink(&(files[jndex][0]));
												}
											}

											// Rename Temp File to Current file
											Result = f_rename(test, DestPath);
										}
									}
								}
							}else{ // ELSE Remove temp File Data from SD using Execution Command
								f_chdrive(0);
								for(index=0; index<updateinfo_index; index++){
									strcpy(DestPath, updateinfo[index].Path);
									for(jndex=0; jndex<strlen(DestPath); jndex++) if(DestPath[jndex] == '\\') DestPath[jndex] = '/';
									f_chdir(DestPath);

									strcpy(test, updateinfo[index].FileName);
									strcpy(DestPath, test);
									strcat(test, ".temp");
									if(f_stat(test, &fileInfo) == FR_OK) f_unlink(test);
								}
								send_to_amulet_string(80, "");
								//SetAmuletString(82, "Corrupt Update File");
								send_amulet_message(L_CORRUPT_UPDATE_FILE, 82);    // "Corrupt Update File"
								SetAmuletByte(110, 0xFF);
								SetAmuletByte(108, 0xFF);
								m_iPhase = PHASE_INFO_WAIT;
								return;
							}

							//SetAmuletString(80, "Finished Copying");
							send_amulet_message(L_FINISHED_COPYING, 80);    // "Finished Copying"
							//SetAmuletString(82, "Please restart");
							send_amulet_message(L_PLEASE_RESTART2, 82);    // "Please restart"
							SetAmuletByte(110, 0xFF);
							SetAmuletByte(92, 100);
							SetAmuletByte(111, 0xFF);
							m_iPhase = PHASE_INFO_WAIT;
							return;
						}else{
							f_close(&SourceFileObject);
							send_to_amulet_string(80, "");
							//SetAmuletString(82, "Invalid update file");
							send_amulet_message(L_INVALID_UPDATE_FILE, 82);    // "Invalid update file"
							SetAmuletByte(110, 0xFF);
							SetAmuletByte(108, 0xFF);
							m_iPhase = PHASE_INFO_WAIT;
							return;
						}
					}else{
						f_close(&SourceFileObject);
						send_to_amulet_string(80, "");
						//SetAmuletString(82, "Invalid update file");
						send_amulet_message(L_INVALID_UPDATE_FILE, 82);    // "Invalid update file"
						SetAmuletByte(110, 0xFF);
						SetAmuletByte(108, 0xFF);
						m_iPhase = PHASE_INFO_WAIT;
						return;
					}
				}
			}
			m_iPhase = PHASE_INFO_WAIT;
			break;

		case PHASE_INFO_UPDATE_ERROR:
			send_to_amulet_string(80, "");
			//SetAmuletString(82, "USB Drive Error\n");
			send_amulet_message(L_USB_DRIVE_ERROR, 82);    // "USB Drive Error\n"

			SetAmuletByte(110, 0xFF);
			SetAmuletByte(108, 0xFF);
			m_iPhase = PHASE_INFO_WAIT;
			break;
	}
}

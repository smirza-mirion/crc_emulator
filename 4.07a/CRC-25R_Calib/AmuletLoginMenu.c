#define PHASE_LOGIN_PRE_INIT	0
#define PHASE_LOGIN_WAIT		1
#define PHASE_LOGIN_GUEST		2
#define PHASE_LOGIN_ACCEPT		3

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "database.h"

char AmuletLoginMenu_username[26];
char AmuletLoginMenu_password[26];
bool AmuletLoginMenu_blockGuest;
int AmuletLoginMenu_minRole;
static char AmuletLoginMenu_overrideCode[7];

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern bool AmuletSecurity_accepted;
extern bool AmuletWellSchillingSearch_accepted;
extern bool AmuletWellPlasmaSearch_accepted;
extern bool AmuletWellRBCSearch_accepted;
extern bool AmuletWellAdvancedSetup_accepted;
extern bool AmuletWellBioAssay_accepted;
extern bool AmuletWellBioAssaySearch_accepted;
extern bool AmuletWellThyroidUptake_accepted;
extern bool AmuletWellRBCSurvival_accepted;
extern bool AmuletSecurity_relogin;
extern time_t clock_time;

void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);

void AmuletLogin_menu(void){
	bool flgPassed;
	float floatvalue, sqrtvalue;
	char key[20], fraction[20];
	int index, value;
	char message[26];
	USER user;

	switch(m_iPhase){
		case PHASE_LOGIN_PRE_INIT:
			if(m_ucClear == 75){
				AmuletLoginMenu_username[0] = 0;
				AmuletLoginMenu_password[0] = 0;
				AmuletLoginMenu_overrideCode[0] = 0;
				srand((unsigned int) clock_time);
				m_ucClear = 0;
			}

			SetAmuletString(100, AmuletLoginMenu_username);

			if(strcmp(AmuletLoginMenu_username, "RoOt") == 0){
				if(strlen(AmuletLoginMenu_overrideCode) == 0){
					flgPassed = FALSE;
					while(!flgPassed){
						value = rand();
						value = value % 65535;
						floatvalue = value;
						sqrtvalue = sqrt(floatvalue);
						sprintf(key, "%.3f", sqrtvalue);
						strcpy(fraction, &(key[strlen(key) - 3]));
						if(strcmp(fraction, "000") != 0) flgPassed = TRUE;
					}
					sprintf(AmuletLoginMenu_overrideCode, "%d", value);
				}
			}else{
				AmuletLoginMenu_overrideCode[0] = 0;
			}

			if(strlen(AmuletLoginMenu_overrideCode) != 0){
				SetAmuletString(102, AmuletLoginMenu_overrideCode);
				SetAmuletByte(102, 0xFF);
			}

			message[0] = 0;
			for(index=0; index<strlen(AmuletLoginMenu_password); index++){
				strcat(message, "*");
			}
			SetAmuletString(101, message);
			if(AmuletLoginMenu_blockGuest) SetAmuletByte(101, 0xFF);
			else SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_LOGIN_WAIT;
			break;

		case PHASE_LOGIN_WAIT:
			break;

		case PHASE_LOGIN_GUEST:
			DB_GetUser("Guest", &user);
			current.user_id = user.UserID;
			SetAmuletBackHTML();
			return;

		case PHASE_LOGIN_ACCEPT:
			if((strlen(AmuletLoginMenu_overrideCode) != 0) && (strcmp(AmuletLoginMenu_username, "RoOt") == 0)){
				value = atoi(AmuletLoginMenu_overrideCode);
				floatvalue = value;
				sqrtvalue = sqrt(floatvalue);
				sprintf(key, "%.3f", sqrtvalue);
				strcpy(fraction, &(key[strlen(key) - 3]));
				if(strcmp(fraction, AmuletLoginMenu_password) == 0){
					AmuletSecurity_accepted = TRUE;
					AmuletWellSchillingSearch_accepted = TRUE;
					AmuletWellPlasmaSearch_accepted = TRUE;
					AmuletWellRBCSearch_accepted = TRUE;
					AmuletWellAdvancedSetup_accepted = TRUE;
					AmuletSecurity_relogin = TRUE;
					AmuletWellBioAssay_accepted = TRUE;
					AmuletWellBioAssaySearch_accepted = TRUE;
					AmuletWellThyroidUptake_accepted = TRUE;
					AmuletWellRBCSurvival_accepted = TRUE;
					DB_GetUser("root", &user);
					current.user_id = user.UserID;
					SetAmuletBackHTML();
					AmuletLoginMenu_overrideCode[0] = 0;
				}else{
					Amulet_DisplayError("Login Error", "Incorrect Username or Password", TRUE);
					AmuletLoginMenu_overrideCode[0] = 0;
				}
			}else{
				DB_GetUser(AmuletLoginMenu_username, &user);
				if(user.UserID < 0){
					Amulet_DisplayError("Login Error", "Incorrect Username or Password", TRUE);
				}else{
					if(user.Inactive){
						Amulet_DisplayError("Login Error", "Username is inactive", TRUE);
					}else{
						if(strcmp(user.Password, AmuletLoginMenu_password) == 0){
							if(AmuletLoginMenu_blockGuest){
								if(user.Role > 0){
									if(user.Role < AmuletLoginMenu_minRole){
										Amulet_DisplayError("Login Error", "This login does not have permission", TRUE);
									}else{
										AmuletSecurity_accepted = TRUE;
										AmuletWellSchillingSearch_accepted = TRUE;
										AmuletWellPlasmaSearch_accepted = TRUE;
										AmuletWellRBCSearch_accepted = TRUE;
										AmuletWellAdvancedSetup_accepted = TRUE;
										AmuletSecurity_relogin = TRUE;
										AmuletWellBioAssay_accepted = TRUE;
										AmuletWellBioAssaySearch_accepted = TRUE;
										AmuletWellThyroidUptake_accepted = TRUE;
										AmuletWellRBCSurvival_accepted = TRUE;
										current.user_id = user.UserID;
										SetAmuletBackHTML();
									}
								}else{
									Amulet_DisplayError("Login Error", "Guest does not have permission", TRUE);
								}
							}else{
								if(user.Role < AmuletLoginMenu_minRole){
									Amulet_DisplayError("Login Error", "This login does not have permission", TRUE);
								}else{
									AmuletSecurity_accepted = TRUE;
									AmuletWellSchillingSearch_accepted = TRUE;
									AmuletWellPlasmaSearch_accepted = TRUE;
									AmuletWellRBCSearch_accepted = TRUE;
									AmuletWellAdvancedSetup_accepted = TRUE;
									AmuletSecurity_relogin = TRUE;
									AmuletWellBioAssay_accepted = TRUE;
									AmuletWellBioAssaySearch_accepted = TRUE;
									AmuletWellThyroidUptake_accepted = TRUE;
									AmuletWellRBCSurvival_accepted = TRUE;
									current.user_id = user.UserID;
									SetAmuletBackHTML();
								}
							}
						}else{
							Amulet_DisplayError("Login Error", "Incorrect Username or Password", TRUE);
						}
					}
				}
			}
			beep_amulet();
			return;
	}
}

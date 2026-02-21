#define PHASE_SECURITY_PRE_INIT	0
#define PHASE_SECURITY_WAIT		1
#define PHASE_SECURITY_PGDOWN		2
#define PHASE_SECURITY_PGUP		3
#define PHASE_SECURITY_ROW1		4
#define PHASE_SECURITY_ROW2		5
#define PHASE_SECURITY_ROW3		6
#define PHASE_SECURITY_ROW4		7
#define PHASE_SECURITY_ROW5		8
#define PHASE_SECURITY_ROW6		9
#define PHASE_SECURITY_ROW7		10
#define PHASE_SECURITY_ROW8		11
#define PHASE_SECURITY_ROW9		12
#define PHASE_SECURITY_ROW10		13
#define PHASE_SECURITY_ADD			14
#define PHASE_SECURITY_EDIT			15
#define PHASE_SECURITY_DISABLE		16
#define PHASE_SECURITY_TRADITIONAL	17
#define PHASE_SECURITY_ENHANCED		18
#define PHASE_SECURITY_HOME			19
#define PHASE_SECURITY_BACK			20


#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "wipes.h"
#include "time.h"
#include "mca.h"
#include "database.h"
#include "printer.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern bool AmuletLoginMenu_blockGuest;
extern int AmuletLoginMenu_minRole;
extern USER AmuletAddEditUser_user;
extern int AmuletWellBioAssay_staffID;
extern int AmuletWellBioAssaySearch_staffID;

bool AmuletSecurity_securityMode;
bool AmuletSecurity_activeOnlyStaff;
bool AmuletSecurity_accepted;
bool AmuletSecurity_relogin;
static int AmuletSecurity_userCount;
static int AmuletSecurity_currentPage;	// Zero based
static int AmuletSecurity_lastPage;		// Zero based
static int AmuletSecurity_currentIndex;	// Zero based
static USER *AmuletSecurity_user;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void SetAmuletHomeHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
unsigned char PopPageStack(void);
static void AmuletSecurity_displayPage(bool refreshScreen);

void AmuletSecurity_menu(void){
	USER guestuser, superuser, user;

	switch(m_iPhase){
		case PHASE_SECURITY_PRE_INIT:
			if(AmuletSecurity_securityMode){
				DB_GetUser("Guest", &guestuser);
				DB_GetUser("root", &superuser);

				AmuletSecurity_currentPage = 0;
				AmuletSecurity_userCount = 0;
				if(m_ucClear == 76){
					AmuletSecurity_relogin = FALSE;
					m_ucClear = 0;
					if((current.security_mode == 1 && guestuser.UserID == current.user_id) || (current.security_mode == 2)){
						AmuletLoginMenu_blockGuest = TRUE;
						AmuletLoginMenu_minRole = 0;
						m_ucClear = 75;
						SetAmuletHTML(AmuletHTMLIndex[LOGIN_HTM]);
						PushPageStack(AmuletHTMLIndex[LOGIN_HTM]);
						AmuletSecurity_accepted = FALSE;
						return;
					}else{
						AmuletSecurity_accepted = TRUE;
					}
				}

				if(!AmuletSecurity_accepted || (guestuser.UserID == current.user_id)){
					SetAmuletBackHTML();
					return;
				}

				SetAmuletString(99, "Setup Staff");
				SetAmuletString(90, "Edit");
				SetAmuletByte(95, 0xFF);

				if(current.security_mode == 0){
					if((current.user_id == superuser.UserID) && (AmuletSecurity_relogin)){
						SetAmuletByte(80, current.security_mode + 1);
						SetAmuletByte(90, 0xFF);
					}
				}else{
					if(current.user_id == superuser.UserID){
						SetAmuletByte(80, current.security_mode + 1);
						SetAmuletByte(90, 0xFF);
					}
				}

				DB_GetUserFromID(current.user_id, &user);

				if(user.Role > 1) SetAmuletByte(93, 0xFF);

				AmuletSecurity_userCount = DB_GetUserCount(&user);

				AmuletSecurity_user = malloc(AmuletSecurity_userCount * sizeof(USER));

				if(current.security_mode == 0){
					if(current.user_id == superuser.UserID){
						if(AmuletSecurity_relogin) AmuletSecurity_userCount = DB_GetUserList(&user, &AmuletSecurity_user, TRUE, TRUE);
						else AmuletSecurity_userCount = DB_GetUserList(&user, &AmuletSecurity_user, FALSE, TRUE);
					}else{
						AmuletSecurity_userCount = DB_GetUserList(&user, &AmuletSecurity_user, TRUE, TRUE);
					}
				}else{
					AmuletSecurity_userCount = DB_GetUserList(&user, &AmuletSecurity_user, TRUE, TRUE);
				}

				AmuletSecurity_displayPage(FALSE);

				SetAmuletByte(97, 0xFF);

				m_iPhase = PHASE_SECURITY_WAIT;
			}else{
				SetAmuletString(99, "Select Staff");
				SetAmuletString(90, "Select");
				SetAmuletByte(90, 0x00);
				SetAmuletByte(93, 0x00);
				SetAmuletByte(95, 0x00);
				DB_GetUser("root", &user);
				AmuletSecurity_userCount = DB_GetUserCount(&user);
				AmuletSecurity_user = malloc(AmuletSecurity_userCount * sizeof(USER));
				AmuletSecurity_userCount = DB_GetUserList(&user, &AmuletSecurity_user, FALSE, !AmuletSecurity_activeOnlyStaff);
				if(AmuletSecurity_userCount == 0){
					PopPageStack();
					Amulet_DisplayError("Staff", "Staff List is empty", TRUE);
					free(AmuletSecurity_user);
					return;
				}
				AmuletSecurity_displayPage(FALSE);
				SetAmuletByte(97, 0xFF);
				m_iPhase = PHASE_SECURITY_WAIT;
			}
			break;

		case PHASE_SECURITY_WAIT:
			break;

		case PHASE_SECURITY_PGDOWN:
			beep_amulet();
			AmuletSecurity_currentPage++;
			AmuletSecurity_displayPage(TRUE);
			m_iPhase = PHASE_SECURITY_WAIT;
			break;

		case PHASE_SECURITY_PGUP:
			beep_amulet();
			if(AmuletSecurity_currentPage){
				AmuletSecurity_currentPage--;
				AmuletSecurity_displayPage(TRUE);
			}
			m_iPhase = PHASE_SECURITY_WAIT;
			break;

		case PHASE_SECURITY_ROW1:
		case PHASE_SECURITY_ROW2:
		case PHASE_SECURITY_ROW3:
		case PHASE_SECURITY_ROW4:
		case PHASE_SECURITY_ROW5:
		case PHASE_SECURITY_ROW6:
		case PHASE_SECURITY_ROW7:
		case PHASE_SECURITY_ROW8:
		case PHASE_SECURITY_ROW9:
		case PHASE_SECURITY_ROW10:
			beep_amulet();
			AmuletSecurity_currentIndex = (10 * AmuletSecurity_currentPage) + (m_iPhase - PHASE_SECURITY_ROW1);
			m_iPhase = PHASE_SECURITY_WAIT;
			break;

		case PHASE_SECURITY_ADD:
			beep_amulet();
			AmuletAddEditUser_user.UserID = -1;
			m_ucClear = 77;
			SetAmuletHTML(AmuletHTMLIndex[ADDEDITUSER_HTM]);
			PushPageStack(AmuletHTMLIndex[ADDEDITUSER_HTM]);
			free(AmuletSecurity_user);
			return;

		case PHASE_SECURITY_EDIT:
			beep_amulet();
			if(AmuletSecurity_securityMode){
				AmuletAddEditUser_user.UserID = AmuletSecurity_user[AmuletSecurity_currentIndex].UserID;
				m_ucClear = 77;
				SetAmuletHTML(AmuletHTMLIndex[ADDEDITUSER_HTM]);
				PushPageStack(AmuletHTMLIndex[ADDEDITUSER_HTM]);
				free(AmuletSecurity_user);
				return;
			}else{
				AmuletWellBioAssay_staffID = AmuletSecurity_user[AmuletSecurity_currentIndex].UserID;
				AmuletWellBioAssaySearch_staffID = AmuletSecurity_user[AmuletSecurity_currentIndex].UserID;
				SetAmuletBackHTML();
				free(AmuletSecurity_user);
				return;
			}

		case PHASE_SECURITY_DISABLE:
			beep_amulet();
			current.security_mode = 0;
			DB_WriteSecurityMode(current.security_mode);
			m_iPhase = PHASE_SECURITY_WAIT;
			break;

		case PHASE_SECURITY_TRADITIONAL:
			beep_amulet();
			current.security_mode = 1;
			DB_WriteSecurityMode(current.security_mode);
			m_iPhase = PHASE_SECURITY_WAIT;
			break;

		case PHASE_SECURITY_ENHANCED:
			beep_amulet();
			current.security_mode = 2;
			DB_WriteSecurityMode(current.security_mode);
			m_iPhase = PHASE_SECURITY_WAIT;
			break;

		case PHASE_SECURITY_BACK:
			beep_amulet();
			if(AmuletSecurity_securityMode){
				if(current.security_mode == 0){
					DB_GetUser("root", &superuser);
					current.user_id = superuser.UserID;
				}else if(current.security_mode == 2){
					DB_GetUser("Guest", &guestuser);
					current.user_id = guestuser.UserID;
				}
			}
			SetAmuletBackHTML();
			free(AmuletSecurity_user);
			return;

		case PHASE_SECURITY_HOME:
			beep_amulet();
			if(AmuletSecurity_securityMode){
				if(current.security_mode == 0){
					DB_GetUser("root", &superuser);
					current.user_id = superuser.UserID;
				}else if(current.security_mode == 2){
					DB_GetUser("Guest", &guestuser);
					current.user_id = guestuser.UserID;
				}
			}
			SetAmuletHomeHTML();
			free(AmuletSecurity_user);
			return;
	}
}

static void AmuletSecurity_displayPage(bool refreshScreen){
	int index;
	char message[26];
	char username[31];
	char firstname[31];
	char lastname[31];

	AmuletSecurity_lastPage = AmuletSecurity_userCount / 10;
	if(AmuletSecurity_userCount % 10) AmuletSecurity_lastPage++;
	AmuletSecurity_lastPage--;
	if(AmuletSecurity_lastPage < 0) AmuletSecurity_lastPage = 0;

	if(AmuletSecurity_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletSecurity_currentPage == 0){
			SetAmuletByte(91, 0x00);  // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletSecurity_currentPage == AmuletSecurity_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletSecurity_currentPage + index) < AmuletSecurity_userCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletSecurity_securityMode){
				if(AmuletSecurity_user[10 * AmuletSecurity_currentPage + index].Inactive){
					username[0] = 4;
					username[1] = 0;
					firstname[0] = 4;
					firstname[1] = 0;
					lastname[0] = 4;
					lastname[1] = 0;
				}else{
					username[0] = 0;
					firstname[0] = 0;
					lastname[0] = 0;
				}
			}else{
				username[0] = 0;
				firstname[0] = 0;
				lastname[0] = 0;
			}

			strcpy(message, (const char *) AmuletSecurity_user[10 * AmuletSecurity_currentPage + index].UserName);
			strcat(username, message);
			SetAmuletString(3*index + 100, username);

			strcpy(message, (const char *) AmuletSecurity_user[10 * AmuletSecurity_currentPage + index].FirstName);
			strcat(firstname, message);
			SetAmuletString(3*index + 101, firstname);

			strcpy(message, (const char *) AmuletSecurity_user[10 * AmuletSecurity_currentPage + index].LastName);
			strcat(lastname, message);
			SetAmuletString(3*index + 102, lastname);
		}else{
			SetAmuletByte(101 + index, 0x00);
			SetAmuletString(3*index + 100, "");
			SetAmuletString(3*index + 101, "");
			SetAmuletString(3*index + 102, "");
		}
	}

	message[0] = 0;
	if(AmuletSecurity_lastPage > 0){
		sprintf(message, "%d of %d", AmuletSecurity_currentPage + 1, AmuletSecurity_lastPage + 1);
	}
	SetAmuletString(130, message);

	if(refreshScreen){
		SetAmuletByte(100, 0xFF);
		SetAmuletByte(111, 0xFF);
	}

	AmuletSecurity_currentIndex = -1;
}


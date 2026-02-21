#define PHASE_ADDEDITUSER_PRE_INIT	0
#define PHASE_ADDEDITUSER_WAIT		1
#define PHASE_ADDEDITUSER_SAVE		2
#define PHASE_ADDEDITUSER_USER		3
#define PHASE_ADDEDITUSER_ADMIN		4
#define PHASE_ADDEDITUSER_INACTIVE	5

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "nuc.h"
#include "wipes.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern unsigned char m_ucCheckboxState;

USER AmuletAddEditUser_user;
char AmuletAddEditUser_passwordEcho[15];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void trim(char *acByte);

void AmuletAddEditUser_menu(void){
	int index;
	char message[26];
	USER superuser, currentuser;

	switch(m_iPhase){
		case PHASE_ADDEDITUSER_PRE_INIT:
			DB_GetUser("root", &superuser);
			DB_GetUserFromID(current.user_id, &currentuser);

			if(m_ucClear == 77){
				if(AmuletAddEditUser_user.UserID < 0){
					AmuletAddEditUser_user.UserName[0] = 0;
					AmuletAddEditUser_user.Password[0] = 0;
					AmuletAddEditUser_passwordEcho[0] = 0;
					AmuletAddEditUser_user.FirstName[0] = 0;
					AmuletAddEditUser_user.LastName[0] = 0;
					AmuletAddEditUser_user.Role = -1;
					AmuletAddEditUser_user.Inactive = 0;
				}else{
					DB_GetUserFromID(AmuletAddEditUser_user.UserID, &AmuletAddEditUser_user);
					strcpy(AmuletAddEditUser_passwordEcho, AmuletAddEditUser_user.Password);
				}
				m_ucClear = 0;
			}

			trim(AmuletAddEditUser_user.UserName);
			trim(AmuletAddEditUser_user.Password);
			trim(AmuletAddEditUser_passwordEcho);
			trim(AmuletAddEditUser_user.FirstName);
			trim(AmuletAddEditUser_user.LastName);

			if(AmuletAddEditUser_user.UserID < 0) SetAmuletString(100, "Add New User");
			else SetAmuletString(100, "Edit User");

			SetAmuletString(102, AmuletAddEditUser_user.UserName);
			message[0] = 0;
			for(index=0; index<strlen(AmuletAddEditUser_user.Password); index++){
				strcat(message, "*");
			}
			SetAmuletString(103, message);
			message[0] = 0;
			for(index=0; index<strlen(AmuletAddEditUser_passwordEcho); index++){
				strcat(message, "*");
			}
			SetAmuletString(104, message);

			SetAmuletString(105, AmuletAddEditUser_user.FirstName);
			SetAmuletString(106, AmuletAddEditUser_user.LastName);
			if(AmuletAddEditUser_user.Role == 1 || AmuletAddEditUser_user.Role == 2) SetAmuletByte(70, AmuletAddEditUser_user.Role);
			else SetAmuletByte(70, 0);
			if(AmuletAddEditUser_user.Inactive) SetAmuletByte(71, 2);
			else SetAmuletByte(71, 1);

			if(AmuletAddEditUser_user.UserID >= 0){
				SetAmuletByte(80, 0x00); // UserName Button OFF
				SetAmuletByte(81, 0xFF); // UserName Field 	ON
			}else{
				SetAmuletByte(80, 0xFF); // UserName Button ON
				SetAmuletByte(81, 0x00); // UserName Field 	OFF
			}

			if(AmuletAddEditUser_user.UserID == superuser.UserID){
				// Edit Admin Account
				SetAmuletByte(82, 0xFF); // Password Button	ON
				SetAmuletByte(83, 0xFF); // Password Button Echo ON
				SetAmuletByte(84, 0x00); // First Name Button OFF
				SetAmuletByte(85, 0xFF); // First Name Field ON
				SetAmuletByte(86, 0x00); // Last Name Button OFF
				SetAmuletByte(87, 0xFF); // Last Name Field ON
				SetAmuletByte(88, 0x00); // Role Label OFF
				SetAmuletByte(89, 0x00); // Role User Button OFF
				SetAmuletByte(90, 0x00); // Role Admin Button OFF
				SetAmuletByte(91, 0x00); // Inactive Checkbox OFF
			}else if(AmuletAddEditUser_user.UserID == current.user_id){
				// Edit Own Account
				SetAmuletByte(82, 0xFF); // Password Button	ON
				SetAmuletByte(83, 0xFF); // Password Button Echo ON
				SetAmuletByte(84, 0xFF); // First Name Button ON
				SetAmuletByte(85, 0x00); // First Name Field OFF
				SetAmuletByte(86, 0xFF); // Last Name Button ON
				SetAmuletByte(87, 0x00); // Last Name Field OFF
				SetAmuletByte(88, 0x00); // Role Label OFF
				SetAmuletByte(89, 0x00); // Role User Button OFF
				SetAmuletByte(90, 0x00); // Role Admin Button OFF
				SetAmuletByte(91, 0x00); // Inactive Checkbox OFF
			}else{
				if(currentuser.Role == 3){
					// Can Create or Edit Admin and User
					SetAmuletByte(82, 0xFF); // Password Button	ON
					SetAmuletByte(83, 0xFF); // Password Button Echo ON
					SetAmuletByte(84, 0xFF); // First Name Button ON
					SetAmuletByte(85, 0x00); // First Name Field OFF
					SetAmuletByte(86, 0xFF); // Last Name Button ON
					SetAmuletByte(87, 0x00); // Last Name Field OFF
					SetAmuletByte(88, 0xFF); // Role Label ON
					SetAmuletByte(89, 0xFF); // Role User Button ON
					SetAmuletByte(90, 0xFF); // Role Admin Button ON

				}else if(currentuser.Role == 2){
					// Can Create or Edit User
					SetAmuletByte(82, 0xFF); // Password Button	ON
					SetAmuletByte(83, 0xFF); // Password Button Echo ON
					SetAmuletByte(84, 0xFF); // First Name Button ON
					SetAmuletByte(85, 0x00); // First Name Field OFF
					SetAmuletByte(86, 0xFF); // Last Name Button ON
					SetAmuletByte(87, 0x00); // Last Name Field OFF
					SetAmuletByte(88, 0xFF); // Role Label ON
					SetAmuletByte(89, 0xFF); // Role User Button ON
					SetAmuletByte(90, 0x00); // Role Admin Button OFF
					SetAmuletByte(70, 0x01); // Turn on User Button
					AmuletAddEditUser_user.Role = 1;
				}else{
					// Users can not create or edit other users
					SetAmuletBackHTML();
					return;
				}

				if(AmuletAddEditUser_user.UserID < 0){
					SetAmuletByte(91, 0x00); // Inactive Checkbox OFF
				}else{
					SetAmuletByte(91, 0xFF); // Inactive Checkbox ON
				}
			}

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_ADDEDITUSER_WAIT;
			break;

		case PHASE_ADDEDITUSER_WAIT:
			break;

		case PHASE_ADDEDITUSER_SAVE:
			beep_amulet();
			if(AmuletAddEditUser_user.UserID < 0){
				// Test Uniqueness of username
				if(DB_TestUserName(AmuletAddEditUser_user.UserName)){
					Amulet_DisplayError("User Info Error", "Duplicate User Name", TRUE);
					return;
				}
			}

			// Test for Existence of Username
			if(strlen(AmuletAddEditUser_user.UserName) == 0){
				Amulet_DisplayError("User Info Error", "Missing Username", TRUE);
				return;
			}

			// Test for Existence of Password
			if(strlen(AmuletAddEditUser_user.Password) == 0){
				Amulet_DisplayError("User Info Error", "Missing Password", TRUE);
				return;
			}

			// Test for equality for Password and Password Echo
			if(strcmp(AmuletAddEditUser_user.Password, AmuletAddEditUser_passwordEcho) != 0){
				Amulet_DisplayError("User Info Error", "Failed Password Verification", TRUE);
				return;
			}
			// Test for Existence of First Name
			if(strlen(AmuletAddEditUser_user.FirstName) == 0){
				Amulet_DisplayError("User Info Error", "Missing First Name", TRUE);
				return;
			}
			// Test for Existence of Last Name
			if(strlen(AmuletAddEditUser_user.LastName) == 0){
				Amulet_DisplayError("User Info Error", "Missing Last Name", TRUE);
				return;
			}

			// Test for Role
			if(AmuletAddEditUser_user.Role < 0){
				Amulet_DisplayError("User Info Error", "Missing Role", TRUE);
				return;
			}

			DB_WriteUser(&AmuletAddEditUser_user);
			SetAmuletBackHTML();
			return;

		case PHASE_ADDEDITUSER_USER:
			beep_amulet();
			AmuletAddEditUser_user.Role = 1;
			m_iPhase = PHASE_ADDEDITUSER_WAIT;
			break;

		case PHASE_ADDEDITUSER_ADMIN:
			beep_amulet();
			AmuletAddEditUser_user.Role = 2;
			m_iPhase = PHASE_ADDEDITUSER_WAIT;
			break;

		case PHASE_ADDEDITUSER_INACTIVE:
			if(m_ucCheckboxState == 1){
				AmuletAddEditUser_user.Inactive = FALSE;
			}else if(m_ucCheckboxState == 2){
				AmuletAddEditUser_user.Inactive = TRUE;
			}

			m_iPhase = PHASE_ADDEDITUSER_WAIT;
			break;

	}
}

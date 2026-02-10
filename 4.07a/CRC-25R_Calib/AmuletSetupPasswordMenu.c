#define PHASE_SETUP_PASSWORD_PRE_INIT 	0
#define PHASE_SETUP_PASSWORD_WAIT		1
#define PHASE_SETUP_PASSWORD_SAVE		2
#define PHASE_SETUP_PASSWORD_DEFAULT	3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "mca.h"
#include "database.h"

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_accept_cancel(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void wildcard(char *input, char *output);
void trim(char *acByte);
void SetAmuletBackHTML(void);

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;

char AmuletSetupPassword_currentPassword[10];
char AmuletSetupPassword_newPassword[10];
char AmuletSetupPassword_confirmPassword[10];

void AmuletSetupPassword_menu(void){
	char wildcard_string[20];

	switch(m_iPhase){
		case PHASE_SETUP_PASSWORD_PRE_INIT:
			if(m_ucClear==115){
				AmuletSetupPassword_currentPassword[0] = 0;
				AmuletSetupPassword_newPassword[0] = 0;
				AmuletSetupPassword_confirmPassword[0] = 0;
				m_ucClear = 0;
			}

			trim(AmuletSetupPassword_currentPassword);
			trim(AmuletSetupPassword_newPassword);
			trim(AmuletSetupPassword_confirmPassword);
			send_amulet_message(L_SETUP_PASSWORD,100);    // "Setup Password"
			send_amulet_message(L_CURRENT_PASSWORD,101);    // "Current Password:"
			wildcard(AmuletSetupPassword_currentPassword, wildcard_string);
			send_to_amulet_string(102, wildcard_string);
			send_amulet_message(L_NEW_PASSWORD,103);    // "New Password:"
			wildcard(AmuletSetupPassword_newPassword, wildcard_string);
			send_to_amulet_string(104, wildcard_string);
			send_amulet_message(L_CONFIRM_PASSWORD,105);    // "Confirm Password:"
			wildcard(AmuletSetupPassword_confirmPassword, wildcard_string);
			send_to_amulet_string(106, wildcard_string);
			send_to_amulet_string(107, "");
			send_amulet_message(L_ENTER_CURRENT_PASSWORD,109);    // "Please enter Current Password"
			send_amulet_message(L_ENTER_NEW_PASSWORD,111);    // "Please enter New Password"
			send_amulet_message(L_ENTER_CONFIRM_PASSWORD,113);    // "Please enter Confirm Password"
			send_amulet_message(L_LAST_3_SN, 115);    // "Use last 3 SN digits"
			send_accept_cancel();

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_PASSWORD_WAIT;
			break;

		case PHASE_SETUP_PASSWORD_WAIT:
			break;

		case PHASE_SETUP_PASSWORD_SAVE:
			if(strlen(AmuletSetupPassword_currentPassword)){
				if(strlen(AmuletSetupPassword_newPassword)){
					if(strlen(AmuletSetupPassword_confirmPassword)){
						if(!strcmp(current.password, AmuletSetupPassword_currentPassword)){
							if(!strcmp(AmuletSetupPassword_newPassword, AmuletSetupPassword_confirmPassword)){
								strcpy(current.password, AmuletSetupPassword_newPassword);
								DB_WritePassword(current.password);
								current.bypass = -1;
								SetAmuletBackHTML();
								m_iPhase = PHASE_SETUP_PASSWORD_WAIT;
								return;
							}else{
								send_amulet_message(L_MISMATCH_PASSWORD,107);    // "New and Confirm passwords do not match"
							}
						}else{
							send_amulet_message(L_INVALID_CURRENT_PASSWORD,107);    // "Invalid Current Password"
						}
					}else{
						send_amulet_message(L_ENTER_CONFIRM_PASSWORD,107);    // "Please enter Confirm Password"
					}
				}else{
					send_amulet_message(L_ENTER_NEW_PASSWORD,107);    // "Please enter New Password"
				}
			}else{
				send_amulet_message(L_ENTER_CURRENT_PASSWORD,107);    // "Please enter Current Password"
			}
			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_SETUP_PASSWORD_WAIT;
			break;

		case PHASE_SETUP_PASSWORD_DEFAULT:
			if(strlen(AmuletSetupPassword_currentPassword)){
				if(!strcmp(current.password, AmuletSetupPassword_currentPassword)){
					DB_WritePassword("");
					DB_ReadPassword(current.password);
					current.bypass = -1;
					SetAmuletBackHTML();
					m_iPhase = PHASE_SETUP_PASSWORD_WAIT;
					return;
				}else{
					send_amulet_message(L_INVALID_CURRENT_PASSWORD,107);    // "Invalid Current Password"
				}
			}else{
				send_amulet_message(L_ENTER_CURRENT_PASSWORD,107);    // "Please enter Current Password"
			}
			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_SETUP_PASSWORD_WAIT;
			break;
	}
}

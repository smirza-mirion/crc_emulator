#define PHASE_SETUP_KEY_PRE_INIT 	0
#define PHASE_SETUP_KEY_WAIT		1
#define PHASE_SETUP_KEY_SAVE		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"
#include "message.h"
#include "mca.h"
#include "database.h"

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;

char AmuletSetupKey_setupKey[50];

void send_accept_cancel(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void GenerateDeviceID(char *deviceID);
long long int ConvertStringToKey(char *key);
void ConvertLongLongIntToAscii(long long int value, char *output);
int VerifyKeyString(char *key);
void SetKeyFeatures(void);
void trim(char *acByte);
void SetAmuletBackHTML(void);

void AmuletSetupKey_menu(void){
	char device_id[30];
	long long int longkey;

	switch(m_iPhase){
		case PHASE_SETUP_KEY_PRE_INIT:
			if(m_ucClear==114){
				if(current.key){
					ConvertLongLongIntToAscii(current.key, AmuletSetupKey_setupKey);
				}else{
					AmuletSetupKey_setupKey[0] = 0;
				}
				m_ucClear = 0;
			}

			trim(AmuletSetupKey_setupKey);
			send_amulet_message(L_SETUP_KEY,100);    // "Setup Key"
			send_amulet_message(L_DEVICE_ID,101);    // "Device ID:"
			GenerateDeviceID(device_id);
			send_to_amulet_string(102, device_id);
			send_amulet_message(L_KEY,104);    // "Key:"
			send_to_amulet_string(105, AmuletSetupKey_setupKey);
			if(current.pccomm_enabled) send_amulet_message(L_COMM_ENABLED, 107);    // "Communications: Enabled"
			else send_amulet_message(L_COMM_DISABLED, 107);    // "Communications: Disabled"
			send_amulet_message(L_PLS_ENTER_KEY, 109);    // "Please enter Key"
			send_accept_cancel();

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_KEY_WAIT;
			break;

		case PHASE_SETUP_KEY_WAIT:
			break;

		case PHASE_SETUP_KEY_SAVE:
			if(strlen(AmuletSetupKey_setupKey)){
				if(VerifyKeyString(AmuletSetupKey_setupKey)){
					send_amulet_message(L_ERR_INVALID_KEY, 107);    // "Error: Key is not valid"
					SetAmuletByte(101, 0xFF);
				}else{
					longkey = ConvertStringToKey(AmuletSetupKey_setupKey);
					if(longkey == -999){
						send_amulet_message(L_ERR_INVALID_KEY, 107);    // "Error: Key is not valid"
						SetAmuletByte(101, 0xFF);
					}else{
						current.key = longkey;
						DB_WriteKey(current.key);
						SetKeyFeatures();
						SetAmuletBackHTML();
					}
				}
			}else{
				if(current.key){
					current.key = 0;
					DB_WriteKey(current.key);
					SetKeyFeatures();
					SetAmuletBackHTML();
				}else{
					send_amulet_message(L_ERR_EMPTY_KEY, 107);    // "Error: Key is empty"
					SetAmuletByte(101, 0xFF);
				}
			}
			m_iPhase = PHASE_SETUP_KEY_WAIT;
			break;
	}
}

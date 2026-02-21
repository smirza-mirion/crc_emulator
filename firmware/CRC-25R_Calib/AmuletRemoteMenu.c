#define PHASE_REMOTE_PRE_INIT	0
#define PHASE_REMOTE_WAIT		1
#define PHASE_REMOTE_EXIT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "message.h"

extern int m_iPhase;
extern uchar EepromRoutines_brightnessCurrent;

char AmuletRemote_message[30];
bool AmuletRemote_valid;

void trim(char *acByte);
void SetAmuletBackHTML(void);
void EepromRoutines_sleepWake(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);

void AmuletRemote_menu(void){
	char message[51];
	ushort brightness, brightness2;
	unsigned char ucValue2;
	double double_value;

	switch(m_iPhase){
		case PHASE_REMOTE_PRE_INIT:
			if(AmuletRemote_valid){
				brightness = EepromRoutines_brightnessCurrent;
				if(brightness<10) brightness = 10;
				brightness *= 10;
				if(brightness>999) brightness = 999;

				if(INVERSION) brightness = 1000 - brightness;
				if (brightness<10) brightness = 10;
				SetAmuletWord(100, brightness);
				brightness2 = 900 - brightness;
				double_value = brightness2;
				double_value *= 235.0;
				double_value /= 890.0;
				double_value += 20.0;
				brightness2 = double_value;
				if(brightness2 > 255) brightness2 = 255;
				if(brightness2 < 20) brightness2 = 20;
				ucValue2 = brightness2;
				SetAmuletByte(55, ucValue2);
				SetAmuletByte(101, 0xFF);
				EepromRoutines_sleepWake();

				if(AmuletRemote_message[1]=='T'){
					if(AmuletRemote_message[3]=='1'){
						//SetAmuletString(100, "Remote Auto Zero");
						send_amulet_message(L_REMOTE_AUTO_ZERO, 100);    // "Remote Auto Zero"
					}else if(AmuletRemote_message[3]=='2'){
						//SetAmuletString(100, "Remote Background");
						send_amulet_message(L_REMOTE_BACKGROUND, 100);    // "Remote Background"
					}else if(AmuletRemote_message[3]=='3'){
						//SetAmuletString(100, "Remote Chamber Voltage Test");
						send_amulet_message(L_REMOTE_CHAMBER_VOLTAGE_TEST, 100);    // "Remote Chamber Voltage Test"
					}
					SetAmuletByte(100, 0xFF);
					m_iPhase = PHASE_REMOTE_WAIT;
				}
				AmuletRemote_valid = FALSE;
			}
			break;

		case PHASE_REMOTE_WAIT:
			if(AmuletRemote_valid){
				if(AmuletRemote_message[1]=='T'){
					switch(AmuletRemote_message[4]){
						case '0':
							send_to_amulet_string(104, "");
							//SetAmuletString(106, "OK");
							send_amulet_message(L_CAPS_OK, 106);    // "OK"
							break;

						case '1':
							//SetAmuletString(104, "Zero Out of Range");
							send_amulet_message(L_ZERO_OUT_OF_RANGE, 104);    // "Zero out of Range"
							send_to_amulet_string(106, "");
							break;

						case '2':
							send_to_amulet_string(104, "");
							//SetAmuletString(106, "Zero Drift");
							send_amulet_message(L_ZERO_DRIFT, 106);    // "Zero Drift"
							break;

						case '3':
							send_to_amulet_string(104, "");
							//SetAmuletString(106, "Background High");
							send_amulet_message(L_BACKGROUND_HIGH, 106);    // "Background High"
							break;

						case '4':
							//SetAmuletString(104, "Background Too High");
							send_amulet_message(L_BACKGROUND_TOO_HIGH3, 104);    // "Background Too High"
							send_to_amulet_string(106, "");
							break;

						case '5':
							//SetAmuletString(104, "Chamber Voltage Failed");
							send_amulet_message(L_CHAMBER_VOLTAGE_FAILED, 104);    // "Chamber Voltage Failed"
							send_to_amulet_string(106, "");
							break;

						case '6':
							send_to_amulet_string(104, "");
							//SetAmuletString(106, "Please Wait");
							send_amulet_message(L_PLEASE_WAIT, 106);    // "Please Wait"
							break;
					}

					message[9] = 0;
					strncpy(message, &(AmuletRemote_message[12]), 9);
					trim(message);
					send_to_amulet_string(102, message);

				}else{
					SetAmuletBackHTML();
					m_iPhase = PHASE_REMOTE_EXIT;
				}
				AmuletRemote_valid = FALSE;
			}
			break;

		case PHASE_REMOTE_EXIT:
			break;
	}
}

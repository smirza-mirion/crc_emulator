#define PHASE_SETUPCOMMUNICATIONS_PRE_INIT	0
#define PHASE_SETUPCOMMUNICATIONS_WAIT		1
#define PHASE_SETUPCOMMUNICATIONS_RCHAMB 	2
#define PHASE_SETUPCOMMUNICATIONS_PCHAMB	3
#define PHASE_SETUPCOMMUNICATIONS_ACCEPT	4


#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern unsigned char m_ucHotKeyNuclideID2[6][20];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void write_binary_to_Options_folder(char *acFilename, char *acData, long int lFilelength);
void SetAmuletBackHTML(void);

void AmuletSetupCommunications_menu(void){
	static unsigned char comm_hotkey;
	int index;
	USER_KEYS user_keys;

	switch(m_iPhase){
		case PHASE_SETUPCOMMUNICATIONS_PRE_INIT:
			if(m_ucClear == 51){
				if(current.comm_hotkey == 0){
					SetAmuletByte(100, 1);
					comm_hotkey = 0;
				}else{
					SetAmuletByte(100, 2);
					comm_hotkey = 1;
				}
				SetAmuletByte(101, 0xFF);
				m_ucClear = 0;
			}
			m_iPhase = PHASE_SETUPCOMMUNICATIONS_WAIT;
			break;

		case PHASE_SETUPCOMMUNICATIONS_WAIT:
			break;

		case PHASE_SETUPCOMMUNICATIONS_RCHAMB:
			comm_hotkey = 0;
			beep_amulet();
			m_iPhase = PHASE_SETUPCOMMUNICATIONS_WAIT;
			break;

		case PHASE_SETUPCOMMUNICATIONS_PCHAMB:
			comm_hotkey = 1;
			beep_amulet();
			m_iPhase = PHASE_SETUPCOMMUNICATIONS_WAIT;
			break;

		case PHASE_SETUPCOMMUNICATIONS_ACCEPT:
			beep_amulet();
			current.comm_hotkey = comm_hotkey;
			write_binary_to_Options_folder("comm.bin", (char *) &current.comm_hotkey, 1);
			if(current.comm_hotkey == 0){
				for(index=0; index<10; index++){
					if(m_ucHotKeyNuclideID2[0][index+4]==255) user_keys.keys[index] = -1;
					else user_keys.keys[index] = m_ucHotKeyNuclideID2[0][index+4];
				}
			}else{
				for(index=0; index<10; index++){
					if(m_ucHotKeyNuclideID2[1][index+4]==255) user_keys.keys[index] = -1;
					else user_keys.keys[index] = m_ucHotKeyNuclideID2[1][index+4];
				}
			}
			EE_WRITE(user_keys[0], (uchar *) &user_keys);
			SetAmuletBackHTML();
			break;
	}
}

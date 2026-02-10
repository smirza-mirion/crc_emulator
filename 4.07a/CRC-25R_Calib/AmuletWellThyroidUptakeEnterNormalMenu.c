#define PHASE_WELLTHYROIDUPTAKEENTERNORMAL_PRE_INIT	0
#define PHASE_WELLTHYROIDUPTAKEENTERNORMAL_WAIT		1
#define PHASE_WELLTHYROIDUPTAKEENTERNORMAL_SAVE		2
#define PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW1	3
#define PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW2	4
#define PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW3	5
#define PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW4	6

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "amulet.h"
#include "bioassay.h"

extern int m_iPhase;
extern unsigned char m_ucClear;

extern PROBETHYROIDUPTAKENORMAL ThyroidUptake_normal[4];
bool AmuletWellThyroidUptakeEnterNormal_enter;
PROBETHYROIDUPTAKENORMAL AmuletWellThyroidUptakeEnterNormal_value[4];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void ThyroidUptakeSaveNormal(void);

void AmuletWellThyroidUptakeEnterNormal_menu(void){
	int index, jndex;
	int tempint;
	char message[26];

	switch(m_iPhase){
		case PHASE_WELLTHYROIDUPTAKEENTERNORMAL_PRE_INIT:
			if(m_ucClear == 95){
				memcpy(&AmuletWellThyroidUptakeEnterNormal_value, &ThyroidUptake_normal, 4 * sizeof(PROBETHYROIDUPTAKENORMAL));
				m_ucClear = 0;
			}

			for(index=0; index<4; index++){
				if((AmuletWellThyroidUptakeEnterNormal_value[index].min != -1) && (AmuletWellThyroidUptakeEnterNormal_value[index].max != -1)){
					if(AmuletWellThyroidUptakeEnterNormal_value[index].min > AmuletWellThyroidUptakeEnterNormal_value[index].max){
						tempint = AmuletWellThyroidUptakeEnterNormal_value[index].min;
						AmuletWellThyroidUptakeEnterNormal_value[index].min = AmuletWellThyroidUptakeEnterNormal_value[index].max;
						AmuletWellThyroidUptakeEnterNormal_value[index].max = tempint;
					}
				}
			}

			if(AmuletWellThyroidUptakeEnterNormal_value[0].hour == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d", AmuletWellThyroidUptakeEnterNormal_value[0].hour);
			}
			SetAmuletString(101, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[0].min == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d%%", AmuletWellThyroidUptakeEnterNormal_value[0].min);
			}
			SetAmuletString(102, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[0].max == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d%%", AmuletWellThyroidUptakeEnterNormal_value[0].max);
			}
			SetAmuletString(103, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[1].hour == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d", AmuletWellThyroidUptakeEnterNormal_value[1].hour);
			}
			SetAmuletString(104, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[1].min == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d%%", AmuletWellThyroidUptakeEnterNormal_value[1].min);
			}
			SetAmuletString(105, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[1].max == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d%%", AmuletWellThyroidUptakeEnterNormal_value[1].max);
			}
			SetAmuletString(106, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[2].hour == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d", AmuletWellThyroidUptakeEnterNormal_value[2].hour);
			}
			SetAmuletString(107, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[2].min == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d%%", AmuletWellThyroidUptakeEnterNormal_value[2].min);
			}
			SetAmuletString(108, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[2].max == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d%%", AmuletWellThyroidUptakeEnterNormal_value[2].max);
			}
			SetAmuletString(109, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[3].hour == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d", AmuletWellThyroidUptakeEnterNormal_value[3].hour);
			}
			SetAmuletString(110, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[3].min == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d%%", AmuletWellThyroidUptakeEnterNormal_value[3].min);
			}
			SetAmuletString(111, message);

			if(AmuletWellThyroidUptakeEnterNormal_value[3].max == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%d%%", AmuletWellThyroidUptakeEnterNormal_value[3].max);
			}
			SetAmuletString(112, message);

			if(AmuletWellThyroidUptakeEnterNormal_enter) SetAmuletByte(100, 0xFF);
			else SetAmuletByte(105, 0xFF);

			m_iPhase = PHASE_WELLTHYROIDUPTAKEENTERNORMAL_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEENTERNORMAL_WAIT:
			break;

		case PHASE_WELLTHYROIDUPTAKEENTERNORMAL_SAVE:
			for(index=0;index<4;index++){
				if((AmuletWellThyroidUptakeEnterNormal_value[index].hour != -1) || (AmuletWellThyroidUptakeEnterNormal_value[index].min != -1) || (AmuletWellThyroidUptakeEnterNormal_value[index].max != -1)){
					if((AmuletWellThyroidUptakeEnterNormal_value[index].hour == -1) || (AmuletWellThyroidUptakeEnterNormal_value[index].min == -1) || (AmuletWellThyroidUptakeEnterNormal_value[index].max == -1)){
						sprintf(message, "Row %d is incomplete", index + 1);
						Amulet_DisplayError("Thyroid Uptake", message, TRUE);
						beep_amulet();
						return;
					}
				}
			}


			for(index=0; index<3; index++){
				if(AmuletWellThyroidUptakeEnterNormal_value[index].hour != -1){
					for(jndex=index+1; jndex<4; jndex++){
						if(AmuletWellThyroidUptakeEnterNormal_value[index].hour == AmuletWellThyroidUptakeEnterNormal_value[jndex].hour){
							sprintf(message, "Duplicate hour = %d", AmuletWellThyroidUptakeEnterNormal_value[index].hour);
							Amulet_DisplayError("Thyroid Uptake", message, TRUE);
							beep_amulet();
							return;
						}
					}
				}
			}

			// Compact to global
			for(index=0; index<4; index++){
				ThyroidUptake_normal[index].hour = -1;
				ThyroidUptake_normal[index].min = -1;
				ThyroidUptake_normal[index].max = -1;
			}
			jndex = 0;
			for(index=0; index<4; index++){
				if((AmuletWellThyroidUptakeEnterNormal_value[index].hour != -1) && (AmuletWellThyroidUptakeEnterNormal_value[index].min != -1) && (AmuletWellThyroidUptakeEnterNormal_value[index].max != -1)){
					ThyroidUptake_normal[jndex].hour = AmuletWellThyroidUptakeEnterNormal_value[index].hour;
					ThyroidUptake_normal[jndex].min = AmuletWellThyroidUptakeEnterNormal_value[index].min;
					ThyroidUptake_normal[jndex].max = AmuletWellThyroidUptakeEnterNormal_value[index].max;
					jndex++;
				}
			}

			// Save to Database
			ThyroidUptakeSaveNormal();
			SetAmuletBackHTML();
			break;

		case PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW1:
		case PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW2:
		case PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW3:
		case PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW4:
			AmuletWellThyroidUptakeEnterNormal_value[m_iPhase - PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW1].hour = -1;
			AmuletWellThyroidUptakeEnterNormal_value[m_iPhase - PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW1].min = -1;
			AmuletWellThyroidUptakeEnterNormal_value[m_iPhase - PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW1].max = -1;
			SetAmuletString(3*(m_iPhase - PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW1) + 101, "");
			SetAmuletString(3*(m_iPhase - PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW1) + 102, "");
			SetAmuletString(3*(m_iPhase - PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW1) + 103, "");
			SetAmuletByte((m_iPhase - PHASE_WELLTHYROIDUPTAKEENTERNORMAL_CLEAR_ROW1) + 101, 0xFF);
			beep_amulet();
			m_iPhase = PHASE_WELLTHYROIDUPTAKEENTERNORMAL_WAIT;
			break;
	}
}

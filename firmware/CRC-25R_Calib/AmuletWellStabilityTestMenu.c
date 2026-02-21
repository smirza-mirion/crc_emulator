#define PHASE_WELLSTABILITYTEST_PRE_INIT		0
#define PHASE_WELLSTABILITYTEST_WAIT			1
#define PHASE_WELLSTABILITYTEST_MEASURE			2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "printer.h"
#include "qspi.h"
#include "cs.h"
//#include "mmcapi.h"
#include "ff.h"

short AmuletWellStabilityTestMenu_countTime;
long AmuletWellStabilityTestMenu_reps;
float AmuletWellStabilityTestMenu_peak[100000];
time_t AmuletWellStabilityTestMenu_stamp[100000];
bool AmuletWellStabilityTestMenu_finished;

extern int m_iPhase;
extern unsigned char m_ucClear;
extern unsigned char m_ucSetWellMeasure;
extern long AmuletWellMeasurementMenu_reps;
extern volatile char m_acTitle[51];
extern bool AmuletWellMeasurementMenu_unlockDetector;

/*extern unsigned char m_ucKeyPadConfig;
extern unsigned char m_ucLenLimit;
extern unsigned char m_ucLenFractionLimit;
extern float m_fMaxValue;
extern float m_fMinValue;
extern unsigned short int m_uiSetKeyPad;
extern short AmuletWellMeasurementMenu_chiReps;

extern time_t clock_time;
extern CURRENT current;
extern bool m_flgShowPlusMinus; */

void PushPageStack(unsigned char ucPage);

/*void AmuletWellChiTest_print(double chi);*/

void AmuletWellStabilityTest_menu(void){
	long index;
	char message[51];
	char timestring[20];
	char linestr[100];
	//F_FILE *pFileHandle;
	FIL fileObject;
	FILINFO fileInfo;
	char longFileName[100];
	UINT bytesWritten;

	switch(m_iPhase){
		case PHASE_WELLSTABILITYTEST_PRE_INIT:
			if(m_ucClear == 50){
				AmuletWellStabilityTestMenu_countTime = 60;
				AmuletWellStabilityTestMenu_reps = 20;
				for(index=0; index<100000; index++){
					AmuletWellStabilityTestMenu_peak[index] = 0;
					AmuletWellStabilityTestMenu_stamp[index] = (time_t) 0;
				}
				AmuletWellStabilityTestMenu_finished = FALSE;
				m_ucClear = 0;
			}

			sprintf(message, "%d", AmuletWellStabilityTestMenu_countTime);
			SetAmuletString(101, message);

			sprintf(message, "%d", AmuletWellStabilityTestMenu_reps);
			SetAmuletString(102, message);

			SetAmuletByte(100, 0xFF);

			if(!AmuletWellStabilityTestMenu_finished){
				SetAmuletByte(101, 0xFF);
			}else{
				SetAmuletByte(102, 0xFF);
				// Save Data
				//f_chdir("\\");
				f_chdrive(0);
				f_chdir("/");

				//if(f_chdir("data") == F_NO_ERROR){
				if(f_chdir("data") == FR_OK){

					//if(mmc_file_exists("stable.csv") == 0) f_delete("stable.csv");
					fileInfo.lfname = longFileName;
					fileInfo.lfsize = 100;
					if(f_stat("stable.csv", &fileInfo) == FR_OK) f_unlink("stable.csv");

					//pFileHandle = f_open("stable.csv", "w");
					f_open(&fileObject, "stable.csv", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

					for(index=0; index<AmuletWellStabilityTestMenu_reps; index++){
						if(AmuletWellStabilityTestMenu_stamp[index]!=(time_t)0){
							timeout(timestring ,&(AmuletWellStabilityTestMenu_stamp[index]));
							sprintf(linestr, "%s,%f\r\n", timestring, AmuletWellStabilityTestMenu_peak[index]);

							//f_write(linestr, strlen(linestr), 1, pFileHandle);
							f_write(&fileObject, linestr, strlen(linestr), &bytesWritten);
						}
					}
					//f_close(pFileHandle);
					f_close(&fileObject);
				}
				SetAmuletByte(103, 0xFF);
			}

			/*if(AmuletWellChiTestMenu_finished){
				AmuletWellChiTestMenu_testTime = clock_time;
				sum = 0;
				squaredsum = 0;
				// Calculate Chi-Square
				for(index=0; index<AmuletWellChiTestMenu_reps; index++){
					sprintf(message, "%d) %ld", index+1, AmuletWellChiTestMenu_counts[index]);
					SetAmuletString(103+index, message);
					if(index==10) delayloop(50);
					sum +=  AmuletWellChiTestMenu_counts[index];
					squared = AmuletWellChiTestMenu_counts[index];
					squared = squared * squared;
					squaredsum += squared;
				}
				num = AmuletWellChiTestMenu_reps;
				chi = (num * squaredsum / sum) - sum;
				sprintf(message, "Chi-Square = %.1f", chi);
				SetAmuletString(123, message);
				SetAmuletByte(101, 0xFF);
				if(current.printer != NONE_PRINTER) SetAmuletByte(102, 0xFF);
			}*/
			m_iPhase = PHASE_WELLSTABILITYTEST_WAIT;
			break;

		case PHASE_WELLSTABILITYTEST_WAIT:
			break;

		case PHASE_WELLSTABILITYTEST_MEASURE:
			beep_amulet();
			AmuletWellStabilityTestMenu_finished = FALSE;
			AmuletWellMeasurementMenu_reps = AmuletWellStabilityTestMenu_reps;
			m_ucClear = 18;
			m_ucSetWellMeasure = 19;
			AmuletWellMeasurementMenu_unlockDetector = TRUE;
			sprintf((char *) m_acTitle, "Stability Test");
			//Mca_setAcquireTime(AmuletWellStabilityTestMenu_countTime);
			SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
			return;
	}
}

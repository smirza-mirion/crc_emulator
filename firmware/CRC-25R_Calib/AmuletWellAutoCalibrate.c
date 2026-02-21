/*#define PHASE_WELLAUTOCAL_PRE_INIT	0
#define PHASE_WELLAUTOCAL_WAIT		1
#define PHASE_WELLAUTOCAL_START		2
#define PHASE_WELLAUTOCAL_DISPLAYPARA	3
#define PHASE_WELLAUTOCAL_WRITE0_G1		4
#define PHASE_WELLAUTOCAL_WRITE50_G1	5
#define PHASE_WELLAUTOCAL_WRITE100_G1	6
#define PHASE_WELLAUTOCAL_WRITE150_G1	7
#define PHASE_WELLAUTOCAL_WRITE200_G1	8
#define PHASE_WELLAUTOCAL_WRITE250_G1	9
#define PHASE_WELLAUTOCAL_CALCPEAK	10
#define PHASE_WELLAUTOCAL_SAVE		11
#define PHASE_WELLAUTOCAL_BACK		12
#define PHASE_WELLAUTOLIN_START		13
#define PHASE_WELLAUTOLIN_CALPEAK	14
#define PHASE_WELLAUTOLIN_CLEARLIN	15
#define PHASE_WELLAUTOCAL_WRITE0_G2		16
#define PHASE_WELLAUTOCAL_WRITE50_G2	17
#define PHASE_WELLAUTOCAL_WRITE100_G2	18
#define PHASE_WELLAUTOCAL_WRITE150_G2	19
#define PHASE_WELLAUTOCAL_WRITE200_G2	20
#define PHASE_WELLAUTOCAL_WRITE250_G2	21
#define PHASE_WELLAUTOCAL_WRITE0_G3		22
#define PHASE_WELLAUTOCAL_ABORT_CAL		23
#define PHASE_WELLAUTOCAL_ABORT_LIN		24*/

#define PHASE_WELLAUTOCAL_PRE_INIT			0
#define PHASE_WELLAUTOCAL_WAIT				1
#define PHASE_WELLAUTOCAL_START				2
#define PHASE_WELLAUTOCAL_DISPLAYPARA		3
#define PHASE_WELLAUTOCAL_CREATE512GRAPH	4
#define PHASE_WELLAUTOCAL_DRAWGRAPH			5
#define PHASE_WELLAUTOCAL_CALCPEAK			6
#define PHASE_WELLAUTOCAL_SAVE				7
#define PHASE_WELLAUTOCAL_BACK				8
#define PHASE_WELLAUTOLIN_START				9
#define PHASE_WELLAUTOLIN_CALPEAK			10
#define PHASE_WELLAUTOLIN_CLEARLIN			11
#define PHASE_WELLAUTOCAL_DONE				12
#define PHASE_WELLAUTOCAL_ABORT_CAL			13
#define PHASE_WELLAUTOCAL_ABORT_LIN			14
#define PHASE_WELLAUTOCAL2_START			15
#define PHASE_WELLAUTOCAL2_CALCPEAK			16

#define CAL_CS137_1							1
#define CAL_CS137_2							2
#define CAL_EU152							3

#define AUTOTHRESHOLD 1

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "crc.h"
#include "mca.h"
#include "database.h"
#include "amulet.h"

char SetAmuletWord(unsigned char ucIndex, short unsigned int uiValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
char SetAmuletLine(ushort x1, ushort y1, ushort x2, ushort y2, ulong rgb, uchar weight);
void SetAmuletBackHTML(void);
float InterpSigma(void);
float GetFwhm(float sigma);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void Mca_setAutoCalThresholdMirror(int AutoCalThreshold);

extern int m_iPhase;
extern unsigned char m_ucClear;
extern bool AmuletWellMeasurementMenu_unlockDetector;

int AmuletWellAutoCalibrate_exportStatus = 0;	// 0 = Not Available, 1 = Auto Calibration Results Available, 2 = Auto Calibration Failed
char AmuletWellAutoCalibrate_exportFirstPeak[11] = {0,0,0,0,0,0,0,0,0,0,0};
char AmuletWellAutoCalibrate_exportFirstPeakDev[11] = {0,0,0,0,0,0,0,0,0,0,0};
char AmuletWellAutoCalibrate_exportSecondPeak[11] = {0,0,0,0,0,0,0,0,0,0,0};
char AmuletWellAutoCalibrate_exportSecondPeakDev[11] = {0,0,0,0,0,0,0,0,0,0,0};
char AmuletWellAutoCalibrate_exportFWHM[11] = {0,0,0,0,0,0,0,0,0,0,0};
char AmuletWellAutoCalibrate_exportVolts[11] = {0,0,0,0,0,0,0,0,0,0,0};
char AmuletWellAutoCalibrate_exportThreshold[11] = {0,0,0,0,0,0,0,0,0,0,0};
char AmuletWellAutoCalibrate_exportGain1[11] = {0,0,0,0,0,0,0,0,0,0,0};
char AmuletWellAutoCalibrate_exportGain2[11] = {0,0,0,0,0,0,0,0,0,0,0};
char AmuletWellAutoCalibrate_exportGain[11] = {0,0,0,0,0,0,0,0,0,0,0};
char AmuletWellAutoCalibrate_exportZero[11] = {0,0,0,0,0,0,0,0,0,0,0};
int AmuletWellAutoCalibrate_exportSequence;

float AmuletWellAutoCalibrate_linMeas[5];
SPEC_MEAS spec_meas_buff[10];
SPEC_MEAS spec_meas_copy;

static void AmuletWellAutoCalibrate_clearExport(void){
	AmuletWellAutoCalibrate_exportStatus = 0;
	memset(AmuletWellAutoCalibrate_exportFirstPeak, 0, 11);
	memset(AmuletWellAutoCalibrate_exportFirstPeakDev, 0, 11);
	memset(AmuletWellAutoCalibrate_exportSecondPeak, 0, 11);
	memset(AmuletWellAutoCalibrate_exportSecondPeakDev, 0, 11);
	memset(AmuletWellAutoCalibrate_exportFWHM, 0, 11);
	memset(AmuletWellAutoCalibrate_exportVolts, 0, 11);
	memset(AmuletWellAutoCalibrate_exportThreshold, 0, 11);
	memset(AmuletWellAutoCalibrate_exportGain1, 0, 11);
	memset(AmuletWellAutoCalibrate_exportGain2, 0, 11);
	memset(AmuletWellAutoCalibrate_exportGain, 0, 11);
	memset(AmuletWellAutoCalibrate_exportZero, 0, 11);
}

static void AmuletWellAutoCalibrate_failedAutoCalExport(void){
	float hv, hvcode;

	hvcode = Mca_getHVValue();
	if((Mca_installedDetector == DET_WELL) || (Mca_installedDetector == DET_BETA)) hv = (.158974 * hvcode) + 595.0;
	else if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)) hv = .263158 * hvcode;

	sprintf(AmuletWellAutoCalibrate_exportVolts, "%.0f", hv);

	sprintf(AmuletWellAutoCalibrate_exportThreshold, "%d", Mca_getThreshold());

	AmuletWellAutoCalibrate_exportStatus = 2;
	AmuletWellAutoCalibrate_exportSequence++;
}

void AmuletWellAutoCalibrate_menu(void){
	char acMsg[100];
	short index, jndex, kndex;
	PEAK peak[40], peak2[40];
	float maxheight;
	short maxheightindex;
	float secondmaxheight;
	short secondmaxheightindex;
	float maxheight2;
	short maxheightindex2;
	float secondmaxheight2;
	short secondmaxheightindex2;
	float channeldiff;
	float gainperchannel;
	float minfreqspread;
	short gain;
	static int firsttryindex;
	static float peakthreshold;
	static float local_auto_cal_threshold;
	static short tunephase;
	static short tunegain;
	static short tuneoffset;
	static float tunediff;
	static float tunefirstpeak;
	static short tunetweek;
	static float firstcentroidoffset1;
	static short firstoffset1;
	static float firstcentroidoffset2;
	static short firstoffset2;
	static short cs137cal;
	static bool readspectrum;
	static bool trydefaultvalues;
	static float sigma;
	static float sigma_lip;
	static ulong startmeasuring;
	static SPEC_MEAS spec_backup;
	float currenttunediff, previoustunediff, firstpeakcentroid, secondpeakcentroid, offsetperchannel, firstcentroiddiff, offsetdiff;
	float currentfirstdiff, previousfirstdiff, combinedgain1, combinedgain2, linstd[5], olddiff, diff, kev, maxenergy, maxchannel, fwhm, sigmach;
	short switchPhase, peakcount, dummynp1;
	short calcStart, calcEnd, resetStartCh, resetEndCh;
	float hv, hvcode, deltachannels, firstpeakchannel, fNumOfChannels, fResetTotalCounts, fResetHighEndCounts;
	long int average;
	static unsigned char display[512];
	long calct_peaklevel[10];
	float calct_peakcentroid[10], calct_peakcentroidavg, calct_peakcentroidsd, calct_peakcentroidcount;
	int calct_index;

	switch(m_iPhase){
		case PHASE_WELLAUTOCAL_PRE_INIT:
			AmuletWellAutoCalibrate_clearExport();
			for(index=0; index<5; index++) AmuletWellAutoCalibrate_linMeas[index] = 0.0;
			Mca_setAcquireTime(31000);
			Mca_setTimeType(COUNT_REAL);
			trydefaultvalues = Mca_getCalibrationStatus();
			if(trydefaultvalues){
				if((Mca_getGain1()>980) || (Mca_getGain1()<750)) trydefaultvalues = FALSE;
			}
			cs137cal = CAL_CS137_1;
			//local_auto_cal_threshold = Mca_getAutoCalThreshold();
			local_auto_cal_threshold = 0;
			sigma = 0;
			sigma_lip = 0;
			m_ucClear = 0;

			sprintf(acMsg, "%d", Mca_getDetectorMirrorPtr()->num_of_channels);
			SetAmuletString(97,  acMsg);

			hvcode = Mca_getHVValue();
			if((Mca_installedDetector == DET_WELL) || (Mca_installedDetector == DET_BETA)) hv = (.158974 * hvcode) + 595.0;
			else if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)) hv = .263158 * hvcode;

			sprintf(acMsg, "%.0f", hv);
			SetAmuletString(98, acMsg);

			sprintf(acMsg, "%d", Mca_getThreshold());
			SetAmuletString(99, acMsg);

			combinedgain1 = Mca_getGain1();
			combinedgain1 /= 64.0;
			combinedgain2 = Mca_getGain2();
			combinedgain2 /= 64.0;
			combinedgain1 *= combinedgain2;
			sprintf(acMsg, "%.2f (%d,%d)", combinedgain1, Mca_getGain1(), Mca_getGain2());
			SetAmuletString(101, acMsg);
			sprintf(acMsg, "%d", Mca_getZeroOpAmpOffset());
			SetAmuletString(103, acMsg);

			Mca_sendDetectorType();

			if(Mca_installedDetector == DET_DRILLEDPROBE700){
				SetAmuletString(95, "Cs137 in Drilled Center");
				SetAmuletString(96, "Cs137 on Perimeter");
			}else{
				SetAmuletString(95, "Cal with Cs137");
				SetAmuletString(96, "");
			}
			SetAmuletWord(91, 0xFFFF);
			SetAmuletWord(98, 0xFFFF);

			if(Mca_isMultiDetector() && AmuletWellMeasurementMenu_unlockDetector) SetAmuletWord(90, 0xFFFF);
			SetAmuletWord(100, 0xFFFF);
			SetAmuletWord(99, 0xFFFF);

			SetAmuletLine(288, 75, 288, 330, 0, 1);
			SetAmuletLine(288, 330, 800, 330, 0, 1);

			m_iPhase = PHASE_WELLAUTOCAL_WAIT;
			break;

		case PHASE_WELLAUTOCAL_WAIT:
			break;

		case PHASE_WELLAUTOCAL_START:
			AmuletWellAutoCalibrate_clearExport();
			sigma = 0;
			sigma_lip = 0;
			local_auto_cal_threshold = 0;
			beep_amulet();
			firsttryindex = 0;
			cs137cal = CAL_CS137_1;
			readspectrum = TRUE;
			for(index=0; index<5; index++) AmuletWellAutoCalibrate_linMeas[index] = 0.0;

#ifdef AUTOTHRESHOLD
			if(local_auto_cal_threshold == 0) peakthreshold = 2000.0;
			else peakthreshold = local_auto_cal_threshold;
#else
			peakthreshold = 2000.0;
#endif

			if(!trydefaultvalues){
				tunephase = 0;
				Mca_setGain(Mca_getInitialGain1(), Mca_getInitialGain2());
			}else{
#ifdef AUTOTHRESHOLD
				if(local_auto_cal_threshold == 0) peakthreshold = 3000.0;
				else peakthreshold = 1.5 * local_auto_cal_threshold;
#else
				peakthreshold = 3000.0;
#endif

				//tunephase = 7;
				tunephase = 8;
			}
			Mca_setZeroOpAmpOffset(-200);
			//SetAmuletWord(106, 0xFFFF);	// Display Stop Button
			SetAmuletWord(93, 0xFFFF);
			SetAmuletWord(99, 0xFFFF);
			m_iPhase = PHASE_WELLAUTOCAL_DISPLAYPARA;
			break;

		case PHASE_WELLAUTOCAL_DISPLAYPARA:
			Mca_startAcquisition();
			startmeasuring = g_csec_tstamp;

			combinedgain1 = Mca_getGain1();
			combinedgain1 /= 64.0;
			combinedgain2 = Mca_getGain2();
			combinedgain2 /= 64.0;
			combinedgain1 *= combinedgain2;
			sprintf(acMsg, "%.2f (%d,%d)", combinedgain1, Mca_getGain1(), Mca_getGain2());
			SetAmuletString(101, acMsg);
			sprintf(acMsg, "%d", Mca_getZeroOpAmpOffset());
			SetAmuletString(103, acMsg);
			SetAmuletWord(100, 0xFFFF);
			m_iPhase = PHASE_WELLAUTOCAL_CREATE512GRAPH;
			break;

		case PHASE_WELLAUTOCAL_CREATE512GRAPH:
			if(readspectrum) Mca_getSpectrum();
			switch(spec_meas.num_of_channels){
				case 256:
					for(index=0; index<511; index++){
						if((index%2)== 0){
							display[index] = spec_meas.display[index/2];
						}else{
							average = spec_meas.display[(index-1)/2];
							average += spec_meas.display[(index+1)/2];
							average >>= 1;
							display[index] = average;
						}
						display[511] = display[510];
					}
					break;

				case 512:
					for(index=0; index<512; index++) display[index] = spec_meas.display[index];
					break;

				case 1024:
					for(index=0; index<512; index++){
						average = spec_meas.display[2*index];
						average += spec_meas.display[2*index + 1];
						average >>= 1;
						display[index] = average;
					}
					break;

				case 2048:
					for(index=0; index<512; index++){
						average = spec_meas.display[4*index];
						average += spec_meas.display[4*index+1];
						average += spec_meas.display[4*index+2];
						average += spec_meas.display[4*index+3];
						average >>= 2;
						display[index] = average;
					}
					break;

				case 4096:
					for(index=0; index<512; index++){
						average = spec_meas.display[8*index];
						average += spec_meas.display[8*index+1];
						average += spec_meas.display[8*index+2];
						average += spec_meas.display[8*index+3];
						average += spec_meas.display[8*index+4];
						average += spec_meas.display[8*index+5];
						average += spec_meas.display[8*index+6];
						average += spec_meas.display[8*index+7];
						average >>= 3;
						display[index] = average;
					}
					break;
			}

			sprintf(acMsg, "%ld", spec_meas.max_count);
			SetAmuletString(104, acMsg);
			SetAmuletWord(101, 0xFFFF);

			AmuletWellMeasurementMenu_drawGraph(display, TRUE);
			m_iPhase = PHASE_WELLAUTOCAL_DRAWGRAPH;
			break;

		case PHASE_WELLAUTOCAL_DRAWGRAPH:
			if(AmuletWellMeasurementMenu_drawGraph(display, FALSE)){
				m_iPhase = PHASE_WELLAUTOCAL_DONE;
			}
			break;

		case PHASE_WELLAUTOCAL_DONE:
			if(readspectrum){
				if(spec_meas.peaklevel > peakthreshold){
					resetStartCh = 0;
					resetEndCh = spec_meas.num_of_channels - 1;
					fResetTotalCounts = Mca_getROICounts(spec_meas.spectrum, resetStartCh, resetEndCh);
					resetStartCh = spec_meas.num_of_channels;
					resetStartCh /= 4;
					resetStartCh *= 3;
					fResetHighEndCounts = Mca_getROICounts(spec_meas.spectrum, resetStartCh, resetEndCh);
					fResetHighEndCounts /= fResetTotalCounts;
					if(cs137cal != CAL_EU152){
						if(fResetHighEndCounts > .01){
							trydefaultvalues = FALSE;
	#ifdef AUTOTHRESHOLD
							if(local_auto_cal_threshold == 0) peakthreshold = 2000.0;
							else peakthreshold = local_auto_cal_threshold;
	#else
							peakthreshold = 2000.0;
	#endif
							Mca_setGain(Mca_getInitialGain1(), Mca_getInitialGain2());
							tunephase = 0;
							m_iPhase = PHASE_WELLAUTOCAL_DISPLAYPARA;
							return;
						}else{
							if(cs137cal == CAL_CS137_1) m_iPhase = PHASE_WELLAUTOCAL_CALCPEAK;
							else if(cs137cal == CAL_CS137_2) m_iPhase = PHASE_WELLAUTOCAL2_CALCPEAK;
							else if(cs137cal == CAL_EU152) m_iPhase = PHASE_WELLAUTOLIN_CALPEAK;
						}
					}else{
						m_iPhase = PHASE_WELLAUTOLIN_CALPEAK;
					}
				}
				else{
					if((g_csec_tstamp - startmeasuring) > 1000){
						if(spec_meas.peaklevel > 50){
							m_iPhase = PHASE_WELLAUTOCAL_CREATE512GRAPH;
						}else{
							if(cs137cal == CAL_CS137_1){
								gain = Mca_getGain2();
								if(gain >= 200){
									Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nUnable to detect spectra",TRUE);
									Mca_setMirrorToGain();
									Mca_setAutoCalThresholdMirror(0);
									local_auto_cal_threshold = 0;
									AmuletWellAutoCalibrate_failedAutoCalExport();
									//DB_WriteAutoCalThreshold(0, Mca_installedDetector);
									return;
									//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
								}else{
									Mca_setAutoCalThresholdMirror(0);
									local_auto_cal_threshold = 0;
									//DB_WriteAutoCalThreshold(0, Mca_installedDetector);
									gain *= 2;
									if(gain>1023) gain = 1023;
									Mca_setGain(Mca_getGain1(), gain);
									m_iPhase = PHASE_WELLAUTOCAL_DISPLAYPARA;
								}
							}else if(cs137cal == CAL_CS137_2){
								Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nUnable to detect spectra",TRUE);
								Mca_setMirrorToGain();
								AmuletWellAutoCalibrate_failedAutoCalExport();
								return;
							}else if(cs137cal == CAL_EU152){
								Amulet_DisplayError("AutoCalibrate Error", "Linearity Correction Aborted\nCannot detect Eu152 source",TRUE);
								return;
								//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
							}
						}
					}else{
						m_iPhase = PHASE_WELLAUTOCAL_CREATE512GRAPH;
					}

#ifdef AUTOTHRESHOLD
					if(local_auto_cal_threshold == 0){
						if(spec_meas.peaklevel > 1000){
							if(firsttryindex < 10){
								memcpy(&(spec_meas_buff[firsttryindex++]), &spec_meas, sizeof(SPEC_MEAS));
							}
						}
					}
#endif
				}
			}else{
				//m_iPhase = PHASE_WELLAUTOCAL_CALCPEAK;
				m_iPhase = PHASE_WELLAUTOCAL_WAIT;
			}
			break;

		case PHASE_WELLAUTOCAL_CALCPEAK:
			/*if(firsttry){
				if(firsttryindex > 0){
					SetAmuletWord(102, 0xFFFF);

					calct_index = 0;

					memcpy(&spec_meas_copy, &spec_meas, sizeof(SPEC_MEAS));
					for(index=0; index<firsttryindex; index++){
						memcpy(&spec_meas, &(spec_meas_buff[index]), sizeof(SPEC_MEAS));
						Mca_getPeaks(peak, &dummynp1, TRUE);

						maxheight = 0;
						maxheightindex = -1;
						for(jndex=0; jndex<40; jndex++){
							if((peak[jndex].height>0) && (peak[jndex].centroid>0)){
								if(peak[jndex].height > maxheight){
									maxheight = peak[jndex].height;
									maxheightindex = jndex;
								}
							}
						}
						if(maxheightindex >= 0){
							//printf("%ld - %f\n", spec_meas.peaklevel, peak[maxheightindex].centroid);
							calct_peaklevel[calct_index] = spec_meas.peaklevel;
							calct_peakcentroid[calct_index] = peak[maxheightindex].centroid;
							calct_index++;
						}
					}
					memcpy(&spec_meas, &spec_meas_copy, sizeof(SPEC_MEAS));

					if(calct_index > 0){
						calct_peakcentroidcount = calct_index;

						calct_peakcentroidavg = 0;
						for(index=0; index<calct_index; index++){
							calct_peakcentroidavg += calct_peakcentroid[index];
						}
						calct_peakcentroidavg /= calct_peakcentroidcount;

						calct_peakcentroidsd = 0;
						for(index=0; index<calct_index; index++){
							calct_peakcentroidsd += ((calct_peakcentroid[index] - calct_peakcentroidavg) * (calct_peakcentroid[index] - calct_peakcentroidavg));
						}
						calct_peakcentroidsd /=  calct_peakcentroidcount;
						calct_peakcentroidsd = sqrt(calct_peakcentroidsd);

						//printf("Average: %f, SD: %f\n", calct_peakcentroidavg, calct_peakcentroidsd);

						for(index=0; index<calct_index; index++){
							if(fabs(calct_peakcentroid[index] - calct_peakcentroidavg) < calct_peakcentroidsd/2.0){
								//printf("%d - %ld, %f, %f (PASS)\n", index, calct_peaklevel[index], calct_peakcentroid[index], calct_peakcentroid[index] - calct_peakcentroidavg);
								//Mca_setAutoCalThresholdMirror(calct_peaklevel[index]);
								//peakthreshold = Mca_getAutoCalThreshold();
								local_auto_cal_threshold = calct_peaklevel[index];
								peakthreshold = local_auto_cal_threshold;
								measured_auto_cal_threshold = TRUE;
								break;
							}
							//else
							//	printf("%d - %ld, %f, %f (FAIL)\n", index, calct_peaklevel[index], calct_peakcentroid[index], calct_peakcentroid[index] - calct_peakcentroidavg);
						}
					}
				}
			}*/

			switchPhase = 0;
			Mca_getPeaks(peak, &dummynp1, TRUE);
			maxheight = secondmaxheight = 0;
			for(index=0; index<40; index++){
				if((peak[index].height>0) && (peak[index].centroid>0)){
					if(peak[index].height > maxheight){
						maxheight = peak[index].height;
						maxheightindex = index;
					}
				}
			}

			if(maxheight!=0){
				for(index=0; index<40; index++){
					if((peak[index].height>0) && (peak[index].centroid>0)){
						if((index!=maxheightindex) && (peak[index].height>secondmaxheight)){
							secondmaxheight = peak[index].height;
							secondmaxheightindex = index;
						}
					}
				}
			}

			minfreqspread = 40.0;
			minfreqspread *= spec_meas.num_of_channels;
			minfreqspread /= 256;

			if((maxheight>0) && (secondmaxheight>0) && (fabs(peak[maxheightindex].centroid - peak[secondmaxheightindex].centroid) > minfreqspread)){
#ifdef AUTOTHRESHOLD
				if(local_auto_cal_threshold == 0){
					if(firsttryindex>7){
						if(tunephase < 7){
							SetAmuletWord(102, 0xFFFF);
						}

						calct_index = 0;
						memcpy(&spec_meas_copy, &spec_meas, sizeof(SPEC_MEAS));
						for(index=0; index<firsttryindex; index++){
							memcpy(&spec_meas, &(spec_meas_buff[index]), sizeof(SPEC_MEAS));
							Mca_getPeaks(peak2, &dummynp1, TRUE);

							maxheight2 = secondmaxheight2 = 0;
							maxheightindex2 = secondmaxheightindex2 = -1;
							for(jndex=0; jndex<40; jndex++){
								if((peak2[jndex].height>0) && (peak2[jndex].centroid>0)){
									if(peak2[jndex].height > maxheight2){
										maxheight2 = peak2[jndex].height;
										maxheightindex2 = jndex;
									}
								}
							}
							if(maxheightindex2 >= 0){
								//printf("%ld - %f\n", spec_meas.peaklevel, peak[maxheightindex].centroid);
								secondmaxheight2 = 0;
								for(jndex=0; jndex<40; jndex++){
									if((peak2[jndex].height>0) && (peak2[jndex].centroid>0) && (jndex!=maxheightindex2) && (peak2[jndex].height>secondmaxheight2)){
										secondmaxheight2 = peak2[jndex].height;
										secondmaxheightindex2 = jndex;
									}
								}
							}

							if(secondmaxheightindex2 >= 0){
								calct_peaklevel[calct_index] = spec_meas.peaklevel;
								if(secondmaxheightindex2 > maxheightindex2) calct_peakcentroid[calct_index] = peak2[secondmaxheightindex2].centroid;
								else calct_peakcentroid[calct_index] = peak2[maxheightindex2].centroid;
								calct_index++;
							}
						}
						memcpy(&spec_meas, &spec_meas_copy, sizeof(SPEC_MEAS));

						if(calct_index > 0){
							calct_peakcentroidcount = calct_index;

							calct_peakcentroidavg = 0;
							for(index=0; index<calct_index; index++){
								calct_peakcentroidavg += calct_peakcentroid[index];
							}
							calct_peakcentroidavg /= calct_peakcentroidcount;

							calct_peakcentroidsd = 0;
							for(index=0; index<calct_index; index++){
								calct_peakcentroidsd += ((calct_peakcentroid[index] - calct_peakcentroidavg) * (calct_peakcentroid[index] - calct_peakcentroidavg));
							}
							calct_peakcentroidsd /=  calct_peakcentroidcount;
							calct_peakcentroidsd = sqrt(calct_peakcentroidsd);

							//printf("Average: %f, SD: %f\n", calct_peakcentroidavg, calct_peakcentroidsd);

							for(index=0; index<calct_index; index++){
								if(fabs(calct_peakcentroid[index] - calct_peakcentroidavg) < calct_peakcentroidsd/2.0){
									//printf("%d - %ld, %f, %f (PASS)\n", index, calct_peaklevel[index], calct_peakcentroid[index], calct_peakcentroid[index] - calct_peakcentroidavg);
									//Mca_setAutoCalThresholdMirror(calct_peaklevel[index]);
									//peakthreshold = Mca_getAutoCalThreshold();
									local_auto_cal_threshold = calct_peaklevel[index];
									peakthreshold = local_auto_cal_threshold;
									break;
								}
								//else
								//	printf("%d - %ld, %f, %f (FAIL)\n", index, calct_peaklevel[index], calct_peakcentroid[index], calct_peakcentroid[index] - calct_peakcentroidavg);
							}
						}
					}
				}
#endif

				if(peak[maxheightindex].centroid > peak[secondmaxheightindex].centroid){
					channeldiff = peak[maxheightindex].centroid - peak[secondmaxheightindex].centroid;
					firstpeakcentroid = peak[secondmaxheightindex].centroid;
					secondpeakcentroid = peak[maxheightindex].centroid;
				}else{
					channeldiff = peak[secondmaxheightindex].centroid - peak[maxheightindex].centroid;
					firstpeakcentroid = peak[maxheightindex].centroid;
					secondpeakcentroid = peak[secondmaxheightindex].centroid;
				}

				fNumOfChannels = spec_meas.num_of_channels - 1;

				deltachannels = 661.66 - 32.85;
				deltachannels *= fNumOfChannels;
				deltachannels /= 2000.0;

				firstpeakchannel = 32.85;
				firstpeakchannel *= fNumOfChannels;
				firstpeakchannel /= 2000.0;

				switch(tunephase){
					case 0:
						gainperchannel = Mca_getGain2();
						gainperchannel /= channeldiff;
						gainperchannel *= deltachannels; // (661.66 - 32.85) * 255 / 2000
						gain = gainperchannel;  // Calculate coarse gain

						if(abs(gainperchannel - Mca_getGain2()) > 1.0){ // Difference between calculated coarse gain and current coarse gain is greater than 1, more coarse gain adjustment is needed

#ifdef AUTOTHRESHOLD
							if(local_auto_cal_threshold == 0) peakthreshold = 2000.0;
							else peakthreshold = local_auto_cal_threshold;
#else
							peakthreshold = 2000.0;
#endif

							if(gain>=1023){
								Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nUpper gain limit reached",TRUE);
								AmuletWellAutoCalibrate_failedAutoCalExport();
								return;
								//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
							}else{
								Mca_setGain(Mca_getGain1(), gain);
							}
						}else{                                          // Difference between calculated coarse gain and current coarse gain is less than 1, moving on to fine gain adjustment
#ifdef AUTOTHRESHOLD
							if(local_auto_cal_threshold == 0) peakthreshold = 3000.0;
							else peakthreshold = 1.5 * local_auto_cal_threshold;
#else
							peakthreshold = 3000.0;
#endif

							gainperchannel = Mca_getGain1();
							gainperchannel /= channeldiff;
							gainperchannel *= deltachannels;
							gain = gainperchannel;
							if(gain>=1023){
								Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nUpper gain limit reached",TRUE);
								AmuletWellAutoCalibrate_failedAutoCalExport();
								return;
								//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
							}else{
								Mca_setGain(gain, Mca_getGain2());
							}
							tunephase = 1;
						}
						break;

					case 1:

						if(channeldiff<=deltachannels) tunetweek = 1;
						else tunetweek = -1;

						tunephase = 2;
						tunegain = Mca_getGain1();
						tunediff = channeldiff;

						if((Mca_getGain1() + tunetweek) >= 1023){
							Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nUpper gain limit reached",TRUE);
							AmuletWellAutoCalibrate_failedAutoCalExport();
							return;
							//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
						}else if((Mca_getGain1() + tunetweek) <= 0){
							Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nLower gain limit reached",TRUE);
							AmuletWellAutoCalibrate_failedAutoCalExport();
							return;
							//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
						}else{
							Mca_setGain(Mca_getGain1() + tunetweek, Mca_getGain2());
						}
						break;

					case 2:
						if(((tunetweek==1) && (channeldiff<deltachannels)) || ((tunetweek==-1) && (channeldiff>deltachannels))){ // Single step until polarity flip
							tunegain = Mca_getGain1();
							tunediff = channeldiff;
							if((Mca_getGain1() + tunetweek) >= 1023){
								Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nUpper gain limit reached",TRUE);
								AmuletWellAutoCalibrate_failedAutoCalExport();
								return;
								//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
							}else if((Mca_getGain1() + tunetweek) <= 0){
								Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nLower gain limit reached",TRUE);
								AmuletWellAutoCalibrate_failedAutoCalExport();
								return;
								//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
							}else{
								Mca_setGain(Mca_getGain1() + tunetweek, Mca_getGain2());
							}
						}else{
							if(channeldiff > deltachannels) currenttunediff = channeldiff - deltachannels; // Evaluate which side of the flip is closest to ideal
							else currenttunediff = deltachannels - channeldiff;

							if(tunediff > deltachannels) previoustunediff = tunediff - deltachannels;
							else previoustunediff = deltachannels - tunediff;

							if(previoustunediff < currenttunediff) Mca_setGain(tunegain, Mca_getGain2());

							if(!trydefaultvalues){
								firstcentroidoffset1 = firstpeakcentroid;
								firstoffset1 = Mca_getZeroOpAmpOffset();
								Mca_setZeroOpAmpOffset(firstoffset1 - 100);
							}else{
								Mca_setMirrorToZeroOpAmpOffset();
							}
							tunephase = 3;
						}
						break;

					case 3:
						firstcentroidoffset2 = firstpeakcentroid;
						firstoffset2 = Mca_getZeroOpAmpOffset();

						firstcentroiddiff = firstcentroidoffset2 - firstcentroidoffset1;
						offsetdiff = firstoffset2 - firstoffset1;
						offsetperchannel = offsetdiff / firstcentroiddiff;

						firstcentroiddiff = firstpeakchannel - firstpeakcentroid; // 32.85 * 2000 / 255
						offsetdiff = offsetperchannel * firstcentroiddiff;
						tunetweek = offsetdiff;
						Mca_setZeroOpAmpOffset(Mca_getZeroOpAmpOffset() + tunetweek);
						tunephase = 4;
						//tunephase = 5;
						break;

					case 4:
						if(firstpeakcentroid == firstpeakchannel){
#ifdef AUTOTHRESHOLD
							//if(local_auto_cal_threshold == 0) peakthreshold = 5000.0;
							//else peakthreshold = 2.5 * local_auto_cal_threshold;
							peakthreshold = 5000.0;
#else
							peakthreshold = 5000.0;
#endif
							tunetweek = 0;
							tunephase = 7;
						}else{
							firstcentroidoffset2 = firstpeakcentroid;
							firstoffset2 = Mca_getZeroOpAmpOffset();

							firstcentroiddiff = firstcentroidoffset2 - firstcentroidoffset1;
							offsetdiff = firstoffset2 - firstoffset1;
							offsetperchannel = offsetdiff / firstcentroiddiff;

							firstcentroiddiff = firstpeakchannel - firstpeakcentroid; // 32.85 * 2000 / 255
							offsetdiff = offsetperchannel * firstcentroiddiff;
							tunetweek = offsetdiff;


							if(tunetweek == 0){
								if(firstpeakcentroid<firstpeakchannel){
									tunetweek = -1;
									tunephase = 6;
								}else if(firstpeakcentroid>firstpeakchannel){
									tunetweek = 1;
									tunephase = 6;
								}else{
#ifdef AUTOTHRESHOLD
									//if(local_auto_cal_threshold == 0) peakthreshold = 5000.0;
									//else peakthreshold = 2.5 * local_auto_cal_threshold;
									peakthreshold = 5000.0;
#else
									peakthreshold = 5000.0;
#endif
									tunetweek = 0;
									tunephase = 7;
								}
								tuneoffset = Mca_getZeroOpAmpOffset();
								tunefirstpeak = firstpeakcentroid;
								Mca_setZeroOpAmpOffset(tuneoffset + tunetweek);
								//printf("%f - Tunetweek: %d, (Stage 4 Tunetweek = 0)\n", firstpeakcentroid, tunetweek);
							}else{
								Mca_setZeroOpAmpOffset(Mca_getZeroOpAmpOffset() + tunetweek);
								//printf("%f - Tunetweek: %d, (Stage 4)\n", firstpeakcentroid, tunetweek);
								tunephase = 5;
							}
						}
						break;

					case 5:
						if(firstpeakcentroid<firstpeakchannel){
							tunetweek = -1;
							tunephase = 6;
						}else if(firstpeakcentroid>firstpeakchannel){
							tunetweek = 1;
							tunephase = 6;
						}else{
#ifdef AUTOTHRESHOLD
									//if(local_auto_cal_threshold == 0) peakthreshold = 5000.0;
									//else peakthreshold = 2.5 * local_auto_cal_threshold;
									peakthreshold = 5000.0;
#else
									peakthreshold = 5000.0;
#endif
							tunetweek = 0;
							tunephase = 7;
						}
						tuneoffset = Mca_getZeroOpAmpOffset();
						tunefirstpeak = firstpeakcentroid;
						Mca_setZeroOpAmpOffset(tuneoffset + tunetweek);
						//printf("%f - Tunetweek: %d, (Stage 5)\n", firstpeakcentroid, tunetweek);
						break;

					case 6:
						if(((tunetweek==1) && (firstpeakcentroid>firstpeakchannel)) || ((tunetweek==-1) && (firstpeakcentroid<firstpeakchannel))){
							tuneoffset = Mca_getZeroOpAmpOffset();
							tunefirstpeak = firstpeakcentroid;
							Mca_setZeroOpAmpOffset(tuneoffset + tunetweek);
							//printf("%f - Tunetweek: %d, (Stage 6 Loop)\n", firstpeakcentroid, tunetweek);
						}else{
							if(firstpeakcentroid > firstpeakchannel) currentfirstdiff = firstpeakcentroid - firstpeakchannel;
							else currentfirstdiff = firstpeakchannel - firstpeakcentroid;

							if(tunefirstpeak > firstpeakchannel) previousfirstdiff = tunefirstpeak - firstpeakchannel;
							else previousfirstdiff = firstpeakchannel - tunefirstpeak;

							if(previousfirstdiff < currentfirstdiff) Mca_setZeroOpAmpOffset(tuneoffset);

#ifdef AUTOTHRESHOLD
							//if(local_auto_cal_threshold == 0) peakthreshold = 5000.0;
							//else peakthreshold = 2.5 * local_auto_cal_threshold;
							peakthreshold = 5000.0;
#else
							peakthreshold = 5000.0;
#endif
							//printf("%f - Tunetweek: %d, (Stage 6 Final)\n", firstpeakcentroid, tunetweek);
							tunephase = 7;
						}
						break;

					case 7:
						//SetAmuletLine(0x1234, 0x5678, 0x9abc, 0xdef0, 0x00123456, 0x12);
						//SetAmuletLine(0, 329, 799, 329, 0xFF0000, 0x01);
						//SetAmuletLine(289, 0, 289, 599, 0xFF0000, 0x01);
						//SetAmuletLine(288, 74, 799, 74, 0xFF0000, 0x01);

						// sigma in channels
						sigmach = InterpSigma();

						// sigma in energy(keV)
						maxenergy = 2000;
						maxchannel = spec_meas.num_of_channels - 1;
						sigma = sigmach * maxenergy;
						sigma /= maxchannel;
						sigma_lip = 0;

						fwhm = GetFwhm(sigma);

						Mca_getROIChannelsWithoutCorrection(661.66 - (2.15 * sigma), 661.66 + (2.15 * sigma), &calcStart, &calcEnd, spec_meas.num_of_channels);

						Mca_drawROI(calcStart, calcEnd, display, spec_meas.num_of_channels);

						fNumOfChannels = spec_meas.num_of_channels - 1;
						firstpeakcentroid *= 2000.0;
						firstpeakcentroid /= fNumOfChannels;
						AmuletWellAutoCalibrate_linMeas[0] = firstpeakcentroid;
						sprintf(AmuletWellAutoCalibrate_exportFirstPeak, "%.2f", firstpeakcentroid);
						sprintf(acMsg, "1st Peak: %.2f keV", firstpeakcentroid);
						SetAmuletString(106, acMsg);

						firstpeakcentroid -= 32.85;
						firstpeakcentroid /= 32.85;
						firstpeakcentroid *= 100.0;
						sprintf(AmuletWellAutoCalibrate_exportFirstPeakDev, "%.2f", firstpeakcentroid);
						sprintf(acMsg, "%.2f %%", firstpeakcentroid);
						SetAmuletString(108, acMsg);

						secondpeakcentroid *= 2000.0;
						secondpeakcentroid /= fNumOfChannels;
						AmuletWellAutoCalibrate_linMeas[4] = secondpeakcentroid;
						sprintf(AmuletWellAutoCalibrate_exportSecondPeak, "%.2f", secondpeakcentroid);
						sprintf(acMsg, "2nd Peak: %.2f keV", secondpeakcentroid);
						SetAmuletString(109, acMsg);

						secondpeakcentroid -= 661.66;
						secondpeakcentroid /= 661.66;
						secondpeakcentroid *= 100.0;
						sprintf(AmuletWellAutoCalibrate_exportSecondPeakDev, "%.2f", secondpeakcentroid);
						sprintf(acMsg, "%.2f %%", secondpeakcentroid);
						SetAmuletString(111, acMsg);

						sprintf(AmuletWellAutoCalibrate_exportFWHM, "%.3f", fwhm);
						sprintf(acMsg, "FWHM: %.3f%%", fwhm);
						//printf("First: %.2f%%, Second: %.2f%%, FWHM: %.3f%%\n", firstpeakcentroid, secondpeakcentroid, fwhm);
						SetAmuletString(112, acMsg);
						SetAmuletString(114, "");

						SetAmuletString(115, "");
						SetAmuletString(117, "");

						SetAmuletString(118, "");
						SetAmuletString(120, "");

						//Test
						//AmuletWellAutoCalibrate_linMeas[1] = 40;
						//AmuletWellAutoCalibrate_linMeas[2] = 122;
						//AmuletWellAutoCalibrate_linMeas[3] = 344;

						if(Mca_installedDetector == DET_DRILLEDPROBE700){
							SetAmuletWord(91, 0);
							SetAmuletWord(92, 0xFFFF);
							SetAmuletWord(93, 0);
							SetAmuletWord(94, 0xFFFF);
							SetAmuletWord(95, 0);
							SetAmuletWord(96, 0);
							SetAmuletWord(97, 0);
							SetAmuletWord(98, 0);
							SetAmuletWord(99, 0xFFFF);
						}else{
							if(Mca_isMultiDetector() && AmuletWellMeasurementMenu_unlockDetector) SetAmuletWord(90, 0xFFFF);
							SetAmuletWord(91, 0xFFFF);
							SetAmuletWord(92, 0);
							SetAmuletWord(93, 0);
							SetAmuletWord(94, 0xFFFF);
							SetAmuletWord(95, 0xFFFF);
							SetAmuletWord(96, 0);
							SetAmuletWord(97, 0xFFFF);
							SetAmuletWord(98, 0xFFFF);
							SetAmuletWord(99, 0xFFFF);
						}
						memorycopy((uchar *) &spec_backup, (uchar *) &spec_meas, sizeof(spec_meas));
						switchPhase = PHASE_WELLAUTOCAL_WAIT;

						hvcode = Mca_getHVValue();
						if((Mca_installedDetector == DET_WELL) || (Mca_installedDetector == DET_BETA)) hv = (.158974 * hvcode) + 595.0;
						else if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)) hv = .263158 * hvcode;
						sprintf(AmuletWellAutoCalibrate_exportVolts, "%.0f", hv);

						sprintf(AmuletWellAutoCalibrate_exportThreshold, "%d", Mca_getThreshold());

						sprintf(AmuletWellAutoCalibrate_exportGain1, "%d", Mca_getGain1());
						sprintf(AmuletWellAutoCalibrate_exportGain2, "%d", Mca_getGain2());
						combinedgain1 = Mca_getGain1();
						combinedgain1 /= 64.0;
						combinedgain2 = Mca_getGain2();
						combinedgain2 /= 64.0;
						combinedgain1 *= combinedgain2;
						sprintf(AmuletWellAutoCalibrate_exportGain, "%.2f", combinedgain1);

						sprintf(AmuletWellAutoCalibrate_exportZero, "%d", Mca_getZeroOpAmpOffset());

						AmuletWellAutoCalibrate_exportStatus = 1;
						AmuletWellAutoCalibrate_exportSequence++;

						//switchPhase = PHASE_WELLAUTOCAL_START;
						//trydefaultvalues = FALSE;
						//local_auto_cal_threshold = 0;
						break;

					case 8:
						gainperchannel = Mca_getGain2();
						gainperchannel /= channeldiff;
						gainperchannel *= deltachannels;
						if(abs(gainperchannel - Mca_getGain2()) > 1.0){
							trydefaultvalues = FALSE;
#ifdef AUTOTHRESHOLD
							if(local_auto_cal_threshold == 0) peakthreshold = 2000.0;
							else peakthreshold = local_auto_cal_threshold;
#else
							peakthreshold = 2000.0;
#endif

							Mca_setGain(Mca_getInitialGain1(), Mca_getInitialGain2());
							tunephase = 0;
						}else{
							gainperchannel = Mca_getGain1();
							gainperchannel /= channeldiff;
							gainperchannel *= deltachannels;
							gain = gainperchannel;
							if(gain>=1023){
								Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nUpper gain limit reached",TRUE);
								AmuletWellAutoCalibrate_failedAutoCalExport();
								return;
								//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
							}else if(gain<=0){
								Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nLower gain limit reached",TRUE);
								AmuletWellAutoCalibrate_failedAutoCalExport();
								return;
								//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
							}else{
								Mca_setGain(gain, Mca_getGain2());
								tunephase = 1;
							}
						}
						break;

					case 999:
						break;
				}
			}else{
				gain = Mca_getGain2();
				if(gain == 1023){
					Amulet_DisplayError("AutoCalibrate Error", "Unable to AutoCalibrate\nUnable to detect two peaks",TRUE);
					AmuletWellAutoCalibrate_failedAutoCalExport();
					return;
					//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
				}else{
					gain *= 2;
					if(gain>1023) gain=1023;
					Mca_setGain(Mca_getGain1(), gain);
				}
			}

			if(switchPhase) m_iPhase = switchPhase;
			else m_iPhase = PHASE_WELLAUTOCAL_DISPLAYPARA;
			firsttryindex = 0;
			break;

		case PHASE_WELLAUTOCAL_SAVE:
			AmuletWellAutoCalibrate_clearExport();
			beep_amulet();
			Mca_saveZeroOpAmpOffsetToMirror();
			Mca_saveGainToMirror();
			Mca_saveSigmaToMirror(sigma);
			Mca_saveSigmaLipToMirror(sigma_lip);
			Mca_setCalStamp();
			if((AmuletWellAutoCalibrate_linMeas[1] != 0.0) && (AmuletWellAutoCalibrate_linMeas[2] != 0.0) && (AmuletWellAutoCalibrate_linMeas[3] != 0.0)){
				if((AmuletWellAutoCalibrate_linMeas[1] == -999.0) && (AmuletWellAutoCalibrate_linMeas[2] == -999.0) && (AmuletWellAutoCalibrate_linMeas[3] == -999.0)){
					AmuletWellAutoCalibrate_linMeas[0] = 0;
					AmuletWellAutoCalibrate_linMeas[1] = 0;
					AmuletWellAutoCalibrate_linMeas[2] = 0;
					AmuletWellAutoCalibrate_linMeas[3] = 0;
					AmuletWellAutoCalibrate_linMeas[4] = 0;
				}
			}else{
				AmuletWellAutoCalibrate_linMeas[0] = 0;
				AmuletWellAutoCalibrate_linMeas[1] = 0;
				AmuletWellAutoCalibrate_linMeas[2] = 0;
				AmuletWellAutoCalibrate_linMeas[3] = 0;
				AmuletWellAutoCalibrate_linMeas[4] = 0;
			}

			Mca_setLinearityCorrection(AmuletWellAutoCalibrate_linMeas);
			Mca_setLinStamp();

			Mca_saveCalibMirror();
			//if(measured_auto_cal_threshold){
			//	Mca_setAutoCalThresholdMirror((int) local_auto_cal_threshold);
				//DB_WriteAutoCalThreshold((int) local_auto_cal_threshold, Mca_installedDetector);
			//}
			SetAmuletBackHTML();
			break;

		case PHASE_WELLAUTOCAL_BACK:
			AmuletWellAutoCalibrate_clearExport();
			Mca_setMirrorToZeroOpAmpOffset();
			Mca_setMirrorToGain();
			SetAmuletBackHTML();
			break;

		case PHASE_WELLAUTOLIN_START:
			AmuletWellAutoCalibrate_clearExport();
			beep_amulet();
			firsttryindex = 0;
			cs137cal = CAL_EU152;
			readspectrum = TRUE;
#ifdef AUTOTHRESHOLD
			//if(local_auto_cal_threshold == 0) peakthreshold = 5000.0;
			//else peakthreshold = 2.5 * local_auto_cal_threshold;
			peakthreshold = 5000.0;
#else
			peakthreshold = 5000.0;
#endif

			//SetAmuletWord(108, 0xFFFF); // Display Stop Linearity Button
			SetAmuletWord(96, 0xFFFF);
			SetAmuletWord(99, 0xFFFF);
			m_iPhase = PHASE_WELLAUTOCAL_DISPLAYPARA;
			break;

		case PHASE_WELLAUTOLIN_CALPEAK:
			fNumOfChannels = spec_meas.num_of_channels - 1;

			Mca_getPeaks(peak, &dummynp1, TRUE);

			peakcount = 0;
			for(index=0; index<40; index++){
				if((peak[index].height>0) && (peak[index].centroid>0)){
					kev = peak[index].centroid;
					kev *= 2000.0;
					kev /= fNumOfChannels;
					if((kev > 34.0) && (kev < 380.0)) peakcount++;
				}
			}

			if(peakcount < 5){
				Amulet_DisplayError("AutoCalibrate Error", "Linearity Correction Aborted\nCannot detect Eu152 source", TRUE);
				return;
			}else{
				linstd[0] = 32.85;
				linstd[1] = 40.8;
				linstd[2] = 121.8;
				linstd[3] = 344.3;
				linstd[4] = 661.66;

				for(index=1; index<4; index++){
					olddiff = 10000.0;
					for(jndex=0; jndex<40; jndex++){
						if((peak[jndex].height>0) && (peak[jndex].centroid>0)){
							kev = peak[jndex].centroid;
							kev *= 2000.0;
							kev /= fNumOfChannels;

							diff = linstd[index] - kev;
							if(diff<0) diff *= -1.0;

							if(diff < olddiff){
								olddiff = diff;
								kndex = jndex;
							}
						}
					}

					kev = peak[kndex].centroid;
					kev *= 2000.0;
					kev /= fNumOfChannels;
					AmuletWellAutoCalibrate_linMeas[index] = kev;
				}

				for(index=0; index<5; index++){
					kev = AmuletWellAutoCalibrate_linMeas[index] - linstd[index];
					kev /= linstd[index];
					kev *= 100.0;
					sprintf(acMsg, "%d) %.2f keV", index+1, AmuletWellAutoCalibrate_linMeas[index]);
					SetAmuletString(106 + 3*index, acMsg);
					sprintf(acMsg, "%.2f %%", kev);
					SetAmuletString(108 + 3*index, acMsg);
				}
			}

			if(Mca_isMultiDetector() && AmuletWellMeasurementMenu_unlockDetector) SetAmuletWord(90, 0xFFFF);

			SetAmuletWord(91, 0xFFFF);
			SetAmuletWord(92, 0);
			SetAmuletWord(93, 0);
			SetAmuletWord(94, 0xFFFF);
			SetAmuletWord(95, 0xFFFF);
			SetAmuletWord(96, 0);
			SetAmuletWord(97, 0xFFFF);
			SetAmuletWord(98, 0xFFFF);
			SetAmuletWord(99, 0xFFFF);

			m_iPhase = PHASE_WELLAUTOCAL_WAIT;
			break;

		case PHASE_WELLAUTOCAL_ABORT_CAL:
			AmuletWellAutoCalibrate_clearExport();
			beep_amulet();
			firsttryindex = 0;
			for(index=0; index<5; index++) AmuletWellAutoCalibrate_linMeas[index] = 0.0;
			trydefaultvalues = Mca_getCalibrationStatus();
			if(trydefaultvalues){
				if((Mca_getGain1()>980) || (Mca_getGain1()<750)) trydefaultvalues = FALSE;
			}
			//SetAmuletWord(107, 0xFFFF); // Display AutoCal and Cancel Button
			if(Mca_isMultiDetector() && AmuletWellMeasurementMenu_unlockDetector) SetAmuletWord(90, 0xFFFF);
			SetAmuletWord(91, 0xFFFF);
			SetAmuletWord(98, 0xFFFF);
			SetAmuletWord(99, 0xFFFF);
			m_iPhase = PHASE_WELLAUTOCAL_WAIT;
			break;

		case PHASE_WELLAUTOCAL_ABORT_LIN:
			AmuletWellAutoCalibrate_clearExport();
			beep_amulet();
			firsttryindex = 0;
			delayloop(100);
			for(index=1; index<4; index++) AmuletWellAutoCalibrate_linMeas[index] = 0.0;

			firstpeakcentroid = AmuletWellAutoCalibrate_linMeas[0];
			sprintf(acMsg, "1st Peak: %.2f keV", firstpeakcentroid);
			SetAmuletString(106, acMsg);

			firstpeakcentroid -= 32.85;
			firstpeakcentroid /= 32.85;
			firstpeakcentroid *= 100.0;
			sprintf(acMsg, "%.2f %%", firstpeakcentroid);
			SetAmuletString(108, acMsg);

			secondpeakcentroid = AmuletWellAutoCalibrate_linMeas[4];
			sprintf(acMsg, "2nd Peak: %.2f keV", secondpeakcentroid);
			SetAmuletString(109, acMsg);

			secondpeakcentroid -= 661.66;
			secondpeakcentroid /= 661.66;
			secondpeakcentroid *= 100.0;
			sprintf(acMsg, "%.2f %%", secondpeakcentroid);
			SetAmuletString(111, acMsg);

			if(sigma_lip == 0){
				fwhm = GetFwhm(sigma);
				sprintf(acMsg, "FWHM: %.3f%%", fwhm);
				SetAmuletString(112, acMsg);
				SetAmuletString(114, "");

				SetAmuletString(115, "");
				SetAmuletString(117, "");

				SetAmuletString(118, "");
				SetAmuletString(120, "");
			}else{
				SetAmuletString(112, "CENTER FWHM:");
				fwhm = GetFwhm(sigma);
				sprintf(acMsg, "%.3f%%", fwhm);
				SetAmuletString(114, acMsg);

				SetAmuletString(115, "PERIMETER FWHM:");
				fwhm = GetFwhm(sigma_lip);
				sprintf(acMsg, "%.3f%%", fwhm);
				SetAmuletString(117, acMsg);

				SetAmuletString(118, "");
				SetAmuletString(120, "");
			}

			if(Mca_isMultiDetector() && AmuletWellMeasurementMenu_unlockDetector) SetAmuletWord(90, 0xFFFF);
			SetAmuletWord(91, 0xFFFF);
			SetAmuletWord(92, 0);
			SetAmuletWord(93, 0);
			SetAmuletWord(94, 0xFFFF);
			SetAmuletWord(95, 0xFFFF);
			SetAmuletWord(96, 0);
			SetAmuletWord(97, 0xFFFF);
			SetAmuletWord(98, 0xFFFF);
			SetAmuletWord(99, 0xFFFF);
			delayloop(100);
			readspectrum = FALSE;
			memorycopy((uchar *) &spec_meas, (uchar *) &spec_backup, sizeof(spec_meas));
			//m_iPhase = PHASE_WELLAUTOCAL_WAIT;
			m_iPhase =PHASE_WELLAUTOCAL_CREATE512GRAPH;
			break;

		case PHASE_WELLAUTOCAL2_START:
			AmuletWellAutoCalibrate_clearExport();
			beep_amulet();
			firsttryindex = 0;
			cs137cal = CAL_CS137_2;
			readspectrum = TRUE;
#ifdef AUTOTHRESHOLD
			//if(local_auto_cal_threshold == 0) peakthreshold = 5000.0;
			//else peakthreshold = 2.5 * local_auto_cal_threshold;
			peakthreshold = 5000.0;
#else
			peakthreshold = 5000.0;
#endif

			SetAmuletWord(93, 0xFFFF);
			SetAmuletWord(99, 0xFFFF);
			m_iPhase = PHASE_WELLAUTOCAL_DISPLAYPARA;
			break;

		case PHASE_WELLAUTOCAL2_CALCPEAK:
			// sigma in channels
			sigmach = InterpSigma();

			maxenergy = 2000;
			maxchannel = spec_meas.num_of_channels - 1;
			sigma_lip = sigmach * maxenergy;
			sigma_lip /= maxchannel;

			//Mca_getROIChannelsWithoutCorrection(661.66 - (2.15 * sigma_lip), 661.66 + (2.15 * sigma_lip), &calcStart, &calcEnd, spec_meas.num_of_channels);

			//Mca_drawROI(calcStart, calcEnd, display, spec_meas.num_of_channels);

			firstpeakcentroid = AmuletWellAutoCalibrate_linMeas[0];
			sprintf(acMsg, "1st Peak: %.2f keV", firstpeakcentroid);
			SetAmuletString(106, acMsg);

			firstpeakcentroid -= 32.85;
			firstpeakcentroid /= 32.85;
			firstpeakcentroid *= 100.0;
			sprintf(acMsg, "%.2f %%", firstpeakcentroid);
			SetAmuletString(108, acMsg);

			secondpeakcentroid = AmuletWellAutoCalibrate_linMeas[4];
			sprintf(acMsg, "2nd Peak: %.2f keV", secondpeakcentroid);
			SetAmuletString(109, acMsg);

			secondpeakcentroid -= 661.66;
			secondpeakcentroid /= 661.66;
			secondpeakcentroid *= 100.0;
			sprintf(acMsg, "%.2f %%", secondpeakcentroid);
			SetAmuletString(111, acMsg);

			SetAmuletString(112, "CENTER FWHM:");
			fwhm = GetFwhm(sigma);
			sprintf(acMsg, "%.3f%%", fwhm);
			SetAmuletString(114, acMsg);


			SetAmuletString(115, "PERIMETER FWHM:");
			fwhm = GetFwhm(sigma_lip);
			sprintf(acMsg, "%.3f%%", fwhm);
			SetAmuletString(117, acMsg);

			SetAmuletString(118, "");
			SetAmuletString(120, "");

			if(Mca_isMultiDetector() && AmuletWellMeasurementMenu_unlockDetector) SetAmuletWord(90, 0xFFFF);
			SetAmuletWord(91, 0xFFFF);
			SetAmuletWord(92, 0);
			SetAmuletWord(93, 0);
			SetAmuletWord(94, 0xFFFF);
			SetAmuletWord(95, 0xFFFF);
			SetAmuletWord(96, 0);
			SetAmuletWord(97, 0xFFFF);
			SetAmuletWord(98, 0xFFFF);
			SetAmuletWord(99, 0xFFFF);
			memorycopy((uchar *) &spec_backup, (uchar *) &spec_meas, sizeof(spec_meas));
			m_iPhase = PHASE_WELLAUTOCAL_WAIT;
			break;
	}
}

float InterpSigma(void){
	short index, startCh, endCh, peakCh, aboveHalfX, belowHalfX;
	float peak, halfPeak, aboveHalfY, belowHalfY, slope;
	float halfX1, halfX2, fwhm, sigma;

	switch(spec_meas.num_of_channels){
		case 256:
			startCh = 76;
			endCh = 92;
			break;

		case 512:
			startCh = 152;
			endCh = 184;
			break;

		case 1024:
			startCh = 304;
			endCh = 368;
			break;

		case 2048:
			startCh = 608;
			endCh = 736;
			break;

		case 4096:
			startCh = 1216;
			endCh = 1472;
			break;
	}

	peakCh = startCh;
	peak = spec_meas.spectrum[startCh];

	for(index=startCh; index<=endCh; index++){
		if(spec_meas.spectrum[index] > peak){
			peak = spec_meas.spectrum[index];
			peakCh = index;
		}
	}

	halfPeak = peak/2;

	// left of pwak
	// get points before and after Half
	for(index=startCh; index<=peakCh; index++){
		if(spec_meas.spectrum[index] > halfPeak){
			aboveHalfX = index;
			aboveHalfY = spec_meas.spectrum[aboveHalfX];
			belowHalfX = index - 1;
			belowHalfY = spec_meas.spectrum[belowHalfX];
			break;
		}
	}

	slope = aboveHalfY - belowHalfY; // / aboveHalfX - belowHalfY or 1
	halfX1 = ((halfPeak - belowHalfY)/slope) + belowHalfX;

	for(index=endCh; index>=peakCh; index--){
		if(spec_meas.spectrum[index]>halfPeak){
			aboveHalfX = index;
			aboveHalfY = spec_meas.spectrum[aboveHalfX];
			belowHalfX = index + 1;
			belowHalfY = spec_meas.spectrum[belowHalfX];
			break;
		}
	}

	slope = belowHalfY - aboveHalfY; // / belowHalfX - aboveHalfX or 1
	halfX2 = ((halfPeak - aboveHalfY)/slope) + aboveHalfX;

	fwhm = halfX2 - halfX1;
	sigma = fwhm / 2.3548;

	return sigma;
}

float GetFwhm(float sigma){
	float fwhm;

	// returns FWHM in % as function of sigma
	fwhm = 235.48 * sigma / 661.66;
	return fwhm;
}

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crc.h"
#include "mca.h"
#include "database.h"
#include "nuc.h"

static PROBEBIOASSAYEFFICIENCY currentProbeBioAssayEfficiency;
static PROBEBIOASSAYSETTING currentProbeBioAssaySetting;

float I131Lower_keV, I131Upper_keV, I125Lower_keV, I125Upper_keV, I123Lower_keV, I123Upper_keV;

void BioAssayInitialize(void){
	/*I131Lower_keV = 292.0;
	I131Upper_keV = 437.0;
	I125Lower_keV = 20.0;
	I125Upper_keV = 50.0;
	I123Lower_keV = 143.0;
	I123Upper_keV = 191.0;*/
	DB_ReadBioAssayROI(&I131Lower_keV, &I131Upper_keV, &I125Lower_keV, &I125Upper_keV, &I123Lower_keV, &I123Upper_keV);
	/*I131Lower_keV = 27.4;
	I131Upper_keV = 58.71;
	I125Lower_keV = 109.59;
	I125Upper_keV = 148.73;
	I123Lower_keV = 324.85;
	I123Upper_keV = 375.73;*/
	DB_ReadCurrentProbeBioAssayEfficiency(&currentProbeBioAssayEfficiency);
	DB_ReadCurrentProbeBioAssaySetting(&currentProbeBioAssaySetting);
}

long long int BioAssayEfficiencyID(void){
	return currentProbeBioAssayEfficiency.ProbeBioAssayEfficiencyID;
}

float BioAssayI131Efficiency(void){
	return currentProbeBioAssayEfficiency.I131Efficiency;
}

float BioAssayI125Efficiency(void){
	return currentProbeBioAssayEfficiency.I125Efficiency;
}

float BioAssayI123Efficiency(void){
	return currentProbeBioAssayEfficiency.I123Efficiency;
}

float BioAssayI131I125Contamination(void){
	return currentProbeBioAssayEfficiency.I131I125Contamination;
}

float BioAssayI123I125Contamination(void){
	return currentProbeBioAssayEfficiency.I123I125Contamination;
}

long long int BioAssaySettingID(void){
	return currentProbeBioAssaySetting.ProbeBioAssaySettingID;
}

bool BioAssayI131Active(void){
	return currentProbeBioAssaySetting.I131Active;
}

bool BioAssayI125Active(void){
	return currentProbeBioAssaySetting.I125Active;
}

bool BioAssayI123Active(void){
	return currentProbeBioAssaySetting.I123Active;
}

float BioAssayI131Threshold(void){
	return currentProbeBioAssaySetting.I131Threshold;
}

float BioAssayI125Threshold(void){
	return currentProbeBioAssaySetting.I125Threshold;
}

float BioAssayI123Threshold(void){
	return currentProbeBioAssaySetting.I123Threshold;
}

int BioAssayCountTime(void){
	return currentProbeBioAssaySetting.CountTime;
}

int BioAssayProbeDistance(void){
	return currentProbeBioAssaySetting.ProbeDistance;
}

void BioAssaySaveSetting(PROBEBIOASSAYSETTING *setting){
	if(setting->I131Threshold <= 0.0) setting->I131Threshold = -1.0;
	if(setting->I125Threshold <= 0.0) setting->I125Threshold = -1.0;
	if(setting->I123Threshold <= 0.0) setting->I123Threshold = -1.0;

	if((setting->CountTime != currentProbeBioAssaySetting.CountTime) ||
	   (setting->ProbeDistance != currentProbeBioAssaySetting.ProbeDistance) ||
	   (setting->I131Active != currentProbeBioAssaySetting.I131Active) ||
	   (setting->I125Active != currentProbeBioAssaySetting.I125Active) ||
	   (setting->I123Active != currentProbeBioAssaySetting.I123Active) ||
	   (setting->I131Threshold != currentProbeBioAssaySetting.I131Threshold) ||
	   (setting->I125Threshold != currentProbeBioAssaySetting.I125Threshold) ||
	   (setting->I123Threshold != currentProbeBioAssaySetting.I123Threshold)
	){
		DB_WriteProbeBioAssaySetting(setting, TRUE);
		if(setting->ProbeBioAssaySettingID >= 0){
			memcpy(&currentProbeBioAssaySetting, setting, sizeof(PROBEBIOASSAYSETTING));
		}
	}else{
		setting->ProbeBioAssaySettingID = currentProbeBioAssaySetting.ProbeBioAssaySettingID;
		setting->Inactive = FALSE;
	}
}

void BioAssayCopyCurrentEfficiency(PROBEBIOASSAYEFFICIENCY *eff){
	memcpy(eff, &currentProbeBioAssayEfficiency, sizeof(PROBEBIOASSAYEFFICIENCY));
}

void BioAssaySaveEfficiency(PROBEBIOASSAYEFFICIENCY *eff, bool flgEnter){
	if(flgEnter){
		if(eff->I131Efficiency <= 0.0) eff->I131Efficiency = -1.0;
		if(eff->I125Efficiency <= 0.0) eff->I125Efficiency = -1.0;
		if(eff->I123Efficiency <= 0.0) eff->I123Efficiency = -1.0;
		if(eff->I131I125Contamination <= 0.0) eff->I131I125Contamination = -1.0;
		if(eff->I123I125Contamination <= 0.0) eff->I123I125Contamination = -1.0;

		if((eff->I131Efficiency != currentProbeBioAssayEfficiency.I131Efficiency) || (eff->I131I125Contamination != currentProbeBioAssayEfficiency.I131I125Contamination)){
			eff->I131Measured = FALSE;
			eff->I131Activity = -1;
			eff->I131Date = (time_t) 0;
			eff->I131PrimaryEnergy = -1;
			eff->I131StartEV = 0;
			eff->I131EndEV = 0;
			eff->I131I125StartEV = 0;
			eff->I131I125EndEV = 0;
			eff->I131BackgroundID = -1;
			eff->I131SpectraID = -1;
			eff->I131ROICounts = 0;
			eff->I131ROICPM = 0;
			eff->I131BackgroundROICounts = 0;
			eff->I131BackgroundROICPM = 0;
			eff->I131NetROICPM = 0;
			eff->I131I125ROICounts = 0;
			eff->I131I125ROICPM = 0;
			eff->I131I125BackgroundROICounts = 0;
			eff->I131I125BackgroundROICPM = 0;
			eff->I131I125NetROICPM = 0;
		}

		if(eff->I125Efficiency != currentProbeBioAssayEfficiency.I125Efficiency){
			eff->I125Measured = FALSE;
			eff->I125Activity = -1;
			eff->I125Date = (time_t) 0;
			eff->I125PrimaryEnergy = -1;
			eff->I125StartEV = 0;
			eff->I125EndEV = 0;
			eff->I125BackgroundID = -1;
			eff->I125SpectraID = -1;
			eff->I125ROICounts = 0;
			eff->I125ROICPM = 0;
			eff->I125BackgroundROICounts = 0;
			eff->I125BackgroundROICPM = 0;
			eff->I125NetROICPM = 0;
		}

		if((eff->I123Efficiency != currentProbeBioAssayEfficiency.I123Efficiency) || (eff->I123I125Contamination != currentProbeBioAssayEfficiency.I123I125Contamination)){
			eff->I123Measured = FALSE;
			eff->I123Activity = -1;
			eff->I123Date = (time_t) 0;
			eff->I123PrimaryEnergy = -1;
			eff->I123StartEV = 0;
			eff->I123EndEV = 0;
			eff->I123I125StartEV = 0;
			eff->I123I125EndEV = 0;
			eff->I123BackgroundID = -1;
			eff->I123SpectraID = -1;
			eff->I123ROICounts = 0;
			eff->I123ROICPM = 0;
			eff->I123BackgroundROICounts = 0;
			eff->I123BackgroundROICPM = 0;
			eff->I123NetROICPM = 0;
			eff->I123I125ROICounts = 0;
			eff->I123I125ROICPM = 0;
			eff->I123I125BackgroundROICounts = 0;
			eff->I123I125BackgroundROICPM = 0;
			eff->I123I125NetROICPM = 0;
		}

		DB_WriteProbeBioAssayEfficiency(eff, TRUE);

		if(eff->ProbeBioAssayEfficiencyID >= 0){
			memcpy(&currentProbeBioAssayEfficiency, eff, sizeof(PROBEBIOASSAYEFFICIENCY));
		}
	}else{
		if(eff->I131Efficiency != -1.0){
			currentProbeBioAssayEfficiency.I131Efficiency = eff->I131Efficiency;
			currentProbeBioAssayEfficiency.I131I125Contamination = eff->I131I125Contamination;
			currentProbeBioAssayEfficiency.I131Measured = TRUE;
			currentProbeBioAssayEfficiency.I131Activity = eff->I131Activity;
			currentProbeBioAssayEfficiency.I131Date = eff->I131Date;
			currentProbeBioAssayEfficiency.I131PrimaryEnergy = eff->I131PrimaryEnergy;
			currentProbeBioAssayEfficiency.I131StartEV = eff->I131StartEV;
			currentProbeBioAssayEfficiency.I131EndEV = eff->I131EndEV;
			currentProbeBioAssayEfficiency.I131I125StartEV = eff->I131I125StartEV;
			currentProbeBioAssayEfficiency.I131I125EndEV = eff->I131I125EndEV;
			currentProbeBioAssayEfficiency.I131BackgroundID = eff->I131BackgroundID;
			currentProbeBioAssayEfficiency.I131SpectraID = DB_WriteSpectrum(&(eff->I131Spectrum), TRUE);
			currentProbeBioAssayEfficiency.I131ROICounts = eff->I131ROICounts;
			currentProbeBioAssayEfficiency.I131ROICPM = eff->I131ROICPM;
			currentProbeBioAssayEfficiency.I131BackgroundROICounts = eff->I131BackgroundROICounts;
			currentProbeBioAssayEfficiency.I131BackgroundROICPM = eff->I131BackgroundROICPM;
			currentProbeBioAssayEfficiency.I131NetROICPM = eff->I131NetROICPM;
			currentProbeBioAssayEfficiency.I131I125ROICounts = eff->I131I125ROICounts;
			currentProbeBioAssayEfficiency.I131I125ROICPM = eff->I131I125ROICPM;
			currentProbeBioAssayEfficiency.I131I125BackgroundROICounts = eff->I131I125BackgroundROICounts;
			currentProbeBioAssayEfficiency.I131I125BackgroundROICPM = eff->I131I125BackgroundROICPM;
			currentProbeBioAssayEfficiency.I131I125NetROICPM = eff->I131I125NetROICPM;
		}

		if(eff->I125Efficiency != -1.0){
			currentProbeBioAssayEfficiency.I125Efficiency = eff->I125Efficiency;
			currentProbeBioAssayEfficiency.I125Measured = TRUE;
			currentProbeBioAssayEfficiency.I125Activity = eff->I125Activity;
			currentProbeBioAssayEfficiency.I125Date = eff->I125Date;
			currentProbeBioAssayEfficiency.I125PrimaryEnergy = eff->I125PrimaryEnergy;
			currentProbeBioAssayEfficiency.I125StartEV = eff->I125StartEV;
			currentProbeBioAssayEfficiency.I125EndEV = eff->I125EndEV;
			currentProbeBioAssayEfficiency.I125BackgroundID = eff->I125BackgroundID;
			currentProbeBioAssayEfficiency.I125SpectraID = DB_WriteSpectrum(&(eff->I125Spectrum), TRUE);
			currentProbeBioAssayEfficiency.I125ROICounts = eff->I125ROICounts;
			currentProbeBioAssayEfficiency.I125ROICPM = eff->I125ROICPM;
			currentProbeBioAssayEfficiency.I125BackgroundROICounts = eff->I125BackgroundROICounts;
			currentProbeBioAssayEfficiency.I125BackgroundROICPM = eff->I125BackgroundROICPM;
			currentProbeBioAssayEfficiency.I125NetROICPM = eff->I125NetROICPM;
		}

		if(eff->I123Efficiency != -1.0){
			currentProbeBioAssayEfficiency.I123Efficiency = eff->I123Efficiency;
			currentProbeBioAssayEfficiency.I123I125Contamination = eff->I123I125Contamination;
			currentProbeBioAssayEfficiency.I123Measured = TRUE;
			currentProbeBioAssayEfficiency.I123Activity = eff->I123Activity;
			currentProbeBioAssayEfficiency.I123Date = eff->I123Date;
			currentProbeBioAssayEfficiency.I123PrimaryEnergy = eff->I123PrimaryEnergy;
			currentProbeBioAssayEfficiency.I123StartEV = eff->I123StartEV;
			currentProbeBioAssayEfficiency.I123EndEV = eff->I123EndEV;
			currentProbeBioAssayEfficiency.I123I125StartEV = eff->I123I125StartEV;
			currentProbeBioAssayEfficiency.I123I125EndEV = eff->I123I125EndEV;
			currentProbeBioAssayEfficiency.I123BackgroundID = eff->I123BackgroundID;
			currentProbeBioAssayEfficiency.I123SpectraID = DB_WriteSpectrum(&(eff->I123Spectrum), TRUE);
			currentProbeBioAssayEfficiency.I123ROICounts = eff->I123ROICounts;
			currentProbeBioAssayEfficiency.I123ROICPM = eff->I123ROICPM;
			currentProbeBioAssayEfficiency.I123BackgroundROICounts = eff->I123BackgroundROICounts;
			currentProbeBioAssayEfficiency.I123BackgroundROICPM = eff->I123BackgroundROICPM;
			currentProbeBioAssayEfficiency.I123NetROICPM = eff->I123NetROICPM;
			currentProbeBioAssayEfficiency.I123I125ROICounts = eff->I123I125ROICounts;
			currentProbeBioAssayEfficiency.I123I125ROICPM = eff->I123I125ROICPM;
			currentProbeBioAssayEfficiency.I123I125BackgroundROICounts = eff->I123I125BackgroundROICounts;
			currentProbeBioAssayEfficiency.I123I125BackgroundROICPM = eff->I123I125BackgroundROICPM;
			currentProbeBioAssayEfficiency.I123I125NetROICPM = eff->I123I125NetROICPM;
		}

		DB_WriteProbeBioAssayEfficiency(&currentProbeBioAssayEfficiency, TRUE);
	}
}

void BioAssayUpdateEfficiencyFromROI(float I131StartkeV, float I131EndkeV, float I125StartkeV, float I125EndkeV, float I123StartkeV, float I123EndkeV){
	bool Changed;
	BACKGND background;
	short halfLifeUnit, nucIndex, startCh, endCh;
	float halfLife, activity, dpm;

	Changed = FALSE;
	if(currentProbeBioAssayEfficiency.I131Measured){
		if((currentProbeBioAssayEfficiency.I131StartEV != I131StartkeV) ||
		   (currentProbeBioAssayEfficiency.I131EndEV != I131EndkeV) ||
		   (currentProbeBioAssayEfficiency.I131I125StartEV != I125StartkeV) ||
		   (currentProbeBioAssayEfficiency.I131I125EndEV != I125EndkeV)
		  ){
			currentProbeBioAssayEfficiency.I131StartEV = I131StartkeV;
			currentProbeBioAssayEfficiency.I131EndEV = I131EndkeV;
			currentProbeBioAssayEfficiency.I131I125StartEV = I125StartkeV;
			currentProbeBioAssayEfficiency.I131I125EndEV = I125EndkeV;

			background.WellBackgroundID = currentProbeBioAssayEfficiency.I131BackgroundID;
			DB_RetrieveBackgnd(&background);
			currentProbeBioAssayEfficiency.I131Spectrum.SpectraID = currentProbeBioAssayEfficiency.I131SpectraID;
			DB_ReadSpectrum(&(currentProbeBioAssayEfficiency.I131Spectrum));

			nucIndex = NuclideData_getIndexFromName("I 131");
			halfLife = NuclideData_getHalflife(nucIndex);
			halfLifeUnit = NuclideData_getHalflifeUnit(nucIndex);
			activity = nucdecay(currentProbeBioAssayEfficiency.I131Activity, currentProbeBioAssayEfficiency.I131Date, currentProbeBioAssayEfficiency.I131Spectrum.MeasuredOn, halfLife, halfLifeUnit);
			dpm = Mca_convertCiToDpm(activity);
			if(dpm >= 1){
				Mca_getROIChannels(currentProbeBioAssayEfficiency.I131StartEV, currentProbeBioAssayEfficiency.I131EndEV, &startCh, &endCh, &(currentProbeBioAssayEfficiency.I131Spectrum.AutoCal));
				currentProbeBioAssayEfficiency.I131ROICounts = Mca_getROICounts(currentProbeBioAssayEfficiency.I131Spectrum.DecompressedSpectra, startCh, endCh);
				currentProbeBioAssayEfficiency.I131ROICPM = currentProbeBioAssayEfficiency.I131ROICounts;
				currentProbeBioAssayEfficiency.I131ROICPM /= currentProbeBioAssayEfficiency.I131Spectrum.LiveTime;
				currentProbeBioAssayEfficiency.I131ROICPM *= 60.0;
				currentProbeBioAssayEfficiency.I131BackgroundROICounts = Mca_getROIBackgroundCounts(startCh, endCh, &background);
				currentProbeBioAssayEfficiency.I131BackgroundROICPM = Mca_getROIBackgroundCPM(startCh, endCh, &background);
				currentProbeBioAssayEfficiency.I131NetROICPM = currentProbeBioAssayEfficiency.I131ROICPM - currentProbeBioAssayEfficiency.I131BackgroundROICPM;

				Mca_getROIChannels(currentProbeBioAssayEfficiency.I131I125StartEV, currentProbeBioAssayEfficiency.I131I125EndEV, &startCh, &endCh, &(currentProbeBioAssayEfficiency.I131Spectrum.AutoCal));
				currentProbeBioAssayEfficiency.I131I125ROICounts = Mca_getROICounts(currentProbeBioAssayEfficiency.I131Spectrum.DecompressedSpectra, startCh, endCh);
				currentProbeBioAssayEfficiency.I131I125ROICPM = currentProbeBioAssayEfficiency.I131I125ROICounts;
				currentProbeBioAssayEfficiency.I131I125ROICPM /= currentProbeBioAssayEfficiency.I131Spectrum.LiveTime;
				currentProbeBioAssayEfficiency.I131I125ROICPM *= 60.0;
				currentProbeBioAssayEfficiency.I131I125BackgroundROICounts = Mca_getROIBackgroundCounts(startCh, endCh, &background);
				currentProbeBioAssayEfficiency.I131I125BackgroundROICPM = Mca_getROIBackgroundCPM(startCh, endCh, &background);
				currentProbeBioAssayEfficiency.I131I125NetROICPM = currentProbeBioAssayEfficiency.I131I125ROICPM - currentProbeBioAssayEfficiency.I131I125BackgroundROICPM;

				currentProbeBioAssayEfficiency.I131Efficiency = 100.0 * currentProbeBioAssayEfficiency.I131NetROICPM / dpm;
				currentProbeBioAssayEfficiency.I131I125Contamination = currentProbeBioAssayEfficiency.I131I125NetROICPM / currentProbeBioAssayEfficiency.I131NetROICPM;
				Changed = TRUE;
			}
		}
	}

	if(currentProbeBioAssayEfficiency.I125Measured){
		if((currentProbeBioAssayEfficiency.I125StartEV != I125StartkeV) || (currentProbeBioAssayEfficiency.I125EndEV != I125EndkeV)){
			currentProbeBioAssayEfficiency.I125StartEV = I125StartkeV;
			currentProbeBioAssayEfficiency.I125EndEV = I125EndkeV;

			background.WellBackgroundID = currentProbeBioAssayEfficiency.I125BackgroundID;
			DB_RetrieveBackgnd(&background);
			currentProbeBioAssayEfficiency.I125Spectrum.SpectraID = currentProbeBioAssayEfficiency.I125SpectraID;
			DB_ReadSpectrum(&(currentProbeBioAssayEfficiency.I125Spectrum));

			nucIndex = NuclideData_getIndexFromName("I 125");
			halfLife = NuclideData_getHalflife(nucIndex);
			halfLifeUnit = NuclideData_getHalflifeUnit(nucIndex);
			activity = nucdecay(currentProbeBioAssayEfficiency.I125Activity, currentProbeBioAssayEfficiency.I125Date, currentProbeBioAssayEfficiency.I125Spectrum.MeasuredOn, halfLife, halfLifeUnit);
			dpm = Mca_convertCiToDpm(activity);
			if(dpm >= 1){
				Mca_getROIChannels(currentProbeBioAssayEfficiency.I125StartEV, currentProbeBioAssayEfficiency.I125EndEV, &startCh, &endCh, &currentProbeBioAssayEfficiency.I125Spectrum.AutoCal);
				currentProbeBioAssayEfficiency.I125ROICounts = Mca_getROICounts(currentProbeBioAssayEfficiency.I125Spectrum.DecompressedSpectra, startCh, endCh);
				currentProbeBioAssayEfficiency.I125ROICPM = currentProbeBioAssayEfficiency.I125ROICounts;
				currentProbeBioAssayEfficiency.I125ROICPM /= currentProbeBioAssayEfficiency.I125Spectrum.LiveTime;
				currentProbeBioAssayEfficiency.I125ROICPM *= 60.0;
				currentProbeBioAssayEfficiency.I125BackgroundROICounts = Mca_getROIBackgroundCounts(startCh, endCh, &background);
				currentProbeBioAssayEfficiency.I125BackgroundROICPM = Mca_getROIBackgroundCPM(startCh, endCh, &background);
				currentProbeBioAssayEfficiency.I125NetROICPM = currentProbeBioAssayEfficiency.I125ROICPM - currentProbeBioAssayEfficiency.I125BackgroundROICPM;

				currentProbeBioAssayEfficiency.I125Efficiency = 100.0 * currentProbeBioAssayEfficiency.I125NetROICPM / dpm;
				Changed = TRUE;
			}
		}
	}

	if(currentProbeBioAssayEfficiency.I123Measured){
		if((currentProbeBioAssayEfficiency.I123StartEV != I123StartkeV) ||
		   (currentProbeBioAssayEfficiency.I123EndEV != I123EndkeV) ||
		   (currentProbeBioAssayEfficiency.I123I125StartEV != I125StartkeV) ||
		   (currentProbeBioAssayEfficiency.I123I125EndEV != I125EndkeV)
		  ){
			currentProbeBioAssayEfficiency.I123StartEV = I123StartkeV;
			currentProbeBioAssayEfficiency.I123EndEV = I123EndkeV;
			currentProbeBioAssayEfficiency.I123I125StartEV = I125StartkeV;
			currentProbeBioAssayEfficiency.I123I125EndEV = I125EndkeV;

			background.WellBackgroundID = currentProbeBioAssayEfficiency.I123BackgroundID;
			DB_RetrieveBackgnd(&background);
			currentProbeBioAssayEfficiency.I123Spectrum.SpectraID = currentProbeBioAssayEfficiency.I123SpectraID;
			DB_ReadSpectrum(&(currentProbeBioAssayEfficiency.I123Spectrum));

			nucIndex = NuclideData_getIndexFromName("I 123");
			halfLife = NuclideData_getHalflife(nucIndex);
			halfLifeUnit = NuclideData_getHalflifeUnit(nucIndex);
			activity = nucdecay(currentProbeBioAssayEfficiency.I123Activity, currentProbeBioAssayEfficiency.I123Date, currentProbeBioAssayEfficiency.I123Spectrum.MeasuredOn, halfLife, halfLifeUnit);
			dpm = Mca_convertCiToDpm(activity);
			if(dpm >= 1){
				Mca_getROIChannels(currentProbeBioAssayEfficiency.I123StartEV, currentProbeBioAssayEfficiency.I123EndEV, &startCh, &endCh, &currentProbeBioAssayEfficiency.I123Spectrum.AutoCal);
				currentProbeBioAssayEfficiency.I123ROICounts = Mca_getROICounts(currentProbeBioAssayEfficiency.I123Spectrum.DecompressedSpectra, startCh, endCh);
				currentProbeBioAssayEfficiency.I123ROICPM = currentProbeBioAssayEfficiency.I123ROICounts;
				currentProbeBioAssayEfficiency.I123ROICPM /= currentProbeBioAssayEfficiency.I123Spectrum.LiveTime;
				currentProbeBioAssayEfficiency.I123ROICPM *= 60.0;
				currentProbeBioAssayEfficiency.I123BackgroundROICounts = Mca_getROIBackgroundCounts(startCh, endCh, &background);
				currentProbeBioAssayEfficiency.I123BackgroundROICPM = Mca_getROIBackgroundCPM(startCh, endCh, &background);
				currentProbeBioAssayEfficiency.I123NetROICPM = currentProbeBioAssayEfficiency.I123ROICPM - currentProbeBioAssayEfficiency.I123BackgroundROICPM;
				Mca_getROIChannels(currentProbeBioAssayEfficiency.I123I125StartEV, currentProbeBioAssayEfficiency.I123I125EndEV, &startCh, &endCh, &currentProbeBioAssayEfficiency.I123Spectrum.AutoCal);
				currentProbeBioAssayEfficiency.I123I125ROICounts = Mca_getROICounts(currentProbeBioAssayEfficiency.I123Spectrum.DecompressedSpectra, startCh, endCh);
				currentProbeBioAssayEfficiency.I123I125ROICPM = currentProbeBioAssayEfficiency.I123I125ROICounts;
				currentProbeBioAssayEfficiency.I123I125ROICPM /= currentProbeBioAssayEfficiency.I123Spectrum.LiveTime;
				currentProbeBioAssayEfficiency.I123I125ROICPM *= 60.0;
				currentProbeBioAssayEfficiency.I123I125BackgroundROICounts = Mca_getROIBackgroundCounts(startCh, endCh, &background);
				currentProbeBioAssayEfficiency.I123I125BackgroundROICPM = Mca_getROIBackgroundCPM(startCh, endCh, &background);
				currentProbeBioAssayEfficiency.I123I125NetROICPM = currentProbeBioAssayEfficiency.I123I125ROICPM - currentProbeBioAssayEfficiency.I123I125BackgroundROICPM;

				currentProbeBioAssayEfficiency.I123Efficiency = 100.0 * currentProbeBioAssayEfficiency.I123NetROICPM / dpm;
				currentProbeBioAssayEfficiency.I123I125Contamination = currentProbeBioAssayEfficiency.I123I125NetROICPM / currentProbeBioAssayEfficiency.I123NetROICPM;
			}
		}
	}

	if(Changed){
		DB_WriteProbeBioAssayEfficiency(&currentProbeBioAssayEfficiency, TRUE);
	}
}

void BioAssayWriteROI(void){
	DB_WriteBioAssayROI(I131Lower_keV, I131Upper_keV, I125Lower_keV, I125Upper_keV, I123Lower_keV, I123Upper_keV);
}

void BioAssaySaveTest(PROBEBIOASSAYTEST *test){
	test->SpectraID = DB_WriteSpectrum(&(test->Spectrum), TRUE);
	if(test->SpectraID != -1) DB_WriteProbeBioAssayTest(test, TRUE);
}

#define PHASE_WELL_MDA_TEST_PRE_INIT	0
#define PHASE_WELL_MDA_TEST_WAIT		1
#define PHASE_WELL_MDA_TEST_CALC		2
#define PHASE_WELL_MDA_TEST_PRINT		3
#define PHASE_WELL_MDA_TEST_SPECTRUM	4
#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "nuc.h"
#include "math.h"
#include "printer.h"
#include "amulet.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;

char AmuletWellMDATestMenu_nuclideID;
WELLMDATEST AmuletWellMDATestMenu_mda;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void PushPageStack(unsigned char ucPage);
void AmuletWellMDATest_print(void);

void AmuletWellMDATest_menu(void){
	short index;
	short startCh, endCh;
	float mda, liveTime;
	char message[51];
	float cps, numerator, denominator;
	double counts, cnts;

	switch(m_iPhase){
		case PHASE_WELL_MDA_TEST_PRE_INIT:
			if(m_ucClear == 45){
				AmuletWellMDATestMenu_mda.Spectrum.SpectraID = 0;
				AmuletWellMDATestMenu_mda.Spectrum.LiveTime = 0;
				AmuletWellMDATestMenu_mda.Spectrum.RealTime = 0;
				AmuletWellMDATestMenu_mda.Spectrum.MeasuredOn = 0;
				//Modified
				for(index=0; index<4096; index++) AmuletWellMDATestMenu_mda.Spectrum.DecompressedSpectra[index] = 0;
				AmuletWellMDATestMenu_mda.PrecisionFactor = 3;
				AmuletWellMDATestMenu_mda.CorrectionFactor = 0;
				AmuletWellMDATestMenu_nuclideID = -1;
				m_ucClear = 0;
			}

			switch(AmuletWellMDATestMenu_mda.DetectorTest){
				case DET_WELL:
					strcpy(message, "WELL");
					break;

				case DET_WELL700:
					strcpy(message, "Well");
					break;

				case DET_BETA:
					strcpy(message, "Beta");
					break;

				case DET_PROBE700:
					strcpy(message, "Probe");
					break;

				case DET_DRILLEDPROBE700:
					strcpy(message, "Drilled Probe");
					break;

				case DET_DRILLEDPROBE700_AS_WELL:
					strcpy(message, "Drilled Probe As Well");
					break;

				case DET_DRILLEDPROBE700_AS_PROBE:
					strcpy(message, "Drilled Probe As Probe");
					break;
			}
			SetAmuletString(9, message);

			if(AmuletWellMDATestMenu_mda.Spectrum.MeasuredOn != (time_t)0){
				sprintf(message, "Measurement Time %.1f sec, Total Rate:", AmuletWellMDATestMenu_mda.Spectrum.LiveTime);
				SetAmuletString(101, message);
				cps = AmuletWellMDATestMenu_mda.Spectrum.TotalCPM / 60.0;
				if(current.system == CI) sprintf(message, "%.0f cpm", AmuletWellMDATestMenu_mda.Spectrum.TotalCPM);
				else sprintf(message, "%.0f cps", cps);
				SetAmuletString(103, message);
			}else{
				SetAmuletString(101, "Measured Background:");
			}

			if(AmuletWellMDATestMenu_nuclideID != -1){
				NuclideData_getName(AmuletWellMDATestMenu_nuclideID, message);
				SetAmuletString(104, message);
			}

			sprintf(message, "%.2f", AmuletWellMDATestMenu_mda.PrecisionFactor);
			SetAmuletString(105, message);

			sprintf(message, "%.2f", AmuletWellMDATestMenu_mda.CorrectionFactor);
			SetAmuletString(106, message);

			switch(AmuletWellMDATestMenu_mda.DetectorTest){
				case DET_WELL:
					SetAmuletByte(110, 0xFF);
					break;

				case DET_BETA:
					SetAmuletByte(111, 0xFF);
					break;

				case DET_PROBE700:
				case DET_DRILLEDPROBE700_AS_PROBE:
					SetAmuletByte(112, 0xFF);
					break;

				case DET_WELL700:
				case DET_DRILLEDPROBE700_AS_WELL:
					SetAmuletByte(113, 0xFF);
					break;
			}

			if((AmuletWellMDATestMenu_mda.Spectrum.MeasuredOn != (time_t)0) && (AmuletWellMDATestMenu_nuclideID != -1) && (AmuletWellMDATestMenu_mda.PrecisionFactor != -1.0) && (AmuletWellMDATestMenu_mda.CorrectionFactor != -1.0)){
				SetAmuletByte(101, 0xFF);
			}
			m_iPhase = PHASE_WELL_MDA_TEST_WAIT;
			break;

		case PHASE_WELL_MDA_TEST_WAIT:
			break;

		case PHASE_WELL_MDA_TEST_CALC:
			beep_amulet();
			AmuletWellMDATestMenu_mda.WellMDATestID = 0;
			AmuletWellMDATestMenu_mda.InactiveReason[0] = 0;
			AmuletWellMDATestMenu_mda.Inactive = FALSE;
			NuclideData_getName(AmuletWellMDATestMenu_nuclideID, AmuletWellMDATestMenu_mda.NuclideName);

			SetAmuletString(107, "Nuclide: ");
			SetAmuletString(108, AmuletWellMDATestMenu_mda.NuclideName);

			AmuletWellMDATestMenu_mda.Energy = NuclideData_getEffectivePrimary(AmuletWellMDATestMenu_nuclideID);
			if(AmuletWellMDATestMenu_mda.DetectorTest == DET_DRILLEDPROBE700_AS_WELL) AmuletWellMDATestMenu_mda.Efficiency = NuclideData_getEffectiveEff(AmuletWellMDATestMenu_nuclideID, DET_WELL700);
			else if(AmuletWellMDATestMenu_mda.DetectorTest == DET_DRILLEDPROBE700_AS_PROBE) AmuletWellMDATestMenu_mda.Efficiency = NuclideData_getEffectiveEff(AmuletWellMDATestMenu_nuclideID, DET_PROBE700);
			else AmuletWellMDATestMenu_mda.Efficiency = NuclideData_getEffectiveEff(AmuletWellMDATestMenu_nuclideID, AmuletWellMDATestMenu_mda.DetectorTest);
			AmuletWellMDATestMenu_mda.Efficiency /= 100.0;
			SetAmuletString(109, "Efficiency (Eff): ");
			sprintf(message, "%.3f", AmuletWellMDATestMenu_mda.Efficiency);
			SetAmuletString(110, message);

			if(AmuletWellMDATestMenu_mda.DetectorTest == DET_DRILLEDPROBE700_AS_PROBE) Mca_getEndPoints(AmuletWellMDATestMenu_mda.Energy, &AmuletWellMDATestMenu_mda.StartROI, &AmuletWellMDATestMenu_mda.EndROI, TRUE);
			else Mca_getEndPoints(AmuletWellMDATestMenu_mda.Energy, &AmuletWellMDATestMenu_mda.StartROI, &AmuletWellMDATestMenu_mda.EndROI, FALSE);

			Mca_getROIChannels(AmuletWellMDATestMenu_mda.StartROI, AmuletWellMDATestMenu_mda.EndROI, &startCh, &endCh, &AmuletWellMDATestMenu_mda.Spectrum.AutoCal);
			SetAmuletString(111, "ROI: ");
			sprintf(message, "%.1f - %.1f keV", AmuletWellMDATestMenu_mda.StartROI, AmuletWellMDATestMenu_mda.EndROI);
			SetAmuletString(112, message);

			AmuletWellMDATestMenu_mda.ROICounts = Mca_getROICounts(AmuletWellMDATestMenu_mda.Spectrum.DecompressedSpectra, startCh, endCh);
			SetAmuletString(113, "Counts(N): ");
			cnts = AmuletWellMDATestMenu_mda.ROICounts;
			sprintf(message, "%.0f", cnts);
			SetAmuletString(114, message);

			counts = sqrt(cnts);
			numerator = (AmuletWellMDATestMenu_mda.PrecisionFactor * counts) + AmuletWellMDATestMenu_mda.CorrectionFactor;
			liveTime = AmuletWellMDATestMenu_mda.Spectrum.LiveTime;
			if(current.system == CI) liveTime /= 60.0;

			SetAmuletString(115, "Count Time (T): ");
			if(current.system == CI) sprintf(message, "%.2f min", liveTime);
			else sprintf(message, "%.2f sec", liveTime);
			SetAmuletString(116, message);

			SetAmuletString(117, "Precision Factor (f): ");
			sprintf(message, "%.2f", AmuletWellMDATestMenu_mda.PrecisionFactor);
			SetAmuletString(119, message);

			SetAmuletString(120, "Correction Factor (C): ");
			sprintf(message, "%.2f", AmuletWellMDATestMenu_mda.CorrectionFactor);
			SetAmuletString(122, message);

			SetAmuletString(123, "MDA = ((f * SQRT(N)) + C) / (Eff * T) = ");
			denominator = AmuletWellMDATestMenu_mda.Efficiency * liveTime;
			mda = numerator /denominator;
			if(current.system == CI){
				AmuletWellMDATestMenu_mda.MDA = mda;
				sprintf(message, "%.1f dpm", mda);
			}else{
				AmuletWellMDATestMenu_mda.MDA = 60.0 * mda;
				sprintf(message, "%.1f Bq", mda);
			}
			SetAmuletString(125, message);

#ifdef TIMES1000
			for(index=0; index<TIMES1000; index++){
#endif
				DB_CreateMDATest(&AmuletWellMDATestMenu_mda, TRUE);
#ifdef TIMES1000
				AmuletWellMDATestMenu_mda.Spectrum.SpectraID = 0;
				AmuletWellMDATestMenu_mda.Spectrum.MeasuredOn -= 86400;
			}
#endif
			SetAmuletByte(102, 0xFF);

			//if(current.printer != NONE_PRINTER){
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
			{
				delayloop(20);
				SetAmuletByte(103, 0xFF);
			}

			m_iPhase = PHASE_WELL_MDA_TEST_WAIT;
			break;

		case PHASE_WELL_MDA_TEST_PRINT:
			beep_amulet();
			AmuletWellMDATest_print();
			SetAmuletByte(104, 0xFF);
			m_iPhase = PHASE_WELL_MDA_TEST_WAIT;
			break;

		case PHASE_WELL_MDA_TEST_SPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &AmuletWellMDATestMenu_mda.Spectrum, sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "MDA Test Spectrum");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;
	}
}

void AmuletWellMDATest_print(void){
	char prtype, strng[90], buf[40], detectortype[30];
	short nc;

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "MDA TEST", AmuletWellMDATestMenu_mda.Spectrum.MeasuredOn);

		switch(AmuletWellMDATestMenu_mda.DetectorTest){
			case DET_WELL:
				strcpy(detectortype, "Det: WELL");
				break;

			case DET_WELL700:
				strcpy(detectortype, "Det: Well");
				break;

			case DET_BETA:
				strcpy(detectortype, "Det: Beta");
				break;

			case DET_PROBE700:
				strcpy(detectortype, "Det: Probe");
				break;

			case DET_DRILLEDPROBE700:
				strcpy(detectortype, "Det: Drilled Probe");
				break;

			case DET_DRILLEDPROBE700_AS_WELL:
				strcpy(detectortype, "Det: Drilled Probe As Well");
				break;

			case DET_DRILLEDPROBE700_AS_PROBE:
				strcpy(detectortype, "Det: Drilled Probe As Probe");
				break;
		}
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%s", detectortype);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Nuclide:", 8);
		nc = strlen(AmuletWellMDATestMenu_mda.NuclideName);
		strncpy(&strng[11], AmuletWellMDATestMenu_mda.NuclideName, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "    ROI: ", 9);
		nc = sprintf(buf, "%.1f - %.1f keV", AmuletWellMDATestMenu_mda.StartROI, AmuletWellMDATestMenu_mda.EndROI);
		strncpy(&strng[11], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Efficiency: %.3f %%", AmuletWellMDATestMenu_mda.Efficiency);
		strncpy(&strng[2], buf, nc);
		strncpy(&strng[28], "=Eff", 4);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "    Counts: %d", AmuletWellMDATestMenu_mda.ROICounts);
		strncpy(&strng[2], buf, nc);
		strncpy(&strng[30], "=N", 2);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(current.system == CI) nc = sprintf(buf, "Count Time: %.2f min", AmuletWellMDATestMenu_mda.Spectrum.LiveTime / 60.0);
		else nc = sprintf(buf, "Count Time: %.2f sec", AmuletWellMDATestMenu_mda.Spectrum.LiveTime);
		strncpy(&strng[2], buf, nc);
		strncpy(&strng[30], "=T", 2);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, " Precision: %.2f", AmuletWellMDATestMenu_mda.PrecisionFactor);
		strncpy(&strng[2], buf, nc);
		strncpy(&strng[30], "=f", 2);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Correction: %.2f", AmuletWellMDATestMenu_mda.CorrectionFactor);
		strncpy(&strng[2], buf, nc);
		strncpy(&strng[30], "=C", 2);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		if(current.system == CI) nc = sprintf(buf, "MDA: %.1f dpm", AmuletWellMDATestMenu_mda.MDA);
		else nc = sprintf(buf, "MDA: %.1f Bq", AmuletWellMDATestMenu_mda.MDA / 60.0);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "       (f * SQRT(N)) + C", 24);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "MDA = -------------------", 25);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "            Eff * T", 19);
		pr_write(strng);

		formfeed(prtype);
	}
}

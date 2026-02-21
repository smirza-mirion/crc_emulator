#define PHASE_WELLSPECTRUM_PRE_INIT		0
#define PHASE_WELLSPECTRUM_WAIT			1
#define PHASE_WELLSPECTRUM_BACK10		2
#define PHASE_WELLSPECTRUM_BACK1		3
#define PHASE_WELLSPECTRUM_FORWARD1		4
#define PHASE_WELLSPECTRUM_FORWARD10	5
#define PHASE_WELLSPECTRUM_CURSOR		6
#define PHASE_WELLSPECTRUM_PRINT		7
#define PHASE_WELLSPECTRUM_BACK10_2		8
#define PHASE_WELLSPECTRUM_BACK1_2		9
#define PHASE_WELLSPECTRUM_FORWARD1_2	10
#define PHASE_WELLSPECTRUM_FORWARD10_2	11
#define PHASE_WELLSPECTRUM_DRAW_GRAPH	12

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "amulet.h"
#include "printer.h"

extern CURRENT current;
extern int m_iPhase;

DB_SPEC AmuletWellSpectrumMenu_spectrum;
char AmuletWellSpectrumMenu_comment1[26];
char AmuletWellSpectrumMenu_comment2[26];
char AmuletWellSpectrumMenu_comment3[26];
char AmuletWellSpectrumMenu_inactiveReason[26];
bool AmuletWellSpectrumMenu_inactive;
static uchar AmuletWellSpectrum_menu_corrected_spectrum[512];
static int AmuletWellSpectrum_menu_cursor;
static int AmuletWellSpectrum_menu_cursor_2;

void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
static void UpdateSpectrum(unsigned char *display);
static void AmuletWellSpectrum_print(void);

void AmuletWellSpectrum_menu(void){
	char acMsg[100], detectortype[20];
	int index, xcoord1, xcoord2;
	float combinedgain1, combinedgain2;
	float hv, hvcode;
	ulong max, graph_max, average;
	float normal_factor, normal_factor_num, normal_factor_denom, display_float, cpm, cps;
	uchar norm_display[4096];
	ulong display[512];
	float energy, corrected_float;

	switch(m_iPhase){
		case PHASE_WELLSPECTRUM_PRE_INIT:
			AmuletWellSpectrum_menu_cursor = 0;
			sprintf(acMsg, "%.2f keV", Mca_convertChToEnergy(AmuletWellSpectrum_menu_cursor, &AmuletWellSpectrumMenu_spectrum.AutoCal));
			SetAmuletString(116, acMsg);
			sprintf(acMsg, "%lu", AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[AmuletWellSpectrum_menu_cursor]);
			SetAmuletString(117, acMsg);

			AmuletWellSpectrum_menu_cursor_2 = 0;
			sprintf(acMsg, "%.2f keV", Mca_convertChToEnergy(AmuletWellSpectrum_menu_cursor_2, &AmuletWellSpectrumMenu_spectrum.AutoCal));
			SetAmuletString(118, acMsg);
			sprintf(acMsg, "%lu", AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[AmuletWellSpectrum_menu_cursor_2]);
			SetAmuletString(119, acMsg);

			// Calculate ROI Counts
			sprintf(acMsg, "%lu", AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[AmuletWellSpectrum_menu_cursor]);
			SetAmuletString(120, acMsg);

			// Calculate ROI CPM /CPS
			cps = AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[AmuletWellSpectrum_menu_cursor];
			cps /= AmuletWellSpectrumMenu_spectrum.LiveTime;
			cpm = cps * 60.0;

			if(current.system == CI){
				sprintf(acMsg, "%.0f", cpm);
				SetAmuletString(122, acMsg);
			}else{
				sprintf(acMsg, "%.0f", cps);
				SetAmuletString(122, acMsg);
			}

			if(AmuletWellSpectrumMenu_spectrum.SpectraID > 0){
				sprintf(acMsg, "Spectrum ID: %lld", AmuletWellSpectrumMenu_spectrum.SpectraID);
				SetAmuletString(100, acMsg);
			}

			switch(AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype){
				case DET_WELL:
					strcpy(detectortype, "WELL");
					break;

				case DET_WELL700:
					strcpy(detectortype, "Well");
					break;

				case DET_BETA:
					strcpy(detectortype, "Beta");
					break;

				case DET_PROBE700:
					strcpy(detectortype, "Probe");
					break;

				case DET_DRILLEDPROBE700:
					strcpy(detectortype, "Drilled Probe");
					break;

				default:
					strcpy(detectortype, "");
					break;
			}

			sprintf(acMsg, "%d Ch, %s", AmuletWellSpectrumMenu_spectrum.AutoCal.num_of_channels, detectortype);
			SetAmuletString(98, acMsg);

			combinedgain1 = AmuletWellSpectrumMenu_spectrum.AutoCal.gain1;
			combinedgain2 = AmuletWellSpectrumMenu_spectrum.AutoCal.gain2;
			combinedgain1 /= 64.0;
			combinedgain2 /= 64.0;
			combinedgain1 *= combinedgain2;
			sprintf(acMsg, "%.2f", combinedgain1);
			SetAmuletString(101, acMsg);

			sprintf(acMsg, "%d", AmuletWellSpectrumMenu_spectrum.AutoCal.zeroopampoffset);
			SetAmuletString(102, acMsg);

			sprintf(acMsg, "%d", AmuletWellSpectrumMenu_spectrum.AutoCal.threshold);
			SetAmuletString(103, acMsg);

			hvcode = AmuletWellSpectrumMenu_spectrum.AutoCal.hv;

			if((AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype == DET_WELL) || (AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype == DET_BETA)) hv = (.158974 * hvcode) + 595.0;
			else if((AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype == DET_PROBE700) || (AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype == DET_WELL700) || (AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype == DET_DRILLEDPROBE700)) hv = .263158 * hvcode;
			//sprintf(acMsg, "%.0f Volts (%.0f)", hv, hvcode);
			sprintf(acMsg, "%.0f Volts", hv);
			SetAmuletString(104, acMsg);

			SetAmuletString(108, AmuletWellSpectrumMenu_spectrum.AutoCal.serialnum);

			GetExtendedTimeInfoSec(&AmuletWellSpectrumMenu_spectrum.MeasuredOn, acMsg);
			SetAmuletString(109, acMsg);

			if(current.system == CI){
				SetAmuletString(111, "total cpm:");
				SetAmuletString(121, "roi cpm:");
			}else if(current.system == BQ){
				SetAmuletString(111, "total cps:");
				SetAmuletString(121, "roi cps:");
			}

			if(AmuletWellSpectrumMenu_comment1[0] != 0){
				SetAmuletString(112, AmuletWellSpectrumMenu_comment1);
			}

			if(AmuletWellSpectrumMenu_comment2[0] != 0){
				SetAmuletString(113, AmuletWellSpectrumMenu_comment2);
			}

			if(AmuletWellSpectrumMenu_comment3[0] != 0){
				SetAmuletString(114, AmuletWellSpectrumMenu_comment3);
			}

			// Get Max point in graph
			max = 0;
			for(index=0; index<AmuletWellSpectrumMenu_spectrum.NumOfChannels; index++){
				if(AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[index] > max) max = AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[index];
			}

			// Scale Y Max of Graph
			graph_max = 500;
			while(max > graph_max) graph_max *= 2;

			sprintf(acMsg, "%ld", graph_max);
			SetAmuletString(106, acMsg);

			sprintf(acMsg, "%.2f", AmuletWellSpectrumMenu_spectrum.RealTime);
			SetAmuletString(107, acMsg);

			sprintf(acMsg, "%.2f", AmuletWellSpectrumMenu_spectrum.LiveTime);
			SetAmuletString(115, acMsg);

			cpm = AmuletWellSpectrumMenu_spectrum.TotalCPM;
			if(current.system == BQ) cpm /= 60.0;
			sprintf(acMsg, "%.0f", cpm);
			SetAmuletString(110, acMsg);

			SetAmuletWord(100, 0xFFFF);

			// Calculate scale factor to scale graph at graph_max and a max count of 255
			normal_factor_num = 255.0;
			normal_factor_denom = graph_max;
			normal_factor = normal_factor_num / normal_factor_denom;

			// Apply to scale factor to graph
			for(index=0; index<AmuletWellSpectrumMenu_spectrum.NumOfChannels; index++){
				display_float = AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[index];
				display_float *= normal_factor;
				if(display_float > 255.0){
					norm_display[index] = 255;
				}else{
					norm_display[index] = display_float;
				}
			}

			// Convert from NumOfChannels to 512 channels on the graph
			switch(AmuletWellSpectrumMenu_spectrum.NumOfChannels){
				case 256:
					for(index=0; index<511; index++){
						if((index%2)== 0){
							display[index] = norm_display[index/2];
						}else{
							average = norm_display[(index-1)/2];
							average += norm_display[(index+1)/2];
							average >>= 1;
							display[index] = average;
						}
					}
					display[511] = display[510];
					break;

				case 512:
					for(index=0; index<512; index++) display[index] = norm_display[index];
					break;

				case 1024:
					for(index=0; index<512; index++){
						average = norm_display[2*index];
						average += norm_display[2*index + 1];
						average >>= 1;
						display[index] = average;
					}
					break;

				case 2048:
					for(index=0; index<512; index++){
						average = norm_display[4*index];
						average += norm_display[4*index+1];
						average += norm_display[4*index+2];
						average += norm_display[4*index+3];
						average >>= 2;
						display[index] = average;
					}
					break;

				case 4096:
					for(index=0; index<512; index++){
						average = norm_display[8*index];
						average += norm_display[8*index+1];
						average += norm_display[8*index+2];
						average += norm_display[8*index+3];
						average += norm_display[8*index+4];
						average += norm_display[8*index+5];
						average += norm_display[8*index+6];
						average += norm_display[8*index+7];
						average >>= 3;
						display[index] = average;
					}
					break;
			}

			//Load display spectrum for linearity correction
			Mca_loadUncorrectedSpectrum(display, &AmuletWellSpectrumMenu_spectrum.AutoCal, 512);

			//Corrected display spectrum is placed in AmuletWellSpectrum_menu_corrected_spectrum
			for(index=0; index<512; index++){
				energy = index;
				energy *= 2000.0;
				energy /= 511.0;
				corrected_float = Mca_getCorrectedSpectra(energy);
				if(corrected_float > 255)AmuletWellSpectrum_menu_corrected_spectrum[index] = 255;
				else AmuletWellSpectrum_menu_corrected_spectrum[index] = corrected_float;
			}

			SetAmuletLine(288, 75, 288, 330, 0, 1);
			SetAmuletLine(288, 330, 800, 330, 0, 1);

			if((current.printer == OKI_PRINTER) || (current.printer == LX_PRINTER) || (current.printer == ROLL_PRINTER) || (current.printer == USB_PRINTER) || (current.printer == USB_EPS_PRINTER)) SetAmuletWord(102, 0xFFFF);

			AmuletWellMeasurementMenu_drawGraph(AmuletWellSpectrum_menu_corrected_spectrum, TRUE);

			m_iPhase = PHASE_WELLSPECTRUM_DRAW_GRAPH;
			break;

		case PHASE_WELLSPECTRUM_WAIT:
			break;

		case PHASE_WELLSPECTRUM_DRAW_GRAPH:
			if(AmuletWellMeasurementMenu_drawGraph(AmuletWellSpectrum_menu_corrected_spectrum, FALSE)){
				m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			}
			break;

		case PHASE_WELLSPECTRUM_BACK10:
			beep_amulet();
			AmuletWellSpectrum_menu_cursor -= (10 * (AmuletWellSpectrumMenu_spectrum.NumOfChannels / 256));
			if(AmuletWellSpectrum_menu_cursor < 0) AmuletWellSpectrum_menu_cursor = 0;
			UpdateSpectrum(AmuletWellSpectrum_menu_corrected_spectrum);
			m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			break;

		case PHASE_WELLSPECTRUM_BACK1:
			beep_amulet();
			AmuletWellSpectrum_menu_cursor -= 1;
			if(AmuletWellSpectrum_menu_cursor < 0) AmuletWellSpectrum_menu_cursor = 0;
			UpdateSpectrum(AmuletWellSpectrum_menu_corrected_spectrum);
			m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			break;

		case PHASE_WELLSPECTRUM_FORWARD1:
			beep_amulet();
			AmuletWellSpectrum_menu_cursor += 1;
			if(AmuletWellSpectrum_menu_cursor > AmuletWellSpectrumMenu_spectrum.NumOfChannels - 1) AmuletWellSpectrum_menu_cursor = AmuletWellSpectrumMenu_spectrum.NumOfChannels - 1;
			UpdateSpectrum(AmuletWellSpectrum_menu_corrected_spectrum);
			m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			break;

		case PHASE_WELLSPECTRUM_FORWARD10:
			beep_amulet();
			AmuletWellSpectrum_menu_cursor += (10 * (AmuletWellSpectrumMenu_spectrum.NumOfChannels / 256));
			if(AmuletWellSpectrum_menu_cursor > AmuletWellSpectrumMenu_spectrum.NumOfChannels - 1) AmuletWellSpectrum_menu_cursor = AmuletWellSpectrumMenu_spectrum.NumOfChannels - 1;
			UpdateSpectrum(AmuletWellSpectrum_menu_corrected_spectrum);
			m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			break;

		case PHASE_WELLSPECTRUM_CURSOR:
			switch(AmuletWellSpectrumMenu_spectrum.NumOfChannels){
				case 256:
					xcoord1 = (2 * AmuletWellSpectrum_menu_cursor) + 289;
					xcoord2 = (2 * AmuletWellSpectrum_menu_cursor_2) + 289;
					break;

				case 512:
					xcoord1 = AmuletWellSpectrum_menu_cursor + 289;
					xcoord2 = AmuletWellSpectrum_menu_cursor_2 + 289;
					break;

				case 1024:
					xcoord1 = (AmuletWellSpectrum_menu_cursor / 2) + 289;
					xcoord2 = (AmuletWellSpectrum_menu_cursor_2 / 2) + 289;
					break;

				case 2048:
					xcoord1 = (AmuletWellSpectrum_menu_cursor / 4) + 289;
					xcoord2 = (AmuletWellSpectrum_menu_cursor_2 / 4) + 289;
					break;

				case 4096:
					xcoord1 = (AmuletWellSpectrum_menu_cursor / 8) + 289;
					xcoord2 = (AmuletWellSpectrum_menu_cursor_2 / 8) + 289;
					break;
			}

			if(xcoord1 == xcoord2){
				if(xcoord1 == 289) SetAmuletLine(xcoord1, 75, xcoord1, 329, 0, 0x01);
				else  SetAmuletLine(xcoord1, 75, xcoord1, 329, 0xFF0000, 0x01);
			}else{
				if(xcoord1 == 289) SetAmuletLine(xcoord1, 75, xcoord1, 329, 0, 0x01);
				else SetAmuletLine(xcoord1, 75, xcoord1, 329, 0xFF, 0x01);

				if(xcoord2 == 289) SetAmuletLine(xcoord2, 75, xcoord2, 329, 0, 0x01);
				else SetAmuletLine(xcoord2, 75, xcoord2, 329, 0x7F00, 0x01);
			}
			m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			break;

		case PHASE_WELLSPECTRUM_PRINT:
			beep_amulet();
			AmuletWellSpectrum_print();
			SetAmuletWord(102, 0xFFFF);
			m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			break;

		case PHASE_WELLSPECTRUM_BACK10_2:
			beep_amulet();
			AmuletWellSpectrum_menu_cursor_2 -= (10 * (AmuletWellSpectrumMenu_spectrum.NumOfChannels / 256));
			if(AmuletWellSpectrum_menu_cursor_2 < 0) AmuletWellSpectrum_menu_cursor_2 = 0;
			UpdateSpectrum(AmuletWellSpectrum_menu_corrected_spectrum);
			m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			break;

		case PHASE_WELLSPECTRUM_BACK1_2:
			beep_amulet();
			AmuletWellSpectrum_menu_cursor_2 -= 1;
			if(AmuletWellSpectrum_menu_cursor_2 < 0) AmuletWellSpectrum_menu_cursor_2 = 0;
			UpdateSpectrum(AmuletWellSpectrum_menu_corrected_spectrum);
			m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			break;

		case PHASE_WELLSPECTRUM_FORWARD1_2:
			beep_amulet();
			AmuletWellSpectrum_menu_cursor_2 += 1;
			if(AmuletWellSpectrum_menu_cursor_2 > AmuletWellSpectrumMenu_spectrum.NumOfChannels - 1) AmuletWellSpectrum_menu_cursor_2 = AmuletWellSpectrumMenu_spectrum.NumOfChannels - 1;
			UpdateSpectrum(AmuletWellSpectrum_menu_corrected_spectrum);
			m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			break;

		case PHASE_WELLSPECTRUM_FORWARD10_2:
			beep_amulet();
			AmuletWellSpectrum_menu_cursor_2 += (10 * (AmuletWellSpectrumMenu_spectrum.NumOfChannels / 256));
			if(AmuletWellSpectrum_menu_cursor_2 > AmuletWellSpectrumMenu_spectrum.NumOfChannels - 1) AmuletWellSpectrum_menu_cursor_2 = AmuletWellSpectrumMenu_spectrum.NumOfChannels - 1;
			UpdateSpectrum(AmuletWellSpectrum_menu_corrected_spectrum);
			m_iPhase = PHASE_WELLSPECTRUM_WAIT;
			break;
	}
}

static void UpdateSpectrum(unsigned char *display){
	char acMsg[100];
	int lower, upper, index;
	long unsigned int counts;
	float cps, cpm;

	sprintf(acMsg, "%.2f keV", Mca_convertChToEnergy(AmuletWellSpectrum_menu_cursor, &AmuletWellSpectrumMenu_spectrum.AutoCal));
	SetAmuletString(116, acMsg);
	sprintf(acMsg, "%lu", AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[AmuletWellSpectrum_menu_cursor]);
	SetAmuletString(117, acMsg);

	sprintf(acMsg, "%.2f keV", Mca_convertChToEnergy(AmuletWellSpectrum_menu_cursor_2, &AmuletWellSpectrumMenu_spectrum.AutoCal));
	SetAmuletString(118, acMsg);
	sprintf(acMsg, "%lu", AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[AmuletWellSpectrum_menu_cursor_2]);
	SetAmuletString(119, acMsg);

	// Calculate ROI Counts
	if(AmuletWellSpectrum_menu_cursor == AmuletWellSpectrum_menu_cursor_2){
		counts = AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[AmuletWellSpectrum_menu_cursor];
	}else{
		if(AmuletWellSpectrum_menu_cursor < AmuletWellSpectrum_menu_cursor_2){
			lower = AmuletWellSpectrum_menu_cursor;
			upper = AmuletWellSpectrum_menu_cursor_2;
		}else{
			upper = AmuletWellSpectrum_menu_cursor;
			lower = AmuletWellSpectrum_menu_cursor_2;
		}

		counts = 0;
		for(index=lower; index<=upper; index++) counts += AmuletWellSpectrumMenu_spectrum.DecompressedSpectra[index];
	}
	sprintf(acMsg, "%lu", counts);
	SetAmuletString(120, acMsg);

	// Calculate ROI CPM
	cps = counts;
	cps /= AmuletWellSpectrumMenu_spectrum.LiveTime;
	cpm = cps * 60.0;

	if(current.system == CI){
		sprintf(acMsg, "%.0f", cpm);
		SetAmuletString(122, acMsg);
	}else{
		sprintf(acMsg, "%.0f", cps);
		SetAmuletString(122, acMsg);
	}

	SetAmuletWord(101, 0xFFFF);

	AmuletWellMeasurementMenu_drawGraph(display, TRUE);
	while(!AmuletWellMeasurementMenu_drawGraph(display, FALSE));
}

static void AmuletWellSpectrum_print(void){
	char prtype;
	char strng[90], buf[40], detectortype[20];
	float combinedgain1, combinedgain2;
	float hv, hvcode, cpm;
	short nc;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		g_resetlinespacing(prtype);
		rawheadersec(prtype, "SPECTRUM", AmuletWellSpectrumMenu_spectrum.MeasuredOn);

		g_print(prtype, AmuletWellSpectrumMenu_spectrum.DecompressedSpectra, -1.0, -1.0, &AmuletWellSpectrumMenu_spectrum.AutoCal);

		// Spectrum ID
		if(AmuletWellSpectrumMenu_spectrum.SpectraID > 0){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "Spectrum ID: %lld", AmuletWellSpectrumMenu_spectrum.SpectraID);
			strncpy(strng, buf, nc);
			pr_write(strng);
		}

		switch(AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype){
			case DET_WELL:
				strcpy(detectortype, "WELL");
				break;

			case DET_WELL700:
				strcpy(detectortype, "Well");
				break;

			case DET_BETA:
				strcpy(detectortype, "Beta");
				break;

			case DET_PROBE700:
				strcpy(detectortype, "Probe");
				break;

			case DET_DRILLEDPROBE700:
				strcpy(detectortype, "Drilled Probe");
				break;

			default:
				strcpy(detectortype, "");
				break;
		}
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%d Ch, %s", AmuletWellSpectrumMenu_spectrum.AutoCal.num_of_channels, detectortype);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		// Gain
		combinedgain1 = AmuletWellSpectrumMenu_spectrum.AutoCal.gain1;
		combinedgain2 = AmuletWellSpectrumMenu_spectrum.AutoCal.gain2;
		combinedgain1 /= 64.0;
		combinedgain2 /= 64.0;
		combinedgain1 *= combinedgain2;
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Gain: %.2f", combinedgain1);
		strncpy(&strng[7], buf, nc);
		pr_write(strng);

		// Offset
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Offset: %d", AmuletWellSpectrumMenu_spectrum.AutoCal.zeroopampoffset);
		strncpy(&strng[5], buf, nc);
		pr_write(strng);

		// Threshold
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Thresh: %d", AmuletWellSpectrumMenu_spectrum.AutoCal.threshold);
		strncpy(&strng[5], buf, nc);
		pr_write(strng);

		// HV
		lininit(strng, TRUE, prtype);
		hvcode = AmuletWellSpectrumMenu_spectrum.AutoCal.hv;
		if((AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype == DET_WELL) || (AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype == DET_BETA)) hv = (.158974 * hvcode) + 595.0;
		else if((AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype == DET_PROBE700) || (AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype == DET_WELL700) || (AmuletWellSpectrumMenu_spectrum.AutoCal.detectortype == DET_DRILLEDPROBE700)) hv = .263158 * hvcode;
		nc = sprintf(buf, "HV: %.0f V", hv);
		strncpy(&strng[9], buf, nc);
		pr_write(strng);

		// SN
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Well S/N: %s", AmuletWellSpectrumMenu_spectrum.AutoCal.serialnum);
		strncpy(&strng[3], buf, nc);
		pr_write(strng);

		// Real
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Real: %.2f sec", AmuletWellSpectrumMenu_spectrum.RealTime);
		strncpy(&strng[7], buf, nc);
		pr_write(strng);

		// Live
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Live: %.2f sec", AmuletWellSpectrumMenu_spectrum.LiveTime);
		strncpy(&strng[7], buf, nc);
		pr_write(strng);

		// Total CPM
		lininit(strng, TRUE, prtype);
		cpm = AmuletWellSpectrumMenu_spectrum.TotalCPM;
		if(current.system == BQ){
			cpm /= 60.0;
			nc = sprintf(buf, "Total CPS: %.0f cps", cpm);
		}else{
			nc = sprintf(buf, "Total CPM: %.0f cpm", cpm);
		}
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		// Comments
		if(AmuletWellSpectrumMenu_comment1[0] != 0){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "%s", AmuletWellSpectrumMenu_comment1);
			strncpy(strng, buf, nc);
			pr_write(strng);
		}

		if(AmuletWellSpectrumMenu_comment2[0] != 0){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "%s", AmuletWellSpectrumMenu_comment2);
			strncpy(strng, buf, nc);
			pr_write(strng);
		}

		if(AmuletWellSpectrumMenu_comment3[0] != 0){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "%s", AmuletWellSpectrumMenu_comment3);
			strncpy(strng, buf, nc);
			pr_write(strng);
		}

		if(AmuletWellSpectrumMenu_inactive){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "INACTIVE: %s", AmuletWellSpectrumMenu_inactiveReason);
			strncpy(strng, buf, nc);
			pr_write(strng);
		}

		formfeed(prtype);
	}
}

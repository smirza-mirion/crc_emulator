#define PHASE_WELLREPORTS_PRE_INIT	0
#define PHASE_WELLREPORTS_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "wipes.h"
#include "database.h"

extern int m_iPhase;
extern time_t clock_time;

static WELLWIPESEARCH wipes[2];
static WELLSCHILLINGSEARCH schillings[2];
static WELLPLASMASEARCH plasmas[2];
static WELLRBCSEARCH rbcs[2];
static WELLBIOASSAYSEARCH bioassays[2];
static PROBETHYROIDUPTAKESEARCH thyroids[100];
static WELLRBCSURVIVALSEARCH rbcsurvials[100];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);

void AmuletWellReports_menu(void){
	int count;
	DETECTOR *probeDetector, *probeWell;

	switch(m_iPhase){
		case PHASE_WELLREPORTS_PRE_INIT:
			if(Mca_installedDetector == DET_WELL){
				SetAmuletByte(91, 0xFF);
				count = DB_SearchSchillingTests(schillings, (time_t) 0, clock_time, 2);
				if(count != 0) SetAmuletByte(92, 0xFF);
				count = DB_SearchPlasmaTests(plasmas, (time_t) 0, clock_time, 2);
				if(count != 0) SetAmuletByte(93, 0xFF);
				count = DB_SearchRBCTests(rbcs, (time_t) 0, clock_time, 2);
				if(count != 0) SetAmuletByte(94, 0xFF);
				count = DB_SearchRBCSurvivalTests(rbcsurvials, "", "", FALSE);
				if(count != 0) SetAmuletByte(97, 0xFF);
			}else{
				probeDetector = Mca_detectorProbe700Mirror();
				probeWell = Mca_detectorWell700Mirror();

				if((probeDetector->installed == DET_DRILLEDPROBE700) || ((probeDetector->installed == DET_PROBE700) && (probeWell->installed == DET_WELL700))){
					SetAmuletByte(91, 0xFF); // Wipe Report
					count = DB_SearchSchillingTests(schillings, (time_t) 0, clock_time, 2);
					if(count != 0) SetAmuletByte(92, 0xFF);
					count = DB_SearchPlasmaTests(plasmas, (time_t) 0, clock_time, 2);
					if(count != 0) SetAmuletByte(93, 0xFF);
					count = DB_SearchRBCTests(rbcs, (time_t) 0, clock_time, 2);
					if(count != 0) SetAmuletByte(94, 0xFF);
					SetAmuletByte(95, 0xFF); // BioAssay Report
					SetAmuletByte(96, 0xFF); // Thyroid Uptake Report
					count = DB_SearchRBCSurvivalTests(rbcsurvials, "", "", FALSE);
					if(count != 0) SetAmuletByte(97, 0xFF);
				}else if(probeDetector->installed == DET_PROBE700){
					SetAmuletByte(95, 0xFF);
					SetAmuletByte(96, 0xFF);

					count = DB_SearchWipes(wipes, (time_t) 0, clock_time, 1, 2);
					if(count != 0) SetAmuletByte(91, 0xFF);

					count = DB_SearchSchillingTests(schillings, (time_t) 0, clock_time, 2);
					if(count != 0) SetAmuletByte(92, 0xFF);

					count = DB_SearchPlasmaTests(plasmas, (time_t) 0, clock_time, 2);
					if(count != 0) SetAmuletByte(93, 0xFF);

					count = DB_SearchRBCTests(rbcs, (time_t) 0, clock_time, 2);
					if(count != 0) SetAmuletByte(94, 0xFF);

					count = DB_SearchRBCSurvivalTests(rbcsurvials, "", "", FALSE);
					if(count != 0) SetAmuletByte(97, 0xFF);
				}else if(probeWell->installed == DET_WELL700){
					SetAmuletByte(91, 0xFF); // Wipe Report
					count = DB_SearchSchillingTests(schillings, (time_t) 0, clock_time, 2);
					if(count != 0) SetAmuletByte(92, 0xFF);
					count = DB_SearchPlasmaTests(plasmas, (time_t) 0, clock_time, 2);
					if(count != 0) SetAmuletByte(93, 0xFF);
					count = DB_SearchRBCTests(rbcs, (time_t) 0, clock_time, 2);
					if(count != 0) SetAmuletByte(94, 0xFF);
					count = DB_SearchRBCSurvivalTests(rbcsurvials, "", "", FALSE);
					if(count != 0) SetAmuletByte(97, 0xFF);

					count = DB_SearchBioAssayTestsAll(bioassays, (time_t) 0, clock_time, 2);
					if(count != 0) SetAmuletByte(95, 0xFF);

					count = DB_SearchThyroidUptakeTests(thyroids, "", "", FALSE);
					if(count != 0) SetAmuletByte(96, 0xFF);
				}
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLREPORTS_WAIT;
			break;

		case PHASE_WELLREPORTS_WAIT:
			break;
	}
}

#pragma once
struct setup_nuclide {
	char acName[7];
	char acElement[15];
	char acRCalNum[8];
	char acPCalNum[8];
	char acCCalNum[8];
	char acKCalNum[8];
	float fHalflife;
	short sHalflifeUnit;
};
typedef struct setup_nuclide SETUP_NUCLIDE;

struct setup_efficiency_obsolete {
	short nuclideID;
	float userEnergy1;
	float userEnergy2;
	float userEnergy3;
	float userWellEff;
	float userBetaEff;
	time_t wellStamp;
	time_t betaStamp;
};
typedef struct setup_efficiency_obsolete SETUP_EFFICIENCY_OBSOLETE;

struct setup_efficiency {
	short nuclideID;
	float userEnergy1;
	float userEnergy2;
	float userEnergy3;
	float userWellEff;
	float userBetaEff;
	float userProbe700Eff;
	float userWell700Eff;
	time_t wellStamp;
	time_t betaStamp;
	time_t probe700Stamp;
	time_t well700Stamp;
};
typedef struct setup_efficiency SETUP_EFFICIENCY;

extern short SortedBuiltInIndex[];

short NuclideData_getNumOfNucs(void);
short NuclideData_getNucIndexFromButtonKey(short key, short chambertype);
char NuclideData_isShow(short index);
void NuclideData_getName(short index, char *name);
void NuclideData_getFullName(short index, char *fullname);
float NuclideData_getHalflife(short index);
short NuclideData_getHalflifeUnit(short index);
float NuclideData_getResponse(short index, short chambertype);
float NuclideData_getUserResponse(short index, short chambertype);
float NuclideData_getEffectiveResponse(short index, short chambertype);
float NuclideData_getPrimary(short index);
float NuclideData_getUserPrimary(short index);
float NuclideData_getEffectivePrimary(short index);
float NuclideData_getSecondary(short index);
float NuclideData_getUserSecondary(short index);
float NuclideData_getEffectiveSecondary(short index);
float NuclideData_getTertiary(short index);
float NuclideData_getUserTertiary(short index);
float NuclideData_getEffectiveTertiary(short index);

float NuclideData_getEff(short index, int detectorType);
float NuclideData_getEff_DrilledProbe_Well(short index, int detectorType);
float NuclideData_getEff_DrilledProbe_Probe(short index, int detectorType);
float NuclideData_getEffSkewWell(short index, int detectorType);
float NuclideData_getEffSkewProbe(short index, int detectorType);

float NuclideData_getUserEff(short index, int detectorType);
float NuclideData_getUserEff_DrilledProbe_Well(short index, int detectorType);
float NuclideData_getUserEff_DrilledProbe_Probe(short index, int detectorType);
float NuclideData_getUserEffSkewWell(short index, int detectorType);
float NuclideData_getUserEffSkewProbe(short index, int detectorType);

float NuclideData_getEffectiveEff(short index, int detectorType);
float NuclideData_getEffectiveEff_DrilledProbe_Well(short index, int detectorType);
float NuclideData_getEffectiveEff_DrilledProbe_Probe(short index, int detectorType);
float NuclideData_getEffectiveEffSkewWell(short index, int detectorType);
float NuclideData_getEffectiveEffSkewProbe(short index, int detectorType);

time_t NuclideData_getMeasuredOnStamp(short index, int detectorType);
time_t NuclideData_getMeasuredOnStamp_DrilledProbe_Well(short index, int detectorType);
time_t NuclideData_getMeasuredOnStamp_DrilledProbe_Probe(short index, int detectorType);
time_t NuclideData_getMeasuredOnStampSkewWell(short index, int detectorType);
time_t NuclideData_getMeasuredOnStampSkewProbe(short index, int detectorType);

void NuclideData_getNuclide(short index, NUCDATA *nuclide);
bool NuclideData_testNuclideTable(ushort *val);
short NuclideData_getIndexFromName(char *name);
short NuclideData_getIndexFromNameIncludingUser(char *name);
void NuclideData_initializeMirror(void);
void NuclideData_populateSetupStruct(short index, SETUP_NUCLIDE *setupnuclide);
char NuclideData_updateMirror(short index, SETUP_NUCLIDE *setupnuclide);
void NuclideData_getDisplayName(short index, short chambertype, char *displayname);
void NuclideData_getExtendedName(short index, char *extendedname);
char NuclideData_updateUserCalNum(short index, short chambertype, char *usercalnum);
void NuclideData_clearEfficiencyMirror(void);
void NuclideData_initializeEfficiencyMirror(void);
void NuclideData_updateEfficiencyMirror(SETUP_EFFICIENCY *temp);
NUCDATA *NuclideData_getNuclideDataPtr(short index);
int NuclideData_getDetectorNuclideCount(void);
void NuclideData_updateMirror2(short index, NUCDATA *nucdata);

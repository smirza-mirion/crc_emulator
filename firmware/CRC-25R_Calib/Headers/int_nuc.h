#pragma once
//#define DAILY_MEMORY  0x06ca
//#define DAILY_MEMORY  0x6ec7
//#define DAILY_MEMORY  0x4928
//#define DAILY_MEMORY  0x3c96
//#define DAILY_MEMORY  0x8c51
//#define DAILY_MEMORY  0x7f10
//#define DAILY_MEMORY  0x24bd
//#define DAILY_MEMORY  0xba9d
#define DAILY_MEMORY  0x3a3d
short NuclideData_getIndexFromName(char *nucname);
short NuclideData_getIndexFromNameIncludingUser(char *name);
static NUCDATA *NucGetNuclide(short sIndex);
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

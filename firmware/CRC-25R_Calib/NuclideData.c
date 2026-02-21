
/*********************************************************************
  MODULE:	NUCLIDE Data

  FILE:		NuclideData.c

  DATE:		08/23/07

  ANALYSIS: Data for built-in Nuclides and routines to retrieve it
              response for R,PET
              0 means no response
            also Daily Memory Test which checks nucldata for integrity
  *************************************************************************/

#include "chambfac.h"
#include "crc.h"
#include "i2c.h"
#include "int_nuc.h"
#include "mca.h"
#include <string.h>

extern CURRENT current;

typedef struct french_elements SPANELEMENTS;
struct french_elements
{
    char	fullname[15];  // full name of nuclide
};	


float NuclideCal_getResponse(char calstrng[], short ch_type);
void DB_ReadAllUserNuclide(NUCDATA *nucData);
void DB_CreateUserNuclide(int nucindexoffset, NUCDATA *nucdata);
void DB_ReadAllUserCal(USERCAL *userCal);
void DB_CreateUserCal(int userCalIndexOffset, USERCAL *userCal);
void DB_ReadAllUserEff(SETUP_EFFICIENCY *setupEfficiency);
void DB_CreateUserEff(int userEffIndexOffset, SETUP_EFFICIENCY *setupEfficiency);
void NuclideData_initializeEfficiencyMirror(void);

static NUCDATA nuclideData[]={
		-1,249.78,1.1891,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ag110m","Silver        ","24110",DAY,658,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,432.6,0.1309,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Am241 ","Americium     ","26241",YEAR,59.5,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,109.611,0.5148,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ar41  ","Argon         ","2741 ",MIN,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,26,0.8187,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"As72  ","Arsenic       ","2772 ",HOUR,511,834,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,17.77,0.3623,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"As74  ","Arsenic       ","2774 ",DAY,595.8,511,634.8,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,26.24,0.1820,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"As76  ","Arsenic       ","2776 ",HOUR,559.08,657.03,1217.7,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,2.6943,0.2182,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Au198 ","Gold          ","28198",DAY,411.8,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,3.139,0.1290,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Au199 ","Gold          ","28199",DAY,158.4,72.3,208.2,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,11.5,0.5491,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ba131 ","Barium        ","22131",DAY,31.6,496.3,123.8,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,10.539,0.66166,0.67561,0.0,0.0,0.59088,0.59088,    0.0,0.0,0.0,0.0,0.0,0.0,"Ba133 ","Barium        ","22133",YEAR,31.6,356,81,17.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		-1,32.9,0.8661,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Bi207 ","Bismuth       ","24207",YEAR,569.7,1063.66,76.5,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,        //10
		-1,57.04,0.1643,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Br77  ","Bromine       ","2777 ",HOUR,239,520.7,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,20.361,0.5045,0.5010,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"C 11  ","Carbon        ","211  ",MIN,511,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,461.9,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Cd109","Cadmium       ","211  ",DAY,22.6,88.03,0.0,0.0,0.0,0.0,53.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,137.641,0.4069,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ce139 ","Cerium        ","23139",DAY,34.2,165.9,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,284.89,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ce144 ","Cerium        ","23144",DAY,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,17.53,0.5268,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Co55  ","Cobalt        ","2655 ",HOUR,511,931.1,477.2,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,271.81,0.1838,0.1623,0.0,0.0,0.12224,0.12224,    0.0,0.0,0.0,0.0,0.0,0.0,"Co57  ","Cobalt        ","2657 ",DAY,123.64,14.41,0.0,80.0,0.0,0.0,71.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,70.85,0.4413,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Co58  ","Cobalt        ","2658 ",DAY,810.8,511,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,5.2711,1.0000,1.0,0.0,0.0,1.0000,1.0000,    0.0,0.0,0.0,0.0,0.0,0.0,"Co60  ","Cobalt        ","2660 ",YEAR,1173.24,1332.5,0.0,4.0,0.0,0.0,6.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		-1,27.704,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Cr51  ","Chromium      ","2751 ",DAY,320.08,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,		// 20
		-1,9.689,0.2173,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Cs131 ","Cesium        ","27131",DAY,30.4,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,6.48,0.5306,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Cs132 ","Cesium        ","27132",DAY,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,2.0644,0.7546,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Cs134 ","Cesium        ","27134",YEAR,604.7,795.8,567.4,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,13.16,1.0686,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Cs136 ","Cesium        ","27136",DAY,818.5,1048.1,340.6,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,30.05,0.29073,0.2878,0.0,0.0,0.27669,0.27669,    0.0,0.0,0.0,0.0,0.0,0.0,"Cs137 ","Cesium        ","27137",YEAR,661.66,32.85,0.0,7.0,0.0,0.0,12.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,12.7004,0.0934,0.092146,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Cu64  ","Copper        ","2864 ",HOUR,511,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,61.83,0.1281,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Cu67  ","Copper        ","2867 ",HOUR,184.6,93.31,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,8.14,0.4739,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Dy157 ","Dysprosium    ","39157",HOUR,326.2,45.5,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,13.522,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Eu152 ","Europium      ","318  ",YEAR,41,122,344.28,0.0,0.0,0.0,14.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		-1,1.8289,0.49804,0.48958,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"F 18  ","Fluorine      ","318  ",HOUR,511,0.0,0.0,18.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,	// 30
		-1,8.273,0.4274,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Fe52  ","Iron          ","3352 ",HOUR,168.7,511,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,44.494,0.4794,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Fe59  ","Iron          ","3359 ",DAY,1099,1292,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,9.49,0.9191,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ga66  ","Gallium       ","4266 ",HOUR,511,1039.9,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,3.2613,0.1727,0.1614,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ga67  ","Gallium       ","4267 ",DAY,93.31,184.58,300.21,34.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,67.83,0.4906,0.4829,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ga68  ","Gallium       ","4268 ",MIN,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,14.1,1.0332,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ga72  ","Gallium       ","4272 ",HOUR,834,630,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,42.39,0.4395,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Hf181 ","Hafnium       ","43181",DAY,482,133,58.8,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,64.14,0.2628,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Hg197 ","Mercury       ","44197",HOUR,71.7,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,46.594,0.1662,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Hg203 ","Mercury       ","44203",DAY,279.2,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		-1,13.2234,0.3372,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"I 123 ","Iodine        ","4123 ",HOUR,158.97,28.05,0.0,28.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,	//40
		-1,4.176,0.6096,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"I 124 ","Iodine        ","4124 ",DAY,602.72,28.04,511,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,59.388,0.3763,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"I 125 ","Iodine        ","4125 ",DAY,28.4,0.0,0.0,16.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,16100000,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"I 129 ","Iodine        ","4125 ",YEAR,31.28,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,12.36,0.9944,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"I 130 ","Iodine        ","4130 ",HOUR,536.1,668.5,739.5,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,8.0233,0.2201,0.2129,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"I 131 ","Iodine        ","4131 ",DAY,364.48,637.14,284.3,18.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,20.87,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"I 133 ","Iodine        ","4131 ",HOUR,529.47,875.33,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,2.8049,0.3614,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"In111 ","Indium        ","46111",DAY,171.28,23.62,245.39,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,99.476,0.1504,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"In113m","Indium        ","46113",MIN,391.7,24.7,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,54.29,0.9851,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"In116m","Indium        ","46116",MIN,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		-1,73.827,0.4590,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ir192 ","Iridium       ","47192",DAY,310.64,468.1,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,		//50
		-1,12.355,0.1104,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"K 42  ","Potassium     ","542  ",HOUR,1524.67,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,22.3,0.4794,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"K 43  ","Potassium     ","543  ",HOUR,375.55,617.49,593.4,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,35.04,0.1262,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Kr79  ","Krypton       ","5779 ",HOUR,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,76.3,0.3121,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Kr87  ","Krypton       ","5787 ",MIN,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,6.647,0.049804,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Lu177 ","Lutetium      ","58177",DAY,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,5.591,1.4162,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Mn52  ","Manganese     ","6652 ",DAY,1434.1,935.5,744.2,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,21.1,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Mn52m ","Manganese     ","6652 ",MIN,511,1434.06,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,312.19,0.3670,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Mn54  ","Manganese     ","6654 ",DAY,834.84,0.0,0.0,0.0,0.0,0.0,10.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,2.57878,0.6626,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Mn56  ","Manganese     ","6656 ",HOUR,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		-1,2.7479,0.2331,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Mo99  ","Molybdenum    ","6699 ",DAY,140.51,749.65,181.05,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,	//60
		-1,9.967,0.5045,0.5039,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"N 13  ","Nitrogen      ","613  ",MIN,511,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,2.6029,0.9693,0.9685,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Na22  ","Sodium        ","6222 ",YEAR,511,1274.54,0.0,0.0,0.0,0.0,16.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,14.958,1.3828,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Na24  ","Sodium        ","6224 ",HOUR,1368,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,10.987,0.2777,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Nd147 ","Neodymium     ","63147",DAY,39.6,91.2,531,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,2.356,0.2164,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Np239 ","Neptunium     ","67239",DAY,105.7,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,2.041,0.5092,0.5077,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"O 15  ","Oxygen        ","615  ",MIN,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,14.284,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"P 32  ","Phosphorus    ","732  ",DAY,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,51.929,0.3995,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Pb203 ","Lead          ","72203",HOUR,74.4,279.2,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,10.64,0.1736,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Pb212 ","Lead          ","72212",HOUR,238.6,78.7,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		-1,1600,0.8029,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ra226 ","Radium        ","72226",YEAR,609,352,79.2,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,		//70
		-1,4.572,0.3307,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Rb81  ","Rubidium      ","7281 ",HOUR,511,446.1,190.3,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,1.2652,0.5480,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Rb82  ","Rubidium      ","7282 ",MIN,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,32.82,0.4023,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Rb84  ","Rubidium      ","7284 ",DAY,881.61,511,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,2.8,0.1560,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Sb117 ","Antimony      ","72117",HOUR,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,2.7238,0.2154,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Sb122 ","Antimony      ","72122",DAY,564.24,692.65,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,60.208,0.7490,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Sb124 ","Antimony      ","72124",DAY,602.73,1690.98,720.17,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,3.97,0.9516,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Sc44  ","Scandium      ","7244 ",HOUR,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,83.787,0.8438,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Sc46  ","Scandium      ","7246 ",DAY,889.25,1120.51,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,119.781,0.3196,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Se75  ","Selenium      ","7375 ",DAY,269.1,1120.51,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		-1,115.09,0.2470,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Sn113 ","Tin           ","76113",DAY,391.7,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,	//80
		-1,64.85,0.2591,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Sr85  ","Strontium     ","7785 ",DAY,514,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,50.57,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Sr89  ","Strontium     ","7789 ",DAY,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,28.8,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Sr90  ","Strontium     ","7790 ",YEAR,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,6.0067,0.1541,0.1414,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Tc99m ","Technetium    ","82996",HOUR,140.51,0.0,0.0,68.0,0.0,0.0,71.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,3.0421,0.2703,0.2539,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Tl201 ","Thallium      ","85201",DAY,72.32,167.43,135.34,46.0,0.0,0.0,45.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,15.9735,1.2172,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"V 48  ","Vanadium      ","848  ",DAY,511,983.52,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,5.2474,0.2545,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Xe133 ","Xenon         ","93133",DAY,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,9.14,0.1587,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Xe135 ","Xenon         ","93135",HOUR,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,14.74,1.4812,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Y 86  ","Yttrium       ","986  ",HOUR,1076.63,511,627.72,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		-1,106.63,1.0240,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Y 88  ","Yttrium       ","988  ",DAY,898,1836,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,		//90
		-1,2.6684,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Y 90  ","Yttrium       ","990  ",DAY,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,32.018,0.9609,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Yb169 ","Ytterbium     ","92169",DAY,54.01,198,177,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,244.01,0.2396,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Zn65  ","Zinc          ","0665 ",DAY,1116,511,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		-1,13.756,0.2127,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Zn69m ","Zinc          ","0669 ",HOUR,436.6,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,65.92,0.0269,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"MoCAP ","              ","66227",HOUR,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,65.92,0.0308,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"MoCAN ","              ","66226",HOUR,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,6.007,0.1187,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"TcCAPM","              ","82227",HOUR,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,  //Malinkcrodt
		0,6.007,0.1085,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"TcCAPB","              ","82227",HOUR,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,  // Bristol Myers


		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
		0,0,0.0,0.0,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"      ","              ","     ",-1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,

		-1,270.95,0.4906,0.4829,0.0,0.0,0.0,0.0,    0.0,0.0,0.0,0.0,0.0,0.0,"Ge68  ","Germanium     ","4368 ",DAY,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, (time_t) 0, (time_t) 0, (time_t) 0, (time_t) 0,
};

short SortedBuiltInIndex[BUILTINNUC] = {
                                      0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                     10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                     20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                                     30, 31, 32, 33, 34, 35, 36, 119,37, 38,
                                     39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
                                     49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
                                     59, 60, 61, 62, 66, 64, 65, 66, 67, 68,
                                     69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
                                     79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
                                     89, 90, 91, 92, 93, 94, 95, 96, 97, 98
};

static SPANELEMENTS SpanElements[]={
		"Plata         ", //"Silver        "
		"Americio      ", //"Americium     "
		"Argón         ", //"Argon         "
		"Arsénico      ", //"Arsenic       "
		"Arsénico      ", //"Arsenic       "
		"Arsénico      ", //"Arsenic       "
		"Oro           ", //Gold          "
		"Oro           ", //"Gold          ",
		"Bario         ", //Barium        "
		"Bario         ", //"Barium        "

		"Bismuto       ", //Bismuth       "
		"Bromo         ", //"Bromine       ",
		"Carbono       ", //"Carbon        ",
		"Cadmio        ", //"Cadmium       ",
		"Cerio         ", //"Cerium        "
		"Cerio         ", //"Cerium        "
		"Cobalto       ", //,"Cobalt       "
		"Cobalto       ", //"Cobalt        "
		"Cobalto       ", //"Cobalt        "
		"Cobalto       ", //"Cobalt        "

		"Cromo         ", //"Chromium      ",
		"Cesio         ", //"Cesium        ",
		"Cesio         ", //"Cesium        "
		"Cesio         ", //"Cesium        "
		"Cesio         ", //"Cesium        "
		"Cesio         ", //"Cesium        "
		"Cobre         ", //"Copper        "
		"Cobre         ", //"Copper        "
		"Disprosio     ", //"Dysprosium    "
		"Europio       ", //"Europium      "

		"Flúor         ", //"Fluorine      ",
		"Hierro        ", //"Iron          "
		"Hierro        ", //"Iron          "
		"Galio         ", //"Gallium       "
		"Galio         ", //"Gallium       "
		"Galio         ", //"Gallium       "
		"Galio         ", //"Gallium       "
		"Hafnio        ", //"Hafnium       "
		"Mercurio      ", //"Mercury       "
		"Mercurio      ", //"Mercury       "

		"Yodo          ", //"Iodine        "
		"Yodo          ", //"Iodine        "
		"Yodo          ", //"Iodine        "
		"Yodo          ", //"Iodine        "
		"Yodo          ", //"Iodine        "
		"Yodo          ", //"Iodine        ",
		"Yodo          ", //"Iodine        "
		"Indio         ", //"Indium        "
		"Indio         ", //"Indium        "
		"Indio         ", //,"Indium        "

		"Irido         ", //"Iridium       "
		"Potasio       ", //"Potassium     "
		"Potasio       ", //"Potassium     "
		"Kripón        ", //"Krypton       "
		"Kripón        ", //"Krypton       "
		"Lutecio       ", //"Lutetium      "
		"Manganeso     ", //"Manganese     "
		"Manganeso     ", //"Manganese     "
		"Manganeso     ", //"Manganese     "
		"Manganeso     ", //"Manganese     "

		"Molibdeno     ", //"Molybdenum    "
		"Nitrógeno     ", //"Nitrogen      "
		"Sodio         ", //"Sodium        "
		"Sodio         ", //"Sodium        "
		"Neodimio      ", //"Neodymium     "
		"Neptunio      ", //"Neptunium     "
		"Oxígeno       ", //"Oxygen        "
		"Fósforo       ", //"Phosphorus    "
		"Plomo         ", //"Lead          "
		"Plomo         ", //","Lead        "

		"Radio         ", //"Radium        "
		"Rubidio       ", //"Rubidium      "
		"Rubidio       ", //"Rubidium      "
		"Rubidio       ", //"Rubidium      "
		"Antimonio     ", //"Antimony      "
		"Antimonio     ", //"Antimony      "
		"Antimonio     ", //"Antimony      "
		"Escandio      ", //"Scandium      "
		"Escandio      ", //"Scandium      "
		"Selenio       ", //"Selenium      "

		"Estaño        ", //"Tin           "
		"Estroncio     ", //"Strontium     "
		"Estroncio     ", //"Strontium     "
		"Estroncio     ", //"Strontium     "
		"Tecnecio      ", //"Technetium    "
		"Talio         ", //"Thallium      "
		"Vanadio       ", //"Vanadium      "
		"Xenón         ", //"Xenon         "
		"Xenón         ", //"Xenon         "
		"Itrio         ", //"Yttrium       "

		"Itrio         ", //"Yttrium       "
		"Itrio         ", //"Yttrium       "
		"Iterbio       ", // "Ytterbium     "
		"Zinc          ", //"Zinc          "
		"Zinc          ", //"Zinc          "
		"              ", //"MoCAP ","              "
		"              ", //"MoCAN ","              "
		"              ", //"TcCAPM","              ",
		"              ", //"TcCAPB","              "

};

static NUCDATA userNuclideMirror[20];
static USERCAL userCalMirror[MAX_NEW_CAL];
static SETUP_EFFICIENCY userEfficiencyMirror[USERNUC];
static short userEfficiencyCount;

const KEYDEF keydef[]={
	"Tc99m","O 15",
	"In111","N 13",
	"Ga67","C 11",
	"F 18","F 18",
	"I 123","",
	"I 131","",
	"Xe133","",
	"Tl201","",
};

void trim(char *acByte);
void trim_and_shrink(char *acByte);

short NuclideData_getNumOfNucs(void){
	//return(sizeof(nucldata)/sizeof(nucldata[0]));
	return USERNUC;
}

short NuclideData_getNucIndexFromButtonKey(short key, short chambertype){
	short index;
	char name[8];

	if(chambertype == C_CHAMB || chambertype == K_CHAMB) strcpy(name, &keydef[key].keyiso[R_CHAMB].iso_name[0]);
	else strcpy(name, &keydef[key].keyiso[chambertype].iso_name[0]);
	index = NuclideData_getIndexFromName(name);
	return index;
}

char NuclideData_isShow(short index){
	if(index >= ALLNUC) return 0;
	else return nuclideData[index].show;
}

void NuclideData_getName(short index, char *name){
	if(index >= ALLNUC) *name = 0;
	else strcpy(name, nuclideData[index].name);
}

void NuclideData_getFullName(short index, char *fullname){
	if(index >= ALLNUC) *fullname = 0;
	else{
		if(index >= USERNUC){
			strcpy(fullname, nuclideData[index].fullname);
		}else{
			if(current.language == 0) strcpy(fullname, nuclideData[index].fullname);
			else if(current.language == 1) strcpy(fullname, SpanElements[index].fullname);
		}
		trim(fullname);
	}
}

void NuclideData_getExtendedName(short index, char *extendedname){
	char acMsg[30];

	if(index >= ALLNUC) *extendedname = 0;
	else{
		strcpy(acMsg, nuclideData[index].name);
		trim_and_shrink(acMsg);
		strcpy(extendedname, acMsg);
		strcat(extendedname, " (");
		if(index >= USERNUC){
			strcpy(acMsg, nuclideData[index].fullname);
		}else{
			if(current.language == 0) strcpy(acMsg, nuclideData[index].fullname);
			else if(current.language == 1) strcpy(acMsg, SpanElements[index].fullname);
		}
		trim_and_shrink(acMsg);
		strcat(extendedname, acMsg);
		strcat(extendedname, ")  ");

		sprintf(acMsg, "%.2f", nuclideData[index].halflife);
		switch(nuclideData[index].hlunit){
			case SEC:
				strcat(acMsg, " sec");
				break;

			case MIN:
				strcat(acMsg, " min");
				break;

			case HOUR:
				strcat(acMsg, " hr");
				break;

			case DAY:
				strcat(acMsg, " day");
				break;

			case YEAR:
				if(nuclideData[index].halflife > 1.0e+6){
					sprintf(acMsg, "%.2f", nuclideData[index].halflife / 1.0e+6);
					strcat(acMsg, " Myr");
				}else{
					strcat(acMsg, " yr");
				}
				break;
		}
		strcat(extendedname, acMsg);
	}
}

float NuclideData_getHalflife(short index){
	if(index >= ALLNUC) return -1;
	else return  nuclideData[index].halflife;
}

short NuclideData_getHalflifeUnit(short index){
	if(index >= ALLNUC) return -1;
	else return nuclideData[index].hlunit;
}

float NuclideData_getResponse(short index, short chambertype){
	if(index >= ALLNUC) return 0.0;
	else return nuclideData[index].response[chambertype];
}

float NuclideData_getUserResponse(short index, short chambertype){
	if(index >= ALLNUC) return 0.0;
	else return nuclideData[index].user_response[chambertype];
}

float NuclideData_getEffectiveResponse(short index, short chambertype){
	float response;

	if(index >= ALLNUC) return 0.0;
	else{
		response = nuclideData[index].user_response[chambertype];
		if(response == 0.0) response = nuclideData[index].response[chambertype];
	}
	return response;
}
float NuclideData_getPrimary(short index){
	if(index >= ALLNUC) return 0.0;
	else return nuclideData[index].energy1;
}

float NuclideData_getUserPrimary(short index){
	if(index >= ALLNUC) return 0.0;
	else return nuclideData[index].userenergy1;
}

float NuclideData_getEffectivePrimary(short index){
	float primary;

	if(index >= ALLNUC) return 0.0;
	else{
		primary = nuclideData[index].userenergy1;
		if(primary == -1.0) return nuclideData[index].energy1;
		else return primary;
	}
}

float NuclideData_getSecondary(short index){
	if(index >= ALLNUC) return 0.0;
	else return nuclideData[index].energy2;
}

float NuclideData_getUserSecondary(short index){
	if(index >= ALLNUC) return 0.0;
	else return nuclideData[index].userenergy2;
}

float NuclideData_getEffectiveSecondary(short index){
	float secondary;

	if(index >= ALLNUC) return 0.0;
	else{
		secondary = nuclideData[index].userenergy2;
		if(secondary == -1.0) return nuclideData[index].energy2;
		else return secondary;
	}
}

float NuclideData_getTertiary(short index){
	if(index >= ALLNUC) return 0.0;
	else return nuclideData[index].energy3;
}

float NuclideData_getUserTertiary(short index){
	if(index >= ALLNUC) return 0.0;
	else return nuclideData[index].userenergy3;
}

float NuclideData_getEffectiveTertiary(short index){
	float tertiary;

	if(index >= ALLNUC) return 0.0;
	else{
		tertiary = nuclideData[index].userenergy3;
		if(tertiary == -1.0) return nuclideData[index].energy3;
		else return tertiary;
	}
}

float NuclideData_getEff(short index, int detectorType){
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].wellefficiency;

			case DET_BETA:
				return nuclideData[index].betaefficiency;

			case DET_PROBE700:
				return nuclideData[index].probe700efficiency;

			case DET_WELL700:
				return nuclideData[index].well700efficiency;

			default:
				return 0.0;
		}
	}
}

float NuclideData_getEff_DrilledProbe_Well(short index, int detectorType){
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].wellefficiency;

			case DET_BETA:
				return nuclideData[index].betaefficiency;

			case DET_PROBE700:
				return nuclideData[index].probe700efficiency;

			case DET_WELL700:
			case DET_DRILLEDPROBE700:
				return nuclideData[index].well700efficiency;

			default:
				return 0.0;
		}
	}
}

float NuclideData_getEff_DrilledProbe_Probe(short index, int detectorType){
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].wellefficiency;

			case DET_BETA:
				return nuclideData[index].betaefficiency;

			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
				return nuclideData[index].probe700efficiency;

			case DET_WELL700:
				return nuclideData[index].well700efficiency;

			default:
				return 0.0;
		}
	}
}

float NuclideData_getEffSkewWell(short index, int detectorType){
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].wellefficiency;

			case DET_BETA:
				return nuclideData[index].betaefficiency;

			case DET_WELL700:
			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
				return nuclideData[index].well700efficiency;

			default:
				return 0.0;
		}
	}
}

float NuclideData_getEffSkewProbe(short index, int detectorType){
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].wellefficiency;

			case DET_BETA:
				return nuclideData[index].betaefficiency;

			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
			case DET_WELL700:
				return nuclideData[index].probe700efficiency;

			default:
				return 0.0;
		}
	}
}

float NuclideData_getUserEff(short index, int detectorType){
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].userwellefficiency;

			case DET_BETA:
				return nuclideData[index].userbetaefficiency;

			case DET_PROBE700:
				return nuclideData[index].userprobe700efficiency;

			case DET_WELL700:
				return nuclideData[index].userwell700efficiency;

			default:
				return 0.0;
		}
	}
}

float NuclideData_getUserEff_DrilledProbe_Well(short index, int detectorType){
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].userwellefficiency;

			case DET_BETA:
				return nuclideData[index].userbetaefficiency;

			case DET_PROBE700:
				return nuclideData[index].userprobe700efficiency;

			case DET_WELL700:
			case DET_DRILLEDPROBE700:
				return nuclideData[index].userwell700efficiency;

			default:
				return 0.0;
		}
	}
}

float NuclideData_getUserEff_DrilledProbe_Probe(short index, int detectorType){
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].userwellefficiency;

			case DET_BETA:
				return nuclideData[index].userbetaefficiency;

			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
				return nuclideData[index].userprobe700efficiency;

			case DET_WELL700:
				return nuclideData[index].userwell700efficiency;

			default:
				return 0.0;
		}
	}
}

float NuclideData_getUserEffSkewWell(short index, int detectorType){
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].userwellefficiency;

			case DET_BETA:
				return nuclideData[index].userbetaefficiency;

			case DET_WELL700:
			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
				return nuclideData[index].userwell700efficiency;

			default:
				return 0.0;
		}
	}
}

float NuclideData_getUserEffSkewProbe(short index, int detectorType){
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].userwellefficiency;

			case DET_BETA:
				return nuclideData[index].userbetaefficiency;

			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
			case DET_WELL700:
				return nuclideData[index].userprobe700efficiency;

			default:
				return 0.0;
		}
	}
}

float NuclideData_getEffectiveEff(short index, int detectorType){
	float efficiency;
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				efficiency = nuclideData[index].userwellefficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].wellefficiency;
				break;

			case DET_BETA:
				efficiency = nuclideData[index].userbetaefficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].betaefficiency;
				break;

			case DET_PROBE700:
				efficiency = nuclideData[index].userprobe700efficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].probe700efficiency;
				break;

			case DET_WELL700:
				efficiency = nuclideData[index].userwell700efficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].well700efficiency;
				break;

			default:
				efficiency = 0.0;
				break;
		}
		return efficiency;
	}
}

float NuclideData_getEffectiveEff_DrilledProbe_Well(short index, int detectorType){
	float efficiency;
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				efficiency = nuclideData[index].userwellefficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].wellefficiency;
				break;

			case DET_BETA:
				efficiency = nuclideData[index].userbetaefficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].betaefficiency;
				break;

			case DET_PROBE700:
				efficiency = nuclideData[index].userprobe700efficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].probe700efficiency;
				break;

			case DET_WELL700:
			case DET_DRILLEDPROBE700:
				efficiency = nuclideData[index].userwell700efficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].well700efficiency;
				break;

			default:
				efficiency = 0.0;
				break;
		}
		return efficiency;
	}
}

float NuclideData_getEffectiveEff_DrilledProbe_Probe(short index, int detectorType){
	float efficiency;
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				efficiency = nuclideData[index].userwellefficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].wellefficiency;
				break;

			case DET_BETA:
				efficiency = nuclideData[index].userbetaefficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].betaefficiency;
				break;

			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
				efficiency = nuclideData[index].userprobe700efficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].probe700efficiency;
				break;

			case DET_WELL700:
				efficiency = nuclideData[index].userwell700efficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].well700efficiency;
				break;

			default:
				efficiency = 0.0;
				break;
		}
		return efficiency;
	}
}

float NuclideData_getEffectiveEffSkewWell(short index, int detectorType){
	float efficiency;
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				efficiency = nuclideData[index].userwellefficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].wellefficiency;
				break;

			case DET_BETA:
				efficiency = nuclideData[index].userbetaefficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].betaefficiency;
				break;

			case DET_WELL700:
			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
				efficiency = nuclideData[index].userwell700efficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].well700efficiency;
				break;

			default:
				efficiency = 0.0;
				break;
		}
		return efficiency;
	}
}

float NuclideData_getEffectiveEffSkewProbe(short index, int detectorType){
	float efficiency;
	if(index >= ALLNUC) return 0.0;
	else{
		switch(detectorType){
			case DET_WELL:
				efficiency = nuclideData[index].userwellefficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].wellefficiency;
				break;

			case DET_BETA:
				efficiency = nuclideData[index].userbetaefficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].betaefficiency;
				break;

			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
			case DET_WELL700:
				efficiency = nuclideData[index].userprobe700efficiency;
				if(efficiency == -1.0) efficiency = nuclideData[index].probe700efficiency;
				break;

			default:
				efficiency = 0.0;
				break;
		}
		return efficiency;
	}
}

time_t NuclideData_getMeasuredOnStamp(short index, int detectorType){
	if(index >= ALLNUC) return (time_t) 0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].wellmeasuredon;

			case DET_BETA:
				return nuclideData[index].betameasuredon;

			case DET_PROBE700:
				return nuclideData[index].probe700measuredon;

			case DET_WELL700:
				return nuclideData[index].well700measuredon;

			default:
				return (time_t) 0;
		}
	}
}

time_t NuclideData_getMeasuredOnStamp_DrilledProbe_Well(short index, int detectorType){
	if(index >= ALLNUC) return (time_t) 0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].wellmeasuredon;

			case DET_BETA:
				return nuclideData[index].betameasuredon;

			case DET_PROBE700:
				return nuclideData[index].probe700measuredon;

			case DET_WELL700:
			case DET_DRILLEDPROBE700:
				return nuclideData[index].well700measuredon;

			default:
				return (time_t) 0;
		}
	}
}

time_t NuclideData_getMeasuredOnStamp_DrilledProbe_Probe(short index, int detectorType){
	if(index >= ALLNUC) return (time_t) 0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].wellmeasuredon;

			case DET_BETA:
				return nuclideData[index].betameasuredon;

			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
				return nuclideData[index].probe700measuredon;

			case DET_WELL700:
				return nuclideData[index].well700measuredon;

			default:
				return (time_t) 0;
		}
	}
}

time_t NuclideData_getMeasuredOnStampSkewWell(short index, int detectorType){
	if(index >= ALLNUC) return (time_t) 0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].wellmeasuredon;

			case DET_BETA:
				return nuclideData[index].betameasuredon;

			case DET_WELL700:
			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
				return nuclideData[index].well700measuredon;

			default:
				return (time_t) 0;
		}
	}
}

time_t NuclideData_getMeasuredOnStampSkewProbe(short index, int detectorType){
	if(index >= ALLNUC) return (time_t) 0;
	else{
		switch(detectorType){
			case DET_WELL:
				return nuclideData[index].wellmeasuredon;

			case DET_BETA:
				return nuclideData[index].betameasuredon;

			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
			case DET_WELL700:
				return nuclideData[index].probe700measuredon;

			default:
				return (time_t) 0;
		}
	}
}

/* transfer nuclide data to RAM */
void NuclideData_getNuclide(short index, NUCDATA *nuclide){
	NUCDATA emptynuclide;

	if(index >= ALLNUC){
		emptynuclide.show = 0;
		emptynuclide.halflife = -1;
		emptynuclide.response[R_CHAMB] = 0.0;
		emptynuclide.response[P_CHAMB] = 0.0;
		emptynuclide.response[B_CHAMB] = 0.0;
		emptynuclide.response[ONE_DOT_EIGHT_CHAMB] = 0.0;
		emptynuclide.response[C_CHAMB] = 0.0;
		emptynuclide.response[K_CHAMB] = 0.0;
		emptynuclide.user_response[R_CHAMB] = 0.0;
		emptynuclide.user_response[P_CHAMB] = 0.0;
		emptynuclide.user_response[B_CHAMB] = 0.0;
		emptynuclide.user_response[ONE_DOT_EIGHT_CHAMB] = 0.0;
		emptynuclide.user_response[C_CHAMB] = 0.0;
		emptynuclide.user_response[K_CHAMB] = 0.0;
		emptynuclide.name[0] = 0;
		emptynuclide.fullname[0] = 0;
		emptynuclide.code[0] = 0;
		emptynuclide.hlunit = -1;
		memcpy(nuclide, &emptynuclide, sizeof(NUCDATA));
	}else{
		memcpy(nuclide, &(nuclideData[index]), sizeof(NUCDATA));
	}
}

bool NuclideData_testNuclideTable(ushort *val){
	uchar *addr;
	short nbytes;
	short i;
	ushort crc;
	uchar ch;
    bool ok;
    NUCDATA nucCopy[USERNUC];

    memcpy(nucCopy, nuclideData, sizeof(nucCopy));
    for(i=0; i<USERNUC; i++){
    	nucCopy[i].user_response[R_CHAMB] = 0.0;
    	nucCopy[i].user_response[P_CHAMB] = 0.0;
    	nucCopy[i].user_response[B_CHAMB] = 0.0;
    	nucCopy[i].user_response[ONE_DOT_EIGHT_CHAMB] = 0.0;
    	nucCopy[i].user_response[C_CHAMB] = 0.0;
    	nucCopy[i].user_response[K_CHAMB] = 0.0;
    	nucCopy[i].userenergy1 = 0.0;
    	nucCopy[i].userenergy2 = 0.0;
    	nucCopy[i].userenergy3 = 0.0;
    	nucCopy[i].userwellefficiency = 0.0;
    	nucCopy[i].userbetaefficiency = 0.0;
    	nucCopy[i].userprobe700efficiency = 0.0;
    	nucCopy[i].userwell700efficiency = 0.0;
    	nucCopy[i].wellmeasuredon = (time_t) 0;
    	nucCopy[i].betameasuredon = (time_t) 0;
    	nucCopy[i].probe700measuredon = (time_t) 0;
    	nucCopy[i].well700measuredon = (time_t) 0;
    }
	generate_crc_table();

	//start at nucldata
	addr = (uchar *) nucCopy;
	nbytes = sizeof(nucCopy);

	crc = 0;
	for(i=0; i<nbytes; i++){
		ch = *addr++;
		crc = Crc16(ch,crc);
	}

    ok = (crc == DAILY_MEMORY);
    if (!ok) *val = crc;
    else *val = DAILY_MEMORY;

    return ok;
}

//get nuclide index from name -- built-in nuclides only
short NuclideData_getIndexFromName(char *name){
	short ii;
	short len;
    short index;

    len = strlen(name);

    index = -1;
    for(ii=0; ii<BUILTINNUC; ii++){
    	if (strncmp(&nuclideData[SortedBuiltInIndex[ii]].name[0], name, len) == 0){
    		index = SortedBuiltInIndex[ii];
    		break;
    	}
    }
    return index;
}

short NuclideData_getIndexFromNameIncludingUser(char *name){
	short ii;
	//short len;
    short index;
    short maxnuc;
    char nucnametrim[10];

    maxnuc = NuclideData_getNumOfNucs();
    //len = strlen(name);

    index = -1;
    for(ii=0; ii<maxnuc+10; ii++){
    	strcpy(nucnametrim, &(nuclideData[ii].name[0]));
    	if (strcmp(nucnametrim, name) == 0){
    		index = ii;
    		break;
    	}
    }

    if(index == -1){
		for(ii=maxnuc+20; ii<ALLNUC; ii++){
			strcpy(nucnametrim, &(nuclideData[ii].name[0]));
			if (strcmp(nucnametrim, name) == 0){
				index = ii;
				break;
			}
		}
    }
    return index;
}

void NuclideData_initializeMirror(void){
	short int i;

	// Replace with getting user added Cal Nuclide from database
    //EE_READ(usernucl, (uchar *) userNuclideMirror);
	DB_ReadAllUserNuclide(userNuclideMirror);
	for(i=0; i<20; i++){
    	memcpy(&(nuclideData[USERNUC + i]), &(userNuclideMirror[i]), sizeof(NUCDATA));
    	if(nuclideData[USERNUC + i].hlunit != -1) nuclideData[USERNUC + i].show = -1;
    	else nuclideData[USERNUC + i].show = 0;
    }

    // Replace with getting user Cal Num from database
    //EE_READ(usercal, (uchar *) userCalMirror);
    DB_ReadAllUserCal(userCalMirror);
    for(i=0; i<MAX_NEW_CAL; i++){
    	/*if(i==1){
    		userCalMirror[i].ch_type = 0;
    		userCalMirror[i].nuc_index = 79;
    		userCalMirror[i].response = 0.2331;
    	}*/
    	if(userCalMirror[i].nuc_index != -1){
    		nuclideData[userCalMirror[i].nuc_index].user_response[userCalMirror[i].ch_type] = userCalMirror[i].response;
    	}
    }

    DB_ReadAllUserEff(userEfficiencyMirror);
    NuclideData_initializeEfficiencyMirror();
}

void NuclideData_populateSetupStruct(short index, SETUP_NUCLIDE *setupnuclide){
	char temp[10];

	if(userNuclideMirror[index].hlunit == -1){
		strcpy(setupnuclide->acRCalNum, "");
		strcpy(setupnuclide->acPCalNum, "");
		strcpy(setupnuclide->acCCalNum, "");
		strcpy(setupnuclide->acKCalNum, "");
		strcpy(setupnuclide->acElement, "");
		strcpy(setupnuclide->acName, "");
		setupnuclide->fHalflife = -1;
		setupnuclide->sHalflifeUnit = -1;
	}else{
		strcpy(setupnuclide->acElement, userNuclideMirror[index].fullname);
		trim(setupnuclide->acElement);
		strcpy(setupnuclide->acName, userNuclideMirror[index].name);
		trim(setupnuclide->acName);
		setupnuclide->fHalflife = userNuclideMirror[index].halflife;
		setupnuclide->sHalflifeUnit = userNuclideMirror[index].hlunit;

		if(userNuclideMirror[index].response[R_CHAMB] == 0.0) temp[0] = 0;
		else DisplayNucCal_getCalNumFromResponse(userNuclideMirror[index].response[R_CHAMB], R_CHAMB, temp);
		trim(temp);
		strcpy(setupnuclide->acRCalNum, temp);

		if(userNuclideMirror[index].response[P_CHAMB] == 0.0) temp[0] = 0;
		else DisplayNucCal_getCalNumFromResponse(userNuclideMirror[index].response[P_CHAMB], P_CHAMB, temp);
		trim(temp);
		strcpy(setupnuclide->acPCalNum, temp);

		if(userNuclideMirror[index].response[C_CHAMB] == 0.0) temp[0] = 0;
		else DisplayNucCal_getCalNumFromResponse(userNuclideMirror[index].response[C_CHAMB], C_CHAMB, temp);
		trim(temp);
		strcpy(setupnuclide->acCCalNum, temp);

		if(userNuclideMirror[index].response[K_CHAMB] == 0.0) temp[0] = 0;
		else DisplayNucCal_getCalNumFromResponse(userNuclideMirror[index].response[K_CHAMB], K_CHAMB, temp);
		trim(temp);
		strcpy(setupnuclide->acKCalNum, temp);
	}
}

char NuclideData_updateMirror(short index, SETUP_NUCLIDE *setupnuclide){
	char setupvalid;
	char returnvalue;
	float response;

	returnvalue = 0;
	setupvalid = 1;
	if(strlen(setupnuclide->acName) == 0) setupvalid = 0;
	if(strlen(setupnuclide->acElement) == 0) setupvalid = 0;
	if((strlen(setupnuclide->acRCalNum) == 0) && (strlen(setupnuclide->acPCalNum) == 0) && (strlen(setupnuclide->acCCalNum) == 0) && (strlen(setupnuclide->acKCalNum) == 0)) setupvalid = 0;
	if(setupnuclide->sHalflifeUnit == -1) setupvalid = 0;

	if(setupvalid == 1){
		if(strlen(setupnuclide->acRCalNum) != 0) response = NuclideCal_getResponse(setupnuclide->acRCalNum, R_CHAMB);
		if(response < 0) setupvalid = 0;
		if(strlen(setupnuclide->acPCalNum) != 0) response = NuclideCal_getResponse(setupnuclide->acPCalNum, P_CHAMB);
		if(response < 0) setupvalid = 0;
		if(strlen(setupnuclide->acCCalNum) != 0) response = NuclideCal_getResponse(setupnuclide->acCCalNum, C_CHAMB);
		if(response < 0) setupvalid = 0;
		if(strlen(setupnuclide->acKCalNum) != 0) response = NuclideCal_getResponse(setupnuclide->acKCalNum, K_CHAMB);
		if(response < 0) setupvalid = 0;
	}

	if(setupvalid == 0){
		if(userNuclideMirror[index].hlunit != -1){
			userNuclideMirror[index].show = 0;
			userNuclideMirror[index].halflife = -1;
			userNuclideMirror[index].response[R_CHAMB] = 0.0;
			userNuclideMirror[index].response[P_CHAMB] = 0.0;
			userNuclideMirror[index].response[B_CHAMB] = 0.0;
			userNuclideMirror[index].response[ONE_DOT_EIGHT_CHAMB] = 0.0;
			userNuclideMirror[index].response[C_CHAMB] = 0.0;
			userNuclideMirror[index].response[K_CHAMB] = 0.0;
			userNuclideMirror[index].user_response[R_CHAMB] = 0.0;
			userNuclideMirror[index].user_response[P_CHAMB] = 0.0;
			userNuclideMirror[index].user_response[B_CHAMB] = 0.0;
			userNuclideMirror[index].user_response[ONE_DOT_EIGHT_CHAMB] = 0.0;
			userNuclideMirror[index].user_response[C_CHAMB] = 0.0;
			userNuclideMirror[index].user_response[K_CHAMB] = 0.0;
			userNuclideMirror[index].name[0] = 0;
			userNuclideMirror[index].fullname[0] = 0;
			userNuclideMirror[index].code[0] = 0;
			userNuclideMirror[index].hlunit = -1;
			// Save in Database
			//EE_WRITE(usernucl[index], (uchar *) &(userNuclideMirror[index]));
			DB_CreateUserNuclide(index, &(userNuclideMirror[index]));
			memcpy(&(nuclideData[USERNUC + index]), &(userNuclideMirror[index]), sizeof(NUCDATA));
			returnvalue = 1;
		}
	}else{
		userNuclideMirror[index].show = -1;
		userNuclideMirror[index].halflife = setupnuclide->fHalflife;
		if(strlen(setupnuclide->acRCalNum) == 0) userNuclideMirror[index].response[R_CHAMB] = 0.0;
		else userNuclideMirror[index].response[R_CHAMB] = NuclideCal_getResponse(setupnuclide->acRCalNum, R_CHAMB);
		if(strlen(setupnuclide->acPCalNum) == 0) userNuclideMirror[index].response[P_CHAMB] = 0.0;
		else userNuclideMirror[index].response[P_CHAMB] = NuclideCal_getResponse(setupnuclide->acPCalNum, P_CHAMB);
		if(strlen(setupnuclide->acCCalNum) == 0) userNuclideMirror[index].response[C_CHAMB] = 0.0;
		else userNuclideMirror[index].response[C_CHAMB] = NuclideCal_getResponse(setupnuclide->acCCalNum, C_CHAMB);
		if(strlen(setupnuclide->acKCalNum) == 0) userNuclideMirror[index].response[K_CHAMB] = 0.0;
		else userNuclideMirror[index].response[K_CHAMB] = NuclideCal_getResponse(setupnuclide->acKCalNum, K_CHAMB);
		userNuclideMirror[index].response[B_CHAMB] = 0.0;
		userNuclideMirror[index].response[ONE_DOT_EIGHT_CHAMB] = 0.0;
		userNuclideMirror[index].user_response[R_CHAMB]=0.0;
		userNuclideMirror[index].user_response[P_CHAMB]=0.0;
		userNuclideMirror[index].user_response[B_CHAMB]=0.0;
		userNuclideMirror[index].user_response[ONE_DOT_EIGHT_CHAMB]=0.0;
		userNuclideMirror[index].user_response[C_CHAMB]=0.0;
		userNuclideMirror[index].user_response[K_CHAMB]=0.0;
		strcpy(userNuclideMirror[index].name, setupnuclide->acName);
		strcpy(userNuclideMirror[index].fullname, setupnuclide->acElement);
		userNuclideMirror[index].code[0]=0;
		userNuclideMirror[index].hlunit=setupnuclide->sHalflifeUnit;
		// Save in Database
		//EE_WRITE(usernucl[index], (uchar *) &(userNuclideMirror[index]));
		DB_CreateUserNuclide(index, &(userNuclideMirror[index]));
		memcpy(&(nuclideData[USERNUC + index]), &(userNuclideMirror[index]), sizeof(NUCDATA));
		returnvalue = 1;
	}

	return returnvalue;
}

void NuclideData_getDisplayName(short index, short chambertype, char *displayname){
	if(index >= ALLNUC) *displayname = 0;
	else{
		if(NuclideData_getUserResponse(index, chambertype) != 0.0) strcpy(displayname, "*");
		else strcpy(displayname, "");
		strcat(displayname, nuclideData[index].name);
		trim_and_shrink(displayname);
	}
}

char NuclideData_updateUserCalNum(short index, short chambertype, char *usercalnum){
	char returnvalue;
	short i;
	float response;

	if(strlen(usercalnum) == 0){
		nuclideData[index].user_response[chambertype] = 0.0;
		for(i=0; i<MAX_NEW_CAL; i++){
			if((userCalMirror[i].nuc_index == index) && (userCalMirror[i].ch_type == chambertype)){
				userCalMirror[i].nuc_index = -1;
				//EE_WRITE(usercal[i].nuc_index, (uchar *) &(userCalMirror[i].nuc_index));
				DB_CreateUserCal(i, &(userCalMirror[i]));
			}
		}
		returnvalue = 0;
	}else{
		response = NuclideCal_getResponse(usercalnum, chambertype);
		if(response == -1.0){
			returnvalue = -1;
		}else{
			nuclideData[index].user_response[chambertype] = response;
			for(i=0; i<MAX_NEW_CAL; i++){
				if((userCalMirror[i].nuc_index == index) && (userCalMirror[i].ch_type == chambertype)){
					userCalMirror[i].nuc_index = -1;
					//EE_WRITE(usercal[i].nuc_index, (uchar *) &(userCalMirror[i].nuc_index));
					DB_CreateUserCal(i, &(userCalMirror[i]));
				}
			}

			returnvalue = -2;
			for(i=0; i<MAX_NEW_CAL; i++){
				if(userCalMirror[i].nuc_index == -1){
					userCalMirror[i].nuc_index = index;
					userCalMirror[i].ch_type = chambertype;
					userCalMirror[i].response = response;
					//EE_WRITE(usercal[i], (uchar *) &(userCalMirror[i]));
					DB_CreateUserCal(i, &(userCalMirror[i]));
					returnvalue = 0;
					break;
				}
			}

			if(returnvalue == -2) nuclideData[index].user_response[chambertype] = 0.0;
		}
	}
	return returnvalue;
}

void NuclideData_clearEfficiencyMirror(void){
	short index;
	SETUP_EFFICIENCY empty;

	empty.nuclideID = -1;
	empty.userEnergy1 = -1;
	empty.userEnergy2 = -1;

	empty.userEnergy3 = -1;
	empty.userWellEff = -1;
	empty.userBetaEff = -1;
	empty.userProbe700Eff = -1;
	empty.userWell700Eff = -1;
	empty.wellStamp = (time_t) 0;
	empty.betaStamp = (time_t) 0;
	empty.probe700Stamp = (time_t) 0;
	empty.well700Stamp = (time_t) 0;

	for(index=0; index<USERNUC; index++) memcpy(&(userEfficiencyMirror[index]), &empty, sizeof(SETUP_EFFICIENCY));
}

void NuclideData_initializeEfficiencyMirror(void){
	short index;

	userEfficiencyCount = 0;
	if(userEfficiencyMirror[0].nuclideID != -1){
		for(index=0; index<USERNUC; index++){
			if(userEfficiencyMirror[index].nuclideID != -1){
				nuclideData[userEfficiencyMirror[index].nuclideID].userenergy1 = userEfficiencyMirror[index].userEnergy1;
				nuclideData[userEfficiencyMirror[index].nuclideID].userenergy2 = userEfficiencyMirror[index].userEnergy2;
				nuclideData[userEfficiencyMirror[index].nuclideID].userenergy3 = userEfficiencyMirror[index].userEnergy3;
				nuclideData[userEfficiencyMirror[index].nuclideID].userwellefficiency = userEfficiencyMirror[index].userWellEff;
				nuclideData[userEfficiencyMirror[index].nuclideID].userbetaefficiency = userEfficiencyMirror[index].userBetaEff;
				nuclideData[userEfficiencyMirror[index].nuclideID].userprobe700efficiency = userEfficiencyMirror[index].userProbe700Eff;
				nuclideData[userEfficiencyMirror[index].nuclideID].userwell700efficiency = userEfficiencyMirror[index].userWell700Eff;
				nuclideData[userEfficiencyMirror[index].nuclideID].wellmeasuredon = userEfficiencyMirror[index].wellStamp;
				nuclideData[userEfficiencyMirror[index].nuclideID].betameasuredon = userEfficiencyMirror[index].betaStamp;
				nuclideData[userEfficiencyMirror[index].nuclideID].probe700measuredon = userEfficiencyMirror[index].probe700Stamp;
				nuclideData[userEfficiencyMirror[index].nuclideID].well700measuredon = userEfficiencyMirror[index].well700Stamp;
				userEfficiencyCount++;
			}else{
				break;
			}
		}
	}
}

void NuclideData_updateEfficiencyMirror(SETUP_EFFICIENCY *temp){
	short index, blankindex;
	bool found;
	SETUP_EFFICIENCY empty;
	SETUP_EFFICIENCY dup[USERNUC];

	memcpy(dup, userEfficiencyMirror, USERNUC * sizeof(SETUP_EFFICIENCY));

	empty.nuclideID = -1;
	empty.userEnergy1 = -1;
	empty.userEnergy2 = -1;
	empty.userEnergy3 = -1;
	empty.userWellEff = -1;
	empty.userBetaEff = -1;
	empty.userProbe700Eff = -1;
	empty.userWell700Eff = -1;
	empty.wellStamp = (time_t) 0;
	empty.betaStamp = (time_t) 0;
	empty.probe700Stamp = (time_t) 0;
	empty.well700Stamp = (time_t) 0;

	//Update nuclideData
	nuclideData[temp->nuclideID].userenergy1 = temp->userEnergy1;
	nuclideData[temp->nuclideID].userenergy2 = temp->userEnergy2;
	nuclideData[temp->nuclideID].userenergy3 = temp->userEnergy3;
	nuclideData[temp->nuclideID].userwellefficiency = temp->userWellEff;
	nuclideData[temp->nuclideID].userbetaefficiency = temp->userBetaEff;
	nuclideData[temp->nuclideID].userprobe700efficiency = temp->userProbe700Eff;
	nuclideData[temp->nuclideID].userwell700efficiency = temp->userWell700Eff;
	nuclideData[temp->nuclideID].wellmeasuredon = temp->wellStamp;
	nuclideData[temp->nuclideID].betameasuredon = temp->betaStamp;
	nuclideData[temp->nuclideID].probe700measuredon = temp->probe700Stamp;
	nuclideData[temp->nuclideID].well700measuredon = temp->well700Stamp;

	found = FALSE;
	if((temp->userEnergy1==-1.0) && (temp->userEnergy2==-1.0) && (temp->userEnergy3==-1.0) && (temp->userWellEff==-1.0) && (temp->userBetaEff==-1.0) && (temp->userProbe700Eff==-1.0) && (temp->userWell700Eff==-1.0)){
		//remove from mirror
		if(userEfficiencyCount>0){
			for(index=0; index<userEfficiencyCount; index++){
				if(userEfficiencyMirror[index].nuclideID == (temp->nuclideID)){
					blankindex = index;
					memcpy(&(userEfficiencyMirror[index]), &empty, sizeof(SETUP_EFFICIENCY));
					found = TRUE;
					break;
				}
			}
		}

		if(found){
			// compress
			//if(blankindex != (userEfficiencyCount - 1)){
			//	for(index=blankindex+1; index<userEfficiencyCount; index++){
			//		memcpy(&(userEfficiencyMirror[index-1]), &(userEfficiencyMirror[index]), sizeof(SETUP_EFFICIENCY));
			//	}
			//	memcpy(&(userEfficiencyMirror[userEfficiencyCount - 1]), &empty, sizeof(SETUP_EFFICIENCY));
			//}

			// new compress, does not re-shuffle, but places the last entry into the blank
			if(blankindex != (userEfficiencyCount - 1)){
				memcpy(&(userEfficiencyMirror[blankindex]), &(userEfficiencyMirror[userEfficiencyCount - 1]), sizeof(SETUP_EFFICIENCY));
				memcpy(&(userEfficiencyMirror[userEfficiencyCount - 1]), &empty, sizeof(SETUP_EFFICIENCY));
			}

			userEfficiencyCount--;
		}
	}else{
		if(userEfficiencyCount>0){
			for(index=0; index<userEfficiencyCount; index++){
				if(userEfficiencyMirror[index].nuclideID == (temp->nuclideID)){
					memcpy(&(userEfficiencyMirror[index]), temp, sizeof(SETUP_EFFICIENCY));
					found = TRUE;
					break;
				}
			}
		}

		if(!found){
			// Add to end
			memcpy(&(userEfficiencyMirror[userEfficiencyCount]), temp, sizeof(SETUP_EFFICIENCY));
			userEfficiencyCount++;
			found = TRUE;
		}
	}

	// Save to SD
	for(index=0; index<USERNUC; index++){
		if(memcmp(&(dup[index]), &(userEfficiencyMirror[index]), sizeof(SETUP_EFFICIENCY)) != 0){
			DB_CreateUserEff(index, &(userEfficiencyMirror[index]));
		}
	}
}

NUCDATA *NuclideData_getNuclideDataPtr(short index){
	return &(nuclideData[index]);
}

int NuclideData_getDetectorNuclideCount(void){
	int index;
	int returnvalue;

	returnvalue = 0;
	for(index=USERNUC+10; index<USERNUC+20; index++){
		if(nuclideData[index].hlunit != -1) returnvalue++;
	}

	return returnvalue;
}

void NuclideData_updateMirror2(short index, NUCDATA *nucdata){
	memcpy(&(userNuclideMirror[index - USERNUC]), nucdata, sizeof(NUCDATA));
	memcpy(&(nuclideData[index]), nucdata, sizeof(NUCDATA));
	DB_CreateUserNuclide(index - USERNUC, nucdata);
}


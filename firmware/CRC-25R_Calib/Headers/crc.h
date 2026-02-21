/*********************************************************************

  MODULE:	GENERAL CALIBRATOR HEADER FILE for CRC-25

  FILE:		crc.h

  DATE:		10/17/06
            08/22/07 -- removed automated constancy


  *************************************************************************/
//#define DEMO 1
//#define ATTACHWELL 1
//#define LOW_LEVEL_CHAMBER 1
//#define LOW_LEVEL_WELL 1
//#define MCA_SIMULATION 1
//#define FASTSTART 1
//#define CALIB_ONLY 1
//#define FORCE_FLASH 1
//#define HOME_CALIBRATION	1
//#define DEMO_DATA 1
//#define TESTING 1
//#define TIMES1000 20
//#define ZERO_TU_PATIENT_BACKGROUND 1
//#define USE_T_WELL_AS_PROBE 1
//#define DISABLE_TU_PREDOSE_PEAK 1
//#define SIMULATE_DECAY_SOURCE 	1
//#define PRINTF_ERRORS	1
//#define WATCHDOG_OFF 1
//#define SIMULATE_DECAY_SOURCE_ADC 1
#define INVERSION 1
//#define WHITEBOX_TESTS
//#define TERMINAL

#ifndef __CRC_H__
#define __CRC_H__	1

#ifdef __GNUC__
#define __interrupt
#endif

  /* variable type definitions */
typedef unsigned char	uchar;
typedef unsigned int	uint;  	// int = long
typedef unsigned short	ushort;
typedef unsigned long	ulong;
typedef unsigned short	bool;
typedef bool	BOOL;
#ifndef _BYTE_
#define _BYTE_
typedef uchar	BYTE;
#endif

bool AmuletError_display(char *title, char *errorstring);
bool AmuletWarning_display(char *title, char *warningstring);
bool AmuletNotification_display(char *title, char *errorstring);
#define DISPLAY_ERROR(titlestring, errorstring) if(AmuletError_display(titlestring, errorstring)) return
#define DISPLAY_WARNING(titlestring, warningstring) if(AmuletWarning_display(titlestring, warningstring)) return
#define DISPLAY_NOTIFICATION(titlestring, notificationstring) if(AmuletNotification_display(titlestring, notificationstring)) return

#include <time.h>
#define NO_TIME (time_t)(-1)
#include "well.h"
#include "linearity.h"

enum{
	NUCKEY,
	USERKEY,
	CALKEY
};

//modes
enum{
	CRC_CAL,
	CRC_WELL
};

//returns from  WELL
enum{
	CAL_RET,    //return to Calibrator
	WELL_RET   //return to WELL
};

//container types
enum{
	REF,
	SYR,
	VIAL,
	CAP
};

   /* mu set in act2str */
#define	MU  '$'

#define DIVSIGN (char)0x81
#define MULT (char)'*'
#define MED_DIV ';'

#ifndef TRUE
	#define TRUE	1
	#define FALSE	0
#endif

// max # of nuclides than can have new calibration number
#define MAX_NEW_CAL	200
#define MAX_NEW_CAL_OBSOLETE	40

// max # of nuclides with container correction factors
#define MAX_CAL_COR	20

#define USERNUC		99
#define ALLNUC     120
#define BUILTINNUC 100

	/* Ci, Bq */
enum{
	CI,
	BQ
};

/* units in measurement */
enum{
	NO_UNIT,
	UCI,
	MCI,
	_CI,
	KBQ,
	MBQ,
	GBQ
};

#define BQFACTOR 3.7e+10

#define		X_ACT		30
#define		Y_ACT 		12
#define		X_FUT_ACT   10
#define		Y_FUT_ACT	49
#define		X_TIME		87
#define     Y_TIME      0
#define		Y_FUT_TIME	44
#define		Y_FUT_DATE	55
#define		Y_FUT_PRMT	54

/* units for half-life */
enum{
	SEC,
	MIN,
	HOUR,
	DAY,
	YEAR
};

/* measurement modes */
enum MeasMode{
	MEASMODE,
	TESTMODE,
	ZEROMODE,
	BKGMODE,
	MOBKGMODE,
	MOASSMODE
};

/* calibrator standard source structure */
typedef struct stnd STAND;
struct stnd{
	float act;		// calibrated activity
	time_t caldate;	// calibration date (0 for no source)
	bool daily;		//daily source?
	char sn[11];	// serial number
	char nucnum;	//for Well: Cs or Ba
	bool const_iso;	//constancy?
};

//Mo/Tc data
#define MoTcHl 6.6094
#define MoTcHlUnit HOUR

enum PCCOMM
{
    PC_COMM_NONE,
    PC_COMM_RS232,
    PC_COMM_USB,
    PC_COMM_USB_DEBUG_CHAMBER,
    PC_COMM_USB_UPDATE,
    PC_COMM_USB_DEBUG_WELL,
    PC_COMM_USB_UPLOAD_FILE
};

enum detector_types
{
    DETECTOR_NONE,
    DETECTOR_WELL,
};

enum language
{
	ENGLISH,
	FRENCH,
};	

/* current data structure  -- these pertain to whole system */
 typedef struct current CURRENT;
 struct current
 {
     uchar system;			// Ci or Bq
     uchar lock;            //locked to Ci or Bq
     short num_chambers;    //number of chambers
     short main_chamber;	    //chamber for Main display
     short detector;        //0 = None, 1 = Well
     bool enhanced;          //enhanced tests & inventory
     short pccomm;           //pc communications option? -- see enum
     short source_index;    //index for source key
     bool  demo_mode;       //is it in demo mode?
     char   printer;        //current printer.
     uchar comm_hotkey;		// 0 = R Hotkeys, 1 = P Hotkeys
     short branding;		// 0 = CRC-55t, 1 = Caprac-t, 2 = CRC-700t, 3 = CRC-77t
     int user_id;			// Currently logged in
     int security_mode;		// Current security mode	// 0 - Disabled, 1 - Traditional, 2 - Enhanced
     int default_keV;		// 0 = Allow User keV, 1 = Use Only Default keV
     int time_format;		// 0 = mm/dd/yyyy, 1 = dd/mm/yyyy, 2 = yyyy/mm/dd
     int usb_device_protocol;	// 0 = HID, 1 = CDC
     int future_date_input;	// 0 = Quick, 1 = Full
	 int language;			//  0 = English, 1 = French
	 char feed_label;		// 0 = Off, 1 = On
	 char moly_streamlined;	// 0 = Off, 1 = On
	 long long int key;		// 0 = empty
	 bool pccomm_enabled;	// 0 = Communications disabled, Other = Communications enabled
	 char password[10];		// Password
	 int bypass;			// Bypass
 };

/* chamber values -- 1 for each chamber */
typedef struct chambervals CHAMBERVALS;
struct chambervals
{
    short   chamb_type;       //CHAMBER_TYPE (0=R, 1=Pet, 2=BT, 3=1.8 Atm, 4=100 Ci, 5=1000 Ci)
    char    sn[6];
    float   resp_corr;        //response correction
    float   nomvolts;         //nominal volts
    char    init_str[8];      // Capintec
    char    rev_num_str[6];       // Rev1.0
    char    feature_flag1;     // BIT 0: HV DIRECTLY TO ADC Chip, BIT 1: TWO GAIN STAGE, BIT 2: STORED GAIN FACTOR, Rev  1.0
    char    feature_flag2;     // 0, Rev 1.0
    double  gainfactor0;      // Gain Factor for Two Gain Stages, Rev 1.0
    double  gainfactor1;      // Gain Factor1 for Three Gain Stages, Rev 1.0
    double  gainfactor2;      // Gain Factor2 for Three Gain Stages, Rev 1.0
    double	future;			  // Low Gain Op amp offset correction, Rev 1.1
};

#define HV_ADC_MASK 1
#define ONE_GAIN_RELAY_MASK 2
#define STORED_GAIN_FACTOR 4

//isotope key definitions R,PET
typedef struct keyiso KEYISO;
struct keyiso
{
    char iso_name[8];
};

typedef struct keydef KEYDEF;
struct keydef
{
    KEYISO  keyiso[2];
};

//conntainer correction factors for calibrator
typedef struct calcorr CALCORR;
struct calcorr
{
        float corr[4];              //REF ,SYR, VIAL, CAP
        short nuc_index;            //nuclide index
};

//future dose data
typedef struct future FUTURE;
struct future
{
    time_t  dosetime;
    time_t  meastime;
    float   act0;
    char    actstr[12];
};

typedef struct nucdata_obsolete NUCDATA_OBSOLETE;
struct nucdata_obsolete
{
	char	show;			// Display Flag
    float   halflife;       //halflife of nuclide
    float   response[2];    //response of nuclide: R,PET
    float   user_response[2]; //user entered calnum of R, PET
    char    name[7];        //name of nuclide
    char	fullname[15];  // full name of nuclide
    char    code[7];        //key code
    short	hlunit;         //unit of halflife (M,H,D,Y)
    float	energy1;		// Primary peak keV
    float	energy2;		// Secondary peak keV
    float	energy3;		// Third peak keV
    float	wellefficiency;
    float	betaefficiency;
    float   userenergy1;
    float	userenergy2;
    float	userenergy3;
    float	userwellefficiency;
    float	userbetaefficiency;
    time_t	wellmeasuredon;
    time_t	betameasuredon;
};

// nuclide data
typedef struct nucdata NUCDATA;
struct nucdata
{
	char	show;			// Display Flag
    float   halflife;       //halflife of nuclide
    float   response[6];    //response of nuclide: R, PET, BT, 1.8, 100 Ci, 1000 Ci
    float   user_response[6]; //user entered calnum of R, PET, BT, 1.8, 100 Ci, 1000 Ci
    char    name[7];        //name of nuclide
    char	fullname[15];  // full name of nuclide
    char    code[7];        //key code
    short	hlunit;         //unit of halflife (M,H,D,Y)
    float	energy1;		// Primary peak keV
    float	energy2;		// Secondary peak keV
    float	energy3;		// Third peak keV
    float	wellefficiency;
    float	betaefficiency;
    float	probe700efficiency;
    float	well700efficiency;
    float	userenergy1;
    float	userenergy2;
    float	userenergy3;
    float	userwellefficiency;
    float	userbetaefficiency;
    float	userprobe700efficiency;
    float	userwell700efficiency;
    time_t	wellmeasuredon;
    time_t	betameasuredon;
    time_t	probe700measuredon;
    time_t	well700measuredon;
};

// chamber structure -- 1 for each chamber  -- these are in SRAM
 typedef struct chamber CHAMBER;
 struct chamber
 {
     bool   exists;             //chamber in system
     bool   active;             //chamber active -- needed only for PET
     short control;             //control by MAIN, Remote or PC
     bool   connected_flag;     //chamber connected
     short  adc_zero;           //zero of chamber's ADC
     float	bkg;			    //background
     float	zero;			    //zero value
     short nuc_index;          //nuclide index
     short  type;              //container type: REF,SYR,VIAL,CAP
     short  num_cal_corr;      //# of container correction factors
     CALCORR calcor;            //correction factor structure
     bool    calkey;		    //was Cal# key used?
     NUCDATA nucdata;
     char   calstrng[11];       //calibration number string
 };

 //remote structure -- 1 for each possible remote
 typedef struct remote REMOTE;
 struct remote
 {
     bool   exists;             //remote in system
     bool   keys_active;        //can keys be used?
     bool   valid_flag;         //data ready to be sent to remote
     short  nuclist;            //position on nuclide list
     short  acc_num;            //accuracy source number
     char   cmd;                //command received from remote and not yet used
     char   sub_cmd;            //sub command
     char   buffer[30];         //data to be sent to remote
     short  mode;               //remote's mode
 };


#define ADC_ZERO_BUF_SIZE 21
#define GAIN_CHANGE_COUNTDOWN_VALUE 12; // 5 Sec
 //measurement structure -- 1 for each chamber
 typedef struct measurement MEASUREMENT;
 struct measurement
 {
     float  actbuf[40];     // buffer for  moving average
     long  adc_value;      //value from adc
     short  polarity;       //polarity of value
     float  volts;         // chamber voltage reading
     float  meas;          // measurement value
     //float  act;            // calculated activity -- after unit factor applied
     float  act0;          // activity before unit factor applied
     float  lastmeas;       // last measurement for averaging
     float  response;      // nuclide's response
     float  resp0;          // response before response correction
     float  corfac;         //container correction factor
     bool   valid_flag;     // valid chamber measurement exists */
     bool   display_flag;   //data ready to be displayed
     short  conv_type;
     bool   over_flag;
     int	over_flag_count;	// Reset to 0 on a good measurement, increments to 2 max. This is used to deactivate future dose
     bool   wasover;        // over serviced
     bool   longavg;
     bool   doavg;
     bool   autodu;
     short  ntimes;
     short  isnum;
     short  gain;           /* current vale of gain , 0,1,2 */
     char   actstr[20];     /* formatted activity string */
     char   actstr0[20];    /* previous activity string */
     char   mode;           /* zero mode, test mode, measure mode */
     char   gain_change;     /* direction gain changed: -1,0,+1 */
     char   prevgain;       /* previous gain */
     char   changed;        /* readings after gain changed */
     char   dogain;         /* how gain should be changed */
     char   nmeas;          /* # of measurements */
     char   idec;           //current # of decimal places
     char   idecmin;        /* mininum # of decimal places allowed */
     char   kun;            //unit
     char   ndec;           //# dec places
     char   kun0;            //previous unit
     char   ndec0;           //previous # of dec places
     FUTURE future;
     bool   display_flag_2;   //data ready to be displayed after regular display
     unsigned long int ulTickAlarm; // When g_ulTickCounter reaches or exceeds this value then chamber should be read
     char gainbuf[40];
     unsigned long int ulMilliSecStamp;	// Time Stamp used in the Half-life Calculator, Set at the beginning of the test. Rollover in 1193 hr. Calculator has max of 100 hrs
     int gainchangecountdown;	// Set to GAIN_CHANGE_COUNTDOWN_VALUE when setchamber is called. Decremented to zero in
 };

#define READ_CHAMBERS_IN_MSEC 500;
// user calibration numbers
typedef struct usercal USERCAL;
struct usercal
{
    float   response;           //response
    short   ch_type;            //chamber type
    short   nuc_index;			//index into nuclide data
};

//
////constancy test automation
//typedef struct auto_constancy AUTO_CON;
//struct auto_constancy
//{
//    short   num;            //number of nuclides in test -- 0 = not automated
//    short   nuclides[8];    //indices of nuclides for automation
//};
//

//user keys
typedef struct user_keys USER_KEYS;
struct user_keys
{
    short keys[10];
};

typedef struct AutoCal AUTOCAL;
struct AutoCal{
	long long int	AutoCalID;		// Rowid for AutoCal table
	short			zeroopampoffset;// zero op amp offset (value to be added to the value on MCA card), Capintec.db.AutoCal.ZeroOffset
	short			gain1;			// gain1, Capintec.db.AutoCal.Gain1
    short			gain2;			// gain2, Capintec.db.AutoCal.Gain2
    float			sigma;			// sigma for Cs137 661.66 peak, Capintec.db.AutoCal.Sigma
    float			sigma_lip;		// sigma for Cs137 661.66 peak taken from the lip of a drilled probe, Capintec.db.AutoCal.Sigma_Lip
    float			linmeasured[5]; // 0=32.9(Cs137), 1=40.8(Eu152), 2=121.8(Eu152), 3=344.3(Eu152), 4=661.7(Cs137), Capintec.db.AutoCal.Linearity[0-4]
    short			detectortype;	// detector type
    time_t			caltstamp;		// Calibrated on, Capintec.db.AutoCal.CalStamp
    time_t			lintstamp;		// Linearity on, Capintec.db.AutoCal.LinStamp
    bool			linearityActive;
    float			linStd[5];
    float			linMeas[5];
    float			factorMtoS[6];
    float			factorStoM[6];
    char			serialnum[8];
    short			threshold;
    short			hv;
    int				num_of_channels;
};

 //detector structure -- will be in EEPROM
 typedef struct detector DETECTOR;
 struct detector
 {
     char   snum[6];        	// serial number, Capintec.db.Config.SettingName=WellSerialNum
     short  hv;             	// high voltage (value to be added to the value on MCA card), Capintec.db.Config.SettingName=WellHV
     short	threshold; 			// threshold (value to be added to the value on MCA card), Capintec.db.Config.SettingName=WellThreshold
     short	installed;			// 0=DET_EMPTY; 1=DET_WELL; 2=DET_BETA; 3=DET_PROBE700; 4=DET_WELL700; 5=DET_DRILLEDPROBE700
     int	num_of_channels;	// Valid values 256, 512, 1024, 2048, 4096
     int	auto_cal_threshold; // 0 = Empty
     short  initial_gain_1;
     short	initial_gain_2;
 };

 typedef struct detector_obsolete DETECTOR_OBSOLETE;
 struct detector_obsolete
 {
     char   snum[6];        // serial number
     short  hv;             // high voltage (value to be added to the value on MCA card)
     short	threshold; 		// threshold (value to be added to the value on MCA card)
     short  zeroopampoffset;// zero op amp offset (value to be added to the value on MCA card)
     short	gain1;			// gain1
     short	gain2;			// gain2
     float	sigma;			// sigma for Cs137 661.66 peak
     bool   calibrated;     // has it been calibrated?
     float  overall_cal;    // overall calibration factor
     float  noise_ratio[2];	// noise ratio, Cs137, Ba133 (Well only)
     float	linmeasured[5]; // 0=32.9(Cs137), 1=40.8(Eu152), 2=121.8(Eu152), 3=344.3(Eu152), 4=661.7(Cs137)
     short  detector;		// DET_EMPTY or DET_WELL or DET_BETA
     time_t caltstamp;		// Calibrated on
     time_t lintstamp;		// Linearity on
 };

//EEPROM
typedef struct eevals EEVALS;
struct eevals
{
    uchar		syst;           //Ci or Bq
    bool        syslock;        //is system locked to Ci or Bq?
    char        snum[6];        //SN for main unit, index = 0, 5, 6, 7, 8, 9
    STAND       stand[5];       //5 standard source structures
    char		constancysource; // 0 = None, 1-5 references stand[5]; 6 standge68, do not use stand.const_iso
    short		constancych[12]; //NuclideIDs for constancy channels
    STAND       stand_ge68;     //6th standard for Ge68
    NUCDATA_OBSOLETE obsolete8[10];   //10 user nuclide structures
    float       mo_tc_lim;        //Mo/Tc limit
    uchar        print[2];       //printer, ticket or line
    USERCAL     obsolete9[MAX_NEW_CAL_OBSOLETE];    //user calibration numbers
    CALCORR     cal_corr[MAX_CAL_COR];    //container factors
    USER_KEYS   user_keys[2];   //Chamber,Well
    uchar       mo_choice[4];   //MoAssay choices
    short       maxscr;         //max value for screen saver, in minutes (0 -- always on)
    char        contrast;       //screen contrast
    float       obsolete1[4];  //trigger levels for Well
    char		obsolete2[10];	// nuclides for Wipe -- Well
    char		obsolete3[10]; // nuclides for Unrestricted -- Well
    char        obsolete4[99]; //nuclides for sealed sources -- Well
    WELLCFDATA  obsolete5[60]; //conversion factor data for Well
    float		obsolete10;//float       gencf;          //generic cf for Well
    DETECTOR_OBSOLETE	obsolete6;       //detector data for Well
    LINDEF      lindef;         //linearity test definition
    KEYDEF      remote_nucs[8]; //definition of remote nuclides for R,PET
    uchar		volume;			//volume setting
    uchar		brightness;		// brightness setting
    uchar		sleepbrightness;// brightness setting for sleep
    uchar		sleeptimeout;	// sleep timeout in minutes
    short		nuclideID[8];	// nuclideID
    char        init_str[10];   //initialization string
    char        snum1[4];       //SN for main unit, index = 1, 2, 3, 4
    char        init_str1[8];   //initialization string - Mirion
};

#define MAX_INVENTORY  30
// inventory structure
typedef struct inventory INVENTORY;
struct inventory{
	float   act;		/* activity */
	float   vol;		/* volume in ml */
	float   mopertc;	/* Mo/Tc ratio */
	short   nucnum;		/* index into nuclides */
	short   type;           /* index into study types ,
                                    -1 = NONE */
	char    id;             /* ID - 1 to 99 -- 0 =NONE */
	char    lot[16];        /* lot number */
	time_t  date;           //date/time for activity & volume
};
//screen memory
typedef struct scrmem SCRMEM;
struct scrmem
{
    uchar  byte[1024];
};

typedef struct cmdsent CMDSENT;
struct cmdsent
{
	char cCommand[258];
	void *ptrOutput;
	void (*ptrFunction) (void *ptrInput);
	unsigned long int ulCreateStamp;
	unsigned long int ulResendStamp;
	unsigned long int ulExpireStamp;
};

typedef struct sdchamber SDCHAMBER;
struct sdchamber{
	short adc_zero;
	float bkg;
	float zero;
	ushort checksum;
};

typedef struct syssetting SYSSETTING;
struct syssetting{
	SDCHAMBER sdchamb[8];
};

typedef struct energypeak ENERGYPEAK;
struct energypeak{
	float energy;
	short peakindex;
	short startCh;
	short endCh;
	bool found;
};

typedef struct specpeak_copy SPECPEAK_COPY;
struct specpeak_copy{
	float energy;
	float cpm;
	short nuclideID;
	float activity;
	short type;
	bool exceedthreshold;
};

typedef struct etherrxdesc ETHERRXDESC;
struct etherrxdesc{
	ushort flag;
	ushort length;
	uchar *buffer;
};

typedef struct ethertxdesc ETHERTXDESC;
struct ethertxdesc{
	ushort flag;
	ushort length;
	uchar *buffer;
};

typedef struct wellwipetype WELLWIPETYPE;
struct wellwipetype{
	long long int WellWipeTypeID;
	long long int WellWipeTypeGroupID;
	char Name[31];
	float Threshold;
	int CountTime;
	double CreatedOnValue;
	time_t CreatedOn;
	time_t LastUpdated;
	int NuclideID[10];
};

typedef struct wellwipelocation WELLWIPELOCATION;
struct wellwipelocation{
	long long int WellWipeLocationID;
	long long int WellWipeLocationGroupID;
	char Name[31];
	long long int WellWipeTypeID;
	long long int WellWipeTypeGroupID;
	char WellWipeTypeName[31];
	float Threshold;
	int CountTime;
	double CreatedOnValue;
	time_t CreatedOn;
	time_t LastUpdated;
	int NuclideID[10];
};

typedef struct wellwipenuclide WELLWIPENUCLIDE;
struct wellwipenuclide{
	long long int WellWipeNuclideID;
	long long int WellWipeNuclideGroupID;
	int NuclideID;
	char Name[31];
	float PrimaryEnergy;
	float SecondaryEnergy;
	float TertiaryEnergy;
	float Efficiency;
	double CreatedOnValue;
	time_t CreatedOn;
	time_t LastUpdated;
};

typedef struct wellwipepeak WELLWIPEPEAK;
struct wellwipepeak{
	long long int WellWipePeakID;
	long long int WellWipeID;
	float Energy;
	float StartROI;
	float EndROI;
	int ROICounts;
	float ROICPM;
	int BackgroundROICounts;
	float BackgroundROICPM;
	float NetROICPM;
	long long int WellWipeNuclideID;
	float Activity;
	bool ExceedsThreshold;
	int PeakType;
};

typedef struct db_spec DB_SPEC;
struct db_spec{
	long long int SpectraID;
	ulong TotalCounts;
	float LiveTime;
	float RealTime;
	float TotalCPM;
	time_t MeasuredOn;
	AUTOCAL AutoCal;
	int NumOfChannels;
	int BitsPerChannel;
	ulong DecompressedSpectra[4096];
};

typedef struct backgnd BACKGND;
struct backgnd{
	long long int WellBackgroundID;
	short Detector;
	DB_SPEC Spectrum;
	time_t CreatedOn;
	bool Inactive;
};

typedef struct wellwipe WELLWIPE;
struct wellwipe{
	long long int WellWipeID;
	WELLWIPELOCATION WipeLocation;
	float NetCPM;
	float Efficiency;
	float Activity;
	bool ExceedsThreshold;
	bool OverallHighActivity;
	DB_SPEC Spectrum;
	BACKGND Background;
	char Comment[26];
	char InactiveReason[26];
	WELLWIPENUCLIDE WellWipeNuclide[10];
	WELLWIPEPEAK Peaks[40];
	time_t CreatedOn;
	bool Inactive;
};

typedef struct wellwipesearch WELLWIPESEARCH;
struct wellwipesearch{
	long long int WellWipeID;
	char WellWipeLocationName[31];
	char WellWipeTypeName[31];
	bool OverallHighActivity;
	time_t MeasuredOn;
	bool Inactive;
};

typedef struct wellsystemtest WELLSYSTEMTEST;
struct wellsystemtest{
	long long int WellSystemTestID;
	int DetectorTest; // 1=WELL, 2=BETA, 3=PROBE700, 4=WELL700, 6=DRILLEDPROBE700_AS_WELL, 7=DRILLEDPROBE700_AS_PROBE
	char NuclideName[10];
	char SerialNumber[20];
	float CalibrationActivity;
	time_t CalibrationDate;
	float Energy;
	float StartROI;
	float EndROI;
	float Efficiency;
	float Halflife;
	int HalflifeUnit;
	float PredictedActivity;
	int ROICounts;
	float ROICPM;
	int BackgroundROICounts;
	float BackgroundROICPM;
	float NetROICPM;
	float Activity;
	float Deviation;
	DB_SPEC Spectrum;
	long long int WellBackgroundID;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct wellsystemtestsearch WELLSYSTEMTESTSEARCH;
struct wellsystemtestsearch{
	long long int WellSystemTestID;
	char SerialNumber[20];
	float Deviation;
	int DetectorTest;
	time_t MeasuredOn;
	bool Inactive;
};

typedef struct wellmdatest WELLMDATEST;
struct wellmdatest{
	long long int WellMDATestID;
	int DetectorTest; // 1=WELL, 2=BETA, 3=PROBE700, 4=WELL700, 6=DRILLEDPROBE700_AS_WELL, 7=DRILLEDPROBE700_AS_PROBE
	char NuclideName[10];
	float Energy;
	float StartROI;
	float EndROI;
	float Efficiency;
	float PrecisionFactor;
	float CorrectionFactor;
	int ROICounts;
	float MDA;
	DB_SPEC Spectrum;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct wellmdasearch WELLMDASEARCH;
struct wellmdasearch{
	long long int WellMDATestID;
	char NuclideName[10];
	float MDA;
	int DetectorType;
	time_t MeasuredOn;
	bool Inactive;
};

typedef struct wellchisquaresample WELLCHISQUARESAMPLE;
struct wellchisquaresample{
	long long int WellChiSquareSampleID;
	long int ROICounts;
	DB_SPEC Spectrum;
};

typedef struct wellchitest WELLCHITEST;
struct wellchitest{
	long long int WellChiSquareTestID;
	int DetectorTest; // 1=WELL, 2=BETA, 3=PROBE700, 4=WELL700, 6=DRILLEDPROBE700_AS_WELL, 7=DRILLEDPROBE700_AS_PROBE
	int SampleTime;
	int NumberOfSamples;
	char NuclideName[10];
	float Energy;
	float StartROI;
	float EndROI;
	float ChiSquare;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
	WELLCHISQUARESAMPLE WellChiSquareSample[20];
};

typedef struct wellchisearch WELLCHISEARCH;
struct wellchisearch{
	long long int WellChiSquareTestID;
	char NuclideName[10];
	float ChiSquare;
	int DetectorTest;
	time_t MeasuredOn;
	bool Inactive;
};

typedef struct testident TESTIDENT;
struct testident{
	long long int TestIdentID;
	long long int TestIdentGroupID;
	char TestID[26];
	char PatientID[26];
	char FirstName[26];
	char LastName[26];
	//time_t DateOfBirth;
	double DateOfBirth;
	char Sex[2];
	char Physician[26];
	char TechID[26];
	time_t CreatedOn;
	time_t LastUpdated;
	bool Inactive;
};

typedef struct wellschillingtest WELLSCHILLINGTEST;
struct wellschillingtest{
	long long int WellSchillingTestID;
	float UrineVolume;
	float AliquotVolume;
	float DilutionFactor;
	float StartROI;
	float EndROI;
	int StandardROICounts;
	float StandardROICPM;
	int AliquotROICounts;
	float AliquotROICPM;
	int BackgroundROICounts;
	float BackgroundROICPM;
	float Excretion;
	DB_SPEC SchillingStandard_Spectrum;
	DB_SPEC SchillingAliquot_Spectrum;
	BACKGND Background;
	TESTIDENT TestIdent;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct wellschillingsearch WELLSCHILLINGSEARCH;
struct wellschillingsearch{
	long long int WellSchillingTestID;
	char LastName[26];
	char FirstName[26];
	time_t MeasuredOn;
	bool Inactive;
};

typedef struct wellplasmatest WELLPLASMATEST;
struct wellplasmatest{
	long long int WellPlasmaTestID;
	float DilutionFactor;
	float SampleVolume;
	float Hematocrit;
	float PatientWeight;
	float StartROI;
	float EndROI;
	int StandardROICounts;
	float StandardROICPM;
	int WholeBloodROICounts;
	float WholeBloodROICPM;
	int PlasmaROICounts;
	float PlasmaROICPM;
	int BackgroundROICounts;
	float BackgroundROICPM;
	float WholeBloodVolume;
	float WholeBloodPerKg;
	float PlasmaVolume;
	float PlasmaPerKg;
	float RBCVolume;
	float RBCPerKg;
	float CalculatedHematocrit;
	DB_SPEC Standard_Spectrum;
	DB_SPEC WholeBlood_Spectrum;
	DB_SPEC Plasma_Spectrum;
	BACKGND Background;
	TESTIDENT TestIdent;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct wellplasmasearch WELLPLASMASEARCH;
struct wellplasmasearch{
	long long int WellPlasmaTestID;
	char LastName[26];
	char FirstName[26];
	time_t MeasuredOn;
	bool Inactive;
};

typedef struct wellrbctest WELLRBCTEST;
struct wellrbctest{
	long long int WellRBCTestID;
	float DoseHematocrit;
	float PatientHematocrit;
	float PatientWeight;
	float StartROI;
	float EndROI;
	int WholeBloodStandardROICounts;
	float WholeBloodStandardROICPM;
	int PlasmaStandardROICounts;
	float PlasmaStandardROICPM;
	int WholeBloodSampleROICounts;
	float WholeBloodSampleROICPM;
	int PlasmaSampleROICounts;
	float PlasmaSampleROICPM;
	int BackgroundROICounts;
	float BackgroundROICPM;
	float RBCVolume;
	float RBCPerKg;
	float WholeBloodVolume;
	float WholeBloodPerKg;
	float PlasmaVolume;
	float PlasmaPerKg;
	DB_SPEC WholeBloodStandard_Spectrum;
	DB_SPEC PlasmaStandard_Spectrum;
	DB_SPEC WholeBloodSample_Spectrum;
	DB_SPEC PlasmaSample_Spectrum;
	BACKGND Background;
	TESTIDENT TestIdent;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct wellrbcsearch WELLRBCSEARCH;
struct wellrbcsearch{
	long long int WellRBCTestID;
	char LastName[26];
	char FirstName[26];
	time_t MeasuredOn;
	bool Inactive;
};

typedef struct wellautocalsearch WELLAUTOCALSEARCH;
struct wellautocalsearch{
	long long int AutoCalID;
	int Gain1;
	int Gain2;
	float Sigma;
	int DetectorType;
	time_t MeasuredOn;
	bool Inactive;
};

typedef struct user USER;
struct user{
	int UserID;
	char UserName[15];
	char Password[15];
	char FirstName[15];
	char LastName[15];
	int Role;
	bool Inactive;
};

typedef struct probebioassayefficiency PROBEBIOASSAYEFFICIENCY;
struct probebioassayefficiency{
	long long int ProbeBioAssayEfficiencyID;
	float I131Efficiency;
	float I125Efficiency;
	float I123Efficiency;
	float I131I125Contamination;
	float I123I125Contamination;
	bool I131Measured;
	float I131Activity;
	time_t I131Date;
	float I131PrimaryEnergy;
	float I131StartEV;
	float I131EndEV;
	float I131I125StartEV;
	float I131I125EndEV;
	long long int I131BackgroundID;
	long long int I131SpectraID;
	DB_SPEC I131Spectrum;
	int I131ROICounts;
	float I131ROICPM;
	int I131BackgroundROICounts;
	float I131BackgroundROICPM;
	float I131NetROICPM;
	int I131I125ROICounts;
	float I131I125ROICPM;
	int I131I125BackgroundROICounts;
	float I131I125BackgroundROICPM;
	float I131I125NetROICPM;
	bool I125Measured;
	float I125Activity;
	time_t I125Date;
	float I125PrimaryEnergy;
	float I125StartEV;
	float I125EndEV;
	long long int I125BackgroundID;
	long long int I125SpectraID;
	DB_SPEC I125Spectrum;
	int I125ROICounts;
	float I125ROICPM;
	int I125BackgroundROICounts;
	float I125BackgroundROICPM;
	float I125NetROICPM;
	bool I123Measured;
	float I123Activity;
	time_t I123Date;
	float I123PrimaryEnergy;
	float I123StartEV;
	float I123EndEV;
	float I123I125StartEV;
	float I123I125EndEV;
	long long int I123BackgroundID;
	long long int I123SpectraID;
	DB_SPEC I123Spectrum;
	int I123ROICounts;
	float I123ROICPM;
	int I123BackgroundROICounts;
	float I123BackgroundROICPM;
	float I123NetROICPM;
	int I123I125ROICounts;
	float I123I125ROICPM;
	int I123I125BackgroundROICounts;
	float I123I125BackgroundROICPM;
	float I123I125NetROICPM;
	time_t CreatedOn;
	bool Inactive;
};

typedef struct probebioassaysetting PROBEBIOASSAYSETTING;
struct probebioassaysetting{
	long long int ProbeBioAssaySettingID;
	bool I131Active;
	bool I125Active;
	bool I123Active;
	float I131Threshold;
	float I125Threshold;
	float I123Threshold;
	int CountTime;
	int ProbeDistance;
	time_t CreatedOn;
	bool Inactive;
};

typedef struct probebioassaytest PROBEBIOASSAYTEST;
struct probebioassaytest{
	long long int ProbeBioAssayTestID;
	char UserName[27];
	char FirstName[27];
	char LastName[27];
	int ProbeDistance;
	bool I131Active;
	float I131PrimaryEnergy;
	float I131StartEV;
	float I131EndEV;
	int I131ROICounts;
	float I131ROICPM;
	int I131BackgroundROICounts;
	float I131BackgroundROICPM;
	float I131NetROICPM;
	float I131Activity;
	bool I131High;
	float I131I125NetROICPM;
	bool I125Active;
	float I125PrimaryEnergy;
	float I125StartEV;
	float I125EndEV;
	int I125ROICounts;
	float I125ROICPM;
	int I125BackgroundROICounts;
	float I125BackgroundROICPM;
	float I125NetROICPM;
	float I125NetROICPMCorrected;
	float I125Activity;
	bool I125High;
	bool I123Active;
	float I123PrimaryEnergy;
	float I123StartEV;
	float I123EndEV;
	int I123ROICounts;
	float I123ROICPM;
	int I123BackgroundROICounts;
	float I123BackgroundROICPM;
	float I123NetROICPM;
	float I123Activity;
	bool I123High;
	float I123I125NetROICPM;
	long long int WellBackgroundID;
	long long int SpectraID;
	DB_SPEC Spectrum;
	long long int ProbeBioAssayEfficiencyID;
	long long int ProbeBioAssaySettingID;
	time_t CreatedOn;
	char Comment[27];
	char InactiveReason[27];
	bool Inactive;
};

typedef struct wellbioassaysearch WELLBIOASSAYSEARCH;
struct wellbioassaysearch{
	long long int ProbeBioAssayTestID;
	bool I131Active;
	float I131NetROICPM;
	float I131Activity;
	bool I131High;
	bool I125Active;
	float I125NetROICPMCorrected;
	float I125Activity;
	bool I125High;
	bool I123Active;
	float I123NetROICPM;
	float I123Activity;
	bool I123High;
	time_t CreatedOn;
	bool Inactive;
};

typedef struct probethyroiduptakeprotocol PROBETHYROIDUPTAKEPROTOCOL;
struct probethyroiduptakeprotocol{
	long long int ProbeTUProtocolID;
	long long int ProbeTUProtocolGroupID;
	char ProtocolName[26];
	int NuclideID;
	int DoseForm;
	int DoseMeasurementMethod;
	bool DoseDecayCorrect;
	bool PreDoseMeasurement;
	bool ResidualMeasurement;
	float I123LowerEV;
	float I123UpperEV;
	float I131LowerEV;
	float I131UpperEV;
	float Tc99mLowerEV;
	float Tc99mUpperEV;
	int DefaultCountTime;
	int DefaultDistance;
	time_t CreatedOn;
	time_t LastUpdated;
	bool Inactive;
};

typedef struct probethyroiduptakeprotocolactivename PROBETHYROIDUPTAKEPROTOCOLACTIVENAME;
struct probethyroiduptakeprotocolactivename{
	long long int ProbeTUProtocolID;
	char ProtocolName[26];
};

typedef struct probethyroiduptakesearch PROBETHYROIDUPTAKESEARCH;
struct probethyroiduptakesearch{
	long long int ProbeTUTestID;
	char PatientID[11];
	char FirstName[18];
	char LastName[18];
	char NuclideName[6];
	time_t DoseAdministeredOn;
	bool Inactive;
};

typedef struct probethyroiduptaketest PROBETHYROIDUPTAKETEST;
struct probethyroiduptaketest{
	long long int ProbeTUTestID;
	int ProbeTUTestPhase;
	char PatientID[11];
	char FirstName[18];
	char LastName[18];
	double DateOfBirth;
	char Sex[2];
	char Physician[18];
	char TechID[18];
	char NuclideName[6];
	float HalfLife;
	int HalfLifeUnit;
	float StartROI;
	float EndROI;
	float PeakEV;
	int CountingTime;
	float DoseMultiplier;
	time_t DoseAdministeredOn;
	float CalibrationActivity;
	time_t CalibrationDate;
	char LotNum[11];
	int ProbeDistance;
	char AccessionNumber[11];
	long long int ProbeTUProtocolID;
	time_t CreatedOn;
	time_t LastUpdated;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct probethyroiduptakepatmeas PROBETHYROIDUPTAKEPATMEAS;
struct probethyroiduptakepatmeas{
	long long int ProbeTUPatientMeasurementID;
	int MeasurementNumber;
	int Neck1ROICounts;
	float Neck1ROICPM;
	int Neck2ROICounts;
	float Neck2ROICPM;
	float NeckROIAverageCPM;
	int Leg1ROICounts;
	float Leg1ROICPM;
	int Leg2ROICounts;
	float Leg2ROICPM;
	float LegROIAverageCPM;
	float PatientROINetCPM;
	float AdjustedPatientROINetCPM;
	float DoseROINetCPM;
	float AdjustedDoseROINetCPM;
	float Uptake;
	float ElapsedTime;
	char comment[26];
	long long int Neck1_SpectraID;
	long long int Neck2_SpectraID;
	long long int Leg1_SpectraID;
	long long int Leg2_SpectraID;
	long long int ProbeTUTestID;
	time_t CreatedOn;
	bool Inactive;

	DB_SPEC Neck1_Spectrum;
	DB_SPEC Neck2_Spectrum;
	DB_SPEC Leg1_Spectrum;
	DB_SPEC Leg2_Spectrum;
};

typedef struct probethyroiduptakedosesinglemeas PROBETHYROIDUPTAKEDOSESINGLEMEAS;
struct probethyroiduptakedosesinglemeas{
	long long int ProbeTUSingleDoseMeasurementID;
	int DoseSubUnit; // 1=First Capsule, 2=Second Capsule ...
	int Dose1ROICounts;
	float Dose1ROICPM;
	int Dose2ROICounts;
	float Dose2ROICPM;
	float DoseROIAverageCPM;
	long long int Dose1_SpectraID;
	long long int Dose2_SpectraID;
	long long int ProbeTUTotalDoseMeasurementID;
	time_t CreatedOn;
	DB_SPEC Dose1_Spectrum;
	DB_SPEC Dose2_Spectrum;
};

typedef struct probethyroiduptakedosetotalmeas PROBETHYROIDUPTAKEDOSETOTALMEAS;
struct probethyroiduptakedosetotalmeas{
	long long int ProbeTUTotalDoseMeasurementID;
	int MeasurementNumber; // -1 = Residual, 0 = Administered Dose, 1 = First Measurement after Administration ...
	int BackgroundROICounts;
	float BackgroundROICPM;
	float DoseROICPM;
	float DoseROINetCPM;
	time_t StartDateTime;
	time_t EndDateTime;
	long long int WellBackgroundID;
	long long int ProbeTUTestID;
	time_t CreatedOn;
	bool Inactive;
	PROBETHYROIDUPTAKEDOSESINGLEMEAS SingleMeasurement[5];
};

typedef struct probethyroiduptakenormal PROBETHYROIDUPTAKENORMAL;
struct probethyroiduptakenormal{
	int hour;
	int min;
	int max;
};

typedef struct wellrbcsurvivalsearch WELLRBCSURVIVALSEARCH;
struct wellrbcsurvivalsearch{
	long long int WellRBCSurvivalTestID;
	int WellRBCSurvivalTestPhase;
	char PatientID[11];
	char FirstName[18];
	char LastName[18];
	time_t InjectedOn;
	bool Inactive;
};

typedef struct wellrbcsurvivaltest WELLRBCSURVIVALTEST;
struct wellrbcsurvivaltest{
	long long int WellRBCSurvivalTestID;
	int WellRBCSurvivalTestPhase;
	char PatientID[11];
	char FirstName[18];
	char LastName[18];
	double DateOfBirth;
	char Sex[2];
	char Physician[18];
	char TechID[18];
	char NuclideName[6];
	float HalfLife;
	int HalfLifeUnit;
	float StartROI;
	float EndROI;
	float PeakEV;
	int CountingTime;
	time_t InjectionDate;
	float CalibrationActivity;
	time_t CalibrationDate;
	char LotNum[11];
	char AccessionNumber[11];
	time_t CreatedOn;
	time_t LastUpdated;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct wellrbcsurvivalmeas WELLRBCSURVIVALMEAS;
struct wellrbcsurvivalmeas{
	long long int WellRBCSurvivalMeasurementID;
	int MeasurementNumber;
	int BackgroundROICounts;
	float BackgroundROICPM;
	int Sample1ROICounts;
	float Sample1ROICPM;
	int Sample2ROICounts;
	float Sample2ROICPM;
	float SampleROIAverageCPM;
	float SampleROINetCPM;
	float DecayCorrectedSampleROINetCPM;
	float SampleHematocrit;
	float Remaining;
	float ElapsedDays;
	char comment[26];
	long long int WellBackgroundID;
	long long int Sample1_SpectraID;
	long long int Sample2_SpectraID;
	long long int WellRBCSurvivalTestID;
	time_t CreatedOn;
	DB_SPEC Sample1_Spectrum;
	DB_SPEC Sample2_Spectrum;
};

typedef struct wellrbcsurvivalresult WELLRBCSURVIVALRESULT;
struct wellrbcsurvivalresult{
	long long int WellRBCSurvivalResultID;
	long long int WellRBCSurvivalTestID;
	float HalfDays;
	float Y0;
	float X10;
	float Y60;
	time_t CreatedOn;
};

typedef struct autolinearitymeasurement AUTOLINEARITYMEASUREMENT;
struct autolinearitymeasurement{
	long long int AutoLinearityMeasurementID;	// >=0 = Saved, -1 = Blank, -2 = MeasuredOn & ElapsedMinutes Prefilled, -3 = Processed
	float MeasuredActivity;
	time_t MeasuredOn;
	int Status;				// 0 = Normal, 1 = Overflow, 2 = Reading After Pause, 3 = Reading After Power Down,  -1 = Paused Skipped, -2 = Power Down Skipped
	int ElapsedMinutes;
	float PredictedActivity;
	float PercentVariation;
};

typedef struct autolinaritytest AUTOLINEARITYTEST;
struct autolinaritytest{
	long long int AutoLinearityTestID; // >=0 = Saved, -1 = Ready, -2 = Running, -3 = Paused, -4 = Recovery
	char SerialNumber[20];
	int Type;
	char NuclideName[10];
	float HalfLife;
	int HalfLifeUnit;
	int IntervalMinutes;
	int TotalMinutes;
	time_t StartedOn;
	char Comment[27];
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
	AUTOLINEARITYMEASUREMENT AutoLinearityMeasurement[1210];
};

typedef struct autolinearitysearch AUTOLINEARITYSEARCH;
struct autolinearitysearch{
	long long int AutoLinearityTestID;
	char SerialNumber[20];
	char NuclideName[10];
	time_t StartedOn;
	bool Inactive;
};

typedef struct langmap LANG_MAP;
struct langmap{
	unsigned long int off[2];
};

typedef struct chamberdailytest CHAMBERDAILYTEST;
struct chamberdailytest{
	long long int ChamberDailyTestID;
	char ChamberSerialNumber[11];
	int ChamberType;
	bool TwoStageChamber;
	char DataCheckTextEnglish[40];
	char DataCheckTextFrench[40];
	bool DataCheckFailed;
	char DataCheckCRC[10];
	long long int ChamberZeroID;
	long long int ChamberBackgroundID;
	long long int ChamberVoltageID;
	long long int ChamberAccuracyTestID;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct chamberzero CHAMBERZERO;
struct chamberzero{
	long long int ChamberZeroID;
	char ChamberSerialNumber[11];
	int ChamberType;
	bool TwoStageChamber;
	int ZeroStatus;
	char ZeroTextEnglish[40];
	char ZeroTextFrench[40];
	float ZeroValue;
	time_t MeasuredOn;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct chamberbackground CHAMBERBACKGROUND;
struct chamberbackground{
	long long int ChamberBackgroundID;
	char ChamberSerialNumber[11];
	int ChamberType;
	bool TwoStageChamber;
	int BackgroundStatus;
	char BackgroundTextEnglish[40];
	char BackgroundTextFrench[40];
	float BackgroundValue;
	time_t MeasuredOn;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct chambervoltage CHAMBERVOLTAGE;
struct chambervoltage{
	long long int ChamberVoltageID;
	char ChamberSerialNumber[11];
	int ChamberType;
	bool TwoStageChamber;
	float NominalVoltage;
	float MinVoltage;
	float MaxVoltage;
	int VoltageStatus;
	char VoltageTextEnglish[40];
	char VoltageTextFrench[40];
	float VoltageValue;
	time_t MeasuredOn;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct chamberaccuracytest CHAMBERACCURACYTEST;
struct chamberaccuracytest{
	long long int ChamberAccuracyTestID;
	char ChamberSerialNumber[11];
	int ChamberType;
	bool TwoStageChamber;
	char ConstancyNuclide[10];
	char ConstancyNuclideSerialNumber[11];
	float ConstancyVoltage;
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
};

typedef struct chamberaccuracymeasurement CHAMBERACCURACYMEASUREMENT;
struct chamberaccuracymeasurement{
	long long int ChamberAccuracyMeasurementID;
	long long int ChamberAccuracyTestID;
	char SourceNuclide[10];
	int SourceNuclideIndex;
	char SourceSerialNumber[11];
	float SourceCalActivity;
	time_t SourceCalDate;
	time_t MeasuredOn;
	char CalculatedActivityTextEnglish[40];
	char CalculatedActivityTextFrench[40];
	float CalculatedActivity;
	float Response;
	float HalfLife;
	int HalfLifeUnit;
	char MeasuredActivityTextEnglish[40];
	char MeasuredActivityTextFrench[40];
	float MeasuredActivity;
	int DeviationStatus;
	char DeviationTextEnglish[40];
	char DeviationTextFrench[40];
	float Deviation;
};

typedef struct chamberautoconstancy CHAMBERAUTOCONSTANCY;
struct chamberautoconstancy{
	long long int ChamberAutoConstancyID;
	long long int ChamberAccuracyTestID;
	char ConstancyNuclide[10];
	int ConstancyNuclideIndex;
	float Response;
	float HalfLife;
	int HalfLifeUnit;
	char ConstancyActivityTextEnglish[40];
	char ConstancyActivityTextFrench[40];
	float ConstancyActivity;
};

typedef struct chambersearch CHAMBERSEARCH;
struct chambersearch{
	long long int ChamberDailyTestID;
	long long int ChamberZeroID;
	long long int ChamberBackgroundID;
	long long int ChamberVoltageID;
	long long int ChamberAccuracyTestID;
	char SerialNumber[20];
	char Results[51];
	time_t StartedOn;
	bool Inactive;
};

typedef struct halflifecalcmeasurement HALFLIFECALCMEASUREMENT;
struct halflifecalcmeasurement{
	long long int HalflifeCalcMeasurementID;	// >=0 = Saved, -1 = Blank, -2 = TargetElapsedSecond Filled, -3 = Processed
	int TargetElapsedSecond;
	float Voltage;
	float CalcLnVoltage;
	float CalcActivity;
	int Status;									// -1 = Unmeasured, 0 = Normal, 1 = Overflow, 2 = Underflow
	unsigned long int MilliSecStamp;
	int ElapsedSecond;
	int ElapsedMilliSecond;
	float PredictedVoltage;
	float PredictedVariation;
	float CalcDeltaLnVoltage;
	float CalcDeltaSec;
	float CalcProductOfDeltaLnVoltageAndDeltaSec;
	float CalcDeltaSecSquared;
};

typedef struct halflifecalc HALFLIFECALC;
struct halflifecalc{
	long long int HalflifeCalcID;				// >=0 = Saved, -1 = Ready, -2 = Running, -3 = Stopped
	int ChamberNumber;
	char SerialNumber[20];
	int ChamberType;
	char NuclideName[10];
	char LotNum[25];
	int IntervalSeconds;
	int TotalSeconds;
	time_t StartedOn;
	char Comment[27];
	time_t CreatedOn;
	char InactiveReason[26];
	bool Inactive;
	int MeasurementCount;
	int MeasurementTotal;
	HALFLIFECALCMEASUREMENT HalflifeCalcMeasurement[1210];
	int InitialGain;
	bool CheckInitialMeasurement;
	int InitialMeasurementStatus;			// 0 = OK, 1 = Initial Activity Too Low
	float CalcValidMeasurementCount;
	float CalcLnVoltageAvg;
	float CalcSecAvg;
	float CalcSumOfProductOfDeltaLnVoltageAndDeltaSec;
	float CalcSumOfDeltaSecSquared;
	int CalcHalflifeStatus;					// 0 = OK, 1 = Less than 5 measurements, 2 = Halflife too short, 3 = Halflife too long, 4 = Halflife negative
	float CalcHalflife;
	int CalcHalflifeUnit;
};

void dateout(char *datestr, time_t *timet, short year_flag);
void dateout_language(char *datestr, time_t *timet, short year_flag);
void dateout_julian(char *datestr, double julian);
void dateout_julian_language(char *datestr, double julian);
void timeout(char *timestr, time_t *timet);
void timeoutsec(char *timestr, time_t *timet);
void outtime(char *timestr, struct tm *time);
void outdate(char *datestr, struct tm *date, short year_flag);
void daytostr_language(char *datestr, time_t *timet);
void stringout(char *output, time_t *timet);
void beep(void);
void beep_amulet(void);
void click_amulet(void);
void format_activity(float act,short syst,char *actstr);
short format_activity_system(float actci, char *actstr);
short format_activity_system2(float actci, char *actstr);
short format_activity_system_kbq(float actci, char *actstr);
void replace(char *inputoutput, char searchchar, char replacechar);
void act2str(float activ, char *actstr, short ndec, bool zflag, short kun);
void act2strnew(float activ, char *actstr, short ndec, bool zflag, short kun);
short getdec(float act, short syst, short *kunit);
short getdec_kbq(float act, short syst, short *kunit);
short main_menu(void);
void util_convert(void);
void val_unit_msg(void);
short setup(void);
void set_date_time(void);
void test_source_menu(void);
void facmenu(void);
void calnum_but(void);
float get_response(char calstrng[], short ch_type);
void getiso(short ch_num, short but);
bool getuser(short ch_num, short but);
short user_key_index(short but);
bool get_new_cal(float *response, short nuc_index,short ch_type);
void mindec(short ch_num);
void nucset(short ch_num);
void nucinit(short ch, bool mode);
//void sysbut(void);
void nucbut(void);
void specify_nuclide_msg(void);
void specify_further_msg(void);
void no_nuclide_msg(void);
void no_calnum_msg(void);
short get_nuclide_index(bool flag);
void disp_nuclide(void);
void disp_cal(float response);
void setup_user_keys(void);
void signon(void);
void tests(void);
short get_crc_mode(void);
void measurement_screen(void);
void add_nuclide(void);
void delete_nuclide(void);
void choose_nuclide_msg(short flag, short msg_num);
void cal_from_response(char *strng,float response);
void DisplayNucCal_getCalNumFromResponse(float response, short chambertype, char *calnum);
void display_cal(char *strng, short xpos0, short ypos, short size, short plane);
void change_cal_num(void);
void print_diagnostic_data(void);
float nucdecay(float act0,time_t time0,time_t time1,float hl,short unit);
void container_correction_factors(void);
void input_nuclide(short *nuc_index);
void change_cal_type(void);
bool select_chamber(void);
void display_chamber(void);
bool display_time_set(void);
void clear_display_time(void);
void set_display_time(void);
void reset_minute_counter(void);
void reset_minute_counter_with_seconds(void);
void set_cal_type(short ch_num, short type);
void get_num_cal_corr(short ch_num, short nuc_index);
void overflow(short ch);
void servicechamber(short ch);
bool getchambervolts(short ch);
short errchamb(short ch_num);
void setchamber(short ch_num, short gain_num);
void activity_to_screen(void);
void display_activity(short ch_num);
void get_measurements(void);
float get_calnum(bool zero_flag,short ch_type);
void daily(void);
char checksum(char *str, short len);
void plwait(bool flag);
void seeman(void);
short accuracy_test(bool daily);
bool get_meas_screen(void);
void set_meas_screen(bool flag);
void generate_crc_table(void);
ushort Crc16(uchar ch0,ushort crc);
bool same_day(time_t *time0, time_t *time1);
//void util_decay(void);
short get_nuclide(short flag, short msg_num);
void dose_table(void);
void sources_key(void);
short well_activity(void);
short beta_activity(void);
void set_crc_mode(short mode);
bool password(void);
short counter_main_menu(void);
short screen_control_menu(void);
void display_act_small(char *actstr, short col, short line);
void measure_activity(char *nucname);
void disp_cal_type(void);
bool set_nuclide_data(short nuc_index, short ch_num);
void downbut(void);
void upbut(void);
void lowresp(short ch_num);
void doauto(short ch_num);
void respcor(short ch_num,float *response);
void ismeas(short ch_num);
void stringtofixed(char *output, const char *input, short length);
time_t mk_time(struct tm *tmt);
void delayloop(unsigned long int csec);
void memorycopy(uchar *destination, uchar *source, ulong size);

void strin(char *result, char *input);
void get_amulet_strin(short msg_num, char *result);
extern volatile unsigned long int g_csec_tstamp;
extern volatile bool lowBattery1;
extern volatile bool lowBattery2;
extern volatile bool lowBattery3;

#define WELL_TIME_GENERAL		0
#define WELL_TIME_BACKGROUND	1
#define WELL_TIME_EFFICIENCY	2
#define WELL_TIME_SCHILLING		3
#define WELL_TIME_PLASMA		4
#define WELL_TIME_RBC			5
#define WELL_TIME_SYSTEM		6
#define WELL_TIME_MDA			7

#define USB_DISK_NOT_ATTACHED	0
#define USB_DISK_ENUMERATED		1
#define USB_DISK_MOUNTED		2
#endif

/*********************************************************************

  MODULE:	GENERAL CALIBRATOR HEADER FILE for CRC-25 -- test of 18bit

  FILE:		crc.h

  DATE:		10/17/06
            08/22/07 -- removed automated constancy 
            03/18/08

  *************************************************************************/

  /* variable type definitions */
typedef unsigned char	uchar;
typedef unsigned int	uint;  	// int = long
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned short bool;
typedef bool BOOL;
typedef uchar BYTE;


#include <time.h>
#define NO_TIME (time_t)(-1)
#include "well.h"
#include "linearity.h"


enum
{
	NUCKEY,
	USERKEY,
	CALKEY,
};

//modes
enum
{
    CRC_CAL,
    CRC_WELL,
};

//returns from  WELL
enum
{
    CAL_RET,    //return to Calibrator
    WELL_RET,   //return to WELL
};    

//container types
enum
{
    REF,
    SYR,
    VIAL,
    CAP,
};    



   /* mu set in act2str */
#define	MU  '$'

#define DIVSIGN (char)0x81
#define MULT (char)'*'
#define MED_DIV ';'

#ifndef TRUE
    #define	TRUE	1
    #define	FALSE	0
#endif


// max # of nuclides than can have new calibration number
#define MAX_NEW_CAL	    40
// max # of nuclides with container correction factors
#define MAX_CAL_COR    20


#define USERNUC  		100

	/* Ci, Bq */
enum
{
	CI,
	BQ,
};


	/* units in measurement */
enum
{
	NO_UNIT,
	UCI,
	MCI,
	_CI,
	KBQ,
	MBQ,
	GBQ,
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
enum
{
    SEC,
    MIN,
    HOUR,
    DAY,
    YEAR,
};

/* measurement modes */
enum MeasMode
{
    MEASMODE,
    TESTMODE,
    ZEROMODE,
    BKGMODE,
    MOBKGMODE,
    MOASSMODE,
};



/* calibrator standard source structure */
typedef struct stnd STAND;
struct stnd				
{
    float	act;		// calibrated activity 
    time_t	caldate;    // calibration date
    bool	daily;     //daily source? 
    uchar	syst;		// Ci or Bq  0xff for no source 
    char	sn[11];		// serial number
    char    nucnum;     //for Well: Cs or Ba
    bool    const_iso;    //constancy?
};




//Mo/Tc data
#define MoTcHl 6.6094
#define MoTcHlUnit HOUR

enum PCCOMM
{
    PC_COMM_NONE,
    PC_COMM_RS232,
    PC_COMM_USB,
};    
    
enum detector_types
{
    DETECTOR_NONE,
    DETECTOR_WELL,
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
     bool lockedbq;           //locked to Bq?
     short pccomm;           //pc communications option? -- see enum
     short source_index;    //index for source key
     bool  demo_mode;       //is it in demo mode?
     char   printer;        //current printer.
 };

/* chamber values -- 1 for each chamber */
typedef struct chambervals CHAMBERVALS;
struct chambervals    
{
    short   chamb_type;         //chamber type: R,Pet,BT
    char    sn[6];
    float	resp_corr;        //response correction
    float   nomvolts;         //nominal volts
    char    init_str[8];
};

// nuclide data
typedef struct nucdata NUCDATA;
struct nucdata
{
    float   halflife;       //halflife of nuclide
    float   response[2];    //response of nuclide: R,PET
    char    name[7];        //name of nuclide
    char    code[7];        //key code
    short  hlunit;         //unit of halflife (M,H,D,Y)
};

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
    float   act;
    char    actstr[12];
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


 //measurement structure -- 1 for each chamber
 typedef struct measurement MEASUREMENT;
 struct measurement
 {
     float  actbuf[40];     // buffer for  moving average
     long  adc_value;      //value from adc
     short  polarity;       //polarity of value
     float  volts;         // chamber voltage reading 
     float  meas;          // measurement value 
     float  act;            // calculated activity -- after unit factor applied
     float  act0;          // activity before unit factor applied
     float  lastmeas;       // last measurement for averaging 
     float  response;      // nuclide's response 
     float  resp0;          // response before response correction 
     float  corfac;         //container correction factor
     bool   valid_flag;     // valid chamber measurement exists */
     bool   display_flag;   //data ready to be displayed
     short  conv_type;
     bool   over_flag;
     bool   wasover;        // over serviced 
     bool   longavg;
     bool   doavg;
     bool   autodu;
     short  ntimes;
     short  isnum;
     short  gain;           /* current vale of gain , 0,1,2 */
     char   actstr[11];     /* formatted activity string */
     char   actstr0[11];    /* previous activity string */
     char   mode;           /* zero mode, test mode, measure mode */
     char   gain_change;     /* direction gain changed: -1,0,+1 */
     char   prevgain;       /* previous gain */
     char   changed;        /* readings after gain changed */
     char   dogain;         /* how gain should be changed */
     char   nmeas;          /* # of measurements */
     char   idec;           //current # of decimal places
     char   idecmin;        /* mininum # of decimal places allowed */
     char   kun;            //unit
     char   syst;           //Ci or Bq
     char   ndec;           //# dec places
     char   kun0;            //previous unit
     char   ndec0;           //previous # of dec places
     char   syst0;          //previous system (Ci or Bq )
     FUTURE future;

 };





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

 //detector structure -- will be in EEPROM
 typedef struct detector DETECTOR;
 struct detector
 {
     char   snum[6];        //serial number
     short  hv;             //high voltage
     bool   calibrated;     //has it been calibrated?
     float  overall_cal;    //overall calibration factor
     float  noise_ratio[2];    //noise ratio, Cs137, Ba133 (Well only)
 };

 
//EEPROM
typedef struct eevals EEVALS;
struct eevals
{
    uchar        syst;           //Ci or Bq
    bool        syslock;        //is system locked to Ci or Bq?
    char        snum[6];        //SN for main unit
    STAND       stand[5];       //5 standard source structures
    STAND       well_stand;     //for Well
    NUCDATA     usernucl[10];   //10 user nuclide structures
    float       mo_tc_lim;        //Mo/Tc limit
    uchar        print[2];       //printer, ticket or line
    USERCAL     usercal[MAX_NEW_CAL];    //user calibration numbers
    CALCORR     cal_corr[MAX_CAL_COR];    //container factors
    USER_KEYS   user_keys[2];   //Chamber,Well
    uchar       mo_choice[4];   //MoAssay choices
    short       maxscr;         //max value for screen saver, in minutes (0 -- always on)
    char        contrast;       //screen contrast
    float       well_trigs[4];  //trigger levels for Well
    WIPENUCS    wipe_nucs[2];   //nuclides for Wipe & Unrestricted -- Well
    char        sealed_nuc[99]; //nuclides for sealed sources -- Well
    WELLCFDATA  well_cf_data[60]; //conversion factor data for Well
    float       gencf;          //generic cf for Well
    DETECTOR    detector;       //detector data for Well
    LINDEF      lindef;         //linearity test definition
    KEYDEF      remote_nucs[8]; //definition of remote nuclides for R,PET
    char        init_str[10];   //initialization string
   
};

//screen memory
typedef struct scrmem SCRMEM;
struct scrmem
{
    uchar  byte[1024];
};


void dateout(char *datestr, time_t *timet, short year_flag);
void timeout(char *timestr, time_t *timet);
void outtime(char *timestr, struct tm *time);
void outdate(char *datestr, struct tm *date, short year_flag);
void beep(void);
void format_activity(float act,short syst,char *actstr);
void act2str(float activ, char *actstr, short ndec, bool zflag, short kun);
short getdec(float act, short syst, short *kunit);
short main_menu(void);
void util_convert(void);
void val_unit_msg(void);
short setup(void);
void set_date_time(void);
void test_source_menu(void);
void facmenu(void);
void calnum_but(void);
float get_response(char calstrng[], short ch_type);
void NuclideData_getNuclide(short index,NUCDATA *nuc);
void NuclideData_getName(short index, char *nucname);
short NuclideData_getIndexFromName(char *nucname);
void getiso(short ch_num, short but);
bool getuser(short ch_num, short but);
short user_key_index(short but);
bool get_new_cal(float *response, short nuc_index,short ch_type);
void mindec(short ch_num);
void nucset(short ch_num);
void nucinit(short ch, bool mode);
void sysbut(void);
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
bool NuclideData_testNuclideTable(ushort *val);
bool same_day(time_t *time0, time_t *time1);
void util_decay(void);
short get_nuclide(short flag, short msg_num);
void dose_table(void);
void sources_key(void);
void cal_from_response(char *strng,float response);
short NuclideData_getNucIndexFromButtonKey(short but,short ch_type);
short well_activity(void);
short beta_activity(void);
void set_crc_mode(short mode);
bool password(void);
short counter_main_menu(void);
short screen_control_menu(void);
short get_num_nucs(void);
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

time_t mk_time(struct tm *tmt);






    
        


























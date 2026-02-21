#pragma once

/*******************************************************************
  MODULE:   definition file for CRC-25W
  
  FILE:     counter.h
  
  DATE:     11/27/06

   ********************************************************************/  




	/* measurement modes */
enum
{
    BKG_MODE,
    WIPE_MODE,
    UNRES_MODE,
    SEALED_MODE,
    GENERAL_MODE,
    TEST_MODE,
    CAL_MODE,
    EFF_MODE,
    LAB_MODE,
};


/* screen position for COUNTING TIME */
#define X_CT	48 //50 
#define Y_CT	0

//screen positions for Nuclide Name
#define X_NUC_WELL  1
#define Y_NUC_WELL  13


	/* display  */
enum
{
    DISP_TEXT,
    DISP_XCD,
    DISP_OK,
    DISP_GRAPH,
};

	/* acquire return values */
enum
{
    PASS,
    FAIL,
    DET_ERR,
};



	/* conversion factor method */
enum
{
    INPUT_CF,
    MEAS_CF,
};

 /* background data structure */
typedef struct back BACK;     
struct back
{
      float         cps[6];         /* counting rate per channel */
      short         counts[6];      /* counts per channel */
      short         sec;            /* measurement period */
      time_t        date;           //measurement date
};


 /* counter data structure */
 typedef struct counter COUNTER;    
 struct counter
 {
    float	act;			// calculated activity
	float	err;			// error
    float   err2;
    float   bcnts;          //background counts
    float   tbkg;           //total bkg counting rate
    float   totcpm;         //total cpm
    float   net;            //net cpm
    short   num_corr;       //number of container correction factors -- Beta
    char	nucnum;			// current nuclide number
	char	type;			// REF (no type), SYR or VIAL 
	char	mode;			// measurement mode 
	char	disp_type;		// text, ok, exceeds, graph 
	char	actstr[11];		// formatted activity string 
	char	errstr[11];     // error string 
	char	sv_str[6];		// syringe / vial string 
	bool    kflag;			// flag for kcpm or kcps
    WELLCFDATA  well_data;    //well data
    char    nuchan[6];     //unpacked channels
    float   chan[6];        //counts in each channel
    float   total_counts;   //all counts including erroneous ones
    float   cpm[6];         //counting rate in each channel
    bool    display_flag;   //data ready for display
    bool    enabled_flag;   //counting enabled flag
 };

//counter test
 typedef  struct c_test C_TEST;
 struct c_test
 {
     char   nuclide;    //1 = Cs, 2 = Ba, 0 = none
     char   fill;
     char   date[6];
     char   gain[6];
     char   en_dev[6];
     char   act_dev[6];

 };

//for calibration
typedef struct ecalib ECALIB;
struct ecalib
{
	float	ratio;
	float	hratio;
	float   low_ratio;
	float	vlt;
	float	vgt;
	float	dev;
	float	dev0;
    float   ratio0;
	short   dlt;
	short	dgt;
	short	dac;
	bool	done;
	short	step_size;
}  ;

#define HIGHRATIO 0.02

//correct values
#define RATIO0_BETA .307   
#define RATIO0_CS   0.7
#define RATIO0_BA   0.9

    
short get_meas_time(short mode);
void display_meas_time(short meastime);
void activity_screen(bool i_flag);
void unpack_chan(char *chans, uchar chbyte);
char pack_chan(char *chans);
void display_counter_nuclide(short plane);
short get_well_data(short nucnum,WELLCFDATA *cfdata, char *nuchan, bool c_flag);
void choose_channels(char *nuchan);
short get_counter_user_key(char ukey);
void input_count_time(char start_ch);
void counter_eff(void);
void count_msg(void);
bool set_bkg(bool h_flag);
short acquire(void);
void meas_error(void);
void det_error(void);
float get_dt(void);
float get_rate_factor(void);
void drawbar(bool flag);
bool format_counts(float val0, char *actstr, bool decflag);
void formae(void);
void cpm2str(float activ, char *actstr);
float calc_total(char *nuchan, short sec, bool flag);
void dtread(void);
void get_idstr(char *str);
time_t get_meas_datime(void);
void print_well_meas(void);
void display_counter_activity(void);
short measure_counter(void);
short calc_well_activity(void);
void print_counter_bkg(void);
void calc_net(short sec, float total);
void calc_err(float total);
void display_id(void);
void read_counters(void);
short get_count_time(void);
void set_count_time(short val);
void set_hv(void);
bool counter_data_exists(short nucnum);
float getdev(float ratio,char *nucname);
void counter_test(void);
short cr_error(short num, bool disp_flag,short min_ctime);
bool bkg_today(bool req_flag);
void set_dac(short dac);

uchar read_control_register(void);
void setup_6channel(void);
void enable_counting(bool on);
void ch_count_time(void);
bool counter_bkg(bool h_flag);










        



		


#pragma once
/*********************************************************
  MODULE:  Daily Test Header for CRC-25

  FILE: daily.h

  DATE:  09/27/06
  *********************************************************/
//background structure
    typedef struct bkgdata BKGDATA;
    struct bkgdata
    {
        float lowbkg;  //starting bkg
        float sum;
        short count;
        short status;
        bool  done;
        short latched_status;
    };

//Bias Voltage structure
    typedef struct biasdata BIASDATA;
    struct biasdata
    {
        float measvolts;
        float minvolts;
        float maxvolts;
        short count;
        short status;
        bool  done;
        bool  wait;
    };

//Zero structure
    typedef struct zerodata ZERODATA;
    struct zerodata
    {
        float zerodisp;
        float sum;
        float znew;
        short count;
        short status;
        short phase;        //ADC_ZERO = 1, OFFSET = 2
        bool  done;
        short barvalue;
    };

//accuracy test data result part
    typedef struct accdatares ACCDATARES;
    struct accdatares  //for a chamber
    {

        float   pred_act;   //predicted activity
        float	ms_act;		//measured activity
        char    pract[12];
        char    msact[12];
        char    var[7];
        char    pr_kun;
        char    ms_kun;
        char    nc;
        time_t	measuredon;
        float	resp0;
        float	diff;
        bool   over_flag;
    };
// accuracy test data -- for each source
    typedef struct accdata ACCDATA;
    struct accdata
    {
        char    sn[11];
        char    nucname[7];
        bool    const_source;      //true if constancy source
        bool    daily[2];          //true if daily source
        short   nuc_index;         //nuclide index
        float	cal_activity;      //calibration activity
        time_t	cal_date;          //calibration date
        ACCDATARES  test_res[8];
    };


//FOR DAILY TEST

//test results
    typedef struct acc_res ACC_RES;
    struct acc_res
    {
        char    source[7];
        char    sn[11];
        char    stand[10];
        char    meas[10];
        char    var[8];
    };
    typedef struct const_res CONST_RES;
    struct const_res
    {
        char    nuclide[7];
        char    act[11];
    };
    typedef struct daily_test_res DAILY_TEST_RES;
    struct daily_test_res
    {
        char    date[6];    //year,month,day,hour,min,sec
        char    ch_num;
        char    ch_type;
        char    zero[6];
        char    bkg[10];
        char    volts[7];
        char    v_err;
        char    data_ok;
        char    fill;
        char    const_source[7];
        char    const_sn[11];
        ACC_RES  acc_res[6];
        CONST_RES const_res[8];
    };

enum test_types
{
    TEST_CLEAR,
    TEST_ZERO,
    TEST_BKG,
    TEST_BIAS,
    TEST_BUSY,
};

enum test_status
{
    TEST_GOOD = '0',
    ZERO_OUT_OF_RANGE,
    ZERO_DRIFT,
    BKG_HIGH,
    BKG_TOO_HIGH,
    BIAS_TEST_FAIL,
    TEST_WAIT,
};

void low_act(short ch_num);
void display_low(short ch_num);
void init_daily_results(void);
bool get_okdata(void);
char *get_hex_str(void);
bool get_auto_const_done(void);
void setup_for_background(short ch_num);
bool measure_bias(short dailyflag);
void setup_for_bias(short ch_num);
void read_bias(short ch_num);
bool measure_zero_offset(bool daily_flag);
void measure_bkg(short daily_flag);
void set_bias(short ch_num);
void set_zero(short ch_num);
void low_mo_bkg(short ch_num);
void low_mo_act(short ch_num);
void read_zero(short ch_num);
void setup_for_zero_adc(short ch_num);
bool test_done(char ch, short ch_num, char test_type);
void get_acc_data(short ciso,bool daily, short ch_num);
float calc_dev(short nacc, short ch_num);
void save_acc_to_daily_res(short ch_num);
short get_const_source(void);
short get_const_source2(void);
void start_daily_res(short ch_num);
void format_bkg(short ch_num, char *str);
void delay_bias(short ch_type);

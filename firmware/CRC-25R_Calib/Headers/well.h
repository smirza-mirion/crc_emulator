#pragma once
/*******************************************************************************
  MODULE:  Definition File for Well Portion  OF crc-25w

  FILE:    Well.h

  DATE:    09/05/06
  
***********************************************************************/

    

#define GENERIC_NUC		99
#define GENERAL_NUC		-2
#define ALL			-1

//Well existence
enum
{
    CONNECTION_ERROR,
    NO_WELL,
    WELL_CONNECTED,
};    


	/* wipe report type */
enum
{
    GOOD,
    BAD,
    CPM,
};




/* screen position for wipe nuclide */
#define X_WELL_NUC	1
#define Y_WELL_NUC	13

//sample number
#define X_SAMP  16
#define Y_SAMP  0


/* screen position for mode designation */
#define X_MODE	0
#define Y_MODE	0

typedef struct well_cf_data WELLCFDATA;
struct well_cf_data
{
    float   factor;         //conversion factor
    char    nucname[7];     //nuclide name
    uchar   channels;       //packed form of channels
};  


/* reports of nuclides included in wipe */
typedef struct wipe_rept WIPE_REPT;
struct wipe_rept			
{
	char    actstr[7];		/* activity string*/
	char	errstr[7];		/* error string */
	char	kflag;
	char	name[7];			/* nuclide name */
	char	type;			/* GOOD or BAD */
	char    nucnum;
    char    nuchan[6];     //unpacked channels
};
	/* nuclides for wipe */
typedef struct wipenucs WIPENUCS;
struct wipenucs
{
	  char nuc[10];
	  short num;
};

//structures for wipes saved to SD card
typedef struct wipe_meas_info WIPE_MEAS_INFO;
struct wipe_meas_info
{
    char    num_wipes;
    char    date[5];    //year,mon,day,hour,min
    char    ct_str[4]; //count time in ascii
    char    idstr[4];   //ID string ex: W-01
    
};    

typedef struct wipe_data WIPE_DATA;
struct wipe_data
{
    char    actstr[10];
    char    nuclide[6];
    char    type;           //good or bad
};

typedef struct wipes WIPES;
struct wipes
{
    WIPE_MEAS_INFO  meas_info;
    WIPE_DATA       wipe_data[12];
};    


void set_meas_num(short index, short num);
short get_meas_num(short index);
void init_meas_num(void);
void change_sample(void);
void set_sampnum(short num);
short get_sampnum(void);

short get_num_wipes(void);
bool testbkg(void);
void calc_activity(void);
bool wipe_display(short type);
void init_saved_wipes(void);

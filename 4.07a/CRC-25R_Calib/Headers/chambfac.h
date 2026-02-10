/*********************************************************************
  MODULE:   CHAMBER FACTORS HEADER FILE

  FILE:     chambfac.h

  DATE:     08/30/04


  *************************************************************************/

#ifndef __CRC_H__
	#include "crc.h"
#endif

//chamber types
enum
{
    R_CHAMB,    //REGULAR
    P_CHAMB,    //PET
    B_CHAMB,    //BT
	ONE_DOT_EIGHT_CHAMB,	// 1.8 Atm
	C_CHAMB,	// 100 Ci
	K_CHAMB,	// 1000 Ci
	UPPER_LIMIT_CHAMB	// Beginning of Invalid Chamber ID
};

float get_gainfact(short ch_type);
float calc_response(short ch_type ,short cnum);
short calc_cnum(short ch_type, float response);
bool response_lt_min(short ch_type, float response);
char get_decmin(short ch_type,short system);
char get_decmin_1(short ch_type,short system);
float get_volts_factor(short ch_type);
void get_stndstr(short index, char *str);
float get_nomvolts(short ch_type);
float get_minvolts(short ch_type);
float get_maxvolts(short ch_type);
short chamber_type(short ch_num);
bool chamber_hv_adc(short ch_num);
bool chamber_one_gain_relay(short ch_num);
bool chamber_stored_gain_factor(short ch_num);
double chamber_gain_factor_0(short ch_num);
double chamber_gain_factor_1(short ch_num);
double chamber_gain_factor_2(short ch_num);
double chamber_low_gain_op_amp_offset_correction(short ch_num);
float chamber_response_correction(short ch_num);
float chamber_nominal_volts(short ch_num);
bool chamber_mcp3550_chip(short ch_num);
bool chamber_77t(void);
bool chamber_C(void);
bool chamber_K(void);
short chamber_type_77t_remap(short ch_type);

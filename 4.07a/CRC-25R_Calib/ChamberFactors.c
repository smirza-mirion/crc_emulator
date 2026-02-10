    /*********************************************************************
  MODULE:	Chamber Factors

  FILE:		Chamber Factors.c

  DATE:		03/10/05

  *************************************************************************/


#include "crc.h"
#include "chambfac.h"
#include <string.h>

//gainfact: max counts (32000) of Co57 (R = .1838) = 200 GBq
// = 200e9 x .1838 / (32000 * 3.7e10)

static const float gainfact[] = { 3.105e-5, 3.105e-4, 3.105e-4, 3.105e-4, 1.6080e-3, 4.584e-3};


/*static const char *stndstr[] = {
        "Co57",
        "Co60",
        "Ba133",
        "Cs137",
        "Ra226",
        "Na22",
}; */

static const char *stndstr[] = {
        "Co57",
        "Co60",
        "Ba133",
        "Cs137",
        "Na22",
        "Ge68",
};


typedef struct biasvals BIASVALS;
struct biasvals
{
    float nomvolts;
    float minvolts;
    float maxvolts;
    float volts_factor;
};

static const BIASVALS biasvals[] =
    {
        155.,
        135.,
        200.,
        0.0189,  

        500.,
        400.,
        550.,
        0.2008,

		155.,
		135.,
		200.,
		0.2008,

		155.,
		135.,
		200.,
		0.2008,

		155.,
		135.,
		200.,
		0.2008,

		155.,
		135.,
		200.,
		0.2008,
    };   

typedef struct dec_min DECMIN;
struct dec_min
{
    char  decmin[2];
    char  decmin_1[2];

};

static const DECMIN dec_min[] =
    {
        4,2,
        2,1,

        5,3,
        2,1,

		5,3,
		2,1,

		5,3,
		2,1,

		0,0,
		0,0,

		0,0,
		0,0,
    };


typedef struct calnumvals CALNUMVALS;
struct calnumvals
{
    float minr;         //minimum value of R
    float fac;          //factor
    float min_resp;     //response for cal# = 10
};    

static const CALNUMVALS calnum_vals[] =
    {
        0.0797,
        1075.68,
        0.089,
 
        0.0555,
        1048.2,
        0.0650,

		0.0555,
		1048.2,
		0.0650,

		0.0555,
		1048.2,
		0.0650,

		0.01027,
		1000.27,
		0.02026,

		0.01027,
		1000.27,
		0.02026,
    };


    float get_gainfact(short ch_type)
    {
        return(gainfact[ch_type]);
    }
    

    //calculate response from calibration number
    float calc_response(short ch_type ,short cnum)
    {
        float fac = calnum_vals[ch_type].fac;
        float minr = calnum_vals[ch_type].minr;
        
        return((float)cnum / fac + minr);
    }

    //calculate calibration number from response
    short calc_cnum(short ch_type, float response)
    {
        float fac = calnum_vals[ch_type].fac;
        float minr = calnum_vals[ch_type].minr;

        return((short)((response - minr) * fac + 0.5));
    }

    /* test response against minimum
       return TRUE if < mininum */
    bool response_lt_min(short ch_type, float response)
    {
        float min_resp = calnum_vals[ch_type].min_resp;
        
        if (response < min_resp)
            return TRUE;
        else
            return FALSE;
    }

    


    char get_decmin(short ch_type,short system)
    {
        return(dec_min[ch_type].decmin[system]);
    }

    char get_decmin_1(short ch_type,short system)
    {
        return(dec_min[ch_type].decmin_1[system]);
    }



        //standard isotope strings

    void get_stndstr(short index, char *str)
    {
        
        strcpy(str,stndstr[index]);
    }


    float get_nomvolts(short ch_type)
    {
        return(biasvals[ch_type].nomvolts);
    }

    float get_minvolts(short ch_type)
    {
        return(biasvals[ch_type].minvolts);

    }    

    float get_maxvolts(short ch_type)
    {
        return(biasvals[ch_type].maxvolts);

    }    

    float get_volts_factor(short ch_type)
    {
        return(biasvals[ch_type].volts_factor);
    }




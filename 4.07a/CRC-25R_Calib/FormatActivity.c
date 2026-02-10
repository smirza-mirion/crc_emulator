/*********************************************************************
  MODULE:	Format Activity

  FILE:		FormatActivity.c

  DATE:		03/01/05

  
  *************************************************************************/


#include "crc.h"
#include "chambfac.h"
#include <string.h>
	extern const short ndecci[],ndecbq[],ndec77tci[];
	extern const float upci[],upci77t[],upbq[];
    extern const float unitfact[];
	extern const char *unit_str[];
	extern CURRENT current;
	extern volatile short DisplayActivity_addedDigits;

	void trim_and_shrink(char *acByte);

	void act2str(float activ, char *actstr, short ndec, bool zflag, short kun)
	{
		char formatstr[7], teststr[10];	/* formatting string */
		short	i;			/* loop counter */
        float aval;         //abs value
        bool neg_flag;      //negative value flag
        bool zero_flag;

		/* set up format string with variable # of decimal places */
		strcpy(formatstr,"%#6. f");
		formatstr[4] = ndec + 0x30;

		//get absolute value
        aval = activ;
        neg_flag = FALSE;
        if (activ < 0.)
        {
            aval = -activ;
            neg_flag = TRUE;
        }

        // change to ascii 
        (void)sprintf(actstr,formatstr,aval);
        if(actstr[strlen(actstr) - 1] == '.'){
        	actstr[strlen(actstr) - 1] = 0;
        	strcpy(teststr, " ");
        	strcat(teststr, actstr);
        	strcpy(actstr, teststr);
        }

        if (zflag){
        	if(((actstr[2]<49) || (actstr[2]>57)) && (actstr[3] == '.')){
            actstr[2] = '0';

        	}
        }
		if (kun != 0)
        {
            /* add on unit in ascii */
            strcat(actstr,unit_str[kun - 1]);
        }

		//if positive, finished
		if (!neg_flag)
			return;

		/* no minus sign if 0 to shown # of decimal places */
		zero_flag = TRUE;
		for(i = 0; i < 6; i++)
		{
			if (!(actstr[i] == ' ' || actstr[i] == '.' ||
					actstr[i] == '0'))
			{
				zero_flag = FALSE;
				break;
			}
		}

		if (!zero_flag)
			actstr[0] = '-';
	}

	void act2strnew(float activ, char *actstr, short ndec, bool zflag, short kun)
	{
		char formatstr[10];	/* formatting string */
		char teststr[10];
		short	i;			/* loop counter */
        float aval;         //abs value
        bool neg_flag;      //negative value flag
        bool zero_flag;
        short totallen;
        short totalsign;

        totallen = 6;
        totallen += DisplayActivity_addedDigits;
        totalsign = ndec + DisplayActivity_addedDigits ;

        sprintf(formatstr, "%%#%d.%df", totallen, totalsign);

		/* set up format string with variable # of decimal places */
		//strcpy(formatstr,"%#6. f");
		//formatstr[4] = ndec + 0x30;

		//get absolute value
        aval = activ;
        neg_flag = FALSE;
        if (activ < 0.)
        {
            aval = -activ;
            neg_flag = TRUE;
        }

        // change to ascii
        (void)sprintf(actstr,formatstr,aval);
        if(actstr[strlen(actstr) - 1] == '.'){
        	actstr[strlen(actstr) - 1] = 0;
        	strcpy(teststr, " ");
        	strcat(teststr, actstr);
        	strcpy(actstr, teststr);
        }

        strcpy(teststr, actstr);
        trim_and_shrink(teststr);

        if(strlen(teststr) > 6){
        	DisplayActivity_addedDigits = 0;

        	totallen = 6;
        	totallen += DisplayActivity_addedDigits;
        	totalsign = ndec + DisplayActivity_addedDigits ;

        	sprintf(formatstr, "%%#%d.%df", totallen, totalsign);
        	(void)sprintf(actstr,formatstr,aval);
        	if(actstr[strlen(actstr) - 1] == '.'){
				actstr[strlen(actstr) - 1] = 0;
				strcpy(teststr, " ");
				strcat(teststr, actstr);
				strcpy(actstr, teststr);
        	}
        }

        if (zflag){
        	if(((actstr[2]<49) || (actstr[2]>57)) && actstr[3] == '.'){
        		actstr[2] = '0';
        	}
        }

		if (kun != 0)
        {
            /* add on unit in ascii */
            strcat(actstr,unit_str[kun - 1]);
        }

		//if positive, finished
		if (!neg_flag)
			return;

		/* no minus sign if 0 to shown # of decimal places */
		zero_flag = TRUE;
		for(i = 0; i < 6; i++)
		{
			if (!(actstr[i] == ' ' || actstr[i] == '.' || actstr[i] == '0'))
			{
				zero_flag = FALSE;
				break;
			}
		}

		if (!zero_flag)
			actstr[0] = '-';
	}

    void format_activity(float act, short syst, char *actstr)
    {
        short ndec;
        short kunit;
        float act_disp;

        ndec = getdec(act,syst,&kunit);
        act_disp = act * unitfact[kunit - 1];
        act2str(act_disp, actstr, ndec, FALSE, kunit);
        
    }

    void act2str2(float activ, char *actstr, short kun, short sigdigits)
	{
		char formatstr[7];	/* formatting string */
		short	i;			/* loop counter */
        char test[20];
        short ndec;
        short significant_digits;
        bool encountered_decimal;

        sprintf(test, "%f", activ);

        i = 0;
        ndec = 0;
        encountered_decimal = FALSE;
        significant_digits = sigdigits;
        while(test[i] != 0){
        	if((test[i] == '0') ||
        		(test[i] == '1') ||
        		(test[i] == '2') ||
        		(test[i] == '3') ||
        		(test[i] == '4') ||
        		(test[i] == '5') ||
        		(test[i] == '6') ||
        		(test[i] == '7') ||
        		(test[i] == '8') ||
        		(test[i] == '9')
        	){
        		if(encountered_decimal && (significant_digits > 0)) ndec++;
        		if(i==0){
        			if(test[i]!='0') significant_digits--;
        		}else{
        			significant_digits--;
        		}
        	}else if(test[i] == '.'){
        		encountered_decimal = TRUE;
        	}
        	i++;
        }

        /* set up format string with variable # of decimal places */
        strcpy(formatstr, "%. f");
        formatstr[2] = ndec + 0x30;

        // change to ascii
        sprintf(actstr, formatstr, activ);

        i = 0;
        encountered_decimal = FALSE;
        while(actstr[i]!=0){
        	if(actstr[i]=='.'){
        		encountered_decimal = TRUE;
        		break;
        	}
        	i++;
        }

        if(encountered_decimal){
        	i = strlen(actstr) - 1;
        	while(i!=0){
        		if(actstr[i]=='0') actstr[i] = 0;
        		else if(actstr[i]=='.'){
        			actstr[i] = 0;
        			break;
        		}else{
        			break;
        		}
        		i--;
        	}
        }

		if (kun != 0){
            /* add on unit in ascii */
			strcat(actstr, " ");
            strcat(actstr,unit_str[kun - 1]);
        }
}

    short format_activity_kunit(float act, short syst, char *actstr){
    	short ndec;
        short kunit;
        float act_disp;

        ndec = getdec(act,syst,&kunit);
        act_disp = act * unitfact[kunit - 1];
        act2str(act_disp, actstr, ndec, FALSE, kunit);
        return kunit;
    }

    short format_activity_kunit2(float act, short syst, char *actstr){
        short kunit;
        float act_disp;

        getdec(act,syst,&kunit);
        act_disp = act * unitfact[kunit - 1];
        act2str2(act_disp, actstr, kunit, 4);
        return kunit;
    }
	
    short format_activity_kunit_kbq(float act, short syst, char *actstr){
    	short ndec;
        short kunit;
        float act_disp;

        ndec = getdec_kbq(act,syst,&kunit);
        act_disp = act * unitfact[kunit - 1];
        act2str(act_disp, actstr, ndec, FALSE, kunit);
        return kunit;
    }

    short format_activity_system(float actci, char *actstr){
    	float activity;
    	short kunit;

    	activity = actci;
    	if(current.system == BQ){
    		activity *= BQFACTOR;
    	}
    	kunit = format_activity_kunit(activity, current.system, actstr);
    	return kunit;
    }

    short format_activity_system2(float actci, char *actstr){
    	float activity;
    	short kunit;

    	activity = actci;
    	if(current.system == BQ){
    		activity *= BQFACTOR;
    	}
    	kunit = format_activity_kunit2(activity, current.system, actstr);
    	return kunit;
    }

    short format_activity_system_kbq(float actci, char *actstr){
    	float activity;
    	short kunit;

    	activity = actci;
    	if(current.system == BQ){
    		activity *= BQFACTOR;
    	}
    	kunit = format_activity_kunit_kbq(activity, current.system, actstr);
    	return kunit;
    }

    void replace(char *inputoutput, char searchchar, char replacechar){
    	while(*inputoutput != 0){
    		if(*inputoutput == searchchar) *inputoutput = replacechar;
    		inputoutput++;
    	}
    }

    short getdec(float act, short syst, short *kunit)
	{

		short i,j;
		float activ;
		short kun;
		short ndec;

		activ = act;
		if (activ < 0.)
			activ = -activ;

		/* CURIES */
		if (syst == CI)
		{
			if(chamber_77t()){
				j = 7;
				for(i = 0; i < 8; i++)
				{
					if (activ <= upci77t[i])
					{
						j = i;
						break;
					}
				}
			}else{
				j = 8;
				for(i = 0; i < 9; i++)
				{
					if (activ <= upci[i])
					{
						j = i;
						break;
					}
				}
			}

			if(chamber_77t()) ndec = ndec77tci[j];
			else ndec = ndecci[j];

			if(chamber_77t()){
				kun = MCI;
				if (j > 3)
					kun = _CI;
			}else{
				kun = UCI;
				if (j > 2)
					kun = MCI;
				if (j > 6)
					kun = _CI;
			}
		}


		/* BQ */
		if (syst == BQ)
		{
			j = 6;
			for(i = 0; i < 7; i++)
			{
				if (activ <= upbq[i])
				{
					j = i;
					break;
				}
			}
			ndec = ndecbq[j];
			kun = MBQ;
			if (j > 3)
				kun = GBQ;
		}
		*kunit = kun;
		return ndec;
	}

short getdec_kbq(float act, short syst, short *kunit){
	short i,j;
	float activ;
	short kun;
	short ndec;

	activ = act;
	if(activ < 0.) activ = -activ;

	/* CURIES */
	if (syst == CI){
		if(chamber_77t()){
			j = 7;
			for(i=0; i<8; i++){
				if (activ <= upci77t[i]){
					j = i;
					break;
				}
			}
		}else{
			j = 8;
			for(i=0; i<9; i++){
				if (activ <= upci[i]){
					j = i;
					break;
				}
			}
		}

		if(chamber_77t()) ndec = ndec77tci[j];
		else ndec = ndecci[j];

		if(chamber_77t()){
			kun = MCI;
			if (j > 3) kun = _CI;
		}else{
			kun = UCI;
			if (j > 2) kun = MCI;
			if (j > 6) kun = _CI;
		}
	}

	/* BQ */
	if(syst == BQ){
		kun = KBQ;
		ndec = 4;
		if(activ >= 1.0e+3){
			kun = KBQ;
			ndec = 3;
		}
		if(activ >= 1.0e+4){
			kun = KBQ;
			ndec = 2;
		}
		if(activ >= 1.0e+5){
			kun = KBQ;
			ndec = 1;
		}
		if(activ >= 1.0e+6){
			kun = MBQ;
			ndec = 3;
		}
		if(activ >= 1.0e+7){
			kun = MBQ;
			ndec = 2;
		}
		if(activ >= 1.0e+8){
			kun = MBQ;
			ndec = 1;
		}
		if(activ >= 1.0e+9){
			kun = GBQ;
			ndec = 3;
		}
		if(activ >= 1.0e+10){
			kun = GBQ;
			ndec = 2;
		}
		if(activ >= 1.0e+11){
			kun = GBQ;
			ndec = 1;
		}
		if(activ >= 1.0e+12){
			kun = GBQ;
			ndec = 0;
		}
	}
	*kunit = kun;
	return ndec;
}

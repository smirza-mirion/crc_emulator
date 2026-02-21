/*********************************************************************
  MODULE:	NUCLIDE / CAL BUTTONS  for CRC-25

  FILE:		NuclideCal_25.c

  DATE:		05/22/07

  ANALYSIS: controls use of NUCL key and Cal# key

  OUTLINE:	 displays messages
			 gets nuclide index via user input
    		 gets nuclide data according to input


  *************************************************************************/
#include "crc.h"
#include "screen.h"
#include "message.h"
#include "chambfac.h"
#include "i2c.h"
#include "keyboard.h"
#include "uart.h"
#include "amulet.h"
#include "nuc.h"
#include <stdlib.h>
#include <string.h>

	extern CHAMBER chamber[];
    extern MEASUREMENT measurement[];
	extern CURRENT current;
	extern volatile bool DisplayActivity_SendActivity;

	void trim_and_shrink(char *acByte);
	void GetExtendedNuclideString(unsigned char ucNuclideID, char *acMsg);
	void send_to_amulet_string(uchar ucIndex, char message0[]);

    void nucbut(void)
	{
		short nuc_index;
        short ch_num = current.main_chamber;  //chamber is Main Chamber

		for(;;)
		{

            erase_screen();

			specify_nuclide_msg();

			nuc_index = get_nuclide_index(TRUE);


			erase_screen();
			switch (nuc_index)
			{
			case -1:
				no_nuclide_msg();
				break;

			case -2:
				specify_further_msg();
				break;

            case -4:  //No cal number for the nuclide
                no_calnum_msg();
                //use same nuclide so fall through to
                //case -3
			case -3:   /* ENTER pressed -- same nuclide */
                erase_screen();
                set_meas_screen(TRUE);
                if (get_crc_mode() == CRC_CAL)
                    nucset(ch_num);
				return;

            default:
                set_meas_screen(TRUE);

                if (get_crc_mode() != CRC_CAL)
                    return;

                if(!set_nuclide_data(nuc_index,ch_num))
                {
                    no_calnum_msg();
                    break;
                }



				return;
			}
            if(home_set())
            {
                erase_screen();
                set_meas_screen(TRUE);
                if (get_crc_mode() == CRC_CAL)
                    nucset(ch_num);
                return;
            }

		}
	}

 void no_nuclide_msg(void)
	{
		beep();
		//display_text(32,12,"NO SUCH",0,MEDIUM,NORMAL);
		//display_text(32,22,"NUCLIDE",0,MEDIUM,NORMAL);
		//display_text(24,32,"IN MEMORY",0,MEDIUM,NORMAL);
        display_medium_message(NUCLIDE_BUTTON_1,10,0,NORMAL);
        display_medium_message(NUCLIDE_BUTTON_2,22,0,NORMAL);
        display_medium_message(NUCLIDE_BUTTON_3,34,0,NORMAL);
		contmsg();
	}

 void specify_nuclide_msg(void)
	{
		//display_text(32,12,"SPECIFY",0,MEDIUM,NORMAL);
		//display_text(32,22,"NUCLIDE",0,MEDIUM,NORMAL);
        display_medium_message(NUCLIDE_BUTTON_4,12,0,NORMAL);
        display_medium_message(NUCLIDE_BUTTON_2,24,0,NORMAL);
	 }

 void specify_further_msg(void)
	{
		beep();
		//display_text(32,12,"SPECIFY",0,MEDIUM,NORMAL);
		//display_text(32,22,"FURTHER",0,MEDIUM,NORMAL);
        display_medium_message(NUCLIDE_BUTTON_4,12,0,NORMAL);
        display_medium_message(NUCLIDE_BUTTON_5,24,0,NORMAL);

		contmsg();
	}

    void no_calnum_msg(void)
    {
        beep();
        //CAL #
        //NOT ENTERED
        //FOR NUCLIDE
        display_medium_message(NUCLIDE_BUTTON_6,10,0,NORMAL);
        display_medium_message(NUCLIDE_BUTTON_7,22,0,NORMAL);
        display_medium_message(NUCLIDE_BUTTON_8,34,0,NORMAL);
        contmsg();
    }



#define MAX_NUM_MATCHES  5
    /* input nuclide name & return nuclide index */
    extern CURRENT current;




bool new_cal_exist(short nuc_index);

    short get_nuclide_index(bool flag)
    {
        NUCDATA tempnuc;
        short i,ii;
        short len,match,kmatch;
        bool nucover;
        char strng[10];
        bool nuc_exists;     //nuclide data exists?
        bool cal_exists;    //cal# exists ?
        bool cal_flag;      //cal# for current nuclide
        char buffer[10];
        short maxnuc;
        short ch_num = current.main_chamber;
        short ch_type;

        ch_type = chamber_type(ch_num);


        maxnuc = NuclideData_getNumOfNucs();

        nucover = FALSE;

        if(!numstr_in(24, 52,strng,MEDIUM,0,5,5,TRUE,FALSE))
            return -3;

        len = 5;
        for(i = 0; i < 5; i++)
        {
            if(strng[i] == '\0')
            {
                len = i;
                break;
            }
        }


        match = -1;
        nuc_exists = FALSE;
        cal_exists = FALSE;
        for(ii = 0; ii < maxnuc; ii++)
        {
            //watchdog();
            NuclideData_getNuclide(ii,&tempnuc);
            if (strncmp(&tempnuc.code[0],strng,len) == 0)
            {
                nuc_exists = TRUE;
                cal_flag = TRUE;
                if(flag && (get_crc_mode() == CRC_CAL &&
                            tempnuc.response[ch_type] == 0.))
                {
                    if(!new_cal_exist(ii))
                        //no cal# for nuclide or for type
                        cal_flag = FALSE;

                }
                if(cal_flag)
                {
                    ++match;
                    if (match == MAX_NUM_MATCHES)
                    {
                        nucover = TRUE;
                        break;
                    }

                    cal_exists = TRUE;
                    buffer[match] = ii;
                }
            }
        }

        /* now search user nuclides */
        if (!nucover)
        {
            for(ii = 0; ii < 10; ii++)
            {
                //EE_READ(usernucl[ii],(uchar *)&tempnuc);
                if (tempnuc.hlunit == -1)   // deleted
                    continue;

                if (strncmp(&tempnuc.code[0],strng,len) == 0)
                {
                    nuc_exists = TRUE;
                    cal_flag = TRUE;
                    if(flag && (get_crc_mode() == CRC_CAL &&
                                tempnuc.response[ch_type] == 0))
                    {
                        if(!new_cal_exist(ii))
                            //no cal# for nuclide or for type
                            cal_flag = FALSE;
                    }
                    if(cal_flag)
                    {
                        ++match;
                        if (match == MAX_NUM_MATCHES)
                        {
                            nucover = TRUE;
                            break;
                        }

                        cal_exists = TRUE;
                        buffer[match] = ii + USERNUC;
                    }
                }
            }
        }


        // no nuclide
        if (!nuc_exists)
            return -1;

        if(!cal_exists)
            return -4;      //no cal# for nuclide for this type

        if (nucover)
            return -2;


        // unique
        if (match == 0)
            kmatch = buffer[0];

        else
        {

            erase_screen();
            for(i = 0; i <= match; i++)
            {
                kmatch = buffer[i];
                strcpy(strng,"          ");
                strng[0] = (char)i + '1';
                strng[1] = '.';
                NuclideData_getName(kmatch,&strng[3]);
                display_text(8,10 * i + 2,strng,0,MEDIUM,NORMAL);
            }
            i = choose(1,match + 1);
            kmatch = buffer[i - 1];
            erase_screen();
            if(home_set())
                return -3;  //use same nuclide if home pressed


        }

        return kmatch;

    }


    bool new_cal_exist(short nuc_index)
    {
        short i;
        bool exist;
        USERCAL ucal;

        exist = FALSE;
        for (i = 0; i < MAX_NEW_CAL_OBSOLETE; i++)
        {

            //EE_READ(usercal[i],(uchar *)&ucal);
            if ((ucal.nuc_index == nuc_index))
            {
                exist = TRUE;
                break;
            }
        }
        return exist;
    }


    void calnum_but(void)
    {
        char calstrng[10];
        float response;
        short ch = current.main_chamber;    //chamber is Main chamber
        short ch_type;
        char nucshrink[25];
        char message[51];

        ch_type = chamber_type(ch);


        erase_screen();
        //ENTER
        //CALIBRATION #
        display_medium_message(CAL_NUM_1,2,0,NORMAL);
        display_medium_message(CAL_NUM_2,12,0,NORMAL);


        do
        {
            if(!numstr_in(24,52,calstrng,MEDIUM,0,8,8,TRUE,TRUE))
            {
                //ENTER or HOME pressed
                erase_screen();
                set_meas_screen(TRUE);
                nucset(ch);
                return;
            }

            response = get_response(calstrng,ch_type);
            if(response == -1)
            {
                erase_lines(52,12,0);
                beep();
            }
        }while(response == -1);

        erase_screen();
        chamber[ch].calkey = TRUE;
        measurement[ch].response = response;
        measurement[ch].resp0 = response;
        strncpy(&chamber[ch].calstrng[0],calstrng,10);
        erase_text_line(56,SMALL,0);
        nucset(ch);
        lowresp(ch);
        strcpy(&chamber[ch].nucdata.name[0],"      ");  /* for PC */
        strcpy(nucshrink, chamber[ch].nucdata.name);
		trim_and_shrink(nucshrink);
		if(DisplayActivity_SendActivity){
			if(nucshrink[0]!=0){
				GetExtendedNuclideString(chamber[ch].nuc_index, message);
				send_to_amulet_string(0, message);
			}
			else send_to_amulet_string(0, "");
			//else send_to_amulet_string(0, "*****");
		}
    }

enum
{
    NO_FAC,
    MULT_FAC,
    DIV_FAC,
};
    float get_response(char calstrng[], short ch_type)
    {

        short i,ii;
        float resp;
        float response;
        short cnum,fac;
        short muldiv;
        bool ok;
        char ch;
        char buffer[10];

        muldiv = NO_FAC;

        /* only 4 digits allowed in number part */
        ok = FALSE;
        for(i = 0; i < 5; i++)
        {
            ch = calstrng[i];
            if(ch < '0' || ch > '9')
            {
                ok = TRUE;
                break;
            }
        }
        if(!ok)
            return(-1);

        for(i = 0; i < 5; i++)
            buffer[i] = ' ';
        buffer[5] = '\0';

        i = 0;
        for(;;)
        {
            ch = calstrng[i];
            if (ch >= '0' && ch <= '9')
            {
                buffer[i] = ch;
                ++i;
            }
            else
                break;
        }

        ii = i;
        if (ii == 0)
            return (-1);

        if (!(ch == MULT || ch == DIVSIGN || ch == '\0'))
            return (-1);

        if (ch == MULT)
            muldiv = MULT_FAC;
        if (ch == DIVSIGN)
            muldiv = DIV_FAC;

        cnum = atoi(buffer);
        if(cnum > 1200 || cnum < 10)
            return(-1);


        if (muldiv != NO_FAC)
        {
            for(i = 0; i < 4; i++)
                buffer[i] = ' ';
            buffer[4] = '\0';
            i = 0;
            for(;;)
            {
                ch = calstrng[i + ii + 1];
                if (ch >= '0' && ch <= '9')
                {
                    buffer[i] = ch;
                    ++i;
                }
                else
                    break;
            }
            if (i == 0)
               return(-1);

            fac = atoi(buffer);
        }

        if (muldiv == MULT_FAC)
        {
            if (!(fac == 10 || fac == 100))
                return(-1);
        }

        if (muldiv == MULT_FAC && (fac == 100 && cnum > 999))
            return(-1);   /* max = 999 x 100 */

        if (muldiv == DIV_FAC && (cnum < 400 || fac != 2))
            return(-1);

        resp = calc_response(ch_type,cnum);

        switch(muldiv)
        {
        case NO_FAC:
            response = resp;
            break;
        case MULT_FAC:
            response = resp / (float)fac;
            break;
        case DIV_FAC:
            response = resp * (float)fac;
            break;
        }

        return(response);
    }

float NuclideCal_getResponse(char calstrng[], short ch_type){
	short i,ii;
	float resp;
	float response;
	short cnum,fac;
	short muldiv;
	bool ok;
	char ch;
	char buffer[10];

	muldiv = NO_FAC;

	/* only 4 digits allowed in number part */
	ok = FALSE;
	for(i=0; i<5; i++){
		ch = calstrng[i];
		if(ch<'0' || ch>'9'){
			ok = TRUE;
			break;
		}
	}

	if(!ok) return (-1);

	for(i=0; i<5; i++) buffer[i] = ' ';
	buffer[5] = '\0';

	i=0;
	for(;;){
		ch = calstrng[i];
		if(ch>='0' && ch<='9'){
			buffer[i] = ch;
			++i;
		}else break;
	}

	ii = i;
	if(ii==0) return (-1);

	if(!(ch==MULT || ch=='/' || ch=='\0')) return (-1);

	if(ch==MULT) muldiv = MULT_FAC;
	if(ch=='/') muldiv = DIV_FAC;

	cnum = atoi(buffer);
	if(cnum>1200 || cnum<10) return (-1);

	if(muldiv!=NO_FAC){
		for(i=0; i<4; i++) buffer[i] = ' ';
		buffer[4] = '\0';
		i = 0;
		for(;;){
			ch = calstrng[i + ii + 1];
			if(ch>='0' && ch<='9'){
				buffer[i] = ch;
				++i;
			}else break;
		}
		if(i==0) return (-1);
		fac = atoi(buffer);
	}

	if(muldiv==MULT_FAC){
		if(!(fac==10 || fac==100)) return (-1);
	}

	if(muldiv==MULT_FAC && (fac==100 && cnum>999)) return (-1);   /* max = 999 x 100 */

	if(muldiv==DIV_FAC && (cnum<400 || fac!= 2)) return (-1);

	resp = calc_response(ch_type, cnum);

	switch(muldiv){
		case NO_FAC:
			response = resp;
			break;

		case MULT_FAC:
			response = resp / (float)fac;
			break;

		case DIV_FAC:
			response = resp * (float)fac;
			break;
	}

	return (response);
}

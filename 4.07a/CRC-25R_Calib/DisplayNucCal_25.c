/*********************************************************************
  MODULE:	DISPLAY NUCLIDE NAME AND CALIBRATION NUMBER etc  for CRC-25

  FILE:		DisplayNucCal_25.c

  DATE:		09/07/06
            03/26/08 added display of Cal# as entered instead of from response

  ANALYSIS: displays nuclide name and cal# in activity measurement

  *************************************************************************/
#include "crc.h"
#include "screen.h"
#include "chambfac.h"
#include "message.h"
#include "remote.h"
#include "uart.h"
#include "amulet.h"
#include <string.h>

    extern const char sdiv;
	extern CURRENT  current;
	extern CHAMBER  chamber[];
	extern volatile bool DisplayActivity_SendActivity;

    extern FONT fonts[];
    void cal_from_response(char *strng,float response);
    void trim_and_shrink(char *acByte);
    void GetExtendedNuclideString(unsigned char ucNuclideID, char *acMsg);
    void send_to_amulet_string(uchar ucIndex, char message0[]);

	/* display nuclide in calibrator */
	void disp_nuclide(void)
	{
        short ch_num = current.main_chamber;
        char nucshrink[25];
        char message[51];

        erase_lines(2,9,0);
        display_text(0,2,&chamber[ch_num].nucdata.name[0],0,MEDIUM,NORMAL);
        strcpy(nucshrink, chamber[ch_num].nucdata.name);
		trim_and_shrink(nucshrink);
		if(DisplayActivity_SendActivity){
			if(nucshrink[0]!=0){
				GetExtendedNuclideString(chamber[ch_num].nuc_index, message);
				send_to_amulet_string(0, message);
			}
			else send_to_amulet_string(0, "");
			//else send_to_amulet_string(0, "*****");
		}
	}

	void disp_cal(float response)
	{
		char strng[12];
        short ch_num = current.main_chamber;

        if(chamber[ch_num].calkey)
        {
            strncpy(strng,&chamber[ch_num].calstrng[0],7);
            strng[7] = '\0';
        }
        else
            cal_from_response(strng,response);


        erase_text_line(56,SMALL,0);
        //display_text(0,56,"Cal #:",0,SMALL,NORMAL);
        display_small_message(CAL_1,56,0,NORMAL);
		//cal_from_response(strng,response);
		display_cal(strng,42,56,SMALL,0);
	}


	void display_cal(char *strng, short xpos0, short ypos, short size, short plane)
	{
		short i;
		char ch;
        short xpos;
        char sOutput[13];

        xpos = xpos0;

		for (i = 0; i < 8; i++)
		{
			switch(strng[i])
			{
			case DIVSIGN:
				if (size == SMALL)
                    write_special((char *)&sdiv, xpos,ypos,SMALL,plane);

				else
				{
					ch = MED_DIV;
				    write_special(&ch, xpos, ypos,MEDIUM,plane);
				}
				break;
			case MULT:
				if (size == SMALL)
                    display_text(xpos,ypos,&strng[i],plane,SMALL,NORMAL);
				else
				{
					ch = 'X';
					display_text(xpos,ypos,&ch,plane,MEDIUM,NORMAL);
				}
				break;

			default:
				if (size == SMALL)
                    display_text(xpos,ypos,&strng[i],plane,SMALL,NORMAL);
				else
                    display_text(xpos,ypos,&strng[i],plane,MEDIUM,NORMAL);
			}
            xpos += fonts[size].delx;
		}

		for (i = 0; i < 8; i++) {
			switch(strng[i]) {
				case DIVSIGN:
					sOutput[i] = 0x2F;
					break;

				default:
					sOutput[i] = strng[i];
					break;
			}
		}
		sOutput[8] = 0x00;
		send_to_amulet_string(3, sOutput);
	}


    	/* get calibration number string from response */
	void cal_from_response(char *strng,float response)
	{

		short i;
		short cnum,fac;
		float resp;
		short muldiv;
		short ch_num = current.main_chamber;
		short ch_type;

		ch_type = chamber_type(ch_num);
		for(i = 0; i < 3; i++)
			strng[i] = '0';
		for(i = 3; i < 8; i++)
			strng[i] = ' ';
        strng[8] = '\0';

		resp = response;
		muldiv = 0;
		fac = 1;
        while(response_lt_min(ch_type,resp))
		{
			fac *= 10;
			resp *= 10.;
		}
		if (fac != 1)
			muldiv = 1;

        cnum = calc_cnum(ch_type,resp);
		if (cnum > 999)
		{
			fac = 2;
			resp /= 2.;
            cnum = calc_cnum(ch_type,resp);
			muldiv = 2;
		}

		i = 0;
		if (cnum < 100)
			i = 1;
		if (cnum < 10)
			i = 2;
        sprintf(&strng[i],"%d",cnum);

		if (muldiv == 1)
		{
			strng[3] = '*';
            sprintf(&strng[4],"%d",fac);
		}
		if(muldiv == 2)
		{
			if(cnum < 1000)
			{	
				strng[3] = DIVSIGN;
				strng[4] = '2';
			}
			else
			{	
				strng[4] = DIVSIGN;
				strng[5] = '2';
			}	
		}

	}

void DisplayNucCal_getCalNumFromResponse(float response, short chambertype, char *calnum){
	short i;
	short cnum,fac;
	float resp;
	short muldiv;

	for(i=0; i<3; i++) calnum[i]='0';
	for(i=3; i<8; i++) calnum[i]=' ';
	calnum[8]='\0';

	resp = response;
	muldiv = 0;
	fac = 1;
	while(response_lt_min(chambertype, resp)){
		fac *= 10;
		resp *= 10.;
	}

	if(fac!=1) muldiv = 1;

	cnum=calc_cnum(chambertype, resp);
	if(cnum>999){
		fac = 2;
		resp /= 2.;
		cnum = calc_cnum(chambertype,resp);
		muldiv = 2;
	}

	i = 0;
	if(cnum<100) i=1;
	if(cnum<10) i=2;
	sprintf(&calnum[i], "%d", cnum);

	if(muldiv==1){
		calnum[3]='*';
		sprintf(&calnum[4], "%d", fac);
	}


	if(muldiv==2){
		if(cnum < 1000)
		{	
			calnum[3]=0x2F;
			calnum[4]='2';
		}
		else
		{
			calnum[4] = 0x2f;
			calnum[5] = '2';
		}	
	}
/*	if(muldiv==2){
 - 		calnum[3]=0x2F;
 - 		calnum[4]='2';
 - 	}*/
}

    extern REMOTE remote[];
    void display_chamber(void)
    {
        char str[2];
        short ch_num = current.main_chamber;


        //display * to show that remote is in system
        if(remote[ch_num].exists) {
            display_text(120,56,"*",0,SMALL,NORMAL);
            send_to_amulet_string(4, "*");
        }

        //finished if only 1 chamber in system
        if(current.num_chambers == 1)
            return;

        //display chamber number
        str[0] = (char)(ch_num + 1) + '0';
        str[1] = '\0';

        display_text(110,52,str,0,MEDIUM,REV);

    }

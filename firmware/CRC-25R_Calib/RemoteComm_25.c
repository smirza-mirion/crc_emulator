/***********************************************************
  MODULE:  Remote Communications for CRC-25

  FILE:  RemoteComm_25.c

  DATE:  11/28/06

  *****************************************************************/
#include "crc.h"
#include "uart.h"
#include "i2c.h"
#include "chambfac.h"
#include "remote.h"
#include "daily.h"
#include "screen.h"
#include "nuc.h"
#include "amulet.h"
#include <string.h>

#define END_CHAR '#'
#define START_CHAR '$'
#define REM_MULT 'X'
#define REM_DIV 0xfd;

extern MEASUREMENT measurement[];
extern CHAMBER chamber[];
extern REMOTE remote[];
extern ZERODATA zerodata[];
extern DAILY_TEST_RES  daily_res[];
extern CURRENT current;
extern const short max_chambers;
extern volatile bool DisplayActivity_SendActivity;
extern char AmuletRemote_message[30];
extern bool AmuletRemote_valid;

static void remote_time(char *buffer);
static void remote_1(char cmd, char *buffer);
static void remote_2(short ch_num, char *buffer);
static void value_to_buffer(char *str, char *buf);
static void remote_error(short ch_num);
void remote_accept_nuclide(short ch_num);
void remote_next_nuclide(short ch_num);
static void remote_first_acc_source(short ch_num);
static void remote_next_acc_source(short ch_num);
static void remote_acc_act(short ch_num);
static void start_acc_test(short ch_num);
unsigned char BasePageStack(void);
unsigned char CurrentPageStack(void);
void PushPageStack(unsigned char ucPage);

static short heartbeats[8];


    void remote_activity(short ch_num)
    {
        char buffer[30];
        int i;
        char cstr[12];

        short ch_type;

        ch_type = chamber_type(ch_num);
        //if data in remote buffer, do not write over
        if(remote[ch_num].valid_flag)
            return;

        if(remote[ch_num].mode == REMOTE_HEARTBEAT_MODE)
        {
            if(heartbeats[ch_num] == 10)
            {    
                //no remote for this chamber
                remote[ch_num].exists = FALSE;
                //if main chamber, remove *
                if(current.main_chamber == ch_num)
                    display_text(120,56," ",0,SMALL,NORMAL);                   
            }    
            else
            {
                ++heartbeats[ch_num];
                //send again;
                remote[ch_num].valid_flag = TRUE;
            }
            return;
        }    
        
        if(remote[ch_num].mode == REMOTE_NUCLIDE_MODE ||
           remote[ch_num].mode == REMOTE_IN_PROGRESS_MODE ||
           remote[ch_num].mode == REMOTE_ACC_SOURCE_MODE)

        {   
            //send same data
            remote[ch_num].valid_flag = TRUE;
            return;
        }

        if(remote[ch_num].mode == REMOTE_ACC_MODE)
        {    
            remote_acc_act(ch_num);
            return;
        }    
            
        remote_1('M',buffer);
        
        //byte 2 is chamber type, R or P
        if(ch_type == R_CHAMB)
            buffer[2] = 'R';
        else if(ch_type == P_CHAMB)
            buffer[2] = 'P';
        else if(ch_type == B_CHAMB)
            buffer[2] = 'B';
        else if(ch_type == ONE_DOT_EIGHT_CHAMB)
            buffer[2] = '8';
        else if(ch_type == C_CHAMB)
            buffer[2] = 'C';
        else if(ch_type == K_CHAMB)
            buffer[2] = 'K';
        else
        	buffer[2] = 'X';

        if(remote[ch_num].keys_active)
            buffer[3] = '1';    //measurement
        else
            buffer[3] = '3';    //display only
        


        //byte 4 is status: 0 = Good, 1 = Over Range
        buffer[4] = '0';
        if (measurement[ch_num].over_flag)
            buffer[4] = '1';
        
        //bytes 5 - 11 is nuclide name or cal #
        if(chamber[ch_num].calkey)
        {

            strcpy(cstr,&chamber[ch_num].calstrng[0]);
            strncpy(&buffer[5],cstr,strlen(cstr));
            for(i = 7; i <= 11; i++)
            {

                if(buffer[i] == DIVSIGN)
                {    
                    buffer[i] = REM_DIV;
                    break;
                }    
                if(buffer[i] == MULT)
                {    
                    buffer[i] = REM_MULT;
                    break;
                }    
          
            }
        }    
        else
        {    
            strcpy(cstr,&chamber[ch_num].nucdata.name[0]);
            strncpy(&buffer[5],cstr,strlen(cstr));
        }    
       

        //activity if not over range
        if (measurement[ch_num].over_flag)
            strncpy(&buffer[12],"OVER",4);
        else
            //bytes 12 - 20 is activity string with unit
            value_to_buffer(&measurement[ch_num].actstr[0],&buffer[12]);
            
        
        //time, checksum, end character, send to remote structure
        remote_2(ch_num,buffer);

        
    }

   //Zero, or Bkg, or Bias to remote
    void remote_value(short ch_num,char *str, char type,char status)
    {
        char buffer[30];

        remote_1('T',buffer);

        //byte 3 is 1 for Zero, 2 for Bkg, 3 for Bias
        buffer[3] = type;

        //byte 4 is status
        buffer[4] = status;

        //bytes 12 -20 is display string with unit
        value_to_buffer(str,&buffer[12]);
        
        //time, checksum, end character, send to remote structure
        remote_2(ch_num,buffer);
        
    }

    //Zero, or Bkg,Bias in Progress message or Release in progress
    void remote_in_progress(short ch_num,char type)
    {
        char buffer[30];
        
        remote_1('I',buffer);

        //byte 2 is 1 for Zero, 2 for Bkg, 3 for Bias, 4 for Busy , 0 for Release
        buffer[2] = type;

        //time, checksum, end character, send to remote structure
        remote_2(ch_num,buffer);

        if (type == '0')
            remote[ch_num].mode = REMOTE_ACTIVITY_MODE;
        else
            remote[ch_num].mode = REMOTE_IN_PROGRESS_MODE;
        
    }

    //Next nuclide message
    void remote_next_nuclide(short ch_num)
    {
        char buffer[30];
        char nucname[8];
        KEYDEF rem_nucs;
        short rem_index;
        
        short ch_type;

        ch_type = chamber_type(ch_num);
        rem_index = remote[ch_num].nuclist;
        if(rem_index == REMOTE_NUCLIDE_8)
            rem_index = REMOTE_NUCLIDE_OTHER;
        ++rem_index;

        EE_READ(remote_nucs[rem_index],(uchar *)&rem_nucs);
        if(ch_type == C_CHAMB || ch_type == K_CHAMB) strcpy(nucname,&rem_nucs.keyiso[R_CHAMB].iso_name[0]);
        else strcpy(nucname,&rem_nucs.keyiso[ch_type].iso_name[0]);
        if(strlen(nucname) == 0)
        {    
            rem_index = REMOTE_NUCLIDE_1;
            EE_READ(remote_nucs[rem_index],(uchar *)&rem_nucs);
            if(ch_type == C_CHAMB || ch_type == K_CHAMB) strcpy(nucname,&rem_nucs.keyiso[R_CHAMB].iso_name[0]);
            else strcpy(nucname,&rem_nucs.keyiso[ch_type].iso_name[0]);
        }
        remote[ch_num].nuclist = rem_index;
        
        remote_1('N',buffer);

        //bytes 2-7 is nuclide name
        strncpy(&buffer[2],nucname,strlen(nucname));
    
        //time, checksum, end character, send to remote structure
        remote_2(ch_num,buffer);

        //set mode to nuclide mode
        remote[ch_num].mode = REMOTE_NUCLIDE_MODE;


    }    


    char checksum(char *str, short len)    
    {

        uchar cs;
        short i;

        cs = 0;
        for(i = 0; i < len; i++)
        {    
            cs += str[i];
        }

        //confine to printable ascii
        while(cs > 0x7e)
        {    
            cs -= 0x7e;
        }

        //checksum must be > 0x26 (&)
        if(cs < 0x26)
            cs += 0x26;
        
        return(cs);
            
            
    }

    static void remote_1(char cmd, char *buffer)
    {

        int i;
        
        
        for(i = 0; i < 28; i++)
            buffer[i] = ' ';
        buffer[28] = '\0';

        
        //byte  0 is START_CHAR
        buffer[0] = START_CHAR;

        //byte 1 is Command
        buffer[1] = cmd;

    }    

    extern time_t clock_time;
    static void remote_time(char *buffer)
    {
        
        char t_str[6];
        
        timeout(t_str,&clock_time);

        //bytes 21 - 25 is time
        strncpy(&buffer[21],t_str,5);

    }    

//time, checksum, End Character, setting remote structure
    static void remote_2(short ch_num, char *buffer)
    {

        uchar cs;
        
        //time stamp
        remote_time(buffer);

        //byte 26 is checksum
        cs = checksum(&buffer[1],25);
        buffer[26] = cs;
    
        //byte 27 is END CHAR
        buffer[27] = END_CHAR;
        
        remote[ch_num].valid_flag = TRUE;
        strcpy(&remote[ch_num].buffer[0],buffer);
        //printf("%s\n", buffer);
        strcpy(AmuletRemote_message, buffer);
        AmuletRemote_valid = TRUE;
    }

//error condition
    static void remote_error(short ch_num)
    {

        char buffer[30];

        remote_1('E',buffer);

        //time, checksum, end character, send to remote structure
        remote_2(ch_num,buffer);

    }



    //accept nuclide
    void remote_accept_nuclide(short ch_num)
    {
        char nucname[8];
        KEYDEF rem_nucs;
        short rem_index;
        short nuc_index;
        
        short ch_type;

        ch_type = chamber_type(ch_num);
        rem_index = remote[ch_num].nuclist;

        if(rem_index!=REMOTE_NUCLIDE_OTHER){
			EE_READ(remote_nucs[rem_index],(uchar *)&rem_nucs);

			if(ch_type == C_CHAMB || ch_type == K_CHAMB) strcpy(nucname,&rem_nucs.keyiso[R_CHAMB].iso_name[0]);
			else strcpy(nucname,&rem_nucs.keyiso[ch_type].iso_name[0]);

			//nuc_index = NuclideData_getIndexFromName(nucname);
			nuc_index = NuclideData_getIndexFromNameIncludingUser(nucname);
			if(nuc_index<0){
				nuc_index = NuclideData_getIndexFromName(nucname);
			}

			if(nuc_index>=0){
				if(current.main_chamber == ch_num && get_meas_screen())
					erase_screen();

				set_nuclide_data(nuc_index,ch_num);
			}
        }

        //return to activity mode
        remote[ch_num].mode = REMOTE_ACTIVITY_MODE;

    }

    //was test result accepted?
     bool test_accepted(short ch_num, char test_type)
    {


        switch (test_type)
        {
        case TEST_BKG:     //Bkg
            if(remote[ch_num].mode != REMOTE_BKG_MODE)
                return FALSE;
            break;
        case TEST_BIAS:     //Bias    
            if(remote[ch_num].mode != REMOTE_BIAS_MODE)
                return FALSE;
            break;
        case TEST_ZERO:    
            if(remote[ch_num].mode != REMOTE_ZERO_MODE)
                return FALSE;
            break;
        }    

        if(remote[ch_num].sub_cmd == '2')
            return TRUE;
        else
            return FALSE;


    }    


    static void value_to_buffer(char *str, char *buf)
    {

        char actstr[10];
        
        strncpy(actstr,str,9);
        //if microCi, change $ to u
        if(actstr[6] == '$')
            actstr[6] = 'u';
        strncpy(buf,actstr,9);


    }    

extern CURRENT current;
extern ACCDATA acc_data[5];          
    void service_remote_request(void)
    {
        short i;
        char cmd;
        char sub;
        char str[10];

        for(i = 0; i < max_chambers; i++)
        {
            if(!remote[i].exists || remote[i].cmd == REMOTE_NO_CMD)
                continue;

            cmd = remote[i].cmd;
            sub = remote[i].sub_cmd;
            
            //clear command
            remote[i].cmd = REMOTE_NO_CMD;

/*
 -             //testing
 -             str[0] = cmd;
 -             str[1] = ',';
 -             str[2] = sub;
 -             str[3] = '\0';
 -             printf("ch:%d cmd,sub: %s\r\n",i,str);*/
            
            switch(cmd)
            {
            case 'E':
                remote_error(i);
                break;

            case REMOTE_EXISTS:
                remote[i].mode = REMOTE_ACTIVITY_MODE;
                //keys now active
                if(BasePageStack() == AmuletHTMLIndex[MAINSCREEN_HTM]){
                	if(i == current.main_chamber){
                		if(DisplayActivity_SendActivity) remote[i].keys_active = TRUE;
                		else remote[i].keys_active = FALSE;
                	}else{
                		remote[i].keys_active = TRUE;
                	}
                }else{
                	remote[i].keys_active = FALSE;
                }
                break;
                    
            case REMOTE_NEXT_NUCLIDE:
                if(sub == '1')
                    remote_next_nuclide(i);
                else    //accept it
                    remote_accept_nuclide(i);
                break;

            case REMOTE_BKG:   //Background measurement
                if(remote[i].keys_active == FALSE)
                {
                    remote[i].mode = REMOTE_ACTIVITY_MODE;
                    break;
                }    
                
                if(sub == '1')    //start measurement
                {

                    chamber[i].control = CONTROL_REMOTE;
                    if((current.main_chamber == i) && (get_crc_mode() == CRC_CAL))
                    {
                    	if(CurrentPageStack() != AmuletHTMLIndex[REMOTE_HTM]){
                    		SetAmuletHTML(AmuletHTMLIndex[REMOTE_HTM]);
                    		PushPageStack(AmuletHTMLIndex[REMOTE_HTM]);
                    	}
                        remote[i].cmd = REMOTE_NO_CMD;
                        measure_bkg(-1);
                        remote[i].valid_flag = FALSE;
                        set_pc_start_flag();
                        return;
                    }
                    else
                        setup_for_background(i);
                }    
                else  //test accepted
                {    
                    //save in daily_res
                    format_bkg(i,str);
                    strcpy(&daily_res[i].bkg[0],str);
                    
                    if((current.main_chamber == i) && (get_crc_mode() == CRC_CAL))
                        return;
                    else
                    {   
                        chamber[i].bkg += measurement[i].meas;
                        measurement[i].mode = MEASMODE;
                    }    
                }    
                break;
                
            case REMOTE_BIAS:   //Bias measurement
                if(remote[i].keys_active == FALSE)
                {
                    remote[i].mode = REMOTE_ACTIVITY_MODE;
                    break;
                }    
                
                if(sub == '1')    //start measurement
                {
                    chamber[i].control = CONTROL_REMOTE;
                    if((current.main_chamber == i) && (get_crc_mode() == CRC_CAL))
                    {
                    	if(CurrentPageStack() != AmuletHTMLIndex[REMOTE_HTM]){
                    		SetAmuletHTML(AmuletHTMLIndex[REMOTE_HTM]);
							PushPageStack(AmuletHTMLIndex[REMOTE_HTM]);
                    	}
                        remote[i].cmd = REMOTE_NO_CMD;
                        measure_bias(-1);
                        remote[i].valid_flag = FALSE;
                        set_pc_start_flag();
                        return;
                    }   
                        
                    else{
                        setup_for_bias(i);
                        setchamber(i, measurement[i].gain);
                    }
                }    
                else  //test accepted
                {    
                    if((current.main_chamber == i) && (get_crc_mode() == CRC_CAL))
                        return;
                    else
                    {   
                        measurement[i].dogain = 1;
                        measurement[i].mode = MEASMODE;
                        setchamber(i,measurement[i].gain);
                    }    
                }
                break;

            case REMOTE_ZERO:   //Zero measurement
                if(remote[i].keys_active == FALSE)
                {
                    remote[i].mode = REMOTE_ACTIVITY_MODE;
                    break;
                }    
                
                chamber[i].control = CONTROL_REMOTE;
                if(sub == '1')    //start measurement
                {
                    //setup daily_res
                    start_daily_res(i);
                    if((current.main_chamber == i) && (get_crc_mode() == CRC_CAL))
                    {
                    	if(CurrentPageStack() != AmuletHTMLIndex[REMOTE_HTM]){
                    		SetAmuletHTML(AmuletHTMLIndex[REMOTE_HTM]);
							PushPageStack(AmuletHTMLIndex[REMOTE_HTM]);
                    	}
                        measure_zero_offset(FALSE);
                        remote[i].valid_flag = FALSE;
                        set_pc_start_flag();
                        return;
                    }   
                    else
                        setup_for_zero_adc(i);
                }    
                else  //test accepted
                {    
                    //save zero in daily_res
                    sprintf(&daily_res[i].zero[0],"%#5.2f",zerodata[i].zerodisp);
                    if((current.main_chamber == i) && (get_crc_mode() == CRC_CAL))
                        return;
                    
                    else
                    {   
                        measurement[i].dogain = 1;
                        measurement[i].dogain = 1;                    
                        chamber[i].zero = zerodata[i].znew;
                        //exit zeromode
                        setchamber(i,0);
                        measurement[i].mode = MEASMODE;
                    }    
                }    
                break;

            case REMOTE_ACCURACY:
                if(remote[i].keys_active == FALSE)
                {
                    remote[i].mode = REMOTE_ACTIVITY_MODE;
                    break;
                }    
                
                chamber[i].control = CONTROL_REMOTE;
                switch(sub)
                {
                case '1':  //first source
                    remote_first_acc_source(i);
                    break;
                case '2':  //measure
                    start_acc_test(i);
                    break;
                case '3':  //accept measurement
                    //save test results
                    save_acc_to_daily_res(i);
                    //return to activity mode
                    remote[i].mode = REMOTE_ACTIVITY_MODE;
                    break;
                case '4':  //next source
                    remote_next_acc_source(i);
                    break;
                case '5':   //accept source
                    start_acc_test(i);
                    break;
                }
                break;

            default:
                break;      

            }       

            //reset command to none
            remote[i].cmd = REMOTE_NO_CMD;

        }    



    }   

    void remote_busy(bool flag)
    {

        short i;
        char type;

        if(flag)
            type = TEST_BUSY;
        else
            type = TEST_CLEAR;

        type += '0';

        for(i = 0; i < max_chambers; i++)
        {
            if(remote[i].exists && remote[i].mode != -1)    //don't send if still in Heartbeat mode
                remote_in_progress(i,type);

        }    
        


    }    

    //first accuracy test source
    static void remote_first_acc_source(short ch_num)
    {
        char buffer[30];
        char nucname[8];
        char sn[11];
        short len;

        get_acc_data(0,FALSE,ch_num);

        
        remote_1('F',buffer);

        //bytes 2-7 is nuclide name
        strcpy(nucname,&acc_data[0].nucname[0]);
        len = strlen(nucname);

        if(len != 0)
        {    
            strncpy(&buffer[2],nucname,len);
    
            //bytes 8 - 17 is serial number
            strcpy(sn,&acc_data[0].sn[0]);
            strncpy(&buffer[8],sn,strlen(sn));
        }
        
        //time, checksum, end character, send to remote structure
        remote_2(ch_num,buffer);

        //set mode to accuracy test source mode
        remote[ch_num].mode = REMOTE_ACC_SOURCE_MODE;

        //set to first accuracy test
        remote[ch_num].acc_num = REMOTE_ACCURACY_TEST1;


    }

    //next accuracy test source
    static void remote_next_acc_source(short ch_num)
    {
        char buffer[30];
        char nucname[8];
        char sn[11];
        short len;
        short nacc;

        nacc = remote[ch_num].acc_num;
        ++nacc;
        if(nacc == 5)
            nacc = 0;

        //if no source, go back to first source
        if(strlen(&acc_data[nacc].nucname[0]) == 0)
           nacc = 0;
        
        remote_1('F',buffer);

        //bytes 2-7 is nuclide name
        strcpy(nucname,&acc_data[nacc].nucname[0]);
        len = strlen(nucname);

        if(len != 0)
        {    
            strncpy(&buffer[2],nucname,len);
    
            //bytes 8 - 17 is serial number
            strcpy(sn,&acc_data[nacc].sn[0]);
            strncpy(&buffer[8],sn,strlen(sn));
        }
        
        //time, checksum, end character, send to remote structure
        remote_2(ch_num,buffer);

        //set mode to accuracy test source mode
        remote[ch_num].mode = REMOTE_ACC_SOURCE_MODE;

        //set accuracy test number
        remote[ch_num].acc_num = nacc;


    }
    
    extern const char *unit_str[];
    
    static void remote_acc_act(short ch_num)
    {
        short nacc;
        char buffer[30];
        short nc;
        char str[10];

        nacc = remote[ch_num].acc_num;
        calc_dev(nacc,ch_num);
        
        remote_1('A',buffer);

        //status
        nc = acc_data[nacc].test_res[ch_num].nc;
        if (nc == -1)
            buffer[4] = '1';    //error
        else
            buffer[4] = '0';    //good

        //activity
        strncpy(&buffer[5],&acc_data[nacc].test_res[ch_num].msact[0],6);
        strncpy(&buffer[11],unit_str[acc_data[nacc].test_res[ch_num].ms_kun - 1],3);
        
        //deviation
        if(nc != -1)
        {    
            strcpy(str,&acc_data[nacc].test_res[ch_num].var[0]);
            strcat(str,"%");
            strncpy(&buffer[14],str,nc + 1);
        }    

        //time, checksum, end character, send to remote structure
        remote_2(ch_num,buffer);

    }    


    static void start_acc_test(short ch_num)
    {
        short nuc_index;
        short nacc;

        nacc = remote[ch_num].acc_num;
        
        nuc_index = acc_data[nacc].nuc_index;
        set_nuclide_data(nuc_index,ch_num);
        disp_nuclide();

        remote[ch_num].mode = REMOTE_ACC_MODE;


    }    

    //setup to send heartbeat to see if remote exists
    void remote_heartbeat(short ch_num)
    {

        char buffer[30];

        remote[ch_num].mode = REMOTE_HEARTBEAT_MODE;
        heartbeats[ch_num] = 0;
        
        remote_1('H',buffer);
        buffer[2] = ch_num + '0';

        remote_2(ch_num,buffer);

        
    }

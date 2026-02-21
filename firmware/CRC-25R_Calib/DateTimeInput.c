/**************************************************************
  MODULE:  Date and Time Input

  FILE:  DateTimeInput.c

  DATE:  12/22/06

  ***************************************************************/

#include "crc.h"
#include "screen.h"
#include "i2c.h"
#include "keyboard.h"
#include "message.h"
#include "remote.h"
#include <stdlib.h>  //for atof
#include <string.h>
#include <stdio.h>

bool test_date(short mon, short day, short year);
static void error_msg(short xpos, short ypos,bool date_flag);
static void out_date(char *datestr, struct tm *date, short year_flag);
void disp_date_time(struct tm *dt);

extern time_t clock_time;
extern time_t low_clock_time;

    void set_date_time(void)
    {
        time_t now;
        struct tm dt;
        bool ok;
        bool yn;


        remote_busy(TRUE);

        erase_screen();
        read_clock(&now);
        memcpy(&dt,gmtime(&now),sizeof(dt));
        disp_date_time(&dt);
        yn = yesorno();
        if(home_set())
        {
            remote_busy(FALSE);
            return;
        }
        if(yn)
        {
            erase_screen();
            remote_busy(FALSE);
            return;
        }

        do
        {
            erase_screen();
            //Enter Date
            display_medium_message(DATE_TIME_IN_1,10,0,NORMAL);
            yn = date_in(32,22,&dt);
            if(!yn || home_set())
            {
                remote_busy(FALSE);
                return;
            }
            erase_screen();
            //Enter Time
            display_medium_message(DATE_TIME_IN_2,10,0,NORMAL);
            yn = time_in(32,22,&dt);
            if(!yn || home_set())
            {
                remote_busy(FALSE);
                return;
            }



            erase_screen();
            disp_date_time(&dt);
            ok = yesorno();
            if(home_set())
            {
                remote_busy(FALSE);
                return;
            }
        }while(!ok);


        ok = set_clock(&dt);
        erase_screen();

        //save new date/time in clock_time
        //clock_time = mktime(&dt);
        read_clock(&clock_time);
        low_clock_time = clock_time;
        //reset_minute_counter();
        reset_minute_counter_with_seconds();

        remote_busy(FALSE);


    }


    bool date_in(short xpos, short ypos, struct tm *dt)
    {
        bool ok;
        char str[10];
        short length;
        short mon,day,year;
        char str1[6];

        for(;;)
        {
            //display_text(30,56,"MMDDYYYY",0,SMALL,NORMAL);
            display_small_message(DATE_TIME_IN_6,56,0,NORMAL);

            ok = numstr_in(xpos, ypos,str,MEDIUM,0,8,12,TRUE,FALSE);
            if (!ok || home_set())
                return FALSE;

            length = strlen(str);
            if(length != 8)
            {
                error_msg(xpos,ypos,TRUE);
                if(home_set())
                    return FALSE;
                continue;
            }

            strncpy(str1,&str[0],2);
            str1[2] = '\0';
            mon = atoi(str1);

            strncpy(str1,&str[2],2);
            str1[2] = '\0';
            day = atoi(str1);

            strncpy(str1,&str[4],4);
            str1[4] = '\0';
            year = atoi(str1);

            if(test_date(mon,day,year))
                break;

            error_msg(xpos,ypos,TRUE);
            if(home_set())
                return FALSE;
        }

        dt -> tm_mon = mon - 1;
        dt -> tm_mday = day;
        dt -> tm_year = year - 1900;
        return TRUE;

    }
    extern const short month_days[];
    bool test_date(short mon, short day, short year)
    {
        short mval[13];


        /* change # of days in February according to leap year */
        memcpy(mval,month_days,sizeof(mval));

        mval[2] = 28;
        if (year % 4 == 0)
            mval[2] = 29;

        if(mon > 12 || mon < 1 || day < 1 || day > mval[mon] ||
           year < 1970 || year > 2037)
            return FALSE;


        return TRUE;

    }

    bool test_date_julian(short mon, short day, short year)
    {
        short mval[13];


        /* change # of days in February according to leap year */
        memcpy(mval,month_days,sizeof(mval));

        mval[2] = 28;
        if (year % 4 == 0)
            mval[2] = 29;

        if(mon > 12 || mon < 1 || day < 1 || day > mval[mon] ||
           year < 1900 || year > 2100)
            return FALSE;


        return TRUE;

    }


    bool time_in(short xpos, short ypos, struct tm *dt)
    {
        bool ok;
        char str[16];
        short length;
        short hour,min;
        int dtime;

        //default
        dt -> tm_hour = 12;
        dt -> tm_min = 00;
        dt -> tm_sec = 0;


        for(;;)
        {
            //display_text(30,56,"hhmm",0,SMALL,NORMAL);
            display_small_message(DATE_TIME_IN_7,56,0,NORMAL);

            ok = numstr_in(xpos,ypos,str,MEDIUM,0,4,12,TRUE,FALSE);
            if (!ok)
                return FALSE;

            length = strlen(str);
            if(length < 3)
            {
                error_msg(xpos,ypos,FALSE);
                continue;
            }

            dtime = atoi(str);
            hour = dtime / 100;
            min = dtime - 100 * hour;

            if(hour <= 23 && min <= 59)
                break;

            error_msg(xpos,ypos,FALSE);
        }

        dt -> tm_hour = hour;
        dt -> tm_min = min;

        return TRUE;


    }

    extern FONT fonts[];
    static void error_msg(short xpos, short ypos,bool date_flag)
    {
        short xpos1;
        short ypos1;

        beep();
        if(date_flag)
            //Date Error
            display_small_message(DATE_TIME_IN_3,40,0,NORMAL);
        else
            //Time Error
            display_small_message(DATE_TIME_IN_4,40,0,NORMAL);
        contmsg();
        xpos1 = xpos + 12 * fonts[MEDIUM].delx;
        ypos1 = ypos + fonts[MEDIUM].dely;
        erase_box(xpos,ypos,xpos1,ypos1,0);
        erase_lines(40,24,0);

    }

    void disp_date_time(struct tm *dt)
    {
        char str[14];
        char tstr[4];

        out_date(str,dt,4);
        display_text(16,2,str,0,MEDIUM,NORMAL);

        strcpy(str,"  :  ");
        sprintf(tstr,"%02d",dt -> tm_hour);
        strncpy(&str[0],tstr,2);
        sprintf(tstr,"%02d",dt -> tm_min);
        strncpy(&str[3],tstr,2);
        display_text(16,14,str,0,MEDIUM,NORMAL);

        //Correct ?
        //Yes or No
        display_medium_message(DATE_TIME_IN_5,42,0,NORMAL);
        display_medium_message(YES_OR_NO,52,0,NORMAL);

    }


    //decode time date entered into keybuf from measurement screen
    //time, day, month, year separated by slashes entered via UP or DOWN keys
    bool decode_time_date(bool meas_flag, struct tm *dt)
    {
        int i;
        char dtstr[18];
        short hour,min,day,mon,year;
        int dtime;
        short num;
        short num_slashes;
        short slashes[4];
        short index;
        char ch;
        time_t nowtime;
        char str[6];
        short numch;
        char *where;

        //set default to now
        read_clock(&nowtime);
        memcpy(dt,gmtime(&nowtime),sizeof(*dt));

        if(meas_flag)
        {
            //get data in keybuf
            get_keybuf(dtstr,0,14);
            num = strlen(dtstr);
            if(num == 0)
                return FALSE;
        }
        else
        {
            num = time_date_input(8,30,dtstr,MEDIUM,0);
            if(num == 0) //default of NOW
                return TRUE;
        }

        year = dt -> tm_year + 1900;
        day = dt -> tm_mday;
        mon = dt -> tm_mon + 1;

        //get number of slashes
        i = 0;
        num_slashes = 0;
        for(;;)
        {
            ch = '/';
            where = strchr(&dtstr[i],(int)ch);
            if(where == NULL)   //not found
            {
                //add slash to end of string
                dtstr[num] = '/';
                dtstr[num + 1] = '\0';
                slashes[num_slashes] = num;
                ++num_slashes;
                break;
            }
            //found, get index
            index = where - dtstr;
            //save index of slashes & increment # of slashes
            slashes[num_slashes] = index;
            ++num_slashes;

            //if it was the last character of string, all found
            if(index == (num - 1))
                break;

            //start next search at character after slash
            i = index + 1;
        }

        //first field is time
        numch = slashes[0];
        if(numch < 3 || numch > 4)
        {
            beep();
            return FALSE;
        }
        strncpy(str,&dtstr[0],numch);
        str[numch] = '\0';

        dtime = atoi(str);
        hour = dtime / 100;
        min = dtime - 100 * hour;

       if(hour > 23 || min > 59)
        {
            beep();
            return FALSE;
        }
        dt -> tm_hour = hour;
        dt -> tm_min = min;

        if(num_slashes == 1)
            return TRUE;

        //second field is day
        numch = slashes[1] - slashes[0] - 1;
        if(numch == 0)
        {
            beep();
            return FALSE;
        }

        index = slashes[0] + 1;
        strncpy(str,&dtstr[index],numch);
        str[numch] = '\0';
        day = atoi(str);

        if(num_slashes > 2)
        {
        //3rd field is month
            numch = slashes[2] - slashes[1] - 1;
            if(numch == 0)
            {
                beep();
                return FALSE;
            }
            index = slashes[1] + 1;
            strncpy(str,&dtstr[index],numch);
            str[numch] = '\0';
            mon = atoi(str);

            if(num_slashes > 3)
            {
            //4th field is 2 digit year (21st century) or 4 digit year (20 or 21st century)
                numch = slashes[3] - slashes[2] - 1;
                if(!(numch == 2 || numch == 4))
                {
                    beep();
                    return FALSE;
                }

                index = slashes[2] + 1;
                strncpy(str,&dtstr[index],numch);
                str[numch] = '\0';
                if(numch == 4)
                    year = atoi(str);
                else
                    year = atoi(str) + 2000;

            }
        }
        if(!test_date(mon,day,year))
            return FALSE;

        dt -> tm_mon = mon - 1;
        dt -> tm_mday = day;
        dt -> tm_year = year - 1900;

        return TRUE;


    }


    /* gets MON DD from tm structure
        gets year according to year_flag
        places in string datestr
        time not set */
    static void out_date(char *datestr, struct tm *date, short year_flag)
    {
        char ascstr[26];
        struct tm temptm;

        memcpy(&temptm, date, sizeof(temptm));

        //give default value to time
        temptm.tm_min = temptm.tm_hour = temptm.tm_sec =
            temptm.tm_wday = 0;

        strcpy(ascstr,asctime(&temptm));
        strncpy(datestr,&ascstr[4],7);
        strncpy(&datestr[7],&ascstr[24 - year_flag],year_flag);
        datestr[7 + year_flag] = '\0';


    }


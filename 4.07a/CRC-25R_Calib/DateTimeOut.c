/**************************************************************
  MODULE:  Date and Time Output

  FILE:  DateTimeOut.c

  DATE:  05/24/06

  ***************************************************************/

#include "crc.h"
#include "sqlite3.h"
#include "message.h"
#include <string.h>

extern CURRENT current;

typedef struct DateTime DateTime;
struct DateTime {
  sqlite3_int64 iJD; /* The julian day number times 86400000 */
  int Y, M, D;       /* Year, month, and day */
  int h, m;          /* Hour and minutes */
  int tz;            /* Timezone offset in minutes */
  double s;          /* Seconds */
  char validYMD;     /* True (1) if Y,M,D are valid */
  char validHMS;     /* True (1) if h,m,s are valid */
  char validJD;      /* True (1) if iJD is valid */
  char validTZ;      /* True (1) if tz is valid */
};
void computeYMD_HMS(DateTime *p);

/*  gets date from time_t value
    MON DD
    does year according to year_flag  (2 or 4)
    places in string datestr */
void dateout(char *datestr, time_t *timet, short year_flag){
	char ascstr[26];
	char Month[4], Day[3], Year[5];
	struct tm tmt;

	gmtime_r(timet, &tmt);
	strcpy(ascstr,asctime(&tmt));

	strncpy(Month, &ascstr[4], 3);
	Month[3] = 0;
	strncpy(Day, &ascstr[8], 2);
	Day[2] = 0;
 	strncpy(Year, &ascstr[24 - year_flag], year_flag);
 	Year[year_flag] = 0;

	*datestr = 0;

	switch(current.time_format){
		case 0:
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;

		case 1:
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;

		case 2:
			strcat(datestr, Year);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			break;

		default:
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;
	}

	//strncpy(datestr,&ascstr[4],7);	  //"Mon DD "
	//strncpy(&datestr[7],&ascstr[24 - year_flag],year_flag);
	//datestr[7 + year_flag] = '\0';
}

void dateout_language(char *datestr, time_t *timet, short year_flag){
	char Month[4], Day[3], Year[5];
	struct tm tmt;

	gmtime_r(timet, &tmt);

	switch(tmt.tm_mon){
		case 0:
			get_amulet_message(L_JAN, Month);    // "Jan"
			break;

		case 1:
			get_amulet_message(L_FEB, Month);    // "Feb"
			break;

		case 2:
			get_amulet_message(L_MAR, Month);    // "Mar"
			break;

		case 3:
			get_amulet_message(L_APR, Month);    // "Apr"
			break;

		case 4:
			get_amulet_message(L_MAY, Month);    // "May"
			break;

		case 5:
			get_amulet_message(L_JUN, Month);    // "Jun"
			break;

		case 6:
			get_amulet_message(L_JUL, Month);    // "Jul"
			break;

		case 7:
			get_amulet_message(L_AUG, Month);    // "Aug"
			break;

		case 8:
			get_amulet_message(L_SEP, Month);    // "Sep"
			break;

		case 9:
			get_amulet_message(L_OCT, Month);    // "Oct"
			break;

		case 10:
			get_amulet_message(L_NOV, Month);    // "Nov"
			break;

		case 11:
			get_amulet_message(L_DEC, Month);    // "Dec"
			break;
	}

	sprintf(Day, "%02d", tmt.tm_mday);

	sprintf(Year, "%d", tmt.tm_year + 1900);

	if(year_flag == 2){
		Year[0] = Year[2];
		Year[1] = Year[3];
		Year[2] = 0;
	}

	*datestr = 0;

	switch(current.time_format){
		case 0:
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;

		case 1:
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;

		case 2:
			strcat(datestr, Year);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			break;

		default:
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;
	}
}

void dateout_julian(char *datestr, double julian){
	char Month[4], Day[3], Year[5];
	DateTime datetime;

	julian *= 86400000.0;
	datetime.iJD = julian;
	datetime.Y = 0;
	datetime.M = 0;
	datetime.D = 0;
	datetime.h = 0;
	datetime.m = 0;
	datetime.s = 0;
	datetime.validJD = 1;
	datetime.validYMD = 0;
	datetime.validHMS = 0;
	datetime.validTZ = 0;
	computeYMD_HMS(&datetime);

	switch(datetime.M){
		case 1:
			strcpy(Month, "Jan");
			break;

		case 2:
			strcpy(Month, "Feb");
			break;

		case 3:
			strcpy(Month, "Mar");
			break;

		case 4:
			strcpy(Month, "Apr");
			break;

		case 5:
			strcpy(Month, "May");
			break;

		case 6:
			strcpy(Month, "Jun");
			break;

		case 7:
			strcpy(Month, "Jul");
			break;

		case 8:
			strcpy(Month, "Aug");
			break;

		case 9:
			strcpy(Month, "Sep");
			break;

		case 10:
			strcpy(Month, "Oct");
			break;

		case 11:
			strcpy(Month, "Nov");
			break;

		case 12:
			strcpy(Month, "Dec");
			break;
	}

	sprintf(Day, "%02d", datetime.D);
	sprintf(Year, "%d", datetime.Y);

	*datestr = 0;
	switch(current.time_format){
		case 0:
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;

		case 1:
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;

		case 2:
			strcat(datestr, Year);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			break;

		default:
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;
	}
}

void dateout_julian_language(char *datestr, double julian){
	char Month[4], Day[3], Year[5];
	DateTime datetime;

	julian *= 86400000.0;
	datetime.iJD = julian;
	datetime.Y = 0;
	datetime.M = 0;
	datetime.D = 0;
	datetime.h = 0;
	datetime.m = 0;
	datetime.s = 0;
	datetime.validJD = 1;
	datetime.validYMD = 0;
	datetime.validHMS = 0;
	datetime.validTZ = 0;
	computeYMD_HMS(&datetime);

	switch(datetime.M){
		case 1:
			get_amulet_message(L_JAN, Month);    // "Jan"
			break;

		case 2:
			get_amulet_message(L_FEB, Month);    // "Feb"
			break;

		case 3:
			get_amulet_message(L_MAR, Month);    // "Mar"
			break;

		case 4:
			get_amulet_message(L_APR, Month);    // "Apr"
			break;

		case 5:
			get_amulet_message(L_MAY, Month);    // "May"
			break;

		case 6:
			get_amulet_message(L_JUN, Month);    // "Jun"
			break;

		case 7:
			get_amulet_message(L_JUL, Month);    // "Jul"
			break;

		case 8:
			get_amulet_message(L_AUG, Month);    // "Aug"
			break;

		case 9:
			get_amulet_message(L_SEP, Month);    // "Sep"
			break;

		case 10:
			get_amulet_message(L_OCT, Month);    // "Oct"
			break;

		case 11:
			get_amulet_message(L_NOV, Month);    // "Nov"
			break;

		case 12:
			get_amulet_message(L_DEC, Month);    // "Dec"
			break;
	}

	sprintf(Day, "%02d", datetime.D);
	sprintf(Year, "%d", datetime.Y);

	*datestr = 0;
	switch(current.time_format){
		case 0:
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;

		case 1:
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;

		case 2:
			strcat(datestr, Year);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			break;

		default:
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;
	}
}

     /* gets time from time_t value
		places in string timestr */
void timeout(char *timestr, time_t *timet){
	char ascstr[26];
	struct tm tmt;

	gmtime_r(timet, &tmt);
	strcpy(ascstr,asctime(&tmt));
        
	strncpy(timestr,&ascstr[11],5);
	timestr[5] = '\0';
}

/* gets DAY MON DD,YY from time_t
       used for printing measurement information */
void daytostr_language(char *datestr, time_t *timet){
	char WDay[4], Month[4], Day[3], Year[5];
	struct tm tmt;

	gmtime_r(timet, &tmt);

	switch(tmt.tm_wday){
		case 0:
			get_amulet_message(L_SUN, WDay);    // "Sun"
			break;

		case 1:
			get_amulet_message(L_MON, WDay);    // "Mon"
			break;

		case 2:
			get_amulet_message(L_TUE, WDay);    // "Tue"
			break;

		case 3:
			get_amulet_message(L_WED, WDay);    // "Wed"
			break;

		case 4:
			get_amulet_message(L_THU, WDay);    // "Thu"
			break;

		case 5:
			get_amulet_message(L_FRI, WDay);    // "Fri"
			break;

		case 6:
			get_amulet_message(L_SAT, WDay);    // "Sat"
			break;
	}

	switch(tmt.tm_mon){
		case 0:
			get_amulet_message(L_JAN, Month);    // "Jan"
			break;

		case 1:
			get_amulet_message(L_FEB, Month);    // "Feb"
			break;

		case 2:
			get_amulet_message(L_MAR, Month);    // "Mar"
			break;

		case 3:
			get_amulet_message(L_APR, Month);    // "Apr"
			break;

		case 4:
			get_amulet_message(L_MAY, Month);    // "May"
			break;

		case 5:
			get_amulet_message(L_JUN, Month);    // "Jun"
			break;

		case 6:
			get_amulet_message(L_JUL, Month);    // "Jul"
			break;

		case 7:
			get_amulet_message(L_AUG, Month);    // "Aug"
			break;

		case 8:
			get_amulet_message(L_SEP, Month);    // "Sep"
			break;

		case 9:
			get_amulet_message(L_OCT, Month);    // "Oct"
			break;

		case 10:
			get_amulet_message(L_NOV, Month);    // "Nov"
			break;

		case 11:
			get_amulet_message(L_DEC, Month);    // "Dec"
			break;
	}

	sprintf(Day, "%02d", tmt.tm_mday);
	sprintf(Year, "%d", tmt.tm_year + 1900);
	Year[0] = Year[2];
	Year[1] = Year[3];
	Year[2] = 0;

	*datestr = 0;
	switch(current.time_format){
		case 0:
			strcat(datestr, WDay);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;

		case 1:
			strcat(datestr, WDay);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;

		case 2:
			strcat(datestr, WDay);
			strcat(datestr, " ");
			strcat(datestr, Year);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			break;

		default:
			strcat(datestr, WDay);
			strcat(datestr, " ");
			strcat(datestr, Month);
			strcat(datestr, " ");
			strcat(datestr, Day);
			strcat(datestr, " ");
			strcat(datestr, Year);
			break;
	}
}

void stringout(char *output, time_t *timet){
	struct tm tmt;

	gmtime_r(timet, &tmt);
	sprintf(output, "%d-%02d-%02d %02d:%02d:%02d", tmt.tm_year + 1900, tmt.tm_mon + 1, tmt.tm_mday, tmt.tm_hour, tmt.tm_min, tmt.tm_sec);
}

void timeoutsec(char *timestr, time_t *timet){
	char ascstr[26];
	struct tm tmt;

	gmtime_r(timet, &tmt);
	strcpy(ascstr,asctime(&tmt));

	strncpy(timestr,&ascstr[11],8);
	timestr[8] = '\0';
}

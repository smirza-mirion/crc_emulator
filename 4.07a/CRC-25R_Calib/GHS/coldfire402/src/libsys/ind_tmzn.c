/*
		    Low Level Interface Library

            Copyright 1983-2000 Green Hills Software,Inc.

 *  This program is the property of Green Hills Software, Inc,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software, Inc., or as agreed in writing signed by the President
 *  of Green Hills Software, Inc.
 *
 * The routines in this file all deal with the concept of the local timezone.
 *
 * On Unix, times are always kept in terms of Greenwich Mean Time and adjusted
 * according to the local timezone when displayed.
 *
 * Entry points are:
 *	localtime()
 *	tzset()
 *	__gh_timezone()
*/
/* ind_tmzn.c: ANSI time zone facilities, and internal library primitive. */

#include "indos.h"
#include "ind_thrd.h"

#if defined(CROSSUNIX) || !defined(ANYUNIX)

/******************************************************************************/
/*  #include <time.h>							      */
/*  struct tm *localtime(const time_t *timer);				      */
/*									      */
/*  localtime returns a structure containing the current local time broken    */
/*  down into tm_year (current year - 1900), tm_mon (current month January=0) */
/*  tm_mday (current day of month), tm_hour (current hour 24 hour time),      */
/*  tm_min (current minute), and tm_sec (current second).                     */
/*									      */
/*  Uses __gh_timezone to determine the current timezone.		      */
/*									      */
/*  timer is a pointer to a long containing the number of seconds since the */
/*  Epoch (as set by time()).						      */
/*									      */
/*  Return 0 if no time of day can be returned.				      */
/******************************************************************************/

/*  Wed Jul 16 12:06:49 PDT 1997 [enj][odin]
    Added a VxWorks-like reentrant version of localtime.
    The new version (localtime_r) takes another argument that
    points to the resulting tm structure.
*/

extern struct tm *gmtime_r(const time_t *timer, struct tm *result);

/* New Code */
struct tm *localtime_r(const time_t *timer, struct tm *result) {
/*
    If no other implementation provided, assume Epoch is 00:00:00 January 1,1970
    as is true in UNIX.

    There should be code in here to calculate daylight savings time in different
    ways than the one given here.  Which is true for USA as of 1987.

    The rules I follow are:
	before 1987
	    at 2am of the last sunday of april dst starts
	    at 2am of the last sunday of october dst ends
	1987 and after
	    at 2am of the first sunday of april dst starts
	    at 2am of the last sunday of october dst ends
	I shall not concern myself with the vargaries of dst in the 70s
*/
    time_t time = *timer - 60 * __gh_timezone();
    register struct tm *temp;
    register int isdst=0, sn_mday;

    if (*timer == (time_t)-1)
	return(NULL);
    if ((temp = gmtime_r(&time, result))==NULL)
	return(NULL);
    if (temp->tm_mon<=2 || temp->tm_mon>=10)/*March or before or Nov or after */
	isdst=0;
    else if ( temp->tm_mon!=3 && temp->tm_mon!=9 )  /* between may and Sept */
	isdst=1;
    else if ( temp->tm_mon==9 ) {                   /* october */
	sn_mday=temp->tm_mday-temp->tm_wday;
	while ( sn_mday+7 <=31 ) sn_mday +=7;
	if ( temp->tm_mday<sn_mday )
	    isdst=1;
	else if ( temp->tm_mday==sn_mday && temp->tm_hour<2 )
	    isdst=1;
    } else if ( temp->tm_year<87 ) {                /* april, before 87 */
	sn_mday=temp->tm_mday-temp->tm_wday;
	while ( sn_mday+7 <=31 ) sn_mday +=7;
	if ( temp->tm_mday>sn_mday )
	    isdst=1;
	else if ( temp->tm_mday==sn_mday && temp->tm_hour>=2 )
	    isdst=1;
    } else {                                        /* april, after 87 */
	if ((temp->tm_mday > temp->tm_wday) &&	/* not before first sunday */
	    (temp->tm_wday|| temp->tm_hour>=2))	/* after sunday, or after 2 am */
	    isdst=1;
    }

    temp->tm_isdst = isdst;
    if ( isdst ) {
	if ( ++(temp->tm_hour)==24 ) {
	    static const char mons[]={31,28,31,30,31,30,31,31,30,31,30,31};
	    temp->tm_hour=0;
	    ++(temp->tm_yday);          /* can't overflow dst not on Dec. 31 */
	    if ( ++(temp->tm_wday)==7 ) temp->tm_wday=0;
	    if ( ++(temp->tm_mday)>mons[temp->tm_mon] ) {
		temp->tm_mday=1;
		++(temp->tm_mon);       /* can't overflow dst not on Dec. 31 */
	    }
	}
    }
    return( temp );
}

struct tm *localtime(const time_t *timer) {
    return(localtime_r(timer, &GetThreadLocalStorage()->gmtime_temp));
}
/* End New Code */
/* Old Code
struct tm *localtime(const time_t *timer) {
[*
    If no other implementation provided, assume Epoch is 00:00:00 January 1,1970
    as is true in UNIX.

    There should be code in here to calculate daylight savings time in different
    ways than the one given here.  Which is true for USA as of 1987.

    The rules I follow are:
	before 1987
	    at 2am of the last sunday of april dst starts
	    at 2am of the last sunday of october dst ends
	1987 and after
	    at 2am of the first sunday of april dst starts
	    at 2am of the last sunday of october dst ends
	I shall not concern myself with the vargaries of dst in the 70s
*]
    time_t time = *timer - 60 * __gh_timezone();
    register struct tm *temp;
    register int isdst=0, sn_mday;

    if (*timer == (time_t)-1)
	return(NULL);
    if ((temp = gmtime(&time))==NULL)
	return(NULL);
    if (temp->tm_mon<=2 || temp->tm_mon>=10)[*March or before or Nov or after *]
	isdst=0;
    else if ( temp->tm_mon!=3 && temp->tm_mon!=9 )  [* between may and Sept *]
	isdst=1;
    else if ( temp->tm_mon==9 ) {                   [* october *]
	sn_mday=temp->tm_mday-temp->tm_wday;
	while ( sn_mday+7 <=31 ) sn_mday +=7;
	if ( temp->tm_mday<sn_mday )
	    isdst=1;
	else if ( temp->tm_mday==sn_mday && temp->tm_hour<2 )
	    isdst=1;
    } else if ( temp->tm_year<87 ) {                [* april, before 87 *]
	sn_mday=temp->tm_mday-temp->tm_wday;
	while ( sn_mday+7 <=31 ) sn_mday +=7;
	if ( temp->tm_mday>sn_mday )
	    isdst=1;
	else if ( temp->tm_mday==sn_mday && temp->tm_hour>=2 )
	    isdst=1;
    } else {                                        [* april, after 87 *]
	sn_mday=temp->tm_mday-temp->tm_wday+7;
	while ( sn_mday-7 > 1 ) sn_mday -=7;
	if ( temp->tm_mday>sn_mday )
	    isdst=1;
	else if ( temp->tm_mday==sn_mday && temp->tm_hour>=2 )
	    isdst=1;
    }

    temp->tm_isdst = isdst;
    if ( isdst ) {
	if ( ++(temp->tm_hour)==24 ) {
	    static const char mons[]={31,28,31,30,31,30,31,31,30,31,30,31};
	    temp->tm_hour=0;
	    ++(temp->tm_yday);          [* can't overflow dst not on Dec. 31 *]
	    if ( ++(temp->tm_wday)==7 ) temp->tm_wday=0;
	    if ( ++(temp->tm_mday)>mons[temp->tm_mon] ) {
		temp->tm_mday=1;
		++(temp->tm_mon);       [* can't overflow dst not on Dec. 31 *]
	    }
	}
    }
    return( temp );
}
End Old Code */
#endif	/* CROSSUNIX or !ANYUNIX */

/* Added the following code for better System V compatibility		*/
/* Actually, most systems provide tzset() in libc.a and that one should	*/
/* be used, but I wanted to add the global variables to <time.h>	*/
/* and it seemed reasonable that if the Green Hills Software <time.h>	*/
/* had the System V style timezone variables, we should also provide	*/
/* the appropriate library routine which sets them.  There are 2 	*/
/* problems with this implementation.  The global variable 'timezone'	*/
/* conflicts with the BSD type 'struct timezone', therefore it is not	*/
/* convenient to use gettimeofday() on BSD to set timezone.  Further,	*/
/* getenv() is needed here, and that is defined in libansi.a., which	*/
/* always preceeds libind.a.  For now getenv() is coded inline. 	*/
#if (defined(ANYSYSV) && defined(CROSSUNIX)) || \
    defined(SIMULATE) || defined(EMBEDDED) || defined(MSW)
#define NEEDTZSET
#endif
#if defined(NEEDTZSET)
/******************************************************************************/
/* void tzset(void);							      */
/* System V compatible method of setting the current timezone and daylight    */
/* savings status.  Requires that the external variable, environ, be set to   */
/* a list of environment strings, including one of the form TZ=PST8PDT	      */
/* Explanation needed for the timezone, altzone, daylight and tzname variables*/
/******************************************************************************/
#define IsAlpha(c)	((c>='A'&&c<='Z')||(c>='a'&&c<='z'))
#define IsDigit(c)	(c>='0'&&c<='9')

long timezone, altzone;
int daylight;
static char tzname_default[] = "GMT\0   ";
#ifdef __ghs_pid
char *tzname[2];
#else
char *tzname[2] = { tzname_default, tzname_default+4 } ;
#endif
void tzset(void) {
    char *tz = NULL;
#if defined(ANYUNIX)||defined(UNIXSYSCALLS)||defined(MSW)
/* Ugly.  An inline copy of getenv() to avoid conflicts with C libraries. */
/* should be tz = getenv("TZ");						  */
    extern char **environ;
    char **envp = environ, *e;
    if (envp) 
	while (e = *envp++)
	    if (e[0] == 'T' && e[1] == 'Z' && e[2] == '=') {
		tz = e + 3;
		break;
	    }
#else
/* Outside of Unix, don't know how to get the environment, so TZ will be NULL */
#endif
    if (tz != NULL) {
	int hour = 0, minute = 0, second = 0,sign = 1;
	char *zone = NULL;
	int ahour = 0, aminute = 0, asecond = 0,asign = 1;
	char *azone = NULL;

	/* 3 letter timezone required */
	if (!IsAlpha(tz[0]) || !IsAlpha(tz[1]) || !IsAlpha(tz[2])) 
	    return;
	zone = tz; tz += 3;

	/* the rest is optional */
	if ( *tz ) {
	    /* signed hour:min:sec difference from GMT */
	    if ( *tz == '+' || *tz == '-' ) {
		if (*tz == '-') sign = -1;
		tz++;
	    }
	    if (!IsDigit(tz[0]))
		return;
	    hour = *tz++ - '0';
	    if (IsDigit(tz[0]))
		hour = hour * 10 + *tz++ - '0';
	    hour *= sign;
	    if (tz[0] == ':' && IsDigit(tz[1]) && IsDigit(tz[2])) {
		minute = ((tz[1] - '0') * 10 + tz[2] - '0') * sign;
		tz += 3;
		if (tz[0] == ':' && IsDigit(tz[1]) && IsDigit(tz[2])) {
		    second = ((tz[1] - '0') * 10 + tz[2] - '0') * sign;
		    tz += 3;
		}
	    }
	    /* the rest is optional */
	    if ( *tz && IsAlpha(tz[0]) && IsAlpha(tz[1]) && IsAlpha(tz[2])) {

		/* 3 letter daylight timezone */
		azone = tz; tz += 3;

		/* signed hour:min:sec difference from GMT (may be skipped) */
		if ( *tz == '+' || *tz == '-' ) {
		    if (*tz == '-') asign = -1;
		    tz++;
		}
		if (IsDigit(tz[0])) {
		    ahour = *tz++ - '0';
		    if (IsDigit(tz[0]))
			ahour = ahour * 10 + *tz++ - '0';
		    ahour *= asign;
		    if (tz[0] == ':' && IsDigit(tz[1]) && IsDigit(tz[2])) {
			aminute = ((tz[1] - '0') * 10 + tz[2] - '0') * asign;
			tz += 3;
			if (tz[0] == ':' && IsDigit(tz[1]) && IsDigit(tz[2])) {
			    asecond = ((tz[1] - '0') * 10 + tz[2] - '0')*asign;
			    tz += 3;
			}
		    }
		} else if (*tz == '\0')
		    ahour = hour - 1;
	    }
	}
	/* the rest of TZ is ignored by this implementation */

	timezone = hour * 60 * 60 + minute * 60 + second;
	daylight = 0;
#ifdef __ghs_pid
	tzname[0] = tzname_default;
	tzname[1] = tzname_default+4;
#endif
	tzname[0][0] = zone[0];
	tzname[0][1] = zone[1];
	tzname[0][2] = zone[2];
	tzname[0][3] = '\0';
	if (azone) {
	    tzname[1][0] = azone[0];
	    tzname[1][1] = azone[1];
	    tzname[1][2] = azone[2];
	    tzname[1][3] = '\0';
	    altzone = ahour * 60 * 60 + aminute * 60 + asecond;
	    daylight = 1;
	}
	return;
    }
}
#endif /* NEEDTZSET */

/******************************************************************************/
/* int __gh_timezone(void);						      */
/*  Return the number of minutes west of Greenwich Mean Time of the current   */
/*  time zone.  If the time() functions return the local time rather than     */
/*  Greenwich Mean Time then return 0 from __gh_timezone().		      */
/*  See also tzset() and localtime()					      */
/******************************************************************************/
int __gh_timezone(void) {
#if defined(ANYSYSV) || defined(NEEDTZSET)
    extern long timezone;
    tzset();
    return(timezone/60);
#elif defined(ANYBSD)
    struct timeval ignore;
    struct timezone tz;
    gettimeofday(&ignore,&tz);
    return(tz.tz_minuteswest);
#elif defined(ANYUNIX)|| defined(UNIXSYSCALLS)|| defined(SIMULATE)
#  if defined(TIMEZONE)
    return(TIMEZONE*60);
#  else
    return(8*60);
#  endif	/* TIMEZONE */
#else
    return 0;
#endif
}

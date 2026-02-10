/*
		    Low Level Interface Library
            Copyright 1983-2000 Green Hills Software,Inc.

 *  This program is the property of Green Hills Software, Inc,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software, Inc., or as agreed in writing signed by the President
 *  of Green Hills Software, Inc.
*/
/* ind_gmtm.c: ANSI gmtime() facility. */

#include "indos.h"
#include "ind_thrd.h"

#if defined(CROSSUNIX) || !defined(ANYUNIX)

static const char mons[]={31,28,31,30,31,30,31,31,30,31,30};
/******************************************************************************/
/* #include <time.h>							      */
/* struct tm *gmtime(const time_t *timer);				      */
/*									      */
/*  gmtime returns a structure containing the current Greenwich Mean Time     */
/*  broken down into tm_year (current year - 1900), tm_mon (current month     */
/*  January=0), tm_mday (current day of month), tm_hour (current hour 24 hour */
/*  time), tm_min (current minute), and tm_sec (current second).              */
/*									      */
/*  timer is a pointer to a long containing the number of seconds since the   */
/*  Epoch (as set by time()).			                              */
/*									      */
/*  Return 0 if no time of day can be returned				      */
/*									      */
/*  NOT REENTRANT because it returns the address of a static buffer           */
/*  could be made reentrant if every call to gmtime allocated a new structure */
/******************************************************************************/
/*  Wed Jul 16 12:06:49 PDT 1997 [enj][odin]
    Added a VxWorks-like reentrant version of gmtime.
    The new version (gmtime_r) takes another argument that
    points to resulting struct tm.
*/

/* New Code */
struct tm *gmtime_r(const time_t *timer, struct tm *result) {
/*  If no other implementation provided, assume Epoch is 00:00:00 January 1,1970
    as is true in UNIX.
*/
    register time_t time = *timer;
    register int islpyr, lpcnt;
    register int t;
    register time_t i, ystart, y;

    if ( *timer == (time_t) -1 )
	return(NULL);

#if defined(MSW) || defined(MSDOS)
    time -= 2209075200L;	/* convert to Epoch from MS-DOS */
#endif

    result->tm_isdst=0;
    i=time;
    while (i<0)
	i+= 7l*24*60*60*1000;    /* preserve the day of the week */
    i = (i/(24l*60*60))%7-3;
    while (i<0)
	i += 7;
    result->tm_wday = i;
    i = time % (24l*60*60);
    if ( i<0 ) i = i+24l*60*60;
    result->tm_hour = (i/(60*60))%24;
    result->tm_min = (i/60)%60;
    result->tm_sec = i%60;

    y = time/(365l*24*60*60+6*60*60);        /* a year is about 365.25 days */
    y += 370;

    do {
	islpyr = y%4==0 && (y%100!=0 || y%400==0);
	lpcnt = y/4;
	lpcnt -= y/100;
	lpcnt += y/400;

	lpcnt -= 89;            /* number of leap years in 370 years */
	ystart = (y-370)*(365l*24*60*60) + lpcnt*(24l*60*60);
	if ( ystart>time )
	    --y;
    } while ( ystart>time );    /* should loop at most twice, at least once */

    time -= ystart;
    time /= 24l*60*60;
/* [JY] Tue Jan  3 14:24:47 PST 1995.  was possible to get 12/32/94 from this.*/
/* If we get here with time of 365, this will happen.  Simply set the day to */
/* zero and increment the year.  This should work. */
    if ( time == 365 ) {
	time=0;
	y++;
    }
/* end of new code */
    if ( islpyr )
	++ time;
    result->tm_yday= time;
    ++ time;

    for ( i=0; i<11; ++i ) {
	t = mons[i];
	if ( i==1 && islpyr )
	    ++t;
	if ( time<=t )
	    break;
	time -= t;
    }

    result->tm_year = y-300;
    result->tm_mon  = i;
    result->tm_mday = time;        /* we incremented the mday above */
    return(result);
}

struct tm *gmtime(const time_t *timer)
{
    return(gmtime_r(timer, &GetThreadLocalStorage()->gmtime_temp));
    
}
/* End New Code */
/* Old Code
struct tm *gmtime(const time_t *timer) {
[*  If no other implementation provided, assume Epoch is 00:00:00 January 1,1970
    as is true in UNIX.
*]
    register time_t time = *timer;
    register int islpyr, lpcnt;
    register int y, ystart, i, t;

    ThreadLocalStorage *tlsptr = GetThreadLocalStorage();
    if ( *timer == (time_t) -1 )
	return(NULL);

#if defined(MSW) || defined(MSDOS)
    time -= 2209075200L;	[* convert to Epoch from MS-DOS *]
#endif

    tlsptr->gmtime_temp.tm_isdst=0;
    i=time;
    while (i<0)
	i+= 7L*24*60*60*1000;    [* preserve the day of the week *]
    i = (i/(24*60*60))%7-3;
    while (i<0)
	i += 7;
    tlsptr->gmtime_temp.tm_wday = i;
    i = time % (24l*60*60);
    if ( i<0 ) i = i+24l*60*60;
    tlsptr->gmtime_temp.tm_hour = (i/(60*60))%24;
    tlsptr->gmtime_temp.tm_min = (i/60)%60;
    tlsptr->gmtime_temp.tm_sec = i%60;

    y = time/(365*24*60*60+6*60*60);        [* a year is about 365.25 days *]
    y += 370;

    do {
	islpyr = y%4==0 && (y%100!=0 || y%400==0);
	lpcnt = y/4;
	lpcnt -= y/100;
	lpcnt += y/400;

	lpcnt -= 89;            [* number of leap years in 370 years *]
	ystart = (y-370)*(365*24*60*60) + lpcnt*(24*60*60);
	if ( ystart>time )
	    --y;
    } while ( ystart>time );    [* should loop at most twice, at least once *]

    time -= ystart;
    time /= 24*60*60;
[* [JY] Tue Jan  3 14:24:47 PST 1995.  was possible to get 12/32/94 from this.*]
[* If we get here with time of 365, this will happen.  Simply set the day to *]
[* zero and increment the year.  This should work. *]
    if ( time == 365 ) {
	time=0;
	y++;
    }
[* end of new code *]
    if ( islpyr )
	++ time;
    tlsptr->gmtime_temp.tm_yday= time;
    ++ time;

    for ( i=0; i<11; ++i ) {
	t = mons[i];
	if ( i==1 && islpyr )
	    ++t;
	if ( time<=t )
	    break;
	time -= t;
    }

    tlsptr->gmtime_temp.tm_year= y-300;
    tlsptr->gmtime_temp.tm_mon= i;
    tlsptr->gmtime_temp.tm_mday = time;        [* we incremented the mday above *]
    return( &tlsptr->gmtime_temp );
}
End Old Code */
#else
int _J_empty_file_illegal;
#endif	/* CROSSUNIX or !ANYUNIX */

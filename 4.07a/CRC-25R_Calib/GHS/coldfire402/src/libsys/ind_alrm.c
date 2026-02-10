/*
		    Low Level Interface Library

            Copyright 1983-2000 Green Hills Software,Inc.

 *  This program is the property of Green Hills Software, Inc,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software, Inc., or as agreed in writing signed by the President
 *  of Green Hills Software, Inc.
*/

#include "indos.h"
#include <signal.h>

#if defined(EMBEDDED)

/******************************************************************************/
/* __ualarmhandler is an interrupt procedure for dispatching alarms */
/******************************************************************************/
#if 0 && defined(__ARM)
/* This is an example of how to insert this handler into the ARM IRQ vector */
#if defined(__THUMB_AWARE) && defined(__THUMB)
# pragma ghs nothumb
#endif /* defined(__THUMB_AWARE) && defined(__THUMB) */
static void __ualarmhandler(void) {
    raise(SIGALRM);
}
#pragma intvect __ualarmhandler 0x18
#if defined(__THUMB_AWARE) && defined(__THUMB)
# pragma ghs thumb
#endif /* defined(__THUMB_AWARE) && defined(__THUMB) */
#else

static void __ualarmhandler(void) {
#pragma ghs interrupt
    raise(SIGALRM);
}

#endif

/******************************************************************************/
/*  unsigned int ualarm(unsigned int value, unsigned int interval);           */
/*  ualarm arranges for a SIGALRM to be delivered in _value_ microseconds and */
/*  then every _interval_ microseconds thereafter, like the BSD ualarm() call.*/
/*  If the environment does not support this facility, return ~0L.            */
/*									      */
/*  WARNING: _interval_ is not supported at this time and is ignored.         */
/******************************************************************************/

unsigned int ualarm(unsigned int value, unsigned int interval) {
    (void) __ghs_syscall(SYSCALL_HANDLER, __ualarmhandler);
    return __ghs_syscall(SYSCALL_TIMEOUT, value);
}

/******************************************************************************/
/*  unsigned int alarm(unsigned int seconds);				      */
/*  ualarm arranges for a SIGALRM to be delivered in _seconds_ seconds.       */
/*  If the environment does not support this facility, return ~0.             */
/******************************************************************************/
unsigned int alarm(unsigned int seconds) {
    return ualarm(seconds*1000000, 0);
}
#else
int alarm_empty_file_is_illegal;
#endif /* defined(EMBEDDED) */

/*
		    Low Level Interface Library

            Copyright 1983-2000 Green Hills Software,Inc.

 *  This program is the property of Green Hills Software, Inc,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software, Inc., or as agreed in writing signed by the President
 *  of Green Hills Software, Inc.
*/
/* ind_sgnl.c: ANSI signal() and raise() facilities, and NON-ANSI [u]alarm(). */

#include "indos.h"
#include "ind_thrd.h"
#include "ind_exit.h"

#if defined(LIBCISANSI)
int _U_empty_file_illegal;
#else /* !defined(LIBCISANSI) */

/******************************************************************************/
/*  #include <signal.h>							      */
/*  int raise(int sig);							      */
/*  Raise the signal, "sig" for the current process according to ANSI C.      */
/*  If sig==SIGKILL then terminate the process with status = 1.		      */
/*  Otherwise, execute the function set for the signal, "sig", by the most    */
/*  recent call to signal(), see below.					      */
/*	If the function specified is SIG_IGN then do nothing.		      */
/*	If the function specified is SIG_DFL then terminate the process with  */
/*		status=1.						      */
/*	Otherwise set the function SIG_DFL for "sig", then execute the	      */
/*		function with the signal, "sig" passed as an argument.	      */
/*  Return -1 if there is no such signal.  Return 0 if the operation succeeds.*/
/******************************************************************************/

/* BSD kill() varies slightly from Ansi raise() */
#if defined(ANYBSD) && !defined(NO_SIGNAL) && !defined(__alpha)
#include <signal.h>
int raise(int sig)
{
    void (*handler)() = signal(sig, SIG_IGN);

    if (handler == SIG_ERR)
	return -1;
    if (handler == SIG_DFL) {
	(void)signal(sig, SIG_DFL);
	return kill(getpid(), sig);
    }
    if (handler != SIG_IGN)
	(*handler)(sig);
    return 0;
}
#elif (defined(ANYUNIX) || defined(MSW)) && !defined(NO_SIGNAL)	
int raise(int sig)
{
    return(kill(getpid(),sig)); /*for a system that has a real signal()/kill()*/
}
#else	/* NO_SIGNAL or ! ( BSD or ANYUNIX) */

    /* this is a very basic signal mechanism, it only implements the minimal */
    /* requirements for ANSI C */

/******************************************************************************/
/*  #include <signal.h>							      */
/*  void (*signal(int sig, void (*func)(int)))(int);			      */
/*  Set the function to execute when the signal, "sig" is raised, see raise() */
/*	above.								      */
/*  If the function specified is SIG_IGN then raise() will do nothing.	      */
/*  If the function specified is SIG_DFL then raise() will terminate the      */
/*	process with status=1.						      */
/*  Otherwise set the function for signal "sig" to be "func".		      */
/*  Return -1 if there is no such signal.  Return the previous function	      */
/*	specified for the signal "sig" if the operation succeeds.	      */
/*									      */
/******************************************************************************/
#include <signal.h>

void __gh_signal_init(ThreadLocalStorage *TheThreadLocalStorage)
{
    int i;

    if (!TheThreadLocalStorage)
        TheThreadLocalStorage = GetThreadLocalStorage();
    for (i = 1; i <= _SIGMAX; i++)
	TheThreadLocalStorage->SignalHandlers[i-1] = SIG_DFL;
    for (i = SIGSTOP; i <= SIGIO; i++)
	TheThreadLocalStorage->SignalHandlers[i-1] = SIG_IGN;
    for (i = SIGPROF; i <= SIGUSR2; i++)
	TheThreadLocalStorage->SignalHandlers[i-1] = SIG_IGN;
}

void (*signal(int sig, void (*func)(int)))(int)
{
    SignalHandler	temp;
    ThreadLocalStorage	*tlsptr;

    if (sig<=0 || sig>_SIGMAX)
	return(SIG_ERR);
    tlsptr = GetThreadLocalStorage();
    temp = tlsptr->SignalHandlers[sig-1];
    tlsptr->SignalHandlers[sig-1] = func;
    return(temp);
}

int raise(int sig) 
 {
    SignalHandler	temp;
    ThreadLocalStorage	*tlsptr;

    if (sig<=0 || sig>_SIGMAX)
	return(-1);
    tlsptr = GetThreadLocalStorage();
    temp = tlsptr->SignalHandlers[sig-1];
    if (sig==SIGKILL || temp==SIG_DFL)
	DefaultSignalHandler(sig);
    else if (temp!=SIG_IGN) {
        tlsptr->SignalHandlers[sig-1] = SIG_DFL;
	(*temp)(sig);
    }
    return(0);
}
#endif
#endif /* defined(LIBCISANSI) */

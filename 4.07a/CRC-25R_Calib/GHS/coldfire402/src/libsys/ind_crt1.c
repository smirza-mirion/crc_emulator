/*
		    ANSI C Runtime Library
	
	Copyright 1983-2000 Green Hills Software, Inc.

    This program is the property of Green Hills Software, Inc,
    its contents are proprietary information and no part of it
    is to be disclosed to anyone except employees of Green Hills
    Software, Inc., or as agreed in writing signed by the President
    of Green Hills Software, Inc.
*/

#include "inddef.h"
#include "ind_crt1.h"
#include "ind_exit.h"
#include "ind_thrd.h"

/*----------------------------------------------------------------------*/
/* ind_crt1.c: Machine Independent library initialization startup code. */
/*									*/
/* CALLED FROM:  __ghs_ind_crt0 in ind_crt0.p				*/
/* ENTRY POINT:  __ghs_ind_crt1 					*/
/*----------------------------------------------------------------------*/
/* This is the first C function called by the standard libraries after	*/
/* memory and static and global data have been initialized.  It         */
/* initializes the ANSI C and other run time libraries before jumping   */
/* to the application entry point (main).				*/
/* Arguments to the initialization routine:  argc, argv, and envp.	*/
/* These are valid if the program is run from the debugger or 0 if not.	*/
/* If argc==0, we construct the arguments to main() on our stack frame. */
/* You may change these arguments to provide any other default values	*/
/*----------------------------------------------------------------------*/

#if defined(RODATA_IS_INDEPENDENT)
#  define CONST_FUNCP *
#else
#  define CONST_FUNCP *const
#endif

char **environ;

void __ghs_ind_crt1 (int argc, char *argv[], char *envp[])
{
    /*---------------------*/
/* initialize ind_io.c */
/*---------------------*/
    _enter();				/* initialize ind_io.c */

/*-----------------*/
/* initialize lock */
/*-----------------*/
    {
    #pragma weak __gh_lock_init
    extern void __gh_lock_init(void);
    static void (CONST_FUNCP lock_init_funcp)(void) = __gh_lock_init;
    /* If ind_thrd.c is loaded, initialize the C library lock */
    if (lock_init_funcp) __gh_lock_init();
    }
	
/*----------------*/
/* initialize iob */
/*----------------*/
    {
    #pragma weak __gh_iob_init
    extern void __gh_iob_init(void);
    static void (CONST_FUNCP iob_init_funcp)(void) = __gh_iob_init;
    /* if ind_iob.c is loaded, initialize _iob for stdin,stdout,stderr */
    if (iob_init_funcp) __gh_iob_init();
    }

/*------------------*/
/* initialize error */
/*------------------*/
    {
    #pragma weak __gh_error_init
    extern void __gh_error_init(void);
    static void (CONST_FUNCP error_init_funcp)(void) = __gh_error_init;
    /* if indrnerr.c is loaded, initialize error */
    if (error_init_funcp) __gh_error_init();
    }

/*-------------------*/
/* initialize signal */
/*-------------------*/
    {
    #pragma weak __gh_signal_init
    extern void __gh_signal_init(void *);
    static void (CONST_FUNCP signal_init_funcp)(void *) = __gh_signal_init;
    /* if ind_sgnl.c is loaded, initialize the signal functions array */
    if (signal_init_funcp) __gh_signal_init(0);
    }

#ifndef __disable_thread_safe_extensions
/*--------------------*/
/* C++ Thread-safe    */
/* Exception handling */
/*--------------------*/
    {
#if !defined (__OSE)
	#pragma weak __cpp_exception_init
	extern void __cpp_exception_init(void **);
	static void (CONST_FUNCP cpp_init_funcp)(void **) = __cpp_exception_init;
	if (cpp_init_funcp) 
	    __cpp_exception_init(&(GetThreadLocalStorage()->__eh_globals));
#endif /* !__OSE */
    }
#endif

/*-----------------------------*/
/* initialize manual profiling */
/*-----------------------------*/
    {
        #pragma weak __ghs_manprf_init
	extern void __ghs_manprf_init(void);
	static void (CONST_FUNCP man_prf_funcp)(void) = __ghs_manprf_init;
	/* if ind_manprf.c is loaded, initialize manual profiling */
	if (man_prf_funcp) __ghs_manprf_init();
    }
    
#if defined(__i960)
/*-----------------------------------*/
/* special initialization for mon960 */
/*-----------------------------------*/
    {
    /* Initialize the mon960 timers for profiling i960	*/
    /* programs downloaded to mon960 by MULTI/960SERV.	*/
    extern void _startmon960(void);
    _startmon960();			/* defined in file mon960.c */
    }
#endif


    /* New code: bcarnes Tue Oct 21 14:30:38 PDT 1997
     * We now have profiling support on all pmon/mips boards
     * which possess a 2681 DUART */
#if defined(__mips) && !defined(__TRW_RH32__) && !defined(__MIPSX__) && !defined(__CORE1__)
    {
	int __ghs_start_pmon_profiling(void);
	__ghs_start_pmon_profiling();			/* defined below */
    }
#endif	

/*--------------------------------------------*/
/* in ROM mode, set default arguments to main */
/*--------------------------------------------*/
    {

    /* Declare 'main' as a far function on MIPS when in LocalPIC mode,
       so that it can be reached however large the user program is.
       Without being 'far', 'main' can be out of the range of 16-bits
       that a local pic call allows when the user program is too large. */
#if defined(__mips) && !defined(__TRW_RH32) && !defined(__MIPS_X) && \
	defined(__ghs_pic) && defined(__localpic)
#pragma ghs callmode=far
    extern 	int main(int argc, char ** argv, char **envp);
#pragma ghs callmode=default
#else
    extern	int main (int argc, char **argv, char **envp);
#endif

/*  char **environ; */

    /* THESE ARE THE DEFAULT ARGUMENTS TO main() WHEN WE EXECUTE FROM ROM */
    if (!argc) {
	char noname[2];
	char *arg[2];
	char *env[2];

	noname[0] = 0;
	noname[1] = 0;

	arg[0] = noname;
	arg[1] = 0;

	env[0] = noname+1;
	env[1] = 0;

	envp = env;
	argv = arg;

	argc = 1;
    }
    environ = envp;

    {
	/* __alt_init allows extra initialization functions to be plugged in.*/
	#pragma weak __alt_init
	extern void __alt_init(int argc, char **argv, char **envp);
	static void (CONST_FUNCP alt_init_funcp)(int,char **,char **) = __alt_init;

	if (alt_init_funcp)
	    __alt_init(argc, argv, envp);
    }
/*------------------------------*/
/* call main(argc, argv, envp)	*/
/*------------------------------*/
    /* From libsys we don't call exit() which is in a higher level library. */
    /* Instead, we call _exit() which is in this library.  There are a few  */
    /* things in libansi.a which may need to be called by _exit().  This is */
    /* handled by calling __ghs_at_exit() from libansi.a to setup the calls */
    _exit(main(argc, argv, envp));	/* _exit() will shut down ind_io.c  */
    /* _exit() should never return. If it does, let our caller handle it.   */
    return;
    }
}

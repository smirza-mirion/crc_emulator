/*
		    Language Independent Library

	    Copyright 1983-2000 Green Hills Software,Inc.

 *  This program is the property of Green Hills Software, Inc,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software, Inc., or as agreed in writing signed by the President
 *  of Green Hills Software, Inc.
*/
/* ind_exit.c: low-level startup & shutdown. Machine Independent. */

/* please examine ind_io.c for an overview of this file's contents */

#if defined(EMBEDDED)

#include "indos.h"
#include "ind_exit.h"
#include "ind_thrd.h"

/*============================================================================*/
/*   FUNCTIONS ALWAYS NEEDED TO RUN PROGRAMS COMPILED WITH THE DEFAULT CRT0   */
/*============================================================================*/
/*	_enter()	ensure the initial file descriptors are ready	      */
/*	_exit()		terminate the program				      */
/*	__ghs_at_exit()	arrange for a function to be invoked at _exit() time  */
/*============================================================================*/

/******************************************************************************/
/*	void _enter (void);						      */
/*  _enter() is called from ind_crt0.c to initialize the I/O system.	      */
/*  After _enter() returns, the standard file descriptors 0, 1, and 2 are     */
/*  expected to be ready for use by functions such as read() and write().     */
/*									      */
/*  When the __ghs_syscall() environment is provided by the MULTI debugger,   */
/*  no work is needed here to set up the initial file descriptors. MULTI sets */
/*  them up each time a process is started in the debugger, and allows the    */
/*  embedded process to use certain of its own file descriptors for Host I/O. */
/*  Processes that are being debugged are only allowed to access descriptors  */
/*  they open, and MULTI ensures that they are forced closed when the process */
/*  is restarted or when MULTI itself exits.  The Green Hills C Library stdio */
/*  implementation closes all FILE*'s opened via fopen() (as well as stdin,   */
/*  stdout, and stderr) when exit() is called.				      */
/*									      */
/*  File 0 is used by the C library "stdin". Example uses: gets(), scanf()    */
/*  File 1 is used by the C library "stdout". Example uses: printf(), puts()  */
/*  File 2 is used by the C library "stderr". Example uses: perror()	      */
/******************************************************************************/
void _enter (void)
{
    /* [TPW] Attempt to enable all caching options that may be available.   */
    /*       This is supported by some ROM monitors, maybe other targets.   */
    /*       It is defined to return -1 with errno == 0 if it does nothing. */
    /*       If it returns success, it assumes that your program will not   */
    /*       play with the memory mapping or cache state that it sets up.   */
    /*       The main purpose of this call is to build benchmarks with the  */
    /*       standard shipping library, without having to link in custom    */
    /*       initialization code every time.  Many targets do nothing when  */
    /*       this is called.  If you are initializing the caches yourself,  */
    /*       remove this call so the debugger leaves the caches alone.	    */
    (void)__ghs_syscall(SYSCALL_CACHESET, ~0L);
}

static struct __GHS_AT_EXIT *exitlisthead;

/******************************************************************************/
/*  void _exit (code);							      */
/*  Exit from the program with a status code specified by code.		      */
/*  DO NOT RETURN!							      */
/*									      */
/*  This function is called by exit() after the libraries have shut down, and */
/*  also by ind_crt0.c in the event that the program's main() returns.	      */
/******************************************************************************/
#if !defined(MINIMAL_STARTUP) && !defined(__ARM)
static unsigned int div0 (unsigned int x) { return (2/(char)x)&1; }
#endif
void _exit (int code)
{
    volatile unsigned int careful_zero = 0;
    struct __GHS_AT_EXIT *el;

    __ghsLock();
    for (el = exitlisthead; el; el = el->next)
	el->func();

/* [nikola] Mon Sep 20 15:07:55 PDT 1999 - cleanup the exception handling */
#ifndef __disable_thread_safe_extensions
/*--------------------*/
/* C++ Thread-safe    */
/* Exception handling */
/*--------------------*/
    {
        #pragma weak __cpp_exception_cleanup
        extern void __cpp_exception_cleanup(void **);
        static void (*const cpp_cleanup_funcp)(void **) = 
	                                               __cpp_exception_cleanup;
	if (cpp_cleanup_funcp)
	  __cpp_exception_cleanup(&(GetThreadLocalStorage()->__eh_globals));
    }
#endif /* !defined(__disable_thread_safe_extensions) */

    (void)__ghs_syscall(SYSCALL_EXIT, code);
/* If we get here, we couldn't trap out. Try to force a divide by zero; */
/* if that doesn't fail (some chips do not trap it), then loop forever. */
#if defined(MINIMAL_STARTUP) || defined(__ARM)
    for (;;)
	continue;
#else
    while (div0(careful_zero) < 2)
	continue;
    __ghsUnlock();
#endif
}

/******************************************************************************/
/*  void __ghs_at_exit (struct __GHS_AT_EXIT *gae);			      */
/*  Add gae to the list of functions to be executed when _exit() is called.   */
/******************************************************************************/
void __ghs_at_exit (struct __GHS_AT_EXIT *gae)
{
    __ghsLock();
    gae->next = exitlisthead;
    exitlisthead = gae;
    __ghsUnlock();
}

/* DefaultSignalHandler is called for signal SIGKILL or if the signal handler
   is SIG_DFL */
void DefaultSignalHandler(int sig)
{
    _exit(EXIT_FAILURE);
}

#endif	/* EMBEDDED */

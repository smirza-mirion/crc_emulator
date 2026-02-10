/*
		    ANSI C Runtime Library
	
	Copyright 1983-2000 Green Hills Software, Inc.

    This program is the property of Green Hills Software, Inc,
    its contents are proprietary information and no part of it
    is to be disclosed to anyone except employees of Green Hills
    Software, Inc., or as agreed in writing signed by the President
    of Green Hills Software, Inc.
*/

/* This file contains i960 Profiling routines working on the Intel MON960 */

#if defined(__i960)
#include "ind_exit.h"
static int ProfCount;   /* number of entries in ProfBuf */

/* A larger ProfBuf means more samples collected on the target before stopping
 * to send them up to 960SERV - more target memory traded for faster profiling.
 */

static unsigned int    ProfBuf[100];    /* array of PC samples */

/* Someday, rewrite the following asm() code to use SVR4 ASM PROCS	*/

/*------------------------------------------------------*/
/* mon960 system calls to enable and disable tracing	*/
/*------------------------------------------------------*/
#pragma ghs Xkeepsaverestore

/* This pragma turns off leaf routine optimization.  This is ensures	*/
/* that arguments to the following asm routines don't get clobbered.	*/

/* enable trace fault by setting 1st bit in PC */

static void trace_enable(void)
{
    asm("       mov     1, g0");
    asm("       modpc   1, 1, g0");     /* set bit 0 (TE) of PC */
    asm("       or      g0, g0, g0");   /* nop */
    asm("       or      g0, g0, g0");   /* nop */
    asm("       or      g0, g0, g0");   /* nop */
}

/* disable trace fault by clearing 1st bit in PC */
static void trace_disable(void)
{
    asm("       mov     0, g0");
    asm("       modpc   1, 1, g0");
    asm("       or      g0, g0, g0");   /* nop */
    asm("       or      g0, g0, g0");   /* nop */
    asm("       or      g0, g0, g0");   /* nop */
}

#pragma ghs Zkeepsaverestore

/*-----------------------------------------------------------------*/
/* timer class: set, initialize, terminate, and service interrupts */
/*-----------------------------------------------------------------*/
#pragma ghs Xkeepsaverestore

/* This pragma turns off leaf routine optimization.  This is ensures	*/
/* that arguments to the following asm routines don't get clobbered.	*/

static void timer_set(int client,  int timer, void *isr)
{
    asm("       lda     249, g4");
    asm("       calls   g4");
}

static void timer_init(int freq)
{
    asm("       lda     243, g4");
    asm("       calls   g4");
}

static void timer_term(void)
{
    asm("       lda     246, g4");
    asm("       calls   g4");
}

/*--------------------------------------*/
/* timer ISR: interrupt service routine	*/
/*--------------------------------------*/

/* MON960 calls this profiling Interrupt Service Routine for each timer	*/
/* expiration/interrupt.  IP is the user IP when the interrupt happens.	*/
/* A "good" frequency does not interrupt too often:  about 50-60 Hz.	*/
/* The timer interrupts at 1000 Hz.  Every 20th interrupt yields 50 Hz.	*/

static void timer_isr(unsigned int IP)
{
    static int cnt;

    if (++cnt < 20) return;
    cnt = 0;

    ProfBuf[ProfCount++] = IP;		/* works first time since	*/
					/* ProfCount is 0 (in BSS)	*/

    /* if the profile buffer is full, write it out */

    if (ProfCount >= sizeof(ProfBuf)/sizeof(ProfBuf[0])) {
        /* allow the special syscall bp by enabling trace faults */
        trace_enable();

        /* write the full profile buffer to the host server */
        __ghs_syscall(17 /* SYSCALL_MANPROF */, ProfBuf, ProfCount);

        trace_disable();
        ProfCount = 0;
    }
}

#pragma ghs Zkeepsaverestore

static void _stopmon(void)
{
    if (ProfCount)
        __ghs_syscall(17 /* SYSCALL_MANPROF */, ProfBuf, ProfCount);
    timer_term();
}

void _startmon960(void)
{
    /* These two variables are defined in crt0.960 */
    extern int _gh_pflag;       
    static struct __GHS_AT_EXIT gae;

    /* 960SERV sets _gh_pflag to 1 to enable profiling. 	*/

    if (!_gh_pflag) return;

    /* On program exit, run the _stopmon cleanup routine.	*/

    gae.func = _stopmon;
    __ghs_at_exit(&gae);
    gae.func = _stopmon;

    timer_set(0, 0, (void *) timer_isr);

    /* The default timer frequency value is 2, meaning 1000 Hz	*/
    /* (an interrupt every 1 ms).  Intel told us that 1000 Hz	*/
    /* is the ONLY frequency currently supported by mon960.	*/

    timer_init(2);
}
#endif	/* __i960 */

/*
                    ANSI C Runtime Library

        Copyright 1983-2000 Green Hills Software, Inc.

    This program is the property of Green Hills Software, Inc,
    its contents are proprietary information and no part of it
    is to be disclosed to anyone except employees of Green Hills
    Software, Inc., or as agreed in writing signed by the President
    of Green Hills Software, Inc.
*/

/* This file contains MIPS Profiling routines working on the  PMONMIPS */

#if defined(__mips) && !defined(__TRW_RH32__) && !defined(__MIPSX__) && !defined(__CORE1__)
#include "ind_exit.h"
/* Begin pmon/mips profiling routines */

/* *********************PMON INTERFACE******************** */

typedef int iFunc();

#define PMON_TABLE  ((iFunc**) 0xbfc00200)
#define PMON_WRITE(F,S,L) (*PMON_TABLE[1])(F,S,L)
#define PMON_ONINTR(I,V)  (*PMON_TABLE[10])(I,V)

int __ghs_bmcisr(void);
static iFunc *__ghs_bmcisrdat[] = {0,__ghs_bmcisr};

static int ticks = 0;
static long savedsr;

/* *********************DUART INTERFACE******************** */

#ifdef MIPSEB
#define DUARTBASE 0xbe000003
#else
#define DUARTBASE 0xbe000000
#endif

/* 2681-specific defs */
#define DUART_IMR       *((volatile unsigned char *)((5*4)+DUARTBASE))
#define DUART_CTU       *((volatile unsigned char *)((6*4)+DUARTBASE))
#define DUART_CTL       *((volatile unsigned char *)((7*4)+DUARTBASE))
#define DUART_START *((volatile unsigned char *)((14*4)+DUARTBASE))
#define DUART_STOP  *((volatile unsigned char *)((15*4)+DUARTBASE))
#define CNTINT          0x08

#define TIMER_10ms      1152
#define TIMER_20ms      (TIMER_10ms*2)
#define TIMER_500ms     (TIMER_10ms*50)

#define SR_IEC          0x00000001      /* Interrupt Enable, current */
#define SR_IMASK        0x0000ff00      /* Interrupt Mask */

#pragma ghs callmode=far
extern /*__farcall*/ long __ghs_mfc0_cause(void);
extern /*__farcall*/ long __ghs_mfc0_epc(void);
extern /*__farcall*/ long __ghs_mfc0_sr(void);
extern /*__farcall*/ long __ghs_mtc0_sr(long);
#pragma ghs callmode=default

extern int  __ghs_enableprofileflag;

void __ghs_bmcisr_c(void)  /* interrupts are still disabled here, be nice */
{
        char buf[10];
        unsigned char c,x;
        unsigned long epc = __ghs_mfc0_epc();

        ticks++;

        buf[0] = '!';
        for (c = 8; c >= 1; c--)
        {
          x = epc & 0xf;
          buf[c] = (x <= 9) ? ('0'+x) : ('a'+x-10);
          epc >>= 4;
        }

        PMON_WRITE(1,buf,9);

        c = DUART_STOP; /* ackowledge interrupt */
}

void __ghs_stop_pmon_profiling(void)
{
        long sr = savedsr;

        __ghs_mtc0_sr(sr);
        DUART_STOP = 1;
        DUART_IMR = 0;

}

int __ghs_start_pmon_profiling()
{
        long t0,t1,intsr;
        static struct __GHS_AT_EXIT gae;

        if (!__ghs_enableprofileflag) return 0;

        gae.func = __ghs_stop_pmon_profiling;
        __ghs_at_exit(&gae);
        gae.func = __ghs_stop_pmon_profiling;

/* disable ints in SR */
        savedsr = __ghs_mfc0_sr();
        __ghs_mtc0_sr(savedsr&~SR_IEC);

        DUART_IMR = 0; /* disable clkints in IMR */


        /* prog timer for short period */
        DUART_CTL = 4;
        DUART_CTU = 0;

        t0 = __ghs_mfc0_cause(); /* get initial cause value */
        DUART_IMR = CNTINT; /* enable clkints in IMR */

        /* wait for cause i change */
        for (;;)
        {
                t1 = __ghs_mfc0_cause();
                if (t0 != t1) break;
        }

        t0 = SR_IMASK&(t0^t1);

        intsr = savedsr;
        intsr |= t0 | SR_IEC;

        /* reprogram timer */
        DUART_CTL = TIMER_20ms&0xff;
        DUART_CTU = TIMER_20ms>>8;

        PMON_ONINTR(0,__ghs_bmcisrdat);
        __ghs_mtc0_sr(intsr); /* set SR w/ interrupts */

	return 1;
}

#endif /* __mips */


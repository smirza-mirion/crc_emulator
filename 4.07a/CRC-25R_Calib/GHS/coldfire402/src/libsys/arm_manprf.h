
#if defined(__ghs_board_is_arm_edb7212)

/***
 * Timer-Driven Profiling for Cirrus Logic edb7xxx
 */

/* use high interrupt vectors */
#define VECT_IRQ	0xFFFF0018

typedef volatile unsigned int mm_reg;

#define MM(x)		(*(mm_reg*)x)

#define SYSCON1		MM(0x80000100)	/* system control register 1 */

#define TC1D		MM(0x80000300)	/* decrementer 1 */
#define TC2D		MM(0x80000340)	/* decrementer 2 */
#define RTCDR		MM(0x80000380)	/* real time clock */
#define RTCMR		MM(0x800003C0)	/* real time clock match */

/* clock source frequency for TC1: SYSCON1[5]
 *  TC2: SYSCON1[7]	(clear: 2kHz, set 512kHz) */

/* RTC at 1Hz */

#define TC1OI		8
#define TC2OI		9

#define TC1_MASK	(1 << TC1OI)
#define TC2_MASK	(1 << TC2OI)

#define INTMR1		MM(0x80000280)	/* interrupt mask register */
#define INTSR1		MM(0x80000240)	/* interrupt status register */

#define TC1EOI		MM(0x800006C0)	/* write for end of interrupt */
#define TC2EOI		MM(0x80000700)	/* write for end of interrupt */

/* setup the IRQ vector */
/*  under PIC would want to adjust address word
 *   at runtime, or place handler in ABS section */
#pragma asm
.org VECT_IRQ
ldr pc, IRQ_ISR_pool
IRQ_ISR_pool:
.word __ghs_manprf_timer_handler
#pragma endasm

#define TICKS_PER_SEC() 		(2*1000)

/* set compare register C; reset count register */
#define TIMER_INIT(ti_count)						\
   TC1D = ti_count;		/* set the decrementer value */

asm unsigned int GET_EPC(void)
{
    /* EPC contained in LR */
    mov r0, r14
}

/* clear the timer interrupt with a read of status register */
#define CLEAR_TIMER_INTERRUPT()						\
   TC1EOI = 0xffffffff;
   
/* enable IRQ's */
asm void ENABLE_IRQ(void)
{
    mrs r0, cpsr
    bic r0, r0, 0x80
    msr cpsr_c, r0
}
   
/* TC Interrup Enable: RC compare */
#define ENABLE_TIMER_INTERRUPT()					\
   SYSCON1 = 0;								\
   INTMR1 = TC1_MASK;	/* Int. Controller: enable timer interrupt */	\
   ENABLE_IRQ();	/* enable IRQ interrupts */


#elif defined(__ghs_board_is_arm_at91eb40)

/****
 * Manual Profiling for Atmel AT91X40 series
 *  (adjust above elif for your board)
 */

#include "at91_std_c.h"
#include "at91_tc.h"
#include "at91_aic.h"

#define VECT_IRQ	0x00000018
   
/*----------------------------*/
/* Advanced Interrupt Control */
/*----------------------------*/

#define AIC_IECR	(AIC_BASE->AIC_IECR)
#define AIC_IDCR	(AIC_BASE->AIC_IDCR)
#define AIC_ICCR	(AIC_BASE->AIC_ICCR)
#define AIC_ISCR	(AIC_BASE->AIC_ISCR)
#define AIC_EOICR	(AIC_BASE->AIC_EOICR)
#define AIC_IVR		(AIC_BASE->AIC_IVR)
#define AIC_SVR		(AIC_BASE->AIC_SVR)

/*---------------*/
/* Timer Counter */
/*---------------*/

#define TC_BMR		(*(at91_reg*)0xFFFE00C4)

#define TC0_BASE	((StructTC *)0xFFFE0000)

#define TC0_CCR		(TC0_BASE->TC_CCR)
#define TC0_CMR     	(TC0_BASE->TC_CMR)
#define TC0_CV      	(TC0_BASE->TC_CV)
#define TC0_RA      	(TC0_BASE->TC_RA)
#define TC0_RB      	(TC0_BASE->TC_RB)
#define TC0_RC      	(TC0_BASE->TC_RC)
#define TC0_SR      	(TC0_BASE->TC_SR)
#define TC0_IER     	(TC0_BASE->TC_IER)
#define TC0_IDR     	(TC0_BASE->TC_IDR)
#define TC0_IMR     	(TC0_BASE->TC_IMR)

#define TC0IRQ_ID	4
#define AIC_TC0IRQ	(1 << TC0IRQ_ID)

#define TC_CMR_VAL 							\
   (TC_WAVE		/* waveform mode */				\
   | TC_CPCSTOP 	/* stop counter with RC compare */		\
   | TC_CLKS_MCK128) 	/* MCK/128 */

/*--------------*/
/* Master Clock */
/*--------------*/

#define MCK         32768000
#define MCKKHz      (MCK/1000)
#define MCKMHz      (MCK/1000000)

/* setup the IRQ vector */
/*  this will load the ISR address from IVR (which is set from appropriate SVR)*/
#pragma asm
.org VECT_IRQ
ldr pc, [pc, -0xf20]
#pragma endasm

#define TICKS_PER_SEC() 		(MCK/128)

/* set compare register C; reset count register */
#define TIMER_INIT(ti_count)						\
   TC0_RC = ti_count;		/* set the RC compare value */

asm unsigned int GET_EPC(void)
{
    /* EPC contained in LR */
    mov r0, r14
}

/* clear the timer interrupt with a read of status register */
#define CLEAR_TIMER_INTERRUPT()						\
   AIC_IVR = 0;			/* clear the vector register   */	\
                                /*  (can use to index AIC_SVR) */	\
   AIC_ICCR = AIC_TC0IRQ;	/* clear TC0 IRQ */			\
   TC0_CCR = TC_SWTRG;   	/* restart the counter */		\
   AIC_EOICR  = TC0_SR; 	/* end of interrupt and reset TC0 interrupt */

/* enable IRQ's */
asm void ENABLE_IRQ(void)
{
    mrs r0, cpsr
    bic r0, r0, 0x80
    msr cpsr_c, r0
}
   
/* TC Interrup Enable: RC compare */
#define ENABLE_TIMER_INTERRUPT()					\
   AIC_SVR[TC0IRQ_ID] = (at91_reg)&__ghs_manprf_timer_handler; 		\
   /* set the appropriate source vector register */			\
   TC0_CMR = TC_CMR_VAL;	/* set timer mode */			\
   TC0_IER = TC_CPCS;		/* enable compare RC interrupt*/	\
   TC0_CCR = TC_CLKEN;		/* enable the clock */			\
   AIC_IECR = AIC_TC0IRQ;	/* AIC: enable timer interrupt */	\
   ENABLE_IRQ();		/* enable IRQ interrupts */		\
   TC0_CCR = TC_SWTRG;		/* reset and start the counter */

#else	/* board type */

/* you must implement the macros for your board */
#define NO_BUILD_MANPRF 1

#endif	/* board type */

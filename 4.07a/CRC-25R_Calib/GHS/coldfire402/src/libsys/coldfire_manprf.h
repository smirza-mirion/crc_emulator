/***
 * ColdFire timer support.
 * Set SYSTEM_CLOCK to the appropriate value.
 * Check that the memory mapped register positions are correct
 * for your board.
 * If your code makes use of MBAR or ACR0, modify the TIMER_INIT
 * code appropriately.
 * Make sure no other interrupts use the same priority and level
 * as the timer interrupt.
 *
 * Recompile libsys with the appropriate cpu selected.
 */

#define MBARx	0x10000000

/* 
   M5407C3	50Mhz
   M5307C3	45Mhz
   M5272C3	66Mhz
   M5249C3	70Mhz
*/
#define SYSTEM_CLOCK (50000000) /* 50Mhz timer clock */


#if defined(__MCF5407) || defined(__MCF5307)

#define TIMER0	(MBARx+0x140)  	/* timer0 base */
#define ICR1 	(MBARx+0x04D)  	/* timer0 interrupt control register */
#define IMR	(MBARx+0x044)	/* interrupt mask register */

#elif defined(__MCF5272)

#define TIMER0	(MBARx+0x200)
#define ICR1	(MBARx+0x020)	/* external interrupts 1-4 and timers */

#else /* older ColdFire V2 cores */

#define TIMER0	(MBARx+0x100)	/* timer 0 base */
#define ICR1 	(MBARx+0x01C)  	/* timer0 interrupt control register */
#define IMR	(MBARx+0x04D)	/* interrupt mask register */

#endif


typedef unsigned int   uint32;
typedef	unsigned short uint16;
typedef unsigned char  uint8;

struct __ghs_coldfire_timer
{
    uint16 tmr; /* timer mode register */
    uint16 pad0;
    uint16 trr; /* timer reference register */
    uint16 pad1;
    uint16 tcr; /* timer capture register */
    uint16 pad2;
    uint16 tcn; /* timer counter register */
    uint16 pad3;
    uint8 pad4;
    uint8 ter; /* timer event register */    
};


/* TICKS_PER_SEC: on some boards you may be able to
 *  read/compute this value (implementing as an 'asm' macro).
 * Otherwise, you'll want to set this value to the number of times
 *  the timer register increments per second, as we have done here.*/
/* We scale the timer source by 256 to compensate for 16 bit timers */
#define TICKS_PER_SEC()		(SYSTEM_CLOCK/256)


uint32 __ghs_exception_pc;

/* Wrapper for __ghs_manprf_timer_handler so we can
   disable interrupts and store the exception pc/sr
 */
#pragma asm
__ghs_manprf_timer_coldfire_handler:
         MOVE.W #0x2700,%SR   ; disable interrupts 
	 MOVE.L %D0,-(%A7)    ; save D0
         ; save exception info to global variables 
	 MOVE.L 8(%A7),%D0
	 MOVE.L %D0,__ghs_exception_pc
	 MOVE.L (%A7)+,%D0    ;restore D0 and A7
	 JMP __ghs_manprf_timer_handler
#pragma endasm

	 
#if defined(__MCF5272)

/* locate the interrupt handler at the correct exception vector */	 
#pragma asm	 
	 ; position this handler at autovector 5
	 ORG 0x114
	 DC.L __ghs_manprf_timer_coldfire_handler
	 previous
#pragma endasm

	 
static void setup_timer_interrupt()
{
    volatile uint32 *icr1 = (uint32*)ICR1; 
    
    /* enable timer interrupts with priority 6*/    
    *icr1 |= 0xe000;
}

#else /* chips that use autovectored timer interrupts */
	 
/* locate the interrupt handler at the correct exception vector */	 
#pragma asm	 
	 ; position this handler at autovector 5
	 ORG 0x74
	 DC.L __ghs_manprf_timer_coldfire_handler
	 previous
#pragma endasm

	 
static void setup_timer_interrupt()
{
    volatile uint8 *icr1 = (uint8*)ICR1; /* interrupt controller for timer0 */
    volatile uint32 *imr = (uint32*)IMR; /* interrupt mask register */
    
    /* give timer0 level 5, priority 3 (autovector at 5) */
    *icr1 = 0x80 | 0x17;
    /* unmask the timer0 interrupt */
    *imr &= ~(0x200);
}

#endif

/* Initialize the timer.  This will overwrite MBAR and ACR0 */
void TIMER_INIT(unsigned int count)
{
    volatile struct __ghs_coldfire_timer *timer =
	(struct __ghs_coldfire_timer*)(TIMER0);
     
    __MOVEC_MBAR(MBARx|0x1);
    __MOVEC_ACR0(MBARx|0xc040); /* do not cache memory mapped area */

    /* reset/disable timer */
    timer->tmr = 0x0000;
    timer->tcn = 0;
    timer->trr = count;

    setup_timer_interrupt();

    /* set and enable tmr0 */
    timer->tmr = 0xff13;
}

/* Enable interrupts. in particular, set MSR_EE=1 to enable
 *  the decrementer interrupts (note: enables all external
 *  interrupts) */
#define ENABLE_TIMER_INTERRUPT __EI

/* Return the exception PC */
uint32 GET_EPC(void)
{
    return __ghs_exception_pc;
}

/* Clear the timer interrupt and reset the timer
   Interrupts will be enabled with RTE restores the status register.
   */
void CLEAR_TIMER_INTERRUPT()
{
    volatile struct __ghs_coldfire_timer *timer =
	(struct __ghs_coldfire_timer*)(TIMER0);

    timer->ter = 0x3; /* write event bits to clear */
    timer->tcn = 0x0;
}

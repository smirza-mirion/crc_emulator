/****
 *  Tested on Malta 4kc and IDT 79eb355
 *   but this implemenation should work on most Mips targets.
 *  Non-standard items:
 *     1. timer fed back on int7 (though this is traditional)
 *     2. TICKS_PER_SEC()
 */

#if defined(__CORE1__) || defined(__mips16)

#define NO_BUILD_MANPRF 1

#else

/* setup the interrupt vector 
 *  - r27 dedicated to interrupts 
 *  - this will not work under PIC: could fixup address
 *     at runtime, or place handler in ABS section */
#pragma asm
.org 0x80000180
la $27, __ghs_manprf_timer_handler
jr $27
nop
nop
nop
#pragma endasm

/* TICKS_PER_SEC: on some boards you may be able to
 *  read/compute this value (implementing as an 'asm' macro).
 * Otherwise, you'll want to set this value to the number of times
 *  the timer register increments per second, as we have done here.*/
#define TICKS_PER_SEC()		200000

/* Initialize the count and compare registers. */
asm void TIMER_INIT(unsigned int compare)
{
% reg counter
    /* clear the count */
    mtc0 $0, $9
    /* set the compare */
    mtc0 compare, $11

% error	
}

/* Enable the timer interrupt */
asm void ENABLE_TIMER_INTERRUPT(void)
{
    /* read the status register */
    mfc0 $3, $12
    nop
    nop
    /* set the timer interrupt mask and enable interrupt bit */
    /* assumes timer on HW INT 7 */
    ori $3, $3, 0x8001
    /* move to the status register */
    mtc0 $3, $12
}

/* Read the exception PC. */
asm unsigned int GET_EPC(void)
{
    /* move from EPC */
    mfc0 $2, $14
    nop
    nop
}

/* Clear the timer interrupt. */
asm void CLEAR_TIMER_INTERRUPT(void)
{
    /* on MIPS, done by SET_TIMER.
     *  EXL cleared on return from interrupt */
}

#endif

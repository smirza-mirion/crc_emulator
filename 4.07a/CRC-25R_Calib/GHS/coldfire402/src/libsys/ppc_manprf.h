/***
 * Tested on RPX classic.
 *  but should work on most PowerPC implemenations.
 *  (implemenation defined behavior: TICKS_PER_SEC)
 */

/* setup the interrupt vector: timer interrupt */
#pragma intvect __ghs_manprf_timer_handler 0x00000900

/* TICKS_PER_SEC: on some boards you may be able to
 *  read/compute this value (implementing as an 'asm' macro).
 * Otherwise, you'll want to set this value to the number of times
 *  the timer register increments per second, as we have done here.*/
#define TICKS_PER_SEC()		500000

/* Initialize the decrementer register. */
asm void TIMER_INIT(unsigned int count)
{
% reg count
    mtdec count

% error	
}

/* Enable interrupts. in particular, set MSR_EE=1 to enable
 *  the decrementer interrupts (note: enables all external
 *  interrupts) */
#define ENABLE_TIMER_INTERRUPT __EI

/* SRR0 stores the address of the next instruction to execute.
 *  Use that value as the sample. */
asm unsigned int GET_EPC(void)
{
    mfsrr0 r3    
}

#define CLEAR_TIMER_INTERRUPT()
    /* PowerPC interrupt cleared on return */

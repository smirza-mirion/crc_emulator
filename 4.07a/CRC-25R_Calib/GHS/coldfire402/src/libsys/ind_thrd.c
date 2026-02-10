#include "ind_thrd.h"
#include <setjmp.h>

#if !defined (__OSE)
/* GlobalTLS is a global structure to use for thread local storage if we      */
/* don't have multi-threading.  To use the multi-threaded capability of the   */
/* libraries this function must be overridden by another definition which     */
/* depends on the OS being used.					      */
ThreadLocalStorage GlobalTLS;

/* Return pointer to thread local storage */
ThreadLocalStorage *GetThreadLocalStorage()
{
    return &GlobalTLS;
}

#ifndef MINIMAL_STARTUP
/* Acquire general lock.  Blocks until the lock becomes available. */
void __ghsLock(void)
{
}

/* Release general lock */
void __ghsUnlock(void)
{
}
#endif

/* A callback to initialize the lock data structure before it is used. */
void __gh_lock_init(void)
{
}

/* Save arbitrary state across a setjmp() */
int __ghs_SaveSignalContext(jmp_buf jmpbuf)
{
    return 0;
}

/* Restore arbitrary state across a longjmp() */
void __ghs_RestoreSignalContext(jmp_buf jmpbuf)
{
}

#ifndef __disable_thread_safe_extensions

/* Must get called after __cpp_except_init() is called to allocate 
 * and initialize the per-thread exception handling structure */
void *__get_eh_globals(void)
{
    return GlobalTLS.__eh_globals;
}

#endif /* __disable_thread_safe_extensions */

/* Acquire lock for FILE *addr */
void __ghs_flock_file(void *addr) {}

/* Release lock for FILE *addr */
void __ghs_funlock_file(void *addr) {}

/* Non blocking acquire lock for FILE *addr.  May return -1 if */ 
/* this cannot be implemented. Returns 0 on success and nonzero. */
int __ghs_ftrylock_file(void *addr) { return -1; }

/* Callbacks to initialize local lock data structures before they */ 
/* are used. */
void __ghs_flock_create(void **addr) {}
void __ghs_flock_destroy(void *addr) {}

#else /* __OSE */


#include "ose.h"

static SEMAPHORE lock_sem = { 1 };

void __ghsLock(void)
{
	wait_sem(&lock_sem);
}

void __ghsUnlock(void)
{
	signal_sem(&lock_sem);
}

int __ghs_SaveSignalContext(jmp_buf jmpbuf)
{
    return 0;
    /* Remove compiler warning */
    (void) jmpbuf;
}

void __ghs_RestoreSignalContext(jmp_buf jmpbuf)
{
    /* Remove compiler warning */
    (void) jmpbuf;
}

void __gh_lock_init(void)
{
}

#endif /* !__OSE */

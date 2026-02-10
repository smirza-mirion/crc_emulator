#ifndef _THREAD_H_
#define _THREAD_H_

#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <setjmp.h>

#if !defined(_SIGMAX)
#  if defined(SOLARIS20)
#    define _SIGMAX MAXSIG
#  elif defined(LINUX) || defined(MSW) || defined(__LYNX)
#    define _SIGMAX (NSIG-1)
#  endif
#endif

typedef void (*SignalHandler)(int);

typedef struct
{
	int			Errno;
	SignalHandler 		SignalHandlers[_SIGMAX];
	char			tmpnam_space[L_tmpnam];
	char			asctime_buff[30];
	char			*strtok_saved_pos;
	struct tm		gmtime_temp;
	void 			*__eh_globals;
} ThreadLocalStorage;

/* Return pointer to thread local storage */
ThreadLocalStorage *GetThreadLocalStorage(void);

/* Acquire general lock.  Blocks until the lock becomes available. */
void __ghsLock(void);

/* Release general lock */
void __ghsUnlock(void);

#if (!defined (EMBEDDED) && !defined (__OSE) && !defined (MSW)) || defined(MINIMAL_STARTUP)
__inline void __ghsLock(void) { }
__inline void __ghsUnlock(void) { }
#endif /* !EMBEDDED && !__OSE && !MSW */

/* Save arbitrary state across a setjmp() */
int  __ghs_SaveSignalContext(jmp_buf);

/* Restore arbitrary state across a longjmp() */
void __ghs_RestoreSignalContext(jmp_buf);

#define GHSLOCK		__ghsLock();
#define GHSUNLOCK	__ghsUnlock();

/* macros used in stdio library source */
#ifndef __disable_thread_safe_extensions
# define LOCKFILE(f)	flockfile(f);
# define TRYLOCKFILE(f)	ftrylockfile(f);
# define UNLOCKFILE(f)	funlockfile(f);
# define LOCKCREATE(f)	flockcreate(f);
# define LOCKCLEANUP(f)	flockdestroy(f);
# define LOCKINUSE(f)   __ghs_flock_in_use(f)
/* prototypes for FILE lock routines (not in POSIX API) */

void ** __ghs_flock_ptr(void *);

/* Acquire lock for FILE *addr */
void __ghs_flock_file(void *);

/* Release lock for FILE *addr */
void __ghs_funlock_file(void *);

/* Non blocking acquire lock for FILE *addr.  May return -1 if */ 
/* this cannot be implemented. Returns 0 on success and nonzero. */
int __ghs_ftrylock_file(void *);

int __ghs_flock_in_use(void *);

/* Callbacks to initialize local lock data structures before they */ 
/* are used. */
void __ghs_flock_create(void **);
void __ghs_flock_destroy(void *);
#else
# define LOCKFILE(f)
# define TRYLOCKFILE(f)	-1;	/* no lock obtained */
# define UNLOCKFILE(f)
# define LOCKCREATE(f)	
# define LOCKCLEANUP(f)
# define LOCKINUSE(f) 0
#endif
/* prototypes for FILE lock routines (not in POSIX API) */
void flockcreate(FILE *stream);
void flockdestroy(FILE *stream);
/* End New */

#ifdef __Declare_Initialization_Routines__
#pragma weak __gh_signal_init
void __gh_signal_init(ThreadLocalStorage *);
#pragma weak __gh_iob_init
extern void __gh_iob_init(void);
#pragma weak __gh_error_init
extern void __gh_error_init(void);
#pragma weak __gh_lock_init
/* A callback to initialize the lock data structure before it is used. */
extern void __gh_lock_init(void);
#if defined(__OSE)
#else
#pragma weak __cpp_exception_init
#endif /* __OSE */
extern void __cpp_exception_init(void **);
#if defined(__OSE)
#else
#pragma weak __cpp_exception_cleanup
#endif /* __OSE */
extern void __cpp_exception_cleanup(void **);
#endif /* __Declare_Initialization_Routines__ */

#endif /* _THREAD_H_ */


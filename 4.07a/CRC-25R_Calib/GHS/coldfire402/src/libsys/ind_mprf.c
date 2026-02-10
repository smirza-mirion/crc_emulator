/*
		    ANSI C Runtime Library
	
	Copyright 1983-2001 Green Hills Software, Inc.

    This program is the property of Green Hills Software, Inc,
    its contents are proprietary information and no part of it
    is to be disclosed to anyone except employees of Green Hills
    Software, Inc., or as agreed in writing signed by the President
    of Green Hills Software, Inc.
*/
/* ind_mprf.c: Machine Independent Call Count Profiling module. */

#include "inddef.h"
#include "ind_exit.h"
#include "ind_heap.h"
#include "ind_io.h"
#include "ind_thrd.h"
#include "indos.h"

#if defined(__ZSP)
#define int32 long
#define MSPACE_ZSP_TEXT 33L
#else
#define int32 int
#endif

/* This is the number of CNT structures we will allocate at one time. */
/* When we use up all the current CNT structures, we will allocate another */
/* set, and link them together in a linked list. */
#if defined(__V850) && (__PTR_BIT==16)
#define CNTCHUNKSIZE 128
#else
#define CNTCHUNKSIZE 1024
#endif

typedef struct {
    int32 pc;	        /* This holds the pc of the function. */
    int32 *countPtr;	/* This points to the call count of the function. */
} CNT;

typedef struct CNT_List {
    struct CNT_List *next;	/* Pointer to the next list node. */
    int numUsed;		/* The number of array elements used. */
    CNT counts[CNTCHUNKSIZE];	/* The array of count information. */
} CNT_List;

/* [Jimmy] cntHead points at the head of our counts list.  cntTail points at
 * the tail of the list, since we will insert at the end.  (We could insert
 * at the beginning, but that will cause the output to appear in reverse order,
 * which is different than the previous behavior, so I kept it in order).
 * cntFreeList holds all the CNT_List structures that are no longer in use.
 * This only happens when the list is cleared, or when we dump the list.  When
 * we dump the list, we allocate an extra CNT_List structure to help us make
 * less calls to write().
 */
static CNT_List *__cntHead__, *__cntTail__, *__cntFreeList__;
#ifdef __disable_thread_safe_extensions
static int in_mcount;
#endif

#define W_ERROR(s) write(2, s, sizeof(s)-1);
#define P_ERROR(msg) W_ERROR(msg)

/**
 * Clears the call count information.  This can be used as a command line
 * procedure call from MULTI so that you can get the call counts for a
 * specific interval in a run of a program.  Just stop the program, clear
 * the data, run to another point, and dump.
 *
 * @author	Jimmy
 */
void __ghs_prof_clear_callcounts(void)
{
    CNT_List *p, *next;
    int i;

    for (p = __cntHead__; p; p = next) {
	next = p->next;

	/* Clear all the counts to 0 for this set.  For good measure, also
	 * set the pc's to 0. */
	for (i=0; i<p->numUsed; i++) {
	    p->counts[i].pc = 0;
	    *(p->counts[i].countPtr) = 0;
	}

	/* Now free this set by putting it on the free list. */
	p->next = __cntFreeList__;
	__cntFreeList__ = p;
    }
    __cntHead__ = 0;
    __cntTail__ = 0;
}

/**
 * Dumps the call count information.  This function used to be written
 * as if it were only called once.  It trashed the data structures it used,
 * and also locked up mcount by setting in_mcount and not unsetting it.
 * But now, MULTI may call this function when it does a profdump, so I fixed
 * it to behave properly.  And I also cleared out all the obfuscated code in
 * here.  You should have seen this function before I rewrote it.
 *
 * @author	Jimmy
 */
void __ghs_prof_dump_callcounts(void)
{
    int32 h[3];		/* lpc, hpc, count */
    int32 fd, totalUsed;
    static const char monout[] = "mon.out";
    CNT_List *p;

    /* creat() and write() may have been compiled for profiling */
#ifdef __disable_thread_safe_extensions
    in_mcount = 1;
#endif

    for (totalUsed = 0, p = __cntHead__; p; p = p->next)
	totalUsed += p->numUsed;

    /* construct header for mon.out file */
    h[0] = h[1] = 0;		/* no stochastic samples */
    h[2] = totalUsed;

    /* Write out the file. Walk the list of buffer chunks and write each one. */
    
    /* [tsay] Wed Feb  6 19:44:34 2002 
     * The following line for ZSP should work on all targets but I don't have
     * time to test it right now.
     */
#if defined(__ZSP)    
    if ((fd = open(monout, O_CREAT | O_TRUNC | O_WRONLY | 0x8000, 0660)) < 0) {
#else	
    if ((fd = creat(monout, 0660)) < 0) {
#endif	
        P_ERROR("mcount: could not create mon.out\n");
    } else {
	static const char writefailed[] = "mcount: could not write mon.out\n";
	if (write(fd, h, sizeof(h)) != sizeof(h)) {
	    W_ERROR(writefailed);
	} else if (totalUsed > 0) {
	    CNT_List *temp;
	    int i, len;

	    /* We need to allocate one CNT_List structure so that we can */
	    /* write our buffers out 1024 at a time instead of 1 at a time. */
	    if (__cntFreeList__) {
		temp = __cntFreeList__;
		__cntFreeList__ = __cntFreeList__->next;
	    } else {
		temp = (CNT_List *) __ghs_calloc(sizeof(CNT_List), 16);
	    }

	    if (!temp) {
		P_ERROR(
		    "__ghs_prof_dump_callcounts: Could not allocate memory.\n");
		return;
	    }

	    /* Now traverse our list of call count sets, and write */
	    /* them out one set of 1024 at a time.  We will copy the */
	    /* sets to temp, with the modification that instead of */
	    /* the pointers to the count, we will fill temp with the */
	    /* actual counts instead. */
	    for (p = __cntHead__; p; p = p->next) {
		for (i=0;i<p->numUsed;i++) {
		    
#if defined(__ZSP_400) || defined(__ZSP_402ZX) || defined(__ZSP_403X)
		    /* Encode the pc with the external memory space for text
		     * on the ZSP chips that have it.
		     */
		    temp->counts[i].pc = ((MSPACE_ZSP_TEXT << 16) |
			                  p->counts[i].pc);
#else		    
		    temp->counts[i].pc = p->counts[i].pc;
#endif		    
#if defined(__PTR_BIT) && __PTR_BIT < 32 && __PTR_BIT == __SHRT_BIT
		    temp->counts[i].countPtr = (int32 *)(short)(*p->counts[i].countPtr);
#else
		    temp->counts[i].countPtr = (int32 *)(*p->counts[i].countPtr);
#endif
		}
		len = p->numUsed * sizeof(CNT);
		/* Write out up to 1024 at a time. */
		if (write(fd, (void *) temp->counts, len) != len) {
		    W_ERROR(writefailed);
		    break;
		}
	    }

	    /* Put temp on the free list, since we are done with it. */
	    temp->next = __cntFreeList__;
	    __cntFreeList__ = temp;
	}
	close(fd);
    }
#ifdef __disable_thread_safe_extensions
    in_mcount = 0;
#endif
}

/**
 * Updates the call count information.
 *
 * @param	area	A pointer to a memory location which holds the
 *			call count for this function.  This location should
 *			hold 0 if this is the first time the function has
 *			been called.
 * @param	archead	The address of the function.
 * @author	Jimmy
 */
void __ghs_indmcount(int32 *area, void (*archead)())
{
/* DAVEK remove the infinite recursion check which keeps threads from
 * legitimately getting in */
    static int nomemory;
#ifdef __disable_thread_safe_extensions
    /* This is not thread-safe; our intent is to prevent infinite recursion. */
    if (in_mcount)
	return;
    in_mcount = 1;
#endif

    if (nomemory)
	return;
    /* If the count is 0, that means we have not added this function to
     * our list of counts.  Otherwise we already have so all we have to do
     * is increment the count. */
    if (!*area) {
	/* DAVEK: add locking when changing prof structure only */
	__ghsLock();
	/* We need to add this function to our list.  First check if there
	 * are any unused slots in cntTail. */
	if (!__cntTail__ || __cntTail__->numUsed == CNTCHUNKSIZE) {
	    /* We need to allocate a new set of CNT structures.  Either this
	     * is the first time, or we used up last of the previous set. */
	    CNT_List *p;

	    /* Try to get one off the free list first, otherwise allocate one */
	    if (__cntFreeList__) {
		p = __cntFreeList__;
		__cntFreeList__ = __cntFreeList__->next;
		p->next = 0;
		p->numUsed = 0;
	    } else
		p = (CNT_List *) __ghs_calloc(sizeof(CNT_List), 16);
	    
	    /* Check for any errors. */
	    if (!p) {
		P_ERROR("mcount: could not allocate memory\n");
		__ghsUnlock();
		/* DAVEK: add locking when changing prof structure only */
		nomemory = 1;
		return;		/* LEAVE in_mcount SET, we're inactive now. */
	    } else {
		static struct __GHS_AT_EXIT gae;
		if (!gae.func) {
		    gae.func = __ghs_prof_dump_callcounts;
		    __ghs_at_exit(&gae);
		}
	    }

	    /* Now insert the new set at the end of the list. */
	    if (__cntTail__) {
		__cntTail__->next = p;
		__cntTail__ = p;
	    } else {
		__cntHead__ = __cntTail__ = p;
	    }
	}
	/* Add the function to our list. */
	__cntTail__->counts[__cntTail__->numUsed].pc = (int32) archead;
	__cntTail__->counts[__cntTail__->numUsed++].countPtr = (int32 *) area;
	/* DAVEK: add locking when changing prof structure only */
	__ghsUnlock();
    }
    ++*area;

#ifdef __disable_thread_safe_extensions
    in_mcount = 0;
#endif
}

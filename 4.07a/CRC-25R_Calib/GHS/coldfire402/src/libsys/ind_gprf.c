/*
		    ANSI C Runtime Library
	
	Copyright 1983-2001 Green Hills Software, Inc.

    This program is the property of Green Hills Software, Inc,
    its contents are proprietary information and no part of it
    is to be disclosed to anyone except employees of Green Hills
    Software, Inc., or as agreed in writing signed by the President
    of Green Hills Software, Inc.
*/
/* ind_gprf.c: Machine Independent Call Graph Profiling module. */

#include "inddef.h"
#include "ind_crt1.h"
#include "ind_io.h"
#include "ind_exit.h"
#include "ind_heap.h"
#include "indos.h" /* For O_CREAT etc. */

#include <string.h> /* for memset() */

/*[TPW] Do only .text for the moment, need to add .secinfo driven code. */
extern void __ghsbegin_text(void), __ghsend_text(void);

/* ptevis Fri Jun 22 17:31:23 2001
   We can't use the int32 defined in ind32bit.h because src/shared is not shipped, so
   in order to make this work we have duplicate the code here. This is a very bad idea, and I
   wish we would fix it.
 */
#if defined(__ZSP)
#define MSPACE_ZSP_TEXT 33L
#define int32 long
#else
#define int32 int
#endif

typedef struct {
    int32 pc;
    int32 count;
} CNT;

static CNT *__arcbuf__;
static int32 narcs; 
#ifdef __disable_thread_safe_extensions
static int32 in_gcount;
#endif

/*[TPW] ARCBUCKETSIZE is the number of text bytes per arc bucket, and should be
	equal to the minimum spacing between subroutine calls for a given
	architecture.

	The 68K value is 4, which is true for compiler generated code, as the
	smallest subroutine call will be a BSR.W which is 4 bytes. It is
	physically possible to have 2 byte spaced subroutine calls, by using
	BSR.S in assembly modules to call compiler generated subroutines, but
	profiling is bound to get mighty confused in such a situation anyway.
*/
#if (defined(__FR20) || defined(__MCORE__))
#define ARCBUCKETSIZE 2
#else
#define ARCBUCKETSIZE 4
#endif

#define W_ERROR(s) write(2, s, sizeof(s)-1);
#define P_ERROR(msg) W_ERROR(msg)

/**
 * Clears the call graph information.  This can be used as a command line
 * procedure call from MULTI so that you can get the call graph for a
 * specific interval in a run of a program.  Just stop the program, clear
 * the data, run to another point, and dump.
 *
 * @author	Jimmy
 */
void __ghs_prof_clear_callgraph(void)
{
    if (__arcbuf__)
	memset((char *) __arcbuf__, 0, narcs * sizeof(CNT));
}

/* the "count" field in the gmon.out header appears to really be the offset
	into the file at which the arc buckets start. It works for me...
*/
/* leave this routine global for proc call from MULTI */
void __ghs_prof_dump_callgraph(void)
{
    int32 fd;
    int32 h[3];		/* lpc, hpc, count */
    static const char gmonout[] = "gmon.out";

#ifdef __disable_thread_safe_extensions
    in_gcount = 1;		/* disable profiling */
#endif

    /* First we write a header: begin_addr, end_addr, sizeof(mcount bucket) */

    h[0] = h[1] = 0;		/* no sampling information */

    /* [tsay] Thu Feb  7 13:52:37 2002 
     * This is supposed to be the size on the host! For now, set it equal
     * to 12 on ZSP since all hosts are 32-bits, and we write 3 32-bit numbers
     * so that is 3 * 4 bytes = 12 bytes.
     */
#if defined(__ZSP)
    h[2] = 12;
#else
    h[2] = sizeof(h);
#endif    

    /* [tsay] Wed Feb  6 19:07:57 2002
     * Putting in ptevis's change from libsrc/ind_gprf.c. The line for ZSP
     * should work on all targets but I don't have time to test it right now.     
     */
#if defined(__ZSP)
    if ((fd = open(gmonout, O_TRUNC | O_WRONLY | O_CREAT | 0x8000 , 0660)) < 0) {
#else    
    if ((fd = creat(gmonout, 0660)) < 0) {
#endif	
	P_ERROR("gcount: could not create gmon.out\n");
    } else {
        static const char writefailed[] = "gcount: could not write gmon.out\n";
	if (write(fd, h, sizeof(h)) != sizeof(h)) {
	    W_ERROR(writefailed);
	} else {
	    CNT *p = __arcbuf__;
	    int32 n = narcs;
	    unsigned int32 r[3];		/* frompc, selfpc, count */

#if defined(__ZSP_400) || defined(__ZSP_402ZX) || defined(__ZSP_403Z)
	    /* Encode external memory space identified only if we have a chip
	     * that supports it.
	     */
	    r[0] = (unsigned int32) ((MSPACE_ZSP_TEXT << 16) | (unsigned int32) __ghsbegin_text);
#else
	    r[0] = (unsigned int32) __ghsbegin_text;
#endif
	    
	    for ( ; n--; r[0] += ARCBUCKETSIZE, ++p) {
		unsigned long l;
		if (0 == (l = p->count))
		    continue;
		r[2] = l;
		r[1] = (unsigned int32) p->pc;
		if (write(fd, r, sizeof(r)) != sizeof(r)) {
		    W_ERROR(writefailed);
		    break;
		}
	    }
	}
	close(fd);
    }
    /* ideally should free the buffers, but __ghs_alloc() doesn't allow that. */
    /* do not clear in_gcount, because we are finished profiling. */
}

#define UDIVBYARCBS(x) ((unsigned long)(x)/ARCBUCKETSIZE)

/* archead should be the address of the caller of __ghs_gcount.
 * arctail should be the address of the address of the caller of the caller of
 * __ghs_gcount().
 * Well, actually the "addresses of the caller" refer to the return addresses
 * passed to the callee.
 * The following code example, although spurious, illustrates what is going on.
 *
 * void foo() {
 *   ack()
 * after_foo:
 * }
 *
 * void ack() {
 *   __ghs_gcount(ack's return pointer);
 * after_ack:
 * }
 *
 * void __ghs_gcount(void *caller) {
 *   __ghs_indgcount(__ghs_gcount's caller's return address, &caller);
 * }
 *
 * Therefore the above sequence is equivalent to
 * __ghs_gcount(after_foo) ->
 * __ghs_indgcount(after_ack, &after_foo)
 * 
 */ 
void __ghs_indgcount(void (*archead)(void), void (**parctail)(void))
{
    unsigned int32 arcs;
    CNT *this;
    static int32 nomemory;

#ifdef __disable_thread_safe_extensions
    if (in_gcount)
	return;
    in_gcount = 1;
#endif
    if (nomemory)
	return;

    if (!__arcbuf__) {
	static struct __GHS_AT_EXIT gae;
	/* round up to make sure we have enough buckets */
	narcs = UDIVBYARCBS((char *)__ghsend_text - (char *)__ghsbegin_text
			+ ARCBUCKETSIZE-1);
	__arcbuf__ = (CNT *) __ghs_calloc(sizeof(CNT)*narcs, 16);
	if (!__arcbuf__) {
	    nomemory = 1;
	    P_ERROR("gcount: could not allocate memory\n");
	    return;		/* LEAVE in_gcount SET, we're inactive now. */
	} else {
	    if (!gae.func) {
		gae.func = __ghs_prof_dump_callgraph;
		__ghs_at_exit(&gae);
	    }
	}
    }
    this = __arcbuf__;
    /* [Jiong] Thu Jun 17 10:40:31 PDT 1999 - backup Adac's change for NDR.
     * On NDR, address space is 0 to 0x00ffffff. 
     * New code: */
#if defined(__NDR)
    arcs = UDIVBYARCBS((*parctail - __ghsbegin_text) & 0x00ffffff);
#else
    arcs = UDIVBYARCBS((char *)*parctail - (char *)__ghsbegin_text);	/* round down */
#endif
    if (arcs >= narcs)
	return;		/* avoid trashing memory in multiple section case */
    this += arcs;
    this->count++;
#if defined(__ZSP_400) || defined(__ZSP_402ZX) || defined(__ZSP_403Z)
    /* The MULTI uses a 32-bit wrapper for the ZSP's 16-bit addresses to
       disambiguate multiple memory spaces. However, we can't tell here
       which space we are in, so we assume internal text memory.
     */
    this->pc = (MSPACE_ZSP_TEXT << 16) | ( (int32) archead);
#else
    this->pc = (int32) archead;
#endif    

#ifdef __disable_thread_safe_extensions
    in_gcount = 0;
#endif
}

#undef int32

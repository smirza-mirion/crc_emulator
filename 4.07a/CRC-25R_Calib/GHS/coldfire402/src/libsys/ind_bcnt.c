/*
 *            Debugger
 *
 *    Copyright 1990-2001 Green Hills Software, Inc.
 *
 *    This program is the property of Green Hills Software, Inc,
 *    its contents are proprietary information and no part of it
 *    is to be disclosed to anyone except employees of Green Hills
 *    Software, Inc., or as agreed in writing signed by the President
 *    of Green Hills Software, Inc.
 */
/* ind_bcnt.c: compiler internal functions for Block Coverage Profiling. */

#if defined(EMBEDDED) || defined(__OSE)

#include "indos.h"
#include "ind_io.h"
#include "ind_exit.h"
#include "ind_thrd.h"

#if defined(__ZSP)
#define int32 long
#define MSPACE_ZSP_TEXT 33L
#else
#define int32 int
#endif

#if defined(__ZSP)
#pragma pack(1)
#endif
struct blk { void (*addr)(); unsigned int32 cnt; };
struct bhdr { int32 len; struct blk *info; int32 inserted; struct bhdr *next; };
static struct bhdr *blocklisthead;

/**
 * Clears the coverage information.  This can be used as a command line
 * procedure call from MULTI so that you can get the coverage for a
 * specific interval in a run of a program.  Just stop the program, clear
 * the data, run to another point, and dump.
 *
 * @author	Jimmy
 */
void __ghs_prof_clear_coverage(void)
{
    int i;
    struct bhdr *p, *next;

    for (p = blocklisthead; p; p = next) {
	next = p->next;
	p->next = 0;
        p->inserted = 0;
	/* reset all counts except the last (-1 marker) to 0 */
	for (i = 0; i < p->len - 1; i++)
	    p->info[i].cnt = 0;
    }
    blocklisthead = NULL;
}

/* leave this routine global for proc call from MULTI */
void __ghs_prof_dump_coverage(void) 
{
    int fd;
    struct bhdr *test;

    /* [tsay] Wed Feb  6 18:42:27 2002
     * I need to put this library change in quick. The line for ZSP should
     * work for all targets (ptevis added it before in libsrc), but just to
     * be safe, use the old line for targets other than ZSP.
     */
#if defined(__ZSP)
    fd = open("bmon.out", O_CREAT | O_WRONLY | O_TRUNC | 0x8000, 0644);    
#else    
    fd = creat("bmon.out", 0644);
#endif
    
    if (fd == -1) {
	static const char creatfailed[] = "bcount: could not create bmon.out\n";
	write(2, creatfailed, sizeof(creatfailed)-1);
	return;
    }
    for (test = blocklisthead; test; test = test->next) {
	static const char writefailed[] = "bcount: write failed for bmon.out\n";
#if defined(alpha) || defined(__alpha)
/* special processing for long pointers on Alpha */
	unsigned int i = 0, u = 0;
	unsigned int us[16*2];		/* must be even number of elements */
	struct blk *b = test->info;
	while (i < test->len) {
	    unsigned int uc;
	    us[u++] = (unsigned int) b->addr;
	    us[u++] = b->cnt;
	    ++b, ++i;
	    uc = sizeof(us[0])*u;
	    if ((i == test->len || (sizeof(us) == uc)) &&
			write(fd, us, uc) != uc) {
		write(2, writefailed, sizeof(writefailed)-1);
		close(fd);
		return;
	    }
	}
#elif defined(__ZSP)
	{
	    int j = 0;
	    for (j = 0; j < test->len; j++ ) {
		struct blk currBlk = test->info[j];
#if defined(__ZSP_400) || defined(__ZSP_402ZX) || defined(__ZSP_403Z)
		/* Encode the memory space identifier only on chips that
		 * support external memory spaces.
		 */ 
		int32 addr = (MSPACE_ZSP_TEXT << 16 | ((int32) currBlk.addr ));
#else
		int32 addr = ((int32) currBlk.addr);
#endif		
		
		unsigned int32 cnt = (unsigned int32) currBlk.cnt ;
		if (write(fd, &addr, sizeof(int32) )!= sizeof(int32)) {
		    write(2, writefailed, sizeof(writefailed)-1);
		    close(fd);
		    return;
		} else if (write(fd, &cnt, sizeof(unsigned int32)) != sizeof(unsigned int32)) {
		    write(2, writefailed, sizeof(writefailed)-1);
		    close(fd);
		    return;
		}
	    }
	}		
#else
	int cnt = sizeof(struct blk)*test->len;
	if (write(fd, test->info, cnt) != cnt) {
	    write(2, writefailed, sizeof(writefailed)-1);
	    close(fd);
	    return;
	}
#endif
    }
    close(fd);
}

void __ghs_bcount(struct bhdr *routinfo, void (*routaddr)())
{
    __ghsLock();
    if (routinfo->inserted) {
	__ghsUnlock();
	return;
    }
    if (!blocklisthead) {
	static struct __GHS_AT_EXIT gae;
	if (!gae.func) {
	    gae.func = __ghs_prof_dump_coverage;
	    __ghs_at_exit(&gae);
	}
    }

    routinfo->inserted = 1;
    routinfo->next = blocklisthead;
    blocklisthead = routinfo;
    __ghsUnlock();
}

#undef int32

#endif /* defined(EMBEDDED) */

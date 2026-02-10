/*
		    Low Level Interface Library

	Copyright 1983-2000 Green Hills Software,Inc.

 *  This program is the property of Green Hills Software, Inc,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software, Inc., or as agreed in writing signed by the President
 *  of Green Hills Software, Inc.
 *
 *  This header file is #included by all of the files in the Low Level
 *  Interface Library (formerly indOS.c).
 *
 *  The various versions of Unix differ slightly in the content of this file
 *  but they differ greatly in where the information is kept in the local
 *  include files.  In addition, this header file must work correctly with
 *  and without the Green Hills Ansi C header files, which sometimes conflict
 *  with local include files.  Of course, if this is not native Unix, we may
 *  not have any local include files.
 *
 *  Therefore, we support 3 approaches.  In the first, only native header files
 *  are used.  The Green Hills Ansi C header files are not used at all.  This
 *  mode is appropriate for Unix System V.4 and any other Unix like environment
 *  which has been updated to Ansi C.
 *
 *  In the second, no native header files are used.  The Green Hills Ansi C
 *  header files are used for generic information and system specific 
 *  information is provided directly in this file.
 *
 *  In the third, and most difficult situation, both native Unix and Green Hills
 *  Ansi C header files are used.  This invariably results in conflicts.
 *
*/

#ifdef __cplusplus		/* this file is now used in libcxx.a */
extern "C" {
#endif

#include "inddef.h"
#include "inderrno.h"

/* Case 1.  The full set of headers used with -ansi really work, use them.    */
/*	    This is becoming more common as we provide modified headers, and  */
/*	    as Unix begins to provide Ansi compliant headers and libraries.   */
#if defined(LIBCISANSI)||defined(SUNOS)||(defined(ANYSYSV)&&(defined(__m88k)||\
					  defined(__i386_)||defined(__m68k)))
#  ifdef VXWORKS
#    include <vxWorks.h>
#    undef BSD
#    undef MSW
#    define _EXTENSION_POSIX_REENTRANT 1
#  endif
#  include <sys/types.h>
#  include <sys/stat.h>
/* DAVEK: alpha native is compiled -DBSD -DLIBCISANSI.  Need <sys/time.h>
 * for struct timeval used in lib/ind_tmzn.c. Old: 
#if defined(SUNOS)
 * New: */
#if defined(SUNOS) || defined(__alpha) && !defined(VXWORKS)
#  include <sys/time.h>
#endif
#  include <time.h> 
#  include <stdlib.h>
#  include <stddef.h>
#  include <unistd.h>		/* POSIX: close(),lseek(),read(),write() */
#  include <fcntl.h>		/* POSIX: creat(), open() */
#if !defined(CHORUS) && !defined (__OSE)
#  include <sys/times.h>	/* added for fortimes.c */
#endif

/* Case 2.  Only use default headers provided by GHS (/usr/include/ansi) */
#elif defined(CROSSUNIX)||!defined(ANYUNIX)
#    include <stdlib.h>
#    include <time.h>
/*#include <sys/types.h> */
#if defined(ANYBSD)
typedef long unsigned ino_t;	/* BSD and System V.4 */
#else
typedef unsigned short ino_t;	/* System V.3 */
#endif
#if defined(ANYSYSV4)
typedef long dev_t;
#define __misc_t	long
#else
typedef short dev_t;
#define __misc_t	short
#endif

#if !defined(__INTEGRITY)	/* conflict with <sys/types.h> */
typedef long off_t;
#endif

/*#include <sys/stat.h> */
struct stat {
    dev_t	st_dev;
#ifdef SOLARIS20
    long	st_pad[3];
#endif
    ino_t	st_ino;
    __misc_t	dummy1[5];	/* mode, nlink, uid, gid, rdev */
#ifdef SOLARIS20
    long	st_pad2[2];
#endif
    off_t	st_size;
#ifdef SOLARIS20
    long	dummy[19];
#else
    long	dummy[16];
#endif
};

/*#include <sys/time.h>*/
#if !defined(__INTEGRITY)
/* UNIX Structure representing time since epoch*/
struct timeval {
    long tv_sec;		/* seconds */
    long tv_usec;		/* fractional microseconds */
};
#endif

/*#include <sys/times.h>*/
struct tms {
    clock_t tms_utime;		/* user time */
    clock_t tms_stime;		/* system time */
    clock_t tms_cutime;		/* user time of all children */
    clock_t tms_cstime;		/* system time of all children */
};

/* [YZ] Wed Dec 18 11:29:48 PST 1991 added these for cxfilebf.cxx */
/*#include <fcntl.h>*/
/* flags for open */
#define O_RDONLY        0	/* open for reading only */ 
#define O_WRONLY        1	/* open for writing only */
#define O_RDWR          2	/* open for reading or writing */
#define O_APPEND        8 	/* always write at the end of the file	*/ 
/* [JY] Tue Sep 22 09:38:59 PDT 1992. Use non-zero values for CROSSUNIX	*/
/* these values are consistent with numerous SysV and Bsd variants, but */
/* are not implemented in the default stand-alone libind.a routines.	*/
#if defined(__INTEGRITY_SHARED_LIBS) || (defined(ANYBSD) && !defined(ANYSYSV))
/* right for SUNOSv4, Tek, DecStation, ultrix, alpha, NeXT */
#define O_CREAT         0x200	/* create the file if it doesn't exist	*/
#define O_TRUNC         0x400	/* empty the file if in O_WRONLY or O_RDWR */ 
#define O_EXCL          0x800	/* fail if O_CREAT and file exists	*/
#else
/* Right for solaris2, m68030, m88000, AUX, SCO, MIPS, HPUX, avion, sgi  */
#define O_CREAT         0x100	/* create the file if it doesn't exist	*/
#define O_TRUNC         0x200	/* empty the file if in O_WRONLY or O_RDWR */ 
#define O_EXCL          0x400	/* fail if O_CREAT and file exists	*/
#endif
/*[TPW] Wed Apr 13 00:26:56 PDT 1994.
	In ind_stat.c we mention R_OK/W_OK/X_OK/F_OK but we don't provide any
	courtesy definitions of it. However cctempnm.c expects W_OK to have
	been defined; it used to include unistd.h which on sun4 defines W_OK,
	but no longer does. In either case MSW is out of luck unless we make
	sure cctempnm.c is content. Do this by checking R_OK and defining all
	four symbols if it has not been set by anyone else.
** New Code: */
#ifndef R_OK
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0
#endif
/* End of New Code */

/*[TPW] Tue Jun 29 00:53:41 PDT 1993.
 *	Added prototypes for indio.c functions because cxfilebf.cxx needs them
 *	Ed fixed open/creat to actually match the correct args in cxfilebf
 *	JY fixed read/write to match args in unistd.h (void* instead of char*)
 */
/* New Code: */
int close(int);
int creat(const char *, int);
int open(const char *, int, ...);
#ifdef __INTEGRITY_SHARED_LIBS
/* [msantos] Thu Sep 20 2001.
 *   INTEGRITY's version of lseek has off_t defined to be a 64 bit signed
 *   integer.
 */
long long lseek(int, long long, int);
#else
long lseek(int, long, int);
#endif
int read(int, void *, int);
int write(int, const void *, int);
/* End Of New Code */
/* ind_sgnl.c wants to call _exit(); */
void _exit(int);
/* ind_trnc.c wants to call access(), unlink(), and rename() */
/* unlink() is a bit sticky; we would like to use remove() instead, BUT that */
/* is in libansi.a which is being linked in before us (libind.a/libsys.a)... */
int access(const char *, int);
int unlink(const char *);
int rename(const char *, const char *);
/* ccclock.c wants to call times() */
int times(struct tms *);
/* ccexecl.c wants to call execve(). I have evil plans for this */
int execve(const char *, char * const *, char **);

#include <stddef.h>
void *sbrk(size_t);

/* ccmalloc.c needs to call sbrk() */
/* ccmktime.c wants to call __gh_timezone, and ind_time.c does internally */
extern int __gh_timezone(void);
/*[TPW] Thu Jul  8 23:27:57 PDT 1993.
 *	Added indsyscl.h support. Currently this is used by indio.c and
 *	ind_time.c and ind_stat.c
 */
#include "indsyscl.h"
/* [Hal] Fri Jul 30 18:05:46 1993 - SDA needs remote_id to be in .data */
#if defined(__ghs_sda)
/* Make it large enough not to be in SDA, also 64-bit clean */
extern struct { int magic:32; int dummy[2]; } __remote_id;
#define __remote_id __remote_id.magic
#else
extern int __remote_id;		/* defined in crt0 -- default to stand alone */
#endif
/* DAVEK Tue Jan 25 16:42:37 PST 1994 avoid using stdarg on SH - assembly
 * coding of __ghs_syscall is strange/tedious and not worth the trouble */
/* [timr] Thu Oct 18 2001 - pDSP doesn't have 32-bit integers, it's doesn't
 * need this check  */
#if defined(__SH7000) || defined(__NDR) || defined (__pDSP__)
extern int __ghs_syscall();	
#else
#if (__INT_BIT==32)
extern int __ghs_syscall(int, ...);	/* now we never have to worry */
#elif (__LONG_BIT==32)
extern long __ghs_syscall(long, ...);	/* now we never have to worry */
#else
#error A 32-bit data type is necessary for the prototype of __ghs_syscall
#endif
#endif
/* end TPW */

/* Tektronics returns ETOOLONG if a file name is longer than its 256 
 * character limit. This value is mapped to ENOENT.
 */
#if defined(TEKEM)
#define ETOOLONG   63   
#endif

/* Case 3.  Use conflictin headers from /usr/include AND /usr/include/ansi   */
/*	    This case is being phased out.  see case 1 for a better solution */
#else	/* ANYUNIX */
/* 
   There are frequently conflicts between typedefs in GHS (Ansi) headers and
   those in Unix headers.  Now we suppress specific typedefs which we will
   never use when #includeing GHS headers.  Unfortunately, the header files
   themselves need these types, so I #define size_t, etc. for the sake of
   the header files, and #undef them after.
   Also removed all of the #define foo_t __wrong_foo_t lines below.
*/
#    define _SIZE_T	
#    define size_t		unsigned int
#    define _PTRDIFF_T
#    define ptrdiff_t	int
#    define _WCHAR_T
#    define wchar_t		int
/* this was only here for FOPEN_MAX, which is only used in indOS.h.  We 
   can't #include stdio.h here, because GHS libraries are used with different
   implementation of the stdio library.  Moved the whole FOPEN_MAX logic back
   into indOS.c
    %include "indstdio.h"  
*/
#    include <stdlib.h>
#    undef _SIZE_T
#    undef size_t
#    undef _PTRDIFF_T
#    undef ptrdiff_t
#    undef _WCHAR_T
#    undef wchar_t
#    include <sys/types.h>
#    include <sys/stat.h>
/* [JY] Wed Sep 16 16:14:44 PDT 1992. Sure is difficult to mix headers in C++ */
#ifdef __cplusplus
#    define open	__wrong__open__
#    include <fcntl.h>		/* only for cxfilebf.cxx */
#    undef open
#endif	/* __cplusplus */
#    if defined(ANYBSD) && ! defined(NEEDTZSET)
#      define _CLOCK_T
#      define _TIME_T
#      include <sys/time.h>
#      undef _CLOCK_T
#      undef _TIME_T
#    endif		/* ANYBSD and not NEEDTZSET */
#    include <sys/times.h> /* [JY] Fri Jun  4 15:07 1993 added for fortimes.c */
/* [JY] Fri Jul 30 15:44:09 PDT 1993.  ap30 88000 host needs these.  */
    int close(int);
    int creat(const char *, int);
    int open(const char *, int, ...);
    long lseek(int, long, int);
    int read(int, void *, int);
    int write(int, const void *, int);
/* End Of New Code */

#endif	/* ANYUNIX */

#ifndef NULL
#define NULL ((void*)0)
#endif

int __gh_timezone(void); /* [JY] Thu Aug  1 17:55:55 PDT 1996 */

#ifndef CLK_TCK			/* header for this varies between systems */
#define CLK_TCK	60		/* should be sysconf(_SC_CLK_TCK) on POSIX */
#endif
#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC	1000000
#endif

#if defined(__OSE)
struct tms {
    clock_t tms_utime;          /* user time */
    clock_t tms_stime;          /* system time */
    clock_t tms_cutime;         /* user time of all children */
    clock_t tms_cstime;         /* system time of all children */
};
/* ccclock.c wants to call times() */
int times(struct tms *);
/* ccexecl.c wants to call execve(). */
int execve(char *, char **, char **);
/* ccmalloc.c needs to call sbrk() */
void *sbrk(int);
/* ccmktime.c wants to call __gh_timezone, and ind_time.c does internally */
extern int __gh_timezone(void);
#endif /* __OSE */



#ifdef __cplusplus		/* this file is now used in libcxx.a */
}
#endif


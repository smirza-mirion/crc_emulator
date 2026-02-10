/*
		Language Independent Library
    Copyright 1983-2000 Green Hills Software,Inc.

 *  This program is the property of Green Hills Software, Inc,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software, Inc., or as agreed in writing signed by the President
 *  of Green Hills Software, Inc.
*/

/*
    The Green Hills Software Libraries have been ported to many versions of
    Unix and other target environments, each with their major and minor 
    differences and conflicts.

    Every attempt has been made to isolate these differences into indOS.c
    and a few other files.

    At present, the following target environments and flavors of Unix are
    understood:

    Only one of the following should ever be set:
	    BSD                           \
	    DECMIPS                        \
	    DGUX                            \
	    HPUX			     \
	    NEXT			      \
	    SCOUNIX			       \  imply ANYUNIX
	    SOLARIS20                          /
	    SYSV                              /
	    SYSV40                           /
	    SUNOS                           /
	    LINUX			   /
	    ANOTHERUNIX                   /
	    ------------
	    EMBEDDED
	    MSDOS
	    MSW
	    SIMULATE
	    UNIXSYSCALLS
	    VMS
	    VXWORKS
	    CHORUS
	    OSE
	    LYNX
    Variables in the first group are intended to represent various genuine
    Unix systems.  All of these imply the variable 'ANYUNIX', which is used
    to select the most generic features of Unix, such as directories with /
    and system calls like fork(), exec(), link() and unlink().

    The UNIXSYSCALLS variable describes a target environment which resembles
    Unix, but lacks the 'isatty' system call and which does not handle
    signals.  It was originally implemented for the i860 APX coprocessor
    environment, which emulates Unix under MsDos and OS/2.

    The SIMULATE variable describes the GHS simulator environment.  It also
    implements most Unix system calls.
*/
/*  MINIMUM_SPACE uses more compact code which is more appropriate for embedded
    uses.  Often this means dropping code which is only there for Unix 
    compatibility, or which is only relevant when we might use our libansi with
    another vendor's libc.a
    LESS_BUFFERED_IO is a specific space-saving change that makes sense in 
    embedded uses, but actually works on Unix as well.  It is almost always used
    together with MINIMUM_SPACE.  stdin and stdout are unbuffered by default.
    To recover the benefits of bufferings, functions like printf and fwrite
    create their own buffers and flush them before exitting.  Thus a call to
    printf or fwrite will probably only call write() one more time than if
    full buffering was in use.  putc() on the other hand calls write everytime.
*/
/* [JY] Thu Dec 15 15:45:41 PST 1994. make EMBEDDED imply MINIMUM_SPACE and
   LESS_BUFFERED_IO.  This is thoroughly tested on 68k embedded and 68k SVR3 */
/* [JY] Fri Jun 27 10:32:50 PDT 1997.  implement NO_INITIALIZED_DATA */
#if defined(EMBEDDED) && !defined(__INTEGRITY_SHARED_LIBS)
#   define	MINIMUM_SPACE		1
#   define	LESS_BUFFERED_IO	1
#   define	NO_INITIALIZED_DATA	1
#endif

#if defined(__INTEGRITY_SHARED_LIBS)
#   define	UN_BUFFERED_STD		1
#endif
/*
    ANYBSD is any non-System V Unix system which provides these Bsd 4.3 calls:
	rename
	truncate
	gettimeofday
	vfork
	signal/kill
    and this library routine
	on_exit
*/

#ifdef __VXWORKS	/* [JY] Wed Feb  9 16:26:17 PST 1994 vxWorks.h defines BSD */
#undef	BSD
#undef	MSW
#endif
#if defined(BSD) || defined(SUNOS)
#define		ANYBSD
#endif

/*	ANYSYSV4 is any Unix system which is based on Unix System V.4
	1.	the timezone interface is System V and not Bsd.
	2.	the stat struct is enlarged versus either V.3 or Bsd.
	3.	weak symbols are available
	4.	the C library is a full (and correct) Ansi C implementation
	5.	truncate exists
	6.	signals are System V style, not BSD style
*/
#if defined(DGUX) || defined(SOLARIS20) || defined(SYSV40) || defined(HPUX)
#define		ANYSYSV4
#endif

/*	ANYSYSV is any Unix system which is based on Unix System V.3 or V.4
	1.	the timezone interface is System V and not Bsd.
	2.	ANYSYSV is used with __m88k to distinguish OCS specific features
*/
#if defined(SCOUNIX) || defined(SYSV) || defined(ANYSYSV4) || defined(DECMIPS)
#define		ANYSYSV
#endif

/*	LIBCISANSI means a Unix system which has an Ansi compliant libc.a
	On such systems the driver will use libc.a instead of libind.a libansi.a
*/
#if (defined(ANYSYSV4) || defined(NEXT) || defined(__VXWORKS) || \
     defined(DECMIPS) || defined(CHORUS) || defined(LINUX) || defined(__LYNX))
#define		LIBCISANSI
#endif

/*	ALWAYSLIBANSI means our libansi.a is always used.
*/
#if defined(EMBEDDED) || defined(MSW)
#define		ALWAYSLIBSANSI
#endif

/*	SOMEANSILIB means either our Ansi C library or someone else's is used.
	In C++, Fortran, Pascal we can freely use all Ansi functions 
*/
#if defined(LIBCISANSI) || defined(ALWAYSLIBSANSI)
#define		SOMEANSILIB
#endif

#define _______UnixCount	\
	defined(BSD) 		+ \
	defined(DECMIPS)	+ \
	defined(DGUX)		+ \
	defined(HPUX)		+ \
	defined(NEXT)		+ \
	defined(SCOUNIX)	+ \
	defined(SOLARIS20)	+ \
	defined(SUNOS)		+ \
	defined(SYSV)		+ \
	defined(SYSV40)		+ \
	defined(LINUX)		+ \
	defined(ANOTHERUNIX)
#define _____OtherCount		\
	defined(EMBEDDED)	+ \
	defined(MSDOS)		+ \
	defined(MSW)		+ \
	defined(SIMULATE)	+ \
	defined(UNIXSYSCALLS)	+ \
	defined(VMS)		+ \
	defined(__VXWORKS)	+ \
	defined(CHORUS)		+ \
	defined(__OSE)		+ \
	defined(__LYNX)

#if defined(EMBEDDED) && defined(__MLIBCALL)
#undef _______UnixCount
#undef _____OtherCount
#define _______UnixCount 0
#define _____OtherCount  1
#endif

#if _______UnixCount == 1
#define		ANYUNIX
#elif _______UnixCount > 1
#error		Too many Unix target environments defined
#endif

#if _____OtherCount > 1
#error		Too many non-Unix target environments defined
#elif (_____OtherCount == 1) && defined(ANYUNIX)
#error		Both Unix and non-Unix target environments defined
#endif

#if (_______UnixCount + _____OtherCount) == 0
#error		No target environment defined
#endif

#if defined(EMBEDDED)
# if !(defined(__MC68000) && defined(__COFF)) && !defined(__scllc_asm)
#  define DYNAMIC_LONG_LONG_PRINTF 1
#  define DYNAMIC_FLOAT_PRINTF 1
#  define DYNAMIC_FLOAT_SCANF 1
# endif
#endif

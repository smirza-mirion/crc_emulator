/*
 *            Debugger
 *
 *    Copyright 1990-2000
 *    Green Hills Software, Inc.
 *
 *    This program is the property of Green Hills Software, Inc,
 *    its contents are proprietary information and no part of it
 *    is to be disclosed to anyone except employees of Green Hills
 *    Software, Inc., or as agreed in writing signed by the President
 *    of Green Hills Software, Inc.
 */

/* This file contains definitions required for emulated embedded system
   call support */
   
/*
 *  Important notes for Green Hills engineers:
 *
 *  This header is used by (preprocessed) assembly files. Constructs
 *  must be limited to #defines, conditionals, and the like.  In particular,
 *  comments (unfortunately) cannot be placed on the same line as #defines,
 *  because this causes errors in the assembler.
 *
 *  Most (if not all) of these values must be considered frozen, as
 *  they are used by product in the field and are assumed by tight assembly
 *  code that provides backwards compatibility with said product. Feel free
 *  to add new values and experiment with them, but avoid altering existing
 *  values if at all possible.
 */

/*
 *  System Call ID codes.
 *
 *  These ID codes are used both in the MULTI <--> server protocol and the
 *  monserv <--> monitor protocol.
 *  
 *  When using the Green Hills cross libraries, these codes are passed as the
 *  first argument to the function __syscall_ghs in the appropriate indsyscl.*
 *  module of the libraries.  The codes are 32-bit values, with the low 16 bits
 *  containing the call number, and the high 16 bits containing the
 *  argument count (including the first argument, the code number itself).
 *
 *  When implementing System Call support in a debug server, these codes
 *  are stored as the first argument in the argument list pointed to by
 *  the XTRA_STAT syscall.args field.
 *  
 *  Codes that have an argument count of zero are handled internally by the
 *  target or its debug server and must not be passed to MULTI via XTRA_STAT.
 *  For these calls, the argument list is defined at the time the call number
 *  is assigned, and the target knows how many arguments there should be by
 *  looking at the call number itself.
 *
 *  Note that function names beginning with '__' don't really exist; they
 *  are called internally by the libraries or debug servers and do not
 *  correspond directly to any well-known system calls.
 */

/* Begin general purpose System Call ID codes */

/* read(fd, buf, n) */
#define SYSCALL_READ	0x40000
/* write(fd, buf, n) */
#define SYSCALL_WRITE	0x40001
/* open(fname, flags) */
#define SYSCALL_OPEN	0x30004
/* open(fname, flags, mode) */
#define SYSCALL_OPEN2	0x40004
/* close(fd) */
#define SYSCALL_CLOSE	0x20005
/* creat(fname, flags) */
#define SYSCALL_CREAT	0x30006
/* lseek(fd, offset, origin) */
#define SYSCALL_LSEEK	0x40007
/* unlink(fname) */
#define SYSCALL_UNLINK	0x20008
/* rename(old, new) */
#define SYSCALL_RENAME	0x30009
/* system(string) */
#define SYSCALL_SYSTEM	0x2000A
/* access(fname, flags) */
#define SYSCALL_ACCESS	0x3000B
/* __time(), host synchronization request */
#define SYSCALL_TIME	0x1000E
/* __modtime(fname) -- currently only used by inddbsrv.c for .pro support */
#define SYSCALL_MODTIME	0x20010
/* fcntl(fd, cmd, arg) */
#define SYSCALL_FCNTL	0x40012
/*[TPW] Tue Feb 27 18:55:33 PST 1996. add remap() system call for Craig. */
/* remap(target, source, length) */
#define SYSCALL_REMAP	0x40013
/* bytestoread(fd) - return the number of bytes to read on given fd.
 * 1 may be returned if >1 */
#define SYSCALL_BYTES_TO_READ 0x20015
/* System call ID for socket emulation (args: Params, CallerID) */
#define SYSCALL_SOCKEMULATE 0x3000F

/* End general purpose System Calls,
 * begin target internal calls */
 
/* exit(status) */
#define SYSCALL_EXIT	2
/* 68K monitor specific */
#define SYSCALL_SETUP	3
/* __handler(interruptfunc) */
#define SYSCALL_HANDLER	12
/* __timeout(microseconds) */
#define SYSCALL_TIMEOUT	13
/* __time(), fetches internal monitor time */
#define SYSCALL_MONTIME	14
/* brk(addr) */
#define SYSCALL_BRK	15
/* __cachesetting(flags), enables CPU specific caching options if available. */
/* This is intended for running benchmarks on boards with the monitor. */
/* See Init_IO in indio.c */
#define SYSCALL_CACHESET 16
/* __manprof(buf, cnt), manual profiling call supported by some debug servers */
/* This call is currently reserved for experimental use with hpserv targets.  */
#define SYSCALL_MANPROF	17
/*[TPW] Mon Jun  3 04:21:26 PDT 1996. add hello() call for new crt0 */
/* hello(), always succeeds */
#define SYSCALL_HELLO	20
/*[jonj] Fri Dec 13 12:59:07 PST 1996. add moncycles() call for getting */
/* number of cycles elapsed since program start.  This is for benchmarking */
/* from the simulator */
/* This call is currently reserved for experimental INTERNAL use only. */
#define SYSCALL_CYCLES  30
/* Experimental system call to relay information from a trap handler to MULTI */
#define SYSCALL_TRAPINFO 40

/* End target internal calls */

/* Green Hills internal remote ID codes */

#define REMIO_NONE	0
#define REMIO_READ	1
#define REMIO_WRITE	2
#define REMIO_EXIT	3
#define REMID_MAGICNUM	0x12345678
#define REMID_MAGICNUM1	$12345678+1
#define REMID_HP64700	0x12345679
#define REMID_68KMON	0x1234567a
#define REMID_68KSIM	0x1234567b
#define REMID_INDMONSYS	0x1234567c
#define REMID_HP64700_2	0x1234567d
#define REMID_68KSIM_2	0x1234567e
#define REMID_PPCSIM	0x1234567f	
#define REMID_68K_BKPT7	0x12345680
#define REMID_68K_BGND	0x12345681
#define REMID_960_FMARK3 0x12345682
#define REMID_MON960	0x12345683
#define REMID_E7KSERV	0x12345684
#define REMID_IDTSERV	0x12345685
/* should have same value as last */
#define REMID_LASTMAGIC	0x12345685

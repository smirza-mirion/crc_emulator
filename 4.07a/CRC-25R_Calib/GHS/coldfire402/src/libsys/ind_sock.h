/*
 *                         Copyright 2002
 *                    Green Hills Software, Inc.
 *
 *    This program is the property of Green Hills Software, Inc,
 *    its contents are proprietary information and no part of it
 *    is to be disclosed to anyone except employees of Green Hills
 *    Software, Inc., or as agreed in writing signed by the President
 *    of Green Hills Software, Inc.
 *
 */

#ifndef __GHS_INDSOCKET_H__
#define __GHS_INDSOCKET_H__

#include <socket_emulation.h>

/* defines for socket emulation syscall interface.  This is private to
   the library and should not be used in customer code */

/* Parameter structure for argument passing: */

/* IMPORTANT: The host assumes the structure of this, so you CAN'T
   CHANGE IT!!!  This struct must always be 16 target-sized-ints. */
typedef struct {
    unsigned int CallType;
#define NUMSOCKCALLARGS 6
    unsigned int CallArgs[NUMSOCKCALLARGS];
    unsigned int ReturnArgs[NUMSOCKCALLARGS];
    unsigned int ReturnValue;
    unsigned int ReturnErrno;
    unsigned int ReturnParamStructPtr;
} EmulatedCallParamStruct;

typedef struct {
    char fd_bits[32];
} IndepFDSet;

typedef struct {
    unsigned field[7];
    unsigned returnfield[7];
} IndepMsgHdrStruct;

typedef struct {
    unsigned field[3];
    unsigned returnfield[3];
} IndepIfconfStruct;

/* As in INTEGRITY's SYSCALL_ defines (in indsyscl.h), the high order
   4 bytes in the defines below contain the number of arguments, and
   the low order four bytes simply contain a unique identifier: */

/* socket(domain, type, protocol) */
#define SOCKEMU_SOCKET 		0x30020
/* bind(fd. sa, namelen) */
#define SOCKEMU_BIND 		0x30021
/* accept(fd, sa, len) */
#define SOCKEMU_ACCEPT 		0x30022
/* getsockname(fd, sa, len) */
#define SOCKEMU_GETSOCKNAME 	0x30023
/* getsockpeername(fd, sa, len) */
#define SOCKEMU_GETPEERNAME 	0x30024
/* getsockopt(fd, level, name, val, valsize) */
#define SOCKEMU_GETSOCKOPT 	0x50020
/* setsockopt(fd, level, name, val, valsize) */
#define SOCKEMU_SETSOCKOPT 	0x50021
/* connect(fd, sa, len) */
#define SOCKEMU_CONNECT 	0x30025
/* sendto(fd, buf, len, flags, sa_to, tolen) */
#define SOCKEMU_SENDTO 		0x60020
/* recvfrom(fd, buf, len, flags, sa_to, tolen) */
#define SOCKEMU_RECVFROM	0x60021
/* listen(fd, backlog) */
#define SOCKEMU_LISTEN 		0x20020
/* close(fd) */
#define SOCKEMU_CLOSE 		0x10020
/* shutdown(fd, how) */
#define SOCKEMU_SHUTDOWN 	0x20021
/* shutdownall() */
#define SOCKEMU_SHUTDOWNALL 	0x00040
/* write(fd, buf, len) */
#define SOCKEMU_WRITE 		0x30026
/* read(fd, buf, len) */
#define SOCKEMU_READ 		0x30027
/* select(n, read_fds, write_fds, except_fds, timeout) */
#define SOCKEMU_SELECT		0x50022
/* ioctl(fd, type, buf) */
#define SOCKEMU_IOCTL           0x30028
/* sendmsg(fd, msg, flags) */
#define SOCKEMU_SENDMSG		0x30029
/* recvmsg(fd, msg, flags) */
#define SOCKEMU_RECVMSG		0x3002A
/* reopen(fd) */
#define SOCKEMU_REOPEN		0x10021
/* gethostname(name, len) */
#define SOCKEMU_GETHOSTNAME	0x20002



/* Supported socket types: */

#define INDEP_SOCK_STREAM 1
#define INDEP_SOCK_DGRAM 2
#define INDEP_SOCK_RAW 3
#define INDEP_SOCK_RDM 4
#define INDEP_SOCK_SEQPACKET 5

/* Supported socket options: */

enum IndepSocketOptions {
    INDEP_SO_DEBUG = 1,
    INDEP_SO_REUSEADDR,
    INDEP_SO_KEEPALIVE,
    INDEP_SO_DONTROUTE,
    INDEP_SO_BROADCAST,
    INDEP_SO_LINGER,
    INDEP_SO_OOBINLINE,
    INDEP_SO_SNDBUF,
    INDEP_SO_RCVBUF,
    INDEP_SO_SNDLOWAT,
    INDEP_SO_RCVLOWAT,
    INDEP_SO_SNDTIMEO,
    INDEP_SO_RCVTIMEO,

    INDEP_TCP_NODELAY
    
#if 0
/* Unsupported options: */,
/* TCP: */
    INDEP_TCP_KEEPALIVE,
    INDEP_TCP_MAXRT,
    INDEP_TCP_MAXSEG,
    INDEP_TCP_STDURG,
/* General: */
    INDEP_SO_ACCEPTCONN,
    INDEP_SO_USELOOPBACK,
    INDEP_SO_ERROR,
    INDEP_SO_TYPE,
    INDEP_SO_SND_COPYAVOID,
    INDEP_SO_RCV_COPYAVOID,
    INDEP_SO_NO_CHECK,
    INDEP_SO_PRIORITY,
/* HP: */
    INDEP_SO_REUSEPORT,
    INDEP_SO_GETIFADDR,
    INDEP_SO_PMTU,
/* Linux: */
    INDEP_SO_BSDCOMPAT,
    INDEP_SO_PASSCRED,
    INDEP_SO_PEERNAME,
    INDEP_SO_TIMESTAMP,
    INDEP_SO_ATTACH_FILTER,
    INDEP_SO_DETACH_FILTER,
    INDEP_SO_SECURITY_AUTHENTICATION,
    INDEP_SO_SECURITY_ENCRYPTION_TRANSPORT,
    INDEP_SO_SECURITY_ENCRYPTION_NETWORK,
    INDEP_SO_BINDTODEVICE,
/* INTEGRITY: */
    INDEP_SO_DGRAM_ERRIND,
    INDEP_SO_DONTLINGER,
    INDEP_SO_PROTOTYPE,
    INDEP_SO_STATE,
    INDEP_SO_SRCADDR,
    INDEP_SO_FILEP,
    INDEP_SO_UNIX_CLOSE,
/* Netport/Iniche: */
    INDEP_SO_HDRINCL,
    INDEP_SO_HOPCNT,
    INDEP_SO_MAXMSG,
    INDEP_SO_RXDATA,
    INDEP_SO_MYADDR,
    INDEP_SO_NBIO,
    INDEP_SO_BIO,
    INDEO_SO_NONBLOCK,
    INDEP_SO_CALLBACK,
/* Win32: */
    INDEP_SO_CONNDATA,
    INDEP_SO_CONNOPT,
    INDEP_SO_DISCDATA,
    INDEP_SO_DISCOPT,
    INDEP_SO_CONNDATALEN,
    INDEP_SO_CONNOPTLEN,
    INDEP_SO_CONNOPTDATALEN,
    INDEP_SO_DISCOPTLEN,
    INDEP_SO_OPENTYPE,
    INDEP_SO_SYNCHRONOUS_ALERT,
    INDEP_SO_SYNCHRONOUS_NONALERT,
    INDEP_SO_MAXDG,
    INDEP_SO_MAXPATHDG,
    INDEP_SO_UPDATE_ACCEPT_CONTEXT,
    INDEP_SO_CONNECT_TIME
#endif
/* whew! */
};

/* Supported socket levels: */

enum IndepSocketLevels {
    INDEP_SOL_SOCKET = 1,
    INDEP_IPPROTO_TCP
};

/* Supported address families: */
/* (can add more as needed) */

#define INDEP_AF_UNSPEC	0
#define INDEP_AF_INET	2

/* Supported protocol families: */

#define INDEP_PF_INET	INDEP_AF_INET
#define INDEP_PF_UNSPEC	INDEP_AF_UNSPEC




/* conversion functions for socket/errno defines: */
int SocketType_ToIndep(int SockType);
int SocketLevel_ToIndep(int SockLevel);
int SocketOption_ToIndep(int SockLevel, int SockOpt);
int SocketAF_ToIndep(int SockAF);
int SocketPF_ToIndep(int SockPF);
int Errno_FromIndep(int IndepErrno);
int IoctlRequest_ToIndep(int IndepRequestType);

/* platform-independent FDSet functions: */
int IndepFDSet_FDIsSet(unsigned fd, IndepFDSet *fdset);
void IndepFDSet_FDSet(unsigned fd, IndepFDSet *fdset);
void IndepFDSet_FDClr(unsigned fd, IndepFDSet *fdset);
void IndepFDSet_FDZero(IndepFDSet *fdset);


#if 0 /* [rus] fd_sets are only needed by select and poll, and we're
         not going to be implementing those for now. */
void FDSet_AddFDsToIndep(fd_set *fdset, IndepFDSet *indep_fdset,
	int maxfd);
void FDSet_AddFDsFromIndep(fd_set *fdset, IndepFDSet *indep_fdset,
	int maxfd);
void FDSet_ToIndep(fd_set *fdset, IndepFDSet *indep_fdset,
	int maxfd);
void FDSet_FromIndep(IndepFDSet *indep_fdset, fd_set *fdset,
	int maxfd);
#endif 

void IndepFDSet_Not(IndepFDSet *fdset);
void IndepFDSet_And(IndepFDSet *input1, IndepFDSet *input2,
	IndepFDSet *output);
void IndepFDSet_Or(IndepFDSet *input1, IndepFDSet *input2,
	IndepFDSet *output);
int IndepFDSet_IsEmpty(IndepFDSet *fdset);
int IndepFDSet_NumInSet(IndepFDSet *fdset);


/* platform-independent message header conversion functions: */
void MsgHeader_ToIndep(const struct msghdr *msg,
	IndepMsgHdrStruct *IndepMsgHeader);
void MsgHeader_FromIndep(IndepMsgHdrStruct *IndepMsgHeader,
	struct msghdr *msg);

/* Errno definitions: */
/* Adapted from NetBSD source: */
/*	$NetBSD: errno.h,v 1.24 2000/03/14 19:16:03 kleink Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)errno.h	8.5 (Berkeley) 1/21/94
 */

#define	INDEP_EPERM		1		/* Operation not permitted */
#define	INDEP_ENOENT		2		/* No such file or directory */
#define	INDEP_ESRCH		3		/* No such process */
#define	INDEP_EINTR		4		/* Interrupted system call */
#define	INDEP_EIO		5		/* Input/output error */
#define	INDEP_ENXIO		6		/* Device not configured */
#define	INDEP_E2BIG		7		/* Argument list too long */
#define	INDEP_ENOEXEC		8		/* Exec format error */
#define	INDEP_EBADF		9		/* Bad file descriptor */
#define	INDEP_ECHILD		10		/* No child processes */
#define	INDEP_EDEADLK		11		/* Resource deadlock avoided */
					/* 11 was EAGAIN */
#define	INDEP_ENOMEM		12		/* Cannot allocate memory */
#define	INDEP_EACCES		13		/* Permission denied */
#define	INDEP_EFAULT		14		/* Bad address */
#define	INDEP_ENOTBLK		15		/* Block device required */
#define	INDEP_EBUSY		16		/* Device busy */
#define	INDEP_EEXIST		17		/* File exists */
#define	INDEP_EXDEV		18		/* Cross-device link */
#define	INDEP_ENODEV		19		/* Operation not supported by device */
#define	INDEP_ENOTDIR		20		/* Not a directory */
#define	INDEP_EISDIR		21		/* Is a directory */
#define	INDEP_EINVAL		22		/* Invalid argument */
#define	INDEP_ENFILE		23		/* Too many open files in system */
#define	INDEP_EMFILE		24		/* Too many open files */
#define	INDEP_ENOTTY		25		/* Inappropriate ioctl for device */
#define	INDEP_ETXTBSY		26		/* Text file busy */
#define	INDEP_EFBIG		27		/* File too large */
#define	INDEP_ENOSPC		28		/* No space left on device */
#define	INDEP_ESPIPE		29		/* Illegal seek */
#define	INDEP_EROFS		30		/* Read-only file system */
#define	INDEP_EMLINK		31		/* Too many links */
#define	INDEP_EPIPE		32		/* Broken pipe */

/* math software */
#define	INDEP_EDOM		33		/* Numerical argument out of domain */
#define	INDEP_ERANGE		34		/* Result too large */

/* non-blocking and interrupt i/o */
#define	INDEP_EAGAIN		35		/* Resource temporarily unavailable */
#define	INDEP_EWOULDBLOCK	135		/* Operation would block */
#define	INDEP_EINPROGRESS	36		/* Operation now in progress */
#define	INDEP_EALREADY		37		/* Operation already in progress */

/* ipc/network software -- argument errors */
#define	INDEP_ENOTSOCK		38		/* Socket operation on non-socket */
#define	INDEP_EDESTADDRREQ	39		/* Destination address required */
#define	INDEP_EMSGSIZE		40		/* Message too long */
#define	INDEP_EPROTOTYPE	41		/* Protocol wrong type for socket */
#define	INDEP_ENOPROTOOPT	42		/* Protocol not available */
#define	INDEP_EPROTONOSUPPORT	43		/* Protocol not supported */
#define	INDEP_ESOCKTNOSUPPORT	44		/* Socket type not supported */
#define	INDEP_EOPNOTSUPP	45		/* Operation not supported */
#define	INDEP_EPFNOSUPPORT	46		/* Protocol family not supported */
#define	INDEP_EAFNOSUPPORT	47		/* Address family not supported by protocol family */
#define	INDEP_EADDRINUSE	48		/* Address already in use */
#define	INDEP_EADDRNOTAVAIL	49		/* Can't assign requested address */

/* ipc/network software -- operational errors */
#define	INDEP_ENETDOWN		50		/* Network is down */
#define	INDEP_ENETUNREACH	51		/* Network is unreachable */
#define	INDEP_ENETRESET		52		/* Network dropped connection on reset */
#define	INDEP_ECONNABORTED	53		/* Software caused connection abort */
#define	INDEP_ECONNRESET	54		/* Connection reset by peer */
#define	INDEP_ENOBUFS		55		/* No buffer space available */
#define	INDEP_EISCONN		56		/* Socket is already connected */
#define	INDEP_ENOTCONN		57		/* Socket is not connected */
#define	INDEP_ESHUTDOWN		58		/* Can't send after socket shutdown */
#define	INDEP_ETOOMANYREFS	59		/* Too many references: can't splice */
#define	INDEP_ETIMEDOUT		60		/* Operation timed out */
#define	INDEP_ECONNREFUSED	61		/* Connection refused */

#define	INDEP_ELOOP		62		/* Too many levels of symbolic links */
#define	INDEP_ENAMETOOLONG	63		/* File name too long */

/* should be rearranged */
#define	INDEP_EHOSTDOWN		64		/* Host is down */
#define	INDEP_EHOSTUNREACH	65		/* No route to host */
#define	INDEP_ENOTEMPTY		66		/* Directory not empty */

/* quotas & mush */
#define	INDEP_EPROCLIM		67		/* Too many processes */
#define	INDEP_EUSERS		68		/* Too many users */
#define	INDEP_EDQUOT		69		/* Disc quota exceeded */

/* Network File System */
#define	INDEP_ESTALE		70		/* Stale NFS file handle */
#define	INDEP_EREMOTE		71		/* Too many levels of remote in path */
#define	INDEP_EBADRPC		72		/* RPC struct is bad */
#define	INDEP_ERPCMISMATCH	73		/* RPC version wrong */
#define	INDEP_EPROGUNAVAIL	74		/* RPC prog. not avail */
#define	INDEP_EPROGMISMATCH	75		/* Program version wrong */
#define	INDEP_EPROCUNAVAIL	76		/* Bad procedure for program */

#define	INDEP_ENOLCK		77		/* No locks available */
#define	INDEP_ENOSYS		78		/* Function not implemented */

#define	INDEP_EFTYPE		79		/* Inappropriate file type or format */
#define	INDEP_EAUTH		80		/* Authentication error */
#define	INDEP_ENEEDAUTH		81		/* Need authenticator */

/* SystemV IPC */
#define	INDEP_EIDRM		82		/* Identifier removed */
#define	INDEP_ENOMSG		83		/* No message of desired type */
#define	INDEP_EOVERFLOW		84		/* Value too large to be stored in data type */

/* Wide/multibyte-character handling, ISO/IEC 9899/AMD1:1995 */
#define	INDEP_EILSEQ		85		/* Illegal byte sequence */

#define	INDEP_ELAST		85		/* Must equal largest errno */

/* pseudo-errors returned inside kernel to modify return to process */
#define	INDEP_ERESTART		-1		/* restart syscall */
#define	INDEP_EJUSTRETURN	-2		/* don't modify regs, just return */

/* Ioctl requests: */

enum {
    INDEP_FIONBIO = 1,
    INDEP_FIONREAD,
    INDEP_FIOASYNC,
    INDEP_SIOCGIFCONF,
    INDEP_SIOCGIFADDR,
    INDEP_SIOCGIFBRDADDR,
    INDEP_SIOCGIFNETMASK
};

#endif /* __GHS_INDSOCKET_H__ */

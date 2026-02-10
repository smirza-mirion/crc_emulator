/*
		    Language Independent Library

	    Copyright 1983-2001 Green Hills Software,Inc.

 *  This program is the property of Green Hills Software, Inc,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software, Inc., or as agreed in writing signed by the President
 *  of Green Hills Software, Inc.
*/


/*
 * If your system is a real UNIX system then nothing needs to be done to
 * this file.
 */

#if defined(EMBEDDED) && defined (EMULATE_SOCKET_IO)

#include "ghs_null.h"
#include "ind_sock.h"
#include "../libsys/indsyscl.h"


#define SOCKET_FD_BASE 1024

extern int __ghs_syscall(int, ...);
/* END */


/* BEGIN SOCKEMUCALLS SNIP */
static void SetupSockCallParams (EmulatedCallParamStruct * ParamStruct,
				 int SockCallType,  unsigned int arg0,
				 unsigned int arg1, unsigned int arg2,
				 unsigned int arg3, unsigned int arg4, 
				 unsigned int arg5) {
    ParamStruct->CallType = SockCallType;
    ParamStruct->CallArgs[0] = ParamStruct->ReturnArgs[0] = arg0;
    ParamStruct->CallArgs[1] = ParamStruct->ReturnArgs[1] = arg1;
    ParamStruct->CallArgs[2] = ParamStruct->ReturnArgs[2] = arg2;
    ParamStruct->CallArgs[3] = ParamStruct->ReturnArgs[3] = arg3;
    ParamStruct->CallArgs[4] = ParamStruct->ReturnArgs[4] = arg4;
    ParamStruct->CallArgs[5] = ParamStruct->ReturnArgs[5] = arg5;
    ParamStruct->ReturnValue = (unsigned int)-1;
    ParamStruct->ReturnErrno = (unsigned int)INDEP_EIO;
    ParamStruct->ReturnParamStructPtr = (unsigned int)ParamStruct;
}


#define SocketEmulationSystemCall(params) \
	__ghs_syscall(SYSCALL_SOCKEMULATE, params, 0)

int socket(int domain, int type, int protocol) 
{
    EmulatedCallParamStruct SockCallParams;
    
    type = SocketType_ToIndep(type); /* convert to platform indep. */
    protocol = SocketPF_ToIndep(protocol); /* convert as well */
    if (type == -1 || protocol == -1) {
	errno = EOPNOTSUPP;
	return -1;
    }
    
    SetupSockCallParams (&SockCallParams, SOCKEMU_SOCKET, (unsigned int)domain,
			 (unsigned int)type, (unsigned int)protocol, 0,0,0);
    SocketEmulationSystemCall(&SockCallParams);
    
    if ((int)SockCallParams.ReturnValue < 0) {
	return (int)SockCallParams.ReturnValue;
    } else {
	return SockCallParams.ReturnValue + SOCKET_FD_BASE;
    }
}

int bind(int s, const struct sockaddr *name, int namelen) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd, rtnval;

    fd = s - SOCKET_FD_BASE;
    
    SetupSockCallParams (&SockCallParams, SOCKEMU_BIND, (unsigned int)fd,
			 (unsigned int)name, (unsigned int)namelen, 0,0,0);
    SocketEmulationSystemCall(&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return rtnval;
    
}

int accept(int s, struct sockaddr *name, int *anamelen) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd;
    
    fd = s - SOCKET_FD_BASE;
    
    anamelen = (int*)(name?anamelen:0);
    SetupSockCallParams (&SockCallParams, SOCKEMU_ACCEPT, (unsigned int)fd,
			 (unsigned int)name, (unsigned int)anamelen, 0,0,0);
    SocketEmulationSystemCall (&SockCallParams);

    if ((int)SockCallParams.ReturnValue < 0) {
	return (int)SockCallParams.ReturnValue;
    } else {
	return SockCallParams.ReturnValue + SOCKET_FD_BASE;
    }
}

int getsockopt(int s, int level, int name, void *val, int *avalsize) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd, rtnval;
    
    name = SocketOption_ToIndep(level, name);
    level = SocketLevel_ToIndep(level);
    if (level == -1 || name == -1) {
	errno = EOPNOTSUPP;
	return -1;
    }
    
    fd = s - SOCKET_FD_BASE;
    
    avalsize = (int*)(val?avalsize:0);
    SetupSockCallParams (&SockCallParams, SOCKEMU_GETSOCKOPT, (unsigned int)fd,
			 (unsigned int)level, (unsigned int)name,
			 (unsigned int)val, (unsigned int)avalsize, 0);
    SocketEmulationSystemCall (&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return rtnval;
    
}

int setsockopt(int s, int level, int name, const void *val, int valsize) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd, rtnval;
    
    name = SocketOption_ToIndep(level, name);
    level = SocketLevel_ToIndep(level);
    if (level == -1 || name == -1) {
	errno = EOPNOTSUPP;
	return -1;
    }
    
    fd = s - SOCKET_FD_BASE;
    
    SetupSockCallParams (&SockCallParams, SOCKEMU_SETSOCKOPT, (unsigned int)fd,
			 (unsigned int)level, (unsigned int)name,
			 (unsigned int)val, (unsigned int)valsize, 0);
    SocketEmulationSystemCall (&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return rtnval;
    
}

int connect(int s, const struct sockaddr *name, int namelen) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd, rtnval;
    
    fd = s - SOCKET_FD_BASE;
    
    SetupSockCallParams (&SockCallParams, SOCKEMU_CONNECT, (unsigned int)fd,
			 (unsigned int)name, (unsigned int)namelen, 0,0,0);
    SocketEmulationSystemCall (&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return rtnval;
}

ssize_t	sendto(int s, const void *buf, size_t len, int flags, 
	       const struct sockaddr *to, int tolen) 
{
    EmulatedCallParamStruct SockCallParams;
    int rtnval;
    int fd;
    
    fd = s - SOCKET_FD_BASE;
    
    tolen = (to ? tolen : 0);
    SetupSockCallParams (&SockCallParams, SOCKEMU_SENDTO, (unsigned int)fd,
			 (unsigned int)buf, (unsigned int)len,
			 (unsigned int)flags, (unsigned int)to, tolen);
    SocketEmulationSystemCall (&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval == -1) {
	/* Set errno  to record the error */
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
	return -1;
    }
    return (ssize_t) rtnval;
}

ssize_t	send(int s, const void *buf, size_t len, int flags) 
{
    return sendto(s, buf, len, flags, NULL, 0);
}

#if 0 /* not currently supported */
ssize_t sendmsg(int s, const struct msghdr *msg, int flags) {
    EmulatedCallParamStruct SockCallParams;
    IndepMsgHdrStruct IndepMsgHeader;
    int fd, rtnval;
    
    if ((fd = SDGetServerSD(s)) == -1) {
	errno = EINVAL;
	return -1;
    }
    
    MsgHeader_ToIndep(msg, &IndepMsgHeader);
    SetupSockCallParams (&SockCallParams, SOCKEMU_SENDMSG, (unsigned int)fd,
			 (unsigned int)&IndepMsgHeader, (unsigned int)flags,
			 0,0,0);
    SocketEmulationSystemCall (&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return rtnval;
}
#endif

#if 0 /* not currently supported */
ssize_t sendmsg(int s, const struct msghdr *msg, int flags) {
    /* This version uses sendto rather than a direct sendmsg call */
    int i;
    int msgsize = 0;
    char* msgbuf;
    char* msgbuf_pos;
    int retval;
    
    if(!msg)
	return -1;
    
    /* Run through the list of iovs and malloc the right ammount of data: */
    /* crude, but it works, until sendmsg is directly supported. */
    for(i = 0; i < msg->msg_iovlen;i++) {
	msgsize += msg->msg_iov[i].iov_len;
    }
    msgbuf = malloc(msgsize);
    
    msgbuf_pos = msgbuf;
    for(i = 0; i < msg->msg_iovlen;i++) {
	memcpy(msgbuf_pos,msg->msg_iov[i].iov_base,msg->msg_iov[i].iov_len);
	msgbuf_pos += msg->msg_iov[i].iov_len;
    }
    
    /* Send it out in a flat buffer: */
    /* forget about sending any control data! (who does that anyway?) */
    retval = sendto(s, msgbuf, msgsize, flags,msg->msg_name,msg->msg_namelen);
    
    free(msgbuf);
    return retval;
}
#endif

ssize_t	recvfrom(int s, void *buf, size_t len, int flags,
		 struct sockaddr *from, int *fromlenaddr) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd, rtnval;
    
    fd = s - SOCKET_FD_BASE;
    
    fromlenaddr = (int*)(from?fromlenaddr:0);
    SetupSockCallParams (&SockCallParams, SOCKEMU_RECVFROM, (unsigned int)fd,
			 (unsigned int)buf, (unsigned int)len,
			 (unsigned int)flags, (unsigned int)from,
			 (unsigned int)fromlenaddr);
    SocketEmulationSystemCall (&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return (ssize_t) rtnval;
}

ssize_t	recv(int s, void *buf, size_t len, int flags) 
{
    return recvfrom(s, buf, len, flags, NULL, NULL);
}

#if 0 /* not currently supported */
ssize_t recvmsg(int s, struct msghdr *msg, int flags) {
    /* For now, assume IndepMsgHdrStruct is same size as struct msghdr,
       so no translation is necessary. */
    EmulatedCallParamStruct SockCallParams;
    IndepMsgHdrStruct IndepMsgHeader;
    int fd, rtnval;
    
    if ((fd = SDGetServerSD(s)) == -1) {
	errno = EINVAL;
	return -1;
    }
    
    MsgHeader_ToIndep(msg, &IndepMsgHeader);
    SetupSockCallParams (SockCallParams, SOCKEMU_RECVMSG, (unsigned int)fd,
			 (unsigned int)&IndepMsgHeader, (unsigned int)flags,
			 0,0,0);
    SocketEmulationSystemCall (&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    MsgHeader_FromIndep(&IndepMsgHeader, msg);
    return rtnval;
}
#endif

#if 0 /* not currently supported */
ssize_t recvmsg(int s, struct msghdr * msg, int flags) {
    /* This version uses recvfrom rather than a direct recvmsg call */
    int i;
    int msgsize = 0;
    char* msgbuf;
    char* msgbuf_pos;
    int retval;
    
    if(!msg)
	return -1;
    
    /* Run through the list of iovs and malloc the right ammount of data: */
    /* crude, but it works, until recvmsg is directly supported. */
    for(i = 0; i < msg->msg_iovlen;i++) {
	msgsize += (msg->msg_iov[i].iov_len);
    }
    msgbuf = malloc(msgsize);
    
    /* Receive into a flat buffer: */
    /* forget about getting back any control data or flags!
       (who does that anyway?) */
    retval = recvfrom(s, msgbuf, msgsize, flags,
		      msg->msg_name, &(msg->msg_namelen));
    if (retval == -1) {
	free(msgbuf);
	return -1;
    }
    
    /* Copy the stuff that we got out: */
    msgbuf_pos = msgbuf;
    for(i = 0; i < msg->msg_iovlen;i++) {
	memcpy(msg->msg_iov[i].iov_base,msgbuf_pos,msg->msg_iov[i].iov_len);
	msgbuf_pos += msg->msg_iov[i].iov_len;
    }
    free(msgbuf);
    return retval;
    
}
#endif


int listen(int s, int backlog) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd, rtnval;
    
    fd = s - SOCKET_FD_BASE;
    
    SetupSockCallParams (&SockCallParams, SOCKEMU_LISTEN, (unsigned int)fd,
			 (unsigned int)backlog, 0,0,0,0);
    SocketEmulationSystemCall (&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return rtnval;
}

int shutdown(int s, int how) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd, rtnval;
    
    fd = s - SOCKET_FD_BASE;
    
    SetupSockCallParams (&SockCallParams, SOCKEMU_SHUTDOWN, (unsigned int)fd,
			 (unsigned int)how, 0,0,0,0);
    SocketEmulationSystemCall (&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return rtnval;
}

#if 0
int shutdownall(void) 
{
    /* not yet implemented */
    return -1;
}
#endif

#if 0 /* not currently supported */
Error socket_ioctl(void *buf, int s, int cmd, caddr_t data) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd = s, request, rtnval;
    IndepIfconfStruct IFC;
    
    request = IoctlRequest_ToIndep(cmd);
    if (request == -1) {
	errno = EOPNOTSUPP;
	return Failure;
    }
    
    if (request == INDEP_SIOCGIFCONF) {
	/* need to use indep. struct in lieu of user's ifconf struct: */
	memcpy(&IFC.field[0], (void*)data, 2*sizeof(unsigned));
	memcpy(&IFC.returnfield[0], (void*)data, 2*sizeof(unsigned));
	/* backup data addr. */
	IFC.field[2] = IFC.returnfield[2] = (unsigned)data;
	/* call with the indep. struct: */
	data = (caddr_t)&IFC;
    }
    SetupSockCallParams(SockCallParams,
			SOCKEMU_IOCTL, fd, request, data,0,0,0);
    SocketEmulationSystemCall(&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0) {
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
	return Failure;
    }
    if (request == INDEP_SIOCGIFCONF) {
	/* copy ifconf struct back from indep. struct: */
	memcpy((void*)IFC.returnfield[2], &IFC.returnfield[0],
	       2*sizeof(unsigned));
    }
    return Success;
}
#endif

#if 0 /* not currently supported */
Error socket_fcntl(void *data, int s, int cmd, int arg) 
{
    /* not yet implemented */
    return Failure;
}
#endif

#if 0 /* not currently supported */
Error socket_close(void *data, int s) 
{
    EmulatedCallParamStruct SockCallParams;
    int rtnval;
    
    SetupSockCallParams(SockCallParams,
			SOCKEMU_CLOSE, s, 0,0,0,0,0);
    SocketEmulationSystemCall(&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return (rtnval ? Failure : Success);
}
#endif

#if 0 /* not currently supported */
Error socket_write(void *data, int s, const void *buf, size_t *len) 
{
    EmulatedCallParamStruct SockCallParams;
    int error = 0, dataSent = 0, retVal;
    
    do {
	/* A partial send may occur which should be retried/continued.  This
	 * occurs due to lack of buffer space in the stack.  The signal for
	 * such a send will be a positive return value (data sent), but an
	 * error of E2BIG.
	 */
	SetupSockCallParams(SockCallParams,
			    SOCKEMU_WRITE, s, (unsigned)buf, *len, 0,0,0);
	SocketEmulationSystemCall(&SockCallParams);
	retVal = SockCallParams.ReturnValue;
	if (retVal > 0) {
	    dataSent += retVal;
	    buf = (char *)buf + retVal;
	    *len -= retVal;
	} else if(retVal <0) {
	    error = Errno_FromIndep(SockCallParams.ReturnErrno);
	    errno = error;
	}
	/* change error test (may not be the error all hosts return)? */  
    } while (error == E2BIG && len > 0);
    *len = dataSent;
    return (error && error != E2BIG) ? Failure : Success;
}
#endif

#if 0 /* not currently supported */
Error socket_read(void *data, int s, void *buf, size_t *len) 
{
    EmulatedCallParamStruct SockCallParams;
    int rtnval;
    
    SetupSockCallParams(SockCallParams,
			SOCKEMU_READ, s, (unsigned)buf, *len, 0,0,0);
    SocketEmulationSystemCall(&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if(rtnval < 0) {
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
	return Failure;
    }
    *len = rtnval;
    return Success;
}
#endif


#if 0 /* not currently supported */
Error socket_reopen(void *data, int fno, int* new_fno) {
    EmulatedCallParamStruct SockCallParams;
    int rtnval;
    
    SetupSockCallParams(SockCallParams,
			SOCKEMU_REOPEN, fno, 0,0,0,0,0);
    SocketEmulationSystemCall(&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0) {
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
	return Failure;
    }
    *new_fno = fno;
    return Success;
}
#endif

#if 0 /* not currently supported */
ssize_t readv(int s, const struct iovec *iovp, int iovcnt)
{
    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = (struct iovec *)iovp;
    msg.msg_iovlen = iovcnt;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;
    return recvmsg(s, &msg, 0);
}

ssize_t writev(int s, const struct iovec *iovp, int iovcnt)
{
    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = (struct iovec *)iovp;
    msg.msg_iovlen = iovcnt;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;
    return sendmsg(s, &msg, 0);
}
#endif

#if 0 /* not currently supported */
/* For now, appropriate below the implementation from libsocket
   of poll(), until a real one is implemented in the socket
   emulation layer. */

/* None of the current TCP/IP stacks for INTEGRITY supports poll 
 * directly, so emulate a poll with select as best we can.  When we 
 * have a stack that supports poll directly, we should send the call
 * across the connection and implement select in terms of poll.
 *
 * NOTE: The current implementation does not correctly handle 
 * out-of-band data and all error conditions because select does not
 * provide the level of detail needed.
 */
int poll(struct pollfd *ufds, unsigned int nfds, int timeoutms) {
    struct timeval tv;
    fd_set rd, wr, ex;
    int i, maxfd = -1;
    int result;
    
    if (timeoutms >= 0) {
	tv.tv_sec = timeoutms / 1000;
	tv.tv_usec = (timeoutms % 1000) * 1000;
    }
    
    /* Setup the select FD sets */
    FD_ZERO(&rd);
    FD_ZERO(&wr);
    FD_ZERO(&ex);
    for (i = 0; i < nfds; i++) {
	ufds[i].revents = 0;
	if (ufds[i].fd > maxfd)
	    maxfd = ufds[i].fd;
	if (ufds[i].events & (POLLIN | POLLPRI))
	    FD_SET(ufds[i].fd, &rd);
	if (ufds[i].events & POLLOUT)
	    FD_SET(ufds[i].fd, &wr);
	FD_SET(ufds[i].fd, &ex);
    }
    
    /* Call select.  if timeoutms < 0, then infinite timeout */
    result = select(maxfd + 1, &rd, &wr, &ex, 
		    (timeoutms < 0) ? NULL : &tv);
    
    if (result <= 0)
	return result;
    
    /* Map select information back into poll information.  Note that there
     * is insufficient information to determine whether POLLPRI, POLLHUP
     * or POLLNVAL should be set.
     *
     * Note that the return value from select is the total number of bits
     * set in all fd_sets, which is not helpful in determining what poll
     * should return.
     */
    result = 0;
    for (i = 0; i < nfds; i++) {
	int ismarked = 0;
	if (FD_ISSET(ufds[i].fd, &rd)) {
	    ismarked = 1;
	    ufds[i].revents |= POLLIN;
	}
	if (FD_ISSET(ufds[i].fd, &wr)) {
	    ismarked = 1;
	    ufds[i].revents |= POLLOUT;
	}
	if (FD_ISSET(ufds[i].fd, &ex)) {
	    ismarked = 1;
	    ufds[i].revents |= POLLERR;
	}
	if (ismarked)
	    result++;
    }
    
    return result;
}

int select(int nd, fd_set *in, fd_set *ou, fd_set *ex, struct timeval *tv)
{
    EmulatedCallParamStruct SockCallParams;
    fd_set trans_in, trans_ou, trans_ex;
    IndepFDSet TransIn_Indep, TransOu_Indep, TransEx_Indep;
    int trans_nd;
    int ret;
    
    if (sizeof(IndepFDSet) != sizeof(fd_set)) {
	fprintf(console,"IndepFDSet size is wrong in sockemucalls.h");
	return -1;
    } /* take this check out after IndepFDSet is more than a
         placeholder for fd_set */
    
    if (FDTranslateSocketSelect(nd, in, ou, ex, &trans_nd, &trans_in,
				&trans_ou, &trans_ex) != Success) {
	/* FDTrans will set the errno */
	return -1;
    }
    
    /* translate FD sets to independent form: */
    FDSet_ToIndep(&trans_in, &TransIn_Indep, trans_nd);
    FDSet_ToIndep(&trans_ou, &TransOu_Indep, trans_nd);
    FDSet_ToIndep(&trans_ex, &TransEx_Indep, trans_nd);
    
    SetupSockCallParams(SockCallParams, SOCKEMU_SELECT,
			trans_nd, (unsigned)&TransIn_Indep,
			(unsigned)&TransOu_Indep,
			(unsigned)&TransEx_Indep, (unsigned)tv, 0);
    SocketEmulationSystemCall(&SockCallParams);
    ret = SockCallParams.ReturnValue;
    if (ret == -1) {
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
	return -1;
    }
    
    /* translate FD sets back from independent form: */
    FDSet_FromIndep(&TransIn_Indep, &trans_in, trans_nd);
    FDSet_FromIndep(&TransOu_Indep, &trans_ou, trans_nd);
    FDSet_FromIndep(&TransEx_Indep, &trans_ex, trans_nd);
    
    if (FDUntranslateSocketSelect(&trans_in, &trans_ou, &trans_ex, nd,
				  in, ou, ex, &socket_handler) != Success) {
	/* FDUntrans will set errno */
	return -1;
    }
    
    return ret;
}

#endif

#if 0 /* not currently supported */
/* These functions are not implemented yet.
   The stubs below are to make everything build (yuck!). */

int gethostname(char *name, int namelen)
{
    EmulatedCallParamStruct SockCallParams;
    int rtnval;
    
    SetupSockCallParams(SockCallParams, SOCKEMU_GETHOSTNAME,
			(unsigned)name, namelen, 0,0,0,0);
    SocketEmulationSystemCall(&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return rtnval;
}

int getpeername(int fdes, struct sockaddr *asa, int *alen) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd, rtnval;
    
    if ((fd = SDGetServerSD(fdes)) == -1) {
	errno = EINVAL;
	return -1;
    }
    
    alen = (int*)(asa?alen:0);
    SetupSockCallParams(SockCallParams,
			SOCKEMU_GETPEERNAME, fd, (unsigned)asa,
			(unsigned)alen, 0,0,0);
    SocketEmulationSystemCall(&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return rtnval;
}

int getsockname(int fdes, struct sockaddr *asa, int *alen) 
{
    EmulatedCallParamStruct SockCallParams;
    int fd, rtnval;
    
    if ((fd = SDGetServerSD(fdes)) == -1) {
	errno = EINVAL;
	return -1;
    }
    
    alen = (int*)(asa?alen:0);
    SetupSockCallParams(SockCallParams,
			SOCKEMU_GETSOCKNAME, fd, (unsigned)asa,
			(unsigned)alen, 0,0,0);
    SocketEmulationSystemCall(&SockCallParams);
    rtnval = SockCallParams.ReturnValue;
    if (rtnval < 0)
	errno = Errno_FromIndep(SockCallParams.ReturnErrno);
    return rtnval;
}

int __getnameserverip(void) {
    return -1;
}

typedef unsigned long ip_addr;
int do_ping(ip_addr r_host, int l_size, int n_ping, int interval){
    return -1;
}
#endif
/* END SOCKEMUCALLS SNIP */










/* BEGIN SOCKDEFCONV SNIP */




/* Socket define conversion functions: */

int SocketType_ToIndep(int SockType) {
    switch (SockType) {
    case SOCK_DGRAM:
	return INDEP_SOCK_DGRAM;
    case SOCK_STREAM:
	return INDEP_SOCK_STREAM;
    case SOCK_RAW:
	return INDEP_SOCK_RAW;
    case SOCK_RDM:
	return INDEP_SOCK_RDM;
    case SOCK_SEQPACKET:
	return INDEP_SOCK_SEQPACKET;
    default:
	return 0;
    }
}

int SocketOption_ToIndep(int SockLevel, int SockOpt) {
    switch (SockLevel) {
    case SOL_SOCKET:
	switch (SockOpt) {
	case SO_DEBUG:
	    return INDEP_SO_DEBUG;
	case SO_REUSEADDR:
	    return INDEP_SO_REUSEADDR;
	case SO_KEEPALIVE:
	    return INDEP_SO_KEEPALIVE;
	case SO_DONTROUTE:
	    return INDEP_SO_DONTROUTE;
	case SO_BROADCAST:
	    return INDEP_SO_BROADCAST;
	case SO_LINGER:
	    return INDEP_SO_LINGER;
	case SO_OOBINLINE:
	    return INDEP_SO_OOBINLINE;
	case SO_SNDBUF:
	    return INDEP_SO_SNDBUF;
	case SO_RCVBUF:
	    return INDEP_SO_RCVBUF;
	case SO_SNDLOWAT:
	    return INDEP_SO_SNDLOWAT;
	case SO_RCVLOWAT:
	    return INDEP_SO_RCVLOWAT;
	case SO_SNDTIMEO:
	    return INDEP_SO_SNDTIMEO;
	case SO_RCVTIMEO:
	    return INDEP_SO_RCVTIMEO;
	    
	default:
	    /* option not supported */
	    return -1;
	}
    case IPPROTO_TCP:
	switch (SockOpt) {
	case TCP_NODELAY:
	    return INDEP_TCP_NODELAY;
	    
	default:
	    /* option not supported */
	    return -1;
	}
    default:
	/* options not supported for this level */
	return -1;
    }
}

int SocketLevel_ToIndep(int SockLevel) {
    switch (SockLevel) {
    case SOL_SOCKET:
	return INDEP_SOL_SOCKET;
    case IPPROTO_TCP:
	return INDEP_IPPROTO_TCP;
    default:
	/* level not supported */
	return -1;
    }
}

int SocketAF_ToIndep(int SockAF) {
    switch (SockAF) {
    case AF_UNSPEC:
	return INDEP_AF_UNSPEC;
    case AF_INET:
	return INDEP_AF_INET;
    default:
	/* Address family not supported */
	return -1;
    }
}

int SocketPF_ToIndep(int SockPF) {
    switch (SockPF) {
    case PF_UNSPEC:
	return INDEP_PF_UNSPEC;
    case PF_INET:
	return INDEP_PF_INET;
    default:
	/* Protocol family not supported */
	return -1;
    }
}	    

/* Platform-Independent FDSet functions: */

#define IndepFDSet_NumBytes (sizeof(IndepFDSet)/sizeof(char))
/* this assumes that the IndepFDSet struct is entirely made
   up of char members and that they fill up the structure */
#define IndepFDSet_MaxFD ((IndepFDSet_NumBytes * 8) - 1)

int IndepFDSet_FDIsSet(unsigned fd, IndepFDSet *fdset) {
    if (fd > IndepFDSet_MaxFD)
	return 0;
    return (fdset->fd_bits[fd/8] >> fd%8)
	& 0x00000001;
}

void IndepFDSet_FDSet(unsigned fd, IndepFDSet *fdset) {
    char fd_bitmask = 0x00000001 << (fd%8);
    if (fd > IndepFDSet_MaxFD)
	return;
    fdset->fd_bits[fd/8] |= fd_bitmask;
}

void IndepFDSet_FDClr(unsigned fd, IndepFDSet *fdset) {
    char fd_bitmask = 0x00000001 << (fd%8);
    if (fd > IndepFDSet_MaxFD)
	return;
    fdset->fd_bits[fd/8] &= (~fd_bitmask);
}

void IndepFDSet_FDZero(IndepFDSet *fdset) {
    int i;
    for (i = 0; i < IndepFDSet_NumBytes; i++)
	fdset->fd_bits[i] = 0;
}

#if 0 /* fd_sets are only needed by select and poll, and those are not
         currently supported. */

void FDSet_AddFDsToIndep(fd_set *fdset, IndepFDSet *indep_fdset,
			 int maxfd) {
    int i, bit;
    if (maxfd > IndepFDSet_MaxFD)
	maxfd = IndepFDSet_MaxFD;
    for (i = 0; i <= maxfd/8; i++)
	for (bit = 0; bit < 8; bit++)
	    if (FD_ISSET(i*8+bit, fdset)) {
		char fd_bitmask = 0x00000001 << bit;
		indep_fdset->fd_bits[i] |= fd_bitmask;
	    }
}

void FDSet_AddFDsFromIndep(fd_set *fdset, IndepFDSet *indep_fdset,
			   int maxfd) {
    int i, bit;
    if (maxfd > IndepFDSet_MaxFD)
	maxfd = IndepFDSet_MaxFD;
    for (i = 0; i <= maxfd/8; i++)
	for (bit = 0; bit < 8; bit++) {
	    char fd_bitmask = 0x00000001 << bit;
	    if (indep_fdset->fd_bits[i] & fd_bitmask)
		FD_SET(i*8+bit, fdset);
	}
}

void FDSet_ToIndep(fd_set *fdset, IndepFDSet *indep_fdset,
		   int maxfd) {
    IndepFDSet_FDZero(indep_fdset);
    if (maxfd > IndepFDSet_MaxFD)
	maxfd = IndepFDSet_MaxFD;
    FDSet_AddFDsToIndep(fdset, indep_fdset, maxfd);
}

void FDSet_FromIndep(IndepFDSet *indep_fdset, fd_set *fdset,
		     int maxfd) {
    FD_ZERO(fdset);
    if (maxfd > IndepFDSet_MaxFD)
	maxfd = IndepFDSet_MaxFD;
    FDSet_AddFDsFromIndep(fdset, indep_fdset, maxfd);
}

#endif 

void IndepFDSet_Not(IndepFDSet *fdset) {
    int i;
    for (i = 0; i < IndepFDSet_NumBytes; i++)
	fdset->fd_bits[i] = ~(fdset->fd_bits[i]);
}

void IndepFDSet_And(IndepFDSet *input1, IndepFDSet *input2,
		    IndepFDSet *output) {
    int i;
    for (i = 0; i < IndepFDSet_NumBytes; i++)
	output->fd_bits[i] = input1->fd_bits[i] & input2->fd_bits[i];
}

void IndepFDSet_Or(IndepFDSet *input1, IndepFDSet *input2,
		   IndepFDSet *output) {
    int i;
    for (i = 0; i < IndepFDSet_NumBytes; i++)
	output->fd_bits[i] = input1->fd_bits[i] | input2->fd_bits[i];
}

int IndepFDSet_IsEmpty(IndepFDSet *fdset) {
    int i;
    for (i = 0; i < IndepFDSet_NumBytes; i++)
	if (fdset->fd_bits[i] != 0)
	    return 0;
    return 1;
}

int IndepFDSet_NumInSet(IndepFDSet *fdset) {
    int i, bit, numfds = 0;
    for (i = 0; i < IndepFDSet_NumBytes; i++)
	for (bit = 0; bit < 8; bit++) {
	    char fd_bitmask = 0x00000001 << bit;
	    if (fdset->fd_bits[i] & fd_bitmask)
		numfds++;
	}
    return numfds;
}

/* Errno conversion: */

int Errno_FromIndep(int IndepErrno) {
    switch (IndepErrno) {
    case INDEP_EPERM:
	return EPERM;
    case INDEP_ENOENT:
	return ENOENT;
    case INDEP_ESRCH:
	return ESRCH;
    case INDEP_EINTR:
	return EINTR;
    case INDEP_EIO:
	return EIO;
    case INDEP_ENXIO:
	return ENXIO;
    case INDEP_E2BIG:
	return E2BIG;
    case INDEP_ENOEXEC:
	return ENOEXEC;
    case INDEP_EBADF:
	return EBADF;
    case INDEP_ECHILD:
	return ECHILD;
    case INDEP_EDEADLK:
	return EDEADLK;
    case INDEP_ENOMEM:
	return ENOMEM;
    case INDEP_EACCES:
	return EACCES;
    case INDEP_EFAULT:
	return EFAULT;
    case INDEP_ENOTBLK:
	return ENOTBLK;
    case INDEP_EBUSY:
	return EBUSY;
    case INDEP_EEXIST:
	return EEXIST;
    case INDEP_EXDEV:
	return EXDEV;
    case INDEP_ENODEV:
	return ENODEV;
    case INDEP_ENOTDIR:
	return ENOTDIR;
    case INDEP_EISDIR:
	return EISDIR;
    case INDEP_EINVAL:
	return EINVAL;
    case INDEP_ENFILE:
	return ENFILE;
    case INDEP_EMFILE:
	return EMFILE;
    case INDEP_ENOTTY:
	return ENOTTY;
    case INDEP_ESPIPE:
	return ESPIPE;
    case INDEP_EROFS:
	return EROFS;
    case INDEP_EMLINK:
	return EMLINK;
    case INDEP_EPIPE:
	return EPIPE;
    case INDEP_EDOM:
	return EDOM;
    case INDEP_ERANGE:
	return ERANGE;
    case INDEP_EAGAIN:
	return EAGAIN;
    case INDEP_EWOULDBLOCK:
	return EWOULDBLOCK;
    case INDEP_EINPROGRESS:
	return EINPROGRESS;
    case INDEP_EALREADY:
	return EALREADY;
    case INDEP_ENOTSOCK:
	return ENOTSOCK;
    case INDEP_EDESTADDRREQ:
	return EDESTADDRREQ;
    case INDEP_EMSGSIZE:
	return EMSGSIZE;
    case INDEP_EPROTOTYPE:
	return EPROTOTYPE;
    case INDEP_ENOPROTOOPT:
	return ENOPROTOOPT;
    case INDEP_EPROTONOSUPPORT:
	return EPROTONOSUPPORT;
    case INDEP_ESOCKTNOSUPPORT:
	return ESOCKTNOSUPPORT;
    case INDEP_EOPNOTSUPP:
	return EOPNOTSUPP;
    case INDEP_EPFNOSUPPORT:
	return EPFNOSUPPORT;
    case INDEP_EAFNOSUPPORT:
	return EAFNOSUPPORT;
    case INDEP_EADDRINUSE:
	return EADDRINUSE;
    case INDEP_EADDRNOTAVAIL:
	return EADDRNOTAVAIL;
    case INDEP_ENETDOWN:
	return ENETDOWN;
    case INDEP_ENETUNREACH:
	return ENETUNREACH;
    case INDEP_ENETRESET:
	return ENETRESET;
    case INDEP_ECONNABORTED:
	return ECONNABORTED;
    case INDEP_ECONNRESET:
	return ECONNRESET;
    case INDEP_ENOBUFS:
	return ENOBUFS;
    case INDEP_EISCONN:
	return EISCONN;
    case INDEP_ENOTCONN:
	return ENOTCONN;
    case INDEP_ESHUTDOWN:
	return ESHUTDOWN;
    case INDEP_ETOOMANYREFS:
	return ETOOMANYREFS;
    case INDEP_ETIMEDOUT:
	return ETIMEDOUT;
    case INDEP_ECONNREFUSED:
	return ECONNREFUSED;
	/*
	  case INDEP_ELOOP:
	  return ELOOP;
	*/
    case INDEP_ENAMETOOLONG:
	return ENAMETOOLONG;
    case INDEP_EHOSTDOWN:
	return EHOSTDOWN;
    case INDEP_EHOSTUNREACH:
	return EHOSTUNREACH;
    case INDEP_ENOTEMPTY:
	return ENOTEMPTY;
    case INDEP_EPROCLIM:
	return EPROCLIM;
    case INDEP_EUSERS:
	return EUSERS;
    case INDEP_EDQUOT:
	return EDQUOT;
	/*
	  case INDEP_ESTALE:
	  return ESTALE;
	  case INDEP_EREMOTE:
	  return EREMOTE;
	  case INDEP_EBADRPC:
	  return EBADRPC;
	  case INDEP_ERPCMISMATCH:
	  return ERPCMISMATCH;
	  case INDEP_EPROGUNAVAIL:
	  return EPROGUNAVAIL;
	  case INDEP_EPROGMISMATCH:
	  return EPROGMISMATCH;
	  case INDEP_EPROCUNAVAIL:
	  return EPROCUNAVAIL;
	*/
    case INDEP_ENOLCK:
	return ENOLCK;
    case INDEP_ENOSYS:
	return ENOSYS;
	/*
	  case INDEP_EFTYPE:
	  return EFTYPE;
	  case INDEP_EAUTH:
	  return EAUTH;
	  case INDEP_ENEEDAUTH:
	  return ENEEDAUTH;
	  case INDEP_EIDRM:
	  return EIDRM;
	  case INDEP_ENOMSG:
	  return ENOMSG;
	  case INDEP_EOVERFLOW:
	  return EOVERFLOW;
	*/
    case INDEP_EILSEQ:
	return EILSEQ;
	/*
	  case INDEP_ELAST:
	  return ELAST;
	*/
    case INDEP_ERESTART:
	return ERESTART;
    case INDEP_EJUSTRETURN:
	return EJUSTRETURN;
	
    default:
	return IndepErrno; /* maybe it will match */
    }
}

#if 0 /* ioctl requests not currently supported */
/* Ioctl requests: */

int IoctlRequest_ToIndep(int IndepRequestType) {
    switch (IndepRequestType) {
    case FIONBIO:
	return INDEP_FIONBIO;
    case FIOASYNC:
	return INDEP_FIOASYNC;
    case FIONREAD:
	return INDEP_FIONREAD;
	
    case SIOCGIFCONF:
	return INDEP_SIOCGIFCONF;
    case SIOCGIFADDR:
	return INDEP_SIOCGIFADDR;
    case SIOCGIFBRDADDR:
	return INDEP_SIOCGIFBRDADDR;
    case SIOCGIFNETMASK:
	return INDEP_SIOCGIFNETMASK;
	
    default:
	/* ioctl request not supported */
	return -1;
    }
}
#endif

#if 0 /* sendmsg and recvmsg not currently supported */

void MsgHeader_ToIndep(const struct msghdr *msg,
		       IndepMsgHdrStruct *IndepMsgHeader) {
    IndepMsgHeader->field[0] =
	IndepMsgHeader->returnfield[0] = (unsigned) msg->msg_name;
    IndepMsgHeader->field[1] =
	IndepMsgHeader->returnfield[1] = (unsigned) msg->msg_namelen;
    IndepMsgHeader->field[2] =
	IndepMsgHeader->returnfield[2] = (unsigned) msg->msg_iov;
    IndepMsgHeader->field[3] =
	IndepMsgHeader->returnfield[3] = (unsigned) msg->msg_iovlen;
    IndepMsgHeader->field[4] =
	IndepMsgHeader->returnfield[4] = (unsigned) msg->msg_control;
    IndepMsgHeader->field[5] =
	IndepMsgHeader->returnfield[5] = (unsigned) msg->msg_controllen;
    IndepMsgHeader->field[6] =
	IndepMsgHeader->returnfield[6] = (unsigned) msg->msg_flags;
}

void MsgHeader_FromIndep(IndepMsgHdrStruct *IndepMsgHeader,
			 struct msghdr *msg) {
    msg->msg_name = (void *)IndepMsgHeader->returnfield[0];
    msg->msg_namelen = (socklen_t) IndepMsgHeader->returnfield[1];
    msg->msg_iov = (struct iovec *)IndepMsgHeader->returnfield[2];
    msg->msg_iovlen = IndepMsgHeader->returnfield[3];
    msg->msg_control = (void *)IndepMsgHeader->returnfield[4];
    msg->msg_controllen = (socklen_t) IndepMsgHeader->returnfield[5];
    msg->msg_flags = (int) IndepMsgHeader->returnfield[6];
}
#endif
/* END SOCKDEFCONV SNIP */



#endif  /* defined(EMBEDDED) && defined(EMULATE_SOCKET_IO) */

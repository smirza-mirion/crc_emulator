/*
		Language Independent Library

	Copyright 1983-2000 Green Hills Software,Inc.

    This program is the property of Green Hills Software, Inc,
    its contents are proprietary information and no part of it
    is to be disclosed to anyone except employees of Green Hills
    Software, Inc., or as agreed in writing signed by the President
    of Green Hills Software, Inc.
*/

/*
 * As a help to making the C library thread-safe, all references to errno are
 * done through __gh_get_errno and __gh_set_errno, when EMBEDDED is defined.
*/

#include "inddef.h"
#include "inderrno.h"
#include "ind_thrd.h"

/*  programs not compiled with the thread-safe option will have direct
    references to errno.  provide a definition of errno for such programs.  */
#undef errno
int errno;

/* 
    embedded programs whose only use of GetThreadLocalStorage is here will not
    bring in GetThreadLocalStorage and its accompanying 250 bytes of storage.
    Instead they will use the global errno declared here.  This allows most
    C programs to avoid the space overhead of GetThreadLocalStorage, while
    still allowing ANY program to use it if appropriate.
*/
#pragma weak GetThreadLocalStorage

int *__gh_errno_ptr(void)
{
#if defined(MINIMAL_STARTUP)
    return &errno;
#else
    if (!GetThreadLocalStorage) 
	return &errno;
    else {
	ThreadLocalStorage* tlsptr = GetThreadLocalStorage();

	if (!tlsptr)
	    return NULL;

	return &tlsptr->Errno;
    }
#endif
}

void __gh_set_errno(int err)
{
#if defined(MINIMAL_STARTUP)
    errno = err;
#else
    int *p;
    p = __gh_errno_ptr();
    if (p) *p = err;
#endif
}

int  __gh_get_errno(void)
{
#if defined(MINIMAL_STARTUP)
    return errno;
#else
    int *p;
    p = __gh_errno_ptr();
    if (p) return *p;
    else   return 0;
#endif
}

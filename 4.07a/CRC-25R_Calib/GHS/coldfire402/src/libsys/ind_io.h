/*
                    Language Independent Library

            Copyright 1983-2000 Green Hills Software,Inc.

 *  This program is the property of Green Hills Software, Inc,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software, Inc., or as agreed in writing signed by the President
 *  of Green Hills Software, Inc.
*/
/* ind_io.h: declarations for callers of ind_io.c */

#if defined(__VXWORKS) || defined (__OSE)
#  include <unistd.h>
#else

/*      FUNCTIONS NEEDED TO PRINT OUT HELLO WORLD AND ENTER SIMPLE INPUT      */

int write (int fno, const void *buf, int size);
int read (int fno, void *buf, int size);

/*        FUNCTIONS NEEDED TO USE THE HOST FILESYSTEM FROM YOUR TARGET        */

int open (const char *filename, int mode, ...);
int creat (const char *filename, int prot);
int close (int fno);
#ifdef __INTEGRITY_SHARED_LIBS
/* [msantos] Thu Sep 20 2001.
 *   INTEGRITY's version of lseek has off_t defined to be a 64 bit signed
 *   integer.
 */
long long lseek (int fno, long long offset, int end);
#else
long lseek (int fno, long offset, int end);
#endif
int unlink (const char *name);
int fcntl (int fno, int cmd, int arg);

/*                           OPTIONAL FUNCTIONS                               */

int getpid (void);
int remap (void *target, void *source, int length);

#endif /* __VXWORKS || __OSE */

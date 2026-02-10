/*
		    ANSI C Runtime Library

	Copyright 1983-2000 Green Hills Software, Inc.

    This program is the property of Green Hills Software, Inc,
    its contents are proprietary information and no part of it
    is to be disclosed to anyone except employees of Green Hills
    Software, Inc., or as agreed in writing signed by the President
    of Green Hills Software, Inc.
*/

#if !defined(__Ptr_Is_64) || defined(__Int_Is_64)
    typedef int ptrdiff_t;
#else
    typedef long ptrdiff_t;
#endif

#if defined(__Ptr_Is_64) && !defined(__Int_Is_64) || \
      defined(__Ptr_Is_32) && defined(__Long_Is_32) && defined(__Int_Is_16)
    typedef unsigned long size_t;
#else
    typedef unsigned int size_t;
#endif

extern void *memcpy(void *s1, const void *s2, size_t n);
extern void *memset(void *s, int c, size_t n);

#if 0
#pragma ghs section text=".starttext" 
#pragma ghs section rodata=".startrodata" 
#endif

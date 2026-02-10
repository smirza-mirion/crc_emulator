/*
		    ANSI C Runtime Library
	
	Copyright 1983-2000 Green Hills Software, Inc.

    This program is the property of Green Hills Software, Inc,
    its contents are proprietary information and no part of it
    is to be disclosed to anyone except employees of Green Hills
    Software, Inc., or as agreed in writing signed by the President
    of Green Hills Software, Inc.
*/

#include "ind_startup.h"

# if defined(__Ptr_Is_64) && !defined(__Int_Is_64)
typedef signed long signed_size_t;
# else
typedef signed int signed_size_t;
# endif

#define are_both_aligned(x, y, type) \
	(((((size_t)x)|((size_t)y)) &(sizeof(type)-1)) == 0)
#define MANUALLY_UNROLL_LOOP_FOR_SPEED 1

#define copy_one_by_type(x, y, type)				   \
        {                                                          \
	type * tx = (type *)x; const type * ty = (const type *)y;  \
	*tx++ = *ty++;                                             \
	x = tx; y = ty;                                            \
    } (void)0                                               

#define copy_four_by_type(x, y, type)				   \
        {                                                          \
	type * tx = (type *)x; const type * ty = (const type *)y;  \
	*tx++ = *ty++;                                             \
	*tx++ = *ty++;                                             \
	*tx++ = *ty++;                                             \
	*tx++ = *ty++;                                             \
	x = tx; y = ty;                                            \
    } (void)0                                               
	
void *memcpy(void *s1, const void *s2, size_t n)
{
    void * result = s1;

    if ((sizeof(int) > sizeof(char)) && are_both_aligned(s1, s2, int)) {
	if ((sizeof(long) > sizeof(int)) && are_both_aligned(s1, s2, long)) {
#if MANUALLY_UNROLL_LOOP_FOR_SPEED
	    while (n >=  4*sizeof(long)) {
		n -= 4*sizeof(long);
		copy_four_by_type(s1, s2, long);
	    }
#endif	    
	    while (n >= sizeof(long)) {
		n -= sizeof(long);
		copy_one_by_type(s1, s2, long);
	    }
	} else {
#if MANUALLY_UNROLL_LOOP_FOR_SPEED
	    while (n >= 4*sizeof(int)) {
		n -= 4*sizeof(int);
		copy_four_by_type(s1, s2, int);
	    }
#endif	    
	    while (n >= sizeof(int)) {
		n -= sizeof(int);
		copy_one_by_type(s1, s2, int);
	    }
	}
#if MANUALLY_UNROLL_LOOP_FOR_SPEED
    } else {
	while (n >= 4*sizeof(char)) {
	    n -= 4*sizeof(char);
	    copy_four_by_type(s1, s2, char);
	}
#endif
    }

    while ((signed_size_t)--n >= 0)
	copy_one_by_type(s1, s2, char);

    return result;    
}

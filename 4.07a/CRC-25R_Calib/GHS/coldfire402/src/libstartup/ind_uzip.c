/*----------------------------------------------------------------------*/
/* ind_uzip.c: ROM decompression code.					*/
/*									*/
/* CALLED FROM:  __ghs_ind_crt0		  				*/
/* ENTRY POINT:  __ghs_decompress 					*/
/*									*/
/* Decompression code based upon LZSS by Haruhiko Okumura		*/
/*                                                                      */
/* This code was modified by GHS to not require a temporary ring buffer */
/*----------------------------------------------------------------------*/

#include "ind_startup.h"

/* Data for the decompression algorithm used for CROM */
#define DECODE_N 	  4096     /* size of ring buffer */
#define DECODE_F 	  18       /* upper limit for match_length */
#define DECODE_THRESHOLD  2        /* encode string into position and length
				    if match_length is greater than this */

/*----------------------------------------------------------------------*/
/*									*/
/*	Decompress and Copy from CROM to RAM				*/
/*									*/
/*----------------------------------------------------------------------*/
void __ghs_decompress(void *dst, const void *src, size_t n)
{
    /* unsigned char temp_buf[DECODE_N + DECODE_F - 1]; */
    int r,c;
    unsigned int flags;

    const unsigned char *s = (const unsigned char *)src;
    unsigned char *t = (unsigned char *)dst;

    const unsigned char *s_stop = s + n;
    unsigned char *t_start = t;

    /* Decompress and copy */
    /* for (i = 0; i < DECODE_N - DECODE_F; i++) temp_buf[i] = '\0'; */
    r = DECODE_N - DECODE_F;
    flags = 0;
    while (s != s_stop) {
	if (((flags >>= 1) & 0x100) == 0) {
	    c = *s++;
	    flags = c | 0xff00;   /* ues higher byte cleverly */
	}                         /* to count eight */
	if (flags & 1) {
	    c = *s++;
	    *t++ = (char)c; /* temp_buf[r++] = c; */
	    r = ((r + 1) & (DECODE_N - 1));
	} else {
	    unsigned char* pattern;	    
	    int patloc, patlen, i;

	    patloc = *s++;
	    patlen = *s++;
	    patloc |= ((patlen & 0xf0) << 4);
	    patlen = (patlen & 0x0f) + DECODE_THRESHOLD + 1;

	    pattern = t - (r - patloc);
	    if (r <= patloc)
		pattern -= DECODE_N;
	    
	    for (i=0; (pattern < t_start) && (i < patlen); i++) {
		*t++ = 0;
		pattern++;
	    }
	    for ( ; i < patlen; i++) {
		*t++ = *pattern++;
	    }
	    r = ((r + patlen) & (DECODE_N - 1));	    
	}
    }
}

/*
                    Language Independent Library

            Copyright 1983-2000 Green Hills Software,Inc.

 *  This program is the property of Green Hills Software, Inc,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software, Inc., or as agreed in writing signed by the President
 *  of Green Hills Software, Inc.
*/
/* ind_exit.h: declarations for callers of ind_exit.c */

/*   FUNCTIONS ALWAYS NEEDED TO RUN PROGRAMS COMPILED WITH THE DEFAULT CRT0   */

void _enter (void);
struct __GHS_AT_EXIT {
    void (*func)(void);
    struct __GHS_AT_EXIT *next;
};
void _exit (int code);
void __ghs_at_exit (struct __GHS_AT_EXIT *gae);
void DefaultSignalHandler(int);

#ifndef _MMCTEST_H_
#define _MMCTEST_H_

/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

#include "fat.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 *
 *  Exports
 *
 ***************************************************************************/

extern void f_dommctest(void);


/****************************************************************************
 *
 *	Imports
 *
 ***************************************************************************/

//extern int _f_poweron(void); /* use to build file system */
extern int _f_mmcpoweron(void); /* use to build file system */
extern void _f_dump(char *s); /* use to display text */
extern long _f_result(long testnum,long result); /* use to display test result */

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of test.h
 *
 ***************************************************************************/

#endif	/* _MMCTEST_H_ */

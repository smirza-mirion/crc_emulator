#ifndef _MMCAPI_H_
#define _MMCAPI_H_

/****************************************************************************
 *
 *            Copyright (c) 2004 by Capintec Inc
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

extern unsigned long f_getrand(unsigned long rand);
extern unsigned short f_getdate(void);
extern unsigned short f_gettime(void);

extern int mmc_display_mbr(void);
extern int mmc_make_directory(char *dirname);
extern int mmc_get_directory(char *dirname, int maxlen);
extern int mmc_display_dir(char *dir);
extern int mmc_read_file(char *filename, char *buffer, unsigned long size);
extern int mmc_create_file(char *filename, char *buffer, unsigned long length);
extern int mmc_delete_file(char *filename);
extern int mmc_reformat(void);
extern int mmc_hardformat(void);
extern int mmc_change_dir(char *direc);
extern int mmc_rename_file(const char *oldname, const char *newname);
extern int mmc_settimedate(char *filename);
extern int mmc_gettimedate(char *filename, unsigned short *pctime, unsigned short *pcdate);
extern int mmc_write_volname(char *volname);
extern int mmc_file_exists(char *filename);
extern F_FILE* mmc_create_file_for_append(char *filename);
extern F_FILE* mmc_open_file_for_append(char *filename);
extern void mmc_append_to_file(F_FILE *file, char *buffer, unsigned long length);
extern int mmc_close_file(F_FILE *file);
#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of mmcapi.h
 *
 ***************************************************************************/

#endif /* _MMCAPI_H_ */


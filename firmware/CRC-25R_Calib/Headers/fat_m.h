#ifndef _FAT_M_H_
#define _FAT_M_H_

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

#ifdef __cplusplus
extern "C" {
#endif

extern char *fm_getversion(void);
extern int fm_initvolume(int drvnumber,F_INITFUNC pfunc);
extern int fm_hardformat(int drivenum,long fattype);
extern int fm_format(int drivenum,long fattype);
extern int fm_getcwd(char *buffer, int maxlen );
extern int fm_getdcwd(int drivenum, char *buffer, int maxlen );
extern int fm_chdrive(int drivenum);
extern int fm_getdrive(void);
extern int fm_getfreespace(int drivenum,F_SPACE *pspace);

extern int fm_chdir(const char *dirname);
extern int fm_mkdir(const char *dirname);
extern int fm_rmdir(const char *dirname);

extern int fm_findfirst(const char *filename,F_FIND *find);
extern int fm_findnext(F_FIND *find);
extern int fm_rename(const char *filename, const char *newname);
extern long fm_filelength(const char *filename);

extern int fm_close(F_FILE *filehandle);
extern F_FILE *fm_open(const char *filename,const char *mode);

extern long fm_read(void *buf,long size,long size_st,F_FILE *filehandle);
extern long fm_write(const void *buf,long size,long size_st,F_FILE *filehandle);

extern int fm_seek(F_FILE *filehandle,long offset,long whence);

extern long fm_tell(F_FILE *filehandle);
extern int fm_getc(F_FILE *filehandle);
extern int fm_putc(int ch,F_FILE *filehandle);
extern int fm_rewind(F_FILE *filehandle);
extern int fm_eof(F_FILE *filehandle);

extern int fm_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate);
extern int fm_settimedate(const char *filename,unsigned short ctime,unsigned short cdate);
extern int fm_delete(const char *filename);

extern int fm_getattr(const char *filename,unsigned char *attr);
extern int fm_setattr(const char *filename,unsigned char attr);

extern int fm_getlabel(int drivenum, char *label, long len);
extern int fm_setlabel(int drivenum, const char *label);

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of fat_m.h
 *
 ***************************************************************************/

#endif /* _FAT_M_H_ */

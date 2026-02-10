#ifndef _FAT_M_C_
#define _FAT_M_C_

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

#if (F_REENTRANCY)

/*  THIS MODULE IS FOR REENTRANCY  */
/*  PLEASE CHECK FAT.H  */

/****************************************************************************
 *
 * _f_checksemaphore
 *
 * Checking if there is a user who already using file system.
 * Only one user could access file system, next user should
 * wait until 1st user finising using.
 *
 *
 * RETURNS
 *
 * 0 - if access granted
 * 1 - if busy
 *
 ***************************************************************************/
		 
static volatile long gdw_semaphore; /* this is used for semaphore */

static int _f_checksemaphore(void) {
long semaphore;
long timeout=1000; /* 1000 retry */
long wait;
   do {

/*  add disable interrupt function here  */

      semaphore=gdw_semaphore;
      gdw_semaphore=1; /* set to busy */

/*  add enable interrupt function here  */

      if (!semaphore) return 0; /* free to use */

      for (wait=10000;;) { /* ~10ms on 10Mips */
         wait--;
         if (!wait) break;
      }

   } while (timeout--);

   return 1; /* busy */
}

/****************************************************************************
 *
 * _f_releasesemaphore
 *
 * Release used semaphore
 *
 ***************************************************************************/

static void _f_releasesemaphore(void) {
   gdw_semaphore=0; /* release it */
}

/****************************************************************************
 *
 * fm_getversion
 *
 * returns with the filesystem version string
 *
 * RETURNS
 *
 * string pointer with version number
 *
 ***************************************************************************/

char *fm_getversion(void) {
   return fn_getversion();
}

/****************************************************************************
 *
 * fm_initvolume
 *
 * initiate a volume, this function has to be called 1st to set physical
 * driver function to a given volume
 *
 * INPUTS
 *
 * drvnumber - which drive need to be initialized
 * pfunc - function pointer which will be called to get driver functions
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_initvolume(int drvnumber,F_INITFUNC pfunc) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_initvolume(drvnumber,pfunc);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_hardformat
 *
 * Making a complete format on media, independently from master boot record,
 * according to media physical
 *
 * INPUTS
 *
 * drivenum - which drive low format is needed
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_hardformat(int drivenum,long fattype) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_hardformat(drivenum,fattype);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_format
 *
 * format a media, 1st it checks existing formatting, then master boot record,
 * then media physical
 *
 * INPUTS
 *
 * drivenum - which drive format is needed
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_format(int drivenum,long fattype) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_format(drivenum,fattype);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_getcwd
 *
 * getting a current working directory of current drive
 *
 * INPUTS
 *
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is F_MAXPATH)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_getcwd(char *buffer, int maxlen ) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_getcwd(buffer,maxlen);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_getdcwd
 *
 * getting a drive current working directory
 *
 * INPUTS
 *
 * drivenum - drive number of which drive current folder needed
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is F_MAXPATH)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_getdcwd(int drivenum, char *buffer, int maxlen ) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_getdcwd(drivenum,buffer,maxlen);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_chdrive
 *
 * Change current drive
 *
 * INPUTS
 *
 * drivenum - new current drive number (0-A, 1-B, ...)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_chdrive(int drivenum) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_chdrive(drivenum);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_getdrive
 *
 * Get current drive number
 *
 * RETURNS
 *
 * with the current drive number (0-A, 1-B,...)
 *
 ***************************************************************************/

int fm_getdrive(void) {
   return fn_getdrive();
}

/****************************************************************************
 *
 * fm_getfreespace
 *
 * get total/free/used/bad diskspace
 *
 * INPUTS
 *
 * drivenum - which drive free space is requested (0-A, 1-B, 2-C)
 * pspace - pointer where to store the information
 *
 * RETURNS
 * error code
 *
 ***************************************************************************/

int fm_getfreespace(int drivenum,F_SPACE *pspace) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY;

   ret=fn_getfreespace(drivenum,pspace);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_chdir
 *
 * change current working directory
 *
 * INPUTS
 *
 * dirname - new working directory name
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error 
 *
 ***************************************************************************/

int fm_chdir(const char *dirname) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_chdir(dirname);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_mkdir
 *
 * making a new directory
 *
 * INPUTS
 *
 * dirname - new directory name
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_mkdir(const char *dirname) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_mkdir(dirname);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_rmdir
 *
 * Remove directory, only could be removed if empty
 *
 * INPUTS
 *
 * dirname - which directory needed to be removed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_rmdir(const char *dirname) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_rmdir(dirname);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_findfirst
 *
 * find a file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * filename - filename (with or without wildcards)
 * find - where to store found file information
 *
 * RETURNS
 *																			 
 * error code or zero if successful
 *																			 
 ***************************************************************************/

int fm_findfirst(const char *filename,F_FIND *find) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_findfirst(filename,find);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_findnext
 *
 * find further file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * find - where to store found file information (findfirst should call 1st)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_findnext(F_FIND *find) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_findnext(find);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_rename
 *
 * Rename file or directory
 *
 * INPUTS
 *
 * filename - filename or directory name (with or without path)
 * newname - new name of the file or directory (without path)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error 
 *
 ***************************************************************************/

int fm_rename(const char *filename, const char *newname) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_rename(filename,newname);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_filelength
 *
 * Get a file length
 *
 * INPUTS
 *
 * filename - file whose length is needed
 *
 * RETURNS
 *
 * length of the file
 *
 ***************************************************************************/

long fm_filelength(const char *filename) {
long ret;

   if (_f_checksemaphore()) return 0; /* busy! */

   ret=fn_filelength(filename);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_close
 *
 * close a previously opened file
 *
 * INPUTS
 *
 * filehandle - which file needs to be closed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_close(F_FILE *filehandle) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_close(filehandle);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_open
 *
 * open a file for reading/writing/appending
 *
 * INPUTS
 *
 * filename - which file need to be opened
 * mode - string how to open ("r"-read, "w"-write, "w+"-overwrite, "a"-append
 *
 * RETURNS
 *
 * F_FILE pointer if successfully
 * 0 - if any error
 *
 ***************************************************************************/

F_FILE *fm_open(const char *filename,const char *mode) {
F_FILE *ret;

   if (_f_checksemaphore()) return 0; /* busy! */

   ret=fn_open(filename,mode);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_read
 *
 * read data from file
 *
 * INPUTS
 *
 * buf - where the store data 
 * size - size of items to be read
 * size_st - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of read bytes
 *
 ***************************************************************************/

long fm_read(void *buf,long size,long size_st,F_FILE *filehandle) {
long ret;

   if (_f_checksemaphore()) return 0; /* busy! */

   ret=fn_read(buf,size,size_st,filehandle);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_write
 *
 * write data into file
 *
 * INPUTS
 *
 * buf - where the store data 
 * size - size of items to be read
 * size_st - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of read bytes
 *
 ***************************************************************************/

long fm_write(const void *buf,long size,long size_st,F_FILE *filehandle) {
long ret;

   if (_f_checksemaphore()) return 0; /* busy! */

   ret=fn_write(buf,size,size_st,filehandle);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_seek
 *
 * moves position into given offset in given file
 *
 * INPUTS
 *
 * filehandle - F_FILE structure which file position needed to be modified
 * offset - relative position 
 * whence - where to calculate position (F_SEEK_SET,F_SEEK_CUR,F_SEEK_END)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error 
 *
 ***************************************************************************/

int fm_seek(F_FILE *filehandle,long offset,long whence) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_seek(filehandle,offset,whence);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_tell
 *
 * Tells the current position of opened file
 *
 * INPUTS
 *
 * filehandle - which file needs the position
 *
 * RETURNS
 *
 * position in the file from start
 *
 ***************************************************************************/

long fm_tell(F_FILE *filehandle) {
long ret;

   if (_f_checksemaphore()) return 0; /* busy! */

   ret=fn_tell(filehandle);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_getc
 *
 * get a character from file
 *
 * INPUTS
 *
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the read character or -1 if read was not successfully 
 *
 ***************************************************************************/

int fm_getc(F_FILE *filehandle) {
int ret;

   if (_f_checksemaphore()) return -1; /* busy! */

   ret=fn_getc(filehandle);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_putc
 *
 * write a character into file
 *
 * INPUTS
 *
 * ch - what to write into file
 * filehandle - file where to write
 *
 * RETURNS
 *
 * with the written character or -1 if any error
 *
 ***************************************************************************/

int fm_putc(int ch,F_FILE *filehandle) {
int ret;

   if (_f_checksemaphore()) return -1; /* busy! */

   ret=fn_putc(ch,filehandle);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_rewind
 *
 * set the fileposition in the opened file to the begining
 *
 * INPUTS
 *
 * filehandle - which file needs to be rewinded
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_rewind(F_FILE *filehandle) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_rewind(filehandle);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_eof
 *
 * Tells if the current position is end of file or not
 *
 * INPUTS
 *
 * filehandle - which file needs the checking
 *
 * RETURNS
 *
 * 0 - if not EOF
 * other - if EOF or invalid file handle
 *
 ***************************************************************************/

int fm_eof(F_FILE *filehandle) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_eof(filehandle);

   _f_releasesemaphore();

   return ret;
}


/****************************************************************************
 *
 * fm_gettimedate
 *
 * get a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be retrive
 * pctime - ctime of the file where to store
 * pcdate - cdate of the file where to store
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_gettimedate(filename,pctime,pcdate);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_settimedate
 *
 * set a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be set
 * ctime - new ctime of the file
 * cdate - new cdate of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_settimedate(const char *filename,unsigned short ctime,unsigned short cdate) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_settimedate(filename,ctime,cdate);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_delete
 *
 * delete a file 
 *
 * INPUTS
 *
 * filename - file which wanted to be deleted (with or without path)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_delete(const char *filename) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_delete(filename);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_getattr
 *
 * get file attribute
 *
 * INPUTS
 *
 * filename - which file attribute is needed
 * attr - pointer to a characterter where to store attribute information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_getattr(const char *filename,unsigned char *attr) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_getattr(filename,attr);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_setattr
 *
 * set attribute of a file
 *
 * INPUTS
 *
 * filename - which file attribute set
 * attr - new attribute of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_setattr(const char *filename,unsigned char attr) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_setattr(filename,attr);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_getlabel
 *
 * get a label of a media
 *
 * INPUTS
 *
 * drivenum - drive number which label's is needed
 * label - char pointer where to store label
 * len - length of label buffer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_getlabel(int drivenum, char *label, long len) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_getlabel(drivenum,label,len);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * fm_setlabel
 *
 * set a label of a media
 *
 * INPUTS
 *
 * drivenum - drive number which label's need to be set
 * label - new label for the media
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fm_setlabel(int drivenum, const char *label) {
int ret;

   if (_f_checksemaphore()) return F_ERR_BUSY; /* busy! */

   ret=fn_setlabel(drivenum,label);

   _f_releasesemaphore();

   return ret;
}

/****************************************************************************
 *
 * end of fat_m.c
 *
 ***************************************************************************/

#endif /* F_REENTRANCY */
#endif /* _FAT_M_C_ */


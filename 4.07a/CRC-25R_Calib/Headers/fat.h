#ifndef _FAT_H_
#define _FAT_H_

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

/****************************************************************************
 *
 * Defines F_LONGFILENAME if long file name is needed according to doc.
 *
 ***************************************************************************/

#define F_LONGFILENAME 1 /*  0 - 8+3 names   1 - long file names   */

/****************************************************************************
 *
 * defines of reentrancy
 *
 ***************************************************************************/

#define F_REENTRANCY 1  /* defines reentrancy in multitask system */

/****************************************************************************
 *
 * some definition
 *
 ***************************************************************************/

//#define F_MAXVOLUME	5     /* maximum number of volumes */
//#define F_MAXFILES  10     /* maximum number of files */
#define F_MAXVOLUME	1     /* maximum number of volumes */
#define F_MAXFILES  50     /* maximum number of files */

#define F_SECTOR_SIZE 512
#define F_MAXPATH 256
#define F_MAXNAME 8		/*  8 byte name  */
#define F_MAXEXT  3		/*  3 byte extension  */


typedef struct {
unsigned char jump_code[3];
unsigned char OEM_name [8];
unsigned short bytes_per_sector;
unsigned char sector_per_cluster;
unsigned short reserved_sectors;
unsigned char number_of_FATs;
unsigned short max_root_entry;
unsigned short number_of_sectors_less32; /*  <32M  */
unsigned char media_descriptor;
unsigned short sector_per_FAT;
unsigned short sector_per_Track;
unsigned short number_of_heads;
unsigned long number_of_hidden_sectors;
unsigned long number_of_sectors;

/* only on fat32 */
unsigned long sector_per_FAT32;
unsigned short extflags;
unsigned short fsversion;
unsigned long rootcluster;
unsigned short fsinfo;
unsigned short bkbootsec;
unsigned char reserved[12];

/* fat12-fat16-fat32 */

unsigned short logical_drive_num;
unsigned char extended_signature;
unsigned long serial_number;
unsigned char volume_name[11];
unsigned char FAT_name[8];
unsigned char executable_marker[2];
} F_BOOTRECORD;


typedef struct {
unsigned short number_of_cylinders;
unsigned short sector_per_track;
unsigned short number_of_heads;
unsigned long number_of_sectors;
} F_PHY;


#if (!F_LONGFILENAME)

typedef struct {
   int drivenum; 	/*  0-A 1-B 2-C  */
	char path [F_MAXPATH];		/*   /directory1/dir2/   */
	char filename[F_MAXNAME];	/*  filename  */
	char fileext [F_MAXEXT];  	/*  extension  */
} F_NAME;

#else

#define F_MAXLNAME 256

typedef struct {
   int drivenum; 	/*  0-A 1-B 2-C  */
	char path  [F_MAXPATH];		/*   /directory1/dir2/   */
	char lname [F_MAXLNAME];	/*  long file name   */
} F_NAME;

#endif /*  F_LONGFILENAME   */

typedef struct {
	unsigned long cluster;
	unsigned long sector;
	unsigned long sectorend;
	unsigned long pos;
} F_POS;

typedef struct {
	char filename[F_MAXPATH];	/* file name+ext */
	char name[F_MAXNAME];		/* file name */
	char ext[F_MAXEXT];		/* file extension */
	char attr;					/* attribute of the file */

	unsigned short ctime;		/* creation time */
	unsigned short cdate;		/* creation date */
	unsigned long cluster;

	long filesize;					/* length of file */

	F_NAME findfsname;		   	/* find properties */
	F_POS pos;
} F_FIND;


#if F_LONGFILENAME

typedef struct {
   unsigned char name[261]; /* with zero term */
   unsigned char ord;
   unsigned char chksum;
   unsigned char state;
   unsigned long start;
   unsigned long end;
} F_LFNINT;

enum {
F_LFNSTATE_LFN,  /* lfn is useable */
F_LFNSTATE_SFN,  /* lfn is useable, contains short filename */
F_LFNSTATE_NEXT, /* lfn need more entry */
F_LFNSTATE_INV   /* invalid lfn */
};

#endif  /* F_LONGFILENAME */

typedef struct {
unsigned char name[F_MAXNAME];	/* 8+3 */
unsigned char ext[F_MAXEXT];
unsigned char attr;					/* 00ADVSHR */

unsigned char ntres;      /* FAT32 only  */
unsigned char crttimetenth;    /* FAT32 only  */
unsigned char crttime[2];   /* FAT32 only  */
unsigned char crtdate[2];   /* FAT32 only */
unsigned char lastaccessdate[2];/* FAT32 only */

unsigned char clusterhi[2]; /* FAT32 only */
unsigned char ctime[2];
unsigned char cdate[2];
unsigned char clusterlo[2]; /* fat12,fat16,fat32 */
unsigned char filesize[4];

} F_DIRENTRY;


#define NTRES_LOW 0x08 /* lower case name */

#define F_ATTR_ARC      0x20
#define F_ATTR_DIR      0x10
#define F_ATTR_VOLUME   0x08
#define F_ATTR_SYSTEM   0x04
#define F_ATTR_HIDDEN   0x02
#define F_ATTR_READONLY 0x01

#if F_LONGFILENAME


typedef struct {
unsigned char ord;

unsigned char lfn_1;
unsigned char lfnhi_1;

unsigned char lfn_2;
unsigned char lfnhi_2;

unsigned char lfn_3;
unsigned char lfnhi_3;

unsigned char lfn_4;
unsigned char lfnhi_4;

unsigned char lfn_5;
unsigned char lfnhi_5;

unsigned char attr;	/* 00ADVSHR */
unsigned char type;  /* always 0 */

unsigned char chksum;

unsigned char lfn_6;
unsigned char lfnhi_6;

unsigned char lfn_7;
unsigned char lfnhi_7;

unsigned char lfn_8;
unsigned char lfnhi_8;

unsigned char lfn_9;
unsigned char lfnhi_9;

unsigned char lfn_10;
unsigned char lfnhi_10;

unsigned char lfn_11;
unsigned char lfnhi_11;

unsigned char clusterlo[2]; /* fat12,fat16,fat32 */

unsigned char lfn_12;
unsigned char lfnhi_12;

unsigned char lfn_13;
unsigned char lfnhi_13;

} F_LFN;


#define F_ATTR_LFN      (F_ATTR_VOLUME|F_ATTR_SYSTEM|F_ATTR_HIDDEN|F_ATTR_READONLY)

#endif /* F_LONGFILENAME  */

#define F_CLUSTER_FREE	   ((unsigned long)0x00000000)
#define F_CLUSTER_RESERVED ((unsigned long)0x0ffffff0)
#define F_CLUSTER_BAD	   ((unsigned long)0x0ffffff7)
#define F_CLUSTER_LAST	   ((unsigned long)0x0ffffff8)
#define F_CLUSTER_LASTF32R ((unsigned long)0x0fffffff)

typedef int (*F_WRITESECTOR)(void *data, unsigned long sector);
typedef int (*F_READSECTOR)(void *data, unsigned long sector);
typedef int (*F_GETPHY)(F_PHY *phy);
typedef long (*F_GETSTATUS)(void);

#define F_ST_MISSING   0x00000001
#define F_ST_CHANGED   0x00000002
#define F_ST_WRPROTECT 0x00000004

typedef struct {
	unsigned long sector;  /* start sector */
	unsigned long num;	   /* number of sectors */
} F_SECTOR;

typedef struct {
	long reference;		/* reference which fileint used */
} F_FILE;

/*  definitions for F_FILE internally used  */

typedef struct {
	F_FILE file;
	long modified;
	long mode;
   int drivenum; 	/*  0-A 1-B 2-C  */
	unsigned long abspos;
	unsigned long relpos;
	unsigned long filesize;
	unsigned char data[F_SECTOR_SIZE];
	unsigned long startcluster;
	unsigned long prevcluster;
	F_POS pos;
	F_POS dirpos;
} F_FILEINT;

typedef struct {
  F_WRITESECTOR writesector;
  F_READSECTOR readsector;
  F_GETPHY getphy;
  F_GETSTATUS getstatus;
} F_FUNC;

typedef int (*F_INITFUNC)(F_FUNC *pfunc);

typedef struct {
  long state;

  F_BOOTRECORD bootrecord;

  F_SECTOR firstfat;

  F_SECTOR root;
  F_SECTOR data;

  unsigned char fat[F_SECTOR_SIZE];
  unsigned long fatsector;
  long fatmodified;
  unsigned char direntry[F_SECTOR_SIZE];
  unsigned long direntrysector;
  unsigned long lastalloccluster;

  F_FUNC func;

  char cwd[F_MAXPATH];		/* current working folder in this volume */
  long mediatype;
} F_VOLUME;

enum {
F_UNKNOWN_MEDIA,
F_FAT12_MEDIA,
F_FAT16_MEDIA,
F_FAT32_MEDIA
};

typedef struct {
	F_VOLUME volumes[F_MAXVOLUME];	/* volumes */
	int curdrive;					   /* current drive */
	F_FILEINT files[F_MAXFILES];
	unsigned char sectorbuffer[F_SECTOR_SIZE];
} F_FILESYSTEM;


#define F_FILE_CLOSE	0
#define F_FILE_RD		1
#define F_FILE_WR		2
#define F_FILE_A		3
#define F_FILE_RDP		4
#define F_FILE_WRP		5
#define F_FILE_AP		6
#define F_FILE_WRERR	7

typedef struct {
   unsigned long total;
   unsigned long free;
   unsigned long used;
   unsigned long bad;
} F_SPACE;

enum {
/*  0 */ F_STATE_NONE,
/*  1 */ F_STATE_NEEDMOUNT,
/*  2 */ F_STATE_WORKING
};

enum {
/*  0 */ F_NO_ERROR,
/*  1 */ F_ERR_INVALIDDRIVE,
/*  2 */ F_ERR_NOTFORMATTED,
/*  3 */ F_ERR_INVALIDDIR,
/*  4 */ F_ERR_INVALIDNAME,
/*  5 */ F_ERR_NOTFOUND,
/*  6 */ F_ERR_DUPLICATED,
/*  7 */ F_ERR_NOMOREENTRY,
/*  8 */ F_ERR_NOTOPEN,
/*  9 */ F_ERR_EOF,
/* 10 */ F_ERR_RESERVED,
/* 11 */ F_ERR_NOTUSEABLE,
/* 12 */ F_ERR_LOCKED,
/* 13 */ F_ERR_ACCESSDENIED,
/* 14 */ F_ERR_NOTEMPTY,
/* 15 */ F_ERR_INITFUNC,
/* 16 */ F_ERR_CARDREMOVED,
/* 17 */ F_ERR_ONDRIVE,
/* 18 */ F_ERR_INVALIDSECTOR,
/* 19 */ F_ERR_READ,
/* 20 */ F_ERR_WRITE,
/* 21 */ F_ERR_INVALIDMEDIA,
/* 22 */ F_ERR_BUSY,
/* 23 */ F_ERR_WRITEPROTECT,
/* 24 */ F_ERR_INVFATTYPE,
/* 25 */ F_ERR_MEDIATOOSMALL,
/* 26 */ F_ERR_MEDIATOOLARGE,
/* 27 */ F_ERR_NOTSUPPSECTORSIZE
};

/*  definitions for ctime  */

#define F_CTIME_SEC_SHIFT		0
#define F_CTIME_SEC_MASK	0x001f	/* 0-30 in 2seconds */
#define F_CTIME_MIN_SHIFT		5
#define F_CTIME_MIN_MASK	0x07e0	/* 0-59  */
#define F_CTIME_HOUR_SHIFT		11
#define F_CTIME_HOUR_MASK	0xf800	/* 0-23 */

/*  definitions for cdate  */

#define F_CDATE_DAY_SHIFT		0
#define F_CDATE_DAY_MASK	0x001f	/* 0-31 */
#define F_CDATE_MONTH_SHIFT	5
#define F_CDATE_MONTH_MASK	0x01e0	/* 1-12 */
#define F_CDATE_YEAR_SHIFT		9
#define F_CDATE_YEAR_MASK	0xfe00	/* 0-119 (1980+value) */

/* Beginning of file */
#ifdef SEEK_SET
#define F_SEEK_SET SEEK_SET
#else
#define F_SEEK_SET 0
#endif

/* Current position of file pointer */
#ifdef SEEK_CUR
#define F_SEEK_CUR SEEK_CUR
#else
#define F_SEEK_CUR 1
#endif

/* End of file */
#ifdef SEEK_END
#define F_SEEK_END SEEK_END
#else
#define F_SEEK_END 2
#endif

extern char *fn_getversion(void);
extern int fn_initvolume(int drvnumber,F_INITFUNC pfunc);
extern int fn_hardformat(int drivenum,long fattype);
extern int fn_format(int drivenum,long fattype);
extern int fn_getcwd(char *buffer, int maxlen );
extern int fn_getdcwd(int drivenum, char *buffer, int maxlen );
extern int fn_chdrive(int drivenum);
extern int fn_getdrive(void);
extern int fn_getfreespace(int drivenum, F_SPACE *pspace);

extern int fn_chdir(const char *dirname);
extern int fn_mkdir(const char *dirname);
extern int fn_rmdir(const char *dirname);

extern int fn_findfirst(const char *filename,F_FIND *find);
extern int fn_findnext(F_FIND *find);
extern int fn_rename(const char *filename, const char *newname);
extern long fn_filelength(const char *filename);

extern int fn_close(F_FILE *filehandle);
extern F_FILE *fn_open(const char *filename,const char *mode);

extern long fn_read(void *buf,long size,long size_st,F_FILE *filehandle);
extern long fn_write(const void *buf,long size,long size_st,F_FILE *filehandle);

extern int fn_seek(F_FILE *filehandle,long offset,long whence);

extern long fn_tell(F_FILE *filehandle);
extern int fn_getc(F_FILE *filehandle);
extern int fn_putc(int ch,F_FILE *filehandle);
extern int fn_rewind(F_FILE *filehandle);
extern int fn_eof(F_FILE *filehandle);

extern int fn_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate);
extern int fn_settimedate(const char *filename,unsigned short ctime,unsigned short cdate);
extern int fn_delete(const char *filename);

extern int fn_getattr(const char *filename,unsigned char *attr);
extern int fn_setattr(const char *filename,unsigned char attr);

extern int fn_getlabel(int drivenum, char *label, long len);
extern int fn_setlabel(int drivenum, const char *label);


#if (!F_REENTRANCY)

#define f_getversion fn_getversion
#define f_initvolume(drvnumber,pfunc) fn_initvolume(drvnumber,pfunc)
#define f_hardformat(drivenum,fattype) fn_hardformat(drivenum,fattype)
#define f_format(drivenum,fattype) fn_format(drivenum,fattype)
#define f_getcwd(buffer,maxlen) fn_getcwd(buffer,maxlen)
#define f_getdcwd(drivenum,buffer,maxlen) fn_getdcwd(drivenum,buffer,maxlen)
#define f_chdrive(drivenum) fn_chdrive(drivenum)
#define f_getdrive fn_getdrive
#define f_getfreespace(drivenum,pspace) fn_getfreespace(drivenum,pspace)

#define f_chdir(dirname) fn_chdir(dirname)
#define f_mkdir(dirname) fn_mkdir(dirname)
#define f_rmdir(dirname) fn_rmdir(dirname)

#define f_findfirst(filename,find) fn_findfirst(filename,find)
#define f_findnext(find) fn_findnext(find)
#define f_rename(filename,newname) fn_rename(filename,newname)
#define f_filelength(filename) fn_filelength(filename)

#define f_close(filehandle) fn_close(filehandle)
#define f_open(filename,mode) fn_open(filename,mode)

#define f_read(buf,size,size_st,filehandle) fn_read(buf,size,size_st,filehandle)
#define f_write(buf,size,size_st,filehandle) fn_write(buf,size,size_st,filehandle)

#define f_seek(filehandle,offset,whence) fn_seek(filehandle,offset,whence)

#define f_tell(filehandle) fn_tell(filehandle)
#define f_getc(filehandle) fn_getc(filehandle)
#define f_putc(ch,filehandle) fn_putc(ch,filehandle)
#define f_rewind(filehandle) fn_rewind(filehandle)
#define f_eof(filehandle) fn_eof(filehandle)

#define f_gettimedate(filename,pctime,pcdate) fn_gettimedate(filename,pctime,pcdate)
#define f_settimedate(filename,ctime,cdate) fn_settimedate(filename,ctime,cdate)
#define f_delete(filename) fn_delete(filename)

#define f_getattr(filename,attr) fn_getattr(filename,attr)
#define f_setattr(filename,attr) fn_setattr(filename,attr)

#define f_getlabel(drivenum,label,len) fn_getlabel(drivenum,label,len)
#define f_setlabel(drivenum,label) fn_setlabel(drivenum,label)

#else

#include "fat_m.h"

#define f_getversion fm_getversion
#define f_initvolume(drvnumber,pfunc) fm_initvolume(drvnumber,pfunc)
#define f_hardformat(drivenum,fattype) fm_hardformat(drivenum,fattype)
#define f_format(drivenum,fattype) fm_format(drivenum,fattype)
#define f_getcwd(buffer,maxlen) fm_getcwd(buffer,maxlen)
#define f_getdcwd(drivenum,buffer,maxlen) fm_getdcwd(drivenum,buffer,maxlen)
#define f_chdrive(drivenum) fm_chdrive(drivenum)
#define f_getdrive fm_getdrive
#define f_getfreespace(drivenum,pspace) fm_getfreespace(drivenum,pspace)

#define f_chdir(dirname) fm_chdir(dirname)
#define f_mkdir(dirname) fm_mkdir(dirname)
#define f_rmdir(dirname) fm_rmdir(dirname)

#define f_findfirst(filename,find) fm_findfirst(filename,find)
#define f_findnext(find) fm_findnext(find)
#define f_rename(filename,newname) fm_rename(filename,newname)
#define f_filelength(filename) fm_filelength(filename)

#define f_close(filehandle) fm_close(filehandle)
#define f_open(filename,mode) fm_open(filename,mode)

#define f_read(buf,size,size_st,filehandle) fm_read(buf,size,size_st,filehandle)
#define f_write(buf,size,size_st,filehandle) fm_write(buf,size,size_st,filehandle)

#define f_seek(filehandle,offset,whence) fm_seek(filehandle,offset,whence)

#define f_tell(filehandle) fm_tell(filehandle)
#define f_getc(filehandle) fm_getc(filehandle)
#define f_putc(ch,filehandle) fm_putc(ch,filehandle)
#define f_rewind(filehandle) fm_rewind(filehandle)
#define f_eof(filehandle) fm_eof(filehandle)

#define f_gettimedate(filename,pctime,pcdate) fm_gettimedate(filename,pctime,pcdate)
#define f_settimedate(filename,ctime,cdate) fm_settimedate(filename,ctime,cdate)
#define f_delete(filename) fm_delete(filename)

#define f_getattr(filename,attr) fm_getattr(filename,attr)
#define f_setattr(filename,attr) fm_setattr(filename,attr)

#define f_getlabel(drivenum,label,len) fm_getlabel(drivenum,label,len)
#define f_setlabel(drivenum,label) fm_setlabel(drivenum,label)

#endif


#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of fat.h
 *
 ***************************************************************************/

#endif /* _FAT_H_ */


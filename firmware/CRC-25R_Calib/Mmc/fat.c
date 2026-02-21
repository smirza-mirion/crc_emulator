#ifndef _FAT_C_
#define _FAT_C_

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
#include "mmcapi.h"

#if (!F_LONGFILENAME)

/*  THIS MODULE IS FOR FAT WITHOUT LONG FILE NAMES ONLY 8+3 NAMES  */
/*  PLEASE CHECK FAT.H WITH F_LONGFILENAME DECLARATION  */

/****************************************************************************
 *
 * only one variable is used
 *
 ***************************************************************************/

F_FILESYSTEM f_filesystem;

/****************************************************************************
 *
 * fn_getversion
 *
 * returns with the filesystem version string
 *
 * RETURNS
 *
 * string pointer with version number
 *
 ***************************************************************************/

char *fn_getversion(void) {
	return (char*)("HCC_FAT ver:2.53");
}

/****************************************************************************
 *
 * _f_getword
 *
 * get a word 16bit number from a memory (it uses LITTLE ENDIAN mode always)
 *
 * INPUTS
 *
 * ptr - pointer where data is
 *
 * RETURNS
 *
 * word number
 *
 ***************************************************************************/

static unsigned short _f_getword(void *ptr) {
unsigned char *sptr=(unsigned char*)ptr;
unsigned short ret;
	ret=sptr[1];
	ret<<=8;
	ret+=sptr[0];
	return ret;
}

/****************************************************************************
 *
 * _f_setword
 *
 * set a word 16bit number into a memory (it uses LITTLE ENDIAN mode always)
 *
 * INPUTS
 *
 * ptr - where to store data
 * num - 16 bit number to store
 *
 ***************************************************************************/

static void _f_setword(void *ptr,unsigned short num) {
unsigned char *sptr=(unsigned char*)ptr;
	sptr[1]=(unsigned char)((num>>8) & 0xff);
	sptr[0]=(unsigned char)(num & 0xff);
}

/****************************************************************************
 *
 * _f_getlong
 *
 * get a long 32bit number from a memory (it uses LITTLE ENDIAN mode always)
 *
 * INPUTS
 *
 * ptr - pointer where data is
 *
 * RETURNS
 *
 * long number
 *
 ***************************************************************************/

static unsigned long _f_getlong(void *ptr) {
unsigned char *sptr=(unsigned char*)ptr;
unsigned long ret;
	ret=sptr[3];
	ret<<=8;
	ret+=sptr[2];
	ret<<=8;
	ret+=sptr[1];
	ret<<=8;
	ret+=sptr[0];
	return ret;
}

/****************************************************************************
 *
 * _f_setlong
 *
 * set a long 32bit number into a memory (it uses LITTLE ENDIAN mode always)
 *
 * INPUTS
 *
 * ptr - where to store data
 * num - 32 bit number to store
 *
 ***************************************************************************/

static void _f_setlong(void *ptr,unsigned long num) {
unsigned char *sptr=(unsigned char*)ptr;
	sptr[3]=(unsigned char)(num>>24);
	sptr[2]=(unsigned char)(num>>16);
	sptr[1]=(unsigned char)(num>>8);
	sptr[0]=(unsigned char)(num);
}

/****************************************************************************
 *
 * _getchar
 *
 * get characters from memory
 *
 * INPUTS
 *
 * array - where to store read data
 * num - number of characters
 * ptr - where to read from
 *
 * RETURNS
 *
 * read pointer new position
 *
 ***************************************************************************/

static unsigned char *_getchar(unsigned char *array,long num,unsigned char *ptr) {

   if (!array) return ptr+num;

   while (num--) {
      *array++=*ptr++;
   }
   return ptr;
}

/****************************************************************************
 *
 * _getword
 *
 * get a 16bit number from memory
 *
 * INPUTS
 *
 * num - pointer where to store 16 bit read number
 * ptr - where to get the value from
 *
 * RETURNS
 *
 * read pointer new position
 *
 ***************************************************************************/

static unsigned char *_getword(unsigned short *num,unsigned char *ptr) {
	*num=_f_getword(ptr);
   return ptr+2;
}

/****************************************************************************
 *
 * _getlong
 *
 * get a 32bit number from memory
 *
 * INPUTS
 *
 * num - pointer where to store 32 bit read number
 * ptr - where to get the value from
 *
 * RETURNS
 *
 * read pointer new position
 *
 ***************************************************************************/

static unsigned char *_getlong(unsigned long *num,unsigned char *ptr) {
   *num=_f_getlong(ptr);
   return ptr+4;
}

/****************************************************************************
 *
 * _f_checkstatus
 *
 * checking a volume driver status, if media is removed or has been changed
 *
 * INPUTS
 *
 * vi - volume pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_checkstatus(F_VOLUME *vi) {
	if (vi->func.getstatus) {
		long status=vi->func.getstatus();
		if (status & (F_ST_MISSING|F_ST_CHANGED)) {
			vi->state=F_STATE_NEEDMOUNT; /* card has been removed; */
			return F_ERR_CARDREMOVED;
		}
	}
	return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_writesector
 *
 * write sector data on a volume, it calls low level driver function, it
 * writes a complete sector
 *
 * INPUTS
 *
 * vi - volume pointer
 * data - data pointer where the data is
 * sector - which physical sector
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_writesector(F_VOLUME *vi,void *data, unsigned long sector) {
int ret;
int retry;
	if (!vi) return F_ERR_INVALIDDRIVE;

   if (!vi->func.writesector) {
      vi->state=F_STATE_NEEDMOUNT; /* no write function */
      return F_ERR_ACCESSDENIED;
   }


   if (vi->func.getstatus) {
		long status=vi->func.getstatus();
		if (status & (F_ST_MISSING|F_ST_CHANGED)) {
			vi->state=F_STATE_NEEDMOUNT; /* card has been removed; */
			return F_ERR_CARDREMOVED;
		}
      if (status & (F_ST_WRPROTECT)) {
         vi->state=F_STATE_NEEDMOUNT; /* card has been removed; */
         return F_ERR_WRITEPROTECT;
      }
   }

   for (retry=3; retry; retry--) {
   	ret=vi->func.writesector(data,sector);
   	if (!ret) return F_NO_ERROR;

   	if (ret==-1) {
   		vi->state=F_STATE_NEEDMOUNT; /* card has been removed; */
   		return F_ERR_CARDREMOVED;
   	}
   }

   ret=_f_checkstatus(vi);
	if (ret) return ret;

	return F_ERR_ONDRIVE;
}

/****************************************************************************
 *
 * _f_readsector
 *
 * read sector data from a volume, it calls low level driver function, it
 * reads a complete sector
 *
 * INPUTS
 *
 * vi - volume pointer
 * data - data pointer where store data
 * sector - which physical sector is read
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_readsector(F_VOLUME *vi,void *data, unsigned long sector) {
int retry;
int ret;
	if (!vi) return F_ERR_INVALIDDRIVE;

	if (!vi->func.readsector) return F_ERR_ACCESSDENIED;

	ret=_f_checkstatus(vi);
	if (ret) return ret;

   for (retry=3; retry; retry--) {
   	ret=vi->func.readsector(data,sector);
   	if (!ret) return F_NO_ERROR;

   	if (ret==-1) {
   		vi->state=F_STATE_NEEDMOUNT; /* card has been removed; */
   		return F_ERR_CARDREMOVED;
   	}
   }

   ret=_f_checkstatus(vi);
	if (ret) return ret;

	return F_ERR_ONDRIVE;
}

/****************************************************************************
 *
 * _f_memset
 *
 * standard memory set into a value
 *
 * INPUTS
 *
 * d - destination address
 * fill - what char used to fill
 * len - length of copied data
 *
 ***************************************************************************/

static void _f_memset(void *d, char fill, long len) {
char *dc=(char*)d;
    while(len--) {
      *dc++=fill;
    }
}

/****************************************************************************
 *
 * _f_getmaxcluster
 *
 * calculate the maximum cluster number from boot record
 *
 * INPUTS
 *
 * vi - volume info
 *
 * RETURNS
 *
 * maximum number of clusters
 *
 ***************************************************************************/

static unsigned long _f_getmaxcluster(F_VOLUME *vi) {
unsigned long maxcluster=vi->bootrecord.number_of_sectors_less32;
	if (!maxcluster) maxcluster=vi->bootrecord.number_of_sectors;
	maxcluster-=vi->data.sector;
	maxcluster+=vi->bootrecord.number_of_hidden_sectors; /* out of calc! */
	maxcluster/=vi->bootrecord.sector_per_cluster;

	return maxcluster;
}

/****************************************************************************
 *
 * _f_buildsectors
 *
 * calculate relative sector position from boot record
 *
 * INPUTS
 *
 * vi - volume pointer
 *
 ***************************************************************************/

static void _f_buildsectors(F_VOLUME *vi) {
long a;
F_BOOTRECORD *bootrecord=&vi->bootrecord;

   vi->mediatype=F_UNKNOWN_MEDIA;

   vi->firstfat.sector=bootrecord->reserved_sectors;
   vi->firstfat.sector+=bootrecord->number_of_hidden_sectors; /* maybe partioned */

   if (bootrecord->sector_per_FAT) {
	   vi->firstfat.num=bootrecord->sector_per_FAT;

	   vi->root.sector=vi->firstfat.sector;
	   for (a=0; a<bootrecord->number_of_FATs;a++) {
		   	vi->root.sector+=vi->firstfat.num;
	   }

	   vi->root.num=bootrecord->max_root_entry;
	   vi->root.num*=sizeof(F_DIRENTRY);
	   vi->root.num/=F_SECTOR_SIZE;

	   vi->data.sector=vi->root.sector+vi->root.num;
	   vi->data.num=0; /* ?? */
   }
   else {
	   vi->firstfat.num=bootrecord->sector_per_FAT32;

	   vi->data.sector=vi->firstfat.sector;
	   for (a=0; a<bootrecord->number_of_FATs;a++) {
		   	vi->data.sector+=vi->firstfat.num;
	   }
	   vi->data.num=0; /* ?? */

	   {
	 	 unsigned long sectorcou=bootrecord->sector_per_cluster;
	 	 vi->root.sector=bootrecord->rootcluster;
	 	 vi->root.sector-=2;
	 	 vi->root.sector*=sectorcou;
	 	 vi->root.sector+=vi->data.sector;
		 vi->root.num=bootrecord->sector_per_cluster;
	   }
   }

   if (_f_getmaxcluster(vi)<0xff0UL) {
	   vi->mediatype=F_FAT12_MEDIA;
   }
   else if (_f_getmaxcluster(vi)<0xfff0UL) {
	   vi->mediatype=F_FAT16_MEDIA;
   }
   else {
	   vi->mediatype=F_FAT32_MEDIA;
   }
}

/****************************************************************************
 *
 * _f_readbootrecord
 *
 * read boot record from a volume, it detects if there is MBR on the media
 *
 * INPUTS
 *
 * vi - volume pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_readbootrecord(F_VOLUME *vi) {
int ret;
unsigned char *ptr=f_filesystem.sectorbuffer;
F_BOOTRECORD *f_bootrecord=&vi->bootrecord;

   _f_memset(f_bootrecord,0,sizeof(F_BOOTRECORD));

   ret=_f_readsector(vi,ptr,0);
   if (ret) return ret;

   if ((ptr[0x1fe]!=0x55) && (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; /* ?? */

   if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) {
	   unsigned long sector;

/* 	   if (ptr[0x1be]!=0x80) return F_ERR_NOTFORMATTED; //check 1st partion active?	    */
	   sector=_f_getlong(&ptr [0x08+0x1be] ); /* start sector for 1st partioon */

       ret=_f_readsector(vi,ptr,sector);
	   if (ret) return ret;

	   if ((ptr[0x1fe]!=0x55) || (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; /* ?? */

	   if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) return F_ERR_NOTFORMATTED; /* ?? */
   }

   ptr=_getchar(f_bootrecord->jump_code,sizeof(f_bootrecord->jump_code),ptr);
   ptr=_getchar(f_bootrecord->OEM_name,sizeof(f_bootrecord->OEM_name),ptr);
   ptr=_getword(&f_bootrecord->bytes_per_sector,ptr);
   if (f_bootrecord->bytes_per_sector!=F_SECTOR_SIZE) {
      vi->mediatype=F_UNKNOWN_MEDIA;
      return F_ERR_NOTSUPPSECTORSIZE;
   }

   f_bootrecord->sector_per_cluster=*ptr++;
   ptr=_getword(&f_bootrecord->reserved_sectors,ptr);
   f_bootrecord->number_of_FATs=*ptr++;
   ptr=_getword(&f_bootrecord->max_root_entry,ptr);
   ptr=_getword(&f_bootrecord->number_of_sectors_less32,ptr);
   f_bootrecord->media_descriptor=*ptr++;
   ptr=_getword(&f_bootrecord->sector_per_FAT,ptr);
   ptr=_getword(&f_bootrecord->sector_per_Track,ptr);
   ptr=_getword(&f_bootrecord->number_of_heads,ptr);
   ptr=_getlong(&f_bootrecord->number_of_hidden_sectors,ptr);
   ptr=_getlong(&f_bootrecord->number_of_sectors,ptr);
/*  */
   if (f_bootrecord->sector_per_FAT) { /* this is 0 on FAT32, but better to check */
		_f_buildsectors(vi);
   }
   else vi->mediatype=F_FAT32_MEDIA;

   if (vi->mediatype==F_FAT32_MEDIA) {
		ptr=_getlong(&f_bootrecord->sector_per_FAT32,ptr);
		ptr=_getword(&f_bootrecord->extflags,ptr);
		ptr=_getword(&f_bootrecord->fsversion,ptr);
		ptr=_getlong(&f_bootrecord->rootcluster,ptr);
		ptr=_getword(&f_bootrecord->fsinfo,ptr);
		ptr=_getword(&f_bootrecord->bkbootsec,ptr);
		ptr=_getchar(f_bootrecord->reserved,sizeof(f_bootrecord->reserved),ptr);

	   ptr=_getword(&f_bootrecord->logical_drive_num,ptr);
	   f_bootrecord->extended_signature=*ptr++;
	   ptr=_getlong(&f_bootrecord->serial_number,ptr);
	   ptr=_getchar(f_bootrecord->volume_name,sizeof(f_bootrecord->volume_name),ptr);
	   ptr=_getchar(f_bootrecord->FAT_name,sizeof(f_bootrecord->FAT_name),ptr);
	   ptr=_getchar(0,448-28,ptr); /* smaller!! */
	   ptr=_getchar(f_bootrecord->executable_marker,sizeof(f_bootrecord->executable_marker),ptr);
   }
   else {
	   ptr=_getword(&f_bootrecord->logical_drive_num,ptr);
	   f_bootrecord->extended_signature=*ptr++;
	   ptr=_getlong(&f_bootrecord->serial_number,ptr);
	   ptr=_getchar(f_bootrecord->volume_name,sizeof(f_bootrecord->volume_name),ptr);
	   ptr=_getchar(f_bootrecord->FAT_name,sizeof(f_bootrecord->FAT_name),ptr);
	   ptr=_getchar(0,448,ptr);
	   ptr=_getchar(f_bootrecord->executable_marker,sizeof(f_bootrecord->executable_marker),ptr);
   }

   if (f_bootrecord->media_descriptor!=0xf8) {     /* fixdrive */
	   if (f_bootrecord->media_descriptor!=0xf0) { /* removable */
		   return F_ERR_NOTFORMATTED; /* ?? */
	   }
   }

   return F_NO_ERROR;
}

/****************************************************************************
 *
 * _setcharzero
 *
 * fills with zero charater to memory
 *
 * INPUTS
 *
 * num - number of characters
 * ptr - where to store data
 *
 * RETURNS
 *
 * last write position
 *
 ***************************************************************************/

static unsigned char *_setcharzero(long num,unsigned char *ptr) {
   while (num--) {
      *ptr++=0;
   }
   return ptr;
}

/****************************************************************************
 *
 * _setchar
 *
 * copy a charater string to memory
 *
 * INPUTS
 *
 * array - original code what to copy
 * num - number of characters
 * ptr - where to store data
 *
 * RETURNS
 *
 * last write position
 *
 ***************************************************************************/

static unsigned char *_setchar(unsigned char *array,long num,unsigned char *ptr) {

   if (!array) return _setcharzero(num,ptr);

   while (num--) {
      *ptr++=*array++;
   }
   return ptr;
}


/****************************************************************************
 *
 * _setword
 *
 * store a 16bit word into memory
 *
 * INPUTS
 *
 * num - 16bit number to store
 * ptr - where to store data
 *
 * RETURNS
 *
 * last write position
 *
 ***************************************************************************/

static unsigned char *_setword(unsigned short num,unsigned char *ptr) {
   _f_setword(ptr,num);
   return ptr+2;
}

/****************************************************************************
 *
 * _setlong
 *
 * store a 32bit long number into memory
 *
 * INPUTS
 *
 * num - 32bit number to store
 * ptr - where to store data
 *
 * RETURNS
 *
 * last write position
 *
 ***************************************************************************/

static unsigned char *_setlong(unsigned long num,unsigned char *ptr) {
   _f_setlong(ptr,num);
   return ptr+4;
}


/****************************************************************************
 *
 * _f_writebootrecord
 *
 * writing boot record onto a volume, it uses number of hidden sector variable
 *
 * INPUTS
 *
 * vi - volume pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_writebootrecord(F_VOLUME *vi) {
unsigned char *ptr=f_filesystem.sectorbuffer;
F_BOOTRECORD *f_bootrecord=&vi->bootrecord;
int ret;

   if (vi->mediatype==F_FAT32_MEDIA) { /* write FS_INFO */
      long a;

      _f_memset(f_filesystem.sectorbuffer,0,sizeof(f_filesystem.sectorbuffer));

      for (a=0; a<f_bootrecord->reserved_sectors; a++) {
         ret=_f_writesector(vi,f_filesystem.sectorbuffer,f_bootrecord->number_of_hidden_sectors+a); /* erase reserved area */
         if (ret) return ret;
      }

      ptr=_setlong(0x41615252UL,ptr); /* signature */
      ptr=_setcharzero(480,ptr);        /* reserved */
      ptr=_setlong(0x61417272UL,ptr); /* signature   */
      ptr=_setlong(0xffffffffUL,ptr); /* no last */
      ptr=_setlong(0xffffffffUL,ptr); /* no hint */
      ptr=_setcharzero(12,ptr);         /* reserved */
      ptr=_setlong(0xaa550000UL,ptr); /* trail */

      ret=_f_writesector(vi,f_filesystem.sectorbuffer,f_bootrecord->number_of_hidden_sectors+f_bootrecord->fsinfo); /* write FSINFO */
      if (ret) return ret;

      ret=_f_writesector(vi,f_filesystem.sectorbuffer,f_bootrecord->number_of_hidden_sectors+f_bootrecord->fsinfo+f_bootrecord->bkbootsec); /* write FSINFO */
      if (ret) return ret;
   }


   ptr=f_filesystem.sectorbuffer;

   ptr=_setchar(f_bootrecord->jump_code,sizeof(f_bootrecord->jump_code),ptr);
   ptr=_setchar(f_bootrecord->OEM_name,sizeof(f_bootrecord->OEM_name),ptr);
   ptr=_setword(f_bootrecord->bytes_per_sector,ptr);
   *ptr++=f_bootrecord->sector_per_cluster;
   ptr=_setword(f_bootrecord->reserved_sectors,ptr);
   *ptr++=f_bootrecord->number_of_FATs;
   ptr=_setword(f_bootrecord->max_root_entry,ptr);
   ptr=_setword(f_bootrecord->number_of_sectors_less32,ptr);
   *ptr++=f_bootrecord->media_descriptor;
   ptr=_setword(f_bootrecord->sector_per_FAT,ptr);
   ptr=_setword(f_bootrecord->sector_per_Track,ptr);
   ptr=_setword(f_bootrecord->number_of_heads,ptr);
   ptr=_setlong(f_bootrecord->number_of_hidden_sectors,ptr);
   ptr=_setlong(f_bootrecord->number_of_sectors,ptr);

   if (vi->mediatype==F_FAT32_MEDIA) {

		ptr=_setlong(f_bootrecord->sector_per_FAT32,ptr);
		ptr=_setword(f_bootrecord->extflags,ptr);   /* ? */
		ptr=_setword(f_bootrecord->fsversion,ptr);  /* ? */
		ptr=_setlong(f_bootrecord->rootcluster,ptr); /* ? */
		ptr=_setword(f_bootrecord->fsinfo,ptr);      /* ? */
		ptr=_setword(f_bootrecord->bkbootsec,ptr);   /* ? */
		ptr=_setchar(f_bootrecord->reserved,sizeof(f_bootrecord->reserved),ptr); /* ? */

      ptr=_setword(f_bootrecord->logical_drive_num,ptr);
      *ptr++=f_bootrecord->extended_signature;
      ptr=_setlong(f_bootrecord->serial_number,ptr);
      ptr=_setchar(f_bootrecord->volume_name,sizeof(f_bootrecord->volume_name),ptr);
      ptr=_setchar(f_bootrecord->FAT_name,sizeof(f_bootrecord->FAT_name),ptr);
      ptr=_setchar(0,448-28,ptr); /* smaller!! */
      ptr=_setchar(f_bootrecord->executable_marker,sizeof(f_bootrecord->executable_marker),ptr);

      ret=_f_writesector(vi,f_filesystem.sectorbuffer,f_bootrecord->number_of_hidden_sectors+f_bootrecord->bkbootsec); /* write backup */
      if (ret) return ret;
   }
   else {
      ptr=_setword(f_bootrecord->logical_drive_num,ptr);
      *ptr++=f_bootrecord->extended_signature;
      ptr=_setlong(f_bootrecord->serial_number,ptr);
      ptr=_setchar(f_bootrecord->volume_name,sizeof(f_bootrecord->volume_name),ptr);
      ptr=_setchar(f_bootrecord->FAT_name,sizeof(f_bootrecord->FAT_name),ptr);
      ptr=_setchar(0,448,ptr);
      ptr=_setchar(f_bootrecord->executable_marker,sizeof(f_bootrecord->executable_marker),ptr);
   }

   return _f_writesector(vi,f_filesystem.sectorbuffer,f_bootrecord->number_of_hidden_sectors); /* write bootrecord */
}

/****************************************************************************
 *
 * _f_memcpy
 *
 * standard memory copy
 *
 * INPUTS
 *
 * d - destination address
 * s - source address
 * len - length of copied data
 *
 ***************************************************************************/

static void _f_memcpy(void *d, void *s, long len) {
char *dc=(char*)d;
char *sc=(char*)s;
    while(len--) {
      *dc++=*sc++;
    }
}

/****************************************************************************
 *
 * _f_writefatsector
 *
 * writing fat sector into volume, this function check if fat was modified
 * and writes data
 *
 * INPUTS
 *
 * vi - volume pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_writefatsector(F_VOLUME *vi) {
long a;
   if (vi->fatmodified) {
      unsigned long fatsector=vi->firstfat.sector+vi->fatsector;

      if (vi->fatsector>=vi->firstfat.num) {
         return F_ERR_INVALIDSECTOR;
      }

      for (a=0; a<vi->bootrecord.number_of_FATs; a++) {
			int ret=_f_writesector(vi,vi->fat,fatsector);
			if (ret) return ret;

			fatsector+=vi->firstfat.num;
      }

      vi->fatmodified=0;

   }

   return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_getfatsector
 *
 * read a fat sector from media
 *
 * INPUTS
 *
 * vi - volume pointer
 * sector - which fat sector is needed, this sector number is zero based
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_getfatsector(F_VOLUME *vi,unsigned long sector) {
long a;
   if (vi->fatsector!=sector) {
      unsigned long fatsector;

      if (vi->fatmodified) {
      	int ret=_f_writefatsector(vi);
         if (ret) return ret;
      }

      vi->fatsector=sector;

      if (vi->fatsector>=vi->firstfat.num) {
         return F_ERR_INVALIDSECTOR;
      }

      fatsector=vi->firstfat.sector+vi->fatsector;

		for (a=0; a<vi->bootrecord.number_of_FATs; a++) {
			if (!_f_readsector(vi,vi->fat,fatsector)) {
				return F_NO_ERROR;
			}
			fatsector+=vi->firstfat.num;
		}
		return F_ERR_READ;
   }

   return F_NO_ERROR;
}


/****************************************************************************
 *
 * _f_setclustervalue
 *
 * set a cluster value in the FAT
 *
 * INPUTS
 *
 * vi - volume pointer
 * cluster - which cluster's value need to be modified
 * data - new value of the cluster
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_setclustervalue(F_VOLUME *vi,unsigned long cluster, unsigned long data) {
int ret;
	if (vi->mediatype==F_FAT16_MEDIA) {
		unsigned long sector=cluster;
		unsigned short sdata=(unsigned short)(data & 0xffff); /* keep 16 bit only */

		sector/=(F_SECTOR_SIZE/2);
		cluster-=sector*(F_SECTOR_SIZE/2);

		ret=_f_getfatsector(vi,sector);
		if (ret) return ret;

		if (_f_getword(&vi->fat[cluster<<1])!=sdata) {
			_f_setword(&vi->fat[cluster<<1],sdata);
			vi->fatmodified=1;
		}
	}
	else if (vi->mediatype==F_FAT12_MEDIA) {
		unsigned char f12new[2];
		unsigned long sector=cluster;
		unsigned long pos;
		unsigned short sdata=(unsigned short)(data & 0x0fff); /* keep 12 bit only */

		if (cluster&1) sdata <<= 4;

		_f_setword(f12new,sdata); /* create new data */

		sector+=sector/2;		  /* 1.5 bytes */
		pos=sector % F_SECTOR_SIZE;
	    sector/=F_SECTOR_SIZE;

		ret=_f_getfatsector(vi,sector);
		if (ret) return ret;

		if (cluster&1) {
			f12new[0] |= vi->fat[pos] & 0x0f;
		}

		if (vi->fat[pos]!=f12new[0]) {
			vi->fat[pos]=f12new[0];
			vi->fatmodified=1;
		}

		pos++;
		if (pos>=512) {
			ret=_f_getfatsector(vi,sector+1);
			if (ret) return ret;
			pos=0;
		}

		if (!(cluster&1)) {
			f12new[1] |= vi->fat[pos] & 0xf0;
		}

		if (vi->fat[pos]!=f12new[1]) {
			vi->fat[pos]=f12new[1];
			vi->fatmodified=1;
		}

		return F_NO_ERROR;
	}
	else if (vi->mediatype==F_FAT32_MEDIA) {
		unsigned long sector=cluster;
		unsigned long oldv;

	    sector/=(F_SECTOR_SIZE/4);
		cluster-=sector*(F_SECTOR_SIZE/4);

		ret=_f_getfatsector(vi,sector);
		if (ret) return ret;

		oldv=_f_getlong(&vi->fat[cluster<<2]);

		data&=0x0fffffff;
		data|=oldv & 0xf0000000UL; /* keep 4 top bits */

		if (data!=oldv) {
			_f_setlong(&vi->fat[cluster<<2],data);
			vi->fatmodified=1;
		}

		return F_NO_ERROR;
	}
	else {
		return F_ERR_INVALIDMEDIA;
	}

	return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_getclustervalue
 *
 * get a cluster value from FAT
 *
 * INPUTS
 *
 * vi - volume pointer
 * cluster - which cluster value is requested
 * pvalue - where to store data
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_getclustervalue(F_VOLUME *vi,unsigned long cluster,unsigned long *pvalue) {
unsigned long val;
int ret;
	if (vi->mediatype==F_FAT16_MEDIA) {
		unsigned long sector=cluster;
	    sector/=(F_SECTOR_SIZE/2);
		cluster-=sector*(F_SECTOR_SIZE/2);

		ret=_f_getfatsector(vi,sector);
		if (ret) return ret;

		val=_f_getword(&vi->fat[cluster<<1]);
		if (val>=0xfff0) val|=0x0ffff000;   /* extends it */

		if (pvalue) *pvalue=val;

		return F_NO_ERROR;
	}
	else if (vi->mediatype==F_FAT12_MEDIA) {
		unsigned char dataf12[2];
		unsigned long sector=cluster;
		unsigned long pos;

		sector+=sector/2;		  /* 1.5 bytes */
		pos=sector % F_SECTOR_SIZE;
	    sector/=F_SECTOR_SIZE;

		ret=_f_getfatsector(vi,sector);
		if (ret) return ret;

		dataf12[0]=vi->fat[pos++];

		if (pos>=512) {
			ret=_f_getfatsector(vi,sector+1);
			if (ret) return ret;
			pos=0;
		}

		dataf12[1]=vi->fat[pos];

		val=_f_getword(dataf12);

		if (cluster&1) {
			val = val >> 4;
		}
		else {
			val = val & 0xfff;
		}

		if (val>=0x0ff0) val|=0x0ffff000;   /* extends it */

		if (pvalue) *pvalue=val;

		return F_NO_ERROR;
	}
	else if (vi->mediatype==F_FAT32_MEDIA) {
		unsigned long sector=cluster;
	    sector/=(F_SECTOR_SIZE/4);
		cluster-=sector*(F_SECTOR_SIZE/4);

		ret=_f_getfatsector(vi,sector);
		if (ret) return ret;

		if (pvalue) *pvalue=_f_getlong(&vi->fat[cluster<<2]) & 0x0fffffff; /* 28bit */

		return F_NO_ERROR;
	}
	else {
		return F_ERR_INVALIDMEDIA;
	}
}

/****************************************************************************
 *
 * _f_getvolume
 *
 * getting back a volume info structure of a given drive, it try to mounts
 * drive if it was not mounted before
 *
 * INPUTS
 *
 * drivenum - which drive volumeinfo needs to be retreive
 * pvi - pointer of volumeinfo structure where to put the pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int _f_getvolume(int drivenum,F_VOLUME **pvi) {
F_VOLUME *vi;

	if (drivenum < 0 || drivenum>=F_MAXVOLUME) return F_ERR_INVALIDDRIVE;

	vi=&f_filesystem.volumes[drivenum];

	if (pvi) *pvi=vi;	/* set volumeinfo */

	switch (vi->state) {
	case F_STATE_NONE: return F_ERR_INVALIDDRIVE;
	case F_STATE_WORKING:

		if (!_f_checkstatus(vi)) return F_NO_ERROR;

      /*  here we don't stop case flow,   */
      /*  because we have to clean up this volume!  */

	case F_STATE_NEEDMOUNT:
		{
			long a;

			vi->fatmodified=0;
			vi->fatsector=(unsigned long)(-1);
			vi->direntrysector=(unsigned long)(-1);
			vi->lastalloccluster=0;

			for (a=0; a<F_MAXFILES; a++) { /* close all files on this volume */
				F_FILEINT *f=&f_filesystem.files[a];
				if (f->drivenum==drivenum) {
					if (f->mode != F_FILE_CLOSE) {
						f->mode=F_FILE_CLOSE;
					}
				}
			}

			vi->cwd[0]=0; /* reset cwd */
			vi->mediatype=F_UNKNOWN_MEDIA;

			if (vi->func.getstatus) {
				long status=vi->func.getstatus();
				if (status & F_ST_MISSING) {
					vi->state=F_STATE_NEEDMOUNT; /* card missing */
					return F_ERR_CARDREMOVED;
				}
			}

			if (!_f_readbootrecord(vi)) {
				_f_buildsectors(vi);

				vi->state=F_STATE_WORKING;
				return F_NO_ERROR;
			}

			vi->mediatype=F_UNKNOWN_MEDIA;
			return F_ERR_NOTFORMATTED;
		}

	}

	return F_ERR_INVALIDDRIVE;
}

/****************************************************************************
 *
 * fn_initvolume
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

int fn_initvolume(int drvnumber,F_INITFUNC pfunc) {
F_VOLUME *vi;
int ret;
	if (drvnumber<0 || drvnumber>=F_MAXVOLUME) return F_ERR_INVALIDDRIVE;

	vi=&f_filesystem.volumes[drvnumber];

	vi->state=F_STATE_NONE;

	if (!pfunc) {
		return F_ERR_INITFUNC;
	}

	_f_memset(&vi->func,0,sizeof(vi->func));

	ret=pfunc(&vi->func);
	if (ret) return F_ERR_INITFUNC;

	vi->state=F_STATE_NEEDMOUNT;

	return _f_getvolume(drvnumber,0);
}



/****************************************************************************
 *
 * _f_getdirsector
 *
 * read a directory sector
 *
 * INPUTS
 *
 * vi - volume pointer
 * sector - which sector is needed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_getdirsector(F_VOLUME *vi,unsigned long sector) {
int ret=F_NO_ERROR;
	if (vi->direntrysector!=sector) {
		ret=_f_readsector(vi,vi->direntry,sector);

		if (!ret) vi->direntrysector=sector;
		else vi->direntrysector=(unsigned long)(-1);
	}
	return ret;
}

/****************************************************************************
 *
 * _f_writedirsector
 *
 * Write directory sector, which was last read
 *
 * INPUTS
 *
 * vi - volume pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_writedirsector(F_VOLUME *vi) {
   if (vi->direntrysector!=(unsigned long)(-1)) {
   	return _f_writesector(vi,vi->direntry,vi->direntrysector);
   }
   return F_ERR_INVALIDSECTOR;
}


/****************************************************************************
 *
 * _f_checkname
 *
 * checking filename and extension for special characters
 *
 * INPUTS
 *
 * name - filename (e.g.: filename)
 * ext - extension of file (e.g.: txt)
 *
 * RETURNS
 *
 * 0 - if no contains invalid character
 * other - if contains any invalid character
 *
 ***************************************************************************/

static long _f_checkname(char *name,char *ext) {
long inspace=0;
long a=0;
	for (a=0; a<F_MAXNAME; a++) {
		char ch=name[a];
		if (!inspace) {
			if (ch==' ') inspace=1;
			if (ch=='|') return 1;
			if (ch=='[') return 1;
			if (ch==']') return 1;
			if (ch=='<') return 1;
			if (ch=='>') return 1;
			if (ch=='/') return 1;
			if (ch=='\\') return 1;
			if (ch==':') return 1;
		}
		else if (ch!=' ') return 1; /* no inspace allowed */
	}

	inspace=0;
	for (a=0; a<F_MAXEXT; a++) {
		char ch=ext[a];
		if (!inspace) {
   		if (ch==' ') inspace=1;
   		if (ch=='|') return 1;
   		if (ch=='[') return 1;
   		if (ch==']') return 1;
   		if (ch=='<') return 1;
   		if (ch=='>') return 1;
   		if (ch=='/') return 1;
   		if (ch=='\\') return 1;
   		if (ch==':') return 1;
		}
		else if (ch!=' ') return 1; /* no inspace allowed */
	}
	return 0;
}

/****************************************************************************
 *
 * _f_checknamewc
 *
 * checking filename and extension for wildcard character
 *
 * INPUTS
 *
 * name - filename (e.g.: filename)
 * ext - extension of file (e.g.: txt)
 *
 * RETURNS
 *
 * 0 - if no contains wildcard character (? or *)
 * other - if contains any wildcard character
 *
 ***************************************************************************/

static long _f_checknamewc(char *name,char *ext) {
long a=0;
	for (a=0; a<F_MAXNAME; a++) {
		char ch=name[a];
		if (ch=='?') return 1;
		if (ch=='*') return 1;
	}
	for (a=0; a<F_MAXEXT; a++) {
		char ch=ext[a];
		if (ch=='?') return 1;
		if (ch=='*') return 1;
	}
	return _f_checkname(name,ext);
}


/****************************************************************************
 *
 * _f_setnameext
 *
 * convert a string into filename and extension separatelly, the terminator
 * character could be zero char, '/' or '\'
 *
 * INPUTS
 *
 * s - source string (e.g.: hello.txt)
 * name - where to store name (this array size has to be F_MAXNAME (8))
 * ext - where to store extension (this array size has to be F_MAXEXT (3))
 *
 * RETURNS
 *
 * length of the used bytes from source string array
 *
 ***************************************************************************/

static long _f_setnameext(const char *s, char *name, char *ext) {
long len,extlen=0;
long a;
long setext=1;

	for (len=0;;) {
		char ch=s[len];
		if (ch==0) break;
		if (ch=='\\' || ch=='/') break;
		len++;					/* calculate len */
	}

	if (len && s[0]=='.') {
		if (len==1) goto dots;
		if (s[1]=='.') {
			if (len==2) goto dots;
			goto dots; /* maybe more than 2 dots */
		}
	}

	for (a=len; a>0; a--) {
		if (s[a-1]=='.') {
			long b;
			extlen=len-a+1;
			len=a-1;

			for (b=0; b<F_MAXEXT; b++) {
				if (b<extlen-1) {
					char ch=s[a++];
					if (ch>='a' && ch<='z') ch+='A'-'a';

					ext[b]=ch;
				}
				else ext[b]=32;
			}
			setext=0;
			break;
		}
	}

dots:
	if (setext) {
		for (a=0; a<F_MAXEXT; a++) {
			ext[a]=32;     /* fills with extension and zeroes */
		}
	}

	for (a=0; a<F_MAXNAME; a++) {
		if (a<len) {
			char ch=s[a];
			if (ch>='a' && ch<='z') ch+='A'-'a';
			name[a]=ch;
		}
		else name[a]=32;
	}

	return len+extlen;
}

/****************************************************************************
 *
 * _f_toupper
 *
 * convert a string into lower case
 *
 * INPUTS
 *
 * s - input string to convert
 *
 ***************************************************************************/

static char _f_toupper(char ch) {
   if (ch>='a' && ch<='z') return ch-'a'+'A';
   return ch;
}

/****************************************************************************
 *
 * _f_setfsname
 *
 * convert a single string into F_NAME structure
 *
 * INPUTS
 *
 * name - combined name with drive,path,filename,extension used for source
 * fsname - where to fill this structure with separated drive,path,name,ext
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if name contains invalid path or name
 *
 ***************************************************************************/

static int _f_setfsname(const char *name,F_NAME *fsname) {
char s[F_MAXPATH];
unsigned long pathpos=0;
unsigned long namepos=0;
unsigned long a;

   s[0]=0;

	if (!name[0]) return 1; /* no name */

	if (name[1]==':') {
		int drv=_f_toupper(name[0]);
		if (drv>='A' && drv<='Z') {
			fsname->drivenum=drv-'A';
			name+=2; /* skip drive number */
		}
		else return 1; /* invalid drive */
	}
	else {
   		fsname->drivenum=fn_getdrive();
	}

	if (name[0]!='/' && name[0]!='\\') {

   		if (fn_getdcwd(fsname->drivenum,fsname->path,F_MAXPATH)) return 1; /* error */
   		for (pathpos=0; fsname->path[pathpos];) pathpos++;
	}

	for (;;) {
		char ch=_f_toupper(*name++);

		if (!ch) break;

		if (ch==':') return 1; /* not allowed */

		if (ch=='/' || ch=='\\') {
			if (pathpos) {
				if (fsname->path[pathpos-1]=='/') return 1; /* not allowed double // */
				if (pathpos>=F_MAXPATH-2) return 1; /* path too long */
				fsname->path[pathpos++]='/';
			}

			for (;namepos;) {
				if (s[namepos-1]!=' ') break;
				namepos--;		  /* remove end spaces */
			}

			for (a=0; a<namepos; a++) {
				if (pathpos>=F_MAXPATH-2) return 1; /* path too long */
				fsname->path[pathpos++]=s[a];
			}
			namepos=0;
			continue;
		}

  		if (ch==' ' && (!namepos)) continue; /* remove start spaces */

		if (namepos>=(sizeof(s)-2)) return 1; /* name too long */
		s[namepos++]=ch;
	}

	s[namepos]=0; /* terminates it */
	fsname->path[pathpos]=0;  /* terminates it */

	for (;namepos;) {
   		if (s[namepos-1]!=' ') break;
   		s[namepos-1]=0; /* remove end spaces */
   		namepos--;
	}

	if (!_f_setnameext(s,fsname->filename,fsname->fileext)) return 2; /* no name */

	if (fsname->filename[0]==' ') return 1; /* cannot be */

	return 0;
}


/****************************************************************************
 *
 * _f_clustertopos
 *
 * convert a cluster position into physical sector position
 *
 * INPUTS
 *
 * vi - volume pointer
 * cluster - original cluster position
 * pos - position structure to fills the position
 *
 ***************************************************************************/

static void _f_clustertopos(F_VOLUME *vi,unsigned long cluster, F_POS *pos) {
 	pos->cluster=cluster;

	if (!cluster) {
		unsigned long sectorcou=vi->root.num;
		pos->sector=vi->root.sector;
		pos->sectorend=pos->sector+sectorcou;
	}
	else {
	 	unsigned long sectorcou=vi->bootrecord.sector_per_cluster;
	 	pos->sector=pos->cluster;
	 	pos->sector-=2;
	 	pos->sector*=sectorcou;
	 	pos->sector+=vi->data.sector;
		pos->sectorend=pos->sector+sectorcou;
	}

	if (cluster>=F_CLUSTER_RESERVED) {
		pos->sectorend=0;
	}

	pos->pos=0;
}


/****************************************************************************
 *
 * _f_prepareformat
 *
 * preparing boot record for formatting, it sets and calculates values
 *
 * INPUTS
 *
 * phy - media physical descriptor
 * f_bootrecord - which bootrecord need to be prepare
 * number_of_hidden_sectors - where boot record starts
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_prepareformat(F_PHY *phy,F_BOOTRECORD *f_bootrecord,unsigned long number_of_hidden_sectors,long fattype) {
	if (!phy->number_of_sectors) return F_ERR_INVALIDSECTOR;

	f_bootrecord->jump_code[0]=0xeb;
	f_bootrecord->jump_code[1]=0x3c;
	f_bootrecord->jump_code[2]=0x90;

	_f_memcpy(f_bootrecord->OEM_name,"MSDOS5.0",8);
	f_bootrecord->bytes_per_sector=F_SECTOR_SIZE;

	f_bootrecord->number_of_FATs=2;

	if (fattype==F_FAT32_MEDIA) {
		f_bootrecord->reserved_sectors=32;
   		f_bootrecord->max_root_entry=0;
	}
	else {
		f_bootrecord->reserved_sectors=1;
   		f_bootrecord->max_root_entry=512;
	}

	f_bootrecord->media_descriptor=0xf0;  /* 0xf0 for removable. 0xf8 for fix */

	f_bootrecord->sector_per_Track=phy->sector_per_track;
	f_bootrecord->number_of_heads=phy->number_of_heads;

	f_bootrecord->number_of_hidden_sectors=number_of_hidden_sectors; /* if this is not partioned */
	f_bootrecord->logical_drive_num=0;

	f_bootrecord->extended_signature=0x29;

 	_f_memcpy(f_bootrecord->volume_name,"NO NAME    ",11);

	f_bootrecord->executable_marker[0]=0x55;
	f_bootrecord->executable_marker[1]=0xaa;

	f_bootrecord->serial_number=f_getrand(f_bootrecord->serial_number^phy->number_of_sectors);

	if ((phy->number_of_sectors < 0x10000UL) && (fattype!=F_FAT32_MEDIA)) {
		f_bootrecord->number_of_sectors_less32=(unsigned short)(phy->number_of_sectors);
		f_bootrecord->number_of_sectors=0;
	}
	else {
		f_bootrecord->number_of_sectors_less32=0;
		f_bootrecord->number_of_sectors=phy->number_of_sectors;
	}

	if (fattype==F_FAT12_MEDIA) {
   		f_bootrecord->sector_per_cluster=1;
	   	while (f_bootrecord->sector_per_cluster) { /* try FAT12 */
   			if (phy->number_of_sectors / f_bootrecord->sector_per_cluster < (unsigned long)(0x00ff0UL)) break;
	   		f_bootrecord->sector_per_cluster<<=1;
	   	}
	   	if (!f_bootrecord->sector_per_cluster) return F_ERR_MEDIATOOLARGE; /* fat12 cannot be there */
	}
	else if (fattype==F_FAT16_MEDIA) {
   		f_bootrecord->sector_per_cluster=1;
	   	while (f_bootrecord->sector_per_cluster) { /* try FAT16 */
	   		if (phy->number_of_sectors / f_bootrecord->sector_per_cluster < (unsigned long)(0x0fff0UL)) break;
	   		f_bootrecord->sector_per_cluster<<=1;
	   	}
	   	if (!f_bootrecord->sector_per_cluster) return F_ERR_MEDIATOOLARGE; /* fat16 cannot be there */
	}
	else if (fattype==F_FAT32_MEDIA) {
   		f_bootrecord->sector_per_cluster=1;
	   	while (f_bootrecord->sector_per_cluster) { /* try FAT32 */
   			if (phy->number_of_sectors / f_bootrecord->sector_per_cluster < (unsigned long)(0x0ffffff0UL)) break;
	   		f_bootrecord->sector_per_cluster<<=1;
	   	}
   		if (!f_bootrecord->sector_per_cluster) return F_ERR_MEDIATOOLARGE; /* fat32 cannot be there */
	}
	else return F_ERR_INVFATTYPE;

	if (!f_bootrecord->sector_per_cluster) return F_ERR_INVALIDMEDIA; /* fat16 cannot be there */

	{
		long secpercl=f_bootrecord->sector_per_cluster;
		long nfat=f_bootrecord->number_of_FATs;
		unsigned long roots;
		long fatsec;

   		roots=f_bootrecord->max_root_entry;
	   	roots*=sizeof(F_DIRENTRY);
   		roots/=F_SECTOR_SIZE;

		if (fattype==F_FAT32_MEDIA) {
   			fatsec=(phy->number_of_sectors-f_bootrecord->reserved_sectors+2*secpercl);
			fatsec+=(128*secpercl+nfat-1);
			fatsec/=(128*secpercl+nfat);

			f_bootrecord->sector_per_FAT32=fatsec;
			f_bootrecord->sector_per_FAT=0;

			f_bootrecord->extflags=0;
   			f_bootrecord->fsversion=0;
	   		f_bootrecord->rootcluster=2;
   			f_bootrecord->fsinfo=1;
   			f_bootrecord->bkbootsec=6;

   			_f_memset(f_bootrecord->reserved,0,sizeof(f_bootrecord->reserved));
		}
		else if (fattype==F_FAT16_MEDIA) {
   			fatsec=(phy->number_of_sectors-f_bootrecord->reserved_sectors-roots+2*secpercl);
			fatsec+=(256*secpercl+nfat-1);
			fatsec/=(256*secpercl+nfat);

   			f_bootrecord->sector_per_FAT=(unsigned short)(fatsec);
		}
		else if (fattype==F_FAT12_MEDIA) {
   			fatsec=3*(phy->number_of_sectors-f_bootrecord->reserved_sectors-roots+2*secpercl);
			fatsec+=(1024*secpercl+3*nfat-1);
			fatsec/=(1024*secpercl+3*nfat);

   			f_bootrecord->sector_per_FAT=(unsigned short)(fatsec);
		}
	}

	return F_NO_ERROR;

}

/****************************************************************************
 *
 * _f_postformat
 *
 * erase fats, erase root directory, reset variables after formatting
 *
 * INPUTS
 *
 * vi - volume pointer
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_postformat(F_VOLUME *vi,long fattype) {
unsigned long maxcluster;
unsigned long a;
int ret;

	_f_buildsectors(vi); /* get positions */
	if (vi->mediatype!=fattype) return F_ERR_MEDIATOOSMALL;

	if (vi->mediatype==F_FAT12_MEDIA) {
		_f_memcpy(vi->bootrecord.FAT_name,"FAT12   ",8);
	}
	else if (vi->mediatype==F_FAT16_MEDIA) {
		_f_memcpy(vi->bootrecord.FAT_name,"FAT16   ",8);
	}
	else if (vi->mediatype==F_FAT32_MEDIA) {
		_f_memcpy(vi->bootrecord.FAT_name,"FAT32   ",8);
	}
	else return F_ERR_INVALIDMEDIA;

	vi->fatmodified=0;
	vi->fatsector=(unsigned long)(-1);
	vi->direntrysector=(unsigned long)(-1);

	maxcluster=_f_getmaxcluster(vi);

	ret=_f_setclustervalue(vi,0,0xfffffff8); /* reset fat */
	if (ret) return ret;

	if (vi->mediatype==F_FAT32_MEDIA) {
   		_f_setlong(vi->fat,0xfffffff8); /* this is because of high 4 bits */
		vi->fatmodified=1;
	}

	ret=_f_setclustervalue(vi,1,0x0fffffff);
	if (ret) return ret;

	for (a=2; a<maxcluster; a++) {
		ret=_f_setclustervalue(vi,a,0);
		if (ret) return ret;
	}

	if (vi->mediatype==F_FAT32_MEDIA) {
		ret=_f_setclustervalue(vi,vi->bootrecord.rootcluster,F_CLUSTER_LAST);
		if (ret) return ret;
	}

	ret=_f_writefatsector(vi);
	if (ret) return ret;

	_f_memset(f_filesystem.sectorbuffer,0,F_SECTOR_SIZE);

 	for (a=0; a<vi->root.num;a++) { /* reset root direntries */
  		ret = _f_writesector(vi,f_filesystem.sectorbuffer,vi->root.sector+a);
  		if (ret) return ret;
	}

	return _f_writebootrecord(vi);
}

/****************************************************************************
 *
 * fn_hardformat
 *
 * Making a complete format on media, independently from master boot record,
 * according to media physical
 *
 * INPUTS
 *
 * drivenum - which drive low format is needed
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fn_hardformat(int drivenum,long fattype) {
F_VOLUME *vi;
int ret;
F_PHY phy;

	ret=_f_getvolume(drivenum,&vi);
	if (ret && ret!=F_ERR_NOTFORMATTED) return ret;

	vi->state=F_STATE_NEEDMOUNT;

	_f_memset(&phy,0,sizeof(F_PHY));
	if (vi->func.getphy) {
		ret=vi->func.getphy(&phy);
		if (ret) return F_ERR_INVALIDDRIVE;
	}
	else return F_ERR_INITFUNC;

	ret=_f_prepareformat(&phy,&vi->bootrecord,0,fattype); /* no partition */
	if (ret) return ret;

	return _f_postformat(vi,fattype);
}

/****************************************************************************
 *
 * fn_format
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

int fn_format(int drivenum,long fattype) {
F_BOOTRECORD *f_bootrecord;
F_VOLUME *vi;
int ret;
F_PHY phy;

	ret=_f_getvolume(drivenum,&vi);
	if (ret && ret!=F_ERR_NOTFORMATTED) return ret;

	vi->state=F_STATE_NEEDMOUNT;

	f_bootrecord=&vi->bootrecord;

	if (!ret) { /* we have existing formating */

		phy.number_of_heads=f_bootrecord->number_of_heads;
		phy.sector_per_track=f_bootrecord->sector_per_Track;

		phy.number_of_sectors=vi->bootrecord.number_of_sectors_less32;
		if (!phy.number_of_sectors) phy.number_of_sectors=vi->bootrecord.number_of_sectors;


		ret=_f_prepareformat(&phy,f_bootrecord,f_bootrecord->number_of_hidden_sectors,fattype);

		if (ret) return ret;
	}
	else if (ret==F_ERR_NOTFORMATTED) { /* we dont have existing formatting */
		unsigned char *ptr=f_filesystem.sectorbuffer;
		unsigned long startsec=0;

		_f_memset(f_bootrecord,0,sizeof(F_BOOTRECORD));
		_f_memset(&phy,0,sizeof(F_PHY));

		if (vi->func.getphy) {
			ret=vi->func.getphy(&phy);
			if (ret) return F_ERR_INVALIDDRIVE;
		}
		else return F_ERR_INITFUNC;

	    ret=_f_readsector(vi,ptr,0); /* check MBR */
		if (ret) return ret;

		if ((ptr[0x1fe]==0x55) && (ptr[0x1ff]==0xaa)) { /* check valid */
			if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) {   /* check MBR */
				if (ptr[0x1be]==0x80) {				  /* check active */
					startsec=_f_getlong(&ptr[0x08+0x1be]); /* start sector for 1st partioon */
					phy.number_of_sectors=_f_getlong(&ptr[0x0c+0x1be]); /* number of sectors */
				}
			}
		}


		ret=_f_prepareformat(&phy,f_bootrecord,startsec,fattype); /* no partition */
		if (ret) return ret;
	}

	return _f_postformat(vi,fattype);
}


/****************************************************************************
 *
 * _f_getcurrsector
 *
 * read current sector according in file structure
 *
 * INPUTS
 *
 * vi - volume pointer
 * f - internal file pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_getcurrsector(F_VOLUME *vi,F_FILEINT *f) {
int ret;
unsigned long cluster;
	while (f->pos.cluster>=2 && f->pos.cluster<F_CLUSTER_RESERVED) {

		if (f->pos.sector<f->pos.sectorend) {

			ret=_f_readsector(vi,f->data,f->pos.sector);
			if (ret) {
				return ret;
			}

			return F_NO_ERROR;
		}

		ret=_f_getclustervalue(vi,f->pos.cluster,&cluster);
		if (ret) return ret;

		if (cluster>=F_CLUSTER_RESERVED) return F_ERR_EOF;

		f->prevcluster=f->pos.cluster;
		_f_clustertopos(vi,cluster,&f->pos);
	}

	return F_ERR_EOF;
}

/****************************************************************************
 *
 * _f_findfile
 *
 * internal function to finding file in directory entry
 *
 * INPUTS
 *
 * vi - volumeinfo structure, where to find file
 * name - filename
 * ext - fileextension
 * pos - where to start searching, and contains current position
 * pde - store back the directory entry pointer
 *
 * RETURNS
 *
 * 0 - if file was not found
 * 1 - if file was found
 *
 ***************************************************************************/

static int _f_findfile(F_VOLUME *vi,char *name,char *ext,F_POS *pos, F_DIRENTRY **pde) {

	while (pos->cluster<F_CLUSTER_RESERVED) {

		for (;pos->sector<pos->sectorend; pos->sector++) {
			F_DIRENTRY *de=(F_DIRENTRY*)(vi->direntry+sizeof(F_DIRENTRY)*pos->pos);

			if (_f_getdirsector(vi,pos->sector)) return 0; /* notfound */

			for (; pos->pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos->pos++) {
				unsigned long b,ok;

				if (!de->name[0]) continue;						 /* empty */
				if (de->name[0]==(unsigned char)0xe5) continue; /* deleted */
				if (de->attr & F_ATTR_VOLUME) continue;

				for (b=0,ok=1; b<sizeof(de->name); b++) {
					if (de->name[b]!=name[b]) {
						ok=0;
						break;
					}
				}

				if (!ok) continue;

				for (b=0,ok=1; b<sizeof(de->ext); b++) {
					if (de->ext[b]!=ext[b]) {
						ok=0;
						break;
					}
				}

				if (ok) {
					if (pde) *pde=de;
					return 1;
				}
			}

			pos->pos=0;
		}

 		if (!pos->cluster) {
			if (vi->mediatype==F_FAT32_MEDIA) {
				pos->cluster=vi->bootrecord.rootcluster;
			}
			else return 0; /* root directory */
		}

		{
			unsigned long nextcluster;
			if (_f_getclustervalue(vi,pos->cluster,&nextcluster)) return 0; /* not found */

	   		if (nextcluster>=F_CLUSTER_RESERVED) return 0; /* eof */

			_f_clustertopos(vi,nextcluster,pos);
		}

	}

	return 0;
}


/****************************************************************************
 *
 * _fs_findfilewc
 *
 * internal function to finding file in directory entry with wild card
 *
 * INPUTS
 *
 * vi - volumeinfo structure, where to find file
 * name - filename
 * ext - fileextension
 * pos - where to start searching, and contains current position
 * pde - store back the directory entry pointer
 *
 * RETURNS
 *
 * 0 - if file was not found
 * 1 - if file was found
 *
 ***************************************************************************/

static int _f_findfilewc(F_VOLUME *vi,char *name,char *ext,F_POS *pos, F_DIRENTRY **pde) {

	while (pos->cluster<F_CLUSTER_RESERVED) {

		for (;pos->sector<pos->sectorend; pos->sector++) {
			F_DIRENTRY *de=(F_DIRENTRY*)(vi->direntry+sizeof(F_DIRENTRY)*pos->pos);

			if (_f_getdirsector(vi,pos->sector)) return 0; /* not found */

			for (; pos->pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos->pos++) {
				unsigned long b,ok;

				if (!de->name[0]) continue;						 /* empty */
				if (de->name[0]==(unsigned char)0xe5) continue; /* deleted */
				if (de->attr & F_ATTR_VOLUME) continue;


				for (b=0,ok=1; b<sizeof(de->name); b++) {
					if (name[b]=='*') break;

					if (name[b]!='?') {
						if (de->name[b]!=name[b]) {
							ok=0;
							break;
						}
					}
				}

				if (!ok) continue;

				for (b=0,ok=1; b<sizeof(de->ext); b++) {
					if (ext[b]=='*') {
						if (pde) *pde=de;
						return 1;
					}

					if (ext[b]!='?') {
						if (de->ext[b]!=ext[b]) {
							ok=0;
							break;
						}
					}

				}

				if (ok) {
					if (pde) *pde=de;
					return 1;
				}
			}

			pos->pos=0;
		}

 		if (!pos->cluster) {
   			if (vi->mediatype==F_FAT32_MEDIA) {
				pos->cluster=vi->bootrecord.rootcluster;
			}
			else return 0; /* root directory */
		}

		{
			unsigned long nextcluster;
			if (_f_getclustervalue(vi,pos->cluster,&nextcluster)) return 0; /* not found */

	   		if (nextcluster>=F_CLUSTER_RESERVED) return 0; /* eof */

			_f_clustertopos(vi,nextcluster,pos);
		}
	}

	return 0;
}

/****************************************************************************
 *
 * _f_getfilename
 *
 * create a complete filename from name and extension
 *
 * INPUTS
 *
 * dest - where to store filename
 * name - name of the file
 * ext - extension of the file
 *
 ***************************************************************************/

static void _f_getfilename(char *dest, char *name,char *ext) {
long a,len;

	for (len=a=F_MAXNAME; a; a--,len--) {
		if (name[a-1]!=' ') break;
	}
	for (a=0; a<len; a++) *dest++=*name++;


	for (len=a=F_MAXEXT; a; a--,len--) {
		if (ext[a-1]!=' ') break;
	}
	if (len) *dest++='.';

	for (a=0; a<len; a++) *dest++=*ext++;

	*dest=0; /* terminateit */
}

/****************************************************************************
 *
 * _f_getdecluster
 *
 * get a directory entry structure start cluster value
 *
 * INPUTS
 *
 * vi - volume pointer
 * de - directory entry
 *
 * RETURNS
 *
 * directory entry cluster value
 *
 ***************************************************************************/

static unsigned long _f_getdecluster(F_VOLUME *vi,F_DIRENTRY *de) {
unsigned long cluster;
	if (vi->mediatype==F_FAT32_MEDIA) {
		cluster=_f_getword(&de->clusterhi);
		cluster<<=16;
		cluster|=_f_getword(&de->clusterlo);
		return cluster;
	}

	return _f_getword(&de->clusterlo);
}

/****************************************************************************
 *
 * _f_setdecluster
 *
 * set a directory entry structure start cluster value
 *
 * INPUTS
 *
 * vi - volume pointer
 * de - directory entry
 * cluster - value of the start cluster
 *
 ***************************************************************************/

static void _f_setdecluster(F_VOLUME *vi,F_DIRENTRY *de,unsigned long cluster) {
	_f_setword(&de->clusterlo,(unsigned short)(cluster&0xffff));

	if (vi->mediatype==F_FAT32_MEDIA) {
		_f_setword(&de->clusterhi,(unsigned short)((cluster>>16)&0xffff));
	}
	else {
		_f_setword(&de->clusterhi,(unsigned short)0);
	}
}


/****************************************************************************
 *
 * _f_findpath
 *
 * finding out if path is valid in F_NAME and
 * correct path info with absolute path (removes relatives)
 *
 * INPUTS
 *
 * vi - volumeinfo
 * fsname - filled structure with path,drive
 * pos - where to start searching, and contains current position
 *
 * RETURNS
 *
 * 0 - if path was not found or invalid
 * 1 - if path was found
 *
 ***************************************************************************/

static int _f_findpath(F_VOLUME *vi,F_NAME *fsname,F_POS *pos) {
char *path=fsname->path;
char *mpath=path;
F_DIRENTRY *de;

	_f_clustertopos(vi,0,pos);

	for (;*path;) {
		char name[F_MAXNAME];
		char ext[F_MAXEXT];

		long len=_f_setnameext(path,name,ext);

		if (pos->cluster==0 && len==1 && name[0]=='.') {
			_f_clustertopos(vi,0,pos);
		}
		else {
			if (!_f_findfile(vi,name,ext,pos,&de)) return 0;
			if (!(de->attr & F_ATTR_DIR ) ) return 0;

			_f_clustertopos(vi,_f_getdecluster(vi,de),pos);
		}


		if (name[0]=='.') {
			if (len==1) {
				path+=len;

				if (!(*path)) {
					if (mpath!=fsname->path) mpath--; /* if we are now at the top */
					break;
				}
				path++;
				continue;
			}

			if (name[1]!='.') return 0; /* invalid name */
			if (len!=2) return 0; /* invalid name ! */

			path+=len;

			if (mpath==fsname->path) return 0; /* we are in the top */

			mpath--; /* no on separator */
			for (;;) {
				if (mpath==fsname->path) break; /* we are now at the top */
				mpath--;
				if (*mpath=='/') {
					mpath++;
					break;
				}
			}

			if (!(*path)) {
				if (mpath!=fsname->path) mpath--; /* if we are now at the top */
				break;
			}
			path++;
			continue;

		}
		else {
			if (path==mpath) {							/* if no was dots just step */
				path+=len;
				mpath+=len;
			}
			else {
				long a;
				for (a=0; a<len;a++) *mpath++=*path++;	 /* copy if in different pos */
			}
		}

		if (!(*path)) break;
		path++;
		*mpath++='/'; /* add separator */
	}

	*mpath=0; /* terminate it */
	return 1;
}


/****************************************************************************
 *
 * fn_getdrive
 *
 * Get current drive number
 *
 * RETURNS
 *
 * with the current drive number (0-A, 1-B,...)
 *
 ***************************************************************************/

int fn_getdrive(void) {
	return f_filesystem.curdrive;
}


/****************************************************************************
 *
 * fn_chdrive
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

int fn_chdrive(int drivenum) {
int ret=_f_getvolume(drivenum,0);
	if (ret && ret!=F_ERR_NOTFORMATTED) return ret;

	f_filesystem.curdrive=drivenum;

	return F_NO_ERROR;
}


/****************************************************************************
 *
 * fn_getdcwd
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

int fn_getdcwd(int drivenum, char *buffer, int maxlen ) {
F_VOLUME *vi;
long a;

	if (drivenum < 0 || drivenum>=F_MAXVOLUME) return F_ERR_INVALIDDRIVE;
	vi=&f_filesystem.volumes[drivenum];

	if (_f_checkstatus(vi)) {
		vi->cwd[0]=0; /* remove cwd */
		vi->state=F_STATE_NEEDMOUNT; /* mount is needed */
	}

	if (!maxlen) return F_NO_ERROR;

	maxlen--;	/* need for termination */

	for (a=0; a<maxlen; a++) {
		char ch=vi->cwd[a];
		buffer[a]=ch;
		if (!ch) break;
	}

	buffer[a]=0;	/* add terminator at the end */

	return F_NO_ERROR;
}

/****************************************************************************
 *
 * fn_getcwd
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

int fn_getcwd(char *buffer, int maxlen ) {
	return fn_getdcwd(fn_getdrive(),buffer,maxlen);
}


/****************************************************************************
 *
 * fn_findfirst
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

int fn_findfirst(const char *filename,F_FIND *find) {
F_VOLUME *vi;
int ret;
	if (_f_setfsname(filename,&find->findfsname)) return F_ERR_INVALIDNAME; /* invalid name */
	if (_f_checkname(find->findfsname.filename,find->findfsname.fileext)) return F_ERR_INVALIDNAME;/* invalid name, wildcard is ok */

	ret=_f_getvolume(find->findfsname.drivenum,&vi);
	if (ret) return ret;

	if (!_f_findpath(vi,&find->findfsname,&find->pos)) return F_ERR_INVALIDDIR; /* search for path */

	return fn_findnext(find);
}


/****************************************************************************
 *
 * fn_findnext
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

int fn_findnext(F_FIND *find) {
F_VOLUME *vi;
F_DIRENTRY *de;
long a;
int ret;

	ret=_f_getvolume(find->findfsname.drivenum,&vi);
	if (ret) return ret;

	if (!_f_findfilewc(vi,find->findfsname.filename,find->findfsname.fileext,&find->pos,&de)) return F_ERR_NOTFOUND;

	for (a=0;a<F_MAXNAME; a++) find->name[a]=de->name[a];
	for (a=0;a<F_MAXEXT; a++) find->ext[a]=de->ext[a];

	_f_getfilename(find->filename,(char*)de->name,(char*)de->ext);

	find->attr=de->attr;
	find->cdate=_f_getword(&de->cdate);
	find->ctime=_f_getword(&de->ctime);
	find->filesize=_f_getlong(&de->filesize);
	find->cluster=_f_getdecluster(vi,de);
	find->pos.pos++; /* goto next position */

	return 0;
}

/****************************************************************************
 *
 * fn_chdir
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

int fn_chdir(const char *dirname) {
F_POS pos;
F_VOLUME *vi;
F_NAME fsname;
long len;
long a;
int ret;

	ret=_f_setfsname(dirname,&fsname);

	if (ret==1) return F_ERR_INVALIDNAME; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/* invalid name */

	ret=_f_getvolume(fsname.drivenum,&vi);
	if (ret) return ret;

	for (len=0;fsname.path[len];) len++;
	if (len && (fsname.filename[0]!=32 || fsname.fileext[0]!=32)) fsname.path[len++]='/';

	_f_getfilename(fsname.path+len,fsname.filename,fsname.fileext);

	if (!(_f_findpath(vi,&fsname,&pos))) return F_ERR_NOTFOUND;

	for (a=0; a<F_MAXPATH;a++) vi->cwd[a]=fsname.path[a];

	return F_NO_ERROR;
}


/****************************************************************************
 *
 * _f_checklocked
 *
 * check if a given file is locked or not
 *
 * INPUTS
 *
 * drvnum - drive number
 * pos - pos structure contains directory entry pos
 *
 * RETURNS
 *
 * zero - if not locked
 * F_ERR_LOCKED - if locked
 *
 ***************************************************************************/

static int _f_checklocked(long drvnum,F_POS *pos) {
long a;
	for (a=0; a<F_MAXFILES; a++) {
		F_FILEINT *f=&f_filesystem.files[a];
		if (f->mode!=F_FILE_CLOSE) {
			if (drvnum==f->drivenum && f->dirpos.pos==pos->pos && f->dirpos.sector==pos->sector){
				return F_ERR_LOCKED;
			}
		}
	}
	return F_NO_ERROR;
}

/****************************************************************************
 *
 * fn_rename
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

int fn_rename(const char *filename, const char *newname) {
F_POS posdir;
F_POS pos;
F_VOLUME *vi;
F_DIRENTRY *de;
F_NAME fsname;
char name[F_MAXNAME];
char ext[F_MAXEXT];
long a;
int ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/* invalid name */

	ret=_f_getvolume(fsname.drivenum,&vi);
	if (ret) return ret;

	if (!(_f_findpath(vi,&fsname,&posdir))) return F_ERR_INVALIDDIR;

	pos.cluster = posdir.cluster;
	pos.sector	= posdir.sector;
	pos.sectorend=posdir.sectorend;
	pos.pos=posdir.pos;

	if (fsname.filename[0]=='.') return F_ERR_NOTFOUND;

	if (!_f_findfile(vi,fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;

	if (de->attr & F_ATTR_READONLY) return F_ERR_ACCESSDENIED;      /* readonly */

	if (_f_checklocked(fsname.drivenum,&pos)) return F_ERR_LOCKED;

	if (!_f_setnameext(newname,name,ext)) return 0; /* no length invalid name */
	if (_f_checknamewc(name,ext)) return 0;/* invalid name */

	if (name[0]=='.') return F_ERR_INVALIDNAME;
	if (name[0]==' ') return F_ERR_INVALIDNAME;

	if (_f_findfile(vi,name,ext,&posdir,0)) return F_ERR_DUPLICATED;

	ret=_f_getdirsector(vi,pos.sector);
	if (ret) return ret;

	for (a=0;a<F_MAXNAME; a++) de->name[a]=name[a];
	for (a=0;a<F_MAXEXT; a++) de->ext[a]=ext[a];

	return _f_writedirsector(vi);
}

/****************************************************************************
 *
 * _f_initentry
 *
 * init directory entry, this function is called if a new entry is coming
 *
 * INPUTS
 *
 * de - directory entry which needs to be initialized
 * name - fil ename  (8)
 * ext - file extension (3)
 *
 ***************************************************************************/

static void _f_initentry(F_DIRENTRY *de,char *name, char *ext) {

	_f_memset (de,0,sizeof(F_DIRENTRY)); /* reset all entries */

	_f_memcpy (de->name,name,sizeof(de->name));
	_f_memcpy (de->ext,ext,sizeof(de->ext));

	_f_setword(&de->cdate,f_getdate());  /* if there is realtime clock then creation date could be set from */
	_f_setword(&de->ctime,f_gettime());  /* if there is realtime clock then creation time could be set from */

}

/****************************************************************************
 *
 * _f_alloccluster
 *
 * allocate cluster from FAT
 *
 * INPUTS
 *
 * vi - volume pointer
 * pcluster - where to store the allocated cluster number
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_alloccluster(F_VOLUME *vi,unsigned long *pcluster) {
unsigned long maxcluster=_f_getmaxcluster(vi);
unsigned long cou;
unsigned long cluster=vi->lastalloccluster;
unsigned long value;
int ret;
	for (cou=0; cou<maxcluster; cou++) {

		if (cluster>=maxcluster) cluster=0;

		ret=_f_getclustervalue(vi,cluster,&value);
		if (ret) return ret;

		if (!value) {
			vi->lastalloccluster=cluster+1; /* set next one */
			if (pcluster) *pcluster=cluster;

			return F_NO_ERROR;
		}

		cluster++;
	}

	return F_ERR_NOMOREENTRY;
}

/****************************************************************************
 *
 * _f_getintfile
 *
 * allocate an internal file handler
 *
 * RETURNS
 *
 * internal file pointer if successful
 * zero if there was no free file pointer (see F_MAXFILES)
 *
 ***************************************************************************/

static F_FILEINT *_f_getintfile(void) {
	long a;
	for (a=0; a<F_MAXFILES; a++) {
		if (f_filesystem.files[a].mode==F_FILE_CLOSE) {
			f_filesystem.files[a].file.reference=a;
			return &f_filesystem.files[a];
		}
	}
	return 0;		/* no more file could be opened  */
}


/****************************************************************************
 *
 * _f_dobadblock
 *
 * Bad block handler, if a given block is bad, this routine mark the cluster
 * as bad, and change the file pointer position values
 *
 * INPUTS
 *
 * vi - volume info
 * f - internal file pointer which contains bad block
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_dobadblock(F_VOLUME *vi,F_FILEINT *f) {
F_POS posold;
F_POS pos;
unsigned long cluster;
unsigned long oldclustervalue;
unsigned long currdif=f->pos.sectorend-f->pos.sector;
int ret;

	ret=_f_getclustervalue(vi,f->pos.cluster,&oldclustervalue); /* get old value */
	if (ret) return ret;

	ret=_f_setclustervalue(vi,f->pos.cluster,F_CLUSTER_BAD); /* set signal as bad */
	if (ret) return ret;

write_again:

	ret=_f_alloccluster(vi,&cluster);						  /* get a new one */
	if (ret) return ret;

	ret=_f_setclustervalue(vi,cluster,oldclustervalue);	/* set old value back */
	if (ret) return ret;

	_f_clustertopos(vi,cluster,&pos);			   /* set new data position */
	_f_clustertopos(vi,f->pos.cluster,&posold); /* set old data position */


	for (;pos.sector<pos.sectorend; pos.sector++,posold.sector++) {

		ret=_f_readsector(vi,f_filesystem.sectorbuffer,posold.sector);
		if (ret) return ret; /* cannot read!!! */

		ret=_f_writesector(vi,f_filesystem.sectorbuffer,pos.sector);
		if (ret) {
			if (vi->state!=F_STATE_WORKING) return ret; /* not working */

			ret=_f_setclustervalue(vi,cluster,F_CLUSTER_BAD); /* set signal as bad also */
			if (ret) return ret;

			goto write_again; /* this will be bad sector as well */
		}
	}

 	_f_clustertopos(vi,cluster,&f->pos);
 	f->pos.sector=f->pos.sectorend-currdif; /* lets go back to current sector */

	return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_addentry
 *
 * Add a new directory entry into driectory list
 *
 * INPUTS
 *
 * vi - volumeinfo
 * fs_name - filled structure what to add into directory list
 * pos - where directory cluster chains starts
 * pde - F_DIRENTRY pointer where to store the entry where it was added
 *
 * RETURNS
 *
 * 0 - if successfully added
 * other - if any error (see FS_xxx errorcodes)
 *
 ***************************************************************************/

static int _f_addentry(F_VOLUME *vi,F_NAME *fsname,F_POS *pos,F_DIRENTRY **pde) {
int ret;

	if (!fsname->filename[0]) return F_ERR_INVALIDNAME;
	if (fsname->filename[0]=='.') return F_ERR_INVALIDNAME;

	while (pos->cluster<F_CLUSTER_RESERVED) {

		for (;pos->sector<pos->sectorend; pos->sector++) {
			F_DIRENTRY *de=(F_DIRENTRY*)(vi->direntry+sizeof(F_DIRENTRY)*pos->pos);

			ret=_f_getdirsector(vi,pos->sector);
			if (ret) return ret;

			for (; pos->pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos->pos++) {
				if ((!de->name[0]) || (de->name[0]==(unsigned char)0xe5)) {
					_f_initentry(de,fsname->filename,fsname->fileext);

					if (vi->mediatype==F_FAT32_MEDIA) {
						_f_setword(&de->crtdate,f_getdate());  /* if there is realtime clock then creation date could be set from */
						_f_setword(&de->crttime,f_gettime());  /* if there is realtime clock then creation time could be set from */
						_f_setword(&de->lastaccessdate,f_getdate());  /* if there is realtime clock then creation date could be set from */
					}

					if (pde) *pde=de;
					return F_NO_ERROR;
				}
			}

			pos->pos=0;
		}

		if (!pos->cluster) {
			if (vi->mediatype==F_FAT32_MEDIA) {
				pos->cluster=vi->bootrecord.rootcluster;
			}
			else return F_ERR_NOMOREENTRY; /* root directory, cannot continue */
		}

		{
			unsigned long cluster;

			ret=_f_getclustervalue(vi,pos->cluster,&cluster); /* try to get next cluster */
			if (ret) return ret;

			if (cluster<F_CLUSTER_RESERVED) {
				_f_clustertopos(vi,cluster,pos);
			}
			else {
				ret=_f_alloccluster(vi,&cluster);	/* get a new one */
				if (ret) return ret;

				if (cluster<F_CLUSTER_RESERVED) {
					F_FILEINT *f=_f_getintfile();
					if (!f) return F_ERR_NOMOREENTRY;

					_f_clustertopos(vi,cluster,&f->pos);


					_f_memset(f->data,0,F_SECTOR_SIZE);
					while(f->pos.sector<f->pos.sectorend) {
write_again:
						ret=_f_writesector(vi,f->data,f->pos.sector);
						if (ret) {
							if (vi->state!=F_STATE_WORKING) return ret; /* check if still working volume */

							ret=_f_dobadblock(vi,f);
							if (ret) return ret;

							goto write_again;
						}
						f->pos.sector++;
					}

					ret=_f_setclustervalue(vi,f->pos.cluster,F_CLUSTER_LAST);
					if (ret) return ret;

					ret=_f_setclustervalue(vi,pos->cluster,f->pos.cluster);
					if (ret) return ret;

					ret=_f_writefatsector(vi);
					if (ret) return ret;

					_f_clustertopos(vi,f->pos.cluster,pos);
				}
				else return F_ERR_NOMOREENTRY;

			}
		}
	}

	return F_ERR_NOMOREENTRY;
}

/****************************************************************************
 *
 * fn_getfreespace
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

int fn_getfreespace(int drivenum, F_SPACE *pspace) {
int ret;
F_VOLUME *vi;
//unsigned long sector;
unsigned long clustersize;
//long sectorcou;
unsigned long maxcluster;
unsigned long a;


	ret=_f_getvolume(drivenum,&vi);
	if (ret) return ret;

	//sector=0;
	//sectorcou=vi->firstfat.num;

	clustersize=vi->bootrecord.sector_per_cluster;
	clustersize*=F_SECTOR_SIZE;

	maxcluster=_f_getmaxcluster(vi);

	pspace->total=maxcluster*clustersize;
	pspace->free=0;
	pspace->used=0;
	pspace->bad=0;


	for (a=0; a<maxcluster; a++) {
		unsigned long value;
		ret=_f_getclustervalue(vi,a+2,&value);
		if (ret) return ret;

		if (!value) pspace->free+=clustersize;
		else if (value==F_CLUSTER_BAD) pspace->bad+=clustersize;
		else pspace->used+=clustersize;
	}

	return F_NO_ERROR;
}

/****************************************************************************
 *
 * fn_mkdir
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

int fn_mkdir(const char *dirname) {
F_FILEINT *f;
F_POS posdir;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
F_VOLUME *vi;
unsigned long cluster;
unsigned long a;
int ret;

	if (_f_setfsname(dirname,&fsname)) return F_ERR_INVALIDNAME; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/* invalid name */

	ret=_f_getvolume(fsname.drivenum,&vi);
	if (ret) return ret;

	if (!_f_findpath(vi,&fsname,&posdir)) return F_ERR_INVALIDDIR;

	pos.cluster = posdir.cluster;
	pos.sector	= posdir.sector;
	pos.sectorend=posdir.sectorend;
	pos.pos=posdir.pos;

	if (fsname.filename[0]=='.') return F_ERR_NOTFOUND;
	if (_f_findfile(vi,fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_DUPLICATED;

	pos.cluster = posdir.cluster;
	pos.sector	= posdir.sector;
	pos.sectorend=posdir.sectorend;
	pos.pos=posdir.pos;

	if (_f_addentry(vi,&fsname,&pos,&de)) return F_ERR_NOMOREENTRY; /* couldnt be added */

	de->attr |= F_ATTR_DIR;		/* set as directory */

	ret=_f_alloccluster(vi,&cluster);
	if (ret) return ret;


/* /// */

	f=_f_getintfile();

	if (!f) return F_ERR_NOMOREENTRY;

	_f_clustertopos(vi,cluster,&f->pos);

	_f_setdecluster(vi,de,cluster); /* new dir */

/* // */

	de=(F_DIRENTRY *)(f->data);

	_f_initentry(de,".       ","   ");
	de->attr = F_ATTR_DIR;		/* set as directory */
	_f_setdecluster(vi,de,cluster);	 /* current */
	de++;

	_f_initentry(de,"..      ","   ");
	de->attr = F_ATTR_DIR;		/* set as directory */
	_f_setdecluster(vi,de,posdir.cluster); /* parent */
	de++;

	for (a=2; a<F_SECTOR_SIZE/sizeof(F_DIRENTRY); a++,de++) {
		_f_memset(de,0,sizeof(F_DIRENTRY));
	}

	ret=_f_writesector(vi,f->data,f->pos.sector);
	if (ret) return ret;

	f->pos.sector++;

/* // */
	de=(F_DIRENTRY *)(f->data);

	for (a=0; a<2; a++,de++) {
		_f_memset(de,0,sizeof(F_DIRENTRY));
	}

	while(f->pos.sector<f->pos.sectorend) {
		de=(F_DIRENTRY *)(f->data);

write_again:
		ret=_f_writesector(vi,f->data,f->pos.sector);
		if (ret) {
		 	if (vi->state!=F_STATE_WORKING) return ret; /* check if still working volume */

		 	ret=_f_dobadblock(vi,f);
		 	if (ret) return ret;

		 	goto write_again;
		}

		f->pos.sector++;
	}

	ret=_f_setclustervalue(vi,f->pos.cluster,F_CLUSTER_LAST);
	if (ret) return ret;

	ret=_f_writefatsector(vi);
	if (ret) return ret;


	return _f_writedirsector(vi); /* writing  direntry */
}


/****************************************************************************
 *
 * fn_filelength
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

long fn_filelength(const char *filename) {
F_VOLUME *vi;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;

	if (_f_setfsname(filename,&fsname)) return 0; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return 0;/* invalid name */

	if (_f_getvolume(fsname.drivenum,&vi)) return 0; /* can't get the size */

	if (!_f_findpath(vi,&fsname,&pos)) return 0;
	if (!_f_findfile(vi,fsname.filename,fsname.fileext,&pos,&de)) return 0;

	if (de->attr & F_ATTR_DIR) return 0;				/* directory */

	return _f_getlong(&de->filesize);
}


/****************************************************************************
 *
 * _f_removechain
 *
 * remove cluster chain from fat
 *
 * INPUTS
 *
 * vi - volume pointer
 * cluster - first cluster in the cluster chain
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_removechain(F_VOLUME *vi,unsigned long cluster) {

	if (cluster<vi->lastalloccluster) { /* this could be the begining of alloc */
		vi->lastalloccluster=cluster;
	}

	while (cluster<F_CLUSTER_RESERVED && cluster>=2) {
		unsigned long nextcluster;

		int ret=_f_getclustervalue(vi,cluster,&nextcluster);
		if (ret) return ret;

		ret=_f_setclustervalue(vi,cluster,F_CLUSTER_FREE);
		if (ret) return ret;

		cluster=nextcluster;
	}

	return _f_writefatsector(vi);
}


/****************************************************************************
 *
 * _f_check_handle
 *
 * internal function it checks if a file handler is valid and converts it
 * into internal file handler
 *
 * INPUTS
 *
 * filehandle - which filehandle needs to be checked
 *
 * RETURNS
 *
 * 0 - if filehandle is not correct
 * F_FILEINT structure pointer if successfully
 *
 ***************************************************************************/

static F_FILEINT *_f_check_handle(F_FILE *filehandle) {
F_FILEINT *f;
	if (!filehandle) return 0;						  /* invalid handle */

	f=(F_FILEINT *)filehandle;

	if (filehandle->reference<0 || filehandle->reference>=F_MAXFILES) return 0; /* out of range */
	if (f!=(&f_filesystem.files[filehandle->reference])) return 0; /* invalid pointer */

	return f;
}

/****************************************************************************
 *
 * _f_emptywritebuffer
 *
 * empty write buffer if it contains unwritten data
 *
 * INPUTS
 *
 * vi - volume pointer
 * f - internal file pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_emptywritebuffer(F_VOLUME *vi,F_FILEINT *f) {
int ret;
	if (!f->modified) return F_NO_ERROR; /* nothing to write */

	if (f->pos.sector>=f->pos.sectorend) {
		if (!f->startcluster) {
			ret=_f_alloccluster(vi,&f->startcluster);
			if (ret) return ret;

			_f_clustertopos(vi,f->startcluster,&f->pos);

			ret=_f_setclustervalue(vi,f->startcluster,F_CLUSTER_LAST);
			if (ret) return ret;
		}
		else {
			unsigned long value;
			ret=_f_getclustervalue(vi,f->pos.cluster,&value);
			if (ret) return ret;

			if (value<F_CLUSTER_RESERVED) {  /* we are in chain */
				f->prevcluster=f->pos.cluster;
				_f_clustertopos(vi,value,&f->pos); /* go to next cluster */
			}
			else {
				unsigned long nextcluster;

				ret=_f_alloccluster(vi,&nextcluster);
				if (ret) return ret;

				ret=_f_setclustervalue(vi,nextcluster,F_CLUSTER_LAST);
				if (ret) return ret;

				ret=_f_setclustervalue(vi,f->pos.cluster,nextcluster);
				if (ret) return ret;

				f->prevcluster=f->pos.cluster;

				_f_clustertopos(vi,nextcluster,&f->pos);
			}
		}
	}

write_again:
	ret=_f_writesector(vi,f->data,f->pos.sector);
	if (ret) {
	 	if (vi->state!=F_STATE_WORKING) return ret; /* check if still working volume */

	 	ret=_f_dobadblock(vi,f);
	 	if (ret) return ret;

		if (!f->prevcluster) {
			f->startcluster=f->pos.cluster;
		}
		else {
			ret=_f_setclustervalue(vi,f->prevcluster,f->pos.cluster);
			if (ret) return ret;
		}

	 	goto write_again;
	}

	f->modified=0;
	return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_fseek
 *
 * subfunction for f_seek it moves position into given offset and read
 * the current sector
 *
 * INPUTS
 *
 * vi - volume pointer
 * f - F_FILEINT structure which file position needed to be modified
 * offset - position from start
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static int _f_fseek(F_VOLUME *vi,F_FILEINT *f,long offset) {
unsigned long cluster;
long clustersize;
int ret;
	if (offset<0) offset=0;
	if ((unsigned long)offset>=f->filesize) offset=f->filesize;

	if (offset>=(long)f->abspos && offset<(long)f->abspos+F_SECTOR_SIZE) {
		f->relpos=offset-f->abspos;
		return 0;
	}

	if (f->modified) {
		ret=_f_emptywritebuffer(vi,f);
		if (ret) {
		 	f->mode=F_FILE_CLOSE; /* cant accessed any more */
			return ret;
		}

		ret=_f_writefatsector(vi);
		if (ret) {
			f->mode=F_FILE_CLOSE;
			return ret;
		}

	}

	f->abspos=0;
	f->relpos=0;
	f->prevcluster=0;
	f->pos.cluster=f->startcluster;

	clustersize=vi->bootrecord.sector_per_cluster;
	clustersize*=F_SECTOR_SIZE;
/* // */
	/* calc cluster */
	while (offset>=clustersize) {
		ret=_f_getclustervalue(vi,f->pos.cluster,&cluster);
		if (ret) {
			f->mode=F_FILE_CLOSE;
			return ret;
		}

		offset-=clustersize;
		f->abspos+=clustersize;

		if (cluster>=F_CLUSTER_RESERVED) {
			if (!offset) {
				_f_clustertopos(vi,f->pos.cluster,&f->pos);
				f->pos.sector=f->pos.sectorend;
				return F_NO_ERROR; /* but eof! */
			}

			f->mode=F_FILE_CLOSE;
			return F_ERR_INVALIDSECTOR;
		}

		f->prevcluster=f->pos.cluster;
		f->pos.cluster=cluster;

	}

	_f_clustertopos(vi,f->pos.cluster,&f->pos);

	while (offset>=F_SECTOR_SIZE) {
		f->pos.sector++;

		offset-=F_SECTOR_SIZE;
		f->abspos+=F_SECTOR_SIZE;
	}

	f->relpos=offset;

	return _f_getcurrsector(vi,f);
}

/****************************************************************************
 *
 * fn_open
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

F_FILE *fn_open(const char *filename,const char *mode) {
F_DIRENTRY *de;
F_NAME fsname;
F_VOLUME *vi;
long m_mode=F_FILE_CLOSE;
F_FILEINT *f=0;

	if (mode[0]=='r' && mode[1]==0) m_mode=F_FILE_RD;
	if (mode[0]=='w' && mode[1]==0) m_mode=F_FILE_WR;
	if (mode[0]=='a' && mode[1]==0) m_mode=F_FILE_A;
	if (mode[0]=='r' && mode[1]=='+' && mode[2]==0) m_mode=F_FILE_RDP;
	if (mode[0]=='w' && mode[1]=='+' && mode[2]==0) m_mode=F_FILE_WRP;
	if (mode[0]=='a' && mode[1]=='+' && mode[2]==0) m_mode=F_FILE_AP;

	if (m_mode==F_FILE_CLOSE) return 0; 			/* invalid mode */

	if (_f_setfsname(filename,&fsname)) return 0; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return 0;/* invalid name */

	if (_f_getvolume(fsname.drivenum,&vi)) return 0; /* cant open any */

	f=_f_getintfile();
	if (!f) return 0; /* no more file could be opened  */


	f->drivenum=fsname.drivenum;
	f->relpos=0;
	f->abspos=0;
	f->filesize=0;
	f->prevcluster=0;
	f->modified=0;

	if (fsname.filename[0]=='.') return 0;

	switch (m_mode) {
		case F_FILE_RDP: /* r */
		case F_FILE_RD: /* r */

			if (!_f_findpath(vi,&fsname,&f->dirpos)) return 0;
			if (!_f_findfile(vi,fsname.filename,fsname.fileext,&f->dirpos,&de)) return 0;

   			if (de->attr & F_ATTR_DIR) return 0;		   /* directory */

			f->startcluster=_f_getdecluster(vi,de);

			if (f->startcluster) {
				_f_clustertopos(vi,f->startcluster,&f->pos);

				f->filesize=_f_getlong(&de->filesize);

				f->abspos=(unsigned long) -F_SECTOR_SIZE; /* forcing seek to read 1st sector! abspos=0; */
				if (_f_fseek(vi,f,0)) return 0;
			}
			else {
				f->pos.sector=0;
				f->pos.sectorend=0;
			}

   			break;

   		case F_FILE_AP:
   		case F_FILE_A: /* a */
			if (!_f_findpath(vi,&fsname,&f->dirpos)) return 0;

			f->pos.cluster=f->dirpos.cluster;
			f->pos.pos=f->dirpos.pos;
			f->pos.sector=f->dirpos.sector;
			f->pos.sectorend=f->dirpos.sectorend;

			if (_f_findfile(vi,fsname.filename,fsname.fileext,&f->dirpos,&de)) {

	   			if (de->attr & F_ATTR_DIR) return 0;		   /* directory */
				if (de->attr & F_ATTR_READONLY) return 0;

				if (_f_checklocked(fsname.drivenum,&f->dirpos)) return 0; /* locked */

				f->startcluster=_f_getdecluster(vi,de);

				f->filesize=_f_getlong(&de->filesize);

				if (f->startcluster) {
					_f_clustertopos(vi,f->startcluster,&f->pos);
					f->abspos= (unsigned long) -F_SECTOR_SIZE; /* forcing seek to read 1st sector! abspos=0; */
					if (_f_fseek(vi,f,f->filesize)) {
						f->mode=F_FILE_CLOSE;
						return 0;
					}
				}
				else {
					f->pos.sector=0;
					f->pos.sectorend=0;
				}
			}
			else {

				f->dirpos.cluster=f->pos.cluster;
				f->dirpos.pos=f->pos.pos;
				f->dirpos.sector=f->pos.sector;
				f->dirpos.sectorend=f->pos.sectorend;

				_f_clustertopos(vi,f->dirpos.cluster,&f->pos);

		   		if (_f_addentry(vi,&fsname,&f->dirpos,&de)) return 0;  /* couldnt be added */

				de->attr |= F_ATTR_ARC;		/* set as archiv */
				if (_f_writedirsector(vi)) return 0;

				f->startcluster=0;
				f->pos.sector=0;
				f->pos.sectorend=0;
			}

	   		break;

	   	case F_FILE_WR: /* w */
	   	case F_FILE_WRP: /* w+ */
			if (!_f_findpath(vi,&fsname,&f->dirpos)) return 0;

			_f_clustertopos(vi,f->dirpos.cluster,&f->pos);
			if (_f_findfile(vi,fsname.filename,fsname.fileext,&f->pos,&de)) {
				unsigned long cluster=_f_getdecluster(vi,de);	   /* exist */

				if (de->attr & F_ATTR_DIR) return 0;		   /* directory */
				if (de->attr & F_ATTR_READONLY) return 0;

				f->dirpos.cluster=f->pos.cluster;
				f->dirpos.pos=f->pos.pos;
				f->dirpos.sector=f->pos.sector;
				f->dirpos.sectorend=f->pos.sectorend;

				if (_f_checklocked(fsname.drivenum,&f->dirpos)) return 0; /* locked */

				de->filesize[0]=0;					/* reset size; */
				de->filesize[1]=0;					/* reset size; */
				de->filesize[2]=0;					/* reset size; */
				de->filesize[3]=0;					/* reset size; */

				de->attr |= F_ATTR_ARC;		/* set as archiv */
				de->clusterlo[0]=0;					/* no points anywhere */
				de->clusterlo[1]=0;					/* no points anywhere */
				de->clusterhi[0]=0;					/* no points anywhere */
				de->clusterhi[1]=0;					/* no points anywhere */

				if (vi->mediatype==F_FAT32_MEDIA) {
					_f_setword(&de->crtdate,f_getdate());  /* if there is realtime clock then creation date could be set from */
					_f_setword(&de->crttime,f_gettime());  /* if there is realtime clock then creation time could be set from */
					_f_setword(&de->lastaccessdate,f_getdate());  /* if there is realtime clock then creation date could be set from */
				}

				if (_f_writedirsector(vi)) return 0;

				if (_f_removechain(vi,cluster)) return 0; /* remove  */
			}
			else {
		   		if (_f_addentry(vi,&fsname,&f->dirpos,&de)) return 0;  /* couldnt be added */

				de->attr |= F_ATTR_ARC;		/* set as archiv */
				if (_f_writedirsector(vi)) return 0;
			}


			f->startcluster=0;
			f->pos.sector=0;
			f->pos.sectorend=0;

	   		break;

		default: return 0; /* invalid mode */
	}

	f->mode=m_mode; /*  lock it  */

	return &f->file;
}

/****************************************************************************
 *
 * fn_close
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

int fn_close(F_FILE *filehandle) {
F_DIRENTRY *de;
F_VOLUME *vi;
F_FILEINT *f=_f_check_handle(filehandle);
int ret;

	if (!f) return F_ERR_NOTOPEN;

	ret=_f_getvolume(f->drivenum,&vi);
	if (ret) return ret;

	switch (f->mode) {
	case F_FILE_CLOSE: return F_ERR_NOTOPEN;		   /* it was not open */

	case F_FILE_RD:
				f->mode=F_FILE_CLOSE;
				return F_NO_ERROR;
	case F_FILE_RDP:
	case F_FILE_WR:
	case F_FILE_A:
	case F_FILE_AP:
	case F_FILE_WRP:
				f->mode=F_FILE_CLOSE;

				if (_f_emptywritebuffer(vi,f)) {
					_f_removechain(vi,f->startcluster);
					/* dont care with return value of _f_removechain */
					return F_ERR_WRITE;
				}

				if (_f_writefatsector(vi)) {
					_f_removechain(vi,f->startcluster);
					/* dont care with return value of _f_removechain */
					return F_ERR_WRITE;
				}

				de=(F_DIRENTRY*)(vi->direntry+sizeof(F_DIRENTRY)*f->dirpos.pos);
				if (_f_getdirsector(vi,f->dirpos.sector)) { /* cannot get directory entry */
					_f_removechain(vi,f->startcluster);
					/* dont care with return value of _f_removechain */
					return F_ERR_WRITE;
				}

				if ((_f_getlong(&de->filesize)==(unsigned long)(f->filesize)) &&
					(_f_getdecluster(vi,de)==f->startcluster) ) {
					return F_NO_ERROR;
				}

				_f_setdecluster(vi,de,f->startcluster);
				_f_setlong(&de->filesize,f->filesize);
				_f_setword(&de->cdate,f_getdate());  /* if there is realtime clock then creation date could be set from */
				_f_setword(&de->ctime,f_gettime());  /* if there is realtime clock then creation time could be set from */

				if (vi->mediatype==F_FAT32_MEDIA) {
					_f_setword(&de->lastaccessdate,f_getdate());  /* if there is realtime clock then creation date could be set from */
				}

            return _f_writedirsector(vi);

	}

  	return F_ERR_NOTOPEN;
}

/****************************************************************************
 *
 * fn_read
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

long fn_read(void *buf,long size,long size_st,F_FILE *filehandle) {
F_VOLUME *vi;
F_FILEINT *f=_f_check_handle(filehandle);
char *buffer=(char*)buf;
long retsize=0;
	if (!f) return 0;

	if (f->mode!=F_FILE_RD && f->mode!=F_FILE_RDP && f->mode!=F_FILE_WRP && f->mode!=F_FILE_AP) return 0;

	size*=size_st;

	if (_f_getvolume(f->drivenum,&vi)) return 0; /* cant read any */

	if (size+f->relpos+f->abspos >= f->filesize) {	/* read len longer than the file */
		size=(f->filesize)-(f->relpos)-(f->abspos);  /* calculate new size */
	}

	if (size<=0) return 0;

	while (size) {
		unsigned long rdsize=size;

		if (f->relpos==F_SECTOR_SIZE) {
			int ret;

			f->abspos+=f->relpos;
			f->relpos=0;

			if (f->modified) {
				ret=_f_emptywritebuffer(vi,f); /* empty write buffer automatically goes to next sector */
				if (ret) {
					f->mode=F_FILE_CLOSE; /* no more read allowed */
					return retsize;
				}

				if (_f_writefatsector(vi)) {
					f->mode=F_FILE_CLOSE;
					return retsize;
				}
			}

			f->pos.sector++;	/* goto next */
			ret=_f_getcurrsector(vi,f);
			if (ret==F_ERR_EOF && (!size)) return retsize;

			if (ret) {
				f->mode=F_FILE_CLOSE; /* no more read allowed */
				return retsize;
			}
		}

		if (!size) break;

		if (rdsize>=F_SECTOR_SIZE-f->relpos) {
			rdsize=F_SECTOR_SIZE-f->relpos;
		}

		_f_memcpy(buffer,f->data+f->relpos,rdsize);

		buffer+=rdsize;
		f->relpos+=rdsize;
		size-=rdsize;
		retsize+=rdsize;

	}

	return retsize;

}

/****************************************************************************
 *
 * fn_write
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

long fn_write(const void *buf,long size,long size_st,F_FILE *filehandle) {
F_VOLUME *vi;
F_FILEINT *f=_f_check_handle(filehandle);
char *buffer=(char*)buf;
long retsize=0;
int ret;
	if (!f) return 0;

	if (f->mode!=F_FILE_WR && f->mode!=F_FILE_A && f->mode!=F_FILE_RDP && f->mode!=F_FILE_WRP && f->mode!=F_FILE_AP) return 0;

	size*=size_st;

	if (_f_getvolume(f->drivenum,&vi)) return 0; /* can't write */

	while (size) {
		unsigned long wrsize=size;

		if (f->relpos==F_SECTOR_SIZE) { /* now full */

			if (_f_emptywritebuffer(vi,f)) {
				f->mode=F_FILE_CLOSE;
				_f_removechain(vi,f->startcluster);
				/* dont care with return value of _f_removechain */
				return 0;
			}

			f->abspos+=f->relpos;
			f->relpos=0;

			f->pos.sector++;	/* goto next */
			if (wrsize<F_SECTOR_SIZE)
			{
			  ret=_f_getcurrsector(vi,f);

			  if (ret) {
			  	if (ret!=F_ERR_EOF) {
					f->mode=F_FILE_CLOSE; /* no more read allowed */
					return retsize;
				}
			  }
			}
		}

		if (wrsize>=F_SECTOR_SIZE-f->relpos) {
			wrsize=F_SECTOR_SIZE-f->relpos;
		}

		_f_memcpy(f->data+f->relpos,buffer,wrsize);

		f->modified=1;	 /* sector is modified */

		buffer+=wrsize;
		f->relpos+=wrsize;
		size-=wrsize;
		retsize+=wrsize;

		if (f->filesize<f->abspos+f->relpos) {
			f->filesize=f->abspos+f->relpos;
		}

	}

	return retsize;
}

/****************************************************************************
 *
 * fn_seek
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

int fn_seek(F_FILE *filehandle,long offset,long whence) {
F_FILEINT *f=_f_check_handle(filehandle);
F_VOLUME *vi;
int ret;

	if (!f) return F_ERR_NOTOPEN;

	if (f->mode!=F_FILE_RD && f->mode!=F_FILE_WR && f->mode!=F_FILE_A && f->mode!=F_FILE_RDP && f->mode!=F_FILE_WRP && f->mode!=F_FILE_AP) return F_ERR_NOTOPEN;

	ret=_f_getvolume(f->drivenum,&vi);
	if (ret) return ret;

	switch (whence) {
	case F_SEEK_CUR:  return _f_fseek(vi,f,f->abspos+f->relpos+offset);
	case F_SEEK_END:  return _f_fseek(vi,f,f->filesize+offset);
	case F_SEEK_SET:  return _f_fseek(vi,f,offset);
	}
	return F_ERR_NOTUSEABLE;
}

/****************************************************************************
 *
 * fn_tell
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

long fn_tell(F_FILE *filehandle) {
F_FILEINT *f=_f_check_handle(filehandle);

	if (!f) return 0;

	if (f->mode!=F_FILE_RD && f->mode!=F_FILE_WR && f->mode!=F_FILE_A && f->mode!=F_FILE_RDP && f->mode!=F_FILE_WRP && f->mode!=F_FILE_AP) return 0;

	return f->abspos+f->relpos;
}


/****************************************************************************
 *
 * fn_eof
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

int fn_eof(F_FILE *filehandle) {
F_FILEINT *f=_f_check_handle(filehandle);

	if (!f) return F_ERR_NOTOPEN;  /* if error */
	if (f->abspos+f->relpos<f->filesize) return 0;
	return F_ERR_EOF;/* EOF */
}


/****************************************************************************
 *
 * fn_rewind
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

int fn_rewind(F_FILE *filehandle) {
	return fn_seek( filehandle, 0L, F_SEEK_SET);
}

/****************************************************************************
 *
 * fn_putc
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

int fn_putc(int ch,F_FILE *filehandle) {
unsigned char tmpch=(unsigned char)(ch);
	if (fn_write(&tmpch,1,1,filehandle)==1) return tmpch;
	return -1;
}

/****************************************************************************
 *
 * fn_getc
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

int fn_getc(F_FILE *filehandle) {
unsigned char ch;
	if (fn_read(&ch,1,1,filehandle)==1) return ch;
	return -1;
}

/****************************************************************************
 *
 * fn_settimedate
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

int fn_settimedate(const char *filename,unsigned short ctime,unsigned short cdate) {
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
F_VOLUME *vi;
int ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/* invalid name */

	ret=_f_getvolume(fsname.drivenum,&vi);
	if (ret) return ret;

	if (!_f_findpath(vi,&fsname,&pos)) return F_ERR_INVALIDDIR;
	if (!_f_findfile(vi,fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;

	_f_setword(&de->ctime,ctime);
	_f_setword(&de->cdate,cdate);

	return _f_writedirsector(vi);
}

/****************************************************************************
 *
 * fn_gettimedate
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

int fn_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate) {
F_POS pos;
F_VOLUME *vi;
F_DIRENTRY *de;
F_NAME fsname;
int ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/* invalid name */

	ret=_f_getvolume(fsname.drivenum,&vi);
	if (ret) return ret;

	if (!_f_findpath(vi,&fsname,&pos)) return F_ERR_INVALIDDIR;
	if (!_f_findfile(vi,fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;


	if (pctime) *pctime=_f_getword(&de->ctime);
	if (pcdate) *pcdate=_f_getword(&de->cdate);

	return F_NO_ERROR;
}

/****************************************************************************
 *
 * fn_delete
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

int fn_delete(const char *filename) {
int ret;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
F_VOLUME *vi;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/* invalid name */

	ret=_f_getvolume(fsname.drivenum,&vi);
	if (ret) return ret;

	if (fsname.filename[0]=='.') return F_ERR_NOTFOUND;
	if (!(_f_findpath(vi,&fsname,&pos))) return F_ERR_INVALIDDIR;
	if (!_f_findfile(vi,fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;

	if (de->attr & F_ATTR_DIR) return F_ERR_INVALIDDIR;		   /* directory */
	if (de->attr & F_ATTR_READONLY) return F_ERR_ACCESSDENIED;      /* readonly */

	//if (_f_checklocked(fsname.drivenum,&pos)) return F_ERR_LOCKED;

	de->name[0]=0xe5;	/* removes it */

	ret=_f_writedirsector(vi);
	if (ret) return ret;

	return _f_removechain(vi,_f_getdecluster(vi,de));
}


/****************************************************************************
 *
 * fn_getattr
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

int fn_getattr(const char *filename,unsigned char *attr) {
F_POS pos;
F_VOLUME *vi;
F_DIRENTRY *de;
F_NAME fsname;
int ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/* invalid name */

	ret=_f_getvolume(fsname.drivenum,&vi);
	if (ret) return ret;

	if (!_f_findpath(vi,&fsname,&pos)) return F_ERR_INVALIDDIR;
	if (!_f_findfile(vi,fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;

	if (attr) *attr=de->attr;

   return F_NO_ERROR;
}

/****************************************************************************
 *
 * fn_setattr
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

int fn_setattr(const char *filename,unsigned char attr) {
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
F_VOLUME *vi;
int ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/* invalid name */

	ret=_f_getvolume(fsname.drivenum,&vi);
	if (ret) return ret;

	if (!_f_findpath(vi,&fsname,&pos)) return F_ERR_INVALIDDIR;
	if (!_f_findfile(vi,fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;

	attr&=F_ATTR_HIDDEN|F_ATTR_READONLY|F_ATTR_ARC|F_ATTR_SYSTEM; /* keep only valid bits */

	de->attr&=F_ATTR_DIR|F_ATTR_VOLUME; /* keep some original */

	attr|=de->attr;	/* add old settings, (dir,volume) */

	if (de->attr==attr) return F_NO_ERROR; /* its the same */

	de->attr=attr;

	return _f_writedirsector(vi);
}

/****************************************************************************
 *
 * fn_rmdir
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

int fn_rmdir(const char *dirname) {
int ret;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
F_VOLUME *vi;
F_FILEINT *f;
unsigned long a;

	if (_f_setfsname(dirname,&fsname)) return F_ERR_INVALIDNAME; /* invalid name */
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/* invalid name */

	ret=_f_getvolume(fsname.drivenum,&vi);
	if (ret) return ret;

	if (!(_f_findpath(vi,&fsname,&pos))) return F_ERR_INVALIDDIR;
	if (!_f_findfile(vi,fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;

	if (!(de->attr & F_ATTR_DIR)) return F_ERR_INVALIDDIR;		   /* not a directory */

	if (fsname.filename[0]=='.') return F_ERR_NOTFOUND;

	f=_f_getintfile();

	if (!f) return F_ERR_NOMOREENTRY;

	_f_clustertopos(vi,_f_getdecluster(vi,de),&f->pos);

	while (1) {
		F_DIRENTRY *de2;
		ret=_f_getcurrsector(vi,f);

		if (ret==F_ERR_EOF) break;
		if (ret) return ret;

		de2=(F_DIRENTRY *)(f->data);

		for (a=0; a<F_SECTOR_SIZE/sizeof(F_DIRENTRY); a++,de2++) {
			unsigned char ch=de2->name[0];
			if (!ch) break;
			if (ch==(unsigned char)0xe5) continue;
			if (ch=='.') continue;
			return F_ERR_NOTEMPTY; /* something is there */
		}
		f->pos.sector++;
	}

	de->name[0]=0xe5;

	ret=_f_writedirsector(vi);
	if (ret) return ret;

	return _f_removechain(vi,_f_getdecluster(vi,de));
}


/****************************************************************************
 *
 * fn_getlabel
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

int fn_getlabel(int drivenum, char *label, long len) {
char tmplabel[11];
long a;
F_VOLUME *vi;
F_POS pos;
int ret;
	if (!len) return F_NO_ERROR; /* how to store volume name? */

	ret=_f_getvolume(drivenum,&vi);
	if (ret) return ret;

	_f_clustertopos(vi,0,&pos);
	_f_memcpy(tmplabel,vi->bootrecord.volume_name,11);

	while (pos.cluster<F_CLUSTER_RESERVED) {
	   	for (;pos.sector<pos.sectorend;pos.sector++) {
   			F_DIRENTRY *de=(F_DIRENTRY*)(vi->direntry);

	   		 ret=_f_getdirsector(vi,pos.sector);
	   		 if (ret) return ret;

	   		 for (pos.pos=0; pos.pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos.pos++) {

   				if (!de->name[0]) continue;						 /* empty */
	   			if (de->name[0]==(unsigned char)0xe5) continue; /* deleted */
		   		if (de->attr == F_ATTR_VOLUME) {
			   		_f_memcpy(tmplabel,de->name,8);
   					_f_memcpy(tmplabel+8,de->ext,3);
   					goto vege;
   				}
   			 }
	   	}

		if (!pos.cluster) {
			if (vi->mediatype==F_FAT32_MEDIA) {
				pos.cluster=vi->bootrecord.rootcluster;
			}
			else goto vege;
		}

		{
			unsigned long nextcluster;
			if (_f_getclustervalue(vi,pos.cluster,&nextcluster)) goto vege;

	   		if (nextcluster>=F_CLUSTER_RESERVED) goto vege;

			_f_clustertopos(vi,nextcluster,&pos);
		}
  	}

vege:
	len--;
	for (a=0; a<11 && a<len; a++) {
		char ch=tmplabel[a];
		if (ch>='a' && ch<='z') ch+='A'-'a';
		label[a]=tmplabel[a];
	}
	label [a]=0; /* terminate it, this is ok because of len--! */

	for (;a>=0; a--) { /* cut the spaces */
		if (label[a]<=32) label[a]=0;
		else break;
	}

	return F_NO_ERROR;
}

/****************************************************************************
 *
 * fn_setlabel
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

int fn_setlabel(int drivenum, const char *label) {
F_NAME fsname;
int ret;
char tmplabel[11];
long a,b;
F_VOLUME *vi;
F_POS pos;
F_DIRENTRY *de;

	ret=_f_getvolume(drivenum,&vi);
	if (ret) return ret;

	_f_clustertopos(vi,0,&pos);

	for (a=b=0; a<11; a++) {
		char ch=label[b];
		if (ch>='a' && ch<='z') ch+='A'-'a';

		if (ch>=32) b++;
		else ch=32;

		tmplabel[a]=ch;
	}

	while (pos.cluster<F_CLUSTER_RESERVED) {
	   	for (;pos.sector<pos.sectorend;pos.sector++) {
			de=(F_DIRENTRY*)(vi->direntry);

			ret=_f_getdirsector(vi,pos.sector);
			if (ret) return ret;

			for (pos.pos=0; pos.pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos.pos++) {

   				if (!de->name[0]) continue;						 /* empty */
	   			if (de->name[0]==(unsigned char)0xe5) continue; /* deleted */
		   		if (de->attr == F_ATTR_VOLUME) {
					_f_memcpy(de->name,tmplabel,8);
					_f_memcpy(de->ext,tmplabel+8,3);
					return _f_writedirsector(vi);
				}
			}
   		}

		if (!pos.cluster) {
			if (vi->mediatype==F_FAT32_MEDIA) {
				pos.cluster=vi->bootrecord.rootcluster;
			}
			else break;
		}

		{
			unsigned long nextcluster;
			if (_f_getclustervalue(vi,pos.cluster,&nextcluster)) break;

	   		if (nextcluster>=F_CLUSTER_RESERVED) break;

			_f_clustertopos(vi,nextcluster,&pos);
		}
  	}

	_f_clustertopos(vi,0,&pos);

   fsname.drivenum=drivenum;
   _f_memcpy(fsname.filename,tmplabel,F_MAXNAME);
   _f_memcpy(fsname.fileext,tmplabel+F_MAXNAME,F_MAXEXT);

	if (_f_addentry(vi,&fsname,&pos,&de)) return F_ERR_NOMOREENTRY;  /* couldnt be added */

	de->attr=F_ATTR_VOLUME;
	return _f_writedirsector(vi);
}


/****************************************************************************
 *
 * end of fat.c
 *
 ***************************************************************************/

#endif  /*  !F_LONGFILENAME  */
#endif /* _FAT_C_ */

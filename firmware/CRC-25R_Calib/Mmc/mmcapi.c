/****************************************************************************
 *
 *            Copyright (c) 2004 by Capintec Inc
 ***************************************************************************/

/****************************************************************************
**    MODULE: MMC
**
**    FILE: mmcapi.c
**
**    DATE: 06/15/05
**
**    This module contains all routines that use the MMC/SD Card
****************************************************************************/
#include "crc.h"
#include "coldfire.h"
#include "qspi.h"
#include "pit.h"
#include "mmc.h"
#include "cs.h"
#include "mmctest.h"
#include "string.h"
#include "screen.h"
#include "port.h"
#include "keyboard.h"

extern int _f_getvolume(int drivenum,F_VOLUME **pvi);

// F_FUNC defined in fat.h
static F_FUNC mmc_api;

static F_PHY mmc_phy;


static void card_read_error(void);

bool init_mmc_card(void)
{
    long mmc_status;
    int ret;

    //power should be off
    //power_mmc(FALSE);

    //delay_msec(50);

    //turn power on
    //power_mmc(TRUE);
    //wait for power on
    //delay_msec(100);


	// MMC Card Initialization....starts here

	// Pass API Structure for filling
	mmc_initfunc(&mmc_api);

	//spiCSDBlock() is called by spiSDInit() which is called by mmc_getphy(), mmc_getstatus()

	mmc_status = mmc_api.getstatus();
    if(mmc_status != 0)
    {
        card_read_error();
        return FALSE;
    }    
    

	mmc_api.getphy(&mmc_phy);

    
    ret=_f_mmcpoweron();
    if (ret)
    {
        card_read_error();
        return FALSE;
    }    

    return TRUE;
}

static void card_read_error(void)
    {
        beep();
        erase_screen();
        display_text(40,10,"Error",0,MEDIUM,REV);
        display_text(0,24,"Insert Card",0,SMALL,NORMAL);
        display_text(0,36,"And Restart",0,SMALL,NORMAL);

        for(;;)
        {
            service_watchdog();
        }    
    }


bool test_mmc_rw(uint startval, uint sector)
{
	uchar write_sec_data[512]; // write sector data
	uchar read_sec_data[512];
	uint i=0;
	uint secno=0;
	uint start=startval;


	for (secno=0; secno<mmc_phy.number_of_sectors; secno++ )
	{
		for (i=0; i<512; i++)
		{
			write_sec_data[i] = (i+start)%256;
			read_sec_data[i] = 0;
		}

		// Write a sector
		mmc_api.writesector(&write_sec_data,sector);

		// Read a sector
		mmc_api.readsector(&read_sec_data,sector);

		for (i=0; i<512; i++)
		{
			if (read_sec_data[i] != write_sec_data[i])
			{
#if SYSCALLS
                printf("Sector %x, Address %x: Wrote %x ,Read %x\r\n",sector,i,write_sec_data[i],read_sec_data[i]);
#endif                
				break;
			}
		}
		start++;
	}

    return TRUE;

}


int mmc_write_volname(char *volname)
{
	int ret;
	unsigned int parlen,len;
	F_BOOTRECORD *f_bootrecord;
	F_VOLUME *vi;

		parlen = strlen(volname);
		if (parlen < 11) len=parlen; else len=11;  //prevent overrun

		ret=_f_getvolume(0,&vi);
		if (ret)
		{
#if SYSCALLS
            printf("Error %d, Cannot Write Volume Name %s",ret,volname);
#endif            
			return F_ERR_NOTUSEABLE;
		}

		f_bootrecord=&vi->bootrecord;
		strncpy((char *)f_bootrecord->volume_name,(const char *)volname,len);
		return F_NO_ERROR;
}


int mmc_display_mbr(void)
{
	F_BOOTRECORD *f_bootrecord;
	F_VOLUME *vi;
	int ret;
	char jcode[4];
	char oem[9];
	char volname[12];
	char fatname[9];
	char exm[3];

		memset(jcode,'\0', 4);
		memset(oem,'\0', 9);
		memset(volname,'\0', 12);
		memset(fatname,'\0', 9);
		memset(exm,'\0', 3);

		// Clive removed 5/18/04 _f_mmcpoweron();

		ret=_f_getvolume(0,&vi);
		if (ret && ret!=F_ERR_NOTFORMATTED)
		{
			if (ret=f_format(0,F_FAT16_MEDIA))
			{
#if SYSCALLS
                printf("Cannot Format MMC Card!, Error: %d",ret);
#endif                
				return ret;
			}
		}

		f_bootrecord=&vi->bootrecord;

		strncpy(jcode,(const char *)f_bootrecord->jump_code,3);
		strncpy(oem,(const char *)f_bootrecord->OEM_name,8);
		strncpy(volname,(const char *)f_bootrecord->volume_name,11);
		strncpy(fatname,(const char *)f_bootrecord->FAT_name,8);
		strncpy(exm,(const char *)f_bootrecord->executable_marker,2);

#if SYSCALLS
        printf("Master Boot Record:-\n");
		printf("\tjump_code: %s\n",jcode);
		printf("\tOEM_name: %s\n",oem);
		printf("\tbytes_per_sector: %d\n",f_bootrecord->bytes_per_sector);
		printf("\tsector_per_cluster: %c\n",f_bootrecord->sector_per_cluster);
		printf("\treserved_sectors: %d\n",f_bootrecord->reserved_sectors);
		printf("\tnumber_of_FATs: %c\n",f_bootrecord->number_of_FATs);
		printf("\tmax_root_entry: %d\n",f_bootrecord->max_root_entry);
		printf("\tnumber_of_sectors_less32: %d\n",f_bootrecord->number_of_sectors_less32);
		printf("\tmedia_descriptor: %c\n",f_bootrecord->media_descriptor);
		printf("\tsector_per_FAT: %d\n",f_bootrecord->sector_per_FAT);
		printf("\tsector_per_Track: %d\n",f_bootrecord->sector_per_Track);
		printf("\tnumber_of_heads: %d\n",f_bootrecord->number_of_heads);
		printf("\tnumber_of_hidden_sectors: %l\n",f_bootrecord->number_of_hidden_sectors);
		printf("\tlogical_drive_num: %d\n",f_bootrecord->logical_drive_num);
		printf("\textended_signature: %c\n",f_bootrecord->extended_signature);
		printf("\tserial_number: %l\n",f_bootrecord->serial_number);
		printf("\tvolume_name: %s\n",volname);
		printf("\tFAT_name: %s\n",fatname);
#endif                

		return 0;

} //END mmc_display_mbr


int mmc_get_directory(char *dirname, int maxlen)
{

	int ret;

		ret=f_getcwd(dirname, maxlen);
		if (ret)
        {
#if SYSCALLS
            printf(" getcwd Error: %d\n",ret); return ret;
#endif
        }

		return ret;
}

int mmc_change_dir(char *direc)
{
	int ret;
	char cwd[F_MAXPATH];

		ret=f_chdir(direc);
		if (ret)
        {
#if SYSCALLS
            printf(" chdir Error: %d\n",ret); return ret;
#endif
        }

		ret=f_getcwd(cwd,F_MAXPATH);
		if (ret)
        {
#if SYSCALLS
            printf(" cwd Error: %d\n",ret); return ret;
#endif
        }

		return ret;
}

int mmc_hardformat(void)
{

	int ret;
//	F_FIND find;

    ret=f_hardformat(0,F_FAT16_MEDIA);
#if SYSCALLS
    if (ret)
    {
        printf("HardFormat Error: %d\n",ret); return ret;
    }
    else printf("Hard Format Complete\n");
#endif

    return ret;

}

int mmc_reformat(void)
{
	int ret;
	F_FIND find;

    //ret=_f_mmcpoweron();
    //if (ret) {printf(" InitVolume Error: %d\n",ret); return ret;}

    ret=f_format(0,F_FAT16_MEDIA);
#if SYSCALLS
	if (ret) {printf("Reformat Error: %d\n",ret); return ret;}
#endif

	ret=f_findfirst("*.*",&find);
#if SYSCALLS
	if (ret!=F_ERR_NOTFOUND) {printf("Volume Not empty error: %d\n",ret);return ret;}
#endif
    

	return ret;
}

int mmc_display_dir(char *dir)
{
	char cwd[F_MAXPATH];
	F_FIND find;
	int ret;

//	ret=f_chdir("\\");
	ret=f_chdir(dir);
	if (ret)
	{
#if SYSCALLS
		printf("Error: %d, Could not chdir to Directory: %s\n",ret,dir);
#endif
		return ret;
	}

	ret=f_findfirst("*.*",&find);
#if SYSCALLS
	if (ret==F_ERR_NOTFOUND) printf("No Files Found!\n");
#endif

	while (ret!=F_ERR_NOTFOUND)
	{
		while (find.attr==F_ATTR_DIR)
		{
#if SYSCALLS
            printf("Found Directory: %s\n",find.filename);
#endif

			ret=f_chdir(find.filename);
			if (ret=f_findnext(&find)) break;	//exit when no more

		}

		while (ret!=F_ERR_NOTFOUND)
		{
#if SYSCALLS
			printf("\tFound file: %s,  length: %d\n", find.filename,find.filesize);
#endif
			ret=f_findnext(&find);
		}

		if (strcmp("",cwd)==0) break;	//exit if we're in root directory

		ret=f_chdir("..");				//go back up a level
		if (ret==F_NO_ERROR)
			ret=f_findnext(&find);		// step to next directory on same level
	}

	// return to Root Level
	ret=f_chdir("\\");
	return ret;
}



int mmc_make_directory(char *dirname)
{
	int ret;

	if (ret=f_mkdir(dirname))
	{
#if SYSCALLS
		printf("\tmkdir %s failed!\n",dirname);
#endif
		return ret;
	}

	return F_NO_ERROR;

} // end make_directory


int mmc_file_exists(char *filename)
{
	F_FILE *file;

	file=f_open(filename,"r");
	if (!file)
	{
		//printf ("\n\rFile %s cannot be opened!",filename);
		return F_ERR_NOTFOUND;
	}

	f_close(file);
	return F_NO_ERROR;
}


int mmc_read_file(char *filename, char *buffer, unsigned long size)
{
	F_FILE *file;
	//int ret;
	long bytesread=0;

	// NOTE: If file exists already it will be overwritten!
	file=f_open(filename,"r");
	if (!file)
    {
#if SYSCALLS        
        printf("\nf_open() file: %s failed!\n",filename);
#endif
        return F_ERR_NOTOPEN;
    }

	bytesread = f_read( (void *)buffer,size,1,file);
#if SYSCALLS        
	printf("read %d bytes from file %s\n",bytesread,filename);
#endif

	//ret=f_close(file);
	f_close(file);
	return bytesread;

} // end mmc_read_file


int mmc_create_file(char *filename, char *buffer, unsigned long length)
{
	F_FILE *file;
	F_SPACE before,after;
	int ret;
	//long size;

	ret=f_getfreespace(0,&before);
#if SYSCALLS        
	printf("Free Space= %d, Used Space= %d\n",before.free,before.used);
#endif

	if (ret)
    {
#if SYSCALLS        
        printf("Free Space Error!\n");
#endif
        return ret;
    }

	// NOTE: If file exists already it will be overwritten!
	file=f_open(filename,"w");
	if (!file)
    {
#if SYSCALLS        
        printf("\nf_open() file: %s failed!\n",filename);
#endif
        return ret;
    }

	//size=f_write(buffer,length,1,file);
	f_write(buffer,length,1,file);
#if SYSCALLS        
	printf("wrote %d bytes to file %s\n",size,filename);
#endif

	ret=f_close(file);
	if (ret)
    {
#if SYSCALLS        
        printf("File Close Error!\n");
#endif
        return ret;
    }

	ret=f_getfreespace(0,&after);
#if SYSCALLS        
	printf("Free Space= %d, Used Space= %d\n",after.free,after.used);
#endif

	return ret;
} // end mmc_create_file

int mmc_rename_file(const char *oldname, const char *newname)
{
	int ret;

	ret=f_rename(oldname, newname);
#if SYSCALLS        
	if (ret)
        printf("Error %d renaming file: %s  to  %s\n",ret,oldname,newname);
#endif
	return ret;

} // end mmc_rename_file


int mmc_delete_file(char *filename)
{
	int ret;

	ret=f_delete(filename);
#if SYSCALLS        
	if (ret)
        printf("Error %d deleting file: %s\n",ret,filename);
#endif
	return ret;
}  // end mmc_delete_file

int mmc_settimedate(char *filename)
{

	int ret;

	ret=f_settimedate(filename,f_gettime(),f_getdate());
#if SYSCALLS        
	if (ret)
        printf("Error %d setting date and time on file: %s\n",ret,filename);
#endif
	return ret;

} // end mmc_settimedate


int mmc_gettimedate(char *filename, ushort *pctime, ushort *pcdate)
{

	int ret;

	ret=f_gettimedate(filename,pctime,pcdate);
#if SYSCALLS        
	if (ret)
        printf("Error %d getting date and time on file: %s\n",ret,filename);
#endif
	return ret;

}  // end mmc_gettimedate



int _f_mmcpoweron(void)
{

	return f_initvolume(0,mmc_initfunc);

}


/////////////////////////////////////////////////////////////////////
// mmc test api functions
/////////////////////////////////////////////////////////////////////

// _f_dump() displays text from the running MMC tests
void _f_dump (char *s)
{
#if SYSCALLS        
   printf("%s\n",s);
#endif
}


// _f_result() function to display errors detected during the MMC test
long _f_result(long testnum, long error)
{
#if SYSCALLS        
	printf("MMC test number %d failed with error %d/n", testnum, error);
#endif
	return(testnum);
}

F_FILE* mmc_create_file_for_append(char *filename)
{
    F_FILE *file;
    F_SPACE before;
    int ret;

    ret=f_getfreespace(0,&before);
#if SYSCALLS        
    printf("Free Space= %d, Used Space= %d\n",before.free,before.used);
#endif

    if (ret)
    {
#if SYSCALLS        
        printf("Free Space Error!\n");
#endif
        return NULL;
    }

    // NOTE: If file exists already it will be overwritten!
    file=f_open(filename,"w");
    if (!file)
    {
#if SYSCALLS        
        printf("\nf_open() file: %s failed!\n",filename);
#endif
        return NULL;
    }
    return file;

}

F_FILE* mmc_open_file_for_append(char *filename)
{
    F_FILE *file;
    F_SPACE before;
    int ret;

    ret=f_getfreespace(0,&before);
#if SYSCALLS        
    printf("Free Space= %d, Used Space= %d\n",before.free,before.used);
#endif

    if (ret)
    {
#if SYSCALLS        
        printf("Free Space Error!\n");
#endif
        return NULL;
    }

    // file is created if it does not exist. stream positioned at end of the file
    file=f_open(filename,"a");
    if (!file)
    {
#if SYSCALLS        
        printf("\nf_open() file: %s failed!\n",filename);
#endif
        return NULL;
    }
    return file;

}

//append to open file
void mmc_append_to_file(F_FILE *file, char *buffer, unsigned long length)
{
    //long size;

    //size=f_write(buffer,length,1,file);
    f_write(buffer,length,1,file);
}

int mmc_close_file(F_FILE *file)
{
    F_SPACE after;
    int ret;

    ret=f_close(file);
    if (ret)
    {
#if SYSCALLS        
        printf("File Close Error!\n");
#endif
        return ret;
    }

    ret=f_getfreespace(0,&after);
#if SYSCALLS        
    printf("Free Space= %d, Used Space= %d\n",after.free,after.used);
#endif

    return ret;
} // end mmc_create_file

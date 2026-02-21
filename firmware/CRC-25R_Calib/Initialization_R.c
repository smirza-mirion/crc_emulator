/*********************************************************************

  MODULE:	INITIALIZATION for CRC-25R

  FILE:		initialization_R.c

  DATE:     05/23/07
            08/22/07 -- removed automated constancy test

  CALLED BY:
		factory menu


  *************************************************************************/
#include "crc.h"
#include "menu.h"
#include "screen.h"
#include "i2c.h"
#include "printer.h"
#include "keyboard.h"
#include "qspi.h"
#include "daily.h"
#include "nuc.h"
#include "chambfac.h"
#include "mca.h"
#include "counter.h"
//#include "mmcapi.h"
//#include "fat_m.h"
#include "ff.h"
#include "database.h"
#include <string.h>

extern const KEYDEF keydef[];
extern const char cap_str[];
extern const char mir_str[];
extern CHAMBER chamber[];
extern short max_chambers;

    void ee_init(void)
    {
        short i, j;
        uchar uee;
        float fee;
        short us;
        uchar uc;
        bool eebool;
        time_t eetime;
        char sn[8];
        NUCDATA_OBSOLETE emptyusernuclide;
        LINDEF linearityempty;
        //DETECTOR detector;
        float fullEfficiency;
        FILINFO fileInfo;
        char longFileName[100];
        uchar zeros[8];
        char initstr[8];
        int index;

        //disable adc servicing
        set_adc_enabled(FALSE);
        
        display_text(10,10,"Please Wait",0,MEDIUM,NORMAL);
        for(index=0; index<8; index++) zeros[index] = 0;
        for(index=0; index<4096-8; index++){
        	eeread((uchar *) initstr, index, 8, EE_CRC);
        	if(strncmp(initstr, cap_str, 8) == 0){
        		eewrite(index, zeros, 8, EE_CRC);
        	}
        }

        for(i = 0; i < 6; i++)
            sn[i] = '0';
        EE_WRITE(snum,(uchar *)sn);
        EE_WRITE(snum1,(uchar *)sn);

        eetime = (time_t) 0;
        uee = 0xff;
        eebool = FALSE;
        for(i = 0; i < 6; i++)
        {   
        	// Removed:
            //EE_WRITE(stand[i].syst,&uee);
            EE_WRITE(stand[i].caldate, (uchar *)&eetime);
            EE_WRITE(stand[i].daily,(uchar *)&eebool);
            EE_WRITE(stand[i].const_iso,(uchar *)&eebool);
        }

        EE_WRITE(stand_ge68.caldate, (uchar *) &eetime);
        EE_WRITE(stand_ge68.daily, (uchar *) &eebool);
        EE_WRITE(stand_ge68.const_iso, (uchar *) &eebool);

        uee = 0;
        EE_WRITE(constancysource, &uee);
        us = -1;
        for(i=0; i<12; i++){
        	EE_WRITE(constancych[i], (uchar *) &us);
        }
        
        //set user nuclides to none
        emptyusernuclide.show = 0;
        emptyusernuclide.halflife = -1;
        emptyusernuclide.response[R_CHAMB] = 0.0;
        emptyusernuclide.response[P_CHAMB] = 0.0;
        emptyusernuclide.user_response[R_CHAMB] = 0.0;
        emptyusernuclide.user_response[P_CHAMB] = 0.0;
        emptyusernuclide.name[0] = 0;
        emptyusernuclide.fullname[0] = 0;
        emptyusernuclide.code[0] = 0;
        emptyusernuclide.hlunit = -1;
        emptyusernuclide.energy1 = 0;
        emptyusernuclide.energy2 = 0;
        emptyusernuclide.energy3 = 0;
        emptyusernuclide.wellefficiency = 0;
        emptyusernuclide.betaefficiency = 0;
        emptyusernuclide.userenergy1 = -1;
        emptyusernuclide.userenergy2 = -1;
        emptyusernuclide.userenergy3 = -1;
        emptyusernuclide.userwellefficiency = -1;
        emptyusernuclide.userbetaefficiency = -1;
        emptyusernuclide.wellmeasuredon = (time_t) 0;
        emptyusernuclide.betameasuredon = (time_t) 0;
        for(i = 0; i < 10; i++)
            EE_WRITE(obsolete8[i],(uchar *)&emptyusernuclide);

        us = -1;
        //set user calibration numbers to none
        for(i = 0; i < MAX_NEW_CAL_OBSOLETE; i++)
            EE_WRITE(obsolete9[i].nuc_index,(uchar *)&us);
        
        //system = CI, lock = 0
        uee = 0;
        EE_WRITE(syst,&uee);
        eebool = FALSE;
        EE_WRITE(syslock,(uchar *)&eebool);
        //no printer, ticket
        uee = NONE_PRINTER;
        EE_WRITE(print[0],&uee);
        uee = TICKET;
        EE_WRITE(print[1],&uee);
                
        
        //moassay -- all methods
        uee = 1;
        for(i = 0; i < 3; i++)
            EE_WRITE(mo_choice[i],&uee);


        //set mo_tc_lim to US values of .15
        fee = 0.15;
        EE_WRITE(mo_tc_lim,(uchar *)&fee);

        //set user keys to none
        us = -1;
        //for(j = 0; j < 10; j++)
        //    EE_WRITE(user_keys[0].keys[j],(uchar *)&us);

        us = NuclideData_getIndexFromName("Ba133");
        EE_WRITE(user_keys[0].keys[0], (uchar *) &us);

        us = NuclideData_getIndexFromName("Co57");
        EE_WRITE(user_keys[0].keys[1], (uchar *) &us);

        us = NuclideData_getIndexFromName("Co60");
        EE_WRITE(user_keys[0].keys[2], (uchar *) &us);

        us = NuclideData_getIndexFromName("Cs137");
        EE_WRITE(user_keys[0].keys[3], (uchar *) &us);

        us = NuclideData_getIndexFromName("Ga67");
        EE_WRITE(user_keys[0].keys[4], (uchar *) &us);

        us = NuclideData_getIndexFromName("I 131");
        EE_WRITE(user_keys[0].keys[5], (uchar *) &us);

        us = NuclideData_getIndexFromName("Na22");
        EE_WRITE(user_keys[0].keys[6], (uchar *) &us);

        us = NuclideData_getIndexFromName("Tc99m");
        EE_WRITE(user_keys[0].keys[7], (uchar *) &us);

        us = NuclideData_getIndexFromName("Tl201");
        EE_WRITE(user_keys[0].keys[8], (uchar *) &us);

        us = -1;
        EE_WRITE(user_keys[0].keys[9], (uchar *) &us);

        //set screen saver to 10 minutes
        us = 10;
        EE_WRITE(maxscr,(uchar *)&us);

        //set contrast to starting value
        uee = 40;
        EE_WRITE(contrast,&uee);

        //no linearity test defined
        us = -1;
        /*linearityempty.type = -1;
        linearityempty.num = 0;
        for(i=0; i<12; i++) linearityempty.hours[i] = -1;
        linearityempty.nuc_index = NuclideData_getIndexFromName("Tc99m");
        for(i=0; i<12; i++) linearityempty.lin_cal_factors[i] = -999;*/
        for(j=0; j<2; j++){
        	linearityempty.num_Std[j] = -1;
        	if(j==0) linearityempty.nuc_index_Std[j] = NuclideData_getIndexFromName("Tc99m");
        	else linearityempty.nuc_index_Std[j] = NuclideData_getIndexFromName("F 18");
        	for(i=0;i<12;i++) linearityempty.hours_Std[j][i] = -1;

        	linearityempty.num_Lin[j] = -1;
        	linearityempty.nuc_index_Lin[j] = NuclideData_getIndexFromName("Tc99m");
        	for(i=0;i<7;i++) linearityempty.chamb_num_Lin[j][i] = 0;
        	for(i=0;i<11;i++) linearityempty.serial_num_Lin[j][i] = 0;
        	for(i=0;i<8;i++) linearityempty.factors_Lin[j][i] = -999;

        	linearityempty.num_Cali[j] = -1;
        	linearityempty.nuc_index_Cali[j] = NuclideData_getIndexFromName("Tc99m");
        	for(i=0;i<7;i++) linearityempty.chamb_num_Cali[j][i] = 0;
        	for(i=0;i<11;i++) linearityempty.serial_num_Cali[j][i] = 0;
        	for(i=0;i<12;i++) linearityempty.factors_Cali[j][i] = -999;
        }

        EE_WRITE(lindef,(uchar *)&linearityempty);

        //initialize chamber data
        for(i=0; i<max_chambers; i++){
        	chamber[i].exists = TRUE;
        	chamber[i].bkg = 0.;
        	chamber[i].adc_zero = 200;
        	chamber[i].zero = 0.;

        	chamber[i].nuc_index = NuclideData_getIndexFromName("Tc99m");
        	NuclideData_getNuclide(NuclideData_getIndexFromName("Tc99m"), &(chamber[i].nucdata));
        }

        //initialize remote nuclides
        EE_WRITE(remote_nucs,(uchar *)&keydef[0]);
        
        //initialize inventory
        //initialize_inventory();

        uc = 255;
        EE_WRITE(volume, &uc);

        uc = 100;
        EE_WRITE(brightness, &uc);

        uc = 10;
        EE_WRITE(sleepbrightness, &uc);

        uc = 0;
        EE_WRITE(sleeptimeout, &uc);

        us = 84;
        for(i=0; i<8; i++) EE_WRITE(nuclideID[i], (uchar *)&us);

        DB_InitializeDetector();

        fullEfficiency = 37.04;
        EE_WRITE(obsolete10, (uchar *) &fullEfficiency);

        //write init string
        EE_WRITE(init_str,(uchar *)cap_str);
        EE_WRITE(init_str1,(uchar *)mir_str);

		//f_chdir("\\");
        f_chdrive(0);
        f_chdir("/");

		//if(f_chdir("data") != F_NO_ERROR){
        if(f_chdir("data") != FR_OK){
			//f_mkdir("data");
        	f_mkdir("data");

			//f_chdir("\\");
        	f_chdir("/");

			//f_chdir("data");
        	f_chdir("data");
		}

        fileInfo.lfname = longFileName;
        fileInfo.lfsize = 100;

		//if(mmc_file_exists("hotkeys.txt") == 0) f_delete("hotkeys.txt");
        if(f_stat("hotkeys.txt", &fileInfo) == FR_OK) f_unlink("hotkeys.txt");

		//if(mmc_file_exists("usereff.bin") == 0) f_delete("usereff.bin");
        //if(f_stat("usereff.bin", &fileInfo) == FR_OK) f_unlink("usereff.bin");

		//if(mmc_file_exists("inventry.bin") == 0) f_delete("inventry.bin");
        if(f_stat("inventry.bin", &fileInfo) == FR_OK) f_unlink("inventry.bin");

		//if(mmc_file_exists("sysset.bin") == 0) f_delete("sysset.bin");
        if(f_stat("sysset.bin", &fileInfo) == FR_OK) f_unlink("sysset.bin");

		//if(mmc_file_exists("comm.bin") == 0) f_delete("comm.bin");
        if(f_stat("comm.bin", &fileInfo) == FR_OK) f_unlink("comm.bin");

        //DB_shutdown();
        //if(f_stat("capintec.db", &fileInfo) == FR_OK) f_unlink("capintec.db");
        //if(f_stat("capintec.db-journal", &fileInfo) == FR_OK) f_unlink("capintec.db-journal");

        display_text(10,10,"           ",0,MEDIUM,NORMAL);
    }

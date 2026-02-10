/*********************************************************************
  MODULE:	FACTORY MENU for CRC-25

  FILE:		FactoryMenu_25.c

  DATE:		03/26/08

  ANALYSIS:		factory setup

  DESIGN: 		special password input before accessing menu

  CALLED BY:
		util


  *************************************************************************/
#include "crc.h"
#include "menu.h"
#include "screen.h"
#include "i2c.h"
#include "printer.h"
#include "keyboard.h"
#include "qspi.h"
#include "chambfac.h"
#include "remote.h"
#include "mca.h"
#include "database.h"
#include <string.h>

static void cor60in(void);
static void chamber_facmenu(void);
void ee_init(void);
static void chamber_ee_init(void);
void write_to_chamber(short ch_num, CHAMBERVALS *ch_vals);
void sernumin(short mode);
static void sysvals(void);
void SetKeyFeatures(void);


extern char cap_str[];
extern char mir_str[];
extern char rev_num_str1_0[];
extern char rev_num_str1_1[];
extern CHAMBERVALS chamb_vals[];
extern CURRENT current;

void first_initialization(void){
	char initstr[10];
	int index;
	USER user;

	//ee_init();
	//strcpy(initstr, "        ");
	//EE_WRITE(init_str, (uchar *) initstr);
	for(index=0; index<4096 - 8; index++){
		if(index != offsetof(EEVALS,init_str)){
			eeread((uchar *)initstr, index, 8, EE_CRC);
			if(strncmp(initstr, cap_str, 8) == 0){
				ee_init();
				break;
			}
		}
	}

	EE_READ(init_str, (uchar *)initstr);
	if(!strncmp(initstr, cap_str, 8) == 0) ee_init();
	EE_READ(init_str1, (uchar *)initstr);
	if(strncmp(initstr, mir_str, 6)){
		for(index=0; index<10; index++) initstr[index] = '0';
		EE_WRITE(snum1, (uchar *)initstr);
		EE_WRITE(init_str1, (uchar *)mir_str);
	}
	EE_READ(syst, &current.system);
	EE_READ(syslock, (uchar *) &current.lock);
	current.branding = DB_ReadBranding();
	current.default_keV = DB_ReadDefaultKEV();
	current.security_mode = DB_ReadSecurityMode();
	if(current.security_mode == 0){
		DB_GetUser("root", &user);
		current.user_id = user.UserID;
	}else if(current.security_mode == 1){
		current.user_id = -1;
	}else if(current.security_mode == 2){
		DB_GetUser("Guest", &user);
		current.user_id = user.UserID;
	}
	current.time_format = DB_ReadTimeFormat();
	current.usb_device_protocol = DB_ReadUSBDeviceProtocol();
	current.future_date_input = DB_ReadFutureDateInput();
	current.language = DB_ReadLanguage();
	current.feed_label = DB_ReadFeedLabel();
	current.moly_streamlined = DB_ReadMolyStreamlined();
	current.key = DB_ReadKey();
	SetKeyFeatures();
	DB_ReadPassword(current.password);
	current.bypass = -1;
}
    
void facmenu(void){

        short iret;
        char strng[10];

	if(!numstr_in(0, 0, strng, SMALL, 0, 4, 4, FALSE, TRUE) || home_set()) return;

	if(!(strng[0]=='5' && strng[1]==DIVSIGN && strng[2]=='1' && strng[3]=='3')) return;

        remote_busy(TRUE);

	for(;;){
            iret = display_menu(FACTORY);

		if(home_set()) return;
            
		switch(iret){
            case -1: //MENU
				return;

            case 1:
                erase_screen();
                ee_init();
                break;
            case 2:
                sernumin(0);
                break;
            case 3:
                chamber_facmenu();
                break;

            }
            if(home_set())
                return;
		}
	}

static void chamber_facmenu(void){

        short iret;
        CHAMBERVALS ch_vals;

        //if never initialized, do initialization now
	read_from_chamber(0, &ch_vals);
        
        //if init_str has not been written, do initialization
	if(!strncmp(&ch_vals.init_str[0], cap_str, 8) == 0) chamber_ee_init();
            
	for(;;){

            iret = display_menu(CAL_FACTORY);
		if(home_set()) return;

		switch (iret){

            case -1: //MENU
                return;

            case 1:
                cor60in();
                break;
            case 3:
                sernumin(-1);
                break;
            case 4:
                sysvals();
                break;
            }
		}
	}

    
    //response correction for Co60
static void cor60in(void){
		short nchoose;
		float resp;
		float pcdev;
        char strng[30];
        CHAMBERVALS ch_vals;


        //read data from chamber
	read_from_chamber(0, &ch_vals);

	for(;;){
			erase_screen();
		display_text(0, 0, "Display for Co60", 0, SMALL,NORMAL);
		display_text(0, 8, "Standard Source is", 0, SMALL,NORMAL);
		display_text(0, 16, "Low  by       %", 0, SMALL, NORMAL);
			resp = 100. * ch_vals.resp_corr;
            if(resp > 0.)
			display_text(0, 16, "High", 0, SMALL, NORMAL);

		if(resp < 0.) resp = -resp;

		sprintf(strng, "%#5.2f", resp);
		display_text(48, 16, strng, 0, SMALL, NORMAL);
		display_text(8, 52, "CORRECT?", 0, MEDIUM, NORMAL);

		if(yesorno()){
                //write the data to the chamber
                write_to_chamber(0, &ch_vals);

                //save in RAM
			memcpy(&chamb_vals[0], &ch_vals, sizeof(ch_vals));
                
                return;
			}
			erase_screen();

		display_text(0, 0, "Is Activity Display", 0, SMALL, NORMAL);
		display_text(0, 8, "for Co60 Standard", 0, SMALL, NORMAL);
		display_text(0, 16, "Source Higher or", 0, SMALL, NORMAL);
		display_text(0, 24, "Lower than Expected", 0, SMALL, NORMAL);
		display_text(0, 32, "1. Higher", 0, SMALL, NORMAL);
		display_text(0, 40, "2. Lower", 0, SMALL, NORMAL);

		nchoose = choose(1, 2);
			erase_screen();

		display_text(0, 10, "Input % deviation", 0, SMALL, NORMAL);

		pcdev = inpflt(16, 37, MEDIUM, 0, 5, 0.0, 30.0);
		if(nchoose == 2) pcdev = -pcdev;

			pcdev *= 0.01;
			ch_vals.resp_corr = pcdev;
		}
	}




    
//serial number entry
void sernumin(short mode){

		short i;
		char ch;
		char sn[8];
        char strng[20];
        CHAMBERVALS ch_vals;
        bool yn_ret; 

		erase_screen();

	switch(mode){
		case 0:
			strcpy(strng, "MAIN UNIT");
			EE_READ(snum, (uchar *)sn);
			break;
		case -1:
			strcpy(strng, "CHAMBER");
            //read data from chamber
			read_from_chamber(0, &ch_vals);
			strncpy(sn, &ch_vals.sn[0], 6);
			break;
        case 1: //Well
			strcpy(strng, "WELL");
			DB_ReadWellSerialNum(sn, Mca_installedDetector);
            break;
		}
        sn[6] = '\0';
	display_text(8, 2, strng, 0, MEDIUM, NORMAL);

	if(sn[0] != ' '){
		display_text(16, 22, "S/N:", 0, MEDIUM, NORMAL);
		display_text(56, 22, &sn[0], 0, MEDIUM, NORMAL);
	}else{
		display_text(16, 22, "NONE", 0, MEDIUM, NORMAL);
		strcpy(&sn[0], "      ");
		}

        display_text(16,52,"Correct?",0,MEDIUM,NORMAL);
	if(yesorno() || home_set()) return;

	for(;;){
		erase_lines(52, 12, 0);
		display_text(16, 22, "SERIAL #:  ", 0, MEDIUM, NORMAL);

		if(!numstr_in(24, 42, strng, MEDIUM, 0, 6, 6, TRUE, FALSE)){
			if(home_set()) return;
                beep();
                continue;
		}else{
			for(i=0; i<6; i++){
                    ch = strng[i];
				if(ch=='.' || ch=='/') ch = '-';
                    sn[i] = ch;
                }
            }

            display_text(16,52,"Correct?",0,MEDIUM,NORMAL);
            yn_ret = yesorno();
		if(home_set()) return;
		if(yn_ret) break;

        }
        
	switch(mode){
        case -1:
            strncpy(&ch_vals.sn[0],sn,6);
            //write the data to the chamber
            write_to_chamber(0, &ch_vals);
            break;
        case 0:     
        	EE_WRITE(snum, (uchar *)sn);
            break;
        case 1:
        	DB_WriteWellSerialNum(sn, Mca_installedDetector);
            break;
        }    

		erase_screen();
	}


    

static void chamber_ee_init(void){
	CHAMBERVALS chamb;
        
	chamb.chamb_type = R_CHAMB;
	//response correction = 0.
	chamb.resp_corr = 0.;
	//serial number 000000
	strncpy(chamb.sn, "000000", 6);
	//nominal voltage
	chamb.nomvolts = get_nomvolts(0);
	//init string
	strncpy(chamb.init_str, "Capintec", 8);

	strncpy(chamb.rev_num_str, "      ", 6);

	chamb.feature_flag1 = 0;
	chamb.feature_flag2 = 0;
	chamb.gainfactor0 = 0;
	chamb.gainfactor1 = 0;
	chamb.gainfactor2 = 0;
	chamb.future = 0;

	//write the data to the chamber
	write_to_chamber(0, &chamb);
}

void read_from_chamber(short ch_num, CHAMBERVALS *ch_vals){
	//bool was_enabled;
	ushort address, num_bytes;

	while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_ADC)) service_watchdog();

	//setup to read from EEPROM in specified chamber
	set_chamber_spi(ch_num, QSPI_EEPROM);
	address = 0;
	num_bytes = sizeof(*ch_vals);
	memcpy(ch_vals,uc_eeprom_read(address,num_bytes),num_bytes);
	if(strncmp(ch_vals->rev_num_str, rev_num_str1_0, 6) == 0){
		if(((ch_vals->feature_flag1) & STORED_GAIN_FACTOR) == 0){
			ch_vals->gainfactor0 = 0;
			ch_vals->gainfactor1 = 0;
			ch_vals->gainfactor2 = 0;
		}
	}else if(strncmp(ch_vals->rev_num_str, rev_num_str1_1, 6) == 0){
		if(((ch_vals->feature_flag1) & STORED_GAIN_FACTOR) == 0){
			ch_vals->gainfactor0 = 0;
			ch_vals->gainfactor1 = 0;
			ch_vals->gainfactor2 = 0;
		}
	}else{
		ch_vals->feature_flag1 = 0;
		ch_vals->feature_flag2 = 0;
		ch_vals->gainfactor0 = 0;
		ch_vals->gainfactor1 = 0;
		ch_vals->gainfactor2 = 0;
	}
	ch_vals->future = 0;

	while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_ADC)) service_watchdog();
}

void write_to_chamber(short ch_num, CHAMBERVALS *ch_vals){
	ushort address, num_bytes;

	while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_ADC)) service_watchdog();

	//setup to write to EEPROM
	set_chamber_spi(ch_num, QSPI_EEPROM);

	//write chamber data
	num_bytes = sizeof(*ch_vals);
	address = 0;
	uc_eeprom_write(address, (uchar *)ch_vals, num_bytes);

	while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_ADC)) service_watchdog();
}


static void sysvals(void){

        float min_val;
        float max_val;
        float val;
        char strng[40];
        short ch_type;
        CHAMBERVALS ch_vals;


        //read data from chamber
        read_from_chamber(0,&ch_vals);
        ch_type = ch_vals.chamb_type;

        erase_screen();
	for(;;){
            display_text(8,2,"SYSTEM TEST",0,MEDIUM,NORMAL);
            display_text(8,12,"VOLTAGE:",0,MEDIUM,NORMAL);
            sprintf(strng,"%#5.1f V",ch_vals.nomvolts);
            display_text(8,32,strng,0,MEDIUM,NORMAL);
            display_text(8,52,"CORRECT?",0,MEDIUM,NORMAL);
		if(yesorno()){
                //write the data to the chamber
                write_to_chamber(0, &ch_vals);
                return;
            }
            erase_screen();

            display_text(1,0,"INPUT VOLTS",0,MEDIUM,NORMAL);
            

            min_val = get_minvolts(ch_type);
            max_val = get_maxvolts(ch_type);
            val = inpflt(16,32,MEDIUM,0,5,min_val, max_val);
            ch_vals.nomvolts = val;

            erase_screen();
        }
    }
short chamber_type(short ch_num) {
	return chamb_vals[ch_num].chamb_type;
}

bool chamber_hv_adc(short ch_num){
	if (((chamb_vals[ch_num].feature_flag1) & HV_ADC_MASK) == 0) return FALSE;
	else return TRUE;
}

bool chamber_one_gain_relay(short ch_num){
	if (((chamb_vals[ch_num].feature_flag1) & ONE_GAIN_RELAY_MASK) == 0) return FALSE;
	else return TRUE;
}

bool chamber_stored_gain_factor(short ch_num){
	if (((chamb_vals[ch_num].feature_flag1) & STORED_GAIN_FACTOR) == 0) return FALSE;
	else return TRUE;
}

double chamber_gain_factor_0(short ch_num){
	return chamb_vals[ch_num].gainfactor0;
}

double chamber_gain_factor_1(short ch_num){
	return chamb_vals[ch_num].gainfactor1;
}

double chamber_gain_factor_2(short ch_num){
	return chamb_vals[ch_num].gainfactor2;
}

bool chamber_77t(void){
	bool returnValue;

	if(current.num_chambers>0){
		if((chamber_type(current.main_chamber) == C_CHAMB) || (chamber_type(current.main_chamber) == K_CHAMB)) returnValue = TRUE;
		else returnValue = FALSE;
	}else{
		returnValue = FALSE;
	}

	return returnValue;
}

bool chamber_C(void){
	bool returnValue;

	if(current.num_chambers>0){
		if(chamber_type(current.main_chamber) == C_CHAMB) returnValue = TRUE;
		else returnValue = FALSE;
	}else{
		returnValue = FALSE;
	}

	return returnValue;
}

bool chamber_K(void){
	bool returnValue;

	if(current.num_chambers>0){
		if(chamber_type(current.main_chamber) == K_CHAMB) returnValue = TRUE;
		else returnValue = FALSE;
	}else{
		returnValue = FALSE;
	}

	return returnValue;
}

float chamber_response_correction(short ch_num){
	return chamb_vals[ch_num].resp_corr;
}

float chamber_nominal_volts(short ch_num){
	return chamb_vals[ch_num].nomvolts;
}
bool chamber_mcp3550_chip(short ch_num){
	if (((chamb_vals[ch_num].feature_flag1) & ONE_GAIN_RELAY_MASK) == 0) return FALSE;
	else return TRUE;
}

short chamber_type_77t_remap(short ch_type){
	short returnValue;

	if(ch_type == C_CHAMB || ch_type == K_CHAMB) returnValue = R_CHAMB;
	else returnValue = ch_type;

	return returnValue;
}

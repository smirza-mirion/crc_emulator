/**
 * \file
 * \details This file contains the entry point for the program - main(). This file also contains functions, which initialize various software modules.
 */
/*********************************************************************

  MODULE:   Main for CRC-25R

  FILE:     CalibratorMain_R.c

    DATE:     03/25/08

  ANALYSIS:     Main Routine -- first C routine
          Calls routines to do hardware startup
          Calls routines to do chamber initialization
          Calls signon screen
          Calls Main Measurement screen
          is never returned to


  *************************************************************************/

#include "crc.h"
#include "coldfire.h"
#include "cs.h"
#include "screen.h"
#include "pit.h"
#include "keyboard.h"
#include "i2c.h"
#include "uart.h"
#include "qspi.h"
#include "daily.h"
#include "remote.h"
#include "printer.h"
#include "sl811h.h"
#include "lowlevelsnoop.h"
#include "uart.h"
#include "chambfac.h"
#include "mca.h"
#include "nuc.h"
#include "sqlite3.h"
#include "amulet.h"
//#include "mmcapi.h"
//#include "fat.h"
//#include "fat_m.h"
#include "ff.h"
#include "database.h"
#include "wipes.h"
#include "bioassay.h"
#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

void first_initialization(void);
void initialize_edge_port(void);
void handle_ex(void);
void startup_811(void);
bool chamber_one_gain_relay(short ch);
void verify_chamber_checksum(void);
static void set_interrupt_level(void);
static void initialize_chambers(void);
static void set_options(void);
static void setup_chambers(void);
static void setup_demo_chamber(short ch_num);
static void initialize_remote(void);
void set_touch_reset(bool on);
void set_touch_program(bool on);
void VolumeMirror(void);
void BrightnessMirror(void);
void smart_linearity(void);
void initialize_inventory(void);
void Sysset_clearStruct(void);
void *Sysset_getMirror(void);
long Sysset_getMirrorSize(void);
void Sysset_updateStruct(void);
void SleepBrightnessMirror(void);
void SleepTimeoutMirror(void);
void AmuletWellMainScreenMenu_wellStandardMirror(void);
void MoveFileName(char *fname, char *lfname);
void set_today_clock(void);
void ThyroidUptakeInitialize(void);
void RBCSurvivalInitialize(void);
void AmuletAutoLinearityTest_clearTest(void);
void AmuletAutoLinearityTest_matchChamberAndNuclide(int *resumeChamber, int *resumeNuclideID);
void AmuletChamberHalflifeCalc_clearTest(void);

static void initDisableInterrupts(void);
static void initSetExceptions(void);
static void initSetGPIO(void);
static void initSetQSPI(void);
static void initSetSDCard(void);
static void initAmuletHTML(void);
static void testSDCard(void);
static void initSetTimer(void);
static void initSetUART(int printerBaud, int pccommBaud, int amuletBaud);
static void initSetUSB(void);
static void initSetEPORT(void);
static void initSetInterrupt(void);
static void UCase(char *str);

__interrupt void irq5_interrupt(void);
__interrupt void printer_interrupt(void);
__interrupt void usb_interrupt(void);
__interrupt void amulet_interrupt(void);
__interrupt void timed_interrupt(void);
__interrupt void sec_interrupt(void);

void setup_watchdog(void);

extern char Amulet_amuletImage[];
extern char Amulet_amuletImage2[];
//extern unsigned long int g_ulTickCounter;
extern CHAMBER chamber[];
extern MEASUREMENT measurement[];
extern CURRENT current;
extern CHAMBERVALS chamb_vals[];
extern time_t clock_time,prev_clock_time;
typedef struct c1 COUNTER;
extern char cap_str[];
extern REMOTE remote[];
extern ushort chamber_checksum[];
extern short max_chambers;
extern INVENTORY  inventory[MAX_INVENTORY];
extern volatile short PCComm_currentPCCommChamber;
extern char AmuletInitialRecoveryMsg[100];
bool CalibratorMain_WelcomeScreen;
extern bool AmuletGenericYesNo_AccuracyTestPending;
extern AUTOLINEARITYTEST AmuletAutoLinearityTest_test;
extern int AmuletWellAutoCalibrate_exportSequence;

extern char lang_study_type[8][14];
void set_study_type(void);
void set_calicheck_tube(void);
void set_time_unit(void);

struct c1{
	float	act;			// calculated activity
	float	err;			// error
	float   err2;
	float   bcnts;          //background counts
	float   tbkg;           //total bkg counting rate
	float   totcpm;         //total cpm
	float   net;            //net cpm
	short   num_corr;       //number of container correction factors -- Beta
	char	nucnum;			// current nuclide number
	char	type;			// REF (no type), SYR or VIAL
	char	mode;			// measurement mode
	char	disp_type;		// text, ok, exceeds, graph
	char	actstr[11];		// formatted activity string
	char	errstr[11];     // error string
	char	sv_str[6];		// syringe / vial string
	bool    kflag;			// flag for kcpm or kcps
	WELLCFDATA  well_data;    //well data
	char    nuchan[6];     //unpacked channels
	float   chan[6];        //counts in each channel
	float   total_counts;   //all counts including erroneous ones
	float   cpm[6];         //counting rate in each channel
	bool    display_flag;   //data ready for display
	bool    enabled_flag;   //counting enabled flag
};
extern unsigned char m_ucHotKeyNuclideID[UPPER_LIMIT_CHAMB][8];
extern unsigned char m_ucHotKeyNuclideID2[UPPER_LIMIT_CHAMB][20];

static char encoded_map[] = {'X','a','0','b','A','c','1','d','B','e','Y','2','f','C','g','3','h','D','i','H','9','j','S','k','8','l','I','U','T','m','6','n','Z','o','F','5','G','L','7','p','4','q','N','r','s','O','t','K','u','Q','J','v','P','w','M','x','W','y','R','z','V','E'};

#ifdef MCA_SIMULATION
typedef struct bkg_meas BKG_MEAS;
struct bkg_meas{
	long spectrum[256];  // spectrum after bundling channels;
	float liveTime;
	float realTime;
	float cpm;
	short detector;	// DET_EMPTY or DET_WELL or DET_BETA
	time_t stamp;
};
#endif

//static void test_print(void);
/**
 * \details This is the entry point of the program.
 * \details It initializes the following Coldfire registers:
 * \details  1) Exception handling
 * \details  2) Interrupt handling
 * \details  3) Chip Select
 * \details  4) General Purpose Input Output
 * \details  5) I2C
 * \details  6) SPI
 * \details  7) UART
 * \details  8) EPORT
 * \details  9) Programmable Interrupt Timer
 * \details 10) Watchdog Timer
 * \details In addition it initializes the following modules:
 * \details 1) Real Time Clock
 * \details 2) SD Card
 * \details 3) FAT filesystem
 * \details 4) Database
 * \details 5) EEPROM
 * \details 6) Ionization Chamber
 * \details 7) USB
 * \details 8) Printer
 * \details 9) Amulet Screen
 * \details After initialization, the measurement_screen function is executed. The measurement_screen is a forever loop which executes the foreground processes.
 * \returns The program never exits the main function
 */
int main(void){
	int status;
	ulong ul100MSec;
	char initstr[10];
	//bool flgOn;
	//ulong ulOneSec;

	//watchdog timer automatically runs after power on with
	//~8 second timeout at 64MHz

	CalibratorMain_WelcomeScreen = TRUE;
	AmuletGenericYesNo_AccuracyTestPending = FALSE;
	AmuletInitialRecoveryMsg[0] = 0;

	// Clear Global Data Structures
	initSetExceptions();
	ClearLowLevel();
	Sysset_clearStruct();
	AmuletChamberHalflifeCalc_clearTest();

	// Disable All Interrupts
	initDisableInterrupts();

	// Set CS
	initSetCS();
	set_touch_reset(FALSE);

	//set_touch_reset(TRUE);
	//set_touch_program(TRUE);
	power_mmc(TRUE);
	//status = 0;
	//while(1){
	//	service_watchdog();
	//	printf("%d\n", status++);
	//}
	//asm("nop");
	//asm("halt");

	// Set GPIO
	initSetGPIO();

	// Set I2C
	initSetI2C();
	read_clock(&clock_time);
	low_clock_time = clock_time;
	reset_minute_counter_with_seconds();

	// Set QSPI
	initSetQSPI();

	//power_mmc(TRUE);
	// Initialize SD Card
	initSetSDCard();

	initAmuletHTML();

	// Database Initialization
	initDB(FALSE);
	WipesInitialize();
	BioAssayInitialize();
	ThyroidUptakeInitialize();
	RBCSurvivalInitialize();

	//// This will be replaced by database read
	// Read EEPROM for Global Data Structures
	first_initialization();
	NuclideData_initializeMirror();
	VolumeMirror();
	BrightnessMirror();
	SleepBrightnessMirror();
	SleepTimeoutMirror();

	AmuletWellMainScreenMenu_wellStandardMirror();

	//read in current printer
	EE_READ(print[0],(uchar *)&current.printer);
	//read values from EEPROM into RAM
	//read values from Database
	//EE_READ(usercal,(uchar *)user_cal);

	// Read SD Card for Global Data Structures
	set_options();
	id_init();

	// Setup Chamber
	initialize_chambers();
	setup_chambers();
	smart_linearity();
	initialize_remote();
	if(current.num_chambers != 0) set_adc_enabled(TRUE);
	else set_adc_enabled(FALSE);

	// Set PIT, Timer Interrupt
	initSetTimer();

	// Set UART
#ifndef TERMINAL
	initSetUART(4800, 9600, 115200);
#else // #ifndef TERMINAL
	initSetUART(4800, 115200, 115200);
	pf("Starting...\r\n");
#endif // #ifndef TERMINAL

	// Set EPORT
	initSetEPORT();

	// Setup Interrupt
	initSetInterrupt();

	// Setup RTC
	if(!setup_rtc()){
		asm("nop");
		asm("halt");
	}
	read_clock(&clock_time);
	low_clock_time = clock_time;
	set_today_clock();
	prev_clock_time = 0;
	//reset_minute_counter();
	reset_minute_counter_with_seconds();

	//initDB();
	//testSDCard();

	// Setup MCA
#ifdef MCA_SIMULATION
	if(MCA_SIMULATION){
		Mca_setup_demo();
	}else{
		status = Mca_setup();
	}
#else // MCA_SIMULATION
	status = Mca_setup();
#endif // MCA_SIMULATION

#ifdef CALIB_ONLY
	if(CALIB_ONLY) Mca_setup_none();
#endif // CALIB_ONLY

	if(Mca_installedDetector != DET_EMPTY) current.language = ENGLISH;

	// Set USB
	initSetUSB();

	//Initialize Printer
	printer_init();

	// Randomize Initial Auto Calibration Sequence Number
	srand((unsigned int) clock_time);
	AmuletWellAutoCalibrate_exportSequence = rand();

	// Set Watchdog
#ifdef WATCHDOG_OFF
	if(!WATCHDOG_OFF){
		setup_watchdog();
	}
#else // WATCHDOG_OFF
	setup_watchdog();
#endif // WATCHDOG_OFF

	//delay_msec(1000);
	set_touch_reset(TRUE);

#ifdef FORCE_FLASH
	FlashAmulet(Amulet_amuletImage);
#endif // FORCE_FLASH

	set_study_type();
	set_calicheck_tube();
	set_time_unit();
	
	initialize_screen();

	display_text(24,0,"Program",0,MEDIUM,NORMAL);
	display_text(24,20,"Power-up",0,MEDIUM,NORMAL);
	display_text(24,34,"Sequence",0,MEDIUM,NORMAL);

	
	measurement_screen();
	return 0;
}

// Masks all interrupts and set current interrupt level = 0
/**
 * \details Disables interrupt
 * \returns None
 */
static void initDisableInterrupts(void){
	__DI();
	cf.intc[0].imrh = 0xffffffff;
	cf.intc[0].imrl = 0xffffffff;
	cf.intc[1].imrh = 0xffffffff;
	cf.intc[1].imrl = 0xffffffff;
	__EI();
}

/**
 * \details Set default exception handler
 * \returns None
 */
static void initSetExceptions(void){
	int i;

	for(i=2; i<63; i++) vector_base.table[i]= (long)&handle_ex;
}

/**
 * \details Initialize Coldfire General Purpose Input Output registers
 * \returns None
 */
static void initSetGPIO(void){
	// Setup Ethernet EMDIO, EMDC
	// Setup Uart2 Rx, Tx
	// Setup I2C SDA, SCL
	cf.gpio.paspar = 0x0faf;

	// Setup Ethernet ETXCLK, ETXEN, ETXD0, ECOL, ERXCLK, ERXDV, ERXD0, ECRS, ETXD3, ETXD2, ETXD1, ETXER, ERXD3, ERXD2, ERXD1, ERXER
	cf.gpio.pehlpar = 0xc0;

	// Setup QSPI CS3, CS2, CS1, CS0, CLK, DIN, DOUT
	cf.gpio.pqspar = 0x7f;

	// Setup printer out as digital io and inputs
	cf.gpio.ptcpar = 0x00;
	cf.gpio.ddrtc = 0x00;

	// Setup Uart1 Rx, Tx
	// Setup Uart0 Rx, Tx
	cf.gpio.puapar = 0x0f;
}

/**
 * \details Initialize Coldfire SPI registers
 * \returns None
 */
static void initSetQSPI(void){
	spiClearSemaphore();
}

/**
 * \details Initialize SD Card and mounts filesystem
 * \returns None
 */
static void initSetSDCard(void){
/*	init_mmc_card(); */

	FATFS * fileSystemObject;
	fileSystemObject = malloc(sizeof(FATFS));
	f_mount(0, fileSystemObject);
}

static void initAmuletHTML(void){
	int i;

	for(i=0; i<AMULET_HTML_ARRAY_SIZE; i++) AmuletHTMLIndex[i] = 0xFF;
}

// board, 0 = Original chip, 1 = New board
void LoadAmuletHTML(int board){
	switch(board){
		case 0:
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x00
			AmuletHTMLIndex[INDEX_HTM] = 0x20;
			AmuletHTMLIndex[NUCLIDE2_HTM] = 0x4F;
			AmuletHTMLIndex[KEYPAD2_HTM] = 0x50;
			AmuletHTMLIndex[KEYBOARD2_HTM] = 0x51;
			AmuletHTMLIndex[SETTIME2_HTM] = 0x52;
			AmuletHTMLIndex[SETACTIVITY_HTM] = 0x53;
			AmuletHTMLIndex[MEASUREACTIVITY_HTM] = 0x54;
			AmuletHTMLIndex[INVENTORYADD_HTM] = 0x55;
			AmuletHTMLIndex[INVENTORYWITHDRAW_HTM] = 0x56;
			AmuletHTMLIndex[MAINSCREEN_HTM] = 0x57;
			AmuletHTMLIndex[GEOMETRYREPORT_HTM] = 0x58;
			AmuletHTMLIndex[SETSTUDY_HTM] = 0x59;
			AmuletHTMLIndex[ONESTRIP_HTM] = 0x5A;
			AmuletHTMLIndex[TWOSTRIP_HTM] = 0x5B;
			AmuletHTMLIndex[HMPAO_HTM] = 0x5C;
			AmuletHTMLIndex[MAG3_HTM] = 0x5D;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x10
			AmuletHTMLIndex[DAILY_HTM] = 0x5E;
			AmuletHTMLIndex[BACKGROUND_HTM] = 0x5F;
			AmuletHTMLIndex[CHAMBERVOLTS_HTM] = 0x60;
			AmuletHTMLIndex[GEOMETRY_HTM] = 0x61;
			AmuletHTMLIndex[ACCURACY_HTM] = 0x62;
			AmuletHTMLIndex[LINEARITY_HTM] = 0x63;
			AmuletHTMLIndex[QC_HTM] = 0x64;
			AmuletHTMLIndex[MOLY_HTM] = 0x65;
			AmuletHTMLIndex[INVENTORY_HTM] = 0x66;
			AmuletHTMLIndex[SETUP_HTM] = 0x67;
			AmuletHTMLIndex[INVENTORYDELETE_HTM] = 0x68;
			AmuletHTMLIndex[INVENTORYKIT_HTM] = 0x69;
			AmuletHTMLIndex[SETSTUDY2_HTM] = 0x6A;
			AmuletHTMLIndex[ENHANCED_HTM] = 0x6B;
			AmuletHTMLIndex[INFO_HTM] = 0x6C;
			AmuletHTMLIndex[OTHER_HTM] = 0x6D;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x20
			AmuletHTMLIndex[SETUPSOURCES_HTM] = 0x6E;
			AmuletHTMLIndex[SETUPMOLY_HTM] = 0x6F;
			AmuletHTMLIndex[SETUPNUCLIDE_HTM] = 0x70;
			AmuletHTMLIndex[SETUPLINEARITY_HTM] = 0x71;
			AmuletHTMLIndex[SETUPREMOTE_HTM] = 0x72;
			AmuletHTMLIndex[SETUPCALNUM_HTM] = 0x73;
			AmuletHTMLIndex[ERRORMSG_HTM] = 0x74;
			AmuletHTMLIndex[WARNINGMSG_HTM] = 0x75;
			AmuletHTMLIndex[NOTIFICATIONMSG_HTM] = 0x76;
			AmuletHTMLIndex[SETUPLINEARITYSTANDARD_HTM] = 0x77;
			AmuletHTMLIndex[SETUPLINEARITYLINEATOR_HTM] = 0x78;
			AmuletHTMLIndex[SETUPLINEARITYCALICHECK_HTM] = 0x79;
			AmuletHTMLIndex[DOSETABLE_HTM] = 0x7A;
			AmuletHTMLIndex[CHAMBER_HTM] = 0x7B;
			AmuletHTMLIndex[GRADIENT_HTM] = 0x7C;
			AmuletHTMLIndex[WELLMAINSCREEN_HTM] = 0x7D;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x30
			AmuletHTMLIndex[BETAMAINSCREEN_HTM] = 0x7E;
			AmuletHTMLIndex[WELLMEASUREMENT_HTM] = 0x7F;
			AmuletHTMLIndex[WELLAUTOCALIBRATE_HTM] = 0x80;
			AmuletHTMLIndex[WELLPEAKS_HTM] = 0x81;
			AmuletHTMLIndex[WELLADVANCEDSETUP_HTM] = 0x82;
			AmuletHTMLIndex[WELLSETUPNUCLIDE_HTM] = 0x83;
			AmuletHTMLIndex[WELLSETUPSEALED_HTM] = 0x84;
			AmuletHTMLIndex[WELLSETUPSEALED2_HTM] = 0x85;
			AmuletHTMLIndex[WELLSETUPTESTSOURCE_HTM] = 0x86;
			AmuletHTMLIndex[WELLSETUPTRIGGERLEVEL_HTM] = 0x87;
			AmuletHTMLIndex[WELLSETUPEFFICIENCIES_HTM] = 0x88;
			AmuletHTMLIndex[WELLEDITEFFICIENCIES_HTM] = 0x89;
			AmuletHTMLIndex[WELLMANUAL_HTM] = 0x8A;
			AmuletHTMLIndex[WELLMEASUREEFFICIENCY_HTM] = 0x8B;
			AmuletHTMLIndex[AUTOCONSTANCY_HTM] = 0x8C;
			AmuletHTMLIndex[SLEEP_HTM] = 0x8D;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x40
			AmuletHTMLIndex[FACTORY_HTM] = 0x8E;
			AmuletHTMLIndex[SETUPCHAMBER_HTM] = 0x8F;
			AmuletHTMLIndex[INVENTORYDELETEALL_HTM] = 0x90;
			AmuletHTMLIndex[WELLMDATEST_HTM] = 0x91;
			AmuletHTMLIndex[WELLMEASUREMENTS_HTM] = 0x92;
			AmuletHTMLIndex[WELLEDITFULLEFFICIENCY_HTM] = 0x93;
			AmuletHTMLIndex[SETUPCALIBSERIALNUM_HTM] = 0x94;
			AmuletHTMLIndex[WELLWIPEREPORT_HTM] = 0x95;
			AmuletHTMLIndex[WELLLABTESTS_HTM] = 0x96;
			AmuletHTMLIndex[WELLSCHILLING_HTM] = 0x97;
			AmuletHTMLIndex[WELLPLASMA_HTM] = 0x98;
			AmuletHTMLIndex[WELLRBC_HTM] = 0x99;
			AmuletHTMLIndex[WELLFACTORY_HTM] = 0x9A;
			AmuletHTMLIndex[WELLPLASMAANALYSIS_HTM] = 0x9B;
			AmuletHTMLIndex[WELLRBCANALYSIS_HTM] = 0x9C;
			AmuletHTMLIndex[WELLQATESTS_HTM] = 0x9D;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x50
			AmuletHTMLIndex[WELLSYSTEMTESTANALYSIS_HTM] = 0x9E;
			AmuletHTMLIndex[WELLCHITEST_HTM] = 0x9F;
			AmuletHTMLIndex[WELLSCHILLINGANALYSIS_HTM] = 0xA0;
			AmuletHTMLIndex[WELLCHANGEROI_HTM] = 0xA1;
			AmuletHTMLIndex[WELLGENERALANALYSIS_HTM] = 0xA2;
			AmuletHTMLIndex[SETUPRHOTKEYS_HTM] = 0xA3;
			AmuletHTMLIndex[SETUPPHOTKEYS_HTM] = 0xA4;
			AmuletHTMLIndex[REMOTE_HTM] = 0xA5;
			AmuletHTMLIndex[DELETELINEATORSETTINGS_HTM] = 0xA6;
			AmuletHTMLIndex[DELETECALICHECKSETTINGS_HTM] = 0xA7;
			AmuletHTMLIndex[SELECTLINEARITY_HTM] = 0xA8;
			AmuletHTMLIndex[LOWLEVEL_HTM] = 0xA9;
			AmuletHTMLIndex[WELLSTABILITYTEST_HTM] = 0xAA;
			AmuletHTMLIndex[SETUPCOMMUNICATIONS_HTM] = 0xAB;
			AmuletHTMLIndex[PCQC_HTM] = 0xAC;
			AmuletHTMLIndex[ETHERNET_HTM] = 0xAD;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x60
			AmuletHTMLIndex[WELLSETUPWIPES_HTM] = 0xAE;
			AmuletHTMLIndex[WELLSETUPTYPES_HTM] = 0xAF;
			AmuletHTMLIndex[WELLSETUPLOCATIONS_HTM] = 0xB0;
			AmuletHTMLIndex[WELLSETUPADDEDITLOCATION_HTM] = 0xB1;
			AmuletHTMLIndex[WELLSETUPDELETELOCATION_HTM] = 0xB2;
			AmuletHTMLIndex[WELLWIPELIST_HTM] = 0xB3;
			AmuletHTMLIndex[ERRORMSGLARGE_HTM] = 0xB4;
			AmuletHTMLIndex[WELLSPECTRUM_HTM] = 0xB5;
			AmuletHTMLIndex[WELLREPORTS_HTM] = 0xB6;
			AmuletHTMLIndex[WELLWIPESEARCH_HTM] = 0xB7;
			AmuletHTMLIndex[GENERICITEMS_HTM] = 0xB8;
			AmuletHTMLIndex[INACTIVATE_HTM] = 0xB9;
			AmuletHTMLIndex[WELLSYSTEMTESTSEARCH_HTM] = 0xBA;
			AmuletHTMLIndex[WELLMDASEARCH_HTM] = 0xBB;
			AmuletHTMLIndex[WELLMDAANALYSIS_HTM] = 0xBC;
			AmuletHTMLIndex[WELLCHISEARCH_HTM] = 0xBD;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x70
			AmuletHTMLIndex[WELLCHIANALYSIS_HTM] = 0xBE;
			AmuletHTMLIndex[TESTIDENT_HTM] = 0xBF;
			AmuletHTMLIndex[WELLSCHILLINGSEARCH_HTM] = 0xC0;
			AmuletHTMLIndex[WELLSCHILLINGREPORT_HTM] = 0xC1;
			AmuletHTMLIndex[WELLPLASMASEARCH_HTM] = 0xC2;
			AmuletHTMLIndex[WELLPLASMAREPORT_HTM] = 0xC3;
			AmuletHTMLIndex[WELLRBCSEARCH_HTM] = 0xC4;
			AmuletHTMLIndex[WELLRBCREPORT_HTM] = 0xC5;
			AmuletHTMLIndex[WELLAUTOCALSEARCH_HTM] = 0xC6;
			AmuletHTMLIndex[WELLAUTOCALREPORT_HTM] = 0xC7;
			AmuletHTMLIndex[WELLFACTORYDETECTORS_HTM] = 0xC8;
			AmuletHTMLIndex[WELLSETUPUSERNUCLIDES_HTM] = 0xC9;
			AmuletHTMLIndex[WELLADDEDITUSERNUCLIDES_HTM] = 0xCA;
			AmuletHTMLIndex[LOGIN_HTM] = 0xCB;
			AmuletHTMLIndex[SECURITY_HTM] = 0xCC;
			AmuletHTMLIndex[ADDEDITUSER_HTM] = 0xCD;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x80
			AmuletHTMLIndex[WELLSETUPBIOASSAY_HTM] = 0xCE;
			AmuletHTMLIndex[WELLSETUPBIOASSAYENTEREFFICIENCY_HTM] = 0xCF;
			AmuletHTMLIndex[WELLSETUPBIOASSAYMEASUREEFFICIENCY_HTM] = 0xD0;
			AmuletHTMLIndex[WELLBIOASSAY_HTM] = 0xD1;
			AmuletHTMLIndex[WELLBIOASSAYANALYSIS_HTM] = 0xD2;
			AmuletHTMLIndex[WELLBIOASSAYSEARCH_HTM] = 0xD3;
			AmuletHTMLIndex[WELLSETUPBIOASSAYENTERROI_HTM] = 0xD4;
			AmuletHTMLIndex[WELLSETUPTHYROIDUPTAKEPROTOCOL_HTM] = 0xD5;
			AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM] = 0xD6;
			AmuletHTMLIndex[WELLTHYROIDUPTAKE_HTM] = 0xD7;
			AmuletHTMLIndex[ADDEDITTHYROIDUPTAKETEST_HTM] = 0xD8;
			AmuletHTMLIndex[WELLTHYROIDUPTAKETEST_HTM] = 0xD9;
			AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM] = 0xDA;
			AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM] = 0xDB;
			AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM] = 0xDC;
			AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERNORMAL_HTM] = 0xDD;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x90
			AmuletHTMLIndex[DUMMY1_HTM] = 0xDE;
			AmuletHTMLIndex[DUMMY2_HTM] = 0xDF;
			AmuletHTMLIndex[WELLRBCSURVIVAL_HTM] = 0xE0;
			AmuletHTMLIndex[ADDEDITRBCSURVIVALTEST_HTM] = 0xE1;
			AmuletHTMLIndex[WELLRBCSURVIVALTEST_HTM] = 0xE2;
			AmuletHTMLIndex[WELLRBCSURVIVALMEASUREMENT_HTM] = 0xE3;
			AmuletHTMLIndex[WELLRBCSURVIVALENTERNORMAL_HTM] = 0xE4;
			AmuletHTMLIndex[SETTIME2EUR_HTM] = 0xE5;
			AmuletHTMLIndex[SETTIME2JAP_HTM] = 0xE6;
			AmuletHTMLIndex[AUTOLINEARITY_HTM] = 0xE7;
			AmuletHTMLIndex[AUTOLINEARITYTEST_HTM] = 0xE8;
			AmuletHTMLIndex[GENERICYESNO_HTM] = 0xE9;
			AmuletHTMLIndex[AUTOLINEARITYSEARCH_HTM] = 0xEA;
			AmuletHTMLIndex[CHAMBERREPORTS_HTM] = 0xEB;
			AmuletHTMLIndex[CHAMBERSEARCH_HTM] = 0xEC;
			AmuletHTMLIndex[CHAMBERDAILYTESTVIEW_HTM] = 0xED;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0xA0
			AmuletHTMLIndex[CHAMBERZEROTESTVIEW_HTM] = 0xEE;
			AmuletHTMLIndex[CHAMBERBACKGROUNDTESTVIEW_HTM] = 0xEF;
			AmuletHTMLIndex[CHAMBERVOLTAGETESTVIEW_HTM] = 0xF0;
			AmuletHTMLIndex[CHAMBERACCURACYTESTVIEW_HTM] = 0xF1;
			AmuletHTMLIndex[CHAMBERAUTOCONSTANCYTESTVIEW_HTM] = 0xF2;
			AmuletHTMLIndex[HALFLIFECALCULATOR_HTM] = 0xF3;
			AmuletHTMLIndex[SETUPKEY_HTM] = 0xF4;
			AmuletHTMLIndex[SETUPPASSWORD_HTM] = 0xF5;
			break;

		case 1:
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x00
			AmuletHTMLIndex[INDEX_HTM] = 0x21;
			AmuletHTMLIndex[NUCLIDE2_HTM] = 0x5B;
			AmuletHTMLIndex[KEYPAD2_HTM] = 0x52;
			AmuletHTMLIndex[KEYBOARD2_HTM] = 0x51;
			AmuletHTMLIndex[SETTIME2_HTM] = 0x66;
			AmuletHTMLIndex[SETACTIVITY_HTM] = 0x63;
			AmuletHTMLIndex[MEASUREACTIVITY_HTM] = 0x58;
			AmuletHTMLIndex[INVENTORYADD_HTM] = 0x4C;
			AmuletHTMLIndex[INVENTORYWITHDRAW_HTM] = 0x50;
			AmuletHTMLIndex[MAINSCREEN_HTM] = 0x57;
			AmuletHTMLIndex[GEOMETRYREPORT_HTM] = 0x45;
			AmuletHTMLIndex[SETSTUDY_HTM] = 0x64;
			AmuletHTMLIndex[ONESTRIP_HTM] = 0x5C;
			AmuletHTMLIndex[TWOSTRIP_HTM] = 0x7C;
			AmuletHTMLIndex[HMPAO_HTM] = 0x48;
			AmuletHTMLIndex[MAG3_HTM] = 0x56;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x10
			AmuletHTMLIndex[DAILY_HTM] = 0x37;
			AmuletHTMLIndex[BACKGROUND_HTM] = 0x2B;
			AmuletHTMLIndex[CHAMBERVOLTS_HTM] = 0x35;
			AmuletHTMLIndex[GEOMETRY_HTM] = 0x44;
			AmuletHTMLIndex[ACCURACY_HTM] = 0x22;
			AmuletHTMLIndex[LINEARITY_HTM] = 0x53;
			AmuletHTMLIndex[QC_HTM] = 0x5F;
			AmuletHTMLIndex[MOLY_HTM] = 0x59;
			AmuletHTMLIndex[INVENTORY_HTM] = 0x4B;
			AmuletHTMLIndex[SETUP_HTM] = 0x69;
			AmuletHTMLIndex[INVENTORYDELETE_HTM] = 0x4D;
			AmuletHTMLIndex[INVENTORYKIT_HTM] = 0x4F;
			AmuletHTMLIndex[SETSTUDY2_HTM] = 0x65;
			AmuletHTMLIndex[ENHANCED_HTM] = 0x3D;
			AmuletHTMLIndex[INFO_HTM] = 0x4A;
			AmuletHTMLIndex[OTHER_HTM] = 0x5D;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x20
			AmuletHTMLIndex[SETUPSOURCES_HTM] = 0x79;
			AmuletHTMLIndex[SETUPMOLY_HTM] = 0x73;
			AmuletHTMLIndex[SETUPNUCLIDE_HTM] = 0x74;
			AmuletHTMLIndex[SETUPLINEARITY_HTM] = 0x6F;
			AmuletHTMLIndex[SETUPREMOTE_HTM] = 0x77;
			AmuletHTMLIndex[SETUPCALNUM_HTM] = 0x6B;
			AmuletHTMLIndex[ERRORMSG_HTM] = 0x3E;
			AmuletHTMLIndex[WARNINGMSG_HTM] = 0x7D;
			AmuletHTMLIndex[NOTIFICATIONMSG_HTM] = 0x5A;
			AmuletHTMLIndex[SETUPLINEARITYSTANDARD_HTM] = 0x72;
			AmuletHTMLIndex[SETUPLINEARITYLINEATOR_HTM] = 0x71;
			AmuletHTMLIndex[SETUPLINEARITYCALICHECK_HTM] = 0x70;
			AmuletHTMLIndex[DOSETABLE_HTM] = 0x3A;
			AmuletHTMLIndex[CHAMBER_HTM] = 0x2D;
			AmuletHTMLIndex[GRADIENT_HTM] = 0x46;
			AmuletHTMLIndex[WELLMAINSCREEN_HTM] = 0x90;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x30
			AmuletHTMLIndex[BETAMAINSCREEN_HTM] = 0x2C;
			AmuletHTMLIndex[WELLMEASUREMENT_HTM] = 0x96;
			AmuletHTMLIndex[WELLAUTOCALIBRATE_HTM] = 0x80;
			AmuletHTMLIndex[WELLPEAKS_HTM] = 0x98;
			AmuletHTMLIndex[WELLADVANCEDSETUP_HTM] = 0x7F;
			AmuletHTMLIndex[WELLSETUPNUCLIDE_HTM] = 0xB3;
			AmuletHTMLIndex[WELLSETUPSEALED_HTM] = 0xB4;
			AmuletHTMLIndex[WELLSETUPSEALED2_HTM] = 0xB5;
			AmuletHTMLIndex[WELLSETUPTESTSOURCE_HTM] = 0xB6;
			AmuletHTMLIndex[WELLSETUPTRIGGERLEVEL_HTM] = 0xB8;
			AmuletHTMLIndex[WELLSETUPEFFICIENCIES_HTM] = 0xB1;
			AmuletHTMLIndex[WELLEDITEFFICIENCIES_HTM] = 0x8A;
			AmuletHTMLIndex[WELLMANUAL_HTM] = 0x91;
			AmuletHTMLIndex[WELLMEASUREEFFICIENCY_HTM] = 0x95;
			AmuletHTMLIndex[AUTOCONSTANCY_HTM] = 0x27;
			AmuletHTMLIndex[SLEEP_HTM] = 0x7A;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x40
			AmuletHTMLIndex[FACTORY_HTM] = 0x41;
			AmuletHTMLIndex[SETUPCHAMBER_HTM] = 0x6C;
			AmuletHTMLIndex[INVENTORYDELETEALL_HTM] = 0x4E;
			AmuletHTMLIndex[WELLMDATEST_HTM] = 0x94;
			AmuletHTMLIndex[WELLMEASUREMENTS_HTM] = 0x97;
			AmuletHTMLIndex[WELLEDITFULLEFFICIENCY_HTM] = 0x8B;
			AmuletHTMLIndex[SETUPCALIBSERIALNUM_HTM] = 0x6A;
			AmuletHTMLIndex[WELLWIPEREPORT_HTM] = 0xC7;
			AmuletHTMLIndex[WELLLABTESTS_HTM] = 0x8F;
			AmuletHTMLIndex[WELLSCHILLING_HTM] = 0xA7;
			AmuletHTMLIndex[WELLPLASMA_HTM] = 0x99;
			AmuletHTMLIndex[WELLRBC_HTM] = 0x9E;
			AmuletHTMLIndex[WELLFACTORY_HTM] = 0x8C;
			AmuletHTMLIndex[WELLPLASMAANALYSIS_HTM] = 0x9A;
			AmuletHTMLIndex[WELLRBCANALYSIS_HTM] = 0x9F;
			AmuletHTMLIndex[WELLQATESTS_HTM] = 0x9D;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x50
			AmuletHTMLIndex[WELLSYSTEMTESTANALYSIS_HTM] = 0xBE;
			AmuletHTMLIndex[WELLCHITEST_HTM] = 0x89;
			AmuletHTMLIndex[WELLSCHILLINGANALYSIS_HTM] = 0xA8;
			AmuletHTMLIndex[WELLCHANGEROI_HTM] = 0x86;
			AmuletHTMLIndex[WELLGENERALANALYSIS_HTM] = 0x8E;
			AmuletHTMLIndex[SETUPRHOTKEYS_HTM] = 0x78;
			AmuletHTMLIndex[SETUPPHOTKEYS_HTM] = 0x76;
			AmuletHTMLIndex[REMOTE_HTM] = 0x60;
			AmuletHTMLIndex[DELETELINEATORSETTINGS_HTM] = 0x39;
			AmuletHTMLIndex[DELETECALICHECKSETTINGS_HTM] = 0x38;
			AmuletHTMLIndex[SELECTLINEARITY_HTM] = 0x62;
			AmuletHTMLIndex[LOWLEVEL_HTM] = 0x55;
			AmuletHTMLIndex[WELLSTABILITYTEST_HTM] = 0xBD;
			AmuletHTMLIndex[SETUPCOMMUNICATIONS_HTM] = 0x6D;
			AmuletHTMLIndex[PCQC_HTM] = 0x5E;
			AmuletHTMLIndex[ETHERNET_HTM] = 0x40;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x60
			AmuletHTMLIndex[WELLSETUPWIPES_HTM] = 0xBB;
			AmuletHTMLIndex[WELLSETUPTYPES_HTM] = 0xB9;
			AmuletHTMLIndex[WELLSETUPLOCATIONS_HTM] = 0xB2;
			AmuletHTMLIndex[WELLSETUPADDEDITLOCATION_HTM] = 0xAB;
			AmuletHTMLIndex[WELLSETUPDELETELOCATION_HTM] = 0xB0;
			AmuletHTMLIndex[WELLWIPELIST_HTM] = 0xC6;
			AmuletHTMLIndex[ERRORMSGLARGE_HTM] = 0x3F;
			AmuletHTMLIndex[WELLSPECTRUM_HTM] = 0xBC;
			AmuletHTMLIndex[WELLREPORTS_HTM] = 0xA6;
			AmuletHTMLIndex[WELLWIPESEARCH_HTM] = 0xC8;
			AmuletHTMLIndex[GENERICITEMS_HTM] = 0x42;
			AmuletHTMLIndex[INACTIVATE_HTM] = 0x49;
			AmuletHTMLIndex[WELLSYSTEMTESTSEARCH_HTM] = 0xBF;
			AmuletHTMLIndex[WELLMDASEARCH_HTM] = 0x93;
			AmuletHTMLIndex[WELLMDAANALYSIS_HTM] = 0x92;
			AmuletHTMLIndex[WELLCHISEARCH_HTM] = 0x88;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x70
			AmuletHTMLIndex[WELLCHIANALYSIS_HTM] = 0x87;
			AmuletHTMLIndex[TESTIDENT_HTM] = 0x7B;
			AmuletHTMLIndex[WELLSCHILLINGSEARCH_HTM] = 0xAA;
			AmuletHTMLIndex[WELLSCHILLINGREPORT_HTM] = 0xA9;
			AmuletHTMLIndex[WELLPLASMASEARCH_HTM] = 0x9C;
			AmuletHTMLIndex[WELLPLASMAREPORT_HTM] = 0x9B;
			AmuletHTMLIndex[WELLRBCSEARCH_HTM] = 0xA1;
			AmuletHTMLIndex[WELLRBCREPORT_HTM] = 0xA0;
			AmuletHTMLIndex[WELLAUTOCALSEARCH_HTM] = 0x82;
			AmuletHTMLIndex[WELLAUTOCALREPORT_HTM] = 0x81;
			AmuletHTMLIndex[WELLFACTORYDETECTORS_HTM] = 0x8D;
			AmuletHTMLIndex[WELLSETUPUSERNUCLIDES_HTM] = 0xBA;
			AmuletHTMLIndex[WELLADDEDITUSERNUCLIDES_HTM] = 0x7E;
			AmuletHTMLIndex[LOGIN_HTM] = 0x54;
			AmuletHTMLIndex[SECURITY_HTM] = 0x61;
			AmuletHTMLIndex[ADDEDITUSER_HTM] = 0x26;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x80
			AmuletHTMLIndex[WELLSETUPBIOASSAY_HTM] = 0xAC;
			AmuletHTMLIndex[WELLSETUPBIOASSAYENTEREFFICIENCY_HTM] = 0xAD;
			AmuletHTMLIndex[WELLSETUPBIOASSAYMEASUREEFFICIENCY_HTM] = 0xAF;
			AmuletHTMLIndex[WELLBIOASSAY_HTM] = 0x83;
			AmuletHTMLIndex[WELLBIOASSAYANALYSIS_HTM] = 0x84;
			AmuletHTMLIndex[WELLBIOASSAYSEARCH_HTM] = 0x85;
			AmuletHTMLIndex[WELLSETUPBIOASSAYENTERROI_HTM] = 0xAE;
			AmuletHTMLIndex[WELLSETUPTHYROIDUPTAKEPROTOCOL_HTM] = 0xB7;
			AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM] = 0x24;
			AmuletHTMLIndex[WELLTHYROIDUPTAKE_HTM] = 0xC0;
			AmuletHTMLIndex[ADDEDITTHYROIDUPTAKETEST_HTM] = 0x25;
			AmuletHTMLIndex[WELLTHYROIDUPTAKETEST_HTM] = 0xC5;
			AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM] = 0xC1;
			AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM] = 0xC3;
			AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM] = 0xC4;
			AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERNORMAL_HTM] = 0xC2;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0x90
			AmuletHTMLIndex[DUMMY1_HTM] = 0x3B;
			AmuletHTMLIndex[DUMMY2_HTM] = 0x3C;
			AmuletHTMLIndex[WELLRBCSURVIVAL_HTM] = 0xA2;
			AmuletHTMLIndex[ADDEDITRBCSURVIVALTEST_HTM] = 0x23;
			AmuletHTMLIndex[WELLRBCSURVIVALTEST_HTM] = 0xA5;
			AmuletHTMLIndex[WELLRBCSURVIVALMEASUREMENT_HTM] = 0xA4;
			AmuletHTMLIndex[WELLRBCSURVIVALENTERNORMAL_HTM] = 0xA3;
			AmuletHTMLIndex[SETTIME2EUR_HTM] = 0x67;
			AmuletHTMLIndex[SETTIME2JAP_HTM] = 0x68;
			AmuletHTMLIndex[AUTOLINEARITY_HTM] = 0x28;
			AmuletHTMLIndex[AUTOLINEARITYTEST_HTM] = 0x2A;
			AmuletHTMLIndex[GENERICYESNO_HTM] = 0x43;
			AmuletHTMLIndex[AUTOLINEARITYSEARCH_HTM] = 0x29;
			AmuletHTMLIndex[CHAMBERREPORTS_HTM] = 0x32;
			AmuletHTMLIndex[CHAMBERSEARCH_HTM] = 0x33;
			AmuletHTMLIndex[CHAMBERDAILYTESTVIEW_HTM] = 0x31;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 0xA0
			AmuletHTMLIndex[CHAMBERZEROTESTVIEW_HTM] = 0x36;
			AmuletHTMLIndex[CHAMBERBACKGROUNDTESTVIEW_HTM] = 0x30;
			AmuletHTMLIndex[CHAMBERVOLTAGETESTVIEW_HTM] = 0x34;
			AmuletHTMLIndex[CHAMBERACCURACYTESTVIEW_HTM] = 0x2E;
			AmuletHTMLIndex[CHAMBERAUTOCONSTANCYTESTVIEW_HTM] = 0x2F;
			AmuletHTMLIndex[HALFLIFECALCULATOR_HTM] = 0x47;
			AmuletHTMLIndex[SETUPKEY_HTM] = 0x6E;
			AmuletHTMLIndex[SETUPPASSWORD_HTM] = 0x75;
			break;

		default:
			break;
	}
}

void testSDCard(void){
	/* F_FILE *fptr;
	short index, jndex;
	static char dummy[256];
	uchar *readptr, *testptr;
	int blocks = 10;
	long filelen;
	bool flgCheck;
	uchar bytevalue;

	//while(1);
	readptr = malloc(1024*256);

	testptr = readptr;
	bytevalue = 0;
	for(index=0; index<1024*256; index++){
		*testptr = bytevalue;
		testptr++;
		bytevalue++;
	}

	while(1){
		f_delete("\\test.txt");
		fptr = f_open("\\test.txt", "a+");
		for(jndex=0; jndex<blocks; jndex++){
			f_write(readptr, 1024*256, 1, fptr);
			//for(index=0; index<1024; index++) f_write(dummy, 256, 1, fptr);
			printf("Writing %d\n", jndex+1);
		}
		f_close(fptr);

		filelen = f_filelength("\\test.txt");
		if(filelen != blocks * 1024 * 256){
			printf("Incorrect filelength, current length: %ld\n", filelen);
		}else{
			fptr = f_open("\\test.txt", "r");
			for(index=0; index<blocks; index++){
				testptr = readptr;
				for(jndex=0; jndex<1024*256; jndex++) *testptr++ = 0;
				f_read(readptr, 10 * 1024 * 256, 1, fptr);
				flgCheck = TRUE;
				bytevalue = 0;
				testptr = readptr;
				for(jndex=0; jndex<1024*256; jndex++){
					if(*testptr != bytevalue){
						printf("Mismatch detected, block: %d, offset: %d (Expected = %u, Actual = %u)\n", index, jndex, bytevalue, *testptr);
						flgCheck = FALSE;
						break;
					}
					testptr++;
					bytevalue++;
				}
				if(flgCheck){
					printf("Verified Block: %d\n", index);
				}
			}
			f_close(fptr);
		}
	}

	while(1) service_watchdog(); */

	//char progname[20];
	//DETECTOR detectorBackup;
	//USER_KEYS user_keys;
	//long sector;

	//int index;
	//int sqlite_return_code;
	//sqlite3 *db = NULL;
	//sqlite3_stmt *statement = NULL;
	//char query[200];
	//char firstname[100];
	//char lastname[100];
	//int firstnameindex, lastnameindex;
	//CHAMBERVALS ch_vals;

	//unsigned char block[512];
	//char *ptrMalloc;
	//ptrMalloc = (char *) malloc(14000000);
	//for(index=0; index<10000000; index++) *ptrMalloc++ = 0;

	//sector = 2097152;
	//while(1){
		//delayloop(100);
		//temp_spiSetDataBlock((unsigned long *) block, 512 * sector++);
	//}

	/* f_delete("database.db");

	sqlite3_initialize();
	sqlite_return_code = sqlite3_open_v2("database.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	printf("SQLite Open Code: %d\n", sqlite_return_code);

	strcpy(query, "CREATE TABLE my_table(UID INTEGER PRIMARY KEY, FName TEXT, LName TEXT)");
	sqlite_return_code = sqlite3_prepare_v2(db, query, strlen(query) + 1, &statement, NULL);
	printf("SQLite Prepare Statement Code: %d\n", sqlite_return_code);

	sqlite_return_code = sqlite3_step(statement);

	if(sqlite_return_code == SQLITE_DONE){
		printf("Success!!!\n");
	}else{
		printf("Failed!!!\n");
	}

	sqlite_return_code = sqlite3_reset(statement);
	printf("SQLite Reset Code: %d\n", sqlite_return_code);

	sqlite_return_code = sqlite3_finalize(statement);
	printf("SQLite Finalize Code: %d\n", sqlite_return_code);

	sqlite3_finalize(statement);

	sqlite_return_code = sqlite3_close(db);
	printf("SQLite Close Code: %d\n", sqlite_return_code);

	sqlite_return_code = sqlite3_open_v2("database.db", &db, SQLITE_OPEN_READWRITE, NULL);
	printf("SQLite Open Code: %d\n", sqlite_return_code);

	strcpy(query, "INSERT INTO my_table(FName, LName) VALUES(:FName, :LName)");
	//strcpy(query, "INSERT INTO my_table (FName, LName) VALUES ('First Name', 'Last Name')");
	sqlite_return_code = sqlite3_prepare_v2(db, query, strlen(query) + 1, &statement, NULL);
	firstnameindex = sqlite3_bind_parameter_index(statement, ":FName");
	lastnameindex = sqlite3_bind_parameter_index(statement, ":LName");

	for(index=0; index<1000; index++){
		sprintf(firstname, "%d", index);
		sprintf(lastname, "%d", index);
		sqlite_return_code = sqlite3_bind_text(statement, firstnameindex, firstname, strlen(firstname), NULL);
		sqlite_return_code = sqlite3_bind_text(statement, lastnameindex, lastname, strlen(lastname), NULL);
		sqlite_return_code = sqlite3_step(statement);
		sqlite_return_code = sqlite3_reset(statement);
		sqlite_return_code = sqlite3_clear_bindings(statement);
	}

	sqlite3_finalize(statement);
	sqlite_return_code = sqlite3_close(db);
	printf("SQLite Close Code: %d\n", sqlite_return_code);

	sqlite3_shutdown(); */
}

#define PIT_PRESCALAR	0x3		// pre-scalar
#define CNTR			3999	// 1ms with 64MHz

#define PIT1_PRESCALER  10         //pre-scalar
#define CNTR1			31249      // 1 sec with 64MHz
/**
 * \details Initialize Coldfire Programmable Interrupt Timer registers
 * \returns None
 */
static void initSetTimer(void){
	char int_level = 3;         //PIT2 interrupt level
    char int_priority = 3;      //PIT2 priority level
    char sec_level = 7;			//PIT1 interrupt level
    char sec_priority = 7;		//PIT1 priority level

	//configure PIT to generate interrupt every 1ms
	//period = (2^(PRE + 1) * (CNTR + 1))/64000000
	cf.pit[2].pcsr = OVER_WRITE | CLEAR_PIF | ROLL_OVER | (PIT_PRESCALAR << 8);

	cf.pit[2].pmr = CNTR;

	//enable interupt & disable OVER WRITE
	cf.pit[2].pcsr = CLEAR_PIF | ROLL_OVER | INT_ENABLE | (PIT_PRESCALAR << 8);

	//configure PIT to generate interrupt every 1 sec
	//period = (2^(PRE + 1) * (CNTR + 1))/64000000
	cf.pit[1].pcsr = OVER_WRITE | CLEAR_PIF | ROLL_OVER | (PIT1_PRESCALER << 8);

	cf.pit[1].pmr = CNTR1;

	//enable interupt & disable OVER WRITE
	cf.pit[1].pcsr = CLEAR_PIF | ROLL_OVER | INT_ENABLE | (PIT1_PRESCALER << 8);

	cf.pit[2].pcsr |= ENABLE_PIT;
	cf.pit[1].pcsr |= ENABLE_PIT;
}

/**
 * \details Initialize Coldfire UART registers
 * \returns None
 */
static void initSetUART(int printerBaud, int pccommBaud, int amuletBaud){
	ushort baud;
	uchar baud_hi, baud_lo;

	// Uart0 - Printer
	cf.uarts[0].ucr = UART_UCR_RESET_TX;					// Reset Tx
	cf.uarts[0].ucr = UART_UCR_RESET_RX;					// Reset Rx
	cf.uarts[0].ir 	= 0;									// Disable all interrupts
	cf.uarts[0].ucr	= UART_UCR_RESET_MR;					// Point to UMR1
	cf.uarts[0].umr	= UART_UMR1_PM_NONE | UART_UMR1_BC_8;	// 8Bit No Parity
	cf.uarts[0].umr	= UART_UMR2_CM_NORMAL | UART_UMR2_STOP_BITS_1;	// Normal Mode, 1 Stop bit
	cf.uarts[0].usr = UART_UCSR_TIMER;						// Prescaled system clock
	baud = (ushort) (SYSCLK / (32 * printerBaud));
	baud_hi = (uchar) (baud >> 8);
	baud_lo = (uchar) (baud & 0xff);
	cf.uarts[0].dur = baud_hi;
	cf.uarts[0].dlr = baud_lo;
	cf.uarts[0].ucr = UART_UCR_TX_ENABLED;
	cf.uarts[0].ucr = UART_UCR_RX_ENABLED;

	// Uart1 - PC Communications
	cf.uarts[1].ucr = UART_UCR_RESET_TX;					// Reset Tx
	cf.uarts[1].ucr = UART_UCR_RESET_RX;					// Reset Rx
	cf.uarts[1].ir 	= 0;									// Disable all interrupts
	cf.uarts[1].ucr	= UART_UCR_RESET_MR;					// Point to UMR1
	cf.uarts[1].umr	= UART_UMR1_PM_NONE | UART_UMR1_BC_8;	// 8Bit No Parity
	cf.uarts[1].umr	= UART_UMR2_CM_NORMAL | UART_UMR2_STOP_BITS_1;	// Normal Mode, 1 Stop bit
	cf.uarts[1].usr = UART_UCSR_TIMER;						// Prescaled system clock
	baud = (ushort) (SYSCLK / (32 * pccommBaud));
	baud_hi = (uchar) (baud >> 8);
	baud_lo = (uchar) (baud & 0xff);
	cf.uarts[1].dur = baud_hi;
	cf.uarts[1].dlr = baud_lo;
	cf.uarts[1].ucr = UART_UCR_TX_ENABLED;
	cf.uarts[1].ucr = UART_UCR_RX_ENABLED;

	// Uart2 - Amulet Communications
	cf.uarts[2].ucr = UART_UCR_RESET_TX;					// Reset Tx
	cf.uarts[2].ucr = UART_UCR_RESET_RX;					// Reset Rx
	cf.uarts[2].ir	= 0;									// Disable all interrupts
	cf.uarts[2].iacr = 0;									// Set input enable control (OFF)
	cf.uarts[2].ucr = UART_UCR_RESET_MR;					// Point to UMR1
	cf.uarts[2].umr = UART_UMR1_PM_NONE | UART_UMR1_BC_8;	// 8Bit No Parity
	cf.uarts[2].umr = UART_UMR2_CM_NORMAL | UART_UMR2_STOP_BITS_1;	// Normal Mode, 1 Stop bit
	cf.uarts[2].usr = UART_UCSR_TIMER;
	baud = (ushort) (SYSCLK / (32 * amuletBaud));
	baud_hi = (uchar) (baud >> 8);
	baud_lo = (uchar) (baud & 0xff);
	cf.uarts[2].dur = baud_hi;
	cf.uarts[2].dlr = baud_lo;

	//cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
	//cf.uarts[2].ucr = UART_UCR_RX_ENABLED;
	//ClearUart1Rx();
	//ClearUart1Tx();
	//ClearUart2Rx();
	//ClearUart2Tx();
	//ClearMasterMessage();
	//ClearSlaveMessage();
	//ClearCmdQueue();
	//cf.uarts[2].ir	= 3;									// Enable interrupt
}

/**
 * \details Initialize Coldfire EPORT registers
 * \returns None
 */
static void initSetEPORT(void){
	cf.eport.eppar = 0;		//level sensitive
	cf.eport.epddr = 0;
	cf.eport.epier = 0xe0;	//IRQ5 & IRQ6 & IRQ7
}

#define INT_NUM_IRQ5 5
#define INT_NUM_IRQ6 6
#define INT_NUM_IRQ7 7
#define INT_NUM_UART2 15
#define INT_NUM_PIT2 57
#define INT_NUM_PIT1 56
/**
 * \details Initialize Coldfire Interrupt registers
 * \returns None
 */
static void initSetInterrupt(void){
	char pit_int_level = 3;		// PIT2 interrupt level
	char pit_int_priority = 3;	// PIT2 priority level
	char uart2_int_level = 4;	// UART2 interrupt level
	char uart2_int_priority = 3;// UART2 priority level
	char sec_level = 7;
	char sec_priority = 7;

	//set interrupt level and priority
	cf.intc[0].icrn[INT_NUM_UART2] = (uart2_int_level << 3) | uart2_int_priority;	// Set priority for UART2
	cf.intc[0].icrn[INT_NUM_PIT2] = (pit_int_level << 3) | pit_int_priority;		// Set priority for PIT
	cf.intc[0].icrn[INT_NUM_PIT1] = (sec_level << 3) | sec_priority;				// Set priority for SEC

	//set address for interrupt to interrupt routine
	vector_base.table[64 + INT_NUM_IRQ5] = (long)&irq5_interrupt;					// Low battery, usb pc connected interrupt, IRQ5
	vector_base.table[64 + INT_NUM_IRQ6] = (long)&printer_interrupt;				// Usb printer interrupt, IRQ 6
	vector_base.table[64 + INT_NUM_IRQ7] = (long)&usb_interrupt;					// Usb pc interrupt, IRQ 7
	vector_base.table[64 + INT_NUM_UART2] = (long)&amulet_interrupt;				// UART2 interrupt, IRQ 15
	vector_base.table[64 + INT_NUM_PIT2] = (long)&timed_interrupt;					// Timer interrupt, IRQ 57
	vector_base.table[64 + INT_NUM_PIT1] = (long)&sec_interrupt;					// Sec interrupt, IRQ 56

	cf.intc[0].imrl &= 0xffff7f5e;													// Unmask IRQ 5, 7, 15 and do not unmask IRQ6
	cf.intc[0].imrh &= 0xfcffffff;													// Unmask IRQ 56, 57

	// Clear UART Structures
	ClearUart1Rx();
	ClearUart1Tx();
	ClearUart2Rx();
	ClearUart2Tx();
	ClearMasterMessage();
	ClearSlaveMessage();
	ClearCmdQueue();

	// Enable UART
	cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
	cf.uarts[2].ucr = UART_UCR_RX_ENABLED;
	cf.uarts[2].ir = 3;
}

/**
 * \details Initialize Cypress SL811 USB registers
 * \returns None
 */
static void initSetUSB(void){
	if(usb_pc_connected())
	{
		if((current.pccomm != PC_COMM_NONE) && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER) && (current.pccomm != PC_COMM_USB_DEBUG_WELL) && (current.pccomm != PC_COMM_USB_UPDATE) && (current.pccomm != PC_COMM_USB_UPLOAD_FILE))
			current.pccomm = PC_COMM_USB;
	}
	else
	{
		if((current.pccomm != PC_COMM_NONE) && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER) && (current.pccomm != PC_COMM_USB_DEBUG_WELL) && (current.pccomm != PC_COMM_USB_UPDATE) && (current.pccomm != PC_COMM_USB_UPLOAD_FILE))
			current.pccomm = PC_COMM_RS232;
	}
	startup_811();
	InitializeSL811H ();
}

void handle_ex(void){
	asm("nop");
	asm("halt");
}

/**
 * \details Read ionization chamber EEPROM. The EEPROM contains the settings for the chamber.
 * \returns None
 */
static void initialize_chambers(void){
	CHAMBERVALS ch_val;
	short index;
	short index_tc; //nuclide index for Tc99m
	short index_f; //nuclide index for F18
	short index_cs; //nuclide index for Cs137
	short num;
	short num_nucs; //number of nuclides with data
	short ch_type;
	char nucname[8];
	KEYDEF rem_nuc;
	short nuc_index;

	current.main_chamber = 255;
	EE_READ(remote_nucs[0],(uchar *)&rem_nuc);
	num_nucs = NuclideData_getNumOfNucs();
	index_tc = NuclideData_getIndexFromName("Tc99m");
    index_f = NuclideData_getIndexFromName("F 18");
    index_cs = NuclideData_getIndexFromName("Cs137");

    PCComm_currentPCCommChamber = -1;
    num = 0;
    for(index=0; index<max_chambers; index++){
    	chamber[index].exists = FALSE;
    	chamber[index].connected_flag = FALSE;

   		read_from_chamber(index, &ch_val);
   		read_from_chamber(index, &chamb_vals[index]);
   		//chamb_vals[index].chamb_type = ONE_DOT_ZERO_CHAMB;
   		//chamb_vals[index].gainfactor0 = 133.0;
   		//chamb_vals[index].nomvolts = 155.0;
   		ch_type = chamber_type(index);

   		if(ch_type >= UPPER_LIMIT_CHAMB) chamb_vals[index].init_str[0] = 0;
   		else{
   			if (NuclideData_getResponse(NuclideData_getIndexFromName("Co60"), ch_type) == 0.0) chamb_vals[index].init_str[0] = 0;
   		}

    	if(!strncmp(chamb_vals[index].init_str, cap_str, 8) == 0) continue;

    	chamber[index].exists = TRUE;

    	//make 1st 1 main chamber
    	if(num==0){
    		PCComm_currentPCCommChamber = index;
    		current.main_chamber = index;
    		chamber[index].control = CONTROL_MAIN;
    	}

    	num++;

    	chamber[index].connected_flag = TRUE;

    	if((ch_type == C_CHAMB || ch_type == K_CHAMB) && (current.branding != 3)){
    		current.branding = 3;
    		DB_WriteBranding(current.branding);
    	}

    	chamber[index].calkey = FALSE;

    	if(!(chamber[index].control == CONTROL_MAIN || chamber[index].control == CONTROL_REMOTE))
    		chamber[index].control = CONTROL_REMOTE;

    	if(chamber[index].control == CONTROL_MAIN){
    		if(chamber[index].nuc_index < 0 || ((chamber[index].nuc_index >= num_nucs) && (chamber[index].nuc_index < (num_nucs + 20))) || (chamber[index].nuc_index >= ALLNUC)){
    			if(ch_type == R_CHAMB){
    				chamber[index].nuc_index = index_tc;
    			}else if(ch_type == C_CHAMB || ch_type == K_CHAMB){
    				chamber[index].nuc_index = index_cs;
    			}else{
    				chamber[index].nuc_index = index_f;
    			}
    		}
    	}else{
    		if(chamber[index].nuc_index < 0 || ((chamber[index].nuc_index >= num_nucs) && (chamber[index].nuc_index < (num_nucs + 20))) || (chamber[index].nuc_index >= ALLNUC)){
    			if(ch_type == C_CHAMB || ch_type == K_CHAMB) strcpy(nucname, &rem_nuc.keyiso[R_CHAMB].iso_name[0]);
    			else strcpy(nucname, &rem_nuc.keyiso[ch_type].iso_name[0]);
    			nuc_index = NuclideData_getIndexFromName(nucname);
    			chamber[index].nuc_index = nuc_index;
    		}
    	}

    	//make sure adc_zero is within range -- make 160 default
    	if (chamber_one_gain_relay(index)){
    		if((chamber[index].adc_zero>8000) || (chamber[index].adc_zero<-3200)) chamber[index].adc_zero = 1280;
    	}else{
    		if((chamber[index].adc_zero>1000) || (chamber[index].adc_zero<-400)) chamber[index].adc_zero = 160;
    	}
    }

    //save number of chambers
    current.num_chambers = num;

    if(current.num_chambers == 0){
    	if(current.demo_mode){
			num = 0;
			for(index=0; index<max_chambers; index++){
				setup_demo_chamber(index);
				ch_type = chamber_type(index);

				chamber[index].exists = TRUE;

				//make 1st 1 main chamber
				if(num==0){
					current.main_chamber = index;
					chamber[index].control = CONTROL_MAIN;
				}

				num++;

				chamber[index].connected_flag = TRUE;

				chamber[index].calkey = FALSE;

				if(!(chamber[index].control == CONTROL_MAIN || chamber[index].control == CONTROL_REMOTE))
					chamber[index].control = CONTROL_REMOTE;

				if(chamber[index].control == CONTROL_MAIN){
					if((chamber[index].nuc_index < 0) || ((chamber[index].nuc_index >= num_nucs) && (chamber[index].nuc_index < (num_nucs + 20))) || (chamber[index].nuc_index >= ALLNUC)){
						if(ch_type == R_CHAMB){
							chamber[index].nuc_index = index_tc;
						}else if(ch_type == C_CHAMB || ch_type == K_CHAMB){
							chamber[index].nuc_index = index_cs;
						}else{
							chamber[index].nuc_index = index_f;
						}
					}
				}else{
					if((chamber[index].nuc_index < 0) || ((chamber[index].nuc_index >= num_nucs) && (chamber[index].nuc_index < (num_nucs + 20))) || (chamber[index].nuc_index >= ALLNUC)){
						if(ch_type == C_CHAMB || ch_type == K_CHAMB) strcpy(nucname, &rem_nuc.keyiso[R_CHAMB].iso_name[0]);
						else strcpy(nucname, &rem_nuc.keyiso[ch_type].iso_name[0]);
						nuc_index = NuclideData_getIndexFromName(nucname);
						chamber[index].nuc_index = nuc_index;
					}
				}

				//make sure adc_zero is within range -- make 160 default
				if (chamber_one_gain_relay(index)){
					if((chamber[index].adc_zero>8000) || (chamber[index].adc_zero<-3200)) chamber[index].adc_zero = 1280;
				}else{
					if((chamber[index].adc_zero>1000) || (chamber[index].adc_zero<-400)) chamber[index].adc_zero = 160;
				}
			}
			//save number of chambers
			current.num_chambers = num;
    	}
    }else{
    	current.demo_mode = FALSE;
    }

    //make sure system OK
    //if(current.lockedbq){
    //	current.system = BQ;
    //	current.lock = TRUE;
    //}else{
    //	if(current.system > 1) current.system = CI;
    //	current.lock = FALSE;
    //}
    //EE_WRITE(syst,(uchar *) &current.system);
    //EE_WRITE(syslock,(uchar *)&current.lock);

    //set source key index to maximum so we start with 1st
    current.source_index = 4;
}

//assume chamber has a remote and initialize its status
static void initialize_remote(void){
	short index;
	KEYDEF rem_nuc;
	short ch_type;
	char nucname[8];
	short nuc_index;

	//get first remote nuclide
	EE_READ(remote_nucs[0],(uchar *)&rem_nuc);

	for(index=0; index<max_chambers; index++){
		if(!chamber[index].exists){
			remote[index].exists = FALSE;
			continue;
		}

		remote[index].exists = TRUE;
		remote[index].keys_active = FALSE;
		remote_heartbeat(index);

		if(current.main_chamber==index){
			remote[index].nuclist = REMOTE_NUCLIDE_OTHER;
		}else{
			ch_type = chamber_type(index);
			if(ch_type == C_CHAMB || ch_type == K_CHAMB) strcpy(nucname, rem_nuc.keyiso[R_CHAMB].iso_name);
			else strcpy(nucname, rem_nuc.keyiso[ch_type].iso_name);
			nuc_index = NuclideData_getIndexFromName(nucname);
			remote[index].nuclist = REMOTE_NUCLIDE_1;
			chamber[index].nuc_index = nuc_index;
		}

		remote[index].cmd = REMOTE_NO_CMD;
		remote[index].sub_cmd = REMOTE_NO_CMD;
	}
}

/* get options which are set on the mmc card
   files are in directory options  on mmc card */
/**
 * \details Read files in the options folder for additional settings
 * \returns None
 */
static void set_options(void){
	long int lFileSize;
	FIL fileObject;
	DIR dirObject;
	FILINFO fileInfo;
	UINT bytesRead, bytesWritten;
	char longFileName[100];
	char acHotKeyData[300];
	char acNucString[10];
	short i, j, k;
	char filename[20];
	int status;
	int filecount;
	char *extension;
	char extbuffer[8];
	char progname[25];
	//F_FILE *pFileHandle;
	//F_FIND find;
#ifdef MCA_SIMULATION
	BKG_MEAS *bkgptr;
#endif

	//f_chdir("\\");
	f_chdrive(0);
	f_chdir("/");
	f_chdir("options");

	current.detector = DETECTOR_NONE;
	current.pccomm = PC_COMM_RS232;

	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;

	//is usb connected to pc?
	//if(mmc_file_exists("lowcmb.opt") == 0) current.pccomm = PC_COMM_USB_DEBUG_CHAMBER;
	if(f_stat("lowcmb.opt", &fileInfo) == FR_OK) current.pccomm = PC_COMM_USB_DEBUG_CHAMBER;

	//if(mmc_file_exists("lowwell.opt") == 0) current.pccomm = PC_COMM_USB_DEBUG_WELL;
	if(f_stat("lowwell.opt", &fileInfo) == FR_OK) current.pccomm = PC_COMM_USB_DEBUG_WELL;

#ifdef LOW_LEVEL_CHAMBER
	if(LOW_LEVEL_CHAMBER) current.pccomm = PC_COMM_USB_DEBUG_CHAMBER;
#endif

#ifdef LOW_LEVEL_WELL
	if(LOW_LEVEL_WELL) current.pccomm = PC_COMM_USB_DEBUG_WELL;
#endif

	//in Demo Mode if demo.opt file exists
	//if(mmc_file_exists("demo.opt") == 0) current.demo_mode = TRUE;
	if(f_stat("demo.opt", &fileInfo) == FR_OK) current.demo_mode = TRUE;
	else{
#ifdef	DEMO
		if(DEMO) current.demo_mode = TRUE;
		else current.demo_mode = FALSE;
#else
		current.demo_mode = FALSE;
#endif
	}

#ifndef FORCE_FLASH
	//f_chdir("\\");
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

	//if(mmc_file_exists("hotkeys.txt") == 0){
	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	if(f_stat("hotkeys.txt", &fileInfo) != FR_OK){ // Create hotkeys.txt for backwards compatibility
		DB_PopulateDefaultHotkeys();

		acHotKeyData[0] = 0;
		for(j=0; j<2; j++){
			for(i=0; i<8; i++){
				sprintf(acNucString, "%u\n", m_ucHotKeyNuclideID[j][i]);
				strcat(acHotKeyData, acNucString);
			}
		}

		for(j=0; j<2; j++){
			for(i=0; i<20; i++){
				sprintf(acNucString, "%u\n", m_ucHotKeyNuclideID2[j][i]);
				strcat(acHotKeyData, acNucString);
			}
		}
		//pFileHandle = f_open("hotkeys.txt", "w");
		f_open(&fileObject, "hotkeys.txt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

		//f_write(acHotKeyData, strlen(acHotKeyData), 1, pFileHandle);
		f_write(&fileObject, acHotKeyData, strlen(acHotKeyData), &bytesWritten);

		//f_close(pFileHandle);
		f_close(&fileObject);
	}

	DB_ReadAllHotkey();

	/*NuclideData_clearEfficiencyMirror();
	//if(mmc_file_exists("usereff.bin") == 0){
	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	//if(f_stat("usereff.bin", &fileInfo) == FR_OK){
		//lFileSize = f_filelength("usereff.bin");
		lFileSize = fileInfo.fsize;

		if(lFileSize > 0){
			//pFileHandle = f_open("usereff.bin", "r");
			//f_open(&fileObject, "usereff.bin", FA_READ);

			//f_read(NuclideData_getEfficiencyMirror(), 1, lFileSize, pFileHandle);
			//f_read(&fileObject, NuclideData_getEfficiencyMirror(), lFileSize, &bytesRead);

			//f_close(pFileHandle);
			f_close(&fileObject);
		}
	}else{
		//pFileHandle = f_open("usereff.bin", "w");
		//f_open(&fileObject, "usereff.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

		//f_write((void *) NULL, 0, 1, pFileHandle);
		f_write(&fileObject, (void *) NULL, 0, &bytesWritten);

		//f_close(pFileHandle);
		f_close(&fileObject);
	}
	NuclideData_initializeEfficiencyMirror();*/
#endif

#ifdef HOME_CALIBRATION
	//if(mmc_file_exists("dect.dat") == 0){
	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	if(f_stat("dect.dat", &fileInfo) == FR_OK){

		//pFileHandle = f_open("dect.dat", "r");
		f_open(&fileObject, "dect.dat", FA_READ);

		//f_read(&detectorBackup, 1, sizeof(detectorBackup), pFileHandle);
		f_read(&fileObject, &detectorBackup, sizeof(detectorBackup), &bytesRead);

		//f_close(pFileHandle);
		f_close(&fileObject);

		EE_WRITE(detector, (uchar *) &detectorBackup);
	}
#endif

#ifndef FORCE_FLASH
	//if(mmc_file_exists("inventry.bin") == 0){
	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	if(f_stat("inventry.bin", &fileInfo) == FR_OK){

		//lFileSize = f_filelength("inventry.bin");
		lFileSize = fileInfo.fsize;

		//pFileHandle = f_open("inventry.bin", "r");
		f_open(&fileObject, "inventry.bin", FA_READ);

		//f_read((void *) &inventory, 1, lFileSize, pFileHandle);
		f_read(&fileObject, (void *) &inventory, lFileSize, &bytesRead);

		//f_close(pFileHandle);
		f_close(&fileObject);
	}else{
		initialize_inventory();

		//pFileHandle = f_open("inventry.bin", "w");
		f_open(&fileObject, "inventry.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

		//f_write((void *) &inventory, MAX_INVENTORY * sizeof(INVENTORY), 1, pFileHandle);
		f_write(&fileObject, (void *) &inventory, MAX_INVENTORY * sizeof(INVENTORY), &bytesWritten);

		//f_close(pFileHandle);
		f_close(&fileObject);
	}


	//if(mmc_file_exists("bkg.bin") == 0){
	/*fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	if(f_stat("bkg.bin", &fileInfo) == FR_OK){
		//pFileHandle = f_open("bkg.bin", "r");
		f_open(&fileObject, "bkg.bin", FA_READ);

		//f_read(Mca_getBackgroundMirror(), 1, Mca_getBackgroundMirrorSize(), pFileHandle);
		f_read(&fileObject, Mca_getBackgroundMirror(), Mca_getBackgroundMirrorSize(), &bytesRead);

		//f_close(pFileHandle);
		f_close(&fileObject);

#ifdef MCA_SIMULATION
		bkgptr = Mca_getBackgroundMirror();
		for(i=0; i<256; i++) bkgptr->spectrum[i] = 0;
		bkgptr->liveTime = 20;
		bkgptr->realTime = 20;
		bkgptr->cpm = 0;
		bkgptr->detector=DET_WELL;
		bkgptr->stamp = clock_time;
#endif
	}else{
		Mca_clearBackgroundMirror();

		//pFileHandle = f_open("bkg.bin", "w");
		f_open(&fileObject, "bkg.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

		//f_write(Mca_getBackgroundMirror(), Mca_getBackgroundMirrorSize(), 1, pFileHandle);
		f_write(&fileObject, Mca_getBackgroundMirror(), Mca_getBackgroundMirrorSize(), &bytesWritten);

		//f_close(pFileHandle);
		f_close(&fileObject);
	}*/

	//if(mmc_file_exists("sysset.bin") == 0){
	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	if(f_stat("sysset.bin", &fileInfo) == FR_OK){

		//pFileHandle = f_open("sysset.bin", "r");
		f_open(&fileObject, "sysset.bin", FA_READ);

		//f_read(Sysset_getMirror(), Sysset_getMirrorSize(), 1, pFileHandle);
		f_read(&fileObject, Sysset_getMirror(), Sysset_getMirrorSize(), &bytesRead);

		//f_close(pFileHandle);
		f_close(&fileObject);
	}else{
		//pFileHandle = f_open("sysset.bin", "w");
		f_open(&fileObject, "sysset.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

		//f_write(Sysset_getMirror(), Sysset_getMirrorSize(), 1, pFileHandle);
		f_write(&fileObject, Sysset_getMirror(), Sysset_getMirrorSize(), &bytesWritten);

		//f_close(pFileHandle);
		f_close(&fileObject);
	}
	Sysset_updateStruct();
#endif

	/*f_chdir("\\");
	if(f_chdir("cur") == F_NO_ERROR){
		strcpy(filename, "*.dat");
		status = f_findfirst(filename, &find);
		if(status == 0){
			filecount = 1;
			do{
				status = f_findnext(&find);
				if(!status) filecount++;
			}while(status==0);
		}else{
			filecount = 0;
		}

		if(filecount == 1){
			status = f_findfirst(filename, &find);
			if(status == 0){
				strcpy(Amulet_amuletImage, find.filename);
			}else{
				Amulet_amuletImage[0] = 0;
			}
		}else{
			Amulet_amuletImage[0] = 0;
		}
	}else{
		Amulet_amuletImage[0] = 0;
	}*/

	Amulet_amuletImage[0] = 0;
	if(f_opendir(&dirObject, "/cur") == FR_OK){
		f_readdir(&dirObject, NULL);
		filecount = 0;
		do{
			fileInfo.lfname = longFileName;
			fileInfo.lfsize = 100;
			longFileName[0] = 0;
			f_readdir(&dirObject, &fileInfo);
			MoveFileName(fileInfo.fname, longFileName);
			if(longFileName[0] != 0){
				if(strlen(longFileName)>4){
					extension = &(longFileName[strlen(longFileName) - 4]);
					strcpy(extbuffer, extension);
					UCase(extbuffer);
					if(strcmp(extbuffer, ".DAT") == 0){
						strcpy(Amulet_amuletImage, longFileName);
						filecount++;
					}
				}
			}
		}while(longFileName[0] != 0);

		if(filecount != 1){
			Amulet_amuletImage[0] = 0;
		}
	}else{
		Amulet_amuletImage[0] = 0;
	}

	Amulet_amuletImage2[0] = 0;
	if(f_opendir(&dirObject, "/cur2") == FR_OK){
		f_readdir(&dirObject, NULL);
		filecount = 0;
		do{
			fileInfo.lfname = longFileName;
			fileInfo.lfsize = 100;
			longFileName[0] = 0;
			f_readdir(&dirObject, &fileInfo);
			MoveFileName(fileInfo.fname, longFileName);
			if(longFileName[0] != 0){
				if(strlen(longFileName)>4){
					extension = &(longFileName[strlen(longFileName) - 4]);
					strcpy(extbuffer, extension);
					UCase(extbuffer);
					if(strcmp(extbuffer, ".DAT") == 0){
						strcpy(Amulet_amuletImage2, longFileName);
						filecount++;
					}
				}
			}
		}while(longFileName[0] != 0);

		if(filecount != 1){
			Amulet_amuletImage2[0] = 0;
		}
	}else{
		f_chdir("/");
		f_mkdir("CUR2");
		f_chdir("/");
		Amulet_amuletImage2[0] = 0;
	}

#ifdef FORCE_FLASH
	//f_chdir("\\");
	//f_chdir("images");
	f_chdir("/images");

	//if(mmc_file_exists("progname.txt")==0) f_delete("progname.txt");
	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	if(f_stat("progname.txt", &fileInfo) == FR_OK) f_unlink("progname.txt");

	//pFileHandle = f_open("progname.txt", "w");
	f_open(&fileObject, "progname.txt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

	strcpy(progname, "crc_25r.mem");

	//f_write(progname, strlen(progname), 1, pFileHandle);
	f_write(&fileObject, progname, strlen(progname), &bytesWritten);

	//f_close(pFileHandle);
	f_close(&fileObject);
#endif
}

/**
 * \details Initialize ionization chamber data structures
 * \returns None
 */
static void setup_chambers(void){
	short nuc_index;
	NUCDATA  nucdata;
	float fUserResponse;
	short ch_type;
	short index;

	for(index=0; index<max_chambers; index++){
		if(!chamber[index].exists) continue;

		chamber[index].active = TRUE;       //for compatibility with PET

		ch_type = chamber_type(index);

		measurement[index].over_flag_count = 0;

		nuc_index = chamber[index].nuc_index;

		NuclideData_getNuclide(nuc_index, &nucdata);

		fUserResponse = NuclideData_getUserResponse(nuc_index, ch_type);
		if(fUserResponse != 0.0) measurement[index].response = fUserResponse;
		else measurement[index].response = nucdata.response[ch_type];

		if(measurement[index].response == 0.0){
			if(ch_type == C_CHAMB || ch_type == K_CHAMB) chamber[index].nuc_index = NuclideData_getIndexFromName("Cs137");
			else chamber[index].nuc_index = NuclideData_getIndexFromName("Tc99m");
			nuc_index = chamber[index].nuc_index;
			NuclideData_getNuclide(nuc_index, &nucdata);
			fUserResponse = NuclideData_getUserResponse(nuc_index, ch_type);
			if(fUserResponse != 0.0) measurement[index].response = fUserResponse;
			else measurement[index].response = nucdata.response[ch_type];
		}

		measurement[index].resp0 = measurement[index].response;
		chamber[index].calkey = FALSE;
		chamber[index].nucdata = nucdata;

		//force gain setting
		measurement[index].gain = -1;
		nucset(index);
		lowresp(index);
		set_adc_mode(index, ADC_READ_VOLTS);
	}
}

/*static void set_interrupt_level(void){
	//set Interrupt level to 0
	asm("move.w %sr,%d0");
	asm("and.l #0xb01f,%d0");
	asm("move.w %d0,%sr");
}*/

//edge port is used for interrupt input
/**
 * \details Initialize the Coldfire EPORT registers
 * \returns None
 */
void initialize_edge_port(void){
	cf.eport.eppar = 0;		//level sensitive
	cf.eport.epddr = 0;
	cf.eport.epier = 0xe0;	//IRQ5 & IRQ6 & IRQ7
}

static void setup_demo_chamber(short ch_num){
	char sn[7];
	char ch_num_str[7];

	if(ch_num==0){
		chamb_vals[ch_num].chamb_type = R_CHAMB;
		//strcpy(sn, "030110");
		strcpy(sn, "000111");
		//sprintf(ch_num_str, "%d", ch_num);
		//sn[5] = ch_num_str[0];
		strncpy(chamb_vals[ch_num].sn, sn, 6);
		chamb_vals[ch_num].resp_corr = 0.;
		chamb_vals[ch_num].nomvolts = 155.;
		strncpy(chamb_vals[ch_num].init_str, "Capintec", 8);
		strncpy(chamb_vals[ch_num].rev_num_str, "Rev1.1", 6);
		chamb_vals[ch_num].feature_flag1 = 7;
		chamb_vals[ch_num].feature_flag2 = 0;
#ifndef TERMINAL
		chamb_vals[ch_num].gainfactor0 = 10000;
#else // #ifndef TERMINAL
		chamb_vals[ch_num].gainfactor0 = 1000.;
#endif // #ifndef TERMINAL
		chamb_vals[ch_num].gainfactor1 = 0;
		chamb_vals[ch_num].gainfactor2 = 0;
		chamb_vals[ch_num].future = 0;
		chamber[ch_num].adc_zero = 0;
		chamber[ch_num].zero = 0;
	}else{
		chamb_vals[ch_num].chamb_type = P_CHAMB;
		//strcpy(sn, "000111");
		strcpy(sn, "030110");
		//sprintf(ch_num_str, "%d", ch_num);
		//sn[5] = ch_num_str[0];
		strncpy(chamb_vals[ch_num].sn, sn, 6);
		chamb_vals[ch_num].resp_corr = 0.;
		chamb_vals[ch_num].nomvolts = 450.;
		strncpy(chamb_vals[ch_num].init_str, "Capintec", 8);
		strncpy(chamb_vals[ch_num].rev_num_str, "Rev1.1", 6);
#ifndef TERMINAL
		//chamb_vals[ch_num].feature_flag1 = 7;
		chamb_vals[ch_num].feature_flag1 = 0;
#else // #ifndef TERMINAL
		chamb_vals[ch_num].feature_flag1 = 7;
#endif // #ifndef TERMINAL
		chamb_vals[ch_num].feature_flag2 = 0;
#ifndef TERMINAL
		chamb_vals[ch_num].gainfactor0 = 10000;
#else // #ifndef TERMINAL
		chamb_vals[ch_num].gainfactor0 = 1000.;
#endif // #ifndef TERMINAL
		chamb_vals[ch_num].gainfactor1 = 0;
		chamb_vals[ch_num].gainfactor2 = 0;
		chamb_vals[ch_num].future = 0;
		chamber[ch_num].adc_zero = 0;
		chamber[ch_num].zero = 0;
	}
}

ushort calc_chamber_checksum(short ch_num){
	char ch;
	ushort csum;
	uchar *addr;
	short i;

	addr = (uchar *)&chamber[ch_num].adc_zero;

	csum = 0;
	for(i=0; i<10; i++){
		ch = *addr++;
		csum += ch;
	}
	return(csum);
}

void verify_chamber_checksum(void){
	ushort csum;
	char badch[18];
	bool allok = TRUE;
	short index;
	char ch;

	strcpy(badch,"               ");

	for(index=0; index<max_chambers; index++){
		if(!chamber[index].exists) continue;

		csum = calc_chamber_checksum(index);

		if(csum != chamber_checksum[index]){
			ch = (char) index + 0x31;
			strncpy(&badch[2 * index], &ch, 1);
			allok = FALSE;
		}
	}

	if(allok) return;

	beep();
	erase_screen();
	display_text(8,12,"Chamber Data",0,MEDIUM,NORMAL);
	display_text(8,30,"Error",0,MEDIUM,NORMAL);
	contmsg();
	erase_screen();
	display_text(8,2,"Must Perform",0,MEDIUM,NORMAL);
	display_text(8,14,"Daily Test",0,MEDIUM,NORMAL);
	display_text(8,26,"For Chambers",0,MEDIUM,NORMAL);
	display_text(0,38,badch,0,MEDIUM,NORMAL);
	contmsg();
}

void setup_watchdog(void){
	//set watchdog timer to 1 second timeout
	//cf.wtm.wmr = 7800;
	cf.wtm.wmr = 15600;     //2 sec
	cf.wtm.wcr = 1;
}

void setup_short_watchdog(void){
	cf.wtm.wmr = 10;
	cf.wtm.wcr = 1;
}

void stop_watchdog(void){
	cf.wtm.wcr = 2;
}

short get_crc_mode(void){
	return(CRC_CAL);
}

short get_counter_user_key(short but){
	return 0;
}

void container_correction_factors(void){
}

void smart_linearity(void){
	bool swaplineator, swapcalicheck, look;
	LINDEF mirror;
	char cur_chamb_num[7];
	short index;
	short temp_num;
	short temp_index;
	char temp_chamb_num[7];
	char temp_serial_num[11];
	float response, temp_factors[12];
	FIL fileObject;
	FILINFO fileInfo;
	char longFileName[100];
	UINT bytesRead;
	int resumeChamber, resumeNuclideID;
	bool flgFound;
	EE_READ(lindef, (uchar *)&mirror);
	swaplineator = FALSE;
	swapcalicheck = FALSE;

	if(chamber[0].exists){
		for(index=0; index<6; index++){
			cur_chamb_num[index] = chamb_vals[0].sn[index];
		}
		cur_chamb_num[6] = 0;

		look = FALSE;
		if(mirror.num_Lin[0] == -1) look = TRUE;
		else{
			if(strcmp(cur_chamb_num, &(mirror.chamb_num_Lin[0][0])) != 0) look = TRUE;
		}

		if(look){
			if((mirror.num_Lin[1] != -1) && (strcmp(cur_chamb_num, &(mirror.chamb_num_Lin[1][0])) == 0)) swaplineator = TRUE;
		}

		look = FALSE;
		if(mirror.num_Cali[0] == -1) look = TRUE;
		else{
			if(strcmp(cur_chamb_num, &(mirror.chamb_num_Cali[0][0])) !=0) look = TRUE;
		}

		if(look){
			if((mirror.num_Cali[1] != -1) && (strcmp(cur_chamb_num, &(mirror.chamb_num_Cali[1][0])) == 0)) swapcalicheck = TRUE;
		}
	}

	if(chamber[1].exists){
		for(index=0; index<6; index++){
			cur_chamb_num[index] = chamb_vals[1].sn[index];
		}
		cur_chamb_num[6] = 0;

		if(!swaplineator){
			look = FALSE;
			if(mirror.num_Lin[1] == -1) look = TRUE;
			else{
				if(strcmp(cur_chamb_num, &(mirror.chamb_num_Lin[1][0])) != 0) look = TRUE;
			}

			if(look){
				if((mirror.num_Lin[0] != -1) && (strcmp(cur_chamb_num, &(mirror.chamb_num_Lin[0][0])) == 0)) swaplineator = TRUE;
			}
		}

		if(!swapcalicheck){
			look = FALSE;
			if(mirror.num_Cali[1] == -1) look = TRUE;
			else{
				if(strcmp(cur_chamb_num, &(mirror.chamb_num_Lin[1][0])) != 0) look = TRUE;
			}

			if(look){
				if((mirror.num_Cali[0] != -1) && (strcmp(cur_chamb_num, &(mirror.chamb_num_Cali[0][0])) == 0)) swapcalicheck = TRUE;
			}
		}
	}

	if(swaplineator){
		temp_num = mirror.num_Lin[0];
		mirror.num_Lin[0] = mirror.num_Lin[1];
		mirror.num_Lin[1] = temp_num;

		temp_index = mirror.nuc_index_Lin[0];
		mirror.nuc_index_Lin[0] = mirror.nuc_index_Lin[1];
		mirror.nuc_index_Lin[1] = temp_index;

		for(index=0; index<7; index++) temp_chamb_num[index] = mirror.chamb_num_Lin[0][index];
		for(index=0; index<7; index++) mirror.chamb_num_Lin[0][index] = mirror.chamb_num_Lin[1][index];
		for(index=0; index<7; index++) mirror.chamb_num_Lin[1][index] = temp_chamb_num[index];

		for(index=0; index<11; index++) temp_serial_num[index] = mirror.serial_num_Lin[0][index];
		for(index=0; index<11; index++) mirror.serial_num_Lin[0][index] = mirror.serial_num_Lin[1][index];
		for(index=0; index<11; index++) mirror.serial_num_Lin[1][index] = temp_serial_num[index];

		for(index=0; index<8; index++) temp_factors[index] = mirror.factors_Lin[0][index];
		for(index=0; index<8; index++) mirror.factors_Lin[0][index] = mirror.factors_Lin[1][index];
		for(index=0; index<8; index++) mirror.factors_Lin[1][index] = temp_factors[index];
	}

	if(swapcalicheck){
		temp_num = mirror.num_Cali[0];
		mirror.num_Cali[0] = mirror.num_Cali[1];
		mirror.num_Cali[1] = temp_num;

		temp_index = mirror.nuc_index_Cali[0];
		mirror.nuc_index_Cali[0] = mirror.nuc_index_Cali[1];
		mirror.nuc_index_Cali[1] = temp_index;

		for(index=0;index<7;index++) temp_chamb_num[index] = mirror.chamb_num_Cali[0][index];
		for(index=0;index<7;index++) mirror.chamb_num_Cali[0][index] = mirror.chamb_num_Cali[1][index];
		for(index=0;index<7;index++) mirror.chamb_num_Cali[1][index] = temp_chamb_num[index];

		for(index=0;index<11;index++) temp_serial_num[index] = mirror.serial_num_Cali[0][index];
		for(index=0;index<11;index++) mirror.serial_num_Cali[0][index] = mirror.serial_num_Cali[1][index];
		for(index=0;index<11;index++) mirror.serial_num_Cali[1][index] = temp_serial_num[index];

		for(index=0;index<12;index++) temp_factors[index] = mirror.factors_Cali[0][index];
		for(index=0;index<12;index++) mirror.factors_Cali[0][index] = mirror.factors_Cali[1][index];
		for(index=0;index<12;index++) mirror.factors_Cali[1][index] = temp_factors[index];
	}

	if(swaplineator || swapcalicheck){
		EE_WRITE(lindef, (uchar *) &mirror);
	}

	// Setup AutoLinearity
	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	f_chdrive(0);
	f_chdir("/");
	f_chdir("data");
	flgFound = FALSE;
	if(f_stat("ald.dat", &fileInfo) == FR_OK){
		flgFound = TRUE;
		if(f_stat("ald.tmp", &fileInfo) == FR_OK) f_unlink("ald.tmp");
	}else{
		if(f_stat("ald.tmp", &fileInfo) == FR_OK){
			f_rename("ald.tmp", "ald.dat");
			flgFound = TRUE;
		}
	}

	if(flgFound){
		if(f_stat("ald.dat", &fileInfo) == FR_OK){
			f_open(&fileObject, "ald.dat", FA_READ);
			f_read(&fileObject, (void *) &AmuletAutoLinearityTest_test, sizeof(AUTOLINEARITYTEST), &bytesRead);
			f_close(&fileObject);

			if(bytesRead == sizeof(AUTOLINEARITYTEST)){
				if(AmuletAutoLinearityTest_test.AutoLinearityTestID == -2){
					// Test Chamber S/N and NuclideID
					AmuletAutoLinearityTest_matchChamberAndNuclide(&resumeChamber, &resumeNuclideID);
					if((resumeChamber == -1) || (resumeNuclideID == -1)) AmuletAutoLinearityTest_test.AutoLinearityTestID = -3;		// Set to AutoLinearity Paused, if unable to find chamber or nuclide
					else AmuletAutoLinearityTest_test.AutoLinearityTestID = -4;														// Set to Autoresume, if found both chamber and nuclide
				}
			}else{
				AmuletAutoLinearityTest_clearTest();
			}
		}else{
			AmuletAutoLinearityTest_clearTest();
		}
	}else{
		AmuletAutoLinearityTest_clearTest();
	}
}

/**
 * \details Copies Null terminated string from fname to lfname, if lfname is a zero length Null terminated string
 * \param fname Source string
 * \param lfname Destination of the target string
 * \returns None
 */
void MoveFileName(char *fname, char *lfname){
 	if(*lfname == 0){
 		strcpy(lfname, fname);
 	}
}

static char lang_study_type[8][14];
void set_study_type(void)
{

	get_amulet_message(L_BONE,&lang_study_type[0][0]);    // "Bone"
	get_amulet_message(L_LUNG,&lang_study_type[1][0]);    // "Lung"
	get_amulet_message(L_HIDA,&lang_study_type[2][0]);    // "Hida"
	get_amulet_message(L_HEART,&lang_study_type[3][0]);    // "Heart"
	get_amulet_message(L_RENAL,&lang_study_type[4][0]);    // "Renal"
	get_amulet_message(L_LIVER,&lang_study_type[5][0]);    // "Liver"
	get_amulet_message(L_BRAIN,&lang_study_type[6][0]);    // "Brain"
	get_amulet_message(L_LYMPH,&lang_study_type[7][0]);    // "Lymph"

}

void get_study_type(short istudy, char *study)
{
	strcpy(study,&lang_study_type[istudy][0]);
}	

static char lang_calicheck_tube[12][24];
void set_calicheck_tube(void)
{
	get_amulet_message(L_PR_TUBE_1_CALICHECK,&lang_calicheck_tube[0][0]);    // "Black"
	get_amulet_message(L_PR_TUBE_2_CALICHECK,&lang_calicheck_tube[1][0]);    // "Black + Red"
	get_amulet_message(L_PR_TUBE_3_CALICHECK,&lang_calicheck_tube[2][0]);    // "Black + Orange"
	get_amulet_message(L_PR_TUBE_4_CALICHECK,&lang_calicheck_tube[3][0]);    // "Black + Yellow"
	get_amulet_message(L_PR_TUBE_5_CALICHECK,&lang_calicheck_tube[4][0]);    // "Black + Green"
	get_amulet_message(L_PR_TUBE_6_CALICHECK,&lang_calicheck_tube[5][0]);    // "Black + Blue"
	get_amulet_message(L_PR_TUBE_7_CALICHECK,&lang_calicheck_tube[6][0]);    // "Black + Purple"
	get_amulet_message(L_PR_TUBE_8_CALICHECK,&lang_calicheck_tube[7][0]);    // "Black/Purple/Red"
	get_amulet_message(L_PR_TUBE_9_CALICHECK,&lang_calicheck_tube[8][0]);    // "Black/Purple/Orange"
	get_amulet_message(L_PR_TUBE_10_CALICHECK,&lang_calicheck_tube[9][0]);    // "Black/Purple/Yellow"
	get_amulet_message(L_PR_TUBE_11_CALICHECK,&lang_calicheck_tube[10][0]);    // "Black/Purple/Green"
	get_amulet_message(L_PR_TUBE_12_CALICHECK,&lang_calicheck_tube[11][0]);    // "Black/Purple/Blue"
}

void get_calicheck_tube(short itube, char *tube)
{
	strcpy(tube,&lang_calicheck_tube[itube][0]);
}	

static char lang_time_unit[5][3];
void set_time_unit(void)
{
	get_amulet_message(L_PR_TIME_UNIT_1,&lang_time_unit[0][0]);    // "M"
	get_amulet_message(L_PR_TIME_UNIT_2,&lang_time_unit[1][0]);    // "H"
	get_amulet_message(L_PR_TIME_UNIT_3,&lang_time_unit[2][0]);    // "D"
	get_amulet_message(L_PR_TIME_UNIT_4,&lang_time_unit[3][0]);    // "Y"
	get_amulet_message(L_PR_TIME_UNIT_5,&lang_time_unit[4][0]);    // "MY"
}

void get_time_unit(short itime, char *unit)
{
	strcpy(unit,&lang_time_unit[itime][0]);
}	

void read_hotkey_txt(void){
	FILINFO fileInfo;
	char longFileName[100];
	long int lFileSize;
	FIL fileObject;
	char acHotKeyData[300];
	UINT bytesRead;
	short i, j, k;
	char acNucString[10];

	f_chdir("/");

	if(f_chdir("data") != FR_OK){
		f_mkdir("data");

		f_chdir("/");

		f_chdir("data");
	}

	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;

	if(f_stat("hotkeys.txt", &fileInfo) == FR_OK){
		lFileSize = fileInfo.fsize;
		f_open(&fileObject, "hotkeys.txt", FA_READ);
		f_read(&fileObject, acHotKeyData, lFileSize, &bytesRead);
		f_close(&fileObject);

		j = 0;
		k = 0;
		for(i=0; i<lFileSize; i++){
			if(k<56){
				if(acHotKeyData[i] == 10){
					acNucString[j] = 0;
					j = 0;
					if(k<8){
						m_ucHotKeyNuclideID[0][k] = atoi(acNucString);
					}else if(k>=8 && k<16){
						m_ucHotKeyNuclideID[1][k-8] = atoi(acNucString);
					}else if(k>=16 && k<36){
						m_ucHotKeyNuclideID2[0][k-16] = atoi(acNucString);
					}else if(k>=36){
						m_ucHotKeyNuclideID2[1][k-36] = atoi(acNucString);
					}
					k++;
				}else{
					acNucString[j] = acHotKeyData[i];
					j++;
				}
			}
		}
	}else{
		DB_PopulateDefaultHotkeys();
	}
}

static int getIndexAZaz09(char input){
	int returnValue = -1;
	int i, length;

	length = sizeof(encoded_map);
	for(i=0; i<length; i++){
		if(input == encoded_map[i]){
			returnValue = i;
			break;
		}
	}

	return returnValue;
}

static void dashSeparate(char *input){
	int i, j, length;
	char output[50];

	j = 0;
	length = strlen(input);
	for(i=0; i<length; i++){
		if((i!=0) && ((i%4) == 0)){
			output[j++] = '-';
		}
		output[j++] = input[i];
	}
	output[j] = 0;
	strcpy(input, output);
}

static long long int getDeviceID(void){
	int i;
	unsigned long int dummy, checksum, software_id;
	FATFS *fs;
	char serial_string[10];
	long long int return_value, remapped;

	f_getfree("0:", &dummy, &fs);
	sprintf(serial_string, "%08X", fs->serial_number);

	software_id = 3;
	software_id = ~software_id;
	software_id = 0x3F & software_id;

	return_value = software_id;
	checksum = software_id;
	return_value <<= 6;

	for(i=0; i<8; i++){
		remapped = getIndexAZaz09(serial_string[i]);
		if((remapped < 0) || (remapped > 63)) remapped = 63;
		return_value += remapped;
		checksum += remapped;
		return_value <<= 6;
	}
	return_value <<= 3;
	return_value += (checksum & 0x1FF);
	return_value = (return_value & 0x7FFFFFFFFFFFFFFF);

	return return_value;
}

//  0 = OK
//  1 = invalid key
//  2 = invalid setting
static int verifyKey(long long int keylong){
	int return_value;
	int i, calc, signature;
	int six_bits[8];
	long long int compid, temp;

	if(keylong != 0){
		compid = getDeviceID();

		// Verify Checksum
		temp = keylong;
		calc = 0;
		for(i=0; i<7; i++){
			calc += (int) (temp & 0xFF);
			temp >>= 8 ;
		}

		if((calc & 0x7F) == (temp &0x7F)){
			// Create Inverted CompID
			temp = 0;
			for(i=0; i<7; i++){
				temp += (int) (compid & 0xFF);
				compid >>= 8;
				if(i != 6) temp <<= 8;
			}

			// XOR Key with Invered CompID
			compid = keylong ^ temp;

			for(i=0; i<8; i++){
				six_bits[7-i] = (int) (compid & 0x3F);
				compid >>= 6;
			}
			signature = (int) (compid & 0xFF);

			// Verify A5 Signature
			if(signature == 0xA5){
				// Verify 000001 feature set
				if(six_bits[0] == 1){
					return_value = 0;
					for(i=1; i<8; i++){
						if(six_bits[i] != 59){
							return_value = 2;
							break;
						}
					}
				}else{
					return_value = 2;
				}
			}else{
				return_value = 1;
			}
		}else{
			return_value = 1;
		}
	}else{
		return_value = 1;
	}

	return return_value;
}

long long int ConvertStringToKey(CHAR *key){
	long long int return_value;
	int i, length, invalid_char;
	char *removed_dash, *ptr, schr;

	invalid_char = 0;
	removed_dash = malloc(50);

	length = strlen(key);
	ptr = removed_dash;
	for(i=0; i<length; i++){
		schr = key[i];
		if(((schr >= '0') && (schr <= '9')) || (schr == '-')){
			if(schr != '-'){
				*ptr++ = schr;
			}
		}else{
			invalid_char = 1;
			break;
		}
	}
	*ptr = 0;

	if(!invalid_char){
		if(strlen(removed_dash)){
			return_value = atoll(removed_dash);
		}else{
			return_value = -999;
		}
	}else{
		return_value = -999;
	}

	free(removed_dash);
	return return_value;
}

void ConvertLongLongIntToAscii(long long int value, char *output){
	sprintf(output, "%lld", value);
	dashSeparate(output);
}

void GenerateDeviceID(char *deviceID){
	ConvertLongLongIntToAscii(getDeviceID(), deviceID);
}

//  0 = OK
//  1 = invalid key
//  2 = invalid setting
int VerifyKeyString(CHAR *key){
	int return_value;
	long long int keylong;

	keylong = ConvertStringToKey(key);
	if(keylong < 1){
		return_value = 1;
	}else{
		return_value = verifyKey(keylong);
	}
	return return_value;
}

void SetKeyFeatures(void){
	if(current.key){
		if(!verifyKey(current.key)){
			current.pccomm_enabled = TRUE;
		}else{
			current.pccomm_enabled = FALSE;
		}
	}else{
		current.pccomm_enabled = FALSE;
	}
}

static void UCase(char *str){
	while(*str){
		if(((*str) >= 0x61) && ((*str) <= 0x7A)) (*str) = (*str) - 0x20;
		str++;
	}
}

// Old main routine
/*int main(void){
	int status;

	//bool flgOn;
	//ulong ulOneSec;

	ulong ul100MSec;

	//asm("nop");
	//asm("halt");
	//watchdog timer automatically runs after power on with
	//~8 second timeout at 64MHz
	ClearLowLevel();

	Sysset_clearStruct();

	service_watchdog();

	set_interrupt_level();

	//disable all interrupts
	cf.intc[0].imrh = 0xffffffff;
	cf.intc[0].imrl = 0xffffffff;
	cf.intc[1].imrh = 0xffffffff;
	cf.intc[1].imrl = 0xffffffff;

	set_exceptions();

	service_watchdog();
	init_chip_selects();
	set_touch_reset(FALSE);
	power_mmc(TRUE);

	ul100MSec = g_csec_tstamp + 20;
	//set_touch_reset(TRUE);
	//set_touch_power(TRUE);
	//set_touch_program(TRUE);
	//asm("nop");
	//asm("halt");

	service_watchdog();
	initialize_i2c();
	first_initialization();
	NuclideData_initializeMirror();
	VolumeMirror();
	BrightnessMirror();
	SleepBrightnessMirror();
	SleepTimeoutMirror();
	AmuletWellMainScreenMenu_wipeMirror();
	AmuletWellMainScreenMenu_wellStandardMirror();
	AmuletWellMainScreenMenu_wellTriggerMirror();

	if(!setup_rtc()){
		asm("nop");
		asm("halt");
	}

	//init previously displayed clock time to 0 to force first display
	service_watchdog();
	read_clock(&clock_time);
	read_clock(&low_clock_time);
	prev_clock_time = 0;
	reset_minute_counter();

	initialize_edge_port();

	service_watchdog();
	initialize_screen();

	//initialize all interrupts
	initialize_interrupts();
	//first_initialization();

	//g_ucRelease = 0;
	//ulOneSec = g_msec_tstamp + 1000;
	//while(!g_ucRelease){
	//	service_amulet();
	//	if(g_msec_tstamp > ulOneSec){
	//		ulOneSec = g_msec_tstamp + 1000;
	//		//SetAmuletByte(100,0xFF);
	//		//printf("Sent One Packet\n");
	//	}
	//}
	//printf("Finished\n");
	//for(;;);

	//initialize uarts
	init_uart(4800,U_PR);       //printer

	// TEMPDEV
	init_uart(9600,U_PC);      //computer

	//read in current printer
	EE_READ(print[0],(uchar *)&current.printer);

	if (current.printer == USB_PRINTER || current.printer == USB_EPS_PRINTER) g_Init_USB_PCL =1 ;
	else g_Init_USB_PCL = 0;

	//read values from EEPROM into RAM
	//EE_READ(usercal,(uchar *)user_cal);

	display_text(24,0,"Program",0,MEDIUM,NORMAL);
	display_text(24,20,"Power-up",0,MEDIUM,NORMAL);
	display_text(24,34,"Sequence",0,MEDIUM,NORMAL);

	//initialize mmc card
	service_watchdog();

	if(!init_mmc_card()){
		asm("nop");
		asm("halt");
	}

	//functions on mmc card
	set_options();

	//initialize QSPI for communicating with chamber
	init_qspi();

	//disable adc servicing
	set_adc_enabled(FALSE);

	//initialize chambers
	initialize_chambers();

	smart_linearity();

	//initialize usb
	startup_811();

	InitializeSL811H();

	//set watchdog timeout to 2 second
#ifdef WATCHDOG_OFF
	if(!WATCHDOG_OFF){
		setup_watchdog();
	}
#else
	setup_watchdog();
#endif


	//display signon screen
	//signon();

	//verify_chamber_checksum();

	//initialize printer
	printer_init();

	id_init();

	setup_chambers();

	//enable ADC to start getting activity
	if(current.num_chambers != 0) set_adc_enabled(TRUE);

	//setup heartbeat and initialize remote
	initialize_remote();

	//setup MCA Board
#ifdef MCA_SIMULATION
	if(MCA_SIMULATION){
		Mca_setup_demo();
	}else{
		status = Mca_setup();
	}
#else
	status = Mca_setup();
#endif

#ifdef CALIB_ONLY
	if(CALIB_ONLY) Mca_setup_none();
#endif

	//ee_init();

	//setup Amulet Screen
	while(ul100MSec > g_csec_tstamp) service_watchdog();
	//set_touch_program(TRUE);
	set_touch_reset(TRUE);

	//ethernet_init();

#ifdef FORCE_FLASH
	FlashAmulet(Amulet_amuletImage);
#endif

	//while(!g_ucRelease) service_amulet();

	//Amulet_DisplayErrorCode(status);

	//call main measurement screen which never returns
	measurement_screen();

	return 0;
}*/



/*static void test_print(void)
 - {
 - 	short i;
 - 	char tstr[10];
 - 	char tstrng[20];
 - 
 - 	if(current.printer != OKI_PRINTER)
 - 		return;
 - 
 - 	for(i = 32; i < 128; i++)
 - 	{
 - 		sprintf(tstrng,"%d %0x  ",i,i);
 - 		tstr[0] = (char)i;
 - 		tstr[1] = '\r';
 - 		tstr[2] = '\n';
 - 		tstr[3] = '\0';
 - 		strcat(tstrng,tstr);
 - 		pr_write(tstrng);
 - 	}
 - 
 - 	for(i = 128; i < 256; i++)
 - 	{
 - 		sprintf(tstrng,"%d %0x  ",i,i);
 - 		tstr[0] = 0x1b;
 - 		tstr[1] = '^';
 - 		tstr[2] = (char)i;
 - 		tstr[3] = '\r';
 - 		tstr[4] = '\n';
 - 		tstr[5] = '\0';
 - 		strcat(tstrng,tstr);
 - 		pr_write(tstrng);
 - 
 - 	}
 - }	*/

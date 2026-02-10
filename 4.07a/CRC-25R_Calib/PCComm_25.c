/**
 * \file
 * \details This files contains functions, which calls either the UART layer or the USB layer and implements PC Communications functions.
 */
/***********************************************************
  MODULE:  PC Communications for CRC-25

  FILE:  PCComm_25.c

  DATE:  01/08/07
         08/22/07 -- removed automated constancy test
  *****************************************************************/
#include "crc.h"
#include "uart.h"
#include "screen.h"
#include "i2c.h"
#include "pit.h"
#include "chambfac.h"
#include "counter.h"
#include "daily.h"
#include "sl811s.h"
#include "keyboard.h"
#include "remote.h"
#include "nuc.h"
#include "qspi.h"
#include "amulet.h"
//#include "fat.h"
//#include "mmcapi.h"
#include "ff.h"
#include "wipes.h"
#include "mca.h"
#include "message.h"
#include <string.h>
#include <stdlib.h>
#ifdef TERMINAL
#include <stdarg.h>
#include "capFIFO.h"
#endif // #ifdef TERMINAL

long PCComm_transferFileSize;
volatile short PCComm_currentPCCommChamber;

#define END_CHAR  '#'
#define START_CHAR '$'



#define INCDCBUFFERLEN	257
char InCDCCircularBuffer[INCDCBUFFERLEN];
unsigned short InCDCCircularHead = 0;
unsigned short InCDCCircularTail = 0;

extern CHAMBER chamber[];
extern MEASUREMENT measurement[];
extern CURRENT current;
extern BKGDATA bkgdata[];
extern BIASDATA biasdata[];
extern ZERODATA zerodata[];
extern short max_chambers;
extern unsigned char UsbUpdate_seqNumber;
extern short UsbUpdate_bufferEnd;
extern bool UsbUpdate_transferFinished;
//extern F_FILE *UsbUpdate_fileHandle;
extern FIL UsbUpdate_fileObject;
//extern F_FILE *FileTransfer_pointer;
extern FIL FileTransfer_fileObject;
extern unsigned long FileTransfer_filesize;
extern bool FileTransfer_lastBlock;
extern short FileTransfer_pccomm;
extern unsigned long ulLowLevelHead;
extern unsigned long ulLowLevelTail;
extern volatile bool AmuletPCQC_Abort;
extern unsigned char m_ucHotKeyNuclideID2[UPPER_LIMIT_CHAMB][20];
extern unsigned char m_ucClear;
extern unsigned char m_ucSetWellMeasure;
extern bool AmuletWellMeasurementMenu_unlockDetector;
extern bool AmuletWellMeasurementMenu_autoMeasure;
extern char AmuletWellMeasurementMenu_testID[11];
extern char AmuletWellMeasurementMenu_testIDPCComm[11];
extern WELLWIPE AmuletWellMeasureMenu_wellWipePCComm;
extern long long int AmuletDailyMenu_ChamberDailyTestID;
extern long long int AmuletGenericYesNo_LatchedDailyTestID;

extern int AmuletWellAutoCalibrate_exportStatus;
extern char AmuletWellAutoCalibrate_exportFirstPeak[11];
extern char AmuletWellAutoCalibrate_exportFirstPeakDev[11];
extern char AmuletWellAutoCalibrate_exportSecondPeak[11];
extern char AmuletWellAutoCalibrate_exportSecondPeakDev[11];
extern char AmuletWellAutoCalibrate_exportFWHM[11];
extern char AmuletWellAutoCalibrate_exportVolts[11];
extern char AmuletWellAutoCalibrate_exportThreshold[11];
extern char AmuletWellAutoCalibrate_exportGain1[11];
extern char AmuletWellAutoCalibrate_exportGain2[11];
extern char AmuletWellAutoCalibrate_exportGain[11];
extern char AmuletWellAutoCalibrate_exportZero[11];
extern int AmuletWellAutoCalibrate_exportSequence;

#ifdef TERMINAL
extern int qspi_sim_adc_gain[2];
extern int qspi_sim_adc_value[2];
#endif // #ifdef TERMINAL

//error codes
enum
{
    RECEIVE_ERROR,	// 0
    CHECKSUM_ERROR, // 1
    COMMAND_ERROR,  // 2
    LENGTH_ERROR,   // 3
    CHAMBER_ERROR,  // 4
    ISOTOPE_ERROR,  // 5
    MEAS_SCREEN_ERROR, // 6
    NO_BETA_MEAS_ERROR, // 7
    NO_WIPE_MEAS_ERROR, // 8
    NO_TEST_READY_ERROR, // 9
    NO_END_CHAR,		// :
    WIPE_LOCATION_MISSING_ERROR, // ;
    WIPE_TEST_ID_MISSING_ERROR, // <
    WIPE_INVALID_NUCLIDE_ERROR, // =
    WIPE_NOT_IN_WIPE_SCREEN_ERROR, // >
    WIPE_AUTOCAL_EXPIRED_ERROR, // ?
    WIPE_BACKGROUND_MISSING_ERROR, // @
    WIPE_BACKGROUND_EXPIRED_ERROR, // A
    WIPE_BACKGROUND_TOO_HIGH_ERROR, // B
    WIPE_INVALID_LOCATION_ERROR,	// C
    WIPE_ALREADY_PERFORMING_TEST_ERROR, // D
    WIPE_INVALID_TEST_ID_ERROR, // E
    WIPE_TEST_IN_PROGRESS_ERROR, // F
    WIPE_TEST_ABORTED, // G
    WIPE_ACTIVITY_OVERFLOW,	// H
	NO_KEY,	// I
};

//tests from PC
enum
{
    PC_DAILY,
    PC_BKG,
    PC_BIAS,
    PC_ACQ,
};

static void error_msg(short error);
static void parse_command(char *msg, short len);
static void parse_extended(char *msg, short len);
static void parse_change_nuclide(char *msg);
static void parse_change_chamber(char *msg);
static void pc_change_chamber(short ch_num);
static void parse_read_chamber(char *msg);
static void pc_read_chamber(short ch_num);
static void parse_continue_accept(char ca);
static void parse_test(short test_num);
static void pc_read_test(char tchar);
static void pc_send_sn(void);
static void pc_screen_print(void);
static void pc_get_configuration(void);
static void pc_data_check(void);
static void pc_num_sources(void);
static void pc_get_source(char num);
static void pc_read_accuracy(char num);
static bool get_command(char *str);
static void send_to_pc(char *str);
static bool pc_getuser(short ch_num, short but);
static void pc_get_user_key(short but,char *name);
static void parse_change_user_nuclide(char *msg);
static void parse_update_user_key(char *msg);
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);
unsigned char CurrentPageStack(void);
void PushPageStack(unsigned char ucPage);
void ClearPageStack(void);
void SleepRefreshTimeout(void);
void trim(char *acByte);
void trim_and_shrink(char *acByte);
void toupper(char *acByte);
unsigned short InCDCPush(char *Input, short Length);
unsigned short InCDCPop(char *Output, short Length);
short InCDCUsed(void);
short InCDCFree(void);
char InCDCPeep(short BufferIndex);
short InCDCScan(char testFor);
void InCDCStrip(char testFor);
unsigned short InBufferPop1(char *Output);
void DB_UpdateDailyTestDataCheck(long long int ChamberDailyTestID, char *DataCheckTextEnglish, char *DataCheckTextSpanish, bool DataCheckFailed, char *DataCheckCRC, bool bookEnd);
void splitoutunits(char *act, char *numbers, char *units);
#ifdef TERMINAL
void setup_short_watchdog(void);
void set_touch_reset(bool on);
void set_touch_power(bool on);
#endif // #ifdef TERMINAL

static short  pc_ret;
static bool pc_freeze_flag;

enum peak_type{
	NOT_FOUND,
	PRIMARY,
	SECONDARY,
	TERTIARY,
	SUMMED
};

bool get_bias_delay_flag(void);

    //send return value to calling routine
/**
 * \details Send return value to calling routine
 * \returns Return value
 */
    short get_pc_ret(void)
    {
        return(pc_ret);
    }

    //allow calling routine to reset pc_ret
/**
 * \details Allow calling routine to reset pc_ret
 * \returns None
 */
    void clear_pc_ret(void)
    {

        pc_ret = PC_RET_NOT_SET;
    }
/**
 * \details Set pc_ret to START
 * \returns None
 */
    void set_pc_start_flag(void)
    {
        pc_ret = PC_START_FLAG;
    }

    //routine to parse and act on data received from PC
/**
 * \details Routine to parse and act on data received from PC
 * \param ok_flag True = OK flag is valid, False = OK flag is not valid
 * \returns None
 */
    void from_pc(bool ok_flag)
    {
        char str[64];
        uchar cs;
        short len;
        uchar calc_cs;
        char *msg;
        char sstr[6];

        //ignore if option not set
        if((current.pccomm == PC_COMM_NONE) || (current.pccomm == PC_COMM_USB_DEBUG_CHAMBER) || (current.pccomm == PC_COMM_USB_UPDATE) || (current.pccomm == PC_COMM_USB_UPLOAD_FILE))
            return;

        pc_ret = PC_NO_ACTION;   //default return

        //if wrong character recieved, send RECEIVE ERROR and return
        if(!ok_flag)
        {
            error_msg(RECEIVE_ERROR);
            delay_msec(6);
            return;
        }

    //send acknowledge
        sstr[0] = START_CHAR;
        sstr[1] = 'O';
        sstr[2] = END_CHAR;
        sstr[3] = '\0';
        send_to_pc(sstr);

        if(!get_command(str))
        {
            error_msg(NO_END_CHAR);
            delay_msec(6);
            return;
        }




    //get length and calculate checksum
        len = str[1] - 0x41;
        calc_cs = checksum(&str[1],len + 1);
        cs = str[len + 2];
   //if checksum incorrect, send checksum error message & return
        if(calc_cs != cs)
        {
            error_msg(CHECKSUM_ERROR);
            return;
        }

    //parse the command
        msg = &str[2];
        parse_command(msg,len);


    }
/**
 * \details Parse command byte from command string and call parse_command, which fulfills the command
 * \param cmd Pointer to command string
 * \returns None
 */
void process_command(char *cmd){
	short len;
	uchar calc_cs, cs;;

	//get length and calculate checksum
    len = cmd[1] - 0x41;
    calc_cs = checksum(&cmd[1],len + 1);
    cs = cmd[len + 2];

    if(calc_cs != cs){
    	error_msg(CHECKSUM_ERROR);
    	return;
    }

    parse_command(&(cmd[2]), len);
}
/**
 * \details Master process to scan the circular buffer for command strings and calls the process_command, which fulfulls the command
 * \returns None
 */
void from_pc_cdc(void){
	short pcStartCharPos, startCharPos, endCharPos;
	char cmdString[60];
	char sstr[6], transferChar;

	if((current.pccomm == PC_COMM_NONE) || (current.pccomm == PC_COMM_USB_DEBUG_CHAMBER) || (current.pccomm == PC_COMM_USB_UPDATE) || (current.pccomm == PC_COMM_USB_UPLOAD_FILE))
	return;

	while(InBufferPop1(&transferChar)) InCDCPush(&transferChar, 1);

	pcStartCharPos = InCDCScan(PC_START_CHAR);
	startCharPos = InCDCScan(START_CHAR);
	endCharPos = InCDCScan(END_CHAR);

	if(pcStartCharPos == 0){
		pc_ret = PC_NO_ACTION;   //default return

		// Remove @
		InCDCPop(cmdString, 1);

		// Send Acknowledge
		sstr[0] = START_CHAR;
		sstr[1] = 'O';
        sstr[2] = END_CHAR;
        sstr[3] = '\0';
        send_to_pc(sstr);
	}else{
		if(pcStartCharPos == -1){
			if((startCharPos != -1) && (endCharPos != -1)){
				if(startCharPos < endCharPos){
					pc_ret = PC_NO_ACTION;   //default return
					//Strip character infront of $
					InCDCStrip(START_CHAR);

					// Process Command
					endCharPos = InCDCScan(END_CHAR);
					InCDCPop(cmdString, endCharPos + 1);
					process_command(cmdString);
				}else{
					// Send Receive Error
					error_msg(RECEIVE_ERROR);

					// Strip character infront of $
					InCDCStrip(START_CHAR);
				}
			}else if((startCharPos == -1) && (endCharPos != -1)){
				// Send Receive Error
				error_msg(RECEIVE_ERROR);

				// Strip character infront of # and also strip #
				InCDCStrip(END_CHAR);
				InCDCPop(cmdString, 1);
			}
		}else{
			if((startCharPos == -1) && (endCharPos == -1)){
				// Unable to Detect $ or #
				// Strip character infront of @
				InCDCStrip(PC_START_CHAR);
			}else if((startCharPos != -1) && (endCharPos == -1)){
				// Detected only $
				if(startCharPos < pcStartCharPos) error_msg(RECEIVE_ERROR);

				// Strip character infront of @
				InCDCStrip(PC_START_CHAR);
			}else if(startCharPos == -1 && endCharPos != -1){
				// Detected only #
				if(endCharPos < pcStartCharPos) error_msg(RECEIVE_ERROR);

				// Strip character infront of @
				InCDCStrip(PC_START_CHAR);
			}else{
				// Detected $ and #
				if((startCharPos < pcStartCharPos) || (endCharPos < pcStartCharPos)){
					// Either $ or # occur before @
					if((startCharPos < pcStartCharPos) && (endCharPos < pcStartCharPos)){
						// Both $ and # occur before @
						if(startCharPos < endCharPos){
							pc_ret = PC_NO_ACTION;   //default return
							// $ occurs before #
							// Strip character infront of $
							InCDCStrip(START_CHAR);

							// Process command
							endCharPos = InCDCScan(END_CHAR);
							InCDCPop(cmdString, endCharPos + 1);
							process_command(cmdString);
						}else{
							// # occurs before $
							// Send Receive Error
							error_msg(RECEIVE_ERROR);

							// Strip character infront of $
							InCDCStrip(START_CHAR);
						}
					}else{
						// Either $ or # occur before @
						// Send Receive Error
						error_msg(RECEIVE_ERROR);

						// Strip character infront of @
						InCDCStrip(PC_START_CHAR);
					}
				}else{
					// Neither $ or # occur before @
					// Strip character infront of @
					InCDCStrip(PC_START_CHAR);
				}
			}
		}
	}
}
/**
 * \details Parse and fulfill the command string
 * \param msg Point to the command string
 * \param len Command length
 * \returns None
 */
    static void parse_command(char *msg, short len)
    {
    	WELLWIPELOCATION *available;
    	float WipeBackgroundTrigger;
    	int i, WipeLocationCount;
    	long long int nucindex;
    	char msgcopy[35], wipeloc[24], testid[11], wipenuc[7], dpm[20], nuclidename[7];
        //short mode = get_crc_mode();
    	char reply[100];
    	bool flgWipeLocFound;
    	static char *AutoCalBuf = NULL;

        switch(msg[0])
        {
        case 'I':   //change isotope to built-in
            //length must be 3
            if(len == 3)
                parse_change_nuclide(msg);
            else
                error_msg(LENGTH_ERROR);
            break;
        case 'U':   //change isotope to User
            //length must be 3
            if(len == 3)
                parse_change_user_nuclide(msg);
            else
                error_msg(LENGTH_ERROR);
            break;
        case 'K':   //update user key
            //length must be 2
            if(len == 2)
                parse_update_user_key(msg);
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'R': //read activity
            if(len == 2)
                parse_read_chamber(msg);
            else
                error_msg(LENGTH_ERROR);
            break;
        case 'M':   //change main chamber
            //can only be issued on measurerment screen
            if(!get_meas_screen() || (!(CurrentPageStack() == AmuletHTMLIndex[MAINSCREEN_HTM]) && !(CurrentPageStack() == AmuletHTMLIndex[PCQC_HTM])))
            {
                error_msg(MEAS_SCREEN_ERROR);
                break;
            }
            if(len == 2)
                parse_change_chamber(msg);
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'B':  //begin Background measurement
            //can only be issued on measurerment screen
            if(!get_meas_screen() || (!(CurrentPageStack() == AmuletHTMLIndex[MAINSCREEN_HTM]) && !(CurrentPageStack() == AmuletHTMLIndex[PCQC_HTM])))
            {
                error_msg(MEAS_SCREEN_ERROR);
                break;
            }
            if(len == 1)
                parse_test(PC_BKG);
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'D':  //begin Daily Test
            //can only be issued on measurerment screen
            if(!get_meas_screen() || (!(CurrentPageStack() == AmuletHTMLIndex[MAINSCREEN_HTM]) && !(CurrentPageStack() == AmuletHTMLIndex[PCQC_HTM])))
            {
                error_msg(MEAS_SCREEN_ERROR);
                break;
            }
            if(len == 1){
            	if(CurrentPageStack() != AmuletHTMLIndex[PCQC_HTM]){
            		SetAmuletHTML(AmuletHTMLIndex[PCQC_HTM]);
            		PushPageStack(AmuletHTMLIndex[PCQC_HTM]);
            	}
            	AmuletPCQC_Abort = FALSE;
                parse_test(PC_DAILY);
                AmuletPCQC_Abort = FALSE;
                SleepRefreshTimeout();
                SetAmuletHTML(AmuletHTMLIndex[MAINSCREEN_HTM]);
                ClearPageStack();
                PushPageStack(AmuletHTMLIndex[MAINSCREEN_HTM]);
            }else{
                error_msg(LENGTH_ERROR);
            }
            break;

        case 'C':  //continue message
            if(len == 1)
                parse_continue_accept('C');
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'Z':  //read zero
            if(len == 1)
                pc_read_test('Z');
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'b':   //read background
            if(len == 1)
                pc_read_test('b');
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'A':  //accept the value
            if(len == 1)
                parse_continue_accept('A');
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'V':  //begin Bias Measurement
            //can only be issued on measurerment screen
            if(!get_meas_screen() || (!(CurrentPageStack() == AmuletHTMLIndex[MAINSCREEN_HTM]) && !(CurrentPageStack() == AmuletHTMLIndex[PCQC_HTM])))
            {
                error_msg(MEAS_SCREEN_ERROR);
                break;
            }
            if(len == 1)
                parse_test(PC_BIAS);
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'v':   //read Bias Voltage
            if(len == 1)
                pc_read_test('v');
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'S':   //send SN
            if(len == 1)
                pc_send_sn();
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'P':     //print screen
            if(len == 1)
                pc_screen_print();
            else
                error_msg(LENGTH_ERROR);
            break;



        case 'G':  //Get Configuration
            if(len == 1)
                pc_get_configuration();
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'd':   //get Data Check
            if(len == 1)
                pc_data_check();
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'N':   //get number of daily sources
            if(len == 1)
                pc_num_sources();
            else
                error_msg(LENGTH_ERROR);
            break;


        case 's':   //Get accuracy source data
            if(len == 2)
                pc_get_source(msg[1]);
            else
                error_msg(LENGTH_ERROR);
            break;

        case 'a':   //get accuracy test measurement
            if(len == 2)
                pc_read_accuracy(msg[1]);
            else
                error_msg(LENGTH_ERROR);
            break;


        case 'm':  // Repeated or Maximum Acquistion
            //can only be issued on measurerment screen
            if(!get_meas_screen() || (!(CurrentPageStack() == AmuletHTMLIndex[MAINSCREEN_HTM]) && !(CurrentPageStack() == AmuletHTMLIndex[PCQC_HTM])))
            {
                error_msg(MEAS_SCREEN_ERROR);
                break;
            }
            if(len == 1)
                parse_test(PC_ACQ);
            else
                error_msg(LENGTH_ERROR);
            break;


        case 'W': // Start Wipe Test
        	if(len == 34){
        		for(i=0; i<34; i++) msgcopy[i] = msg[i];
        		for(i=23; i>0; i--){
        			if(msgcopy[i] == ' '){
        				msgcopy[i] = 0;
        			}else{
        				break;
        			}
        		}
        		wipeloc[23] = 0;
        		for(i=0; i<23; i++){
        			wipeloc[i] = msgcopy[i+1];
        		}

        		for(i=33; i>23; i--){
        			if(msgcopy[i] == ' '){
        				msgcopy[i] = 0;
        			}else{
        				break;
        			}
        		}
        		testid[10] = 0;
        		for(i=0; i<10; i++){
        			testid[i] = msgcopy[i + 24];
        		}

        		WipeBackgroundTrigger = (WipesCurrentType(1))->Threshold;
        		WipeLocationCount = ActiveWipeLocationCount();
        		available = ActiveWipeLocations();
        		flgWipeLocFound = FALSE;
        		if(WipeLocationCount > 0){
        			for(i=0; i<WipeLocationCount; i++){
        				if(strcmp(available[i].Name, wipeloc) == 0){
        					flgWipeLocFound = TRUE;
        					break;
        				}
        			}
        		}
        		if(strlen(wipeloc) == 0){
        			// Check Wipe Location
        			error_msg(WIPE_LOCATION_MISSING_ERROR);
        		}else if(strlen(testid) == 0){
        			// Check Test ID
        			error_msg(WIPE_TEST_ID_MISSING_ERROR);
        		}else if(AmuletWellMeasurementMenu_autoMeasure){
        			// Check Wipe Test is not running
        			error_msg(WIPE_ALREADY_PERFORMING_TEST_ERROR);
        		}else if(CurrentPageStack() != AmuletHTMLIndex[WELLWIPELIST_HTM]){
        			// Check Current Screen is Wipe
        			error_msg(WIPE_NOT_IN_WIPE_SCREEN_ERROR);
        		}else if(!Mca_getCalibratedToday()){
        			// Check Autocal has not expired
        			error_msg(WIPE_AUTOCAL_EXPIRED_ERROR);
        		}else if(!Mca_getBackgroundStatus()){
        			// Check presence of Background
        			error_msg(WIPE_BACKGROUND_MISSING_ERROR);
        		}else if(!Mca_getBackgroundToday()){
        			// Check Background has not expired
        			error_msg(WIPE_BACKGROUND_EXPIRED_ERROR);
        		}else if(Mca_getAverageBackgroundCPM(NULL) > WipeBackgroundTrigger){
        			// Check Background is not too high
        			error_msg(WIPE_BACKGROUND_TOO_HIGH_ERROR);
        		}else if(!flgWipeLocFound){
        			// Check Wipe Location Validitiy
        			error_msg(WIPE_INVALID_LOCATION_ERROR);
        		}else{
        			reply[0] = START_CHAR;
        			reply[1] = 35 + 0x41;
        			reply[2] = 'D';
        			for(i=0;i<34;i++) reply[i + 3] = msg[i];
        			reply[37] = checksum(&(reply[1]), 36);
        			reply[38] = '#';
        			reply[39] = 0;
					send_to_pc(reply);
					AmuletWellMeasurementMenu_autoMeasure = TRUE;
					strcpy(AmuletWellMeasurementMenu_testID, testid);

					InitializeAvailableWipeLocations();
					available = AvailableWipeLocations();
					WipeLocationCount = AvailableWipeLocationCount();
					for(i=0; i<WipeLocationCount; i++){
						if(strcmp(available[i].Name, wipeloc) == 0){
							AddAvailableWipeLocation(i);
							break;
						}
					}

					SelectWipeLocationClearIndex();
					m_ucClear = 18;
					m_ucSetWellMeasure = 4;
					AmuletWellMeasurementMenu_unlockDetector = FALSE;
					SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
					PushPageStack(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
        		}
        	}else{
        		error_msg(LENGTH_ERROR);
        	}
        	break;

        case 'w': // Send Result of Wipe Test
        	if(len == 17){
        		for(i=0; i<17; i++) msgcopy[i] = msg[i];

        		for(i=6; i>0; i--){
        			if(msgcopy[i] == ' '){
        				msgcopy[i] = 0;
        			}else{
        				break;
        			}
        		}
        		wipenuc[6] = 0;
        		for(i=0; i<6; i++){
        			wipenuc[i] = msgcopy[i+1];
        		}

        		for(i=16; i>6; i--){
        			if(msgcopy[i] == ' '){
        				msgcopy[i] = 0;
        			}else{
        				break;
        			}
        		}
        		testid[10] = 0;
        		for(i=0; i<10; i++){
        			testid[i] = msgcopy[i + 7];
        		}

        		if(strlen(testid) == 0){
        			// Check Test ID
        			error_msg(WIPE_TEST_ID_MISSING_ERROR);
        		}else if(AmuletWellMeasurementMenu_autoMeasure){
        			// Check Wipe Test is not running
        			error_msg(WIPE_TEST_IN_PROGRESS_ERROR);
        		}else if(CurrentPageStack() != AmuletHTMLIndex[WELLWIPELIST_HTM]){
        			// Check Current Screen is Wipe
        			error_msg(WIPE_NOT_IN_WIPE_SCREEN_ERROR);
        		}else if(strcmp(testid, AmuletWellMeasurementMenu_testIDPCComm) != 0){
        			// Check TestID against Run Version
        			error_msg(WIPE_INVALID_TEST_ID_ERROR);
        		}else if(AmuletWellMeasureMenu_wellWipePCComm.WellWipeID < 1){
        			error_msg(WIPE_TEST_ABORTED);
        		}else{
        			if(strlen(wipenuc) == 0){
        				reply[0] = '$';
        				reply[1] = 28 + 0x41;
        				reply[2] = 'D';
        				for(i=0; i<17; i++) reply[i + 3] = msg[i];
        				sprintf(dpm, "%.0f", AmuletWellMeasureMenu_wellWipePCComm.Activity);
        				if(strlen(dpm) > 10){
        					error_msg(WIPE_ACTIVITY_OVERFLOW);
        				}else{
        					for(i=20; i<30; i++) reply[i] = ' ';
        					reply[20] = 0;
        					strcat(reply, dpm);
        					for(i=20; i<30; i++){
        						if(reply[i] == 0) reply[i] = ' ';
        					}
        				}
        				reply[30] = checksum(&(reply[1]), 29);
        				reply[31] = '#';
        				reply[32] = 0;
        				send_to_pc(reply);
        			}else{
        				nucindex = -1;
        				for(i=0; i<10; i++){
        					if(AmuletWellMeasureMenu_wellWipePCComm.WellWipeNuclide[i].WellWipeNuclideID > 0){
        						strcpy(nuclidename, AmuletWellMeasureMenu_wellWipePCComm.WellWipeNuclide[i].Name);
        						toupper(nuclidename);
        						trim_and_shrink(nuclidename);
        						toupper(wipenuc);
        						trim_and_shrink(wipenuc);
        						if(strcmp(nuclidename, wipenuc) == 0){
        							nucindex = AmuletWellMeasureMenu_wellWipePCComm.WellWipeNuclide[i].WellWipeNuclideID;
        							break;
        						}
        					}
        				}

        				if(nucindex == -1){
        					error_msg(WIPE_INVALID_NUCLIDE_ERROR);
        				}else{
        					dpm[0] = '0';
        					dpm[1] = 0;
        					for(i=0; i<40; i++){
        						if(AmuletWellMeasureMenu_wellWipePCComm.Peaks[i].Energy >= 0.0){
        							if(AmuletWellMeasureMenu_wellWipePCComm.Peaks[i].PeakType == PRIMARY){
        								if(AmuletWellMeasureMenu_wellWipePCComm.Peaks[i].WellWipeNuclideID == nucindex){
        									sprintf(dpm, "%.0f", AmuletWellMeasureMenu_wellWipePCComm.Peaks[i].Activity);
        									break;
        								}
        							}
        						}
        					}

        					reply[0] = '$';
							reply[1] = 28 + 0x41;
							reply[2] = 'D';
							for(i=0; i<17; i++) reply[i + 3] = msg[i];
							if(strlen(dpm) > 10){
								error_msg(WIPE_ACTIVITY_OVERFLOW);
							}else{
								for(i=20; i<30; i++) reply[i] = ' ';
								reply[20] = 0;
								strcat(reply, dpm);
								for(i=20; i<30; i++){
									if(reply[i] == 0) reply[i] = ' ';
								}
							}
							reply[30] = checksum(&(reply[1]), 29);
							reply[31] = '#';
							reply[32] = 0;
							send_to_pc(reply);
        				}
        			}
        		}
        	}else{
        		error_msg(LENGTH_ERROR);
        	}
        	break;

        case '5': // 55t extended instruction set
        	parse_extended(&(msg[1]), strlen(msg) - 1);
        	break;

        case 'L': // Get AutoCal Result $BL6#
        	if(!AutoCalBuf) AutoCalBuf = malloc(131);

        	memset(AutoCalBuf, ' ', 131);
        	AutoCalBuf[130] = 0;
        	AutoCalBuf[0] = '$';
        	if(AmuletWellAutoCalibrate_exportStatus){
        		EE_READ(snum, (uchar *) &(AutoCalBuf[2]));

        		if(AmuletWellAutoCalibrate_exportStatus == 1){
					AutoCalBuf[1] = '1';

					if(*AmuletWellAutoCalibrate_exportFirstPeak){
						memcpy(&(AutoCalBuf[8]), AmuletWellAutoCalibrate_exportFirstPeak, strlen(AmuletWellAutoCalibrate_exportFirstPeak));
					}

					if(*AmuletWellAutoCalibrate_exportFirstPeakDev){
						memcpy(&(AutoCalBuf[18]), AmuletWellAutoCalibrate_exportFirstPeakDev, strlen(AmuletWellAutoCalibrate_exportFirstPeakDev));
					}

					if(*AmuletWellAutoCalibrate_exportSecondPeak){
						memcpy(&(AutoCalBuf[28]), AmuletWellAutoCalibrate_exportSecondPeak, strlen(AmuletWellAutoCalibrate_exportSecondPeak));
					}

					if(*AmuletWellAutoCalibrate_exportSecondPeakDev){
						memcpy(&(AutoCalBuf[38]), AmuletWellAutoCalibrate_exportSecondPeakDev, strlen(AmuletWellAutoCalibrate_exportSecondPeakDev));
					}

					if(*AmuletWellAutoCalibrate_exportFWHM){
						memcpy(&(AutoCalBuf[48]), AmuletWellAutoCalibrate_exportFWHM, strlen(AmuletWellAutoCalibrate_exportFWHM));
					}

					if(*AmuletWellAutoCalibrate_exportVolts){
						memcpy(&(AutoCalBuf[58]), AmuletWellAutoCalibrate_exportVolts, strlen(AmuletWellAutoCalibrate_exportVolts));
					}

					if(*AmuletWellAutoCalibrate_exportThreshold){
						memcpy(&(AutoCalBuf[68]), AmuletWellAutoCalibrate_exportThreshold, strlen(AmuletWellAutoCalibrate_exportThreshold));
					}

					if(*AmuletWellAutoCalibrate_exportGain1){
						memcpy(&(AutoCalBuf[78]), AmuletWellAutoCalibrate_exportGain1, strlen(AmuletWellAutoCalibrate_exportGain1));
					}

					if(*AmuletWellAutoCalibrate_exportGain2){
						memcpy(&(AutoCalBuf[88]), AmuletWellAutoCalibrate_exportGain2, strlen(AmuletWellAutoCalibrate_exportGain2));
					}

					if(*AmuletWellAutoCalibrate_exportGain){
						memcpy(&(AutoCalBuf[98]), AmuletWellAutoCalibrate_exportGain, strlen(AmuletWellAutoCalibrate_exportGain));
					}

					if(*AmuletWellAutoCalibrate_exportZero){
						memcpy(&(AutoCalBuf[108]), AmuletWellAutoCalibrate_exportZero, strlen(AmuletWellAutoCalibrate_exportZero));
					}
        		}else if(AmuletWellAutoCalibrate_exportStatus == 2){
        			AutoCalBuf[1] = '2';

        			if(*AmuletWellAutoCalibrate_exportVolts){
        				memcpy(&(AutoCalBuf[58]), AmuletWellAutoCalibrate_exportVolts, strlen(AmuletWellAutoCalibrate_exportVolts));
        			}

        			if(*AmuletWellAutoCalibrate_exportThreshold){
        				memcpy(&(AutoCalBuf[68]), AmuletWellAutoCalibrate_exportThreshold, strlen(AmuletWellAutoCalibrate_exportThreshold));
        			}
        		}

        		sprintf(&(AutoCalBuf[118]), "%d", (AmuletWellAutoCalibrate_exportSequence & 0xFFFF));
        		for(i=118; i<128; i++){
        			if(AutoCalBuf[i] == 0) AutoCalBuf[i] = ' ';
        		}
        	}else{
        		AutoCalBuf[1] = '0';
        	}
        	AutoCalBuf[128] = checksum(&(AutoCalBuf[1]), 127);
        	AutoCalBuf[129] = '#';
        	send_to_pc(AutoCalBuf);
        	break;

        default:
            //illegal command
            error_msg(COMMAND_ERROR);
            break;
        }
    }

/**
 * \details Parse and fulfill extended command string
 * \param msg Pointer to Command string, Command Byte 1 = Update Calibrator Software
 * \param len Command length
 * \returns None
 */
static void parse_extended(char *msg, short len){
	char message[60], titlestring[50], messagestring[104];
	char ok[4];
	long filesize;
	unsigned char filesizearray[5];
	unsigned char *puc;
	unsigned char cksum;
	char pathfile[60];
	char path[60];
	char file[60];
	short i;
	FILINFO fileInfo;
	char longFileName[100];

	switch(msg[0]){
		case '1': // Update Calibrator Software from PC
			msg++;
			strncpy(message, msg, len - 3);
			message[len - 3] = 0;
			PCComm_transferFileSize = atol(message);
			UsbUpdate_seqNumber = 0;
			UsbUpdate_bufferEnd = 0;
			UsbUpdate_transferFinished = FALSE;

			//f_chdir("\\");
			f_chdrive(0);
			f_chdir("/");

			//if(f_chdir("temp") != F_NO_ERROR){
			if(f_chdir("temp") != FR_OK){

				//f_mkdir("temp");
				f_mkdir("temp");

				//f_chdir("\\");
				f_chdir("/");

				//if(f_chdir("temp") != F_NO_ERROR){
				if(f_chdir("temp") != FR_OK){
					printf("Unable to make temp\n");
				}else{
					//UsbUpdate_fileHandle = f_open("temp.bin", "w");
					f_open(&UsbUpdate_fileObject, "temp.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

					//f_close(UsbUpdate_fileHandle);
					f_close(&UsbUpdate_fileObject);
				}
			}

			//if(mmc_file_exists("temp.bin") == 0) f_delete("temp.bin");
			fileInfo.lfname = longFileName;
			fileInfo.lfsize = 100;
			if(f_stat("temp.bin", &fileInfo) == FR_OK) f_unlink("temp.bin");

			//UsbUpdate_fileHandle = f_open("temp.bin", "w");
			f_open(&UsbUpdate_fileObject, "temp.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

			current.pccomm = PC_COMM_USB_UPDATE;
			ok[0] = START_CHAR;
			ok[1] = 'O';
			ok[2] = END_CHAR;
			ok[3] = 0;
			send_to_pc(ok);
			get_amulet_message(L_SOFTWARE_UPDATE, titlestring);    // "Software Update"
			get_amulet_message(L_SOFTWARE_DOWNLOAD_IN_PROGRESS, messagestring);    // "Software download is in progress\nPlease do not turn off calibrator"
			//Amulet_DisplayNotification("Software Update", "Software download is in progress\nPlease do not turn off calibrator", FALSE);
			Amulet_DisplayNotification(titlestring, messagestring, FALSE);
			break;

		case '2': // Upload File to PC
			msg++;
			if(*msg == '\\'){
				strncpy(pathfile, msg, len - 3);
				pathfile[len - 3] = 0;
			}else{
				strncpy(&(pathfile[1]), msg, len - 3);
				pathfile[len - 2] = 0;
				pathfile[0] = '\\';
			}

			strcpy(path, pathfile);
			len = strlen(path);
			for(i=len - 1; i >=0; i--){
				if(path[i] == '\\'){
					path[i] = 0;
					strcpy(file, &(path[i+1]));
					break;
				}
			}

			//f_chdir("\\");
			f_chdrive(0);
			f_chdir("/");

			//if(f_chdir(path) == F_NO_ERROR){
			if(f_chdir(path) != FR_OK){

				//if(mmc_file_exists(file) == 0){
				fileInfo.lfname = longFileName;
				fileInfo.lfsize = 100;
				if(f_stat(file, &fileInfo) == FR_OK){
					ulLowLevelHead = 0;
					ulLowLevelTail = 0;
					//filesize = f_filelength(file);
					filesize = fileInfo.fsize;
					FileTransfer_filesize = filesize;

					//FileTransfer_pointer = f_open(file, "r");
					f_open(&FileTransfer_fileObject, file, FA_READ);

					FileTransfer_lastBlock = FALSE;
					FileTransfer_pccomm = current.pccomm;
					current.pccomm = PC_COMM_USB_UPLOAD_FILE;
				}else{
					filesize = -6;
				}
			}else{
				filesize = -5;
			}
			puc = (unsigned char *) &filesize;

			filesizearray[0] = *puc++;
			cksum = filesizearray[0];
			filesizearray[1] = *puc++;
			cksum = cksum + filesizearray[1];
			filesizearray[2] = *puc++;
			cksum = cksum + filesizearray[2];
			filesizearray[3] = *puc;
			cksum = cksum + filesizearray[3];
			filesizearray[4] = (~cksum) + 1;
			USB_Send_Data_LowLevel_num(filesizearray, 5);
			break;

		case '3': // Download File from PC
			break;

		case '4': // Send the List of Folders to PC
			break;

		case '5': // Send the List Files to PC
			break;
	}
}


    //send error message to PC
/**
 * \details Send error message to PC
 * \param error Error code
 * \returns None
 */
    static void error_msg(short error)
    {
        char str[10];
        short i;

        for(i = 0; i < 10; i++)
            str[i] = ' ';

        str[0] = START_CHAR;
        str[1] = 'e';
        str[2] = (char)error + '0';
        str[7] = END_CHAR;
        str[8] = '\0';
        send_to_pc(str);

    }

/**
 * \details Parse and execute the Change Nuclide command
 * \param msg Pointer to command string
 * \returns None
 */
    static void parse_change_nuclide(char *msg)
    {
        char str[20];
        char len;
        short ch_num;
        char cs;
        short but;

        str[0] = START_CHAR;
        //len is 4 for return of isotope data
        len = 4;
        str[1] = len + 0x41;
        //done character
        str[2] = 'D';
        //copy original message
        strncpy(&str[3],&msg[0],3);

        //get chamber number
        ch_num = (short)(msg[1] - '1');  //1 is first chamber
        //if chamber doesn't exist, error
        if(!chamber[ch_num].exists)
        {
            error_msg(CHAMBER_ERROR);
            return;
        }

        //change the nuclide
        but = (short)(msg[2] - '1');
        getiso(ch_num,but);

        //calculate checksum
        cs = checksum(&str[1],5);
        str[6] = cs;
        //put in end character
        str[7] = END_CHAR;
        str[8] = '\0';
        //send to PC
        send_to_pc(str);


    }
/**
 * \details Parse and execute the Change User Nuclide command
 * \param msg Pointer to command string
 * \returns None
 */
    static void parse_change_user_nuclide(char *msg)
    {
        char str[20];
        char len;
        short ch_num;
        char cs;
        short but;

        str[0] = START_CHAR;
        //len is 4 for return of isotope data
        len = 4;
        str[1] = len + 0x41;
        //done character
        str[2] = 'D';
        //copy original message
        strncpy(&str[3],&msg[0],3);

        //get chamber number
        ch_num = (short)(msg[1] - '1');  //1 is first chamber
        //if chamber doesn't exist, error
        if(!chamber[ch_num].exists)
        {
            error_msg(CHAMBER_ERROR);
            return;
        }

        //change the nuclide
        but = (short)(msg[2] - '1');
        if(!pc_getuser(ch_num,but))
           str[3] = 'u';

        //calculate checksum
        cs = checksum(&str[1],5);
        str[6] = cs;
        //put in end character
        str[7] = END_CHAR;
        str[8] = '\0';
        //send to PC
        send_to_pc(str);

    }
/**
 * \details Parse and execute the Update User Key command
 * \param msg Pointer to command string
 * \returns None
 */
    static void parse_update_user_key(char *msg)
    {
        char str[20];
        char len;
        char cs;
        short but;
        char name[8];

        str[0] = START_CHAR;
        //len is 9 for return of isotope name
        len = 9;
        str[1] = len + 0x41;
        //done character
        str[2] = 'D';
        //copy original message
        strncpy(&str[3],&msg[0],2);

        //get name for key
        but = msg[1] - '1';
        pc_get_user_key(but,name);
        strncpy(&str[5],name,6);

        //calculate checksum
        cs = checksum(&str[1],10);
        str[11] = cs;
        //put in end character
        str[12] = END_CHAR;
        str[13] = '\0';
        //send to PC
        send_to_pc(str);

    }

    //parse change main chamber command
/**
 * \details Parse ane execute the Change Chamber command
 * \param msg Pointer to command string
 * \returns None
 */
    static void parse_change_chamber(char *msg)
    {
        char str[20];
        char len;
        short ch_num;
        char cs;

        str[0] = START_CHAR;
        //len is 3 for return of chamber data
        len = 3;
        str[1] = len + 0x41;
        //done character
        str[2] = 'D';
        //copy original message
        strncpy(&str[3],&msg[0],2);

        //get chamber number
        ch_num = (short)(msg[1] - '1');  //1 is first chamber
        //if chamber doesn't exist, error
        if(!chamber[ch_num].exists)
        {
            error_msg(CHAMBER_ERROR);
            return;
        }

        //calculate checksum
        cs = checksum(&str[1],4);
        str[5] = cs;
        //put in end character
        str[6] = END_CHAR;
        str[7] = '\0';
        //send to PC
        //uart_write(str,U_PC);
        send_to_pc(str);

        //change the chamber
        pc_change_chamber(ch_num);

    }

/**
 * \details Change current chamber on the calibrator
 * \param ch_num Chamber number
 * \returns None
 */
    static void pc_change_chamber(short ch_num)
    {

        current.main_chamber = ch_num;

        //initialize display
        measurement[ch_num].idec = measurement[ch_num].idecmin;
        measurement[ch_num].act0 = 0.;
        // Removed:
        //measurement[ch_num].syst0 = -1;
        measurement[ch_num].kun0 = 0;
        strcpy(&measurement[ch_num].actstr0[0],"xxxxxx");
        measurement[ch_num].future.dosetime = NO_TIME;
        //id_init();
        erase_lines(16,48,1);
        erase_lines(Y_FUT_TIME,64 - Y_FUT_TIME,1);

        disp_cal(measurement[ch_num].response);
        display_chamber();
        if(!chamber[ch_num].calkey)
            disp_nuclide();

        pc_ret = PC_START_FLAG;   //will set ch_num to current.main_chamber in meas screen


    }

    //parse read chamber command
/**
 * \details Parse and execute the Read Chamber command
 * \param msg Pointer to command string
 * \returns None
 */
    static void parse_read_chamber(char *msg)
    {
        short ch_num;

        //get chamber number
        ch_num = (short)(msg[1] - '1');  //1 is first chamber
        //if chamber doesn't exist, error
        if(!chamber[ch_num].exists)
        {
            error_msg(CHAMBER_ERROR);
            return;
        }
        PCComm_currentPCCommChamber = ch_num;
        //read the chamber
        pc_read_chamber(ch_num);

    }

#define REM_MULT 'X'
#define REM_DIV 0xfd;
    //send chamber reading to PC
/**
 * \details Send chamber reading to PC
 * \param ch_num Chamber number
 * \returns None
 */
    static void pc_read_chamber(short ch_num)
    {

        char buffer[30];
        uchar cs;
        short i;
        short len;
        char number_string[11];
        char unit_string[11];
        int number_length;

        for(i = 0; i < 24; i++)
            buffer[i] = ' ';
        buffer[24] = '\0';


        //byte  0 is START_CHAR
        buffer[0] = START_CHAR;

        //len is 18 for reading chamber
        len = 18;
        buffer[1] = len + 0x41;


        //byte 2 is D (done), byte 3 is R (read)
        if(pc_freeze_flag)
            strncpy(&buffer[2],"BR",2);
        else
            strncpy(&buffer[2],"DR",2);

        //byte 4 is chamber number: from 1 to 8
        buffer[4] = (char)ch_num + '1';

        //bytes 5 - 12 is nuclide name or cal #
        if(chamber[ch_num].calkey)
        {

            strncpy(&buffer[5],&chamber[ch_num].calstrng[0],strlen(&chamber[ch_num].calstrng[0]));
            for(i = 6; i <= 8; i++)
            {

                if(buffer[i] == DIVSIGN)
                    buffer[i] = REM_DIV;
                if(buffer[i] == MULT)
                    buffer[i] = REM_MULT;

            }
        }
        else
            strncpy(&buffer[5], chamber[ch_num].nucdata.name, strlen(chamber[ch_num].nucdata.name));

        //bytes 13 - 18 is activity string
        splitoutunits(&measurement[ch_num].actstr[0], number_string, unit_string);
        trim_and_shrink(number_string);
        number_string[6] = 0;
        number_length = strlen(number_string);
        if((number_length >= 1) && (number_length <=6)){
        	strncpy(&buffer[19 - number_length], number_string, number_length);
        }

        //byte 19 is unit in ascii
        if(measurement[ch_num].over_flag)
            buffer[19] = '0';
        else
            buffer[19] = measurement[ch_num].kun + '0';

        //byte 20 is checksum
        cs = checksum(&buffer[1],19);
        buffer[20] = cs;

        //byte 21 is END CHAR
        buffer[21] = END_CHAR;
        buffer[22] = '\0';


        send_to_pc(buffer);


    }


    //for entering Daily, Background, Bias Voltage
/**
 * \details Parse and execute Daily, Background, and Bias Voltage test
 * \param test_num 0 = Daily Test, 1 = Background Test, 2 = Bias Voltage Test, 3 = Regular Reading
 * \returns None
 */
    static void parse_test(short test_num)
    {
        char str[20];
        char len;
        char cs;
        short ch_num = current.main_chamber;

        short ch_type;

        ch_type = chamber_type(ch_num);

        str[0] = START_CHAR;
        //len is 4 for return
        len = 4;
        str[1] = len + 0x41;
        //done character
        str[2] = 'D';

        //original message
        switch(test_num)
        {
        case PC_DAILY:
            str[3] = 'D';
            break;
        case PC_BKG:
            str[3] = 'B';
            break;
        case PC_BIAS:
            str[3] = 'V';
            break;
        case PC_ACQ:
            str[3] = 'm';
            break;
        }

        //send chamber #
        str[4] = ch_num + '1';

        //send chamber type
        if(ch_type == R_CHAMB)
            str[5] = 'R';
        else if(ch_type == P_CHAMB)
            str[5] = 'P';
        else if(ch_type == B_CHAMB)
            str[5] = 'B';
        else if(ch_type == ONE_DOT_EIGHT_CHAMB)
            str[5] = '8';
        else if(ch_type == C_CHAMB)
            str[5] = 'C';
        else if(ch_type == K_CHAMB)
            str[5] = 'K';
        else
        	str[5] = 'X';
        //calculate checksum
        cs = checksum(&str[1],5);
        str[6] = cs;

        //byte 7 is END CHAR
        str[7] = END_CHAR;
        str[8] = '\0';
        //send to PC
        send_to_pc(str);

        //do test
        chamber[ch_num].control = CONTROL_PC;
        switch (test_num)
        {
        case PC_DAILY:
            set_meas_screen(FALSE);
            daily();
            break;
        case PC_BKG:
            //measure background
            measure_bkg(0);
            break;
        case PC_BIAS:
            measure_bias(0);
            break;
        case PC_ACQ:
            break;
        }



        pc_ret = PC_START_FLAG;   //will redo measurement screen

    }

/**
 * \details Send a Continue/Accept message
 * \param ca C = Continue, A = Accept
 * \returns None
 */
    static void parse_continue_accept(char ca)
    {
        char str[20];
        char len;
        char cs;

        strcpy(str,"                ");

        str[0] = START_CHAR;
        //len is 2 for return
        len = 2;
        str[1] = len + 0x41;
        //done character
        str[2] = 'D';
        //original message C or A
        str[3] = ca;

        //calculate checksum
        cs = checksum(&str[1],3);
        str[4] = cs;
        //put in end character
        str[7] = END_CHAR;
        str[8] = '\0';
        //send to PC
        send_to_pc(str);

        if (ca == 'C')
            pc_ret = PC_CONTINUE;
        else
            pc_ret = PC_ACCEPT;

    }


    //for reading Zero, Bkg, System test
/**
 * \details Send results of the Zero, Background, Bias Voltage Test
 * \param tchar b = Background Test, v = Bias Voltage Test, Z = Zero Test
 * \returns None
 */
    static void pc_read_test(char tchar)
    {

        char buffer[30];
        uchar cs;
        int i;
        int len;
        short ch_num = current.main_chamber;

        for(i = 0; i < 24; i++)
            buffer[i] = ' ';


        //byte  0 is START_CHAR
        buffer[0] = START_CHAR;

        //len is 12
        len = 12;
        buffer[1] = len + 0x41;


        //byte 2 is D (done)
        buffer[2] = 'D';

        //byte 3 is Z or b or v according to tchar
        buffer[3] = tchar;

        //status
        switch (tchar)
        {
        case 'b':
            buffer[4] = (char)bkgdata[ch_num].status;
            break;
        case 'v':
            buffer[4] = (char)biasdata[ch_num].status;
            break;
        case 'Z':
            buffer[4] = (char)zerodata[ch_num].status;
            break;
        }

        //bytes 5 - 13 is zero or background measurement  or Bias  string
        strncpy(&buffer[5],&measurement[ch_num].actstr[0],9);


        //byte 14 is checksum
        cs = checksum(&buffer[1],13);
        buffer[14] = cs;

        //byte 15 is END CHAR
        buffer[15] = END_CHAR;
        buffer[16] = '\0';

        send_to_pc(buffer);


    }
/**
 * \details Send calibrator serial number
 * \returns None
 */
    static void pc_send_sn(void)
    {
        char str[20];
        char len;
        char cs;
        char snum[6];

        str[0] = START_CHAR;
        //len is 2 for return
        len = 2;
        str[1] = len + 0x41;
        //done character
        str[2] = 'D';
        //send original message S
        str[3] = 'S';

        //read the serial number
        EE_READ(snum,(uchar *)snum);
        //put it into bytes 4-9
        strncpy(&str[4],snum,6);

        //byte 10 is checksum
        cs = checksum(&str[1],11);
        str[10] = cs;

        //byte 11 is END CHAR
        str[11] = END_CHAR;
        str[12] = '\0';

        send_to_pc(str);


    }

/**
 * \details Send screen print acknowledge and call screen print function
 * \returns None
 */
    static void pc_screen_print(void)
    {
        char str[20];
        char len;
        char cs;

        str[0] = START_CHAR;
        //len is 2 for return
        len = 2;
        str[1] = len + 0x41;
        //done character
        str[2] = 'D';

        //original message
        str[3] = 'P';
        //calculate checksum
        cs = checksum(&str[1],3);
        str[4] = cs;
        //put in end character
        str[5] = END_CHAR;
        str[6] = '\n';
        str[7] = '\0';
        //send to PC
        send_to_pc(str);

        screen_print(0,63);
    }

/**
 * \details Send Configuration message
 * \returns None
 */
    static void pc_get_configuration(void)
    {

        char buffer[30];
        uchar cs;
        short i;
        short len;

        for(i = 0; i < 24; i++)
            buffer[i] = ' ';


        //byte  0 is START_CHAR
        buffer[0] = START_CHAR;

        //len is 12 for configuration
        len = 12;
        buffer[1] = len + 0x41;


        //byte 2 is D (done), byte 3 is G (Get)
        strncpy(&buffer[2],"DG",2);

        //set bytes 4 - 13 to none as default
        for(i = 4; i <= 13; i++)
            buffer[i] = 'N';

        for(i = 0; i < max_chambers; i++)
        {
            if(chamber[i].exists)
            {
                if(chamber_type(i) == R_CHAMB)
                    buffer[i + 4] = 'R';
                else if(chamber_type(i) == P_CHAMB)
                    buffer[i + 4] = 'P';
                else if(chamber_type(i) == B_CHAMB)
                	buffer[i + 4] = 'B';
                else if(chamber_type(i) == ONE_DOT_EIGHT_CHAMB)
                	buffer[i + 4] = '8';
                else if(chamber_type(i) == C_CHAMB)
                	buffer[i + 4] = 'C';
                else if(chamber_type(i) == K_CHAMB)
                	buffer[i + 4] = 'K';
                else
                	buffer[i + 4] = 'X';
            }
        }


        //byte 14 is checksum
        cs = checksum(&buffer[1],13);
        buffer[14] = cs;

        //byte 15 is END CHAR
        buffer[15] = END_CHAR;
        buffer[16] = '\0';

        send_to_pc(buffer);


    }


/**
 * \details Send Data Check message
 * \returns None
 */
    static void pc_data_check(void)
    {
        char str[20];
        char len;
        char cs;
        short i;
        char *acMsg, *acMsg2, *hex_str;

        for(i = 0; i < 20; i++)
            str[i] = ' ';

        str[0] = START_CHAR;
        //len is 3 for return
        len = 3;
        str[1] = len + 0x41;
        //done character & command d
        strncpy(&str[2],"Dd",2);

        if(get_bias_delay_flag())
            str[4] = 'b';
        else
        {
        	hex_str = get_hex_str();
        	if(get_okdata()){
				acMsg = malloc(20);
				acMsg2 = malloc(20);
				get_amulet_message_with_language(L_CAPS_OK, acMsg, ENGLISH);    // "OK"
				get_amulet_message_with_language(L_CAPS_OK, acMsg2, SPANISH);    // "OK"
				DB_UpdateDailyTestDataCheck(AmuletDailyMenu_ChamberDailyTestID, acMsg, acMsg2, FALSE, hex_str, TRUE);
				free(acMsg2);
				free(acMsg);
			}else{
				acMsg = malloc(30);
				acMsg2 = malloc(30);
				get_amulet_message_with_language(L_ERROR, acMsg, ENGLISH);    // "ERROR"
				strcat(acMsg, ", ");
				strcat(acMsg, hex_str);
				get_amulet_message_with_language(L_ERROR, acMsg2, SPANISH);    // "ERROR"
				strcat(acMsg2, ", ");
				strcat(acMsg2, hex_str);
				DB_UpdateDailyTestDataCheck(AmuletDailyMenu_ChamberDailyTestID, acMsg, acMsg2, TRUE, hex_str, TRUE);
				free(acMsg2);
				free(acMsg);
			}

            //G or B according to okdata
            if (get_okdata())
                str[4] = 'G';
            else
                str[4] = 'B';
        }

        //calculate checksum
        cs = checksum(&str[1],4);
        str[5] = cs;
        //put in end character
        str[7] = END_CHAR;
        str[8] = '\0';


        //send to PC
        send_to_pc(str);


    }

extern short num_daily;
/**
 * \details Send Number of Test Sources Message
 * \returns None
 */
    static void pc_num_sources(void)
    {
        char str[20];
        char len;
        char cs;
        short i;

        for(i = 0; i < 20; i++)
            str[i] = ' ';

        str[0] = START_CHAR;
        //len is 3 for return
        len = 3;
        str[1] = len + 0x41;
        //done character & command N
        strncpy(&str[2],"DN",2);

        //number of daily sources
        str[4] = (char)num_daily + '0';
        if(num_daily > 0) AmuletGenericYesNo_LatchedDailyTestID = AmuletDailyMenu_ChamberDailyTestID;

        //calculate checksum
        cs = checksum(&str[1],4);
        str[5] = cs;
        //put in end character
        str[7] = END_CHAR;
        str[8] = '\0';
        //send to PC
        send_to_pc(str);

        //set return to indicated num_daily sent
        pc_ret = PC_NUM_DAILY;

        /*if(num_daily == 0){
        	if(CurrentPageStack() != MAINSCREEN_HTM){
        		SetAmuletHTML(MAINSCREEN_HTM);
        		ClearPageStack();
        		PushPageStack(MAINSCREEN_HTM);
        	}
        }*/
    }


extern ACCDATA acc_data[5];
extern short num_daily;
/**
 * \details Send Test Source Information message
 * \param num Test source number
 * \returns None
 */
    static void pc_get_source(char num)
    {
        char buffer[40];
        uchar cs;
        short i;
        short len;
        short test_num;
        short ch_num = current.main_chamber;
        char fixedlen[20];
        short index;
        bool zerodetected;

        for(i = 0; i < 36; i++)
            buffer[i] = ' ';


        //byte  0 is START_CHAR
        buffer[0] = START_CHAR;

        len = 26;
        buffer[1] = len + 0x41;


        //byte 2 is D (done), byte 3 is s
        strncpy(&buffer[2],"Ds",2);

        //byte 4 is source  number
        buffer[4] = num;


        i = (short)(num - '1'); //start at index 0

        zerodetected = FALSE;
        for(index=0; index<10; index++){
        	if(!zerodetected){
        		if(acc_data[i].sn[index] != 0){
        			fixedlen[index] = acc_data[i].sn[index];
        		}else{
        			zerodetected = TRUE;
        			fixedlen[index] = ' ';
        		}
        	}else{
        		fixedlen[index] = ' ';
        	}
        }
        //strncpy(&buffer[5],&acc_data[i].sn[0],10);
        strncpy(&buffer[5], fixedlen, 10);

        zerodetected = FALSE;
        for(index=0; index<5; index++){
        	if(!zerodetected){
        		if(acc_data[i].nucname[index] != 0){
        			fixedlen[index] = acc_data[i].nucname[index];
        		}else{
        			zerodetected = TRUE;
        			fixedlen[index] = ' ';
        		}
        	}else{
        		fixedlen[index] = ' ';
        	}
        }
        //strncpy(&buffer[15],&acc_data[i].nucname[0],5);
        strncpy(&buffer[15], fixedlen, 5);

        zerodetected = FALSE;
        for(index=0; index<6; index++){
        	if(!zerodetected){
        		if(acc_data[i].test_res[ch_num].pract[index] != 0){
        			fixedlen[index] = acc_data[i].test_res[ch_num].pract[index];
        		}else{
        			zerodetected = TRUE;
        			fixedlen[index] = ' ';
        		}
        	}else{
        		fixedlen[index] = ' ';
        	}
        }
        //strncpy(&buffer[20],&acc_data[i].test_res[ch_num].pract[0],6);
        strncpy(&buffer[20], fixedlen, 6);
        buffer[26]= '0' + (char)acc_data[i].test_res[ch_num].pr_kun;

        if(acc_data[i].const_source)
            buffer[27] = 'C';

        //byte 28 is checksum
        cs = checksum(&buffer[1],27);
        buffer[28] = cs;



        buffer[31] = END_CHAR;
        buffer[32] = '\0';


        send_to_pc(buffer);

        test_num =(short)(num - '0');

        //set return to indicated sources sent if it is the last sources
        if(test_num == num_daily)
            pc_ret = PC_ACC_SOURCES;



    }


    //send accuracy test measurement to PC
/**
 * \details Send Accuracy Test Measurement message
 * \param num Test source number
 * \returns None
 */
    static void pc_read_accuracy(char num)
    {

        char buffer[30];
        uchar cs;
        short i;
        short len;
        short ch_num = current.main_chamber;

        for(i = 0; i < 24; i++)
            buffer[i] = ' ';
        buffer[24] = '\0';


        //byte  0 is START_CHAR
        buffer[0] = START_CHAR;

        //len is 15 for reading accuracy
        len = 15;
        buffer[1] = len + 0x41;


        //byte 2 is D (done), byte 3 is a
        strncpy(&buffer[2],"Da",2);

        //byte 4 is source  number
        buffer[4] = num;


        i = (short)(num - '1'); //start at index 0

        if(acc_data[i].test_res[ch_num].ms_kun != -1)
        {
            //bytes 5 - 10 is activity string
            strncpy(&buffer[5],&acc_data[i].test_res[ch_num].msact[0],6);

            //byte 11 is unit in ascii
            buffer[11]= '0' + (char)acc_data[i].test_res[ch_num].ms_kun;
            //bytes 12 - 16 is % variation
            strncpy(&buffer[12],&acc_data[i].test_res[ch_num].var[0],5);
        }

        //byte 17 is checksum
        cs = checksum(&buffer[1],16);
        buffer[17] = cs;

        //byte 18 is END CHAR
        buffer[18] = END_CHAR;

        buffer[19] = '\0';

        send_to_pc(buffer);


    }


/**
 * \details Strip command string of the header, footer and do a verify checksum
 * \param str Pointer to command string
 * \returns True = Command string correctly formatted, False = Command string is corrupt
 */
    static bool get_command(char *str)
    {

        char *s;
        char ch;
        short i;
        bool waiting;
        uchar usb_in[64];
        bool end;

        //wait for command
        s = str;

        end = FALSE;    //assume no END_CHAR;

        if(current.pccomm == PC_COMM_RS232)
        {
            for(i = 0; i < 64; i++)
            {
            	// TEMPDEV
                ch = (char)uart_getchar(TRUE,U_PC);
            	//ch = '@';

                //if waiting for command and getting @, just exit
                if(i == 0 && ch == '@')
                {
                    return FALSE;
                }

                *s++ = ch;
                if(ch == END_CHAR)
                {
                    *s = '\0';
                    end = TRUE;
                    break;
                }
            }

            return end;
        }

        //USB
        //wait for command from USB

        do
        {
            service_watchdog();
            waiting = is_usb_char_waiting();
        }while(!waiting);

        USB_Get_Data(usb_in);
        for(i = 0; i < 64; i++)
        {
            ch = (char)usb_in[i];
            if(i == 0 && ch == '@')
            {
                return FALSE;
            }

            *s++ = ch;
            if(ch == END_CHAR)
            {
                *s = '\0';
                end = TRUE;
                break;
            }

        }
        return end;


    }

/**
 * \details Send a Null terminated string to PC
 * \param str Pointer to string
 * \returns None
 */
    static void send_to_pc(char *str)
    {
        bool ok;
        short count;

        if(current.pccomm == PC_COMM_RS232) {
        	// TEMPDEV
            uart_write(str,U_PC);
		}
        else
        {
            count = 0;
            do
            {
                ok = USB_Send_Data((uchar *)str);
                if(!ok)
                {
                    delay_msec(10);
                    if(count == 100)
                    {
                        //printf("can't sent USB\r\n");
                        ok = TRUE;
                    }
                    ++count;
                }
            }while(!ok);
        }

    }
/**
 * \details Set the PC Freeze flag
 * \param flag Flag value
 * \returns None
 */
    void set_pc_freeze_flag(bool flag)
    {
        pc_freeze_flag = flag;
    }

/**
 * \details Set Nuclide as defined by the aux hotkeys
 * \param ch_num Chamber number
 * \param but Hotkey index - 4
 * \returns True = Success, False = Error
 */
    static bool pc_getuser(short ch_num, short but)
    {
        short nuc_index;
        short ch_type;

        ch_type = chamber_type(ch_num);
        if(m_ucHotKeyNuclideID2[ch_type][but+4]==255) nuc_index = -1;
        else nuc_index = m_ucHotKeyNuclideID2[ch_type][but+4];

        //nuc_index = user_key_index(but);
        if (nuc_index == -1)
            return(FALSE);
        set_nuclide_data(nuc_index, ch_num);
        return(TRUE);
    }
/**
 * \details Get Nuclide name for an aux hotkey
 * \param but Hotkey index - 1
 * \param name Pointer to buffer, which receives the Nuclide name
 * \returns None
 */
    static void pc_get_user_key(short but,char *name)
    {
        short nuc_index;
        short ch_type;
        int i;

        strcpy(name,"      ");

        ch_type = chamber_type(PCComm_currentPCCommChamber);
        if(m_ucHotKeyNuclideID2[ch_type][but+4]==255) nuc_index = -1;
        else nuc_index = m_ucHotKeyNuclideID2[ch_type][but+4];

        //nuc_index = user_key_index(but);

        if (nuc_index == -1)
            return;

        NuclideData_getName(nuc_index,name);
        for(i=0; i<6; i++){
        	if(!name[i]) name[i] = ' ';
        }
    }
/**
 * \details Get the number of characters in the CDC buffer
 * \returns Number of characters in the buffer
 */
short InCDCUsed(void){
	short returnValue;

	returnValue = InCDCCircularTail - InCDCCircularHead;
	if(returnValue < 0) returnValue += INCDCBUFFERLEN;
	return returnValue;
}
/**
 * \details Get the free space in the CDC buffer
 * \returns Free space as the number of characters
 */
short InCDCFree(void){
	return INCDCBUFFERLEN - InCDCUsed() - 1;
}
/**
 * \details Retrieve a value in the CDC buffer
 * \param BufferIndex Index number from the buffer head
 * \returns Buffer value
 */
char InCDCPeep(short BufferIndex){
	short relativeIndex;

	relativeIndex = InCDCCircularHead + BufferIndex;

	while(relativeIndex >= INCDCBUFFERLEN) relativeIndex -= INCDCBUFFERLEN;
	return InCDCCircularBuffer[relativeIndex];
}
/**
 * \details Search for a value in the CDC buffer
 * \param testFor Value to test for
 * \returns Index from the buffer head or -1, if not found
 */
short InCDCScan(char testFor){
	short i, returnValue;

	if(InCDCUsed() != 0){
		returnValue = -1;
		for(i=0; i<InCDCUsed(); i++){
			if(InCDCPeep(i) == testFor){
				returnValue = i;
				break;
			}
		}
	}else{
		returnValue = -1;
	}

	return returnValue;
}
/**
 * \details Remove a value from the CDC buffer
 * \param testFor Value to remove
 * \returns None
 */
void InCDCStrip(char testFor){
	short location;
	char dump[INCDCBUFFERLEN + 1];

	location = InCDCScan(testFor);
	if(location > 0){
		InCDCPop(dump, location);
	}
}

#ifdef TERMINAL
void pf(const char *format, ...){
	char *buffer;
	va_list ap;

	buffer = malloc(2048);
	va_start(ap, format);
	vsprintf(buffer, format, ap);
	send_to_pc(buffer);
	va_end(ap);
	free(buffer);
}

static void strip_linefeed(char *line){
	int32_t length, i, j;
	char *new;

	length = strlen(line);
	if(length){
		if((new = malloc(length+3)) != NULL){
			memset(new, 0, length+3);
			i=j=0;
			while(line[i] != 0){
				if(line[i] != '\n') new[j++] = line[i];
				i++;
			}
			strcpy(line, new);
			free(new);
		}else pf("Error: new = malloc(length + 2)\r\n");
	}
}

#define PUTTY_BSP	0x7F
#define ASCII_BSP	0x08

static void backspace(char *line){
	int32_t len, line_index, buffer_index;
	char *buffer;

	len = strlen(line);
	if(len){
		buffer = malloc(len + 1);
		if(buffer != NULL){
			strcpy(buffer, line);	// Copy line to buffer
			memset(line, 0, len);   // Clear line

			// Process bsp in buffer and output to line
			line_index = 0;
			for(buffer_index=0; buffer_index<len; buffer_index++){
				if((buffer[buffer_index] != PUTTY_BSP) && (buffer[buffer_index] != ASCII_BSP)) line[line_index++] = buffer[buffer_index];
				else if(line_index) line[--line_index] = 0;
			}

			free(buffer);
		}
	}
}

#define ASCII_SPACE	0x20
#define ASCII_HTAB	0x09
static void trim(char *line){
	int32_t len, index;
	char *buffer;

	len = strlen(line);
	if(len){
		buffer = malloc(len + 1);
		if(buffer != NULL){
			strcpy(buffer, line);	// Copy line to buffer
			memset(line, 0, len);	// Clear line

			// Remove whitespace from beginning
			index = 0;
			while((buffer[index] == ASCII_SPACE) || (buffer[index] == ASCII_HTAB)) index++;
			strcpy(line, &(buffer[index]));

			// Remove whitespaces from end
			len = strlen(line);
			if(len){
				index = len - 1;
				while((index >= 0) && ((line[index] == ASCII_SPACE) || (line[index] == ASCII_HTAB))) line[index--] = 0;
			}

			free(buffer);
		}
	}
}

#define PARSE_DETECT_ARG	0
#define PARSE_DETECT_SPACE 1
static int32_t parse(char *line, void **args, int32_t args_size){
	int32_t return_value, i, line_len, state;

	return_value = 0;

	line_len = strlen(line);

	if(line_len){
		// Replace whitespaces with 0
		for(i=0; i<line_len; i++){
			if((line[i] == ASCII_SPACE) || (line[i] == ASCII_HTAB)) line[i] = 0;
		}

		state = PARSE_DETECT_ARG;
		for(i=0; i<line_len; i++){
			switch(state){
				case PARSE_DETECT_ARG:
					if(line[i] != 0){
						if(return_value < args_size) args[return_value] = (void *) &(line[i]);
						return_value++;
						state = PARSE_DETECT_SPACE;
					}
					break;

				case PARSE_DETECT_SPACE:
					if(line[i] == 0) state = PARSE_DETECT_ARG;
					break;
			}
		}
	}

	return return_value;
}

void from_terminal(char key){
	static FIFO *input_buffer = NULL;
	char *line;
	char serch;
	int32_t length;
	uint32_t tstamp;
	int32_t args_size, arg_num, gain, adc_value;
	void **args = NULL;

	//pf("\r\n%c - %02Xh\r\n", key, key);

	// Restart immediately when detecting ~
	if(key == '~'){
		pf("\rRestarting...\r\n\r\n");
		set_touch_power(0);			// Power Off
		set_touch_reset(0);			// Put Amulet into reset
		tstamp = g_csec_tstamp + 100;
		while(tstamp > g_csec_tstamp) service_watchdog();
		set_touch_power(1);			// Power On
		disable_all_interrupts();
		setup_short_watchdog();
		for(;;);
	}else if(key == '`'){
		pf("\rReset Amulet...\r\n\r\n");
		set_touch_reset(0);
		tstamp = g_csec_tstamp + 100;
		while(tstamp > g_csec_tstamp) service_watchdog();
		set_touch_reset(1);
	}else if(key == '!'){
		pf("\rSleep...\r\n\r\n");
		for(;;){
			if(is_pc_char_waiting()){
				serch = (char)(uart_getchar(TRUE,U_PC));
				if(serch == '~'){
					pf("\rRestarting...\r\n\r\n");
					set_touch_power(0);			// Power Off
					set_touch_reset(0);			// Put Amulet into reset
					tstamp = g_csec_tstamp + 100;
					while(tstamp > g_csec_tstamp) service_watchdog();
					set_touch_power(1);			// Power On
					disable_all_interrupts();
					setup_short_watchdog();
					for(;;);
				}
			}
			service_watchdog();
		}
	}

	// Initially create input buffer
	if(input_buffer == NULL){
		if(capFIFO_Create(&input_buffer, 128, 1)) pf("Error: capFIFO_Create(&input_buffer, 128, 1)\r\n");
	}

	if(input_buffer != NULL){
		if(key != '\r'){
			// Push input character into input buffer, if it is not a CR
			if(capFIFO_Push(input_buffer, &key, 1)) pf("Error: capFIFO_Push(input_buffer, &key, 1)\r\n");
		}else{
			if((line = malloc(130)) != NULL){
				// Remove line from input buffer
				length = capFIFO_Used(input_buffer);
				memset(line, 0, 130);
				if(length) capFIFO_Pop(input_buffer, line, length, 0);

				// Strip LF from line
				strip_linefeed(line);

				// Process backspace
				backspace(line);

				// Trim begin and end from line (whitespace characters, space, tab)
				trim(line);

				if(strlen(line)){
					args_size = 10;
					args = malloc(args_size * sizeof(char *));

					if(args != NULL){
						// parse line, get number of arguments and an array of arguments
						arg_num = parse(line, args, args_size);

						// function for valid arguments, from 1) set, 2) is number, 3) is range
						if(!strcmp((char *) args[0], "adc") && arg_num == 3){
							gain = atoi((char *)args[1]);
							adc_value = atoi((char *) args[2]);
							qspi_sim_adc_gain[current.main_chamber] = gain;
							qspi_sim_adc_value[current.main_chamber] = adc_value;
							pf("\r\nadc gain = %d, adc_value = %d\r\n", gain, adc_value);
						}else if(!strcmp((char *)args[0], "adc_off") && arg_num == 1){
							qspi_sim_adc_gain[current.main_chamber] = -1;
							qspi_sim_adc_value[current.main_chamber] = 0;
							pf("\r\nadc is off\r\n");
						}else pf("Usage: ~ [Restart Coldfire]\r\nUsage: ` [Reset Amulet]\r\nUsage: ! [Pause Coldfire]\r\nUsage: adc GAIN ADC_VALUE [Set ADC Value, GAIN={0|1}, ADC_VALUE={0-256000}]\r\nUsage: adc_off [Turn off \"Set ADC Value\"]\r\n");

						free(args);
					}else pf("Error: args = malloc(%d * %d)\r\n", args_size, sizeof(void *));
				}else pf("Usage: ~ [Restart Coldfire]\r\nUsage: ` [Reset Amulet]\r\nUsage: ! [Pause Coldfire]\r\nUsage: adc GAIN ADC_VALUE [Set ADC Value, GAIN={0|1}, ADC_VALUE={0-256000}]\r\nUsage: adc_off [Turn off \"Set ADC Value\"]\r\n");
				free(line);
			}else pf("Error: line = malloc(130)\r\n");
		}
	}else pf("Error: input_buffer == NULL\r\n");
}
#endif // #ifdef TERMINAL

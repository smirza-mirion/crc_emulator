/**
 * \file
 * \details This file handles calls from the Amulet Test Source Setup Screen
 */
#define PHASE_SETUP_SOURCES_PRE_INIT	0
#define PHASE_SETUP_SOURCES_WAIT		1
#define PHASE_SETUP_SOURCES_CO57DAILY	2
#define PHASE_SETUP_SOURCES_CO60DAILY	3
#define PHASE_SETUP_SOURCES_BA133DAILY	4
#define PHASE_SETUP_SOURCES_CS137DAILY	5
#define PHASE_SETUP_SOURCES_NA22DAILY	6
#define PHASE_SETUP_SOURCES_GE68DAILY	18
#define PHASE_SETUP_SOURCES_CO57CONST	7
#define PHASE_SETUP_SOURCES_CO60CONST	8
#define PHASE_SETUP_SOURCES_BA133CONST 	9
#define PHASE_SETUP_SOURCES_CS137CONST	10
#define PHASE_SETUP_SOURCES_NA22CONST	11
#define PHASE_SETUP_SOURCES_GE68CONST	19
#define PHASE_SETUP_SOURCES_CO57CLEAR	12
#define PHASE_SETUP_SOURCES_CO60CLEAR	13
#define PHASE_SETUP_SOURCES_BA133CLEAR	14
#define PHASE_SETUP_SOURCES_CS137CLEAR	15
#define PHASE_SETUP_SOURCES_NA22CLEAR	16
#define PHASE_SETUP_SOURCES_GE68CLEAR	20
#define PHASE_SETUP_SOURCES_SAVE		17

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"
#include "nuc.h"
#include "message.h"


void GetShortNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
void GetExtendedTimeInfoLanguage(time_t *dtmDateTime, char *acMsg);
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void SetAmuletBackHTML(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void send_accept_cancel(void);

extern int m_iPhase;
extern unsigned char m_ucClear;
extern unsigned char m_ucCheckboxState;
extern time_t clock_time;
extern time_t low_clock_time;
extern CURRENT current;

char m_acSetupSources_Co57SN[11];
char m_acSetupSources_Co60SN[11];
char m_acSetupSources_Ba133SN[11];
char m_acSetupSources_Cs137SN[11];
char m_acSetupSources_Na22SN[11];
char m_acSetupSources_Ge68SN[11];

float m_fSetupSources_Co57Activity;
float m_fSetupSources_Co60Activity;
float m_fSetupSources_Ba133Activity;
float m_fSetupSources_Cs137Activity;
float m_fSetupSources_Na22Activity;
float m_fSetupSources_Ge68Activity;

time_t m_dtmSetupSources_Co57Date;
time_t m_dtmSetupSources_Co60Date;
time_t m_dtmSetupSources_Ba133Date;
time_t m_dtmSetupSources_Cs137Date;
time_t m_dtmSetupSources_Na22Date;
time_t m_dtmSetupSources_Ge68Date;

char m_cSetupSources_Co57Daily;
char m_cSetupSources_Co60Daily;
char m_cSetupSources_Ba133Daily;
char m_cSetupSources_Cs137Daily;
char m_cSetupSources_Na22Daily;
char m_cSetupSources_Ge68Daily;

// 0 - Co57
// 1 - Co60
// 2 - Ba133
// 3 - Cs137
// 4 - Na22
// 5 - Ge68
//char m_cSetupSources_NuclideChanged;

short AmuletSetupSourcesMenu_constancyCh[12];

/**
 * \details Handles the Amulet Screen SetupSources.htm. SetupSources.htm is the setup for the accuracy test sources and the autoconstancy nuclides.
 * \param Amulet_Byte_ID Description
 * \param 100 Display
 * \param 101 Co57 Daily Checkbox State
 * \param 102 Co60 Daily Checkbox State
 * \param 103 Ba133 Daily Checkbox State
 * \param 104 Cs137 Daily Checkbox State
 * \param 105 Na22 Daily Checkbox State
 * \param 107 Ge68 Daily Checkbox State
 * \param 106 Const Source State
 * \param Amulet_String_ID Description
 * \param 90 Accept
 * \param 91 Cancel
 * \param 100 Co57  SN
 * \param 101 Co57  Activity
 * \param 102 Co57  Date
 * \param 103 Co60  SN
 * \param 104 Co60  Activity
 * \param 105 Co60  Date
 * \param 106 Ba133 SN
 * \param 107 Ba133 Activity
 * \param 108 Ba133 Date
 * \param 109 Cs137 SN
 * \param 110 Cs137 Activity
 * \param 111 Cs137 Date
 * \param 112 Na22 SN
 * \param 113 Na22 Activity
 * \param 114 Na22 Date
 * \param 168 Ge68 SN
 * \param 169 Ge68 Activity
 * \param 170 Ge68 Date
 * \param 115 Channel1
 * \param 116 Channel2
 * \param 117 Channel3
 * \param 118 Channel4
 * \param 119 Channel5
 * \param 120 Channel6
 * \param 121 Channel7
 * \param 122 Channel8
 * \param 123 Channel9
 * \param 124 Channel10
 * \param 125 Channel11
 * \param 126 Channel12
 * \param 129_130 Title
 * \param 131 Activity
 * \param 132 Date
 * \param 133 Daily
 * \param 134 Constancy Channels
 * \param 135 Clear Button
 * \param 136_137 Please enter Co57 S/N
 * \param 138_137 Please enter Co57 Activity
 * \param 140_141 Please enter Co57 Calibration Time
 * \param 142_143 Please enter Co60 S/N
 * \param 144_145 Please enter Co60 Activity
 * \param 146_147 Please enter Co60 Calibration Time
 * \param 148_149 Please enter Ba133 S/N
 * \param 150_151 Please enter Ba133 Activity
 * \param 152_153 Please enter Ba133 Calibration Time
 * \param 154_155 Please enter Cs137 S/N
 * \param 156_157 Please enter Cs137 Activity
 * \param 158_159 Please enter Cs137 Calibration Time
 * \param 160_161 Please enter Na22 S/N
 * \param 162_163 Please enter Na22 Activity
 * \param 164_165 Please enter Na22 Calibration Time
 * \param 171_172 Please enter Ge68 S/N
 * \param 173_174 Please enter Ge68 Activity
 * \param 175_176 Please enter Ge68 Calibration Time
 * \param 166_167 Please Select Nuclide
 * \returns None
 */
void AmuletSetupSources_menu(void){
	char acMsg[100];
	short i;
	STAND standard[6];
	NUCDATA output_nuc;
	bool constsourceselected, constchannelselected;
	static char constsource;
	bool min_activity_passed;
	time_t now_time;
	short nuc_index;
	float current_activity, lower_limit, upper_limit;
	char errormessage[104];
	char message[104];
	char temp_message[25];
	char fmt_str[100];
	char *ptr;

	switch(m_iPhase){
		case PHASE_SETUP_SOURCES_PRE_INIT:
			if(m_ucClear){
				m_acSetupSources_Co57SN[0] = 0;
				m_acSetupSources_Co60SN[0] = 0;
				m_acSetupSources_Ba133SN[0] = 0;
				m_acSetupSources_Cs137SN[0] = 0;
				m_acSetupSources_Na22SN[0] = 0;
				m_acSetupSources_Ge68SN[0] = 0;

				m_fSetupSources_Co57Activity = -999;
				m_fSetupSources_Co60Activity = -999;
				m_fSetupSources_Ba133Activity = -999;
				m_fSetupSources_Cs137Activity = -999;
				m_fSetupSources_Na22Activity = -999;
				m_fSetupSources_Ge68Activity = -999;

				m_dtmSetupSources_Co57Date = (time_t)0;
				m_dtmSetupSources_Co60Date = (time_t)0;
				m_dtmSetupSources_Ba133Date = (time_t)0;
				m_dtmSetupSources_Cs137Date = (time_t)0;
				m_dtmSetupSources_Na22Date = (time_t)0;
				m_dtmSetupSources_Ge68Date = (time_t)0;

				m_cSetupSources_Co57Daily = 0;
				m_cSetupSources_Co60Daily = 0;
				m_cSetupSources_Ba133Daily = 0;
				m_cSetupSources_Cs137Daily = 0;
				m_cSetupSources_Na22Daily = 0;
				m_cSetupSources_Ge68Daily = 0;

				EE_READ(constancysource, (uchar *)&constsource);
				for(i=0; i<5; i++) EE_READ(stand[i], (uchar *)(&(standard[i])));
				EE_READ(stand_ge68, (uchar *)(&(standard[5])));
				EE_READ(constancych, (uchar *) AmuletSetupSourcesMenu_constancyCh);

				if(standard[0].caldate != (time_t)0){
					strcpy(m_acSetupSources_Co57SN, standard[0].sn);
					m_fSetupSources_Co57Activity = standard[0].act;
					// Removed:
					//if(m_Standard[0].syst == CI) m_ucSetupSources_Co57Unit = 0;
					//else m_ucSetupSources_Co57Unit = 3;
					m_dtmSetupSources_Co57Date = standard[0].caldate;
					m_cSetupSources_Co57Daily = standard[0].daily;
				}

				if(standard[1].caldate != (time_t)0){
					strcpy(m_acSetupSources_Co60SN, standard[1].sn);
					m_fSetupSources_Co60Activity = standard[1].act;
					// Removed:
					//if(m_Standard[1].syst == CI) m_ucSetupSources_Co60Unit = 0;
					//else m_ucSetupSources_Co60Unit = 3;
					m_dtmSetupSources_Co60Date = standard[1].caldate;
					m_cSetupSources_Co60Daily = standard[1].daily;
				}

				if(standard[2].caldate != (time_t)0){
					strcpy(m_acSetupSources_Ba133SN, standard[2].sn);
					m_fSetupSources_Ba133Activity = standard[2].act;
					// Removed:
					//if(m_Standard[2].syst == CI) m_ucSetupSources_Ba133Unit = 0;
					//else m_ucSetupSources_Ba133Unit = 3;
					m_dtmSetupSources_Ba133Date = standard[2].caldate;
					m_cSetupSources_Ba133Daily = standard[2].daily;
				}

				if(standard[3].caldate != (time_t)0){
					strcpy(m_acSetupSources_Cs137SN, standard[3].sn);
					m_fSetupSources_Cs137Activity = standard[3].act;
					// Removed:
					//if(m_Standard[3].syst == CI) m_ucSetupSources_Cs137Unit = 0;
					//else m_ucSetupSources_Cs137Unit = 3;
					m_dtmSetupSources_Cs137Date = standard[3].caldate;
					m_cSetupSources_Cs137Daily = standard[3].daily;
				}

				if(standard[4].caldate != (time_t)0){
					strcpy(m_acSetupSources_Na22SN, standard[4].sn);
					m_fSetupSources_Na22Activity = standard[4].act;
					// Removed:
					//if(m_Standard[4].syst == CI) m_ucSetupSources_Na22Unit = 0;
					//else m_ucSetupSources_Na22Unit = 3;
					m_dtmSetupSources_Na22Date = standard[4].caldate;
					m_cSetupSources_Na22Daily = standard[4].daily;
				}

				if(standard[5].caldate != (time_t)0){
					strcpy(m_acSetupSources_Ge68SN, standard[5].sn);
					m_fSetupSources_Ge68Activity = standard[5].act;
					// Removed:
					//if(m_Standard[5].syst == CI) m_ucSetupSources_Ge68Unit = 0;
					//else m_ucSetupSources_Ge68Unit = 3;
					m_dtmSetupSources_Ge68Date = standard[5].caldate;
					m_cSetupSources_Ge68Daily = standard[5].daily;
				}

				m_ucClear = 0;
			}

			send_to_amulet_string(100, m_acSetupSources_Co57SN);
			if(m_fSetupSources_Co57Activity == -999) acMsg[0] = 0;
			//Removed:
			//else GetExtendedActivityInfo(m_fSetupSources_Co57Activity, CI, acMsg);
			else format_activity_system2(m_fSetupSources_Co57Activity, acMsg);
			send_to_amulet_string(101, acMsg);
			if(m_dtmSetupSources_Co57Date == 0) acMsg[0] = 0;
			else GetExtendedTimeInfoLanguage(&m_dtmSetupSources_Co57Date, acMsg);
			send_to_amulet_string(102, acMsg);
			if(m_cSetupSources_Co57Daily == FALSE) SetAmuletByte(101, 1);
			else SetAmuletByte(101, 2);

			send_to_amulet_string(103, m_acSetupSources_Co60SN);
			if(m_fSetupSources_Co60Activity == -999) acMsg[0] = 0;
			// Removed:
			//else GetExtendedActivityInfo(m_fSetupSources_Co60Activity, CI, acMsg);
			else format_activity_system2(m_fSetupSources_Co60Activity, acMsg);
			send_to_amulet_string(104, acMsg);
			if(m_dtmSetupSources_Co60Date == 0) acMsg[0] = 0;
			else GetExtendedTimeInfoLanguage(&m_dtmSetupSources_Co60Date, acMsg);
			send_to_amulet_string(105, acMsg);
			if(m_cSetupSources_Co60Daily == FALSE) SetAmuletByte(102, 3);
			else SetAmuletByte(102, 4);

			send_to_amulet_string(106, m_acSetupSources_Ba133SN);
			if(m_fSetupSources_Ba133Activity == -999) acMsg[0] = 0;
			// Removed:
			//else GetExtendedActivityInfo(m_fSetupSources_Ba133Activity, CI, acMsg);
			else format_activity_system2(m_fSetupSources_Ba133Activity, acMsg);
			send_to_amulet_string(107, acMsg);
			if(m_dtmSetupSources_Ba133Date == 0) acMsg[0] = 0;
			else GetExtendedTimeInfoLanguage(&m_dtmSetupSources_Ba133Date, acMsg);
			send_to_amulet_string(108, acMsg);
			if(m_cSetupSources_Ba133Daily == FALSE) SetAmuletByte(103, 5);
			else SetAmuletByte(103, 6);

			send_to_amulet_string(109, m_acSetupSources_Cs137SN);
			if(m_fSetupSources_Cs137Activity == -999) acMsg[0] = 0;
			// Removed:
			//else GetExtendedActivityInfo(m_fSetupSources_Cs137Activity, CI, acMsg);
			else format_activity_system2(m_fSetupSources_Cs137Activity, acMsg);
			send_to_amulet_string(110, acMsg);
			if(m_dtmSetupSources_Cs137Date == 0) acMsg[0] = 0;
			else GetExtendedTimeInfoLanguage(&m_dtmSetupSources_Cs137Date, acMsg);
			send_to_amulet_string(111, acMsg);
			if(m_cSetupSources_Cs137Daily == FALSE) SetAmuletByte(104, 7);
			else SetAmuletByte(104, 8);

			send_to_amulet_string(112, m_acSetupSources_Na22SN);
			if(m_fSetupSources_Na22Activity == -999) acMsg[0] = 0;
			// Removed:
			//else GetExtendedActivityInfo(m_fSetupSources_Na22Activity, CI, acMsg);
			else format_activity_system2(m_fSetupSources_Na22Activity, acMsg);
			send_to_amulet_string(113, acMsg);
			if(m_dtmSetupSources_Na22Date == 0) acMsg[0] = 0;
			else GetExtendedTimeInfoLanguage(&m_dtmSetupSources_Na22Date, acMsg);
			send_to_amulet_string(114, acMsg);
			if(m_cSetupSources_Na22Daily == FALSE) SetAmuletByte(105, 9);
			else SetAmuletByte(105, 10);

			send_to_amulet_string(168, m_acSetupSources_Ge68SN);
			if(m_fSetupSources_Ge68Activity == -999) acMsg[0] = 0;
			//Removed:
			//else GetExtendedActivityInfo(m_fSetupSources_Ge68Activity, CI, acMsg);
			else format_activity_system2(m_fSetupSources_Ge68Activity, acMsg);
			send_to_amulet_string(169, acMsg);
			if(m_dtmSetupSources_Ge68Date == 0) acMsg[0] = 0;
			else GetExtendedTimeInfoLanguage(&m_dtmSetupSources_Ge68Date, acMsg);
			send_to_amulet_string(170, acMsg);
			if(m_cSetupSources_Ge68Daily == FALSE) SetAmuletByte(107, 11);
			else SetAmuletByte(107, 12);

			SetAmuletByte(106, constsource);

			for(i=0; i<12; i++){
				if(AmuletSetupSourcesMenu_constancyCh[i]>=0){
					GetShortNuclideInfo(AmuletSetupSourcesMenu_constancyCh[i], &output_nuc, acMsg);
					send_to_amulet_string(115+i, output_nuc.name);
				}else send_to_amulet_string(115+i, "");
			}

			send_amulet_message(L_SETUP_SOURCES,129);    // "Setup Sources"
			
			send_amulet_message(L_SETUP_SOURCES_ACTIVITY,131);    // "ACTIVITY"
			
			send_amulet_message(L_SETUP_SOURCES_DATE,132);    // "DATE"
			
			send_amulet_message(L_SETUP_SOURCES_DAILY,133);    // "DAILY"

			send_amulet_message(L_SETUP_SOURCES_CONSTANCY_CHANNELS,134);    // "Constancy Channels"
			
			send_amulet_message(L_CLEAR_BUTTON,135);    // "Clear"

			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Co57 ");
			get_amulet_message(L_SN,temp_message);    // "S/N"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(136,message);
			
			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Co57 ");
			get_amulet_message(L_ACTIVITY,temp_message);    // "Activity"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(138,message);
			
			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Co57 ");
			get_amulet_message(L_CALIBRATION_TIME,temp_message);    // "Calibration Time"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(140,message);

			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Co60 ");
			get_amulet_message(L_SN,temp_message);    // "S/N"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(142,message);
			
			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Co60 ");
			get_amulet_message(L_ACTIVITY,temp_message);    // "Activity"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(144,message);
			
			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Co60 ");
			get_amulet_message(L_CALIBRATION_TIME,temp_message);    // "Calibration Time"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(146,message);

			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Ba133 ");
			get_amulet_message(L_SN,temp_message);    // "S/N"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(148,message);
			
			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Ba133 ");
			get_amulet_message(L_ACTIVITY,temp_message);    // "Activity"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(150,message);
			
			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Ba133 ");
			get_amulet_message(L_CALIBRATION_TIME,temp_message);    // "Calibration Time"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(152,message);

			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Cs137 ");
			get_amulet_message(L_SN,temp_message);    // "S/N"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(154,message);
			
			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Cs137 ");
			get_amulet_message(L_ACTIVITY,temp_message);    // "Activity"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(156,message);
			
			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Cs137 ");
			get_amulet_message(L_CALIBRATION_TIME,temp_message);    // "Calibration Time"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(158,message);

			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Na22 ");
			get_amulet_message(L_SN,temp_message);    // "S/N"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(160,message);
			
			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Na22 ");
			get_amulet_message(L_ACTIVITY,temp_message);    // "Activity"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(162,message);
			
			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Na22 ");
			get_amulet_message(L_CALIBRATION_TIME,temp_message);    // "Calibration Time"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(164,message);

			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Ge68 ");
			get_amulet_message(L_SN,temp_message);    // "S/N"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(171,message);

			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Ge68 ");
			get_amulet_message(L_ACTIVITY,temp_message);    // "Activity"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(173,message);

			get_amulet_message(L_PLEASE_ENTER,message);    // "Please Enter"
			strcat(message," Ge68 ");
			get_amulet_message(L_CALIBRATION_TIME,temp_message);    // "Calibration Time"
			strcat(message,temp_message);
			strcat(message,":");
			send_to_amulet_string(175,message);

			send_amulet_message(L_PLEASE_SELECT_NUCLIDE,166);    // "Please Select Nuclide"

			send_accept_cancel();
			

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_WAIT:
			break;

		case PHASE_SETUP_SOURCES_CO57DAILY:
			if(m_ucCheckboxState == 1){
				m_cSetupSources_Co57Daily = FALSE;
				SetAmuletByte(101, 1);
			}
			else if(m_ucCheckboxState == 2){
				m_cSetupSources_Co57Daily = TRUE;
				SetAmuletByte(101, 2);
			}
			m_ucCheckboxState = 0;
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_CO60DAILY:
			if(m_ucCheckboxState == 3){
				m_cSetupSources_Co60Daily = FALSE;
				SetAmuletByte(102, 3);
			}
			else if(m_ucCheckboxState == 4){
				m_cSetupSources_Co60Daily = TRUE;
				SetAmuletByte(102, 4);
			}
			m_ucCheckboxState = 0;
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_BA133DAILY:
			if(m_ucCheckboxState == 5){
				m_cSetupSources_Ba133Daily = FALSE;
				SetAmuletByte(103, 5);
			}
			else if(m_ucCheckboxState == 6){
				m_cSetupSources_Ba133Daily = TRUE;
				SetAmuletByte(103, 6);
			}
			m_ucCheckboxState = 0;
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_CS137DAILY:
			if(m_ucCheckboxState == 7){
				m_cSetupSources_Cs137Daily = FALSE;
				SetAmuletByte(104, 7);
			}
			else if(m_ucCheckboxState == 8){
				m_cSetupSources_Cs137Daily = TRUE;
				SetAmuletByte(104, 8);
			}
			m_ucCheckboxState = 0;
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_NA22DAILY:
			if(m_ucCheckboxState == 9){
				m_cSetupSources_Na22Daily = FALSE;
				SetAmuletByte(105, 9);
			}
			else if(m_ucCheckboxState == 10){
				m_cSetupSources_Na22Daily = TRUE;
				SetAmuletByte(105, 10);
			}
			m_ucCheckboxState = 0;
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_GE68DAILY:
			if(m_ucCheckboxState == 11){
				m_cSetupSources_Ge68Daily = FALSE;
				SetAmuletByte(107, 11);
			}
			else if(m_ucCheckboxState == 12){
				m_cSetupSources_Ge68Daily = TRUE;
				SetAmuletByte(107, 12);
			}
			m_ucCheckboxState = 0;
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_CO57CONST:
			beep_amulet();
			constsource = 1;
			SetAmuletByte(106, constsource);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_CO60CONST:
			beep_amulet();
			constsource = 2;
			SetAmuletByte(106, constsource);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_BA133CONST:
			beep_amulet();
			constsource = 3;
			SetAmuletByte(106, constsource);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_CS137CONST:
			beep_amulet();
			constsource = 4;
			SetAmuletByte(106, constsource);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_NA22CONST:
			beep_amulet();
			constsource = 5;
			SetAmuletByte(106, constsource);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_GE68CONST:
			beep_amulet();
			constsource = 6;
			SetAmuletByte(106, constsource);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_CO57CLEAR:
			beep_amulet();
			m_acSetupSources_Co57SN[0] = 0;
			send_to_amulet_string(100,"");
			m_fSetupSources_Co57Activity = -999;
			send_to_amulet_string(101,"");
			m_dtmSetupSources_Co57Date = (time_t)0;
			send_to_amulet_string(102,"");
			m_cSetupSources_Co57Daily = 0;
			SetAmuletByte(101,1);
			if(constsource == 1){
				constsource = 0;
				SetAmuletByte(106, constsource);
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_CO60CLEAR:
			beep_amulet();
			m_acSetupSources_Co60SN[0] = 0;
			send_to_amulet_string(103,"");
			m_fSetupSources_Co60Activity = -999;
			send_to_amulet_string(104,"");
			m_dtmSetupSources_Co60Date = (time_t)0;
			send_to_amulet_string(105,"");
			m_cSetupSources_Co60Daily = 0;
			SetAmuletByte(102, 3);
			if(constsource == 2){
				constsource = 0;
				SetAmuletByte(106, constsource);
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_BA133CLEAR:
			beep_amulet();
			m_acSetupSources_Ba133SN[0] = 0;
			send_to_amulet_string(106,"");
			m_fSetupSources_Ba133Activity = -999;
			send_to_amulet_string(107,"");
			m_dtmSetupSources_Ba133Date = (time_t)0;
			send_to_amulet_string(108,"");
			m_cSetupSources_Ba133Daily = 0;
			SetAmuletByte(103, 5);
			if(constsource == 3){
				constsource = 0;
				SetAmuletByte(106, constsource);
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_CS137CLEAR:
			beep_amulet();
			m_acSetupSources_Cs137SN[0] = 0;
			send_to_amulet_string(109,"");
			m_fSetupSources_Cs137Activity = -999;
			send_to_amulet_string(110,"");
			m_dtmSetupSources_Cs137Date = (time_t)0;
			send_to_amulet_string(111,"");
			m_cSetupSources_Cs137Daily = 0;
			SetAmuletByte(104, 7);
			if(constsource == 4){
				constsource = 0;
				SetAmuletByte(106, constsource);
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_NA22CLEAR:
			beep_amulet();
			m_acSetupSources_Na22SN[0] = 0;
			send_to_amulet_string(112,"");
			m_fSetupSources_Na22Activity = -999;
			send_to_amulet_string(113,"");
			m_dtmSetupSources_Na22Date = (time_t)0;
			send_to_amulet_string(114,"");
			m_cSetupSources_Na22Daily = 0;
			SetAmuletByte(105, 9);
			if(constsource == 5){
				constsource = 0;
				SetAmuletByte(106, constsource);
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_GE68CLEAR:
			beep_amulet();
			m_acSetupSources_Ge68SN[0] = 0;
			send_to_amulet_string(168,"");
			m_fSetupSources_Ge68Activity = -999;
			send_to_amulet_string(169,"");
			m_dtmSetupSources_Ge68Date = (time_t)0;
			send_to_amulet_string(170,"");
			m_cSetupSources_Ge68Daily = 0;
			SetAmuletByte(107, 11);
			if(constsource == 6){
				constsource = 0;
				SetAmuletByte(106, constsource);
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUP_SOURCES_WAIT;
			break;

		case PHASE_SETUP_SOURCES_SAVE:
			beep_amulet();
			if((constsource!=0)){
				switch(constsource){
					case 1:
						if((m_acSetupSources_Co57SN[0]!=0) && (m_fSetupSources_Co57Activity!=-999) && (m_dtmSetupSources_Co57Date!=(time_t)0)) constsourceselected = TRUE;
						else constsourceselected = FALSE;
						break;

					case 2:
						if((m_acSetupSources_Co60SN[0]!=0) && (m_fSetupSources_Co60Activity!=-999) && (m_dtmSetupSources_Co60Date!=(time_t)0)) constsourceselected = TRUE;
						else constsourceselected = FALSE;
						break;

					case 3:
						if((m_acSetupSources_Ba133SN[0]!=0) && (m_fSetupSources_Ba133Activity!=-999) && (m_dtmSetupSources_Ba133Date!=(time_t)0)) constsourceselected = TRUE;
						else constsourceselected = FALSE;
						break;

					case 4:
						if((m_acSetupSources_Cs137SN[0]!=0) && (m_fSetupSources_Cs137Activity!=-999) && (m_dtmSetupSources_Cs137Date!=(time_t)0)) constsourceselected = TRUE;
						else constsourceselected = FALSE;
						break;

					case 5:
						if((m_acSetupSources_Na22SN[0]!=0) && (m_fSetupSources_Na22Activity!=-999) && (m_dtmSetupSources_Na22Date!=(time_t)0)) constsourceselected = TRUE;
						else constsourceselected = FALSE;
						break;

					case 6:
						if((m_acSetupSources_Ge68SN[0]!=0) && (m_fSetupSources_Ge68Activity!=-999) && (m_dtmSetupSources_Ge68Date!=(time_t)0)) constsourceselected = TRUE;
						else constsourceselected = FALSE;
						break;
				}
			}else{
				constsourceselected = FALSE;
			}

			constchannelselected = FALSE;
			for(i=0; i<12; i++){
				if(AmuletSetupSourcesMenu_constancyCh[i]>=0){
					constchannelselected = TRUE;
					break;
				}
			}

			if(constsourceselected && !constchannelselected){
				//Amulet_DisplayError("Source Setup Error", "Constancy Source selected without Constancy Channels",TRUE);
				get_amulet_message(L_SETUP_SOURCES_ERROR,message);    // "Setup Sources Error"
				get_amulet_message(L_NO_CONSTANCY_CHANNEL,errormessage);    // "Constancy Source selected without Constancy Channels"
				Amulet_DisplayError(message,errormessage,TRUE);
				return;
			}else if(!constsourceselected && constchannelselected){
				//Amulet_DisplayError("Source Setup Error", "Constancy Channels selected without Constancy Source",TRUE);
				get_amulet_message(L_SETUP_SOURCES_ERROR,message);    // "Setup Sources Error"
				get_amulet_message(L_NO_CONSTANCY_SOURCE,errormessage);    // "Constancy Channels selected without Constancy Source"
				Amulet_DisplayError(message,errormessage,TRUE);
				return;
			}else{
				min_activity_passed = TRUE;

				if(min_activity_passed && (m_acSetupSources_Co57SN[0]!=0) && (m_fSetupSources_Co57Activity!=-999) && (m_dtmSetupSources_Co57Date!=(time_t)0)){
					read_clock(&now_time);
					clock_time = now_time;
					low_clock_time = clock_time;
					reset_minute_counter_with_seconds();
					nuc_index = NuclideData_getIndexFromName("Co57");
					current_activity = nucdecay(m_fSetupSources_Co57Activity, m_dtmSetupSources_Co57Date, now_time, NuclideData_getHalflife(nuc_index), NuclideData_getHalflifeUnit(nuc_index));
					upper_limit = 1.0;
					lower_limit = .000050;
					if(current_activity > upper_limit){
						if(current.system == CI){
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_CO57_CI,fmt_str);    // "Above Maximum Activity\nCo57 Activity: %.3f Ci\nUpper Limit: %.3f Ci"
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nCo57 Activity: %.3f Ci\nUpper Limit: %.3f Ci", current_activity, upper_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+9;
							upper_limit = upper_limit * BQFACTOR;
							upper_limit = upper_limit / 1.0e+9;
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_CO57_BQ,fmt_str);    // "Above Maximum Activity\nCo57 Activity: %.3f GBq\nUpper Limit: %.3f GBq"
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nCo57 Activity: %.3f GBq\nUpper Limit: %.3f GBq", current_activity, upper_limit);
						}
						min_activity_passed = FALSE;
					}
					if(current_activity < lower_limit){
						if(current.system == CI){
							current_activity = current_activity * 1.0e+6;
							lower_limit = lower_limit * 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_CO57_CI,fmt_str);    // "Below Minimum Activity\nCo57 Activity: %.3f uCi\nLower Limit: %.3f uCi"
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nCo57 Activity: %.3f uCi\nLower Limit: %.3f uCi", current_activity, lower_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+6;
							lower_limit = lower_limit * BQFACTOR;
							lower_limit = lower_limit / 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_CO57_BQ,fmt_str);    // "Below Minimum Activity\nCo57 Activity: %.6f MBq\nLower Limit: %.6f MBq"
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nCo57 Activity: %.6f MBq\nLower Limit: %.6f MBq", current_activity, lower_limit);
						}
						min_activity_passed = FALSE;
					}
				}

				if(min_activity_passed && (m_acSetupSources_Co60SN[0]!=0) && (m_fSetupSources_Co60Activity!=-999) && (m_dtmSetupSources_Co60Date!=(time_t)0)){
					read_clock(&now_time);
					clock_time = now_time;
					low_clock_time = clock_time;
					reset_minute_counter_with_seconds();
					nuc_index = NuclideData_getIndexFromName("Co60");
					current_activity = nucdecay(m_fSetupSources_Co60Activity, m_dtmSetupSources_Co60Date, now_time, NuclideData_getHalflife(nuc_index), NuclideData_getHalflifeUnit(nuc_index));
					upper_limit = 1.0;
					lower_limit = .000050;
					if(current_activity > upper_limit){
						if(current.system == CI){
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_CO60_CI,fmt_str);    // "Above Maximum Activity\nCo60 Activity: %.3f Ci\nUpper Limit: %.3f Ci"
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nCo60 Activity: %.3f Ci\nUpper Limit: %.3f Ci", current_activity, upper_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+9;
							upper_limit = upper_limit * BQFACTOR;
							upper_limit = upper_limit / 1.0e+9;
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_CO60_BQ,fmt_str);    // "Above Maximum Activity\nCo60 Activity: %.3f GBq\nUpper Limit: %.3f GBq"
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nCo60 Activity: %.3f GBq\nUpper Limit: %.3f GBq", current_activity, upper_limit);
						}
						min_activity_passed = FALSE;
					}
					if(current_activity < lower_limit){
						if(current.system == CI){
							current_activity = current_activity * 1.0e+6;
							lower_limit = lower_limit * 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_CO60_CI,fmt_str);    // "Below Minimum Activity\nCo60 Activity: %.3f uCi\nLower Limit: %.3f uCi"
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nCo60 Activity: %.3f uCi\nLower Limit: %.3f uCi", current_activity, lower_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+6;
							lower_limit = lower_limit * BQFACTOR;
							lower_limit = lower_limit / 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_CO60_BQ,fmt_str);    // "Below Minimum Activity\nCo60 Activity: %.6f MBq\nLower Limit: %.6f MBq"
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nCo60 Activity: %.6f MBq\nLower Limit: %.6f MBq", current_activity, lower_limit);
						}
						min_activity_passed = FALSE;
					}
				}

				if(min_activity_passed && (m_acSetupSources_Ba133SN[0]!=0) && (m_fSetupSources_Ba133Activity!=-999) && (m_dtmSetupSources_Ba133Date!=(time_t)0)){
					read_clock(&now_time);
					clock_time = now_time;
					low_clock_time = clock_time;
					reset_minute_counter_with_seconds();
					nuc_index = NuclideData_getIndexFromName("Ba133");
					current_activity = nucdecay(m_fSetupSources_Ba133Activity, m_dtmSetupSources_Ba133Date, now_time, NuclideData_getHalflife(nuc_index), NuclideData_getHalflifeUnit(nuc_index));
					upper_limit = 1.0;
					lower_limit = .000050;
					if(current_activity > upper_limit){
						if(current.system == CI){
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_BA133_CI,fmt_str);    // "Above Maximum Activity\nBa133 Activity: %.3f Ci\nUpper Limit: %.3f Ci"
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nBa133 Activity: %.3f Ci\nUpper Limit: %.3f Ci", current_activity, upper_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+9;
							upper_limit = upper_limit * BQFACTOR;
							upper_limit = upper_limit / 1.0e+9;
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_BA133_BQ,fmt_str);    // "Above Maximum Activity\nBa133 Activity: %.3f GBq\nUpper Limit: %.3f GBq"
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nBa133 Activity: %.3f GBq\nUpper Limit: %.3f GBq", current_activity, upper_limit);
						}
						min_activity_passed = FALSE;
					}
					if(current_activity < lower_limit){
						if(current.system == CI){
							current_activity = current_activity * 1.0e+6;
							lower_limit = lower_limit * 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_BA133_CI,fmt_str);    // "Below Minimum Activity\nBa133 Activity: %.3f uCi\nLower Limit: %.3f uCi"
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nBa133 Activity: %.3f uCi\nLower Limit: %.3f uCi", current_activity, lower_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+6;
							lower_limit = lower_limit * BQFACTOR;
							lower_limit = lower_limit / 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_BA133_BQ,fmt_str);    // "Below Minimum Activity\nBa133 Activity: %.6f MBq\nLower Limit: %.6f MBq"
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nBa133 Activity: %.6f MBq\nLower Limit: %.6f MBq", current_activity, lower_limit);
						}
						min_activity_passed = FALSE;
					}
				}

				if(min_activity_passed && (m_acSetupSources_Cs137SN[0]!=0) && (m_fSetupSources_Cs137Activity!=-999) && (m_dtmSetupSources_Cs137Date!=(time_t)0)){
					read_clock(&now_time);
					clock_time = now_time;
					low_clock_time = clock_time;
					reset_minute_counter_with_seconds();
					nuc_index = NuclideData_getIndexFromName("Cs137");
					current_activity = nucdecay(m_fSetupSources_Cs137Activity, m_dtmSetupSources_Cs137Date, now_time, NuclideData_getHalflife(nuc_index), NuclideData_getHalflifeUnit(nuc_index));
					upper_limit = 1.0;
					lower_limit = .000050;
					if(current_activity > upper_limit){
						if(current.system == CI){
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_CS137_CI,fmt_str);    // "Above Maximum Activity\nCs137 Activity: %.3f Ci\nUpper Limit: %.3f Ci"
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nCs137 Activity: %.3f Ci\nUpper Limit: %.3f Ci", current_activity, upper_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+9;
							upper_limit = upper_limit * BQFACTOR;
							upper_limit = upper_limit / 1.0e+9;
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_CS137_BQ,fmt_str);    // "Above Maximum Activity\nCs137 Activity: %.3f GBq\nUpper Limit: %.3f GBq"
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nCs137 Activity: %.3f GBq\nUpper Limit: %.3f GBq", current_activity, upper_limit);
						}
						min_activity_passed = FALSE;
					}
					if(current_activity < lower_limit){
						if(current.system == CI){
							current_activity = current_activity * 1.0e+6;
							lower_limit = lower_limit * 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_CS137_CI,fmt_str);    // "Below Minimum Activity\nCs137 Activity: %.3f uCi\nLower Limit: %.3f uCi"
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nCs137 Activity: %.3f uCi\nLower Limit: %.3f uCi", current_activity, lower_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+6;
							lower_limit = lower_limit * BQFACTOR;
							lower_limit = lower_limit / 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_CS137_BQ,fmt_str);    // "Below Minimum Activity\nCs137 Activity: %.6f MBq\nLower Limit: %.6f MBq"
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nCs137 Activity: %.6f MBq\nLower Limit: %.6f MBq", current_activity, lower_limit);
						}
						min_activity_passed = FALSE;
					}
				}

				if(min_activity_passed && (m_acSetupSources_Na22SN[0]!=0) && (m_fSetupSources_Na22Activity!=-999) && (m_dtmSetupSources_Na22Date!=(time_t)0)){
					read_clock(&now_time);
					clock_time = now_time;
					low_clock_time = clock_time;
					reset_minute_counter_with_seconds();
					nuc_index = NuclideData_getIndexFromName("Na22");
					current_activity = nucdecay(m_fSetupSources_Na22Activity, m_dtmSetupSources_Na22Date, now_time, NuclideData_getHalflife(nuc_index), NuclideData_getHalflifeUnit(nuc_index));
					upper_limit = 1.0;
					lower_limit = .000050;
					if(current_activity > upper_limit){
						if(current.system == CI){
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_NA22_CI,fmt_str);    // "Above Maximum Activity\nNa22 Activity: %.3f Ci\nUpper Limit: %.3f Ci"
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nNa22 Activity: %.3f Ci\nUpper Limit: %.3f Ci", current_activity, upper_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+9;
							upper_limit = upper_limit * BQFACTOR;
							upper_limit = upper_limit / 1.0e+9;
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_NA22_BQ,fmt_str);    // "Above Maximum Activity\nNa22 Activity: %.3f GBq\nUpper Limit: %.3f GBq"
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nNa22 Activity: %.3f GBq\nUpper Limit: %.3f GBq", current_activity, upper_limit);
						}
						min_activity_passed = FALSE;
					}
					if(current_activity < lower_limit){
						if(current.system == CI){
							current_activity = current_activity * 1.0e+6;
							lower_limit = lower_limit * 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_NA22_CI,fmt_str);    // "Below Minimum Activity\nNa22 Activity: %.3f uCi\nLower Limit: %.3f uCi"
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nNa22 Activity: %.3f uCi\nLower Limit: %.3f uCi", current_activity, lower_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+6;
							lower_limit = lower_limit * BQFACTOR;
							lower_limit = lower_limit / 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_NA22_BQ,fmt_str);    // "Below Minimum Activity\nNa22 Activity: %.6f MBq\nLower Limit: %.6f MBq"
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nNa22 Activity: %.6f MBq\nLower Limit: %.6f MBq", current_activity, lower_limit);
						}
						min_activity_passed = FALSE;
					}
				}

				if(min_activity_passed && (m_acSetupSources_Ge68SN[0]!=0) && (m_fSetupSources_Ge68Activity!=-999) && (m_dtmSetupSources_Ge68Date!=(time_t)0)){
					read_clock(&now_time);
					clock_time = now_time;
					low_clock_time = clock_time;
					reset_minute_counter_with_seconds();
					nuc_index = NuclideData_getIndexFromName("Ge68");
					current_activity = nucdecay(m_fSetupSources_Ge68Activity, m_dtmSetupSources_Ge68Date, now_time, NuclideData_getHalflife(nuc_index), NuclideData_getHalflifeUnit(nuc_index));
					upper_limit = 1.0;
					lower_limit = .000050;
					if(current_activity > upper_limit){
						if(current.system == CI){
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_NA22_CI,fmt_str);    // "Above Maximum Activity\nNa22 Activity: %.3f Ci\nUpper Limit: %.3f Ci"
							ptr = strstr(fmt_str, "Na22");
							*ptr++ = 'G';
							*ptr++ = 'e';
							*ptr++ = '6';
							*ptr = '8';
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nGe68 Activity: %.3f Ci\nUpper Limit: %.3f Ci", current_activity, upper_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+9;
							upper_limit = upper_limit * BQFACTOR;
							upper_limit = upper_limit / 1.0e+9;
							get_amulet_message(L_ABOVE_MAXIMUM_ACTIVITY_NA22_BQ,fmt_str);    // "Above Maximum Activity\nNa22 Activity: %.3f GBq\nUpper Limit: %.3f GBq"
							ptr = strstr(fmt_str, "Na22");
							*ptr++ = 'G';
							*ptr++ = 'e';
							*ptr++ = '6';
							*ptr = '8';
							sprintf(errormessage, fmt_str, current_activity, upper_limit);
							//sprintf(errormessage, "Above Maximum Activity\nGe68 Activity: %.3f GBq\nUpper Limit: %.3f GBq", current_activity, upper_limit);
						}
						min_activity_passed = FALSE;
					}
					if(current_activity < lower_limit){
						if(current.system == CI){
							current_activity = current_activity * 1.0e+6;
							lower_limit = lower_limit * 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_NA22_CI,fmt_str);    // "Below Minimum Activity\nNa22 Activity: %.3f uCi\nLower Limit: %.3f uCi"
							ptr = strstr(fmt_str, "Na22");
							*ptr++ = 'G';
							*ptr++ = 'e';
							*ptr++ = '6';
							*ptr = '8';
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nGe68 Activity: %.3f uCi\nLower Limit: %.3f uCi", current_activity, lower_limit);
						}else{
							current_activity = current_activity * BQFACTOR;
							current_activity = current_activity / 1.0e+6;
							lower_limit = lower_limit * BQFACTOR;
							lower_limit = lower_limit / 1.0e+6;
							get_amulet_message(L_BELOW_MINIMUM_ACTIVITY_NA22_BQ,fmt_str);    // "Below Minimum Activity\nNa22 Activity: %.6f MBq\nLower Limit: %.6f MBq"
							ptr = strstr(fmt_str, "Na22");
							*ptr++ = 'G';
							*ptr++ = 'e';
							*ptr++ = '6';
							*ptr = '8';
							sprintf(errormessage,fmt_str, current_activity, lower_limit);
							//sprintf(errormessage, "Below Minimum Activity\nGe68 Activity: %.6f MBq\nLower Limit: %.6f MBq", current_activity, lower_limit);
						}
						min_activity_passed = FALSE;
					}
				}

				if(min_activity_passed){
					if((m_acSetupSources_Co57SN[0]!=0) && (m_fSetupSources_Co57Activity!=-999) && (m_dtmSetupSources_Co57Date!=(time_t)0)){
						standard[0].act = m_fSetupSources_Co57Activity;
						standard[0].caldate = m_dtmSetupSources_Co57Date;
						if(constsource == 1) standard[0].const_iso = TRUE;
						else standard[0].const_iso = FALSE;
						if(m_cSetupSources_Co57Daily) standard[0].daily = TRUE;
						else standard[0].daily = FALSE;
						strcpy(standard[0].sn, m_acSetupSources_Co57SN);
					}else{
						standard[0].act = 0;
						standard[0].caldate = (time_t)0;
						standard[0].const_iso = FALSE;
						standard[0].daily = FALSE;
						strcpy(standard[0].sn, "");
					}

					if((m_acSetupSources_Co60SN[0]!=0) && (m_fSetupSources_Co60Activity!=-999) && (m_dtmSetupSources_Co60Date!=(time_t)0)){
						standard[1].act = m_fSetupSources_Co60Activity;
						standard[1].caldate = m_dtmSetupSources_Co60Date;
						if(constsource == 2) standard[1].const_iso = TRUE;
						else standard[1].const_iso = FALSE;
						if(m_cSetupSources_Co60Daily) standard[1].daily = TRUE;
						else standard[1].daily = FALSE;
						strcpy(standard[1].sn, m_acSetupSources_Co60SN);
					}else{
						standard[1].act = 0;
						standard[1].caldate = (time_t)0;
						standard[1].const_iso = FALSE;
						standard[1].daily = FALSE;
						strcpy(standard[1].sn, "");
					}

					if((m_acSetupSources_Ba133SN[0]!=0) && (m_fSetupSources_Ba133Activity!=-999) && (m_dtmSetupSources_Ba133Date!=(time_t)0)){
						standard[2].act = m_fSetupSources_Ba133Activity;
						standard[2].caldate = m_dtmSetupSources_Ba133Date;
						if(constsource == 3) standard[2].const_iso = TRUE;
						else standard[2].const_iso = FALSE;
						if(m_cSetupSources_Ba133Daily) standard[2].daily = TRUE;
						else standard[2].daily = FALSE;
						strcpy(standard[2].sn, m_acSetupSources_Ba133SN);
					}else{
						standard[2].act = 0;
						standard[2].caldate = (time_t)0;
						standard[2].const_iso = FALSE;
						standard[2].daily = FALSE;
						strcpy(standard[2].sn, "");
					}

					if((m_acSetupSources_Cs137SN[0]!=0) && (m_fSetupSources_Cs137Activity!=-999) && (m_dtmSetupSources_Cs137Date!=(time_t)0)){
						standard[3].act = m_fSetupSources_Cs137Activity;
						standard[3].caldate = m_dtmSetupSources_Cs137Date;
						if(constsource == 4) standard[3].const_iso = TRUE;
						else standard[3].const_iso = FALSE;
						if(m_cSetupSources_Cs137Daily) standard[3].daily = TRUE;
						else standard[3].daily = FALSE;
						strcpy(standard[3].sn, m_acSetupSources_Cs137SN);
					}else{
						standard[3].act = 0;
						standard[3].caldate = (time_t)0;
						standard[3].const_iso = FALSE;
						standard[3].daily = FALSE;
						strcpy(standard[3].sn, "");
					}

					if((m_acSetupSources_Na22SN[0]!=0) && (m_fSetupSources_Na22Activity!=-999) && (m_dtmSetupSources_Na22Date!=(time_t)0)){
						standard[4].act = m_fSetupSources_Na22Activity;
						standard[4].caldate = m_dtmSetupSources_Na22Date;
						if(constsource == 5) standard[4].const_iso = TRUE;
						else standard[4].const_iso = FALSE;
						if(m_cSetupSources_Na22Daily) standard[4].daily = TRUE;
						else standard[4].daily = FALSE;
						strcpy(standard[4].sn, m_acSetupSources_Na22SN);
					}else{
						standard[4].act = 0;
						standard[4].caldate = (time_t)0;
						standard[4].const_iso = FALSE;
						standard[4].daily = FALSE;
						strcpy(standard[4].sn, "");
					}

					if((m_acSetupSources_Ge68SN[0]!=0) && (m_fSetupSources_Ge68Activity!=-999) && (m_dtmSetupSources_Ge68Date!=(time_t)0)){
						standard[5].act = m_fSetupSources_Ge68Activity;
						standard[5].caldate = m_dtmSetupSources_Ge68Date;
						if(constsource == 6) standard[5].const_iso = TRUE;
						else standard[5].const_iso = FALSE;
						if(m_cSetupSources_Ge68Daily) standard[5].daily = TRUE;
						else standard[5].daily = FALSE;
						strcpy(standard[5].sn, m_acSetupSources_Ge68SN);
					}else{
						standard[5].act = 0;
						standard[5].caldate = (time_t)0;
						standard[5].const_iso = FALSE;
						standard[5].daily = FALSE;
						strcpy(standard[5].sn, "");
					}

					EE_WRITE(constancysource, (uchar *)&constsource);
					EE_WRITE(constancych, (uchar *) AmuletSetupSourcesMenu_constancyCh);
					EE_WRITE(stand, (uchar *)standard);
					EE_WRITE(stand_ge68, (uchar *)(&(standard[5])));
					SetAmuletBackHTML();
					return;
				}else{
					get_amulet_message(L_SETUP_SOURCES_ERROR,message);    // "Setup Sources Error"
					Amulet_DisplayError(message, errormessage, TRUE);
					//Amulet_DisplayError("Source Setup Error", errormessage, TRUE);
					return;
				}
			}
			//break;
	}
}

void send_accept_cancel(void)
{
	char message[26];

	get_amulet_message(L_ACCEPT_BUTTON,message);    // "Accept"
	send_to_amulet_string(90,message);

	
	get_amulet_message(L_CANCEL_BUTTON,message);    // "Cancel"
	send_to_amulet_string(91,message);
}	
	

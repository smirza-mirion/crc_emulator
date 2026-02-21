/**
 * \file
 * \details This file contains functions, which calls the Sql layer and implements communications with the database.
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef EMULATOR
#include <stdio.h>
#include <unistd.h>
#endif
#include "crc.h"
#include "sqlite3.h"
#include "ff.h"
#include "mca.h"
#include "wipes.h"
#include "nuc.h"
#include "keyboard.h"
#include "i2c.h"
#include "chambfac.h"

extern time_t clock_time;
extern short int AmuletWellMeasureMenu_countTime[8];
extern char cap_str[];
extern CURRENT current;
extern unsigned char m_ucHotKeyNuclideID[UPPER_LIMIT_CHAMB][8];
extern unsigned char m_ucHotKeyNuclideID2[UPPER_LIMIT_CHAMB][20];

typedef struct DateTime DateTime;
struct DateTime {
  sqlite3_int64 iJD; /* The julian day number times 86400000 */
  int Y, M, D;       /* Year, month, and day */
  int h, m;          /* Hour and minutes */
  int tz;            /* Timezone offset in minutes */
  double s;          /* Seconds */
  char validYMD;     /* True (1) if Y,M,D are valid */
  char validHMS;     /* True (1) if h,m,s are valid */
  char validJD;      /* True (1) if iJD is valid */
  char validTZ;      /* True (1) if tz is valid */
};

void Amulet_DisplayError2(char *title, char *errorstring, bool showOK);
void MCACalcLinFactors(AUTOCAL *autoCal);

static void db_initialize(int *status);
static void db_open_v2(int *status, const char *filename, sqlite3 **ppDb, int flags, const char *zVfs);
static void db_prepare_v2(int *status, sqlite3 *db, const char *zSql, int nBytes, sqlite3_stmt **ppStmt, const char **pzTail);

static void db_bind_blob(int *status, sqlite3_stmt *pStmt, const char *name, const void *data, int data_len, void(*mem_callback)(void *data));
static void db_bind_double(int *status, sqlite3_stmt *pStmt, const char *name, double data);
static void db_bind_int(int *status, sqlite3_stmt *pStmt, const char *name, int data);
static void db_bind_int64(int *status, sqlite3_stmt *pStmt, const char *name, sqlite3_int64 data);
static void db_bind_null(int *status, sqlite3_stmt *pStmt, const char *name);
static void db_bind_text(int *status, sqlite3_stmt *pStmt, const char *name, const char *data, int data_len, void(*mem_callback)(void *data));
static void db_bind_text16(int *status, sqlite3_stmt *pStmt, const char *name, const void *data, int data_len, void(*mem_callback)(void *data));
static void db_bind_value(int *status, sqlite3_stmt *pStmt, const char *name, const sqlite3_value *data_value);
static void db_bind_zeroblob(int *status, sqlite3_stmt *pStmt, const char *name, int len);

static void db_step(int *status, sqlite3_stmt *pStmt);

static int db_column_count(int *status, sqlite3_stmt *pStmt);
static const void *db_column_blob(int *status, sqlite3_stmt *pStmt, int iCol);
static int db_column_bytes(int *status, sqlite3_stmt *pStmt, int iCol);
static int db_column_bytes16(int *status, sqlite3_stmt *pStmt, int iCol);
static double db_column_double(int *status, sqlite3_stmt *pStmt, int iCol);
static int db_column_int(int *status, sqlite3_stmt *pStmt, int iCol);
static sqlite3_int64 db_column_int64(int *status, sqlite3_stmt *pStmt, int iCol);
static const unsigned char *db_column_text(int *status, sqlite3_stmt *pStmt, int iCol);
static const void *db_column_text16(int *status, sqlite3_stmt *pStmt, int iCol);
static int db_column_type(int *status, sqlite3_stmt *pStmt, int iCol);
static sqlite3_value *db_column_value(int *status, sqlite3_stmt *pStmt, int iCol);
static const char *db_column_name(int *status, sqlite3_stmt *pStmt, int iCol);
static const void *db_column_name16(int *status, sqlite3_stmt *pStmt, int iCol);

static double d_column_double(int iCol);

static void db_reset(int *status, sqlite3_stmt *pStmt);
static void db_finalize(int *status, sqlite3_stmt *pStmt);
static void db_close(int *status, sqlite3 *db);
static void db_shutdown(int *status);

static void d_command(const char *sCommand);
static int d_query(const char *sCommand);
static void d_prepare_v2(const char *sCommand);
static void d_bind_blob(const char *name, const void *data, int data_len, void(*mem_callback)(void *data));
static void d_bind_double(const char *name, double data);
static void d_bind_int(const char *name, int data);
static void d_bind_int64(const char *name, sqlite3_int64 data);
static void d_bind_null(const char *name);
static void d_bind_text(const char *name, const char *data);
static void d_bind_text16(const char *name, const void *data);
static void d_bind_value(const char *name, const sqlite3_value *data_value);
static void d_bind_zeroblob(const char *name, int len);
static void d_step(void);
static void d_reset(void);
static void d_finalize(void);
static void DB_retrieveWellTime(short int *value, int index);

void DB_WriteProbeThyroidUptakeProtocol(PROBETHYROIDUPTAKEPROTOCOL *protocol, bool bookEnd);
void DB_WriteProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test, bool bookEnd);
void DB_CreateWellWipeType(char *Name, float Threshold, int CountTime);
void trim(char *acByte);
void DB_moveUserNuclideFromEEPROM(void);
void DB_createEmptyUserNuclide(bool recreate);
void DB_moveUserCalFromEEPROM(void);
void DB_createEmptyUserCal(bool recreate);
void DB_moveUserEffFromFile(bool recreate);
void DB_moveGencfFromEEPROM(void);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
double time_ttojd(time_t time);
void DB_SetDefaultEnergy(void);
void DB_WriteHotkey(int ChamberType, int HotkeyMenu, int HotkeyIndex, int HotkeyNuclideIndex);
void read_hotkey_txt(void);
void DB_WriteAllHotkey(int ch_type);
void DB_WriteAllHotkeyWithInsert(void);
void DB_PopulateDefaultHotkeys(void);

static char err_output[100];
static sqlite3 *capDB;
static sqlite3_stmt *capStmt;
static int *capStatus = NULL;
static PROBETHYROIDUPTAKETEST originalTUTest;
static SETUP_EFFICIENCY_OBSOLETE userEfficiencyObsolete[USERNUC];

/**
 * \details Convert date/time string to time_t format
 * \param timestr Pointer to Null terminated time string
 * \returns time_t structure
 */
static time_t strtotime_t(const char *timestr){
	char str_time[30], year[10], month[10], day[10], hour[10], minute[10], second[10];
	struct tm dt;
	int index, indexbegin, iYear;
	unsigned char ucMonth, ucDay, ucHour, ucMinute, ucSecond;

	strcpy(str_time, timestr);

	indexbegin = 0;
	index = indexbegin;
	while(str_time[index] != '-') index++;
	str_time[index] = 0;
	strcpy(year, &(str_time[indexbegin]));
	iYear = atoi(year);

	indexbegin = index + 1;
	index = indexbegin;
	while(str_time[index] != '-') index++;
	str_time[index] = 0;
	strcpy(month, &(str_time[indexbegin]));
	ucMonth = atoi(month);

	indexbegin = index + 1;
	index = indexbegin;
	while(str_time[index] != ' ') index++;
	str_time[index] = 0;
	strcpy(day, &(str_time[indexbegin]));
	ucDay = atoi(day);

	indexbegin = index + 1;
	index = indexbegin;
	while(str_time[index] != ':') index++;
	str_time[index] = 0;
	strcpy(hour, &(str_time[indexbegin]));
	ucHour = atoi(hour);

	indexbegin = index + 1;
	index = indexbegin;
	while(str_time[index] != ':') index++;
	str_time[index] = 0;
	strcpy(minute, &(str_time[indexbegin]));
	ucMinute = atoi(minute);

	indexbegin = index + 1;
	strcpy(second, &(str_time[indexbegin]));
	ucSecond = atoi(second);

	dt.tm_year = iYear - 1900;
	dt.tm_mon = ucMonth - 1;
	dt.tm_mday = ucDay;
	dt.tm_hour = ucHour;
	dt.tm_min = ucMinute;
	dt.tm_sec = ucSecond;

	return mk_time(&dt);
}
/**
 * \details Open Capintec database
 * \param recreate Display Amulet status strings, while recreating the database
 * \returns None
 */
void initDB(bool recreate){
	bool flgDirOK;
	FILINFO fileInfo;
	char longFileName[100];
	//sqlite3 *capDB;
	//int *status;
	//char sCommand[32768];
	char *sCommand;
	float fDBRev;
	char initstr[10];
	//DateTime datetime;
	int hotkeyIndex;

	if(capStatus == NULL) capStatus = malloc(sizeof(int));
	sCommand = malloc(32768);

	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;

	f_chdrive(0);
	if(f_chdir("/data") != FR_OK){
		if(f_chdir("/") == FR_OK){
			if(f_mkdir("data") == FR_OK){
				if(f_chdir("/data") == FR_OK) flgDirOK = TRUE;
				else flgDirOK = FALSE;
			}else{
				flgDirOK = FALSE;
			}
		}else{
			flgDirOK = FALSE;
		}
	}else{
		flgDirOK = TRUE;
	}

	if(flgDirOK){
		//if(f_stat("capintec.db", &fileInfo) == FR_OK) f_unlink("capintec.db");
		//if(f_stat("capintec.db-journal", &fileInfo) == FR_OK) f_unlink("capintec.db-journal");

		*capStatus = SQLITE_OK;

		if(f_stat("capintec.db", &fileInfo) != FR_OK){
			if(f_stat("capintec.db-journal", &fileInfo) == FR_OK) f_unlink("capintec.db-journal");
			if(f_stat("temp.db", &fileInfo) == FR_OK) f_unlink("temp.db");
			if(f_stat("temp.db-journal", &fileInfo) == FR_OK) f_unlink("temp.db-journal");


			// Created Database
			db_initialize(capStatus);
			//db_open_v2(capStatus, "capintec.db", &capDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
			db_open_v2(capStatus, "temp.db", &capDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

			*sCommand = 0;
			strcat(sCommand, "PRAGMA foreign_keys = 1;");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: PRAGMA foreign_keys, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "PRAGMA cache_size = 2000;");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: PRAGMA cache_size, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "PRAGMA page_size = 1024;");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: PRAGMA page_size, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating Config");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE Config(");
			strcat(sCommand, "ConfigID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "SettingName TEXT,");
			strcat(sCommand, "SettingValue FLOAT");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE Config, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating Spectra");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE Spectra(");
			strcat(sCommand, "SpectraID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "TotalCounts INTEGER,");
			strcat(sCommand, "LiveTime FLOAT,");
			strcat(sCommand, "RealTime FLOAT,");
			strcat(sCommand, "TotalCPM FLOAT,");
			strcat(sCommand, "MeasuredOn FLOAT,");
			strcat(sCommand, "AutoCalID INTEGER REFERENCES AutoCal(AutoCalID),");
			strcat(sCommand, "NumOfChannels INTEGER,");
			strcat(sCommand, "BitsPerChannel INTEGER,");
			strcat(sCommand, "CompressedSpectra BLOB");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE Spectra, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_Spectra_MeasuredOn ON Spectra(MeasuredOn);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_Spectra_MeasuredOn, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating AutoCal");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE AutoCal(");
			strcat(sCommand, "AutoCalID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ZeroOffset INTEGER,");
			strcat(sCommand, "Gain1 INTEGER,");
			strcat(sCommand, "Gain2 INTEGER,");
			strcat(sCommand, "Sigma FLOAT,");
			strcat(sCommand, "Sigma_Lip FLOAT,");  // Added Rev 1.1
			strcat(sCommand, "Linearity0 FLOAT,");
			strcat(sCommand, "Linearity1 FLOAT,");
			strcat(sCommand, "Linearity2 FLOAT,");
			strcat(sCommand, "Linearity3 FLOAT,");
			strcat(sCommand, "Linearity4 FLOAT,");
			strcat(sCommand, "DetectorType INTEGER,");
			strcat(sCommand, "CalStamp FLOAT,");
			strcat(sCommand, "LinStamp FLOAT,");
			strcat(sCommand, "SerialNum TEXT,");
			strcat(sCommand, "Threshold INTEGER,");
			strcat(sCommand, "HV INTEGER,");
			strcat(sCommand, "NumOfChannels INTEGER,");  // Added Rev 1.1
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE AutoCal, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_AutoCal_CalStamp ON AutoCal(CalStamp);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_AutoCal_CalStamp, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating Background");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellBackground(");
			strcat(sCommand, "WellBackgroundID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellBackground_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "DetectorType INTEGER,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellBackground, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating Wipe");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellWipe(");
			strcat(sCommand, "WellWipeID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellWipeLocationID INTEGER REFERENCES WellWipeLocation(WellWipeLocationID),");
			strcat(sCommand, "NetCPM FLOAT,");
			strcat(sCommand, "Efficiency FLOAT,");
			strcat(sCommand, "Activity FLOAT,");
			strcat(sCommand, "ExceedsThreshold INTEGER,");
			strcat(sCommand, "OverallHighActivity INTEGER,");
			strcat(sCommand, "WellWipe_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
			strcat(sCommand, "Comment TEXT,");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellWipe, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellWipe_WellWipe_SpectraID ON WellWipe(WellWipe_SpectraID);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellWipe_WellWipe_SpectraID, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellWipe_WellWipeLocationID ON WellWipe(WellWipeLocationID);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellWipe_WellWipeLocationID, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating WipeWipeNuclide");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellWipeWellWipeNuclide(");
			strcat(sCommand, "WellWipeWellWipeNuclideID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellWipeID INTEGER REFERENCES WellWipe(WellWipeID),");
			strcat(sCommand, "WellWipeNuclideID INTEGER REFERENCES WellWipeNuclide(WellWipeNuclideID)");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellWipeWellWipeNuclide, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating WipeNuclide");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellWipeNuclide(");
			strcat(sCommand, "WellWipeNuclideID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellWipeNuclideGroupID INTEGER,");
			strcat(sCommand, "NuclideID INTEGER,");
			strcat(sCommand, "Name TEXT,");
			strcat(sCommand, "PrimaryEnergy FLOAT,");
			strcat(sCommand, "SecondaryEnergy FLOAT,");
			strcat(sCommand, "TertiaryEnergy FLOAT,");
			strcat(sCommand, "Efficiency FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellWipeNuclide, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating WipePeak");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellWipePeak(");
			strcat(sCommand, "WellWipePeakID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellWipeID INTEGER REFERENCES WellWipe(WellWipeID),");
			strcat(sCommand, "Energy FLOAT,");
			strcat(sCommand, "StartROI FLOAT,");
			strcat(sCommand, "EndROI FLOAT,");
			strcat(sCommand, "ROICounts INTEGER,");
			strcat(sCommand, "ROICPM FLOAT,");
			strcat(sCommand, "BackgroundROICounts INTEGER,");
			strcat(sCommand, "BackgroundROICPM FLOAT,");
			strcat(sCommand, "NetROICPM FLOAT,");
			strcat(sCommand, "WellWipeNuclideID INTEGER REFERENCES WellWipeNuclide(WellWipeNuclideID),");
			strcat(sCommand, "Activity FLOAT,");
			strcat(sCommand, "ExceedsThreshold INTEGER,");
			strcat(sCommand, "PeakType INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellWipePeak, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating WipeLocation");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellWipeLocation(");
			strcat(sCommand, "WellWipeLocationID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellWipeLocationGroupID INTEGER,");
			strcat(sCommand, "Name TEXT,");
			strcat(sCommand, "WellWipeTypeID INTEGER REFERENCES WellWipeType(WellWipeTypeID),");
			strcat(sCommand, "Threshold FLOAT,");
			strcat(sCommand, "CountTime INTEGER,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellWipeLocation, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating WipeLocationNuclide");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellWipeLocationNuclide(");
			strcat(sCommand, "WellWipeLocationNuclideID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellWipeLocationID INTEGER REFERENCES WellWipeLocation(WellWipeLocationID),");
			strcat(sCommand, "NuclideID INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellWipeLocationNuclide, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating WipeType");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellWipeType(");
			strcat(sCommand, "WellWipeTypeID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellWipeTypeGroupID INTEGER,");
			strcat(sCommand, "Name TEXT,");
			strcat(sCommand, "Threshold FLOAT,");
			strcat(sCommand, "CountTime INTEGER,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellWipeType, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating WipeTypeNuclide");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellWipeTypeNuclide(");
			strcat(sCommand, "WellWipeTypeNuclideID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellWipeTypeID INTEGER REFERENCES WellWipeType(WellWipeTypeID),");
			strcat(sCommand, "NuclideID INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellWipeTypeNuclide, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating SystemTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellSystemTest(");
			strcat(sCommand, "WellSystemTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "DetectorTest INTEGER,");
			strcat(sCommand, "NuclideName TEXT,");
			strcat(sCommand, "SerialNumber TEXT,");
			strcat(sCommand, "CalibrationActivity FLOAT,");
			strcat(sCommand, "CalibrationDate FLOAT,");
			strcat(sCommand, "Energy FLOAT,");
			strcat(sCommand, "StartROI FLOAT,");
			strcat(sCommand, "EndROI FLOAT,");
			strcat(sCommand, "Efficiency FLOAT,");
			strcat(sCommand, "Halflife FLOAT,");
			strcat(sCommand, "HalflifeUnit INTEGER,");
			strcat(sCommand, "PredictedActivity FLOAT,");
			strcat(sCommand, "ROICounts INTEGER,");
			strcat(sCommand, "ROICPM FLOAT,");
			strcat(sCommand, "BackgroundROICounts INTEGER,");
			strcat(sCommand, "BackgroundROICPM FLOAT,");
			strcat(sCommand, "NetROICPM FLOAT,");
			strcat(sCommand, "Activity FLOAT,");
			strcat(sCommand, "Deviation FLOAT,");
			strcat(sCommand, "WellSystemTest_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellSystemTest, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellSystemTest_WellSystemTest_SpectraID ON WellSystemTest(WellSystemTest_SpectraID);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellSystemTest_WellSystemTest_SpectraID, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating MDATest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellMDATest(");
			strcat(sCommand, "WellMDATestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "DetectorTest INTEGER,");
			strcat(sCommand, "NuclideName TEXT,");
			strcat(sCommand, "Energy FLOAT,");
			strcat(sCommand, "StartROI FLOAT,");
			strcat(sCommand, "EndROI FLOAT,");
			strcat(sCommand, "Efficiency FLOAT,");
			strcat(sCommand, "PrecisionFactor FLOAT,");
			strcat(sCommand, "CorrectionFactor FLOAT,");
			strcat(sCommand, "ROICounts INTEGER,");
			strcat(sCommand, "MDA FLOAT,");
			strcat(sCommand, "WellMDATest_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellMDATest, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellMDATest_WellMDATest_SpectraID ON WellMDATest(WellMDATest_SpectraID);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellMDATest_WellMDATest_SpectraID, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating ChiSquareTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellChiSquareTest(");
			strcat(sCommand, "WellChiSquareTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "DetectorTest INTEGER,");
			strcat(sCommand, "SampleTime INTEGER,");
			strcat(sCommand, "NumberOfSamples INTEGER,");
			strcat(sCommand, "NuclideName TEXT,");
			strcat(sCommand, "Energy FLOAT,");
			strcat(sCommand, "StartROI FLOAT,");
			strcat(sCommand, "EndROI FLOAT,");
			strcat(sCommand, "ChiSquare FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellChiSquareTest, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellChiSquareTest_CreatedOn ON WellChiSquareTest(CreatedOn);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellChiSquareTest_CreatedOn, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating ChiSquareSample");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellChiSquareSample(");
			strcat(sCommand, "WellChiSquareSampleID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellChiSquareTestID INTEGER REFERENCES WellChiSquareTest(WellChiSquareTestID),");
			strcat(sCommand, "ROICounts INTEGER,");
			strcat(sCommand, "WellChiSquareSample_SpectraID INTEGER REFERENCES Spectra(SpectraID)");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellChiSquareSample, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating SchillingTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellSchillingTest(");
			strcat(sCommand, "WellSchillingTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "UrineVolume FLOAT,");
			strcat(sCommand, "AliquotVolume FLOAT,");
			strcat(sCommand, "DilutionFactor FLOAT,");
			strcat(sCommand, "StartROI FLOAT,");
			strcat(sCommand, "EndROI FLOAT,");
			strcat(sCommand, "StandardROICounts INTEGER,");
			strcat(sCommand, "StandardROICPM FLOAT,");
			strcat(sCommand, "AliquotROICounts INTEGER,");
			strcat(sCommand, "AliquotROICPM FLOAT,");
			strcat(sCommand, "BackgroundROICounts INTEGER,");
			strcat(sCommand, "BackgroundROICPM FLOAT,");
			strcat(sCommand, "Excretion FLOAT,");
			strcat(sCommand, "SchillingStandard_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "SchillingAliquot_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
			strcat(sCommand, "TestIdentID INTEGER REFERENCES TestIdent(TestIdentID),");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellSchillingTest, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellSchillingTest_CreatedOn ON WellSchillingTest(CreatedOn);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellSchillingTest_CreatedOn, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellSchillingTest_TestIdentID ON WellSchillingTest(TestIdentID);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellSchillingTest_TestIdentID, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating PlasmaTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellPlasmaTest(");
			strcat(sCommand, "WellPlasmaTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "DilutionFactor FLOAT,");
			strcat(sCommand, "SampleVolume FLOAT,");
			strcat(sCommand, "Hematocrit FLOAT,");
			strcat(sCommand, "PatientWeight FLOAT,");
			strcat(sCommand, "StartROI FLOAT,");
			strcat(sCommand, "EndROI FLOAT,");
			strcat(sCommand, "StandardROICounts INTEGER,");
			strcat(sCommand, "StandardROICPM FLOAT,");
			strcat(sCommand, "WholeBloodROICounts INTEGER,");
			strcat(sCommand, "WholeBloodROICPM FLOAT,");
			strcat(sCommand, "PlasmaROICounts INTEGER,");
			strcat(sCommand, "PlasmaROICPM FLOAT,");
			strcat(sCommand, "BackgroundROICounts INTEGER,");
			strcat(sCommand, "BackgroundROICPM FLOAT,");
			strcat(sCommand, "WholeBloodVolume FLOAT,");
			strcat(sCommand, "WholeBloodPerKg FLOAT,");
			strcat(sCommand, "PlasmaVolume FLOAT,");
			strcat(sCommand, "PlasmaPerKg FLOAT,");
			strcat(sCommand, "RBCVolume FLOAT,");
			strcat(sCommand, "RBCPerKg FLOAT,");
			strcat(sCommand, "CalculatedHematocrit FLOAT,");
			strcat(sCommand, "Standard_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "WholeBlood_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "Plasma_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
			strcat(sCommand, "TestIdentID INTEGER REFERENCES TestIdent(TestIdentID),");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellPlasmaTest, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellPlasmaTest_CreatedOn ON WellPlasmaTest(CreatedOn);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellPlasmaTest_CreatedOn, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellPlasmaTest_TestIdentID ON WellPlasmaTest(TestIdentID);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellPlasmaTest_TestIdentID, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating RBCTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellRBCTest(");
			strcat(sCommand, "WellRBCTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "DoseHematocrit FLOAT,");
			strcat(sCommand, "PatientHematocrit FLOAT,");
			strcat(sCommand, "PatientWeight FLOAT,");
			strcat(sCommand, "StartROI FLOAT,");
			strcat(sCommand, "EndROI FLOAT,");
			strcat(sCommand, "WholeBloodStandardROICounts INTEGER,");
			strcat(sCommand, "WholeBloodStandardROICPM FLOAT,");
			strcat(sCommand, "PlasmaStandardROICounts INTEGER,");
			strcat(sCommand, "PlasmaStandardROICPM FLOAT,");
			strcat(sCommand, "WholeBloodSampleROICounts INTEGER,");
			strcat(sCommand, "WholeBloodSampleROICPM FLOAT,");
			strcat(sCommand, "PlasmaSampleROICounts INTEGER,");
			strcat(sCommand, "PlasmaSampleROICPM FLOAT,");
			strcat(sCommand, "BackgroundROICounts INTEGER,");
			strcat(sCommand, "BackgroundROICPM FLOAT,");
			strcat(sCommand, "RBCVolume FLOAT,");
			strcat(sCommand, "RBCPerKg FLOAT,");
			strcat(sCommand, "WholeBloodVolume FLOAT,");
			strcat(sCommand, "WholeBloodPerKg FLOAT,");
			strcat(sCommand, "PlasmaVolume FLOAT,");
			strcat(sCommand, "PlasmaPerKg FLOAT,");
			strcat(sCommand, "WholeBloodStandard_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "PlasmaStandard_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "WholeBloodSample_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "PlasmaSample_SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
			strcat(sCommand, "TestIdentID INTEGER REFERENCES TestIdent(TestIdentID),");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellRBCTest, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellRBCTest_CreatedOn ON WellRBCTest(CreatedOn);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellRBCTest_CreatedOn, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_WellRBCTest_TestIdentID ON WellRBCTest(TestIdentID);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_WellRBCTest_TestIdentID, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating TestIdent");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE TestIdent(");
			strcat(sCommand, "TestIdentID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "TestIdentGroupID INTEGER,");
			strcat(sCommand, "TestID TEXT,");
			strcat(sCommand, "PatientID TEXT,");
			strcat(sCommand, "FirstName TEXT,");
			strcat(sCommand, "LastName TEXT,");
			strcat(sCommand, "DateOfBirth FLOAT,");
			strcat(sCommand, "Sex TEXT,");
			strcat(sCommand, "Physician TEXT,");
			strcat(sCommand, "TechID TEXT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellIdent, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating UserNuclide");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE UserNuclide(");							// Added Rev 1.1
			strcat(sCommand, "UserNuclideID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "NucIndexOffset INTEGER,");
			strcat(sCommand, "Show INTEGER,");
			strcat(sCommand, "HalfLife FLOAT,");
			strcat(sCommand, "Response_R FLOAT,");
			strcat(sCommand, "Response_P FLOAT,");
			strcat(sCommand, "Response_B FLOAT,");									// Added Rev 1.6
			strcat(sCommand, "Response_18 FLOAT,");									// Added Rev 1.6
			strcat(sCommand, "Response_C FLOAT,");									// Added Rev 1.6
			strcat(sCommand, "Response_K FLOAT,");									// Added Rev 1.6
			strcat(sCommand, "Response_UserR FLOAT,");
			strcat(sCommand, "Response_UserP FLOAT,");
			strcat(sCommand, "Response_UserB FLOAT,");								// Added Rev 1.6
			strcat(sCommand, "Response_User18 FLOAT,");								// Added Rev 1.6
			strcat(sCommand, "Response_UserC FLOAT,");								// Added Rev 1.6
			strcat(sCommand, "Response_UserK FLOAT,");								// Added Rev 1.6
			strcat(sCommand, "Name TEXT,");
			strcat(sCommand, "FullName TEXT,");
			strcat(sCommand, "Code TEXT,");
			strcat(sCommand, "HLUnit INTERGER,");
			strcat(sCommand, "Energy1 FLOAT,");
			strcat(sCommand, "Energy2 FLOAT,");
			strcat(sCommand, "Energy3 FLOAT,");
			strcat(sCommand, "WellEfficiency FLOAT,");
			strcat(sCommand, "BetaEfficiency FLOAT,");
			strcat(sCommand, "Probe700Efficiency FLOAT,");
			strcat(sCommand, "Well700Efficiency FLOAT,");
			strcat(sCommand, "UserEnergy1 FLOAT,");
			strcat(sCommand, "UserEnergy2 FLOAT,");
			strcat(sCommand, "UserEnergy3 FLOAT,");
			strcat(sCommand, "UserWellEfficiency FLOAT,");
			strcat(sCommand, "UserBetaEfficiency FLOAT,");
			strcat(sCommand, "UserProbe700Efficiency FLOAT,");
			strcat(sCommand, "UserWell700Efficiency FLOAT,");
			strcat(sCommand, "WellMeasuredOn FLOAT,");
			strcat(sCommand, "BetaMeasuredOn FLOAT,");
			strcat(sCommand, "Probe700MeasuredOn FLOAT,");
			strcat(sCommand, "Well700MeasuredOn FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE UserNuclide, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating UserCal");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE UserCal(");							// Added Rev 1.1
			strcat(sCommand, "UserCalID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "UserCalIndexOffset INTEGER,");
			strcat(sCommand, "NuclideIndex INTEGER,");
			strcat(sCommand, "ChamberType INTEGER,");
			strcat(sCommand, "Response FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE UserCal, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating UserEff");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE UserEff(");							// Added Rev 1.1
			strcat(sCommand, "UserEffID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "UserEffIndexOffset INTEGER,");
			strcat(sCommand, "NuclideIndex INTEGER,");
			strcat(sCommand, "UserEnergy1 FLOAT,");
			strcat(sCommand, "UserEnergy2 FLOAT,");
			strcat(sCommand, "UserEnergy3 FLOAT,");
			strcat(sCommand, "UserWellEff FLOAT,");
			strcat(sCommand, "UserBetaEff FLOAT,");
			strcat(sCommand, "UserProbe700Eff FLOAT,");
			strcat(sCommand, "UserWell700Eff FLOAT,");
			strcat(sCommand, "WellMeasuredOn FLOAT,");
			strcat(sCommand, "BetaMeasuredOn FLOAT,");
			strcat(sCommand, "Probe700MeasuredOn FLOAT,");
			strcat(sCommand, "Well700MeasuredOn FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE UserEff, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating User");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE User(");							// Added Rev 1.1
			strcat(sCommand, "UserID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "UserName TEXT,");
			strcat(sCommand, "Password TEXT,");
			strcat(sCommand, "FirstName TEXT,");
			strcat(sCommand, "LastName TEXT,");
			strcat(sCommand, "Role INTEGER,"); // 0 = Guest, 1 = User, 2 = Administrator, 3 = SuperUser
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)), ");
			strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE User, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating BioAssayEff");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ProbeBioAssayEfficiency(");							// Added Rev 1.1
			strcat(sCommand, "ProbeBioAssayEfficiencyID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "I131_Efficiency FLOAT,");
			strcat(sCommand, "I125_Efficiency FLOAT,");
			strcat(sCommand, "I123_Efficiency FLOAT,");
			strcat(sCommand, "I131_I125_Contamination FLOAT,");
			strcat(sCommand, "I123_I125_Contamination FLOAT,");
			strcat(sCommand, "I131Measured INTEGER,");
			strcat(sCommand, "I131_Activity FLOAT,");
			strcat(sCommand, "I131_Date FLOAT,");
			strcat(sCommand, "I131_PrimaryEnergy FLOAT,");
			strcat(sCommand, "I131_startEV FLOAT,");
			strcat(sCommand, "I131_endEV FLOAT,");
			strcat(sCommand, "I131_I125_startEV FLOAT,");
			strcat(sCommand, "I131_I125_endEV FLOAT,");
			strcat(sCommand, "I131BackgroundID INTEGER,");
			strcat(sCommand, "I131_SpectraID INTEGER,");
			strcat(sCommand, "I131_ROICounts INTEGER,");
			strcat(sCommand, "I131_ROICPM FLOAT,");
			strcat(sCommand, "I131_BackgroundROICounts INTEGER,");
			strcat(sCommand, "I131_BackgroundROICPM FLOAT,");
			strcat(sCommand, "I131_NetROICPM FLOAT,");
			strcat(sCommand, "I131_I125_ROICounts INTEGER,");
			strcat(sCommand, "I131_I125_ROICPM FLOAT,");
			strcat(sCommand, "I131_I125_BackgroundROICounts INTEGER,");
			strcat(sCommand, "I131_I125_BackgroundROICPM FLOAT,");
			strcat(sCommand, "I131_I125_NetROICPM FLOAT,");
			strcat(sCommand, "I125Measured INTEGER,");
			strcat(sCommand, "I125_Activity FLOAT,");
			strcat(sCommand, "I125_Date FLOAT,");
			strcat(sCommand, "I125_PrimaryEnergy FLOAT,");
			strcat(sCommand, "I125_startEV FLOAT,");
			strcat(sCommand, "I125_endEV FLOAT,");
			strcat(sCommand, "I125BackgroundID INTEGER,");
			strcat(sCommand, "I125_SpectraID INTEGER,");
			strcat(sCommand, "I125_ROICounts INTEGER,");
			strcat(sCommand, "I125_ROICPM FLOAT,");
			strcat(sCommand, "I125_BackgroundROICounts INTEGER,");
			strcat(sCommand, "I125_BackgroundROICPM FLOAT,");
			strcat(sCommand, "I125_NetROICPM FLOAT,");
			strcat(sCommand, "I123Measured INTEGER,");
			strcat(sCommand, "I123_Activity FLOAT,");
			strcat(sCommand, "I123_Date FLOAT,");
			strcat(sCommand, "I123_PrimaryEnergy FLOAT,");
			strcat(sCommand, "I123_startEV FLOAT,");
			strcat(sCommand, "I123_endEV FLOAT,");
			strcat(sCommand, "I123_I125_startEV FLOAT,");
			strcat(sCommand, "I123_I125_endEV FLOAT,");
			strcat(sCommand, "I123BackgroundID INTEGER,");
			strcat(sCommand, "I123_SpectraID INTEGER,");
			strcat(sCommand, "I123_ROICounts INTEGER,");
			strcat(sCommand, "I123_ROICPM FLOAT,");
			strcat(sCommand, "I123_BackgroundROICounts INTEGER,");
			strcat(sCommand, "I123_BackgroundROICPM FLOAT,");
			strcat(sCommand, "I123_NetROICPM FLOAT,");
			strcat(sCommand, "I123_I125_ROICounts INTEGER,");
			strcat(sCommand, "I123_I125_ROICPM FLOAT,");
			strcat(sCommand, "I123_I125_BackgroundROICounts INTEGER,");
			strcat(sCommand, "I123_I125_BackgroundROICPM FLOAT,");
			strcat(sCommand, "I123_I125_NetROICPM FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ProbeBioAssayEfficiency, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating BioAssaySetting");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ProbeBioAssaySetting(");							// Added Rev 1.1
			strcat(sCommand, "ProbeBioAssaySettingID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "I131_Active INTEGER,");
			strcat(sCommand, "I125_Active INTEGER,");
			strcat(sCommand, "I123_Active INTEGER,");
			strcat(sCommand, "I131_Threshold FLOAT,");
			strcat(sCommand, "I125_Threshold FLOAT,");
			strcat(sCommand, "I123_Threshold FLOAT,");
			strcat(sCommand, "CountTime INTEGER,");
			strcat(sCommand, "ProbeDistance INTEGER,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ProbeBioAssaySetting, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating BioAssayTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ProbeBioAssayTest(");							// Added Rev 1.1
			strcat(sCommand, "ProbeBioAssayTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "UserName TEXT,");
			strcat(sCommand, "FirstName TEXT,");
			strcat(sCommand, "LastName TEXT,");
			strcat(sCommand, "ProbeDistance INTEGER,");
			strcat(sCommand, "I131_Active INTEGER,");
			strcat(sCommand, "I131_PrimaryEnergy FLOAT,");
			strcat(sCommand, "I131_startEV FLOAT,");
			strcat(sCommand, "I131_endEV FLOAT,");
			strcat(sCommand, "I131_ROICounts INTEGER,");
			strcat(sCommand, "I131_ROICPM FLOAT,");
			strcat(sCommand, "I131_BackgroundROICounts INTEGER,");
			strcat(sCommand, "I131_BackgroundROICPM FLOAT,");
			strcat(sCommand, "I131_NetROICPM FLOAT,");
			strcat(sCommand, "I131_Activity FLOAT,");
			strcat(sCommand, "I131_High INTEGER,");
			strcat(sCommand, "I131_I125_NetROICPM FLOAT,");
			strcat(sCommand, "I125_Active INTEGER,");
			strcat(sCommand, "I125_PrimaryEnergy FLOAT,");
			strcat(sCommand, "I125_startEV FLOAT,");
			strcat(sCommand, "I125_endEV FLOAT,");
			strcat(sCommand, "I125_ROICounts INTEGER,");
			strcat(sCommand, "I125_ROICPM FLOAT,");
			strcat(sCommand, "I125_BackgroundROICounts INTEGER,");
			strcat(sCommand, "I125_BackgroundROICPM FLOAT,");
			strcat(sCommand, "I125_NetROICPM FLOAT,");
			strcat(sCommand, "I125_NetROICPMCorrected FLOAT,");
			strcat(sCommand, "I125_Activity FLOAT,");
			strcat(sCommand, "I125_High INTEGER,");
			strcat(sCommand, "I123_Active INTEGER,");
			strcat(sCommand, "I123_PrimaryEnergy FLOAT,");
			strcat(sCommand, "I123_startEV FLOAT,");
			strcat(sCommand, "I123_endEV FLOAT,");
			strcat(sCommand, "I123_ROICounts INTEGER,");
			strcat(sCommand, "I123_ROICPM FLOAT,");
			strcat(sCommand, "I123_BackgroundROICounts INTEGER,");
			strcat(sCommand, "I123_BackgroundROICPM FLOAT,");
			strcat(sCommand, "I123_NetROICPM FLOAT,");
			strcat(sCommand, "I123_Activity FLOAT,");
			strcat(sCommand, "I123_High INTEGER,");
			strcat(sCommand, "I123_I125_NetROICPM FLOAT,");
			strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
			strcat(sCommand, "SpectraID INTEGER REFERENCES Spectra(SpectraID),");
			strcat(sCommand, "ProbeBioAssayEfficiencyID INTEGER REFERENCES ProbeBioAssayEfficiency(ProbeBioAssayEfficiencyID),");
			strcat(sCommand, "ProbeBioAssaySettingID INTEGER REFERENCES ProbeBioAssaySetting(ProbeBioAssaySettingID),");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Comment TEXT,");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ProbeBioAssayTest, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating TUProtocol");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ProbeTUProtocol(");							// Added Rev 1.1
			strcat(sCommand, "ProbeTUProtocolID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ProbeTUProtocolGroupID INTEGER,");
			strcat(sCommand, "ProtocolName TEXT,");
			strcat(sCommand, "NuclideID INTEGER,");
			strcat(sCommand, "DoseForm INTEGER,");					// 1=Capsule, 2=Liquid
			strcat(sCommand, "DoseMeasurementMethod INTEGER,");		// 1=Dose Measured, 2=Sum All Doses, 3=Measure one and Multiply by Number, 4=Measure Liquid and Multiply by Factor
			strcat(sCommand, "DoseDecayCorrect INTEGER,");			// 0=Measure reference with Patient, 1=Measure Dose and Calculate Decay Value with Patient;
			strcat(sCommand, "PreDoseMeasurement INTEGER,");		// 0=No PreDose Measurement, 1=PreDose Measurement
			strcat(sCommand, "ResidualMeasurement INTEGER,");		// 0=Residual Dose Measurement, 1=Residual Dose Measurement
			strcat(sCommand, "I123LowerEV FLOAT,");
			strcat(sCommand, "I123UpperEV FLOAT,");
			strcat(sCommand, "I131LowerEV FLOAT,");
			strcat(sCommand, "I131UpperEV FLOAT,");
			strcat(sCommand, "Tc99mLowerEV FLOAT,");
			strcat(sCommand, "Tc99mUpperEV FLOAT,");
			strcat(sCommand, "DefaultCountTime INTEGER,");
			strcat(sCommand, "DefaultDistance INTEGER,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ProbeTUProtocol, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating TUTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ProbeTUTest(");							// Added Rev 1.1
			strcat(sCommand, "ProbeTUTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ProbeTUTestPhase INTEGER,");
			strcat(sCommand, "PatientID TEXT,");
			strcat(sCommand, "FirstName TEXT,");
			strcat(sCommand, "LastName TEXT,");
			strcat(sCommand, "DateOfBirth FLOAT,");
			strcat(sCommand, "Sex TEXT,");
			strcat(sCommand, "Physician TEXT,");
			strcat(sCommand, "TechID TEXT,");
			strcat(sCommand, "NuclideName TEXT,");
			strcat(sCommand, "HalfLife FLOAT,");
			strcat(sCommand, "HalfLifeUnit INTEGER,");
			strcat(sCommand, "StartROI FLOAT,");
			strcat(sCommand, "EndROI FLOAT,");
			strcat(sCommand, "PeakEV FLOAT,");
			strcat(sCommand, "CountingTime INTEGER,");
			strcat(sCommand, "DoseMultiplier FLOAT,");
			strcat(sCommand, "DoseAdministeredOn FLOAT,");
			strcat(sCommand, "CalibrationActivity FLOAT,");
			strcat(sCommand, "CalibrationDate FLOAT,");
			strcat(sCommand, "LotNum TEXT,");
			strcat(sCommand, "ProbeDistance INTEGER,");
			strcat(sCommand, "AccessionNumber TEXT,");
			strcat(sCommand, "ProbeTUProtocolID INTEGER,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ProbeTUTest, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating TUPatientMeas");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ProbeTUPatientMeasurement(");							// Added Rev 1.1
			strcat(sCommand, "ProbeTUPatientMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "MeasurementNumber INTEGER,"); // 0 = Pre-Dose, 1 = First Measurement after Administration ...
			strcat(sCommand, "Neck1ROICounts INTEGER,");
			strcat(sCommand, "Neck1ROICPM FLOAT,");
			strcat(sCommand, "Neck2ROICounts INTEGER,");
			strcat(sCommand, "Neck2ROICPM FLOAT,");
			strcat(sCommand, "NeckROIAverageCPM FLOAT,");
			strcat(sCommand, "Leg1ROICounts INTEGER,");
			strcat(sCommand, "Leg1ROICPM FLOAT,");
			strcat(sCommand, "Leg2ROICounts INTEGER,");
			strcat(sCommand, "Leg2ROICPM FLOAT,");
			strcat(sCommand, "LegROIAverageCPM FLOAT,");
			strcat(sCommand, "PatientROINetCPM FLOAT,");
			strcat(sCommand, "AdjustedPatientROINetCPM FLOAT,");
			strcat(sCommand, "DoseROINetCPM FLOAT,");
			strcat(sCommand, "AdjustedDoseROINetCPM FLOAT,");
			strcat(sCommand, "Uptake FLOAT,");				// For Pre-Dose, -1 = Skipped, 0 = Measured
			strcat(sCommand, "ElapsedTime FLOAT,");
			strcat(sCommand, "Comment TEXT,");
			strcat(sCommand, "Neck1_SpectraID INTEGER,");
			strcat(sCommand, "Neck2_SpectraID INTEGER,");
			strcat(sCommand, "Leg1_SpectraID INTEGER,");
			strcat(sCommand, "Leg2_SpectraID INTEGER,");
			strcat(sCommand, "ProbeTUTestID INTEGER REFERENCES ProbeTUTest(ProbeTUTestID),");
			strcat(sCommand, "CreatedOn FLOAT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ProbeTUPatientMeasurement, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating TUTotalDoseMeas");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ProbeTUTotalDoseMeasurement(");							// Added Rev 1.1
			strcat(sCommand, "ProbeTUTotalDoseMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "MeasurementNumber INTEGER,"); // -1 = Residual, 0 = Administered Dose, 1 = First Measurement after Administration ...
			strcat(sCommand, "BackgroundROICounts INTEGER,");
			strcat(sCommand, "BackgroundROICPM FLOAT,");
			strcat(sCommand, "DoseROICPM FLOAT,");
			strcat(sCommand, "DoseROINetCPM FLOAT,");
			strcat(sCommand, "StartDateTime FLOAT,");
			strcat(sCommand, "EndDateTime FLOAT,");
			strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
			strcat(sCommand, "ProbeTUTestID INTEGER REFERENCES ProbeTUTest(ProbeTUTestID),");
			strcat(sCommand, "CreatedOn FLOAT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ProbeTUTotalDoseMeasurement, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating TUSingleDoseMeas");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ProbeTUSingleDoseMeasurement(");							// Added Rev 1.1
			strcat(sCommand, "ProbeTUSingleDoseMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "DoseSubUnit INTEGER,"); // 1=First Capsule, 2=Second Capsule ...
			strcat(sCommand, "Dose1ROICounts INTEGER,");
			strcat(sCommand, "Dose1ROICPM FLOAT,");
			strcat(sCommand, "Dose2ROICounts INTEGER,");
			strcat(sCommand, "Dose2ROICPM FLOAT,");
			strcat(sCommand, "DoseROIAverageCPM FLOAT,");
			strcat(sCommand, "Dose1_SpectraID INTEGER,");
			strcat(sCommand, "Dose2_SpectraID INTEGER,");
			strcat(sCommand, "ProbeTUTotalDoseMeasurementID INTEGER REFERENCES ProbeTUTotalDoseMeasurement(ProbeTUTotalDoseMeasurementID),");
			strcat(sCommand, "CreatedOn FLOAT");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ProbeTUSingleDoseMeasurement, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating AutoLinearityTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE AutoLinearityTest(");							// Added Rev 1.3
			strcat(sCommand, "AutoLinearityTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "SerialNumber TEXT,");
			strcat(sCommand, "Type INTEGER,");
			strcat(sCommand, "NuclideName TEXT,");
			strcat(sCommand, "HalfLife FLOAT,");
			strcat(sCommand, "HalfLifeUnit INTEGER,");
			strcat(sCommand, "IntervalMinutes INTEGER,");
			strcat(sCommand, "TotalMinutes INTEGER,");
			strcat(sCommand, "StartedOn FLOAT,");
			strcat(sCommand, "Comment TEXT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE AutoLinearityTest, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "CREATE INDEX idx_tbl_AutoLinearityTest_StartedOn ON AutoLinearityTest(StartedOn);");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE INDEX idx_tbl_AutoLinearityTest_StartedOn, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating AutoLinearityMeasurement");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE AutoLinearityMeasurement(");						// Added Rev 1.3
			strcat(sCommand, "AutoLinearityMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "AutoLinearityTestID INTEGER REFERENCES AutoLinearityTest(AutoLinearityTestID),");
			strcat(sCommand, "MeasuredActivity FLOAT,");
			strcat(sCommand, "MeasuredOn FLOAT,");
			strcat(sCommand, "Status INTEGER,");
			strcat(sCommand, "ElapsedMinutes INTEGER,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE AutoLinearityMeasurement, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating WellRBCSurvivalTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellRBCSurvivalTest(");							// Added Rev 1.3
			strcat(sCommand, "WellRBCSurvivalTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "WellRBCSurvivalTestPhase INTEGER,");
			strcat(sCommand, "PatientID TEXT,");
			strcat(sCommand, "FirstName TEXT,");
			strcat(sCommand, "LastName TEXT,");
			strcat(sCommand, "DateOfBirth FLOAT,");
			strcat(sCommand, "Sex TEXT,");
			strcat(sCommand, "Physician TEXT,");
			strcat(sCommand, "TechID TEXT,");
			strcat(sCommand, "NuclideName TEXT,");
			strcat(sCommand, "HalfLife FLOAT,");
			strcat(sCommand, "HalfLifeUnit INTEGER,");
			strcat(sCommand, "StartROI FLOAT,");
			strcat(sCommand, "EndROI FLOAT,");
			strcat(sCommand, "PeakEV FLOAT,");
			strcat(sCommand, "CountingTime INTEGER,");
			strcat(sCommand, "InjectionDate FLOAT,");
			strcat(sCommand, "CalibrationActivity FLOAT,");
			strcat(sCommand, "CalibrationDate FLOAT,");
			strcat(sCommand, "LotNum TEXT,");
			strcat(sCommand, "AccessionNumber TEXT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellRBCSurvivalTest, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating WellRBCSurvivalMeasurement");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellRBCSurvivalMeasurement(");							// Added Rev 1.3
			strcat(sCommand, "WellRBCSurvivalMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "MeasurementNumber INTEGER,");
			strcat(sCommand, "BackgroundROICounts INTEGER,");
			strcat(sCommand, "BackgroundROICPM FLOAT,");
			strcat(sCommand, "Sample1ROICounts INTEGER,");
			strcat(sCommand, "Sample1ROICPM FLOAT,");
			strcat(sCommand, "Sample2ROICounts INTEGER,");
			strcat(sCommand, "Sample2ROICPM FLOAT,");
			strcat(sCommand, "SampleROIAverageCPM FLOAT,");
			strcat(sCommand, "SampleROINetCPM FLOAT,");
			strcat(sCommand, "DecayCorrectedSampleROINetCPM FLOAT,");
			strcat(sCommand, "SampleHematocrit FLOAT,");
			strcat(sCommand, "Remaining FLOAT,");
			strcat(sCommand, "ElapsedDays FLOAT,");
			strcat(sCommand, "Comment TEXT,");
			strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
			strcat(sCommand, "Sample1_SpectraID INTEGER,");
			strcat(sCommand, "Sample2_SpectraID INTEGER,");
			strcat(sCommand, "WellRBCSurvivalTestID INTEGER REFERENCES WellRBCSurvivalTest(WellRBCSurvivalTestID),");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP))");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellRBCSurvivalMeasurement, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating WellRBCSurvivalResult");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE WellRBCSurvivalResult(");							// Added Rev 1.3
			strcat(sCommand, "WellRBCSurvivalResultID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "HalfDays FLOAT,");
			strcat(sCommand, "Y0 FLOAT,");
			strcat(sCommand, "X10 FLOAT,");
			strcat(sCommand, "Y60 FLOAT,");
			strcat(sCommand, "WellRBCSurvivalTestID INTEGER REFERENCES WellRBCSurvivalTest(WellRBCSurvivalTestID),");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP))");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE WellRBCSurvivalResult, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating ChamberDailyTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ChamberDailyTest(");							// Added Rev 1.5
			strcat(sCommand, "ChamberDailyTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ChamberSerialNumber TEXT,");
			strcat(sCommand, "ChamberType INTEGER,");
			strcat(sCommand, "TwoStageChamber INTEGER,");
			strcat(sCommand, "DataCheckTextEnglish TEXT,");
			strcat(sCommand, "DataCheckTextFrench TEXT,");
			strcat(sCommand, "DataCheckFailed INTEGER,");
			strcat(sCommand, "DataCheckCRC TEXT,");
			strcat(sCommand, "ChamberZeroID INTEGER,");
			strcat(sCommand, "ChamberBackgroundID INTEGER,");
			strcat(sCommand, "ChamberVoltageID INTEGER,");
			strcat(sCommand, "ChamberAccuracyTestID INTEGER,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ChamberDailyTest, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating ChamberZero");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ChamberZero(");								// Added Rev 1.5
			strcat(sCommand, "ChamberZeroID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ChamberSerialNumber TEXT,");
			strcat(sCommand, "ChamberType INTEGER,");
			strcat(sCommand, "TwoStageChamber INTEGER,");
			strcat(sCommand, "ZeroStatus INTEGER,");
			strcat(sCommand, "ZeroTextEnglish TEXT,");
			strcat(sCommand, "ZeroTextFrench TEXT,");
			strcat(sCommand, "ZeroValue FLOAT,");
			strcat(sCommand, "MeasuredOn FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ChamberZero, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating ChamberBackground");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ChamberBackground(");								// Added Rev 1.5
			strcat(sCommand, "ChamberBackgroundID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ChamberSerialNumber TEXT,");
			strcat(sCommand, "ChamberType INTEGER,");
			strcat(sCommand, "TwoStageChamber INTEGER,");
			strcat(sCommand, "BackgroundStatus INTEGER,");
			strcat(sCommand, "BackgroundTextEnglish TEXT,");
			strcat(sCommand, "BackgroundTextFrench TEXT,");
			strcat(sCommand, "BackgroundValue FLOAT,");
			strcat(sCommand, "MeasuredOn FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ChamberBackground, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating ChamberVoltage");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ChamberVoltage(");									// Added Rev 1.5
			strcat(sCommand, "ChamberVoltageID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ChamberSerialNumber TEXT,");
			strcat(sCommand, "ChamberType INTEGER,");
			strcat(sCommand, "TwoStageChamber INTEGER,");
			strcat(sCommand, "NominalVoltage FLOAT,");
			strcat(sCommand, "MinVoltage FLOAT,");
			strcat(sCommand, "MaxVoltage FLOAT,");
			strcat(sCommand, "VoltageStatus INTEGER,");
			strcat(sCommand, "VoltageTextEnglish TEXT,");
			strcat(sCommand, "VoltageTextFrench TEXT,");
			strcat(sCommand, "VoltageValue FLOAT,");
			strcat(sCommand, "MeasuredOn FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ChamberVoltage, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating ChamberAccuracyTest");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ChamberAccuracyTest(");								// Added Rev 1.5
			strcat(sCommand, "ChamberAccuracyTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ChamberSerialNumber TEXT,");
			strcat(sCommand, "ChamberType INTEGER,");
			strcat(sCommand, "TwoStageChamber INTEGER,");
			strcat(sCommand, "ConstancyNuclide TEXT,");
			strcat(sCommand, "ConstancyNuclideSerialNumber TEXT,");
			strcat(sCommand, "ConstancyVoltage FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
			strcat(sCommand, "InactiveReason TEXT,");
			strcat(sCommand, "Inactive INTEGER");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ChamberAccuracyTest, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating ChamberAccuracyMeasurement");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ChamberAccuracyMeasurement(");						// Added Rev 1.5
			strcat(sCommand, "ChamberAccuracyMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ChamberAccuracyTestID INTEGER REFERENCES ChamberAccuracyTest(ChamberAccuracyTestID),"),
			strcat(sCommand, "SourceNuclide TEXT,");
			strcat(sCommand, "SourceNuclideIndex INTEGER,");
			strcat(sCommand, "SourceSerialNumber TEXT,");
			strcat(sCommand, "SourceCalActivity FLOAT,");
			strcat(sCommand, "SourceCalDate FLOAT,");
			strcat(sCommand, "MeasuredOn FLOAT,");
			strcat(sCommand, "CalculatedActivityTextEnglish TEXT,");
			strcat(sCommand, "CalculatedActivityTextFrench TEXT,");
			strcat(sCommand, "CalculatedActivity FLOAT,");
			strcat(sCommand, "Response FLOAT,");
			strcat(sCommand, "HalfLife FLOAT,");
			strcat(sCommand, "HalfLifeUnit INTEGER,");
			strcat(sCommand, "MeasuredActivityTextEnglish TEXT,");
			strcat(sCommand, "MeasuredActivityTextFrench TEXT,");
			strcat(sCommand, "MeasuredActivity FLOAT,");
			strcat(sCommand, "DeviationStatus INTEGER,");
			strcat(sCommand, "DeviationTextEnglish TEXT,");
			strcat(sCommand, "DeviationTextFrench TEXT,");
			strcat(sCommand, "Deviation FLOAT");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ChamberAccuracyMeasurement, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating ChamberAutoConstancy");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ChamberAutoConstancy(");							// Added Rev 1.5
			strcat(sCommand, "ChamberAutoConstancyID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ChamberAccuracyTestID INTEGER REFERENCES ChamberAccuracyTest(ChamberAccuracyTestID),"),
			strcat(sCommand, "ConstancyNuclide TEXT,");
			strcat(sCommand, "ConstancyNuclideIndex INTEGER,");
			strcat(sCommand, "Response FLOAT,");
			strcat(sCommand, "HalfLife FLOAT,");
			strcat(sCommand, "HalfLifeUnit INTEGER,");
			strcat(sCommand, "ConstancyActivityTextEnglish TEXT,");
			strcat(sCommand, "ConstancyActivityTextFrench TEXT,");
			strcat(sCommand, "ConstancyActivity FLOAT");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ChamberAutoConstancy, %d\n", *capStatus);
#endif

			if(recreate){
				SetAmuletString(102, "Creating Hotkey");
				SetAmuletByte(100, 0xFF);
			}
			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE Hotkey(");
			strcat(sCommand, "HotkeyID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ChamberType INTEGER,");
			strcat(sCommand, "HotkeyMenu INTEGER,");
			strcat(sCommand, "HotkeyIndex INTEGER,");
			strcat(sCommand, "HotkeyNuclideIndex INTEGER,");
			strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP))");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE Hotkey, %d\n", *capStatus);
#endif

/*			*sCommand = 0;
			strcat(sCommand, "CREATE TABLE ProbeTUNormalRange(");							// Added Rev 1.1
			strcat(sCommand, "ProbeTUNormalRangeID INTEGER PRIMARY KEY AUTOINCREMENT,");
			strcat(sCommand, "ProbeTUProtocolGroupID INTEGER,");
			strcat(sCommand, "Hour1 INTEGER,");
			strcat(sCommand, "Hour2 INTEGER,");
			strcat(sCommand, "Hour3 INTEGER,");
			strcat(sCommand, "Hour4 INTEGER,");
			strcat(sCommand, "MinValue1 FLOAT,");
			strcat(sCommand, "MinValue2 FLOAT,");
			strcat(sCommand, "MinValue3 FLOAT,");
			strcat(sCommand, "MinValue4 FLOAT,");
			strcat(sCommand, "MaxValue1 FLOAT,");
			strcat(sCommand, "MaxValue2 FLOAT,");
			strcat(sCommand, "MaxValue3 FLOAT,");
			strcat(sCommand, "MaxValue4 FLOAT,");
			strcat(sCommand, "CreatedOn FLOAT");
			strcat(sCommand, ");");
			d_command(sCommand);
			service_watchdog();
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: CREATE TABLE ProbeTUNormalRange, %d\n", *capStatus);
#endif

			ChDailyTest
	ChDailyTestID
	SerialNumber
	Type
	FeatureFlag

	AutoZeroVoltage
	AutoZeroStatus
	AutoZero
	Zero

	DataCheckStatus
	DataCheckCode

	ChBackgroundID
	ChChamberVoltageID
	ChAccuracyTestID

	CreatedOn

ChBackground
	ChBackgroundID
	SerialNumber
	Type
	FeatureFlag

	BackgroundActivity
	BackgroundStatus
	Background

	DailyTestID
	CreatedOn

ChChamberVoltage
	ChChamberVoltageID
	SerialNumber
	Type
	FeatureFlag

	ChamberVoltage
	ChamberVoltageStatus

	DailyTestID
	CreatedOn

ChAccuracyTest
	ChAccuracyTestID
	SerialNumber
	Type
	FeatureFlag
	CreatedOn

ChAccuracyMeasurement
	ChAccuracyMeasurementID
	Nuclide
	HalfLife
	HalfLifeUnit
	Response
	SerialNumber
	CalActivity
	CalDate
	Activity
	MeasuredOn
	CalcActivity
	Deviation
	Status
	ChAccuracyTestID
	CreatedOn

*/
			db_close(capStatus, capDB);

			if(recreate){
				SetAmuletString(102, "Openning DB");
				SetAmuletByte(100, 0xFF);
			}
			//db_open_v2(capStatus, "capintec.db", &capDB, SQLITE_OPEN_READWRITE, NULL);
			db_open_v2(capStatus, "temp.db", &capDB, SQLITE_OPEN_READWRITE, NULL);

			if(recreate){
				SetAmuletString(102, "Default Config");
				SetAmuletByte(100, 0xFF);
			}

			d_command("BEGIN;");

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('DBRev',:DBRev);");
			d_bind_double(":DBRev", 1.7);
			d_step();
			d_finalize();
			service_watchdog();

			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellSerialNum',0);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700SerialNum',0);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Well700SerialNum',0);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellHV',660);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700HV',3800);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Well700HV',3800);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellThreshold',80);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700Threshold',80);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Well700Threshold',80);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700Installed', 3);"); // DET_PROBE700
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Well700Installed', 4);"); // DET_WELL700
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellChannels', 256);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700Channels', 512);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Well700Channels', 512);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellAutoCalThreshold', 0);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700AutoCalThreshold', 0);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Well700AutoCalThreshold', 0);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellTime0',60);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellTime1',60);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellTime2',60);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellTime3',60);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellTime4',60);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellTime5',60);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellTime6',60);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellTime7',60);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('Branding',0);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('DefaultKEV',1);");
			d_command(sCommand);
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('DetectorTestSourceSN',:DetectorTestSourceSN);");
			d_bind_text(":DetectorTestSourceSN", "$FTXT$");
			d_step();
			d_finalize();
			service_watchdog();
			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('DetectorTestSourceActivity',:DetectorTestSourceActivity);");
			d_bind_double(":DetectorTestSourceActivity", -1.0);
			d_step();
			d_finalize();
			service_watchdog();
			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('DetectorTestSourceDate',:DetectorTestSourceDate);");
			d_bind_int(":DetectorTestSourceDate", 0);
			d_step();
			d_finalize();
			service_watchdog();
			strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('SecurityMode',0);");
			d_command(sCommand);
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI131LowerEV',:BioAssayI131LowerEV);");
			d_bind_double(":BioAssayI131LowerEV", 292.0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI131UpperEV',:BioAssayI131UpperEV);");
			d_bind_double(":BioAssayI131UpperEV", 437.0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI125LowerEV',:BioAssayI125LowerEV);");
			d_bind_double(":BioAssayI125LowerEV", 20.0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI125UpperEV',:BioAssayI125UpperEV);");
			d_bind_double(":BioAssayI125UpperEV", 50.0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI123LowerEV',:BioAssayI123LowerEV);");
			d_bind_double(":BioAssayI123LowerEV", 143.0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI123UpperEV',:BioAssayI123UpperEV);");
			d_bind_double(":BioAssayI123UpperEV", 191.0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour1',:TUHour1);");
			d_bind_int(":TUHour1", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour2',:TUHour2);");
			d_bind_int(":TUHour2", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour3',:TUHour3);");
			d_bind_int(":TUHour3", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour4',:TUHour4);");
			d_bind_int(":TUHour4", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour1',:TUMinHour1);");
			d_bind_int(":TUMinHour1", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour2',:TUMinHour2);");
			d_bind_int(":TUMinHour2", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour3',:TUMinHour3);");
			d_bind_int(":TUMinHour3", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour4',:TUMinHour4);");
			d_bind_int(":TUMinHour4", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour1',:TUMaxHour1);");
			d_bind_int(":TUMaxHour1", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour2',:TUMaxHour2);");
			d_bind_int(":TUMaxHour2", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour3',:TUMaxHour3);");
			d_bind_int(":TUMaxHour3", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour4',:TUMaxHour4);");
			d_bind_int(":TUMaxHour4", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('RBCSurvivalMinNormal',:RBCSurvivalMinNormal);");
			d_bind_double(":RBCSurvivalMinNormal", -1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('RBCSurvivalMaxNormal',:RBCSurvivalMaxNormal);");
			d_bind_double(":RBCSurvivalMaxNormal", -1);
			d_step();
			d_finalize();
			service_watchdog();

			strcpy(sCommand, "INSERT INTO User(UserName, Password, FirstName, LastName, Role, Inactive) VALUES('root', 'AlphaDrive', 'Super', 'User', 3, 0);");
			d_command(sCommand);
			service_watchdog();
			strcpy(sCommand, "INSERT INTO User(UserName, Password, FirstName, LastName, Role, Inactive) VALUES('Guest', '', 'Restricted', 'User', 0, 0);");
			d_command(sCommand);
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TimeFormat',:TimeFormat);");
			d_bind_int(":TimeFormat", 0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('USBDeviceProtocol',:Protocol);");
			d_bind_int(":Protocol", 0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FutureDateInput',:FutureDateInput);");
			d_bind_int(":FutureDateInput", 0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('Language',:Language);");
			d_bind_int(":Language", 0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FeedLabel',:FeedLabel);");
			d_bind_int(":FeedLabel", 1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('MolyStreamlined',:MolyStreamlined);");
			d_bind_int(":MolyStreamlined", 1);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('Key0',:Key0);");
			d_bind_int(":Key0", 0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('Key1',:Key1);");
			d_bind_int(":Key1", 0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('Password',:Password);");
			d_bind_int(":Password", 0);
			d_step();
			d_finalize();
			service_watchdog();

			d_command("COMMIT;");

			d_command("BEGIN;");
			if(recreate){
				SetAmuletString(102, "Default BioAssay");
				SetAmuletByte(100, 0xFF);
			}
			d_prepare_v2("INSERT INTO ProbeBioAssayEfficiency(I131_Efficiency,I125_Efficiency,I123_Efficiency,I131_I125_Contamination,I123_I125_Contamination,I131Measured,I131_Activity,I131_Date,I131_PrimaryEnergy,I131_startEV,I131_endEV,I131_I125_startEV,I131_I125_endEV,I131BackgroundID,I131_SpectraID,I131_ROICounts,I131_ROICPM,I131_BackgroundROICounts,I131_BackgroundROICPM,I131_NetROICPM,I131_I125_ROICounts,I131_I125_ROICPM,I131_I125_BackgroundROICounts,I131_I125_BackgroundROICPM,I131_I125_NetROICPM,I125Measured,I125_Activity,I125_Date,I125_PrimaryEnergy,I125_startEV,I125_endEV,I125BackgroundID,I125_SpectraID,I125_ROICounts,I125_ROICPM,I125_BackgroundROICounts,I125_BackgroundROICPM,I125_NetROICPM,I123Measured,I123_Activity,I123_Date,I123_PrimaryEnergy,I123_startEV,I123_endEV,I123_I125_startEV,I123_I125_endEV,I123BackgroundID,I123_SpectraID,I123_ROICounts,I123_ROICPM,I123_BackgroundROICounts,I123_BackgroundROICPM,I123_NetROICPM,I123_I125_ROICounts,I123_I125_ROICPM,I123_I125_BackgroundROICounts,I123_I125_BackgroundROICPM,I123_I125_NetROICPM,Inactive) VALUES(:I131_Efficiency,:I125_Efficiency,:I123_Efficiency,:I131_I125_Contamination,:I123_I125_Contamination,:I131Measured,:I131_Activity,:I131_Date,:I131_PrimaryEnergy,:I131_startEV,:I131_endEV,:I131_I125_startEV,:I131_I125_endEV,:I131BackgroundID,:I131_SpectraID,:I131_ROICounts,:I131_ROICPM,:I131_BackgroundROICounts,:I131_BackgroundROICPM,:I131_NetROICPM,:I131_I125_ROICounts,:I131_I125_ROICPM,:I131_I125_BackgroundROICounts,:I131_I125_BackgroundROICPM,:I131_I125_NetROICPM,:I125Measured,:I125_Activity,:I125_Date,:I125_PrimaryEnergy,:I125_startEV,:I125_endEV,:I125BackgroundID,:I125_SpectraID,:I125_ROICounts,:I125_ROICPM,:I125_BackgroundROICounts,:I125_BackgroundROICPM,:I125_NetROICPM,:I123Measured,:I123_Activity,:I123_Date,:I123_PrimaryEnergy,:I123_startEV,:I123_endEV,:I123_I125_startEV,:I123_I125_endEV,:I123BackgroundID,:I123_SpectraID,:I123_ROICounts,:I123_ROICPM,:I123_BackgroundROICounts,:I123_BackgroundROICPM,:I123_NetROICPM,:I123_I125_ROICounts,:I123_I125_ROICPM,:I123_I125_BackgroundROICounts,:I123_I125_BackgroundROICPM,:I123_I125_NetROICPM,:Inactive);");
			d_bind_double(":I131_Efficiency",-1.0);
			d_bind_double(":I125_Efficiency",-1.0);
			d_bind_double(":I123_Efficiency",-1.0);
			d_bind_double(":I131_I125_Contamination",0.085);
			d_bind_double(":I123_I125_Contamination",0.92);
			d_bind_int(":I131Measured",0);
			d_bind_double(":I131_Activity",-1);
			d_bind_double(":I131_Date",0);
			d_bind_double(":I131_PrimaryEnergy",-1);
			d_bind_double(":I131_startEV", 0);
			d_bind_double(":I131_endEV", 0);
			d_bind_double(":I131_I125_startEV", 0);
			d_bind_double(":I131_I125_endEV", 0);
			d_bind_int64(":I131BackgroundID",-1);
			d_bind_int64(":I131_SpectraID",-1);
			d_bind_int(":I131_ROICounts",0);
			d_bind_double(":I131_ROICPM",0);
			d_bind_int(":I131_BackgroundROICounts",0);
			d_bind_double(":I131_BackgroundROICPM",0);
			d_bind_double(":I131_NetROICPM",0);
			d_bind_int(":I131_I125_ROICounts",0);
			d_bind_double(":I131_I125_ROICPM",0);
			d_bind_int(":I131_I125_BackgroundROICounts",0);
			d_bind_double(":I131_I125_BackgroundROICPM",0);
			d_bind_double(":I131_I125_NetROICPM",0);
			d_bind_int(":I125Measured",0);
			d_bind_double(":I125_Activity",-1);
			d_bind_double(":I125_Date",0);
			d_bind_double(":I125_PrimaryEnergy",-1);
			d_bind_double(":I125_startEV", 0);
			d_bind_double(":I125_endEV", 0);
			d_bind_int64(":I125BackgroundID",-1);
			d_bind_int64(":I125_SpectraID",-1);
			d_bind_int(":I125_ROICounts",0);
			d_bind_double(":I125_ROICPM",0);
			d_bind_int(":I125_BackgroundROICounts",0);
			d_bind_double(":I125_BackgroundROICPM",0);
			d_bind_double(":I125_NetROICPM",0);
			d_bind_int(":I123Measured",0);
			d_bind_double(":I123_Activity",-1);
			d_bind_double(":I123_Date",0);
			d_bind_double(":I123_PrimaryEnergy",-1);
			d_bind_double(":I123_startEV", 0);
			d_bind_double(":I123_endEV", 0);
			d_bind_double(":I123_I125_startEV", 0);
			d_bind_double(":I123_I125_endEV", 0);
			d_bind_int64(":I123BackgroundID",-1);
			d_bind_int64(":I123_SpectraID",-1);
			d_bind_int(":I123_ROICounts",0);
			d_bind_double(":I123_ROICPM",0);
			d_bind_int(":I123_BackgroundROICounts",0);
			d_bind_double(":I123_BackgroundROICPM",0);
			d_bind_double(":I123_NetROICPM",0);
			d_bind_int(":I123_I125_ROICounts",0);
			d_bind_double(":I123_I125_ROICPM",0);
			d_bind_int(":I123_I125_BackgroundROICounts",0);
			d_bind_double(":I123_I125_BackgroundROICPM",0);
			d_bind_double(":I123_I125_NetROICPM",0);
			d_bind_int(":Inactive",0);
			d_step();
			d_finalize();
			service_watchdog();

			d_prepare_v2("INSERT INTO ProbeBioAssaySetting(I131_Active,I125_Active,I123_Active,I131_Threshold,I125_Threshold,I123_Threshold,CountTime,ProbeDistance,Inactive) VALUES(:I131_Active,:I125_Active,:I123_Active,:I131_Threshold,:I125_Threshold,:I123_Threshold,:CountTime,:ProbeDistance,:Inactive);");
			d_bind_int(":I131_Active",1);
			d_bind_int(":I125_Active",1);
			d_bind_int(":I123_Active",1);
			d_bind_double(":I131_Threshold",0.00000004);
			d_bind_double(":I125_Threshold",0.00000012);
			d_bind_double(":I123_Threshold",0.0000005);
			d_bind_int(":CountTime",60);
			d_bind_int(":ProbeDistance",25);
			d_bind_int(":Inactive",0);
			d_step();
			d_finalize();
			service_watchdog();
			d_command("COMMIT;");

			d_command("BEGIN;");
			if(recreate){
				SetAmuletString(102, "Default Wipe");
				SetAmuletByte(100, 0xFF);
			}
			DB_CreateWellWipeType("Background", 3000, 60);
			service_watchdog();
			DB_CreateWellWipeType("Work Area", 2000, 60);
			service_watchdog();
			DB_CreateWellWipeType("Unrestricted Area", 200, 60);
			service_watchdog();
			DB_CreateWellWipeType("Sealed Source", 11100, 60);
			service_watchdog();
			DB_CreateWellWipeType("Package", 200, 60);
			service_watchdog();
			d_command("COMMIT;");

			// Populate User Nuclide from EEPROM
			if(!recreate){
				EE_READ(init_str, (uchar *) initstr);
				if(strncmp(initstr, cap_str, 8) == 0){
					DB_moveUserNuclideFromEEPROM();
					DB_moveUserCalFromEEPROM();
					DB_moveGencfFromEEPROM();
				}else{
					DB_createEmptyUserNuclide(recreate);
					DB_createEmptyUserCal(recreate);
					d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FullSpectrumEffCapracT',:FullSpectrumEffCapracT);");
					d_bind_double(":FullSpectrumEffCapracT", 37.04);
					d_step();
					d_finalize();
					service_watchdog();
					d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FullSpectrumEffCaptusT',:FullSpectrumEffCaptusT);");
					d_bind_double(":FullSpectrumEffCaptusT", 65.0);
					d_step();
					d_finalize();
					service_watchdog();
				}
			}else{
				if(recreate){
					SetAmuletString(102, "Default User Nuclide");
					SetAmuletByte(100, 0xFF);
				}
				DB_createEmptyUserNuclide(recreate);
				if(recreate){
					SetAmuletString(102, "Default User Cal");
					SetAmuletByte(100, 0xFF);
				}
				DB_createEmptyUserCal(recreate);
				if(recreate){
					SetAmuletString(102, "Default Full Spectrum");
					SetAmuletByte(100, 0xFF);
				}
				d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FullSpectrumEffCapracT',:FullSpectrumEffCapracT);");
				d_bind_double(":FullSpectrumEffCapracT", 37.04);
				d_step();
				d_finalize();
				service_watchdog();
				d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FullSpectrumEffCaptusT',:FullSpectrumEffCaptusT);");
				d_bind_double(":FullSpectrumEffCaptusT", 65.0);
				d_step();
				d_finalize();
				service_watchdog();
			}

			if(recreate){
				SetAmuletString(102, "User Eff");
				SetAmuletByte(100, 0xFF);
			}
			DB_moveUserEffFromFile(recreate);

			if(recreate){
				SetAmuletString(102, "Hotkey");
				SetAmuletByte(100, 0xFF);
			}
			DB_PopulateDefaultHotkeys();
			DB_WriteAllHotkeyWithInsert();

			if(recreate){
				SetAmuletString(102, "Close DB");
				SetAmuletByte(100, 0xFF);
			}
			db_close(capStatus, capDB);
			service_watchdog();

			if(recreate){
				SetAmuletString(102, "Shutdown DB");
				SetAmuletByte(100, 0xFF);
			}
			db_shutdown(capStatus);
			service_watchdog();

			f_rename("temp.db", "capintec.db");
		}

		if(*capStatus == SQLITE_OK){
			db_initialize(capStatus);
			db_open_v2(capStatus, "capintec.db", &capDB, SQLITE_OPEN_READWRITE, NULL);

			*sCommand = 0;
			strcat(sCommand, "PRAGMA foreign_keys = 1;");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: PRAGMA foreign_keys, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "PRAGMA cache_size = 2000;");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: PRAGMA cache_size, %d\n", *capStatus);
#endif

			*sCommand = 0;
			strcat(sCommand, "PRAGMA page_size = 1024;");
			d_command(sCommand);
#ifdef PRINTF_ERRORS
			if(*capStatus != SQLITE_OK)
				printf("Error: PRAGMA page_size, %d\n", *capStatus);
#endif

			if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'DBRev';")){
				if(*capStatus == SQLITE_ROW){
					fDBRev = d_column_double(0);
					d_finalize();
					if((fDBRev > .95) && (fDBRev < 1.05)){
						strcpy(sCommand, "ALTER TABLE AutoCal ADD COLUMN NumOfChannels INTEGER;");
						d_command(sCommand);

						strcpy(sCommand, "ALTER TABLE AutoCal ADD COLUMN Sigma_Lip FLOAT;");
						d_command(sCommand);

						strcpy(sCommand, "UPDATE AutoCal SET NumOfChannels = 256 WHERE NumOfChannels IS NULL;"); // 1.0 was released as Caprac-t, which was only 256 channels
						d_command(sCommand);

						d_prepare_v2("UPDATE AutoCal SET Sigma_Lip = :Sigma_Lip WHERE Sigma_Lip IS NULL");
						d_bind_double(":Sigma_Lip", 0.0);
						d_step();
						d_finalize();

						strcpy(sCommand, "ALTER TABLE WellSystemTest ADD COLUMN DetectorTest INTEGER;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE WellSystemTest SET DetectorTest = 1 WHERE DetectorTest IS NULL;"); // 1.0 was released as Caprac-t, which was only DET_WELL
						d_command(sCommand);

						strcpy(sCommand, "ALTER TABLE WellMDATest ADD COLUMN DetectorTest INTEGER;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE WellMDATest SET DetectorTest = 1 WHERE DetectorTest IS NULL;"); // 1.0 was released as Caprac-t, which was only DET_WELL
						d_command(sCommand);

						strcpy(sCommand, "ALTER TABLE WellChiSquareTest ADD COLUMN DetectorTest INTEGER;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE WellChiSquareTest SET DetectorTest = 1 WHERE DetectorTest IS NULL;"); // 1.0 was released as Caprac-t, which was only DET_WELL
						d_command(sCommand);

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE UserNuclide(");							// Added Rev 1.1
						strcat(sCommand, "UserNuclideID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "NucIndexOffset INTEGER,");
						strcat(sCommand, "Show INTEGER,");
						strcat(sCommand, "HalfLife FLOAT,");
						strcat(sCommand, "Response_R FLOAT,");
						strcat(sCommand, "Response_P FLOAT,");
						strcat(sCommand, "Response_UserR FLOAT,");
						strcat(sCommand, "Response_UserP FLOAT,");
						strcat(sCommand, "Name TEXT,");
						strcat(sCommand, "FullName TEXT,");
						strcat(sCommand, "Code TEXT,");
						strcat(sCommand, "HLUnit INTERGER,");
						strcat(sCommand, "Energy1 FLOAT,");
						strcat(sCommand, "Energy2 FLOAT,");
						strcat(sCommand, "Energy3 FLOAT,");
						strcat(sCommand, "WellEfficiency FLOAT,");
						strcat(sCommand, "BetaEfficiency FLOAT,");
						strcat(sCommand, "Probe700Efficiency FLOAT,");
						strcat(sCommand, "Well700Efficiency FLOAT,");
						strcat(sCommand, "UserEnergy1 FLOAT,");
						strcat(sCommand, "UserEnergy2 FLOAT,");
						strcat(sCommand, "UserEnergy3 FLOAT,");
						strcat(sCommand, "UserWellEfficiency FLOAT,");
						strcat(sCommand, "UserBetaEfficiency FLOAT,");
						strcat(sCommand, "UserProbe700Efficiency FLOAT,");
						strcat(sCommand, "UserWell700Efficiency FLOAT,");
						strcat(sCommand, "WellMeasuredOn FLOAT,");
						strcat(sCommand, "BetaMeasuredOn FLOAT,");
						strcat(sCommand, "Probe700MeasuredOn FLOAT,");
						strcat(sCommand, "Well700MeasuredOn FLOAT,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE UserNuclide, %d\n", *capStatus);
#endif
						// Populate User Nuclide from EEPROM
						DB_moveUserNuclideFromEEPROM();

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE UserCal(");							// Added Rev 1.1
						strcat(sCommand, "UserCalID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "UserCalIndexOffset INTEGER,");
						strcat(sCommand, "NuclideIndex INTEGER,");
						strcat(sCommand, "ChamberType INTEGER,");
						strcat(sCommand, "Response FLOAT,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE UserCal, %d\n", *capStatus);
#endif
						// Populate User Cal from EEPROM
						DB_moveUserCalFromEEPROM();

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE UserEff(");							// Added Rev 1.1
						strcat(sCommand, "UserEffID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "UserEffIndexOffset INTEGER,");
						strcat(sCommand, "NuclideIndex INTEGER,");
						strcat(sCommand, "UserEnergy1 FLOAT,");
						strcat(sCommand, "UserEnergy2 FLOAT,");
						strcat(sCommand, "UserEnergy3 FLOAT,");
						strcat(sCommand, "UserWellEff FLOAT,");
						strcat(sCommand, "UserBetaEff FLOAT,");
						strcat(sCommand, "UserProbe700Eff FLOAT,");
						strcat(sCommand, "UserWell700Eff FLOAT,");
						strcat(sCommand, "WellMeasuredOn FLOAT,");
						strcat(sCommand, "BetaMeasuredOn FLOAT,");
						strcat(sCommand, "Probe700MeasuredOn FLOAT,");
						strcat(sCommand, "Well700MeasuredOn FLOAT,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE UserEff, %d\n", *capStatus);
#endif
						// Populate UserEff from usereff.bin file
						DB_moveUserEffFromFile(FALSE);

						DB_moveGencfFromEEPROM();

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE User(");							// Added Rev 1.1
						strcat(sCommand, "UserID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "UserName TEXT,");
						strcat(sCommand, "Password TEXT,");
						strcat(sCommand, "FirstName TEXT,");
						strcat(sCommand, "LastName TEXT,");
						strcat(sCommand, "Role INTEGER,"); // 0 = Guest, 1 = User, 2 = Administrator, 3 = SuperUser
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)), ");
						strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE User, %d\n", *capStatus);
#endif

						strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('SecurityMode',0);");
						d_command(sCommand);
						service_watchdog();
						strcpy(sCommand, "INSERT INTO Config(SettingName,SettingValue) VALUES('DefaultKEV',1);");
						d_command(sCommand);
						service_watchdog();
						strcpy(sCommand, "UPDATE UserEff SET UserEnergy1 = -1, UserEnergy2 = -1, UserEnergy3 = -1 WHERE Inactive = 0 AND NuclideIndex != -1;");
						d_command(sCommand);
						service_watchdog();
						strcpy(sCommand, "INSERT INTO User(UserName, Password, FirstName, LastName, Role, Inactive) VALUES('root', 'AlphaDrive', 'Super', 'User', 3, 0);");
						d_command(sCommand);
						service_watchdog();
						strcpy(sCommand, "INSERT INTO User(UserName, Password, FirstName, LastName, Role, Inactive) VALUES('Guest', '', 'Restricted', 'User', 0, 0);");
						d_command(sCommand);
						service_watchdog();

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ProbeBioAssayEfficiency(");							// Added Rev 1.1
						strcat(sCommand, "ProbeBioAssayEfficiencyID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "I131_Efficiency FLOAT,");
						strcat(sCommand, "I125_Efficiency FLOAT,");
						strcat(sCommand, "I123_Efficiency FLOAT,");
						strcat(sCommand, "I131_I125_Contamination FLOAT,");
						strcat(sCommand, "I123_I125_Contamination FLOAT,");
						strcat(sCommand, "I131Measured INTEGER,");
						strcat(sCommand, "I131_Activity FLOAT,");
						strcat(sCommand, "I131_Date FLOAT,");
						strcat(sCommand, "I131_PrimaryEnergy FLOAT,");
						strcat(sCommand, "I131_startEV FLOAT,");
						strcat(sCommand, "I131_endEV FLOAT,");
						strcat(sCommand, "I131_I125_startEV FLOAT,");
						strcat(sCommand, "I131_I125_endEV FLOAT,");
						strcat(sCommand, "I131BackgroundID INTEGER,");
						strcat(sCommand, "I131_SpectraID INTEGER,");
						strcat(sCommand, "I131_ROICounts INTEGER,");
						strcat(sCommand, "I131_ROICPM FLOAT,");
						strcat(sCommand, "I131_BackgroundROICounts INTEGER,");
						strcat(sCommand, "I131_BackgroundROICPM FLOAT,");
						strcat(sCommand, "I131_NetROICPM FLOAT,");
						strcat(sCommand, "I131_I125_ROICounts INTEGER,");
						strcat(sCommand, "I131_I125_ROICPM FLOAT,");
						strcat(sCommand, "I131_I125_BackgroundROICounts INTEGER,");
						strcat(sCommand, "I131_I125_BackgroundROICPM FLOAT,");
						strcat(sCommand, "I131_I125_NetROICPM FLOAT,");
						strcat(sCommand, "I125Measured INTEGER,");
						strcat(sCommand, "I125_Activity FLOAT,");
						strcat(sCommand, "I125_Date FLOAT,");
						strcat(sCommand, "I125_PrimaryEnergy FLOAT,");
						strcat(sCommand, "I125_startEV FLOAT,");
						strcat(sCommand, "I125_endEV FLOAT,");
						strcat(sCommand, "I125BackgroundID INTEGER,");
						strcat(sCommand, "I125_SpectraID INTEGER,");
						strcat(sCommand, "I125_ROICounts INTEGER,");
						strcat(sCommand, "I125_ROICPM FLOAT,");
						strcat(sCommand, "I125_BackgroundROICounts INTEGER,");
						strcat(sCommand, "I125_BackgroundROICPM FLOAT,");
						strcat(sCommand, "I125_NetROICPM FLOAT,");
						strcat(sCommand, "I123Measured INTEGER,");
						strcat(sCommand, "I123_Activity FLOAT,");
						strcat(sCommand, "I123_Date FLOAT,");
						strcat(sCommand, "I123_PrimaryEnergy FLOAT,");
						strcat(sCommand, "I123_startEV FLOAT,");
						strcat(sCommand, "I123_endEV FLOAT,");
						strcat(sCommand, "I123_I125_startEV FLOAT,");
						strcat(sCommand, "I123_I125_endEV FLOAT,");
						strcat(sCommand, "I123BackgroundID INTEGER,");
						strcat(sCommand, "I123_SpectraID INTEGER,");
						strcat(sCommand, "I123_ROICounts INTEGER,");
						strcat(sCommand, "I123_ROICPM FLOAT,");
						strcat(sCommand, "I123_BackgroundROICounts INTEGER,");
						strcat(sCommand, "I123_BackgroundROICPM FLOAT,");
						strcat(sCommand, "I123_NetROICPM FLOAT,");
						strcat(sCommand, "I123_I125_ROICounts INTEGER,");
						strcat(sCommand, "I123_I125_ROICPM FLOAT,");
						strcat(sCommand, "I123_I125_BackgroundROICounts INTEGER,");
						strcat(sCommand, "I123_I125_BackgroundROICPM FLOAT,");
						strcat(sCommand, "I123_I125_NetROICPM FLOAT,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ProbeBioAssayEfficiency, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ProbeBioAssaySetting(");							// Added Rev 1.1
						strcat(sCommand, "ProbeBioAssaySettingID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "I131_Active INTEGER,");
						strcat(sCommand, "I125_Active INTEGER,");
						strcat(sCommand, "I123_Active INTEGER,");
						strcat(sCommand, "I131_Threshold FLOAT,");
						strcat(sCommand, "I125_Threshold FLOAT,");
						strcat(sCommand, "I123_Threshold FLOAT,");
						strcat(sCommand, "CountTime INTEGER,");
						strcat(sCommand, "ProbeDistance INTEGER,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ProbeBioAssaySetting, %d\n", *capStatus);
#endif
						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ProbeBioAssayTest(");							// Added Rev 1.1
						strcat(sCommand, "ProbeBioAssayTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "UserName TEXT,");
						strcat(sCommand, "FirstName TEXT,");
						strcat(sCommand, "LastName TEXT,");
						strcat(sCommand, "ProbeDistance INTEGER,");
						strcat(sCommand, "I131_Active INTEGER,");
						strcat(sCommand, "I131_PrimaryEnergy FLOAT,");
						strcat(sCommand, "I131_startEV FLOAT,");
						strcat(sCommand, "I131_endEV FLOAT,");
						strcat(sCommand, "I131_ROICounts INTEGER,");
						strcat(sCommand, "I131_ROICPM FLOAT,");
						strcat(sCommand, "I131_BackgroundROICounts INTEGER,");
						strcat(sCommand, "I131_BackgroundROICPM FLOAT,");
						strcat(sCommand, "I131_NetROICPM FLOAT,");
						strcat(sCommand, "I131_Activity FLOAT,");
						strcat(sCommand, "I131_High INTEGER,");
						strcat(sCommand, "I131_I125_NetROICPM FLOAT,");
						strcat(sCommand, "I125_Active INTEGER,");
						strcat(sCommand, "I125_PrimaryEnergy FLOAT,");
						strcat(sCommand, "I125_startEV FLOAT,");
						strcat(sCommand, "I125_endEV FLOAT,");
						strcat(sCommand, "I125_ROICounts INTEGER,");
						strcat(sCommand, "I125_ROICPM FLOAT,");
						strcat(sCommand, "I125_BackgroundROICounts INTEGER,");
						strcat(sCommand, "I125_BackgroundROICPM FLOAT,");
						strcat(sCommand, "I125_NetROICPM FLOAT,");
						strcat(sCommand, "I125_NetROICPMCorrected FLOAT,");
						strcat(sCommand, "I125_Activity FLOAT,");
						strcat(sCommand, "I125_High INTEGER,");
						strcat(sCommand, "I123_Active INTEGER,");
						strcat(sCommand, "I123_PrimaryEnergy FLOAT,");
						strcat(sCommand, "I123_startEV FLOAT,");
						strcat(sCommand, "I123_endEV FLOAT,");
						strcat(sCommand, "I123_ROICounts INTEGER,");
						strcat(sCommand, "I123_ROICPM FLOAT,");
						strcat(sCommand, "I123_BackgroundROICounts INTEGER,");
						strcat(sCommand, "I123_BackgroundROICPM FLOAT,");
						strcat(sCommand, "I123_NetROICPM FLOAT,");
						strcat(sCommand, "I123_Activity FLOAT,");
						strcat(sCommand, "I123_High INTEGER,");
						strcat(sCommand, "I123_I125_NetROICPM FLOAT,");
						strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
						strcat(sCommand, "SpectraID INTEGER REFERENCES Spectra(SpectraID),");
						strcat(sCommand, "ProbeBioAssayEfficiencyID INTEGER REFERENCES ProbeBioAssayEfficiency(ProbeBioAssayEfficiencyID),");
						strcat(sCommand, "ProbeBioAssaySettingID INTEGER REFERENCES ProbeBioAssaySetting(ProbeBioAssaySettingID),");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "Comment TEXT,");
						strcat(sCommand, "InactiveReason TEXT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ProbeBioAssayTest, %d\n", *capStatus);
#endif

						d_prepare_v2("INSERT INTO ProbeBioAssayEfficiency(I131_Efficiency,I125_Efficiency,I123_Efficiency,I131_I125_Contamination,I123_I125_Contamination,I131Measured,I131_Activity,I131_Date,I131_PrimaryEnergy,I131_startEV,I131_endEV,I131_I125_startEV,I131_I125_endEV,I131BackgroundID,I131_SpectraID,I131_ROICounts,I131_ROICPM,I131_BackgroundROICounts,I131_BackgroundROICPM,I131_NetROICPM,I131_I125_ROICounts,I131_I125_ROICPM,I131_I125_BackgroundROICounts,I131_I125_BackgroundROICPM,I131_I125_NetROICPM,I125Measured,I125_Activity,I125_Date,I125_PrimaryEnergy,I125_startEV,I125_endEV,I125BackgroundID,I125_SpectraID,I125_ROICounts,I125_ROICPM,I125_BackgroundROICounts,I125_BackgroundROICPM,I125_NetROICPM,I123Measured,I123_Activity,I123_Date,I123_PrimaryEnergy,I123_startEV,I123_endEV,I123_I125_startEV,I123_I125_endEV,I123BackgroundID,I123_SpectraID,I123_ROICounts,I123_ROICPM,I123_BackgroundROICounts,I123_BackgroundROICPM,I123_NetROICPM,I123_I125_ROICounts,I123_I125_ROICPM,I123_I125_BackgroundROICounts,I123_I125_BackgroundROICPM,I123_I125_NetROICPM,Inactive) VALUES(:I131_Efficiency,:I125_Efficiency,:I123_Efficiency,:I131_I125_Contamination,:I123_I125_Contamination,:I131Measured,:I131_Activity,:I131_Date,:I131_PrimaryEnergy,:I131_startEV,:I131_endEV,:I131_I125_startEV,:I131_I125_endEV,:I131BackgroundID,:I131_SpectraID,:I131_ROICounts,:I131_ROICPM,:I131_BackgroundROICounts,:I131_BackgroundROICPM,:I131_NetROICPM,:I131_I125_ROICounts,:I131_I125_ROICPM,:I131_I125_BackgroundROICounts,:I131_I125_BackgroundROICPM,:I131_I125_NetROICPM,:I125Measured,:I125_Activity,:I125_Date,:I125_PrimaryEnergy,:I125_startEV,:I125_endEV,:I125BackgroundID,:I125_SpectraID,:I125_ROICounts,:I125_ROICPM,:I125_BackgroundROICounts,:I125_BackgroundROICPM,:I125_NetROICPM,:I123Measured,:I123_Activity,:I123_Date,:I123_PrimaryEnergy,:I123_startEV,:I123_endEV,:I123_I125_startEV,:I123_I125_endEV,:I123BackgroundID,:I123_SpectraID,:I123_ROICounts,:I123_ROICPM,:I123_BackgroundROICounts,:I123_BackgroundROICPM,:I123_NetROICPM,:I123_I125_ROICounts,:I123_I125_ROICPM,:I123_I125_BackgroundROICounts,:I123_I125_BackgroundROICPM,:I123_I125_NetROICPM,:Inactive);");
						d_bind_double(":I131_Efficiency",-1.0);
						d_bind_double(":I125_Efficiency",-1.0);
						d_bind_double(":I123_Efficiency",-1.0);
						d_bind_double(":I131_I125_Contamination",0.085);
						d_bind_double(":I123_I125_Contamination",0.92);
						d_bind_int(":I131Measured",0);
						d_bind_double(":I131_Activity",-1);
						d_bind_double(":I131_Date",0);
						d_bind_double(":I131_PrimaryEnergy",-1);
						d_bind_double(":I131_startEV", 0);
						d_bind_double(":I131_endEV", 0);
						d_bind_double(":I131_I125_startEV", 0);
						d_bind_double(":I131_I125_endEV", 0);
						d_bind_int64(":I131BackgroundID",-1);
						d_bind_int64(":I131_SpectraID",-1);
						d_bind_int(":I131_ROICounts",0);
						d_bind_double(":I131_ROICPM",0);
						d_bind_int(":I131_BackgroundROICounts",0);
						d_bind_double(":I131_BackgroundROICPM",0);
						d_bind_double(":I131_NetROICPM",0);
						d_bind_int(":I131_I125_ROICounts",0);
						d_bind_double(":I131_I125_ROICPM",0);
						d_bind_int(":I131_I125_BackgroundROICounts",0);
						d_bind_double(":I131_I125_BackgroundROICPM",0);
						d_bind_double(":I131_I125_NetROICPM",0);
						d_bind_int(":I125Measured",0);
						d_bind_double(":I125_Activity",-1);
						d_bind_double(":I125_Date",0);
						d_bind_double(":I125_PrimaryEnergy",-1);
						d_bind_double(":I125_startEV", 0);
						d_bind_double(":I125_endEV", 0);
						d_bind_int64(":I125BackgroundID",-1);
						d_bind_int64(":I125_SpectraID",-1);
						d_bind_int(":I125_ROICounts",0);
						d_bind_double(":I125_ROICPM",0);
						d_bind_int(":I125_BackgroundROICounts",0);
						d_bind_double(":I125_BackgroundROICPM",0);
						d_bind_double(":I125_NetROICPM",0);
						d_bind_int(":I123Measured",0);
						d_bind_double(":I123_Activity",-1);
						d_bind_double(":I123_Date",0);
						d_bind_double(":I123_PrimaryEnergy",-1);
						d_bind_double(":I123_startEV", 0);
						d_bind_double(":I123_endEV", 0);
						d_bind_double(":I123_I125_startEV", 0);
						d_bind_double(":I123_I125_endEV", 0);
						d_bind_int64(":I123BackgroundID",-1);
						d_bind_int64(":I123_SpectraID",-1);
						d_bind_int(":I123_ROICounts",0);
						d_bind_double(":I123_ROICPM",0);
						d_bind_int(":I123_BackgroundROICounts",0);
						d_bind_double(":I123_BackgroundROICPM",0);
						d_bind_double(":I123_NetROICPM",0);
						d_bind_int(":I123_I125_ROICounts",0);
						d_bind_double(":I123_I125_ROICPM",0);
						d_bind_int(":I123_I125_BackgroundROICounts",0);
						d_bind_double(":I123_I125_BackgroundROICPM",0);
						d_bind_double(":I123_I125_NetROICPM",0);
						d_bind_int(":Inactive",0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO ProbeBioAssaySetting(I131_Active,I125_Active,I123_Active,I131_Threshold,I125_Threshold,I123_Threshold,CountTime,ProbeDistance,Inactive) VALUES(:I131_Active,:I125_Active,:I123_Active,:I131_Threshold,:I125_Threshold,:I123_Threshold,:CountTime,:ProbeDistance,:Inactive);");
						d_bind_int(":I131_Active",1);
						d_bind_int(":I125_Active",1);
						d_bind_int(":I123_Active",1);
						d_bind_double(":I131_Threshold",0.00000004);
						d_bind_double(":I125_Threshold",0.00000012);
						d_bind_double(":I123_Threshold",0.0000005);
						d_bind_int(":CountTime",60);
						d_bind_int(":ProbeDistance",25);
						d_bind_int(":Inactive",0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI131LowerEV',:BioAssayI131LowerEV);");
						d_bind_double(":BioAssayI131LowerEV", 292.0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI131UpperEV',:BioAssayI131UpperEV);");
						d_bind_double(":BioAssayI131UpperEV", 437.0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI125LowerEV',:BioAssayI125LowerEV);");
						d_bind_double(":BioAssayI125LowerEV", 20.0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI125UpperEV',:BioAssayI125UpperEV);");
						d_bind_double(":BioAssayI125UpperEV", 50.0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI123LowerEV',:BioAssayI123LowerEV);");
						d_bind_double(":BioAssayI123LowerEV", 143.0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI123UpperEV',:BioAssayI123UpperEV);");
						d_bind_double(":BioAssayI123UpperEV", 191.0);
						d_step();
						d_finalize();
						service_watchdog();

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ProbeTUProtocol(");							// Added Rev 1.1
						strcat(sCommand, "ProbeTUProtocolID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "ProbeTUProtocolGroupID INTEGER,");
						strcat(sCommand, "ProtocolName TEXT,");
						strcat(sCommand, "NuclideID INTEGER,");
						strcat(sCommand, "DoseForm INTEGER,");					// 1=Capsule, 2=Liquid
						strcat(sCommand, "DoseMeasurementMethod INTEGER,");		// 1=Dose Measured, 2=Sum All Doses, 3=Measure one and Multiply by Number, 4=Measure Liquid and Multiply by Factor
						strcat(sCommand, "DoseDecayCorrect INTEGER,");			// 0=Measure reference with Patient, 1=Measure Dose and Calculate Decay Value with Patient;
						strcat(sCommand, "PreDoseMeasurement INTEGER,");		// 0=No PreDose Measurement, 1=PreDose Measurement
						strcat(sCommand, "ResidualMeasurement INTEGER,");		// 0=Residual Dose Measurement, 1=Residual Dose Measurement
						strcat(sCommand, "I123LowerEV FLOAT,");
						strcat(sCommand, "I123UpperEV FLOAT,");
						strcat(sCommand, "I131LowerEV FLOAT,");
						strcat(sCommand, "I131UpperEV FLOAT,");
						strcat(sCommand, "Tc99mLowerEV FLOAT,");
						strcat(sCommand, "Tc99mUpperEV FLOAT,");
						strcat(sCommand, "DefaultCountTime INTEGER,");
						strcat(sCommand, "DefaultDistance INTEGER,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ProbeTUProtocol, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ProbeTUTest(");							// Added Rev 1.1
						strcat(sCommand, "ProbeTUTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "ProbeTUTestPhase INTEGER,");
						strcat(sCommand, "PatientID TEXT,");
						strcat(sCommand, "FirstName TEXT,");
						strcat(sCommand, "LastName TEXT,");
						strcat(sCommand, "DateOfBirth FLOAT,");
						strcat(sCommand, "Sex TEXT,");
						strcat(sCommand, "Physician TEXT,");
						strcat(sCommand, "TechID TEXT,");
						strcat(sCommand, "NuclideName TEXT,");
						strcat(sCommand, "HalfLife FLOAT,");
						strcat(sCommand, "HalfLifeUnit INTEGER,");
						strcat(sCommand, "StartROI FLOAT,");
						strcat(sCommand, "EndROI FLOAT,");
						strcat(sCommand, "PeakEV FLOAT,");
						strcat(sCommand, "CountingTime INTEGER,");
						strcat(sCommand, "DoseMultiplier FLOAT,");
						strcat(sCommand, "DoseAdministeredOn FLOAT,");
						strcat(sCommand, "CalibrationActivity FLOAT,");
						strcat(sCommand, "CalibrationDate FLOAT,");
						strcat(sCommand, "LotNum TEXT,");
						strcat(sCommand, "ProbeDistance INTEGER,");
						strcat(sCommand, "AccessionNumber TEXT,");
						strcat(sCommand, "ProbeTUProtocolID INTEGER,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "InactiveReason TEXT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ProbeTUTest, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ProbeTUPatientMeasurement(");							// Added Rev 1.1
						strcat(sCommand, "ProbeTUPatientMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "MeasurementNumber INTEGER,"); // 0 = Pre-Dose, 1 = First Measurement after Administration ...
						strcat(sCommand, "Neck1ROICounts INTEGER,");
						strcat(sCommand, "Neck1ROICPM FLOAT,");
						strcat(sCommand, "Neck2ROICounts INTEGER,");
						strcat(sCommand, "Neck2ROICPM FLOAT,");
						strcat(sCommand, "NeckROIAverageCPM FLOAT,");
						strcat(sCommand, "Leg1ROICounts INTEGER,");
						strcat(sCommand, "Leg1ROICPM FLOAT,");
						strcat(sCommand, "Leg2ROICounts INTEGER,");
						strcat(sCommand, "Leg2ROICPM FLOAT,");
						strcat(sCommand, "LegROIAverageCPM FLOAT,");
						strcat(sCommand, "PatientROINetCPM FLOAT,");
						strcat(sCommand, "AdjustedPatientROINetCPM FLOAT,");
						strcat(sCommand, "DoseROINetCPM FLOAT,");
						strcat(sCommand, "AdjustedDoseROINetCPM FLOAT,");
						strcat(sCommand, "Uptake FLOAT,");				// For Pre-Dose, -1 = Skipped, 0 = Measured
						strcat(sCommand, "ElapsedTime FLOAT,");
						strcat(sCommand, "Comment TEXT,");
						strcat(sCommand, "Neck1_SpectraID INTEGER,");
						strcat(sCommand, "Neck2_SpectraID INTEGER,");
						strcat(sCommand, "Leg1_SpectraID INTEGER,");
						strcat(sCommand, "Leg2_SpectraID INTEGER,");
						strcat(sCommand, "ProbeTUTestID INTEGER REFERENCES ProbeTUTest(ProbeTUTestID),");
						strcat(sCommand, "CreatedOn FLOAT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ProbeTUPatientMeasurement, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ProbeTUTotalDoseMeasurement(");							// Added Rev 1.1
						strcat(sCommand, "ProbeTUTotalDoseMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "MeasurementNumber INTEGER,"); // -1 = Residual, 0 = Administered Dose, 1 = First Measurement after Administration ...
						strcat(sCommand, "BackgroundROICounts INTEGER,");
						strcat(sCommand, "BackgroundROICPM FLOAT,");
						strcat(sCommand, "DoseROICPM FLOAT,");
						strcat(sCommand, "DoseROINetCPM FLOAT,");
						strcat(sCommand, "StartDateTime FLOAT,");
						strcat(sCommand, "EndDateTime FLOAT,");
						strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
						strcat(sCommand, "ProbeTUTestID INTEGER REFERENCES ProbeTUTest(ProbeTUTestID),");
						strcat(sCommand, "CreatedOn FLOAT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ProbeTUTotalDoseMeasurement, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ProbeTUSingleDoseMeasurement(");							// Added Rev 1.1
						strcat(sCommand, "ProbeTUSingleDoseMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "DoseSubUnit INTEGER,"); // 1=First Capsule, 2=Second Capsule ...
						strcat(sCommand, "Dose1ROICounts INTEGER,");
						strcat(sCommand, "Dose1ROICPM FLOAT,");
						strcat(sCommand, "Dose2ROICounts INTEGER,");
						strcat(sCommand, "Dose2ROICPM FLOAT,");
						strcat(sCommand, "DoseROIAverageCPM FLOAT,");
						strcat(sCommand, "Dose1_SpectraID INTEGER,");
						strcat(sCommand, "Dose2_SpectraID INTEGER,");
						strcat(sCommand, "ProbeTUTotalDoseMeasurementID INTEGER REFERENCES ProbeTUTotalDoseMeasurement(ProbeTUTotalDoseMeasurementID),");
						strcat(sCommand, "CreatedOn FLOAT");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ProbeTUSingleDoseMeasurement, %d\n", *capStatus);
#endif

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour1',:TUHour1);");
						d_bind_int(":TUHour1", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour2',:TUHour2);");
						d_bind_int(":TUHour2", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour3',:TUHour3);");
						d_bind_int(":TUHour3", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour4',:TUHour4);");
						d_bind_int(":TUHour4", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour1',:TUMinHour1);");
						d_bind_int(":TUMinHour1", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour2',:TUMinHour2);");
						d_bind_int(":TUMinHour2", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour3',:TUMinHour3);");
						d_bind_int(":TUMinHour3", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour4',:TUMinHour4);");
						d_bind_int(":TUMinHour4", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour1',:TUMaxHour1);");
						d_bind_int(":TUMaxHour1", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour2',:TUMaxHour2);");
						d_bind_int(":TUMaxHour2", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour3',:TUMaxHour3);");
						d_bind_int(":TUMaxHour3", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour4',:TUMaxHour4);");
						d_bind_int(":TUMaxHour4", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("UPDATE Config SET SettingValue = :DBRev WHERE SettingName = 'DBRev';");
						d_bind_double(":DBRev", 1.1);
						d_step();
						d_finalize();

						fDBRev = 1.1;
					}

					if((fDBRev > 1.05) && (fDBRev < 1.15)){
						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TimeFormat',:TimeFormat);");
						d_bind_int(":TimeFormat", 0);
						d_step();
						d_finalize();

						d_prepare_v2("UPDATE Config SET SettingValue = :DBRev WHERE SettingName = 'DBRev';");
						d_bind_double(":DBRev", 1.2);
						d_step();
						d_finalize();

						fDBRev = 1.2;
					}

					if((fDBRev > 1.15) && (fDBRev < 1.25)){
						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE AutoLinearityTest(");									// Added Rev 1.3
						strcat(sCommand, "AutoLinearityTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "SerialNumber TEXT,");
						strcat(sCommand, "Type INTEGER,");
						strcat(sCommand, "NuclideName TEXT,");
						strcat(sCommand, "HalfLife FLOAT,");
						strcat(sCommand, "HalfLifeUnit INTEGER,");
						strcat(sCommand, "IntervalMinutes INTEGER,");
						strcat(sCommand, "TotalMinutes INTEGER,");
						strcat(sCommand, "StartedOn FLOAT,");
						strcat(sCommand, "Comment TEXT,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "InactiveReason TEXT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE AutoLinearityTest, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE INDEX idx_tbl_AutoLinearityTest_StartedOn ON AutoLinearityTest(StartedOn);");
						d_command(sCommand);
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE INDEX idx_tbl_AutoLinearityTest_StartedOn, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE AutoLinearityMeasurement(");								// Added Rev 1.3
						strcat(sCommand, "AutoLinearityMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "AutoLinearityTestID INTEGER REFERENCES AutoLinearityTest(AutoLinearityTestID),");
						strcat(sCommand, "MeasuredActivity FLOAT,");
						strcat(sCommand, "MeasuredOn FLOAT,");
						strcat(sCommand, "Status INTEGER,");
						strcat(sCommand, "ElapsedMinutes INTEGER,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE AutoLinearityMeasurement, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE WellRBCSurvivalTest(");							// Added Rev 1.3
						strcat(sCommand, "WellRBCSurvivalTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "WellRBCSurvivalTestPhase INTEGER,");
						strcat(sCommand, "PatientID TEXT,");
						strcat(sCommand, "FirstName TEXT,");
						strcat(sCommand, "LastName TEXT,");
						strcat(sCommand, "DateOfBirth FLOAT,");
						strcat(sCommand, "Sex TEXT,");
						strcat(sCommand, "Physician TEXT,");
						strcat(sCommand, "TechID TEXT,");
						strcat(sCommand, "NuclideName TEXT,");
						strcat(sCommand, "HalfLife FLOAT,");
						strcat(sCommand, "HalfLifeUnit INTEGER,");
						strcat(sCommand, "StartROI FLOAT,");
						strcat(sCommand, "EndROI FLOAT,");
						strcat(sCommand, "PeakEV FLOAT,");
						strcat(sCommand, "CountingTime INTEGER,");
						strcat(sCommand, "InjectionDate FLOAT,");
						strcat(sCommand, "CalibrationActivity FLOAT,");
						strcat(sCommand, "CalibrationDate FLOAT,");
						strcat(sCommand, "LotNum TEXT,");
						strcat(sCommand, "AccessionNumber TEXT,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "LastUpdated FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "InactiveReason TEXT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE WellRBCSurvivalTest, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE WellRBCSurvivalMeasurement(");							// Added Rev 1.3
						strcat(sCommand, "WellRBCSurvivalMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "MeasurementNumber INTEGER,");
						strcat(sCommand, "BackgroundROICounts INTEGER,");
						strcat(sCommand, "BackgroundROICPM FLOAT,");
						strcat(sCommand, "Sample1ROICounts INTEGER,");
						strcat(sCommand, "Sample1ROICPM FLOAT,");
						strcat(sCommand, "Sample2ROICounts INTEGER,");
						strcat(sCommand, "Sample2ROICPM FLOAT,");
						strcat(sCommand, "SampleROIAverageCPM FLOAT,");
						strcat(sCommand, "SampleROINetCPM FLOAT,");
						strcat(sCommand, "DecayCorrectedSampleROINetCPM FLOAT,");
						strcat(sCommand, "SampleHematocrit FLOAT,");
						strcat(sCommand, "Remaining FLOAT,");
						strcat(sCommand, "ElapsedDays FLOAT,");
						strcat(sCommand, "Comment TEXT,");
						strcat(sCommand, "WellBackgroundID INTEGER REFERENCES WellBackground(WellBackgroundID),");
						strcat(sCommand, "Sample1_SpectraID INTEGER,");
						strcat(sCommand, "Sample2_SpectraID INTEGER,");
						strcat(sCommand, "WellRBCSurvivalTestID INTEGER REFERENCES WellRBCSurvivalTest(WellRBCSurvivalTestID),");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP))");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE WellRBCSurvivalMeasurement, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE WellRBCSurvivalResult(");							// Added Rev 1.3
						strcat(sCommand, "WellRBCSurvivalResultID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "HalfDays FLOAT,");
						strcat(sCommand, "Y0 FLOAT,");
						strcat(sCommand, "X10 FLOAT,");
						strcat(sCommand, "Y60 FLOAT,");
						strcat(sCommand, "WellRBCSurvivalTestID INTEGER REFERENCES WellRBCSurvivalTest(WellRBCSurvivalTestID),");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP))");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE WellRBCSurvivalResult, %d\n", *capStatus);
#endif

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('USBDeviceProtocol',:Protocol);");
						d_bind_int(":Protocol", 0);
						d_step();
						d_finalize();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('RBCSurvivalMinNormal',:RBCSurvivalMinNormal);");
						d_bind_double(":RBCSurvivalMinNormal", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('RBCSurvivalMaxNormal',:RBCSurvivalMaxNormal);");
						d_bind_double(":RBCSurvivalMaxNormal", -1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FutureDateInput',:FutureDateInput);");
						d_bind_int(":FutureDateInput", 0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("UPDATE Config SET SettingValue = :DBRev WHERE SettingName = 'DBRev';");
						d_bind_double(":DBRev", 1.3);
						d_step();
						d_finalize();

						fDBRev = 1.3;
					}

					if((fDBRev > 1.25) && (fDBRev < 1.35)){
						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('Language',:Language);");
						d_bind_int(":Language", 0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("UPDATE Config SET SettingValue = :DBRev WHERE SettingName = 'DBRev';");
						d_bind_double(":DBRev", 1.4);
						d_step();
						d_finalize();

						fDBRev = 1.4;
					}

					if((fDBRev > 1.35) && (fDBRev < 1.45)){
						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ChamberDailyTest(");							// Added Rev 1.5
						strcat(sCommand, "ChamberDailyTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "ChamberSerialNumber TEXT,");
						strcat(sCommand, "ChamberType INTEGER,");
						strcat(sCommand, "TwoStageChamber INTEGER,");
						strcat(sCommand, "DataCheckTextEnglish TEXT,");
						strcat(sCommand, "DataCheckTextFrench TEXT,");
						strcat(sCommand, "DataCheckFailed INTEGER,");
						strcat(sCommand, "DataCheckCRC TEXT,");
						strcat(sCommand, "ChamberZeroID INTEGER,");
						strcat(sCommand, "ChamberBackgroundID INTEGER,");
						strcat(sCommand, "ChamberVoltageID INTEGER,");
						strcat(sCommand, "ChamberAccuracyTestID INTEGER,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "InactiveReason TEXT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ChamberDailyTest, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ChamberZero(");								// Added Rev 1.5
						strcat(sCommand, "ChamberZeroID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "ChamberSerialNumber TEXT,");
						strcat(sCommand, "ChamberType INTEGER,");
						strcat(sCommand, "TwoStageChamber INTEGER,");
						strcat(sCommand, "ZeroStatus INTEGER,");
						strcat(sCommand, "ZeroTextEnglish TEXT,");
						strcat(sCommand, "ZeroTextFrench TEXT,");
						strcat(sCommand, "ZeroValue FLOAT,");
						strcat(sCommand, "MeasuredOn FLOAT,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "InactiveReason TEXT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ChamberZero, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ChamberBackground(");								// Added Rev 1.5
						strcat(sCommand, "ChamberBackgroundID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "ChamberSerialNumber TEXT,");
						strcat(sCommand, "ChamberType INTEGER,");
						strcat(sCommand, "TwoStageChamber INTEGER,");
						strcat(sCommand, "BackgroundStatus INTEGER,");
						strcat(sCommand, "BackgroundTextEnglish TEXT,");
						strcat(sCommand, "BackgroundTextFrench TEXT,");
						strcat(sCommand, "BackgroundValue FLOAT,");
						strcat(sCommand, "MeasuredOn FLOAT,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "InactiveReason TEXT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ChamberBackground, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ChamberVoltage(");									// Added Rev 1.5
						strcat(sCommand, "ChamberVoltageID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "ChamberSerialNumber TEXT,");
						strcat(sCommand, "ChamberType INTEGER,");
						strcat(sCommand, "TwoStageChamber INTEGER,");
						strcat(sCommand, "NominalVoltage FLOAT,");
						strcat(sCommand, "MinVoltage FLOAT,");
						strcat(sCommand, "MaxVoltage FLOAT,");
						strcat(sCommand, "VoltageStatus INTEGER,");
						strcat(sCommand, "VoltageTextEnglish TEXT,");
						strcat(sCommand, "VoltageTextFrench TEXT,");
						strcat(sCommand, "VoltageValue FLOAT,");
						strcat(sCommand, "MeasuredOn FLOAT,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "InactiveReason TEXT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ChamberVoltage, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ChamberAccuracyTest(");								// Added Rev 1.5
						strcat(sCommand, "ChamberAccuracyTestID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "ChamberSerialNumber TEXT,");
						strcat(sCommand, "ChamberType INTEGER,");
						strcat(sCommand, "TwoStageChamber INTEGER,");
						strcat(sCommand, "ConstancyNuclide TEXT,");
						strcat(sCommand, "ConstancyNuclideSerialNumber TEXT,");
						strcat(sCommand, "ConstancyVoltage FLOAT,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP)),");
						strcat(sCommand, "InactiveReason TEXT,");
						strcat(sCommand, "Inactive INTEGER");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ChamberAccuracyTest, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ChamberAccuracyMeasurement(");						// Added Rev 1.5
						strcat(sCommand, "ChamberAccuracyMeasurementID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "ChamberAccuracyTestID INTEGER REFERENCES ChamberAccuracyTest(ChamberAccuracyTestID),"),
						strcat(sCommand, "SourceNuclide TEXT,");
						strcat(sCommand, "SourceNuclideIndex INTEGER,");
						strcat(sCommand, "SourceSerialNumber TEXT,");
						strcat(sCommand, "SourceCalActivity FLOAT,");
						strcat(sCommand, "SourceCalDate FLOAT,");
						strcat(sCommand, "MeasuredOn FLOAT,");
						strcat(sCommand, "CalculatedActivityTextEnglish TEXT,");
						strcat(sCommand, "CalculatedActivityTextFrench TEXT,");
						strcat(sCommand, "CalculatedActivity FLOAT,");
						strcat(sCommand, "Response FLOAT,");
						strcat(sCommand, "HalfLife FLOAT,");
						strcat(sCommand, "HalfLifeUnit INTEGER,");
						strcat(sCommand, "MeasuredActivityTextEnglish TEXT,");
						strcat(sCommand, "MeasuredActivityTextFrench TEXT,");
						strcat(sCommand, "MeasuredActivity FLOAT,");
						strcat(sCommand, "DeviationStatus INTEGER,");
						strcat(sCommand, "DeviationTextEnglish TEXT,");
						strcat(sCommand, "DeviationTextFrench TEXT,");
						strcat(sCommand, "Deviation FLOAT");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ChamberAccuracyMeasurement, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE ChamberAutoConstancy(");							// Added Rev 1.5
						strcat(sCommand, "ChamberAutoConstancyID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "ChamberAccuracyTestID INTEGER REFERENCES ChamberAccuracyTest(ChamberAccuracyTestID),"),
						strcat(sCommand, "ConstancyNuclide TEXT,");
						strcat(sCommand, "ConstancyNuclideIndex INTEGER,");
						strcat(sCommand, "Response FLOAT,");
						strcat(sCommand, "HalfLife FLOAT,");
						strcat(sCommand, "HalfLifeUnit INTEGER,");
						strcat(sCommand, "ConstancyActivityTextEnglish TEXT,");
						strcat(sCommand, "ConstancyActivityTextFrench TEXT,");
						strcat(sCommand, "ConstancyActivity FLOAT");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE ChamberAutoConstancy, %d\n", *capStatus);
#endif

						d_prepare_v2("UPDATE Config SET SettingValue = :DBRev WHERE SettingName = 'DBRev';");
						d_bind_double(":DBRev", 1.5);
						d_step();
						d_finalize();

						fDBRev = 1.5;
					}

					if((fDBRev > 1.45) && (fDBRev < 1.55)){
						strcpy(sCommand, "ALTER TABLE UserNuclide ADD COLUMN Response_B FLOAT;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE UserNuclide SET Response_B = 0 WHERE Response_B IS NULL;");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: ALTER TABLE UserNuclide ADD COLUMN Response_B, %d\n", *capStatus);
#endif

						strcpy(sCommand, "ALTER TABLE UserNuclide ADD COLUMN Response_18 FLOAT;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE UserNuclide SET Response_18 = 0 WHERE Response_18 IS NULL;");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: ALTER TABLE UserNuclide ADD COLUMN Response_18, %d\n", *capStatus);
#endif

						strcpy(sCommand, "ALTER TABLE UserNuclide ADD COLUMN Response_C FLOAT;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE UserNuclide SET Response_C = 0 WHERE Response_C IS NULL;");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: ALTER TABLE UserNuclide ADD COLUMN Response_C, %d\n", *capStatus);
#endif

						strcpy(sCommand, "ALTER TABLE UserNuclide ADD COLUMN Response_K FLOAT;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE UserNuclide SET Response_K = 0 WHERE Response_K IS NULL;");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: ALTER TABLE UserNuclide ADD COLUMN Response_K, %d\n", *capStatus);
#endif

						strcpy(sCommand, "ALTER TABLE UserNuclide ADD COLUMN Response_UserB FLOAT;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE UserNuclide SET Response_UserB = 0 WHERE Response_UserB IS NULL;");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: ALTER TABLE UserNuclide ADD COLUMN Response_UserB, %d\n", *capStatus);
#endif

						strcpy(sCommand, "ALTER TABLE UserNuclide ADD COLUMN Response_User18 FLOAT;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE UserNuclide SET Response_User18 = 0 WHERE Response_User18 IS NULL;");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: ALTER TABLE UserNuclide ADD COLUMN Response_User18, %d\n", *capStatus);
#endif

						strcpy(sCommand, "ALTER TABLE UserNuclide ADD COLUMN Response_UserC FLOAT;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE UserNuclide SET Response_UserC = 0 WHERE Response_UserC IS NULL;");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: ALTER TABLE UserNuclide ADD COLUMN Response_UserC, %d\n", *capStatus);
#endif

						strcpy(sCommand, "ALTER TABLE UserNuclide ADD COLUMN Response_UserK FLOAT;");
						d_command(sCommand);
						strcpy(sCommand, "UPDATE UserNuclide SET Response_UserK = 0 WHERE Response_UserK IS NULL;");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: ALTER TABLE UserNuclide ADD COLUMN Response_UserK, %d\n", *capStatus);
#endif

						*sCommand = 0;
						strcat(sCommand, "CREATE TABLE Hotkey(");
						strcat(sCommand, "HotkeyID INTEGER PRIMARY KEY AUTOINCREMENT,");
						strcat(sCommand, "ChamberType INTEGER,");
						strcat(sCommand, "HotkeyMenu INTEGER,");
						strcat(sCommand, "HotkeyIndex INTEGER,");
						strcat(sCommand, "HotkeyNuclideIndex INTEGER,");
						strcat(sCommand, "CreatedOn FLOAT DEFAULT(julianday(CURRENT_TIMESTAMP))");
						strcat(sCommand, ");");
						d_command(sCommand);
						service_watchdog();
#ifdef PRINTF_ERRORS
						if(*capStatus != SQLITE_OK)
							printf("Error: CREATE TABLE Hotkey, %d\n", *capStatus);
#endif

						DB_PopulateDefaultHotkeys();
						read_hotkey_txt();
						DB_WriteAllHotkeyWithInsert();

						d_prepare_v2("UPDATE Config SET SettingValue = :DBRev WHERE SettingName = 'DBRev';");
						d_bind_double(":DBRev", 1.6);
						d_step();
						d_finalize();

						fDBRev = 1.6;
					}

					if((fDBRev > 1.55) && (fDBRev < 1.65)){
						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FeedLabel',:FeedLabel);");
						d_bind_int(":FeedLabel", 1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('MolyStreamlined',:MolyStreamlined);");
						d_bind_int(":MolyStreamlined", 1);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('Key0',:Key0);");
						d_bind_int(":Key0", 0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('Key1',:Key1);");
						d_bind_int(":Key1", 0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('Password',:Password);");
						d_bind_int(":Password", 0);
						d_step();
						d_finalize();
						service_watchdog();

						d_prepare_v2("UPDATE Config SET SettingValue = :DBRev WHERE SettingName = 'DBRev';");
						d_bind_double(":DBRev", 1.7);
						d_step();
						d_finalize();

						fDBRev = 1.7;
					}
				}
			}else{
				d_finalize();
			}

			if(*capStatus != SQLITE_OK){
				db_close(capStatus, capDB);
				db_shutdown(capStatus);
			}
		}
	}else{
		printf("Unable go to /data");
	}

	free(sCommand);
}
/**
 * \details Initialize sql library with error messages
 * \param status Pointer to database status
 * \returns None
 */
static void db_initialize(int *status){
	if(*status == SQLITE_OK){
#ifdef EMULATOR
		printf("[db_init] calling sqlite3_initialize, status=%d\n", *status);
		fflush(stdout);
#endif
		*status = sqlite3_initialize();
#ifdef EMULATOR
		printf("[db_init] sqlite3_initialize returned %d\n", *status);
		fflush(stdout);
#endif

		if(*status != SQLITE_OK){
			sprintf(err_output, "Failed to initialize database\nCode: %d", *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_initialize, %d\n", *status);
#endif
		}
	}
}
/**
 * \details Open database with error messages
 * \param status Pointer to database status
 * \param filename Pointer to Null terminated database filename
 * \param ppDb Pointer to pointer to main database structure
 * \param flags Database configuration flag
 * \param zVfs Pointer to virtual configuration flags
 * \returns None
 */
static void db_open_v2(int *status, const char *filename, sqlite3 **ppDb, int flags, const char *zVfs){
	if(*status == SQLITE_OK){
#ifdef EMULATOR
		printf("[db_open] opening '%s' flags=0x%x\n", filename, flags);
		fflush(stdout);
#endif
		*status = sqlite3_open_v2(filename, ppDb, flags, zVfs);
#ifdef EMULATOR
		printf("[db_open] sqlite3_open_v2 returned %d\n", *status);
		fflush(stdout);
#endif

		if(*status != SQLITE_OK){
			sprintf(err_output, "Failed to open database - %s\nCode: %d", filename, *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_open_v2, %d\n", *status);
#endif
		}
	}
#ifdef EMULATOR
	else {
		printf("[db_open] SKIPPED opening '%s' because status=%d\n", filename, *status);
		fflush(stdout);
	}
#endif
}
/**
 * \details Prepare a SQL query with error messages
 * \param status Pointer to database status
 * \param db Pointer to main database structure
 * \param zSql Pointer to SQL query string
 * \param nBytes Length of SQL query string
 * \param ppStmt Pointer to Pointer to SQL statement structure
 * \param pzTail Pointer to the end of the parsed string
 * \returns None
 */
static void db_prepare_v2(int *status, sqlite3 *db, const char *zSql, int nBytes, sqlite3_stmt **ppStmt, const char **pzTail){
	if(*status == SQLITE_OK){
		*ppStmt = NULL;
		*status = sqlite3_prepare_v2(db, zSql, nBytes, ppStmt, pzTail);
		if(*status != SQLITE_OK){
			sprintf(err_output, "Improper Database command\nCode: %d", *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_prepare_v2, %d\n", *status);
#endif
			*status = *status + 60000;
		}
	}
}

//static void db_bind_blob(int *status, sqlite3_stmt *pStmt, int pidx, const void *data, int data_len, void(*mem_callback)(void *data)){
/**
 * \details Bind blob binary data to a SQL statement with errors
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param name Pointer to Null terminated blob name
 * \param data Pointer to blob binary data
 * \param data_len Length of binary data
 * \param mem_callback Pointer to memory callback function
 * \returns None
 */
static void db_bind_blob(int *status, sqlite3_stmt *pStmt, const char *name, const void *data, int data_len, void(*mem_callback)(void *data)){
	if(*status == SQLITE_OK){
		*status = sqlite3_bind_blob(pStmt, sqlite3_bind_parameter_index(pStmt, name), data, data_len, mem_callback);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Improper Blob Binding - %s\nCode: %d", name, *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_bind_blob, %d\n", *status);
#endif
		}
	}
}

//static void db_bind_double(int *status, sqlite3_stmt *pStmt, int pidx, double data){
/**
 * \details Bind double float to a SQL statement with errors
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param name Pointer to Null terminated double name
 * \param data Double float number
 * \returns None
 */
static void db_bind_double(int *status, sqlite3_stmt *pStmt, const char *name, double data){
	if(*status == SQLITE_OK){
		*status = sqlite3_bind_double(pStmt, sqlite3_bind_parameter_index(pStmt, name), data);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Improper Double Binding - %s\nCode: %d", name, *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_bind_double, %d\n", *status);
#endif
		}
	}
}

//static void db_bind_int(int *status, sqlite3_stmt *pStmt, int pidx, int data){
/**
 * \details Bind 32-bit integer data to a SQL statement with errors
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param name Pointer to Null terminated integer name
 * \param data 32-bit integer number
 * \returns None
 */
static void db_bind_int(int *status, sqlite3_stmt *pStmt, const char *name, int data){
	if(*status == SQLITE_OK){
		*status = sqlite3_bind_int(pStmt, sqlite3_bind_parameter_index(pStmt, name), data);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Improper Int Binding - %s\nCode: %d", name, *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_bind_int, %d\n", *status);
#endif
		}
	}
}

//static void db_bind_int64(int *status, sqlite3_stmt *pStmt, int pidx, sqlite3_int64 data){
/**
 * \details Bind 64-bit integer data to a SQL statement with errors
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param name Pointer to Null terminated integer name
 * \param data 64-bit integer number
 * \returns None
 */
static void db_bind_int64(int *status, sqlite3_stmt *pStmt, const char *name, sqlite3_int64 data){
	if(*status == SQLITE_OK){
		*status = sqlite3_bind_int64(pStmt, sqlite3_bind_parameter_index(pStmt, name), data);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Improper Int64 Binding - %s\nCode: %d", name, *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_bind_int64, %d\n", *status);
#endif
		}
	}
}

//static void db_bind_null(int *status, sqlite3_stmt *pStmt, int pidx){
/**
 * \details Bind null value to a SQL statement with errors
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param name Pointer to Null terminated field name
 * \returns None
 */
static void db_bind_null(int *status, sqlite3_stmt *pStmt, const char *name){
	if(*status == SQLITE_OK){
		*status = sqlite3_bind_null(pStmt, sqlite3_bind_parameter_index(pStmt, name));

		if(*status != SQLITE_OK){
			sprintf(err_output, "Improper Null Binding - %s\nCode: %d", name, *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_bind_null, %d\n", *status);
#endif
		}
	}
}

//static void db_bind_text(int *status, sqlite3_stmt *pStmt, int pidx, const char *data, int data_len, void(*mem_callback)(void *data)){
/**
 * \details Bind ASCII text to a SQL statement with errors
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param name Pointer to Null terminated text name
 * \param data Pointer to text data
 * \param data_len Text data length
 * \param mem_callback Pointer to memory callback function
 * \returns None
 */
static void db_bind_text(int *status, sqlite3_stmt *pStmt, const char *name, const char *data, int data_len, void(*mem_callback)(void *data)){
	if(*status == SQLITE_OK){
		*status = sqlite3_bind_text(pStmt, sqlite3_bind_parameter_index(pStmt, name), data, data_len, mem_callback);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Improper Text Binding - %s\nCode: %d", name, *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_bind_text, %d\n", *status);
#endif
		}
	}
}

//static void db_bind_text16(int *status, sqlite3_stmt *pStmt, int pidx, const void *data, int data_len, void(*mem_callback)(void *data)){
/**
 * \details Bind Unicode text to a SQL statement with errors
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param name Pointer to Null terminated text name
 * \param data Pointer to text data
 * \param data_len Text data length
 * \param mem_callback Pointer to memory callback function
 * \returns None
 */
static void db_bind_text16(int *status, sqlite3_stmt *pStmt, const char *name, const void *data, int data_len, void(*mem_callback)(void *data)){
	if(*status == SQLITE_OK){
		*status = sqlite3_bind_text16(pStmt, sqlite3_bind_parameter_index(pStmt, name), data, data_len, mem_callback);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Improper Text Binding - %s\nCode: %d", name, *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_bind_text16, %d\n", *status);
#endif
		}
	}
}

//static void db_bind_value(int *status, sqlite3_stmt *pStmt, int pidx, const sqlite3_value *data_value){
/**
 * \details Bind value to a SQL statement with errors
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param name Pointer to Null terminated field name
 * \param data_value Pointer to data
 * \returns None
 */
static void db_bind_value(int *status, sqlite3_stmt *pStmt, const char *name, const sqlite3_value *data_value){
	if(*status == SQLITE_OK){
		*status = sqlite3_bind_value(pStmt, sqlite3_bind_parameter_index(pStmt, name), data_value);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Improper Value Binding - %s\nCode: %d", name, *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_bind_value, %d\n", *status);
#endif
		}
	}
}

//static void db_bind_zeroblob(int *status, sqlite3_stmt *pStmt, int pidx, int len){
/**
 * \details Bind a zero blob to a SQL statement with errors
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param name Pointer to Null terminated field name
 * \param len Length of zero blob
 * \returns None
 */
static void db_bind_zeroblob(int *status, sqlite3_stmt *pStmt, const char *name, int len){
	if(*status == SQLITE_OK){
		*status = sqlite3_bind_zeroblob(pStmt, sqlite3_bind_parameter_index(pStmt, name), len);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Improper Zero Blob Binding - %s\nCode: %d", name, *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_bind_zeroblob, %d\n", *status);
#endif
		}
	}
}
/**
 * \details Step to next row in the SQL statement structure with errors
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \returns None
 */
static void db_step(int *status, sqlite3_stmt *pStmt){
	if((*status == SQLITE_OK) || (*status == SQLITE_ROW)){
		*status = sqlite3_step(pStmt);

		if((*status != SQLITE_OK) && (*status != SQLITE_ROW) && (*status != SQLITE_DONE)){
			sprintf(err_output, "Error Executing Statement\nCode: %d", *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_step, %d\n", *status);
#endif
		}
	}
}
/**
 * \details Get number of rows in the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \returns Number of rows
 */
static int db_column_count(int *status, sqlite3_stmt *pStmt){
	if(*status == SQLITE_ROW){
		return sqlite3_column_count(pStmt);
	}else{
		return 0;
	}
}
/**
 * \details Get blob data from the current row of the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns Pointer to blob data
 */
static const void *db_column_blob(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_blob(pStmt, iCol);
	}else{
		return NULL;
	}
}
/**
 * \details Get blob data length from the current row of the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns Blob data length
 */
static int db_column_bytes(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_bytes(pStmt, iCol);
	}else{
		return 0;
	}
}
/**
 * \details Get 16-bit blob data length from the current row of the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns Blob data length
 */
static int db_column_bytes16(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_bytes16(pStmt, iCol);
	}else{
		return 0;
	}
}
/**
 * \details Get double float value from the current row of the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns Double value
 */
static double db_column_double(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_double(pStmt, iCol);
	}else{
		return 0;
	}
}
/**
 * \details Get 32-bit integer value from the current row of the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns 32-bit integer value
 */
static int db_column_int(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_int(pStmt, iCol);
	}else{
		return 0;
	}
}
/**
 * \details Get 64-bit integer value from the current row of the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns 64-bit integer value
 */
static sqlite3_int64 db_column_int64(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_int64(pStmt, iCol);
	}else{
		return 0;
	}
}
/**
 * \details Get ASCII text data from the current row of the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns Pointer to Null terminated text data
 */
static const unsigned char *db_column_text(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_text(pStmt, iCol);
	}else{
		return NULL;
	}
}
/**
 * \details Get 16-bit text data from the current row of the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns Pointer to Null terminated text data
 */
static const void *db_column_text16(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_text16(pStmt, iCol);
	}else{
		return NULL;
	}
}
/**
 * \details Get column type from the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns Column Type
 */
static int db_column_type(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_type(pStmt, iCol);
	}else{
		return 0;
	}
}
/**
 * \details Get column value from the current row of the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns Column value
 */
static sqlite3_value *db_column_value(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_value(pStmt, iCol);
	}else{
		return NULL;
	}
}
/**
 * \details Get field name from the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns Pointer to Null terminated field name
 */
static const char *db_column_name(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_name(pStmt, iCol);
	}else{
		return NULL;
	}
}
/**
 * \details Get 16-bit field name from the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \param iCol SQL result column index
 * \returns Pointer to Null terminated 16-bit field name
 */
static const void *db_column_name16(int *status, sqlite3_stmt *pStmt, int iCol){
	if(*status == SQLITE_ROW){
		return sqlite3_column_name16(pStmt, iCol);
	}else{
		return NULL;
	}
}
/**
 * \details Reset the SQL statement structure for a new query
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \returns None
 */
static void db_reset(int *status, sqlite3_stmt *pStmt){
	if((*status == SQLITE_OK) || (*status == SQLITE_ROW) || (*status == SQLITE_DONE)){
		*status = sqlite3_reset(pStmt);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Unable to Reset Statement\nCode: %d", *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_reset, %d\n", *status);
#endif
		}
	}
}
/**
 * \details Close the SQL statement structure
 * \param status Pointer to database status
 * \param pStmt Pointer to SQL statement structure
 * \returns None
 */
static void db_finalize(int *status, sqlite3_stmt *pStmt){
	if((*status == SQLITE_OK) || (*status == SQLITE_ROW) || (*status == SQLITE_DONE)){
		*status = sqlite3_finalize(pStmt);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Unable to Finalize Statement\nCode: %d", *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_finalize, %d\n", *status);
#endif
		}
		pStmt = NULL;
	}else{
		if(*status < 60000){
			sqlite3_finalize(pStmt);
			pStmt = NULL;
		}
	}
}
/**
 * \details Close SQL database
 * \param status Pointer to database status
 * \param db Pointer to SQL database structure
 * \returns None
 */
static void db_close(int *status, sqlite3 *db){
	if(*status == SQLITE_OK){
		*status = sqlite3_close(db);

		if(*status != SQLITE_OK){
			sprintf(err_output, "Unable to Close Database\nCode: %d", *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_close, %d\n", *status);
#endif
		}
	}else{
		sqlite3_close(db);
	}
	db = NULL;
}
/**
 * \details Shutdown SQL database engine
 * \param status Pointer to database status
 * \returns None
 */
static void db_shutdown(int *status){
	if(*status == SQLITE_OK){
		*status = sqlite3_shutdown();

		if(*status != SQLITE_OK){
			sprintf(err_output, "Unable to Shutdown Database\nCode: %d", *status);
			Amulet_DisplayError2("Database Error", err_output, TRUE);
#ifdef PRINTF_ERRORS
			printf("Error: db_shutdown, %d\n", *status);
#endif
		}
	}else{
		sqlite3_shutdown();
	}
}
/**
 * \details Prepare a SQL query with capStatus(status), capDB(database) and capStmt(statement)
 * \param sCommand Pointer to Null terminated SQL query string
 * \returns None
 */
static void d_prepare_v2(const char *sCommand){
	db_prepare_v2(capStatus, capDB, sCommand, strlen(sCommand) + 1, &capStmt, NULL);
}
/**
 * \details Bind blob binary data to capStmt(statement) with capStatus(status)
 * \param name Pointer to Null terminated blob name
 * \param data Pointer to blob binary data
 * \param data_len Length of binary data
 * \param mem_callback Pointer to memory callback function
 * \returns None
 */
static void d_bind_blob(const char *name, const void *data, int data_len, void(*mem_callback)(void *data)){
	db_bind_blob(capStatus, capStmt, name, data, data_len, mem_callback);
}
/**
 * \details Bind double float to capStmt(statement) with capStatus(status)
 * \param name Pointer to Null terminated double name
 * \param data Double float number
 * \returns None
 */
static void d_bind_double(const char *name, double data){
	db_bind_double(capStatus, capStmt, name, data);
}
/**
 * \details Bind 32-bit integer data to capStmt(statement) with capStatus(status)
 * \param name Pointer to Null terminated integer name
 * \param data 32-bit integer number
 * \returns None
 */
static void d_bind_int(const char *name, int data){
	db_bind_int(capStatus, capStmt, name, data);
}
/**
 * \details  Bind 64-bit integer data to capStmt(statement) with capStatus(status)
 * \param name Pointer to Null terminated integer name
 * \param data 64-bit integer number
 * \returns None
 */
static void d_bind_int64(const char *name, sqlite3_int64 data){
	db_bind_int64(capStatus, capStmt, name, data);
}
/**
 * \details Bind null value to capStmt(statement) with capStatus(status)
 * \param name Pointer to Null terminated field name
 * \returns None
 */
static void d_bind_null(const char *name){
	db_bind_null(capStatus, capStmt, name);
}
/**
 * \details Bind ASCII text to capStmt(statement) with capStatus(status)
 * \param name Pointer to Null terminated text name
 * \param data Pointer to Null terminated text data
 * \returns None
 */
static void d_bind_text(const char *name, const char *data){
	db_bind_text(capStatus, capStmt, name, data, strlen(data), SQLITE_TRANSIENT);
}
/**
 * \details Bind Unicode text to capStmt(statement) with capStatus(status)
 * \param name Pointer to Null terminated text name
 * \param data Pointer to Null terminated text data
 * \returns None
 */
static void d_bind_text16(const char *name, const void *data){
	db_bind_text16(capStatus, capStmt, name, data, strlen(data), SQLITE_TRANSIENT);
}
/**
 * \details Bind value to capStmt(statement) with capStatus(status)
 * \param name Pointer to Null terminated field name
 * \param data_value Pointer to data
 * \returns None
 */
static void d_bind_value(const char *name, const sqlite3_value *data_value){
	db_bind_value(capStatus, capStmt, name, data_value);
}
/**
 * \details Bind a zero blob to capStmt(statement) with capStatus(status)
 * \param name Pointer to Null terminated field name
 * \param len Length of zero blob
 * \returns None
 */
static void d_bind_zeroblob(const char *name, int len){
	db_bind_zeroblob(capStatus, capStmt, name, len);
}
/**
 * \details Step to next row to capStmt(statement) with capStatus(status)
 * \returns None
 */
static void d_step(void){
	db_step(capStatus, capStmt);
}
/**
 * \details Get number of rows to capStmt(statement) with capStatus(status)
 * \returns Number of rows
 */
static int d_column_count(void){
	return db_column_count(capStatus, capStmt);
}
/**
 * \details Get blob data from the current row of capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns Pointer to blob data
 */
static const void *d_column_blob(int iCol){
	return db_column_blob(capStatus, capStmt, iCol);
}
/**
 * \details Get blob data length from the current row of capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns Blob data length
 */
static int d_column_bytes(int iCol){
	return db_column_bytes(capStatus, capStmt, iCol);
}
/**
 * \details Get 16-bit blob data length from the current row of capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns Blob data length
 */
static int d_column_bytes16(int iCol){
	return db_column_bytes16(capStatus, capStmt, iCol);
}
/**
 * \details Get double float value from the current row of capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns Double value
 */
static double d_column_double(int iCol){
	return db_column_double(capStatus, capStmt, iCol);
}
/**
 * \details Get 32-bit integer value from the current row of capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns 32-bit integer value
 */
static int d_column_int(int iCol){
	return db_column_int(capStatus, capStmt, iCol);
}
/**
 * \details Get 64-bit integer value from the current row of capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns 64-bit integer value
 */
static sqlite3_int64 d_column_int64(int iCol){
	return db_column_int64(capStatus, capStmt, iCol);
}
/**
 * \details Get ASCII text data from the current row of capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns Pointer to Null terminated text data
 */
static const unsigned char *d_column_text(int iCol){
	return db_column_text(capStatus, capStmt, iCol);
}
/**
 * \details Get 16-bit text data from the current row of capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns Pointer to Null terminated text data
 */
static const void *d_column_text16(int iCol){
	return db_column_text16(capStatus, capStmt, iCol);
}
/**
 * \details Get column type from capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns Column Type
 */
static int d_column_type(int iCol){
	return db_column_type(capStatus, capStmt, iCol);
}
/**
 * \details Get column value from the current row of capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns Column value
 */
static sqlite3_value *d_column_value(int iCol){
	return db_column_value(capStatus, capStmt, iCol);
}
/**
 * \details Get field name from capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns Pointer to Null terminated field name
 */
static const char *d_column_name(int iCol){
	return db_column_name(capStatus, capStmt, iCol);
}
/**
 * \details Get 16-bit field name from capStmt(statement) with capStatus(status)
 * \param iCol SQL result column index
 * \returns Pointer to Null terminated 16-bit field name
 */
static const void *d_column_name16(int iCol){
	return db_column_name16(capStatus, capStmt, iCol);
}
/**
 * \details Reset the capStmt(statement) with capStatus(status)
 * \returns None
 */
static void d_reset(void){
	db_reset(capStatus, capStmt);
}
/**
 * \details Close the capStmt(statement) with capStatus(status)
 * \returns None
 */
static void d_finalize(void){
	db_finalize(capStatus, capStmt);
}
/**
 * \details Execute a complete SQL command
 * \param sCommand Pointer to Null terminated SQL command string
 * \returns None
 */
static void d_command(const char *sCommand){
//static void d_command(int *status, sqlite3 *db, const char *sCommand){
	//db_prepare_v2(status, db, sCommand, strlen(sCommand) + 1, &stmt, NULL);
	d_prepare_v2(sCommand);
	//db_step(status, stmt);
	d_step();
	//db_finalize(status, stmt);
	d_finalize();
}
/**
 * \details Execute a SQL query and advance to the first row
 * \param sCommand Pointer to Null terminated SQL query string
 * \returns 0 = No rows returns, 1 = Valid row returned
 */
static int d_query(const char *sCommand){
	//prepare statement
	d_prepare_v2(sCommand);

	//step
	d_step();

	if(*capStatus == SQLITE_DONE) return 0;
	else return 1;
}

void DB_InitializeDetector(void){
	char query[200];

	if(capDB != NULL){
		*capStatus = SQLITE_OK;

		d_command("BEGIN;");
		d_command("UPDATE Config SET SettingValue = 0 WHERE SettingName='WellSerialNum';");
		d_command("UPDATE Config SET SettingValue = 660 WHERE SettingName='WellHV';");
		d_command("UPDATE Config SET SettingValue = 80 WHERE SettingName='WellThreshold';");
		d_command("UPDATE Config SET SettingValue = 0 WHERE SettingName='Probe700SerialNum';");
		d_command("UPDATE Config SET SettingValue = 3800 WHERE SettingName='Probe700HV';");
		d_command("UPDATE Config SET SettingValue = 80 WHERE SettingName='Probe700Threshold';");
		d_command("UPDATE Config SET SettingValue = 0 WHERE SettingName='Well700SerialNum';");
		d_command("UPDATE Config SET SettingValue = 3800 WHERE SettingName='Well700HV';");
		d_command("UPDATE Config SET SettingValue = 80 WHERE SettingName='Well700Threshold';");
		d_command("UPDATE Config SET SettingValue = 3 WHERE SettingName='Probe700Installed';");
		d_command("UPDATE Config SET SettingValue = 4 WHERE SettingName='Well700Installed';");
		d_command("UPDATE Config SET SettingValue = 256 WHERE SettingName='WellChannels';");
		d_command("UPDATE Config SET SettingValue = 512 WHERE SettingName='Probe700Channels';");
		d_command("UPDATE Config SET SettingValue = 512 WHERE SettingName='Well700Channels';");
		d_command("UPDATE Config SET SettingValue = 0 WHERE SettingName='WellAutoCalThreshold';");
		d_command("UPDATE Config SET SettingValue = 0 WHERE SettingName='Probe700AutoCalThreshold';");
		d_command("UPDATE Config SET SettingValue = 0 WHERE SettingName='Well700AutoCalThreshold';");
		d_command("UPDATE Config SET SettingValue = 60 WHERE SettingName='WellTime0';");
		d_command("UPDATE Config SET SettingValue = 60 WHERE SettingName='WellTime1';");
		d_command("UPDATE Config SET SettingValue = 60 WHERE SettingName='WellTime2';");
		d_command("UPDATE Config SET SettingValue = 60 WHERE SettingName='WellTime3';");
		d_command("UPDATE Config SET SettingValue = 60 WHERE SettingName='WellTime4';");
		d_command("UPDATE Config SET SettingValue = 60 WHERE SettingName='WellTime5';");
		d_command("UPDATE Config SET SettingValue = 60 WHERE SettingName='WellTime6';");
		d_command("UPDATE Config SET SettingValue = 60 WHERE SettingName='WellTime7';");
		d_command("UPDATE Config SET SettingValue = 0 WHERE SettingName='Branding';");
		d_command("UPDATE Config SET SettingValue = 1 WHERE SettingName='DefaultKEV';");
		d_command("UPDATE AutoCal SET Inactive = 1");

		strcpy(query, "UPDATE Config SET SettingValue = :DetectorTestSourceSN WHERE SettingName = 'DetectorTestSourceSN';");
		d_prepare_v2(query);
		d_bind_text(":DetectorTestSourceSN", "$FTXT$");
		d_step();
		d_finalize();

		strcpy(query, "UPDATE Config SET SettingValue = :DetectorTestSourceActivity WHERE SettingName = 'DetectorTestSourceActivity';");
		d_prepare_v2(query);
		d_bind_double(":DetectorTestSourceActivity", -1.0);
		d_step();
		d_finalize();

		strcpy(query, "UPDATE Config SET SettingValue = :DetectorTestSourceDate WHERE SettingName = 'DetectorTestSourceDate';");
		d_prepare_v2(query);
		d_bind_int(":DetectorTestSourceDate", 0);
		d_step();
		d_finalize();

		if(*capStatus == SQLITE_OK){
			d_command("COMMIT;");
			//DB_createEmptyUserNuclide();
			//DB_createEmptyUserCal();
		}else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_PopWellTime(void){
	int index;

	for(index=0; index<8; index++){
		DB_retrieveWellTime(&(AmuletWellMeasureMenu_countTime[index]), index);
	}
}

void DB_PopDetector(DETECTOR *detector, short detectorType){
	int SerialNum, HV, Threshold, Installed, NumOfChannels, AutoCalThreshold, index; // DetectorType;
	char snum[20];

	if(capDB != NULL){
		*capStatus = SQLITE_OK;

		switch(detectorType){
			case DET_WELL:
			case DET_BETA:
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'WellSerialNum';")){
					if(*capStatus == SQLITE_ROW){
						SerialNum = d_column_int(0);
					}else{
						SerialNum = 0;
					}
					d_finalize();
				}else{
					d_finalize();
					SerialNum = 0;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('WellSerialNum',0);");
				}
				sprintf(snum, "%06d", SerialNum);
				for(index=0; index<6; index++) detector->snum[index] = snum[index];

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'WellHV';")){
					if(*capStatus == SQLITE_ROW){
						HV = d_column_int(0);
					}else{
						HV = 660;
					}
					d_finalize();
				}else{
					d_finalize();
					HV = 660;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('WellHV',660);");
				}
				detector->hv = HV;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'WellThreshold';")){
					if(*capStatus == SQLITE_ROW){
						Threshold = d_column_int(0);
					}else{
						Threshold = 80;
					}
					d_finalize();
				}else{
					d_finalize();
					Threshold = 80;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('WellThreshold',80);");
				}
				detector->threshold = Threshold;

				Installed = DET_EMPTY;
				detector->installed = Installed;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'WellChannels';")){
					if(*capStatus == SQLITE_ROW){
						NumOfChannels = d_column_int(0);
					}else{
						NumOfChannels = 256;
					}
					d_finalize();
				}else{
					d_finalize();
					NumOfChannels = 256;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('WellChannels',256);");
				}
				detector->num_of_channels = NumOfChannels;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'WellAutoCalThreshold';")){
					if(*capStatus == SQLITE_ROW){
						AutoCalThreshold = d_column_int(0);
					}else{
						AutoCalThreshold = 0;
					}
					d_finalize();
				}else{
					d_finalize();
					AutoCalThreshold = 0;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('WellAutoCalThreshold',0);");
				}
				detector->auto_cal_threshold = AutoCalThreshold;

				detector->initial_gain_1 = 900;
				//detector->initial_gain_2 = 99;
				detector->initial_gain_2 = 4;
				break;

			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Probe700SerialNum';")){
					if(*capStatus == SQLITE_ROW){
						SerialNum = d_column_int(0);
					}else{
						SerialNum = 0;
					}
					d_finalize();
				}else{
					d_finalize();
					SerialNum = 0;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700SerialNum',0);");
				}
				sprintf(snum, "%06d", SerialNum);
				for(index=0; index<6; index++) detector->snum[index] = snum[index];

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Probe700HV';")){
					if(*capStatus == SQLITE_ROW){
						HV = d_column_int(0);
					}else{
						HV = 3800;
					}
					d_finalize();
				}else{
					d_finalize();
					HV = 3800;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700HV',3800);");
				}
				detector->hv = HV;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Probe700Threshold';")){
					if(*capStatus == SQLITE_ROW){
						Threshold = d_column_int(0);
					}else{
						Threshold = 80;
					}
					d_finalize();
				}else{
					d_finalize();
					Threshold = 80;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700Threshold',80);");
				}
				detector->threshold = Threshold;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Probe700Installed';")){
					if(*capStatus == SQLITE_ROW){
						Installed = d_column_int(0);
					}else{
						Installed = 3;
					}
					d_finalize();
				}else{
					d_finalize();
					Installed = 3;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700Installed',3);");
				}
				detector->installed = Installed;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Probe700Channels';")){
					if(*capStatus == SQLITE_ROW){
						NumOfChannels = d_column_int(0);
					}else{
						NumOfChannels = 512;
					}
					d_finalize();
				}else{
					d_finalize();
					NumOfChannels = 512;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700Channels',512);");
				}
				detector->num_of_channels = NumOfChannels;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Probe700AutoCalThreshold';")){
					if(*capStatus == SQLITE_ROW){
						AutoCalThreshold = d_column_int(0);
					}else{
						AutoCalThreshold = 0;
					}
					d_finalize();
				}else{
					d_finalize();
					AutoCalThreshold = 0;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Probe700AutoCalThreshold',0);");
				}
				detector->auto_cal_threshold = AutoCalThreshold;

				detector->initial_gain_1 = 900;
				detector->initial_gain_2 = 4;
				break;

			case DET_WELL700:
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Well700SerialNum';")){
					if(*capStatus == SQLITE_ROW){
						SerialNum = d_column_int(0);
					}else{
						SerialNum = 0;
					}
					d_finalize();
				}else{
					d_finalize();
					SerialNum = 0;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Well700SerialNum',0);");
				}
				sprintf(snum, "%06d", SerialNum);
				for(index=0; index<6; index++) detector->snum[index] = snum[index];

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Well700HV';")){
					if(*capStatus == SQLITE_ROW){
						HV = d_column_int(0);
					}else{
						HV = 3800;
					}
					d_finalize();
				}else{
					d_finalize();
					HV = 3800;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Well700HV',3800);");
				}
				detector->hv = HV;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Well700Threshold';")){
					if(*capStatus == SQLITE_ROW){
						Threshold = d_column_int(0);
					}else{
						Threshold = 80;
					}
					d_finalize();
				}else{
					d_finalize();
					Threshold = 80;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Well700Threshold',80);");
				}
				detector->threshold = Threshold;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Well700Installed';")){
					if(*capStatus == SQLITE_ROW){
						Installed = d_column_int(0);
					}else{
						Installed = 4;
					}
					d_finalize();
				}else{
					d_finalize();
					Installed = 4;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Well700Installed',4);");
				}
				detector->installed = Installed;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Well700Channels';")){
					if(*capStatus == SQLITE_ROW){
						NumOfChannels = d_column_int(0);
					}else{
						NumOfChannels = 512;
					}
					d_finalize();
				}else{
					d_finalize();
					NumOfChannels = 512;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Well700Channels',512);");
				}
				detector->num_of_channels = NumOfChannels;

				*capStatus = SQLITE_OK;
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Well700AutoCalThreshold';")){
					if(*capStatus == SQLITE_ROW){
						AutoCalThreshold = d_column_int(0);
					}else{
						AutoCalThreshold = 0;
					}
					d_finalize();
				}else{
					d_finalize();
					AutoCalThreshold = 0;
					d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Well700AutoCalThreshold',0);");
				}
				detector->auto_cal_threshold = AutoCalThreshold;

				detector->initial_gain_1 = 900;
				detector->initial_gain_2 = 4;
				break;
		}
	}
}

void DB_PopDetectorStandard(STAND *detectorStandard){
	bool  flgFound;
	char snfixup[20], prefix[20];
	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'DetectorTestSourceSN';")){
			if(*capStatus == SQLITE_ROW){
				strcpy(snfixup, (const char *) d_column_text(0));
				strcpy(prefix, snfixup);
				prefix[6] = 0;
				if(strcmp(prefix, "$FTXT$") == 0){
					strcpy(prefix, &(snfixup[6]));
					prefix[10] = 0;
					strcpy(detectorStandard->sn, prefix);
					flgFound = TRUE;
				}else{
					snfixup[10] = 0;
					strcpy(detectorStandard->sn, snfixup);
					flgFound = FALSE;
				}
			}else{
				flgFound = TRUE;
				detectorStandard->sn[0] = 0;
			}
			d_finalize();
			if(!flgFound){
				strcpy(snfixup, "$FTXT$");
				strcat(snfixup, detectorStandard->sn);
				d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('DetectorTestSourceSN',:DetectorTestSourceSN);");
				d_bind_text(":DetectorTestSourceSN", snfixup);
				d_step();
				d_finalize();
			}
		}else{
			d_finalize();
			detectorStandard->sn[0] = 0;
			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('DetectorTestSourceSN',:DetectorTestSourceSN);");
			d_bind_text(":DetectorTestSourceSN", "$FTXT$");
			d_step();
			d_finalize();
		}

		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'DetectorTestSourceActivity';")){
			if(*capStatus == SQLITE_ROW){
				detectorStandard->act = d_column_double(0);
			}else{
				detectorStandard->act = -1.0;
			}
			d_finalize();
		}else{
			d_finalize();
			detectorStandard->act = -1.0;
			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('DetectorTestSourceActivity',:DetectorTestSourceActivity);");
			d_bind_double(":DetectorTestSourceActivity", -1.0);
			d_step();
			d_finalize();
		}

		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'DetectorTestSourceDate';")){
			if(*capStatus == SQLITE_ROW){
				detectorStandard->caldate = (time_t) d_column_int(0);
			}else{
				detectorStandard->caldate = (time_t) 0;
			}
			d_finalize();
		}else{
			d_finalize();
			detectorStandard->caldate = (time_t) 0;
			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('DetectorTestSourceDate',:DetectorTestSourceDate);");
			d_bind_int(":DetectorTestSourceDate", 0);
			d_step();
			d_finalize();
		}
	}
}
/**
 * \details Read Branding ID from the Config table
 * \returns Branding ID, 0 = CRC-55t, 1 = CAPRAC-t, 2 = CAPTUS-700t, 3 = CRC-77t
 */
int DB_ReadBranding(void){
	int returnvalue;

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Branding';")){
			if(*capStatus == SQLITE_ROW){
				returnvalue = d_column_int(0);
			}else{
				returnvalue = 0;
			}
			d_finalize();
		}else{
			d_finalize();
			returnvalue = 0;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Branding',0);");
		}
	}

	return returnvalue;
}
/**
 * \details Write Branding ID to the Config table
 * \param Branding Branding ID, 0 = CRC-55t, 1 = CAPRAC-t, 2 = CAPTUS-700t, 3 = CRC-77t
 * \returns None
 */
void DB_WriteBranding(int Branding){
	char query[200];

	strcpy(query, "UPDATE Config SET SettingValue = :Branding WHERE SettingName = 'Branding';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":Branding", Branding);
	d_step();
	d_finalize();
}

int DB_ReadDefaultKEV(void){
	int returnvalue;

		if(capDB != NULL){
			*capStatus = SQLITE_OK;
			if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'DefaultKEV';")){
				if(*capStatus == SQLITE_ROW){
					returnvalue = d_column_int(0);
				}else{
					returnvalue = 1;
				}
				d_finalize();
			}else{
				d_finalize();
				returnvalue = 1;
				d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('DefaultKEV',1);");
			}
		}

		return returnvalue;
}

void DB_WriteDefaultKEV(int DefaultKEV){
	char query[200];

	strcpy(query, "UPDATE Config SET SettingValue = :DefaultKEV WHERE SettingName = 'DefaultKEV';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":DefaultKEV", DefaultKEV);
	d_step();
	d_finalize();
}

void DB_InactiveAutoCalAndBkg(int detectorType){
	*capStatus = SQLITE_OK;
	d_command("BEGIN;");

	d_prepare_v2("UPDATE AutoCal SET Inactive = 1 WHERE DetectorType = :DetectorType;");
	d_bind_int(":DetectorType", detectorType);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE WellBackground SET Inactive = 1 WHERE DetectorType = :DetectorType;");
	d_bind_int(":DetectorType", detectorType);
	d_step();
	d_finalize();

	if(*capStatus == SQLITE_OK)	d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}

void DB_PopAutoCal(AUTOCAL *autoCal, int detectorType){
	int index;
	char calstamp[30], linstamp[30];
	float calstampfloat, linstampfloat;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT AutoCalID, ZeroOffset, Gain1, Gain2, Sigma, Sigma_Lip, Linearity0, Linearity1, Linearity2, Linearity3, Linearity4, datetime(CalStamp), CalStamp, datetime(LinStamp), LinStamp, SerialNum, Threshold, HV, NumOfChannels FROM AutoCal WHERE Inactive = 0 AND DetectorType = :DetectorType;");
	d_bind_int(":DetectorType", detectorType);
	d_step();
	if(*capStatus == SQLITE_ROW){
		autoCal->AutoCalID = d_column_int64(0);
		autoCal->zeroopampoffset = d_column_int(1);
		autoCal->gain1 = d_column_int(2);
		autoCal->gain2 = d_column_int(3);
		autoCal->sigma = d_column_double(4);
		autoCal->sigma_lip = d_column_double(5);
		autoCal->linmeasured[0] = d_column_double(6);
		autoCal->linmeasured[1] = d_column_double(7);
		autoCal->linmeasured[2] = d_column_double(8);
		autoCal->linmeasured[3] = d_column_double(9);
		autoCal->linmeasured[4] = d_column_double(10);
		autoCal->detectortype = detectorType;
		strcpy(calstamp, (const char *) d_column_text(11));
		calstampfloat = d_column_double(12);
		strcpy(linstamp, (const char *) d_column_text(13));
		linstampfloat = d_column_double(14);
		strcpy(autoCal->serialnum, (const char *) d_column_text(15));
		autoCal->threshold = d_column_int(16);
		autoCal->hv = d_column_int(17);
		autoCal->num_of_channels = d_column_int(18);

		if(calstampfloat == 0) autoCal->caltstamp = 0;
		else autoCal->caltstamp = strtotime_t(calstamp);

		if(linstampfloat == 0) autoCal->lintstamp = 0;
		else autoCal->lintstamp = strtotime_t(linstamp);

		MCACalcLinFactors(autoCal);
	}else{
		autoCal->AutoCalID = 0;
		autoCal->zeroopampoffset  = 0;
		autoCal->gain1 = 900;
		autoCal->gain2 = 4;
		autoCal->sigma = 0;
		autoCal->sigma_lip = 0;
		for(index=0; index<5; index++){
			autoCal->linmeasured[index] = 0.0;
			autoCal->linStd[index] = 0.0;
			autoCal->linMeas[index] = 0.0;
		}
		for(index=0; index<6; index++){
			autoCal->factorMtoS[index] = 0.0;
			autoCal->factorStoM[index] = 0.0;
		}
		autoCal->serialnum[0] = 0;
		autoCal->threshold = 0;
		autoCal->hv = 0;
		autoCal->num_of_channels = Mca_getDetectorMirrorPtr()->num_of_channels;
		autoCal->caltstamp = 0;
		autoCal->lintstamp = 0;
		autoCal->linearityActive = FALSE;
		autoCal->detectortype = 0;
	}
	d_finalize();
}

void DB_WriteWellSerialNum(char *SerialNum, short detectorType){
	int serialnum;
	char query[200];

	serialnum = atoi(SerialNum);
	switch(detectorType){
		case DET_WELL:
		case DET_BETA:
			strcpy(query, "UPDATE Config SET SettingValue = :SerialNum WHERE SettingName = 'WellSerialNum';");
			break;

		case DET_PROBE700:
		case DET_DRILLEDPROBE700:
			strcpy(query, "UPDATE Config SET SettingValue = :SerialNum WHERE SettingName = 'Probe700SerialNum';");
			break;

		case DET_WELL700:
			strcpy(query, "UPDATE Config SET SettingValue = :SerialNum WHERE SettingName = 'Well700SerialNum';");
			break;
	}

	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":SerialNum", serialnum);
	d_step();
	d_finalize();
}

void DB_ReadWellSerialNum(char *SerialNum, short detectorType){
	int nSerialNum;
	char snum[20];

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		switch(detectorType){
			case DET_WELL:
			case DET_BETA:
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'WellSerialNum';")){
					if(*capStatus == SQLITE_ROW){
						nSerialNum = d_column_int(0);
					}else{
						nSerialNum = 0;
					}
					d_finalize();
				}else{
					d_finalize();
					nSerialNum = 0;
					*capStatus = SQLITE_OK;
					d_command("INSERT INTO Config(SettingName, SettingValue) VALUES('WellSerialNum', 0);");
				}
				break;

			case DET_PROBE700:
			case DET_DRILLEDPROBE700:
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Probe700SerialNum';")){
					if(*capStatus == SQLITE_ROW){
						nSerialNum = d_column_int(0);
					}else{
						nSerialNum = 0;
					}
					d_finalize();
				}else{
					d_finalize();
					nSerialNum = 0;
					*capStatus = SQLITE_OK;
					d_command("INSERT INTO Config(SettingName, SettingValue) VALUES('Probe700SerialNum', 0);");
				}
				break;

			case DET_WELL700:
				if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Well700SerialNum';")){
					if(*capStatus == SQLITE_ROW){
						nSerialNum = d_column_int(0);
					}else{
						nSerialNum = 0;
					}
					d_finalize();
				}else{
					d_finalize();
					nSerialNum = 0;
					*capStatus = SQLITE_OK;
					d_command("INSERT INTO Config(SettingName, SettingValue) VALUES('Well700SerialNum', 0);");
				}
				break;
		}

		sprintf(snum, "%06d", nSerialNum);
		strcpy(SerialNum, snum);
	}
}

void DB_WriteWellHV(short HV, short detectorType){
	char query[200];

	switch(detectorType){
		case DET_WELL:
		case DET_BETA:
			strcpy(query, "UPDATE Config SET SettingValue = :HV WHERE SettingName = 'WellHV';");
			break;

		case DET_PROBE700:
		case DET_DRILLEDPROBE700:
			strcpy(query, "UPDATE Config SET SettingValue = :HV WHERE SettingName = 'Probe700HV';");
			break;

		case DET_WELL700:
			strcpy(query, "UPDATE Config SET SettingValue = :HV WHERE SettingName = 'Well700HV';");
			break;
	}

	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":HV", HV);
	d_step();
	d_finalize();
}

void DB_WriteWellThreshold(short Threshold, short detectorType){
	char query[200];

	switch(detectorType){
		case DET_WELL:
		case DET_BETA:
			strcpy(query, "UPDATE Config SET SettingValue = :Threshold WHERE SettingName = 'WellThreshold';");
			break;

		case DET_PROBE700:
		case DET_DRILLEDPROBE700:
			strcpy(query, "UPDATE Config SET SettingValue = :Threshold WHERE SettingName = 'Probe700Threshold';");
			break;

		case DET_WELL700:
			strcpy(query, "UPDATE Config SET SettingValue = :Threshold WHERE SettingName = 'Well700Threshold';");
			break;
	}
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":Threshold", Threshold);
	d_step();
	d_finalize();
}

void DB_WriteWellChannels(int Channels, short detectorType){
	char query[200];

	switch(detectorType){
		case DET_WELL:
		case DET_BETA:
			strcpy(query, "UPDATE Config SET SettingValue = :Channels WHERE SettingName = 'WellChannels';");
			break;

		case DET_PROBE700:
		case DET_DRILLEDPROBE700:
			strcpy(query, "UPDATE Config SET SettingValue = :Channels WHERE SettingName = 'Probe700Channels';");
			break;

		case DET_WELL700:
			strcpy(query, "UPDATE Config SET SettingValue = :Channels WHERE SettingName = 'Well700Channels';");
			break;
	}
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":Channels", Channels);
	d_step();
	d_finalize();
}

void DB_WriteAutoCalThreshold(int AutoCalThreshold, short detectorType){
	char query[200];

	switch(detectorType){
		case DET_WELL:
		case DET_BETA:
			strcpy(query, "UPDATE Config SET SettingValue = :AutoCalThreshold WHERE SettingName = 'WellAutoCalThreshold';");
			break;

		case DET_PROBE700:
		case DET_DRILLEDPROBE700:
			strcpy(query, "UPDATE Config SET SettingValue = :AutoCalThreshold WHERE SettingName = 'Probe700AutoCalThreshold';");
			break;

		case DET_WELL700:
			strcpy(query, "UPDATE Config SET SettingValue = :AutoCalThreshold WHERE SettingName = 'Well700AutoCalThreshold';");
			break;
	}
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":AutoCalThreshold", AutoCalThreshold);
	d_step();
	d_finalize();
}

void DB_WriteWellInstalled(short installed, short detectorType){
	char query[200];

	switch(detectorType){
		case DET_PROBE700:
		case DET_DRILLEDPROBE700:
			strcpy(query, "UPDATE Config SET SettingValue = :Installed WHERE SettingName = 'Probe700Installed';");
			break;

		case DET_WELL700:
			strcpy(query, "UPDATE Config SET SettingValue = :Installed WHERE SettingName = 'Well700Installed';");
			break;
	}
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":Installed", installed);
	d_step();
	d_finalize();
}

void DB_WriteDetector(DETECTOR *detector, short detectorType){
	short index;
	char sn[7];

	for(index=0; index<6; index++) sn[index] = detector->snum[index];
	sn[6] = 0;

	DB_WriteWellSerialNum(sn, detectorType);
	DB_WriteWellHV(detector->hv, detectorType);
	DB_WriteWellThreshold(detector->threshold, detectorType);
	DB_WriteWellChannels(detector->num_of_channels, detectorType);
	if((detectorType == DET_PROBE700) || (detectorType == DET_DRILLEDPROBE700) || (detectorType == DET_WELL700)){
		DB_WriteWellInstalled(detector->installed, detectorType);
	}
}

void DB_WriteWellCalib(AUTOCAL *autoCal, bool bookEnd){
	char datetimestr[30];
	sqlite3_int64 rowid;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}
	d_prepare_v2("UPDATE AutoCal SET Inactive = 1 WHERE DetectorType = :DetectorType;");
	d_bind_int(":DetectorType", autoCal->detectortype);
	d_step();
	d_finalize();

	d_prepare_v2("INSERT INTO AutoCal(ZeroOffset, Gain1, Gain2, Sigma, Sigma_Lip, Linearity0, Linearity1, Linearity2, Linearity3, Linearity4, CalStamp, LinStamp, Inactive, DetectorType, SerialNum, Threshold, HV, NumOfChannels) VALUES(:ZeroOffset, :Gain1, :Gain2, :Sigma, :Sigma_Lip, :Linearity0, :Linearity1, :Linearity2, :Linearity3, :Linearity4, :CalStamp, :LinStamp, :Inactive, :DetectorType, :SerialNum, :Threshold, :HV, :NumOfChannels);");
	d_bind_int(":ZeroOffset", autoCal->zeroopampoffset);
	d_bind_int(":Gain1", autoCal->gain1);
	d_bind_int(":Gain2", autoCal->gain2);
	d_bind_double(":Sigma", autoCal->sigma);
	d_bind_double(":Sigma_Lip", autoCal->sigma_lip);
	d_bind_double(":Linearity0", autoCal->linmeasured[0]);
	d_bind_double(":Linearity1", autoCal->linmeasured[1]);
	d_bind_double(":Linearity2", autoCal->linmeasured[2]);
	d_bind_double(":Linearity3", autoCal->linmeasured[3]);
	d_bind_double(":Linearity4", autoCal->linmeasured[4]);
	d_bind_int(":DetectorType", autoCal->detectortype);
	d_bind_int(":CalStamp", 0);
	d_bind_int(":LinStamp", 0);
	d_bind_int(":Inactive", 0);
	d_bind_text(":SerialNum", autoCal->serialnum);
	d_bind_int(":Threshold", autoCal->threshold);
	d_bind_int(":HV", autoCal->hv);
	d_bind_int(":NumOfChannels", autoCal->num_of_channels);
	d_step();

	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
	else rowid = -1;

	d_finalize();

	if(rowid != -1){
		if((autoCal->caltstamp) != 0){
			stringout(datetimestr, &(autoCal->caltstamp));
			d_prepare_v2("UPDATE AutoCal SET CalStamp = julianday(:datetimestr) WHERE AutoCalID = :rowid;");
			d_bind_text(":datetimestr", datetimestr);
			d_bind_int64(":rowid", rowid);
			d_step();
			d_finalize();
		}

		if((autoCal->lintstamp) != 0){
			stringout(datetimestr, &(autoCal->lintstamp));
			d_prepare_v2("UPDATE AutoCal SET LinStamp = julianday(:datetimestr) WHERE AutoCalID = :rowid;");
			d_bind_text(":datetimestr", datetimestr);
			d_bind_int64(":rowid", rowid);
			d_step();
			d_finalize();
		}

		autoCal->AutoCalID = rowid;
	}

	d_prepare_v2("UPDATE WellBackground SET Inactive = 1 WHERE DetectorType = :DetectorType;");
	d_bind_int(":DetectorType", autoCal->detectortype);
	d_step();
	d_finalize();

	Mca_clearBackgroundMirror();

	if(bookEnd){
		if(*capStatus == SQLITE_OK)	d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

ulong CalcBitIndex(int Index, int BitsPerChannel, bool UseLastBit){
	ulong returnValue;
	returnValue = Index * BitsPerChannel;
	if(UseLastBit) returnValue = returnValue + BitsPerChannel - 1;
	return returnValue;
}

void CalcByteAndBit(ulong BitIndex, int *ByteLoc, int *BitLoc){
	*ByteLoc = BitIndex / 8;
	*BitLoc = BitIndex % 8;
}

int CompressSpectra(int NumOfChannels, ulong *spectra, int *BitsPerChannel, int *CompressedSpectraSize, uchar *CompressedSpectra){
	int index, jndex, ByteLoc, BitLoc, MaxTransferByteIndex, returnValue;
	ulong MaxCount, TotalBits, BitIndexStart;
	ulong *pSpectra;
	uchar *pCompressedSpectra;
	unsigned long long int working;
	uchar *workingPtr;

	MaxCount = 0;
	pSpectra = spectra;
	returnValue = 0;
	for(index=0; index<NumOfChannels; index++){
		returnValue += *pSpectra;
		if(*pSpectra > MaxCount) MaxCount = *pSpectra;
		pSpectra++;
	}

	//printf("MaxCount: %lu\n", MaxCount);
	*BitsPerChannel = 0;
	if(MaxCount == 0) *BitsPerChannel = 1;
	else{
		while(MaxCount > 0){
			MaxCount = MaxCount >> 1;
			*BitsPerChannel = (*BitsPerChannel) + 1;
		}
	}
	//printf("BitsPerChannel: %d\n", *BitsPerChannel);

	TotalBits = NumOfChannels;
	TotalBits *= (*BitsPerChannel);

	*CompressedSpectraSize = TotalBits / 8;
	if(TotalBits % 8){
		*CompressedSpectraSize = (*CompressedSpectraSize) + 1;
	}

	pCompressedSpectra = CompressedSpectra;
	for(index = 0; index < (*CompressedSpectraSize); index++){
		*pCompressedSpectra = 0;
		pCompressedSpectra++;
	}

	if(*BitsPerChannel == 1){
		MaxTransferByteIndex = 1;
	}else if(((*BitsPerChannel)%8) == 0){
		MaxTransferByteIndex = (*BitsPerChannel) / 8;
	}else if(*BitsPerChannel == 9){
		MaxTransferByteIndex = 2;
	}else if(*BitsPerChannel == 17){
		MaxTransferByteIndex = 3;
	}else if(*BitsPerChannel == 25){
		MaxTransferByteIndex = 4;
	}else{
		MaxTransferByteIndex = ((*BitsPerChannel) / 8) + 2;
	}

	pSpectra = spectra;
	for(index=0; index<NumOfChannels; index++){
		//BitIndexStart = CalcBitIndex(index, *BitsPerChannel, 0);

		//printf("index: %d, BitIndexStart: %d\n", index, BitIndexStart);

		//CalcByteAndBit(BitIndexStart, &ByteLoc, &BitLoc);
		//printf("BitIndexStart: %d, ByteLoc: %d, BitLoc: %d\n", BitIndexStart, ByteLoc, BitLoc);

		BitIndexStart = index * (*BitsPerChannel);
		ByteLoc = BitIndexStart / 8;
		BitLoc = BitIndexStart % 8;

		working = *pSpectra;
		working <<= BitLoc;
		//printf("Before: %X, After: %llX\n", *pSpectra, working);

		workingPtr = (uchar *) &working;
		workingPtr += 7;
		pCompressedSpectra = CompressedSpectra + ByteLoc;
		for(jndex=0; jndex<MaxTransferByteIndex; jndex++){
			*pCompressedSpectra += *workingPtr;
			pCompressedSpectra++;
			workingPtr--;
		}

		pSpectra++;
	}

	//pSpectra = spectra;
	//pCompressedSpectra = CompressedSpectra;
	//for(index=0; index<NumOfChannels; index++){
	//	printf("%d - Raw:%u, Comp: %u\n", index, *pSpectra++, *pCompressedSpectra++);
	//}

	return returnValue;
}

void DecompressSpectra(int NumOfChannels, int BitsPerChannel, uchar *CompressedSpectra, ulong *DecompressedSpectra){
	int index, jndex;
	uchar *pCompressedSpectra, *workingPtr;
	ulong *pDecompressedSpectra;
	int BitIndexStart, ByteLoc, BitLoc, MaxTransferByteIndex;
	unsigned long long int working, mask;

	if(BitsPerChannel == 1){
		MaxTransferByteIndex = 1;
	}else if((BitsPerChannel % 8) == 0){
		MaxTransferByteIndex = BitsPerChannel >> 3;
	}else if(BitsPerChannel == 9){
		MaxTransferByteIndex = 2;
	}else if(BitsPerChannel == 17){
		MaxTransferByteIndex = 3;
	}else if(BitsPerChannel == 25){
		MaxTransferByteIndex = 4;
	}else{
		MaxTransferByteIndex = (BitsPerChannel >> 3) + 2;
	}

	mask = 1;
	mask <<= BitsPerChannel;
	mask -= 1;

	pDecompressedSpectra = DecompressedSpectra;
	for(index=0; index<NumOfChannels; index++){
		BitIndexStart = index * BitsPerChannel;
		ByteLoc = BitIndexStart >> 3;
		BitLoc = BitIndexStart % 8;

		working = 0;
		workingPtr = (uchar *) &working;
		workingPtr += 7;
		pCompressedSpectra = CompressedSpectra + ByteLoc;
		for(jndex=0; jndex<MaxTransferByteIndex; jndex++){
			*workingPtr = *pCompressedSpectra;
			workingPtr--;
			pCompressedSpectra++;
		}

		working >>= BitLoc;

		*pDecompressedSpectra = working & mask;
		pDecompressedSpectra++;
	}
}

void DB_ReadSpectrum(DB_SPEC *db_spec){
	char MeasuredOn[40];
	uchar *ptrWorking;
	uchar *Compressed;
	uchar *CompressedWorking;
	int index, CompressedSize;

	if(capDB != NULL){
		db_spec->AutoCal.AutoCalID = 0;
		db_spec->AutoCal.zeroopampoffset = 0;
		db_spec->AutoCal.gain1 = 0;
		db_spec->AutoCal.gain2 = 0;
		db_spec->AutoCal.sigma = 0;
		db_spec->AutoCal.sigma_lip = 0;
		for(index=0; index<5; index++){
			db_spec->AutoCal.linmeasured[index] = 0.0;
			db_spec->AutoCal.linStd[index] = 0.0;
			db_spec->AutoCal.linMeas[index] = 0.0;
		}
		for(index=0; index<6; index++){
			db_spec->AutoCal.factorMtoS[index] = 0.0;
			db_spec->AutoCal.factorStoM[index] = 0.0;
		}
		db_spec->AutoCal.serialnum[0] = 0;
		db_spec->AutoCal.threshold = 0;
		db_spec->AutoCal.hv = 0;
		db_spec->AutoCal.caltstamp = 0;
		db_spec->AutoCal.lintstamp = 0;
		db_spec->AutoCal.linearityActive = FALSE;
		db_spec->AutoCal.detectortype = 0;

		*capStatus = SQLITE_OK;
		d_prepare_v2("SELECT SpectraID, TotalCounts, LiveTime, RealTime, TotalCPM, datetime(MeasuredOn), AutoCalID, NumOfChannels, BitsPerChannel, CompressedSpectra FROM Spectra WHERE SpectraID = :SpectraID;");
		d_bind_int64(":SpectraID", db_spec->SpectraID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			db_spec->SpectraID = d_column_int64(0);
			db_spec->TotalCounts = d_column_int(1);
			db_spec->LiveTime = d_column_double(2);
			db_spec->RealTime = d_column_double(3);
			db_spec->TotalCPM = d_column_double(4);
			strcpy(MeasuredOn, (const char *) d_column_text(5));
			db_spec->MeasuredOn = strtotime_t(MeasuredOn);
			db_spec->AutoCal.AutoCalID = d_column_int64(6);
			db_spec->NumOfChannels = d_column_int(7);
			db_spec->BitsPerChannel = d_column_int(8);
			ptrWorking = (uchar *) d_column_blob(9);
			CompressedSize = d_column_bytes(9);
			Compressed = malloc(4096*4);
			CompressedWorking = Compressed;
			for(index=0; index<CompressedSize; index++){
				*CompressedWorking++ = *ptrWorking++;
			}
			DecompressSpectra(db_spec->NumOfChannels, db_spec->BitsPerChannel, Compressed, db_spec->DecompressedSpectra);
			free(Compressed);
		}else{
			// Fill in with empty
			db_spec->SpectraID = 0;
			db_spec->TotalCounts = 0;
			db_spec->LiveTime = 0;
			db_spec->RealTime = 0;
			db_spec->TotalCPM = 0;
			db_spec->MeasuredOn = 0;
			db_spec->NumOfChannels = 0;
			db_spec->BitsPerChannel = 0;
		}
		d_finalize();

		if(db_spec->AutoCal.AutoCalID > 0){
			d_prepare_v2("SELECT ZeroOffset, Gain1, Gain2, Sigma, Sigma_Lip, Linearity0, Linearity1, Linearity2, Linearity3, Linearity4, DetectorType, datetime(CalStamp), datetime(LinStamp), SerialNum, Threshold, HV, NumOfChannels FROM AutoCal WHERE AutoCalID = :AutoCalID");
			d_bind_int64(":AutoCalID", db_spec->AutoCal.AutoCalID);
			d_step();
			if(*capStatus == SQLITE_ROW){
				db_spec->AutoCal.zeroopampoffset = d_column_int(0);
				db_spec->AutoCal.gain1 = d_column_int(1);
				db_spec->AutoCal.gain2 = d_column_int(2);
				db_spec->AutoCal.sigma = d_column_double(3);
				db_spec->AutoCal.sigma_lip = d_column_double(4);
				db_spec->AutoCal.linmeasured[0] = d_column_double(5);
				db_spec->AutoCal.linmeasured[1] = d_column_double(6);
				db_spec->AutoCal.linmeasured[2] = d_column_double(7);
				db_spec->AutoCal.linmeasured[3] = d_column_double(8);
				db_spec->AutoCal.linmeasured[4] = d_column_double(9);
				db_spec->AutoCal.detectortype = d_column_int(10);
				strcpy(MeasuredOn, (const char *) d_column_text(11));
				db_spec->AutoCal.caltstamp = strtotime_t(MeasuredOn);
				strcpy(MeasuredOn, (const char *) d_column_text(12));
				db_spec->AutoCal.lintstamp = strtotime_t(MeasuredOn);
				strcpy(db_spec->AutoCal.serialnum, (const char *) d_column_text(13));
				db_spec->AutoCal.threshold = d_column_int(14);
				db_spec->AutoCal.hv = d_column_int(15);
				db_spec->AutoCal.num_of_channels = d_column_int(16);
			}
			d_finalize();

			MCACalcLinFactors(&(db_spec->AutoCal));
		}
	}
}

long long int DB_WriteSpectrum(DB_SPEC *spectrum, bool bookEnd){
	long long int rowid;
	char MeasuredOn[40];
	int TotalCounts, NumOfChannels, BitsPerChannel, CompressedSpectraSize;
	uchar *CompressedSpectra;
	//unsigned char CompressedSpectra[1024];

	// Generate NumOfChannels, BitsPerChannel, CompressedSpectra
	NumOfChannels = spectrum->NumOfChannels;
	CompressedSpectra = malloc(16384);
	TotalCounts = CompressSpectra(NumOfChannels, (ulong *) spectrum->DecompressedSpectra, &BitsPerChannel, &CompressedSpectraSize, CompressedSpectra);

	rowid = -1;
	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}
	d_prepare_v2("INSERT INTO Spectra(TotalCounts, LiveTime, RealTime, TotalCPM, MeasuredOn, AutoCalID, NumOfChannels, BitsPerChannel, CompressedSpectra) VALUES(:TotalCounts, :LiveTime, :RealTime, :TotalCPM, julianday(:MeasuredOn), :AutoCalID, :NumOfChannels, :BitsPerChannel, :CompressedSpectra);");
	d_bind_int(":TotalCounts", TotalCounts);
	d_bind_double(":LiveTime", spectrum->LiveTime);
	d_bind_double(":RealTime", spectrum->RealTime);
	d_bind_double(":TotalCPM", spectrum->TotalCPM);
	stringout(MeasuredOn, &(spectrum->MeasuredOn));
	d_bind_text(":MeasuredOn", MeasuredOn);
	d_bind_int64(":AutoCalID", spectrum->AutoCal.AutoCalID);
	d_bind_int(":NumOfChannels", NumOfChannels);
	d_bind_int(":BitsPerChannel", BitsPerChannel);
	d_bind_blob(":CompressedSpectra", CompressedSpectra, CompressedSpectraSize, SQLITE_TRANSIENT);
	d_step();

	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK){
		rowid = sqlite3_last_insert_rowid(capDB);
	}
	d_finalize();
	free(CompressedSpectra);

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}

	if(rowid > 0){
		spectrum->SpectraID = rowid;
		spectrum->BitsPerChannel = BitsPerChannel;
	}
	return rowid;
}

void DB_SPEC_MEAS_to_DB_SPEC(DB_SPEC *spectrum, SPEC_MEAS *spec_meas){
	int index;
	ulong TotalCounts;

	spectrum->BitsPerChannel = 32;
	TotalCounts = 0;
	for(index=0; index<spec_meas->num_of_channels; index++){
		TotalCounts += spec_meas->spectrum[index];
		spectrum->DecompressedSpectra[index] = spec_meas->spectrum[index];
	}
	spectrum->LiveTime = spec_meas->live_time;
	spectrum->MeasuredOn = spec_meas->stamp;
	spectrum->NumOfChannels = spec_meas->num_of_channels;
	spectrum->RealTime = spec_meas->real_time;
	spectrum->SpectraID = 0;
	spectrum->TotalCPM = spec_meas->averagecpm;
	spectrum->TotalCounts = TotalCounts;
	memcpy(&(spectrum->AutoCal), Mca_getCurrentAutoCal(), sizeof(AUTOCAL));
}

void DB_WriteWellBackground(int detectorType, SPEC_MEAS *spec_meas, BACKGND *bkg_meas, bool bookEnd){
	long long int SpectraID;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	DB_SPEC_MEAS_to_DB_SPEC(&(bkg_meas->Spectrum), spec_meas);
	SpectraID = DB_WriteSpectrum(&(bkg_meas->Spectrum), FALSE);
	bkg_meas->WellBackgroundID = 0;
	bkg_meas->Detector = detectorType;
	bkg_meas->Inactive = FALSE;

	d_prepare_v2("UPDATE WellBackground SET Inactive = 1 WHERE DetectorType = :DetectorType;");
	d_bind_int(":DetectorType", detectorType);
	d_step();
	d_finalize();

	d_prepare_v2("INSERT INTO WellBackground(WellBackground_SpectraID, DetectorType, Inactive) VALUES(:WellBackground_SpectraID, :DetectorType, :Inactive);");
	d_bind_int64(":WellBackground_SpectraID", SpectraID);
	d_bind_int(":DetectorType", detectorType);
	d_bind_int(":Inactive", 0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK){
		bkg_meas->WellBackgroundID = sqlite3_last_insert_rowid(capDB);
		bkg_meas->CreatedOn = spec_meas->stamp;
	}
	d_finalize();

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_WriteDetectorStandard(STAND *detectorStandard){
	char snfixup[20];
	*capStatus = SQLITE_OK;
	d_command("BEGIN;");
	d_prepare_v2("UPDATE Config SET SettingValue = :DetectorTestSourceSN WHERE SettingName = 'DetectorTestSourceSN';");
	strcpy(snfixup, "$FTXT$");
	strcat(snfixup, detectorStandard->sn);
	d_bind_text(":DetectorTestSourceSN", snfixup);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :DetectorTestSourceActivity WHERE SettingName = 'DetectorTestSourceActivity';");
	d_bind_double(":DetectorTestSourceActivity", detectorStandard->act);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :DetectorTestSourceDate WHERE SettingName = 'DetectorTestSourceDate';");
	d_bind_int(":DetectorTestSourceDate", (int) detectorStandard->caldate);
	d_step();
	d_finalize();

	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}
void DB_PopBackground(BACKGND *backgnd, int detectorType){
	bool flgFound;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT WellBackground_SpectraID, DetectorType, datetime(CreatedOn), Inactive, WellBackgroundID FROM WellBackground WHERE Inactive = 0 AND DetectorType = :DetectorType;");
	d_bind_int(":DetectorType", detectorType);
	d_step();

	if(*capStatus == SQLITE_ROW){
		backgnd->Spectrum.SpectraID = d_column_int64(0);
		backgnd->Detector = d_column_int(1);
		backgnd->CreatedOn = strtotime_t((const char *) d_column_text(2));
		if(d_column_int(3) == 0) backgnd->Inactive = FALSE;
		else backgnd->Inactive = TRUE;
		backgnd->WellBackgroundID = d_column_int64(4);

		if(backgnd->Spectrum.SpectraID == 0) flgFound = FALSE;
		else flgFound = TRUE;
	}else{
		flgFound = FALSE;
	}
	d_finalize();

	if(flgFound){
		DB_ReadSpectrum(&(backgnd->Spectrum));
		if(backgnd->Spectrum.SpectraID == 0) flgFound = FALSE;
	}

	if(flgFound && (backgnd->Spectrum.AutoCal.AutoCalID != Mca_getAutoCalID(detectorType))){
		d_prepare_v2("UPDATE WellBackground SET Inactive = 1 WHERE DetectorType = :DetectorType;");
		d_bind_int(":DetectorType", detectorType);
		d_step();
		d_finalize();
		flgFound = FALSE;
	}

	if(!flgFound){
		MCA_clearBackgroundMirror(backgnd);
	}
}

void DB_RetrieveBackgnd(BACKGND *backgnd){
	backgnd->Spectrum.SpectraID = 0;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT WellBackground_SpectraID, DetectorType, datetime(CreatedOn), Inactive FROM WellBackground WHERE WellBackgroundID = :WellBackgroundID;");
	d_bind_int64(":WellBackgroundID", backgnd->WellBackgroundID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		backgnd->Spectrum.SpectraID = d_column_int64(0);
		backgnd->Detector = d_column_int(1);
		backgnd->CreatedOn = strtotime_t((const char *) d_column_text(2));
		if(d_column_int(3) == 0) backgnd->Inactive = FALSE;
		else backgnd->Inactive = TRUE;
	}
	d_finalize();

	if(backgnd->Spectrum.SpectraID > 0){
		DB_ReadSpectrum(&(backgnd->Spectrum));
	}
}

bool DB_ExistsAutoCal(int detectorType){
	bool returnvalue;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT * FROM AutoCal WHERE DetectorType = :DetectorType;");
	d_bind_int(":DetectorType", detectorType);
	d_step();
	if(*capStatus == SQLITE_ROW) returnvalue = TRUE;
	else returnvalue = FALSE;
	d_finalize();

	return returnvalue;
}

bool DB_ExistsBackground(int detectorType){
	bool returnvalue;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT * FROM WellBackground WHERE DetectorType = :DetectorType;");
	d_bind_int(":DetectorType", detectorType);
	d_step();
	if(*capStatus == SQLITE_ROW) returnvalue = TRUE;
	else returnvalue = FALSE;
	d_finalize();

	return returnvalue;
}

void DB_CopyWipeType(WELLWIPETYPE *to, WELLWIPETYPE *from){
	memcpy(to, from, sizeof(WELLWIPETYPE));
}
void DB_CreateWellWipeType(char *Name, float Threshold, int CountTime){
	char WipeName[30];
	bool flgFound;
	long long int rowid;

	strcpy(WipeName, Name);
	trim(WipeName);
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT * FROM WellWipeType WHERE Name=:Name AND Inactive = 0;");
	d_bind_text(":Name", WipeName);
	d_step();
	if (*capStatus == SQLITE_ROW) flgFound = TRUE;
	else flgFound = FALSE;
	d_finalize();

	if(!flgFound){
		*capStatus = SQLITE_OK;
		d_prepare_v2("INSERT INTO WellWipeType(WellWipeTypeGroupID, Name, Threshold, CountTime, Inactive) VALUES(:WellWipeTypeGroupID, :Name, :Threshold, :CountTime, :Inactive);");
		d_bind_int64(":WellWipeTypeGroupID", 0);
		d_bind_text(":Name", WipeName);
		d_bind_double(":Threshold", Threshold);
		d_bind_int(":CountTime", CountTime);
		d_bind_int(":Inactive", 0);
		d_step();
		if(*capStatus == SQLITE_DONE) rowid = sqlite3_last_insert_rowid(capDB);
		else rowid = -1;
		d_finalize();

		if(rowid != -1){
			*capStatus = SQLITE_OK;
			d_prepare_v2("UPDATE WellWipeType SET WellWipeTypeGroupID = WellWipeTypeID WHERE WellWipeTypeID = :WellWipeTypeID;");
			d_bind_int64(":WellWipeTypeID", rowid);
			d_step();
			d_finalize();
		}
	}
}

void DB_InactivateAllWellWipeType(void){
	d_prepare_v2("UPDATE WellWipeType SET Inactive = 1;");
	d_step();
	d_finalize();
}

void DB_FixWellWipeType(void){
	d_command("BEGIN;");
	DB_InactivateAllWellWipeType();
	DB_CreateWellWipeType("Background", 3000, 60);
	d_prepare_v2("UPDATE WellWipeType SET WellWipeTypeGroupID = 1 WHERE Name = 'Background' AND Inactive = 0;");
	d_step();
	d_finalize();
	service_watchdog();
	DB_CreateWellWipeType("Work Area", 2000, 60);
	d_prepare_v2("UPDATE WellWipeType SET WellWipeTypeGroupID = 2 WHERE Name = 'Work Area' AND Inactive = 0;");
	d_step();
	d_finalize();
	service_watchdog();
	DB_CreateWellWipeType("Unrestricted Area", 200, 60);
	d_prepare_v2("UPDATE WellWipeType SET WellWipeTypeGroupID = 3 WHERE Name = 'Unrestricted Area' AND Inactive = 0;");
	d_step();
	d_finalize();
	service_watchdog();
	DB_CreateWellWipeType("Sealed Source", 11100, 60);
	d_prepare_v2("UPDATE WellWipeType SET WellWipeTypeGroupID = 4 WHERE Name = 'Sealed Source' AND Inactive = 0;");
	d_step();
	d_finalize();
	service_watchdog();
	DB_CreateWellWipeType("Package", 200, 60);
	d_prepare_v2("UPDATE WellWipeType SET WellWipeTypeGroupID = 5 WHERE Name = 'Package' AND Inactive = 0;");
	d_step();
	d_finalize();
	service_watchdog();
	d_command("COMMIT;");
}

void DB_UpdateWellWipeType(WELLWIPETYPE *input, bool bookEnd){
	int index, jndex, kndex;
	long long int rowid;
	int Nuclide[10];
	bool flgFound;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("UPDATE WellWipeType SET Inactive = 1 WHERE WellWipeTypeGroupID = :WellWipeTypeGroupID;");
	d_bind_int64(":WellWipeTypeGroupID", input->WellWipeTypeGroupID);
	d_step();
	d_finalize();

	d_prepare_v2("INSERT INTO WellWipeType(WellWipeTypeGroupID, Name, Threshold, CountTime, CreatedOn, Inactive) VALUES(:WellWipeTypeGroupID, :Name, :Threshold, :CountTime, :CreatedOn, :Inactive);");
	d_bind_int64(":WellWipeTypeGroupID", input->WellWipeTypeGroupID);
	d_bind_text(":Name", input->Name);
	d_bind_double(":Threshold", input->Threshold);
	d_bind_int(":CountTime", input->CountTime);
	d_bind_double(":CreatedOn", input->CreatedOnValue);
	d_bind_int(":Inactive", 0);
	d_step();
	if(*capStatus == SQLITE_DONE) rowid = sqlite3_last_insert_rowid(capDB);
	else rowid = -1;
	d_finalize();

	if(rowid != -1){
		if((input->NuclideID[0] >= 0) || (input->NuclideID[1] >= 0) || (input->NuclideID[2] >= 0) || (input->NuclideID[3] >= 0) || (input->NuclideID[4] >= 0) || (input->NuclideID[5] >= 0) || (input->NuclideID[6] >= 0) || (input->NuclideID[7] >= 0) || (input->NuclideID[8] >= 0) || (input->NuclideID[9] >= 0)){
			for(index=0;index<10;index++) Nuclide[index] = -1;

			kndex = 0;
			for(index=0; index<10; index++){
				if(input->NuclideID[index] >= 0){
					flgFound = FALSE;
					for(jndex=0; jndex<10; jndex++){
						if(input->NuclideID[index] == Nuclide[jndex]){
							flgFound = TRUE;
							break;
						}
					}

					if(!flgFound){
						Nuclide[kndex] = input->NuclideID[index];
						kndex++;
					}
				}
			}


			for(index=0; index<10; index++){
				if(Nuclide[index] >= 0){
					d_prepare_v2("INSERT INTO WellWipeTypeNuclide(WellWipeTypeID, NuclideID) VALUES(:WellWipeTypeID, :NuclideID);");
					d_bind_int64(":WellWipeTypeID", rowid);
					d_bind_int(":NuclideID", Nuclide[index]);
					d_step();
					d_finalize();
				}

				input->NuclideID[index] = Nuclide[index];
			}
		}
		input->WellWipeTypeID = rowid;
		d_prepare_v2("SELECT datetime(LastUpdated) FROM WellWipeType WHERE WellWipeTypeID = :WellWipeTypeID;");
		d_bind_int64(":WellWipeTypeID", rowid);
		d_step();
		if(*capStatus == SQLITE_ROW){
			input->LastUpdated = strtotime_t((const char *) d_column_text(0));
		}
		d_finalize();
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK){
			d_command("COMMIT;");
		}else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
			input->WellWipeTypeID = 0;
		}
	}
}

void DB_readCurrentWellWipeType(WELLWIPETYPE *output){
	int index;

	output->WellWipeTypeID = 0;
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT WellWipeTypeID, WellWipeTypeGroupID, Name, Threshold, CountTime, CreatedOn, datetime(CreatedOn), datetime(LastUpdated) FROM WellWipeType WHERE Name = :Name AND Inactive = 0;");
	d_bind_text(":Name", output->Name);
	d_step();
	if(*capStatus == SQLITE_ROW){
		output->WellWipeTypeID = d_column_int64(0);
		output->WellWipeTypeGroupID = d_column_int64(1);
		strcpy(output->Name, (const char *) d_column_text(2));
		output->Threshold = d_column_double(3);
		output->CountTime = d_column_int(4);
		output->CreatedOnValue = d_column_double(5);
		output->CreatedOn = strtotime_t((const char *) d_column_text(6));
		output->LastUpdated = strtotime_t((const char *) d_column_text(7));
		for(index=0; index<10; index++) output->NuclideID[index] = -1;
	}
	d_finalize();

	if(output->WellWipeTypeID > 0){
		*capStatus = SQLITE_OK;
		d_prepare_v2("SELECT NuclideID FROM WellWipeTypeNuclide WHERE WellWipeTypeID = :WellWipeTypeID;");
		d_bind_int64(":WellWipeTypeID", output->WellWipeTypeID);
		d_step();
		index = 0;
		while(*capStatus == SQLITE_ROW){
			if(index<10){
				output->NuclideID[index] = d_column_int(0);
				index++;
			}
			d_step();
		}
		d_finalize();
	}
}

void DB_CreateWellWipeLocation(WELLWIPELOCATION *createItem, bool bookEnd){
	long long int rowid;
	int index, jndex, kndex;
	bool flgFound;
	int Nuclide[10];

	trim(createItem->Name);

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("SELECT * FROM WellWipeLocation WHERE Name=:Name AND Inactive = 0;");
	d_bind_text(":Name", createItem->Name);
	d_step();
	if(*capStatus == SQLITE_ROW) flgFound = TRUE;
	else flgFound = FALSE;
	d_finalize();

	if(flgFound == FALSE){
		d_prepare_v2("SELECT WellWipeTypeID, Name FROM WellWipeType WHERE Inactive = 0 AND WellWipeTypeGroupID = :WellWipeTypeGroupID;");
		d_bind_int64(":WellWipeTypeGroupID", createItem->WellWipeTypeGroupID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			createItem->WellWipeTypeID = d_column_int64(0);
			strcpy(createItem->WellWipeTypeName, (const char *) d_column_text(1));
			flgFound = TRUE;
		}else{
			flgFound = FALSE;
		}
		d_finalize();

		if(flgFound){
			d_prepare_v2("INSERT INTO WellWipeLocation(WellWipeLocationGroupID, Name, WellWipeTypeID, Threshold, CountTime, Inactive) VALUES(:WellWipeLocationGroupID, :Name, :WellWipeTypeID, :Threshold, :CountTime, :Inactive);");
			d_bind_int64(":WellWipeLocationGroupID", 0);
			d_bind_text(":Name", createItem->Name);
			d_bind_int64(":WellWipeTypeID", createItem->WellWipeTypeID);
			d_bind_double(":Threshold", createItem->Threshold);
			d_bind_int(":CountTime", createItem->CountTime);
			d_bind_int(":Inactive", 0);
			d_step();
			if(*capStatus == SQLITE_DONE) rowid = sqlite3_last_insert_rowid(capDB);
			else rowid = 0;
			d_finalize();

			if(rowid){
				d_prepare_v2("UPDATE WellWipeLocation SET WellWipeLocationGroupID = WellWipeLocationID WHERE WellWipeLocationID = :WellWipeLocationID;");
				d_bind_int64(":WellWipeLocationID", rowid);
				d_step();
				d_finalize();

				d_prepare_v2("SELECT CreatedOn, datetime(CreatedOn), datetime(LastUpdated) FROM WellWipeLocation WHERE WellWipeLocationID = :WellWipeLocationID;");
				d_bind_int64(":WellWipeLocationID", rowid);
				d_step();
				if(*capStatus == SQLITE_ROW){
					createItem->CreatedOnValue = d_column_double(0);
					createItem->CreatedOn = strtotime_t((const char *) d_column_text(1));
					createItem->LastUpdated = strtotime_t((const char *) d_column_text(2));
					flgFound = TRUE;
				}else{
					flgFound = FALSE;
				}
				d_finalize();

				if(flgFound){
					if((createItem->NuclideID[0] >= 0) || (createItem->NuclideID[1] >= 0) || (createItem->NuclideID[2] >= 0) || (createItem->NuclideID[3] >= 0) || (createItem->NuclideID[4] >= 0) || (createItem->NuclideID[5] >= 0) || (createItem->NuclideID[6] >= 0) || (createItem->NuclideID[7] >= 0) || (createItem->NuclideID[8] >= 0) || (createItem->NuclideID[9] >= 0)){
						for(index=0; index<10; index++) Nuclide[index] = -1;
						kndex = 0;
						for(index=0; index<10; index++){
							if(createItem->NuclideID[index] >= 0){
								flgFound = FALSE;
								for(jndex=0; jndex<10; jndex++){
									if(createItem->NuclideID[index] == Nuclide[jndex]){
										flgFound = TRUE;
										break;
									}
								}

								if(!flgFound){
									Nuclide[kndex] = createItem->NuclideID[index];
									kndex++;
								}
							}
						}

						d_prepare_v2("INSERT INTO WellWipeLocationNuclide(WellWipeLocationID, NuclideID) VALUES(:WellWipeLocationID, :NuclideID);");
						for(index=0; index<10; index++){
							if(Nuclide[index] >= 0){
								d_bind_int64(":WellWipeLocationID", rowid);
								d_bind_int(":NuclideID", Nuclide[index]);
								d_step();
								d_reset();
							}
							createItem->NuclideID[index] = Nuclide[index];
						}
						d_finalize();
					}

					createItem->WellWipeLocationGroupID = rowid;
					createItem->WellWipeLocationID = rowid;
				}
			}
		}
	}

	if(*capStatus != SQLITE_OK){
		createItem->WellWipeLocationID = 0;
		createItem->WellWipeLocationGroupID = 0;
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_CopyWipeLocation(WELLWIPELOCATION *to, WELLWIPELOCATION *from){
	memcpy(to, from, sizeof(WELLWIPELOCATION));
}

void DB_ReadCurrentWipeLocations(int *LocationCount, int *LocationMax, WELLWIPELOCATION **activeWipeLocation, WELLWIPELOCATION **availableWipeLocation, WELLWIPELOCATION **selectedWipeLocation){
	WELLWIPELOCATION *currentWipeLocation;
	int index, jndex, reqMax;

	*capStatus = SQLITE_OK;

	if(d_query("SELECT Count(*) FROM WellWipeLocation WHERE Inactive = 0;")){
		if(*capStatus == SQLITE_ROW){
			*LocationCount = d_column_int(0);
		}else{
			*LocationCount = 0;
		}
	}else{
		*LocationCount = 0;
	}
	d_finalize();

	if(*LocationCount > 0){
		reqMax = *LocationCount / 50;
		reqMax++;
		reqMax *= 50;

		if(reqMax > *LocationMax){
			if(*LocationMax > 0){
				free(*activeWipeLocation);
				free(*availableWipeLocation);
				free(*selectedWipeLocation);
			}
			currentWipeLocation = malloc(reqMax * sizeof(WELLWIPELOCATION));
			*activeWipeLocation = currentWipeLocation;
			*availableWipeLocation = malloc(reqMax * sizeof(WELLWIPELOCATION));
			*selectedWipeLocation = malloc(reqMax * sizeof(WELLWIPELOCATION));
			*LocationMax = reqMax;
		}else{
			currentWipeLocation = *activeWipeLocation;
		}

		d_prepare_v2("SELECT a.WellWipeLocationID, a.WellWipeLocationGroupID, a.Name, a.WellWipeTypeID, b.WellWipeTypeGroupID, b.Name, a.Threshold, a.CountTime, a.CreatedOn, datetime(a.CreatedOn), datetime(a.LastUpdated) FROM WellWipeLocation AS a JOIN WellWipeType AS b ON a.WellWipeTypeID = b.WellWipeTypeID WHERE a.Inactive = 0 ORDER BY a.WellWipeLocationGroupID;");
		d_step();
		while(*capStatus == SQLITE_ROW){
			currentWipeLocation->WellWipeLocationID = d_column_int64(0);
			currentWipeLocation->WellWipeLocationGroupID = d_column_int64(1);
			strcpy(currentWipeLocation->Name, (const char *) d_column_text(2));
			currentWipeLocation->WellWipeTypeID = d_column_int64(3);
			currentWipeLocation->WellWipeTypeGroupID = d_column_int64(4);
			strcpy(currentWipeLocation->WellWipeTypeName, (const char *) d_column_text(5));
			currentWipeLocation->Threshold = d_column_double(6);
			currentWipeLocation->CountTime = d_column_int(7);
			currentWipeLocation->CreatedOnValue = d_column_double(8);
			currentWipeLocation->CreatedOn = strtotime_t((const char *) d_column_text(9));
			currentWipeLocation->LastUpdated = strtotime_t((const char *) d_column_text(10));
			currentWipeLocation++;
			d_step();
		}
		d_finalize();

		currentWipeLocation = *activeWipeLocation;
		d_prepare_v2("SELECT NuclideID FROM WellWipeLocationNuclide WHERE WellWipeLocationID = :WellWipeLocationID;");
		for(index=0; index<(*LocationCount); index++){
			for(jndex=0; jndex<10; jndex++) currentWipeLocation->NuclideID[jndex] = -1;
			d_bind_int64(":WellWipeLocationID", currentWipeLocation->WellWipeLocationID);
			d_step();
			jndex = 0;
			while(*capStatus == SQLITE_ROW){
				if(jndex<10){
					currentWipeLocation->NuclideID[jndex] = d_column_int(0);
					jndex++;
				}
				d_step();
			}
			d_reset();
			currentWipeLocation++;
		}
		d_finalize();
	}
}

void DB_DeleteCurrentWipeLocation(WELLWIPELOCATION *delItem, bool bookEnd){
	long long int rowid;
	int index;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("UPDATE WellWipeLocation SET Inactive = 1 WHERE WellWipeLocationGroupID = :WellWipeLocationGroupID;");
	d_bind_int64(":WellWipeLocationGroupID", delItem->WellWipeLocationGroupID);
	d_step();
	d_finalize();

	d_prepare_v2("INSERT INTO WellWipeLocation(WellWipeLocationGroupID, Name, WellWipeTypeID, Threshold, CountTime, CreatedOn, Inactive) VALUES(:WellWipeLocationGroupID, :Name, :WellWipeTypeID, :Threshold, :CountTime, :CreatedOn, :Inactive);");
	d_bind_int64(":WellWipeLocationGroupID", delItem->WellWipeLocationGroupID);
	d_bind_text(":Name", delItem->Name);
	d_bind_int64(":WellWipeTypeID", delItem->WellWipeTypeID);
	d_bind_double(":Threshold", delItem->Threshold);
	d_bind_int(":CountTime", delItem->CountTime);
	d_bind_double(":CreatedOn", delItem->CreatedOnValue);
	d_bind_int(":Inactive", 1);
	d_step();
	if(*capStatus == SQLITE_DONE) rowid = sqlite3_last_insert_rowid(capDB);
	else rowid = -1;
	d_finalize();

	if(rowid != -1){
		for(index=0; index<10; index++){
			if(delItem->NuclideID[index] >= 0){
				d_prepare_v2("INSERT INTO WellWipeLocationNuclide(WellWipeLocationID, NuclideID) VALUES(:WellWipeLocationID, :NuclideID);");
				d_bind_int64(":WellWipeLocationID", rowid);
				d_bind_int(":NuclideID", delItem->NuclideID[index]);
				d_step();
				d_finalize();
			}
		}
	}

	if(*capStatus != SQLITE_OK) delItem->WellWipeLocationID = 0;

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_UpdateWellWipeLocation(WELLWIPELOCATION *updateLocation, bool bookEnd){
	bool flgFound;
	long long int rowid;
	int Nuclide[10];
	int index, jndex, kndex;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("SELECT WellWipeTypeID, Name FROM WellWipeType WHERE Inactive = 0 AND WellWipeTypeGroupID = :WellWipeTypeGroupID;");
	d_bind_int64(":WellWipeTypeGroupID", updateLocation->WellWipeTypeGroupID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		updateLocation->WellWipeTypeID = d_column_int64(0);
		strcpy(updateLocation->WellWipeTypeName, (const char *) d_column_text(1));
		flgFound = TRUE;
	}else{
		flgFound = FALSE;
	}
	d_finalize();

	if(flgFound){
		d_prepare_v2("UPDATE WellWipeLocation SET Inactive = 1 WHERE WellWipeLocationGroupID = :WellWipeLocationGroupID;");
		d_bind_int64(":WellWipeLocationGroupID", updateLocation->WellWipeLocationGroupID);
		d_step();
		d_finalize();

		d_prepare_v2("INSERT INTO WellWipeLocation(WellWipeLocationGroupID, Name, WellWipeTypeID, Threshold, CountTime, CreatedOn, Inactive) VALUES(:WellWipeLocationGroupID, :Name, :WellWipeTypeID, :Threshold, :CountTime, :CreatedOn, :Inactive);");
		d_bind_int64(":WellWipeLocationGroupID", updateLocation->WellWipeLocationGroupID);
		d_bind_text(":Name", updateLocation->Name);
		d_bind_int64(":WellWipeTypeID", updateLocation->WellWipeTypeID);
		d_bind_double(":Threshold", updateLocation->Threshold);
		d_bind_int(":CountTime", updateLocation->CountTime);
		d_bind_double(":CreatedOn", updateLocation->CreatedOnValue);
		d_bind_int(":Inactive", 0);
		d_step();
		if(*capStatus == SQLITE_DONE) rowid = sqlite3_last_insert_rowid(capDB);
		else rowid = -1;
		d_finalize();

		if(rowid != -1){
			if((updateLocation->NuclideID[0] >= 0) || (updateLocation->NuclideID[1] >= 0) || (updateLocation->NuclideID[2] >= 0) || (updateLocation->NuclideID[3] >= 0) || (updateLocation->NuclideID[4] >= 0) || (updateLocation->NuclideID[5] >= 0) || (updateLocation->NuclideID[6] >= 0) || (updateLocation->NuclideID[7] >= 0) || (updateLocation->NuclideID[8] >= 0) || (updateLocation->NuclideID[9] >= 0)){
				for(index=0; index<10; index++) Nuclide[index] = -1;
				kndex = 0;
				for(index=0; index<10; index++){
					if(updateLocation->NuclideID[index] >= 0){
						flgFound = FALSE;
						for(jndex=0; jndex<10; jndex++){
							if(updateLocation->NuclideID[index] == Nuclide[jndex]){
								flgFound = TRUE;
								break;
							}
						}

						if(!flgFound){
							Nuclide[kndex] = updateLocation->NuclideID[index];
							kndex++;
						}
					}
				}

				d_prepare_v2("INSERT INTO WellWipeLocationNuclide(WellWipeLocationID, NuclideID) VALUES(:WellWipeLocationID, :NuclideID);");
				for(index=0; index<10; index++){
					if(Nuclide[index] >= 0){
						d_bind_int64(":WellWipeLocationID", rowid);
						d_bind_int(":NuclideID", Nuclide[index]);
						d_step();
						d_reset();
					}
					updateLocation->NuclideID[index] = Nuclide[index];
				}
				d_finalize();
			}

			d_prepare_v2("SELECT datetime(LastUpdated) FROM WellWipeLocation WHERE WellWipeLocationID = :WellWipeLocationID;");
			d_bind_int64(":WellWipeLocationID", rowid);
			d_step();
			if(*capStatus == SQLITE_ROW){
				updateLocation->LastUpdated = strtotime_t((const char *) d_column_text(0));
				updateLocation->WellWipeLocationID = rowid;
			}else{
				updateLocation->WellWipeLocationID = 0;
			}
			d_finalize();
		}else{
			updateLocation->WellWipeLocationID = 0;
		}
	}else{
		updateLocation->WellWipeLocationID = 0;
	}

	if(*capStatus != SQLITE_OK) updateLocation->WellWipeLocationID = 0;

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_ReadCurrentWipeNuclides(int *WipeNuclideCount, int *WipeNuclideMax, WELLWIPENUCLIDE **activeWipeNuclide){
	WELLWIPENUCLIDE *currentWipeNuclide;
	int reqMax;

	*capStatus = SQLITE_OK;
	if(d_query("SELECT Count(*) FROM WellWipeNuclide WHERE Inactive = 0;")){
		if(*capStatus == SQLITE_ROW){
			*WipeNuclideCount = d_column_int(0);
		}else{
			*WipeNuclideCount = 0;
		}
	}else{
		*WipeNuclideCount = 0;
	}
	d_finalize();

	if(*WipeNuclideCount > 0){
		reqMax = *WipeNuclideCount / 50;
		reqMax++;
		reqMax *= 50;

		if(reqMax > *WipeNuclideMax){
			if(*WipeNuclideMax > 0){
				free(*activeWipeNuclide);
			}
			currentWipeNuclide = malloc(reqMax * sizeof(WELLWIPENUCLIDE));
			*activeWipeNuclide = currentWipeNuclide;
			*WipeNuclideMax = reqMax;
		}else{
			currentWipeNuclide = *activeWipeNuclide;
		}

		d_prepare_v2("SELECT WellWipeNuclideID, WellWipeNuclideGroupID, NuclideID, Name, PrimaryEnergy, SecondaryEnergy, TertiaryEnergy, Efficiency, CreatedOn, datetime(CreatedOn), datetime(LastUpdated) FROM WellWipeNuclide WHERE Inactive = 0;");
		d_step();
		while(*capStatus == SQLITE_ROW){
			currentWipeNuclide->WellWipeNuclideID = d_column_int64(0);
			currentWipeNuclide->WellWipeNuclideGroupID = d_column_int64(1);
			currentWipeNuclide->NuclideID = d_column_int(2);
			strcpy(currentWipeNuclide->Name, (const char *) d_column_text(3));
			currentWipeNuclide->PrimaryEnergy = d_column_double(4);
			currentWipeNuclide->SecondaryEnergy = d_column_double(5);
			currentWipeNuclide->TertiaryEnergy = d_column_double(6);
			currentWipeNuclide->Efficiency = d_column_double(7);
			currentWipeNuclide->CreatedOnValue = d_column_double(8);
			currentWipeNuclide->CreatedOn = strtotime_t((const char *) d_column_text(9));
			currentWipeNuclide->LastUpdated = strtotime_t((const char *) d_column_text(10));
			currentWipeNuclide++;
			d_step();
		}
		d_finalize();
	}
}

void DB_RefreshCurrentWipeNuclides(int *WipeNuclideCount, int *WipeNuclideMax, WELLWIPENUCLIDE **activeWipeNuclide, int NuclideID){
	bool flgFound;
	int index, reqMax;
	WELLWIPENUCLIDE *currentWipeNuclide, *originalWipeNuclide;
	char Name[31];
	float Primary, Secondary, Tertiary, Efficiency;
	long long int rowid;

	if(NuclideID >= 0){
		NuclideData_getName(NuclideID, Name);
		trim(Name);
		Primary = NuclideData_getEffectivePrimary(NuclideID);
		Secondary = NuclideData_getEffectiveSecondary(NuclideID);
		Tertiary = NuclideData_getEffectiveTertiary(NuclideID);
		Efficiency = NuclideData_getEffectiveEffSkewWell(NuclideID, Mca_installedDetector);

		reqMax = (*WipeNuclideCount) + 1;
		reqMax /= 50;
		reqMax++;
		reqMax *= 50;
		if(reqMax > *WipeNuclideMax){
			currentWipeNuclide = malloc(reqMax * sizeof(WELLWIPENUCLIDE));
			originalWipeNuclide = *activeWipeNuclide;
			for(index=0; index<*WipeNuclideCount; index++){
				memcpy(&(currentWipeNuclide[index]), &(originalWipeNuclide[index]), sizeof(WELLWIPENUCLIDE));
			}
			if(*WipeNuclideMax>0){
				free(*activeWipeNuclide);
			}
			*activeWipeNuclide = currentWipeNuclide;
			*WipeNuclideMax = reqMax;
		}else{
			currentWipeNuclide = *activeWipeNuclide;
		}

		flgFound = FALSE;
		if(*WipeNuclideCount > 0){
			for(index=0; index<*WipeNuclideCount; index++){
				if(currentWipeNuclide[index].NuclideID == NuclideID){
					// compare and update
					if((strcmp(currentWipeNuclide[index].Name, Name) != 0) || (currentWipeNuclide[index].PrimaryEnergy != Primary) || (currentWipeNuclide[index].SecondaryEnergy != Secondary) || (currentWipeNuclide[index].TertiaryEnergy != Tertiary) || (currentWipeNuclide[index].Efficiency != Efficiency)){
						*capStatus = SQLITE_OK;
						d_command("BEGIN;");
						d_prepare_v2("UPDATE WellWipeNuclide SET Inactive = 1 WHERE WellWipeNuclideGroupID = :WellWipeNuclideGroupID;");
						d_bind_int64(":WellWipeNuclideGroupID", currentWipeNuclide[index].WellWipeNuclideGroupID);
						d_step();
						d_finalize();

						d_prepare_v2("INSERT INTO WellWipeNuclide(WellWipeNuclideGroupID, NuclideID, Name, PrimaryEnergy, SecondaryEnergy, TertiaryEnergy, Efficiency, CreatedOn, Inactive) VALUES(:WellWipeNuclideGroupID, :NuclideID, :Name, :PrimaryEnergy, :SecondaryEnergy, :TertiaryEnergy, :Efficiency, :CreatedOn, :Inactive);");
						d_bind_int64(":WellWipeNuclideGroupID", currentWipeNuclide[index].WellWipeNuclideGroupID);
						d_bind_int(":NuclideID", currentWipeNuclide[index].NuclideID);
						d_bind_text(":Name", Name);
						d_bind_double(":PrimaryEnergy", Primary);
						d_bind_double(":SecondaryEnergy", Secondary);
						d_bind_double(":TertiaryEnergy", Tertiary);
						d_bind_double(":Efficiency", Efficiency);
						d_bind_double(":CreatedOn", currentWipeNuclide[index].CreatedOnValue);
						d_bind_int(":Inactive", 0);
						d_step();

						if(*capStatus == SQLITE_DONE) rowid = sqlite3_last_insert_rowid(capDB);
						else rowid = -1;
						d_finalize();

						if(rowid != -1){
							d_prepare_v2("SELECT datetime(LastUpdated) FROM WellWipeNuclide WHERE WellWipeNuclideID = :WellWipeNuclideID;");
							d_bind_int64(":WellWipeNuclideID", rowid);
							d_step();
							if(*capStatus == SQLITE_ROW){
								currentWipeNuclide[index].LastUpdated = strtotime_t((const char *) d_column_text(0));
								currentWipeNuclide[index].WellWipeNuclideID = rowid;
								strcpy(currentWipeNuclide[index].Name, Name);
								currentWipeNuclide[index].PrimaryEnergy = Primary;
								currentWipeNuclide[index].SecondaryEnergy = Secondary;
								currentWipeNuclide[index].TertiaryEnergy = Tertiary;
								currentWipeNuclide[index].Efficiency = Efficiency;
							}
							d_finalize();
						}

						if(*capStatus == SQLITE_OK) d_command("COMMIT;");
						else{
							*capStatus = SQLITE_OK;
							d_command("ROLLBACK;");
						}
					}

					flgFound = TRUE;
					break;
				}
			}
		}

		if(!flgFound){
			*capStatus = SQLITE_OK;
			d_command("BEGIN;");
			d_prepare_v2("INSERT INTO WellWipeNuclide(WellWipeNuclideGroupID, NuclideID, Name, PrimaryEnergy, SecondaryEnergy, TertiaryEnergy, Efficiency, Inactive) VALUES(:WellWipeNuclideGroupID, :NuclideID, :Name, :PrimaryEnergy, :SecondaryEnergy, :TertiaryEnergy, :Efficiency, :Inactive);");
			d_bind_int64(":WellWipeNuclideGroupID", 0);
			d_bind_int(":NuclideID", NuclideID);
			d_bind_text(":Name", Name);
			d_bind_double(":PrimaryEnergy", Primary);
			d_bind_double(":SecondaryEnergy", Secondary);
			d_bind_double(":TertiaryEnergy", Tertiary);
			d_bind_double(":Efficiency", Efficiency);
			d_bind_int(":Inactive", 0);
			d_step();
			if(*capStatus == SQLITE_DONE) rowid = sqlite3_last_insert_rowid(capDB);
			else rowid = -1;
			d_finalize();

			if(rowid != -1){
				d_prepare_v2("UPDATE WellWipeNuclide SET WellWipeNuclideGroupID = :WellWipeNuclideGroupID WHERE WellWipeNuclideID = :WellWipeNuclideID;");
				d_bind_int64(":WellWipeNuclideGroupID", rowid);
				d_bind_int64(":WellWipeNuclideID", rowid);
				d_step();
				d_finalize();

				d_prepare_v2("SELECT CreatedOn, datetime(CreatedOn), datetime(LastUpdated) FROM WellWipeNuclide WHERE WellWipeNuclideID = :WellWipeNuclideID;");
				d_bind_int64(":WellWipeNuclideID", rowid);
				d_step();
				if(*capStatus == SQLITE_ROW){
					currentWipeNuclide[*WipeNuclideCount].CreatedOnValue = d_column_double(0);
					currentWipeNuclide[*WipeNuclideCount].CreatedOn = strtotime_t((const char *) d_column_text(1));
					currentWipeNuclide[*WipeNuclideCount].LastUpdated = strtotime_t((const char *) d_column_text(2));
					currentWipeNuclide[*WipeNuclideCount].WellWipeNuclideID = rowid;
					currentWipeNuclide[*WipeNuclideCount].WellWipeNuclideGroupID = rowid;
					currentWipeNuclide[*WipeNuclideCount].NuclideID = NuclideID;
					strcpy(currentWipeNuclide[*WipeNuclideCount].Name, Name);
					currentWipeNuclide[*WipeNuclideCount].PrimaryEnergy = Primary;
					currentWipeNuclide[*WipeNuclideCount].SecondaryEnergy = Secondary;
					currentWipeNuclide[*WipeNuclideCount].TertiaryEnergy = Tertiary;
					currentWipeNuclide[*WipeNuclideCount].Efficiency = Efficiency;
					(*WipeNuclideCount)++;
				}
				d_finalize();
			}

			if(*capStatus == SQLITE_OK) d_command("COMMIT;");
			else{
				*capStatus = SQLITE_OK;
				d_command("ROLLBACK;");
			}
		}
	}
}

void DB_CreateWellWipe(WELLWIPE *wellWipe, bool bookEnd){
	long long int SpectraID, rowid;
	int index;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	// Save Spectra
	SpectraID = DB_WriteSpectrum(&(wellWipe->Spectrum), FALSE);

	if(SpectraID > 0){
		// Save WellWipe
		d_prepare_v2("INSERT INTO WellWipe(WellWipeLocationID, NetCPM, Efficiency, Activity, ExceedsThreshold, OverallHighActivity, WellWipe_SpectraID, WellBackgroundID, Comment, InactiveReason, Inactive) VALUES(:WellWipeLocationID, :NetCPM, :Efficiency, :Activity, :ExceedsThreshold, :OverallHighActivity, :WellWipe_SpectraID, :WellBackgroundID, :Comment, :InactiveReason, :Inactive);");
		d_bind_int64(":WellWipeLocationID", (wellWipe->WipeLocation).WellWipeLocationID);
		d_bind_double(":NetCPM", wellWipe->NetCPM);
		d_bind_double(":Efficiency", wellWipe->Efficiency);
		d_bind_double(":Activity", wellWipe->Activity);
		if(wellWipe->ExceedsThreshold) d_bind_int(":ExceedsThreshold", 1);
		else d_bind_int(":ExceedsThreshold", 0);
		if(wellWipe->OverallHighActivity) d_bind_int(":OverallHighActivity", 1);
		else d_bind_int(":OverallHighActivity", 0);
		d_bind_int64(":WellWipe_SpectraID", (wellWipe->Spectrum).SpectraID);
		d_bind_int64(":WellBackgroundID", (wellWipe->Background).WellBackgroundID);
		d_bind_text(":Comment", wellWipe->Comment);
		d_bind_text(":InactiveReason", wellWipe->InactiveReason);
		d_bind_int(":Inactive", 0);
		d_step();

		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK){
			rowid = sqlite3_last_insert_rowid(capDB);
			//wellWipe->WellWipeID = rowid;
		}else{
			rowid = -1;
		}
		d_finalize();

		if(rowid != -1){
			d_prepare_v2("SELECT datetime(CreatedOn) FROM WellWipe WHERE WellWipeID = :WellWipeID;");
			d_bind_int64(":WellWipeID", rowid);
			d_step();
			if(*capStatus == SQLITE_ROW){
				wellWipe->WellWipeID = rowid;
				wellWipe->CreatedOn = strtotime_t((const char *) d_column_text(0));
			}
			d_finalize();

			// Save WellWipeWellWipeNuclide
			for(index=0; index<10; index++){
				if((wellWipe->WellWipeNuclide[index]).WellWipeNuclideID > 0){
					d_prepare_v2("INSERT INTO WellWipeWellWipeNuclide(WellWipeID, WellWipeNuclideID) VALUES(:WellWipeID, :WellWipeNuclideID);");
					d_bind_int64(":WellWipeID", rowid);
					d_bind_int64(":WellWipeNuclideID", (wellWipe->WellWipeNuclide[index]).WellWipeNuclideID);
					d_step();
					d_finalize();
				}
			}

			// Save WellWipePeak
			for(index=0; index<40; index++){
				if((wellWipe->Peaks[index]).Energy >= 0.0){
					d_prepare_v2("INSERT INTO WellWipePeak(WellWipeID, Energy, StartROI, EndROI, ROICounts, ROICPM, BackgroundROICounts, BackgroundROICPM, NETROICPM, WellWipeNuclideID, Activity, ExceedsThreshold, PeakType) VALUES(:WellWipeID, :Energy, :StartROI, :EndROI, :ROICounts, :ROICPM, :BackgroundROICounts, :BackgroundROICPM, :NETROICPM, :WellWipeNuclideID, :Activity, :ExceedsThreshold, :PeakType);");
					d_bind_int64(":WellWipeID", rowid);
					d_bind_double(":Energy", (wellWipe->Peaks[index]).Energy);
					d_bind_double(":StartROI", (wellWipe->Peaks[index]).StartROI);
					d_bind_double(":EndROI", (wellWipe->Peaks[index]).EndROI);
					d_bind_int(":ROICounts", (wellWipe->Peaks[index]).ROICounts);
					d_bind_double(":ROICPM", (wellWipe->Peaks[index]).ROICPM);
					d_bind_int(":BackgroundROICounts", (wellWipe->Peaks[index]).BackgroundROICounts);
					d_bind_double(":BackgroundROICPM", (wellWipe->Peaks[index]).BackgroundROICPM);
					d_bind_double(":NETROICPM", (wellWipe->Peaks[index]).NetROICPM);
					if((wellWipe->Peaks[index]).WellWipeNuclideID > 0) d_bind_int64(":WellWipeNuclideID",(wellWipe->Peaks[index]).WellWipeNuclideID);
					else d_bind_null(":WellWipeNuclideID");
					d_bind_double(":Activity", (wellWipe->Peaks[index]).Activity);
					if((wellWipe->Peaks[index]).ExceedsThreshold) d_bind_int(":ExceedsThreshold", 1);
					else d_bind_int(":ExceedsThreshold", 0);
					d_bind_int(":PeakType", (wellWipe->Peaks[index]).PeakType);
					d_step();
					d_finalize();
				}else{
					break;
				}
			}
		}

		if(*capStatus != SQLITE_OK) wellWipe->WellWipeID = 0;

		if(bookEnd){
			if(*capStatus == SQLITE_OK) d_command("COMMIT;");
			else{
				*capStatus = SQLITE_OK;
				d_command("ROLLBACK;");
			}
		}
	}
}

int DB_SearchWipes(WELLWIPESEARCH *wellWipes, time_t startDate, time_t endDate, int wipeType, int maxRetrieved){
	int retrievedCount, index;
	time_t buffer;
	char datestring[40];
	WELLWIPESEARCH *wipePtr;

	wipePtr = wellWipes;
	for(index=0; index<maxRetrieved; index++){
		wipePtr->WellWipeID = 0;
		wipePtr++;
	}
	wipePtr = wellWipes;
	retrievedCount = 0;

	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	if(wipeType == 1){
		d_prepare_v2("SELECT a.WellWipeID, a.Inactive, datetime(b.MeasuredOn), c.Name, d.Name, a.OverallHighActivity FROM WellWipe AS a JOIN Spectra AS b ON a.WellWipe_SpectraID = b.SpectraID JOIN WellWipeLocation AS c ON a.WellWipeLocationID = c.WellWipeLocationID JOIN WellWipeType AS d ON c.WellWipeTypeID = d.WellWipeTypeID WHERE b.MeasuredOn >= julianday(:startDate) AND b.MeasuredOn < julianday(:endDate) ORDER BY b.MeasuredOn DESC;");
		stringout(datestring, &startDate);
		d_bind_text(":startDate", datestring);
		stringout(datestring, &endDate);
		d_bind_text(":endDate", datestring);
	}else if((wipeType >= 2) && (wipeType <= 5)){
		d_prepare_v2("SELECT a.WellWipeID, a.Inactive, datetime(b.MeasuredOn), c.Name, d.Name, a.OverallHighActivity FROM WellWipe AS a JOIN Spectra AS b ON a.WellWipe_SpectraID = b.SpectraID JOIN WellWipeLocation AS c ON a.WellWipeLocationID = c.WellWipeLocationID JOIN WellWipeType AS d ON c.WellWipeTypeID = d.WellWipeTypeID WHERE b.MeasuredOn >= julianday(:startDate) AND b.MeasuredOn < julianday(:endDate) AND d.WellWipeTypeGroupID = :wipeType ORDER BY b.MeasuredOn DESC;");
		stringout(datestring, &startDate);
		d_bind_text(":startDate", datestring);
		stringout(datestring, &endDate);
		d_bind_text(":endDate", datestring);
		d_bind_int(":wipeType", wipeType);
	}else if(wipeType == 6){
		d_prepare_v2("SELECT a.WellWipeID, a.Inactive, datetime(b.MeasuredOn), c.Name, d.Name, a.OverallHighActivity FROM WellWipe AS a JOIN Spectra AS b ON a.WellWipe_SpectraID = b.SpectraID JOIN WellWipeLocation AS c ON a.WellWipeLocationID = c.WellWipeLocationID JOIN WellWipeType AS d ON c.WellWipeTypeID = d.WellWipeTypeID WHERE b.MeasuredOn >= julianday(:startDate) AND b.MeasuredOn < julianday(:endDate) AND NOT(a.OverallHighActivity = 0) ORDER BY b.MeasuredOn DESC;");
		stringout(datestring, &startDate);
		d_bind_text(":startDate", datestring);
		stringout(datestring, &endDate);
		d_bind_text(":endDate", datestring);
	}
	d_step();
	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				wipePtr->WellWipeID = d_column_int64(0);
				if(d_column_int(1) == 0) wipePtr->Inactive = FALSE;
				else wipePtr->Inactive = TRUE;
				wipePtr->MeasuredOn = strtotime_t((const char *) d_column_text(2));
				strcpy(wipePtr->WellWipeLocationName, (const char *) d_column_text(3));
				strcpy(wipePtr->WellWipeTypeName, (const char *) d_column_text(4));
				if(d_column_int(5) == 0) wipePtr->OverallHighActivity = FALSE;
				else wipePtr->OverallHighActivity = TRUE;
				wipePtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}

void DB_RetrieveWipe(WELLWIPE *wellWipe){
	int index;

	wellWipe->WipeLocation.WellWipeLocationID = 0;
	wellWipe->Spectrum.SpectraID = 0;
	wellWipe->Background.WellBackgroundID = 0;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT WellWipeLocationID, NetCPM, Efficiency, Activity, ExceedsThreshold, OverallHighActivity, WellWipe_SpectraID, WellBackgroundID, Comment, datetime(CreatedOn), Inactive, InactiveReason FROM WellWipe WHERE WellWipeID = :WellWipeID;");
	d_bind_int64(":WellWipeID", wellWipe->WellWipeID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		wellWipe->WipeLocation.WellWipeLocationID = d_column_int64(0);
		wellWipe->NetCPM = d_column_double(1);
		wellWipe->Efficiency = d_column_double(2);
		wellWipe->Activity = d_column_double(3);
		if(d_column_int(4) == 0) wellWipe->ExceedsThreshold = FALSE;
		else wellWipe->ExceedsThreshold = TRUE;
		if(d_column_int(5) == 0) wellWipe->OverallHighActivity = FALSE;
		else wellWipe->OverallHighActivity = TRUE;
		wellWipe->Spectrum.SpectraID = d_column_int64(6);
		wellWipe->Background.WellBackgroundID = d_column_int64(7);
		strcpy(wellWipe->Comment, (const char *) d_column_text(8));
		wellWipe->CreatedOn = strtotime_t((const char *) d_column_text(9));
		if(d_column_int(10) == 0) wellWipe->Inactive = FALSE;
		else wellWipe->Inactive = TRUE;
		strcpy(wellWipe->InactiveReason, (const char *) d_column_text(11));
	}
	d_finalize();

	if(wellWipe->WipeLocation.WellWipeLocationID > 0){
		d_prepare_v2("SELECT a.WellWipeLocationGroupID, a.Name, a.Threshold, a.CountTime, a.CreatedOn, datetime(a.CreatedOn), datetime(a.LastUpdated), b.WellWipeTypeID, b.WellWipeTypeGroupID, b.Name FROM WellWipeLocation AS a JOIN WellWipeType AS b ON a.WellWipeTypeID = b.WellWipeTypeID WHERE a.WellWipeLocationID = :WellWipeLocationID;");
		d_bind_int64(":WellWipeLocationID", wellWipe->WipeLocation.WellWipeLocationID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			wellWipe->WipeLocation.WellWipeLocationGroupID = d_column_int64(0);
			strcpy(wellWipe->WipeLocation.Name, (const char *) d_column_text(1));
			wellWipe->WipeLocation.Threshold = d_column_double(2);
			wellWipe->WipeLocation.CountTime = d_column_int(3);
			wellWipe->WipeLocation.CreatedOnValue = d_column_double(4);
			wellWipe->WipeLocation.CreatedOn = strtotime_t((const char *) d_column_text(5));
			wellWipe->WipeLocation.LastUpdated = strtotime_t((const char *) d_column_text(6));
			wellWipe->WipeLocation.WellWipeTypeID = d_column_int64(7);
			wellWipe->WipeLocation.WellWipeTypeGroupID = d_column_int64(8);
			strcpy(wellWipe->WipeLocation.WellWipeTypeName, (const char *) d_column_text(9));
		}
		d_finalize();

		for(index=0; index<10; index++) wellWipe->WipeLocation.NuclideID[index] = -1;

		d_prepare_v2("SELECT NuclideID FROM WellWipeLocationNuclide WHERE WellWipeLocationID = :WellWipeLocationID;");
		d_bind_int64(":WellWipeLocationID", wellWipe->WipeLocation.WellWipeLocationID);
		d_step();
		index = 0;
		while(*capStatus == SQLITE_ROW){
			wellWipe->WipeLocation.NuclideID[index] = d_column_int(0);
			index++;
			d_step();
		}
		d_finalize();
	}

	if(wellWipe->Spectrum.SpectraID > 0) DB_ReadSpectrum(&(wellWipe->Spectrum));

	if(wellWipe->Background.WellBackgroundID > 0) DB_RetrieveBackgnd(&(wellWipe->Background));

	for(index=0; index<10; index++) wellWipe->WellWipeNuclide[index].WellWipeNuclideID = 0;
	d_prepare_v2("SELECT b.WellWipeNuclideID, b.WellWipeNuclideGroupID, b.NuclideID, b.Name, b.PrimaryEnergy, b.SecondaryEnergy, b.TertiaryEnergy, b.Efficiency, CreatedOn, datetime(CreatedOn), datetime(LastUpdated) FROM WellWipeWellWipeNuclide AS a JOIN WellWipeNuclide AS b ON a.WellWipeNuclideID = b.WellWipeNuclideID WHERE a.WellWipeID = :WellWipeID;");
	d_bind_int64(":WellWipeID", wellWipe->WellWipeID);
	d_step();
	index = 0;
	while(*capStatus == SQLITE_ROW){
		if(index < 10){
			wellWipe->WellWipeNuclide[index].WellWipeNuclideID = d_column_int64(0);
			wellWipe->WellWipeNuclide[index].WellWipeNuclideGroupID = d_column_int64(1);
			wellWipe->WellWipeNuclide[index].NuclideID = d_column_int(2);
			strcpy(wellWipe->WellWipeNuclide[index].Name, (const char *) d_column_text(3));
			wellWipe->WellWipeNuclide[index].PrimaryEnergy = d_column_double(4);
			wellWipe->WellWipeNuclide[index].SecondaryEnergy = d_column_double(5);
			wellWipe->WellWipeNuclide[index].TertiaryEnergy = d_column_double(6);
			wellWipe->WellWipeNuclide[index].Efficiency = d_column_double(7);
			wellWipe->WellWipeNuclide[index].CreatedOnValue = d_column_double(8);
			wellWipe->WellWipeNuclide[index].CreatedOn = strtotime_t((const char *) d_column_text(9));
			wellWipe->WellWipeNuclide[index].LastUpdated = strtotime_t((const char *) d_column_text(10));
			index++;
		}
		d_step();
	}
	d_finalize();

	for(index=0; index<40; index++){
		wellWipe->Peaks[index].WellWipePeakID = 0;
		wellWipe->Peaks[index].Energy = -1;
	}
	d_prepare_v2("SELECT WellWipePeakID, Energy, StartROI, EndROI, ROICounts, ROICPM, BackgroundROICounts, BackgroundROICPM, NetROICPM, WellWipeNuclideID, Activity, ExceedsThreshold, PeakType FROM WellWipePeak WHERE WellWipeID = :WellWipeID;");
	d_bind_int64(":WellWipeID", wellWipe->WellWipeID);
	d_step();
	index = 0;
	while(*capStatus == SQLITE_ROW){
		if(index<40){
			wellWipe->Peaks[index].WellWipePeakID = d_column_int64(0);
			wellWipe->Peaks[index].WellWipeID = wellWipe->WellWipeID;
			wellWipe->Peaks[index].Energy = d_column_double(1);
			wellWipe->Peaks[index].StartROI = d_column_double(2);
			wellWipe->Peaks[index].EndROI = d_column_double(3);
			wellWipe->Peaks[index].ROICounts = d_column_int(4);
			wellWipe->Peaks[index].ROICPM = d_column_double(5);
			wellWipe->Peaks[index].BackgroundROICounts = d_column_int(6);
			wellWipe->Peaks[index].BackgroundROICPM = d_column_double(7);
			wellWipe->Peaks[index].NetROICPM = d_column_double(8);
			wellWipe->Peaks[index].WellWipeNuclideID = d_column_int64(9);
			wellWipe->Peaks[index].Activity = d_column_double(10);
			if(d_column_int(11) == 0) wellWipe->Peaks[index].ExceedsThreshold = FALSE;
			else wellWipe->Peaks[index].ExceedsThreshold = TRUE;
			wellWipe->Peaks[index].PeakType = d_column_int(12);
			index++;
		}
		d_step();
	}
	d_finalize();
}

void DB_InactivateWipe(WELLWIPE *wellWipe){
	*capStatus = SQLITE_OK;

	d_prepare_v2("UPDATE WellWipe SET InactiveReason = :InactiveReason, Inactive = 1 WHERE WellWipeID = :WellWipeID;");
	d_bind_text(":InactiveReason", wellWipe->InactiveReason);
	d_bind_int64(":WellWipeID", wellWipe->WellWipeID);
	d_step();
	d_finalize();
}

void DB_CreateWellSystemTest(WELLSYSTEMTEST *wellSystemTest, bool bookEnd){
	char datetimestr[30];

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	DB_WriteSpectrum(&wellSystemTest->Spectrum, FALSE);
	if(wellSystemTest->Spectrum.SpectraID > 0){
		d_prepare_v2("INSERT INTO WellSystemTest(DetectorTest, NuclideName, SerialNumber, CalibrationActivity, CalibrationDate, Energy, StartROI, EndROI, Efficiency, Halflife, HalflifeUnit, PredictedActivity, ROICounts, ROICPM, BackgroundROICounts, BackgroundROICPM, NetROICPM, Activity, Deviation, WellSystemTest_SpectraID, WellBackgroundID, InactiveReason, Inactive) VALUES(:DetectorTest, :NuclideName, :SerialNumber, :CalibrationActivity, julianday(:CalibrationDate), :Energy, :StartROI, :EndROI, :Efficiency, :Halflife, :HalflifeUnit, :PredictedActivity, :ROICounts, :ROICPM, :BackgroundROICounts, :BackgroundROICPM, :NetROICPM, :Activity, :Deviation, :WellSystemTest_SpectraID, :WellBackgroundID, :InactiveReason, :Inactive);");
		d_bind_int(":DetectorTest", wellSystemTest->DetectorTest);
		d_bind_text(":NuclideName", wellSystemTest->NuclideName);
		d_bind_text(":SerialNumber", wellSystemTest->SerialNumber);
		d_bind_double(":CalibrationActivity", wellSystemTest->CalibrationActivity);
		stringout(datetimestr, &(wellSystemTest->CalibrationDate));
		d_bind_text(":CalibrationDate", datetimestr);
		d_bind_double(":Energy", wellSystemTest->Energy);
		d_bind_double(":StartROI", wellSystemTest->StartROI);
		d_bind_double(":EndROI", wellSystemTest->EndROI);
		d_bind_double(":Efficiency", wellSystemTest->Efficiency);
		d_bind_double(":Halflife", wellSystemTest->Halflife);
		d_bind_int(":HalflifeUnit", wellSystemTest->HalflifeUnit);
		d_bind_double(":PredictedActivity", wellSystemTest->PredictedActivity);
		d_bind_int(":ROICounts", wellSystemTest->ROICounts);
		d_bind_double(":ROICPM", wellSystemTest->ROICPM);
		d_bind_int(":BackgroundROICounts", wellSystemTest->BackgroundROICounts);
		d_bind_double(":BackgroundROICPM", wellSystemTest->BackgroundROICPM);
		d_bind_double(":NetROICPM", wellSystemTest->NetROICPM);
		d_bind_double(":Activity", wellSystemTest->Activity);
		d_bind_double(":Deviation", wellSystemTest->Deviation);
		d_bind_int64(":WellSystemTest_SpectraID", wellSystemTest->Spectrum.SpectraID);
		d_bind_int64(":WellBackgroundID", wellSystemTest->WellBackgroundID);
		d_bind_text(":InactiveReason", wellSystemTest->InactiveReason);
		if(wellSystemTest->Inactive) d_bind_int(":Inactive", 1);
		else d_bind_int(":Inactive", 0);
		d_step();
		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK){
			wellSystemTest->WellSystemTestID = sqlite3_last_insert_rowid(capDB);
		}
		d_finalize();
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

int DB_SearchSystemTests(WELLSYSTEMTESTSEARCH *wellSystemTests, time_t startDate, time_t endDate, int maxRetrieved, int devType){
	int retrievedCount, index;
	WELLSYSTEMTESTSEARCH *testPtr;
	time_t buffer;
	char datestring[30];

	testPtr = wellSystemTests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->WellSystemTestID = 0;
		testPtr++;
	}

	testPtr = wellSystemTests;
	retrievedCount = 0;

	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	if(devType == 0){
		d_prepare_v2("SELECT a.WellSystemTestID, a.SerialNumber, a.Deviation, datetime(b.MeasuredOn), a.Inactive, a.DetectorTest FROM WellSystemTest AS a JOIN Spectra AS b ON a.WellSystemTest_SpectraID = b.SpectraID WHERE b.MeasuredOn >= julianday(:startDate) AND b.MeasuredOn < julianday(:endDate) ORDER BY b.MeasuredOn DESC");
	}else if(devType == 1){
		d_prepare_v2("SELECT a.WellSystemTestID, a.SerialNumber, a.Deviation, datetime(b.MeasuredOn), a.Inactive, a.DetectorTest FROM WellSystemTest AS a JOIN Spectra AS b ON a.WellSystemTest_SpectraID = b.SpectraID WHERE b.MeasuredOn >= julianday(:startDate) AND b.MeasuredOn < julianday(:endDate) AND (a.DetectorTest = 3 OR a.DetectorTest = 5 OR a.DetectorTest = 7) ORDER BY b.MeasuredOn DESC");
	}else if(devType == 2){
		d_prepare_v2("SELECT a.WellSystemTestID, a.SerialNumber, a.Deviation, datetime(b.MeasuredOn), a.Inactive, a.DetectorTest FROM WellSystemTest AS a JOIN Spectra AS b ON a.WellSystemTest_SpectraID = b.SpectraID WHERE b.MeasuredOn >= julianday(:startDate) AND b.MeasuredOn < julianday(:endDate) AND (a.DetectorTest = 1 OR a.DetectorTest = 4 OR a.DetectorTest = 6) ORDER BY b.MeasuredOn DESC");
	}

	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();

	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->WellSystemTestID = d_column_int64(0);
				strcpy(testPtr->SerialNumber, (const char *) d_column_text(1));
				testPtr->Deviation = d_column_double(2);
				testPtr->MeasuredOn = strtotime_t((const char *) d_column_text(3));
				if(d_column_int(4) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;
				testPtr->DetectorTest = d_column_int(5);
				testPtr++;
			}
			retrievedCount++;
			d_step();
		}

		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}

void DB_RetrieveSystemTest(WELLSYSTEMTEST *wellSystemTest){
	wellSystemTest->Spectrum.SpectraID = 0;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT DetectorTest, NuclideName, SerialNumber, CalibrationActivity, datetime(CalibrationDate), Energy, StartROI, EndROI, Efficiency, Halflife, HalflifeUnit, PredictedActivity, ROICounts, ROICPM, BackgroundROICounts, BackgroundROICPM, NetROICPM, Activity, Deviation, WellSystemTest_SpectraID, WellBackgroundID, datetime(CreatedOn), InactiveReason, Inactive FROM WellSystemTest WHERE WellSystemTestID = :WellSystemTestID;");
	d_bind_int64(":WellSystemTestID", wellSystemTest->WellSystemTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		wellSystemTest->DetectorTest = d_column_int(0);
		strcpy(wellSystemTest->NuclideName, (const char *) d_column_text(1));
		strcpy(wellSystemTest->SerialNumber, (const char *) d_column_text(2));
		wellSystemTest->CalibrationActivity = d_column_double(3);
		wellSystemTest->CalibrationDate = strtotime_t((const char *) d_column_text(4));
		wellSystemTest->Energy = d_column_double(5);
		wellSystemTest->StartROI = d_column_double(6);
		wellSystemTest->EndROI = d_column_double(7);
		wellSystemTest->Efficiency = d_column_double(8);
		wellSystemTest->Halflife = d_column_double(9);
		wellSystemTest->HalflifeUnit = d_column_int(10);
		wellSystemTest->PredictedActivity = d_column_double(11);
		wellSystemTest->ROICounts = d_column_int(12);
		wellSystemTest->ROICPM = d_column_double(13);
		wellSystemTest->BackgroundROICounts = d_column_int(14);
		wellSystemTest->BackgroundROICPM = d_column_double(15);
		wellSystemTest->NetROICPM = d_column_double(16);
		wellSystemTest->Activity = d_column_double(17);
		wellSystemTest->Deviation = d_column_double(18);
		wellSystemTest->Spectrum.SpectraID = d_column_int64(19);
		wellSystemTest->WellBackgroundID = d_column_int64(20);
		wellSystemTest->CreatedOn = strtotime_t((const char *) d_column_text(21));
		strcpy(wellSystemTest->InactiveReason, (const char *) d_column_text(22));
		if(d_column_int(23) == 0) wellSystemTest->Inactive = FALSE;
		else wellSystemTest->Inactive = TRUE;
	}
	d_finalize();

	if(wellSystemTest->Spectrum.SpectraID > 0) DB_ReadSpectrum(&(wellSystemTest->Spectrum));
}

void DB_InactivateSystemTest(WELLSYSTEMTEST *wellSystemTest){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE WellSystemTest SET InactiveReason = :InactiveReason, Inactive = 1 WHERE WellSystemTestID = :WellSystemTestID;");
	d_bind_text(":InactiveReason", wellSystemTest->InactiveReason);
	d_bind_int64(":WellSystemTestID", wellSystemTest->WellSystemTestID);
	d_step();
	d_finalize();
}

void DB_CreateMDATest(WELLMDATEST *wellMDATest, bool bookEnd){
	bool flgNotFound;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	if(wellMDATest->Spectrum.SpectraID == 0){
		DB_WriteSpectrum(&(wellMDATest->Spectrum), FALSE);
		flgNotFound = TRUE;
	}else{
		d_prepare_v2("SELECT WellMDATestID FROM WellMDATest WHERE DetectorTest = :DetectorTest AND NuclideName = :NuclideName AND PrecisionFactor = :PrecisionFactor AND CorrectionFactor = :CorrectionFactor AND WellMDATest_SpectraID = :WellMDATest_SpectraID;");
		d_bind_int(":DetectorTest", wellMDATest->DetectorTest);
		d_bind_text(":NuclideName", wellMDATest->NuclideName);
		d_bind_double(":PrecisionFactor", wellMDATest->PrecisionFactor);
		d_bind_double(":CorrectionFactor", wellMDATest->CorrectionFactor);
		d_bind_int64(":WellMDATest_SpectraID", wellMDATest->Spectrum.SpectraID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			wellMDATest->WellMDATestID = d_column_int64(0);
			flgNotFound = FALSE;
		}else{
			flgNotFound = TRUE;
		}
		d_finalize();
	}

	if(flgNotFound){
		d_prepare_v2("INSERT INTO WellMDATest(DetectorTest, NuclideName, Energy, StartROI, EndROI, Efficiency, PrecisionFactor, CorrectionFactor, ROICounts, MDA, WellMDATest_SpectraID, InactiveReason, Inactive) VALUES(:DetectorTest, :NuclideName, :Energy, :StartROI, :EndROI, :Efficiency, :PrecisionFactor, :CorrectionFactor, :ROICounts, :MDA, :WellMDATest_SpectraID, :InactiveReason, :Inactive);");
		d_bind_int(":DetectorTest", wellMDATest->DetectorTest);
		d_bind_text(":NuclideName", wellMDATest->NuclideName);
		d_bind_double(":Energy", wellMDATest->Energy);
		d_bind_double(":StartROI", wellMDATest->StartROI);
		d_bind_double(":EndROI", wellMDATest->EndROI);
		d_bind_double(":Efficiency", wellMDATest->Efficiency);
		d_bind_double(":PrecisionFactor", wellMDATest->PrecisionFactor);
		d_bind_double(":CorrectionFactor", wellMDATest->CorrectionFactor);
		d_bind_int(":ROICounts", wellMDATest->ROICounts);
		d_bind_double(":MDA", wellMDATest->MDA);
		d_bind_int64(":WellMDATest_SpectraID", wellMDATest->Spectrum.SpectraID);
		d_bind_text(":InactiveReason", wellMDATest->InactiveReason);
		d_bind_int(":Inactive", wellMDATest->Inactive);
		d_step();
		d_finalize();
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

int DB_SearchMDATests(WELLMDASEARCH *wellMDATests, time_t startDate, time_t endDate, int maxRetrieved, int devType){
	int retrievedCount, index;
	WELLMDASEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = wellMDATests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->WellMDATestID  = 0;
		testPtr++;
	}

	testPtr = wellMDATests;
	retrievedCount = 0;

	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	if(devType == 0){
		d_prepare_v2("SELECT a.WellMDATestID, a.NuclideName, a.MDA, datetime(b.MeasuredOn), a.Inactive, a.DetectorTest FROM WellMDATest As a JOIN Spectra As b ON a.WellMDATest_SpectraID = b.SpectraID WHERE b.MeasuredOn >= julianday(:startDate) AND b.MeasuredOn < julianday(:endDate) ORDER BY b.MeasuredOn DESC;");
	}else if(devType == 1){
		d_prepare_v2("SELECT a.WellMDATestID, a.NuclideName, a.MDA, datetime(b.MeasuredOn), a.Inactive, a.DetectorTest FROM WellMDATest As a JOIN Spectra As b ON a.WellMDATest_SpectraID = b.SpectraID WHERE b.MeasuredOn >= julianday(:startDate) AND b.MeasuredOn < julianday(:endDate) AND (a.DetectorTest = 3 OR a.DetectorTest = 5 OR a.DetectorTest = 7) ORDER BY b.MeasuredOn DESC;");
	}else if(devType == 2){
		d_prepare_v2("SELECT a.WellMDATestID, a.NuclideName, a.MDA, datetime(b.MeasuredOn), a.Inactive, a.DetectorTest FROM WellMDATest As a JOIN Spectra As b ON a.WellMDATest_SpectraID = b.SpectraID WHERE b.MeasuredOn >= julianday(:startDate) AND b.MeasuredOn < julianday(:endDate) AND (a.DetectorTest = 1 OR a.DetectorTest = 4 OR a.DetectorTest = 6) ORDER BY b.MeasuredOn DESC;");
	}

	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();
	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->WellMDATestID = d_column_int64(0);
				strcpy(testPtr->NuclideName, (const char *) d_column_text(1));
				testPtr->MDA = d_column_double(2);
				testPtr->MeasuredOn = strtotime_t((const char *) d_column_text(3));
				if(d_column_int(4) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;
				testPtr->DetectorType = d_column_int(5);
				testPtr++;
			}
			retrievedCount++;
			d_step();
		}

		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}

void DB_RetrieveMDATest(WELLMDATEST *wellMDATest){
	wellMDATest->Spectrum.SpectraID = 0;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT DetectorTest, NuclideName, Energy, StartROI, EndROI, Efficiency, PrecisionFactor, CorrectionFactor, ROICounts, MDA, WellMDATest_SpectraID, datetime(CreatedOn), InactiveReason, Inactive FROM WellMDATest WHERE WellMDATestID = :WellMDATestID;");
	d_bind_int64(":WellMDATestID", wellMDATest->WellMDATestID);
	d_step();

	if(*capStatus == SQLITE_ROW){
		wellMDATest->DetectorTest = d_column_int(0);
		strcpy(wellMDATest->NuclideName, (const char *) d_column_text(1));
		wellMDATest->Energy = d_column_double(2);
		wellMDATest->StartROI = d_column_double(3);
		wellMDATest->EndROI = d_column_double(4);
		wellMDATest->Efficiency = d_column_double(5);
		wellMDATest->PrecisionFactor = d_column_double(6);
		wellMDATest->CorrectionFactor = d_column_double(7);
		wellMDATest->ROICounts = d_column_int(8);
		wellMDATest->MDA = d_column_double(9);
		wellMDATest->Spectrum.SpectraID = d_column_int64(10);
		wellMDATest->CreatedOn = strtotime_t((const char *) d_column_text(11));
		strcpy(wellMDATest->InactiveReason, (const char *) d_column_text(12));
		if(d_column_int(13) == 0) wellMDATest->Inactive = FALSE;
		else wellMDATest->Inactive = TRUE;
	}
	d_finalize();

	if(wellMDATest->Spectrum.SpectraID > 0) DB_ReadSpectrum(&(wellMDATest->Spectrum));
}

void DB_InactivateMDATest(WELLMDATEST *wellMDATest){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE WellMDATest SET InactiveReason = :InactiveReason, Inactive = 1 WHERE WellMDATestID = :WellMDATestID;");
	d_bind_text(":InactiveReason", wellMDATest->InactiveReason);
	d_bind_int64(":WellMDATestID", wellMDATest->WellMDATestID);
	d_step();
	d_finalize();
}

void DB_CreateChiTest(WELLCHITEST *wellChiTest, bool bookEnd){
	int index;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO WellChiSquareTest(DetectorTest, SampleTime, NumberOfSamples, NuclideName, Energy, StartROI, EndROI, ChiSquare, InactiveReason, Inactive) VALUES(:DetectorTest, :SampleTime, :NumberOfSamples, :NuclideName, :Energy, :StartROI, :EndROI, :ChiSquare, :InactiveReason, :Inactive);");
	d_bind_int(":DetectorTest", wellChiTest->DetectorTest);
	d_bind_int(":SampleTime", wellChiTest->SampleTime);
	d_bind_int(":NumberOfSamples", wellChiTest->NumberOfSamples);
	d_bind_text(":NuclideName", wellChiTest->NuclideName);
	d_bind_double(":Energy", wellChiTest->Energy);
	d_bind_double(":StartROI", wellChiTest->StartROI);
	d_bind_double(":EndROI", wellChiTest->EndROI);
	d_bind_double(":ChiSquare", wellChiTest->ChiSquare);
	d_bind_text(":InactiveReason", "");
	d_bind_int(":Inactive", 0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) wellChiTest->WellChiSquareTestID = sqlite3_last_insert_rowid(capDB);
	else wellChiTest->WellChiSquareTestID = 0;
	d_finalize();

	if(wellChiTest->WellChiSquareTestID > 0){
		d_prepare_v2("SELECT datetime(CreatedOn) FROM WellChiSquareTest WHERE WellChiSquareTestID = :WellChiSquareTestID;");
		d_bind_int64(":WellChiSquareTestID", wellChiTest->WellChiSquareTestID);
		d_step();
		if(*capStatus == SQLITE_ROW) wellChiTest->CreatedOn = strtotime_t((const char *) d_column_text(0));
		d_finalize();

		for(index=0; index<wellChiTest->NumberOfSamples; index++){
			DB_WriteSpectrum(&(wellChiTest->WellChiSquareSample[index].Spectrum), FALSE);
			d_prepare_v2("INSERT INTO WellChiSquareSample(WellChiSquareTestID, ROICounts, WellChiSquareSample_SpectraID) VALUES(:WellChiSquareTestID, :ROICounts, :WellChiSquareSample_SpectraID);");
			d_bind_int64(":WellChiSquareTestID", wellChiTest->WellChiSquareTestID);
			d_bind_int(":ROICounts", wellChiTest->WellChiSquareSample[index].ROICounts);
			d_bind_int64(":WellChiSquareSample_SpectraID", wellChiTest->WellChiSquareSample[index].Spectrum.SpectraID);
			d_step();
			if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
			if(*capStatus == SQLITE_OK) wellChiTest->WellChiSquareSample[index].WellChiSquareSampleID = sqlite3_last_insert_rowid(capDB);
			d_finalize();
		}
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

int DB_SearchChiTests(WELLCHISEARCH *wellChiTests, time_t startDate, time_t endDate, int maxRetrieved, int devType){
	int retrievedCount, index;
	WELLCHISEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = wellChiTests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->WellChiSquareTestID  = 0;
		testPtr++;
	}

	testPtr = wellChiTests;
	retrievedCount = 0;

	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	if(devType == 0){
		d_prepare_v2("SELECT WellChiSquareTestID, NuclideName, ChiSquare, datetime(CreatedOn), Inactive, DetectorTest FROM WellChiSquareTest WHERE CreatedOn >= julianday(:startDate) AND CreatedOn < julianday(:endDate) ORDER BY CreatedOn DESC;");
	}else if(devType == 1){
		d_prepare_v2("SELECT WellChiSquareTestID, NuclideName, ChiSquare, datetime(CreatedOn), Inactive, DetectorTest FROM WellChiSquareTest WHERE CreatedOn >= julianday(:startDate) AND CreatedOn < julianday(:endDate) AND (DetectorTest = 3 OR DetectorTest = 5 OR DetectorTest = 7) ORDER BY CreatedOn DESC;");
	}else if(devType == 2){
		d_prepare_v2("SELECT WellChiSquareTestID, NuclideName, ChiSquare, datetime(CreatedOn), Inactive, DetectorTest FROM WellChiSquareTest WHERE CreatedOn >= julianday(:startDate) AND CreatedOn < julianday(:endDate) AND (DetectorTest = 1 OR DetectorTest = 4 OR DetectorTest = 6) ORDER BY CreatedOn DESC;");
	}

	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();
	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->WellChiSquareTestID = d_column_int64(0);
				strcpy(testPtr->NuclideName, (const char *) d_column_text(1));
				testPtr->ChiSquare = d_column_double(2);
				testPtr->MeasuredOn = strtotime_t((const char *) d_column_text(3));
				if(d_column_int(4) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;
				testPtr->DetectorTest = d_column_int(5);
				testPtr++;
			}
			retrievedCount++;
			d_step();
		}

		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}

void DB_RetrieveChiTest(WELLCHITEST *wellChiTest){
	int index;

	wellChiTest->CreatedOn = 0;
	for(index=0; index<20; index++){
		wellChiTest->WellChiSquareSample[index].WellChiSquareSampleID = 0;
		wellChiTest->WellChiSquareSample[index].ROICounts = 0;
		wellChiTest->WellChiSquareSample[index].Spectrum.SpectraID = 0;
	}

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT DetectorTest, SampleTime, NumberOfSamples, NuclideName, Energy, StartROI, EndROI, ChiSquare, datetime(CreatedOn), InactiveReason, Inactive FROM WellChiSquareTest WHERE WellChiSquareTestID = :WellChiSquareTestID;");
	d_bind_int64(":WellChiSquareTestID", wellChiTest->WellChiSquareTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		wellChiTest->DetectorTest = d_column_int(0);
		wellChiTest->SampleTime = d_column_int(1);
		wellChiTest->NumberOfSamples = d_column_int(2);
		strcpy(wellChiTest->NuclideName, (const char *) d_column_text(3));
		wellChiTest->Energy = d_column_double(4);
		wellChiTest->StartROI = d_column_double(5);
		wellChiTest->EndROI = d_column_double(6);
		wellChiTest->ChiSquare = d_column_double(7);
		wellChiTest->CreatedOn = strtotime_t((const char *) d_column_text(8));
		strcpy(wellChiTest->InactiveReason, (const char *) d_column_text(9));
		if(d_column_int(10) == 0) wellChiTest->Inactive = FALSE;
		else wellChiTest->Inactive = TRUE;
	}
	d_finalize();

	d_prepare_v2("SELECT WellChiSquareSampleID, ROICounts, WellChiSquareSample_SpectraID FROM WellChiSquareSample WHERE WellChiSquareTestID = :WellChiSquareTestID;");
	d_bind_int64(":WellChiSquareTestID", wellChiTest->WellChiSquareTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		index = 0;
		while(*capStatus == SQLITE_ROW){
			if(index < 20){
				wellChiTest->WellChiSquareSample[index].WellChiSquareSampleID = d_column_int64(0);
				wellChiTest->WellChiSquareSample[index].ROICounts = d_column_int(1);
				wellChiTest->WellChiSquareSample[index].Spectrum.SpectraID = d_column_int64(2);
				index++;
			}
			d_step();
		}
	}
	d_finalize();

	for(index=0; index<20; index++){
		if(wellChiTest->WellChiSquareSample[index].Spectrum.SpectraID > 0) DB_ReadSpectrum(&(wellChiTest->WellChiSquareSample[index].Spectrum));
	}
}

void DB_InactivateChiTest(WELLCHITEST *wellChiTest){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE WellChiSquareTest SET InactiveReason = :InactiveReason, Inactive = 1 WHERE WellChiSquareTestID = :WellChiSquareTestID;");
	d_bind_text(":InactiveReason", wellChiTest->InactiveReason);
	d_bind_int64(":WellChiSquareTestID", wellChiTest->WellChiSquareTestID);
	d_step();
	d_finalize();
}

void DB_CreateTestIdent(TESTIDENT *testIdent, bool bookEnd){
	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO TestIdent(TestIdentGroupID, TestID, PatientID, FirstName, LastName, DateOfBirth, Sex, Physician, TechID, Inactive) VALUES(:TestIdentGroupID, :TestID, :PatientID, :FirstName, :LastName, :DateOfBirth, :Sex, :Physician, :TechID, :Inactive);");
	d_bind_int64(":TestIdentGroupID", 0);
	d_bind_text(":TestID", testIdent->TestID);
	d_bind_text(":PatientID", testIdent->PatientID);
	d_bind_text(":FirstName", testIdent->FirstName);
	d_bind_text(":LastName", testIdent->LastName);
	d_bind_double(":DateOfBirth", testIdent->DateOfBirth);
	d_bind_text(":Sex", testIdent->Sex);
	d_bind_text(":Physician", testIdent->Physician);
	d_bind_text(":TechID", testIdent->TechID);
	d_bind_int(":Inactive", 0);

	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK)	testIdent->TestIdentID = sqlite3_last_insert_rowid(capDB);
	else testIdent->TestIdentID = 0;
	d_finalize();

	if(testIdent->TestIdentID > 0){
		d_prepare_v2("UPDATE TestIdent SET TestIdentGroupID = TestIdentID WHERE TestIdentID = :TestIdentID;");
		d_bind_int64(":TestIdentID", testIdent->TestIdentID);
		d_step();
		d_finalize();

		d_prepare_v2("SELECT datetime(CreatedOn), datetime(LastUpdated) FROM TestIdent WHERE TestIdentID = :TestIdentID;");
		d_bind_int64(":TestIdentID", testIdent->TestIdentID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			testIdent->CreatedOn = strtotime_t((const char *) d_column_text(0));
			testIdent->LastUpdated = strtotime_t((const char *) d_column_text(1));
		}
		d_finalize();
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_RetrieveTestIdent(TESTIDENT *testIdent){
	*capStatus = SQLITE_OK;

	d_prepare_v2("SELECT TestIdentGroupID, TestID, PatientID, FirstName, LastName, DateOfBirth, Sex, Physician, TechID, datetime(CreatedOn), datetime(LastUpdated), Inactive FROM TestIdent WHERE TestIdentID = :TestIdentID;");
	d_bind_int64(":TestIdentID", testIdent->TestIdentID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		testIdent->TestIdentGroupID = d_column_int64(0);
		strcpy(testIdent->TestID, (const char *) d_column_text(1));
		strcpy(testIdent->PatientID, (const char *) d_column_text(2));
		strcpy(testIdent->FirstName, (const char *) d_column_text(3));
		strcpy(testIdent->LastName, (const char *) d_column_text(4));
		testIdent->DateOfBirth = d_column_double(5);
		strcpy(testIdent->Sex, (const char *) d_column_text(6));
		strcpy(testIdent->Physician, (const char *) d_column_text(7));
		strcpy(testIdent->TechID, (const char *) d_column_text(8));
		testIdent->CreatedOn = strtotime_t((const char *) d_column_text(9));
		testIdent->LastUpdated = strtotime_t((const char *) d_column_text(10));
		if(d_column_int(11) == 0) testIdent->Inactive = FALSE;
		else testIdent->Inactive = TRUE;
	}else{
		testIdent->TestIdentID = 0;
	}
	d_finalize();
}

void DB_CreateSchillingTest(WELLSCHILLINGTEST *wellSchillingTest, bool bookEnd){
	char CreatedOn[40];

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	DB_WriteSpectrum(&wellSchillingTest->SchillingStandard_Spectrum, FALSE);
	DB_WriteSpectrum(&wellSchillingTest->SchillingAliquot_Spectrum, FALSE);
	DB_CreateTestIdent(&wellSchillingTest->TestIdent, FALSE);
	if((wellSchillingTest->SchillingStandard_Spectrum.SpectraID > 0) && (wellSchillingTest->SchillingAliquot_Spectrum.SpectraID > 0) && (wellSchillingTest->TestIdent.TestIdentID >0)){
		d_prepare_v2("INSERT INTO WellSchillingTest(UrineVolume, AliquotVolume, DilutionFactor, StartROI, EndROI, StandardROICounts, StandardROICPM, AliquotROICounts, AliquotROICPM, BackgroundROICounts, BackgroundROICPM, Excretion, SchillingStandard_SpectraID, SchillingAliquot_SpectraID, WellBackgroundID, TestIdentID, CreatedOn, InactiveReason, Inactive) VALUES(:UrineVolume, :AliquotVolume, :DilutionFactor, :StartROI, :EndROI, :StandardROICounts, :StandardROICPM, :AliquotROICounts, :AliquotROICPM, :BackgroundROICounts, :BackgroundROICPM, :Excretion, :SchillingStandard_SpectraID, :SchillingAliquot_SpectraID, :WellBackgroundID, :TestIdentID, julianday(:CreatedOn), :InactiveReason, :Inactive);");
		d_bind_double(":UrineVolume", wellSchillingTest->UrineVolume);
		d_bind_double(":AliquotVolume", wellSchillingTest->AliquotVolume);
		d_bind_double(":DilutionFactor", wellSchillingTest->DilutionFactor);
		d_bind_double(":StartROI", wellSchillingTest->StartROI);
		d_bind_double(":EndROI", wellSchillingTest->EndROI);
		d_bind_int(":StandardROICounts", wellSchillingTest->StandardROICounts);
		d_bind_double(":StandardROICPM", wellSchillingTest->StandardROICPM);
		d_bind_int(":AliquotROICounts", wellSchillingTest->AliquotROICounts);
		d_bind_double(":AliquotROICPM", wellSchillingTest->AliquotROICPM);
		d_bind_int(":BackgroundROICounts", wellSchillingTest->BackgroundROICounts);
		d_bind_double(":BackgroundROICPM", wellSchillingTest->BackgroundROICPM);
		d_bind_double(":Excretion", wellSchillingTest->Excretion);
		d_bind_int64(":SchillingStandard_SpectraID", wellSchillingTest->SchillingStandard_Spectrum.SpectraID);
		d_bind_int64(":SchillingAliquot_SpectraID", wellSchillingTest->SchillingAliquot_Spectrum.SpectraID);
		d_bind_int64(":WellBackgroundID", wellSchillingTest->Background.WellBackgroundID);
		d_bind_int64(":TestIdentID", wellSchillingTest->TestIdent.TestIdentID);
		stringout(CreatedOn, &(wellSchillingTest->CreatedOn));
		d_bind_text(":CreatedOn", CreatedOn);
		d_bind_text(":InactiveReason", "");
		d_bind_int(":Inactive", 0);

		d_step();

		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK)	wellSchillingTest->WellSchillingTestID = sqlite3_last_insert_rowid(capDB);
		else wellSchillingTest->WellSchillingTestID = 0;

		d_finalize();
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

int DB_SearchSchillingTests(WELLSCHILLINGSEARCH *wellSchillingTests, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount, index;
	WELLSCHILLINGSEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = wellSchillingTests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->WellSchillingTestID = 0;
		testPtr++;
	}
	testPtr = wellSchillingTests;
	retrievedCount = 0;

	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT a.WellSchillingTestID, b.LastName, b.FirstName, datetime(a.CreatedOn), a.Inactive FROM WellSchillingTest AS a JOIN TestIdent AS b ON a.TestIdentID = b.TestIdentID WHERE a.CreatedOn >= julianday(:startDate) AND a.CreatedOn < julianday(:endDate) ORDER BY a.CreatedOn DESC;");
	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();
	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->WellSchillingTestID = d_column_int64(0);
				strcpy(testPtr->LastName, (const char *) d_column_text(1));
				strcpy(testPtr->FirstName, (const char *) d_column_text(2));
				testPtr->MeasuredOn = strtotime_t((const char *) d_column_text(3));
				if(d_column_int(4) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;
				testPtr++;
			}
			retrievedCount++;
			d_step();
		}

		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}

void DB_RetrieveSchillingTest(WELLSCHILLINGTEST *wellSchillingTest){
	wellSchillingTest->SchillingAliquot_Spectrum.SpectraID = 0;
	wellSchillingTest->SchillingStandard_Spectrum.SpectraID = 0;
	wellSchillingTest->TestIdent.TestIdentID = 0;
	wellSchillingTest->Background.WellBackgroundID = 0;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT UrineVolume, AliquotVolume, DilutionFactor, StartROI, EndROI, StandardROICounts, StandardROICPM, AliquotROICounts, AliquotROICPM, BackgroundROICounts, BackgroundROICPM, Excretion, SchillingStandard_SpectraID, SchillingAliquot_SpectraID, WellBackgroundID, TestIdentID, datetime(CreatedOn), InactiveReason, Inactive FROM WellSchillingTest WHERE WellSchillingTestID = :WellSchillingTestID;");
	d_bind_int64(":WellSchillingTestID", wellSchillingTest->WellSchillingTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		wellSchillingTest->UrineVolume = d_column_double(0);
		wellSchillingTest->AliquotVolume = d_column_double(1);
		wellSchillingTest->DilutionFactor = d_column_double(2);
		wellSchillingTest->StartROI = d_column_double(3);
		wellSchillingTest->EndROI = d_column_double(4);
		wellSchillingTest->StandardROICounts = d_column_int(5);
		wellSchillingTest->StandardROICPM = d_column_double(6);
		wellSchillingTest->AliquotROICounts = d_column_int(7);
		wellSchillingTest->AliquotROICPM = d_column_double(8);
		wellSchillingTest->BackgroundROICounts = d_column_int(9);
		wellSchillingTest->BackgroundROICPM = d_column_double(10);
		wellSchillingTest->Excretion = d_column_double(11);
		wellSchillingTest->SchillingStandard_Spectrum.SpectraID = d_column_int64(12);
		wellSchillingTest->SchillingAliquot_Spectrum.SpectraID = d_column_int64(13);
		wellSchillingTest->Background.WellBackgroundID = d_column_int64(14);
		wellSchillingTest->TestIdent.TestIdentID = d_column_int64(15);
		wellSchillingTest->CreatedOn = strtotime_t((const char *) d_column_text(16));
		strcpy(wellSchillingTest->InactiveReason, (const char *) d_column_text(17));
		if(d_column_int(18) == 0) wellSchillingTest->Inactive = FALSE;
		else wellSchillingTest->Inactive = TRUE;
	}else{
		wellSchillingTest->WellSchillingTestID = 0;
	}
	d_finalize();

	if(wellSchillingTest->SchillingAliquot_Spectrum.SpectraID > 0){
		DB_ReadSpectrum(&(wellSchillingTest->SchillingAliquot_Spectrum));
	}

	if(wellSchillingTest->SchillingStandard_Spectrum.SpectraID > 0){
		DB_ReadSpectrum(&(wellSchillingTest->SchillingStandard_Spectrum));
	}

	if(wellSchillingTest->TestIdent.TestIdentID > 0){
		DB_RetrieveTestIdent(&(wellSchillingTest->TestIdent));
	}

	if(wellSchillingTest->Background.WellBackgroundID > 0){
		DB_RetrieveBackgnd(&(wellSchillingTest->Background));
	}
}

void DB_InactivateSchillingTest(WELLSCHILLINGTEST *wellSchillingTest){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE WellSchillingTest SET InactiveReason = :InactiveReason, Inactive = 1 WHERE WellSchillingTestID = :WellSchillingTestID;");
	d_bind_text(":InactiveReason", wellSchillingTest->InactiveReason);
	d_bind_int64(":WellSchillingTestID", wellSchillingTest->WellSchillingTestID);
	d_step();
	d_finalize();
}

void DB_CreatePlasmaTest(WELLPLASMATEST *wellPlasmaTest, bool bookEnd){
	char CreatedOn[40];

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	DB_WriteSpectrum(&wellPlasmaTest->Standard_Spectrum, FALSE);
	DB_WriteSpectrum(&wellPlasmaTest->WholeBlood_Spectrum, FALSE);
	DB_WriteSpectrum(&wellPlasmaTest->Plasma_Spectrum, FALSE);
	DB_CreateTestIdent(&wellPlasmaTest->TestIdent, FALSE);

	if((wellPlasmaTest->Standard_Spectrum.SpectraID > 0) && (wellPlasmaTest->WholeBlood_Spectrum.SpectraID > 0) && (wellPlasmaTest->Plasma_Spectrum.SpectraID > 0) && (wellPlasmaTest->TestIdent.TestIdentID > 0)){
		d_prepare_v2("INSERT INTO WellPlasmaTest(DilutionFactor, SampleVolume, Hematocrit, PatientWeight, StartROI, EndROI, StandardROICounts, StandardROICPM, WholeBloodROICounts, WholeBloodROICPM, PlasmaROICounts, PlasmaROICPM, BackgroundROICounts, BackgroundROICPM, WholeBloodVolume, WholeBloodPerKg, PlasmaVolume, PlasmaPerKg, RBCVolume, RBCPerKg, CalculatedHematocrit, Standard_SpectraID, WholeBlood_SpectraID, Plasma_SpectraID, WellBackgroundID, TestIdentID, CreatedOn, InactiveReason, Inactive) VALUES(:DilutionFactor, :SampleVolume, :Hematocrit, :PatientWeight, :StartROI, :EndROI, :StandardROICounts, :StandardROICPM, :WholeBloodROICounts, :WholeBloodROICPM, :PlasmaROICounts, :PlasmaROICPM, :BackgroundROICounts, :BackgroundROICPM, :WholeBloodVolume, :WholeBloodPerKg, :PlasmaVolume, :PlasmaPerKg, :RBCVolume, :RBCPerKg, :CalculatedHematocrit, :Standard_SpectraID, :WholeBlood_SpectraID, :Plasma_SpectraID, :WellBackgroundID, :TestIdentID, julianday(:CreatedOn), :InactiveReason, :Inactive);");
		d_bind_double(":DilutionFactor", wellPlasmaTest->DilutionFactor);
		d_bind_double(":SampleVolume", wellPlasmaTest->SampleVolume);
		d_bind_double(":Hematocrit", wellPlasmaTest->Hematocrit);
		d_bind_double(":PatientWeight", wellPlasmaTest->PatientWeight);
		d_bind_double(":StartROI", wellPlasmaTest->StartROI);
		d_bind_double(":EndROI", wellPlasmaTest->EndROI);
		d_bind_int(":StandardROICounts", wellPlasmaTest->StandardROICounts);
		d_bind_double(":StandardROICPM", wellPlasmaTest->StandardROICPM);
		d_bind_int(":WholeBloodROICounts", wellPlasmaTest->WholeBloodROICounts);
		d_bind_double(":WholeBloodROICPM", wellPlasmaTest->WholeBloodROICPM);
		d_bind_int(":PlasmaROICounts", wellPlasmaTest->PlasmaROICounts);
		d_bind_double(":PlasmaROICPM", wellPlasmaTest->PlasmaROICPM);
		d_bind_int(":BackgroundROICounts", wellPlasmaTest->BackgroundROICounts);
		d_bind_double(":BackgroundROICPM", wellPlasmaTest->BackgroundROICPM);
		d_bind_double(":WholeBloodVolume", wellPlasmaTest->WholeBloodVolume);
		d_bind_double(":WholeBloodPerKg", wellPlasmaTest->WholeBloodPerKg);
		d_bind_double(":PlasmaVolume", wellPlasmaTest->PlasmaVolume);
		d_bind_double(":PlasmaPerKg", wellPlasmaTest->PlasmaPerKg);
		d_bind_double(":RBCVolume", wellPlasmaTest->RBCVolume);
		d_bind_double(":RBCPerKg", wellPlasmaTest->RBCPerKg);
		d_bind_double(":CalculatedHematocrit", wellPlasmaTest->CalculatedHematocrit);
		d_bind_int64(":Standard_SpectraID", wellPlasmaTest->Standard_Spectrum.SpectraID);
		d_bind_int64(":WholeBlood_SpectraID", wellPlasmaTest->WholeBlood_Spectrum.SpectraID);
		d_bind_int64(":Plasma_SpectraID", wellPlasmaTest->Plasma_Spectrum.SpectraID);
		d_bind_int64(":WellBackgroundID", wellPlasmaTest->Background.WellBackgroundID);
		d_bind_int64(":TestIdentID", wellPlasmaTest->TestIdent.TestIdentID);
		stringout(CreatedOn, &(wellPlasmaTest->CreatedOn));
		d_bind_text(":CreatedOn", CreatedOn);
		d_bind_text(":InactiveReason", "");
		d_bind_int(":Inactive", 0);

		d_step();

		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK) wellPlasmaTest->WellPlasmaTestID = sqlite3_last_insert_rowid(capDB);
		else wellPlasmaTest->WellPlasmaTestID = 0;

		d_finalize();
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

int DB_SearchPlasmaTests(WELLPLASMASEARCH *wellPlasmaTests, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount, index;
	WELLPLASMASEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = wellPlasmaTests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->WellPlasmaTestID = 0;
		testPtr++;
	}
	testPtr = wellPlasmaTests;
	retrievedCount = 0;

	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT a.WellPlasmaTestID, b.LastName, b.FirstName, datetime(a.CreatedOn), a.Inactive FROM WellPlasmaTest AS a JOIN TestIdent AS b ON a.TestIdentID = b.TestIdentID WHERE a.CreatedOn >= julianday(:startDate) AND a.CreatedOn < julianday(:endDate) ORDER BY a.CreatedOn DESC;");
	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();
	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->WellPlasmaTestID = d_column_int64(0);
				strcpy(testPtr->LastName, (const char *) d_column_text(1));
				strcpy(testPtr->FirstName, (const char *) d_column_text(2));
				testPtr->MeasuredOn = strtotime_t((const char *) d_column_text(3));
				if(d_column_int(4) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;
				testPtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}

void DB_RetrievePlasmaTest(WELLPLASMATEST *wellPlasmaTest){
	wellPlasmaTest->Standard_Spectrum.SpectraID = 0;
	wellPlasmaTest->WholeBlood_Spectrum.SpectraID = 0;
	wellPlasmaTest->Plasma_Spectrum.SpectraID = 0;
	wellPlasmaTest->TestIdent.TestIdentID = 0;
	wellPlasmaTest->Background.WellBackgroundID = 0;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT DilutionFactor, SampleVolume, Hematocrit, PatientWeight, StartROI, EndROI, StandardROICounts, StandardROICPM, WholeBloodROICounts, WholeBloodROICPM, PlasmaROICounts, PlasmaROICPM, BackgroundROICounts, BackgroundROICPM, WholeBloodVolume, WholeBloodPerKg, PlasmaVolume, PlasmaPerKg, RBCVolume, RBCPerKg, CalculatedHematocrit, Standard_SpectraID, WholeBlood_SpectraID, Plasma_SpectraID, WellBackgroundID, TestIdentID, datetime(CreatedOn), InactiveReason, Inactive FROM WellPlasmaTest WHERE WellPlasmaTestID = :WellPlasmaTestID;");
	d_bind_int64(":WellPlasmaTestID", wellPlasmaTest->WellPlasmaTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		wellPlasmaTest->DilutionFactor = d_column_double(0);
		wellPlasmaTest->SampleVolume = d_column_double(1);
		wellPlasmaTest->Hematocrit = d_column_double(2);
		wellPlasmaTest->PatientWeight = d_column_double(3);
		wellPlasmaTest->StartROI = d_column_double(4);
		wellPlasmaTest->EndROI = d_column_double(5);
		wellPlasmaTest->StandardROICounts = d_column_int(6);
		wellPlasmaTest->StandardROICPM = d_column_double(7);
		wellPlasmaTest->WholeBloodROICounts = d_column_int(8);
		wellPlasmaTest->WholeBloodROICPM = d_column_double(9);
		wellPlasmaTest->PlasmaROICounts = d_column_int(10);
		wellPlasmaTest->PlasmaROICPM = d_column_double(11);
		wellPlasmaTest->BackgroundROICounts = d_column_int(12);
		wellPlasmaTest->BackgroundROICPM =  d_column_double(13);
		wellPlasmaTest->WholeBloodVolume = d_column_double(14);
		wellPlasmaTest->WholeBloodPerKg = d_column_double(15);
		wellPlasmaTest->PlasmaVolume = d_column_double(16);
		wellPlasmaTest->PlasmaPerKg = d_column_double(17);
		wellPlasmaTest->RBCVolume = d_column_double(18);
		wellPlasmaTest->RBCPerKg = d_column_double(19);
		wellPlasmaTest->CalculatedHematocrit = d_column_double(20);
		wellPlasmaTest->Standard_Spectrum.SpectraID = d_column_int64(21);
		wellPlasmaTest->WholeBlood_Spectrum.SpectraID = d_column_int64(22);
		wellPlasmaTest->Plasma_Spectrum.SpectraID = d_column_int64(23);
		wellPlasmaTest->Background.WellBackgroundID = d_column_int64(24);
		wellPlasmaTest->TestIdent.TestIdentID = d_column_int64(25);
		wellPlasmaTest->CreatedOn = strtotime_t((const char *) d_column_text(26));
		strcpy(wellPlasmaTest->InactiveReason, (const char *) d_column_text(27));
		if(d_column_int(28) == 0) wellPlasmaTest->Inactive = FALSE;
		else wellPlasmaTest->Inactive = TRUE;
	}else{
		wellPlasmaTest->WellPlasmaTestID = 0;
	}
	d_finalize();

	if(wellPlasmaTest->Standard_Spectrum.SpectraID > 0){
		DB_ReadSpectrum(&(wellPlasmaTest->Standard_Spectrum));
	}

	if(wellPlasmaTest->WholeBlood_Spectrum.SpectraID > 0){
		DB_ReadSpectrum(&(wellPlasmaTest->WholeBlood_Spectrum));
	}

	if(wellPlasmaTest->Plasma_Spectrum.SpectraID > 0){
		DB_ReadSpectrum(&(wellPlasmaTest->Plasma_Spectrum));
	}

	if(wellPlasmaTest->TestIdent.TestIdentID > 0){
		DB_RetrieveTestIdent(&(wellPlasmaTest->TestIdent));
	}

	if(wellPlasmaTest->Background.WellBackgroundID > 0){
		DB_RetrieveBackgnd(&(wellPlasmaTest->Background));
	}
}

void DB_InactivatePlasmaTest(WELLPLASMATEST *wellPlasmaTest){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE WellPlasmaTest SET InactiveReason = :InactiveReason, Inactive = 1 WHERE WellPlasmaTestID = :WellPlasmaTestID;");
	d_bind_text(":InactiveReason", wellPlasmaTest->InactiveReason);
	d_bind_int64(":WellPlasmaTestID", wellPlasmaTest->WellPlasmaTestID);
	d_step();
	d_finalize();
}

void DB_CreateRBCTest(WELLRBCTEST *wellRBCTest, bool bookEnd){
	char CreatedOn[40];

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	DB_WriteSpectrum(&wellRBCTest->WholeBloodStandard_Spectrum, FALSE);
	DB_WriteSpectrum(&wellRBCTest->PlasmaStandard_Spectrum, FALSE);
	DB_WriteSpectrum(&wellRBCTest->WholeBloodSample_Spectrum, FALSE);
	DB_WriteSpectrum(&wellRBCTest->PlasmaSample_Spectrum, FALSE);
	DB_CreateTestIdent(&wellRBCTest->TestIdent, FALSE);

	if((wellRBCTest->WholeBloodStandard_Spectrum.SpectraID > 0) && (wellRBCTest->PlasmaStandard_Spectrum.SpectraID > 0) && (wellRBCTest->WholeBloodSample_Spectrum.SpectraID > 0) && (wellRBCTest->PlasmaSample_Spectrum.SpectraID > 0) && (wellRBCTest->TestIdent.TestIdentID > 0)){
		d_prepare_v2("INSERT INTO WellRBCTest(DoseHematocrit, PatientHematocrit, PatientWeight, StartROI, EndROI, WholeBloodStandardROICounts, WholeBloodStandardROICPM, PlasmaStandardROICounts, PlasmaStandardROICPM, WholeBloodSampleROICounts, WholeBloodSampleROICPM, PlasmaSampleROICounts, PlasmaSampleROICPM, BackgroundROICounts, BackgroundROICPM, RBCVolume, RBCPerKg, WholeBloodVolume, WholeBloodPerKg, PlasmaVolume, PlasmaPerKg, WholeBloodStandard_SpectraID, PlasmaStandard_SpectraID, WholeBloodSample_SpectraID, PlasmaSample_SpectraID, WellBackgroundID, TestIdentID, CreatedOn, InactiveReason, Inactive) VALUES(:DoseHematocrit, :PatientHematocrit, :PatientWeight, :StartROI, :EndROI, :WholeBloodStandardROICounts, :WholeBloodStandardROICPM, :PlasmaStandardROICounts, :PlasmaStandardROICPM, :WholeBloodSampleROICounts, :WholeBloodSampleROICPM, :PlasmaSampleROICounts, :PlasmaSampleROICPM, :BackgroundROICounts, :BackgroundROICPM, :RBCVolume, :RBCPerKg, :WholeBloodVolume, :WholeBloodPerKg, :PlasmaVolume, :PlasmaPerKg, :WholeBloodStandard_SpectraID, :PlasmaStandard_SpectraID, :WholeBloodSample_SpectraID, :PlasmaSample_SpectraID, :WellBackgroundID, :TestIdentID, julianday(:CreatedOn), :InactiveReason, :Inactive);");
		d_bind_double(":DoseHematocrit", wellRBCTest->DoseHematocrit);
		d_bind_double(":PatientHematocrit", wellRBCTest->PatientHematocrit);
		d_bind_double(":PatientWeight", wellRBCTest->PatientWeight);
		d_bind_double(":StartROI", wellRBCTest->StartROI);
		d_bind_double(":EndROI", wellRBCTest->EndROI);
		d_bind_int(":WholeBloodStandardROICounts", wellRBCTest->WholeBloodStandardROICounts);
		d_bind_double(":WholeBloodStandardROICPM", wellRBCTest->WholeBloodStandardROICPM);
		d_bind_int(":PlasmaStandardROICounts", wellRBCTest->PlasmaStandardROICounts);
		d_bind_double(":PlasmaStandardROICPM", wellRBCTest->PlasmaStandardROICPM);
		d_bind_int(":WholeBloodSampleROICounts", wellRBCTest->WholeBloodSampleROICounts);
		d_bind_double(":WholeBloodSampleROICPM", wellRBCTest->WholeBloodSampleROICPM);
		d_bind_int(":PlasmaSampleROICounts", wellRBCTest->PlasmaSampleROICounts);
		d_bind_double(":PlasmaSampleROICPM", wellRBCTest->PlasmaSampleROICPM);
		d_bind_int(":BackgroundROICounts", wellRBCTest->BackgroundROICounts);
		d_bind_double(":BackgroundROICPM", wellRBCTest->BackgroundROICPM);
		d_bind_double(":RBCVolume", wellRBCTest->RBCVolume);
		d_bind_double(":RBCPerKg", wellRBCTest->RBCPerKg);
		d_bind_double(":WholeBloodVolume", wellRBCTest->WholeBloodVolume);
		d_bind_double(":WholeBloodPerKg", wellRBCTest->WholeBloodPerKg);
		d_bind_double(":PlasmaVolume", wellRBCTest->PlasmaVolume);
		d_bind_double(":PlasmaPerKg", wellRBCTest->PlasmaPerKg);
		d_bind_int64(":WholeBloodStandard_SpectraID", wellRBCTest->WholeBloodStandard_Spectrum.SpectraID);
		d_bind_int64(":PlasmaStandard_SpectraID", wellRBCTest->PlasmaStandard_Spectrum.SpectraID);
		d_bind_int64(":WholeBloodSample_SpectraID", wellRBCTest->WholeBloodSample_Spectrum.SpectraID);
		d_bind_int64(":PlasmaSample_SpectraID", wellRBCTest->PlasmaSample_Spectrum.SpectraID);
		d_bind_int64(":WellBackgroundID", wellRBCTest->Background.WellBackgroundID);
		d_bind_int64(":TestIdentID", wellRBCTest->TestIdent.TestIdentID);
		stringout(CreatedOn, &(wellRBCTest->CreatedOn));
		d_bind_text(":CreatedOn", CreatedOn);
		d_bind_text(":InactiveReason", "");
		d_bind_int(":Inactive", 0);
		d_step();

		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK) wellRBCTest->WellRBCTestID = sqlite3_last_insert_rowid(capDB);
		else wellRBCTest->WellRBCTestID = 0;

		d_finalize();
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

int DB_SearchRBCTests(WELLRBCSEARCH *wellRBCTests, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount, index;
	WELLRBCSEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = wellRBCTests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->WellRBCTestID = 0;
		testPtr++;
	}
	testPtr = wellRBCTests;
	retrievedCount = 0;

	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT a.WellRBCTestID, b.LastName, b.FirstName, datetime(a.CreatedOn), a.Inactive FROM WellRBCTest AS a JOIN TestIdent AS b ON a.TestIdentID = b.TestIdentID WHERE a.CreatedOn >= julianday(:startDate) AND a.CreatedOn < julianday(:endDate) ORDER BY a.CreatedOn DESC;");
	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();
	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->WellRBCTestID = d_column_int64(0);
				strcpy(testPtr->LastName, (const char *) d_column_text(1));
				strcpy(testPtr->FirstName, (const char *) d_column_text(2));
				testPtr->MeasuredOn = strtotime_t((const char *) d_column_text(3));
				if(d_column_int(4) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;
				testPtr++;
			}
			retrievedCount++;
			d_step();
		}

		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}

void DB_RetrieveRBCTest(WELLRBCTEST *wellRBCTest){
	wellRBCTest->WholeBloodStandard_Spectrum.SpectraID = 0;
	wellRBCTest->PlasmaStandard_Spectrum.SpectraID = 0;
	wellRBCTest->WholeBloodSample_Spectrum.SpectraID = 0;
	wellRBCTest->PlasmaSample_Spectrum.SpectraID = 0;
	wellRBCTest->TestIdent.TestIdentID = 0;
	wellRBCTest->Background.WellBackgroundID = 0;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT DoseHematocrit, PatientHematocrit, PatientWeight, StartROI, EndROI, WholeBloodStandardROICounts, WholeBloodStandardROICPM, PlasmaStandardROICounts, PlasmaStandardROICPM, WholeBloodSampleROICounts, WholeBloodSampleROICPM, PlasmaSampleROICounts, PlasmaSampleROICPM, BackgroundROICounts, BackgroundROICPM, RBCVolume, RBCPerKg, WholeBloodVolume, WholeBloodPerKg, PlasmaVolume, PlasmaPerKg, WholeBloodStandard_SpectraID, PlasmaStandard_SpectraID, WholeBloodSample_SpectraID, PlasmaSample_SpectraID, WellBackgroundID, TestIdentID, datetime(CreatedOn), InactiveReason, Inactive FROM WellRBCTest WHERE WellRBCTestID = :WellRBCTestID;");
	d_bind_int64(":WellRBCTestID", wellRBCTest->WellRBCTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		//DoseHematocrit
		wellRBCTest->DoseHematocrit = d_column_double(0);

		//PatientHematocrit
		wellRBCTest->PatientHematocrit = d_column_double(1);

		//PatientWeight
		wellRBCTest->PatientWeight = d_column_double(2);

		//StartROI
		wellRBCTest->StartROI = d_column_double(3);

		//EndROI
		wellRBCTest->EndROI = d_column_double(4);

		//WholeBloodStandardROICounts
		wellRBCTest->WholeBloodStandardROICounts = d_column_int(5);

		//WholeBloodStandardROICPM
		wellRBCTest->WholeBloodStandardROICPM = d_column_double(6);

		//PlasmaStandardROICounts
		wellRBCTest->PlasmaStandardROICounts = d_column_int(7);

		//PlasmaStandardROICPM
		wellRBCTest->PlasmaStandardROICPM = d_column_double(8);

		//WholeBloodSampleROICounts
		wellRBCTest->WholeBloodSampleROICounts = d_column_int(9);

		//WholeBloodSampleROICPM
		wellRBCTest->WholeBloodSampleROICPM = d_column_double(10);

		//PlasmaSampleROICounts
		wellRBCTest->PlasmaSampleROICounts = d_column_int(11);

		//PlasmaSampleROICPM
		wellRBCTest->PlasmaSampleROICPM = d_column_double(12);

		//BackgroundROICounts
		wellRBCTest->BackgroundROICounts = d_column_int(13);

		//BackgroundROICPM
		wellRBCTest->BackgroundROICPM = d_column_double(14);

		//RBCVolume
		wellRBCTest->RBCVolume = d_column_double(15);

		//RBCPerKg
		wellRBCTest->RBCPerKg = d_column_double(16);

		//WholeBloodVolume
		wellRBCTest->WholeBloodVolume = d_column_double(17);

		//WholeBloodPerKg
		wellRBCTest->WholeBloodPerKg = d_column_double(18);

		//PlasmaVolume
		wellRBCTest->PlasmaVolume = d_column_double(19);

		//PlasmaPerKg
		wellRBCTest->PlasmaPerKg = d_column_double(20);

		//WholeBloodStandard_SpectraID
		wellRBCTest->WholeBloodStandard_Spectrum.SpectraID = d_column_int64(21);

		//PlasmaStandard_SpectraID
		wellRBCTest->PlasmaStandard_Spectrum.SpectraID = d_column_int64(22);

		//WholeBloodSample_SpectraID
		wellRBCTest->WholeBloodSample_Spectrum.SpectraID = d_column_int64(23);

		//PlasmaSample_SpectraID
		wellRBCTest->PlasmaSample_Spectrum.SpectraID = d_column_int64(24);

		//WellBackgroundID
		wellRBCTest->Background.WellBackgroundID = d_column_int64(25);

		//TestIdentID
		wellRBCTest->TestIdent.TestIdentID = d_column_int64(26);

		//datetime(CreatedOn)
		wellRBCTest->CreatedOn = strtotime_t((const char *) d_column_text(27));

		//InactiveReason
		strcpy(wellRBCTest->InactiveReason, (const char *) d_column_text(28));

		//Inactive
		if(d_column_int(29) == 0) wellRBCTest->Inactive = FALSE;
		else wellRBCTest->Inactive = TRUE;
	}else{
		wellRBCTest->WellRBCTestID = 0;
	}
	d_finalize();

	if(wellRBCTest->WholeBloodStandard_Spectrum.SpectraID > 0){
		DB_ReadSpectrum(&(wellRBCTest->WholeBloodStandard_Spectrum));
	}

	if(wellRBCTest->PlasmaStandard_Spectrum.SpectraID > 0){
		DB_ReadSpectrum(&(wellRBCTest->PlasmaStandard_Spectrum));
	}

	if(wellRBCTest->WholeBloodSample_Spectrum.SpectraID > 0){
		DB_ReadSpectrum(&(wellRBCTest->WholeBloodSample_Spectrum));
	}

	if(wellRBCTest->PlasmaSample_Spectrum.SpectraID > 0){
		DB_ReadSpectrum(&(wellRBCTest->PlasmaSample_Spectrum));
	}

	if(wellRBCTest->TestIdent.TestIdentID > 0){
		DB_RetrieveTestIdent(&(wellRBCTest->TestIdent));
	}

	if(wellRBCTest->Background.WellBackgroundID > 0){
		DB_RetrieveBackgnd(&(wellRBCTest->Background));
	}
}

void DB_InactivateRBCTest(WELLRBCTEST *wellRBCTest){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE WellRBCTest SET InactiveReason = :InactiveReason, Inactive = 1 WHERE WellRBCTestID = :WellRBCTestID;");
	d_bind_text(":InactiveReason", wellRBCTest->InactiveReason);
	d_bind_int64(":WellRBCTestID", wellRBCTest->WellRBCTestID);
	d_step();
	d_finalize();
}

int DB_SearchAutoCalTests(WELLAUTOCALSEARCH *wellAutoCal, time_t startDate, time_t endDate, int maxRetrieved, int devType){
	int retrievedCount, index;
	WELLAUTOCALSEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = wellAutoCal;
	for(index=0; index<maxRetrieved; index++){
		testPtr->AutoCalID = 0;
		testPtr++;
	}
	testPtr = wellAutoCal;
	retrievedCount = 0;

	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	if(devType == 0){
		d_prepare_v2("SELECT AutoCalID, Gain1, Gain2, Sigma, datetime(CalStamp), DetectorType FROM AutoCal WHERE CalStamp >= julianday(:startDate) AND CalStamp < julianday(:endDate) ORDER BY CalStamp DESC;");
	}else if(devType == 1){
		d_prepare_v2("SELECT AutoCalID, Gain1, Gain2, Sigma, datetime(CalStamp), DetectorType FROM AutoCal WHERE CalStamp >= julianday(:startDate) AND CalStamp < julianday(:endDate) AND (DetectorType = 3 OR DetectorType = 5 OR DetectorType = 7) ORDER BY CalStamp DESC;");
	}else if(devType == 2){
		d_prepare_v2("SELECT AutoCalID, Gain1, Gain2, Sigma, datetime(CalStamp), DetectorType FROM AutoCal WHERE CalStamp >= julianday(:startDate) AND CalStamp < julianday(:endDate) AND (DetectorType = 1 OR DetectorType = 4 OR DetectorType = 6) ORDER BY CalStamp DESC;");
	}
	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();
	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->AutoCalID = d_column_int64(0);
				testPtr->Gain1 = d_column_int(1);
				testPtr->Gain2 = d_column_int(2);
				testPtr->Sigma = d_column_double(3);
				testPtr->MeasuredOn = strtotime_t((const char *) d_column_text(4));
				testPtr->DetectorType = d_column_int(5);
				testPtr->Inactive = FALSE;
				testPtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();
	return retrievedCount;
}

void DB_RetrieveAutoCalTest(AUTOCAL *autoCal){
	char calstamp[30], linstamp[30];
	float calstampfloat, linstampfloat;

	*capStatus = SQLITE_OK;

	d_prepare_v2("SELECT AutoCalID, ZeroOffset, Gain1, Gain2, Sigma, Sigma_Lip, Linearity0, Linearity1, Linearity2, Linearity3, Linearity4, datetime(CalStamp), CalStamp, datetime(LinStamp), LinStamp, SerialNum, Threshold, HV, DetectorType, NumOfChannels FROM AutoCal WHERE AutoCalID = :AutoCalID;");
	d_bind_int64(":AutoCalID", autoCal->AutoCalID);
	d_step();

	if(*capStatus == SQLITE_ROW){
		autoCal->AutoCalID = d_column_int64(0);
		autoCal->zeroopampoffset = d_column_int(1);
		autoCal->gain1 = d_column_int(2);
		autoCal->gain2 = d_column_int(3);
		autoCal->sigma = d_column_double(4);
		autoCal->sigma_lip = d_column_double(5);
		autoCal->linmeasured[0] = d_column_double(6);
		autoCal->linmeasured[1] = d_column_double(7);
		autoCal->linmeasured[2] = d_column_double(8);
		autoCal->linmeasured[3] = d_column_double(9);
		autoCal->linmeasured[4] = d_column_double(10);

		strcpy(calstamp, (const char *) d_column_text(11));
		calstampfloat = d_column_double(12);
		strcpy(linstamp, (const char *) d_column_text(13));
		linstampfloat = d_column_double(14);
		strcpy(autoCal->serialnum, (const char *) d_column_text(15));
		autoCal->threshold = d_column_int(16);
		autoCal->hv = d_column_int(17);
		autoCal->detectortype = d_column_int(18);
		autoCal->num_of_channels = d_column_int(19);

		if(calstampfloat == 0) autoCal->caltstamp = 0;
		else autoCal->caltstamp = strtotime_t(calstamp);

		if(linstampfloat == 0) autoCal->lintstamp = 0;
		else autoCal->lintstamp = strtotime_t(linstamp);
	}else{
		autoCal->AutoCalID = 0;
	}
	d_finalize();
}

static void DB_retrieveWellTime(short int *value, int index){
	char sQuery[100];

	*capStatus = SQLITE_OK;

	sprintf(sQuery, "SELECT SettingValue FROM Config WHERE SettingName = 'WellTime%d';", index);

	if(d_query(sQuery)){
		if(*capStatus == SQLITE_ROW){
			*value = d_column_double(0);
		}else{
			*value = 60;
		}
		d_finalize();
	}else{
		d_finalize();
		*value = 60;
		sprintf(sQuery, "INSERT INTO Config(SettingName,SettingValue) VALUES('WellTime%d',60);", index);
		d_command(sQuery);
	}
}

void DB_updateWellTime(short int value, int index){
	char sQuery[100];
	double fIndex;

	*capStatus = SQLITE_OK;
	sprintf(sQuery, "UPDATE Config SET SettingValue = :SettingValue WHERE SettingName = 'WellTime%d';", index);
	d_prepare_v2(sQuery);
	fIndex = value;
	d_bind_double(":SettingValue", fIndex);
	d_step();
	d_finalize();
}
/**
 * \details Create an entry in the UserNuclide table
 * \param nucindexoffset Offset in the nuclideData table
 * \param nucdata NUCDATA structure containing the User Nuclide info
 * \returns None
 */
void DB_CreateUserNuclide(int nucindexoffset, NUCDATA *nucdata){
	char nuclidename[8], fullname[16], code[8], datestring[30];

	*capStatus = SQLITE_OK;
	d_command("BEGIN;");

	d_prepare_v2("UPDATE UserNuclide SET Inactive = 1 WHERE NucIndexOffset=:NucIndexOffset;");
	d_bind_int(":NucIndexOffset", nucindexoffset);
	d_step();
	d_finalize();

	d_prepare_v2("INSERT INTO UserNuclide(NucIndexOffset, Show, HalfLife, Response_R, Response_P, Response_B, Response_18, Response_C, Response_K, Response_UserR, Response_UserP, Response_UserB, Response_User18, Response_UserC, Response_UserK, Name, FullName, Code, HLUnit, Energy1, Energy2, Energy3, WellEfficiency, BetaEfficiency, Probe700Efficiency, Well700Efficiency, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEfficiency, UserBetaEfficiency, UserProbe700Efficiency, UserWell700Efficiency, WellMeasuredOn, BetaMeasuredOn, Probe700MeasuredOn, Well700MeasuredOn, Inactive) VALUES(:NucIndexOffset, :Show, :HalfLife, :Response_R, :Response_P, :Response_B, :Response_18, :Response_C, :Response_K, :Response_UserR, :Response_UserP, :Response_UserB, :Response_User18, :Response_UserC, :Response_UserK, :Name, :FullName, :Code, :HLUnit, :Energy1, :Energy2, :Energy3, :WellEfficiency, :BetaEfficiency, :Probe700Efficiency, :Well700Efficiency, :UserEnergy1, :UserEnergy2, :UserEnergy3, :UserWellEfficiency, :UserBetaEfficiency, :UserProbe700Efficiency, :UserWell700Efficiency, julianday(:WellMeasuredOn), julianday(:BetaMeasuredOn), julianday(:Probe700MeasuredOn), julianday(:Well700MeasuredOn), :Inactive);");
	d_bind_int(":NucIndexOffset", nucindexoffset);
	d_bind_int(":Show", nucdata->show);
	d_bind_double(":HalfLife", nucdata->halflife);
	d_bind_double(":Response_R", nucdata->response[R_CHAMB]);
	d_bind_double(":Response_P", nucdata->response[P_CHAMB]);
	d_bind_double(":Response_B", nucdata->response[B_CHAMB]);
	d_bind_double(":Response_18", nucdata->response[ONE_DOT_EIGHT_CHAMB]);
	d_bind_double(":Response_C", nucdata->response[C_CHAMB]);
	d_bind_double(":Response_K", nucdata->response[K_CHAMB]);
	d_bind_double(":Response_UserR", nucdata->user_response[R_CHAMB]);
	d_bind_double(":Response_UserP", nucdata->user_response[P_CHAMB]);
	d_bind_double(":Response_UserB", nucdata->user_response[B_CHAMB]);
	d_bind_double(":Response_User18", nucdata->user_response[ONE_DOT_EIGHT_CHAMB]);
	d_bind_double(":Response_UserC", nucdata->user_response[C_CHAMB]);
	d_bind_double(":Response_UserK", nucdata->user_response[K_CHAMB]);

	strncpy(nuclidename, nucdata->name, 7);
	nuclidename[7] = 0;
	trim(nuclidename);
	d_bind_text(":Name", nuclidename);

	strncpy(fullname, nucdata->fullname, 15);
	fullname[15] = 0;
	trim(fullname);
	d_bind_text(":FullName", nucdata->fullname);

	strncpy(code, nucdata->code, 7);
	code[7] = 0;
	trim(code);
	d_bind_text(":Code", nucdata->code);

	d_bind_int(":HLUnit", nucdata->hlunit);
	d_bind_double(":Energy1", nucdata->energy1);
	d_bind_double(":Energy2", nucdata->energy2);
	d_bind_double(":Energy3", nucdata->energy3);
	d_bind_double(":WellEfficiency", nucdata->wellefficiency);
	d_bind_double(":BetaEfficiency", nucdata->betaefficiency);
	d_bind_double(":Probe700Efficiency", nucdata->probe700efficiency);
	d_bind_double(":Well700Efficiency", nucdata->well700efficiency);
	d_bind_double(":UserEnergy1", nucdata->userenergy1);
	d_bind_double(":UserEnergy2", nucdata->userenergy2);
	d_bind_double(":UserEnergy3", nucdata->userenergy3);
	d_bind_double(":UserWellEfficiency", nucdata->userwellefficiency);
	d_bind_double(":UserBetaEfficiency", nucdata->userbetaefficiency);
	d_bind_double(":UserProbe700Efficiency", nucdata->userprobe700efficiency);
	d_bind_double(":UserWell700Efficiency", nucdata->userwell700efficiency);
	stringout(datestring, &(nucdata->wellmeasuredon));
	d_bind_text(":WellMeasuredOn", datestring);
	stringout(datestring, &(nucdata->betameasuredon));
	d_bind_text(":BetaMeasuredOn", datestring);
	stringout(datestring, &(nucdata->probe700measuredon));
	d_bind_text(":Probe700MeasuredOn", datestring);
	stringout(datestring, &(nucdata->well700measuredon));
	d_bind_text(":Well700MeasuredOn", datestring);
	d_bind_int(":Inactive", 0);
	d_step();
	d_finalize();

	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}
/**
 * \details Read the User Nuclide info from EEPROM and create entries in the SQL database in UserNuclide table
 * \returns None
 */
void DB_moveUserNuclideFromEEPROM(void){
	int index, jndex;
	NUCDATA_OBSOLETE nucdata_obsolete, emptyusernuclide;
	NUCDATA nucdata;
	char nuclidename[8], fullname[16], code[8], datestring[30];

	emptyusernuclide.show = 0;
	emptyusernuclide.halflife = -1;
	emptyusernuclide.response[0] = 0.0;
	emptyusernuclide.response[1] = 0.0;
	emptyusernuclide.user_response[0] = 0.0;
	emptyusernuclide.user_response[1] = 0.0;
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

	*capStatus = SQLITE_OK;
	d_command("BEGIN;");

	d_prepare_v2("UPDATE UserNuclide SET Inactive = 1;");
	d_step();
	d_finalize();

	for(index=0; index<10; index++){
		EE_READ(obsolete8[index], (uchar *)&nucdata_obsolete);

		nucdata.show = nucdata_obsolete.show;
		nucdata.halflife = nucdata_obsolete.halflife;
		nucdata.response[R_CHAMB] = nucdata_obsolete.response[R_CHAMB];
		nucdata.response[P_CHAMB] = nucdata_obsolete.response[P_CHAMB];
		nucdata.response[B_CHAMB] = 0.0;
		nucdata.response[ONE_DOT_EIGHT_CHAMB] = 0.0;
		nucdata.response[C_CHAMB] = 0.0;
		nucdata.response[K_CHAMB] = 0.0;
		nucdata.user_response[R_CHAMB] = nucdata_obsolete.user_response[R_CHAMB];
		nucdata.user_response[P_CHAMB] = nucdata_obsolete.user_response[P_CHAMB];
		nucdata.user_response[B_CHAMB] = 0.0;
		nucdata.user_response[ONE_DOT_EIGHT_CHAMB] = 0.0;
		nucdata.user_response[C_CHAMB] = 0.0;
		nucdata.user_response[K_CHAMB] = 0.0;
		for(jndex=0; jndex<7; jndex++) nucdata.name[jndex] = nucdata_obsolete.name[jndex];
		for(jndex=0; jndex<15; jndex++) nucdata.fullname[jndex] = nucdata_obsolete.fullname[jndex];
		for(jndex=0; jndex<7; jndex++) nucdata.code[jndex] = nucdata_obsolete.code[jndex];
		nucdata.hlunit = nucdata_obsolete.hlunit;
		nucdata.energy1 = nucdata_obsolete.energy1;
		nucdata.energy2 = nucdata_obsolete.energy2;
		nucdata.energy3 = nucdata_obsolete.energy3;
		nucdata.wellefficiency = nucdata_obsolete.wellefficiency;
		nucdata.betaefficiency = nucdata_obsolete.betaefficiency;
		nucdata.probe700efficiency = 0.0;
		nucdata.well700efficiency = 0.0;
		nucdata.userenergy1 = nucdata_obsolete.userenergy1;
		nucdata.userenergy2 = nucdata_obsolete.userenergy2;
		nucdata.userenergy3 = nucdata_obsolete.userenergy3;
		nucdata.userwellefficiency = nucdata_obsolete.userwellefficiency;
		nucdata.userbetaefficiency = nucdata_obsolete.userbetaefficiency;
		nucdata.userprobe700efficiency = -1.0;
		nucdata.userwell700efficiency = -1.0;
		nucdata.wellmeasuredon = nucdata_obsolete.wellmeasuredon;
		nucdata.betameasuredon = nucdata_obsolete.betameasuredon;
		nucdata.probe700measuredon = (time_t) 0;
		nucdata.well700measuredon = (time_t) 0;

		//DB_CreateUserNuclide(index, &nucdata);
		d_prepare_v2("INSERT INTO UserNuclide(NucIndexOffset, Show, HalfLife, Response_R, Response_P, Response_B, Response_18, Response_C, Response_K, Response_UserR, Response_UserP, Response_UserB, Response_User18, Response_UserC, Response_UserK, Name, FullName, Code, HLUnit, Energy1, Energy2, Energy3, WellEfficiency, BetaEfficiency, Probe700Efficiency, Well700Efficiency, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEfficiency, UserBetaEfficiency, UserProbe700Efficiency, UserWell700Efficiency, WellMeasuredOn, BetaMeasuredOn, Probe700MeasuredOn, Well700MeasuredOn, Inactive) VALUES(:NucIndexOffset, :Show, :HalfLife, :Response_R, :Response_P, :Response_B, :Response_18, :Response_C, :Response_K, :Response_UserR, :Response_UserP, :Response_UserB, :Response_User18, :Response_UserC, :Response_UserK, :Name, :FullName, :Code, :HLUnit, :Energy1, :Energy2, :Energy3, :WellEfficiency, :BetaEfficiency, :Probe700Efficiency, :Well700Efficiency, :UserEnergy1, :UserEnergy2, :UserEnergy3, :UserWellEfficiency, :UserBetaEfficiency, :UserProbe700Efficiency, :UserWell700Efficiency, julianday(:WellMeasuredOn), julianday(:BetaMeasuredOn), julianday(:Probe700MeasuredOn), julianday(:Well700MeasuredOn), :Inactive);");
		d_bind_int(":NucIndexOffset", index);
		d_bind_int(":Show", nucdata.show);
		d_bind_double(":HalfLife", nucdata.halflife);
		d_bind_double(":Response_R", nucdata.response[R_CHAMB]);
		d_bind_double(":Response_P", nucdata.response[P_CHAMB]);
		d_bind_double(":Response_B", nucdata.response[B_CHAMB]);
		d_bind_double(":Response_18", nucdata.response[ONE_DOT_EIGHT_CHAMB]);
		d_bind_double(":Response_C", nucdata.response[C_CHAMB]);
		d_bind_double(":Response_K", nucdata.response[K_CHAMB]);
		d_bind_double(":Response_UserR", nucdata.user_response[R_CHAMB]);
		d_bind_double(":Response_UserP", nucdata.user_response[P_CHAMB]);
		d_bind_double(":Response_UserB", nucdata.user_response[B_CHAMB]);
		d_bind_double(":Response_User18", nucdata.user_response[ONE_DOT_EIGHT_CHAMB]);
		d_bind_double(":Response_UserC", nucdata.user_response[C_CHAMB]);
		d_bind_double(":Response_UserK", nucdata.user_response[K_CHAMB]);

		strncpy(nuclidename, nucdata.name, 7);
		nuclidename[7] = 0;
		trim(nuclidename);
		d_bind_text(":Name", nuclidename);

		strncpy(fullname, nucdata.fullname, 15);
		fullname[15] = 0;
		trim(fullname);
		d_bind_text(":FullName", fullname);

		strncpy(code, nucdata.code, 7);
		code[7] = 0;
		trim(code);
		d_bind_text(":Code", code);

		d_bind_int(":HLUnit", nucdata.hlunit);
		d_bind_double(":Energy1", nucdata.energy1);
		d_bind_double(":Energy2", nucdata.energy2);
		d_bind_double(":Energy3", nucdata.energy3);
		d_bind_double(":WellEfficiency", nucdata.wellefficiency);
		d_bind_double(":BetaEfficiency", nucdata.betaefficiency);
		d_bind_double(":Probe700Efficiency", nucdata.probe700efficiency);
		d_bind_double(":Well700Efficiency", nucdata.well700efficiency);
		d_bind_double(":UserEnergy1", nucdata.userenergy1);
		d_bind_double(":UserEnergy2", nucdata.userenergy2);
		d_bind_double(":UserEnergy3", nucdata.userenergy3);
		d_bind_double(":UserWellEfficiency", nucdata.userwellefficiency);
		d_bind_double(":UserBetaEfficiency", nucdata.userbetaefficiency);
		d_bind_double(":UserProbe700Efficiency", nucdata.userprobe700efficiency);
		d_bind_double(":UserWell700Efficiency", nucdata.userwell700efficiency);
		stringout(datestring, &(nucdata.wellmeasuredon));
		d_bind_text(":WellMeasuredOn", datestring);
		stringout(datestring, &(nucdata.betameasuredon));
		d_bind_text(":BetaMeasuredOn", datestring);
		stringout(datestring, &(nucdata.probe700measuredon));
		d_bind_text(":Probe700MeasuredOn", datestring);
		stringout(datestring, &(nucdata.well700measuredon));
		d_bind_text(":Well700MeasuredOn", datestring);
		d_bind_int(":Inactive", 0);
		d_step();
		d_finalize();

		EE_WRITE(obsolete8[index], (uchar *) &emptyusernuclide);
		service_watchdog();
	}

	nucdata.show = 0;
	nucdata.halflife = -1;
	nucdata.response[R_CHAMB] = 0.0;
	nucdata.response[P_CHAMB] = 0.0;
	nucdata.response[B_CHAMB] = 0.0;
	nucdata.response[ONE_DOT_EIGHT_CHAMB] = 0.0;
	nucdata.response[C_CHAMB] = 0.0;
	nucdata.response[K_CHAMB] = 0.0;
	nucdata.user_response[R_CHAMB] = 0.0;
	nucdata.user_response[P_CHAMB] = 0.0;
	nucdata.user_response[B_CHAMB] = 0.0;
	nucdata.user_response[ONE_DOT_EIGHT_CHAMB] = 0.0;
	nucdata.user_response[C_CHAMB] = 0.0;
	nucdata.user_response[K_CHAMB] = 0.0;
	nucdata.name[0] = 0;
	nucdata.fullname[0] = 0;
	nucdata.code[0] = 0;
	nucdata.hlunit = -1;
	nucdata.energy1 = 0.0;
	nucdata.energy2 = 0.0;
	nucdata.energy3 = 0.0;
	nucdata.wellefficiency = 0.0;
	nucdata.betaefficiency = 0.0;
	nucdata.probe700efficiency = 0.0;
	nucdata.well700efficiency = 0.0;
	nucdata.userenergy1 = -1.0;
	nucdata.userenergy2 = -1.0;
	nucdata.userenergy3 = -1.0;
	nucdata.userwellefficiency = -1.0;
	nucdata.userbetaefficiency = -1.0;
	nucdata.userprobe700efficiency = -1.0;
	nucdata.userwell700efficiency = -1.0;
	nucdata.wellmeasuredon = (time_t) 0;
	nucdata.betameasuredon = (time_t) 0;
	nucdata.probe700measuredon = (time_t) 0;
	nucdata.well700measuredon = (time_t) 0;
	for(index=10; index<20; index++){
		//DB_CreateUserNuclide(index, &nucdata);
		d_prepare_v2("INSERT INTO UserNuclide(NucIndexOffset, Show, HalfLife, Response_R, Response_P, Response_B, Response_18, Response_C, Response_K, Response_UserR, Response_UserP, Response_UserB, Response_User18, Response_UserC, Response_UserK, Name, FullName, Code, HLUnit, Energy1, Energy2, Energy3, WellEfficiency, BetaEfficiency, Probe700Efficiency, Well700Efficiency, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEfficiency, UserBetaEfficiency, UserProbe700Efficiency, UserWell700Efficiency, WellMeasuredOn, BetaMeasuredOn, Probe700MeasuredOn, Well700MeasuredOn, Inactive) VALUES(:NucIndexOffset, :Show, :HalfLife, :Response_R, :Response_P, :Response_B, :Response_18, :Response_C, :Response_K, :Response_UserR, :Response_UserP, :Response_UserB, :Response_User18, :Response_UserC, :Response_UserK, :Name, :FullName, :Code, :HLUnit, :Energy1, :Energy2, :Energy3, :WellEfficiency, :BetaEfficiency, :Probe700Efficiency, :Well700Efficiency, :UserEnergy1, :UserEnergy2, :UserEnergy3, :UserWellEfficiency, :UserBetaEfficiency, :UserProbe700Efficiency, :UserWell700Efficiency, julianday(:WellMeasuredOn), julianday(:BetaMeasuredOn), julianday(:Probe700MeasuredOn), julianday(:Well700MeasuredOn), :Inactive);");
		d_bind_int(":NucIndexOffset", index);
		d_bind_int(":Show", nucdata.show);
		d_bind_double(":HalfLife", nucdata.halflife);
		d_bind_double(":Response_R", nucdata.response[R_CHAMB]);
		d_bind_double(":Response_P", nucdata.response[P_CHAMB]);
		d_bind_double(":Response_B", nucdata.response[B_CHAMB]);
		d_bind_double(":Response_18", nucdata.response[ONE_DOT_EIGHT_CHAMB]);
		d_bind_double(":Response_C", nucdata.response[C_CHAMB]);
		d_bind_double(":Response_K", nucdata.response[K_CHAMB]);
		d_bind_double(":Response_UserR", nucdata.user_response[R_CHAMB]);
		d_bind_double(":Response_UserP", nucdata.user_response[P_CHAMB]);
		d_bind_double(":Response_UserB", nucdata.user_response[B_CHAMB]);
		d_bind_double(":Response_User18", nucdata.user_response[ONE_DOT_EIGHT_CHAMB]);
		d_bind_double(":Response_UserC", nucdata.user_response[C_CHAMB]);
		d_bind_double(":Response_UserK", nucdata.user_response[K_CHAMB]);
		d_bind_text(":Name", nucdata.name);
		d_bind_text(":FullName", nucdata.fullname);
		d_bind_text(":Code", nucdata.code);
		d_bind_int(":HLUnit", nucdata.hlunit);
		d_bind_double(":Energy1", nucdata.energy1);
		d_bind_double(":Energy2", nucdata.energy2);
		d_bind_double(":Energy3", nucdata.energy3);
		d_bind_double(":WellEfficiency", nucdata.wellefficiency);
		d_bind_double(":BetaEfficiency", nucdata.betaefficiency);
		d_bind_double(":Probe700Efficiency", nucdata.probe700efficiency);
		d_bind_double(":Well700Efficiency", nucdata.well700efficiency);
		d_bind_double(":UserEnergy1", nucdata.userenergy1);
		d_bind_double(":UserEnergy2", nucdata.userenergy2);
		d_bind_double(":UserEnergy3", nucdata.userenergy3);
		d_bind_double(":UserWellEfficiency", nucdata.userwellefficiency);
		d_bind_double(":UserBetaEfficiency", nucdata.userbetaefficiency);
		d_bind_double(":UserProbe700Efficiency", nucdata.userprobe700efficiency);
		d_bind_double(":UserWell700Efficiency", nucdata.userwell700efficiency);
		stringout(datestring, &(nucdata.wellmeasuredon));
		d_bind_text(":WellMeasuredOn", datestring);
		stringout(datestring, &(nucdata.betameasuredon));
		d_bind_text(":BetaMeasuredOn", datestring);
		stringout(datestring, &(nucdata.probe700measuredon));
		d_bind_text(":Probe700MeasuredOn", datestring);
		stringout(datestring, &(nucdata.well700measuredon));
		d_bind_text(":Well700MeasuredOn", datestring);
		d_bind_int(":Inactive", 0);
		d_step();
		d_finalize();
		service_watchdog();
	}

	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}
/**
 * \details Insert Empty User Nuclide entries into the UserNuclide table
 * \param recreate True = Display Amulet status strings, False = No status string output
 * \returns None
 */
void DB_createEmptyUserNuclide(bool recreate){
	int index;
	NUCDATA nucdata;
	char strng[40];
	char datestring[30];

	nucdata.show = 0;
	nucdata.halflife = -1;
	nucdata.response[R_CHAMB] = 0.0;
	nucdata.response[P_CHAMB] = 0.0;
	nucdata.response[B_CHAMB] = 0.0;
	nucdata.response[ONE_DOT_EIGHT_CHAMB] = 0.0;
	nucdata.response[C_CHAMB] = 0.0;
	nucdata.response[K_CHAMB] = 0.0;
	nucdata.user_response[R_CHAMB] = 0.0;
	nucdata.user_response[P_CHAMB] = 0.0;
	nucdata.user_response[B_CHAMB] = 0.0;
	nucdata.user_response[ONE_DOT_EIGHT_CHAMB] = 0.0;
	nucdata.user_response[C_CHAMB] = 0.0;
	nucdata.user_response[K_CHAMB] = 0.0;
	nucdata.name[0] = 0;
	nucdata.fullname[0] = 0;
	nucdata.code[0] = 0;
	nucdata.hlunit = -1;
	nucdata.energy1 = 0.0;
	nucdata.energy2 = 0.0;
	nucdata.energy3 = 0.0;
	nucdata.wellefficiency = 0.0;
	nucdata.betaefficiency = 0.0;
	nucdata.probe700efficiency = 0.0;
	nucdata.well700efficiency = 0.0;
	nucdata.userenergy1 = -1.0;
	nucdata.userenergy2 = -1.0;
	nucdata.userenergy3 = -1.0;
	nucdata.userwellefficiency = -1.0;
	nucdata.userbetaefficiency = -1.0;
	nucdata.userprobe700efficiency = -1.0;
	nucdata.userwell700efficiency = -1.0;
	nucdata.wellmeasuredon = (time_t) 0;
	nucdata.betameasuredon = (time_t) 0;
	nucdata.probe700measuredon = (time_t) 0;
	nucdata.well700measuredon = (time_t) 0;

	*capStatus = SQLITE_OK;
	d_command("BEGIN;");

	d_prepare_v2("UPDATE UserNuclide SET Inactive = 1;");
	d_step();
	d_finalize();

	for(index=0; index<20; index++){

		if(recreate){
			sprintf(strng, "Default User Nuclide %d", index + 1);
			SetAmuletString(102, strng);
			SetAmuletByte(100, 0xFF);
		}
		//DB_CreateUserNuclide(index, &nucdata);
		d_prepare_v2("INSERT INTO UserNuclide(NucIndexOffset, Show, HalfLife, Response_R, Response_P, Response_B, Response_18, Response_C, Response_K, Response_UserR, Response_UserP, Response_UserB, Response_User18, Response_UserC, Response_UserK, Name, FullName, Code, HLUnit, Energy1, Energy2, Energy3, WellEfficiency, BetaEfficiency, Probe700Efficiency, Well700Efficiency, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEfficiency, UserBetaEfficiency, UserProbe700Efficiency, UserWell700Efficiency, WellMeasuredOn, BetaMeasuredOn, Probe700MeasuredOn, Well700MeasuredOn, Inactive) VALUES(:NucIndexOffset, :Show, :HalfLife, :Response_R, :Response_P, :Response_B, :Response_18, :Response_C, :Response_K, :Response_UserR, :Response_UserP, :Response_UserB, :Response_User18, :Response_UserC, :Response_UserK, :Name, :FullName, :Code, :HLUnit, :Energy1, :Energy2, :Energy3, :WellEfficiency, :BetaEfficiency, :Probe700Efficiency, :Well700Efficiency, :UserEnergy1, :UserEnergy2, :UserEnergy3, :UserWellEfficiency, :UserBetaEfficiency, :UserProbe700Efficiency, :UserWell700Efficiency, julianday(:WellMeasuredOn), julianday(:BetaMeasuredOn), julianday(:Probe700MeasuredOn), julianday(:Well700MeasuredOn), :Inactive);");
		d_bind_int(":NucIndexOffset", index);
		d_bind_int(":Show", nucdata.show);
		d_bind_double(":HalfLife", nucdata.halflife);
		d_bind_double(":Response_R", nucdata.response[R_CHAMB]);
		d_bind_double(":Response_P", nucdata.response[P_CHAMB]);
		d_bind_double(":Response_B", nucdata.response[B_CHAMB]);
		d_bind_double(":Response_18", nucdata.response[ONE_DOT_EIGHT_CHAMB]);
		d_bind_double(":Response_C", nucdata.response[C_CHAMB]);
		d_bind_double(":Response_K", nucdata.response[K_CHAMB]);
		d_bind_double(":Response_UserR", nucdata.user_response[R_CHAMB]);
		d_bind_double(":Response_UserP", nucdata.user_response[P_CHAMB]);
		d_bind_double(":Response_UserB", nucdata.user_response[B_CHAMB]);
		d_bind_double(":Response_User18", nucdata.user_response[ONE_DOT_EIGHT_CHAMB]);
		d_bind_double(":Response_UserC", nucdata.user_response[C_CHAMB]);
		d_bind_double(":Response_UserK", nucdata.user_response[K_CHAMB]);
		d_bind_text(":Name", nucdata.name);
		d_bind_text(":FullName", nucdata.fullname);
		d_bind_text(":Code", nucdata.code);
		d_bind_int(":HLUnit", nucdata.hlunit);
		d_bind_double(":Energy1", nucdata.energy1);
		d_bind_double(":Energy2", nucdata.energy2);
		d_bind_double(":Energy3", nucdata.energy3);
		d_bind_double(":WellEfficiency", nucdata.wellefficiency);
		d_bind_double(":BetaEfficiency", nucdata.betaefficiency);
		d_bind_double(":Probe700Efficiency", nucdata.probe700efficiency);
		d_bind_double(":Well700Efficiency", nucdata.well700efficiency);
		d_bind_double(":UserEnergy1", nucdata.userenergy1);
		d_bind_double(":UserEnergy2", nucdata.userenergy2);
		d_bind_double(":UserEnergy3", nucdata.userenergy3);
		d_bind_double(":UserWellEfficiency", nucdata.userwellefficiency);
		d_bind_double(":UserBetaEfficiency", nucdata.userbetaefficiency);
		d_bind_double(":UserProbe700Efficiency", nucdata.userprobe700efficiency);
		d_bind_double(":UserWell700Efficiency", nucdata.userwell700efficiency);
		stringout(datestring, &(nucdata.wellmeasuredon));
		d_bind_text(":WellMeasuredOn", datestring);
		stringout(datestring, &(nucdata.betameasuredon));
		d_bind_text(":BetaMeasuredOn", datestring);
		stringout(datestring, &(nucdata.probe700measuredon));
		d_bind_text(":Probe700MeasuredOn", datestring);
		stringout(datestring, &(nucdata.well700measuredon));
		d_bind_text(":Well700MeasuredOn", datestring);
		d_bind_int(":Inactive", 0);
		d_step();
		d_finalize();

		service_watchdog();
	}
	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}
/**
 * \details Read all User Nuclide info from the UserNuclide table
 * \param nucData Pointer to NUCDATA array, which will receive the User Nuclide info
 * \returns None
 */
void DB_ReadAllUserNuclide(NUCDATA *nucData){
	char MeasuredOn[40];
	int index, nucIndexOffset;
	NUCDATA empty;
	//bool found[20];

	empty.show = 0;
	empty.halflife = -1;
	empty.response[R_CHAMB] = 0.0;
	empty.response[P_CHAMB] = 0.0;
	empty.response[B_CHAMB] = 0.0;
	empty.response[ONE_DOT_EIGHT_CHAMB] = 0.0;
	empty.response[C_CHAMB] = 0.0;
	empty.response[K_CHAMB] = 0.0;
	empty.user_response[R_CHAMB] = 0.0;
	empty.user_response[P_CHAMB] = 0.0;
	empty.user_response[B_CHAMB] = 0.0;
	empty.user_response[ONE_DOT_EIGHT_CHAMB] = 0.0;
	empty.user_response[C_CHAMB] = 0.0;
	empty.user_response[K_CHAMB] = 0.0;
	empty.name[0] = 0;
	empty.fullname[0] = 0;
	empty.code[0] = 0;
	empty.hlunit = -1;
	empty.energy1 = 0.0;
	empty.energy2 = 0.0;
	empty.energy3 = 0.0;
	empty.wellefficiency = 0.0;
	empty.betaefficiency = 0.0;
	empty.probe700efficiency = 0.0;
	empty.well700efficiency = 0.0;
	empty.userenergy1 = -1.0;
	empty.userenergy2 = -1.0;
	empty.userenergy3 = -1.0;
	empty.userwellefficiency = -1.0;
	empty.userbetaefficiency = -1.0;
	empty.userprobe700efficiency = -1.0;
	empty.userwell700efficiency = -1.0;
	empty.wellmeasuredon = (time_t) 0;
	empty.betameasuredon = (time_t) 0;
	empty.probe700measuredon = (time_t) 0;
	empty.well700measuredon = (time_t) 0;

	for(index=0; index<20; index++) memcpy(&(nucData[index]), &empty, sizeof(NUCDATA));

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT Show, HalfLife, Response_R, Response_P, Response_B, Response_18, Response_C, Response_K, Response_UserR, Response_UserP, Response_UserB, Response_User18, Response_UserC, Response_UserK, Name, FullName, Code, HLUnit, Energy1, Energy2, Energy3, WellEfficiency, BetaEfficiency, Probe700Efficiency, Well700Efficiency, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEfficiency, UserBetaEfficiency, UserProbe700Efficiency, UserWell700Efficiency, datetime(WellMeasuredOn), datetime(BetaMeasuredOn), datetime(Probe700MeasuredOn), datetime(Well700MeasuredOn), NucIndexOffset FROM UserNuclide WHERE Inactive = 0 AND HLUnit != -1;");
	d_step();
	if(*capStatus == SQLITE_ROW){
		while(*capStatus == SQLITE_ROW){
			nucIndexOffset = d_column_int(36);
			if(nucIndexOffset < 20){
				nucData[nucIndexOffset].show = d_column_int(0);//Show,
				nucData[nucIndexOffset].halflife = d_column_double(1);//HalfLife,
				nucData[nucIndexOffset].response[R_CHAMB] = d_column_double(2);//Response_R,
				nucData[nucIndexOffset].response[P_CHAMB] = d_column_double(3);//Response_P,
				nucData[nucIndexOffset].response[B_CHAMB] = d_column_double(4);//Response_B,
				nucData[nucIndexOffset].response[ONE_DOT_EIGHT_CHAMB] = d_column_double(5);//Response_18,
				nucData[nucIndexOffset].response[C_CHAMB] = d_column_double(6);//Response_C,
				nucData[nucIndexOffset].response[K_CHAMB] = d_column_double(7);//Response_K,
				nucData[nucIndexOffset].user_response[R_CHAMB] = d_column_double(8);//Response_UserR,
				nucData[nucIndexOffset].user_response[P_CHAMB] = d_column_double(9);//Response_UserP,
				nucData[nucIndexOffset].user_response[B_CHAMB] = d_column_double(10);//Response_UserB,
				nucData[nucIndexOffset].user_response[ONE_DOT_EIGHT_CHAMB] = d_column_double(11);//Response_User18,
				nucData[nucIndexOffset].user_response[C_CHAMB] = d_column_double(12);//Response_UserC,
				nucData[nucIndexOffset].user_response[K_CHAMB] = d_column_double(13);//Response_UserK,
				strncpy(nucData[nucIndexOffset].name, (const char *) d_column_text(14), 7);//Name,
				strncpy(nucData[nucIndexOffset].fullname, (const char *) d_column_text(15), 15);//FullName,
				strncpy(nucData[nucIndexOffset].code, (const char *) d_column_text(16), 7);//Code,
				nucData[nucIndexOffset].hlunit = d_column_int(17);//HLUnit,
				nucData[nucIndexOffset].energy1 = d_column_double(18);//Energy1,
				nucData[nucIndexOffset].energy2 = d_column_double(19);//Energy2,
				nucData[nucIndexOffset].energy3 = d_column_double(20);//Energy3,
				nucData[nucIndexOffset].wellefficiency = d_column_double(21);//WellEfficiency,
				nucData[nucIndexOffset].betaefficiency = d_column_double(22);//BetaEfficiency,
				nucData[nucIndexOffset].probe700efficiency = d_column_double(23);//Probe700Efficiency,
				nucData[nucIndexOffset].well700efficiency = d_column_double(24);//Well700Efficiency,
				nucData[nucIndexOffset].userenergy1 = d_column_double(25);//UserEnergy1,
				nucData[nucIndexOffset].userenergy2 = d_column_double(26);//UserEnergy2,
				nucData[nucIndexOffset].userenergy3 = d_column_double(27);//UserEnergy3,
				nucData[nucIndexOffset].userwellefficiency = d_column_double(28);//UserWellEfficiency,
				nucData[nucIndexOffset].userbetaefficiency = d_column_double(29);//UserBetaEfficiency,
				nucData[nucIndexOffset].userprobe700efficiency = d_column_double(30);//UserProbe700Efficiency,
				nucData[nucIndexOffset].userwell700efficiency = d_column_double(31);//UserWell700Efficiency,
				strcpy(MeasuredOn, (const char *) d_column_text(32));
				nucData[nucIndexOffset].wellmeasuredon = strtotime_t(MeasuredOn);//WellMeasuredOn,
				strcpy(MeasuredOn, (const char *) d_column_text(33));
				nucData[nucIndexOffset].betameasuredon = strtotime_t(MeasuredOn);//BetaMeasuredOn,
				strcpy(MeasuredOn, (const char *) d_column_text(34));
				nucData[nucIndexOffset].probe700measuredon = strtotime_t(MeasuredOn);//Probe700MeasuredOn,
				strcpy(MeasuredOn, (const char *) d_column_text(35));
				nucData[nucIndexOffset].well700measuredon = strtotime_t(MeasuredOn);//Well700MeasuredOn
			}
			d_step();
		}
	}
	d_finalize();
}
/**
 * \details Read a User Nuclide from the UserNuclide table
 * \param nucindexoffset Offset in the nuclideData table
 * \param nucdata Pointer to NUCDATA structure, which will receive the User Nuclide info
 * \returns None
 */
void DB_ReadUserNuclide(int nucindexoffset, NUCDATA *nucdata){
	char MeasuredOn[40];

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT Show, HalfLife, Response_R, Response_P, Response_B, Response_18, Response_C, Response_K, Response_UserR, Response_UserP, Response_UserB, Response_User18, Response_UserC, Response_UserK, Name, FullName, Code, HLUnit, Energy1, Energy2, Energy3, WellEfficiency, BetaEfficiency, Probe700Efficiency, Well700Efficiency, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEfficiency, UserBetaEfficiency, UserProbe700Efficiency, UserWell700Efficiency, datetime(WellMeasuredOn), datetime(BetaMeasuredOn), datetime(Probe700MeasuredOn), datetime(Well700MeasuredOn) FROM UserNuclide WHERE Inactive = 0 AND NucIndexOffset = :NucIndexOffset;");
	d_bind_int(":NucIndexOffset", nucindexoffset);
	d_step();
	if(*capStatus == SQLITE_ROW){
		nucdata->show = d_column_int(0);//Show,
		nucdata->halflife = d_column_double(1);//HalfLife,
		nucdata->response[R_CHAMB] = d_column_double(2);//Response_R,
		nucdata->response[P_CHAMB] = d_column_double(3);//Response_P,
		nucdata->response[B_CHAMB] = d_column_double(4);//Response_B,
		nucdata->response[ONE_DOT_EIGHT_CHAMB] = d_column_double(5);//Response_18,
		nucdata->response[C_CHAMB] = d_column_double(6);//Response_C,
		nucdata->response[K_CHAMB] = d_column_double(7);//Response_K,
		nucdata->user_response[R_CHAMB] = d_column_double(8);//Response_UserR,
		nucdata->user_response[P_CHAMB] = d_column_double(9);//Response_UserP,
		nucdata->user_response[B_CHAMB] = d_column_double(10);//Response_UserB,
		nucdata->user_response[ONE_DOT_EIGHT_CHAMB] = d_column_double(11);//Response_User18,
		nucdata->user_response[C_CHAMB] = d_column_double(12);//Response_UserC,
		nucdata->user_response[K_CHAMB] = d_column_double(13);//Response_UserK,
		strncpy(nucdata->name, (const char *) d_column_text(14), 7);//Name,
		strncpy(nucdata->fullname, (const char *) d_column_text(15), 15);//FullName,
		strncpy(nucdata->code, (const char *) d_column_text(16), 7);//Code,
		nucdata->hlunit = d_column_int(17);//HLUnit,
		nucdata->energy1 = d_column_double(18);//Energy1,
		nucdata->energy2 = d_column_double(19);//Energy2,
		nucdata->energy3 = d_column_double(20);//Energy3,
		nucdata->wellefficiency = d_column_double(21);//WellEfficiency,
		nucdata->betaefficiency = d_column_double(22);//BetaEfficiency,
		nucdata->probe700efficiency = d_column_double(23);//Probe700Efficiency,
		nucdata->well700efficiency = d_column_double(24);//Well700Efficiency,
		nucdata->userenergy1 = d_column_double(25);//UserEnergy1,
		nucdata->userenergy2 = d_column_double(26);//UserEnergy2,
		nucdata->userenergy3 = d_column_double(27);//UserEnergy3,
		nucdata->userwellefficiency = d_column_double(28);//UserWellEfficiency,
		nucdata->userbetaefficiency = d_column_double(29);//UserBetaEfficiency,
		nucdata->userprobe700efficiency = d_column_double(30);//UserProbe700Efficiency,
		nucdata->userwell700efficiency = d_column_double(31);//UserWell700Efficiency,
		strcpy(MeasuredOn, (const char *) d_column_text(32));
		nucdata->wellmeasuredon = strtotime_t(MeasuredOn);//WellMeasuredOn,
		strcpy(MeasuredOn, (const char *) d_column_text(33));
		nucdata->betameasuredon = strtotime_t(MeasuredOn);//BetaMeasuredOn,
		strcpy(MeasuredOn, (const char *) d_column_text(34));
		nucdata->probe700measuredon = strtotime_t(MeasuredOn);//Probe700MeasuredOn,
		strcpy(MeasuredOn, (const char *) d_column_text(35));
		nucdata->well700measuredon = strtotime_t(MeasuredOn);//Well700MeasuredOn
		d_finalize();
	}else{
		d_finalize();
		nucdata->show = 0;
		nucdata->halflife = -1;
		nucdata->response[R_CHAMB] = 0.0;
		nucdata->response[P_CHAMB] = 0.0;
		nucdata->response[B_CHAMB] = 0.0;
		nucdata->response[ONE_DOT_EIGHT_CHAMB] = 0.0;
		nucdata->response[C_CHAMB] = 0.0;
		nucdata->response[K_CHAMB] = 0.0;
		nucdata->user_response[R_CHAMB] = 0.0;
		nucdata->user_response[P_CHAMB] = 0.0;
		nucdata->user_response[B_CHAMB] = 0.0;
		nucdata->user_response[ONE_DOT_EIGHT_CHAMB] = 0.0;
		nucdata->user_response[C_CHAMB] = 0.0;
		nucdata->user_response[K_CHAMB] = 0.0;
		nucdata->name[0] = 0;
		nucdata->fullname[0] = 0;
		nucdata->code[0] = 0;
		nucdata->hlunit = -1;
		nucdata->energy1 = 0.0;
		nucdata->energy2 = 0.0;
		nucdata->energy3 = 0.0;
		nucdata->wellefficiency = 0.0;
		nucdata->betaefficiency = 0.0;
		nucdata->probe700efficiency = 0.0;
		nucdata->well700efficiency = 0.0;
		nucdata->userenergy1 = -1.0;
		nucdata->userenergy2 = -1.0;
		nucdata->userenergy3 = -1.0;
		nucdata->userwellefficiency = -1.0;
		nucdata->userbetaefficiency = -1.0;
		nucdata->userprobe700efficiency = -1.0;
		nucdata->userwell700efficiency = -1.0;
		nucdata->wellmeasuredon = (time_t) 0;
		nucdata->betameasuredon = (time_t) 0;
		nucdata->probe700measuredon = (time_t) 0;
		nucdata->well700measuredon = (time_t) 0;

		DB_CreateUserNuclide(nucindexoffset, nucdata);
	}
}
/**
 * \details Create an entry in the UserCal table
 * \param userCalIndexOffset Offset in the nuclideData table
 * \param userCal USERCAL structure containing the User Cal info
 * \returns None
 */
void DB_CreateUserCal(int userCalIndexOffset, USERCAL *userCal){
	*capStatus = SQLITE_OK;
	d_command("BEGIN;");

	d_prepare_v2("UPDATE UserCal SET Inactive = 1 WHERE UserCalIndexOffset = :UserCalIndexOffset;");
	d_bind_int(":UserCalIndexOffset", userCalIndexOffset);
	d_step();
	d_finalize();

	d_prepare_v2("INSERT INTO UserCal(UserCalIndexOffset, NuclideIndex, ChamberType, Response, Inactive) VALUES(:UserCalIndexOffset, :NuclideIndex, :ChamberType, :Response, :Inactive);");
	d_bind_int(":UserCalIndexOffset", userCalIndexOffset);
	d_bind_int(":NuclideIndex", userCal->nuc_index);
	d_bind_int(":ChamberType", userCal->ch_type);
	d_bind_double(":Response", userCal->response);
	d_bind_int(":Inactive", 0);
	d_step();
	d_finalize();

	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}
/**
 * \details Read the User Cal info from EEPROM and create entries in the SQL database in the UserCal table
 * \returns None
 */
void DB_moveUserCalFromEEPROM(void){
	int index;
	USERCAL emptyusercal, usercal;

	emptyusercal.ch_type = 0;
	emptyusercal.response = 0.0;
	emptyusercal.nuc_index = -1;

	*capStatus = SQLITE_OK;
	d_command("BEGIN;");

	d_prepare_v2("UPDATE UserCal SET Inactive = 1;");
	d_step();
	d_finalize();

	for(index=0; index<MAX_NEW_CAL_OBSOLETE; index++){
		EE_READ(obsolete9[index], (uchar *) &usercal);
		//DB_CreateUserCal(index, &usercal);
		d_prepare_v2("INSERT INTO UserCal(UserCalIndexOffset, NuclideIndex, ChamberType, Response, Inactive) VALUES(:UserCalIndexOffset, :NuclideIndex, :ChamberType, :Response, :Inactive);");
		d_bind_int(":UserCalIndexOffset", index);
		d_bind_int(":NuclideIndex", usercal.nuc_index);
		d_bind_int(":ChamberType", usercal.ch_type);
		d_bind_double(":Response", usercal.response);
		d_bind_int(":Inactive", 0);
		d_step();
		d_finalize();
		EE_WRITE(obsolete9[index], (uchar *) &emptyusercal);
		service_watchdog();
	}

	for(index=MAX_NEW_CAL_OBSOLETE; index<MAX_NEW_CAL; index++){
		//DB_CreateUserCal(index, &emptyusercal);
		d_prepare_v2("INSERT INTO UserCal(UserCalIndexOffset, NuclideIndex, ChamberType, Response, Inactive) VALUES(:UserCalIndexOffset, :NuclideIndex, :ChamberType, :Response, :Inactive);");
		d_bind_int(":UserCalIndexOffset", index);
		d_bind_int(":NuclideIndex", emptyusercal.nuc_index);
		d_bind_int(":ChamberType", emptyusercal.ch_type);
		d_bind_double(":Response", emptyusercal.response);
		d_bind_int(":Inactive", 0);
		d_step();
		d_finalize();
		service_watchdog();
	}

	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}
/**
 * \details Insert Empty User Cal entries into the UserCal table
 * \param recreate True = Display Amulet status strings, False = No status string output
 * \returns None
 */
void DB_createEmptyUserCal(bool recreate){
	int index;
	USERCAL emptyusercal;
	char strng[40];

	emptyusercal.ch_type = 0;
	emptyusercal.response = 0.0;
	emptyusercal.nuc_index = -1;

	*capStatus = SQLITE_OK;
	d_command("BEGIN;");

	d_prepare_v2("UPDATE UserCal SET Inactive = 1;");
	d_step();
	d_finalize();

	for(index=0; index<MAX_NEW_CAL; index++){
		if(recreate){
			sprintf(strng, "Default User Cal %d", index + 1);
			SetAmuletString(102, strng);
			SetAmuletByte(100, 0xFF);
		}
		//DB_CreateUserCal(index, &emptyusercal);
		d_prepare_v2("INSERT INTO UserCal(UserCalIndexOffset, NuclideIndex, ChamberType, Response, Inactive) VALUES(:UserCalIndexOffset, :NuclideIndex, :ChamberType, :Response, :Inactive);");
		d_bind_int(":UserCalIndexOffset", index);
		d_bind_int(":NuclideIndex", emptyusercal.nuc_index);
		d_bind_int(":ChamberType", emptyusercal.ch_type);
		d_bind_double(":Response", emptyusercal.response);
		d_bind_int(":Inactive", 0);
		d_step();
		d_finalize();
		service_watchdog();
	}

	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}
/**
 * \details Read all User Cal info from the UserCal table
 * \param userCal Pointer to USERCAL array, which will receive the User Cal info
 * \returns None
 */
void DB_ReadAllUserCal(USERCAL *userCal){
	int index, userCalIndexOffset;
	USERCAL empty;

	empty.nuc_index = -1;
	empty.ch_type = 0;
	empty.response = 0;
	for(index=0; index<MAX_NEW_CAL; index++) memcpy(&(userCal[index]), &empty, sizeof(USERCAL));

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT NuclideIndex, ChamberType, Response, UserCalIndexOffset FROM UserCal WHERE Inactive = 0 AND NuclideIndex != -1;");
	d_step();
	if(*capStatus == SQLITE_ROW){
		while(*capStatus == SQLITE_ROW){
			userCalIndexOffset = d_column_int(3);
			if(userCalIndexOffset < MAX_NEW_CAL){
				userCal[userCalIndexOffset].nuc_index = d_column_int(0);//NuclideIndex
				userCal[userCalIndexOffset].ch_type = d_column_int(1);//ChamberType
				userCal[userCalIndexOffset].response = d_column_double(2);//Response
			}
			d_step();
		}
	}
	d_finalize();
}
/**
 * \details Read a User Cal from the UserCal table
 * \param userCalIndexOffset Offset in the nuclideData table
 * \param userCal Pointer to USERCAL structure, which will receive the User Cal info
 * \returns None
 */
void DB_ReadUserCal(int userCalIndexOffset, USERCAL *userCal){
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT NuclideIndex, ChamberType, Response FROM UserCal WHERE Inactive = 0 AND UserCalIndexOffset = :UserCalIndexOffset;");
	d_bind_int(":UserCalIndexOffset", userCalIndexOffset);
	d_step();
	if(*capStatus == SQLITE_ROW){
		userCal->nuc_index = d_column_int(0);//NuclideIndex
		userCal->ch_type = d_column_int(1);//ChamberType
		userCal->response = d_column_double(2);//Response
		d_finalize();
	}else{
		d_finalize();
		userCal->nuc_index = -1;
		userCal->ch_type = 0;
		userCal->response = 0.0;
		DB_CreateUserCal(userCalIndexOffset, userCal);
	}
}

void DB_CreateUserEff(int userEffIndexOffset, SETUP_EFFICIENCY *setupEfficiency){
	char datestring[30];
	*capStatus = SQLITE_OK;
	d_command("BEGIN;");

	d_prepare_v2("UPDATE UserEff SET Inactive = 1 WHERE UserEffIndexOffset = :UserEffIndexOffset;");
	d_bind_int(":UserEffIndexOffset", userEffIndexOffset);
	d_step();
	d_finalize();

	d_prepare_v2("INSERT INTO UserEff(UserEffIndexOffset, NuclideIndex, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEff, UserBetaEff, UserProbe700Eff, UserWell700Eff, WellMeasuredOn, BetaMeasuredOn, Probe700MeasuredOn, Well700MeasuredOn, Inactive) VALUES(:UserEffIndexOffset, :NuclideIndex, :UserEnergy1, :UserEnergy2, :UserEnergy3, :UserWellEff, :UserBetaEff, :UserProbe700Eff, :UserWell700Eff, julianday(:WellMeasuredOn), julianday(:BetaMeasuredOn), julianday(:Probe700MeasuredOn), julianday(:Well700MeasuredOn), :Inactive);");
	d_bind_int(":UserEffIndexOffset", userEffIndexOffset);
	d_bind_int(":NuclideIndex", setupEfficiency->nuclideID);
	d_bind_double(":UserEnergy1", setupEfficiency->userEnergy1);
	d_bind_double(":UserEnergy2", setupEfficiency->userEnergy2);
	d_bind_double(":UserEnergy3", setupEfficiency->userEnergy3);
	d_bind_double(":UserWellEff", setupEfficiency->userWellEff);
	d_bind_double(":UserBetaEff", setupEfficiency->userBetaEff);
	d_bind_double(":UserProbe700Eff", setupEfficiency->userProbe700Eff);
	d_bind_double(":UserWell700Eff", setupEfficiency->userWell700Eff);
	stringout(datestring, &(setupEfficiency->wellStamp));
	d_bind_text(":WellMeasuredOn", datestring);
	stringout(datestring, &(setupEfficiency->betaStamp));
	d_bind_text(":BetaMeasuredOn", datestring);
	stringout(datestring, &(setupEfficiency->probe700Stamp));
	d_bind_text(":Probe700MeasuredOn", datestring);
	stringout(datestring, &(setupEfficiency->well700Stamp));
	d_bind_text(":Well700MeasuredOn", datestring);
	d_bind_int(":Inactive", 0);
	d_step();
	d_finalize();

	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}

void DB_moveUserEffFromFile(bool recreate){
	SETUP_EFFICIENCY emptySetupEff, setupEff;
	FIL fileObject;
	FILINFO fileInfo;
	char longFileName[100];
	UINT bytesRead;
	long lFileSize, lObsoleteSize;
	int index;
	char strng[40];
	char datestring[30];

	emptySetupEff.nuclideID = -1;
	emptySetupEff.userEnergy1 = -1;
	emptySetupEff.userEnergy2 = -1;
	emptySetupEff.userEnergy3 = -1;
	emptySetupEff.userWellEff = -1;
	emptySetupEff.userBetaEff = -1;
	emptySetupEff.userProbe700Eff = -1;
	emptySetupEff.userWell700Eff = -1;
	emptySetupEff.wellStamp = (time_t) 0;
	emptySetupEff.betaStamp = (time_t) 0;
	emptySetupEff.probe700Stamp = (time_t) 0;
	emptySetupEff.well700Stamp = (time_t) 0;

	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	if(f_stat("usereff.bin", &fileInfo) == FR_OK){
		lFileSize = fileInfo.fsize;
		lObsoleteSize = sizeof(SETUP_EFFICIENCY_OBSOLETE);
		if(lFileSize > 0){
			f_open(&fileObject, "usereff.bin", FA_READ);
			f_read(&fileObject, userEfficiencyObsolete, lFileSize, &bytesRead);
			f_close(&fileObject);

			lFileSize /= lObsoleteSize;

			*capStatus = SQLITE_OK;
			d_command("BEGIN;");

			d_prepare_v2("UPDATE UserEff SET Inactive = 1;");
			d_step();
			d_finalize();

			for(index=0; index<lFileSize; index++){
				if(recreate){
					sprintf(strng, "User Eff %d", index + 1);
					SetAmuletString(102, strng);
					SetAmuletByte(100, 0xFF);
				}
				if(userEfficiencyObsolete[index].nuclideID == -1){
					//DB_CreateUserEff(index, &emptySetupEff);
					d_prepare_v2("INSERT INTO UserEff(UserEffIndexOffset, NuclideIndex, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEff, UserBetaEff, UserProbe700Eff, UserWell700Eff, WellMeasuredOn, BetaMeasuredOn, Probe700MeasuredOn, Well700MeasuredOn, Inactive) VALUES(:UserEffIndexOffset, :NuclideIndex, :UserEnergy1, :UserEnergy2, :UserEnergy3, :UserWellEff, :UserBetaEff, :UserProbe700Eff, :UserWell700Eff, julianday(:WellMeasuredOn), julianday(:BetaMeasuredOn), julianday(:Probe700MeasuredOn), julianday(:Well700MeasuredOn), :Inactive);");
					d_bind_int(":UserEffIndexOffset", index);
					d_bind_int(":NuclideIndex", emptySetupEff.nuclideID);
					d_bind_double(":UserEnergy1", emptySetupEff.userEnergy1);
					d_bind_double(":UserEnergy2", emptySetupEff.userEnergy2);
					d_bind_double(":UserEnergy3", emptySetupEff.userEnergy3);
					d_bind_double(":UserWellEff", emptySetupEff.userWellEff);
					d_bind_double(":UserBetaEff", emptySetupEff.userBetaEff);
					d_bind_double(":UserProbe700Eff", emptySetupEff.userProbe700Eff);
					d_bind_double(":UserWell700Eff", emptySetupEff.userWell700Eff);
					stringout(datestring, &(emptySetupEff.wellStamp));
					d_bind_text(":WellMeasuredOn", datestring);
					stringout(datestring, &(emptySetupEff.betaStamp));
					d_bind_text(":BetaMeasuredOn", datestring);
					stringout(datestring, &(emptySetupEff.probe700Stamp));
					d_bind_text(":Probe700MeasuredOn", datestring);
					stringout(datestring, &(emptySetupEff.well700Stamp));
					d_bind_text(":Well700MeasuredOn", datestring);
					d_bind_int(":Inactive", 0);
					d_step();
					d_finalize();
				}else{
					setupEff.nuclideID = userEfficiencyObsolete[index].nuclideID;
					setupEff.userEnergy1 = userEfficiencyObsolete[index].userEnergy1;
					setupEff.userEnergy2 = userEfficiencyObsolete[index].userEnergy2;
					setupEff.userEnergy3 = userEfficiencyObsolete[index].userEnergy3;
					setupEff.userWellEff = userEfficiencyObsolete[index].userWellEff;
					setupEff.userBetaEff = userEfficiencyObsolete[index].userBetaEff;
					setupEff.userProbe700Eff = -1;
					setupEff.userWell700Eff = -1;
					setupEff.wellStamp = userEfficiencyObsolete[index].wellStamp;
					setupEff.betaStamp = userEfficiencyObsolete[index].betaStamp;
					setupEff.probe700Stamp = (time_t) 0;
					setupEff.well700Stamp = (time_t) 0;
					//DB_CreateUserEff(index, &setupEff);
					d_prepare_v2("INSERT INTO UserEff(UserEffIndexOffset, NuclideIndex, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEff, UserBetaEff, UserProbe700Eff, UserWell700Eff, WellMeasuredOn, BetaMeasuredOn, Probe700MeasuredOn, Well700MeasuredOn, Inactive) VALUES(:UserEffIndexOffset, :NuclideIndex, :UserEnergy1, :UserEnergy2, :UserEnergy3, :UserWellEff, :UserBetaEff, :UserProbe700Eff, :UserWell700Eff, julianday(:WellMeasuredOn), julianday(:BetaMeasuredOn), julianday(:Probe700MeasuredOn), julianday(:Well700MeasuredOn), :Inactive);");
					d_bind_int(":UserEffIndexOffset", index);
					d_bind_int(":NuclideIndex", setupEff.nuclideID);
					d_bind_double(":UserEnergy1", setupEff.userEnergy1);
					d_bind_double(":UserEnergy2", setupEff.userEnergy2);
					d_bind_double(":UserEnergy3", setupEff.userEnergy3);
					d_bind_double(":UserWellEff", setupEff.userWellEff);
					d_bind_double(":UserBetaEff", setupEff.userBetaEff);
					d_bind_double(":UserProbe700Eff", setupEff.userProbe700Eff);
					d_bind_double(":UserWell700Eff", setupEff.userWell700Eff);
					stringout(datestring, &(setupEff.wellStamp));
					d_bind_text(":WellMeasuredOn", datestring);
					stringout(datestring, &(setupEff.betaStamp));
					d_bind_text(":BetaMeasuredOn", datestring);
					stringout(datestring, &(setupEff.probe700Stamp));
					d_bind_text(":Probe700MeasuredOn", datestring);
					stringout(datestring, &(setupEff.well700Stamp));
					d_bind_text(":Well700MeasuredOn", datestring);
					d_bind_int(":Inactive", 0);
					d_step();
					d_finalize();
				}
				service_watchdog();
			}

			if(lFileSize < USERNUC){
				for(index=lFileSize; index<USERNUC; index++){
					if(recreate){
						sprintf(strng, "User Eff %d", index + 1);
						SetAmuletString(102, strng);
						SetAmuletByte(100, 0xFF);
					}
					//DB_CreateUserEff(index, &emptySetupEff);
					d_prepare_v2("INSERT INTO UserEff(UserEffIndexOffset, NuclideIndex, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEff, UserBetaEff, UserProbe700Eff, UserWell700Eff, WellMeasuredOn, BetaMeasuredOn, Probe700MeasuredOn, Well700MeasuredOn, Inactive) VALUES(:UserEffIndexOffset, :NuclideIndex, :UserEnergy1, :UserEnergy2, :UserEnergy3, :UserWellEff, :UserBetaEff, :UserProbe700Eff, :UserWell700Eff, julianday(:WellMeasuredOn), julianday(:BetaMeasuredOn), julianday(:Probe700MeasuredOn), julianday(:Well700MeasuredOn), :Inactive);");
					d_bind_int(":UserEffIndexOffset", index);
					d_bind_int(":NuclideIndex", emptySetupEff.nuclideID);
					d_bind_double(":UserEnergy1", emptySetupEff.userEnergy1);
					d_bind_double(":UserEnergy2", emptySetupEff.userEnergy2);
					d_bind_double(":UserEnergy3", emptySetupEff.userEnergy3);
					d_bind_double(":UserWellEff", emptySetupEff.userWellEff);
					d_bind_double(":UserBetaEff", emptySetupEff.userBetaEff);
					d_bind_double(":UserProbe700Eff", emptySetupEff.userProbe700Eff);
					d_bind_double(":UserWell700Eff", emptySetupEff.userWell700Eff);
					stringout(datestring, &(emptySetupEff.wellStamp));
					d_bind_text(":WellMeasuredOn", datestring);
					stringout(datestring, &(emptySetupEff.betaStamp));
					d_bind_text(":BetaMeasuredOn", datestring);
					stringout(datestring, &(emptySetupEff.probe700Stamp));
					d_bind_text(":Probe700MeasuredOn", datestring);
					stringout(datestring, &(emptySetupEff.well700Stamp));
					d_bind_text(":Well700MeasuredOn", datestring);
					d_bind_int(":Inactive", 0);
					d_step();
					d_finalize();
					service_watchdog();
				}
			}

			if(*capStatus == SQLITE_OK) d_command("COMMIT;");
			else{
				*capStatus = SQLITE_OK;
				d_command("ROLLBACK;");
			}
		}else{
			*capStatus = SQLITE_OK;
			d_command("BEGIN;");

			d_prepare_v2("UPDATE UserEff SET Inactive = 1;");
			d_step();
			d_finalize();

			for(index=0; index<USERNUC; index++){
				if(recreate){
					sprintf(strng, "User Eff %d", index + 1);
					SetAmuletString(102, strng);
					SetAmuletByte(100, 0xFF);
				}
				//DB_CreateUserEff(index, &emptySetupEff);
				d_prepare_v2("INSERT INTO UserEff(UserEffIndexOffset, NuclideIndex, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEff, UserBetaEff, UserProbe700Eff, UserWell700Eff, WellMeasuredOn, BetaMeasuredOn, Probe700MeasuredOn, Well700MeasuredOn, Inactive) VALUES(:UserEffIndexOffset, :NuclideIndex, :UserEnergy1, :UserEnergy2, :UserEnergy3, :UserWellEff, :UserBetaEff, :UserProbe700Eff, :UserWell700Eff, julianday(:WellMeasuredOn), julianday(:BetaMeasuredOn), julianday(:Probe700MeasuredOn), julianday(:Well700MeasuredOn), :Inactive);");
				d_bind_int(":UserEffIndexOffset", index);
				d_bind_int(":NuclideIndex", emptySetupEff.nuclideID);
				d_bind_double(":UserEnergy1", emptySetupEff.userEnergy1);
				d_bind_double(":UserEnergy2", emptySetupEff.userEnergy2);
				d_bind_double(":UserEnergy3", emptySetupEff.userEnergy3);
				d_bind_double(":UserWellEff", emptySetupEff.userWellEff);
				d_bind_double(":UserBetaEff", emptySetupEff.userBetaEff);
				d_bind_double(":UserProbe700Eff", emptySetupEff.userProbe700Eff);
				d_bind_double(":UserWell700Eff", emptySetupEff.userWell700Eff);
				stringout(datestring, &(emptySetupEff.wellStamp));
				d_bind_text(":WellMeasuredOn", datestring);
				stringout(datestring, &(emptySetupEff.betaStamp));
				d_bind_text(":BetaMeasuredOn", datestring);
				stringout(datestring, &(emptySetupEff.probe700Stamp));
				d_bind_text(":Probe700MeasuredOn", datestring);
				stringout(datestring, &(emptySetupEff.well700Stamp));
				d_bind_text(":Well700MeasuredOn", datestring);
				d_bind_int(":Inactive", 0);
				d_step();
				d_finalize();
				service_watchdog();
			}

			if(*capStatus == SQLITE_OK) d_command("COMMIT;");
			else{
				*capStatus = SQLITE_OK;
				d_command("ROLLBACK;");
			}
		}
		// erase usereff.bin
		f_unlink("usereff.bin");
	}else{
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");

		d_prepare_v2("UPDATE UserEff SET Inactive = 1;");
		d_step();
		d_finalize();

		for(index=0; index<USERNUC; index++){
			if(recreate){
				sprintf(strng, "User Eff %d", index + 1);
				SetAmuletString(102, strng);
				SetAmuletByte(100, 0xFF);
			}
			//DB_CreateUserEff(index, &emptySetupEff);
			d_prepare_v2("INSERT INTO UserEff(UserEffIndexOffset, NuclideIndex, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEff, UserBetaEff, UserProbe700Eff, UserWell700Eff, WellMeasuredOn, BetaMeasuredOn, Probe700MeasuredOn, Well700MeasuredOn, Inactive) VALUES(:UserEffIndexOffset, :NuclideIndex, :UserEnergy1, :UserEnergy2, :UserEnergy3, :UserWellEff, :UserBetaEff, :UserProbe700Eff, :UserWell700Eff, julianday(:WellMeasuredOn), julianday(:BetaMeasuredOn), julianday(:Probe700MeasuredOn), julianday(:Well700MeasuredOn), :Inactive);");
			d_bind_int(":UserEffIndexOffset", index);
			d_bind_int(":NuclideIndex", emptySetupEff.nuclideID);
			d_bind_double(":UserEnergy1", emptySetupEff.userEnergy1);
			d_bind_double(":UserEnergy2", emptySetupEff.userEnergy2);
			d_bind_double(":UserEnergy3", emptySetupEff.userEnergy3);
			d_bind_double(":UserWellEff", emptySetupEff.userWellEff);
			d_bind_double(":UserBetaEff", emptySetupEff.userBetaEff);
			d_bind_double(":UserProbe700Eff", emptySetupEff.userProbe700Eff);
			d_bind_double(":UserWell700Eff", emptySetupEff.userWell700Eff);
			stringout(datestring, &(emptySetupEff.wellStamp));
			d_bind_text(":WellMeasuredOn", datestring);
			stringout(datestring, &(emptySetupEff.betaStamp));
			d_bind_text(":BetaMeasuredOn", datestring);
			stringout(datestring, &(emptySetupEff.probe700Stamp));
			d_bind_text(":Probe700MeasuredOn", datestring);
			stringout(datestring, &(emptySetupEff.well700Stamp));
			d_bind_text(":Well700MeasuredOn", datestring);
			d_bind_int(":Inactive", 0);
			d_step();
			d_finalize();
			service_watchdog();
		}
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_ReadAllUserEff(SETUP_EFFICIENCY *setupEfficiency){
	int userEffIndexOffset;
	char MeasuredOn[40];

	NuclideData_clearEfficiencyMirror();

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT UserEffIndexOffset, NuclideIndex, UserEnergy1, UserEnergy2, UserEnergy3, UserWellEff, UserBetaEff, UserProbe700Eff, UserWell700Eff, datetime(WellMeasuredOn), datetime(BetaMeasuredOn), datetime(Probe700MeasuredOn), datetime(Well700MeasuredOn) FROM UserEff WHERE Inactive = 0 AND NuclideIndex != -1;");
	d_step();
	if(*capStatus == SQLITE_ROW){
		while(*capStatus == SQLITE_ROW){
			userEffIndexOffset = d_column_int(0); //UserEffIndexOffset
			if(userEffIndexOffset < USERNUC){
				setupEfficiency[userEffIndexOffset].nuclideID = d_column_int(1); //NuclideIndex
				if(current.default_keV == 0){
					setupEfficiency[userEffIndexOffset].userEnergy1 = d_column_double(2); //UserEnergy1
					setupEfficiency[userEffIndexOffset].userEnergy2 = d_column_double(3); //UserEnergy2
					setupEfficiency[userEffIndexOffset].userEnergy3 = d_column_double(4); //UserEnergy3
				}else{
					if(setupEfficiency[userEffIndexOffset].nuclideID >= 0){
						if(NuclideData_getPrimary(setupEfficiency[userEffIndexOffset].nuclideID) == 0.0){
							setupEfficiency[userEffIndexOffset].userEnergy1 = d_column_double(2); //UserEnergy1
							setupEfficiency[userEffIndexOffset].userEnergy2 = d_column_double(3); //UserEnergy2
							setupEfficiency[userEffIndexOffset].userEnergy3 = d_column_double(4); //UserEnergy3
						}
					}
				}
				setupEfficiency[userEffIndexOffset].userWellEff = d_column_double(5); //UserWellEff
				setupEfficiency[userEffIndexOffset].userBetaEff = d_column_double(6); //UserBetaEff
				setupEfficiency[userEffIndexOffset].userProbe700Eff = d_column_double(7); //UserProbe700Eff
				setupEfficiency[userEffIndexOffset].userWell700Eff = d_column_double(8); //UserWell700Eff
				strcpy(MeasuredOn, (const char *) d_column_text(9)); //WellMeasuredOn
				setupEfficiency[userEffIndexOffset].wellStamp = strtotime_t(MeasuredOn);
				strcpy(MeasuredOn, (const char *) d_column_text(10)); //BetaMeasuredOn
				setupEfficiency[userEffIndexOffset].betaStamp = strtotime_t(MeasuredOn);
				strcpy(MeasuredOn, (const char *) d_column_text(11)); //Probe700MeasuredOn
				setupEfficiency[userEffIndexOffset].probe700Stamp = strtotime_t(MeasuredOn);
				strcpy(MeasuredOn, (const char *) d_column_text(12)); //Well700MeasuredOn
				setupEfficiency[userEffIndexOffset].well700Stamp = strtotime_t(MeasuredOn);
			}
			d_step();
		}
	}
	d_finalize();
}

void DB_SetDefaultEnergy(void){
	char query[1024], buff[30];
	int index;

	strcpy(query, "UPDATE UserEff SET UserEnergy1=-1,UserEnergy2=-1,UserEnergy3=-1 WHERE Inactive=0 AND NuclideIndex!=-1");
	for(index=0; index <= USERNUC - 5; index++){
		if(NuclideData_getPrimary(index) == 0.0){
			sprintf(buff, " AND NuclideIndex!=%d", index);
			strcat(query, buff);
		}
	}
	for(index=USERNUC+20; index < ALLNUC; index++){
		if(NuclideData_getPrimary(index) == 0.0){
			sprintf(buff, " AND NuclideIndex!=%d", index);
			strcat(query, buff);
		}
	}
	strcat(query, ";");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_step();
	d_finalize();
}

void DB_moveGencfFromEEPROM(void){
	float gencf;

	EE_READ(obsolete10, (uchar *) &gencf);
	d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FullSpectrumEffCapracT',:FullSpectrumEffCapracT);");
	d_bind_double(":FullSpectrumEffCapracT", gencf);
	d_step();
	d_finalize();
	service_watchdog();
	gencf = 37.04;
	EE_WRITE(obsolete10, (uchar *) &gencf);
	d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FullSpectrumEffCaptusT',:FullSpectrumEffCaptusT);");
	d_bind_double(":FullSpectrumEffCaptusT", 65.0);
	d_step();
	d_finalize();
	service_watchdog();
}

float DB_ReadFullSpecEff(short detector){
	float returnvalue;

	*capStatus = SQLITE_OK;
	if(detector == DET_WELL){
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'FullSpectrumEffCapracT';")){
			if(*capStatus == SQLITE_ROW){
				returnvalue = d_column_double(0);
			}else{
				returnvalue = 37.04;
			}
			d_finalize();
		}else{
			d_finalize();
			returnvalue = 37.04;
			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FullSpectrumEffCapracT',:FullSpectrumEffCapracT);");
			d_bind_double(":FullSpectrumEffCapracT", returnvalue);
			d_step();
			d_finalize();
		}
	}else{
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'FullSpectrumEffCaptusT';")){
			if(*capStatus == SQLITE_ROW){
				returnvalue = d_column_double(0);
			}else{
				returnvalue = 65.0;
			}
			d_finalize();
		}else{
			d_finalize();
			returnvalue = 65.0;
			d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('FullSpectrumEffCaptusT',:FullSpectrumEffCaptusT);");
			d_bind_double(":FullSpectrumEffCaptusT", 65.0);
			d_step();
			d_finalize();
		}
	}

	return returnvalue;
}

void DB_WriteFullSpecEff(short detector, float fullSpecEff){
	char query[100];

	if(detector == DET_WELL){
		strcpy(query, "UPDATE Config SET SettingValue = :fullSpecEff WHERE SettingName = 'FullSpectrumEffCapracT'");
	}else{
		strcpy(query, "UPDATE Config SET SettingValue = :fullSpecEff WHERE SettingName = 'FullSpectrumEffCaptusT'");
	}

	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_double(":fullSpecEff", fullSpecEff);
	d_step();
	d_finalize();
}

int DB_ReadSecurityMode(void){
	int returnvalue;

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'SecurityMode';")){
			if(*capStatus == SQLITE_ROW){
				returnvalue = d_column_int(0);
			}else{
				returnvalue = 0;
			}
			d_finalize();
		}else{
			d_finalize();
			returnvalue = 0;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('SecurityMode',0);");
		}
	}

	return returnvalue;
}

void DB_WriteSecurityMode(int SecurityMode){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE Config SET SettingValue = :SecurityMode WHERE SettingName = 'SecurityMode';");
	d_bind_int(":SecurityMode", SecurityMode);
	d_step();
	d_finalize();
}

void DB_GetUser(char *username, USER *user){
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT UserID, UserName, Password, FirstName, LastName, Role, Inactive FROM User WHERE UserName = :UserName;");
	d_bind_text(":UserName", username);
	d_step();
	if(*capStatus == SQLITE_ROW){
		user->UserID = d_column_int(0); //UserID,
		strcpy(user->UserName,  (const char *) d_column_text(1)); //UserName,
		strcpy(user->Password, (const char *) d_column_text(2)); //Password,
		strcpy(user->FirstName, (const char *) d_column_text(3)); //FirstName,
		strcpy(user->LastName, (const char *) d_column_text(4)); //LastName,
		user->Role = d_column_int(5); //Role,
		if(d_column_int(6) == 0) user->Inactive = FALSE; //Inactive
		else user->Inactive = TRUE;
	}else{
		user->UserID = -1;
		strcpy(user->UserName, username);
		user->Password[0] = 0;
		user->FirstName[0] = 0;
		user->LastName[0] = 0;
		user->Role = -1;
		user->Inactive = 1;
	}
	d_finalize();
}

void DB_GetUserFromID(int userID, USER *user){
	if(userID > 0){
		*capStatus = SQLITE_OK;
		d_prepare_v2("SELECT UserID, UserName, Password, FirstName, LastName, Role, Inactive FROM User WHERE UserID = :UserID;");
		d_bind_int(":UserID", userID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			user->UserID = d_column_int(0); //UserID,
			strcpy(user->UserName,  (const char *) d_column_text(1)); //UserName,
			strcpy(user->Password, (const char *) d_column_text(2)); //Password,
			strcpy(user->FirstName, (const char *) d_column_text(3)); //FirstName,
			strcpy(user->LastName, (const char *) d_column_text(4)); //LastName,
			user->Role = d_column_int(5); //Role,
			if(d_column_int(6) == 0) user->Inactive = FALSE; //Inactive
			else user->Inactive = TRUE;
		}else{
			user->UserID = -1;
			user->UserName[0] = 0;
			user->Password[0] = 0;
			user->FirstName[0] = 0;
			user->LastName[0] = 0;
			user->Role = -1;
			user->Inactive = 1;
		}
		d_finalize();
	}else{
		user->UserID = -1;
		user->UserName[0] = 0;
		user->Password[0] = 0;
		user->FirstName[0] = 0;
		user->LastName[0] = 0;
		user->Role = -1;
		user->Inactive = 1;
	}
}

int DB_GetUserCount(USER *user){
	int recordcount;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT COUNT(*) FROM User WHERE Role > 0 AND UserID != :userID AND Role < :role;");
	d_bind_int(":userID", user->UserID);
	d_bind_int(":role", user->Role);
	d_step();
	if(*capStatus == SQLITE_ROW){
		recordcount = d_column_int(0);
	}else{
		recordcount = 0;
	}
	d_finalize();
	recordcount++;
	return recordcount;
}

int DB_GetUserList(USER *user, USER **userList, bool includeUser, bool includeInactive){
	USER *pointer;
	int returnvalue;

	returnvalue = 0;
	pointer = *userList;
	if(includeUser){
		memcpy(pointer, user, sizeof(USER));
		pointer++;
		returnvalue++;
	}

	*capStatus = SQLITE_OK;
	if(includeInactive) d_prepare_v2("SELECT UserID, UserName, Password, FirstName, LastName, Role, Inactive FROM User WHERE Role > 0 AND UserID != :userID AND Role < :role ORDER BY UserName;");
	else d_prepare_v2("SELECT UserID, UserName, Password, FirstName, LastName, Role, Inactive FROM User WHERE Inactive = 0 AND Role > 0 AND UserID != :userID AND Role < :role ORDER BY UserName;");
	d_bind_int(":userID", user->UserID);
	d_bind_int(":role", user->Role);
	d_step();
	while(*capStatus == SQLITE_ROW){
		pointer->UserID = d_column_int(0);
		strcpy(pointer->UserName, (const char *) d_column_text(1));
		strcpy(pointer->Password, (const char *) d_column_text(2));
		strcpy(pointer->FirstName, (const char *) d_column_text(3));
		strcpy(pointer->LastName, (const char *) d_column_text(4));
		pointer->Role = d_column_int(5);
		if(d_column_int(6) == 0) pointer->Inactive = FALSE;
		else pointer->Inactive = TRUE;
		pointer++;
		returnvalue++;
		d_step();
	}
	d_finalize();

	return returnvalue;
}

bool DB_TestUserName(char *userName){
	bool returnvalue;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT UserID FROM User WHERE UserName LIKE :UserName;");
	d_bind_text(":UserName", userName);
	d_step();
	if(*capStatus == SQLITE_ROW) returnvalue = TRUE;
	else returnvalue = FALSE;
	d_finalize();

	return returnvalue;
}

void DB_WriteUser(USER *user){
	*capStatus = SQLITE_OK;
	d_command("BEGIN;");
	if(user->UserID < 0){
		// Add New User
		d_prepare_v2("INSERT INTO User(UserName, Password, FirstName, LastName, Role, Inactive) VALUES(:UserName, :Password, :FirstName, :LastName, :Role, :Inactive);");
		d_bind_text(":UserName", user->UserName);
		d_bind_text(":Password", user->Password);
		d_bind_text(":FirstName", user->FirstName);
		d_bind_text(":LastName", user->LastName);
		d_bind_int(":Role", user->Role);
		if(user->Inactive) d_bind_int(":Inactive", 1);
		else d_bind_int(":Inactive", 0);
		d_step();
		d_finalize();
	}else{
		// Update User
		d_prepare_v2("UPDATE User SET UserName = :UserName WHERE UserID = :UserID;");
		d_bind_text(":UserName", user->UserName);
		d_bind_int(":UserID", user->UserID);
		d_step();
		d_finalize();

		d_prepare_v2("UPDATE User SET Password = :Password WHERE UserID = :UserID;");
		d_bind_text(":Password", user->Password);
		d_bind_int(":UserID", user->UserID);
		d_step();
		d_finalize();

		d_prepare_v2("UPDATE User Set FirstName = :FirstName WHERE UserID = :UserID;");
		d_bind_text(":FirstName", user->FirstName);
		d_bind_int(":UserID", user->UserID);
		d_step();
		d_finalize();

		d_prepare_v2("UPDATE User Set LastName = :LastName WHERE UserID = :UserID;");
		d_bind_text(":LastName", user->LastName);
		d_bind_int(":UserID", user->UserID);
		d_step();
		d_finalize();

		d_prepare_v2("UPDATE User Set Role = :Role WHERE UserID = :UserID;");
		d_bind_int(":Role", user->Role);
		d_bind_int(":UserID", user->UserID);
		d_step();
		d_finalize();

		if(user->Inactive){
			d_prepare_v2("UPDATE User Set Inactive = 1 WHERE UserID = :UserID;");
			d_bind_int(":UserID", user->UserID);
			d_step();
			d_finalize();
		}else{
			d_prepare_v2("UPDATE User Set Inactive = 0 WHERE UserID = :UserID;");
			d_bind_int(":UserID", user->UserID);
			d_step();
			d_finalize();
		}

		d_prepare_v2("UPDATE User Set LastUpdated = julianday(CURRENT_TIMESTAMP) WHERE UserID = :UserID;");
		d_bind_int(":UserID", user->UserID);
		d_step();
		d_finalize();
	}
	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}

void DB_ReadCurrentProbeBioAssayEfficiency(PROBEBIOASSAYEFFICIENCY *bioassayeff){
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ProbeBioAssayEfficiencyID,I131_Efficiency,I125_Efficiency,I123_Efficiency,I131_I125_Contamination,I123_I125_Contamination,I131Measured,I131_Activity,I131_Date,datetime(I131_Date),I131_PrimaryEnergy,I131_startEV,I131_endEV,I131_I125_startEV,I131_I125_endEV,I131BackgroundID,I131_SpectraID,I131_ROICounts,I131_ROICPM,I131_BackgroundROICounts,I131_BackgroundROICPM,I131_NetROICPM,I131_I125_ROICounts,I131_I125_ROICPM,I131_I125_BackgroundROICounts,I131_I125_BackgroundROICPM,I131_I125_NetROICPM,I125Measured,I125_Activity,I125_Date,datetime(I125_Date),I125_PrimaryEnergy,I125_startEV,I125_endEV,I125BackgroundID,I125_SpectraID,I125_ROICounts,I125_ROICPM,I125_BackgroundROICounts,I125_BackgroundROICPM,I125_NetROICPM,I123Measured,I123_Activity,I123_Date,datetime(I123_Date),I123_PrimaryEnergy,I123_startEV,I123_endEV,I123_I125_startEV,I123_I125_endEV,I123BackgroundID,I123_SpectraID,I123_ROICounts,I123_ROICPM,I123_BackgroundROICounts,I123_BackgroundROICPM,I123_NetROICPM,I123_I125_ROICounts,I123_I125_ROICPM,I123_I125_BackgroundROICounts,I123_I125_BackgroundROICPM,I123_I125_NetROICPM,datetime(CreatedOn),Inactive FROM ProbeBioAssayEfficiency WHERE Inactive = 0;");
	d_step();
	if(*capStatus == SQLITE_ROW){
		bioassayeff->ProbeBioAssayEfficiencyID = d_column_int64(0); //ProbeBioAssayEfficiencyID
		bioassayeff->I131Efficiency = d_column_double(1); //I131_Efficiency
		bioassayeff->I125Efficiency = d_column_double(2); //I125_Efficiency
		bioassayeff->I123Efficiency = d_column_double(3); //I123_Efficiency
		bioassayeff->I131I125Contamination = d_column_double(4); //I131_I125_Contamination
		bioassayeff->I123I125Contamination = d_column_double(5); //I123_I125_Contamination
		if(d_column_int(6) == 0) bioassayeff->I131Measured = FALSE; //I131Measured
		else bioassayeff->I131Measured = TRUE; //I131Measured
		bioassayeff->I131Activity = d_column_double(7); //I131_Activity
		if(d_column_double(8) == 0) bioassayeff->I131Date = (time_t) 0; //I131_Date
		else bioassayeff->I131Date = strtotime_t((const char *) d_column_text(9)); //datetime(I131_Date)
		bioassayeff->I131PrimaryEnergy = d_column_double(10); //I131_PrimaryEnergy
		bioassayeff->I131StartEV = d_column_double(11); //I131_startEV,
		bioassayeff->I131EndEV = d_column_double(12); //I131_endEV
		bioassayeff->I131I125StartEV = d_column_double(13); //I131_I125_startEV
		bioassayeff->I131I125EndEV = d_column_double(14); //I131_I125_endEV
		bioassayeff->I131BackgroundID = d_column_int64(15); //I131BackgroundID
		bioassayeff->I131SpectraID = d_column_int64(16); //I131_SpectraID
		bioassayeff->I131ROICounts = d_column_int(17); //I131_ROICounts
		bioassayeff->I131ROICPM = d_column_double(18); //I131_ROICPM
		bioassayeff->I131BackgroundROICounts = d_column_int(19); //I131_BackgroundROICounts
		bioassayeff->I131BackgroundROICPM = d_column_double(20); //I131_BackgroundROICPM
		bioassayeff->I131NetROICPM = d_column_double(21); //I131_NetROICPM
		bioassayeff->I131I125ROICounts = d_column_int(22); //I131_I125_ROICounts
		bioassayeff->I131I125ROICPM = d_column_double(23); //I131_I125_ROICPM
		bioassayeff->I131I125BackgroundROICounts = d_column_int(24); //I131_I125_BackgroundROICounts
		bioassayeff->I131I125BackgroundROICPM = d_column_double(25); //I131_I125_BackgroundROICPM
		bioassayeff->I131I125NetROICPM = d_column_double(26); //I131_I125_NetROICPM
		if(d_column_int(27) == 0) bioassayeff->I125Measured = FALSE; //I125Measured
		else bioassayeff->I125Measured = TRUE; //I125Measured
		bioassayeff->I125Activity = d_column_double(28); //I125_Activity
		if(d_column_double(29) == 0) bioassayeff->I125Date = (time_t) 0; //I125_Date
		else bioassayeff->I125Date = strtotime_t((const char *) d_column_text(30)); //datetime(I125_Date)
		bioassayeff->I125PrimaryEnergy = d_column_double(31); //I125_PrimaryEnergy
		bioassayeff->I125StartEV = d_column_double(32); //I125_startEV
		bioassayeff->I125EndEV = d_column_double(33); //I125_endEV
		bioassayeff->I125BackgroundID = d_column_int64(34); //I125BackgroundID
		bioassayeff->I125SpectraID = d_column_int64(35); //I125_SpectraID
		bioassayeff->I125ROICounts = d_column_int(36); //I125_ROICounts
		bioassayeff->I125ROICPM = d_column_double(37); //I125_ROICPM
		bioassayeff->I125BackgroundROICounts = d_column_int(38); //I125_BackgroundROICounts
		bioassayeff->I125BackgroundROICPM = d_column_double(39); //I125_BackgroundROICPM
		bioassayeff->I125NetROICPM = d_column_double(40); //I125_NetROICPM
		if(d_column_int(41) == 0) bioassayeff->I123Measured = FALSE; //I123Measured
		else bioassayeff->I123Measured = TRUE; //I123Measured
		bioassayeff->I123Activity = d_column_double(42); //I123_Activity
		if(d_column_double(43) == 0 ) bioassayeff->I123Date = (time_t) 0; //I123_Date
		else bioassayeff->I123Date = strtotime_t((const char *) d_column_text(44)); //datetime(I123_Date)
		bioassayeff->I123PrimaryEnergy = d_column_double(45); //I123_PrimaryEnergy
		bioassayeff->I123StartEV = d_column_double(46); //I123_startEV
		bioassayeff->I123EndEV = d_column_double(47); //I123_endEV
		bioassayeff->I123I125StartEV = d_column_double(48); //I123_I125_startEV
		bioassayeff->I123I125EndEV = d_column_double(49); //I123_I125_endEV
		bioassayeff->I123BackgroundID = d_column_int64(50); //I123BackgroundID
		bioassayeff->I123SpectraID = d_column_int64(51); //I123_SpectraID
		bioassayeff->I123ROICounts = d_column_int(52);//I123_ROICounts
		bioassayeff->I123ROICPM = d_column_double(53); //I123_ROICPM
		bioassayeff->I123BackgroundROICounts = d_column_int(54); //I123_BackgroundROICounts
		bioassayeff->I123BackgroundROICPM = d_column_double(55); //I123_BackgroundROICPM
		bioassayeff->I123NetROICPM = d_column_double(56); //I123_NetROICPM
		bioassayeff->I123I125ROICounts = d_column_int(57); //I123_I125_ROICounts
		bioassayeff->I123I125ROICPM = d_column_double(58); //I123_I125_ROICPM
		bioassayeff->I123I125BackgroundROICounts = d_column_int(59); //I123_I125_BackgroundROICounts
		bioassayeff->I123I125BackgroundROICPM = d_column_double(60); //I123_I125_BackgroundROICPM
		bioassayeff->I123I125NetROICPM = d_column_double(61); //I123_I125_NetROICPM
		bioassayeff->CreatedOn = strtotime_t((const char *) d_column_text(62));//datetime(CreatedOn),
		if(d_column_int(63) == 0) bioassayeff->Inactive = FALSE; //Inactive
		else bioassayeff->Inactive = TRUE;
	}else{
		bioassayeff->ProbeBioAssayEfficiencyID = 0;
		bioassayeff->I131Efficiency = -1;
		bioassayeff->I125Efficiency = -1;
		bioassayeff->I123Efficiency = -1;
		bioassayeff->I131I125Contamination = -1;
		bioassayeff->I123I125Contamination = -1;
		bioassayeff->I131Measured = FALSE;
		bioassayeff->I131Activity = 0;
		bioassayeff->I131Date = (time_t) 0;
		bioassayeff->I131PrimaryEnergy = 0;
		bioassayeff->I131StartEV = 0;
		bioassayeff->I131EndEV = 0;
		bioassayeff->I131I125StartEV = 0;
		bioassayeff->I131I125EndEV = 0;
		bioassayeff->I131BackgroundID = -1;
		bioassayeff->I131SpectraID = -1;
		bioassayeff->I131ROICounts = 0;
		bioassayeff->I131ROICPM = 0;
		bioassayeff->I131BackgroundROICounts = 0;
		bioassayeff->I131BackgroundROICPM = 0;
		bioassayeff->I131NetROICPM = 0;
		bioassayeff->I131I125ROICounts = 0;
		bioassayeff->I131I125ROICPM = 0;
		bioassayeff->I131I125BackgroundROICounts = 0;
		bioassayeff->I131I125BackgroundROICPM = 0;
		bioassayeff->I131I125NetROICPM = 0;
		bioassayeff->I125Measured = FALSE;
		bioassayeff->I125Activity = 0;
		bioassayeff->I125Date = (time_t) 0;
		bioassayeff->I125PrimaryEnergy = 0;
		bioassayeff->I125StartEV = 0;
		bioassayeff->I125EndEV = 0;
		bioassayeff->I125BackgroundID = -1;
		bioassayeff->I125SpectraID = -1;
		bioassayeff->I125ROICounts = 0;
		bioassayeff->I125ROICPM = 0;
		bioassayeff->I125BackgroundROICounts = 0;
		bioassayeff->I125BackgroundROICPM = 0;
		bioassayeff->I125NetROICPM = 0;
		bioassayeff->I123Measured = FALSE;
		bioassayeff->I123Activity = 0;
		bioassayeff->I123Date = (time_t) 0;
		bioassayeff->I123PrimaryEnergy = 0;
		bioassayeff->I123StartEV = 0;
		bioassayeff->I123EndEV = 0;
		bioassayeff->I123I125StartEV = 0;
		bioassayeff->I123I125EndEV = 0;
		bioassayeff->I123BackgroundID = -1;
		bioassayeff->I123SpectraID = -1;
		bioassayeff->I123ROICounts = 0;
		bioassayeff->I123ROICPM = 0;
		bioassayeff->I123BackgroundROICounts = 0;
		bioassayeff->I123BackgroundROICPM = 0;
		bioassayeff->I123NetROICPM = 0;
		bioassayeff->I123I125ROICounts = 0;
		bioassayeff->I123I125ROICPM = 0;
		bioassayeff->I123I125BackgroundROICounts = 0;
		bioassayeff->I123I125BackgroundROICPM = 0;
		bioassayeff->I123I125NetROICPM = 0;
		bioassayeff->CreatedOn = (time_t) 0;
		bioassayeff->Inactive = FALSE;
	}
	d_finalize();
}

void DB_ReadCurrentProbeBioAssaySetting(PROBEBIOASSAYSETTING *bioassaysetting){
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ProbeBioAssaySettingID,I131_Active,I125_Active,I123_Active,I131_Threshold,I125_Threshold,I123_Threshold,CountTime,ProbeDistance,datetime(CreatedOn),Inactive FROM ProbeBioAssaySetting WHERE Inactive = 0;");
	d_step();
	if(*capStatus == SQLITE_ROW){
		bioassaysetting->ProbeBioAssaySettingID = d_column_int64(0); //ProbeBioAssaySettingID
		if(d_column_int(1) == 0) bioassaysetting->I131Active = FALSE; //I131_Active
		else bioassaysetting->I131Active = TRUE;
		if(d_column_int(2) == 0) bioassaysetting->I125Active = FALSE; //I125_Active
		else bioassaysetting->I125Active = TRUE;
		if(d_column_int(3) == 0) bioassaysetting->I123Active = FALSE; //I123_Active
		else bioassaysetting->I123Active = TRUE;
		bioassaysetting->I131Threshold = d_column_double(4); //I131_Threshold
		bioassaysetting->I125Threshold = d_column_double(5); //I125_Threshold
		bioassaysetting->I123Threshold = d_column_double(6); //I123_Threshold
		bioassaysetting->CountTime = d_column_int(7); //CountTime
		bioassaysetting->ProbeDistance = d_column_int(8); //ProbeDistance
		bioassaysetting->CreatedOn = strtotime_t((const char *) d_column_text(9)); //datetime(CreatedOn)
		if(d_column_int(10) == 0) bioassaysetting->Inactive = FALSE; //Inactive
		else bioassaysetting->Inactive = TRUE;
	}else{
		bioassaysetting->ProbeBioAssaySettingID = 0;
		bioassaysetting->I131Active = FALSE;
		bioassaysetting->I125Active = FALSE;
		bioassaysetting->I123Active = FALSE;
		bioassaysetting->I131Threshold = -1;
		bioassaysetting->I125Threshold = -1;
		bioassaysetting->I123Threshold = -1;
		bioassaysetting->CountTime = 60;
		bioassaysetting->ProbeDistance = 25;
		bioassaysetting->CreatedOn = (time_t) 0;
		bioassaysetting->Inactive = FALSE;
	}
	d_finalize();
}

void DB_ReadProbeBioAssaySetting(long long int ProbeBioAssaySettingID, PROBEBIOASSAYSETTING *bioassaysetting){
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ProbeBioAssaySettingID,I131_Active,I125_Active,I123_Active,I131_Threshold,I125_Threshold,I123_Threshold,CountTime,ProbeDistance,datetime(CreatedOn),Inactive FROM ProbeBioAssaySetting WHERE ProbeBioAssaySettingID=:ProbeBioAssaySettingID;");
	d_bind_int64(":ProbeBioAssaySettingID", ProbeBioAssaySettingID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		bioassaysetting->ProbeBioAssaySettingID = d_column_int64(0); //ProbeBioAssaySettingID
		if(d_column_int(1) == 0) bioassaysetting->I131Active = FALSE; //I131_Active
		else bioassaysetting->I131Active = TRUE;
		if(d_column_int(2) == 0) bioassaysetting->I125Active = FALSE; //I125_Active
		else bioassaysetting->I125Active = TRUE;
		if(d_column_int(3) == 0) bioassaysetting->I123Active = FALSE; //I123_Active
		else bioassaysetting->I123Active = TRUE;
		bioassaysetting->I131Threshold = d_column_double(4); //I131_Threshold
		bioassaysetting->I125Threshold = d_column_double(5); //I125_Threshold
		bioassaysetting->I123Threshold = d_column_double(6); //I123_Threshold
		bioassaysetting->CountTime = d_column_int(7); //CountTime
		bioassaysetting->ProbeDistance = d_column_int(8); //ProbeDistance
		bioassaysetting->CreatedOn = strtotime_t((const char *) d_column_text(9)); //datetime(CreatedOn)
		if(d_column_int(10) == 0) bioassaysetting->Inactive = FALSE; //Inactive
		else bioassaysetting->Inactive = TRUE;
	}else{
		bioassaysetting->ProbeBioAssaySettingID = 0;
		bioassaysetting->I131Active = FALSE;
		bioassaysetting->I125Active = FALSE;
		bioassaysetting->I123Active = FALSE;
		bioassaysetting->I131Threshold = -1;
		bioassaysetting->I125Threshold = -1;
		bioassaysetting->I123Threshold = -1;
		bioassaysetting->CountTime = 60;
		bioassaysetting->ProbeDistance = 25;
		bioassaysetting->CreatedOn = (time_t) 0;
		bioassaysetting->Inactive = FALSE;
	}
	d_finalize();
}

void DB_WriteProbeBioAssaySetting(PROBEBIOASSAYSETTING *setting, bool bookEnd){
	sqlite3_int64 rowid;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("UPDATE ProbeBioAssaySetting SET Inactive = 1;");
	d_step();
	d_finalize();

	d_prepare_v2("INSERT INTO ProbeBioAssaySetting(I131_Active,I125_Active,I123_Active,I131_Threshold,I125_Threshold,I123_Threshold,CountTime,ProbeDistance,Inactive) VALUES(:I131_Active,:I125_Active,:I123_Active,:I131_Threshold,:I125_Threshold,:I123_Threshold,:CountTime,:ProbeDistance,:Inactive);");

	if(setting->I131Active) d_bind_int(":I131_Active",1);
	else d_bind_int(":I131_Active",0);

	if(setting->I125Active) d_bind_int(":I125_Active",1);
	else d_bind_int(":I125_Active",0);

	if(setting->I123Active)	d_bind_int(":I123_Active",1);
	else	d_bind_int(":I123_Active",0);

	d_bind_double(":I131_Threshold",setting->I131Threshold);
	d_bind_double(":I125_Threshold",setting->I125Threshold);
	d_bind_double(":I123_Threshold",setting->I123Threshold);
	d_bind_int(":CountTime",setting->CountTime);
	d_bind_int(":ProbeDistance",setting->ProbeDistance);
	d_bind_int(":Inactive",0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
	else rowid = -1;
	d_finalize();

	setting->ProbeBioAssaySettingID = rowid;

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_WriteProbeBioAssayEfficiency(PROBEBIOASSAYEFFICIENCY *eff, bool bookEnd){
	sqlite3_int64 rowid;
	char datetimestr[30];

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("UPDATE ProbeBioAssayEfficiency SET Inactive = 1;");
	d_step();
	d_finalize();

	d_prepare_v2("INSERT INTO ProbeBioAssayEfficiency(I131_Efficiency,I125_Efficiency,I123_Efficiency,I131_I125_Contamination,I123_I125_Contamination,I131Measured,I131_Activity,I131_Date,I131_PrimaryEnergy,I131_startEV,I131_endEV,I131_I125_startEV,I131_I125_endEV,I131BackgroundID,I131_SpectraID,I131_ROICounts,I131_ROICPM,I131_BackgroundROICounts,I131_BackgroundROICPM,I131_NetROICPM,I131_I125_ROICounts,I131_I125_ROICPM,I131_I125_BackgroundROICounts,I131_I125_BackgroundROICPM,I131_I125_NetROICPM,I125Measured,I125_Activity,I125_Date,I125_PrimaryEnergy,I125_startEV,I125_endEV,I125BackgroundID,I125_SpectraID,I125_ROICounts,I125_ROICPM,I125_BackgroundROICounts,I125_BackgroundROICPM,I125_NetROICPM,I123Measured,I123_Activity,I123_Date,I123_PrimaryEnergy,I123_startEV,I123_endEV,I123_I125_startEV,I123_I125_endEV,I123BackgroundID,I123_SpectraID,I123_ROICounts,I123_ROICPM,I123_BackgroundROICounts,I123_BackgroundROICPM,I123_NetROICPM,I123_I125_ROICounts,I123_I125_ROICPM,I123_I125_BackgroundROICounts,I123_I125_BackgroundROICPM,I123_I125_NetROICPM,Inactive) VALUES(:I131_Efficiency,:I125_Efficiency,:I123_Efficiency,:I131_I125_Contamination,:I123_I125_Contamination,:I131Measured,:I131_Activity,julianday(:I131_Date),:I131_PrimaryEnergy,:I131_startEV,:I131_endEV,:I131_I125_startEV,:I131_I125_endEV,:I131BackgroundID,:I131_SpectraID,:I131_ROICounts,:I131_ROICPM,:I131_BackgroundROICounts,:I131_BackgroundROICPM,:I131_NetROICPM,:I131_I125_ROICounts,:I131_I125_ROICPM,:I131_I125_BackgroundROICounts,:I131_I125_BackgroundROICPM,:I131_I125_NetROICPM,:I125Measured,:I125_Activity,julianday(:I125_Date),:I125_PrimaryEnergy,:I125_startEV,:I125_endEV,:I125BackgroundID,:I125_SpectraID,:I125_ROICounts,:I125_ROICPM,:I125_BackgroundROICounts,:I125_BackgroundROICPM,:I125_NetROICPM,:I123Measured,:I123_Activity,julianday(:I123_Date),:I123_PrimaryEnergy,:I123_startEV,:I123_endEV,:I123_I125_startEV,:I123_I125_endEV,:I123BackgroundID,:I123_SpectraID,:I123_ROICounts,:I123_ROICPM,:I123_BackgroundROICounts,:I123_BackgroundROICPM,:I123_NetROICPM,:I123_I125_ROICounts,:I123_I125_ROICPM,:I123_I125_BackgroundROICounts,:I123_I125_BackgroundROICPM,:I123_I125_NetROICPM,:Inactive);");
	d_bind_double(":I131_Efficiency",eff->I131Efficiency);
	d_bind_double(":I125_Efficiency",eff->I125Efficiency);
	d_bind_double(":I123_Efficiency",eff->I123Efficiency);
	d_bind_double(":I131_I125_Contamination",eff->I131I125Contamination);
	d_bind_double(":I123_I125_Contamination",eff->I123I125Contamination);
	if(eff->I131Measured) d_bind_int(":I131Measured",1);
	else d_bind_int(":I131Measured",0);
	d_bind_double(":I131_Activity",eff->I131Activity);
	stringout(datetimestr, &(eff->I131Date));
	d_bind_text(":I131_Date",datetimestr);
	d_bind_double(":I131_PrimaryEnergy",eff->I131PrimaryEnergy);
	d_bind_double(":I131_startEV", eff->I131StartEV);
	d_bind_double(":I131_endEV", eff->I131EndEV);
	d_bind_double(":I131_I125_startEV", eff->I131I125StartEV);
	d_bind_double(":I131_I125_endEV", eff->I131I125EndEV);
	d_bind_int64(":I131BackgroundID",eff->I131BackgroundID);
	d_bind_int64(":I131_SpectraID",eff->I131SpectraID);
	d_bind_int(":I131_ROICounts",eff->I131ROICounts);
	d_bind_double(":I131_ROICPM",eff->I131ROICPM);
	d_bind_int(":I131_BackgroundROICounts",eff->I131BackgroundROICounts);
	d_bind_double(":I131_BackgroundROICPM",eff->I131BackgroundROICPM);
	d_bind_double(":I131_NetROICPM",eff->I131NetROICPM);
	d_bind_int(":I131_I125_ROICounts",eff->I131I125ROICounts);
	d_bind_double(":I131_I125_ROICPM",eff->I131I125ROICPM);
	d_bind_int(":I131_I125_BackgroundROICounts",eff->I131I125BackgroundROICounts);
	d_bind_double(":I131_I125_BackgroundROICPM",eff->I131I125BackgroundROICPM);
	d_bind_double(":I131_I125_NetROICPM",eff->I131I125NetROICPM);
	if(eff->I125Measured) d_bind_int(":I125Measured",1);
	else d_bind_int(":I125Measured",0);
	d_bind_double(":I125_Activity",eff->I125Activity);
	stringout(datetimestr, &(eff->I125Date));
	d_bind_text(":I125_Date",datetimestr);
	d_bind_double(":I125_PrimaryEnergy",eff->I125PrimaryEnergy);
	d_bind_double(":I125_startEV", eff->I125StartEV);
	d_bind_double(":I125_endEV", eff->I125EndEV);
	d_bind_int64(":I125BackgroundID",eff->I125BackgroundID);
	d_bind_int64(":I125_SpectraID",eff->I125SpectraID);
	d_bind_int(":I125_ROICounts",eff->I125ROICounts);
	d_bind_double(":I125_ROICPM",eff->I125ROICPM);
	d_bind_int(":I125_BackgroundROICounts",eff->I125BackgroundROICounts);
	d_bind_double(":I125_BackgroundROICPM",eff->I125BackgroundROICPM);
	d_bind_double(":I125_NetROICPM",eff->I125NetROICPM);
	if(eff->I123Measured) d_bind_int(":I123Measured",1);
	else d_bind_int(":I123Measured",0);
	d_bind_double(":I123_Activity",eff->I123Activity);
	stringout(datetimestr, &(eff->I123Date));
	d_bind_text(":I123_Date",datetimestr);
	d_bind_double(":I123_PrimaryEnergy",eff->I123PrimaryEnergy);
	d_bind_double(":I123_startEV", eff->I123StartEV);
	d_bind_double(":I123_endEV", eff->I123EndEV);
	d_bind_double(":I123_I125_startEV", eff->I123I125StartEV);
	d_bind_double(":I123_I125_endEV", eff->I123I125EndEV);
	d_bind_int64(":I123BackgroundID",eff->I123BackgroundID);
	d_bind_int64(":I123_SpectraID",eff->I123SpectraID);
	d_bind_int(":I123_ROICounts",eff->I123ROICounts);
	d_bind_double(":I123_ROICPM",eff->I123ROICPM);
	d_bind_int(":I123_BackgroundROICounts",eff->I123BackgroundROICounts);
	d_bind_double(":I123_BackgroundROICPM",eff->I123BackgroundROICPM);
	d_bind_double(":I123_NetROICPM",eff->I123NetROICPM);
	d_bind_int(":I123_I125_ROICounts",eff->I123I125ROICounts);
	d_bind_double(":I123_I125_ROICPM",eff->I123I125ROICPM);
	d_bind_int(":I123_I125_BackgroundROICounts",eff->I123I125BackgroundROICounts);
	d_bind_double(":I123_I125_BackgroundROICPM",eff->I123I125BackgroundROICPM);
	d_bind_double(":I123_I125_NetROICPM",eff->I123I125NetROICPM);
	d_bind_int(":Inactive",0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
	else rowid = -1;
	d_finalize();

	eff->ProbeBioAssayEfficiencyID = rowid;

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_WriteProbeBioAssayTest(PROBEBIOASSAYTEST *test, bool bookEnd){
	sqlite3_int64 rowid;
	char datetimestr[30];

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO ProbeBioAssayTest(UserName,FirstName,LastName,ProbeDistance,I131_Active,I131_PrimaryEnergy,I131_startEV,I131_endEV,I131_ROICounts,I131_ROICPM,I131_BackgroundROICounts,I131_BackgroundROICPM,I131_NetROICPM,I131_Activity,I131_High,I131_I125_NetROICPM,I125_Active,I125_PrimaryEnergy,I125_startEV,I125_endEV,I125_ROICounts,I125_ROICPM,I125_BackgroundROICounts,I125_BackgroundROICPM,I125_NetROICPM,I125_NetROICPMCorrected,I125_Activity,I125_High,I123_Active,I123_PrimaryEnergy,I123_startEV,I123_endEV,I123_ROICounts,I123_ROICPM,I123_BackgroundROICounts,I123_BackgroundROICPM,I123_NetROICPM,I123_Activity,I123_High,I123_I125_NetROICPM,WellBackgroundID,SpectraID,ProbeBioAssayEfficiencyID,ProbeBioAssaySettingID,CreatedOn,Comment,InactiveReason,Inactive) VALUES(:UserName,:FirstName,:LastName,:ProbeDistance,:I131_Active,:I131_PrimaryEnergy,:I131_startEV,:I131_endEV,:I131_ROICounts,:I131_ROICPM,:I131_BackgroundROICounts,:I131_BackgroundROICPM,:I131_NetROICPM,:I131_Activity,:I131_High,:I131_I125_NetROICPM,:I125_Active,:I125_PrimaryEnergy,:I125_startEV,:I125_endEV,:I125_ROICounts,:I125_ROICPM,:I125_BackgroundROICounts,:I125_BackgroundROICPM,:I125_NetROICPM,:I125_NetROICPMCorrected,:I125_Activity,:I125_High,:I123_Active,:I123_PrimaryEnergy,:I123_startEV,:I123_endEV,:I123_ROICounts,:I123_ROICPM,:I123_BackgroundROICounts,:I123_BackgroundROICPM,:I123_NetROICPM,:I123_Activity,:I123_High,:I123_I125_NetROICPM,:WellBackgroundID,:SpectraID,:ProbeBioAssayEfficiencyID,:ProbeBioAssaySettingID,julianday(:CreatedOn),:Comment,:InactiveReason,:Inactive);");
	d_bind_text(":UserName", test->UserName);
	d_bind_text(":FirstName", test->FirstName);
	d_bind_text(":LastName", test->LastName);
	d_bind_int(":ProbeDistance", test->ProbeDistance);
	if(test->I131Active) d_bind_int(":I131_Active", 1);
	else d_bind_int(":I131_Active", 0);
	d_bind_double(":I131_PrimaryEnergy", test->I131PrimaryEnergy);
	d_bind_double(":I131_startEV", test->I131StartEV);
	d_bind_double(":I131_endEV", test->I131EndEV);
	d_bind_int(":I131_ROICounts", test->I131ROICounts);
	d_bind_double(":I131_ROICPM", test->I131ROICPM);
	d_bind_int(":I131_BackgroundROICounts", test->I131BackgroundROICounts);
	d_bind_double(":I131_BackgroundROICPM", test->I131BackgroundROICPM);
	d_bind_double(":I131_NetROICPM", test->I131NetROICPM);
	d_bind_double(":I131_Activity", test->I131Activity);
	if(test->I131High) d_bind_int(":I131_High", 1);
	else d_bind_int(":I131_High", 0);
	d_bind_double(":I131_I125_NetROICPM", test->I131I125NetROICPM);
	if(test->I125Active) d_bind_int(":I125_Active", 1);
	else d_bind_int(":I125_Active", 0);
	d_bind_double(":I125_PrimaryEnergy", test->I125PrimaryEnergy);
	d_bind_double(":I125_startEV", test->I125StartEV);
	d_bind_double(":I125_endEV", test->I125EndEV);
	d_bind_int(":I125_ROICounts", test->I125ROICounts);
	d_bind_double(":I125_ROICPM", test->I125ROICPM);
	d_bind_int(":I125_BackgroundROICounts", test->I125BackgroundROICounts);
	d_bind_double(":I125_BackgroundROICPM", test->I125BackgroundROICPM);
	d_bind_double(":I125_NetROICPM", test->I125NetROICPM);
	d_bind_double(":I125_NetROICPMCorrected", test->I125NetROICPMCorrected);
	d_bind_double(":I125_Activity", test->I125Activity);
	if(test->I125High) d_bind_int(":I125_High", 1);
	else d_bind_int(":I125_High", 0);
	if(test->I123Active) d_bind_int(":I123_Active", 1);
	else d_bind_int(":I123_Active", 0);
	d_bind_double(":I123_PrimaryEnergy", test->I123PrimaryEnergy);
	d_bind_double(":I123_startEV", test->I123StartEV);
	d_bind_double(":I123_endEV", test->I123EndEV);
	d_bind_int(":I123_ROICounts", test->I123ROICounts);
	d_bind_double(":I123_ROICPM", test->I123ROICPM);
	d_bind_int(":I123_BackgroundROICounts", test->I123BackgroundROICounts);
	d_bind_double(":I123_BackgroundROICPM", test->I123BackgroundROICPM);
	d_bind_double(":I123_NetROICPM", test->I123NetROICPM);
	d_bind_double(":I123_Activity", test->I123Activity);
	if(test->I123High) d_bind_int(":I123_High", 1);
	else d_bind_int(":I123_High", 0);
	d_bind_double(":I123_I125_NetROICPM", test->I123I125NetROICPM);
	d_bind_int64(":WellBackgroundID", test->WellBackgroundID);
	d_bind_int64(":SpectraID", test->SpectraID);
	d_bind_int64(":ProbeBioAssayEfficiencyID", test->ProbeBioAssayEfficiencyID);
	d_bind_int64(":ProbeBioAssaySettingID", test->ProbeBioAssaySettingID);
	stringout(datetimestr, &(test->CreatedOn));
	d_bind_text(":CreatedOn", datetimestr);
	d_bind_text(":Comment", test->Comment);
	d_bind_text(":InactiveReason", test->InactiveReason);
	if(test->Inactive) d_bind_int(":Inactive", 1);
	else d_bind_int(":Inactive", 0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
	else rowid = -1;
	d_finalize();

	test->ProbeBioAssayTestID = rowid;

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

int DB_SearchBioAssayTests(WELLBIOASSAYSEARCH *wellBioAssay, time_t startDate, time_t endDate, int maxRetrieved, int staffID){
	int retrievedCount, index;
	WELLBIOASSAYSEARCH *testPtr;
	char datestring[30];
	time_t buffer;
	USER user;

	DB_GetUserFromID(staffID, &user);

	testPtr = wellBioAssay;
	for(index=0; index<maxRetrieved; index++){
		testPtr->ProbeBioAssayTestID = 0;
		testPtr++;
	}
	testPtr = wellBioAssay;
	retrievedCount = 0;

	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT ProbeBioAssayTestID, I131_Active, I131_NetROICPM, I131_Activity, I131_High, I125_Active, I125_NetROICPMCorrected, I125_Activity, I125_High, I123_Active, I123_NetROICPM, I123_Activity, I123_High, datetime(CreatedOn), Inactive FROM ProbeBioAssayTest WHERE CreatedOn >= julianday(:startDate) AND CreatedOn < julianday(:endDate) AND UserName = :UserName ORDER BY CreatedOn DESC;");

	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);

	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);

	d_bind_text(":UserName", user.UserName);

	d_step();
	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->ProbeBioAssayTestID = d_column_int64(0); //ProbeBioAssayTestID

				if(d_column_int(1) == 0) testPtr->I131Active = FALSE; //I131_Active
				else testPtr->I131Active = TRUE; //I131_Active

				testPtr->I131NetROICPM = d_column_double(2); //I131_NetROICPM

				testPtr->I131Activity = d_column_double(3); //I131_Activity

				if(d_column_int(4) == 0) testPtr->I131High = FALSE; //I131_High
				else testPtr->I131High = TRUE; //I131_High

				if(d_column_int(5) == 0) testPtr->I125Active = FALSE; //I125_Active
				else testPtr->I125Active = TRUE; //I125_Active

				testPtr->I125NetROICPMCorrected = d_column_double(6); //I125_NetROICPMCorrected

				testPtr->I125Activity = d_column_double(7); //I125_Activity

				if(d_column_int(8) == 0) testPtr->I125High = FALSE; //I125_High
				else testPtr->I125High = TRUE; //I125_High

				if(d_column_int(9) == 0) testPtr->I123Active = FALSE; //I123_Active
				else testPtr->I123Active = TRUE; //I123_Active

				testPtr->I123NetROICPM = d_column_double(10); //I123_NetROICPM

				testPtr->I123Activity = d_column_double(11); //I123_Activity

				if(d_column_int(12) == 0) testPtr->I123High = FALSE; //I123_High
				else testPtr->I123High = TRUE; //I123_High

				testPtr->CreatedOn = strtotime_t((const char *) d_column_text(13));

				if(d_column_int(14) == 0) testPtr->Inactive = FALSE; //Inactive
				else testPtr->Inactive = TRUE; //Inactive

				testPtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();
	return retrievedCount;
}

int DB_SearchBioAssayTestsAll(WELLBIOASSAYSEARCH *wellBioAssay, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount, index;
	WELLBIOASSAYSEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = wellBioAssay;
	for(index=0; index<maxRetrieved; index++){
		testPtr->ProbeBioAssayTestID = 0;
		testPtr++;
	}
	testPtr = wellBioAssay;
	retrievedCount = 0;

	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT ProbeBioAssayTestID, I131_Active, I131_NetROICPM, I131_Activity, I131_High, I125_Active, I125_NetROICPMCorrected, I125_Activity, I125_High, I123_Active, I123_NetROICPM, I123_Activity, I123_High, datetime(CreatedOn), Inactive FROM ProbeBioAssayTest WHERE CreatedOn >= julianday(:startDate) AND CreatedOn < julianday(:endDate) ORDER BY CreatedOn DESC;");

	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);

	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);

	d_step();
	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->ProbeBioAssayTestID = d_column_int64(0); //ProbeBioAssayTestID

				if(d_column_int(1) == 0) testPtr->I131Active = FALSE; //I131_Active
				else testPtr->I131Active = TRUE; //I131_Active

				testPtr->I131NetROICPM = d_column_double(2); //I131_NetROICPM

				testPtr->I131Activity = d_column_double(3); //I131_Activity

				if(d_column_int(4) == 0) testPtr->I131High = FALSE; //I131_High
				else testPtr->I131High = TRUE; //I131_High

				if(d_column_int(5) == 0) testPtr->I125Active = FALSE; //I125_Active
				else testPtr->I125Active = TRUE; //I125_Active

				testPtr->I125NetROICPMCorrected = d_column_double(6); //I125_NetROICPMCorrected

				testPtr->I125Activity = d_column_double(7); //I125_Activity

				if(d_column_int(8) == 0) testPtr->I125High = FALSE; //I125_High
				else testPtr->I125High = TRUE; //I125_High

				if(d_column_int(9) == 0) testPtr->I123Active = FALSE; //I123_Active
				else testPtr->I123Active = TRUE; //I123_Active

				testPtr->I123NetROICPM = d_column_double(10); //I123_NetROICPM

				testPtr->I123Activity = d_column_double(11); //I123_Activity

				if(d_column_int(12) == 0) testPtr->I123High = FALSE; //I123_High
				else testPtr->I123High = TRUE; //I123_High

				testPtr->CreatedOn = strtotime_t((const char *) d_column_text(13));

				if(d_column_int(14) == 0) testPtr->Inactive = FALSE; //Inactive
				else testPtr->Inactive = TRUE; //Inactive

				testPtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();
	return retrievedCount;
}

void DB_RetrieveBioAssayTest(PROBEBIOASSAYTEST *test){
	test->SpectraID = -1;
	test->Spectrum.SpectraID = -1;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ProbeBioAssayTestID, UserName,FirstName,LastName,ProbeDistance,I131_Active,I131_PrimaryEnergy,I131_startEV,I131_endEV,I131_ROICounts,I131_ROICPM,I131_BackgroundROICounts,I131_BackgroundROICPM,I131_NetROICPM,I131_Activity,I131_High,I131_I125_NetROICPM,I125_Active,I125_PrimaryEnergy,I125_startEV,I125_endEV,I125_ROICounts,I125_ROICPM,I125_BackgroundROICounts,I125_BackgroundROICPM,I125_NetROICPM,I125_NetROICPMCorrected,I125_Activity,I125_High,I123_Active,I123_PrimaryEnergy,I123_startEV,I123_endEV,I123_ROICounts,I123_ROICPM,I123_BackgroundROICounts,I123_BackgroundROICPM,I123_NetROICPM,I123_Activity,I123_High,I123_I125_NetROICPM,WellBackgroundID,SpectraID,ProbeBioAssayEfficiencyID,ProbeBioAssaySettingID,datetime(CreatedOn),Comment,InactiveReason,Inactive FROM ProbeBioAssayTest WHERE ProbeBioAssayTestID = :ProbeBioAssayTestID;");
	d_bind_int64(":ProbeBioAssayTestID", test->ProbeBioAssayTestID);
	d_step();

	if(*capStatus == SQLITE_ROW){
		test->ProbeBioAssayTestID = d_column_int64(0); //ProbeBioAssayTestID
		strcpy(test->UserName, (const char *) d_column_text(1)); //UserName
		strcpy(test->FirstName, (const char *) d_column_text(2)); //FirstName
		strcpy(test->LastName, (const char *) d_column_text(3)); //LastName
		test->ProbeDistance = d_column_int(4); //ProbeDistance
		if(d_column_int(5) == 0) test->I131Active = FALSE; //I131_Active
		else test->I131Active = TRUE; //I131_Active
		test->I131PrimaryEnergy = d_column_double(6); //I131_PrimaryEnergy
		test->I131StartEV = d_column_double(7); //I131_startEV
		test->I131EndEV = d_column_double(8); //I131_endEV
		test->I131ROICounts = d_column_int(9); //I131_ROICounts
		test->I131ROICPM = d_column_double(10); //I131_ROICPM
		test->I131BackgroundROICounts = d_column_int(11); //I131_BackgroundROICounts
		test->I131BackgroundROICPM = d_column_double(12);//I131_BackgroundROICPM
		test->I131NetROICPM = d_column_double(13); //I131_NetROICPM
		test->I131Activity = d_column_double(14); //I131_Activity
		if(d_column_int(15) == 0) test->I131High = FALSE; //I131_High
		else test->I131High = TRUE; //I131_High
		test->I131I125NetROICPM = d_column_double(16); //I131_I125_NetROICPM
		if(d_column_int(17) == 0) test->I125Active = FALSE; //I125_Active
		else test->I125Active = TRUE; //I125_Active
		test->I125PrimaryEnergy = d_column_double(18); //I125_PrimaryEnergy
		test->I125StartEV = d_column_double(19); //I125_startEV
		test->I125EndEV = d_column_double(20); //I125_endEV
		test->I125ROICounts = d_column_int(21); //I125_ROICounts
		test->I125ROICPM = d_column_double(22); //I125_ROICPM
		test->I125BackgroundROICounts = d_column_int(23); //I125_BackgroundROICounts
		test->I125BackgroundROICPM = d_column_double(24); //I125_BackgroundROICPM
		test->I125NetROICPM = d_column_double(25); //I125_NetROICPM
		test->I125NetROICPMCorrected = d_column_double(26); //I125_NetROICPMCorrected
		test->I125Activity = d_column_double(27); //I125_Activity
		if(d_column_int(28) == 0) test->I125High = FALSE; //I125_High
		else test->I125High = TRUE; //I125_High
		if(d_column_int(29) == 0) test->I123Active = FALSE; //I123_Active
		else test->I123Active = TRUE; //I123_Active
		test->I123PrimaryEnergy = d_column_double(30); //I123_PrimaryEnergy
		test->I123StartEV = d_column_double(31); //I123_startEV
		test->I123EndEV = d_column_double(32); //I123_endEV
		test->I123ROICounts = d_column_int(33); //I123_ROICounts
		test->I123ROICPM = d_column_double(34); //I123_ROICPM
		test->I123BackgroundROICounts = d_column_int(35); //I123_BackgroundROICounts
		test->I123BackgroundROICPM = d_column_double(36); //I123_BackgroundROICPM
		test->I123NetROICPM = d_column_double(37); //I123_NetROICPM
		test->I123Activity = d_column_double(38); //I123_Activity
		if(d_column_int(39) == 0) test->I123High = FALSE; //I123_High
		else test->I123High = TRUE; //I123_High
		test->I123I125NetROICPM = d_column_double(40); //I123_I125_NetROICPM
		test->WellBackgroundID = d_column_int64(41); //WellBackgroundID
		test->SpectraID = d_column_int64(42); //SpectraID
		test->ProbeBioAssayEfficiencyID = d_column_int64(43); //ProbeBioAssayEfficiencyID
		test->ProbeBioAssaySettingID = d_column_int64(44); //ProbeBioAssaySettingID
		test->CreatedOn = strtotime_t((const char *) d_column_text(45)); //CreatedOn
		strcpy(test->Comment, (const char *) d_column_text(46)); //Comment
	    strcpy(test->InactiveReason, (const char *) d_column_text(47)); //InactiveReason
	    if(d_column_int(48) == 0) test->Inactive = FALSE; //Inactive
	    else test->Inactive = TRUE; //Inactive
	}
	d_finalize();
}

void DB_ReadBioAssayROI(float *I131Lower, float *I131Upper, float *I125Lower, float *I125Upper, float *I123Lower, float *I123Upper){
	*capStatus = SQLITE_OK;
	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'BioAssayI131LowerEV';")){
		if(*capStatus == SQLITE_ROW){
			*I131Lower = d_column_double(0);
		}else{
			*I131Lower = 292.0;
		}
		d_finalize();
	}else{
		d_finalize();
		*I131Lower = 292.0;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI131LowerEV',:BioAssayI131LowerEV);");
		d_bind_double(":BioAssayI131LowerEV", 292.0);
		d_step();
		d_finalize();
	}

	*capStatus = SQLITE_OK;
	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'BioAssayI131UpperEV';")){
		if(*capStatus == SQLITE_ROW){
			*I131Upper = d_column_double(0);
		}else{
			*I131Upper = 437.0;
		}
		d_finalize();
	}else{
		d_finalize();
		*I131Upper = 437.0;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI131UpperEV',:BioAssayI131UpperEV);");
		d_bind_double(":BioAssayI131UpperEV", 437.0);
		d_step();
		d_finalize();
	}

	*capStatus = SQLITE_OK;
	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'BioAssayI125LowerEV';")){
		if(*capStatus == SQLITE_ROW){
			*I125Lower = d_column_double(0);
		}else{
			*I125Lower = 20.0;
		}
		d_finalize();
	}else{
		d_finalize();
		*I125Lower = 20.0;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI125LowerEV',:BioAssayI125LowerEV);");
		d_bind_double(":BioAssayI125LowerEV", 20.0);
		d_step();
		d_finalize();
	}

	*capStatus = SQLITE_OK;
	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'BioAssayI125UpperEV';")){
		if(*capStatus == SQLITE_ROW){
			*I125Upper = d_column_double(0);
		}else{
			*I125Upper = 50.0;
		}
		d_finalize();
	}else{
		d_finalize();
		*I125Upper = 50.0;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI125UpperEV',:BioAssayI125UpperEV);");
		d_bind_double(":BioAssayI125UpperEV", 50.0);
		d_step();
		d_finalize();
	}

	*capStatus = SQLITE_OK;
	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'BioAssayI123LowerEV';")){
		if(*capStatus == SQLITE_ROW){
			*I123Lower = d_column_double(0);
		}else{
			*I123Lower = 143.0;
		}
		d_finalize();
	}else{
		d_finalize();
		*I123Lower = 143.0;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI123LowerEV',:BioAssayI123LowerEV);");
		d_bind_double(":BioAssayI123LowerEV", 143.0);
		d_step();
		d_finalize();
		service_watchdog();
	}

	*capStatus = SQLITE_OK;
	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'BioAssayI123UpperEV';")){
		if(*capStatus == SQLITE_ROW){
			*I123Upper = d_column_double(0);
		}else{
			*I123Upper = 191.0;
		}
		d_finalize();
	}else{
		d_finalize();
		*I123Upper = 191.0;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('BioAssayI123UpperEV',:BioAssayI123UpperEV);");
		d_bind_double(":BioAssayI123UpperEV", 191.0);
		d_step();
		d_finalize();
	}
}

void DB_WriteBioAssayROI(float I131Lower, float I131Upper, float I125Lower, float I125Upper, float I123Lower, float I123Upper){
	*capStatus = SQLITE_OK;

	d_prepare_v2("UPDATE Config SET SettingValue = :BioAssayI131LowerEV WHERE SettingName = 'BioAssayI131LowerEV';");
	d_bind_double(":BioAssayI131LowerEV", I131Lower);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :BioAssayI131UpperEV WHERE SettingName = 'BioAssayI131UpperEV';");
	d_bind_double(":BioAssayI131UpperEV", I131Upper);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :BioAssayI125LowerEV WHERE SettingName = 'BioAssayI125LowerEV';");
	d_bind_double(":BioAssayI125LowerEV", I125Lower);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :BioAssayI125UpperEV WHERE SettingName = 'BioAssayI125UpperEV';");
	d_bind_double(":BioAssayI125UpperEV", I125Upper);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :BioAssayI123LowerEV WHERE SettingName = 'BioAssayI123LowerEV';");
	d_bind_double(":BioAssayI123LowerEV", I123Lower);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :BioAssayI123UpperEV WHERE SettingName = 'BioAssayI123UpperEV';");
	d_bind_double(":BioAssayI123UpperEV", I123Upper);
	d_step();
	d_finalize();
}

void DB_InactivateBioAssayTest(PROBEBIOASSAYTEST *probeBioAssayTest){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ProbeBioAssayTest SET InactiveReason = :InactiveReason, Inactive = 1 WHERE ProbeBioAssayTestID = :ProbeBioAssayTestID;");
	d_bind_text(":InactiveReason", probeBioAssayTest->InactiveReason);
	d_bind_int64(":ProbeBioAssayTestID", probeBioAssayTest->ProbeBioAssayTestID);
	d_step();
	d_finalize();
}

int DB_ActiveProbeThyroidUptakeProtocolCount(void){
	int returnvalue;
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT Count(*) FROM ProbeTUProtocol WHERE Inactive = 0;");
	d_step();
	if(*capStatus == SQLITE_ROW){
		returnvalue = d_column_int(0);
	}else{
		returnvalue = 0;
	}
	d_finalize();
	return returnvalue;
}

void DB_RetrieveActiveProbeThyroidUptakeProtocolNames(PROBETHYROIDUPTAKEPROTOCOLACTIVENAME *protocolname){
	PROBETHYROIDUPTAKEPROTOCOLACTIVENAME *ptr;

	ptr = protocolname;
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ProbeTUProtocolID, ProtocolName FROM ProbeTUProtocol WHERE Inactive = 0 ORDER BY ProtocolName");
	d_step();
	if(*capStatus == SQLITE_ROW){
		while(*capStatus == SQLITE_ROW){
			ptr->ProbeTUProtocolID = d_column_int64(0);
			strcpy(ptr->ProtocolName, (const char *) d_column_text(1));
			ptr++;
			d_step();
		}
	}
	d_finalize();
}

void DB_RetrieveProbeThyroidUptakeProtocol(PROBETHYROIDUPTAKEPROTOCOL *protocol){
	*capStatus = SQLITE_OK;

	d_prepare_v2("SELECT ProbeTUProtocolID,ProbeTUProtocolGroupID,ProtocolName,NuclideID,DoseForm,DoseMeasurementMethod,DoseDecayCorrect,PreDoseMeasurement,ResidualMeasurement,I123LowerEV,I123UpperEV,I131LowerEV,I131UpperEV,Tc99mLowerEV,Tc99mUpperEV,DefaultCountTime,DefaultDistance,datetime(CreatedOn),datetime(LastUpdated),Inactive FROM ProbeTUProtocol WHERE ProbeTUProtocolID = :ProbeTUProtocolID;");
	d_bind_int64(":ProbeTUProtocolID", protocol->ProbeTUProtocolID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		protocol->ProbeTUProtocolID = d_column_int64(0);						//ProbeTUProtocolID
		protocol->ProbeTUProtocolGroupID = d_column_int64(1);					//ProbeTUProtocolGroupID
		strcpy(protocol->ProtocolName,(const char *) d_column_text(2));			//ProtocolName
		protocol->NuclideID = d_column_int(3);									//NuclideID
		protocol->DoseForm = d_column_int(4);									//DoseForm
		protocol->DoseMeasurementMethod = d_column_int(5);						//DoseMeasurementMethod
		if(d_column_int(6) == 0) protocol->DoseDecayCorrect = FALSE;			//DoseDecayCorrect
		else protocol->DoseDecayCorrect = TRUE;									//DoseDecayCorrect
		if(d_column_int(7) == 0) protocol->PreDoseMeasurement = FALSE;			//PreDoseMeasurement
		else protocol->PreDoseMeasurement = TRUE;								//PreDoseMeasurement
		if(d_column_int(8) == 0) protocol->ResidualMeasurement = FALSE;			//ResidualMeasurement
		else protocol->ResidualMeasurement = TRUE;								//ResidualMeasurement
		protocol->I123LowerEV = d_column_double(9);								//I123LowerEV
		protocol->I123UpperEV = d_column_double(10);							//I123UpperEV
		protocol->I131LowerEV = d_column_double(11);							//I131LowerEV
		protocol->I131UpperEV = d_column_double(12);							//I131UpperEV
		protocol->Tc99mLowerEV = d_column_double(13);							//Tc99mLowerEV
		protocol->Tc99mUpperEV = d_column_double(14);							//Tc99mUpperEV
		protocol->DefaultCountTime = d_column_int(15);							//DefaultCountTime
		protocol->DefaultDistance = d_column_int(16);							//DefaultDistance
		protocol->CreatedOn = strtotime_t((const char *) d_column_text(17));	//CreatedOn
		protocol->LastUpdated = strtotime_t((const char *) d_column_text(18));	//LastUpdated
		if(d_column_int(19) == 0) protocol->Inactive = FALSE;					//Inactive
		else protocol->Inactive = TRUE;											//Inactive
	}else{
		protocol->ProbeTUProtocolID = 0;
	}
	d_finalize();
}

bool DB_TestProbeThyroidUptakeProtocolName(long long int ProbeTUProtocolID, char *protocolName){
	bool returnvalue;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ProbeTUProtocolID FROM ProbeTUProtocol WHERE Inactive = 0 AND ProtocolName LIKE :ProtocolName AND NOT ProbeTUProtocolID = :ProbeTUProtocolID;");
	d_bind_text(":ProtocolName", protocolName);
	d_bind_int64(":ProbeTUProtocolID", ProbeTUProtocolID);
	d_step();
	if(*capStatus == SQLITE_ROW) returnvalue = TRUE;
	else returnvalue = FALSE;
	d_finalize();

	return returnvalue;
}

void DB_WriteProbeThyroidUptakeProtocol(PROBETHYROIDUPTAKEPROTOCOL *protocol, bool bookEnd){
	long long int rowid;
	double CreatedOn;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	if(protocol->ProbeTUProtocolID == -1){
		d_prepare_v2("INSERT INTO ProbeTUProtocol(ProtocolName,NuclideID,DoseForm,DoseMeasurementMethod,DoseDecayCorrect,PreDoseMeasurement,ResidualMeasurement,I123LowerEV,I123UpperEV,I131LowerEV,I131UpperEV,Tc99mLowerEV,Tc99mUpperEV,DefaultCountTime,DefaultDistance,Inactive) VALUES(:ProtocolName,:NuclideID,:DoseForm,:DoseMeasurementMethod,:DoseDecayCorrect,:PreDoseMeasurement,:ResidualMeasurement,:I123LowerEV,:I123UpperEV,:I131LowerEV,:I131UpperEV,:Tc99mLowerEV,:Tc99mUpperEV,:DefaultCountTime,:DefaultDistance,:Inactive);");
		d_bind_text(":ProtocolName", protocol->ProtocolName);
		d_bind_int(":NuclideID", protocol->NuclideID);
		d_bind_int(":DoseForm", protocol->DoseForm);
		d_bind_int(":DoseMeasurementMethod", protocol->DoseMeasurementMethod);
		if(protocol->DoseDecayCorrect) d_bind_int(":DoseDecayCorrect", 1);
		else d_bind_int(":DoseDecayCorrect", 0);
		if(protocol->PreDoseMeasurement) d_bind_int(":PreDoseMeasurement", 1);
		else d_bind_int(":PreDoseMeasurement", 0);
		if(protocol->ResidualMeasurement) d_bind_int(":ResidualMeasurement", 1);
		else d_bind_int(":ResidualMeasurement", 0);
		d_bind_double(":I123LowerEV", protocol->I123LowerEV);
		d_bind_double(":I123UpperEV", protocol->I123UpperEV);
		d_bind_double(":I131LowerEV", protocol->I131LowerEV);
		d_bind_double(":I131UpperEV", protocol->I131UpperEV);
		d_bind_double(":Tc99mLowerEV", protocol->Tc99mLowerEV);
		d_bind_double(":Tc99mUpperEV", protocol->Tc99mUpperEV);
		d_bind_int(":DefaultCountTime", protocol->DefaultCountTime);
		d_bind_int(":DefaultDistance", protocol->DefaultDistance);
		d_bind_int(":Inactive", 0);
		d_step();
		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
		else rowid = -1;
		d_finalize();

		if(rowid != -1){
			*capStatus = SQLITE_OK;
			d_prepare_v2("UPDATE ProbeTUProtocol SET ProbeTUProtocolGroupID = ProbeTUProtocolID WHERE ProbeTUProtocolID = :ProbeTUProtocolID;");
			d_bind_int64(":ProbeTUProtocolID", rowid);
			d_step();
			d_finalize();
		}
		protocol->ProbeTUProtocolID = rowid;
		protocol->ProbeTUProtocolGroupID = rowid;
	}else{
		d_prepare_v2("SELECT CreatedOn FROM ProbeTUProtocol WHERE ProbeTUProtocolID = :ProbeTUProtocolID;");
		d_bind_int64(":ProbeTUProtocolID", protocol->ProbeTUProtocolID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			CreatedOn = d_column_double(0);
		}else{
			CreatedOn = 0;
		}
		d_finalize();

		*capStatus = SQLITE_OK;
		d_prepare_v2("UPDATE ProbeTUProtocol SET Inactive = 1 WHERE Inactive = 0 AND ProbeTUProtocolGroupID = :ProbeTUProtocolGroupID;");
		d_bind_int64(":ProbeTUProtocolGroupID", protocol->ProbeTUProtocolGroupID);
		d_step();
		d_finalize();

		*capStatus = SQLITE_OK;
		d_prepare_v2("INSERT INTO ProbeTUProtocol(ProbeTUProtocolGroupID,ProtocolName,NuclideID,DoseForm,DoseMeasurementMethod,DoseDecayCorrect,PreDoseMeasurement,ResidualMeasurement,I123LowerEV,I123UpperEV,I131LowerEV,I131UpperEV,Tc99mLowerEV,Tc99mUpperEV,DefaultCountTime,DefaultDistance,CreatedOn,Inactive) VALUES(:ProbeTUProtocolGroupID,:ProtocolName,:NuclideID,:DoseForm,:DoseMeasurementMethod,:DoseDecayCorrect,:PreDoseMeasurement,:ResidualMeasurement,:I123LowerEV,:I123UpperEV,:I131LowerEV,:I131UpperEV,:Tc99mLowerEV,:Tc99mUpperEV,:DefaultCountTime,:DefaultDistance,:CreatedOn,:Inactive);");
		d_bind_int64(":ProbeTUProtocolGroupID", protocol->ProbeTUProtocolGroupID);
		d_bind_text(":ProtocolName", protocol->ProtocolName);
		d_bind_int(":NuclideID", protocol->NuclideID);
		d_bind_int(":DoseForm", protocol->DoseForm);
		d_bind_int(":DoseMeasurementMethod", protocol->DoseMeasurementMethod);
		if(protocol->DoseDecayCorrect) d_bind_int(":DoseDecayCorrect", 1);
		else d_bind_int(":DoseDecayCorrect", 0);
		if(protocol->PreDoseMeasurement) d_bind_int(":PreDoseMeasurement", 1);
		else d_bind_int(":PreDoseMeasurement", 0);
		if(protocol->ResidualMeasurement) d_bind_int(":ResidualMeasurement", 1);
		else d_bind_int(":ResidualMeasurement", 0);
		d_bind_double(":I123LowerEV", protocol->I123LowerEV);
		d_bind_double(":I123UpperEV", protocol->I123UpperEV);
		d_bind_double(":I131LowerEV", protocol->I131LowerEV);
		d_bind_double(":I131UpperEV", protocol->I131UpperEV);
		d_bind_double(":Tc99mLowerEV", protocol->Tc99mLowerEV);
		d_bind_double(":Tc99mUpperEV", protocol->Tc99mUpperEV);
		d_bind_int(":DefaultCountTime", protocol->DefaultCountTime);
		d_bind_int(":DefaultDistance", protocol->DefaultDistance);
		d_bind_double(":CreatedOn", CreatedOn);
		d_bind_int(":Inactive", 0);
		d_step();
		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
		else rowid = -1;
		protocol->ProbeTUProtocolID = rowid;
		d_finalize();
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_InactivateProbeThyroidUptakeProtocol(PROBETHYROIDUPTAKEPROTOCOL *protocol){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ProbeTUProtocol SET Inactive = 1 WHERE ProbeTUProtocolGroupID = :ProbeTUProtocolGroupID;");
	d_bind_int64(":ProbeTUProtocolGroupID", protocol->ProbeTUProtocolGroupID);
	d_step();
	d_finalize();
}

int DB_SearchThyroidUptakeTests(PROBETHYROIDUPTAKESEARCH *probeThyroidUptake, char *lastName, char *patientID, bool active){
	int retrievedCount;
	PROBETHYROIDUPTAKESEARCH *ptr;

	retrievedCount = 0;
	ptr = probeThyroidUptake;

	*capStatus = SQLITE_OK;
	if(active){
		if((strlen(lastName) == 0) && (strlen(patientID) == 0)){
			d_prepare_v2("SELECT ProbeTUTestID, PatientID, FirstName, LastName, NuclideName, DoseAdministeredOn, datetime(DoseAdministeredOn), Inactive FROM ProbeTUTest WHERE Inactive = 0 ORDER BY DoseAdministeredOn DESC, CreatedOn DESC LIMIT 100;");
		}else if((strlen(lastName) != 0) && (strlen(patientID) == 0)){
			d_prepare_v2("SELECT ProbeTUTestID, PatientID, FirstName, LastName, NuclideName, DoseAdministeredOn, datetime(DoseAdministeredOn), Inactive FROM ProbeTUTest WHERE Inactive = 0 AND LastName LIKE :LastName ORDER BY DoseAdministeredOn DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":LastName", lastName);
		}else if((strlen(lastName) == 0) && (strlen(patientID) != 0)){
			d_prepare_v2("SELECT ProbeTUTestID, PatientID, FirstName, LastName, NuclideName, DoseAdministeredOn, datetime(DoseAdministeredOn), Inactive FROM ProbeTUTest WHERE Inactive = 0 AND PatientID LIKE :PatientID ORDER BY DoseAdministeredOn DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":PatientID", patientID);
		}else{
			d_prepare_v2("SELECT ProbeTUTestID, PatientID, FirstName, LastName, NuclideName, DoseAdministeredOn, datetime(DoseAdministeredOn), Inactive FROM ProbeTUTest WHERE Inactive = 0 AND LastName LIKE :LastName AND PatientID LIKE :PatientID ORDER BY DoseAdministeredOn DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":LastName", lastName);
			d_bind_text(":PatientID", patientID);
		}
	}else{
		if((strlen(lastName) == 0) && (strlen(patientID) == 0)){
			d_prepare_v2("SELECT ProbeTUTestID, PatientID, FirstName, LastName, NuclideName, DoseAdministeredOn, datetime(DoseAdministeredOn), Inactive FROM ProbeTUTest WHERE Inactive != 0 ORDER BY DoseAdministeredOn DESC, CreatedOn DESC LIMIT 100;");
		}else if((strlen(lastName) != 0) && (strlen(patientID) == 0)){
			d_prepare_v2("SELECT ProbeTUTestID, PatientID, FirstName, LastName, NuclideName, DoseAdministeredOn, datetime(DoseAdministeredOn), Inactive FROM ProbeTUTest WHERE Inactive != 0 AND LastName LIKE :LastName ORDER BY DoseAdministeredOn DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":LastName", lastName);
		}else if((strlen(lastName) == 0) && (strlen(patientID) != 0)){
			d_prepare_v2("SELECT ProbeTUTestID, PatientID, FirstName, LastName, NuclideName, DoseAdministeredOn, datetime(DoseAdministeredOn), Inactive FROM ProbeTUTest WHERE Inactive != 0 AND PatientID LIKE :PatientID ORDER BY DoseAdministeredOn DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":PatientID", patientID);
		}else{
			d_prepare_v2("SELECT ProbeTUTestID, PatientID, FirstName, LastName, NuclideName, DoseAdministeredOn, datetime(DoseAdministeredOn), Inactive FROM ProbeTUTest WHERE Inactive != 0 AND LastName LIKE :LastName AND PatientID LIKE :PatientID ORDER BY DoseAdministeredOn DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":LastName", lastName);
			d_bind_text(":PatientID", patientID);
		}
	}
	d_step();

	if(*capStatus == SQLITE_ROW){
		while(*capStatus == SQLITE_ROW){
			ptr->ProbeTUTestID = d_column_int64(0); //ProbeTUTestID
			strcpy(ptr->PatientID, (const char *) d_column_text(1)); //PatientID
			strcpy(ptr->FirstName, (const char *) d_column_text(2)); //FirstName
			strcpy(ptr->LastName, (const char *) d_column_text(3)); //LastName
			strcpy(ptr->NuclideName, (const char *) d_column_text(4)); //NuclideName
			if(d_column_double(5) >= 1e+9){ //DoseAdministeredOn
				ptr->DoseAdministeredOn = (time_t) 0;
			}else{
				ptr->DoseAdministeredOn = strtotime_t((const char *) d_column_text(6)); //datetime(DoseAdministeredOn)
			}
			if(d_column_int(7) == 0) ptr->Inactive = FALSE;
			else ptr->Inactive = TRUE;

			retrievedCount++;
			ptr++;
			d_step();
		}
	}
	d_finalize();

	return retrievedCount;
}

void DB_RetrieveProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test){
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ProbeTUTestID,ProbeTUTestPhase,PatientID,FirstName,LastName,DateOfBirth,Sex,Physician,TechID,NuclideName,HalfLife,HalfLifeUnit,StartROI,EndROI,PeakEV,CountingTime,DoseMultiplier,DoseAdministeredOn,datetime(DoseAdministeredOn),CalibrationActivity,CalibrationDate,datetime(CalibrationDate),LotNum,ProbeDistance,AccessionNumber,ProbeTUProtocolID,datetime(CreatedOn),datetime(LastUpdated),InactiveReason,Inactive FROM ProbeTUTest WHERE ProbeTUTestID = :ProbeTUTestID;");
	d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		test->ProbeTUTestID = d_column_int64(0);						//ProbeTUTestID
		test->ProbeTUTestPhase = d_column_int(1);						//ProbeTUTestPhase
		strcpy(test->PatientID, (const char *) d_column_text(2));		//PatientID
		strcpy(test->FirstName, (const char *) d_column_text(3));		//FirstName
		strcpy(test->LastName, (const char *) d_column_text(4));		//LastName
		test->DateOfBirth = d_column_double(5);
		strcpy(test->Sex, (const char *) d_column_text(6));				//Sex
		strcpy(test->Physician, (const char *) d_column_text(7));		//Physician
		strcpy(test->TechID, (const char *) d_column_text(8));			//TechID
		strcpy(test->NuclideName, (const char *) d_column_text(9));	//NuclideName
		test->HalfLife = d_column_double(10);							//HalfLife
		test->HalfLifeUnit = d_column_int(11);							//HalfLifeUnit
		test->StartROI = d_column_double(12);							//StartROI
		test->EndROI = d_column_double(13);								//EndROI
		test->PeakEV = d_column_double(14);								//PeakEV
		test->CountingTime = d_column_int(15);							//CountingTime
		test->DoseMultiplier = d_column_double(16);						//DoseMultiplier
		if(d_column_double(17) >= 1e+9){								//DoseAdministeredOn
			test->DoseAdministeredOn = (time_t) 0;
		}else{
			test->DoseAdministeredOn = strtotime_t((const char *) d_column_text(18));	//datetime(DoseAdministeredOn)
		}
		test->CalibrationActivity = d_column_double(19);				//CalibrationActivity
		if(d_column_double(20) == 0){									//CalibrationDate
			test->CalibrationDate = (time_t) 0;
		}else{
			test->CalibrationDate = strtotime_t((const char *) d_column_text(21));		//datetime(CalibrationDate)
		}
		strcpy(test->LotNum, (const char *) d_column_text(22));			//LotNum
		test->ProbeDistance = d_column_int(23);							//ProbeDistance
		strcpy(test->AccessionNumber, (const char *) d_column_text(24));//AccessionNumber
		test->ProbeTUProtocolID = d_column_int64(25);					//ProbeTUProtocolID
		test->CreatedOn = strtotime_t((const char *) d_column_text(26));				//datetime(CreatedOn)
		test->LastUpdated = strtotime_t((const char *) d_column_text(27));				//datetime(LastUpdated)
		strcpy(test->InactiveReason, (const char *) d_column_text(28));	//InactiveReason
		if(d_column_int(29) == 0) test->Inactive = FALSE;				//Inactive
		else test->Inactive = TRUE;
	}else{
		test->ProbeTUTestID = 0;
	}
	d_finalize();
}

int DB_GetActiveTUProtocol(void){
	int returnvalue;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT Count(*) FROM ProbeTUProtocol WHERE Inactive = 0;");
	d_step();
	if(*capStatus == SQLITE_ROW){
		returnvalue = d_column_int(0);
	}
	d_finalize();

	return returnvalue;
}

void DB_WriteProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test, bool bookEnd){
	sqlite3_int64 rowid;
	char datetimestr[30];
	bool flgChanged;

	if(test->ProbeTUTestID != -1){
		originalTUTest.ProbeTUTestID = test->ProbeTUTestID;
		DB_RetrieveProbeThyroidUptakeTest(&originalTUTest);
	}

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	if(test->ProbeTUTestID == -1){
		d_prepare_v2("INSERT INTO ProbeTUTest(ProbeTUTestPhase,PatientID,FirstName,LastName,DateOfBirth,Sex,Physician,TechID,NuclideName,HalfLife,HalfLifeUnit,StartROI,EndROI,PeakEV,CountingTime,DoseMultiplier,DoseAdministeredOn,CalibrationActivity,CalibrationDate,LotNum,ProbeDistance,AccessionNumber,ProbeTUProtocolID,InactiveReason,Inactive) VALUES(:ProbeTUTestPhase,:PatientID,:FirstName,:LastName,:DateOfBirth,:Sex,:Physician,:TechID,:NuclideName,:HalfLife,:HalfLifeUnit,:StartROI,:EndROI,:PeakEV,:CountingTime,:DoseMultiplier,:DoseAdministeredOn,:CalibrationActivity,julianday(:CalibrationDate),:LotNum,:ProbeDistance,:AccessionNumber,:ProbeTUProtocolID,:InactiveReason,:Inactive);");
		d_bind_int(":ProbeTUTestPhase", test->ProbeTUTestPhase);
		d_bind_text(":PatientID", test->PatientID);
		d_bind_text(":FirstName", test->FirstName);
		d_bind_text(":LastName", test->LastName);
		d_bind_double(":DateOfBirth", test->DateOfBirth);
		d_bind_text(":Sex", test->Sex);
		d_bind_text(":Physician", test->Physician);
		d_bind_text(":TechID", test->TechID);
		d_bind_text(":NuclideName", test->NuclideName);
		d_bind_double(":HalfLife", test->HalfLife);
		d_bind_int(":HalfLifeUnit", test->HalfLifeUnit);
		d_bind_double(":StartROI", test->StartROI);
		d_bind_double(":EndROI", test->EndROI);
		d_bind_double(":PeakEV", test->PeakEV);
		d_bind_int(":CountingTime", test->CountingTime);
		d_bind_double(":DoseMultiplier", test->DoseMultiplier);
		d_bind_double(":DoseAdministeredOn", 1e+9);
		d_bind_double(":CalibrationActivity", test->CalibrationActivity);
		stringout(datetimestr, &(test->CalibrationDate));
		d_bind_text(":CalibrationDate", datetimestr);
		d_bind_text(":LotNum", test->LotNum);
		d_bind_int(":ProbeDistance", test->ProbeDistance);
		d_bind_text(":AccessionNumber", test->AccessionNumber);
		d_bind_int64(":ProbeTUProtocolID", test->ProbeTUProtocolID);
		d_bind_text(":InactiveReason", test->InactiveReason);
		d_bind_int(":Inactive", 0);
		d_step();
		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
		else rowid = -1;
		d_finalize();
		test->ProbeTUTestID = rowid;
	}else{
		flgChanged = FALSE;

		if(originalTUTest.ProbeTUTestPhase != test->ProbeTUTestPhase){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET ProbeTUTestPhase = :ProbeTUTestPhase WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_int(":ProbeTUTestPhase", test->ProbeTUTestPhase);
			d_step();
			d_finalize();
		}

		if(strcmp(originalTUTest.PatientID, test->PatientID) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET PatientID = :PatientID WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_text(":PatientID", test->PatientID);
			d_step();
			d_finalize();
		}

		if(strcmp(originalTUTest.FirstName, test->FirstName) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET FirstName = :FirstName WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_text(":FirstName", test->FirstName);
			d_step();
			d_finalize();
		}

		if(strcmp(originalTUTest.LastName, test->LastName) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET LastName = :LastName WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_text(":LastName", test->LastName);
			d_step();
			d_finalize();
		}

		if(originalTUTest.DateOfBirth != test->DateOfBirth){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET DateOfBirth = :DateOfBirth WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_double(":DateOfBirth", test->DateOfBirth);
			d_step();
			d_finalize();
		}

		if(strcmp(originalTUTest.Sex, test->Sex) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET Sex = :Sex WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_text(":Sex", test->Sex);
			d_step();
			d_finalize();
		}

		if(strcmp(originalTUTest.Physician, test->Physician) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET Physician = :Physician WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_text(":Physician", test->Physician);
			d_step();
			d_finalize();
		}

		if(strcmp(originalTUTest.TechID, test->TechID) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET TechID = :TechID WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_text(":TechID", test->TechID);
			d_step();
			d_finalize();
		}

		if(strcmp(originalTUTest.NuclideName, test->NuclideName) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET NuclideName = :NuclideName WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_text(":NuclideName", test->NuclideName);
			d_step();
			d_finalize();
		}

		if(originalTUTest.HalfLife != test->HalfLife){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET HalfLife = :HalfLife WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_double(":HalfLife", test->HalfLife);
			d_step();
			d_finalize();
		}

		if(originalTUTest.HalfLifeUnit != test->HalfLifeUnit){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET HalfLifeUnit = :HalfLifeUnit WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_int(":HalfLifeUnit", test->HalfLifeUnit);
			d_step();
			d_finalize();
		}

		if(originalTUTest.StartROI != test->StartROI){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET StartROI = :StartROI WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_double(":StartROI", test->StartROI);
			d_step();
			d_finalize();
		}

		if(originalTUTest.EndROI != test->EndROI){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET EndROI = :EndROI WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_double(":EndROI", test->EndROI);
			d_step();
			d_finalize();
		}

		if(originalTUTest.PeakEV != test->PeakEV){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET PeakEV = :PeakEV WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_double(":PeakEV", test->PeakEV);
			d_step();
			d_finalize();
		}

		if(originalTUTest.CountingTime != test->CountingTime){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET CountingTime = :CountingTime WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_int(":CountingTime", test->CountingTime);
			d_step();
			d_finalize();
		}

		if(originalTUTest.DoseMultiplier != test->DoseMultiplier){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET DoseMultiplier = :DoseMultiplier WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_double(":DoseMultiplier", test->DoseMultiplier);
			d_step();
			d_finalize();
		}

		if(originalTUTest.DoseAdministeredOn != test->DoseAdministeredOn){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET DoseAdministeredOn = julianday(:DoseAdministeredOn) WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			stringout(datetimestr, &(test->DoseAdministeredOn));
			d_bind_text(":DoseAdministeredOn", datetimestr);
			d_step();
			d_finalize();
		}

		if(originalTUTest.CalibrationActivity != test->CalibrationActivity){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET CalibrationActivity = :CalibrationActivity WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_double(":CalibrationActivity", test->CalibrationActivity);
			d_step();
			d_finalize();
		}

		if(originalTUTest.CalibrationDate != test->CalibrationDate){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET CalibrationDate = julianday(:CalibrationDate) WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			stringout(datetimestr, &(test->CalibrationDate));
			d_bind_text(":CalibrationDate", datetimestr);
			d_step();
			d_finalize();
		}

		if(strcmp(originalTUTest.LotNum, test->LotNum) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET LotNum = :LotNum WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_text(":LotNum", test->LotNum);
			d_step();
			d_finalize();
		}

		if(originalTUTest.ProbeDistance != test->ProbeDistance){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET ProbeDistance = :ProbeDistance WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_int(":ProbeDistance", test->ProbeDistance);
			d_step();
			d_finalize();
		}

		if(strcmp(originalTUTest.AccessionNumber, test->AccessionNumber) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET AccessionNumber = :AccessionNumber WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_text(":AccessionNumber", test->AccessionNumber);
			d_step();
			d_finalize();
		}

		if(originalTUTest.ProbeTUProtocolID != test->ProbeTUProtocolID){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET ProbeTUProtocolID = :ProbeTUProtocolID WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_int64(":ProbeTUProtocolID", test->ProbeTUProtocolID);
			d_step();
			d_finalize();
		}

		if(strcmp(originalTUTest.InactiveReason, test->InactiveReason) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET InactiveReason = :InactiveReason WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_text(":InactiveReason", test->InactiveReason);
			d_step();
			d_finalize();
		}

		if(originalTUTest.Inactive != test->Inactive){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE ProbeTUTest SET Inactive = :Inactive WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			d_bind_int(":Inactive", test->Inactive);
			d_step();
			d_finalize();
		}

		if(flgChanged){
			d_prepare_v2("UPDATE ProbeTUTest SET LastUpdated = julianday(:LastUpdated) WHERE ProbeTUTestID = :ProbeTUTestID;");
			d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
			stringout(datetimestr, &clock_time);
			d_bind_text(":LastUpdated", datetimestr);
			d_step();
			d_finalize();
		}
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_DeleteProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test){
	*capStatus = SQLITE_OK;
	d_prepare_v2("DELETE FROM ProbeTUTest WHERE ProbeTUTestID = :ProbeTUTestID;");
	d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
	d_step();
	d_finalize();
}

void DB_InactivateProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test){
	char datetimestr[30];

	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ProbeTUTest SET LastUpdated = julianday(:LastUpdated), InactiveReason = :InactiveReason, Inactive = 1 WHERE ProbeTUTestID = :ProbeTUTestID;");
	stringout(datetimestr, &clock_time);
	d_bind_text(":LastUpdated", datetimestr);
	d_bind_text(":InactiveReason", test->InactiveReason);
	d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
	d_step();
	d_finalize();
}

void DB_ReactivateProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test){
	char datetimestr[30];

	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ProbeTUTest SET LastUpdated = julianday(:LastUpdated), Inactive = 0 WHERE ProbeTUTestID = :ProbeTUTestID;");
	stringout(datetimestr, &clock_time);
	d_bind_text(":LastUpdated", datetimestr);
	d_bind_int64(":ProbeTUTestID", test->ProbeTUTestID);
	d_step();
	d_finalize();
}

bool DB_ExistsThyroidUptakePreDose(long long int ProbeTUTestID){
	bool returnvalue;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ProbeTUPatientMeasurementID FROM ProbeTUPatientMeasurement WHERE MeasurementNumber = 0 AND Inactive = 0 AND ProbeTUTestID = :ProbeTUTestID;");
	d_bind_int64(":ProbeTUTestID", ProbeTUTestID);
	d_step();
	if(*capStatus == SQLITE_ROW) returnvalue = TRUE;
	else returnvalue = FALSE;
	d_finalize();

	return returnvalue;
}

bool DB_ExistsThyroidUptakeAdministeredDose(long long int ProbeTUTestID){
	bool returnvalue;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ProbeTUTotalDoseMeasurementID FROM ProbeTUTotalDoseMeasurement WHERE MeasurementNumber = 0 AND Inactive = 0 AND ProbeTUTestID = :ProbeTUTestID;");
	d_bind_int64(":ProbeTUTestID", ProbeTUTestID);
	d_step();
	if(*capStatus == SQLITE_ROW) returnvalue = TRUE;
	else returnvalue = FALSE;
	d_finalize();

	return returnvalue;
}

bool DB_ExistsThyroidUptakeResidualDose(long long int ProbeTUTestID){
	bool returnvalue;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ProbeTUTotalDoseMeasurementID FROM ProbeTUTotalDoseMeasurement WHERE MeasurementNumber = -1 AND Inactive = 0 AND ProbeTUTestID = :ProbeTUTestID;");
	d_bind_int64(":ProbeTUTestID", ProbeTUTestID);
	d_step();
	if(*capStatus == SQLITE_ROW) returnvalue = TRUE;
	else returnvalue = FALSE;
	d_finalize();

	return returnvalue;
}

int DB_NextThyroidUptakeMeasurementNumber(long long int ProbeTUTestID){
	int returnvalue;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT MAX(MeasurementNumber) FROM ProbeTUPatientMeasurement WHERE Inactive = 0 AND ProbeTUTestID = :ProbeTUTestID;");
	d_bind_int64(":ProbeTUTestID", ProbeTUTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		returnvalue = d_column_int(0);
		if(returnvalue < 1) returnvalue = 1;
		else returnvalue++;
	}else returnvalue = 1;
	d_finalize();

	return returnvalue;
}

void DB_WriteProbeThyroidUptakePatientMeasurement(PROBETHYROIDUPTAKEPATMEAS *measurement, bool bookEnd){
	sqlite3_int64 rowid;
	char CreatedOn[40];

	if((measurement->ProbeTUPatientMeasurementID == -1) && (measurement->Neck1_Spectrum.SpectraID == 0)){
		measurement->Neck1_SpectraID = DB_WriteSpectrum(&(measurement->Neck1_Spectrum), TRUE);
	}

	if((measurement->ProbeTUPatientMeasurementID == -1) && (measurement->Neck2_Spectrum.SpectraID == 0)){
		measurement->Neck2_SpectraID = DB_WriteSpectrum(&(measurement->Neck2_Spectrum), TRUE);
	}

	if((measurement->ProbeTUPatientMeasurementID == -1) && (measurement->Leg1_Spectrum.SpectraID == 0)){
		measurement->Leg1_SpectraID = DB_WriteSpectrum(&(measurement->Leg1_Spectrum), TRUE);
	}

	if((measurement->ProbeTUPatientMeasurementID == -1) && (measurement->Leg2_Spectrum.SpectraID == 0)){
		measurement->Leg2_SpectraID = DB_WriteSpectrum(&(measurement->Leg2_Spectrum), TRUE);
	}

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	if(measurement->ProbeTUPatientMeasurementID == -1){
		d_prepare_v2("INSERT INTO ProbeTUPatientMeasurement(MeasurementNumber,Neck1ROICounts,Neck1ROICPM,Neck2ROICounts,Neck2ROICPM,NeckROIAverageCPM,Leg1ROICounts,Leg1ROICPM,Leg2ROICounts,Leg2ROICPM,LegROIAverageCPM,PatientROINetCPM,AdjustedPatientROINetCPM,DoseROINetCPM,AdjustedDoseROINetCPM,Uptake,ElapsedTime,Comment,Neck1_SpectraID,Neck2_SpectraID,Leg1_SpectraID,Leg2_SpectraID,ProbeTUTestID,CreatedOn,Inactive) VALUES(:MeasurementNumber,:Neck1ROICounts,:Neck1ROICPM,:Neck2ROICounts,:Neck2ROICPM,:NeckROIAverageCPM,:Leg1ROICounts,:Leg1ROICPM,:Leg2ROICounts,:Leg2ROICPM,:LegROIAverageCPM,:PatientROINetCPM,:AdjustedPatientROINetCPM,:DoseROINetCPM,:AdjustedDoseROINetCPM,:Uptake,:ElapsedTime,:Comment,:Neck1_SpectraID,:Neck2_SpectraID,:Leg1_SpectraID,:Leg2_SpectraID,:ProbeTUTestID,julianday(:CreatedOn),:Inactive);");
		d_bind_int(":MeasurementNumber", measurement->MeasurementNumber);
		d_bind_int(":Neck1ROICounts", measurement->Neck1ROICounts);
		d_bind_double(":Neck1ROICPM", measurement->Neck1ROICPM);
		d_bind_int(":Neck2ROICounts", measurement->Neck2ROICounts);
		d_bind_double(":Neck2ROICPM", measurement->Neck2ROICPM);
		d_bind_double(":NeckROIAverageCPM", measurement->NeckROIAverageCPM);
		d_bind_int(":Leg1ROICounts", measurement->Leg1ROICounts);
	   	d_bind_double(":Leg1ROICPM", measurement->Leg1ROICPM);
		d_bind_int(":Leg2ROICounts", measurement->Leg2ROICounts);
		d_bind_double(":Leg2ROICPM", measurement->Leg2ROICPM);
		d_bind_double(":LegROIAverageCPM", measurement->LegROIAverageCPM);
		d_bind_double(":PatientROINetCPM", measurement->PatientROINetCPM);
		d_bind_double(":AdjustedPatientROINetCPM", measurement->AdjustedPatientROINetCPM);
		d_bind_double(":DoseROINetCPM", measurement->DoseROINetCPM);
		d_bind_double(":AdjustedDoseROINetCPM", measurement->AdjustedDoseROINetCPM);
		d_bind_double(":Uptake", measurement->Uptake);
		d_bind_double(":ElapsedTime", measurement->ElapsedTime);
		d_bind_text(":Comment", measurement->comment);
		d_bind_int64(":Neck1_SpectraID", measurement->Neck1_SpectraID);
		d_bind_int64(":Neck2_SpectraID", measurement->Neck2_SpectraID);
		d_bind_int64(":Leg1_SpectraID", measurement->Leg1_SpectraID);
		d_bind_int64(":Leg2_SpectraID", measurement->Leg2_SpectraID);
		d_bind_int64(":ProbeTUTestID", measurement->ProbeTUTestID);
		stringout(CreatedOn, &(measurement->CreatedOn));
		d_bind_text(":CreatedOn", CreatedOn);
		d_bind_int(":Inactive", 0);
		d_step();
		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
		else rowid = -1;
		d_finalize();
		measurement->ProbeTUPatientMeasurementID = rowid;
	}else{

	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_WriteProbeThyroidUptakeDoseMeasurement(PROBETHYROIDUPTAKEDOSETOTALMEAS *measurement, bool bookEnd){
	char TimeText[40];
	sqlite3_int64 rowid;
	int index;

	if(measurement->ProbeTUTotalDoseMeasurementID == -1){
		for(index=0; index<5; index++){
			if(measurement->SingleMeasurement[index].Dose1_Spectrum.SpectraID != -1){
				measurement->SingleMeasurement[index].Dose1_SpectraID = DB_WriteSpectrum(&(measurement->SingleMeasurement[index].Dose1_Spectrum), TRUE);
			}
			if(measurement->SingleMeasurement[index].Dose2_Spectrum.SpectraID != -1){
				measurement->SingleMeasurement[index].Dose2_SpectraID = DB_WriteSpectrum(&(measurement->SingleMeasurement[index].Dose2_Spectrum), TRUE);
			}
		}
	}

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	if(measurement->ProbeTUTotalDoseMeasurementID == -1){
		d_prepare_v2("INSERT INTO ProbeTUTotalDoseMeasurement(MeasurementNumber,BackgroundROICounts,BackgroundROICPM,DoseROICPM,DoseROINetCPM,StartDateTime,EndDateTime,WellBackgroundID,ProbeTUTestID,CreatedOn,Inactive) VALUES(:MeasurementNumber,:BackgroundROICounts,:BackgroundROICPM,:DoseROICPM,:DoseROINetCPM,julianday(:StartDateTime),julianday(:EndDateTime),:WellBackgroundID,:ProbeTUTestID,julianday(:CreatedOn),:Inactive);");
		d_bind_int(":MeasurementNumber", measurement->MeasurementNumber);
		d_bind_int(":BackgroundROICounts", measurement->BackgroundROICounts);
		d_bind_double(":BackgroundROICPM", measurement->BackgroundROICPM);
		d_bind_double(":DoseROICPM", measurement->DoseROICPM);
		d_bind_double(":DoseROINetCPM", measurement->DoseROINetCPM);
		stringout(TimeText, &(measurement->StartDateTime));
		d_bind_text(":StartDateTime", TimeText);
		stringout(TimeText, &(measurement->EndDateTime));
		d_bind_text(":EndDateTime", TimeText);
		d_bind_int64(":WellBackgroundID", measurement->WellBackgroundID);
		d_bind_int64(":ProbeTUTestID", measurement->ProbeTUTestID);
		stringout(TimeText, &(measurement->CreatedOn));
		d_bind_text(":CreatedOn", TimeText);
		d_bind_int(":Inactive", 0);
		d_step();
		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
		else rowid = -1;
		d_finalize();
		measurement->ProbeTUTotalDoseMeasurementID = rowid;

		if(measurement->ProbeTUTotalDoseMeasurementID != -1){
			for(index=0;index<5;index++){
				if((measurement->SingleMeasurement[index].Dose1_SpectraID != -1) || (measurement->SingleMeasurement[index].Dose2_SpectraID != -1)){
					measurement->SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = measurement->ProbeTUTotalDoseMeasurementID;
					d_prepare_v2("INSERT INTO ProbeTUSingleDoseMeasurement(DoseSubUnit,Dose1ROICounts,Dose1ROICPM,Dose2ROICounts,Dose2ROICPM,DoseROIAverageCPM,Dose1_SpectraID,Dose2_SpectraID,ProbeTUTotalDoseMeasurementID,CreatedOn) VALUES(:DoseSubUnit,:Dose1ROICounts,:Dose1ROICPM,:Dose2ROICounts,:Dose2ROICPM,:DoseROIAverageCPM,:Dose1_SpectraID,:Dose2_SpectraID,:ProbeTUTotalDoseMeasurementID,julianday(:CreatedOn));");
					d_bind_int(":DoseSubUnit", measurement->SingleMeasurement[index].DoseSubUnit);
					d_bind_int(":Dose1ROICounts", measurement->SingleMeasurement[index].Dose1ROICounts);
					d_bind_double(":Dose1ROICPM", measurement->SingleMeasurement[index].Dose1ROICPM);
					d_bind_int(":Dose2ROICounts", measurement->SingleMeasurement[index].Dose2ROICounts);
					d_bind_double(":Dose2ROICPM", measurement->SingleMeasurement[index].Dose2ROICPM);
					d_bind_double(":DoseROIAverageCPM", measurement->SingleMeasurement[index].DoseROIAverageCPM);
					d_bind_int64(":Dose1_SpectraID", measurement->SingleMeasurement[index].Dose1_SpectraID);
					d_bind_int64(":Dose2_SpectraID", measurement->SingleMeasurement[index].Dose2_SpectraID);
					d_bind_int64(":ProbeTUTotalDoseMeasurementID", measurement->SingleMeasurement[index].ProbeTUTotalDoseMeasurementID);
					stringout(TimeText, &(measurement->SingleMeasurement[index].CreatedOn));
					d_bind_text(":CreatedOn", TimeText);
					d_step();
					if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
					if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
					else rowid = -1;
					d_finalize();
					measurement->SingleMeasurement[index].ProbeTUSingleDoseMeasurementID = rowid;
				}
			}
		}
	}else{
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_RetrieveProbeThyroidUptakeDoseMeasurement(PROBETHYROIDUPTAKEDOSETOTALMEAS *measurement){
	*capStatus = SQLITE_OK;

	if(measurement->ProbeTUTotalDoseMeasurementID == -1){
		d_prepare_v2("SELECT ProbeTUTotalDoseMeasurementID,MeasurementNumber,BackgroundROICounts,BackgroundROICPM,DoseROICPM,DoseROINetCPM,datetime(StartDateTime),datetime(EndDateTime),WellBackgroundID,ProbeTUTestID,datetime(CreatedOn),Inactive FROM ProbeTUTotalDoseMeasurement WHERE Inactive = 0 AND ProbeTUTestID = :ProbeTUTestID AND MeasurementNumber = :MeasurementNumber;");
		d_bind_int64(":ProbeTUTestID", measurement->ProbeTUTestID);
		d_bind_int(":MeasurementNumber", measurement->MeasurementNumber);
	}else{
		d_prepare_v2("SELECT ProbeTUTotalDoseMeasurementID,MeasurementNumber,BackgroundROICounts,BackgroundROICPM,DoseROICPM,DoseROINetCPM,datetime(StartDateTime),datetime(EndDateTime),WellBackgroundID,ProbeTUTestID,datetime(CreatedOn),Inactive FROM ProbeTUTotalDoseMeasurement WHERE ProbeTUTotalDoseMeasurementID = :ProbeTUTotalDoseMeasurementID;");
		d_bind_int64(":ProbeTUTotalDoseMeasurementID", measurement->ProbeTUTotalDoseMeasurementID);
	}
	d_step();
	if(*capStatus == SQLITE_ROW){
		measurement->ProbeTUTotalDoseMeasurementID = d_column_int64(0);				//ProbeTUTotalDoseMeasurementID,
		measurement->MeasurementNumber = d_column_int(1);							//MeasurementNumber,
		measurement->BackgroundROICounts = d_column_int(2);							//BackgroundROICounts,
		measurement->BackgroundROICPM = d_column_double(3);							//BackgroundROICPM,
		measurement->DoseROICPM = d_column_double(4);								//DoseROICPM,
		measurement->DoseROINetCPM = d_column_double(5);							//DoseROINetCPM,
		measurement->StartDateTime = strtotime_t((const char *) d_column_text(6));	//StartDateTime,
		measurement->EndDateTime = strtotime_t((const char *) d_column_text(7));	//EndDateTime,
		measurement->WellBackgroundID = d_column_int64(8);							//WellBackgroundID,
		measurement->ProbeTUTestID = d_column_int64(9);								//ProbeTUTestID,
		measurement->CreatedOn = strtotime_t((const char *) d_column_text(10));		//CreatedOn,
		if(d_column_int(11) == 0) measurement->Inactive = FALSE;					//Inactive
		else measurement->Inactive = FALSE;											//Inactive
	}else{
		measurement->ProbeTUTotalDoseMeasurementID = -1;
	}
	d_finalize();
}

void DB_RetrieveProbeThyroidUptakePatientMeasurement(PROBETHYROIDUPTAKEPATMEAS *measurement){
	*capStatus = SQLITE_OK;

	if(measurement->ProbeTUPatientMeasurementID == -1){
		d_prepare_v2("SELECT ProbeTUPatientMeasurementID,MeasurementNumber,Neck1ROICounts,Neck1ROICPM,Neck2ROICounts,Neck2ROICPM,NeckROIAverageCPM,Leg1ROICounts,Leg1ROICPM,Leg2ROICounts,Leg2ROICPM,LegROIAverageCPM,PatientROINetCPM,AdjustedPatientROINetCPM,DoseROINetCPM,AdjustedDoseROINetCPM,Uptake,ElapsedTime,Comment,Neck1_SpectraID,Neck2_SpectraID,Leg1_SpectraID,Leg2_SpectraID,ProbeTUTestID,datetime(CreatedOn),Inactive FROM ProbeTUPatientMeasurement WHERE Inactive = 0 AND ProbeTUTestID = :ProbeTUTestID AND MeasurementNumber = :MeasurementNumber;");
		d_bind_int64(":ProbeTUTestID", measurement->ProbeTUTestID);
		d_bind_int(":MeasurementNumber", measurement->MeasurementNumber);
	}else{
		d_prepare_v2("SELECT ProbeTUPatientMeasurementID,MeasurementNumber,Neck1ROICounts,Neck1ROICPM,Neck2ROICounts,Neck2ROICPM,NeckROIAverageCPM,Leg1ROICounts,Leg1ROICPM,Leg2ROICounts,Leg2ROICPM,LegROIAverageCPM,PatientROINetCPM,AdjustedPatientROINetCPM,DoseROINetCPM,AdjustedDoseROINetCPM,Uptake,ElapsedTime,Comment,Neck1_SpectraID,Neck2_SpectraID,Leg1_SpectraID,Leg2_SpectraID,ProbeTUTestID,datetime(CreatedOn),Inactive FROM ProbeTUPatientMeasurement WHERE ProbeTUPatientMeasurementID = :ProbeTUPatientMeasurementID;");
		d_bind_int64(":ProbeTUPatientMeasurementID", measurement->ProbeTUPatientMeasurementID);
	}
	d_step();
	if(*capStatus == SQLITE_ROW){
		measurement->ProbeTUPatientMeasurementID = d_column_int64(0);	//ProbeTUPatientMeasurementID,
		measurement->MeasurementNumber = d_column_int(1);				//MeasurementNumber,
		measurement->Neck1ROICounts = d_column_int(2);					//Neck1ROICounts,
		measurement->Neck1ROICPM = d_column_double(3);					//Neck1ROICPM,
		measurement->Neck2ROICounts = d_column_int(4);					//Neck2ROICounts,
		measurement->Neck2ROICPM = d_column_double(5);					//Neck2ROICPM,
		measurement->NeckROIAverageCPM = d_column_double(6);			//NeckROIAverageCPM,
		measurement->Leg1ROICounts = d_column_int(7);					//Leg1ROICounts,
		measurement->Leg1ROICPM = d_column_double(8);					//Leg1ROICPM,
		measurement->Leg2ROICounts = d_column_int(9);					//Leg2ROICounts,
		measurement->Leg2ROICPM = d_column_double(10);					//Leg2ROICPM,
		measurement->LegROIAverageCPM = d_column_double(11);			//LegROIAverageCPM,
		measurement->PatientROINetCPM = d_column_double(12);			//PatientROINetCPM,
		measurement->AdjustedPatientROINetCPM = d_column_double(13);	//AdjustedPatientROINetCPM,
		measurement->DoseROINetCPM = d_column_double(14);				//DoseROINetCPM,
		measurement->AdjustedDoseROINetCPM = d_column_double(15);		//AdjustedDoseROINetCPM,
		measurement->Uptake = d_column_double(16);						//Uptake,
		measurement->ElapsedTime = d_column_double(17);					//ElapsedTime,
		strcpy(measurement->comment, (const char *) d_column_text(18));	//Comment,
		measurement->Neck1_SpectraID = d_column_int64(19);				//Neck1_SpectraID,
		measurement->Neck2_SpectraID = d_column_int64(20);				//Neck2_SpectraID,
		measurement->Leg1_SpectraID = d_column_int64(21);				//Leg1_SpectraID,
		measurement->Leg2_SpectraID = d_column_int64(22);				//Leg2_SpectraID,
		measurement->ProbeTUTestID = d_column_int64(23);				//ProbeTUTestID,
		measurement->CreatedOn = strtotime_t((const char *) d_column_text(24));	//CreatedOn,
		if(d_column_int(25) == 0) measurement->Inactive = FALSE;		//Inactive
		else  measurement->Inactive = TRUE;								//Inactive
	}else{
		measurement->ProbeTUPatientMeasurementID = -1;
	}
	d_finalize();
}

void DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(PROBETHYROIDUPTAKEDOSESINGLEMEAS *single){
	*capStatus = SQLITE_OK;

	d_prepare_v2("SELECT ProbeTUSingleDoseMeasurementID,DoseSubUnit,Dose1ROICounts,Dose1ROICPM,Dose2ROICounts,Dose2ROICPM,DoseROIAverageCPM,Dose1_SpectraID,Dose2_SpectraID,ProbeTUTotalDoseMeasurementID,datetime(CreatedOn) FROM ProbeTUSingleDoseMeasurement WHERE ProbeTUTotalDoseMeasurementID=:ProbeTUTotalDoseMeasurementID AND DoseSubUnit=:DoseSubUnit;");
	d_bind_int64(":ProbeTUTotalDoseMeasurementID", single->ProbeTUTotalDoseMeasurementID);
	d_bind_int(":DoseSubUnit", single->DoseSubUnit);
	d_step();
	if(*capStatus == SQLITE_ROW){
		single->ProbeTUSingleDoseMeasurementID = d_column_int64(0);			//ProbeTUSingleDoseMeasurementID
		single->DoseSubUnit = d_column_int(1);								//DoseSubUnit
		single->Dose1ROICounts = d_column_int(2);							//Dose1ROICounts
		single->Dose1ROICPM = d_column_double(3);							//Dose1ROICPM
		single->Dose2ROICounts = d_column_int(4);							//Dose2ROICounts
		single->Dose2ROICPM = d_column_double(5);							//Dose2ROICPM
		single->DoseROIAverageCPM = d_column_double(6);						//DoseROIAverageCPM
		single->Dose1_SpectraID = d_column_int64(7);						//Dose1_SpectraID
		single->Dose2_SpectraID = d_column_int64(8);						//Dose2_SpectraID
		single->ProbeTUTotalDoseMeasurementID = d_column_int64(9);			//ProbeTUTotalDoseMeasurementID
		single->CreatedOn = strtotime_t((const char *) d_column_text(10));	//CreatedOn
	}else{
		single->ProbeTUSingleDoseMeasurementID = -1;
	}
	d_finalize();
}

void DB_RetrieveTUNormal(PROBETHYROIDUPTAKENORMAL normal[4]){
	*capStatus = SQLITE_OK;

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUHour1';")){
		if(*capStatus == SQLITE_ROW){
			normal[0].hour = d_column_int(0);
		}else{
			normal[0].hour = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[0].hour = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour1',:TUHour1);");
		d_bind_int(":TUHour1", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUHour2';")){
		if(*capStatus == SQLITE_ROW){
			normal[1].hour = d_column_int(0);
		}else{
			normal[1].hour = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[1].hour = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour2',:TUHour2);");
		d_bind_int(":TUHour2", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUHour3';")){
		if(*capStatus == SQLITE_ROW){
			normal[2].hour = d_column_int(0);
		}else{
			normal[2].hour = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[2].hour = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour3',:TUHour3);");
		d_bind_int(":TUHour3", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUHour4';")){
		if(*capStatus == SQLITE_ROW){
			normal[3].hour = d_column_int(0);
		}else{
			normal[3].hour = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[3].hour = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUHour4',:TUHour4);");
		d_bind_int(":TUHour4", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUMinHour1';")){
		if(*capStatus == SQLITE_ROW){
			normal[0].min = d_column_int(0);
		}else{
			normal[0].min = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[0].min = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour1',:TUMinHour1);");
		d_bind_int(":TUMinHour1", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUMinHour2';")){
		if(*capStatus == SQLITE_ROW){
			normal[1].min = d_column_int(0);
		}else{
			normal[1].min = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[1].min = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour2',:TUMinHour2);");
		d_bind_int(":TUMinHour2", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUMinHour3';")){
		if(*capStatus == SQLITE_ROW){
			normal[2].min = d_column_int(0);
		}else{
			normal[2].min = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[2].min = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour3',:TUMinHour3);");
		d_bind_int(":TUMinHour3", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUMinHour4';")){
		if(*capStatus == SQLITE_ROW){
			normal[3].min = d_column_int(0);
		}else{
			normal[3].min = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[3].min = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMinHour4',:TUMinHour4);");
		d_bind_int(":TUMinHour4", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUMaxHour1';")){
		if(*capStatus == SQLITE_ROW){
			normal[0].max = d_column_int(0);
		}else{
			normal[0].max = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[0].max = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour1',:TUMaxHour1);");
		d_bind_int(":TUMaxHour1", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUMaxHour2';")){
		if(*capStatus == SQLITE_ROW){
			normal[1].max = d_column_int(0);
		}else{
			normal[1].max = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[1].max = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour2',:TUMaxHour2);");
		d_bind_int(":TUMaxHour2", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUMaxHour3';")){
		if(*capStatus == SQLITE_ROW){
			normal[2].max = d_column_int(0);
		}else{
			normal[2].max = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[2].max = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour3',:TUMaxHour3);");
		d_bind_int(":TUMaxHour3", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TUMaxHour4';")){
		if(*capStatus == SQLITE_ROW){
			normal[3].max = d_column_int(0);
		}else{
			normal[3].max = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		normal[3].max = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('TUMaxHour4',:TUMaxHour4);");
		d_bind_int(":TUMaxHour4", -1);
		d_step();
		d_finalize();
	}
}

void DB_WriteTUNormal(PROBETHYROIDUPTAKENORMAL normal[4]){
	*capStatus = SQLITE_OK;

	d_command("BEGIN;");

	d_prepare_v2("UPDATE Config SET SettingValue = :TUHour1 WHERE SettingName='TUHour1';");
	d_bind_int(":TUHour1", normal[0].hour);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUHour2 WHERE SettingName='TUHour2';");
	d_bind_int(":TUHour2", normal[1].hour);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUHour3 WHERE SettingName='TUHour3';");
	d_bind_int(":TUHour3", normal[2].hour);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUHour4 WHERE SettingName='TUHour4';");
	d_bind_int(":TUHour4", normal[3].hour);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUMinHour1 WHERE SettingName='TUMinHour1';");
	d_bind_int(":TUMinHour1", normal[0].min);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUMinHour2 WHERE SettingName='TUMinHour2';");
	d_bind_int(":TUMinHour2", normal[1].min);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUMinHour3 WHERE SettingName='TUMinHour3';");
	d_bind_int(":TUMinHour3", normal[2].min);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUMinHour4 WHERE SettingName='TUMinHour4';");
	d_bind_int(":TUMinHour4", normal[3].min);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUMaxHour1 WHERE SettingName='TUMaxHour1';");
	d_bind_int(":TUMaxHour1", normal[0].max);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUMaxHour2 WHERE SettingName='TUMaxHour2';");
	d_bind_int(":TUMaxHour2", normal[1].max);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUMaxHour3 WHERE SettingName='TUMaxHour3';");
	d_bind_int(":TUMaxHour3", normal[2].max);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :TUMaxHour4 WHERE SettingName='TUMaxHour4';");
	d_bind_int(":TUMaxHour4", normal[3].max);
	d_step();
	d_finalize();

	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}

int DB_SearchRBCSurvivalTests(WELLRBCSURVIVALSEARCH *wellRBCSurvival, char *lastName, char *patientID, bool active){
	int retrievedCount;
	WELLRBCSURVIVALSEARCH *ptr;

	retrievedCount = 0;
	ptr = wellRBCSurvival;

	*capStatus = SQLITE_OK;
	if(active){
		if((strlen(lastName) == 0) && (strlen(patientID) == 0)){
			d_prepare_v2("SELECT WellRBCSurvivalTestID, PatientID, FirstName, LastName, InjectionDate, datetime(InjectionDate), Inactive, WellRBCSurvivalTestPhase FROM WellRBCSurvivalTest WHERE Inactive = 0 ORDER BY InjectionDate DESC, CreatedOn DESC LIMIT 100;");
		}else if((strlen(lastName) != 0) && (strlen(patientID) == 0)){
			d_prepare_v2("SELECT WellRBCSurvivalTestID, PatientID, FirstName, LastName, InjectionDate, datetime(InjectionDate), Inactive, WellRBCSurvivalTestPhase FROM WellRBCSurvivalTest WHERE Inactive = 0 AND LastName LIKE :LastName ORDER BY InjectionDate DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":LastName", lastName);
		}else if((strlen(lastName) == 0) && (strlen(patientID) != 0)){
			d_prepare_v2("SELECT WellRBCSurvivalTestID, PatientID, FirstName, LastName, InjectionDate, datetime(InjectionDate), Inactive, WellRBCSurvivalTestPhase FROM WellRBCSurvivalTest WHERE Inactive = 0 AND PatientID LIKE :PatientID ORDER BY InjectionDate DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":PatientID", patientID);
		}else{
			d_prepare_v2("SELECT WellRBCSurvivalTestID, PatientID, FirstName, LastName, InjectionDate, datetime(InjectionDate), Inactive, WellRBCSurvivalTestPhase FROM WellRBCSurvivalTest WHERE Inactive = 0 AND LastName LIKE :LastName AND PatientID LIKE :PatientID ORDER BY InjectionDate DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":LastName", lastName);
			d_bind_text(":PatientID", patientID);
		}
	}else{
		if((strlen(lastName) == 0) && (strlen(patientID) == 0)){
			d_prepare_v2("SELECT WellRBCSurvivalTestID, PatientID, FirstName, LastName, InjectionDate, datetime(InjectionDate), Inactive, WellRBCSurvivalTestPhase FROM WellRBCSurvivalTest WHERE Inactive != 0 ORDER BY InjectionDate DESC, CreatedOn DESC LIMIT 100;");
		}else if((strlen(lastName) != 0) && (strlen(patientID) == 0)){
			d_prepare_v2("SELECT WellRBCSurvivalTestID, PatientID, FirstName, LastName, InjectionDate, datetime(InjectionDate), Inactive, WellRBCSurvivalTestPhase FROM WellRBCSurvivalTest WHERE Inactive != 0 AND LastName LIKE :LastName ORDER BY InjectionDate DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":LastName", lastName);
		}else if((strlen(lastName) == 0) && (strlen(patientID) != 0)){
			d_prepare_v2("SELECT WellRBCSurvivalTestID, PatientID, FirstName, LastName, InjectionDate, datetime(InjectionDate), Inactive, WellRBCSurvivalTestPhase FROM WellRBCSurvivalTest WHERE Inactive != 0 AND PatientID LIKE :PatientID ORDER BY InjectionDate DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":PatientID", patientID);
		}else{
			d_prepare_v2("SELECT WellRBCSurvivalTestID, PatientID, FirstName, LastName, InjectionDate, datetime(InjectionDate), Inactive, WellRBCSurvivalTestPhase FROM WellRBCSurvivalTest WHERE Inactive != 0 AND LastName LIKE :LastName AND PatientID LIKE :PatientID ORDER BY InjectionDate DESC, CreatedOn DESC LIMIT 100;");
			d_bind_text(":LastName", lastName);
			d_bind_text(":PatientID", patientID);
		}
	}
	d_step();

	if(*capStatus == SQLITE_ROW){
		while(*capStatus == SQLITE_ROW){
			ptr->WellRBCSurvivalTestID = d_column_int64(0); //WellRBCSurvivalTestID
			strcpy(ptr->PatientID, (const char *) d_column_text(1)); //PatientID
			strcpy(ptr->FirstName, (const char *) d_column_text(2)); //FirstName
			strcpy(ptr->LastName, (const char *) d_column_text(3)); //LastName
			if(d_column_double(4) >= 1e+9){ //InjectionDate
				ptr->InjectedOn = (time_t) 0;
			}else{
				ptr->InjectedOn = strtotime_t((const char *) d_column_text(5)); //datetime(InjectionDate)
			}
			if(d_column_int(6) == 0) ptr->Inactive = FALSE;
			else ptr->Inactive = TRUE;
			ptr->WellRBCSurvivalTestPhase = d_column_int(7); //WellRBCSurvivalTestPhase
			retrievedCount++;
			ptr++;
			d_step();
		}
	}
	d_finalize();

	return retrievedCount;
}

void DB_RetrieveWellRBCSurvivalTest(WELLRBCSURVIVALTEST *test){
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT WellRBCSurvivalTestID,WellRBCSurvivalTestPhase,PatientID,FirstName,LastName,DateOfBirth,Sex,Physician,TechID,NuclideName,HalfLife,HalfLifeUnit,StartROI,EndROI,PeakEV,CountingTime,InjectionDate,datetime(InjectionDate),CalibrationActivity,CalibrationDate,datetime(CalibrationDate),LotNum,AccessionNumber,datetime(CreatedOn),datetime(LastUpdated),InactiveReason,Inactive FROM WellRBCSurvivalTest WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
	d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
	d_step();

	if(*capStatus == SQLITE_ROW){
		test->WellRBCSurvivalTestID = d_column_int64(0);							//WellRBCSurvivalTestID
		test->WellRBCSurvivalTestPhase = d_column_int(1);							//WellRBCSurvivalTestPhase
		strcpy(test->PatientID, (const char *) d_column_text(2));					//PatientID
		strcpy(test->FirstName, (const char *) d_column_text(3));					//FirstName
		strcpy(test->LastName, (const char *) d_column_text(4));					//LastName
		test->DateOfBirth = d_column_double(5);										//DateOfBirth
		strcpy(test->Sex, (const char *) d_column_text(6));							//Sex
		strcpy(test->Physician, (const char *) d_column_text(7));					//Physician
		strcpy(test->TechID, (const char *) d_column_text(8));						//TechID
		strcpy(test->NuclideName, (const char *) d_column_text(9));					//NuclideName
		test->HalfLife = d_column_double(10);										//HalfLife
		test->HalfLifeUnit = d_column_int(11);										//HalfLifeUnit
		test->StartROI = d_column_double(12);										//StartROI
		test->EndROI = d_column_double(13);											//EndROI
		test->PeakEV = d_column_double(14);											//PeakEV
		test->CountingTime = d_column_int(15);										//CountingTime
		if(d_column_double(16) >= 1e+9){											//InjectionDate
			test->InjectionDate = (time_t) 0;
		}else{
			test->InjectionDate = strtotime_t((const char *) d_column_text(17));	//datetime(InjectionDate)
		}
		test->CalibrationActivity = d_column_double(18);							//CalibrationActivity
		if(d_column_double(19) == 0){												//CalibrationDate
			test->CalibrationDate = (time_t) 0;
		}else{
			test->CalibrationDate = strtotime_t((const char *) d_column_text(20));	//datetime(CalibrationDate)
		}
		strcpy(test->LotNum, (const char *) d_column_text(21));						//LotNum
		strcpy(test->AccessionNumber, (const char *) d_column_text(22));			//AccessionNumber
		test->CreatedOn = strtotime_t((const char *) d_column_text(23));			//datetime(CreatedOn)
		test->LastUpdated = strtotime_t((const char *) d_column_text(24));			//datetime(LastUpdated)
		strcpy(test->InactiveReason, (const char *) d_column_text(25));				//InactiveReason
		if(d_column_int(26) == 0) test->Inactive = FALSE;							//Inactive
		else test->Inactive = TRUE;
	}else{
		test->WellRBCSurvivalTestID = 0;
	}
	d_finalize();
}

void DB_WriteRBCSurvivalTest(WELLRBCSURVIVALTEST *test, bool bookEnd){
	sqlite3_int64 rowid;
	char datetimestr[30];
	bool flgChanged;
	WELLRBCSURVIVALTEST *original;

	original = malloc(sizeof(WELLRBCSURVIVALTEST));
	if(test->WellRBCSurvivalTestID != -1){
		original->WellRBCSurvivalTestID = test->WellRBCSurvivalTestID;
		DB_RetrieveWellRBCSurvivalTest(original);
	}

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	if(test->WellRBCSurvivalTestID == -1){
		d_prepare_v2("INSERT INTO WellRBCSurvivalTest(WellRBCSurvivalTestPhase,PatientID,FirstName,LastName,DateOfBirth,Sex,Physician,TechID,NuclideName,HalfLife,HalfLifeUnit,StartROI,EndROI,PeakEV,CountingTime,InjectionDate,CalibrationActivity,CalibrationDate,LotNum,AccessionNumber,InactiveReason,Inactive) VALUES(:WellRBCSurvivalTestPhase,:PatientID,:FirstName,:LastName,:DateOfBirth,:Sex,:Physician,:TechID,:NuclideName,:HalfLife,:HalfLifeUnit,:StartROI,:EndROI,:PeakEV,:CountingTime,:InjectionDate,:CalibrationActivity,:CalibrationDate,:LotNum,:AccessionNumber,:InactiveReason,:Inactive);");
		d_bind_int(":WellRBCSurvivalTestPhase", test->WellRBCSurvivalTestPhase);
		d_bind_text(":PatientID", test->PatientID);
		d_bind_text(":FirstName", test->FirstName);
		d_bind_text(":LastName", test->LastName);
		d_bind_double(":DateOfBirth", test->DateOfBirth);
		d_bind_text(":Sex", test->Sex);
		d_bind_text(":Physician", test->Physician);
		d_bind_text(":TechID", test->TechID);
		d_bind_text(":NuclideName", test->NuclideName);
		d_bind_double(":HalfLife", test->HalfLife);
		d_bind_int(":HalfLifeUnit", test->HalfLifeUnit);
		d_bind_double(":StartROI", test->StartROI);
		d_bind_double(":EndROI", test->EndROI);
		d_bind_double(":PeakEV", test->PeakEV);
		d_bind_int(":CountingTime", test->CountingTime);

		if(test->InjectionDate == (time_t)0) d_bind_double(":InjectionDate", 1e+9);
		else d_bind_double(":InjectionDate", time_ttojd(test->InjectionDate));


		d_bind_double(":CalibrationActivity", test->CalibrationActivity);

		if(test->CalibrationDate == (time_t)0) d_bind_double(":CalibrationDate", 0);
		else d_bind_double(":CalibrationDate", time_ttojd(test->CalibrationDate));

		d_bind_text(":LotNum", test->LotNum);
		d_bind_text(":AccessionNumber", test->AccessionNumber);
		d_bind_text(":InactiveReason", test->InactiveReason);
		d_bind_int(":Inactive", 0);
		d_step();
		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
		else rowid = -1;
		d_finalize();
		test->WellRBCSurvivalTestID = rowid;
	}else{
		flgChanged = FALSE;

		if(original->WellRBCSurvivalTestPhase != test->WellRBCSurvivalTestPhase){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET WellRBCSurvivalTestPhase = :WellRBCSurvivalTestPhase WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_int(":WellRBCSurvivalTestPhase", test->WellRBCSurvivalTestPhase);
			d_step();
			d_finalize();
		}

		if(strcmp(original->PatientID, test->PatientID) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET PatientID = :PatientID WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_text(":PatientID", test->PatientID);
			d_step();
			d_finalize();
		}

		if(strcmp(original->FirstName, test->FirstName)){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET FirstName = :FirstName WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_text(":FirstName", test->FirstName);
			d_step();
			d_finalize();
		}

		if(strcmp(original->LastName, test->LastName) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET LastName = :LastName WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_text(":LastName", test->LastName);
			d_step();
			d_finalize();
		}

		if(original->DateOfBirth != test->DateOfBirth){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET DateOfBirth = :DateOfBirth WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_double(":DateOfBirth", test->DateOfBirth);
			d_step();
			d_finalize();
		}

		if(strcmp(original->Sex, test->Sex) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET Sex = :Sex WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_text(":Sex", test->Sex);
			d_step();
			d_finalize();
		}

		if(strcmp(original->Physician, test->Physician) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET Physician = :Physician WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_text(":Physician", test->Physician);
			d_step();
			d_finalize();
		}

		if(strcmp(original->TechID, test->TechID) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET TechID = :TechID WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_text(":TechID", test->TechID);
			d_step();
			d_finalize();
		}

		if(strcmp(original->NuclideName, test->NuclideName) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET NuclideName = :NuclideName WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_text(":NuclideName", test->NuclideName);
			d_step();
			d_finalize();
		}

		if(original->HalfLife != test->HalfLife){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET HalfLife = :HalfLife WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_double(":HalfLife", test->HalfLife);
			d_step();
			d_finalize();
		}

		if(original->HalfLifeUnit != test->HalfLifeUnit){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET HalfLifeUnit = :HalfLifeUnit WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_int(":HalfLifeUnit", test->HalfLifeUnit);
			d_step();
			d_finalize();
		}

		if(original->StartROI != test->StartROI){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET StartROI = :StartROI WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_double(":StartROI", test->StartROI);
			d_step();
			d_finalize();
		}

		if(original->EndROI != test->EndROI){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET EndROI = :EndROI WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_double(":EndROI", test->EndROI);
			d_step();
			d_finalize();
		}

		if(original->PeakEV != test->PeakEV){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET PeakEV = :PeakEV WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_double(":PeakEV", test->PeakEV);
			d_step();
			d_finalize();
		}

		if(original->CountingTime != test->CountingTime){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET CountingTime = :CountingTime WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_int(":CountingTime", test->CountingTime);
			d_step();
			d_finalize();
		}

		if(original->InjectionDate != test->InjectionDate){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET InjectionDate = :InjectionDate WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			if(test->InjectionDate == (time_t)0) d_bind_double(":InjectionDate", 1e+9);
			else d_bind_double(":InjectionDate", time_ttojd(test->InjectionDate));
			d_step();
			d_finalize();
		}

		if(original->CalibrationActivity != test->CalibrationActivity){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET CalibrationActivity = :CalibrationActivity WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_double(":CalibrationActivity", test->CalibrationActivity);
			d_step();
			d_finalize();
		}

		if(original->CalibrationDate != test->CalibrationDate){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET CalibrationDate = :CalibrationDate WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			if(test->CalibrationDate == 0) d_bind_double(":CalibrationDate", 0);
			else d_bind_double(":CalibrationDate", time_ttojd(test->CalibrationDate));
			d_step();
			d_finalize();
		}

		if(strcmp(original->LotNum, test->LotNum) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET LotNum = :LotNum WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_text(":LotNum", test->LotNum);
			d_step();
			d_finalize();
		}

		if(strcmp(original->AccessionNumber, test->AccessionNumber) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET AccessionNumber = :AccessionNumber WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_text(":AccessionNumber", test->AccessionNumber);
			d_step();
			d_finalize();
		}

		if(strcmp(original->InactiveReason, test->InactiveReason) != 0){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET InactiveReason = :InactiveReason WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			d_bind_text(":InactiveReason", test->InactiveReason);
			d_step();
			d_finalize();
		}

		if(original->Inactive != test->Inactive){
			flgChanged = TRUE;
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET Inactive = :Inactive WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			if(test->Inactive) d_bind_int(":Inactive", 1);
			else d_bind_int(":Inactive", 0);
			d_step();
			d_finalize();
		}

		if(flgChanged){
			d_prepare_v2("UPDATE WellRBCSurvivalTest SET LastUpdated = julianday(:LastUpdated) WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
			d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
			stringout(datetimestr, &clock_time);
			d_bind_text(":LastUpdated", datetimestr);
			d_step();
			d_finalize();
		}
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
	free(original);
}

void DB_DeleteRBCSurvivalTest(WELLRBCSURVIVALTEST *test){
	*capStatus = SQLITE_OK;
	d_prepare_v2("DELETE FROM WellRBCSurvivalTest WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
	d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
	d_step();
	d_finalize();
}

void DB_InactivateWellRBCSurvivalTest(WELLRBCSURVIVALTEST *test){
	char datetimestr[30];

	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE WellRBCSurvivalTest SET LastUpdated = julianday(:LastUpdated), InactiveReason = :InactiveReason, Inactive = 1 WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
	stringout(datetimestr, &clock_time);
	d_bind_text(":LastUpdated", datetimestr);
	d_bind_text(":InactiveReason", test->InactiveReason);
	d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
	d_step();
	d_finalize();
}

void DB_ReactivateWellRBCSurvivalTest(WELLRBCSURVIVALTEST *test){
	char datetimestr[30];

	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE WellRBCSurvivalTest SET LastUpdated = julianday(:LastUpdated), Inactive = 0 WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
	stringout(datetimestr, &clock_time);
	d_bind_text(":LastUpdated", datetimestr);
	d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
	d_step();
	d_finalize();
}

int DB_RBCSurvivalMeasurementCount(WELLRBCSURVIVALTEST *test){
	int returnvalue;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT Count(*) FROM WellRBCSurvivalMeasurement WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID;");
	d_bind_int64(":WellRBCSurvivalTestID", test->WellRBCSurvivalTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		returnvalue = d_column_int(0);
	}else{
		returnvalue = 0;
	}
	d_finalize();

	return returnvalue;
}

void DB_RetrieveWellRBCSurvivalMeasurement(WELLRBCSURVIVALMEAS *measurement){
	//WellRBCSurvivalMeasurementID,MeasurementNumber,BackgroundROICounts,BackgroundROICPM,Sample1ROICounts,Sample1ROICPM,Sample2ROICounts,Sample2ROICPM,SampleROIAverageCPM,SampleROINetCPM,DecayCorrectedSampleROINetCPM,SampleHematocrit,Remaining,ElapsedDays,Comment,WellBackgroundID,Sample1_SpectraID,Sample2_SpectraID,WellRBCSurvivalTestID,CreatedOn
	if(measurement->WellRBCSurvivalMeasurementID == -1){
		d_prepare_v2("SELECT WellRBCSurvivalMeasurementID,MeasurementNumber,BackgroundROICounts,BackgroundROICPM,Sample1ROICounts,Sample1ROICPM,Sample2ROICounts,Sample2ROICPM,SampleROIAverageCPM,SampleROINetCPM,DecayCorrectedSampleROINetCPM,SampleHematocrit,Remaining,ElapsedDays,Comment,WellBackgroundID,Sample1_SpectraID,Sample2_SpectraID,WellRBCSurvivalTestID,datetime(CreatedOn) FROM WellRBCSurvivalMeasurement WHERE WellRBCSurvivalTestID = :WellRBCSurvivalTestID AND MeasurementNumber = :MeasurementNumber;");
		d_bind_int64(":WellRBCSurvivalTestID", measurement->WellRBCSurvivalTestID);
		d_bind_int(":MeasurementNumber", measurement->MeasurementNumber);
	}else{
		d_prepare_v2("SELECT WellRBCSurvivalMeasurementID,MeasurementNumber,BackgroundROICounts,BackgroundROICPM,Sample1ROICounts,Sample1ROICPM,Sample2ROICounts,Sample2ROICPM,SampleROIAverageCPM,SampleROINetCPM,DecayCorrectedSampleROINetCPM,SampleHematocrit,Remaining,ElapsedDays,Comment,WellBackgroundID,Sample1_SpectraID,Sample2_SpectraID,WellRBCSurvivalTestID,datetime(CreatedOn) FROM WellRBCSurvivalMeasurement WHERE WellRBCSurvivalMeasurementID = :WellRBCSurvivalMeasurementID;");
		d_bind_int64(":WellRBCSurvivalMeasurementID", measurement->WellRBCSurvivalMeasurementID);
	}
	d_step();

	if(*capStatus == SQLITE_ROW){
		measurement->WellRBCSurvivalMeasurementID = d_column_int64(0);			//WellRBCSurvivalMeasurementID,
		measurement->MeasurementNumber = d_column_int(1);						//MeasurementNumber,
		measurement->BackgroundROICounts = d_column_int(2);						//BackgroundROICounts,
		measurement->BackgroundROICPM = d_column_double(3);						//BackgroundROICPM,
		measurement->Sample1ROICounts = d_column_int(4);						//Sample1ROICounts,
		measurement->Sample1ROICPM = d_column_double(5);						//Sample1ROICPM,
		measurement->Sample2ROICounts = d_column_int(6);						//Sample2ROICounts,
		measurement->Sample2ROICPM = d_column_double(7);						//Sample2ROICPM,
		measurement->SampleROIAverageCPM = d_column_double(8);					//SampleROIAverageCPM,
		measurement->SampleROINetCPM = d_column_double(9);						//SampleROINetCPM,
		measurement->DecayCorrectedSampleROINetCPM = d_column_double(10);		//DecayCorrectedSampleROINetCPM,
		measurement->SampleHematocrit = d_column_double(11);					//SampleHematocrit,
		measurement->Remaining = d_column_double(12);							//Remaining,
		measurement->ElapsedDays = d_column_double(13);							//ElapsedDays,
	    strcpy(measurement->comment, (const char *) d_column_text(14));						//Comment,
		measurement->WellBackgroundID = d_column_int64(15);						//WellBackgroundID,
		measurement->Sample1_SpectraID = d_column_int64(16);					//Sample1_SpectraID,
		measurement->Sample2_SpectraID = d_column_int64(17);					//Sample2_SpectraID,
		measurement->WellRBCSurvivalTestID = d_column_int64(18);				//WellRBCSurvivalTestID,
		measurement->CreatedOn = strtotime_t((const char *) d_column_text(19));	//datetime(CreatedOn)
	}else{
		measurement->WellRBCSurvivalMeasurementID = -1;
	}
	d_finalize();
}

void DB_WriteRBCSurvivalMeasurement(WELLRBCSURVIVALMEAS *measurement, bool bookEnd){
	sqlite3_int64 rowid;
	char CreatedOn[40];

	if((measurement->WellRBCSurvivalMeasurementID == -1) && (measurement->Sample1_Spectrum.SpectraID == 0)){
		measurement->Sample1_SpectraID = DB_WriteSpectrum(&(measurement->Sample1_Spectrum), TRUE);
	}

	if((measurement->WellRBCSurvivalMeasurementID == -1) && (measurement->Sample2_Spectrum.SpectraID == 0)){
		measurement->Sample2_SpectraID = DB_WriteSpectrum(&(measurement->Sample2_Spectrum), TRUE);
	}

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}


//MeasurementNumber,BackgroundROICounts,BackgroundROICPM,Sample1ROICounts,Sample1ROICPM,Sample2ROICounts,Sample2ROICPM,SampleROIAverageCPM,SampleROINetCPM,DecayCorrectedSampleROINetCPM,SampleHematocrit,Remaining,ElapsedDays,Comment,WellBackgroundID,Sample1_SpectraID,Sample2_SpectraID,WellRBCSurvivalTestID,CreatedOn

	if(measurement->WellRBCSurvivalMeasurementID == -1){
		d_prepare_v2("INSERT INTO WellRBCSurvivalMeasurement(MeasurementNumber,BackgroundROICounts,BackgroundROICPM,Sample1ROICounts,Sample1ROICPM,Sample2ROICounts,Sample2ROICPM,SampleROIAverageCPM,SampleROINetCPM,DecayCorrectedSampleROINetCPM,SampleHematocrit,Remaining,ElapsedDays,Comment,WellBackgroundID,Sample1_SpectraID,Sample2_SpectraID,WellRBCSurvivalTestID,CreatedOn) VALUES(:MeasurementNumber,:BackgroundROICounts,:BackgroundROICPM,:Sample1ROICounts,:Sample1ROICPM,:Sample2ROICounts,:Sample2ROICPM,:SampleROIAverageCPM,:SampleROINetCPM,:DecayCorrectedSampleROINetCPM,:SampleHematocrit,:Remaining,:ElapsedDays,:Comment,:WellBackgroundID,:Sample1_SpectraID,:Sample2_SpectraID,:WellRBCSurvivalTestID,julianday(:CreatedOn));");
		d_bind_int(":MeasurementNumber", measurement->MeasurementNumber);
		d_bind_int(":BackgroundROICounts", measurement->BackgroundROICounts);
		d_bind_double(":BackgroundROICPM", measurement->BackgroundROICPM);
		d_bind_int(":Sample1ROICounts", measurement->Sample1ROICounts);
		d_bind_double(":Sample1ROICPM", measurement->Sample1ROICPM);
		d_bind_int(":Sample2ROICounts", measurement->Sample2ROICounts);
		d_bind_double(":Sample2ROICPM", measurement->Sample2ROICPM);
		d_bind_double(":SampleROIAverageCPM", measurement->SampleROIAverageCPM);
		d_bind_double(":SampleROINetCPM", measurement->SampleROINetCPM);
		d_bind_double(":DecayCorrectedSampleROINetCPM", measurement->DecayCorrectedSampleROINetCPM);
		d_bind_double(":SampleHematocrit", measurement->SampleHematocrit);
		d_bind_double(":Remaining", measurement->Remaining);
		d_bind_double(":ElapsedDays", measurement->ElapsedDays);
		d_bind_text(":Comment", measurement->comment);
		d_bind_int64(":WellBackgroundID", measurement->WellBackgroundID);
		d_bind_int64(":Sample1_SpectraID", measurement->Sample1_SpectraID);
		d_bind_int64(":Sample2_SpectraID", measurement->Sample2_SpectraID);
		d_bind_int64(":WellRBCSurvivalTestID", measurement->WellRBCSurvivalTestID);
		stringout(CreatedOn, &(measurement->CreatedOn));
		d_bind_text(":CreatedOn", CreatedOn);
		d_step();
		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
		else rowid = -1;
		d_finalize();
		measurement->WellRBCSurvivalMeasurementID = rowid;
	}else{
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_RetrieveWellRBCSurvivalResult(WELLRBCSURVIVALRESULT *result){
	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT WellRBCSurvivalResultID,HalfDays,Y0,X10,Y60,WellRBCSurvivalTestID,datetime(CreatedOn) FROM WellRBCSurvivalResult WHERE WellRBCSurvivalTestID=:WellRBCSurvivalTestID;");
	d_bind_int64(":WellRBCSurvivalTestID", result->WellRBCSurvivalTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		result->WellRBCSurvivalResultID = d_column_int64(0);				//WellRBCSurvivalResultID,
		result->HalfDays = d_column_double(1);								//HalfDays,
		result->Y0 = d_column_double(2);									//Y0,
		result->X10 = d_column_double(3);									//X10,
		result->Y60 = d_column_double(4);									//Y60,
		result->WellRBCSurvivalTestID = d_column_int64(5);					//WellRBCSurvivalTestID,
		result->CreatedOn = strtotime_t((const char *) d_column_text(6));	//datetime(CreatedOn)
	}else{
		result->WellRBCSurvivalResultID = -1;
	}
	d_finalize();
}

void DB_WriteRBCSurvivalResult(WELLRBCSURVIVALRESULT *result, bool bookEnd){
	WELLRBCSURVIVALRESULT original;
	sqlite3_int64 rowid;

	original.WellRBCSurvivalTestID = result->WellRBCSurvivalTestID;
	DB_RetrieveWellRBCSurvivalResult(&original);

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	if(original.WellRBCSurvivalResultID == -1){
		d_prepare_v2("INSERT INTO WellRBCSurvivalResult(HalfDays,Y0,X10,Y60,WellRBCSurvivalTestID) VALUES(:HalfDays,:Y0,:X10,:Y60,:WellRBCSurvivalTestID);");
		d_bind_double(":HalfDays", result->HalfDays);
		d_bind_double(":Y0", result->Y0);
		d_bind_double(":X10", result->X10);
		d_bind_double(":Y60", result->Y60);
		d_bind_int64(":WellRBCSurvivalTestID", result->WellRBCSurvivalTestID);
		d_step();
		if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
		if(*capStatus == SQLITE_OK) rowid = sqlite3_last_insert_rowid(capDB);
		else rowid = -1;
		d_finalize();
		result->WellRBCSurvivalResultID = rowid;
	}else{
		result->WellRBCSurvivalResultID = original.WellRBCSurvivalResultID;

		if(original.HalfDays != result->HalfDays){
			d_prepare_v2("UPDATE WellRBCSurvivalResult SET HalfDays = :HalfDays WHERE WellRBCSurvivalResultID = :WellRBCSurvivalResultID;");
			d_bind_int64(":WellRBCSurvivalResultID", original.WellRBCSurvivalResultID);
			d_bind_double(":HalfDays", result->HalfDays);
			d_step();
			d_finalize();
		}

		if(original.Y0 != result->Y0){
			d_prepare_v2("UPDATE WellRBCSurvivalResult SET Y0 = :Y0 WHERE WellRBCSurvivalResultID = :WellRBCSurvivalResultID;");
			d_bind_int64(":WellRBCSurvivalResultID", original.WellRBCSurvivalResultID);
			d_bind_double(":Y0", result->Y0);
			d_step();
			d_finalize();
		}

		if(original.X10 != result->X10){
			d_prepare_v2("UPDATE WellRBCSurvivalResult SET X10 = :X10 WHERE WellRBCSurvivalResultID = :WellRBCSurvivalResultID;");
			d_bind_int64(":WellRBCSurvivalResultID", original.WellRBCSurvivalResultID);
			d_bind_double(":X10", result->X10);
			d_step();
			d_finalize();
		}

		if(original.Y60 != result->Y60){
			d_prepare_v2("UPDATE WellRBCSurvivalResult SET Y60 = :Y60 WHERE WellRBCSurvivalResultID = :WellRBCSurvivalResultID;");
			d_bind_int64(":WellRBCSurvivalResultID", original.WellRBCSurvivalResultID);
			d_bind_double(":Y60", result->Y60);
			d_step();
			d_finalize();
		}
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}

void DB_RetrieveRBCSurvivalNormal(float *minNormal, float *maxNormal){
	*capStatus = SQLITE_OK;

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'RBCSurvivalMinNormal';")){
		if(*capStatus == SQLITE_ROW){
			*minNormal = d_column_double(0);
		}else{
			*minNormal = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		*minNormal = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('RBCSurvivalMinNormal',:RBCSurvivalMinNormal);");
		d_bind_double(":RBCSurvivalMinNormal", -1);
		d_step();
		d_finalize();
	}

	if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'RBCSurvivalMaxNormal';")){
		if(*capStatus == SQLITE_ROW){
			*maxNormal = d_column_double(0);
		}else{
			*maxNormal = -1;
		}
		d_finalize();
	}else{
		d_finalize();
		*maxNormal = -1;
		d_prepare_v2("INSERT INTO Config(SettingName,SettingValue) VALUES('RBCSurvivalMaxNormal',:RBCSurvivalMaxNormal);");
		d_bind_double(":RBCSurvivalMaxNormal", -1);
		d_step();
		d_finalize();
	}
}

void DB_WriteRBCSurvivalNormal(float minNormal, float maxNormal){
	*capStatus = SQLITE_OK;

	d_command("BEGIN;");

	d_prepare_v2("UPDATE Config SET SettingValue = :RBCSurvivalMinNormal WHERE SettingName='RBCSurvivalMinNormal';");
	d_bind_int(":RBCSurvivalMinNormal", minNormal);
	d_step();
	d_finalize();

	d_prepare_v2("UPDATE Config SET SettingValue = :RBCSurvivalMaxNormal WHERE SettingName='RBCSurvivalMaxNormal';");
	d_bind_int(":RBCSurvivalMaxNormal", maxNormal);
	d_step();
	d_finalize();

	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}
/**
 * \details Read time format from Config table
 * \returns 0 = mm/dd/yyyy, 1 = dd/mm/yyyy, 2 = yyyy/mm/dd
 */
int DB_ReadTimeFormat(void){
	int returnvalue;

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'TimeFormat';")){
			if(*capStatus == SQLITE_ROW){
					returnvalue = d_column_int(0);
				}else{
					returnvalue = 0;
				}
				d_finalize();
		}else{
			d_finalize();
			returnvalue = 0;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('TimeFormat',0);");
		}
	}

	return returnvalue;
}
/**
 * \details Save time format to Config table
 * \param TimeFormat Time format, 0 = mm/dd/yyyy, 1 = dd/mm/yyyy, 2 = yyyy/mm/dd
 * \returns None
 */
void DB_WriteTimeFormat(int TimeFormat){
	char query[200];

	strcpy(query, "UPDATE Config SET SettingValue = :TimeFormat WHERE SettingName = 'TimeFormat';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":TimeFormat", TimeFormat);
	d_step();
	d_finalize();
}
/**
 * \details Read USB device protocol
 * \returns 0 = HID protocol, 1 = CDC protocol
 */
int DB_ReadUSBDeviceProtocol(void){
	int returnvalue;

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'USBDeviceProtocol';")){
			if(*capStatus == SQLITE_ROW){
					returnvalue = d_column_int(0);
				}else{
					returnvalue = 0;
				}
				d_finalize();
		}else{
			d_finalize();
			returnvalue = 0;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('USBDeviceProtocol',0);");
		}
	}

	return returnvalue;
}
/**
 * \details Write USB device protocol
 * \param Protocol USB device protocol, 0 = HID protocol, 1 = CDC protocol
 * \returns None
 */
void DB_WriteUSBDeviceProtocol(int Protocol){
	char query[200];

	strcpy(query, "UPDATE Config SET SettingValue = :Protocol WHERE SettingName = 'USBDeviceProtocol';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":Protocol", Protocol);
	d_step();
	d_finalize();
}
/**
 * \details Save Auto Linearity Test in AutoLinearityTest and AutoLinearityMeasurement tables
 * \param test Pointer to AUTOLINEARITYTEST structure with information to be written to the database
 * \param bookEnd True = Execute with rollback, False = Execute without rollback
 * \returns None
 */
void DB_CreateAutoLinearityTest(AUTOLINEARITYTEST *test, bool bookEnd){
	int index;
	char datetimestr[30];

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO AutoLinearityTest(SerialNumber,Type,NuclideName,HalfLife,HalfLifeUnit,IntervalMinutes,TotalMinutes,StartedOn,Comment,InactiveReason,Inactive) VALUES(:SerialNumber,:Type,:NuclideName,:HalfLife,:HalfLifeUnit,:IntervalMinutes,:TotalMinutes,julianday(:StartedOn),:Comment,:InactiveReason,:Inactive);");
	d_bind_text(":SerialNumber", test->SerialNumber);
	d_bind_int(":Type", test->Type);
	d_bind_text(":NuclideName", test->NuclideName);
	d_bind_double(":HalfLife", test->HalfLife);
	d_bind_int(":HalfLifeUnit", test->HalfLifeUnit);
	d_bind_int(":IntervalMinutes", test->IntervalMinutes);
	d_bind_int(":TotalMinutes", test->TotalMinutes);
	stringout(datetimestr, &(test->StartedOn));
	d_bind_text(":StartedOn", datetimestr);
	d_bind_text(":Comment", test->Comment);
	d_bind_text(":InactiveReason", "");
	d_bind_int(":Inactive", 0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) test->AutoLinearityTestID = sqlite3_last_insert_rowid(capDB);
	else test->AutoLinearityTestID = 0;
	d_finalize();

	if(test->AutoLinearityTestID > 0){
		for(index=0; index<1210; index++){
			if(test->AutoLinearityMeasurement[index].AutoLinearityMeasurementID == -3){	// Measurement
				d_prepare_v2("INSERT INTO AutoLinearityMeasurement(AutoLinearityTestID,MeasuredActivity,MeasuredOn,Status,ElapsedMinutes,Inactive) VALUES(:AutoLinearityTestID,:MeasuredActivity,julianday(:MeasuredOn),:Status,:ElapsedMinutes,:Inactive);");
				d_bind_int64(":AutoLinearityTestID", test->AutoLinearityTestID);
				d_bind_double(":MeasuredActivity", test->AutoLinearityMeasurement[index].MeasuredActivity);
				stringout(datetimestr, &(test->AutoLinearityMeasurement[index].MeasuredOn));
				d_bind_text(":MeasuredOn", datetimestr);
				d_bind_int(":Status", test->AutoLinearityMeasurement[index].Status);
				d_bind_int(":ElapsedMinutes", test->AutoLinearityMeasurement[index].ElapsedMinutes);
				d_bind_int(":Inactive", 0);
				d_step();
				if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
				//if(*capStatus == SQLITE_OK) test->AutoLinearityMeasurement[index].AutoLinearityMeasurementID = sqlite3_last_insert_rowid(capDB);
				d_finalize();
			}
		}
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}
/**
 * \details Retrieve Auto Linearity Tests from a date range
 * \param tests Pointer to array of AUTOLINEARITYSEARCH structures, which will receive the Auto Linearity Test
 * \param startDate Start of Date Range
 * \param endDate End of Date Range
 * \param maxRetrieved Maximum number of Auto Linearity Tests to retrieve
 * \returns Number of Auto Linearity Tests retrieved
 */
int DB_SearchAutoLinearityTests(AUTOLINEARITYSEARCH *tests, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount, index;
	AUTOLINEARITYSEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = tests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->AutoLinearityTestID  = 0;
		testPtr++;
	}
	testPtr = tests;

	retrievedCount = 0;
	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT AutoLinearityTestID, SerialNumber, NuclideName, datetime(StartedOn), Inactive FROM AutoLinearityTest WHERE StartedOn >= julianday(:startDate) AND StartedOn < julianday(:endDate) ORDER BY StartedOn DESC;");
	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();

	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->AutoLinearityTestID = d_column_int64(0);
				strcpy(testPtr->SerialNumber, (const char *) d_column_text(1));
				strcpy(testPtr->NuclideName, (const char *) d_column_text(2));
				testPtr->StartedOn = strtotime_t((const char *) d_column_text(3));
				if(d_column_int(4) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;
				testPtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}
/**
 * \details Retrieve Auto Linearity Test details
 * \param test Pointer to AUTOLINEARITYTEST structure, which will receive the data from the database. test->AutoLinearityTestID provides the routine with the ID in the query
 * \returns None
 */
void DB_RetrieveAutoLinearityTest(AUTOLINEARITYTEST *test){
	int index;

	test->CreatedOn = 0;
	for(index=0; index<1210; index++){
		test->AutoLinearityMeasurement[index].AutoLinearityMeasurementID = -1;
		test->AutoLinearityMeasurement[index].MeasuredActivity = 0;
		test->AutoLinearityMeasurement[index].MeasuredOn = 0;
		test->AutoLinearityMeasurement[index].Status = 0;
		test->AutoLinearityMeasurement[index].ElapsedMinutes = 0;
		test->AutoLinearityMeasurement[index].PredictedActivity = 0;
		test->AutoLinearityMeasurement[index].PercentVariation = 0;
	}

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT SerialNumber,NuclideName,HalfLife,HalfLifeUnit,IntervalMinutes,TotalMinutes,datetime(StartedOn),Comment,datetime(CreatedOn),InactiveReason,Inactive FROM AutoLinearityTest WHERE AutoLinearityTestID = :AutoLinearityTestID;");
	d_bind_int64(":AutoLinearityTestID", test->AutoLinearityTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		strcpy(test->SerialNumber, (const char *) d_column_text(0));
		strcpy(test->NuclideName, (const char *) d_column_text(1));
		test->HalfLife = d_column_double(2);
		test->HalfLifeUnit = d_column_int(3);
		test->IntervalMinutes = d_column_int(4);
		test->TotalMinutes = d_column_int(5);
		test->StartedOn = strtotime_t((const char *) d_column_text(6));
		strcpy(test->Comment, (const char *) d_column_text(7));
		test->CreatedOn = strtotime_t((const char *) d_column_text(8));
		strcpy(test->InactiveReason, (const char *) d_column_text(9));
		if(d_column_int(10) == 0) test->Inactive = FALSE;
		else test->Inactive = TRUE;
	}
	d_finalize();

	d_prepare_v2("SELECT AutoLinearityMeasurementID,MeasuredActivity,datetime(MeasuredOn),Status,ElapsedMinutes FROM AutoLinearityMeasurement WHERE AutoLinearityTestID = :AutoLinearityTestID ORDER BY MeasuredOn;");
	d_bind_int64(":AutoLinearityTestID", test->AutoLinearityTestID);
	d_step();
	if(*capStatus == SQLITE_ROW){
		index = 0;
		while(*capStatus == SQLITE_ROW){
			if(index < 1210){
				test->AutoLinearityMeasurement[index].AutoLinearityMeasurementID = d_column_int64(0);
				test->AutoLinearityMeasurement[index].MeasuredActivity = d_column_double(1);
				test->AutoLinearityMeasurement[index].MeasuredOn = strtotime_t((const char *) d_column_text(2));
				test->AutoLinearityMeasurement[index].Status = d_column_int(3);
				test->AutoLinearityMeasurement[index].ElapsedMinutes = d_column_int(4);
				index++;
			}
			d_step();
		}
	}
	d_finalize();
}
/**
 * \details Inactivate Auto Linearity Test
 * \param test Pointer to AUTOLINEARITYTEST structure, test->AutoLinearityTestID provides the routine with the ID in the query and test->InactiveReason provides the routine with the Inactivate Reason
 * \returns None
 */
void DB_InactivateAutoLinearityTest(AUTOLINEARITYTEST *test){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE AutoLinearityTest SET InactiveReason = :InactiveReason, Inactive = 1 WHERE AutoLinearityTestID = :AutoLinearityTestID;");
	d_bind_text(":InactiveReason", test->InactiveReason);
	d_bind_int64(":AutoLinearityTestID", test->AutoLinearityTestID);
	d_step();
	d_finalize();
}
/**
 * \details Read the Future Date Input format from the Config table
 * \returns 0 = Quick Input, 1 = Full Input
 */
int DB_ReadFutureDateInput(void){
	int returnvalue;

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'FutureDateInput';")){
			if(*capStatus == SQLITE_ROW){
					returnvalue = d_column_int(0);
				}else{
					returnvalue = 0;
				}
				d_finalize();
		}else{
			d_finalize();
			returnvalue = 0;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('FutureDateInput',0);");
		}
	}

	return returnvalue;
}
/**
 * \details Write the Future Date Input format to the Config table
 * \param FutureDateInput Future Date Input format, 0 = Quick Input, 1 = Full Input
 * \returns None
 */
void DB_WriteFutureDateInput(int FutureDateInput){
	char query[200];

	strcpy(query, "UPDATE Config SET SettingValue = :FutureDateInput WHERE SettingName = 'FutureDateInput';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":FutureDateInput", FutureDateInput);
	d_step();
	d_finalize();
}
/**
 * \details Read Language ID from the Config table
 * \returns 0 = English, 1 = French
 */
int DB_ReadLanguage(void){
	int returnvalue;

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Language';")){
			if(*capStatus == SQLITE_ROW){
					returnvalue = d_column_int(0);
				}else{
					returnvalue = 0;
				}
				d_finalize();
		}else{
			d_finalize();
			returnvalue = 0;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Language',0);");
		}
	}

	return returnvalue;
}
/**
 * \details Write Language ID to the Config table
 * \param Language Language ID, 0 = English, 1 = French
 * \returns None
 */
void DB_WriteLanguage(int Language){
	char query[200];

	strcpy(query, "UPDATE Config SET SettingValue = :Language WHERE SettingName = 'Language';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":Language", Language);
	d_step();
	d_finalize();
}
/**
 * \details Read Feed Label status from the Config table
 * \returns 0 = No, Other = Yes
 */
int DB_ReadFeedLabel(void){
	int returnvalue;

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'FeedLabel';")){
			if(*capStatus == SQLITE_ROW){
				returnvalue = d_column_int(0);
			}else{
				returnvalue = 1;
			}
			d_finalize();
		}else{
			d_finalize();
			returnvalue = 1;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('FeedLabel',1);");
		}
	}else{
		returnvalue = 1;
	}

	return returnvalue;
}
/**
 * \details Write Feed Label setting to the Config table
 * \param FeedLabel Feed label setting, 0 = No feed label, Other = Feed label
 * \returns None
 */
void DB_WriteFeedLabel(int FeedLabel){
	char query[200];

	strcpy(query, "UPDATE Config SET SettingValue = :FeedLabel WHERE SettingName = 'FeedLabel';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":FeedLabel", FeedLabel);
	d_step();
	d_finalize();
}
/**
 * \details Read Moly Streamlined status from the Config table
 * \returns 0 = No, Other = Yes
 */
int DB_ReadMolyStreamlined(void){
	int returnvalue;

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'MolyStreamlined';")){
			if(*capStatus == SQLITE_ROW){
				returnvalue = d_column_int(0);
			}else{
				returnvalue = 1;
			}
			d_finalize();
		}else{
			d_finalize();
			returnvalue = 1;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('MolyStreamlined',1);");
		}
	}else{
		returnvalue = 1;
	}

	return returnvalue;
}
/**
 * \details Write Moly Streamlined setting to the Config table
 * \param MolyStreamlined Moly Streamlined setting, 0 = No, Other = Yes
 * \returns None
 */
void DB_WriteMolyStreamlined(int MolyStreamlined){
	char query[200];

	strcpy(query, "UPDATE Config SET SettingValue = :MolyStreamlined WHERE SettingName = 'MolyStreamlined';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":MolyStreamlined", MolyStreamlined);
	d_step();
	d_finalize();
}
/**
 * \details Read Key from the Config table
 * \returns 0 = Empty
 */
long long int DB_ReadKey(void){
	long long int returnvalue;
	int key0, key1;
	unsigned int *ptr;

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Key0';")){
			if(*capStatus == SQLITE_ROW){
				key0 = d_column_int(0);
			}else{
				key0 = 0;
			}
			d_finalize();
		}else{
			d_finalize();
			key0 = 0;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Key0',0);");
		}

		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Key1';")){
			if(*capStatus == SQLITE_ROW){
				key1 = d_column_int(0);
			}else{
				key1 = 0;
			}
			d_finalize();
		}else{
			d_finalize();
			key1 = 0;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Key1',0);");
		}

		ptr = (unsigned int *) &key1;
		returnvalue = *ptr;
		returnvalue <<= 32;
		ptr = (unsigned int *) &key0;
		returnvalue += (*ptr);
	}else{
		returnvalue = 0;
	}

	return returnvalue;
}
/**
 * \details Write Key to the Config table
 * \param Key Key, 0 = Empty
 * \returns None
 */
void DB_WriteKey(long long int Key){
	char query[200];
	int key0, key1;
	unsigned int *ptr;

	ptr = (unsigned int *) &key0;
	*ptr = Key & 0xFFFFFFFF;

	Key >>= 32;
	ptr = (unsigned int *) &key1;
	*ptr = Key & 0xFFFFFFFF;

	strcpy(query, "UPDATE Config SET SettingValue = :Key0 WHERE SettingName = 'Key0';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":Key0", key0);
	d_step();
	d_finalize();

	strcpy(query, "UPDATE Config SET SettingValue = :Key1 WHERE SettingName = 'Key1';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":Key1", key1);
	d_step();
	d_finalize();
}
/**
 * \details Read Password from the Config table
 * \returns None
 */
void DB_ReadPassword(char *password){
	int i, number, length;
	char sn[6];
	char *ptr;
	char pwd[12];

	if(capDB != NULL){
		*capStatus = SQLITE_OK;
		if(d_query("SELECT SettingValue FROM Config WHERE SettingName = 'Password';")){
			if(*capStatus == SQLITE_ROW){
				number = d_column_int(0);
			}else{
				number = 0;
			}
			d_finalize();
		}else{
			d_finalize();
			number = 0;
			d_command("INSERT INTO Config(SettingName,SettingValue) VALUES('Password',0);");
		}
	}else{
		number = 0;
	}

	length = number & 0xF;
	number >>= 4;

	if(!length){
		EE_READ(snum, (uchar *) sn);
		ptr = password;
		*ptr++ = sn[3];
		*ptr++ = sn[4];
		*ptr++ = sn[5];
		*ptr = 0;
	}else{
		*password = 0;
		sprintf(pwd, "%ld", number);
		length -= strlen(pwd);
		if(length > 0){
			ptr = password;
			for(i=0; i<length; i++){
				*ptr++ = '0';
			}
			*ptr = 0;
		}
		strcat(password, pwd);
	}
}
/**
 * \details Write Password to the Config table
 * \param password Password, 0 = Last 3 digits of serial number
 * \returns None
 */
void DB_WritePassword(char *password){
	char query[200];
	int number, length;
	unsigned int *ptr;

	length = strlen(password);
	if(length > 0){
		if(length > 8) length = 0; // Error: length can not be longer than 8
		number = atoi(password);
		number <<= 4;
		number += length;
	}else{
		number = 0;
	}

	strcpy(query, "UPDATE Config SET SettingValue = :Password WHERE SettingName = 'Password';");
	*capStatus = SQLITE_OK;
	d_prepare_v2(query);
	d_bind_int(":Password", number);
	d_step();
	d_finalize();
}
/**
 * \details Save Daily Test into the ChambeDailyTest table
 * \param chamberdailytest Pointer to CHAMBERDAILYTEST structure, which holds the Daily Test info
 * \param bookEnd True = Execute with rollback, False = Execute without rollback
 * \returns None
 */
static void DB_CreateChamberDailyTest(CHAMBERDAILYTEST *chamberdailytest, bool bookEnd){
	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO ChamberDailyTest(ChamberSerialNumber, ChamberType, TwoStageChamber, DataCheckTextEnglish, DataCheckTextFrench, DataCheckFailed, DataCheckCRC, ChamberZeroID, ChamberBackgroundID, ChamberVoltageID, ChamberAccuracyTestID, InactiveReason, Inactive) VALUES(:ChamberSerialNumber, :ChamberType, :TwoStageChamber, :DataCheckTextEnglish, :DataCheckTextFrench, :DataCheckFailed, :DataCheckCRC, :ChamberZeroID, :ChamberBackgroundID, :ChamberVoltageID, :ChamberAccuracyTestID, :InactiveReason, :Inactive);");
	d_bind_text(":ChamberSerialNumber", chamberdailytest->ChamberSerialNumber);
	d_bind_int(":ChamberType", chamberdailytest->ChamberType);
	d_bind_int(":TwoStageChamber", chamberdailytest->TwoStageChamber);
	d_bind_text(":DataCheckTextEnglish", chamberdailytest->DataCheckTextEnglish);
	d_bind_text(":DataCheckTextFrench", chamberdailytest->DataCheckTextFrench);
	d_bind_int(":DataCheckFailed", chamberdailytest->DataCheckFailed);
	d_bind_text(":DataCheckCRC", chamberdailytest->DataCheckCRC);
	d_bind_int64(":ChamberZeroID", chamberdailytest->ChamberZeroID);
	d_bind_int64(":ChamberBackgroundID", chamberdailytest->ChamberBackgroundID);
	d_bind_int64(":ChamberVoltageID", chamberdailytest->ChamberVoltageID);
	d_bind_int64(":ChamberAccuracyTestID", chamberdailytest->ChamberAccuracyTestID);
	d_bind_text(":InactiveReason", "");
	d_bind_int(":Inactive", 0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) chamberdailytest->ChamberDailyTestID = sqlite3_last_insert_rowid(capDB);
	else chamberdailytest->ChamberDailyTestID = 0;
	d_finalize();

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}
/**
 * \details Update the record in the ChamberDailyTest table, with the foreign key ChamberBackgroundID
 * \param ChamberDailyTestID Primary ID from the ChamberDailyTest table
 * \param ChamberBackgroundID Foreign Key to the ChamberBackground table
 * \returns None
 */
static void DB_UpdateDailyTestBackgroundID(long long int ChamberDailyTestID, long long int ChamberBackgroundID){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ChamberDailyTest SET ChamberBackgroundID = :ChamberBackgroundID WHERE ChamberDailyTestID = :ChamberDailyTestID;");
	d_bind_int64(":ChamberDailyTestID", ChamberDailyTestID);
	d_bind_int64(":ChamberBackgroundID", ChamberBackgroundID);
	d_step();
	d_finalize();
}
/**
 * \details Update the record in the ChamberDailyTest table, with the foreign key ChamberVoltageID
 * \param ChamberDailyTestID Primary ID from the ChamberDailyTest table
 * \param ChamberVoltageID Foreign Key to the ChamberVoltage table
 * \returns None
 */
static void DB_UpdateDailyTestVoltageID(long long int ChamberDailyTestID, long long int ChamberVoltageID){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ChamberDailyTest SET ChamberVoltageID = :ChamberVoltageID WHERE ChamberDailyTestID = :ChamberDailyTestID;");
	d_bind_int64(":ChamberDailyTestID", ChamberDailyTestID);
	d_bind_int64(":ChamberVoltageID", ChamberVoltageID);
	d_step();
	d_finalize();
}
/**
 * \details Update the record in the ChamberDailyTest table, with the foreign key ChamberAccuracyTestID
 * \param ChamberDailyTestID Primary ID from the ChamberDailyTest table
 * \param ChamberAccuracyTestID Foreign Key to the ChamberAccuracyTest table
 * \returns None
 */
static void DB_UpdateDailyTestAccuracyTestID(long long int ChamberDailyTestID, long long int ChamberAccuracyTestID){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ChamberDailyTest SET ChamberAccuracyTestID = :ChamberAccuracyTestID WHERE ChamberDailyTestID = :ChamberDailyTestID;");
	d_bind_int64(":ChamberDailyTestID", ChamberDailyTestID);
	d_bind_int64(":ChamberAccuracyTestID", ChamberAccuracyTestID);
	d_step();
	d_finalize();
}
/**
 * \details Save Zero Test into the ChamberZero table and optionally Save Daily Test into the ChamberDailyTest table
 * \param chamberzero Pointer to CHAMBERZERO structure, which holds the Zero Test info
 * \param ChamberDailyTestID Pointer to variable, which will receive the Primary Key from the ChamberDailyTest table. The ChamberDailyTest record is only created, if this pointer is not NULL
 * \param bookEnd True = Execute with rollback, False = Execute without rollback
 * \returns None
 */
void DB_CreateChamberZero(CHAMBERZERO *chamberzero, long long int *ChamberDailyTestID, bool bookEnd){
	char MeasuredOn[40];
	CHAMBERDAILYTEST *chamberdailytest;

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO ChamberZero(ChamberSerialNumber, ChamberType, TwoStageChamber, ZeroStatus, ZeroTextEnglish, ZeroTextFrench, ZeroValue, MeasuredOn, InactiveReason, Inactive) VALUES(:ChamberSerialNumber, :ChamberType, :TwoStageChamber, :ZeroStatus, :ZeroTextEnglish, :ZeroTextFrench, :ZeroValue, julianday(:MeasuredOn), :InactiveReason, :Inactive);");
	d_bind_text(":ChamberSerialNumber", chamberzero->ChamberSerialNumber);
	d_bind_int(":ChamberType", chamberzero->ChamberType);
	d_bind_int(":TwoStageChamber", chamberzero->TwoStageChamber);
	d_bind_int(":ZeroStatus", chamberzero->ZeroStatus);
	d_bind_text(":ZeroTextEnglish", chamberzero->ZeroTextEnglish);
	d_bind_text(":ZeroTextFrench", chamberzero->ZeroTextFrench);
	d_bind_double(":ZeroValue", chamberzero->ZeroValue);
	stringout(MeasuredOn, &(chamberzero->MeasuredOn));
	d_bind_text(":MeasuredOn", MeasuredOn);
	d_bind_text(":InactiveReason", "");
	d_bind_int(":Inactive", 0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) chamberzero->ChamberZeroID = sqlite3_last_insert_rowid(capDB);
	else chamberzero->ChamberZeroID = 0;
	d_finalize();

	if(ChamberDailyTestID != NULL){
		if(chamberzero->ChamberZeroID > 0){
			chamberdailytest = (CHAMBERDAILYTEST *) malloc(sizeof(CHAMBERDAILYTEST));
			chamberdailytest->ChamberDailyTestID = 0;
			strcpy(chamberdailytest->ChamberSerialNumber, chamberzero->ChamberSerialNumber);
			chamberdailytest->ChamberType = chamberzero->ChamberType;
			chamberdailytest->TwoStageChamber = chamberzero->TwoStageChamber;
			chamberdailytest->DataCheckTextEnglish[0] = 0;
			chamberdailytest->DataCheckTextFrench[0] = 0;
			chamberdailytest->DataCheckFailed = FALSE;
			chamberdailytest->DataCheckCRC[0] = 0;
			chamberdailytest->ChamberZeroID = chamberzero->ChamberZeroID;
			chamberdailytest->ChamberBackgroundID = 0;
			chamberdailytest->ChamberVoltageID = 0;
			chamberdailytest->ChamberAccuracyTestID = 0;
			chamberdailytest->CreatedOn = 0;
			chamberdailytest->InactiveReason[0] = 0;
			chamberdailytest->Inactive = FALSE;
			DB_CreateChamberDailyTest(chamberdailytest, FALSE);
			if(chamberdailytest->ChamberDailyTestID > 0) *ChamberDailyTestID = chamberdailytest->ChamberDailyTestID;
			free(chamberdailytest);
		}
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}
/**
 * \details Save Chamber Background Test into the ChamberBackground table
 * \param chamberbackground Pointer to CHAMBERBACKGROUND structure, which holds the Background Test info
 * \param ChamberDailyTestID Primary Key to the record in the ChambeDailyTest table. The ChamberBackgroundID foreign key will be updated with the primary key from the ChamberBackground table
 * \param bookEnd True = Execute with rollback, False = Execute without rollback
 * \returns None
 */
void DB_CreateChamberBackground(CHAMBERBACKGROUND *chamberbackground, long long int ChamberDailyTestID, bool bookEnd){
	char MeasuredOn[40];

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO ChamberBackground(ChamberSerialNumber, ChamberType, TwoStageChamber, BackgroundStatus, BackgroundTextEnglish, BackgroundTextFrench, BackgroundValue, MeasuredOn, InactiveReason, Inactive) VALUES(:ChamberSerialNumber, :ChamberType, :TwoStageChamber, :BackgroundStatus, :BackgroundTextEnglish, :BackgroundTextFrench, :BackgroundValue, julianday(:MeasuredOn), :InactiveReason, :Inactive);");
	d_bind_text(":ChamberSerialNumber", chamberbackground->ChamberSerialNumber);
	d_bind_int(":ChamberType", chamberbackground->ChamberType);
	d_bind_int(":TwoStageChamber", chamberbackground->TwoStageChamber);
	d_bind_int(":BackgroundStatus", chamberbackground->BackgroundStatus);
	d_bind_text(":BackgroundTextEnglish", chamberbackground->BackgroundTextEnglish);
	d_bind_text(":BackgroundTextFrench", chamberbackground->BackgroundTextFrench);
	d_bind_double(":BackgroundValue", chamberbackground->BackgroundValue);
	stringout(MeasuredOn, &(chamberbackground->MeasuredOn));
	d_bind_text(":MeasuredOn", MeasuredOn);
	d_bind_text(":InactiveReason", "");
	d_bind_int(":Inactive", 0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) chamberbackground->ChamberBackgroundID = sqlite3_last_insert_rowid(capDB);
	else chamberbackground->ChamberBackgroundID = 0;
	d_finalize();

	if(ChamberDailyTestID > 0){
		if(chamberbackground->ChamberBackgroundID > 0){
			DB_UpdateDailyTestBackgroundID(ChamberDailyTestID, chamberbackground->ChamberBackgroundID);
		}
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}
/**
 * \details Save Chamber Voltage Test into the ChamberVoltage table
 * \param chambervoltage Pointer to CHAMBERVOLTAGE structure, which holds the Chamber Voltage Test info
 * \param ChamberDailyTestID Primary Key to the record in the ChambeDailyTest table. The ChamberVoltageID foreign key will be updated with the primary key from the ChamberBackground table
 * \param bookEnd True = Execute with rollback, False = Execute without rollback
 */
void DB_CreateChamberVoltage(CHAMBERVOLTAGE *chambervoltage, long long int ChamberDailyTestID, bool bookEnd){
	char MeasuredOn[40];

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO ChamberVoltage(ChamberSerialNumber, ChamberType, TwoStageChamber, NominalVoltage, MinVoltage, MaxVoltage, VoltageStatus, VoltageTextEnglish, VoltageTextFrench, VoltageValue, MeasuredOn, InactiveReason, Inactive) VALUES(:ChamberSerialNumber, :ChamberType, :TwoStageChamber, :NominalVoltage, :MinVoltage, :MaxVoltage, :VoltageStatus, :VoltageTextEnglish, :VoltageTextFrench, :VoltageValue, julianday(:MeasuredOn), :InactiveReason, :Inactive);");
	d_bind_text(":ChamberSerialNumber", chambervoltage->ChamberSerialNumber);
	d_bind_int(":ChamberType", chambervoltage->ChamberType);
	d_bind_int(":TwoStageChamber", chambervoltage->TwoStageChamber);
	d_bind_double(":NominalVoltage", chambervoltage->NominalVoltage);
	d_bind_double(":MinVoltage", chambervoltage->MinVoltage);
	d_bind_double(":MaxVoltage", chambervoltage->MaxVoltage);
	d_bind_int(":VoltageStatus", chambervoltage->VoltageStatus);
	d_bind_text(":VoltageTextEnglish", chambervoltage->VoltageTextEnglish);
	d_bind_text(":VoltageTextFrench", chambervoltage->VoltageTextFrench);
	d_bind_double(":VoltageValue", chambervoltage->VoltageValue);
	stringout(MeasuredOn, &(chambervoltage->MeasuredOn));
	d_bind_text(":MeasuredOn", MeasuredOn);
	d_bind_text(":InactiveReason", "");
	d_bind_int(":Inactive", 0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) chambervoltage->ChamberVoltageID = sqlite3_last_insert_rowid(capDB);
	else chambervoltage->ChamberVoltageID = 0;
	d_finalize();

	if(ChamberDailyTestID > 0){
		if(chambervoltage->ChamberVoltageID > 0){
			DB_UpdateDailyTestVoltageID(ChamberDailyTestID, chambervoltage->ChamberVoltageID);
		}
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}
/**
 * \details Update Daily Test Record in the ChamberDailyTest table with the results of the Data Check Test
 * \param ChamberDailyTestID Primary Key to the record in the ChambeDailyTest table
 * \param DataCheckTextEnglish Pointer to Null terminated text string to be updated in the DataCheckTextEnglish field
 * \param DataCheckTextFrench Pointer to Null terminated text string to be updated in the DataCheckTextFrench field
 * \param DataCheckFailed Value to be updated in the DataCheckFailed field
 * \param DataCheckCRC Pointer to Null terminated text string to be updated in the DataCheckCRC field
 * \param bookEnd True = Execute with rollback, False = Execute without rollback
 * \returns None
 */
void DB_UpdateDailyTestDataCheck(long long int ChamberDailyTestID, char *DataCheckTextEnglish, char *DataCheckTextFrench, bool DataCheckFailed, char *DataCheckCRC, bool bookEnd){
	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ChamberDailyTest SET DataCheckTextEnglish = :DataCheckTextEnglish, DataCheckTextFrench = :DataCheckTextFrench, DataCheckFailed = :DataCheckFailed, DataCheckCRC = :DataCheckCRC WHERE ChamberDailyTestID = :ChamberDailyTestID;");
	d_bind_int64(":ChamberDailyTestID", ChamberDailyTestID);
	d_bind_text(":DataCheckTextEnglish", DataCheckTextEnglish);
	d_bind_text(":DataCheckTextFrench", DataCheckTextFrench);
	d_bind_int(":DataCheckFailed", DataCheckFailed);
	d_bind_text(":DataCheckCRC", DataCheckCRC);
	d_step();
	d_finalize();

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}
/**
 * \details Save Chamber Accuracy Test into the ChamberAccuracyTest table
 * \param chamberaccuracytest Pointer to CHAMBERACCURACYTEST structure, which holds the Chamber Accuracy Test info
 * \param ChamberDailyTestID Primary Key to the record in the ChamberDailyTest table. The ChamberAccuracyTestID foreign key will be updated with the primary key from the ChamberAccuracyTest table
 * \param bookEnd True = Execute with rollback, False = Execute without rollback
 * \returns None
 */
void DB_CreateChamberAccuracyTest(CHAMBERACCURACYTEST *chamberaccuracytest, long long int ChamberDailyTestID, bool bookEnd){
	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO ChamberAccuracyTest(ChamberSerialNumber, ChamberType, TwoStageChamber, ConstancyNuclide, ConstancyNuclideSerialNumber, ConstancyVoltage, InactiveReason, Inactive) VALUES(:ChamberSerialNumber, :ChamberType, :TwoStageChamber, :ConstancyNuclide, :ConstancyNuclideSerialNumber, :ConstancyVoltage, :InactiveReason, :Inactive);");
	d_bind_text(":ChamberSerialNumber", chamberaccuracytest->ChamberSerialNumber);
	d_bind_int(":ChamberType", chamberaccuracytest->ChamberType);
	d_bind_int(":TwoStageChamber", chamberaccuracytest->TwoStageChamber);
	d_bind_text(":ConstancyNuclide", chamberaccuracytest->ConstancyNuclide);
	d_bind_text(":ConstancyNuclideSerialNumber", chamberaccuracytest->ConstancyNuclideSerialNumber);
	d_bind_double(":ConstancyVoltage", chamberaccuracytest->ConstancyVoltage);
	d_bind_text(":InactiveReason", "");
	d_bind_int(":Inactive", 0);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) chamberaccuracytest->ChamberAccuracyTestID = sqlite3_last_insert_rowid(capDB);
	else chamberaccuracytest->ChamberAccuracyTestID = 0;
	d_finalize();

	if(ChamberDailyTestID > 0){
		if(chamberaccuracytest->ChamberAccuracyTestID > 0){
			DB_UpdateDailyTestAccuracyTestID(ChamberDailyTestID, chamberaccuracytest->ChamberAccuracyTestID);
		}
	}

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}
/**
 * \details Save Chamber Accuracy Measurement into the ChamberAccuracyMeasurement table
 * \param chamberaccuracymeasurement Pointer to CHAMBERACCURACYMEASUREMENT structure, which holds the Chamber Accuracy Measurement info
 * \param bookEnd True = Execute with rollback, False = Execute without rollback
 * \returns None
 */
void DB_CreateChamberAccuracyMeasurement(CHAMBERACCURACYMEASUREMENT *chamberaccuracymeasurement, bool bookEnd){
	char DateString[40];

	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO ChamberAccuracyMeasurement(ChamberAccuracyTestID, SourceNuclide, SourceNuclideIndex, SourceSerialNumber, SourceCalActivity, SourceCalDate, MeasuredOn, CalculatedActivityTextEnglish, CalculatedActivityTextFrench, CalculatedActivity, Response, HalfLife, HalfLifeUnit, MeasuredActivityTextEnglish, MeasuredActivityTextFrench, MeasuredActivity, DeviationStatus, DeviationTextEnglish, DeviationTextFrench, Deviation) VALUES(:ChamberAccuracyTestID, :SourceNuclide, :SourceNuclideIndex, :SourceSerialNumber, :SourceCalActivity,  julianday(:SourceCalDate),  julianday(:MeasuredOn), :CalculatedActivityTextEnglish, :CalculatedActivityTextFrench, :CalculatedActivity, :Response, :HalfLife, :HalfLifeUnit, :MeasuredActivityTextEnglish, :MeasuredActivityTextFrench, :MeasuredActivity, :DeviationStatus, :DeviationTextEnglish, :DeviationTextFrench, :Deviation);");
	d_bind_int64(":ChamberAccuracyTestID", chamberaccuracymeasurement->ChamberAccuracyTestID);
	d_bind_text(":SourceNuclide", chamberaccuracymeasurement->SourceNuclide);
    d_bind_int(":SourceNuclideIndex", chamberaccuracymeasurement->SourceNuclideIndex);
	d_bind_text(":SourceSerialNumber", chamberaccuracymeasurement->SourceSerialNumber);
	d_bind_double(":SourceCalActivity", chamberaccuracymeasurement->SourceCalActivity);
	stringout(DateString, &(chamberaccuracymeasurement->SourceCalDate));
	d_bind_text(":SourceCalDate", DateString);
	stringout(DateString, &(chamberaccuracymeasurement->MeasuredOn));
	d_bind_text(":MeasuredOn", DateString);
	d_bind_text(":CalculatedActivityTextEnglish", chamberaccuracymeasurement->CalculatedActivityTextEnglish);
	d_bind_text(":CalculatedActivityTextFrench", chamberaccuracymeasurement->CalculatedActivityTextFrench);
	d_bind_double(":CalculatedActivity", chamberaccuracymeasurement->CalculatedActivity);
	d_bind_double(":Response", chamberaccuracymeasurement->Response);
	d_bind_double(":HalfLife", chamberaccuracymeasurement->HalfLife);
	d_bind_int(":HalfLifeUnit", chamberaccuracymeasurement->HalfLifeUnit);
	d_bind_text(":MeasuredActivityTextEnglish", chamberaccuracymeasurement->MeasuredActivityTextEnglish);
	d_bind_text(":MeasuredActivityTextFrench", chamberaccuracymeasurement->MeasuredActivityTextFrench);
	d_bind_double(":MeasuredActivity", chamberaccuracymeasurement->MeasuredActivity);
	d_bind_int(":DeviationStatus", chamberaccuracymeasurement->DeviationStatus);
	d_bind_text(":DeviationTextEnglish", chamberaccuracymeasurement->DeviationTextEnglish);
	d_bind_text(":DeviationTextFrench", chamberaccuracymeasurement->DeviationTextFrench);
	d_bind_double(":Deviation", chamberaccuracymeasurement->Deviation);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) chamberaccuracymeasurement->ChamberAccuracyMeasurementID = sqlite3_last_insert_rowid(capDB);
	else chamberaccuracymeasurement->ChamberAccuracyMeasurementID = 0;
	d_finalize();

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}
/**
 * \details Starts the SQL Query Block
 * \returns None
 */
void DB_StartBookEnd(void){
	*capStatus = SQLITE_OK;
	d_command("BEGIN;");
}
/**
 * \details Save Chamber AutoConstancy Test info the ChamberAutoConstancy table
 * \param chamberautoconstancy Pointer to CHAMBERAUTOCONSTANCY structure, which holds the Chamber Autoconstancy Test info
 * \param bookEnd True = Execute with rollback, False = Execute without rollback
 */
void DB_CreateChamberAutoConstancy(CHAMBERAUTOCONSTANCY *chamberautoconstancy, bool bookEnd){
	if(bookEnd){
		*capStatus = SQLITE_OK;
		d_command("BEGIN;");
	}

	d_prepare_v2("INSERT INTO ChamberAutoConstancy(ChamberAccuracyTestID, ConstancyNuclide, ConstancyNuclideIndex, Response, HalfLife, HalfLifeUnit, ConstancyActivityTextEnglish, ConstancyActivityTextFrench, ConstancyActivity) VALUES(:ChamberAccuracyTestID, :ConstancyNuclide, :ConstancyNuclideIndex, :Response, :HalfLife, :HalfLifeUnit, :ConstancyActivityTextEnglish, :ConstancyActivityTextFrench, :ConstancyActivity);");
	d_bind_int64(":ChamberAccuracyTestID", chamberautoconstancy->ChamberAccuracyTestID);
	d_bind_text(":ConstancyNuclide", chamberautoconstancy->ConstancyNuclide);
	d_bind_int(":ConstancyNuclideIndex", chamberautoconstancy->ConstancyNuclideIndex);
	d_bind_double(":Response", chamberautoconstancy->Response);
	d_bind_double(":HalfLife", chamberautoconstancy->HalfLife);
	d_bind_int(":HalfLifeUnit", chamberautoconstancy->HalfLifeUnit);
	d_bind_text(":ConstancyActivityTextEnglish", chamberautoconstancy->ConstancyActivityTextEnglish);
	d_bind_text(":ConstancyActivityTextFrench", chamberautoconstancy->ConstancyActivityTextFrench);
	d_bind_double(":ConstancyActivity", chamberautoconstancy->ConstancyActivity);
	d_step();
	if(*capStatus == SQLITE_DONE) *capStatus = SQLITE_OK;
	if(*capStatus == SQLITE_OK) chamberautoconstancy->ChamberAutoConstancyID = sqlite3_last_insert_rowid(capDB);
	else chamberautoconstancy->ChamberAutoConstancyID = 0;
	d_finalize();

	if(bookEnd){
		if(*capStatus == SQLITE_OK) d_command("COMMIT;");
		else{
			*capStatus = SQLITE_OK;
			d_command("ROLLBACK;");
		}
	}
}
/**
 * \details Ends the SQL Query Block
 * \returns None
 */
void DB_EndBookEnd(void){
	if(*capStatus == SQLITE_OK) d_command("COMMIT;");
	else{
		*capStatus = SQLITE_OK;
		d_command("ROLLBACK;");
	}
}
/**
 * \details Search ChamberDailyTest table by date range
 * \param tests Pointer to an array of CHAMBERSEARCH structure, which will receive the results of the search
 * \param startDate Start of the date range
 * \param endDate End of the date range
 * \param maxRetrieved Maximum number of results to retrieve
 * \returns Actual number of results returned from the search
 */
static int DB_SearchDailyTests(CHAMBERSEARCH *tests, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount, index;
	CHAMBERSEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = tests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->ChamberDailyTestID = 0;
		testPtr->ChamberZeroID = 0;
		testPtr->ChamberBackgroundID = 0;
		testPtr->ChamberVoltageID = 0;
		testPtr->ChamberAccuracyTestID = 0;
		testPtr->Results[0] = 0;
		testPtr++;
	}
	testPtr = tests;

	retrievedCount = 0;
	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT ChamberDailyTestID, ChamberSerialNumber, ChamberZeroID, ChamberBackgroundID, ChamberVoltageID, ChamberAccuracyTestID, datetime(CreatedOn), Inactive FROM ChamberDailyTest WHERE CreatedOn >= julianday(:startDate) AND CreatedOn < julianday(:endDate) ORDER BY CreatedOn DESC;");
	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();

	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->ChamberDailyTestID = d_column_int64(0);
				strcpy(testPtr->SerialNumber, (const char *) d_column_text(1));
				testPtr->ChamberZeroID = d_column_int64(2);
				testPtr->ChamberBackgroundID = d_column_int64(3);
				testPtr->ChamberVoltageID = d_column_int64(4);
				testPtr->ChamberAccuracyTestID = d_column_int64(5);
				testPtr->StartedOn = strtotime_t((const char *) d_column_text(6));

				if(d_column_int(7) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;

				testPtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}
/**
 * \details Search ChamberZero table by date range
 * \param tests Pointer to an array of CHAMBERSEARCH structure, which will receive the results of the search
 * \param startDate Start of the date range
 * \param endDate End of the date range
 * \param maxRetrieved Maximum number of results to retrieve
 * \returns Actual number of results returned from the search
 */
static int DB_SearchZeroMeasurements(CHAMBERSEARCH *tests, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount, index;
	CHAMBERSEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = tests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->ChamberDailyTestID = 0;
		testPtr->ChamberZeroID = 0;
		testPtr->ChamberBackgroundID = 0;
		testPtr->ChamberVoltageID = 0;
		testPtr->ChamberAccuracyTestID = 0;
		testPtr->Results[0] = 0;
		testPtr++;
	}
	testPtr = tests;

	retrievedCount = 0;
	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT ChamberZeroID, ChamberSerialNumber, ZeroTextEnglish, ZeroTextFrench, datetime(MeasuredOn), Inactive FROM ChamberZero WHERE MeasuredOn >= julianday(:startDate) AND MeasuredOn < julianday(:endDate) ORDER BY MeasuredOn DESC;");
	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();

	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->ChamberZeroID = d_column_int64(0);
				strcpy(testPtr->SerialNumber, (const char *) d_column_text(1));

				if(current.language == ENGLISH) strcpy(testPtr->Results, (const char *) d_column_text(2));
				else if(current.language == FRENCH) strcpy(testPtr->Results, (const char *) d_column_text(3));

				testPtr->StartedOn = strtotime_t((const char *) d_column_text(4));

				if(d_column_int(5) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;

				if(strlen(testPtr->Results) > 25) testPtr->Results[25] = 0;

				testPtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}
/**
 * \details Search ChamberBackground table by date range
 * \param tests Pointer to an array of CHAMBERSEARCH structure, which will receive the results of the search
 * \param startDate Start of the date range
 * \param endDate End of the date range
 * \param maxRetrieved Maximum number of results to retrieve
 * \returns Actual number of results returned from the search
 */
static int DB_SearchBackgroundMeasurements(CHAMBERSEARCH *tests, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount, index;
	CHAMBERSEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = tests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->ChamberDailyTestID = 0;
		testPtr->ChamberZeroID = 0;
		testPtr->ChamberBackgroundID = 0;
		testPtr->ChamberVoltageID = 0;
		testPtr->ChamberAccuracyTestID = 0;
		testPtr->Results[0] = 0;
		testPtr++;
	}
	testPtr = tests;

	retrievedCount = 0;
	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT ChamberBackgroundID, ChamberSerialNumber, BackgroundTextEnglish, BackgroundTextFrench, datetime(MeasuredOn), Inactive FROM ChamberBackground WHERE MeasuredOn >= julianday(:startDate) AND MeasuredOn < julianday(:endDate) ORDER BY MeasuredOn DESC;");
	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();

	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->ChamberBackgroundID = d_column_int64(0);
				strcpy(testPtr->SerialNumber, (const char *) d_column_text(1));

				if(current.language == ENGLISH) strcpy(testPtr->Results, (const char *) d_column_text(2));
				else if(current.language == FRENCH) strcpy(testPtr->Results, (const char *) d_column_text(3));

				testPtr->StartedOn = strtotime_t((const char *) d_column_text(4));

				if(d_column_int(5) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;

				if(strlen(testPtr->Results) > 25) testPtr->Results[25] = 0;

				testPtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}
/**
 * \details Search ChamberVoltage table by date range
 * \param tests Pointer to an array of CHAMBERSEARCH structure, which will receive the results of the search
 * \param startDate Start of the date range
 * \param endDate End of the date range
 * \param maxRetrieved Maximum number of results to retrieve
 * \returns Actual number of results returned from the search
 */
static int DB_SearchChamberVoltage(CHAMBERSEARCH *tests, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount, index;
	CHAMBERSEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = tests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->ChamberDailyTestID = 0;
		testPtr->ChamberZeroID = 0;
		testPtr->ChamberBackgroundID = 0;
		testPtr->ChamberVoltageID = 0;
		testPtr->ChamberAccuracyTestID = 0;
		testPtr->Results[0] = 0;
		testPtr++;
	}
	testPtr = tests;

	retrievedCount = 0;
	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT ChamberVoltageID, ChamberSerialNumber, VoltageTextEnglish, VoltageTextFrench, datetime(MeasuredOn), Inactive FROM ChamberVoltage WHERE MeasuredOn >= julianday(:startDate) AND MeasuredOn < julianday(:endDate) ORDER BY MeasuredOn DESC;");
	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();

	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->ChamberVoltageID = d_column_int64(0);
				strcpy(testPtr->SerialNumber, (const char *) d_column_text(1));

				if(current.language == ENGLISH) strcpy(testPtr->Results, (const char *) d_column_text(2));
				else if(current.language == FRENCH) strcpy(testPtr->Results, (const char *) d_column_text(3));

				testPtr->StartedOn = strtotime_t((const char *) d_column_text(4));

				if(d_column_int(5) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;

				if(strlen(testPtr->Results) > 25) testPtr->Results[25] = 0;

				testPtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}
/**
 * \details Search ChamberAccuracyTest table by date range
 * \param tests Pointer to an array of CHAMBERSEARCH structure, which will receive the results of the search
 * \param startDate Start of the date range
 * \param endDate End of the date range
 * \param maxRetrieved Maximum number of results to retrieve
 * \returns Actual number of results returned from the search
 */
static int DB_SearchAccuracyTest(CHAMBERSEARCH *tests, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount, index;
	CHAMBERSEARCH *testPtr;
	char datestring[30];
	time_t buffer;

	testPtr = tests;
	for(index=0; index<maxRetrieved; index++){
		testPtr->ChamberDailyTestID = 0;
		testPtr->ChamberZeroID = 0;
		testPtr->ChamberBackgroundID = 0;
		testPtr->ChamberVoltageID = 0;
		testPtr->ChamberAccuracyTestID = 0;
		testPtr->Results[0] = 0;
		testPtr++;
	}
	testPtr = tests;

	retrievedCount = 0;
	*capStatus = SQLITE_OK;

	if(startDate > endDate){
		buffer = endDate;
		endDate = startDate;
		startDate = buffer;
	}
	endDate += 86400;

	d_prepare_v2("SELECT ChamberAccuracyTestID, ChamberSerialNumber, datetime(CreatedOn), Inactive FROM ChamberAccuracyTest WHERE CreatedOn >= julianday(:startDate) AND CreatedOn < julianday(:endDate) ORDER BY CreatedOn DESC;");
	stringout(datestring, &startDate);
	d_bind_text(":startDate", datestring);
	stringout(datestring, &endDate);
	d_bind_text(":endDate", datestring);
	d_step();

	if(*capStatus == SQLITE_ROW){
		while((*capStatus == SQLITE_ROW) && (retrievedCount <= maxRetrieved)){
			if(retrievedCount < maxRetrieved){
				testPtr->ChamberAccuracyTestID = d_column_int64(0);
				strcpy(testPtr->SerialNumber, (const char *) d_column_text(1));
				testPtr->StartedOn = strtotime_t((const char *) d_column_text(2));

				if(d_column_int(3) == 0) testPtr->Inactive = FALSE;
				else testPtr->Inactive = TRUE;

				testPtr++;
			}
			retrievedCount++;
			d_step();
		}
		if(retrievedCount > maxRetrieved) retrievedCount = -1;
	}
	d_finalize();

	return retrievedCount;
}
/**
 * \details Search Chamber Test by date range
 * \param mode Specify database table, 108 = ChamberDailyTest, 109 = ChamberZero, 110 = ChamberBackground, 111 = ChamberVoltage, 112 = ChamberAccuracyTest
 * \param tests Pointer to an array of CHAMBERSEARCH structure, which will receive the results of the search
 * \param startDate Start of the date range
 * \param endDate End of the date range
 * \param maxRetrieved Maximum number of results to retrieve
 * \returns Actual number of results returned from the search
 */
int DB_SearchChamberTests(int mode, CHAMBERSEARCH *tests, time_t startDate, time_t endDate, int maxRetrieved){
	int retrievedCount;

	retrievedCount = 0;
	switch(mode){
		case 108:
			retrievedCount = DB_SearchDailyTests(tests, startDate, endDate, maxRetrieved);
			break;

		case 109:
			retrievedCount = DB_SearchZeroMeasurements(tests, startDate, endDate, maxRetrieved);
			break;

		case 110:
			retrievedCount = DB_SearchBackgroundMeasurements(tests, startDate, endDate, maxRetrieved);
			break;

		case 111:
			retrievedCount = DB_SearchChamberVoltage(tests, startDate, endDate, maxRetrieved);
			break;

		case 112:
			retrievedCount = DB_SearchAccuracyTest(tests, startDate, endDate, maxRetrieved);
			break;
	}

	return retrievedCount;
}
/**
 * \details Retrieve the details of a daily test from ChamberDailyTest table
 * \param test Pointer to CHAMBERDAILYTEST structure, which contains the Primary Key, ChamberDailyTestID, and receives the details from the record
 * \returns None
 */
void DB_RetrieveDailyTest(CHAMBERDAILYTEST *test){
	test->CreatedOn = 0;

	if(test->ChamberDailyTestID > 0){
		*capStatus = SQLITE_OK;

		d_prepare_v2("SELECT ChamberSerialNumber, ChamberType, TwoStageChamber, DataCheckTextEnglish, DataCheckTextFrench, DataCheckFailed, DataCheckCRC, ChamberZeroID, ChamberBackgroundID, ChamberVoltageID, ChamberAccuracyTestID, datetime(CreatedOn), InactiveReason, Inactive FROM ChamberDailyTest WHERE ChamberDailyTestID = :ChamberDailyTestID;");
		d_bind_int64(":ChamberDailyTestID", test->ChamberDailyTestID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			strcpy(test->ChamberSerialNumber, (const char *) d_column_text(0));
			test->ChamberType = d_column_int(1);

			if(d_column_int(2) == 0) test->TwoStageChamber = FALSE;
			else test->TwoStageChamber = TRUE;

			strcpy(test->DataCheckTextEnglish, (const char *) d_column_text(3));
			strcpy(test->DataCheckTextFrench, (const char *) d_column_text(4));

			if(d_column_int(5) == 0) test->DataCheckFailed = FALSE;
			else test->DataCheckFailed = TRUE;

			strcpy(test->DataCheckCRC, (const char *) d_column_text(6));

			test->ChamberZeroID = d_column_int64(7);
			test->ChamberBackgroundID = d_column_int64(8);
			test->ChamberVoltageID = d_column_int64(9);
			test->ChamberAccuracyTestID = d_column_int64(10);
			test->CreatedOn = strtotime_t((const char *) d_column_text(11));
			strcpy(test->InactiveReason, (const char *) d_column_text(12));

			if(d_column_int(13) == 0) test->Inactive = FALSE;
			else test->Inactive = TRUE;
		}
		d_finalize();
	}
}
/**
 * \details Retrieve the details of a zero test from ChamberZero table
 * \param test Pointer to CHAMBERZERO structure, which contains the Primary Key, ChamberZeroID, and receives the details from the record
 * \returns None
 */
void DB_RetrieveZeroMeasurement(CHAMBERZERO *test){
	test->CreatedOn = 0;

	if(test->ChamberZeroID > 0){
		*capStatus = SQLITE_OK;
		d_prepare_v2("SELECT ChamberSerialNumber, ChamberType, TwoStageChamber, ZeroStatus, ZeroTextEnglish, ZeroTextFrench, ZeroValue, datetime(MeasuredOn), datetime(CreatedOn), InactiveReason, Inactive FROM ChamberZero WHERE ChamberZeroID = :ChamberZeroID;");
		d_bind_int64(":ChamberZeroID", test->ChamberZeroID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			strcpy(test->ChamberSerialNumber, (const char *) d_column_text(0));
			test->ChamberType = d_column_int(1);

			if(d_column_int(2) == 0) test->TwoStageChamber = FALSE;
			else test->TwoStageChamber = TRUE;

			test->ZeroStatus = d_column_int(3);
			strcpy(test->ZeroTextEnglish, (const char *) d_column_text(4));
			strcpy(test->ZeroTextFrench, (const char *) d_column_text(5));
			test->ZeroValue = d_column_double(6);
			test->MeasuredOn = strtotime_t((const char *) d_column_text(7));
			test->CreatedOn = strtotime_t((const char *) d_column_text(8));
			strcpy(test->InactiveReason, (const char *) d_column_text(9));

			if(d_column_int(10) == 0) test->Inactive = FALSE;
			else test->Inactive = TRUE;
		}
		d_finalize();
	}
}
/**
 * \details Retrieve the details of a background test from ChamberBackground table
 * \param test Pointer to CHAMBERBACKGROUND structure, which contains the Primary Key, ChamberBackgroundID, and receives the details from the record
 * \returns None
 */
void DB_RetrieveBackgroundMeasurement(CHAMBERBACKGROUND *test){
	test->CreatedOn = 0;

	if(test->ChamberBackgroundID > 0){
		*capStatus = SQLITE_OK;
		d_prepare_v2("SELECT ChamberSerialNumber, ChamberType, TwoStageChamber, BackgroundStatus, BackgroundTextEnglish, BackgroundTextFrench, BackgroundValue, datetime(MeasuredOn), datetime(CreatedOn), InactiveReason, Inactive FROM ChamberBackground WHERE ChamberBackgroundID = :ChamberBackgroundID;");
		d_bind_int64(":ChamberBackgroundID", test->ChamberBackgroundID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			strcpy(test->ChamberSerialNumber, (const char *) d_column_text(0));
			test->ChamberType = d_column_int(1);

			if(d_column_int(2) == 0) test->TwoStageChamber = FALSE;
			else test->TwoStageChamber = TRUE;

			test->BackgroundStatus = d_column_int(3);
			strcpy(test->BackgroundTextEnglish, (const char *) d_column_text(4));
			strcpy(test->BackgroundTextFrench, (const char *) d_column_text(5));
			test->BackgroundValue = d_column_double(6);
			test->MeasuredOn = strtotime_t((const char *) d_column_text(7));
			test->CreatedOn = strtotime_t((const char *) d_column_text(8));
			strcpy(test->InactiveReason, (const char *) d_column_text(9));

			if(d_column_int(10) == 0) test->Inactive = FALSE;
			else test->Inactive = TRUE;
		}
		d_finalize();
	}
}
/**
 * \details Retrieve the details of a chamber voltage test from ChamberVoltage table
 * \param test Pointer to CHAMBERVOLTAGE structure, which contains the Primary Key, ChamberVoltageID, and receives the details from the record
 * \returns None
 */
void DB_RetrieveChamberVoltage(CHAMBERVOLTAGE *test){
	test->CreatedOn = 0;

	if(test->ChamberVoltageID > 0){
		*capStatus = SQLITE_OK;
		d_prepare_v2("SELECT ChamberSerialNumber, ChamberType, TwoStageChamber, NominalVoltage, MinVoltage, MaxVoltage, VoltageStatus, VoltageTextEnglish, VoltageTextFrench, VoltageValue, datetime(MeasuredOn), datetime(CreatedOn), InactiveReason, Inactive FROM ChamberVoltage WHERE ChamberVoltageID = :ChamberVoltageID;");
		d_bind_int64(":ChamberVoltageID", test->ChamberVoltageID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			strcpy(test->ChamberSerialNumber, (const char *) d_column_text(0));
			test->ChamberType = d_column_int(1);

			if(d_column_int(2) == 0) test->TwoStageChamber = FALSE;
			else test->TwoStageChamber = TRUE;

			test->NominalVoltage = d_column_double(3);
			test->MinVoltage = d_column_double(4);
			test->MaxVoltage = d_column_double(5);
			test->VoltageStatus = d_column_int(6);
			strcpy(test->VoltageTextEnglish, (const char *) d_column_text(7));
			strcpy(test->VoltageTextFrench, (const char *) d_column_text(8));
			test->VoltageValue = d_column_double(9);
			test->MeasuredOn = strtotime_t((const char *) d_column_text(10));
			test->CreatedOn = strtotime_t((const char *) d_column_text(11));
			strcpy(test->InactiveReason, (const char *) d_column_text(12));

			if(d_column_int(13) == 0) test->Inactive = FALSE;
			else test->Inactive = TRUE;
		}
		d_finalize();
	}
}
/**
 * \details Retrieve the details of a chamber accuracy test from the ChamberAccuracyTest, ChamberAccuracyMeasurement, ChamberAutoConstancy tables
 * \param test Pointer to CHAMBERACCURACYTEST structure, which contains the Primary Key, ChamberAccuracyTestID, and receives the details from the record
 * \param measurement An array of CHAMBERACCURACYMEASUREMENT structures, which receives the details from the ChamberAccuracyMeasurement table
 * \param autoconstancy An array of CHAMBERAUTOCONSTANCY structures, which receives the details from the ChamberAutoConstancy table
 * \returns None
 */
void DB_RetrieveAccuracyTest(CHAMBERACCURACYTEST *test, CHAMBERACCURACYMEASUREMENT measurement[], CHAMBERAUTOCONSTANCY autoconstancy[]){
	int i;

	test->CreatedOn = 0;
	for(i=0; i<6; i++) measurement[i].ChamberAccuracyMeasurementID = 0;
	for(i=0; i<12; i++) autoconstancy[i].ChamberAutoConstancyID = 0;

	if(test->ChamberAccuracyTestID > 0){
		*capStatus = SQLITE_OK;

		d_prepare_v2("SELECT ChamberSerialNumber, ChamberType, TwoStageChamber, ConstancyNuclide, ConstancyNuclideSerialNumber, ConstancyVoltage, datetime(CreatedOn), InactiveReason, Inactive FROM ChamberAccuracyTest WHERE ChamberAccuracyTestID = :ChamberAccuracyTestID;");
		d_bind_int64(":ChamberAccuracyTestID", test->ChamberAccuracyTestID);
		d_step();
		if(*capStatus == SQLITE_ROW){
			strcpy(test->ChamberSerialNumber, (const char *) d_column_text(0));
			test->ChamberType = d_column_int(1);

			if(d_column_int(2) == 0) test->TwoStageChamber = FALSE;
			else test->TwoStageChamber = TRUE;

			strcpy(test->ConstancyNuclide, (const char *) d_column_text(3));
			strcpy(test->ConstancyNuclideSerialNumber, (const char *) d_column_text(4));
			test->ConstancyVoltage = d_column_double(5);
			test->CreatedOn = strtotime_t((const char *) d_column_text(6));
			strcpy(test->InactiveReason, (const char *) d_column_text(7));

			if(d_column_int(8) == 0) test->Inactive = FALSE;
			else test->Inactive = TRUE;
		}
		d_finalize();

		if(test->CreatedOn != 0){
			i = 0;
			*capStatus = SQLITE_OK;

			d_prepare_v2("SELECT ChamberAccuracyMeasurementID, SourceNuclide, SourceNuclideIndex, SourceSerialNumber, SourceCalActivity, datetime(SourceCalDate), datetime(MeasuredOn), CalculatedActivityTextEnglish, CalculatedActivityTextFrench, CalculatedActivity, Response, HalfLife, HalfLifeUnit, MeasuredActivityTextEnglish, MeasuredActivityTextFrench, MeasuredActivity, DeviationStatus, DeviationTextEnglish, DeviationTextFrench, Deviation FROM ChamberAccuracyMeasurement WHERE ChamberAccuracyTestID = :ChamberAccuracyTestID;");
			d_bind_int64(":ChamberAccuracyTestID", test->ChamberAccuracyTestID);
			d_step();

			if(*capStatus == SQLITE_ROW){
				while((*capStatus == SQLITE_ROW) && (i < 6)){
					measurement[i].ChamberAccuracyMeasurementID = d_column_int64(0);
					measurement[i].ChamberAccuracyTestID = test->ChamberAccuracyTestID;
					strcpy(measurement[i].SourceNuclide, (const char *) d_column_text(1));
					measurement[i].SourceNuclideIndex = d_column_int(2);
					strcpy(measurement[i].SourceSerialNumber, (const char *) d_column_text(3));
					measurement[i].SourceCalActivity = d_column_double(4);
					measurement[i].SourceCalDate = strtotime_t((const char *) d_column_text(5));
					measurement[i].MeasuredOn = strtotime_t((const char *) d_column_text(6));
					strcpy(measurement[i].CalculatedActivityTextEnglish, (const char *) d_column_text(7));
					strcpy(measurement[i].CalculatedActivityTextFrench, (const char *) d_column_text(8));
					measurement[i].CalculatedActivity = d_column_double(9);
					measurement[i].Response = d_column_double(10);
					measurement[i].HalfLife = d_column_double(11);
					measurement[i].HalfLifeUnit = d_column_int(12);
					strcpy(measurement[i].MeasuredActivityTextEnglish, (const char *) d_column_text(13));
					strcpy(measurement[i].MeasuredActivityTextFrench, (const char *) d_column_text(14));
					measurement[i].MeasuredActivity = d_column_double(15);
					measurement[i].DeviationStatus = d_column_int(16);
					strcpy(measurement[i].DeviationTextEnglish, (const char *) d_column_text(17));
					strcpy(measurement[i].DeviationTextFrench, (const char *) d_column_text(18));
					measurement[i].Deviation = d_column_double(19);

					i++;
					d_step();
				}
			}
			d_finalize();

			i = 0;
			*capStatus = SQLITE_OK;
			d_prepare_v2("SELECT ChamberAutoConstancyID, ConstancyNuclide, ConstancyNuclideIndex, Response, HalfLife, HalfLifeUnit, ConstancyActivityTextEnglish, ConstancyActivityTextFrench, ConstancyActivity FROM ChamberAutoConstancy WHERE ChamberAccuracyTestID = :ChamberAccuracyTestID;");
			d_bind_int64(":ChamberAccuracyTestID", test->ChamberAccuracyTestID);
			d_step();

			if(*capStatus == SQLITE_ROW){
				while((*capStatus == SQLITE_ROW) && (i < 12)){
					autoconstancy[i].ChamberAutoConstancyID = d_column_int64(0);
					autoconstancy[i].ChamberAccuracyTestID = test->ChamberAccuracyTestID;
					strcpy(autoconstancy[i].ConstancyNuclide, (const char *) d_column_text(1));
					autoconstancy[i].ConstancyNuclideIndex = d_column_int(2);
					autoconstancy[i].Response = d_column_double(3);
					autoconstancy[i].HalfLife = d_column_double(4);
					autoconstancy[i].HalfLifeUnit = d_column_int(5);
					strcpy(autoconstancy[i].ConstancyActivityTextEnglish, (const char *) d_column_text(6));
					strcpy(autoconstancy[i].ConstancyActivityTextFrench, (const char *) d_column_text(7));
					autoconstancy[i].ConstancyActivity = d_column_double(8);

					i++;
					d_step();
				}
			}
			d_finalize();
		}
	}
}
/**
 * \details Inactivates a record in the ChamberDailyTest table
 * \param test Pointer to CHAMBERDAILYTEST structure, which contains the Primary Key, ChamberDailyTestID, and the InactiveReason text
 * \returns None
 */
void DB_InactivateDailyTest(CHAMBERDAILYTEST *test){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ChamberDailyTest SET InactiveReason = :InactiveReason, Inactive = 1 WHERE ChamberDailyTestID = :TestID;");
	d_bind_text(":InactiveReason", test->InactiveReason);
	d_bind_int64(":TestID", test->ChamberDailyTestID);
	d_step();
	d_finalize();
	test->Inactive = TRUE;
}
/**
 * \details Inactivates a record in the ChamberZero table
 * \param test Pointer to CHAMBERZERO structure, which contains the Primary Key, ChamberZeroID, and the InactiveReason text
 * \returns None
 */
void DB_InactivateZeroMeasurement(CHAMBERZERO *test){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ChamberZero SET InactiveReason = :InactiveReason, Inactive = 1 WHERE ChamberZeroID = :TestID;");
	d_bind_text(":InactiveReason", test->InactiveReason);
	d_bind_int64(":TestID", test->ChamberZeroID);
	d_step();
	d_finalize();
	test->Inactive = TRUE;
}
/**
 * \details Inactivates a record in the ChamberBackground table
 * \param test Pointer to CHAMBERBACKGROUND structure, which contains the Primary Key, ChamberBackgroundID, and the InactiveReason text
 * \returns None
 */
void DB_InactivateBackgroundMeasurement(CHAMBERBACKGROUND *test){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ChamberBackground SET InactiveReason = :InactiveReason, Inactive = 1 WHERE ChamberBackgroundID = :TestID;");
	d_bind_text(":InactiveReason", test->InactiveReason);
	d_bind_int64(":TestID", test->ChamberBackgroundID);
	d_step();
	d_finalize();
	test->Inactive = TRUE;
}
/**
 * \details Inactivates a record in the ChamberVoltage table
 * \param test Pointer to CHAMBERVOLTAGE structure, which contains the Primary Key, ChmaberVoltageID, and the InactiveReason text
 * \returns None
 */
void DB_InactivateChamberVoltage(CHAMBERVOLTAGE *test){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ChamberVoltage SET InactiveReason = :InactiveReason, Inactive = 1 WHERE ChamberVoltageID = :TestID;");
	d_bind_text(":InactiveReason", test->InactiveReason);
	d_bind_int64(":TestID", test->ChamberVoltageID);
	d_step();
	d_finalize();
	test->Inactive = TRUE;
}
/**
 * \details Inactivates a record in the ChamberAccuracyTest table
 * \param test Pointer to CHAMBERACCURACYTEST structure, which contains the Primary Key, ChamberAccuracyTestID, and the InactiveReason text
 * \returns None
 */
void DB_InactivateAccuracyTest(CHAMBERACCURACYTEST *test){
	*capStatus = SQLITE_OK;
	d_prepare_v2("UPDATE ChamberAccuracyTest SET InactiveReason = :InactiveReason, Inactive = 1 WHERE ChamberAccuracyTestID = :TestID;");
	d_bind_text(":InactiveReason", test->InactiveReason);
	d_bind_int64(":TestID", test->ChamberAccuracyTestID);
	d_step();
	d_finalize();
	test->Inactive = TRUE;
}
/**
 * \details Test the ChamberZeroID field in the ChamberDailyTest table
 * \param TestID ChamberZeroID value to find
 * \returns True = Record found, False = No record found
 */
bool DB_CheckDailyTestForZeroID(long long int TestID){
	bool return_value;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT * FROM ChamberDailyTest WHERE ChamberZeroID = :TestID;");
	d_bind_int64(":TestID", TestID);
	d_step();
	if(*capStatus == SQLITE_ROW) return_value = TRUE;
	else return_value = FALSE;
	d_finalize();

	return return_value;
}
/**
 * \details Test the ChamberBackgroundID field in the ChamberDailyTest table
 * \param TestID ChamberBackgroundID value to find
 * \returns True = Record found, False = No record found
 */
bool DB_CheckDailyTestForBackgroundID(long long int TestID){
	bool return_value;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT * FROM ChamberDailyTest WHERE ChamberBackgroundID = :TestID;");
	d_bind_int64(":TestID", TestID);
	d_step();
	if(*capStatus == SQLITE_ROW) return_value = TRUE;
	else return_value = FALSE;
	d_finalize();

	return return_value;
}
/**
 * \details Test the ChamberVoltageID field in the ChamberDailyTest table
 * \param TestID ChamberVoltageID value to find
 * \returns True = Record found, False = No record found
 */
bool DB_CheckDailyTestForVoltageID(long long int TestID){
	bool return_value;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT * FROM ChamberDailyTest WHERE ChamberVoltageID = :TestID;");
	d_bind_int64(":TestID", TestID);
	d_step();
	if(*capStatus == SQLITE_ROW) return_value = TRUE;
	else return_value = FALSE;
	d_finalize();

	return return_value;
}
/**
 * \details Test the ChamberAccuracyTestID field in the ChamberDailyTest table
 * \param TestID ChamberAccuracyTestID value to find
 * \returns True = Record found, False = No record found
 */
bool DB_CheckDailyTestForAccuracyID(long long int TestID){
	bool return_value;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT * FROM ChamberDailyTest WHERE ChamberAccuracyTestID = :TestID;");
	d_bind_int64(":TestID", TestID);
	d_step();
	if(*capStatus == SQLITE_ROW) return_value = TRUE;
	else return_value = FALSE;
	d_finalize();

	return return_value;
}
/**
 * \details Erase all records in the Hotkey table
 * \returns None
 */
void DB_ClearHotkeyTable(void){
	*capStatus = SQLITE_OK;
	d_prepare_v2("DELETE FROM Hotkey;");
	d_step();
	d_finalize();
}
/**
 * \details Write Hotkey setting into the Hotkey table
 * \param ChamberType Chamber Type ID (0 = R Chamber, 1 = PET Chamber ...)
 * \param HotkeyMenu 0 = Primary Menu, 1 = Auxilary Menu
 * \param HotkeyIndex 0 to 7 for Primary Menu, 0 to 19 for Auxilary Menu
 * \param HotkeyNuclideIndex Nuclide index
 * \returns None
 */
void DB_WriteHotkey(int ChamberType, int HotkeyMenu, int HotkeyIndex, int HotkeyNuclideIndex){
	int hotkeyNuclideIndex;
	bool flgFound, flgDifferent, flgWrite;

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT HotkeyNuclideIndex FROM Hotkey WHERE ChamberType=:ChamberType AND HotkeyMenu=:HotkeyMenu AND HotkeyIndex=:HotkeyIndex;");
	d_bind_int(":ChamberType", ChamberType);
	d_bind_int(":HotkeyMenu", HotkeyMenu);
	d_bind_int(":HotkeyIndex", HotkeyIndex);
	d_step();
	if (*capStatus == SQLITE_ROW){
		hotkeyNuclideIndex = d_column_int(0);
		if(hotkeyNuclideIndex != HotkeyNuclideIndex) flgDifferent = TRUE;
		else flgDifferent = FALSE;
		flgFound = TRUE;
	}else flgFound = FALSE;
	d_finalize();

	flgWrite = FALSE;
	*capStatus = SQLITE_OK;
	if(flgFound){
		if(flgDifferent){
			d_prepare_v2("UPDATE Hotkey SET HotkeyNuclideIndex=:HotkeyNuclideIndex WHERE ChamberType=:ChamberType AND HotkeyMenu=:HotkeyMenu AND HotkeyIndex=:HotkeyIndex;");
			flgWrite = TRUE;
		}
	}else{
		d_prepare_v2("INSERT INTO Hotkey(ChamberType, HotkeyMenu, HotkeyIndex, HotkeyNuclideIndex) VALUES(:ChamberType, :HotkeyMenu, :HotkeyIndex, :HotkeyNuclideIndex);");
		flgWrite = TRUE;
	}

	if(flgWrite){
		d_bind_int(":ChamberType", ChamberType);
		d_bind_int(":HotkeyMenu", HotkeyMenu);
		d_bind_int(":HotkeyIndex", HotkeyIndex);
		d_bind_int(":HotkeyNuclideIndex", HotkeyNuclideIndex);
		d_step();
		d_finalize();
	}
}
/**
 * \details Create a new record in the Hotkey table
 * \param ChamberType Chamber Type ID (0 = R Chamber, 1 = PET Chamber ...)
 * \param HotkeyMenu 0 = Primary Menu, 1 = Auxilary Menu
 * \param HotkeyIndex 0 to 7 for Primary Menu, 0 to 19 for Auxilary Menu
 * \param HotkeyNuclideIndex Nuclide index
 * \returns None
 */
void DB_WriteHotkeyWithInsert(int ChamberType, int HotkeyMenu, int HotkeyIndex, int HotkeyNuclideIndex){
	*capStatus = SQLITE_OK;
	d_prepare_v2("INSERT INTO Hotkey(ChamberType, HotkeyMenu, HotkeyIndex, HotkeyNuclideIndex) VALUES(:ChamberType, :HotkeyMenu, :HotkeyIndex, :HotkeyNuclideIndex);");
	d_bind_int(":ChamberType", ChamberType);
	d_bind_int(":HotkeyMenu", HotkeyMenu);
	d_bind_int(":HotkeyIndex", HotkeyIndex);
	d_bind_int(":HotkeyNuclideIndex", HotkeyNuclideIndex);
	d_step();
	d_finalize();
}
/**
 * \details Read the contents of the Hotkey table into the m_ucHotKeyNuclideID and the m_ucHotKeyNuclideID2 tables
 * \returns None
 */
void DB_ReadAllHotkey(void){
	int i, j, chamberType, hotkeyMenu, hotkeyIndex, hotkeyNuclideIndex;

	for(i=0; i<UPPER_LIMIT_CHAMB; i++){
		for(j=0; j<8; j++){
			m_ucHotKeyNuclideID[i][j] = 255;
		}

		for(j=0; j<20; j++){
			m_ucHotKeyNuclideID2[i][j] = 255;
		}
	}

	*capStatus = SQLITE_OK;
	d_prepare_v2("SELECT ChamberType, HotkeyMenu, HotkeyIndex, HotkeyNuclideIndex FROM Hotkey;");
	d_step();
	while(*capStatus == SQLITE_ROW){
		chamberType = d_column_int(0);
		hotkeyMenu = d_column_int(1);
		hotkeyIndex = d_column_int(2);
		hotkeyNuclideIndex = d_column_int(3);
		d_step();

		if((chamberType >= R_CHAMB) && (chamberType < UPPER_LIMIT_CHAMB)){
			if(hotkeyMenu == 0){
				if((hotkeyIndex >= 0) && (hotkeyIndex < 8)){
					if((hotkeyNuclideIndex == 255) || ((hotkeyNuclideIndex >= 0) && (hotkeyNuclideIndex < (USERNUC + 10))) || ((hotkeyNuclideIndex >= USERNUC + 20) && (hotkeyNuclideIndex < ALLNUC))){
						m_ucHotKeyNuclideID[chamberType][hotkeyIndex] = hotkeyNuclideIndex;
					}
				}
			}else if(hotkeyMenu == 1){
				if((hotkeyIndex >= 0) && (hotkeyIndex < 20)){
					if((hotkeyNuclideIndex == 255) || ((hotkeyNuclideIndex >= 0) && (hotkeyNuclideIndex < (USERNUC + 10))) || ((hotkeyNuclideIndex >= USERNUC + 20) && (hotkeyNuclideIndex < ALLNUC))){
						m_ucHotKeyNuclideID2[chamberType][hotkeyIndex] = hotkeyNuclideIndex;
					}
				}
			}
		}
	}
	d_finalize();
}
/**
 * \details Create new records in the Hotkey table from the values in the m_ucHotKeyNuclideID and the m_ucHotKeyNuclideID2 tables
 * \returns None
 */
void DB_WriteAllHotkeyWithInsert(void){
	int i;

	d_command("BEGIN;");

	DB_ClearHotkeyTable();

	for(i=0; i<8; i++){
		DB_WriteHotkeyWithInsert(R_CHAMB, 0, i, m_ucHotKeyNuclideID[R_CHAMB][i]);
		DB_WriteHotkeyWithInsert(P_CHAMB, 0, i, m_ucHotKeyNuclideID[P_CHAMB][i]);
		DB_WriteHotkeyWithInsert(C_CHAMB, 0, i, m_ucHotKeyNuclideID[C_CHAMB][i]);
		DB_WriteHotkeyWithInsert(K_CHAMB, 0, i, m_ucHotKeyNuclideID[K_CHAMB][i]);
	}

	for(i=0; i<20; i++){
		DB_WriteHotkeyWithInsert(R_CHAMB, 1, i, m_ucHotKeyNuclideID2[R_CHAMB][i]);
		DB_WriteHotkeyWithInsert(P_CHAMB, 1, i, m_ucHotKeyNuclideID2[P_CHAMB][i]);
		DB_WriteHotkeyWithInsert(C_CHAMB, 1, i, m_ucHotKeyNuclideID2[C_CHAMB][i]);
		DB_WriteHotkeyWithInsert(K_CHAMB, 1, i, m_ucHotKeyNuclideID2[K_CHAMB][i]);
	}

	d_command("COMMIT;");
}
/**
 * \details Update records in the Hotkey table from the values in the m_ucHotKeyNuclideID and m_ucHotKeyNuclideID2 tables
 * \param ch_type Chamber Type ID (0 = R Chamber, 1 = PET Chamber ...) or 7FFFh will write all chamber types to the Hotkey table
 * \returns None
 */
void DB_WriteAllHotkey(int ch_type){
	int i;

	d_command("BEGIN;");

	for(i=0; i<8; i++){
		if(ch_type == 0x7fff){
			DB_WriteHotkey(R_CHAMB, 0, i, m_ucHotKeyNuclideID[R_CHAMB][i]);
			DB_WriteHotkey(P_CHAMB, 0, i, m_ucHotKeyNuclideID[P_CHAMB][i]);
			DB_WriteHotkey(C_CHAMB, 0, i, m_ucHotKeyNuclideID[C_CHAMB][i]);
			DB_WriteHotkey(K_CHAMB, 0, i, m_ucHotKeyNuclideID[K_CHAMB][i]);
		}else{
			DB_WriteHotkey(ch_type, 0, i, m_ucHotKeyNuclideID[ch_type][i]);
		}
	}

	for(i=0; i<20; i++){
		if(ch_type == 0x7fff){
			DB_WriteHotkey(R_CHAMB, 1, i, m_ucHotKeyNuclideID2[R_CHAMB][i]);
			DB_WriteHotkey(P_CHAMB, 1, i, m_ucHotKeyNuclideID2[P_CHAMB][i]);
			DB_WriteHotkey(C_CHAMB, 1, i, m_ucHotKeyNuclideID2[C_CHAMB][i]);
			DB_WriteHotkey(K_CHAMB, 1, i, m_ucHotKeyNuclideID2[K_CHAMB][i]);
		}else{
			DB_WriteHotkey(ch_type, 1, i, m_ucHotKeyNuclideID2[ch_type][i]);
		}
	}

	d_command("COMMIT;");
}
/**
 * \details Close and shutdown the database
 * \returns None
 */
void DB_shutdown(void){
	*capStatus = SQLITE_OK;
	db_close(capStatus, capDB);
	db_shutdown(capStatus);
}
/**
 * \details Fill the m_ucHotKeyNuclideID and m_ucHotKeyNuclideID2 tables with the default Hotkey values
 * \returns None
 */
void DB_PopulateDefaultHotkeys(void){
	m_ucHotKeyNuclideID[R_CHAMB][0] = NuclideData_getIndexFromName("F 18");
	m_ucHotKeyNuclideID[R_CHAMB][1] = NuclideData_getIndexFromName("Ga67");
	m_ucHotKeyNuclideID[R_CHAMB][2] = NuclideData_getIndexFromName("In111");
	m_ucHotKeyNuclideID[R_CHAMB][3] = NuclideData_getIndexFromName("Tc99m");
	m_ucHotKeyNuclideID[R_CHAMB][4] = NuclideData_getIndexFromName("I 123");
	m_ucHotKeyNuclideID[R_CHAMB][5] = NuclideData_getIndexFromName("I 131");
	m_ucHotKeyNuclideID[R_CHAMB][6] = NuclideData_getIndexFromName("Xe133");
	m_ucHotKeyNuclideID[R_CHAMB][7] = NuclideData_getIndexFromName("Tl201");

	m_ucHotKeyNuclideID2[R_CHAMB][0] = NuclideData_getIndexFromName("F 18");
	m_ucHotKeyNuclideID2[R_CHAMB][1] = NuclideData_getIndexFromName("Ga67");
	m_ucHotKeyNuclideID2[R_CHAMB][2] = NuclideData_getIndexFromName("In111");
	m_ucHotKeyNuclideID2[R_CHAMB][3] = NuclideData_getIndexFromName("Tc99m");
	m_ucHotKeyNuclideID2[R_CHAMB][4] = NuclideData_getIndexFromName("I 123");
	m_ucHotKeyNuclideID2[R_CHAMB][5] = NuclideData_getIndexFromName("I 131");
	m_ucHotKeyNuclideID2[R_CHAMB][6] = NuclideData_getIndexFromName("Xe133");
	m_ucHotKeyNuclideID2[R_CHAMB][7] = NuclideData_getIndexFromName("Tl201");
	m_ucHotKeyNuclideID2[R_CHAMB][8] = NuclideData_getIndexFromName("C 11");
	m_ucHotKeyNuclideID2[R_CHAMB][9] = NuclideData_getIndexFromName("N 13");
	m_ucHotKeyNuclideID2[R_CHAMB][10] = NuclideData_getIndexFromName("O 15");
	m_ucHotKeyNuclideID2[R_CHAMB][11] = NuclideData_getIndexFromName("Ba133");
	m_ucHotKeyNuclideID2[R_CHAMB][12] = NuclideData_getIndexFromName("Co57");
	m_ucHotKeyNuclideID2[R_CHAMB][13] = NuclideData_getIndexFromName("Co60");
	m_ucHotKeyNuclideID2[R_CHAMB][14] = NuclideData_getIndexFromName("Cs137");
	m_ucHotKeyNuclideID2[R_CHAMB][15] = 255;
	m_ucHotKeyNuclideID2[R_CHAMB][16] = 255;
	m_ucHotKeyNuclideID2[R_CHAMB][17] = 255;
	m_ucHotKeyNuclideID2[R_CHAMB][18] = 255;
	m_ucHotKeyNuclideID2[R_CHAMB][19] = 255;

	m_ucHotKeyNuclideID[P_CHAMB][0] = NuclideData_getIndexFromName("F 18");
	m_ucHotKeyNuclideID[P_CHAMB][1] = NuclideData_getIndexFromName("C 11");
	m_ucHotKeyNuclideID[P_CHAMB][2] = NuclideData_getIndexFromName("N 13");
	m_ucHotKeyNuclideID[P_CHAMB][3] = NuclideData_getIndexFromName("O 15");
	m_ucHotKeyNuclideID[P_CHAMB][4] = NuclideData_getIndexFromName("Tc99m");
	m_ucHotKeyNuclideID[P_CHAMB][5] = NuclideData_getIndexFromName("Cs137");
	m_ucHotKeyNuclideID[P_CHAMB][6] = NuclideData_getIndexFromName("Co60");
	m_ucHotKeyNuclideID[P_CHAMB][7] = NuclideData_getIndexFromName("I 131");

	m_ucHotKeyNuclideID2[P_CHAMB][0] = NuclideData_getIndexFromName("F 18");
	m_ucHotKeyNuclideID2[P_CHAMB][1] = NuclideData_getIndexFromName("C 11");
	m_ucHotKeyNuclideID2[P_CHAMB][2] = NuclideData_getIndexFromName("N 13");
	m_ucHotKeyNuclideID2[P_CHAMB][3] = NuclideData_getIndexFromName("O 15");
	m_ucHotKeyNuclideID2[P_CHAMB][4] = NuclideData_getIndexFromName("Ba133");
	m_ucHotKeyNuclideID2[P_CHAMB][5] = NuclideData_getIndexFromName("Co57");
	m_ucHotKeyNuclideID2[P_CHAMB][6] = NuclideData_getIndexFromName("Co60");
	m_ucHotKeyNuclideID2[P_CHAMB][7] = NuclideData_getIndexFromName("Cs137");
	m_ucHotKeyNuclideID2[P_CHAMB][8] = NuclideData_getIndexFromName("Ga67");
	m_ucHotKeyNuclideID2[P_CHAMB][9] = NuclideData_getIndexFromName("I 131");
	m_ucHotKeyNuclideID2[P_CHAMB][10] = NuclideData_getIndexFromName("Na22");
	m_ucHotKeyNuclideID2[P_CHAMB][11] = NuclideData_getIndexFromName("Tc99m");
	m_ucHotKeyNuclideID2[P_CHAMB][12] = NuclideData_getIndexFromName("Tl201");
	m_ucHotKeyNuclideID2[P_CHAMB][13] = 255;
	m_ucHotKeyNuclideID2[P_CHAMB][14] = 255;
	m_ucHotKeyNuclideID2[P_CHAMB][15] = 255;
	m_ucHotKeyNuclideID2[P_CHAMB][16] = 255;
	m_ucHotKeyNuclideID2[P_CHAMB][17] = 255;
	m_ucHotKeyNuclideID2[P_CHAMB][18] = 255;
	m_ucHotKeyNuclideID2[P_CHAMB][19] = 255;

	m_ucHotKeyNuclideID[B_CHAMB][0] = 255;
	m_ucHotKeyNuclideID[B_CHAMB][1] = 255;
	m_ucHotKeyNuclideID[B_CHAMB][2] = 255;
	m_ucHotKeyNuclideID[B_CHAMB][3] = 255;
	m_ucHotKeyNuclideID[B_CHAMB][4] = 255;
	m_ucHotKeyNuclideID[B_CHAMB][5] = 255;
	m_ucHotKeyNuclideID[B_CHAMB][6] = 255;
	m_ucHotKeyNuclideID[B_CHAMB][7] = 255;

	m_ucHotKeyNuclideID2[B_CHAMB][0] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][1] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][2] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][3] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][4] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][5] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][6] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][7] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][8] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][9] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][10] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][11] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][12] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][13] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][14] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][15] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][16] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][17] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][18] = 255;
	m_ucHotKeyNuclideID2[B_CHAMB][19] =255;

	m_ucHotKeyNuclideID[ONE_DOT_EIGHT_CHAMB][0] = 255;
	m_ucHotKeyNuclideID[ONE_DOT_EIGHT_CHAMB][1] = 255;
	m_ucHotKeyNuclideID[ONE_DOT_EIGHT_CHAMB][2] = 255;
	m_ucHotKeyNuclideID[ONE_DOT_EIGHT_CHAMB][3] = 255;
	m_ucHotKeyNuclideID[ONE_DOT_EIGHT_CHAMB][4] = 255;
	m_ucHotKeyNuclideID[ONE_DOT_EIGHT_CHAMB][5] = 255;
	m_ucHotKeyNuclideID[ONE_DOT_EIGHT_CHAMB][6] = 255;
	m_ucHotKeyNuclideID[ONE_DOT_EIGHT_CHAMB][7] = 255;

	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][0] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][1] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][2] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][3] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][4] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][5] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][6] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][7] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][8] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][9] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][10] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][11] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][12] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][13] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][14] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][15] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][16] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][17] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][18] = 255;
	m_ucHotKeyNuclideID2[ONE_DOT_EIGHT_CHAMB][19] = 255;

	m_ucHotKeyNuclideID[C_CHAMB][0] = 255;
	m_ucHotKeyNuclideID[C_CHAMB][1] = 255;
	m_ucHotKeyNuclideID[C_CHAMB][2] = 255;
	m_ucHotKeyNuclideID[C_CHAMB][3] = 255;
	m_ucHotKeyNuclideID[C_CHAMB][4] = 255;
	m_ucHotKeyNuclideID[C_CHAMB][5] = 255;
	m_ucHotKeyNuclideID[C_CHAMB][6] = 255;
	m_ucHotKeyNuclideID[C_CHAMB][7] = 255;

	m_ucHotKeyNuclideID2[C_CHAMB][0] = NuclideData_getIndexFromName("Cs137");
	m_ucHotKeyNuclideID2[C_CHAMB][1] = NuclideData_getIndexFromName("Co57");
	m_ucHotKeyNuclideID2[C_CHAMB][2] = NuclideData_getIndexFromName("Co60");
	m_ucHotKeyNuclideID2[C_CHAMB][3] = NuclideData_getIndexFromName("Ba133");
	m_ucHotKeyNuclideID2[C_CHAMB][4] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][5] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][6] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][7] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][8] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][9] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][10] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][11] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][12] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][13] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][14] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][15] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][16] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][17] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][18] = 255;
	m_ucHotKeyNuclideID2[C_CHAMB][19] = 255;

	m_ucHotKeyNuclideID[K_CHAMB][0] = NuclideData_getIndexFromName("Cs137");
	m_ucHotKeyNuclideID[K_CHAMB][1] = NuclideData_getIndexFromName("Co57");
	m_ucHotKeyNuclideID[K_CHAMB][2] = NuclideData_getIndexFromName("Co60");
	m_ucHotKeyNuclideID[K_CHAMB][3] = NuclideData_getIndexFromName("Ba133");
	m_ucHotKeyNuclideID[K_CHAMB][4] = 255;
	m_ucHotKeyNuclideID[K_CHAMB][5] = 255;
	m_ucHotKeyNuclideID[K_CHAMB][6] = 255;
	m_ucHotKeyNuclideID[K_CHAMB][7] = 255;

	m_ucHotKeyNuclideID2[K_CHAMB][0] = NuclideData_getIndexFromName("Cs137");
	m_ucHotKeyNuclideID2[K_CHAMB][1] = NuclideData_getIndexFromName("Co57");
	m_ucHotKeyNuclideID2[K_CHAMB][2] = NuclideData_getIndexFromName("Co60");
	m_ucHotKeyNuclideID2[K_CHAMB][3] = NuclideData_getIndexFromName("Ba133");
	m_ucHotKeyNuclideID2[K_CHAMB][4] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][5] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][6] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][7] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][8] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][9] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][10] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][11] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][12] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][13] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][14] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][15] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][16] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][17] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][18] = 255;
	m_ucHotKeyNuclideID2[K_CHAMB][19] = 255;
}

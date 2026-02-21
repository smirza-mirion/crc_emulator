/*
 * hal_stubs.c - Stub implementations for excluded firmware symbols
 *
 * Provides global variables from Globals_25.c and stub functions from
 * CalibratorMain_R.c, Screen.c, ScreenFonts_25.c, USB/*.c, Ethernet.c
 * that are referenced by the firmware but not needed in the emulator.
 */

#include "crc.h"
#include "coldfire.h"
#include "ff.h"
#include "daily.h"
#include "linearity.h"
#include "counter.h"

/* FONT struct from screen.h - included directly to avoid function
 * prototype conflicts between screen.h and our stubs. */
typedef struct font FONT;
struct font
{
    short x;
    short y;
    short nbytes;
    short delx;
    short dely;
    char  keep[7];
    unsigned char rev_mask;
};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

/* ================================================================
 * Globals from Globals_25.c
 * ================================================================ */

/* Unit conversion factors for activity */
const float unitfact[6] = {1.e+6, 1.e+3, 1.0, 1.e-3, 1.e-6, 1.e-9};

/* Upper autorange thresholds in Curie */
const float upci[9] = {19.99e-6, 199.9e-6, 999.e-6,
                        1.999e-3, 19.99e-3, 199.9e-3,
                        999.e-3, 1.999, 8.0};
const float downci[9] = {14.0e-6, 140.e-6, .6e-3, 1.4e-3,
                          14.e-3, 140.e-3, .6, 1.4, 8.0};

/* Upper autorange thresholds in Becquerel */
const float upbq[8] = {1.999e+6, 19.99e+6, 199.9e+6,
                        1999e+6, 19.99e+9, 199.9e+9, 37000.e+9, 0};
const float downbq[8] = {1.40e+6, 14.0e+6, 140e+6, 1.4e+9,
                          14.0e+9, 140.e+9, 37000.e+9, 0};

/* CRC-77t autorange thresholds */
const float upci77t[8] = {1.999e-3, 19.99e-3, 199.9e-3, 999.0e-3,
                           1.999e+0, 19.99e+0, 199.9e+0, 1000.e+0};
const float downci77t[8] = {1.400e-3, 14.00e-3, 140.0e-3, 600.0e-3,
                             1.400e+0, 14.00e+0, 140.0e+0, 1000.e+0};

/* Days per month (index 0 unused) */
const short month_days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* Decimal places for display by range */
const short ndecbq[] = {3, 2, 1, 0, 2, 1, 0, 0, 0, 0};
const short ndecci[] = {2, 1, 0, 3, 2, 1, 0, 3, 2, 0};
const short ndec77tci[] = {2, 2, 1, 0, 3, 2, 1, 0};

/* EEPROM initialization strings - padded to match EE_WRITE field sizes.
 * init_str[10] gets cap_str, init_str1[8] gets mir_str. */
const char cap_str[10] = "Capintec";
const char mir_str[8] = "Mirion";
const char rev_num_str1_0[8] = "Rev1.0";
const char rev_num_str1_1[] = "Rev1.1";

/* Unit display strings */
const char *unit_str[] = {"$Ci", "mCi", " Ci", "kBq", "MBq", "GBq"};

/* Time unit strings */
const char *timeunit[] = {"M", "H", "D", "Y"};

/* Tone duration */
short tone_duration = 0;

/* Standard linearity measurements */
STANDLIN standlin[8][12];

/* Number of daily sources */
short num_daily;

/* ADC wait time */
short adc_wait_time;

/* Clock times */
time_t clock_time;
time_t prev_clock_time;

/* User calibration numbers */
USERCAL user_cal[MAX_NEW_CAL];

/* Container correction factors */
CALCORR calcorr[MAX_CAL_COR];

/* Well detector test data */
C_TEST c_test;

/* Daily test results per chamber */
DAILY_TEST_RES daily_res[8];

/* Release and start flags */
volatile unsigned char g_ucRelease = 0;
volatile unsigned char g_ucStartFlag = 0;

/* Today's clock (from KeyInput_25.c) */
time_t today_clock;

/* ================================================================
 * Globals from CalibratorMain_R.c
 * ================================================================ */

bool CalibratorMain_WelcomeScreen = TRUE;

/* Container correction factors - it's a function, not a variable */
void container_correction_factors(void)
{
    /* In the real firmware, reads correction factors from EEPROM.
     * In the emulator, use defaults (all 1.0). */
}

/* ================================================================
 * Globals from ChipSelectIO_RP.c
 * ================================================================ */

/* Chip select pointer - not used in emulator */
volatile unsigned char *pCS1 = NULL;

/* ================================================================
 * Globals from USB/sl811s.c
 * ================================================================ */

unsigned char clear_to_send1 = 0;
unsigned char InPacketCircularBuffer[4096];
unsigned short InPacketCircularHead = 0;
unsigned short InPacketCircularTail = 0;
unsigned char OutPacketCircularBuffer[4096];
unsigned short OutPacketCircularHead = 0;
unsigned short OutPacketCircularTail = 0;

/* ================================================================
 * Globals from USB/sl811h_101508.c
 * ================================================================ */

unsigned char m_ucEnumerated = 0;

/* ================================================================
 * Globals from Ethernet.c
 * ================================================================ */

short ethernet_connection_status = 0;
short ethernet_echo = 0;
short ethernet_echo_status = 0;

/* ================================================================
 * Globals for smu (system measurement unit) - not found, stub
 * ================================================================ */

short smu = 0;

/* ================================================================
 * Globals from Qspi_25.c
 * ================================================================ */

short adc_pinset = 0;

/* ================================================================
 * Functions from CalibratorMain_R.c
 * ================================================================ */

/* Amulet HTML page index */
extern unsigned char AmuletHTMLIndex[];

void LoadAmuletHTML(int board)
{
    /* In the emulator, HTML page mappings are handled by the frontend.
     * Just set some reasonable defaults.
     * AmuletHTMLIndex is only AMULET_HTML_ARRAY_SIZE (0xA8 = 168) bytes!
     * Writing beyond that corrupts adjacent globals. */
    (void)board;
    int i;
    for (i = 0; i < 0xA8; i++) {
        AmuletHTMLIndex[i] = (unsigned char)i;
    }
}

/* Study type strings */
static char study_types[8][20];

void set_study_type(void)
{
    strcpy(study_types[0], "Bone");
    strcpy(study_types[1], "Lung");
    strcpy(study_types[2], "Hida");
    strcpy(study_types[3], "Heart");
    strcpy(study_types[4], "Renal");
    strcpy(study_types[5], "Liver");
    strcpy(study_types[6], "Brain");
    strcpy(study_types[7], "Lymph");
}

void get_study_type(short istudy, char *study)
{
    if (istudy >= 0 && istudy < 8)
        strcpy(study, study_types[istudy]);
    else
        strcpy(study, "");
}

/* Calicheck tube names */
static char calicheck_tubes[12][30];

void set_calicheck_tube(void)
{
    strcpy(calicheck_tubes[0], "None");
    strcpy(calicheck_tubes[1], "Orange/Green");
    strcpy(calicheck_tubes[2], "Blue/Green");
    strcpy(calicheck_tubes[3], "Orange/Red");
    strcpy(calicheck_tubes[4], "Blue/Red");
    strcpy(calicheck_tubes[5], "Red/Green");
    strcpy(calicheck_tubes[6], "Blue/Orange");
    strcpy(calicheck_tubes[7], "Green/Yellow");
    strcpy(calicheck_tubes[8], "Orange/Yellow");
    strcpy(calicheck_tubes[9], "Red/Yellow");
    strcpy(calicheck_tubes[10], "Blue/Yellow");
    strcpy(calicheck_tubes[11], "Purple/Yellow");
}

void get_calicheck_tube(short itube, char *tube)
{
    if (itube >= 0 && itube < 12)
        strcpy(tube, calicheck_tubes[itube]);
    else
        strcpy(tube, "");
}

/* Time unit strings */
static char time_units[5][10];

void set_time_unit(void)
{
    strcpy(time_units[0], "M");
    strcpy(time_units[1], "H");
    strcpy(time_units[2], "D");
    strcpy(time_units[3], "Y");
    strcpy(time_units[4], "MY");
}

void get_time_unit(short itime, char *unit)
{
    if (itime >= 0 && itime < 5)
        strcpy(unit, time_units[itime]);
    else
        strcpy(unit, "");
}

short get_crc_mode(void)
{
    return CRC_CAL; /* Always calibrator mode */
}

short get_counter_user_key(char ukey)
{
    (void)ukey;
    return 0;
}

void set_options(void)
{
    /* In the real firmware, reads options from SD card files.
     * In the emulator, use defaults. Enable demo mode so that
     * get_measurements() skips hardware ADC reads. */
    extern CURRENT current;
    current.demo_mode = TRUE;
}

void smart_linearity(void)
{
    /* In the real firmware, swaps linearity data in EEPROM.
     * In the emulator, nothing to swap. */
}

ushort calc_chamber_checksum(short ch_num)
{
    (void)ch_num;
    return 0; /* Always valid in emulator */
}

void verify_chamber_checksum(void)
{
    /* All checksums valid in emulator */
}

void setup_short_watchdog(void)
{
    /* No watchdog in emulator */
}

void read_hotkey_txt(void)
{
    /* No hotkey file needed in emulator */
}

void ConvertLongLongIntToAscii(long long int value, char *output)
{
    sprintf(output, "%lld", value);
}

long long int ConvertStringToKey(char *key)
{
    if (!key || !*key) return -999;
    char clean[64];
    int j = 0;
    for (int i = 0; key[i] && j < 63; i++) {
        if (key[i] != '-') clean[j++] = key[i];
    }
    clean[j] = 0;
    return atoll(clean);
}

void GenerateDeviceID(char *deviceID)
{
    strcpy(deviceID, "EMULATOR-0000");
}

void SetKeyFeatures(void)
{
    /* Enable all features in emulator */
    extern CURRENT current;
    current.pccomm_enabled = TRUE;
}

int VerifyKeyString(char *key)
{
    (void)key;
    return 0; /* Always valid */
}

void MoveFileName(char *fname, char *lfname)
{
    if (lfname[0] == 0 && fname[0] != 0) {
        strcpy(lfname, fname);
    }
}

/* ================================================================
 * Functions from Screen.c (LCD display - no-ops in emulator)
 * ================================================================ */

/* Screen memory buffer (the LCD framebuffer) */
static unsigned short screen_mem[800 * 600];

void draw_horiz_line(short x1, short y1, short x2, short color)
{
    (void)x1; (void)y1; (void)x2; (void)color;
}

void draw_vert_line(short x1, short y1, short y2, short color)
{
    (void)x1; (void)y1; (void)y2; (void)color;
}

void erase_box(short x1, short y1, short x2, short y2)
{
    (void)x1; (void)y1; (void)x2; (void)y2;
}

void erase_lines(short y1, short y2)
{
    (void)y1; (void)y2;
}

unsigned short *get_screen_address(short x, short y)
{
    if (x < 0 || x >= 800 || y < 0 || y >= 600) return &screen_mem[0];
    return &screen_mem[y * 800 + x];
}

unsigned short *get_scrmem(void)
{
    return screen_mem;
}

void read_screen(short y1, short y2, unsigned short *buf)
{
    (void)y1; (void)y2; (void)buf;
}

unsigned char read_screen_byte(short x, short y)
{
    (void)x; (void)y;
    return 0;
}

void screen_print(short x, short y, char *str, short color, short size)
{
    (void)x; (void)y; (void)str; (void)color; (void)size;
}

void write_screen(short y1, short y2, unsigned short *buf)
{
    (void)y1; (void)y2; (void)buf;
}

void write_screen_data(short x, short y, unsigned short data)
{
    (void)x; (void)y; (void)data;
}

/* ================================================================
 * Functions from ScreenFonts_25.c
 * ================================================================ */

/* Font table - matches ScreenFonts_25.c definition.
 * The emulator uses the Amulet display (not LCD), but some firmware code
 * (DisplayText.c, Initialization_R.c) accesses fonts[] during init. */
const FONT fonts[] = {
    {5,7,1,6,8,{0,0,0,0,0,0,0},0xf8},     /* SMALL */
    {7,9,1,8,9,{0,0,0,0,0,0,0},0xfe},      /* MEDIUM */
    {8,13,1,10,13,{0,0,0,0,0,0,0},0xff},    /* BIG */
    {13,20,2,16,20,{0,0x7,0,0,0,0,0},0xff}, /* VERY BIG */
    {15,20,2,16,20,{0,0x1,0,0,0,0,0},0xff}, /* LOGO */
};

unsigned char *get_ch_array(short font, unsigned char ch)
{
    (void)font; (void)ch;
    static unsigned char empty_char[32] = {0};
    return empty_char;
}

/* ================================================================
 * Functions from USB/sl811s.c and USB/sl811h_101508.c
 * ================================================================ */

short is_usb_char_waiting(void)
{
    return 0; /* No USB data */
}

short USB_Get_Data(unsigned char *buf, short maxlen)
{
    (void)buf; (void)maxlen;
    return 0;
}

short USB_Send_Data(unsigned char *buf, short len)
{
    (void)buf; (void)len;
    return 0;
}

short USB_Send_Data_LowLevel_num(unsigned char *buf, short len)
{
    (void)buf; (void)len;
    return 0;
}

long TransposeLongInt(long val)
{
    /* Byte-swap for big-endian/little-endian conversion */
    return ((val & 0xFF000000) >> 24) |
           ((val & 0x00FF0000) >> 8)  |
           ((val & 0x0000FF00) << 8)  |
           ((val & 0x000000FF) << 24);
}

/* ================================================================
 * Functions from Ethernet.c
 * ================================================================ */

void ethernet_init(void)
{
    /* No Ethernet in emulator */
}

void ethernet_reset(void)
{
    /* No Ethernet in emulator */
}

void ethernet_send_ping(void)
{
    /* No Ethernet in emulator */
}

/* ================================================================
 * FatFS functions not in hal_fatfs.c
 * ================================================================ */

/* Track the FatFS-style current working directory path.
 * The firmware's sqlite3.c capintecFullPathname() uses f_getcwd() to build
 * full paths, and it requires strlen > 3 (more than just "0:/") to properly
 * initialize its path buffer. */
static char fatfs_cwd[512] = "0:/";

FRESULT f_chdir(const TCHAR *path)
{
    /* Actually change the process CWD so that SQLite (which uses POSIX I/O)
     * resolves relative paths consistently with FatFS.
     * Strip FatFS drive prefix and leading slashes, same as map_path. */
    const char *p = (const char *)path;
    char mapped[512];

    if (p[0] >= '0' && p[0] <= '9' && p[1] == ':') {
        p += 2;
        if (*p == '/' || *p == '\\') p++;
    }
    while (*p == '/' || *p == '\\') p++;

    snprintf(mapped, sizeof(mapped), "%s", *p ? p : ".");

    /* Ensure directory exists before chdir */
    mkdir(mapped, 0755);

    if (chdir(mapped) == 0) {
        /* Update the FatFS-style CWD tracker.
         * If the path starts with "/" it's absolute (relative to SD root).
         * Otherwise it's relative to the current FatFS CWD. */
        if (((const char *)path)[0] == '/' || ((const char *)path)[0] == '\\' ||
            (((const char *)path)[0] >= '0' && ((const char *)path)[0] <= '9' &&
             ((const char *)path)[1] == ':')) {
            /* Absolute path - rebuild from root */
            snprintf(fatfs_cwd, sizeof(fatfs_cwd), "0:/%s", *mapped == '.' ? "" : mapped);
        } else {
            /* Relative path - append to current */
            if (strcmp(mapped, "..") == 0) {
                /* Go up one level */
                char *last_slash = strrchr(fatfs_cwd, '/');
                if (last_slash && last_slash > fatfs_cwd + 2) /* Don't go above "0:/" */
                    *last_slash = '\0';
            } else {
                size_t len = strlen(fatfs_cwd);
                if (len > 0 && fatfs_cwd[len-1] != '/')
                    strncat(fatfs_cwd, "/", sizeof(fatfs_cwd) - len - 1);
                strncat(fatfs_cwd, mapped, sizeof(fatfs_cwd) - strlen(fatfs_cwd) - 1);
            }
        }
        return FR_OK;
    }

    return FR_NO_PATH;
}

FRESULT f_chdrive(BYTE drv)
{
    (void)drv;
    return FR_OK;
}

FRESULT f_getcwd(TCHAR *buff, UINT len)
{
    if (buff && len > 0) {
        strncpy(buff, fatfs_cwd, len);
        buff[len - 1] = '\0';
    }
    return FR_OK;
}

/* ================================================================
 * Compiler intrinsics / library functions
 * ================================================================ */

/* __DI and __EI are GHS compiler intrinsics for disable/enable interrupts.
 * They're referenced by some firmware files that don't include compat.h.
 * Provide function versions as fallback. */
void __DI(void) { /* no-op */ }
void __EI(void) { /* no-op */ }

/* sdiv - signed division helper from GHS runtime library */
long sdiv(long num, long den)
{
    if (den == 0) return 0;
    return num / den;
}

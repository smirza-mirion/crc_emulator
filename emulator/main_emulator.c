/*
 * main_emulator.c - CRC-25R Emulator Entry Point
 *
 * Replaces CalibratorMain_R.c:main(). Initializes the HAL layer,
 * WebSocket bridge, and simulation engine, then runs the original
 * firmware initialization sequence and main service loop.
 *
 * The key insight is that most of the firmware's init functions
 * either work as-is (SQLite, string tables, nuclide data) or
 * are replaced by HAL stubs (hardware init). The main service
 * loop (service_amulet → amulet_menu) is entirely software and
 * works unchanged.
 */
#include "compat.h"
#include "crc.h"
#include "Amulet.h"
#include "coldfire.h"
#include "hal.h"
#include "platform.h"
#include "sim_chamber.h"
#include "amulet_bridge.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

/* ---- GPIO/USB Stubs (from HAL) ---- */
extern void initSetUSB(void);
extern void set_touch_reset(bool on);
extern void power_mmc(bool on);

/* ---- Firmware Functions (extern) ---- */

/* From CalibratorMain_R.c (now provided by HAL stubs) */
extern void initSetExceptions(void);
extern void initDisableInterrupts(void);
extern void initSetCS(void);
extern void initSetGPIO(void);
extern void initSetI2C(void);
extern void initSetQSPI(void);
extern void initSetEPORT(void);
extern void initSetInterrupt(void);
extern void initialize_edge_port(void);

/* From CalibratorMain_R.c - these work as-is */
extern void ClearLowLevel(void);
extern void Sysset_clearStruct(void);
extern void AmuletChamberHalflifeCalc_clearTest(void);
extern void first_initialization(void);
extern void NuclideData_initializeMirror(void);
extern void VolumeMirror(void);
extern void BrightnessMirror(void);
extern void SleepBrightnessMirror(void);
extern void SleepTimeoutMirror(void);
extern void AmuletWellMainScreenMenu_wellStandardMirror(void);
extern void set_options(void);
extern void id_init(void);
extern void smart_linearity(void);
extern void set_study_type(void);
extern void set_calicheck_tube(void);
extern void set_time_unit(void);
extern void AmuletAutoLinearityTest_clearTest(void);

/* Database */
extern void initDB(bool flag);
extern void DB_ReadAllHotkey(void);
extern void WipesInitialize(void);
extern void BioAssayInitialize(void);
extern void ThyroidUptakeInitialize(void);
extern void RBCSurvivalInitialize(void);

/* Amulet HTML page index */
extern unsigned char AmuletHTMLIndex[];
extern void LoadAmuletHTML(int board);
/* AMULET_HTML_ARRAY_SIZE is defined in Headers/Amulet.h as 0xA8 (168) */

/* Chamber and measurement */
extern CHAMBER chamber[];
extern MEASUREMENT measurement[];
extern CURRENT current;
extern time_t clock_time, prev_clock_time;
extern time_t low_clock_time;
extern short max_chambers;

/* ADC control */
extern void set_adc_enabled(bool on);

/* Timer */
extern void reset_minute_counter_with_seconds(void);

/* UART */
extern void init_uart(int BaudRate, int iu);

/* RTC */
extern short read_clock(time_t *nowtime);
extern bool setup_rtc(void);
extern void set_today_clock(void);

/* MCA */
extern void Mca_setup_demo(void);
extern short Mca_installedDetector;
#define DET_EMPTY 0

/* USB / Printer */
extern void printer_init(void);

/* Watchdog */
extern void setup_watchdog(void);

/* Screen */
extern void initialize_screen(void);
extern void erase_screen(void);
extern void display_text(short x, short y, char *text, short color, short size, short mode);
#define MEDIUM 1
#define NORMAL 0

/* Main measurement loop */
extern void measurement_screen(void);

/* service_amulet for the main loop */
extern void service_amulet(void);

/* EEPROM read macro - firmware uses EE_READ in main */
/* Defined in eeprom.h or similar header */
extern bool eeread(unsigned char *data, unsigned short offset, int nbytes, int iEEType);

/* Global flags from firmware */
extern bool CalibratorMain_WelcomeScreen;
extern bool AmuletGenericYesNo_AccuracyTestPending;
extern char AmuletInitialRecoveryMsg[];
extern int AmuletWellAutoCalibrate_exportSequence;

/* ---- Emulator Configuration ---- */

static int ws_port = 9876;
static int scenario = SCENARIO_COLD_START;
static volatile int running = 1;

/* ---- Signal Handler ---- */

static void signal_handler(int sig)
{
    (void)sig;
    printf("\n[Emulator] Shutting down...\n");
    running = 0;
}

/* ---- Usage ---- */

static void print_usage(const char *progname)
{
    printf("CRC-25R Emulator\n");
    printf("Usage: %s [options]\n", progname);
    printf("Options:\n");
    printf("  --ws-port=PORT   WebSocket server port (default: 9876)\n");
    printf("  --scenario=N     Load preset scenario (0-9, default: 0)\n");
    printf("  --data-dir=DIR   Data directory (default: ./data)\n");
    printf("  --help           Show this help\n");
    printf("\nScenarios:\n");
    printf("  0: Cold Start (default)\n");
    printf("  1: Daily QC with Co-57\n");
    printf("  2: Tc-99m Measurement (15 mCi)\n");
    printf("  3: Multi-Chamber (Tc99m + F18)\n");
    printf("  4: MCA Well Detector (Cs137)\n");
    printf("  5: Accuracy Test (5 sources)\n");
    printf("  6: Linearity Test (200 mCi Tc99m)\n");
    printf("  7: Moly Assay (Mo99/Tc99m)\n");
    printf("  8: Background Only\n");
    printf("  9: Error Conditions\n");
}

/* ---- Parse Command Line ---- */

static void parse_args(int argc, char *argv[])
{
    int i;
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--ws-port=", 10) == 0) {
            ws_port = atoi(argv[i] + 10);
        } else if (strncmp(argv[i], "--scenario=", 11) == 0) {
            scenario = atoi(argv[i] + 11);
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(0);
        }
    }
}

/* ---- Static init functions that were in CalibratorMain_R.c ---- */

static void initAmuletHTML(void)
{
    int i;
    for (i = 0; i < AMULET_HTML_ARRAY_SIZE; i++) {
        AmuletHTMLIndex[i] = 0xFF;
    }
}

static void initSetSDCard(void)
{
    /* In the emulator, FatFS is backed by POSIX files.
     * The HAL's f_mount() creates the data/ directory. */
    extern unsigned char f_mount(unsigned char drive, void *fs);
    void *fs = malloc(1024); /* FATFS struct - sizeof(FATFS) is ~616 bytes */
    f_mount(0, fs);
}

static void initSetTimer(void)
{
    /* Start the platform timer thread (replaces PIT hardware init) */
    platform_timer_start();
}

static void initSetUART(int printerBaud, int pccommBaud, int amuletBaud)
{
    init_uart(printerBaud, 0);  /* Printer */
    init_uart(pccommBaud, 1);   /* PC comm */
    /* Amulet UART is handled by the bridge, not traditional UART */
    (void)amuletBaud;
}

/* Forward declarations for chamber init */
static void initialize_chambers(void);
static void setup_chambers(void);
static void initialize_remote(void);

/* ---- Chamber Initialization ---- */

/* These functions set up the chamber[] and measurement[] arrays
 * based on the emulated EEPROM and simulation state. */

static void initialize_chambers(void)
{
    SimState *sim = sim_get_state();
    short i;
    short limit = max_chambers < SIM_MAX_CHANNELS ? max_chambers : SIM_MAX_CHANNELS;

    current.num_chambers = 0;

    for (i = 0; i < limit; i++) {
        if (sim->sources[i].enabled) {
            chamber[i].exists = TRUE;
            chamber[i].connected_flag = !sim->sources[i].disconnected;
            chamber[i].active = TRUE;
            chamber[i].type = sim->sources[i].container_type;
            chamber[i].adc_zero = 0;
            chamber[i].bkg = 0.0f;
            chamber[i].zero = 0.0f;
            chamber[i].nuc_index = sim->sources[i].nuclide_index;

            measurement[i].valid_flag = FALSE;
            measurement[i].display_flag = FALSE;
            measurement[i].over_flag = FALSE;
            measurement[i].adc_value = 0;
            measurement[i].gain = 0;
            measurement[i].mode = 0; /* MEASMODE */
            measurement[i].conv_type = 0;

            current.num_chambers++;
        } else {
            chamber[i].exists = FALSE;
            chamber[i].connected_flag = FALSE;
            chamber[i].active = FALSE;
        }
    }

    if (current.num_chambers > 0) {
        current.main_chamber = 0;
    }
}

static void setup_chambers(void)
{
    /* In the real firmware, this reads chamber EEPROM for gain factors.
     * In the emulator, gain factors come from the simulation config. */
    short i;
    for (i = 0; i < max_chambers; i++) {
        if (chamber[i].exists) {
            measurement[i].gain = 0;
            measurement[i].nmeas = 0;
        }
    }
}

static void initialize_remote(void)
{
    /* No remote displays in the emulator */
}

/* ---- Main Entry Point ---- */

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL); /* Disable stdout buffering for debug output */
    printf("=== CRC-25R Calibrator Emulator ===\n");
    printf("Firmware version: 4.07c (French)\n\n");

    /* Parse command line arguments */
    parse_args(argc, argv);

    /* Install signal handler for clean shutdown */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* ---- Phase 1: Platform Setup ---- */
    printf("[Init] Creating data directory...\n");
    platform_ensure_data_dir();

    /* Change CWD into "data/" so both FatFS (via map_path) and SQLite
     * (which uses POSIX I/O directly) resolve relative paths consistently.
     * "data/" represents the SD card root. The firmware's f_chdir("/data")
     * will then chdir into "data" (a subdirectory within the SD card root),
     * and SQLite opens like "capintec.db" will land in the correct place. */
    if (chdir("data") != 0) {
        perror("[Init] FATAL: Cannot chdir to data/");
        return 1;
    }

    /* Record the SD card root path for absolute FatFS path resolution.
     * Must be called after chdir("data") but before any f_chdir() calls. */
    {
        extern void hal_fatfs_set_sd_root(void);
        hal_fatfs_set_sd_root();
    }

    /* ---- Phase 2: HAL Initialization ---- */
    printf("[Init] Initializing Hardware Abstraction Layer...\n");
    hal_init();

    /* ---- Phase 3: Simulation Engine ---- */
    printf("[Init] Initializing simulation engine (scenario %d)...\n", scenario);
    sim_init();
    sim_load_scenario(scenario);

    /* ---- Phase 4: WebSocket Bridge ---- */
    printf("[Init] Starting WebSocket server on port %d...\n", ws_port);
    amulet_bridge_init(ws_port);

    /* ---- Phase 5: Firmware Initialization Sequence ---- */
    /* This follows CalibratorMain_R.c:main() lines 206-384 */

    printf("[Init] Running firmware initialization...\n");

    CalibratorMain_WelcomeScreen = TRUE;
    AmuletGenericYesNo_AccuracyTestPending = FALSE;
    AmuletInitialRecoveryMsg[0] = 0;

    /* 1. Clear Global Data Structures */
    initSetExceptions();
    ClearLowLevel();
    Sysset_clearStruct();
    AmuletChamberHalflifeCalc_clearTest();

    /* 2. Disable All Interrupts (no-op in emulator) */
    initDisableInterrupts();

    /* 3. Set Chip Select (no-op) */
    initSetCS();
    set_touch_reset(FALSE);
    power_mmc(TRUE);

    /* 4. Set GPIO (no-op) */
    initSetGPIO();

    /* 5. Set I2C & read RTC */
    printf("[Init] Initializing I2C and RTC...\n");
    initSetI2C();
    read_clock(&clock_time);
    low_clock_time = clock_time;
    reset_minute_counter_with_seconds();

    /* 6. Set QSPI (no-op) */
    initSetQSPI();

    /* 7. Initialize SD Card (maps to POSIX filesystem) */
    printf("[Init] Mounting filesystem...\n");
    initSetSDCard();

    /* 8. Initialize Amulet HTML page index */
    initAmuletHTML();

    /* 9. Initialize Database (SQLite - works as-is) */
    printf("[Init] Initializing SQLite database...\n");
    initDB(FALSE);
    DB_ReadAllHotkey();  /* Populate hotkey nuclide IDs from database */
    WipesInitialize();
    BioAssayInitialize();
    ThyroidUptakeInitialize();
    RBCSurvivalInitialize();

    /* 10. Read EEPROM for global data structures */
    printf("[Init] Reading EEPROM (first initialization)...\n");
    first_initialization();
    NuclideData_initializeMirror();
    VolumeMirror();
    BrightnessMirror();
    SleepBrightnessMirror();
    SleepTimeoutMirror();
    AmuletWellMainScreenMenu_wellStandardMirror();

    /* 11. Read printer setting from EEPROM */
    /* EE_READ(print[0],(uchar *)&current.printer); */
    /* Skip - printer not needed in emulator */

    /* 12. Read options from SD card */
    set_options();
    id_init();

    /* 13. Initialize chambers from simulation config */
    printf("[Init] Setting up chambers...\n");
    initialize_chambers();
    setup_chambers();
    smart_linearity();
    initialize_remote();

    printf("[Init] num_chambers = %d, main_chamber = %d\n",
           current.num_chambers, current.main_chamber);

    if (current.num_chambers != 0)
        set_adc_enabled(TRUE);
    else
        set_adc_enabled(FALSE);

    /* 14. Start timer (PIT → OS timer thread) */
    printf("[Init] Starting timer...\n");
    initSetTimer();

    /* 15. Set UART */
    initSetUART(4800, 9600, 115200);

    /* 16. Set EPORT (no-op) */
    initSetEPORT();

    /* 17. Setup interrupts (no-op) */
    initSetInterrupt();

    /* 18. Setup RTC */
    if (!setup_rtc()) {
        printf("[WARNING] RTC setup failed\n");
    }
    read_clock(&clock_time);
    low_clock_time = clock_time;
    set_today_clock();
    prev_clock_time = 0;
    reset_minute_counter_with_seconds();

    /* 19. Setup MCA (demo mode) */
    printf("[Init] Setting up MCA (simulation mode)...\n");
    Mca_setup_demo();

    if (Mca_installedDetector != DET_EMPTY)
        current.language = ENGLISH;

    /* 20. Initialize USB (no-op) and printer (no-op) */
    initSetUSB();
    printer_init();

    /* 21. Randomize */
    srand((unsigned int)clock_time);
    AmuletWellAutoCalibrate_exportSequence = rand();

    /* 22. Watchdog (no-op in emulator) */
    /* setup_watchdog() skipped - WATCHDOG_OFF is defined */

    /* 23. Enable touch screen */
    set_touch_reset(TRUE);

    /* 24. Setup study/cal types */
    set_study_type();
    set_calicheck_tube();
    set_time_unit();

    /* 25. Initialize screen (LCD - just prints debug message) */
    initialize_screen();

    /* 26. Load Amulet HTML page map */
    printf("[Init] Loading Amulet HTML page map...\n");
    LoadAmuletHTML(1); /* Board type 1 = new board */

    /* Emulator doesn't have firmware update .DAT files on SD card.
     * Zero out these globals to prevent the version-check code
     * in AmuletMainScreenMenu from reading uninitialized memory. */
    {
        extern char Amulet_amuletImage[];
        extern char Amulet_amuletImage2[];
        memset(Amulet_amuletImage, 0, 20);
        memset(Amulet_amuletImage2, 0, 20);
    }

    printf("\n[Emulator] Initialization complete!\n");
    printf("[Emulator] WebSocket server: ws://localhost:%d/ws\n", ws_port);
    printf("[Emulator] Open http://localhost:%d in your browser\n", ws_port);
    printf("[Emulator] Press Ctrl+C to stop\n\n");

    /* ---- Phase 6: Enter Main Firmware Loop ---- */
    /* Inject the startup command that the Amulet display would normally send.
     * This tells the firmware to enter the main measurement screen. */
    amulet_bridge_inject_startup();

    /* The firmware's measurement_screen() is a forever loop that calls
     * service_amulet() to process touchscreen commands. In the emulator,
     * we call it directly and let it run. */
    printf("[Emulator] Entering measurement screen (main loop)...\n");
    measurement_screen();

    /* If measurement_screen returns (shouldn't normally), clean up */
    printf("[Emulator] Main loop exited, shutting down...\n");

    /* ---- Cleanup ---- */
    platform_timer_stop();
    amulet_bridge_shutdown();
    hal_shutdown();
    sim_shutdown();

    printf("[Emulator] Shutdown complete.\n");
    return 0;
}

/* ---- HAL Init/Shutdown Aggregators ---- */

extern void hal_coldfire_init(void);
extern void hal_uart_init(void);
extern void hal_uart_shutdown(void);
extern void hal_i2c_init(void);
extern void hal_i2c_shutdown(void);

void hal_init(void)
{
    hal_coldfire_init();
    hal_uart_init();
    hal_i2c_init();
    /* QSPI, PIT, FatFS, GPIO are initialized via the firmware's init sequence */
}

void hal_shutdown(void)
{
    hal_uart_shutdown();
    hal_i2c_shutdown();
}

/* ---- Timer Control (called from hal_pit.c) ---- */

void hal_timer_start(void)
{
    platform_timer_start();
}

void hal_timer_stop(void)
{
    platform_timer_stop();
}

/* (extern declarations moved to top of file) */

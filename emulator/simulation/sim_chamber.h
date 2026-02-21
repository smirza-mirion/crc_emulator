/*
 * sim_chamber.h - Measurement Simulation Engine for CRC-25R Emulator
 *
 * Generates realistic ADC values and MCA spectra for testing the
 * firmware without real radioactive sources. Simulates ionization
 * chamber current based on source activity, nuclide response factors,
 * radioactive decay, and Gaussian noise.
 */
#ifndef SIM_CHAMBER_H
#define SIM_CHAMBER_H

#include "crc.h"

/* Maximum channels (chambers) */
#define SIM_MAX_CHANNELS 8

/* Source configuration for a single chamber */
typedef struct {
    bool enabled;           /* Chamber enabled */
    short chamber_type;     /* 0=R, 1=PET, 2=BT, 3=1.8Atm, 4=100Ci, 5=1000Ci */
    short nuclide_index;    /* Index into nuclide database */
    float activity_uci;     /* Source activity in microcuries */
    float half_life;        /* Half-life in seconds */
    short half_life_unit;   /* SEC, MIN, HOUR, DAY, YEAR */
    time_t calibration_time;/* When the activity was calibrated */
    short container_type;   /* REF, SYR, VIAL, CAP */
    float noise_pct;        /* Noise level 0-10% */
    float response_factor;  /* Nuclide response factor */
    float gain_factor;      /* Chamber gain factor */
    float container_corr;   /* Container correction factor */
    float background;       /* Background reading (uCi equivalent) */
    bool over_range;        /* Force over-range condition */
    bool disconnected;      /* Simulate disconnected chamber */
    float voltage;          /* Chamber voltage (typically ~150V) */
} SimSource;

/* Simulation state */
typedef struct {
    SimSource sources[SIM_MAX_CHANNELS];
    int num_active;
    bool initialized;
    float ambient_background;  /* Background radiation level */
    unsigned int random_seed;
} SimState;

/* Initialize the simulation engine */
void sim_init(void);
void sim_shutdown(void);

/* Get simulated ADC value for a chamber */
long sim_get_adc_value(short channel);

/* Get simulated chamber voltage */
float sim_get_voltage(short channel);

/* Configure a source */
void sim_set_source(short channel, SimSource *source);
void sim_set_source_activity(short channel, float activity_uci);
void sim_clear_source(short channel);

/* Get current source config */
SimSource *sim_get_source(short channel);

/* Load a preset scenario */
void sim_load_scenario(int scenario_id);

/* Get the global simulation state */
SimState *sim_get_state(void);

/* MCA spectrum simulation */
void sim_mca_init(void);
void sim_mca_generate_spectrum(long *spectrum, int num_channels,
                               float energy_kev, float activity,
                               float efficiency);
void sim_mca_generate_background(long *spectrum, int num_channels);

/* Scenario IDs */
#define SCENARIO_COLD_START       0
#define SCENARIO_DAILY_CO57       1
#define SCENARIO_TC99M_MEAS       2
#define SCENARIO_MULTI_CHAMBER    3
#define SCENARIO_MCA_WELL         4
#define SCENARIO_ACCURACY         5
#define SCENARIO_LINEARITY        6
#define SCENARIO_MOLY_ASSAY       7
#define SCENARIO_BACKGROUND       8
#define SCENARIO_ERROR_CONDITIONS  9

#endif /* SIM_CHAMBER_H */

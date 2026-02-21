/*
 * sim_chamber.c - Measurement Simulation Engine for CRC-25R Emulator
 *
 * Generates realistic ADC values based on simulated radioactive source
 * activity, nuclide response factors, radioactive decay, and measurement
 * noise. Called by hal_qspi.c's service_adc() to provide simulated
 * ionization chamber current readings.
 *
 * ADC model:
 *   adc_value = (activity_mCi * response_factor * gain_factor * container_corr)
 *               * ADC_COUNTS_PER_MCI
 *               + background_noise
 *               + gaussian_random_noise(noise_level)
 *
 * The real firmware's ADC produces a 24-bit signed value from the
 * ionization current. A typical Tc-99m measurement at 15 mCi gives
 * an ADC value around 500,000.
 */

#include "sim_chamber.h"
#include "crc.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---------- Constants ---------- */

/* ADC counts per mCi for the standard R chamber type.
 * 15 mCi Tc-99m -> ~500,000 ADC counts -> ~33,333 counts/mCi */
#define ADC_COUNTS_PER_MCI  33333.0f

/* 24-bit signed ADC maximum (2^23 - 1) */
#define ADC_MAX_VALUE       8388607L

/* Natural log(2) for decay calculation */
#define LN2                 0.693147180559945f

/* Half-life unit conversion to seconds */
#define SECONDS_PER_MIN     60.0f
#define SECONDS_PER_HOUR    3600.0f
#define SECONDS_PER_DAY     86400.0f
#define SECONDS_PER_YEAR    31557600.0f   /* Julian year: 365.25 * 86400 */

/* Default ambient background in uCi-equivalent */
#define DEFAULT_AMBIENT_BKG 0.005f

/* ---------- Global Simulation State ---------- */

static SimState g_sim_state;

/* ---------- Internal Helper Functions ---------- */

/*
 * gaussian_random - Generate a Gaussian-distributed random number
 *
 * Uses the Box-Muller transform to convert two uniform random numbers
 * into a standard normal variate (mean=0, sigma=1).
 */
static float gaussian_random(void)
{
    float u1, u2;

    /* Avoid log(0) by clamping u1 away from zero */
    do {
        u1 = (float)rand() / (float)RAND_MAX;
    } while (u1 <= 1.0e-7f);

    u2 = (float)rand() / (float)RAND_MAX;

    return (float)(sqrt(-2.0 * log((double)u1)) * cos(2.0 * M_PI * (double)u2));
}

/*
 * half_life_to_seconds - Convert half-life to seconds based on unit enum
 *
 * The firmware uses SEC=0, MIN=1, HOUR=2, DAY=3, YEAR=4
 * (defined in crc.h).
 */
static float half_life_to_seconds(float half_life, short unit)
{
    switch (unit) {
    case SEC:
        return half_life;
    case MIN:
        return half_life * SECONDS_PER_MIN;
    case HOUR:
        return half_life * SECONDS_PER_HOUR;
    case DAY:
        return half_life * SECONDS_PER_DAY;
    case YEAR:
        return half_life * SECONDS_PER_YEAR;
    default:
        return half_life;  /* Assume seconds if unknown */
    }
}

/*
 * calculate_decayed_activity - Apply radioactive decay to source activity
 *
 * A(t) = A0 * exp(-ln(2) * elapsed / half_life)
 *
 * All times in seconds. Returns current activity in microcuries.
 */
static float calculate_decayed_activity(const SimSource *src)
{
    float hl_sec;
    double elapsed;
    float decay_factor;

    if (src->activity_uci <= 0.0f) {
        return 0.0f;
    }

    /* If no calibration time set, return undecayed activity */
    if (src->calibration_time == 0 || src->calibration_time == (time_t)(-1)) {
        return src->activity_uci;
    }

    hl_sec = half_life_to_seconds(src->half_life, src->half_life_unit);
    if (hl_sec <= 0.0f) {
        return src->activity_uci;
    }

    elapsed = difftime(time(NULL), src->calibration_time);
    if (elapsed < 0.0) {
        /* Calibration is in the future -- return full activity */
        return src->activity_uci;
    }

    decay_factor = (float)exp(-LN2 * elapsed / (double)hl_sec);

    return src->activity_uci * decay_factor;
}

/* ---------- Public API ---------- */

/*
 * sim_init - Initialize the simulation engine
 *
 * Sets up default state: one R chamber connected with no source
 * (SCENARIO_COLD_START).
 */
void sim_init(void)
{
    int i;

    memset(&g_sim_state, 0, sizeof(SimState));

    /* Initialize all chambers to disconnected with safe defaults */
    for (i = 0; i < SIM_MAX_CHANNELS; i++) {
        g_sim_state.sources[i].enabled = FALSE;
        g_sim_state.sources[i].chamber_type = 0;  /* R */
        g_sim_state.sources[i].nuclide_index = 0;
        g_sim_state.sources[i].activity_uci = 0.0f;
        g_sim_state.sources[i].half_life = 0.0f;
        g_sim_state.sources[i].half_life_unit = SEC;
        g_sim_state.sources[i].calibration_time = 0;
        g_sim_state.sources[i].container_type = REF;
        g_sim_state.sources[i].noise_pct = 1.0f;
        g_sim_state.sources[i].response_factor = 1.0f;
        g_sim_state.sources[i].gain_factor = 1.0f;
        g_sim_state.sources[i].container_corr = 1.0f;
        g_sim_state.sources[i].background = 0.0f;
        g_sim_state.sources[i].over_range = FALSE;
        g_sim_state.sources[i].disconnected = FALSE;
        g_sim_state.sources[i].voltage = 0.0f;
    }

    g_sim_state.num_active = 0;
    g_sim_state.ambient_background = DEFAULT_AMBIENT_BKG;
    g_sim_state.random_seed = (unsigned int)time(NULL);
    srand(g_sim_state.random_seed);

    /* Default: cold start with one R chamber */
    g_sim_state.sources[0].enabled = TRUE;
    g_sim_state.sources[0].chamber_type = 0;  /* R */
    g_sim_state.sources[0].voltage = 150.0f;
    g_sim_state.num_active = 1;

    g_sim_state.initialized = TRUE;
}

/*
 * sim_shutdown - Clean up the simulation engine
 */
void sim_shutdown(void)
{
    g_sim_state.initialized = FALSE;
}

/*
 * sim_get_adc_value - Get simulated ADC reading for a chamber
 *
 * This is the primary interface called by hal_qspi.c service_adc().
 * Returns a 24-bit signed ADC value representing ionization current.
 *
 * The conversion chain:
 *   1. Calculate decayed activity (uCi -> mCi)
 *   2. Multiply by response factor, gain factor, container correction
 *   3. Scale to ADC counts (33333 counts / mCi)
 *   4. Add background contribution
 *   5. Add Gaussian noise
 *   6. Clamp to 24-bit range
 */
long sim_get_adc_value(short channel)
{
    SimSource *src;
    float current_activity_uci;
    float current_activity_mci;
    float signal;
    float noise_amplitude;
    float noise;
    float background_adc;
    double adc_raw;
    long adc_value;

    if (!g_sim_state.initialized) {
        return 0;
    }

    if (channel < 0 || channel >= SIM_MAX_CHANNELS) {
        return 0;
    }

    src = &g_sim_state.sources[channel];

    /* Disconnected chamber: no signal */
    if (!src->enabled || src->disconnected) {
        return 0;
    }

    /* Over-range: saturate ADC */
    if (src->over_range) {
        return ADC_MAX_VALUE;
    }

    /* Calculate current activity with radioactive decay */
    current_activity_uci = calculate_decayed_activity(src);

    /* Convert uCi to mCi (1 mCi = 1000 uCi) */
    current_activity_mci = current_activity_uci / 1000.0f;

    /* Calculate signal: activity scaled by chamber and nuclide factors */
    signal = current_activity_mci
             * src->response_factor
             * src->gain_factor
             * src->container_corr;

    /* Scale to ADC counts */
    adc_raw = (double)signal * ADC_COUNTS_PER_MCI;

    /* Add background contribution */
    background_adc = (src->background + g_sim_state.ambient_background)
                     / 1000.0f  /* uCi -> mCi */
                     * ADC_COUNTS_PER_MCI;
    adc_raw += (double)background_adc;

    /* Add Gaussian noise proportional to the signal or a minimum floor */
    if (src->noise_pct > 0.0f) {
        noise_amplitude = (float)fabs(adc_raw) * (src->noise_pct / 100.0f);

        /* Enforce a minimum noise floor so even zero-activity readings
         * have some fluctuation (simulates electronic noise) */
        if (noise_amplitude < 5.0f) {
            noise_amplitude = 5.0f;
        }

        noise = gaussian_random() * noise_amplitude;
        adc_raw += (double)noise;
    }

    /* Clamp to 24-bit signed range */
    if (adc_raw > (double)ADC_MAX_VALUE) {
        adc_value = ADC_MAX_VALUE;
    } else if (adc_raw < (double)(-ADC_MAX_VALUE)) {
        adc_value = -ADC_MAX_VALUE;
    } else {
        adc_value = (long)adc_raw;
    }

    return adc_value;
}

/*
 * sim_get_voltage - Get simulated chamber voltage
 *
 * Returns the configured chamber voltage, or 0 if disconnected.
 * The real hardware reads ~150V from the electrometer bias supply.
 */
float sim_get_voltage(short channel)
{
    SimSource *src;

    if (!g_sim_state.initialized) {
        return 0.0f;
    }

    if (channel < 0 || channel >= SIM_MAX_CHANNELS) {
        return 0.0f;
    }

    src = &g_sim_state.sources[channel];

    if (!src->enabled || src->disconnected) {
        return 0.0f;
    }

    return src->voltage;
}

/*
 * sim_set_source - Configure a chamber source from a SimSource struct
 */
void sim_set_source(short channel, SimSource *source)
{
    if (channel < 0 || channel >= SIM_MAX_CHANNELS || source == NULL) {
        return;
    }

    memcpy(&g_sim_state.sources[channel], source, sizeof(SimSource));

    /* Recalculate active count */
    {
        int i, count = 0;
        for (i = 0; i < SIM_MAX_CHANNELS; i++) {
            if (g_sim_state.sources[i].enabled) {
                count++;
            }
        }
        g_sim_state.num_active = count;
    }
}

/*
 * sim_set_source_activity - Quick-set the activity for a channel
 *
 * Convenience function for the simulation control panel.
 * Sets the activity and resets calibration time to now.
 */
void sim_set_source_activity(short channel, float activity_uci)
{
    if (channel < 0 || channel >= SIM_MAX_CHANNELS) {
        return;
    }

    g_sim_state.sources[channel].activity_uci = activity_uci;
    g_sim_state.sources[channel].calibration_time = time(NULL);
}

/*
 * sim_clear_source - Remove a source from a chamber
 *
 * Sets activity to zero and disables the chamber.
 */
void sim_clear_source(short channel)
{
    if (channel < 0 || channel >= SIM_MAX_CHANNELS) {
        return;
    }

    g_sim_state.sources[channel].activity_uci = 0.0f;
    g_sim_state.sources[channel].calibration_time = 0;
    g_sim_state.sources[channel].over_range = FALSE;

    /* Recalculate active count */
    {
        int i, count = 0;
        for (i = 0; i < SIM_MAX_CHANNELS; i++) {
            if (g_sim_state.sources[i].enabled &&
                g_sim_state.sources[i].activity_uci > 0.0f) {
                count++;
            }
        }
        g_sim_state.num_active = count;
    }
}

/*
 * sim_get_source - Get pointer to a channel's source configuration
 */
SimSource *sim_get_source(short channel)
{
    if (channel < 0 || channel >= SIM_MAX_CHANNELS) {
        return NULL;
    }

    return &g_sim_state.sources[channel];
}

/*
 * sim_get_state - Get pointer to the global simulation state
 */
SimState *sim_get_state(void)
{
    return &g_sim_state;
}

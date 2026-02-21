/*
 * sim_scenarios.c - Preset Test Scenarios for CRC-25R Emulator
 *
 * Each scenario configures the SimState with appropriate sources
 * for common testing situations. These are selected from the
 * simulation control panel in the web frontend.
 *
 * Nuclide response factors are for the R chamber type (index 0)
 * and represent the calibration number (dial setting) used in the
 * real CRC-25R to convert ionization current to activity.
 *
 * Common nuclear medicine nuclides and their properties:
 *   Tc-99m:  HL=6.0067h,   response_R=963,  energy=140.5 keV
 *   Co-57:   HL=271.8d,    response_R=1410, energy=122.1 keV
 *   Ba-133:  HL=10.54y,    response_R=1050, energy=356.0 keV
 *   Cs-137:  HL=30.07y,    response_R=1180, energy=661.7 keV
 *   Co-60:   HL=5.271y,    response_R=1350, energy=1332.5 keV
 *   F-18:    HL=109.77m,   response_R=980,  energy=511.0 keV
 *   I-131:   HL=8.02d,     response_R=1120, energy=364.5 keV
 *   Mo-99:   HL=65.94h,    response_R=1320, energy=739.5 keV
 *   Ga-67:   HL=78.28h,    response_R=1100, energy=300.2 keV
 *   In-111:  HL=67.31h,    response_R=1060, energy=245.4 keV
 *   Tl-201:  HL=72.91h,    response_R=1130, energy=167.4 keV
 */

#include "sim_chamber.h"
#include "crc.h"
#include <string.h>
#include <time.h>

/* ---------- Internal Helper ---------- */

/*
 * init_source_defaults - Set sane defaults for a SimSource
 */
static void init_source_defaults(SimSource *src)
{
    memset(src, 0, sizeof(SimSource));
    src->enabled = FALSE;
    src->chamber_type = 0;      /* R */
    src->nuclide_index = 0;
    src->activity_uci = 0.0f;
    src->half_life = 0.0f;
    src->half_life_unit = SEC;
    src->calibration_time = 0;
    src->container_type = REF;
    src->noise_pct = 1.0f;
    src->response_factor = 1.0f;
    src->gain_factor = 1.0f;
    src->container_corr = 1.0f;
    src->background = 0.0f;
    src->over_range = FALSE;
    src->disconnected = FALSE;
    src->voltage = 0.0f;
}

/*
 * configure_tc99m - Set up a Tc-99m source on a channel
 *
 * Tc-99m (Technetium-99m): Most commonly used radionuclide in
 * nuclear medicine. Half-life 6.0067 hours, 140.5 keV gamma.
 */
static void configure_tc99m(SimSource *src, float activity_uci)
{
    src->enabled = TRUE;
    src->nuclide_index = 0;          /* Tc-99m is typically index 0 */
    src->activity_uci = activity_uci;
    src->half_life = 6.0067f;        /* hours */
    src->half_life_unit = HOUR;
    src->calibration_time = time(NULL);
    src->response_factor = 963.0f;
    src->gain_factor = 1.0f;
    src->container_corr = 1.0f;
    src->noise_pct = 1.5f;
    src->voltage = 150.0f;
}

/*
 * configure_co57 - Set up a Co-57 source on a channel
 *
 * Co-57 (Cobalt-57): Standard daily constancy check source.
 * Half-life 271.8 days, 122.1 keV gamma.
 */
static void configure_co57(SimSource *src, float activity_uci)
{
    src->enabled = TRUE;
    src->nuclide_index = 13;         /* Co-57 typical index */
    src->activity_uci = activity_uci;
    src->half_life = 271.8f;         /* days */
    src->half_life_unit = DAY;
    src->calibration_time = time(NULL);
    src->response_factor = 1410.0f;
    src->gain_factor = 1.0f;
    src->container_corr = 1.0f;
    src->noise_pct = 2.0f;
    src->voltage = 150.0f;
}

/*
 * configure_ba133 - Set up a Ba-133 source
 *
 * Ba-133 (Barium-133): Standard accuracy test source.
 * Half-life 10.54 years, 356.0 keV gamma.
 */
static void configure_ba133(SimSource *src, float activity_uci)
{
    src->enabled = TRUE;
    src->nuclide_index = 14;
    src->activity_uci = activity_uci;
    src->half_life = 10.54f;         /* years */
    src->half_life_unit = YEAR;
    src->calibration_time = time(NULL);
    src->response_factor = 1050.0f;
    src->gain_factor = 1.0f;
    src->container_corr = 1.0f;
    src->noise_pct = 2.0f;
    src->voltage = 150.0f;
}

/*
 * configure_cs137 - Set up a Cs-137 source
 *
 * Cs-137 (Cesium-137): Standard calibration and accuracy source.
 * Half-life 30.07 years, 661.7 keV gamma.
 */
static void configure_cs137(SimSource *src, float activity_uci)
{
    src->enabled = TRUE;
    src->nuclide_index = 15;
    src->activity_uci = activity_uci;
    src->half_life = 30.07f;         /* years */
    src->half_life_unit = YEAR;
    src->calibration_time = time(NULL);
    src->response_factor = 1180.0f;
    src->gain_factor = 1.0f;
    src->container_corr = 1.0f;
    src->noise_pct = 1.5f;
    src->voltage = 150.0f;
}

/*
 * configure_co60 - Set up a Co-60 source
 *
 * Co-60 (Cobalt-60): High-energy calibration source.
 * Half-life 5.271 years, 1173.2 + 1332.5 keV gammas.
 */
static void configure_co60(SimSource *src, float activity_uci)
{
    src->enabled = TRUE;
    src->nuclide_index = 16;
    src->activity_uci = activity_uci;
    src->half_life = 5.271f;         /* years */
    src->half_life_unit = YEAR;
    src->calibration_time = time(NULL);
    src->response_factor = 1350.0f;
    src->gain_factor = 1.0f;
    src->container_corr = 1.0f;
    src->noise_pct = 1.5f;
    src->voltage = 150.0f;
}

/*
 * configure_f18 - Set up an F-18 source
 *
 * F-18 (Fluorine-18): PET imaging isotope.
 * Half-life 109.77 minutes, 511 keV annihilation photons.
 */
static void configure_f18(SimSource *src, float activity_uci)
{
    src->enabled = TRUE;
    src->nuclide_index = 1;          /* F-18 common index */
    src->activity_uci = activity_uci;
    src->half_life = 109.77f;        /* minutes */
    src->half_life_unit = MIN;
    src->calibration_time = time(NULL);
    src->response_factor = 980.0f;
    src->gain_factor = 1.0f;
    src->container_corr = 1.0f;
    src->noise_pct = 1.5f;
    src->voltage = 150.0f;
}

/*
 * configure_i131 - Set up an I-131 source
 *
 * I-131 (Iodine-131): Therapy isotope.
 * Half-life 8.02 days, 364.5 keV gamma.
 */
static void configure_i131(SimSource *src, float activity_uci)
{
    src->enabled = TRUE;
    src->nuclide_index = 5;
    src->activity_uci = activity_uci;
    src->half_life = 8.02f;          /* days */
    src->half_life_unit = DAY;
    src->calibration_time = time(NULL);
    src->response_factor = 1120.0f;
    src->gain_factor = 1.0f;
    src->container_corr = 1.0f;
    src->noise_pct = 1.5f;
    src->voltage = 150.0f;
}

/*
 * configure_mo99 - Set up a Mo-99 source
 *
 * Mo-99 (Molybdenum-99): Generator parent for Tc-99m.
 * Half-life 65.94 hours, 739.5 keV gamma.
 */
static void configure_mo99(SimSource *src, float activity_uci)
{
    src->enabled = TRUE;
    src->nuclide_index = 3;
    src->activity_uci = activity_uci;
    src->half_life = 65.94f;         /* hours */
    src->half_life_unit = HOUR;
    src->calibration_time = time(NULL);
    src->response_factor = 1320.0f;
    src->gain_factor = 1.0f;
    src->container_corr = 1.0f;
    src->noise_pct = 2.0f;
    src->voltage = 150.0f;
}

/* ---------- Public API ---------- */

/*
 * sim_load_scenario - Load a preset test scenario
 *
 * Reinitializes the simulation state and configures sources
 * appropriate for the requested test scenario.
 */
void sim_load_scenario(int scenario_id)
{
    SimState *state = sim_get_state();
    int i;

    /* Reinitialize all channels to clean defaults */
    sim_init();

    switch (scenario_id) {

    /* --------------------------------------------------------
     * SCENARIO_COLD_START (0)
     * System just powered on. One R chamber connected, no source.
     * This is the default state after sim_init().
     * -------------------------------------------------------- */
    case SCENARIO_COLD_START:
        state->sources[0].enabled = TRUE;
        state->sources[0].chamber_type = 0;  /* R */
        state->sources[0].activity_uci = 0.0f;
        state->sources[0].voltage = 150.0f;
        state->sources[0].noise_pct = 0.5f;
        state->num_active = 1;
        break;

    /* --------------------------------------------------------
     * SCENARIO_DAILY_CO57 (1)
     * Daily constancy check with a Co-57 reference source.
     * Typical activity: 5 uCi (a sealed reference standard).
     * -------------------------------------------------------- */
    case SCENARIO_DAILY_CO57:
        configure_co57(&state->sources[0], 5.0f);  /* 5 uCi */
        state->sources[0].chamber_type = 0;  /* R */
        state->sources[0].container_type = REF;
        state->num_active = 1;
        break;

    /* --------------------------------------------------------
     * SCENARIO_TC99M_MEAS (2)
     * Standard Tc-99m measurement at 15 mCi (15000 uCi).
     * Typical patient dose measurement scenario.
     * -------------------------------------------------------- */
    case SCENARIO_TC99M_MEAS:
        configure_tc99m(&state->sources[0], 15000.0f);  /* 15 mCi */
        state->sources[0].chamber_type = 0;  /* R */
        state->sources[0].container_type = SYR;
        state->num_active = 1;
        break;

    /* --------------------------------------------------------
     * SCENARIO_MULTI_CHAMBER (3)
     * Two chambers with different sources.
     * Chamber 0: Tc-99m 10 mCi (patient dose in syringe)
     * Chamber 1: F-18 5 mCi (PET dose in vial)
     * -------------------------------------------------------- */
    case SCENARIO_MULTI_CHAMBER:
        /* Chamber 0: Tc-99m at 10 mCi */
        configure_tc99m(&state->sources[0], 10000.0f);  /* 10 mCi */
        state->sources[0].chamber_type = 0;  /* R */
        state->sources[0].container_type = SYR;

        /* Chamber 1: F-18 at 5 mCi */
        configure_f18(&state->sources[1], 5000.0f);  /* 5 mCi */
        state->sources[1].chamber_type = 1;  /* PET */
        state->sources[1].container_type = VIAL;

        state->num_active = 2;
        break;

    /* --------------------------------------------------------
     * SCENARIO_MCA_WELL (4)
     * Well detector with a Cs-137 check source for MCA calibration.
     * Low activity source (~1 uCi) placed in the well detector.
     * -------------------------------------------------------- */
    case SCENARIO_MCA_WELL:
        /* Chamber 0 still active but no source */
        state->sources[0].enabled = TRUE;
        state->sources[0].chamber_type = 0;  /* R */
        state->sources[0].voltage = 150.0f;
        state->sources[0].noise_pct = 0.5f;

        /* Chamber 1 represents the well detector source */
        configure_cs137(&state->sources[1], 1.0f);  /* 1 uCi */
        state->sources[1].chamber_type = 0;
        state->sources[1].container_type = REF;

        state->num_active = 2;
        break;

    /* --------------------------------------------------------
     * SCENARIO_ACCURACY (5)
     * Five standard reference sources for accuracy testing
     * per NRC/state regulatory requirements.
     * Uses NIST-traceable sources at known activities.
     * -------------------------------------------------------- */
    case SCENARIO_ACCURACY:
        /* Source 1: Co-57 at 10 uCi */
        configure_co57(&state->sources[0], 10.0f);
        state->sources[0].container_type = REF;

        /* Source 2: Ba-133 at 15 uCi */
        configure_ba133(&state->sources[1], 15.0f);
        state->sources[1].container_type = REF;

        /* Source 3: Cs-137 at 20 uCi */
        configure_cs137(&state->sources[2], 20.0f);
        state->sources[2].container_type = REF;

        /* Source 4: Co-60 at 10 uCi */
        configure_co60(&state->sources[3], 10.0f);
        state->sources[3].container_type = REF;

        /* Source 5: I-131 at 50 uCi */
        configure_i131(&state->sources[4], 50.0f);
        state->sources[4].container_type = REF;

        state->num_active = 5;
        break;

    /* --------------------------------------------------------
     * SCENARIO_LINEARITY (6)
     * High-activity Tc-99m for linearity test.
     * Starts at ~200 mCi and operator waits for it to decay.
     * The firmware measures at intervals to verify linearity
     * across the activity range.
     * -------------------------------------------------------- */
    case SCENARIO_LINEARITY:
        configure_tc99m(&state->sources[0], 200000.0f);  /* 200 mCi */
        state->sources[0].chamber_type = 0;  /* R */
        state->sources[0].container_type = VIAL;
        state->sources[0].noise_pct = 1.0f;
        state->num_active = 1;
        break;

    /* --------------------------------------------------------
     * SCENARIO_MOLY_ASSAY (7)
     * Mo-99/Tc-99m generator assay (moly breakthrough test).
     * The generator eluate is measured for Mo-99 contamination
     * using a lead shield that blocks the Tc-99m 140 keV gamma
     * but passes the Mo-99 739 keV gamma.
     *
     * Chamber 0: Tc-99m eluate (the dose)
     * Chamber 1: Same vial with Pb shield (Mo-99 breakthrough)
     * -------------------------------------------------------- */
    case SCENARIO_MOLY_ASSAY:
        /* Unshielded measurement: Tc-99m at 30 mCi */
        configure_tc99m(&state->sources[0], 30000.0f);  /* 30 mCi */
        state->sources[0].chamber_type = 0;
        state->sources[0].container_type = VIAL;

        /* Shielded measurement: Mo-99 breakthrough at 0.5 uCi
         * (regulatory limit is 0.15 uCi Mo-99 per mCi Tc-99m) */
        configure_mo99(&state->sources[1], 0.5f);  /* 0.5 uCi Mo-99 */
        state->sources[1].chamber_type = 0;
        state->sources[1].container_type = VIAL;
        state->sources[1].noise_pct = 3.0f;  /* Higher noise at low activity */

        state->num_active = 2;
        break;

    /* --------------------------------------------------------
     * SCENARIO_BACKGROUND (8)
     * No source present, just ambient background radiation.
     * Used to verify the instrument background reading before
     * performing measurements.
     * -------------------------------------------------------- */
    case SCENARIO_BACKGROUND:
        state->sources[0].enabled = TRUE;
        state->sources[0].chamber_type = 0;  /* R */
        state->sources[0].activity_uci = 0.0f;
        state->sources[0].noise_pct = 0.5f;
        state->sources[0].voltage = 150.0f;
        state->sources[0].background = 0.002f;  /* ~2 nCi ambient equiv */
        state->num_active = 1;
        break;

    /* --------------------------------------------------------
     * SCENARIO_ERROR_CONDITIONS (9)
     * Simulate various error conditions:
     *   Chamber 0: Over-range (ADC saturated)
     *   Chamber 1: Disconnected (no chamber present)
     *   Chamber 2: Low voltage (degraded electrometer)
     * -------------------------------------------------------- */
    case SCENARIO_ERROR_CONDITIONS:
        /* Chamber 0: Over-range condition */
        state->sources[0].enabled = TRUE;
        state->sources[0].chamber_type = 0;
        state->sources[0].over_range = TRUE;
        state->sources[0].voltage = 150.0f;

        /* Chamber 1: Disconnected chamber */
        state->sources[1].enabled = TRUE;
        state->sources[1].disconnected = TRUE;
        state->sources[1].voltage = 0.0f;

        /* Chamber 2: Low voltage - simulates degraded electrometer
         * or failing HV supply. Readings will be present but
         * the voltage check should flag a warning. */
        configure_tc99m(&state->sources[2], 5000.0f);  /* 5 mCi */
        state->sources[2].voltage = 85.0f;  /* Below normal ~150V */
        state->sources[2].noise_pct = 5.0f; /* Noisy due to low voltage */

        state->num_active = 3;
        break;

    default:
        /* Unknown scenario: fall through to cold start (already set by sim_init) */
        break;
    }
}

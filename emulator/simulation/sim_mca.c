/*
 * sim_mca.c - MCA (Multi-Channel Analyzer) Spectrum Simulation
 *
 * Generates realistic gamma-ray spectra for the Well detector
 * simulation. The firmware already has MCA_SIMULATION support via
 * Mca_setup_demo(); this file provides additional spectrum generation
 * utilities used by the emulator's simulation control panel.
 *
 * Spectrum model for a gamma-ray source:
 *   - Gaussian photopeak at the primary energy
 *   - Compton continuum from 0 to the Compton edge
 *   - Backscatter peak around 200 keV
 *   - Poisson statistical noise on all channels
 *   - Optional background (K-40 at 1461 keV, ambient)
 *
 * Energy-to-channel mapping:
 *   channel = energy_keV * num_channels / full_range_keV
 *   where full_range_keV = 2048 keV (typical NaI calibration)
 *
 * Resolution model (NaI scintillator):
 *   FWHM(keV) = 30 + 0.03 * energy_keV
 *   sigma = FWHM / 2.355
 */

#include "sim_chamber.h"
#include "crc.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* ---------- Constants ---------- */

/* Full energy range in keV (typical NaI MCA calibration) */
#define MCA_FULL_RANGE_KEV  2048.0f

/* Compton scattering: electron rest mass energy */
#define ELECTRON_MASS_KEV   511.0f

/* FWHM-to-sigma conversion factor: FWHM = 2.355 * sigma */
#define FWHM_TO_SIGMA       2.355f

/* Resolution model parameters for NaI(Tl) detector:
 * FWHM(keV) = RESOLUTION_BASE + RESOLUTION_SLOPE * energy_keV */
#define RESOLUTION_BASE     30.0f
#define RESOLUTION_SLOPE    0.03f

/* Compton continuum level relative to photopeak height */
#define COMPTON_FRACTION    0.15f

/* Backscatter peak relative height */
#define BACKSCATTER_FRAC    0.05f
#define BACKSCATTER_KEV     200.0f

/* Background K-40 peak energy */
#define K40_ENERGY_KEV      1461.0f
#define K40_PEAK_HEIGHT     20L

/* Minimum Poisson noise floor */
#define MIN_POISSON_COUNTS  1L

/* ---------- Internal Helper Functions ---------- */

/*
 * energy_to_channel - Convert energy in keV to MCA channel number
 */
static int energy_to_channel(float energy_kev, int num_channels)
{
    int ch = (int)(energy_kev * (float)num_channels / MCA_FULL_RANGE_KEV);
    if (ch < 0) ch = 0;
    if (ch >= num_channels) ch = num_channels - 1;
    return ch;
}

/*
 * gaussian_value - Evaluate Gaussian function at position x
 *
 * Returns: height * exp(-0.5 * ((x - center) / sigma)^2)
 */
static float gaussian_value(float x, float center, float sigma, float height)
{
    float dx = x - center;
    return height * (float)exp(-0.5 * (double)(dx * dx) / (double)(sigma * sigma));
}

/*
 * compton_edge_energy - Calculate Compton edge energy for a given photopeak
 *
 * E_compton = E_gamma * (2 * E_gamma / (ELECTRON_MASS + 2 * E_gamma))
 */
static float compton_edge_energy(float energy_kev)
{
    float two_e = 2.0f * energy_kev;
    return energy_kev * (two_e / (ELECTRON_MASS_KEV + two_e));
}

/*
 * poisson_noise - Add Poisson-distributed noise to a count value
 *
 * For large counts, approximate Poisson noise as Gaussian with
 * sigma = sqrt(counts). For small counts, use direct sampling.
 */
static long poisson_noise(long counts)
{
    float sigma;
    float noise;

    if (counts <= 0) {
        /* Small chance of a background count */
        return (rand() % 100 < 5) ? 1L : 0L;
    }

    if (counts < 20) {
        /* For small counts, add +/- a few counts */
        int delta = (rand() % 5) - 2;
        long result = counts + delta;
        return (result < 0) ? 0 : result;
    }

    /* Gaussian approximation: sigma = sqrt(N) */
    sigma = (float)sqrt((double)counts);

    /* Box-Muller for one Gaussian variate */
    {
        float u1, u2, g;
        do {
            u1 = (float)rand() / (float)RAND_MAX;
        } while (u1 <= 1.0e-7f);
        u2 = (float)rand() / (float)RAND_MAX;
        g = (float)(sqrt(-2.0 * log((double)u1)) * cos(2.0 * M_PI * (double)u2));

        noise = g * sigma;
    }

    {
        long result = counts + (long)noise;
        return (result < 0) ? 0 : result;
    }
}

/* ---------- Public API ---------- */

/*
 * sim_mca_init - Initialize MCA simulation state
 *
 * Seeds the random number generator for reproducible spectra
 * during testing.
 */
void sim_mca_init(void)
{
    /* RNG is already seeded by sim_init() in sim_chamber.c.
     * This function exists for future MCA-specific initialization
     * (e.g., loading detector efficiency curves). */
}

/*
 * sim_mca_generate_spectrum - Generate a simulated gamma-ray spectrum
 *
 * Produces a realistic NaI(Tl) scintillation detector spectrum with:
 *   - Gaussian photopeak at the specified energy
 *   - Compton continuum from channel 0 to the Compton edge
 *   - Backscatter peak near 200 keV
 *   - Poisson statistical noise
 *
 * Parameters:
 *   spectrum     - Output buffer (must be at least num_channels long)
 *   num_channels - Number of MCA channels (256, 512, 1024, or 2048)
 *   energy_kev   - Primary gamma-ray energy in keV
 *   activity     - Source activity (arbitrary units, scales peak height)
 *   efficiency   - Detector efficiency factor (0.0 - 1.0)
 */
void sim_mca_generate_spectrum(long *spectrum, int num_channels,
                               float energy_kev, float activity,
                               float efficiency)
{
    float fwhm, sigma;
    float peak_center;
    float peak_height;
    float compton_edge_kev;
    int compton_edge_ch;
    float compton_height;
    float bs_center, bs_sigma, bs_height;
    int i;

    if (spectrum == NULL || num_channels <= 0) {
        return;
    }

    /* Clear the spectrum buffer */
    memset(spectrum, 0, (size_t)num_channels * sizeof(long));

    if (energy_kev <= 0.0f || activity <= 0.0f) {
        return;
    }

    /* --- Energy resolution model --- */
    fwhm = RESOLUTION_BASE + RESOLUTION_SLOPE * energy_kev;
    sigma = fwhm / FWHM_TO_SIGMA;

    /* Peak center in channel units */
    peak_center = energy_kev * (float)num_channels / MCA_FULL_RANGE_KEV;

    /* Peak height proportional to activity and efficiency.
     * Scale so that a 1 uCi source with 100% efficiency gives
     * a reasonable peak around 1000 counts. */
    peak_height = activity * efficiency * 1000.0f;

    /* Convert sigma from keV to channels */
    {
        float sigma_ch = sigma * (float)num_channels / MCA_FULL_RANGE_KEV;

        /* --- Generate Gaussian photopeak --- */
        for (i = 0; i < num_channels; i++) {
            float ch = (float)i;
            float contribution = gaussian_value(ch, peak_center, sigma_ch, peak_height);
            if (contribution > 0.5f) {
                spectrum[i] += (long)contribution;
            }
        }

        /* --- Compton continuum --- */
        compton_edge_kev = compton_edge_energy(energy_kev);
        compton_edge_ch = energy_to_channel(compton_edge_kev, num_channels);

        /* Compton plateau: roughly flat from 0 to the Compton edge,
         * with height proportional to the photopeak */
        compton_height = peak_height * COMPTON_FRACTION;

        for (i = 0; i <= compton_edge_ch && i < num_channels; i++) {
            /* Slight upward slope toward the Compton edge */
            float slope_factor = 0.7f + 0.3f * ((float)i / (float)(compton_edge_ch + 1));
            spectrum[i] += (long)(compton_height * slope_factor);
        }

        /* Compton edge: a small bump just below the edge channel */
        if (compton_edge_ch > 2 && compton_edge_ch < num_channels) {
            float edge_sigma = sigma_ch * 1.5f;
            float edge_height = compton_height * 0.4f;
            int edge_start = compton_edge_ch - (int)(3.0f * edge_sigma);
            int edge_end = compton_edge_ch + (int)(2.0f * edge_sigma);
            if (edge_start < 0) edge_start = 0;
            if (edge_end >= num_channels) edge_end = num_channels - 1;

            for (i = edge_start; i <= edge_end; i++) {
                float contribution = gaussian_value((float)i,
                    (float)compton_edge_ch, edge_sigma, edge_height);
                if (contribution > 0.5f) {
                    spectrum[i] += (long)contribution;
                }
            }
        }

        /* --- Backscatter peak (~200 keV) --- */
        if (energy_kev > 300.0f) {
            bs_center = BACKSCATTER_KEV * (float)num_channels / MCA_FULL_RANGE_KEV;
            bs_sigma = sigma_ch * 2.0f;  /* Broader than photopeak */
            bs_height = peak_height * BACKSCATTER_FRAC;

            {
                int bs_start = (int)(bs_center - 4.0f * bs_sigma);
                int bs_end = (int)(bs_center + 4.0f * bs_sigma);
                if (bs_start < 0) bs_start = 0;
                if (bs_end >= num_channels) bs_end = num_channels - 1;

                for (i = bs_start; i <= bs_end; i++) {
                    float contribution = gaussian_value((float)i, bs_center,
                        bs_sigma, bs_height);
                    if (contribution > 0.5f) {
                        spectrum[i] += (long)contribution;
                    }
                }
            }
        }
    }

    /* --- Add Poisson noise to all channels --- */
    for (i = 0; i < num_channels; i++) {
        spectrum[i] = poisson_noise(spectrum[i]);
    }
}

/*
 * sim_mca_generate_background - Generate a background spectrum
 *
 * Produces a low-level background spectrum with:
 *   - Low random noise across all channels
 *   - A small K-40 peak at 1461 keV (natural background from
 *     potassium in building materials and detector shielding)
 *
 * Parameters:
 *   spectrum     - Output buffer (must be at least num_channels long)
 *   num_channels - Number of MCA channels
 */
void sim_mca_generate_background(long *spectrum, int num_channels)
{
    int i;
    int k40_ch;

    if (spectrum == NULL || num_channels <= 0) {
        return;
    }

    /* Low-level random noise across all channels (1-3 counts) */
    for (i = 0; i < num_channels; i++) {
        spectrum[i] = (long)(rand() % 4);
    }

    /* Slightly elevated low-energy region (scatter and electronic noise) */
    {
        int low_cutoff = num_channels / 10;  /* Bottom ~10% of channels */
        for (i = 0; i < low_cutoff && i < num_channels; i++) {
            spectrum[i] += (long)(rand() % 6) + 2;
        }
    }

    /* K-40 peak at 1461 keV if within range */
    k40_ch = energy_to_channel(K40_ENERGY_KEV, num_channels);
    if (k40_ch > 0 && k40_ch < num_channels) {
        float fwhm = RESOLUTION_BASE + RESOLUTION_SLOPE * K40_ENERGY_KEV;
        float sigma_ch = (fwhm / FWHM_TO_SIGMA) * (float)num_channels / MCA_FULL_RANGE_KEV;
        int peak_start = (int)((float)k40_ch - 3.0f * sigma_ch);
        int peak_end = (int)((float)k40_ch + 3.0f * sigma_ch);

        if (peak_start < 0) peak_start = 0;
        if (peak_end >= num_channels) peak_end = num_channels - 1;

        for (i = peak_start; i <= peak_end; i++) {
            float contribution = gaussian_value((float)i, (float)k40_ch,
                sigma_ch, (float)K40_PEAK_HEIGHT);
            if (contribution > 0.5f) {
                spectrum[i] += (long)contribution;
            }
        }
    }

    /* Apply Poisson noise */
    for (i = 0; i < num_channels; i++) {
        spectrum[i] = poisson_noise(spectrum[i]);
    }
}

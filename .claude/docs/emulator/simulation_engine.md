# Simulation Engine

## Purpose

The simulation engine generates realistic ADC values and MCA spectra for testing the firmware without real radioactive sources. It models ionization chamber current based on source activity, nuclide response factors, radioactive decay, and Gaussian noise.

## Source Files

| File | Description |
|------|-------------|
| `emulator/simulation/sim_chamber.h` | Data types, API declarations, scenario IDs |
| `emulator/simulation/sim_chamber.c` | ADC value generation with decay and noise |
| `emulator/simulation/sim_scenarios.c` | 10 preset test scenario configurations |
| `emulator/simulation/sim_mca.c` | MCA spectrum generation |

## Data Types

### SimSource

Configuration for a single ionization chamber:

```c
typedef struct {
    bool  enabled;           // Chamber enabled
    short chamber_type;      // 0=R, 1=PET, 2=BT, 3=1.8Atm, 4=100Ci, 5=1000Ci
    short nuclide_index;     // Index into firmware nuclide database
    float activity_uci;      // Source activity in microcuries
    float half_life;         // Half-life in seconds
    short half_life_unit;    // SEC, MIN, HOUR, DAY, YEAR
    time_t calibration_time; // When activity was calibrated
    short container_type;    // REF, SYR, VIAL, CAP
    float noise_pct;         // Noise level 0-10%
    float response_factor;   // Nuclide-specific calibration number
    float gain_factor;       // Chamber gain factor
    float container_corr;    // Container correction factor
    float background;        // Background reading (uCi equivalent)
    bool  over_range;        // Force over-range condition
    bool  disconnected;      // Simulate disconnected chamber
    float voltage;           // Chamber voltage (~150V typical)
} SimSource;
```

### SimState

Global simulation state:

```c
typedef struct {
    SimSource sources[SIM_MAX_CHANNELS]; // Up to 8 chambers
    int  num_active;                      // Number of enabled chambers
    bool initialized;
    float ambient_background;             // Ambient radiation level
    unsigned int random_seed;
} SimState;
```

## API

### Lifecycle

```c
void sim_init(void);       // Initialize engine, zero all sources
void sim_shutdown(void);   // Clean up resources
```

### ADC Generation

```c
long sim_get_adc_value(short channel);  // Get simulated ADC reading
float sim_get_voltage(short channel);   // Get simulated chamber voltage
```

Called by `hal_qspi.c:service_adc()` during each measurement cycle.

### Source Configuration

```c
void sim_set_source(short channel, SimSource *source);  // Full source config
void sim_set_source_activity(short channel, float activity_uci);  // Activity only
void sim_clear_source(short channel);   // Disable and clear
SimSource *sim_get_source(short channel);  // Get current config
SimState  *sim_get_state(void);          // Get global state
```

### Scenario Loading

```c
void sim_load_scenario(int scenario_id);  // Load preset scenario (0-9)
```

### MCA Spectrum

```c
void sim_mca_init(void);
void sim_mca_generate_spectrum(long *spectrum, int num_channels,
                               float energy_kev, float activity,
                               float efficiency);
void sim_mca_generate_background(long *spectrum, int num_channels);
```

## ADC Value Calculation

The ADC generator models the physical measurement chain:

```
adc_value = (activity × response_factor × gain_factor × container_correction)
            + background_noise
            + gaussian_random_noise(noise_level)
```

Where:
- **activity** = `activity_uci * exp(-ln(2) * elapsed_seconds / half_life)` (radioactive decay)
- **response_factor** = nuclide-specific calibration number from the database
- **gain_factor** = chamber gain (gainfactor0/1/2 from `CHAMBERVALS`)
- **container_correction** = correction for container type (syringe, vial, capsule, reference)
- **background_noise** = small random value modeling ambient radiation
- **gaussian_random_noise** = `±(noise_pct% × adc_value)` using Box-Muller transform

### Radioactive Decay

Activity decays exponentially from the calibration time:

```
current_activity = initial_activity × 2^(-elapsed_time / half_life)
```

Half-life units are converted to seconds for calculation.

### Over-Range Simulation

When `over_range = true`, the ADC returns the maximum value, causing the firmware to display "OVER" and set `measurement[ch].over_flag = TRUE`.

### Disconnected Chamber

When `disconnected = true`, the ADC returns 0 and `chamber[ch].connected_flag = FALSE`, causing the firmware to display a disconnection warning.

## Preset Scenarios

| ID | Name | Configuration |
|----|------|---------------|
| 0 | Cold Start | No sources, fresh system. All chambers disabled. |
| 1 | Daily QC (Co-57) | Chamber 0: Co-57 at 5 uCi, R type, reference container. Response factor 1410. |
| 2 | Tc-99m Measurement | Chamber 0: Tc-99m at 15 mCi, R type, syringe container. Response factor 963. |
| 3 | Multi-Chamber | Chamber 0: Tc-99m 15 mCi. Chamber 1: F-18 10 mCi. Both R type. |
| 4 | MCA Well Detector | Chamber 0: Cs-137 5 uCi. MCA well detector with gamma spectrum. |
| 5 | Accuracy Test | 5 chambers: Co-57 (5 uCi), Ba-133 (10 uCi), Cs-137 (5 uCi), Co-60 (5 uCi), Tc-99m (15 mCi) |
| 6 | Linearity Test | Chamber 0: Tc-99m at 200 mCi (high activity for linearity curve). |
| 7 | Moly Assay | Chamber 0: Mo-99 150 mCi + Tc-99m 10 mCi. Generator test. |
| 8 | Background Only | No sources. Only ambient background radiation (~0.001 uCi equivalent). |
| 9 | Error Conditions | Chamber 0: over-range. Chamber 1: disconnected. Chamber 2: low voltage. |

### Nuclide Response Factors (R Chamber)

From the firmware's nuclide database, used in scenarios:

| Nuclide | Response Factor | Half-Life |
|---------|----------------|-----------|
| Tc-99m | 963 | 6.01 hours |
| Co-57 | 1410 | 271.74 days |
| Ba-133 | 1050 | 10.51 years |
| Cs-137 | 1180 | 30.08 years |
| Co-60 | 1350 | 5.27 years |
| F-18 | 980 | 109.77 minutes |
| I-131 | 1120 | 8.02 days |
| Mo-99 | 1320 | 2.75 days |
| Ga-67 | 1100 | 3.26 days |
| In-111 | 1060 | 2.80 days |
| Tl-201 | 1130 | 3.04 days |

## MCA Spectrum Generation

### Gamma Spectrum

`sim_mca_generate_spectrum()` creates a realistic gamma-ray energy spectrum:

1. **Photopeak:** Gaussian distribution centered at the nuclide's primary energy
   - Center channel = `energy_kev / channel_width`
   - Width (sigma) = FWHM from Cs-137 reference at 661.66 keV
   - Height = proportional to `activity × efficiency`

2. **Compton Continuum:** Flat-to-sloped background from Compton scattering
   - Extends from 0 to Compton edge
   - Compton edge = `energy / (1 + 2 × energy / 511)`

3. **Statistical noise:** Poisson distribution on each channel count

### Background Spectrum

`sim_mca_generate_background()` generates ambient background:
- Low-level counts across all channels
- Slight increase at low energies (natural background)
- Random Poisson noise

## Integration with HAL

The simulation engine is called from the HAL at two points:

1. **`hal_qspi.c:service_adc()`** — Called from `timed_interrupt()` every measurement cycle:
   ```c
   measurement[ch].adc_value = sim_get_adc_value(ch);
   ```

2. **`hal_qspi.c`** — Chamber voltage reads:
   ```c
   float v = sim_get_voltage(ch);
   ```

The firmware's measurement pipeline then processes these values identically to real hardware.

## WebSocket Configuration

The frontend can configure sources at runtime via WebSocket:

```json
{"type": "loadScenario", "scenario": 1}

{"type": "configSource", "channel": 0, "enabled": true,
 "nuclide": "Tc-99m", "activity": 15.0, "unit": "mCi"}
```

These messages are handled by the bridge, which calls `sim_load_scenario()` or `sim_set_source()`.

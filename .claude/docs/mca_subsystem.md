# MCA (Multi-Channel Analyzer) Subsystem

## Purpose

The MCA subsystem controls spectrum acquisition and analysis for radiation detection. It interfaces with the MCA hardware card to acquire gamma/beta spectra, find energy peaks, compute efficiencies, and support auto-calibration. This is the core measurement engine for the Well Detector portion of the system.

## Usage

### Basic Spectrum Acquisition Sequence

1. Configure acquisition time: `Mca_setAcquireTime(seconds)` (`Mca.c:1603`)
2. Set counting mode: `Mca_setTimeType(COUNT_REAL|COUNT_LIVE)` (`Mca.c:992`)
3. Start counting: `Mca_startAcquisition()` (`Mca.c:1001`)
4. Wait for completion or stop: `Mca_stopAcquisition()` (`Mca.c:980`)
5. Read spectrum data: `Mca_getSpectrum()` (`Mca.c:1020`)
6. Find peaks: `Mca_getPeaks(peak, &numPeaks, isAutoCal)` (`Mca.c:1969`)

### Detector Switching

The system supports multiple detector types. Switch between them:
- `Mca_switchToWell()` - Select well detector
- `Mca_switchToProbe()` - Select probe detector
- `Mca_switchDetector(type)` - Generic switch

Detector types defined in `Headers/mca.h:1-10` (full enum) and `Headers/crc.h:189-193` (simplified):
- `DET_EMPTY (0)`, `DET_WELL (1)`, `DET_BETA (2)`, `DET_PROBE700 (3)`, `DET_WELL700 (4)`, `DET_DRILLEDPROBE700 (5)`

## Key Components

### Spectrum Data

The MCA reads spectral data from hardware at memory address `0x4000`, reading 8192 words into the `spec_meas` structure (`Mca.c:1020-1040`). Each channel represents an energy bin; channel counts represent detected events at that energy.

### Peak Finding

`Mca_getPeaks()` at `Mca.c:1969` identifies energy peaks in the spectrum:
- Uses tophat filtering algorithm (`Mca_mcaFindPeaks()` at `Mca.c:1628`)
- Returns array of `PEAK` structures with channel positions and energies
- `Mca_dedupPeaks()` removes duplicate peak detections

### ROI (Region of Interest) Processing

| Function | Header Line | Description |
|----------|-------------|-------------|
| `Mca_getEnergyLimits()` | `mca.h:127` | Calculate energy window around a peak |
| `Mca_getEndPoints()` | `mca.h:129` | Get start/end energy for ROI |
| `Mca_getROIChannels()` | `mca.h:130` | Convert energy limits to channel numbers |
| `Mca_drawROI()` | `mca.h:137` | Highlight ROI on spectrum display |

### Auto-Calibration

The auto-calibration system (`AUTOCAL` struct at `Headers/crc.h:456-477`) stores:
- Zero offset, gain1, gain2 settings
- Sigma (peak width) for Cs-137 661.66 keV
- Linearity measurements at 5 energy points: 32.9, 40.8, 121.8, 344.3, 661.7 keV
- Energy-to-channel and channel-to-energy conversion factors (`factorMtoS[6]`, `factorStoM[6]`)

### Background Management

| Function | Header Line | Description |
|----------|-------------|-------------|
| `Mca_clearBackgroundMirror()` | `mca.h:107` | Reset background spectrum for current detector |
| `Mca_clearBkgMirror()` | `mca.h:108` | Clear background mirror by detector type |
| `Mca_getBackgroundMirror()` | `mca.h:109` | Retrieve stored background for subtraction |

## API Reference

### Acquisition Control (`Headers/mca.h:75-81`)

| Function | Line | Description |
|----------|------|-------------|
| `Mca_setAcquireTime(short sec)` | `Mca.c:1603` | Set counting time (converted to centiseconds internally) |
| `Mca_refreshAcquireTime()` | `Mca.c:1619` | Reset/refresh acquire timer |
| `Mca_setTimeType(short type)` | `Mca.c:992` | Set COUNT_REAL or COUNT_LIVE mode |
| `Mca_startAcquisition()` | `Mca.c:1001` | Begin spectrum acquisition |
| `Mca_stopAcquisition()` | `Mca.c:980` | Halt acquisition |
| `Mca_getSpectrum()` | `Mca.c:1020` | Read 8192-channel spectrum from hardware |

### Peak Analysis

| Function | Header Line | Description |
|----------|-------------|-------------|
| `Mca_getPeaks(PEAK*, short*, bool)` | `mca.h:81` | Find peaks in current spectrum |
| `Mca_findPeak()` | `mca.h:128` | Find peak at specific energy in spectrum |
| `Mca_dedupPeaks()` | `mca.h:141` | Remove duplicate peak entries |
| `Mca_getEnergyLimits()` | `mca.h:127` | Calculate energy window for ROI |
| `Mca_getROIChannels()` | `mca.h:130` | Convert energy to channel range |

### Detector Control (`Headers/mca.h:153-157`)

| Function | Description |
|----------|-------------|
| `Mca_switchToWell()` | Activate well detector |
| `Mca_switchToProbe()` | Activate probe detector |
| `Mca_switchDetector(type)` | Switch to specified detector type |

### Calibration Timestamps (`Headers/mca.h:95-96`)

| Function | Description |
|----------|-------------|
| `Mca_setCalStamp()` | Record calibration timestamp |
| `Mca_setLinStamp()` | Record linearity check timestamp |

## Edge Cases

- **MCA not installed:** `ERR_MCA_NOT_INSTALLED` error (`Headers/err.h:2`) - system falls back to calibrator-only mode
- **Detector not attached:** `ERR_MCA_DETECTOR_NOT_ATTACHED` (`Headers/err.h:6`) - Well menu operations disabled
- **Command timeout:** `ERR_MCA_COMMAND_TIMEOUT` (`Headers/err.h:8`) - hardware communication failure
- **DSP program failure:** `ERR_MCA_PROG_LOAD_FAILURE` / `ERR_MCA_PROG_NOT_RUNNING` (`Headers/err.h:4-5`)
- **Simulation mode:** When `MCA_SIMULATION` is defined (`Headers/crc.h:16`), all hardware calls are replaced with simulated data. Initialization diverges at `CalibratorMain_R.c:334-338`
- **Channel count variability:** Detectors support 256, 512, 1024, 2048, or 4096 channels (`DETECTOR.num_of_channels` at `Headers/crc.h:487`)
- **Background subtraction:** Must call `Mca_getBackgroundMirror()` before measurement to get valid net counts; stale backgrounds produce incorrect results

## Dependencies

- **Hardware:** MCA card at memory-mapped address, I2C for detector EEPROM
- **Database:** `DB_WriteSpectrum()` / `DB_ReadSpectrum()` for persistent spectrum storage
- **Auto-cal data:** `AUTOCAL` struct loaded from database at startup
- **Error system:** `Headers/err.h` defines 7 MCA-specific error codes (lines 2-8)
- **Detector config:** `DETECTOR` struct (`Headers/crc.h:480`) loaded from EEPROM/database

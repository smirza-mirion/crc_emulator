enum detectors{
	DET_EMPTY,						// 0
    DET_WELL,						// 1
    DET_BETA,						// 2
    DET_PROBE700,					// 3
    DET_WELL700,					// 4
    DET_DRILLEDPROBE700,			// 5
    DET_DRILLEDPROBE700_AS_WELL,	// 6
    DET_DRILLEDPROBE700_AS_PROBE	// 7
};

enum counting_modes{
    COUNT_REAL,
    COUNT_LIVE,
};

typedef struct spec_meas SPEC_MEAS;
struct spec_meas
{
    ulong  spectrum[4096];	//spectrum after bundling channels
    uchar display[4096];	//normalized spectrum
    ushort raw[8192];		//spectrum from DSP
    bool  done_flag;		//measurement done
    float live_time;
    float real_time;
    float dead_time;
    float countdown_time;
    time_t stamp;
    float normal_factor;
    long max_count;
    bool  max_exceeded;     //max counts exceeded -- counting rate exceeds dead time factor
    long peaklevel;
    float averagecpm;
    int num_of_channels;
};

typedef struct specpeak SPECPEAK;
struct specpeak{
	float energy;
	float cpm;
	short nuclideID;
	float activity;
	short type;
	bool exceedthreshold;
};

typedef struct peak PEAK;
struct peak{
	float centroid;
	float height;
	float ratio;
	float ratiolocal;
};

typedef struct labcount LABCOUNT;
struct labcount{
	float measurement[2];
	float average;
	short startCh;
	short endCh;
	short countTime;
};

extern const short Mca_installedDetector;
extern SPEC_MEAS spec_meas;

int Mca_setup(void);
int Mca_setup_demo(void);
int Mca_setup_none(void);
short Mca_getZeroOpAmpOffset(void);
int Mca_setZeroOpAmpOffset(short value);
short Mca_getGain1(void);
short Mca_getGain2(void);
int Mca_setGain(short gain1, short gain2);
void Mca_setAcquireTime(short seconds);
void Mca_refreshAcquireTime(void);
void Mca_startAcquisition(void);
void Mca_getSpectrum(void);
bool Mca_presetReached(void);
void Mca_stopAcquisition(void);
void Mca_getPeaks(PEAK *peak, short *np1, bool AutoCal);
void Mca_saveZeroOpAmpOffsetToMirror(void);
void Mca_saveGainToMirror(void);
void Mca_saveSigmaToMirror(float sigma);
void Mca_saveSigmaLipToMirror(float sigma);
void Mca_saveCalibMirror(void);
void Mca_setMirrorToZeroOpAmpOffset(void);
void Mca_setMirrorToGain(void);
short Mca_getThreshold(void);
short Mca_getHVValue(void);
bool Mca_getHVState(void);
short Mca_getAcquireTime(void);
void Mca_setTimeType(short timetype);
short Mca_getTimeType(void);
void Mca_setCalStamp(void);
void Mca_setLinStamp(void);
float Mca_convertChToEnergy(float channel, AUTOCAL *autocal);
float Mca_convertChToEnergyWithoutCorrection(float channel, int numOfChannels);
float Mca_convertChToEnergyWithExplicitNumOfCh(float channel, AUTOCAL *autocal, int NumOfChannels);
float Mca_convertEnergyToCh(float keV, AUTOCAL *autocal);
float Mca_convertEnergyToChWithoutCorrection(float keV, int numOfChannels);
void Mca_setLinearityCorrection(float *measurement);
long long int Mca_getAutoCalID(int detectorType);
long long int Mca_getCurrentAutoCalID(void);
AUTOCAL *Mca_getCurrentAutoCal(void);
void MCA_clearBackgroundMirror(BACKGND *backgndPtr);
void Mca_clearBackgroundMirror(void);
void Mca_clearBkgMirror(int detectorType);
void *Mca_getBackgroundMirror(void);
long Mca_getBackgroundMirrorSize(void);
float Mca_getSigma(void);
float Mca_getSigmaLip(void);
bool Mca_getCalibrationStatus(void);
bool Mca_getCalibratedToday(void);
time_t Mca_getCalibrationStamp(void);
bool Mca_getBackgroundStatus(void);
bool Mca_getBackgroundToday(void);
time_t Mca_getBackgroundStamp(void);
void Mca_saveToBackgroundMirror(void);
float Mca_getBackgroundLiveTime(void);
bool Mca_existsAutoCal(void);
bool Mca_existsBackground(void);
unsigned long Mca_getROIBackgroundCounts(short startCh, short endCh, BACKGND *backgnd);
float Mca_getROIBackgroundCPM(short startCh, short endCh, BACKGND *backgnd);
float Mca_getAverageBackgroundCPM(BACKGND *backgnd);
ulong Mca_getROICounts(ulong spectrum[], short startCh, short endCh);
void Mca_getEnergyLimits(float energy, float *lowerLimit, float *upperLimit, int numOfChannels);
bool Mca_findPeak(ulong spectrum[], ENERGYPEAK energyPeak[], bool sigmaLip, AUTOCAL *autocal);
void Mca_getEndPoints(float energy, float *startEV, float *endEV, bool sigmaLip);
void Mca_getROIChannels(float startEV, float endEV, short *startCh, short *endCh, AUTOCAL *autocal);
void Mca_getROIChannelsWithoutCorrection(float startEV, float endEV, short *startCh, short *endCh, int numOfChannels);
float Mca_getCpm(ulong spectrum[], short startCh, short endCh, float liveSeconds, bool subtractBackground);
float Mca_convertCiToDpm(float Ci);
float Mca_convertDpmToCi(float dpm);
float Mca_convertBqToDpm(float Bq);
float Mca_convertDpmToBq(float dpm);
void Mca_drawROI(short startCh, short endCh, uchar display[], int num_of_channels);
float Mca_getFullEfficiency(void);
void Mca_updateFullEfficiency(float value);
void Mca_fullEfficiencyMirrorUpdate(void);
void Mca_dedupPeaks(PEAK detectpeak[], short secondregionstart, int numOfChannels);
void Mca_setDebugOffset(short offset);
DETECTOR *Mca_getDetectorMirrorPtr(void);
int Mca_getNumOfChannels(void);
int Mca_getAutoCalThreshold(void);
short Mca_getInitialGain1(void);
short Mca_getInitialGain2(void);
void Mca_loadUncorrectedSpectrum(ulong spectra[4096], AUTOCAL *autocal, int NumOfChannels);
float Mca_getCorrectedSpectra(float eV);
bool Mca_isMultiDetector(void);
bool Mca_hasWell(void);
bool Mca_hasProbe(void);
void Mca_switchToWell(void);
void Mca_switchToProbe(void);
short Mca_probe700Installed(void);
short Mca_well700Installed(void);
void Mca_switchDetector(short newDetector);
void Mca_saveCurrentDetector(void);
void Mca_sendDetectorType(void);
DETECTOR *Mca_detectorMirror(void);
DETECTOR *Mca_detectorProbe700Mirror(void);
DETECTOR *Mca_detectorWell700Mirror(void);

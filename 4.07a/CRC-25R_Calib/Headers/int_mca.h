
/*******************************************************************
  MODULE:   definition file for CapMCA
  
  FILE:     mca.h
  
  DATE:     08/02/07

********************************************************************/  
//Device Offsets
#define DSPRW           0x10
#define DSPADDRESS      0x12
#define DSPSYSCONTROL   0x14 // Bit7 = HV, Write 0x00 to reset DSP, Write 0x40 to release DSP
#define DSPID           0x1e


//DSP Internal Addresses
#define SW_CONTROL_REG  0x7fc0      //0x3fc0 or 0x4000, 0x4000 designates data memory
#define AVER_VAL_REG  SW_CONTROL_REG + 1
#define MEAS_TIME_LOW_REG  SW_CONTROL_REG + 2
#define MEAS_TIME_HIGH_REG  SW_CONTROL_REG + 3
#define CURRENT_RT_LOW_REG  SW_CONTROL_REG + 10 //0A
#define CURRENT_RT_HIGH_REG  SW_CONTROL_REG + 11 //0B
#define CURRENT_DT_LOW_REG  SW_CONTROL_REG + 12 //0C
#define CURRENT_DT_HIGH_REG  SW_CONTROL_REG + 13 //0D
#define COUNTDOWN_TIME_LOW_REG SW_CONTROL_REG + 14 //0E
#define COUNTDOWN_TIME_HIGH_REG SW_CONTROL_REG + 15 //0F

#define DAC_DATA_REG  SW_CONTROL_REG + 2
#define ZHT_DAC  0x24       //Zero, HV, Threshold
#define GAIN_DAC 0x14
#define WRITE_GAIN_A  0x1000    //load and update Dac A
#define WRITE_GAIN_B  0x4000    //load and update Dac B
#define WRITE_THRESH  0x0 		// Included with the DAC value to signal to the DAC chip, which channel
#define WRITE_HV      0x4000	// Included with the DAC value to signal to the DAC chip, which channel
#define WRITE_ZERO    0x8000	// Included with the DAC value to signal to the DAC chip, which channel

//nominal DAC value for 1000V
#define DAC1000         3800

enum detectors
{
	DET_EMPTY,
    DET_WELL,
    DET_BETA,
    DET_PROBE700,
    DET_WELL700,
    DET_DRILLEDPROBE700
};    

enum counting_modes
{
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

enum peak_type{
	NOT_FOUND,
	PRIMARY,
	SECONDARY,
	TERTIARY,
	SUMMED
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

typedef struct mca_data MCA_DATA;
struct mca_data
{
    char    name[6];        //CapMca
    char    sn[6];          //serial number
    short   thresh_off;     //threshold offset
    short   zero_off;       //zero offset
};

typedef struct board  BOARD;
struct board
{
    uchar date_str[8];      //date string in VB format
    short gain1;
    short gain2;
    short hv;
    float zero;
    float sigma;
};    

typedef struct peak PEAK;
struct peak{
	float centroid;
	float height;
	float ratio;
	float ratiolocal;
};

void MCASetAddress(short int sAddress);
unsigned short int MCAReadData(void);
static void MCAWriteData(unsigned short int usValue);
static int MCAWaitForFinish(void);
static ushort McaReadID(void);
static void McaReadData(void);
static int McaLoadDSP(void);
static int MCAClearChannels(uchar waitforfinish);
static ushort MCAGetDetector(void);
int MCASetHV(short value, bool state);
int MCASetThreshold(short value);
static void MCACountingFinished(void);
static void MCAGetTime(void);
static void MCAMaxCountsExceeded(void);


int Mca_setup(void);
void Mca_setTopHat(void);
int Mca_setZeroOpAmpOffset(short value);
int Mca_setGain(short gain1, short gain2);
void Mca_setAcquireTime(short seconds);
void Mca_startAcquisition(void);
void Mca_getSpectrum(void);
bool Mca_presetReached(void);
void Mca_stopAcquisition(void);
void Mca_setTimeType(short timetype);
short Mca_getTimeType(void);
short Mca_getHVValue(void);
bool Mca_getHVState(void);
short Mca_getThreshold(void);
short Mca_getZeroOpAmpOffset(void);
short Mca_getGain1(void);
short Mca_getGain2(void);
void Mca_getPeaks(PEAK *peak, short *np1, bool AutoCal);
void Mca_saveZeroOpAmpOffsetToMirror(void);
void Mca_saveGainToMirror(void);
void Mca_saveCalibMirror(void);
void Mca_setMirrorToZeroOpAmpOffset(void);
void Mca_setMirrorToGain(void);
short Mca_getAcquireTime(void);
void MCACalcLinFactors(AUTOCAL *autoCal);
float Mca_getLinearityMeasured(short index);
float Mca_getSigma(void);
float Mca_getSigmaLip(void);
void Mca_setDebugOffset(short offset);

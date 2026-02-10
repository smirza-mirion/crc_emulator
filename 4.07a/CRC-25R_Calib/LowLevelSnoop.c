#include <time.h>
#include "crc.h"
#include "lowlevelsnoop.h"
#include "sl811s.h"
#include "int_mca.h"

#define TYPE_ADC_READ			1
#define TYPE_ADC_ZERO_READ		2
#define TYPE_SERVICE_CHAMBER	3
#define TYPE_CHAMBER_VOLTS		4
#define TYPE_ISMEAS				5
#define TYPE_DISPLAY_ACTIVITY	6
#define TYPE_CHAMBER_VOLTS2		7
#define TYPE_ZERO				8

#define TYPE_ADC_READ__LADC								1
#define TYPE_ADC_ZERO_READ__LADCZERO					2
#define TYPE_SERVICE_CHAMBER__LCHAMBERMAX 				3
#define TYPE_SERVICE_CHAMBER__LCHAMBERMIN				4
#define TYPE_SERVICE_CHAMBER__LADCVALUE					5
#define TYPE_SERVICE_CHAMBER__LCHAMBERVALUE 			6
#define TYPE_SERVICE_CHAMBER__LADCZERO					7
#define TYPE_CHAMBER_VOLTS__FADCVALUE					8
#define TYPE_CHAMBER_VOLTS__FADCZERO					9
#define TYPE_CHAMBER_VOLTS__FAPPLIEDADCZERO				10
#define TYPE_CHAMBER_VOLTS__FCHAMBERZERO				11
#define TYPE_CHAMBER_VOLTS__FAPPLIEDCHAMBERZERO			12
#define TYPE_CHAMBER_VOLTS__FCHAMBERZEROCORR			13
#define TYPE_CHAMBER_VOLTS__FAPPLIEDCHAMBERZEROCORR		14
#define TYPE_CHAMBER_VOLTS__FAPPLIED18BIT				15
#define TYPE_CHAMBER_VOLTS__UCGAIN						16
#define TYPE_CHAMBER_VOLTS__FGAINFACTOR					17
#define TYPE_CHAMBER_VOLTS__FAPPLIEDGAIN				18
#define TYPE_CHAMBER_VOLTS__FAPPLIEDLINCOR				19
#define TYPE_CHAMBER_VOLTS__FCHAMBERBKG					20
#define TYPE_CHAMBER_VOLTS__FAPPLIEDBKG					21
#define TYPE_ISMEAS__UCLONGAVG							22
#define TYPE_ISMEAS__ISAMPLECOUNT						23
#define TYPE_ISMEAS__FAVERAGE							24
#define TYPE_DISPLAY_ACTIVITY__FVOLTS					25
#define TYPE_DISPLAY_ACTIVITY__FACTFACT					26
#define TYPE_DISPLAY_ACTIVITY__FRESPONSE				27
#define TYPE_DISPLAY_ACTIVITY__FACTIVITY				28
#define TYPE_DISPLAY_ACTIVITY__FCONTAINERFACTOR			29
#define TYPE_DISPLAY_ACTIVITY__FAPPLIEDCONTAINERFACTOR	30

#define LOW_LEVEL_BUF_SIZE 501

volatile unsigned char flgADCRead_lADC[8] = {0,0,0,0,0,0,0,0};
volatile long ADCRead_lADC[8];
volatile unsigned char flgADCZeroRead_lADCZero[8] = {0,0,0,0,0,0,0,0};
volatile long ADCZeroRead_lADCZero[8];
volatile unsigned char flgServiceChamber_lChamberMax[8] = {0,0,0,0,0,0,0,0};
volatile long ServiceChamber_lChamberMax[8];
volatile unsigned char flgServiceChamber_lChamberMin[8] = {0,0,0,0,0,0,0,0};
volatile long ServiceChamber_lChamberMin[8];
volatile unsigned char flgServiceChamber_lADCValue[8] = {0,0,0,0,0,0,0,0};
volatile long ServiceChamber_lADCValue[8];
volatile unsigned char flgServiceChamber_lChamberValue[8] = {0,0,0,0,0,0,0,0};
volatile long ServiceChamber_lChamberValue[8];
volatile unsigned char flgServiceChamber_lADCZero[8] = {0,0,0,0,0,0,0,0};
volatile long ServiceChamber_lADCZero[8];
volatile unsigned char flgChamberVolts_fADCValue[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fADCValue[8];
volatile unsigned char flgChamberVolts_fADCZero[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fADCZero[8];
volatile unsigned char flgChamberVolts_fAppliedADCZero[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fAppliedADCZero[8];
volatile unsigned char flgChamberVolts_fChamberZero[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fChamberZero[8];
volatile unsigned char flgChamberVolts_fAppliedChamberZero[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fAppliedChamberZero[8];
volatile unsigned char flgChamberVolts_fChamberZeroCorr[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fChamberZeroCorr[8];
volatile unsigned char flgChamberVolts_fAppliedChamberZeroCorr[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fAppliedChamberZeroCorr[8];
volatile unsigned char flgChamberVolts_fApplied18Bit[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fApplied18Bit[8];
volatile unsigned char flgChamberVolts_ucGain[8] = {0,0,0,0,0,0,0,0};
volatile unsigned char ChamberVolts_ucGain[8];
volatile unsigned char flgChamberVolts_fGainFactor[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fGainFactor[8];
volatile unsigned char flgChamberVolts_fAppliedGain[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fAppliedGain[8];
volatile unsigned char flgChamberVolts_fAppliedLinCor[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fAppliedLinCor[8];
volatile unsigned char flgChamberVolts_fChamberBkg[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fChamberBkg[8];
volatile unsigned char flgChamberVolts_fAppliedChamberBkg[8] = {0,0,0,0,0,0,0,0};
volatile float ChamberVolts_fAppliedChamberBkg[8];
volatile unsigned char flgIsMeas_ucLongAvg[8] = {0,0,0,0,0,0,0,0};
volatile unsigned char IsMeas_ucLongAvg[8];
volatile unsigned char flgIsMeas_iSampleCount[8] = {0,0,0,0,0,0,0,0};
volatile short int IsMeas_iSampleCount[8];
volatile unsigned char flgIsMeas_fAverage[8] = {0,0,0,0,0,0,0,0};
volatile float IsMeas_fAverage[8];
volatile unsigned char flgDisplayActivity_fVolts[9] = {0,0,0,0,0,0,0,0,0};
volatile float DisplayActivity_fVolts[9];
volatile unsigned char flgDisplayActivity_fActFact[9] = {0,0,0,0,0,0,0,0,0};
volatile float DisplayActivity_fActFact[9];
volatile unsigned char flgDisplayActivity_fResponse[9] = {0,0,0,0,0,0,0,0,0};
volatile float DisplayActivity_fResponse[9];
volatile unsigned char flgDisplayActivity_fActivity[9] = {0,0,0,0,0,0,0,0,0};
volatile float DisplayActivity_fActivity[9];
volatile unsigned char flgDisplayActivity_fContainerFactor[9] = {0,0,0,0,0,0,0,0,0};
volatile float DisplayActivity_fContainerFactor[9];
volatile unsigned char flgDisplayActivity_fAppliedContainerFactor[9] = {0,0,0,0,0,0,0,0,0};
volatile float DisplayActivity_fAppliedContainerFactor[9];

unsigned char ucLowLevelBuf[LOW_LEVEL_BUF_SIZE][63];
unsigned long ulSeqNum;
unsigned long ulLowLevelHead;
unsigned long ulLowLevelTail;
unsigned char ucSeqNum;
ulong LowLevelSnoop_timeout=0;
bool LowLevelSnoop_WellSwitch = FALSE;

typedef struct{
	unsigned long ulSeqNum;
	unsigned char ucYear;
	unsigned char ucMonth;
	unsigned char ucDay;
	unsigned char ucHour;
	unsigned char ucMinute;
	unsigned char ucSecond;
	unsigned short int uiType;
	unsigned char ucData[50];
} LowLevelStruct;

time_t low_clock_time;
extern bool clear_to_send1;
//extern SPEC_MEAS spec_meas;
static unsigned char ConvertToBCD(short int number);
static void ClearData(unsigned char *data);
static unsigned long InsertDoubleWord(unsigned long ulCurrent, unsigned char *ucData, unsigned char *ulADC);
static unsigned long InsertWord(unsigned long ulCurrent, unsigned char *ucData, unsigned char *uiADC);
static unsigned long InsertByte(unsigned long ulCurrent, unsigned char *ucData, unsigned char *ucADC);
static void PushLowLevel(unsigned short int uiType, unsigned char ucData[50]);
static void PushLowLevelWell(ushort type,  unsigned char data[60]);
int PopLowLevel(unsigned char *data);

void ClearLowLevel(void){
	short index;

	ulLowLevelHead = 0;
	ulLowLevelTail = 0;
	ulSeqNum = 0;
	for(index=0; index<8; index++) {
		flgADCRead_lADC[index] = 0;
		flgADCZeroRead_lADCZero[index] = 0;
		flgServiceChamber_lChamberMax[index] = 0;
		flgServiceChamber_lChamberMin[index] = 0;
		flgServiceChamber_lADCValue[index] = 0;
		flgServiceChamber_lChamberValue[index] = 0;
		flgServiceChamber_lADCZero[index] = 0;
		flgChamberVolts_fADCValue[index] = 0;
		flgChamberVolts_fADCZero[index] = 0;
		flgChamberVolts_fAppliedADCZero[index] = 0;
		flgChamberVolts_fChamberZero[index] = 0;
		flgChamberVolts_fAppliedChamberZero[index] = 0;
		flgChamberVolts_fChamberZeroCorr[index] = 0;
		flgChamberVolts_fAppliedChamberZeroCorr[index] = 0;
		flgChamberVolts_fApplied18Bit[index] = 0;
		flgChamberVolts_ucGain[index] = 0;
		flgChamberVolts_fGainFactor[index] = 0;
		flgChamberVolts_fAppliedGain[index] = 0;
		flgChamberVolts_fAppliedLinCor[index] = 0;
		flgChamberVolts_fChamberBkg[index] = 0;
		flgChamberVolts_fAppliedChamberBkg[index] = 0;
		flgIsMeas_ucLongAvg[index] = 0;
		flgIsMeas_iSampleCount[index] = 0;
		flgIsMeas_fAverage[index] = 0;
		flgDisplayActivity_fVolts[index] = 0;
		flgDisplayActivity_fActFact[index] = 0;
		flgDisplayActivity_fResponse[index] = 0;
		flgDisplayActivity_fActivity[index] = 0;
		flgDisplayActivity_fContainerFactor[index] = 0;
		flgDisplayActivity_fAppliedContainerFactor[index] = 0;
	}
}

void PushADCRead(unsigned char ucChamber, long lADC){
	unsigned char ucData[50];
	unsigned long ulCurrent;

	ClearData(ucData);
	ulCurrent = 0;
	ulCurrent = InsertByte(ulCurrent, ucData, &ucChamber);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *)&lADC);

	PushLowLevel(TYPE_ADC_READ, ucData);
}

void PushADCZero(unsigned char ucChamber, long int lADCZero){
	unsigned char ucData[50];
	unsigned long ulCurrent;

	ClearData(ucData);
	ulCurrent = 0;
	ulCurrent = InsertByte(ulCurrent, ucData, &ucChamber);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *)&lADCZero);

	PushLowLevel(TYPE_ADC_ZERO_READ, ucData);
}

void PushServiceChamber(unsigned char ucChamber, long lChamberMax, long lChamberMin, unsigned char ucMinFlag, unsigned char ucMaxFlag, long lADCValue, long lChamberValue, long lADCZero){
	unsigned char ucData[50];
	unsigned long ulCurrent;

	ClearData(ucData);
	ulCurrent = 0;
	ulCurrent = InsertByte(ulCurrent, ucData, &ucChamber);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &lChamberMax);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &lChamberMin);
	ulCurrent = InsertByte(ulCurrent, ucData, &ucMinFlag);
	ulCurrent = InsertByte(ulCurrent, ucData, &ucMaxFlag);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &lADCValue);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &lChamberValue);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &lADCZero);

	PushLowLevel(TYPE_SERVICE_CHAMBER, ucData);
}

void PushChamberVolts(unsigned char ucChamber, float fADCValue, float fADCZero, float fAppliedADCZero, float fChamberZero, float fAppliedChamberZero, float fChamberZeroCorr, float fChamberZeroCorrApplied, float fApplied18Bit, unsigned char ucGain, float fGainFactor, float fAppliedGain, float fAppliedLinCor, float fChamberBkg){
	unsigned char ucData[50];
	unsigned long ulCurrent;

	ClearData(ucData);
	ulCurrent = 0;
	ulCurrent = InsertByte(ulCurrent, ucData, &ucChamber);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fADCValue);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fADCZero);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fAppliedADCZero);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fChamberZero);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fAppliedChamberZero);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fChamberZeroCorr);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fChamberZeroCorrApplied);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fApplied18Bit);
	ulCurrent = InsertByte(ulCurrent, ucData, &ucGain);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fGainFactor);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fAppliedGain);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fAppliedLinCor);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fChamberBkg);
	PushLowLevel(TYPE_CHAMBER_VOLTS, ucData);
}

void PushChamberVolts2(unsigned char ucChamber, float fAppliedBkg){
	unsigned char ucData[50];
	unsigned long ulCurrent;

	ClearData(ucData);
	ulCurrent = 0;
	ulCurrent = InsertByte(ulCurrent, ucData, &ucChamber);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fAppliedBkg);
	PushLowLevel(TYPE_CHAMBER_VOLTS2, ucData);
}

void PushIsMeas(unsigned char ucChamber, unsigned char ucLongAvg, short int iSampleCount, float fAverage, float fAverageLinear, float fAverageSquare, float fAverageCube, unsigned char ucWeighted){
	unsigned char ucData[50];
	unsigned long ulCurrent;

	ClearData(ucData);
	ulCurrent = 0;
	ulCurrent = InsertByte(ulCurrent, ucData, &ucChamber);
	ulCurrent = InsertByte(ulCurrent, ucData, &ucLongAvg);
	ulCurrent = InsertWord(ulCurrent, ucData, (unsigned char *) &iSampleCount);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fAverage);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fAverageLinear);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fAverageSquare);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fAverageCube);
	ulCurrent = InsertByte(ulCurrent, ucData, &ucWeighted);
	PushLowLevel(TYPE_ISMEAS, ucData);
}

void PushDisplayActivity(unsigned char ucChamber, float fVolts, float fActFact, float fResponse, float fActivity, float fContainerFactor, float fAppliedContainerFactor){
	unsigned char ucData[50];
	unsigned long ulCurrent;

	ClearData(ucData);
	ulCurrent = 0;
	ulCurrent = InsertByte(ulCurrent, ucData, &ucChamber);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fVolts);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fActFact);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fResponse);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fActivity);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fContainerFactor);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fAppliedContainerFactor);

	PushLowLevel(TYPE_DISPLAY_ACTIVITY, ucData);
}

void PushZero(unsigned char ucChamber, float fOpAmpZeroElement)
{
	unsigned char ucData[50];
	unsigned long ulCurrent;

	ClearData(ucData);
	ulCurrent = 0;
	ulCurrent = InsertByte(ulCurrent, ucData, &ucChamber);
	ulCurrent = InsertDoubleWord(ulCurrent, ucData, (unsigned char *) &fOpAmpZeroElement);

	PushLowLevel(TYPE_ZERO, ucData);
}

void SendLowLevelChamber(void) {
	unsigned char data[64];
	unsigned char acc;
	unsigned int i;

	unsigned short int *uiPtr;
	short int *iPtr;
	long *lPtr;
	float *fPtr;

	unsigned short int uiCommand;
	unsigned char ucSwitch;
	unsigned char ucValue;
	char cValue;
	//unsigned short int uiValue;
	short int iValue;
	long lValue;
	float fValue;

	if (is_usb_char_waiting()) {
		USB_Get_Data(data);

		acc = 0;

		// valid packets are prefixed with "WL"
		if ((data[0] == 'W') && (data[1] == 'L')) {
			for (i = 0; i < 64; i++) acc = acc + data[i];

			// checksum test
			if (acc == 0) {
				uiPtr = (unsigned short int *) &(data[2]);
				uiCommand = *uiPtr;

				ucSwitch = data[4];

				ucValue = data[5];

				cValue = (char) data[6];

				uiPtr = (unsigned short int *) &(data[7]);
				//uiValue = *uiPtr;

				iPtr = (short int *) &(data[9]);
				iValue = *iPtr;

				lPtr = (long *) &(data[11]);
				lValue = *lPtr;

				fPtr = (float *) &(data[15]);
				fValue = *fPtr;

				switch(uiCommand) {
					case TYPE_ADC_READ__LADC:
						if (ucSwitch == 0) {
							flgADCRead_lADC[cValue] = 0;
						}
						else {
							flgADCRead_lADC[cValue] = 0xFF;
							ADCRead_lADC[cValue] = lValue;
						}
						break;

					case TYPE_ADC_ZERO_READ__LADCZERO:
						if (ucSwitch == 0){
							flgADCZeroRead_lADCZero[cValue] = 0;
						}
						else {
							flgADCZeroRead_lADCZero[cValue] = 0xFF;
							ADCZeroRead_lADCZero[cValue] = lValue;
						}
						break;

					case TYPE_SERVICE_CHAMBER__LCHAMBERMAX:
						if (ucSwitch == 0){
							flgServiceChamber_lChamberMax[cValue] = 0;
						}
						else {
							flgServiceChamber_lChamberMax[cValue] = 0xFF;
							ServiceChamber_lChamberMax[cValue] = lValue;
						}
						break;

					case TYPE_SERVICE_CHAMBER__LCHAMBERMIN:
						if (ucSwitch == 0){
							flgServiceChamber_lChamberMin[cValue] = 0;
						}
						else {
							flgServiceChamber_lChamberMin[cValue] = 0xFF;
							ServiceChamber_lChamberMin[cValue] = lValue;
						}
						break;

					case TYPE_SERVICE_CHAMBER__LADCVALUE:
						if (ucSwitch == 0){
							flgServiceChamber_lADCValue[cValue] = 0;
						}
						else {
							flgServiceChamber_lADCValue[cValue] = 0xFF;
							ServiceChamber_lADCValue[cValue] = lValue;
						}
						break;

					case TYPE_SERVICE_CHAMBER__LCHAMBERVALUE:
						if (ucSwitch == 0){
							flgServiceChamber_lChamberValue[cValue] = 0;
						}
						else {
							flgServiceChamber_lChamberValue[cValue] = 0xFF;
							ServiceChamber_lChamberValue[cValue] = lValue;
						}
						break;

					case TYPE_SERVICE_CHAMBER__LADCZERO:
						if (ucSwitch == 0){
							flgServiceChamber_lADCZero[cValue] = 0;
						}
						else {
							flgServiceChamber_lADCZero[cValue] = 0xFF;
							ServiceChamber_lADCZero[cValue] = lValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FADCVALUE:
						if (ucSwitch == 0){
							flgChamberVolts_fADCValue[cValue] = 0;
						}
						else {
							flgChamberVolts_fADCValue[cValue] = 0xFF;
							ChamberVolts_fADCValue[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FADCZERO:
						if (ucSwitch == 0){
							flgChamberVolts_fADCZero[cValue] = 0;
						}
						else {
							flgChamberVolts_fADCZero[cValue] = 0xFF;
							ChamberVolts_fADCZero[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FAPPLIEDADCZERO:
						if (ucSwitch == 0){
							flgChamberVolts_fAppliedADCZero[cValue] = 0;
						}
						else {
							flgChamberVolts_fAppliedADCZero[cValue] = 0xFF;
							ChamberVolts_fAppliedADCZero[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FCHAMBERZERO:
						if (ucSwitch == 0){
							flgChamberVolts_fChamberZero[cValue] = 0;
						}
						else {
							flgChamberVolts_fChamberZero[cValue] = 0xFF;
							ChamberVolts_fChamberZero[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FAPPLIEDCHAMBERZERO:
						if (ucSwitch == 0){
							flgChamberVolts_fAppliedChamberZero[cValue] = 0;
						}
						else {
							flgChamberVolts_fAppliedChamberZero[cValue] = 0xFF;
							ChamberVolts_fAppliedChamberZero[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FCHAMBERZEROCORR:
						if (ucSwitch == 0){
							flgChamberVolts_fChamberZeroCorr[cValue] = 0;
						}
						else {
							flgChamberVolts_fChamberZeroCorr[cValue] = 0xFF;
							ChamberVolts_fChamberZeroCorr[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FAPPLIEDCHAMBERZEROCORR:
						if (ucSwitch == 0) {
							flgChamberVolts_fAppliedChamberZeroCorr[cValue] = 0;
						}
						else {
							flgChamberVolts_fAppliedChamberZeroCorr[cValue] = 0xFF;
							ChamberVolts_fAppliedChamberZeroCorr[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FAPPLIED18BIT:
						if (ucSwitch == 0){
							flgChamberVolts_fApplied18Bit[cValue] = 0;
						}
						else {
							flgChamberVolts_fApplied18Bit[cValue] = 0xFF;
							ChamberVolts_fApplied18Bit[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__UCGAIN:
						if (ucSwitch == 0){
							flgChamberVolts_ucGain[cValue] = 0;
						}
						else {
							flgChamberVolts_ucGain[cValue] = 0xFF;
							ChamberVolts_ucGain[cValue] = ucValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FGAINFACTOR:
						if (ucSwitch == 0){
							flgChamberVolts_fGainFactor[cValue] = 0;
						}
						else {
							flgChamberVolts_fGainFactor[cValue] = 0xFF;
							ChamberVolts_fGainFactor[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FAPPLIEDGAIN:
						if (ucSwitch == 0){
							flgChamberVolts_fAppliedGain[cValue] = 0;
						}
						else {
							flgChamberVolts_fAppliedGain[cValue] = 0xFF;
							ChamberVolts_fAppliedGain[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FAPPLIEDLINCOR:
						if (ucSwitch == 0){
							flgChamberVolts_fAppliedLinCor[cValue] = 0;
						}
						else {
							flgChamberVolts_fAppliedLinCor[cValue] = 0xFF;
							ChamberVolts_fAppliedLinCor[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FCHAMBERBKG:
						if (ucSwitch == 0){
							flgChamberVolts_fChamberBkg[cValue] = 0;
						}
						else {
							flgChamberVolts_fChamberBkg[cValue] = 0xFF;
							ChamberVolts_fChamberBkg[cValue] = fValue;
						}
						break;

					case TYPE_CHAMBER_VOLTS__FAPPLIEDBKG:
						if (ucSwitch == 0){
							flgChamberVolts_fAppliedChamberBkg[cValue] = 0;
						}
						else {
							flgChamberVolts_fAppliedChamberBkg[cValue] = 0xFF;
							ChamberVolts_fAppliedChamberBkg[cValue] = fValue;
						}
						break;

					case TYPE_ISMEAS__UCLONGAVG:
						if (ucSwitch == 0){
							flgIsMeas_ucLongAvg[cValue] = 0;
						}
						else {
							flgIsMeas_ucLongAvg[cValue] = 0xFF;
							IsMeas_ucLongAvg[cValue] = ucValue;
						}
						break;

					case TYPE_ISMEAS__ISAMPLECOUNT:
						if (ucSwitch == 0){
							flgIsMeas_iSampleCount[cValue] = 0;
						}
						else {
							flgIsMeas_iSampleCount[cValue] = 0xFF;
							IsMeas_iSampleCount[cValue] = iValue;
						}
						break;

					case TYPE_ISMEAS__FAVERAGE:
						if (ucSwitch == 0){
							flgIsMeas_fAverage[cValue] = 0;
						}
						else {
							flgIsMeas_fAverage[cValue] = 0xFF;
							IsMeas_fAverage[cValue] = fValue;
						}
						break;

					case TYPE_DISPLAY_ACTIVITY__FVOLTS:
						if (ucSwitch == 0){
							flgDisplayActivity_fVolts[cValue] = 0;
						}
						else {
							flgDisplayActivity_fVolts[cValue] = 0xFF;
							DisplayActivity_fVolts[cValue] = fValue;
						}
						break;

					case TYPE_DISPLAY_ACTIVITY__FACTFACT:
						if (ucSwitch == 0){
							flgDisplayActivity_fActFact[cValue] = 0;
						}
						else {
							flgDisplayActivity_fActFact[cValue] = 0xFF;
							DisplayActivity_fActFact[cValue] = fValue;
						}
						break;

					case TYPE_DISPLAY_ACTIVITY__FRESPONSE:
						if (ucSwitch == 0){
							flgDisplayActivity_fResponse[cValue] = 0;
						}
						else {
							flgDisplayActivity_fResponse[cValue] = 0xFF;
							DisplayActivity_fResponse[cValue] = fValue;
						}
						break;

					case TYPE_DISPLAY_ACTIVITY__FACTIVITY:
						if (ucSwitch == 0){
							flgDisplayActivity_fActivity[cValue] = 0;
						}
						else {
							flgDisplayActivity_fActivity[cValue] = 0xFF;
							DisplayActivity_fActivity[cValue] = fValue;
						}
						break;

					case TYPE_DISPLAY_ACTIVITY__FCONTAINERFACTOR:
						if (ucSwitch == 0){
							flgDisplayActivity_fContainerFactor[cValue] = 0;
						}
						else {
							flgDisplayActivity_fContainerFactor[cValue] = 0xFF;
							DisplayActivity_fContainerFactor[cValue] = fValue;
						}
						break;

					case TYPE_DISPLAY_ACTIVITY__FAPPLIEDCONTAINERFACTOR:
						if (ucSwitch == 0){
							flgDisplayActivity_fAppliedContainerFactor[cValue] = 0;
						}
						else {
							flgDisplayActivity_fAppliedContainerFactor[cValue] = 0xFF;
							DisplayActivity_fAppliedContainerFactor[cValue] = fValue;
						}
						break;

					default:
						break;
				}
			}
		}
	}

	if (clear_to_send1) {
		if (PopLowLevel(data)) {
			USB_Send_Data_LowLevel_num(data, 63);
		}
	}
}

static void dec(long *lLowLevelHead){
	long ptr;

	ptr = *lLowLevelHead;
	ptr--;
	if(ptr < 0) ptr = LOW_LEVEL_BUF_SIZE - 1;
	*lLowLevelHead = ptr;
}

void SendLowLevelWell(void){
	unsigned char data[64];
	unsigned char resend_seqnum;
	unsigned char resend_type;
	long lLowLevelHead;
	long counter;
	short index;
	ushort type;
	unsigned char *ptr;

	if(is_usb_char_waiting()){
		USB_Get_Data(data);
		if(data[0] == 'W'){
			if(data[1] == 'L'){
				resend_seqnum = data[2];
				lLowLevelHead = ulLowLevelHead;
				dec(&lLowLevelHead);
				counter = 0;
				while((counter<=LOW_LEVEL_BUF_SIZE) && (ucLowLevelBuf[lLowLevelHead][0] != resend_seqnum)){
					dec(&lLowLevelHead);
					counter++;
				}

				if(ucLowLevelBuf[lLowLevelHead][0] == resend_seqnum){
					for(index=0; index<60; index++) data[index] = ucLowLevelBuf[lLowLevelHead][index+3];
					ptr = (unsigned char *) &type;
					*ptr = ucLowLevelBuf[lLowLevelHead][2];
					ptr++;
					*ptr = ucLowLevelBuf[lLowLevelHead][1];
					PushLowLevelWell(type, data);
					//printf("Resending Seq: %d, lType: %d\n", resend_seqnum, type);
				}
			}

			if(data[1] == 'M'){
				resend_type = data[2];
				lLowLevelHead = ulLowLevelHead;
				dec(&lLowLevelHead);
				counter = 0;
				while((counter<=LOW_LEVEL_BUF_SIZE) && (ucLowLevelBuf[lLowLevelHead][1] != resend_type)){
					dec(&lLowLevelHead);
					counter++;
				}

				if(ucLowLevelBuf[lLowLevelHead][1] == resend_type){
					for(index=0; index<60; index++) data[index] = ucLowLevelBuf[lLowLevelHead][index+3];
					ptr = (unsigned char *) &type;
					*ptr = ucLowLevelBuf[lLowLevelHead][2];
					ptr++;
					*ptr = ucLowLevelBuf[lLowLevelHead][1];
					PushLowLevelWell(type, data);
					//printf("Resending lType: %d\n", type);
				}
			}
		}
	}

	if(LowLevelSnoop_WellSwitch){
		//if(g_csec_tstamp >= LowLevelSnoop_timeout){
			if(clear_to_send1){
				if(PopLowLevel(data)){
					//printf("Sending: %d\n", data[1]);
					USB_Send_Data_LowLevel_num(data, 63);
				}
			}
			//LowLevelSnoop_timeout = g_csec_tstamp + 10;
			//LowLevelSnoop_timeout = 0;
		//}
	}
}

int PopLowLevel(unsigned char *data){
	int i;
	int iReturn;
	unsigned char *ucFrom;

	if (ulLowLevelHead == ulLowLevelTail) iReturn = 0;
	else {
		ucFrom = &(ucLowLevelBuf[ulLowLevelHead][0]);
		for (i = 0; i < 63; i++) {
			*data = *ucFrom;
			data++;
			ucFrom++;
		}

		ulLowLevelHead++;
		if (ulLowLevelHead == LOW_LEVEL_BUF_SIZE) ulLowLevelHead = 0;
		iReturn = -1;
	}
	return iReturn;
}


static void PushLowLevel(unsigned short int uiType, unsigned char ucData[50]){
	LowLevelStruct sLowLevel;
	struct tm now;
	unsigned long i;
	unsigned char *ucFrom, *ucTo;

	sLowLevel.ulSeqNum = ulSeqNum++;
	gmtime_r(&low_clock_time, &now);
	sLowLevel.ucYear = ConvertToBCD((now.tm_year) - 100);
	sLowLevel.ucMonth = ConvertToBCD((now.tm_mon) + 1);
	sLowLevel.ucDay = ConvertToBCD(now.tm_mday);
	sLowLevel.ucHour = ConvertToBCD(now.tm_hour);
	sLowLevel.ucMinute = ConvertToBCD(now.tm_min);
	sLowLevel.ucSecond = ConvertToBCD(now.tm_sec);
	sLowLevel.uiType = uiType;

	for (i=0; i<51; i++) sLowLevel.ucData[i] = ucData[i];

	ucFrom = (unsigned char *) &sLowLevel;
	ucTo = &(ucLowLevelBuf[ulLowLevelTail][0]);

	for (i=0; i<63; i++){
		*ucTo = *ucFrom;
		ucTo++;
		ucFrom++;
	}

	ulLowLevelTail++;
	if (ulLowLevelTail == LOW_LEVEL_BUF_SIZE) ulLowLevelTail = 0;
	if (ulLowLevelTail == ulLowLevelHead){
		ulLowLevelHead++;
		if (ulLowLevelHead == LOW_LEVEL_BUF_SIZE) ulLowLevelHead = 0;
	}
}

static void PushLowLevelWell(ushort type,  unsigned char data[60]){
	uchar *ucTo;
	unsigned long ulCurrent;
	short index;

	ucTo = &(ucLowLevelBuf[ulLowLevelTail][0]);

	ulCurrent = 0;
	*ucTo = ucSeqNum++;

	ulCurrent = 1;
	ulCurrent = InsertWord(ulCurrent, ucTo, (unsigned char *) &type);

	for(index=0; index<60; index++){
		ucLowLevelBuf[ulLowLevelTail][index+3] = data[index];
	}
	ulLowLevelTail++;

	if (ulLowLevelTail == LOW_LEVEL_BUF_SIZE) ulLowLevelTail = 0;
	if (ulLowLevelTail == ulLowLevelHead){
		ulLowLevelHead++;
		if (ulLowLevelHead == LOW_LEVEL_BUF_SIZE) ulLowLevelHead = 0;
	}
}

/*void PushSpectrum(void){
	unsigned char data[60];
	short index, jndex;
	unsigned long ulCurrent;
	short HVValue;
	short Threshold;
	short Zero;
	short Gain1, Gain2;
	float Sigma, linmeasured;

	ulLowLevelHead = 0;
	ulLowLevelTail = 0;
	LowLevelSnoop_WellSwitch = FALSE;

	for(index=0; index<17; index++){
		ulCurrent = 0;
		for(jndex=0; jndex<15; jndex++){
			ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(spec_meas.spectrum[15*index + jndex]));
		}
		PushLowLevelWell(index+1, data);
	}

	for(index=0; index<60; index++) data[index] = 0;
	ulCurrent = 0;
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(spec_meas.spectrum[255])); //4
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &spec_meas.live_time); // 8
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &spec_meas.averagecpm); // 12
	HVValue = Mca_getHVValue();
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &HVValue); // 14
	Threshold = Mca_getThreshold();
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &Threshold); // 16
	Zero = Mca_getZeroOpAmpOffset();
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &Zero); // 18
	Gain1 = Mca_getGain1();
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &Gain1); // 20
	Gain2 = Mca_getGain2();
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &Gain2); // 22
	Sigma = Mca_getSigma();
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &Sigma); // 26
	linmeasured = Mca_getLinearityMeasured(0);
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &linmeasured); // 30
	linmeasured = Mca_getLinearityMeasured(1);
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &linmeasured); // 34
	linmeasured = Mca_getLinearityMeasured(2);
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &linmeasured); // 38
	linmeasured = Mca_getLinearityMeasured(3);
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &linmeasured); // 42
	linmeasured = Mca_getLinearityMeasured(4);
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &linmeasured); // 46

	PushLowLevelWell(18, data);
} */

/*void PushEnergyPeak(ENERGYPEAK energyPeak[10]){
	unsigned char data[60];
	short index;
	unsigned long ulCurrent;

	ulCurrent = 0;
	for(index=0; index<3; index++){
		ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].energy));
		ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].lowerLimit));
		ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].upperLimit));
		ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].startCh));
		ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].endCh));
		ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].found));
	}
	PushLowLevelWell(19, data);

	ulCurrent = 0;
	for(index=3; index<6; index++){
		ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].energy));
		ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].lowerLimit));
		ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].upperLimit));
		ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].startCh));
		ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].endCh));
		ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].found));
	}
	PushLowLevelWell(20, data);

	ulCurrent = 0;
	for(index=6; index<9; index++){
		ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].energy));
		ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].lowerLimit));
		ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].upperLimit));
		ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].startCh));
		ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].endCh));
		ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[index].found));
	}
	PushLowLevelWell(21, data);

	ulCurrent = 0;
	index = 9;
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[9].energy));
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[9].lowerLimit));
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(energyPeak[9].upperLimit));
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[9].startCh));
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[9].endCh));
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(energyPeak[9].found));
	PushLowLevelWell(22, data);
}*/

/*void PushBackground(void){
	BKG_MEAS *copy;
	short index, jndex;
	unsigned long ulCurrent;
	unsigned char data[60];

	copy = (BKG_MEAS *) Mca_getBackgroundMirror();
	for(index=0; index<17; index++){
		ulCurrent = 0;
		for(jndex=0; jndex<15; jndex++){
			ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(copy->spectrum[15*index + jndex]));
		}
		PushLowLevelWell(index+23, data);
	}

	for(index=0; index<60; index++) data[index] = 0;
	ulCurrent = 0;
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(copy->spectrum[255]));
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(copy->liveTime));
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(copy->cpm));
	PushLowLevelWell(40, data);
}*/

void PushSpecPeak(SPECPEAK specpeak[40]){
	short index, jndex;
	ulong ulCurrent;
	unsigned char data[60];

	for(index=0; index<13; index++){
		ulCurrent = 0;
		for(jndex=0; jndex<3; jndex++){
			ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(specpeak[3*index + jndex].energy));
			ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(specpeak[3*index + jndex].cpm));
			ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(specpeak[3*index + jndex].nuclideID));
			ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(specpeak[3*index + jndex].activity));
			ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(specpeak[3*index + jndex].type));
			ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(specpeak[3*index + jndex].exceedthreshold));
		}
		PushLowLevelWell(41 + index, data);
	}

	ulCurrent = 0;
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(specpeak[39].energy));
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(specpeak[39].cpm));
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(specpeak[39].nuclideID));
	ulCurrent = InsertDoubleWord(ulCurrent, data, (unsigned char *) &(specpeak[39].activity));
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(specpeak[39].type));
	ulCurrent = InsertWord(ulCurrent, data, (unsigned char *) &(specpeak[39].exceedthreshold));

	PushLowLevelWell(54, data);
}

static unsigned char ConvertToBCD(short int number){
	unsigned char ucReturn;
	short int iDecimal;

	ucReturn = 0;

	iDecimal = number / 10;

	if (iDecimal > 0){
		ucReturn = 16 * iDecimal;
	}

	iDecimal = number % 10;
	ucReturn = ucReturn + iDecimal;

	return ucReturn;
}

static void ClearData(unsigned char *data){
	unsigned long i;

	for (i = 0; i<51; i++){
		*data = 0;
		data++;
	}
}

static unsigned long InsertDoubleWord(unsigned long ulCurrent, unsigned char *ucData, unsigned char *ulADC){
	ucData = ucData + ulCurrent;
	*ucData = *(ulADC + 3);
	ucData++;
	*ucData = *(ulADC + 2);
	ucData++;
	*ucData = *(ulADC + 1);
	ucData++;
	*ucData = *ulADC;
	return ulCurrent + 4;
}

static unsigned long InsertWord(unsigned long ulCurrent, unsigned char *ucData, unsigned char *uiADC){
	ucData = ucData + ulCurrent;
	*ucData = *(uiADC + 1);
	ucData++;
	*ucData = *uiADC;
	return ulCurrent + 2;
}

static unsigned long InsertByte(unsigned long ulCurrent, unsigned char *ucData, unsigned char *ucADC){
	ucData = ucData + ulCurrent;
	*ucData = *ucADC;
	return ulCurrent + 1;
}


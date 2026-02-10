#ifndef _SQLITE3_H_
#include "sqlite3.h"
#endif

typedef struct DateTime DateTime;
struct DateTime {
  sqlite3_int64 iJD; /* The julian day number times 86400000 */
  int Y, M, D;       /* Year, month, and day */
  int h, m;          /* Hour and minutes */
  int tz;            /* Timezone offset in minutes */
  double s;          /* Seconds */
  char validYMD;     /* True (1) if Y,M,D are valid */
  char validHMS;     /* True (1) if h,m,s are valid */
  char validJD;      /* True (1) if iJD is valid */
  char validTZ;      /* True (1) if tz is valid */
};

void computeYMD_HMS(DateTime *p);
void computeJD(DateTime *p);

//void initDB(void);
void initDB(bool recreate);
void DB_InitializeDetector(void);

long long int DB_WriteSpectrum(DB_SPEC *spectrum, bool bookEnd);
void DB_ReadSpectrum(DB_SPEC *db_spec);
void DB_SPEC_MEAS_to_DB_SPEC(DB_SPEC *spectrum, SPEC_MEAS *spec_meas);

void DB_PopWellTime(void);
void DB_PopDetector(DETECTOR *detector, short detectorType);
void DB_PopDetectorStandard(STAND *detectorStandard);
int DB_ReadBranding(void);
void DB_WriteBranding(int Branding);
int DB_ReadDefaultKEV(void);
void DB_WriteDefaultKEV(int DefaultKEV);
void DB_InactiveAutoCalAndBkg(int detectorType);
void DB_PopAutoCal(AUTOCAL *autoCal, int detectorType);
void DB_PopBackground(BACKGND *bkg_meas, int detectorType);
void DB_RetrieveBackgnd(BACKGND *backgnd);
bool DB_ExistsAutoCal(int detectorType);
bool DB_ExistsBackground(int detectorType);
void DB_WriteWellSerialNum(char *SerialNum, short detectorType);
void DB_ReadWellSerialNum(char *SerialNum, short detectorType);
void DB_WriteWellHV(short HV, short detectorType);
void DB_WriteWellThreshold(short Threshold, short detectorType);
void DB_WriteWellChannels(int Channels, short detectorType);
void DB_WriteDetector(DETECTOR *detector, short detectorType);
void DB_WriteAutoCalThreshold(int AutoCalThreshold, short detectorType);
void DB_WriteWellInstalled(short installed, short detectorType);
void DB_WriteWellCalib(AUTOCAL *autoCal, bool bookEnd);
void DB_WriteWellBackground(int detectorType, SPEC_MEAS *spec_meas, BACKGND *bkg_meas, bool bookEnd);
void DB_WriteDetectorStandard(STAND *detectorStandard);

void DB_CopyWipeType(WELLWIPETYPE *to, WELLWIPETYPE *from);
void DB_CreateWellWipeType(char *Name, float Threshold, int CountTime);
void DB_InactivateAllWellWipeType(void);
void DB_FixWellWipeType(void);
void DB_UpdateWellWipeType(WELLWIPETYPE *input, bool bookEnd);
void DB_readCurrentWellWipeType(WELLWIPETYPE *output);

void DB_CopyWipeLocation(WELLWIPELOCATION *to, WELLWIPELOCATION *from);
void DB_CreateWellWipeLocation(WELLWIPELOCATION *createItem, bool bookEnd);
void DB_ReadCurrentWipeLocations(int *LocationCount, int *LocationMax, WELLWIPELOCATION **activeWipeLocation, WELLWIPELOCATION **availableWipeLocation, WELLWIPELOCATION **selectedWipeLocation);
void DB_DeleteCurrentWipeLocation(WELLWIPELOCATION *delItem, bool bookEnd);
void DB_UpdateWellWipeLocation(WELLWIPELOCATION *updateLocation, bool bookEnd);

void DB_ReadCurrentWipeNuclides(int *WipeNuclideCount, int *WipeNuclideMax, WELLWIPENUCLIDE **activeWipeNuclide);
void DB_RefreshCurrentWipeNuclides(int *WipeNuclideCount, int *WipeNuclideMax, WELLWIPENUCLIDE **activeWipeNuclide, int NuclideID);
int DB_SearchWipes(WELLWIPESEARCH *wellWipes, time_t startDate, time_t endDate, int wipeType, int maxRetrieved);
void DB_RetrieveWipe(WELLWIPE *wellWipe);
void DB_InactivateWipe(WELLWIPE *wellWipe);

void DB_CreateWellSystemTest(WELLSYSTEMTEST *wellSystemTest, bool bookEnd);
int DB_SearchSystemTests(WELLSYSTEMTESTSEARCH *wellSystemTests, time_t startDate, time_t endDate, int maxRetrieved, int devType);
void DB_RetrieveSystemTest(WELLSYSTEMTEST *wellSystemTest);
void DB_InactivateSystemTest(WELLSYSTEMTEST *wellSystemTest);

void DB_CreateMDATest(WELLMDATEST *wellMDATest, bool bookEnd);
int DB_SearchMDATests(WELLMDASEARCH *wellMDATests, time_t startDate, time_t endDate, int maxRetrieved, int devType);
void DB_RetrieveMDATest(WELLMDATEST *wellMDATest);
void DB_InactivateMDATest(WELLMDATEST *wellMDATest);

void DB_CreateChiTest(WELLCHITEST *wellChiTest, bool bookEnd);
int DB_SearchChiTests(WELLCHISEARCH *wellChiTests, time_t startDate, time_t endDate, int maxRetrieved, int devType);
void DB_RetrieveChiTest(WELLCHITEST *wellChiTest);
void DB_InactivateChiTest(WELLCHITEST *wellChiTest);

void DB_CreateTestIdent(TESTIDENT *testIdent, bool bookEnd);
void DB_RetrieveTestIdent(TESTIDENT *testIdent);
void DB_CreateSchillingTest(WELLSCHILLINGTEST *wellSchillingTest, bool bookEnd);
int DB_SearchSchillingTests(WELLSCHILLINGSEARCH *wellSchillingTests, time_t startDate, time_t endDate, int maxRetrieved);
void DB_RetrieveSchillingTest(WELLSCHILLINGTEST *wellSchillingTest);
void DB_InactivateSchillingTest(WELLSCHILLINGTEST *wellSchillingTest);

void DB_CreatePlasmaTest(WELLPLASMATEST *wellPlasmaTest, bool bookEnd);
int DB_SearchPlasmaTests(WELLPLASMASEARCH *wellPlasmaTests, time_t startDate, time_t endDate, int maxRetrieved);
void DB_RetrievePlasmaTest(WELLPLASMATEST *wellPlasmaTest);
void DB_InactivatePlasmaTest(WELLPLASMATEST *wellPlasmaTest);

void DB_CreateRBCTest(WELLRBCTEST *wellRBCTest, bool bookEnd);
int DB_SearchRBCTests(WELLRBCSEARCH *wellRBCTests, time_t startDate, time_t endDate, int maxRetrieved);
void DB_RetrieveRBCTest(WELLRBCTEST *wellRBCTest);
void DB_InactivateRBCTest(WELLRBCTEST *wellRBCTest);

int DB_SearchAutoCalTests(WELLAUTOCALSEARCH *wellAutoCal, time_t startDate, time_t endDate, int maxRetrieved, int devType);
void DB_RetrieveAutoCalTest(AUTOCAL *autoCal);

void DB_updateWellTime(short int value, int index);
float DB_ReadFullSpecEff(short detector);
void DB_WriteFullSpecEff(short detector, float fullSpecEff);

int DB_ReadSecurityMode(void);
void DB_WriteSecurityMode(int SecurityMode);
void DB_GetUser(char *username, USER *user);
void DB_GetUserFromID(int userID, USER *user);
int DB_GetUserCount(USER *user);
int DB_GetUserList(USER *user, USER **userList, bool includeUser, bool includeInactive);
bool DB_TestUserName(char *userName);
void DB_WriteUser(USER *user);

void DB_ReadCurrentProbeBioAssayEfficiency(PROBEBIOASSAYEFFICIENCY *bioassayeff);
void DB_ReadCurrentProbeBioAssaySetting(PROBEBIOASSAYSETTING *bioassaysetting);
void DB_ReadProbeBioAssaySetting(long long int ProbeBioAssaySettingID, PROBEBIOASSAYSETTING *bioassaysetting);
void DB_WriteProbeBioAssaySetting(PROBEBIOASSAYSETTING *setting, bool bookEnd);
void DB_WriteProbeBioAssayEfficiency(PROBEBIOASSAYEFFICIENCY *eff, bool bookEnd);
void DB_WriteProbeBioAssayTest(PROBEBIOASSAYTEST *test, bool bookEnd);
int DB_SearchBioAssayTests(WELLBIOASSAYSEARCH *wellBioAssay, time_t startDate, time_t endDate, int maxRetrieved, int staffID);
int DB_SearchBioAssayTestsAll(WELLBIOASSAYSEARCH *wellBioAssay, time_t startDate, time_t endDate, int maxRetrieved);
void DB_RetrieveBioAssayTest(PROBEBIOASSAYTEST *test);
void DB_ReadBioAssayROI(float *I131Lower, float *I131Upper, float *I125Lower, float *I125Upper, float *I123Lower, float *I123Upper);
void DB_WriteBioAssayROI(float I131Lower, float I131Upper, float I125Lower, float I125Upper, float I123Lower, float I123Upper);
void DB_InactivateBioAssayTest(PROBEBIOASSAYTEST *probeBioAssayTest);

int DB_ActiveProbeThyroidUptakeProtocolCount(void);
void DB_RetrieveActiveProbeThyroidUptakeProtocolNames(PROBETHYROIDUPTAKEPROTOCOLACTIVENAME *protocolname);
void DB_RetrieveProbeThyroidUptakeProtocol(PROBETHYROIDUPTAKEPROTOCOL *protocol);
bool DB_TestProbeThyroidUptakeProtocolName(long long int ProbeTUProtocolID, char *protocolName);
void DB_WriteProbeThyroidUptakeProtocol(PROBETHYROIDUPTAKEPROTOCOL *protocol, bool bookEnd);
void DB_InactivateProbeThyroidUptakeProtocol(PROBETHYROIDUPTAKEPROTOCOL *protocol);
int DB_SearchThyroidUptakeTests(PROBETHYROIDUPTAKESEARCH *probeThyroidUptake, char *lastName, char *patientID, bool active);
void DB_RetrieveProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test);
int DB_GetActiveTUProtocol(void);
void DB_WriteProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test, bool bookEnd);
void DB_DeleteProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test);
void DB_InactivateProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test);
void DB_ReactivateProbeThyroidUptakeTest(PROBETHYROIDUPTAKETEST *test);
bool DB_ExistsThyroidUptakePreDose(long long int ProbeTUTestID);
bool DB_ExistsThyroidUptakeAdministeredDose(long long int ProbeTUTestID);
bool DB_ExistsThyroidUptakeResidualDose(long long int ProbeTUTestID);
int DB_NextThyroidUptakeMeasurementNumber(long long int ProbeTUTestID);
void DB_WriteProbeThyroidUptakePatientMeasurement(PROBETHYROIDUPTAKEPATMEAS *measurement, bool bookEnd);
void DB_WriteProbeThyroidUptakeDoseMeasurement(PROBETHYROIDUPTAKEDOSETOTALMEAS *measurement, bool bookEnd);
void DB_RetrieveProbeThyroidUptakeDoseMeasurement(PROBETHYROIDUPTAKEDOSETOTALMEAS *measurement);
void DB_RetrieveProbeThyroidUptakePatientMeasurement(PROBETHYROIDUPTAKEPATMEAS *measurement);
void DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(PROBETHYROIDUPTAKEDOSESINGLEMEAS *single);
void DB_RetrieveTUNormal(PROBETHYROIDUPTAKENORMAL normal[4]);
void DB_WriteTUNormal(PROBETHYROIDUPTAKENORMAL normal[4]);
int DB_SearchRBCSurvivalTests(WELLRBCSURVIVALSEARCH *wellRBCSurvival, char *lastName, char *patientID, bool active);
void DB_RetrieveWellRBCSurvivalTest(WELLRBCSURVIVALTEST *test);
void DB_WriteRBCSurvivalTest(WELLRBCSURVIVALTEST *test, bool bookEnd);
void DB_DeleteRBCSurvivalTest(WELLRBCSURVIVALTEST *test);
void DB_InactivateWellRBCSurvivalTest(WELLRBCSURVIVALTEST *test);
void DB_ReactivateWellRBCSurvivalTest(WELLRBCSURVIVALTEST *test);
int DB_RBCSurvivalMeasurementCount(WELLRBCSURVIVALTEST *test);
void DB_RetrieveWellRBCSurvivalMeasurement(WELLRBCSURVIVALMEAS *measurement);
void DB_WriteRBCSurvivalMeasurement(WELLRBCSURVIVALMEAS *measurement, bool bookEnd);
void DB_WriteRBCSurvivalResult(WELLRBCSURVIVALRESULT *result, bool bookEnd);
void DB_RetrieveWellRBCSurvivalResult(WELLRBCSURVIVALRESULT *result);
void DB_RetrieveRBCSurvivalNormal(float *minNormal, float *maxNormal);
void DB_WriteRBCSurvivalNormal(float minNormal, float maxNormal);
int DB_ReadTimeFormat(void);
void DB_WriteTimeFormat(int TimeFormat);
int DB_ReadUSBDeviceProtocol(void);
void DB_WriteUSBDeviceProtocol(int Protocol);
void DB_CreateAutoLinearityTest(AUTOLINEARITYTEST *test, bool bookEnd);
int DB_SearchAutoLinearityTests(AUTOLINEARITYSEARCH *tests, time_t startDate, time_t endDate, int maxRetrieved);
void DB_RetrieveAutoLinearityTest(AUTOLINEARITYTEST *test);
void DB_InactivateAutoLinearityTest(AUTOLINEARITYTEST *test);
int DB_ReadFutureDateInput(void);
void DB_WriteFutureDateInput(int FutureDateInput);
int DB_ReadLanguage(void);
void DB_WriteLanguage(int Language);
int DB_ReadFeedLabel(void);
void DB_WriteFeedLabel(int FeedLabel);
int DB_ReadMolyStreamlined(void);
void DB_WriteMolyStreamlined(int MolyStreamlined);
long long int DB_ReadKey(void);
void DB_WriteKey(long long int Key);
void DB_ReadPassword(char *password);
void DB_WritePassword(char *password);

void DB_CreateChamberZero(CHAMBERZERO *chamberzero, long long int *ChamberDailyTestID, bool bookEnd);
void DB_CreateChamberBackground(CHAMBERBACKGROUND *chamberbackground, long long int ChamberDailyTestID, bool bookEnd);
void DB_CreateChamberVoltage(CHAMBERVOLTAGE *chambervoltage, long long int ChamberDailyTestID, bool bookEnd);
void DB_UpdateDailyTestDataCheck(long long int ChamberDailyTestID, char *DataCheckTextEnglish, char *DataCheckTextFrench, bool DataCheckFailed, char *DataCheckCRC, bool bookEnd);
void DB_CreateChamberAccuracyTest(CHAMBERACCURACYTEST *chamberaccuracytest, long long int ChamberDailyTestID, bool bookEnd);
void DB_CreateChamberAccuracyMeasurement(CHAMBERACCURACYMEASUREMENT *chamberaccuracymeasurement, bool bookEnd);
void DB_StartBookEnd(void);
void DB_CreateChamberAutoConstancy(CHAMBERAUTOCONSTANCY *chamberautoconstancy, bool bookEnd);
void DB_EndBookEnd(void);
int DB_SearchChamberTests(int mode, CHAMBERSEARCH *tests, time_t startDate, time_t endDate, int maxRetrieved);
void DB_RetrieveDailyTest(CHAMBERDAILYTEST *test);
void DB_RetrieveZeroMeasurement(CHAMBERZERO *test);
void DB_RetrieveBackgroundMeasurement(CHAMBERBACKGROUND *test);
void DB_RetrieveChamberVoltage(CHAMBERVOLTAGE *test);
void DB_RetrieveAccuracyTest(CHAMBERACCURACYTEST *test, CHAMBERACCURACYMEASUREMENT measurement[], CHAMBERAUTOCONSTANCY autoconstancy[]);
void DB_InactivateDailyTest(CHAMBERDAILYTEST *test);
void DB_InactivateZeroMeasurement(CHAMBERZERO *test);
void DB_InactivateBackgroundMeasurement(CHAMBERBACKGROUND *test);
void DB_InactivateChamberVoltage(CHAMBERVOLTAGE *test);
void DB_InactivateAccuracyTest(CHAMBERACCURACYTEST *test);
bool DB_CheckDailyTestForZeroID(long long int TestID);
bool DB_CheckDailyTestForBackgroundID(long long int TestID);
bool DB_CheckDailyTestForVoltageID(long long int TestID);
bool DB_CheckDailyTestForAccuracyID(long long int TestID);
void DB_ReadAllHotkey(void);
void DB_WriteAllHotkey(int ch_type);
void DB_PopulateDefaultHotkeys(void);

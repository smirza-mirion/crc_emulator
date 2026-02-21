#pragma once
#define INDEX_HTM                                    0x00
#define NUCLIDE2_HTM                                 0x01
#define KEYPAD2_HTM                                  0x02
#define KEYBOARD2_HTM                                0x03
#define SETTIME2_HTM                                 0x04
#define SETACTIVITY_HTM                              0x05
#define MEASUREACTIVITY_HTM                          0x06
#define INVENTORYADD_HTM                             0x07
#define INVENTORYWITHDRAW_HTM                        0x08
#define MAINSCREEN_HTM                               0x09
#define GEOMETRYREPORT_HTM                           0x0A
#define SETSTUDY_HTM                                 0x0B
#define ONESTRIP_HTM                                 0x0C
#define TWOSTRIP_HTM                                 0x0D
#define HMPAO_HTM                                    0x0E
#define MAG3_HTM                                     0x0F
#define DAILY_HTM                                    0x10
#define BACKGROUND_HTM                               0x11
#define CHAMBERVOLTS_HTM                             0x12
#define GEOMETRY_HTM                                 0x13
#define ACCURACY_HTM                                 0x14
#define LINEARITY_HTM                                0x15
#define QC_HTM                                       0x16
#define MOLY_HTM                                     0x17
#define INVENTORY_HTM                                0x18
#define SETUP_HTM                                    0x19
#define INVENTORYDELETE_HTM                          0x1A
#define INVENTORYKIT_HTM                             0x1B
#define SETSTUDY2_HTM                                0x1C
#define ENHANCED_HTM                                 0x1D
#define INFO_HTM                                     0x1E
#define OTHER_HTM                                    0x1F
#define SETUPSOURCES_HTM                             0x20
#define SETUPMOLY_HTM                                0x21
#define SETUPNUCLIDE_HTM                             0x22
#define SETUPLINEARITY_HTM                           0x23
#define SETUPREMOTE_HTM                              0x24
#define SETUPCALNUM_HTM                              0x25
#define ERRORMSG_HTM                                 0x26
#define WARNINGMSG_HTM                               0x27
#define NOTIFICATIONMSG_HTM                          0x28
#define SETUPLINEARITYSTANDARD_HTM                   0x29
#define SETUPLINEARITYLINEATOR_HTM                   0x2A
#define SETUPLINEARITYCALICHECK_HTM                  0x2B
#define DOSETABLE_HTM                                0x2C
#define CHAMBER_HTM                                  0x2D
#define GRADIENT_HTM                                 0x2E
#define WELLMAINSCREEN_HTM                           0x2F
#define BETAMAINSCREEN_HTM                           0x30
#define WELLMEASUREMENT_HTM                          0x31
#define WELLAUTOCALIBRATE_HTM                        0x32
#define WELLPEAKS_HTM                                0x33
#define WELLADVANCEDSETUP_HTM                        0x34
#define WELLSETUPNUCLIDE_HTM                         0x35
#define WELLSETUPSEALED_HTM                          0x36
#define WELLSETUPSEALED2_HTM                         0x37
#define WELLSETUPTESTSOURCE_HTM                      0x38
#define WELLSETUPTRIGGERLEVEL_HTM                    0x39
#define WELLSETUPEFFICIENCIES_HTM                    0x3A
#define WELLEDITEFFICIENCIES_HTM                     0x3B
#define WELLMANUAL_HTM                               0x3C
#define WELLMEASUREEFFICIENCY_HTM                    0x3D
#define AUTOCONSTANCY_HTM                            0x3E
#define SLEEP_HTM                                    0x3F
#define FACTORY_HTM                                  0x40
#define SETUPCHAMBER_HTM                             0x41
#define INVENTORYDELETEALL_HTM                       0x42
#define WELLMDATEST_HTM                              0x43
#define WELLMEASUREMENTS_HTM                         0x44
#define WELLEDITFULLEFFICIENCY_HTM                   0x45
#define SETUPCALIBSERIALNUM_HTM                      0x46
#define WELLWIPEREPORT_HTM                           0x47
#define WELLLABTESTS_HTM                             0x48
#define WELLSCHILLING_HTM                            0x49
#define WELLPLASMA_HTM                               0x4A
#define WELLRBC_HTM                                  0x4B
#define WELLFACTORY_HTM                              0x4C
#define WELLPLASMAANALYSIS_HTM                       0x4D
#define WELLRBCANALYSIS_HTM                          0x4E
#define WELLQATESTS_HTM                              0x4F
#define WELLSYSTEMTESTANALYSIS_HTM                   0x50
#define WELLCHITEST_HTM                              0x51
#define WELLSCHILLINGANALYSIS_HTM                    0x52
#define WELLCHANGEROI_HTM                            0x53
#define WELLGENERALANALYSIS_HTM                      0x54
#define SETUPRHOTKEYS_HTM                            0x55
#define SETUPPHOTKEYS_HTM                            0x56
#define REMOTE_HTM                                   0x57
#define DELETELINEATORSETTINGS_HTM                   0x58
#define DELETECALICHECKSETTINGS_HTM                  0x59
#define SELECTLINEARITY_HTM                          0x5A
#define LOWLEVEL_HTM                                 0x5B
#define WELLSTABILITYTEST_HTM                        0x5C
#define SETUPCOMMUNICATIONS_HTM                      0x5D
#define PCQC_HTM                                     0x5E
#define ETHERNET_HTM                                 0x5F
#define WELLSETUPWIPES_HTM                           0x60
#define WELLSETUPTYPES_HTM                           0x61
#define WELLSETUPLOCATIONS_HTM                       0x62
#define WELLSETUPADDEDITLOCATION_HTM                 0x63
#define WELLSETUPDELETELOCATION_HTM                  0x64
#define WELLWIPELIST_HTM                             0x65
#define ERRORMSGLARGE_HTM                            0x66
#define WELLSPECTRUM_HTM                             0x67
#define WELLREPORTS_HTM                              0x68
#define WELLWIPESEARCH_HTM                           0x69
#define GENERICITEMS_HTM                             0x6A
#define INACTIVATE_HTM                               0x6B
#define WELLSYSTEMTESTSEARCH_HTM                     0x6C
#define WELLMDASEARCH_HTM                            0x6D
#define WELLMDAANALYSIS_HTM                          0x6E
#define WELLCHISEARCH_HTM                            0x6F
#define WELLCHIANALYSIS_HTM                          0x70
#define TESTIDENT_HTM                                0x71
#define WELLSCHILLINGSEARCH_HTM                      0x72
#define WELLSCHILLINGREPORT_HTM                      0x73
#define WELLPLASMASEARCH_HTM                         0x74
#define WELLPLASMAREPORT_HTM                         0x75
#define WELLRBCSEARCH_HTM                            0x76
#define WELLRBCREPORT_HTM                            0x77
#define WELLAUTOCALSEARCH_HTM                        0x78
#define WELLAUTOCALREPORT_HTM                        0x79
#define WELLFACTORYDETECTORS_HTM                     0x7A
#define WELLSETUPUSERNUCLIDES_HTM                    0x7B
#define WELLADDEDITUSERNUCLIDES_HTM                  0x7C
#define LOGIN_HTM                                    0x7D
#define SECURITY_HTM                                 0x7E
#define ADDEDITUSER_HTM                              0x7F
#define WELLSETUPBIOASSAY_HTM                        0x80
#define WELLSETUPBIOASSAYENTEREFFICIENCY_HTM         0x81
#define WELLSETUPBIOASSAYMEASUREEFFICIENCY_HTM       0x82
#define WELLBIOASSAY_HTM                             0x83
#define WELLBIOASSAYANALYSIS_HTM                     0x84
#define WELLBIOASSAYSEARCH_HTM                       0x85
#define WELLSETUPBIOASSAYENTERROI_HTM                0x86
#define WELLSETUPTHYROIDUPTAKEPROTOCOL_HTM           0x87
#define ADDEDITTHYROIDUPTAKEPROTOCOL_HTM             0x88
#define WELLTHYROIDUPTAKE_HTM                        0x89
#define ADDEDITTHYROIDUPTAKETEST_HTM                 0x8A
#define WELLTHYROIDUPTAKETEST_HTM                    0x8B
#define WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM 0x8C
#define WELLTHYROIDUPTAKEMEASUREDOSE_HTM             0x8D
#define WELLTHYROIDUPTAKEMEASUREPATIENT_HTM          0x8E
#define WELLTHYROIDUPTAKEENTERNORMAL_HTM             0x8F
#define DUMMY1_HTM                                   0x90
#define DUMMY2_HTM                                   0x91
#define WELLRBCSURVIVAL_HTM                          0x92
#define ADDEDITRBCSURVIVALTEST_HTM                   0x93
#define WELLRBCSURVIVALTEST_HTM                      0x94
#define WELLRBCSURVIVALMEASUREMENT_HTM               0x95
#define WELLRBCSURVIVALENTERNORMAL_HTM               0x96
#define SETTIME2EUR_HTM                              0x97
#define SETTIME2JAP_HTM                              0x98
#define AUTOLINEARITY_HTM                            0x99
#define AUTOLINEARITYTEST_HTM                        0x9A
#define GENERICYESNO_HTM                             0x9B
#define AUTOLINEARITYSEARCH_HTM                      0x9C
#define CHAMBERREPORTS_HTM                           0x9D
#define CHAMBERSEARCH_HTM                            0x9E
#define CHAMBERDAILYTESTVIEW_HTM                     0x9F
#define CHAMBERZEROTESTVIEW_HTM                      0xA0
#define CHAMBERBACKGROUNDTESTVIEW_HTM                0xA1
#define CHAMBERVOLTAGETESTVIEW_HTM                   0xA2
#define CHAMBERACCURACYTESTVIEW_HTM                  0xA3
#define CHAMBERAUTOCONSTANCYTESTVIEW_HTM             0xA4
#define HALFLIFECALCULATOR_HTM                       0xA5
#define SETUPKEY_HTM                                 0xA6
#define SETUPPASSWORD_HTM                            0xA7
#define AMULET_HTML_ARRAY_SIZE                       0xA8

#define SET_HTML_ACCURACY		1
#define SET_HTML_BACKGROUND		2
#define SET_HTML_CHAMBERVOLTS		3
#define SET_HTML_DAILY		4
#define SET_HTML_GEOMETRY		5
#define SET_HTML_GEOMETRYREPORT	6
#define SET_HTML_HMPAO		7
#define SET_HTML_INDEX		8
#define SET_HTML_INVENTORY		9
#define SET_HTML_INVENTORYADD		10
#define SET_HTML_INVENTORYWITHDRAW	11
#define SET_HTML_KEYBOARD2		12
#define SET_HTML_KEYPAD2		13
#define SET_HTML_LINEARITY		14
#define SET_HTML_MAG3			15
#define SET_HTML_MAINSCREEN		16
#define SET_HTML_MEASUREACTIVITY	17
#define SET_HTML_MOLY			18
#define SET_HTML_NUCLIDE2		19
#define SET_HTML_ONESTRIP		20
#define SET_HTML_QC			21
#define SET_HTML_SETACTIVITY		22
#define SET_HTML_SETSTUDY		23
#define SET_HTML_SETTIME2		24
#define SET_HTML_SETUP		25
#define SET_HTML_TWOSTRIP		26
#define SET_HTML_INVENTORYDELETE	27
#define SET_HTML_INVENTORYKIT	28
#define SET_HTML_SETSTUDY2		29
#define SET_HTML_ENHANCED		30
#define SET_HTML_INFO			31
#define SET_HTML_PSETUP			32
#define SET_HTML_SETUPSOURCES	33
#define SET_HTML_SETUPMOLY		34
#define SET_HTML_SETUPNUCLIDE	35
#define SET_HTML_SETUPLINEARITY	36
#define SET_HTML_SETUPREMOTE	37
#define SET_HTML_SETUPCALNUM	38
#define SET_HTML_ERRORMSG		39
#define SET_HTML_WARNINGMSG		40
#define SET_HTML_NOTIFICATIONMSG 41
#define SET_HTML_SETUPLINEARITYSTANDARD 42
#define SET_HTML_SETUPLINEARITYLINEATOR 43
#define SET_HTML_SETUPLINEARITYCALICHECK 44
#define SET_HTML_DOSETABLE		45
#define SET_HTML_SELECT_CHAMBER 46
#define SET_HTML_GRADIENT		47
#define SET_HTML_WELLMAINSCREEN	48
#define SET_HTML_BETAMAINSCREEN 49
#define SET_HTML_WELLMEASUREMENT	50
#define SET_HTML_WELLAUTOCALIBRATE 51
#define SET_HTML_WELLPEAKS	52
#define SET_HTML_WELLADVANCEDSETUP	53
#define SET_HTML_WELLSETUPNUCLIDE	54
#define SET_HTML_WELLSETUPSEALED	55
#define SET_HTML_WELLSETUPSEALED2	56
#define SET_HTML_WELLSETUPTESTSOURCE 57
#define SET_HTML_WELLSETUPTRIGGERLEVEL	58
#define SET_HTML_WELLSETUPEFFICIENCIES	59
#define SET_HTML_WELLEDITEFFICIENCIES	60
#define SET_HTML_WELLMANUAL				61
#define SET_HTML_WELLMEASUREEFFICIENCY	62
#define SET_HTML_AUTOCONSTANCY			63
#define SET_HTML_SLEEP					64
#define SET_HTML_FACTORY				65
#define SET_HTML_SETUPCHAMBER			66
#define SET_HTML_INVENTORYDELETEALL		67
#define SET_HTML_WELLMDATEST			68
#define SET_HTML_WELLMEASUREMENTS		69
#define SET_HTML_WELLEDITFULLEFFICIENCY 70
#define SET_HTML_SETUPCALIBSERIAL		71
#define SET_HTML_WELLWIPEREPORT			72
#define SET_HTML_WELLLABTESTS			73
#define SET_HTML_WELLSCHILLING			74
#define SET_HTML_WELLPLASMA				75
#define SET_HTML_WELLRBC				76
#define SET_HTML_WELLFACTORY			77
#define SET_HTML_WELLPLASMAANALYSIS		78
#define SET_HTML_WELLRBCANALYSIS		79
#define SET_HTML_WELLQATESTS			80
#define SET_HTML_WELLSYSTESTANALYSIS	81
#define SET_HTML_WELLCHITEST			82
#define SET_HTML_WELLSCHILLINGANALYSIS	83
#define SET_HTML_WELLCHANGEROI			84
#define SET_HTML_WELLGENERALANALYSIS	85
#define SET_HTML_SETUPRHOTKEYS			86
#define SET_HTML_SETUPPHOTKEYS			87
#define SET_HTML_REMOTE					88
#define SET_HTML_DELETELINEATOR			89
#define SET_HTML_DELETECALICHECK		90
#define SET_HTML_SELECTLINEARITY		91
#define SET_HTML_LOWLEVEL				92
#define SET_HTML_WELLSTABILITYTEST		93
#define SET_HTML_SETUPCOMMUNICATIONS	94
#define SET_HTML_PCQC					95
#define SET_HTML_ETHERNET				96
#define SET_HTML_WELLSETUPWIPES			97
#define SET_HTML_WELLSETUPTYPES			98
#define SET_HTML_WELLSETUPLOCATIONS		99
#define SET_HTML_WELLSETUPADDEDITLOCATION 100
#define SET_HTML_WELLSETUPDELETELOCATION 101
#define SET_HTML_WELLWIPELIST			102
#define SET_HTML_ERRORMSGLARGE			103
#define SET_HTML_WELLSPECTRUM			104
#define SET_HTML_WELLREPORTS			105
#define SET_HTML_WELLWIPESEARCH			106
#define SET_HTML_GENERICITEMS			107
#define SET_HTML_INACTIVATE				108
#define SET_HTML_WELLSYSTEMTESTSEARCH	109
#define SET_HTML_WELLMDASEARCH			110
#define SET_HTML_WELLMDAANALYSIS		111
#define SET_HTML_WELLCHISEARCH			112
#define SET_HTML_WELLCHIANALYSIS		113
#define SET_HTML_TESTIDENT				114
#define SET_HTML_WELLSCHILLINGSEARCH	115
#define SET_HTML_WELLSCHILLINGREPORT	116
#define SET_HTML_WELLPLASMASEARCH		117
#define SET_HTML_WELLPLASMAREPORT		118
#define SET_HTML_WELLRBCSEARCH			119
#define SET_HTML_WELLRBCREPORT			120
#define SET_HTML_WELLAUTOCALSEARCH		121
#define SET_HTML_WELLAUTOCALREPORT		122
#define SET_HTML_WELLFACTORYDETECTORS	123
#define SET_HTML_WELLSETUPUSERNUCLIDES	124
#define SET_HTML_WELLADDEDITUSERNUCLIDES	125
#define SET_HTML_LOGIN					126
#define SET_HTML_SECURITY				127
#define SET_HTML_ADDEDITUSER			128
#define SET_HTML_WELLSETUPBIOASSAY		129
#define SET_HTML_WELLSETUPBIOASSAYENTEREFF	130
#define SET_HTML_WELLSETUPBIOASSAYMEASUREEFF	131
#define SET_HTML_WELLBIOASSAY			132
#define SET_HTML_WELLBIOASSAYANALYSIS	133
#define SET_HTML_WELLBIOASSAYSEARCH		134
#define SET_HTML_WELLSETUPBIOASSAYENTERROI	135
#define SET_HTML_WELLSETUPTHYROIDUPTAKEPROTOCOL	136
#define SET_HTML_ADDEDITTHYROIDUPTAKEPROTOCOL	137
#define SET_HTML_WELLTHYROIDUPTAKE		138
#define SET_HTML_ADDEDITTHYROIDUPTAKETEST	139
#define SET_HTML_WELLTHYROIDUPTAKETEST	140
#define SET_HTML_WELLTHYROIDUPTAKEENTERADMIN	141
#define SET_HTML_WELLTHYROIDUPTAKEMEASDOSE		142
#define SET_HTML_WELLTHYROIDUPTAKEMEASPAT		143
#define SET_HTML_WELLTHYROIDUPTAKEENTERNORMAL	144
#define SET_HTML_DUMMY1							145
#define SET_HTML_DUMMY2							146
#define SET_HTML_WELLRBCSURVIVAL				147
#define SET_HTML_ADDEDITRBCSURVIVALTEST			148
#define SET_HTML_WELLRBCSURVIVALTEST			149
#define SET_HTML_WELLRBCSURVIVALMEASUREMENT		150
#define SET_HTML_WELLRBCSURVIVALENTERNORMAL		151
#define SET_HTML_SETTIME2EUR					152
#define SET_HTML_SETTIME2JAP					153
#define SET_HTML_AUTOLINEARITY					154
#define SET_HTML_AUTOLINEARITYTEST				155
#define SET_HTML_GENERIC_YES_NO					156
#define SET_HTML_AUTOLINEARITYSEARCH			157
#define SET_HTML_REPORTS						158
#define SET_HTML_CHAMBER_SEARCH					159
#define SET_HTML_CHAMBERDAILYTESTVIEW			160
#define SET_HTML_CHAMBERZEROTESTVIEW			161
#define SET_HTML_CHAMBERBACKGROUNDTESTVIEW		162
#define SET_HTML_CHAMBERVOLTAGETESTVIEW			163
#define SET_HTML_CHAMBERACCURACYTESTVIEW		164
#define SET_HTML_CHAMBERAUTOCONSTANCYTEST		165
#define SET_HTML_CHAMBERHALFLIFECALC			166

#define MENU_NONE 0
#define MENU_DAILY 1
#define MENU_BKG 2
#define MENU_CHAMBERVOLTS 3
#define MENU_ACCURACY 4
#define MENU_GEOMETRY 5
#define MENU_LINEARITY 6
#define MENU_ONESTRIP 7
#define MENU_TWOSTRIP 8
#define MENU_HMPAO 9
#define MENU_MAG3 10
#define MENU_MOLY 11
#define MENU_INVENTORY 12
#define MENU_NUCLIDE 13
#define MENU_TIME 14
#define MENU_ADD_INVENTORY 15
#define MENU_KEYPAD 16
#define MENU_KEYBOARD 17
#define MENU_STUDY 18
#define MENU_ACTIVITY 19
#define MENU_MEASURE 20
#define MENU_WITHDRAW_INVENTORY 21
#define MENU_DELETE_INVENTORY	22
#define MENU_KIT_INVENTORY 		23
#define MENU_STUDY2				24
#define MENU_INFO				25
#define MENU_SETUP				26
#define MENU_SETUP_SOURCES		27
#define MENU_SETUP_MOLY			28
#define MENU_SETUP_NUCLIDE		29
#define MENU_SETUP_LINEARITY	30
#define MENU_SETUP_REMOTE		31
#define MENU_MAIN_SCREEN		32
#define MENU_SETUP_CALNUM		33
#define MENU_ERROR_MSG			34
#define MENU_WARNING_MSG		35
#define MENU_NOTIFICATION_MSG	36
#define MENU_SETUP_LINEARITY_STANDARD 37
#define MENU_SETUP_LINEARITY_LINEATOR 38
#define MENU_SETUP_LINEARITY_CALICHECK 39
#define MENU_DOSE_TABLE			40
#define MENU_SELECT_CHAMBER		41
#define MENU_INITIAL			42
#define MENU_OTHER				43
#define MENU_WELL_MAIN_SCREEN	44
#define MENU_WELL_MEASUREMENT	45
#define MENU_WELL_AUTOCALIBRATE	46
#define MENU_WELL_PEAKS			47
#define MENU_WELL_ADVANCED_SETUP 48
#define MENU_WELL_SETUP_NUCLIDE 49
#define MENU_WELL_SETUP_SEALED	50
#define MENU_WELL_SETUP_SEALED2	51
#define MENU_WELL_SETUP_TEST_SOURCE 52
#define MENU_WELL_SETUP_TRIGGER_LEVEL 53
#define MENU_WELL_SETUP_EFFICIENCIES 54
#define MENU_WELL_EDIT_EFFICIENCIES	55
#define MENU_WELL_MANUAL			56
#define MENU_WELL_MEASURE_EFFICIENCY 57
#define MENU_GEOMETRY_REPORT		58
#define MENU_AUTOCONSTANCY			59
#define MENU_SLEEP					60
#define MENU_ENHANCED				61
#define MENU_FACTORY				62
#define MENU_SETUPCHAMBER			63
#define MENU_DELETE_ALL_INVENTORY	64
#define MENU_WELL_MDA_TEST			65
#define MENU_WELL_MEASUREMENTS		66
#define MENU_WELL_EDITFULLEFFICIENCY 67
#define MENU_SETUPCALIBSERIAL		68
#define MENU_WELL_WIPEREPORT		69
#define MENU_WELL_SCHILLING			70
#define MENU_WELL_PLASMA			71
#define MENU_WELL_RBC				72
#define MENU_WELL_FACTORY			73
#define MENU_WELL_PLASMAANALYSIS	74
#define MENU_WELL_RBCANALYSIS		75
#define MENU_WELL_QATESTS			76
#define MENU_WELL_SYSTESTANALYSIS	77
#define MENU_WELL_CHITEST			78
#define MENU_WELL_SCHILLINGANALYSIS 79
#define MENU_WELL_CHANGE_ROI		80
#define MENU_WELL_GENERALANALYSIS	81
#define MENU_SETUP_RHOTKEYS			82
#define MENU_SETUP_PHOTKEYS			83
#define MENU_REMOTE					84
#define MENU_DELETE_LINEATOR		85
#define MENU_DELETE_CALICHECK		86
#define MENU_SELECT_LINEARITY		87
#define MENU_QC						88
#define MENU_LOW_LEVEL				89
#define MENU_WELL_STABILITYTEST		90
#define MENU_SETUP_COMMUNICATIONS	91
#define MENU_PCQC					92
#define MENU_ETHERNET				93
#define MENU_SETUP_TYPES			94
#define MENU_WELL_SETUP_LOCATIONS	95
#define MENU_WELL_SETUP_ADDEDITLOCATION 96
#define MENU_WELL_SETUP_DELETELOCATION 97
#define MENU_WELL_WIPE_LIST			98
#define MENU_ERROR_MSG_LARGE		99
#define MENU_WELL_SPECTRUM			100
#define MENU_WELL_WIPE_SEARCH		101
#define MENU_GENERIC_ITEMS			102
#define MENU_INACTIVATE				103
#define MENU_WELL_SYSTEM_TEST_SEARCH	104
#define MENU_WELL_MDA_SEARCH		105
#define MENU_WELL_MDA_ANALYSIS		106
#define MENU_WELL_CHI_SEARCH		107
#define MENU_WELL_CHI_ANALYSIS		108
#define MENU_TEST_IDENT				109
#define MENU_WELL_SCHILLING_SEARCH	110
#define MENU_WELL_SCHILLING_REPORT	111
#define MENU_WELL_PLASMA_SEARCH		112
#define MENU_WELL_PLASMA_REPORT		113
#define MENU_WELL_RBC_SEARCH		114
#define MENU_WELL_RBC_REPORT		115
#define MENU_WELL_AUTOCAL_SEARCH	116
#define MENU_WELL_AUTOCAL_REPORT	117
#define MENU_WELL_FACTORY_DETECTORS	118
#define MENU_WELL_SETUP_USER_NUCLIDES 119
#define MENU_WELL_ADD_EDIT_USER_NUCLIDES 120
#define MENU_WELL_LAB_TESTS			121
#define MENU_WELL_REPORTS			122
#define MENU_LOGIN					123
#define MENU_SECURITY				124
#define MENU_ADD_EDIT_USER			125
#define MENU_WELL_SETUP_BIOASSAY	126
#define MENU_WELL_SETUP_BIOASSAY_ENTER_EFF	127
#define MENU_WELL_SETUP_BIOASSAY_MEASURE_EFF	128
#define MENU_WELL_BIOASSAY			129
#define MENU_WELL_BIOASSAYANALYSIS	130
#define MENU_WELL_BIOASSAY_SEARCH	131
#define MENU_WELL_SETUP_BIOASSAY_ENTER_ROI	132
#define MENU_WELL_SETUP_THYROID_UPTAKE_PROTOCOL	133
#define MENU_ADD_EDIT_THYROID_UPTAKE_PROTOCOL	134
#define MENU_WELL_THYROID_UPTAKE	135
#define MENU_ADD_EDIT_THYROID_UPTAKE_TEST	136
#define MENU_WELL_THYROID_UPTAKE_TEST	137
#define MENU_WELL_THYROID_UPTAKE_ENTER_ADMIN	138
#define MENU_WELL_THYROID_UPTAKE_MEAS_DOSE		139
#define MENU_WELL_THYROID_UPTAKE_MEAS_PAT		140
#define MENU_WELL_THYROID_UPTAKE_ENTER_NORMAL	141
#define MENU_WELL_RBC_SURVIVAL					144
#define MENU_ADD_EDIT_RBC_SURVIVAL_TEST			145
#define MENU_WELL_RBC_SURVIVAL_TEST				146
#define MENU_WELL_RBC_SURVIVAL_MEASUREMENT		147
#define MENU_WELL_RBC_SURVIVAL_ENTER_NORMAL		148
#define MENU_AUTOLINEARITY						149
#define MENU_AUTOLINEARITY_TEST					150
#define MENU_GENERIC_YES_NO						151
#define MENU_AUTOLINEARITY_SEARCH				152
#define MENU_CHAMBER_REPORTS					153
#define MENU_CHAMBER_SEARCH						154
#define MENU_CHAMBER_DAILY_TEST_VIEW			155
#define MENU_CHAMBER_ZERO_TEST_VIEW				156
#define MENU_CHAMBER_BACKGROUND_TEST_VIEW		157
#define MENU_CHAMBER_VOLTAGE_TEST_VIEW			158
#define MENU_CHAMBER_ACCURACY_TEST_VIEW			159
#define MENU_CHAMBER_AUTOCONSTANCY_TEST_VIEW	160
#define MENU_CHAMBER_HALFLIFE_CALC				161
#define MENU_SETUP_KEY							162
#define MENU_SETUP_PASSWORD						163

#define PHASE_GENERIC_BEFORE_INIT 0
#define PHASE_GENERIC_NOP -1
#define PHASE_GENERIC_CUR_NOP -2

/*#define PHASE_ACCURACY_WAIT_FOR_AMULET_INIT 0
#define PHASE_ACCURACY_BEFORE_INIT 1
#define PHASE_ACCURACY_AFTER_INIT 2
#define PHASE_ACCURACY_DURING_ACCURACY 3
#define PHASE_ACCURACY_MEASURE_SOURCE1 4
#define PHASE_ACCURACY_MEASURE_SOURCE2 5
#define PHASE_ACCURACY_MEASURE_SOURCE3 6
#define PHASE_ACCURACY_MEASURE_SOURCE4 7
#define PHASE_ACCURACY_MEASURE_SOURCE5 8
#define PHASE_ACCURACY_SET_NUCLIDE 9
#define PHASE_ACCURACY_MEASURE_NUCLIDE 10
#define PHASE_ACCURACY_ACCEPT_PRESSED 11
#define PHASE_ACCURACY_PRINT	12
#define PHASE_ACCURACY_FINISHED 100 */

#define MEASURE_EFF_EDIT_WELL	1
#define MEASURE_EFF_EDIT_PROBE	2
#define MEASURE_EFF_ADD_WELL	3
#define MEASURE_EFF_ADD_PROBE	4

struct activity_measurement{
	float activity;
	time_t timestamp;
	short nuclideid;
};
typedef struct activity_measurement ACTIVITY_MEASUREMENT;

void service_amulet(void);
void amulet_menu(void);
void AmuletDaily_menu(void);
void AmuletBackground_menu(void);
void AmuletChamberVolts_menu(void);
void AmuletAccuracy_menu(void);
void AmuletGeometry_menu(void);
void AmuletGeometryReport_menu(void);
void AmuletLinearity_menu(void);
void AmuletOneStrip_menu(void);
void AmuletTwoStrip_menu(void);
void AmuletHMPAO_menu(void);
void AmuletMAG3_menu(void);
void AmuletMoly_menu(void);
void AmuletInventory_menu(void);
//void nuclide_menu(void);
void AmuletNuclide_menu(void);
void AmuletTime_menu(void);
void AmuletAddInventory_menu(void);
void AmuletKeypad_menu(void);
void AmuletKeyboard_menu(void);
void AmuletStudy_menu(void);
void AmuletStudy2_menu(void);
void AmuletActivity_menu(void);
void AmuletMeasure_menu(void);
void AmuletWithdrawInv_menu(void);
void AmuletDeleteInv_menu(void);
void AmuletKitInv_menu(void);
void AmuletInfo_menu(void);
void AmuletSetup_menu(void);
void AmuletSetupSources_menu(void);
void AmuletSetupMoly_menu(void);
void AmuletSetupNuclide_menu(void);
void AmuletSetupLinearity_menu(void);
void AmuletSetupRemote_menu(void);
void AmuletMainScreen_menu(void);
void AmuletSetupCalNum_menu(void);
void AmuletErrorMsg_menu(void);
void AmuletWarningMsg_menu(void);
void AmuletNotificationMsg_menu(void);
void AmuletSetupLinearityStandard_menu(void);
void AmuletSetupLinearityLineator_menu(void);
void AmuletSetupLinearityCalicheck_menu(void);
void AmuletDoseTable_menu(void);
void AmuletSelectChamber_menu(void);
void AmuletInitial_menu(void);
void AmuletOther_menu(void);
void AmuletWellMainScreen_menu(void);
void AmuletWellMeasurement_menu(void);
void Amulet_DisplayErrorCode(short errcode);
void AmuletWellAutoCalibrate_menu(void);
void AmuletWellPeaks_menu(void);
void AmuletWellSetup_menu(void);
void AmuletWellAdvancedSetup_menu(void);
void AmuletWellSetupNuclide_menu(void);
void AmuletWellSetupSealed_menu(void);
void AmuletWellSetupSealed2_menu(void);
void AmuletWellSetupTestNuclide_menu(void);
void AmuletWellSetupTriggerLevel_menu(void);
void AmuletWellSetupEfficiencies_menu(void);
void AmuletWellEditEfficiencies_menu(void);
void AmuletWellManual_menu(void);
void AmuletWellMeasureEfficiency_menu(void);
void AmuletAutoConstancy_menu(void);
void AmuletSleep_menu(void);
void AmuletEnhanced_menu(void);
void AmuletFactory_menu(void);
void AmuletSetupChamber_menu(void);
void AmuletDeleteAllInv_menu(void);
void AmuletWellMDATest_menu(void);
void AmuletWellMeasurements_menu(void);
void AmuletWellEditFullEfficiency_menu(void);
void AmuletSetupCalibSerial_menu(void);
void AmuletWellWipeReport_menu(void);
void AmuletWellSchilling_menu(void);
void AmuletWellPlasma_menu(void);
void AmuletWellRBC_menu(void);
void AmuletWellFactory_menu(void);
void AmuletWellPlasmaAnalysis_menu(void);
void AmuletWellRBCAnalysis_menu(void);
void AmuletWellQATests_menu(void);
void AmuletWellSystemTestAnalysis_menu(void);
void AmuletWellChiTest_menu(void);
void AmuletWellSchillingAnalysis_menu(void);
void AmuletWellChangeROI_menu(void);
void AmuletWellGeneralAnalysis_menu(void);
void AmuletSetupRHotkeys_menu(void);
void AmuletSetupPHotkeys_menu(void);
void AmuletRemote_menu(void);
void AmuletDeleteLineator_menu(void);
void AmuletDeleteCalicheck_menu(void);
void AmuletSelectLinearity_menu(void);
void AmuletQC_menu(void);
void AmuletLowLevel_menu(void);
void AmuletWellStabilityTest_menu(void);
void AmuletSetupCommunications_menu(void);
void AmuletPCQC_menu(void);
void AmuletEthernet_menu(void);
void AmuletWellSetupTypes_menu(void);
void AmuletWellSetupLocations_menu(void);
void AmuletWellSetupAddEditLocation_menu(void);
void AmuletWellSetupDeleteLocation_menu(void);
void AmuletWellWipeList_menu(void);
void AmuletErrorMsgLarge_menu(void);
void AmuletWellSpectrum_menu(void);
void AmuletWellWipeSearch_menu(void);
void AmuletGenericItems_menu(void);
void AmuletInactivate_menu(void);
void AmuletWellSystemTestSearch_menu(void);
void AmuletWellMDASearch_menu(void);
void AmuletWellMDAAnalysis_menu(void);
void AmuletWellChiSearch_menu(void);
void AmuletWellChiAnalysis_menu(void);
void AmuletTestIdent_menu(void);
void AmuletWellSchillingSearch_menu(void);
void AmuletWellSchillingReport_menu(void);
void AmuletWellPlasmaSearch_menu(void);
void AmuletWellPlasmaReport_menu(void);
void AmuletWellRBCSearch_menu(void);
void AmuletWellRBCReport_menu(void);
void AmuletWellAutoCalSearch_menu(void);
void AmuletWellAutoCalReport_menu(void);
void AmuletWellFactoryDetectors_menu(void);
void AmuletWellSetupUserNuclides_menu(void);
void AmuletWellAddEditUserNuclides_menu(void);
void AmuletWellLabTests_menu(void);
void AmuletWellReports_menu(void);
void AmuletLogin_menu(void);
void AmuletSecurity_menu(void);
void AmuletAddEditUser_menu(void);
void AmuletWellSetupBioAssay_menu(void);
void AmuletWellSetupBioAssayEnterEfficiency_menu(void);
void AmuletWellSetupBioAssayMeasureEfficiency_menu(void);
void AmuletWellBioAssay_menu(void);
void AmuletWellBioAssayAnalysis_menu(void);
void AmuletWellBioAssaySearch_menu(void);
void AmuletWellSetupBioAssayEnterROI_menu(void);
void AmuletWellSetupThyroidUptakeProtocol_menu(void);
void AmuletAddEditThyroidUptakeProtocol_menu(void);
void AmuletWellThyroidUptake_menu(void);
void AmuletAddEditThyroidUptakeTest_menu(void);
void AmuletWellThyroidUptakeTest_menu(void);
void AmuletWellThyroidUptakeEnterAdministrationDate_menu(void);
void AmuletWellThyroidUptakeMeasureDose_menu(void);
void AmuletWellThyroidUptakeMeasurePatient_menu(void);
void AmuletWellThyroidUptakeEnterNormal_menu(void);
void AmuletWellRBCSurvival_menu(void);
void AmuletAddEditRBCSurvivalTest_menu(void);
void AmuletWellRBCSurvivalTest_menu(void);
void AmuletWellRBCSurvivalMeasurement_menu(void);
void AmuletWellRBCSurvivalEnterNormal_menu(void);
void AmuletAutoLinearity_menu(void);
void AmuletAutoLinearityTest_menu(void);
void AmuletGenericYesNo_menu(void);
void AmuletAutoLinearitySearch_menu(void);
void AmuletChamberReports_menu(void);
void AmuletChamberSearch_menu(void);
void AmuletChamberDailyTestView_menu(void);
void AmuletChamberZeroTestView_menu(void);
void AmuletChamberBackgroundTestView_menu(void);
void AmuletChamberVoltageTestView_menu(void);
void AmuletChamberAccuracyTestView_menu(void);
void AmuletChamberAutoconstancyTestView_menu(void);
void AmuletChamberHalflifeCalc_menu(void);
void AmuletSetupKey_menu(void);
void AmuletSetupPassword_menu(void);
bool AmuletWellMeasurementMenu_drawGraph(unsigned char *display, bool reset);
void display_inventory(short *map, short *first_map_item_displayed, short *selected_map_item, short last_map_item);
void display_nuclide(short *first_map_item_displayed, short *selected_map_item, char tagbothusercalnum);
char ntoh(unsigned char ucValue);
void ctoh(unsigned char ucValue, char *pcOutput);
void itoh(short unsigned int uiValue, char *pcOutput);
unsigned char ConvertNibble(char cInput);
unsigned char ConvertByte(char *pcInput);
short unsigned int ConvertWord(char *pcInput);

char IsMSOM(unsigned char ucValue);
char IsSSOM(unsigned char ucValue);

void ClearMasterMessage(void);
short unsigned int QueryPendingMasterMessage(void);
char PushMasterMessage(char *pcMessage);
char PopMasterMessage(char *pcOutput);

void ClearSlaveMessage(void);
short unsigned int QueryPendingSlaveMessage(void);
char PushSlaveMessage(char *pcMessage);
char PopSlaveMessage(char *pcOutput);

void ClearCmdQueue(void);
short unsigned int QueryPendingCmdQueue(void);
char InCmdQueue(int iPosition);
char PushCmdQueue(char *pcCommand, void *ptrOutput, void (*ptrFunction) (void *ptrInput), unsigned long int ulResendTimeout, unsigned long int ulExpireTimeout);
void RemoveCmdQueue(int iPosition);
void CompressCmdQueue(void);
char ScanCmdQueueCommand(char *pcCmd, CMDSENT *pCmdSent);
char ScanCmdQueueResend(CMDSENT *pCmdSent);
char ScanCmdQueueExpired(CMDSENT *pCmdSent);

void PushPacket(char *pcPacketArray);
char CheckPacket(char *pcPacketArray);

char SendUart2Command (char *cOutput, void *ptrOutput, void (*ptrFunction)(void *ptrInput), unsigned long int ulResendTimeout, unsigned long int ulExpireTimeout);
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletWord(unsigned char ucIndex, short unsigned int uiValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
char SetAmuletHTML(unsigned int uiValue);
char SetAmuletLine(ushort x1, ushort y1, ushort x2, ushort y2, ulong rgb, uchar weight);
char SetAmuletFillRect(ushort x1, ushort y1, ushort dx, ushort dy, ulong rgb, uchar weight);

void print_uart(void);

extern unsigned char AmuletHTMLIndex[AMULET_HTML_ARRAY_SIZE];

void LoadAmuletHTML(int board);

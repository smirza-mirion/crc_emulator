#include "crc.h"

const char lang_str[]={
'R','e','v',' ',		// L_REVI (english)  4(0), 0
'V','e','r',' ',		// L_REVI (french)  4(0)
'R','e','v',':',		// L_PR_REV (english)  4(0), 1
'V','e','r',':',		// L_PR_REV (french)  4(0)
'R','E','V',' ',' ',' ',' ',' ',' ',' ','S','N',':',' ',		// L_REV_SN (english)  14(0), 2
'V','E','R',' ',' ',' ',' ',' ',' ',' ','S','N',':',' ',		// L_REV_SN (french)  14(0)
'C','O','P','Y','R','I','G','H','T',		// L_COPYRIGHT (english)  9(0), 3
'D','e','r','e','c','h','o',' ','d','e',' ','A','u','t','o','r',		// L_COPYRIGHT (french)  16(0)
'A','L','L',' ','R','I','G','H','T','S',' ','R','E','S','E','R','V','E','D',		// L_RIGHTS (english)  19(0), 4
'D','E','R','E','C','H','O','S',' ','R','E','S','E','R','V','A','D','O','S',		// L_RIGHTS (french)  19(0)
'C','o','n','t','i','n','u','e',		// L_CONTINUE_BTN (english)  8(0), 5
'C','o','n','t','i','n','u','a','r',		// L_CONTINUE_BTN (french)  9(0)
'L','o','g','i','n',		// L_LOGIN (english)  5(0), 6
'L','o','g','i','n',		// L_LOGIN (french)  5(0)
'U','p','d','a','t','e',' ','S','y','s','t','e','m',		// L_UPDATE_SYSTEM (english)  13(0), 7
'A','c','t','u','a','l','i','z','a','c','i','ó','n',' ','d','e','l',' ','S','i','s','t','e','m','a',		// L_UPDATE_SYSTEM (french)  26(1)
'O','F','F',		// L_TIMEOUT_OFF (english)  3(0), 8
'N','O',		// L_TIMEOUT_OFF (french)  2(0)
'B','r','i','g','h','t','n','e','s','s',':',		// L_BRIGHTNESS (english)  11(0), 9
'L','u','m','i','n','o','s','i','d','a','d',':',		// L_BRIGHTNESS (french)  12(0)
'S','l','e','e','p',' ','B','r','i','g','h','t','n','e','s','s',':',		// L_SLEEP_BRIGHTNESS (english)  17(0), 10
'B','r','i','l','l','o',' ','e','n',' ','E','s','p','e','r','a',':',		// L_SLEEP_BRIGHTNESS (french)  17(0)
'S','l','e','e','p',' ','T','i','m','e','o','u','t',':',		// L_SLEEP_TIMEOUT (english)  14(0), 11
'T','i','e','m','p','o',' ','d','e',' ','E','s','p','e','r','a',':',		// L_SLEEP_TIMEOUT (french)  17(0)
'V','o','l','u','m','e',':',		// L_VOLUME (english)  7(0), 12
'V','o','l','u','m','e','n',':',		// L_VOLUME (french)  8(0)
'S','e','t','u','p',		// L_SETUP_TITLE (english)  5(0), 13
'C','o','n','f','i','g','u','r','a','r',		// L_SETUP_TITLE (french)  10(0)
'A','d','v','a','n','c','e','d',' ','C','h','a','m','b','e','r',		// L_ADVANCED_CHAMBER (english)  16(0), 14
'C','á','m','a','r','a',' ','A','v','a','n','z','a','d','a',		// L_ADVANCED_CHAMBER (french)  16(0)
'A','d','v','a','n','c','e','d',' ','D','e','t','e','c','t','o','r',		// L_ADVANCED_DETECTOR (english)  17(0), 15
'D','e','t','e','c','t','o','r',' ','A','v','a','n','z','a','d','o',		// L_ADVANCED_DETECTOR (french)  17(0)
'S','t','a','f','f',		// L_STAFF (english)  5(0), 16
'P','e','r','s','o','n','a','l',		// L_STAFF (french)  8(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','P','a','s','s','w','o','r','d',':',		// L_ENTER_PASSWORD (english)  22(0), 17
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','C','o','n','t','r','a','s','e','ñ','a',':',		// L_ENTER_PASSWORD (french)  32(1)
'T','e','s','t',		// L_TEST (english)  4(0), 18
'P','r','u','e','b','a',		// L_TEST (french)  6(0)
'T','e','s','t',':',		// L_TEST2 (english)  5(0), 19
'P','r','u','e','b','a',':',		// L_TEST2 (french)  7(0)
'A','c','t','i','v','i','t','y',' ','U','n','i','t',':',		// L_ACTIVITY_UNIT (english)  14(0), 20
'U','n','i','d','a','d',' ','d','e',' ','A','c','t',':',		// L_ACTIVITY_UNIT (french)  14(0)
'D','a','t','e',' ','F','o','r','m','a','t',':',		// L_SETUP_DATE_FORMAT (english)  12(0), 21
'F','o','r','m','a','t','o',' ','d','e',' ','F','e','c','h','a',':',		// L_SETUP_DATE_FORMAT (french)  17(0)
'P','r','i','n','t','e','r',':',		// L_SETUP_PRINTER (english)  8(0), 22
'I','m','p','r','e','s','a',':',		// L_SETUP_PRINTER (french)  8(0)
'S','e','t','u','p',' ','S','o','u','r','c','e','s',		// L_SETUP_SOURCES (english)  13(0), 23
'C','o','n','f','i','g','u','r','a','r',' ','F','u','e','n','t','e','s',		// L_SETUP_SOURCES (french)  18(0)
'S','e','t','u','p',' ','M','o','l','y',		// L_SETUP_MOLY (english)  10(0), 24
'C','o','n','f','i','g','u','r','a','r',' ','M','o','l','y',		// L_SETUP_MOLY (french)  15(0)
'S','e','t','u','p',' ','N','u','c','l','i','d','e',		// L_SETUP_NUCLIDES (english)  13(0), 25
'C','o','n','f','i','g','u','r','a','r',' ','N','u','c','l','e','i','d','o',		// L_SETUP_NUCLIDES (french)  19(0)
'S','e','t','u','p',' ','C','a','l','N','u','m',		// L_SETUP_CALNUM (english)  12(0), 26
'C','o','n','f','i','g','u','r','a','r',' ','C','a','l','N','u','m',		// L_SETUP_CALNUM (french)  17(0)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',		// L_SETUP_LINEARITY (english)  15(0), 27
'C','o','n','f','i','g','u','r','a','r',' ','L','i','n','e','a','l','i','d','a','d',		// L_SETUP_LINEARITY (french)  21(0)
'S','e','t','u','p',' ','R','e','m','o','t','e',		// L_SETUP_REMOTE (english)  12(0), 28
'C','o','n','f','i','g','u','r','a','r',' ','R','e','m','o','t','a',		// L_SETUP_REMOTE (french)  17(0)
'S','e','t','u','p',' ','R',' ','C','h','a','m','b','e','r',' ','H','o','t','k','e','y','s',		// L_SETUP_R_HOTKEYS (english)  23(0), 29
'C','o','n','f','i','g','.',' ','T','e','c','l','a',' ','R','a','p','i','d','a',' ','C','á','m','a','r','a',' ','R',		// L_SETUP_R_HOTKEYS (french)  30(1)
'S','e','t','u','p',' ','P','E','T',' ','C','h','a','m','b','e','r',' ','H','o','t','k','e','y','s',		// L_SETUP_PET_HOTKEYS (english)  25(0), 30
'C','o','n','f','i','g','.',' ','T','e','c','l','a',' ','R','a','p','i','d','a',' ','C','á','m','a','r','a',' ','P','E','T',		// L_SETUP_PET_HOTKEYS (french)  32(1)
'A','d','v','a','n','c','e','d',' ','C','h','a','m','b','e','r',' ','S','e','t','u','p',		// L_ADVANCED_CHAMBER_SETUP_TITLE (english)  22(0), 31
'C','o','n','f','i','g','u','r','a','r',' ','C','á','m','a','r','a',' ','A','v','a','n','z','a','d','a',		// L_ADVANCED_CHAMBER_SETUP_TITLE (french)  27(1)
'A','C','T','I','V','I','T','Y',		// L_SETUP_SOURCES_ACTIVITY (english)  8(0), 32
'A','C','T','I','V','I','D','A','D',		// L_SETUP_SOURCES_ACTIVITY (french)  9(0)
'D','A','T','E',		// L_SETUP_SOURCES_DATE (english)  4(0), 33
'F','E','C','H','A',		// L_SETUP_SOURCES_DATE (french)  5(0)
'D','A','I','L','Y',		// L_SETUP_SOURCES_DAILY (english)  5(0), 34
'D','I','A','R','I','A',		// L_SETUP_SOURCES_DAILY (french)  6(0)
'C','o','n','s','t','a','n','c','y',' ','C','h','a','n','n','e','l','s',		// L_SETUP_SOURCES_CONSTANCY_CHANNELS (english)  18(0), 35
'C','a','n','a','l','e','s',' ','d','e',' ','E','s','t','a','b','i','l','i','d','a','d',		// L_SETUP_SOURCES_CONSTANCY_CHANNELS (french)  22(0)
'C','l','e','a','r',		// L_CLEAR_BUTTON (english)  5(0), 36
'B','o','r','r','a','r',		// L_CLEAR_BUTTON (french)  6(0)
'A','c','c','e','p','t',		// L_ACCEPT_BUTTON (english)  6(0), 37
'A','c','e','p','t','a','r',		// L_ACCEPT_BUTTON (french)  7(0)
'C','a','n','c','e','l',		// L_CANCEL_BUTTON (english)  6(0), 38
'C','a','n','c','e','l','a','r',		// L_CANCEL_BUTTON (french)  8(0)
'M','o','l','y',' ','M','e','t','h','o','d',':',		// L_SETUP_MOLY_METHOD (english)  12(0), 39
'M','é','t','o','d','o',' ','d','e',' ','M','o','l','y',':',		// L_SETUP_MOLY_METHOD (french)  16(0)
'M','o','/','T','c',' ','L','i','m','i','t',':',		// L_SETUP_MOLY_LIMIT (english)  12(0), 40
'M','o','/','T','c',' ','L','í','m','i','t','e',':',		// L_SETUP_MOLY_LIMIT (french)  14(0)
'N','u','c','l','i','d','e',		// L_NUCLIDE (english)  7(0), 41
'N','u','c','l','e','i','d','o',		// L_NUCLIDE (french)  8(0)
'E','l','e','m','e','n','t',		// L_SETUP_NUCLIDE_ELEMENT (english)  7(0), 42
'E','l','e','m','e','n','t','o',		// L_SETUP_NUCLIDE_ELEMENT (french)  8(0)
'H','a','l','f','l','i','f','e',		// L_SETUP_NUCLIDE_HALFLIFE (english)  8(0), 43
'V','i','d','a',' ','m','e','d','i','a',		// L_SETUP_NUCLIDE_HALFLIFE (french)  10(0)
'E','n','g','l','i','s','h',		// L_ENGLISH (english)  7(0), 44
'I','n','g','l','é','s',		// L_ENGLISH (french)  7(0)
'S','p','a','n','i','s','h',		// L_FRENCH (english)  7(0), 45
'E','s','p','a','ñ','o','l',		// L_FRENCH (french)  8(0)
'L','a','n','g','u','a','g','e',':',		// L_LANGUAGE (english)  9(0), 46
'I','d','i','o','m','a',':',		// L_LANGUAGE (french)  7(0)
'D','e','f','a','u','l','t',		// L_DEFAULT (english)  7(0), 47
'P','r','e','d','e','t','e','r','m','i','n','a','d','o',		// L_DEFAULT (french)  14(0)
'U','s','e','r',		// L_USER (english)  4(0), 48
'U','s','u','a','r','i','o',		// L_USER (french)  7(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','N','u','c','l','i','d','e',		// L_ENTER_NUCLIDE (english)  20(0), 49
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','N','u','c','l','e','i','d','o',		// L_ENTER_NUCLIDE (french)  28(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','E','l','e','m','e','n','t',		// L_ENTER_ELEMENT (english)  20(0), 50
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','E','l','e','m','e','n','t','o',		// L_ENTER_ELEMENT (french)  28(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','H','a','l','f','l','i','f','e',		// L_ENTER_HALFLIFE (english)  21(0), 51
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','V','i','d','a',' ','m','e','d','i','a',		// L_ENTER_HALFLIFE (french)  30(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','R',' ','C','a','l',' ','#',':',		// L_ENTER_RCAL (english)  21(0), 52
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','R',' ','C','a','l',' ','#',':',		// L_ENTER_RCAL (french)  28(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','P',' ','C','a','l',' ','#',':',		// L_ENTER_PCAL (english)  21(0), 53
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','P',' ','C','a','l',' ','#',':',		// L_ENTER_PCAL (french)  28(1)
'B','a','c','k','s','p','a','c','e',		// L_BACKSPACE (english)  9(0), 54
'R','e','t','r','o','c','e','d','e','r',		// L_BACKSPACE (french)  10(0)
'B','a','c','k','S','P',		// L_BACKSPACE_SHORT (english)  6(0), 55
'R','e','g','r','e','s','o',		// L_BACKSPACE_SHORT (french)  7(0)
'S','h','i','f','t',		// L_SHIFT (english)  5(0), 56
'S','h','i','f','t',		// L_SHIFT (french)  5(0)
'<','-','-',' ','W','i','l','d','c','a','r','d',		// L_WILDCARD (english)  12(0), 57
'<','-','-',' ','C','o','m','o','d','í','n',		// L_WILDCARD (french)  12(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',		// L_PLEASE_ENTER (english)  12(0), 58
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',		// L_PLEASE_ENTER (french)  19(0)
'S','/','N',		// L_SN (english)  3(0), 59
'S','/','N',		// L_SN (french)  3(0)
'S','/','N',':',		// L_SN2 (english)  4(0), 60
'S','/','N',':',		// L_SN2 (french)  4(0)
'C','a','l','i','b','r','a','t','i','o','n',' ','T','i','m','e',		// L_CALIBRATION_TIME (english)  16(0), 61
'T','i','e','m','p','o',' ','d','e',' ','C','a','l','i','b','r','a','c','i','ó','n',		// L_CALIBRATION_TIME (french)  22(0)
'A','c','t','i','v','i','t','y',		// L_ACTIVITY (english)  8(0), 62
'A','c','t','i','v','i','d','a','d',		// L_ACTIVITY (french)  9(0)
'S','t','a','n','d','a','r','d',' ','R',' ','C','h','a','m','b','e','r',		// L_STANDARD_R_CHAMBER (english)  18(0), 63
'E','s','t','á','n','d','a','r',' ','c','á','m','a','r','a',' ','R',		// L_STANDARD_R_CHAMBER (french)  19(0)
'S','t','a','n','d','a','r','d',' ','P','E','T',' ','C','h','a','m','b','e','r',		// L_STANDARD_PET_CHAMBER (english)  20(0), 64
'E','s','t','á','n','d','a','r',' ','c','á','m','a','r','a',' ','P','E','T',		// L_STANDARD_PET_CHAMBER (french)  21(0)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','S','t','a','n','d','a','r','d',' ','-',' ','R',' ','C','h','a','m','b','e','r',		// L_SETUP_LINEARITY_STANDARD_R (english)  36(1), 65
'C','o','n','f','i','g','u','r','a',' ','e','l',' ','e','s','t','á','n','d','a','r',' ','d','e',' ','l','i','n','e','a','l','i','d','a','d',' ','-',' ','c','á','m','a','r','a',' ','R',		// L_SETUP_LINEARITY_STANDARD_R (french)  48(1)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','S','t','a','n','d','a','r','d',' ','-',' ','P','E','T',' ','C','h','a','m','b','e','r',		// L_SETUP_LINEARITY_STANDARD_PET (english)  38(1), 66
'C','o','n','f','i','g','u','r','a',' ','e','l',' ','e','s','t','á','n','d','a','r',' ','d','e',' ','l','i','n','e','a','l','i','d','a','d',' ','-',' ','c','á','m','a','r','a',' ','P','E','T',		// L_SETUP_LINEARITY_STANDARD_PET (french)  50(1)
'N','u','m',' ','o','f',' ','M','e','a','s','u','r','e','m','e','n','t','s',':',		// L_NUM_OF_MEASUREMENTS (english)  20(0), 67
'N','u','m','e','r','o',' ','d','e',' ','m','e','d','i','d','a','s',':',		// L_NUM_OF_MEASUREMENTS (french)  18(0)
'M','e','a','s','u','r','e','d',' ','O','n',		// L_MEASURED_ON (english)  11(0), 68
'M','e','d','i','d','o',' ','E','n',		// L_MEASURED_ON (french)  9(0)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','E','r','r','o','r',		// L_SETUP_LINEARITY_ERROR (english)  21(0), 69
'E','r','r','o','r',' ','e','n',' ','C','o','n','f','i','g','.',' ','L','i','n','e','a','l','i','d','a','d',		// L_SETUP_LINEARITY_ERROR (french)  27(1)
'P','l','e','a','s','e',' ','F','i','l','l',' ','i','n',' ','A','l','l',' ','H','o','u','r','s',		// L_FILL_IN_HOURS (english)  24(0), 70
'P','o','r',' ','f','a','v','o','r',' ','l','l','e','n','e',' ','t','o','d','a','s',' ','l','a','s',' ','h','o','r','a','s',		// L_FILL_IN_HOURS (french)  31(1)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','h','o','u','r','s',' ','i','n',' ','a','s','c','e','n','d','i','n','g',' ','o','r','d','e','r',		// L_ASCENDING_ORDER (english)  37(1), 71
'P','o','r',' ','f','a','v','o','r',',',' ','i','n','t','r','o','d','u','z','c','a',' ','h','o','r','a','s',' ','e','n',' ','o','r','d','e','n',' ','a','s','c','e','n','d','e','n','t','e',		// L_ASCENDING_ORDER (french)  47(1)
'O','K',		// L_CAPS_OK (english)  2(0), 72
'B','I','E','N',		// L_CAPS_OK (french)  4(0)
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','W','i','p','e',' ','C','r','i','t','e','r','i','a',		// L_SELECT_WIPE_CRITERIA (english)  27(1), 73
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','W','i','p','e',' ','C','r','i','t','e','r','i','a',		// L_SELECT_WIPE_CRITERIA (french)  27(1)
'A','l','l',' ','W','i','p','e','s',		// L_ALL_WIPES (english)  9(0), 74
'A','l','l',' ','W','i','p','e','s',		// L_ALL_WIPES (french)  9(0)
'W','o','r','k',' ','A','r','e','a',		// L_WORK_AREA (english)  9(0), 75
'W','o','r','k',' ','A','r','e','a',		// L_WORK_AREA (french)  9(0)
'U','n','r','e','s','t','r','i','c','t','e','d',' ','A','r','e','a',		// L_UNRESTRICTED_AREA (english)  17(0), 76
'U','n','r','e','s','t','r','i','c','t','e','d',' ','A','r','e','a',		// L_UNRESTRICTED_AREA (french)  17(0)
'S','e','a','l','e','d',' ','S','o','u','r','c','e',		// L_SEALED_SOURCE (english)  13(0), 77
'S','e','a','l','e','d',' ','S','o','u','r','c','e',		// L_SEALED_SOURCE (french)  13(0)
'P','a','c','k','a','g','e',		// L_PACKAGE (english)  7(0), 78
'P','a','c','k','a','g','e',		// L_PACKAGE (french)  7(0)
'H','i','g','h',' ','A','c','t','i','v','i','t','y',		// L_HIGH_ACTIVITY (english)  13(0), 79
'H','i','g','h',' ','A','c','t','i','v','i','t','y',		// L_HIGH_ACTIVITY (french)  13(0)
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','D','e','t','e','c','t','o','r',		// L_SELECT_DETECTOR (english)  22(0), 80
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','D','e','t','e','c','t','o','r',		// L_SELECT_DETECTOR (french)  22(0)
'P','r','o','b','e',		// L_PROBE (english)  5(0), 81
'P','r','o','b','e',		// L_PROBE (french)  5(0)
'W','e','l','l',		// L_WELL (english)  4(0), 82
'W','e','l','l',		// L_WELL (french)  4(0)
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','B','r','a','n','d','i','n','g',		// L_SELECT_BRANDING (english)  22(0), 83
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','B','r','a','n','d','i','n','g',		// L_SELECT_BRANDING (french)  22(0)
'A','l','l',		// L_ALL (english)  3(0), 84
'A','l','l',		// L_ALL (french)  3(0)
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','C','o','u','n','t','i','n','g',' ','M','e','t','h','o','d',		// L_SELECT_TU_METHOD (english)  29(1), 85
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','C','o','u','n','t','i','n','g',' ','M','e','t','h','o','d',		// L_SELECT_TU_METHOD (french)  29(1)
'D','e','c','a','y',' ','C','o','r','r','e','c','t',' ','A','d','m','i','n','i','s','t','e','r','e','d',' ','D','o','s','e',		// L_DECAY_CORRECT_ADMIN_DOSE (english)  31(1), 86
'D','e','c','a','y',' ','C','o','r','r','e','c','t',' ','A','d','m','i','n','i','s','t','e','r','e','d',' ','D','o','s','e',		// L_DECAY_CORRECT_ADMIN_DOSE (french)  31(1)
'M','e','a','s','u','r','e',' ','S','a','m','e',' ','R','e','f','e','r','e','n','c','e',' ','D','o','s','e',' ','B','e','f','o','r','e',' ','E','a','c','h',' ','U','p','t','a','k','e',		// L_MEAS_REF_DOSE (english)  46(1), 87
'M','e','a','s','u','r','e',' ','S','a','m','e',' ','R','e','f','e','r','e','n','c','e',' ','D','o','s','e',' ','B','e','f','o','r','e',' ','E','a','c','h',' ','U','p','t','a','k','e',		// L_MEAS_REF_DOSE (french)  46(1)
'S','e','l','e','c','t',' ','D','o','s','e',' ','M','e','a','s','u','r','e','m','e','n','t',		// L_SELECT_DOSE_MEASUREMENT (english)  23(0), 88
'S','e','l','e','c','t',' ','D','o','s','e',' ','M','e','a','s','u','r','e','m','e','n','t',		// L_SELECT_DOSE_MEASUREMENT (french)  23(0)
'D','o','s','e',' ','M','e','a','s','u','r','e','d',' ','i','s',' ','D','o','s','e',' ','A','d','m','i','n','i','s','t','e','r','e','d',		// L_DOSE_MEAS_IS_DOSE_ADMIN (english)  34(1), 89
'D','o','s','e',' ','M','e','a','s','u','r','e','d',' ','i','s',' ','D','o','s','e',' ','A','d','m','i','n','i','s','t','e','r','e','d',		// L_DOSE_MEAS_IS_DOSE_ADMIN (french)  34(1)
'M','e','a','s','u','r','e',' ','E','a','c','h',' ','D','o','s','e',' ','a','n','d',' ','A','d','d',' ','A','c','t','i','v','i','t','y',		// L_MEAS_DOSE_AND_ADD (english)  34(1), 90
'M','e','a','s','u','r','e',' ','E','a','c','h',' ','D','o','s','e',' ','a','n','d',' ','A','d','d',' ','A','c','t','i','v','i','t','y',		// L_MEAS_DOSE_AND_ADD (french)  34(1)
'M','e','a','s','u','r','e',' ','O','n','e',' ','C','a','p','s','u','l','e',' ','a','n','d',' ','M','u','l','t','i','p','l','y',' ','b','y',' ','N','u','m','b','e','r',' ','A','d','m','i','n','i','s','t','e','r','e','d',		// L_MEAS_DOSE_AND_MULTIP (english)  55(2), 91
'M','e','a','s','u','r','e',' ','O','n','e',' ','C','a','p','s','u','l','e',' ','a','n','d',' ','M','u','l','t','i','p','l','y',' ','b','y',' ','N','u','m','b','e','r',' ','A','d','m','i','n','i','s','t','e','r','e','d',		// L_MEAS_DOSE_AND_MULTIP (french)  55(2)
'M','e','a','s','u','r','e',' ','L','i','q','u','i','d',' ','a','n','d',' ','M','u','l','t','i','p','l','y',' ','b','y',' ','F','a','c','t','o','r',		// L_MEAS_LIQ_AND_MULTIP (english)  37(1), 92
'M','e','a','s','u','r','e',' ','L','i','q','u','i','d',' ','a','n','d',' ','M','u','l','t','i','p','l','y',' ','b','y',' ','F','a','c','t','o','r',		// L_MEAS_LIQ_AND_MULTIP (french)  37(1)
'S','e','l','e','c','t',' ','N','u','c','l','i','d','e',		// L_SELECT_NUCLIDE (english)  14(0), 93
'S','e','l','e','c','t',' ','N','u','c','l','i','d','e',		// L_SELECT_NUCLIDE (french)  14(0)
'S','e','l','e','c','t',' ','D','o','s','e',' ','F','o','r','m',		// L_SELECT_DOSE_FORM (english)  16(0), 94
'S','e','l','e','c','t',' ','D','o','s','e',' ','F','o','r','m',		// L_SELECT_DOSE_FORM (french)  16(0)
'C','a','p','s','u','l','e',		// L_CAPSULE (english)  7(0), 95
'C','a','p','s','u','l','e',		// L_CAPSULE (french)  7(0)
'L','i','q','u','i','d',		// L_LIQUID (english)  6(0), 96
'L','i','q','u','i','d',		// L_LIQUID (french)  6(0)
'M','e','a','s','u','r','e',' ','P','r','e','-','D','o','s','e',' ','P','a','t','i','e','n','t','?',		// L_MEAS_PREDOSE_PATIENT (english)  25(0), 97
'M','e','a','s','u','r','e',' ','P','r','e','-','D','o','s','e',' ','P','a','t','i','e','n','t','?',		// L_MEAS_PREDOSE_PATIENT (french)  25(0)
'Y','e','s',		// L_YES (english)  3(0), 98
'S','í',		// L_YES (french)  3(0)
'N','o',		// L_NO (english)  2(0), 99
'N','o',		// L_NO (french)  2(0)
'D','e','l','e','t','e',' ','T','h','y','r','o','i','d',' ','U','p','t','a','k','e',' ','T','e','s','t','?',		// L_DELETE_THYROID (english)  27(1), 100
'D','e','l','e','t','e',' ','T','h','y','r','o','i','d',' ','U','p','t','a','k','e',' ','T','e','s','t','?',		// L_DELETE_THYROID (french)  27(1)
'M','e','a','s','u','r','e',' ','R','e','s','i','d','u','a','l',' ','D','o','s','e','?',		// L_MEAS_REDISUAL (english)  22(0), 101
'M','e','a','s','u','r','e',' ','R','e','s','i','d','u','a','l',' ','D','o','s','e','?',		// L_MEAS_REDISUAL (french)  22(0)
'R','e','A','c','t','i','v','a','t','e',' ','T','h','y','r','o','i','d',' ','U','p','t','a','k','e',' ','T','e','s','t','?',		// L_REACTIVATE_TU (english)  31(1), 102
'R','e','A','c','t','i','v','a','t','e',' ','T','h','y','r','o','i','d',' ','U','p','t','a','k','e',' ','T','e','s','t','?',		// L_REACTIVATE_TU (french)  31(1)
'U','s','e',' ','O','n','l','y',' ','D','e','f','a','u','l','t',' ','N','u','c','l','i','d','e',' ','k','e','V','?',		// L_ONLY_DEFAULT_NUC (english)  29(1), 103
'U','s','e',' ','O','n','l','y',' ','D','e','f','a','u','l','t',' ','N','u','c','l','i','d','e',' ','k','e','V','?',		// L_ONLY_DEFAULT_NUC (french)  29(1)
'R','e','A','c','t','i','v','a','t','e',' ','R','B','C',' ','S','u','r','v','i','v','a','l',' ','T','e','s','t','?',		// L_REACTIVATE_RBC (english)  29(1), 104
'R','e','A','c','t','i','v','a','t','e',' ','R','B','C',' ','S','u','r','v','i','v','a','l',' ','T','e','s','t','?',		// L_REACTIVATE_RBC (french)  29(1)
'D','e','l','e','t','e',' ','R','B','C',' ','S','u','r','v','i','v','a','l',' ','T','e','s','t','?',		// L_DELETE_RBC (english)  25(0), 105
'D','e','l','e','t','e',' ','R','B','C',' ','S','u','r','v','i','v','a','l',' ','T','e','s','t','?',		// L_DELETE_RBC (french)  25(0)
'U','p','d','a','t','e',' ','t','o',' ','R','e','v',':',' ','%','s',',',' ','P','r','o','c','e','e','d','?',		// L_UPDATE_TO_REV (english)  27(1), 106
'A','c','t','u','a','l','i','z','a','r',' ','a',' ','R','e','v',':',' ','%','s',',',' ','¿','P','r','o','c','e','d','e','?',		// L_UPDATE_TO_REV (french)  32(1)
'T','h','i','s',' ','v','e','r','s','i','o','n',' ','w','i','l','l',' ','d','i','s','a','b','l','e',' ','U','S','B',' ','U','p','d','a','t','e','.',' ','P','r','o','c','e','e','d','?',		// L_UPDATE_DISABLES_USB (english)  46(1), 107
'E','s','t','á',' ','v','e','r','s','i','ó','n',' ','d','e','s','a','c','t','i','v','a',' ','A','c','t','u','a','l','i','z','a','c','i','ó','n',' ','U','S','B','.',' ','¿','P','r','o','c','e','d','e','?',		// L_UPDATE_DISABLES_USB (french)  55(2)
'O','v','e','r','w','r','i','t','e',' ','s','e','t','t','i','n','g','s',' ','w','i','t','h',' ','d','e','f','a','u','l','t','s',' ','f','r','o','m',' ','%','s','?',		// L_OVERWRITE_SETTING_WITH_DEFAULT (english)  41(1), 108
'O','v','e','r','w','r','i','t','e',' ','s','e','t','t','i','n','g','s',' ','w','i','t','h',' ','d','e','f','a','u','l','t','s',' ','f','r','o','m',' ','%','s','?',		// L_OVERWRITE_SETTING_WITH_DEFAULT (french)  41(1)
'P','l','e','a','s','e',' ','S','e','l','e','c','t',' ','D','a','t','e',' ','F','o','r','m','a','t',		// L_SELECT_DATE_FORMAT (english)  25(0), 109
'F','a','v','o','r',' ','d','e',' ','S','e','l','e','c','c','i','o','n','a','r',' ','e','l',' ','F','o','r','m','a','t','o',' ','d','e',' ','F','e','c','h','a',		// L_SELECT_DATE_FORMAT (french)  40(1)
'P','l','e','a','s','e',' ','S','e','l','e','c','t',' ','U','S','B',' ','P','C',' ','D','r','i','v','e','r',		// L_SELECT_USB_DRIVER (english)  27(1), 110
'F','a','v','o','r',' ','d','e',' ','S','e','l','e','c','c','i','o','n','a','r',' ','e','l',' ','C','o','n','t','r','o','l','a','d','o','r',' ','U','S','B',' ','P','C',		// L_SELECT_USB_DRIVER (french)  42(1)
'L','e','g','a','c','y',		// L_LEGACY (english)  6(0), 111
'L','e','g','a','d','o',		// L_LEGACY (french)  6(0)
'C','D','C',		// L_CDC (english)  3(0), 112
'C','D','C',		// L_CDC (french)  3(0)
'P','l','e','a','s','e',' ','S','e','l','e','c','t',' ','D','o','s','e',' ','D','e','c','a','y',' ','E','n','t','r','y',' ','M','o','d','e',		// L_SELECT_DOSE_DECAY_ENTRY_MODE (english)  35(1), 113
'S','e','l','e','c','c','i','o','n','e',' ','M','o','d','o',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','D','e','c','a','i','m','i','e','n','t','o',' ','d','e',' ','D','o','s','i','s',		// L_SELECT_DOSE_DECAY_ENTRY_MODE (french)  50(1)
'Q','u','i','c','k',		// L_QUICK (english)  5(0), 114
'R','á','p','i','d','o',		// L_QUICK (french)  7(0)
'F','u','l','l',		// L_FULL (english)  4(0), 115
'L','l','e','n','o',		// L_FULL (french)  5(0)
'P','l','e','a','s','e',' ','S','e','l','e','c','t',' ','L','a','n','g','u','a','g','e',		// L_SELECT_LANGUAGE (english)  22(0), 116
'F','a','v','o','r',' ','d','e',' ','S','e','l','e','c','c','i','o','n','a','r',' ','e','l',' ','I','d','i','o','m','a',		// L_SELECT_LANGUAGE (french)  30(1)
'C','R','C','-','5','5','t',' ',' ',' ',' ',' ',' ','R','A','D','I','O','I','S','O','T','O','P','E',' ','D','O','S','E',' ','C','A','L','I','B','R','A','T','O','R',		// L_55T_FULL_TITLE (english)  41(1), 117
'C','R','C','-','5','5','t',' ',' ',' ',' ',' ',' ','C','A','L','I','B','R','A','D','O','R',' ','D','E',' ','D','O','S','I','S',' ','D','E',' ','R','A','D','I','O','I','S','Ó','T','O','P','O','S',		// L_55T_FULL_TITLE (french)  50(1)
'C','A','P','R','A','C','-','t',' ',' ',' ',' ',' ',' ',' ',' ','R','A','D','I','O','I','S','O','T','O','P','E',' ','W','E','L','L',' ','C','O','U','N','T','E','R',		// L_CAPRAC_FULL_TITLE (english)  41(1), 118
'C','A','P','R','A','C','-','t',' ',' ',' ',' ',' ',' ',' ',' ','R','A','D','I','O','I','S','O','T','O','P','E',' ','W','E','L','L',' ','C','O','U','N','T','E','R',		// L_CAPRAC_FULL_TITLE (french)  41(1)
'C','A','P','T','U','S','-','7','0','0','t',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','R','A','D','I','O','I','S','O','T','O','P','E',' ','C','O','U','N','T','E','R',		// L_700T_FULL_TITLE (english)  41(1), 119
'C','A','P','T','U','S','-','7','0','0','t',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','R','A','D','I','O','I','S','O','T','O','P','E',' ','C','O','U','N','T','E','R',		// L_700T_FULL_TITLE (french)  41(1)
'F','a','c','t','o','r','y',		// L_FACTORY (english)  7(0), 120
'F','a','c','t','o','r','y',		// L_FACTORY (french)  7(0)
'S','e','t','t','i','n','g',' ','h','a','s',' ','b','e','e','n',' ','s','a','v','e','d','.',' ','P','l','e','a','s','e',' ','r','e','s','t','a','r','t','.',		// L_PLEASE_RESTART (english)  39(1), 121
'S','e','t','t','i','n','g',' ','h','a','s',' ','b','e','e','n',' ','s','a','v','e','d','.',' ','P','l','e','a','s','e',' ','r','e','s','t','a','r','t','.',		// L_PLEASE_RESTART (french)  39(1)
'W','i','p','e',' ','T','y','p','e',' ','D','e','f','a','u','l','t','s',		// L_WIPE_TYPE_DEFAULTS (english)  18(0), 122
'W','i','p','e',' ','T','y','p','e',' ','D','e','f','a','u','l','t','s',		// L_WIPE_TYPE_DEFAULTS (french)  18(0)
'C','u','r','r','e','n','t',' ','s','e','t','t','i','n','g','s',' ','h','a','v','e',' ','b','e','e','n',' ','o','v','e','r','w','r','i','t','t','e','n',' ','w','i','t','h',' ','%','s',' ','d','e','f','a','u','l','t',' ','v','a','l','u','e','s',		// L_WIPE_OVERWRITE_STRING (english)  61(2), 123
'C','u','r','r','e','n','t',' ','s','e','t','t','i','n','g','s',' ','h','a','v','e',' ','b','e','e','n',' ','o','v','e','r','w','r','i','t','t','e','n',' ','w','i','t','h',' ','%','s',' ','d','e','f','a','u','l','t',' ','v','a','l','u','e','s',		// L_WIPE_OVERWRITE_STRING (french)  61(2)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','L','i','n','e','a','t','o','r',		// L_SETUP_LINEARITY_LINEATOR (english)  24(0), 124
'C','o','n','f','i','g','u','r','a',' ','l','a',' ','l','i','n','e','a','l','i','d','a','d',' ','d','e',' ','L','i','n','e','a','t','o','r',		// L_SETUP_LINEARITY_LINEATOR (french)  35(1)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','C','a','l','i','c','h','e','c','k',		// L_SETUP_LINEARITY_CALICHECK (english)  25(0), 125
'C','o','n','f','i','g','u','r','a',' ','l','a',' ','l','i','n','e','a','l','i','d','a','d',' ','d','e',' ','C','a','l','i','c','h','e','c','k',		// L_SETUP_LINEARITY_CALICHECK (french)  36(1)
'T','u','b','e',		// L_TUBE (english)  4(0), 126
'T','u','b','o',		// L_TUBE (french)  4(0)
'T','u','b','e',':',		// L_TUBE2 (english)  5(0), 127
'T','u','b','o',':',		// L_TUBE2 (french)  5(0)
'L','i','n','e','a','t','o','r',' ','S','e','r','i','a','l',':',		// L_LINEATOR_SERIAL (english)  16(0), 128
'L','i','n','e','a','t','o','r',' ','S','/','N',':',		// L_LINEATOR_SERIAL (french)  13(0)
'C','a','l','i','c','h','e','c','k',' ','S','e','r','i','a','l',':',		// L_CALICHECK_SERIAL (english)  17(0), 129
'C','a','l','i','c','h','e','c','k',' ','S','/','N',':',		// L_CALICHECK_SERIAL (french)  14(0)
'P','r','i','n','t',		// L_PRINT (english)  5(0), 130
'I','m','p','r','i','m','e',		// L_PRINT (french)  7(0)
'D','e','l','e','t','e',' ','C','u','r','r','e','n','t',' ','S','e','t','t','i','n','g','s',		// L_DELETE_CURRENT_SETTINGS (english)  23(0), 131
'B','o','r','r','a','r',' ','C','o','n','f','i','g','.',' ','A','c','t','u','a','l',		// L_DELETE_CURRENT_SETTINGS (french)  21(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','L','i','n','e','a','t','o','r',' ','S','e','r','i','a','l',' ','#',		// L_PLEASE_ENTER_LINEATOR_SN (english)  30(1), 132
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','e','l',' ','#',' ','d','e',' ','S','e','r','i','e',' ','d','e',' ','L','i','n','e','a','t','o','r',		// L_PLEASE_ENTER_LINEATOR_SN (french)  45(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','C','a','l','i','c','h','e','c','k',' ','S','e','r','i','a','l',' ','#',		// L_PLEASE_ENTER_CALICHECK_SN (english)  31(1), 133
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','e','l',' ','#',' ','d','e',' ','S','e','r','i','e',' ','d','e','l',' ','C','a','l','i','c','h','e','c','k',		// L_PLEASE_ENTER_CALICHECK_SN (french)  47(1)
'N','U','C','L','I','D','E',		// L_PR_NUCLIDE (english)  7(0), 134
'N','U','C','L','E','I','D','O',		// L_PR_NUCLIDE (french)  8(0)
'C','h','a','m','b','e','r',' ',' ',' ','S','/','N',':',		// L_PR_CHAMBER_SN (english)  14(0), 135
'C','á','m','a','r','a',' ',' ',' ',' ','S','/','N',':',		// L_PR_CHAMBER_SN (french)  15(0)
'1',')','B','l','a','c','k',':',		// L_CAL_1 (english)  8(0), 136
'1',')','N','e','g','r','o',':',		// L_CAL_1 (french)  8(0)
'1',')','B','l','a','c','k',		// L_CAL1 (english)  7(0), 137
'1',')','N','e','g','r','o',		// L_CAL1 (french)  7(0)
'B','l','a','c','k',		// L_CAL__1 (english)  5(0), 138
'N','e','g','r','o',		// L_CAL__1 (french)  5(0)
'2',')','B','l','a','c','k',' ','+',' ','R','e','d',':',		// L_CAL_2 (english)  14(0), 139
'2',')','N','e','g','r','o',' ','+',' ','R','o','j','o',':',		// L_CAL_2 (french)  15(0)
'2',')','B','l','a','c','k','/','R','e','d',		// L_CAL2 (english)  11(0), 140
'2',')','N','e','g','r','o','/','R','o','j','o',		// L_CAL2 (french)  12(0)
'B','l','a','c','k',' ','+',' ','R','e','d',		// L_CAL__2 (english)  11(0), 141
'N','e','g','r','o',' ','+',' ','R','o','j','o',		// L_CAL__2 (french)  12(0)
'3',')','B','l','a','c','k',' ','+',' ','O','r','a','n','g','e',':',		// L_CAL_3 (english)  17(0), 142
'3',')','N','e','g','r','o',' ','+',' ','N','a','r','a','n','j','a',':',		// L_CAL_3 (french)  18(0)
'3',')','B','l','a','c','k','/','O','r','a','n','g','e',		// L_CAL3 (english)  14(0), 143
'3',')','N','e','g','r','o','/','N','a','r','a','n','j','a',		// L_CAL3 (french)  15(0)
'B','l','a','c','k',' ','+',' ','O','r','a','n','g','e',		// L_CAL__3 (english)  14(0), 144
'N','e','g','r','o',' ','+',' ','N','a','r','a','n','j','a',		// L_CAL__3 (french)  15(0)
'4',')','B','l','a','c','k',' ','+',' ','Y','e','l','l','o','w',':',		// L_CAL_4 (english)  17(0), 145
'4',')','N','e','g','r','o',' ','+',' ','A','m','a','r','i','l','l','o',':',		// L_CAL_4 (french)  19(0)
'4',')','B','l','a','c','k','/','Y','e','l','l','o','w',		// L_CAL4 (english)  14(0), 146
'4',')','N','e','g','r','o','/','A','m','a','r','i','l','l','o',		// L_CAL4 (french)  16(0)
'B','l','a','c','k',' ','+',' ','Y','e','l','l','o','w',		// L_CAL__4 (english)  14(0), 147
'N','e','g','r','o',' ','+',' ','A','m','a','r','i','l','l','o',		// L_CAL__4 (french)  16(0)
'5',')','B','l','a','c','k',' ','+',' ','G','r','e','e','n',':',		// L_CAL_5 (english)  16(0), 148
'5',')','N','e','g','r','o',' ','+',' ','V','e','r','d','e',':',		// L_CAL_5 (french)  16(0)
'5',')','B','l','a','c','k','/','G','r','e','e','n',		// L_CAL5 (english)  13(0), 149
'5',')','N','e','g','r','o','/','V','e','r','d','e',		// L_CAL5 (french)  13(0)
'B','l','a','c','k',' ','+',' ','G','r','e','e','n',		// L_CAL__5 (english)  13(0), 150
'N','e','g','r','o',' ','+',' ','V','e','r','d','e',		// L_CAL__5 (french)  13(0)
'6',')','B','l','a','c','k',' ','+',' ','B','l','u','e',':',		// L_CAL_6 (english)  15(0), 151
'6',')','N','e','g','r','o',' ','+',' ','A','z','u','l',':',		// L_CAL_6 (french)  15(0)
'6',')','B','l','a','c','k','/','B','l','u','e',		// L_CAL6 (english)  12(0), 152
'6',')','N','e','g','r','o','/','A','z','u','l',		// L_CAL6 (french)  12(0)
'B','l','a','c','k',' ','+',' ','B','l','u','e',		// L_CAL__6 (english)  12(0), 153
'N','e','g','r','o',' ','+',' ','A','z','u','l',		// L_CAL__6 (french)  12(0)
'7',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',':',		// L_CAL_7 (english)  17(0), 154
'7',')','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',':',		// L_CAL_7 (french)  19(0)
'7',')','B','l','a','c','k','/','P','u','r','p','l','e',		// L_CAL7 (english)  14(0), 155
'7',')','N','e','g','r','o','/','P','ú','r','p','u','r','a',		// L_CAL7 (french)  16(0)
'B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL__7 (english)  14(0), 156
'N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',		// L_CAL__7 (french)  16(0)
'8',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL_8 (english)  16(0), 157
'8',')','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',		// L_CAL_8 (french)  18(0)
'8',')','B','l','a','c','k','/','P','u','r','p','l','e','/','R','e','d',		// L_CAL8 (english)  18(0), 158
'8',')','N','e','g','r','o','/','P','ú','r','p','u','r','a','/','R','o','j','a',		// L_CAL8 (french)  21(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','R','e','d',		// L_CAL__8 (english)  16(0), 159
'N','e','g','r','o','/','P','ú','r','p','u','r','a','/','R','o','j','a',		// L_CAL__8 (french)  19(0)
'9',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL_9 (english)  16(0), 160
'9',')','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',		// L_CAL_9 (french)  18(0)
'9',')','B','l','k','/','P','u','r','p','l','e','/','O','r','a','n','g','e',		// L_CAL9 (english)  19(0), 161
'9',')','N','e','g','r','o','/','P','U','R','/','N','a','r','a','n','j','a',		// L_CAL9 (french)  19(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','O','r','a','n','g','e',		// L_CAL__9 (english)  19(0), 162
'N','e','g','r','o','/','P','ú','r','p','u','r','a','/','N','a','r','a','n','j','a',		// L_CAL__9 (french)  22(0)
'1','0',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL_10 (english)  17(0), 163
'1','0',')','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',		// L_CAL_10 (french)  19(0)
'1','0',')','B','l','k','/','P','u','r','p','l','/','Y','e','l','l','o','w',		// L_CAL10 (english)  19(0), 164
'1','0',')','N','e','g','r','o','/','P','U','R','/','A','m','a','r','i','l','l','o',		// L_CAL10 (french)  21(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','Y','e','l','l','o','w',		// L_CAL__10 (english)  19(0), 165
'N','e','g','r','o','/','P','ú','r','p','u','r','a','/','A','m','a','r','i','l','l','o',		// L_CAL__10 (french)  23(0)
'1','1',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL_11 (english)  17(0), 166
'1','1',')','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',		// L_CAL_11 (french)  19(0)
'1','1',')','B','l','k','/','P','u','r','p','l','e','/','G','r','e','e','n',		// L_CAL11 (english)  19(0), 167
'1','1',')','N','e','g','r','o','/','P','U','R','/','V','e','r','d','e',		// L_CAL11 (french)  18(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','G','r','e','e','n',		// L_CAL__11 (english)  18(0), 168
'N','e','g','r','o','/','P','ú','r','p','u','r','a','/','V','e','r','d','e',		// L_CAL__11 (french)  20(0)
'1','2',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL_12 (english)  17(0), 169
'1','2',')','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',		// L_CAL_12 (french)  19(0)
'1','2',')','B','l','k','/','P','u','r','p','l','e','/','B','l','u','e',		// L_CAL12 (english)  18(0), 170
'1','2',')','N','e','g','r','o','/','P','ú','r','p','u','r','a','/','A','z','u','l',		// L_CAL12 (french)  22(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','B','l','u','e',		// L_CAL__12 (english)  17(0), 171
'N','e','g','r','o','/','P','ú','r','p','u','r','a','/','A','z','u','l',		// L_CAL__12 (french)  19(0)
'+',' ','R','e','d',':',		// L_CAL_82 (english)  6(0), 172
'+',' ','R','o','j','a',':',		// L_CAL_82 (french)  7(0)
'+',' ','O','r','a','n','g','e',':',		// L_CAL_92 (english)  9(0), 173
'+',' ','N','a','r','a','n','j','a',':',		// L_CAL_92 (french)  10(0)
'+',' ','Y','e','l','l','o','w',':',		// L_CAL_102 (english)  9(0), 174
'+',' ','A','m','a','r','i','l','l','o',':',		// L_CAL_102 (french)  11(0)
'+',' ','G','r','e','e','n',':',		// L_CAL_11_2 (english)  8(0), 175
'+',' ','V','e','r','d','e',':',		// L_CAL_11_2 (french)  8(0)
'+',' ','B','l','u','e',':',		// L_CAL_12_2 (english)  7(0), 176
'+',' ','A','z','u','l',':',		// L_CAL_12_2 (french)  7(0)
'Y','e','s',		// L_YES_ENGLISH (english)  3(0), 177
'Y','e','s',		// L_YES_ENGLISH (french)  3(0)
'N','o',		// L_NO_ENGLISH (english)  2(0), 178
'N','o',		// L_NO_ENGLISH (french)  2(0)
'C','o','m','m','e','n','t',':',		// L_COMMENT (english)  8(0), 179
'C','o','m','e','n','t','a','r','i','o',':',		// L_COMMENT (french)  11(0)
'R','e','s','u','m','e',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',		// L_RESUME_AUTOLINEARITY_TEST (english)  25(0), 180
'R','e','a','n','u','d','a','r',' ','l','a',' ','P','r','u','e','b','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',		// L_RESUME_AUTOLINEARITY_TEST (french)  33(1)
'E','R','R','O','R',		// L_ERROR (english)  5(0), 181
'E','R','R','O','R',		// L_ERROR (french)  5(0)
'E','r','r','o','r',		// L_ERROR2 (english)  5(0), 182
'E','r','r','o','r',		// L_ERROR2 (french)  5(0)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','c','h','a','m','b','e','r',' ','w','i','t','h',' ','S','/','N',':',' ','%','s',		// L_UNABLE_TO_FIND_CHAMBER_WITH_SN (english)  35(1), 183
'N','o',' ','s','e',' ','p','u','e','d','e',' ','e','n','c','o','n','t','r','a','r',' ','l','a',' ','c','á','m','a','r','a',' ','c','o','n',' ','S','/','N',':',' ','%','s',		// L_UNABLE_TO_FIND_CHAMBER_WITH_SN (french)  44(1)
'A','b','o','r','t',' ','c','u','r','r','e','n','t',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t','?',		// L_ABORT_CURRENT_AUTOLINEARITY_TEST (english)  33(1), 184
'¿','A','b','o','r','t','a','r',' ','p','r','u','e','b','a',' ','a','c','t','u','a','l',' ','d','e',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d','?',		// L_ABORT_CURRENT_AUTOLINEARITY_TEST (french)  42(1)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','u','s','a','b','l','e',' ','N','u','c','l','i','d','e',':',' ','%','s',		// L_ABORT_UNABLE_TO_FIND_USABLE_NUCLIDE (english)  33(1), 185
'N','o',' ','s','e',' ','p','u','e','d','e',' ','e','n','c','o','n','t','r','a','r',' ','ú','t','i','l',' ','N','u','c','l','e','i','d','o',':',' ','%','s',		// L_ABORT_UNABLE_TO_FIND_USABLE_NUCLIDE (french)  40(1)
'C','o','m','p','l','e','t','e','d',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',' ','h','a','s',' ','n','o','t',' ','b','e','e','n',' ','s','a','v','e','d','.',		// L_COMPLETED_AUTOLINEARITY_NOT_SAVED (english)  48(1), 186
'P','r','u','e','b','a',' ','C','o','m','p','l','e','t','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',' ','n','o',' ','s','e',' ','h','a',' ','g','u','a','r','d','a','d','o','.',		// L_COMPLETED_AUTOLINEARITY_NOT_SAVED (french)  49(1)
'P','r','o','c','e','e','d',' ','t','o',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t','?',		// L_PROCEED_TO_AUTOLINEARITY (english)  30(1), 187
'¿','P','r','o','c','e','d','a',' ','a',' ','l','a',' ','p','r','u','e','b','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d','?',		// L_PROCEED_TO_AUTOLINEARITY (french)  37(1)
'P','l','e','a','s','e',' ','p','l','a','c','e',' ','N','u','c','l','i','d','e',':',' ','%','s',' ','i','n','t','o',' ','C','h','a','m','b','e','r',':',' ','%','d',		// L_PLEASE_PLACE_NUCLIDE (english)  41(1), 188
'F','a','v','o','r',' ','d','e',' ','C','o','l','o','c','a','r',' ','e','l',' ','N','u','c','l','e','i','d','o',':',' ','%','s',' ','e','n',' ','l','a',' ','C','á','m','a','r','a',':',' ','%','d',		// L_PLEASE_PLACE_NUCLIDE (french)  50(1)
'R','e','s','u','m','e',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t','?',		// L_RESUME_AUTOLINEARITY (english)  26(1), 189
'¿','R','e','a','n','u','d','a','r',' ','l','a',' ','P','r','u','e','b','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d','?',		// L_RESUME_AUTOLINEARITY (french)  36(1)
'P','a','u','s','e',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',		// L_PAUSE_AUTOLINEARITY_TEST (english)  24(0), 190
'P','a','u','s','a',' ','p','r','u','e','b','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',		// L_PAUSE_AUTOLINEARITY_TEST (french)  27(1)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','h','a','s',' ','b','e','e','n',' ','p','a','u','s','e','d','.',		// L_AUTOLINEARITY_HAS_BEEN_PAUSED (english)  30(1), 191
'A','u','t','o','L','i','n','e','a','l','i','d','a','d',' ','s','e',' ','h','a',' ','p','a','u','s','a','d','o','.',		// L_AUTOLINEARITY_HAS_BEEN_PAUSED (french)  29(1)
'C','o','m','p','l','e','t','e','d',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',' ','h','a','s',' ','n','o','t',' ','b','e','e','n',' ','s','a','v','e','d','.',		// L_COMPLETED_AUTOLINEARITY_HAS_NOT_BEEN_SAVED (english)  48(1), 192
'P','r','u','e','b','a',' ','C','o','m','p','l','e','t','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',' ','n','o',' ','s','e',' ','h','a',' ','g','u','a','r','d','a','d','o','.',		// L_COMPLETED_AUTOLINEARITY_HAS_NOT_BEEN_SAVED (french)  49(1)
'T','o',' ','a','v','o','i','d',' ','m','i','s','s','i','n','g',' ','m','e','a','s','u','r','e','m','e','n','t','s',',',' ',		// L_TO_AVOID_MISSING_MEASUREMENTS (english)  31(1), 193
'P','a','r','a',' ','e','v','i','t','a','r',' ','m','e','d','i','d','a','s',' ','p','e','r','d','i','d','a','s',',',' ',		// L_TO_AVOID_MISSING_MEASUREMENTS (french)  30(1)
'P','l','e','a','s','e',' ','r','e','t','u','r','n',' ','t','o',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','a','s',' ','s','o','o','n',' ','a','s',' ','p','o','s','s','i','b','l','e','.',		// L_PLEASE_RETURN_TO_AUTOLINEARITY (english)  51(1), 194
'R','e','g','r','e','s','e',' ','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',' ','l','o',' ','m','a','s',' ','p','r','o','n','t','o',' ','p','o','s','i','b','l','e','.',		// L_PLEASE_RETURN_TO_AUTOLINEARITY (french)  47(1)
'P','r','o','c','e','e','d',' ','t','o',' ','H','o','m','e',' ','S','c','r','e','e','n','?',		// L_PROCEED_TO_HOME_SCREEN (english)  23(0), 195
'¿','P','r','o','c','e','d','a',' ','a',' ','l','a',' ','p','a','n','t','a','l','l','a',' ','d','e',' ','i','n','i','c','i','o','?',		// L_PROCEED_TO_HOME_SCREEN (french)  34(1)
'P','r','o','c','e','e','d',' ','t','o',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','M','e','n','u','?',		// L_PROCEED_TO_AUTOLINEARITY_MENU (english)  30(1), 196
'¿','P','r','o','c','e','d','a',' ','a','l',' ','m','e','n','ú',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d','?',		// L_PROCEED_TO_AUTOLINEARITY_MENU (french)  34(1)
'A','b','o','r','t',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',		// L_ABORT_AUTOLINEARITY_TEST (english)  24(0), 197
'A','b','o','r','t','a','r',' ','P','r','u','e','b','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',		// L_ABORT_AUTOLINEARITY_TEST (french)  29(1)
'A','b','o','r','t',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t','?',		// L_ABORT_AUTOLINEARITY_TEST2 (english)  25(0), 198
'¿','A','b','o','r','t','a','r',' ','P','r','u','e','b','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d','?',		// L_ABORT_AUTOLINEARITY_TEST2 (french)  32(1)
'C','u','r','r','e','n','t',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',' ','h','a','s',' ','f','i','n','i','s','h','e','d','.',		// L_CURRENT_AUTOLINEARITY_TEST_HAS_FINISHED (english)  40(1), 199
'P','r','u','e','b','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',' ','a','c','t','u','a','l',' ','h','a',' ','t','e','r','m','i','n','a','d','o','.',		// L_CURRENT_AUTOLINEARITY_TEST_HAS_FINISHED (french)  42(1)
'E','r','a','s','e',' ','C','u','r','r','e','n','t',' ','T','e','s','t','?',		// L_ERASE_CURRENT_TEST (english)  19(0), 200
'¿','B','o','r','r','a','r',' ','P','r','u','e','b','a',' ','A','c','t','u','a','l','?',		// L_ERASE_CURRENT_TEST (french)  23(0)
'S','a','v','e',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',		// L_SAVE_AUTOLINEARITY_TEST (english)  23(0), 201
'G','u','a','r','d','a','r',' ','P','r','u','e','b','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',		// L_SAVE_AUTOLINEARITY_TEST (french)  29(1)
'S','a','v','e',' ','C','u','r','r','e','n','t',' ','T','e','s','t','?',		// L_SAVE_CURRENT_TEST (english)  18(0), 202
'¿','G','u','a','r','d','a','r',' ','P','r','u','e','b','a',' ','A','c','t','u','a','l','?',		// L_SAVE_CURRENT_TEST (french)  24(0)
'C','u','r','r','e','n','t',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',' ','i','s',' ','p','a','r','t','i','a','l','l','y',' ','c','o','m','p','l','e','t','e','.',		// L_CURRENT_AUTOLINEARITY_IS_PARTIALLY_COMPLETE (english)  49(1), 203
'P','r','u','e','b','a',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',' ','a','c','t','u','a','l',' ','e','s',' ','p','a','r','c','i','a','l','m','e','n','t','e',' ','c','o','m','p','l','e','t','a','.',		// L_CURRENT_AUTOLINEARITY_IS_PARTIALLY_COMPLETE (french)  54(2)
'E','n','d',' ','a','n','d',' ','S','a','v','e',' ','C','u','r','r','e','n','t',' ','T','e','s','t','?',		// L_END_AND_SAVE_TEST (english)  26(1), 204
'¿','F','i','n','a','l',' ','y',' ','G','u','a','r','d','a','r',' ','P','r','u','e','b','a',' ','A','c','t','u','a','l','?',		// L_END_AND_SAVE_TEST (french)  32(1)
'M','a','x','i','m','u','m',' ','i','s',' ','%','.','4','f',		// L_MAXIMUM_IS (english)  15(0), 205
'M','á','x','i','m','a',' ','e','s',' ','%','.','4','f',		// L_MAXIMUM_IS (french)  15(0)
'I','n','p','u','t',' ','E','r','r','o','r',		// L_INPUT_ERROR (english)  11(0), 206
'E','r','r','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','c','i','ó','n',		// L_INPUT_ERROR (french)  22(0)
'M','i','n','i','m','u','m',' ','i','s',' ','%','.','4','f',		// L_MINIMUM_IS (english)  15(0), 207
'M','í','n','i','m','o',' ','e','s',' ','%','.','4','f',		// L_MINIMUM_IS (french)  15(0)
'C','a','l',' ','N','u','m','b','e','r',' ','E','r','r','o','r',		// L_CAL_NUMBER_ERROR (english)  16(0), 208
'E','r','r','o','r',' ','d','e','l',' ','F','a','c','t','o','r',' ','C','a','l','.',		// L_CAL_NUMBER_ERROR (french)  21(0)
'I','n','v','a','l','i','d',' ','C','a','l',' ','N','u','m','b','e','r',		// L_INVALID_CAL_NUMBER (english)  18(0), 209
'I','n','v','á','l','i','d','o',' ','N','ú','m','e','r','o',' ','d','e',' ','C','a','l','i','b','r','a','c','i','ó','n',		// L_INVALID_CAL_NUMBER (french)  33(1)
'U','s','e','r',' ','C','a','l',' ','F','u','l','l',		// L_USER_CAL_FULL (english)  13(0), 210
'C','a','l','i','b','r','a','c','i','ó','n',' ','d','e',' ','U','s','u','a','r','i','o',' ','C','o','m','p','l','e','t','a',		// L_USER_CAL_FULL (french)  32(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','C','u','r','r','e','n','t',' ','T','i','m','e',':',		// L_PLEASE_ENTER_CURRENT_TIME (english)  26(1), 211
'F','a','v','o','r',' ','d','e',' ','i','n','t','r','o','d','u','c','i','r',' ','h','o','r','a',' ','a','c','t','u','a','l',':',		// L_PLEASE_ENTER_CURRENT_TIME (french)  32(1)
'S','e','r','i','a','l',' ','N','u','m','b','e','r',' ','E','r','r','o','r',		// L_SERIAL_NUMBER_ERROR (english)  19(0), 212
'E','r','r','o','r',' ','d','e','l',' ','N','ú','m','e','r','o',' ','d','e',' ','S','e','r','i','e',		// L_SERIAL_NUMBER_ERROR (french)  26(1)
'S','e','r','i','a','l',' ','n','u','m','b','e','r',' ','m','u','s','t',' ','h','a','v','e',' ','s','i','x',' ','d','i','g','i','t','s',		// L_SERIAL_NUMBER_MUST_HAVE_SIX_DIGITS (english)  34(1), 213
'N','ú','m','e','r','o',' ','d','e',' ','s','e','r','i','e',' ','d','e','b','e',' ','s','e','r',' ','d','e',' ','6',' ','d','í','g','i','t','o','s',		// L_SERIAL_NUMBER_MUST_HAVE_SIX_DIGITS (french)  39(1)
'D','o','s','e',' ','T','i','m','e',' ','E','r','r','o','r',		// L_DOSE_TIME_ERROR (english)  15(0), 214
'E','r','r','o','r',' ','d','e','l',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','D','o','s','i','s',		// L_DOSE_TIME_ERROR (french)  28(1)
'I','n','v','a','l','i','d',' ','D','a','t','e',		// L_INVALID_DATE (english)  12(0), 215
'F','e','c','h','a',' ','I','n','v','a','l','i','d','a',		// L_INVALID_DATE (french)  14(0)
'I','n','v','a','l','i','d',' ','T','i','m','e',		// L_INVALID_TIME (english)  12(0), 216
'T','i','e','m','p','o',' ','I','n','v','a','l','i','d','o',		// L_INVALID_TIME (french)  15(0)
'M','a','x',' ','H','o','u','r',' ','i','s',' ','2','3',		// L_MAX_HOUR_IS_23 (english)  14(0), 217
'H','o','r','a',' ','M','a','x',' ','e','s',' ','2','3',		// L_MAX_HOUR_IS_23 (french)  14(0)
'M','a','x',' ','M','i','n','u','t','e',' ','i','s',' ','5','9',		// L_MAX_MINUTE_IS_59 (english)  16(0), 218
'M','i','n','u','t','o',' ','M','a','x',' ','e','s',' ','5','9',		// L_MAX_MINUTE_IS_59 (french)  16(0)
'M','i','n',' ','D','a','y',' ','i','s',' ','1',		// L_MIN_DAY_IS_1 (english)  12(0), 219
'D','í','a',' ','M','i','n',' ','e','s',' ','1',		// L_MIN_DAY_IS_1 (french)  13(0)
'M','a','x',' ','D','a','y',' ','i','s',' ','3','1',		// L_MAX_DAY_IS_31 (english)  13(0), 220
'D','í','a',' ','M','a','x',' ','e','s',' ','3','1',		// L_MAX_DAY_IS_31 (french)  14(0)
'M','i','n',' ','M','o','n','t','h',' ','i','s',' ','1',		// L_MIN_MONTH_IS_1 (english)  14(0), 221
'M','e','s',' ','M','i','n',' ','e','s',' ','1',		// L_MIN_MONTH_IS_1 (french)  12(0)
'M','a','x',' ','M','o','n','t','h',' ','i','s',' ','1','2',		// L_MAX_MONTH_IS_12 (english)  15(0), 222
'M','e','s',' ','M','a','x',' ','e','s',' ','1','2',		// L_MAX_MONTH_IS_12 (french)  13(0)
'M','a','x',' ','Y','e','a','r',' ','i','s',' ','2','0','3','0',		// L_MAX_YEAR_IS_2030 (english)  16(0), 223
'A','ñ','o',' ','M','a','x',' ','e','s',' ','2','0','3','0',		// L_MAX_YEAR_IS_2030 (french)  16(0)
'I','n','v','a','l','i','d',' ','Y','e','a','r',		// L_INVALID_YEAR (english)  12(0), 224
'A','ñ','o',' ','I','n','v','a','l','i','d','o',		// L_INVALID_YEAR (french)  13(0)
'T','u','b','e',' ','#','1',':',' ','(','1',')',		// L_TUBE_1_LINEATOR (english)  12(0), 225
'T','u','b','o',' ','#','1',':',' ','(','1',')',		// L_TUBE_1_LINEATOR (french)  12(0)
'1',')',' ','T','u','b','e',' ','1',		// L_TUBE_1_LINEATOR2 (english)  9(0), 226
'1',')',' ','T','u','b','o',' ','1',		// L_TUBE_1_LINEATOR2 (french)  9(0)
'T','u','b','e',' ','#','2',':',' ','(','1',' ','+',' ','2',')',		// L_TUBE_2_LINEATOR (english)  16(0), 227
'T','u','b','o',' ','#','2',':',' ','(','1',' ','+',' ','2',')',		// L_TUBE_2_LINEATOR (french)  16(0)
'2',')',' ','T','u','b','e',' ','1','+','2',		// L_TUBE_2_LINEATOR2 (english)  11(0), 228
'2',')',' ','T','u','b','o',' ','1','+','2',		// L_TUBE_2_LINEATOR2 (french)  11(0)
'T','u','b','e',' ','#','3',':',' ','(','1',' ','+',' ','3',')',		// L_TUBE_3_LINEATOR (english)  16(0), 229
'T','u','b','o',' ','#','3',':',' ','(','1',' ','+',' ','3',')',		// L_TUBE_3_LINEATOR (french)  16(0)
'3',')',' ','T','u','b','e',' ','1','+','3',		// L_TUBE_3_LINEATOR2 (english)  11(0), 230
'3',')',' ','T','u','b','o',' ','1','+','3',		// L_TUBE_3_LINEATOR2 (french)  11(0)
'T','u','b','e',' ','#','4',':',' ','(','1',' ','+',' ','2',' ','+',' ','3',')',		// L_TUBE_4_LINEATOR (english)  20(0), 231
'T','u','b','o',' ','#','4',':',' ','(','1',' ','+',' ','2',' ','+',' ','3',')',		// L_TUBE_4_LINEATOR (french)  20(0)
'4',')',' ','T','u','b','e',' ','1','+','2',',','3',		// L_TUBE_4_LINEATOR2 (english)  13(0), 232
'4',')',' ','T','u','b','o',' ','1','+','2',',','3',		// L_TUBE_4_LINEATOR2 (french)  13(0)
'T','u','b','e',' ','#','5',':',' ','(','1',' ','+',' ','4',')',		// L_TUBE_5_LINEATOR (english)  16(0), 233
'T','u','b','o',' ','#','5',':',' ','(','1',' ','+',' ','4',')',		// L_TUBE_5_LINEATOR (french)  16(0)
'5',')',' ','T','u','b','e',' ','1','+','4',		// L_TUBE_5_LINEATOR2 (english)  11(0), 234
'5',')',' ','T','u','b','o',' ','1','+','4',		// L_TUBE_5_LINEATOR2 (french)  11(0)
'T','u','b','e',' ','#','6',':',' ','(','1',' ','+',' ','2',' ','+',' ','4',')',		// L_TUBE_6_LINEATOR (english)  20(0), 235
'T','u','b','o',' ','#','6',':',' ','(','1',' ','+',' ','2',' ','+',' ','4',')',		// L_TUBE_6_LINEATOR (french)  20(0)
'6',')',' ','T','u','b','e',' ','1','+','2',',','4',		// L_TUBE_6_LINEATOR2 (english)  13(0), 236
'6',')',' ','T','u','b','o',' ','1','+','2',',','4',		// L_TUBE_6_LINEATOR2 (french)  13(0)
'T','u','b','e',' ','#','7',':',' ','(','1',' ','+',' ','3',' ','+',' ','4',')',		// L_TUBE_7_LINEATOR (english)  20(0), 237
'T','u','b','o',' ','#','7',':',' ','(','1',' ','+',' ','3',' ','+',' ','4',')',		// L_TUBE_7_LINEATOR (french)  20(0)
'7',')',' ','T','u','b','e',' ','1','+','3',',','4',		// L_TUBE_7_LINEATOR2 (english)  13(0), 238
'7',')',' ','T','u','b','o',' ','1','+','3',',','4',		// L_TUBE_7_LINEATOR2 (french)  13(0)
'T','u','b','e',' ','#','8',':',' ','(','1',' ','+',' ','2',' ','+',' ','3',' ','+',' ','4',')',		// L_TUBE_8_LINEATOR (english)  24(0), 239
'T','u','b','o',' ','#','8',':',' ','(','1',' ','+',' ','2',' ','+',' ','3',' ','+',' ','4',')',		// L_TUBE_8_LINEATOR (french)  24(0)
'8',')',' ','T','u','b','e',' ','1','+','2',',','3',',','4',		// L_TUBE_8_LINEATOR2 (english)  15(0), 240
'8',')',' ','T','u','b','o',' ','1','+','2',',','3',',','4',		// L_TUBE_8_LINEATOR2 (french)  15(0)
'T','u','b','e',' ','1',':',' ','B','l','a','c','k',		// L_TUBE_1_CALICHECK (english)  13(0), 241
'T','u','b','o',' ','1',':',' ','N','e','g','r','o',		// L_TUBE_1_CALICHECK (french)  13(0)
'T','u','b','e',' ','2',':',' ','B','l','a','c','k',' ','+',' ','R','e','d',		// L_TUBE_2_CALICHECK (english)  19(0), 242
'T','u','b','o',' ','2',':',' ','N','e','g','r','o',' ','+',' ','R','o','j','o',		// L_TUBE_2_CALICHECK (french)  20(0)
'T','u','b','e',' ','3',':',' ','B','l','a','c','k',' ','+',' ','O','r','a','n','g','e',		// L_TUBE_3_CALICHECK (english)  22(0), 243
'T','u','b','o',' ','3',':',' ','N','e','g','r','o',' ','+',' ','N','a','r','a','n','j','a',		// L_TUBE_3_CALICHECK (french)  23(0)
'T','u','b','e',' ','4',':',' ','B','l','a','c','k',' ','+',' ','Y','e','l','l','o','w',		// L_TUBE_4_CALICHECK (english)  22(0), 244
'T','u','b','o',' ','4',':',' ','N','e','g','r','o',' ','+',' ','A','m','a','r','i','l','l','o',		// L_TUBE_4_CALICHECK (french)  24(0)
'T','u','b','e',' ','5',':',' ','B','l','a','c','k',' ','+',' ','G','r','e','e','n',		// L_TUBE_5_CALICHECK (english)  21(0), 245
'T','u','b','o',' ','5',':',' ','N','e','g','r','o',' ','+',' ','V','e','r','d','e',		// L_TUBE_5_CALICHECK (french)  21(0)
'T','u','b','e',' ','6',':',' ','B','l','a','c','k',' ','+',' ','B','l','u','e',		// L_TUBE_6_CALICHECK (english)  20(0), 246
'T','u','b','o',' ','6',':',' ','N','e','g','r','o',' ','+',' ','A','z','u','l',		// L_TUBE_6_CALICHECK (french)  20(0)
'T','u','b','e',' ','7',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_TUBE_7_CALICHECK (english)  22(0), 247
'T','u','b','o',' ','7',':',' ','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',		// L_TUBE_7_CALICHECK (french)  24(0)
'T','u','b','e',' ','8',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',' ','+',' ','R','e','d',		// L_TUBE_8_CALICHECK (english)  28(1), 248
'T','u','b','o',' ','8',':',' ','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',' ','+',' ','R','o','j','o',		// L_TUBE_8_CALICHECK (french)  31(1)
'T','u','b','e',' ','9',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',' ','+',' ','O','r','a','n','g','e',		// L_TUBE_9_CALICHECK (english)  31(1), 249
'T','u','b','o',' ','9',':',' ','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',' ','+',' ','N','a','r','a','n','j','a',		// L_TUBE_9_CALICHECK (french)  34(1)
'T','u','b','e',' ','1','0',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',' ','+',' ','Y','e','l','l','o','w',		// L_TUBE_10_CALICHECK (english)  32(1), 250
'T','u','b','o',' ','1','0',':',' ','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',' ','+',' ','A','m','a','r','i','l','l','o',		// L_TUBE_10_CALICHECK (french)  36(1)
'T','u','b','e',' ','1','1',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',' ','+',' ','G','r','e','e','n',		// L_TUBE_11_CALICHECK (english)  31(1), 251
'T','u','b','o',' ','1','1',':',' ','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',' ','+',' ','V','e','r','d','e',		// L_TUBE_11_CALICHECK (french)  33(1)
'T','u','b','e',' ','1','2',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',' ','+',' ','B','l','u','e',		// L_TUBE_12_CALICHECK (english)  30(1), 252
'T','u','b','o',' ','1','2',':',' ','N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',' ','+',' ','A','z','u','l',		// L_TUBE_12_CALICHECK (french)  32(1)
'B','l','a','c','k',		// L_PR_TUBE_1_CALICHECK (english)  5(0), 253
'N','e','g','r','o',		// L_PR_TUBE_1_CALICHECK (french)  5(0)
'B','l','a','c','k',' ','+',' ','R','e','d',		// L_PR_TUBE_2_CALICHECK (english)  11(0), 254
'N','e','g','r','o',' ','+',' ','R','o','j','o',		// L_PR_TUBE_2_CALICHECK (french)  12(0)
'B','l','a','c','k',' ','+',' ','O','r','a','n','g','e',		// L_PR_TUBE_3_CALICHECK (english)  14(0), 255
'N','e','g','r','o',' ','+',' ','N','a','r','a','n','j','a',		// L_PR_TUBE_3_CALICHECK (french)  15(0)
'B','l','a','c','k',' ','+',' ','Y','e','l','l','o','w',		// L_PR_TUBE_4_CALICHECK (english)  14(0), 256
'N','e','g','r','o',' ','+',' ','A','m','a','r','i','l','l','o',		// L_PR_TUBE_4_CALICHECK (french)  16(0)
'B','l','a','c','k',' ','+',' ','G','r','e','e','n',		// L_PR_TUBE_5_CALICHECK (english)  13(0), 257
'N','e','g','r','o',' ','+',' ','V','e','r','d','e',		// L_PR_TUBE_5_CALICHECK (french)  13(0)
'B','l','a','c','k',' ','+',' ','B','l','u','e',		// L_PR_TUBE_6_CALICHECK (english)  12(0), 258
'N','e','g','r','o',' ','+',' ','A','z','u','l',		// L_PR_TUBE_6_CALICHECK (french)  12(0)
'B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_PR_TUBE_7_CALICHECK (english)  14(0), 259
'N','e','g','r','o',' ','+',' ','P','ú','r','p','u','r','a',		// L_PR_TUBE_7_CALICHECK (french)  16(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','R','e','d',		// L_PR_TUBE_8_CALICHECK (english)  16(0), 260
'N','e','g','r','o','/','P','ú','r','p','u','r','a','/','R','o','j','o',		// L_PR_TUBE_8_CALICHECK (french)  19(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','O','r','a','n','g','e',		// L_PR_TUBE_9_CALICHECK (english)  19(0), 261
'N','e','g','r','o','/','P','ú','r','p','u','r','a','/','N','a','r','a','n','j','a',		// L_PR_TUBE_9_CALICHECK (french)  22(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','Y','e','l','l','o','w',		// L_PR_TUBE_10_CALICHECK (english)  19(0), 262
'N','e','g','r','o','/','P','ú','r','p','u','r','a','/','A','m','a','r','i','l','l','o',		// L_PR_TUBE_10_CALICHECK (french)  23(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','G','r','e','e','n',		// L_PR_TUBE_11_CALICHECK (english)  18(0), 263
'N','e','g','r','o','/','P','ú','r','p','u','r','a','/','V','e','r','d','e',		// L_PR_TUBE_11_CALICHECK (french)  20(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','B','l','u','e',		// L_PR_TUBE_12_CALICHECK (english)  17(0), 264
'N','e','g','r','o','/','P','ú','r','p','u','r','a','/','A','z','u','l',		// L_PR_TUBE_12_CALICHECK (french)  19(0)
'M','e','a','s','u','r','e',' ','t','e','s','t',' ','s','o','u','r','c','e',',',' ',		// L_MEASURE_TEST_SOURCE (english)  21(0), 265
'M','e','d','i','r',' ','p','r','u','e','b','a',' ','d','e',' ','o','r','i','g','e','n',',',' ',		// L_MEASURE_TEST_SOURCE (french)  24(0)
',',' ','S','/','N',':',' ',		// L_MEASURE_TEST_SOURCE_SN (english)  7(0), 266
',',' ','S','/','N',':',' ',		// L_MEASURE_TEST_SOURCE_SN (french)  7(0)
'O','V','E','R',' ','R','A','N','G','E',		// L_OVERRANGE (english)  10(0), 267
'M','U','Y',' ','A','L','T','O',		// L_OVERRANGE (french)  8(0)
'O','v','e','r',' ','R','a','n','g','e',		// L_OVERRANGE2 (english)  10(0), 268
'M','u','y',' ','A','l','t','o',		// L_OVERRANGE2 (french)  8(0)
'O','V','E','R',' ','R','A','N','G','E','\n','E','R','R','O','R',		// L_OVERRANGE_ERROR (english)  16(0), 269
'E','R','R','O','R','\n','M','U','Y',' ','A','L','T','O',		// L_OVERRANGE_ERROR (french)  14(0)
'O','v','e','r',' ','R','a','n','g','e',' ','E','r','r','o','r',		// L_OVERRANGE_ERROR2 (english)  16(0), 270
'E','r','r','o','r',' ','M','u','y',' ','A','l','t','o',		// L_OVERRANGE_ERROR2 (french)  14(0)
'C','a','l','c',':',' ','%','s',',',' ','D','e','v',':',' ','E','R','R','O','R',		// L_CALC_DEV_ERROR (english)  20(0), 271
'C','a','l','c',':',' ','%','s',',',' ','D','e','s','v',':',' ','E','R','R','O','R',		// L_CALC_DEV_ERROR (french)  21(0)
'C','a','l','c',':',' ','%','s',',',' ','D','e','v',':',' ','%','.','1','f','%','%',		// L_CALC_DEV (english)  21(0), 272
'C','a','l','c',':',' ','%','s',',',' ','D','e','s','v',':',' ','%','.','1','f','%','%',		// L_CALC_DEV (french)  22(0)
'C','l','e','a','r',' ','S','e','l','e','c','t','e','d','\n','N','u','c','l','i','d','e',		// L_CLEAR_SELECTED_NUCLIDE (english)  22(0), 273
'B','o','r','r','a','r',' ','N','u','c','l','e','i','d','o','\n','S','e','l','e','c','c','i','o','n','a','d','o',		// L_CLEAR_SELECTED_NUCLIDE (french)  28(1)
'P','a','g','e',' ','%','d',' ','o','f',' ','%','d',		// L_PAGE_OF (english)  13(0), 274
'P','á','g','i','n','a',' ','%','d',' ','d','e',' ','%','d',		// L_PAGE_OF (french)  16(0)
'U','S','B',' ','P','C',' ','D','r','i','v','e','r',':',		// L_USB_PC_DRIVER (english)  14(0), 275
'C','o','n','t','r','o','l','a','d','o','r',' ','U','S','B',' ','P','C',':',		// L_USB_PC_DRIVER (french)  19(0)
'S','c','r','e','e','n',' ','C','a','l','i','b',		// L_SCREEN_CALIB (english)  12(0), 276
'P','a','n','t','a','l','l','a',' ','d','e',' ','C','a','l','i','b',		// L_SCREEN_CALIB (french)  17(0)
'D','o','s','e',' ','D','e','c','a','y',' ','E','n','t','r','y',':',		// L_DOSE_DECAY_ENTRY (english)  17(0), 277
'I','n','t','r','o',' ','D','o','s','i','s',' ','D','e','c','a','í','d','a',':',		// L_DOSE_DECAY_ENTRY (french)  21(0)
'P','l','e','a','s','e',' ','S','e','l','e','c','t',' ','N','u','c','l','i','d','e',		// L_PLEASE_SELECT_NUCLIDE (english)  21(0), 278
'F','a','v','o','r',' ','d','e',' ','S','e','l','e','c','c','i','o','n','a','r',' ','e','l',' ','N','u','c','l','e','i','d','o',		// L_PLEASE_SELECT_NUCLIDE (french)  32(1)
'E','n','t','e','r',' ','M','o','/','T','c',' ','L','i','m','i','t',':',		// L_ENTER_MOTC_LIMIT (english)  18(0), 279
'I','n','t','r','o','d','u','z','c','a',' ','M','o','/','T','c',' ','L','í','m','i','t','e',':',		// L_ENTER_MOTC_LIMIT (french)  25(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','#',' ','o','f',' ','M','e','a','s','u','r','e','m','e','n','t','s',':',		// L_ENTER_NUM_MEASUREMENTS (english)  31(1), 280
'I','n','t','r','o','d','u','z','c','a',' ','#',' ','d','e',' ','M','e','d','i','c','i','o','n','e','s',':',		// L_ENTER_NUM_MEASUREMENTS (french)  27(1)
'E','n','t','e','r',' ','T','e','s','t',' ','2',' ','T','i','m','e',':',		// L_ENTER_TEST_2_TIME (english)  18(0), 281
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','2',':',		// L_ENTER_TEST_2_TIME (french)  33(1)
'E','n','t','e','r',' ','T','e','s','t',' ','3',' ','T','i','m','e',':',		// L_ENTER_TEST_3_TIME (english)  18(0), 282
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','3',':',		// L_ENTER_TEST_3_TIME (french)  33(1)
'E','n','t','e','r',' ','T','e','s','t',' ','4',' ','T','i','m','e',':',		// L_ENTER_TEST_4_TIME (english)  18(0), 283
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','4',':',		// L_ENTER_TEST_4_TIME (french)  33(1)
'E','n','t','e','r',' ','T','e','s','t',' ','5',' ','T','i','m','e',':',		// L_ENTER_TEST_5_TIME (english)  18(0), 284
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','5',':',		// L_ENTER_TEST_5_TIME (french)  33(1)
'E','n','t','e','r',' ','T','e','s','t',' ','6',' ','T','i','m','e',':',		// L_ENTER_TEST_6_TIME (english)  18(0), 285
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','6',':',		// L_ENTER_TEST_6_TIME (french)  33(1)
'E','n','t','e','r',' ','T','e','s','t',' ','7',' ','T','i','m','e',':',		// L_ENTER_TEST_7_TIME (english)  18(0), 286
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','7',':',		// L_ENTER_TEST_7_TIME (french)  33(1)
'E','n','t','e','r',' ','T','e','s','t',' ','8',' ','T','i','m','e',':',		// L_ENTER_TEST_8_TIME (english)  18(0), 287
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','8',':',		// L_ENTER_TEST_8_TIME (french)  33(1)
'E','n','t','e','r',' ','T','e','s','t',' ','9',' ','T','i','m','e',':',		// L_ENTER_TEST_9_TIME (english)  18(0), 288
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','9',':',		// L_ENTER_TEST_9_TIME (french)  33(1)
'E','n','t','e','r',' ','T','e','s','t',' ','1','0',' ','T','i','m','e',':',		// L_ENTER_TEST_10_TIME (english)  19(0), 289
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','1','0',':',		// L_ENTER_TEST_10_TIME (french)  34(1)
'E','n','t','e','r',' ','T','e','s','t',' ','1','1',' ','T','i','m','e',':',		// L_ENTER_TEST_11_TIME (english)  19(0), 290
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','1','1',':',		// L_ENTER_TEST_11_TIME (french)  34(1)
'E','n','t','e','r',' ','T','e','s','t',' ','1','2',' ','T','i','m','e',':',		// L_ENTER_TEST_12_TIME (english)  19(0), 291
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','d','e',' ','l','a',' ','P','r','u','e','b','a',' ','1','2',':',		// L_ENTER_TEST_12_TIME (french)  34(1)
' ','S','e','c',		// L_SEC (english)  4(0), 292
' ','S','e','g',		// L_SEC (french)  4(0)
' ','s','e','c',		// L_SEC2 (english)  4(0), 293
' ','s','e','g',		// L_SEC2 (french)  4(0)
's','e','c',		// L_SEC3 (english)  3(0), 294
's','e','g',		// L_SEC3 (french)  3(0)
' ','M','i','n',		// L_MIN (english)  4(0), 295
' ','M','i','n',		// L_MIN (french)  4(0)
' ','m','i','n',		// L_MIN2 (english)  4(0), 296
' ','m','i','n',		// L_MIN2 (french)  4(0)
'm','i','n',		// L_MIN3 (english)  3(0), 297
'm','i','n',		// L_MIN3 (french)  3(0)
' ','H','r',		// L_HR (english)  3(0), 298
' ','H',		// L_HR (french)  2(0)
' ','h','r',		// L_HR2 (english)  3(0), 299
' ','h',		// L_HR2 (french)  2(0)
'h','r',		// L_HR3 (english)  2(0), 300
'h',		// L_HR3 (french)  1(0)
' ','H','r',		// L_HR4 (english)  3(0), 301
' ','H','o','r','a',		// L_HR4 (french)  5(0)
' ','D','a','y',		// L_DAY (english)  4(0), 302
' ','D','í','a',		// L_DAY (french)  5(0)
' ','d','a','y',		// L_DAY2 (english)  4(0), 303
' ','d','í','a',		// L_DAY2 (french)  5(0)
'd','a','y',		// L_DAY3 (english)  3(0), 304
'd','í','a',		// L_DAY3 (french)  4(0)
' ','Y','r',		// L_YR (english)  3(0), 305
' ','A','ñ','o',		// L_YR (french)  5(0)
' ','y','r',		// L_YR2 (english)  3(0), 306
' ','a','ñ','o',		// L_YR2 (french)  5(0)
'y','r',		// L_YR3 (english)  2(0), 307
'a','ñ','o',		// L_YR3 (french)  4(0)
' ','M','y','r',		// L_MYR (english)  4(0), 308
'M','a','ñ','o',		// L_MYR (french)  5(0)
'S','e','t','u','p',' ','N','u','c','l','i','d','e',' ','E','r','r','o','r',		// L_SETUP_NUCLIDE_ERROR (english)  19(0), 309
'E','r','r','o','r',' ','C','o','n','f','i','g','.',' ','d','e','l',' ','N','u','c','l','e','i','d','o',		// L_SETUP_NUCLIDE_ERROR (french)  26(1)
'P','l','e','a','s','e',' ','c','o','m','p','l','e','t','e',' ','p','a','r','t','i','a','l',' ','N','u','c','l','i','d','e',' ','e','n','t','r','y','.','\n','R','o','w','(','s',')',':',' ',		// L_PLEASE_COMPLETE_PARTIAL_NUCLIDE_ENTRY (english)  47(1), 310
'P','o','r',' ','f','a','v','o','r',',',' ','c','o','m','p','l','e','t','e',' ','l','a',' ','i','n','t','r','o','d','u','c','i','ó','n',' ','p','a','r','c','i','a','l',' ','d','e','l',' ','n','u','c','l','e','i','d','o','.','\n','F','i','l','a','(','s',')',':',' ',		// L_PLEASE_COMPLETE_PARTIAL_NUCLIDE_ENTRY (french)  67(2)
'D','u','p','l','i','c','a','t','e',' ','N','a','m','e',' ','f','o','u','n','d',':','\n',		// L_DUPLICATE_NAME_FOUND (english)  22(0), 311
'N','o','m','b','r','e',' ','d','u','p','l','i','c','a','d','o',' ','e','n','c','o','n','t','r','a','d','o',':','\n',		// L_DUPLICATE_NAME_FOUND (french)  29(1)
'S','e','t','u','p',' ','S','o','u','r','c','e','s',' ','E','r','r','o','r',		// L_SETUP_SOURCES_ERROR (english)  19(0), 312
'E','r','r','o','r',' ','C','o','n','f','i','g','.',' ','d','e',' ','F','u','e','n','t','e','s',		// L_SETUP_SOURCES_ERROR (french)  24(0)
'C','o','n','s','t','a','n','c','y',' ','S','o','u','r','c','e',' ','s','e','l','e','c','t','e','d',' ','w','i','t','h','o','u','t',' ','C','o','n','s','t','a','n','c','y',' ','C','h','a','n','n','e','l','s',		// L_NO_CONSTANCY_CHANNEL (english)  52(2), 313
'F','u','e','n','t','e',' ','d','e',' ','E','s','t','a','b','i','l','i','d','a','d',' ','s','e','l','e','c','c','i','o','n','a','d','a',' ','s','i','n',' ','l','o','s',' ','C','a','n','a','l','e','s',' ','d','e',' ','E','s','t','a','b','i','l','i','d','a','d',		// L_NO_CONSTANCY_CHANNEL (french)  65(2)
'C','o','n','s','t','a','n','c','y',' ','C','h','a','n','n','e','l','s',' ','s','e','l','e','c','t','e','d',' ','w','i','t','h','o','u','t',' ','C','o','n','s','t','a','n','c','y',' ','S','o','u','r','c','e',		// L_NO_CONSTANCY_SOURCE (english)  52(2), 314
'C','a','n','a','l','e','s',' ','d','e',' ','E','s','t','a','b','i','l','i','d','a','d',' ','s','e','l','e','c','c','i','o','n','a','d','o','s',' ','s','i','n',' ','F','u','e','n','t','e',' ','d','e',' ','E','s','t','a','b','i','l','i','d','a','d',		// L_NO_CONSTANCY_SOURCE (french)  62(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','5','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','C','i','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CO57_CI (english)  66(2), 315
'E','n','c','i','m','a',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','á','x','i','m','a','\n','C','o','5','7',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','C','i','\n','L','í','m','i','t','e',' ','S','u','p','e','r','i','o','r',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CO57_CI (french)  77(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','5','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','G','B','q','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CO57_BQ (english)  68(2), 316
'E','n','c','i','m','a',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','á','x','i','m','a','\n','C','o','5','7',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','G','B','q','\n','L','í','m','i','t','e',' ','S','u','p','e','r','i','o','r',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CO57_BQ (french)  79(3)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','5','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','u','C','i','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CO57_CI (english)  68(2), 317
'D','e','b','a','j','o',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','í','n','i','m','a','\n','C','o','5','7',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','u','C','i','\n','L','í','m','i','t','e',' ','I','n','f','e','r','i','o','r',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CO57_CI (french)  79(3)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','5','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','6','f',' ','M','B','q','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CO57_BQ (english)  68(2), 318
'D','e','b','a','j','o',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','í','n','i','m','a','\n','C','o','5','7',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','6','f',' ','M','B','q','\n','L','í','m','i','t','e',' ','I','n','f','e','r','i','o','r',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CO57_BQ (french)  79(3)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','6','0',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','C','i','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CO60_CI (english)  66(2), 319
'E','n','c','i','m','a',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','á','x','i','m','a','\n','C','o','6','0',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','C','i','\n','L','í','m','i','t','e',' ','S','u','p','e','r','i','o','r',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CO60_CI (french)  77(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','6','0',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','G','B','q','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CO60_BQ (english)  68(2), 320
'E','n','c','i','m','a',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','á','x','i','m','a','\n','C','o','6','0',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','G','B','q','\n','L','í','m','i','t','e',' ','S','u','p','e','r','i','o','r',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CO60_BQ (french)  79(3)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','6','0',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','u','C','i','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CO60_CI (english)  68(2), 321
'D','e','b','a','j','o',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','í','n','i','m','a','\n','C','o','6','0',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','u','C','i','\n','L','í','m','i','t','e',' ','I','n','f','e','r','i','o','r',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CO60_CI (french)  79(3)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','6','0',' ','A','c','t','i','v','i','t','y',':',' ','%','.','6','f',' ','M','B','q','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CO60_BQ (english)  68(2), 322
'D','e','b','a','j','o',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','í','n','i','m','a','\n','C','o','6','0',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','6','f',' ','M','B','q','\n','L','í','m','i','t','e',' ','I','n','f','e','r','i','o','r',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CO60_BQ (french)  79(3)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','C','i','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_BA133_CI (english)  67(2), 323
'E','n','c','i','m','a',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','á','x','i','m','a','\n','B','a','1','3','3',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','C','i','\n','L','í','m','i','t','e',' ','S','u','p','e','r','i','o','r',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_BA133_CI (french)  78(3)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','G','B','q','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_BA133_BQ (english)  69(2), 324
'E','n','c','i','m','a',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','á','x','i','m','a','\n','B','a','1','3','3',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','G','B','q','\n','L','í','m','i','t','e',' ','S','u','p','e','r','i','o','r',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_BA133_BQ (french)  80(3)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','u','C','i','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_BA133_CI (english)  69(2), 325
'D','e','b','a','j','o',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','í','n','i','m','a','\n','B','a','1','3','3',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','u','C','i','\n','L','í','m','i','t','e',' ','I','n','f','e','r','i','o','r',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_BA133_CI (french)  80(3)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t','y',':',' ','%','.','6','f',' ','M','B','q','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_BA133_BQ (english)  69(2), 326
'D','e','b','a','j','o',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','í','n','i','m','a','\n','B','a','1','3','3',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','6','f',' ','M','B','q','\n','L','í','m','i','t','e',' ','I','n','f','e','r','i','o','r',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_BA133_BQ (french)  80(3)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','C','i','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CS137_CI (english)  67(2), 327
'E','n','c','i','m','a',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','á','x','i','m','a','\n','C','s','1','3','7',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','C','i','\n','L','í','m','i','t','e',' ','S','u','p','e','r','i','o','r',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CS137_CI (french)  78(3)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','G','B','q','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CS137_BQ (english)  69(2), 328
'E','n','c','i','m','a',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','á','x','i','m','a','\n','C','s','1','3','7',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','G','B','q','\n','L','í','m','i','t','e',' ','S','u','p','e','r','i','o','r',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CS137_BQ (french)  80(3)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','u','C','i','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CS137_CI (english)  69(2), 329
'D','e','b','a','j','o',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','í','n','i','m','a','\n','C','s','1','3','7',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','u','C','i','\n','L','í','m','i','t','e',' ','I','n','f','e','r','i','o','r',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CS137_CI (french)  80(3)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','6','f',' ','M','B','q','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CS137_BQ (english)  69(2), 330
'D','e','b','a','j','o',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','í','n','i','m','a','\n','C','s','1','3','7',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','6','f',' ','M','B','q','\n','L','í','m','i','t','e',' ','I','n','f','e','r','i','o','r',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CS137_BQ (french)  80(3)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','N','a','2','2',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','C','i','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_NA22_CI (english)  66(2), 331
'E','n','c','i','m','a',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','á','x','i','m','a','\n','N','a','2','2',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','C','i','\n','L','í','m','i','t','e',' ','S','u','p','e','r','i','o','r',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_NA22_CI (french)  77(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','N','a','2','2',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','G','B','q','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_NA22_BQ (english)  68(2), 332
'E','n','c','i','m','a',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','á','x','i','m','a','\n','N','a','2','2',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','G','B','q','\n','L','í','m','i','t','e',' ','S','u','p','e','r','i','o','r',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_NA22_BQ (french)  79(3)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','N','a','2','2',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','u','C','i','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_NA22_CI (english)  68(2), 333
'D','e','b','a','j','o',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','í','n','i','m','a','\n','N','a','2','2',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','3','f',' ','u','C','i','\n','L','í','m','i','t','e',' ','I','n','f','e','r','i','o','r',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_NA22_CI (french)  79(3)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','N','a','2','2',' ','A','c','t','i','v','i','t','y',':',' ','%','.','6','f',' ','M','B','q','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_NA22_BQ (english)  68(2), 334
'D','e','b','a','j','o',' ','d','e',' ','A','c','t','i','v','i','d','a','d',' ','M','í','n','i','m','a','\n','N','a','2','2',' ','A','c','t','i','v','i','d','a','d',':',' ','%','.','6','f',' ','M','B','q','\n','L','í','m','i','t','e',' ','I','n','f','e','r','i','o','r',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_NA22_BQ (french)  79(3)
'I','n','p','u','t',' ','E','r','r','o','r',		// L_INPUT_ERROR_ENGLISH (english)  11(0), 335
'I','n','p','u','t',' ','E','r','r','o','r',		// L_INPUT_ERROR_ENGLISH (french)  11(0)
'M','a','x','i','m','u','m',' ','i','s',' ','3','6','9','.','9',' ','M','B','q',		// L_MAX_IS_369_9_MBQ_ENGLISH (english)  20(0), 336
'M','a','x','i','m','u','m',' ','i','s',' ','3','6','9','.','9',' ','M','B','q',		// L_MAX_IS_369_9_MBQ_ENGLISH (french)  20(0)
'M','a','x','i','m','u','m',' ','i','s',' ','9','9','9','9',' ','u','C','i',		// L_MAX_IS_9999_UCI_ENGLISH (english)  19(0), 337
'M','a','x','i','m','u','m',' ','i','s',' ','9','9','9','9',' ','u','C','i',		// L_MAX_IS_9999_UCI_ENGLISH (french)  19(0)
'M','i','n','i','m','u','m',' ','i','s',' ','3','.','7','0','1',' ','k','B','q',		// L_MIN_IS_3_701_KBQ_ENGLISH (english)  20(0), 338
'M','i','n','i','m','u','m',' ','i','s',' ','3','.','7','0','1',' ','k','B','q',		// L_MIN_IS_3_701_KBQ_ENGLISH (french)  20(0)
'M','i','n','i','m','u','m',' ','i','s',' ','.','1',' ','u','C','i',		// L_MIN_IS_1_UCI_ENGLISH (english)  17(0), 339
'M','i','n','i','m','u','m',' ','i','s',' ','.','1',' ','u','C','i',		// L_MIN_IS_1_UCI_ENGLISH (french)  17(0)
'C','u','r','r','e','n','t',' ','F','a','c','t','o','r',':',' ','%','.','2','f',		// L_CURRENT_FACTOR (english)  20(0), 340
'F','a','c','t','o','r',' ','A','c','t','u','a','l',':',' ','%','.','2','f',		// L_CURRENT_FACTOR (french)  19(0)
'C','h','a','m','b','e','r',' ','S','e','r','i','a','l',':',' ','%','s',		// L_CHAMBER_SERIAL (english)  18(0), 341
'S','e','r','i','e',' ','d','e',' ','C','á','m','a','r','a',':',' ','%','s',		// L_CHAMBER_SERIAL (french)  20(0)
'(','C','h','a','m','b','e','r',' ','S','e','r','i','a','l',')',' ','O','l','d',':','%','s',',',' ','N','e','w',':','%','s',		// L_CHAMBER_SERIAL_OLD_NEW (english)  31(1), 342
'(','S','e','r','i','e',' ','C','á','m','a','r','a',')',' ','V','i','e','j','o',':','%','s',',',' ','N','u','e','v','o',':','%','s',		// L_CHAMBER_SERIAL_OLD_NEW (french)  34(1)
'S','e','t','u','p',' ','L','i','n','e','a','t','o','r',' ','E','r','r','o','r',		// L_SETUP_LINEATOR_ERROR (english)  20(0), 343
'E','r','r','o','r',' ','e','n',' ','C','o','n','f','i','g','.',' ','d','e','l',' ','L','i','n','e','a','t','o','r',		// L_SETUP_LINEATOR_ERROR (french)  29(1)
'S','e','t','u','p',' ','C','a','l','i','c','h','e','c','k',' ','E','r','r','o','r',		// L_SETUP_CALICHECK_ERROR (english)  21(0), 344
'E','r','r','o','r',' ','C','o','n','f','i','g','.',' ','d','e',' ','C','a','l','i','c','h','e','c','k',		// L_SETUP_CALICHECK_ERROR (french)  26(1)
'P','l','e','a','s','e',' ','f','i','l','l',' ','i','n',' ','a','l','l',' ','m','e','a','s','u','r','e','m','e','n','t','s',		// L_FILL_IN_ALL_MEASUREMENTS (english)  31(1), 345
'P','o','r',' ','f','a','v','o','r',' ','l','l','e','n','e',' ','t','o','d','a','s',' ','l','a','s',' ','m','e','d','i','d','a','s',		// L_FILL_IN_ALL_MEASUREMENTS (french)  33(1)
'P','l','e','a','s','e',' ','f','i','l','l',' ','i','n',' ','t','h','e',' ','N','u','m',' ','o','f',' ','M','e','a','s','u','r','e','m','e','n','t','s',		// L_FILL_IN_NUM_MEASUREMENTS (english)  38(1), 346
'P','o','r',' ','f','a','v','o','r',' ','l','l','e','n','e',' ','n','u','m','e','r','o',' ','d','e',' ','m','e','d','i','d','a','s',		// L_FILL_IN_NUM_MEASUREMENTS (french)  33(1)
'P','l','e','a','s','e',' ','f','i','l','l',' ','i','n',' ','L','i','n','e','a','t','o','r',' ','S','e','r','i','a','l',' ','N','u','m','b','e','r',		// L_FILL_IN_LINEATOR_SERIAL_NUMBER (english)  37(1), 347
'L','l','e','n','e',' ','e','l',' ','N','ú','m','e','r','o',' ','d','e',' ','S','e','r','i','e',' ','d','e','l',' ','L','i','n','e','a','t','o','r',		// L_FILL_IN_LINEATOR_SERIAL_NUMBER (french)  38(1)
'P','l','e','a','s','e',' ','f','i','l','l',' ','i','n',' ','C','a','l','i','c','h','e','c','k',' ','S','e','r','i','a','l',' ','N','u','m','b','e','r',		// L_FILL_IN_CALICHECK_SERIAL_NUMBER (english)  38(1), 348
'L','l','e','n','e',' ','e','l',' ','N','ú','m','e','r','o',' ','d','e',' ','S','e','r','i','e',' ','d','e',' ','C','a','l','i','c','h','e','c','k',		// L_FILL_IN_CALICHECK_SERIAL_NUMBER (french)  38(1)
'O','l','d',':',' ','%','.','2','f',',',' ','N','e','w',':',' ','%','.','2','f',		// L_FACTOR_OLD_NEW (english)  20(0), 349
'V','i','e','j','o',':',' ','%','.','2','f',',',' ','N','u','e','v','o',':',' ','%','.','2','f',		// L_FACTOR_OLD_NEW (french)  24(0)
'N','e','w',' ','F','a','c','t','o','r',':',' ','%','.','2','f',		// L_NEW_FACTOR (english)  16(0), 350
'N','u','e','v','o',' ','F','a','c','t','o','r',':',' ','%','.','2','f',		// L_NEW_FACTOR (french)  18(0)
'T','h','e',' ','c','u','r','r','e','n','t',' ','f','a','c','t','o','r','s',' ','a','r','e',' ','f','o','r',' ','a',' ','d','i','f','f','e','r','e','n','t',' ','c','h','a','m','b','e','r','\n','a','n','d',' ','c','a','n',' ','n','o','t',' ','b','e',' ','u','s','e','d',' ','f','o','r',' ','t','h','i','s',' ','c','h','a','m','b','e','r',		// L_DIFFERENT_CHAMBERS (english)  84(3), 351
'L','o','s',' ','f','a','c','t','o','r','e','s',' ','a','c','t','u','a','l','e','s',' ','s','o','n',' ',' ','p','a','r','a',' ','u','n','a',' ','c','á','m','a','r','a',' ',' ','d','i','f','e','r','e','n','t','e','\n','y',' ','n','o',' ','s','e',' ','p','u','e','d','e',' ','u','t','i','l','i','z','a','r',' ','p','a','r','a',' ','e','s','t','a',' ','c','á','m','a','r','a','.',		// L_DIFFERENT_CHAMBERS (french)  96(3)
'S','e','t','u','p',' ','L','i','n','e','a','t','o','r',		// L_SETUP_LINEATOR (english)  14(0), 352
'C','o','n','f','i','g','u','r','a','c','i','ó','n',' ','d','e','l',' ','L','i','n','e','a','t','o','r',		// L_SETUP_LINEATOR (french)  27(1)
'S','e','t','u','p',' ','C','a','l','i','c','h','e','c','k',		// L_SETUP_CALICHECK (english)  15(0), 353
'C','o','n','f','i','g','u','r','a','c','i','ó','n',' ','d','e',' ','C','a','l','i','c','h','e','c','k',		// L_SETUP_CALICHECK (french)  27(1)
'I','n','v','a','l','i','d',' ','D','a','y',		// L_INVALID_DAY (english)  11(0), 354
'D','i','a',' ','i','n','v','a','l','i','d','o',		// L_INVALID_DAY (french)  12(0)
'I','n','v','a','l','i','d',' ','H','o','u','r',		// L_INVALID_HOUR (english)  12(0), 355
'H','o','r','a',' ','i','n','v','a','l','i','d','a',		// L_INVALID_HOUR (french)  13(0)
'J','a','n',		// L_JAN (english)  3(0), 356
'e','n','e',		// L_JAN (french)  3(0)
'F','e','b',		// L_FEB (english)  3(0), 357
'f','e','b',		// L_FEB (french)  3(0)
'M','a','r',		// L_MAR (english)  3(0), 358
'm','a','r',		// L_MAR (french)  3(0)
'A','p','r',		// L_APR (english)  3(0), 359
'a','b','r',		// L_APR (french)  3(0)
'M','a','y',		// L_MAY (english)  3(0), 360
'm','a','y',		// L_MAY (french)  3(0)
'J','u','n',		// L_JUN (english)  3(0), 361
'j','u','n',		// L_JUN (french)  3(0)
'J','u','l',		// L_JUL (english)  3(0), 362
'j','u','l',		// L_JUL (french)  3(0)
'A','u','g',		// L_AUG (english)  3(0), 363
'a','g','o',		// L_AUG (french)  3(0)
'S','e','p',		// L_SEP (english)  3(0), 364
's','e','p',		// L_SEP (french)  3(0)
'O','c','t',		// L_OCT (english)  3(0), 365
'o','c','t',		// L_OCT (french)  3(0)
'N','o','v',		// L_NOV (english)  3(0), 366
'n','o','v',		// L_NOV (french)  3(0)
'D','e','c',		// L_DEC (english)  3(0), 367
'd','e','c',		// L_DEC (french)  3(0)
'S','u','n',		// L_SUN (english)  3(0), 368
'd','o','m',		// L_SUN (french)  3(0)
'M','o','n',		// L_MON (english)  3(0), 369
'l','u','n',		// L_MON (french)  3(0)
'T','u','e',		// L_TUE (english)  3(0), 370
'm','a','r',		// L_TUE (french)  3(0)
'W','e','d',		// L_WED (english)  3(0), 371
'm','i','é',		// L_WED (french)  4(0)
'T','h','u',		// L_THU (english)  3(0), 372
'j','u','e',		// L_THU (french)  3(0)
'F','r','i',		// L_FRI (english)  3(0), 373
'v','i','e',		// L_FRI (french)  3(0)
'S','a','t',		// L_SAT (english)  3(0), 374
's','á','b',		// L_SAT (french)  4(0)
'S','/','N',' ','M','a','i','n',' ','U','n','i','t',':',		// L_SN_MAIN_UNIT (english)  14(0), 375
'S','/','N',' ','U','n','i','d','a','d',' ','P','r','i','n','c','i','p','a','l',':',		// L_SN_MAIN_UNIT (french)  21(0)
'C','A','L','I','C','H','E','C','K',' ','T','E','S','T',' ','C','A','L','I','B','R','A','T','I','O','N',		// L_CALICHECK_TEST_CALIBRATION (english)  26(1), 376
'P','r','u','e','b','a',' ','d','e',' ','C','a','l','i','b','.',' ','d','e',' ','C','a','l','i','c','h','e','c','k',		// L_CALICHECK_TEST_CALIBRATION (french)  29(1)
'L','I','N','E','A','T','O','R',' ','T','E','S','T',' ','C','A','L','I','B','R','A','T','I','O','N',' ',		// L_LINEATOR_TEST_CALIBRATION (english)  26(1), 377
'P','R','U','E','B','A',' ','D','E',' ','C','A','L','I','B','.',' ','D','E','L',' ','L','I','N','E','A','T','O','R',' ',		// L_LINEATOR_TEST_CALIBRATION (french)  30(1)
'I','n','i','t','i','a','l',' ','F','a','c','t','o','r',':',' ',' ',' ',' ',' ',		// L_INITIAL_FACTOR (english)  20(0), 378
'F','a','c','t','o','r',' ','I','n','i','c','i','a','l',':',' ',' ',' ',' ',' ',		// L_INITIAL_FACTOR (french)  20(0)
'C','a','l','i','b','r','a','t','i','o','n',' ','F','a','c','t','o','r',':',' ',		// L_CALIBRATION_FACTOR (english)  20(0), 379
'F','a','c','t','o','r',' ','d','e',' ','C','a','l','i','b','r','a','c','i','ó','n',':',' ',		// L_CALIBRATION_FACTOR (french)  24(0)
'C','h',' ','S','e','r','i','a','l',')','O','l','d',':','%','s',' ','N','e','w',':','%','s','	','	',		// L_CH_SERIAL_OLD_NEW (english)  25(0), 380
'S','e','r','i','e',' ','d','e',' ','C','h',')','V','i','e','j','o',':','%','s',' ','N','u','e','v','o',':','%','s','	','	',		// L_CH_SERIAL_OLD_NEW (french)  31(1)
'R',' ','C','h','a','m','b','e','r',		// L_R_CHAMBER (english)  9(0), 381
'c','á','m','a','r','a',' ','R',		// L_R_CHAMBER (french)  9(0)
'P',' ','C','h','a','m','b','e','r',		// L_P_CHAMBER (english)  9(0), 382
'c','á','m','a','r','a',' ','P',		// L_P_CHAMBER (french)  9(0)
'S','e','t','u','p',' ','R','e','m','o','t','e',' ','N','u','c','l','i','d','e','s',		// L_SETUP_REMOTE_NUCLIDES (english)  21(0), 383
'C','o','n','f','i','g','u','r','a',' ','R','e','m','o','t','a',' ','d','e',' ','l','o','s',' ','N','u','c','l','e','i','d','o','s',		// L_SETUP_REMOTE_NUCLIDES (french)  33(1)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','P','a','u','s','e','d','!','!','!',		// L_AUTOLINEARITY_PAUSED (english)  23(0), 384
'A','u','t','o','L','i','n','.',' ','P','a','u','s','a','d','a','!','!','!',		// L_AUTOLINEARITY_PAUSED (french)  19(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','C','a','l',' ','#',':',		// L_PLEASE_ENTER_CAL_NUM (english)  19(0), 385
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','e','l',' ','C','a','l',' ','#',':',		// L_PLEASE_ENTER_CAL_NUM (french)  29(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','D','o','s','e',' ','T','i','m','e',':',		// L_PLEASE_ENTER_DOSE_TIME (english)  23(0), 386
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','T','i','e','m','p','o',' ','d','e',' ','D','o','s','i','s',':',		// L_PLEASE_ENTER_DOSE_TIME (french)  36(1)
'L','o','w',' ','B','a','t','t','e','r','y',		// L_LOW_BATTERY (english)  11(0), 387
'B','a','t','e','r','í','a',' ','B','a','j','a',		// L_LOW_BATTERY (french)  13(0)
'D','o','s','e',' ','D','e','c','a','y',		// L_DOSE_DECAY (english)  10(0), 388
'D','e','c','a','i','m','i','e','n','t','o',' ','d','e',' ','D','o','s','i','s',		// L_DOSE_DECAY (french)  20(0)
'H','o','m','e','\n',' ','S','c','r','e','e','n',		// L_HOME_SCREEN (english)  12(0), 389
'P','a','n','t','a','l','l','a','\n','d','e',' ','I','n','i','c','i','o',		// L_HOME_SCREEN (french)  18(0)
'N','u','c','l','i','d','e','\n',' ','S','c','r','e','e','n',		// L_NUCLIDE_SCREEN (english)  15(0), 390
'P','a','n','t','a','l','l','a',' ','d','e','\n','N','u','c','l','e','i','d','o',		// L_NUCLIDE_SCREEN (french)  20(0)
'Y','E','S',		// L_BTN_YES (english)  3(0), 391
'S','I',		// L_BTN_YES (french)  2(0)
'N','O',		// L_BTN_NO (english)  2(0), 392
'N','O',		// L_BTN_NO (french)  2(0)
'D','e','l','e','t','e',' ','L','i','n','e','a','t','o','r',' ','S','e','t','t','i','n','g','s',		// L_DELETE_LINEATOR_SETTINGS (english)  24(0), 393
'B','o','r','r','a','r',' ','A','j','u','s','t','e','s',' ','d','e',' ','L','i','n','e','a','t','o','r',		// L_DELETE_LINEATOR_SETTINGS (french)  26(1)
'D','e','l','e','t','e',' ','C','a','l','i','c','h','e','c','k',' ','S','e','t','t','i','n','g','s',		// L_DELETE_CALICHECK_SETTINGS (english)  25(0), 394
'B','o','r','r','a','r',' ','A','j','u','s','t','e','s',' ','d','e',' ','C','a','l','i','c','h','e','c','k',		// L_DELETE_CALICHECK_SETTINGS (french)  27(1)
'D','e','l','e','t','e',' ','L','i','n','e','a','t','o','r',' ','S','e','t','t','i','n','g','s','?',		// L_DELETE_LINEATOR_SETTINGS_Q (english)  25(0), 395
'¿','B','o','r','r','a','r',' ','A','j','u','s','t','e','s',' ','d','e',' ','L','i','n','e','a','t','o','r','?',		// L_DELETE_LINEATOR_SETTINGS_Q (french)  29(1)
'D','e','l','e','t','e',' ','C','a','l','i','c','h','e','c','k',' ','S','e','t','t','i','n','g','s','?',		// L_DELETE_CALICHECK_SETTINGS_Q (english)  26(1), 396
'¿','B','o','r','r','a','r',' ','A','j','u','s','t','e','s',' ','d','e',' ','C','a','l','i','c','h','e','c','k','?',		// L_DELETE_CALICHECK_SETTINGS_Q (french)  30(1)
'U','t','i','l','i','t','y',		// L_UTILITY (english)  7(0), 397
'U','t','i','l','i','d','a','d',		// L_UTILITY (french)  8(0)
'[','C','i',',','B','q',' ','C','o','n','v',']',' ','I','n','p','u','t',' ','A','c','t','i','v','i','t','y',':',		// L_CONV_INPUT_ACTIVITY (english)  28(1), 398
'[','C','i',',','B','q',' ','C','o','n','v',']',' ','I','n','t','r','o','d','u','c','i','r',' ','A','c','t','i','v','i','d','a','d',':',		// L_CONV_INPUT_ACTIVITY (french)  34(1)
'[','D','e','c','a','y',' ','C','a','l','c','u','l','a','t','o','r',']',		// L_DECAY_CALCULATOR (english)  18(0), 399
'[','C','a','l','c','u','l','a','d','o','r','a',' ','d','e',' ','d','e','c','a','i','m','i','e','n','t','o',']',		// L_DECAY_CALCULATOR (french)  28(1)
'F','R','O','M',':',		// L_FROM (english)  5(0), 400
'D','E','S','D','E',':',		// L_FROM (french)  6(0)
'F','r','o','m',':',		// L_FROM2 (english)  5(0), 401
'D','e','s','d','e',':',		// L_FROM2 (french)  6(0)
'T','O',':',		// L_TO (english)  3(0), 402
'A',':',		// L_TO (french)  2(0)
'T','o',':',		// L_TO2 (english)  3(0), 403
'A',':',		// L_TO2 (french)  2(0)
'D','i','a','g','n','o','s','t','i','c','s',		// L_DIAGNOSTICS (english)  11(0), 404
'D','i','a','g','n','o','s','t','i','c','o','s',		// L_DIAGNOSTICS (french)  12(0)
'D','o','s','e',' ','T','a','b','l','e',		// L_DOSE_TABLE (english)  10(0), 405
'T','a','b','l','a',' ','d','e',' ','D','o','s','i','s',		// L_DOSE_TABLE (french)  14(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','A','c','t','i','v','i','t','y',':',		// L_PLEASE_ENTER_ACTIVITY (english)  22(0), 406
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','A','c','t','i','v','i','d','a','d',':',		// L_PLEASE_ENTER_ACTIVITY (french)  30(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','S','t','a','r','t',' ','T','i','m','e',':',		// L_PLEASE_ENTER_START_TIME (english)  24(0), 407
'F','a','v','o','r',' ','d','e',' ','i','n','t','r','o','d','u','c','i','r',' ','h','o','r','a',' ','d','e',' ','e','m','p','e','z','a','r',':',		// L_PLEASE_ENTER_START_TIME (french)  36(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','E','n','d',' ','T','i','m','e',':',		// L_PLEASE_ENTER_END_TIME (english)  22(0), 408
'F','a','v','o','r',' ','d','e',' ','i','n','t','r','o','d','u','c','i','r',' ','h','o','r','a',' ','d','e',' ','f','i','n','a','l','i','z','a','c','i','ó','n',':',		// L_PLEASE_ENTER_END_TIME (french)  42(1)
'T','O','O',' ','F','A','R',' ','I','N','T','O',' ','F','U','T','U','R','E',		// L_TOO_FAR_INTO_FUTURE (english)  19(0), 409
'D','E','M','A','S','I','A','D','O',' ','L','E','J','O','S',' ','H','A','C','I','A',' ','E','L',' ','F','U','T','U','R','O',		// L_TOO_FAR_INTO_FUTURE (french)  31(1)
'T','O','O',' ','F','A','R',' ','I','N','T','O',' ','P','A','S','T',		// L_TOO_FAR_INTO_PAST (english)  17(0), 410
'D','E','M','A','S','I','A','D','O',' ','L','E','J','O','S',' ','H','A','C','I','A',' ','E','L',' ','P','A','S','A','D','O',		// L_TOO_FAR_INTO_PAST (french)  31(1)
'T','O','O',' ','L','A','R','G','E',		// L_TOO_LARGE (english)  9(0), 411
'M','U','Y',' ','G','R','A','N','D','E',		// L_TOO_LARGE (french)  10(0)
'P','R','O','G','R','A','M',' ','I','N','T','E','G','R','I','T','Y',		// L_PROGRAM_INTEGRITY (english)  17(0), 412
'I','N','T','E','G','R','I','D','A','D',' ','D','E','L',' ','P','R','O','G','R','A','M','A',		// L_PROGRAM_INTEGRITY (french)  23(0)
'F','A','I','L',':',' ',		// L_FAIL (english)  6(0), 413
'F','A','L','L','A','R',':',' ',		// L_FAIL (french)  8(0)
'F','A','I','L',		// L_FAIL2 (english)  4(0), 414
'F','A','L','L','A','R',		// L_FAIL2 (french)  6(0)
'P','A','S','S',':',' ',		// L_PASS (english)  6(0), 415
'S','U','P','E','R','A','D','A',':',' ',		// L_PASS (french)  10(0)
'P','A','S','S',		// L_PASS2 (english)  4(0), 416
'S','U','P','E','R','A','D','A',		// L_PASS2 (french)  8(0)
'P','a','s','s','e','d',		// L_PASSED (english)  6(0), 417
'A','p','r','o','v','a','d','o',		// L_PASSED (french)  8(0)
'F','a','i','l','e','d',',',' ',		// L_FAILED (english)  8(0), 418
'F','r','a','c','a','s','a','d','o',',',' ',		// L_FAILED (french)  11(0)
'D','a','i','l','y',' ','T','e','s','t',		// L_DAILY_TEST (english)  10(0), 419
'P','r','u','e','b','a',' ','D','i','a','r','i','a',		// L_DAILY_TEST (french)  13(0)
'D','A','I','L','Y',' ','T','E','S','T',':',		// L_DAILY_TEST2 (english)  11(0), 420
'P','R','U','E','B','A',' ','D','I','A','R','I','A',':',		// L_DAILY_TEST2 (french)  14(0)
'1',')',' ','R','e','m','o','v','e',' ','A','l','l',' ','S','o','u','r','c','e','s',' ','f','r','o','m',' ','C','h','a','m','b','e','r',		// L_REMOVE_ALL_SOURCES_FROM_CHAMBER (english)  34(1), 421
'1',')',' ','E','l','i','m','i','n','a','r',' ','t','o','d','a','s',' ','l','a','s',' ','f','u','e','n','t','e','s',' ','d','e',' ','l','a',' ','c','á','m','a','r','a',		// L_REMOVE_ALL_SOURCES_FROM_CHAMBER (french)  43(1)
'2',')',' ','A','u','t','o',' ','Z','e','r','o',		// L_AUTO_ZERO (english)  12(0), 422
'2',')',' ','A','u','t','o',' ','c','e','r','o',		// L_AUTO_ZERO (french)  12(0)
'3',')',' ','M','e','a','s','u','r','e',' ','B','a','c','k','g','n','d',		// L_MEASURE_BACKGROUND (english)  18(0), 423
'3',')',' ','M','e','d','i','d','a',' ','d','e',' ','f','o','n','d','o',		// L_MEASURE_BACKGROUND (french)  18(0)
'2',')',' ','M','e','a','s','u','r','e',' ','B','a','c','k','g','n','d',		// L_MEASURE_BACKGROUND2 (english)  18(0), 424
'2',')',' ','M','e','d','i','d','a',' ','d','e',' ','f','o','n','d','o',		// L_MEASURE_BACKGROUND2 (french)  18(0)
'4',')',' ','C','h','e','c','k',' ','C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',		// L_CHECK_CHAMBER_VOLTAGE (english)  24(0), 425
'4',')',' ','V','e','r','i','f','i','q','u','e',' ','e','l',' ','v','o','l','t','a','j','e',' ','d','e',' ','l','a',' ','c','á','m','a','r','a',		// L_CHECK_CHAMBER_VOLTAGE (french)  37(1)
'1',')',' ','C','h','e','c','k',' ','C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',		// L_CHECK_CHAMBER_VOLTAGE2 (english)  24(0), 426
'1',')',' ','V','e','r','i','f','i','q','u','e',' ','e','l',' ','v','o','l','t','a','j','e',' ','d','e',' ','l','a',' ','c','á','m','a','r','a',		// L_CHECK_CHAMBER_VOLTAGE2 (french)  37(1)
'5',')',' ','D','a','t','a',' ','C','h','e','c','k',		// L_DATA_CHECK (english)  13(0), 427
'5',')',' ','V','e','r','i','f','i','c','a','c','i','ó','n',' ','d','e',' ','l','o','s',' ','d','a','t','o','s',		// L_DATA_CHECK (french)  29(1)
'A','c','c','u','r','a','c','y',		// L_ACCURACY (english)  8(0), 428
'E','x','a','c','t','i','t','u','d',		// L_ACCURACY (french)  9(0)
'Z','e','r','o',' ','D','r','i','f','t',		// L_ZERO_DRIFT (english)  10(0), 429
'D','e','s','v','í','o',' ','C','e','r','o',		// L_ZERO_DRIFT (french)  12(0)
'Z','e','r','o',' ','o','u','t',' ','o','f',' ','R','a','n','g','e',		// L_ZERO_OUT_OF_RANGE (english)  17(0), 430
'C','e','r','o',' ','F','u','e','r','a',' ','d','e',' ','L','í','m','i','t','e',		// L_ZERO_OUT_OF_RANGE (french)  21(0)
'B','A','C','K','G','R','O','U','N','D',' ','T','O','O',' ','H','I','G','H',		// L_BACKGROUND_TOO_HIGH (english)  19(0), 431
'F','O','N','D','O',' ','D','E','M','A','S','I','A','D','O',' ','A','L','T','O',		// L_BACKGROUND_TOO_HIGH (french)  20(0)
'B','A','C','K','G','R','O','U','N','D','\n',' ','T','O','O',' ','H','I','G','H',		// L_BACKGROUND_TOO_HIGH2 (english)  20(0), 432
'F','O','N','D','O','\n',' ','D','E','M','A','S','I','A','D','O',' ','A','L','T','O',		// L_BACKGROUND_TOO_HIGH2 (french)  21(0)
'B','a','c','k','g','r','o','u','n','d',' ','T','o','o',' ','H','i','g','h',		// L_BACKGROUND_TOO_HIGH3 (english)  19(0), 433
'F','o','n','d','o',' ','D','e','m','a','s','i','a','d','o',' ','A','l','t','o',		// L_BACKGROUND_TOO_HIGH3 (french)  20(0)
'6',')',' ','N','O',' ','D','A','I','L','Y',' ','S','O','U','R','C','E',' ','D','A','T','A',		// L_NO_DAILY_SOURCE_DATA (english)  23(0), 434
'6',')',' ','N','O',' ','H','A','Y',' ','D','A','T','O',' ','D','E',' ','F','U','E','N','T','E',' ','D','I','A','R','I','A',		// L_NO_DAILY_SOURCE_DATA (french)  31(1)
'P','L','E','A','S','E',' ','W','A','I','T',' ','2',' ','M','i','n','u','t','e','s',' ','f','o','r',		// L_PLEASE_WAIT_2_MINUTES_FOR (english)  25(0), 435
'F','A','V','O','R',' ','D','E',' ','E','S','P','E','R','A','R',' ','2',' ','m','i','n','u','t','o','s',' ','p','a','r','a',		// L_PLEASE_WAIT_2_MINUTES_FOR (french)  31(1)
'S','t','a','b','i','l','i','z','a','t','i','o','n',		// L_STABILIZATION (english)  13(0), 436
'E','s','t','a','b','i','l','i','z','a','c','i','ó','n',		// L_STABILIZATION (french)  15(0)
'P','L','E','A','S','E',' ','W','A','I','T',' ','2',' ','M','i','n','u','t','e','s',' ','f','o','r',' ','S','t','a','b','i','l','i','z','a','t','i','o','n',		// L_PLEASE_WAIT_2_MINUTES_FOR_STABILIZATION (english)  39(1), 437
'F','A','V','O','R',' ','D','E',' ','E','S','P','E','R','A','R',' ','2',' ','m','i','n','u','t','o','s',' ','p','a','r','a',' ','E','s','t','a','b','i','l','i','z','a','c','i','ó','n',		// L_PLEASE_WAIT_2_MINUTES_FOR_STABILIZATION (french)  47(1)
'H','I','G','H',		// L_HIGH (english)  4(0), 438
'A','L','T','O',		// L_HIGH (french)  4(0)
'C','h','a','m','b','e','r',		// L_CHAMBER (english)  7(0), 439
'C','á','m','a','r','a',		// L_CHAMBER (french)  7(0)
'Z','E','R','O',':',		// L_ZERO (english)  5(0), 440
'C','E','R','O',':',		// L_ZERO (french)  5(0)
'B','A','C','K','G','R','O','U','N','D',':',		// L_BACKGROUND (english)  11(0), 441
'F','O','N','D','O',':',		// L_BACKGROUND (french)  6(0)
'B','a','c','k','g','r','o','u','n','d',		// L_BACKGROUND2 (english)  10(0), 442
'F','o','n','d','o',		// L_BACKGROUND2 (french)  5(0)
'C','H','A','M','B','E','R',' ','V','O','L','T','A','G','E',':',		// L_CHAMBER_VOLTAGE (english)  16(0), 443
'V','O','L','T','A','J','E',' ','d','e',' ','l','a',' ','C','Á','M','A','R','A',':',		// L_CHAMBER_VOLTAGE (french)  22(0)
'V','o','l','t','s',		// L_VOLTS (english)  5(0), 444
'V',		// L_VOLTS (french)  1(0)
'D','A','T','A',' ','C','H','E','C','K',':',		// L_DATA_CHECK_PRINTED (english)  11(0), 445
'V','e','r','i','f','i','c','a','c','i','ó','n',' ','d','e',' ','l','o','s',' ','d','a','t','o','s',':',		// L_DATA_CHECK_PRINTED (french)  27(1)
's','i','g','n','a','t','u','r','e',		// L_SIGNATURE (english)  9(0), 446
'f','i','r','m','a',		// L_SIGNATURE (french)  5(0)
'V','o','l','u','m','e',		// L_DOSE_TABLE_VOLUME (english)  6(0), 447
'V','o','l','u','m','e','n',		// L_DOSE_TABLE_VOLUME (french)  7(0)
'D','o','s','e',		// L_DOSE (english)  4(0), 448
'D','o','s','i','s',		// L_DOSE (french)  5(0)
'I','n','t','e','r','v','a','l',		// L_INTERVAL (english)  8(0), 449
'I','n','t','e','r','v','a','l','o',		// L_INTERVAL (french)  9(0)
'I','n','t','e','r','v','a','l',':',		// L_INTERVAL2 (english)  9(0), 450
'I','n','t','e','r','v','a','l','o',':',		// L_INTERVAL2 (french)  10(0)
'M','e','a','s','u','r','e',' ','A','c','t','i','v','i','t','y',		// L_MEASURE_ACTIVITY (english)  16(0), 451
'M','e','d','i','r',' ','l','a',' ','A','c','t','i','v','i','d','a','d',		// L_MEASURE_ACTIVITY (french)  18(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','V','o','l','u','m','e','(','m','l',')',':',		// L_PLEASE_ENTER_VOLUME (english)  24(0), 452
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','e','l',' ','V','o','l','u','m','e','n','(','m','l',')',':',		// L_PLEASE_ENTER_VOLUME (french)  35(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','D','o','s','e',		// L_PLEASE_ENTER_DOSE (english)  17(0), 453
'F','a','v','o','r',' ','d','e',' ','i','n','t','r','o','d','u','c','i','r',' ','d','o','s','i','s',':',		// L_PLEASE_ENTER_DOSE (french)  26(1)
'E','n','t','e','r',' ','I','n','t','e','r','v','a','l',' ','i','n',' ','m','i','n','u','t','e','s',':',		// L_ENTER_INTERVAL (english)  26(1), 454
'F','a','v','o','r',' ','d','e',' ','i','n','t','r','o','d','u','c','i','r',' ','I','n','t','e','r','v','a','l','o',' ','e','n',' ','m','i','n','u','t','o','s',':',		// L_ENTER_INTERVAL (french)  41(1)
'T','I','M','E',		// L_PR_TIME (english)  4(0), 455
'H','O','R','A',		// L_PR_TIME (french)  4(0)
'D','o','s','e',' ','T','a','b','l','e',' ','E','r','r','o','r',		// L_DOSE_TABLE_ERROR (english)  16(0), 456
'E','r','r','o','r',' ','d','e',' ','l','a',' ','T','a','b','l','a',' ','d','e',' ','D','o','s','i','s',		// L_DOSE_TABLE_ERROR (french)  26(1)
'D','o','s','e',' ','c','a','n',' ','n','o','t',' ','b','e',' ','l','a','r','g','e','r',' ','t','h','a','n',' ','i','n','i','t','i','a','l',' ','A','c','t','i','v','i','t','y','.',		// L_DOSE_LARGER_THAN_ACTIVITY (english)  45(1), 457
'D','o','s','i','s',' ','n','o',' ','p','u','e','d','e',' ','s','e','r',' ','m','a','y','o','r',' ','q','u','e',' ','l','a',' ','a','c','t','i','v','i','d','a','d',' ','i','n','i','c','i','a','l','.',		// L_DOSE_LARGER_THAN_ACTIVITY (french)  50(1)
' ','-',' ','E','x','c','e','e','d','s',' ','I','n','i','t','i','a','l',		// L_EXCEEDS_INITIAL (english)  18(0), 458
' ','-',' ','I','n','i','c','i','a','l',' ','E','x','c','e','d','i','d','o',		// L_EXCEEDS_INITIAL (french)  19(0)
'D','O','S','E',' ','T','A','B','L','E',		// L_PR_DOSE_TABLE (english)  10(0), 459
'T','A','B','L','A',' ','D','E',' ','D','O','S','I','S',		// L_PR_DOSE_TABLE (french)  14(0)
'V','O','L','U','M','E',' ','(','m','l',')',		// L_PR_VOLUME_HEADING (english)  11(0), 460
'V','O','L','U','M','E','N',' ','(','m','l',')',		// L_PR_VOLUME_HEADING (french)  12(0)
'E','x','c','e','e','d','s',' ','I','n','i','t','i','a','l',' ','V','o','l',		// L_PR_EXCEEDS_INITIAL (english)  19(0), 461
'V','o','l','.',' ','I','n','i','c','i','a','l',' ','E','x','c','e','d','i','d','o',		// L_PR_EXCEEDS_INITIAL (french)  21(0)
'N','U','C','L','I','D','E',' ','D','A','T','A',':',		// L_PR_NUCLIDE_DATA (english)  13(0), 462
'D','A','T','O','S',' ','D','E',' ','N','U','C','L','E','I','D','O',':',		// L_PR_NUCLIDE_DATA (french)  18(0)
'T','E','S','T',' ','S','O','U','R','C','E','S',':',		// L_PR_TEST_SOURCES (english)  13(0), 463
'F','U','E','N','T','E','S',' ','D','E',' ','P','R','U','E','B','A',':',		// L_PR_TEST_SOURCES (french)  18(0)
'N','o',' ','S','o','u','r','c','e',		// L_PR_NO_SOURCE (english)  9(0), 464
'N','i','n','g','u','n','a',' ','F','u','e','n','t','e',		// L_PR_NO_SOURCE (french)  14(0)
'D',':',' ','D','a','i','l','y',' ',' ','C',':',' ','C','o','n','s','t','a','n','c','y',' ','S','o','u','r','c','e',		// L_PR_DAILY_CONSTANCY (english)  29(1), 465
'D',':',' ','D','i','a','r','i','a',' ','C',':',' ','F','u','e','n','t','e',' ','d','e',' ','E','s','t','a','b','i','l','i','d','a','d',		// L_PR_DAILY_CONSTANCY (french)  34(1)
'R','e','s','p','o','n','s','e',' ','C','o','r','r','e','c','t','i','o','n',':',		// L_PR_RESPONSE_CORRECTION (english)  20(0), 466
'C','o','r','r','e','c','c','i','ó','n',' ','d','e',' ','r','e','s','p','u','e','s','t','a',':',		// L_PR_RESPONSE_CORRECTION (french)  25(0)
'(','C','o','6','0',' ','w','a','s',' ','L','O','W',' ',')',		// L_PR_WAS_LOW (english)  15(0), 467
'(','C','o','6','0',' ','f','u','e',' ','B','A','J','O',' ',')',		// L_PR_WAS_LOW (french)  16(0)
'(','C','o','6','0',' ','w','a','s',' ','H','I','G','H',')',		// L_PR_WAS_HIGH (english)  15(0), 468
'(','C','o','6','0',' ','f','u','e',' ','A','L','T','O',')',		// L_PR_WAS_HIGH (french)  15(0)
'N','o','m','i','n','a','l',' ','V','o','l','t','a','g','e',':',' ',		// L_PR_NOMINAL_VOLTAGE (english)  17(0), 469
'V','o','l','t','a','j','e',' ','N','o','m','i','n','a','l',':',' ',		// L_PR_NOMINAL_VOLTAGE (french)  17(0)
'M','E','M','O','R','Y',' ','S','T','A','T','U','S',		// L_PR_MEMORY_STATUS (english)  13(0), 470
'E','S','T','A','D','O',' ','D','E',' ','M','E','M','O','R','I','A',		// L_PR_MEMORY_STATUS (french)  17(0)
'A','v','a','i','l','a','b','l','e',':',		// L_PR_AVAILABLE (english)  10(0), 471
'D','i','s','p','o','n','i','b','l','e',':',		// L_PR_AVAILABLE (french)  11(0)
'T','o','t','a','l',':',		// L_PR_TOTAL (english)  6(0), 472
'T','o','t','a','l',':',		// L_PR_TOTAL (french)  6(0)
'R',' ','C','H','A','M','B','E','R',		// L_PR_R_CHAMBER (english)  9(0), 473
'C','á','m','a','r','a',' ','R',		// L_PR_R_CHAMBER (french)  9(0)
'P','E','T',' ','C','H','A','M','B','E','R',		// L_PR_PET_CHAMBER (english)  11(0), 474
'C','á','m','a','r','a',' ','P','E','T',		// L_PR_PET_CHAMBER (french)  11(0)
'P','E','T',' ','C','h','a','m','b','e','r',		// L_PET_CHAMBER (english)  11(0), 475
'C','á','m','a','r','a',' ','P','E','T',		// L_PET_CHAMBER (french)  11(0)
'M','e','a','s','u','r','e','d',':',		// L_MEASURED (english)  9(0), 476
'M','e','d','i','r',':',		// L_MEASURED (french)  6(0)
'M','e','a','s','u','r','e','d',		// L_MEASURED2 (english)  8(0), 477
'M','e','d','i','r',		// L_MEASURED2 (french)  5(0)
'M','e','a','s','u','r','e','d',		// L_MEASURED3 (english)  8(0), 478
'M','e','d','i','d','o',		// L_MEASURED3 (french)  6(0)
'C','a','l','c','u','l','a','t','e','d',':',		// L_CALCULATED (english)  11(0), 479
'C','a','l','c','u','l','a','d','o',':',		// L_CALCULATED (french)  10(0)
'A','u','t','o','C','o','n','s','t','a','n','c','y',		// L_AUTOCONSTANCY (english)  13(0), 480
'A','u','t','o','-','E','s','t','a','b','i','l','i','d','a','d',		// L_AUTOCONSTANCY (french)  16(0)
'N','o',' ','S','o','u','r','c','e',' ','D','a','t','a',		// L_NOSOURCEDATA (english)  14(0), 481
'N','i','n','g','ú','n',' ','D','a','t','o','s',' ','d','e',' ','F','u','e','n','t','e',		// L_NOSOURCEDATA (french)  23(0)
'D','e','v','i','a','t','i','o','n',':',		// L_DEVIATION (english)  10(0), 482
'D','e','s','v','i','a','c','i','ó','n',':',		// L_DEVIATION (french)  12(0)
'U','S','E','R',' ','C','H','A','M','B','E','R',' ','N','U','C','L','I','D','E','S',':',		// L_PR_USER_CHAMBER_NUCLIDES (english)  22(0), 483
'U','S','U','A','R','I','O',' ','d','e',' ','C','Á','M','A','R','A',' ','d','e',' ','N','U','C','L','E','I','D','O','S',':',		// L_PR_USER_CHAMBER_NUCLIDES (french)  32(1)
'H','V',' ','d','i','r','e','c','t','l','y',' ','i','n','t','o',' ','A','D','C',		// L_PR_HV_DIRECTLY_INTO_ADC (english)  20(0), 484
'H','V',' ','d','i','r','e','c','t','a','m','e','n','t','e',' ','e','n',' ','A','D','C',		// L_PR_HV_DIRECTLY_INTO_ADC (french)  22(0)
'S','i','n','g','l','e',' ','G','a','i','n',' ','R','e','l','a','y',		// L_PR_SINGLE_GAIN_RELAY (english)  17(0), 485
'R','e','l','é',' ','d','e',' ','g','a','n','a','n','c','i','a',' ','ú','n','i','c','a',		// L_PR_SINGLE_GAIN_RELAY (french)  24(0)
'G','a','i','n',':',' ','%','4','.','6','f',		// L_PR_GAIN (english)  11(0), 486
'G','a','n','a','n','c','i','a',':',' ','%','4','.','6','f',		// L_PR_GAIN (french)  15(0)
'M',		// L_PR_TIME_UNIT_1 (english)  1(0), 487
'M',		// L_PR_TIME_UNIT_1 (french)  1(0)
'H',		// L_PR_TIME_UNIT_2 (english)  1(0), 488
'H',		// L_PR_TIME_UNIT_2 (french)  1(0)
'D',		// L_PR_TIME_UNIT_3 (english)  1(0), 489
'D',		// L_PR_TIME_UNIT_3 (french)  1(0)
'Y',		// L_PR_TIME_UNIT_4 (english)  1(0), 490
'A',		// L_PR_TIME_UNIT_4 (french)  1(0)
'M','Y',		// L_PR_TIME_UNIT_5 (english)  2(0), 491
'M','A',		// L_PR_TIME_UNIT_5 (french)  2(0)
'H','a','l','f','-','L','i','f','e',		// L_PR_HALFLIFE (english)  9(0), 492
'V','i','d','a','-','m','e','d','i','a',		// L_PR_HALFLIFE (french)  10(0)
'H','a','l','f','l','i','f','e',':',		// L_PR_HALFLIFE2 (english)  9(0), 493
'V','i','d','a',' ','m','e','d','i','a',':',		// L_PR_HALFLIFE2 (french)  11(0)
'C','U','R','R','E','N','T',		// L_PR_CURRENT (english)  7(0), 494
'A','C','T','U','A','L',		// L_PR_CURRENT (french)  6(0)
'C','A','L','I','B','R','A','T','I','O','N',		// L_PR_CALIBRATION (english)  11(0), 495
'C','A','L','I','B','R','A','C','I','Ó','N',		// L_PR_CALIBRATION (french)  12(0)
'U','S','E','R',' ','N','U','C','L','I','D','E','S',':',		// L_PR_USER_NUCLIDES (english)  14(0), 496
'U','S','U','A','R','I','O',' ','D','E',' ','N','U','C','L','E','I','D','O','S',':',		// L_PR_USER_NUCLIDES (french)  21(0)
'(','C','o','6','0',' ','H','I','G','H',')',		// L_PR_CO60_HIGH (english)  11(0), 497
'(','C','o','6','0',' ','A','L','T','O',')',		// L_PR_CO60_HIGH (french)  11(0)
'(','C','o','6','0',' ','L','O','W',' ',')',		// L_PR_CO60_LOW (english)  11(0), 498
'(','C','o','6','0',' ','B','A','J','O',' ',')',		// L_PR_CO60_LOW (french)  12(0)
'A','C','C','U','R','A','C','Y',' ','T','E','S','T',':',		// L_ACCURACYTEST (english)  14(0), 499
'P','R','U','E','B','A',' ','D','E',' ','E','X','A','C','T','I','T','U','D',':',		// L_ACCURACYTEST (french)  20(0)
'S','o','u','r','c','e',':',		// L_SOURCE (english)  7(0), 500
'F','u','e','n','t','e',':',		// L_SOURCE (french)  7(0)
'S','e','r','i','a','l',' ','#',':',		// L_SERIALNUM (english)  9(0), 501
'#',' ','d','e',' ','S','e','r','i','e',':',		// L_SERIALNUM (french)  11(0)
'S','t','a','n','d','.',' ','S','o','u','r','c','e','(','S',')',':',		// L_STANDSOURCE (english)  17(0), 502
'F','u','e','n','t','e',' ','E','s','t','á','n','d','a','r','(','S',')',':',		// L_STANDSOURCE (french)  20(0)
'M','e','a','s','u','r','e','d',' ','A','s',' ',' ','(','M',')',':',		// L_MEASUREDAS (english)  17(0), 503
'M','e','d','i','d','o',' ','c','o','m','o',' ','(','M',')',':',		// L_MEASUREDAS (french)  16(0)
'D','e','v','i','a','t','i','o','n','(','M','-','S',')','/','S',':',		// L_DEVIATIONMSS (english)  17(0), 504
'D','e','s','v','.',' ',' ',' ',' ','(','M','-','S',')','/','S',':',		// L_DEVIATIONMSS (french)  17(0)
'C','O','N','S','T','A','N','C','Y',' ','T','E','S','T',':',		// L_CONSTANCYTEST (english)  15(0), 505
'P','R','U','E','B','A',' ','D','E',' ','E','S','T','A','B','I','L','I','D','A','D',':',		// L_CONSTANCYTEST (french)  22(0)
'F','i','n','i','s','h','e','d',		// L_FINISHED (english)  8(0), 506
'T','e','r','m','i','n','a','d','o',		// L_FINISHED (french)  9(0)
'F','i','n','i','s','h','e','d','!','!','!',		// L_FINISHED2 (english)  11(0), 507
'T','e','r','m','i','n','a','d','o','!','!','!',		// L_FINISHED2 (french)  12(0)
'C','h','a','m','b','e','r',' ','V','o','l','t','s',		// L_CHAMBER_VOLTS (english)  13(0), 508
'V','o','l','t','i','o','s',' ','d','e',' ','l','a',' ','c','á','m','a','r','a',		// L_CHAMBER_VOLTS (french)  21(0)
'E','n','h','a','n','c','e','d',' ','T','e','s','t','s',		// L_ENHANCED_TESTS (english)  14(0), 509
'P','r','u','e','b','a','s',' ','M','e','j','o','r','a','d','a','s',		// L_ENHANCED_TESTS (french)  17(0)
'G','e','o','m','e','t','r','y',		// L_GEOMETRY (english)  8(0), 510
'G','e','o','m','e','t','r','í','a',		// L_GEOMETRY (french)  10(0)
'L','i','n','e','a','r','i','t','y',		// L_LINEARITY (english)  9(0), 511
'L','i','n','e','a','l','i','d','a','d',		// L_LINEARITY (french)  10(0)
'Q','C',		// L_QC (english)  2(0), 512
'Q','C',		// L_QC (french)  2(0)
'I','n','v','e','n','t','o','r','y',		// L_INVENTORY (english)  9(0), 513
'I','n','v','e','n','t','a','r','i','o',		// L_INVENTORY (french)  10(0)
'I','n','v','e','n','t','o','r','y',' ','L','i','s','t',		// L_INVENTORY_LIST (english)  14(0), 514
'L','i','s','t','a',' ','d','e',' ','I','n','v','e','n','t','a','r','i','o',		// L_INVENTORY_LIST (french)  19(0)
'S','t','u','d','y',		// L_STUDY (english)  5(0), 515
'E','s','t','u','d','i','o',		// L_STUDY (french)  7(0)
'L','o','t',		// L_LOT (english)  3(0), 516
'L','o','t','e',		// L_LOT (french)  4(0)
'D','a','t','e',		// L_DATE (english)  4(0), 517
'F','e','c','h','a',		// L_DATE (french)  5(0)
'I','D',		// L_ID (english)  2(0), 518
'I','D',		// L_ID (french)  2(0)
'C','o','n','c',		// L_CONC (english)  4(0), 519
'C','o','n','c',		// L_CONC (french)  4(0)
'A','d','d',' ','I','t','e','m',		// L_ADD_ITEM (english)  8(0), 520
'A','ñ','a','d','i','r',		// L_ADD_ITEM (french)  7(0)
'D','e','l','e','t','e',' ','A','l','l',		// L_DELETE_ALL (english)  10(0), 521
'B','o','r','r','e',' ','T','o','d','o',		// L_DELETE_ALL (french)  10(0)
'W','i','t','h','d','r','a','w',		// L_WITHDRAW (english)  8(0), 522
'E','x','t','r','a','e','r',		// L_WITHDRAW (french)  7(0)
'M','a','k','e',' ','K','i','t',		// L_MAKE_KIT (english)  8(0), 523
'K','i','t',		// L_MAKE_KIT (french)  3(0)
'D','e','l','e','t','e',' ','I','t','e','m',		// L_DELETE_ITEM (english)  11(0), 524
'B','o','r','r','e',' ','Í','t','e','m',		// L_DELETE_ITEM (french)  11(0)
'I','N','V','E','N','T','O','R','Y',		// L_PR_INVENTORY (english)  9(0), 525
'I','N','V','E','N','T','A','R','I','O',		// L_PR_INVENTORY (french)  10(0)
'L','o','t',' ','#',':',		// L_PR_LOT (english)  6(0), 526
'L','o','t','e','#',':',		// L_PR_LOT (french)  6(0)
'O','V','E','R',		// L_PR_OVER (english)  4(0), 527
'P','O','R',' ','A','R','R','I','B','A',		// L_PR_OVER (french)  10(0)
'A','d','d',' ','I','n','v','e','n','t','o','r','y',' ','I','t','e','m',		// L_ADD_INVENTORY_ITEM (english)  18(0), 528
'A','ñ','a','d','i','r',' ','E','l','e','m','e','n','t','o',' ','a','l',' ','I','n','v','e','n','t','a','r','i','o',		// L_ADD_INVENTORY_ITEM (french)  30(1)
'N','u','c','l','i','d','e',':',		// L_NUCLIDE_COLON (english)  8(0), 529
'N','u','c','l','e','i','d','o',':',		// L_NUCLIDE_COLON (french)  9(0)
'N','U','C','L','I','D','E',':',		// L_NUCLIDE_COLON2 (english)  8(0), 530
'N','U','C','L','E','I','D','O',':',		// L_NUCLIDE_COLON2 (french)  9(0)
'L','o','t',':',		// L_LOT_COLON (english)  4(0), 531
'L','o','t','e',':',		// L_LOT_COLON (french)  5(0)
'A','c','t','i','v','i','t','y',':',		// L_ACTIVITY_COLON (english)  9(0), 532
'A','c','t','i','v','i','d','a','d',':',		// L_ACTIVITY_COLON (french)  10(0)
'D','a','t','e',':',		// L_DATE_COLON (english)  5(0), 533
'F','e','c','h','a',':',		// L_DATE_COLON (french)  6(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','I','D',':',		// L_PLEASE_ENTER_ID (english)  16(0), 534
'F','a','v','o','r',' ','d','e',' ','i','n','t','r','o','d','u','c','i','r',' ','e','l',' ','I','D',':',		// L_PLEASE_ENTER_ID (french)  26(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','M','e','a','s','u','r','m','e','n','t',' ','T','i','m','e',':',		// L_PLEASE_ENTER_MEASUREMENT_TIME (english)  29(1), 535
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','T','i','e','m','p','o',' ','d','e',' ','M','e','d','i','c','i','ó','n',':',		// L_PLEASE_ENTER_MEASUREMENT_TIME (french)  40(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','L','o','t',':',		// L_PLEASE_ENTER_LOT (english)  17(0), 536
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','e','l',' ','L','o','t','e',':',		// L_PLEASE_ENTER_LOT (french)  28(1)
'P','l','e','a','s','e',' ','A','c','c','e','p','t',' ','A','c','t','i','v','i','t','y',':',		// L_PLEASE_ACCEPT_ACTIVITY (english)  23(0), 537
'F','a','v','o','r',' ','d','e',' ','a','c','e','p','t','a','r',' ','l','a',' ','a','c','t','i','v','i','d','a','d',':',		// L_PLEASE_ACCEPT_ACTIVITY (french)  30(1)
'S','u','b','m','i','t',		// L_SUBMIT (english)  6(0), 538
'S','o','m','e','t','e','r',		// L_SUBMIT (french)  7(0)
'N','o','n','e',		// L_NONE (english)  4(0), 539
'N','i','n','g','u','n','o',		// L_NONE (french)  7(0)
'B','o','n','e',		// L_BONE (english)  4(0), 540
'H','u','e','s','o',		// L_BONE (french)  5(0)
'L','u','n','g',		// L_LUNG (english)  4(0), 541
'P','u','l','m','ó','n',		// L_LUNG (french)  7(0)
'H','i','d','a',		// L_HIDA (english)  4(0), 542
'H','e','p','a','t','o','b','i','l','i','a','r',		// L_HIDA (french)  12(0)
'H','e','a','r','t',		// L_HEART (english)  5(0), 543
'C','o','r','a','z','ó','n',		// L_HEART (french)  8(0)
'R','e','n','a','l',		// L_RENAL (english)  5(0), 544
'R','e','n','a','l',		// L_RENAL (french)  5(0)
'L','i','v','e','r',		// L_LIVER (english)  5(0), 545
'H','í','g','a','d','o',		// L_LIVER (french)  7(0)
'B','r','a','i','n',		// L_BRAIN (english)  5(0), 546
'C','e','r','e','b','r','o',		// L_BRAIN (french)  7(0)
'L','y','m','p','h',		// L_LYMPH (english)  5(0), 547
'L','i','n','f','a',		// L_LYMPH (french)  5(0)
'S','T','U','D','Y',':',		// L_STUDY_LABEL (english)  6(0), 548
'E','S','T','U','D','I','O',':',		// L_STUDY_LABEL (french)  8(0)
'M','e','a','s','u','r','e',		// L_MEASURE (english)  7(0), 549
'M','e','d','i','r',		// L_MEASURE (french)  5(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','M','o','/','T','c',' ','R','a','t','i','o',':',		// L_PLEASE_ENTER_MOTC_RATIO (english)  25(0), 550
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','p','r','o','p','o','r','c','i','ó','n',' ','M','o','/','T','c',':',		// L_PLEASE_ENTER_MOTC_RATIO (french)  38(1)
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','s','t','u','d','y',		// L_PLEASE_SELECT_STUDY (english)  19(0), 551
'F','a','v','o','r',' ','d','e',' ','s','e','l','e','c','c','i','o','n','a','r',' ','e','l',' ','e','s','t','u','d','i','o',		// L_PLEASE_SELECT_STUDY (french)  31(1)
'1',')',' ','C','o','n','t','a','i','n','e','r',		// L_CONTAINER (english)  12(0), 552
'1',')',' ','R','e','c','i','p','i','e','n','t','e',		// L_CONTAINER (french)  13(0)
'2',')',' ','I','n','i','t','i','a','l',' ','V','o','l','u','m','e',		// L_INITIAL_VOLUME (english)  17(0), 553
'2',')',' ','V','o','l','u','m','e','n',' ','I','n','i','c','i','a','l',		// L_INITIAL_VOLUME (french)  18(0)
'I','n','i','t','i','a','l',' ','M','e','a','s','u','r','e','m','e','n','t',		// L_INITIAL_MEASUREMENT (english)  19(0), 554
'M','e','d','i','d','a',' ','I','n','i','c','i','a','l',		// L_INITIAL_MEASUREMENT (french)  14(0)
'4',')',' ','A','d','d','e','d',' ','V','o','l','u','m','e',		// L_ADDED_VOLUME (english)  15(0), 555
'4',')',' ','V','o','l','u','m','e','n',' ','A','g','r','e','g','a','d','o',		// L_ADDED_VOLUME (french)  19(0)
'4',')',' ','V','o','l','u','m','e',		// L_VOLUME2 (english)  9(0), 556
'4',')',' ','V','o','l','u','m','e','n',		// L_VOLUME2 (french)  10(0)
'M','e','a','s','u','r','e','m','e','n','t',		// L_MEASUREMENT (english)  11(0), 557
'M','e','d','i','d','a',		// L_MEASUREMENT (french)  6(0)
'C','A','L',' ','#',' ','N','O','T',' ','E','N','T','E','R','E','D',' ','F','O','R',' ','T','c','9','9','m',		// L_CAL_NOT_ENTERED_FOR_NUCLIDE (english)  27(1), 558
'C','A','L',' ','#',' ','N','O',' ','I','N','T','R','O','D','U','C','I','D','O',' ','p','a','r','a',' ','e','l',' ','N','U','C','L','E','I','D','O',		// L_CAL_NOT_ENTERED_FOR_NUCLIDE (french)  37(1)
'M','o','r','e',' ','M','e','a','s','u','r','e','m','e','n','t','s',		// L_MORE_MEASUREMENTS (english)  17(0), 559
'M','a','s',' ','M','e','d','i','d','a','s',		// L_MORE_MEASUREMENTS (french)  11(0)
'S','y','r','i','n','g','e',		// L_SYRINGE (english)  7(0), 560
'J','e','r','i','n','g','a',		// L_SYRINGE (french)  7(0)
'V','i','a','l',		// L_VIAL (english)  4(0), 561
'F','r','a','s','c','o',		// L_VIAL (french)  6(0)
'V','a','l','i','d',' ','r','a','n','g','e',' ','i','s',' ','9','8','.','9','m','l',' ','t','o',' ','0','.','1','m','l',		// L_VALID_RANGE_FOR_GEOMETRY (english)  30(1), 562
'E','l',' ','L','í','m','i','t','e',' ','v','á','l','i','d','o',' ','e','s',' ','d','e',' ','9','8','.','9','m','l',' ','a',' ','0','.','1','m','l',		// L_VALID_RANGE_FOR_GEOMETRY (french)  39(1)
'V','a','l','i','d',' ','r','a','n','g','e',' ','i','s',' ','%','.','3','f','m','l',' ','t','o',' ','0','.','1','m','l',		// L_VALID_RANGE_IS_ML_TO_ML (english)  30(1), 563
'E','l',' ','L','í','m','i','t','e',' ','v','á','l','i','d','o',' ','e','s',' ','d','e',' ','%','.','3','f','m','l',' ','a',' ','0','.','1','m','l',		// L_VALID_RANGE_IS_ML_TO_ML (french)  39(1)
'A','d','d','e','d',' ','V','o','l',':',' ','%','.','3','f','m','l','\n','T','o','t','a','l',' ','V','o','l',':',' ','%','.','3','f','m','l',		// L_ADDED_VOL_TOTAL_VOL (english)  35(1), 564
'V','o','l',' ','A','g','r','e','g','a','d','o',':',' ','%','.','3','f','m','l','\n','V','o','l',' ','T','o','t','a','l',':',' ','%','.','3','f','m','l',		// L_ADDED_VOL_TOTAL_VOL (french)  38(1)
'B','A','S','E',		// L_BASE (english)  4(0), 565
'B','A','S','E',		// L_BASE (french)  4(0)
'S','y','r','i','n','g','e',' ','A','s','s','a','y',		// L_SYRINGE_ASSAY (english)  13(0), 566
'E','n','s','a','y','o',' ','d','e',' ','l','a',' ','j','e','r','i','n','g','a',		// L_SYRINGE_ASSAY (french)  20(0)
'V','i','a','l',' ','A','s','s','a','y',		// L_VIAL_ASSAY (english)  10(0), 567
'E','n','s','a','y','o',' ','d','e','l',' ','f','r','a','s','c','o',		// L_VIAL_ASSAY (french)  17(0)
'G','e','o','m','e','t','r','y',' ','R','e','p','o','r','t',		// L_GEOMETRY_REPORT (english)  15(0), 568
'R','e','p','o','r','t','e',' ','d','e',' ','G','e','o','m','e','t','r','í','a',		// L_GEOMETRY_REPORT (french)  21(0)
'U','s','i','n','g',' ','V','o','l','u','m','e',		// L_USING_VOLUME (english)  12(0), 569
'V','o','l','u','m','e','n',' ','U','s','a','d','o',		// L_USING_VOLUME (french)  13(0)
'%',' ','V','a','r',		// L_PERCENT_VAR (english)  5(0), 570
'%',' ','d','e',' ','V','a','r','i','a','c','i','ó','n',		// L_PERCENT_VAR (french)  15(0)
'%',' ','V','a','r',':',		// L_PERCENT_VAR2 (english)  6(0), 571
'%',' ','d','e',' ','V','a','r','i','a','c','i','ó','n',':',		// L_PERCENT_VAR2 (french)  16(0)
'G','E','O','M','E','T','R','Y',' ','T','E','S','T',		// L_GEOMETRY_TEST (english)  13(0), 572
'P','R','U','E','B','A',' ','d','e',' ','G','E','O','M','E','T','R','I','A',		// L_GEOMETRY_TEST (french)  19(0)
'U','s','i','n','g',':',		// L_USING (english)  6(0), 573
'U','s','a','n','d','o',':',		// L_USING (french)  7(0)
'#',' ',' ',' ','V','o','l','u','m','e',' ',' ',' ',' ','A','s','s','a','y',' ',' ',' ',' ','V','a','r','i','a','t','i','o','n',		// L_VOLUME_ASSAY_VARIATION (english)  32(1), 574
'#',' ',' ',' ','V','o','l','u','m','e','n',' ',' ',' ','E','n','s','a','y','o',' ',' ',' ','V','a','r','i','a','c','i','ó','n',		// L_VOLUME_ASSAY_VARIATION (french)  33(1)
'S','t','a','n','d','a','r','d',',',' ','L','i','n','e','a','t','o','r',' ','a','n','d',' ','C','a','l','i','c','h','e','c','k',' ','a','r','e',' ','a','v','a','i','l','a','b','l','e',' ','i','n',' ','s','e','t','u','p',		// L_STD_LINEATOR_CALICHECK_AVAILABLE_IN_SETUP (english)  55(2), 575
'E','s','t','á','n','d','a','r',',',' ','L','i','n','e','a','t','o','r',' ','y',' ','C','a','l','i','c','h','e','c','k',' ','e','s','t','á','n',' ','d','i','s','p','o','n','i','b','l','e','s',' ','e','n',' ','l','a',' ','c','o','n','f','i','g','u','r','a','c','i','ó','n',		// L_STD_LINEATOR_CALICHECK_AVAILABLE_IN_SETUP (french)  71(2)
'A','u','t','o','L','i','n','e','a','r','i','t','y',		// L_AUTOLINEARITY (english)  13(0), 576
'A','u','t','o','L','i','n','e','a','l','i','d','a','d',		// L_AUTOLINEARITY (french)  14(0)
'S','t','a','n','d','a','r','d',		// L_STANDARD (english)  8(0), 577
'E','s','t','á','n','d','a','r',		// L_STANDARD (french)  9(0)
'L','i','n','e','a','t','o','r',		// L_LINEATOR (english)  8(0), 578
'L','i','n','e','a','t','o','r',		// L_LINEATOR (french)  8(0)
'C','a','l','i','c','h','e','c','k',		// L_CALICHECK (english)  9(0), 579
'C','a','l','i','c','h','e','c','k',		// L_CALICHECK (french)  9(0)
'S','e','l','e','c','t',' ','C','h','a','m','b','e','r',		// L_SELECT_CHAMBER (english)  14(0), 580
'S','e','l','e','c','c','i','o','n','e',' ','C','á','m','a','r','a',		// L_SELECT_CHAMBER (french)  18(0)
'D','e','l','e','t','e',' ','A','l','l',' ','I','n','v','e','n','t','o','r','y',		// L_DELETE_ALL_INVENTORY (english)  20(0), 581
'B','o','r','r','a','r',' ','T','o','d','o',' ','e','l',' ','I','n','v','e','n','t','a','r','i','o',		// L_DELETE_ALL_INVENTORY (french)  25(0)
'D','e','l','e','t','e',' ','A','l','l',' ','I','n','v','e','n','t','o','r','y','?',		// L_DELETE_ALL_INVENTORY_Q (english)  21(0), 582
'¿','B','o','r','r','a','r',' ','t','o','d','o',' ','I','n','v','e','n','t','a','r','i','o','?',		// L_DELETE_ALL_INVENTORY_Q (french)  25(0)
'T','h','e',' ','P','E','T',' ','C','h','a','m','b','e','r',' ','c','a','n',' ','n','o','t',' ','a','c','c','e','s','s',' ','i','n','v','e','n','t','o','r','y',' ','f','u','n','c','t','i','o','n','s',		// L_PET_CHAMBER_NO_INVENTORY (english)  50(1), 583
'L','a',' ','c','á','m','a','r','a',' ','P','E','T',' ','n','o',' ','p','u','e','d','e','n',' ','a','c','c','e','d','e','r',' ','a',' ','l','a','s',' ','f','u','n','c','i','o','n','e','s',' ','d','e','l',' ','i','n','v','e','n','t','a','r','i','o',		// L_PET_CHAMBER_NO_INVENTORY (french)  63(2)
'D','e','l','e','t','e',' ','I','n','v','e','n','t','o','r','y',		// L_DELETE_INVENTORY (english)  16(0), 584
'B','o','r','r','a','r',' ','I','n','v','e','n','t','a','r','i','o',		// L_DELETE_INVENTORY (french)  17(0)
'T','c',' ','L','e','v','e','l',' ','T','O','O',' ','L','O','W',		// L_TC_LEVEL_TOO_LOW (english)  16(0), 585
'N','i','v','e','l',' ','d','e',' ','T','c',' ','D','e','m','a','s','i','a','d','o',' ','B','a','j','o',		// L_TC_LEVEL_TOO_LOW (french)  26(1)
'W','i','t','h','d','r','a','w',' ','I','n','v','e','n','t','o','r','y',		// L_WITHDRAW_INVENTORY (english)  18(0), 586
'R','e','t','i','r','a','r',' ','d','e','l',' ','I','n','v','e','n','t','a','r','i','o',		// L_WITHDRAW_INVENTORY (french)  22(0)
'W','i','t','h','d','r','a','w',' ','A','c','t','i','v','i','t','y',':',		// L_WITHDRAW_ACTIVITY (english)  18(0), 587
'E','x','t','r','a','e','r',' ','A','c','t','i','v','i','d','a','d',':',		// L_WITHDRAW_ACTIVITY (french)  18(0)
'P','l','e','a','s','e',' ','w','i','t','h','d','r','a','w',':',' ','%','5','.','1','f',' ','m','l',		// L_PLEASE_WITHDRAW_ML (english)  25(0), 588
'F','a','v','o','r',' ','d','e',' ','E','x','t','r','a','e','r',':',' ','%','5','.','1','f',' ','m','l',		// L_PLEASE_WITHDRAW_ML (french)  26(1)
'a','n','d',' ','M','e','a','s','u','r','e',' ','A','c','t','i','v','i','t','y','.',		// L_AND_MEASURE_ACTIVITY (english)  21(0), 589
'Y',' ','M','e','d','i','r',' ','A','c','t','i','v','i','d','a','d','.',		// L_AND_MEASURE_ACTIVITY (french)  18(0)
'A','C','T','I','V','I','T','Y',' ','G','R','E','A','T','E','R',' ','T','H','A','N',' ','T','H','A','T',' ','I','N',' ','I','N','V','E','N','T','O','R','Y',		// L_ACTIVITY_GREATER_THAN_INVENTORY (english)  39(1), 590
'A','C','T','I','V','I','D','A','D',' ','M','A','Y','O','R',' ','Q','U','E',' ','L','A',' ','D','E',' ','I','N','V','E','N','T','A','R','I','O',		// L_ACTIVITY_GREATER_THAN_INVENTORY (french)  36(1)
'P','l','e','a','s','e',' ','M','e','a','s','u','r','e',' ','W','i','t','h','d','r','a','w','a','l',':',		// L_PLEASE_MEASURE_WITHDRAWAL (english)  26(1), 591
'F','a','v','o','r',' ','d','e',' ','M','e','d','i','r',' ','l','o',' ','E','x','t','r','a','i','d','o',':',		// L_PLEASE_MEASURE_WITHDRAWAL (french)  27(1)
'T','i','m','e',' ','o','f',' ','U','s','e',':',		// L_TIME_OF_USE (english)  12(0), 592
'T','i','e','m','p','o',' ','d','e',' ','u','s','o',':',		// L_TIME_OF_USE (french)  14(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','T','i','m','e',' ','O','f',' ','U','s','e',':',		// L_PLEASE_ENTER_TIME_OF_USE (english)  25(0), 593
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','e','l',' ','T','i','e','m','p','o',' ','d','e',' ','U','s','o',':',		// L_PLEASE_ENTER_TIME_OF_USE (french)  37(1)
'S','t','a','r','t',' ','T','e','s','t',		// L_START_TEST (english)  10(0), 594
'I','n','i','c','i','a','r',' ','P','r','u','e','b','a',		// L_START_TEST (french)  14(0)
'R','e','s','u','m','e',' ','T','e','s','t',		// L_RESUME_TEST (english)  11(0), 595
'R','e','a','n','u','d','a','r',' ','l','a',' ','P','r','u','e','b','a',		// L_RESUME_TEST (french)  18(0)
'R','e','p','o','r','t','s',		// L_REPORTS (english)  7(0), 596
'R','e','p','o','r','t','e','s',		// L_REPORTS (french)  8(0)
'E','n','t','e','r',' ','T','o','t','a','l',' ','T','i','m','e',' ','i','n',' ','h','o','u','r','s',':',		// L_ENTER_TOTAL_TIME_IN_HOURS (english)  26(1), 597
'I','n','t','r','o','d','u','z','c','a',' ','T','i','e','m','p','o',' ','T','o','t','a','l',' ','e','n',' ','h','o','r','a','s',':',		// L_ENTER_TOTAL_TIME_IN_HOURS (french)  33(1)
'I','n','a','c','t','i','v','e',':',		// L_INACTIVE (english)  9(0), 598
'I','n','a','c','t','i','v','o',':',		// L_INACTIVE (french)  9(0)
'S','t','a','r','t',		// L_START (english)  5(0), 599
'I','n','i','c','i','a','r',		// L_START (french)  7(0)
'D','a','t','e',' ','T','i','m','e',' ','/',' ','E','l','a','p','s','e','d',		// L_DATE_TIME_ELAPSED (english)  19(0), 600
'F','e','c','h','a',' ','H','o','r','a','/','T','r','a','n','s','c','u','r','r','i','d','o',		// L_DATE_TIME_ELAPSED (french)  23(0)
'P','r','e','d','i','c','t','e','d',		// L_PREDICTED (english)  9(0), 601
'P','r','e','v','i','s','t','o',		// L_PREDICTED (french)  8(0)
'P','r','e','d','i','c','t','e','d',':',		// L_PREDICTED2 (english)  10(0), 602
'P','r','e','v','i','s','t','o',':',		// L_PREDICTED2 (french)  9(0)
'A','b','o','r','t',		// L_ABORT (english)  5(0), 603
'C','a','n','c','e','l','a','r',		// L_ABORT (french)  8(0)
'I','n','a','c','t','i','v','a','t','e',		// L_INACTIVATE (english)  10(0), 604
'I','N','A','C','T','I','V','A','R',		// L_INACTIVATE (french)  9(0)
'S','a','v','e',		// L_SAVE (english)  4(0), 605
'G','u','a','r','d','a','r',		// L_SAVE (french)  7(0)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',' ','-',' ','C','h',':','%','d',',',' ','%','s',		// L_AUTOLINEARITY_TEST (english)  30(1), 606
'P','r','u','e','b','a',' ','d','e',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',' ','-',' ','C','h',':','%','d',',',' ','%','s',		// L_AUTOLINEARITY_TEST (french)  36(1)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',		// L_AUTOLINEARITY_TEST2 (english)  18(0), 607
'P','r','u','e','b','a',' ','d','e',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',		// L_AUTOLINEARITY_TEST2 (french)  24(0)
'%','d',' ','h','r','s',		// L_HRS (english)  6(0), 608
'%','d',' ','h','r','s',		// L_HRS (french)  6(0)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','R','e','p','o','r','t',',',' ','C','h',':','1',' ','(','%','s',')',		// L_AUTOLINEARITY_REPORT1 (english)  31(1), 609
'R','e','p','o','r','t','e',' ','d','e',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',',','C','h',':','1','(','%','s',')',		// L_AUTOLINEARITY_REPORT1 (french)  34(1)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','R','e','p','o','r','t',',',' ','C','h',':','2',' ','(','%','s',')',		// L_AUTOLINEARITY_REPORT2 (english)  31(1), 610
'R','e','p','o','r','t','e',' ','d','e',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',',','C','h',':','2','(','%','s',')',		// L_AUTOLINEARITY_REPORT2 (french)  34(1)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','R','e','p','o','r','t',',',' ','S','/','N',':',' ','%','s',		// L_AUTOLINEARITY_REPORT3 (english)  29(1), 611
'R','e','p','o','r','t','e',' ','d','e',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',',','S','/','N',':','%','s',		// L_AUTOLINEARITY_REPORT3 (french)  32(1)
'%','s','\n','E','l','a','p','s','e','d',':',' ','%','d',' ','m','i','n',		// L_ELAPSED (english)  18(0), 612
'%','s','\n','T','r','a','n','s','c','u','r','r','i','d','o',':',' ','%','d',' ','m','i','n',		// L_ELAPSED (french)  23(0)
'P','a','u','s','e','d',		// L_PAUSED (english)  6(0), 613
'E','n',' ','p','a','u','s','a',		// L_PAUSED (french)  8(0)
'P','o','w','e','r',' ','F','a','i','l',		// L_POWER_FAIL (english)  10(0), 614
'F','a','l','l','o',' ','d','e',' ','E','n','e','r','g','í','a',		// L_POWER_FAIL (french)  17(0)
'%','d',' ','o','f',' ','%','d',		// L_OF (english)  8(0), 615
'%','d',' ','d','e',' ','%','d',		// L_OF (french)  8(0)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','C','h','a','m','b','e','r',		// L_UNABLE_TO_FIND_CHAMBER (english)  22(0), 616
'I','n','c','a','p','a','z',' ','d','e',' ','e','n','c','o','n','t','r','a','r',' ','l','a',' ','C','á','m','a','r','a',		// L_UNABLE_TO_FIND_CHAMBER (french)  31(1)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','N','u','c','l','i','d','e',		// L_UNABLE_TO_FIND_NUCLIDE (english)  22(0), 617
'I','n','c','a','p','a','z',' ','d','e',' ','e','n','c','o','n','t','r','a','r',' ','e','l',' ','N','u','c','l','e','i','d','o',		// L_UNABLE_TO_FIND_NUCLIDE (french)  32(1)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','N','u','c','l','i','d','e',		// L_UNABLE_TO_FIND_NUCLIDE2 (english)  22(0), 618
'I','n','c','a','p','a','z',' ','d','e',' ','e','n','c','o','n','t','r','a','r',		// L_UNABLE_TO_FIND_NUCLIDE2 (french)  20(0)
'C','u','r','r','e','n','t',' ','N','u','c','l','i','d','e',' ','i','s',' ','i','n','c','o','r','r','e','c','t',		// L_CURRENT_NUCLIDE_IS_INCORRECT (english)  28(1), 619
'N','u','c','l','e','i','d','o',' ','a','c','t','u','a','l',' ','e','s',' ','i','n','c','o','r','r','e','c','t','o',		// L_CURRENT_NUCLIDE_IS_INCORRECT (french)  29(1)
'C','h','a','m','b','e','r',' ','S','/','N',' ','i','s',' ','i','n','c','o','r','r','e','c','t',		// L_CHAMBER_SN_IS_INCORRECT (english)  24(0), 620
'S','/','N',' ','d','e',' ','l','a',' ','C','á','m','a','r','a',' ','e','s',' ','i','n','c','o','r','r','e','c','t','o',		// L_CHAMBER_SN_IS_INCORRECT (french)  31(1)
'M','i','s','s','i','n','g',' ','N','u','c','l','i','d','e',		// L_MISSING_NUCLIDE (english)  15(0), 621
'F','a','l','t','a',' ','N','u','c','l','e','i','d','o',		// L_MISSING_NUCLIDE (french)  14(0)
'M','i','s','s','i','n','g',' ','I','n','t','e','r','v','a','l',		// L_MISSING_INTERVAL (english)  16(0), 622
'F','a','l','t','a',' ','I','n','t','e','r','v','a','l','o',		// L_MISSING_INTERVAL (french)  15(0)
'M','i','s','s','i','n','g',' ','T','o','t','a','l',' ','T','i','m','e',		// L_MISSING_TOTAL_TIME (english)  18(0), 623
'F','a','l','t','a',' ','T','i','e','m','p','o',' ','T','o','t','a','l',		// L_MISSING_TOTAL_TIME (french)  18(0)
'N','u','m','b','e','r',' ','o','f',' ','m','e','a','s','u','r','e','m','e','n','t','s',' ','h','a','s',' ','t','o',' ','b','e',' ','g','r','e','a','t','e','r',' ','t','h','a','n',' ','4',		// L_NUM_OF_MEAS_HAS_TO_BE_GREATER_THAN_4 (english)  47(1), 624
'N','ú','m','e','r','o',' ','d','e',' ','m','e','d','i','c','i','o','n','e','s',' ','t','i','e','n','e',' ','q','u','e',' ','s','e','r',' ','m','a','y','o','r',' ','q','u','e',' ','4',		// L_NUM_OF_MEAS_HAS_TO_BE_GREATER_THAN_4 (french)  47(1)
'T','y','p','e',':',		// L_TYPE (english)  5(0), 625
'T','i','p','o',':',		// L_TYPE (french)  5(0)
'S','t','a','r','t','e','d',':',		// L_STARTED (english)  8(0), 626
'I','n','i','c','i','a','d','o',':',		// L_STARTED (french)  9(0)
'C','a','n','\'','t',' ','b','e',' ','s','a','v','e','d','.','\n','L','e','s','s',' ','t','h','a','n',' ','5',' ','m','e','a','s','u','r','e','m','e','n','t','s',		// L_CANT_BE_SAVED_LESS_THAN_5_MEASUREMENTS (english)  40(1), 627
'N','o',' ','p','u','e','d','e',' ','s','e','r',' ','g','u','a','r','d','a','d','o','.','\n','M','e','n','o','s',' ','d','e',' ','5',' ','m','e','d','i','c','i','o','n','e','s',		// L_CANT_BE_SAVED_LESS_THAN_5_MEASUREMENTS (french)  44(1)
'E','l','a','p','s','e','d',' ','M','i','n','u','t','e','s',':',' ','%','d','\n','R','e','m','a','i','n','i','n','g',' ','M','i','n','u','t','e','s',':','%','d',		// L_ELAPSED_MINUTES_REMAINING_MINUTES (english)  40(1), 628
'M','i','n',' ','T','r','a','n','s','c','u','r','r','i','d','o','s',':','%','d','\n','M','i','n',' ','R','e','s','t','a','n','t','e','s',':',' ','%','d',		// L_ELAPSED_MINUTES_REMAINING_MINUTES (french)  38(1)
'S','e','a','r','c','h',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t','s',		// L_SEARCH_AUTOLINEARITY_TESTS (english)  26(1), 629
'B','u','s','c','a','r',' ','l','a','s',' ','P','r','u','e','b','a','s',' ','d','e',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',		// L_SEARCH_AUTOLINEARITY_TESTS (french)  36(1)
'E','n','t','e','r',' ','S','t','a','r','t',' ','D','a','t','e',':',		// L_ENTER_START_DATE (english)  17(0), 630
'I','n','t','r','o','d','u','z','c','a',' ','F','e','c','h','a',' ','d','e',' ','I','n','i','c','i','o',':',		// L_ENTER_START_DATE (french)  27(1)
'E','n','t','e','r',' ','E','n','d',' ','D','a','t','e',':',		// L_ENTER_END_DATE (english)  15(0), 631
'I','n','t','r','o','d','u','z','c','a',' ','F','e','c','h','a',' ','F','i','n','a','l',':',		// L_ENTER_END_DATE (french)  23(0)
'S','e','a','r','c','h',		// L_SEARCH (english)  6(0), 632
'B','u','s','c','a','r',		// L_SEARCH (french)  6(0)
'V','i','e','w',		// L_VIEW (english)  4(0), 633
'V','e','r',		// L_VIEW (french)  3(0)
'S','e','a','r','c','h',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','E','r','r','o','r',		// L_SEARCH_AUTOLINEARITY_ERROR (english)  26(1), 634
'B','u','s','c','a','r',' ','E','r','r','o','r',' ','A','u','t','o','L','i','n','e','a','l','i','d','a','d',		// L_SEARCH_AUTOLINEARITY_ERROR (french)  27(1)
'M','o','r','e',' ','t','h','a','n',' ','1','0','0',' ','i','t','e','m','s',' ','h','a','v','e',' ','b','e','e','n',' ','r','e','t','u','r','n','e','d','\n','P','l','e','a','s','e',' ','r','e','f','i','n','e',' ','c','r','i','t','e','r','i','a',		// L_MORE_THAN_100_ITEMS_RETURNED (english)  61(2), 635
'M','á','s',' ','d','e',' ','1','0','0',' ','e','l','e','m','e','n','t','o','s',' ','h','a','n',' ','s','i','d','o',' ','r','e','g','r','e','s','a','d','o','s','.',' ','F','a','v','o','r',' ','d','e',' ','r','e','v','i','s','a','r',' ','l','o','s',' ','c','r','i','t','e','r','i','o','s',		// L_MORE_THAN_100_ITEMS_RETURNED (french)  73(2)
'0',' ','i','t','e','m','s',' ','h','a','v','e',' ','b','e','e','n',' ','r','e','t','u','r','n','e','d',		// L_0_ITEMS_RETURNED (english)  26(1), 636
'0',' ','E','l','e','m','e','n','t','o','s',' ','h','a','n',' ','s','i','d','o',' ','r','e','g','r','e','s','a','d','o','s',		// L_0_ITEMS_RETURNED (french)  31(1)
'M','a','k','e',' ','I','n','v','e','n','t','o','r','y',' ','K','i','t',		// L_MAKE_INVENTORY_KIT (english)  18(0), 637
'P','r','e','p','a','r','a','r',' ','K','i','t',' ','d','e','l',' ','I','n','v','e','n','t','a','r','i','o',		// L_MAKE_INVENTORY_KIT (french)  27(1)
'S','o','u','r','c','e',' ','N','u','c','l','i','d','e',':',		// L_SOURCE_NUCLIDE (english)  15(0), 638
'F','u','e','n','t','e',' ','N','u','c','l','e','i','d','o',':',		// L_SOURCE_NUCLIDE (french)  16(0)
'S','o','u','r','c','e',' ','I','D',':',		// L_SOURCE_ID (english)  10(0), 639
'I','D',' ','d','e',' ','l','a',' ','F','u','e','n','t','e',':',		// L_SOURCE_ID (french)  16(0)
'S','o','u','r','c','e',' ','L','o','t',':',		// L_SOURCE_LOT (english)  11(0), 640
'L','o','t','e',' ','d','e',' ','l','a',' ','F','u','e','n','t','e',':',		// L_SOURCE_LOT (french)  18(0)
'S','o','u','r','c','e',' ','V','o','l','u','m','e',':',		// L_SOURCE_VOLUME (english)  14(0), 641
'V','o','l','u','m','e','n',' ','d','e',' ','l','a',' ','F','u','e','n','t','e',':',		// L_SOURCE_VOLUME (french)  21(0)
'S','o','u','r','c','e',' ','A','c','t','i','v','i','t','y',':',		// L_SOURCE_ACTIVITY (english)  16(0), 642
'A','c','t','.',' ','d','e',' ','l','a',' ','F','u','e','n','t','e',':',		// L_SOURCE_ACTIVITY (french)  18(0)
'S','o','u','r','c','e',' ','D','a','t','e',':',		// L_SOURCE_DATE (english)  12(0), 643
'F','e','c','h','a',' ','d','e',' ','l','a',' ','F','u','e','n','t','e',':',		// L_SOURCE_DATE (french)  19(0)
'K','i','t',' ','S','t','u','d','y',':',		// L_KIT_STUDY (english)  10(0), 644
'E','s','t','u','d','i','o',' ','d','e','l',' ','K','i','t',':',		// L_KIT_STUDY (french)  16(0)
'K','i','t',' ','I','D',':',		// L_KIT_ID (english)  7(0), 645
'I','d','e','n','t','i','f','i','c','a','c','i','ó','n',' ','d','e','l',' ','K','i','t',':',		// L_KIT_ID (french)  24(0)
'K','i','t',' ','L','o','t',':',		// L_KIT_LOT (english)  8(0), 646
'L','o','t','e',' ','d','e','l',' ','K','i','t',':',		// L_KIT_LOT (french)  13(0)
'K','i','t',' ','A','c','t','i','v','i','t','y',':',		// L_KIT_ACTIVITY (english)  13(0), 647
'A','c','t','i','v','i','d','a','d',' ','d','e','l',' ','K','i','t',':',		// L_KIT_ACTIVITY (french)  18(0)
'K','i','t',' ','V','o','l','u','m','e',':',		// L_KIT_VOLUME (english)  11(0), 648
'V','o','l','u','m','e','n',' ','d','e','l',' ','K','i','t',':',		// L_KIT_VOLUME (french)  16(0)
'K','I','T',' ','A','C','T','I','V','I','T','Y',' ','G','R','E','A','T','E','R',' ','T','H','A','N',' ','T','H','A','T',		// L_KIT_ACTIVITY_GREATER (english)  30(1), 649
'A','C','T','I','V','I','D','A','D',' ','D','E','L',' ','K','I','T',' ','M','A','Y','O','R',' ','Q','U','E',		// L_KIT_ACTIVITY_GREATER (french)  27(1)
'I','N',' ','S','O','U','R','C','E',		// L_IN_SOURCE (english)  9(0), 650
'E','N',' ','L','A',' ','F','U','E','N','T','E',		// L_IN_SOURCE (french)  12(0)
'K','I','T',' ','V','O','L','U','M','E',' ','L','E','S','S',' ','T','H','A','N',' ','T','H','E',		// L_KIT_VOLUME_LESS (english)  24(0), 651
'%','.','3','f',' ','m','l',' ','d','e',' ','l','a',' ','f','u','e','n','t','e',' ','m','a','y','o','r',' ','q','u','e',		// L_KIT_VOLUME_LESS (french)  30(1)
'%','.','3','f',' ','m','l',' ','D','R','A','W',' ','F','R','O','M',' ','S','O','U','R','C','E',		// L_DRAW_FROM_SOURCE (english)  24(0), 652
'e','l',' ','v','o','l','u','m','e','n',' ','q','u','e',' ','e','l',' ','k','i','t',' ','r','e','q','u','i','e','r','e','.',		// L_DRAW_FROM_SOURCE (french)  31(1)
'P','l','e','a','s','e',' ','w','i','t','h','d','r','a','w',':',' ','%','5','.','1','f',' ','m','l',',','\n',' ','a','n','d',' ','M','e','a','s','u','r','e',' ','A','c','t','i','v','i','t','y','.',		// L_PLEASE_WITHDRAW_AND_MEASURE_ACTIVITY (english)  49(1), 653
'F','a','v','o','r',' ','d','e',' ','e','x','t','r','a','e','r',':',' ','%','5','.','1','f',' ','m','l',',','\n',' ','y',' ','M','i','d','a',' ','A','c','t','i','v','i','d','a','d','.',		// L_PLEASE_WITHDRAW_AND_MEASURE_ACTIVITY (french)  46(1)
'C','A','P','M','A','C',' ','f','o','r',' ','M','a','l','l','i','n','c','k','r','o','d','t',' ','G','e','n',		// L_CAPMAC_FOR_MAL_GEN (english)  27(1), 654
'C','A','P','M','A','C',' ','p','a','r','a',' ','M','a','l','l','i','n','c','k','r','o','d','t',' ','G','e','n',		// L_CAPMAC_FOR_MAL_GEN (french)  28(1)
'C','A','P','M','A','C',' ','f','o','r',' ','B','r','i','s','t','o','l',' ','M','y','e','r','s',' ','G','e','n',		// L_CAPMAC_FOR_BM_GEN (english)  28(1), 655
'C','A','P','M','A','C',' ','p','a','r','a',' ','B','r','i','s','t','o','l',' ','M','y','e','r','s',' ','G','e','n',		// L_CAPMAC_FOR_BM_GEN (french)  29(1)
'C','a','p','i','n','t','e','c',' ','C','a','n','i','s','t','e','r',		// L_CAPINTEC_CANISTER (english)  17(0), 656
'C','a','p','i','n','t','e','c',' ','F','r','a','s','c','o',		// L_CAPINTEC_CANISTER (french)  15(0)
'M','o','l','y',' ','A','s','s','a','y',		// L_MOLY_ASSAY (english)  10(0), 657
'E','n','s','a','y','o',' ','d','e',' ','M','o','l','y',		// L_MOLY_ASSAY (french)  14(0)
'M','o','l','y',' ','A','s','s','a','y',' ','-',' ','M','a','l','l','i','n','c','k','r','o','d','t',' ','G','e','n',' ','M','e','t','h','o','d',		// L_MOLY_ASSAY_MAL_GEN_METHOD (english)  36(1), 658
'E','n','s','a','y','o',' ','d','e',' ','M','o','l','y',' ','-',' ','M','é','t','o','d','o',' ','M','a','l','l','i','n','c','k','r','o','d','t',' ','G','e','n',		// L_MOLY_ASSAY_MAL_GEN_METHOD (french)  41(1)
'M','o','l','y',' ','A','s','s','a','y',' ','-',' ','B','r','i','s','t','o','l',' ','M','y','e','r','s',' ','G','e','n',' ','M','e','t','h','o','d',		// L_MOLY_ASSAY_BM_GEN_METHOD (english)  37(1), 659
'E','n','s','a','y','o',' ','d','e',' ','M','o','l','y',' ','-',' ','M','é','t','o','d','o',' ','B','r','i','s','t','o','l',' ','M','y','e','r','s',' ','G','e','n',		// L_MOLY_ASSAY_BM_GEN_METHOD (french)  42(1)
'M','o','l','y',' ','A','s','s','a','y',' ','-',' ','C','a','p','i','n','t','e','c',' ','C','a','n','i','s','t','e','r',' ','M','e','t','h','o','d',		// L_MOLY_ASSAY_CAPINTEC_CANISTER_METHOD (english)  37(1), 660
'E','n','s','a','y','o',' ','d','e',' ','M','o','l','y',' ','-',' ','M','é','t','o','d','o',' ','C','a','p','i','n','t','e','c',' ','F','r','a','s','c','o',		// L_MOLY_ASSAY_CAPINTEC_CANISTER_METHOD (french)  40(1)
'M','e','a','s','u','r','e',' ','B','k','g',		// L_MEASURE_BKG (english)  11(0), 661
'M','e','d','i','r',' ','e','l',' ','F','o','n','d','o',		// L_MEASURE_BKG (french)  14(0)
'S','k','i','p',' ','B','k','g',		// L_SKIP_BKG (english)  8(0), 662
'O','m','i','t','i','r',' ','e','l',' ','F','o','n','d','o',		// L_SKIP_BKG (french)  15(0)
'1',')',' ','M','e','a','s','u','r','e',' ','C','a','n','i','s','t','e','r',' ','B','k','g',':',		// L_MEASURE_CANISTER_BKG (english)  24(0), 663
'1',')',' ','M','i','d','i','r',' ','F','o','n','d','o',' ','d','e',' ','F','r','a','s','c','o',':',		// L_MEASURE_CANISTER_BKG (french)  25(0)
'1',')',' ','M','e','a','s','u','r','e',' ','C','A','P','M','A','C',' ','B','k','g',':',		// L_MEASURE_CAPMAC_BKG (english)  22(0), 664
'1',')',' ','M','e','d','i','r',' ','F','o','n','d','o',' ','d','e',' ','C','A','P','M','A','C',':',		// L_MEASURE_CAPMAC_BKG (french)  25(0)
'S','k','i','p','p','e','d',		// L_SKIPPED (english)  7(0), 665
'O','m','i','t','i','d','o',		// L_SKIPPED (french)  7(0)
'2',')',' ','E','l','u','a','t','e',' ','i','n',' ','C','A','N','I','S','T','E','R',':',' ',		// L_ELUATE_IN_CANISTER (english)  23(0), 666
'2',')',' ','E','l','u','i','d','o',' ','e','n',' ','F','R','A','S','C','O',':',' ',' ',' ',		// L_ELUATE_IN_CANISTER (french)  23(0)
'2',')',' ','E','l','u','a','t','e',' ','i','n',' ','C','A','P','M','A','C',':',' ',		// L_ELUATE_IN_CAPMAC (english)  21(0), 667
'2',')',' ','E','l','u','i','d','o',' ','e','n',' ','C','A','P','M','A','C',':',' ',		// L_ELUATE_IN_CAPMAC (french)  21(0)
'3',')',' ','M','e','a','s','u','r','e',' ','T','c','9','9','m',' ','A','s','s','a','y',':',		// L_MEASURE_TC_ASSAY (english)  23(0), 668
'3',')',' ','M','e','d','i','r',' ','E','n','s','a','y','o',' ','d','e',' ','T','c','9','9','m',':',		// L_MEASURE_TC_ASSAY (french)  25(0)
'A','c','t','i','v','i','t','y',' ','T','o','o',' ','L','o','w',' ','E','r','r','o','r',		// L_ACTIVITY_TOO_LOW_ERROR (english)  22(0), 669
'E','r','r','o','r',':',' ','A','c','t','i','v','i','d','a','d',' ','M','u','y',' ','B','a','j','a',		// L_ACTIVITY_TOO_LOW_ERROR (french)  25(0)
'C','A','U','T','I','O','N',		// L_CAUTION (english)  7(0), 670
'P','R','E','C','A','U','C','I','Ó','N',		// L_CAUTION (french)  11(0)
'D','O',' ','N','O','T',' ','U','S','E',' ','A','F','T','E','R',' ','%','d',' ','H','O','U','R','S',		// L_DO_NOT_USE_AFTER_HOURS (english)  25(0), 671
'N','O',' ','U','T','I','L','I','C','E',' ','D','E','S','P','U','É','S',' ','D','E',' ','%','d',' ','H','R','S',		// L_DO_NOT_USE_AFTER_HOURS (french)  29(1)
'M','O',' ','T','O','O',' ','H','I','G','H',		// L_MO_TOO_HIGH (english)  11(0), 672
'M','O',' ','D','E','M','A','S','I','A','D','O',' ','A','L','T','O',		// L_MO_TOO_HIGH (french)  17(0)
'D','O',' ','N','O','T',' ','U','S','E',		// L_DO_NOT_USE (english)  10(0), 673
'N','O',' ','U','T','I','L','I','C','E',		// L_DO_NOT_USE (french)  10(0)
'M','O',' ','H','I','G','H',' ','T','e','r','m','i','n','a','t','e','?',		// L_MO_HIGH_TERMINATE_QUESTION (english)  18(0), 674
'M','o','l','y',' ','A','L','T','A',' ','¿','T','e','r','m','i','n','a','r','?',		// L_MO_HIGH_TERMINATE_QUESTION (french)  21(0)
'N','E','G','A','T','I','V','E',' ','A','C','T','I','V','I','T','Y',		// L_NEGATIVE_ACTIVITY (english)  17(0), 675
'A','C','T','I','V','I','D','A','D',' ','N','E','G','A','T','I','V','A',		// L_NEGATIVE_ACTIVITY (french)  18(0)
'M','O',' ','H','I','G','H',		// L_MO_HIGH (english)  7(0), 676
'M','O',' ','A','L','T','O',		// L_MO_HIGH (french)  7(0)
'E','x','p','i','r','e','s',		// L_EXPIRES (english)  7(0), 677
'E','x','p','i','r','a',		// L_EXPIRES (french)  6(0)
'M','o','l','y',' ','B','k','g',		// L_MOLY_BKG (english)  8(0), 678
'F','o','n','d','o',' ','d','e',' ','M','o','l','y',		// L_MOLY_BKG (french)  13(0)
'M','O',' ','T','O','O',' ','H','I','G','H',' ','-',' ','D','O',' ','N','O','T',' ','U','S','E',		// L_MO_TOO_HIGH_DO_NOT_USE (english)  24(0), 679
'M','O',' ','D','E','M','A','S','I','A','D','O',' ','A','L','T','O',' ','-',' ','N','O',' ','U','T','I','L','I','C','E',		// L_MO_TOO_HIGH_DO_NOT_USE (french)  30(1)
'S','i','n','g','l','e',' ','S','t','r','i','p',' ','T','e','s','t',		// L_SINGLE_STRIP_TEST (english)  17(0), 680
'P','r','u','e','b','a',' ','d','e',' ','T','i','r','a',' ','Ú','n','i','c','a',		// L_SINGLE_STRIP_TEST (french)  21(0)
'T','w','o',' ','S','t','r','i','p',' ','T','e','s','t',		// L_TWO_STRIP_TEST (english)  14(0), 681
'P','r','u','e','b','a',' ','d','e',' ','D','o','b','l','e',' ','T','i','r','a',		// L_TWO_STRIP_TEST (french)  20(0)
'H','M','P','A','O',' ','T','e','s','t',		// L_HMPAO_TEST (english)  10(0), 682
'P','r','u','e','b','a',' ','d','e',' ','H','M','P','A','O',		// L_HMPAO_TEST (french)  15(0)
'M','A','G','3',' ','T','e','s','t',		// L_MAG3_TEST (english)  9(0), 683
'P','r','u','e','b','a',' ','d','e',' ','M','A','G','3',		// L_MAG3_TEST (french)  14(0)
'N','o',' ','d','e','t','e','c','t','o','r','s',' ','f','o','u','n','d','\n','N','o',' ','C','h','a','m','b','e','r',' ','a','t','t','a','c','h','e','d','\n','N','o',' ','W','e','l','l',' ','a','t','t','a','c','h','e','d',		// L_NONE_FOUND (english)  55(2), 684
'N','i','n','g','ú','n',' ','d','e','t','e','c','t','o','r',' ','e','n','c','o','n','t','r','a','d','o','\n','N','i','n','g','u','n','a',' ','C','á','m','a','r','a',' ','c','o','n','e','c','t','a','d','a','\n','N','i','n','g','ú','n',' ','c','o','n','t','a','d','o','r',' ','d','e',' ','p','o','z','o',' ','e','s','t','a',' ','a','d','j','u','n','t','o',		// L_NONE_FOUND (french)  91(3)
'Q','C',' ','-',' ','O','n','e',' ','S','t','r','i','p',' ','T','e','s','t',		// L_QC_ONE_STRIP_TEST (english)  19(0), 685
'Q','C',' ','-',' ','P','r','u','e','b','a',' ','d','e',' ','U','n','a',' ','T','i','r','a',		// L_QC_ONE_STRIP_TEST (french)  23(0)
'1',')',' ','T','o','p',' ','o','f',' ','s','t','r','i','p',		// L_TOP_OF_STRIP (english)  15(0), 686
'1',')',' ','T','i','r','a',' ','p','a','r','t','e',' ','S','u','p','e','r','i','o','r',		// L_TOP_OF_STRIP (french)  22(0)
'T','o','p',' ','o','f',' ','S','t','r','i','p',':',		// L_TOP_OF_STRIP2 (english)  13(0), 687
'T','i','r','a',' ','p','a','r','t','e',' ','S','u','p','e','r','i','o','r',':',		// L_TOP_OF_STRIP2 (french)  20(0)
'2',')',' ','B','o','t','t','o','m',' ','o','f',' ','s','t','r','i','p',		// L_BOTTOM_OF_STRIP (english)  18(0), 688
'2',')',' ','T','i','r','a',' ','p','a','r','t','e',' ','I','n','f','e','r','i','o','r',		// L_BOTTOM_OF_STRIP (french)  22(0)
'B','o','t','t','o','m',' ','o','f',' ','S','t','r','i','p',':',		// L_BOTTOM_OF_STRIP2 (english)  16(0), 689
'T','i','r','a',' ','p','a','r','t','e',' ','I','n','f','e','r','i','o','r',':',		// L_BOTTOM_OF_STRIP2 (french)  20(0)
'O','n','e',' ','S','t','r','i','p',		// L_ONE_STRIP (english)  9(0), 690
'U','n','a',' ','T','i','r','a',		// L_ONE_STRIP (french)  8(0)
'Q','C',' ','-',' ','T','w','o',' ','S','t','r','i','p',' ','T','e','s','t',		// L_QC_TWO_STRIP_TEST (english)  19(0), 691
'Q','C',' ','-',' ','P','r','u','e','b','a',' ','d','e',' ','D','o','b','l','e',' ','T','i','r','a',		// L_QC_TWO_STRIP_TEST (french)  25(0)
'1',')',' ','T','o','p',' ','o','f',' ','s','t','r','i','p',' ','A',		// L_TOP_OF_STRIP_A (english)  17(0), 692
'1',')',' ','P','a','r','t','e',' ','S','u','p','.',' ','T','i','r','a',' ','A',		// L_TOP_OF_STRIP_A (french)  20(0)
'2',')',' ','B','o','t','t','o','m',' ','o','f',' ','s','t','r','i','p',' ','A',		// L_BOTTOM_OF_STRIP_A (english)  20(0), 693
'2',')',' ','P','a','r','t','e',' ','I','n','f','.',' ','T','i','r','a',' ','A',		// L_BOTTOM_OF_STRIP_A (french)  20(0)
'3',')',' ','T','o','p',' ','o','f',' ','s','t','r','i','p',' ','B',		// L_TOP_OF_STRIP_B (english)  17(0), 694
'3',')',' ','P','a','r','t','e',' ','S','u','p','.',' ','T','i','r','a',' ','B',		// L_TOP_OF_STRIP_B (french)  20(0)
'4',')',' ','B','o','t','t','o','m',' ','o','f',' ','s','t','r','i','p',' ','B',		// L_BOTTOM_OF_STRIP_B (english)  20(0), 695
'4',')',' ','P','a','r','t','e',' ','I','n','f','.',' ','T','i','r','a',' ','B',		// L_BOTTOM_OF_STRIP_B (french)  20(0)
'T','o','p',' ','o','f',' ','S','t','r','i','p',' ','A',':',		// L_TOP_OF_STRIP_A2 (english)  15(0), 696
'P','a','r','t','e',' ','S','u','p','.',' ','d','e',' ','T','i','r','a',' ','A',		// L_TOP_OF_STRIP_A2 (french)  20(0)
'B','o','t','t','o','m',' ','o','f',' ','S','t','r','i','p',' ','A',':',		// L_BOTTOM_OF_STRIP_A2 (english)  18(0), 697
'P','a','r','t','e',' ','I','n','f','.',' ','d','e',' ','T','i','r','a',' ','A',		// L_BOTTOM_OF_STRIP_A2 (french)  20(0)
'T','o','p',' ','o','f',' ','S','t','r','i','p',' ','B',':',		// L_TOP_OF_STRIP_B2 (english)  15(0), 698
'P','a','r','t','e',' ','S','u','p','.',' ','d','e',' ','T','i','r','a',' ','B',		// L_TOP_OF_STRIP_B2 (french)  20(0)
'B','o','t','t','o','m',' ','o','f',' ','S','t','r','i','p',' ','B',':',		// L_BOTTOM_OF_STRIP_B2 (english)  18(0), 699
'P','a','r','t','e',' ','I','n','f','.',' ','d','e',' ','T','i','r','a',' ','B',		// L_BOTTOM_OF_STRIP_B2 (french)  20(0)
'5',')',' ','T','o','p',' ','o','f',' ','s','t','r','i','p',' ','C',		// L_TOP_OF_STRIP_C (english)  17(0), 700
'5',')',' ','P','a','r','t','e',' ','S','u','p','.',' ','T','i','r','a',' ','C',		// L_TOP_OF_STRIP_C (french)  20(0)
'6',')',' ','B','o','t','t','o','m',' ','o','f',' ','s','t','r','i','p',' ','C',		// L_BOTTOM_OF_STRIP_C (english)  20(0), 701
'6',')',' ','P','a','r','t','e',' ','I','n','f','.',' ','T','i','r','a',' ','C',		// L_BOTTOM_OF_STRIP_C (french)  20(0)
'T','o','p',' ','o','f',' ','S','t','r','i','p',' ','C',':',		// L_TOP_OF_STRIP_C2 (english)  15(0), 702
'P','a','r','t','e',' ','S','u','p','.',' ','d','e',' ','T','i','r','a',' ','C',		// L_TOP_OF_STRIP_C2 (french)  20(0)
'B','o','t','t','o','m',' ','o','f',' ','S','t','r','i','p',' ','C',':',		// L_BOTTOM_OF_STRIP_C2 (english)  18(0), 703
'P','a','r','t','e',' ','I','n','f','.',' ','d','e',' ','T','i','r','a',' ','C',		// L_BOTTOM_OF_STRIP_C2 (french)  20(0)
'F','r','e','e',' ','T','c','9','9','m',' ',':',' ','%','5','.','1','f',		// L_FREE (english)  18(0), 704
'T','c','9','9','m',' ','L','i','b','r','e',' ',':',' ','%','5','.','1','f',		// L_FREE (french)  19(0)
'R','e','d','u','c','e','d','/','H','y','d','r','o','l','i','z','e','d',':',' ','%','5','.','1','f',		// L_REDUCED_HYDROLIZED (english)  25(0), 705
'R','e','d','u','c','i','d','o','/','H','i','d','r','o','l','i','z','a','d','o',':',' ','%','5','.','1','f',		// L_REDUCED_HYDROLIZED (french)  27(1)
'H','y','d','r','o','l','i','z','e','d','/','R','e','d','u','c','e','d',':',' ','%','5','.','1','f',		// L_HYDROLIZED_REDUCED (english)  25(0), 706
'H','i','d','r','o','l','i','z','a','d','o','/','R','e','d','u','c','i','d','o',':',' ','%','5','.','1','f',		// L_HYDROLIZED_REDUCED (french)  27(1)
'P','u','r','i','t','y',':',' ','%','5','.','1','f',		// L_PURITY (english)  13(0), 707
'P','u','r','e','z','a',':',' ','%','5','.','1','f',		// L_PURITY (french)  13(0)
' ',' ',' ',' ','T','o','p','/','(','T','o','p','+','B','o','t','t','o','m',')',' ','=',' ','%','5','.','1','f',		// L_TOP_TOP_BOTTOM_EQUALS (english)  28(1), 708
'S','u','p','e','r','i','o','r','/','(','S','u','p','e','r','i','o','r','+','I','n','f','e','r','i','o','r',')',' ','=',' ','%','5','.','1','f',		// L_TOP_TOP_BOTTOM_EQUALS (french)  36(1)
'B','o','t','t','o','m','/','(','T','o','p','+','B','o','t','t','o','m',')',' ','=',' ','%','5','.','1','f',		// L_BOTTOM_TOP_BOTTOM_EQUALS (english)  27(1), 709
'I','n','f','e','r','i','o','r','/','(','S','u','p','e','r','i','o','r','+','I','n','f','e','r','i','o','r',')',' ','=',' ','%','5','.','1','f',		// L_BOTTOM_TOP_BOTTOM_EQUALS (french)  36(1)
'Q','U','A','L','I','T','Y',' ','C','O','N','T','R','O','L',' ','T','E','S','T',		// L_QUALITY_CONTROL_TEST (english)  20(0), 710
'P','R','U','E','B','A',' ','D','E',' ','C','O','N','T','R','O','L',' ','D','E',' ','C','A','L','I','D','A','D',		// L_QUALITY_CONTROL_TEST (french)  28(1)
'S','i','n','g','l','e',' ','S','t','r','i','p',' ','/',' ','S','i','n','g','l','e',' ','S','o','l','v','e','n','t',		// L_SINGLE_STRIP_SINGLE_SOLVENT (english)  29(1), 711
'T','i','r','a',' ','Ú','n','i','c','a',' ','/',' ','S','o','l','v','e','n','t','e',' ','Ú','n','i','c','a',		// L_SINGLE_STRIP_SINGLE_SOLVENT (french)  29(1)
'M','e','a','s','u','r','e','d',' ','V','a','l','u','e','s',		// L_MEASURED_VALUES (english)  15(0), 712
'V','a','l','o','r','e','s',' ','M','e','d','i','d','o','s',		// L_MEASURED_VALUES (french)  15(0)
'R','e','s','u','l','t','s',		// L_RESULTS (english)  7(0), 713
'R','e','s','u','l','t','a','d','o','s',		// L_RESULTS (french)  10(0)
'R','a','d','i','o','p','h','a','r','m','a','c','e','u','t','i','c','a','l',':',' ','_','_','_','_','_','_','_','_','_',		// L_QC_RADIOPHARMACEUTICAL (english)  30(1), 714
'R','a','d','i','o','f','á','r','m','a','c','o',':',' ','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',		// L_QC_RADIOPHARMACEUTICAL (french)  31(1)
'L','o','t',' ','#',':',' ','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',		// L_QC_LOT (english)  30(1), 715
'L','o','t','e','#',':',' ','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',		// L_QC_LOT (french)  30(1)
'K','i','t',' ','#',':',' ','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',		// L_QC_KIT (english)  30(1), 716
'K','i','t',' ','#',':',' ','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',		// L_QC_KIT (french)  30(1)
'T','o','p',' ',' ',' ',' ','/',' ','(','T','o','p',' ','+',' ','B','o','t','t','o','m',')',':',' ',		// L_TOP_TOP_BOTTOM (english)  25(0), 717
'S','u','p','e','r','i','o','r',' ','/',' ',' ','(','S','u','p','.','+','I','n','f','.',')',':',' ',		// L_TOP_TOP_BOTTOM (french)  25(0)
'B','o','t','t','o','m',' ','/',' ','(','T','o','p',' ','+',' ','B','o','t','t','o','m',')',':',' ',		// L_BOTTOM_TOP_BOTTOM (english)  25(0), 718
'I','n','f','e','r','i','o','r',' ','/',' ',' ','(','S','u','p','.','+','I','n','f','.',')',':',' ',		// L_BOTTOM_TOP_BOTTOM (french)  25(0)
'T','w','o',' ','S','t','r','i','p',		// L_TWO_STRIP (english)  9(0), 719
'D','o','b','l','e',' ','T','i','r','a',		// L_TWO_STRIP (french)  10(0)
'T','w','o',' ','S','t','r','i','p','s',' ','/',' ','T','w','o',' ','S','o','l','v','e','n','t','s',		// L_TWO_STRIPS_TWO_SOLVENTS (english)  25(0), 720
'D','o','b','l','e',' ','T','i','r','a',' ','/','D','o','s',' ','S','o','l','v','e','n','t','e','s',		// L_TWO_STRIPS_TWO_SOLVENTS (french)  25(0)
'S','t','r','i','p',' ','A',':',' ','F','r','e','e',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','=',' ',		// L_STRIP_A_FREE (english)  25(0), 721
'T','i','r','a',' ',' ','A',':',' ','L','i','b','r','e',' ',' ',' ',' ',' ',' ',' ',' ',' ','=',' ',		// L_STRIP_A_FREE (french)  25(0)
'S','t','r','i','p',' ','B',':',' ','R',' ','/',' ','H',' ',' ',' ',' ',' ',' ',' ',' ',' ','=',' ',		// L_STRIP_B_R_H (english)  25(0), 722
'T','i','r','a',' ',' ','B',':',' ','R',' ','/',' ','H',' ',' ',' ',' ',' ',' ',' ',' ',' ','=',' ',		// L_STRIP_B_R_H (french)  25(0)
'P','u','r','i','t','y',':',' ','1','0','0',' ','-','(','F',' ','+',' ','R','/','H',')',' ','=',' ',		// L_PURITY_F_R_H (english)  25(0), 723
'P','u','r','e','z','a',':',' ','1','0','0',' ','-','(','L',' ','+',' ','R','/','H',')',' ','=',' ',		// L_PURITY_F_R_H (french)  25(0)
'F',' ','=',' ','F','r','e','e',		// L_F_FREE (english)  8(0), 724
'L',' ','=',' ','L','i','b','r','e',		// L_F_FREE (french)  9(0)
'R',' ','=',' ','R','e','d','u','c','e','d',		// L_R_REDUCED (english)  11(0), 725
'R',' ','=',' ','R','e','d','u','c','i','d','o',		// L_R_REDUCED (french)  12(0)
'H',' ','=',' ','H','y','d','r','o','l','i','z','e','d',		// L_H_HYDROLIZED (english)  14(0), 726
'H',' ','=',' ','H','i','d','r','o','l','i','z','a','d','o',		// L_H_HYDROLIZED (french)  15(0)
'F','r','e','e',' ','T','c','9','9','m',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_FREE_TC99M (english)  25(0), 727
'T','c','9','9','m',' ','L','i','b','r','e',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_FREE_TC99M (french)  25(0)
'H','y','d','r','o','l','i','z','e','d','/','R','e','d','u','c','e','d',' ','T','c','9','9','m',':',		// L_HYDROLIZED_REDUCED_TC99M (english)  25(0), 728
'H','i','d','r','o','l','i','z','a','d','o','/','R','e','d','u','c','i','d','o',' ',' ',' ',' ',':',		// L_HYDROLIZED_REDUCED_TC99M (french)  25(0)
'T','c','9','9','m',' ','H','M','P','A','O',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_TC99M_HMPAO (english)  25(0), 729
'T','c','9','9','m',' ','H','M','P','A','O',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_TC99M_HMPAO (french)  25(0)
'1',')',' ','F','r','a','c','t','i','o','n',' ','#','1',		// L_FRACTION_1 (english)  14(0), 730
'1',')',' ','F','r','a','c','c','i','ó','n',' ','#','1',		// L_FRACTION_1 (french)  15(0)
'2',')',' ','F','r','a','c','t','i','o','n',' ','#','2',		// L_FRACTION_2 (english)  14(0), 731
'2',')',' ','F','r','a','c','c','i','ó','n',' ','#','2',		// L_FRACTION_2 (french)  15(0)
'F','r','a','c','t','i','o','n',' ','#','1',' ',' ',' ',' ',' ',' ',':',' ',		// L_FRACTION_12 (english)  19(0), 732
'F','r','a','c','c','i','ó','n',' ','#','1',' ',' ',' ',' ',' ',' ',':',' ',		// L_FRACTION_12 (french)  20(0)
'F','r','a','c','t','i','o','n',' ','#','2',' ',' ',' ',' ',' ',' ',':',' ',		// L_FRACTION_22 (english)  19(0), 733
'F','r','a','c','c','i','ó','n',' ','#','2',' ',' ',' ',' ',' ',' ',':',' ',		// L_FRACTION_22 (french)  20(0)
'3',')',' ','C','a','r','t','r','i','d','g','e',		// L_CARTRIDGE (english)  12(0), 734
'3',')',' ','C','a','r','t','u','c','h','o',		// L_CARTRIDGE (french)  11(0)
'C','a','r','t','r','i','d','g','e',' ',' ',' ',' ',' ',' ',' ',' ',':',' ',		// L_CARTRIDGE2 (english)  19(0), 735
'C','a','r','t','u','c','h','o',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',' ',		// L_CARTRIDGE2 (french)  19(0)
'N','o','n','-','e','l','u','t','a','b','l','e',' ','T','c','9','9','m',':',' ','%','5','.','1','f',		// L_NON_ELUTABLE (english)  25(0), 736
'T','c','9','9','m',' ','N','o',' ','e','l','u','i','b','l','e',' ',' ',':',' ','%','5','.','1','f',		// L_NON_ELUTABLE (french)  25(0)
'N','o','n','-','e','l','u','t','a','b','l','e',' ','T','c','9','9','m',' ',' ',' ',' ',' ',' ',' ',':',		// L_NON_ELUTABLE2 (english)  26(1), 737
'T','c','9','9','m',' ','N','o',' ','e','l','u','i','b','l','e',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_NON_ELUTABLE2 (french)  26(1)
'H','y','d','r','o','p','h','i','l','i','c',' ','I','m','p','u','r','e',':',' ','%','5','.','1','f',		// L_HYDROPHILIC_IMPURE (english)  25(0), 738
'H','i','d','r','o','f','í','l','i','c','o',' ','i','m','p','u','r','o',':',' ','%','5','.','1','f',		// L_HYDROPHILIC_IMPURE (french)  26(1)
'H','y','d','r','o','p','h','i','l','i','c',' ','T','c',' ','I','m','p','u','r','i','t','i','e','s',':',		// L_HYDROPHILIC_TC_IMPURITIES (english)  26(1), 739
'I','m','p','u','r','e','z','a','s',' ','H','i','d','r','ó','f','i','l','a','s',' ','d','e',' ','T','c',':',		// L_HYDROPHILIC_TC_IMPURITIES (french)  28(1)
'T','c','9','9','m',' ','M','e','r','t','i','a','t','i','d','e',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_TC_MERTIATIDE (english)  26(1), 740
'T','c','9','9','m',' ','M','e','r','t','i','a','t','i','d','e',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_TC_MERTIATIDE (french)  26(1)
'C','o','m','p','l','e','t','e',' ','T','h','y','r','o','i','d',' ','U','p','t','a','k','e',' ','T','e','s','t',		// L_COMPLETE_THYROID_UPTAKE_TEST_ENGLISH (english)  28(1), 741
'C','o','m','p','l','e','t','e',' ','T','h','y','r','o','i','d',' ','U','p','t','a','k','e',' ','T','e','s','t',		// L_COMPLETE_THYROID_UPTAKE_TEST_ENGLISH (french)  28(1)
'M','a','r','k',' ','T','e','s','t',' ','C','o','m','p','l','e','t','e','d','?',		// L_MARK_TEST_COMPLETED_ENGLISH (english)  20(0), 742
'M','a','r','k',' ','T','e','s','t',' ','C','o','m','p','l','e','t','e','d','?',		// L_MARK_TEST_COMPLETED_ENGLISH (french)  20(0)
'C','o','m','p','l','e','t','e',' ','R','B','C',' ','S','u','r','v','i','v','a','l',' ','T','e','s','t',		// L_COMPLETE_RBC_SURVIVAL_TEST_ENGLISH (english)  26(1), 743
'C','o','m','p','l','e','t','e',' ','R','B','C',' ','S','u','r','v','i','v','a','l',' ','T','e','s','t',		// L_COMPLETE_RBC_SURVIVAL_TEST_ENGLISH (french)  26(1)
'I','n','a','c','t','i','v','a','t','e',' ','R','e','c','o','r','d',		// L_INACTIVATE_RECORD (english)  17(0), 744
'I','n','a','c','t','i','v','a','r',' ','R','e','g','i','s','t','r','o',		// L_INACTIVATE_RECORD (french)  18(0)
'I','n','a','c','t','i','v','a','t','e',' ','r','e','c','o','r','d','?',		// L_INACTIVATE_RECORD2 (english)  18(0), 745
'¿','Q','u','i','e','r','e','s',' ','I','n','a','c','t','i','v','a','r',' ','e','l',' ','R','e','g','i','s','t','r','o','?',		// L_INACTIVATE_RECORD2 (french)  32(1)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','C','o','m','m','e','n','t',':',		// L_PLEASE_ENTER_COMMENT (english)  21(0), 746
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','e','l',' ','C','o','m','e','n','t','a','r','i','o',':',		// L_PLEASE_ENTER_COMMENT (french)  34(1)
'C','l','e','a','r',' ','A','l','l',' ','M','e','a','s','u','r','e','m','e','n','t','s',		// L_CLEAR_ALL_MEASUREMENTS (english)  22(0), 747
'B','o','r','r','a','r',' ','T','o','d','a','s',' ','L','a','s',' ','M','e','d','i','d','a','s',		// L_CLEAR_ALL_MEASUREMENTS (french)  24(0)
'M','e','a','s','u','r','e',' ','N','o','w',		// L_MEASURE_NOW (english)  11(0), 748
'M','e','d','i','r',' ','A','h','o','r','a',		// L_MEASURE_NOW (french)  11(0)
'N','o',' ','S','e','t','t','i','n','g','s',		// L_NO_SETTINGS (english)  11(0), 749
'N','i','n','g','u','n','o','s',' ','A','j','u','s','t','e','s',		// L_NO_SETTINGS (french)  16(0)
'f','o','r',' ','R',' ','C','h','a','m','b','e','r',		// L_FOR_R_CHAMBER (english)  13(0), 750
'p','a','r','a',' ','l','a',' ','C','á','m','a','r','a',' ','R',		// L_FOR_R_CHAMBER (french)  17(0)
'f','o','r',' ','R',' ','C','h','a','m','b','e','r',		// L_FOR_R_CHAMBER2 (english)  13(0), 751
'd','e',' ','C','á','m','a','r','a',' ','R',		// L_FOR_R_CHAMBER2 (french)  12(0)
'f','o','r',' ','P','E','T',' ','C','h','a','m','b','e','r',		// L_FOR_PET_CHAMBER (english)  15(0), 752
'p','a','r','a',' ','l','a',' ','C','á','m','a','r','a',' ','P','E','T',		// L_FOR_PET_CHAMBER (french)  19(0)
'f','o','r',' ','P','E','T',' ','C','h','a','m','b','e','r',		// L_FOR_PET_CHAMBER2 (english)  15(0), 753
'd','e',' ','C','á','m','a','r','a',' ','P','E','T',		// L_FOR_PET_CHAMBER2 (french)  14(0)
'c','a','l','i','b','r','a','t','i','o','n',' ','n','u','m','b','e','r',		// L_CALIBRATION_NUMBER (english)  18(0), 754
'n','ú','m','e','r','o',' ','d','e',' ','c','a','l','i','b','r','a','c','i','ó','n',		// L_CALIBRATION_NUMBER (french)  23(0)
'L','i','n','e','a','r','i','t','y',',',' ','S','t','d',		// L_LINEARITY_STD (english)  14(0), 755
'L','i','n','e','a','l','i','d','a','d',',',' ','S','t','d','.',		// L_LINEARITY_STD (french)  16(0)
'i','n',' ','%','d',' ','h','r','s',',',' ','%','d',' ','m','i','n','s',		// L_IN_HRS_MINS (english)  18(0), 756
'e','n',' ','%','d',' ','h','o','r','a','s',',',' ','%','d',' ','m','i','n','s',		// L_IN_HRS_MINS (french)  20(0)
'F','i','r','s','t',' ','M','e','a','s','u','r','e','m','e','n','t',':',		// L_FIRST_MEASUREMENT (english)  18(0), 757
'P','r','i','m','e','r','a',' ','M','e','d','i','d','a',':',		// L_FIRST_MEASUREMENT (french)  15(0)
'C','a','l','c',':',' ',		// L_CALC (english)  6(0), 758
'C','a','l','c',':',' ',		// L_CALC (french)  6(0)
'F','a','c','t','o','r',		// L_FACTOR (english)  6(0), 759
'F','a','c','t','o','r',		// L_FACTOR (french)  6(0)
'%',' ','R','a','t','i','o',		// L_RATIO (english)  7(0), 760
'%',' ','R','e','l','a','c','i','ó','n',		// L_RATIO (french)  11(0)
'R','a','t','i','o',':',		// L_RATIO2 (english)  6(0), 761
'R','e','l','a','c','i','ó','n',':',		// L_RATIO2 (french)  10(0)
'R','e','s','u','l','t',		// L_RESULT (english)  6(0), 762
'R','e','s','u','l','t','a','d','o',		// L_RESULT (french)  9(0)
'R','e','s','u','l','t',':',		// L_RESULT2 (english)  7(0), 763
'R','e','s','u','l','t','a','d','o',':',		// L_RESULT2 (french)  10(0)
'S','t','d',' ','L','i','n','e','a','r','i','t','y',		// L_STD_LINEARITY (english)  13(0), 764
'S','t','d','.',' ','L','i','n','e','a','l','i','d','a','d',		// L_STD_LINEARITY (french)  15(0)
'S','T','A','N','D','A','R','D',' ','L','I','N','E','A','R','I','T','Y',' ','T','E','S','T',		// L_STANDARD_LINEARITY_TEST (english)  23(0), 765
'P','R','U','E','B','A',' ','D','E',' ','L','I','N','E','A','L','I','D','A','D',' ','E','S','T','Á','N','D','A','R',		// L_STANDARD_LINEARITY_TEST (french)  30(1)
'A','c','t','u','a','l',':',		// L_ACTUAL (english)  7(0), 766
'A','c','t','u','a','l',':',		// L_ACTUAL (french)  7(0)
'V','a','r','i','a','t','i','o','n',':',		// L_VARIATION (english)  10(0), 767
'V','a','r','i','a','c','i','ó','n',':',		// L_VARIATION (french)  11(0)
'L','I','N','E','A','T','O','R',' ','L','I','N','E','A','R','I','T','Y',' ','T','E','S','T',		// L_LINEATOR_LINEARITY_TEST (english)  23(0), 768
'P','R','U','E','B','A',' ','D','E',' ','L','I','N','E','A','L','I','D','A','D',' ','L','I','N','E','A','T','O','R',		// L_LINEATOR_LINEARITY_TEST (french)  29(1)
'C','h','a','m','b','e','r',' ','S','/','N',':',' ','%','s',		// L_CHAMBER_SN (english)  15(0), 769
'C','á','m','a','r','a',' ','S','/','N',':',' ','%','s',		// L_CHAMBER_SN (french)  15(0)
'L','i','n','e','a','t','o','r',' ','S','/','N',':',' ','%','s',		// L_LINEATOR_SN (english)  16(0), 770
'L','i','n','e','a','t','o','r',' ','S','/','N',':',' ','%','s',		// L_LINEATOR_SN (french)  16(0)
'P','r','e','s','e','n','t',' ','F','a','c','t','o','r',':',		// L_PRESENT_FACTOR (english)  15(0), 771
'F','a','c','t','o','r',' ','P','r','e','s','e','n','t','e',':',		// L_PRESENT_FACTOR (french)  16(0)
'C','A','L','I','C','H','E','C','K',' ','L','I','N','E','A','R','I','T','Y',' ','T','E','S','T',		// L_CALICHECK_LINEARITY_TEST (english)  24(0), 772
'P','R','U','E','B','A',' ','D','E',' ','L','I','N','E','A','L','I','D','A','D',' ','C','A','L','I','C','H','E','C','K',		// L_CALICHECK_LINEARITY_TEST (french)  30(1)
'C','a','l','i','c','h','e','c','k',' ','S','/','N',':',' ','%','s',		// L_CALICHECK_SN (english)  17(0), 773
'C','a','l','i','c','h','e','c','k',' ','S','/','N',':',' ','%','s',		// L_CALICHECK_SN (french)  17(0)
'M','e','a','n',' ','R','e','s','u','l','t',':',' ','%','s',		// L_MEAN_RESULT (english)  15(0), 774
'R','e','s','u','l','t','a','d','o',' ','M','e','d','i','o',':',' ','%','s',		// L_MEAN_RESULT (french)  19(0)
'V','a','r','i','a','t','i','o','n',' ','f','r','o','m',' ','M','e','a','n',':',		// L_VARIATION_FROM_MEAN (english)  20(0), 775
'V','a','r','i','a','c','i','ó','n',' ','d','e',' ','l','a',' ','M','e','d','i','a',':',		// L_VARIATION_FROM_MEAN (french)  23(0)
'P','C',' ','C','o','m','m','u','n','i','c','a','t','i','o','n','s',' ','M','o','d','e',		// L_PC_COMMUNICATIONS_MODE (english)  22(0), 776
'M','o','d','o',' ','d','e',' ','C','o','m','u','n','i','c','a','c','i','o','n','e','s',' ','d','e',' ','c','o','m','p','u','t','a','d','o','r','a',' ','p','e','r','s','o','n','a','l',		// L_PC_COMMUNICATIONS_MODE (french)  46(1)
'R','u','n','n','i','n','g',' ','Q','C',' ','t','e','s','t','s',' ','f','r','o','m',' ','P','C',		// L_RUNNING_QC_TESTS_FROM_PC (english)  24(0), 777
'E','j','e','c','u','t','a','n','d','o',' ','P','r','u','e','b','a','s',' ','d','e',' ','C','o','n','t','r','o','l',' ','d','e',' ','C','a','l','i','d','a','d',' ','d','e',' ','l','a',' ','P','C',		// L_RUNNING_QC_TESTS_FROM_PC (french)  49(1)
'A','b','o','r','t',' ','Q','C',' ','T','e','s','t','s',		// L_ABORT_QC_TESTS (english)  14(0), 778
'C','a','n','c','e','l','a','r',' ','P','r','u','e','b','a',' ','d','e',' ','C','o','n','t','r','o','l',' ','d','e',' ','C','a','l','i','d','a','d',		// L_ABORT_QC_TESTS (french)  37(1)
'R','e','m','o','t','e',' ','A','u','t','o',' ','Z','e','r','o',		// L_REMOTE_AUTO_ZERO (english)  16(0), 779
'A','u','t','o',' ','C','e','r','o',' ','R','e','m','o','t','o',		// L_REMOTE_AUTO_ZERO (french)  16(0)
'R','e','m','o','t','e',' ','B','a','c','k','g','r','o','u','n','d',		// L_REMOTE_BACKGROUND (english)  17(0), 780
'F','o','n','d','o',' ','R','e','m','o','t','o',		// L_REMOTE_BACKGROUND (french)  12(0)
'R','e','m','o','t','e',' ','C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',' ','T','e','s','t',		// L_REMOTE_CHAMBER_VOLTAGE_TEST (english)  27(1), 781
'P','r','u','e','b','a',' ','d','e',' ','V','o','l','t','a','j','e',' ','d','e',' ','l','a',' ','C','á','m','a','r','a',' ','R','e','m','o','t','a',		// L_REMOTE_CHAMBER_VOLTAGE_TEST (french)  38(1)
'B','a','c','k','g','r','o','u','n','d',' ','H','i','g','h',		// L_BACKGROUND_HIGH (english)  15(0), 782
'F','o','n','d','o',' ','A','l','t','o',		// L_BACKGROUND_HIGH (french)  10(0)
'C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',' ','F','a','i','l','e','d',		// L_CHAMBER_VOLTAGE_FAILED (english)  22(0), 783
'F','a','l','l','ó',' ','V','o','l','t','a','j','e',' ','d','e',' ','l','a',' ','C','á','m','a','r','a',		// L_CHAMBER_VOLTAGE_FAILED (french)  28(1)
'P','l','e','a','s','e',' ','W','a','i','t',		// L_PLEASE_WAIT (english)  11(0), 784
'F','a','v','o','r',' ','d','e',' ','E','s','p','e','r','a','r',		// L_PLEASE_WAIT (french)  16(0)
'T','h','e',' ','P','E','T',' ','c','h','a','m','b','e','r',' ','c','a','n',' ','n','o','t',' ','r','u','n',' ','a',' ','M','o','l','y',' ','A','s','s','a','y',		// L_PET_CHAMBER_CANNOT_RUN_MOLY_ASSAY (english)  40(1), 785
'L','a',' ','c','á','m','a','r','a',' ','d','e',' ','P','E','T',' ','n','o',' ','p','u','e','d','e',' ','e','j','e','c','u','t','a','r',' ','u','n',' ','E','n','s','a','y','o',' ','d','e',' ','M','o','l','y',		// L_PET_CHAMBER_CANNOT_RUN_MOLY_ASSAY (french)  53(2)
'M','i','s','s','i','n','g',' ','U','S','B',' ','D','r','i','v','e',		// L_MISSING_USB_DRIVE (english)  17(0), 786
'F','a','l','t','a',' ','C','o','n','t','r','o','l','a','d','o','r',' ','U','S','B',		// L_MISSING_USB_DRIVE (french)  21(0)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','f','i','l','e',		// L_UNABLE_TO_FIND_FILE (english)  19(0), 787
'I','n','c','a','p','a','z',' ','d','e',' ','e','n','c','o','n','t','r','a','r',' ','e','l',' ','a','r','c','h','i','v','o',		// L_UNABLE_TO_FIND_FILE (french)  31(1)
'P','l','a','c','e',' ','o','n','l','y',' ','o','n','e',' ','u','p','d','a','t','e',		// L_PLACE_ONLY_ONE_UPDATE (english)  21(0), 788
'C','o','l','o','q','u','e',' ','u','n',' ','s','o','l','o',' ','a','r','c','h','i','v','o',' ','d','e',		// L_PLACE_ONLY_ONE_UPDATE (french)  26(1)
'f','i','l','e',' ','i','n',' ','r','o','o','t',' ','f','o','l','d','e','r',		// L_FILE_IN_ROOT_FOLDER (english)  19(0), 789
'a','c','t','u','a','l','i','z','a','c','i','ó','n',' ','e','n',' ','l','a',' ','c','a','r','p','e','t','a',' ','r','a','í','z',		// L_FILE_IN_ROOT_FOLDER (french)  34(1)
'I','n','v','a','l','i','d',' ','u','p','d','a','t','e',' ','f','i','l','e',		// L_INVALID_UPDATE_FILE (english)  19(0), 790
'A','r','c','h','i','v','o',' ','d','e',' ','A','c','t','u','a','l','i','z','a','c','i','ó','n',' ','I','n','v','á','l','i','d','o',		// L_INVALID_UPDATE_FILE (french)  35(1)
'C','o','p','y','i','n','g',' ','%','s',		// L_COPYING (english)  10(0), 791
'C','o','p','i','a','r',' ','%','s',		// L_COPYING (french)  9(0)
'o','f',' ','%','s',		// L_OF2 (english)  5(0), 792
'd','e',' ','%','s',		// L_OF2 (french)  5(0)
'C','o','r','r','u','p','t',' ','U','p','d','a','t','e',' ','F','i','l','e',		// L_CORRUPT_UPDATE_FILE (english)  19(0), 793
'A','r','c','h','i','v','o',' ','d','e',' ','A','c','t','u','a','l','i','z','a','c','i','ó','n',' ','C','o','r','r','u','p','t','o','s',		// L_CORRUPT_UPDATE_FILE (french)  35(1)
'F','i','n','i','s','h','e','d',' ','C','o','p','y','i','n','g',		// L_FINISHED_COPYING (english)  16(0), 794
'T','e','r','m','i','n','ó',' ','C','o','p','i','a','d','o',		// L_FINISHED_COPYING (french)  16(0)
'P','l','e','a','s','e',' ','r','e','s','t','a','r','t',		// L_PLEASE_RESTART2 (english)  14(0), 795
'F','a','v','o','r',' ','d','e',' ','R','e','i','n','i','c','i','a','r',		// L_PLEASE_RESTART2 (french)  18(0)
'U','S','B',' ','D','r','i','v','e',' ','E','r','r','o','r','\n',		// L_USB_DRIVE_ERROR (english)  16(0), 796
'E','r','r','o','r',' ','d','e','l',' ','C','o','n','t','r','o','l','a','d','o','r',' ','U','S','B','\n',		// L_USB_DRIVE_ERROR (french)  26(1)
'S','o','f','t','w','a','r','e',' ','U','p','d','a','t','e',		// L_SOFTWARE_UPDATE (english)  15(0), 797
'A','c','t','u','a','l','i','z','a','c','i','ó','n',' ','d','e',' ','s','o','f','t','w','a','r','e',		// L_SOFTWARE_UPDATE (french)  26(1)
'S','o','f','t','w','a','r','e',' ','d','o','w','n','l','o','a','d',' ','i','s',' ','i','n',' ','p','r','o','g','r','e','s','s','\n','P','l','e','a','s','e',' ','d','o',' ','n','o','t',' ','t','u','r','n',' ','o','f','f',' ','c','a','l','i','b','r','a','t','o','r',		// L_SOFTWARE_DOWNLOAD_IN_PROGRESS (english)  66(2), 798
'D','e','s','c','a','r','g','a',' ','d','e','l',' ','S','o','f','t','w','a','r','e',' ','e','s','t','á',' ','e','n',' ','p','r','o','c','e','s','o','\n','P','o','r',' ','f','a','v','o','r',',',' ','n','o',' ','a','p','a','g','u','e',' ','e','l',' ','c','a','l','i','b','r','a','d','o','r',		// L_SOFTWARE_DOWNLOAD_IN_PROGRESS (french)  73(2)
'P','r','i','n','t','e','r',' ','P','r','o','b','l','e','m',		// L_PRINTER_PROBLEM (english)  15(0), 799
'P','r','o','b','l','e','m','a',' ','d','e',' ','l','a',' ','I','m','p','r','e','s','o','r','a',		// L_PRINTER_PROBLEM (french)  24(0)
'I','N','S','E','R','T',' ','T','I','C','K','E','T',		// L_INSERT_TICKET (english)  13(0), 800
'I','n','t','r','o','d','u','z','c','a',' ','e','l',' ','T','i','q','u','e','t','e',		// L_INSERT_TICKET (french)  21(0)
'C','H','E','C','K',' ','P','R','I','N','T','E','R',		// L_CHECK_PRINTER (english)  13(0), 801
'R','e','v','i','s','e',' ','l','a',' ','I','m','p','r','e','s','o','r','a',		// L_CHECK_PRINTER (french)  19(0)
'C','O','N','N','E','C','T',' ','P','R','I','N','T','E','R',		// L_CONNECT_PRINTER (english)  15(0), 802
'C','O','N','E','C','T','E',' ','I','M','P','R','E','S','O','R','A',		// L_CONNECT_PRINTER (french)  17(0)
'I','N','S','E','R','T',' ','P','A','P','E','R',		// L_INSERT_PAPER (english)  12(0), 803
'I','N','S','E','R','T','E',' ','P','A','P','E','L',		// L_INSERT_PAPER (french)  13(0)
'F','i','l','e',' ','T','r','a','n','s','f','e','r',' ','E','r','r','o','r',		// L_FILE_TRANSFER_ERROR (english)  19(0), 804
'E','r','r','o','r',' ','d','e',' ','T','r','a','n','s','f','e','r',' ','d','e',' ','A','r','c','h','i','v','o',		// L_FILE_TRANSFER_ERROR (french)  28(1)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','f','i','l','e',' ','p','a','t','h',':',' ','%','s','\n',		// L_UNABLE_TO_FIND_FILE_PATH (english)  29(1), 805
'N','o',' ','s','e',' ','p','u','e','d','e',' ','e','n','c','o','n','t','r','a','r',' ','l','a',' ','r','u','t','a',' ','d','e','l',' ','a','r','c','h','i','v','o',':',' ','%','s','\n',		// L_UNABLE_TO_FIND_FILE_PATH (french)  46(1)
'U','p','d','a','t','e',		// L_UPDATE (english)  6(0), 806
'A','c','t','u','a','l','i','z','a','r',		// L_UPDATE (french)  10(0)
'P','o','w','e','r',' ','O','f','f',',','\n','a','n','d',' ','t','h','e','n',' ','P','o','w','e','r',' ','O','n','\n','t','o',' ','c','o','m','p','l','e','t','e',' ','u','p','d','a','t','e','.',		// L_POWER_OFF_AND_THEN_POWER_ON_TO_COMPLETE_UPDATE (english)  48(1), 807
'A','p','a','g','u','e',',','\n','y',' ','d','e','s','p','u','é','s',' ','e','n','c','i','e','n','d','a','\n','p','a','r','a',' ','c','o','m','p','l','e','t','a','r',' ','l','a',' ','a','c','t','u','a','l','i','z','a','c','i','ó','n',		// L_POWER_OFF_AND_THEN_POWER_ON_TO_COMPLETE_UPDATE (french)  60(2)
'C','h','e','c','k','s','u','m',' ','h','a','s',' ','f','a','i','l','e','d','.',		// L_CHECKSUM_HAS_FAILED (english)  20(0), 808
'L','a',' ','s','u','m','a',' ','d','e',' ','c','o','m','p','r','o','b','a','c','i','o','n',' ','h','a',' ','f','a','l','l','a','d','o',		// L_CHECKSUM_HAS_FAILED (french)  34(1)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','t','r','a','n','s','f','e','r',' ','f','i','l','e','.',		// L_UNABLE_TO_FIND_TRANSFER_FILE (english)  29(1), 809
'N','o',' ','s','e',' ','p','u','e','d','e',' ','e','n','c','o','n','t','r','a','r',' ','l','a',' ','t','r','a','n','s','f','e','r','e','n','c','i','a',' ','d','e',' ','a','r','c','h','i','v','o','s',		// L_UNABLE_TO_FIND_TRANSFER_FILE (french)  50(1)
'U','p','d','a','t','e',' ','S','o','f','t','w','a','r','e',		// L_UPDATE_SOFTWARE (english)  15(0), 810
'A','c','t','u','a','l','i','c','e',' ','S','o','f','t','w','a','r','e',		// L_UPDATE_SOFTWARE (french)  18(0)
'N','o','w',':',		// L_NOW (english)  4(0), 811
'A','h','o','r','a',':',		// L_NOW (french)  6(0)
'C','o','n','s','t','a','n','c','y',' ','C','h','a','n','n','e','l','s',':',		// L_PR_CONSTANCY_CHANNELS (english)  19(0), 812
'C','a','n','a','l','e','s',' ','d','e',' ','E','s','t','a','b','i','l','i','d','a','d',':',		// L_PR_CONSTANCY_CHANNELS (french)  23(0)
' ','O','V','E','R',' ',		// L_OVER (english)  6(0), 813
' ','A','L','T','O',' ',		// L_OVER (french)  6(0)
'C','h','a','m','b','e','r',' ','E','r','r','o','r',		// L_CHAMBER_ERROR (english)  13(0), 814
'E','r','r','o','r',' ','d','e',' ','C','á','m','a','r','a',		// L_CHAMBER_ERROR (french)  16(0)
'C','h','a','m','b','e','r',' ','#','%','d',' ','h','a','s',' ','l','o','s','t',' ','c','o','n','n','e','c','t','i','o','n','.','\n','P','l','e','a','s','e',' ','t','u','r','n',' ','o','f','f',' ','c','a','l','i','b','r','a','t','o','r',' ','a','n','d',' ','r','e','a','t','t','a','c','h',' ','c','h','a','m','b','e','r','.',		// L_CHAMBER_LOST_CONNECTION (english)  81(3), 815
'L','a',' ','c','á','m','a','r','a',' ','#',' ','1',' ','p','e','r','d','i','d','o',' ','l','a',' ','c','o','n','e','x','i','ó','n','.',' ','P','o','r',' ','f','a','v','o','r',',',' ','a','p','a','g','u','e',' ','e','l',' ','c','a','l','i','b','r','a','d','o','r',' ','y',' ','r','e','c','o','n','e','c','t','e',' ','l','a',' ','c','á','m','a','r','a','.',		// L_CHAMBER_LOST_CONNECTION (french)  92(3)
'S','a','v','e',' ','A','c','c','u','r','a','c','y',' ','/',' ','A','u','t','o','C','o','n','s','t','a','n','c','y',		// L_SAVE_ACCURACY_TEST (english)  29(1), 816
'G','u','a','r','d','a','r',' ','E','x','a','c','t','i','t','u','d',' ','/',' ','A','u','t','o','-','E','s','t','a','b','i','l','i','d','a','d',		// L_SAVE_ACCURACY_TEST (french)  36(1)
'Z','e','r','o',		// L_ZERO_2 (english)  4(0), 817
'C','e','r','o',		// L_ZERO_2 (french)  4(0)
'C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',		// L_CHAMBER_VOLTAGE_2 (english)  15(0), 818
'V','o','l','t','a','j','e',' ','d','e',' ','l','a',' ','C','á','m','a','r','a',		// L_CHAMBER_VOLTAGE_2 (french)  21(0)
'A','c','c','u','r','a','c','y',' ','T','e','s','t',		// L_ACCURACYTEST_2 (english)  13(0), 819
'P','r','u','e','b','a',' ','d','e',' ','E','x','a','c','t','i','t','u','d',		// L_ACCURACYTEST_2 (french)  19(0)
'S','e','a','r','c','h',' ','D','a','i','l','y',' ','T','e','s','t','s',		// L_SEARCH_DAILY_TESTS (english)  18(0), 820
'B','u','s','c','a','r',' ','P','r','u','e','b','a','s',' ','D','i','a','r','i','a','s',		// L_SEARCH_DAILY_TESTS (french)  22(0)
'S','e','a','r','c','h',' ','Z','e','r','o',' ','M','e','a','s','u','r','e','m','e','n','t','s',		// L_SEARCH_ZERO_MEASUREMENTS (english)  24(0), 821
'B','u','s','c','a','r',' ','M','e','d','i','c','i','o','n','e','s',' ','C','e','r','o',		// L_SEARCH_ZERO_MEASUREMENTS (french)  22(0)
'S','e','a','r','c','h',' ','B','a','c','k','g','r','o','u','n','d',' ','M','e','a','s','u','r','e','m','e','n','t','s',		// L_SEARCH_BACKGROUND_MEASUREMENTS (english)  30(1), 822
'B','u','s','c','a','r',' ','M','e','d','i','c','i','o','n','e','s',' ','d','e',' ','F','o','n','d','o',		// L_SEARCH_BACKGROUND_MEASUREMENTS (french)  26(1)
'S','e','a','r','c','h',' ','C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',' ','T','e','s','t','s',		// L_SEARCH_CHAMBER_VOLTAGE (english)  28(1), 823
'B','u','s','c','a','r',' ','P','r','u','e','b','a','s',' ','d','e','l',' ','V','o','l','t','a','j','e',' ','d','e',' ','l','a',' ','C','á','m','a','r','a',		// L_SEARCH_CHAMBER_VOLTAGE (french)  40(1)
'S','e','a','r','c','h',' ','A','c','c','u','r','a','c','y',' ','T','e','s','t','s',		// L_SEARCH_ACCURACY_TESTS (english)  21(0), 824
'B','u','s','c','a','r',' ','P','r','u','e','b','a','s',' ','d','e',' ','E','x','a','c','t','i','t','u','d',		// L_SEARCH_ACCURACY_TESTS (french)  27(1)
'N','O','M','I','N','A','L',' ','V','O','L','T','A','G','E',':',		// L_NOMINAL_VOLTAGE (english)  16(0), 825
'V','O','L','T','A','J','E',' ','N','O','M','I','N','A','L',':',		// L_NOMINAL_VOLTAGE (french)  16(0)
'M','I','N',' ','V','O','L','T','A','G','E',':',		// L_MIN_VOLTAGE (english)  12(0), 826
'V','O','L','T','A','J','E',' ','M','I','N',':',		// L_MIN_VOLTAGE (french)  12(0)
'M','A','X',' ','V','O','L','T','A','G','E',':',		// L_MAX_VOLTAGE (english)  12(0), 827
'V','O','L','T','A','J','E',' ','M','A','X',':',		// L_MAX_VOLTAGE (french)  12(0)
'S','t','a','g','e',		// L_STAGE (english)  5(0), 828
'E','t','a','p','a',		// L_STAGE (french)  5(0)
'P','a','g','e',' ','%','d',		// L_PAGE (english)  7(0), 829
'P','á','g','i','n','a',' ','%','d',		// L_PAGE (french)  10(0)
'H','a','l','f','-','l','i','f','e',' ','C','a','l','c','u','l','a','t','o','r',		// L_HALFLIFE_CALC (english)  20(0), 830
'H','a','l','f','-','l','i','f','e',' ','C','a','l','c','u','l','a','t','o','r',		// L_HALFLIFE_CALC (french)  20(0)
'H','a','l','f','-','l','i','f','e',' ','C','a','l','c',' ','-',' ','C','h',':','%','d',',',' ','%','s',		// L_HALFLIFE_CALC2 (english)  26(1), 831
'H','a','l','f','-','l','i','f','e',' ','C','a','l','c',' ','-',' ','C','h',':','%','d',',',' ','%','s',		// L_HALFLIFE_CALC2 (french)  26(1)
'%','d',' ','s','e','c',		// L_SECS (english)  6(0), 832
'%','d',' ','s','e','q',		// L_SECS (french)  6(0)
'E','n','t','e','r',' ','I','n','t','e','r','v','a','l',' ','i','n',' ','s','e','c','o','n','d','s',':',		// L_ENTER_INTERVAL_SEC (english)  26(1), 833
'E','n','t','e','r',' ','I','n','t','e','r','v','a','l',' ','i','n',' ','s','e','c','o','n','d','s',':',		// L_ENTER_INTERVAL_SEC (french)  26(1)
'E','n','t','e','r',' ','T','o','t','a','l',' ','T','i','m','e',' ','i','n',' ','m','i','n','u','t','e','s',':',		// L_ENTER_TOTAL_TIME_IN_MINUTES (english)  28(1), 834
'E','n','t','e','r',' ','T','o','t','a','l',' ','T','i','m','e',' ','i','n',' ','m','i','n','u','t','e','s',':',		// L_ENTER_TOTAL_TIME_IN_MINUTES (french)  28(1)
'%','d',' ','m','i','n',		// L_MINS (english)  6(0), 835
'%','d',' ','m','i','n',		// L_MINS (french)  6(0)
'S','t','o','p',		// L_STOP (english)  4(0), 836
'S','t','o','p',		// L_STOP (french)  4(0)
'E','l','a','p','s','e','d',' ','(','s','e','c',')',		// L_ELAPSED_SEC (english)  13(0), 837
'E','l','a','p','s','e','d',' ','(','s','e','c',')',		// L_ELAPSED_SEC (french)  13(0)
'C','o','u','n','t',' ','i','s',' ','g','r','e','a','t','e','r',' ','t','h','a','n',' ','1','2','0','1','\n','T','r','y',' ','T','o','t','a','l',':',' ','%','d',' ','o','r',' ','I','n','t','e','r','v','a','l',':',' ','%','d',		// L_NUM_OF_MEAS_GREATER_THAN_1201 (english)  56(2), 838
'C','o','u','n','t',' ','i','s',' ','g','r','e','a','t','e','r',' ','t','h','a','n',' ','1','2','0','1','\n','T','r','y',' ','T','o','t','a','l',':',' ','%','d',' ','o','r',' ','I','n','t','e','r','v','a','l',':',' ','%','d',		// L_NUM_OF_MEAS_GREATER_THAN_1201 (french)  56(2)
'E','l','a','p','s','e','d',':',' ','%','d',':','%','0','2','d','\n','R','e','m','a','i','n','i','n','g',':',' ','%','d',':','%','0','2','d',		// L_ELAPSED_REMAINING (english)  35(1), 839
'E','l','a','p','s','e','d',':',' ','%','d',':','%','0','2','d','\n','R','e','m','a','i','n','i','n','g',':',' ','%','d',':','%','0','2','d',		// L_ELAPSED_REMAINING (french)  35(1)
'U','n','d','e','r',' ','R','a','n','g','e',		// L_UNDERRANGE (english)  11(0), 840
'U','n','d','e','r',' ','R','a','n','g','e',		// L_UNDERRANGE (french)  11(0)
'L','e','s','s',' ','t','h','a','n',' ','5',' ','m','e','a','s','u','r','e','m','e','n','t','s',		// L_LESS_THAN_5_MEASUREMENTS (english)  24(0), 841
'M','e','n','o','s',' ','d','e',' ','5',' ','m','e','d','i','c','i','o','n','e','s',		// L_LESS_THAN_5_MEASUREMENTS (french)  21(0)
'H','a','l','f','-','l','i','f','e',' ','i','s',' ','t','o','o',' ','s','h','o','r','t',		// L_HALFLIFE_TOO_SHORT (english)  22(0), 842
'H','a','l','f','-','l','i','f','e',' ','i','s',' ','t','o','o',' ','s','h','o','r','t',		// L_HALFLIFE_TOO_SHORT (french)  22(0)
'H','a','l','f','-','l','i','f','e',' ','i','s',' ','t','o','o',' ','l','o','n','g',		// L_HALFLIFE_TOO_LONG (english)  21(0), 843
'H','a','l','f','-','l','i','f','e',' ','i','s',' ','t','o','o',' ','l','o','n','g',		// L_HALFLIFE_TOO_LONG (french)  21(0)
'H','a','l','f','-','l','i','f','e',' ','i','s',' ','n','e','g','a','t','i','v','e',		// L_HALFLIFE_NEGATIVE (english)  21(0), 844
'H','a','l','f','-','l','i','f','e',' ','i','s',' ','n','e','g','a','t','i','v','e',		// L_HALFLIFE_NEGATIVE (french)  21(0)
'C','h','a','m','b','e','r',' ','S','/','N',':',		// L_CHAMBER_SN_2 (english)  12(0), 845
'C','á','m','a','r','a',' ','S','/','N',':',		// L_CHAMBER_SN_2 (french)  12(0)
'P','r','i','n','t','\n','S','u','m','m','a','r','y',		// L_PRINT_SUMMARY (english)  13(0), 846
'P','r','i','n','t','\n','S','u','m','m','a','r','y',		// L_PRINT_SUMMARY (french)  13(0)
'B','T',' ','C','H','A','M','B','E','R',		// L_PR_BT_CHAMBER (english)  10(0), 847
'C','á','m','a','r','a',' ','B','T',		// L_PR_BT_CHAMBER (french)  10(0)
'1','.','8',' ','A','t','m',' ','C','H','A','M','B','E','R',		// L_PR_18_CHAMBER (english)  15(0), 848
'C','á','m','a','r','a',' ','1','.','8',' ','A','t','m',		// L_PR_18_CHAMBER (french)  15(0)
'B','T',' ','C','h','a','m','b','e','r',		// L_BT_CHAMBER (english)  10(0), 849
'c','á','m','a','r','a',' ','B','T',		// L_BT_CHAMBER (french)  10(0)
'1','.','8',' ','A','t','m',' ','C','h','a','m','b','e','r',		// L_18_CHAMBER (english)  15(0), 850
'c','á','m','a','r','a',' ','1','.','8',' ','A','t','m',		// L_18_CHAMBER (french)  15(0)
'1',' ','A','t','m',' ','C','h','a','m','b','e','r',		// L_10_CHAMBER (english)  13(0), 851
'c','á','m','a','r','a',' ','1',' ','A','t','m',		// L_10_CHAMBER (french)  13(0)
'f','o','r',' ','B','T',' ','C','h','a','m','b','e','r',		// L_FOR_BT_CHAMBER (english)  14(0), 852
'p','a','r','a',' ','l','a',' ','C','á','m','a','r','a',' ','B','T',		// L_FOR_BT_CHAMBER (french)  18(0)
'f','o','r',' ','1','.','8',' ','A','t','m',' ','C','h','a','m','b','e','r',		// L_FOR_18_CHAMBER (english)  19(0), 853
'p','a','r','a',' ','l','a',' ','C','á','m','a','r','a',' ','1','.','8',' ','A','t','m',		// L_FOR_18_CHAMBER (french)  23(0)
'f','o','r',' ','B','T',' ','C','h','a','m','b','e','r',		// L_FOR_BT_CHAMBER2 (english)  14(0), 854
'd','e',' ','C','á','m','a','r','a',' ','B','T',		// L_FOR_BT_CHAMBER2 (french)  13(0)
'f','o','r',' ','1','.','8',' ','A','t','m',' ','C','h','a','m','b','e','r',		// L_FOR_18_CHAMBER2 (english)  19(0), 855
'd','e',' ','C','á','m','a','r','a',' ','1','.','8',' ','A','t','m',		// L_FOR_18_CHAMBER2 (french)  18(0)
'C','R','C','-','7','7','t',' ',' ',' ',' ',' ',' ','R','A','D','I','O','I','S','O','T','O','P','E',' ','D','O','S','E',' ','C','A','L','I','B','R','A','T','O','R',		// L_77T_FULL_TITLE (english)  41(1), 856
'C','R','C','-','7','7','t',' ',' ',' ',' ',' ',' ','C','A','L','I','B','R','A','D','O','R',' ','D','E',' ','D','O','S','I','S',' ','D','E',' ','R','A','D','I','O','I','S','Ó','T','O','P','O','S',		// L_77T_FULL_TITLE (french)  50(1)
'T','h','e',' ','1',' ','A','t','m',' ','c','h','a','m','b','e','r',' ','c','a','n',' ','n','o','t',' ','r','u','n',' ','a',' ','M','o','l','y',' ','A','s','s','a','y',		// L_10_CHAMBER_CANNOT_RUN_MOLY_ASSAY (english)  42(1), 857
'L','a',' ','c','á','m','a','r','a',' ','d','e',' ','1',' ','A','t','m',' ','n','o',' ','p','u','e','d','e',' ','e','j','e','c','u','t','a','r',' ','u','n',' ','E','n','s','a','y','o',' ','d','e',' ','M','o','l','y',		// L_10_CHAMBER_CANNOT_RUN_MOLY_ASSAY (french)  55(2)
'S','e','t','u','p',' ','H','o','t','k','e','y','s',		// L_SETUP_HOTKEYS (english)  13(0), 858
'C','o','n','f','i','g','.',' ','T','e','c','l','a',' ','R','a','p','i','d','a',		// L_SETUP_HOTKEYS (french)  20(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','C','a','l',' ','#',':',		// L_ENTER_CAL (english)  19(0), 859
'F','a','v','o','r',' ','d','e',' ','I','n','t','r','o','d','u','c','i','r',' ','C','a','l',' ','#',':',		// L_ENTER_CAL (french)  26(1)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','S','t','a','n','d','a','r','d',		// L_SETUP_LINEARITY_STANDARD (english)  24(0), 860
'C','o','n','f','i','g','u','r','a',' ','e','l',' ','e','s','t','á','n','d','a','r',' ','d','e',' ','l','i','n','e','a','l','i','d','a','d',		// L_SETUP_LINEARITY_STANDARD (french)  36(1)
'H','R',' ','C','H','A','M','B','E','R',		// L_PR_C_CHAMBER (english)  10(0), 861
'C','á','m','a','r','a',' ','H','R',		// L_PR_C_CHAMBER (french)  10(0)
'1','K',' ','C','H','A','M','B','E','R',		// L_PR_K_CHAMBER (english)  10(0), 862
'C','á','m','a','r','a',' ','1','K',		// L_PR_K_CHAMBER (french)  10(0)
'H','R',' ','C','h','a','m','b','e','r',		// L_C_CHAMBER (english)  10(0), 863
'c','á','m','a','r','a',' ','H','R',		// L_C_CHAMBER (french)  10(0)
'1','K',' ','C','h','a','m','b','e','r',		// L_K_CHAMBER (english)  10(0), 864
'c','á','m','a','r','a',' ','1','K',		// L_K_CHAMBER (french)  10(0)
'f','o','r',' ','H','R',' ','C','h','a','m','b','e','r',		// L_FOR_C_CHAMBER (english)  14(0), 865
'p','a','r','a',' ','l','a',' ','C','á','m','a','r','a',' ','H','R',		// L_FOR_C_CHAMBER (french)  18(0)
'f','o','r',' ','1','K',' ','C','h','a','m','b','e','r',		// L_FOR_K_CHAMBER (english)  14(0), 866
'p','a','r','a',' ','l','a',' ','C','á','m','a','r','a',' ','1','K',		// L_FOR_K_CHAMBER (french)  18(0)
'f','o','r',' ','H','R',' ','C','h','a','m','b','e','r',		// L_FOR_C_CHAMBER2 (english)  14(0), 867
'd','e',' ','C','á','m','a','r','a',' ','H','R',		// L_FOR_C_CHAMBER2 (french)  13(0)
'f','o','r',' ','1','K',' ','C','h','a','m','b','e','r',		// L_FOR_K_CHAMBER2 (english)  14(0), 868
'd','e',' ','C','á','m','a','r','a',' ','1','K',		// L_FOR_K_CHAMBER2 (french)  13(0)
'S','t','a','n','d','a','r','d',' ','H','R',' ','C','h','a','m','b','e','r',		// L_STANDARD_C_CHAMBER (english)  19(0), 869
'E','s','t','á','n','d','a','r',' ','c','á','m','a','r','a',' ','H','R',		// L_STANDARD_C_CHAMBER (french)  20(0)
'S','t','a','n','d','a','r','d',' ','1','K',' ','C','h','a','m','b','e','r',		// L_STANDARD_K_CHAMBER (english)  19(0), 870
'E','s','t','á','n','d','a','r',' ','c','á','m','a','r','a',' ','1','K',		// L_STANDARD_K_CHAMBER (french)  20(0)
'M','a','x',' ','S','e','c','o','n','d',' ','i','s',' ','5','9',		// L_MAX_SECOND_IS_59 (english)  16(0), 871
'M','a','x',' ','S','e','c','o','n','d',' ','i','s',' ','5','9',		// L_MAX_SECOND_IS_59 (french)  16(0)
'S','t','a','r','t',' ','M','e','a','s','u','r','e','m','e','n','t',		// L_START_MEASUREMENT (english)  17(0), 872
'S','t','a','r','t',' ','M','e','a','s','u','r','e','m','e','n','t',		// L_START_MEASUREMENT (french)  17(0)
'S','e','t','u','p',' ','K','e','y',		// L_SETUP_KEY (english)  9(0), 873
'S','e','t','u','p',' ','K','e','y',		// L_SETUP_KEY (french)  9(0)
'D','e','v','i','c','e',' ','I','D',':',		// L_DEVICE_ID (english)  10(0), 874
'D','e','v','i','c','e',' ','I','D',':',		// L_DEVICE_ID (french)  10(0)
'K','e','y',':',		// L_KEY (english)  4(0), 875
'K','e','y',':',		// L_KEY (french)  4(0)
'C','o','m','m','u','n','i','c','a','t','i','o','n','s',':',' ','E','n','a','b','l','e','d',		// L_COMM_ENABLED (english)  23(0), 876
'C','o','m','m','u','n','i','c','a','t','i','o','n','s',':',' ','E','n','a','b','l','e','d',		// L_COMM_ENABLED (french)  23(0)
'C','o','m','m','u','n','i','c','a','t','i','o','n','s',':',' ','D','i','s','a','b','l','e','d',		// L_COMM_DISABLED (english)  24(0), 877
'C','o','m','m','u','n','i','c','a','t','i','o','n','s',':',' ','D','i','s','a','b','l','e','d',		// L_COMM_DISABLED (french)  24(0)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','K','e','y',		// L_PLS_ENTER_KEY (english)  16(0), 878
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','K','e','y',		// L_PLS_ENTER_KEY (french)  16(0)
'E','r','r','o','r',':',' ','K','e','y',' ','i','s',' ','e','m','p','t','y',		// L_ERR_EMPTY_KEY (english)  19(0), 879
'E','r','r','o','r',':',' ','K','e','y',' ','i','s',' ','e','m','p','t','y',		// L_ERR_EMPTY_KEY (french)  19(0)
'E','r','r','o','r',':',' ','K','e','y',' ','i','s',' ','n','o','t',' ','v','a','l','i','d',		// L_ERR_INVALID_KEY (english)  23(0), 880
'E','r','r','o','r',':',' ','K','e','y',' ','i','s',' ','n','o','t',' ','v','a','l','i','d',		// L_ERR_INVALID_KEY (french)  23(0)
'S','e','t','u','p',' ','P','a','s','s','w','o','r','d',		// L_SETUP_PASSWORD (english)  14(0), 881
'S','e','t','u','p',' ','P','a','s','s','w','o','r','d',		// L_SETUP_PASSWORD (french)  14(0)
'C','u','r','r','e','n','t',' ','P','a','s','s','w','o','r','d',':',		// L_CURRENT_PASSWORD (english)  17(0), 882
'C','u','r','r','e','n','t',' ','P','a','s','s','w','o','r','d',':',		// L_CURRENT_PASSWORD (french)  17(0)
'N','e','w',' ','P','a','s','s','w','o','r','d',':',		// L_NEW_PASSWORD (english)  13(0), 883
'N','e','w',' ','P','a','s','s','w','o','r','d',':',		// L_NEW_PASSWORD (french)  13(0)
'C','o','n','f','i','r','m',' ','P','a','s','s','w','o','r','d',':',		// L_CONFIRM_PASSWORD (english)  17(0), 884
'C','o','n','f','i','r','m',' ','P','a','s','s','w','o','r','d',':',		// L_CONFIRM_PASSWORD (french)  17(0)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','C','u','r','r','e','n','t',' ','P','a','s','s','w','o','r','d',		// L_ENTER_CURRENT_PASSWORD (english)  29(1), 885
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','C','u','r','r','e','n','t',' ','P','a','s','s','w','o','r','d',		// L_ENTER_CURRENT_PASSWORD (french)  29(1)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','N','e','w',' ','P','a','s','s','w','o','r','d',		// L_ENTER_NEW_PASSWORD (english)  25(0), 886
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','N','e','w',' ','P','a','s','s','w','o','r','d',		// L_ENTER_NEW_PASSWORD (french)  25(0)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','C','o','n','f','i','r','m',' ','P','a','s','s','w','o','r','d',		// L_ENTER_CONFIRM_PASSWORD (english)  29(1), 887
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','C','o','n','f','i','r','m',' ','P','a','s','s','w','o','r','d',		// L_ENTER_CONFIRM_PASSWORD (french)  29(1)
'I','n','v','a','l','i','d',' ','C','u','r','r','e','n','t',' ','P','a','s','s','w','o','r','d',		// L_INVALID_CURRENT_PASSWORD (english)  24(0), 888
'I','n','v','a','l','i','d',' ','C','u','r','r','e','n','t',' ','P','a','s','s','w','o','r','d',		// L_INVALID_CURRENT_PASSWORD (french)  24(0)
'N','e','w',' ','a','n','d',' ','C','o','n','f','i','r','m',' ','p','a','s','s','w','o','r','d','s',' ','d','o',' ','n','o','t',' ','m','a','t','c','h',		// L_MISMATCH_PASSWORD (english)  38(1), 889
'N','e','w',' ','a','n','d',' ','C','o','n','f','i','r','m',' ','p','a','s','s','w','o','r','d','s',' ','d','o',' ','n','o','t',' ','m','a','t','c','h',		// L_MISMATCH_PASSWORD (french)  38(1)
'U','s','e',' ','l','a','s','t',' ','3',' ','S','N',' ','d','i','g','i','t','s',		// L_LAST_3_SN (english)  20(0), 890
'U','s','e',' ','l','a','s','t',' ','3',' ','S','N',' ','d','i','g','i','t','s',		// L_LAST_3_SN (french)  20(0)
0
};

const LANG_MAP lang_map[] = {
0,4,
8,12,
16,30,
44,53,
69,88,
107,115,
124,129,
134,147,
172,175,
177,188,
200,217,
234,248,
265,272,
280,285,
295,311,
326,343,
360,365,
373,395,
426,430,
436,441,
448,462,
476,488,
505,513,
521,534,
552,562,
577,590,
609,621,
638,653,
674,686,
703,726,
755,780,
811,833,
859,867,
876,880,
885,890,
896,914,
936,941,
947,953,
960,966,
974,986,
1001,1013,
1026,1033,
1041,1048,
1056,1064,
1074,1081,
1087,1094,
1101,1110,
1117,1124,
1138,1142,
1149,1169,
1197,1217,
1245,1266,
1296,1317,
1345,1366,
1394,1403,
1413,1419,
1426,1431,
1436,1448,
1459,1471,
1490,1493,
1496,1500,
1504,1520,
1541,1549,
1558,1576,
1593,1613,
1632,1668,
1714,1752,
1800,1820,
1838,1849,
1858,1879,
1906,1930,
1961,1998,
2045,2047,
2051,2078,
2105,2114,
2123,2132,
2141,2158,
2175,2188,
2201,2208,
2215,2228,
2241,2263,
2285,2290,
2295,2299,
2303,2325,
2347,2350,
2353,2382,
2411,2442,
2473,2519,
2565,2588,
2611,2645,
2679,2713,
2747,2802,
2857,2894,
2931,2945,
2959,2975,
2991,2998,
3005,3011,
3017,3042,
3067,3070,
3072,3074,
3076,3103,
3130,3152,
3174,3205,
3236,3265,
3294,3323,
3352,3377,
3402,3429,
3460,3506,
3557,3598,
3639,3664,
3704,3731,
3773,3779,
3785,3788,
3791,3826,
3876,3881,
3887,3891,
3896,3918,
3948,3989,
4038,4079,
4120,4161,
4202,4209,
4216,4255,
4294,4312,
4330,4391,
4452,4476,
4511,4536,
4572,4576,
4580,4585,
4590,4606,
4619,4636,
4650,4655,
4662,4685,
4706,4736,
4781,4812,
4859,4866,
4874,4888,
4902,4910,
4918,4925,
4932,4937,
4942,4956,
4971,4982,
4994,5005,
5017,5034,
5052,5066,
5081,5095,
5110,5127,
5146,5160,
5176,5190,
5206,5222,
5238,5251,
5264,5277,
5290,5305,
5320,5332,
5344,5356,
5368,5385,
5403,5417,
5432,5446,
5461,5477,
5494,5512,
5532,5548,
5566,5582,
5599,5618,
5637,5656,
5677,5694,
5712,5731,
5752,5771,
5793,5810,
5828,5847,
5865,5883,
5902,5919,
5937,5955,
5976,5993,
6011,6017,
6024,6033,
6043,6052,
6063,6071,
6079,6086,
6093,6096,
6099,6101,
6103,6111,
6122,6147,
6180,6185,
6190,6195,
6200,6235,
6278,6311,
6352,6385,
6424,6472,
6521,6551,
6587,6628,
6677,6703,
6738,6762,
6789,6819,
6848,6896,
6945,6976,
7006,7057,
7104,7127,
7160,7190,
7222,7246,
7275,7300,
7331,7371,
7413,7432,
7454,7477,
7506,7524,
7547,7596,
7650,7676,
7707,7722,
7736,7747,
7768,7783,
7797,7813,
7834,7852,
7882,7895,
7926,7952,
7984,8003,
8028,8062,
8099,8114,
8142,8154,
8168,8180,
8195,8209,
8223,8239,
8255,8267,
8279,8292,
8305,8319,
8331,8346,
8359,8375,
8390,8402,
8414,8426,
8438,8447,
8456,8472,
8488,8499,
8510,8526,
8542,8553,
8564,8584,
8604,8617,
8630,8646,
8662,8673,
8684,8704,
8724,8737,
8750,8770,
8790,8803,
8816,8840,
8864,8879,
8894,8907,
8920,8939,
8959,8981,
9004,9026,
9050,9071,
9092,9112,
9132,9154,
9177,9205,
9235,9266,
9299,9331,
9366,9397,
9429,9459,
9490,9495,
9500,9511,
9523,9537,
9552,9566,
9582,9595,
9608,9620,
9632,9646,
9661,9677,
9695,9714,
9735,9754,
9776,9794,
9813,9830,
9848,9869,
9893,9900,
9907,9917,
9925,9935,
9943,9959,
9973,9989,
10003,10023,
10044,10065,
10087,10109,
10137,10150,
10165,10179,
10198,10210,
10227,10244,
10264,10285,
10317,10335,
10359,10390,
10417,10435,
10468,10486,
10519,10537,
10570,10588,
10621,10639,
10672,10690,
10723,10741,
10774,10792,
10825,10844,
10878,10897,
10931,10950,
10984,10988,
10992,10996,
11000,11003,
11006,11010,
11014,11018,
11022,11025,
11028,11031,
11033,11036,
11038,11040,
11041,11044,
11049,11053,
11057,11061,
11065,11068,
11071,11074,
11078,11081,
11085,11087,
11090,11094,
11098,11117,
11143,11190,
11256,11278,
11307,11326,
11350,11402,
11467,11519,
11581,11647,
11722,11790,
11867,11935,
12012,12080,
12157,12223,
12298,12366,
12443,12511,
12588,12656,
12733,12800,
12876,12945,
13023,13092,
13170,13239,
13317,13384,
13460,13529,
13607,13676,
13754,13823,
13901,13967,
14042,14110,
14187,14255,
14332,14400,
14477,14488,
14499,14519,
14539,14558,
14577,14597,
14617,14634,
14651,14671,
14690,14708,
14727,14758,
14791,14811,
14840,14861,
14887,14918,
14951,14989,
15022,15059,
15096,15134,
15171,15191,
15215,15231,
15249,15333,
15427,15441,
15467,15482,
15508,15519,
15531,15543,
15556,15559,
15562,15565,
15568,15571,
15574,15577,
15580,15583,
15586,15589,
15592,15595,
15598,15601,
15604,15607,
15610,15613,
15616,15619,
15622,15625,
15628,15631,
15634,15637,
15640,15643,
15646,15649,
15652,15655,
15658,15661,
15664,15667,
15670,15684,
15705,15731,
15760,15786,
15816,15836,
15856,15876,
15899,15924,
15955,15964,
15972,15981,
15989,16010,
16043,16066,
16085,16104,
16133,16156,
16192,16203,
16215,16225,
16245,16257,
16275,16290,
16310,16313,
16315,16317,
16319,16343,
16369,16394,
16421,16446,
16474,16500,
16529,16536,
16544,16572,
16606,16624,
16652,16657,
16663,16668,
16674,16677,
16679,16682,
16684,16695,
16707,16717,
16731,16753,
16783,16807,
16843,16865,
16906,16925,
16956,16973,
17004,17013,
17023,17040,
17063,17069,
17077,17081,
17087,17093,
17103,17107,
17115,17121,
17129,17137,
17148,17158,
17171,17182,
17196,17230,
17272,17284,
17296,17314,
17332,17350,
17368,17392,
17428,17452,
17488,17501,
17529,17537,
17546,17556,
17567,17584,
17604,17623,
17643,17663,
17684,17703,
17723,17746,
17777,17802,
17833,17846,
17860,17899,
17945,17949,
17953,17960,
17966,17971,
17976,17987,
17993,18003,
18008,18024,
18045,18050,
18051,18062,
18088,18097,
18102,18108,
18115,18119,
18124,18132,
18141,18150,
18160,18176,
18194,18218,
18253,18270,
18296,18322,
18363,18367,
18371,18387,
18413,18458,
18508,18526,
18545,18555,
18569,18580,
18592,18611,
18632,18645,
18663,18676,
18694,18703,
18717,18746,
18780,18800,
18824,18839,
18855,18870,
18885,18902,
18919,18932,
18949,18959,
18970,18976,
18982,18991,
18999,19010,
19020,19031,
19041,19050,
19056,19064,
19069,19077,
19083,19094,
19104,19117,
19133,19147,
19169,19179,
19190,19212,
19243,19263,
19285,19302,
19324,19335,
19350,19351,
19352,19353,
19354,19355,
19356,19357,
19358,19360,
19362,19371,
19381,19390,
19401,19408,
19414,19425,
19436,19450,
19471,19482,
19493,19504,
19516,19530,
19550,19557,
19564,19573,
19584,19601,
19620,19637,
19653,19670,
19687,19702,
19724,19732,
19741,19752,
19764,19777,
19797,19811,
19828,19836,
19845,19854,
19864,19866,
19868,19877,
19887,19901,
19920,19925,
19932,19935,
19939,19943,
19948,19950,
19952,19956,
19960,19968,
19974,19984,
19994,20002,
20009,20017,
20020,20031,
20041,20050,
20060,20066,
20072,20076,
20086,20104,
20133,20141,
20150,20158,
20167,20171,
20176,20185,
20195,20200,
20206,20222,
20248,20277,
20316,20333,
20361,20384,
20414,20420,
20427,20431,
20438,20442,
20447,20451,
20457,20461,
20473,20478,
20485,20490,
20495,20500,
20506,20511,
20518,20523,
20528,20534,
20542,20549,
20554,20579,
20616,20635,
20666,20678,
20691,20708,
20726,20745,
20759,20774,
20793,20802,
20812,20823,
20829,20856,
20893,20910,
20921,20928,
20935,20939,
20945,20975,
21012,21042,
21079,21114,
21152,21156,
21160,21173,
21193,21203,
21220,21235,
21255,21267,
21280,21285,
21299,21305,
21320,21333,
21352,21358,
21365,21397,
21429,21484,
21552,21565,
21579,21587,
21595,21603,
21611,21620,
21629,21643,
21660,21680,
21705,21726,
21750,21800,
21862,21878,
21895,21911,
21937,21955,
21977,21995,
22013,22038,
22064,22085,
22103,22142,
22178,22204,
22231,22243,
22257,22282,
22319,22329,
22343,22354,
22372,22379,
22387,22413,
22446,22455,
22464,22469,
22476,22495,
22518,22527,
22535,22545,
22554,22559,
22567,22577,
22586,22590,
22597,22627,
22663,22681,
22705,22711,
22717,22748,
22782,22813,
22847,22876,
22908,22926,
22949,22955,
22963,22973,
22989,22997,
23005,23027,
23057,23079,
23111,23133,
23153,23181,
23210,23234,
23264,23279,
23293,23309,
23324,23342,
23360,23407,
23453,23458,
23463,23471,
23480,23520,
23564,23604,
23642,23668,
23704,23721,
23748,23763,
23786,23792,
23798,23802,
23805,23831,
23858,23919,
23991,24017,
24048,24066,
24093,24108,
24124,24134,
24150,24161,
24179,24193,
24214,24230,
24248,24260,
24279,24289,
24305,24312,
24335,24343,
24356,24369,
24387,24398,
24414,24444,
24471,24480,
24492,24516,
24546,24570,
24601,24650,
24696,24723,
24751,24779,
24808,24825,
24840,24850,
24864,24900,
24940,24977,
25018,25055,
25094,25105,
25119,25127,
25142,25166,
25191,25213,
25238,25245,
25252,25275,
25298,25319,
25340,25363,
25388,25410,
25435,25442,
25452,25477,
25505,25516,
25533,25543,
25553,25571,
25591,25608,
25626,25633,
25640,25647,
25653,25661,
25674,25698,
25728,25745,
25765,25779,
25799,25809,
25824,25833,
25847,25902,
25990,26009,
26032,26047,
26069,26082,
26102,26120,
26142,26158,
26178,26187,
26195,26214,
26239,26256,
26276,26296,
26316,26333,
26353,26373,
26393,26408,
26428,26446,
26466,26481,
26501,26519,
26539,26556,
26576,26596,
26616,26631,
26651,26669,
26689,26707,
26726,26751,
26778,26803,
26830,26843,
26856,26884,
26920,26947,
26983,27003,
27031,27060,
27087,27102,
27117,27124,
27134,27164,
27194,27224,
27254,27284,
27314,27339,
27364,27389,
27414,27423,
27433,27458,
27483,27508,
27533,27558,
27583,27608,
27633,27641,
27650,27661,
27673,27687,
27702,27727,
27752,27777,
27802,27827,
27852,27866,
27880,27894,
27908,27927,
27946,27965,
27984,27996,
28007,28026,
28045,28070,
28095,28121,
28147,28172,
28197,28223,
28250,28276,
28302,28330,
28358,28378,
28398,28424,
28450,28467,
28485,28503,
28534,28555,
28589,28611,
28635,28646,
28657,28668,
28684,28697,
28713,28726,
28737,28752,
28770,28785,
28798,28816,
28837,28851,
28867,28885,
28905,28923,
28938,28944,
28950,28956,
28962,28969,
28979,28985,
28994,29000,
29009,29016,
29026,29039,
29054,29077,
29106,29113,
29120,29130,
29140,29163,
29192,29207,
29221,29237,
29253,29268,
29284,29308,
29338,29355,
29372,29387,
29406,29426,
29448,29470,
29516,29540,
29589,29603,
29640,29656,
29672,29689,
29701,29728,
29765,29780,
29790,29812,
29838,29849,
29865,29905,
29957,29974,
29995,30014,
30045,30066,
30092,30111,
30143,30162,
30195,30205,
30214,30219,
30224,30243,
30277,30293,
30308,30322,
30340,30356,
30382,30397,
30422,30488,
30560,30575,
30599,30612,
30633,30646,
30665,30680,
30697,30709,
30722,30741,
30769,30798,
30844,30850,
30860,30908,
30966,30986,
31020,31049,
31099,31114,
31132,31136,
31142,31161,
31184,31190,
31196,31209,
31224,31305,
31394,31423,
31459,31463,
31467,31482,
31502,31515,
31534,31552,
31574,31598,
31620,31650,
31676,31704,
31743,31764,
31791,31807,
31823,31835,
31847,31859,
31871,31876,
31881,31888,
31897,31917,
31937,31963,
31989,31995,
32001,32027,
32053,32081,
32109,32115,
32121,32125,
32129,32142,
32155,32211,
32267,32302,
32337,32348,
32359,32383,
32404,32426,
32448,32469,
32490,32511,
32532,32544,
32555,32568,
32581,32591,
32600,32615,
32629,32639,
32648,32663,
32677,32690,
32702,32716,
32733,32752,
32774,32788,
32800,32819,
32836,32877,
32926,32968,
33022,33035,
33055,33074,
33100,33124,
33159,33169,
33178,33188,
33197,33207,
33216,33226,
33235,33249,
33266,33280,
33297,33311,
33323,33337,
33349,33368,
33386,33405,
33423,33439,
33455,33472,
33489,33498,
33507,33517,
33527,33531,
33535,33558,
33581,33605,
33629,33645,
33661,33680,
33699,33722,
33745,33759,
33773,33790,
33807,33820,
33833,33850,
33867,33896,
33925,33950,
33975,34004,
34033,34057,
34081,34119,
34157,34177
};

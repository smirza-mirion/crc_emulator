#include "crc.h"

const char lang_str[]={
'R','e','v',' ',		// L_REVI (english)  4(0), 0
'R',0xE9,'v',' ',		// L_REVI (french)  4(1)
'R','e','v',':',		// L_PR_REV (english)  4(0), 1
'R',0xE9,'v',' ',':',		// L_PR_REV (french)  5(1)
'R','E','V',' ',' ',' ',' ',' ',' ',' ','S','N',':',' ',		// L_REV_SN (english)  14(0), 2
'R',0xC9,'V',' ',' ',' ',' ',' ',' ',' ','S','N',' ',':',' ',		// L_REV_SN (french)  15(1)
'C','O','P','Y','R','I','G','H','T',		// L_COPYRIGHT (english)  9(0), 3
'D','R','O','I','T','S',' ','D',0x27,'A','U','T','E','U','R',		// L_COPYRIGHT (french)  15(0)
'A','L','L',' ','R','I','G','H','T','S',' ','R','E','S','E','R','V','E','D',		// L_RIGHTS (english)  19(0), 4
'T','O','U','S',' ','D','R','O','I','T','S',' ','R',0xC9,'S','E','R','V',0xC9,'S',		// L_RIGHTS (french)  20(2)
'C','o','n','t','i','n','u','e',		// L_CONTINUE_BTN (english)  8(0), 5
'C','o','n','t','i','n','u','e','r',		// L_CONTINUE_BTN (french)  9(0)
'L','o','g','i','n',		// L_LOGIN (english)  5(0), 6
'C','o','n','n','e','x','i','o','n',		// L_LOGIN (french)  9(0)
'U','p','d','a','t','e',' ','S','y','s','t','e','m',		// L_UPDATE_SYSTEM (english)  13(0), 7
'M','i','s','e',' ',0xE0,' ','j','o','u','r',' ','s','y','s','t',0xE8,'m','e',		// L_UPDATE_SYSTEM (french)  19(2)
'O','F','F',		// L_TIMEOUT_OFF (english)  3(0), 8
'N','O','N',		// L_TIMEOUT_OFF (french)  3(0)
'B','r','i','g','h','t','n','e','s','s',':',		// L_BRIGHTNESS (english)  11(0), 9
'L','u','m','i','n','o','s','i','t',0xE9,' ',':',		// L_BRIGHTNESS (french)  12(1)
'S','l','e','e','p',' ','B','r','i','g','h','t','n','e','s','s',':',		// L_SLEEP_BRIGHTNESS (english)  17(0), 10
'L','u','m','i','n','o','s','i','t',0xE9,' ','v','e','i','l','l','e',' ',':',		// L_SLEEP_BRIGHTNESS (french)  19(1)
'S','l','e','e','p',' ','T','i','m','e','o','u','t',':',		// L_SLEEP_TIMEOUT (english)  14(0), 11
'D',0xE9,'l','a','i',' ','d','e',' ','v','e','i','l','l','e',' ',':',		// L_SLEEP_TIMEOUT (french)  17(1)
'V','o','l','u','m','e',':',		// L_VOLUME (english)  7(0), 12
'V','o','l','u','m','e',' ',':',		// L_VOLUME (french)  8(0)
'S','e','t','u','p',		// L_SETUP_TITLE (english)  5(0), 13
'C','o','n','f','i','g','u','r','a','t','i','o','n',		// L_SETUP_TITLE (french)  13(0)
'A','d','v','a','n','c','e','d',' ','C','h','a','m','b','e','r',		// L_ADVANCED_CHAMBER (english)  16(0), 14
'C','h','a','m','b','r','e',' ','a','v','a','n','c',0xE9,'e',		// L_ADVANCED_CHAMBER (french)  15(1)
'A','d','v','a','n','c','e','d',' ','D','e','t','e','c','t','o','r',		// L_ADVANCED_DETECTOR (english)  17(0), 15
'D',0xE9,'t','e','c','t','e','u','r',' ','a','v','a','n','c',0xE9,		// L_ADVANCED_DETECTOR (french)  16(2)
'S','t','a','f','f',		// L_STAFF (english)  5(0), 16
'P','e','r','s','o','n','n','e','l',		// L_STAFF (french)  9(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','P','a','s','s','w','o','r','d',':',		// L_ENTER_PASSWORD (english)  22(0), 17
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','m','o','t',' ','d','e',' ','p','a','s','s','e',' ',':',		// L_ENTER_PASSWORD (french)  33(0)
'T','e','s','t',		// L_TEST (english)  4(0), 18
'T','e','s','t',		// L_TEST (french)  4(0)
'T','e','s','t',':',		// L_TEST2 (english)  5(0), 19
'T','e','s','t',' ',':',		// L_TEST2 (french)  6(0)
'A','c','t','i','v','i','t','y',' ','U','n','i','t',':',		// L_ACTIVITY_UNIT (english)  14(0), 20
'U','n','i','t',0xE9,' ','d',0x27,'a','c','t','i','v','i','t',0xE9,' ',':',		// L_ACTIVITY_UNIT (french)  18(2)
'D','a','t','e',' ','F','o','r','m','a','t',':',		// L_SETUP_DATE_FORMAT (english)  12(0), 21
'F','o','r','m','a','t',' ','d','e',' ','d','a','t','e',' ',':',		// L_SETUP_DATE_FORMAT (french)  16(0)
'P','r','i','n','t','e','r',':',		// L_SETUP_PRINTER (english)  8(0), 22
'I','m','p','r','i','m','a','n','t','e',' ',':',		// L_SETUP_PRINTER (french)  12(0)
'S','e','t','u','p',' ','S','o','u','r','c','e','s',		// L_SETUP_SOURCES (english)  13(0), 23
'C','o','n','f','i','g','.',' ','s','o','u','r','c','e','s',		// L_SETUP_SOURCES (french)  15(0)
'S','e','t','u','p',' ','M','o','l','y',		// L_SETUP_MOLY (english)  10(0), 24
'C','o','n','f','i','g','.',' ','M','o','l','y',		// L_SETUP_MOLY (french)  12(0)
'S','e','t','u','p',' ','N','u','c','l','i','d','e',		// L_SETUP_NUCLIDES (english)  13(0), 25
'C','o','n','f','i','g','.',' ','n','u','c','l',0xE9,'i','d','e',		// L_SETUP_NUCLIDES (french)  16(1)
'S','e','t','u','p',' ','C','a','l','N','u','m',		// L_SETUP_CALNUM (english)  12(0), 26
'C','o','n','f','i','g','.',' ','C','a','l','N','u','m',		// L_SETUP_CALNUM (french)  14(0)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',		// L_SETUP_LINEARITY (english)  15(0), 27
'C','o','n','f','i','g','.',' ','l','i','n',0xE9,'a','r','i','t',0xE9,		// L_SETUP_LINEARITY (french)  17(2)
'S','e','t','u','p',' ','R','e','m','o','t','e',		// L_SETUP_REMOTE (english)  12(0), 28
'C','o','n','f','i','g','.',' ','d','i','s','t','a','n','t',		// L_SETUP_REMOTE (french)  15(0)
'S','e','t','u','p',' ','R',' ','C','h','a','m','b','e','r',' ','H','o','t','k','e','y','s',		// L_SETUP_R_HOTKEYS (english)  23(0), 29
'C','o','n','f','i','g','.',' ','r','a','c','c','o','u','r','c','i','s',' ','c','h','a','m','b','r','e',' ','R',		// L_SETUP_R_HOTKEYS (french)  28(0)
'S','e','t','u','p',' ','P','E','T',' ','C','h','a','m','b','e','r',' ','H','o','t','k','e','y','s',		// L_SETUP_PET_HOTKEYS (english)  25(0), 30
'C','o','n','f','i','g','.',' ','r','a','c','c','o','u','r','c','i','s',' ','c','h','a','m','b','r','e',' ','P','E','T',		// L_SETUP_PET_HOTKEYS (french)  30(0)
'A','d','v','a','n','c','e','d',' ','C','h','a','m','b','e','r',' ','S','e','t','u','p',		// L_ADVANCED_CHAMBER_SETUP_TITLE (english)  22(0), 31
'C','o','n','f','i','g','.',' ','c','h','a','m','b','r','e',' ','a','v','a','n','c',0xE9,'e',		// L_ADVANCED_CHAMBER_SETUP_TITLE (french)  23(1)
'A','C','T','I','V','I','T','Y',		// L_SETUP_SOURCES_ACTIVITY (english)  8(0), 32
'A','C','T','I','V','I','T',0xC9,		// L_SETUP_SOURCES_ACTIVITY (french)  8(1)
'D','A','T','E',		// L_SETUP_SOURCES_DATE (english)  4(0), 33
'D','A','T','E',		// L_SETUP_SOURCES_DATE (french)  4(0)
'D','A','I','L','Y',		// L_SETUP_SOURCES_DAILY (english)  5(0), 34
'Q','U','O','T','I','D','I','E','N',		// L_SETUP_SOURCES_DAILY (french)  9(0)
'C','o','n','s','t','a','n','c','y',' ','C','h','a','n','n','e','l','s',		// L_SETUP_SOURCES_CONSTANCY_CHANNELS (english)  18(0), 35
'C','a','n','a','u','x',' ','d','e',' ','c','o','n','s','t','a','n','c','e',		// L_SETUP_SOURCES_CONSTANCY_CHANNELS (french)  19(0)
'C','l','e','a','r',		// L_CLEAR_BUTTON (english)  5(0), 36
'E','f','f','a','c','e','r',		// L_CLEAR_BUTTON (french)  7(0)
'A','c','c','e','p','t',		// L_ACCEPT_BUTTON (english)  6(0), 37
'A','c','c','e','p','t','e','r',		// L_ACCEPT_BUTTON (french)  8(0)
'C','a','n','c','e','l',		// L_CANCEL_BUTTON (english)  6(0), 38
'A','n','n','u','l','e','r',		// L_CANCEL_BUTTON (french)  7(0)
'M','o','l','y',' ','M','e','t','h','o','d',':',		// L_SETUP_MOLY_METHOD (english)  12(0), 39
'M',0xE9,'t','h','o','d','e',' ','M','o','l','y',' ',':',		// L_SETUP_MOLY_METHOD (french)  14(1)
'M','o','/','T','c',' ','L','i','m','i','t',':',		// L_SETUP_MOLY_LIMIT (english)  12(0), 40
'L','i','m','i','t','e',' ','M','o','/','T','c',' ',':',		// L_SETUP_MOLY_LIMIT (french)  14(0)
'N','u','c','l','i','d','e',		// L_NUCLIDE (english)  7(0), 41
'N','u','c','l',0xE9,'i','d','e',		// L_NUCLIDE (french)  8(1)
'E','l','e','m','e','n','t',		// L_SETUP_NUCLIDE_ELEMENT (english)  7(0), 42
0xC9,'l',0xE9,'m','e','n','t',		// L_SETUP_NUCLIDE_ELEMENT (french)  7(2)
'H','a','l','f','l','i','f','e',		// L_SETUP_NUCLIDE_HALFLIFE (english)  8(0), 43
'D','e','m','i','-','v','i','e',		// L_SETUP_NUCLIDE_HALFLIFE (french)  8(0)
'E','n','g','l','i','s','h',		// L_ENGLISH (english)  7(0), 44
'A','n','g','l','a','i','s',		// L_ENGLISH (french)  7(0)
'F','r','e','n','c','h',		// L_FRENCH (english)  6(0), 45
'F','r','a','n',0xE7,'a','i','s',		// L_FRENCH (french)  8(1)
'L','a','n','g','u','a','g','e',':',		// L_LANGUAGE (english)  9(0), 46
'L','a','n','g','u','e',' ',':',		// L_LANGUAGE (french)  8(0)
'D','e','f','a','u','l','t',		// L_DEFAULT (english)  7(0), 47
'P','a','r',' ','d',0xE9,'f','a','u','t',		// L_DEFAULT (french)  10(1)
'U','s','e','r',		// L_USER (english)  4(0), 48
'U','t','i','l','i','s','a','t','e','u','r',		// L_USER (french)  11(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','N','u','c','l','i','d','e',		// L_ENTER_NUCLIDE (english)  20(0), 49
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','n','u','c','l',0xE9,'i','d','e',		// L_ENTER_NUCLIDE (french)  27(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','E','l','e','m','e','n','t',		// L_ENTER_ELEMENT (english)  20(0), 50
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l',0x27,0xE9,'l',0xE9,'m','e','n','t',		// L_ENTER_ELEMENT (french)  25(2)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','H','a','l','f','l','i','f','e',		// L_ENTER_HALFLIFE (english)  21(0), 51
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','a',' ','d','e','m','i','-','v','i','e',		// L_ENTER_HALFLIFE (french)  27(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','R',' ','C','a','l',' ','#',':',		// L_ENTER_RCAL (english)  21(0), 52
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','R',' ','C','a','l',' ','#',' ',':',		// L_ENTER_RCAL (french)  25(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','P',' ','C','a','l',' ','#',':',		// L_ENTER_PCAL (english)  21(0), 53
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','P',' ','C','a','l',' ','#',' ',':',		// L_ENTER_PCAL (french)  25(0)
'B','a','c','k','s','p','a','c','e',		// L_BACKSPACE (english)  9(0), 54
'R','e','t','o','u','r',' ','a','r','r','i',0xE8,'r','e',		// L_BACKSPACE (french)  14(1)
'B','a','c','k','S','P',		// L_BACKSPACE_SHORT (english)  6(0), 55
'R','e','t','o','u','r',		// L_BACKSPACE_SHORT (french)  6(0)
'S','h','i','f','t',		// L_SHIFT (english)  5(0), 56
'S','h','i','f','t',		// L_SHIFT (french)  5(0)
'<','-','-',' ','W','i','l','d','c','a','r','d',		// L_WILDCARD (english)  12(0), 57
'<','-','-',' ','J','o','k','e','r',		// L_WILDCARD (french)  9(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',		// L_PLEASE_ENTER (english)  12(0), 58
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',		// L_PLEASE_ENTER (french)  15(0)
'S','/','N',		// L_SN (english)  3(0), 59
'S','/','N',		// L_SN (french)  3(0)
'S','/','N',':',		// L_SN2 (english)  4(0), 60
'S','/','N',' ',':',		// L_SN2 (french)  5(0)
'C','a','l','i','b','r','a','t','i','o','n',' ','T','i','m','e',		// L_CALIBRATION_TIME (english)  16(0), 61
'H','e','u','r','e',' ','d','e',' ','c','a','l','i','b','r','a','t','i','o','n',		// L_CALIBRATION_TIME (french)  20(0)
'A','c','t','i','v','i','t','y',		// L_ACTIVITY (english)  8(0), 62
'A','c','t','i','v','i','t',0xE9,		// L_ACTIVITY (french)  8(1)
'S','t','a','n','d','a','r','d',' ','R',' ','C','h','a','m','b','e','r',		// L_STANDARD_R_CHAMBER (english)  18(0), 63
'S','t','a','n','d','a','r','d',' ','c','h','a','m','b','r','e',' ','R',		// L_STANDARD_R_CHAMBER (french)  18(0)
'S','t','a','n','d','a','r','d',' ','P','E','T',' ','C','h','a','m','b','e','r',		// L_STANDARD_PET_CHAMBER (english)  20(0), 64
'S','t','a','n','d','a','r','d',' ','c','h','a','m','b','r','e',' ','P','E','T',		// L_STANDARD_PET_CHAMBER (french)  20(0)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','S','t','a','n','d','a','r','d',' ','-',' ','R',' ','C','h','a','m','b','e','r',		// L_SETUP_LINEARITY_STANDARD_R (english)  36(0), 65
'C','o','n','f','i','g','.',' ','s','t','a','n','d','a','r','d',' ','l','i','n',0xE9,'a','r','i','t',0xE9,' ','-',' ','c','h','a','m','b','r','e',' ','R',		// L_SETUP_LINEARITY_STANDARD_R (french)  38(2)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','S','t','a','n','d','a','r','d',' ','-',' ','P','E','T',' ','C','h','a','m','b','e','r',		// L_SETUP_LINEARITY_STANDARD_PET (english)  38(0), 66
'C','o','n','f','i','g','.',' ','s','t','a','n','d','a','r','d',' ','l','i','n',0xE9,'a','r','i','t',0xE9,' ','-',' ','c','h','a','m','b','r','e',' ','P','E','T',		// L_SETUP_LINEARITY_STANDARD_PET (french)  40(2)
'N','u','m',' ','o','f',' ','M','e','a','s','u','r','e','m','e','n','t','s',':',		// L_NUM_OF_MEASUREMENTS (english)  20(0), 67
'N','b',' ','d','e',' ','m','e','s','u','r','e','s',' ',':',		// L_NUM_OF_MEASUREMENTS (french)  15(0)
'M','e','a','s','u','r','e','d',' ','O','n',		// L_MEASURED_ON (english)  11(0), 68
'M','e','s','u','r',0xE9,' ','l','e',		// L_MEASURED_ON (french)  9(1)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','E','r','r','o','r',		// L_SETUP_LINEARITY_ERROR (english)  21(0), 69
'E','r','r','e','u','r',' ','c','o','n','f','i','g','.',' ','l','i','n',0xE9,'a','r','i','t',0xE9,		// L_SETUP_LINEARITY_ERROR (french)  24(2)
'P','l','e','a','s','e',' ','F','i','l','l',' ','i','n',' ','A','l','l',' ','H','o','u','r','s',		// L_FILL_IN_HOURS (english)  24(0), 70
'V','e','u','i','l','l','e','z',' ','r','e','m','p','l','i','r',' ','t','o','u','t','e','s',' ','l','e','s',' ','h','e','u','r','e','s',		// L_FILL_IN_HOURS (french)  34(0)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','h','o','u','r','s',' ','i','n',' ','a','s','c','e','n','d','i','n','g',' ','o','r','d','e','r',		// L_ASCENDING_ORDER (english)  37(0), 71
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e','s',' ','h','e','u','r','e','s',' ','e','n',' ','o','r','d','r','e',' ','c','r','o','i','s','s','a','n','t',		// L_ASCENDING_ORDER (french)  45(0)
'O','K',		// L_CAPS_OK (english)  2(0), 72
'O','K',		// L_CAPS_OK (french)  2(0)
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','W','i','p','e',' ','C','r','i','t','e','r','i','a',		// L_SELECT_WIPE_CRITERIA (english)  27(0), 73
'S',0xE9,'l','e','c','t','i','o','n','n','e','z',' ','l','e','s',' ','c','r','i','t',0xE8,'r','e','s',' ','d','e',' ','f','r','o','t','t','i','s',		// L_SELECT_WIPE_CRITERIA (french)  36(2)
'A','l','l',' ','W','i','p','e','s',		// L_ALL_WIPES (english)  9(0), 74
'T','o','u','s',' ','l','e','s',' ','f','r','o','t','t','i','s',		// L_ALL_WIPES (french)  16(0)
'W','o','r','k',' ','A','r','e','a',		// L_WORK_AREA (english)  9(0), 75
'Z','o','n','e',' ','d','e',' ','t','r','a','v','a','i','l',		// L_WORK_AREA (french)  15(0)
'U','n','r','e','s','t','r','i','c','t','e','d',' ','A','r','e','a',		// L_UNRESTRICTED_AREA (english)  17(0), 76
'Z','o','n','e',' ','n','o','n',' ','r','e','s','t','r','e','i','n','t','e',		// L_UNRESTRICTED_AREA (french)  19(0)
'S','e','a','l','e','d',' ','S','o','u','r','c','e',		// L_SEALED_SOURCE (english)  13(0), 77
'S','o','u','r','c','e',' ','s','c','e','l','l',0xE9,'e',		// L_SEALED_SOURCE (french)  14(1)
'P','a','c','k','a','g','e',		// L_PACKAGE (english)  7(0), 78
'C','o','l','i','s',		// L_PACKAGE (french)  5(0)
'H','i','g','h',' ','A','c','t','i','v','i','t','y',		// L_HIGH_ACTIVITY (english)  13(0), 79
'H','a','u','t','e',' ','a','c','t','i','v','i','t',0xE9,		// L_HIGH_ACTIVITY (french)  14(1)
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','D','e','t','e','c','t','o','r',		// L_SELECT_DETECTOR (english)  22(0), 80
'V','e','u','i','l','l','e','z',' ','s',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','l','e',' ','d',0xE9,'t','e','c','t','e','u','r',		// L_SELECT_DETECTOR (french)  34(2)
'P','r','o','b','e',		// L_PROBE (english)  5(0), 81
'S','o','n','d','e',		// L_PROBE (french)  5(0)
'W','e','l','l',		// L_WELL (english)  4(0), 82
'P','u','i','t','s',		// L_WELL (french)  5(0)
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','B','r','a','n','d','i','n','g',		// L_SELECT_BRANDING (english)  22(0), 83
'V','e','u','i','l','l','e','z',' ','s',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','l','a',' ','m','a','r','q','u','e',		// L_SELECT_BRANDING (french)  31(1)
'A','l','l',		// L_ALL (english)  3(0), 84
'T','o','u','t',		// L_ALL (french)  4(0)
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','C','o','u','n','t','i','n','g',' ','M','e','t','h','o','d',		// L_SELECT_TU_METHOD (english)  29(0), 85
'V','e','u','i','l','l','e','z',' ','s',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','l','a',' ','m',0xE9,'t','h','o','d','e',' ','d','e',' ','c','o','m','p','t','a','g','e',		// L_SELECT_TU_METHOD (french)  44(2)
'D','e','c','a','y',' ','C','o','r','r','e','c','t',' ','A','d','m','i','n','i','s','t','e','r','e','d',' ','D','o','s','e',		// L_DECAY_CORRECT_ADMIN_DOSE (english)  31(0), 86
'C','o','r','r','i','g','e','r',' ','d',0xE9,'c','r','o','i','s','s','a','n','c','e',' ','d','o','s','e',' ','a','d','m','i','n','i','s','t','r',0xE9,'e',		// L_DECAY_CORRECT_ADMIN_DOSE (french)  38(2)
'M','e','a','s','u','r','e',' ','S','a','m','e',' ','R','e','f','e','r','e','n','c','e',' ','D','o','s','e',' ','B','e','f','o','r','e',' ','E','a','c','h',' ','U','p','t','a','k','e',		// L_MEAS_REF_DOSE (english)  46(0), 87
'M','e','s','u','r','e','r',' ','l','a',' ','m',0xEA,'m','e',' ','d','o','s','e',' ','r',0xE9,'f','.',' ','a','v','a','n','t',' ','c','h','a','q','u','e',' ','c','a','p','t','a','t','i','o','n',		// L_MEAS_REF_DOSE (french)  48(2)
'S','e','l','e','c','t',' ','D','o','s','e',' ','M','e','a','s','u','r','e','m','e','n','t',		// L_SELECT_DOSE_MEASUREMENT (english)  23(0), 88
'S',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','m','e','s','u','r','e',' ','d','e',' ','d','o','s','e',		// L_SELECT_DOSE_MEASUREMENT (french)  27(1)
'D','o','s','e',' ','M','e','a','s','u','r','e','d',' ','i','s',' ','D','o','s','e',' ','A','d','m','i','n','i','s','t','e','r','e','d',		// L_DOSE_MEAS_IS_DOSE_ADMIN (english)  34(0), 89
'D','o','s','e',' ','m','e','s','u','r',0xE9,'e',' ','e','s','t',' ','d','o','s','e',' ','a','d','m','i','n','i','s','t','r',0xE9,'e',		// L_DOSE_MEAS_IS_DOSE_ADMIN (french)  33(2)
'M','e','a','s','u','r','e',' ','E','a','c','h',' ','D','o','s','e',' ','a','n','d',' ','A','d','d',' ','A','c','t','i','v','i','t','y',		// L_MEAS_DOSE_AND_ADD (english)  34(0), 90
'M','e','s','u','r','e','r',' ','c','h','a','q','u','e',' ','d','o','s','e',' ','e','t',' ','a','j','o','u','t','e','r',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,		// L_MEAS_DOSE_AND_ADD (french)  41(1)
'M','e','a','s','u','r','e',' ','O','n','e',' ','C','a','p','s','u','l','e',' ','a','n','d',' ','M','u','l','t','i','p','l','y',' ','b','y',' ','N','u','m','b','e','r',' ','A','d','m','i','n','i','s','t','e','r','e','d',		// L_MEAS_DOSE_AND_MULTIP (english)  55(0), 91
'M','e','s','u','r','e','r',' ','u','n','e',' ','c','a','p','s','u','l','e',' ','e','t',' ','m','u','l','t','i','p','l','i','e','r',' ','p','a','r',' ','l','e',' ','n','o','m','b','r','e',' ','a','d','m','i','n','i','s','t','r',0xE9,		// L_MEAS_DOSE_AND_MULTIP (french)  58(1)
'M','e','a','s','u','r','e',' ','L','i','q','u','i','d',' ','a','n','d',' ','M','u','l','t','i','p','l','y',' ','b','y',' ','F','a','c','t','o','r',		// L_MEAS_LIQ_AND_MULTIP (english)  37(0), 92
'M','e','s','u','r','e','r',' ','l','e',' ','l','i','q','u','i','d','e',' ','e','t',' ','m','u','l','t','i','p','l','i','e','r',' ','p','a','r',' ','l','e',' ','f','a','c','t','e','u','r',		// L_MEAS_LIQ_AND_MULTIP (french)  47(0)
'S','e','l','e','c','t',' ','N','u','c','l','i','d','e',		// L_SELECT_NUCLIDE (english)  14(0), 93
'S',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','n','u','c','l',0xE9,'i','d','e',		// L_SELECT_NUCLIDE (french)  21(2)
'S','e','l','e','c','t',' ','D','o','s','e',' ','F','o','r','m',		// L_SELECT_DOSE_FORM (english)  16(0), 94
'S',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','f','o','r','m','e',' ','d','e',' ','d','o','s','e',		// L_SELECT_DOSE_FORM (french)  26(1)
'C','a','p','s','u','l','e',		// L_CAPSULE (english)  7(0), 95
'C','a','p','s','u','l','e',		// L_CAPSULE (french)  7(0)
'L','i','q','u','i','d',		// L_LIQUID (english)  6(0), 96
'L','i','q','u','i','d','e',		// L_LIQUID (french)  7(0)
'M','e','a','s','u','r','e',' ','P','r','e','-','D','o','s','e',' ','P','a','t','i','e','n','t','?',		// L_MEAS_PREDOSE_PATIENT (english)  25(0), 97
'M','e','s','u','r','e','r',' ','p','a','t','i','e','n','t',' ','p','r',0xE9,'-','d','o','s','e',' ','?',		// L_MEAS_PREDOSE_PATIENT (french)  26(1)
'Y','e','s',		// L_YES (english)  3(0), 98
'O','u','i',		// L_YES (french)  3(0)
'N','o',		// L_NO (english)  2(0), 99
'N','o','n',		// L_NO (french)  3(0)
'D','e','l','e','t','e',' ','T','h','y','r','o','i','d',' ','U','p','t','a','k','e',' ','T','e','s','t','?',		// L_DELETE_THYROID (english)  27(0), 100
'S','u','p','p','r','i','m','e','r',' ','t','e','s','t',' ','c','a','p','t','a','t','i','o','n',' ','t','h','y','r','o',0xEF,'d','e',' ','?',		// L_DELETE_THYROID (french)  35(1)
'M','e','a','s','u','r','e',' ','R','e','s','i','d','u','a','l',' ','D','o','s','e','?',		// L_MEAS_REDISUAL (english)  22(0), 101
'M','e','s','u','r','e','r',' ','d','o','s','e',' ','r',0xE9,'s','i','d','u','e','l','l','e',' ','?',		// L_MEAS_REDISUAL (french)  25(1)
'R','e','A','c','t','i','v','a','t','e',' ','T','h','y','r','o','i','d',' ','U','p','t','a','k','e',' ','T','e','s','t','?',		// L_REACTIVATE_TU (english)  31(0), 102
'R',0xE9,'a','c','t','i','v','e','r',' ','t','e','s','t',' ','c','a','p','t','a','t','i','o','n',' ','t','h','y','r','o',0xEF,'d','e',' ','?',		// L_REACTIVATE_TU (french)  35(2)
'U','s','e',' ','O','n','l','y',' ','D','e','f','a','u','l','t',' ','N','u','c','l','i','d','e',' ','k','e','V','?',		// L_ONLY_DEFAULT_NUC (english)  29(0), 103
'U','t','i','l','i','s','e','r',' ','n','u','c','l',0xE9,'i','d','e',' ','k','e','V',' ','p','a','r',' ','d',0xE9,'f','a','u','t',' ','s','e','u','l',' ','?',		// L_ONLY_DEFAULT_NUC (french)  39(2)
'R','e','A','c','t','i','v','a','t','e',' ','R','B','C',' ','S','u','r','v','i','v','a','l',' ','T','e','s','t','?',		// L_REACTIVATE_RBC (english)  29(0), 104
'R',0xE9,'a','c','t','i','v','e','r',' ','t','e','s','t',' ','d','e',' ','s','u','r','v','i','e',' ','R','B','C',' ','?',		// L_REACTIVATE_RBC (french)  30(1)
'D','e','l','e','t','e',' ','R','B','C',' ','S','u','r','v','i','v','a','l',' ','T','e','s','t','?',		// L_DELETE_RBC (english)  25(0), 105
'S','u','p','p','r','i','m','e','r',' ','t','e','s','t',' ','d','e',' ','s','u','r','v','i','e',' ','R','B','C',' ','?',		// L_DELETE_RBC (french)  30(0)
'U','p','d','a','t','e',' ','t','o',' ','R','e','v',':',' ','%','s',',',' ','P','r','o','c','e','e','d','?',		// L_UPDATE_TO_REV (english)  27(0), 106
'M','i','s','e',' ',0xE0,' ','j','o','u','r',' ','r',0xE9,'v',' ',':',' ','%','s',',',' ','c','o','n','t','i','n','u','e','r',' ','?',		// L_UPDATE_TO_REV (french)  33(2)
'T','h','i','s',' ','v','e','r','s','i','o','n',' ','w','i','l','l',' ','d','i','s','a','b','l','e',' ','U','S','B',' ','U','p','d','a','t','e','.',' ','P','r','o','c','e','e','d','?',		// L_UPDATE_DISABLES_USB (english)  46(0), 107
'C','e','t','t','e',' ','v','e','r','s','i','o','n',' ','d',0xE9,'s','a','c','t','i','v','e',' ','l','a',' ','m','i','s','e',' ',0xE0,' ','j','o','u','r',' ','U','S','B','.',' ','C','o','n','t','i','n','u','e','r',' ','?',		// L_UPDATE_DISABLES_USB (french)  55(2)
'O','v','e','r','w','r','i','t','e',' ','s','e','t','t','i','n','g','s',' ','w','i','t','h',' ','d','e','f','a','u','l','t','s',' ','f','r','o','m',' ','%','s','?',		// L_OVERWRITE_SETTING_WITH_DEFAULT (english)  41(0), 108
0xC9,'c','r','a','s','e','r',' ','l','e','s',' ','r',0xE9,'g','l','a','g','e','s',' ','p','a','r',' ','d',0xE9,'f','a','u','t',' ','d','e',' ','%','s',' ','?',		// L_OVERWRITE_SETTING_WITH_DEFAULT (french)  39(3)
'P','l','e','a','s','e',' ','S','e','l','e','c','t',' ','D','a','t','e',' ','F','o','r','m','a','t',		// L_SELECT_DATE_FORMAT (english)  25(0), 109
'V','e','u','i','l','l','e','z',' ','s',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','l','e',' ','f','o','r','m','a','t',' ','d','e',' ','d','a','t','e',		// L_SELECT_DATE_FORMAT (french)  39(1)
'P','l','e','a','s','e',' ','S','e','l','e','c','t',' ','U','S','B',' ','P','C',' ','D','r','i','v','e','r',		// L_SELECT_USB_DRIVER (english)  27(0), 110
'V','e','u','i','l','l','e','z',' ','s',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','l','e',' ','p','i','l','o','t','e',' ','U','S','B',' ','P','C',		// L_SELECT_USB_DRIVER (french)  38(1)
'L','e','g','a','c','y',		// L_LEGACY (english)  6(0), 111
'A','n','c','i','e','n',		// L_LEGACY (french)  6(0)
'C','D','C',		// L_CDC (english)  3(0), 112
'C','D','C',		// L_CDC (french)  3(0)
'P','l','e','a','s','e',' ','S','e','l','e','c','t',' ','D','o','s','e',' ','D','e','c','a','y',' ','E','n','t','r','y',' ','M','o','d','e',		// L_SELECT_DOSE_DECAY_ENTRY_MODE (english)  35(0), 113
'S',0xE9,'l','e','c','t','i','o','n','n','e','z',' ','l','e',' ','m','o','d','e',' ','s','a','i','s','i','e',' ','d',0xE9,'c','r','o','i','s','s','a','n','c','e',		// L_SELECT_DOSE_DECAY_ENTRY_MODE (french)  40(2)
'Q','u','i','c','k',		// L_QUICK (english)  5(0), 114
'R','a','p','i','d','e',		// L_QUICK (french)  6(0)
'F','u','l','l',		// L_FULL (english)  4(0), 115
'C','o','m','p','l','e','t',		// L_FULL (french)  7(0)
'P','l','e','a','s','e',' ','S','e','l','e','c','t',' ','L','a','n','g','u','a','g','e',		// L_SELECT_LANGUAGE (english)  22(0), 116
'V','e','u','i','l','l','e','z',' ','s',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','l','a',' ','l','a','n','g','u','e',		// L_SELECT_LANGUAGE (french)  31(1)
'C','R','C','-','5','5','t',' ',' ',' ',' ',' ',' ','R','A','D','I','O','I','S','O','T','O','P','E',' ','D','O','S','E',' ','C','A','L','I','B','R','A','T','O','R',		// L_55T_FULL_TITLE (english)  41(0), 117
'C','R','C','-','5','5','t',' ',' ',' ',' ',' ',' ','C','A','L','I','B','R','A','T','E','U','R',' ','D','E',' ','D','O','S','E',' ','R','A','D','I','O','I','S','O','T','O','P','E',		// L_55T_FULL_TITLE (french)  45(0)
'C','A','P','R','A','C','-','t',' ',' ',' ',' ',' ',' ',' ',' ','R','A','D','I','O','I','S','O','T','O','P','E',' ','W','E','L','L',' ','C','O','U','N','T','E','R',		// L_CAPRAC_FULL_TITLE (english)  41(0), 118
'C','A','P','R','A','C','-','t',' ',' ',' ',' ',' ',' ',' ',' ','C','O','M','P','T','E','U','R',' ','P','U','I','T','S',' ','R','A','D','I','O','I','S','O','T','O','P','E',		// L_CAPRAC_FULL_TITLE (french)  43(0)
'C','A','P','T','U','S','-','7','0','0','t',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','R','A','D','I','O','I','S','O','T','O','P','E',' ','C','O','U','N','T','E','R',		// L_700T_FULL_TITLE (english)  41(0), 119
'C','A','P','T','U','S','-','7','0','0','t',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','C','O','M','P','T','E','U','R',' ','R','A','D','I','O','I','S','O','T','O','P','E',		// L_700T_FULL_TITLE (french)  42(0)
'F','a','c','t','o','r','y',		// L_FACTORY (english)  7(0), 120
'U','s','i','n','e',		// L_FACTORY (french)  5(0)
'S','e','t','t','i','n','g',' ','h','a','s',' ','b','e','e','n',' ','s','a','v','e','d','.',' ','P','l','e','a','s','e',' ','r','e','s','t','a','r','t','.',		// L_PLEASE_RESTART (english)  39(0), 121
'R',0xE9,'g','l','a','g','e',' ','e','n','r','e','g','i','s','t','r',0xE9,'.',' ','V','e','u','i','l','l','e','z',' ','r','e','d',0xE9,'m','a','r','r','e','r','.',		// L_PLEASE_RESTART (french)  40(3)
'W','i','p','e',' ','T','y','p','e',' ','D','e','f','a','u','l','t','s',		// L_WIPE_TYPE_DEFAULTS (english)  18(0), 122
'D',0xE9,'f','a','u','t','s',' ','t','y','p','e',' ','d','e',' ','f','r','o','t','t','i','s',		// L_WIPE_TYPE_DEFAULTS (french)  23(1)
'C','u','r','r','e','n','t',' ','s','e','t','t','i','n','g','s',' ','h','a','v','e',' ','b','e','e','n',' ','o','v','e','r','w','r','i','t','t','e','n',' ','w','i','t','h',' ','%','s',' ','d','e','f','a','u','l','t',' ','v','a','l','u','e','s',		// L_WIPE_OVERWRITE_STRING (english)  61(0), 123
'L','e','s',' ','r',0xE9,'g','l','a','g','e','s',' ','o','n','t',' ',0xE9,'t',0xE9,' ',0xE9,'c','r','a','s',0xE9,'s',' ','a','v','e','c',' ','l','e','s',' ','v','a','l','e','u','r','s',' ','p','a','r',' ','d',0xE9,'f','a','u','t',' ','%','s',		// L_WIPE_OVERWRITE_STRING (french)  59(6)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','L','i','n','e','a','t','o','r',		// L_SETUP_LINEARITY_LINEATOR (english)  24(0), 124
'C','o','n','f','i','g','.',' ','l','i','n',0xE9,'a','r','i','t',0xE9,' ','L','i','n','e','a','t','o','r',		// L_SETUP_LINEARITY_LINEATOR (french)  26(2)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','C','a','l','i','c','h','e','c','k',		// L_SETUP_LINEARITY_CALICHECK (english)  25(0), 125
'C','o','n','f','i','g','.',' ','l','i','n',0xE9,'a','r','i','t',0xE9,' ','C','a','l','i','c','h','e','c','k',		// L_SETUP_LINEARITY_CALICHECK (french)  27(2)
'T','u','b','e',		// L_TUBE (english)  4(0), 126
'T','u','b','e',		// L_TUBE (french)  4(0)
'T','u','b','e',':',		// L_TUBE2 (english)  5(0), 127
'T','u','b','e',' ',':',		// L_TUBE2 (french)  6(0)
'L','i','n','e','a','t','o','r',' ','S','e','r','i','a','l',':',		// L_LINEATOR_SERIAL (english)  16(0), 128
'L','i','n','e','a','t','o','r',' ','S','/','N',' ',':',		// L_LINEATOR_SERIAL (french)  14(0)
'C','a','l','i','c','h','e','c','k',' ','S','e','r','i','a','l',':',		// L_CALICHECK_SERIAL (english)  17(0), 129
'C','a','l','i','c','h','e','c','k',' ','S','/','N',' ',':',		// L_CALICHECK_SERIAL (french)  15(0)
'P','r','i','n','t',		// L_PRINT (english)  5(0), 130
'I','m','p','r','i','m','e','r',		// L_PRINT (french)  8(0)
'D','e','l','e','t','e',' ','C','u','r','r','e','n','t',' ','S','e','t','t','i','n','g','s',		// L_DELETE_CURRENT_SETTINGS (english)  23(0), 131
'S','u','p','p','r','i','m','e','r',' ','r',0xE9,'g','l','a','g','e','s',' ','a','c','t','u','e','l','s',		// L_DELETE_CURRENT_SETTINGS (french)  26(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','L','i','n','e','a','t','o','r',' ','S','e','r','i','a','l',' ','#',		// L_PLEASE_ENTER_LINEATOR_SN (english)  30(0), 132
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','#',' ','d','e',' ','s',0xE9,'r','i','e',' ','L','i','n','e','a','t','o','r',		// L_PLEASE_ENTER_LINEATOR_SN (french)  38(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','C','a','l','i','c','h','e','c','k',' ','S','e','r','i','a','l',' ','#',		// L_PLEASE_ENTER_CALICHECK_SN (english)  31(0), 133
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','#',' ','d','e',' ','s',0xE9,'r','i','e',' ','C','a','l','i','c','h','e','c','k',		// L_PLEASE_ENTER_CALICHECK_SN (french)  39(1)
'N','U','C','L','I','D','E',		// L_PR_NUCLIDE (english)  7(0), 134
'N','U','C','L',0xC9,'I','D','E',		// L_PR_NUCLIDE (french)  8(1)
'C','h','a','m','b','e','r',' ',' ',' ','S','/','N',':',		// L_PR_CHAMBER_SN (english)  14(0), 135
'C','h','a','m','b','r','e',' ',' ',' ','S','/','N',' ',':',		// L_PR_CHAMBER_SN (french)  15(0)
'1',')','B','l','a','c','k',':',		// L_CAL_1 (english)  8(0), 136
'1',')','N','o','i','r',' ',':',		// L_CAL_1 (french)  8(0)
'1',')','B','l','a','c','k',		// L_CAL1 (english)  7(0), 137
'1',')','N','o','i','r',		// L_CAL1 (french)  6(0)
'B','l','a','c','k',		// L_CAL__1 (english)  5(0), 138
'N','o','i','r',		// L_CAL__1 (french)  4(0)
'2',')','B','l','a','c','k',' ','+',' ','R','e','d',':',		// L_CAL_2 (english)  14(0), 139
'2',')','N','o','i','r',' ','+',' ','R','o','u','g','e',' ',':',		// L_CAL_2 (french)  16(0)
'2',')','B','l','a','c','k','/','R','e','d',		// L_CAL2 (english)  11(0), 140
'2',')','N','o','i','r','/','R','o','u','g','e',		// L_CAL2 (french)  12(0)
'B','l','a','c','k',' ','+',' ','R','e','d',		// L_CAL__2 (english)  11(0), 141
'N','o','i','r',' ','+',' ','R','o','u','g','e',		// L_CAL__2 (french)  12(0)
'3',')','B','l','a','c','k',' ','+',' ','O','r','a','n','g','e',':',		// L_CAL_3 (english)  17(0), 142
'3',')','N','o','i','r',' ','+',' ','O','r','a','n','g','e',' ',':',		// L_CAL_3 (french)  17(0)
'3',')','B','l','a','c','k','/','O','r','a','n','g','e',		// L_CAL3 (english)  14(0), 143
'3',')','N','o','i','r','/','O','r','a','n','g','e',		// L_CAL3 (french)  13(0)
'B','l','a','c','k',' ','+',' ','O','r','a','n','g','e',		// L_CAL__3 (english)  14(0), 144
'N','o','i','r',' ','+',' ','O','r','a','n','g','e',		// L_CAL__3 (french)  13(0)
'4',')','B','l','a','c','k',' ','+',' ','Y','e','l','l','o','w',':',		// L_CAL_4 (english)  17(0), 145
'4',')','N','o','i','r',' ','+',' ','J','a','u','n','e',' ',':',		// L_CAL_4 (french)  16(0)
'4',')','B','l','a','c','k','/','Y','e','l','l','o','w',		// L_CAL4 (english)  14(0), 146
'4',')','N','o','i','r','/','J','a','u','n','e',		// L_CAL4 (french)  12(0)
'B','l','a','c','k',' ','+',' ','Y','e','l','l','o','w',		// L_CAL__4 (english)  14(0), 147
'N','o','i','r',' ','+',' ','J','a','u','n','e',		// L_CAL__4 (french)  12(0)
'5',')','B','l','a','c','k',' ','+',' ','G','r','e','e','n',':',		// L_CAL_5 (english)  16(0), 148
'5',')','N','o','i','r',' ','+',' ','V','e','r','t',' ',':',		// L_CAL_5 (french)  15(0)
'5',')','B','l','a','c','k','/','G','r','e','e','n',		// L_CAL5 (english)  13(0), 149
'5',')','N','o','i','r','/','V','e','r','t',		// L_CAL5 (french)  11(0)
'B','l','a','c','k',' ','+',' ','G','r','e','e','n',		// L_CAL__5 (english)  13(0), 150
'N','o','i','r',' ','+',' ','V','e','r','t',		// L_CAL__5 (french)  11(0)
'6',')','B','l','a','c','k',' ','+',' ','B','l','u','e',':',		// L_CAL_6 (english)  15(0), 151
'6',')','N','o','i','r',' ','+',' ','B','l','e','u',' ',':',		// L_CAL_6 (french)  15(0)
'6',')','B','l','a','c','k','/','B','l','u','e',		// L_CAL6 (english)  12(0), 152
'6',')','N','o','i','r','/','B','l','e','u',		// L_CAL6 (french)  11(0)
'B','l','a','c','k',' ','+',' ','B','l','u','e',		// L_CAL__6 (english)  12(0), 153
'N','o','i','r',' ','+',' ','B','l','e','u',		// L_CAL__6 (french)  11(0)
'7',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',':',		// L_CAL_7 (english)  17(0), 154
'7',')','N','o','i','r',' ','+',' ','V','i','o','l','e','t',' ',':',		// L_CAL_7 (french)  17(0)
'7',')','B','l','a','c','k','/','P','u','r','p','l','e',		// L_CAL7 (english)  14(0), 155
'7',')','N','o','i','r','/','V','i','o','l','e','t',		// L_CAL7 (french)  13(0)
'B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL__7 (english)  14(0), 156
'N','o','i','r',' ','+',' ','V','i','o','l','e','t',		// L_CAL__7 (french)  13(0)
'8',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL_8 (english)  16(0), 157
'8',')','N','o','i','r',' ','+',' ','V','i','o','l','e','t',		// L_CAL_8 (french)  15(0)
'8',')','B','l','a','c','k','/','P','u','r','p','l','e','/','R','e','d',		// L_CAL8 (english)  18(0), 158
'8',')','N','o','i','r','/','V','i','o','l','e','t','/','R','o','u','g','e',		// L_CAL8 (french)  19(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','R','e','d',		// L_CAL__8 (english)  16(0), 159
'N','o','i','r','/','V','i','o','l','e','t','/','R','o','u','g','e',		// L_CAL__8 (french)  17(0)
'9',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL_9 (english)  16(0), 160
'9',')','N','o','i','r',' ','+',' ','V','i','o','l','e','t',		// L_CAL_9 (french)  15(0)
'9',')','B','l','k','/','P','u','r','p','l','e','/','O','r','a','n','g','e',		// L_CAL9 (english)  19(0), 161
'9',')','N','o','i','r','/','V','i','o','l','e','t','/','O','r','a','n','g','e',		// L_CAL9 (french)  20(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','O','r','a','n','g','e',		// L_CAL__9 (english)  19(0), 162
'N','o','i','r','/','V','i','o','l','e','t','/','O','r','a','n','g','e',		// L_CAL__9 (french)  18(0)
'1','0',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL_10 (english)  17(0), 163
'1','0',')','N','o','i','r',' ','+',' ','V','i','o','l','e','t',		// L_CAL_10 (french)  16(0)
'1','0',')','B','l','k','/','P','u','r','p','l','/','Y','e','l','l','o','w',		// L_CAL10 (english)  19(0), 164
'1','0',')','N','o','i','r','/','V','i','o','l','e','t','/','J','a','u','n','e',		// L_CAL10 (french)  20(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','Y','e','l','l','o','w',		// L_CAL__10 (english)  19(0), 165
'N','o','i','r','/','V','i','o','l','e','t','/','J','a','u','n','e',		// L_CAL__10 (french)  17(0)
'1','1',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL_11 (english)  17(0), 166
'1','1',')','N','o','i','r',' ','+',' ','V','i','o','l','e','t',		// L_CAL_11 (french)  16(0)
'1','1',')','B','l','k','/','P','u','r','p','l','e','/','G','r','e','e','n',		// L_CAL11 (english)  19(0), 167
'1','1',')','N','o','i','r','/','V','i','o','l','e','t','/','V','e','r','t',		// L_CAL11 (french)  19(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','G','r','e','e','n',		// L_CAL__11 (english)  18(0), 168
'N','o','i','r','/','V','i','o','l','e','t','/','V','e','r','t',		// L_CAL__11 (french)  16(0)
'1','2',')','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_CAL_12 (english)  17(0), 169
'1','2',')','N','o','i','r',' ','+',' ','V','i','o','l','e','t',		// L_CAL_12 (french)  16(0)
'1','2',')','B','l','k','/','P','u','r','p','l','e','/','B','l','u','e',		// L_CAL12 (english)  18(0), 170
'1','2',')','N','o','i','r','/','V','i','o','l','e','t','/','B','l','e','u',		// L_CAL12 (french)  19(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','B','l','u','e',		// L_CAL__12 (english)  17(0), 171
'N','o','i','r','/','V','i','o','l','e','t','/','B','l','e','u',		// L_CAL__12 (french)  16(0)
'+',' ','R','e','d',':',		// L_CAL_82 (english)  6(0), 172
'+',' ','R','o','u','g','e',' ',':',		// L_CAL_82 (french)  9(0)
'+',' ','O','r','a','n','g','e',':',		// L_CAL_92 (english)  9(0), 173
'+',' ','O','r','a','n','g','e',' ',':',		// L_CAL_92 (french)  10(0)
'+',' ','Y','e','l','l','o','w',':',		// L_CAL_102 (english)  9(0), 174
'+',' ','J','a','u','n','e',' ',':',		// L_CAL_102 (french)  9(0)
'+',' ','G','r','e','e','n',':',		// L_CAL_11_2 (english)  8(0), 175
'+',' ','V','e','r','t',' ',':',		// L_CAL_11_2 (french)  8(0)
'+',' ','B','l','u','e',':',		// L_CAL_12_2 (english)  7(0), 176
'+',' ','B','l','e','u',' ',':',		// L_CAL_12_2 (french)  8(0)
'Y','e','s',		// L_YES_ENGLISH (english)  3(0), 177
'Y','e','s',		// L_YES_ENGLISH (french)  3(0)
'N','o',		// L_NO_ENGLISH (english)  2(0), 178
'N','o',		// L_NO_ENGLISH (french)  2(0)
'C','o','m','m','e','n','t',':',		// L_COMMENT (english)  8(0), 179
'C','o','m','m','e','n','t','a','i','r','e',' ',':',		// L_COMMENT (french)  13(0)
'R','e','s','u','m','e',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',		// L_RESUME_AUTOLINEARITY_TEST (english)  25(0), 180
'R','e','p','r','e','n','d','r','e',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,		// L_RESUME_AUTOLINEARITY_TEST (french)  28(2)
'E','R','R','O','R',		// L_ERROR (english)  5(0), 181
'E','R','R','E','U','R',		// L_ERROR (french)  6(0)
'E','r','r','o','r',		// L_ERROR2 (english)  5(0), 182
'E','r','r','e','u','r',		// L_ERROR2 (french)  6(0)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','c','h','a','m','b','e','r',' ','w','i','t','h',' ','S','/','N',':',' ','%','s',		// L_UNABLE_TO_FIND_CHAMBER_WITH_SN (english)  35(0), 183
'I','m','p','o','s','s','i','b','l','e',' ','d','e',' ','t','r','o','u','v','e','r',' ','l','a',' ','c','h','a','m','b','r','e',' ','S','/','N',' ',':',' ','%','s',		// L_UNABLE_TO_FIND_CHAMBER_WITH_SN (french)  41(0)
'A','b','o','r','t',' ','c','u','r','r','e','n','t',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t','?',		// L_ABORT_CURRENT_AUTOLINEARITY_TEST (english)  33(0), 184
'A','b','a','n','d','o','n','n','e','r',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','e','n',' ','c','o','u','r','s',' ','?',		// L_ABORT_CURRENT_AUTOLINEARITY_TEST (french)  40(2)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','u','s','a','b','l','e',' ','N','u','c','l','i','d','e',':',' ','%','s',		// L_ABORT_UNABLE_TO_FIND_USABLE_NUCLIDE (english)  33(0), 185
'N','u','c','l',0xE9,'i','d','e',' ','u','t','i','l','i','s','a','b','l','e',' ','i','n','t','r','o','u','v','a','b','l','e',' ',':',' ','%','s',		// L_ABORT_UNABLE_TO_FIND_USABLE_NUCLIDE (french)  36(1)
'C','o','m','p','l','e','t','e','d',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',' ','h','a','s',' ','n','o','t',' ','b','e','e','n',' ','s','a','v','e','d','.',		// L_COMPLETED_AUTOLINEARITY_NOT_SAVED (english)  48(0), 186
'L','e',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','t','e','r','m','i','n',0xE9,' ','n',0x27,'a',' ','p','a','s',' ',0xE9,'t',0xE9,' ','e','n','r','e','g','i','s','t','r',0xE9,'.',		// L_COMPLETED_AUTOLINEARITY_NOT_SAVED (french)  53(6)
'P','r','o','c','e','e','d',' ','t','o',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t','?',		// L_PROCEED_TO_AUTOLINEARITY (english)  30(0), 187
'P','r','o','c',0xE9,'d','e','r',' ','a','u',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','?',		// L_PROCEED_TO_AUTOLINEARITY (french)  32(3)
'P','l','e','a','s','e',' ','p','l','a','c','e',' ','N','u','c','l','i','d','e',':',' ','%','s',' ','i','n','t','o',' ','C','h','a','m','b','e','r',':',' ','%','d',		// L_PLEASE_PLACE_NUCLIDE (english)  41(0), 188
'V','e','u','i','l','l','e','z',' ','p','l','a','c','e','r',' ','l','e',' ','n','u','c','l',0xE9,'i','d','e',' ',':',' ','%','s',' ','d','a','n','s',' ','l','a',' ','c','h','a','m','b','r','e',' ',':',' ','%','d',		// L_PLEASE_PLACE_NUCLIDE (french)  53(1)
'R','e','s','u','m','e',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t','?',		// L_RESUME_AUTOLINEARITY (english)  26(0), 189
'R','e','p','r','e','n','d','r','e',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','?',		// L_RESUME_AUTOLINEARITY (french)  30(2)
'P','a','u','s','e',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',		// L_PAUSE_AUTOLINEARITY_TEST (english)  24(0), 190
'P','a','u','s','e',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,		// L_PAUSE_AUTOLINEARITY_TEST (french)  24(2)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','h','a','s',' ','b','e','e','n',' ','p','a','u','s','e','d','.',		// L_AUTOLINEARITY_HAS_BEEN_PAUSED (english)  30(0), 191
'A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','a',' ',0xE9,'t',0xE9,' ','m','i','s','e',' ','e','n',' ','p','a','u','s','e','.',		// L_AUTOLINEARITY_HAS_BEEN_PAUSED (french)  34(4)
'C','o','m','p','l','e','t','e','d',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',' ','h','a','s',' ','n','o','t',' ','b','e','e','n',' ','s','a','v','e','d','.',		// L_COMPLETED_AUTOLINEARITY_HAS_NOT_BEEN_SAVED (english)  48(0), 192
'L','e',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','t','e','r','m','i','n',0xE9,' ','n',0x27,'a',' ','p','a','s',' ',0xE9,'t',0xE9,' ','e','n','r','e','g','i','s','t','r',0xE9,'.',		// L_COMPLETED_AUTOLINEARITY_HAS_NOT_BEEN_SAVED (french)  53(6)
'T','o',' ','a','v','o','i','d',' ','m','i','s','s','i','n','g',' ','m','e','a','s','u','r','e','m','e','n','t','s',',',' ',		// L_TO_AVOID_MISSING_MEASUREMENTS (english)  31(0), 193
'P','o','u','r',' ',0xE9,'v','i','t','e','r',' ','d','e',' ','m','a','n','q','u','e','r',' ','d','e','s',' ','m','e','s','u','r','e','s',',',' ',		// L_TO_AVOID_MISSING_MEASUREMENTS (french)  36(1)
'P','l','e','a','s','e',' ','r','e','t','u','r','n',' ','t','o',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','a','s',' ','s','o','o','n',' ','a','s',' ','p','o','s','s','i','b','l','e','.',		// L_PLEASE_RETURN_TO_AUTOLINEARITY (english)  51(0), 194
'V','e','u','i','l','l','e','z',' ','r','e','v','e','n','i','r',' ',0xE0,' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','d',0xE8,'s',' ','q','u','e',' ','p','o','s','s','i','b','l','e','.',		// L_PLEASE_RETURN_TO_AUTOLINEARITY (french)  50(4)
'P','r','o','c','e','e','d',' ','t','o',' ','H','o','m','e',' ','S','c','r','e','e','n','?',		// L_PROCEED_TO_HOME_SCREEN (english)  23(0), 195
'A','l','l','e','r',' ',0xE0,' ','l',0x27,0xE9,'c','r','a','n',' ','d',0x27,'a','c','c','u','e','i','l',' ','?',		// L_PROCEED_TO_HOME_SCREEN (french)  27(2)
'P','r','o','c','e','e','d',' ','t','o',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','M','e','n','u','?',		// L_PROCEED_TO_AUTOLINEARITY_MENU (english)  30(0), 196
'A','l','l','e','r',' ','a','u',' ','m','e','n','u',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','?',		// L_PROCEED_TO_AUTOLINEARITY_MENU (french)  29(2)
'A','b','o','r','t',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',		// L_ABORT_AUTOLINEARITY_TEST (english)  24(0), 197
'A','b','a','n','d','o','n','n','e','r',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,		// L_ABORT_AUTOLINEARITY_TEST (french)  29(2)
'A','b','o','r','t',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t','?',		// L_ABORT_AUTOLINEARITY_TEST2 (english)  25(0), 198
'A','b','a','n','d','o','n','n','e','r',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','?',		// L_ABORT_AUTOLINEARITY_TEST2 (french)  31(2)
'C','u','r','r','e','n','t',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',' ','h','a','s',' ','f','i','n','i','s','h','e','d','.',		// L_CURRENT_AUTOLINEARITY_TEST_HAS_FINISHED (english)  40(0), 199
'L','e',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','e','n',' ','c','o','u','r','s',' ','e','s','t',' ','t','e','r','m','i','n',0xE9,'.',		// L_CURRENT_AUTOLINEARITY_TEST_HAS_FINISHED (french)  43(3)
'E','r','a','s','e',' ','C','u','r','r','e','n','t',' ','T','e','s','t','?',		// L_ERASE_CURRENT_TEST (english)  19(0), 200
'E','f','f','a','c','e','r',' ','l','e',' ','t','e','s','t',' ','e','n',' ','c','o','u','r','s',' ','?',		// L_ERASE_CURRENT_TEST (french)  26(0)
'S','a','v','e',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',		// L_SAVE_AUTOLINEARITY_TEST (english)  23(0), 201
'E','n','r','e','g','i','s','t','r','e','r',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,		// L_SAVE_AUTOLINEARITY_TEST (french)  30(2)
'S','a','v','e',' ','C','u','r','r','e','n','t',' ','T','e','s','t','?',		// L_SAVE_CURRENT_TEST (english)  18(0), 202
'E','n','r','e','g','i','s','t','r','e','r',' ','l','e',' ','t','e','s','t',' ','e','n',' ','c','o','u','r','s',' ','?',		// L_SAVE_CURRENT_TEST (french)  30(0)
'C','u','r','r','e','n','t',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',' ','i','s',' ','p','a','r','t','i','a','l','l','y',' ','c','o','m','p','l','e','t','e','.',		// L_CURRENT_AUTOLINEARITY_IS_PARTIALLY_COMPLETE (english)  49(0), 203
'L','e',' ','t','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','e','n',' ','c','o','u','r','s',' ','e','s','t',' ','p','a','r','t','i','e','l','l','e','m','e','n','t',' ','t','e','r','m','i','n',0xE9,'.',		// L_CURRENT_AUTOLINEARITY_IS_PARTIALLY_COMPLETE (french)  57(3)
'E','n','d',' ','a','n','d',' ','S','a','v','e',' ','C','u','r','r','e','n','t',' ','T','e','s','t','?',		// L_END_AND_SAVE_TEST (english)  26(0), 204
'T','e','r','m','i','n','e','r',' ','e','t',' ','e','n','r','e','g','i','s','t','r','e','r',' ','t','e','s','t',' ','?',		// L_END_AND_SAVE_TEST (french)  30(0)
'M','a','x','i','m','u','m',' ','i','s',' ','%','.','4','f',		// L_MAXIMUM_IS (english)  15(0), 205
'M','a','x','i','m','u','m',' ','e','s','t',' ','%','.','4','f',		// L_MAXIMUM_IS (french)  16(0)
'I','n','p','u','t',' ','E','r','r','o','r',		// L_INPUT_ERROR (english)  11(0), 206
'E','r','r','e','u','r',' ','d','e',' ','s','a','i','s','i','e',		// L_INPUT_ERROR (french)  16(0)
'M','i','n','i','m','u','m',' ','i','s',' ','%','.','4','f',		// L_MINIMUM_IS (english)  15(0), 207
'M','i','n','i','m','u','m',' ','e','s','t',' ','%','.','4','f',		// L_MINIMUM_IS (french)  16(0)
'C','a','l',' ','N','u','m','b','e','r',' ','E','r','r','o','r',		// L_CAL_NUMBER_ERROR (english)  16(0), 208
'E','r','r','e','u','r',' ','n','u','m',0xE9,'r','o',' ','C','a','l',		// L_CAL_NUMBER_ERROR (french)  17(1)
'I','n','v','a','l','i','d',' ','C','a','l',' ','N','u','m','b','e','r',		// L_INVALID_CAL_NUMBER (english)  18(0), 209
'N','u','m',0xE9,'r','o',' ','C','a','l',' ','i','n','v','a','l','i','d','e',		// L_INVALID_CAL_NUMBER (french)  19(1)
'U','s','e','r',' ','C','a','l',' ','F','u','l','l',		// L_USER_CAL_FULL (english)  13(0), 210
'C','a','l',' ','u','t','i','l','i','s','a','t','e','u','r',' ','p','l','e','i','n',		// L_USER_CAL_FULL (french)  21(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','C','u','r','r','e','n','t',' ','T','i','m','e',':',		// L_PLEASE_ENTER_CURRENT_TIME (english)  26(0), 211
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l',0x27,'h','e','u','r','e',' ','a','c','t','u','e','l','l','e',' ',':',		// L_PLEASE_ENTER_CURRENT_TIME (french)  34(0)
'S','e','r','i','a','l',' ','N','u','m','b','e','r',' ','E','r','r','o','r',		// L_SERIAL_NUMBER_ERROR (english)  19(0), 212
'E','r','r','e','u','r',' ','n','u','m',0xE9,'r','o',' ','d','e',' ','s',0xE9,'r','i','e',		// L_SERIAL_NUMBER_ERROR (french)  22(2)
'S','e','r','i','a','l',' ','n','u','m','b','e','r',' ','m','u','s','t',' ','h','a','v','e',' ','s','i','x',' ','d','i','g','i','t','s',		// L_SERIAL_NUMBER_MUST_HAVE_SIX_DIGITS (english)  34(0), 213
'L','e',' ','n','u','m',0xE9,'r','o',' ','d','e',' ','s',0xE9,'r','i','e',' ','d','o','i','t',' ','a','v','o','i','r',' ','s','i','x',' ','c','h','i','f','f','r','e','s',		// L_SERIAL_NUMBER_MUST_HAVE_SIX_DIGITS (french)  42(2)
'D','o','s','e',' ','T','i','m','e',' ','E','r','r','o','r',		// L_DOSE_TIME_ERROR (english)  15(0), 214
'E','r','r','e','u','r',' ','h','e','u','r','e',' ','d','e',' ','d','o','s','e',		// L_DOSE_TIME_ERROR (french)  20(0)
'I','n','v','a','l','i','d',' ','D','a','t','e',		// L_INVALID_DATE (english)  12(0), 215
'D','a','t','e',' ','i','n','v','a','l','i','d','e',		// L_INVALID_DATE (french)  13(0)
'I','n','v','a','l','i','d',' ','T','i','m','e',		// L_INVALID_TIME (english)  12(0), 216
'H','e','u','r','e',' ','i','n','v','a','l','i','d','e',		// L_INVALID_TIME (french)  14(0)
'M','a','x',' ','H','o','u','r',' ','i','s',' ','2','3',		// L_MAX_HOUR_IS_23 (english)  14(0), 217
'H','e','u','r','e',' ','m','a','x',' ','e','s','t',' ','2','3',		// L_MAX_HOUR_IS_23 (french)  16(0)
'M','a','x',' ','M','i','n','u','t','e',' ','i','s',' ','5','9',		// L_MAX_MINUTE_IS_59 (english)  16(0), 218
'M','i','n','u','t','e',' ','m','a','x',' ','e','s','t',' ','5','9',		// L_MAX_MINUTE_IS_59 (french)  17(0)
'M','i','n',' ','D','a','y',' ','i','s',' ','1',		// L_MIN_DAY_IS_1 (english)  12(0), 219
'J','o','u','r',' ','m','i','n',' ','e','s','t',' ','1',		// L_MIN_DAY_IS_1 (french)  14(0)
'M','a','x',' ','D','a','y',' ','i','s',' ','3','1',		// L_MAX_DAY_IS_31 (english)  13(0), 220
'J','o','u','r',' ','m','a','x',' ','e','s','t',' ','3','1',		// L_MAX_DAY_IS_31 (french)  15(0)
'M','i','n',' ','M','o','n','t','h',' ','i','s',' ','1',		// L_MIN_MONTH_IS_1 (english)  14(0), 221
'M','o','i','s',' ','m','i','n',' ','e','s','t',' ','1',		// L_MIN_MONTH_IS_1 (french)  14(0)
'M','a','x',' ','M','o','n','t','h',' ','i','s',' ','1','2',		// L_MAX_MONTH_IS_12 (english)  15(0), 222
'M','o','i','s',' ','m','a','x',' ','e','s','t',' ','1','2',		// L_MAX_MONTH_IS_12 (french)  15(0)
'M','a','x',' ','Y','e','a','r',' ','i','s',' ','2','0','3','0',		// L_MAX_YEAR_IS_2030 (english)  16(0), 223
'A','n','n',0xE9,'e',' ','m','a','x',' ','e','s','t',' ','2','0','3','0',		// L_MAX_YEAR_IS_2030 (french)  18(1)
'I','n','v','a','l','i','d',' ','Y','e','a','r',		// L_INVALID_YEAR (english)  12(0), 224
'A','n','n',0xE9,'e',' ','i','n','v','a','l','i','d','e',		// L_INVALID_YEAR (french)  14(1)
'T','u','b','e',' ','#','1',':',' ','(','1',')',		// L_TUBE_1_LINEATOR (english)  12(0), 225
'T','u','b','e',' ','#','1',' ',':',' ','(','1',')',		// L_TUBE_1_LINEATOR (french)  13(0)
'1',')',' ','T','u','b','e',' ','1',		// L_TUBE_1_LINEATOR2 (english)  9(0), 226
'1',')',' ','T','u','b','e',' ','1',		// L_TUBE_1_LINEATOR2 (french)  9(0)
'T','u','b','e',' ','#','2',':',' ','(','1',' ','+',' ','2',')',		// L_TUBE_2_LINEATOR (english)  16(0), 227
'T','u','b','e',' ','#','2',' ',':',' ','(','1',' ','+',' ','2',')',		// L_TUBE_2_LINEATOR (french)  17(0)
'2',')',' ','T','u','b','e',' ','1','+','2',		// L_TUBE_2_LINEATOR2 (english)  11(0), 228
'2',')',' ','T','u','b','e',' ','1','+','2',		// L_TUBE_2_LINEATOR2 (french)  11(0)
'T','u','b','e',' ','#','3',':',' ','(','1',' ','+',' ','3',')',		// L_TUBE_3_LINEATOR (english)  16(0), 229
'T','u','b','e',' ','#','3',' ',':',' ','(','1',' ','+',' ','3',')',		// L_TUBE_3_LINEATOR (french)  17(0)
'3',')',' ','T','u','b','e',' ','1','+','3',		// L_TUBE_3_LINEATOR2 (english)  11(0), 230
'3',')',' ','T','u','b','e',' ','1','+','3',		// L_TUBE_3_LINEATOR2 (french)  11(0)
'T','u','b','e',' ','#','4',':',' ','(','1',' ','+',' ','2',' ','+',' ','3',')',		// L_TUBE_4_LINEATOR (english)  20(0), 231
'T','u','b','e',' ','#','4',' ',':',' ','(','1',' ','+',' ','2',' ','+',' ','3',')',		// L_TUBE_4_LINEATOR (french)  21(0)
'4',')',' ','T','u','b','e',' ','1','+','2',',','3',		// L_TUBE_4_LINEATOR2 (english)  13(0), 232
'4',')',' ','T','u','b','e',' ','1','+','2',',','3',		// L_TUBE_4_LINEATOR2 (french)  13(0)
'T','u','b','e',' ','#','5',':',' ','(','1',' ','+',' ','4',')',		// L_TUBE_5_LINEATOR (english)  16(0), 233
'T','u','b','e',' ','#','5',' ',':',' ','(','1',' ','+',' ','4',')',		// L_TUBE_5_LINEATOR (french)  17(0)
'5',')',' ','T','u','b','e',' ','1','+','4',		// L_TUBE_5_LINEATOR2 (english)  11(0), 234
'5',')',' ','T','u','b','e',' ','1','+','4',		// L_TUBE_5_LINEATOR2 (french)  11(0)
'T','u','b','e',' ','#','6',':',' ','(','1',' ','+',' ','2',' ','+',' ','4',')',		// L_TUBE_6_LINEATOR (english)  20(0), 235
'T','u','b','e',' ','#','6',' ',':',' ','(','1',' ','+',' ','2',' ','+',' ','4',')',		// L_TUBE_6_LINEATOR (french)  21(0)
'6',')',' ','T','u','b','e',' ','1','+','2',',','4',		// L_TUBE_6_LINEATOR2 (english)  13(0), 236
'6',')',' ','T','u','b','e',' ','1','+','2',',','4',		// L_TUBE_6_LINEATOR2 (french)  13(0)
'T','u','b','e',' ','#','7',':',' ','(','1',' ','+',' ','3',' ','+',' ','4',')',		// L_TUBE_7_LINEATOR (english)  20(0), 237
'T','u','b','e',' ','#','7',' ',':',' ','(','1',' ','+',' ','3',' ','+',' ','4',')',		// L_TUBE_7_LINEATOR (french)  21(0)
'7',')',' ','T','u','b','e',' ','1','+','3',',','4',		// L_TUBE_7_LINEATOR2 (english)  13(0), 238
'7',')',' ','T','u','b','e',' ','1','+','3',',','4',		// L_TUBE_7_LINEATOR2 (french)  13(0)
'T','u','b','e',' ','#','8',':',' ','(','1',' ','+',' ','2',' ','+',' ','3',' ','+',' ','4',')',		// L_TUBE_8_LINEATOR (english)  24(0), 239
'T','u','b','e',' ','#','8',' ',':',' ','(','1',' ','+',' ','2',' ','+',' ','3',' ','+',' ','4',')',		// L_TUBE_8_LINEATOR (french)  25(0)
'8',')',' ','T','u','b','e',' ','1','+','2',',','3',',','4',		// L_TUBE_8_LINEATOR2 (english)  15(0), 240
'8',')',' ','T','u','b','e',' ','1','+','2',',','3',',','4',		// L_TUBE_8_LINEATOR2 (french)  15(0)
'T','u','b','e',' ','1',':',' ','B','l','a','c','k',		// L_TUBE_1_CALICHECK (english)  13(0), 241
'T','u','b','e',' ','1',' ',':',' ','N','o','i','r',		// L_TUBE_1_CALICHECK (french)  13(0)
'T','u','b','e',' ','2',':',' ','B','l','a','c','k',' ','+',' ','R','e','d',		// L_TUBE_2_CALICHECK (english)  19(0), 242
'T','u','b','e',' ','2',' ',':',' ','N','o','i','r',' ','+',' ','R','o','u','g','e',		// L_TUBE_2_CALICHECK (french)  21(0)
'T','u','b','e',' ','3',':',' ','B','l','a','c','k',' ','+',' ','O','r','a','n','g','e',		// L_TUBE_3_CALICHECK (english)  22(0), 243
'T','u','b','e',' ','3',' ',':',' ','N','o','i','r',' ','+',' ','O','r','a','n','g','e',		// L_TUBE_3_CALICHECK (french)  22(0)
'T','u','b','e',' ','4',':',' ','B','l','a','c','k',' ','+',' ','Y','e','l','l','o','w',		// L_TUBE_4_CALICHECK (english)  22(0), 244
'T','u','b','e',' ','4',' ',':',' ','N','o','i','r',' ','+',' ','J','a','u','n','e',		// L_TUBE_4_CALICHECK (french)  21(0)
'T','u','b','e',' ','5',':',' ','B','l','a','c','k',' ','+',' ','G','r','e','e','n',		// L_TUBE_5_CALICHECK (english)  21(0), 245
'T','u','b','e',' ','5',' ',':',' ','N','o','i','r',' ','+',' ','V','e','r','t',		// L_TUBE_5_CALICHECK (french)  20(0)
'T','u','b','e',' ','6',':',' ','B','l','a','c','k',' ','+',' ','B','l','u','e',		// L_TUBE_6_CALICHECK (english)  20(0), 246
'T','u','b','e',' ','6',' ',':',' ','N','o','i','r',' ','+',' ','B','l','e','u',		// L_TUBE_6_CALICHECK (french)  20(0)
'T','u','b','e',' ','7',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_TUBE_7_CALICHECK (english)  22(0), 247
'T','u','b','e',' ','7',' ',':',' ','N','o','i','r',' ','+',' ','V','i','o','l','e','t',		// L_TUBE_7_CALICHECK (french)  22(0)
'T','u','b','e',' ','8',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',' ','+',' ','R','e','d',		// L_TUBE_8_CALICHECK (english)  28(0), 248
'T','u','b','e',' ','8',' ',':',' ','N','o','i','r',' ','+',' ','V','i','o','l','e','t',' ','+',' ','R','o','u','g','e',		// L_TUBE_8_CALICHECK (french)  30(0)
'T','u','b','e',' ','9',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',' ','+',' ','O','r','a','n','g','e',		// L_TUBE_9_CALICHECK (english)  31(0), 249
'T','u','b','e',' ','9',' ',':',' ','N','o','i','r',' ','+',' ','V','i','o','l','e','t',' ','+',' ','O','r','a','n','g','e',		// L_TUBE_9_CALICHECK (french)  31(0)
'T','u','b','e',' ','1','0',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',' ','+',' ','Y','e','l','l','o','w',		// L_TUBE_10_CALICHECK (english)  32(0), 250
'T','u','b','e',' ','1','0',' ',':',' ','N','o','i','r',' ','+',' ','V','i','o','l','e','t',' ','+',' ','J','a','u','n','e',		// L_TUBE_10_CALICHECK (french)  31(0)
'T','u','b','e',' ','1','1',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',' ','+',' ','G','r','e','e','n',		// L_TUBE_11_CALICHECK (english)  31(0), 251
'T','u','b','e',' ','1','1',' ',':',' ','N','o','i','r',' ','+',' ','V','i','o','l','e','t',' ','+',' ','V','e','r','t',		// L_TUBE_11_CALICHECK (french)  30(0)
'T','u','b','e',' ','1','2',':',' ','B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',' ','+',' ','B','l','u','e',		// L_TUBE_12_CALICHECK (english)  30(0), 252
'T','u','b','e',' ','1','2',' ',':',' ','N','o','i','r',' ','+',' ','V','i','o','l','e','t',' ','+',' ','B','l','e','u',		// L_TUBE_12_CALICHECK (french)  30(0)
'B','l','a','c','k',		// L_PR_TUBE_1_CALICHECK (english)  5(0), 253
'N','o','i','r',		// L_PR_TUBE_1_CALICHECK (french)  4(0)
'B','l','a','c','k',' ','+',' ','R','e','d',		// L_PR_TUBE_2_CALICHECK (english)  11(0), 254
'N','o','i','r',' ','+',' ','R','o','u','g','e',		// L_PR_TUBE_2_CALICHECK (french)  12(0)
'B','l','a','c','k',' ','+',' ','O','r','a','n','g','e',		// L_PR_TUBE_3_CALICHECK (english)  14(0), 255
'N','o','i','r',' ','+',' ','O','r','a','n','g','e',		// L_PR_TUBE_3_CALICHECK (french)  13(0)
'B','l','a','c','k',' ','+',' ','Y','e','l','l','o','w',		// L_PR_TUBE_4_CALICHECK (english)  14(0), 256
'N','o','i','r',' ','+',' ','J','a','u','n','e',		// L_PR_TUBE_4_CALICHECK (french)  12(0)
'B','l','a','c','k',' ','+',' ','G','r','e','e','n',		// L_PR_TUBE_5_CALICHECK (english)  13(0), 257
'N','o','i','r',' ','+',' ','V','e','r','t',		// L_PR_TUBE_5_CALICHECK (french)  11(0)
'B','l','a','c','k',' ','+',' ','B','l','u','e',		// L_PR_TUBE_6_CALICHECK (english)  12(0), 258
'N','o','i','r',' ','+',' ','B','l','e','u',		// L_PR_TUBE_6_CALICHECK (french)  11(0)
'B','l','a','c','k',' ','+',' ','P','u','r','p','l','e',		// L_PR_TUBE_7_CALICHECK (english)  14(0), 259
'N','o','i','r',' ','+',' ','V','i','o','l','e','t',		// L_PR_TUBE_7_CALICHECK (french)  13(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','R','e','d',		// L_PR_TUBE_8_CALICHECK (english)  16(0), 260
'N','o','i','r','/','V','i','o','l','e','t','/','R','o','u','g','e',		// L_PR_TUBE_8_CALICHECK (french)  17(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','O','r','a','n','g','e',		// L_PR_TUBE_9_CALICHECK (english)  19(0), 261
'N','o','i','r','/','V','i','o','l','e','t','/','O','r','a','n','g','e',		// L_PR_TUBE_9_CALICHECK (french)  18(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','Y','e','l','l','o','w',		// L_PR_TUBE_10_CALICHECK (english)  19(0), 262
'N','o','i','r','/','V','i','o','l','e','t','/','J','a','u','n','e',		// L_PR_TUBE_10_CALICHECK (french)  17(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','G','r','e','e','n',		// L_PR_TUBE_11_CALICHECK (english)  18(0), 263
'N','o','i','r','/','V','i','o','l','e','t','/','V','e','r','t',		// L_PR_TUBE_11_CALICHECK (french)  16(0)
'B','l','a','c','k','/','P','u','r','p','l','e','/','B','l','u','e',		// L_PR_TUBE_12_CALICHECK (english)  17(0), 264
'N','o','i','r','/','V','i','o','l','e','t','/','B','l','e','u',		// L_PR_TUBE_12_CALICHECK (french)  16(0)
'M','e','a','s','u','r','e',' ','t','e','s','t',' ','s','o','u','r','c','e',',',' ',		// L_MEASURE_TEST_SOURCE (english)  21(0), 265
'M','e','s','u','r','e','r',' ','s','o','u','r','c','e',' ','d','e',' ','t','e','s','t',',',' ',		// L_MEASURE_TEST_SOURCE (french)  24(0)
',',' ','S','/','N',':',' ',		// L_MEASURE_TEST_SOURCE_SN (english)  7(0), 266
',',' ','S','/','N',' ',':',' ',		// L_MEASURE_TEST_SOURCE_SN (french)  8(0)
'O','V','E','R',' ','R','A','N','G','E',		// L_OVERRANGE (english)  10(0), 267
'H','O','R','S',' ','P','L','A','G','E',		// L_OVERRANGE (french)  10(0)
'O','v','e','r',' ','R','a','n','g','e',		// L_OVERRANGE2 (english)  10(0), 268
'H','o','r','s',' ','p','l','a','g','e',		// L_OVERRANGE2 (french)  10(0)
'O','V','E','R',' ','R','A','N','G','E','\n','E','R','R','O','R',		// L_OVERRANGE_ERROR (english)  16(0), 269
'H','O','R','S',' ','P','L','A','G','E','\n','E','R','R','E','U','R',		// L_OVERRANGE_ERROR (french)  17(0)
'O','v','e','r',' ','R','a','n','g','e',' ','E','r','r','o','r',		// L_OVERRANGE_ERROR2 (english)  16(0), 270
'E','r','r','e','u','r',' ','h','o','r','s',' ','p','l','a','g','e',		// L_OVERRANGE_ERROR2 (french)  17(0)
'C','a','l','c',':',' ','%','s',',',' ','D','e','v',':',' ','E','R','R','O','R',		// L_CALC_DEV_ERROR (english)  20(0), 271
'C','a','l','c',' ',':',' ','%','s',',',' ',0xC9,'c','a','r','t',' ',':',' ','E','R','R','E','U','R',		// L_CALC_DEV_ERROR (french)  25(1)
'C','a','l','c',':',' ','%','s',',',' ','D','e','v',':',' ','%','.','1','f','%','%',		// L_CALC_DEV (english)  21(0), 272
'C','a','l','c',' ',':',' ','%','s',',',' ',0xC9,'c','a','r','t',' ',':',' ','%','.','1','f','%','%',		// L_CALC_DEV (french)  25(1)
'C','l','e','a','r',' ','S','e','l','e','c','t','e','d','\n','N','u','c','l','i','d','e',		// L_CLEAR_SELECTED_NUCLIDE (english)  22(0), 273
'E','f','f','a','c','e','r',' ','n','u','c','l',0xE9,'i','d','e','\n','s',0xE9,'l','e','c','t','i','o','n','n',0xE9,		// L_CLEAR_SELECTED_NUCLIDE (french)  28(3)
'P','a','g','e',' ','%','d',' ','o','f',' ','%','d',		// L_PAGE_OF (english)  13(0), 274
'P','a','g','e',' ','%','d',' ','d','e',' ','%','d',		// L_PAGE_OF (french)  13(0)
'U','S','B',' ','P','C',' ','D','r','i','v','e','r',':',		// L_USB_PC_DRIVER (english)  14(0), 275
'P','i','l','o','t','e',' ','U','S','B',' ','P','C',' ',':',		// L_USB_PC_DRIVER (french)  15(0)
'S','c','r','e','e','n',' ','C','a','l','i','b',		// L_SCREEN_CALIB (english)  12(0), 276
'C','a','l','i','b','.',' ',0xE9,'c','r','a','n',		// L_SCREEN_CALIB (french)  12(1)
'D','o','s','e',' ','D','e','c','a','y',' ','E','n','t','r','y',':',		// L_DOSE_DECAY_ENTRY (english)  17(0), 277
'S','a','i','s','i','e',' ','d',0xE9,'c','r','o','i','s','s','a','n','c','e',' ',':',		// L_DOSE_DECAY_ENTRY (french)  21(1)
'P','l','e','a','s','e',' ','S','e','l','e','c','t',' ','N','u','c','l','i','d','e',		// L_PLEASE_SELECT_NUCLIDE (english)  21(0), 278
'V','e','u','i','l','l','e','z',' ','s',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','l','e',' ','n','u','c','l',0xE9,'i','d','e',		// L_PLEASE_SELECT_NUCLIDE (french)  33(2)
'E','n','t','e','r',' ','M','o','/','T','c',' ','L','i','m','i','t',':',		// L_ENTER_MOTC_LIMIT (english)  18(0), 279
'E','n','t','r','e','z',' ','l','i','m','i','t','e',' ','M','o','/','T','c',' ',':',		// L_ENTER_MOTC_LIMIT (french)  21(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','#',' ','o','f',' ','M','e','a','s','u','r','e','m','e','n','t','s',':',		// L_ENTER_NUM_MEASUREMENTS (english)  31(0), 280
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','n','b',' ','d','e',' ','m','e','s','u','r','e','s',' ',':',		// L_ENTER_NUM_MEASUREMENTS (french)  34(0)
'E','n','t','e','r',' ','T','e','s','t',' ','2',' ','T','i','m','e',':',		// L_ENTER_TEST_2_TIME (english)  18(0), 281
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','2',' ',':',		// L_ENTER_TEST_2_TIME (french)  21(0)
'E','n','t','e','r',' ','T','e','s','t',' ','3',' ','T','i','m','e',':',		// L_ENTER_TEST_3_TIME (english)  18(0), 282
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','3',' ',':',		// L_ENTER_TEST_3_TIME (french)  21(0)
'E','n','t','e','r',' ','T','e','s','t',' ','4',' ','T','i','m','e',':',		// L_ENTER_TEST_4_TIME (english)  18(0), 283
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','4',' ',':',		// L_ENTER_TEST_4_TIME (french)  21(0)
'E','n','t','e','r',' ','T','e','s','t',' ','5',' ','T','i','m','e',':',		// L_ENTER_TEST_5_TIME (english)  18(0), 284
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','5',' ',':',		// L_ENTER_TEST_5_TIME (french)  21(0)
'E','n','t','e','r',' ','T','e','s','t',' ','6',' ','T','i','m','e',':',		// L_ENTER_TEST_6_TIME (english)  18(0), 285
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','6',' ',':',		// L_ENTER_TEST_6_TIME (french)  21(0)
'E','n','t','e','r',' ','T','e','s','t',' ','7',' ','T','i','m','e',':',		// L_ENTER_TEST_7_TIME (english)  18(0), 286
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','7',' ',':',		// L_ENTER_TEST_7_TIME (french)  21(0)
'E','n','t','e','r',' ','T','e','s','t',' ','8',' ','T','i','m','e',':',		// L_ENTER_TEST_8_TIME (english)  18(0), 287
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','8',' ',':',		// L_ENTER_TEST_8_TIME (french)  21(0)
'E','n','t','e','r',' ','T','e','s','t',' ','9',' ','T','i','m','e',':',		// L_ENTER_TEST_9_TIME (english)  18(0), 288
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','9',' ',':',		// L_ENTER_TEST_9_TIME (french)  21(0)
'E','n','t','e','r',' ','T','e','s','t',' ','1','0',' ','T','i','m','e',':',		// L_ENTER_TEST_10_TIME (english)  19(0), 289
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','1','0',' ',':',		// L_ENTER_TEST_10_TIME (french)  22(0)
'E','n','t','e','r',' ','T','e','s','t',' ','1','1',' ','T','i','m','e',':',		// L_ENTER_TEST_11_TIME (english)  19(0), 290
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','1','1',' ',':',		// L_ENTER_TEST_11_TIME (french)  22(0)
'E','n','t','e','r',' ','T','e','s','t',' ','1','2',' ','T','i','m','e',':',		// L_ENTER_TEST_12_TIME (english)  19(0), 291
'E','n','t','r','e','z',' ','h','e','u','r','e',' ','t','e','s','t',' ','1','2',' ',':',		// L_ENTER_TEST_12_TIME (french)  22(0)
' ','S','e','c',		// L_SEC (english)  4(0), 292
' ','S','e','c',		// L_SEC (french)  4(0)
' ','s','e','c',		// L_SEC2 (english)  4(0), 293
' ','s','e','c',		// L_SEC2 (french)  4(0)
's','e','c',		// L_SEC3 (english)  3(0), 294
's','e','c',		// L_SEC3 (french)  3(0)
' ','M','i','n',		// L_MIN (english)  4(0), 295
' ','M','i','n',		// L_MIN (french)  4(0)
' ','m','i','n',		// L_MIN2 (english)  4(0), 296
' ','m','i','n',		// L_MIN2 (french)  4(0)
'm','i','n',		// L_MIN3 (english)  3(0), 297
'm','i','n',		// L_MIN3 (french)  3(0)
' ','H','r',		// L_HR (english)  3(0), 298
' ','H','r',		// L_HR (french)  3(0)
' ','h','r',		// L_HR2 (english)  3(0), 299
' ','h','r',		// L_HR2 (french)  3(0)
'h','r',		// L_HR3 (english)  2(0), 300
'h','r',		// L_HR3 (french)  2(0)
' ','H','r',		// L_HR4 (english)  3(0), 301
' ','H','r',		// L_HR4 (french)  3(0)
' ','D','a','y',		// L_DAY (english)  4(0), 302
' ','J','o','u','r',		// L_DAY (french)  5(0)
' ','d','a','y',		// L_DAY2 (english)  4(0), 303
' ','j','o','u','r',		// L_DAY2 (french)  5(0)
'd','a','y',		// L_DAY3 (english)  3(0), 304
'j','o','u','r',		// L_DAY3 (french)  4(0)
' ','Y','r',		// L_YR (english)  3(0), 305
' ','A','n',		// L_YR (french)  3(0)
' ','y','r',		// L_YR2 (english)  3(0), 306
' ','a','n',		// L_YR2 (french)  3(0)
'y','r',		// L_YR3 (english)  2(0), 307
'a','n',		// L_YR3 (french)  2(0)
' ','M','y','r',		// L_MYR (english)  4(0), 308
' ','M','a','n',		// L_MYR (french)  4(0)
'S','e','t','u','p',' ','N','u','c','l','i','d','e',' ','E','r','r','o','r',		// L_SETUP_NUCLIDE_ERROR (english)  19(0), 309
'E','r','r','e','u','r',' ','c','o','n','f','i','g','.',' ','n','u','c','l',0xE9,'i','d','e',		// L_SETUP_NUCLIDE_ERROR (french)  23(1)
'P','l','e','a','s','e',' ','c','o','m','p','l','e','t','e',' ','p','a','r','t','i','a','l',' ','N','u','c','l','i','d','e',' ','e','n','t','r','y','.','\n','R','o','w','(','s',')',':',' ',		// L_PLEASE_COMPLETE_PARTIAL_NUCLIDE_ENTRY (english)  47(0), 310
'C','o','m','p','l',0xE9,'t','e','z',' ','l','a',' ','s','a','i','s','i','e',' ','p','a','r','t','i','e','l','l','e',' ','d','u',' ','n','u','c','l',0xE9,'i','d','e','.','\n','L','i','g','n','e','(','s',')',' ',':',' ',		// L_PLEASE_COMPLETE_PARTIAL_NUCLIDE_ENTRY (french)  54(2)
'D','u','p','l','i','c','a','t','e',' ','N','a','m','e',' ','f','o','u','n','d',':','\n',		// L_DUPLICATE_NAME_FOUND (english)  22(0), 311
'N','o','m',' ','e','n',' ','d','o','u','b','l','e',' ','t','r','o','u','v',0xE9,' ',':','\n',		// L_DUPLICATE_NAME_FOUND (french)  23(1)
'S','e','t','u','p',' ','S','o','u','r','c','e','s',' ','E','r','r','o','r',		// L_SETUP_SOURCES_ERROR (english)  19(0), 312
'E','r','r','e','u','r',' ','c','o','n','f','i','g','.',' ','s','o','u','r','c','e','s',		// L_SETUP_SOURCES_ERROR (french)  22(0)
'C','o','n','s','t','a','n','c','y',' ','S','o','u','r','c','e',' ','s','e','l','e','c','t','e','d',' ','w','i','t','h','o','u','t',' ','C','o','n','s','t','a','n','c','y',' ','C','h','a','n','n','e','l','s',		// L_NO_CONSTANCY_CHANNEL (english)  52(0), 313
'S','o','u','r','c','e',' ','d','e',' ','c','o','n','s','t','a','n','c','e',' ','s',0xE9,'l','e','c','t','i','o','n','n',0xE9,'e',' ','s','a','n','s',' ','c','a','n','a','u','x',' ','d','e',' ','c','o','n','s','t','a','n','c','e',		// L_NO_CONSTANCY_CHANNEL (french)  57(2)
'C','o','n','s','t','a','n','c','y',' ','C','h','a','n','n','e','l','s',' ','s','e','l','e','c','t','e','d',' ','w','i','t','h','o','u','t',' ','C','o','n','s','t','a','n','c','y',' ','S','o','u','r','c','e',		// L_NO_CONSTANCY_SOURCE (english)  52(0), 314
'C','a','n','a','u','x',' ','d','e',' ','c','o','n','s','t','a','n','c','e',' ','s',0xE9,'l','e','c','t','i','o','n','n',0xE9,'s',' ','s','a','n','s',' ','s','o','u','r','c','e',' ','d','e',' ','c','o','n','s','t','a','n','c','e',		// L_NO_CONSTANCY_SOURCE (french)  57(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','5','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','C','i','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CO57_CI (english)  66(0), 315
'A','u','-','d','e','s','s','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','a','x','i','m','a','l','e','\n','C','o','5','7',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','C','i','\n','L','i','m','i','t','e',' ','s','u','p','.',' ',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CO57_CI (french)  78(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','5','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','G','B','q','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CO57_BQ (english)  68(0), 316
'A','u','-','d','e','s','s','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','a','x','i','m','a','l','e','\n','C','o','5','7',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','G','B','q','\n','L','i','m','i','t','e',' ','s','u','p','.',' ',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CO57_BQ (french)  80(2)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','5','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','u','C','i','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CO57_CI (english)  68(0), 317
'E','n',' ','d','e','s','s','o','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','i','n','i','m','a','l','e','\n','C','o','5','7',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','u','C','i','\n','L','i','m','i','t','e',' ','i','n','f','.',' ',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CO57_CI (french)  81(2)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','5','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','6','f',' ','M','B','q','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CO57_BQ (english)  68(0), 318
'E','n',' ','d','e','s','s','o','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','i','n','i','m','a','l','e','\n','C','o','5','7',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','6','f',' ','M','B','q','\n','L','i','m','i','t','e',' ','i','n','f','.',' ',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CO57_BQ (french)  81(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','6','0',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','C','i','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CO60_CI (english)  66(0), 319
'A','u','-','d','e','s','s','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','a','x','i','m','a','l','e','\n','C','o','6','0',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','C','i','\n','L','i','m','i','t','e',' ','s','u','p','.',' ',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CO60_CI (french)  78(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','6','0',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','G','B','q','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CO60_BQ (english)  68(0), 320
'A','u','-','d','e','s','s','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','a','x','i','m','a','l','e','\n','C','o','6','0',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','G','B','q','\n','L','i','m','i','t','e',' ','s','u','p','.',' ',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CO60_BQ (french)  80(2)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','6','0',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','u','C','i','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CO60_CI (english)  68(0), 321
'E','n',' ','d','e','s','s','o','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','i','n','i','m','a','l','e','\n','C','o','6','0',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','u','C','i','\n','L','i','m','i','t','e',' ','i','n','f','.',' ',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CO60_CI (french)  81(2)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','o','6','0',' ','A','c','t','i','v','i','t','y',':',' ','%','.','6','f',' ','M','B','q','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CO60_BQ (english)  68(0), 322
'E','n',' ','d','e','s','s','o','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','i','n','i','m','a','l','e','\n','C','o','6','0',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','6','f',' ','M','B','q','\n','L','i','m','i','t','e',' ','i','n','f','.',' ',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CO60_BQ (french)  81(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','C','i','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_BA133_CI (english)  67(0), 323
'A','u','-','d','e','s','s','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','a','x','i','m','a','l','e','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','C','i','\n','L','i','m','i','t','e',' ','s','u','p','.',' ',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_BA133_CI (french)  79(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','G','B','q','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_BA133_BQ (english)  69(0), 324
'A','u','-','d','e','s','s','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','a','x','i','m','a','l','e','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','G','B','q','\n','L','i','m','i','t','e',' ','s','u','p','.',' ',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_BA133_BQ (french)  81(2)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','u','C','i','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_BA133_CI (english)  69(0), 325
'E','n',' ','d','e','s','s','o','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','i','n','i','m','a','l','e','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','u','C','i','\n','L','i','m','i','t','e',' ','i','n','f','.',' ',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_BA133_CI (french)  82(2)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t','y',':',' ','%','.','6','f',' ','M','B','q','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_BA133_BQ (english)  69(0), 326
'E','n',' ','d','e','s','s','o','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','i','n','i','m','a','l','e','\n','B','a','1','3','3',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','6','f',' ','M','B','q','\n','L','i','m','i','t','e',' ','i','n','f','.',' ',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_BA133_BQ (french)  82(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','C','i','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CS137_CI (english)  67(0), 327
'A','u','-','d','e','s','s','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','a','x','i','m','a','l','e','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','C','i','\n','L','i','m','i','t','e',' ','s','u','p','.',' ',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_CS137_CI (french)  79(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','G','B','q','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CS137_BQ (english)  69(0), 328
'A','u','-','d','e','s','s','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','a','x','i','m','a','l','e','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','G','B','q','\n','L','i','m','i','t','e',' ','s','u','p','.',' ',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_CS137_BQ (french)  81(2)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','u','C','i','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CS137_CI (english)  69(0), 329
'E','n',' ','d','e','s','s','o','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','i','n','i','m','a','l','e','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','u','C','i','\n','L','i','m','i','t','e',' ','i','n','f','.',' ',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_CS137_CI (french)  82(2)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t','y',':',' ','%','.','6','f',' ','M','B','q','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CS137_BQ (english)  69(0), 330
'E','n',' ','d','e','s','s','o','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','i','n','i','m','a','l','e','\n','C','s','1','3','7',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','6','f',' ','M','B','q','\n','L','i','m','i','t','e',' ','i','n','f','.',' ',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_CS137_BQ (french)  82(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','N','a','2','2',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','C','i','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_NA22_CI (english)  66(0), 331
'A','u','-','d','e','s','s','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','a','x','i','m','a','l','e','\n','N','a','2','2',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','C','i','\n','L','i','m','i','t','e',' ','s','u','p','.',' ',':',' ','%','.','3','f',' ','C','i',		// L_ABOVE_MAXIMUM_ACTIVITY_NA22_CI (french)  78(2)
'A','b','o','v','e',' ','M','a','x','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','N','a','2','2',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','G','B','q','\n','U','p','p','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_NA22_BQ (english)  68(0), 332
'A','u','-','d','e','s','s','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','a','x','i','m','a','l','e','\n','N','a','2','2',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','G','B','q','\n','L','i','m','i','t','e',' ','s','u','p','.',' ',':',' ','%','.','3','f',' ','G','B','q',		// L_ABOVE_MAXIMUM_ACTIVITY_NA22_BQ (french)  80(2)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','N','a','2','2',' ','A','c','t','i','v','i','t','y',':',' ','%','.','3','f',' ','u','C','i','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_NA22_CI (english)  68(0), 333
'E','n',' ','d','e','s','s','o','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','i','n','i','m','a','l','e','\n','N','a','2','2',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','3','f',' ','u','C','i','\n','L','i','m','i','t','e',' ','i','n','f','.',' ',':',' ','%','.','3','f',' ','u','C','i',		// L_BELOW_MINIMUM_ACTIVITY_NA22_CI (french)  81(2)
'B','e','l','o','w',' ','M','i','n','i','m','u','m',' ','A','c','t','i','v','i','t','y','\n','N','a','2','2',' ','A','c','t','i','v','i','t','y',':',' ','%','.','6','f',' ','M','B','q','\n','L','o','w','e','r',' ','L','i','m','i','t',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_NA22_BQ (english)  68(0), 334
'E','n',' ','d','e','s','s','o','u','s',' ','d','e',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','m','i','n','i','m','a','l','e','\n','N','a','2','2',' ','A','c','t','i','v','i','t',0xE9,' ',':',' ','%','.','6','f',' ','M','B','q','\n','L','i','m','i','t','e',' ','i','n','f','.',' ',':',' ','%','.','6','f',' ','M','B','q',		// L_BELOW_MINIMUM_ACTIVITY_NA22_BQ (french)  81(2)
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
'F','a','c','t','e','u','r',' ','a','c','t','u','e','l',' ',':',' ','%','.','2','f',		// L_CURRENT_FACTOR (french)  21(0)
'C','h','a','m','b','e','r',' ','S','e','r','i','a','l',':',' ','%','s',		// L_CHAMBER_SERIAL (english)  18(0), 341
'S',0xE9,'r','i','e',' ','c','h','a','m','b','r','e',' ',':',' ','%','s',		// L_CHAMBER_SERIAL (french)  18(1)
'(','C','h','a','m','b','e','r',' ','S','e','r','i','a','l',')',' ','O','l','d',':','%','s',',',' ','N','e','w',':','%','s',		// L_CHAMBER_SERIAL_OLD_NEW (english)  31(0), 342
'(','S',0xE9,'r','i','e',' ','c','h','a','m','b','r','e',')',' ','A','n','c','i','e','n',' ',':','%','s',',',' ','N','o','u','v','e','a','u',' ',':','%','s',		// L_CHAMBER_SERIAL_OLD_NEW (french)  39(1)
'S','e','t','u','p',' ','L','i','n','e','a','t','o','r',' ','E','r','r','o','r',		// L_SETUP_LINEATOR_ERROR (english)  20(0), 343
'E','r','r','e','u','r',' ','c','o','n','f','i','g','.',' ','L','i','n','e','a','t','o','r',		// L_SETUP_LINEATOR_ERROR (french)  23(0)
'S','e','t','u','p',' ','C','a','l','i','c','h','e','c','k',' ','E','r','r','o','r',		// L_SETUP_CALICHECK_ERROR (english)  21(0), 344
'E','r','r','e','u','r',' ','c','o','n','f','i','g','.',' ','C','a','l','i','c','h','e','c','k',		// L_SETUP_CALICHECK_ERROR (french)  24(0)
'P','l','e','a','s','e',' ','f','i','l','l',' ','i','n',' ','a','l','l',' ','m','e','a','s','u','r','e','m','e','n','t','s',		// L_FILL_IN_ALL_MEASUREMENTS (english)  31(0), 345
'V','e','u','i','l','l','e','z',' ','r','e','m','p','l','i','r',' ','t','o','u','t','e','s',' ','l','e','s',' ','m','e','s','u','r','e','s',		// L_FILL_IN_ALL_MEASUREMENTS (french)  35(0)
'P','l','e','a','s','e',' ','f','i','l','l',' ','i','n',' ','t','h','e',' ','N','u','m',' ','o','f',' ','M','e','a','s','u','r','e','m','e','n','t','s',		// L_FILL_IN_NUM_MEASUREMENTS (english)  38(0), 346
'V','e','u','i','l','l','e','z',' ','r','e','m','p','l','i','r',' ','l','e',' ','n','o','m','b','r','e',' ','d','e',' ','m','e','s','u','r','e','s',		// L_FILL_IN_NUM_MEASUREMENTS (french)  37(0)
'P','l','e','a','s','e',' ','f','i','l','l',' ','i','n',' ','L','i','n','e','a','t','o','r',' ','S','e','r','i','a','l',' ','N','u','m','b','e','r',		// L_FILL_IN_LINEATOR_SERIAL_NUMBER (english)  37(0), 347
'V','e','u','i','l','l','e','z',' ','r','e','m','p','l','i','r',' ','l','e',' ','n','u','m',0xE9,'r','o',' ','d','e',' ','s',0xE9,'r','i','e',' ','L','i','n','e','a','t','o','r',		// L_FILL_IN_LINEATOR_SERIAL_NUMBER (french)  44(2)
'P','l','e','a','s','e',' ','f','i','l','l',' ','i','n',' ','C','a','l','i','c','h','e','c','k',' ','S','e','r','i','a','l',' ','N','u','m','b','e','r',		// L_FILL_IN_CALICHECK_SERIAL_NUMBER (english)  38(0), 348
'V','e','u','i','l','l','e','z',' ','r','e','m','p','l','i','r',' ','l','e',' ','n','u','m',0xE9,'r','o',' ','d','e',' ','s',0xE9,'r','i','e',' ','C','a','l','i','c','h','e','c','k',		// L_FILL_IN_CALICHECK_SERIAL_NUMBER (french)  45(2)
'O','l','d',':',' ','%','.','2','f',',',' ','N','e','w',':',' ','%','.','2','f',		// L_FACTOR_OLD_NEW (english)  20(0), 349
'A','n','c','i','e','n',' ',':',' ','%','.','2','f',',',' ','N','o','u','v','e','a','u',' ',':',' ','%','.','2','f',		// L_FACTOR_OLD_NEW (french)  29(0)
'N','e','w',' ','F','a','c','t','o','r',':',' ','%','.','2','f',		// L_NEW_FACTOR (english)  16(0), 350
'N','o','u','v','e','a','u',' ','f','a','c','t','e','u','r',' ',':',' ','%','.','2','f',		// L_NEW_FACTOR (french)  22(0)
'T','h','e',' ','c','u','r','r','e','n','t',' ','f','a','c','t','o','r','s',' ','a','r','e',' ','f','o','r',' ','a',' ','d','i','f','f','e','r','e','n','t',' ','c','h','a','m','b','e','r','\n','a','n','d',' ','c','a','n',' ','n','o','t',' ','b','e',' ','u','s','e','d',' ','f','o','r',' ','t','h','i','s',' ','c','h','a','m','b','e','r',		// L_DIFFERENT_CHAMBERS (english)  84(0), 351
'L','e','s',' ','f','a','c','t','e','u','r','s',' ','a','c','t','u','e','l','s',' ','s','o','n','t',' ','p','o','u','r',' ','u','n','e',' ','a','u','t','r','e',' ','c','h','a','m','b','r','e','\n','e','t',' ','n','e',' ','p','e','u','v','e','n','t',' ','p','a','s',' ',0xEA,'t','r','e',' ','u','t','i','l','i','s',0xE9,'s',' ','p','o','u','r',' ','c','e','t','t','e',' ','c','h','a','m','b','r','e',		// L_DIFFERENT_CHAMBERS (french)  99(2)
'S','e','t','u','p',' ','L','i','n','e','a','t','o','r',		// L_SETUP_LINEATOR (english)  14(0), 352
'C','o','n','f','i','g','u','r','a','t','i','o','n',' ','L','i','n','e','a','t','o','r',		// L_SETUP_LINEATOR (french)  22(0)
'S','e','t','u','p',' ','C','a','l','i','c','h','e','c','k',		// L_SETUP_CALICHECK (english)  15(0), 353
'C','o','n','f','i','g','u','r','a','t','i','o','n',' ','C','a','l','i','c','h','e','c','k',		// L_SETUP_CALICHECK (french)  23(0)
'I','n','v','a','l','i','d',' ','D','a','y',		// L_INVALID_DAY (english)  11(0), 354
'J','o','u','r',' ','i','n','v','a','l','i','d','e',		// L_INVALID_DAY (french)  13(0)
'I','n','v','a','l','i','d',' ','H','o','u','r',		// L_INVALID_HOUR (english)  12(0), 355
'H','e','u','r','e',' ','i','n','v','a','l','i','d','e',		// L_INVALID_HOUR (french)  14(0)
'J','a','n',		// L_JAN (english)  3(0), 356
'j','a','n',		// L_JAN (french)  3(0)
'F','e','b',		// L_FEB (english)  3(0), 357
'f',0xE9,'v',		// L_FEB (french)  3(1)
'M','a','r',		// L_MAR (english)  3(0), 358
'm','a','r',		// L_MAR (french)  3(0)
'A','p','r',		// L_APR (english)  3(0), 359
'a','v','r',		// L_APR (french)  3(0)
'M','a','y',		// L_MAY (english)  3(0), 360
'm','a','i',		// L_MAY (french)  3(0)
'J','u','n',		// L_JUN (english)  3(0), 361
'j','u','n',		// L_JUN (french)  3(0)
'J','u','l',		// L_JUL (english)  3(0), 362
'j','u','l',		// L_JUL (french)  3(0)
'A','u','g',		// L_AUG (english)  3(0), 363
'a','o',0xFB,		// L_AUG (french)  3(1)
'S','e','p',		// L_SEP (english)  3(0), 364
's','e','p',		// L_SEP (french)  3(0)
'O','c','t',		// L_OCT (english)  3(0), 365
'o','c','t',		// L_OCT (french)  3(0)
'N','o','v',		// L_NOV (english)  3(0), 366
'n','o','v',		// L_NOV (french)  3(0)
'D','e','c',		// L_DEC (english)  3(0), 367
'd',0xE9,'c',		// L_DEC (french)  3(1)
'S','u','n',		// L_SUN (english)  3(0), 368
'd','i','m',		// L_SUN (french)  3(0)
'M','o','n',		// L_MON (english)  3(0), 369
'l','u','n',		// L_MON (french)  3(0)
'T','u','e',		// L_TUE (english)  3(0), 370
'm','a','r',		// L_TUE (french)  3(0)
'W','e','d',		// L_WED (english)  3(0), 371
'm','e','r',		// L_WED (french)  3(0)
'T','h','u',		// L_THU (english)  3(0), 372
'j','e','u',		// L_THU (french)  3(0)
'F','r','i',		// L_FRI (english)  3(0), 373
'v','e','n',		// L_FRI (french)  3(0)
'S','a','t',		// L_SAT (english)  3(0), 374
's','a','m',		// L_SAT (french)  3(0)
'S','/','N',' ','M','a','i','n',' ','U','n','i','t',':',		// L_SN_MAIN_UNIT (english)  14(0), 375
'S','/','N',' ','U','n','i','t',0xE9,' ','p','r','i','n','c','i','p','a','l','e',' ',':',		// L_SN_MAIN_UNIT (french)  22(1)
'C','A','L','I','C','H','E','C','K',' ','T','E','S','T',' ','C','A','L','I','B','R','A','T','I','O','N',		// L_CALICHECK_TEST_CALIBRATION (english)  26(0), 376
'C','A','L','I','B','R','A','T','I','O','N',' ','T','E','S','T',' ','C','A','L','I','C','H','E','C','K',		// L_CALICHECK_TEST_CALIBRATION (french)  26(0)
'L','I','N','E','A','T','O','R',' ','T','E','S','T',' ','C','A','L','I','B','R','A','T','I','O','N',' ',		// L_LINEATOR_TEST_CALIBRATION (english)  26(0), 377
'C','A','L','I','B','R','A','T','I','O','N',' ','T','E','S','T',' ','L','I','N','E','A','T','O','R',' ',		// L_LINEATOR_TEST_CALIBRATION (french)  26(0)
'I','n','i','t','i','a','l',' ','F','a','c','t','o','r',':',' ',' ',' ',' ',' ',		// L_INITIAL_FACTOR (english)  20(0), 378
'F','a','c','t','e','u','r',' ','i','n','i','t','i','a','l',' ',':',' ',' ',' ',		// L_INITIAL_FACTOR (french)  20(0)
'C','a','l','i','b','r','a','t','i','o','n',' ','F','a','c','t','o','r',':',' ',		// L_CALIBRATION_FACTOR (english)  20(0), 379
'F','a','c','t','e','u','r',' ','d','e',' ','c','a','l','i','b','r','a','t','i','o','n',' ',':',' ',		// L_CALIBRATION_FACTOR (french)  25(0)
'C','h',' ','S','e','r','i','a','l',')','O','l','d',':','%','s',' ','N','e','w',':','%','s','\t','\t',		// L_CH_SERIAL_OLD_NEW (english)  25(0), 380
'S',0xE9,'r','i','e',' ','C','h',')','A','n','c','i','e','n',' ',':','%','s',' ','N','o','u','v','e','a','u',' ',':','%','s','\t','\t',		// L_CH_SERIAL_OLD_NEW (french)  33(1)
'R',' ','C','h','a','m','b','e','r',		// L_R_CHAMBER (english)  9(0), 381
'c','h','a','m','b','r','e',' ','R',		// L_R_CHAMBER (french)  9(0)
'P',' ','C','h','a','m','b','e','r',		// L_P_CHAMBER (english)  9(0), 382
'c','h','a','m','b','r','e',' ','P',		// L_P_CHAMBER (french)  9(0)
'S','e','t','u','p',' ','R','e','m','o','t','e',' ','N','u','c','l','i','d','e','s',		// L_SETUP_REMOTE_NUCLIDES (english)  21(0), 383
'C','o','n','f','i','g','.',' ','n','u','c','l',0xE9,'i','d','e','s',' ','d','i','s','t','a','n','t','s',		// L_SETUP_REMOTE_NUCLIDES (french)  26(1)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','P','a','u','s','e','d','!','!','!',		// L_AUTOLINEARITY_PAUSED (english)  23(0), 384
'A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','e','n',' ','p','a','u','s','e',' ','!','!','!',		// L_AUTOLINEARITY_PAUSED (french)  26(2)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','C','a','l',' ','#',':',		// L_PLEASE_ENTER_CAL_NUM (english)  19(0), 385
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','C','a','l',' ','#',' ',':',		// L_PLEASE_ENTER_CAL_NUM (french)  23(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','D','o','s','e',' ','T','i','m','e',':',		// L_PLEASE_ENTER_DOSE_TIME (english)  23(0), 386
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l',0x27,'h','e','u','r','e',' ','d','e',' ','d','o','s','e',' ',':',		// L_PLEASE_ENTER_DOSE_TIME (french)  33(0)
'L','o','w',' ','B','a','t','t','e','r','y',		// L_LOW_BATTERY (english)  11(0), 387
'B','a','t','t','e','r','i','e',' ','f','a','i','b','l','e',		// L_LOW_BATTERY (french)  15(0)
'D','o','s','e',' ','D','e','c','a','y',		// L_DOSE_DECAY (english)  10(0), 388
'D',0xE9,'c','r','o','i','s','s','a','n','c','e',' ','d','o','s','e',		// L_DOSE_DECAY (french)  17(1)
'H','o','m','e','\n',' ','S','c','r','e','e','n',		// L_HOME_SCREEN (english)  12(0), 389
0xC9,'c','r','a','n','\n','d',0x27,'a','c','c','u','e','i','l',		// L_HOME_SCREEN (french)  15(1)
'N','u','c','l','i','d','e','\n',' ','S','c','r','e','e','n',		// L_NUCLIDE_SCREEN (english)  15(0), 390
0xC9,'c','r','a','n','\n','n','u','c','l',0xE9,'i','d','e',		// L_NUCLIDE_SCREEN (french)  14(2)
'Y','E','S',		// L_BTN_YES (english)  3(0), 391
'O','U','I',		// L_BTN_YES (french)  3(0)
'N','O',		// L_BTN_NO (english)  2(0), 392
'N','O','N',		// L_BTN_NO (french)  3(0)
'D','e','l','e','t','e',' ','L','i','n','e','a','t','o','r',' ','S','e','t','t','i','n','g','s',		// L_DELETE_LINEATOR_SETTINGS (english)  24(0), 393
'S','u','p','p','r','i','m','e','r',' ','r',0xE9,'g','l','a','g','e','s',' ','L','i','n','e','a','t','o','r',		// L_DELETE_LINEATOR_SETTINGS (french)  27(1)
'D','e','l','e','t','e',' ','C','a','l','i','c','h','e','c','k',' ','S','e','t','t','i','n','g','s',		// L_DELETE_CALICHECK_SETTINGS (english)  25(0), 394
'S','u','p','p','r','i','m','e','r',' ','r',0xE9,'g','l','a','g','e','s',' ','C','a','l','i','c','h','e','c','k',		// L_DELETE_CALICHECK_SETTINGS (french)  28(1)
'D','e','l','e','t','e',' ','L','i','n','e','a','t','o','r',' ','S','e','t','t','i','n','g','s','?',		// L_DELETE_LINEATOR_SETTINGS_Q (english)  25(0), 395
'S','u','p','p','r','i','m','e','r',' ','r',0xE9,'g','l','a','g','e','s',' ','L','i','n','e','a','t','o','r',' ','?',		// L_DELETE_LINEATOR_SETTINGS_Q (french)  29(1)
'D','e','l','e','t','e',' ','C','a','l','i','c','h','e','c','k',' ','S','e','t','t','i','n','g','s','?',		// L_DELETE_CALICHECK_SETTINGS_Q (english)  26(0), 396
'S','u','p','p','r','i','m','e','r',' ','r',0xE9,'g','l','a','g','e','s',' ','C','a','l','i','c','h','e','c','k',' ','?',		// L_DELETE_CALICHECK_SETTINGS_Q (french)  30(1)
'U','t','i','l','i','t','y',		// L_UTILITY (english)  7(0), 397
'U','t','i','l','i','t','a','i','r','e',		// L_UTILITY (french)  10(0)
'[','C','i',',','B','q',' ','C','o','n','v',']',' ','I','n','p','u','t',' ','A','c','t','i','v','i','t','y',':',		// L_CONV_INPUT_ACTIVITY (english)  28(0), 398
'[','C','i',',','B','q',' ','C','o','n','v',']',' ','E','n','t','r','e','r',' ','a','c','t','i','v','i','t',0xE9,' ',':',		// L_CONV_INPUT_ACTIVITY (french)  30(1)
'[','D','e','c','a','y',' ','C','a','l','c','u','l','a','t','o','r',']',		// L_DECAY_CALCULATOR (english)  18(0), 399
'[','C','a','l','c','u','l','a','t','e','u','r',' ','d',0xE9,'c','r','o','i','s','s','a','n','c','e',']',		// L_DECAY_CALCULATOR (french)  26(1)
'F','R','O','M',':',		// L_FROM (english)  5(0), 400
'D','E',' ',':',		// L_FROM (french)  4(0)
'F','r','o','m',':',		// L_FROM2 (english)  5(0), 401
'D','e',' ',':',		// L_FROM2 (french)  4(0)
'T','O',':',		// L_TO (english)  3(0), 402
0xC0,' ',':',		// L_TO (french)  3(1)
'T','o',':',		// L_TO2 (english)  3(0), 403
0xC0,' ',':',		// L_TO2 (french)  3(1)
'D','i','a','g','n','o','s','t','i','c','s',		// L_DIAGNOSTICS (english)  11(0), 404
'D','i','a','g','n','o','s','t','i','c','s',		// L_DIAGNOSTICS (french)  11(0)
'D','o','s','e',' ','T','a','b','l','e',		// L_DOSE_TABLE (english)  10(0), 405
'T','a','b','l','e',' ','d','e',' ','d','o','s','e','s',		// L_DOSE_TABLE (french)  14(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','A','c','t','i','v','i','t','y',':',		// L_PLEASE_ENTER_ACTIVITY (english)  22(0), 406
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ',':',		// L_PLEASE_ENTER_ACTIVITY (french)  28(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','S','t','a','r','t',' ','T','i','m','e',':',		// L_PLEASE_ENTER_START_TIME (english)  24(0), 407
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l',0x27,'h','e','u','r','e',' ','d','e',' ','d',0xE9,'b','u','t',' ',':',		// L_PLEASE_ENTER_START_TIME (french)  34(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','E','n','d',' ','T','i','m','e',':',		// L_PLEASE_ENTER_END_TIME (english)  22(0), 408
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l',0x27,'h','e','u','r','e',' ','d','e',' ','f','i','n',' ',':',		// L_PLEASE_ENTER_END_TIME (french)  32(0)
'T','O','O',' ','F','A','R',' ','I','N','T','O',' ','F','U','T','U','R','E',		// L_TOO_FAR_INTO_FUTURE (english)  19(0), 409
'T','R','O','P',' ','L','O','I','N',' ','D','A','N','S',' ','L','E',' ','F','U','T','U','R',		// L_TOO_FAR_INTO_FUTURE (french)  23(0)
'T','O','O',' ','F','A','R',' ','I','N','T','O',' ','P','A','S','T',		// L_TOO_FAR_INTO_PAST (english)  17(0), 410
'T','R','O','P',' ','L','O','I','N',' ','D','A','N','S',' ','L','E',' ','P','A','S','S',0xC9,		// L_TOO_FAR_INTO_PAST (french)  23(1)
'T','O','O',' ','L','A','R','G','E',		// L_TOO_LARGE (english)  9(0), 411
'T','R','O','P',' ','G','R','A','N','D',		// L_TOO_LARGE (french)  10(0)
'P','R','O','G','R','A','M',' ','I','N','T','E','G','R','I','T','Y',		// L_PROGRAM_INTEGRITY (english)  17(0), 412
'I','N','T',0xC9,'G','R','I','T',0xC9,' ','D','U',' ','P','R','O','G','R','A','M','M','E',		// L_PROGRAM_INTEGRITY (french)  22(2)
'F','A','I','L',':',' ',		// L_FAIL (english)  6(0), 413
0xC9,'C','H','E','C',' ',':',' ',		// L_FAIL (french)  8(1)
'F','A','I','L',		// L_FAIL2 (english)  4(0), 414
0xC9,'C','H','E','C',		// L_FAIL2 (french)  5(1)
'P','A','S','S',':',' ',		// L_PASS (english)  6(0), 415
'R',0xC9,'U','S','S','I',' ',':',' ',		// L_PASS (french)  9(1)
'P','A','S','S',		// L_PASS2 (english)  4(0), 416
'R',0xC9,'U','S','S','I',		// L_PASS2 (french)  6(1)
'P','a','s','s','e','d',		// L_PASSED (english)  6(0), 417
'R',0xE9,'u','s','s','i',		// L_PASSED (french)  6(1)
'F','a','i','l','e','d',',',' ',		// L_FAILED (english)  8(0), 418
0xC9,'c','h','o','u',0xE9,',',' ',		// L_FAILED (french)  8(2)
'D','a','i','l','y',' ','T','e','s','t',		// L_DAILY_TEST (english)  10(0), 419
'T','e','s','t',' ','q','u','o','t','i','d','i','e','n',		// L_DAILY_TEST (french)  14(0)
'D','A','I','L','Y',' ','T','E','S','T',':',		// L_DAILY_TEST2 (english)  11(0), 420
'T','E','S','T',' ','Q','U','O','T','I','D','I','E','N',' ',':',		// L_DAILY_TEST2 (french)  16(0)
'1',')',' ','R','e','m','o','v','e',' ','A','l','l',' ','S','o','u','r','c','e','s',' ','f','r','o','m',' ','C','h','a','m','b','e','r',		// L_REMOVE_ALL_SOURCES_FROM_CHAMBER (english)  34(0), 421
'1',')',' ','R','e','t','i','r','e','r',' ','t','o','u','t','e','s',' ','l','e','s',' ','s','o','u','r','c','e','s',' ','d','e',' ','l','a',' ','c','h','a','m','b','r','e',		// L_REMOVE_ALL_SOURCES_FROM_CHAMBER (french)  43(0)
'2',')',' ','A','u','t','o',' ','Z','e','r','o',		// L_AUTO_ZERO (english)  12(0), 422
'2',')',' ','A','u','t','o',' ','z',0xE9,'r','o',		// L_AUTO_ZERO (french)  12(1)
'3',')',' ','M','e','a','s','u','r','e',' ','B','a','c','k','g','n','d',		// L_MEASURE_BACKGROUND (english)  18(0), 423
'3',')',' ','M','e','s','u','r','e',' ','b','r','u','i','t',' ','d','e',' ','f','o','n','d',		// L_MEASURE_BACKGROUND (french)  23(0)
'2',')',' ','M','e','a','s','u','r','e',' ','B','a','c','k','g','n','d',		// L_MEASURE_BACKGROUND2 (english)  18(0), 424
'2',')',' ','M','e','s','u','r','e',' ','b','r','u','i','t',' ','d','e',' ','f','o','n','d',		// L_MEASURE_BACKGROUND2 (french)  23(0)
'4',')',' ','C','h','e','c','k',' ','C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',		// L_CHECK_CHAMBER_VOLTAGE (english)  24(0), 425
'4',')',' ','V',0xE9,'r','i','f','i','e','r',' ','t','e','n','s','i','o','n',' ','c','h','a','m','b','r','e',		// L_CHECK_CHAMBER_VOLTAGE (french)  27(1)
'1',')',' ','C','h','e','c','k',' ','C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',		// L_CHECK_CHAMBER_VOLTAGE2 (english)  24(0), 426
'1',')',' ','V',0xE9,'r','i','f','i','e','r',' ','t','e','n','s','i','o','n',' ','c','h','a','m','b','r','e',		// L_CHECK_CHAMBER_VOLTAGE2 (french)  27(1)
'5',')',' ','D','a','t','a',' ','C','h','e','c','k',		// L_DATA_CHECK (english)  13(0), 427
'5',')',' ','V',0xE9,'r','i','f','i','c','a','t','i','o','n',' ','d','o','n','n',0xE9,'e','s',		// L_DATA_CHECK (french)  23(2)
'A','c','c','u','r','a','c','y',		// L_ACCURACY (english)  8(0), 428
'E','x','a','c','t','i','t','u','d','e',		// L_ACCURACY (french)  10(0)
'Z','e','r','o',' ','D','r','i','f','t',		// L_ZERO_DRIFT (english)  10(0), 429
'D',0xE9,'r','i','v','e',' ','d','u',' ','z',0xE9,'r','o',		// L_ZERO_DRIFT (french)  14(2)
'Z','e','r','o',' ','o','u','t',' ','o','f',' ','R','a','n','g','e',		// L_ZERO_OUT_OF_RANGE (english)  17(0), 430
'Z',0xE9,'r','o',' ','h','o','r','s',' ','p','l','a','g','e',		// L_ZERO_OUT_OF_RANGE (french)  15(1)
'B','A','C','K','G','R','O','U','N','D',' ','T','O','O',' ','H','I','G','H',		// L_BACKGROUND_TOO_HIGH (english)  19(0), 431
'B','R','U','I','T',' ','D','E',' ','F','O','N','D',' ','T','R','O','P',' ',0xC9,'L','E','V',0xC9,		// L_BACKGROUND_TOO_HIGH (french)  24(2)
'B','A','C','K','G','R','O','U','N','D','\n',' ','T','O','O',' ','H','I','G','H',		// L_BACKGROUND_TOO_HIGH2 (english)  20(0), 432
'B','R','U','I','T',' ','D','E',' ','F','O','N','D','\n',' ','T','R','O','P',' ',0xC9,'L','E','V',0xC9,		// L_BACKGROUND_TOO_HIGH2 (french)  25(2)
'B','a','c','k','g','r','o','u','n','d',' ','T','o','o',' ','H','i','g','h',		// L_BACKGROUND_TOO_HIGH3 (english)  19(0), 433
'B','r','u','i','t',' ','d','e',' ','f','o','n','d',' ','t','r','o','p',' ',0xE9,'l','e','v',0xE9,		// L_BACKGROUND_TOO_HIGH3 (french)  24(2)
'6',')',' ','N','O',' ','D','A','I','L','Y',' ','S','O','U','R','C','E',' ','D','A','T','A',		// L_NO_DAILY_SOURCE_DATA (english)  23(0), 434
'6',')',' ','P','A','S',' ','D','E',' ','D','O','N','N',0xC9,'E','S',' ','S','O','U','R','C','E',' ','Q','U','O','T','I','D','.',		// L_NO_DAILY_SOURCE_DATA (french)  32(1)
'P','L','E','A','S','E',' ','W','A','I','T',' ','2',' ','M','i','n','u','t','e','s',' ','f','o','r',		// L_PLEASE_WAIT_2_MINUTES_FOR (english)  25(0), 435
'V','E','U','I','L','L','E','Z',' ','P','A','T','I','E','N','T','E','R',' ','2',' ','m','i','n','u','t','e','s',' ','p','o','u','r',		// L_PLEASE_WAIT_2_MINUTES_FOR (french)  33(0)
'S','t','a','b','i','l','i','z','a','t','i','o','n',		// L_STABILIZATION (english)  13(0), 436
'S','t','a','b','i','l','i','s','a','t','i','o','n',		// L_STABILIZATION (french)  13(0)
'P','L','E','A','S','E',' ','W','A','I','T',' ','2',' ','M','i','n','u','t','e','s',' ','f','o','r',' ','S','t','a','b','i','l','i','z','a','t','i','o','n',		// L_PLEASE_WAIT_2_MINUTES_FOR_STABILIZATION (english)  39(0), 437
'V','E','U','I','L','L','E','Z',' ','P','A','T','I','E','N','T','E','R',' ','2',' ','m','i','n','u','t','e','s',' ','p','o','u','r',' ','l','a',' ','s','t','a','b','i','l','i','s','a','t','i','o','n',		// L_PLEASE_WAIT_2_MINUTES_FOR_STABILIZATION (french)  50(0)
'H','I','G','H',		// L_HIGH (english)  4(0), 438
0xC9,'L','E','V',0xC9,		// L_HIGH (french)  5(2)
'C','h','a','m','b','e','r',		// L_CHAMBER (english)  7(0), 439
'C','h','a','m','b','r','e',		// L_CHAMBER (french)  7(0)
'Z','E','R','O',':',		// L_ZERO (english)  5(0), 440
'Z',0xC9,'R','O',' ',':',		// L_ZERO (french)  6(1)
'B','A','C','K','G','R','O','U','N','D',':',		// L_BACKGROUND (english)  11(0), 441
'B','R','U','I','T',' ','D','E',' ','F','O','N','D',' ',':',		// L_BACKGROUND (french)  15(0)
'B','a','c','k','g','r','o','u','n','d',		// L_BACKGROUND2 (english)  10(0), 442
'B','r','u','i','t',' ','d','e',' ','f','o','n','d',		// L_BACKGROUND2 (french)  13(0)
'C','H','A','M','B','E','R',' ','V','O','L','T','A','G','E',':',		// L_CHAMBER_VOLTAGE (english)  16(0), 443
'T','E','N','S','I','O','N',' ','C','H','A','M','B','R','E',' ',':',		// L_CHAMBER_VOLTAGE (french)  17(0)
'V','o','l','t','s',		// L_VOLTS (english)  5(0), 444
'V',		// L_VOLTS (french)  1(0)
'D','A','T','A',' ','C','H','E','C','K',':',		// L_DATA_CHECK_PRINTED (english)  11(0), 445
'V',0xC9,'R','I','F','.',' ','D','O','N','N',0xC9,'E','S',' ',':',		// L_DATA_CHECK_PRINTED (french)  16(2)
's','i','g','n','a','t','u','r','e',		// L_SIGNATURE (english)  9(0), 446
's','i','g','n','a','t','u','r','e',		// L_SIGNATURE (french)  9(0)
'V','o','l','u','m','e',		// L_DOSE_TABLE_VOLUME (english)  6(0), 447
'V','o','l','u','m','e',		// L_DOSE_TABLE_VOLUME (french)  6(0)
'D','o','s','e',		// L_DOSE (english)  4(0), 448
'D','o','s','e',		// L_DOSE (french)  4(0)
'I','n','t','e','r','v','a','l',		// L_INTERVAL (english)  8(0), 449
'I','n','t','e','r','v','a','l','l','e',		// L_INTERVAL (french)  10(0)
'I','n','t','e','r','v','a','l',':',		// L_INTERVAL2 (english)  9(0), 450
'I','n','t','e','r','v','a','l','l','e',' ',':',		// L_INTERVAL2 (french)  12(0)
'M','e','a','s','u','r','e',' ','A','c','t','i','v','i','t','y',		// L_MEASURE_ACTIVITY (english)  16(0), 451
'M','e','s','u','r','e','r',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,		// L_MEASURE_ACTIVITY (french)  18(1)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','V','o','l','u','m','e','(','m','l',')',':',		// L_PLEASE_ENTER_VOLUME (english)  24(0), 452
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','v','o','l','u','m','e',' ','(','m','l',')',' ',':',		// L_PLEASE_ENTER_VOLUME (french)  32(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','D','o','s','e',		// L_PLEASE_ENTER_DOSE (english)  17(0), 453
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','a',' ','d','o','s','e',		// L_PLEASE_ENTER_DOSE (french)  23(0)
'E','n','t','e','r',' ','I','n','t','e','r','v','a','l',' ','i','n',' ','m','i','n','u','t','e','s',':',		// L_ENTER_INTERVAL (english)  26(0), 454
'E','n','t','r','e','z',' ','l',0x27,'i','n','t','e','r','v','a','l','l','e',' ','e','n',' ','m','i','n','u','t','e','s',' ',':',		// L_ENTER_INTERVAL (french)  32(0)
'T','I','M','E',		// L_PR_TIME (english)  4(0), 455
'H','E','U','R','E',		// L_PR_TIME (french)  5(0)
'D','o','s','e',' ','T','a','b','l','e',' ','E','r','r','o','r',		// L_DOSE_TABLE_ERROR (english)  16(0), 456
'E','r','r','e','u','r',' ','t','a','b','l','e',' ','d','e',' ','d','o','s','e','s',		// L_DOSE_TABLE_ERROR (french)  21(0)
'D','o','s','e',' ','c','a','n',' ','n','o','t',' ','b','e',' ','l','a','r','g','e','r',' ','t','h','a','n',' ','i','n','i','t','i','a','l',' ','A','c','t','i','v','i','t','y','.',		// L_DOSE_LARGER_THAN_ACTIVITY (english)  45(0), 457
'L','a',' ','d','o','s','e',' ','n','e',' ','p','e','u','t',' ','p','a','s',' ','d',0xE9,'p','a','s','s','e','r',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ','i','n','i','t','i','a','l','e','.',		// L_DOSE_LARGER_THAN_ACTIVITY (french)  49(2)
' ','-',' ','E','x','c','e','e','d','s',' ','I','n','i','t','i','a','l',		// L_EXCEEDS_INITIAL (english)  18(0), 458
' ','-',' ','D',0xE9,'p','a','s','s','e',' ','l',0x27,'i','n','i','t','i','a','l',		// L_EXCEEDS_INITIAL (french)  20(1)
'D','O','S','E',' ','T','A','B','L','E',		// L_PR_DOSE_TABLE (english)  10(0), 459
'T','A','B','L','E',' ','D','E',' ','D','O','S','E','S',		// L_PR_DOSE_TABLE (french)  14(0)
'V','O','L','U','M','E',' ','(','m','l',')',		// L_PR_VOLUME_HEADING (english)  11(0), 460
'V','O','L','U','M','E',' ','(','m','l',')',		// L_PR_VOLUME_HEADING (french)  11(0)
'E','x','c','e','e','d','s',' ','I','n','i','t','i','a','l',' ','V','o','l',		// L_PR_EXCEEDS_INITIAL (english)  19(0), 461
'V','o','l','.',' ','i','n','i','t','i','a','l',' ','d',0xE9,'p','a','s','s',0xE9,		// L_PR_EXCEEDS_INITIAL (french)  20(2)
'N','U','C','L','I','D','E',' ','D','A','T','A',':',		// L_PR_NUCLIDE_DATA (english)  13(0), 462
'D','O','N','N',0xC9,'E','S',' ','N','U','C','L',0xC9,'I','D','E',' ',':',		// L_PR_NUCLIDE_DATA (french)  18(2)
'T','E','S','T',' ','S','O','U','R','C','E','S',':',		// L_PR_TEST_SOURCES (english)  13(0), 463
'S','O','U','R','C','E','S',' ','D','E',' ','T','E','S','T',' ',':',		// L_PR_TEST_SOURCES (french)  17(0)
'N','o',' ','S','o','u','r','c','e',		// L_PR_NO_SOURCE (english)  9(0), 464
'A','u','c','u','n','e',' ','s','o','u','r','c','e',		// L_PR_NO_SOURCE (french)  13(0)
'D',':',' ','D','a','i','l','y',' ',' ','C',':',' ','C','o','n','s','t','a','n','c','y',' ','S','o','u','r','c','e',		// L_PR_DAILY_CONSTANCY (english)  29(0), 465
'Q',' ',':',' ','Q','u','o','t','i','d','i','e','n',' ',' ','C',' ',':',' ','S','o','u','r','c','e',' ','c','o','n','s','t','a','n','c','e',		// L_PR_DAILY_CONSTANCY (french)  35(0)
'R','e','s','p','o','n','s','e',' ','C','o','r','r','e','c','t','i','o','n',':',		// L_PR_RESPONSE_CORRECTION (english)  20(0), 466
'C','o','r','r','e','c','t','i','o','n',' ','d','e',' ','r',0xE9,'p','o','n','s','e',' ',':',		// L_PR_RESPONSE_CORRECTION (french)  23(1)
'(','C','o','6','0',' ','w','a','s',' ','L','O','W',' ',')',		// L_PR_WAS_LOW (english)  15(0), 467
'(','C','o','6','0',' ',0xE9,'t','a','i','t',' ','B','A','S',' ',')',		// L_PR_WAS_LOW (french)  17(1)
'(','C','o','6','0',' ','w','a','s',' ','H','I','G','H',')',		// L_PR_WAS_HIGH (english)  15(0), 468
'(','C','o','6','0',' ',0xE9,'t','a','i','t',' ','H','A','U','T',')',		// L_PR_WAS_HIGH (french)  17(1)
'N','o','m','i','n','a','l',' ','V','o','l','t','a','g','e',':',' ',		// L_PR_NOMINAL_VOLTAGE (english)  17(0), 469
'T','e','n','s','i','o','n',' ','n','o','m','i','n','a','l','e',' ',':',' ',		// L_PR_NOMINAL_VOLTAGE (french)  19(0)
'M','E','M','O','R','Y',' ','S','T','A','T','U','S',		// L_PR_MEMORY_STATUS (english)  13(0), 470
0xC9,'T','A','T',' ','D','E',' ','L','A',' ','M',0xC9,'M','O','I','R','E',		// L_PR_MEMORY_STATUS (french)  18(2)
'A','v','a','i','l','a','b','l','e',':',		// L_PR_AVAILABLE (english)  10(0), 471
'D','i','s','p','o','n','i','b','l','e',' ',':',		// L_PR_AVAILABLE (french)  12(0)
'T','o','t','a','l',':',		// L_PR_TOTAL (english)  6(0), 472
'T','o','t','a','l',' ',':',		// L_PR_TOTAL (french)  7(0)
'R',' ','C','H','A','M','B','E','R',		// L_PR_R_CHAMBER (english)  9(0), 473
'C','H','A','M','B','R','E',' ','R',		// L_PR_R_CHAMBER (french)  9(0)
'P','E','T',' ','C','H','A','M','B','E','R',		// L_PR_PET_CHAMBER (english)  11(0), 474
'C','H','A','M','B','R','E',' ','P','E','T',		// L_PR_PET_CHAMBER (french)  11(0)
'P','E','T',' ','C','h','a','m','b','e','r',		// L_PET_CHAMBER (english)  11(0), 475
'C','h','a','m','b','r','e',' ','P','E','T',		// L_PET_CHAMBER (french)  11(0)
'M','e','a','s','u','r','e','d',':',		// L_MEASURED (english)  9(0), 476
'M','e','s','u','r',0xE9,' ',':',		// L_MEASURED (french)  8(1)
'M','e','a','s','u','r','e','d',		// L_MEASURED2 (english)  8(0), 477
'M','e','s','u','r',0xE9,		// L_MEASURED2 (french)  6(1)
'M','e','a','s','u','r','e','d',		// L_MEASURED3 (english)  8(0), 478
'M','e','s','u','r',0xE9,		// L_MEASURED3 (french)  6(1)
'C','a','l','c','u','l','a','t','e','d',':',		// L_CALCULATED (english)  11(0), 479
'C','a','l','c','u','l',0xE9,' ',':',		// L_CALCULATED (french)  9(1)
'A','u','t','o','C','o','n','s','t','a','n','c','y',		// L_AUTOCONSTANCY (english)  13(0), 480
'A','u','t','o','C','o','n','s','t','a','n','c','e',		// L_AUTOCONSTANCY (french)  13(0)
'N','o',' ','S','o','u','r','c','e',' ','D','a','t','a',		// L_NOSOURCEDATA (english)  14(0), 481
'A','u','c','u','n','e',' ','d','o','n','n',0xE9,'e',' ','s','o','u','r','c','e',		// L_NOSOURCEDATA (french)  20(1)
'D','e','v','i','a','t','i','o','n',':',		// L_DEVIATION (english)  10(0), 482
0xC9,'c','a','r','t',' ',':',		// L_DEVIATION (french)  7(1)
'U','S','E','R',' ','C','H','A','M','B','E','R',' ','N','U','C','L','I','D','E','S',':',		// L_PR_USER_CHAMBER_NUCLIDES (english)  22(0), 483
'N','U','C','L',0xC9,'I','D','E','S',' ','C','H','A','M','B','R','E',' ','U','T','I','L','I','S','A','T','E','U','R',' ',':',		// L_PR_USER_CHAMBER_NUCLIDES (french)  31(1)
'H','V',' ','d','i','r','e','c','t','l','y',' ','i','n','t','o',' ','A','D','C',		// L_PR_HV_DIRECTLY_INTO_ADC (english)  20(0), 484
'H','V',' ','d','i','r','e','c','t','e','m','e','n','t',' ','d','a','n','s',' ','A','D','C',		// L_PR_HV_DIRECTLY_INTO_ADC (french)  23(0)
'S','i','n','g','l','e',' ','G','a','i','n',' ','R','e','l','a','y',		// L_PR_SINGLE_GAIN_RELAY (english)  17(0), 485
'R','e','l','a','i','s',' ','d','e',' ','g','a','i','n',' ','u','n','i','q','u','e',		// L_PR_SINGLE_GAIN_RELAY (french)  21(0)
'G','a','i','n',':',' ','%','4','.','6','f',		// L_PR_GAIN (english)  11(0), 486
'G','a','i','n',' ',':',' ','%','4','.','6','f',		// L_PR_GAIN (french)  12(0)
'M',		// L_PR_TIME_UNIT_1 (english)  1(0), 487
'M',		// L_PR_TIME_UNIT_1 (french)  1(0)
'H',		// L_PR_TIME_UNIT_2 (english)  1(0), 488
'H',		// L_PR_TIME_UNIT_2 (french)  1(0)
'D',		// L_PR_TIME_UNIT_3 (english)  1(0), 489
'J',		// L_PR_TIME_UNIT_3 (french)  1(0)
'Y',		// L_PR_TIME_UNIT_4 (english)  1(0), 490
'A',		// L_PR_TIME_UNIT_4 (french)  1(0)
'M','Y',		// L_PR_TIME_UNIT_5 (english)  2(0), 491
'M','A',		// L_PR_TIME_UNIT_5 (french)  2(0)
'H','a','l','f','-','L','i','f','e',		// L_PR_HALFLIFE (english)  9(0), 492
'D','e','m','i','-','v','i','e',		// L_PR_HALFLIFE (french)  8(0)
'H','a','l','f','l','i','f','e',':',		// L_PR_HALFLIFE2 (english)  9(0), 493
'D','e','m','i','-','v','i','e',' ',':',		// L_PR_HALFLIFE2 (french)  10(0)
'C','U','R','R','E','N','T',		// L_PR_CURRENT (english)  7(0), 494
'A','C','T','U','E','L',		// L_PR_CURRENT (french)  6(0)
'C','A','L','I','B','R','A','T','I','O','N',		// L_PR_CALIBRATION (english)  11(0), 495
'C','A','L','I','B','R','A','T','I','O','N',		// L_PR_CALIBRATION (french)  11(0)
'U','S','E','R',' ','N','U','C','L','I','D','E','S',':',		// L_PR_USER_NUCLIDES (english)  14(0), 496
'N','U','C','L',0xC9,'I','D','E','S',' ','U','T','I','L','I','S','A','T','E','U','R',' ',':',		// L_PR_USER_NUCLIDES (french)  23(1)
'(','C','o','6','0',' ','H','I','G','H',')',		// L_PR_CO60_HIGH (english)  11(0), 497
'(','C','o','6','0',' ','H','A','U','T',')',		// L_PR_CO60_HIGH (french)  11(0)
'(','C','o','6','0',' ','L','O','W',' ',')',		// L_PR_CO60_LOW (english)  11(0), 498
'(','C','o','6','0',' ','B','A','S',' ',')',		// L_PR_CO60_LOW (french)  11(0)
'A','C','C','U','R','A','C','Y',' ','T','E','S','T',':',		// L_ACCURACYTEST (english)  14(0), 499
'T','E','S','T',' ','D',0x27,'E','X','A','C','T','I','T','U','D','E',' ',':',		// L_ACCURACYTEST (french)  19(0)
'S','o','u','r','c','e',':',		// L_SOURCE (english)  7(0), 500
'S','o','u','r','c','e',' ',':',		// L_SOURCE (french)  8(0)
'S','e','r','i','a','l',' ','#',':',		// L_SERIALNUM (english)  9(0), 501
'#',' ','d','e',' ','s',0xE9,'r','i','e',' ',':',		// L_SERIALNUM (french)  12(1)
'S','t','a','n','d','.',' ','S','o','u','r','c','e','(','S',')',':',		// L_STANDSOURCE (english)  17(0), 502
'S','o','u','r','c','e',' ','s','t','d','.',' ','(','S',')',' ',':',		// L_STANDSOURCE (french)  17(0)
'M','e','a','s','u','r','e','d',' ','A','s',' ',' ','(','M',')',':',		// L_MEASUREDAS (english)  17(0), 503
'M','e','s','u','r',0xE9,' ','c','o','m','m','e',' ','(','M',')',' ',':',		// L_MEASUREDAS (french)  18(1)
'D','e','v','i','a','t','i','o','n','(','M','-','S',')','/','S',':',		// L_DEVIATIONMSS (english)  17(0), 504
0xC9,'c','a','r','t',' ',' ',' ','(','M','-','S',')','/','S',' ',':',		// L_DEVIATIONMSS (french)  17(1)
'C','O','N','S','T','A','N','C','Y',' ','T','E','S','T',':',		// L_CONSTANCYTEST (english)  15(0), 505
'T','E','S','T',' ','D','E',' ','C','O','N','S','T','A','N','C','E',' ',':',		// L_CONSTANCYTEST (french)  19(0)
'F','i','n','i','s','h','e','d',		// L_FINISHED (english)  8(0), 506
'T','e','r','m','i','n',0xE9,		// L_FINISHED (french)  7(1)
'F','i','n','i','s','h','e','d','!','!','!',		// L_FINISHED2 (english)  11(0), 507
'T','e','r','m','i','n',0xE9,' ','!','!','!',		// L_FINISHED2 (french)  11(1)
'C','h','a','m','b','e','r',' ','V','o','l','t','s',		// L_CHAMBER_VOLTS (english)  13(0), 508
'T','e','n','s','i','o','n',' ','c','h','a','m','b','r','e',		// L_CHAMBER_VOLTS (french)  15(0)
'E','n','h','a','n','c','e','d',' ','T','e','s','t','s',		// L_ENHANCED_TESTS (english)  14(0), 509
'T','e','s','t','s',' ','a','m',0xE9,'l','i','o','r',0xE9,'s',		// L_ENHANCED_TESTS (french)  15(2)
'G','e','o','m','e','t','r','y',		// L_GEOMETRY (english)  8(0), 510
'G',0xE9,'o','m',0xE9,'t','r','i','e',		// L_GEOMETRY (french)  9(2)
'L','i','n','e','a','r','i','t','y',		// L_LINEARITY (english)  9(0), 511
'L','i','n',0xE9,'a','r','i','t',0xE9,		// L_LINEARITY (french)  9(2)
'Q','C',		// L_QC (english)  2(0), 512
'Q','C',		// L_QC (french)  2(0)
'I','n','v','e','n','t','o','r','y',		// L_INVENTORY (english)  9(0), 513
'I','n','v','e','n','t','a','i','r','e',		// L_INVENTORY (french)  10(0)
'I','n','v','e','n','t','o','r','y',' ','L','i','s','t',		// L_INVENTORY_LIST (english)  14(0), 514
'L','i','s','t','e',' ','d',0x27,'i','n','v','e','n','t','a','i','r','e',		// L_INVENTORY_LIST (french)  18(0)
'S','t','u','d','y',		// L_STUDY (english)  5(0), 515
0xC9,'t','u','d','e',		// L_STUDY (french)  5(1)
'L','o','t',		// L_LOT (english)  3(0), 516
'L','o','t',		// L_LOT (french)  3(0)
'D','a','t','e',		// L_DATE (english)  4(0), 517
'D','a','t','e',		// L_DATE (french)  4(0)
'I','D',		// L_ID (english)  2(0), 518
'I','D',		// L_ID (french)  2(0)
'C','o','n','c',		// L_CONC (english)  4(0), 519
'C','o','n','c',		// L_CONC (french)  4(0)
'A','d','d',' ','I','t','e','m',		// L_ADD_ITEM (english)  8(0), 520
'A','j','o','u','t','e','r',		// L_ADD_ITEM (french)  7(0)
'D','e','l','e','t','e',' ','A','l','l',		// L_DELETE_ALL (english)  10(0), 521
'T','o','u','t',' ','s','u','p','p','r','i','m','e','r',		// L_DELETE_ALL (french)  14(0)
'W','i','t','h','d','r','a','w',		// L_WITHDRAW (english)  8(0), 522
'P','r',0xE9,'l','e','v','e','r',		// L_WITHDRAW (french)  8(1)
'M','a','k','e',' ','K','i','t',		// L_MAKE_KIT (english)  8(0), 523
'K','i','t',		// L_MAKE_KIT (french)  3(0)
'D','e','l','e','t','e',' ','I','t','e','m',		// L_DELETE_ITEM (english)  11(0), 524
'S','u','p','p','r','i','m','e','r',' ',0xE9,'l',0xE9,'m','e','n','t',		// L_DELETE_ITEM (french)  17(2)
'I','N','V','E','N','T','O','R','Y',		// L_PR_INVENTORY (english)  9(0), 525
'I','N','V','E','N','T','A','I','R','E',		// L_PR_INVENTORY (french)  10(0)
'L','o','t',' ','#',':',		// L_PR_LOT (english)  6(0), 526
'L','o','t',' ','#',' ',':',		// L_PR_LOT (french)  7(0)
'O','V','E','R',		// L_PR_OVER (english)  4(0), 527
'D',0xC9,'P','A','S','S',0xC9,		// L_PR_OVER (french)  7(2)
'A','d','d',' ','I','n','v','e','n','t','o','r','y',' ','I','t','e','m',		// L_ADD_INVENTORY_ITEM (english)  18(0), 528
'A','j','o','u','t','e','r',' ',0xE9,'l',0xE9,'m','e','n','t',' ',0xE0,' ','l',0x27,'i','n','v','e','n','t','a','i','r','e',		// L_ADD_INVENTORY_ITEM (french)  30(3)
'N','u','c','l','i','d','e',':',		// L_NUCLIDE_COLON (english)  8(0), 529
'N','u','c','l',0xE9,'i','d','e',' ',':',		// L_NUCLIDE_COLON (french)  10(1)
'N','U','C','L','I','D','E',':',		// L_NUCLIDE_COLON2 (english)  8(0), 530
'N','U','C','L',0xC9,'I','D','E',' ',':',		// L_NUCLIDE_COLON2 (french)  10(1)
'L','o','t',':',		// L_LOT_COLON (english)  4(0), 531
'L','o','t',' ',':',		// L_LOT_COLON (french)  5(0)
'A','c','t','i','v','i','t','y',':',		// L_ACTIVITY_COLON (english)  9(0), 532
'A','c','t','i','v','i','t',0xE9,' ',':',		// L_ACTIVITY_COLON (french)  10(1)
'D','a','t','e',':',		// L_DATE_COLON (english)  5(0), 533
'D','a','t','e',' ',':',		// L_DATE_COLON (french)  6(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','I','D',':',		// L_PLEASE_ENTER_ID (english)  16(0), 534
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l',0x27,'I','D',' ',':',		// L_PLEASE_ENTER_ID (french)  22(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','M','e','a','s','u','r','m','e','n','t',' ','T','i','m','e',':',		// L_PLEASE_ENTER_MEASUREMENT_TIME (english)  29(0), 535
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l',0x27,'h','e','u','r','e',' ','d','e',' ','m','e','s','u','r','e',' ',':',		// L_PLEASE_ENTER_MEASUREMENT_TIME (french)  35(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','L','o','t',':',		// L_PLEASE_ENTER_LOT (english)  17(0), 536
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','l','o','t',' ',':',		// L_PLEASE_ENTER_LOT (french)  24(0)
'P','l','e','a','s','e',' ','A','c','c','e','p','t',' ','A','c','t','i','v','i','t','y',':',		// L_PLEASE_ACCEPT_ACTIVITY (english)  23(0), 537
'V','e','u','i','l','l','e','z',' ','a','c','c','e','p','t','e','r',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,' ',':',		// L_PLEASE_ACCEPT_ACTIVITY (french)  30(1)
'S','u','b','m','i','t',		// L_SUBMIT (english)  6(0), 538
'S','o','u','m','e','t','t','r','e',		// L_SUBMIT (french)  9(0)
'N','o','n','e',		// L_NONE (english)  4(0), 539
'A','u','c','u','n',		// L_NONE (french)  5(0)
'B','o','n','e',		// L_BONE (english)  4(0), 540
'O','s',		// L_BONE (french)  2(0)
'L','u','n','g',		// L_LUNG (english)  4(0), 541
'P','o','u','m','o','n',		// L_LUNG (french)  6(0)
'H','i','d','a',		// L_HIDA (english)  4(0), 542
'H',0xE9,'p','a','t','o','b','i','l','i','a','i','r','e',		// L_HIDA (french)  14(1)
'H','e','a','r','t',		// L_HEART (english)  5(0), 543
'C','o','e','u','r',		// L_HEART (french)  5(0)
'R','e','n','a','l',		// L_RENAL (english)  5(0), 544
'R',0xE9,'n','a','l',		// L_RENAL (french)  5(1)
'L','i','v','e','r',		// L_LIVER (english)  5(0), 545
'F','o','i','e',		// L_LIVER (french)  4(0)
'B','r','a','i','n',		// L_BRAIN (english)  5(0), 546
'C','e','r','v','e','a','u',		// L_BRAIN (french)  7(0)
'L','y','m','p','h',		// L_LYMPH (english)  5(0), 547
'L','y','m','p','h','e',		// L_LYMPH (french)  6(0)
'S','T','U','D','Y',':',		// L_STUDY_LABEL (english)  6(0), 548
0xC9,'T','U','D','E',' ',':',		// L_STUDY_LABEL (french)  7(1)
'M','e','a','s','u','r','e',		// L_MEASURE (english)  7(0), 549
'M','e','s','u','r','e','r',		// L_MEASURE (french)  7(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','M','o','/','T','c',' ','R','a','t','i','o',':',		// L_PLEASE_ENTER_MOTC_RATIO (english)  25(0), 550
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','r','a','p','p','o','r','t',' ','M','o','/','T','c',' ',':',		// L_PLEASE_ENTER_MOTC_RATIO (french)  34(0)
'P','l','e','a','s','e',' ','s','e','l','e','c','t',' ','s','t','u','d','y',		// L_PLEASE_SELECT_STUDY (english)  19(0), 551
'V','e','u','i','l','l','e','z',' ','s',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','l',0x27,0xE9,'t','u','d','e',		// L_PLEASE_SELECT_STUDY (french)  29(2)
'1',')',' ','C','o','n','t','a','i','n','e','r',		// L_CONTAINER (english)  12(0), 552
'1',')',' ','R',0xE9,'c','i','p','i','e','n','t',		// L_CONTAINER (french)  12(1)
'2',')',' ','I','n','i','t','i','a','l',' ','V','o','l','u','m','e',		// L_INITIAL_VOLUME (english)  17(0), 553
'2',')',' ','V','o','l','u','m','e',' ','i','n','i','t','i','a','l',		// L_INITIAL_VOLUME (french)  17(0)
'I','n','i','t','i','a','l',' ','M','e','a','s','u','r','e','m','e','n','t',		// L_INITIAL_MEASUREMENT (english)  19(0), 554
'M','e','s','u','r','e',' ','i','n','i','t','i','a','l','e',		// L_INITIAL_MEASUREMENT (french)  15(0)
'4',')',' ','A','d','d','e','d',' ','V','o','l','u','m','e',		// L_ADDED_VOLUME (english)  15(0), 555
'4',')',' ','V','o','l','u','m','e',' ','a','j','o','u','t',0xE9,		// L_ADDED_VOLUME (french)  16(1)
'4',')',' ','V','o','l','u','m','e',		// L_VOLUME2 (english)  9(0), 556
'4',')',' ','V','o','l','u','m','e',		// L_VOLUME2 (french)  9(0)
'M','e','a','s','u','r','e','m','e','n','t',		// L_MEASUREMENT (english)  11(0), 557
'M','e','s','u','r','e',		// L_MEASUREMENT (french)  6(0)
'C','A','L',' ','#',' ','N','O','T',' ','E','N','T','E','R','E','D',' ','F','O','R',' ','T','c','9','9','m',		// L_CAL_NOT_ENTERED_FOR_NUCLIDE (english)  27(0), 558
'C','A','L',' ','#',' ','N','O','N',' ','S','A','I','S','I',' ','P','O','U','R',' ','T','c','9','9','m',		// L_CAL_NOT_ENTERED_FOR_NUCLIDE (french)  26(0)
'M','o','r','e',' ','M','e','a','s','u','r','e','m','e','n','t','s',		// L_MORE_MEASUREMENTS (english)  17(0), 559
'P','l','u','s',' ','d','e',' ','m','e','s','u','r','e','s',		// L_MORE_MEASUREMENTS (french)  15(0)
'S','y','r','i','n','g','e',		// L_SYRINGE (english)  7(0), 560
'S','e','r','i','n','g','u','e',		// L_SYRINGE (french)  8(0)
'V','i','a','l',		// L_VIAL (english)  4(0), 561
'F','l','a','c','o','n',		// L_VIAL (french)  6(0)
'V','a','l','i','d',' ','r','a','n','g','e',' ','i','s',' ','9','8','.','9','m','l',' ','t','o',' ','0','.','1','m','l',		// L_VALID_RANGE_FOR_GEOMETRY (english)  30(0), 562
'P','l','a','g','e',' ','v','a','l','i','d','e',' ','d','e',' ','9','8',',','9',' ','m','l',' ',0xE0,' ','0',',','1',' ','m','l',		// L_VALID_RANGE_FOR_GEOMETRY (french)  32(1)
'V','a','l','i','d',' ','r','a','n','g','e',' ','i','s',' ','%','.','3','f','m','l',' ','t','o',' ','0','.','1','m','l',		// L_VALID_RANGE_IS_ML_TO_ML (english)  30(0), 563
'P','l','a','g','e',' ','v','a','l','i','d','e',' ','d','e',' ','%','.','3','f','m','l',' ',0xE0,' ','0','.','1','m','l',		// L_VALID_RANGE_IS_ML_TO_ML (french)  30(1)
'A','d','d','e','d',' ','V','o','l',':',' ','%','.','3','f','m','l','\n','T','o','t','a','l',' ','V','o','l',':',' ','%','.','3','f','m','l',		// L_ADDED_VOL_TOTAL_VOL (english)  35(0), 564
'V','o','l','.',' ','a','j','o','u','t',0xE9,' ',':',' ','%','.','3','f','m','l','\n','V','o','l','.',' ','t','o','t','a','l',' ',':',' ','%','.','3','f','m','l',		// L_ADDED_VOL_TOTAL_VOL (french)  40(1)
'B','A','S','E',		// L_BASE (english)  4(0), 565
'B','A','S','E',		// L_BASE (french)  4(0)
'S','y','r','i','n','g','e',' ','A','s','s','a','y',		// L_SYRINGE_ASSAY (english)  13(0), 566
'E','s','s','a','i',' ','s','e','r','i','n','g','u','e',		// L_SYRINGE_ASSAY (french)  14(0)
'V','i','a','l',' ','A','s','s','a','y',		// L_VIAL_ASSAY (english)  10(0), 567
'E','s','s','a','i',' ','f','l','a','c','o','n',		// L_VIAL_ASSAY (french)  12(0)
'G','e','o','m','e','t','r','y',' ','R','e','p','o','r','t',		// L_GEOMETRY_REPORT (english)  15(0), 568
'R','a','p','p','o','r','t',' ','d','e',' ','g',0xE9,'o','m',0xE9,'t','r','i','e',		// L_GEOMETRY_REPORT (french)  20(2)
'U','s','i','n','g',' ','V','o','l','u','m','e',		// L_USING_VOLUME (english)  12(0), 569
'V','o','l','u','m','e',' ','u','t','i','l','i','s',0xE9,		// L_USING_VOLUME (french)  14(1)
'%',' ','V','a','r',		// L_PERCENT_VAR (english)  5(0), 570
'%',' ','V','a','r',		// L_PERCENT_VAR (french)  5(0)
'%',' ','V','a','r',':',		// L_PERCENT_VAR2 (english)  6(0), 571
'%',' ','V','a','r',' ',':',		// L_PERCENT_VAR2 (french)  7(0)
'G','E','O','M','E','T','R','Y',' ','T','E','S','T',		// L_GEOMETRY_TEST (english)  13(0), 572
'T','E','S','T',' ','D','E',' ','G',0xC9,'O','M',0xC9,'T','R','I','E',		// L_GEOMETRY_TEST (french)  17(2)
'U','s','i','n','g',':',		// L_USING (english)  6(0), 573
'U','t','i','l','i','s',0xE9,' ',':',		// L_USING (french)  9(1)
'#',' ',' ',' ','V','o','l','u','m','e',' ',' ',' ',' ','A','s','s','a','y',' ',' ',' ',' ','V','a','r','i','a','t','i','o','n',		// L_VOLUME_ASSAY_VARIATION (english)  32(0), 574
'#',' ',' ',' ','V','o','l','u','m','e',' ',' ',' ',' ','E','s','s','a','i',' ',' ',' ',' ','V','a','r','i','a','t','i','o','n',		// L_VOLUME_ASSAY_VARIATION (french)  32(0)
'S','t','a','n','d','a','r','d',',',' ','L','i','n','e','a','t','o','r',' ','a','n','d',' ','C','a','l','i','c','h','e','c','k',' ','a','r','e',' ','a','v','a','i','l','a','b','l','e',' ','i','n',' ','s','e','t','u','p',		// L_STD_LINEATOR_CALICHECK_AVAILABLE_IN_SETUP (english)  55(0), 575
'S','t','a','n','d','a','r','d',',',' ','L','i','n','e','a','t','o','r',' ','e','t',' ','C','a','l','i','c','h','e','c','k',' ','d','i','s','p','o','n','i','b','l','e','s',' ','d','a','n','s',' ','c','o','n','f','i','g','.',		// L_STD_LINEATOR_CALICHECK_AVAILABLE_IN_SETUP (french)  56(0)
'A','u','t','o','L','i','n','e','a','r','i','t','y',		// L_AUTOLINEARITY (english)  13(0), 576
'A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,		// L_AUTOLINEARITY (french)  13(2)
'S','t','a','n','d','a','r','d',		// L_STANDARD (english)  8(0), 577
'S','t','a','n','d','a','r','d',		// L_STANDARD (french)  8(0)
'L','i','n','e','a','t','o','r',		// L_LINEATOR (english)  8(0), 578
'L','i','n','e','a','t','o','r',		// L_LINEATOR (french)  8(0)
'C','a','l','i','c','h','e','c','k',		// L_CALICHECK (english)  9(0), 579
'C','a','l','i','c','h','e','c','k',		// L_CALICHECK (french)  9(0)
'S','e','l','e','c','t',' ','C','h','a','m','b','e','r',		// L_SELECT_CHAMBER (english)  14(0), 580
'S',0xE9,'l','e','c','t','i','o','n','n','e','r',' ','c','h','a','m','b','r','e',		// L_SELECT_CHAMBER (french)  20(1)
'D','e','l','e','t','e',' ','A','l','l',' ','I','n','v','e','n','t','o','r','y',		// L_DELETE_ALL_INVENTORY (english)  20(0), 581
'S','u','p','p','r','i','m','e','r',' ','t','o','u','t',' ','l',0x27,'i','n','v','e','n','t','a','i','r','e',		// L_DELETE_ALL_INVENTORY (french)  27(0)
'D','e','l','e','t','e',' ','A','l','l',' ','I','n','v','e','n','t','o','r','y','?',		// L_DELETE_ALL_INVENTORY_Q (english)  21(0), 582
'S','u','p','p','r','i','m','e','r',' ','t','o','u','t',' ','l',0x27,'i','n','v','e','n','t','a','i','r','e',' ','?',		// L_DELETE_ALL_INVENTORY_Q (french)  29(0)
'T','h','e',' ','P','E','T',' ','C','h','a','m','b','e','r',' ','c','a','n',' ','n','o','t',' ','a','c','c','e','s','s',' ','i','n','v','e','n','t','o','r','y',' ','f','u','n','c','t','i','o','n','s',		// L_PET_CHAMBER_NO_INVENTORY (english)  50(0), 583
'L','a',' ','c','h','a','m','b','r','e',' ','P','E','T',' ','n','e',' ','p','e','u','t',' ','p','a','s',' ','a','c','c',0xE9,'d','e','r',' ','a','u','x',' ','f','o','n','c','t','i','o','n','s',' ','d',0x27,'i','n','v','e','n','t','a','i','r','e',		// L_PET_CHAMBER_NO_INVENTORY (french)  61(1)
'D','e','l','e','t','e',' ','I','n','v','e','n','t','o','r','y',		// L_DELETE_INVENTORY (english)  16(0), 584
'S','u','p','p','r','i','m','e','r',' ','i','n','v','e','n','t','a','i','r','e',		// L_DELETE_INVENTORY (french)  20(0)
'T','c',' ','L','e','v','e','l',' ','T','O','O',' ','L','O','W',		// L_TC_LEVEL_TOO_LOW (english)  16(0), 585
'N','i','v','e','a','u',' ','T','c',' ','T','R','O','P',' ','B','A','S',		// L_TC_LEVEL_TOO_LOW (french)  18(0)
'W','i','t','h','d','r','a','w',' ','I','n','v','e','n','t','o','r','y',		// L_WITHDRAW_INVENTORY (english)  18(0), 586
'P','r',0xE9,'l','e','v','e','r',' ','d','e',' ','l',0x27,'i','n','v','e','n','t','a','i','r','e',		// L_WITHDRAW_INVENTORY (french)  24(1)
'W','i','t','h','d','r','a','w',' ','A','c','t','i','v','i','t','y',':',		// L_WITHDRAW_ACTIVITY (english)  18(0), 587
'P','r',0xE9,'l','e','v','e','r',' ','a','c','t','i','v','i','t',0xE9,' ',':',		// L_WITHDRAW_ACTIVITY (french)  19(2)
'P','l','e','a','s','e',' ','w','i','t','h','d','r','a','w',':',' ','%','5','.','1','f',' ','m','l',		// L_PLEASE_WITHDRAW_ML (english)  25(0), 588
'V','e','u','i','l','l','e','z',' ','p','r',0xE9,'l','e','v','e','r',' ',':',' ','%','5','.','1','f',' ','m','l',		// L_PLEASE_WITHDRAW_ML (french)  28(1)
'a','n','d',' ','M','e','a','s','u','r','e',' ','A','c','t','i','v','i','t','y','.',		// L_AND_MEASURE_ACTIVITY (english)  21(0), 589
'e','t',' ','m','e','s','u','r','e','r',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,'.',		// L_AND_MEASURE_ACTIVITY (french)  22(1)
'A','C','T','I','V','I','T','Y',' ','G','R','E','A','T','E','R',' ','T','H','A','N',' ','T','H','A','T',' ','I','N',' ','I','N','V','E','N','T','O','R','Y',		// L_ACTIVITY_GREATER_THAN_INVENTORY (english)  39(0), 590
'A','C','T','I','V','I','T',0xC9,' ','S','U','P',0xC9,'R','I','E','U','R','E',' ',0xC0,' ','C','E','L','L','E',' ','D','E',' ','L',0x27,'I','N','V','E','N','T','A','I','R','E',		// L_ACTIVITY_GREATER_THAN_INVENTORY (french)  43(3)
'P','l','e','a','s','e',' ','M','e','a','s','u','r','e',' ','W','i','t','h','d','r','a','w','a','l',':',		// L_PLEASE_MEASURE_WITHDRAWAL (english)  26(0), 591
'V','e','u','i','l','l','e','z',' ','m','e','s','u','r','e','r',' ','l','e',' ','p','r',0xE9,'l',0xE8,'v','e','m','e','n','t',' ',':',		// L_PLEASE_MEASURE_WITHDRAWAL (french)  33(2)
'T','i','m','e',' ','o','f',' ','U','s','e',':',		// L_TIME_OF_USE (english)  12(0), 592
'H','e','u','r','e',' ','d',0x27,'u','t','i','l','i','s','a','t','i','o','n',' ',':',		// L_TIME_OF_USE (french)  21(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','T','i','m','e',' ','O','f',' ','U','s','e',':',		// L_PLEASE_ENTER_TIME_OF_USE (english)  25(0), 593
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l',0x27,'h','e','u','r','e',' ','d',0x27,'u','t','i','l','i','s','a','t','i','o','n',' ',':',		// L_PLEASE_ENTER_TIME_OF_USE (french)  39(0)
'S','t','a','r','t',' ','T','e','s','t',		// L_START_TEST (english)  10(0), 594
'D',0xE9,'m','a','r','r','e','r',' ','t','e','s','t',		// L_START_TEST (french)  13(1)
'R','e','s','u','m','e',' ','T','e','s','t',		// L_RESUME_TEST (english)  11(0), 595
'R','e','p','r','e','n','d','r','e',' ','t','e','s','t',		// L_RESUME_TEST (french)  14(0)
'R','e','p','o','r','t','s',		// L_REPORTS (english)  7(0), 596
'R','a','p','p','o','r','t','s',		// L_REPORTS (french)  8(0)
'E','n','t','e','r',' ','T','o','t','a','l',' ','T','i','m','e',' ','i','n',' ','h','o','u','r','s',':',		// L_ENTER_TOTAL_TIME_IN_HOURS (english)  26(0), 597
'E','n','t','r','e','z',' ','l','e',' ','t','e','m','p','s',' ','t','o','t','a','l',' ','e','n',' ','h','e','u','r','e','s',' ',':',		// L_ENTER_TOTAL_TIME_IN_HOURS (french)  33(0)
'I','n','a','c','t','i','v','e',':',		// L_INACTIVE (english)  9(0), 598
'I','n','a','c','t','i','f',' ',':',		// L_INACTIVE (french)  9(0)
'S','t','a','r','t',		// L_START (english)  5(0), 599
'D',0xE9,'m','a','r','r','e','r',		// L_START (french)  8(1)
'D','a','t','e',' ','T','i','m','e',' ','/',' ','E','l','a','p','s','e','d',		// L_DATE_TIME_ELAPSED (english)  19(0), 600
'D','a','t','e',' ','H','e','u','r','e',' ','/',' ',0xC9,'c','o','u','l',0xE9,		// L_DATE_TIME_ELAPSED (french)  19(2)
'P','r','e','d','i','c','t','e','d',		// L_PREDICTED (english)  9(0), 601
'P','r',0xE9,'v','u',		// L_PREDICTED (french)  5(1)
'P','r','e','d','i','c','t','e','d',':',		// L_PREDICTED2 (english)  10(0), 602
'P','r',0xE9,'v','u',' ',':',		// L_PREDICTED2 (french)  7(1)
'A','b','o','r','t',		// L_ABORT (english)  5(0), 603
'A','b','a','n','d','o','n','n','e','r',		// L_ABORT (french)  10(0)
'I','n','a','c','t','i','v','a','t','e',		// L_INACTIVATE (english)  10(0), 604
'D',0xE9,'s','a','c','t','i','v','e','r',		// L_INACTIVATE (french)  10(1)
'S','a','v','e',		// L_SAVE (english)  4(0), 605
'E','n','r','e','g','i','s','t','r','e','r',		// L_SAVE (french)  11(0)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',' ','-',' ','C','h',':','%','d',',',' ','%','s',		// L_AUTOLINEARITY_TEST (english)  30(0), 606
'T','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,' ','-',' ','C','h',' ',':','%','d',',',' ','%','s',		// L_AUTOLINEARITY_TEST (french)  31(2)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t',		// L_AUTOLINEARITY_TEST2 (english)  18(0), 607
'T','e','s','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,		// L_AUTOLINEARITY_TEST2 (french)  18(2)
'%','d',' ','h','r','s',		// L_HRS (english)  6(0), 608
'%','d',' ','h','r','s',		// L_HRS (french)  6(0)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','R','e','p','o','r','t',',',' ','C','h',':','1',' ','(','%','s',')',		// L_AUTOLINEARITY_REPORT1 (english)  31(0), 609
'R','a','p','p','o','r','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,',',' ','C','h',' ',':','1',' ','(','%','s',')',		// L_AUTOLINEARITY_REPORT1 (french)  33(2)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','R','e','p','o','r','t',',',' ','C','h',':','2',' ','(','%','s',')',		// L_AUTOLINEARITY_REPORT2 (english)  31(0), 610
'R','a','p','p','o','r','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,',',' ','C','h',' ',':','2',' ','(','%','s',')',		// L_AUTOLINEARITY_REPORT2 (french)  33(2)
'A','u','t','o','L','i','n','e','a','r','i','t','y',' ','R','e','p','o','r','t',',',' ','S','/','N',':',' ','%','s',		// L_AUTOLINEARITY_REPORT3 (english)  29(0), 611
'R','a','p','p','o','r','t',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,',',' ','S','/','N',' ',':',' ','%','s',		// L_AUTOLINEARITY_REPORT3 (french)  31(2)
'%','s','\n','E','l','a','p','s','e','d',':',' ','%','d',' ','m','i','n',		// L_ELAPSED (english)  18(0), 612
'%','s','\n',0xC9,'c','o','u','l',0xE9,' ',':',' ','%','d',' ','m','i','n',		// L_ELAPSED (french)  18(2)
'P','a','u','s','e','d',		// L_PAUSED (english)  6(0), 613
'E','n',' ','p','a','u','s','e',		// L_PAUSED (french)  8(0)
'P','o','w','e','r',' ','F','a','i','l',		// L_POWER_FAIL (english)  10(0), 614
'P','a','n','n','e',' ','d','e',' ','c','o','u','r','a','n','t',		// L_POWER_FAIL (french)  16(0)
'%','d',' ','o','f',' ','%','d',		// L_OF (english)  8(0), 615
'%','d',' ','d','e',' ','%','d',		// L_OF (french)  8(0)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','C','h','a','m','b','e','r',		// L_UNABLE_TO_FIND_CHAMBER (english)  22(0), 616
'I','m','p','o','s','s','i','b','l','e',' ','d','e',' ','t','r','o','u','v','e','r',' ','l','a',' ','c','h','a','m','b','r','e',		// L_UNABLE_TO_FIND_CHAMBER (french)  32(0)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','N','u','c','l','i','d','e',		// L_UNABLE_TO_FIND_NUCLIDE (english)  22(0), 617
'I','m','p','o','s','s','i','b','l','e',' ','d','e',' ','t','r','o','u','v','e','r',' ','l','e',' ','n','u','c','l',0xE9,'i','d','e',		// L_UNABLE_TO_FIND_NUCLIDE (french)  33(1)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','N','u','c','l','i','d','e',		// L_UNABLE_TO_FIND_NUCLIDE2 (english)  22(0), 618
'I','m','p','o','s','s','i','b','l','e',' ','d','e',' ','t','r','o','u','v','e','r',' ','l','e',' ','n','u','c','l',0xE9,'i','d','e',		// L_UNABLE_TO_FIND_NUCLIDE2 (french)  33(1)
'C','u','r','r','e','n','t',' ','N','u','c','l','i','d','e',' ','i','s',' ','i','n','c','o','r','r','e','c','t',		// L_CURRENT_NUCLIDE_IS_INCORRECT (english)  28(0), 619
'L','e',' ','n','u','c','l',0xE9,'i','d','e',' ','a','c','t','u','e','l',' ','e','s','t',' ','i','n','c','o','r','r','e','c','t',		// L_CURRENT_NUCLIDE_IS_INCORRECT (french)  32(1)
'C','h','a','m','b','e','r',' ','S','/','N',' ','i','s',' ','i','n','c','o','r','r','e','c','t',		// L_CHAMBER_SN_IS_INCORRECT (english)  24(0), 620
'S','/','N',' ','d','e',' ','l','a',' ','c','h','a','m','b','r','e',' ','e','s','t',' ','i','n','c','o','r','r','e','c','t',		// L_CHAMBER_SN_IS_INCORRECT (french)  31(0)
'M','i','s','s','i','n','g',' ','N','u','c','l','i','d','e',		// L_MISSING_NUCLIDE (english)  15(0), 621
'N','u','c','l',0xE9,'i','d','e',' ','m','a','n','q','u','a','n','t',		// L_MISSING_NUCLIDE (french)  17(1)
'M','i','s','s','i','n','g',' ','I','n','t','e','r','v','a','l',		// L_MISSING_INTERVAL (english)  16(0), 622
'I','n','t','e','r','v','a','l','l','e',' ','m','a','n','q','u','a','n','t',		// L_MISSING_INTERVAL (french)  19(0)
'M','i','s','s','i','n','g',' ','T','o','t','a','l',' ','T','i','m','e',		// L_MISSING_TOTAL_TIME (english)  18(0), 623
'T','e','m','p','s',' ','t','o','t','a','l',' ','m','a','n','q','u','a','n','t',		// L_MISSING_TOTAL_TIME (french)  20(0)
'N','u','m','b','e','r',' ','o','f',' ','m','e','a','s','u','r','e','m','e','n','t','s',' ','h','a','s',' ','t','o',' ','b','e',' ','g','r','e','a','t','e','r',' ','t','h','a','n',' ','4',		// L_NUM_OF_MEAS_HAS_TO_BE_GREATER_THAN_4 (english)  47(0), 624
'L','e',' ','n','o','m','b','r','e',' ','d','e',' ','m','e','s','u','r','e','s',' ','d','o','i','t',' ',0xEA,'t','r','e',' ','s','u','p',0xE9,'r','i','e','u','r',' ',0xE0,' ','4',		// L_NUM_OF_MEAS_HAS_TO_BE_GREATER_THAN_4 (french)  44(3)
'T','y','p','e',':',		// L_TYPE (english)  5(0), 625
'T','y','p','e',' ',':',		// L_TYPE (french)  6(0)
'S','t','a','r','t','e','d',':',		// L_STARTED (english)  8(0), 626
'D',0xE9,'m','a','r','r',0xE9,' ',':',		// L_STARTED (french)  9(2)
'C','a','n',0x27,'t',' ','b','e',' ','s','a','v','e','d','.','\n','L','e','s','s',' ','t','h','a','n',' ','5',' ','m','e','a','s','u','r','e','m','e','n','t','s',		// L_CANT_BE_SAVED_LESS_THAN_5_MEASUREMENTS (english)  40(0), 627
'N','e',' ','p','e','u','t',' ','p','a','s',' ',0xEA,'t','r','e',' ','e','n','r','e','g','i','s','t','r',0xE9,'.','\n','M','o','i','n','s',' ','d','e',' ','5',' ','m','e','s','u','r','e','s',		// L_CANT_BE_SAVED_LESS_THAN_5_MEASUREMENTS (french)  47(2)
'E','l','a','p','s','e','d',' ','M','i','n','u','t','e','s',':',' ','%','d','\n','R','e','m','a','i','n','i','n','g',' ','M','i','n','u','t','e','s',':','%','d',		// L_ELAPSED_MINUTES_REMAINING_MINUTES (english)  40(0), 628
'M','i','n',' ',0xE9,'c','o','u','l',0xE9,'e','s',' ',':',' ','%','d','\n','M','i','n',' ','r','e','s','t','a','n','t','e','s',' ',':',' ','%','d',		// L_ELAPSED_MINUTES_REMAINING_MINUTES (french)  36(2)
'S','e','a','r','c','h',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','T','e','s','t','s',		// L_SEARCH_AUTOLINEARITY_TESTS (english)  26(0), 629
'R','e','c','h','e','r','c','h','e','r',' ','t','e','s','t','s',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,		// L_SEARCH_AUTOLINEARITY_TESTS (french)  30(2)
'E','n','t','e','r',' ','S','t','a','r','t',' ','D','a','t','e',':',		// L_ENTER_START_DATE (english)  17(0), 630
'E','n','t','r','e','z',' ','d','a','t','e',' ','d','e',' ','d',0xE9,'b','u','t',' ',':',		// L_ENTER_START_DATE (french)  22(1)
'E','n','t','e','r',' ','E','n','d',' ','D','a','t','e',':',		// L_ENTER_END_DATE (english)  15(0), 631
'E','n','t','r','e','z',' ','d','a','t','e',' ','d','e',' ','f','i','n',' ',':',		// L_ENTER_END_DATE (french)  20(0)
'S','e','a','r','c','h',		// L_SEARCH (english)  6(0), 632
'R','e','c','h','e','r','c','h','e','r',		// L_SEARCH (french)  10(0)
'V','i','e','w',		// L_VIEW (english)  4(0), 633
'V','o','i','r',		// L_VIEW (french)  4(0)
'S','e','a','r','c','h',' ','A','u','t','o','L','i','n','e','a','r','i','t','y',' ','E','r','r','o','r',		// L_SEARCH_AUTOLINEARITY_ERROR (english)  26(0), 634
'E','r','r','e','u','r',' ','r','e','c','h','e','r','c','h','e',' ','A','u','t','o','L','i','n',0xE9,'a','r','i','t',0xE9,		// L_SEARCH_AUTOLINEARITY_ERROR (french)  30(2)
'M','o','r','e',' ','t','h','a','n',' ','1','0','0',' ','i','t','e','m','s',' ','h','a','v','e',' ','b','e','e','n',' ','r','e','t','u','r','n','e','d','\n','P','l','e','a','s','e',' ','r','e','f','i','n','e',' ','c','r','i','t','e','r','i','a',		// L_MORE_THAN_100_ITEMS_RETURNED (english)  61(0), 635
'P','l','u','s',' ','d','e',' ','1','0','0',' ','r',0xE9,'s','u','l','t','a','t','s',' ','r','e','t','o','u','r','n',0xE9,'s','\n','V','e','u','i','l','l','e','z',' ','a','f','f','i','n','e','r',' ','l','e','s',' ','c','r','i','t',0xE8,'r','e','s',		// L_MORE_THAN_100_ITEMS_RETURNED (french)  61(3)
'0',' ','i','t','e','m','s',' ','h','a','v','e',' ','b','e','e','n',' ','r','e','t','u','r','n','e','d',		// L_0_ITEMS_RETURNED (english)  26(0), 636
'0',' ','r',0xE9,'s','u','l','t','a','t','s',' ','r','e','t','o','u','r','n',0xE9,'s',		// L_0_ITEMS_RETURNED (french)  21(2)
'M','a','k','e',' ','I','n','v','e','n','t','o','r','y',' ','K','i','t',		// L_MAKE_INVENTORY_KIT (english)  18(0), 637
'P','r',0xE9,'p','a','r','e','r',' ','k','i','t',' ','d',0x27,'i','n','v','e','n','t','a','i','r','e',		// L_MAKE_INVENTORY_KIT (french)  25(1)
'S','o','u','r','c','e',' ','N','u','c','l','i','d','e',':',		// L_SOURCE_NUCLIDE (english)  15(0), 638
'N','u','c','l',0xE9,'i','d','e',' ','s','o','u','r','c','e',' ',':',		// L_SOURCE_NUCLIDE (french)  17(1)
'S','o','u','r','c','e',' ','I','D',':',		// L_SOURCE_ID (english)  10(0), 639
'I','D',' ','s','o','u','r','c','e',' ',':',		// L_SOURCE_ID (french)  11(0)
'S','o','u','r','c','e',' ','L','o','t',':',		// L_SOURCE_LOT (english)  11(0), 640
'L','o','t',' ','s','o','u','r','c','e',' ',':',		// L_SOURCE_LOT (french)  12(0)
'S','o','u','r','c','e',' ','V','o','l','u','m','e',':',		// L_SOURCE_VOLUME (english)  14(0), 641
'V','o','l','u','m','e',' ','s','o','u','r','c','e',' ',':',		// L_SOURCE_VOLUME (french)  15(0)
'S','o','u','r','c','e',' ','A','c','t','i','v','i','t','y',':',		// L_SOURCE_ACTIVITY (english)  16(0), 642
'A','c','t','i','v','i','t',0xE9,' ','s','o','u','r','c','e',' ',':',		// L_SOURCE_ACTIVITY (french)  17(1)
'S','o','u','r','c','e',' ','D','a','t','e',':',		// L_SOURCE_DATE (english)  12(0), 643
'D','a','t','e',' ','s','o','u','r','c','e',' ',':',		// L_SOURCE_DATE (french)  13(0)
'K','i','t',' ','S','t','u','d','y',':',		// L_KIT_STUDY (english)  10(0), 644
0xC9,'t','u','d','e',' ','d','u',' ','k','i','t',' ',':',		// L_KIT_STUDY (french)  14(1)
'K','i','t',' ','I','D',':',		// L_KIT_ID (english)  7(0), 645
'I','D',' ','d','u',' ','k','i','t',' ',':',		// L_KIT_ID (french)  11(0)
'K','i','t',' ','L','o','t',':',		// L_KIT_LOT (english)  8(0), 646
'L','o','t',' ','d','u',' ','k','i','t',' ',':',		// L_KIT_LOT (french)  12(0)
'K','i','t',' ','A','c','t','i','v','i','t','y',':',		// L_KIT_ACTIVITY (english)  13(0), 647
'A','c','t','i','v','i','t',0xE9,' ','d','u',' ','k','i','t',' ',':',		// L_KIT_ACTIVITY (french)  17(1)
'K','i','t',' ','V','o','l','u','m','e',':',		// L_KIT_VOLUME (english)  11(0), 648
'V','o','l','u','m','e',' ','d','u',' ','k','i','t',' ',':',		// L_KIT_VOLUME (french)  15(0)
'K','I','T',' ','A','C','T','I','V','I','T','Y',' ','G','R','E','A','T','E','R',' ','T','H','A','N',' ','T','H','A','T',		// L_KIT_ACTIVITY_GREATER (english)  30(0), 649
'A','C','T','I','V','I','T',0xC9,' ','D','U',' ','K','I','T',' ','S','U','P',0xC9,'R','I','E','U','R','E',' ',0xC0,		// L_KIT_ACTIVITY_GREATER (french)  28(3)
'I','N',' ','S','O','U','R','C','E',		// L_IN_SOURCE (english)  9(0), 650
'C','E','L','L','E',' ','D','E',' ','L','A',' ','S','O','U','R','C','E',		// L_IN_SOURCE (french)  18(0)
'K','I','T',' ','V','O','L','U','M','E',' ','L','E','S','S',' ','T','H','A','N',' ','T','H','E',		// L_KIT_VOLUME_LESS (english)  24(0), 651
'V','O','L','U','M','E',' ','D','U',' ','K','I','T',' ','I','N','F',0xC9,'R','I','E','U','R',' ','A','U',		// L_KIT_VOLUME_LESS (french)  26(1)
'%','.','3','f',' ','m','l',' ','D','R','A','W',' ','F','R','O','M',' ','S','O','U','R','C','E',		// L_DRAW_FROM_SOURCE (english)  24(0), 652
'%','.','3','f',' ','m','l',' ','P','R',0xC9,'L','E','V','E','R',' ','D','E',' ','L','A',' ','S','O','U','R','C','E',		// L_DRAW_FROM_SOURCE (french)  29(1)
'P','l','e','a','s','e',' ','w','i','t','h','d','r','a','w',':',' ','%','5','.','1','f',' ','m','l',',','\n',' ','a','n','d',' ','M','e','a','s','u','r','e',' ','A','c','t','i','v','i','t','y','.',		// L_PLEASE_WITHDRAW_AND_MEASURE_ACTIVITY (english)  49(0), 653
'V','e','u','i','l','l','e','z',' ','p','r',0xE9,'l','e','v','e','r',' ',':',' ','%','5','.','1','f',' ','m','l',',','\n',' ','e','t',' ','m','e','s','u','r','e','r',' ','l',0x27,'a','c','t','i','v','i','t',0xE9,'.',		// L_PLEASE_WITHDRAW_AND_MEASURE_ACTIVITY (french)  53(2)
'C','A','P','M','A','C',' ','f','o','r',' ','M','a','l','l','i','n','c','k','r','o','d','t',' ','G','e','n',		// L_CAPMAC_FOR_MAL_GEN (english)  27(0), 654
'C','A','P','M','A','C',' ','p','o','u','r',' ','M','a','l','l','i','n','c','k','r','o','d','t',' ','G','e','n',		// L_CAPMAC_FOR_MAL_GEN (french)  28(0)
'C','A','P','M','A','C',' ','f','o','r',' ','B','r','i','s','t','o','l',' ','M','y','e','r','s',' ','G','e','n',		// L_CAPMAC_FOR_BM_GEN (english)  28(0), 655
'C','A','P','M','A','C',' ','p','o','u','r',' ','B','r','i','s','t','o','l',' ','M','y','e','r','s',' ','G','e','n',		// L_CAPMAC_FOR_BM_GEN (french)  29(0)
'C','a','p','i','n','t','e','c',' ','C','a','n','i','s','t','e','r',		// L_CAPINTEC_CANISTER (english)  17(0), 656
'C','a','p','i','n','t','e','c',' ','B','i','d','o','n',		// L_CAPINTEC_CANISTER (french)  14(0)
'M','o','l','y',' ','A','s','s','a','y',		// L_MOLY_ASSAY (english)  10(0), 657
'E','s','s','a','i',' ','M','o','l','y',		// L_MOLY_ASSAY (french)  10(0)
'M','o','l','y',' ','A','s','s','a','y',' ','-',' ','M','a','l','l','i','n','c','k','r','o','d','t',' ','G','e','n',' ','M','e','t','h','o','d',		// L_MOLY_ASSAY_MAL_GEN_METHOD (english)  36(0), 658
'E','s','s','a','i',' ','M','o','l','y',' ','-',' ','M',0xE9,'t','h','o','d','e',' ','M','a','l','l','i','n','c','k','r','o','d','t',' ','G','e','n',		// L_MOLY_ASSAY_MAL_GEN_METHOD (french)  37(1)
'M','o','l','y',' ','A','s','s','a','y',' ','-',' ','B','r','i','s','t','o','l',' ','M','y','e','r','s',' ','G','e','n',' ','M','e','t','h','o','d',		// L_MOLY_ASSAY_BM_GEN_METHOD (english)  37(0), 659
'E','s','s','a','i',' ','M','o','l','y',' ','-',' ','M',0xE9,'t','h','o','d','e',' ','B','r','i','s','t','o','l',' ','M','y','e','r','s',' ','G','e','n',		// L_MOLY_ASSAY_BM_GEN_METHOD (french)  38(1)
'M','o','l','y',' ','A','s','s','a','y',' ','-',' ','C','a','p','i','n','t','e','c',' ','C','a','n','i','s','t','e','r',' ','M','e','t','h','o','d',		// L_MOLY_ASSAY_CAPINTEC_CANISTER_METHOD (english)  37(0), 660
'E','s','s','a','i',' ','M','o','l','y',' ','-',' ','M',0xE9,'t','h','o','d','e',' ','C','a','p','i','n','t','e','c',' ','B','i','d','o','n',		// L_MOLY_ASSAY_CAPINTEC_CANISTER_METHOD (french)  35(1)
'M','e','a','s','u','r','e',' ','B','k','g',		// L_MEASURE_BKG (english)  11(0), 661
'M','e','s','u','r','e','r',' ','b','r','u','i','t',' ','f','o','n','d',		// L_MEASURE_BKG (french)  18(0)
'S','k','i','p',' ','B','k','g',		// L_SKIP_BKG (english)  8(0), 662
'I','g','n','o','r','e','r',' ','b','r','u','i','t',' ','f','o','n','d',		// L_SKIP_BKG (french)  18(0)
'1',')',' ','M','e','a','s','u','r','e',' ','C','a','n','i','s','t','e','r',' ','B','k','g',':',		// L_MEASURE_CANISTER_BKG (english)  24(0), 663
'1',')',' ','M','e','s','u','r','e','r',' ','b','r','u','i','t',' ','f','o','n','d',' ','b','i','d','o','n',' ',':',		// L_MEASURE_CANISTER_BKG (french)  29(0)
'1',')',' ','M','e','a','s','u','r','e',' ','C','A','P','M','A','C',' ','B','k','g',':',		// L_MEASURE_CAPMAC_BKG (english)  22(0), 664
'1',')',' ','M','e','s','u','r','e','r',' ','b','r','u','i','t',' ','f','o','n','d',' ','C','A','P','M','A','C',' ',':',		// L_MEASURE_CAPMAC_BKG (french)  30(0)
'S','k','i','p','p','e','d',		// L_SKIPPED (english)  7(0), 665
'I','g','n','o','r',0xE9,		// L_SKIPPED (french)  6(1)
'2',')',' ','E','l','u','a','t','e',' ','i','n',' ','C','A','N','I','S','T','E','R',':',' ',		// L_ELUATE_IN_CANISTER (english)  23(0), 666
'2',')',' ',0xC9,'l','u','a','t',' ','d','a','n','s',' ','B','I','D','O','N',' ',':',' ',' ',' ',		// L_ELUATE_IN_CANISTER (french)  24(1)
'2',')',' ','E','l','u','a','t','e',' ','i','n',' ','C','A','P','M','A','C',':',' ',		// L_ELUATE_IN_CAPMAC (english)  21(0), 667
'2',')',' ',0xC9,'l','u','a','t',' ','d','a','n','s',' ','C','A','P','M','A','C',' ',':',' ',		// L_ELUATE_IN_CAPMAC (french)  23(1)
'3',')',' ','M','e','a','s','u','r','e',' ','T','c','9','9','m',' ','A','s','s','a','y',':',		// L_MEASURE_TC_ASSAY (english)  23(0), 668
'3',')',' ','M','e','s','u','r','e','r',' ','e','s','s','a','i',' ','T','c','9','9','m',' ',':',		// L_MEASURE_TC_ASSAY (french)  24(0)
'A','c','t','i','v','i','t','y',' ','T','o','o',' ','L','o','w',' ','E','r','r','o','r',		// L_ACTIVITY_TOO_LOW_ERROR (english)  22(0), 669
'E','r','r','e','u','r',' ',':',' ','a','c','t','i','v','i','t',0xE9,' ','t','r','o','p',' ','f','a','i','b','l','e',		// L_ACTIVITY_TOO_LOW_ERROR (french)  29(1)
'C','A','U','T','I','O','N',		// L_CAUTION (english)  7(0), 670
'A','T','T','E','N','T','I','O','N',		// L_CAUTION (french)  9(0)
'D','O',' ','N','O','T',' ','U','S','E',' ','A','F','T','E','R',' ','%','d',' ','H','O','U','R','S',		// L_DO_NOT_USE_AFTER_HOURS (english)  25(0), 671
'N','E',' ','P','A','S',' ','U','T','I','L','I','S','E','R',' ','A','P','R',0xC8,'S',' ','%','d',' ','H','E','U','R','E','S',		// L_DO_NOT_USE_AFTER_HOURS (french)  31(1)
'M','O',' ','T','O','O',' ','H','I','G','H',		// L_MO_TOO_HIGH (english)  11(0), 672
'M','O',' ','T','R','O','P',' ',0xC9,'L','E','V',0xC9,		// L_MO_TOO_HIGH (french)  13(2)
'D','O',' ','N','O','T',' ','U','S','E',		// L_DO_NOT_USE (english)  10(0), 673
'N','E',' ','P','A','S',' ','U','T','I','L','I','S','E','R',		// L_DO_NOT_USE (french)  15(0)
'M','O',' ','H','I','G','H',' ','T','e','r','m','i','n','a','t','e','?',		// L_MO_HIGH_TERMINATE_QUESTION (english)  18(0), 674
'M','O',' ',0xC9,'L','E','V',0xC9,' ','T','e','r','m','i','n','e','r',' ','?',		// L_MO_HIGH_TERMINATE_QUESTION (french)  19(2)
'N','E','G','A','T','I','V','E',' ','A','C','T','I','V','I','T','Y',		// L_NEGATIVE_ACTIVITY (english)  17(0), 675
'A','C','T','I','V','I','T',0xC9,' ','N',0xC9,'G','A','T','I','V','E',		// L_NEGATIVE_ACTIVITY (french)  17(2)
'M','O',' ','H','I','G','H',		// L_MO_HIGH (english)  7(0), 676
'M','O',' ',0xC9,'L','E','V',0xC9,		// L_MO_HIGH (french)  8(2)
'E','x','p','i','r','e','s',		// L_EXPIRES (english)  7(0), 677
'E','x','p','i','r','e',		// L_EXPIRES (french)  6(0)
'M','o','l','y',' ','B','k','g',		// L_MOLY_BKG (english)  8(0), 678
'B','r','u','i','t',' ','f','o','n','d',' ','M','o','l','y',		// L_MOLY_BKG (french)  15(0)
'M','O',' ','T','O','O',' ','H','I','G','H',' ','-',' ','D','O',' ','N','O','T',' ','U','S','E',		// L_MO_TOO_HIGH_DO_NOT_USE (english)  24(0), 679
'M','O',' ','T','R','O','P',' ',0xC9,'L','E','V',0xC9,' ','-',' ','N','E',' ','P','A','S',' ','U','T','I','L','I','S','E','R',		// L_MO_TOO_HIGH_DO_NOT_USE (french)  31(2)
'S','i','n','g','l','e',' ','S','t','r','i','p',' ','T','e','s','t',		// L_SINGLE_STRIP_TEST (english)  17(0), 680
'T','e','s','t',' ','b','a','n','d','e',' ','u','n','i','q','u','e',		// L_SINGLE_STRIP_TEST (french)  17(0)
'T','w','o',' ','S','t','r','i','p',' ','T','e','s','t',		// L_TWO_STRIP_TEST (english)  14(0), 681
'T','e','s','t',' ','d','o','u','b','l','e',' ','b','a','n','d','e',		// L_TWO_STRIP_TEST (french)  17(0)
'H','M','P','A','O',' ','T','e','s','t',		// L_HMPAO_TEST (english)  10(0), 682
'T','e','s','t',' ','H','M','P','A','O',		// L_HMPAO_TEST (french)  10(0)
'M','A','G','3',' ','T','e','s','t',		// L_MAG3_TEST (english)  9(0), 683
'T','e','s','t',' ','M','A','G','3',		// L_MAG3_TEST (french)  9(0)
'N','o',' ','d','e','t','e','c','t','o','r','s',' ','f','o','u','n','d','\n','N','o',' ','C','h','a','m','b','e','r',' ','a','t','t','a','c','h','e','d','\n','N','o',' ','W','e','l','l',' ','a','t','t','a','c','h','e','d',		// L_NONE_FOUND (english)  55(0), 684
'A','u','c','u','n',' ','d',0xE9,'t','e','c','t','e','u','r',' ','t','r','o','u','v',0xE9,'\n','A','u','c','u','n','e',' ','c','h','a','m','b','r','e',' ','c','o','n','n','e','c','t',0xE9,'e','\n','A','u','c','u','n',' ','p','u','i','t','s',' ','c','o','n','n','e','c','t',0xE9,		// L_NONE_FOUND (french)  68(4)
'Q','C',' ','-',' ','O','n','e',' ','S','t','r','i','p',' ','T','e','s','t',		// L_QC_ONE_STRIP_TEST (english)  19(0), 685
'Q','C',' ','-',' ','T','e','s','t',' ','u','n','e',' ','b','a','n','d','e',		// L_QC_ONE_STRIP_TEST (french)  19(0)
'1',')',' ','T','o','p',' ','o','f',' ','s','t','r','i','p',		// L_TOP_OF_STRIP (english)  15(0), 686
'1',')',' ','H','a','u','t',' ','d','e',' ','b','a','n','d','e',		// L_TOP_OF_STRIP (french)  16(0)
'T','o','p',' ','o','f',' ','S','t','r','i','p',':',		// L_TOP_OF_STRIP2 (english)  13(0), 687
'H','a','u','t',' ','d','e',' ','b','a','n','d','e',' ',':',		// L_TOP_OF_STRIP2 (french)  15(0)
'2',')',' ','B','o','t','t','o','m',' ','o','f',' ','s','t','r','i','p',		// L_BOTTOM_OF_STRIP (english)  18(0), 688
'2',')',' ','B','a','s',' ','d','e',' ','b','a','n','d','e',		// L_BOTTOM_OF_STRIP (french)  15(0)
'B','o','t','t','o','m',' ','o','f',' ','S','t','r','i','p',':',		// L_BOTTOM_OF_STRIP2 (english)  16(0), 689
'B','a','s',' ','d','e',' ','b','a','n','d','e',' ',':',		// L_BOTTOM_OF_STRIP2 (french)  14(0)
'O','n','e',' ','S','t','r','i','p',		// L_ONE_STRIP (english)  9(0), 690
'U','n','e',' ','b','a','n','d','e',		// L_ONE_STRIP (french)  9(0)
'Q','C',' ','-',' ','T','w','o',' ','S','t','r','i','p',' ','T','e','s','t',		// L_QC_TWO_STRIP_TEST (english)  19(0), 691
'Q','C',' ','-',' ','T','e','s','t',' ','d','o','u','b','l','e',' ','b','a','n','d','e',		// L_QC_TWO_STRIP_TEST (french)  22(0)
'1',')',' ','T','o','p',' ','o','f',' ','s','t','r','i','p',' ','A',		// L_TOP_OF_STRIP_A (english)  17(0), 692
'1',')',' ','H','a','u','t',' ','b','a','n','d','e',' ','A',		// L_TOP_OF_STRIP_A (french)  15(0)
'2',')',' ','B','o','t','t','o','m',' ','o','f',' ','s','t','r','i','p',' ','A',		// L_BOTTOM_OF_STRIP_A (english)  20(0), 693
'2',')',' ','B','a','s',' ','b','a','n','d','e',' ','A',		// L_BOTTOM_OF_STRIP_A (french)  14(0)
'3',')',' ','T','o','p',' ','o','f',' ','s','t','r','i','p',' ','B',		// L_TOP_OF_STRIP_B (english)  17(0), 694
'3',')',' ','H','a','u','t',' ','b','a','n','d','e',' ','B',		// L_TOP_OF_STRIP_B (french)  15(0)
'4',')',' ','B','o','t','t','o','m',' ','o','f',' ','s','t','r','i','p',' ','B',		// L_BOTTOM_OF_STRIP_B (english)  20(0), 695
'4',')',' ','B','a','s',' ','b','a','n','d','e',' ','B',		// L_BOTTOM_OF_STRIP_B (french)  14(0)
'T','o','p',' ','o','f',' ','S','t','r','i','p',' ','A',':',		// L_TOP_OF_STRIP_A2 (english)  15(0), 696
'H','a','u','t',' ','d','e',' ','b','a','n','d','e',' ','A',' ',':',		// L_TOP_OF_STRIP_A2 (french)  17(0)
'B','o','t','t','o','m',' ','o','f',' ','S','t','r','i','p',' ','A',':',		// L_BOTTOM_OF_STRIP_A2 (english)  18(0), 697
'B','a','s',' ','d','e',' ','b','a','n','d','e',' ','A',' ',':',		// L_BOTTOM_OF_STRIP_A2 (french)  16(0)
'T','o','p',' ','o','f',' ','S','t','r','i','p',' ','B',':',		// L_TOP_OF_STRIP_B2 (english)  15(0), 698
'H','a','u','t',' ','d','e',' ','b','a','n','d','e',' ','B',' ',':',		// L_TOP_OF_STRIP_B2 (french)  17(0)
'B','o','t','t','o','m',' ','o','f',' ','S','t','r','i','p',' ','B',':',		// L_BOTTOM_OF_STRIP_B2 (english)  18(0), 699
'B','a','s',' ','d','e',' ','b','a','n','d','e',' ','B',' ',':',		// L_BOTTOM_OF_STRIP_B2 (french)  16(0)
'5',')',' ','T','o','p',' ','o','f',' ','s','t','r','i','p',' ','C',		// L_TOP_OF_STRIP_C (english)  17(0), 700
'5',')',' ','H','a','u','t',' ','b','a','n','d','e',' ','C',		// L_TOP_OF_STRIP_C (french)  15(0)
'6',')',' ','B','o','t','t','o','m',' ','o','f',' ','s','t','r','i','p',' ','C',		// L_BOTTOM_OF_STRIP_C (english)  20(0), 701
'6',')',' ','B','a','s',' ','b','a','n','d','e',' ','C',		// L_BOTTOM_OF_STRIP_C (french)  14(0)
'T','o','p',' ','o','f',' ','S','t','r','i','p',' ','C',':',		// L_TOP_OF_STRIP_C2 (english)  15(0), 702
'H','a','u','t',' ','d','e',' ','b','a','n','d','e',' ','C',' ',':',		// L_TOP_OF_STRIP_C2 (french)  17(0)
'B','o','t','t','o','m',' ','o','f',' ','S','t','r','i','p',' ','C',':',		// L_BOTTOM_OF_STRIP_C2 (english)  18(0), 703
'B','a','s',' ','d','e',' ','b','a','n','d','e',' ','C',' ',':',		// L_BOTTOM_OF_STRIP_C2 (french)  16(0)
'F','r','e','e',' ','T','c','9','9','m',' ',':',' ','%','5','.','1','f',		// L_FREE (english)  18(0), 704
'T','c','9','9','m',' ','l','i','b','r','e',' ',':',' ','%','5','.','1','f',		// L_FREE (french)  19(0)
'R','e','d','u','c','e','d','/','H','y','d','r','o','l','i','z','e','d',':',' ','%','5','.','1','f',		// L_REDUCED_HYDROLIZED (english)  25(0), 705
'R',0xE9,'d','u','i','t','/','H','y','d','r','o','l','y','s',0xE9,' ',':',' ','%','5','.','1','f',		// L_REDUCED_HYDROLIZED (french)  24(2)
'H','y','d','r','o','l','i','z','e','d','/','R','e','d','u','c','e','d',':',' ','%','5','.','1','f',		// L_HYDROLIZED_REDUCED (english)  25(0), 706
'H','y','d','r','o','l','y','s',0xE9,'/','R',0xE9,'d','u','i','t',' ',':',' ','%','5','.','1','f',		// L_HYDROLIZED_REDUCED (french)  24(2)
'P','u','r','i','t','y',':',' ','%','5','.','1','f',		// L_PURITY (english)  13(0), 707
'P','u','r','e','t',0xE9,' ',':',' ','%','5','.','1','f',		// L_PURITY (french)  14(1)
' ',' ',' ',' ','T','o','p','/','(','T','o','p','+','B','o','t','t','o','m',')',' ','=',' ','%','5','.','1','f',		// L_TOP_TOP_BOTTOM_EQUALS (english)  28(0), 708
' ',' ',' ',' ','H','a','u','t','/','(','H','a','u','t','+','B','a','s',')',' ','=',' ','%','5','.','1','f',		// L_TOP_TOP_BOTTOM_EQUALS (french)  27(0)
'B','o','t','t','o','m','/','(','T','o','p','+','B','o','t','t','o','m',')',' ','=',' ','%','5','.','1','f',		// L_BOTTOM_TOP_BOTTOM_EQUALS (english)  27(0), 709
'B','a','s','/','(','H','a','u','t','+','B','a','s',')',' ','=',' ','%','5','.','1','f',		// L_BOTTOM_TOP_BOTTOM_EQUALS (french)  22(0)
'Q','U','A','L','I','T','Y',' ','C','O','N','T','R','O','L',' ','T','E','S','T',		// L_QUALITY_CONTROL_TEST (english)  20(0), 710
'T','E','S','T',' ','D','E',' ','C','O','N','T','R',0xD4,'L','E',' ','Q','U','A','L','I','T',0xC9,		// L_QUALITY_CONTROL_TEST (french)  24(2)
'S','i','n','g','l','e',' ','S','t','r','i','p',' ','/',' ','S','i','n','g','l','e',' ','S','o','l','v','e','n','t',		// L_SINGLE_STRIP_SINGLE_SOLVENT (english)  29(0), 711
'B','a','n','d','e',' ','u','n','i','q','u','e',' ','/',' ','S','o','l','v','a','n','t',' ','u','n','i','q','u','e',		// L_SINGLE_STRIP_SINGLE_SOLVENT (french)  29(0)
'M','e','a','s','u','r','e','d',' ','V','a','l','u','e','s',		// L_MEASURED_VALUES (english)  15(0), 712
'V','a','l','e','u','r','s',' ','m','e','s','u','r',0xE9,'e','s',		// L_MEASURED_VALUES (french)  16(1)
'R','e','s','u','l','t','s',		// L_RESULTS (english)  7(0), 713
'R',0xE9,'s','u','l','t','a','t','s',		// L_RESULTS (french)  9(1)
'R','a','d','i','o','p','h','a','r','m','a','c','e','u','t','i','c','a','l',':',' ','_','_','_','_','_','_','_','_','_',		// L_QC_RADIOPHARMACEUTICAL (english)  30(0), 714
'R','a','d','i','o','p','h','a','r','m','a','c','e','u','t','i','q','u','e',' ',':',' ','_','_','_','_','_','_','_','_',		// L_QC_RADIOPHARMACEUTICAL (french)  30(0)
'L','o','t',' ','#',':',' ','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',		// L_QC_LOT (english)  30(0), 715
'L','o','t',' ','#',' ',':',' ','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',		// L_QC_LOT (french)  30(0)
'K','i','t',' ','#',':',' ','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',		// L_QC_KIT (english)  30(0), 716
'K','i','t',' ','#',' ',':',' ','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',		// L_QC_KIT (french)  30(0)
'T','o','p',' ',' ',' ',' ','/',' ','(','T','o','p',' ','+',' ','B','o','t','t','o','m',')',':',' ',		// L_TOP_TOP_BOTTOM (english)  25(0), 717
'H','a','u','t',' ',' ',' ','/',' ','(','H','a','u','t',' ','+',' ','B','a','s',')',' ',':',' ',		// L_TOP_TOP_BOTTOM (french)  24(0)
'B','o','t','t','o','m',' ','/',' ','(','T','o','p',' ','+',' ','B','o','t','t','o','m',')',':',' ',		// L_BOTTOM_TOP_BOTTOM (english)  25(0), 718
'B','a','s',' ',' ',' ',' ','/',' ','(','H','a','u','t',' ','+',' ','B','a','s',')',' ',':',' ',		// L_BOTTOM_TOP_BOTTOM (french)  24(0)
'T','w','o',' ','S','t','r','i','p',		// L_TWO_STRIP (english)  9(0), 719
'D','o','u','b','l','e',' ','b','a','n','d','e',		// L_TWO_STRIP (french)  12(0)
'T','w','o',' ','S','t','r','i','p','s',' ','/',' ','T','w','o',' ','S','o','l','v','e','n','t','s',		// L_TWO_STRIPS_TWO_SOLVENTS (english)  25(0), 720
'D','o','u','b','l','e',' ','b','a','n','d','e',' ','/',' ','D','e','u','x',' ','s','o','l','v','a','n','t','s',		// L_TWO_STRIPS_TWO_SOLVENTS (french)  28(0)
'S','t','r','i','p',' ','A',':',' ','F','r','e','e',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','=',' ',		// L_STRIP_A_FREE (english)  25(0), 721
'B','a','n','d','e',' ','A',' ',':',' ','L','i','b','r','e',' ',' ',' ',' ',' ',' ',' ',' ','=',' ',		// L_STRIP_A_FREE (french)  25(0)
'S','t','r','i','p',' ','B',':',' ','R',' ','/',' ','H',' ',' ',' ',' ',' ',' ',' ',' ',' ','=',' ',		// L_STRIP_B_R_H (english)  25(0), 722
'B','a','n','d','e',' ','B',' ',':',' ','R',' ','/',' ','H',' ',' ',' ',' ',' ',' ',' ',' ','=',' ',		// L_STRIP_B_R_H (french)  25(0)
'P','u','r','i','t','y',':',' ','1','0','0',' ','-','(','F',' ','+',' ','R','/','H',')',' ','=',' ',		// L_PURITY_F_R_H (english)  25(0), 723
'P','u','r','e','t',0xE9,' ',':',' ','1','0','0',' ','-','(','L',' ','+',' ','R','/','H',')',' ','=',' ',		// L_PURITY_F_R_H (french)  26(1)
'F',' ','=',' ','F','r','e','e',		// L_F_FREE (english)  8(0), 724
'L',' ','=',' ','L','i','b','r','e',		// L_F_FREE (french)  9(0)
'R',' ','=',' ','R','e','d','u','c','e','d',		// L_R_REDUCED (english)  11(0), 725
'R',' ','=',' ','R',0xE9,'d','u','i','t',		// L_R_REDUCED (french)  10(1)
'H',' ','=',' ','H','y','d','r','o','l','i','z','e','d',		// L_H_HYDROLIZED (english)  14(0), 726
'H',' ','=',' ','H','y','d','r','o','l','y','s',0xE9,		// L_H_HYDROLIZED (french)  13(1)
'F','r','e','e',' ','T','c','9','9','m',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_FREE_TC99M (english)  25(0), 727
'T','c','9','9','m',' ','l','i','b','r','e',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_FREE_TC99M (french)  25(0)
'H','y','d','r','o','l','i','z','e','d','/','R','e','d','u','c','e','d',' ','T','c','9','9','m',':',		// L_HYDROLIZED_REDUCED_TC99M (english)  25(0), 728
'H','y','d','r','o','l','y','s',0xE9,'/','R',0xE9,'d','u','i','t',' ','T','c','9','9','m',' ',' ',':',		// L_HYDROLIZED_REDUCED_TC99M (french)  25(2)
'T','c','9','9','m',' ','H','M','P','A','O',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_TC99M_HMPAO (english)  25(0), 729
'T','c','9','9','m',' ','H','M','P','A','O',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_TC99M_HMPAO (french)  25(0)
'1',')',' ','F','r','a','c','t','i','o','n',' ','#','1',		// L_FRACTION_1 (english)  14(0), 730
'1',')',' ','F','r','a','c','t','i','o','n',' ','#','1',		// L_FRACTION_1 (french)  14(0)
'2',')',' ','F','r','a','c','t','i','o','n',' ','#','2',		// L_FRACTION_2 (english)  14(0), 731
'2',')',' ','F','r','a','c','t','i','o','n',' ','#','2',		// L_FRACTION_2 (french)  14(0)
'F','r','a','c','t','i','o','n',' ','#','1',' ',' ',' ',' ',' ',' ',':',' ',		// L_FRACTION_12 (english)  19(0), 732
'F','r','a','c','t','i','o','n',' ','#','1',' ',' ',' ',' ',' ',' ',':',' ',		// L_FRACTION_12 (french)  19(0)
'F','r','a','c','t','i','o','n',' ','#','2',' ',' ',' ',' ',' ',' ',':',' ',		// L_FRACTION_22 (english)  19(0), 733
'F','r','a','c','t','i','o','n',' ','#','2',' ',' ',' ',' ',' ',' ',':',' ',		// L_FRACTION_22 (french)  19(0)
'3',')',' ','C','a','r','t','r','i','d','g','e',		// L_CARTRIDGE (english)  12(0), 734
'3',')',' ','C','a','r','t','o','u','c','h','e',		// L_CARTRIDGE (french)  12(0)
'C','a','r','t','r','i','d','g','e',' ',' ',' ',' ',' ',' ',' ',' ',':',' ',		// L_CARTRIDGE2 (english)  19(0), 735
'C','a','r','t','o','u','c','h','e',' ',' ',' ',' ',' ',' ',' ',' ',':',' ',		// L_CARTRIDGE2 (french)  19(0)
'N','o','n','-','e','l','u','t','a','b','l','e',' ','T','c','9','9','m',':',' ','%','5','.','1','f',		// L_NON_ELUTABLE (english)  25(0), 736
'T','c','9','9','m',' ','n','o','n',' ',0xE9,'l','u','a','b','l','e',' ',':',' ','%','5','.','1','f',		// L_NON_ELUTABLE (french)  25(1)
'N','o','n','-','e','l','u','t','a','b','l','e',' ','T','c','9','9','m',' ',' ',' ',' ',' ',' ',' ',':',		// L_NON_ELUTABLE2 (english)  26(0), 737
'T','c','9','9','m',' ','n','o','n',' ',0xE9,'l','u','a','b','l','e',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_NON_ELUTABLE2 (french)  26(1)
'H','y','d','r','o','p','h','i','l','i','c',' ','I','m','p','u','r','e',':',' ','%','5','.','1','f',		// L_HYDROPHILIC_IMPURE (english)  25(0), 738
'I','m','p','u','r','e','t',0xE9,' ','h','y','d','r','o','p','h','i','l','e',' ',':',' ','%','5','.','1','f',		// L_HYDROPHILIC_IMPURE (french)  27(1)
'H','y','d','r','o','p','h','i','l','i','c',' ','T','c',' ','I','m','p','u','r','i','t','i','e','s',':',		// L_HYDROPHILIC_TC_IMPURITIES (english)  26(0), 739
'I','m','p','u','r','e','t',0xE9,'s',' ','T','c',' ','h','y','d','r','o','p','h','i','l','e','s',' ',':',		// L_HYDROPHILIC_TC_IMPURITIES (french)  26(1)
'T','c','9','9','m',' ','M','e','r','t','i','a','t','i','d','e',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_TC_MERTIATIDE (english)  26(0), 740
'T','c','9','9','m',' ','M','e','r','t','i','a','t','i','d','e',' ',' ',' ',' ',' ',' ',' ',' ',' ',':',		// L_TC_MERTIATIDE (french)  26(0)
'C','o','m','p','l','e','t','e',' ','T','h','y','r','o','i','d',' ','U','p','t','a','k','e',' ','T','e','s','t',		// L_COMPLETE_THYROID_UPTAKE_TEST_ENGLISH (english)  28(0), 741
'C','o','m','p','l','e','t','e',' ','T','h','y','r','o','i','d',' ','U','p','t','a','k','e',' ','T','e','s','t',		// L_COMPLETE_THYROID_UPTAKE_TEST_ENGLISH (french)  28(0)
'M','a','r','k',' ','T','e','s','t',' ','C','o','m','p','l','e','t','e','d','?',		// L_MARK_TEST_COMPLETED_ENGLISH (english)  20(0), 742
'M','a','r','k',' ','T','e','s','t',' ','C','o','m','p','l','e','t','e','d','?',		// L_MARK_TEST_COMPLETED_ENGLISH (french)  20(0)
'C','o','m','p','l','e','t','e',' ','R','B','C',' ','S','u','r','v','i','v','a','l',' ','T','e','s','t',		// L_COMPLETE_RBC_SURVIVAL_TEST_ENGLISH (english)  26(0), 743
'C','o','m','p','l','e','t','e',' ','R','B','C',' ','S','u','r','v','i','v','a','l',' ','T','e','s','t',		// L_COMPLETE_RBC_SURVIVAL_TEST_ENGLISH (french)  26(0)
'I','n','a','c','t','i','v','a','t','e',' ','R','e','c','o','r','d',		// L_INACTIVATE_RECORD (english)  17(0), 744
'D',0xE9,'s','a','c','t','i','v','e','r',' ','e','n','r','e','g','i','s','t','r','e','m','e','n','t',		// L_INACTIVATE_RECORD (french)  25(1)
'I','n','a','c','t','i','v','a','t','e',' ','r','e','c','o','r','d','?',		// L_INACTIVATE_RECORD2 (english)  18(0), 745
'D',0xE9,'s','a','c','t','i','v','e','r',' ','l',0x27,'e','n','r','e','g','i','s','t','r','e','m','e','n','t',' ','?',		// L_INACTIVATE_RECORD2 (french)  29(1)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','C','o','m','m','e','n','t',':',		// L_PLEASE_ENTER_COMMENT (english)  21(0), 746
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','c','o','m','m','e','n','t','a','i','r','e',' ',':',		// L_PLEASE_ENTER_COMMENT (french)  32(0)
'C','l','e','a','r',' ','A','l','l',' ','M','e','a','s','u','r','e','m','e','n','t','s',		// L_CLEAR_ALL_MEASUREMENTS (english)  22(0), 747
'E','f','f','a','c','e','r',' ','t','o','u','t','e','s',' ','l','e','s',' ','m','e','s','u','r','e','s',		// L_CLEAR_ALL_MEASUREMENTS (french)  26(0)
'M','e','a','s','u','r','e',' ','N','o','w',		// L_MEASURE_NOW (english)  11(0), 748
'M','e','s','u','r','e','r',' ','m','a','i','n','t','e','n','a','n','t',		// L_MEASURE_NOW (french)  18(0)
'N','o',' ','S','e','t','t','i','n','g','s',		// L_NO_SETTINGS (english)  11(0), 749
'A','u','c','u','n',' ','r',0xE9,'g','l','a','g','e',		// L_NO_SETTINGS (french)  13(1)
'f','o','r',' ','R',' ','C','h','a','m','b','e','r',		// L_FOR_R_CHAMBER (english)  13(0), 750
'p','o','u','r',' ','l','a',' ','c','h','a','m','b','r','e',' ','R',		// L_FOR_R_CHAMBER (french)  17(0)
'f','o','r',' ','R',' ','C','h','a','m','b','e','r',		// L_FOR_R_CHAMBER2 (english)  13(0), 751
'd','e',' ','c','h','a','m','b','r','e',' ','R',		// L_FOR_R_CHAMBER2 (french)  12(0)
'f','o','r',' ','P','E','T',' ','C','h','a','m','b','e','r',		// L_FOR_PET_CHAMBER (english)  15(0), 752
'p','o','u','r',' ','l','a',' ','c','h','a','m','b','r','e',' ','P','E','T',		// L_FOR_PET_CHAMBER (french)  19(0)
'f','o','r',' ','P','E','T',' ','C','h','a','m','b','e','r',		// L_FOR_PET_CHAMBER2 (english)  15(0), 753
'd','e',' ','c','h','a','m','b','r','e',' ','P','E','T',		// L_FOR_PET_CHAMBER2 (french)  14(0)
'c','a','l','i','b','r','a','t','i','o','n',' ','n','u','m','b','e','r',		// L_CALIBRATION_NUMBER (english)  18(0), 754
'n','u','m',0xE9,'r','o',' ','d','e',' ','c','a','l','i','b','r','a','t','i','o','n',		// L_CALIBRATION_NUMBER (french)  21(1)
'L','i','n','e','a','r','i','t','y',',',' ','S','t','d',		// L_LINEARITY_STD (english)  14(0), 755
'L','i','n',0xE9,'a','r','i','t',0xE9,',',' ','S','t','d',		// L_LINEARITY_STD (french)  14(2)
'i','n',' ','%','d',' ','h','r','s',',',' ','%','d',' ','m','i','n','s',		// L_IN_HRS_MINS (english)  18(0), 756
'e','n',' ','%','d',' ','h','r','s',',',' ','%','d',' ','m','i','n','s',		// L_IN_HRS_MINS (french)  18(0)
'F','i','r','s','t',' ','M','e','a','s','u','r','e','m','e','n','t',':',		// L_FIRST_MEASUREMENT (english)  18(0), 757
'P','r','e','m','i',0xE8,'r','e',' ','m','e','s','u','r','e',' ',':',		// L_FIRST_MEASUREMENT (french)  17(1)
'C','a','l','c',':',' ',		// L_CALC (english)  6(0), 758
'C','a','l','c',' ',':',' ',		// L_CALC (french)  7(0)
'F','a','c','t','o','r',		// L_FACTOR (english)  6(0), 759
'F','a','c','t','e','u','r',		// L_FACTOR (french)  7(0)
'%',' ','R','a','t','i','o',		// L_RATIO (english)  7(0), 760
'%',' ','R','a','p','p','o','r','t',		// L_RATIO (french)  9(0)
'R','a','t','i','o',':',		// L_RATIO2 (english)  6(0), 761
'R','a','p','p','o','r','t',' ',':',		// L_RATIO2 (french)  9(0)
'R','e','s','u','l','t',		// L_RESULT (english)  6(0), 762
'R',0xE9,'s','u','l','t','a','t',		// L_RESULT (french)  8(1)
'R','e','s','u','l','t',':',		// L_RESULT2 (english)  7(0), 763
'R',0xE9,'s','u','l','t','a','t',' ',':',		// L_RESULT2 (french)  10(1)
'S','t','d',' ','L','i','n','e','a','r','i','t','y',		// L_STD_LINEARITY (english)  13(0), 764
'L','i','n',0xE9,'a','r','i','t',0xE9,' ','s','t','d',		// L_STD_LINEARITY (french)  13(2)
'S','T','A','N','D','A','R','D',' ','L','I','N','E','A','R','I','T','Y',' ','T','E','S','T',		// L_STANDARD_LINEARITY_TEST (english)  23(0), 765
'T','E','S','T',' ','D','E',' ','L','I','N',0xC9,'A','R','I','T',0xC9,' ','S','T','A','N','D','A','R','D',		// L_STANDARD_LINEARITY_TEST (french)  26(2)
'A','c','t','u','a','l',':',		// L_ACTUAL (english)  7(0), 766
'R',0xE9,'e','l',' ',':',		// L_ACTUAL (french)  6(1)
'V','a','r','i','a','t','i','o','n',':',		// L_VARIATION (english)  10(0), 767
'V','a','r','i','a','t','i','o','n',' ',':',		// L_VARIATION (french)  11(0)
'L','I','N','E','A','T','O','R',' ','L','I','N','E','A','R','I','T','Y',' ','T','E','S','T',		// L_LINEATOR_LINEARITY_TEST (english)  23(0), 768
'T','E','S','T',' ','D','E',' ','L','I','N',0xC9,'A','R','I','T',0xC9,' ','L','I','N','E','A','T','O','R',		// L_LINEATOR_LINEARITY_TEST (french)  26(2)
'C','h','a','m','b','e','r',' ','S','/','N',':',' ','%','s',		// L_CHAMBER_SN (english)  15(0), 769
'C','h','a','m','b','r','e',' ','S','/','N',' ',':',' ','%','s',		// L_CHAMBER_SN (french)  16(0)
'L','i','n','e','a','t','o','r',' ','S','/','N',':',' ','%','s',		// L_LINEATOR_SN (english)  16(0), 770
'L','i','n','e','a','t','o','r',' ','S','/','N',' ',':',' ','%','s',		// L_LINEATOR_SN (french)  17(0)
'P','r','e','s','e','n','t',' ','F','a','c','t','o','r',':',		// L_PRESENT_FACTOR (english)  15(0), 771
'F','a','c','t','e','u','r',' ','p','r',0xE9,'s','e','n','t',' ',':',		// L_PRESENT_FACTOR (french)  17(1)
'C','A','L','I','C','H','E','C','K',' ','L','I','N','E','A','R','I','T','Y',' ','T','E','S','T',		// L_CALICHECK_LINEARITY_TEST (english)  24(0), 772
'T','E','S','T',' ','D','E',' ','L','I','N',0xC9,'A','R','I','T',0xC9,' ','C','A','L','I','C','H','E','C','K',		// L_CALICHECK_LINEARITY_TEST (french)  27(2)
'C','a','l','i','c','h','e','c','k',' ','S','/','N',':',' ','%','s',		// L_CALICHECK_SN (english)  17(0), 773
'C','a','l','i','c','h','e','c','k',' ','S','/','N',' ',':',' ','%','s',		// L_CALICHECK_SN (french)  18(0)
'M','e','a','n',' ','R','e','s','u','l','t',':',' ','%','s',		// L_MEAN_RESULT (english)  15(0), 774
'R',0xE9,'s','u','l','t','a','t',' ','m','o','y','e','n',' ',':',' ','%','s',		// L_MEAN_RESULT (french)  19(1)
'V','a','r','i','a','t','i','o','n',' ','f','r','o','m',' ','M','e','a','n',':',		// L_VARIATION_FROM_MEAN (english)  20(0), 775
'V','a','r','i','a','t','i','o','n',' ','d','e',' ','l','a',' ','m','o','y','e','n','n','e',' ',':',		// L_VARIATION_FROM_MEAN (french)  25(0)
'P','C',' ','C','o','m','m','u','n','i','c','a','t','i','o','n','s',' ','M','o','d','e',		// L_PC_COMMUNICATIONS_MODE (english)  22(0), 776
'M','o','d','e',' ','d','e',' ','c','o','m','m','u','n','i','c','a','t','i','o','n',' ','P','C',		// L_PC_COMMUNICATIONS_MODE (french)  24(0)
'R','u','n','n','i','n','g',' ','Q','C',' ','t','e','s','t','s',' ','f','r','o','m',' ','P','C',		// L_RUNNING_QC_TESTS_FROM_PC (english)  24(0), 777
'E','x',0xE9,'c','u','t','i','o','n',' ','d','e','s',' ','t','e','s','t','s',' ','Q','C',' ','d','e','p','u','i','s',' ','l','e',' ','P','C',		// L_RUNNING_QC_TESTS_FROM_PC (french)  35(1)
'A','b','o','r','t',' ','Q','C',' ','T','e','s','t','s',		// L_ABORT_QC_TESTS (english)  14(0), 778
'A','b','a','n','d','o','n','n','e','r',' ','t','e','s','t','s',' ','Q','C',		// L_ABORT_QC_TESTS (french)  19(0)
'R','e','m','o','t','e',' ','A','u','t','o',' ','Z','e','r','o',		// L_REMOTE_AUTO_ZERO (english)  16(0), 779
'A','u','t','o',' ','z',0xE9,'r','o',' ','d','i','s','t','a','n','t',		// L_REMOTE_AUTO_ZERO (french)  17(1)
'R','e','m','o','t','e',' ','B','a','c','k','g','r','o','u','n','d',		// L_REMOTE_BACKGROUND (english)  17(0), 780
'B','r','u','i','t',' ','d','e',' ','f','o','n','d',' ','d','i','s','t','a','n','t',		// L_REMOTE_BACKGROUND (french)  21(0)
'R','e','m','o','t','e',' ','C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',' ','T','e','s','t',		// L_REMOTE_CHAMBER_VOLTAGE_TEST (english)  27(0), 781
'T','e','s','t',' ','t','e','n','s','i','o','n',' ','c','h','a','m','b','r','e',' ','d','i','s','t','a','n','t',		// L_REMOTE_CHAMBER_VOLTAGE_TEST (french)  28(0)
'B','a','c','k','g','r','o','u','n','d',' ','H','i','g','h',		// L_BACKGROUND_HIGH (english)  15(0), 782
'B','r','u','i','t',' ','d','e',' ','f','o','n','d',' ',0xE9,'l','e','v',0xE9,		// L_BACKGROUND_HIGH (french)  19(2)
'C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',' ','F','a','i','l','e','d',		// L_CHAMBER_VOLTAGE_FAILED (english)  22(0), 783
'T','e','n','s','i','o','n',' ','c','h','a','m','b','r','e',' ',0xE9,'c','h','o','u',0xE9,'e',		// L_CHAMBER_VOLTAGE_FAILED (french)  23(2)
'P','l','e','a','s','e',' ','W','a','i','t',		// L_PLEASE_WAIT (english)  11(0), 784
'V','e','u','i','l','l','e','z',' ','p','a','t','i','e','n','t','e','r',		// L_PLEASE_WAIT (french)  18(0)
'T','h','e',' ','P','E','T',' ','c','h','a','m','b','e','r',' ','c','a','n',' ','n','o','t',' ','r','u','n',' ','a',' ','M','o','l','y',' ','A','s','s','a','y',		// L_PET_CHAMBER_CANNOT_RUN_MOLY_ASSAY (english)  40(0), 785
'L','a',' ','c','h','a','m','b','r','e',' ','P','E','T',' ','n','e',' ','p','e','u','t',' ','p','a','s',' ','e','x',0xE9,'c','u','t','e','r',' ','u','n',' ','e','s','s','a','i',' ','M','o','l','y',		// L_PET_CHAMBER_CANNOT_RUN_MOLY_ASSAY (french)  49(1)
'M','i','s','s','i','n','g',' ','U','S','B',' ','D','r','i','v','e',		// L_MISSING_USB_DRIVE (english)  17(0), 786
'C','l',0xE9,' ','U','S','B',' ','m','a','n','q','u','a','n','t','e',		// L_MISSING_USB_DRIVE (french)  17(1)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','f','i','l','e',		// L_UNABLE_TO_FIND_FILE (english)  19(0), 787
'I','m','p','o','s','s','i','b','l','e',' ','d','e',' ','t','r','o','u','v','e','r',' ','l','e',' ','f','i','c','h','i','e','r',		// L_UNABLE_TO_FIND_FILE (french)  32(0)
'P','l','a','c','e',' ','o','n','l','y',' ','o','n','e',' ','u','p','d','a','t','e',		// L_PLACE_ONLY_ONE_UPDATE (english)  21(0), 788
'P','l','a','c','e','z',' ','u','n',' ','s','e','u','l',' ','f','i','c','h','i','e','r',' ','d','e',		// L_PLACE_ONLY_ONE_UPDATE (french)  25(0)
'f','i','l','e',' ','i','n',' ','r','o','o','t',' ','f','o','l','d','e','r',		// L_FILE_IN_ROOT_FOLDER (english)  19(0), 789
'm','i','s','e',' ',0xE0,' ','j','o','u','r',' ','d','a','n','s',' ','l','e',' ','d','o','s','s','i','e','r',' ','r','a','c','i','n','e',		// L_FILE_IN_ROOT_FOLDER (french)  34(1)
'I','n','v','a','l','i','d',' ','u','p','d','a','t','e',' ','f','i','l','e',		// L_INVALID_UPDATE_FILE (english)  19(0), 790
'F','i','c','h','i','e','r',' ','d','e',' ','m','i','s','e',' ',0xE0,' ','j','o','u','r',' ','i','n','v','a','l','i','d','e',		// L_INVALID_UPDATE_FILE (french)  31(1)
'C','o','p','y','i','n','g',' ','%','s',		// L_COPYING (english)  10(0), 791
'C','o','p','i','e',' ','%','s',		// L_COPYING (french)  8(0)
'o','f',' ','%','s',		// L_OF2 (english)  5(0), 792
'd','e',' ','%','s',		// L_OF2 (french)  5(0)
'C','o','r','r','u','p','t',' ','U','p','d','a','t','e',' ','F','i','l','e',		// L_CORRUPT_UPDATE_FILE (english)  19(0), 793
'F','i','c','h','i','e','r',' ','d','e',' ','m','i','s','e',' ',0xE0,' ','j','o','u','r',' ','c','o','r','r','o','m','p','u',		// L_CORRUPT_UPDATE_FILE (french)  31(1)
'F','i','n','i','s','h','e','d',' ','C','o','p','y','i','n','g',		// L_FINISHED_COPYING (english)  16(0), 794
'C','o','p','i','e',' ','t','e','r','m','i','n',0xE9,'e',		// L_FINISHED_COPYING (french)  14(1)
'P','l','e','a','s','e',' ','r','e','s','t','a','r','t',		// L_PLEASE_RESTART2 (english)  14(0), 795
'V','e','u','i','l','l','e','z',' ','r','e','d',0xE9,'m','a','r','r','e','r',		// L_PLEASE_RESTART2 (french)  19(1)
'U','S','B',' ','D','r','i','v','e',' ','E','r','r','o','r','\n',		// L_USB_DRIVE_ERROR (english)  16(0), 796
'E','r','r','e','u','r',' ','c','l',0xE9,' ','U','S','B','\n',		// L_USB_DRIVE_ERROR (french)  15(1)
'S','o','f','t','w','a','r','e',' ','U','p','d','a','t','e',		// L_SOFTWARE_UPDATE (english)  15(0), 797
'M','i','s','e',' ',0xE0,' ','j','o','u','r',' ','l','o','g','i','c','i','e','l','l','e',		// L_SOFTWARE_UPDATE (french)  22(1)
'S','o','f','t','w','a','r','e',' ','d','o','w','n','l','o','a','d',' ','i','s',' ','i','n',' ','p','r','o','g','r','e','s','s','\n','P','l','e','a','s','e',' ','d','o',' ','n','o','t',' ','t','u','r','n',' ','o','f','f',' ','c','a','l','i','b','r','a','t','o','r',		// L_SOFTWARE_DOWNLOAD_IN_PROGRESS (english)  66(0), 798
'T',0xE9,'l',0xE9,'c','h','a','r','g','e','m','e','n','t',' ','e','n',' ','c','o','u','r','s','\n','V','e','u','i','l','l','e','z',' ','n','e',' ','p','a','s',' ',0xE9,'t','e','i','n','d','r','e',' ','l','e',' ','c','a','l','i','b','r','a','t','e','u','r',		// L_SOFTWARE_DOWNLOAD_IN_PROGRESS (french)  63(3)
'P','r','i','n','t','e','r',' ','P','r','o','b','l','e','m',		// L_PRINTER_PROBLEM (english)  15(0), 799
'P','r','o','b','l',0xE8,'m','e',' ','d',0x27,'i','m','p','r','i','m','a','n','t','e',		// L_PRINTER_PROBLEM (french)  21(1)
'I','N','S','E','R','T',' ','T','I','C','K','E','T',		// L_INSERT_TICKET (english)  13(0), 800
'I','N','S',0xC9,'R','E','R',' ','L','E',' ','T','I','C','K','E','T',		// L_INSERT_TICKET (french)  17(1)
'C','H','E','C','K',' ','P','R','I','N','T','E','R',		// L_CHECK_PRINTER (english)  13(0), 801
'V',0xC9,'R','I','F','I','E','R',' ','L',0x27,'I','M','P','R','I','M','A','N','T','E',		// L_CHECK_PRINTER (french)  21(1)
'C','O','N','N','E','C','T',' ','P','R','I','N','T','E','R',		// L_CONNECT_PRINTER (english)  15(0), 802
'C','O','N','N','E','C','T','E','R',' ','I','M','P','R','I','M','A','N','T','E',		// L_CONNECT_PRINTER (french)  20(0)
'I','N','S','E','R','T',' ','P','A','P','E','R',		// L_INSERT_PAPER (english)  12(0), 803
'I','N','S',0xC9,'R','E','R',' ','L','E',' ','P','A','P','I','E','R',		// L_INSERT_PAPER (french)  17(1)
'F','i','l','e',' ','T','r','a','n','s','f','e','r',' ','E','r','r','o','r',		// L_FILE_TRANSFER_ERROR (english)  19(0), 804
'E','r','r','e','u','r',' ','t','r','a','n','s','f','e','r','t',' ','f','i','c','h','i','e','r',		// L_FILE_TRANSFER_ERROR (french)  24(0)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','f','i','l','e',' ','p','a','t','h',':',' ','%','s','\n',		// L_UNABLE_TO_FIND_FILE_PATH (english)  29(0), 805
'C','h','e','m','i','n',' ','d','u',' ','f','i','c','h','i','e','r',' ','i','n','t','r','o','u','v','a','b','l','e',' ',':',' ','%','s','\n',		// L_UNABLE_TO_FIND_FILE_PATH (french)  35(0)
'U','p','d','a','t','e',		// L_UPDATE (english)  6(0), 806
'M','e','t','t','r','e',' ',0xE0,' ','j','o','u','r',		// L_UPDATE (french)  13(1)
'P','o','w','e','r',' ','O','f','f',',','\n','a','n','d',' ','t','h','e','n',' ','P','o','w','e','r',' ','O','n','\n','t','o',' ','c','o','m','p','l','e','t','e',' ','u','p','d','a','t','e','.',		// L_POWER_OFF_AND_THEN_POWER_ON_TO_COMPLETE_UPDATE (english)  48(0), 807
0xC9,'t','e','i','g','n','e','z',',','\n','p','u','i','s',' ','r','a','l','l','u','m','e','z','\n','p','o','u','r',' ','t','e','r','m','i','n','e','r',' ','l','a',' ','m','i','s','e',' ',0xE0,' ','j','o','u','r','.',		// L_POWER_OFF_AND_THEN_POWER_ON_TO_COMPLETE_UPDATE (french)  53(2)
'C','h','e','c','k','s','u','m',' ','h','a','s',' ','f','a','i','l','e','d','.',		// L_CHECKSUM_HAS_FAILED (english)  20(0), 808
'L','a',' ','s','o','m','m','e',' ','d','e',' ','c','o','n','t','r',0xF4,'l','e',' ','a',' ',0xE9,'c','h','o','u',0xE9,'.',		// L_CHECKSUM_HAS_FAILED (french)  30(3)
'U','n','a','b','l','e',' ','t','o',' ','f','i','n','d',' ','t','r','a','n','s','f','e','r',' ','f','i','l','e','.',		// L_UNABLE_TO_FIND_TRANSFER_FILE (english)  29(0), 809
'F','i','c','h','i','e','r',' ','d','e',' ','t','r','a','n','s','f','e','r','t',' ','i','n','t','r','o','u','v','a','b','l','e','.',		// L_UNABLE_TO_FIND_TRANSFER_FILE (french)  33(0)
'U','p','d','a','t','e',' ','S','o','f','t','w','a','r','e',		// L_UPDATE_SOFTWARE (english)  15(0), 810
'M','e','t','t','r','e',' ',0xE0,' ','j','o','u','r',' ','l','e',' ','l','o','g','i','c','i','e','l',		// L_UPDATE_SOFTWARE (french)  25(1)
'N','o','w',':',		// L_NOW (english)  4(0), 811
'M','a','i','n','t','e','n','a','n','t',' ',':',		// L_NOW (french)  12(0)
'C','o','n','s','t','a','n','c','y',' ','C','h','a','n','n','e','l','s',':',		// L_PR_CONSTANCY_CHANNELS (english)  19(0), 812
'C','a','n','a','u','x',' ','d','e',' ','c','o','n','s','t','a','n','c','e',' ',':',		// L_PR_CONSTANCY_CHANNELS (french)  21(0)
' ','O','V','E','R',' ',		// L_OVER (english)  6(0), 813
' ','D',0xC9,'P','A','S','S',0xC9,' ',		// L_OVER (french)  9(2)
'C','h','a','m','b','e','r',' ','E','r','r','o','r',		// L_CHAMBER_ERROR (english)  13(0), 814
'E','r','r','e','u','r',' ','c','h','a','m','b','r','e',		// L_CHAMBER_ERROR (french)  14(0)
'C','h','a','m','b','e','r',' ','#','%','d',' ','h','a','s',' ','l','o','s','t',' ','c','o','n','n','e','c','t','i','o','n','.','\n','P','l','e','a','s','e',' ','t','u','r','n',' ','o','f','f',' ','c','a','l','i','b','r','a','t','o','r',' ','a','n','d',' ','r','e','a','t','t','a','c','h',' ','c','h','a','m','b','e','r','.',		// L_CHAMBER_LOST_CONNECTION (english)  81(0), 815
'L','a',' ','c','h','a','m','b','r','e',' ','#','%','d',' ','a',' ','p','e','r','d','u',' ','l','a',' ','c','o','n','n','e','x','i','o','n','.','\n','V','e','u','i','l','l','e','z',' ',0xE9,'t','e','i','n','d','r','e',' ','l','e',' ','c','a','l','i','b','r','a','t','e','u','r',' ','e','t',' ','r','e','c','o','n','n','e','c','t','e','r',' ','l','a',' ','c','h','a','m','b','r','e','.',		// L_CHAMBER_LOST_CONNECTION (french)  96(1)
'S','a','v','e',' ','A','c','c','u','r','a','c','y',' ','/',' ','A','u','t','o','C','o','n','s','t','a','n','c','y',		// L_SAVE_ACCURACY_TEST (english)  29(0), 816
'E','n','r','e','g','i','s','t','r','e','r',' ','e','x','a','c','t','i','t','u','d','e',' ','/',' ','A','u','t','o','C','o','n','s','t','a','n','c','e',		// L_SAVE_ACCURACY_TEST (french)  38(0)
'Z','e','r','o',		// L_ZERO_2 (english)  4(0), 817
'Z',0xE9,'r','o',		// L_ZERO_2 (french)  4(1)
'C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',		// L_CHAMBER_VOLTAGE_2 (english)  15(0), 818
'T','e','n','s','i','o','n',' ','c','h','a','m','b','r','e',		// L_CHAMBER_VOLTAGE_2 (french)  15(0)
'A','c','c','u','r','a','c','y',' ','T','e','s','t',		// L_ACCURACYTEST_2 (english)  13(0), 819
'T','e','s','t',' ','d',0x27,'e','x','a','c','t','i','t','u','d','e',		// L_ACCURACYTEST_2 (french)  17(0)
'S','e','a','r','c','h',' ','D','a','i','l','y',' ','T','e','s','t','s',		// L_SEARCH_DAILY_TESTS (english)  18(0), 820
'R','e','c','h','e','r','c','h','e','r',' ','t','e','s','t','s',' ','q','u','o','t','i','d','i','e','n','s',		// L_SEARCH_DAILY_TESTS (french)  27(0)
'S','e','a','r','c','h',' ','Z','e','r','o',' ','M','e','a','s','u','r','e','m','e','n','t','s',		// L_SEARCH_ZERO_MEASUREMENTS (english)  24(0), 821
'R','e','c','h','e','r','c','h','e','r',' ','m','e','s','u','r','e','s',' ','z',0xE9,'r','o',		// L_SEARCH_ZERO_MEASUREMENTS (french)  23(1)
'S','e','a','r','c','h',' ','B','a','c','k','g','r','o','u','n','d',' ','M','e','a','s','u','r','e','m','e','n','t','s',		// L_SEARCH_BACKGROUND_MEASUREMENTS (english)  30(0), 822
'R','e','c','h','e','r','c','h','e','r',' ','m','e','s','u','r','e','s',' ','b','r','u','i','t',' ','d','e',' ','f','o','n','d',		// L_SEARCH_BACKGROUND_MEASUREMENTS (french)  32(0)
'S','e','a','r','c','h',' ','C','h','a','m','b','e','r',' ','V','o','l','t','a','g','e',' ','T','e','s','t','s',		// L_SEARCH_CHAMBER_VOLTAGE (english)  28(0), 823
'R','e','c','h','e','r','c','h','e','r',' ','t','e','s','t','s',' ','t','e','n','s','i','o','n',' ','c','h','a','m','b','r','e',		// L_SEARCH_CHAMBER_VOLTAGE (french)  32(0)
'S','e','a','r','c','h',' ','A','c','c','u','r','a','c','y',' ','T','e','s','t','s',		// L_SEARCH_ACCURACY_TESTS (english)  21(0), 824
'R','e','c','h','e','r','c','h','e','r',' ','t','e','s','t','s',' ','d',0x27,'e','x','a','c','t','i','t','u','d','e',		// L_SEARCH_ACCURACY_TESTS (french)  29(0)
'N','O','M','I','N','A','L',' ','V','O','L','T','A','G','E',':',		// L_NOMINAL_VOLTAGE (english)  16(0), 825
'T','E','N','S','I','O','N',' ','N','O','M','I','N','A','L','E',' ',':',		// L_NOMINAL_VOLTAGE (french)  18(0)
'M','I','N',' ','V','O','L','T','A','G','E',':',		// L_MIN_VOLTAGE (english)  12(0), 826
'T','E','N','S','I','O','N',' ','M','I','N',' ',':',		// L_MIN_VOLTAGE (french)  13(0)
'M','A','X',' ','V','O','L','T','A','G','E',':',		// L_MAX_VOLTAGE (english)  12(0), 827
'T','E','N','S','I','O','N',' ','M','A','X',' ',':',		// L_MAX_VOLTAGE (french)  13(0)
'S','t','a','g','e',		// L_STAGE (english)  5(0), 828
0xC9,'t','a','p','e',		// L_STAGE (french)  5(1)
'P','a','g','e',' ','%','d',		// L_PAGE (english)  7(0), 829
'P','a','g','e',' ','%','d',		// L_PAGE (french)  7(0)
'H','a','l','f','-','l','i','f','e',' ','C','a','l','c','u','l','a','t','o','r',		// L_HALFLIFE_CALC (english)  20(0), 830
'C','a','l','c','u','l','a','t','e','u','r',' ','d','e','m','i','-','v','i','e',		// L_HALFLIFE_CALC (french)  20(0)
'H','a','l','f','-','l','i','f','e',' ','C','a','l','c',' ','-',' ','C','h',':','%','d',',',' ','%','s',		// L_HALFLIFE_CALC2 (english)  26(0), 831
'C','a','l','c','.',' ','d','e','m','i','-','v','i','e',' ','-',' ','C','h',' ',':','%','d',',',' ','%','s',		// L_HALFLIFE_CALC2 (french)  27(0)
'%','d',' ','s','e','c',		// L_SECS (english)  6(0), 832
'%','d',' ','s','e','c',		// L_SECS (french)  6(0)
'E','n','t','e','r',' ','I','n','t','e','r','v','a','l',' ','i','n',' ','s','e','c','o','n','d','s',':',		// L_ENTER_INTERVAL_SEC (english)  26(0), 833
'E','n','t','r','e','z',' ','l',0x27,'i','n','t','e','r','v','a','l','l','e',' ','e','n',' ','s','e','c','o','n','d','e','s',' ',':',		// L_ENTER_INTERVAL_SEC (french)  33(0)
'E','n','t','e','r',' ','T','o','t','a','l',' ','T','i','m','e',' ','i','n',' ','m','i','n','u','t','e','s',':',		// L_ENTER_TOTAL_TIME_IN_MINUTES (english)  28(0), 834
'E','n','t','r','e','z',' ','l','e',' ','t','e','m','p','s',' ','t','o','t','a','l',' ','e','n',' ','m','i','n','u','t','e','s',' ',':',		// L_ENTER_TOTAL_TIME_IN_MINUTES (french)  34(0)
'%','d',' ','m','i','n',		// L_MINS (english)  6(0), 835
'%','d',' ','m','i','n',		// L_MINS (french)  6(0)
'S','t','o','p',		// L_STOP (english)  4(0), 836
'A','r','r',0xEA,'t','e','r',		// L_STOP (french)  7(1)
'E','l','a','p','s','e','d',' ','(','s','e','c',')',		// L_ELAPSED_SEC (english)  13(0), 837
0xC9,'c','o','u','l',0xE9,' ','(','s','e','c',')',		// L_ELAPSED_SEC (french)  12(2)
'C','o','u','n','t',' ','i','s',' ','g','r','e','a','t','e','r',' ','t','h','a','n',' ','1','2','0','1','\n','T','r','y',' ','T','o','t','a','l',':',' ','%','d',' ','o','r',' ','I','n','t','e','r','v','a','l',':',' ','%','d',		// L_NUM_OF_MEAS_GREATER_THAN_1201 (english)  56(0), 838
'L','e',' ','c','o','m','p','t','a','g','e',' ','d',0xE9,'p','a','s','s','e',' ','1','2','0','1','\n','E','s','s','a','y','e','z',' ','T','o','t','a','l',' ',':',' ','%','d',' ','o','u',' ','I','n','t','e','r','v','a','l','l','e',' ',':',' ','%','d',		// L_NUM_OF_MEAS_GREATER_THAN_1201 (french)  62(1)
'E','l','a','p','s','e','d',':',' ','%','d',':','%','0','2','d','\n','R','e','m','a','i','n','i','n','g',':',' ','%','d',':','%','0','2','d',		// L_ELAPSED_REMAINING (english)  35(0), 839
0xC9,'c','o','u','l',0xE9,' ',':',' ','%','d',':','%','0','2','d','\n','R','e','s','t','a','n','t',' ',':',' ','%','d',':','%','0','2','d',		// L_ELAPSED_REMAINING (french)  34(2)
'U','n','d','e','r',' ','R','a','n','g','e',		// L_UNDERRANGE (english)  11(0), 840
'S','o','u','s',' ','l','a',' ','p','l','a','g','e',		// L_UNDERRANGE (french)  13(0)
'L','e','s','s',' ','t','h','a','n',' ','5',' ','m','e','a','s','u','r','e','m','e','n','t','s',		// L_LESS_THAN_5_MEASUREMENTS (english)  24(0), 841
'M','o','i','n','s',' ','d','e',' ','5',' ','m','e','s','u','r','e','s',		// L_LESS_THAN_5_MEASUREMENTS (french)  18(0)
'H','a','l','f','-','l','i','f','e',' ','i','s',' ','t','o','o',' ','s','h','o','r','t',		// L_HALFLIFE_TOO_SHORT (english)  22(0), 842
'L','a',' ','d','e','m','i','-','v','i','e',' ','e','s','t',' ','t','r','o','p',' ','c','o','u','r','t','e',		// L_HALFLIFE_TOO_SHORT (french)  27(0)
'H','a','l','f','-','l','i','f','e',' ','i','s',' ','t','o','o',' ','l','o','n','g',		// L_HALFLIFE_TOO_LONG (english)  21(0), 843
'L','a',' ','d','e','m','i','-','v','i','e',' ','e','s','t',' ','t','r','o','p',' ','l','o','n','g','u','e',		// L_HALFLIFE_TOO_LONG (french)  27(0)
'H','a','l','f','-','l','i','f','e',' ','i','s',' ','n','e','g','a','t','i','v','e',		// L_HALFLIFE_NEGATIVE (english)  21(0), 844
'L','a',' ','d','e','m','i','-','v','i','e',' ','e','s','t',' ','n',0xE9,'g','a','t','i','v','e',		// L_HALFLIFE_NEGATIVE (french)  24(1)
'C','h','a','m','b','e','r',' ','S','/','N',':',		// L_CHAMBER_SN_2 (english)  12(0), 845
'C','h','a','m','b','r','e',' ','S','/','N',' ',':',		// L_CHAMBER_SN_2 (french)  13(0)
'P','r','i','n','t','\n','S','u','m','m','a','r','y',		// L_PRINT_SUMMARY (english)  13(0), 846
'I','m','p','r','i','m','e','r','\n','R',0xE9,'s','u','m',0xE9,		// L_PRINT_SUMMARY (french)  15(2)
'B','T',' ','C','H','A','M','B','E','R',		// L_PR_BT_CHAMBER (english)  10(0), 847
'C','H','A','M','B','R','E',' ','B','T',		// L_PR_BT_CHAMBER (french)  10(0)
'1','.','8',' ','A','t','m',' ','C','H','A','M','B','E','R',		// L_PR_18_CHAMBER (english)  15(0), 848
'C','H','A','M','B','R','E',' ','1','.','8',' ','A','t','m',		// L_PR_18_CHAMBER (french)  15(0)
'B','T',' ','C','h','a','m','b','e','r',		// L_BT_CHAMBER (english)  10(0), 849
'c','h','a','m','b','r','e',' ','B','T',		// L_BT_CHAMBER (french)  10(0)
'1','.','8',' ','A','t','m',' ','C','h','a','m','b','e','r',		// L_18_CHAMBER (english)  15(0), 850
'c','h','a','m','b','r','e',' ','1','.','8',' ','A','t','m',		// L_18_CHAMBER (french)  15(0)
'1',' ','A','t','m',' ','C','h','a','m','b','e','r',		// L_10_CHAMBER (english)  13(0), 851
'c','h','a','m','b','r','e',' ','1',' ','A','t','m',		// L_10_CHAMBER (french)  13(0)
'f','o','r',' ','B','T',' ','C','h','a','m','b','e','r',		// L_FOR_BT_CHAMBER (english)  14(0), 852
'p','o','u','r',' ','l','a',' ','c','h','a','m','b','r','e',' ','B','T',		// L_FOR_BT_CHAMBER (french)  18(0)
'f','o','r',' ','1','.','8',' ','A','t','m',' ','C','h','a','m','b','e','r',		// L_FOR_18_CHAMBER (english)  19(0), 853
'p','o','u','r',' ','l','a',' ','c','h','a','m','b','r','e',' ','1','.','8',' ','A','t','m',		// L_FOR_18_CHAMBER (french)  23(0)
'f','o','r',' ','B','T',' ','C','h','a','m','b','e','r',		// L_FOR_BT_CHAMBER2 (english)  14(0), 854
'd','e',' ','c','h','a','m','b','r','e',' ','B','T',		// L_FOR_BT_CHAMBER2 (french)  13(0)
'f','o','r',' ','1','.','8',' ','A','t','m',' ','C','h','a','m','b','e','r',		// L_FOR_18_CHAMBER2 (english)  19(0), 855
'd','e',' ','c','h','a','m','b','r','e',' ','1','.','8',' ','A','t','m',		// L_FOR_18_CHAMBER2 (french)  18(0)
'C','R','C','-','7','7','t',' ',' ',' ',' ',' ',' ','R','A','D','I','O','I','S','O','T','O','P','E',' ','D','O','S','E',' ','C','A','L','I','B','R','A','T','O','R',		// L_77T_FULL_TITLE (english)  41(0), 856
'C','R','C','-','7','7','t',' ',' ',' ',' ',' ',' ','C','A','L','I','B','R','A','T','E','U','R',' ','D','E',' ','D','O','S','E',' ','R','A','D','I','O','I','S','O','T','O','P','E',		// L_77T_FULL_TITLE (french)  45(0)
'T','h','e',' ','1',' ','A','t','m',' ','c','h','a','m','b','e','r',' ','c','a','n',' ','n','o','t',' ','r','u','n',' ','a',' ','M','o','l','y',' ','A','s','s','a','y',		// L_10_CHAMBER_CANNOT_RUN_MOLY_ASSAY (english)  42(0), 857
'L','a',' ','c','h','a','m','b','r','e',' ','1',' ','A','t','m',' ','n','e',' ','p','e','u','t',' ','p','a','s',' ','e','x',0xE9,'c','u','t','e','r',' ','u','n',' ','e','s','s','a','i',' ','M','o','l','y',		// L_10_CHAMBER_CANNOT_RUN_MOLY_ASSAY (french)  51(1)
'S','e','t','u','p',' ','H','o','t','k','e','y','s',		// L_SETUP_HOTKEYS (english)  13(0), 858
'C','o','n','f','i','g','.',' ','r','a','c','c','o','u','r','c','i','s',		// L_SETUP_HOTKEYS (french)  18(0)
'P','l','e','a','s','e',' ','E','n','t','e','r',' ','C','a','l',' ','#',':',		// L_ENTER_CAL (english)  19(0), 859
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','C','a','l',' ','#',' ',':',		// L_ENTER_CAL (french)  23(0)
'S','e','t','u','p',' ','L','i','n','e','a','r','i','t','y',' ','S','t','a','n','d','a','r','d',		// L_SETUP_LINEARITY_STANDARD (english)  24(0), 860
'C','o','n','f','i','g','.',' ','s','t','a','n','d','a','r','d',' ','l','i','n',0xE9,'a','r','i','t',0xE9,		// L_SETUP_LINEARITY_STANDARD (french)  26(2)
'H','R',' ','C','H','A','M','B','E','R',		// L_PR_C_CHAMBER (english)  10(0), 861
'C','H','A','M','B','R','E',' ','H','R',		// L_PR_C_CHAMBER (french)  10(0)
'1','K',' ','C','H','A','M','B','E','R',		// L_PR_K_CHAMBER (english)  10(0), 862
'C','H','A','M','B','R','E',' ','1','K',		// L_PR_K_CHAMBER (french)  10(0)
'H','R',' ','C','h','a','m','b','e','r',		// L_C_CHAMBER (english)  10(0), 863
'c','h','a','m','b','r','e',' ','H','R',		// L_C_CHAMBER (french)  10(0)
'1','K',' ','C','h','a','m','b','e','r',		// L_K_CHAMBER (english)  10(0), 864
'c','h','a','m','b','r','e',' ','1','K',		// L_K_CHAMBER (french)  10(0)
'f','o','r',' ','H','R',' ','C','h','a','m','b','e','r',		// L_FOR_C_CHAMBER (english)  14(0), 865
'p','o','u','r',' ','l','a',' ','c','h','a','m','b','r','e',' ','H','R',		// L_FOR_C_CHAMBER (french)  18(0)
'f','o','r',' ','1','K',' ','C','h','a','m','b','e','r',		// L_FOR_K_CHAMBER (english)  14(0), 866
'p','o','u','r',' ','l','a',' ','c','h','a','m','b','r','e',' ','1','K',		// L_FOR_K_CHAMBER (french)  18(0)
'f','o','r',' ','H','R',' ','C','h','a','m','b','e','r',		// L_FOR_C_CHAMBER2 (english)  14(0), 867
'd','e',' ','c','h','a','m','b','r','e',' ','H','R',		// L_FOR_C_CHAMBER2 (french)  13(0)
'f','o','r',' ','1','K',' ','C','h','a','m','b','e','r',		// L_FOR_K_CHAMBER2 (english)  14(0), 868
'd','e',' ','c','h','a','m','b','r','e',' ','1','K',		// L_FOR_K_CHAMBER2 (french)  13(0)
'S','t','a','n','d','a','r','d',' ','H','R',' ','C','h','a','m','b','e','r',		// L_STANDARD_C_CHAMBER (english)  19(0), 869
'S','t','a','n','d','a','r','d',' ','c','h','a','m','b','r','e',' ','H','R',		// L_STANDARD_C_CHAMBER (french)  19(0)
'S','t','a','n','d','a','r','d',' ','1','K',' ','C','h','a','m','b','e','r',		// L_STANDARD_K_CHAMBER (english)  19(0), 870
'S','t','a','n','d','a','r','d',' ','c','h','a','m','b','r','e',' ','1','K',		// L_STANDARD_K_CHAMBER (french)  19(0)
'M','a','x',' ','S','e','c','o','n','d',' ','i','s',' ','5','9',		// L_MAX_SECOND_IS_59 (english)  16(0), 871
'S','e','c','o','n','d','e',' ','m','a','x',' ','e','s','t',' ','5','9',		// L_MAX_SECOND_IS_59 (french)  18(0)
'S','t','a','r','t',' ','M','e','a','s','u','r','e','m','e','n','t',		// L_START_MEASUREMENT (english)  17(0), 872
'D',0xE9,'m','a','r','r','e','r',' ','l','a',' ','m','e','s','u','r','e',		// L_START_MEASUREMENT (french)  18(1)
'S','e','t','u','p',' ','K','e','y',		// L_SETUP_KEY (english)  9(0), 873
'C','o','n','f','i','g','.',' ','c','l',0xE9,		// L_SETUP_KEY (french)  11(1)
'D','e','v','i','c','e',' ','I','D',':',		// L_DEVICE_ID (english)  10(0), 874
'I','D',' ','a','p','p','a','r','e','i','l',' ',':',		// L_DEVICE_ID (french)  13(0)
'K','e','y',':',		// L_KEY (english)  4(0), 875
'C','l',0xE9,' ',':',		// L_KEY (french)  5(1)
'C','o','m','m','u','n','i','c','a','t','i','o','n','s',':',' ','E','n','a','b','l','e','d',		// L_COMM_ENABLED (english)  23(0), 876
'C','o','m','m','u','n','i','c','a','t','i','o','n','s',' ',':',' ','A','c','t','i','v',0xE9,'e','s',		// L_COMM_ENABLED (french)  25(1)
'C','o','m','m','u','n','i','c','a','t','i','o','n','s',':',' ','D','i','s','a','b','l','e','d',		// L_COMM_DISABLED (english)  24(0), 877
'C','o','m','m','u','n','i','c','a','t','i','o','n','s',' ',':',' ','D',0xE9,'s','a','c','t','i','v',0xE9,'e','s',		// L_COMM_DISABLED (french)  28(2)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','K','e','y',		// L_PLS_ENTER_KEY (english)  16(0), 878
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','a',' ','c','l',0xE9,		// L_PLS_ENTER_KEY (french)  22(1)
'E','r','r','o','r',':',' ','K','e','y',' ','i','s',' ','e','m','p','t','y',		// L_ERR_EMPTY_KEY (english)  19(0), 879
'E','r','r','e','u','r',' ',':',' ','l','a',' ','c','l',0xE9,' ','e','s','t',' ','v','i','d','e',		// L_ERR_EMPTY_KEY (french)  24(1)
'E','r','r','o','r',':',' ','K','e','y',' ','i','s',' ','n','o','t',' ','v','a','l','i','d',		// L_ERR_INVALID_KEY (english)  23(0), 880
'E','r','r','e','u','r',' ',':',' ','l','a',' ','c','l',0xE9,' ','e','s','t',' ','i','n','v','a','l','i','d','e',		// L_ERR_INVALID_KEY (french)  28(1)
'S','e','t','u','p',' ','P','a','s','s','w','o','r','d',		// L_SETUP_PASSWORD (english)  14(0), 881
'C','o','n','f','i','g','.',' ','m','o','t',' ','d','e',' ','p','a','s','s','e',		// L_SETUP_PASSWORD (french)  20(0)
'C','u','r','r','e','n','t',' ','P','a','s','s','w','o','r','d',':',		// L_CURRENT_PASSWORD (english)  17(0), 882
'M','o','t',' ','d','e',' ','p','a','s','s','e',' ','a','c','t','u','e','l',' ',':',		// L_CURRENT_PASSWORD (french)  21(0)
'N','e','w',' ','P','a','s','s','w','o','r','d',':',		// L_NEW_PASSWORD (english)  13(0), 883
'N','o','u','v','e','a','u',' ','m','o','t',' ','d','e',' ','p','a','s','s','e',' ',':',		// L_NEW_PASSWORD (french)  22(0)
'C','o','n','f','i','r','m',' ','P','a','s','s','w','o','r','d',':',		// L_CONFIRM_PASSWORD (english)  17(0), 884
'C','o','n','f','i','r','m','e','r',' ','m','o','t',' ','d','e',' ','p','a','s','s','e',' ',':',		// L_CONFIRM_PASSWORD (french)  24(0)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','C','u','r','r','e','n','t',' ','P','a','s','s','w','o','r','d',		// L_ENTER_CURRENT_PASSWORD (english)  29(0), 885
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','m','o','t',' ','d','e',' ','p','a','s','s','e',' ','a','c','t','u','e','l',		// L_ENTER_CURRENT_PASSWORD (french)  38(0)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','N','e','w',' ','P','a','s','s','w','o','r','d',		// L_ENTER_NEW_PASSWORD (english)  25(0), 886
'V','e','u','i','l','l','e','z',' ','e','n','t','r','e','r',' ','l','e',' ','n','o','u','v','e','a','u',' ','m','o','t',' ','d','e',' ','p','a','s','s','e',		// L_ENTER_NEW_PASSWORD (french)  39(0)
'P','l','e','a','s','e',' ','e','n','t','e','r',' ','C','o','n','f','i','r','m',' ','P','a','s','s','w','o','r','d',		// L_ENTER_CONFIRM_PASSWORD (english)  29(0), 887
'V','e','u','i','l','l','e','z',' ','c','o','n','f','i','r','m','e','r',' ','l','e',' ','m','o','t',' ','d','e',' ','p','a','s','s','e',		// L_ENTER_CONFIRM_PASSWORD (french)  34(0)
'I','n','v','a','l','i','d',' ','C','u','r','r','e','n','t',' ','P','a','s','s','w','o','r','d',		// L_INVALID_CURRENT_PASSWORD (english)  24(0), 888
'M','o','t',' ','d','e',' ','p','a','s','s','e',' ','a','c','t','u','e','l',' ','i','n','v','a','l','i','d','e',		// L_INVALID_CURRENT_PASSWORD (french)  28(0)
'N','e','w',' ','a','n','d',' ','C','o','n','f','i','r','m',' ','p','a','s','s','w','o','r','d','s',' ','d','o',' ','n','o','t',' ','m','a','t','c','h',		// L_MISMATCH_PASSWORD (english)  38(0), 889
'L','e','s',' ','m','o','t','s',' ','d','e',' ','p','a','s','s','e',' ','n','e',' ','c','o','r','r','e','s','p','o','n','d','e','n','t',' ','p','a','s',		// L_MISMATCH_PASSWORD (french)  38(0)
'U','s','e',' ','l','a','s','t',' ','3',' ','S','N',' ','d','i','g','i','t','s',		// L_LAST_3_SN (english)  20(0), 890
'U','t','i','l','i','s','e','r',' ','3',' ','d','e','r','n','i','e','r','s',' ','c','h','i','f','f','r','e','s',' ','S','N',		// L_LAST_3_SN (french)  31(0)
0
};

const LANG_MAP lang_map[] = {
0,4,
8,12,
17,31,
46,55,
70,89,
109,117,
126,131,
140,153,
172,175,
178,189,
201,218,
237,251,
268,275,
283,288,
301,317,
332,349,
365,370,
379,401,
434,438,
442,447,
453,467,
485,497,
513,521,
533,546,
561,571,
583,596,
612,624,
638,653,
670,682,
697,720,
748,773,
803,825,
848,856,
864,868,
872,877,
886,904,
923,928,
935,941,
949,955,
962,974,
988,1000,
1014,1021,
1029,1036,
1043,1051,
1059,1066,
1073,1079,
1087,1096,
1104,1111,
1121,1125,
1136,1156,
1183,1203,
1228,1249,
1276,1297,
1322,1343,
1368,1377,
1391,1397,
1403,1408,
1413,1425,
1434,1446,
1461,1464,
1467,1471,
1476,1492,
1512,1520,
1528,1546,
1564,1584,
1604,1640,
1678,1716,
1756,1776,
1791,1802,
1811,1832,
1856,1880,
1914,1951,
1996,1998,
2000,2027,
2063,2072,
2088,2097,
2112,2129,
2148,2161,
2175,2182,
2187,2200,
2214,2236,
2270,2275,
2280,2284,
2289,2311,
2342,2345,
2349,2378,
2422,2453,
2491,2537,
2585,2608,
2635,2669,
2702,2736,
2777,2832,
2890,2927,
2974,2988,
3009,3025,
3051,3058,
3065,3071,
3078,3103,
3129,3132,
3135,3137,
3140,3167,
3202,3224,
3249,3280,
3315,3344,
3383,3412,
3442,3467,
3497,3524,
3557,3603,
3658,3699,
3738,3763,
3802,3829,
3867,3873,
3879,3882,
3885,3920,
3960,3965,
3971,3975,
3982,4004,
4035,4076,
4121,4162,
4205,4246,
4288,4295,
4300,4339,
4379,4397,
4420,4481,
4540,4564,
4590,4615,
4642,4646,
4650,4655,
4661,4677,
4691,4708,
4723,4728,
4736,4759,
4785,4815,
4853,4884,
4923,4930,
4938,4952,
4967,4975,
4983,4990,
4996,5001,
5005,5019,
5035,5046,
5058,5069,
5081,5098,
5115,5129,
5142,5156,
5169,5186,
5202,5216,
5228,5242,
5254,5270,
5285,5298,
5309,5322,
5333,5348,
5363,5375,
5386,5398,
5409,5426,
5443,5457,
5470,5484,
5497,5513,
5528,5546,
5565,5581,
5598,5614,
5629,5648,
5668,5687,
5705,5722,
5738,5757,
5777,5796,
5813,5830,
5846,5865,
5884,5902,
5918,5935,
5951,5969,
5988,6005,
6021,6027,
6036,6045,
6055,6064,
6073,6081,
6089,6096,
6104,6107,
6110,6112,
6114,6122,
6135,6160,
6188,6193,
6199,6204,
6210,6245,
6286,6319,
6359,6392,
6428,6476,
6529,6559,
6591,6632,
6685,6711,
6741,6765,
6789,6819,
6853,6901,
6954,6985,
7021,7072,
7122,7145,
7172,7202,
7231,7255,
7284,7309,
7340,7380,
7423,7442,
7468,7491,
7521,7539,
7569,7618,
7675,7701,
7731,7746,
7762,7773,
7789,7804,
7820,7836,
7853,7871,
7890,7903,
7924,7950,
7984,8003,
8025,8059,
8101,8116,
8136,8148,
8161,8173,
8187,8201,
8217,8233,
8250,8262,
8276,8289,
8304,8318,
8332,8347,
8362,8378,
8396,8408,
8422,8434,
8447,8456,
8465,8481,
8498,8509,
8520,8536,
8553,8564,
8575,8595,
8616,8629,
8642,8658,
8675,8686,
8697,8717,
8738,8751,
8764,8784,
8805,8818,
8831,8855,
8880,8895,
8910,8923,
8936,8955,
8976,8998,
9020,9042,
9063,9084,
9104,9124,
9144,9166,
9188,9216,
9246,9277,
9308,9340,
9371,9402,
9432,9462,
9492,9497,
9501,9512,
9524,9538,
9551,9565,
9577,9590,
9601,9613,
9624,9638,
9651,9667,
9684,9703,
9721,9740,
9757,9775,
9791,9808,
9824,9845,
9869,9876,
9884,9894,
9904,9914,
9924,9940,
9957,9973,
9990,10010,
10035,10056,
10081,10103,
10131,10144,
10157,10171,
10186,10198,
10210,10227,
10248,10269,
10302,10320,
10341,10372,
10406,10424,
10445,10463,
10484,10502,
10523,10541,
10562,10580,
10601,10619,
10640,10658,
10679,10697,
10718,10737,
10759,10778,
10800,10819,
10841,10845,
10849,10853,
10857,10860,
10863,10867,
10871,10875,
10879,10882,
10885,10888,
10891,10894,
10897,10899,
10901,10904,
10907,10911,
10916,10920,
10925,10928,
10932,10935,
10938,10941,
10944,10946,
10948,10952,
10956,10975,
10998,11045,
11099,11121,
11144,11163,
11185,11237,
11294,11346,
11403,11469,
11547,11615,
11695,11763,
11844,11912,
11993,12059,
12137,12205,
12285,12353,
12434,12502,
12583,12650,
12729,12798,
12879,12948,
13030,13099,
13181,13248,
13327,13396,
13477,13546,
13628,13697,
13779,13845,
13923,13991,
14071,14139,
14220,14288,
14369,14380,
14391,14411,
14431,14450,
14469,14489,
14509,14526,
14543,14563,
14584,14602,
14620,14651,
14690,14710,
14733,14754,
14778,14809,
14844,14882,
14919,14956,
15000,15038,
15083,15103,
15132,15148,
15170,15254,
15353,15367,
15389,15404,
15427,15438,
15451,15463,
15477,15480,
15483,15486,
15489,15492,
15495,15498,
15501,15504,
15507,15510,
15513,15516,
15519,15522,
15525,15528,
15531,15534,
15537,15540,
15543,15546,
15549,15552,
15555,15558,
15561,15564,
15567,15570,
15573,15576,
15579,15582,
15585,15588,
15591,15605,
15627,15653,
15679,15705,
15731,15751,
15771,15791,
15816,15841,
15874,15883,
15892,15901,
15910,15931,
15957,15980,
16006,16025,
16048,16071,
16104,16115,
16130,16140,
16157,16169,
16184,16199,
16213,16216,
16219,16221,
16224,16248,
16275,16300,
16328,16353,
16382,16408,
16438,16445,
16455,16483,
16513,16531,
16557,16562,
16566,16571,
16575,16578,
16581,16584,
16587,16598,
16609,16619,
16633,16655,
16683,16707,
16741,16763,
16795,16814,
16837,16854,
16877,16886,
16896,16913,
16935,16941,
16949,16953,
16958,16964,
16973,16977,
16983,16989,
16995,17003,
17011,17021,
17035,17046,
17062,17096,
17139,17151,
17163,17181,
17204,17222,
17245,17269,
17296,17320,
17347,17360,
17383,17391,
17401,17411,
17425,17442,
17457,17476,
17500,17520,
17545,17564,
17588,17611,
17643,17668,
17701,17714,
17727,17766,
17816,17820,
17825,17832,
17839,17844,
17850,17861,
17876,17886,
17899,17915,
17932,17937,
17938,17949,
17965,17974,
17983,17989,
17995,17999,
18003,18011,
18021,18030,
18042,18058,
18076,18100,
18132,18149,
18172,18198,
18230,18234,
18239,18255,
18276,18321,
18370,18388,
18408,18418,
18432,18443,
18454,18473,
18493,18506,
18524,18537,
18554,18563,
18576,18605,
18640,18660,
18683,18698,
18715,18730,
18747,18764,
18783,18796,
18814,18824,
18836,18842,
18849,18858,
18867,18878,
18889,18900,
18911,18920,
18928,18936,
18942,18950,
18956,18967,
18976,18989,
19002,19016,
19036,19046,
19053,19075,
19106,19126,
19149,19166,
19187,19198,
19210,19211,
19212,19213,
19214,19215,
19216,19217,
19218,19220,
19222,19231,
19239,19248,
19258,19265,
19271,19282,
19293,19307,
19330,19341,
19352,19363,
19374,19388,
19407,19414,
19422,19431,
19443,19460,
19477,19494,
19512,19529,
19546,19561,
19580,19588,
19595,19606,
19617,19630,
19645,19659,
19674,19682,
19691,19700,
19709,19711,
19713,19722,
19732,19746,
19764,19769,
19774,19777,
19780,19784,
19788,19790,
19792,19796,
19800,19808,
19815,19825,
19839,19847,
19855,19863,
19866,19877,
19894,19903,
19913,19919,
19926,19930,
19937,19955,
19985,19993,
20003,20011,
20021,20025,
20030,20039,
20049,20054,
20060,20076,
20098,20127,
20162,20179,
20203,20226,
20256,20262,
20271,20275,
20280,20284,
20286,20290,
20296,20300,
20314,20319,
20324,20329,
20334,20339,
20343,20348,
20355,20360,
20366,20372,
20379,20386,
20393,20418,
20452,20471,
20500,20512,
20524,20541,
20558,20577,
20592,20607,
20623,20632,
20641,20652,
20658,20685,
20711,20728,
20743,20750,
20758,20762,
20768,20798,
20830,20860,
20890,20925,
20965,20969,
20973,20986,
21000,21010,
21022,21037,
21057,21069,
21083,21088,
21093,21099,
21106,21119,
21136,21142,
21151,21183,
21215,21270,
21326,21339,
21352,21360,
21368,21376,
21384,21393,
21402,21416,
21436,21456,
21483,21504,
21533,21583,
21644,21660,
21680,21696,
21714,21732,
21756,21774,
21793,21818,
21846,21867,
21889,21928,
21971,21997,
22030,22042,
22063,22088,
22127,22137,
22150,22161,
22175,22182,
22190,22216,
22249,22258,
22267,22272,
22280,22299,
22318,22327,
22332,22342,
22349,22354,
22364,22374,
22384,22388,
22399,22429,
22460,22478,
22496,22502,
22508,22539,
22572,22603,
22636,22665,
22696,22714,
22732,22738,
22746,22756,
22772,22780,
22788,22810,
22842,22864,
22897,22919,
22952,22980,
23012,23036,
23067,23082,
23099,23115,
23134,23152,
23172,23219,
23263,23268,
23274,23282,
23291,23331,
23378,23418,
23454,23480,
23510,23527,
23549,23564,
23584,23590,
23600,23604,
23608,23634,
23664,23725,
23786,23812,
23833,23851,
23876,23891,
23908,23918,
23929,23940,
23952,23966,
23981,23997,
24014,24026,
24039,24049,
24063,24070,
24081,24089,
24101,24114,
24131,24142,
24157,24187,
24215,24224,
24242,24266,
24292,24316,
24345,24394,
24447,24474,
24502,24530,
24559,24576,
24590,24600,
24610,24646,
24683,24720,
24758,24795,
24830,24841,
24859,24867,
24885,24909,
24938,24960,
24990,24997,
25003,25026,
25050,25071,
25094,25117,
25141,25163,
25192,25199,
25208,25233,
25264,25275,
25288,25298,
25313,25331,
25350,25367,
25384,25391,
25399,25406,
25412,25420,
25435,25459,
25490,25507,
25524,25538,
25555,25565,
25575,25584,
25593,25648,
25716,25735,
25754,25769,
25785,25798,
25813,25831,
25846,25862,
25876,25885,
25894,25913,
25935,25952,
25967,25987,
26001,26018,
26033,26053,
26067,26082,
26099,26117,
26133,26148,
26165,26183,
26199,26216,
26231,26251,
26265,26280,
26297,26315,
26331,26349,
26368,26393,
26417,26442,
26466,26479,
26493,26521,
26548,26575,
26597,26617,
26641,26670,
26699,26714,
26730,26737,
26746,26776,
26806,26836,
26866,26896,
26926,26951,
26975,27000,
27024,27033,
27045,27070,
27098,27123,
27148,27173,
27198,27223,
27249,27257,
27266,27277,
27287,27301,
27314,27339,
27364,27389,
27414,27439,
27464,27478,
27492,27506,
27520,27539,
27558,27577,
27596,27608,
27620,27639,
27658,27683,
27708,27734,
27760,27785,
27812,27838,
27864,27890,
27916,27944,
27972,27992,
28012,28038,
28064,28081,
28106,28124,
28153,28174,
28206,28228,
28254,28265,
28283,28294,
28307,28320,
28337,28350,
28362,28377,
28396,28411,
28425,28443,
28464,28478,
28492,28510,
28528,28546,
28563,28569,
28576,28582,
28589,28596,
28605,28611,
28620,28626,
28634,28641,
28651,28664,
28677,28700,
28726,28733,
28739,28749,
28760,28783,
28809,28824,
28840,28856,
28873,28888,
28905,28929,
28956,28973,
28991,29006,
29025,29045,
29070,29092,
29116,29140,
29175,29189,
29208,29224,
29241,29258,
29279,29306,
29334,29349,
29368,29390,
29413,29424,
29442,29482,
29531,29548,
29565,29584,
29616,29637,
29662,29681,
29715,29734,
29765,29775,
29783,29788,
29793,29812,
29843,29859,
29873,29887,
29906,29922,
29937,29952,
29974,30040,
30103,30118,
30139,30152,
30169,30182,
30203,30218,
30238,30250,
30267,30286,
30310,30339,
30374,30380,
30393,30441,
30494,30514,
30544,30573,
30606,30621,
30646,30650,
30662,30681,
30702,30708,
30717,30730,
30744,30825,
30921,30950,
30988,30992,
30996,31011,
31026,31039,
31056,31074,
31101,31125,
31148,31178,
31210,31238,
31270,31291,
31320,31336,
31354,31366,
31379,31391,
31404,31409,
31414,31421,
31428,31448,
31468,31494,
31521,31527,
31533,31559,
31592,31620,
31654,31660,
31666,31670,
31677,31690,
31702,31758,
31820,31855,
31889,31900,
31913,31937,
31955,31977,
32004,32025,
32052,32073,
32097,32109,
32122,32135,
32150,32160,
32170,32185,
32200,32210,
32220,32235,
32250,32263,
32276,32290,
32308,32327,
32350,32364,
32377,32396,
32414,32455,
32500,32542,
32593,32606,
32624,32643,
32666,32690,
32716,32726,
32736,32746,
32756,32766,
32776,32786,
32796,32810,
32828,32842,
32860,32874,
32887,32901,
32914,32933,
32952,32971,
32990,33006,
33024,33041,
33059,33068,
33079,33089,
33102,33106,
33111,33134,
33159,33183,
33211,33227,
33249,33268,
33292,33315,
33343,33357,
33377,33394,
33415,33428,
33450,33467,
33491,33520,
33558,33583,
33622,33651,
33685,33709,
33737,33775,
33813,33833
};

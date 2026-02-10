/*********************************************************************
  MODULE:		DISPLAY Message

  FILE:			DisplayMessage.c

  DATE:			01/22/07
                01/17/12  added display_amulet_message

  ANALYSIS:     display a message

    *************************************************************************/

#include "crc.h"
#include "screen.h"
#include "message.h"
#include <stdio.h>
#include <string.h>

extern CURRENT current;
extern const char lang_str[];
extern const LANG_MAP lang_map[];

//amulet messages
/*static const AMULET_MESSAGE amulet_messages[] =
	{
		L_REVI,
		"Rev ",
		"Ver ",
		
		L_PR_REV,
		"Rev:",
		"Ver:",

		L_REV_SN,
		"REV       SN: ",
		"VER       SN: ",

		L_COPYRIGHT,
		"COPYRIGHT",
		"Derecho de Autor",

		L_RIGHTS,
		"ALL RIGHTS RESERVED",
		"DERECHOS RESERVADOS",

		L_CONTINUE_BTN,
		"Continue",
		"Continuar",

		L_LOGIN,
		"Login",
		"Login",

		L_UPDATE_SYSTEM,
		"Update System",
		"Actualización del Sistema",

		L_TIMEOUT_OFF,
		"OFF",
		"NO",

		L_BRIGHTNESS,
		"Brightness:",
		"Luminosidad:",

		L_SLEEP_TIMEOUT,
		"Sleep Timeout:",
		"Tiempo de Espera:",

		L_SLEEP_BRIGHTNESS,
		"Sleep Brightness:",
		"Brillo en Espera:",

		L_VOLUME,
		"Volume:",
		"Volumen:",

		L_SETUP_TITLE,
		"Setup",
		"Configurar",

		L_ADVANCED_CHAMBER,
		"Advanced Chamber",
		"Cámara Avanzada",

		L_ADVANCED_DETECTOR,
		"Advanced Detector",
		"Detector Avanzado",

		L_STAFF,
		"Staff",
		"Personal",

		L_ENTER_PASSWORD,
		"Please Enter Password:",
		"Favor de Introducir Contraseña:",

		L_TEST,
		"Test",
		"Prueba",

		L_TEST2,
		"Test:",
		"Prueba:",

		L_ACTIVITY_UNIT,
		"Activity Unit:",
		"Unidad de Act:",

		L_SETUP_DATE_FORMAT,
		"Date Format:",
		"Formato de Fecha:",
		
		L_SETUP_PRINTER,
		"Printer:",
		"Impresa:",

		L_SETUP_SOURCES,
		"Setup Sources",
		"Configurar Fuentes",	//Changed

		L_SETUP_MOLY,
		"Setup Moly",
		"Configurar Moly",	//Changed

		L_SETUP_NUCLIDES,
		"Setup Nuclide",
		"Configurar Nucleido",		//Changed

		L_SETUP_CALNUM,
		"Setup CalNum",
		"Configurar CalNum",		//Changed

		L_SETUP_LINEARITY,
		"Setup Linearity",
		"Configurar Linealidad",	//Changed

		L_SETUP_REMOTE,
		"Setup Remote",
		"Configurar Remota",	//Changed

		L_SETUP_R_HOTKEYS,
		"Setup R Chamber Hotkeys",
		"Config. Tecla Rapida Cámara R",

		L_SETUP_PET_HOTKEYS,
		"Setup PET Chamber Hotkeys",
		"Config. Tecla Rapida Cámara PET",

		L_ADVANCED_CHAMBER_SETUP_TITLE,
		"Advanced Chamber Setup",
		"Configurar Cámara Avanzada",	//Changed

		L_SETUP_SOURCES_ACTIVITY,
		"ACTIVITY",
		"ACTIVIDAD",

		L_SETUP_SOURCES_DATE,
		"DATE",
		"FECHA",

		L_SETUP_SOURCES_DAILY,
		"DAILY",
		"DIARIA",		//Changed

		L_SETUP_SOURCES_CONSTANCY_CHANNELS,
		"Constancy Channels",
		"Canales de Estabilidad",		//Changed

		L_CLEAR_BUTTON,
		"Clear",
		"Borrar",

		L_ACCEPT_BUTTON,
		"Accept",
		"Aceptar",			//Changed

		L_CANCEL_BUTTON,
		"Cancel",
		"Cancelar",

		L_SETUP_MOLY_METHOD,
		"Moly Method:",
		"Método de Moly:",

		L_SETUP_MOLY_LIMIT,
		"Mo/Tc Limit:",
		"Mo/Tc Límite:",

		L_NUCLIDE,
		"Nuclide",
		"Nucleido",

		L_SETUP_NUCLIDE_ELEMENT,
		"Element",
		"Elemento",

		L_SETUP_NUCLIDE_HALFLIFE,
		"Halflife",
		"Vida media",
		
		L_ENGLISH,
		"English",
		"Inglés",

		L_FRENCH,
		"French",
		"Español",
		
		L_LANGUAGE,
		"Language:",
		"Idioma:",

		L_DEFAULT,
		"Default",
		"Predeterminado",

		L_USER,
		"User",
		"Usuario",

		L_ENTER_NUCLIDE,
		"Please Enter Nuclide",
		"Favor de Introducir Nucleido",

		L_ENTER_ELEMENT,
		"Please Enter Element",
		"Favor de Introducir Elemento",

		L_ENTER_HALFLIFE,
		"Please Enter Halflife",
		"Favor de Introducir Vida media",

		L_ENTER_RCAL,
		"Please Enter R Cal #:",
		"Favor de Introducir R Cal #:",
		
		L_ENTER_PCAL,
		"Please Enter P Cal #:",
		"Favor de Introducir P Cal #:",
		
		L_BACKSPACE,
		"Backspace",
		"Retroceder",

		L_BACKSPACE_SHORT,
		"BackSP",
		"Regreso",

		L_SHIFT,
		"Shift",
		"Shift",

		L_WILDCARD,
		"<-- Wildcard",
		"<-- Comodín",

		L_PLEASE_ENTER,
		"Please Enter",
		"Favor de Introducir",

		L_SN,
		"S/N",
		"S/N",

		L_SN2,
		"S/N:",
		"S/N:",

		L_CALIBRATION_TIME,
		"Calibration Time",
		"Tiempo de Calibración",

		L_ACTIVITY,
		"Activity",
		"Actividad",

		L_STANDARD_R_CHAMBER,
		"Standard R Chamber",
		"Estándar cámara R",

		L_STANDARD_PET_CHAMBER,
		"Standard PET Chamber",
		"Estándar cámara PET",

		L_SETUP_LINEARITY_STANDARD_R,
		"Setup Linearity Standard - R Chamber",
		"Configura el estándar de linealidad - cámara R",

		L_SETUP_LINEARITY_STANDARD_PET,
		"Setup Linearity Standard - PET Chamber",
		"Configura el estándar de linealidad - cámara PET",

		L_NUM_OF_MEASUREMENTS,
		"Num of Measurements:",
		"Numero de medidas:",

		L_MEASURED_ON,
		"Measured On",
		"Medido En",

		L_SETUP_LINEARITY_ERROR,
		"Setup Linearity Error",
		"Error en Config. Linealidad",

		L_FILL_IN_HOURS,
		"Please Fill in All Hours",
		"Por favor llene todas las horas",

		L_ASCENDING_ORDER,
		"Please enter hours in ascending order",
		"Por favor, introduzca horas en orden ascendente",
		
		L_CAPS_OK,
		"OK",
		"BIEN",

		L_SELECT_WIPE_CRITERIA,
		"Please select Wipe Criteria",
		"Please select Wipe Criteria",

		L_ALL_WIPES,
		"All Wipes",
		"All Wipes",

		L_WORK_AREA,
		"Work Area",
		"Work Area",

		L_UNRESTRICTED_AREA,
		"Unrestricted Area",
		"Unrestricted Area",

		L_SEALED_SOURCE,
		"Sealed Source",
		"Sealed Source",

		L_PACKAGE,
		"Package",
		"Package",

		L_HIGH_ACTIVITY,
		"High Activity",
		"High Activity",

		L_SELECT_DETECTOR,
		"Please select Detector",
		"Please select Detector",

		L_PROBE,
		"Probe",
		"Probe",

		L_WELL,
		"Well",
		"Well",

		L_SELECT_BRANDING,
		"Please select Branding",
		"Please select Branding",

		L_ALL,
		"All",
		"All",

		L_SELECT_TU_METHOD,
		"Please select Counting Method",
		"Please select Counting Method",

		L_DECAY_CORRECT_ADMIN_DOSE,
		"Decay Correct Administered Dose",
		"Decay Correct Administered Dose",

		L_MEAS_REF_DOSE,
		"Measure Same Reference Dose Before Each Uptake",
		"Measure Same Reference Dose Before Each Uptake",

		L_SELECT_DOSE_MEASUREMENT,
		"Select Dose Measurement",
		"Select Dose Measurement",

		L_DOSE_MEAS_IS_DOSE_ADMIN,
		"Dose Measured is Dose Administered",
		"Dose Measured is Dose Administered",

		L_MEAS_DOSE_AND_ADD,
		"Measure Each Dose and Add Activity",
		"Measure Each Dose and Add Activity",

		L_MEAS_DOSE_AND_MULTIP,
		"Measure One Capsule and Multiply by Number Administered",
		"Measure One Capsule and Multiply by Number Administered",

		L_MEAS_LIQ_AND_MULTIP,
		"Measure Liquid and Multiply by Factor",
		"Measure Liquid and Multiply by Factor",

		L_SELECT_NUCLIDE,
		"Select Nuclide",
		"Select Nuclide",

		L_SELECT_DOSE_FORM,
		"Select Dose Form",
		"Select Dose Form",

		L_CAPSULE,
		"Capsule",
		"Capsule",

		L_LIQUID,
		"Liquid",
		"Liquid",

		L_MEAS_PREDOSE_PATIENT,
		"Measure Pre-Dose Patient?",
		"Measure Pre-Dose Patient?",

		L_YES,
		"Yes",
		"Sí",

		L_NO,
		"No",
		"No",

		L_DELETE_THYROID,
		"Delete Thyroid Uptake Test?",
		"Delete Thyroid Uptake Test?",

		L_MEAS_REDISUAL,
		"Measure Residual Dose?",
		"Measure Residual Dose?",

		L_REACTIVATE_TU,
		"ReActivate Thyroid Uptake Test?",
		"ReActivate Thyroid Uptake Test?",

		L_ONLY_DEFAULT_NUC,
		"Use Only Default Nuclide keV?",
		"Use Only Default Nuclide keV?",

		L_REACTIVATE_RBC,
		"ReActivate RBC Survival Test?",
		"ReActivate RBC Survival Test?",

		L_DELETE_RBC,
		"Delete RBC Survival Test?",
		"Delete RBC Survival Test?",

		L_UPDATE_TO_REV,
		"Update to Rev: %s, Proceed?",
		"Actualizar a Rev: %s, ¿Procede?",

		L_UPDATE_DISABLES_USB,
		"This version will disable USB Update. Proceed?",
		"Está versión desactiva Actualización USB. ¿Procede?",

		L_OVERWRITE_SETTING_WITH_DEFAULT,
		"Overwrite settings with defaults from %s?",
		"Overwrite settings with defaults from %s?",

		L_SELECT_DATE_FORMAT,
		"Please Select Date Format",
		"Favor de Seleccionar el Formato de Fecha",

		L_SELECT_USB_DRIVER,
		"Please Select USB PC Driver",
		"Favor de Seleccionar el Controlador USB PC",

		L_LEGACY,
		"Legacy",
		"Legado",

		L_CDC,
		"CDC",
		"CDC",

		L_SELECT_DOSE_DECAY_ENTRY_MODE,
		"Please Select Dose Decay Entry Mode",
		"Seleccione Modo de Introducir Decaimiento de Dosis",	//Changed

		L_QUICK,
		"Quick",
		"Rápido",

		L_FULL,
		"Full",
		"Lleno",

		L_SELECT_LANGUAGE,
		"Please Select Language",
		"Favor de Seleccionar el Idioma",

		L_55T_FULL_TITLE,
		"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",
		"CRC-55t      CALIBRADOR DE DOSIS DE RADIOISÓTOPOS",

		L_CAPRAC_FULL_TITLE,
		"CAPRAC-t        RADIOISOTOPE WELL COUNTER",
		"CAPRAC-t        RADIOISOTOPE WELL COUNTER",

		L_700T_FULL_TITLE,
		"CAPTUS-700t          RADIOISOTOPE COUNTER",
		"CAPTUS-700t          RADIOISOTOPE COUNTER",

		L_FACTORY,
		"Factory",
		"Factory",

		L_PLEASE_RESTART,
		"Setting has been saved. Please restart.",
		"Setting has been saved. Please restart.",

		L_WIPE_TYPE_DEFAULTS,
		"Wipe Type Defaults",
		"Wipe Type Defaults",

		L_WIPE_OVERWRITE_STRING,
		"Current settings have been overwritten with %s default values",
		"Current settings have been overwritten with %s default values",

		L_SETUP_LINEARITY_LINEATOR,
		"Setup Linearity Lineator",
		"Configura la linealidad de Lineator",

		L_SETUP_LINEARITY_CALICHECK,
		"Setup Linearity Calicheck",
		"Configura la linealidad de Calicheck",

		L_TUBE,
		"Tube",
		"Tubo",

		L_TUBE2,
		"Tube:",
		"Tubo:",

		L_LINEATOR_SERIAL,
		"Lineator Serial:",
		"Lineator S/N:",

		L_CALICHECK_SERIAL,
		"Calicheck Serial:",
		"Calicheck S/N:",

		L_PRINT,
		"Print",
		"Imprime",

		L_DELETE_CURRENT_SETTINGS,
		"Delete Current Settings",
		"Borrar Config. Actual",

		L_PLEASE_ENTER_LINEATOR_SN,
		"Please Enter Lineator Serial #",
		"Favor de Introducir el # de Serie de Lineator",	//Changed
		
		L_PLEASE_ENTER_CALICHECK_SN,
		"Please Enter Calicheck Serial #",
		"Favor de Introducir el # de Serie del Calicheck",	//Changed

		L_PR_NUCLIDE,
		"NUCLIDE",
		"NUCLEIDO",		//Changed

		L_PR_CHAMBER_SN,
		"Chamber   S/N:",
		"Cámara    S/N:",

		L_CAL_1,
		"1)Black:",
		"1)Negro:",

		L_CAL1,
		"1)Black",
		"1)Negro",

		L_CAL__1,
		"Black",
		"Negro",

		L_CAL_2,
		"2)Black + Red:",
		"2)Negro + Rojo:",

		L_CAL2,
		"2)Black/Red",
		"2)Negro/Rojo",

		L_CAL__2,
		"Black + Red",
		"Negro + Rojo",

		L_CAL_3,
		"3)Black + Orange:",
		"3)Negro + Naranja:",

		L_CAL3,
		"3)Black/Orange"
		"3)Negro/Naranja"

		L_CAL__3,
		"Black + Orange"
		"Negro + Naranja"

		L_CAL_4,
		"4)Black + Yellow:",
		"4)Negro + Amarillo:",

		L_CAL4,
		"4)Black/Yellow",
		"4)Negro/Amarillo",

		L_CAL__4,
		"Black + Yellow",
		"Negro + Amarillo",

		L_CAL_5,
		"5)Black + Green:",
		"5)Negro + Verde:",

		L_CAL5,
		"5)Black/Green",
		"5)Negro/Verde",

		L_CAL__5,
		"Black + Green",
		"Negro + Verde",

		L_CAL_6,
		"6)Black + Blue:",
		"6)Negro + Azul:",

		L_CAL6,
		"6)Black/Blue",
		"6)Negro/Azul",

		L_CAL__6,
		"Black + Blue",
		"Negro + Azul",

		L_CAL_7,
		"7)Black + Purple:",
		"7)Negro + Púrpura:",

		L_CAL7,
		"7)Black/Purple",
		"7)Negro/Púrpura",

		L_CAL__7,
		"Black + Purple",
		"Negro + Púrpura",

		L_CAL_8,
		"8)Black + Purple",
		"8)Negro + Púrpura",

		L_CAL_82,
		"+ Red:",
		"+ Roja:",

		L_CAL8,
		"8)Black/Purple/Red",
		"8)Negro/Púrpura/Roja",

		L_CAL__8,
		"Black/Purple/Red",
		"Negro/Púrpura/Roja",

		L_CAL_9,
		"9)Black + Purple",
		"9)Negro + Púrpura",

		L_CAL_92,
		"+ Orange:",
		"+ Naranja:",
		
		L_CAL9,
		"9)Blk/Purple/Orange",
		"9)Negro/PUR/Naranja",

		L_CAL__9,
		"Black/Purple/Orange",
		"Negro/Púrpura/Naranja",

		L_CAL_10,
		"10)Black + Purple",
		"10)Negro + Púrpura",

		L_CAL_102,
		"+ Yellow:",
		"+ Amarillo:",		//Changed

		L_CAL10,
		"10)Blk/Purpl/Yellow",
		"10)Negro/PUR/Amarillo",	//Changed

		L_CAL__10,
		"Black/Purple/Yellow",
		"Negro/Púrpura/Amarillo",	//Changed

		L_CAL_11,
		"11)Black + Purple",
		"11)Negro + Púrpura",

		L_CAL_11_2,
		"+ Green:",
		"+ Verde:",

		L_CAL11,
		"11)Blk/Purple/Green",
		"11)Negro/PUR/Verde",

		L_CAL__11,
		"Black/Purple/Green",
		"Negro/Púrpura/Verde",

		L_CAL_12,
		"12)Black + Purple",
		"12)Negro + Púrpura",
		
		L_CAL_12_2,
		"+ Blue:",
		"+ Azul:",
		
		L_CAL12,
		"12)Blk/Purple/Blue",
		"12)Negro/Púrpura/Azul",

		L_CAL__12,
		"Black/Purple/Blue",
		"Negro/Púrpura/Azul",

		L_YES_ENGLISH,
		"Yes",
		"Yes",

		L_NO_ENGLISH,
		"No",
		"No",

		L_COMMENT,
		"Comment:",
		"Comentario:",

		L_RESUME_AUTOLINEARITY_TEST,
		"Resume AutoLinearity Test",
		"Reanudar la Prueba AutoLinealidad",

		L_ERROR,
		"ERROR",
		"ERROR",

		L_ERROR2,
		"Error",
		"Error",

		L_UNABLE_TO_FIND_CHAMBER_WITH_SN,
		"Unable to find chamber with S/N: %s",
		"No se puede encontrar la cámara con S/N: %s",

		L_ABORT_CURRENT_AUTOLINEARITY_TEST,
		"Abort current AutoLinearity Test?",
		"¿Abortar prueba actual de AutoLinealidad?",

		L_ABORT_UNABLE_TO_FIND_USABLE_NUCLIDE,
		"Unable to find usable Nuclide: %s",
		"No se puede encontrar útil Nucleido: %s",

		L_COMPLETED_AUTOLINEARITY_NOT_SAVED,
		"Completed AutoLinearity Test has not been saved.",
		"Prueba Completa AutoLinealidad no se ha guardado.",

		L_PROCEED_TO_AUTOLINEARITY,
		"Proceed to AutoLinearity Test?",
		"¿Proceda a la prueba AutoLinealidad?",

		L_PLEASE_PLACE_NUCLIDE,
		"Please place Nuclide: %s into Chamber: %d",
		"Favor de Colocar el Nucleido: %s en la Cámara: %d",

		L_RESUME_AUTOLINEARITY,
		"Resume AutoLinearity Test?",
		"¿Reanudar la Prueba AutoLinealidad?",

		L_PAUSE_AUTOLINEARITY_TEST,
		"Pause AutoLinearity Test",
		"Pausa prueba AutoLinealidad",

		L_AUTOLINEARITY_HAS_BEEN_PAUSED,
		"AutoLinearity has been paused.",
		"AutoLinealidad se ha pausado.",

		L_COMPLETED_AUTOLINEARITY_HAS_NOT_BEEN_SAVED,
		"Completed AutoLinearity Test has not been saved.",
		"Prueba Completa AutoLinealidad no se ha guardado.",

		L_TO_AVOID_MISSING_MEASUREMENTS,
		"To avoid missing measurements, ",
		"Para evitar medidas perdidas, "

		L_PLEASE_RETURN_TO_AUTOLINEARITY,
		"Please return to AutoLinearity as soon as possible.",
		"Regrese a AutoLinealidad lo mas pronto posible.",

		L_PROCEED_TO_HOME_SCREEN,
		"Proceed to Home Screen?",
		"¿Proceda a la pantalla de inicio?",

		L_PROCEED_TO_AUTOLINEARITY_MENU,
		"Proceed to AutoLinearity Menu?",
		"¿Proceda al menú AutoLinealidad?",

		L_ABORT_AUTOLINEARITY_TEST,
		"Abort AutoLinearity Test",
		"Abortar Prueba AutoLinealidad",

		L_ABORT_AUTOLINEARITY_TEST2,
		"Abort AutoLinearity Test?",
		"¿Abortar Prueba AutoLinealidad?",

		L_CURRENT_AUTOLINEARITY_TEST_HAS_FINISHED,
		"Current AutoLinearity Test has finished.",
		"Prueba AutoLinealidad actual ha terminado.",

		L_ERASE_CURRENT_TEST,
		"Erase Current Test?",
		"¿Borrar Prueba Actual?",

		L_SAVE_AUTOLINEARITY_TEST,
		"Save AutoLinearity Test",
		"Guardar Prueba AutoLinealidad",

		L_SAVE_CURRENT_TEST,
		"Save Current Test?",
		"¿Guardar Prueba Actual?",

		L_CURRENT_AUTOLINEARITY_IS_PARTIALLY_COMPLETE,
		"Current AutoLinearity Test is partially complete.",
		"Prueba AutoLinealidad actual es parcialmente completa.",

		L_END_AND_SAVE_TEST,
		"End and Save Current Test?",
		"¿Final y Guardar Prueba Actual?",

		L_MAXIMUM_IS,
		"Maximum is %.4f",
		"Máxima es %.4f",

		L_INPUT_ERROR,
		"Input Error",
		"Error de Introducción",

		L_MINIMUM_IS,
		"Minimum is %.4f",
		"Mínimo es %.4f",

		L_CAL_NUMBER_ERROR,
		"Cal Number Error",
		"Error del Factor Cal.",

		L_INVALID_CAL_NUMBER,
		"Invalid Cal Number",
		"Inválido Número de Calibración",

		L_USER_CAL_FULL,
		"User Cal Full",
		"Calibración de Usuario Completa",

		L_PLEASE_ENTER_CURRENT_TIME,
		"Please Enter Current Time:",
		"Favor de introducir hora actual:",		// Changed

		L_SERIAL_NUMBER_ERROR,
		"Serial Number Error",
		"Error del Número de Serie",

		L_SERIAL_NUMBER_MUST_HAVE_SIX_DIGITS,
		"Serial number must have six digits",
		"Número de serie debe ser de 6 dígitos",

		L_DOSE_TIME_ERROR,
		"Dose Time Error",
		"Error del Tiempo de la Dosis",

		L_INVALID_DATE,
		"Invalid Date",
		"Fecha Invalida",

		L_INVALID_TIME,
		"Invalid Time",
		"Tiempo Invalido",

		L_MAX_HOUR_IS_23,
		"Max Hour is 23",
		"Hora Max es 23",

		L_MAX_MINUTE_IS_59,
		"Max Minute is 59",
		"Minuto Max es 59",

		L_MIN_DAY_IS_1,
		"Min Day is 1",
		"Día Min es 1",

		L_MAX_DAY_IS_31,
		"Max Day is 31",
		"Día Max es 31",

		L_MIN_MONTH_IS_1,
		"Min Month is 1",
		"Mes Min es 1",

		L_MAX_MONTH_IS_12,
		"Max Month is 12",
		"Mes Max es 12",

		L_MAX_YEAR_IS_2030,
		"Max Year is 2030",
		"Año Max es 2030",

		L_INVALID_YEAR,
		"Invalid Year",
		"Año Invalido",

		L_TUBE_1_LINEATOR,
		"Tube #1: (1)",
		"Tubo #1: (1)",

		L_TUBE_1_LINEATOR2,
		"1) Tube 1",
		"1) Tubo 1",

		L_TUBE_2_LINEATOR,
		"Tube #2: (1 + 2)",
		"Tubo #2: (1 + 2)",

		L_TUBE_2_LINEATOR2,
		"2) Tube 1+2",
		"2) Tubo 1+2",

		L_TUBE_3_LINEATOR,
		"Tube #3: (1 + 3)",
		"Tubo #3: (1 + 3)",

		L_TUBE_3_LINEATOR2,
		"3) Tube 1+3",
		"3) Tubo 1+3",

		L_TUBE_4_LINEATOR,
		"Tube #4: (1 + 2 + 3)",
		"Tubo #4: (1 + 2 + 3)",

		L_TUBE_4_LINEATOR2,
		"4) Tube 1+2,3",
		"4) Tubo 1+2,3",

		L_TUBE_5_LINEATOR,
		"Tube #5: (1 + 4)",
		"Tubo #5: (1 + 4)",

		L_TUBE_5_LINEATOR2,
		"5) Tube 1+4",
		"5) Tubo 1+4",

		L_TUBE_6_LINEATOR,
		"Tube #6: (1 + 2 + 4)",
		"Tubo #6: (1 + 2 + 4)",

		L_TUBE_6_LINEATOR2,
		"6) Tube 1+2,4",
		"6) Tubo 1+2,4",

		L_TUBE_7_LINEATOR,
		"Tube #7: (1 + 3 + 4)",
		"Tubo #7: (1 + 3 + 4)",

		L_TUBE_7_LINEATOR2,
		"7) Tube 1+3,4",
		"7) Tubo 1+3,4",

		L_TUBE_8_LINEATOR,
		"Tube #8: (1 + 2 + 3 + 4)",
		"Tubo #8: (1 + 2 + 3 + 4)",

		L_TUBE_8_LINEATOR2,
		"8) Tube 1+2,3,4",
		"8) Tubo 1+2,3,4",

		L_TUBE_1_CALICHECK,
		"Tube 1: Black",
		"Tubo 1: Negro",

		L_TUBE_2_CALICHECK,
		"Tube 2: Black + Red",
		"Tubo 2: Negro + Rojo",

		L_TUBE_3_CALICHECK,
		"Tube 3: Black + Orange",
		"Tubo 3: Negro + Naranja",

		L_TUBE_4_CALICHECK,
		"Tube 4: Black + Yellow",
		"Tubo 4: Negro + Amarillo",

		L_TUBE_5_CALICHECK,
		"Tube 5: Black + Green",
		"Tubo 5: Negro + Verde",

		L_TUBE_6_CALICHECK,
		"Tube 6: Black + Blue",
		"Tubo 6: Negro + Azul",

		L_TUBE_7_CALICHECK,
		"Tube 7: Black + Purple",
		"Tubo 7: Negro + Púrpura",

		L_TUBE_8_CALICHECK,
		"Tube 8: Black + Purple + Red",
		"Tubo 8: Negro + Púrpura + Rojo",

		L_TUBE_9_CALICHECK,
		"Tube 9: Black + Purple + Orange",
		"Tubo 9: Negro + Púrpura + Naranja",

		L_TUBE_10_CALICHECK,
		"Tube 10: Black + Purple + Yellow",
		"Tubo 10: Negro + Púrpura + Amarillo",

		L_TUBE_11_CALICHECK,
		"Tube 11: Black + Purple + Green",
		"Tubo 11: Negro + Púrpura + Verde",

		L_TUBE_12_CALICHECK,
		"Tube 12: Black + Purple + Blue",
		"Tubo 12: Negro + Púrpura + Azul",

		L_PR_TUBE_1_CALICHECK,
		"Black",
		"Negro",

		L_PR_TUBE_2_CALICHECK,
		"Black + Red",
		"Negro + Rojo",

		L_PR_TUBE_3_CALICHECK,
		"Black + Orange",
		"Negro + Naranja",

		L_PR_TUBE_4_CALICHECK,
		"Black + Yellow",
		"Negro + Amarillo",

		L_PR_TUBE_5_CALICHECK,
		"Black + Green",
		"Negro + Verde",

		L_PR_TUBE_6_CALICHECK,
		"Black + Blue",
		"Negro + Azul",

		L_PR_TUBE_7_CALICHECK,
		"Black + Purple",
		"Negro + Púrpura",

		L_PR_TUBE_8_CALICHECK,
		"Black/Purple/Red",
		"Negro/Púrpura/Rojo",

		L_PR_TUBE_9_CALICHECK,
		"Black/Purple/Orange",
		"Negro/Púrpura/Naranja",

		L_PR_TUBE_10_CALICHECK,
		"Black/Purple/Yellow",
		"Negro/Púrpura/Amarillo",

		L_PR_TUBE_11_CALICHECK,
		"Black/Purple/Green",
		"Negro/Púrpura/Verde",

		L_PR_TUBE_12_CALICHECK,
		"Black/Purple/Blue",
		"Negro/Púrpura/Azul",

		L_MEASURE_TEST_SOURCE,
		"Measure test source, ",
		"Medir prueba de origen, ",	//Changed

		L_MEASURE_TEST_SOURCE_SN,
		", S/N: ",
		", S/N: ",

		L_OVERRANGE,
		"OVER RANGE",
		"MUY ALTO",

		L_OVERRANGE2,
		"Over Range",
		"Muy Alto",

		L_OVERRANGE_ERROR,
		"OVER RANGE\nERROR",
		"ERROR\nMUY ALTO",

		L_OVERRANGE_ERROR2,
		"Over Range Error",
		"Error Muy Alto",		//Check

		L_CALC_DEV_ERROR,
		"Calc: %s, Dev: ERROR",
		"Calc: %s, Desv: ERROR",

		L_CALC_DEV,
		"Calc: %s, Dev: %.1f%%",
		"Calc: %s, Desv: %.1f%%",

		L_CLEAR_SELECTED_NUCLIDE,
		"Clear Selected\nNuclide",
		"Borrar Nucleido\nSeleccionado",

		L_PAGE_OF,
		"Page %d of %d",
		"Página %d de %d",

		L_USB_PC_DRIVER,
		"USB PC Driver:",
		"Controlador USB PC:",

		L_SCREEN_CALIB,
		"Screen Calib",
		"Pantalla de Calib",

		L_DOSE_DECAY_ENTRY,
		"Dose Decay Entry:",
		"Intro Dosis Decaída:",		//Changed

		L_PLEASE_SELECT_NUCLIDE,
		"Please Select Nuclide",
		"Favor de Seleccionar el Nucleido",

		L_ENTER_MOTC_LIMIT,
		"Enter Mo/Tc Limit:",
		"Introduzca Mo/Tc Límite:",

		L_ENTER_NUM_MEASUREMENTS,
		"Please Enter # of Measurements:",
		"Introduzca # de Mediciones:",

		L_ENTER_TEST_2_TIME,
		"Enter Test 2 Time:",
		"Introduzca Tiempo de la Prueba 2:",
		
		L_ENTER_TEST_3_TIME,
		"Enter Test 3 Time:",
		"Introduzca Tiempo de la Prueba 3:",

		L_ENTER_TEST_4_TIME,
		"Enter Test 4 Time:",
		"Introduzca Tiempo de la Prueba 4:",

		L_ENTER_TEST_5_TIME,
		"Enter Test 5 Time:",
		"Introduzca Tiempo de la Prueba 5:",

		L_ENTER_TEST_6_TIME,
		"Enter Test 6 Time:",
		"Introduzca Tiempo de la Prueba 6:",

		L_ENTER_TEST_7_TIME,
		"Enter Test 7 Time:",
		"Introduzca Tiempo de la Prueba 7:",

		L_ENTER_TEST_8_TIME,
		"Enter Test 8 Time:",
		"Introduzca Tiempo de la Prueba 8:",

		L_ENTER_TEST_9_TIME,
		"Enter Test 9 Time:",
		"Introduzca Tiempo de la Prueba 9:",

		L_ENTER_TEST_10_TIME,
		"Enter Test 10 Time:",
		"Introduzca Tiempo de la Prueba 10:",

		L_ENTER_TEST_11_TIME,
		"Enter Test 11 Time:",
		"Introduzca Tiempo de la Prueba 11:",

		L_ENTER_TEST_12_TIME,
		"Enter Test 12 Time:",
		"Introduzca Tiempo de la Prueba 12:",

		L_SEC,
		" Sec",
		" Seg",

		L_SEC2,
		" sec",
		" seg",

		L_SEC3,
		"sec",
		"seg",

		L_MIN,
		" Min",
		" Min",

		L_MIN2,
		" min",
		" min",

		L_MIN3,
		"min",
		"min",

		L_HR,
		" Hr",
		" H",

		L_HR2,
		" hr",
		" h",

		L_HR3,
		"hr",
		"h",

		L_HR4,
		" Hr",
		" Hora",

		L_DAY,
		" Day",
		" Día",

		L_DAY2,
		" day",
		" día",

		L_DAY3,
		"day",
		"día",

		L_YR,
		" Yr",
		" Año",

		L_YR2,
		" yr",
		" año",

		L_YR3,
		"yr",
		"año",

		L_MYR,
		" Myr",
		"Maño",

		L_SETUP_NUCLIDE_ERROR,
		"Setup Nuclide Error",
		"Error Config. del Nucleido",

		L_PLEASE_COMPLETE_PARTIAL_NUCLIDE_ENTRY,
		"Please complete partial Nuclide entry.\nRow(s): ",
		"Por favor, complete la introdución parcial del nucleido.\nFila(s): ",

		L_DUPLICATE_NAME_FOUND,
		"Duplicate Name found:\n",
		"Nombre duplicado encontrado:\n",
		
		L_SETUP_SOURCES_ERROR,
		"Setup Sources Error",
		"Error Config. de Fuentes",

		L_NO_CONSTANCY_CHANNEL,
		"Constancy Source selected without Constancy Channels",
		"Fuente de Estabilidad seleccionada sin los Canales de Estabilidad",	//Changed

		L_NO_CONSTANCY_SOURCE,
		"Constancy Channels selected without Constancy Source",
		"Canales de Estabilidad seleccionados sin Fuente de Estabilidad",	//Changed

		L_ABOVE_MAXIMUM_ACTIVITY_CO57_CI,
		"Above Maximum Activity\nCo57 Activity: %.3f Ci\nUpper Limit: %.3f Ci",
		"Encima de Actividad Máxima\nCo57 Actividad: %.3f Ci\nLímite Superior: %.3f Ci",

		L_ABOVE_MAXIMUM_ACTIVITY_CO57_BQ,
		"Above Maximum Activity\nCo57 Activity: %.3f GBq\nUpper Limit: %.3f GBq",
		"Encima de Actividad Máxima\nCo57 Actividad: %.3f GBq\nLímite Superior: %.3f GBq",

		L_BELOW_MINIMUM_ACTIVITY_CO57_CI,
		"Below Minimum Activity\nCo57 Activity: %.3f uCi\nLower Limit: %.3f uCi",
		"Debajo de Actividad Mínima\nCo57 Actividad: %.3f uCi\nLímite Inferior: %.3f uCi",
		
		L_BELOW_MINIMUM_ACTIVITY_CO57_BQ,
		"Below Minimum Activity\nCo57 Activity: %.6f MBq\nLower Limit: %.6f MBq",
		"Debajo de Actividad Mínima\nCo57 Actividad: %.6f MBq\nLímite Inferior: %.6f MBq",
		
		L_ABOVE_MAXIMUM_ACTIVITY_CO60_CI,
		"Above Maximum Activity\nCo60 Activity: %.3f Ci\nUpper Limit: %.3f Ci",
		"Encima de Actividad Máxima\nCo60 Actividad: %.3f Ci\nLímite Superior: %.3f Ci",
		
		L_ABOVE_MAXIMUM_ACTIVITY_CO60_BQ,
		"Above Maximum Activity\nCo60 Activity: %.3f GBq\nUpper Limit: %.3f GBq",
		"Encima de Actividad Máxima\nCo60 Actividad: %.3f GBq\nLímite Superior: %.3f GBq",
		
		L_BELOW_MINIMUM_ACTIVITY_CO60_CI,
		"Below Minimum Activity\nCo60 Activity: %.3f uCi\nLower Limit: %.3f uCi",
		"Debajo de Actividad Mínima\nCo60 Actividad: %.3f uCi\nLímite Inferior: %.3f uCi",
		
		L_BELOW_MINIMUM_ACTIVITY_CO60_BQ,
		"Below Minimum Activity\nCo60 Activity: %.6f MBq\nLower Limit: %.6f MBq",
		"Debajo de Actividad Mínima\nCo60 Actividad: %.6f MBq\nLímite Inferior: %.6f MBq",
		
		L_ABOVE_MAXIMUM_ACTIVITY_BA133_CI,
		"Above Maximum Activity\nBa133 Activity: %.3f Ci\nUpper Limit: %.3f Ci",
		"Encima de Actividad Máxima\nBa133 Actividad: %.3f Ci\nLímite Superior: %.3f Ci",

		L_ABOVE_MAXIMUM_ACTIVITY_BA133_BQ,
		"Above Maximum Activity\nBa133 Activity: %.3f GBq\nUpper Limit: %.3f GBq",
		"Encima de Actividad Máxima\nBa133 Actividad: %.3f GBq\nLímite Superior: %.3f GBq",
		
		L_BELOW_MINIMUM_ACTIVITY_BA133_CI,
		"Below Minimum Activity\nBa133 Activity: %.3f uCi\nLower Limit: %.3f uCi",
		"Debajo de Actividad Mínima\nBa133 Actividad: %.3f uCi\nLímite Inferior: %.3f uCi",
		
		L_BELOW_MINIMUM_ACTIVITY_BA133_BQ,
		"Below Minimum Activity\nBa133 Activity: %.6f MBq\nLower Limit: %.6f MBq",
		"Debajo de Actividad Mínima\nBa133 Actividad: %.6f MBq\nLímite Inferior: %.6f MBq",
		
		L_ABOVE_MAXIMUM_ACTIVITY_CS137_CI,
		"Above Maximum Activity\nCs137 Activity: %.3f Ci\nUpper Limit: %.3f Ci",
		"Encima de Actividad Máxima\nCs137 Actividad: %.3f Ci\nLímite Superior: %.3f Ci",
		
		L_ABOVE_MAXIMUM_ACTIVITY_CS137_BQ,
		"Above Maximum Activity\nCs137 Activity: %.3f GBq\nUpper Limit: %.3f GBq",
		"Encima de Actividad Máxima\nCs137 Actividad: %.3f GBq\nLímite Superior: %.3f GBq",
		
		L_BELOW_MINIMUM_ACTIVITY_CS137_CI,
		"Below Minimum Activity\nCs137 Activity: %.3f uCi\nLower Limit: %.3f uCi",
		"Debajo de Actividad Mínima\nCs137 Actividad: %.3f uCi\nLímite Inferior: %.3f uCi",
		
		L_BELOW_MINIMUM_ACTIVITY_CS137_BQ,
		"Below Minimum Activity\nCs137 Activity: %.6f MBq\nLower Limit: %.6f MBq",
		"Debajo de Actividad Mínima\nCs137 Actividad: %.6f MBq\nLímite Inferior: %.6f MBq",
		
		L_ABOVE_MAXIMUM_ACTIVITY_NA22_CI,
		"Above Maximum Activity\nNa22 Activity: %.3f Ci\nUpper Limit: %.3f Ci",
		"Encima de Actividad Máxima\nNa22 Actividad: %.3f Ci\nLímite Superior: %.3f Ci",
		
		L_ABOVE_MAXIMUM_ACTIVITY_NA22_BQ,
		"Above Maximum Activity\nNa22 Activity: %.3f GBq\nUpper Limit: %.3f GBq",
		"Encima de Actividad Máxima\nNa22 Actividad: %.3f GBq\nLímite Superior: %.3f GBq",
		
		L_BELOW_MINIMUM_ACTIVITY_NA22_CI,
		"Below Minimum Activity\nNa22 Activity: %.3f uCi\nLower Limit: %.3f uCi",
		"Debajo de Actividad Mínima\nNa22 Actividad: %.3f uCi\nLímite Inferior: %.3f uCi",
		
		L_BELOW_MINIMUM_ACTIVITY_NA22_BQ,
		"Below Minimum Activity\nNa22 Activity: %.6f MBq\nLower Limit: %.6f MBq",
		"Debajo de Actividad Mínima\nNa22 Actividad: %.6f MBq\nLímite Inferior: %.6f MBq",

		L_INPUT_ERROR_ENGLISH,
		"Input Error",
		"Input Error",

		L_MAX_IS_369_9_MBQ_ENGLISH,
		"Maximum is 369.9 MBq",
		"Maximum is 369.9 MBq",

		L_MAX_IS_9999_UCI_ENGLISH,
		"Maximum is 9999 uCi",
		"Maximum is 9999 uCi",

		L_MIN_IS_3_701_KBQ_ENGLISH,
		"Minimum is 3.701 kBq",
		"Minimum is 3.701 kBq",

		L_MIN_IS_1_UCI_ENGLISH,
		"Minimum is .1 uCi",
		"Minimum is .1 uCi",

		L_CURRENT_FACTOR,
		"Current Factor: %.2f",
		"Factor Actual: %.2f",

		L_CHAMBER_SERIAL,
		"Chamber Serial: %s",	
		"Serie de Cámara: %s",

		L_CHAMBER_SERIAL_OLD_NEW,
		"(Chamber Serial) Old:%s, New:%s",
		"(Serie Cámara) Viejo:%s, Nuevo:%s",

		L_SETUP_LINEATOR_ERROR,
		"Setup Lineator Error",
		"Error en Config. del Lineator",

		L_SETUP_CALICHECK_ERROR,
		"Setup Calicheck Error",
		"Error Config. de Calicheck",

		L_FILL_IN_ALL_MEASUREMENTS,
		"Please fill in all measurements",		
		"Por favor llene todas las medidas",

		L_FILL_IN_NUM_MEASUREMENTS,
		"Please fill in the Num of Measurements",
		"Por favor llene numero de medidas",

		L_FILL_IN_LINEATOR_SERIAL_NUMBER,
		"Please fill in Lineator Serial Number",	
		"Llene el Número de Serie del Lineator",

		L_FILL_IN_CALICHECK_SERIAL_NUMBER,
		"Please fill in Calicheck Serial Number",	
		"Llene el Número de Serie de Calicheck",		//Changed

		L_FACTOR_OLD_NEW,
		"Old: %.2f, New: %.2f",
		"Viejo: %.2f, Nuevo: %.2f",

		L_NEW_FACTOR,
		"New Factor: %.2f",
		"Nuevo Factor: %.2f",

		L_DIFFERENT_CHAMBERS,
		"The current factors are for a different chamber\nand can not be used for this chamber",
		"Los factores actuales son  para una cámara  diferente\ny no se puede utilizar para esta cámara.",

		L_SETUP_LINEATOR,
		"Setup Lineator",
		"Configuración del Lineator",

		L_SETUP_CALICHECK,
		"Setup Calicheck",
		"Configuración de Calicheck",

		L_INVALID_DAY,
		"Invalid Day",
		"Dia invalido",

		L_INVALID_HOUR,
		"Invalid Hour",
		"Hora invalida",

		L_JAN,
		"Jan",
		"ene",

		L_FEB,
		"Feb",
		"feb",

		L_MAR,
		"Mar",
		"mar",

		L_APR,
		"Apr",
		"abr",

		L_MAY,
		"May",
		"may",

		L_JUN,
		"Jun",
		"jun",

		L_JUL,
		"Jul",
		"jul",

		L_AUG,
		"Aug",
		"ago",

		L_SEP,
		"Sep",
		"sep",

		L_OCT,
		"Oct",
		"oct",

		L_NOV,
		"Nov",
		"nov",

		L_DEC,
		"Dec",
		"dec",

		L_SUN,
		"Sun",
		"dom",		//Changed

		L_MON,
		"Mon",
		"lun",		//Changed

		L_TUE,
		"Tue",
		"mar",		//Changed

		L_WED,
		"Wed",
		"mié",		//Changed

		L_THU,
		"Thu",
		"jue",		//Changed

		L_FRI,
		"Fri",
		"vie",		//Changed

		L_SAT,
		"Sat",
		"sáb",		//Changed

		L_SN_MAIN_UNIT,
		"S/N Main Unit:",
		"S/N Unidad Principal:",

		L_CALICHECK_TEST_CALIBRATION,
		"CALICHECK TEST CALIBRATION",
		"Prueba de Calib. de Calicheck",

		L_LINEATOR_TEST_CALIBRATION,
		"LINEATOR TEST CALIBRATION ",
		"PRUEBA DE CALIB. DEL LINEATOR ",

		L_INITIAL_FACTOR,
		"Initial Factor:     ",
		"Factor Inicial:     ",

		L_CALIBRATION_FACTOR,
		"Calibration Factor: ",
		"Factor de Calibración: ",

		L_CH_SERIAL_OLD_NEW,
		"Ch Serial)Old:%s New:%s		",
		"Serie de Ch)Viejo:%s Nuevo:%s		",

		L_R_CHAMBER,
		"R Chamber",
		"cámara R",

		L_P_CHAMBER,
		"P Chamber",
		"cámara P",

		L_SETUP_REMOTE_NUCLIDES,
		"Setup Remote Nuclides",
		"Configura Remota de los Nucleidos",	//Changed
		
		L_AUTOLINEARITY_PAUSED,
		"AutoLinearity Paused!!!",
		"AutoLin. Pausada!!!",

		L_PLEASE_ENTER_CAL_NUM,
		"Please Enter Cal #:",
		"Favor de Introducir el Cal #:",

		L_PLEASE_ENTER_DOSE_TIME,
		"Please Enter Dose Time:",
		"Favor de Introducir Tiempo de Dosis:",

		L_LOW_BATTERY,
		"Low Battery",
		"Batería Baja",

		L_DOSE_DECAY,
		"Dose Decay",
		"Decaimiento de Dosis",

		L_HOME_SCREEN,
		"Home\n Screen",
		"Pantalla\nde Inicio",

		L_NUCLIDE_SCREEN,
		"Nuclide\n Screen",
		"Pantalla de\nNucleido",

		L_BTN_YES,
		"YES",
		"SI",

		L_BTN_NO,
		"NO",
		"NO",

		L_DELETE_LINEATOR_SETTINGS,
		"Delete Lineator Settings",
		"Borrar Ajustes de Lineator",
		
		L_DELETE_CALICHECK_SETTINGS,
		"Delete Calicheck Settings",
		"Borrar Ajustes de Calicheck",

		L_DELETE_LINEATOR_SETTINGS_Q,
		"Delete Lineator Settings?",
		"¿Borrar Ajustes de Lineator?",
		
		L_DELETE_CALICHECK_SETTINGS_Q,
		"Delete Calicheck Settings?",
		"¿Borrar Ajustes de Calicheck?",

		L_UTILITY,
		"Utility",
		"Utilidad",

		L_CONV_INPUT_ACTIVITY,
		"[Ci,Bq Conv] Input Activity:",
		"[Ci,Bq Conv] Introducir Actividad:",

		L_DECAY_CALCULATOR,
		"[Decay Calculator]",
		"[Calculadora de decaimiento]",

		L_FROM,
		"FROM:",
		"DESDE:",

		L_FROM2,
		"From:",
		"Desde:",

		L_TO,
		"TO:",
		"A:",

		L_TO2,
		"To:",
		"A:",

		L_DIAGNOSTICS,
		"Diagnostics",
		"Diagnosticos",

		L_DOSE_TABLE,
		"Dose Table",
		"Tabla de Dosis",

		L_PLEASE_ENTER_ACTIVITY,
		"Please Enter Activity:",
		"Favor de Introducir Actividad:",	//Changed

		L_PLEASE_ENTER_START_TIME,
		"Please Enter Start Time:",
		"Favor de introducir hora de empezar:",		//Changed

		L_PLEASE_ENTER_END_TIME,
		"Please Enter End Time:",
		"Favor de introducir hora de finalización:",	//Changed

		L_TOO_FAR_INTO_FUTURE,
		"TOO FAR INTO FUTURE",
		"DEMASIADO LEJOS HACIA EL FUTURO",

		L_TOO_FAR_INTO_PAST,
		"TOO FAR INTO PAST",
		"DEMASIADO LEJOS HACIA EL PASADO",

		L_TOO_LARGE,
		"TOO LARGE",
		"MUY GRANDE",

		L_PROGRAM_INTEGRITY,
		"PROGRAM INTEGRITY",
		"INTEGRIDAD DEL PROGRAMA",

		L_FAIL,
		"FAIL: ",
		"FALLAR: ",

		L_FAIL2,
		"FAIL",
		"FALLAR",

		L_PASS,
		"PASS: ",
		"SUPERADA: ",

		L_PASS2,
		"PASS",
		"SUPERADA",

		L_PASSED,
		"Passed",
		"Aprovado",

		L_FAILED,
		"Failed, ",
		"Fracasado, ",

		L_DAILY_TEST,
		"Daily Test",
		"Prueba Diaria",

		L_DAILY_TEST2,
		"DAILY TEST:",
		"PRUEBA DIARIA:",

		L_REMOVE_ALL_SOURCES_FROM_CHAMBER,
		"1) Remove All Sources from Chamber",
		"1) Eliminar todas las fuentes de la cámara",

		L_AUTO_ZERO,
		"2) Auto Zero",
		"2) Auto cero",

		L_MEASURE_BACKGROUND,
		"3) Measure Backgnd",
		"3) Medida de fondo",

		L_MEASURE_BACKGROUND2,
		"2) Measure Backgnd",
		"2) Medida de fondo",

		L_CHECK_CHAMBER_VOLTAGE,
		"4) Check Chamber Voltage",
		"4) Verifique el voltaje de la cámara",

		L_CHECK_CHAMBER_VOLTAGE2,
		"1) Check Chamber Voltage",
		"1) Verifique el voltaje de la cámara",

		L_DATA_CHECK,
		"5) Data Check",
		"5) Verificación de los datos",

		L_ACCURACY,
		"Accuracy",
		"Exactitud",	//Changed

		L_ZERO_DRIFT,
		"Zero Drift",
		"Desvío Cero",

		L_ZERO_OUT_OF_RANGE,
		"Zero out of Range",
		"Cero Fuera de Límite",

		L_BACKGROUND_TOO_HIGH,
		"BACKGROUND TOO HIGH",
		"FONDO DEMASIADO ALTO",

		L_BACKGROUND_TOO_HIGH2,
		"BACKGROUND\n TOO HIGH",
		"FONDO\n DEMASIADO ALTO",

		L_BACKGROUND_TOO_HIGH3,
		"Background Too High",
		"Fondo Demasiado Alto",

		L_NO_DAILY_SOURCE_DATA,
		"6) NO DAILY SOURCE DATA",
		"6) NO HAY DATO DE FUENTE DIARIA",

		L_PLEASE_WAIT_2_MINUTES_FOR,
		"PLEASE WAIT 2 Minutes for",
		"FAVOR DE ESPERAR 2 minutos para",

		L_STABILIZATION,
		"Stabilization",
		"Estabilización",

		L_PLEASE_WAIT_2_MINUTES_FOR_STABILIZATION,
		"PLEASE WAIT 2 Minutes for Stabilization",
		"FAVOR DE ESPERAR 2 minutos para Estabilización",

		L_HIGH,
		"HIGH",
		"ALTO",

		L_CHAMBER,
		"Chamber",
		"Cámara",

		L_ZERO,
		"ZERO:",
		"CERO:",

		L_BACKGROUND,
		"BACKGROUND:",
		"FONDO:",

		L_BACKGROUND2,
		"Background",
		"Fondo",

		L_CHAMBER_VOLTAGE,
		"CHAMBER VOLTAGE:",
		"VOLTAJE de la CÁMARA:",

		L_VOLTS,
		"Volts",
		"V",

		L_DATA_CHECK_PRINTED,
		"DATA CHECK:",
		"Verificación de los datos:",

		L_SIGNATURE,
		"signature",
		"firma",

		L_DOSE_TABLE_VOLUME,
		"Volume",
		"Volumen",

		L_DOSE,
		"Dose",
		"Dosis",

		L_INTERVAL,
		"Interval",
		"Intervalo",

		L_INTERVAL2,
		"Interval:",
		"Intervalo:",

		L_MEASURE_ACTIVITY,
		"Measure Activity",
		"Medir la Actividad",		//Changed
	
		L_PLEASE_ENTER_VOLUME,
		"Please Enter Volume(ml):",
		"Favor de Introducir el Volumen(ml):",		//Changed

		L_PLEASE_ENTER_DOSE,
		"Please Enter Dose",
		"Favor de introducir dosis:",		//Changed

		L_ENTER_INTERVAL,
		"Enter Interval in minutes:",
		"Favor de introducir Intervalo en minutos:",	//Changed

		L_PR_TIME,
		"TIME",
		"HORA",

		L_DOSE_TABLE_ERROR,
		"Dose Table Error",
		"Error de la Tabla de Dosis",
		
		L_DOSE_LARGER_THAN_ACTIVITY,
		"Dose can not be larger than initial Activity.",
		"Dosis no puede ser mayor que la actividad inicial."

		L_EXCEEDS_INITIAL,
		" - Exceeds Initial",
		" - Inicial Excedido",

		L_PR_DOSE_TABLE,
		"DOSE TABLE",
		"TABLA DE DOSIS",

		L_PR_VOLUME_HEADING,
		"VOLUME (ml)",
		"VOLUMEN (ml)",

		L_PR_EXCEEDS_INITIAL,
		"Exceeds Initial Vol",
		"Vol. Inicial Excedido",

		L_PR_NUCLIDE_DATA,
		"NUCLIDE DATA:",
		"DATOS DE NUCLEIDO:",

		L_PR_TEST_SOURCES,
		"TEST SOURCES:",
		"FUENTES DE PRUEBA:",

		L_PR_NO_SOURCE,
		"No Source",
		"Ninguna Fuente",

		L_PR_DAILY_CONSTANCY,
		"D: Daily  C: Constancy Source",		
		"D: Diaria C: Fuente de Estabilidad",	//Changed

		L_PR_RESPONSE_CORRECTION,
		"Response Correction:",
		"Corrección de respuesta:",

		L_PR_WAS_LOW,
		"(Co60 was LOW )",					
		"(Co60 fue BAJO )",

		L_PR_WAS_HIGH,
		"(Co60 was HIGH)",					
		"(Co60 fue ALTO)",

		L_PR_NOMINAL_VOLTAGE,
		"Nominal Voltage: ",
		"Voltaje Nominal: ",

		L_PR_MEMORY_STATUS,
		"MEMORY STATUS",
		"ESTADO DE MEMORIA",

		L_PR_AVAILABLE,
		"Available:",
		"Disponible:",

		L_PR_TOTAL,
		"Total:",
		"Total:",
		
		L_PR_R_CHAMBER,
		"R CHAMBER",
		"Cámara R",
		
		L_PR_PET_CHAMBER,
		"PET CHAMBER",
		"Cámara PET",

		L_PET_CHAMBER,
		"PET Chamber",
		"Cámara PET",
		
		L_MEASURED,
		"Measured:",
		"Medir:",

		L_MEASURED2,
		"Measured",
		"Medir",

		L_MEASURED3,
		"Measured",
		"Medido",

		L_CALCULATED,
		"Calculated:",
		"Calculado:",

		L_AUTOCONSTANCY,
		"AutoConstancy",
		"Auto-Estabilidad",

		L_NOSOURCEDATA,
		"No Source Data",
		"Ningún Datos de Fuente",

		L_DEVIATION,
		"Deviation:",
		"Desviación:",

		L_PR_USER_CHAMBER_NUCLIDES,
		"USER CHAMBER NUCLIDES:",
		"USUARIO de CÁMARA de NUCLEIDOS:",

		L_PR_HV_DIRECTLY_INTO_ADC,
		"HV directly into ADC",
		"HV directamente en ADC",

		L_PR_SINGLE_GAIN_RELAY,
		"Single Gain Relay",
		"Relé de ganancia única",	//Changed

		L_PR_GAIN,
		"Gain: %4.6f",
		"Ganancia: %4.6f",

		L_PR_TIME_UNIT_1,
		"M",
		"M",

		L_PR_TIME_UNIT_2,
		"H",
		"H",

		L_PR_TIME_UNIT_3,
		"D",
		"D",

		L_PR_TIME_UNIT_4,
		"Y",
		"A",
		
		L_PR_TIME_UNIT_5,
		"MY",
		"MA",
		
		L_PR_HALFLIFE,
		"Half-Life",
		"Vida-media",

		L_PR_HALFLIFE2,
		"Halflife:",
		"Vida media:",

		L_PR_CURRENT,
		"CURRENT",
		"ACTUAL",

		L_PR_CALIBRATION,
		"CALIBRATION",
		"CALIBRACIÓN",

		L_PR_USER_NUCLIDES,
		"USER NUCLIDES:",
		"USUARIO DE NUCLEIDOS:",

		L_PR_CO60_HIGH,
		"(Co60 HIGH)",
		"(Co60 ALTO)",

		L_PR_CO60_LOW,
		"(Co60 LOW )",
		"(Co60 BAJO )",

		L_ACCURACYTEST,
		"ACCURACY TEST:",
		"PRUEBA DE EXACTITUD:",

		L_SOURCE,
		"Source:",
		"Fuente:",

		L_SERIALNUM,
		"Serial #:",
		"# de Serie:",

		L_STANDSOURCE,
		"Stand. Source(S):",
		"Fuente Estándar(S):",		//Changed

		L_MEASUREDAS,
		"Measured As  (M):",
		"Medido como (M):",

		L_DEVIATIONMSS,
		"Deviation(M-S)/S:",
		"Desv.    (M-S)/S:",

		L_CONSTANCYTEST,
		"CONSTANCY TEST:",
		"PRUEBA DE ESTABILIDAD:",	//Changed

		L_FINISHED,
		"Finished",
		"Terminado",

		L_FINISHED2,
		"Finished!!!",
		"Terminado!!!",

		L_CHAMBER_VOLTS,
		"Chamber Volts",
		"Voltios de la cámara",

		L_ENHANCED_TESTS,
		"Enhanced Tests",
		"Pruebas Mejoradas",

		L_GEOMETRY,
		"Geometry",
		"Geometría",

		L_LINEARITY,
		"Linearity",
		"Linealidad",

		L_QC,
		"QC",
		"QC",

		L_INVENTORY,
		"Inventory",
		"Inventario",

		L_INVENTORY_LIST,
		"Inventory List",
		"Lista de Inventario",

		L_STUDY,
		"Study",
		"Estudio",

		L_LOT,
		"Lot",
		"Lote",

		L_DATE,
		"Date",
		"Fecha",

		L_ID,
		"ID",
		"ID",

		L_CONC,
		"Conc",
		"Conc",

		L_ADD_ITEM,
		"Add Item",
		"Añadir",		//Changed

		L_DELETE_ALL,
		"Delete All",
		"Borre Todo",
		
		L_WITHDRAW,
		"Withdraw",
		"Extraer",		//Changed

		L_MAKE_KIT,
		"Make Kit",
		"Kit",

		L_DELETE_ITEM,
		"Delete Item",
		"Borre Ítem",

		L_PR_INVENTORY,
		"INVENTORY",
		"INVENTARIO",

		L_PR_LOT,
		"Lot #:",
		"Lote#:",

		L_PR_OVER,
		"OVER",
		"POR ARRIBA",

		L_ADD_INVENTORY_ITEM,
		"Add Inventory Item",
		"Añadir Elemento al Inventario",	//Changed

		L_NUCLIDE_COLON,
		"Nuclide:",
		"Nucleido:",

		L_NUCLIDE_COLON2,
		"NUCLIDE:",
		"NUCLEIDO:",

		L_LOT_COLON,
		"Lot:",
		"Lote:",

		L_ACTIVITY_COLON,
		"Activity:",
		"Actividad:",

		L_DATE_COLON,
		"Date:",
		"Fecha:",

		L_PLEASE_ENTER_ID,
		"Please Enter ID:",
		"Favor de introducir el ID:",	//Changed

		L_PLEASE_ENTER_MEASUREMENT_TIME,
		"Please Enter Measurment Time:",
		"Favor de Introducir Tiempo de Medición:"	//Changed

		L_PLEASE_ENTER_LOT,
		"Please Enter Lot:",
		"Favor de Introducir el Lote:",

		L_PLEASE_ACCEPT_ACTIVITY,
		"Please Accept Activity:",
		"Favor de aceptar la actividad:",
		
		L_SUBMIT,
		"Submit",
		"Someter",

		L_NONE,
		"None",
		"Ninguno",

		L_BONE,
		"Bone",
		"Hueso",

		L_LUNG,
		"Lung",
		"Pulmón",

		L_HIDA,
		"Hida",
		"Hepatobiliar",

		L_HEART,
		"Heart",
		"Corazón",

		L_RENAL,
		"Renal",
		"Renal",

		L_LIVER,
		"Liver",
		"Hígado",

		L_BRAIN,
		"Brain",
		"Cerebro",

		L_LYMPH,
		"Lymph",
		"Linfa",

		L_STUDY_LABEL,
		"STUDY:",
		"ESTUDIO:",

		L_MEASURE,
		"Measure",
		"Medir",	//Changed

		L_PLEASE_ENTER_MOTC_RATIO,
		"Please Enter Mo/Tc Ratio:",
		"Favor de Introducir proporción Mo/Tc:",

		L_PLEASE_SELECT_STUDY,
		"Please select study",
		"Favor de seleccionar el estudio",
		
		L_CONTAINER,
		"1) Container",
		"1) Recipiente",

		L_INITIAL_VOLUME,
		"2) Initial Volume",
		"2) Volumen Inicial",

		L_INITIAL_MEASUREMENT,
		"Initial Measurement",
		"Medida Inicial",

		L_ADDED_VOLUME,
		"4) Added Volume",
		"4) Volumen Agregado",

		L_VOLUME2,
		"4) Volume",
		"4) Volumen"

		L_MEASUREMENT,
		"Measurement",
		"Medida",

		L_CAL_NOT_ENTERED_FOR_NUCLIDE,
		"CAL # NOT ENTERED FOR Tc99m",
		"CAL # NO INTRODUCIDO para el NUCLEIDO",

		L_MORE_MEASUREMENTS,
		"More Measurements",
		"Mas Medidas",

		L_SYRINGE,
		"Syringe",
		"Jeringa",

		L_VIAL,
		"Vial",
		"Frasco",

		L_VALID_RANGE_FOR_GEOMETRY,
		"Valid range is 98.9ml to 0.1ml",
		"El Límite válido es de 98.9ml a 0.1ml",

		L_VALID_RANGE_IS_ML_TO_ML,
		"Valid range is %.3fml to 0.1ml",
		"El Límite válido es de %.3fml a 0.1ml",

		L_ADDED_VOL_TOTAL_VOL,
		"Added Vol: %.3fml\nTotal Vol: %.3fml",
		"Vol Agregado: %.3fml\nVol Total: %.3fml",

		L_BASE,
		"BASE",
		"BASE",

		L_SYRINGE_ASSAY,
		"Syringe Assay",
		"Ensayo de la jeringa",		//Changed

		L_VIAL_ASSAY,
		"Vial Assay",
		"Ensayo del frasco",	//Changed

		L_GEOMETRY_REPORT,
		"Geometry Report",
		"Reporte de Geometría",

		L_USING_VOLUME,
		"Using Volume",
		"Volumen Usado",

		L_PERCENT_VAR,
		"% Var",
		"% de Variación",

		L_PERCENT_VAR2,
		"% Var:",
		"% de Variación:",

		L_GEOMETRY_TEST,
		"GEOMETRY TEST",
		"PRUEBA de GEOMETRIA",

		L_USING,
		"Using:",
		"Usando:",

		L_VOLUME_ASSAY_VARIATION,
		"#   Volume    Assay    Variation",
		"#   Volumen   Ensayo   Variación",

		L_STD_LINEATOR_CALICHECK_AVAILABLE_IN_SETUP,
		"Standard, Lineator and Calicheck are available in setup",
		"Estándar, Lineator y Calicheck están disponibles en la configuración",

		L_AUTOLINEARITY,
		"AutoLinearity",
		"AutoLinealidad",

		L_STANDARD,
		"Standard",
		"Estándar",

		L_LINEATOR,
		"Lineator",
		"Lineator",

		L_CALICHECK,
		"Calicheck",
		"Calicheck",

		L_SELECT_CHAMBER,
		"Select Chamber",
		"Seleccione Cámara",

		L_DELETE_ALL_INVENTORY,
		"Delete All Inventory",
		"Borrar Todo el Inventario",

		L_DELETE_ALL_INVENTORY_Q,
		"Delete All Inventory?",
		"¿Borrar todo Inventario?",

		L_PET_CHAMBER_NO_INVENTORY,
		"The PET Chamber can not access inventory functions",
		"La cámara PET no pueden acceder a las funciones del inventario",

		L_DELETE_INVENTORY,
		"Delete Inventory",
		"Borrar Inventario",

		L_TC_LEVEL_TOO_LOW,
		"Tc Level TOO LOW",
		"Nivel de Tc Demasiado Bajo",

		L_WITHDRAW_INVENTORY,
		"Withdraw Inventory",
		"Retirar del Inventario",	//Changed

		L_WITHDRAW_ACTIVITY,
		"Withdraw Activity:",
		"Extraer Actividad:",	//Changed

		L_PLEASE_WITHDRAW_ML,
		"Please withdraw: %5.1f ml",
		"Favor de Extraer: %5.1f ml",

		L_AND_MEASURE_ACTIVITY,
		"and Measure Activity."		
		"Y Medir Actividad."		//Changed
		
		L_ACTIVITY_GREATER_THAN_INVENTORY,
		"ACTIVITY GREATER THAN THAT IN INVENTORY",
		"ACTIVIDAD MAYOR QUE LA DE INVENTARIO",

		L_PLEASE_MEASURE_WITHDRAWAL,
		"Please Measure Withdrawal:",
		"Favor de Medir lo Extraido:",

		L_TIME_OF_USE,
		"Time of Use:",
		"Tiempo de uso:",

		L_PLEASE_ENTER_TIME_OF_USE,
		"Please Enter Time Of Use:",
		"Favor de Introducir el Tiempo de Uso:",

		L_START_TEST,
		"Start Test",
		"Iniciar Prueba",

		L_RESUME_TEST,
		"Resume Test",
		"Reanudar la Prueba",

		L_REPORTS,
		"Reports",
		"Reportes",		//Changed

		L_ENTER_TOTAL_TIME_IN_HOURS,
		"Enter Total Time in hours:",
		"Introduzca Tiempo Total en horas:",

		L_INACTIVE,
		"Inactive:",
		"Inactivo:",

		L_START,
		"Start",
		"Iniciar",

		L_DATE_TIME_ELAPSED,
		"Date Time / Elapsed",
		"Fecha Hora/Transcurrido",

		L_PREDICTED,
		"Predicted",
		"Previsto",

		L_PREDICTED2,
		"Predicted:",
		"Previsto:",

		L_ABORT,
		"Abort",
		"Cancelar",

		L_INACTIVATE,
		"Inactivate",
		"INACTIVAR",

		L_SAVE,
		"Save",
		"Guardar",

		L_AUTOLINEARITY_TEST,
		"AutoLinearity Test - Ch:%d, %s",
		"Prueba de AutoLinealidad - Ch:%d, %s",

		L_AUTOLINEARITY_TEST2,
		"AutoLinearity Test",
		"Prueba de AutoLinealidad",

		L_HRS,
		"%d hrs",
		"%d hrs",

		L_AUTOLINEARITY_REPORT1,
		"AutoLinearity Report, Ch:1 (%s)",
		"Reporte de AutoLinealidad,Ch:1(%s)",

		L_AUTOLINEARITY_REPORT2,
		"AutoLinearity Report, Ch:2 (%s)",
		"Reporte de AutoLinealidad,Ch:2(%s)",

		L_AUTOLINEARITY_REPORT3,
		"AutoLinearity Report, S/N: %s",
		"Reporte de AutoLinealidad,S/N:%s",

		L_ELAPSED,
		"%s\nElapsed: %d min",
		"%s\nTranscurrido: %d min",

		L_PAUSED,
		"Paused",
		"En pausa",

		L_POWER_FAIL,
		"Power Fail",
		"Fallo de Energía",

		L_OF,
		"%d of %d",
		"%d de %d",

		L_UNABLE_TO_FIND_CHAMBER,
		"Unable to find Chamber",
		"Incapaz de encontrar la Cámara",

		L_UNABLE_TO_FIND_NUCLIDE,
		"Unable to find Nuclide",
		"Incapaz de encontrar el Nucleido",

		L_UNABLE_TO_FIND_NUCLIDE2,
		"Unable to find Nuclide",
		"Incapaz de encontrar",

		L_CURRENT_NUCLIDE_IS_INCORRECT,
		"Current Nuclide is incorrect",
		"Nucleido actual es incorrecto",

		L_CHAMBER_SN_IS_INCORRECT,
		"Chamber S/N is incorrect",
		"S/N de la Cámara es incorrecto",

		L_MISSING_NUCLIDE,
		"Missing Nuclide",
		"Falta Nucleido",

		L_MISSING_INTERVAL,
		"Missing Interval",
		"Falta Intervalo",

		L_MISSING_TOTAL_TIME,
		"Missing Total Time",
		"Falta Tiempo Total",

		L_NUM_OF_MEAS_HAS_TO_BE_GREATER_THAN_4,
		"Number of measurements has to be greater than 4",
		"Número de mediciones tiene que ser mayor que 4",

		L_TYPE,
		"Type:",
		"Tipo:",

		L_STARTED,
		"Started:",
		"Iniciado:",

		L_CANT_BE_SAVED_LESS_THAN_5_MEASUREMENTS,
		"Can't be saved.\nLess than 5 measurements",
		"No puede ser guardado.\nMenos de 5 mediciones",

		L_ELAPSED_MINUTES_REMAINING_MINUTES,
		"Elapsed Minutes: %d\nRemaining Minutes:%d",
		"Min Transcurridos:%d\nMin Restantes: %d",

		L_SEARCH_AUTOLINEARITY_TESTS,
		"Search AutoLinearity Tests",
		"Buscar las Pruebas de AutoLinealidad",

		L_ENTER_START_DATE,
		"Enter Start Date:",
		"Introduzca Fecha de Inicio:",

		L_ENTER_END_DATE,
		"Enter End Date:",
		"Introduzca Fecha Final:",

		L_SEARCH,
		"Search",
		"Buscar",

		L_VIEW,
		"View",
		"Ver",

		L_SEARCH_AUTOLINEARITY_ERROR,
		"Search AutoLinearity Error",
		"Buscar Error AutoLinealidad",

		L_MORE_THAN_100_ITEMS_RETURNED,
		"More than 100 items have been returned\nPlease refine criteria",
		"Más de 100 elementos han sido regresados. Favor de revisar los criterios",

		L_0_ITEMS_RETURNED,
		"0 items have been returned",
		"0 Elementos han sido regresados",

		L_MAKE_INVENTORY_KIT,
		"Make Inventory Kit",
		"Preparar Kit del Inventario",

		L_SOURCE_NUCLIDE,
		"Source Nuclide:",
		"Fuente Nucleido:",

		L_SOURCE_ID,
		"Source ID:",
		"ID de la Fuente:",

		L_SOURCE_LOT,
		"Source Lot:",
		"Lote de la Fuente:",

		L_SOURCE_VOLUME,
		"Source Volume:",
		"Volumen de la Fuente:",

		L_SOURCE_ACTIVITY,
		"Source Activity:",
		"Act. de la Fuente:",
		
		L_SOURCE_DATE,
		"Source Date:",
		"Fecha de la Fuente:",

		L_KIT_STUDY,
		"Kit Study:",
		"Estudio del Kit:",

		L_KIT_ID,
		"Kit ID:",
		"Identificación del Kit:",

		L_KIT_LOT,
		"Kit Lot:",
		"Lote del Kit:",

		L_KIT_ACTIVITY,
		"Kit Activity:",
		"Actividad del Kit:",

		L_KIT_VOLUME,
		"Kit Volume:",
		"Volumen del Kit:",
		
		L_KIT_ACTIVITY_GREATER,
		"KIT ACTIVITY GREATER THAN THAT",
		"ACTIVIDAD DEL KIT MAYOR QUE",
		
		L_IN_SOURCE,
		"IN SOURCE",
		"EN LA FUENTE",

		L_KIT_VOLUME_LESS,
		"KIT VOLUME LESS THAN THE"
		"%.3f ml de la fuente mayor que",	//Changed

		L_DRAW_FROM_SOURCE,
		"%.3f ml DRAW FROM SOURCE",
		"el volumen que el kit requiere.",		//Changed

		L_PLEASE_WITHDRAW_AND_MEASURE_ACTIVITY,
		"Please withdraw: %5.1f ml,\n and Measure Activity.",
		"Favor de extraer: %5.1f ml,\n y Mida Actividad.",

		L_CAPMAC_FOR_MAL_GEN,
		"CAPMAC for Mallinckrodt Gen",
		"CAPMAC para Mallinckrodt Gen",

		L_CAPMAC_FOR_BM_GEN,
		"CAPMAC for Bristol Myers Gen",
		"CAPMAC para Bristol Myers Gen",

		L_CAPINTEC_CANISTER,
		"Capintec Canister",
		"Capintec Frasco",

		L_MOLY_ASSAY,
		"Moly Assay",
		"Ensayo de Moly",	//Changed

		L_MOLY_ASSAY_MAL_GEN_METHOD,
		"Moly Assay - Mallinckrodt Gen Method",
		"Ensayo de Moly - Método Mallinckrodt Gen",	//Changed

		L_MOLY_ASSAY_BM_GEN_METHOD,
		"Moly Assay - Bristol Myers Gen Method",
		"Ensayo de Moly - Método Bristol Myers Gen",	//Changed

		L_MOLY_ASSAY_CAPINTEC_CANISTER_METHOD,
		"Moly Assay - Capintec Canister Method",
		"Ensayo de Moly - Método Capintec Frasco",		//Changed

		L_MEASURE_BKG,
		"Measure Bkg",
		"Medir el Fondo",	//Changed

		L_SKIP_BKG,
		"Skip Bkg",
		"Omitir el Fondo",	//Changed

		L_MEASURE_CANISTER_BKG,
		"1) Measure Canister Bkg:",
		"1) Midir Fondo de Frasco:",	//Changed

		L_MEASURE_CAPMAC_BKG,
		"1) Measure CAPMAC Bkg:",
		"1) Medir Fondo de CAPMAC:",	//Changed

		L_SKIPPED,
		"Skipped",
		"Omitido",		//Changed

		L_ELUATE_IN_CANISTER,
		"2) Eluate in CANISTER: ",
		"2) Eluido en FRASCO:   ",

		L_ELUATE_IN_CAPMAC,
		"2) Eluate in CAPMAC: ",
		"2) Eluido en CAPMAC: ",

		L_MEASURE_TC_ASSAY,
		"3) Measure Tc99m Assay:"
		"3) Medir Ensayo de Tc99m:",		//Changed

		L_ACTIVITY_TOO_LOW_ERROR,
		"Activity Too Low Error",
		"Error: Actividad Muy Baja",		//Changed

		L_CAUTION,
		"CAUTION",
		"PRECAUCIÓN",

		L_DO_NOT_USE_AFTER_HOURS,
		"DO NOT USE AFTER %d HOURS",
		"NO UTILICE DESPUÉS DE %d HRS",

		L_MO_TOO_HIGH,
		"MO TOO HIGH",
		"MO DEMASIADO ALTO",

		L_DO_NOT_USE,
		"DO NOT USE",
		"NO UTILICE",

		L_MO_HIGH_TERMINATE_QUESTION,
		"MO HIGH Terminate?",
		"Moly ALTA ¿Terminar?",		//Changed

		L_NEGATIVE_ACTIVITY,
		"NEGATIVE ACTIVITY",
		"ACTIVIDAD NEGATIVA",

		L_MO_HIGH,
		"MO HIGH",
		"MO ALTO",

		L_EXPIRES,
		"Expires",
		"Expira",

		L_MOLY_BKG,
		"Moly Bkg",
		"Fondo de Moly",
		

		L_MO_TOO_HIGH_DO_NOT_USE,
		"MO TOO HIGH - DO NOT USE",
		"MO DEMASIADO ALTO - NO UTILICE",

		L_SINGLE_STRIP_TEST,
		"Single Strip Test",
		"Prueba de Tira Única",

		L_TWO_STRIP_TEST,
		"Two Strip Test",
		"Prueba de Doble Tira",		//Changed

		L_HMPAO_TEST,
		"HMPAO Test",
		"Prueba de HMPAO",

		L_MAG3_TEST,
		"MAG3 Test",
		"Prueba de MAG3",

		L_NONE_FOUND,
		"No detectors found\nNo Chamber attached\nNo Well attached",		,
		"Ningún detector encontrado\nNinguna Cámara conectada\nNingún contador de pozo esta adjunto",
		
		L_QC_ONE_STRIP_TEST,
		"QC - One Strip Test",
		"QC - Prueba de Una Tira",

		L_TOP_OF_STRIP,
		"1) Top of strip",
		"1) Tira parte Superior",

		L_BOTTOM_OF_STRIP,
		"2) Bottom of strip",
		"2) Tira parte Inferior",
		
		L_TOP_OF_STRIP2,
		"Top of Strip:",
		"Tira parte Superior:",

		L_BOTTOM_OF_STRIP2,
		"Bottom of Strip:",
		"Tira parte Inferior:",

		L_ONE_STRIP,
		"One Strip",
		"Una Tira",

		L_QC_TWO_STRIP_TEST,
		"QC - Two Strip Test",
		"QC - Prueba de Doble Tira",	//Changed

		L_TOP_OF_STRIP_A,
		"1) Top of strip A",
		"1) Parte Sup. Tira A",

		L_BOTTOM_OF_STRIP_A,
		"2) Bottom of strip A",
		"2) Parte Inf. Tira A",
		
		L_TOP_OF_STRIP_B,
		"3) Top of strip B",
		"3) Parte Sup. Tira B",

		L_BOTTOM_OF_STRIP_B,
		"4) Bottom of strip B",
		"4) Parte Inf. Tira B",

		L_TOP_OF_STRIP_C,
		"5) Top of strip C",
		"5) Parte Sup. Tira C",

		L_BOTTOM_OF_STRIP_C,
		"6) Bottom of strip C",
		"6) Parte Inf. Tira C",

		L_TOP_OF_STRIP_A2,
		"Top of Strip A:",
		"Parte Sup. de Tira A",

		L_BOTTOM_OF_STRIP_A2,
		"Bottom of Strip A:",
		"Parte Inf. de Tira A",
		
		L_TOP_OF_STRIP_B2,
		"Top of Strip B:",
		"Parte Sup. de Tira B",

		L_BOTTOM_OF_STRIP_B2,
		"Bottom of Strip B:",
		"Parte Inf. de Tira B",
		
		L_TOP_OF_STRIP_C2,
		"Top of Strip C:",
		"Parte Sup. de Tira C",

		L_BOTTOM_OF_STRIP_C2,
		"Bottom of Strip C:",
		"Parte Inf. de Tira C",

		L_FREE,
		"Free Tc99m : %5.1f",
		"Tc99m Libre : %5.1f",	//Changed

		L_REDUCED_HYDROLIZED,
		"Reduced/Hydrolized: %5.1f",
		"Reducido/Hidrolizado: %5.1f",

		L_HYDROLIZED_REDUCED,
		"Hydrolized/Reduced: %5.1f",
		"Hidrolizado/Reducido: %5.1f",

		L_PURITY,
		"Purity: %5.1f",
		"Pureza: %5.1f",

		L_TOP_TOP_BOTTOM_EQUALS,
		"    Top/(Top+Bottom) = %5.1f",
		"Superior/(Superior+Inferior) = %5.1f",

		L_BOTTOM_TOP_BOTTOM_EQUALS,
		"Bottom/(Top+Bottom) = %5.1f",
		"Inferior/(Superior+Inferior) = %5.1f",

		L_QUALITY_CONTROL_TEST,
		"QUALITY CONTROL TEST",
		"PRUEBA DE CONTROL DE CALIDAD",

		L_SINGLE_STRIP_SINGLE_SOLVENT,
		"Single Strip / Single Solvent",
		"Tira Única / Solvente Única",

		L_MEASURED_VALUES,
		"Measured Values",
		"Valores Medidos",

		L_RESULTS,
		"Results",
		"Resultados",

		L_QC_RADIOPHARMACEUTICAL,
		"Radiopharmaceutical: _________",
		"Radiofármaco: ________________",	//Changed

		L_QC_LOT,
		"Lot #: _______________________",
		"Lote#: _______________________",

		L_QC_KIT,
		"Kit #: _______________________",		
		"Kit #: _______________________",

		L_TOP_TOP_BOTTOM,
		"Top    / (Top + Bottom): ",
		"Superior /  (Sup.+Inf.): ",

		L_BOTTOM_TOP_BOTTOM,
		"Bottom / (Top + Bottom): ",
		"Inferior /  (Sup.+Inf.): ",

		L_TWO_STRIP,
		"Two Strip",
		"Doble Tira",	//Changed

		L_TWO_STRIPS_TWO_SOLVENTS,
		"Two Strips / Two Solvents",
		"Doble Tira /Dos Solventes",	//Changed

		L_STRIP_A_FREE,
		"Strip A: Free          = ",
		"Tira  A: Libre         = ",

		L_STRIP_B_R_H,
		"Strip B: R / H         = ",
		"Tira  B: R / H         = ",

		L_PURITY_F_R_H,
		"Purity: 100 -(F + R/H) = ",	
		"Pureza: 100 -(L + R/H) = ",

		L_F_FREE,
		"F = Free",
		"L = Libre",

		L_R_REDUCED,
		"R = Reduced",
		"R = Reducido",

		L_H_HYDROLIZED,
		"H = Hydrolized",
		"H = Hidrolizado",

		L_FREE_TC99M,
		"Free Tc99m              :",
		"Tc99m Libre             :",	//Changed

		L_HYDROLIZED_REDUCED_TC99M,
		"Hydrolized/Reduced Tc99m:",		
		"Hidrolizado/Reducido    :",

		L_TC99M_HMPAO,
		"Tc99m HMPAO             :",
		"Tc99m HMPAO             :",

		L_FRACTION_1,
		"1) Fraction #1",
		"1) Fracción #1",

		L_FRACTION_2,
		"2) Fraction #2",
		"2) Fracción #2",

		L_CARTRIDGE,
		"3) Cartridge",
		"3) Cartucho",

		L_FRACTION_12,
		"Fraction #1      : "
		"Fracción #1      : "

		L_FRACTION_22,
		"Fraction #2      : "
		"Fracción #2      : "

		L_CARTRIDGE2,
		"Cartridge        : "
		"Cartucho         : ",
		
		L_NON_ELUTABLE,
		"Non-elutable Tc99m: %5.1f",
		"Tc99m No eluible  : %5.1f",	//Changed

		L_NON_ELUTABLE2,
		"Non-elutable Tc99m       :",
		"Tc99m No eluible         :",	//Changed

		L_HYDROPHILIC_IMPURE,
		"Hydrophilic Impure: %5.1f",
		"Hidrofílico impuro: %5.1f",

		L_HYDROPHILIC_TC_IMPURITIES,
		"Hydrophilic Tc Impurities:",
		"Impurezas Hidrófilas de Tc:",
		
		L_TC_MERTIATIDE,
		"Tc99m Mertiatide         :",
		"Tc99m Mertiatide         :",
		
		L_COMPLETE_THYROID_UPTAKE_TEST_ENGLISH,
		"Complete Thyroid Uptake Test",
		"Complete Thyroid Uptake Test",

		L_MARK_TEST_COMPLETED_ENGLISH,
		"Mark Test Completed?",
		"Mark Test Completed?",

		L_COMPLETE_RBC_SURVIVAL_TEST_ENGLISH,
		"Complete RBC Survival Test",
		"Complete RBC Survival Test",

		L_INACTIVATE_RECORD,
		"Inactivate Record",
		"Inactivar Registro",

		L_INACTIVATE_RECORD2,
		"Inactivate record?",
		"¿Quieres Inactivar el Registro?",

		L_PLEASE_ENTER_COMMENT,
		"Please enter Comment:",
		"Favor de Introducir el Comentario:",
		
		L_CLEAR_ALL_MEASUREMENTS,
		"Clear All Measurements",
		"Borrar Todas Las Medidas",

		L_MEASURE_NOW,
		"Measure Now",
		"Medir Ahora",	//Changed

		L_NO_SETTINGS,
		"No Settings",
		"Ningunos Ajustes",

		L_FOR_R_CHAMBER,
		"for R Chamber",
		"para la Cámara R",

		L_FOR_R_CHAMBER2,
		"for R Chamber",
		"de Cámara R",

		L_FOR_PET_CHAMBER,
		"for PET Chamber",
		"para la Cámara PET",

		L_FOR_PET_CHAMBER2,
		"for PET Chamber",
		"de Cámara PET",

		L_CALIBRATION_NUMBER,
		"calibration number",
		"número de calibración",

		L_LINEARITY_STD,
		"Linearity, Std",
		"Linealidad, Std.",

		L_IN_HRS_MINS,
		"in %d hrs, %d mins",
		"en %d horas, %d mins",	//Changed

		L_FIRST_MEASUREMENT,
		"First Measurement:",
		"Primera Medida:",

		L_CALC,
		"Calc: ",
		"Calc: ",

		L_FACTOR,
		"Factor",
		"Factor",

		L_RATIO,
		"% Ratio",
		"% Relación",

		L_RATIO2,
		"Ratio:",
		"Relación:",

		L_RESULT,
		"Result",
		"Resultado",

		L_RESULT2,
		"Result:",
		"Resultado:",

		L_STD_LINEARITY,
		"Std Linearity",
		"Std. Linealidad",

		L_STANDARD_LINEARITY_TEST,
		"STANDARD LINEARITY TEST",
		"PRUEBA DE LINEALIDAD ESTÁNDAR",

		L_ACTUAL,
		"Actual:",
		"Actual:",		//Changed

		L_VARIATION,
		"Variation:",
		"Variación:",

		L_LINEATOR_LINEARITY_TEST,
		"LINEATOR LINEARITY TEST",
		"PRUEBA DE LINEALIDAD LINEATOR",

		L_CHAMBER_SN,
		"Chamber S/N: %s",
		"Cámara S/N: %s",

		L_LINEATOR_SN,
		"Lineator S/N: %s",
		"Lineator S/N: %s",

		L_PRESENT_FACTOR,
		"Present Factor:",
		"Factor Presente:",

		L_CALICHECK_LINEARITY_TEST,
		"CALICHECK LINEARITY TEST",
		"PRUEBA DE LINEALIDAD CALICHECK",

		L_CALICHECK_SN,
		"Calicheck S/N: %s",
		"Calicheck S/N: %s",

		L_MEAN_RESULT,
		"Mean Result: %s",
		"Resultado Medio: %s",

		L_VARIATION_FROM_MEAN,
		"Variation from Mean:",
		"Variación de la Media:",

		L_PC_COMMUNICATIONS_MODE,
		"PC Communications Mode",
		"Modo de Comunicaciones de computadora personal",

		L_RUNNING_QC_TESTS_FROM_PC,
		"Running QC tests from PC",
		"Ejecutando Pruebas de Control de Calidad de la PC",

		L_ABORT_QC_TESTS,
		"Abort QC Tests",
		"Cancelar Prueba de Control de Calidad",	//Changed

		L_REMOTE_AUTO_ZERO,
		"Remote Auto Zero",
		"Auto Cero Remoto",

		L_REMOTE_BACKGROUND,
		"Remote Background",
		"Fondo Remoto",

		L_REMOTE_CHAMBER_VOLTAGE_TEST,
		"Remote Chamber Voltage Test",
		"Prueba de Voltaje de la Cámara Remota",

		L_BACKGROUND_HIGH,
		"Background High",
		"Fondo Alto",

		L_CHAMBER_VOLTAGE_FAILED,
		"Chamber Voltage Failed",
		"Falló Voltaje de la Cámara",

		L_PLEASE_WAIT,
		"Please Wait",
		"Favor de Esperar",

		L_PET_CHAMBER_CANNOT_RUN_MOLY_ASSAY,
		"The PET chamber can not run a Moly Assay",
		"La cámara de PET no puede ejecutar un Ensayo de Moly",		//Changed
		
		L_MISSING_USB_DRIVE,
		"Missing USB Drive",
		"Falta Controlador USB",

		L_UNABLE_TO_FIND_FILE,
		"Unable to find file",
		"Incapaz de encontrar el archivo",

		L_PLACE_ONLY_ONE_UPDATE,
		"Place only one update",
		"Coloque un solo archivo de",

		L_FILE_IN_ROOT_FOLDER,
		"file in root folder",
		"actualización en la carpeta raíz",

		L_INVALID_UPDATE_FILE,
		"Invalid update file",
		"Archivo de Actualización Inválido",

		L_COPYING,
		"Copying %s",
		"Copiar %s",

		L_OF2,
		"of %s",
		"de %s",

		L_CORRUPT_UPDATE_FILE,
		"Corrupt Update File",
		"Archivo de Actualización Corruptos",

		L_FINISHED_COPYING,
		"Finished Copying",
		"Terminó Copiado",

		L_PLEASE_RESTART2,
		"Please restart",
		"Favor de Reiniciar",

		L_USB_DRIVE_ERROR,
		"USB Drive Error\n",
		"Error del Controlador USB\n",

		L_SOFTWARE_UPDATE,
		"Software Update",
		"Actualización de software",

		L_SOFTWARE_DOWNLOAD_IN_PROGRESS,
		"Software download is in progress\nPlease do not turn off calibrator",
		"Descarga del Software está en proceso\nPor favor, no apague el calibrador",

		L_PRINTER_PROBLEM,
		"Printer Problem",
		"Problema de la Impresora",

		L_INSERT_TICKET,
		"INSERT TICKET",
		"Introduzca el Tiquete",	//Changed

		L_CHECK_PRINTER,
		"CHECK PRINTER",
		"Revise la Impresora",		//Changed

		L_CONNECT_PRINTER,
		"CONNECT PRINTER",
		"CONECTE IMPRESORA",

		L_INSERT_PAPER,
		"INSERT PAPER",
		"INSERTE PAPEL",

		L_FILE_TRANSFER_ERROR,
		"File Transfer Error",
		"Error de Transfer de Archivo",

		L_UNABLE_TO_FIND_FILE_PATH,
		"Unable to find file path: %s\n",
		"No se puede encontrar la ruta del archivo: %s\n",

		L_UPDATE,
		"Update",
		"Actualizar",

		L_POWER_OFF_AND_THEN_POWER_ON_TO_COMPLETE_UPDATE,
		"Power Off,\nand then Power On\nto complete update.",
		"Apague,\ny después encienda\npara completar la actualización",

		L_CHECKSUM_HAS_FAILED,
		"Checksum has failed.",
		"La suma de comprobacion ha fallado",

		L_UNABLE_TO_FIND_TRANSFER_FILE,
		"Unable to find transfer file.",
		"No se puede encontrar la transferencia de archivos",

		L_UPDATE_SOFTWARE,
		"Update Software",
		"Actualice Software",

		L_NOW,
		"Now:",
		"Ahora:",

		L_PR_CONSTANCY_CHANNELS,
		"Constancy Channels:",
		"Canales de Estabilidad:",		//Changed

		L_OVER,
		" OVER ",
		" ALTO ",		//Changed

		L_CHAMBER_ERROR,
		"Chamber Error",
		"Error de Cámara",		//Changed

		L_CHAMBER_LOST_CONNECTION,
		"Chamber #%d has lost connection.\nPlease turn off calibrator and reattach chamber.",
		"La cámara # 1 perdido la conexión. Por favor, apague el calibrador y reconecte la cámara.",	//Changed

		L_SAVE_ACCURACY_TEST,
		"Save Accuracy / AutoConstancy",
		"Guardar Exactitud / Auto-Estabilidad",

		L_ZERO_2,
		"Zero",
		"Cero",

		L_CHAMBER_VOLTAGE_2,
		"Chamber Voltage",
		"Voltaje de la Cámara",

		L_ACCURACYTEST_2,
		"Accuracy Test",
		"Prueba de Exactitud",

		L_SEARCH_DAILY_TESTS,
		"Search Daily Tests",
		"Buscar Pruebas Diarias",

		L_SEARCH_ZERO_MEASUREMENTS,
		"Search Zero Measurements",
		"Buscar Mediciones Cero",

		L_SEARCH_BACKGROUND_MEASUREMENTS,
		"Search Background Measurements",
		"Buscar Mediciones de Fondo",

		L_SEARCH_CHAMBER_VOLTAGE,
		"Search Chamber Voltage Tests",
		"Buscar Pruebas del Voltaje de la Cámara",

		L_SEARCH_ACCURACY_TESTS,
		"Search Accuracy Tests",
		"Buscar Pruebas de Exactitud",

		L_NOMINAL_VOLTAGE,
		"NOMINAL VOLTAGE:",
		"VOLTAJE NOMINAL:",

		L_MIN_VOLTAGE,
		"MIN VOLTAGE:",
		"VOLTAJE MIN:",

		L_MAX_VOLTAGE,
		"MAX VOLTAGE:",
		"VOLTAJE MAX:",

		L_STAGE,
		"Stage",
		"Etapa",

		L_PAGE,
		"Page %d",
		"Página %d",

		L_HALFLIFE_CALC,
		"Half-life Calculator",
		"Half-life Calculator",

		L_HALFLIFE_CALC2,
		"Half-life Calc - Ch:%d, %s",
		"Half-life Calc - Ch:%d, %s",

		L_SECS,
		"%d sec",
		"%d seq",

		L_ENTER_INTERVAL_SEC,
		"Enter Interval in seconds:",
		"Enter Interval in seconds:",

		L_ENTER_TOTAL_TIME_IN_MINUTES,
		"Enter Total Time in minutes:",
		"Enter Total Time in minutes:",

		L_MINS,
		"%d min",
		"%d min",

		L_STOP,
		"Stop",
		"Stop",

		L_ELAPSED_SEC,
		"Elapsed (sec)",
		"Elapsed (sec)",

		L_NUM_OF_MEAS_GREATER_THAN_1201,
		"Count is greater than 1201\nTry Total: %d or Interval: %d",
		"Count is greater than 1201\nTry Total: %d or Interval: %d",

		L_ELAPSED_REMAINING,
		"Elapsed: %d:%02d\nRemaining: %d:%02d",
		"Elapsed: %d:%02d\nRemaining: %d:%02d",

		L_UNDERRANGE,
		"Under Range",
		"Under Range",

		L_LESS_THAN_5_MEASUREMENTS,
		"Less than 5 measurements",
		"Menos de 5 mediciones",

		L_HALFLIFE_TOO_SHORT,
		"Half-life is too short",
		"Half-life is too short",

		L_HALFLIFE_TOO_LONG,
		"Half-life is too long",
		"Half-life is too long",

		L_HALFLIFE_NEGATIVE,
		"Half-life is negative",
		"Half-life is negative",

		L_CHAMBER_SN_2,
		"Chamber S/N:",
		"Cámara S/N:",

		L_PRINT_SUMMARY,
		"Print\nSummary",
		"Print\nSummary",

		L_PR_BT_CHAMBER,
		"BT CHAMBER",
		"Cámara BT",

		L_PR_18_CHAMBER,
		"1.8 Atm CHAMBER",
		"Cámara 1.8 Atm",

		L_BT_CHAMBER,
		"BT Chamber",
		"cámara BT",

		L_18_CHAMBER,
		"1.8 Atm Chamber",
		"cámara 1.8 Atm",

		L_10_CHAMBER,
		"1 Atm Chamber",
		"cámara 1 Atm",

		L_FOR_BT_CHAMBER,
		"for BT Chamber",
		"para la Cámara BT",

		L_FOR_18_CHAMBER,
		"for 1.8 Atm Chamber",
		"para la Cámara 1.8 Atm",

		L_FOR_BT_CHAMBER2,
		"for BT Chamber",
		"de Cámara BT",

		L_FOR_18_CHAMBER2,
		"for 1.8 Atm Chamber",
		"de Cámara 1.8 Atm",

		L_77T_FULL_TITLE,
		"CRC-77t      RADIOISOTOPE DOSE CALIBRATOR",
		"CRC-77t      CALIBRADOR DE DOSIS DE RADIOISÓTOPOS",

		L_10_CHAMBER_CANNOT_RUN_MOLY_ASSAY,
		"The 1 Atm chamber can not run a Moly Assay",
		"La cámara de 1 Atm no puede ejecutar un Ensayo de Moly",

		L_SETUP_HOTKEYS,
		"Setup Hotkeys",
		"Config. Tecla Rapida",

		L_ENTER_CAL,
		"Please Enter Cal #:",
		"Favor de Introducir Cal #:",

		L_SETUP_LINEARITY_STANDARD,
		"Setup Linearity Standard",
		"Configura el estándar de linealidad",

		L_PR_C_CHAMBER,
		"HR CHAMBER",
		"Cámara HR",

		L_PR_K_CHAMBER,
		"1K CHAMBER",
		"Cámara 1K",

		L_C_CHAMBER,
		"HR Chamber",
		"cámara HR",

		L_K_CHAMBER,
		"1K Chamber",
		"cámara 1K",

		L_FOR_C_CHAMBER,
		"for HR Chamber",
		"para la Cámara HR",

		L_FOR_K_CHAMBER,
		"for 1K Chamber",
		"para la Cámara 1K",

		L_FOR_C_CHAMBER2,
		"for HR Chamber",
		"de Cámara HR",

		L_FOR_K_CHAMBER2,
		"for 1K Chamber",
		"de Cámara 1K",

		L_STANDARD_C_CHAMBER,
		"Standard HR Chamber",
		"Estándar cámara HR",

		L_STANDARD_K_CHAMBER,
		"Standard 1K Chamber",
		"Estándar cámara 1K",

		L_MAX_SECOND_IS_59,
		"Max Second is 59",
		"Max Second is 59",

		L_START_MEASUREMENT,
		"Start Measurement",
		"Start Measurement",

		L_SETUP_KEY,
		"Setup Key",
		"Setup Key",

		L_DEVICE_ID,
		"Device ID:",
		"Device ID:",

		L_KEY,
		"Key:",
		"Key:",

		L_COMM_ENABLED,
		"Communications: Enabled",
		"Communications: Enabled",

		L_COMM_DISABLED,
		"Communications: Disabled",
		"Communications: Disabled",

		L_PLS_ENTER_KEY,
		"Please enter Key",
		"Please enter Key",

		L_ERR_EMPTY_KEY,
		"Error: Key is empty",
		"Error: Key is empty",

		L_ERR_INVALID_KEY,
		"Error: Key is not valid",
		"Error: Key is not valid",

		L_SETUP_PASSWORD,
		"Setup Password",
		"Setup Password",

		L_CURRENT_PASSWORD,
		"Current Password:",
		"Current Password:",

		L_NEW_PASSWORD,
		"New Password:",
		"New Password:",

		L_CONFIRM_PASSWORD,
		"Confirm Password:",
		"Confirm Password:",

		L_ENTER_CURRENT_PASSWORD,
		"Please enter Current Password",
		"Please enter Current Password",

		L_ENTER_NEW_PASSWORD,
		"Please enter New Password",
		"Please enter New Password",

		L_ENTER_CONFIRM_PASSWORD,
		"Please enter Confirm Password",
		"Please enter Confirm Password",

		L_INVALID_CURRENT_PASSWORD,
		"Invalid Current Password",
		"Invalid Current Password",

		L_MISMATCH_PASSWORD,
		"New and Confirm passwords do not match",
		"New and Confirm passwords do not match",

		L_LAST_3_SN,
		"Use last 3 SN digits",
		"Use last 3 SN digits",

		};*/

//message in small font
static const SM_MESSAGE sm_messages[] =
	{
        SIGNON_1,
        "ENTER to Continue",
        2,
        
        CONTINUE,
        "Any Key to Continue",
        1,

        CHANGE_CAL_3,
        "NONE",
        12,

        CHANGE_CAL_4,
        " New Cal #: ",
        0,
        
        CHANGE_CAL_5,
        "Restore Orig Cal # ?",
        0,
        
        CHANGE_CAL_6,
        "Change Cal # ?",
        0,
        
        CHANGE_CAL_7,
        "Delete Cal # ?",
        0,

        CHANGE_CAL_8,
        "Orig Cal #:",
        0,
        
        CHOOSE_NUCLIDE_2,
        "Press NUCL or",
        0,
        
        CHOOSE_NUCLIDE_3,
        "Pre-set nuclide key"        ,
        0,

        CHOOSE_NUCLIDE_4,
        "or User key",
        0,

        CHOOSE_NUCLIDE_5,
        "or CAL# for Half-Life",
        0,

        CHOOSE_NUCLIDE_6,
        "or CAL# for All Chans",
        0,
        
        DATE_TIME_IN_3,
        "Date Error",
        2,
        
        DATE_TIME_IN_4,
        "Time Error",
        2,

        DATE_TIME_IN_6,
        "MMDDYYYY",
        5,

        DATE_TIME_IN_7,
        "hhmm",
        5,

        DATE_TIME_IN_8,
        "hhmm^DD^MM^YY",    //^ displays down arrow
        2,

        DATE_TIME_IN_9,
        "ENTER for NOW",
        1,
        
        USER_KEY_5,
        "Any Other Key To",
        0,

        USER_KEY_6,
        "Continue Setup",
        0,

        AUTO_ZERO_8,
        "ENTER to Accept",
        3,

        ACC_6,
        "Deviation:",
        0,

        CONST_6,
        "CONSTANCY",
        12,

        CONST_7,
        "TEST",
        15,

        BIAS_3,
        "ENTER to Acknowledge",
        0,

        CAL_1,
        "Cal #:",
        0,

        MOLY_1,
        "SELECT",
        4,

        MOLY_2,
        "1. CAPMAC for",
        0,

        MOLY_3,
        "Mallinckrodt Gen",
        3,

        MOLY_4,
        "2. CAPMAC for",
        0,
        
        MOLY_5,
        "Bristol Myers Gen",
        3,

        MOLY_6,
        "3. Capintec Canister",
        0,

        MOLY_7,
        "Any key to Terminate",
        1,

        MOLY_8,
        "Press N(6) to Skip",
        0,

        MOLY_9,
        "Other Key to Proceed",
        0,

        MOLY_12,
        "DO NOT USE AFTER",
        2,

        MOLY_13,
        "HOURS",
        6,

        MOLY_17,
        "Terminate? (Y/N)",
        3,

        CONTRAST_3,
        "Press Enter When Done",
        0,

        REMOTES_3,
        "Already On List",
        1,

        REMOTES_4,
        "Delete? Y or N",
        1,

        VAL_UNIT_1,
        "value",
        2,

        VAL_UNIT_2,
        "unit",
        12,

        VAL_UNIT_3,
        "-> to change unit",
        1,

        ACT_SCR_4,
        "Press BETA to START",
        1,

        ACT_SCR_5,
        "Press WELL to START",
        1,

        BETA_ACT_1,
        "NO DATA EXISTS",
        1,

        BETA_ACT_2,
        "For",
        1,

        BETA_ACT_7,
        "Prin in Imp. ROI /",
        1,

        BETA_ACT_8,
        "Prin in Prin ROI",
        1,

        BETA_ACT_9,
        "Press BETA or DISPLAY",
        0,

        BETA_ACT_10,
        "IMPURITY:",
        3,

        BETA_ACT_14,
        "Impurity in Prin ROI/",
        0,

        BETA_ACT_15,
        "Impurity in Imp. ROI",
        0,

        BETA_ACT_16,
        "No Room To Add",
        0,


        BETA_ACT_18,
        "Write Over",
        0,

        BETA_ACT_19,
        "Existing Data?",
        0,

        CHANNELS_1,
        "Y selects channels",
        0,

        CHANNELS_2,
        "WELL when finished",
        1,

        CHANNELS_3,
        "ENTER when finished",
        1,

        CALIB_16,
        "Press BETA",
        0,

        CALIB_17,
        "Press WELL",
        0,

        CALIB_18,
        "or DISPLAY",
        11,

        CALIB_21,
        "ERROR",
        3,

        CALIB_22,
        "TOO MANY COUNTS",
        3,

        CALIB_23,
        "IN UPPER CHANNEL",
        3,

        CALIB_24,
        "SEE MANUAL",
        3,

        EFF_4,
        "No Room To Add",
        0,

        EFF_5,
        "EFF Data",
        0,

        EFF_6,
        "Write Over",
        0,

        EFF_7,
        "Existing Data?",
        0,

        BKG_10,
        "ENTER or",
        0,
        
        BKG_11,
        "WELL ",
        9,

        BKG_12,
        "BETA ",
        9,

        BKG_13,
        "to SAVE",
        14,

        BKG_14,
        "MENU key for Setup",
        2,

        BKG_14A,
        "or Another key",
        2,
        
        BKG_15,
        "to Continue",
        2,
        
        TEST_16,
        "Measured as:",
        2,

        TEST_17,
        "Deviation:",
        4,

        SM_OK_MSG,
        "OK? Y or N",
        10,

        LAB_8,
        "Press ENTER to Print",
        1,

        LAB_9,
        "Press WELL to Exit",
        1,

        LIN_13,
        "HOURS FOR TESTS",
        0,
        
        LIN_14,
        "Measure Tube # ",
        0,

        LIN_15,
        "Tube # ",
        0,

        LIN_16,
        "Activity: ",
        0,

        LIN_17,
        "Initial Factor",
        0,

        LIN_18,
        "Calibration Factor",
        0,

        LIN_21,
        "Present Factor:",
        0,

        LIN_22,
        "% Ratio: ",
        0,
        
        LIN_23,
        "LINEATOR TEST",
        4,

        LIN_24,
        "#    FACTOR   %RATIO",
        0,

        LIN_25,
        "Result",
        0,

        LIN_26,
        "CALICHECK TEST",
        4,

        LIN_27,
        "MEAN: ",
        1,

        LIN_28,
        "#    RESULT   % VAR ",
        0,    
        
        LIN_32,
        "Test # ",
        0,

        LIN_33,
        "First Measurement",
        0,
        
        LIN_34,
        "Measure At: ",
        0,

        LIN_35,
        "Hours",
        16,

        LIN_36,
        "Elapsed: ",
        0,
        
        LIN_37,
        "Hr",
        12,

        LIN_38,
        "Min",
        18,

        LIN_39,
        "Measured",
        0,

        LIN_40,
        "Predicted",
        0,
        
        LIN_41,
        "STANDARD LINEARITY",
        1,

        LIN_42,
        "#   MEASURED    %VAR",
        0,

        LIN_43,
        "New Test? Y or N",
        1,

        LIN_57,
        "Black Only",
        0,

        LIN_58,
       "Black + Red",
       0,

       LIN_59,
       "Black + Orange",
       0,

       LIN_60,
       "Black + Yellow",
       0,

       LIN_61,
       "Black + Green",
       0,

       LIN_62,
       "Black + Blue",
       0,

       LIN_63,
       "Black + Purple",
       0,

       LIN_64,
       "Black/Purple/Red",
       0,

       LIN_65,
       "Black/Purple/Orange",
       0,

       LIN_66,
       "Black/Purple/Yellow",
       0,

       LIN_67,
       "Black/Purple/Green",
       0,

       LIN_68,
       "Black/Purple/Blue",
       0,
        
        GEOM_4,
        "SELECT BASE",
        4,

        GEOM_5,
        "DOWN: Next",
        0,

        GEOM_6,
        "UP: Prev",
        12,
        
        GEOM_7,
        "GEOMETRY TEST",
        4,
        
        GEOM_8,
        "Using: ",
        0,

        GEOM_9,
        "Syringe",
        7,

        GEOM_10,
        "Vial",
        7,

        GEOM_11,
        "Volume  Assay    %Var",
        0,

        GEOM_12,
        " BASE",
        16,

        INV_8,
        "Lot:",
        0,
      
        INV_9,
        "Volume  :",
        0,

        INV_10,
        "Activity:",
        0,

        INV_11,
        "OVER",
        9,

        INV_12,
        "SELECT STUDY",
        2,

        INV_13,
        "0. NONE",
        1,    

        INV_21,
        "Withdraw:",
        0,

        INV_29,
        "Inventory is Empty",
        0,
        
        INV_30,
        "DELETE",
        7,

        INV_31,
        "SELECT",
        7,

        INV_32,
        "Ci Must Be Selected",
        0,

        INV_33,
        "On Main Screen",
        0,

        INV_34,
        "To Withdraw From",
        0,

        INV_35,
        "Item Entered in Ci",
        0,

        INV_36,
        "Bq Must Be Selected",
        0,
        
        INV_37,
        "Item Entered in Bq",
        0,

        QC_1,
        "Place TOP of Strip",
        0,

        QC_2,
        "Place BOTTOM of Strip",
        0,

        QC_3,
        "in Chamber",
        0,

        QC_4,
        "QC TEST",
        8,

        QC_5,
        "Single Strip",
        2,

        QC_6,
        "Top   /(Top+Bottom)",
        0,

        QC_7,
        "Bottom/(Top+Bottom)",
        0,
        
        QC_8,
        "Place TOP of",
        0,

        QC_9,
        "Place BOTTOM of",
        0,

        QC_10,
        "Strip A in Chamber",
        0,

        QC_11,
        "Strip B in Chamber",
        0,

        QC_12,
        "QC TEST   Two Strips",
        0,
        
        QC_13,
        "Free Tc99m",
        0,

        QC_14,
        "Reduced/Hydrolized",
        0,

        QC_15,
        "Purity",
        0,
        
        QC_16,
        "Strip C in Chamber",
        0,

        QC_17,
        "QC TEST  HMPAO",
        4,
        
        QC_18,
        "Free Tc99m",
        0,

        QC_19,
        "Hydrolized/Reduced",
        0,

        QC_20,
        "Tc99m HMPAO",
        0,

        QC_21,
        "Place Fraction #1",
        0,

        QC_22,
        "Place Fraction #2",
        0,

        QC_23,
        "Place Cartridge",
        0,

        QC_24,
        "QC TEST  MAG3",
        3,

        QC_25,
        "Non-elutable Tc99m",
        0,

        QC_26,
        "Hydrophilic Impure",
        0,

        QC_27,
        "Tc99m Mertiatide",
        0,

        CALC_1,
        "MENU to End",
        2,
        
        
    };



//messages in medium font
static const MED_MESSAGE med_messages[] =
    {
        NO_ROOM,
        "NO ROOM TO ADD",
        1,

        OK_MSG,
        "OK?  Y or N",
        2,

        CAL_NUM_1,
        "ENTER",
        5,

        CAL_NUM_2,
        "CALIBRATION #",
        1,

        ADD_NUCLIDE_1,
        "TYPE IN",
        1,

        ADD_NUCLIDE_2,
        "NUCLIDE NAME",
        1,

        ADD_NUCLIDE_3,
        "NUCLIDE",
        3,

        ADD_NUCLIDE_4,
        "ALREADY EXISTS",
        0,
        
        ADD_NUCLIDE_5,
        "HALF LIFE:",
        0,

        ADD_NUCLIDE_6,
        "ERROR",
        4,

        ADD_NUCLIDE_7,
        "CANNOT DELETE",
        0,

        ADD_NUCLIDE_8,
        "DELETE? Y or N",
        1,

        ADD_NUCLIDE_9,
        "For R",
        1,

        ADD_NUCLIDE_10,
        "For PET",
        1,

        CHANGE_CAL_1,
        "Cal Numbers",
        1,

        CHANGE_CAL_2,
        "Change Cal #?",
        0,

        CHOOSE_NUCLIDE_1,
        "CHOOSE NUCLIDE",
        1,

        CONTAINER_1,
        "CORR. FACTOR",
        1,

        CONTAINER_2,
        "Corr. Factor",
        1,

        CONTAINER_3,
        "For:",
        1,

        CONTAINER_4,
        "ENTER",
        1,


        CONTAINER_5,
        "Corr Facs",
        7,

        DATE_TIME_IN_1,
        "Enter Date",
        4,
        
        DATE_TIME_IN_2,
        "Enter Time",
        4,

         DATE_TIME_IN_5,
        "Correct ?",
        2,
        
        YES_OR_NO,
        "Yes or No",
        2,

        INPUT_1,
        "ERROR",
        5,

        INPUT_2,
        "Entry:",
        0,

        INPUT_3,
        "Minimum:",
        0,

        INPUT_4,
        "Maximum:",
        0,

        MOLY_SETUP_1,
        "Will You Use",
        1,

        MOLY_SETUP_2,
        "CAPMAC For",
        1,
        
        MOLY_SETUP_3,
        "Generator ?",
        1,

        MOLY_SETUP_4,
        "Mo/Tc LIMIT:",
        1,

        MOLY_SETUP_5,
        "INPUT",
        1,

        MOLY_SETUP_6,
        "Mo/Tc Limit",
        1,

        MOLY_SETUP_7,
        "in",
        1,

        MOLY_SETUP_8,
        "CAPMAC for",
        0,
        
        
        CAL_TYPE_0_0,
        "       ",
        7,
        CAL_TYPE_1_0,
        "SYRINGE",
        7,
        CAL_TYPE_2_0,
        "VIAL   ",
        7,
        CAL_TYPE_3_0,
        "CAPSULE",
        7,

        CAL_TYPE_0_1,
        "       ",
        0,
        CAL_TYPE_1_1,
        "SYRINGE",
        0,
        CAL_TYPE_2_1,
        "VIAL   ",
        0,
        
        CAL_TYPE_3_1,
        "CAPSULE",
        0,

        PASSWORD,
        "ENTER PASSWORD",
        1,

        PRINTER_1,
        "PRINTER IS",
        1,
    
        PRINTER_TYPE_USB_EPSON,
        "USB: EPSON",
        1,

        PRINTER_TYPE_USB_HP,
        "USB: HP",
        1,

        PRINTER_TYPE_SLIP,
        "SLIP - TICKET",
        1,
        
        PRINTER_TYPE_ROLL,
        "ROLL",
        3,
        
        PRINTER_TYPE_OKI,
        "OKI",
        5,

        PRINTER_TYPE_LX,
        "LX-300",
        3,
        
        PRINTER_TYPE_NONE,
        "NONE",
        3,
        
        OKI_TICKET,
        "TICKET",
        3,
        
        OKI_LINE,
        "ONE LINE",
        2,
        
        CI_BQ_1,
        "Ci / Bq",
        3,

        CI_BQ_2,
        "FIXED TO Ci",
        1,

        CI_BQ_3,
        "FIXED TO Bq",
        1,

        CI_BQ_4,
        "SELECTABLE",
        1,

        TEST_SOURCE_1,
        "Do You Have",
        1,

        TEST_SOURCE_2,
        "?",
        9,

        TEST_SOURCE_3,
        "Use Daily ?",
        2,

        TEST_SOURCE_4,
        "SOURCE",
        2,

        TEST_SOURCE_5,
        "TOO WEAK",
        2,

        TEST_SOURCE_6,
        "CHAMBER",
        8,
        
        TEST_SOURCE_7,
        "COUNTER",
        8,
        
        TEST_SOURCE_8,
        "BOTH",
        8,

        TEST_SOURCE_9,
        "NO SOURCE",
        1,

        TEST_SOURCE_10,
        "S/N:",
        0,

        TEST_SOURCE_11,
        "DAILY",
        10,

        TEST_SOURCE_12,
        "ENTER S/N",
        1,

        TEST_SOURCE_13,
        "CALIBRATED",
        1,

        TEST_SOURCE_14,
        "CALIB ACTIVITY:",
        0,

        TEST_SOURCE_15,
        "In MBq",
        2,

        TEST_SOURCE_16,
        "TOO STRONG",
        2,

        TEST_SOURCE_17,
        "CONSTANCY TEST",
        0,

        TEST_SOURCE_18,
        "NO DAILY SOURCE",
        0,
        
        TEST_SOURCE_19,
        "WAS CHOSEN",
        0,
        
        TEST_SOURCE_20,
        "Use Daily for",
        0,

        TEST_SOURCE_21,
        "Pet Chambers ?",
        0,

        TEST_SOURCE_22,
        "R Chambers ?",
        0,

        TEST_SOURCE_23,
        "Constancy Test",
        1,

        TEST_SOURCE_24,
        "Not Automated",
        1,

        USER_KEY_1,
        "R-CHAMBER",
        7,
        
        USER_KEY_1A,
        "PET-CHAMBER",
        5,
        
        USER_KEY_2,
        "WELL",
        12,

        USER_KEY_3,
        "SELECT",
        1,

        USER_KEY_4,
        "USER KEY",
        1,


        USER_KEY_7,
        "EFF DATA",
        2,

        USER_KEY_8,
        "NOT ENTERED",
        2,

        USER_KEY_9,
        "FOR NUCLIDE",
        2,

        NUCLIDE_BUTTON_1,
        "NO SUCH",
        4,

        NUCLIDE_BUTTON_2,
        "NUCLIDE",
        4,

        NUCLIDE_BUTTON_3,
        "IN MEMORY",
        4,

        NUCLIDE_BUTTON_4,
        "SPECIFY",
        4,

        NUCLIDE_BUTTON_5,
        "FURTHER",
        4,
        
        NUCLIDE_BUTTON_6,
        "CAL #",
        2,

        NUCLIDE_BUTTON_7,
        "NOT ENTERED",
        2,

        NUCLIDE_BUTTON_8,
        "FOR NUCLIDE",
        2,

        AUTO_ZERO_1,
        "AUTO ZERO",
        3,

        AUTO_ZERO_2,
        "NO SOURCES",
        2,

        AUTO_ZERO_3,
        "PLEASE WAIT",
        2,

        AUTO_ZERO_4,
        "OUT OF RANGE",
        2,

        AUTO_ZERO_5,
        "  CAUTION ",
        3,

        AUTO_ZERO_6,
        "ZERO DRIFT",
        4,

        AUTO_ZERO_7,
        "    OK     ",
        3,

        PLEASE_WAIT,
        "PLEASE WAIT",
        2,

        ACC_1,
        "NO DAILY",
        1,

        ACC_2,
        "SOURCE DATA",
        1,

        ACC_3,
        "MEASURE",
        1,

        ACC_4,
        "S/N:",
        1,

        ACC_5,
        "NO SOURCE DATA",
        1,

        CONST_1,
        "NO CONSTANCY",
        1,

        CONST_2,
        "SOURCE",
        1,

        CONST_3,
        "Leave Source",
        1,

        CONST_4,
        "In For",
        1,

        CONST_5,
        "Constancy Test",
        1,

        CONST_8,
        "CONSTANCY TEST",
        1,

        CONST_9,
        "PASSED",
        1,

        BIAS_1,
        "CHAMBER VOLTAGE",
        0,

        BIAS_2,
        "REMOVE SOURCES",
        1,

        PRINT_1,
        "PRINT AGAIN?",
        1,

        DAILY_1,
        "DATA CHECK",
        3,

        DIAG_1,
        "DIAGNOSTICS",
        2,

        DIAG_14,
        "View List?",
        1,

        DIAG_15,
        "Chamber  :",
        0,

        DIAG_16,
        "S/N:",
        0,

        DIAG_17,
        "Resp. Corr.:",
        0,
        
        DIAG_18,
        "HIGH",
        11,

        DIAG_19,
        "LOW ",
        11,

        DIAG_20,
        "Nominal:",
        0,

        DIAG_21,
        "Counter: Well",
        0,

        DIAG_22,
        "Counter: Beta",
        0,

        DIAG_23,
        "Cal Fac:",
        0,
        
        DIAG_30,
        "Change Paper",
        1,

        DIAG_55,
        "PRINTING",
        0,

        DIAG_56,
        "DEVICE LIST",
        0,

        DIAG_57,
        "CHAMBER DATA",
        0,

        DIAG_58,
        "WELL DATA",
        0,

        DIAG_59,
        "BETA DATA",
        0,
        
        BKG_1,
        "MEASURE",
        4,

        BKG_2,
        "BACKGROUND",
        2,

        BKG_3,
        "NO SOURCES",
        2,

        BKG_4,
        "MEASURE BETA",
        1,

        BKG_5,
        "MEASURE WELL",
        1,

        BKG_6,
        "BACKGROUND NOT",
        1,

        BKG_7,
        "MEASURED TODAY",
        1,

        BKG_8,
        "USE LAST BKG?",
        1,

        BKG_9,    
        "TOO HIGH",
        7,

        MOLY_10,
        "CAUTION",
        1,

        MOLY_11,
        "MO HIGH",
        1,

        MOLY_14,
        "MO TOO HIGH",
        0,

        MOLY_15,
        "DO NOT USE",
        0,

        MOLY_16,
        "VOLUME?  ml",
        0,

        CARD_1,
        "Saving To Card",
        0,

        CARD_2,
        "Will Disable",
        0,

        CARD_3,
        "All Measurements",
        0,

        CARD_4,
        "Continue? Y or N",
        0,
        
        CARD_5,
          "Writing",
        1,

        CARD_6,
        "Daily Test",
        1,

        CARD_7,
        "Wrong Card",
        0,

        CARD_8,
        "Written",
        1,

        SAVER_1,
        "Screen is Off",
        0,

        SAVER_2,
        "Never",
        1,

        SAVER_3,
        "After 5 min",
        1,

        SAVER_4,
        "After 10 min",
        1,

        SAVER_5,
        "After 15 min",
        1,

        CONTRAST_1,
        "Use Arrows To",
        1,

        CONTRAST_2,
        "Adjust Screen",
        1,

        REMOTES_1,
        "Save Nuclides?",
        1,

        REMOTES_2,
        "List Full",
        1,

        ACT_SCR_1,
        " BKG",
        0,
        
        ACT_SCR_2,
        "TEST",
        0,

        ACT_SCR_3,
        " CAL",
        0,

        BETA_ACT_3,
        "WRONG NUCLIDE?",
        1,

        BETA_ACT_4,
        "RATIO",
        3,

        BETA_ACT_5,
        "HIGH",
        9,    

        BETA_ACT_6,
        "LOW",
        9,

        BETA_ACT_11,
        "IMPURITY IN",
        0,

        BETA_ACT_12,
        "ENTER FP",
        2,

        BETA_ACT_13,
        "ENTER FC",
        2,

        BETA_ACT_17,
        "Impurity Data",
        0,
        

        BETA_ACT_20,
        "No Impurity data",
        1,

        BETA_ACT_21,
        "for that nuclide",
        1,

        BETA_ACT_22,
        "Erase Impurity",
        1,

        BETA_ACT_23,
        "Data for:",
        1,

        BETA_ACT_24,
        "Write Over",
        0,
        
        LIMITS_1,
        "BKG LIMIT",
        2,

        LIMITS_2,
        "INPUT BACKGROUND",
        0,

        LIMITS_3,
        "LIMIT IN",
        0,

        LIMITS_4,
        "SHOW IMPURITY",
        1,

        LIMITS_5,
        "IF MORE THAN",
        1,

        LIMITS_6,
        "INPUT IMPURITY",
        0,

        LIMITS_7,
        "TO DISPLAY IN %",
        0,

        LIMITS_8,
        "% PRECISION",
        1,

        LIMITS_9,
        "NOT TESTED",
        1,

        LIMITS_10,
        "Test Precision?",
        0,

        LIMITS_11,
        "INPUT",
        2,

        LIMITS_12,
        "% PRECISION",
        1,

        CALIB_1,
        "Enter Data In",
        1,

        CALIB_2,
        "Setup",
        1,

        CALIB_3,
        "Test Source",
        1,

        CALIB_4,
        "AUTO CAL",
        2,

        CALIB_5,
        "Measure Cs137",
        1,
        
        CALIB_6,
        "Measure Ba133",
        1,

        CALIB_7,
        "AUT0",
        3,

        CALIB_8,
        "CALIBRATION",
        0,

        CALIB_9,
        "OUT OF RANGE",
        0,

        CALIB_10,
        "SEE MANUAL",
        2,

        CALIB_11,
        "IN PROGRESS",
        2,
        
        CALIB_12,
        "Count for",
        6,

        CALIB_13,
        "or more",
        6,

        CALIB_14,
        "Source Is",
        5,

        CALIB_15,
        "Too Weak",
        5,
        
        CALIB_19,
        "Energy",
        0,

        CALIB_20,
        "Deviation:",
        0,

        WELL_1,
        "net",
        11,

        EFF_1,
        "ALL CHANNELS",
        1,

        EFF_2,
        "ENTER %EFF",
        0,

        EFF_3,
        "FOR ALL CHANNELS",
        0,

        EFF_8,
        "No EFF data",
        1,

        EFF_9,
        "for that nuclide",
        0,

        EFF_10,
        "Erase EFF data",
        1,

        EFF_11,
        "for:",
        1,
        
        EFF_12,
        "Enter",
        0,

        EFF_13,
        "ACTIVITY",
        6,

        EFF_14,
        "IMPURITY",
        6,

        EFF_15,
        "% EFFICIENCY",
        1,

        EFF_16,
        "Will you use",
        0,

        EFF_17,
        "Corr. Factor",
        1,

        EFF_18,
        "Corr. Fac",
        7,

        EFF_19,
        "Calib Activity:",
        1,

        EFF_20,
        "in MBq",
        2,
        
        EFF_21,
        "Measure Source",
        0,

        EFF_22,
        "No Data",
        1,

        DET_1,    
        "DEAD TIME",
        0,

        DET_2,
        "Correct? Y or N",
        0,

        DET_3,    
        "INPUT DEAD TIME",
        0,

        DET_4,
        "CAL FACTOR",
        0,

        DET_5,
        "CAL FACTOR",
        1,
        
        DET_6,
        "Noise Ratio",
        2,

        DET_7,
        "HV DAC",
        4,

        DET_8,
        "RESET?",
       3,

        DET_9,
       "Input HV",
       1,
       
        DET_10,
       "AUTO CAL MUST",
       1,

       DET_11,
       "BE PERFORMED",
       1,

       MEAS_1,
       "MEASUREMENT",
       4,

       MEAS_2,
       "ERROR",
       4,
       

       MEAS_3,
       "DETECTOR",
       4,

       MEAS_4,
       "MALFUNCTION",
       4,

       TEST_1,
       "MUST PERFORM",
        2,

        TEST_2,
        "AUTO CAL",
        4,

        TEST_3,
        "BEFORE TEST",
        2,

        TEST_4,
        "No Data For",
        1,

        TEST_5,
        "Test Source",
        1,

        TEST_6,
        "See Manual",
        1,
        
        TEST_7,
        "No Eff Data",
        1,

        TEST_8,
        "at 10cm",
        1,

        TEST_9,
        "at 20cm",
        1,

        TEST_10,
        "Input Cs137",
        0,

        TEST_11,
        "Input Ba133",
        0,
        
        TEST_12,
        "Activity EFF",
        0,

        TEST_13,
        "TEST",
        6,

        TEST_14,
        "Measd:",
        0,

        TEST_15,
        "FAIL",
        2,

        LAB_1,
        "Schilling Test",
        0,

        LAB_2,
        "ALIQUOT:",
        0,

        LAB_3,
        "URINE  :",
        0,

        LAB_4,
        "EXCRETION:",
        0,

        LAB_5,
        "BKG  :",
        0,

        LAB_6,
        "STAND:",
        0,

        LAB_7,
        "URINE:",
        0,
        
        LAB_10,
        "BLOOD:",
        0,

        LAB_11,
        "PLSMA:",
        0,
        
        LAB_12,
        "SAMPL:",
        0,

        LAB_13,
        "ENTER DILUTION",
        1,

        LAB_14,
        "FACTOR",
        1,

        LAB_15,
        "Volume In ml",
        0,

        LAB_16,
        "SCHILLING TEST",
        1,

        LAB_17,
        "DICOPAC TEST",
        1,

        LAB_18,
        "PLASMA VOLUME",
        1,

        LAB_19,
        "RBC VOLUME",
        1,

        LAB_20,
        "MEASURE",
        1,

        LAB_21,
        "STANDARD",
        1,

        LAB_22,
        "STND",
        0,

        LAB_23,
        "SAMPLE",
        1,

        LAB_24,
        "SAMP",
        0,

        LAB_25,
        "MEASURE WHOLE",
        1,

        LAB_26,
        "BLOOD SAMPLE",
        1,

        LAB_27,
        "BLD",
        0,

        LAB_28,
        "PLASMA SAMPLE",
        1,

        LAB_29,
        "PLAS",
        0,

        LAB_30,
        "BLOOD STANDARD",
        1,
        
        LAB_31,
        "MEASURE PLASMA",
        1,

        LAB_32,
        "BKG",
        1,

        LAB_33,
        "MEASURE Co58",
        1,
        
        LAB_34,
        "MEASURE Co57",
        1,

        LAB_35,
        "MEASURE ROOM",
        1,

        LAB_36,
        "BACKGROUND",
        1,

        LAB_37,
        "Co58",
        1,

        LAB_38,
        "Co57",
        1,
        
        LAB_39,
        "Aliquot",
        7,

        LAB_40,
        "Urine",
        7,

        LAB_41,
        "Sample",
        7,

        LAB_42,
        "CALCULATED",
        1,

        LAB_43,
        "HEMATOCRIT",
        1,

        LAB_44,
        "ENTER WEIGHT",
        1,

        LAB_45,
        "In kg",
        1,

        LAB_46,
        "WHOLE BLOOD",
        0,

        LAB_47,
        "PLASMA",
        0,
        
        LAB_48,
        "HCT in %",
        1,

        LAB_49,
        "Dose",
        7,

        LAB_50,
        "Patient",
        7,

        LAB_51,
        "BLOOD VOLUME ",
        1,
        
        LAB_52,
        " RBC VOLUME  ",
        1,

        TRIG_1,
        "Input Trigger",
        0,

        TRIG_2,
        "Level In",
        0,

        TRIG_3,
        " BACKGROUND  ",
        2,

        TRIG_4,
        "    WIPE     ",
        2,

        TRIG_5,
        " UNRESTRICTED",
        2,

        TRIG_6,
        "SEALED SOURCE",
        2,
        
        NUCS_1,
        "WIPE",
        1,

        NUCS_2,
        "UNRESTRICTED",
        1,

        WIPE_1,
        "EXCEEDS",
        5,

        WIPE_2,
        "OK",
        5,

        LIN_1,
        "LINEARITY TEST",
        0,

        LIN_2,
        "ALREADY DEFINED",
        0,

        LIN_3,
        "NEW DEFINITION?",
        0,
        
        LIN_4,
        "ENTER NUMBER",
        0,

        LIN_5,
        "OF ",
        0,

        LIN_6,
        "TESTS",
        3,

        LIN_7,
        "TUBES",
        3,

        LIN_8,
        "STANDARD",
        0,

        LIN_9,
        "LINEARITY TEST",
        0,

        LIN_10,
        "TIME FOR",
        0,

        LIN_11,
        "MEASUREMENT ",
        0,

        LIN_12,
        "IN HOURS",
        0,

        LIN_19,
        "Must Define",
        0,

        LIN_20,
        "Linearity Test",
        0,

        LIN_29,
        "TEST NOT",
        0,

        LIN_30,
        "COMPLETE",
        0,

        LIN_31,
        "START NEW TEST?",
        0,
        

        DOSE_1,
        "PRINTING",
        3,

        DOSE_2,
        "Enter Dose",
        2,

        DOSE_3,
        "OUT OF RANGE",
        1,
        
        GEOM_1,
        "VOLUME",
        0,

        GEOM_2,
        "TOO SMALL",
        0,

        GEOM_3,
        "Another Sample?",
        0,
        
        GEOM_19,
        "TOO LARGE",
        0,
        
        INV_1,
        "ENTER ID",
        0,

        INV_2,
        "0 FOR NONE",
        0,

        INV_3,
        "ENTER LOT",
        0,

        INV_4,
        "NUMBER",
        0,
        
       INV_5,
       "ACTIVITY ?",
       1,

       INV_6,
       "INVENTORY",
       0,

       INV_7,
       "ENTER Mo/Tc",
       0,
       
       INV_14,
       "ENTER ACTIVITY",
       1,

       INV_15,
       "NO ROOM TO",
       1,

       INV_16,
       "ADD KIT",
       1,

       INV_17,
       "NO Tc FOR KITS",
       0,

       INV_18,
       "IN INVENTORY",
       1,

       INV_19,
       "ACTIVITY GREATER",
       0,

       INV_20,
       "THAN THAT IN",
       0,
       
       INV_22,
       "ACTIVITY",
       1,

       INV_23,
       "ENTER KIT",
       0,
       
       INV_24,
       "PRINT INVENTORY?",
       0,

       INV_25,
       "ARE YOU SURE",
       1,

       INV_26,
       "YOU WANT TO",
       1,

       INV_27,
       "DELETE WHOLE",
       1,

       INV_28,
       "INVENTORY ?",
       1,
       
        INV_38,
        "Date of Use",
        1,
       
        INV_39,
        "Time of Use",
        1,

        QC_28,
        "Do Another",
        1,

        QC_29,
        "Test? Y or N",
        1,

        PRINTER_2,
        "INSERT TICKET",
        1,

        PRINTER_3,
        "CHECK PRINTER",
        1,
      
    };       


//messages for printing
static const MESSAGE messages[] =
    {
        USER_NUCLIDES,
        "USER NUCLIDES:",

        USER_KEYS_MSG,
        "USER KEYS:",

        DOSE_4,
        "DOSE TABLE",

        DOSE_5,
        "Activity:",

        DOSE_6,
        "Volume:",

        DOSE_7,
        "Dose:",

        DOSE_8,
        "TIME",

        DOSE_9,
        "VOL (ml)",

        DOSE_10,
        "Required volume greater",

        DOSE_11,
        "than existing volume.",
        
        QC_30,
        "Results",

        QC_31,
        "Top    of Strip: ",

        QC_32,
        "Bottom of Strip: ",

        QC_33,
        "Top    / (Top + Bottom): ",

        QC_34,
        "Bottom / (Top + Bottom): ",
        
        QC_35,
        "Top    of Strip A: ",

        QC_36,
        "Bottom of Strip A: ",

        QC_37,
        "Top    of Strip B: ",

        QC_38,
        "Bottom of Strip B: ",

        QC_39,
        "Strip A: Free          = ",

        QC_40,
        "Strip B: R / H         = ",

        QC_41,
        "Purity: 100 -(F + R/H) = ",

        QC_42,
        "F = Free",

        QC_43,
        "R = Reduced",

        QC_44,
        "H = Hydrolized",
        
        QC_45,
        "Top    of Strip C: ",

        QC_46,
        "Bottom of Strip C: ",
        
        QC_47,
        "Free Tc99m              :",

        QC_48,
        "Hydrolized/Reduced Tc99m:",

        QC_49,
        "Tc99m HMPAO             :",
        
        QC_50,
        "Fraction #1      : ",

        QC_51,
        "Fraction #2      : ",

        QC_52,
        "Cartridge        : ",
       
        QC_53,
        "Non-elutable Tc99m       :",

        QC_54,
        "Hydrophilic Tc Impurities:",

        QC_55,
        "Tc99m Mertiatide         :",
        
        QC_56,
        "QUALITY CONTROL TEST",

        QC_57,   
        "Measured Values",

        QC_58,
        "Radiopharmaceutical: _________",

        QC_59,
        "Lot #: _______________________",

        QC_60,
        "Kit #: _______________________",
        
        QC_61,
        "Single Strip / Single Solvent",

        QC_62,
        "Two Strips / Two Solvents",

        QC_63,
        "Tc99m HMPAO (Exametazine)",

        QC_64,
        "Tc99m MAG3",

        INV_40,
        "INVENTORY",
        
        INV_41,
        "Lot #:",
        
        INV_42,
        "OVER",

        DAILY_2,
        "DAILY TESTS:",
       
        DAILY_3,
        "Chamber",

        DAILY_4,
        "ZERO:",

        DAILY_5,
        "ERROR ",

        DAILY_6,
        "BACKGROUND:",

        DAILY_7,
        "CHAMBER VOLTAGE:",

        DAILY_8,
        "Volts",

        DAILY_9,
        "DATA CHECK:",

        DAILY_10,
        "OK",

        DAILY_11,
        "MEMORY ERROR",
        
        
        DAILY_12,
        "signature",
        
        DAILY_13,
        "BY:",
        
        ACC_7,
        "ACCURACY TEST:",
        
        ACC_8,
        "Source:",
        
        ACC_9,
        "Serial #:",

        ACC_10,
        "S/N:",
        
        ACC_11,
        "Stand. Source(S):",

        ACC_12,
        "Measured As  (M):",

        ACC_13,
        "Deviation(M-S)/S:",

        CONST_10,
        "CONSTANCY TEST:",
        
        CONST_11,
        "Nuclide     Activity",

        MOLY_18,
        "Vol:",

        MOLY_19,
        "MO TOO HIGH - DO NOT USE",

        MOLY_20,
        "Expires",
        
        
        GEOM_13,
        "GEOMETRY TEST",

        GEOM_14,
        "Using:",

        GEOM_15,
        "Syringe",

        GEOM_16,
        "Vial",

        GEOM_17,
        "#   Volume    Assay    Variation",

        GEOM_18,
        "BASE",
        
        LIN_44,
        "CALICHECK TEST CALIBRATION",

        LIN_45,
        "LINEATOR TEST CALIBRATION ",


        LIN_46,
        "Tube:",

        LIN_47,
        "Activity:",

        LIN_48,
        "Calibration Factor:",

        LIN_49,
        "Initial Factor:",
                
        LIN_50,
        "LINEATOR LINEARITY TEST",

        LIN_51,
        "Present Factor:",

        LIN_52,
        "Ratio:",
        
        LIN_53,
        "CALICHECK LINEARITY TEST",

        LIN_54,
        "Mean Result:",

        LIN_55,
        "Result:",

        LIN_56,
        "Variation from Mean:",
        
       LIN_69,
       "Black Only",

       LIN_70,
       "Black + Red",

       LIN_71,
       "Black + Orange",

       LIN_72,
       "Black + Yellow",

       LIN_73,
       "Black + Green",

       LIN_74,
       "Black + Blue",

       LIN_75,
       "Black + Purple",

       LIN_76,
       "Black/Purple/Red",

       LIN_77,
       "Black/Purple/Orange",

       LIN_78,
       "Black/Purple/Yellow",

       LIN_79,
       "Black/Purple/Green",

       LIN_80,
       "Black/Purple/Blue",
       
       LIN_81,
       "STANDARD LINEARITY TEST",

       LIN_82,
       "Test:",

       LIN_83,
       "Predicted:",

       LIN_84,
       "Actual:",

       LIN_85,
       "Variation:",

       DIAG_2, 
       "DATA FOR",
       
        DIAG_3,
        "CHAMBERS",

        DIAG_4,
        "NUCLIDE DATA:",

        DIAG_5,
        "Correction Factors",

        DIAG_6,
        "Half-Life",
        
        DIAG_7,
        "Cal #",
        
        DIAG_8,
        "Syr     Vial     Cap",

        DIAG_9,
        "Channels      %Eff",

        DIAG_10,
        "Channels      %Eff     Ref     Syr     Vial",
        
        DIAG_11,
        "Cal#:",

        DIAG_12,
        "NONE",

        DIAG_13,
        "DEVICE LIST",
        
        DIAG_24,
        "Chamber  :",
        
        DIAG_25,
        "Type :",

        DIAG_26,
        "Response Correction:",

        DIAG_27,
        "was HIGH",

        DIAG_28,
        "was LOW ",
        
        DIAG_29,
        "Nominal Voltage:",
        
        DIAG_31,
        "Corr:",

        DIAG_32,
        "None",

        DIAG_33,
        "No Source",

        DIAG_34,
        "D: Daily  C: Constancy Source",
        
        DIAG_35,
        "Background:",

        DIAG_36,
        "Show % Impurity:",
        
        DIAG_37,
        "Precision:",
        
        DIAG_38,
        "Not Set",

        DIAG_39,
        "IMPURITY DATA:",
        
        DIAG_40,
        "Wipe:",

        DIAG_41,
        "Unrestricted:",

        DIAG_42,
        "Sealed Source:",
        
        DIAG_43,
        "LIMITS:",
        
        DIAG_44,
        "Counter: Well",

        DIAG_45,
        "Counter: Beta",

        DIAG_46,
        "Cal Factor:",
        
        DIAG_47,
        "Now:",

        DIAG_48,
        "TEST SOURCE :",

        DIAG_49,
        "CALIBRATION",

        DIAG_50,
        "CURRENT",

        DIAG_51,
        "NUCLIDE",

        DIAG_52,
        "S/N",

        DIAG_53,
        "DATE",

        DIAG_54,
        "ACTIVITY",
        
        BETA_ACT_25,
        "SYR",

        BETA_ACT_26,
        "VIAL",

        BETA_ACT_27,
        "sec",

        BETA_ACT_28,
        "Impurity:",

        BETA_ACT_29,
        "net",

        WIPE_3,
        "BACKGROUND",
        
        TEST_18,
        "TEST",

        TEST_19,
        "OF WELL",

        TEST_20,
        "OF BETA",

        TEST_21,
        "at",

        TEST_22,
        "Standard",

        TEST_23,
        "Counted for",

        TEST_24,
        "Measured as:",

        TEST_25,
        "Deviation:",
        
        TEST_26,
        "Energy Deviation:",

        TEST_27,
        "FAIL",
        
        TEST_28,
        "Gain Reference",
        
        CALIB_25,
        "AUTO CALIBRATION of",

        CALIB_26,
        "WELL",

        CALIB_27,
        "BETA",

        CALIB_28,
        "Calibrated By:",

        CALIB_29,
        "ERROR:",

        CALIB_30,
        "TOO MANY COUNTS:",

        CALIB_31,
        "IN UPPER CHANNEL",

        CALIB_32,
        "CALIBRATION FAILED",

        CALIB_33,
        "SEE MANUAL",

        
    };

static short find_small_message(short msg_num,SM_MESSAGE *msg);
static short find_medium_message(short msg_num,MED_MESSAGE *msg);
static short find_message(short msg_num,MESSAGE *msg);

    void display_small_message(short msg_num,short ypos, short plane,short rflag)
	{
		SM_MESSAGE msg;
        short xpos;

		if(find_small_message(msg_num,&msg) == -1)
			return;

        xpos = 6 * msg.col;
        display_text(xpos,ypos,&msg.str[0],plane,SMALL,rflag);


	}

    void display_medium_message(short msg_num,short ypos, short plane,short rflag)
    {
        MED_MESSAGE msg;
        short xpos;

        if(find_medium_message(msg_num,&msg) == -1)
            return;

        xpos = 8 * msg.col;
        display_text(xpos,ypos,&msg.str[0],plane,MEDIUM,rflag);
    }


		// find small font message 
    static short find_small_message(short msg_num,SM_MESSAGE *msg)
	{
		short index;
		short i;

		index = -1;
		for(i = 0; i < NUM_SMALL; i++)
		{
			memcpy(msg,&sm_messages[i],sizeof(*msg));
			if(msg -> id == msg_num)
			{
				index = i;
				break;
			}
		}

		if(index == -1)
		{
#if SYSCALLS
            printf("messge %d not found\r\n",msg_num);
#endif            
		}
		return (index);
	}
	
    //find medium font message
    static short find_medium_message(short msg_num,MED_MESSAGE *msg)
    {
        short index;
        short i;

        index = -1;
        for(i = 0; i < NUM_MEDIUM; i++)
        {
            memcpy(msg,&med_messages[i],sizeof(*msg));
            if(msg -> id == msg_num)
            {
                index = i;
                break;
            }
        }

        if(index == -1)
        {
#if SYSCALLS            
            printf("messge %d not found\r\n",msg_num);
#endif            
        }
        return (index);
    }

	



    //get message to be printed
    void get_message(short msg_num, char *str)
    {
        MESSAGE msg;
        int len;

        if(find_message(msg_num,&msg) == -1)
            return;

        len = strlen(&msg.str[0]);
        strncpy(str,&msg.str[0],len);

    }



        // find message for printing
    static short find_message(short msg_num,MESSAGE *msg)
    {
        short index;
        short i;

        index = -1;
        for(i = 0; i < NUM_STRINGS; i++)
        {
            memcpy(msg,&messages[i],sizeof(*msg));
            if(msg -> id == msg_num)
            {
                index = i;
                break;
            }
        }

        if(index == -1)
        {
#if SYSCALLS
            printf("messge %d not found\r\n",msg_num);
#endif            
        }
        return (index);
    }
    

    //get message for amulet
    void get_amulet_message(short msg_num, char *str)
    {
    	unsigned long int offset, length;
    	const unsigned long int *ptrNext, *ptrLast;

    	ptrLast = &(lang_map[NUM_AMULET_STRINGS - 1].off[1]);
    	offset = lang_map[msg_num].off[current.language];
    	ptrNext = &(lang_map[msg_num].off[current.language]);
    	if(ptrNext==ptrLast){
    		length = strlen(&(lang_str[offset]));
    	}else{
    		ptrNext++;
    		length = *ptrNext - offset;
    	}

    	strncpy(str, &(lang_str[offset]), length);
    	*(str + length) = 0;
    }
    
    void get_amulet_message_with_language(short msg_num, char *str, int language)
    {
    	unsigned long int offset, length;
    	const unsigned long int *ptrNext, *ptrLast;

    	ptrLast = &(lang_map[NUM_AMULET_STRINGS - 1].off[1]);
    	offset = lang_map[msg_num].off[language];
    	ptrNext = &(lang_map[msg_num].off[language]);
    	if(ptrNext==ptrLast){
    		length = strlen(&(lang_str[offset]));
    	}else{
    		ptrNext++;
    		length = *ptrNext - offset;
    	}

    	strncpy(str, &(lang_str[offset]), length);
    	*(str + length) = 0;
    }

    //L_UPDATE_SYSTEM 25, 51 -------------OK
    //L_ENTER_PASSWORD 25, 51 -------------OK
    //L_SETUP_R_HOTKEYS 25, 51 -------------OK
    //L_SETUP_PET_HOTKEYS 25, 51 -------------OK
    //L_ADVANCED_CHAMBER_SETUP_TITLE 25, 51 -------------OK
    //L_ENTER_NUCLIDE 25, 51 -------------OK
    //L_ENTER_ELEMENT 25, 51 -------------OK
    //L_ENTER_HALFLIFE 25, 51 -------------OK
    //L_ENTER_RCAL 25, 51 -------------OK
    //L_ENTER_PCAL 25, 51 -------------OK
    //L_SETUP_LINEARITY_ERROR 25, 51 -------------OK
    //L_FILL_IN_HOURS 25, 51 -------------OK
    //L_UPDATE_DISABLES_USB 51, 77 -------------OK
    //L_SELECT_DATE_FORMAT 25, 51 -------------OK
    //L_SELECT_LANGUAGE 25, 51 -------------OK
    //L_SETUP_LINEARITY_LINEATOR 25, 51 -------------OK
    //L_SETUP_LINEARITY_CALICHECK 25, 51 -------------OK
    //L_RESUME_AUTOLINEARITY_TEST 25, 51 -------------OK
    //L_PAUSE_AUTOLINEARITY_TEST 25, 51 -------------OK
    //L_PROCEED_TO_HOME_SCREEN 25, 51 -------------OK
    //L_ABORT_AUTOLINEARITY_TEST 25, 51 -------------OK
    //L_SAVE_AUTOLINEARITY_TEST 25, 51 -------------OK
    //L_CURRENT_AUTOLINEARITY_IS_PARTIALLY_COMPLETE 51, 77 -------------OK
    //L_INVALID_CAL_NUMBER 25, 51 -------------OK
    //L_USER_CAL_FULL 25, 51 -------------OK
    //L_SERIAL_NUMBER_ERROR 25, 51 -------------OK
    //L_DOSE_TIME_ERROR 25, 51 -------------OK
    //L_MEASURE_TEST_SOURCE 25, 51 -------------OK
    //L_CLEAR_SELECTED_NUCLIDE 25, 51 -------------FIXED
    //L_DOSE_DECAY_ENTRY 25, 51 -------------FIXED
    //L_PLEASE_SELECT_NUCLIDE 25, 51 -------------OK
    //L_ENTER_TEST_2_TIME 25, 51 -------------OK
    //L_ENTER_TEST_3_TIME 25, 51 -------------OK
    //L_ENTER_TEST_4_TIME 25, 51 -------------OK
    //L_ENTER_TEST_5_TIME 25, 51 -------------OK
    //L_ENTER_TEST_6_TIME 25, 51 -------------OK
    //L_ENTER_TEST_7_TIME 25, 51 -------------OK
    //L_ENTER_TEST_8_TIME 25, 51 -------------OK
    //L_ENTER_TEST_9_TIME 25, 51 -------------OK
    //L_ENTER_TEST_10_TIME 25, 51 -------------OK
    //L_ENTER_TEST_11_TIME 25, 51 -------------OK
    //L_ENTER_TEST_12_TIME 25, 51 -------------OK
    //L_SETUP_NUCLIDE_ERROR 25, 51 -------------OK
    //L_PLEASE_COMPLETE_PARTIAL_NUCLIDE_ENTRY 51, 77 -------------OK
    //L_DUPLICATE_NAME_FOUND 25, 51 -------------OK
    //L_ABOVE_MAXIMUM_ACTIVITY_CO57_BQ 77, 103 -------------OK
    //L_ABOVE_MAXIMUM_ACTIVITY_CO60_BQ 77, 103 -------------OK
    //L_ABOVE_MAXIMUM_ACTIVITY_BA133_CI 77, 103 -------------OK
    //L_ABOVE_MAXIMUM_ACTIVITY_BA133_BQ 77, 103 -------------OK
    //L_ABOVE_MAXIMUM_ACTIVITY_CS137_CI 77, 103 -------------OK
    //L_ABOVE_MAXIMUM_ACTIVITY_CS137_BQ 77, 103 -------------OK
    //L_ABOVE_MAXIMUM_ACTIVITY_NA22_BQ 77, 103 -------------OK
    //L_SETUP_LINEATOR_ERROR 25, 51 -------------OK
    //L_SETUP_CALICHECK_ERROR 25, 51 -------------OK
    //L_SETUP_LINEATOR 25, 51 -------------OK
    //L_SETUP_CALICHECK 25, 51 -------------OK
    //L_CH_SERIAL_OLD_NEW 25, 51 -------------OK
    //L_SETUP_REMOTE_NUCLIDES 25, 51 -------------OK
    //L_PLEASE_ENTER_CAL_NUM 25, 51 -------------OK
    //L_PLEASE_ENTER_DOSE_TIME 25, 51 -------------OK
    //L_DELETE_LINEATOR_SETTINGS 25, 51 -------------OK
    //L_DELETE_CALICHECK_SETTINGS 25, 51 -------------OK
    //L_DELETE_LINEATOR_SETTINGS_Q 25, 51 -------------OK
    //L_DECAY_CALCULATOR 25, 51 -------------OK
    //L_PLEASE_ENTER_ACTIVITY 25, 51 -------------OK
    //L_PLEASE_ENTER_START_TIME 25, 51 -------------OK
    //L_PLEASE_ENTER_END_TIME 25, 51 -------------OK
    //L_TOO_FAR_INTO_FUTURE 25, 51 -------------OK
    //L_TOO_FAR_INTO_PAST 25, 51 -------------OK
    //L_CHECK_CHAMBER_VOLTAGE 25, 51 -------------OK
    //L_CHECK_CHAMBER_VOLTAGE2 25, 51 -------------OK
    //L_DATA_CHECK 25, 51 -------------OK
    //L_NO_DAILY_SOURCE_DATA 25, 51 -------------OK
    //L_PLEASE_WAIT_2_MINUTES_FOR 25, 51 -------------OK
    //L_DATA_CHECK_PRINTED 25, 51 -------------OK
    //L_PLEASE_ENTER_VOLUME 25, 51 -------------OK
    //L_PLEASE_ENTER_DOSE 25, 51 -------------OK
    //L_DOSE_TABLE_ERROR 25, 51 -------------OK
    //L_PR_USER_CHAMBER_NUCLIDES 25, 51 -------------OK
    //L_ADD_INVENTORY_ITEM 25, 51 -------------OK
    //L_PLEASE_ENTER_LOT 25, 51 -------------OK
    //L_PLEASE_ACCEPT_ACTIVITY 25, 51 -------------OK
    //L_PLEASE_ENTER_MOTC_RATIO 25, 51 -------------OK
    //L_PLEASE_ENTER_VOLUME_ML 25, 51 -------------OK
    //L_PLEASE_SELECT_STUDY 25, 51 -------------OK
    //L_DELETE_ALL_INVENTORY_Q 25, 51 -------------OK
    //L_PET_CHAMBER_NO_INVENTORY 51, 77 -------------OK
    //L_TC_LEVEL_TOO_LOW 25, 51 -------------FIXED
    //L_PLEASE_WITHDRAW_ML 25, 51 -------------OK
    //L_PLEASE_MEASURE_WITHDRAWAL 25, 51 -------------OK
    //L_PLEASE_ENTER_TIME_OF_USE 25, 51 -------------OK
    //L_UNABLE_TO_FIND_CHAMBER 25, 51 -------------OK
    //L_UNABLE_TO_FIND_NUCLIDE 25, 51 -------------OK
    //L_CHAMBER_SN_IS_INCORRECT 25, 51 -------------OK
    //L_ENTER_START_DATE 25, 51 -------------OK
    //L_MAKE_INVENTORY_KIT 25, 51 -------------OK
    //L_SOURCE_ID 25, 51 -------------OK
    //L_KIT_VOLUME_LESS 25, 51 -------------OK
    //L_DRAW_FROM_SOURCE 25, 51 -------------OK
    //L_MEASURE_CANISTER_BKG 25, 51 -------------OK
    //L_ACTIVITY_TOO_LOW_ERROR 25, 51 -------------OK
    //L_DO_NOT_USE_AFTER_HOURS 25, 51 -------------OK
    //L_MO_TOO_HIGH_DO_NOT_USE 25, 51 -------------OK
    //L_NONE_FOUND 77, 103 -------------OK
    //L_REDUCED_HYDROLIZED 25, 51 -------------OK
    //L_HYDROLIZED_REDUCED 25, 51 -------------OK
    //L_QUALITY_CONTROL_TEST 25, 51 -------------OK
    //L_TOP_TOP_BOTTOM 25, 51 -------------OK
    //L_BOTTOM_TOP_BOTTOM 25, 51 -------------OK
    //L_HYDROLIZED_REDUCED_TC99M 25, 51 -------------OK
    //L_HYDROPHILIC_IMPURE 25, 51 -------------OK
    //L_INACTIVATE_RECORD2 25, 51 -------------OK
    //L_PLEASE_ENTER_COMMENT 25, 51 -------------OK
    //L_NO_SETTINGS 25, 51 -------------OK
    //L_FOR_R_CHAMBER 25, 51 -------------OK
    //L_FOR_PET_CHAMBER 25, 51 -------------OK
    //L_STANDARD_LINEARITY_TEST 25, 51 -------------OK
    //L_LINEATOR_LINEARITY_TEST 25, 51 -------------OK
    //L_CALICHECK_LINEARITY_TEST 25, 51 -------------OK
    //L_PC_COMMUNICATIONS_MODE 25, 51 -------------OK
    //L_RUNNING_QC_TESTS_FROM_PC 25, 51 -------------OK
    //L_ABORT_QC_TESTS 25, 51 -------------OK
    //L_CHAMBER_VOLTAGE_FAILED 25, 51 -------------OK
    //L_PET_CHAMBER_CANNOT_RUN_MOLY_ASSAY 51, 77 -------------OK
    //L_UNABLE_TO_FIND_FILE 25, 51 -------------OK
    //L_PLACE_ONLY_ONE_UPDATE 25, 51 -------------OK
    //L_FILE_IN_ROOT_FOLDER 25, 51 -------------OK
    //L_INVALID_UPDATE_FILE 25, 51 -------------OK
    //L_CORRUPT_UPDATE_FILE 25, 51 -------------OK
    //L_USB_DRIVE_ERROR 25, 51 -------------OK
    //L_SOFTWARE_UPDATE 25, 51 -------------OK
    //L_FILE_TRANSFER_ERROR 25, 51 -------------OK
    //L_POWER_OFF_AND_THEN_POWER_ON_TO_COMPLETE_UPDATE 51, 77 -------------OK
    //L_CHECKSUM_HAS_FAILED 25, 51 -------------OK


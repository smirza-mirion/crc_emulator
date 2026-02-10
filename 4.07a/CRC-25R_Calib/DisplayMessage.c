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
		"Rév ",
		
		L_PR_REV,
		"Rev:",
		"Rév :",

		L_REV_SN,
		"REV       SN: ",
		"RÉV       SN : ",

		L_COPYRIGHT,
		"COPYRIGHT",
		"DROITS D'AUTEUR",

		L_RIGHTS,
		"ALL RIGHTS RESERVED",
		"TOUS DROITS RÉSERVÉS",

		L_CONTINUE_BTN,
		"Continue",
		"Continuer",

		L_LOGIN,
		"Login",
		"Connexion",

		L_UPDATE_SYSTEM,
		"Update System",
		"Mise à jour système",

		L_TIMEOUT_OFF,
		"OFF",
		"NON",

		L_BRIGHTNESS,
		"Brightness:",
		"Luminosité :",

		L_SLEEP_TIMEOUT,
		"Sleep Timeout:",
		"Délai de veille :",

		L_SLEEP_BRIGHTNESS,
		"Sleep Brightness:",
		"Luminosité veille :",

		L_VOLUME,
		"Volume:",
		"Volume :",

		L_SETUP_TITLE,
		"Setup",
		"Configuration",

		L_ADVANCED_CHAMBER,
		"Advanced Chamber",
		"Chambre avancée",

		L_ADVANCED_DETECTOR,
		"Advanced Detector",
		"Détecteur avancé",

		L_STAFF,
		"Staff",
		"Personnel",

		L_ENTER_PASSWORD,
		"Please Enter Password:",
		"Veuillez entrer le mot de passe :",

		L_TEST,
		"Test",
		"Test",

		L_TEST2,
		"Test:",
		"Test :",

		L_ACTIVITY_UNIT,
		"Activity Unit:",
		"Unité d'activité :",

		L_SETUP_DATE_FORMAT,
		"Date Format:",
		"Format de date :",
		
		L_SETUP_PRINTER,
		"Printer:",
		"Imprimante :",

		L_SETUP_SOURCES,
		"Setup Sources",
		"Config. sources",	//Changed

		L_SETUP_MOLY,
		"Setup Moly",
		"Config. Moly",	//Changed

		L_SETUP_NUCLIDES,
		"Setup Nuclide",
		"Config. nucléide",		//Changed

		L_SETUP_CALNUM,
		"Setup CalNum",
		"Config. CalNum",		//Changed

		L_SETUP_LINEARITY,
		"Setup Linearity",
		"Config. linéarité",	//Changed

		L_SETUP_REMOTE,
		"Setup Remote",
		"Config. distant",	//Changed

		L_SETUP_R_HOTKEYS,
		"Setup R Chamber Hotkeys",
		"Config. raccourcis chambre R",

		L_SETUP_PET_HOTKEYS,
		"Setup PET Chamber Hotkeys",
		"Config. raccourcis chambre PET",

		L_ADVANCED_CHAMBER_SETUP_TITLE,
		"Advanced Chamber Setup",
		"Config. chambre avancée",	//Changed

		L_SETUP_SOURCES_ACTIVITY,
		"ACTIVITY",
		"ACTIVITÉ",

		L_SETUP_SOURCES_DATE,
		"DATE",
		"DATE",

		L_SETUP_SOURCES_DAILY,
		"DAILY",
		"QUOTIDIEN",		//Changed

		L_SETUP_SOURCES_CONSTANCY_CHANNELS,
		"Constancy Channels",
		"Canaux de constance",		//Changed

		L_CLEAR_BUTTON,
		"Clear",
		"Effacer",

		L_ACCEPT_BUTTON,
		"Accept",
		"Accepter",			//Changed

		L_CANCEL_BUTTON,
		"Cancel",
		"Annuler",

		L_SETUP_MOLY_METHOD,
		"Moly Method:",
		"Méthode Moly :",

		L_SETUP_MOLY_LIMIT,
		"Mo/Tc Limit:",
		"Limite Mo/Tc :",

		L_NUCLIDE,
		"Nuclide",
		"Nucléide",

		L_SETUP_NUCLIDE_ELEMENT,
		"Element",
		"Élément",

		L_SETUP_NUCLIDE_HALFLIFE,
		"Halflife",
		"Demi-vie",
		
		L_ENGLISH,
		"English",
		"Anglais",

		L_FRENCH,
		"French",
		"Français",
		
		L_LANGUAGE,
		"Language:",
		"Langue :",

		L_DEFAULT,
		"Default",
		"Par défaut",

		L_USER,
		"User",
		"Utilisateur",

		L_ENTER_NUCLIDE,
		"Please Enter Nuclide",
		"Veuillez entrer le nucléide",

		L_ENTER_ELEMENT,
		"Please Enter Element",
		"Veuillez entrer l'élément",

		L_ENTER_HALFLIFE,
		"Please Enter Halflife",
		"Veuillez entrer la demi-vie",

		L_ENTER_RCAL,
		"Please Enter R Cal #:",
		"Veuillez entrer R Cal # :",
		
		L_ENTER_PCAL,
		"Please Enter P Cal #:",
		"Veuillez entrer P Cal # :",
		
		L_BACKSPACE,
		"Backspace",
		"Retour arrière",

		L_BACKSPACE_SHORT,
		"BackSP",
		"Retour",

		L_SHIFT,
		"Shift",
		"Shift",

		L_WILDCARD,
		"<-- Wildcard",
		"<-- Joker",

		L_PLEASE_ENTER,
		"Please Enter",
		"Veuillez entrer",

		L_SN,
		"S/N",
		"S/N",

		L_SN2,
		"S/N:",
		"S/N :",

		L_CALIBRATION_TIME,
		"Calibration Time",
		"Heure de calibration",

		L_ACTIVITY,
		"Activity",
		"Activité",

		L_STANDARD_R_CHAMBER,
		"Standard R Chamber",
		"Standard chambre R",

		L_STANDARD_PET_CHAMBER,
		"Standard PET Chamber",
		"Standard chambre PET",

		L_SETUP_LINEARITY_STANDARD_R,
		"Setup Linearity Standard - R Chamber",
		"Config. standard linéarité - chambre R",

		L_SETUP_LINEARITY_STANDARD_PET,
		"Setup Linearity Standard - PET Chamber",
		"Config. standard linéarité - chambre PET",

		L_NUM_OF_MEASUREMENTS,
		"Num of Measurements:",
		"Nb de mesures :",

		L_MEASURED_ON,
		"Measured On",
		"Mesuré le",

		L_SETUP_LINEARITY_ERROR,
		"Setup Linearity Error",
		"Erreur config. linéarité",

		L_FILL_IN_HOURS,
		"Please Fill in All Hours",
		"Veuillez remplir toutes les heures",

		L_ASCENDING_ORDER,
		"Please enter hours in ascending order",
		"Veuillez entrer les heures en ordre croissant",
		
		L_CAPS_OK,
		"OK",
		"OK",

		L_SELECT_WIPE_CRITERIA,
		"Please select Wipe Criteria",
		"Sélectionnez les critères de frottis",

		L_ALL_WIPES,
		"All Wipes",
		"Tous les frottis",

		L_WORK_AREA,
		"Work Area",
		"Zone de travail",

		L_UNRESTRICTED_AREA,
		"Unrestricted Area",
		"Zone non restreinte",

		L_SEALED_SOURCE,
		"Sealed Source",
		"Source scellée",

		L_PACKAGE,
		"Package",
		"Colis",

		L_HIGH_ACTIVITY,
		"High Activity",
		"Haute activité",

		L_SELECT_DETECTOR,
		"Please select Detector",
		"Veuillez sélectionner le détecteur",

		L_PROBE,
		"Probe",
		"Sonde",

		L_WELL,
		"Well",
		"Puits",

		L_SELECT_BRANDING,
		"Please select Branding",
		"Veuillez sélectionner la marque",

		L_ALL,
		"All",
		"Tout",

		L_SELECT_TU_METHOD,
		"Please select Counting Method",
		"Veuillez sélectionner la méthode de comptage",

		L_DECAY_CORRECT_ADMIN_DOSE,
		"Decay Correct Administered Dose",
		"Corriger décroissance dose administrée",

		L_MEAS_REF_DOSE,
		"Measure Same Reference Dose Before Each Uptake",
		"Mesurer la même dose réf. avant chaque captation",

		L_SELECT_DOSE_MEASUREMENT,
		"Select Dose Measurement",
		"Sélectionner mesure de dose",

		L_DOSE_MEAS_IS_DOSE_ADMIN,
		"Dose Measured is Dose Administered",
		"Dose mesurée est dose administrée",

		L_MEAS_DOSE_AND_ADD,
		"Measure Each Dose and Add Activity",
		"Mesurer chaque dose et ajouter l'activité",

		L_MEAS_DOSE_AND_MULTIP,
		"Measure One Capsule and Multiply by Number Administered",
		"Mesurer une capsule et multiplier par le nombre administré",

		L_MEAS_LIQ_AND_MULTIP,
		"Measure Liquid and Multiply by Factor",
		"Mesurer le liquide et multiplier par le facteur",

		L_SELECT_NUCLIDE,
		"Select Nuclide",
		"Sélectionner nucléide",

		L_SELECT_DOSE_FORM,
		"Select Dose Form",
		"Sélectionner forme de dose",

		L_CAPSULE,
		"Capsule",
		"Capsule",

		L_LIQUID,
		"Liquid",
		"Liquide",

		L_MEAS_PREDOSE_PATIENT,
		"Measure Pre-Dose Patient?",
		"Mesurer patient pré-dose ?",

		L_YES,
		"Yes",
		"Oui",

		L_NO,
		"No",
		"Non",

		L_DELETE_THYROID,
		"Delete Thyroid Uptake Test?",
		"Supprimer test captation thyroïde ?",

		L_MEAS_REDISUAL,
		"Measure Residual Dose?",
		"Mesurer dose résiduelle ?",

		L_REACTIVATE_TU,
		"ReActivate Thyroid Uptake Test?",
		"Réactiver test captation thyroïde ?",

		L_ONLY_DEFAULT_NUC,
		"Use Only Default Nuclide keV?",
		"Utiliser nucléide keV par défaut seul ?",

		L_REACTIVATE_RBC,
		"ReActivate RBC Survival Test?",
		"Réactiver test de survie RBC ?",

		L_DELETE_RBC,
		"Delete RBC Survival Test?",
		"Supprimer test de survie RBC ?",

		L_UPDATE_TO_REV,
		"Update to Rev: %s, Proceed?",
		"Mise à jour rév : %s, continuer ?",

		L_UPDATE_DISABLES_USB,
		"This version will disable USB Update. Proceed?",
		"Cette version désactive la mise à jour USB. Continuer ?",

		L_OVERWRITE_SETTING_WITH_DEFAULT,
		"Overwrite settings with defaults from %s?",
		"Écraser les réglages par défaut de %s ?",

		L_SELECT_DATE_FORMAT,
		"Please Select Date Format",
		"Veuillez sélectionner le format de date",

		L_SELECT_USB_DRIVER,
		"Please Select USB PC Driver",
		"Veuillez sélectionner le pilote USB PC",

		L_LEGACY,
		"Legacy",
		"Ancien",

		L_CDC,
		"CDC",
		"CDC",

		L_SELECT_DOSE_DECAY_ENTRY_MODE,
		"Please Select Dose Decay Entry Mode",
		"Sélectionnez le mode saisie décroissance",	//Changed

		L_QUICK,
		"Quick",
		"Rapide",

		L_FULL,
		"Full",
		"Complet",

		L_SELECT_LANGUAGE,
		"Please Select Language",
		"Veuillez sélectionner la langue",

		L_55T_FULL_TITLE,
		"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",
		"CRC-55t      CALIBRATEUR DE DOSE RADIOISOTOPE",

		L_CAPRAC_FULL_TITLE,
		"CAPRAC-t        RADIOISOTOPE WELL COUNTER",
		"CAPRAC-t        COMPTEUR PUITS RADIOISOTOPE",

		L_700T_FULL_TITLE,
		"CAPTUS-700t          RADIOISOTOPE COUNTER",
		"CAPTUS-700t          COMPTEUR RADIOISOTOPE",

		L_FACTORY,
		"Factory",
		"Usine",

		L_PLEASE_RESTART,
		"Setting has been saved. Please restart.",
		"Réglage enregistré. Veuillez redémarrer.",

		L_WIPE_TYPE_DEFAULTS,
		"Wipe Type Defaults",
		"Défauts type de frottis",

		L_WIPE_OVERWRITE_STRING,
		"Current settings have been overwritten with %s default values",
		"Les réglages ont été écrasés avec les valeurs par défaut %s",

		L_SETUP_LINEARITY_LINEATOR,
		"Setup Linearity Lineator",
		"Config. linéarité Lineator",

		L_SETUP_LINEARITY_CALICHECK,
		"Setup Linearity Calicheck",
		"Config. linéarité Calicheck",

		L_TUBE,
		"Tube",
		"Tube",

		L_TUBE2,
		"Tube:",
		"Tube :",

		L_LINEATOR_SERIAL,
		"Lineator Serial:",
		"Lineator S/N :",

		L_CALICHECK_SERIAL,
		"Calicheck Serial:",
		"Calicheck S/N :",

		L_PRINT,
		"Print",
		"Imprimer",

		L_DELETE_CURRENT_SETTINGS,
		"Delete Current Settings",
		"Supprimer réglages actuels",

		L_PLEASE_ENTER_LINEATOR_SN,
		"Please Enter Lineator Serial #",
		"Veuillez entrer le # de série Lineator",	//Changed
		
		L_PLEASE_ENTER_CALICHECK_SN,
		"Please Enter Calicheck Serial #",
		"Veuillez entrer le # de série Calicheck",	//Changed

		L_PR_NUCLIDE,
		"NUCLIDE",
		"NUCLÉIDE",		//Changed

		L_PR_CHAMBER_SN,
		"Chamber   S/N:",
		"Chambre   S/N :",

		L_CAL_1,
		"1)Black:",
		"1)Noir :",

		L_CAL1,
		"1)Black",
		"1)Noir",

		L_CAL__1,
		"Black",
		"Noir",

		L_CAL_2,
		"2)Black + Red:",
		"2)Noir + Rouge :",

		L_CAL2,
		"2)Black/Red",
		"2)Noir/Rouge",

		L_CAL__2,
		"Black + Red",
		"Noir + Rouge",

		L_CAL_3,
		"3)Black + Orange:",
		"3)Noir + Orange :",

		L_CAL3,
		"3)Black/Orange"
		"3)Noir/Orange",

		L_CAL__3,
		"Black + Orange"
		"Noir + Orange",

		L_CAL_4,
		"4)Black + Yellow:",
		"4)Noir + Jaune :",

		L_CAL4,
		"4)Black/Yellow",
		"4)Noir/Jaune",

		L_CAL__4,
		"Black + Yellow",
		"Noir + Jaune",

		L_CAL_5,
		"5)Black + Green:",
		"5)Noir + Vert :",

		L_CAL5,
		"5)Black/Green",
		"5)Noir/Vert",

		L_CAL__5,
		"Black + Green",
		"Noir + Vert",

		L_CAL_6,
		"6)Black + Blue:",
		"6)Noir + Bleu :",

		L_CAL6,
		"6)Black/Blue",
		"6)Noir/Bleu",

		L_CAL__6,
		"Black + Blue",
		"Noir + Bleu",

		L_CAL_7,
		"7)Black + Purple:",
		"7)Noir + Violet :",

		L_CAL7,
		"7)Black/Purple",
		"7)Noir/Violet",

		L_CAL__7,
		"Black + Purple",
		"Noir + Violet",

		L_CAL_8,
		"8)Black + Purple",
		"8)Noir + Violet",

		L_CAL_82,
		"+ Red:",
		"+ Rouge :",

		L_CAL8,
		"8)Black/Purple/Red",
		"8)Noir/Violet/Rouge",

		L_CAL__8,
		"Black/Purple/Red",
		"Noir/Violet/Rouge",

		L_CAL_9,
		"9)Black + Purple",
		"9)Noir + Violet",

		L_CAL_92,
		"+ Orange:",
		"+ Orange :",
		
		L_CAL9,
		"9)Blk/Purple/Orange",
		"9)Noir/Violet/Orange",

		L_CAL__9,
		"Black/Purple/Orange",
		"Noir/Violet/Orange",

		L_CAL_10,
		"10)Black + Purple",
		"10)Noir + Violet",

		L_CAL_102,
		"+ Yellow:",
		"+ Jaune :",		//Changed

		L_CAL10,
		"10)Blk/Purpl/Yellow",
		"10)Noir/Violet/Jaune",	//Changed

		L_CAL__10,
		"Black/Purple/Yellow",
		"Noir/Violet/Jaune",	//Changed

		L_CAL_11,
		"11)Black + Purple",
		"11)Noir + Violet",

		L_CAL_11_2,
		"+ Green:",
		"+ Vert :",

		L_CAL11,
		"11)Blk/Purple/Green",
		"11)Noir/Violet/Vert",

		L_CAL__11,
		"Black/Purple/Green",
		"Noir/Violet/Vert",

		L_CAL_12,
		"12)Black + Purple",
		"12)Noir + Violet",
		
		L_CAL_12_2,
		"+ Blue:",
		"+ Bleu :",
		
		L_CAL12,
		"12)Blk/Purple/Blue",
		"12)Noir/Violet/Bleu",

		L_CAL__12,
		"Black/Purple/Blue",
		"Noir/Violet/Bleu",

		L_YES_ENGLISH,
		"Yes",
		"Yes",

		L_NO_ENGLISH,
		"No",
		"No",

		L_COMMENT,
		"Comment:",
		"Commentaire :",

		L_RESUME_AUTOLINEARITY_TEST,
		"Resume AutoLinearity Test",
		"Reprendre test AutoLinéarité",

		L_ERROR,
		"ERROR",
		"ERREUR",

		L_ERROR2,
		"Error",
		"Erreur",

		L_UNABLE_TO_FIND_CHAMBER_WITH_SN,
		"Unable to find chamber with S/N: %s",
		"Impossible de trouver la chambre S/N : %s",

		L_ABORT_CURRENT_AUTOLINEARITY_TEST,
		"Abort current AutoLinearity Test?",
		"Abandonner test AutoLinéarité en cours ?",

		L_ABORT_UNABLE_TO_FIND_USABLE_NUCLIDE,
		"Unable to find usable Nuclide: %s",
		"Nucléide utilisable introuvable : %s",

		L_COMPLETED_AUTOLINEARITY_NOT_SAVED,
		"Completed AutoLinearity Test has not been saved.",
		"Le test AutoLinéarité terminé n'a pas été enregistré.",

		L_PROCEED_TO_AUTOLINEARITY,
		"Proceed to AutoLinearity Test?",
		"Procéder au test AutoLinéarité ?",

		L_PLEASE_PLACE_NUCLIDE,
		"Please place Nuclide: %s into Chamber: %d",
		"Veuillez placer le nucléide : %s dans la chambre : %d",

		L_RESUME_AUTOLINEARITY,
		"Resume AutoLinearity Test?",
		"Reprendre test AutoLinéarité ?",

		L_PAUSE_AUTOLINEARITY_TEST,
		"Pause AutoLinearity Test",
		"Pause test AutoLinéarité",

		L_AUTOLINEARITY_HAS_BEEN_PAUSED,
		"AutoLinearity has been paused.",
		"AutoLinéarité a été mise en pause.",

		L_COMPLETED_AUTOLINEARITY_HAS_NOT_BEEN_SAVED,
		"Completed AutoLinearity Test has not been saved.",
		"Le test AutoLinéarité terminé n'a pas été enregistré.",

		L_TO_AVOID_MISSING_MEASUREMENTS,
		"To avoid missing measurements, ",
		"Pour éviter de manquer des mesures, ",

		L_PLEASE_RETURN_TO_AUTOLINEARITY,
		"Please return to AutoLinearity as soon as possible.",
		"Veuillez revenir à AutoLinéarité dès que possible.",

		L_PROCEED_TO_HOME_SCREEN,
		"Proceed to Home Screen?",
		"Aller à l'écran d'accueil ?",

		L_PROCEED_TO_AUTOLINEARITY_MENU,
		"Proceed to AutoLinearity Menu?",
		"Aller au menu AutoLinéarité ?",

		L_ABORT_AUTOLINEARITY_TEST,
		"Abort AutoLinearity Test",
		"Abandonner test AutoLinéarité",

		L_ABORT_AUTOLINEARITY_TEST2,
		"Abort AutoLinearity Test?",
		"Abandonner test AutoLinéarité ?",

		L_CURRENT_AUTOLINEARITY_TEST_HAS_FINISHED,
		"Current AutoLinearity Test has finished.",
		"Le test AutoLinéarité en cours est terminé.",

		L_ERASE_CURRENT_TEST,
		"Erase Current Test?",
		"Effacer le test en cours ?",

		L_SAVE_AUTOLINEARITY_TEST,
		"Save AutoLinearity Test",
		"Enregistrer test AutoLinéarité",

		L_SAVE_CURRENT_TEST,
		"Save Current Test?",
		"Enregistrer le test en cours ?",

		L_CURRENT_AUTOLINEARITY_IS_PARTIALLY_COMPLETE,
		"Current AutoLinearity Test is partially complete.",
		"Le test AutoLinéarité en cours est partiellement terminé.",

		L_END_AND_SAVE_TEST,
		"End and Save Current Test?",
		"Terminer et enregistrer test ?",

		L_MAXIMUM_IS,
		"Maximum is %.4f",
		"Maximum est %.4f",

		L_INPUT_ERROR,
		"Input Error",
		"Erreur de saisie",

		L_MINIMUM_IS,
		"Minimum is %.4f",
		"Minimum est %.4f",

		L_CAL_NUMBER_ERROR,
		"Cal Number Error",
		"Erreur numéro Cal",

		L_INVALID_CAL_NUMBER,
		"Invalid Cal Number",
		"Numéro Cal invalide",

		L_USER_CAL_FULL,
		"User Cal Full",
		"Cal utilisateur plein",

		L_PLEASE_ENTER_CURRENT_TIME,
		"Please Enter Current Time:",
		"Veuillez entrer l'heure actuelle :",

		L_SERIAL_NUMBER_ERROR,
		"Serial Number Error",
		"Erreur numéro de série",

		L_SERIAL_NUMBER_MUST_HAVE_SIX_DIGITS,
		"Serial number must have six digits",
		"Le numéro de série doit avoir six chiffres",

		L_DOSE_TIME_ERROR,
		"Dose Time Error",
		"Erreur heure de dose",

		L_INVALID_DATE,
		"Invalid Date",
		"Date invalide",

		L_INVALID_TIME,
		"Invalid Time",
		"Heure invalide",

		L_MAX_HOUR_IS_23,
		"Max Hour is 23",
		"Heure max est 23",

		L_MAX_MINUTE_IS_59,
		"Max Minute is 59",
		"Minute max est 59",

		L_MIN_DAY_IS_1,
		"Min Day is 1",
		"Jour min est 1",

		L_MAX_DAY_IS_31,
		"Max Day is 31",
		"Jour max est 31",

		L_MIN_MONTH_IS_1,
		"Min Month is 1",
		"Mois min est 1",

		L_MAX_MONTH_IS_12,
		"Max Month is 12",
		"Mois max est 12",

		L_MAX_YEAR_IS_2030,
		"Max Year is 2030",
		"Année max est 2030",

		L_INVALID_YEAR,
		"Invalid Year",
		"Année invalide",

		L_TUBE_1_LINEATOR,
		"Tube #1: (1)",
		"Tube #1 : (1)",

		L_TUBE_1_LINEATOR2,
		"1) Tube 1",
		"1) Tube 1",

		L_TUBE_2_LINEATOR,
		"Tube #2: (1 + 2)",
		"Tube #2 : (1 + 2)",

		L_TUBE_2_LINEATOR2,
		"2) Tube 1+2",
		"2) Tube 1+2",

		L_TUBE_3_LINEATOR,
		"Tube #3: (1 + 3)",
		"Tube #3 : (1 + 3)",

		L_TUBE_3_LINEATOR2,
		"3) Tube 1+3",
		"3) Tube 1+3",

		L_TUBE_4_LINEATOR,
		"Tube #4: (1 + 2 + 3)",
		"Tube #4 : (1 + 2 + 3)",

		L_TUBE_4_LINEATOR2,
		"4) Tube 1+2,3",
		"4) Tube 1+2,3",

		L_TUBE_5_LINEATOR,
		"Tube #5: (1 + 4)",
		"Tube #5 : (1 + 4)",

		L_TUBE_5_LINEATOR2,
		"5) Tube 1+4",
		"5) Tube 1+4",

		L_TUBE_6_LINEATOR,
		"Tube #6: (1 + 2 + 4)",
		"Tube #6 : (1 + 2 + 4)",

		L_TUBE_6_LINEATOR2,
		"6) Tube 1+2,4",
		"6) Tube 1+2,4",

		L_TUBE_7_LINEATOR,
		"Tube #7: (1 + 3 + 4)",
		"Tube #7 : (1 + 3 + 4)",

		L_TUBE_7_LINEATOR2,
		"7) Tube 1+3,4",
		"7) Tube 1+3,4",

		L_TUBE_8_LINEATOR,
		"Tube #8: (1 + 2 + 3 + 4)",
		"Tube #8 : (1 + 2 + 3 + 4)",

		L_TUBE_8_LINEATOR2,
		"8) Tube 1+2,3,4",
		"8) Tube 1+2,3,4",

		L_TUBE_1_CALICHECK,
		"Tube 1: Black",
		"Tube 1 : Noir",

		L_TUBE_2_CALICHECK,
		"Tube 2: Black + Red",
		"Tube 2 : Noir + Rouge",

		L_TUBE_3_CALICHECK,
		"Tube 3: Black + Orange",
		"Tube 3 : Noir + Orange",

		L_TUBE_4_CALICHECK,
		"Tube 4: Black + Yellow",
		"Tube 4 : Noir + Jaune",

		L_TUBE_5_CALICHECK,
		"Tube 5: Black + Green",
		"Tube 5 : Noir + Vert",

		L_TUBE_6_CALICHECK,
		"Tube 6: Black + Blue",
		"Tube 6 : Noir + Bleu",

		L_TUBE_7_CALICHECK,
		"Tube 7: Black + Purple",
		"Tube 7 : Noir + Violet",

		L_TUBE_8_CALICHECK,
		"Tube 8: Black + Purple + Red",
		"Tube 8 : Noir + Violet + Rouge",

		L_TUBE_9_CALICHECK,
		"Tube 9: Black + Purple + Orange",
		"Tube 9 : Noir + Violet + Orange",

		L_TUBE_10_CALICHECK,
		"Tube 10: Black + Purple + Yellow",
		"Tube 10 : Noir + Violet + Jaune",

		L_TUBE_11_CALICHECK,
		"Tube 11: Black + Purple + Green",
		"Tube 11 : Noir + Violet + Vert",

		L_TUBE_12_CALICHECK,
		"Tube 12: Black + Purple + Blue",
		"Tube 12 : Noir + Violet + Bleu",

		L_PR_TUBE_1_CALICHECK,
		"Black",
		"Noir",

		L_PR_TUBE_2_CALICHECK,
		"Black + Red",
		"Noir + Rouge",

		L_PR_TUBE_3_CALICHECK,
		"Black + Orange",
		"Noir + Orange",

		L_PR_TUBE_4_CALICHECK,
		"Black + Yellow",
		"Noir + Jaune",

		L_PR_TUBE_5_CALICHECK,
		"Black + Green",
		"Noir + Vert",

		L_PR_TUBE_6_CALICHECK,
		"Black + Blue",
		"Noir + Bleu",

		L_PR_TUBE_7_CALICHECK,
		"Black + Purple",
		"Noir + Violet",

		L_PR_TUBE_8_CALICHECK,
		"Black/Purple/Red",
		"Noir/Violet/Rouge",

		L_PR_TUBE_9_CALICHECK,
		"Black/Purple/Orange",
		"Noir/Violet/Orange",

		L_PR_TUBE_10_CALICHECK,
		"Black/Purple/Yellow",
		"Noir/Violet/Jaune",

		L_PR_TUBE_11_CALICHECK,
		"Black/Purple/Green",
		"Noir/Violet/Vert",

		L_PR_TUBE_12_CALICHECK,
		"Black/Purple/Blue",
		"Noir/Violet/Bleu",

		L_MEASURE_TEST_SOURCE,
		"Measure test source, ",
		"Mesurer source de test, ",	//Changed

		L_MEASURE_TEST_SOURCE_SN,
		", S/N: ",
		", S/N : ",

		L_OVERRANGE,
		"OVER RANGE",
		"HORS PLAGE",

		L_OVERRANGE2,
		"Over Range",
		"Hors plage",

		L_OVERRANGE_ERROR,
		"OVER RANGE\nERROR",
		"HORS PLAGE
ERREUR",

		L_OVERRANGE_ERROR2,
		"Over Range Error",
		"Erreur hors plage",

		L_CALC_DEV_ERROR,
		"Calc: %s, Dev: ERROR",
		"Calc : %s, Écart : ERREUR",

		L_CALC_DEV,
		"Calc: %s, Dev: %.1f%%",
		"Calc : %s, Écart : %.1f%%",

		L_CLEAR_SELECTED_NUCLIDE,
		"Clear Selected\nNuclide",
		"Effacer nucléide
sélectionné",

		L_PAGE_OF,
		"Page %d of %d",
		"Page %d de %d",

		L_USB_PC_DRIVER,
		"USB PC Driver:",
		"Pilote USB PC :",

		L_SCREEN_CALIB,
		"Screen Calib",
		"Calib. écran",

		L_DOSE_DECAY_ENTRY,
		"Dose Decay Entry:",
		"Saisie décroissance :",		//Changed

		L_PLEASE_SELECT_NUCLIDE,
		"Please Select Nuclide",
		"Veuillez sélectionner le nucléide",

		L_ENTER_MOTC_LIMIT,
		"Enter Mo/Tc Limit:",
		"Entrez limite Mo/Tc :",

		L_ENTER_NUM_MEASUREMENTS,
		"Please Enter # of Measurements:",
		"Veuillez entrer le nb de mesures :",

		L_ENTER_TEST_2_TIME,
		"Enter Test 2 Time:",
		"Entrez heure test 2 :",
		
		L_ENTER_TEST_3_TIME,
		"Enter Test 3 Time:",
		"Entrez heure test 3 :",

		L_ENTER_TEST_4_TIME,
		"Enter Test 4 Time:",
		"Entrez heure test 4 :",

		L_ENTER_TEST_5_TIME,
		"Enter Test 5 Time:",
		"Entrez heure test 5 :",

		L_ENTER_TEST_6_TIME,
		"Enter Test 6 Time:",
		"Entrez heure test 6 :",

		L_ENTER_TEST_7_TIME,
		"Enter Test 7 Time:",
		"Entrez heure test 7 :",

		L_ENTER_TEST_8_TIME,
		"Enter Test 8 Time:",
		"Entrez heure test 8 :",

		L_ENTER_TEST_9_TIME,
		"Enter Test 9 Time:",
		"Entrez heure test 9 :",

		L_ENTER_TEST_10_TIME,
		"Enter Test 10 Time:",
		"Entrez heure test 10 :",

		L_ENTER_TEST_11_TIME,
		"Enter Test 11 Time:",
		"Entrez heure test 11 :",

		L_ENTER_TEST_12_TIME,
		"Enter Test 12 Time:",
		"Entrez heure test 12 :",

		L_SEC,
		" Sec",
		" Sec",

		L_SEC2,
		" sec",
		" sec",

		L_SEC3,
		"sec",
		"sec",

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
		" Hr",

		L_HR2,
		" hr",
		" hr",

		L_HR3,
		"hr",
		"hr",

		L_HR4,
		" Hr",
		" Hr",

		L_DAY,
		" Day",
		" Jour",

		L_DAY2,
		" day",
		" jour",

		L_DAY3,
		"day",
		"jour",

		L_YR,
		" Yr",
		" An",

		L_YR2,
		" yr",
		" an",

		L_YR3,
		"yr",
		"an",

		L_MYR,
		" Myr",
		" Man",

		L_SETUP_NUCLIDE_ERROR,
		"Setup Nuclide Error",
		"Erreur config. nucléide",

		L_PLEASE_COMPLETE_PARTIAL_NUCLIDE_ENTRY,
		"Please complete partial Nuclide entry.\nRow(s): ",
		"Complétez la saisie partielle du nucléide.
Ligne(s) : ",

		L_DUPLICATE_NAME_FOUND,
		"Duplicate Name found:\n",
		"Nom en double trouvé :
",
		
		L_SETUP_SOURCES_ERROR,
		"Setup Sources Error",
		"Erreur config. sources",

		L_NO_CONSTANCY_CHANNEL,
		"Constancy Source selected without Constancy Channels",
		"Source de constance sélectionnée sans canaux de constance",	//Changed

		L_NO_CONSTANCY_SOURCE,
		"Constancy Channels selected without Constancy Source",
		"Canaux de constance sélectionnés sans source de constance",	//Changed

		L_ABOVE_MAXIMUM_ACTIVITY_CO57_CI,
		"Above Maximum Activity\nCo57 Activity: %.3f Ci\nUpper Limit: %.3f Ci",
		"Au-dessus de l'activité maximale
Co57 Activité : %.3f Ci
Limite sup. : %.3f Ci",

		L_ABOVE_MAXIMUM_ACTIVITY_CO57_BQ,
		"Above Maximum Activity\nCo57 Activity: %.3f GBq\nUpper Limit: %.3f GBq",
		"Au-dessus de l'activité maximale
Co57 Activité : %.3f GBq
Limite sup. : %.3f GBq",

		L_BELOW_MINIMUM_ACTIVITY_CO57_CI,
		"Below Minimum Activity\nCo57 Activity: %.3f uCi\nLower Limit: %.3f uCi",
		"En dessous de l'activité minimale
Co57 Activité : %.3f uCi
Limite inf. : %.3f uCi",
		
		L_BELOW_MINIMUM_ACTIVITY_CO57_BQ,
		"Below Minimum Activity\nCo57 Activity: %.6f MBq\nLower Limit: %.6f MBq",
		"En dessous de l'activité minimale
Co57 Activité : %.6f MBq
Limite inf. : %.6f MBq",
		
		L_ABOVE_MAXIMUM_ACTIVITY_CO60_CI,
		"Above Maximum Activity\nCo60 Activity: %.3f Ci\nUpper Limit: %.3f Ci",
		"Au-dessus de l'activité maximale
Co60 Activité : %.3f Ci
Limite sup. : %.3f Ci",
		
		L_ABOVE_MAXIMUM_ACTIVITY_CO60_BQ,
		"Above Maximum Activity\nCo60 Activity: %.3f GBq\nUpper Limit: %.3f GBq",
		"Au-dessus de l'activité maximale
Co60 Activité : %.3f GBq
Limite sup. : %.3f GBq",
		
		L_BELOW_MINIMUM_ACTIVITY_CO60_CI,
		"Below Minimum Activity\nCo60 Activity: %.3f uCi\nLower Limit: %.3f uCi",
		"En dessous de l'activité minimale
Co60 Activité : %.3f uCi
Limite inf. : %.3f uCi",
		
		L_BELOW_MINIMUM_ACTIVITY_CO60_BQ,
		"Below Minimum Activity\nCo60 Activity: %.6f MBq\nLower Limit: %.6f MBq",
		"En dessous de l'activité minimale
Co60 Activité : %.6f MBq
Limite inf. : %.6f MBq",
		
		L_ABOVE_MAXIMUM_ACTIVITY_BA133_CI,
		"Above Maximum Activity\nBa133 Activity: %.3f Ci\nUpper Limit: %.3f Ci",
		"Au-dessus de l'activité maximale
Ba133 Activité : %.3f Ci
Limite sup. : %.3f Ci",

		L_ABOVE_MAXIMUM_ACTIVITY_BA133_BQ,
		"Above Maximum Activity\nBa133 Activity: %.3f GBq\nUpper Limit: %.3f GBq",
		"Au-dessus de l'activité maximale
Ba133 Activité : %.3f GBq
Limite sup. : %.3f GBq",
		
		L_BELOW_MINIMUM_ACTIVITY_BA133_CI,
		"Below Minimum Activity\nBa133 Activity: %.3f uCi\nLower Limit: %.3f uCi",
		"En dessous de l'activité minimale
Ba133 Activité : %.3f uCi
Limite inf. : %.3f uCi",
		
		L_BELOW_MINIMUM_ACTIVITY_BA133_BQ,
		"Below Minimum Activity\nBa133 Activity: %.6f MBq\nLower Limit: %.6f MBq",
		"En dessous de l'activité minimale
Ba133 Activité : %.6f MBq
Limite inf. : %.6f MBq",
		
		L_ABOVE_MAXIMUM_ACTIVITY_CS137_CI,
		"Above Maximum Activity\nCs137 Activity: %.3f Ci\nUpper Limit: %.3f Ci",
		"Au-dessus de l'activité maximale
Cs137 Activité : %.3f Ci
Limite sup. : %.3f Ci",
		
		L_ABOVE_MAXIMUM_ACTIVITY_CS137_BQ,
		"Above Maximum Activity\nCs137 Activity: %.3f GBq\nUpper Limit: %.3f GBq",
		"Au-dessus de l'activité maximale
Cs137 Activité : %.3f GBq
Limite sup. : %.3f GBq",
		
		L_BELOW_MINIMUM_ACTIVITY_CS137_CI,
		"Below Minimum Activity\nCs137 Activity: %.3f uCi\nLower Limit: %.3f uCi",
		"En dessous de l'activité minimale
Cs137 Activité : %.3f uCi
Limite inf. : %.3f uCi",
		
		L_BELOW_MINIMUM_ACTIVITY_CS137_BQ,
		"Below Minimum Activity\nCs137 Activity: %.6f MBq\nLower Limit: %.6f MBq",
		"En dessous de l'activité minimale
Cs137 Activité : %.6f MBq
Limite inf. : %.6f MBq",
		
		L_ABOVE_MAXIMUM_ACTIVITY_NA22_CI,
		"Above Maximum Activity\nNa22 Activity: %.3f Ci\nUpper Limit: %.3f Ci",
		"Au-dessus de l'activité maximale
Na22 Activité : %.3f Ci
Limite sup. : %.3f Ci",
		
		L_ABOVE_MAXIMUM_ACTIVITY_NA22_BQ,
		"Above Maximum Activity\nNa22 Activity: %.3f GBq\nUpper Limit: %.3f GBq",
		"Au-dessus de l'activité maximale
Na22 Activité : %.3f GBq
Limite sup. : %.3f GBq",
		
		L_BELOW_MINIMUM_ACTIVITY_NA22_CI,
		"Below Minimum Activity\nNa22 Activity: %.3f uCi\nLower Limit: %.3f uCi",
		"En dessous de l'activité minimale
Na22 Activité : %.3f uCi
Limite inf. : %.3f uCi",
		
		L_BELOW_MINIMUM_ACTIVITY_NA22_BQ,
		"Below Minimum Activity\nNa22 Activity: %.6f MBq\nLower Limit: %.6f MBq",
		"En dessous de l'activité minimale
Na22 Activité : %.6f MBq
Limite inf. : %.6f MBq",

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
		"Facteur actuel : %.2f",

		L_CHAMBER_SERIAL,
		"Chamber Serial: %s",	
		"Série chambre : %s",

		L_CHAMBER_SERIAL_OLD_NEW,
		"(Chamber Serial) Old:%s, New:%s",
		"(Série chambre) Ancien :%s, Nouveau :%s",

		L_SETUP_LINEATOR_ERROR,
		"Setup Lineator Error",
		"Erreur config. Lineator",

		L_SETUP_CALICHECK_ERROR,
		"Setup Calicheck Error",
		"Erreur config. Calicheck",

		L_FILL_IN_ALL_MEASUREMENTS,
		"Please fill in all measurements",		
		"Veuillez remplir toutes les mesures",

		L_FILL_IN_NUM_MEASUREMENTS,
		"Please fill in the Num of Measurements",
		"Veuillez remplir le nombre de mesures",

		L_FILL_IN_LINEATOR_SERIAL_NUMBER,
		"Please fill in Lineator Serial Number",	
		"Veuillez remplir le numéro de série Lineator",

		L_FILL_IN_CALICHECK_SERIAL_NUMBER,
		"Please fill in Calicheck Serial Number",	
		"Veuillez remplir le numéro de série Calicheck",		//Changed

		L_FACTOR_OLD_NEW,
		"Old: %.2f, New: %.2f",
		"Ancien : %.2f, Nouveau : %.2f",

		L_NEW_FACTOR,
		"New Factor: %.2f",
		"Nouveau facteur : %.2f",

		L_DIFFERENT_CHAMBERS,
		"The current factors are for a different chamber\nand can not be used for this chamber",
		"Les facteurs actuels sont pour une autre chambre
et ne peuvent pas être utilisés pour cette chambre",

		L_SETUP_LINEATOR,
		"Setup Lineator",
		"Configuration Lineator",

		L_SETUP_CALICHECK,
		"Setup Calicheck",
		"Configuration Calicheck",

		L_INVALID_DAY,
		"Invalid Day",
		"Jour invalide",

		L_INVALID_HOUR,
		"Invalid Hour",
		"Heure invalide",

		L_JAN,
		"Jan",
		"jan",

		L_FEB,
		"Feb",
		"fév",

		L_MAR,
		"Mar",
		"mar",

		L_APR,
		"Apr",
		"avr",

		L_MAY,
		"May",
		"mai",

		L_JUN,
		"Jun",
		"jun",

		L_JUL,
		"Jul",
		"jul",

		L_AUG,
		"Aug",
		"aoû",

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
		"déc",

		L_SUN,
		"Sun",
		"dim",		//Changed

		L_MON,
		"Mon",
		"lun",		//Changed

		L_TUE,
		"Tue",
		"mar",		//Changed

		L_WED,
		"Wed",
		"mer",		//Changed

		L_THU,
		"Thu",
		"jeu",		//Changed

		L_FRI,
		"Fri",
		"ven",		//Changed

		L_SAT,
		"Sat",
		"sam",		//Changed

		L_SN_MAIN_UNIT,
		"S/N Main Unit:",
		"S/N Unité principale :",

		L_CALICHECK_TEST_CALIBRATION,
		"CALICHECK TEST CALIBRATION",
		"CALIBRATION TEST CALICHECK",

		L_LINEATOR_TEST_CALIBRATION,
		"LINEATOR TEST CALIBRATION ",
		"CALIBRATION TEST LINEATOR ",

		L_INITIAL_FACTOR,
		"Initial Factor:     ",
		"Facteur initial :   ",

		L_CALIBRATION_FACTOR,
		"Calibration Factor: ",
		"Facteur de calibration : ",

		L_CH_SERIAL_OLD_NEW,
		"Ch Serial)Old:%s New:%s		",
		"Série Ch)Ancien :%s Nouveau :%s		",

		L_R_CHAMBER,
		"R Chamber",
		"chambre R",

		L_P_CHAMBER,
		"P Chamber",
		"chambre P",

		L_SETUP_REMOTE_NUCLIDES,
		"Setup Remote Nuclides",
		"Config. nucléides distants",	//Changed
		
		L_AUTOLINEARITY_PAUSED,
		"AutoLinearity Paused!!!",
		"AutoLinéarité en pause !!!",

		L_PLEASE_ENTER_CAL_NUM,
		"Please Enter Cal #:",
		"Veuillez entrer Cal # :",

		L_PLEASE_ENTER_DOSE_TIME,
		"Please Enter Dose Time:",
		"Veuillez entrer l'heure de dose :",

		L_LOW_BATTERY,
		"Low Battery",
		"Batterie faible",

		L_DOSE_DECAY,
		"Dose Decay",
		"Décroissance dose",

		L_HOME_SCREEN,
		"Home\n Screen",
		"Écran
d'accueil",

		L_NUCLIDE_SCREEN,
		"Nuclide\n Screen",
		"Écran
nucléide",

		L_BTN_YES,
		"YES",
		"OUI",

		L_BTN_NO,
		"NO",
		"NON",

		L_DELETE_LINEATOR_SETTINGS,
		"Delete Lineator Settings",
		"Supprimer réglages Lineator",
		
		L_DELETE_CALICHECK_SETTINGS,
		"Delete Calicheck Settings",
		"Supprimer réglages Calicheck",

		L_DELETE_LINEATOR_SETTINGS_Q,
		"Delete Lineator Settings?",
		"Supprimer réglages Lineator ?",
		
		L_DELETE_CALICHECK_SETTINGS_Q,
		"Delete Calicheck Settings?",
		"Supprimer réglages Calicheck ?",

		L_UTILITY,
		"Utility",
		"Utilitaire",

		L_CONV_INPUT_ACTIVITY,
		"[Ci,Bq Conv] Input Activity:",
		"[Ci,Bq Conv] Entrer activité :",

		L_DECAY_CALCULATOR,
		"[Decay Calculator]",
		"[Calculateur décroissance]",

		L_FROM,
		"FROM:",
		"DE :",

		L_FROM2,
		"From:",
		"De :",

		L_TO,
		"TO:",
		"À :",

		L_TO2,
		"To:",
		"À :",

		L_DIAGNOSTICS,
		"Diagnostics",
		"Diagnostics",

		L_DOSE_TABLE,
		"Dose Table",
		"Table de doses",

		L_PLEASE_ENTER_ACTIVITY,
		"Please Enter Activity:",
		"Veuillez entrer l'activité :",	//Changed

		L_PLEASE_ENTER_START_TIME,
		"Please Enter Start Time:",
		"Veuillez entrer l'heure de début :",		//Changed

		L_PLEASE_ENTER_END_TIME,
		"Please Enter End Time:",
		"Veuillez entrer l'heure de fin :",	//Changed

		L_TOO_FAR_INTO_FUTURE,
		"TOO FAR INTO FUTURE",
		"TROP LOIN DANS LE FUTUR",

		L_TOO_FAR_INTO_PAST,
		"TOO FAR INTO PAST",
		"TROP LOIN DANS LE PASSÉ",

		L_TOO_LARGE,
		"TOO LARGE",
		"TROP GRAND",

		L_PROGRAM_INTEGRITY,
		"PROGRAM INTEGRITY",
		"INTÉGRITÉ DU PROGRAMME",

		L_FAIL,
		"FAIL: ",
		"ÉCHEC : ",

		L_FAIL2,
		"FAIL",
		"ÉCHEC",

		L_PASS,
		"PASS: ",
		"RÉUSSI : ",

		L_PASS2,
		"PASS",
		"RÉUSSI",

		L_PASSED,
		"Passed",
		"Réussi",

		L_FAILED,
		"Failed, ",
		"Échoué, ",

		L_DAILY_TEST,
		"Daily Test",
		"Test quotidien",

		L_DAILY_TEST2,
		"DAILY TEST:",
		"TEST QUOTIDIEN :",

		L_REMOVE_ALL_SOURCES_FROM_CHAMBER,
		"1) Remove All Sources from Chamber",
		"1) Retirer toutes les sources de la chambre",

		L_AUTO_ZERO,
		"2) Auto Zero",
		"2) Auto zéro",

		L_MEASURE_BACKGROUND,
		"3) Measure Backgnd",
		"3) Mesure bruit de fond",

		L_MEASURE_BACKGROUND2,
		"2) Measure Backgnd",
		"2) Mesure bruit de fond",

		L_CHECK_CHAMBER_VOLTAGE,
		"4) Check Chamber Voltage",
		"4) Vérifier tension chambre",

		L_CHECK_CHAMBER_VOLTAGE2,
		"1) Check Chamber Voltage",
		"1) Vérifier tension chambre",

		L_DATA_CHECK,
		"5) Data Check",
		"5) Vérification données",

		L_ACCURACY,
		"Accuracy",
		"Exactitude",	//Changed

		L_ZERO_DRIFT,
		"Zero Drift",
		"Dérive du zéro",

		L_ZERO_OUT_OF_RANGE,
		"Zero out of Range",
		"Zéro hors plage",

		L_BACKGROUND_TOO_HIGH,
		"BACKGROUND TOO HIGH",
		"BRUIT DE FOND TROP ÉLEVÉ",

		L_BACKGROUND_TOO_HIGH2,
		"BACKGROUND\n TOO HIGH",
		"BRUIT DE FOND
 TROP ÉLEVÉ",

		L_BACKGROUND_TOO_HIGH3,
		"Background Too High",
		"Bruit de fond trop élevé",

		L_NO_DAILY_SOURCE_DATA,
		"6) NO DAILY SOURCE DATA",
		"6) PAS DE DONNÉES SOURCE QUOTID.",

		L_PLEASE_WAIT_2_MINUTES_FOR,
		"PLEASE WAIT 2 Minutes for",
		"VEUILLEZ PATIENTER 2 minutes pour",

		L_STABILIZATION,
		"Stabilization",
		"Stabilisation",

		L_PLEASE_WAIT_2_MINUTES_FOR_STABILIZATION,
		"PLEASE WAIT 2 Minutes for Stabilization",
		"VEUILLEZ PATIENTER 2 minutes pour la stabilisation",

		L_HIGH,
		"HIGH",
		"ÉLEVÉ",

		L_CHAMBER,
		"Chamber",
		"Chambre",

		L_ZERO,
		"ZERO:",
		"ZÉRO :",

		L_BACKGROUND,
		"BACKGROUND:",
		"BRUIT DE FOND :",

		L_BACKGROUND2,
		"Background",
		"Bruit de fond",

		L_CHAMBER_VOLTAGE,
		"CHAMBER VOLTAGE:",
		"TENSION CHAMBRE :",

		L_VOLTS,
		"Volts",
		"V",

		L_DATA_CHECK_PRINTED,
		"DATA CHECK:",
		"VÉRIF. DONNÉES :",

		L_SIGNATURE,
		"signature",
		"signature",

		L_DOSE_TABLE_VOLUME,
		"Volume",
		"Volume",

		L_DOSE,
		"Dose",
		"Dose",

		L_INTERVAL,
		"Interval",
		"Intervalle",

		L_INTERVAL2,
		"Interval:",
		"Intervalle :",

		L_MEASURE_ACTIVITY,
		"Measure Activity",
		"Mesurer l'activité",		//Changed
	
		L_PLEASE_ENTER_VOLUME,
		"Please Enter Volume(ml):",
		"Veuillez entrer le volume (ml) :",		//Changed

		L_PLEASE_ENTER_DOSE,
		"Please Enter Dose",
		"Veuillez entrer la dose",		//Changed

		L_ENTER_INTERVAL,
		"Enter Interval in minutes:",
		"Entrez l'intervalle en minutes :",	//Changed

		L_PR_TIME,
		"TIME",
		"HEURE",

		L_DOSE_TABLE_ERROR,
		"Dose Table Error",
		"Erreur table de doses",
		
		L_DOSE_LARGER_THAN_ACTIVITY,
		"Dose can not be larger than initial Activity.",
		"La dose ne peut pas dépasser l'activité initiale.",

		L_EXCEEDS_INITIAL,
		" - Exceeds Initial",
		" - Dépasse l'initial",

		L_PR_DOSE_TABLE,
		"DOSE TABLE",
		"TABLE DE DOSES",

		L_PR_VOLUME_HEADING,
		"VOLUME (ml)",
		"VOLUME (ml)",

		L_PR_EXCEEDS_INITIAL,
		"Exceeds Initial Vol",
		"Vol. initial dépassé",

		L_PR_NUCLIDE_DATA,
		"NUCLIDE DATA:",
		"DONNÉES NUCLÉIDE :",

		L_PR_TEST_SOURCES,
		"TEST SOURCES:",
		"SOURCES DE TEST :",

		L_PR_NO_SOURCE,
		"No Source",
		"Aucune source",

		L_PR_DAILY_CONSTANCY,
		"D: Daily  C: Constancy Source",		
		"Q : Quotidien  C : Source constance",	//Changed

		L_PR_RESPONSE_CORRECTION,
		"Response Correction:",
		"Correction de réponse :",

		L_PR_WAS_LOW,
		"(Co60 was LOW )",					
		"(Co60 était BAS )",

		L_PR_WAS_HIGH,
		"(Co60 was HIGH)",					
		"(Co60 était HAUT)",

		L_PR_NOMINAL_VOLTAGE,
		"Nominal Voltage: ",
		"Tension nominale : ",

		L_PR_MEMORY_STATUS,
		"MEMORY STATUS",
		"ÉTAT DE LA MÉMOIRE",

		L_PR_AVAILABLE,
		"Available:",
		"Disponible :",

		L_PR_TOTAL,
		"Total:",
		"Total :",
		
		L_PR_R_CHAMBER,
		"R CHAMBER",
		"CHAMBRE R",
		
		L_PR_PET_CHAMBER,
		"PET CHAMBER",
		"CHAMBRE PET",

		L_PET_CHAMBER,
		"PET Chamber",
		"Chambre PET",
		
		L_MEASURED,
		"Measured:",
		"Mesuré :",

		L_MEASURED2,
		"Measured",
		"Mesuré",

		L_MEASURED3,
		"Measured",
		"Mesuré",

		L_CALCULATED,
		"Calculated:",
		"Calculé :",

		L_AUTOCONSTANCY,
		"AutoConstancy",
		"AutoConstance",

		L_NOSOURCEDATA,
		"No Source Data",
		"Aucune donnée source",

		L_DEVIATION,
		"Deviation:",
		"Écart :",

		L_PR_USER_CHAMBER_NUCLIDES,
		"USER CHAMBER NUCLIDES:",
		"NUCLÉIDES CHAMBRE UTILISATEUR :",

		L_PR_HV_DIRECTLY_INTO_ADC,
		"HV directly into ADC",
		"HV directement dans ADC",

		L_PR_SINGLE_GAIN_RELAY,
		"Single Gain Relay",
		"Relais de gain unique",	//Changed

		L_PR_GAIN,
		"Gain: %4.6f",
		"Gain : %4.6f",

		L_PR_TIME_UNIT_1,
		"M",
		"M",

		L_PR_TIME_UNIT_2,
		"H",
		"H",

		L_PR_TIME_UNIT_3,
		"D",
		"J",

		L_PR_TIME_UNIT_4,
		"Y",
		"A",
		
		L_PR_TIME_UNIT_5,
		"MY",
		"MA",
		
		L_PR_HALFLIFE,
		"Half-Life",
		"Demi-vie",

		L_PR_HALFLIFE2,
		"Halflife:",
		"Demi-vie :",

		L_PR_CURRENT,
		"CURRENT",
		"ACTUEL",

		L_PR_CALIBRATION,
		"CALIBRATION",
		"CALIBRATION",

		L_PR_USER_NUCLIDES,
		"USER NUCLIDES:",
		"NUCLÉIDES UTILISATEUR :",

		L_PR_CO60_HIGH,
		"(Co60 HIGH)",
		"(Co60 HAUT)",

		L_PR_CO60_LOW,
		"(Co60 LOW )",
		"(Co60 BAS )",

		L_ACCURACYTEST,
		"ACCURACY TEST:",
		"TEST D'EXACTITUDE :",

		L_SOURCE,
		"Source:",
		"Source :",

		L_SERIALNUM,
		"Serial #:",
		"# de série :",

		L_STANDSOURCE,
		"Stand. Source(S):",
		"Source std. (S) :",		//Changed

		L_MEASUREDAS,
		"Measured As  (M):",
		"Mesuré comme (M) :",

		L_DEVIATIONMSS,
		"Deviation(M-S)/S:",
		"Écart   (M-S)/S :",

		L_CONSTANCYTEST,
		"CONSTANCY TEST:",
		"TEST DE CONSTANCE :",	//Changed

		L_FINISHED,
		"Finished",
		"Terminé",

		L_FINISHED2,
		"Finished!!!",
		"Terminé !!!",

		L_CHAMBER_VOLTS,
		"Chamber Volts",
		"Tension chambre",

		L_ENHANCED_TESTS,
		"Enhanced Tests",
		"Tests améliorés",

		L_GEOMETRY,
		"Geometry",
		"Géométrie",

		L_LINEARITY,
		"Linearity",
		"Linéarité",

		L_QC,
		"QC",
		"QC",

		L_INVENTORY,
		"Inventory",
		"Inventaire",

		L_INVENTORY_LIST,
		"Inventory List",
		"Liste d'inventaire",

		L_STUDY,
		"Study",
		"Étude",

		L_LOT,
		"Lot",
		"Lot",

		L_DATE,
		"Date",
		"Date",

		L_ID,
		"ID",
		"ID",

		L_CONC,
		"Conc",
		"Conc",

		L_ADD_ITEM,
		"Add Item",
		"Ajouter",		//Changed

		L_DELETE_ALL,
		"Delete All",
		"Tout supprimer",
		
		L_WITHDRAW,
		"Withdraw",
		"Prélever",		//Changed

		L_MAKE_KIT,
		"Make Kit",
		"Kit",

		L_DELETE_ITEM,
		"Delete Item",
		"Supprimer élément",

		L_PR_INVENTORY,
		"INVENTORY",
		"INVENTAIRE",

		L_PR_LOT,
		"Lot #:",
		"Lot # :",

		L_PR_OVER,
		"OVER",
		"DÉPASSÉ",

		L_ADD_INVENTORY_ITEM,
		"Add Inventory Item",
		"Ajouter élément à l'inventaire",	//Changed

		L_NUCLIDE_COLON,
		"Nuclide:",
		"Nucléide :",

		L_NUCLIDE_COLON2,
		"NUCLIDE:",
		"NUCLÉIDE :",

		L_LOT_COLON,
		"Lot:",
		"Lot :",

		L_ACTIVITY_COLON,
		"Activity:",
		"Activité :",

		L_DATE_COLON,
		"Date:",
		"Date :",

		L_PLEASE_ENTER_ID,
		"Please Enter ID:",
		"Veuillez entrer l'ID :",	//Changed

		L_PLEASE_ENTER_MEASUREMENT_TIME,
		"Please Enter Measurment Time:",
		"Veuillez entrer l'heure de mesure :"	//Changed

		L_PLEASE_ENTER_LOT,
		"Please Enter Lot:",
		"Veuillez entrer le lot :",

		L_PLEASE_ACCEPT_ACTIVITY,
		"Please Accept Activity:",
		"Veuillez accepter l'activité :",
		
		L_SUBMIT,
		"Submit",
		"Soumettre",

		L_NONE,
		"None",
		"Aucun",

		L_BONE,
		"Bone",
		"Os",

		L_LUNG,
		"Lung",
		"Poumon",

		L_HIDA,
		"Hida",
		"Hépatobiliaire",

		L_HEART,
		"Heart",
		"Coeur",

		L_RENAL,
		"Renal",
		"Rénal",

		L_LIVER,
		"Liver",
		"Foie",

		L_BRAIN,
		"Brain",
		"Cerveau",

		L_LYMPH,
		"Lymph",
		"Lymphe",

		L_STUDY_LABEL,
		"STUDY:",
		"ÉTUDE :",

		L_MEASURE,
		"Measure",
		"Mesurer",	//Changed

		L_PLEASE_ENTER_MOTC_RATIO,
		"Please Enter Mo/Tc Ratio:",
		"Veuillez entrer le rapport Mo/Tc :",

		L_PLEASE_SELECT_STUDY,
		"Please select study",
		"Veuillez sélectionner l'étude",
		
		L_CONTAINER,
		"1) Container",
		"1) Récipient",

		L_INITIAL_VOLUME,
		"2) Initial Volume",
		"2) Volume initial",

		L_INITIAL_MEASUREMENT,
		"Initial Measurement",
		"Mesure initiale",

		L_ADDED_VOLUME,
		"4) Added Volume",
		"4) Volume ajouté",

		L_VOLUME2,
		"4) Volume",
		"4) Volume",

		L_MEASUREMENT,
		"Measurement",
		"Mesure",

		L_CAL_NOT_ENTERED_FOR_NUCLIDE,
		"CAL # NOT ENTERED FOR Tc99m",
		"CAL # NON SAISI POUR Tc99m",

		L_MORE_MEASUREMENTS,
		"More Measurements",
		"Plus de mesures",

		L_SYRINGE,
		"Syringe",
		"Seringue",

		L_VIAL,
		"Vial",
		"Flacon",

		L_VALID_RANGE_FOR_GEOMETRY,
		"Valid range is 98.9ml to 0.1ml",
		"Plage valide de 98,9 ml à 0,1 ml",

		L_VALID_RANGE_IS_ML_TO_ML,
		"Valid range is %.3fml to 0.1ml",
		"Plage valide de %.3fml à 0.1ml",

		L_ADDED_VOL_TOTAL_VOL,
		"Added Vol: %.3fml\nTotal Vol: %.3fml",
		"Vol. ajouté : %.3fml
Vol. total : %.3fml",

		L_BASE,
		"BASE",
		"BASE",

		L_SYRINGE_ASSAY,
		"Syringe Assay",
		"Essai seringue",		//Changed

		L_VIAL_ASSAY,
		"Vial Assay",
		"Essai flacon",	//Changed

		L_GEOMETRY_REPORT,
		"Geometry Report",
		"Rapport de géométrie",

		L_USING_VOLUME,
		"Using Volume",
		"Volume utilisé",

		L_PERCENT_VAR,
		"% Var",
		"% Var",

		L_PERCENT_VAR2,
		"% Var:",
		"% Var :",

		L_GEOMETRY_TEST,
		"GEOMETRY TEST",
		"TEST DE GÉOMÉTRIE",

		L_USING,
		"Using:",
		"Utilisé :",

		L_VOLUME_ASSAY_VARIATION,
		"#   Volume    Assay    Variation",
		"#   Volume    Essai    Variation",

		L_STD_LINEATOR_CALICHECK_AVAILABLE_IN_SETUP,
		"Standard, Lineator and Calicheck are available in setup",
		"Standard, Lineator et Calicheck disponibles dans config.",

		L_AUTOLINEARITY,
		"AutoLinearity",
		"AutoLinéarité",

		L_STANDARD,
		"Standard",
		"Standard",

		L_LINEATOR,
		"Lineator",
		"Lineator",

		L_CALICHECK,
		"Calicheck",
		"Calicheck",

		L_SELECT_CHAMBER,
		"Select Chamber",
		"Sélectionner chambre",

		L_DELETE_ALL_INVENTORY,
		"Delete All Inventory",
		"Supprimer tout l'inventaire",

		L_DELETE_ALL_INVENTORY_Q,
		"Delete All Inventory?",
		"Supprimer tout l'inventaire ?",

		L_PET_CHAMBER_NO_INVENTORY,
		"The PET Chamber can not access inventory functions",
		"La chambre PET ne peut pas accéder aux fonctions d'inventaire",

		L_DELETE_INVENTORY,
		"Delete Inventory",
		"Supprimer inventaire",

		L_TC_LEVEL_TOO_LOW,
		"Tc Level TOO LOW",
		"Niveau Tc TROP BAS",

		L_WITHDRAW_INVENTORY,
		"Withdraw Inventory",
		"Prélever de l'inventaire",	//Changed

		L_WITHDRAW_ACTIVITY,
		"Withdraw Activity:",
		"Prélever activité :",	//Changed

		L_PLEASE_WITHDRAW_ML,
		"Please withdraw: %5.1f ml",
		"Veuillez prélever : %5.1f ml",

		L_AND_MEASURE_ACTIVITY,
		"and Measure Activity."		
		"et mesurer l'activité."		//Changed
		
		L_ACTIVITY_GREATER_THAN_INVENTORY,
		"ACTIVITY GREATER THAN THAT IN INVENTORY",
		"ACTIVITÉ SUPÉRIEURE À CELLE DE L'INVENTAIRE",

		L_PLEASE_MEASURE_WITHDRAWAL,
		"Please Measure Withdrawal:",
		"Veuillez mesurer le prélèvement :",

		L_TIME_OF_USE,
		"Time of Use:",
		"Heure d'utilisation :",

		L_PLEASE_ENTER_TIME_OF_USE,
		"Please Enter Time Of Use:",
		"Veuillez entrer l'heure d'utilisation :",

		L_START_TEST,
		"Start Test",
		"Démarrer test",

		L_RESUME_TEST,
		"Resume Test",
		"Reprendre test",

		L_REPORTS,
		"Reports",
		"Rapports",		//Changed

		L_ENTER_TOTAL_TIME_IN_HOURS,
		"Enter Total Time in hours:",
		"Entrez le temps total en heures :",

		L_INACTIVE,
		"Inactive:",
		"Inactif :",

		L_START,
		"Start",
		"Démarrer",

		L_DATE_TIME_ELAPSED,
		"Date Time / Elapsed",
		"Date Heure / Écoulé",

		L_PREDICTED,
		"Predicted",
		"Prévu",

		L_PREDICTED2,
		"Predicted:",
		"Prévu :",

		L_ABORT,
		"Abort",
		"Abandonner",

		L_INACTIVATE,
		"Inactivate",
		"Désactiver",

		L_SAVE,
		"Save",
		"Enregistrer",

		L_AUTOLINEARITY_TEST,
		"AutoLinearity Test - Ch:%d, %s",
		"Test AutoLinéarité - Ch :%d, %s",

		L_AUTOLINEARITY_TEST2,
		"AutoLinearity Test",
		"Test AutoLinéarité",

		L_HRS,
		"%d hrs",
		"%d hrs",

		L_AUTOLINEARITY_REPORT1,
		"AutoLinearity Report, Ch:1 (%s)",
		"Rapport AutoLinéarité, Ch :1 (%s)",

		L_AUTOLINEARITY_REPORT2,
		"AutoLinearity Report, Ch:2 (%s)",
		"Rapport AutoLinéarité, Ch :2 (%s)",

		L_AUTOLINEARITY_REPORT3,
		"AutoLinearity Report, S/N: %s",
		"Rapport AutoLinéarité, S/N : %s",

		L_ELAPSED,
		"%s\nElapsed: %d min",
		"%s
Écoulé : %d min",

		L_PAUSED,
		"Paused",
		"En pause",

		L_POWER_FAIL,
		"Power Fail",
		"Panne de courant",

		L_OF,
		"%d of %d",
		"%d de %d",

		L_UNABLE_TO_FIND_CHAMBER,
		"Unable to find Chamber",
		"Impossible de trouver la chambre",

		L_UNABLE_TO_FIND_NUCLIDE,
		"Unable to find Nuclide",
		"Impossible de trouver le nucléide",

		L_UNABLE_TO_FIND_NUCLIDE2,
		"Unable to find Nuclide",
		"Impossible de trouver le nucléide",

		L_CURRENT_NUCLIDE_IS_INCORRECT,
		"Current Nuclide is incorrect",
		"Le nucléide actuel est incorrect",

		L_CHAMBER_SN_IS_INCORRECT,
		"Chamber S/N is incorrect",
		"S/N de la chambre est incorrect",

		L_MISSING_NUCLIDE,
		"Missing Nuclide",
		"Nucléide manquant",

		L_MISSING_INTERVAL,
		"Missing Interval",
		"Intervalle manquant",

		L_MISSING_TOTAL_TIME,
		"Missing Total Time",
		"Temps total manquant",

		L_NUM_OF_MEAS_HAS_TO_BE_GREATER_THAN_4,
		"Number of measurements has to be greater than 4",
		"Le nombre de mesures doit être supérieur à 4",

		L_TYPE,
		"Type:",
		"Type :",

		L_STARTED,
		"Started:",
		"Démarré :",

		L_CANT_BE_SAVED_LESS_THAN_5_MEASUREMENTS,
		"Can't be saved.\nLess than 5 measurements",
		"Ne peut pas être enregistré.
Moins de 5 mesures",

		L_ELAPSED_MINUTES_REMAINING_MINUTES,
		"Elapsed Minutes: %d\nRemaining Minutes:%d",
		"Min écoulées : %d
Min restantes : %d",

		L_SEARCH_AUTOLINEARITY_TESTS,
		"Search AutoLinearity Tests",
		"Rechercher tests AutoLinéarité",

		L_ENTER_START_DATE,
		"Enter Start Date:",
		"Entrez date de début :",

		L_ENTER_END_DATE,
		"Enter End Date:",
		"Entrez date de fin :",

		L_SEARCH,
		"Search",
		"Rechercher",

		L_VIEW,
		"View",
		"Voir",

		L_SEARCH_AUTOLINEARITY_ERROR,
		"Search AutoLinearity Error",
		"Erreur recherche AutoLinéarité",

		L_MORE_THAN_100_ITEMS_RETURNED,
		"More than 100 items have been returned\nPlease refine criteria",
		"Plus de 100 résultats retournés
Veuillez affiner les critères",

		L_0_ITEMS_RETURNED,
		"0 items have been returned",
		"0 résultats retournés",

		L_MAKE_INVENTORY_KIT,
		"Make Inventory Kit",
		"Préparer kit d'inventaire",

		L_SOURCE_NUCLIDE,
		"Source Nuclide:",
		"Nucléide source :",

		L_SOURCE_ID,
		"Source ID:",
		"ID source :",

		L_SOURCE_LOT,
		"Source Lot:",
		"Lot source :",

		L_SOURCE_VOLUME,
		"Source Volume:",
		"Volume source :",

		L_SOURCE_ACTIVITY,
		"Source Activity:",
		"Activité source :",
		
		L_SOURCE_DATE,
		"Source Date:",
		"Date source :",

		L_KIT_STUDY,
		"Kit Study:",
		"Étude du kit :",

		L_KIT_ID,
		"Kit ID:",
		"ID du kit :",

		L_KIT_LOT,
		"Kit Lot:",
		"Lot du kit :",

		L_KIT_ACTIVITY,
		"Kit Activity:",
		"Activité du kit :",

		L_KIT_VOLUME,
		"Kit Volume:",
		"Volume du kit :",
		
		L_KIT_ACTIVITY_GREATER,
		"KIT ACTIVITY GREATER THAN THAT",
		"ACTIVITÉ DU KIT SUPÉRIEURE À",
		
		L_IN_SOURCE,
		"IN SOURCE",
		"CELLE DE LA SOURCE",

		L_KIT_VOLUME_LESS,
		"KIT VOLUME LESS THAN THE"
		"VOLUME DU KIT INFÉRIEUR AU",	//Changed

		L_DRAW_FROM_SOURCE,
		"%.3f ml DRAW FROM SOURCE",
		"%.3f ml PRÉLEVER DE LA SOURCE",		//Changed

		L_PLEASE_WITHDRAW_AND_MEASURE_ACTIVITY,
		"Please withdraw: %5.1f ml,\n and Measure Activity.",
		"Veuillez prélever : %5.1f ml,
 et mesurer l'activité.",

		L_CAPMAC_FOR_MAL_GEN,
		"CAPMAC for Mallinckrodt Gen",
		"CAPMAC pour Mallinckrodt Gen",

		L_CAPMAC_FOR_BM_GEN,
		"CAPMAC for Bristol Myers Gen",
		"CAPMAC pour Bristol Myers Gen",

		L_CAPINTEC_CANISTER,
		"Capintec Canister",
		"Capintec Bidon",

		L_MOLY_ASSAY,
		"Moly Assay",
		"Essai Moly",	//Changed

		L_MOLY_ASSAY_MAL_GEN_METHOD,
		"Moly Assay - Mallinckrodt Gen Method",
		"Essai Moly - Méthode Mallinckrodt Gen",	//Changed

		L_MOLY_ASSAY_BM_GEN_METHOD,
		"Moly Assay - Bristol Myers Gen Method",
		"Essai Moly - Méthode Bristol Myers Gen",	//Changed

		L_MOLY_ASSAY_CAPINTEC_CANISTER_METHOD,
		"Moly Assay - Capintec Canister Method",
		"Essai Moly - Méthode Capintec Bidon",		//Changed

		L_MEASURE_BKG,
		"Measure Bkg",
		"Mesurer bruit fond",	//Changed

		L_SKIP_BKG,
		"Skip Bkg",
		"Ignorer bruit fond",	//Changed

		L_MEASURE_CANISTER_BKG,
		"1) Measure Canister Bkg:",
		"1) Mesurer bruit fond bidon :",	//Changed

		L_MEASURE_CAPMAC_BKG,
		"1) Measure CAPMAC Bkg:",
		"1) Mesurer bruit fond CAPMAC :",	//Changed

		L_SKIPPED,
		"Skipped",
		"Ignoré",		//Changed

		L_ELUATE_IN_CANISTER,
		"2) Eluate in CANISTER: ",
		"2) Éluat dans BIDON :   ",

		L_ELUATE_IN_CAPMAC,
		"2) Eluate in CAPMAC: ",
		"2) Éluat dans CAPMAC : ",

		L_MEASURE_TC_ASSAY,
		"3) Measure Tc99m Assay:"
		"3) Mesurer essai Tc99m :",		//Changed

		L_ACTIVITY_TOO_LOW_ERROR,
		"Activity Too Low Error",
		"Erreur : activité trop faible",		//Changed

		L_CAUTION,
		"CAUTION",
		"ATTENTION",

		L_DO_NOT_USE_AFTER_HOURS,
		"DO NOT USE AFTER %d HOURS",
		"NE PAS UTILISER APRÈS %d HEURES",

		L_MO_TOO_HIGH,
		"MO TOO HIGH",
		"MO TROP ÉLEVÉ",

		L_DO_NOT_USE,
		"DO NOT USE",
		"NE PAS UTILISER",

		L_MO_HIGH_TERMINATE_QUESTION,
		"MO HIGH Terminate?",
		"MO ÉLEVÉ Terminer ?",		//Changed

		L_NEGATIVE_ACTIVITY,
		"NEGATIVE ACTIVITY",
		"ACTIVITÉ NÉGATIVE",

		L_MO_HIGH,
		"MO HIGH",
		"MO ÉLEVÉ",

		L_EXPIRES,
		"Expires",
		"Expire",

		L_MOLY_BKG,
		"Moly Bkg",
		"Bruit fond Moly",
		

		L_MO_TOO_HIGH_DO_NOT_USE,
		"MO TOO HIGH - DO NOT USE",
		"MO TROP ÉLEVÉ - NE PAS UTILISER",

		L_SINGLE_STRIP_TEST,
		"Single Strip Test",
		"Test bande unique",

		L_TWO_STRIP_TEST,
		"Two Strip Test",
		"Test double bande",		//Changed

		L_HMPAO_TEST,
		"HMPAO Test",
		"Test HMPAO",

		L_MAG3_TEST,
		"MAG3 Test",
		"Test MAG3",

		L_NONE_FOUND,
		"No detectors found\nNo Chamber attached\nNo Well attached",		,
		"Aucun détecteur trouvé
Aucune chambre connectée
Aucun puits connecté",
		
		L_QC_ONE_STRIP_TEST,
		"QC - One Strip Test",
		"QC - Test une bande",

		L_TOP_OF_STRIP,
		"1) Top of strip",
		"1) Haut de bande",

		L_BOTTOM_OF_STRIP,
		"2) Bottom of strip",
		"2) Bas de bande",
		
		L_TOP_OF_STRIP2,
		"Top of Strip:",
		"Haut de bande :",

		L_BOTTOM_OF_STRIP2,
		"Bottom of Strip:",
		"Bas de bande :",

		L_ONE_STRIP,
		"One Strip",
		"Une bande",

		L_QC_TWO_STRIP_TEST,
		"QC - Two Strip Test",
		"QC - Test double bande",	//Changed

		L_TOP_OF_STRIP_A,
		"1) Top of strip A",
		"1) Haut bande A",

		L_BOTTOM_OF_STRIP_A,
		"2) Bottom of strip A",
		"2) Bas bande A",
		
		L_TOP_OF_STRIP_B,
		"3) Top of strip B",
		"3) Haut bande B",

		L_BOTTOM_OF_STRIP_B,
		"4) Bottom of strip B",
		"4) Bas bande B",

		L_TOP_OF_STRIP_C,
		"5) Top of strip C",
		"5) Haut bande C",

		L_BOTTOM_OF_STRIP_C,
		"6) Bottom of strip C",
		"6) Bas bande C",

		L_TOP_OF_STRIP_A2,
		"Top of Strip A:",
		"Haut de bande A :",

		L_BOTTOM_OF_STRIP_A2,
		"Bottom of Strip A:",
		"Bas de bande A :",
		
		L_TOP_OF_STRIP_B2,
		"Top of Strip B:",
		"Haut de bande B :",

		L_BOTTOM_OF_STRIP_B2,
		"Bottom of Strip B:",
		"Bas de bande B :",
		
		L_TOP_OF_STRIP_C2,
		"Top of Strip C:",
		"Haut de bande C :",

		L_BOTTOM_OF_STRIP_C2,
		"Bottom of Strip C:",
		"Bas de bande C :",

		L_FREE,
		"Free Tc99m : %5.1f",
		"Tc99m libre : %5.1f",	//Changed

		L_REDUCED_HYDROLIZED,
		"Reduced/Hydrolized: %5.1f",
		"Réduit/Hydrolysé : %5.1f",

		L_HYDROLIZED_REDUCED,
		"Hydrolized/Reduced: %5.1f",
		"Hydrolysé/Réduit : %5.1f",

		L_PURITY,
		"Purity: %5.1f",
		"Pureté : %5.1f",

		L_TOP_TOP_BOTTOM_EQUALS,
		"    Top/(Top+Bottom) = %5.1f",
		"    Haut/(Haut+Bas) = %5.1f",

		L_BOTTOM_TOP_BOTTOM_EQUALS,
		"Bottom/(Top+Bottom) = %5.1f",
		"Bas/(Haut+Bas) = %5.1f",

		L_QUALITY_CONTROL_TEST,
		"QUALITY CONTROL TEST",
		"TEST DE CONTRÔLE QUALITÉ",

		L_SINGLE_STRIP_SINGLE_SOLVENT,
		"Single Strip / Single Solvent",
		"Bande unique / Solvant unique",

		L_MEASURED_VALUES,
		"Measured Values",
		"Valeurs mesurées",

		L_RESULTS,
		"Results",
		"Résultats",

		L_QC_RADIOPHARMACEUTICAL,
		"Radiopharmaceutical: _________",
		"Radiopharmaceutique : ________",	//Changed

		L_QC_LOT,
		"Lot #: _______________________",
		"Lot # : ______________________",

		L_QC_KIT,
		"Kit #: _______________________",		
		"Kit # : ______________________",

		L_TOP_TOP_BOTTOM,
		"Top    / (Top + Bottom): ",
		"Haut   / (Haut + Bas) : ",

		L_BOTTOM_TOP_BOTTOM,
		"Bottom / (Top + Bottom): ",
		"Bas    / (Haut + Bas) : ",

		L_TWO_STRIP,
		"Two Strip",
		"Double bande",	//Changed

		L_TWO_STRIPS_TWO_SOLVENTS,
		"Two Strips / Two Solvents",
		"Double bande / Deux solvants",	//Changed

		L_STRIP_A_FREE,
		"Strip A: Free          = ",
		"Bande A : Libre        = ",

		L_STRIP_B_R_H,
		"Strip B: R / H         = ",
		"Bande B : R / H        = ",

		L_PURITY_F_R_H,
		"Purity: 100 -(F + R/H) = ",	
		"Pureté : 100 -(L + R/H) = ",

		L_F_FREE,
		"F = Free",
		"L = Libre",

		L_R_REDUCED,
		"R = Reduced",
		"R = Réduit",

		L_H_HYDROLIZED,
		"H = Hydrolized",
		"H = Hydrolysé",

		L_FREE_TC99M,
		"Free Tc99m              :",
		"Tc99m libre             :",	//Changed

		L_HYDROLIZED_REDUCED_TC99M,
		"Hydrolized/Reduced Tc99m:",		
		"Hydrolysé/Réduit Tc99m  :",

		L_TC99M_HMPAO,
		"Tc99m HMPAO             :",
		"Tc99m HMPAO             :",

		L_FRACTION_1,
		"1) Fraction #1",
		"1) Fraction #1",

		L_FRACTION_2,
		"2) Fraction #2",
		"2) Fraction #2",

		L_CARTRIDGE,
		"3) Cartridge",
		"3) Cartouche",

		L_FRACTION_12,
		"Fraction #1      : "
		"Fraction #1      : ",

		L_FRACTION_22,
		"Fraction #2      : "
		"Fraction #2      : ",

		L_CARTRIDGE2,
		"Cartridge        : "
		"Cartouche        : ",
		
		L_NON_ELUTABLE,
		"Non-elutable Tc99m: %5.1f",
		"Tc99m non éluable : %5.1f",	//Changed

		L_NON_ELUTABLE2,
		"Non-elutable Tc99m       :",
		"Tc99m non éluable        :",	//Changed

		L_HYDROPHILIC_IMPURE,
		"Hydrophilic Impure: %5.1f",
		"Impureté hydrophile : %5.1f",

		L_HYDROPHILIC_TC_IMPURITIES,
		"Hydrophilic Tc Impurities:",
		"Impuretés Tc hydrophiles :",
		
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
		"Désactiver enregistrement",

		L_INACTIVATE_RECORD2,
		"Inactivate record?",
		"Désactiver l'enregistrement ?",

		L_PLEASE_ENTER_COMMENT,
		"Please enter Comment:",
		"Veuillez entrer le commentaire :",
		
		L_CLEAR_ALL_MEASUREMENTS,
		"Clear All Measurements",
		"Effacer toutes les mesures",

		L_MEASURE_NOW,
		"Measure Now",
		"Mesurer maintenant",	//Changed

		L_NO_SETTINGS,
		"No Settings",
		"Aucun réglage",

		L_FOR_R_CHAMBER,
		"for R Chamber",
		"pour la chambre R",

		L_FOR_R_CHAMBER2,
		"for R Chamber",
		"de chambre R",

		L_FOR_PET_CHAMBER,
		"for PET Chamber",
		"pour la chambre PET",

		L_FOR_PET_CHAMBER2,
		"for PET Chamber",
		"de chambre PET",

		L_CALIBRATION_NUMBER,
		"calibration number",
		"numéro de calibration",

		L_LINEARITY_STD,
		"Linearity, Std",
		"Linéarité, Std",

		L_IN_HRS_MINS,
		"in %d hrs, %d mins",
		"en %d hrs, %d mins",	//Changed

		L_FIRST_MEASUREMENT,
		"First Measurement:",
		"Première mesure :",

		L_CALC,
		"Calc: ",
		"Calc : ",

		L_FACTOR,
		"Factor",
		"Facteur",

		L_RATIO,
		"% Ratio",
		"% Rapport",

		L_RATIO2,
		"Ratio:",
		"Rapport :",

		L_RESULT,
		"Result",
		"Résultat",

		L_RESULT2,
		"Result:",
		"Résultat :",

		L_STD_LINEARITY,
		"Std Linearity",
		"Linéarité std",

		L_STANDARD_LINEARITY_TEST,
		"STANDARD LINEARITY TEST",
		"TEST DE LINÉARITÉ STANDARD",

		L_ACTUAL,
		"Actual:",
		"Réel :",		//Changed

		L_VARIATION,
		"Variation:",
		"Variation :",

		L_LINEATOR_LINEARITY_TEST,
		"LINEATOR LINEARITY TEST",
		"TEST DE LINÉARITÉ LINEATOR",

		L_CHAMBER_SN,
		"Chamber S/N: %s",
		"Chambre S/N : %s",

		L_LINEATOR_SN,
		"Lineator S/N: %s",
		"Lineator S/N : %s",

		L_PRESENT_FACTOR,
		"Present Factor:",
		"Facteur présent :",

		L_CALICHECK_LINEARITY_TEST,
		"CALICHECK LINEARITY TEST",
		"TEST DE LINÉARITÉ CALICHECK",

		L_CALICHECK_SN,
		"Calicheck S/N: %s",
		"Calicheck S/N : %s",

		L_MEAN_RESULT,
		"Mean Result: %s",
		"Résultat moyen : %s",

		L_VARIATION_FROM_MEAN,
		"Variation from Mean:",
		"Variation de la moyenne :",

		L_PC_COMMUNICATIONS_MODE,
		"PC Communications Mode",
		"Mode de communication PC",

		L_RUNNING_QC_TESTS_FROM_PC,
		"Running QC tests from PC",
		"Exécution des tests QC depuis le PC",

		L_ABORT_QC_TESTS,
		"Abort QC Tests",
		"Abandonner tests QC",	//Changed

		L_REMOTE_AUTO_ZERO,
		"Remote Auto Zero",
		"Auto zéro distant",

		L_REMOTE_BACKGROUND,
		"Remote Background",
		"Bruit de fond distant",

		L_REMOTE_CHAMBER_VOLTAGE_TEST,
		"Remote Chamber Voltage Test",
		"Test tension chambre distant",

		L_BACKGROUND_HIGH,
		"Background High",
		"Bruit de fond élevé",

		L_CHAMBER_VOLTAGE_FAILED,
		"Chamber Voltage Failed",
		"Tension chambre échouée",

		L_PLEASE_WAIT,
		"Please Wait",
		"Veuillez patienter",

		L_PET_CHAMBER_CANNOT_RUN_MOLY_ASSAY,
		"The PET chamber can not run a Moly Assay",
		"La chambre PET ne peut pas exécuter un essai Moly",		//Changed
		
		L_MISSING_USB_DRIVE,
		"Missing USB Drive",
		"Clé USB manquante",

		L_UNABLE_TO_FIND_FILE,
		"Unable to find file",
		"Impossible de trouver le fichier",

		L_PLACE_ONLY_ONE_UPDATE,
		"Place only one update",
		"Placez un seul fichier de",

		L_FILE_IN_ROOT_FOLDER,
		"file in root folder",
		"mise à jour dans le dossier racine",

		L_INVALID_UPDATE_FILE,
		"Invalid update file",
		"Fichier de mise à jour invalide",

		L_COPYING,
		"Copying %s",
		"Copie %s",

		L_OF2,
		"of %s",
		"de %s",

		L_CORRUPT_UPDATE_FILE,
		"Corrupt Update File",
		"Fichier de mise à jour corrompu",

		L_FINISHED_COPYING,
		"Finished Copying",
		"Copie terminée",

		L_PLEASE_RESTART2,
		"Please restart",
		"Veuillez redémarrer",

		L_USB_DRIVE_ERROR,
		"USB Drive Error\n",
		"Erreur clé USB
",

		L_SOFTWARE_UPDATE,
		"Software Update",
		"Mise à jour logicielle",

		L_SOFTWARE_DOWNLOAD_IN_PROGRESS,
		"Software download is in progress\nPlease do not turn off calibrator",
		"Téléchargement en cours
Veuillez ne pas éteindre le calibrateur",

		L_PRINTER_PROBLEM,
		"Printer Problem",
		"Problème d'imprimante",

		L_INSERT_TICKET,
		"INSERT TICKET",
		"INSÉRER LE TICKET",	//Changed

		L_CHECK_PRINTER,
		"CHECK PRINTER",
		"VÉRIFIER L'IMPRIMANTE",		//Changed

		L_CONNECT_PRINTER,
		"CONNECT PRINTER",
		"CONNECTER IMPRIMANTE",

		L_INSERT_PAPER,
		"INSERT PAPER",
		"INSÉRER LE PAPIER",

		L_FILE_TRANSFER_ERROR,
		"File Transfer Error",
		"Erreur transfert fichier",

		L_UNABLE_TO_FIND_FILE_PATH,
		"Unable to find file path: %s\n",
		"Chemin du fichier introuvable : %s
",

		L_UPDATE,
		"Update",
		"Mettre à jour",

		L_POWER_OFF_AND_THEN_POWER_ON_TO_COMPLETE_UPDATE,
		"Power Off,\nand then Power On\nto complete update.",
		"Éteignez,
puis rallumez
pour terminer la mise à jour.",

		L_CHECKSUM_HAS_FAILED,
		"Checksum has failed.",
		"La somme de contrôle a échoué.",

		L_UNABLE_TO_FIND_TRANSFER_FILE,
		"Unable to find transfer file.",
		"Fichier de transfert introuvable.",

		L_UPDATE_SOFTWARE,
		"Update Software",
		"Mettre à jour le logiciel",

		L_NOW,
		"Now:",
		"Maintenant :",

		L_PR_CONSTANCY_CHANNELS,
		"Constancy Channels:",
		"Canaux de constance :",		//Changed

		L_OVER,
		" OVER ",
		" DÉPASSÉ ",		//Changed

		L_CHAMBER_ERROR,
		"Chamber Error",
		"Erreur chambre",		//Changed

		L_CHAMBER_LOST_CONNECTION,
		"Chamber #%d has lost connection.\nPlease turn off calibrator and reattach chamber.",
		"La chambre #%d a perdu la connexion.
Veuillez éteindre le calibrateur et reconnecter la chambre.",	//Changed

		L_SAVE_ACCURACY_TEST,
		"Save Accuracy / AutoConstancy",
		"Enregistrer exactitude / AutoConstance",

		L_ZERO_2,
		"Zero",
		"Zéro",

		L_CHAMBER_VOLTAGE_2,
		"Chamber Voltage",
		"Tension chambre",

		L_ACCURACYTEST_2,
		"Accuracy Test",
		"Test d'exactitude",

		L_SEARCH_DAILY_TESTS,
		"Search Daily Tests",
		"Rechercher tests quotidiens",

		L_SEARCH_ZERO_MEASUREMENTS,
		"Search Zero Measurements",
		"Rechercher mesures zéro",

		L_SEARCH_BACKGROUND_MEASUREMENTS,
		"Search Background Measurements",
		"Rechercher mesures bruit de fond",

		L_SEARCH_CHAMBER_VOLTAGE,
		"Search Chamber Voltage Tests",
		"Rechercher tests tension chambre",

		L_SEARCH_ACCURACY_TESTS,
		"Search Accuracy Tests",
		"Rechercher tests d'exactitude",

		L_NOMINAL_VOLTAGE,
		"NOMINAL VOLTAGE:",
		"TENSION NOMINALE :",

		L_MIN_VOLTAGE,
		"MIN VOLTAGE:",
		"TENSION MIN :",

		L_MAX_VOLTAGE,
		"MAX VOLTAGE:",
		"TENSION MAX :",

		L_STAGE,
		"Stage",
		"Étape",

		L_PAGE,
		"Page %d",
		"Page %d",

		L_HALFLIFE_CALC,
		"Half-life Calculator",
		"Calculateur demi-vie",

		L_HALFLIFE_CALC2,
		"Half-life Calc - Ch:%d, %s",
		"Calc. demi-vie - Ch :%d, %s",

		L_SECS,
		"%d sec",
		"%d sec",

		L_ENTER_INTERVAL_SEC,
		"Enter Interval in seconds:",
		"Entrez l'intervalle en secondes :",

		L_ENTER_TOTAL_TIME_IN_MINUTES,
		"Enter Total Time in minutes:",
		"Entrez le temps total en minutes :",

		L_MINS,
		"%d min",
		"%d min",

		L_STOP,
		"Stop",
		"Arrêter",

		L_ELAPSED_SEC,
		"Elapsed (sec)",
		"Écoulé (sec)",

		L_NUM_OF_MEAS_GREATER_THAN_1201,
		"Count is greater than 1201\nTry Total: %d or Interval: %d",
		"Le comptage dépasse 1201
Essayez Total : %d ou Intervalle : %d",

		L_ELAPSED_REMAINING,
		"Elapsed: %d:%02d\nRemaining: %d:%02d",
		"Écoulé : %d:%02d
Restant : %d:%02d",

		L_UNDERRANGE,
		"Under Range",
		"Sous la plage",

		L_LESS_THAN_5_MEASUREMENTS,
		"Less than 5 measurements",
		"Moins de 5 mesures",

		L_HALFLIFE_TOO_SHORT,
		"Half-life is too short",
		"La demi-vie est trop courte",

		L_HALFLIFE_TOO_LONG,
		"Half-life is too long",
		"La demi-vie est trop longue",

		L_HALFLIFE_NEGATIVE,
		"Half-life is negative",
		"La demi-vie est négative",

		L_CHAMBER_SN_2,
		"Chamber S/N:",
		"Chambre S/N :",

		L_PRINT_SUMMARY,
		"Print\nSummary",
		"Imprimer
Résumé",

		L_PR_BT_CHAMBER,
		"BT CHAMBER",
		"CHAMBRE BT",

		L_PR_18_CHAMBER,
		"1.8 Atm CHAMBER",
		"CHAMBRE 1.8 Atm",

		L_BT_CHAMBER,
		"BT Chamber",
		"chambre BT",

		L_18_CHAMBER,
		"1.8 Atm Chamber",
		"chambre 1.8 Atm",

		L_10_CHAMBER,
		"1 Atm Chamber",
		"chambre 1 Atm",

		L_FOR_BT_CHAMBER,
		"for BT Chamber",
		"pour la chambre BT",

		L_FOR_18_CHAMBER,
		"for 1.8 Atm Chamber",
		"pour la chambre 1.8 Atm",

		L_FOR_BT_CHAMBER2,
		"for BT Chamber",
		"de chambre BT",

		L_FOR_18_CHAMBER2,
		"for 1.8 Atm Chamber",
		"de chambre 1.8 Atm",

		L_77T_FULL_TITLE,
		"CRC-77t      RADIOISOTOPE DOSE CALIBRATOR",
		"CRC-77t      CALIBRATEUR DE DOSE RADIOISOTOPE",

		L_10_CHAMBER_CANNOT_RUN_MOLY_ASSAY,
		"The 1 Atm chamber can not run a Moly Assay",
		"La chambre 1 Atm ne peut pas exécuter un essai Moly",

		L_SETUP_HOTKEYS,
		"Setup Hotkeys",
		"Config. raccourcis",

		L_ENTER_CAL,
		"Please Enter Cal #:",
		"Veuillez entrer Cal # :",

		L_SETUP_LINEARITY_STANDARD,
		"Setup Linearity Standard",
		"Config. standard linéarité",

		L_PR_C_CHAMBER,
		"HR CHAMBER",
		"CHAMBRE HR",

		L_PR_K_CHAMBER,
		"1K CHAMBER",
		"CHAMBRE 1K",

		L_C_CHAMBER,
		"HR Chamber",
		"chambre HR",

		L_K_CHAMBER,
		"1K Chamber",
		"chambre 1K",

		L_FOR_C_CHAMBER,
		"for HR Chamber",
		"pour la chambre HR",

		L_FOR_K_CHAMBER,
		"for 1K Chamber",
		"pour la chambre 1K",

		L_FOR_C_CHAMBER2,
		"for HR Chamber",
		"de chambre HR",

		L_FOR_K_CHAMBER2,
		"for 1K Chamber",
		"de chambre 1K",

		L_STANDARD_C_CHAMBER,
		"Standard HR Chamber",
		"Standard chambre HR",

		L_STANDARD_K_CHAMBER,
		"Standard 1K Chamber",
		"Standard chambre 1K",

		L_MAX_SECOND_IS_59,
		"Max Second is 59",
		"Seconde max est 59",

		L_START_MEASUREMENT,
		"Start Measurement",
		"Démarrer la mesure",

		L_SETUP_KEY,
		"Setup Key",
		"Config. clé",

		L_DEVICE_ID,
		"Device ID:",
		"ID appareil :",

		L_KEY,
		"Key:",
		"Clé :",

		L_COMM_ENABLED,
		"Communications: Enabled",
		"Communications : Activées",

		L_COMM_DISABLED,
		"Communications: Disabled",
		"Communications : Désactivées",

		L_PLS_ENTER_KEY,
		"Please enter Key",
		"Veuillez entrer la clé",

		L_ERR_EMPTY_KEY,
		"Error: Key is empty",
		"Erreur : la clé est vide",

		L_ERR_INVALID_KEY,
		"Error: Key is not valid",
		"Erreur : la clé est invalide",

		L_SETUP_PASSWORD,
		"Setup Password",
		"Config. mot de passe",

		L_CURRENT_PASSWORD,
		"Current Password:",
		"Mot de passe actuel :",

		L_NEW_PASSWORD,
		"New Password:",
		"Nouveau mot de passe :",

		L_CONFIRM_PASSWORD,
		"Confirm Password:",
		"Confirmer mot de passe :",

		L_ENTER_CURRENT_PASSWORD,
		"Please enter Current Password",
		"Veuillez entrer le mot de passe actuel",

		L_ENTER_NEW_PASSWORD,
		"Please enter New Password",
		"Veuillez entrer le nouveau mot de passe",

		L_ENTER_CONFIRM_PASSWORD,
		"Please enter Confirm Password",
		"Veuillez confirmer le mot de passe",

		L_INVALID_CURRENT_PASSWORD,
		"Invalid Current Password",
		"Mot de passe actuel invalide",

		L_MISMATCH_PASSWORD,
		"New and Confirm passwords do not match",
		"Les mots de passe ne correspondent pas",

		L_LAST_3_SN,
		"Use last 3 SN digits",
		"Utiliser 3 derniers chiffres SN",

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


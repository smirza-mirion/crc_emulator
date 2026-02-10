		/* header for menus  -- CRC-25 family*/
		/* file: menu.h */
		/* date: 09/06/06 */
        //include in all files using menus

typedef struct selection SELECTION;
struct selection
{
	char	prompt[16];
	short	ret;
};

typedef struct key_selection KEY_SELECT;
struct key_selection
{
	short	active;
	short	ret;
};

typedef struct menu MENU;
struct menu
{
	short		id;
    char        heading[16];
	SELECTION	selection[6];
	KEY_SELECT	key_select;   //MENU
};

enum menu_ids
{
    MAIN_MENU,
    MAIN_MENU_LOCK, //locked to BQ
    MAIN_MENU_E,     //Enhanced
    MAIN_MENU_E_LOCK,     //Enhanced & locked
    CAL_SETUP_BQ,
    CAL_SETUP,
    UTIL_CALC_MENU,
    WELL_SETUP,
	BETA_SETUP,
	OTHER_MENU,
    OTHER_MENU_PET,
	OTHER_MENU_E,
	OTHER_MENU_PET_E,
    TS_MENU,
    TS_MENU_PET,
    NUCLIDE_MENU_R,
    NUCLIDE_MENU_W,
    NUCLIDE_MENU_PET,
	MO_MENU,
   	FACTORY,
	FACTORY_WB,
	CAL_FACTORY,
	DETECTOR_FACTORY_SETUP,
    TESTS_MENU,
    TESTS_MENU_E,
    TESTS_MENU_PET,
    TESTS_MENU_PET_E,
    ENHANCED_TESTS_MENU,
    ENHANCED_TESTS_PET_MENU,
    BETA_LIMITS_MENU,
	WELL_NUCLIDES_MENU,
    WELL_TRIG_MENU,
    BETA_CF_MENU,
    BETA_MAIN_MENU,
    WELL_MAIN_MENU,
    CF_MENU,
    CONTAINER_MENU,
    CAL_FACTOR_MENU,
    PRINTER_MENU,
    PRINTER_OKI_MENU,
    CI_BQ_MENU,
    SCREEN_SAVER_MENU,
    CAL_TYPE_MENU,
    SCREEN_CONTROL_MENU,
    WELL_MEASURE_MENU,
    BETA_DISTANCE_MENU,
    LAB_TESTS_MENU,
    WELL_TEST_SOURCE_MENU,
    LIN_TEST_MENU,
    INVENTORY_MENU,
    INVENTORY_DELETE_MENU,
    GEOMETRY_TEST_MENU,
    QC_MENU,
    OTHER_MENU_BT,
    NUCLIDE_MENU_BT,
    SOURCES_MENU_BT,
    ADD_SOURCE_MENU_BT,
    MULTI_SOURCE_MENU_BT,    
    MULTI_SOURCE_METHOD_BT,
    TS_MENU_BT,
    PRINTER_MENU_RS232,
    PRINTER_MENU_USB,
    NUM_MENUS,    /* always make this last */
};

#define  MENU_RET   -1

//prototype display_menu
short display_menu(short menu_num);


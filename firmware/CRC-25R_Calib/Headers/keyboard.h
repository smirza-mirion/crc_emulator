#pragma once
/*********************************************************************
  MODULE:	DEFINITION FILE for keyboard usage  for crc-25

  FILE:		keyboard.h

  DATE:		09/18/06


  *************************************************************************/

#define NOKEY   (char)0xff
#define HOMEKEY 100
enum
{
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    ISO1,       //10
    ISO2,
    ISO3,
    ISO4,
    ISO5,
    ISO6,
    ISO7,
    ISO8,
    USER1,      
    USER2,
    USER3,      //20
    USER4,
    USER5,
    SOURCESBUT,
    NUCBUT,
    CALBUT,
    TIMEBUT,
    MENUBUT,
    SYSBUT,         
    OK,               
    CLR,            //30
    UP,
    DOWN,
    BKGBUT,        
    DET_KEY,
    COUNT_KEY,
    DISP_KEY,
    SVR_KEY,        
    TESTBUT,      //38
};

#define CHAMB_KEY  TIMEBUT
#define TIMEBUT_PET TESTBUT

enum Pet_User_Keys
{
    USER_PET_1  = 14,
    USER_PET_2,
    USER_PET_3,
    USER_PET_4,
    USER_PET_5,
    USER_PET_6,
    USER_PET_7,
    USER_PET_8,
    USER_PET_9,

};    
#define ENTER OK

#define RO 0x7F
#define CR 0x0D
#define DP UP

#define  YPROMPT    44

void scan_keyboard(void);
void beep(void);
char getkey(void);
void set_nokey(void);
void crc_setkey(char ch);  /* renamed from setkey to avoid POSIX conflict */
char keyin(void);
void clear_home(void);
bool home_set(void);
void prompt_date_time(void);
void dokey(short num);
void get_keybuf(char *str,short pos, short num);
void id_init(void);
void set_idnum(void);
bool id_set(void);
void get_id_str(char *str);
bool input_time_set(void);
void keybuf_bsp(void);
void init_screen_saver(void);
void key_press_screen_save(void);
void enter_idnum(short xpos, short ypos);
void service_watchdog(void);
void service_pc(void);


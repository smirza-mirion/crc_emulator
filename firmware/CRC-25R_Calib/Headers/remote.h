#pragma once
/*******************************************************************************
  MODULE:  Header for Remotes

  FILE:    remote.h

  DATE:     09/18/06
  *******************************************************************************/


//values of remote[].nuclist
enum nuclist
{
    REMOTE_NUCLIDE_OTHER = -1, //nuclide not set by remote
    REMOTE_NUCLIDE_1,
    REMOTE_NUCLIDE_2,
    REMOTE_NUCLIDE_3,
    REMOTE_NUCLIDE_4,
    REMOTE_NUCLIDE_5,
    REMOTE_NUCLIDE_6,
    REMOTE_NUCLIDE_7,
    REMOTE_NUCLIDE_8,
};

enum acclist
{
    REMOTE_ACCURACY_TEST1,
    REMOTE_ACCURACY_TEST2,  //2nd possible source
    REMOTE_ACCURACY_TEST3,  //3rd possible source
    REMOTE_ACCURACY_TEST4,  //4th possible source
    REMOTE_ACCURACY_TEST5,  //5th possible source
};
//chamber control values
enum    chambercontrol
{
    CONTROL_MAIN,
    CONTROL_REMOTE,
    CONTROL_PC,
};

//commands from Remote
#define REMOTE_NO_CMD           'O'
#define REMOTE_NEXT_NUCLIDE     'N'
#define REMOTE_READ_ACTIVITY    'A'
#define REMOTE_BKG              'G'
#define REMOTE_BIAS             'V'
#define REMOTE_ZERO             'Z'
#define REMOTE_ACCURACY         'B'
#define REMOTE_EXISTS           'Y'

//remote's modes
enum remote_modes
{
    REMOTE_HEARTBEAT_MODE = -1,
    REMOTE_ACTIVITY_MODE,
    REMOTE_NUCLIDE_MODE,
    REMOTE_IN_PROGRESS_MODE,
    REMOTE_BKG_MODE,
    REMOTE_BIAS_MODE,
    REMOTE_ZERO_MODE,
    REMOTE_ACC_SOURCE_MODE,
    REMOTE_ACC_MODE,

};  

void remote_heartbeat(short ch_num);
void remote_activity(short ch_num);
void remote_value(short ch_num,char *str, char type,char status);
void remote_in_progress(short ch_num,char type);
void remote_next_nuclide(short ch_num);
void remote_accept_nuclide(short ch_num);
bool test_accepted(short ch_num, char test_type);
void remote_busy(bool flag);
void busy_sent(void);
void set_pc_freeze_flag(bool flag);

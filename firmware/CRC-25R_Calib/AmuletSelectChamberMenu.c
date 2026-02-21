#define PHASE_SELECT_CHAMBER_PRE_INIT 0
#define PHASE_SELECT_CHAMBER_WAIT 1
#define PHASE_SELECT_CHAMBER_CH_1 2
#define PHASE_SELECT_CHAMBER_CH_2 3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "chambfac.h"
#include "Amulet.h"
#include "keyboard.h"
#include "screen.h"

extern int m_iPhase;
extern CURRENT  current;
extern MEASUREMENT measurement[];
extern CHAMBER chamber[];

void send_to_amulet_string(uchar ucIndex, char message0[]);
void SetAmuletBackHTML(void);

void AmuletSelectChamber_menu(void){
	char message[25];

	switch(m_iPhase){
		case PHASE_SELECT_CHAMBER_PRE_INIT:
			send_amulet_message(L_SELECT_CHAMBER, 102);    // "Select Chamber"
			send_amulet_message(L_CANCEL_BUTTON, 104);    // "Cancel"
			//SetAmuletWord(101, m_uiOffsetX + 0);
			//SetAmuletWord(102, m_uiOffsetY + 0);
			//SetAmuletWord(103, m_uiOffsetX + 25);
			//SetAmuletWord(104, m_uiOffsetY + 2);
			//SetAmuletWord(105, m_uiOffsetX + 75);
			//SetAmuletWord(106, m_uiOffsetY + 87);
			//SetAmuletWord(107, m_uiOffsetX + 75);
			//SetAmuletWord(108, m_uiOffsetY + 187);

			strcpy(message, "Ch1 ");
			if(chamber_type(0)==R_CHAMB) strcat(message, "(R)");
			else if(chamber_type(0)==P_CHAMB) strcat(message, "(PET)");
			else if(chamber_type(0)==B_CHAMB) strcat(message, "(BT)");
			else if(chamber_type(0)==ONE_DOT_EIGHT_CHAMB) strcat(message, "(1.8 Atm)");
			else if(chamber_type(0)==C_CHAMB) strcat(message, "(HR)");
			else if(chamber_type(0)==K_CHAMB) strcat(message, "(1K)");
			send_to_amulet_string(100, message);

			strcpy(message, "Ch2 ");
			if(chamber_type(1)==R_CHAMB) strcat(message, "(R)");
			else if(chamber_type(1)==P_CHAMB) strcat(message, "(PET)");
			else if(chamber_type(1)==B_CHAMB) strcat(message, "(BT)");
			else if(chamber_type(1)==ONE_DOT_EIGHT_CHAMB) strcat(message, "(1.8 Atm)");
			else if(chamber_type(1)==C_CHAMB) strcat(message, "(HR)");
			else if(chamber_type(1)==K_CHAMB) strcat(message, "(1K)");
			send_to_amulet_string(101, message);

			SetAmuletByte(100, 0xFF);
			m_iPhase = 1;
			break;

		case PHASE_SELECT_CHAMBER_WAIT:
			break;

		case PHASE_SELECT_CHAMBER_CH_1:
			current.main_chamber = 0;
			measurement[current.main_chamber].idec = measurement[current.main_chamber].idecmin;
			measurement[current.main_chamber].act0 = 0.;
			// Removed:
			//measurement[current.main_chamber].syst0 = -1;
			measurement[current.main_chamber].kun0 = 0;
			strcpy(&measurement[current.main_chamber].actstr0[0],"xxxxxx");
			measurement[current.main_chamber].future.dosetime = NO_TIME;
			id_init();
			erase_screen();
			//disp_cal(measurement[j].response);
			disp_cal(measurement[current.main_chamber].resp0);
			measurement[current.main_chamber].wasover = FALSE;
			display_chamber();
			if(!chamber[current.main_chamber].calkey)
				disp_nuclide();

			//setup for displaying time
			set_display_time();
			SetAmuletBackHTML();
			break;

		case PHASE_SELECT_CHAMBER_CH_2:
			current.main_chamber = 1;
			measurement[current.main_chamber].idec = measurement[current.main_chamber].idecmin;
			measurement[current.main_chamber].act0 = 0.;
			// Removed:
			//measurement[current.main_chamber].syst0 = -1;
			measurement[current.main_chamber].kun0 = 0;
			strcpy(&measurement[current.main_chamber].actstr0[0],"xxxxxx");
			measurement[current.main_chamber].future.dosetime = NO_TIME;
			id_init();
			erase_screen();
			//disp_cal(measurement[j].response);
			disp_cal(measurement[current.main_chamber].resp0);
			measurement[current.main_chamber].wasover = FALSE;
			display_chamber();
			if(!chamber[current.main_chamber].calkey)
				disp_nuclide();

			//setup for displaying time
			set_display_time();
			SetAmuletBackHTML();
			break;
	}
}

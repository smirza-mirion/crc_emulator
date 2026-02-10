#define PHASE_DELETE_ALL_INVENTORY_PRE_INIT	0
#define PHASE_DELETE_ALL_INVENTORY_WAIT		1
#define PHASE_DELETE_ALL_INVENTORY_YES		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void write_Inventory(void);

extern int m_iPhase;
extern INVENTORY inventory[MAX_INVENTORY];
extern CURRENT current;

void AmuletDeleteAllInv_menu(void){
	short index;
	//char message[50];

	switch(m_iPhase){
		case PHASE_DELETE_ALL_INVENTORY_PRE_INIT:

			SetAmuletByte(92,current.language);			
			
			send_amulet_message(L_DELETE_ALL_INVENTORY,100);    // "Delete All Inventory"
			send_amulet_message(L_DELETE_ALL_INVENTORY_Q,102);    // "Delete All Inventory?"
			send_amulet_message(L_YES,104);    // "Yes"
			send_amulet_message(L_NO,105);    // "No"
			
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_DELETE_ALL_INVENTORY_WAIT;
			break;

		case PHASE_DELETE_ALL_INVENTORY_WAIT:
			break;

		case PHASE_DELETE_ALL_INVENTORY_YES:
			beep_amulet();
			for(index=0; index<MAX_INVENTORY; index++) inventory[index].nucnum = -1;
			write_Inventory();
			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_DELETE_ALL_INVENTORY_WAIT;
			break;
	}
}

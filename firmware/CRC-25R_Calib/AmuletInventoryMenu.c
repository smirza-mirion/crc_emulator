#define PHASE_INVENTORY_PRE_INIT 0
#define PHASE_INVENTORY_REFRESH_SCREEN 1
#define PHASE_INVENTORY_WAIT 2
#define PHASE_INVENTORY_PAGE_DOWN 3
#define PHASE_INVENTORY_PAGE_UP 4
#define PHASE_INVENTORY_ROW1 5
#define PHASE_INVENTORY_ROW2 6
#define PHASE_INVENTORY_ROW3 7
#define PHASE_INVENTORY_ROW4 8
#define PHASE_INVENTORY_ROW5 9
#define PHASE_INVENTORY_ROW6 10
#define PHASE_INVENTORY_ROW7 11
#define PHASE_INVENTORY_ROW8 12
#define PHASE_INVENTORY_ROW9 13
#define PHASE_INVENTORY_ROW10 14
#define PHASE_INVENTORY_PRINT 15

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "chambfac.h"
#include "printer.h"

#define ITEMS_PER_PAGE 10

extern CURRENT  current;
extern int m_iPhase;
extern short int m_iSelectedMapItem;
extern INVENTORY inventory[MAX_INVENTORY];
extern short int m_iMap[MAX_INVENTORY];

unsigned char PopPageStack(void);
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);
void Amulet_printInventory(void);

void AmuletInventory_menu(void) {
	short i;
	static short first_map_item_displayed;
	static short last_map_item;
	short ch_num;
	char message[50];
	char message1[77];

	switch(m_iPhase) {
		case PHASE_INVENTORY_PRE_INIT:
			ch_num = current.main_chamber;

			if(chamber_type(ch_num) == P_CHAMB){
				PopPageStack();
				//Amulet_DisplayNotification("PET Chamber", "The PET Chamber can not access inventory functions", TRUE);
				get_amulet_message(L_PR_PET_CHAMBER,message);    // "PET CHAMBER"
				get_amulet_message(L_PET_CHAMBER_NO_INVENTORY,message1);    // "The PET Chamber can not access inventory functions"
				Amulet_DisplayNotification(message,message1,TRUE);
				return;
			}else{
				first_map_item_displayed = -1;
				m_iSelectedMapItem = -1;
				m_iPhase = PHASE_INVENTORY_REFRESH_SCREEN;
			}

			SetAmuletByte(92,current.language);

			send_amulet_message(L_INVENTORY,200);    // "Inventory"
			send_amulet_message(L_INVENTORY_LIST,202);    // "Inventory List"
			send_amulet_message(L_STUDY,203);    // "Study"
			send_amulet_message(L_LOT,204);    // "Lot"
			send_amulet_message(L_DOSE_TABLE_VOLUME,205);    // "Volume"
			send_amulet_message(L_ACTIVITY,206);    // "Activity"
			send_amulet_message(L_NUCLIDE,207);    // "Nuclide"
			send_amulet_message(L_ID,208);    // "ID"
			send_amulet_message(L_CONC,209);    // "Conc"

			SetAmuletByte(233,0xff);

			break;

		case PHASE_INVENTORY_REFRESH_SCREEN:
			// Scan inventory array for items
			last_map_item = -1;
			for (i=0; i<MAX_INVENTORY; i++) {
				if (inventory[i].nucnum != -1) {
					last_map_item++;
					m_iMap[last_map_item] = i;
				}
			}

			// Display List and Page Buttons
			display_inventory(m_iMap, &first_map_item_displayed, &m_iSelectedMapItem, last_map_item);

			send_amulet_message(L_ADD_ITEM,210);    // "Add Item"
			send_amulet_message(L_DELETE_ALL,211);    // "Delete All"

			// Decide to display Add Item button
			if (last_map_item >= MAX_INVENTORY - 1) {
				SetAmuletByte(241, 0);
			}
			else {
				SetAmuletByte(241, 1);
			}

			// Decide to display Withdraw button
			// Decide to display Make Kit button
			// Decide to display Delete Item button
			send_amulet_message(L_WITHDRAW,212);    // "Withdraw"
			send_amulet_message(L_MAKE_KIT,213);    // "Make Kit"
			send_amulet_message(L_DELETE_ITEM,214);    // "Delete Item"

			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 0);
				SetAmuletByte(239, 0);
				SetAmuletByte(238, 0);
			}
			else {
				SetAmuletByte(240, 1);
				// Display Make Kit only if there is an empty inventory slot
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[m_iSelectedMapItem].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
			}

			// Decide to display print button
			if (last_map_item == -1) {
				SetAmuletByte(237, 0);
				SetAmuletByte(231, 0);
			}
			else {
				//if(current.printer != NONE_PRINTER)
				if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
					SetAmuletByte(237, 1);
				else SetAmuletByte(237, 0);
				SetAmuletByte(231, 1);
			}

			SetAmuletByte(235, 0xFF);
			SetAmuletByte(236, 0xFF);

			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_WAIT:
			break;

		case PHASE_INVENTORY_PAGE_DOWN:
			i = first_map_item_displayed;
			i += ITEMS_PER_PAGE;
			if (i > last_map_item) {
				i = last_map_item / ITEMS_PER_PAGE;
				i *= ITEMS_PER_PAGE;
			}
			first_map_item_displayed = i;

			m_iPhase= PHASE_INVENTORY_REFRESH_SCREEN;
			break;

		case PHASE_INVENTORY_PAGE_UP:
			i = first_map_item_displayed;
			i -= ITEMS_PER_PAGE;
			if (i < 0) i = 0;

			first_map_item_displayed = i;
			m_iPhase = PHASE_INVENTORY_REFRESH_SCREEN;
			break;

		case PHASE_INVENTORY_ROW1:
			i = 0;
			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 1);
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
				SetAmuletByte(236, 0xFF);
			}else{
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(236, 0xFF);
			}
			m_iSelectedMapItem = first_map_item_displayed + i;
			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_ROW2:
			i = 1;
			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 1);
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
				SetAmuletByte(236, 0xFF);
			}else{
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(236, 0xFF);
			}
			m_iSelectedMapItem = first_map_item_displayed + i;
			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_ROW3:
			i = 2;
			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 1);
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
				SetAmuletByte(236, 0xFF);
			}else{
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(236, 0xFF);
			}
			m_iSelectedMapItem = first_map_item_displayed + i;
			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_ROW4:
			i = 3;
			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 1);
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
				SetAmuletByte(236, 0xFF);
			}else{
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(236, 0xFF);
			}
			m_iSelectedMapItem = first_map_item_displayed + i;
			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_ROW5:
			i = 4;
			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 1);
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
				SetAmuletByte(236, 0xFF);
			}else{
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(236, 0xFF);
			}
			m_iSelectedMapItem = first_map_item_displayed + i;
			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_ROW6:
			i = 5;
			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 1);
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
				SetAmuletByte(236, 0xFF);
			}else{
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(236, 0xFF);
			}
			m_iSelectedMapItem = first_map_item_displayed + i;
			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_ROW7:
			i = 6;
			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 1);
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
				SetAmuletByte(236, 0xFF);
			}else{
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(236, 0xFF);
			}
			m_iSelectedMapItem = first_map_item_displayed + i;
			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_ROW8:
			i = 7;
			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 1);
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
				SetAmuletByte(236, 0xFF);
			}else{
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(236, 0xFF);
			}
			m_iSelectedMapItem = first_map_item_displayed + i;
			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_ROW9:
			i = 8;
			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 1);
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
				SetAmuletByte(236, 0xFF);
			}else{
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(236, 0xFF);
			}
			m_iSelectedMapItem = first_map_item_displayed + i;
			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_ROW10:
			i = 9;
			if (m_iSelectedMapItem == -1) {
				SetAmuletByte(240, 1);
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(238, 1);
				SetAmuletByte(236, 0xFF);
			}else{
				if(last_map_item >= MAX_INVENTORY - 1) SetAmuletByte(239, 0);
				else{
					if(inventory[first_map_item_displayed + i].type == -1) SetAmuletByte(239, 1);
					else SetAmuletByte(239, 0);
				}
				SetAmuletByte(236, 0xFF);
			}
			m_iSelectedMapItem = first_map_item_displayed + i;
			m_iPhase = PHASE_INVENTORY_WAIT;
			break;

		case PHASE_INVENTORY_PRINT:
			beep_amulet();
			Amulet_printInventory();
			//m_iPhase = PHASE_INVENTORY_WAIT;
			m_iPhase= PHASE_INVENTORY_REFRESH_SCREEN;
			break;
	}
}

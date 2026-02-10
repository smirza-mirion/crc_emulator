#define PHASE_STUDY_PRE_INIT 0
#define PHASE_STUDY_NONE 1
#define PHASE_STUDY_BONE 2
#define PHASE_STUDY_LUNG 3
#define PHASE_STUDY_HIDA 4
#define PHASE_STUDY_HEART 5
#define PHASE_STUDY_RENAL 6
#define PHASE_STUDY_LIVER 7
#define PHASE_STUDY_BRAIN 8
#define PHASE_STUDY_LYMPH 9
#define PHASE_STUDY_WAIT 10

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "Amulet.h"

extern int m_iPhase;
extern CURRENT  current;
extern short m_iKitInventory_KitStudy;

void SetAmuletBackHTML(void);

void AmuletStudy2_menu(void) {
	switch(m_iPhase) {
		case PHASE_STUDY_PRE_INIT:
				SetAmuletByte(80, current.language);
				send_amulet_message(L_PLEASE_SELECT_STUDY, 100);    // "Please select study"
				SetAmuletByte(100, 0xFF);
				m_iPhase = PHASE_STUDY_WAIT;
				break;

		case PHASE_STUDY_NONE:
			m_iKitInventory_KitStudy = -1;
			SetAmuletBackHTML();
			break;

		case PHASE_STUDY_BONE:
			m_iKitInventory_KitStudy  = 0;
			SetAmuletBackHTML();
			break;

		case PHASE_STUDY_LUNG:
			m_iKitInventory_KitStudy  = 1;
			SetAmuletBackHTML();
			break;

		case PHASE_STUDY_HIDA:
			m_iKitInventory_KitStudy  = 2;
			SetAmuletBackHTML();
			break;

		case PHASE_STUDY_HEART:
			m_iKitInventory_KitStudy  = 3;
			SetAmuletBackHTML();
			break;

		case PHASE_STUDY_RENAL:
			m_iKitInventory_KitStudy  = 4;
			SetAmuletBackHTML();
			break;

		case PHASE_STUDY_LIVER:
			m_iKitInventory_KitStudy  = 5;
			SetAmuletBackHTML();
			break;

		case PHASE_STUDY_BRAIN:
			m_iKitInventory_KitStudy  = 6;
			SetAmuletBackHTML();
			break;

		case PHASE_STUDY_LYMPH:
			m_iKitInventory_KitStudy  = 7;
			SetAmuletBackHTML();
			break;

		case PHASE_STUDY_WAIT:
			break;
	}
}

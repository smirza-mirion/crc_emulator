#define PHASE_ETHERNET_PRE_INIT	0
#define PHASE_ETHERNET_WAIT		1
#define PHASE_ETHERNET_SEND		2
#define PHASE_ETHERNET_MIRROR	3
#define PHASE_ETHERNET_DELAY_SEND 4
#define PHASE_ETHERNET_SEND_NEXT	5
#define PHASE_ETHERNET_BACK		6
#define PHASE_ETHERNET_HOME		7

#include <stdlib.h>
#include <string.h>
#include "crc.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern volatile unsigned long int g_csec_tstamp;
extern bool ethernet_echo;
extern short ethernet_echo_status;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void ethernet_init(void);
unsigned short int ethernet_connection_status(void);
void ethernet_send_ping(void);
void ethernet_reset(void);
void SetAmuletHomeHTML(void);
void SetAmuletBackHTML(void);

unsigned long AmuletEthernet_count;

void AmuletEthernet_menu(void){
	static unsigned long int delay;
	unsigned short int status;
	char countstring[20];

	switch(m_iPhase){
		case PHASE_ETHERNET_PRE_INIT:
			if(m_ucClear==51){
				ethernet_init();
				m_ucClear = 0;
			}
			SetAmuletByte(100, 0xFF);
			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_ETHERNET_WAIT;
			break;

		case PHASE_ETHERNET_WAIT:
			break;

		case PHASE_ETHERNET_SEND:
			status = ethernet_connection_status();
			if((status & 0x04) == 0x00){
				Amulet_DisplayError("Ethernet Test", "Unable to detect connection\nPlease check cable", TRUE);
				return;
			}
			AmuletEthernet_count = 0;
			SetAmuletByte(102, 0xFF);
			ethernet_send_ping();
			delay = g_csec_tstamp + 100;
			m_iPhase = PHASE_ETHERNET_DELAY_SEND;
			break;

		case PHASE_ETHERNET_MIRROR:
			status = ethernet_connection_status();
			if((status & 0x04) == 0x00){
				Amulet_DisplayError("Ethernet Test", "Unable to detect connection\nPlease check cable", TRUE);
				return;
			}
			SetAmuletByte(103, 0xFF);
			ethernet_echo = TRUE;
			m_iPhase = PHASE_ETHERNET_WAIT;
			break;

		case PHASE_ETHERNET_DELAY_SEND:
			if(ethernet_echo_status == 0){
				if(g_csec_tstamp > delay){
					//SetAmuletByte(100, 0xFF);
					Amulet_DisplayError("Ethernet Test", "Echo Timeout Error", TRUE);
					return;
					//m_iPhase = PHASE_ETHERNET_WAIT;
				}
			}else if(ethernet_echo_status == 1){
				AmuletEthernet_count++;
				if((AmuletEthernet_count%10) == 0){
					sprintf(countstring, "%lu\n", AmuletEthernet_count);
					SetAmuletString(100, countstring);
				}
				m_iPhase = PHASE_ETHERNET_SEND_NEXT;
			}else if(ethernet_echo_status == -1){
				Amulet_DisplayError("Ethernet Test", "Echo does not match original", TRUE);
				return;
			}else if(ethernet_echo_status == -2){
				Amulet_DisplayError("Ethernet Test", "Truncated Echo Frame", TRUE);
				return;
			}else if(ethernet_echo_status == -3){
				Amulet_DisplayError("Ethernet Test", "Overrun in Echo Frame", TRUE);
				return;
			}else if(ethernet_echo_status == -4){
				Amulet_DisplayError("Ethernet Test", "CRC Error in Echo Frame", TRUE);
				return;
			}else if(ethernet_echo_status == -5){
				Amulet_DisplayError("Ethernet Test", "Byte Boundary Error in Echo Frame", TRUE);
				return;
			}else if(ethernet_echo_status == -6){
				Amulet_DisplayError("Ethernet Test", "Frame Length Error in Echo Frame", TRUE);
				return;
			}else{
				Amulet_DisplayError("Ethernet Test", "Unknown Echo Status", TRUE);
				return;
			}
			break;

		case PHASE_ETHERNET_SEND_NEXT:
			ethernet_send_ping();
			delay = g_csec_tstamp + 100;
			m_iPhase = PHASE_ETHERNET_DELAY_SEND;
			break;

		case PHASE_ETHERNET_BACK:
			ethernet_reset();
			SetAmuletBackHTML();
			beep_amulet();
			return;
			//break;

		case PHASE_ETHERNET_HOME:
			ethernet_reset();
			SetAmuletHomeHTML();
			beep_amulet();
			return;
			//break;
	}
}

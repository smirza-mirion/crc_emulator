/*********************************************************************

  MODULE:	ETHERNET for CRC-25R

  FILE:		ethernet.c

  DATE:     05/23/07
            08/22/07 -- removed automated constancy test

  CALLED BY:
		factory menu


  *************************************************************************/
#include "coldfire.h"
#include "crc.h"
#include "keyboard.h"
#include <stdio.h>
#include <stdlib.h>

extern ETHERTXDESC ethertxdesc[10];
extern uchar ethertxbuffer[10][2032];
extern ETHERRXDESC etherrxdesc[10];
extern uchar etherrxbuffer[10][2032];
extern time_t clock_time;

__interrupt void ether_isr(void);

volatile long counter;
volatile long framecounter;
volatile long buffercounter;
unsigned long int physaddress;
unsigned char macaddress[6];
bool ethernet_echo;
short ethernet_echo_status;

void ethernet_reset(void){
	cf.fec.ecr = 0x00000001;
}

void ethernet_init(void){
	unsigned long int index;
	//char ch;
	unsigned long int receiveaddress;
	unsigned long int transmitaddress;
	//unsigned long int ledon;
	unsigned long int basic;
	unsigned long int status;
	unsigned long int value;
	long currentcounter;

	counter = 0;
	framecounter = 0;
	buffercounter = 0;
	ethernet_echo = FALSE;

	srand((unsigned int) clock_time);
	for(index=0; index<6; index++) macaddress[index] = (unsigned char) (rand() & 0xFF);
	//printf("%0X:%0X:%0X:%0X:%0X:%0X\n", macaddress[0], macaddress[1], macaddress[2], macaddress[3], macaddress[4], macaddress[5]);

	cf.fec.ecr = 0x00000001;
	delayloop(10);

	// Enable MII Interupt mask, 10-8
	cf.intc[0].imrl = 0x007FFFFE & cf.intc[0].imrl;
	cf.intc[0].imrh = 0xFFFFFFF0 & cf.intc[0].imrh;
	cf.intc[0].icrn[35] = 0x24;
	cf.intc[0].icrn[34] = 0x24;
	cf.intc[0].icrn[33] = 0x24;
	cf.intc[0].icrn[32] = 0x24;
	cf.intc[0].icrn[31] = 0x24;
	cf.intc[0].icrn[30] = 0x24;
	cf.intc[0].icrn[29] = 0x24;
	cf.intc[0].icrn[28] = 0x24;
	cf.intc[0].icrn[27] = 0x24;
	cf.intc[0].icrn[26] = 0x24;
	cf.intc[0].icrn[25] = 0x24;
	cf.intc[0].icrn[24] = 0x24;
	cf.intc[0].icrn[23] = 0x24;

	vector_base.table[64+23] = (unsigned long) &ether_isr;
	vector_base.table[64+24] = (unsigned long) &ether_isr;
	vector_base.table[64+25] = (unsigned long) &ether_isr;
	vector_base.table[64+26] = (unsigned long) &ether_isr;
	vector_base.table[64+27] = (unsigned long) &ether_isr;
	vector_base.table[64+28] = (unsigned long) &ether_isr;
	vector_base.table[64+29] = (unsigned long) &ether_isr;
	vector_base.table[64+30] = (unsigned long) &ether_isr;
	vector_base.table[64+31] = (unsigned long) &ether_isr;
	vector_base.table[64+32] = (unsigned long) &ether_isr;
	vector_base.table[64+33] = (unsigned long) &ether_isr;
	vector_base.table[64+34] = (unsigned long) &ether_isr;
	vector_base.table[64+35] = (unsigned long) &ether_isr;

	// Configure GPIO Pins for ethernet
	// Set EMDIO and EMDC, 26-20
	cf.gpio.paspar = (cf.gpio.paspar | 0x0F00) & 0x0FFF;
	// Set All other Ethernet pins, 26-21
	cf.gpio.pehlpar = 0xC0;

	// Set MII Ethernet Interrupts, 17-26
	cf.fec.eimr = 0xFFF80000;
	// Clear Ethernet Interrupt Status Register, 17-23
	cf.fec.eir = 0xFFF80000;

	// Set Transmit Watermark, 17-40
	cf.fec.tfwr = 0x00000000;

	// Set the IAUR, IALR, let everything thru 17-37, 17-38
	cf.fec.iaur = 0xFFFFFFFF;
	cf.fec.ialr = 0xFFFFFFFF;

	// Set the GAUR, GALR, let everything thru 17-39, 17-40
	cf.fec.gaur = 0xFFFFFFFF;
	cf.fec.galr = 0xFFFFFFFF;

	// Set the PALR, PAUR, 17-35, 17-36
	cf.fec.palr = 0x00000000;
	cf.fec.paur = 0x00000000;

	// Set the OPD, 17-37
	cf.fec.opd = 0x00000001;

	// Set the RCR, 17-33
	cf.fec.rcr = 0x05EE002C;

	// Set the TCR, 17-34
	cf.fec.tcr = 0x00000004;

	// Set the EMRBR, 17-44
	cf.fec.emrbr = 0x000007F0;

	for(index=0; index<10; index++){
		if(index == 9){
			etherrxdesc[index].flag = 0xA000;
		}else{
			etherrxdesc[index].flag = 0x8000;
		}
		etherrxdesc[index].length = 2032;
		etherrxdesc[index].buffer = &(etherrxbuffer[index][0]);
		ethertxdesc[index].buffer = &(ethertxbuffer[index][0]);
	}

	ethertxdesc[0].flag = 0x2C00;
	ethertxdesc[0].length = 2032;
	ethertxdesc[0].buffer = &(ethertxbuffer[0][0]);

	ethertxdesc[1].flag = 0x2C00;
	ethertxdesc[1].length = 2032;
	ethertxdesc[1].buffer = &(ethertxbuffer[1][0]);

	// Set the ERDSR, 17-42
	receiveaddress = (unsigned long) &(etherrxdesc[0]);
	cf.fec.erdsr = receiveaddress;

	// Set the ETDSR, 17-43
	transmitaddress = (unsigned long) &(ethertxdesc[0]);
	cf.fec.etdsr = transmitaddress;

	cf.fec.mscr = 0x00000004;

	physaddress = 1;
	basic = 0x50021100;
	//ledon = 0x50620038;
	status = 0x60060000;

	currentcounter = counter;
	value = basic | (physaddress << 23);
	cf.fec.mdata = value;
	while(currentcounter == counter);
	delayloop(3);

	currentcounter = counter;
	value = status | (physaddress << 23);
	cf.fec.mdata = value;
	while(currentcounter == counter);
	delayloop(3);

	value = cf.fec.mdata;
	if((value & 0xFFFF) == 0xFFFF){
		for(index=0; index<32; index++){
			currentcounter = counter;
			value = basic | (index << 23);
			cf.fec.mdata = value;
			while(currentcounter == counter);
			delayloop(20);

			currentcounter = counter;
			value = status | (index << 23);
			cf.fec.mdata = value;
			while(currentcounter == counter);
			delayloop(20);

			value = cf.fec.mdata;
			if((value & 0xFFFF)!= 0xFFFF){
				physaddress = index;
			}
		}
	}

	// Set ECR, 17-28
	cf.fec.ecr = 0x00000002;
	cf.fec.rdar = 0x00000000;

	/*while(1){
		delayloop(100);
		printf("Frame Counter: %ld\n", framecounter);
		printf("Buffer Counter: %ld\n", buffercounter);
	}*/

	/*ethertxbuffer[0][0] = 0xFF;
	ethertxbuffer[0][1] = 0xFF;
	ethertxbuffer[0][2] = 0xFF;
	ethertxbuffer[0][3] = 0xFF;
	ethertxbuffer[0][4] = 0xFF;
	ethertxbuffer[0][5] = 0xFF;
	ethertxbuffer[0][6] = 0x00;
	ethertxbuffer[0][7] = 0x11;
	ethertxbuffer[0][8] = 0x22;
	ethertxbuffer[0][9] = 0x33;
	ethertxbuffer[0][10] = 0x44;
	ethertxbuffer[0][11] = 0x55;
	ethertxbuffer[0][12] = 0x07;
	ethertxbuffer[0][13] = 0x00;
	for(index=0; index<46; index++) ethertxbuffer[0][13 + index] = index;

	// Set ECR, 17-28
	cf.fec.ecr = 0x00000002;
	delayloop(20);

	while(1){
		ch = getkey();
		ethertxdesc[0].buffer = &(ethertxbuffer[0][0]);
		ethertxdesc[0].length = 60;
		ethertxdesc[0].flag = 0x8C00;
		cf.fec.tdar = 0x01000000;
		cf.fec.ecr = 0x00000002;
		delayloop(200);

		//cf.fec.mdata = 0x50E20038;
		//delayloop(100);
		//cf.fec.mdata = 0x50E2003F;

		//currentcounter = counter;
		//cf.fec.mdata = (ledon | (physaddress << 23));
		//while(counter == currentcounter);
		//delayloop(100);

		//currentcounter = counter;
		//cf.fec.mdata = ((ledon | (physaddress << 23)) | 0x00000007);
		//while(counter == currentcounter);
		//delayloop(100);
	}*/
}

__interrupt void ether_isr(void){
	ulong value;
	short index, jndex;
	short length;
	bool duplicate;
	bool match;

	value = cf.fec.eir;

	if((value & 0x00080000)!= 0){
		printf("Transmit FIFO Underrun\n");
	}

	if((value & 0x00100000)!= 0){
		printf("Collision Retry Limit\n");
	}

	if((value & 0x00200000)!= 0){
		printf("Late Collision\n");
	}

	if((value & 0x00400000)!= 0){
		printf("Ethernet Bus Error\n");
	}

	if((value & 0x00800000)!= 0){
		counter++;
	}

	if((value & 0x01000000)!= 0){
		buffercounter++;
		printf("Receive Buffer\n");
		for(index=0; index<10; index++){
			if((etherrxdesc[index].flag & 0x8000) == 0x0000){
				//printf("Buffer Index: %d\n", index);
			}
		}
		cf.fec.rdar = 0x00000000;
	}

	if((value & 0x02000000)!= 0){
		framecounter++;
		for(index=0; index<10; index++){
			if((etherrxdesc[index].flag & 0x8000) == 0x0000){
				length = etherrxdesc[index].length - 4;
				if((etherrxbuffer[index][0] == 0xFF) && (etherrxbuffer[index][1] == 0xFF) && (etherrxbuffer[index][2] == 0xFF) && (etherrxbuffer[index][3] == 0xFF) && (etherrxbuffer[index][4] == 0xFF) && (etherrxbuffer[index][5] == 0xFF) && (etherrxbuffer[index][12] == 'W') && (etherrxbuffer[index][13] == 'L')){
					// Send Echo
					if(ethernet_echo){
						duplicate = TRUE;
						for(jndex=0; jndex<length; jndex++){
							if(etherrxbuffer[index][jndex] != ethertxbuffer[1][jndex]){
								duplicate = FALSE;
								break;
							}
						}

						if(!duplicate){
							for(jndex=0; jndex<length; jndex++) ethertxbuffer[1][jndex] = etherrxbuffer[index][jndex];
							if((ethertxdesc[0].flag & 0x8000)== 0){
								for(jndex=0; jndex<6; jndex++) ethertxbuffer[0][jndex] = etherrxbuffer[index][jndex + 6];
								for(jndex=0; jndex<6; jndex++) ethertxbuffer[0][jndex+6] = macaddress[jndex];
								ethertxbuffer[0][12] = 'X';
								ethertxbuffer[0][13] = 'M';
								for(jndex=0; jndex<length-14; jndex++) ethertxbuffer[0][jndex + 14] = etherrxbuffer[index][jndex + 14];
								ethertxdesc[0].length = length;
								ethertxdesc[0].flag = ethertxdesc[0].flag | 0x8000;
								cf.fec.tdar = 0x01000000;
							}else{
								printf("Error Transmit Echo Buffer Already filled!!!\n");
							}
						}
					}
				}else if((etherrxbuffer[index][0] == macaddress[0]) && (etherrxbuffer[index][1] == macaddress[1]) && (etherrxbuffer[index][2] == macaddress[2]) && (etherrxbuffer[index][3] == macaddress[3]) && (etherrxbuffer[index][4] == macaddress[4]) && (etherrxbuffer[index][5] == macaddress[5]) && (etherrxbuffer[index][12] == 'X') && (etherrxbuffer[index][13] == 'M')){
					// Compare against Current
					if(!ethernet_echo){
						duplicate = TRUE;
						for(jndex=0; jndex<length; jndex++){
							if(etherrxbuffer[index][jndex] != ethertxbuffer[1][jndex]){
								duplicate = FALSE;
								break;
							}
						}

						if(!duplicate){
							for(jndex=0; jndex<length; jndex++) ethertxbuffer[1][jndex] = etherrxbuffer[index][jndex];

							if((etherrxdesc[index].flag & 0x01) != 0x00){
								ethernet_echo_status = -2;
							}else if((etherrxdesc[index].flag & 0x02) != 0x00){
								ethernet_echo_status = -3;
							}else if((etherrxdesc[index].flag & 0x04) != 0x00){
								ethernet_echo_status = -4;
							}else if((etherrxdesc[index].flag & 0x10) != 0x00){
								ethernet_echo_status = -5;
							}else if((etherrxdesc[index].flag & 0x20) != 0x00){
								ethernet_echo_status = -6;
							}else{
								match = TRUE;
								for(jndex=0; jndex<length-14; jndex++){
									if(ethertxbuffer[0][jndex+14] != etherrxbuffer[index][jndex+14]){
										match = FALSE;
										break;
									}
								}

								if(match){
									ethernet_echo_status = 1;
								}else{
									ethernet_echo_status = -1;
								}
							}
						}
					}
				}
				etherrxdesc[index].flag = etherrxdesc[index].flag  | 0x8000;
			}
		}
		cf.fec.rdar = 0x00000000;
	}

	if((value & 0x04000000)!= 0){
		//printf("Transmit Buffer\n");
	}

	if((value & 0x08000000)!= 0){
		//printf("Transmit Frame\n");
	}

	if((value & 0x10000000)!= 0){
		printf("Graceful Stop\n");
	}

	if((value & 0x20000000)!= 0){
		printf("Babbling Transmit Error\n");
	}

	if((value & 0x40000000)!= 0){
		printf("Babbling Receive Error\n");
	}

	if((value & 0x80000000)!= 0){
		printf("Heartbeat Error\n");
	}

	cf.fec.eir = value;
}

unsigned short int ethernet_connection_status(void){
	ulong value, status;
	long currentcounter;

	status = 0x60060000;

	value = status | (physaddress << 23);
	currentcounter = counter;
	cf.fec.mdata = value;
	while(currentcounter == counter);
	value = cf.fec.mdata;
	return (unsigned short int) (value & 0xFFFF);
}

void ethernet_send_ping(void){
	short index;
	//unsigned long int value;

	for(index=0; index<6; index++) ethertxbuffer[0][index] = 0xFF;
	for(index=0; index<6; index++) ethertxbuffer[0][index + 6] = macaddress[index];
	ethertxbuffer[0][12] = 'W';
	ethertxbuffer[0][13] = 'L';

	//while(1){
	for(index=0; index<1024; index++){
		ethertxbuffer[0][index+14] = (unsigned char) (rand() & 0xFF);
	}
	ethertxdesc[0].length = index + 14;
	ethertxdesc[0].flag = ethertxdesc[0].flag | 0x8000;
	ethernet_echo_status = 0;
	cf.fec.tdar = 0x01000000;
	//while((ethertxdesc[0].flag & 0x8000)!=0);
	//}
}

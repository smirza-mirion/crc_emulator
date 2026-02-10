
/**********************************************************************
*   MCF5282 ColdFire Assembly Header File                             *
*                                                                     *
*   Developed by       : Motorola                                     *
*                        Imaging and Storage Systems Division         *
*                        Austin, TX                                   *
*                                                                     *
**********************************************************************/
//File: coldfire.h
#ifndef _SIM5282_H
#define _SIM5282_H

//#define SYSCLK 66000000
#define SYSCLK 64000000

typedef volatile unsigned char vubyte;
typedef volatile unsigned short vuword;
typedef volatile unsigned long vudword;


typedef struct {		/* system control module */
	vudword	ipsbar; 	/* 00->03 ips base address register*/
	vudword	rambar;		/* 04->07 processor-local memory base address register */
	vudword	flashbar;	/* 08->0b */
	vubyte	pack00[4];	/* 0c->0f */
	vubyte	crsr;		/* 10 core reset status register */
	vubyte	cwcr;		/* 11 core watchdog control register */
	vubyte	lpicr;		/* 12 */
	vubyte	cwsr;		/* 13 core watchdog service register */
	vudword	dmareqc;	/* 14->17 dma request control register */
	vubyte	pack01[4];	/* 18->1b */
	vudword	mpark;		/* 1c->1f system bus master arbitration programming model */
	vubyte	mpr;		/* 20 master privilege register */
	vubyte	pack02[3];	/* 21-23 */
	vubyte	pacr0;		/* 24 peripheral access control registers(0-8) */
	vubyte	pacr1;		/* 25 */
	vubyte	pacr2;		/* 26 */
	vubyte	pacr3;		/* 27 */
	vubyte	pacr4;		/* 28 */
	vubyte	pack03;		/* 29 */
	vubyte	pacr5;		/* 2a */
	vubyte	pacr6;		/* 2b */
	vubyte	pacr7;		/* 2c */
	vubyte	pack04;		/* 2d */
	vubyte	pacr8;		/* 2e */
	vubyte	pack05;		/* 2f */
	vubyte	gpacr0;		/* 30 grouped peripheral access control registers(0-1) */
	vubyte	gpacr1;		/* 31 */
	vubyte 	pack06[14];	/* 32->3f */
} scmstruct ;

typedef struct {		/* synchronous dram */
	vuword	dcr;		/* 40->41 dram control register */
	vubyte	pack00[6];	/* 42->47 */
	vudword	dacr0;		/* 48->4b dram address and control registers(0-1) */
	vudword	dmr0;		/* 4c->4f dram mask registers(0-1) */
	vudword	dacr1;		/* 50->53 */
	vudword	dmr1;		/* 54->57 */
} sdramcstruct;

/* -------------  will become array cs[8] 0080->00df ---------------- */
typedef struct {		/* chip-select */
	vuword	csar;		/* 16bit chip-select address register */
	vubyte	pack00[2];	/* 16bit reserved */
	vudword	csmr;		/* 32bit chip-select mask register */
	vubyte	pack01[2];	/* 16bit reserved */
	vuword	cscr;		/* 16bit chip-select control register */
} csstruct;


/* -------------  will become array dma[4] 0100->01d0 --------------- */
typedef struct {		/* dma controller modules */
	vudword	sar;		/* 32bit source address register */
	vudword	dar;		/* 32bit destination address register */
	vudword	dcr;		/* 32bit dma controller register */
	vubyte	pack00[3];	/* 24bit reserved */
	vudword	bcr;		/* 32bit byte count register */
	vubyte	dsr;		/* dma status register */
	vubyte	pack01[3];	/* 24bit reserved */
	vubyte 	pack02[40];
} dmastruct;

/* ------------- will become array uart[3] 0200->02bc --------------- */
typedef struct {		/* uart modules */
	vubyte	umr;		/* 8bit uart mode register */
	vubyte	pack00[3];	/* 24bit reserved */
	vubyte	usr;		/* 8bit read: status register */
						/*	write: clock-select register */
	vubyte	pack01[3];	/* 24bit reserved */
	vubyte	ucr;		/* 8bit read: do not access */
						/*	write: command register */
	vubyte	pack02[3];	/* 24bit reserved */
	vubyte	udb;		/* 8bit read: recieve buffer */
						/*	write: transmit buffer */
	vubyte	pack03[3];	/* 24bit reserved */
	vubyte	iacr;		/* 8bit read: input port change register */
						/*	write: auxiliary control register */
	vubyte	pack04[3];	/* 24bit reserved */
	vubyte	ir;			/* 8bit read: interupt status register */
						/*	write: interupt mask register */
	vubyte	pack05[3];	/* 24bit reserved */
	vubyte	dur;		/* 8bit read: do not access */
						/*	write: divider upper register */
	vubyte	pack06[3];	/* 24bit reserved */
	vubyte	dlr;		/* 8bit read: do not access */
						/*	write: divider lower register */
	vubyte	pack07[23];	/* 24bit reserved */
	vubyte	uip;		/* 8bit read: input port register */
						/*	write: do not access */
	vubyte	pack08[3];	/* 24bit reserved */
	vubyte	ops;		/* 8bit read: do not access */
						/*	write: output port bit set command register */
	vubyte	pack09[3];
	vubyte	opr;		/* 8bit read: do not access */
						/*	write: output port bit reset commmand register */
	vubyte	pack0a[3];
} uartstruct;

typedef struct {			/* i2c module */
	vubyte	adr;		/* 0300 address register */
	vubyte	pack00[3];	/* 0301->0303 */
	vubyte	fdr;		/* 0304 frequency divider register */
	vubyte	pack01[3];	/* 0305->0307 */
	vubyte	cr;		/* 0308 control register */
	vubyte	pack02[3];	/* 0309-030b */
	vubyte	sr;		/* 030c status register */
	vubyte	pack03[3];	/* 030d->030f */
	vubyte	dr;		/* 0310 data i/o register */
	vubyte	pack04[3];	/* 0311->0313 */
} i2cstruct;

typedef struct {		/* qspi module */
	vuword	qmr;		/* 0340->0341 mode register */
	vubyte	pack00[2];	/* 0342->0343 */
	vuword	qdlyr;		/* 0344->0345 delay register */
	vubyte	pack01[2];	/* 0346->0347 */
	vuword	qwr;		/* 0348->0349 wrap register */
	vubyte	pack02[2];	/* 034a->034b */
	vuword	qir;		/* 034c->034d interrupt register */
	vubyte	pack03[2];	/* 034e->034f */
	vuword	qar;		/* 0350->0351 address register */
	vubyte	pack04[2];	/* 0352->0353 */
	vuword	qdr;		/* 0354->0355 data register */
	vubyte	pack05[2];	/* 0356->0357 */
} qspistruct;

/* ------------- will become array timer[4] 0400->04cc --------------- */
typedef struct {		/* dma timers */
	vuword	tmr;		/* 0400->0401 mode register */
	vubyte	txmr;		/* 0402 extended mode register */
	vubyte	ter;		/* 0403 event register */
	vudword	trr;		/* 0404->0407 reference register */
	vudword tcr;		/* 0408->040b capture register */
	vudword tcn;		/* 040c->040f counter register */
	vubyte	pack00[48];	/* 0410->047f */
} timerstruct;

typedef struct {		/* interrupt controller module0 */
	vudword	iprh;		/* 0c00->0c03 pending register high */
	vudword	iprl;		/* 0c04->0c07 pending register low */
	vudword	imrh;		/* 0c08->0c0b mask register high */
	vudword	imrl;		/* 0c0c->0c0f mask register low */
	vudword	intfrch;	/* 0c10->0c13 force register high */
	vudword intfrcl;	/* 0c14->0c17 force register low */
	vubyte	irlr;		/* 0c18 request level register */
	vubyte  iacklpr;	/* 0c19 acknowledge level and priority register */
	vubyte	pack01[38];	/* 0c1a->0c3f */
	vubyte  icrn[64];
	vubyte	pack03[96];	/* 0c7f->0cdf */
	vubyte	swackr;		/* 0ce0 software ack register*/
	vubyte	pack04[3];	/* 0ce1->0ce3 */
	vubyte	l1ackr;		/* 0ce4 level ack registers(1-7) */
	vubyte	pack05[3];	/* 0ce5->0ce7 */
	vubyte	l2ackr;		/* 0ce8 */
	vubyte	pack06[3];	/* 0ce9->0ceb */
	vubyte	l3ackr;		/* 0cec */
	vubyte	pack07[3];	/* 0ced->0cef */
	vubyte	l4ackr;		/* 0cf0 */
	vubyte	pack08[3];	/* 0cf1->0cf3 */
	vubyte	l5ackr;		/* 0cf4 */
	vubyte	pack09[3];	/* 0cf5->0cf7 */
	vubyte	l6ackr;		/* 0cf8 */
	vubyte	pack10[3];	/* 0cf9->0cfb */
	vubyte	l7ackr;		/* 0cfc */
	vubyte	pack11[3];	/* 0cfd->0cff */
} intcstruct;


typedef struct { 		/* global interrupt acknowledge cycle module */

	vubyte gswackr;		/* 0fe0->0fe0 global software interrupt acknowledge register */
	vubyte pack00[3];	/* 0fe1->0fe3 */
	vubyte gl1iackr; 	/* 0fe4->0fe4 global level 1 interrupt acknowledge register */
	vubyte pack01[3];	/* 0fe5->0fe7 */
	vubyte gl2iackr; 	/* 0fe8->0fe8 global level 2 interrupt acknowledge register */
	vubyte pack02[3];	/* 0fe9->0feb */
	vubyte gl3iackr;	/* 0fec->0fec global level 3 interrupt acknowledge register */
	vubyte pack03[3];	/* 0fed->0fef */
	vubyte gl4iackr;	/* 0ff0->0ff0 global level 4 interrupt acknowledge register */
	vubyte pack04[3];	/* 0ff1->0ff3 */
	vubyte gl5iackr;	/* 0ff4->0ff4 global level 5 interrupt acknowledge register */
	vubyte pack05[3];	/* 0ff5->0ff7 */
	vubyte gl6iackr;	/* 0ff8->0ff8 global level 6 interrupt acknowledge register */
	vubyte pack06[3];     	/* 0ff9->0ffb */
	vubyte gl7iackr; 	/* 0ffc->0ffc global level 7 interrupt acknowledge register */
	vubyte pack07[3];	/* 0ffc->0fff */

     } giacstruct;



typedef struct {	   	/* fast ethernet controller */
	vudword	eir;		/* 1004->1007 interrupt event register */
	vudword	eimr;		/* 1008->100b interrupt mask register */
	vubyte	pack00[4];	/* 100c->100f */
	vudword	rdar;		/* 1010->1013 recieve descriptor active register */
	vudword	tdar;		/* 1014->1017 transmit descriptor active register */
	vubyte	pack01[12];	/* 1018->1023 */
	vudword	ecr;		/* 1024->1027 ethernet control register */
	vubyte	pack02[24];	/* 1028->103f */
	vudword	mdata;		/* 1040->1043 mii data register */
	vudword	mscr;		/* 1044->1047 mii speed control register */
	vubyte	pack03[28];	/* 1048->1063 */
	vudword	mibc;		/* 1064->1067 mib control/status register */
	vubyte	pack04[28];	/* 1068->1083 */
	vudword	rcr;		/* 1084->1087 recieve control register */
	vubyte	pack05[60];	/* 1088->10c3 */
	vudword	tcr;		/* 10c4->10c7 transmit control register */
	vubyte	pack06[28];	/* 10c8->10e3 */
	vudword	palr;		/* 10e4->10e7 physical address low register */
	vudword	paur;		/* 10e8->10eb physical address high+ type field */
	vudword	opd;		/* 10ec->10ef opcode + pause duration */
	vubyte	pack07[40];	/* 10f0->1117 */
	vudword	iaur;		/* 1118->111b upper 32bits of individual hash table */
	vudword	ialr;		/* 111c->111f lower 32bits of individual hash table */
	vudword	gaur;		/* 1120->1123 upper 32bits of group hash table */
	vudword	galr;		/* 1124->1127 lower 32bits of group hash table */
	vubyte	pack08[28];	/* 1128->1143 */
	vudword	tfwr;		/* 1144->1147 transmit fifo watermark */
	vubyte	pack09[4];	/* 1148->114b */
	vudword	frbr;		/* 114c->114f fifo receive bound register */
	vudword	frsr;		/* 1150->1153 fifo receive fifo start registers */
	vubyte	pack10[44];	/* 1154->117f */
	vudword	erdsr;		/* 1180->1183 pointer to receive descriptor ring */
	vudword	etdsr;		/* 1184->1187 pointer to transmit descriptor ring */
	vudword	emrbr;		/* 1188->118b maximum receive buffer size */
} fecstruct;

typedef struct {			/* fec, rmon transmit */
	vudword	drop;		/* 1200->1203 count of frames not counted correctly */
	vudword	packets;	/* 1204->1207 packet count */
	vudword bc_pkt;		/* 1208->120b broadcast packets */
	vudword mc_pkt;		/* 120c->120f multicast packets */
	vudword crc_align;	/* 1200->1213 packets with crc/align error */
	vudword	undersize;	/* 1214->1217 packets < 64 bytes, good crc */
	vudword oversize;	/* 1218->121b packets > max_fl bytes, good crc */
	vudword frag;		/* 121c->121f packets < 64 bytes, bad crc */
	vudword	jab;		/* 1220->1223 packets > max_fl bytes, bad crc */
	vudword col;		/* 1224->1227 collision count */
	vudword	p64;		/* 1228->122b 64 byte packets */
	vudword p65to127;	/* 122c->122f 65 to 127 byte packets */
	vudword	p128to255;	/* 1230->1233 128 to 255 byte packets */
	vudword	p256to511;	/* 1234->1237 256 to 511 byte packets */
	vudword p512to1023;	/* 1238->123b 512 to 1023 byte packets */
	vudword	p1024to2047;	/* 123c->123f 1024 to 2047 byte packets */
	vudword	p_gte2048;	/* 1230->1243 > 2048 byte packets */
	vudword octets;		/* 1244->1247 octets */
} fec_rmon_tstruct;

typedef struct {			/* fec, ieee transmit */
	vudword	drop;		/* 1248->124b count of frames not counted correctly */
	vudword	frame_ok;	/* 124c->124f frames transmitted ok */
	vudword scol;		/* 1250->1253 frames transmitted with single collision */
	vudword mcol;		/* 1254->1257 frames transmitted with multiple collisions*/
	vudword def;		/* 1258->125b frames transmitted after deferral delay */
	vudword lcol;		/* 125c->125f frames transmitted with late collision */
	vudword	excol;		/* 1260->1263 frames transmitted with excessive collisions */
	vudword macerr;		/* 1264->1267 frames transmitted with tx fifo underrun */
	vudword cserr;		/* 1268->126b frames transmitted with carrier sense error */
	vudword sqe;		/* 126c->126f frames transmitted with sqe error */
	vudword	fdxfc;		/* 1270->1273 flow control pause frames trasmitted */
	vudword octets_ok;	/* 1274->1277 octet count for frames transmitted w/o error */
} fec_ieee_tstruct;

typedef struct {			/* fec, rmon receive */
	vudword	packets;	/* 1284->1287 packet count */
	vudword bc_pkt;		/* 1288->128b broadcast packets */
	vudword mc_pkt;		/* 128c->128f multicast packets */
	vudword crc_align;	/* 1290->1293 packets with crc/align error */
	vudword	undersize;	/* 1294->1297 packets < 64 bytes, good crc */
	vudword oversize;	/* 1298->129b packets > max_fl bytes, good crc */
	vudword frag;		/* 129c->129f packets < 64 bytes, bad crc */
	vudword	jab;		/* 12a0->12a3 packets > max_fl bytes, bad crc */
	vudword	resvd_0;	/* 12a4->12a7 */
	vudword	p64;		/* 12a8->12ab 64 byte packets */
	vudword p65to127;	/* 12ac->12af 65 to 127 byte packets */
	vudword	p128top255;	/* 12b0->12b3 128 to 255 byte packets */
	vudword	p256to511;	/* 12b4->12b7 256 to 511 byte packets */
	vudword p512to1023;	/* 12b8->12bb 512 to 1023 byte packets */
	vudword	p1024to2047;	/* 12bc->12bf 1024 to 2047 byte packets */
	vudword	p_gte2048;	/* 12c0->12c3 > 2048 byte packets */
	vudword octets;		/* 12c4->12c7 octets */
} fec_rmon_rstruct;

typedef struct {			/* fec, ieee receive  */
	vudword	drop;		/* 12c8->12cb count of frames not counted correctly */
	vudword	frame_ok;	/* 12cc->12cf frames received ok */
	vudword crc;		/* 12d0->12d3 frames received with crc error */
	vudword	align;		/* 12d4->12d7 frames received with alignment error */
	vudword macerr;		/* 12d8->12db receive fifo overflow count */
	vudword	fdxfc;		/* 12dc->12df flow control pause frames received */
	vudword octets_ok;	/* 12e0->12e3 octet count for frames received w/o error */
} fec_ieee_rstruct;

typedef struct {		/* general purpose i/o */
	vubyte	porta;		/* 100000 */
	vubyte	portb;		/* 100001 */
	vubyte	portc;		/* 100002 */
	vubyte	portd;		/* 100003 */
	vubyte	porte;		/* 100004 */
	vubyte	portf;		/* 100005 */
	vubyte	portg;		/* 100006 */
	vubyte	porth;		/* 100007 */
	vubyte	portj;	        /* 100008 */
	vubyte	portdd;		/* 100009 */
	vubyte	porteh;		/* 10000a */
	vubyte	portel;		/* 10000b */
	vubyte	portas;		/* 10000c */
	vubyte	portqs;		/* 10000d */
	vubyte	portsd;		/* 10000e */
	vubyte	porttc;		/* 10000f */
	vubyte	porttd;		/* 100010 */
	vubyte	portua;		/* 100011 */
	vubyte	pack00[2];	/* 100012->100013 */
	vubyte	ddra;		/* 100014 */
	vubyte	ddrb;		/* 100015 */
	vubyte	ddrc;		/* 100016 */
	vubyte	ddrd;		/* 100017 */
	vubyte	ddre;		/* 100018 */
	vubyte	ddrf;		/* 100019 */
	vubyte	ddrg;		/* 10001a */
	vubyte	ddrh;		/* 10001b */
	vubyte	ddrj;		/* 10001c */
	vubyte	ddrdd;	        /* 10001d */
	vubyte	ddreh;		/* 10001e */
	vubyte	ddrel;		/* 10001f */
	vubyte	ddras;		/* 100020 */
	vubyte	ddrqs;		/* 100021 */
	vubyte	ddrsd;		/* 100022 */
	vubyte	ddrtc;		/* 100023 */
	vubyte	ddrtd;		/* 100024 */
	vubyte	ddrua;		/* 100025 */
	vubyte	pack01[2];	/* 100026->100027 */
	vubyte	portap;		/* 100028 */
	vubyte	portbp;		/* 100029 */
	vubyte	portcp;		/* 10002a */
	vubyte	portdp;		/* 10002b */
	vubyte	portep;		/* 10002c */
	vubyte	portfp;		/* 10002d */
	vubyte	portgp;		/* 10002e */
	vubyte	porthp;		/* 10002f */
	vubyte	portjp;		/* 100030 */
	vubyte	portddp;	/* 100031 */
	vubyte	portehp;	/* 100032 */
	vubyte	portelp;	/* 100033 */
	vubyte	portasp;	/* 100034 */
	vubyte	portqsp;	/* 100035 */
	vubyte	portsdp;	/* 100036 */
	vubyte	porttcp;	/* 100037 */
	vubyte	porttdp;	/* 100038 */
	vubyte	portuap;	/* 100039 */
	vubyte	pack02[2];	/* 10003a->10003b */
	vubyte	clra;		/* 10003c */
	vubyte	clrb;		/* 10003d */
	vubyte	clrc;		/* 10003e */
	vubyte	clrd;		/* 10003f */
	vubyte	clre;		/* 100040 */
	vubyte	clrf;		/* 100041 */
	vubyte	clrg;		/* 100042 */
	vubyte	clrh;		/* 100043 */
	vubyte	clrj;		/* 100044 */
	vubyte	clrdd;		/* 100045 */
	vubyte	clreh;		/* 100046 */
	vubyte	clrel;		/* 100047 */
	vubyte	clras;		/* 100048 */
	vubyte	clrqs;		/* 100049 */
	vubyte	clrsd;		/* 10004a */
	vubyte	clrtc;		/* 10004b */
	vubyte	clrtd;		/* 10004c */
	vubyte	clrua;		/* 10004d */
	vubyte	pack03[2];	/* 10004e->10004f */
	vubyte	pbcdpar;	/* 100050 */
	vubyte	pfpar;		/* 100051 */
	vuword	pepar;		/* 100052-100053 */
	vubyte	pjpar;		/* 100054 */
	vubyte	psdpar;		/* 100055 */
	vuword	paspar;		/* 100056->100057 */
	vubyte	pehlpar;	/* 100058 */
	vubyte	pqspar;		/* 100059 */
	vubyte	ptcpar;		/* 10005a */
	vubyte	ptdpar;		/* 10005b */
	vubyte	puapar;		/* 10005c */
	vubyte	pack04;		/* 10005d */
} gpiostruct;

typedef struct { 		/* reset controller module */

	vubyte rcr;		/* 110000->110000 reset control register */
	vubyte rsr;		/* 110001->110001 reset status register */


    } resetstruct;

typedef struct {  		/* chip configuration module */

    	vuword ccr;		/* 110004->110005 chip configuration register */
	vubyte pack00;  	/* 110006 */
	vubyte lpcr;		/* 110007 low power control register */
	vuword rcon;		/* 110008->110009 reset configuration register*/
	vuword cir;		/* 11000a->11000b chip identification register*/


    } ccmstruct;


typedef struct {   		/* clock module */


	vuword syncr; 		/* 120000->120001 synthesizer control register */
	vubyte synsr;      	/* 120002->120002 synthesizer status register */


    } clockstruct;



typedef struct {		/* edge port module */

	vuword eppar;		/* 130000->130001 eport pin assignment register */
	vubyte epddr;		/* 130002->130002 eport data direction register */
	vubyte epier;		/* 130003->130003 eport interrupt enable register */
	vubyte epdr;		/* 130004->130004 eport data register */
	vubyte eppdr;		/* 130005->130005 eport pin data register */
	vubyte epfr;		/* 130006->130006 eport flag register */


     } eportstruct;



typedef struct {		/* watchdog timer module */

	vuword wcr;		/* 140000->140001 watchdog control register */
	vuword wmr;		/* 140002->140003 watchdog modulus register */
	vuword wcntr;		/* 140004->140005 watchdog count register */
	vuword wsr;		/* 140006->140007 watchdog service register */


     } wtmstruct;

typedef struct {		/* programmable interrupt timer modules */
	vuword	pcsr;		/* 16bit control and status register */
	vuword	pmr;		/* 16bit modulus register */
	vuword	pcntr;		/* 16bit count register */
	vubyte	pack00[65530];	/* 0xfffc */
} pitstruct;

typedef struct {		/* queued analog-to-digital converter module */

	vuword qadcmcr;		/* 190000->190001 qadc module configuration register */
	vubyte pack00[4];	/* 190002->190005 */
	vubyte portqa;		/* 190006->190006 (qa) port data register */
	vubyte portqb;		/* 190007->190007 (qb) port data register */
	vubyte ddrqa;		/* 190008->190008 port qa data direction register */
	vubyte ddrqb;		/* 190009->190009 port qb data direction register */
	vuword qacr0;		/* 19000a->19000b qadc control register 0 */
	vuword qacr1;		/* 19000c->19000d qadc control register 1 */
	vuword qacr2;		/* 19000e->19000f qadc control register 2 */
    vuword qasr0;		/* 190010->190011 qadc status register 0 */
	vuword qasr1; 		/* 190012->190013 qadc status register 1 */
	vubyte pack01[491];	/* 190014->1901ff */
	vuword ccw[64];		/* 190200->190201 conversion command word table */
	vuword rjurr[64];		/* 190280->190281 right-justified unsigned result register */
	vuword ljsrr[64];		/* 190300->190301 left-justified signed result register */
	vuword ljurr[64];		/* 190380->190381 left-justified unsigned result register */


     } qadcstruct;


typedef struct {		/* general purpose timer modules */
	vubyte	ios;		/* 8bit ic/oc select register */
	vubyte	cforc;		/* 8bit compare force register */
	vubyte	oc3m;		/* 8bit output compare 3 mask register */
	vubyte	oc3d;		/* 8bit output compare 3 data register */
	vuword	cnt;		/* 16bit counter register */
	vubyte	scr1;		/* 8bit system control register 1 */
	vubyte	pack00;		/* 8bit reserved */
	vubyte	tov;		/* 8bit toggle on overflow register */
	vubyte	ctl1;		/* 8bit control register 1 */
	vubyte	pack01;		/* 8bit reserved */
	vubyte	ctl2;		/* 8bit control register 2 */
	vubyte	tie;		/* 8bit interupt enable register */
	vubyte	scr2;		/* 8bit system control register 2 */
	vubyte	flg1;		/* 8bit flag register 1 */
	vubyte	flg2;		/* 8bit flag register 2 */
	vuword	c0;		/* 16bit channel 0  */
	vuword	c1;		/* 16bit channel 1 */
	vuword	c2;		/* 16bit channel 2 */
	vuword	c3;		/* 16bit channel 3 */
	vubyte	pactl;		/* 8bit pulse accumulator control register */
	vubyte	paflg;		/* 8bit pulse accumulator flag register */
	vuword	pacnt;		/* 16bit pulse accumulator counter register */
	vubyte	pack02;		/* 8bit reserved */
	vubyte	port;		/* 8bit port data register */
	vubyte	ddr;		/* 8bit data direction register */
	vubyte	pack03[65504];	/* 0xffe2 */
} gptstruct;


typedef struct {
 vubyte bTimeStamp;
 vubyte bStatus;
 vuword id_hi;
 vuword id_low;
 vubyte data[8];
 vuword reserved;
}can_mb_struct;

typedef struct {		/* flexcan module */

   	vuword canmcr; 		/* 1c0000->1c0001 can module configuration register */
	vubyte pack00[4];	/* 1c0002->1c0005 */
	vubyte canctrl0;	/* 1c0006->1c0006 flexcan control register 0 */
	vubyte canctrl1; 	/* 1c0007->1c0007 flexcan control register 1 */
	vubyte presdiv;		/* 1c0008->1c0008 prescaler divide register */
	vubyte canctrl2;	/* 1c0009->1c0009 flexcan control register 2 */
	vuword timer;		/* 1c000a->1c000b free running timer */
	vubyte pack01[4];	/* 1c000c->1c000f */
	vudword rxgmask; 	/* 1c0010->1c0013 receive global mask register */
	vudword rx14mask;	/* 1c0014->1c0017 receive mask register */
	vudword rx15mask;	/* 1c0018->1c001b receive mask register */
	vubyte pack02[4];	/* 1c001c->1c001f */
	vuword esr;		/* 1c0020->1c0021 flexcan error and status register */
	vuword imask;		/* 1c0022->1c0023 interrupt mask register */
	vuword iflag; 		/* 1c0024->1c0025 interrupt flag register */
	vubyte rxerrcnt;	/* 1c0026->1c0026 flexcan receive error counter */
	vubyte txerrcnt;	/* 1c0027->1c0027 flexcan transmit error counter */
	vubyte pack03[88];	/* 1c0034->1c007f */
	can_mb_struct mbs[16]; /*1c_0080 -> 1c_017F */

    } flexcanstruct;

typedef struct { 		/* coldfire flash module  */

      vuword cfmmcr;		/* 1d0000->1d0001 cfm configuration register */
      vubyte cfmclkd; 		/* 1d0002->1d0002 cfm clock divider register */
      vubyte pack00[5];         /* 1d0003->1d0007 */
      vudword cfmsec;		/* 1d0008->1d000b cfm security register */
      vubyte pack01[4];		/* 1d000c->1d000f */
      vudword cfmprot;		/* 1d0010->1d0013 cfm protection register */
      vudword cfmsacc;		/* 1d0014->1d0017 cfm supervisor access register */
      vudword cfmdacc;		/* 1d0018->1d001b cfm data access register */
      vubyte pack02[4];		/* 1d001c->1d001f */
      vubyte cfmusat;		/* 1d0020->1d0020 cfm user status register */
      vubyte pack03[3];		/* 1d0021->1d0023 */
      vubyte cfmcmd;		/* 1d0024->1d0024 cfm command register */
      vubyte pack04[28];	/* 1d0025->1d0041 */
      vuword  cfmdisu;		/* 1d0042->1d0044 */



    } cfmstruct;


/* ---------------------- 5282 main struct ------------------------- */
typedef struct {

	scmstruct 	scm;		/* 0x000000->0x00003f */

	sdramcstruct 	sdram;		/* 0x000040->0x000057 */

	vubyte		pack00[40];	/* 0x000058->0x00007f */

	csstruct	cs[8];		/* 0x000080->0x0000df */

	vubyte		pack01[32];	/* 0x0000e0->0x0000ff */

	dmastruct	dma[4];		/* 0x000100->0x0001ff */

	uartstruct	uarts[3];	/* 0x000200->0x0002bc */

	vubyte		pack03[64];	/* 0x0002bd->0x0002ff */

	i2cstruct	i2c;		/* 0x000300->0x000313 */

	vubyte		pack04[44];	/* 0x000314->0x00033f */

	qspistruct	qspi;		/* 0x000340->0x000357 */

	vubyte		pack05[168];	/* 0x000358->0x0003ff */

	timerstruct	timer[4];	/* 0x000400->0x00050f */

	vubyte		pack06[1792];	/* 0x000510->0x000bff */

	intcstruct	intc[2];

	vubyte		pack07[480];	/* 0x000e00->0x000fdf */

	giacstruct	giac;		/* 0x000fe0->0x000fff */

	vubyte		pack08[4];	/* 0x001000->0x001003 */

	fecstruct	fec;		/* 0x001004->0x00118b */

	vubyte		pack09[116];	/* 0x00118c->0x0011ff */

	fec_rmon_tstruct fec_rmon_t;	/* 0x001200->0x001247 */

	fec_ieee_tstruct fec_ieee_t;	/* 0x001248->0x001277 */

	vubyte		pack10[12];	/* 0x001278->0x001283 */

	fec_rmon_rstruct fec_rmon_r;	/* 0x001284->0x0012c7 */

	fec_ieee_rstruct fec_ieee_r;	/* 0x0012c8->0x0012e3 */

	vubyte		pack11[1043740]; /* 0x0012e4->0x0fffff */

	gpiostruct	gpio;		/* 0x100000->0x10005d */

	vubyte		pack12[65442];	/* 0x10005e->0x10ffff */

	resetstruct	reset;		/* 1x110000->1x110001 */

	vubyte		pack13[2];	/* 1x110002->1x110003 */

	ccmstruct	ccm;		/* 1x110004->1x11000b */

	vubyte		pack14[65524];	/* 1x11000c->1x11ffff */

	clockstruct	clock;		/* 1x120000->1x120001 */

	vubyte		pack15[65532];	/* 1x120002->1x12ffff */

	eportstruct	eport;		/* 1x130000->1x130006 */

	vubyte		pack16[65527];	/* 1x130007->1x13ffff */

	wtmstruct	wtm;		/* 1x140000->1x140007 */

	vubyte		pack17[65528];	/* 1x140008->1x14ffff */

	pitstruct	pit[4];		/* 1x150000->1x18ffff */

	qadcstruct	qadc;		/* 1x190000->1x1903FF */

	vubyte		pack18[64512];	/* 1x190400->1x19ffff */

	gptstruct	gpt[2];		/* 1x1a0000->1x19bfff */

	flexcanstruct	flexcan;	/* 1x1c0000->1x1c017F */

	vubyte		pack19[65152];	/* 1x1c0200->1x1cffff */

	cfmstruct     	cfm;		/* 1x1d0000->1x1d0044 */


 } mcf5282;


 extern vudword  flash_mirror[0x80000];  /* 1x400000->1x4080000 */

 extern volatile mcf5282 cf;

 typedef struct
{
  unsigned long table[256];
} vectors;


 extern vectors vector_base;

 typedef struct
{
unsigned short flags;
unsigned short length;
unsigned long  address;
}EtherBD;


#define RXBD_Flag_Empty     (0x8000)
#define RXBD_Flag_SW1b      (0x4000)
#define RXBD_Flag_Wrap      (0x2000)
#define RXBD_Flag_SW2b      (0x1000)
#define RXBD_Flag_Last      (0x0800)
#define RXBD_Flag_Miss      (0x0100)
#define RXBD_Flag_BroadCast (0x0080)
#define RXBD_Flag_MultiCast (0x0040)
#define RXBD_Flag_LengthErr (0x0020)
#define RXBD_Flag_Align_Err (0x0010)
#define RXBD_Flag_ShortErr  (0x0008)
#define RXBD_Flag_CRC_Err   (0x0004)
#define RXBD_Flag_OverErr   (0x0002)
#define RXBD_Flag_TruncErr  (0x0001)
#define RXBD_Error_Mask     (RXBD_Flag_LengthErr | RXBD_Flag_Align_Err | RXBD_Flag_ShortErr  | RXBD_Flag_CRC_Err | RXBD_Flag_OverErr | RXBD_Flag_TruncErr  )




#define TXBD_Flag_Ready     (0x8000)
#define TXBD_Flag_SW1b      (0x4000)
#define TXBD_Flag_Wrap      (0x2000)
#define TXBD_Flag_SW2b      (0x1000)
#define TXBD_Flag_Last      (0x0800)
#define TXBD_Flag_TxCRC     (0x0400)
#define TXBD_Flag_Defered   (0x0200)
#define TXBD_Flag_HB_Error  (0x0100)
#define TXBD_Flag_LC_Error  (0x0080)
#define TXBD_Flag_RT_Error  (0x0040)
#define TXBD_Flag_RC_B3     (0x0020)
#define TXBD_Flag_RC_B2     (0x0010)
#define TXBD_Flag_RC_B1     (0x0008)
#define TXBD_Flag_RC_B0     (0x0004)
#define TXBD_Flag_UN_Error  (0x0002)
#define TXBD_Flag_CSL_Error (0x0001)

#define TXBD_Error_Mask ( TXBD_Flag_UN_Error |TXBD_Flag_RT_Error |TXBD_Flag_LC_Error |TXBD_Flag_HB_Error)
#define TXBD_Flag_NormalSend (TXBD_Flag_TxCRC | TXBD_Flag_Ready | TXBD_Flag_Last)

#define FEC_ISR_MASK_HBERR  (0x80000000)
#define FEC_ISR_MASK_BABR   (0x40000000)
#define FEC_ISR_MASK_BABT   (0x20000000)
#define FEC_ISR_MASK_GRA    (0x10000000)
#define FEC_ISR_MASK_TXF    (0x08000000)
#define FEC_ISR_MASK_TXB    (0x04000000)
#define FEC_ISR_MASK_RXF    (0x02000000)
#define FEC_ISR_MASK_RXB    (0x01000000)
#define FEC_ISR_MASK_MII    (0x00800000)
#define FEC_ISR_MASK_EBERR  (0x00400000)
#define FEC_ISR_MASK_LC     (0x00200000)
#define FEC_ISR_MASK_RL     (0x00100000)
#define FEC_ISR_MASK_UN     (0x00080000)







#endif

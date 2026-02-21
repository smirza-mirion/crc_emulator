/*********************************************************************
  MODULE:	Cyclic Redundancy Check

  FILE:		CyclicRedundancy.c

  DATE:		04/21/04

  ANALYSIS: Routines for Cyclic Redundancy Check
            From Dr. Dobbs Journal, April 1997
  *************************************************************************/

#include "crc.h"
static ushort Crc16Table[256];
void generate_crc_table(void)
{
	short i,j;
	ushort crc;

	for(i = 0; i < 256; i++)
	{
		crc = (i << 8);
		for(j = 0; j < 8; j++)
		{
			crc = (crc << 1) ^ ((crc & 0x8000) ? 0x01021:0);
		}
		Crc16Table[i] = crc & 0xffff;
	}
}

ushort Crc16(uchar ch0,ushort crc)
{
	ushort new_crc;
	ushort ch = (ushort)ch0;


	new_crc = Crc16Table[((crc >> 8) & 255)] ^ (crc << 8) ^ ch;

	return(new_crc & 0xffff);
}




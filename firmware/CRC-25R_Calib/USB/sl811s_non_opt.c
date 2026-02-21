//#include "crc.h"
//#include "cs.h"
//#include "pit.h"
//#include "coldfire.h"
//#include <string.h>
//#include "sl811s.h"
//#include "descp.h"
//#include "keyboard.h"
//#include "i2c.h"

#define INPACKETBUFFERLEN	129
#define OUTPACKETBUFFERLEN	139
#define INCDCBUFFERLEN	257

extern char InPacketCircularBuffer[INPACKETBUFFERLEN];
extern unsigned short InPacketCircularHead;
extern unsigned short InPacketCircularTail;
extern char OutPacketCircularBuffer[OUTPACKETBUFFERLEN];
extern unsigned short OutPacketCircularHead;
extern unsigned short OutPacketCircularTail;
extern char InCDCCircularBuffer[INCDCBUFFERLEN];
extern unsigned short InCDCCircularHead;
extern unsigned short InCDCCircularTail;


unsigned short InBufferPush(char *Input, short Length){
	short count, nextPosition, tempHead, tempTail;

	count = 0;
	tempHead = InPacketCircularHead;
	tempTail = InPacketCircularTail;
	do{
		nextPosition = tempTail + 1;
		if(nextPosition >= INPACKETBUFFERLEN) nextPosition = 0;
		if(nextPosition != tempHead){
			InPacketCircularBuffer[tempTail] = *Input;
			tempTail = nextPosition;
			Input++;
			count++;
		}
	}while((nextPosition != tempHead) && (count < Length));

	InPacketCircularTail = tempTail;
	if(count == Length) return 1;
	else return 0;
}

unsigned short InBufferPop(char *Output, short Length){
	short count, tempHead, tempTail;

	count = 0;
	tempHead = InPacketCircularHead;
	tempTail = InPacketCircularTail;
	while((tempHead != tempTail) && (count < Length)){
		*Output = InPacketCircularBuffer[tempHead];
		tempHead++;
		if(tempHead >= INPACKETBUFFERLEN) tempHead = 0;
		Output++;
		count++;
	}

	InPacketCircularHead = tempHead;
	if(count == Length) return 1;
	else return 0;
}

unsigned short InBufferPop1(char *Output){
	short tempHead;
	if(InPacketCircularHead != InPacketCircularTail){
		tempHead = InPacketCircularHead;
		*Output = InPacketCircularBuffer[tempHead];
		tempHead++;
		if(tempHead >= INPACKETBUFFERLEN) tempHead = 0;
		InPacketCircularHead = tempHead;
		return 1;
	}else{
		return 0;
	}
}

unsigned short OutBufferPush(char *Input, short Length){
	short count, nextPosition, tempHead, tempTail;

	count = 0;
	tempHead = OutPacketCircularHead;
	tempTail = OutPacketCircularTail;
	do{
		nextPosition = tempTail + 1;
		if(nextPosition >= OUTPACKETBUFFERLEN) nextPosition = 0;
		if(nextPosition != tempHead){
			OutPacketCircularBuffer[tempTail] = *Input;
			tempTail = nextPosition;
			Input++;
			count++;
		}
	}while((nextPosition != tempHead) && (count < Length));

	OutPacketCircularTail = tempTail;
	if(count == Length) return 1;
	else return 0;
}

unsigned short OutBufferPop(char *Output, short Length){
	short count, tempHead, tempTail;

	count = 0;
	tempHead = OutPacketCircularHead;
	tempTail = OutPacketCircularTail;
	while((tempHead != tempTail) && (count < Length)){
		*Output = OutPacketCircularBuffer[tempHead];
		tempHead++;
		if(tempHead >= OUTPACKETBUFFERLEN) tempHead = 0;
		Output++;
		count++;
	}

	OutPacketCircularHead = tempHead;
	if(count == Length) return 1;
	else return 0;
}

unsigned short OutBufferPop1(char *Output){
	short tempHead;
	if(OutPacketCircularHead != OutPacketCircularTail){
		tempHead = OutPacketCircularHead;
		*Output = OutPacketCircularBuffer[tempHead];
		tempHead++;
		if(tempHead >= OUTPACKETBUFFERLEN) tempHead = 0;
		OutPacketCircularHead = tempHead;
		return 1;
	}else{
		return 0;
	}
}

unsigned short InCDCPush(char *Input, short Length){
	short count, nextPosition, tempHead, tempTail;

	count = 0;
	tempHead = InCDCCircularHead;
	tempTail = InCDCCircularTail;
	do{
		nextPosition = tempTail + 1;
		if(nextPosition >= INCDCBUFFERLEN) nextPosition = 0;
		if(nextPosition != tempHead){
			InCDCCircularBuffer[tempTail] = *Input;
			tempTail = nextPosition;
			Input++;
			count++;
		}
	}while((nextPosition != tempHead) && (count < Length));

	InCDCCircularTail = tempTail;
	if(count == Length) return 1;
	else return 0;
}

unsigned short InCDCPop(char *Output, short Length){
	short count, tempHead, tempTail;

	count = 0;
	tempHead = InCDCCircularHead;
	tempTail = InCDCCircularTail;
	while((tempHead != tempTail) && (count < Length)){
		*Output = InCDCCircularBuffer[tempHead];
		tempHead++;
		if(tempHead >= INCDCBUFFERLEN) tempHead = 0;
		Output++;
		count++;
	}

	InCDCCircularHead = tempHead;
	if(count == Length) return 1;
	else return 0;
}

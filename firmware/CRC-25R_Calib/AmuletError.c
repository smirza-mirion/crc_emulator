#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "Amulet.h"

extern char AmuletErrorMsgMenu_title[52];
extern char AmuletErrorMsgMenu_message[100];
//extern short AmuletErrorMsgMenu_x;
//extern short AmuletErrorMsgMenu_y;
extern bool AmuletErrorMsgMenu_showOK;

unsigned char CurrentPageStack(void);
void PushPageStack(unsigned char ucPage);

static uint currentHTML = 0;
static char oldMessages[20][101];

static void clearMessages(void){
	short index;

	currentHTML = 0;
	for(index = 0; index<20; index++) oldMessages[index][0] = 0;
}

void AmuletError_setNewPage(uint value){
	if(currentHTML!=0){
		if((value != AmuletHTMLIndex[ERRORMSG_HTM]) && (value != AmuletHTMLIndex[WARNINGMSG_HTM]) && (value != AmuletHTMLIndex[NOTIFICATIONMSG_HTM])){
			if(currentHTML != value) currentHTML = 0;
		}
	}
}

bool AmuletError_display(char *title, char *errorstring){
	bool returnvalue;
	short index;
	bool found;

	found = FALSE;
	if(CurrentPageStack() != currentHTML){
		clearMessages();
	}else{
		for(index=0; index<20; index++){
			if(strcmp(errorstring, &(oldMessages[index][0]))==0){
				found = TRUE;
				break;
			}
		}
	}

	if(!found){
		currentHTML = CurrentPageStack();
		index = 0;
		while((index<20) && (oldMessages[index][0]!=0)) index++;

		if(index<20){
			strcpy(&(oldMessages[index][0]), errorstring);
		}

		//AmuletErrorMsgMenu_x = x;
		//AmuletErrorMsgMenu_y = y;
		strcpy(AmuletErrorMsgMenu_title, title);
		strcpy(AmuletErrorMsgMenu_message, errorstring);
		AmuletErrorMsgMenu_showOK = TRUE;
		SetAmuletHTML(AmuletHTMLIndex[ERRORMSG_HTM]);
		PushPageStack(AmuletHTMLIndex[ERRORMSG_HTM]);

		returnvalue = TRUE;
	}else{
		returnvalue = FALSE;
	}

	return returnvalue;
}

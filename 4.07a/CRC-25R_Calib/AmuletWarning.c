#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "Amulet.h"

extern char AmuletWarningMsgMenu_title[52];
extern char AmuletWarningMsgMenu_message[100];
extern bool AmuletWarningMsgMenu_showOK;

unsigned char CurrentPageStack(void);
void PushPageStack(unsigned char ucPage);

static uint currentHTML = 0;
static char oldMessages[20][101];

static void clearMessages(void){
	short index;

	currentHTML = 0;
	for(index = 0; index<20; index++) oldMessages[index][0] = 0;
}

void AmuletWarning_setNewPage(uint value){
	if(currentHTML!=0){
		if((value != AmuletHTMLIndex[ERRORMSG_HTM]) && (value != AmuletHTMLIndex[WARNINGMSG_HTM]) && (value != AmuletHTMLIndex[NOTIFICATIONMSG_HTM])){
			if(currentHTML != value) currentHTML = 0;
		}
	}
}

bool AmuletWarning_display(char *title, char *warningstring){
	bool returnvalue;
	short index;
	bool found;

	found = FALSE;
	if(CurrentPageStack() != currentHTML){
		clearMessages();
	}else{
		for(index=0; index<20; index++){
			if(strcmp(warningstring, &(oldMessages[index][0]))==0){
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
			strcpy(&(oldMessages[index][0]), warningstring);
		}

		strcpy(AmuletWarningMsgMenu_title, title);
		strcpy(AmuletWarningMsgMenu_message, warningstring);
		AmuletWarningMsgMenu_showOK = TRUE;
		SetAmuletHTML(AmuletHTMLIndex[WARNINGMSG_HTM]);
		PushPageStack(AmuletHTMLIndex[WARNINGMSG_HTM]);

		returnvalue = TRUE;
	}else{
		returnvalue = FALSE;
	}

	return returnvalue;
}

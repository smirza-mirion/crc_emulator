#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "Amulet.h"

extern char AmuletNotificationMsgMenu_title[52];
extern char AmuletNotificationMsgMenu_message[100];
extern bool AmuletNotificationMsgMenu_showOK;

unsigned char CurrentPageStack(void);
void PushPageStack(unsigned char ucPage);

static uint currentHTML = 0;
static char oldMessages[20][101];

static void clearMessages(void){
	short index;

	currentHTML = 0;
	for(index = 0; index<20; index++) oldMessages[index][0] = 0;
}

void AmuletNotification_setNewPage(uint value){
	if(currentHTML!=0){
		if((value != AmuletHTMLIndex[ERRORMSG_HTM]) && (value != AmuletHTMLIndex[WARNINGMSG_HTM]) && (value != AmuletHTMLIndex[NOTIFICATIONMSG_HTM])){
			if(currentHTML != value) currentHTML = 0;
		}
	}
}

bool AmuletNotification_display(char *title, char *notificationstring){
	bool returnvalue;
	short index;
	bool found;

	found = FALSE;
	if(CurrentPageStack() != currentHTML){
		clearMessages();
	}else{
		for(index=0; index<20; index++){
			if(strcmp(notificationstring, &(oldMessages[index][0]))==0){
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
			strcpy(&(oldMessages[index][0]), notificationstring);
		}

		strcpy(AmuletNotificationMsgMenu_title, title);
		strcpy(AmuletNotificationMsgMenu_message, notificationstring);
		AmuletNotificationMsgMenu_showOK = TRUE;
		SetAmuletHTML(AmuletHTMLIndex[NOTIFICATIONMSG_HTM]);
		PushPageStack(AmuletHTMLIndex[NOTIFICATIONMSG_HTM]);

		returnvalue = TRUE;
	}else{
		returnvalue = FALSE;
	}

	return returnvalue;
}

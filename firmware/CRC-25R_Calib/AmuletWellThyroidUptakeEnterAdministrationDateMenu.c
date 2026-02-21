#define PHASE_WELLTHYROIDUPTAKEENTERADMIN_PRE_INIT	0
#define PHASE_WELLTHYROIDUPTAKEENTERADMIN_WAIT		1
#define PHASE_WELLTHYROIDUPTAKEENTERADMIN_SAVE		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "sqlite3.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;

long long int AmuletWellThyroidUptakeEnterAdministrationDate_ProbeTUTestID;
time_t AmuletWellThyroidUptakeEnterAdministrationDate;
static PROBETHYROIDUPTAKETEST test;
static PROBETHYROIDUPTAKEPROTOCOL protocol;
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
unsigned char PopPageStack(void);
void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);

void AmuletWellThyroidUptakeEnterAdministrationDate_menu(void){
	char message[51];

	switch(m_iPhase){
		case PHASE_WELLTHYROIDUPTAKEENTERADMIN_PRE_INIT:
			if(m_ucClear == 92){
				AmuletWellThyroidUptakeEnterAdministrationDate = (time_t) 0;
				m_ucClear = 0;
			}
			test.ProbeTUTestID = AmuletWellThyroidUptakeEnterAdministrationDate_ProbeTUTestID;
			DB_RetrieveProbeThyroidUptakeTest(&test);
			if(test.ProbeTUTestID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Test", TRUE);
				return;
			}

			SetAmuletString(100, test.PatientID);
			strcpy(message, test.LastName);
			strcat(message, ", ");
			strcat(message, test.FirstName);
			SetAmuletString(101, message);

			if(AmuletWellThyroidUptakeEnterAdministrationDate == (time_t) 0){
				SetAmuletString(103, "");
			}else{
				GetExtendedTimeInfo(&AmuletWellThyroidUptakeEnterAdministrationDate, message);
				SetAmuletString(103, message);
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLTHYROIDUPTAKEENTERADMIN_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEENTERADMIN_WAIT:
			break;

		case PHASE_WELLTHYROIDUPTAKEENTERADMIN_SAVE:
			test.ProbeTUTestID = AmuletWellThyroidUptakeEnterAdministrationDate_ProbeTUTestID;
			DB_RetrieveProbeThyroidUptakeTest(&test);
			if(test.ProbeTUTestID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Test", TRUE);
				return;
			}
			protocol.ProbeTUProtocolID = test.ProbeTUProtocolID;
			DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
			if(protocol.ProbeTUProtocolID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
				return;
			}
			if(AmuletWellThyroidUptakeEnterAdministrationDate == (time_t) 0){
				// Display Error Message
				Amulet_DisplayError("Thyroid Uptake", "Missing Administration Date", TRUE);
				return;
			}else{
				if(test.DoseAdministeredOn == (time_t) 0){
					test.DoseAdministeredOn = AmuletWellThyroidUptakeEnterAdministrationDate;
					// Update Phase
					if(protocol.ResidualMeasurement) test.ProbeTUTestPhase = 4; // Measure Residual
					else test.ProbeTUTestPhase = 5; // Measure Patient
					DB_WriteProbeThyroidUptakeTest(&test, TRUE);
					PopPageStack();
					SetAmuletBackHTML();
					return;
				}else{
					// Display Error Message
					Amulet_DisplayError("Thyroid Uptake", "Administration Date Exists", TRUE);
					return;
				}
			}
	}
}

#define PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_PRE_INIT		0
#define PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_WAIT			1
#define PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_DEFAULTROI	2
#define PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_SAVE			3
#define PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_DELETE		4

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "nuc.h"
#include "wipes.h"
#include "database.h"
#include "amulet.h"

extern int m_iPhase;
extern unsigned char m_ucClear;

PROBETHYROIDUPTAKEPROTOCOL AmuletAddEditThyroidUptakeProtocol_protocol;
static PROBETHYROIDUPTAKEPROTOCOL protocol;
void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void trim(char *acByte);

void AmuletAddEditThyroidUptakeProtocol_menu(void){
	float tempvalue;
	char message[26];

	switch(m_iPhase){
		case PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_PRE_INIT:
			if(m_ucClear == 88){
				if(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID == -1){
					AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID = -1;
					AmuletAddEditThyroidUptakeProtocol_protocol.ProtocolName[0] = 0;
					AmuletAddEditThyroidUptakeProtocol_protocol.NuclideID = -1;
					AmuletAddEditThyroidUptakeProtocol_protocol.DoseForm = -1;
					AmuletAddEditThyroidUptakeProtocol_protocol.DoseMeasurementMethod = -1;
					AmuletAddEditThyroidUptakeProtocol_protocol.DoseDecayCorrect = TRUE;
					AmuletAddEditThyroidUptakeProtocol_protocol.PreDoseMeasurement = FALSE;
					AmuletAddEditThyroidUptakeProtocol_protocol.ResidualMeasurement = FALSE;
					AmuletAddEditThyroidUptakeProtocol_protocol.I123LowerEV = 143.0;
					AmuletAddEditThyroidUptakeProtocol_protocol.I123UpperEV = 191.0;
					AmuletAddEditThyroidUptakeProtocol_protocol.I131LowerEV = 292.0;
					AmuletAddEditThyroidUptakeProtocol_protocol.I131UpperEV = 437.0;
					AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mLowerEV = 112.0;
					AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mUpperEV = 169.0;
					AmuletAddEditThyroidUptakeProtocol_protocol.DefaultCountTime = 60;
					AmuletAddEditThyroidUptakeProtocol_protocol.DefaultDistance = 25;
					AmuletAddEditThyroidUptakeProtocol_protocol.Inactive = FALSE;
				}else{
					protocol.ProbeTUProtocolID = AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID;
					protocol.ProbeTUProtocolGroupID = -1;
					protocol.ProtocolName[0] = 0;
					protocol.NuclideID = -1;
					protocol.DoseForm = -1;
					protocol.DoseMeasurementMethod = -1;
					protocol.DoseDecayCorrect = TRUE;
					protocol.PreDoseMeasurement = FALSE;
					protocol.ResidualMeasurement = FALSE;
					protocol.I123LowerEV = 143.0;
					protocol.I123UpperEV = 191.0;
					protocol.I131LowerEV = 292.0;
					protocol.I131UpperEV = 437.0;
					protocol.Tc99mLowerEV = 112.0;
					protocol.Tc99mUpperEV = 169.0;
					protocol.DefaultCountTime = 60;
					protocol.DefaultDistance = 25;
					protocol.Inactive = FALSE;
					DB_RetrieveProbeThyroidUptakeProtocol(&protocol);

					if(protocol.ProbeTUProtocolID > 0){
						if(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID < 0){
							// View Parameters
							protocol.ProbeTUProtocolGroupID = AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID;
							memcpy(&AmuletAddEditThyroidUptakeProtocol_protocol, &protocol, sizeof(PROBETHYROIDUPTAKEPROTOCOL));
						}else{
							// Edit Parameters
							memcpy(&AmuletAddEditThyroidUptakeProtocol_protocol, &protocol, sizeof(PROBETHYROIDUPTAKEPROTOCOL));
						}
					}else{
						Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
						return;
					}
				}

				m_ucClear = 0;
			}

			if(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID == -1) SetAmuletString(100, "Add Thyroid Uptake Protocol");
			else{
				if(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID == -1) SetAmuletString(100, "View Thyroid Uptake Protocol");
				else if(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID == -2) SetAmuletString(100, "Delete Thyroid Uptake Protocol");
				else SetAmuletString(100, "Edit Thyroid Uptake Protocol");
			}

			trim(AmuletAddEditThyroidUptakeProtocol_protocol.ProtocolName);
			SetAmuletString(102, AmuletAddEditThyroidUptakeProtocol_protocol.ProtocolName);

			if(AmuletAddEditThyroidUptakeProtocol_protocol.DoseDecayCorrect) SetAmuletString(103, "Decay Correct Administered Dose");
			else SetAmuletString(103, "Measure Same Reference Dose Before Each Uptake");

			switch(AmuletAddEditThyroidUptakeProtocol_protocol.DoseMeasurementMethod){
				case -1:
					SetAmuletString(106, "");
					break;

				case 1:
					SetAmuletString(106, "Dose Measured is Dose Administered");
					break;

				case 2:
					SetAmuletString(106, "Measure Each Dose and Add Activity");
					break;

				case 3:
					SetAmuletString(106, "Measure One Capsule and Multiply by Number Administered");
					break;

				case 4:
					SetAmuletString(106, "Measure Liquid and Multiply by Factor");
					break;

				//case 5:
				//	SetAmuletString(106, "Measure Liquid and Subtract Residual");
				//	break;
			}

			switch(AmuletAddEditThyroidUptakeProtocol_protocol.NuclideID){
				case -1:
					SetAmuletString(109, "");
					break;

				case 40:
					SetAmuletString(109, "I123");
					break;

				case 45:
					SetAmuletString(109, "I131");
					break;

				case 84:
					SetAmuletString(109, "Tc99m");
					break;
			}

			switch(AmuletAddEditThyroidUptakeProtocol_protocol.DoseForm){
				case -1:
					SetAmuletString(110, "");
					break;

				case 1:
					SetAmuletString(110, "Capsule");
					break;

				case 2:
					SetAmuletString(110, "Liquid");
					break;
			}

			if(AmuletAddEditThyroidUptakeProtocol_protocol.PreDoseMeasurement) SetAmuletString(111, "Yes");
			else SetAmuletString(111, "No");

			if(AmuletAddEditThyroidUptakeProtocol_protocol.ResidualMeasurement) SetAmuletString(112, "Yes");
			else SetAmuletString(112, "No");

			if(AmuletAddEditThyroidUptakeProtocol_protocol.DefaultCountTime == -1) message[0] = 0;
			else sprintf(message, "%d sec", AmuletAddEditThyroidUptakeProtocol_protocol.DefaultCountTime);
			SetAmuletString(113, message);

			if(AmuletAddEditThyroidUptakeProtocol_protocol.DefaultDistance == -1) message[0] = 0;
			else sprintf(message, "%d cm", AmuletAddEditThyroidUptakeProtocol_protocol.DefaultDistance);
			SetAmuletString(114, message);

			if(AmuletAddEditThyroidUptakeProtocol_protocol.I123LowerEV > AmuletAddEditThyroidUptakeProtocol_protocol.I123UpperEV){
				tempvalue = AmuletAddEditThyroidUptakeProtocol_protocol.I123LowerEV;
				AmuletAddEditThyroidUptakeProtocol_protocol.I123LowerEV = AmuletAddEditThyroidUptakeProtocol_protocol.I123UpperEV;
				AmuletAddEditThyroidUptakeProtocol_protocol.I123UpperEV = tempvalue;
			}
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.I123LowerEV);
			SetAmuletString(115, message);
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.I123UpperEV);
			SetAmuletString(116, message);

			if(AmuletAddEditThyroidUptakeProtocol_protocol.I131LowerEV > AmuletAddEditThyroidUptakeProtocol_protocol.I131UpperEV){
				tempvalue = AmuletAddEditThyroidUptakeProtocol_protocol.I131LowerEV;
				AmuletAddEditThyroidUptakeProtocol_protocol.I131LowerEV = AmuletAddEditThyroidUptakeProtocol_protocol.I131UpperEV;
				AmuletAddEditThyroidUptakeProtocol_protocol.I131UpperEV = tempvalue;
			}
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.I131LowerEV);
			SetAmuletString(117, message);
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.I131UpperEV);
			SetAmuletString(118, message);

			if(AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mLowerEV > AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mUpperEV){
				tempvalue = AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mLowerEV;
				AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mLowerEV = AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mUpperEV;
				AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mUpperEV = tempvalue;
			}
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mLowerEV);
			SetAmuletString(119, message);
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mUpperEV);
			SetAmuletString(120, message);

			if(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID == -1) SetAmuletByte(100, 0xFF);
			else{
				if(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID == -1) SetAmuletByte(101, 0xFF); // View Parameters
				else if(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID == -2) SetAmuletByte(102, 0xFF); // Delete protocol
				else SetAmuletByte(100, 0xFF);
			}
			m_iPhase = PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_WAIT;
			break;

		case PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_WAIT:
			break;

		case PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_DEFAULTROI:
			beep_amulet();
			AmuletAddEditThyroidUptakeProtocol_protocol.I123LowerEV = 143.0;
			AmuletAddEditThyroidUptakeProtocol_protocol.I123UpperEV = 191.0;
			AmuletAddEditThyroidUptakeProtocol_protocol.I131LowerEV = 292.0;
			AmuletAddEditThyroidUptakeProtocol_protocol.I131UpperEV = 437.0;
			AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mLowerEV = 112.0;
			AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mUpperEV = 169.0;
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.I123LowerEV);
			SetAmuletString(115, message);
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.I123UpperEV);
			SetAmuletString(116, message);
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.I131LowerEV);
			SetAmuletString(117, message);
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.I131UpperEV);
			SetAmuletString(118, message);
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mLowerEV);
			SetAmuletString(119, message);
			sprintf(message, "%.1f", AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mUpperEV);
			SetAmuletString(120, message);
			SetAmuletByte(103, 0xFF);
			m_iPhase = PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_WAIT;
			break;

		case PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_SAVE:
			if(AmuletAddEditThyroidUptakeProtocol_protocol.ProtocolName[0] == 0){
				beep_amulet();
				Amulet_DisplayError("Thyroid Uptake Protocol", "Missing Thyroid Uptake Protocol Name", TRUE);
				return;
			}else{
				if(DB_TestProbeThyroidUptakeProtocolName(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID, AmuletAddEditThyroidUptakeProtocol_protocol.ProtocolName)){
					beep_amulet();
					Amulet_DisplayError("Thyroid Uptake Protocol", "Duplicate Thyroid Uptake Protocol Name", TRUE);
					return;
				}
			}

			if(AmuletAddEditThyroidUptakeProtocol_protocol.DoseMeasurementMethod == -1){
				beep_amulet();
				Amulet_DisplayError("Thyroid Uptake Protocol", "Missing Dose Measurement Method", TRUE);
				return;
			}

			if(AmuletAddEditThyroidUptakeProtocol_protocol.NuclideID == -1){
				beep_amulet();
				Amulet_DisplayError("Thyroid Uptake Protocol", "Missing Nuclide", TRUE);
				return;
			}

			if(AmuletAddEditThyroidUptakeProtocol_protocol.DoseForm == -1){
				beep_amulet();
				Amulet_DisplayError("Thyroid Uptake Protocol", "Missing Dose Form", TRUE);
				return;
			}

			if(AmuletAddEditThyroidUptakeProtocol_protocol.DefaultCountTime == -1){
				beep_amulet();
				Amulet_DisplayError("Thyroid Uptake Protocol", "Missing Counting Time", TRUE);
				return;
			}

			if(AmuletAddEditThyroidUptakeProtocol_protocol.DefaultDistance == -1){
				beep_amulet();
				Amulet_DisplayError("Thyroid Uptake Protocol", "Missing Probe Distance", TRUE);
				return;
			}

			if((AmuletAddEditThyroidUptakeProtocol_protocol.DoseMeasurementMethod == 3) && AmuletAddEditThyroidUptakeProtocol_protocol.DoseForm == 2){
				beep_amulet();
				Amulet_DisplayError("Thyroid Uptake Protocol", "Dose Measurement implies that Dose Form is a capsule", TRUE);
				return;
			}

			if((AmuletAddEditThyroidUptakeProtocol_protocol.DoseMeasurementMethod == 4) && (AmuletAddEditThyroidUptakeProtocol_protocol.DoseForm == 1)){
				beep_amulet();
				Amulet_DisplayError("Thyroid Uptake Protocol", "Dose Measurement implies that Dose Form is a liquid", TRUE);
				return;
			}

			if(AmuletAddEditThyroidUptakeProtocol_protocol.ResidualMeasurement && AmuletAddEditThyroidUptakeProtocol_protocol.DoseForm == 1){
				beep_amulet();
				Amulet_DisplayError("Thyroid Uptake Protocol", "Residual Measurement implies that Dose Form is a liquid", TRUE);
				return;
			}

			DB_WriteProbeThyroidUptakeProtocol(&AmuletAddEditThyroidUptakeProtocol_protocol, TRUE);

			SetAmuletBackHTML();
			return;

		case PHASE_ADDEDITTHYROIDUPTAKEPROTOCOL_DELETE:
			DB_RetrieveProbeThyroidUptakeProtocol(&AmuletAddEditThyroidUptakeProtocol_protocol);
			if(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
				return;
			}
			if(AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID > 0){
				DB_InactivateProbeThyroidUptakeProtocol(&AmuletAddEditThyroidUptakeProtocol_protocol);
				SetAmuletBackHTML();
				return;
			}else{
				Amulet_DisplayError("Thyroid Uptake", "Invalid TU Protocol GID", TRUE);
				return;
			}
	}
}

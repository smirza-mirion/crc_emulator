/**
 * \file
 * \details This file handles calls from the Amulet Geometry Report Screen
 */
#define PHASE_GEOMETRY_REPORT_PRE_INIT 0
#define PHASE_GEOMETRY_REPORT_WAIT 1
#define PHASE_GEOMETRY_REPORT_PRINT 2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "Amulet.h"
#include "chambfac.h"
#include "printer.h"

typedef struct geomeas GEOMEAS;
struct geomeas{
	float meas_act;
	float totvol;
	char act_str[10];
	float var;
};

extern char Geometry_acContainer[8];
extern int m_iPhase;
extern CURRENT  current;
extern GEOMEAS geomeas[10];
extern int Geometry_nmeas;

void send_to_amulet_string(uchar ucIndex, char message0[]);
void Amulet_printGeometry(short num_meas, short ref_num, short sorv);

/**
 * \details Handles the Amulet Scree GeometryReport.htm. GeometryReport.htm displays the results of the geometry test.
 * \param Amulet_Byte_ID Description
 * \param 80 Language		(STATE) English = 0, French = 1
 * \param 168 0xFF = Display Report
 * \param 169 0xFF = Display Line 1
 * \param 170 0xFF = Display Line 2
 * \param 171 0xFF = Display Line 3
 * \param 172 0xFF = Display Line 4
 * \param 173 0xFF = Display Line 5
 * \param 174 0xFF = Display Line 6
 * \param 175 0xFF = Display Line 7
 * \param 176 0xFF = Display Line 8
 * \param 177 0xFF = Display Line 9
 * \param 178 0xFF = Display Line 10
 * \param 179 0xFF = Display Line 11
 * \param Amulet_String_ID Description
 * \param 100_101 Title
 * \param 102 Using Volume
 * \param 103 sfTitleAssay
 * \param 104 % Var
 * \param 169 sfVol1
 * \param 170 sfAssay1
 * \param 171 sfVar1
 * \param 172 sfVol2
 * \param 173 sfAssay2
 * \param 174 sfVar2
 * \param 175 sfVol3
 * \param 176 sfAssay3
 * \param 177 sfVar3
 * \param 178 sfVol4
 * \param 179 sfAssay4
 * \param 180 sfVar4
 * \param 181 sfVol5
 * \param 182 sfAssay5
 * \param 183 sfVar5
 * \param 184 sfVol6
 * \param 185 sfAssay6
 * \param 186 sfVar6
 * \param 187 sfVol7
 * \param 188 sfAssay7
 * \param 189 sfVar7
 * \param 190 sfVol8
 * \param 191 sfAssay8
 * \param 192 sfVar8
 * \param 193 sfVol9
 * \param 194 sfAssay9
 * \param 195 sfVar9
 * \param 196 sfVol10
 * \param 197 sfAssay10
 * \param 198 sfVar10
 * \returns None
 */
void AmuletGeometryReport_menu(void){
	float ref_act;
	short i;
	char acMsg[100];
	//static char acContainer[8];

	switch(m_iPhase){
		case PHASE_GEOMETRY_REPORT_PRE_INIT:
			if(current.num_chambers>0){
				SetAmuletByte(80, current.language);
				send_amulet_message(L_GEOMETRY_REPORT, 100);    // "Geometry Report"
				send_amulet_message(L_USING_VOLUME, 102);    // "Using Volume"
				if(!strcmp(Geometry_acContainer, "Syringe")) send_amulet_message(L_SYRINGE_ASSAY, 103);    // "Syringe Assay"
				else if(!strcmp(Geometry_acContainer, "Vial")) send_amulet_message(L_VIAL_ASSAY, 103);    // "Vial Assay"
				send_amulet_message(L_PERCENT_VAR, 104);    // "% Var"
				sprintf(acMsg, "Ch: %d", current.main_chamber + 1);
				switch(chamber_type(current.main_chamber)){
					case R_CHAMB:
						strcat(acMsg, ", R");
						break;
					case P_CHAMB:
						strcat(acMsg, ", PET");
						break;
					case B_CHAMB:
						strcat(acMsg, ", BT");
						break;
					case ONE_DOT_EIGHT_CHAMB:
						strcat(acMsg, ", 1.8 Atm");
						break;
					case C_CHAMB:
						strcat(acMsg, ", HR");
						break;
					case K_CHAMB:
						strcat(acMsg, ", 1K");
						break;
				}
				send_to_amulet_string(9, acMsg);
				SetAmuletByte(20, 0xFF);

				ref_act = geomeas[0].meas_act;
				for(i=0; i<Geometry_nmeas; i++){
					geomeas[i].var = 100 * ((geomeas[i].meas_act - ref_act)/ref_act);
				}
				//strcpy(acMsg, Geometry_acContainer);
				//strcat(acMsg, " Assay");
				//SetAmuletString(168, acMsg);
				delayloop(5);
				SetAmuletByte(169, 0xFF);
				delayloop(10);
				for(i=0; i<Geometry_nmeas; i++){
					sprintf(acMsg, "%.3fml",geomeas[i].totvol);
					send_to_amulet_string((3*i)+0+169, acMsg);
					delayloop(5);
					send_to_amulet_string((3*i)+1+169, geomeas[i].act_str);
					delayloop(5);
					if(i==0){
						//strcpy(acMsg, "BASE");
						get_amulet_message(L_BASE, acMsg);    // "BASE"
					}else{
						sprintf(acMsg, "%.1f", geomeas[i].var);
					}
					send_to_amulet_string((3*i)+2+169, acMsg);
					delayloop(5);
					//SetAmuletByte(i+170, 0xFF);
					//delayloop(20);
				}
				SetAmuletByte(168, 0xFF);
				delayloop(100);
				for(i=0; i<Geometry_nmeas; i++){
					SetAmuletByte(i+170, 0xFF);
				}
				//if(current.printer!=NONE_PRINTER)
				if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER)) SetAmuletByte(100, 0xFF);
				m_iPhase = PHASE_GEOMETRY_REPORT_WAIT;
			}
			break;

		case PHASE_GEOMETRY_REPORT_WAIT:
			break;

		case PHASE_GEOMETRY_REPORT_PRINT:
			if(strcmp("Syringe", Geometry_acContainer)==0) Amulet_printGeometry(Geometry_nmeas, 0, 1);
			else Amulet_printGeometry(Geometry_nmeas, 0, 2);

			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_GEOMETRY_REPORT_WAIT;
			break;
	}
}

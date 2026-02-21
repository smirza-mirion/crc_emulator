#pragma once
/*******************************************************
  MODULE:  Linearity Test header File

  FILE:  linearity.h

  DATE:  03/28/05
  *******************************************************************/

//linearity tests
enum
{
    STANDARD,
    LINEATOR,
    CALICHECK,
};

//linarity test definition structure
typedef struct lindef LINDEF;
struct lindef
{
    short num_Std[2];		//number of standard readings, R (77t), PET
    short nuc_index_Std[2];	//Nuclide of standard readings, R (77t), PET
    short hours_Std[2][12];	//test time for standard reading, R (77t), PET

    short num_Lin[2];			// Number of Lineator readings, Chamber 0, 1
    short nuc_index_Lin[2];		// Nuclide of Lineator readings, Chamber 0, 1
    char chamb_num_Lin[2][7];	// Chamber Number, Chamber 0, 1
    char serial_num_Lin[2][11]; // Serial Number of Lineator Set
    float factors_Lin[2][8];	// Factors for Lineator, Chamber 0, 1

    short num_Cali[2];				// Number of Calicheck readings, Chamber 0, 1
    short nuc_index_Cali[2];		// Nuclide of Calicheck readings, Chamber 0, 1
    char chamb_num_Cali[2][7];		// Chamber Number, Chamber 0 1
    char serial_num_Cali[2][11];	// Serial Number of Calicheck Set
    float factors_Cali[2][12];  	//factors for Calicheck, Chamber 0, 1
};    

//standard linearity test structure for 1 measurement
typedef struct standlin STANDLIN;
struct standlin
{
    time_t date;        //date,time of measurement
    float meas;         //activity
    //char  syst;         //Ci or Bq
    short meas_flag;    //0 not measured, 1 measured, -1 not in protocol
    char actstr[10];    //formatted activity
};


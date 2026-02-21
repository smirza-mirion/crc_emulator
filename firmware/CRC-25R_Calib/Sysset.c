#include "crc.h"
#include "remote.h"
#include "nuc.h"
#include "printer.h"
#include "i2c.h"

extern CHAMBER chamber[8];
extern ushort chamber_checksum[8];
extern REMOTE remote[8];
extern CURRENT current;
void write_binary_to_Options_folder(char *acFilename, char *acData, long int lFilelength);

static SYSSETTING sysset;

void Sysset_clearStruct(void){
	short index, jndex;

	for(index=0; index<8; index++){
		chamber[index].exists = FALSE;
		chamber[index].active = FALSE;
		chamber[index].control = CONTROL_MAIN;
		chamber[index].connected_flag = FALSE;
		chamber[index].adc_zero = 0;
		chamber[index].bkg = 0;
		chamber[index].zero = 0;
		chamber[index].nuc_index = NuclideData_getIndexFromName("Tc99m");
		chamber[index].type = REF;
		chamber[index].num_cal_corr = 0;
		for(jndex=0; jndex<4; jndex++) chamber[index].calcor.corr[jndex] = 0;
		chamber[index].calkey = FALSE;
		NuclideData_getNuclide(NuclideData_getIndexFromName("Tc99m"), &(chamber[index].nucdata));
		for(jndex=0; jndex<11; jndex++) chamber[index].calstrng[jndex] = 0;

		chamber_checksum[index] = 0xFFFF;

		remote[index].exists = FALSE;
		remote[index].keys_active = FALSE;
		remote[index].valid_flag = FALSE;
		remote[index].nuclist = REMOTE_NUCLIDE_OTHER;
		remote[index].acc_num = REMOTE_ACCURACY_TEST1;
		remote[index].cmd = REMOTE_NO_CMD;
		remote[index].sub_cmd = REMOTE_NO_CMD;
		for(jndex=0; jndex<30; jndex++) remote[index].buffer[jndex] = 0;
		remote[index].mode = REMOTE_HEARTBEAT_MODE;

		sysset.sdchamb[index].adc_zero = 0;
		sysset.sdchamb[index].bkg = 0;
		sysset.sdchamb[index].zero = 0;
		sysset.sdchamb[index].checksum = 0xFFFF;
	}

	current.system = CI;
	current.lock = FALSE;
	current.num_chambers = 0;
	current.main_chamber = 0;
	current.detector = DETECTOR_NONE;
	current.enhanced = FALSE;
	//current.lockedbq = FALSE;
	current.pccomm = PC_COMM_NONE;
	current.source_index = 4;
	current.demo_mode = FALSE;
	current.printer = NONE_PRINTER;
}

void *Sysset_getMirror(void){
	return (void *) &sysset;
}

long Sysset_getMirrorSize(void){
	return sizeof(sysset);
}

void Sysset_updateStruct(void){
	short index;

	for(index=0; index<8; index++){
		chamber[index].adc_zero = sysset.sdchamb[index].adc_zero;
		chamber[index].bkg = sysset.sdchamb[index].bkg;
		chamber[index].zero = sysset.sdchamb[index].zero;

		EE_READ(nuclideID[index], (uchar *) &(chamber[index].nuc_index));
		NuclideData_getNuclide(chamber[index].nuc_index, &(chamber[index].nucdata));
		chamber_checksum[index] = sysset.sdchamb[index].checksum;
	}
}

void Sysset_updateMirror(void){
	short index;

	for(index=0; index<8; index++){
		sysset.sdchamb[index].adc_zero = chamber[index].adc_zero;
		sysset.sdchamb[index].bkg = chamber[index].bkg;
		sysset.sdchamb[index].zero = chamber[index].zero;
		sysset.sdchamb[index].checksum = chamber_checksum[index];
	}
	write_binary_to_Options_folder("sysset.bin", (char *) &sysset, sizeof(sysset));
}

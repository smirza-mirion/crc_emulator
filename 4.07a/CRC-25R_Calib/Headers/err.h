enum{
	ERR_MCA_NOT_INSTALLED = 1,	// MCA BOARD NOT PRESENT
	ERR_MCA_EE_SIG_MISSING,    	// CapMca can not be read from EEPROM on the MCA BOARD
	ERR_MCA_PROG_LOAD_FAILURE,	// Error while loading DSP program into MCA BOARD
	ERR_MCA_PROG_NOT_RUNNING,	// Failed check of MCA after DSP program has been loaded
	ERR_MCA_DETECTOR_NOT_ATTACHED, // No detector was attached to MCA Board
	ERR_MCA_UNKNOWN_DETECTOR,		// Unable to determine detector attached to MCA Board
	ERR_MCA_COMMAND_TIMEOUT,		// MCAWaitForFinish has timed out
};

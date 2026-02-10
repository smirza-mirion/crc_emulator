#define USB_WRITE_TIMEOUT               50000     //= 50 sec

// Offsets from pCS1 to access sl811 registers
#define USB_HOST_OFF_SET            6       // set register address
#define USB_HOST_OFF_RW             7       // reads from /writes to register

// sl811 host registers addresses
#define	REG_CONTROL                 0x00    // write this register to kick off a transfer
#define REG_CONTROL_B               0x08	// write this register to kick off a transfer
#define	REG_BUFFER_ADDRESS          0x01    // start of internal data buffer
#define REG_BUFFER_ADDRESS_B        0x09	// start of internal data buffer
#define	REG_BUFFER_LENGTH           0x02    // length of internal buffer
#define REG_BUFFER_LENGTH_B         0x0A	// length of internal buffer
#define	REG_PID_ENDPOINT            0x03    // name when written--PID and Endpoint for next xfr
#define REG_PID_ENDPOINT_B          0x0B	// name when written--PID and Endpoint for next xfr
#define	REG_PACKET_STATUS           0x03    // name when read--status of last transfer
#define REG_PACKET_STATUS_B         0x0B	// name when read--status of last transfer
#define	REG_DEVICE_ADDRESS          0x04    // name when written--USB function address
#define REG_DEVICE_ADDRESS_B        0x0C	// name when written--USB function address
#define REG_TRANSFER_COUNT          0x04	// name when read--number of bytes received or transfered
#define REG_TRANSFER_COUNT_B        0x0C	// name when read--number of bytes received or transfered
#define	REG_CONTROL_1               0x05    // more control stuff
#define REG_INT_ENABLE              0x06    //
#define	REG_INT_STATUS              0x0D    // Interrupt request status bits. We use DONE and SOF.
#define	REG_SOF_COUNT_LO            0x0E    // SOF (EOP) time constant low byte
#define	REG_SOF_COUNT_HI            0x0F    // name when written--EOP time constant high byte

// sl811 buffer values
#define VAL_BUFFER_OFFSET           0x10    // Base location for sl811 data buffer

// sl811 PID (Packet Identifier) bit settings (must be ORed with endpoint in the low nibble)
#define	BIT_SETUP_PID               0xD0    // for the 'set address' request
#define	BIT_IN_PID                  0x90    // IN transactions
#define BIT_OUT_PID                 0x10    // OUT transactions
#define	BIT_SOF_PID                 0x50    // start of frame marker
#define BIT_PREAMBLE_PID            0xC0
#define BIT_NAK_PID                 0xA0
#define BIT_STALL_PID               0xE0
#define BIT_DATA0_PID               0x30
#define BIT_DATA1_PID               0xB0

// Endpoint must be ORed with PID in the high nibble
#define BIT_EP0                     0x00    // Endpoint 0
#define BIT_EP1                     0x01    // Endpoint 1
#define BIT_EP2                     0x02    // Endpoint 2

// Control Bit Settings
#define BIT_CTRL_ARM                0x01
#define BIT_CTRL_ENABLE             0x02
#define BIT_CTRL_IN                 0x00
#define BIT_CTRL_OUT                0x04

// constants for 811 CONTROL 1 register
#define	USB_RESET                           0x08    // Reset USB bus
#define	USB_OPERATE_LO_SPEED                0x21    // Low Speed=1(b5) and SOF(EOP)EN=1(b0)
#define USB_OPERATE_HI_SPEED                0x01    // Low Speed=0(b5) and SOF(EOP)EN=1(b0)
#define USB_OPERATE_HI_SPEED_WITHOUT_SOF    0x00    // Low Speed=0(b5) and SOF(EOP)EN=0(b0)

// sl811 Interrupt Enable bit settings
#define BIT_INT_USB_A_ENABLE        0x01    // Enables USB A interrupt
#define BIT_INT_USB_B_ENABLE        0x02    // Enables USB B interrupt
#define BIT_INT_SOF_ENABLE          0x10    // Enables the Start Of Frame interrupt
#define BIT_INT_INSERT_ENABLE       0x20    // Enables the Slave Insertion / Removal interrupt
#define BIT_INT_DEVICE_ENABLE       0x40    // Enables the Device Detect interrupt

// sl811 Interrupt Status bit settings
#define BIT_INT_USB_A_STATUS        0x01    // USB A interrupt status
#define BIT_INT_USB_B_STATUS        0x02    // USB B interrupt status
#define BIT_INT_SOF_STATUS          0x10    // Start Of Frame interrupt status
#define BIT_INT_INSERT_STATUS       0x20    // Slave Insertion / Removal interrupt status
#define BIT_INT_DEVICE_STATUS       0x40    // Device Detect state
#define BIT_INT_D_PLUS_STATUS       0x80    // D+ state

// USB printer port bit settings
#define BIT_PORT_STATUS_EMPTY       0x20    // Paper Empty Condition on printer (1 = empty)
#define BIT_PORT_STATUS_SELECT      0x10    // Selected (1 = selected)
#define BIT_PORT_STATUS_NOT_ERROR   0x08    // No Error (0 = Error)

// sl811 Packet Status bit settings
#define BIT_STATUS_ACK              0x01
#define BIT_STATUS_ERROR            0x02
#define BIT_STATUS_TIMEOUT          0x04
#define BIT_STATUS_SEQ              0x08
#define BIT_STATUS_SETUP            0x10
#define BIT_STATUS_OVERFLOW         0x20
#define BIT_STATUS_NAK              0x40
#define BIT_STATUS_STALL            0x80

#define KEEP_ALIVE_COUNT            500

// USB DESCRIPTOR
#define DESC_TYPE_CONFIGURATION		0x02
#define DESC_TYPE_INTERFACE 		0x04
#define DESC_TYPE_ENDPOINT          0x05

#define ATTACHED_UNKNOWN	0x00
#define ATTACHED_PRINTER	0x01
#define ATTACHED_FLASH		0x02

#define WAIT_FOR_INTA_TIMEOUT	100                 // Number of 10 ms cycles to wait for a response before jumping to P_RESET_USB_BUS

#define CIR_BUF_SIZE 32767

#define MAX_STATE	18

#define MASTER							0
#define MASTER_WAIT						0
#define MASTER_RESET_USB_BUS			1
#define MASTER_ENABLE_USB_BUS			2
#define MASTER_CHK_ATTACHED_DEVICE		3
#define MASTER_ASK_DESC_DEV_INIT		4
#define MASTER_RESET_USB_BUS2			5
#define MASTER_ISSUE_SET_USB_ADDRESS	6
#define MASTER_GET_DESCRIPTOR			7
#define MASTER_READ_FULL_DEVICE_DESCRIPTOR 8
#define MASTER_READ_INIT_CONFIG_DESCRIPTOR 9
#define MASTER_READ_FULL_CONFIG_DESCRIPTOR 10
#define MASTER_SET_CONFIGURATION 		11
#define MASTER_SET_INTERFACE			12
#define MASTER_READ_PRINTER_CLASS_DESCRIPTOR	13
#define MASTER_PRINTER					14
#define MASTER_SCSI_MAX_LUN				15
#define MASTER_SCSI_MAX_LUN_STALL		16
#define MASTER_SCSI_MAX_LUN_SENSE		17
#define MASTER_SCSI_INQUIRY				18
#define MASTER_SCSI_INQUIRY_STALL		19
#define MASTER_SCSI_INQUIRY_SENSE		20
#define MASTER_SCSI_READ_CAPACITY		21
#define MASTER_SCSI_READ_CAPACITY_STALL	22
#define MASTER_SCSI_READ_CAPACITY_SENSE 23
#define MASTER_SCSI_TEST_UNIT_READY		24
#define MASTER_SCSI_TEST_UNIT_READY_STALL 25
#define MASTER_SCSI_TEST_UNIT_READY_SENSE 26
#define MASTER_SCSI						27
#define MASTER_POLL_STALL			  	28

#define TIMER					1
#define TIMER_WAIT				0
#define TIMER_WAIT_FOR_TIMER	1

#define USBA				2
#define USBA_WAIT			0
#define USBA_WAIT_FOR_USBA	1

#define SETADDR				3
#define SETADDR_WAIT		0
#define SETADDR_ERROR		1
#define SETADDR_STALL		2
#define SETADDR_IN			3
#define SETADDR_FINISHED	4

#define GETDESC				4
#define GETDESC_WAIT		0
#define GETDESC_ERROR		1
#define GETDESC_STALL		2
#define GETDESC_INIT_IN		3
#define GETDESC_IN			4
#define GETDESC_FINISHED	5

#define SETCONFIG			5
#define SETCONFIG_WAIT		0
#define SETCONFIG_ERROR		1
#define SETCONFIG_STALL		2
#define SETCONFIG_IN		3
#define SETCONFIG_FINISHED	4

#define SETINTFACE			6
#define SETINTFACE_WAIT		0
#define SETINTFACE_ERROR	1
#define SETINTFACE_STALL	2
#define SETINTFACE_IN		3
#define SETINTFACE_FINISH	4

#define RUNPRINTER			7
#define RUNPRINTER_WAIT		0
#define RUNPRINTER_ERROR	1
#define RUNPRINTER_STALL	2
#define RUNPRINTER_SEND		3
#define RUNPRINTER_FINISH	4

#define SCSIIN				8
#define SCSIIN_WAIT			0
#define SCSIIN_ERROR		1
#define SCSIIN_STALL		2
#define SCSIIN_IN			3

#define SCSIINQUIRY			9
#define SCSIINQUIRY_WAIT	0
#define SCSIINQUIRY_ERROR	1
#define SCSIINQUIRY_STALL	2
#define SCSIINQUIRY_IN		3
#define SCSIINQUIRY_CSW		4
#define SCSIINQUIRY_FINISH	5

#define SCSITESTUNITREADY		10
#define SCSITESTUNITREADY_WAIT	0
#define SCSITESTUNITREADY_ERROR	1
#define SCSITESTUNITREADY_STALL 2
#define SCSITESTUNITREADY_CSW	3
#define SCSITESTUNITREADY_FINISH	4

#define SCSIREADCAPACITY		11
#define SCSIREADCAPACITY_WAIT	0
#define SCSIREADCAPACITY_ERROR	1
#define SCSIREADCAPACITY_STALL	2
#define SCSIREADCAPACITY_IN		3
#define SCSIREADCAPACITY_CSW	4
#define SCSIREADCAPACITY_FINISH	5

#define SCSIREADBLOCK			12
#define SCSIREADBLOCK_WAIT		0
#define SCSIREADBLOCK_ERROR		1
#define SCSIREADBLOCK_STALL		2
#define SCSIREADBLOCK_IN		3
#define SCSIREADBLOCK_CSW		4
#define SCSIREADBLOCK_FINISH	5

#define SCSIWRITEBLOCK			13
#define SCSIWRITEBLOCK_WAIT		0
#define SCSIWRITEBLOCK_ERROR	1
#define SCSIWRITEBLOCK_STALL	2
#define SCSIWRITEBLOCK_OUT		3
#define SCSIWRITEBLOCK_CSW		4
#define SCSIWRITEBLOCK_FINISH	5

#define SCSIININT				14
#define SCSIININT_WAIT			0
#define SCSIININT_ERROR			1
#define SCSIININT_STALL			2
#define SCSIININT_IN			3

#define SCSIOUTINT				15
#define SCSIOUTINT_WAIT			0
#define SCSIOUTINT_ERROR		1
#define SCSIOUTINT_STALL		2
#define SCSIOUTINT_OUT			3

#define CLEARFEATURE			16
#define CLEARFEATURE_WAIT		0
#define CLEARFEATURE_ERROR		1
#define CLEARFEATURE_STALL		2
#define CLEARFEATURE_IN			3
#define CLEARFEATURE_RERUNLAST	4
#define CLEARFEATURE_FINISH		5

#define REQUESTSENSE			17
#define REQUESTSENSE_WAIT		0
#define REQUESTSENSE_ERROR		1
#define REQUESTSENSE_STALL		2
#define REQUESTSENSE_IN			3
#define REQUESTSENSE_CSW		4
#define REQUESTSENSE_FINISH		5

#define NAK_REPLY_INSIDE_INTERRUPT	0
#define NAK_DELAY_REPLY_INSIDE_PIT	1
#define NAK_REPLY_SENT_FROM_PIT		2

typedef struct state STATE;
struct state{
	int Phase;
	int ReturnState;
	int ReturnPhase;
	int ReturnError;
	int ReturnStall;
};

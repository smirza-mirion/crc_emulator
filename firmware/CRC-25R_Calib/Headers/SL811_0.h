#pragma once
#define SL811_EC_ARM		0x01
#define SL811_EC_ENABLE		0x02
#define SL811_EC_DIR_TO_HOST	0x04
#define SL811_EC_NEXT_DATA_B	0x08
#define SL811_EC_ISO		0x10
#define SL811_EC_SEND_STALL	0x20
#define SL811_EC_SEQ_DATA_1	0x40
void set_811_endpoint_control (uchar endpoint, uchar dataset, uchar data);
uchar get_811_endpoint_control(uchar endpoint, uchar dataset);

void set_811_endpoint_base_address(uchar endpoint, uchar dataset, uchar data);
uchar get_811_endpoint_base_address(uchar endpoint, uchar dataset);

void set_811_endpoint_base_length(uchar endpoint, uchar dataset, uchar data);
uchar get_811_endpoint_base_length(uchar endpoint, uchar dataset);

#define SL811_EPS_ACK		0x01
#define SL811_EPS_ERR		0x02
#define SL811_EPS_TIMEOUT		0x04
#define SL811_EPS_SEQ_DATA_1	0x08
#define SL811_EPS_SETUP		0x10
#define SL811_EPS_OVERFLOW	0x20
#define SL811_EPS_NAK		0x40
#define SL811_EPS_STALL		0x80
void set_811_endpoint_packet_status(uchar endpoint, uchar dataset, uchar data);
uchar get_811_endpoint_packet_status(uchar endpoint, uchar dataset);

void set_811_endpoint_transfer_count(uchar endpoint, uchar dataset, uchar data);
uchar get_811_endpoint_transfer_count(uchar endpoint, uchar dataset);

#define SL811_CTRL_USB_ENABLE 0x01
#define SL811_CTRL_DMA_ENABLE 0x02
#define SL811_CTRL_DMA_READ	0x04
#define SL811_CTRL_J_K_0		0x08
#define SL811_CTRL_J_K_1		0x10
#define SL811_CTRL_LOW_SPD	0x20
#define SL811_CTRL_STANDBY	0x40
void set_811_control(uchar data);
uchar get_811_control(void);

#define SL811_IE_END_0		0x01
#define SL811_IE_END_1		0x02
#define SL811_IE_END_2		0x04
#define SL811_IE_END_3		0x08
#define SL811_IE_DMA			0x10
#define SL811_IE_SOF			0x20
#define SL811_IE_USB_RESET	0x40
#define SL811_IE_DMA_TRANSFER	0x80
void set_811_interrupt_enable(uchar data);
uchar get_811_interrupt_enable(void);

void set_811_usb_address(uchar data);
uchar get_811_usb_address(void);

#define SL811_IS_END_0		0x01
#define SL811_IS_END_1		0x02
#define SL811_IS_END_2		0x04
#define SL811_IS_END_3		0x08
#define SL811_IS_DMA			0x10
#define SL811_IS_SOF			0x20
#define SL811_IS_USB_RESET	0x40
#define SL811_IS_DMA_TRANSFER	0x80
void set_811_interrupt_status(uchar data);
uchar get_811_interrupt_status(void);

#define SL811_CD_END_0_B		0x01
#define SL811_CD_END_1_B		0x02
#define SL811_CD_END_2_B		0x04
#define SL811_CD_END_3_B		0x08
void set_811_current_dataset(uchar data);
uchar get_811_current_dataset(void);

uchar get_811_SOF_low(void);
uchar get_811_SOF_high(void);

void set_811_DMA_low(uchar data);
uchar get_811_DMA_low(void);
void set_811_DMA_high(uchar data);
uchar get_811_DMA_high(void);
void startup_811(void);
void isr_811(void);
void get_usb_buffer(uchar base_address, uint length, uchar *ptr_dump);
void set_usb_buffer(uchar base_address, uchar length, uchar set_value);
void set_usb_buffer_array (uchar base_adderss, uchar length, uchar *ptr_value);
void print_message_values (uchar direction, uchar request_type, uchar recipient,
                           uchar value_low, uchar value_high, uchar index_low,
                           uchar index_high, uchar length_low, uchar length_high);

 typedef struct reg_block REG_BLOCK;    
 struct reg_block
{
	uchar ep0_a_control;		// 0x00
	uchar ep0_a_base_address;	// 0x01
	uchar ep0_a_length;			// 0x02
	uchar ep0_a_packet_status;	// 0x03
	uchar ep0_a_transfer_count; // 0x04
	uchar sl811_control;		// 0x05
	uchar sl811_int_enable;		// 0x06
	uchar sl811_usb_address;	// 0x07
	uchar ep0_b_control;	    // 0x08
	uchar ep0_b_base_address;	// 0x09
	uchar ep0_b_length;			// 0x0a
	uchar ep0_b_packet_status;	// 0x0b
	uchar ep0_b_transfer_count; // 0x0c
	uchar sl811_int_status;		// 0x0d
	uchar sl811_current_data;	// 0x0e
	uchar dummy1;				// 0x0f
	uchar ep1_a_control;		// 0x10
	uchar ep1_a_base_address;	// 0x11
	uchar ep1_a_length;			// 0x12
	uchar ep1_a_packet_status;	// 0x13
	uchar ep1_a_transfer_count; // 0x14
	uchar sl811_SOF_lo;			// 0x15
	uchar sl811_SOF_hi;			// 0x16
	uchar dummy2;				// 0x17
	uchar ep1_b_control;	    // 0x18
	uchar ep1_b_base_address;	// 0x19
	uchar ep1_b_length;			// 0x1a
	uchar ep1_b_packet_status;	// 0x1b
	uchar ep1_b_transfer_count; // 0x1c
	uchar dummy3[3];			// 0x1d, 0x1e, 0x1f
	uchar ep2_a_control;		// 0x20
	uchar ep2_a_base_address;	// 0x21
	uchar ep2_a_length;			// 0x22
	uchar ep2_a_packet_status;	// 0x23
	uchar ep2_a_transfer_count; // 0x24
	uchar dummy4[3];			// 0x25, 0x26, 0x27
	uchar ep2_b_control;	    // 0x28
	uchar ep2_b_base_address;	// 0x29
	uchar ep2_b_length;			// 0x2a
	uchar ep2_b_packet_status;	// 0x2b
	uchar ep2_b_transfer_count; // 0x2c
	uchar dummy5[3];			// 0x2d, 0x2e, 0x2f
	uchar ep3_a_control;		// 0x30
	uchar ep3_a_base_address;	// 0x31
	uchar ep3_a_length;			// 0x32
	uchar ep3_a_packet_status;	// 0x33
	uchar ep3_a_transfer_count; // 0x34
	uchar sl811_dma_count_lo;	// 0x35
	uchar sl811_dma_count_hi;	// 0x36
	uchar dummy6;				// 0x37
	uchar ep3_b_control;	    // 0x38
	uchar ep3_b_base_address;	// 0x39
	uchar ep3_b_length;			// 0x3a
	uchar ep3_b_packet_status;	// 0x3b
	uchar ep3_b_transfer_count; // 0x3c
	uchar dummy7[195];			// 0x3d - 0xff
};					// sizeof should be 256 bytes

extern REG_BLOCK global_reg_dump[64]; // allocate 16K for debug dump
extern uchar global_reg_dump_last_index; // set to last addressable
extern uchar global_reg_dump_current_index; // set to first blank index
extern uchar global_message[65535];			// dump for text message

void printf_global_buffer (void);
void printf_global_messages (void);


//PIT header
//pit.h
// 09/23/05

#define PRE_SCALE_1 0x0000
#define PRE_SCALE_2 0x0001
#define STOP_DOZE_HALTED  0x0060
#define CLEAR_PIF  0x0004
#define ROLL_OVER 0x0002
#define ENABLE_PIT 0x0001
#define DISABLE_PIT 0x0
#define OVER_WRITE 0x0010
#define INT_ENABLE 0X0008



void delay_msec(short nmsec);
void reset_timed_interrupt(void);
void initialize_interrupts(void);
void reset_sec_counter(void);
void disable_all_interrupts(void);



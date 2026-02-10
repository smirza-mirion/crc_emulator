/**
 * \file
 * \details This file contains functions, which calls the Coldfire Programmable Interrupt Timer Module.
 */
/******************************************************************

  MODULE:  PIT Routines for CRC-15R, PET

  FILE: PitRoutines_R.c

  DATE: 05/03/06

  This module contains all routines using the Programmable Interrupt Timers

  PIT0: provides delay. routine delay_msec.  routine is sent
      the number of msec to delay

  PIT2:  provides 1msec interrupt

  *******************************************************************/
#include "crc.h"
#include "coldfire.h"
#include "pit.h"
#include "cs.h"
#include "keyboard.h"
#include "uart.h"
#include "qspi.h"
#include "i2c.h"
#include "sl811.h"
#include "sl811s.h"
#include "screen.h"
#include "amulet.h"
#include "lowlevelsnoop.h"
#include "sl811h_int.h"
#include <string.h>

#define USB_NAK_DELAY_TIMEOUT_UPPER_LIMIT	1000  // 1000 * 1 millisec = 1000ms = 1 sec,
											     // 1ms + 2ms ... 1000ms = 1000 * ((1000 + 1) / 2) = 500500ms = 500.5 sec = 8.34min
												 // Reaches 1 sec after 8.34 minutes

extern volatile unsigned long int g_ulTickCounter;
extern short max_chambers;
extern CHAMBER chamber[];
extern MEASUREMENT measurement[];

extern volatile unsigned int m_uiNAKReplyDelay;
extern volatile unsigned long m_ulNAKReplyDelayCountMilliSecs;
extern volatile unsigned long m_ulNAKReplyDelayThresholdMilliSecs;
extern volatile unsigned long m_ulConsecutiveNAKS;
extern volatile unsigned long m_ulConsecutiveNAKSPrevious;
extern volatile unsigned long m_ulConsecutiveNAKSCentiSecs;

extern volatile uchar m_ucEnumerated;

static void service_screen_saver(void);
void USBTimedPoll (void);
void service_remote_request(void);
__interrupt void amulet_interrupt(void);
void USBClearNAKCounters(void);
void USBUsbAReplyToNAK(void);
volatile unsigned long int g_csec_tstamp = 0; // centisec timer rollover in 1.36 years
static volatile unsigned long int msec_tstamp = 0; // millisec timer rollover in 1193 hrs

extern CURRENT current;
/**
 * \details Timer driven delay loop in milliseconds
 * \param nmsec Number of milliseconds
 * \returns None
 */
    void delay_msec(short nmsec)
    {
        int i;
        ushort mod;


        mod = 31999;

        //disable
        cf.pit[0].pcsr = 0;

        cf.pit[0].pcsr = PRE_SCALE_1 |
                          ROLL_OVER |
                          CLEAR_PIF;
        cf.pit[0].pmr = mod;
        cf.pit[0].pcsr |= ENABLE_PIT;

        for(i = 0; i < nmsec; i++)
        {

            //service watchdog
            service_watchdog();

            //wait for PIF to be set
            while(((cf.pit[0].pcsr) & 0x0004) == 0)
                ;

            //clear PPIF
           cf.pit[0].pcsr |= CLEAR_PIF;

        }
        //disable
        cf.pit[0].pcsr = 0;


    }

/**
 * \details Timer driven delay loop in microseconds
 * \param nusec Number of microseconds
 * \returns None
 */
    void delay_micro_sec(short nusec)
    {
        int i;
        ushort mod;


        mod = 33;

        //disable
        cf.pit[0].pcsr = 0;

        cf.pit[0].pcsr = PRE_SCALE_1 |
                          ROLL_OVER |
                          CLEAR_PIF;
        cf.pit[0].pmr = mod;
        cf.pit[0].pcsr |= ENABLE_PIT;

        for(i = 0; i < nusec; i++)
        {
            //wait for PIF to be set
            while(((cf.pit[0].pcsr) & 0x0004) == 0)
                ;

            //clear PPIF
           cf.pit[0].pcsr |= CLEAR_PIF;

        }
        //disable
        cf.pit[0].pcsr = 0;


    }


//==================================================================================================
    //Timed interrupt routines
    //timer interrupt is set for 1ms

    static ushort minute_counter = 0;
    static bool display_time = TRUE;
    extern short tone_duration;

//values to generate interrupt every 10ms
#define PRE     0x3         //pre-scalar
//1 msec tick
#define CNTR 3999       // 1ms with 64MHz
//values to generate interrupt every 1 sec
#define PRE1     10         //pre-scalar
//1 sec tick
#define CNTR1 31249         // 1 sec with 64MHz
/**
 * \details Reset the Coldfire Programmable Interrupt Timer Registers
 * \returns None
 */
    void reset_timed_interrupt(void)
    {

        cf.pit[2].pcsr = 0x000f | (PRE << 8);
    }

    extern ushort adc_tick_time;
    static short tick_counter = 0;
    static short sec_counter;



    //reset second counter
/**
 * \details Reset the sec_count variable
 * \returns None
 */
    void reset_sec_counter(void)
    {
        sec_counter = 0;
    }

    //send sec_counter to other routine
/**
 * \details Get the sec_count value
 * \returns Seconds count
 */
    short get_sec_counter(void)
    {

        return(sec_counter);
    }


    extern time_t clock_time;
    //timed interrupt processing routine
/**
 * \details Timed interrupt routine called every millisecond. This routine 1) Updates various timer counters, 2) Service USB Host routine, 3) Service Chamber routine, 4) Screen saver routine
 * \returns None
 */
    __interrupt void timed_interrupt(void)
    {
        bool screen_flag = FALSE;

		static short int iSecondCounter = 0;
		short i;
		//struct tm now;

        //reset interrupt request
        cf.pit[2].pcsr = 0x000f | (PRE << 8);

        msec_tstamp++;

        //if 1 minute up, set display time flag
        ++minute_counter;
        //if(minute_counter == 1000)
        if(minute_counter == 60000)
        {
            //gmtime_r(&clock_time, &now);
            //set_clock(&now);

            reset_minute_counter();
            screen_flag = TRUE;
        }
        //adc gets serviced every msec, others every ten msec

		++iSecondCounter;
		if (iSecondCounter > 999){
			low_clock_time++;
			iSecondCounter = 0;
		}

        if(++tick_counter == 10)
        {
            //service tone -- decrement duration and turn off tone if done
            if(tone_duration != 0)
            {
                --tone_duration;
                if(tone_duration == 0)
                    stop_tone();
            }


            //check for character from PC
            // TEMPDEV
            (void)uart_char_waiting(U_PC);

            //service keyboard
            scan_keyboard();

            //service USB printer
            USBTimedPoll();

            if(m_ucEnumerated == 1){
				if(m_uiNAKReplyDelay == NAK_REPLY_INSIDE_INTERRUPT){
					if(m_ulConsecutiveNAKS != m_ulConsecutiveNAKSPrevious){
						m_ulConsecutiveNAKSPrevious = m_ulConsecutiveNAKS;
						m_ulConsecutiveNAKSCentiSecs++;
					}else{
						USBClearNAKCounters();
					}
				}
            }

            g_csec_tstamp++;

            tick_counter = 0;
        }

        if(m_ucEnumerated == 1){
			if(m_uiNAKReplyDelay == NAK_DELAY_REPLY_INSIDE_PIT){
				m_ulNAKReplyDelayCountMilliSecs++;

				if((m_ulNAKReplyDelayCountMilliSecs > USB_NAK_DELAY_TIMEOUT_UPPER_LIMIT) || (m_ulNAKReplyDelayCountMilliSecs > m_ulNAKReplyDelayThresholdMilliSecs)){
					m_uiNAKReplyDelay = NAK_REPLY_SENT_FROM_PIT;
					m_ulNAKReplyDelayCountMilliSecs = 0;
					m_ulNAKReplyDelayThresholdMilliSecs++;
					if(m_ulNAKReplyDelayThresholdMilliSecs > USB_NAK_DELAY_TIMEOUT_UPPER_LIMIT) m_ulNAKReplyDelayThresholdMilliSecs = USB_NAK_DELAY_TIMEOUT_UPPER_LIMIT;
					m_ulConsecutiveNAKS = 0;
					m_ulConsecutiveNAKSPrevious = 0;
					m_ulConsecutiveNAKSCentiSecs = 0;
					USBUsbAReplyToNAK();
				}
			}
        }

        //just used for USB timeout
        if(++sec_counter == 1000)
        {
            sec_counter = 0;
        }


        g_ulTickCounter++;
        if(g_ulTickCounter > 0xFFFFE000) {
        	g_ulTickCounter-= 0xFFFF0000;
        	for(i=0; i<max_chambers; i++){
        		if(chamber[i].connected_flag){
        			measurement[i].ulTickAlarm -= 0xFFFF0000;
        		}
        	}
        }
        if(get_adc_enabled())
        {
            if (adc_tick_time > 0){
                adc_tick_time--;
            }else{
            	if(!spiGetSemaphore()){
            		spiSemaphoreLoadValues(SPI_SEMA_ADC);
            		service_adc(msec_tstamp);
            	}
#ifdef PRINTF_ERRORS
            	else{
            		if(spiGetSemaphore() != SPI_SEMA_SDCARD){
            			adc_tick_time = READ_CHAMBERS_IN_MSEC;
            			printf("Locked: %d\n", spiGetSemaphore());
            		}
            	}
#endif
            }
        }

        //if 1 minute up service screen saver
        if(screen_flag)
            service_screen_saver();
    }
/**
 * \details Second interrupt routine called every second. This routine updates the current time
 * \returns None
 */
    __interrupt void sec_interrupt(void)
        {
            //reset interrupt request
            cf.pit[1].pcsr = 0x000f | (PRE1 << 8);

    		clock_time++;
    		display_time = TRUE;
        }
/**
 * \details Reset the minute counter
 * \returns None
 */
    void reset_minute_counter(void)
    {
            minute_counter = 0;
    }
/**
 * \details Reset the minute counter but preserve the second count
 * \returns None
 */
    void reset_minute_counter_with_seconds(void){
    	struct tm now;

    	gmtime_r(&clock_time, &now);
    	minute_counter = 1000 * now.tm_sec;
    	minute_counter -= 1000;
    	if(minute_counter >= 60000) minute_counter = 0;
    }
/**
 * \details Get the value of minute_counter
 * \returns Minute counter value
 */
    ushort get_minute_counter(void)
    {
        return (minute_counter);

    }
/**
 * \details Get value of display_time
 * \returns Display time value
 */
    bool display_time_set(void)
    {
        return(display_time);
    }
/**
 * \details Set display_time to false
 * \returns None
 */
    void clear_display_time(void)
    {

        display_time = FALSE;
    }
/**
 * \details Set display_time to true
 * \returns None
 */
    void set_display_time(void)
    {
        display_time = TRUE;
    }





    __interrupt void irq5_interrupt(void)
    {
        if(battery_low()){
            display_text(0,50,"]",0,BIG,NORMAL);   //] = low battery symbol
            lowBattery1 = TRUE;
            lowBattery2 = TRUE;
            lowBattery3 = TRUE;
        }
        else
        {
            if(usb_pc_connected())
            {
                if((current.pccomm != PC_COMM_NONE) && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER) && (current.pccomm != PC_COMM_USB_DEBUG_WELL) && (current.pccomm != PC_COMM_USB_UPDATE) && (current.pccomm != PC_COMM_USB_UPLOAD_FILE))
                    current.pccomm = PC_COMM_USB;
            }
            else
            {
                if((current.pccomm != PC_COMM_NONE) && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER) && (current.pccomm != PC_COMM_USB_DEBUG_WELL) && (current.pccomm != PC_COMM_USB_UPDATE) && (current.pccomm != PC_COMM_USB_UPLOAD_FILE))
                    current.pccomm = PC_COMM_RS232;
            }
            lowBattery1 = FALSE;
	    lowBattery2 = FALSE;
	    lowBattery3 = FALSE;
        }
    }

    __interrupt void usb_interrupt(void)
    {
        unsigned long int interrupt_state_low;
        unsigned long int interrupt_state_high;
        interrupt_state_low = cf.intc[0].imrl;
        interrupt_state_high = cf.intc[0].imrh;
        cf.intc[0].imrl |= 0x00000001;
        isr_811();
        cf.intc[0].imrl &= interrupt_state_low;
        cf.intc[0].imrh &= interrupt_state_high;
    }


    //Initialization of Interrupts
#define INT_NUM_PIT2 57
#define INT_NUM_IRQ5 5
#define INT_NUM_IRQ6 6
#define INT_NUM_IRQ7 7
#define INT_NUM_UART1 14
#define INT_NUM_UART2 15

/*  fixed level and priority
    interrupt   level  priority    function
    5            5       mid          expander
    7            7       mid          usb PC
*/

/**
 * \details Initialize Timer, UART, USB interrupt registers in the Coldfire processor
 * \returns None
 */
    void initialize_interrupts(void)
    {
        char int_level = 3;         //PIT2 interrupt level
        char int_priority = 3;      //PIT2 priority level
        //char uart1_int_level = 5;	//UART1 interrupt level
        //char uart1_int_priority = 5; //UART1 priority level
        char uart2_int_level = 4;
        char uart2_int_priority = 4;

        //configure PIT to generate interrupt every 1ms
        //period = (2^(PRE + 1) * (CNTR + 1))/64000000
        cf.pit[2].pcsr = OVER_WRITE |
                         CLEAR_PIF |
                         ROLL_OVER |
                         (PRE << 8);
        cf.pit[2].pmr = CNTR;
        //enable interupt & disable OVER WRITE
        cf.pit[2].pcsr = CLEAR_PIF |
                         ROLL_OVER |
                         INT_ENABLE |
                         (PRE << 8);

        //enable interrupt IRQ5 & IRQ6
        cf.intc[0].imrl &= 0xffffff9f;

        //enable interrupt from PIT2 -- interrupt #57
        cf.intc[0].imrh &= 0xfdffffff;

        //set interrupt level and priority
        cf.intc[0].icrn[INT_NUM_PIT2] = (int_level << 3) | int_priority;
        //cf.intc[0].icrn[INT_NUM_UART1] = (uart1_int_level << 3) | uart1_int_priority;
        cf.intc[0].icrn[INT_NUM_UART2] = (uart2_int_level << 3) | uart2_int_priority;


        //set address for interrupt to interrupt routine
        vector_base.table[64 + INT_NUM_PIT2] = (long)&timed_interrupt;
        vector_base.table[64 + INT_NUM_IRQ5] = (long)&irq5_interrupt;
        vector_base.table[64 + INT_NUM_IRQ7] = (long)&usb_interrupt;
        //vector_base.table[64 + INT_NUM_UART1] = (long)&uart1_interrupt;
        vector_base.table[64 + INT_NUM_UART2] = (long)&amulet_interrupt;

        //enable PIT counting
        cf.pit[2].pcsr |= ENABLE_PIT;

        // Enable GPIO to hook UART1 to IO pin
        cf.gpio.puapar |= 0x0c;

        // Enable GPIO to hook UART2 to IO pin
        cf.gpio.paspar &= 0xffaf;
        cf.gpio.paspar |= 0x00a0;

        //Enable UART for interrupt
        // Reset TX
        cf.uarts[1].ucr = UART_UCR_RESET_TX;
        cf.uarts[2].ucr = UART_UCR_RESET_TX;

        // Reset RX
        cf.uarts[1].ucr = UART_UCR_RESET_RX;
        cf.uarts[2].ucr = UART_UCR_RESET_RX;

        // Disable Interrupts
        cf.uarts[1].ir = 0;
        cf.uarts[2].ir = 0;

        // Set input enable control (OFF)
        cf.uarts[1].iacr = 0;
        cf.uarts[2].iacr = 0;

        // Set Clock Source (Internal)
        cf.uarts[1].usr = UART_UCSR_TIMER;
        cf.uarts[2].usr = UART_UCSR_TIMER;

        // Set MSB for Clock Divider
        cf.uarts[1].dur = 0x00;
        cf.uarts[2].dur = 0x00;

        // Set LSB for Clock Divider
        //cf.uarts[1].dlr = 209; //9600
        cf.uarts[1].dlr = 17; //115200
        cf.uarts[2].dlr = 17; //115200

        // Reset Mode Register Pointer
        cf.uarts[1].ucr = UART_UCR_RESET_MR;
        cf.uarts[2].ucr = UART_UCR_RESET_MR;

        // Set UMR1
        cf.uarts[1].umr = 0x13;
        cf.uarts[2].umr = 0x13;

        // Set UMR2
        cf.uarts[1].umr = UART_UMR2_STOP_BITS_1;
        cf.uarts[2].umr = UART_UMR2_STOP_BITS_1;

        // Enable Tx and Rx
		cf.uarts[1].ucr = UART_UCR_TX_ENABLED;
		cf.uarts[1].ucr = UART_UCR_RX_ENABLED;
		cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
		cf.uarts[2].ucr = UART_UCR_RX_ENABLED;

		ClearUart1Rx();
		ClearUart1Tx();
		ClearUart2Rx();
		ClearUart2Tx();
		ClearMasterMessage();
		ClearSlaveMessage();
		ClearCmdQueue();

        // Enable Rx/Tx Interrupt
        //cf.uarts[1].ir = 3;
        cf.uarts[2].ir = 3;

        //enable all interrupts to be used
        // TEMPDEV
        //cf.intc[0].imrl &= 0xffffff5e;
		//cf.intc[0].imrl &= 0xffffbf5e;
		//cf.intc[0].imrl &= 0xffff3f5e;
        cf.intc[0].imrl &= 0xffff7f5e;
    }
/**
 * \details Disable all interrupts. The process will not respond to the interrupt controller. Any pending interrupts will be left on the Interrupt Controller and will be serviced when interrupt processing on the processor is enabled.
 * \returns None
 */
    void disable_all_interrupts(void)
    {
    	__DI();
        cf.intc[0].imrh = 0xffffffff;
        cf.intc[0].imrl = 0xffffffff;
        cf.intc[1].imrh = 0xffffffff;
        cf.intc[1].imrl = 0xffffffff;
        __EI();
    }


static short sscount;
static bool screen_on;
static short maxscr;    //# of minutes to keep screen on without key press
                        //if 0, always on

    //initialize screen saver
/**
 * \details Read the Screen Saver Brightness setting from the EEPROM and set the state variable to NOT_SLEEP
 * \returns None
 */
    void init_screen_saver(void)
    {
        sscount = 0;
        screen_on = TRUE;
        //get maxscr from EEPROM
        EE_READ(maxscr,(uchar *)&maxscr);
        //turn screen on
        set_backlight(TRUE);

    }


    //this is called by interrupt service routine every minute
/**
 * \details Check if screen should go into sleep, out of sleep or do nothing
 * \returns None
 */
    static void service_screen_saver(void)
    {
        if(!screen_on)
            return;

        //if maxscr = 0, always on
        if(maxscr == 0)
            return;

        //increment screen saver count
        ++sscount;
        //if no key for max time, turn screen off
        if (sscount == maxscr)
        {
            screen_on = FALSE;
            set_backlight(FALSE);

        }
    }


    //this is called after keyboard read
    //sscount reset to 0; if screen was off, turned on
    void key_press_screen_save(void)
    {
        if(maxscr == 0)
            return;

        if (!screen_on)
        {
            screen_on = TRUE;
            set_backlight(TRUE);
        }

        //reset screen saver count
        sscount = 0;
    }


/**
 * \details Timer driver delay loop in 10 millisecond units
 * \param csec 10 millisecond count
 * \returns None
 */
void delayloop(unsigned long int csec){
	unsigned long int wait;

	wait = g_csec_tstamp + csec;
	while(wait>g_csec_tstamp){
		service_watchdog();
		if(get_adc_enabled())
        {
            get_measurements();
            service_remote_request();
        }
		service_pc();
	}
}
/**
 * \details Clear the 10 millisecond counter
 * \returns None
 */
void clear_milli_sec_counter(void){
	msec_tstamp = 0;
}

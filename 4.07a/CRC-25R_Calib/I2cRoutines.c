/**
 * \file
 * \details This file contains functions, which calls the Coldfire I2C Module
 */
  /**********************************************************************
  MODULE: I2C Routines

  FILE: I2cRoutines.c

  DATE: 04/25/06

  This module contains the routines needed to read and write to I2C devices

  *******************************************************************************/

#include "crc.h"
#include "coldfire.h"         
#include "i2c.h"
#include "keyboard.h"

extern unsigned char m_ucVolumeCurrent;

static bool i2c_rx(uchar *data);
static bool i2c_tx(uchar data);
static bool wait_while_busy(void);
static bool wait_till_busy(void);
static bool wait_till_ifbit_set(void);
static void check_arbitration_lost(void);
static void enable_for_tx(void);
static void generate_stop_signal(void);

static bool i2c_in_use = FALSE;

    //write data to the I2C
/**
 * \details Write data to the I2C
 * \param addr I2C device address
 * \param data Pointer to data
 * \param len Data length
 * \param stop_flag True = Generate stop signal at the end, False = No stop signal at the end
 * \returns True = Success, False = Error
 */
    bool i2c_write(int addr, uchar *data, int len, bool stop_flag)
    {
        
        i2c_in_use = TRUE;
        
        check_arbitration_lost();	
    
        if(!wait_while_busy())
        {
            i2c_in_use = FALSE;
            return FALSE;
        }    
            
        enable_for_tx();
        
        if(!i2c_tx(addr))			// Output Address (R/W = 0) 
        {
            i2c_in_use = FALSE;
            return FALSE;	
                // No ack - fail 
        }		
    
        //send each character
        while(len--)
        {
            if(!i2c_tx(*data++))
            {    
                i2c_in_use = FALSE;
                return FALSE;
            }    
        }
        
        if(stop_flag)
            generate_stop_signal();
        
        i2c_in_use = FALSE;
        return TRUE;
    }
    
    
    
    //read data from the I2C
/**
 * \details Read data from the I2C
 * \param addr I2C device address
 * \param data Pointer to byte array, which receives the data
 * \param len0 Data length
 * \param restart_flag True = Generate repeat start, False = No repeat start
 * \returns True = Success, False = Error
 */
    bool i2c_read(int addr, uchar *data, int len0,bool restart_flag)
    {
    
        int len = len0 + 1;
        uchar rdata;

        i2c_in_use = TRUE;
        
    
        if(!restart_flag)
        {    
            check_arbitration_lost();
        
            if(!wait_while_busy())
            {    
                i2c_in_use = FALSE;
                return FALSE;				// Timed out
            }   
        }
        
        cf.i2c.cr = 0xb0;
        if(restart_flag)
            cf.i2c.cr |= 0x04;          //generate repeat start
            
        if(!wait_till_busy())
        {    
            i2c_in_use = FALSE;
            return FALSE;				// Timed out
        }
        
        cf.i2c.sr = 0;	
        
        if(!i2c_tx(addr|1))			// Output Address (R/W = 1)
        {    
            i2c_in_use = FALSE;
            return FALSE;			// No ack - fail 
        }
        
        if (!wait_till_busy())
        {    
            i2c_in_use = FALSE;
            return FALSE;
        }    
            
        cf.i2c.cr &= 0xEF;     //set receive mode
        
        *data = cf.i2c.dr;     //dummy read to start transfer
        
        //read in each character
        while(len--)
        {
            if(len == 1)			// 2nd to last byte? 
                cf.i2c.cr |= 0x08;  //disable ack
            if(len == 0)
            {

                //wait till last byte read
                if(!wait_till_ifbit_set())
                    return FALSE;
                
                generate_stop_signal();
                i2c_in_use = FALSE;
                return TRUE;
            }
            if(!i2c_rx(&rdata))
            {    
                i2c_in_use = FALSE;
                return FALSE;
            }
            *data++ = rdata;    
        }
    
        i2c_in_use = FALSE;
        return TRUE;
    }
    
    //receive 1 character
/**
 * \details Receive 1 character from I2C device
 * \param data Pointer to byte, which receives the result
 * \returns True = Success, False = Error
 */
    static bool i2c_rx(uchar *data)
    {
        //service watchdog
        service_watchdog();

        //wait till IF bit set & then clear it
        if(!wait_till_ifbit_set())
            return FALSE;
    
        //read the data
        *data = cf.i2c.dr;
        return TRUE;
    
    }
/**
 * \details Initialize the Coldfire I2C registers
 * \returns None
 */
void initSetI2C(void){
	//clear status register
	cf.i2c.sr = 0x00;

	//set frequecy divider to SYSCLCK / 640 = 100kHz
	cf.i2c.fdr = 0x15;

	cf.i2c.cr = 0x00;	// Reset the i2c

	enable_for_tx();
	i2c_tx(0);   //output address (R/W = 0)

	generate_stop_signal();

	//reset the i2c
	cf.i2c.cr = 0x00;

	//enable i2c register
	cf.i2c.cr = 0x80;
}
    
    //initialize the I2C
    void initialize_i2c(void)
    {
        //setup gpio for i2c
        cf.gpio.paspar |= 0x0f;
        
        //clear status register
        cf.i2c.sr = 0x00;
        
        //set frequecy divider to SYSCLCK / 640 = 100kHz
        cf.i2c.fdr = 0x15;
    
        cf.i2c.cr = 0x00;	// Reset the i2c

        enable_for_tx();
        i2c_tx(0);   //output address (R/W = 0)
        
        generate_stop_signal();
    
        //reset the i2c
        cf.i2c.cr = 0x00;
        //enable i2c register
        cf.i2c.cr = 0x80;
    
    }

    
    //setup the I2C
/**
 * \details Reset the I2C Coldfire module
 * \returns None
 */
    void setup_i2c(void)
    {
    
        //reset the i2c
        cf.i2c.cr = 0x00;
        //enable i2c register
        cf.i2c.cr = 0x80;
    
    }
    
    //transmit 1 character to the I2C
/**
 * \details Send 1 character to I2C device
 * \param data Value to send
 * \returns True = Successful, False = Error
 */
static bool i2c_tx(uchar data){
	//service watchdog
	service_watchdog();
        
	//send the character
	cf.i2c.dr = data;
        
	if(!wait_till_busy()) return FALSE;
    
        //wait till IF bit set & then clear it
        if(!wait_till_ifbit_set())
                return FALSE;
    
    
        //Ack received? 
        if(!(cf.i2c.sr & 0x01))
            return TRUE;
        
        return FALSE;
    }
    
    //wait until I2C is busy, return FALSE if timed out
/**
 * \details Wait until I2C is busy, return FALSE if timed out
 * \returns True = Successful, False = Timed out
 */
static bool wait_till_busy(void){
	int n= 100000;

	//wait till busy
	while(n-- && !(cf.i2c.sr & 0x20));

	if(!n) return FALSE;				// Timed out
	return TRUE;
}
        
    //wait till IF bit set & then clear it
/**
 * \details Wait till IF bit set & then clear it
 * \returns None
 */
    static bool wait_till_ifbit_set(void)
    {
        int n = 100000;
    
        //wait till IF set
        while(n-- && !(cf.i2c.sr & 0x02))  
            ;
        //timeout?
        if(!n)
            return FALSE;
            
        //clear IF bit by turning off bit 1
        cf.i2c.sr &= 0xfd;            
        
        return TRUE;
    
    
    }
    
    //wait while I2C is busy -- return FALSE if timed out
/**
 * \details Wait while I2C is busy -- return FALSE if timed out
 * \returns True = Success, False = Timed out
 */
    static bool wait_while_busy(void)
    {
        int n= 100000;
    
        while(n-- && (cf.i2c.sr & 0x20));
        if(!n)
            return FALSE;			
    
        return (TRUE);
    }
    
    //check if arbitration lost & clear condition if so
/**
 * \details Check if arbitration lost & clear condition if so
 * \returns None
 */
    static void check_arbitration_lost(void)
    {
        
        if(cf.i2c.sr & 0x10)  
        {
           cf.i2c.sr &= 0xef;  
        }
    
    
    }
    
//enable for Tx and set the start enable
/**
 * \details Enable for Tx and set the start enable
 * \returns None
 */
static void enable_for_tx(void){
	cf.i2c.cr = 0x90;       //enable the i2c for Tx
	cf.i2c.cr |= 0x20;     //set the start enable
}
    
//generate the stop signal by turning off Bit 5
/**
 * \details Generate the stop signal by turning off Bit 5
 * \returns None
 */
static void generate_stop_signal(void) {
	cf.i2c.cr &= 0xdf;     //generate stop signal
}
    
/**
 * \details Get the I2C in use status
 * \returns True = I2C in use, False = I2C not in use
 */
    bool get_i2c_in_use(void)
    {
        return(i2c_in_use);

    }

/**
 * \details Set Volume by setting the I2C DAC
 * \returns None
 */
void SetVolume(unsigned char ucValue){
	unsigned char aucI2cVol[2];

	aucI2cVol[0] = 0x00;
	aucI2cVol[1] = ucValue;
	i2c_write(0x58, aucI2cVol, 2, TRUE);
	m_ucVolumeCurrent = ucValue;
}

void ReadSN(char *sn){
	char s0[6];
	char s1[4];
	int i;

	EE_READ(snum, (uchar *) s0);
	EE_READ(snum1, (uchar *) s1);
	sn[0] = s0[0];
	for(i=0; i<4; i++) sn[i+1] = s1[i];
	for(i=1; i<6; i++) sn[i+4] = s0[i];
}

void WriteSN(char *sn){
	char s0[6];
	char s1[4];
	int i;

	s0[0] = sn[0];
	for(i=1; i<6; i++) s0[i] = sn[i+4];
	for(i=0; i<4; i++) s1[i] = sn[i+1];
	EE_WRITE(snum, (uchar *) s0);
	EE_WRITE(snum1, (uchar *) s1);
 }

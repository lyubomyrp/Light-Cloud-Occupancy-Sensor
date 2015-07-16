/*****************************************************************************
*   File name: checked_xmit.c
*
*   Description: Checked transmission of packets
*     This file contains functions for transmitting packets to the radio module.
*     Packets are wait for a response from the radio, and retranmit if no
*     response is recieved. System voltage (Vcc) is also checked before a tranmission.
*     If the voltage falls too low, transmits will not occur, and the system
*     will wait for Vcc to climb to a safe level before resuming transmissions.
*
*   Author: Lee Thalblum
******************************************************************************/

#include  <stdbool.h>
#include "io430.h"
#include "io.h"
#include "dhm.h"
#include "timer.h"
#include "serial.h"
#include "api_pkt.h"
#include "read_digi_radio_info.h"
#define CHK_XMIT_DEF
#include "checked_xmit.h"

unsigned int radioResponseDelay = 0;            // wait for response from radio
bool radioRetryFlag = false;                    // Retry radio if no response
bool radioDelayExpiryFlag = false;              // Delay between retries has expired

extern bool noAssocFlag;
extern bool radioActiveFlag;
extern unsigned long panIDLow;
extern bool dataConsolidateFlag;
extern unsigned char channel;

// Declare functions
void chk_send_radio_api_packet_task();

/*****************************************************************************
*
* Function: chk_send_radio_api_packet
*
* Description: Send AT command API packet
*
* Input:  cmd1 - first AT command character
*         cmd2 - second AT command character
*
******************************************************************************
*/
unsigned char chk_send_radio_api_packet(unsigned char cmd1, unsigned char cmd2)
{
    unsigned char i = 0;
    bool assocFlag = false;
    unsigned char sleep = 0;
    
    radioRetryFlag = true;
    while(radioRetryFlag == true)
   {
        for(i=0; i< RX_BUF_SIZE; i++)         // Clear recieve buffer
          rx_buf[i] = 0;
        current_rx_ptr = rx_buf;              // reset receive ptr
        rx_byte_count = 0;                    // reset received byte count
        packet_rx_flag = false;               // Reset Rx flag
        
		send_radio_api_packet(cmd1, cmd2);    // Send command
       
        radioResponseDelay = 0;
        radioRetryFlag = false;             // Clear flag before check
        while(!(packet_rx_flag == true))
        {
          __delay_cycles(10000);            // Delay 1 mS @ 1 MHz
          if(++radioResponseDelay == 500)   // Retry for 1/2 second
          {
            assocFlag = true;
            radioResponseDelay = 0;         // Clear delay count
            radioRetryFlag = true;          // No response, put radio to sleep and try again
          }
        }
        // Wait 5 seconds and try again
        if(radioRetryFlag == true)
        {
          radioRetryDelay = RADIO_RETRY_DELAY;    // Wait 5 seconds
          while(radioDelayExpiryFlag == false);
          radioDelayExpiryFlag = false;
        }         
   }
   packet_rx_flag = false;                      // Reset flag
   if(assocFlag == true)
      LED_ASSOC_CHK;                            // Disable no associate LED
   return 0;
}

/*****************************************************************************
*
* Function: chk_send_radio_api_packet_param
*
* Description: Send AT command API packet with parameter
*
* Input:  cmd1 - first AT command character
*         cmd2 - second AT command character
*         param - parameter passed in packet
*
******************************************************************************
*/
unsigned char chk_send_radio_api_packet_param(unsigned char cmd1, unsigned char cmd2, unsigned char param)
{ 
//#if 0 
    unsigned char i = 0;
    bool assocFlag = false;
    unsigned char sleep = 0;
    
    radioRetryFlag = true;
    while(radioRetryFlag == true)
   {
        for(i=0; i< RX_BUF_SIZE; i++)         // Clear recieve buffer
          rx_buf[i] = 0;
        current_rx_ptr = rx_buf;              // reset receive ptr
        rx_byte_count = 0;                    // reset received byte count
        packet_rx_flag = false;                 // Reset Rx flag
        send_radio_api_packet_param(cmd1, cmd2, param);      // Get high word of address
       
        radioResponseDelay = 0;
        radioRetryFlag = false;             // Clear flag before check
        while(!(packet_rx_flag == true))
        {
          __delay_cycles(10000);            // Delay 1 mS @ 1 MHz
          if(++radioResponseDelay == 500)   // Retry for 1/2 second
          {
            assocFlag = true;
            radioResponseDelay = 0;         // Clear delay count
            radioRetryFlag = true;          // No response, put radio to sleep and try again
            __delay_cycles(350000);            // Delay 350 mS @ 1 MHz     
            SLEEP_RQ_ON;                    // Put radio to sleep 
            break;
          }
        }
        // Wait 5 seconds and try again
        if(radioRetryFlag == true)
        {
          radioRetryDelay = RADIO_RETRY_DELAY;    // Wait 5 seconds
          while(radioDelayExpiryFlag == false);
          radioDelayExpiryFlag = false;
        }
   }
    if(assocFlag == true)
      LED_ASSOC_CHK;                                   // Disable no associate LED
   return 0;
}

/*****************************************************************************
*
* Function: chk_send_radio_api_packet_param_int
*
* Description: Send AT command API packet with 16 bit parameter
*
* Input:  cmd1 - first AT command character
*         cmd2 - second AT command character
*         param - parameter passed in packet
*
******************************************************************************
*/
unsigned char chk_send_radio_api_packet_param_int(unsigned char cmd1, unsigned char cmd2, unsigned int param)
{
    unsigned char i = 0;
    bool assocFlag = false;
    unsigned char sleep = 0;
    
    radioRetryFlag = true;
    while(radioRetryFlag == true)
   {
        for(i=0; i< RX_BUF_SIZE; i++)         // Clear recieve buffer
          rx_buf[i] = 0;
        current_rx_ptr = rx_buf;              // reset receive ptr
        rx_byte_count = 0;                    // reset received byte count
        packet_rx_flag = false;                 // Reset Rx flag
        send_radio_api_packet_param_int(cmd1, cmd2, param);      // Get high word of address
            
        radioResponseDelay = 0;
        radioRetryFlag = false;             // Clear flag before check
        while(!(packet_rx_flag == true))
        {
          __delay_cycles(10000);            // Delay 1 mS @ 1 MHz
          if(++radioResponseDelay == 500)   // Retry for 1/2 second
          {
            assocFlag = true;
            radioResponseDelay = 0;         // Clear delay count
            radioRetryFlag = true;          // No response, put radio to sleep and try again
            __delay_cycles(350000);            // Delay 350 mS @ 1 MHz     
            SLEEP_RQ_ON;                    // Put radio to sleep 
            break;
          }
        }
        // Wait 5 seconds and try again
        if(radioRetryFlag == true)
        {
          radioRetryDelay = RADIO_RETRY_DELAY;    // Wait 5 seconds
          while(radioDelayExpiryFlag == false);
          radioDelayExpiryFlag = false;
        }      
   }
    if(assocFlag == true)
      LED_ASSOC_CHK;                                   // Disable no associate LED
   return 0;
}

/*****************************************************************************
*
* Function: chk_send_radio_api_packet_param_string
*
* Description: Send AT command API packet with parameter string
*
* Input:  cmd1 - first AT command character
*         cmd2 - second AT command character
*         len - length of data to be copied
*         param - pointer to parameter string
*
******************************************************************************
*/
unsigned char chk_send_radio_api_packet_param_string(unsigned char cmd1, unsigned char cmd2, unsigned char len, unsigned char *param)
{
    unsigned char i = 0;
    bool assocFlag = false;
    unsigned char sleep = 0;
    
    radioRetryFlag = true;
    while(radioRetryFlag == true)
   {       
        for(i=0; i< RX_BUF_SIZE; i++)         // Clear recieve buffer
          rx_buf[i] = 0;
        current_rx_ptr = rx_buf;              // reset receive ptr
        rx_byte_count = 0;                    // reset received byte count
        packet_rx_flag = false;                 // Reset Rx flag
        send_radio_api_packet_param_string(cmd1, cmd2, len, param);      // Send parameters
       
        radioResponseDelay = 0;
        radioRetryFlag = false;             // Clear flag before check
        while(!(packet_rx_flag == true))
        {
          __delay_cycles(10000);            // Delay 1 mS @ 1 MHz
          if(++radioResponseDelay == 500)   // Retry for 1/2 second
          {
            assocFlag = true;
            radioResponseDelay = 0;         // Clear delay count
            radioRetryFlag = true;          // No response, put radio to sleep and try again
            __delay_cycles(350000);         // Delay 350 mS @ 1 MHz     
            SLEEP_RQ_ON;                    // Put radio to sleep 
            break;
          }
        }
        // Wait 5 seconds and try again
        if(radioRetryFlag == true)
        {
          radioRetryDelay = RADIO_RETRY_DELAY;    // Wait 5 seconds
          while(radioDelayExpiryFlag == false);
          radioDelayExpiryFlag = false;
        }  
   }
    if(assocFlag == true)
      LED_ASSOC_CHK;                                   // Disable no associate LED
   return 0;
}

/*****************************************************************************
*
* Function: chk_send_radio_api_packet_data
*
* Description: Send AT command API packet with parameter
*
* Input:  rf_data - pointer to data to be copied into packet
*         len - length of data to be copied
*
******************************************************************************
*/
unsigned char chk_send_radio_api_packet_data(unsigned char *dest_addr, unsigned char *network_addr, unsigned char *rf_data, unsigned int len)
{  
    unsigned char i = 0;
    bool assocFlag = false;
    unsigned char sleep = 0;
    
    radioRetryFlag = true;
    while(radioRetryFlag == true)
   { 
        for(i=0; i< RX_BUF_SIZE; i++)         // Clear recieve buffer
          rx_buf[i] = 0;
        current_rx_ptr = rx_buf;              // reset receive ptr
        rx_byte_count = 0;                    // reset received byte count
        packet_rx_flag = false;                 // Reset Rx flag
        send_radio_api_packet_data(dest_addr, network_addr, rf_data, len);      // Transmit LCP data packet
       
        radioResponseDelay = 0;
        radioRetryFlag = false;             // Clear flag before check
        while(!(packet_rx_flag == true))
        {
          __delay_cycles(10000);            // Delay 1 mS @ 1 MHz
          if(++radioResponseDelay == 500)   // Retry for 1/2 second
          {
			assocFlag = true;
            radioResponseDelay = 0;         // Clear delay count
            radioRetryFlag = true;          // No response, put radio to sleep and try again
            if(usbState == false)           // If we're USB powered, don't put radio to sleep
            {    
               __delay_cycles(3500000);            // Delay 350 mS @ 1 MHz
              SLEEP_RQ_ON;                            // Put radio to sleep 
            }
            break;
          }
        }
        // Wait 5 seconds and try again
        if(radioRetryFlag == true)
        {
          radioRetryDelay = RADIO_RETRY_DELAY;    // Wait 5 seconds
          while(radioDelayExpiryFlag == false);
          radioDelayExpiryFlag = false;
        }
     }
            
   if(assocFlag == true)
      LED_ASSOC_CHK;                                   // Disable no associate LED
   
   return 0;
}

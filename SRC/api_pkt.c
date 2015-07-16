/*******************************************************************************
*   File name: api_pkt.c
*
*   Description: API packet generation
*     This file contains functions for generating Zigbee API interface packets
*
********************************************************************************
*/

#include <string.h>
#include  <stdbool.h>
#include "serial.h"
#include "timer.h"
#include "ctrl_pkt.h"
#define API_DEF
#include "api_pkt.h"
#include "global.h"

//unsigned char destination_addr[8] = {0};
unsigned char destination_addr[8] = {0x00, 0x13, 0xa2, 0x00, 0x40, 0xb6, 0x68, 0x3d};
unsigned char network_addr[2] = {0xFF, 0xFE};
//unsigned char network_addr[2] = {0};
unsigned char tx_buf[TX_BUF_SIZE];

/*****************************************************************************
*
* Function: send_radio_api_packet
*
* Description: Send AT command API packet
*
* Input:  cmd1 - first AT command character
*         cmd2 - second AT command character
*
******************************************************************************
*/
unsigned char send_radio_api_packet(unsigned char cmd1, unsigned char cmd2)
{
    unsigned char i, index = 0;
    unsigned char checksum = 0;
    
    tx_buf[index++] = 0x7E;             // Start delimiter
    tx_buf[index++] = 00;               // Length of packet body
    tx_buf[index++] = 0x04;
    tx_buf[index++] = 0x08;             // Frame type
    tx_buf[index++] = 0x52;             // Frame ID
    tx_buf[index++] = cmd1;             // Command name, 1st character
    tx_buf[index++] = cmd2;             // Command name, 2nd character
    
    for(i=3; i < index; i++)            // calculate checksum
      checksum += tx_buf[i];
    tx_buf[index++] = 0xFF - checksum;    // Checksum
    
    serial_0_xmit(0, index);          // Send packet
    //led_rvpkt_delay = TIMER_2S;
    return 0;
}

/*****************************************************************************
*
* Function: send_radio_api_packet_param
*
* Description: Send AT command API packet with parameter
*
* Input:  cmd1 - first AT command character
*         cmd2 - second AT command character
*         param - parameter passed in packet
*
******************************************************************************
*/
unsigned char send_radio_api_packet_param(unsigned char cmd1, unsigned char cmd2, unsigned char param)
{
    unsigned char i, index = 0;
    unsigned char checksum = 0;
    
    tx_buf[index++] = 0x7E;             // Start delimiter
    tx_buf[index++] = 00;               // Length of packet body
    tx_buf[index++] = 0x05;
    tx_buf[index++] = 0x08;             // Frame type
    tx_buf[index++] = 0x01;             // Frame ID
    tx_buf[index++] = cmd1;             // Command name, 1st character
    tx_buf[index++] = cmd2;             // Command name, 2nd character
    tx_buf[index++] = param;            // parameter
    
    for(i=3; i < index; i++)            // calculate checksum
      checksum += tx_buf[i];
    tx_buf[index++] = 0xFF - checksum;    // Checksum
    
    serial_0_xmit(0, index);          // Send packet
    //led_rvpkt_delay = TIMER_2S;
    return 0;
}

/*****************************************************************************
*
* Function: send_radio_api_packet_param_int
*
* Description: Send AT command API packet with parameter
*
* Input:  cmd1 - first AT command character
*         cmd2 - second AT command character
*         param - parameter passed in packet
*
******************************************************************************
*/
unsigned char send_radio_api_packet_param_int(unsigned char cmd1, unsigned char cmd2, unsigned int param)
{
    unsigned char i, index = 0;
    unsigned char checksum = 0;
    
    tx_buf[index++] = 0x7E;             // Start delimiter
    tx_buf[index++] = 00;               // Length of packet body
    tx_buf[index++] = 0x06;
    tx_buf[index++] = 0x08;             // Frame type
    tx_buf[index++] = 0x01;             // Frame ID
    tx_buf[index++] = cmd1;             // Command name, 1st character
    tx_buf[index++] = cmd2;             // Command name, 2nd character
    tx_buf[index++] = (char)(param >> 8); // parameter high byte
    tx_buf[index++] = (char)(param & 0x00ff);   // parameter low byte
       
    for(i=3; i < index; i++)            // calculate checksum
      checksum += tx_buf[i];
    tx_buf[index++] = 0xFF - checksum;    // Checksum
    
    serial_0_xmit(0, index);          // Send packet
    //led_rvpkt_delay = TIMER_2S;
    return 0;
}

/*****************************************************************************
*
* Function: send_radio_api_packet_param_string
*
* Description: Send AT command API packet with parameter string
*
* Input:  cmd1 - first AT command character
*         cmd2 - second AT command character
*         param - parameter passed in packet
*
******************************************************************************
*/
unsigned char send_radio_api_packet_param_string(unsigned char cmd1, unsigned char cmd2, unsigned char len, unsigned char *param)
{
    unsigned char i, index = 0;
    unsigned char checksum = 0;
    
    tx_buf[index++] = 0x7E;             // Start delimiter
    tx_buf[index++] = 00;               // Length of packet body
    tx_buf[index++] = len + 4;
    tx_buf[index++] = 0x08;             // Frame type
    tx_buf[index++] = 0x01;             // Frame ID
    tx_buf[index++] = cmd1;             // Command name, 1st character
    tx_buf[index++] = cmd2;             // Command name, 2nd character
    
    memcpy(&tx_buf[index], param, len);      // Copy data into packet
    index += len;
    
    for(i=3; i < index; i++)            // calculate checksum
      checksum += tx_buf[i];
    tx_buf[index++] = 0xFF - checksum;    // Checksum
    
    serial_0_xmit(0, index);          // Send packet
    //led_rvpkt_delay = TIMER_2S;
    return 0;
}

/*****************************************************************************
*
* Function: send_radio_api_packet_data
*
* Description: Send AT command API packet with data
*
* Input:  rf_data - pointer to data to be copied into packet
*         len - length of data to be copied
*
******************************************************************************
*/
unsigned char send_radio_api_packet_data(unsigned char *dest_addr, unsigned char *network_addr, unsigned char *rf_data, unsigned int len)
{
    unsigned char i, index = 0;
    unsigned char checksum = 0;
    
    tx_buf[index++] = 0x7E;                     // Start delimiter
    tx_buf[index++] = 00;                       // Length of packet body
    tx_buf[index++] = len + 14;
    tx_buf[index++] = 0x10;                     // Frame type
    tx_buf[index++] = 0x01;                     // Frame ID
    memcpy(&tx_buf[index], dest_addr, 8);       // Copy destination address
    index += 8;
//    memcpy(&tx_buf[index], network_addr, 2);    // Copy network address of destination
//    index += 2;
//    tx_buf[index++] = 0x68;                      // Broadcast network address
//    tx_buf[index++] = 0x17;  
    tx_buf[index++] = 0xFF;                      // Broadcast network address
    tx_buf[index++] = 0xFE;  
    tx_buf[index++] = 0;                        // Broadcast Radius (max number of hops)
    tx_buf[index++] = 0;                        // Options
    memcpy(&tx_buf[index], rf_data, len);      // Copy data into packet
    index += len;
   
    
    for(i=3; i < index; i++)                    // calculate checksum
      checksum += tx_buf[i];
    tx_buf[index++] = 0xFF - checksum;            // Checksum
    
    serial_0_xmit(0, index);                  // Send packet
    //led_rvpkt_delay = TIMER_2S;
    return 0;
}

/*****************************************************************************
*
* Function: send_radio_api_mode_cmd
*
* Description: Send AT mode command to have radio re-enter API mode
*
* Input:  None
*
******************************************************************************
*/
unsigned char send_radio_api_mode_cmd(void)
{
    unsigned char index = 0;
    
    tx_buf[index++] = '+';              // Escape sequence
    tx_buf[index++] = '+';
    tx_buf[index++] = '+';
    tx_buf[index++] = 'A';             // command characters
    tx_buf[index++] = 'P';
    tx_buf[index++] = '1';
    serial_0_xmit(0, index);                  // Send packet
    return 0;
}

/*****************************************************************************
*
* Function: Check send AT command
*
*****************************************************************************/

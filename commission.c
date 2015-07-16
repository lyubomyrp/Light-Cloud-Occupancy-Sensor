/*****************************************************************************
*   File name: commission.c
*
*   Description: Radio commission packet generation
*     This file contains functions for generating a commission packet
*
*   Author: Lee Thalblum
******************************************************************************
*/

#include <string.h>
#include  <stdbool.h>
#include "serial.h"
#include "dhm.h"
#include "api_pkt.h"

bool commission_flag = false;

extern unsigned long sourceAddressLow;
extern unsigned long sourceAddressHigh;
extern unsigned int sourceNetworkAddress;
 

/*****************************************************************************
*
* Function: send_commission_packet
*
* Description: send commission packet to radio 
*
* Input:  None
*
******************************************************************************
*/

int send_commission_packet()
{
    unsigned char i, index = 0;
    unsigned char checksum = 0;
    
    // build status packet
    tx_buf[index++] = 0x7E;                             // Start delimiter
    tx_buf[index++] =  0;                               // Length of packet body
    tx_buf[index++] = 0x20;                             // 
    tx_buf[index++] = 0x95;                              // API ID: Node Identification Indicator
    memcpy(&tx_buf[index], &sourceAddressHigh, 4);      // Copy address of sender (high )
    index += 4;
    memcpy(&tx_buf[index], &sourceAddressLow, 4);       // Copy address of sender (low )
    index += 4;
    memcpy(&tx_buf[index], &sourceNetworkAddress, 2);   // Copy network address of sender
    index += 2;
    tx_buf[index++] = 0x02;                             // Packet is broadcast packet
    tx_buf[index++] = 0xFF;                             // Network address of remote
    tx_buf[index++] = 0xFE;
    memcpy(&tx_buf[index], &sourceAddressHigh, 4);      // Copy address of remote module (high )
    index += 4;
    memcpy(&tx_buf[index], &sourceAddressLow, 4);       // Copy address of remote module (low )
    index += 4;
    tx_buf[index++] = 0x20;                             // Node Identifier
    tx_buf[index++] = 0x00;                             // Node ID terminator
    tx_buf[index++] = 0xFF;                             // Address of remote's parents (none)
    tx_buf[index++] = 0xFE;
    tx_buf[index++] = 0x02;                             // Device Type - end device (?)
    tx_buf[index++] = 0x01;                             // Source event - Node ID pushbutton event
#ifdef RADIO_DIGI    
    tx_buf[index++] = 0xC1;                             // Application profile ID (Digi)
    tx_buf[index++] = 0x05;
    tx_buf[index++] = 0x10;                             // manufacturers ID (Digi)
    tx_buf[index++] = 0x1E;
#else   
    tx_buf[index++] = 0xC1;                             // Application profile ID (Digi) **** Change for new radio!!!! *****
    tx_buf[index++] = 0x05;
    tx_buf[index++] = 0x10;                             // manufacturers ID (Digi)
    tx_buf[index++] = 0x1E;
#endif
     for(i=3; i < index; i++)                           // calculate checksum
      checksum += tx_buf[i];
    tx_buf[index++] = 0xFF - checksum;                    // Checksum
    
    // START TRANSMIT HERE !!!
//    send_radio_api_packet_data(destination_addr, network_addr, tx_buf, index);
    serial_0_xmit(0, index);                            // Send buffer
   
    return 0;
}

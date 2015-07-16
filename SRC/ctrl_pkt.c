/*******************************************************************************
*   File name: ctrl_pkt.c
*
*   Description: Controller packet generation
*     This file contains functions for generating LCP response and information
*     packets for the Controller Module
*
********************************************************************************
*/
#include <string.h>
#include <stdbool.h>
#define CTRL_PKT_DEF
#include "msp430.h"
#include "dim.h"
#include "lcp.h"
#include "lcp_constants.h"
#include "io.h"
#include "adc.h"
#include "global.h"
#include "serial.h"
#include "ctrl_pkt.h"
#include "api_pkt.h"
#include "timer.h"

// Define device identity
unsigned char hwVersion;
unsigned long sourceAddressLow;
unsigned long sourceAddressHigh;
unsigned char panID[8] = {0};
unsigned char channel = 0;
unsigned char pkt_buf[TX_BUF_SIZE];

/*******************************************************************************
*
* Function: send_status_response
*
* Description: send LCP status response packet
*
* Input:  None
*
********************************************************************************
*/

int send_status_response(void)
{ 
  char index = 0;
    
    // build status packet
    pkt_buf[index++] = LC_DEVICE_RESPONSE_PREFIX_BYTE;                          // Prefix
    pkt_buf[index++] =  0x08;                                                   // Payload Length
    pkt_buf[index++] = myDeviceType;                                            // Device code
    pkt_buf[index++] = LCP_PROTOCOL_VERSION;                                    // LCP protocol version
    pkt_buf[index++] = lcpPktID;                                                // Packet ID
    pkt_buf[index++] = LC_PACKET_CMD_STATUS;                                    // Command code
    pkt_buf[index++] = RLY_READ;                                                // Actuator Status
    pkt_buf[index++] = CurrentLevel>>8;                                         // Dimming level
    pkt_buf[index++] = CurrentLevel;
    pkt_buf[index++] = DeviceCfg->curveType;
    pkt_buf[index++] = LC_PACKET_TERMINATOR_BYTE;                               // Packet terminator
    
    send_radio_api_packet_data(destination_addr, network_addr, pkt_buf, index);
    led_rvpkt_delay = TIMER_2S;
    return 0;
}

/*******************************************************************************
*
* Function: send_identity_response
*
* Description: send LCP identity response packet
*
* Input:  None
*
********************************************************************************
*/

int send_identity_response()
{
  char index = 0;
    
    // build identity packet
    pkt_buf[index++] = LC_DEVICE_RESPONSE_PREFIX_BYTE;                          // Prefix
    pkt_buf[index++] =  0;                                                      // Payload Length
    pkt_buf[index++] = myDeviceType;                                            // Device code
    pkt_buf[index++] = LCP_PROTOCOL_VERSION;                                    // LCP protocol version
    pkt_buf[index++] = lcpPktID;                                                // Packet ID
    pkt_buf[index++] = LC_PACKET_CMD_IDENTITY;                                  // Command Type
    pkt_buf[index++] = FIRMWARE_VERSION_MAJOR;                                  // Firmware Major
    pkt_buf[index++] = FIRMWARE_VERSION_MINOR;                                  // Firmware Minor
    pkt_buf[index++] = hwVersion;                                               // Hardware version
    pkt_buf[index++] = LCP_PROTOCOL_VERSION;                                    // LCP protocol version
    pkt_buf[index++] = (unsigned char) (sourceAddressHigh >> 24);               // MAC address high
    pkt_buf[index++] = (unsigned char) ((sourceAddressHigh >> 16) & 0x00FF);
    pkt_buf[index++] = (unsigned char) ((sourceAddressHigh >> 8) & 0x00FF);
    pkt_buf[index++] = (unsigned char) (sourceAddressHigh & 0x00FF);
    pkt_buf[index++] = (unsigned char) (sourceAddressLow >> 24);                // MAC address low
    pkt_buf[index++] = (unsigned char) ((sourceAddressLow >> 16) & 0x00FF);
    pkt_buf[index++] = (unsigned char) ((sourceAddressLow >> 8) & 0x00FF);
    pkt_buf[index++] = (unsigned char) (sourceAddressLow & 0x00FF);
    memcpy(&pkt_buf[index], &panID, 8);                                         // PAN ID
    index += 8;
    pkt_buf[index++] = lcp_id_feedback;                                         // Response request
    pkt_buf[1] = index-2;                                                       // Add length
    pkt_buf[index++] = LC_PACKET_TERMINATOR_BYTE;                               // Packet terminator
    
    send_radio_api_packet_data(destination_addr, network_addr, pkt_buf, index);
    led_rvpkt_delay = TIMER_2S;
    return 0;
}

/*************** Controller Specific Response Packets *************************/

/*******************************************************************************
*
* Function: state_change_report
*
* Description: send LCP state change report packet
*
* Input:  None
*
********************************************************************************
*/

int state_change_report(void)
{ 
  char index = 0;
    
    // build state packet
    pkt_buf[index++] = LC_DEVICE_RESPONSE_PREFIX_BYTE;                          // Prefix
    pkt_buf[index++] =  0x07;                                                   // Payload Length
    pkt_buf[index++] = myDeviceType;                                            // Device code
    pkt_buf[index++] = LCP_PROTOCOL_VERSION;                                    // LCP protocol version
    pkt_buf[index++] = lcpPktID;                                                // Packet ID
    pkt_buf[index++] = LC_PACKET_CMD_STATE_CHANGE;                              // Command code
    pkt_buf[index++] = ((PrevRLY>>3)|(RLY_READ>>4));                            // Actuator Status
    pkt_buf[index++] = CurrentLevel>>8;                                         // Dimming level
    pkt_buf[index++] = CurrentLevel;
    pkt_buf[index++] = LC_PACKET_TERMINATOR_BYTE;                               // Packet terminator
    
    send_radio_api_packet_data(destination_addr, network_addr, pkt_buf, index);
    led_rvpkt_delay = TIMER_2S;
    return 0;
}

/*******************************************************************************
*
* Function: send_power_loss_detect
*
* Description: send LCP power loss detect packet
*
* Input:  None
*
********************************************************************************
*/

int send_power_loss_detect(void)
{
  char index = 0;
    
    pkt_buf[index++] = LC_DEVICE_RESPONSE_PREFIX_BYTE;                          // Prefix
    pkt_buf[index++] = 0x05;                                                   // Payload Length
    pkt_buf[index++] = myDeviceType;                                            // Device code 
    pkt_buf[index++] = LCP_PROTOCOL_VERSION;                                    // LCP protocol version
    pkt_buf[index++] = lcpPktID;                                                // Packet ID
    pkt_buf[index++] = LC_PACKET_POWER_LOSS;                                    // Command code
    pkt_buf[index++] = 0x01;                                                    // power loss signal
    pkt_buf[index++] = LC_PACKET_TERMINATOR_BYTE;                               // Packet terminator
    
    send_radio_api_packet_data(destination_addr, network_addr, pkt_buf, index);
    return 0;
}

/*******************************************************************************
*
* Function: send_power_response
*
* Description: send power response packet
*
* Input:  None
*
********************************************************************************
*/

int send_pm_result(void)
{
    unsigned char i, index = 0;
    unsigned char checksum = 0;

    pkt_buf[index++] = LC_DEVICE_RESPONSE_PREFIX_BYTE;                          // Prefix
    pkt_buf[index++] =  0x0E;                                                   // Payload Length
    pkt_buf[index++] = myDeviceType;                                            // Device code
    pkt_buf[index++] = LCP_PROTOCOL_VERSION;                                    // LCP protocol version
    pkt_buf[index++] = lcpPktID;                                                // Packet ID
    pkt_buf[index++] = LC_PACKET_CMD_POWER_REPORT;
    pkt_buf[index++] = (power_t0 >> 8 & 0xFF);
    pkt_buf[index++] = (power_t0 & 0xFF);
    pkt_buf[index++] = (power_t1 >> 8 & 0xFF);
    pkt_buf[index++] = (power_t1 & 0xFF);
    pkt_buf[index++] = (send_V >> 8 & 0xFF);
    pkt_buf[index++] = (send_V & 0xFF);
    pkt_buf[index++] = (send_I >> 8 & 0xFF);
    pkt_buf[index++] = (send_I & 0xFF);
    pkt_buf[index++] = (send_P >> 8 & 0xFF);
    pkt_buf[index++] = (send_P & 0xFF);
    pkt_buf[index++] = (send_F >> 8 & 0xFF);
    pkt_buf[index++] = (send_F & 0xFF);
    pkt_buf[index++] = (send_PF >> 8 & 0xFF);
    pkt_buf[index++] = (send_PF & 0xFF);
    pkt_buf[index++] = (send_THD >> 8 & 0xFF);
    pkt_buf[index++] = (send_THD & 0xFF);
    for(i=3; i < index; i++)            // calculate checksum
      checksum += tx_buf[i];
    tx_buf[index++] = 0xFF - checksum;  // Checksum
    
    send_radio_api_packet_data(destination_addr, network_addr, pkt_buf, index);
    led_rvpkt_delay = TIMER_2S;
    return 0;
}




#ifdef LC_OCCUPANCY

/*******************************************************************************
*
* Function: send_occupancy_event
*
* Description: send send_occupancy_event
*
* Input:  None
*
********************************************************************************
*/

int send_occupancy_event(void)
{
  char index = 0;
    
    pkt_buf[index++] = LC_DEVICE_RESPONSE_PREFIX_BYTE;                          // Prefix
    pkt_buf[index++] = 0x05;                                                    // Payload Length
    pkt_buf[index++] = myDeviceType;                                            // Device code 
    pkt_buf[index++] = LCP_PROTOCOL_VERSION;                                    // LCP protocol version
    pkt_buf[index++] = lcpPktID;                                                // Packet ID
    pkt_buf[index++] = LC_PACKET_OCCUPANCY_EVENT;                               // Command code
    pkt_buf[index++] = 0x01;                                                    // power loss signal                    //CHANGEIT 
    pkt_buf[index++] = LC_PACKET_TERMINATOR_BYTE;                               // Packet terminator
    
    send_radio_api_packet_data(destination_addr, network_addr, pkt_buf, index);
    return 0;
}

#endif
#include "msp430.h"
#include <string.h>
#include  <stdbool.h>
#include "io.h"
#include "serial.h"
#include "lcp_constants.h"
#include "api_pkt.h"
#include "ctrl_pkt.h"
#include "global.h"
#include "timer.h"

//unsigned char panID[8] = {0};
unsigned char blankPanID[8] = {0};
//unsigned char channel = 0;
unsigned char panID64[8] = {0};
unsigned char panID16[2] = {0};
unsigned char gatewayHigh[4];
unsigned char gatewayLow[4];
  

void read_digi_radio_info(void)
{
  unsigned char i;
  unsigned char *srcPtr;
  unsigned char *destPtr; 

  ResetRvBuf();
  radioResponseDelay = RADIORSPDELAY;
  send_radio_api_packet_param('A', 'P', 1);
  while((packet_rx_flag == true) || (radioResponseDelay==0));
  
  ResetRvBuf();
  radioResponseDelay = RADIORSPDELAY;
  send_radio_api_packet_param('S', 'M', 0);
  while((packet_rx_flag == true) || (radioResponseDelay==0));
  
  ResetRvBuf();
  radioResponseDelay = RADIORSPDELAY;
  send_radio_api_packet_param_int('D', 'D', 0xFF01);
  while((packet_rx_flag == true) || (radioResponseDelay==0));
  
  ResetRvBuf();
  radioResponseDelay = RADIORSPDELAY;
  send_radio_api_packet('S', 'H');
  while((packet_rx_flag == true) || (radioResponseDelay==0));
  packet_rx_flag = false;               // reset flag
  srcPtr = rx_buf+8;                   // Copy address from buffer
  destPtr = (unsigned char *)&sourceAddressHigh;
  for(i=4; i > 0; i--)
  {
    *destPtr = *srcPtr;
    srcPtr--;
    destPtr++;
  }
  
  ResetRvBuf();
  radioResponseDelay = RADIORSPDELAY;
  send_radio_api_packet('S', 'L');
  while((packet_rx_flag == true) || (radioResponseDelay==0));
  packet_rx_flag = false;               // reset flag
  srcPtr = rx_buf+8;                   // Copy address from buffer
  destPtr = (unsigned char *)&sourceAddressLow;
  for(i=4; i > 0; i--)
  {
    *destPtr = *srcPtr;
    srcPtr--;
    destPtr++;
  }
  
/*  ResetRvBuf();
  radioResponseDelay = RADIORSPDELAY;
  send_radio_api_packet_param_string('D', 'H', 4, gatewayHigh);
  while((!(packet_rx_flag == true)) || (radioResponseDelay==0));
  
  ResetRvBuf();
  send_radio_api_packet_param_string('D', 'L', 4, gatewayLow);
  while((!(packet_rx_flag == true)) || (radioResponseDelay==0));
*/  
  ResetRvBuf();
  radioResponseDelay = RADIORSPDELAY;
  send_radio_api_packet('A', 'I');
  while((packet_rx_flag == true) || (radioResponseDelay==0));
  if((rx_buf[2] == 'A') && (rx_buf[3] == 'I')){
    if((rx_buf[CMD_RSP_STATUS] == 0) && (rx_buf[CMD_RSP_DATA] == 0)){
      AssoFlag = true;
      LED_R_OFF;
      LED_G_ON;
      radioResponseDelay = RADIORSPDELAY;
      send_radio_api_packet('O', 'P');    // Get PAN ID
      while((packet_rx_flag == true) || (radioResponseDelay==0));           
      memcpy(&panID, rx_buf+5, 8);        // Copy address from buffer
    
      ResetRvBuf();
      radioResponseDelay = RADIORSPDELAY;
      send_radio_api_packet('M', 'Y');    // Get 16 bit address
      while((packet_rx_flag == true) || (radioResponseDelay==0));
      memcpy(&panID16, rx_buf+5, 2);      // Copy address from buffer
    
      ResetRvBuf();
      radioResponseDelay = RADIORSPDELAY;
      send_radio_api_packet('C', 'H');    // Read channel number
      while((packet_rx_flag == true) || (radioResponseDelay==0));
      channel = rx_buf[5];
    
    // Write PAN ID to flash (segment 1) here!!
/*  flash_info_copy(DATA_STORAGE_SEGMENT);
    info_ptr = (flash_info *)flash_image;
    memcpy(info_ptr, &panID, 8);
    flash_info_update(DATA_STORAGE_SEGMENT);
*/  
    }
  }
  else
  {
    AssoFlag = false;
    LED_G_OFF;
    led_30s_19_delay = TIMER_30S_19;
    led_1s_9s_delay = TIMER_1S;
    led_9s_1s_delay = TIMER_9S;
    ResetRvBuf();
    radioResponseDelay = RADIORSPDELAY;
    StrengthLevel = 0;
    send_radio_api_packet_param('N', 'R', 0);
    while((packet_rx_flag == true) || (radioResponseDelay==0));
  }
  
    ResetRvBuf();
    radioResponseDelay = RADIORSPDELAY;
    send_radio_api_packet('W', 'R');    // Write PAN ID to non-volatile memory
    while((packet_rx_flag == true) || (radioResponseDelay==0));
}
  
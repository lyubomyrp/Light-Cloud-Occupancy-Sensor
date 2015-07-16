
#include  <stdbool.h>
#include "msp430.h"
#include "io.h"
#include "timer.h"
#include "serial.h"
#include "LCP.h"
#include "lcp_constants.h"
#include "global.h"
#include "api_pkt.h"

bool SignalStrengthFlag = 0;
unsigned char StrengthLevel;

int SignalStrength(void)
{
  unsigned char temp;
  if(SignalStrengthFlag){
    SignalStrengthFlag = false;
    ResetRvBuf();
    send_radio_api_packet('D', 'B');    // Send received signal strength
    while(!(packet_rx_flag == true));
    packet_rx_flag == false;
    temp = rx_buf[CMD_RSP_DATA];
    if((rx_buf[CMD_RSP_STATUS] == 0) && ( temp > 0x19) && (temp < 0x59)){        // valid strength value 0x1A - 0x58
        if(temp < 0x20) StrengthLevel = 3;
        else if(temp < 0x46) StrengthLevel = 2;
        else StrengthLevel = 1;
    }
    ResetRvBuf();
    led_rvpkt_delay = TIMER_2S;
  }
  
  return 0;
}
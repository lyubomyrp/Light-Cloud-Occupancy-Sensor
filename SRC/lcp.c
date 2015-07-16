/*****************************************************************************
*   File name: lcp.c
*
*   Description: LightCloud Protocol processing
*     This file contains the code for decoding incoming LCP packets
*       and creating and transmitting outgoing LCP response packets.
*
*   Data is located in rx_buf, filled by serial recieve function
*
******************************************************************************
*/
#include  <stdbool.h>
#include "msp430.h"
#include "io.h"
#include "timer.h"
#include "dim.h"
#define LCP_DEF
#include "LCP.h"
#include "global.h"
#include "serial.h"
#include "lcp_constants.h"
#include "flash.h"

int parse_basic_lcp_packet();
int parse_command();

// Packet field vars
unsigned char lcpPktPayloadLen = 0;
unsigned char lcpPktDeviceType = 0;
unsigned char lcpPktAPI = 0;
unsigned char lcpPktID = 0;

unsigned char lcp_id_rsp = 0;
bool lcp_dim_flag = false, lcp_state_report = false;

// Dimming field vars
unsigned char DimMode=0, Direction=0, TransitionTime = 0;

/****************************************************************
*
* Function: parse_lcp_packet
*
* Description: Parse LightCloud Protocol packet
*
*
*****************************************************************/
int parse_lcp_packet()
{
    int result;
    
    result = parse_basic_lcp_packet();  // Parse completed packet
    
    if(result == EParseTestOK)          // Valid packet
    {
        result = parse_command();       // Check if it's a global command
    }

    return result;
}

/****************************************************************
*
* Function: parse_basic_lcp_packet
*
* Description: Check LightCloud Protocol basic packet format
* 
* Returns: EParseTestOK if packet is good
*          EParseFrameErr if packet foormat error detected
*
*****************************************************************/
int parse_basic_lcp_packet()
{
    // Check for non-zero payload length byte and payload length must be <= 
    // max payload length 
    if((rx_lcp_buf[LCP_OFFSET_PAYLOAD_LENGTH] == 0) ||  
                                      (rx_lcp_buf[LCP_OFFSET_PAYLOAD_LENGTH] >= LC_MAX_PACKET_SIZE))
      return EParseFrameErr;
  
    // buffer data must equal payload length byte
    if(lcp_byte_count != rx_lcp_buf[LCP_OFFSET_PAYLOAD_LENGTH])
      return EParseFrameErr;
  
    // If device type is not wildcard, then it must match our device type
    if((rx_lcp_buf[LCP_OFFSET_DEVICE_CODE] != LC_DEVICE_TYPE_WILDCARD) && 
                                      (rx_lcp_buf[LCP_OFFSET_DEVICE_CODE] != myDeviceType))
      return EParseFrameErr;
  
    // API code must match
    if(rx_lcp_buf[LCP_OFFSET_PROTOCOL_VERSION] != 0x01) 
       return EParseFrameErr;
    
    // OK, we got here, so the header must be good. Store header info
    lcpPktPayloadLen = rx_lcp_buf[LCP_OFFSET_PAYLOAD_LENGTH];
    lcpPktDeviceType = rx_lcp_buf[LCP_OFFSET_DEVICE_CODE];
    lcpPktAPI = rx_lcp_buf[LCP_OFFSET_PROTOCOL_VERSION];
    lcpPktID = rx_lcp_buf[LCP_OFFSET_PACKET_ID];
 
    return EParseTestOK;
}

/****************************************************************
*
* Function: parse_device_specific_command
*
* Description: Parse LightCloud device commands
*
*
*****************************************************************/
int parse_command()
{
  unsigned int CfgMode, LCConfigInfo;
  
  switch(rx_lcp_buf[LCP_OFFSET_COMMAND_CODE])
  { // Process Controller Module commands
    
    case LC_PACKET_CMD_STATUS:
        return(EParseGoodStatus);
        break;
    case LC_PACKET_CMD_IDENTITY:
        lcp_id_feedback = rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS];
        return(EParseGoodIdentity);
        break;
    case LC_PACKET_CMD_DEVICE_CONFIG:
          CfgMode = rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS];
          LCConfigInfo = rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS+2];
          switch(CfgMode)
          {
          case 2:
            DeviceCfg->deviceMode = LCConfigInfo;
            break;
          case 3:
            DeviceCfg->curveType = LCConfigInfo;
            break;
          case 4:
            DeviceCfg->softDimOn= LCConfigInfo;
            break;
          case 5:
            DeviceCfg->powerPktOn = LCConfigInfo;
            break;
          case 6:
            DeviceCfg->relayState = LCConfigInfo;
            break;
          }
          flash_info_update();
          break;
    case LC_PACKET_CMD_STATE_CHANGE:
          DimMode = (rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS] >> 4) & 0x0F;
          Direction = rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS] & 0x0F;
          TransitionTime = rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS+1];
          SetLevel = (rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS+2] << 8) & 0xFF00 + 
                                        rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS+3];
          lcp_dim_flag = true;
          if(DimMode==2)PWMrateFlag = true;
          PrevRLY = RLY_READ;
          break;
          
    case LC_PACKET_CMD_POWER_REPORT:
          return(EParseGoodPower);
          break;
    }
    return(EParseTestOK);
}
//#endif


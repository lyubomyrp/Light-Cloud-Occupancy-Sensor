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
int parse_global_command();

// Packet field vars
unsigned char lcpPktPayloadLen = 0;
unsigned char lcpPktDeviceType = 0;
unsigned char lcpPktAPI = 0;
unsigned char lcpPktID = 0;

unsigned int lcp_packet_count = 0;

unsigned char responseFlag = 0;
unsigned char lcp_softon_flag = 0;
bool lcp_dim_flag = false, lcp_pm_inst_flag = false, lcp_state_report = false;

// Dimming field vars
unsigned int LCConfigInfo = 0;
unsigned char ControllerType=0, CfgMode=0, DimMode=0, Direction=0, TransitionTime = 0;

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
    
    result = parse_basic_lcp_packet();          // Parse completed packet
    
    if(result == EParseTestOK)                  // Valid packet
    {
        result = parse_global_command();        // Check if it's a global command
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
* Function: parse_global_command
*
* Description: Check if this is a global command packet
*
* Returns: EParseGoodDiscovery - if Discovery packet
*          EParseGoodStatus - if Status packet
*          EParseTestOK - If packet is not a global command
*
*****************************************************************/
int parse_global_command()
{
    // Check payload length and device code for match with wildcard values
    //if ((rx_lcp_buf[LCP_OFFSET_PAYLOAD_LENGTH] == LC_WILDCARD_PAYLOAD_LEN_GENERIC_CMD) ||
    //    (rx_lcp_buf[LCP_OFFSET_DEVICE_CODE] == LC_DEVICE_TYPE_WILDCARD))
    {
      if(rx_lcp_buf[LCP_OFFSET_COMMAND_CODE] == LC_PACKET_CMD_DISCOVERY)
        return(EParseGoodDiscovery);
      
      if(rx_lcp_buf[LCP_OFFSET_COMMAND_CODE] == LC_PACKET_CMD_STATUS)
        return(EParseGoodStatus);
      
      if(rx_lcp_buf[LCP_OFFSET_COMMAND_CODE] == LC_PACKET_CMD_IDENTITY)
        return(EParseGoodIdentity);
      
    }
    return(EParseTestOK);           // Not a global command
}

//#if 0
/****************************************************************
*
* Function: parse_device_specific_command
*
* Description: Parse LightCloud device specific commands
*
*
*****************************************************************/
int parse_device_specific_command()
{
  switch(rx_lcp_buf[LCP_OFFSET_COMMAND_CODE])
    {
        // Process Controller Module commands
    case LC_PACKET_CMD_DEVICE_CONFIG:
          CfgMode = rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS];
          LCConfigInfo = rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS+2];
          switch(CfgMode)
          {
          case 2:
            ControllerType = LCConfigInfo;
            flash_image[FLASH_DEVICE_MODE] = LCConfigInfo;
            break;
          case 3:
            CurveType = LCConfigInfo;
            flash_image[FLASH_CURVE_TYPE] = LCConfigInfo;
            break;
          case 4:
            lcp_softon_flag = LCConfigInfo;
            flash_image[FLASH_SOFT_DIM_ON] = LCConfigInfo;
            break;
          }
          flash_info_update();
          break;
    case LC_PACKET_CMD_DIMMING_CONTROL:
          DimMode = (rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS] >> 4) & 0x0F;
          Direction = rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS] & 0x0F;
          TransitionTime = rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS+1];
          SetLevel = (rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS+2] << 8) & 0xFF00 + 
                                        rx_lcp_buf[LCP_OFFSET_BEGIN_ARGS+3];
          lcp_dim_flag = true;
          if(DimMode==2)PWMrateFlag = true;
          soft_on_off_flag = true;
          PrevRLY = RLY_READ;
          break;
          
    case LC_PACKET_CMD_POWER_REPORT:
          lcp_pm_inst_flag = true;
          break;
    }
    return 0;
}
//#endif


/*****************************************************************************
*   File name: lcp.h
*
*       Defines for LightCloud Protocol handling
*
*   Author: Lee Thalblum
******************************************************************************
*/

#ifdef LCP_DEF
#define LCP_EXT
#else
#define LCP_EXT extern
#endif

// LCP packet offsets
#define LCP_OFFSET_PAYLOAD_LENGTH       0
#define LCP_OFFSET_PROTOCOL_VERSION     1
#define LCP_OFFSET_DEVICE_CODE          2
#define LCP_OFFSET_PACKET_ID            3
#define LCP_OFFSET_COMMAND_CODE         4
#define LCP_OFFSET_BEGIN_ARGS           5

#define LCP_OFFSET_USER_FEEDBACK_FLAG   5

enum E_LC_PARSE_RESULTS
{
 EParseTestOK        = 0,
 EParseGoodDiscovery = 1,
 EParseGoodStatus    = 2,
 EParseGoodCommand   = 3,
 EParseGoodIdentity  = 4,
 EParseGoodPower     = 5,
 
 EParsePrefixErr     = -1,
 EParseShortErr      = -2,
 EParseFrameErr      = -3
};

LCP_EXT int parse_lcp_packet();
LCP_EXT int parse_device_specific_command();

// Packet field vars
LCP_EXT unsigned char lcpPktPayloadLen;
LCP_EXT unsigned char lcpPktDeviceType;
LCP_EXT unsigned char lcpPktAPI;
LCP_EXT unsigned char lcpPktID;

LCP_EXT unsigned int lcp_packet_count;

LCP_EXT unsigned char responseFlag;

LCP_EXT bool lcp_dim_flag, lcp_pm_inst_flag, lcp_state_report;

LCP_EXT unsigned char lcp_softon_flag, CfgMode, DimMode, Direction, TransitionTime, ControllerType;

// LCP Generic Packet Types
#define PKT_TYPE_DISCOVERY              0x01
#define PKT_TYPE_IDENTITY               0x05
#define PKT_TYPE_FW_UPDATE              0x06



/*****************************************************************************
*   File name: lcp.h
*
*       Defines for LightCloud Protocol handling
*
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
#define LCP_OFFSET_BEGIN_ARGS           5 // also LCP_OFFSET_USER_FEEDBACK_FLAG

enum E_LC_PARSE_RESULTS
{
 EParseTestOK        = 0,
 EParseGoodStatus    = 1,
 EParseGoodIdentity  = 2,
 EParseGoodPower     = 3,
 
 EParseFrameErr      = -1
};

LCP_EXT int parse_lcp_packet();

// Packet field vars
LCP_EXT unsigned char lcpPktPayloadLen;
LCP_EXT unsigned char lcpPktDeviceType;
LCP_EXT unsigned char lcpPktAPI;
LCP_EXT unsigned char lcpPktID;

LCP_EXT unsigned char lcp_id_feedback;

LCP_EXT bool lcp_dim_flag, lcp_state_report;

LCP_EXT unsigned char DimMode, Direction, TransitionTime;



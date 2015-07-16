/*****************************************************************************
*   File name: lcp_constants.h
*
*   Defines constants used in building and decoding LCP packets
*
*   Author: Lee Thalblum
******************************************************************************
*/
// Max packet size. Need to determine what the size really is
#define LC_MAX_PACKET_SIZE              50

// prefix for all commands to LightCloud devices
#define LC_DEVICE_COMMAND_PREFIX_BYTE   0xff

// prefix for all responses from LightCloud devices
#define LC_DEVICE_RESPONSE_PREFIX_BYTE  0xfe

// terminator for all protocol packets
#define LC_PACKET_TERMINATOR_BYTE       0x0d

// API version number
#define LC_DEVICE_API_VERSION           0x01

// the 'wildcard' device type
#define LC_DEVICE_TYPE_WILDCARD         0x00
#define LC_CONTROLLER_TYPE_DIMMER       0x01
#define LC_CONTROLLER_TYPE_TRIGGER      0x02
#define LC_CONTROLLER_TYPE_REPEATER     0x03

#define LC_WILDCARD_PAYLOAD_LEN_GENERIC_CMD     5

// ************* generic LightCloud command codes ************************
// reserved from 00-1f

// request discovery, status, identity, update commands
#define LC_PACKET_CMD_DISCOVERY         0x01
#define LC_PACKET_CMD_STATUS            0x02
#define LC_PACKET_CMD_IDENTITY          0x05
#define LC_PACKET_FW_UPDATE             0x06
#define LC_PACKET_POWER_LOSS            0x07

// response
#define LC_PACKET_RESPONSE_DISCOVERY            LC_PACKET_CMD_DISCOVERY
#define LC_PACKET_RESPONSE_STATUS               LC_PACKET_CMD_STATUS

// ************ Device Specific LightCloud command codes
// Controller Module
#define LC_PACKET_CMD_DIMMING_CONTROL   0x08
#define LC_PACKET_CMD_POWER_REPORT      0x09
#define LC_PACKET_CMD_DEVICE_CONFIG     0x0A
#define LC_PACKET_CMD_STATE_CHANGE      0x0B
#ifdef LC_OCCUPANCY
// Occupancy sensor Module
#define LC_PACKET_OCCUPANCY_EVENT               0x30

#endif
// response
#define LC_PACKET_RESPONSE_ACTUATOR_CONTROL     LC_PACKET_CMD_ACTUATOR_CONTROL

// ******************* device types ****************************

// Dimmer 0-10 w/MCU
#define LC_DEVICE_TYPE_CONTROLLER       0x01

// Daylight Harvester Module
#define LC_DEVICE_TYPE_DHM              0x02

// Ceiling Sensor
#define LC_DEVICE_TYPE_CSCOUT           0x11

// 'Deploy test' firmware on Deploy (Atmel 328P MCU + XBee) hardware
#define LC_DEVICE_TYPE_DEPLOY_TEST      0x80


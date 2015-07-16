
#ifdef CTRL_PKT_DEF
#define CTRL_PKT_EXT 
#else
#define CTRL_PKT_EXT extern
#endif

#define FIRMWARE_VERSION_MAJOR          0       // Major version number
#define FIRMWARE_VERSION_MINOR          1       // Minor version number

#define LCP_PROTOCOL_VERSION            1

CTRL_PKT_EXT int send_status_response(void);
CTRL_PKT_EXT int send_discovery_response(void);
CTRL_PKT_EXT int send_threshold_report(unsigned int thresh_low, unsigned int thresh_hi, unsigned int sample);
CTRL_PKT_EXT int send_light_level_report(unsigned char mode);
CTRL_PKT_EXT int send_identity_response(void);
CTRL_PKT_EXT int send_pm_result(void);
CTRL_PKT_EXT int state_change_report(void);
CTRL_PKT_EXT int send_power_loss_detect(void);
#ifdef LC_OCCUPANCY

CTRL_PKT_EXT int send_occupancy_event(void);

#endif
// Define device identity
CTRL_PKT_EXT unsigned char hwVersion;
extern unsigned long sourceAddressLow;
extern unsigned long sourceAddressHigh;
extern unsigned char panID[8];
extern unsigned char channel;

 // Definitions
#define MODE_TIMER              0x00
#define MODE_BUTTON             0x11
#define MODE_POWER_CHANGE       0x22
 
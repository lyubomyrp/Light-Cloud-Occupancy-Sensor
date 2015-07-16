/*****************************************************************************
*   File name: api_pkt.h
*
*       Defines for API packets
*
******************************************************************************
*/

#ifdef API_DEF
#define API_EXT
#else
#define API_EXT extern
#endif

API_EXT unsigned char send_radio_api_packet(unsigned char cmd1, unsigned char cmd2);
API_EXT unsigned char send_radio_api_packet_param(unsigned char cmd1, unsigned char cmd2, unsigned char param);
API_EXT unsigned char send_radio_api_packet_param_int(unsigned char cmd1, unsigned char cmd2, unsigned int param);
API_EXT unsigned char send_radio_api_packet_data(unsigned char *dest_addr, unsigned char *network_addr, unsigned char *rf_data, unsigned int len);
API_EXT unsigned char send_radio_api_packet_param_string(unsigned char cmd1, unsigned char cmd2, unsigned char len, unsigned char *param);

API_EXT unsigned char destination_addr[8];
API_EXT unsigned char network_addr[2];


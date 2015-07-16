/*****************************************************************************
*   File name: commission.h
*
*       Defines for commissioning packet
*
*   Author: Lee Thalblum
******************************************************************************
*/

#ifdef COM_DEF
#define COM_EXT
#else
#define COM_EXT extern
#endif

COM_EXT int send_commission_packet();

COM_EXT bool commission_flag;


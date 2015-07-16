/*****************************************************************************
*   
*   File name: sensor_motion.h
*
*   Description: motion and sound sensor handlers
*   This file contains the code for motion and sound sensor handlers.
*
******************************************************************************/

#ifdef SM_DEF
#define SM_EXT
#else
#define SM_EXT extern
#endif

#ifdef LC_OCCUPANCY
#include "msp430.h"

#define TIMER_01S       25
#define TIMER_025S      62
#define TIMER_05S       125
#define TIMER_1S        250
#define TIMER_2S        500
#define TIMER_3S        750
#define TIMER_4S        1000
#define TIMER_5S        1250
#define TIMER_9S        2250
#define TIMER_10S       2500
#define TIMER_30S_19    3

SM_EXT void pir_triger(void);
SM_EXT void led_green1(unsigned int ,unsigned int );                            //Green 1 led driver On_t - led on time sec, led period - period_t
void ocupansy_action_check(void);				                // check routine for ocupansy actiot run led and relay for time , send packet ...

#endif
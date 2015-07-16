/* Global variables extern */
#ifdef GLOBAL_DEF
#define GLOBAL_EXT
#else
#define GLOBAL_EXT extern
#endif

#include <stdbool.h>

extern void ResetRvBuf(void);
extern int ActuatorHardOn(void);
extern void read_digi_radio_info(void);
extern void write_InfoSeg(char x);
extern void read_digi_radio_info(void);

extern bool RecFlag, SampleReadyFlag, AssoFlag, AssoCheckFlag, WaitControllerRsp, SignalStrengthFlag;
extern unsigned char StrengthLevel;
extern unsigned int myDeviceType;

#ifdef LC_OCCUPANCY
/*******************************************************************************
                            PIR sensor
*******************************************************************************/
/*                          general device settings							  */
typedef enum{																	//device modes
  occupancy_sensor,
  no_actuator,
  vacancies
  
}controller_mode;

GLOBAL_EXT controller_mode current_controller_mode;								//current device mode

/*                          PIR   settings                                    */

#define PIR_THRESHOLD                   10000	                                // PIR sensor treshhold
#define PIR_MIN_PULSE_PERIOD            20     	   	                        	// min slope duration MS 
#define PIR_MAX_PULSE_PERIOD            2000                                    // max slope duration MS
#define TRIGER_DELAY					3				                		// dalay for ocuupansy (sec)

/*                          PIR Variables                                     */

typedef enum{                                                                   //LC Occupansy event                                 
	sensor_nill,
	pir_event,
	mic_event
} sensor_event;

/*                          LED SENSOR Variables                                     */
typedef enum {
	led_on,
	led_off,
	blink_patern_1,
	blink_patern_2,
	blink_patern_3,
} led_state;
        
/*                          PIR TIMER Variables                               */  
GLOBAL_EXT unsigned int pir_tic;												//timer trigger for PIR sensing
GLOBAL_EXT unsigned int current_triger_delay;									//delay in secconds after accupanse detected
GLOBAL_EXT unsigned int current_triger_delay_counter;							//variable used only in timer loop to count  "current_triger_delay" variable

GLOBAL_EXT bool triger_delay_time_over_report_flag;                             // triger delay time over report flag set to false when motion happen and set to true when state report is sent ;
/*                          PIR LED     riables                               */  

GLOBAL_EXT unsigned int led_green1_tic;											//timer trigger for green1
GLOBAL_EXT led_state led_current_state_G1;

GLOBAL_EXT unsigned int ipirresult;                                             //PIR sensor value from ADC
GLOBAL_EXT unsigned int PIR_min,PIR_max;  										// PIR min and max values
GLOBAL_EXT unsigned int PIR_direction;	        		        				//0 = down direction, 1 = up direction
GLOBAL_EXT unsigned int PIR_sensitivity;        								// sensitivity min = , max =
GLOBAL_EXT unsigned int PIR_minSlopeTime;                						// mS
GLOBAL_EXT unsigned int PIR_maxSlopeTime;                						// mS
GLOBAL_EXT unsigned int PIR_slopetime;  
GLOBAL_EXT unsigned int PIR_newADC;     	        							//temporerry result for
GLOBAL_EXT unsigned int PIR_diff;                 								// PIR MAX-MIN

GLOBAL_EXT sensor_event sensor_event_flag ;                                     // init sensor flag

#endif
/*******************************************************************************
                            Read me area
*******************************************************************************/


/*******************************************************************************
                                 End
*******************************************************************************/

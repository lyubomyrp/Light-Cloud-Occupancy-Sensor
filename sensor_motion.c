/*****************************************************************************
*   
*   File name: sensor_moution.c
*
*   Description: motion and sound sensor handlers
*   This file contains the code for motion and sound sensor handlers.
*
******************************************************************************/
#ifdef LC_OCCUPANCY
#include "sensor_motion.h"
#include "io.h"
#include "global.h"

//unsigned int ipirresult;                                                        //PIR sensor value from ADC
//unsigned int PIR_min,PIR_max;              					// PIR min and max values
//unsigned int PIR_direction = 0; 		        			//0 = down direction, 1 = up direction
//unsigned int PIR_sensitivity  = PIR_THRESHOLD;	        			// sensitivity min = , max =
//unsigned int PIR_minSlopeTime = PIR_MIN_PULSE_PERIOD; 	        		//100mS
//unsigned int PIR_maxSlopeTime = PIR_MAX_PULSE_PERIOD; 		        	//2000mS
//unsigned int PIR_slopetime=0;
//unsigned int PIR_newADC;     						        //temporerry result for

//unsigned int PIR_diff;                  					// PIR MAX-MIN




/**************************************************
 *	PIR sensor routine
 *************************************************/
void pir_triger(void)
{
  
  PIR_sensitivity  = PIR_THRESHOLD;	        			        				// sensitivity min = , max =
  PIR_minSlopeTime = PIR_MIN_PULSE_PERIOD; 	        		        			//100mS
  PIR_maxSlopeTime = PIR_MAX_PULSE_PERIOD; 		        	        			//2000mS

  PIR_newADC=ipirresult;                                                      	//sensor output value
	if(PIR_slopetime == 0)
	{
		PIR_min = PIR_newADC;
		PIR_max = 0;
		PIR_slopetime++;
	}
	else if(PIR_slopetime == 1)
	{
		if(PIR_newADC > PIR_min)
		{
			PIR_max = PIR_newADC;
			PIR_direction = 1;
			PIR_slopetime++;
		}
		if (PIR_newADC < PIR_min)
		{
			PIR_max = PIR_min;
			PIR_min = PIR_newADC;
			PIR_direction = 0;
			PIR_slopetime++;
		}
	}
	else if(PIR_slopetime > 1)
	{
		if(PIR_direction == 0)
		{
			if(PIR_newADC < PIR_min)
			{
				PIR_min = PIR_newADC;
				PIR_slopetime++;
			}
			else
			{
				PIR_diff = PIR_max-PIR_min;
				if((PIR_diff > PIR_sensitivity)&&((PIR_slopetime > PIR_minSlopeTime) && (PIR_slopetime < PIR_maxSlopeTime)))
				{
					sensor_event_flag = pir_event ;// PIR event happen
				} //else pirsens = false;
				PIR_max = 0;
				PIR_min = 0;
				PIR_slopetime = 0;
				PIR_direction = 0;//??
			}
		}
		if(PIR_direction == 1)
		{
			if(PIR_newADC > PIR_max)
			{
				PIR_max = PIR_newADC;
				PIR_slopetime++;
			}
			else
			{
				PIR_diff = PIR_max-PIR_min;
				if ((PIR_diff > PIR_sensitivity)&&((PIR_slopetime > PIR_minSlopeTime) && (PIR_slopetime < PIR_maxSlopeTime)))
				{
					sensor_event_flag = pir_event ;// PIR event happen
				} //else pirsens = false;
				PIR_max = 0;
				PIR_min = 0;
				PIR_slopetime = 0;
				PIR_direction = 0;
			}
		}
	}
        
}


void led_green1(unsigned int On_t,unsigned int period_t)                        //Green 1 led driver On_t - led on time sec, led period - period_t
{
	if(led_green1_tic==0) led_green1_tic =period_t;
	else if(led_green1_tic>On_t) LED_G1_OFF;
		 else LED_G1_ON;
}


void ocupansy_action_check(void)				                				// check routine for ocupansy actiot run led and relay for time , send packet ...
{
	if((sensor_event_flag == pir_event) && (current_triger_delay ==0)){			//event just happen
	current_triger_delay = TRIGER_DELAY;										//reset delay timer
	if(current_controller_mode == occupancy_sensor) RLY_ON;                     //turn on relay only for test
    #ifdef OS_TEST_MODE
    led_current_state_G1 =led_on;                                               // moution (green led ) off, for test on
   	#endif    
	#ifndef OS_TEST_MODE
	led_current_state_G1 =led_off;												// moution (green led ) off, for test on    
	#endif    
    sensor_event_flag = sensor_nill;                                            //reset motion flag
    ResetRvBuf();                                                               // clear serial receive buffer
    send_occupancy_event();                     	                            //send message event heppen
    state_change_report();														//send state
    triger_delay_time_over_report_flag=false;
  }
  if((!(sensor_event_flag == pir_event)) && (current_triger_delay ==0)){        //no event happen and time over
	if(current_controller_mode == occupancy_sensor) RLY_OFF;                    // moution (green led ) run for patern 3 
    #ifdef OS_TEST_MODE
    led_current_state_G1 =blink_patern_3;                                       // moution (green led ) off, for test on
   	#endif    
	#ifndef OS_TEST_MODE
	led_current_state_G1 =blink_patern_3;										// moution (green led ) off, for test on    
	#endif    
    sensor_event_flag = sensor_nill;                                            //reset motion flag
    if(triger_delay_time_over_report_flag==false) {
      state_change_report();
      triger_delay_time_over_report_flag=true;  
    }
  }
  if((sensor_event_flag == pir_event) && (!(current_triger_delay ==0))){        //event happen but time is not over
    current_triger_delay = TRIGER_DELAY;                                        //reset delay timer
    if(current_controller_mode == occupancy_sensor) RLY_ON;                     //keep on relay only for test
    #ifdef OS_TEST_MODE
    led_current_state_G1 =led_on;                                               // moution (green led ) off, for test on
   	#endif    
	#ifndef OS_TEST_MODE
	led_current_state_G1 =led_off;												// moution (green led ) off, for test on    
	#endif    
    sensor_event_flag = sensor_nill;                                            // do not send a message to gateway
  }
}


#endif
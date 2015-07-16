/*******************************************************************************
*
*   File name: timer.c
*
*   LC - Controller
*   Initialize timer and timer service routine
*
*******************************************************************************/
#include  <stdbool.h>
#include "msp430.h"
#include "io.h"
#include "timer.h"
#include "dim.h"
#include "global.h"

bool radioInitflag=0;
unsigned char count_100ms=0, count_8ms=0, radioInitDelay;
unsigned int radioResponseDelay = 0, AssoCounter=0, SsCounter=0;;
unsigned int led_rvpkt_delay=0, led_30s_1s_delay=0, led_1s_delay=0,led_30s_2s_delay, led_2s_delay;
unsigned int led_30s_05s_delay, led_05s_delay, led_1s_9s_delay, led_9s_1s_delay;
unsigned int power_t0, power_t1;

/*******************************************************************************
*
* Function: timer_init
*
* Description: timer Initialization
* Initialize timer period and source clock
*
*******************************************************************************/

// Timer A0 Initialization
void timer_init()
{
#ifdef LC_CONTROLLER
  P2SEL0 |= BIT6;                       // P2.6 CCRx Function
  P2DIR  |= BIT6;                       // Set P2.6 as output
    
  //TA0CTL = TASSEL_2 | MC_1 | ID_3;    // SMCLK/8 = 2.048Mhz,UpMode
  TA0CTL = TASSEL_2 | MC_2 | ID_0;      // SMCLK = 16.384Mhz,ContMode
  TA0CCTL0 = CCIE;                      // Enable CCR0 Periodic interrupt,compare mode
  TA0CCR0 = TIMER_FREQ;                 // Timer0 Base Freq = 250Hz
  TA0CCTL1 = OUTMOD_PWM;                // Output mode: reset/set
  TA0CCR1 = 0;                          // PWM duty cycle register setup
  
#elif  LC_OCCUPANCY
  P1SEL1 |= BIT5;                       // P1.5 CCRx Function
  P1DIR  |= BIT5;                       // Set P1.5 as output
  
  //TA0CTL = TASSEL_2 | MC_1 | ID_3;    // SMCLK/8 = 2.048Mhz,UpMode
  TA0CTL = TASSEL_2 | MC_2 | ID_0;      // SMCLK = 16.384Mhz,ContMode
  TA0CCTL0 = CCIE;                      // Enable CCR0 Periodic interrupt,compare mode
  TA0CCR0 = TIMER_FREQ;                 // Timer0 Base Freq = 250Hz
  TA0CCTL1 = OUTMOD_PWM;                // Output mode: reset/set
  TA0CCR1 = 0;                          // PWM duty cycle register setup
#endif

  radioInitDelay = RADIOINITDELAY;      // radio initialize delay
}

/*******************************************************************************
// Timer A0 interrupt service routine @ 250hz, 4ms
*******************************************************************************/
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void)
{
   switch(__even_in_range(TA0IV,TA0IV_TAIFG)) {
        case TA0IV_NONE:
          if(count_8ms) count_8ms--;            // main loop 8ms timer
          
          if(count_100ms)
            count_100ms--;                      // count 100ms timer
          else{
            count_100ms = 25;
            if(AssoCounter)AssoCounter--;       // ask for associate timer
            if(SsCounter)SsCounter--;           // ask for signal strength timer
            if(radioResponseDelay) radioResponseDelay--;// radio response time out        
            if(radioInitDelay) radioInitDelay--;        // radio init delay
            else radioInitflag = true;
          }
          
          // signal strength level LEDs state, default all off
          if(led_rvpkt_delay){
            switch(StrengthLevel){
              case 1:
                LED_B3_ON;
                break;
              case 2:  
                LED_B3_ON;
                LED_B2_ON;
                break;
              case 3:
                LED_B1_ON;
                LED_B2_ON;
                LED_B3_ON;
                break;
              default:
                LED_B1_OFF;
                LED_B2_OFF;
                LED_B3_OFF;
                break;
            }
            led_rvpkt_delay--;
          }
          else{
            LED_B1_OFF;
            LED_B2_OFF;
            LED_B3_OFF;
          }
          
          // identity packet w/feedback, blink white LED every 1sec for 30s
          if(led_30s_1s_delay){
            if(led_1s_delay)
              led_1s_delay--;
            else {
              led_30s_1s_delay--;
              led_1s_delay = TIMER_1S;
              LED_W_TOGGLE;
            }
          }
          else 
            led_1s_delay = 0;
          
          // identity packet w/actuator feedback, blink and toggle relay every 2 sec for 30s
          if(led_30s_2s_delay){
            if(led_2s_delay)
              led_2s_delay--;
            else {
              led_2s_delay = TIMER_2S;
              LED_W_TOGGLE;
              if(RLY_READ == 0){ // Actuator Hard ON
                if(led_30s_2s_delay == 1)TA0CCR1 = CurrentLevel;
                else TA0CCR1 = PWM_FREQ;
                RLY_ON;
              }
              else { // Actuator Hard OFF
                TA0CCR1 = 0;
                RLY_OFF;
              }
              led_30s_2s_delay--;
            }
          }
          else
            led_2s_delay = 0;
          
          // network reset flash white LED every 0.5s for 30s
          if(led_30s_05s_delay){
            if(led_05s_delay)
              led_05s_delay--;
            else {
              led_30s_05s_delay--;
              led_05s_delay = TIMER_05S;
              LED_W_TOGGLE;
            }
          }
          else
            led_05s_delay = 0;
          
          // associate red LED 1s ON 9s OFF
          if(AssoFlag==0){
            LED_G_OFF;
            if(led_1s_9s_delay){
                led_1s_9s_delay--;
                LED_R_ON;
            }
            else if(led_9s_1s_delay){
                LED_R_OFF;
                led_9s_1s_delay--;
            }
            else {
                led_1s_9s_delay = TIMER_1S;
                led_9s_1s_delay = TIMER_9S;
            }
            StrengthLevel = 0;
          }
          else{
            LED_R_OFF;
            LED_G_ON;
          }
                

#ifdef LC_OCCUPANCY
/*******************************************************************************
                            PIR sensor
*******************************************************************************/
/*                          PIR Variables                                     */
          
          
/******************************************************************************
*	               	 counters                                              *
*******************************************************************************/
          if(pir_tic) pir_tic--;					        //conter for PIR sensor
          if(led_green1_tic) led_green1_tic--;			                //conter for led
          if(current_triger_delay){
            if(!current_triger_delay_counter){
              current_triger_delay_counter= TIMER_1S;			        //couter for ocupansy triger delay after detection            
            }
            if(current_triger_delay_counter){
              current_triger_delay_counter--;                                   //couter for ocupansy triger delay after detection          
            }
          }
          if(current_triger_delay && (!current_triger_delay_counter))
            current_triger_delay--;
#endif               
          
          break;
        case TA0IV_TACCR1:break;
        case TA0IV_TACCR2:break;
        case TA0IV_TAIFG: break;
        default: break;
    }
}


/*******************************************************************************
*
*       RAB Lighting
*
*       LC - Controller
*
* This project is based on the TI MSP430I2020 processor. 
* This file contains PIR sensing and light ambient sensing function 
* as well as radio and relay control functions, including handling
* device specific commands and responses.
*
* Rev 7-8-15:   Add preproessor defined symbol LC_CONTROLLER
                Store device configuration in main flash segment 2
                Store factory default in flash information segment
                Implement power loss function
*******************************************************************************/

#include "msp430.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#define GLOBAL_DEF
#include "io.h"
#include "adc.h"
#include "timer.h"
#include "serial.h"
#include "dim.h"
#include "ctrl_pkt.h"
#include "api_pkt.h"
#include "lcp.h"
#include "lcp_constants.h"
#include "global.h"
#include "flash.h"


#define STRENGTH_H      50      // signal strength high level definition
#define STRENGTH_L      70      // signal strength low level definition

#define HOLDTIME        1000    // button hold time 10s
#define WAITTIME        200     // button wait for expire time 2s
#define CANCELTIME      400     // button cancel time 4s

unsigned int myDeviceType;      // device type variable

struct flash_def *DeviceCfg;
 
// Flag variables
bool SampleReadyFlag = 0, AssoFlag = 0, WaitControllerRsp = 0, WaitATRsp =0, SignalStrengthFlag = 0;

// Button variables
bool ButtonHold=0, ButtonPressHold=0; 
bool ButtonPress1=0, ButtonPress1Done=0, ButtonPress1Wait=0;
bool ButtonPress2=0, ButtonPress2Done=0;
unsigned int ButtonCounter1 = 0, ButtonCounter2 = 0, WaitCounter1 = 0, WaitCounter2 = 0;

unsigned char StrengthLevel = 0;
#ifdef LC_OCCUPANCY
/*******************************************************************************
                            PIR sensor 
*******************************************************************************/
/*                          PIR Variables                                     */
#include "sensor_motion.h"

#endif

/*******************************************************************************
*
* Function: ResetRvBuf
*
* Description:  reset serial receive buffer
*                        
*******************************************************************************/
void ResetRvBuf(void)
{
  unsigned int i;
  if(WaitATRsp==0){                     // Clear when not waiting for AT command response
  for(i=0; i< RX_BUF_SIZE; i++)         // Clear recieve buffer
      rx_buf[i] = 0;
  current_rx_ptr = rx_buf;              // reset receive ptr
  rx_byte_count = 0;                    // reset received byte count
  packet_rx_flag = false;               // Reset Rx flag
  }
}

/*******************************************************************************
*
* Function: ResetFlashImageBuf
*
* Description:  reset flash image buffer
*                        
*******************************************************************************/
void ResetFlashImageBuf(void)
{
  unsigned int i;
  for(i=0; i<FLASH_IMAGE_SIZE; i++)
    flash_image[i] = 0;
    
}

/*******************************************************************************
*
* Function: DoButtons
*
* Description:  process button press
*               1 long press: network reset
*                        
*******************************************************************************/
void DoButtons(void)
{
  if(BTN_READ==0) {                     // if Button is Pressed
    if((!ButtonPress2) && (!ButtonPressHold)&& (!ButtonHold)){ // if this is the first press
        if((ButtonPress1Wait == 0)){    // Only the 1st press, NOT waiting for second press
          
          if(ButtonCounter1 < HOLDTIME){// if button 1-press counter counts less than 10sec hold time
            ButtonPress1 = 1;           // this is a 1 short press temp flag
            ButtonCounter1 += 1;        // button 1 press counter increament
            if(ButtonCounter1 > CANCELTIME){    // if 1 press counter more than 4sec cancel time
              ButtonPress1 = 0;         // clear 1 short press temp flag
              ButtonPress1Wait = 0;     // clear wait for second press flag
            }
          }
          else {                        // if button 1-press counter counts more than 10sec hold time
            ButtonHold = 1;             // this is a 1 long press
            ButtonPress1 = 0;           // clear 1 short press temp flag
            ButtonPress1Wait = 0;       // clear wait for second press flag
            
            led_30s_05s_delay = 60;     // Do 30seconds led flash
            led_05s_delay = TIMER_05S;  // set up 0.5s interval
            ResetRvBuf();               // reset receive buffer
            send_radio_api_packet_param('N', 'R', 0);    // Send network reset
            ResetRvBuf();               // reset receive buffer
            AssoFlag = false;           // clear associate flag
            LED_G_OFF;                  // turn off Asso/Green LED
          }
        }
        else {                          // now we are Waiting for 2nd press
          ButtonPress1 = 0;
          ButtonPress1Done = 0;
          ButtonPress1Wait = 0;
          ButtonPress2 = 1;             // set 2 press temp flag
        }
    }
    if(ButtonPress2) {                  // this is the second press
      ButtonPress1Done = 0;             // not set 1 short press done flag 
      ButtonPress1Wait = 0;             // clear wait for 2nd press flag
      ButtonPress1 = 0;                 // clear 1 short press temp flag
      if(ButtonCounter2 < WAITTIME) {   // button 2 press counter less than wait time
        ButtonCounter2 += 1;            // button 2 press counter increment
        ButtonPress2 = 1;
      }
      else {
        ButtonPress2 = 0;               // if more than wait time, clear 2 short press temp flag
        ButtonPressHold = 1;            // setup 1 short 1 long press flag
      }
    }
  }
  else {                                // if Button Released
    ButtonCounter1 = 0;                 // clear button counter
    ButtonCounter2 = 0;                 // clear button counter
    ButtonHold = 0;                     // clear 1 long press flag
    if(ButtonPressHold){                // if 1 press 1 long hold
      ButtonPressHold=0;                // clear it
      if(DeviceCfg->deviceMode == LC_CONTROLLER_TYPE_DIMMER)
        lcp_state_report=1;             // send state change report
    }
    if(ButtonPress1){                   // if 1 short press temp flag set
        if(WaitCounter1 < WAITTIME){
          WaitCounter1 += 1;            // keep counting wait for 2nd press counter
          ButtonPress1Wait = 1;         // setup 1 short press wait flag
          ButtonPress2 = 0;
          ButtonPress2Done = 0;
        }
        else {
          WaitCounter1 = 0;             // if 1 press counter less than wait time, button released
          ButtonPress1 = 0;             // clear 1 press temp flag
          ButtonPress1Wait = 0;
          ButtonPress1Done = 1;         // set 1 press done flag
          ButtonPress2 = 0;               // clear 2 press flag
          ButtonPress2Done = 1;           // clear 2 press done flag
        }
    }
    else {                              // if not 1 short press
      if(ButtonPress2) {                // check if 2 press
        ButtonPress2 = 0;               
        ButtonPress2Done = 1;           // setup 2 press flag
      }
      ButtonPress1Wait = 0;             // clear 1 press wait flag
      ButtonPress1 = 0;                 // clear 1 short press flag
    }
  }
}

/*******************************************************************************
*
* Function: ButtonState
*
* Description:  button state machine
*               1 press: send identity packet
*               2 press: toggle relay
*               1 short press 1 long press: dimming in triangular fashion
*                        
*******************************************************************************/
void ButtonState(void)
{
      if(ButtonPress1Done){             // 1 short press
            ButtonPress1Done = 0;
            ButtonPress2Done = 0;
            ResetRvBuf();
            send_identity_response();   // send identity packet
      }
      if(ButtonPress2Done) {            // 2 short press
            ButtonPress2Done = 0;
            ButtonPress1Done = 0;
            if(DeviceCfg->deviceMode == LC_CONTROLLER_TYPE_DIMMER){    // if we are dimmer
              DoButtonToggle();         // toggle relay
            }
            if(DeviceCfg->deviceMode == LC_CONTROLLER_TYPE_TRIGGER){   // uf we are trugger
              lcp_state_report = 1;     // only send out state report
            }
      }
      if(ButtonPressHold){              // 1 short 1 long press
        if(DeviceCfg->deviceMode == LC_CONTROLLER_TYPE_DIMMER){
            RLY_ON;
            DoButtonDim();              // Dimming function
        }
      }
}

/*******************************************************************************
*
* Function: DoLCPReceive
*
* Description:  process LightCloud protocol receive packet
*               state request, identity request
*                        
*******************************************************************************/
void DoLCPReceive(void)
{
    unsigned int result;
    
    // Check if new LCP packet is available. If so, process it 
    if(lcp_packet_rx_flag == true)
    {
      WaitControllerRsp = true;         // wait for controller to send out response pkt
      led_rvpkt_delay = TIMER_2S;       // setup signal strength 2s led timer
      lcp_packet_rx_flag = false;       // clear receive pkt flag
      result = parse_lcp_packet();      // parse LCP packet
      switch(result)
      {
            case EParseTestOK:
                WaitControllerRsp = false;
                led_rvpkt_delay = 0;
                break;
                
            case EParseGoodStatus:      // this is good State request
                ResetRvBuf();
                send_status_response(); // Send status response packet
                ResetRvBuf();
                break;
                
            case EParseGoodIdentity:    // this is good Identity request
                if(lcp_id_feedback == 0x01){    // Check if we need to generate feedback, and generate it!!
                  led_30s_1s_delay = 30;        // setup white led blink every 1sec for 30sec
                  led_1s_delay = TIMER_1S;
                }
                if(lcp_id_feedback == 0x03){    // check if we need to generate feedback and toggle relay
                  led_30s_2s_delay = 16;        // setup white led blink every 2sec for 30sec and toggle relay
                  led_2s_delay = TIMER_2S;
                }
                ResetRvBuf();
                send_identity_response();// Send identity response packet
                break;              
            
            case EParseGoodPower:       // this is good Power request
                send_pm_result();
                break;
      }
    }
}


/*******************************************************************************
*
* Function: DoATReceive
*
* Description:  process LightCloud protocol receive packet
*               
*                        
*******************************************************************************/
void DoATReceive(void)
{
    unsigned char temp;
    
    // Check if new AT response pkt is available. If so, process it 
    if(packet_rx_flag == true)
    {
      packet_rx_flag = false;
      
      // Do AI command ask for association - 00 indicate associated;
      if((rx_buf[2] == 'A') && (rx_buf[3] == 'I')){
        if((rx_buf[CMD_RSP_STATUS] == 0) && (rx_buf[CMD_RSP_DATA] == 0)) // check if we are associated
              AssoFlag = true;  // setup associate flag
        else {
          AssoFlag = false;
          StrengthLevel = 0;
        }
      }
      // Do DB command ask for signal strength
      else if ((rx_buf[2] == 'D') && (rx_buf[3] == 'B')){
        temp = rx_buf[CMD_RSP_DATA];
        if((rx_buf[CMD_RSP_STATUS] == 0) && ( temp > 0x19) && (temp < 0x59)){ // if valid strength value 0x1A - 0x58
          if(temp < STRENGTH_H) StrengthLevel = 3;
          else if(temp < STRENGTH_L) StrengthLevel = 2;
          else StrengthLevel = 1;
        }
      }
    }
    WaitATRsp = 0;      // clear AT command response flag
}


/*******************************************************************************
*
* Function: main code
*
* Description:  initialize microcontroller
*               setup radio parameters
*               main loop - flag polling and function call
*                        
*******************************************************************************/
int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW | WDTHOLD;                     // Stop watchdog timer
  
  // start up xtal oscillator
  CSCTL1 |= DIVS__1 + DIVM__1;                  // SMCLK = DCO, MCLK = DCO
  FCTL2 = FWKEY + FSSEL_1 + FN4;                // Clk = SMCLK/16
  io_init();                                    // Configure IO ports
  timer_init();                                 // Configure Timer
  serial_init();                                // Configure Serial
  adc_init();                                   // ADC initialization
  __bis_SR_register(GIE);                       // Enable interrupt
  
  DeviceCfg = (struct flash_def *)flash_image;
  
  #ifdef LC_CONTROLLER
  // Define definition for this device
  myDeviceType = LC_DEVICE_TYPE_CONTROLLER;     // We're a Controller module
  #endif
  #ifdef LC_OCCUPANCY
  //  myDeviceType = LC_DEVICE_TYPE_CSCOUT;
  //* for test only *///
  myDeviceType = LC_DEVICE_TYPE_CONTROLLER;     // We're a Controller module 
  
  #ifdef OS_TEST_MODE
  current_controller_mode = occupancy_sensor;
  #endif
  #endif
  
  ResetRvBuf();
    
  //Reload flash device configuration to RAM
  memcpy(DeviceCfg, (unsigned char *)SEGSTART, FLASH_IMAGE_SIZE);       // copy flash device config to image buffer
  
  if((DeviceCfg->deviceMode < 1) || (DeviceCfg->deviceMode > 3)){       // if not setup as one of the device mode
    ResetFlashImageBuf();                                               // reset flash
    DeviceCfg->deviceMode = LC_CONTROLLER_TYPE_DIMMER;                  // set as a dimmer
  }
  
  //Reload flash power loss info to RAM
  memcpy(&powerloss_image, (unsigned char *)POWERLOSSSTART, POWER_LOSS_SIZE); // copy flash power loss config to image buffer
  TA0CCR1 = (powerloss_image[2] << 8) + (powerloss_image[1]);           // copy pwm state
  if(TA0CCR1 > PWM_FREQ) TA0CCR1 = PWM_FREQ;                            // check if pwm max reach
  if(powerloss_image[0] == 0) RLY_OFF;                                  // copy relay state
  else RLY_ON;
  
  // Erase flash Segment 2 and store back the device configuration
  flash_info_update();
  
  while(radioInitflag ==0){     // Waiting for radio to be readyperiod
    if(count_8ms == 0){         // run button function during initialization
      count_8ms = 2;
      DoButtons();
      ButtonState();
    }
  }
  read_digi_radio_info();       // Preset read radio information

  
  //**** Main Loop *************************************************************   
  while(1){
    // check white led - when no blink function needed, indicate power on
    if((led_30s_2s_delay==0) && (led_30s_05s_delay==0) &&(led_30s_1s_delay==0)) 
      LED_W_ON;
    
    DoLCPReceive();     // decode LC protocol pkt function call
    DoATReceive();      // decode AT command response 
    
    // check if need to send out state change report after dim or actuator toggle
    if((lcp_state_report)&&(DeviceCfg->deviceMode!= LC_CONTROLLER_TYPE_REPEATER)){
      ResetRvBuf();             // clear serial receive buffer
      state_change_report();    // make up state change report pkt
      lcp_state_report = 0;     // clear state change report flag
    }
       
    //**** Runs every 4ms ******************************************************
    if(count_8ms == 0){
      count_8ms = 2;
      DoButtons();
      ButtonState();
      
       //**** LCP Dim function *************************************************
      if(lcp_dim_flag) {
        //if(ControllerType == LC_CONTROLLER_TYPE_DIMMER) 
          DoDimFunction();
      }
      
      else if(WaitControllerRsp==0){ //Wait for finishing transmit
          // Signal Strength****************************************************
          if(SsCounter == 0){
            SsCounter = SSCOUNTER;
            ResetRvBuf();
            send_radio_api_packet('D', 'B');
            WaitATRsp = 1;
          }
          // Associate**********************************************************
          if(AssoCounter == 0){
            AssoCounter = ASSOCIATECOUNTER;
            ResetRvBuf();
            send_radio_api_packet('A', 'I');
            WaitATRsp = 1;
          }
      }
      
      if(AssoFlag){ // setup minimum signal strength level when associated!*****
        if((StrengthLevel==0) || (StrengthLevel > 3))
          StrengthLevel = 1;
      }
    } // finish 4ms loop********************************************************

#ifdef LC_OCCUPANCY
      
    //**** Runs every 4ms ***************************************   
    if(pir_tic==0){                                                             //run pir function every timer cycle (if)
                                                                                //if PIR motion hapen sensor_event_flag = pir_event
      pir_triger();                                                             //one sample of pir trigger function
      pir_tic=1;                                                                //run pir function every timer cycle (reset)
      
      ocupansy_action_check();
    
/**********************************************
 *	 LED driver in main loop
 ****************************************** */
 
    switch (led_current_state_G1) {
	case led_on:
          led_green1(TIMER_01S,TIMER_01S);
	break;
	case led_off:
          led_green1(0,TIMER_01S);
	break;
	case blink_patern_1:
          led_green1(TIMER_05S,TIMER_1S);
	break;
	case blink_patern_2:
          led_green1(TIMER_025S,TIMER_1S);
	break;
	case blink_patern_3:
          led_green1(TIMER_025S,TIMER_3S);
	break;
	default:
	break;
    }
  }    
    
#endif

  } // finish while loop
  
  return 0;
}

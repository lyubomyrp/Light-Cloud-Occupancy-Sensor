/*******************************************************************
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
********************************************************************/

#include "msp430.h"
#include <stdint.h>
#include <stdbool.h>
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

#define ASSOCIATECOUNTER 100    // check association every 10s based on 100ms timer loop
#define SSCOUNTER       30      // check signal strength every 13s based on 100ms timer

unsigned int myDeviceType;      //

// Flag variables
bool RecFlag = 0, PowerGood = 1, SampleReadyFlag = 0, AssoFlag = 0, WaitControllerRsp = 0, WaitATRsp =0, SignalStrengthFlag = 0;
bool lcp_goodid_2s_finish=0;

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
//**** Reset receive buffer***********************
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

//**** Reset flash image buffer******************
void ResetFlashImageBuf(void)
{
  unsigned int i;
  for(i=0; i<FLASH_IMAGE_SIZE; i++)
    flash_image[i] = 0;
}

//**** Button press process *********************
void DoButtons(void)
{
  if(BTN_READ==0) { // if Button is Pressed
    if((!ButtonPress2) && (!ButtonPressHold)&& (!ButtonHold)){ // if this is the first press
        if((ButtonPress1Wait == 0)) { // Only 1st press, not wait for second press
          if(ButtonCounter1 < HOLDTIME) { // if button 1-press counter counts less than 10sec hold time
            ButtonPress1 = 1;   // 
            ButtonCounter1 += 1;
            if(ButtonCounter1 > CANCELTIME){
              ButtonPress1 = 0;
              ButtonPress1Wait = 0;
            }
          }
          else {
            ButtonHold = 1;
            ButtonPress1 = 0;
            ButtonPress1Wait = 0;
            
            led_30s_05s_delay = 60;     // Do 30seconds led flash
            led_05s_delay = TIMER_05S;  // set up 0.5s interval
            ResetRvBuf();               // reset receive buffer
            send_radio_api_packet_param('N', 'R', 0);    // Send network reset
            ResetRvBuf();               // reset receive buffer
            AssoFlag = false;           // clear associate flag
            LED_G_OFF;                  // turn off Asso/Green LED
          }
        }
        else { // Waiting for 2nd press
          ButtonPress1 = 0;
          ButtonPress1Done = 0;
          ButtonPress1Wait = 0;
          ButtonPress2 = 1;
        }
    }   
        if(ButtonPress2) {
          ButtonPress1Done = 0;
          ButtonPress1Wait = 0;
          ButtonPress1 = 0;
          if(ButtonCounter2 < WAITTIME) {
            ButtonCounter2 += 1;
            ButtonPress2 = 1;
          }
          else {
            ButtonPress2 = 0;
            ButtonPressHold = 1;
          }
        }
  }
  else { // Button Released
    ButtonCounter1 = 0;
    ButtonCounter2 = 0;
    ButtonHold = 0;    
    if(ButtonPressHold){
      ButtonPressHold=0;
      if(ControllerType == LC_CONTROLLER_TYPE_DIMMER)lcp_state_report=1;
    }
    if(ButtonPress1){
        if(WaitCounter1 < WAITTIME){
          WaitCounter1 += 1;
          ButtonPress1Wait = 1;
          ButtonPress2 = 0;
          ButtonPress2Done = 0;
        }
        else {
          WaitCounter1 = 0;
          ButtonPress1 = 0;
          ButtonPress1Wait = 0;
          ButtonPress1Done = 1;
          ButtonPress2 = 0;
          ButtonPress2Done = 1;
        }
    }
    else {
      if(ButtonPress2) {
        ButtonPress2 = 0;
        ButtonPress2Done = 1;
      }
      ButtonPress1Wait = 0;
      ButtonPress1 = 0;
    }
  }
}

 //**** Button State Machine ********************
void ButtonState(void)
{
      if(ButtonPress1Done){ 
            ButtonPress1Done = 0;
            ButtonPress2Done = 0;
            ResetRvBuf();
            send_identity_response();
      }
      if(ButtonPress2Done) { // Actuator control
            ButtonPress2Done = 0;
            ButtonPress1Done = 0;            
            if(ControllerType == LC_CONTROLLER_TYPE_DIMMER){
              DoButtonToggle();
            }
            if(ControllerType == LC_CONTROLLER_TYPE_TRIGGER){
              lcp_state_report = 1;
            }
      }
      if((ButtonPressHold) && (ControllerType == LC_CONTROLLER_TYPE_DIMMER)){ // Dimming function
            RLY_ON;
            DoButtonDim();
      }
}

//**** Process LC Protocol **********************
void DoLCPReceive(void)
{
    unsigned int result;
    
    // Check if new LCP packet is available. If so, process it 
    if(lcp_packet_rx_flag == true)
    {
      WaitControllerRsp = true;
          led_rvpkt_delay = TIMER_2S;
          lcp_packet_rx_flag = false;
          result = parse_lcp_packet();
          switch(result)
          {
            case EParseTestOK:
                lcp_packet_count++;             // bump rx'd packet count
                parse_device_specific_command();// Not a global command
                                                // Send response packet
                break;
                
            case EParseGoodDiscovery:
                lcp_packet_count++;             // bump rx'd packet count
                ResetRvBuf();
                send_discovery_response();      // Send discovery response packet
                ResetRvBuf();
                break;
                
            case EParseGoodStatus:
                lcp_packet_count++;             // bump rx'd packet count
                ResetRvBuf();
                send_status_response();          // Send status response packet
                ResetRvBuf();
                break;
                
            case EParseGoodIdentity:
                lcp_packet_count++;             // bump rx'd packet count
                if(rx_lcp_buf[LCP_OFFSET_USER_FEEDBACK_FLAG] == 0x01){ // Check if we need to generate feedback, and generate it!!
                  led_30s_1s_delay = 30;
                  led_1s_delay = TIMER_1S;
                }
                if(rx_lcp_buf[LCP_OFFSET_USER_FEEDBACK_FLAG] == 0x03){
                  led_30s_2s_delay = 16;
                  led_2s_delay = TIMER_2S;
                }
                ResetRvBuf();
                send_identity_response();     // Send identity response packet
                break;              
          }
          if(lcp_pm_inst_flag){
            lcp_pm_inst_flag = false;
            send_pm_result();
          }
      }
}


//**** Process AT Command Response **********************
void DoATReceive(void)
{
    unsigned char temp;
    
    // Check if new AT response pkt is available. If so, process it 
    if(packet_rx_flag == true)
    {
      packet_rx_flag = false;
      
      // Do AI command ask for association - 00 indicate associated;
      if((rx_buf[2] == 'A') && (rx_buf[3] == 'I')){
        if((rx_buf[CMD_RSP_STATUS] == 0) && (rx_buf[CMD_RSP_DATA] == 0)) 
              AssoFlag = true;  // setup associate flag
        else {
          AssoFlag = false;
          StrengthLevel = 0;
        }
      }
      // Do DB command ask for signal strength
      else if ((rx_buf[2] == 'D') && (rx_buf[3] == 'B')){
        temp = rx_buf[CMD_RSP_DATA];
        if((rx_buf[CMD_RSP_STATUS] == 0) && ( temp > 0x19) && (temp < 0x59)){ 
          // valid strength value 0x1A - 0x58
          if(temp < STRENGTH_H) StrengthLevel = 3;
          else if(temp < STRENGTH_L) StrengthLevel = 2;
          else StrengthLevel = 1;
        }
      }
    }
    WaitATRsp = 0;      // clear AT command response flag
}


/****************************************************************
*
* Function: main code
*
* Description:  initialize microcontroller
*               setup radio parameters
*               main loop - flag polling and function call
*                        
*****************************************************************/
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
  
    
  #ifdef LC_CONTROLLER
  // Define definition for this device
  myDeviceType = LC_DEVICE_TYPE_CONTROLLER;     // We're a Controller module
  ControllerType = LC_CONTROLLER_TYPE_DIMMER;   // set Dimmer as default
  #endif
  #ifdef LC_OCCUPANCY
  //  myDeviceType = LC_DEVICE_TYPE_CSCOUT;
  //* for test only *///
  myDeviceType = LC_DEVICE_TYPE_CONTROLLER;     								// We're a Controller module 
  ControllerType = LC_CONTROLLER_TYPE_DIMMER;   								// set Dimmer as default
  
  #ifdef OS_TEST_MODE
  current_controller_mode = occupancy_sensor;
  #endif
  #endif
  
  
  
  ResetFlashImageBuf();
  ResetRvBuf();
  
  // Define definition for this device
  myDeviceType = LC_DEVICE_TYPE_CONTROLLER;             // We're a Controller module
  LED_W_ON;                                             // if power loss, white LED on?
    
  // Reload flash device configuration to RAM
  flash_info_copy();                                    // copy flash device config to image buffer
  ControllerType = flash_image[FLASH_DEVICE_MODE];
  if((ControllerType < 1) || (ControllerType > 3)){     // if not setup as repeater or trigger
    ResetFlashImageBuf();
    flash_image[FLASH_DEVICE_MODE] = LC_CONTROLLER_TYPE_DIMMER;         // set as a dimmer
    ControllerType = LC_CONTROLLER_TYPE_DIMMER;
    CurveType = CURVE_LINEAR;
    lcp_softon_flag = 0;
  }
  else{
    CurveType = flash_image[FLASH_CURVE_TYPE];            // copy curve type info
    lcp_softon_flag = flash_image[FLASH_SOFT_DIM_ON];     // copy soft on/off setting
    if(lcp_softon_flag > 1) lcp_softon_flag = 1;
  }
  
  // Reload flash power loss information to RAM
  powerloss_copy();                                     // copy flash power loss info to image buffer
  TA0CCR1 = (powerloss_image[2] << 8) + (powerloss_image[1]);   // copy pwm state
  if(TA0CCR1 > PWM_FREQ) TA0CCR1 = PWM_FREQ;                
  if(powerloss_image[0] == 0) RLY_OFF;                  // copy relay state
  else RLY_ON;
  
  // Erase flash segment 2 and store back the device configuration
  flash_info_update();
  
  while(radioInitflag ==0){                             // Waiting for radio to be readyperiod
    if(count_8ms == 0){                                 // run button function during initialization
      count_8ms = 2;
      DoButtons();
      ButtonState();
    }
  }
  read_digi_radio_info();                               // Preset read radio information

  
  //**** Loop *************************************************   
  while(1){
    
    // check white led - when no blink function needed, indicate power on
    if((led_30s_2s_delay==0) && (led_30s_05s_delay==0) &&(led_30s_1s_delay==0)) 
      LED_W_ON;
    
    DoLCPReceive();     // decode LC protocol pkt function call
    DoATReceive();      // decode AT command response 
    
    // check if need to send out state change report after dim or actuator toggle
    if((lcp_state_report)&&(ControllerType != LC_CONTROLLER_TYPE_REPEATER)){
      ResetRvBuf();             // clear serial receive buffer
      state_change_report();    // make up state change report pkt
      lcp_state_report = 0;     // clear state change report flag
    }
       
    //**** Runs every 4ms ***************************************   
    if(count_8ms == 0){
      count_8ms = 2;
      DoButtons();
      ButtonState();
      
       //**** LCP Dim function ***********************************
      if(lcp_dim_flag) {
        //if(ControllerType == LC_CONTROLLER_TYPE_DIMMER) 
          DoDimFunction();
      }
      // Signal Strength and Association************************
      //else if((WaitATRsp == 0)&&(WaitControllerRsp==0)){
      else if(WaitControllerRsp==0){ //Wait for finishing transmit
          if(SsCounter == 0){
            SsCounter = SSCOUNTER;
            ResetRvBuf();
            send_radio_api_packet('D', 'B');
            WaitATRsp = 1;
          }
          // Associate*****************************************
          if(AssoCounter == 0){
            AssoCounter = ASSOCIATECOUNTER;
            ResetRvBuf();
            send_radio_api_packet('A', 'I');
            WaitATRsp = 1;
          }
      }
      
      if(AssoFlag){
        if((StrengthLevel==0) || (StrengthLevel > 3))
          StrengthLevel = 1;
      }
    } // finish 4ms loop

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

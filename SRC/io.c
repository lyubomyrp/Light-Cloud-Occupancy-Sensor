/*******************************************************************************
*
*   File name: io.c
*
*   LC - Controller
*   UART        - P1.2, P1.3
*   I2C         - P1.6, P1.7
*   Outputs (1) - P2.6(PWM); P1.0(RLY); P2.1-P2.5,P2.7,P1.1(LEDs)
*   Inputs (0)  - P1.1(P2.0)(Btn); P1.4(PWR_OK); A0.0(V), A1.0(I)
*
******************************************************************************S*/

#define IO_DEF
#include  <stdbool.h>
#include "msp430.h"
#include "io.h"
#include "ctrl_pkt.h"
#include "flash.h"
#include "global.h"

/*******************************************************************************
*
* Function: io_init
*
* Description: Digital IO port Initialization
* Initialize input and output pins
*
*******************************************************************************/
int io_init()
{
#ifdef LC_CONTROLLER
  
  P1DIR |= BIT0 + BIT1;         // Set P1.0, P1.1 as output    
  P2DIR = 0xFE;                 // Set P2.1-P2.7 as outputs
  //P2IES |= BIT0;                // P2.0 power ok high-low transition interrupt
  //P2IE |= BIT0;                 // Enable interrupt
  //P2IFG &= ~BIT0;               // Clear interrupt flag
  P1DIR &= ~BIT4;
  P1IES |= BIT4;                // P1.4 power ok high-low transition interrupt
  P1IE |= BIT4;                 // Enable interrupt
  P1IFG &= ~BIT4;               // Clear interrupt flag
  
  LED_ALL_OFF;                  // All led off
  LED_W_ON;                     // if no power loss, white LED on?
  RLY_OFF;                      // relay off
  PWM_OFF;                      // pwm output off
  

#elif  LC_OCCUPANCY
 
  /*Set as output*/
  P1DIR |= BIT0;        // White LED
  P1DIR |= BIT4;        // White REL
  P1DIR |= BIT5;        // White PWM
  
  P2DIR |=BIT0;         // GREEN1 LED
  P2DIR |=BIT1;         // BLUE1  LED  
  P2DIR |=BIT2;         // BLUE2  LED    
  P2DIR |=BIT3;         // BLUE3  LED  
  P2DIR |=BIT4;         // BLUE3  LED
  P2DIR |=BIT5;         // GREEN  LED 
  P2DIR |=BIT6;         // BLUE   LED   
    
  /*Set as Input*/
  P1DIR &= ~BIT1;       // BTN
  P1DIR &= ~BIT7;       // Pwr ok
  P2IES |= BIT7;        // Power ok high-low transition interrupt
  P2IE |= BIT7;         // Enable interrupt
  P2IFG &= ~BIT7;       // Clear interrupt flag
  
  RLY_OFF;
  PWM_OFF;
  
  
#endif

  return 0;
}

/*******************************************************************************
*
* Function: PORT1_ISR
*
* Description: Digital IO port 1 interrupt handler
*
* Interrupt handling for P1 IO pins
*
*******************************************************************************/
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    switch(__even_in_range(P1IV,0x10)){
        case 0x00: break;               // No interrupt
        case 0x02: break;               // P1.0 interrupt
        case 0x04: break;               // P1.1 interrupt
        case 0x06: break;               // P1.2 interrupt
        case 0x08: break;               // P1.3 interrupt
        case 0x0A:                      // P1.4 interrupt
          LED_ALL_OFF;
          powerloss_image[0] = RLY_READ;
          RLY_OFF;
          powerloss_image[1] = TA0CCR1;
          powerloss_image[2] = TA0CCR1 >> 8;
          flash_write();
          send_power_loss_detect();
          P1IFG &= ~BIT4;               // Clear IFG
          break;
        case 0x0C: break;               // P1.5 interrupt
        case 0x0E: break;               // P1.6 interrupt
        case 0x10: break;               // P1.7 interrupt
    }
}


/*****************************************************************************
*   
*   File name: io.h
*
*   LC - Controller
*   
*   Red and Green LEDs  - Associate indicator
*   3 Blue LEDs         - RSSI indicators
*   White LED           - Button indicator
*   Xbee reset
*   Button, Relay output, PWM control
*
*****************************************************************************/

#ifdef IO_DEF
#define IO_EXT
#else
#define IO_EXT extern
#endif

IO_EXT int io_init();

// define IO output pins

#ifdef LC_CONTROLLER

#define LED_R_ON        P1OUT |= 0x02   //P1.1
#define LED_R_OFF       P1OUT &= ~0x02
#define LED_G_ON        P2OUT |= 0x02   //P2.1
#define LED_G_OFF       P2OUT &= ~0x02
#define LED_B0_ON       P2OUT |= 0x04   //P2.2
#define LED_B0_OFF      P2OUT &= ~0x04
#define LED_B1_ON       P2OUT |= 0x08   //P2.3
#define LED_B1_OFF      P2OUT &= ~0x08
#define LED_B2_ON       P2OUT |= 0x10   //P2.4
#define LED_B2_OFF      P2OUT &= ~0x10
#define LED_B3_ON       P2OUT |= 0x20   //P2.5
#define LED_B3_OFF      P2OUT &= ~0x20
#define LED_W_ON        P2OUT |= 0x80   //P2.7
#define LED_W_OFF       P2OUT &= ~0x80
#define LED_W_TOGGLE    P2OUT ^= 0x80
#define LED_R_TOGGLE    P1OUT ^= 0x02
#define LED_G_TOGGLE    P2OUT ^= 0x02
#define LED_B1_TOGGLE   P2OUT ^= 0x08
#define LED_B2_TOGGLE   P2OUT ^= 0x10
#define LED_B3_TOGGLE   P2OUT ^= 0x20
#define LED_ALL_ON      ((P2OUT|=0xBE)&&(P1OUT |= 0x02))
#define LED_ALL_OFF     ((P2OUT&=~0xBE)&&(P1OUT &= ~0x02))

#define PWM_ON          P2OUT |= 0x40   //P2.6
#define PWM_OFF         P2OUT &= ~0x40
#define RLY_ON         	P1OUT |= 0x01   //P1.0
#define RLY_OFF         P1OUT &= ~0x01
#define RLY_TOGGLE      P1OUT ^= 0x01
#define RLY_READ        (P1IN & 0x01)
#define BTN_READ        (P2IN & 0x01)

#endif

#ifdef  LC_OCCUPANCY

  #define LED_R_ON        P2OUT |= 0x10                 // RED (MULTICOLOR)
  #define LED_R_OFF       P2OUT &= ~0x10                // RED (MULTICOLOR)
  #define LED_R_TOGGLE    P2OUT ^= 0x10                 // RED (MULTICOLOR)

  #define LED_G_ON        P2OUT |= 0x20                 // GREEN (MULTICOLOR)
  #define LED_G_TOGGLE    P2OUT ^= 0x20                 // GREEN (MULTICOLOR)
  #define LED_G_OFF       P2OUT &= ~0x20                // GREEN (MULTICOLOR)

  #define LED_B0_ON       P2OUT |= 0x40                 // BLUE (MULTICOLOR)
  #define LED_B0_OFF      P2OUT &= ~0x40                // BLUE (MULTICOLOR)
  #define LED_B0_TOGGLE   P2OUT ^= 0x40                 // BLUE (MULTICOLOR)


  #define LED_G1_ON        P2OUT |= 0x01                // GREEN LED
  #define LED_G1_TOGGLE    P2OUT ^= 0x01                // GREEN LED
  #define LED_G1_OFF       P2OUT &= ~0x01               // GREEN LED

  
  #define LED_B1_ON       P2OUT |= 0x02                 // BLUE1 LED
  #define LED_B1_OFF      P2OUT &= ~0x02
  #define LED_B1_TOGGLE   P2OUT ^= 0x02

  #define LED_B2_ON       P2OUT |= 0x04                 // BLUE2 LED
  #define LED_B2_OFF      P2OUT &= ~0x04
  #define LED_B2_TOGGLE   P2OUT ^= 0x04

  #define LED_B3_ON       P2OUT |= 0x08                 // BLUE3 LED
  #define LED_B3_OFF      P2OUT &= ~0x08
  #define LED_B3_TOGGLE   P2OUT ^= 0x08


  #define LED_W_ON        P1OUT |= 0x01                 //WHITE  LED
  #define LED_W_OFF       P1OUT &= ~0x01
  #define LED_W_TOGGLE    P1OUT ^= 0x01
  

  #define LED_ALL_ON      (P2OUT|=0x3F)
  #define LED_ALL_OFF     (P2OUT&=~0x3F)
  
  #define PWM_ON          P2OUT |= 0x20
  #define PWM_OFF         P2OUT &= ~0x20

  #define RLY_ON          P1OUT |= 0x10   //P1.0
  #define RLY_OFF         P1OUT &= ~0x10
  #define RLY_TOGGLE      P1OUT ^= 0x10
  #define RLY_READ        (P1IN & 0x10)
  #define BTN_READ        (P1IN & 0x02)

#endif

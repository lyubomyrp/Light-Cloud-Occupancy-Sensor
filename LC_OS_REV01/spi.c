/*****************************************************************************
*
*   File name: spi.c
*
*   LC - Controller
*   Initialize UCB0 SPI-master mode and service routine
*   PGA113 SPI mode 0,0 clk idles low, data read on rising edge
*   data change on falling edge
*
*****************************************************************************/
#include  <stdbool.h>
#include "io430.h"
#include "io.h"
#include "spi.h"

char received_ch = 0;

void spi_init()
{
 UCB0CTL1 = UCSWRST;    // UCSI software reset
  UCB0CTL0 |=
    UCCKPH      // Clock phase-data change on first UCLK edge and capture on following edge
    + UCMSB     // MSB first
    + UCMST     // SPI-master
    + UCSYNC;   // SPI mode
  UCB0CTL1 |= UCSSEL_2; // SMCLK
  UCB0BR0 |= 0x02;      // Baud rate - /2
  UCB0BR1 = 0;
  UCB0CTL1 &= ~UCSWRST; // Initialize USCI state machine
}

void opamp_init()
{
  SPI_ENA;      //  Select  Device
  while(!(IFG2 & UCB0TXIFG));   //  USCI_A0 TX buffer ready?
  UCB0TXBUF = 0x2A;             //  Send write command to slave
  UCB0TXBUF = 0x11;             //  Select CH1 and Gain x100 = 0x2A61
  while(!(IFG2 & UCB0TXIFG));   //  USCI_A0 TX buffer ready?
  while(!(IFG2 & UCB0RXIFG));   //  USCI_A0 RX Received?
  received_ch = UCB0RXBUF;      //  Store received data
  SPI_DIS;      //  Unselect Device
}

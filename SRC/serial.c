/*****************************************************************************
*   
*   File name: serial.c
*
*   Description: Serial port interrupt handlers
*   This file contains the code for low level handling of two serial ports.
*
******************************************************************************
*/
#include  <stdbool.h>
#include "msp430.h"
//#define SERIAL_DEF
#include "io.h"
#include "serial.h"
#include "lcp.h"
#include "lcp_constants.h"
#include "api_pkt.h"
#include "global.h"

unsigned int tx_offset = 0;
unsigned int tx_index = 0;
unsigned int tx_count = 0;
unsigned char *current_tx_ptr;
unsigned char *current_rx_ptr;
unsigned int packet_length = 0;
unsigned char frame_type = 0;

unsigned char lcp_byte_count;
unsigned char rx_byte_count = 0;
bool packet_rx_flag = false;
bool tx_complete_flag = false;
bool lcp_packet_rx_flag = false;

unsigned char rx_state = 0;             // LightCloud Protocol state
unsigned char lcp_length = 0;           // LightCloud Protocol payload length

// Digi frame decode variables
unsigned char digi_header_count = 0;    // Count  of Digi header field bytes

unsigned char rx_buf[RX_BUF_SIZE]; // receive all other packet data except LCP data
unsigned char rx_lcp_buf[RX_LCP_BUF_SIZE];// receive only LCP packet data
/****************************************************************
*
* Function: serial_init
*
* Description: Serial port Initialization
*
* Initialize USART0
* (some code shamelessly lifted from the TI demo code - fet140_uart01_19200_2.c)
*
*****************************************************************/
int serial_init()
{

  P1SEL0 |=   BIT2 | BIT3;              // P1.2/3 eUSCI_A Function
  P1SEL1 &= ~(BIT2 | BIT3);
 
  UCA0CTL1 |= UCSWRST;                  // L:Hold eUSCI in reset
  UCA0CTL1 |= UCSSEL_2;                 // H:SMCLK 16MHz
  UCA0CTL0 = 0;                         // noParity,LSB,8-bit,1-Stop,UART,Asyn
  UCA0BR0   = 0xAA;                     // 9600 baud rate
  UCA0BR1   = 0x06;                     // 16.384MHz/9600 = 1706.6667 integer portion = x06AA
  UCA0MCTLW = 0xD600;                   // fraction portion 0.6667 = xD6 (See UG)
  UCA0CTL1 &= ~UCSWRST;                 // Initialize USART state machine/Release from reset
  UCA0IE   |= UCRXIE;                   // Enable RX interrupt
    
  current_rx_ptr = rx_buf;              // reset receive ptr
  rx_byte_count = 0;

  return 0;
}

/*****************************************************************************
*
* Function: serial_0_xmit
*
* Description: serial port 0 transmit function
* This function starts the transmission of a buffer on USART 0
* The first character is transmitted here. The remainder of the buffer is
* transmitted via the transmit interrupt function
*
* Input:  buf_index - offset to correct transmit buffer
*         len - length of data
******************************************************************************
*/

unsigned int serial_0_xmit(unsigned char buf_index, unsigned char len)
{
  unsigned int ret_val = 0;

  tx_complete_flag = false;             // Clear transmit complete flag
  tx_offset = buf_index;                // save buffer we're transmitting
  tx_index = 1;                         // bump index for next time
  tx_count = len;                       // save byte count
  current_tx_ptr = tx_buf;              // set up transmit ptr
  //if (!(UCA0STATW & UCBUSY))            // if UCSI not busy
  {
    UCA0TXBUF = *current_tx_ptr;        // send the byte
    current_tx_ptr++;                   // point to next byte
    UCA0IE |= UCTXIE;                   // Enable USART0 TX interrupt
  }
  return ret_val;
}

/*****************************************************************************
*
* Function: usart0_rx_int
*
* Description: USART0 receive interrupt handler
* Called on reciept of a character or receive error condition.
* Check for any receive error conditions. If error, throw away recieved byte,
* bump error count. Otherwise, get byte from the receive register. add it to
* the receive buffer. If we've reached the end of the message, send the buffer
* for processing.
*
* Function: usart0_tx_int
*
* Description: USART0 transmit interrupt handler
* Check if there are any more bytes to transmit. If there are, transmit the
* next byte and decrement the byte count. Otherwise just exit
*
******************************************************************************
*/

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG)) {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:{ // usart0 recieve interrupt service routine
          unsigned char temp;
            temp = UCA0RXBUF;                           // get recieved byte         
            // Decode frame
            switch (rx_state)
            {
              case RX_WAIT_FOR_FRAME_START:             // Digi API frame Start Delimiter
                if(temp == DIGI_API_FRAME_START){       // Start of valid API packet
                  rx_state++;
                }
                break;
            
              case RX_WAIT_FOR_LEN_1:                   // Length MSB byte
                packet_length = temp;                   // Store MSB
                packet_length <<= 8;                    // Shift to high byte
                rx_state++;
                break;
            
              case RX_WAIT_FOR_LEN_2:                   // Length LSB byte
                packet_length |= temp;                  // Shift in low byte
                rx_state++;
                break; 
                 
              case RX_WAIT_FOR_FRAME_TYPE:              // Digi API Frame type
                frame_type = temp;
                if(frame_type == RX_PACKET_TYPE)        // Zigbee Receive Packet?
                {
                  current_rx_ptr = rx_lcp_buf;          // reset receive ptr
                  rx_state++;                           // Process Lightcloud packet
                }
                else if((frame_type == AT_RSP_PACKET_TYPE) || (frame_type == TX_STATUS_PACKET_TYPE)) //or Zigbee Transmit Status?
                //else if(frame_type == AT_RSP_PACKET_TYPE)
                { // AT Command Response?
                  current_rx_ptr = rx_buf;              // reset receive ptr
                  *current_rx_ptr++ = temp;             // Store data
                  rx_state = RX_RCV_RSP_DATA;           // Process command response
                }
                else
                {
                  rx_state = RX_WAIT_FOR_FRAME_START;   // Ignore other frame types, start over
                }
                break;
            
              case RX_WAIT_FOR_DATA_START:              // Process Zigbee Receive Packet Address field
                if(digi_header_count == API_BODY_COUNT) // 64 bit source address + 16 bit network address done + receive option?
                {
                  rx_state++;                           // Yes, wait for LCP data
                  digi_header_count = 0;
                }
                else
                {
                  digi_header_count++;                  // wait for next address byte
                }
                break;
                        
              case RX_WAIT_FOR_PREFIX:                  // Prepare for Zigbee Receive Packet Receive LCP Data - Prefix
                if(temp == LC_DEVICE_COMMAND_PREFIX_BYTE)
                {
                  current_rx_ptr = rx_lcp_buf;          // reset receive ptr
                  lcp_byte_count = 0;                   // reset rx'd byte count
                  rx_state++;                           // We got a prefix. Get byte count next
                }
                break;
            
              case RX_WAIT_FOR_COUNT:                   // Store Zigbee Receive Packet Receive LCP Data - length
                lcp_length = temp;                      // Save payload length
                *current_rx_ptr++ = temp;               // Store length into data buffer
                rx_state++;                             // Next get data, wait for terminating character
                break;
            
              case RX_PROCESS_DATA:                     // Check for end of buffer!
                *current_rx_ptr = temp;                 // Store data
                current_rx_ptr++;                       // point to next receive slot
                lcp_byte_count++;                       // bump byte count
                lcp_length--;                           // Decrement count
                if(lcp_length == 0)                     // Packet should be finished
                  rx_state++;                           // So check for terminating character
                break;
            
              case RX_WAIT_FOR_TERMINATOR:              // Wait for LCP packet terminator 0x0D
                if(temp == LC_PACKET_TERMINATOR_BYTE)
                {
                  lcp_packet_rx_flag = true;            // trigger packet processing if valid frame
                }
                SignalStrengthFlag = true;
                rx_state = RX_WAIT_FOR_FRAME_START;     // Wait for next frame
                break;  
              
              case RX_RCV_RSP_DATA:                     // Process Transmit status or AT Command Response
                *current_rx_ptr++ = temp;               // Store data
                rx_byte_count++; 
                if(rx_byte_count == packet_length)
                {
                  packet_rx_flag = true;                // trigger packet processing
                  rx_state = RX_WAIT_FOR_FRAME_START;
                  rx_byte_count = 0;
                }
                break;
            }
            break;
        }
        case USCI_UART_UCTXIFG:{ // usart0 transmit interrupt service routine
          if(tx_index == tx_count)
          {
            UCA0IE &= ~UCTXIE;          // Disable USCI_A0 TX interrupt
            tx_complete_flag = true;    // Announce transmit completion
            WaitControllerRsp = 0;
          }
          else
          if(tx_index < tx_count)       // if we haven't finished sending the data in the buffer
          {
            UCA0TXBUF = *current_tx_ptr;
            current_tx_ptr++;           // next byte to send
            tx_index++;                 // transmitted byte count
          }
          break;
        }
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;
        default: break;
      }
}


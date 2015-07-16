/*******************************************************************************
*   
*   File name: serial.h
*
*   defines serial interrupt
*   
*******************************************************************************/
#ifdef SERIAL_DEF
#define SERIAL_EXT
#else
#define SERIAL_EXT extern
#endif

// Defines
#define TX_BUF_SIZE     50
#define RX_BUF_SIZE     50
#define RX_LCP_BUF_SIZE 20

// Offsets in rx_buf for various fields
#define CMD_RSP_FRM_TYPE        0x03            // Frame type for response frame
#define CMD_RSP_STATUS          0x04            // Status info in Command Response frame
#define CMD_RSP_DATA            0x05

#define DIGI_API_FRAME_START    0x7E            // Digi API Frame start delimiter
#define AT_RSP_PACKET_TYPE      0x88            // Digi API AT command response packet
#define TX_STATUS_PACKET_TYPE   0x8B            // Digi API Transmit status
#define RX_PACKET_TYPE          0x90            // Digi API receive frame type
#define API_BODY_COUNT          10              // 64 bit source address + 16 bit network address + receive options byte

enum serial_rx_states {
                 RX_WAIT_FOR_FRAME_START,       // Digi API frame start - State 0
                 RX_WAIT_FOR_LEN_1,             // First length byte - State 1
                 RX_WAIT_FOR_LEN_2,             // Second length byte - State 2
                 RX_WAIT_FOR_FRAME_TYPE,        // Digi API Frame type - State 3
                 RX_WAIT_FOR_DATA_START,        // Count to API frame data field - State 4
                 RX_WAIT_FOR_PREFIX,            // LCP Prefix byte - State 5
                 RX_WAIT_FOR_COUNT,             // LCP packet byte count - State 6
                 RX_PROCESS_DATA,               // State 7
                 RX_WAIT_FOR_TERMINATOR,        // State 8
                 RX_RCV_RSP_DATA                // State 9
};

SERIAL_EXT int serial_init(void);
SERIAL_EXT unsigned int serial_0_xmit(unsigned char buf_index, unsigned char len);

SERIAL_EXT bool packet_rx_flag;
SERIAL_EXT bool lcp_packet_rx_flag;

SERIAL_EXT unsigned char rx_byte_count;
SERIAL_EXT unsigned char lcp_byte_count;
SERIAL_EXT unsigned char *current_rx_ptr;

// Define buffers
SERIAL_EXT unsigned char rx_buf[RX_BUF_SIZE]; // receive all other packet data except LCP data
SERIAL_EXT unsigned char rx_lcp_buf[RX_LCP_BUF_SIZE];// receive only LCP packet data


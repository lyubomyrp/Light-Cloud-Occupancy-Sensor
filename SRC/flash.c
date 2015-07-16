#include "msp430.h"
#include <stdbool.h>
#include <string.h>
#include "ctrl_pkt.h"
#include "global.h"
#include "flash.h"

//unsigned char panID[8] = {0x00, 0x13, 0xa2, 0x00, 0x40, 0xb6, 0x68, 0x3d};

typedef struct flash_def
{
    unsigned char deviceType;
    unsigned char deviceMode;
    unsigned char curveType;
    unsigned char softDimOn;
    unsigned char powerPktOn;
    unsigned char relayState;
    unsigned char filler[10];
}flash_info;

unsigned char flash_image[FLASH_IMAGE_SIZE];
unsigned char powerloss_image[POWER_LOSS_SIZE];
flash_info *info_ptr;

/****************************************************************
*
* Function: flash_erase
*
* Description: 
* 
* 
****************************************************************/

void flash_erase(void) {
  int *addr = (int *)SEGSTART;
  
  while(BUSY & FCTL3);
  FCTL2 = FWKEY + FSSEL_1 + FN3;       // Clk = SMCLK
  FCTL1 = FWKEY + ERASE;               // Set Erase bit
  FCTL3 = FWKEY;                       // Clear Lock bit
  *addr = 0;                           // Dummy write to erase Flash segment
  while(BUSY & FCTL3);                 // Check if Flash being used
  FCTL1 = FWKEY;                       // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                // Set LOCK bit
}

/****************************************************************
*
* Function: flash_info_copy
*
* Description: Copy a flash info segment into RAM
* 
* 
****************************************************************/

unsigned int flash_info_copy(void)
{
  unsigned int temp_ptr;
    
  temp_ptr = SEGSTART;
  memcpy(&flash_image, (unsigned char *)temp_ptr, FLASH_IMAGE_SIZE);      // Copy flash segment to RAM

  return 0;
}

unsigned int powerloss_copy(void)
{
  unsigned int temp_ptr;
    
  temp_ptr = POWERLOSSSTART;
  memcpy(&powerloss_image, (unsigned char *)temp_ptr, POWER_LOSS_SIZE);      // Copy flash segment to RAM

  return 0;
}

/****************************************************************
*
* Function: flash_powerloss_update
*
* Description: store power loss information from 0xF410
* 
****************************************************************/

void flash_powerloss_update(void) 
{
    unsigned char *Flash_ptr;                   // Flash pointer
    unsigned int i;

    Flash_ptr = (unsigned char *)POWERLOSSSTART;// Initialize Flash pointer

    if(FCTL3 & LOCKSEG) {                       // If Info Seg is stil locked
        FCTL3 = FWKEY | LOCKSEG;                // Clear LOCKSEG bit
    }
    FCTL1 = FWKEY | WRT;                        // Set WRT bit for write operation

    for (i = 0; i < POWER_LOSS_SIZE; i++) {
        *Flash_ptr++ = powerloss_image[i];      // Write value to flash
    }

    FCTL1 = FWKEY;                              // Clear WRT bit
    FCTL3 = FWKEY | LOCKSEG;                    // Set LOCKSEG bit
}

/****************************************************************
*
* Function: flash_info_update
*
* Description: Update a flash info segment block
* Segments are 64 bytes long and start at address 0x1000
****************************************************************/

unsigned int flash_info_update(void)
{
  unsigned char *flash_ptr;                     // Flash pointer
  unsigned int i;
    
  flash_ptr =  (unsigned char *)SEGSTART;       // Calculate segment address
  
  // Erase segment
  while(FCTL3 & BUSY);                  // Check Flash BUSY bit  
  FCTL2 = FWKEY + FSSEL_1 + FN3;
  FCTL3 = FWKEY;                        // Clear Lock bit
  FCTL1 = FWKEY | ERASE;                // Set Erase bit
  *flash_ptr = 0;                       // Dummy write to erase Flash segment

  // Copy RAM area to flash info segment
  while(FCTL3 & BUSY);                  // Check Flash BUSY bit
  FCTL1 = FWKEY | WRT;                  // Set WRT bit for write operation
  for(i = 0; i < FLASH_IMAGE_SIZE; i++)
  {
    while(!(FCTL3 & WAIT));             // WAIT until Flash is ready
    *flash_ptr++ = flash_image[i];      // Write value to flash
  }
  while(FCTL3 & BUSY);                  // Check Flash BUSY bit
  FCTL1 = FWKEY;                        // Clear WRT bit
  FCTL3 = FWKEY | LOCKSEG;              // Set LOCKSEG bit
 
  return 0;
}
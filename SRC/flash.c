/*******************************************************************************
*   File name: flash.c
*
*   Description: flash write and flash memory update in main memory
*
*
********************************************************************************
*/

#include "msp430.h"
#include <stdbool.h>
#include "ctrl_pkt.h"
#include "global.h"
#include "flash.h"

unsigned char flash_image[FLASH_IMAGE_SIZE];
unsigned char powerloss_image[POWER_LOSS_SIZE];

/*******************************************************************************
*
* Function: flash_write
*
* Description: store power loss information from 0xF410
* 
*******************************************************************************/

void flash_write(void) 
{
    unsigned char *flash_ptr;                   // Flash pointer
    unsigned int i;

    flash_ptr = (unsigned char *)POWERLOSSSTART;// Initialize Flash pointer

    if(FCTL3 & LOCKSEG) {                       // If Info Seg is stil locked
        FCTL3 = FWKEY | LOCKSEG;                // Clear LOCKSEG bit
    }
    FCTL1 = FWKEY | WRT;                        // Set WRT bit for write operation

    for (i = 0; i < POWER_LOSS_SIZE; i++) {
        *flash_ptr++ = powerloss_image[i];      // Write value to flash
    }

    FCTL1 = FWKEY;                              // Clear WRT bit
    FCTL3 = FWKEY | LOCKSEG;                    // Set LOCKSEG bit
}

/*******************************************************************************
*
* Function: flash_info_update
*
* Description: Update a flash info segment block
* Segments are 64 bytes long and start at address 0xF400
*******************************************************************************/

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

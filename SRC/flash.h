/*****************************************************************************
*   File name: flash.h
*
*       Defines for flash memory program
*
******************************************************************************
*/

#define SEGSTART 0xF400                         // Address of the beginning of Segment 2 (xF400 - xF700)
#define POWERLOSSSTART 0xF410                   // Dedicate for power loss store actuator state/dim level
#define SEG_LEN  0x10                           // Number of bytes within segment to erase/write
#define FLASH_IMAGE_SIZE 0x10                   // Only use 16 bytes of flash for image storage
#define POWER_LOSS_SIZE 0x04                    // only use 4 bytes of flash for power loss informatin storage

extern void flash_erase(void);
extern unsigned int flash_info_copy(void);
extern unsigned int flash_info_update(void);
extern void flash_write(void);
extern unsigned int powerloss_copy(void);

extern unsigned char flash_image[FLASH_IMAGE_SIZE];
extern unsigned char powerloss_image[POWER_LOSS_SIZE];
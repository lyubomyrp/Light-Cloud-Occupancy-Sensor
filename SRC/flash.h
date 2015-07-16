

#define SEGSTART 0xF400                         // Address of the beginning of Segment 2 (xF400 - xF700)
#define POWERLOSSSTART 0xF410                   // Dedicate for power loss store actuator state/dim level
#define SEG_LEN  0x10                           // Number of bytes within segment to erase/write
#define FLASH_IMAGE_SIZE 0x10                   // Only use 16 bytes of flash for image storage
#define POWER_LOSS_SIZE 0x04                    // only use 4 bytes of flash for power loss informatin storage

#define FLASH_DEVICE_MODE     0
#define FLASH_CURVE_TYPE      1
#define FLASH_SOFT_DIM_ON     2
#define FLASH_POWER_PKT_ON    3
#define FLASH_HB_DEFAULT      4
#define FLASH_HB_ENA          5
#define FLASH_RELAY_STATE     6
#define FLASH_PWM_L           7
#define FLASH_PWM_H           8

extern void flash_erase(void);
extern unsigned int flash_info_copy(void);
extern unsigned int flash_info_update(void);
extern void flash_powerloss_update(void);
extern unsigned int powerloss_copy(void);

extern unsigned char flash_image[FLASH_IMAGE_SIZE];
extern unsigned char powerloss_image[POWER_LOSS_SIZE];
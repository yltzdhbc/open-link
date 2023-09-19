#ifndef __BSP_MCU_H__
#define __BSP_MCU_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define STOP_BOOT_APP_FLAG_VAR (0x1234abcd)

extern uint32_t *stop_boot_app_flag;
extern uint8_t g_app_start;
extern uint8_t g_sn[16];
extern uint16_t g_sn_crc16;

void mcu_software_reset(void);
void mcu_set_stop_app_flag(void);
void mcu_reset_stop_app_flag(void);

void mcu_app_start(void);

void bsp_mcu_info_init(void);

#endif /* __BSP_MCU_H__ */

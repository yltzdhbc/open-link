#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include <stdio.h>
#include <string.h>
#include "gd32f4xx.h"

typedef enum
{
    FLASH_OK = 0,
    FLASH_BUSY = 1,
    FLASH_ERROR,
    FLASH_ERROR_WRP,
    FLASH_ERROR_PROGRAM,
    FLASH_COMPLETE,
    FLASH_TIMEOUT,
    FLASH_PARAMETER_ERROR,
} flash_status;

typedef struct
{
    uint32_t sector_name;       /*!< the name of the sector */
    uint32_t sector_num;        /*!< the number of the sector */
    uint32_t sector_size;       /*!< the size of the sector */
    uint32_t sector_start_addr; /*!< the start address of the sector */
    uint32_t sector_end_addr;   /*!< the end address of the sector */
} fmc_sector_info_struct;

/* sector size */
#define SIZE_16KB ((uint32_t)0x00004000U)  /*!< size of 16KB*/
#define SIZE_64KB ((uint32_t)0x00010000U)  /*!< size of 64KB*/
#define SIZE_128KB ((uint32_t)0x00020000U) /*!< size of 128KB*/
#define SIZE_256KB ((uint32_t)0x00040000U) /*!< size of 256KB*/
/* FMC BANK address */
#define FMC_START_ADDRESS FLASH_BASE                         /*!< FMC start address */
#define FMC_BANK0_START_ADDRESS FMC_START_ADDRESS            /*!< FMC BANK0 start address */
#define FMC_BANK1_START_ADDRESS ((uint32_t)0x08100000U)      /*!< FMC BANK1 start address */
#define FMC_SIZE (*(uint16_t *)0x1FFF7A22U)                  /*!< FMC SIZE */
#define FMC_END_ADDRESS (FLASH_BASE + (FMC_SIZE * 1024) - 1) /*!< FMC end address */
#define FMC_MAX_END_ADDRESS ((uint32_t)0x08300000U)          /*!< FMC maximum end address */

/* FMC error message */
#define FMC_WRONG_SECTOR_NAME ((uint32_t)0xFFFFFFFFU) /*!< wrong sector name*/
#define FMC_WRONG_SECTOR_NUM ((uint32_t)0xFFFFFFFFU)  /*!< wrong sector number*/
#define FMC_INVALID_SIZE ((uint32_t)0xFFFFFFFFU)      /*!< invalid sector size*/
#define FMC_INVALID_ADDR ((uint32_t)0xFFFFFFFFU)      /*!< invalid sector address*/

/*
    sector0   0x0800 0000 - 0x0800 3FFF   16K      bootloader   2 0000
    sector1   0x0800 4000 - 0x0800 7FFF   16K      |
    sector2   0x0800 8000 - 0x0800 BFFF   16K      |
    sector3   0x0800 C000 - 0x0800 FFFF   16K      |
    sector4   0x0801 0000 - 0x0801 FFFF   64K      | bootloader-end
    sector5   0x0802 0000 - 0x0803 FFFF   128K     app          A 0000
    sector6   0x0804 0000 - 0x0805 FFFF   128K     |
    sector7   0x0806 0000 - 0x0807 FFFF   128K     |
    sector8   0x0808 0000 - 0x0809 FFFF   128K     | app-end
    sector9   0x080A 0000 - 0x080B FFFF   128K     |
    ...
    sector10  0x080C 0000 - 0x080D FFFF   128K     params       2 0000
    sector11  0x080E 0000 - 0x080F FFFF   128K     |
*/

#define FMC_PAGE_SIZE ((uint16_t)(0x100U)) // GD32F4 页大小 4k
#define SYS_PARAMS_FLASH_START_ADDR (0x080C0000U)
#define SYS_PARAMS_FLASH_END_ADDR (0x080DFFFFU)
#define SYS_PARAMS_FLASH_LENGTH (0x00020000U)

enum
{
    SYS_PARAM_OK = 0,
    SYS_PARAM_ERROR = 1,
};

#pragma pack(push, 1)
typedef struct
{
    /* system params */
    uint32_t device_id;
    uint32_t build_time;
    uint32_t boot_times;
    /* bootloader params */
    uint32_t loader_ver;
    uint32_t app_ver;
    uint32_t app_size;
    uint8_t app_md5[16];
    /* app params */
} sys_cfg_params_t;
#pragma pack(pop)

extern sys_cfg_params_t g_sys_params;

uint32_t flash_erase(uint32_t addr, uint32_t erase_size);
uint32_t flash_write(uint32_t addr, uint8_t *buf, uint32_t num);

int sys_params_read(void);
int sys_params_save(void);

void bsp_flash_init(void);

#endif /* __BSP_FLASH_H__ */

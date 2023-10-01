
#include "stm32f4xx.h"
#include "stm32f407_bsp_mcu.h"
#include "open_protocol_upgrade.h"
#include "crc.h"

// 用于停止启动的标志(APP和Loader中都不初始化，指定位置0x2020000），重新上电可以被Loader读到
// 如果等于0x1234abcd则停留在Loader中，该变量进入Loader后置0
//uint32_t *stop_boot_app_flag = (uint32_t *)(0x2002FF00);
uint32_t *stop_boot_app_flag = (uint32_t *)(0x2001E000);

uint8_t g_app_start = 0;
uint8_t g_sn[16];
uint16_t g_sn_crc16 = 0;

#define STORAGE_INFO_ADDR 0x1FFF7A20
#define UNIQUE_ID_ADDR 0x1FFF7A10
#define CHIP_SN_BYTES_NUM 12

typedef struct
{
    union
    {
        uint32_t size;
        struct
        {
            uint16_t sram_size;
            uint16_t flash_size;
        };
    } storage;
    uint8_t unique_id[CHIP_SN_BYTES_NUM];
} mcu_info_t;

mcu_info_t mcu_info;

void getMcuInfo(void)
{
    mcu_info.storage.size = *(uint32_t *)(STORAGE_INFO_ADDR);
    for (int i = 0; i < CHIP_SN_BYTES_NUM; i++)
    {
        mcu_info.unique_id[i] = *(uint8_t *)(UNIQUE_ID_ADDR + i);
        if(mcu_info.unique_id[i] == 0)
        {
            mcu_info.unique_id[i] = '.';
        }
        if(mcu_info.unique_id[i] == ' ')
        {
            mcu_info.unique_id[i] = '_';
        }
    }
}

void mcu_sn_read(uint8_t *sn)
{
    getMcuInfo();
    memcpy(sn, &mcu_info.unique_id, CHIP_SN_BYTES_NUM);
}

uint16_t mcu_sn_crc16(void)
{
    uint8_t sn[CHIP_SN_BYTES_NUM];
    uint16_t sn_crc16;
    mcu_sn_read(sn);
    sn_crc16 = crc16_checksum_get(sn, sizeof(sn), 0X4F19);
    return sn_crc16;
}

void mcu_software_reset(void)
{
    /* set FAULTMASK */
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

void mcu_set_stop_app_flag(void)
{
    *stop_boot_app_flag = STOP_BOOT_APP_FLAG_VAR;
}

void mcu_reset_stop_app_flag(void)
{
    *stop_boot_app_flag = 0;
}

#define FLASH_APP1_ADDR 0X08020000
typedef void (*pFunction)(void);

pFunction jump_app;
uint32_t jump_addr = 0;

void mcu_app_start(void)
{
    // 检查栈顶指针是否合法，是否存在app
    if (0x20000000 == ((*(__IO uint32_t *)FLASH_APP1_ADDR & 0x2FFE0000)))
    {
        __disable_irq();

        NVIC_ClearPendingIRQ(SysTick_IRQn);

        __set_MSP(*(__IO uint32_t *)FLASH_APP1_ADDR);
        SCB->VTOR = FLASH_APP1_ADDR;
        jump_addr = *(__IO uint32_t *)(FLASH_APP1_ADDR + 4);
        jump_app = (pFunction)jump_addr;
        jump_app();
    }
    else
    {
        g_app_start = 0;
    }
}

void bsp_mcu_info_init(void)
{
    mcu_sn_read(g_sn);
    g_sn_crc16 = mcu_sn_crc16();
    upgrade_init(g_sn_crc16, (char *)g_sn);
}
#include "stm32f407_bsp_flash.h"
#include "stm32f4xx.h"

sys_cfg_params_t g_sys_params;
uint32_t *sys_params_addr_flag = (uint32_t *)(SYS_PARAMS_FLASH_START_ADDR);

fmc_sector_info_struct fmc_sector_info_get(uint32_t addr)
{
//    fmc_sector_info_struct sector_info;
//    uint32_t temp = 0x00000000U;
//    if ((FMC_START_ADDRESS <= addr) && (FMC_END_ADDRESS >= addr))
//    {
//        if ((FMC_BANK1_START_ADDRESS > addr))
//        {
//            /* bank0 area */
//            temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_16KB;
//            if (4U > temp)
//            {
//                sector_info.sector_name = (uint32_t)temp;
//                sector_info.sector_num = CTL_SN(temp);
//                sector_info.sector_size = SIZE_16KB;
//                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
//                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
//            }
//            else if (8U > temp)
//            {
//                sector_info.sector_name = 0x00000004U;
//                sector_info.sector_num = CTL_SN(4);
//                sector_info.sector_size = SIZE_64KB;
//                sector_info.sector_start_addr = 0x08010000U;
//                sector_info.sector_end_addr = 0x0801FFFFU;
//            }
//            else
//            {
//                temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_128KB;
//                sector_info.sector_name = (uint32_t)(temp + 4);
//                sector_info.sector_num = CTL_SN(temp + 4);
//                sector_info.sector_size = SIZE_128KB;
//                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_128KB * temp);
//                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
//            }
//        }
//        else
//        {
//            /* bank1 area */
//            temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_16KB;
//            if (4U > temp)
//            {
//                sector_info.sector_name = (uint32_t)(temp + 12);
//                sector_info.sector_num = CTL_SN(temp + 16);
//                sector_info.sector_size = SIZE_16KB;
//                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
//                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
//            }
//            else if (8U > temp)
//            {
//                sector_info.sector_name = 0x00000010;
//                sector_info.sector_num = CTL_SN(20);
//                sector_info.sector_size = SIZE_64KB;
//                sector_info.sector_start_addr = 0x08110000U;
//                sector_info.sector_end_addr = 0x0811FFFFU;
//            }
//            else if (64U > temp)
//            {
//                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_128KB;
//                sector_info.sector_name = (uint32_t)(temp + 16);
//                sector_info.sector_num = CTL_SN(temp + 20);
//                sector_info.sector_size = SIZE_128KB;
//                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_128KB * temp);
//                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
//            }
//            else
//            {
//                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_256KB;
//                sector_info.sector_name = (uint32_t)(temp + 20);
//                sector_info.sector_num = CTL_SN(temp + 8);
//                sector_info.sector_size = SIZE_256KB;
//                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_256KB * temp);
//                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_256KB - 1;
//            }
//        }
//    }
//    else
//    {
//        /* invalid address */
//        sector_info.sector_name = FMC_WRONG_SECTOR_NAME;
//        sector_info.sector_num = FMC_WRONG_SECTOR_NUM;
//        sector_info.sector_size = FMC_INVALID_SIZE;
//        sector_info.sector_start_addr = FMC_INVALID_ADDR;
//        sector_info.sector_end_addr = FMC_INVALID_ADDR;
//    }
//    return sector_info;
}

uint32_t flash_erase(uint32_t addr, uint32_t erase_size)
{
//    uint32_t ret = 0;

//    fmc_sector_info_struct sector_info;
//    sector_info = fmc_sector_info_get(addr);

//    if (FMC_WRONG_SECTOR_NAME == sector_info.sector_name)
//    {
//        ret = FLASH_ERROR;
//        return ret;
//    }
//    else
//    {
//        fmc_unlock();
//        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
//        /* wait the erase operation complete*/
//        if (FMC_READY != fmc_sector_erase(sector_info.sector_num))
//        {
//            ret = FLASH_ERROR;
//        }
//        else
//        {
//            ret = FLASH_OK;
//        }
//        fmc_lock();
//        return ret;
//    }
}

uint32_t flash_write(uint32_t addr, uint8_t *buf, uint32_t num)
{
//    fmc_unlock();
//    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

//    for (uint32_t i = 0; i < num; i++)
//    {
//        if (FMC_READY == fmc_byte_program(addr, buf[i]))
//        {
//            addr++;
//        }
//        else
//        {
//            return FLASH_ERROR;
//        }
//    }

//    fmc_lock();

//    return FLASH_OK;
}

int sys_params_read(void)
{
    __disable_irq();
    memcpy(&g_sys_params, (sys_params_addr_flag), sizeof(g_sys_params));
    __enable_irq();

    return SYS_PARAM_OK;
}

int sys_params_save(void)
{
    int ret = SYS_PARAM_OK;

    __disable_irq();

//    /* 擦除Flash */
//    if (FLASH_OK != flash_erase(SYS_PARAMS_FLASH_START_ADDR, SYS_PARAMS_FLASH_LENGTH))
//    {
//        ret = SYS_PARAM_ERROR;
//    }

//    if (FLASH_OK != flash_write(SYS_PARAMS_FLASH_START_ADDR, (uint8_t *)&g_sys_params, sizeof(g_sys_params)))
//    {
//        ret = SYS_PARAM_ERROR;
//    }

    __enable_irq();

    return ret;
}

void bsp_flash_init(void)
{
    sys_params_read();
    g_sys_params.boot_times++;
    sys_params_save();
}

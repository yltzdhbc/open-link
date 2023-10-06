#include "stm32f407_bsp_flash.h"
#include "stm32f4xx.h"

sys_cfg_params_t g_sys_params;
uint32_t *sys_params_addr_flag = (uint32_t *)(SYS_PARAMS_FLASH_START_ADDR);
static uint32_t ger_sector(uint32_t address);
uint32_t get_next_flash_address(uint32_t address);

/**
 * @brief          擦除flash
 * @param[in]      address: flash 地址
 * @param[in]      len: 页数量
 * @retval         none
 */
void flash_erase_address(uint32_t address, uint16_t len)
{
    FLASH_EraseInitTypeDef flash_erase;
    uint32_t error;

    flash_erase.Sector = ger_sector(address);
    flash_erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    flash_erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    flash_erase.NbSectors = len;

    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&flash_erase, &error);
    HAL_FLASH_Lock();
}

/**
 * @brief          往一页flash写数据
 * @param[in]      start_address: flash 地址
 * @param[in]      buf: 数据指针
 * @param[in]      len: 数据长度
 * @retval         success 0, fail -1
 */
int8_t flash_write_single_address(uint32_t start_address, uint32_t *buf, uint32_t len)
{
    static uint32_t uw_address;
    static uint32_t end_address;
    static uint32_t *data_buf;
    static uint32_t data_len;

    HAL_FLASH_Unlock();

    uw_address = start_address;
    end_address = get_next_flash_address(start_address);
    data_buf = buf;
    data_len = 0;

    while (uw_address <= end_address)
    {

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, uw_address, *data_buf) == HAL_OK)
        {
            uw_address += 4;
            data_buf++;
            data_len++;
            if (data_len == len)
            {
                break;
            }
        }
        else
        {
            HAL_FLASH_Lock();
            return -1;
        }
    }

    HAL_FLASH_Lock();
    return 0;
}

/**
 * @brief          往几页flash写数据
 * @param[in]      start_address: flash 开始地址
 * @param[in]      end_address: flash 结束地址
 * @param[in]      buf: 数据指针
 * @param[in]      len: 数据长度
 * @retval         success 0, fail -1
 */
int8_t flash_write_muli_address(uint32_t start_address, uint32_t end_address, uint32_t *buf, uint32_t len)
{
    uint32_t uw_address = 0;
    uint32_t *data_buf;
    uint32_t data_len;

    HAL_FLASH_Unlock();

    uw_address = start_address;
    data_buf = buf;
    data_len = 0;
    while (uw_address <= end_address)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, uw_address, *data_buf) == HAL_OK)
        {
            uw_address += 4;
            data_buf++;
            data_len++;
            if (data_len == len)
            {
                break;
            }
        }
        else
        {
            HAL_FLASH_Lock();
            return -1;
        }
    }

    HAL_FLASH_Lock();
    return 0;
}

/**
 * @brief          从flash读数据
 * @param[in]      start_address: flash 地址
 * @param[out]     buf: 数据指针
 * @param[in]      len: 数据长度
 * @retval         none
 */
void flash_read(uint32_t address, uint32_t *buf, uint32_t len)
{
    memcpy(buf, (void *)address, len * 4);
}

/**
 * @brief          获取flash的sector号
 * @param[in]      address: flash 地址
 * @retval         sector号
 */
uint32_t ger_sector(uint32_t address)
{
    uint32_t sector = 0;
    if ((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_SECTOR_0;
    }
    else if ((address < ADDR_FLASH_SECTOR_2) && (address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_SECTOR_1;
    }
    else if ((address < ADDR_FLASH_SECTOR_3) && (address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_SECTOR_2;
    }
    else if ((address < ADDR_FLASH_SECTOR_4) && (address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_SECTOR_3;
    }
    else if ((address < ADDR_FLASH_SECTOR_5) && (address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_SECTOR_4;
    }
    else if ((address < ADDR_FLASH_SECTOR_6) && (address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_SECTOR_5;
    }
    else if ((address < ADDR_FLASH_SECTOR_7) && (address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_SECTOR_6;
    }
    else if ((address < ADDR_FLASH_SECTOR_8) && (address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_SECTOR_7;
    }
    else if ((address < ADDR_FLASH_SECTOR_9) && (address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_SECTOR_8;
    }
    else if ((address < ADDR_FLASH_SECTOR_10) && (address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_SECTOR_9;
    }
    else if ((address < ADDR_FLASH_SECTOR_11) && (address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_SECTOR_10;
    }
    else if ((address < ADDR_FLASH_SECTOR_12) && (address >= ADDR_FLASH_SECTOR_11))
    {
        sector = FLASH_SECTOR_11;
    }
    else
    {
        sector = FLASH_SECTOR_11;
    }

    return sector;
}

/**
 * @brief          获取下一页flash地址
 * @param[in]      address: flash 地址
 * @retval         下一页flash地址
 */
uint32_t get_next_flash_address(uint32_t address)
{
    uint32_t sector = 0;

    if ((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0))
    {
        sector = ADDR_FLASH_SECTOR_1;
    }
    else if ((address < ADDR_FLASH_SECTOR_2) && (address >= ADDR_FLASH_SECTOR_1))
    {
        sector = ADDR_FLASH_SECTOR_2;
    }
    else if ((address < ADDR_FLASH_SECTOR_3) && (address >= ADDR_FLASH_SECTOR_2))
    {
        sector = ADDR_FLASH_SECTOR_3;
    }
    else if ((address < ADDR_FLASH_SECTOR_4) && (address >= ADDR_FLASH_SECTOR_3))
    {
        sector = ADDR_FLASH_SECTOR_4;
    }
    else if ((address < ADDR_FLASH_SECTOR_5) && (address >= ADDR_FLASH_SECTOR_4))
    {
        sector = ADDR_FLASH_SECTOR_5;
    }
    else if ((address < ADDR_FLASH_SECTOR_6) && (address >= ADDR_FLASH_SECTOR_5))
    {
        sector = ADDR_FLASH_SECTOR_6;
    }
    else if ((address < ADDR_FLASH_SECTOR_7) && (address >= ADDR_FLASH_SECTOR_6))
    {
        sector = ADDR_FLASH_SECTOR_7;
    }
    else if ((address < ADDR_FLASH_SECTOR_8) && (address >= ADDR_FLASH_SECTOR_7))
    {
        sector = ADDR_FLASH_SECTOR_8;
    }
    else if ((address < ADDR_FLASH_SECTOR_9) && (address >= ADDR_FLASH_SECTOR_8))
    {
        sector = ADDR_FLASH_SECTOR_9;
    }
    else if ((address < ADDR_FLASH_SECTOR_10) && (address >= ADDR_FLASH_SECTOR_9))
    {
        sector = ADDR_FLASH_SECTOR_10;
    }
    else if ((address < ADDR_FLASH_SECTOR_11) && (address >= ADDR_FLASH_SECTOR_10))
    {
        sector = ADDR_FLASH_SECTOR_11;
    }
    else /*(address < FLASH_END_ADDR) && (address >= ADDR_FLASH_SECTOR_23))*/
    {
        sector = FLASH_END_ADDR;
    }
    return sector;
}

uint32_t flash_erase(uint32_t addr, uint32_t erase_size)
{
    uint32_t start_sector = 0;
    uint32_t end_sector = 0;
    uint32_t erase_sector_num = 0;

    start_sector = ger_sector(addr);
    end_sector = ger_sector(addr + erase_size);
    erase_sector_num = end_sector - start_sector;

    if (erase_sector_num < 0)
    {
        return FLASH_ERROR;
    }
    if (erase_sector_num == 0)
    {
        erase_sector_num = 1;
        flash_erase_address(addr, erase_sector_num);
    }
    else
    {
        flash_erase_address(addr, erase_sector_num);
    }

    return FLASH_OK;
}

uint32_t flash_write(uint32_t addr, uint8_t *buf, uint32_t num)
{
    HAL_FLASH_Unlock();
    
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGAERR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGPERR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR);

    for (uint32_t i = 0; i < num; i++)
    {
        if (HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, buf[i]))
        {
            addr++;
        }
        else
        {
            HAL_FLASH_Lock();
            return FLASH_ERROR;
        }
    }

    HAL_FLASH_Lock();
    return FLASH_OK;
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

    if (FLASH_OK != flash_erase(SYS_PARAMS_FLASH_START_ADDR, SYS_PARAMS_FLASH_LENGTH))
    {
        ret = SYS_PARAM_ERROR;
    }

    if (FLASH_OK != flash_write(SYS_PARAMS_FLASH_START_ADDR, (uint8_t *)&g_sys_params, sizeof(g_sys_params)))
    {
        ret = SYS_PARAM_ERROR;
    }

    __enable_irq();

    return ret;
}

void bsp_flash_init(void)
{
    sys_params_read();
    
    if(g_sys_params.magic_flag != 0x0a0a0a0a)
    {
        memset(&g_sys_params, 0, sizeof(sys_cfg_params_t));
        g_sys_params.magic_flag = 0x0a0a0a0a;
        sys_params_save();
    }
    else
    {
        g_sys_params.boot_times++;
        sys_params_save();
    }
}

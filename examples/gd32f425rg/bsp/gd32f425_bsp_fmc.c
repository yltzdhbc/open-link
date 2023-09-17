#include "gd32f425_bsp_fmc.h"
#include "gd32f4xx.h"

/*!
    \brief      get the sector number, size and range of the given address
    \param[in]  address: The flash address
    \param[out] none
    \retval     fmc_sector_info_struct: The information of a sector
*/
fmc_sector_info_struct fmc_sector_info_get(uint32_t addr)
{
    fmc_sector_info_struct sector_info;
    uint32_t temp = 0x00000000U;
    if ((FMC_START_ADDRESS <= addr) && (FMC_END_ADDRESS >= addr))
    {
        if ((FMC_BANK1_START_ADDRESS > addr))
        {
            /* bank0 area */
            temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_16KB;
            if (4U > temp)
            {
                sector_info.sector_name = (uint32_t)temp;
                sector_info.sector_num = CTL_SN(temp);
                sector_info.sector_size = SIZE_16KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
            }
            else if (8U > temp)
            {
                sector_info.sector_name = 0x00000004U;
                sector_info.sector_num = CTL_SN(4);
                sector_info.sector_size = SIZE_64KB;
                sector_info.sector_start_addr = 0x08010000U;
                sector_info.sector_end_addr = 0x0801FFFFU;
            }
            else
            {
                temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_128KB;
                sector_info.sector_name = (uint32_t)(temp + 4);
                sector_info.sector_num = CTL_SN(temp + 4);
                sector_info.sector_size = SIZE_128KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_128KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
            }
        }
        else
        {
            /* bank1 area */
            temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_16KB;
            if (4U > temp)
            {
                sector_info.sector_name = (uint32_t)(temp + 12);
                sector_info.sector_num = CTL_SN(temp + 16);
                sector_info.sector_size = SIZE_16KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
            }
            else if (8U > temp)
            {
                sector_info.sector_name = 0x00000010;
                sector_info.sector_num = CTL_SN(20);
                sector_info.sector_size = SIZE_64KB;
                sector_info.sector_start_addr = 0x08110000U;
                sector_info.sector_end_addr = 0x0811FFFFU;
            }
            else if (64U > temp)
            {
                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_128KB;
                sector_info.sector_name = (uint32_t)(temp + 16);
                sector_info.sector_num = CTL_SN(temp + 20);
                sector_info.sector_size = SIZE_128KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_128KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
            }
            else
            {
                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_256KB;
                sector_info.sector_name = (uint32_t)(temp + 20);
                sector_info.sector_num = CTL_SN(temp + 8);
                sector_info.sector_size = SIZE_256KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_256KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_256KB - 1;
            }
        }
    }
    else
    {
        /* invalid address */
        sector_info.sector_name = FMC_WRONG_SECTOR_NAME;
        sector_info.sector_num = FMC_WRONG_SECTOR_NUM;
        sector_info.sector_size = FMC_INVALID_SIZE;
        sector_info.sector_start_addr = FMC_INVALID_ADDR;
        sector_info.sector_end_addr = FMC_INVALID_ADDR;
    }
    return sector_info;
}

/*!
    \brief      get the sector number by a given sector name
    \param[in]  address: a given sector name
    \param[out] none
    \retval     uint32_t: sector number
*/
uint32_t sector_name_to_number(uint32_t sector_name)
{
    if (11 >= sector_name)
    {
        return CTL_SN(sector_name);
    }
    else if (23 >= sector_name)
    {
        return CTL_SN(sector_name + 4);
    }
    else if (27 >= sector_name)
    {
        return CTL_SN(sector_name - 12);
    }
    else
    { // printf
        while (1)
            ;
    }
}

/*!
    \brief      erases the sector of a given address
    \param[in]  address: a given address
    \param[out] none
    \retval     none
*/
void fmc_erase_sector_by_address(uint32_t address)
{
    fmc_sector_info_struct sector_info;
    // printf("\r\nFMC erase operation:\n");
    /* get information about the sector in which the specified address is located */
    sector_info = fmc_sector_info_get(address);
    if (FMC_WRONG_SECTOR_NAME == sector_info.sector_name)
    {
        // printf("\r\nWrong address!\n");
        while (1)
            ;
    }
    else
    {
        // printf("\r\nErase start ......\n");
        /* unlock the flash program erase controller */
        fmc_unlock();
        /* clear pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
        /* wait the erase operation complete*/
        if (FMC_READY != fmc_sector_erase(sector_info.sector_num))
        {
            while (1)
                ;
        }
        /* lock the flash program erase controller */
        fmc_lock();
        // printf("\r\nAddress 0x%08X is located in the : SECTOR_NUMBER_%d !\n", address, sector_info.sector_name);
        // printf("\r\nSector range: 0x%08X to 0x%08X\n", sector_info.sector_start_addr, sector_info.sector_end_addr);
        // printf("\r\nSector size: %d KB\n", (sector_info.sector_size/1024));
        // printf("\r\nErase success!\n");
        // printf("\r\n");
    }
}

/*!
    \brief      write 32 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_32: data pointer
    \param[out] none
    \retval     none
*/
void fmc_write_32bit_data(uint32_t address, uint16_t length, int32_t *data_32)
{
    fmc_sector_info_struct start_sector_info;
    fmc_sector_info_struct end_sector_info;
    uint32_t sector_num, i;

    // printf("\r\nFMC word programe operation:\n");
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* get the information of the start and end sectors */
    start_sector_info = fmc_sector_info_get(address);
    end_sector_info = fmc_sector_info_get(address + 4 * length);
    /* erase sector */
    for (i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++)
    {
        sector_num = sector_name_to_number(i);
        if (FMC_READY != fmc_sector_erase(sector_num))
        {
            while (1)
                ;
        }
    }

    /* write data_32 to the corresponding address */
    for (i = 0; i < length; i++)
    {
        if (FMC_READY == fmc_word_program(address, data_32[i]))
        {
            address = address + 4;
        }
        else
        {
            while (1)
                ;
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
    // printf("\r\nWrite complete!\n");
    // printf("\r\n");
}

/*!
    \brief      read 32 bit length data from a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_32: data pointer
    \param[out] none
    \retval     none
*/
void fmc_read_32bit_data(uint32_t address, uint16_t length, int32_t *data_32)
{
    uint8_t i;
    // printf("\r\nRead data from 0x%08X\n", address);
    // printf("\r\n");
    for (i = 0; i < length; i++)
    {
        data_32[i] = *(__IO int32_t *)address;
        // printf("0x%08X  ", data_32[i]);
        address = address + 4;
    }
    // printf("\r\nRead end\n");
    // printf("\r\n");
}

/*!
    \brief      write 16 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_16: data pointer
    \param[out] none
    \retval     none
*/
void fmc_write_16bit_data(uint32_t address, uint16_t length, int16_t *data_16)
{
    fmc_sector_info_struct start_sector_info;
    fmc_sector_info_struct end_sector_info;
    uint32_t sector_num, i;

    // printf("\r\nFMC half_word program operation:\n");
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* get the information of the start and end sectors */
    start_sector_info = fmc_sector_info_get(address);
    end_sector_info = fmc_sector_info_get(address + 2 * length);
    /* erase sector */
    for (i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++)
    {
        sector_num = sector_name_to_number(i);
        if (FMC_READY != fmc_sector_erase(sector_num))
        {
            while (1)
                ;
        }
    }

    /* write data_16 to the corresponding address */
    for (i = 0; i < length; i++)
    {
        if (FMC_READY == fmc_halfword_program(address, data_16[i]))
        {
            address = address + 2;
        }
        else
        {
            while (1)
                ;
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
    // printf("\r\nWrite complete!\n");
    // printf("\r\n");
}

/*!
    \brief      read 16 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_16: data pointer
    \param[out] none
    \retval     none
*/
void fmc_read_16bit_data(uint32_t address, uint16_t length, int16_t *data_16)
{
    uint8_t i;
    // printf("\r\nRead data from 0x%04X\n", address);
    // printf("\r\n");
    for (i = 0; i < length; i++)
    {
        data_16[i] = *(__IO int16_t *)address;
        // printf("0x%04X  ", data_16[i]);
        address = address + 2;
    }
    // printf("\r\nRead end\n");
    // printf("\r\n");
}

/*!
    \brief      write 8 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_8: data pointer
    \param[out] none
    \retval     none
*/
void fmc_write_8bit_data(uint32_t address, uint16_t length, int8_t *data_8)
{
    fmc_sector_info_struct start_sector_info;
    fmc_sector_info_struct end_sector_info;
    uint32_t sector_num, i;

    // printf("\r\nFMC half_word program operation:\n");
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* get the information of the start and end sectors */
    start_sector_info = fmc_sector_info_get(address);
    end_sector_info = fmc_sector_info_get(address + 2 * length);
    /* erase sector */
    for (i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++)
    {
        sector_num = sector_name_to_number(i);
        if (FMC_READY != fmc_sector_erase(sector_num))
        {
            while (1)
                ;
        }
    }

    /* write data_8 to the corresponding address */
    for (i = 0; i < length; i++)
    {
        if (FMC_READY == fmc_byte_program(address, data_8[i]))
        {
            address++;
        }
        else
        {
            while (1)
                ;
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
    // printf("\r\nWrite complete!\n");
    // printf("\r\n");
}

/*!
    \brief      read 8 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_8: data pointer
    \param[out] none
    \retval     none
*/
void fmc_read_8bit_data(uint32_t address, uint16_t length, int8_t *data_8)
{
    uint8_t i;
    // printf("\r\nRead data from 0x%02X\n", address);
    // printf("\r\n");
    for (i = 0; i < length; i++)
    {
        data_8[i] = *(__IO int8_t *)address;
        // printf("0x%02X  ", data_8[i]);
        address++;
    }
    // printf("\r\nRead end\n");
    // printf("\r\n");
}

#define ERASE_ADDRESS ((uint32_t)0x08004000)
#define WRITE_ADDRESS_WORD ((uint32_t)0x08008000)
#define WRITE_ADDRESS_HALF_WORD ((uint32_t)0x0800C000)
#define WRITE_ADDRESS_BYTE ((uint32_t)0x08010000)

int32_t data_32_1[10] = {0x5555AAAA, 0x5555AAAA, 0x5555AAAA, 0x5555AAAA, 0x5555AAAA, 0x5555AAAA, 0x5555AAAA, 0x5555AAAA, 0x5555AAAA, 0x5555AAAA};
int32_t data_32_2[10];
int16_t data_16_1[10] = {
    0x44BB,
    0x44BB,
    0x44BB,
    0x44BB,
    0x44BB,
    0x44BB,
    0x44BB,
    0x44BB,
    0x44BB,
    0x44BB,
};
int16_t data_16_2[10];
int8_t data_8_1[10] = {0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D};
int8_t data_8_2[10];


sys_cfg_params_t g_sys_params;


int sys_params_save()
{

}


int sys_params_read()
{
    
}

void bsp_fmc_init(void)
{

    sys_params_read();
    g_sys_params.boot_times++;
    sys_params_save();


    // /* erases the sector of a given sector number */
    // fmc_erase_sector_by_address(ERASE_ADDRESS);

    // /* write 32 bit length data to a given address */
    // fmc_write_32bit_data(WRITE_ADDRESS_WORD, 10, data_32_1);
    // /* read 32 bit length data from a given address */
    // fmc_read_32bit_data(WRITE_ADDRESS_WORD, 10, data_32_2);

    // /* write 16 bit length data to a given address */
    // fmc_write_16bit_data(WRITE_ADDRESS_HALF_WORD, 10, data_16_1);
    // /* read 16 bit length data from a given address */
    // fmc_read_16bit_data(WRITE_ADDRESS_HALF_WORD, 10, data_16_2);

    // /* write 8 bit length data to a given address */
    // fmc_write_8bit_data(WRITE_ADDRESS_BYTE, 10, data_8_1);
    // /* read 8 bit length data from a given address */
    // fmc_read_8bit_data(WRITE_ADDRESS_BYTE, 10, data_8_2);
}

uint32_t flash_erase(uint32_t addr, uint32_t erase_size)
{
    uint32_t ret = 0;

    fmc_sector_info_struct sector_info;
    sector_info = fmc_sector_info_get(addr);

    if (FMC_WRONG_SECTOR_NAME == sector_info.sector_name)
    {
        ret = FLASH_ERROR;
        return ret;
    }
    else
    {
        fmc_unlock();
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
        /* wait the erase operation complete*/
        if (FMC_READY != fmc_sector_erase(sector_info.sector_num))
        {
            ret = FLASH_ERROR;
        }
        else
        {
            ret = FLASH_OK;
        }
        fmc_lock();
        return ret;
    }
}

uint32_t flash_write(uint32_t addr, uint8_t *buf, uint32_t num)
{
    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

    for (uint32_t i = 0; i < num; i++)
    {
        if (FMC_READY == fmc_byte_program(addr, buf[i]))
        {
            addr++;
        }
        else
        {
            return FLASH_ERROR;
        }
    }

    fmc_lock();

    return FLASH_OK;
}

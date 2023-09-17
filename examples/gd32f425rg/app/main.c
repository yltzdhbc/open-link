#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"

#include "gd32f425_bsp_gpio.h"
#include "gd32f425_bsp_uart.h"
#include "gd32f425_bsp_can.h"
#include "gd32f425_bsp_fmc.h"
#include "gd32f425_bsp_mcu.h"
#include "app_protocol.h"

uint32_t time_now = 0;
uint32_t time_last[3] = {0};

int main(void)
{
#ifdef FIRMWARE_APP
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x20000);
    __enable_irq();
#endif

    systick_config();

    bsp_can_init();
    bsp_fmc_init();
    bsp_uart_init();
    bsp_gpio_init();

    bsp_mcu_info_init();

    app_protocol_init();

#ifdef FIRMWARE_LOADER
    gpio_bit_reset(GPIOB, GPIO_PIN_4);
    gpio_bit_set(GPIOB, GPIO_PIN_5);
    if (*stop_boot_app_flag == STOP_BOOT_APP_FLAG_VAR)
    {
        g_app_start = 0;
        *stop_boot_app_flag = 0;
    }
#endif

    while (1)
    {
        time_now = sys_tick_ms_get();

        if (time_now - time_last[0] > 2)
        {
            time_last[0] = time_now;
            app_protocol_loop();
        }

#ifdef FIRMWARE_LOADER
        if (time_now - time_last[2] > 10)
        {
            time_last[2] = time_now;
            if (g_app_start == 1 && time_now > 1000)
            {
                gpio_bit_set(GPIOB, GPIO_PIN_4);
                mcu_app_start();
            }
            gpio_bit_toggle(GPIOB, GPIO_PIN_4);
        }
#else
        if (time_now - time_last[2] > 100)
        {
            time_last[2] = time_now;
            gpio_bit_toggle(GPIOB, GPIO_PIN_5);
        }
#endif
    }
}

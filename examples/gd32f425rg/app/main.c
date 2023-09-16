#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"

#include "gd32f425_bsp_gpio.h"
#include "gd32f425_bsp_uart.h"
#include "gd32f425_bsp_can.h"
#include "gd32f425_bsp_fmc.h"
#include "app_protocol.h"

uint8_t g_app_start = 1;

uint32_t time_now = 0;
uint32_t time_last[3] = {0};

int main(void)
{
    systick_config();

    bsp_can_init();
    bsp_fmc_init();
    bsp_uart_init();
    bsp_gpio_init();

    app_protocol_init();

    while (1)
    {
        time_now = sys_tick_ms_get();

        if (time_now - time_last[0] > 2)
        {
            time_last[0] = time_now;
            app_protocol_loop();
        }

        if (time_now - time_last[1] > 100)
        {
            time_last[1] = time_now;
            gpio_bit_toggle(GPIOB, GPIO_PIN_4);
            gpio_bit_toggle(GPIOB, GPIO_PIN_5);
        }
    }
}

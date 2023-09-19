
#include "stm32f4xx_hal.h"
#include "stm32f407_bsp_uart.h"
#include "usart.h"

#include "ringbuffer.h"

#define RX_FIFO_SIZE 1024
uint8_t rb_uart0_buffer[RX_FIFO_SIZE];
ring_buffer_t rb_uart0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t data;
    HAL_UART_Receive_IT(&huart1, &data, sizeof(uint8_t));
    ring_buffer_queue(&rb_uart0, data);
}

void uart0_send(uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit(&huart1, data, len, 2);
}

uint16_t uart0_receive(uint8_t *buff, uint16_t len)
{
    uint16_t readlen = ring_buffer_dequeue_arr(&rb_uart0, buff, len);
    return readlen;
}

void uart0_init(void)
{
    ring_buffer_init(&rb_uart0, rb_uart0_buffer, sizeof(rb_uart0_buffer));
}

void bsp_uart_init(void)
{
    uart0_init();
}

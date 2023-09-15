
#include "gd32f4xx.h"
#include "gd32f425_bsp_uart.h"
#include "ringbuffer.h"

#define RX_FIFO_SIZE 256
uint8_t rb_uart0_buffer[RX_FIFO_SIZE];
ring_buffer_t rb_uart0;

void USART0_IRQHandler(void)
{
    if ((RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)) &&
        (RESET != usart_flag_get(USART0, USART_FLAG_RBNE)))
    {
        usart_flag_clear(USART0, USART_FLAG_RBNE);
        uint8_t data = usart_data_receive(USART0);
        ring_buffer_queue(&rb_uart0, data);
    }
}

void uart0_send(uint8_t *data, uint16_t len)
{
    for (int i = 0; i < len; i++)
    {
        usart_data_transmit(USART0, (uint32_t)data[i]);
        while (RESET == usart_flag_get(USART0, USART_FLAG_TBE))
            ;
    }
}

uint16_t uart0_receive(uint8_t *buff, uint16_t len)
{
    uint16_t readlen = ring_buffer_dequeue_arr(&rb_uart0, buff, len);
    return readlen;
}

void uart0_init(void)
{
    ring_buffer_init(&rb_uart0, rb_uart0_buffer, sizeof(rb_uart0_buffer));

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);

    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);

    nvic_irq_enable(USART0_IRQn, 0, 0);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
}

void bsp_uart_init(void)
{
    uart0_init();
}

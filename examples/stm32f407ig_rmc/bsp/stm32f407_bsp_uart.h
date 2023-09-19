#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include <stdint.h>
#include <stdio.h>

void uart0_send(uint8_t *data, uint16_t len);
uint16_t uart0_receive(uint8_t *buff, uint16_t len);
void bsp_uart_init(void);

#endif /* __BSP_UART_H__ */

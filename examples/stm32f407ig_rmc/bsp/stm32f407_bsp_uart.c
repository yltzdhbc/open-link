
#include "stm32f4xx_hal.h"
#include "stm32f407_bsp_uart.h"
#include "usart.h"

#include "ringbuffer.h"

#define RX_FIFO_SIZE 1024
uint8_t rb_uart0_buffer[RX_FIFO_SIZE];
ring_buffer_t rb_uart0;
uint8_t uasrt1_rxdata;

uint8_t Rx1Buf[128];

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     // put to buffer
//     ring_buffer_queue(&rb_uart0, uasrt1_rxdata);
//     // restart it receive
//     HAL_UART_Receive_IT(&huart1, &uasrt1_rxdata, 1);
// }

//  DMA加串口空闲中断
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
//	if(huart->Instance == USART1)
//	{
//        uint16_t recv_len = 128 - __HAL_DMA_GET_COUNTER(huart->hdmarx);
//        ring_buffer_queue_arr(&rb_uart0, Rx1Buf, recv_len);
//        memset(Rx1Buf, 0x00, sizeof(Rx1Buf));
//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, Rx1Buf, sizeof(Rx1Buf));
//	}
//}

void USART1_USER_IRQHandler(void)
{
    volatile uint8_t receive;
    // receive interrupt 接收中断
    if (huart1.Instance->SR & UART_FLAG_RXNE)
    {
        receive = huart1.Instance->DR;
        ring_buffer_queue(&rb_uart0, receive);
    }
    // // idle interrupt 空闲中断
    // else if (huart1.Instance->SR & UART_FLAG_IDLE)
    // {
    //     receive = huart1.Instance->DR;
    //     HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
    // }
}

void uart0_send(uint8_t *data, uint16_t len)
{
    //HAL_UART_Transmit(&huart1, data, len, 5000);
    //HAL_UART_Transmit_IT(&huart1, data, len);
    
    HAL_UART_Transmit_DMA(&huart1, data, len);
}

uint16_t uart0_receive(uint8_t *buff, uint16_t len)
{
    uint16_t readlen = ring_buffer_dequeue_arr(&rb_uart0, buff, len);
    return readlen;
}

void uart0_init(void)
{
    ring_buffer_init(&rb_uart0, rb_uart0_buffer, sizeof(rb_uart0_buffer));
    
    //HAL_UARTEx_ReceiveToIdle_DMA(&huart1, Rx1Buf, 128); //串口1开启DMA接受

    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); // receive interrupt
    
    //    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);  //idle interrupt

    // HAL_UART_Receive_IT(&huart1, &uasrt1_rxdata, 1);
}

void bsp_uart_init(void)
{
    uart0_init();
}

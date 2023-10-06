#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "ringbuffer.h"

#define MODEL_SLAVE

typedef struct
{
    uint16_t can_id;
    ring_buffer_t rb;
} ST_CAN_RX;

typedef struct
{
    uint16_t can_id;
    uint16_t data_len;
    ring_buffer_t rb;
} ST_CAN_TX;

typedef struct
{
    uint16_t std_id;
    uint8_t dlc;
    uint8_t data[8];
} can_frame_t;

uint16_t can1_receive(uint8_t *buf, uint16_t buf_size);
void can1_send(uint8_t *buf, uint16_t len);
void bsp_can_init(void);

#endif /* __BSP_CAN_H__ */

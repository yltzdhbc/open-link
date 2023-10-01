
#include "stm32f4xx.h"
#include "stm32f407_bsp_can.h"

#include "can.h"
#define CAN1_TX_IRQ_DISABLE __HAL_CAN_DISABLE_IT(&hcan1, CAN_IT_TX_MAILBOX_EMPTY)
#define CAN1_TX_IRQ_ENABLE  __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_TX_MAILBOX_EMPTY)

#define CAN1_TX_FALG_CLEAR  __HAL_CAN_CLEAR_FLAG(&hcan1, CAN_FLAG_TXOK0)

/** bsp_can逻辑
 * 发送接口 can1_send(): 将字节流通过messagequeue的方式传递给CAN发送线程，CAN发送线程进行分包发送
 * 接收接口 can1_recv(): 读取缓存区中的CAN报文字节流数据，该收据由CAN接收中断收到报文后填充
 *
 */

#define CAN_RB_FIFO_SIZE 256
static uint8_t rb_can1_tx_buffer[CAN_RB_FIFO_SIZE];
static uint8_t rb_can1_rx_buffer[CAN_RB_FIFO_SIZE];

ring_buffer_t *can1_rb;

ST_CAN_RX can1_rx[3];
ST_CAN_TX can1_tx[3];

uint8_t can_busy = 0;
uint32_t can_send_error_cnt;

static int can1_recv_frame_port(can_frame_t *frame);
static void can1_send_frame_port(can_frame_t *frame);

// open_protocol port----------------------------------------------------------------------------------
void can1_send(uint8_t *buf, uint16_t len)
{
    uint32_t rb_len;
    can_frame_t tx_frame;

    // 将所有data数据加入到环形缓冲中
    
    //ring_buffer_queue_arr(can1_rb, (uint8_t *)buf, len);
    
    ring_buffer_queue_arr(&can1_tx[0].rb, (uint8_t *)buf, len);

    rb_len = ring_buffer_num_items(&can1_tx[0].rb);
    if ((rb_len != 0) && (can_busy == 0) && (len > 0))
    {
        can_busy = 1;
        rb_len = (rb_len > 8) ? 8 : rb_len;
        ring_buffer_dequeue_arr(&can1_tx[0].rb, (uint8_t *)&tx_frame.data[0], rb_len);
        tx_frame.std_id = can1_tx[0].can_id;
        tx_frame.dlc = rb_len;
        can1_send_frame_port(&tx_frame);
    }
    else
    {
        can_busy = 0;
        // can_interrupt_disable(CAN0, CAN_INT_TME);
        CAN1_TX_IRQ_DISABLE;
    }
}

static __inline uint16_t can1_receive_index(uint8_t n, uint8_t *buf, uint16_t buf_size)
{
    uint16_t len, readlen;
    len = ring_buffer_num_items(&can1_rx[n].rb);
    if (len > 0)
    {
        len = (len > buf_size) ? buf_size : len;
        readlen = ring_buffer_dequeue_arr(&can1_rx[n].rb, (uint8_t *)buf, len);
    }
    else
    {
        readlen = 0;
    }
    return readlen;
}
uint16_t can1_receive(uint8_t *buf, uint16_t buf_size)
{
    return can1_receive_index(0, buf, buf_size);
}

// can tx rx port----------------------------------------------------------------------------------
void can1_rx_int_handle(void)
{
    can_frame_t rx_frame;
    if (can1_recv_frame_port(&rx_frame) == 0)
    {
        for (uint8_t n = 0; n < (sizeof(can1_rx) / sizeof(can1_rx[0])); n++)
        {
            if (rx_frame.std_id == can1_rx[n].can_id)
            {
                ring_buffer_queue_arr(&can1_rx[n].rb, (uint8_t *)&rx_frame.data[0], rx_frame.dlc);
                break;
            }
        }
    }
}

void can1_tx_int_handle(void)
{
    uint32_t rb_len;
    can_frame_t tx_frame;

    rb_len = ring_buffer_num_items(&can1_tx[0].rb);
    if (rb_len != 0)
    {
        rb_len = (rb_len > 8) ? 8 : rb_len;
        ring_buffer_dequeue_arr(&can1_tx[0].rb, (uint8_t *)&tx_frame.data[0], rb_len);
        tx_frame.std_id = can1_tx[0].can_id;
        tx_frame.dlc = rb_len;
        can1_send_frame_port(&tx_frame);
    }
    else
    {
        can_busy = 0;
        //can_interrupt_disable(CAN0, CAN_INT_TME);
        CAN1_TX_IRQ_DISABLE;
    }
}

// user port----------------------------------------------------------------------------------
// 提供给其他简单接受CAN的部分使用
//_weak void can1_rx_callback_hook(struct rt_can_msg *rx_msg)
//{
//}

static int can1_recv_frame_port(can_frame_t *frame)
{
    uint8_t rx_data[8];
    CAN_RxHeaderTypeDef rx_header;
    HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header, rx_data);

    // can_receive_message_struct rx_message;
    // can_message_receive(CAN0, CAN_FIFO0, &rx_message);

    frame->std_id = rx_header.StdId;
    frame->dlc = rx_header.DLC;
    memcpy(frame->data, rx_data, rx_header.DLC);

    return 0;
}

static void can1_send_frame_port(can_frame_t *frame)
{
    CAN1_TX_IRQ_ENABLE;

    uint8_t tx_data[8];
    // uint32_t tx_mailbox;
    CAN_TxHeaderTypeDef tx_header;

    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.StdId = frame->std_id;
    tx_header.DLC = frame->dlc;
    memcpy(&tx_data[0], &frame->data[0], frame->dlc);
    if (HAL_OK != HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data, (uint32_t*)CAN_TX_MAILBOX0))
    {
        can_send_error_cnt++;
    }
}

/*gd32 can*/
// void CAN0_TX_IRQHandler(void)
// {
//     // can_flag_clear(CAN0, CAN_FLAG_MTF0);
//     CAN1_TX_FALG_CLEAR;
//     can1_rx_int_handle();
// }

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
    CAN1_TX_FALG_CLEAR;
    can1_tx_int_handle();
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    can1_rx_int_handle();
}

// void CAN0_RX0_IRQHandler(void)
// {
//     can1_rx_int_handle();
// }


// user init----------------------------------------------------------------------------------
void bsp_can1_init(void)
{
    can1_tx[0].can_id = 0X021;
    can1_rx[0].can_id = 0X020;
    
    ring_buffer_init(&can1_tx[0].rb, rb_can1_tx_buffer, sizeof(rb_can1_tx_buffer));
    ring_buffer_init(&can1_rx[0].rb, rb_can1_rx_buffer, sizeof(rb_can1_rx_buffer));
    
    //ring_buffer_init(can1_rb, rb_can1_tx_buffer, sizeof(rb_can1_tx_buffer));
    

    CAN_FilterTypeDef can_filter_st;
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 0;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

    /* enable CAN receive FIFO1 not empty interrupt */
    // can_interrupt_enable(CAN0, CAN_INT_RFNE0);
    CAN1_TX_IRQ_ENABLE;
}

void bsp_can_init(void)
{
    bsp_can1_init();
    //    bsp_can2_init();
}

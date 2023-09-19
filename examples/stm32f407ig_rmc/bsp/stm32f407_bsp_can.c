
#include "gd32f4xx.h"
#include "gd32f425_bsp_can.h"

/** bsp_can逻辑
 * 发送接口 can1_send(): 将字节流通过messagequeue的方式传递给CAN发送线程，CAN发送线程进行分包发送
 * 接收接口 can1_recv(): 读取缓存区中的CAN报文字节流数据，该收据由CAN接收中断收到报文后填充
 *
 */

#define CAN_RB_FIFO_SIZE 256
uint8_t rb_can1_tx_buffer[CAN_RB_FIFO_SIZE];
uint8_t rb_can1_rx_buffer[CAN_RB_FIFO_SIZE];

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
    ring_buffer_queue_arr(can1_tx[0].rb, (uint8_t *)buf, len);

    rb_len = ring_buffer_num_items(can1_tx[0].rb);
    if ((rb_len != 0) && (can_busy == 0) && (len > 0))
    {
        can_busy = 1;
        rb_len = (rb_len > 8) ? 8 : rb_len;
        ring_buffer_dequeue_arr(can1_tx[0].rb, (uint8_t *)&tx_frame.data[0], rb_len);
        tx_frame.std_id = can1_tx[0].can_id;
        tx_frame.dlc = rb_len;
        can1_send_frame_port(&tx_frame);
    }
    else
    {
        can_busy = 0;
        can_interrupt_disable(CAN0, CAN_INT_TME);
    }
}

static __inline uint16_t can1_receive_index(uint8_t n, uint8_t *buf, uint16_t buf_size)
{
    uint16_t len, readlen;
    len = ring_buffer_num_items(can1_rx[n].rb);
    if (len > 0)
    {
        len = (len > buf_size) ? buf_size : len;
        readlen = ring_buffer_dequeue_arr(can1_rx[n].rb, (uint8_t *)buf, len);
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
    if (can1_recv_frame_port(&rx_frame) != 0)
    {
        for (uint8_t n = 0; n < (sizeof(can1_rx) / sizeof(can1_rx[0])); n++)
        {
            if (rx_frame.std_id == can1_rx[n].can_id)
            {
                ring_buffer_queue_arr(can1_rx[n].rb, (uint8_t *)&rx_frame.data[0], rx_frame.dlc);
                break;
            }
        }
    }
}

void can1_tx_int_handle(void)
{
    uint32_t rb_len;
    can_frame_t tx_frame;

    rb_len = ring_buffer_num_items(can1_tx[0].rb);
    if (rb_len != 0)
    {
        rb_len = (rb_len > 8) ? 8 : rb_len;
        ring_buffer_dequeue_arr(can1_tx[0].rb, (uint8_t *)&tx_frame.data[0], rb_len);
        tx_frame.std_id = can1_tx[0].can_id;
        tx_frame.dlc = rb_len;
        can1_send_frame_port(&tx_frame);
    }
    else
    {
        can_busy = 0;
        can_interrupt_disable(CAN0, CAN_INT_TME);
    }
}

// user port----------------------------------------------------------------------------------
// 提供给其他简单接受CAN的部分使用
//_weak void can1_rx_callback_hook(struct rt_can_msg *rx_msg)
//{
//}

static int can1_recv_frame_port(can_frame_t *frame)
{
    can_receive_message_struct rx_message;
    can_message_receive(CAN0, CAN_FIFO0, &rx_message);

    frame->std_id = rx_message.rx_sfid;
    frame->dlc = rx_message.rx_dlen;
    memcpy(frame->data, rx_message.rx_data, rx_message.rx_dlen);

    return 0;
}

static void can1_send_frame_port(can_frame_t *frame)
{
    can_interrupt_enable(CAN0, CAN_INT_TME);
    can_trasnmit_message_struct tx_message;
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &tx_message);
    tx_message.tx_ft = CAN_FT_DATA;
    tx_message.tx_ff = CAN_FF_STANDARD;
    tx_message.tx_sfid = frame->std_id;
    tx_message.tx_dlen = frame->dlc;
    memcpy(&tx_message.tx_data[0], &frame->data[0], frame->dlc);
    if (CAN_NOMAILBOX == can_message_transmit(CAN0, &tx_message))
    {
        can_send_error_cnt++;
    }
}

/*gd32 can*/
void CAN0_TX_IRQHandler(void)
{
    can_flag_clear(CAN0, CAN_FLAG_MTF0);
    can1_rx_int_handle();
}

void CAN0_RX0_IRQHandler(void)
{
    can1_rx_int_handle();
}

// user init----------------------------------------------------------------------------------
void bsp_can1_init(void)
{
    ring_buffer_init(can1_tx[0].rb, rb_can1_tx_buffer, sizeof(rb_can1_tx_buffer));
    ring_buffer_init(can1_rx[0].rb, rb_can1_rx_buffer, sizeof(rb_can1_rx_buffer));

    /* enable can clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure CAN0 GPIO */
    gpio_af_set(GPIOA, GPIO_AF_9, GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_9, GPIO_PIN_12);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    /* initialize CAN */
    can_parameter_struct can_parameter;
    can_filter_parameter_struct can_filter;
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_FILTER_STRUCT, &can_filter);
    /* initialize CAN register */
    can_deinit(CAN0);
    /* initialize CAN */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = ENABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = ENABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_7TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_2TQ;
    /* baudrate 1Mbps */
    can_parameter.prescaler = 6; // 240 - pll 60m
    can_init(CAN0, &can_parameter);
    /* initialize filter */
    can_filter.filter_number = 0;
    can_filter.filter_mode = CAN_FILTERMODE_LIST;
    can_filter.filter_bits = CAN_FILTERBITS_16BIT;
    can_filter.filter_list_high = (uint16_t)(0x020 << 5);
    can_filter.filter_list_low = (uint16_t)(0x020 << 5);
    can_filter.filter_mask_high = (uint16_t)(0x020 << 5);
    can_filter.filter_mask_low = (uint16_t)(0x020 << 5);
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE;
    can_filter_init(&can_filter);

    can_filter.filter_number = 1;
    can_filter.filter_mode = CAN_FILTERMODE_LIST;
    can_filter.filter_bits = CAN_FILTERBITS_16BIT;
    can_filter.filter_list_high = (uint16_t)(0x510 << 5);
    can_filter.filter_list_low = (uint16_t)(0x511 << 5);
    can_filter.filter_mask_high = (uint16_t)(0x512 << 5);
    can_filter.filter_mask_low = (uint16_t)(0x513 << 5);
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE;
    can_filter_init(&can_filter);

    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_RX0_IRQn, 1, 2);
    nvic_irq_enable(CAN0_TX_IRQn, 1, 3);
    /* enable CAN receive FIFO1 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INT_RFNE0);
}

void bsp_can_init(void)
{
    bsp_can1_init();
    //    bsp_can2_init();
}

/*
 * Copyright (C) 2022 DJI.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-27     robomaster   first version
 */

#include "open_protocol_cmd.h"
#include "open_protocol.h"
#include "open_protocol_error.h"
#include "string.h"
#include "sys_param.h"
#include <stdio.h>

/* Private define ------------------------------------------------------------*/
#define STOP_BOOT_APP_FLAG_VAR (0x1234abcd)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// extern uint8_t g_factory_led_test_flag;
// extern log_module_t* g_log_proto;
// extern uint16_t g_sn_crc16;
// extern sys_param_t g_sys_param;                        //系统参数

// 用于停止启动的标志(APP和Loader中都不初始化，指定位置0x2020000），重新上电可以被Loader读到
// 如果等于0x1234abcd则停留在Loader中，该变量进入Loader后置0
uint32_t *stop_boot_app_flag = (uint32_t *)(0x20200000);

/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

extern uint8_t g_app_start;

/**
 * @brief 默认命令解包
 *
 * @param pack_desc
 */

void open_cmd_default(open_protocol_header_t *pack_desc)
{
    uint8_t ack_data = 0;
    if (pack_desc->is_ack == 0)
    {
        if (pack_desc->need_ack)
        {
            open_proto_ack(pack_desc, &ack_data, 1);
        }
    }
}

void mcu_software_reset(void)
{
    /* set FAULTMASK */
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

/**
 * @brief 重启
 *
 * @param pack_desc
 */
void open_cmd_reboot(open_protocol_header_t *pack_desc)
{
    open_cmd_version_rsp_t rsp;
    if (pack_desc->is_ack == 0)
    {
        mcu_software_reset();
        if (pack_desc->need_ack)
        {
            open_proto_ack(pack_desc, (uint8_t *)(&rsp), sizeof(rsp));
        }
    }
}

#define APP_VERSION (0X0101000D)
#define LOADER_VERSION (0X01010000)
#define HW_VER_ID "target_v2.0.1"

/**
 * @brief 查询版本
 *
 * @param pack_desc
 */
void open_cmd_ver(open_protocol_header_t *pack_desc)
{
    open_cmd_version_rsp_t rsp;
    if (pack_desc->is_ack == 0)
    {
        if (pack_desc->need_ack)
        {
            // sys_param_t param;
            // sys_param_read(&param);
            // memset(&rsp, 0, sizeof(rsp));
            // rsp.loader_ver = param.loader_ver;
            rsp.loader_ver = LOADER_VERSION;
            rsp.app_ver = APP_VERSION;
            memcpy(rsp.hw_id, HW_VER_ID, sizeof(HW_VER_ID));
            // board_sn_read(rsp.sn);
            rsp.sn[0] = 1;
            open_proto_ack(pack_desc, (uint8_t *)(&rsp), sizeof(rsp));
        }
    }
}

/**
 * @brief ECHO
 *
 * @param pack_desc
 */
void open_cmd_echo(open_protocol_header_t *pack_desc)
{
    // open_cmd_echo_rsp_t rsp;
    // if(pack_desc->is_ack == 0)
    // {
    //     if(pack_desc->need_ack)
    //     {
    //         rsp.error = 0;
    //         rsp.sn_crc16 = g_sn_crc16;
    //         open_proto_ack(pack_desc, (uint8_t*)(&rsp), sizeof(rsp));
    //     }
    // }
}

/**
 * @brief 进入Loader并停留在Loader
 *
 * @param pack_desc
 */
void open_cmd_enter_loader(open_protocol_header_t *pack_desc)
{
    open_comm_rsp_t rsp;
    if (pack_desc->is_ack == 0)
    {
        g_app_start = 0;
        if (pack_desc->need_ack)
        {
            rsp.err_code = 0;
            open_proto_ack(pack_desc, (uint8_t *)(&rsp), sizeof(rsp));
        }
    }
}

void open_cmd_stop_boot_app(open_protocol_header_t *pack_desc)
{
    open_comm_rsp_t rsp;
    if (pack_desc->is_ack == 0)
    {
        g_app_start = 0;
        if (pack_desc->need_ack)
        {
            rsp.err_code = 0;
            open_proto_ack(pack_desc, (uint8_t *)(&rsp), sizeof(rsp));
        }
    }
}

/**
 * @brief LED测试
 *
 * @param pack_desc
 */
void open_cmd_led_test(open_protocol_header_t *pack_desc)
{
    // open_cmd_led_test_t *req = (open_cmd_led_test_t*)(pack_desc->data);

    // g_factory_led_test_flag = req->en;

    // open_comm_rsp_t rsp;
    // if(pack_desc->is_ack == 0)
    // {
    //     if(pack_desc->need_ack)
    //     {
    //         rsp.err_code = 0;
    //         open_proto_ack(pack_desc, (uint8_t*)(&rsp), sizeof(rsp));
    //     }
    // }
}


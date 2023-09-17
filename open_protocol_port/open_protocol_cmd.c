/*
 * Copyright (C) 2022 DJI.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-27     robomaster   first version
 */
#include "open_protocol.h"
#include "open_protocol_cmd.h"
#include "open_protocol_error.h"

#include "gd32f425_bsp_mcu.h"
#include "systick.h"

#define APP_VERSION (0X0101000D)
#define LOADER_VERSION (0X01010000)
#define HW_VER_ID "target_v2.0.1"

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
        if (pack_desc->need_ack)
        {
            open_proto_ack(pack_desc, (uint8_t *)(&rsp), sizeof(rsp));
        }
        mcu_software_reset();
    }
}

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
            rsp.loader_ver = LOADER_VERSION;
            rsp.app_ver = APP_VERSION;
            // memcpy(rsp.hw_id, HW_VER_ID, sizeof(HW_VER_ID));
            memcpy(rsp.hw_id, g_sn, sizeof(g_sn));
            memcpy(rsp.sn, g_sn, sizeof(g_sn));
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
    open_cmd_echo_rsp_t rsp;
    if (pack_desc->is_ack == 0)
    {
        if (pack_desc->need_ack)
        {
            rsp.error = 0;
            extern uint16_t g_sn_crc16;
            rsp.sn_crc16 = g_sn_crc16;
            open_proto_ack(pack_desc, (uint8_t *)(&rsp), sizeof(rsp));
        }
    }
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
        if (pack_desc->need_ack)
        {
            rsp.err_code = 0;
            open_proto_ack(pack_desc, (uint8_t *)(&rsp), sizeof(rsp));
        }
        
        delay_1ms(50);
        /* user function */
        mcu_set_stop_app_flag();
        mcu_software_reset();
    }
}

/**
 * @brief 进入Loader且启动app
 *
 * @param pack_desc
 */
void open_cmd_stop_boot_app(open_protocol_header_t *pack_desc)
{
    open_comm_rsp_t rsp;
    if (pack_desc->is_ack == 0)
    {
        if (pack_desc->need_ack)
        {
            rsp.err_code = 0;
            open_proto_ack(pack_desc, (uint8_t *)(&rsp), sizeof(rsp));
        }
        /* user function */
        mcu_reset_stop_app_flag();
        mcu_software_reset();
    }
}

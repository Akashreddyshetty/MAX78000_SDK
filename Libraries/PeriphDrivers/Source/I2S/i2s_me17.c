/**
 * @file    i2s.c
 * @brief   Inter-Integrated Sound (I2S) driver implementation.
 */

/* ****************************************************************************
 * Copyright (C) 2017 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 * $Date: 2020-01-24 14:40:34 -0500 (Fri, 24 Jan 2020) $
 * $Revision: 28122 $
 *
 *************************************************************************** */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "mxc_device.h"
#include "mxc_assert.h"
#include "mxc_lock.h"
#include "mxc_sys.h"
#include "mxc_delay.h"
#include "dma.h"
#include "i2s.h"
#include "i2s_reva.h"
#include "i2s_regs.h"

int MXC_I2S_Init(mxc_i2s_req_t* req)
{
    MXC_I2S_Shutdown();
    
#ifdef MXC_SYS_CLOCK_ERFO // ME17 only
    MXC_SYS_ClockSourceEnable(MXC_SYS_CLOCK_ERFO);
#endif
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_I2S);
    MXC_GPIO_Config(&gpio_cfg_i2s0);
    
    return MXC_I2S_RevA_Init(req);
}

int MXC_I2S_Shutdown(void)
{
    MXC_I2S_RevA_Shutdown();
    
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_I2S);
    MXC_SYS_Reset_Periph(MXC_SYS_RESET1_I2S);
    
    return E_NO_ERROR;
}

int MXC_I2S_ConfigData(mxc_i2s_req_t* req)
{
    return MXC_I2S_RevA_ConfigData(req);
}

void MXC_I2S_TXEnable()
{
    MXC_I2S_RevA_TXEnable();
}

void MXC_I2S_TXDisable()
{
    MXC_I2S_RevA_TXDisable();
}

void MXC_I2S_RXEnable()
{
    MXC_I2S_RevA_RXEnable();
}

void MXC_I2S_RXDisable()
{
    MXC_I2S_RevA_RXDisable();
}

int MXC_I2S_SetRXThreshold(uint8_t threshold)
{
    return MXC_I2S_RevA_SetRXThreshold(threshold);
}

int MXC_I2S_SetFrequency(mxc_i2s_ch_mode_t mode, uint16_t clkdiv)
{
    return MXC_I2S_RevA_SetFrequency(mode, clkdiv);
}

void MXC_I2S_Flush(void)
{
    MXC_I2S_RevA_Flush();
}

void MXC_I2S_EnableInt(uint32_t flags)
{
    MXC_I2S_RevA_EnableInt(flags);
}

void MXC_I2S_DisableInt(uint32_t flags)
{
    MXC_I2S_RevA_DisableInt(flags);
}

int MXC_I2S_GetFlags()
{
    return MXC_I2S_RevA_GetFlags();
}

void MXC_I2S_ClearFlags(uint32_t flags)
{
    MXC_I2S_RevA_ClearFlags(flags);
}

void MXC_I2S_TXDMAConfig(void* src_addr, int len)
{
    MXC_I2S_RevA_TXDMAConfig(src_addr, len);
}

void MXC_I2S_RXDMAConfig(void* dest_addr, int len)
{
    MXC_I2S_RevA_RXDMAConfig(dest_addr, len);
}

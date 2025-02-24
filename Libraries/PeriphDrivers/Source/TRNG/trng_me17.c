/**
 * @file
 * @brief   Trust Protection Unit driver.
 */

/* ****************************************************************************
 * Copyright (C) 2018 Maxim Integrated Products, Inc., All Rights Reserved.
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
 * $Date$
 * $Revision$
 *
 *************************************************************************** */

#include "mxc_device.h"
#include "mxc_errors.h"
#include "mxc_assert.h"
#include "mxc_sys.h"
#include "trng_revb.h"
#include "trng.h"


/* ************************************************************************* */
/* Global Control/Configuration functions                                    */
/* ************************************************************************* */

/********************************************************/

int MXC_TRNG_Init()
{
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_TRNG);
    
    MXC_TRNG_RevB_Init();
    
    return E_NO_ERROR;
}

void MXC_TRNG_EnableInt()
{
    MXC_TRNG_RevB_EnableInt();
}

void MXC_TRNG_DisableInt()
{
    MXC_TRNG_RevB_DisableInt();
}

int MXC_TRNG_Shutdown()
{
    int error = MXC_TRNG_RevB_Shutdown();
    
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_TRNG);
    
    return error;
}

void MXC_TRNG_Handler(void)
{
    MXC_TRNG_RevB_Handler();
}

/* ************************************************************************* */
/* True Random Number Generator (TRNG) functions                             */
/* ************************************************************************* */

int MXC_TRNG_RandomInt(void)
{
    return MXC_TRNG_RevB_RandomInt();
}

int MXC_TRNG_Random(uint8_t* data, uint32_t len)
{
    return MXC_TRNG_RevB_Random(data, len);
}

void MXC_TRNG_RandomAsync(uint8_t* data, uint32_t len, mxc_trng_complete_t callback)
{
    MXC_TRNG_RevB_RandomAsync(data, len, callback);
}

/* *****************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software");,
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
 **************************************************************************** */

/* **** Includes **** */
#include "mxc_device.h"
#include "mxc_errors.h"
#include "mxc_assert.h"
#include "mxc_sys.h"
#include "wdt.h"

/* **** Definitions **** */
typedef enum {
    MXC_WDT_DISABLE = 0,
    MXC_WDT_ENABLE = 1,
} mxc_wdt_en_t;

/* **** Functions **** */
int MXC_WDT_RevB_Init (mxc_wdt_regs_t* wdt, mxc_wdt_cfg_t *cfg);
void MXC_WDT_RevB_SetIntPeriod (mxc_wdt_regs_t* wdt, mxc_wdt_cfg_t *cfg);
void MXC_WDT_RevB_SetResetPeriod (mxc_wdt_regs_t* wdt, mxc_wdt_cfg_t *cfg);
void MXC_WDT_RevB_Enable (mxc_wdt_regs_t* wdt);
void MXC_WDT_RevB_Disable (mxc_wdt_regs_t* wdt);
void MXC_WDT_RevB_EnableInt (mxc_wdt_regs_t* wdt, mxc_wdt_en_t enable);
void MXC_WDT_RevB_EnableReset (mxc_wdt_regs_t* wdt, mxc_wdt_en_t enable);
void MXC_WDT_RevB_ResetTimer (mxc_wdt_regs_t* wdt);
int MXC_WDT_RevB_GetResetFlag (mxc_wdt_regs_t* wdt);
void MXC_WDT_RevB_ClearResetFlag (mxc_wdt_regs_t* wdt);
int MXC_WDT_RevB_GetIntFlag (mxc_wdt_regs_t* wdt);
void MXC_WDT_RevB_ClearIntFlag (mxc_wdt_regs_t* wdt);
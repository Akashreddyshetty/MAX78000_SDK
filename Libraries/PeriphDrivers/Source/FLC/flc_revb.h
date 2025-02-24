/**
 * @file flc.h
 * @brief      Flash Controler driver.
 * @details    This driver can be used to operate on the embedded flash memory.
 */
/* ****************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
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
 * $Date: 2018-10-15 16:49:29 -0500 (Mon, 15 Oct 2018) $
 * $Revision: 38520 $
 *
 *************************************************************************** */

/* **** Includes **** */
#include <string.h>
#include "mxc_device.h"
#include "mxc_assert.h"
#include "mxc_sys.h"
#include "flc.h"
#include "flc_regs.h"
#include "mcr_regs.h" // For ECCEN registers.

/**
 * @ingroup flc
 * @{
 */

/* **** Definitions **** */

/* **** Globals **** */

/* **** Functions **** */

int MXC_FLC_RevB_Busy (void);

int MXC_FLC_RevB_MassErase (mxc_flc_regs_t *flc);

int MXC_FLC_RevB_PageErase (mxc_flc_regs_t *flc,uint32_t addr);

int MXC_FLC_RevB_Write32 (mxc_flc_regs_t *flc, uint32_t locgialAddr, uint32_t data, uint32_t physicalAddr);

int MXC_FLC_RevB_Write128 (mxc_flc_regs_t *flc, uint32_t addr, uint32_t *data);

int MXC_FLC_RevB_EnableInt (uint32_t mask);

int MXC_FLC_RevB_DisableInt (uint32_t mask);

int MXC_FLC_RevB_GetFlags (void);

int MXC_FLC_RevB_ClearFlags (uint32_t mask);

int MXC_FLC_RevB_UnlockInfoBlock (mxc_flc_regs_t *flc, uint32_t address);

int MXC_FLC_RevB_LockInfoBlock (mxc_flc_regs_t *flc, uint32_t address);
/**@} end of group flc */


#ifdef __cplusplus
}
#endif

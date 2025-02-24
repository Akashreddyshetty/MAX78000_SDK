/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
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
******************************************************************************/

/* **** Includes **** */
#include <string.h>
#include "cameraif.h"
#include "mxc_device.h"
#include "mxc_assert.h"
#include "mxc_pins.h"
#include "mxc_sys.h"

/* **** Definitions **** */
#define PCIF_IE_MASK (MXC_F_CAMERAIF_INT_EN_IMG_DONE | MXC_F_CAMERAIF_INT_EN_FIFO_FULL | \
                      MXC_F_CAMERAIF_INT_EN_FIFO_THRESH)

/* **** Globals **** */

/* **** Functions **** */

int MXC_PCIF_RevA_Init(void)
{
    return 0;
}

void MXC_PCIF_RevA_SetDataWidth(mxc_pcif_datawidth_t datawidth)
{
    MXC_PCIF->ctrl &= ~(MXC_F_CAMERAIF_CTRL_DATA_WIDTH);
    MXC_PCIF->ctrl |= (datawidth << MXC_F_CAMERAIF_CTRL_DATA_WIDTH_POS);
}

void MXC_PCIF_RevA_SetTimingSel(mxc_pcif_timingsel_t timingsel)
{
    MXC_PCIF->ctrl &= ~(MXC_F_CAMERAIF_CTRL_DS_TIMING_EN);
    MXC_PCIF->ctrl |= (timingsel << MXC_F_CAMERAIF_CTRL_DS_TIMING_EN_POS);
}

void MXC_PCIF_RevA_SetThreshhold(int fifo_thrsh)
{
    MXC_PCIF->ctrl &= ~(MXC_F_CAMERAIF_CTRL_FIFO_THRSH);
    MXC_PCIF->ctrl |= ((fifo_thrsh << MXC_F_CAMERAIF_CTRL_FIFO_THRSH_POS) & MXC_F_CAMERAIF_CTRL_FIFO_THRSH);
}

void MXC_PCIF_RevA_EnableInt(uint32_t flags)
{
    MXC_PCIF->int_en |= (flags & PCIF_IE_MASK);
}

void MXC_PCIF_RevA_DisableInt(uint32_t flags)
{
    MXC_PCIF->int_en &= ~(flags & PCIF_IE_MASK);
}

void MXC_PCIF_RevA_Start(mxc_pcif_readmode_t readmode)
{
    MXC_PCIF->ctrl &= ~(MXC_F_CAMERAIF_CTRL_READ_MODE);
    MXC_PCIF->ctrl |= (readmode & MXC_F_CAMERAIF_CTRL_READ_MODE);
}

void MXC_PCIF_RevA_Stop(void)
{
    MXC_PCIF->ctrl &= ~(MXC_F_CAMERAIF_CTRL_READ_MODE);
}

unsigned int MXC_PCIF_RevA_GetData(void)
{
    return (unsigned int)(MXC_PCIF->fifo_data);
}

/**@} end of group cameraif */

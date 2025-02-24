/* *****************************************************************************
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
 * $Date: 2019-10-25 14:21:06 -0500 (Fri, 25 Oct 2019) $
 * $Revision: 48094 $
 *
 **************************************************************************** */

/* **** Includes **** */
#include "mxc_device.h"
#include "mxc_assert.h"
#include "wut.h"
#include "gcr_regs.h"

/* **** Definitions **** */

/* **** Globals **** */

/* **** Local Variables **** */
static uint32_t wut_count;
static uint32_t wut_snapshot;

/* **** Functions **** */

/* ************************************************************************** */
void MXC_WUT_RevA_Init(mxc_wut_regs_t* wut, mxc_wut_pres_t pres)
{
    // Disable timer and clear settings
    wut->cn = 0;
    
    // Clear interrupt flag
    wut->intr = MXC_F_WUT_INTR_IRQ_CLR;
    
    // Set the prescaler
    wut->cn |= pres;
    
    // Initialize the compare register
    wut->cmp = 0xFFFFFFFF;
    
    // Initialize the local variables
    wut_count = 0;
    wut_snapshot = 0;
}

void MXC_WUT_RevA_Shutdown(mxc_wut_regs_t* wut)
{
    // Disable timer and clear settings
    wut->cn = 0;
}

/* ************************************************************************** */
void MXC_WUT_RevA_Enable(mxc_wut_regs_t* wut)
{
    wut->cn |= MXC_F_WUT_CN_TEN;
}

/* ************************************************************************** */
void MXC_WUT_RevA_Disable(mxc_wut_regs_t* wut)
{
    wut->cn &= ~(MXC_F_WUT_CN_TEN);
}

/* ************************************************************************** */
void MXC_WUT_RevA_Config(mxc_wut_regs_t* wut, const mxc_wut_cfg_t* cfg)
{
    // Configure the timer
    wut->cn |= (wut->cn & ~(MXC_F_WUT_CN_TMODE | MXC_F_WUT_CN_TPOL)) |
               ((cfg->mode << MXC_F_WUT_CN_TMODE_POS) & MXC_F_WUT_CN_TMODE);
               
    wut->cnt = 0x1;
    
    wut->cmp = cfg->cmp_cnt;
}

/* ************************************************************************** */
uint32_t MXC_WUT_RevA_GetCompare(mxc_wut_regs_t* wut)
{
    return wut->cmp;
}

/* ************************************************************************** */
uint32_t MXC_WUT_RevA_GetCapture(mxc_wut_regs_t* wut)
{
    return wut->pwm;
}

/* ************************************************************************* */
uint32_t MXC_WUT_RevA_GetCount(mxc_wut_regs_t* wut)
{
    return wut->cnt;
}

/* ************************************************************************* */
void MXC_WUT_RevA_IntClear(mxc_wut_regs_t* wut)
{
    wut->intr = MXC_F_WUT_INTR_IRQ_CLR;
}

/* ************************************************************************* */
uint32_t MXC_WUT_RevA_IntStatus(mxc_wut_regs_t* wut)
{
    return wut->intr;
}

/* ************************************************************************* */
void MXC_WUT_RevA_SetCompare(mxc_wut_regs_t* wut, uint32_t cmp_cnt)
{
    wut->cmp = cmp_cnt;
}

/* ************************************************************************* */
void MXC_WUT_RevA_SetCount(mxc_wut_regs_t* wut, uint32_t cnt)
{
    wut->cnt = cnt;
}

/* ************************************************************************* */
int MXC_WUT_RevA_GetTicks(mxc_wut_regs_t* wut, uint32_t timerClock, uint32_t time, mxc_wut_unit_t units, uint32_t* ticks)
{
    uint32_t unit_div0, unit_div1;
    uint32_t prescale;
    uint64_t temp_ticks;
    
    prescale = ((wut->cn & MXC_F_WUT_CN_PRES) >> MXC_F_WUT_CN_PRES_POS)
               | (((wut->cn & MXC_F_WUT_CN_PRES3) >> (MXC_F_WUT_CN_PRES3_POS)) << 3);
               
    switch (units) {
    case MXC_WUT_UNIT_NANOSEC:
        unit_div0 = 1000000;
        unit_div1 = 1000;
        break;
        
    case MXC_WUT_UNIT_MICROSEC:
        unit_div0 = 1000;
        unit_div1 = 1000;
        break;
        
    case MXC_WUT_UNIT_MILLISEC:
        unit_div0 = 1;
        unit_div1 = 1000;
        break;
        
    case MXC_WUT_UNIT_SEC:
        unit_div0 = 1;
        unit_div1 = 1;
        break;
        
    default:
        return E_BAD_PARAM;
    }
    
    temp_ticks = (uint64_t) time * (timerClock / unit_div0) / (unit_div1 * (1 << (prescale & 0xF)));
    
    //make sure ticks is within a 32 bit value
    if (!(temp_ticks & 0xffffffff00000000)  && (temp_ticks & 0xffffffff)) {
        *ticks = temp_ticks;
        return E_NO_ERROR;
    }
    
    return E_INVALID;
}

/* ************************************************************************* */
int MXC_WUT_RevA_GetTime(mxc_wut_regs_t* wut, uint32_t timerClock, uint32_t ticks, uint32_t* time, mxc_wut_unit_t* units)
{
    uint64_t temp_time = 0;
    uint32_t prescale = ((wut->cn & MXC_F_WUT_CN_PRES) >> MXC_F_WUT_CN_PRES_POS)
                        | (((wut->cn & MXC_F_WUT_CN_PRES3) >> (MXC_F_WUT_CN_PRES3_POS)) << 3);
                        
    temp_time = (uint64_t) ticks * 1000 * (1 << (prescale & 0xF)) / (timerClock / 1000000);
    
    if (!(temp_time & 0xffffffff00000000)) {
        *time = temp_time;
        *units = MXC_WUT_UNIT_NANOSEC;
        return E_NO_ERROR;
    }
    
    temp_time = (uint64_t) ticks * 1000 * (1 << (prescale & 0xF)) / (timerClock / 1000);
    
    if (!(temp_time & 0xffffffff00000000)) {
        *time = temp_time;
        *units = MXC_WUT_UNIT_MICROSEC;
        return E_NO_ERROR;
    }
    
    temp_time = (uint64_t) ticks * 1000 * (1 << (prescale & 0xF)) / timerClock;
    
    if (!(temp_time & 0xffffffff00000000)) {
        *time = temp_time;
        *units = MXC_WUT_UNIT_MILLISEC;
        return E_NO_ERROR;
    }
    
    temp_time = (uint64_t) ticks * (1 << (prescale & 0xF)) / timerClock;
    
    if (!(temp_time & 0xffffffff00000000)) {
        *time = temp_time;
        *units = MXC_WUT_UNIT_SEC;
        return E_NO_ERROR;
    }
    
    return E_INVALID;
}

/* ************************************************************************** */
void MXC_WUT_RevA_Edge(mxc_wut_regs_t* wut)
{
    // Wait for a WUT edge
    uint32_t tmp = wut->cnt;
    
    while (tmp == wut->cnt) {}
}

/* ************************************************************************** */
void MXC_WUT_RevA_Store(mxc_wut_regs_t* wut)
{
    wut_count = wut->cnt;
    wut_snapshot = wut->snapshot;
}

/* ************************************************************************** */
void MXC_WUT_RevA_RestoreBBClock(mxc_wut_regs_t* wut, uint32_t dbbFreq, uint32_t timerClock)
{
    /* restore DBB clock from WUT */
    MXC_WUT_RevA_Edge(wut);
    wut->preset = wut_snapshot + (uint64_t)(wut->cnt - wut_count + 1)
                  * dbbFreq / timerClock;
    wut->reload = 1;  // arm DBB_CNT update on the next rising WUT clock
    MXC_WUT_RevA_Edge(wut);
}

/* ************************************************************************** */
uint32_t MXC_WUT_RevA_GetSleepTicks(mxc_wut_regs_t* wut)
{
    return (wut->cnt - wut_count);
}

/* ************************************************************************** */
void MXC_WUT_RevA_Delay_MS(mxc_wut_regs_t* wut, uint32_t waitMs, uint32_t timerClock)
{
    /* assume WUT is already running */
    uint32_t  tmp = wut->cnt;
    
    tmp += (waitMs * (timerClock /
                      (0x1 << ((wut->cn & MXC_F_WUT_CN_PRES) >> MXC_F_WUT_CN_PRES_POS)))
            + 500) / 1000 ;
            
    while (wut->cnt < tmp) {}
}

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
#include "i2s_reva.h"
#include "i2s.h"

/* ***** Definitions ***** */
#define DATALENGTH_EIGHT        (8 - 1)
#define DATALENGTH_SIXTEEN      (16 - 1)
#define DATALENGTH_TWENTY       (20 - 1)
#define DATALENGTH_TWENTYFOUR   (24 - 1)
#define DATALENGTH_THIRTYTWO    (32 - 1)

/* ****** Functions ****** */
int MXC_I2S_RevA_Init(mxc_i2s_req_t* req)
{
    if (((req->txData == NULL) || (req->rawData == NULL)) && (req->rxData == NULL)) {
        return E_NULL_PTR;
    }
    
    if (req->length == 0) {
        return E_BAD_PARAM;
    }
    
    if (req->stereoMode) {
        MXC_I2S->ctrl0ch0 |= (req->stereoMode << MXC_F_I2S_CTRL0CH0_STEREO_POS);
    }
    
    //Set RX Threshold 2 (default)
    MXC_I2S->ctrl0ch0 |= (2 << MXC_F_I2S_CTRL0CH0_RX_THD_VAL_POS);
    
    //Set justify
    MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_ALIGN, (req->justify) << MXC_F_I2S_CTRL0CH0_ALIGN_POS);
    
    if (MXC_I2S_ConfigData(req) != E_NO_ERROR) {
        return E_BAD_PARAM;
    }
    
    MXC_I2S_SetFrequency(req->channelMode, req->clkdiv);
    
    return E_NO_ERROR;
}

int MXC_I2S_RevA_Shutdown(void)
{
    MXC_I2S_DisableInt(0xFF);
    
    //Disable I2S TX and RX channel
    MXC_I2S_TXDisable();
    MXC_I2S_RXDisable();
    
    MXC_I2S_Flush();
    
    //Clear all the registers. Not cleared on reset
    MXC_I2S->ctrl0ch0 = 0x00;
    MXC_I2S->dmach0   = 0x00;
    MXC_I2S->ctrl1ch0 = 0x00;
    
    MXC_I2S->ctrl0ch0 |= MXC_F_I2S_CTRL0CH0_RST;             //Reset channel
    
    return E_NO_ERROR;
}

int MXC_I2S_RevA_ConfigData(mxc_i2s_req_t* req)
{
    uint32_t dataMask;
    
    if ((req->txData == NULL) & (req->rxData == NULL)) {
        return E_NULL_PTR;
    }
    
    if (req->length == 0) {
        return E_BAD_PARAM;
    }
    
    // Clear configuration bits
    MXC_I2S->ctrl0ch0 &= ~MXC_F_I2S_CTRL0CH0_WSIZE;
    MXC_I2S->ctrl1ch0 &= ~MXC_F_I2S_CTRL1CH0_BITS_WORD;
    MXC_I2S->ctrl1ch0 &= ~MXC_F_I2S_CTRL1CH0_SMP_SIZE;
    
    switch (req->sampleSize) {
    case MXC_I2S_SAMPLESIZE_EIGTH:
        if (req->wordSize == MXC_I2S_DATASIZE_WORD) {
            //Set word length
            MXC_I2S->ctrl1ch0 |= (DATALENGTH_THIRTYTWO << MXC_F_I2S_CTRL1CH0_BITS_WORD_POS);
        }
        else if (req->wordSize == MXC_I2S_DATASIZE_HALFWORD) {
            //Set word length
            MXC_I2S->ctrl1ch0 |= (DATALENGTH_SIXTEEN << MXC_F_I2S_CTRL1CH0_BITS_WORD_POS);
        }
        else {
            //Set word length
            MXC_I2S->ctrl1ch0 |= (DATALENGTH_EIGHT << MXC_F_I2S_CTRL1CH0_BITS_WORD_POS);
        }
        
        //Set sample length
        MXC_I2S->ctrl1ch0 |= (DATALENGTH_EIGHT << MXC_F_I2S_CTRL1CH0_SMP_SIZE_POS);
        
        //Set datasize to load in FIFO
        MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_WSIZE, (MXC_I2S_DATASIZE_BYTE) << MXC_F_I2S_CTRL0CH0_WSIZE_POS);
        
        dataMask =  0x000000ff;
        
        if ((req->rawData != NULL) && (req->txData != NULL)) {
            for (uint32_t i = 0; i < req->length ; i++) {
                * ((uint8_t*) req->txData++) = * ((uint8_t*) req->rawData++) & dataMask;
            }
        }
        
        break;
        
    case MXC_I2S_SAMPLESIZE_SIXTEEN:
        if (req->wordSize == MXC_I2S_DATASIZE_WORD) {
            //Set word length
            MXC_I2S->ctrl1ch0 |= (DATALENGTH_THIRTYTWO << MXC_F_I2S_CTRL1CH0_BITS_WORD_POS);
        }
        else {
            //Set word length
            MXC_I2S->ctrl1ch0 |= (DATALENGTH_SIXTEEN << MXC_F_I2S_CTRL1CH0_BITS_WORD_POS);
        }
        
        //Set sample length
        MXC_I2S->ctrl1ch0 |= (DATALENGTH_SIXTEEN << MXC_F_I2S_CTRL1CH0_SMP_SIZE_POS);
        
        //Set datasize
        MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_WSIZE, (MXC_I2S_DATASIZE_HALFWORD) << MXC_F_I2S_CTRL0CH0_WSIZE_POS);
        
        dataMask = 0x0000ffff;
        
        if ((req->rawData != NULL) && (req->txData != NULL)) {
            for (uint32_t i = 0; i < req->length ; i++) {
                * ((uint16_t*) req->txData++) = * ((uint16_t*) req->rawData++) & dataMask;
            }
        }
        
        break;
        
    case MXC_I2S_SAMPLESIZE_TWENTY:
        //Set word length
        MXC_I2S->ctrl1ch0 |= (DATALENGTH_THIRTYTWO << MXC_F_I2S_CTRL1CH0_BITS_WORD_POS);
        
        //Set sample length
        MXC_I2S->ctrl1ch0 |= (DATALENGTH_TWENTY << MXC_F_I2S_CTRL1CH0_SMP_SIZE_POS);
        
        //Set datasize
        MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_WSIZE, (MXC_I2S_DATASIZE_WORD) << MXC_F_I2S_CTRL0CH0_WSIZE_POS);
        
        dataMask = 0x00fffff;
        
        if ((req->rawData != NULL) && (req->txData != NULL)) {
            for (uint32_t i = 0; i < req->length ; i++) {
                * ((uint32_t*) req->txData++) = (* ((uint32_t*) req->rawData++) & dataMask) << 12;
            }
        }
        
        break;
        
    case MXC_I2S_SAMPLESIZE_TWENTYFOUR:
        //Set word length
        MXC_I2S->ctrl1ch0 |= (DATALENGTH_THIRTYTWO << MXC_F_I2S_CTRL1CH0_BITS_WORD_POS);
        
        //Set sample length
        MXC_I2S->ctrl1ch0 |= (DATALENGTH_TWENTYFOUR << MXC_F_I2S_CTRL1CH0_SMP_SIZE_POS);
        
        //Set datasize
        MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_WSIZE, (MXC_I2S_DATASIZE_WORD) << MXC_F_I2S_CTRL0CH0_WSIZE_POS);
        
        dataMask = 0x00ffffff;
        
        if ((req->rawData != NULL) && (req->txData != NULL)) {
            for (uint32_t i = 0; i < req->length ; i++) {
                * ((uint32_t*) req->txData++) = (* ((uint32_t*) req->rawData++) & dataMask) << 8;
            }
        }
        
        break;
        
    case MXC_I2S_SAMPLESIZE_THIRTYTWO:
        //Set word length
        MXC_I2S->ctrl1ch0 |= (DATALENGTH_THIRTYTWO << MXC_F_I2S_CTRL1CH0_BITS_WORD_POS);
        
        //Set sample length
        MXC_I2S->ctrl1ch0 |= (DATALENGTH_THIRTYTWO << MXC_F_I2S_CTRL1CH0_SMP_SIZE_POS);
        
        //Set datasize
        MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_WSIZE, (MXC_I2S_DATASIZE_WORD) << MXC_F_I2S_CTRL0CH0_WSIZE_POS);
        
        dataMask = 0xffffffff;
        
        if ((req->rawData != NULL) && (req->txData != NULL)) {
            for (uint32_t i = 0; i < req->length ; i++) {
                * ((uint32_t*) req->txData++) = * ((uint32_t*) req->rawData++) & dataMask;
            }
        }
        
        break;
        
    default:
        return E_BAD_PARAM;
        break;
    }
    
    return E_NO_ERROR;
}

void MXC_I2S_RevA_TXEnable(void)
{
    MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_TX_EN, 1 << MXC_F_I2S_CTRL0CH0_TX_EN_POS);
}

void MXC_I2S_RevA_TXDisable(void)
{
    MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_TX_EN, 0 << MXC_F_I2S_CTRL0CH0_TX_EN_POS);
}

void MXC_I2S_RevA_RXEnable(void)
{
    MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_RX_EN, 1 << MXC_F_I2S_CTRL0CH0_RX_EN_POS);
}

void MXC_I2S_RevA_RXDisable(void)
{
    MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_RX_EN, 0 << MXC_F_I2S_CTRL0CH0_RX_EN_POS);
}

int MXC_I2S_RevA_SetRXThreshold(uint8_t threshold)
{
    if ((threshold == 0) || (threshold > 8)) {
        return E_NOT_SUPPORTED;
    }
    
    MXC_I2S->ctrl0ch0 |= (threshold << MXC_F_I2S_CTRL0CH0_RX_THD_VAL_POS);
    
    return E_NO_ERROR;
}

int MXC_I2S_RevA_SetFrequency(mxc_i2s_ch_mode_t mode, uint16_t clkdiv)
{
    MXC_I2S->ctrl1ch0 &= ~MXC_F_I2S_CTRL1CH0_EN;
    
    MXC_SETFIELD(MXC_I2S->ctrl0ch0, MXC_F_I2S_CTRL0CH0_CH_MODE, (mode) << MXC_F_I2S_CTRL0CH0_CH_MODE_POS);
    
    MXC_I2S->ctrl1ch0 |= ((uint32_t) clkdiv) << MXC_F_I2S_CTRL1CH0_CLKDIV_POS;
    
    MXC_I2S->ctrl1ch0 |= MXC_F_I2S_CTRL1CH0_EN;
    
    return E_NO_ERROR;
}

void MXC_I2S_RevA_Flush(void)
{
    MXC_I2S->ctrl0ch0 |= MXC_F_I2S_CTRL0CH0_FLUSH;
    
    while (MXC_I2S->ctrl0ch0 & MXC_F_I2S_CTRL0CH0_FLUSH);
}

void MXC_I2S_RevA_EnableInt(uint32_t flags)
{
    MXC_I2S->inten |= flags;
}

void MXC_I2S_RevA_DisableInt(uint32_t flags)
{
    MXC_I2S->inten &= ~flags;
}

int MXC_I2S_RevA_GetFlags(void)
{
    return (MXC_I2S->intfl & 0xF);
}

void MXC_I2S_RevA_ClearFlags(uint32_t flags)
{
    MXC_I2S->intfl |= flags;
}

void MXC_I2S_RevA_TXDMAConfig(void* src_addr, int len)
{
    uint8_t channel;
    mxc_dma_config_t config;
    mxc_dma_srcdst_t srcdst;
    
    MXC_DMA_Init();
    
    MXC_I2S->dmach0 |= (2 << MXC_F_I2S_DMACH0_DMA_TX_THD_VAL_POS);     //TX DMA Threshold
    
    channel = MXC_DMA_AcquireChannel();
    
    config.reqsel = MXC_DMA_REQUEST_I2STX;
    
    config.ch = channel;
    
    config.srcwd = MXC_DMA_WIDTH_WORD;
    config.dstwd = MXC_DMA_WIDTH_WORD;
    
    config.srcinc_en = 1;
    config.dstinc_en = 0;
    
    srcdst.ch = channel;
    srcdst.source = src_addr;
    srcdst.len = len;
    
    MXC_DMA_ConfigChannel(config, srcdst);
    MXC_DMA_SetCallback(channel, NULL);
    
    MXC_I2S_TXEnable();                                 //Enable I2S TX
    MXC_I2S->dmach0 |= MXC_F_I2S_DMACH0_DMA_TX_EN;      //Enable I2S DMA
    
    MXC_DMA_EnableInt(channel);
    MXC_DMA_Start(channel);
    MXC_DMA->ch[channel].ctrl |= MXC_F_DMA_CTRL_CTZ_IE;
}

void MXC_I2S_RevA_RXDMAConfig(void* dest_addr, int len)
{
    uint8_t channel;
    mxc_dma_config_t config;
    mxc_dma_srcdst_t srcdst;
    
    MXC_DMA_Init();
    
    MXC_I2S->dmach0 |= (6 << MXC_F_I2S_DMACH0_DMA_RX_THD_VAL_POS);     //RX DMA Threshold
    
    channel = MXC_DMA_AcquireChannel();
    
    config.reqsel = MXC_DMA_REQUEST_I2SRX;
    
    config.ch = channel;
    
    config.srcwd = MXC_DMA_WIDTH_WORD;
    config.dstwd = MXC_DMA_WIDTH_WORD;
    
    config.srcinc_en = 0;
    config.dstinc_en = 1;
    
    srcdst.ch = channel;
    srcdst.dest = dest_addr;
    srcdst.len = len;
    
    MXC_DMA_ConfigChannel(config, srcdst);
    MXC_DMA_SetCallback(channel, NULL);
    
    MXC_I2S_RXEnable();                                 //Enable I2S RX
    MXC_I2S->dmach0 |= MXC_F_I2S_DMACH0_DMA_RX_EN;      //Enable I2S DMA
    
    MXC_DMA_EnableInt(channel);
    MXC_DMA_Start(channel);
    MXC_DMA->ch[channel].ctrl |= MXC_F_DMA_CTRL_CTZ_IE;
}
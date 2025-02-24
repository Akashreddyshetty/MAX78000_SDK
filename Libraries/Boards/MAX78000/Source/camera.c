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
#include <stdlib.h>
#include <string.h>
#include "mxc_device.h"
#include "board.h"
#include "nvic_table.h"
#include "pt.h"
#include "cameraif.h"
#include "dma.h"
#include "uart.h"
#include "mxc_delay.h"
#include "gpio.h"
#include "camera.h"
#include "sccb.h"
#include "dma_regs.h"

/*******************************      DEFINES      ***************************/
#define FIFO_THRES_HOLD         (4)
#define PCIF_DATA_BUS_WITH      MXC_S_CAMERAIF_CTRL_DATA_WIDTH_8BIT
#define CAMERA_STARTUP_DELAY    (300)
#define CAMERA_DATA_BIT_WIDTH   MXC_PCIF_GPIO_DATAWIDTH_8_BIT

/******************************** Static Functions ***************************/

static uint8_t* rx_data = NULL;
static volatile uint32_t rx_data_index = 0;
static volatile uint32_t g_is_img_rcv = 0;
static int g_total_img_size = 0;
static int g_framesize_width = 64;
static int g_framesize_height = 64;
static int g_pixel_format = PIXFORMAT_RGB888;
static fifomode_t g_fifo_mode = FIFO_THREE_BYTE;
static dmamode_t g_dma_mode = NO_DMA;
static camera_t camera;
extern int sensor_register(camera_t* camera);

//----------------------------------------
// PCIF ISR
//----------------------------------------
void camera_irq_handler(void)
{
    uint32_t i;
    uint32_t data;
    uint32_t step_size;
    
    if (g_dma_mode == NO_DMA) {
        // Check fifo threshold flag.
        if (MXC_PCIF->int_fl & MXC_F_CAMERAIF_INT_FL_FIFO_THRESH) {
            for (i = 0; i < FIFO_THRES_HOLD; i++) {
                data = MXC_PCIF->fifo_data;
                
                rx_data[rx_data_index + 0] = data >> 0;
                rx_data[rx_data_index + 1] = data >> 8;
                rx_data[rx_data_index + 2] = data >> 16;
                
                if (g_fifo_mode == FIFO_FOUR_BYTE) {
                    rx_data[rx_data_index + 3] = data >> 24;
                    step_size = 4;
                }
                else {
                    step_size = 3;
                }
                
                if ((rx_data_index + step_size) < g_total_img_size) {
                    rx_data_index += step_size; // increase it only if there is free space
                }
            }
        }
    }
    
    // check img_done flag
    if (MXC_PCIF->int_fl & MXC_F_CAMERAIF_INT_FL_IMG_DONE) {
        /* Flush the fifo. */
        if (g_dma_mode == NO_DMA) {
            while (MXC_PCIF->int_fl & MXC_F_CAMERAIF_INT_FL_FIFO_NOT_EMPTY) {
                data = MXC_PCIF->fifo_data;
                MXC_PCIF->int_fl = MXC_F_CAMERAIF_INT_FL_FIFO_NOT_EMPTY;
                
                rx_data[rx_data_index + 0] = data >> 0;
                rx_data[rx_data_index + 1] = data >> 8;
                rx_data[rx_data_index + 2] = data >> 16;
                
                //rx_data[rx_data_index + 3] = data>>24;
                if (g_fifo_mode == FIFO_FOUR_BYTE) {
                    rx_data[rx_data_index + 3] = data >> 24;
                    step_size = 4;
                }
                else {
                    step_size = 3;
                }
                
                if ((rx_data_index + step_size) < g_total_img_size) {
                    rx_data_index += step_size; // increase it only if there is free space
                }
            }
        }
        else {
            while (MXC_PCIF->int_fl & MXC_F_CAMERAIF_INT_FL_FIFO_NOT_EMPTY) {
                data = MXC_PCIF->fifo_data;
                MXC_PCIF->int_fl = MXC_F_CAMERAIF_INT_FL_FIFO_NOT_EMPTY;
            }
            rx_data_index = g_total_img_size;
        }
        
        g_is_img_rcv = 1;
    }
    
    // clear flags, tmp flag is used to pass coverity check
    uint32_t flags = MXC_PCIF->int_fl;
    MXC_PCIF->int_fl = flags; // clear flags
}

#ifdef __riscv
void __attribute__((interrupt("machine")))  PCIF_IRQHandler(void)
{
    camera_irq_handler();
    NVIC_ClearPendingIRQ(PCIF_IRQn);
}
#endif

static void setup_dma(void)
{
    int dma_handle = 0;
    MXC_DMA->ch[dma_handle].status = MXC_F_DMA_STATUS_CTZ_IF; // Clear CTZ status flag
    MXC_DMA->inten = 0x0;

    MXC_DMA->ch[dma_handle].dst = (uint32_t) rx_data; // Cast Pointer
    
    if (PCIF_DATA_BUS_WITH == MXC_V_CAMERAIF_CTRL_DATA_WIDTH_8BIT) {
        MXC_DMA->ch[dma_handle].cnt = g_total_img_size;
    }
    else {
        MXC_DMA->ch[dma_handle].cnt = g_total_img_size * 2; // 10 and 12 bit use 2 bytes per word in the fifo
    }
    
    MXC_DMA->ch[dma_handle].ctrl = ((0x1 << MXC_F_DMA_CTRL_CTZ_IE_POS)  +
                                    (0x0 << MXC_F_DMA_CTRL_DIS_IE_POS)  +
                                    (0x3 << MXC_F_DMA_CTRL_BURST_SIZE_POS)    +
                                    (0x1 << MXC_F_DMA_CTRL_DSTINC_POS)  +
                                    (0x2 << MXC_F_DMA_CTRL_DSTWD_POS)   +
                                    (0x0 << MXC_F_DMA_CTRL_SRCINC_POS)  +
                                    (0x2 << MXC_F_DMA_CTRL_SRCWD_POS)   +
                                    (0x0 << MXC_F_DMA_CTRL_TO_CLKDIV_POS)   +
                                    (0x0 << MXC_F_DMA_CTRL_TO_WAIT_POS) +
                                    (0xD << MXC_F_DMA_CTRL_REQUEST_POS)  +
                                    (0x0 << MXC_F_DMA_CTRL_PRI_POS)     +
                                    (0x0 << MXC_F_DMA_CTRL_RLDEN_POS)   +
                                    (0x1 << MXC_F_DMA_CTRL_EN_POS)
                                   );

    MXC_DMA->inten = MXC_F_DMA_INTEN_CH0;
}

/******************************** Public Functions ***************************/
int camera_init(void)
{
    int ret = 0;
    
    // initialize XCLK for camera
    MXC_PT_Init(MXC_PT_CLK_DIV1);
    MXC_PT_SqrWaveConfig(0, CAMERA_FREQ);
    MXC_PT_Start(MXC_F_PTG_ENABLE_PT0);
    MXC_GPIO_SetVSSEL(gpio_cfg_pt0.port, MXC_GPIO_VSSEL_VDDIOH, gpio_cfg_pt0.mask);
    // Camera requires a delay after starting its input clock.
    MXC_Delay(MSEC(CAMERA_STARTUP_DELAY));
    MXC_PCIF->ctrl |= 0;
    // Initialize serial camera communication bus.
    sccb_init();
    // Register functions
    sensor_register(&camera);
    
    ret = camera.init();
    
    if (ret == 0) {
        ret |= camera.reset();
        ret |= camera.set_contrast(-2);
        ret |= camera.set_hmirror(1);
        ret |= camera.set_vflip(1);
    }
    
    if (ret == 0) {
        MXC_PCIF_Init(CAMERA_DATA_BIT_WIDTH);
        
        MXC_GPIO_SetVSSEL(gpio_cfg_pcif_vsync.port, MXC_GPIO_VSSEL_VDDIOH, gpio_cfg_pcif_vsync.mask);
        MXC_GPIO_SetVSSEL(gpio_cfg_pcif_hsync.port, MXC_GPIO_VSSEL_VDDIOH, gpio_cfg_pcif_hsync.mask);
        MXC_GPIO_SetVSSEL(gpio_cfg_pcif_P0_BITS_0_7.port, MXC_GPIO_VSSEL_VDDIOH, gpio_cfg_pcif_P0_BITS_0_7.mask);
        MXC_GPIO_SetVSSEL(gpio_cfg_pcif_xclk.port, MXC_GPIO_VSSEL_VDDIOH, gpio_cfg_pcif_xclk.mask);
        
        // Default to reading 8 bits of data from the camera.
        MXC_PCIF_SetDataWidth(MXC_PCIF_DATAWIDTH_8_BIT);
        MXC_PCIF_SetTimingSel(MXC_PCIF_TIMINGSEL_HSYNC_and_VSYNC);
        MXC_PCIF_SetThreshhold(FIFO_THRES_HOLD);
        
        MXC_SETFIELD(MXC_PCIF->ctrl, MXC_F_CAMERAIF_CTRL_PCIF_SYS, MXC_F_CAMERAIF_CTRL_PCIF_SYS);
        
        MXC_PCIF_EnableInt(MXC_F_CAMERAIF_INT_EN_IMG_DONE);
        #ifndef __riscv
            NVIC_SetVector(PCIF_IRQn, camera_irq_handler);
        #else
            __enable_irq();
            NVIC_EnableIRQ(PCIF_IRQn);
        #endif
    }
    
    return ret;
}

int camera_reset(void)
{
    return camera.reset();
}

int camera_setup(int xres, int yres, pixformat_t pixformat, fifomode_t fifo_mode, dmamode_t dma_mode)
{
    int ret = STATUS_OK;
    int bytes_per_pixel = 2;
    
    // Setup fifo mode.
    g_fifo_mode = fifo_mode;
    
    switch (g_fifo_mode) {
    case FIFO_THREE_BYTE:
        MXC_PCIF->ctrl |= MXC_F_CAMERAIF_CTRL_THREE_CH_EN;
        break;
        
    case FIFO_FOUR_BYTE:
        MXC_PCIF->ctrl &= ~MXC_F_CAMERAIF_CTRL_THREE_CH_EN;
        break;
        
    default:
        return -1;
    }

    // Setup DMA.
    g_dma_mode = dma_mode;
    
    g_pixel_format = pixformat;

    // Setup hardware bit expansion from rgb565 to rgb888 conversion.
    // If enabled then hardware will read in rgb565 from the camera
    // and treat it as rgb888.
    if (pixformat == PIXFORMAT_RGB888) {
        MXC_PCIF_SetDataWidth(MXC_PCIF_DATAWIDTH_12_BIT);
        // Bit expansion mode will yeild three bytes per pixel.
        if(g_dma_mode == USE_DMA) {
            bytes_per_pixel = 4;
        } else {
            bytes_per_pixel = 3;
        }
    }
    
    // Setup camera resolution and allocate a camera frame buffer.
    g_framesize_width = xres;
    g_framesize_height = yres;
    // Calculate the number of bytes for the frame buffer.
    g_total_img_size = g_framesize_width * g_framesize_height * bytes_per_pixel;
    
    // Free up memory if a camera frame buffer was previously allocated.
    if (rx_data != NULL) {
        free(rx_data);
        rx_data = NULL;
    }
    
    // Allocate memory with a buffer large enough for a camera frame.
    rx_data = (uint8_t*)malloc(g_total_img_size);
    
    if (rx_data == NULL) {
        // Return error if unable to allocate memory.
        return STATUS_ERROR_ALLOCATING;
    }
    memset((uint8_t*)rx_data, 0xff, g_total_img_size);

    if (g_dma_mode == USE_DMA) {
        MXC_DMA_Init();
        setup_dma();
        MXC_SETFIELD(MXC_PCIF->ctrl, MXC_F_CAMERAIF_CTRL_RX_DMA_THRSH, (0x1 << MXC_F_CAMERAIF_CTRL_RX_DMA_THRSH_POS));
        MXC_SETFIELD(MXC_PCIF->ctrl, MXC_F_CAMERAIF_CTRL_RX_DMA, MXC_F_CAMERAIF_CTRL_RX_DMA);
        MXC_PCIF_DisableInt(MXC_F_CAMERAIF_INT_EN_FIFO_THRESH);
    }
    else {
        MXC_SETFIELD(MXC_PCIF->ctrl, MXC_F_CAMERAIF_CTRL_RX_DMA, 0x0);
        MXC_PCIF_EnableInt(MXC_F_CAMERAIF_INT_EN_FIFO_THRESH);
    }
    camera.set_framesize(g_framesize_width, g_framesize_height);
    camera.set_pixformat(pixformat);
    return ret;
}

int camera_read_reg(uint8_t reg_addr, uint8_t* reg_data)
{
    return camera.read_reg(reg_addr, reg_data);
}

int camera_write_reg(uint8_t reg_addr, uint8_t reg_data)
{
    return camera.write_reg(reg_addr, reg_data);
}

int camera_get_slave_address(void)
{
    return camera.get_slave_address();
}

int camera_get_product_id(int* id)
{
    return camera.get_product_id(id);
}

int camera_get_manufacture_id(int* id)
{
    return camera.get_manufacture_id(id);
}

int camera_dump_registers(void)
{
    return camera.dump_registers();
}

int camera_start_capture_image(void)
{
    int ret = STATUS_OK;
    
    if (g_dma_mode == USE_DMA) {
        setup_dma();
    }
    
    // Clear flags.
    g_is_img_rcv = 0;
    rx_data_index = 0;
    MXC_PCIF->int_fl = MXC_PCIF->int_fl;
    MXC_PCIF_Start(MXC_PCIF_READMODE_SINGLE_MODE);
    return ret;
}

int camera_is_image_rcv(void)
{
    return (g_is_img_rcv) ? 1 : 0;
}

uint8_t* camera_get_pixel_format(void)
{
    pixformat_t pix_format;
    
    // Get the pixel format from the camers driver.
    camera.get_pixformat(&pix_format);
    
    if (pix_format == PIXFORMAT_RGB444) {
        return (uint8_t*)"RGB444";
    }
    else if (pix_format == PIXFORMAT_RGB565) {
        return (uint8_t*)"RGB565";
    }
    else if (pix_format == PIXFORMAT_RGB888) {
        return (uint8_t*)"RGB888";
    }
    else if (pix_format == PIXFORMAT_YUV422) {
        return (uint8_t*)"YUV422";
    }
    else if (pix_format == PIXFORMAT_BAYER) {
        return (uint8_t*)"BAYER";
    }
    
    return (uint8_t*)"INVALID";
}

void camera_get_image(uint8_t** img, uint32_t* imgLen, uint32_t* w, uint32_t* h)
{
    int n = 0, index;
    MXC_PCIF->int_fl |= MXC_PCIF->int_fl;
    if (g_dma_mode == USE_DMA && g_pixel_format == PIXFORMAT_RGB888) {
        // Filter image data
        index = 4;
        for(n=3; n<((g_framesize_width )*(g_framesize_height)*3); n++) {
            index++;
            if(index%4 == 0) {
                index++;
            }
            rx_data[n] = rx_data[index -1];
        }
    }
    *img    = (uint8_t*)rx_data;
    *imgLen = g_total_img_size;
    
    *w = g_framesize_width;
    *h = g_framesize_height;
}

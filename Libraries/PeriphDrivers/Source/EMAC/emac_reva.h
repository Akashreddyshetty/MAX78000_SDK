/* ****************************************************************************
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
 *************************************************************************** */

#ifndef _EMAC_REVA_H_
#define _EMAC_REVA_H_

/* **** Includes **** */
#include <stddef.h>

/* **** Definitions **** */
#define MAX_SYS_EMAC_RX_BUFFER_SIZE             16384
#define MAX_SYS_EMAC_RX_RING_SIZE               128
#define MAX_SYS_EMAC_TX_RING_SIZE               16
#define EMAC_RX_BUFFER_SIZE                     128
#define CONFIG_SYS_EMAC_TX_TIMEOUT              1000        /**! Transmission Timeout in Microseconds     */
#define CONFIG_SYS_EMAC_AUTONEG_TIMEOUT         500000      /**! Auto Negotiation Timeout in Microseconds */

#define RXADDR_USED                             0x00000001
#define RXADDR_WRAP                             0x00000002
#define RXBUF_FRMLEN_MASK                       0x00000fff
#define RXBUF_FRAME_START                       0x00004000
#define RXBUF_FRAME_END                         0x00008000
#define RXBUF_TYPEID_MATCH                      0x00400000
#define RXBUF_ADDR4_MATCH                       0x00800000
#define RXBUF_ADDR3_MATCH                       0x01000000
#define RXBUF_ADDR2_MATCH                       0x02000000
#define RXBUF_ADDR1_MATCH                       0x04000000
#define RXBUF_BROADCAST                         0x80000000
#define TXBUF_FRMLEN_MASK                       0x000007ff
#define TXBUF_FRAME_END                         0x00008000
#define TXBUF_NOCRC                             0x00010000
#define TXBUF_EXHAUSTED                         0x08000000
#define TXBUF_UNDERRUN                          0x10000000
#define TXBUF_MAXRETRY                          0x20000000
#define TXBUF_WRAP                              0x40000000
#define TXBUF_USED                              0x80000000

/* Definitions for MII-Compatible Transceivers */
#define MII_BMCR                                0x00        /**! Basic Mode Control Register    */
#define MII_BMSR                                0x01        /**! Basic Mode Status Register     */
#define MII_PHYSID1                             0x02        /**! PHYS ID 1                      */
#define MII_PHYSID2                             0x03        /**! PHYS ID 2                      */
#define MII_ADVERTISE                           0x04        /**! Advertisement Control Register */
#define MII_LPA                                 0x05        /**! Link Partner Ability Register  */

/* Basic Mode Control Register */
#define BMCR_ANRESTART                          0x0200      /**! Auto Negotiation Restart       */
#define BMCR_ANENABLE                           0x1000      /**! Enable Auto Negotiation        */

/* Basic Mode Status Register */
#define BMSR_LSTATUS                            0x0004      /**! Link Status                    */
#define BMSR_ANEGCOMPLETE                       0x0020      /**! Auto Negotiation Complete      */

/* Advertisement Control Register */
#define ADVERTISE_CSMA                          0x0001
#define ADVERTISE_10HALF                        0x0020
#define ADVERTISE_10FULL                        0x0040
#define ADVERTISE_100HALF                       0x0080
#define ADVERTISE_100FULL                       0x0100
#define ADVERTISE_FULL                          ( ADVERTISE_100FULL |\
                                                  ADVERTISE_10FULL  |\
                                                  ADVERTISE_CSMA )
#define ADVERTISE_ALL                           ( ADVERTISE_10HALF  |\
                                                  ADVERTISE_10FULL  |\
                                                  ADVERTISE_100HALF |\
                                                  ADVERTISE_100FULL )

/* Link Partner Ability Register */
#define LPA_10HALF                              0x0020
#define LPA_10FULL                              0x0040
#define LPA_100HALF                             0x0080
#define LPA_100FULL                             0x0100
#define LPA_100BASE4                            0x0200

/* Constants for CLK */
#define EMAC_CLK_DIV8                           0
#define EMAC_CLK_DIV16                          1
#define EMAC_CLK_DIV32                          2
#define EMAC_CLK_DIV64                          3

/* **** Macros **** */
/* Bit Manipulation */
#define EMAC_BIT(reg, name)                     (1 << MXC_F_EMAC_##reg##_##name##_POS)
#define EMAC_BF(reg, name, value)               (((value) & (MXC_F_EMAC_##reg##_##name >> MXC_F_EMAC_##reg##_##name##_POS)) << MXC_F_EMAC_##reg##_##name##_POS)
#define EMAC_BFEXT(reg, name, value)            (((value) >> MXC_F_EMAC_##reg##_##name##_POS) & (MXC_F_EMAC_##reg##_##name >> MXC_F_EMAC_##reg##_##name##_POS))

/* Register Access */
#define REG_READL(a)                            (*(volatile uint32_t *)(a))
#define REG_WRITEL(v, a)                        (*(volatile uint32_t *)(a) = (v))
#define EMAC_READL(port, reg)                   REG_READL(&(port)->regs->reg)
#define EMAC_WRITEL(port, reg, value)           REG_WRITEL((value), &(port)->regs->reg)

/* Misc */
#define barrier()                               asm volatile("" ::: "memory")

/* **** Function Prototypes **** */
/* ************************************************************************* */
/* Control/Configuration Functions                                           */
/* ************************************************************************* */
int MXC_EMAC_RevA_Init (mxc_emac_config_t *config);
int MXC_EMAC_RevA_SetConfiguration (mxc_emac_config_t *config);
int MXC_EMAC_RevA_SetHwAddr (unsigned char *enetaddr);
int MXC_EMAC_RevA_EnableInterruptEvents (unsigned int events);
int MXC_EMAC_RevA_DisableInterruptEvents (unsigned int events);

/* ************************************************************************* */
/* Low-Level Functions                                                       */
/* ************************************************************************* */
int MXC_EMAC_RevA_Start (void);
int MXC_EMAC_RevA_Stop (void);
int MXC_EMAC_RevA_ReadLinkStatus (void);

/* ************************************************************************* */
/* Transaction-Level Functions                                               */
/* ************************************************************************* */
int MXC_EMAC_RevA_SendSync (const void *packet, unsigned int length);
int MXC_EMAC_RevA_SendAsync (const void *packet, unsigned int length);
int MXC_EMAC_RevA_Recv (void *rx_buff, unsigned int max_len);
void MXC_EMAC_RevA_IrqHandler (void);

#endif /* _EMAC_REVA_H_ */

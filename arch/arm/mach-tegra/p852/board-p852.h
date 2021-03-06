/*
 * arch/arm/mach-tegra/board-p852.h
 *
 * Copyright (c) 2010, NVIDIA Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _MACH_TEGRA_BOARD_P852M_H
#define _MACH_TEGRA_BOARD_P852M_H

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/serial_8250.h>
#include <linux/clk.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/i2c-tegra.h>
#include <linux/tegra_usb.h>
#include <linux/gpio.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <mach/usb_phy.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/flash.h>

#include <mach/sdhci.h>
#include <mach/iomap.h>
#include <mach/irqs.h>
#include <mach/nand.h>
#include <mach/nor.h>
#include <mach/clk.h>
#include <mach/i2s.h>
#include <mach/audio.h>
#include <mach/spi_i2s.h>
#include <mach/tegra_das.h>

#include "../clock.h"
#include "../board.h"
#include "../devices.h"
#include "../gpio-names.h"
#include "../power.h"
#include "../wakeups-t2.h"



int p852_regulator_init(void);
int p852_panel_init(void);
void p852_sdhci_init(void);
void p852_i2c_init(void);
void p852_i2c_set_default_clock(int adapter, unsigned long clock);
void p852_pinmux_init(void);
void p852_gpio_init(void);
void p852_power_off_init(void);

extern unsigned int p852_sku_peripherals;
extern unsigned int p852_spi_peripherals;
extern unsigned int p852_i2s_peripherals;
extern unsigned int p852_uart_peripherals;
extern unsigned int p852_sdhci_peripherals;
extern unsigned int p852_display_peripherals;
extern unsigned int p852_i2c_peripherals;
extern struct tegra_sdhci_platform_data p852_sdhci_platform_data[];
extern struct platform_device tegra_8250_uarta_device;
extern struct platform_device tegra_8250_uartb_device;
extern struct platform_device tegra_8250_uartc_device;
extern struct platform_device tegra_8250_uartd_device;
extern struct platform_device tegra_8250_uarte_device;

extern void p852_spi_i2s_init(void);
void tegra_p852_fixup(struct machine_desc *desc,
	struct tag *tags, char **cmdline, struct meminfo *mi);

void p852_common_init(void);

/* True for sku8 with any revs */
#define IS_P852_SKU8 (machine_is_p852_sku8_b00()	\
	|| machine_is_p852_sku8_c01())

#define IS_P852_SKU9 (machine_is_p852_sku9_b00()	\
	|| machine_is_p852_sku9_c01())

#define IS_P852_SKU1 (machine_is_p852_sku1_b00()	\
	|| machine_is_p852_sku1()			\
	|| machine_is_p852_sku1_c0x())


#define P852_SDIO3_PINMUX_ENABLE 0x01

#define P852_SKU_SPI_SHIFT	0x00
#define P852_SKU_SPI_ENABLE	(1 << P852_SKU_SPI_SHIFT)
#define P852_SKU_SPI_MASK	(1 << P852_SKU_SPI_SHIFT)

#define P852_SKU_I2S_SHIFT	0x01
#define P852_SKU_I2S_ENABLE	(1 << P852_SKU_I2S_SHIFT)
#define P852_SKU_I2S_MASK	(1 << P852_SKU_I2S_SHIFT)

#define P852_SKU_SDHCI_SHIFT    0x02
#define P852_SKU_SDHCI_ENABLE   (1 << P852_SKU_SDHCI_SHIFT)
#define P852_SKU_SDHCI_MASK     (1 << P852_SKU_SDHCI_SHIFT)

#define P852_SKU_UART_SHIFT	0x03
#define P852_SKU_UART_ENABLE	(1 << P852_SKU_UART_SHIFT)
#define P852_SKU_UART_MASK	(1 << P852_SKU_UART_SHIFT)

#define P852_SKU_NAND_SHIFT	0x04
#define P852_SKU_NAND_ENABLE	(1 << P852_SKU_NAND_SHIFT)
#define P852_SKU_NAND_MASK	(1 << P852_SKU_NAND_SHIFT)

#define P852_SKU_NOR_SHIFT	0x05
#define P852_SKU_NOR_ENABLE	(1 << P852_SKU_NOR_SHIFT)
#define P852_SKU_NOR_MASK	(1 << P852_SKU_NOR_SHIFT)

#define P852_SKU_DISPLAY_SHIFT 		0x06
#define P852_SKU_DISPLAY_ENABLE		(1 << P852_SKU_DISPLAY_SHIFT)
#define P852_SKU_DISPLAY_MASK		(1 << P852_SKU_DISPLAY_SHIFT)

#define P852_SKU_ULPI_SHIFT 	0x07
#define P852_SKU_ULPI_DISABLE 		(1 << P852_SKU_ULPI_SHIFT)

#define P852_SKU_I2C_SHIFT 	0x08
#define P852_SKU_I2C_ENABLE	(1 << P852_SKU_I2C_SHIFT)
#define P852_SKU_I2C_MASK	(1 << P852_SKU_I2C_SHIFT)

#define P852_MAX_DISP		0x2
#define P852_DISP_SHIFT		0x16
#define P852_DISPA_SHIFT	0x0
#define P852_DISPB_SHIFT	0x16

#define P852_DISP_MASK		0x1
#define P852_DISP_ENABLE	0x1
#define P852_DISPA_MASK		(P852_DISP_MASK << P852_DISPA_SHIFT)
#define P852_DISPB_MASK		(P852_DISP_MASK << P852_DISPB_SHIFT)

#define P852_MAX_SPI		0x04
#define P852_SPI_SHIFT		0x03
#define P852_SPI1_SHIFT		0x00
#define P852_SPI2_SHIFT		0x03
#define P852_SPI3_SHIFT		0x06
#define P852_SPI4_SHIFT		0x09

#define P852_SPI_MASK		0x07
#define P852_SPI1_MASK		(P852_SPI_MASK << P852_SPI1_SHIFT)
#define P852_SPI2_MASK		(P852_SPI_MASK << P852_SPI2_SHIFT)
#define P852_SPI3_MASK		(P852_SPI_MASK << P852_SPI3_SHIFT)
#define P852_SPI4_MASK		(P852_SPI_MASK << P852_SPI4_SHIFT)

#define P852_SPI_ENABLE		0x01
#define P852_SPI_MASTER		0x02
#define P852_SPI_SLAVE		0x04

#define P852_I2S_SHIFT		0x05
#define P852_I2S1_SHIFT		0x00
#define P852_I2S2_SHIFT		0x05

#define P852_I2S_MASK		0x1F
#define P852_I2S1_MASK		(P852_I2S_MASK << P852_I2S1_SHIFT)
#define P852_I2S2_MASK		(P852_I2S_MASK << P852_I2S2_SHIFT)

#define P852_I2S_ENABLE		0x10
#define P852_I2S_TDM		0x08
#define P852_MAX_SDHCI		0x04
#define P852_SDHCI_SHIFT        0x04
#define P852_SDHCI1_SHIFT       0x00
#define P852_SDHCI2_SHIFT       0x04
#define P852_SDHCI3_SHIFT       0x08
#define P852_SDHCI4_SHIFT       0x0C

#define P852_SDHCI_MASK		0x0F
#define P852_SDHCI1_MASK	(P852_SDHCI_MASK << P852_SDHCI1_SHIFT)
#define P852_SDHCI2_MASK	(P852_SDHCI_MASK << P852_SDHCI2_SHIFT)
#define P852_SDHCI3_MASK	(P852_SDHCI_MASK << P852_SDHCI3_SHIFT)
#define P852_SDHCI4_MASK	(P852_SDHCI_MASK << P852_SDHCI4_SHIFT)

#define P852_SDHCI_ENABLE       0x01
#define P852_SDHCI_CD_EN	0x02
#define P852_SDHCI_WP_EN	0x04
#define P852_SDHCI_PW_EN        0x08

#define P852_UART_SHIFT		0x04
#define P852_UARTA_SHIFT	0x00
#define P852_UARTB_SHIFT	0x04
#define P852_UARTC_SHIFT	0x08
#define P852_UARTD_SHIFT	0x0C

#define P852_UART_MASK		0x0F
#define P852_UARTA_MASK		(P852_UART_MASK << P852_UARTA_SHIFT)
#define P852_UARTB_MASK		(P852_UART_MASK << P852_UARTB_SHIFT)
#define P852_UARTC_MASK		(P852_UART_MASK << P852_UARTC_SHIFT)
#define P852_UARTD_MASK		(P852_UART_MASK << P852_UARTD_SHIFT)

#define P852_MAX_UART		0x4
#define P852_UART_ALT_PIN_CFG 	0x8
#define P852_UART_ENABLE	0x4
#define P852_UART_DB		0x1
#define P852_UART_HS		0x2

#define P852_MAX_I2C		0x4
#define P852_I2C_SHIFT		0x01
#define P852_I2C1_SHIFT		0x00
#define P852_I2C2_SHIFT		0x01
#define P852_I2C3_SHIFT		0x02
#define P852_I2C4_SHIFT		0x03


#define P852_I2C_MASK		0x01
#define P852_I2C1_MASK		(P852_I2C_MASK << P852_I2C1_SHIFT)
#define P852_I2C2_MASK		(P852_I2C_MASK << P852_I2C2_SHIFT)
#define P852_I2C3_MASK		(P852_I2C_MASK << P852_I2C3_SHIFT)
#define P852_I2C4_MASK		(P852_I2C_MASK << P852_I2C4_SHIFT)

#define P852_I2C_ENABLE		0x01

#endif

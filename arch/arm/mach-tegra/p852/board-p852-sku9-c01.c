/*
 * arch/arm/mach-tegra/board-p852-sku9-c00.c
 *
 * Copyright (C) 2010, NVIDIA Corporation.
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

#include "board-p852.h"

static inline void p852_sku9_c00_spi_init(void)
{
	p852_sku_peripherals |= P852_SKU_SPI_ENABLE;
	p852_spi_peripherals |=
	    ((P852_SPI_SLAVE  | P852_SPI_ENABLE) << P852_SPI1_SHIFT) |
	    ((P852_SPI_MASTER | P852_SPI_ENABLE) << P852_SPI3_SHIFT);
}

static inline void p852_sku9_c00_i2s_init(void)
{
	p852_sku_peripherals |= P852_SKU_I2S_ENABLE;
	p852_i2s_peripherals |= ((P852_I2S_ENABLE | P852_I2S_TDM) << P852_I2S1_SHIFT) |
	    ((P852_I2S_ENABLE | P852_I2S_TDM) << P852_I2S2_SHIFT);
}

static inline void p852_sku9_c00_sdhci_init(void)
{
	p852_sku_peripherals |= P852_SKU_SDHCI_ENABLE;
	p852_sdhci_peripherals |=
	    ((P852_SDHCI_ENABLE)
	     << P852_SDHCI4_SHIFT) |
	    ((P852_SDHCI_ENABLE | P852_SDHCI_CD_EN | P852_SDHCI_WP_EN)
	     << P852_SDHCI1_SHIFT) |
	    ((P852_SDHCI_ENABLE)
	     << P852_SDHCI2_SHIFT);

	p852_sdhci_platform_data[0].bus_width = 4;
	p852_sdhci_platform_data[0].cd_gpio = TEGRA_GPIO_PV0;
	p852_sdhci_platform_data[0].wp_gpio = TEGRA_GPIO_PV1;
}

static inline void p852_sku9_c00_uart_init(void)
{
	p852_sku_peripherals |= P852_SKU_UART_ENABLE;
	p852_uart_peripherals |=
	    ((P852_UART_ENABLE | P852_UART_HS) << P852_UARTD_SHIFT) |
	    ((P852_UART_ENABLE | P852_UART_DB) << P852_UARTB_SHIFT);
}

static inline void p852_sku9_c00_display_init(void)
{
	p852_sku_peripherals |= P852_SKU_DISPLAY_ENABLE;
}

static inline void p852_sku9_c00_ulpi_init(void)
{
	p852_sku_peripherals |= P852_SKU_ULPI_DISABLE;
}

static inline void p852_sku9_c00_i2c_init(void)
{
	p852_sku_peripherals |= P852_SKU_I2C_ENABLE;
	p852_i2c_peripherals |=
	    ((P852_I2C_ENABLE) << P852_I2C1_SHIFT) |
	    ((P852_I2C_ENABLE) << P852_I2C2_SHIFT) |
	    ((P852_I2C_ENABLE) << P852_I2C4_SHIFT);
}

static void __init p852_sku9_c00_init(void)
{
	extern struct tegra_audio_platform_data tegra_i2s_audio_pdata[];

	tegra_i2s_audio_pdata[0].dap_port_num = 1;
	tegra_i2s_audio_pdata[1].dap_port_num = 3;

	p852_sku_peripherals |= P852_SKU_NOR_ENABLE;

	p852_sku9_c00_spi_init();
	p852_sku9_c00_i2s_init();
	p852_sku9_c00_uart_init();
	p852_sku9_c00_sdhci_init();
	p852_sku9_c00_display_init();
	p852_sku9_c00_ulpi_init();
	p852_sku9_c00_i2c_init();

	p852_common_init();
}

MACHINE_START(P852_SKU9_C01, "Tegra P852 SKU9 C01")
	.boot_params  = 0x00000100,
	.phys_io      = IO_APB_PHYS,
	.io_pg_offst  = ((IO_APB_VIRT) >> 18) & 0xfffc,
	.fixup        = tegra_p852_fixup,
	.init_irq     = tegra_init_irq,
	.init_machine = p852_sku9_c00_init,
	.map_io       = tegra_map_common_io,
	.timer        = &tegra_timer,
MACHINE_END

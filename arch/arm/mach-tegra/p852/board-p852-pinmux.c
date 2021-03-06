/*
 * arch/arm/mach-tegra/board-p852-pinmux.c
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

#include <linux/kernel.h>
#include <linux/init.h>
#include <mach/pinmux.h>
#include <asm/mach-types.h>
#include <mach/board_id.h>

#include "board-p852.h"

#define DEFAULT_DRIVE(_name)					\
	{							\
		.pingroup = TEGRA_DRIVE_PINGROUP_##_name,	\
		.hsm = TEGRA_HSM_DISABLE,			\
		.schmitt = TEGRA_SCHMITT_ENABLE,		\
		.drive = TEGRA_DRIVE_DIV_1,			\
		.pull_down = TEGRA_PULL_31,			\
		.pull_up = TEGRA_PULL_31,			\
		.slew_rising = TEGRA_SLEW_SLOWEST,		\
		.slew_falling = TEGRA_SLEW_SLOWEST,		\
	}

#define SET_DRIVE(_name, _hsm, _schmitt, _drive, _pulldn_drive, _pullup_drive, _pulldn_slew, _pullup_slew)	\
	{							\
		.pingroup = TEGRA_DRIVE_PINGROUP_##_name,	\
		.hsm = TEGRA_HSM_##_hsm,			\
		.schmitt = TEGRA_SCHMITT_##_schmitt,		\
		.drive = TEGRA_DRIVE_##_drive,			\
		.pull_down = TEGRA_PULL_##_pulldn_drive,	\
		.pull_up = TEGRA_PULL_##_pullup_drive,		\
		.slew_rising = TEGRA_SLEW_##_pullup_slew,	\
		.slew_falling = TEGRA_SLEW_##_pulldn_slew,	\
	}

#define SKU9_LCD_DRIVE(_name)                              \
        {                                                       \
                .pingroup = TEGRA_DRIVE_PINGROUP_##_name,       \
	        .hsm = TEGRA_HSM_DISABLE,                       \
	        .schmitt = TEGRA_SCHMITT_ENABLE,                \
	        .drive = TEGRA_DRIVE_DIV_2,                     \
	        .pull_down = TEGRA_PULL_31,                     \
	        .pull_up = TEGRA_PULL_31,                       \
	        .slew_rising = TEGRA_MAX_SLEW,                  \
	        .slew_falling = TEGRA_MAX_SLEW,                 \
	}

void tegra_pinmux_config_pupd_table(const struct tegra_pingroup_config *config, int len);

static __initdata struct tegra_drive_pingroup_config p852_drive_pinmux[] = {
	SET_DRIVE(DDC, DISABLE, ENABLE, DIV_4, 0, 31, SLOWEST, SLOWEST),
	DEFAULT_DRIVE(VI1),
	DEFAULT_DRIVE(VI2),
	DEFAULT_DRIVE(SDIO1),
	SET_DRIVE(SPI, DISABLE, DISABLE, DIV_1, 10, 10, SLOWEST, SLOWEST),
	SET_DRIVE(DAP1, DISABLE, ENABLE, DIV_1, 3, 3, SLOWEST, SLOWEST),
	SET_DRIVE(DAP2, DISABLE, DISABLE, DIV_1, 10, 10, SLOWEST, SLOWEST),
	DEFAULT_DRIVE(CDEV1),
	DEFAULT_DRIVE(CDEV2),
	SET_DRIVE(UAA, DISABLE, DISABLE, DIV_8, 0, 0, FASTEST, FASTEST),
	SET_DRIVE(UAB, DISABLE, DISABLE, DIV_8, 0, 0, FASTEST, FASTEST),
	SET_DRIVE(UAD, DISABLE, DISABLE, DIV_8, 0, 0, FASTEST, FASTEST),
};

static __initdata struct tegra_drive_pingroup_config p852_drive_pinmux_sku9_lcd[] = {
	SKU9_LCD_DRIVE(LCD1),
	SKU9_LCD_DRIVE(LCD2),
};

static __initdata struct tegra_drive_pingroup_config p852_drive_pinmux_sku8_sku9[] = {
	DEFAULT_DRIVE(DBG),
	SET_DRIVE(DAP3, DISABLE, ENABLE, DIV_1, 3, 3, SLOWEST, SLOWEST),
	SET_DRIVE(AT1, DISABLE, ENABLE, DIV_1, 0, 0,SLOWEST, SLOWEST),
	SET_DRIVE(AT2, DISABLE, ENABLE, DIV_1, 0, 0,SLOWEST, SLOWEST),
	SET_DRIVE(DAP4, DISABLE, DISABLE, DIV_1, 0, 0,SLOWEST, SLOWEST),
	SET_DRIVE(UART3, DISABLE, DISABLE, DIV_1, 0, 0,SLOWEST, SLOWEST),
	SET_DRIVE(GMD, DISABLE, DISABLE, DIV_1, 10, 10, SLOWEST, SLOWEST),
	SET_DRIVE(GMA, DISABLE, ENABLE, DIV_1, 0, 0, SLOWEST, SLOWEST),
	SET_DRIVE(GME, DISABLE, ENABLE, DIV_1, 0, 0, SLOWEST, SLOWEST),
};

static __initdata struct tegra_drive_pingroup_config p852_drive_pinmux_sku8_rev602[] = {
	SET_DRIVE(DAP3, DISABLE, ENABLE, DIV_1, 3, 3, SLOWEST, SLOWEST),
	SET_DRIVE(AT1, DISABLE, ENABLE, DIV_1, 0, 0, SLOWEST, SLOWEST),
	SET_DRIVE(AT2, DISABLE, ENABLE, DIV_1, 0, 0, SLOWEST, SLOWEST),
	SET_DRIVE(DAP4, DISABLE, DISABLE, DIV_1, 0, 0, SLOWEST, SLOWEST),
	SET_DRIVE(GMD, DISABLE, DISABLE, DIV_1, 10, 10, SLOWEST, SLOWEST),
	SET_DRIVE(GMA, DISABLE, ENABLE, DIV_1, 0, 0, SLOWEST, SLOWEST),
	SET_DRIVE(GME, DISABLE, ENABLE, DIV_1, 0, 0, SLOWEST, SLOWEST),
	SET_DRIVE(UART2, DISABLE, DISABLE, DIV_8, 0, 0, FASTEST, FASTEST),
	SET_DRIVE(UART3, DISABLE, DISABLE, DIV_4, 0, 0, SLOWEST, SLOWEST),
	SET_DRIVE(DBG, DISABLE, ENABLE, DIV_8, 0, 0, FASTEST, FASTEST),
};


static __initdata struct tegra_pingroup_config p852_common_pinmux[] = {
	{TEGRA_PINGROUP_SDIO1, TEGRA_MUX_SDIO1,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LSDI,  TEGRA_MUX_SPI3,          TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LSDA,  TEGRA_MUX_SPI3,          TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LCSN,  TEGRA_MUX_SPI3,          TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LSCK,  TEGRA_MUX_SPI3,          TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_DTA,   TEGRA_MUX_VI,            TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DTC,   TEGRA_MUX_VI,            TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DTD,   TEGRA_MUX_VI,            TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_CSUS,  TEGRA_MUX_VI_SENSOR_CLK, TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_DTB,   TEGRA_MUX_SPI1,          TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DTF,   TEGRA_MUX_I2C3,          TEGRA_PUPD_PULL_UP,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DTE,   TEGRA_MUX_SPI1,          TEGRA_PUPD_PULL_UP,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DDC,   TEGRA_MUX_I2C2,          TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
        /* IRDA is same as UART2 option for the pingroup */
	{TEGRA_PINGROUP_UAD,   TEGRA_MUX_IRDA,          TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_RM,    TEGRA_MUX_I2C,           TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_KBCA,  TEGRA_MUX_SDIO2,         TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_KBCD,  TEGRA_MUX_SDIO2,         TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_KBCB,  TEGRA_MUX_SDIO2,         TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_I2CP,  TEGRA_MUX_I2C,           TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_GPU7,  TEGRA_MUX_RTCK,          TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DAP1,  TEGRA_MUX_DAP1,          TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_SPDI,  TEGRA_MUX_SPDIF,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SPIF,  TEGRA_MUX_SPI2,          TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_XM2D,  TEGRA_MUX_NONE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_XM2C,  TEGRA_MUX_NONE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_ATB,   TEGRA_MUX_SDIO4,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_GMA,   TEGRA_MUX_SDIO4,         TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_GME,   TEGRA_MUX_SDIO4,         TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_CK32,  TEGRA_MUX_NONE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DDRC,  TEGRA_MUX_NONE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_PMCA,  TEGRA_MUX_NONE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_PMCB,  TEGRA_MUX_NONE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_PMCC,  TEGRA_MUX_NONE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_PMCD,  TEGRA_MUX_NONE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_PMCE,  TEGRA_MUX_NONE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_UAC,   TEGRA_MUX_OWR,           TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_HDINT, TEGRA_MUX_HDMI,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_CRTP,  TEGRA_MUX_CRT,           TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_KBCC,  TEGRA_MUX_KBC,           TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_KBCF,  TEGRA_MUX_KBC,           TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_KBCE,  TEGRA_MUX_KBC,           TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_PMC,   TEGRA_MUX_PWR_ON,        TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_OWC,   TEGRA_MUX_RSVD2,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_CDEV1, TEGRA_MUX_PLLA_OUT,      TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SPDO,  TEGRA_MUX_RSVD2,         TEGRA_PUPD_NORMAL,    TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_CDEV2, TEGRA_MUX_PLLP_OUT4,     TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_SPIG,  TEGRA_MUX_SPI2_ALT,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SPIH,  TEGRA_MUX_SPI2_ALT,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_GPV,   TEGRA_MUX_PCIE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_GMD,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_PTA,   TEGRA_MUX_HDMI,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_pingroup_config p852_nand_pinmux[] = {
	{TEGRA_PINGROUP_IRRX,  TEGRA_MUX_UARTB,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_IRTX,  TEGRA_MUX_UARTB,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_UCA,   TEGRA_MUX_UARTC,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_UCB,   TEGRA_MUX_UARTC,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_GPU,   TEGRA_MUX_UARTA,         TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DAP2,  TEGRA_MUX_DAP2,          TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_SPID,  TEGRA_MUX_SPI2_ALT,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SPIE,  TEGRA_MUX_SPI2_ALT,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_ATC,   TEGRA_MUX_NAND,          TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_ATA,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_GMB,   TEGRA_MUX_NAND,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DAP4,  TEGRA_MUX_GMI,           TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_SPIA,  TEGRA_MUX_GMI,           TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_SPIB,  TEGRA_MUX_GMI,           TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_SPIC,  TEGRA_MUX_GMI,           TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_ATD,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,    TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_ATE,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,    TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_DAP3,  TEGRA_MUX_DAP3,          TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_TRISTATE},
};

static __initdata struct tegra_pingroup_config p852_sdio3_pinmux[] = {
	{TEGRA_PINGROUP_SDD,   TEGRA_MUX_SDIO3,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SDC,   TEGRA_MUX_SDIO3,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SDB,   TEGRA_MUX_SDIO3,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_pingroup_config p852_spi3_pinmux[] = {
	{TEGRA_PINGROUP_SDD,   TEGRA_MUX_SPI3,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SDC,   TEGRA_MUX_SPI3,         TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_pingroup_config p852_uarta_pinmux[] = {
	{TEGRA_PINGROUP_SDD,   TEGRA_MUX_PWM,           TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SDC,   TEGRA_MUX_PWM,           TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SDB,   TEGRA_MUX_PWM,           TEGRA_PUPD_NORMAL,    TEGRA_TRI_TRISTATE},
};

static __initdata struct tegra_pingroup_config p852_ulpi_pinmux[] = {
	{TEGRA_PINGROUP_UAA,   TEGRA_MUX_ULPI,          TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_UAB,   TEGRA_MUX_ULPI,          TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_UDA,   TEGRA_MUX_ULPI,          TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_pingroup_config p852_uarta_1_pinmux[] = {
	{TEGRA_PINGROUP_UAA,   TEGRA_MUX_UARTA,         TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_UAB,   TEGRA_MUX_ULPI,          TEGRA_PUPD_NORMAL,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_UDA,   TEGRA_MUX_ULPI,          TEGRA_PUPD_NORMAL,   TEGRA_TRI_TRISTATE},
};

static __initdata struct tegra_pingroup_config p852_uartd_pinmux[] = {
	{TEGRA_PINGROUP_GMC,   TEGRA_MUX_UARTD,         TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_pingroup_config p852_spi4_pinmux[] = {
	{TEGRA_PINGROUP_GMC,   TEGRA_MUX_SPI4,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SLXA,  TEGRA_MUX_PCIE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_SLXK,  TEGRA_MUX_PCIE,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_SLXC,  TEGRA_MUX_SPI4,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SLXD,  TEGRA_MUX_SPDIF,         TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_pingroup_config p852_spi4_1_pinmux[] = {
	{TEGRA_PINGROUP_SLXA,   TEGRA_MUX_SPI4,          TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SLXK,   TEGRA_MUX_SPI4,          TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SLXC,   TEGRA_MUX_SPI4,          TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SLXD,   TEGRA_MUX_SPI4,          TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_pingroup_config p852_nor_pinmux[] = {
	{TEGRA_PINGROUP_IRRX,  TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_IRTX,  TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_UCA,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_UCB,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_GPU,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DAP2,  TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SPID,  TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SPIE,  TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_ATC,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_ATA,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_GMB,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DAP4,  TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SPIA,  TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SPIB,  TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SPIC,  TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_ATD,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_ATE,   TEGRA_MUX_GMI,           TEGRA_PUPD_NORMAL,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DAP3,  TEGRA_MUX_DAP3,          TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_pingroup_config p852_display_a_pinmux[] = {
	{TEGRA_PINGROUP_LSPI,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LSC0,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHS,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LVS,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD0,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD1,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD2,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD3,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD4,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD5,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD6,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD7,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD8,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD9,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD10,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD11,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD12,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD13,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD14,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD15,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD16,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD17,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHP1,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHP2,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LVP1,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHP0,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LDI,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LPP,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LM1,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LPW0,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LSC1,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LVP0,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LM0,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LDC,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LPW2,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LPW1,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
};

static __initdata struct tegra_pingroup_config p852_display_b_pinmux[] = {
	{TEGRA_PINGROUP_LPW1,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LPW2,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LDC,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LM0,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LVP0,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LSC1,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LM1,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LSPI,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LSC0,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHS,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LVS,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD0,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD1,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD2,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD3,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD4,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD5,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD6,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD7,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD8,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD9,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD10,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD11,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD12,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD13,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD14,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD15,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD16,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD17,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHP1,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHP2,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LVP1,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHP0,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LDI,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LPP,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LM1,   TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LPW0,  TEGRA_MUX_DISPLAYB,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_drive_pingroup_config p852_drive_pinmux_sku23[] = {
	DEFAULT_DRIVE(DBG),
	SET_DRIVE(SDMMC3, ENABLE, ENABLE, DIV_1, 31, 31, SLOWEST, SLOWEST),
};

static __initdata struct tegra_drive_pingroup_config p852_drive_pinmux_sku13[] = {
	DEFAULT_DRIVE(DBG),
	SET_DRIVE(SDMMC3, ENABLE, ENABLE, DIV_1, 31, 31, SLOWEST, SLOWEST),
};

static __initdata struct tegra_pingroup_config p852_pupd_sku23[] = {
	{TEGRA_PINGROUP_GPV,   TEGRA_MUX_NONE,          TEGRA_PUPD_PULL_UP,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_GMC,   TEGRA_MUX_NONE,          TEGRA_PUPD_PULL_UP,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DTB,   TEGRA_MUX_NONE,          TEGRA_PUPD_PULL_UP,    TEGRA_TRI_NORMAL},
};


static __initdata struct tegra_pingroup_config p852_pupd_sku13[] = {
	{TEGRA_PINGROUP_GPV,   TEGRA_MUX_NONE,          TEGRA_PUPD_PULL_UP,    TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_pingroup_config p852_pupd_sku5[] = {
	{TEGRA_PINGROUP_GMC,   TEGRA_MUX_NONE,          TEGRA_PUPD_PULL_UP,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_DTB,   TEGRA_MUX_NONE,          TEGRA_PUPD_PULL_UP,    TEGRA_TRI_NORMAL},
};

static __initdata struct tegra_pingroup_config p852_sku8_sku9_eth_pinmux[] = {
	{TEGRA_PINGROUP_SLXC,  TEGRA_MUX_SPI4,          TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SLXD,  TEGRA_MUX_SPDIF,         TEGRA_PUPD_NORMAL,    TEGRA_TRI_NORMAL},
};

void __init p852_pinmux_init(void)
{
	bool is_sku_rev;
	unsigned int sdio3_config = (p852_sdhci_peripherals >> P852_SDHCI3_SHIFT)
					& P852_SDHCI_MASK;
	unsigned int uartd_config = (p852_uart_peripherals >> P852_UARTD_SHIFT)
					& P852_UART_MASK;
	unsigned int uarta_config = (p852_uart_peripherals >> P852_UARTA_SHIFT)
					& P852_UART_MASK;
	unsigned int spi4_config = (p852_spi_peripherals >> P852_SPI4_SHIFT)
					& P852_SPI_MASK;
	unsigned int displayb_config = (p852_display_peripherals >> P852_DISPB_SHIFT)
					& P852_DISP_MASK;
	unsigned int spi3_config = (p852_spi_peripherals >> P852_SPI3_SHIFT)
					& P852_SPI_MASK;


	tegra_pinmux_config_table(p852_common_pinmux, ARRAY_SIZE(p852_common_pinmux));

	if ((uarta_config & P852_UART_ENABLE)
		&& (uarta_config & P852_UART_ALT_PIN_CFG)) {
		tegra_pinmux_config_table(p852_uarta_1_pinmux,
				ARRAY_SIZE(p852_uarta_1_pinmux));
	} else {
		tegra_pinmux_config_table(p852_ulpi_pinmux,
				ARRAY_SIZE(p852_ulpi_pinmux));
	}

	/* FIXME: same controller can use different pingroup. Condition
	   should be based on pingroup */
	if (sdio3_config & P852_SDHCI_ENABLE) {
		tegra_pinmux_config_table(p852_sdio3_pinmux,
				ARRAY_SIZE(p852_sdio3_pinmux));
	} else if ((spi3_config & P852_SPI_ENABLE) &&
			(IS_P852_SKU8 || IS_P852_SKU9 || IS_P852_SKU1)) {
		tegra_pinmux_config_table(p852_spi3_pinmux,
				ARRAY_SIZE(p852_spi3_pinmux));
	} else {
		tegra_pinmux_config_table(p852_uarta_pinmux,
				ARRAY_SIZE(p852_uarta_pinmux));
	}

	if ((uartd_config & P852_UART_ENABLE) && (spi4_config & P852_SPI_ENABLE)) {
		tegra_pinmux_config_table(p852_uartd_pinmux,
				ARRAY_SIZE(p852_uartd_pinmux));
		tegra_pinmux_config_table(p852_spi4_1_pinmux,
				ARRAY_SIZE(p852_spi4_1_pinmux));
	} else {
		if (IS_P852_SKU8 || IS_P852_SKU9 || IS_P852_SKU1) {
			tegra_pinmux_config_table(p852_uartd_pinmux,
					ARRAY_SIZE(p852_uartd_pinmux));
		} else {
			tegra_pinmux_config_table(p852_spi4_pinmux,
					ARRAY_SIZE(p852_spi4_pinmux));
		}
	}

	if (p852_sku_peripherals & P852_SKU_NOR_ENABLE) {
		tegra_pinmux_config_table(p852_nor_pinmux,
				ARRAY_SIZE(p852_nor_pinmux));
	} else {
		tegra_pinmux_config_table(p852_nand_pinmux,
				ARRAY_SIZE(p852_nand_pinmux));
	}

	if (p852_sku_peripherals & P852_SKU_DISPLAY_ENABLE) {
		if (displayb_config) {
			tegra_pinmux_config_table(p852_display_b_pinmux,
					ARRAY_SIZE(p852_display_b_pinmux));
		}
		else {
			tegra_pinmux_config_table(p852_display_a_pinmux,
					ARRAY_SIZE(p852_display_a_pinmux));
		}
	}

	tegra_drive_pinmux_config_table(p852_drive_pinmux,
			ARRAY_SIZE(p852_drive_pinmux));

	if(machine_is_p852_sku23() ||
		machine_is_p852_sku23_b00() ||
		machine_is_p852_sku23_c01())
	{
		tegra_drive_pinmux_config_table(p852_drive_pinmux_sku23,
				ARRAY_SIZE(p852_drive_pinmux_sku23));

		tegra_pinmux_config_pupd_table(p852_pupd_sku23, ARRAY_SIZE(p852_pupd_sku23));
	}
	else if(machine_is_p852_sku13() ||
			machine_is_p852_sku13_b00())
	{
		tegra_drive_pinmux_config_table(p852_drive_pinmux_sku13,
				ARRAY_SIZE(p852_drive_pinmux_sku13));

		tegra_pinmux_config_pupd_table(p852_pupd_sku13, ARRAY_SIZE(p852_pupd_sku13));
	}
	else if(machine_is_p852_sku5_b00() || machine_is_p852_sku5_c01())
	{
		tegra_pinmux_config_pupd_table(p852_pupd_sku5, ARRAY_SIZE(p852_pupd_sku5));
	}
	else if(machine_is_p852_sku8_b00() ||
			machine_is_p852_sku8_c01() ||
			machine_is_p852_sku9_b00() ||
			machine_is_p852_sku9_c01())
	{
		tegra_pinmux_config_table(p852_sku8_sku9_eth_pinmux,
				ARRAY_SIZE(p852_sku8_sku9_eth_pinmux));
		is_sku_rev = tegra_is_vcm_board_of_type(NULL, "60852",
						"0008", NULL);
		if (is_sku_rev && tegra_get_vcm_sku_rev() >= 602) {
			tegra_drive_pinmux_config_table(
				p852_drive_pinmux_sku8_rev602,
				ARRAY_SIZE(p852_drive_pinmux_sku8_rev602));
		} else {
			tegra_drive_pinmux_config_table(
				p852_drive_pinmux_sku8_sku9,
				ARRAY_SIZE(p852_drive_pinmux_sku8_sku9));
                }
                tegra_drive_pinmux_config_table(p852_drive_pinmux_sku9_lcd,
                                ARRAY_SIZE(p852_drive_pinmux_sku9_lcd));
	}

}

void tegra_pinmux_config_pupd_table(const struct tegra_pingroup_config *config, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		/* config.func, the pin_mux setting is not used here */
		tegra_pinmux_config_pullupdown_table(&config[i], 1, config[i].pupd);
	}
}

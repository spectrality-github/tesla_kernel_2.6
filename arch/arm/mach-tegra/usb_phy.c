/*
 * arch/arm/mach-tegra/usb_phy.c
 *
 * Copyright (C) 2010 Google, Inc.
 *
 * Author:
 *	Erik Gilling <konkers@google.com>
 *	Benoit Goby <benoit@android.com>
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

#include <linux/resource.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <asm/mach-types.h>
#include <mach/usb_phy.h>
#include <mach/iomap.h>
#include <mach/pinmux.h>
#include "gpio-names.h"
#include "fuse.h"

#define USB_USBSTS		0x144
#define   USB_USBSTS_PCI	(1 << 2)

#define ULPI_VIEWPORT		0x170
#define   ULPI_WAKEUP		(1 << 31)
#define   ULPI_RUN		(1 << 30)
#define   ULPI_RD_RW_WRITE	(1 << 29)
#define   ULPI_RD_RW_READ	(0 << 29)
#define   ULPI_PORT(x)		(((x) & 0x7) << 24)
#define   ULPI_ADDR(x)		(((x) & 0xff) << 16)
#define   ULPI_DATA_RD(x)	(((x) & 0xff) << 8)
#define   ULPI_DATA_WR(x)	(((x) & 0xff) << 0)

#define USB_PORTSC1		0x184
#define   USB_PORTSC1_PTS(x)	(((x) & 0x3) << 30)
#define   USB_PORTSC1_PSPD(x)	(((x) & 0x3) << 26)
#define   USB_PORTSC1_PHCD	(1 << 23)
#define   USB_PORTSC1_WKOC	(1 << 22)
#define   USB_PORTSC1_WKDS	(1 << 21)
#define   USB_PORTSC1_WKCN	(1 << 20)
#define   USB_PORTSC1_PTC(x)	(((x) & 0xf) << 16)
#define   USB_PORTSC1_PP	(1 << 12)
#define   USB_PORTSC1_SUSP	(1 << 7)
#define   USB_PORTSC1_PE	(1 << 2)
#define   USB_PORTSC1_CCS	(1 << 0)

#define USB_SUSP_CTRL		0x400
#define   USB_WAKE_ON_CNNT_EN_DEV	(1 << 3)
#define   USB_WAKE_ON_DISCON_EN_DEV	(1 << 4)
#define   USB_SUSP_CLR		(1 << 5)
#define   USB_PHY_CLK_VALID	(1 << 7)
#define   UTMIP_RESET			(1 << 11)
#define   UHSIC_RESET			(1 << 11)
#define   UTMIP_PHY_ENABLE		(1 << 12)
#define   ULPI_PHY_ENABLE	(1 << 13)
#define   USB_SUSP_SET		(1 << 14)
#define   USB_WAKEUP_DEBOUNCE_COUNT(x)	(((x) & 0x7) << 16)

#define USB1_LEGACY_CTRL	0x410
#define   USB1_NO_LEGACY_MODE			(1 << 0)
#define   USB1_VBUS_SENSE_CTL_MASK		(3 << 1)
#define   USB1_VBUS_SENSE_CTL_VBUS_WAKEUP	(0 << 1)
#define   USB1_VBUS_SENSE_CTL_AB_SESS_VLD_OR_VBUS_WAKEUP \
						(1 << 1)
#define   USB1_VBUS_SENSE_CTL_AB_SESS_VLD	(2 << 1)
#define   USB1_VBUS_SENSE_CTL_A_SESS_VLD	(3 << 1)

#define ULPI_TIMING_CTRL_0	0x424
#define   ULPI_OUTPUT_PINMUX_BYP	(1 << 10)
#define   ULPI_CLKOUT_PINMUX_BYP	(1 << 11)

#define ULPI_TIMING_CTRL_1	0x428
#define   ULPI_DATA_TRIMMER_LOAD	(1 << 0)
#define   ULPI_DATA_TRIMMER_SEL(x)	(((x) & 0x7) << 1)
#define   ULPI_STPDIRNXT_TRIMMER_LOAD	(1 << 16)
#define   ULPI_STPDIRNXT_TRIMMER_SEL(x)	(((x) & 0x7) << 17)
#define   ULPI_DIR_TRIMMER_LOAD		(1 << 24)
#define   ULPI_DIR_TRIMMER_SEL(x)	(((x) & 0x7) << 25)

#define UTMIP_PLL_CFG1		0x804
#define   UTMIP_XTAL_FREQ_COUNT(x)		(((x) & 0xfff) << 0)
#define   UTMIP_PLLU_ENABLE_DLY_COUNT(x)	(((x) & 0x1f) << 27)

#define UTMIP_XCVR_CFG0		0x808
#define   UTMIP_XCVR_SETUP(x)			(((x) & 0xf) << 0)
#define   UTMIP_XCVR_LSRSLEW(x)			(((x) & 0x3) << 8)
#define   UTMIP_XCVR_LSFSLEW(x)			(((x) & 0x3) << 10)
#define   UTMIP_FORCE_PD_POWERDOWN		(1 << 14)
#define   UTMIP_FORCE_PD2_POWERDOWN		(1 << 16)
#define   UTMIP_FORCE_PDZI_POWERDOWN		(1 << 18)
#define   UTMIP_XCVR_LSBIAS_SEL(x)		(((x) & 0x1) << 21)
#define   UTMIP_XCVR_HSSLEW_MSB(x)		(((x) & 0x7f) << 25)

#define UTMIP_BIAS_CFG0		0x80c
#define   UTMIP_OTGPD			(1 << 11)
#define   UTMIP_BIASPD			(1 << 10)
#define   UTMIP_SQUELCH_LEVEL_SETUP(x)	(((x) & 0x3) << 0)
#define   UTMIP_SQUELCH_LEVEL_MASK	0x3
#define   UTMIP_HS_DISCON_MSB_MASK	(1 << 24)
#define   UTMIP_HS_DISCON_MSB_SETUP(x)	(((x) & 0x1) << 24)

#define UTMIP_HSRX_CFG0		0x810
#define   UTMIP_ELASTIC_LIMIT(x)	(((x) & 0x1f) << 10)
#define   UTMIP_IDLE_WAIT(x)		(((x) & 0x1f) << 15)

#define UTMIP_HSRX_CFG1		0x814
#define   UTMIP_HS_SYNC_START_DLY(x)	(((x) & 0x1f) << 1)

#define UTMIP_TX_CFG0		0x820
#define   UTMIP_FS_PREABMLE_J		(1 << 19)
#define   UTMIP_HS_DISCON_DISABLE	(1 << 8)

#define UTMIP_MISC_CFG0		0x824
#define   UTMIP_SUSPEND_EXIT_ON_EDGE	(1 << 22)

#define UTMIP_MISC_CFG1		0x828
#define   UTMIP_PLL_ACTIVE_DLY_COUNT(x)	(((x) & 0x1f) << 18)
#define   UTMIP_PLLU_STABLE_COUNT(x)	(((x) & 0xfff) << 6)

#define UTMIP_DEBOUNCE_CFG0	0x82c
#define   UTMIP_BIAS_DEBOUNCE_A(x)	(((x) & 0xffff) << 0)

#define UTMIP_BAT_CHRG_CFG0	0x830
#define   UTMIP_PD_CHRG			(1 << 0)

#define UTMIP_SPARE_CFG0	0x834
#define   FUSE_SETUP_SEL		(1 << 3)

#define FUSE_USB_CALIB          0x1f0
#define   FUSE_CALIB_MSBS(x)		(((x) & 0x70) >> 4)

#define UTMIP_XCVR_CFG1		0x838
#define   UTMIP_FORCE_PDDISC_POWERDOWN	(1 << 0)
#define   UTMIP_FORCE_PDCHRP_POWERDOWN	(1 << 2)
#define   UTMIP_FORCE_PDDR_POWERDOWN	(1 << 4)
#define   UTMIP_XCVR_TERM_RANGE_ADJ(x)	(((x) & 0xf) << 18)

#define UTMIP_BIAS_CFG1		0x83c
#define   UTMIP_BIAS_PDTRK_COUNT(x)	(((x) & 0x1f) << 3)

static DEFINE_SPINLOCK(utmip_pad_lock);
static int utmip_pad_count;

static const int udc_freq_table[] = {
	12000000,
	13000000,
	19200000,
	26000000,
};

static const u8 udc_delay_table[][4] = {
	/* ENABLE_DLY, STABLE_CNT, ACTIVE_DLY, XTAL_FREQ_CNT */
	{0x02,         0x2F,       0x04,       0x76}, /* 12 MHz */
	{0x02,         0x33,       0x05,       0x7F}, /* 13 MHz */
	{0x03,         0x4B,       0x06,       0xBB}, /* 19.2 MHz */
	{0x04,         0x66,       0x09,       0xFE}, /* 26 Mhz */
};

static const u16 udc_debounce_table[] = {
	0x7530, /* 12 MHz */
	0x7EF4, /* 13 MHz */
	0xBB80, /* 19.2 MHz */
	0xFDE8, /* 26 MHz */
};

static struct tegra_utmip_config utmip_default[] = {
	[0] = {
		.hssync_start_delay = 9,
		.idle_wait_delay = 17,
		.elastic_limit = 16,
		.term_range_adj = 6,
		.xcvr_setup = 9,
		.xcvr_lsfslew = 1,
		.xcvr_lsrslew = 1,
		.squelch_level = 2,
		.hsdiscon_msb = 1,
	},
	[2] = {
		.hssync_start_delay = 9,
		.idle_wait_delay = 17,
		.elastic_limit = 16,
		.term_range_adj = 6,
		.xcvr_setup = 9,
		.xcvr_lsfslew = 2,
		.xcvr_lsrslew = 2,
		.squelch_level = 2,
		.hsdiscon_msb = 1,
	},
};

static struct tegra_ulpi_config ulpi_default[] = {
	[1] = {
		.reset_gpio = TEGRA_GPIO_PV1,
		.clk = "clk_dev2",
	},
};

static int utmip_pad_open(struct tegra_usb_phy *phy)
{
	phy->pad_clk = clk_get_sys("utmip-pad", NULL);
	if (IS_ERR(phy->pad_clk)) {
		pr_err("%s: can't get utmip pad clock\n", __func__);
		return -1;
	}

	if (phy->instance == 0) {
		phy->pad_regs = phy->regs;
	} else {
		phy->pad_regs = ioremap(TEGRA_USB_BASE, TEGRA_USB_SIZE);
		if (!phy->pad_regs) {
			pr_err("%s: can't remap usb registers\n", __func__);
			clk_put(phy->pad_clk);
			return -ENOMEM;
		}
	}
	return 0;
}

static void utmip_pad_close(struct tegra_usb_phy *phy)
{
	if (phy->instance != 0)
		iounmap(phy->pad_regs);
	clk_put(phy->pad_clk);
}

static void utmip_pad_power_on(struct tegra_usb_phy *phy)
{
	unsigned long val, flags;
	void __iomem *base = phy->pad_regs;
	struct tegra_utmip_config *config = phy->config;

	clk_enable(phy->pad_clk);

	spin_lock_irqsave(&utmip_pad_lock, flags);

	if (utmip_pad_count++ == 0) {
		val = readl(base + UTMIP_BIAS_CFG0);
		/* USB1/3 are sharing bias pad registers */
		val &= ~(UTMIP_OTGPD
				| UTMIP_BIASPD
				| UTMIP_SQUELCH_LEVEL_MASK
				| UTMIP_HS_DISCON_MSB_MASK);
		val |= UTMIP_SQUELCH_LEVEL_SETUP(config->squelch_level);
		val |= UTMIP_HS_DISCON_MSB_SETUP(config->hsdiscon_msb);
		writel(val, base + UTMIP_BIAS_CFG0);
	}

	spin_unlock_irqrestore(&utmip_pad_lock, flags);

	clk_disable(phy->pad_clk);
}

static void utmip_usb_phy_calibrate(struct tegra_usb_phy *phy)
{
	unsigned long val;
	unsigned int spare_bit_57;          /*indicates the fusing scheme used*/
	struct tegra_utmip_config *config = phy->config;

	spare_bit_57 = tegra_spare_fuse(57);
	if(spare_bit_57==1)
	{
		val = tegra_fuse_readl(FUSE_USB_CALIB);
		config->term_range_adj = FUSE_CALIB_MSBS(val);
	}
	else {
		config->term_range_adj = 6;
	}
}

static int utmip_pad_power_off(struct tegra_usb_phy *phy)
{
	unsigned long val, flags;
	void __iomem *base = phy->pad_regs;

	if (!utmip_pad_count) {
		pr_err("%s: utmip pad already powered off\n", __func__);
		return -1;
	}

	clk_enable(phy->pad_clk);

	spin_lock_irqsave(&utmip_pad_lock, flags);

	if (--utmip_pad_count == 0) {
		val = readl(base + UTMIP_BIAS_CFG0);
		val |= UTMIP_OTGPD | UTMIP_BIASPD;
		writel(val, base + UTMIP_BIAS_CFG0);
	}

	spin_unlock_irqrestore(&utmip_pad_lock, flags);

	clk_disable(phy->pad_clk);

	return 0;
}

static int utmi_wait_register(void __iomem *reg, u32 mask, u32 result)
{
	unsigned long timeout = 2000;
	do {
		if ((readl(reg) & mask) == result)
			return 0;
		udelay(1);
		timeout--;
	} while (timeout);
	return -1;
}

static void utmi_phy_clk_disable(struct tegra_usb_phy *phy)
{
	unsigned long val;
	void __iomem *base = phy->regs;

	if (phy->instance == 0) {
		val = readl(base + USB_SUSP_CTRL);
		val |= USB_SUSP_SET;
		writel(val, base + USB_SUSP_CTRL);

		udelay(10);

		val = readl(base + USB_SUSP_CTRL);
		val &= ~USB_SUSP_SET;
		writel(val, base + USB_SUSP_CTRL);
	}

	if (phy->instance == 2) {
		val = readl(base + USB_PORTSC1);
		val |= USB_PORTSC1_PHCD;
		writel(val, base + USB_PORTSC1);
	}

	if (utmi_wait_register(base + USB_SUSP_CTRL, USB_PHY_CLK_VALID, 0) < 0)
		pr_err("%s: timeout waiting for phy to stabilize\n", __func__);
}

static void utmi_phy_clk_enable(struct tegra_usb_phy *phy)
{
	unsigned long val;
	void __iomem *base = phy->regs;

	if (phy->instance == 0) {
		val = readl(base + USB_SUSP_CTRL);
		val |= USB_SUSP_CLR;
		writel(val, base + USB_SUSP_CTRL);

		udelay(10);

		val = readl(base + USB_SUSP_CTRL);
		val &= ~USB_SUSP_CLR;
		writel(val, base + USB_SUSP_CTRL);
	}

	if (phy->instance == 2) {
		val = readl(base + USB_PORTSC1);
		val &= ~USB_PORTSC1_PHCD;
		writel(val, base + USB_PORTSC1);
	}

	if (utmi_wait_register(base + USB_SUSP_CTRL, USB_PHY_CLK_VALID,
						     USB_PHY_CLK_VALID))
		pr_err("%s: timeout waiting for phy to stabilize\n", __func__);
}

static int utmi_phy_power_on(struct tegra_usb_phy *phy)
{
	unsigned long val;
	void __iomem *base = phy->regs;
	int gpio_status;
	struct tegra_utmip_config *config = phy->config;

	if ((phy->instance == 2) && (config->enable_vbus))
		config->enable_vbus(1);

	val = readl(base + USB_SUSP_CTRL);
	val |= UTMIP_RESET;
	writel(val, base + USB_SUSP_CTRL);

	if (phy->instance == 0) {
		val = readl(base + USB1_LEGACY_CTRL);
		val |= USB1_NO_LEGACY_MODE;
		writel(val, base + USB1_LEGACY_CTRL);
	}

	val = readl(base + UTMIP_TX_CFG0);
	val &= ~UTMIP_FS_PREABMLE_J;
	writel(val, base + UTMIP_TX_CFG0);

	val = readl(base + UTMIP_HSRX_CFG0);
	val &= ~(UTMIP_IDLE_WAIT(~0) | UTMIP_ELASTIC_LIMIT(~0));
	val |= UTMIP_IDLE_WAIT(config->idle_wait_delay);
	val |= UTMIP_ELASTIC_LIMIT(config->elastic_limit);
	writel(val, base + UTMIP_HSRX_CFG0);

	val = readl(base + UTMIP_HSRX_CFG1);
	val &= ~UTMIP_HS_SYNC_START_DLY(~0);
	val |= UTMIP_HS_SYNC_START_DLY(config->hssync_start_delay);
	writel(val, base + UTMIP_HSRX_CFG1);

	val = readl(base + UTMIP_DEBOUNCE_CFG0);
	val &= ~UTMIP_BIAS_DEBOUNCE_A(~0);
	val |= UTMIP_BIAS_DEBOUNCE_A(udc_debounce_table[phy->freq_sel]);
	writel(val, base + UTMIP_DEBOUNCE_CFG0);

	val = readl(base + UTMIP_MISC_CFG0);
	val &= ~UTMIP_SUSPEND_EXIT_ON_EDGE;
	writel(val, base + UTMIP_MISC_CFG0);

	val = readl(base + UTMIP_MISC_CFG1);
	val &= ~(UTMIP_PLL_ACTIVE_DLY_COUNT(~0) | UTMIP_PLLU_STABLE_COUNT(~0));
	val |= UTMIP_PLL_ACTIVE_DLY_COUNT(udc_delay_table[phy->freq_sel][2]) |
		UTMIP_PLLU_STABLE_COUNT(udc_delay_table[phy->freq_sel][1]);
	writel(val, base + UTMIP_MISC_CFG1);

	val = readl(base + UTMIP_PLL_CFG1);
	val &= ~(UTMIP_XTAL_FREQ_COUNT(~0) | UTMIP_PLLU_ENABLE_DLY_COUNT(~0));
	val |= UTMIP_XTAL_FREQ_COUNT(udc_delay_table[phy->freq_sel][3]) |
		UTMIP_PLLU_ENABLE_DLY_COUNT(udc_delay_table[phy->freq_sel][0]);
	writel(val, base + UTMIP_PLL_CFG1);

	if (phy->mode == TEGRA_USB_PHY_MODE_DEVICE) {
		val = readl(base + USB_SUSP_CTRL);
		val &= ~(USB_WAKE_ON_CNNT_EN_DEV | USB_WAKE_ON_DISCON_EN_DEV);
		writel(val, base + USB_SUSP_CTRL);
	}

	utmip_pad_power_on(phy);

	utmip_usb_phy_calibrate(phy);

	val = readl(base + UTMIP_XCVR_CFG0);
	val &= ~(UTMIP_FORCE_PD_POWERDOWN | UTMIP_FORCE_PD2_POWERDOWN |
		 UTMIP_FORCE_PDZI_POWERDOWN | UTMIP_XCVR_SETUP(~0) |
		 UTMIP_XCVR_LSFSLEW(~0) | UTMIP_XCVR_LSRSLEW(~0) |
		 UTMIP_XCVR_HSSLEW_MSB(~0));
	val |= UTMIP_XCVR_SETUP(config->xcvr_setup);
	val |= UTMIP_XCVR_LSFSLEW(config->xcvr_lsfslew);
	val |= UTMIP_XCVR_LSRSLEW(config->xcvr_lsrslew);
	val |= UTMIP_XCVR_LSBIAS_SEL(config->xcvr_lsbias_sel);
	writel(val, base + UTMIP_XCVR_CFG0);

	val = readl(base + UTMIP_XCVR_CFG1);
	val &= ~(UTMIP_FORCE_PDDISC_POWERDOWN | UTMIP_FORCE_PDCHRP_POWERDOWN |
		 UTMIP_FORCE_PDDR_POWERDOWN | UTMIP_XCVR_TERM_RANGE_ADJ(~0));
	val |= UTMIP_XCVR_TERM_RANGE_ADJ(config->term_range_adj);
	writel(val, base + UTMIP_XCVR_CFG1);

	val = readl(base + UTMIP_BAT_CHRG_CFG0);
	val &= ~UTMIP_PD_CHRG;
	writel(val, base + UTMIP_BAT_CHRG_CFG0);

	val = readl(base + UTMIP_BIAS_CFG1);
	val &= ~UTMIP_BIAS_PDTRK_COUNT(~0);
	val |= UTMIP_BIAS_PDTRK_COUNT(0x5);
	writel(val, base + UTMIP_BIAS_CFG1);

	if (phy->instance == 2) {
		val = readl(base + USB_SUSP_CTRL);
		val |= UTMIP_PHY_ENABLE;
		writel(val, base + USB_SUSP_CTRL);
	}

	val = readl(base + USB_SUSP_CTRL);
	val &= ~UTMIP_RESET;
	writel(val, base + USB_SUSP_CTRL);

	if (phy->instance == 0) {
		val = readl(base + USB1_LEGACY_CTRL);
		val &= ~USB1_VBUS_SENSE_CTL_MASK;
		val |= USB1_VBUS_SENSE_CTL_A_SESS_VLD;
		writel(val, base + USB1_LEGACY_CTRL);

		val = readl(base + USB_SUSP_CTRL);
		val &= ~USB_SUSP_SET;
		writel(val, base + USB_SUSP_CTRL);
		if (phy->mode == TEGRA_USB_PHY_MODE_HOST) {
			gpio_status = gpio_request(TEGRA_GPIO_PD0,"VBUS_BUS");
			if (gpio_status < 0) {
				printk("VBUS_USB1 request GPIO FAILED\n");
				WARN_ON(1);
			}
			tegra_gpio_enable(TEGRA_GPIO_PD0);
			gpio_status = gpio_direction_output(TEGRA_GPIO_PD0, 1);
			if (gpio_status < 0) {
				printk("VBUS_USB1 request GPIO DIRECTION FAILED \n");
				WARN_ON(1);
			}
			gpio_set_value(TEGRA_GPIO_PD0, 1);
			tegra_pinmux_set_tristate(TEGRA_PINGROUP_SLXK, TEGRA_TRI_NORMAL);
		}
	}

	utmi_phy_clk_enable(phy);

	if (phy->instance == 2) {
		val = readl(base + USB_PORTSC1);
		val &= ~USB_PORTSC1_PTS(~0);
		writel(val, base + USB_PORTSC1);
	}
	return 0;
}

static void utmi_phy_power_off(struct tegra_usb_phy *phy)
{
	unsigned long val;
	void __iomem *base = phy->regs;
	struct tegra_utmip_config *config = phy->config;

	utmi_phy_clk_disable(phy);

	if (phy->instance == 0 && phy->mode == TEGRA_USB_PHY_MODE_HOST) {
		gpio_free(TEGRA_GPIO_PD0);
		tegra_pinmux_set_tristate(TEGRA_PINGROUP_SLXK, TEGRA_TRI_TRISTATE);
	}

	if (phy->mode == TEGRA_USB_PHY_MODE_DEVICE) {
		val = readl(base + USB_SUSP_CTRL);
		val &= ~USB_WAKEUP_DEBOUNCE_COUNT(~0);
		val |= USB_WAKE_ON_CNNT_EN_DEV | USB_WAKEUP_DEBOUNCE_COUNT(5);
		writel(val, base + USB_SUSP_CTRL);
	}

	val = readl(base + USB_SUSP_CTRL);
	val |= UTMIP_RESET;
	writel(val, base + USB_SUSP_CTRL);

	val = readl(base + UTMIP_BAT_CHRG_CFG0);
	val |= UTMIP_PD_CHRG;
	writel(val, base + UTMIP_BAT_CHRG_CFG0);

	val = readl(base + UTMIP_XCVR_CFG0);
	val |= UTMIP_FORCE_PD_POWERDOWN | UTMIP_FORCE_PD2_POWERDOWN |
	       UTMIP_FORCE_PDZI_POWERDOWN;
	writel(val, base + UTMIP_XCVR_CFG0);

	val = readl(base + UTMIP_XCVR_CFG1);
	val |= UTMIP_FORCE_PDDISC_POWERDOWN | UTMIP_FORCE_PDCHRP_POWERDOWN |
	       UTMIP_FORCE_PDDR_POWERDOWN;
	writel(val, base + UTMIP_XCVR_CFG1);

	if ((phy->instance == 2) && (config->enable_vbus))
		config->enable_vbus(0);

	utmip_pad_power_off(phy);
}

static void utmi_phy_preresume(struct tegra_usb_phy *phy)
{
	unsigned long val;
	void __iomem *base = phy->regs;

	val = readl(base + UTMIP_TX_CFG0);
	val |= UTMIP_HS_DISCON_DISABLE;
	writel(val, base + UTMIP_TX_CFG0);
}

static void utmi_phy_postresume(struct tegra_usb_phy *phy)
{
	unsigned long val;
	void __iomem *base = phy->regs;

	val = readl(base + UTMIP_TX_CFG0);
	val &= ~UTMIP_HS_DISCON_DISABLE;
	writel(val, base + UTMIP_TX_CFG0);
}

static void ulpi_viewport_write(struct tegra_usb_phy *phy, u8 addr, u8 data)
{
	unsigned long val;
	void __iomem *base = phy->regs;

	val = ULPI_RUN | ULPI_RD_RW_WRITE | ULPI_PORT(0);
	val |= ULPI_ADDR(addr) | ULPI_DATA_WR(data);
	writel(val, base + ULPI_VIEWPORT);

	if (utmi_wait_register(base + ULPI_VIEWPORT, ULPI_RUN, 0))
		pr_err("%s: timeout accessing ulpi phy\n", __func__);
}

static int ulpi_phy_power_on(struct tegra_usb_phy *phy)
{
	unsigned long val;
	void __iomem *base = phy->regs;
	struct tegra_ulpi_config *config = phy->config;

	gpio_direction_output(config->reset_gpio, 0);
	msleep(5);
	gpio_direction_output(config->reset_gpio, 1);

        if(phy->clk) {
		clk_enable(phy->clk);
	}

	msleep(1);

	val = readl(base + USB_SUSP_CTRL);
	val |= UHSIC_RESET;
	writel(val, base + USB_SUSP_CTRL);

	val = readl(base + ULPI_TIMING_CTRL_0);
	val |= ULPI_OUTPUT_PINMUX_BYP | ULPI_CLKOUT_PINMUX_BYP;
	writel(val, base + ULPI_TIMING_CTRL_0);

	val = readl(base + USB_SUSP_CTRL);
	val |= ULPI_PHY_ENABLE;
	writel(val, base + USB_SUSP_CTRL);

	val = 0;
	writel(val, base + ULPI_TIMING_CTRL_1);

	val |= ULPI_DATA_TRIMMER_SEL(4);
	val |= ULPI_STPDIRNXT_TRIMMER_SEL(4);
	val |= ULPI_DIR_TRIMMER_SEL(4);
	writel(val, base + ULPI_TIMING_CTRL_1);
	udelay(10);

	val |= ULPI_DATA_TRIMMER_LOAD;
	val |= ULPI_STPDIRNXT_TRIMMER_LOAD;
	val |= ULPI_DIR_TRIMMER_LOAD;
	writel(val, base + ULPI_TIMING_CTRL_1);

	val = ULPI_WAKEUP | ULPI_RD_RW_WRITE | ULPI_PORT(0);
	writel(val, base + ULPI_VIEWPORT);

	if (utmi_wait_register(base + ULPI_VIEWPORT, ULPI_WAKEUP, 0)) {
		pr_err("%s: timeout waiting for ulpi phy wakeup\n", __func__);
		return -ETIMEDOUT;
	}

	/* Fix VbusInvalid due to floating VBUS */
	ulpi_viewport_write(phy, 0x08, 0x40);
	ulpi_viewport_write(phy, 0x0B, 0x80);

	val = readl(base + USB_PORTSC1);
	val |= USB_PORTSC1_WKOC | USB_PORTSC1_WKDS | USB_PORTSC1_WKCN;
	writel(val, base + USB_PORTSC1);

	val = readl(base + USB_SUSP_CTRL);
	val |= USB_SUSP_CLR;
	writel(val, base + USB_SUSP_CTRL);
	udelay(100);

	val = readl(base + USB_SUSP_CTRL);
	val &= ~USB_SUSP_CLR;
	writel(val, base + USB_SUSP_CTRL);

	return 0;
}

static void ulpi_phy_power_off(struct tegra_usb_phy *phy)
{
	unsigned long val;
	void __iomem *base = phy->regs;
	struct tegra_ulpi_config *config = phy->config;

	/* Programming the ULPI register function control */
	ulpi_viewport_write(phy, 0x04, 0x4D);

	/* Resetting the ULPI register IndicatorPassThru */
	ulpi_viewport_write(phy, 0x09, 0x40);

	/* USB Interrupt Rising - making sure vbus comparator and id are off */
	ulpi_viewport_write(phy, 0x0D, 0x00);

	/* USB Interrupt Falling */
	ulpi_viewport_write(phy, 0x10, 0x00);

	/* Carkit Control */
	ulpi_viewport_write(phy, 0x19, 0x00);

	/* Disabling ID float Rise/Fall (Carkit Enable) */
	ulpi_viewport_write(phy, 0x1D, 0x00);

	/* USB I/O and power */
	ulpi_viewport_write(phy, 0x39, 0x00);

	/* Clear WKCN/WKDS/WKOC wake-on events that can cause the USB
	 * Controller to immediately bring the ULPI PHY out of low power
	 */
	val = readl(base + USB_PORTSC1);
	val &= ~(USB_PORTSC1_WKOC | USB_PORTSC1_WKDS | USB_PORTSC1_WKCN);
	writel(val, base + USB_PORTSC1);

	gpio_direction_output(config->reset_gpio, 0);

	if (phy->clk) {
		clk_disable(phy->clk);
	}
}

struct tegra_usb_phy *tegra_usb_phy_open(int instance, void __iomem *regs,
			void *config, enum tegra_usb_phy_mode phy_mode)
{
	struct tegra_usb_phy *phy;
	struct tegra_ulpi_config *ulpi_config;
	unsigned long parent_rate;
	int freq_sel;
	int err=0;

	phy = kmalloc(sizeof(struct tegra_usb_phy), GFP_KERNEL);
	if (!phy)
		return ERR_PTR(-ENOMEM);

	phy->instance = instance;
	phy->regs = regs;
	phy->config = config;
	phy->context.valid = false;
	phy->mode = phy_mode;

	if (!phy->config) {
		if (instance == 1) {
			pr_info("%s: ulpi phy configuration missing - "
					"using default", __func__);
			phy->config = config = &ulpi_default[instance];
		} else {
			phy->config = &utmip_default[instance];
		}
	}

	phy->pll_u = clk_get_sys(NULL, "pll_u");
	if (IS_ERR(phy->pll_u)) {
		pr_err("Can't get pll_u clock\n");
		err = PTR_ERR(phy->pll_u);
		goto err0;
	}
	clk_enable(phy->pll_u);

	parent_rate = clk_get_rate(clk_get_parent(phy->pll_u));
	for (freq_sel = 0; freq_sel < ARRAY_SIZE(udc_freq_table); freq_sel++) {
		if (udc_freq_table[freq_sel] == parent_rate)
			break;
	}
	if (freq_sel == ARRAY_SIZE(udc_freq_table)) {
		pr_err("invalid pll_u parent rate %ld\n", parent_rate);
		err = -EINVAL;
		goto err1;
	}
	phy->freq_sel = freq_sel;

	if (phy->instance == 1) {
		ulpi_config = config;
		if(ulpi_config->clk) {
			phy->clk = clk_get_sys(NULL, ulpi_config->clk);
			if (IS_ERR(phy->clk)) {
				pr_err("%s: can't get ulpi clock\n", __func__);
				err = -ENXIO;
				goto err1;
			}
		} else {
			/* Some USB ULPI chips are not driven by Tegra clocks or PLL */
			phy->clk = NULL;
		}
		tegra_gpio_enable(ulpi_config->reset_gpio);
		gpio_request(ulpi_config->reset_gpio, "ulpi_phy_reset_b");
		gpio_direction_output(ulpi_config->reset_gpio, 0);
	} else {
		err = utmip_pad_open(phy);
		if (err < 0)
			goto err1;
	}

	return phy;

err1:
	clk_disable(phy->pll_u);
	clk_put(phy->pll_u);
err0:
	kfree(phy);
	return ERR_PTR(err);
}

int tegra_usb_phy_power_on(struct tegra_usb_phy *phy)
{
	int err;

	if (phy->instance == 1)
		err = ulpi_phy_power_on(phy);
	else
		err = utmi_phy_power_on(phy);

	return err;
}

int tegra_usb_phy_power_off(struct tegra_usb_phy *phy)
{
	if (phy->instance == 1)
		ulpi_phy_power_off(phy);
	else
		utmi_phy_power_off(phy);

	return 0;
}

int tegra_usb_phy_preresume(struct tegra_usb_phy *phy)
{
	if (phy->instance != 1)
		utmi_phy_preresume(phy);
	return 0;
}

int tegra_usb_phy_postresume(struct tegra_usb_phy *phy)
{
	if (phy->instance != 1)
		utmi_phy_postresume(phy);
	return 0;
}

int tegra_usb_phy_clk_disable(struct tegra_usb_phy *phy)
{
	if (phy->instance != 1)
		utmi_phy_clk_disable(phy);

	return 0;
}

int tegra_usb_phy_clk_enable(struct tegra_usb_phy *phy)
{
	if (phy->instance != 1)
		utmi_phy_clk_enable(phy);

	return 0;
}

int tegra_usb_phy_close(struct tegra_usb_phy *phy)
{
	if (phy->instance == 1)
		clk_put(phy->clk);
	else
		utmip_pad_close(phy);
	clk_disable(phy->pll_u);
	clk_put(phy->pll_u);
	kfree(phy);
	return 0;
}

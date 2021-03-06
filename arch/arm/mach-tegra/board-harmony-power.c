/*
 * Copyright (C) 2010 NVIDIA, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA
 */
#include <linux/i2c.h>
#include <linux/pda_power.h>
#include <linux/platform_device.h>
#include <linux/resource.h>
#include <linux/regulator/machine.h>
#include <linux/mfd/tps6586x.h>
#include <linux/gpio.h>
#include <mach/suspend.h>
#include <linux/io.h>

#include <mach/iomap.h>
#include <mach/irqs.h>

#include "gpio-names.h"
#include "power.h"
#include "wakeups-t2.h"
#include "board.h"

#define PMC_CTRL		0x0
#define PMC_CTRL_INTR_LOW	(1 << 17)

static int ac_ok = TEGRA_GPIO_PV3;
static int charge_disable = TEGRA_GPIO_PR6;

static int charge_init(struct device *dev)
{
	int ret = gpio_request(charge_disable, "chg_disable");
	if (ret < 0)
		return ret;

	ret = gpio_request(ac_ok, "ac_ok");
	if (ret < 0) {
		gpio_free(charge_disable);
		return ret;
	}

	ret = gpio_direction_output(charge_disable, 0);
	if (ret < 0)
		goto cleanup;

	ret = gpio_direction_input(ac_ok);
	if (ret < 0)
		goto cleanup;

	tegra_gpio_enable(ac_ok);
	tegra_gpio_enable(charge_disable);

	return 0;

cleanup:
	gpio_free(ac_ok);
	gpio_free(charge_disable);
	return ret;
}

static void charge_exit(struct device *dev)
{
	gpio_free(charge_disable);
}

static int ac_online(void)
{
	return !gpio_get_value(ac_ok);
}

static void set_charge(int flags)
{
	if (flags == PDA_POWER_CHARGE_AC)
		gpio_set_value(charge_disable, 0);
	else if (!flags)
		gpio_set_value(charge_disable, 1);
	/* USB charging not supported on harmony */
}

static struct resource harmony_pda_resources[] = {
	[0] = {
	       .name = "ac",
	       .start = TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PV3),
	       .end = TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PV3),
	       .flags = (IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHEDGE |
			 IORESOURCE_IRQ_LOWEDGE),
	       },
};

static struct pda_power_pdata harmony_pda_data = {
	.is_ac_online = ac_online,
	.exit = charge_exit,
	.init = charge_init,
	.set_charge = set_charge,
};

static struct platform_device harmony_pda_power_device = {
	.name = "pda-power",
	.id = -1,
	.resource = harmony_pda_resources,
	.num_resources = ARRAY_SIZE(harmony_pda_resources),
	.dev = {
		.platform_data = &harmony_pda_data,
		},
};

static struct regulator_consumer_supply tps658621_sm0_supply[] = {
	REGULATOR_SUPPLY("vdd_core", NULL),
};

static struct regulator_consumer_supply tps658621_sm1_supply[] = {
	REGULATOR_SUPPLY("vdd_cpu", NULL),
};

static struct regulator_consumer_supply tps658621_sm2_supply[] = {
	REGULATOR_SUPPLY("vdd_sm2", NULL),
};

static struct regulator_consumer_supply tps658621_ldo0_supply[] = {
	REGULATOR_SUPPLY("p_cam_avdd", NULL),
	REGULATOR_SUPPLY("hvdd_pex", NULL),
};

static struct regulator_consumer_supply tps658621_ldo1_supply[] = {
	REGULATOR_SUPPLY("avdd_pll", NULL),
};

static struct regulator_consumer_supply tps658621_ldo2_supply[] = {
	REGULATOR_SUPPLY("vdd_rtc", NULL),
};

static struct regulator_consumer_supply tps658621_ldo3_supply[] = {
	REGULATOR_SUPPLY("avdd_usb", NULL),
	REGULATOR_SUPPLY("avdd_usb_pll", NULL),
	REGULATOR_SUPPLY("avdd_lvds", NULL),
};

static struct regulator_consumer_supply tps658621_ldo4_supply[] = {
	REGULATOR_SUPPLY("avdd_osc", NULL),
	REGULATOR_SUPPLY("vddio_sys", "panjit_touch"),
};

static struct regulator_consumer_supply tps658621_ldo5_supply[] = {
	REGULATOR_SUPPLY("vcore_mmc", "sdhci-tegra.1"),
	REGULATOR_SUPPLY("vcore_mmc", "sdhci-tegra.3"),
};

static struct regulator_consumer_supply tps658621_ldo6_supply[] = {
	REGULATOR_SUPPLY("avdd_vdac", NULL),
};

static struct regulator_consumer_supply tps658621_ldo7_supply[] = {
	REGULATOR_SUPPLY("avdd_hdmi", NULL),
	REGULATOR_SUPPLY("vdd_fuse", NULL),
};

static struct regulator_consumer_supply tps658621_ldo8_supply[] = {
	REGULATOR_SUPPLY("avdd_hdmi_pll", NULL),
};

static struct regulator_consumer_supply tps658621_ldo9_supply[] = {
	REGULATOR_SUPPLY("avdd_2v85", NULL),
	REGULATOR_SUPPLY("vdd_ddr_rx", NULL),
	REGULATOR_SUPPLY("avdd_amp", NULL),
};

#define REGULATOR_INIT(_id, _minmv, _maxmv)				\
	{								\
		.constraints = {					\
			.min_uV = (_minmv)*1000,			\
			.max_uV = (_maxmv)*1000,			\
			.valid_modes_mask = (REGULATOR_MODE_NORMAL |	\
					     REGULATOR_MODE_STANDBY),	\
			.valid_ops_mask = (REGULATOR_CHANGE_MODE |	\
					   REGULATOR_CHANGE_STATUS |	\
					   REGULATOR_CHANGE_VOLTAGE),	\
		},							\
		.num_consumer_supplies = ARRAY_SIZE(tps658621_##_id##_supply),\
		.consumer_supplies = tps658621_##_id##_supply,		\
	}

static struct regulator_init_data sm0_data = REGULATOR_INIT(sm0, 725, 1500);
static struct regulator_init_data sm1_data = REGULATOR_INIT(sm1, 725, 1500);
static struct regulator_init_data sm2_data = REGULATOR_INIT(sm2, 3000, 4550);
static struct regulator_init_data ldo0_data = REGULATOR_INIT(ldo0, 1250, 3300);
static struct regulator_init_data ldo1_data = REGULATOR_INIT(ldo1, 725, 1500);
static struct regulator_init_data ldo2_data = REGULATOR_INIT(ldo2, 725, 1500);
static struct regulator_init_data ldo3_data = REGULATOR_INIT(ldo3, 1250, 3300);
static struct regulator_init_data ldo4_data = REGULATOR_INIT(ldo4, 1700, 2475);
static struct regulator_init_data ldo5_data = REGULATOR_INIT(ldo5, 1250, 3300);
static struct regulator_init_data ldo6_data = REGULATOR_INIT(ldo6, 1250, 3300);
static struct regulator_init_data ldo7_data = REGULATOR_INIT(ldo7, 1250, 3300);
static struct regulator_init_data ldo8_data = REGULATOR_INIT(ldo8, 1250, 3300);
static struct regulator_init_data ldo9_data = REGULATOR_INIT(ldo9, 1250, 3300);

static struct tps6586x_rtc_platform_data rtc_data = {
	.irq = TEGRA_NR_IRQS + TPS6586X_INT_RTC_ALM1,
};

#define TPS_REG(_id, _data)			\
	{					\
		.id = TPS6586X_ID_##_id,	\
		.name = "tps6586x-regulator",	\
		.platform_data = _data,		\
	}

static struct tps6586x_subdev_info tps_devs[] = {
	TPS_REG(SM_0, &sm0_data),
	TPS_REG(SM_1, &sm1_data),
	TPS_REG(SM_2, &sm2_data),
	TPS_REG(LDO_0, &ldo0_data),
	TPS_REG(LDO_1, &ldo1_data),
	TPS_REG(LDO_2, &ldo2_data),
	TPS_REG(LDO_3, &ldo3_data),
	TPS_REG(LDO_4, &ldo4_data),
	TPS_REG(LDO_5, &ldo5_data),
	TPS_REG(LDO_6, &ldo6_data),
	TPS_REG(LDO_7, &ldo7_data),
	TPS_REG(LDO_8, &ldo8_data),
	TPS_REG(LDO_9, &ldo9_data),
	{
	 .id = 0,
	 .name = "tps6586x-rtc",
	 .platform_data = &rtc_data,
	 },
};

static struct tps6586x_platform_data tps_platform = {
	.irq_base = TEGRA_NR_IRQS,
	.num_subdevs = ARRAY_SIZE(tps_devs),
	.subdevs = tps_devs,
	.gpio_base = TEGRA_NR_GPIOS,
};

static struct i2c_board_info __initdata harmony_regulators[] = {
	{
	 I2C_BOARD_INFO("tps6586x", 0x34),
	 .irq = INT_EXTERNAL_PMU,
	 .platform_data = &tps_platform,
	 },
};

static struct tegra_suspend_platform_data harmony_suspend_data = {
	.cpu_timer = 2000,
	.cpu_off_timer = 0,
	.suspend_mode = TEGRA_SUSPEND_LP1,
	.core_timer = 0x7e7e,
	.core_off_timer = 0,
	.separate_req = true,
	.corereq_high = false,
	.sysclkreq_high = true,
	.wake_enb = TEGRA_WAKE_GPIO_PV2,
	.wake_high = 0,
	.wake_low = TEGRA_WAKE_GPIO_PV2,
	.wake_any = 0,
};

static void harmony_power_off(void)
{
	int ret;

	ret = tps6586x_power_off();
	if (ret)
		pr_err("harmony: failed to power off\n");

	while(1);
}

void __init harmony_power_off_init(void)
{
	pm_power_off = harmony_power_off;
}

int __init harmony_regulator_init(void)
{
	void __iomem *pmc = IO_ADDRESS(TEGRA_PMC_BASE);
	u32 pmc_ctrl;

	/* configure the power management controller to trigger PMU
	 * interrupts when low */
	pmc_ctrl = readl(pmc + PMC_CTRL);
	writel(pmc_ctrl | PMC_CTRL_INTR_LOW, pmc + PMC_CTRL);
	platform_device_register(&harmony_pda_power_device);
	i2c_register_board_info(4, harmony_regulators, 1);
	tegra_init_suspend(&harmony_suspend_data);
	return 0;
}

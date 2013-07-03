/*
 * (C) Copyright 2012-2013 Henrik Nordstrom <henrik@henriknordstrom.net>
 * (C) Copyright 2013 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some board init for the Allwinner A10-evb board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/dram.h>
#include <asm/arch/clock.h>
#include <asm/gpio.h>
#include <asm/arch/mmc.h>
#ifdef CONFIG_AXP152_POWER
#include <axp152.h>
#endif
#ifdef CONFIG_AXP209_POWER
#include <axp209.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

/* add board specific code here */
int board_init(void)
{
	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100);

#ifdef CONFIG_STATUS_LED
	status_led_set(STATUS_LED_BOOT, STATUS_LED_ON);
#endif
	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	printf("Board: %s\n", CONFIG_SYS_BOARD_NAME);

	return 0;
}
#endif

int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, 1 << 30);

	return 0;
}

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	sunxi_mmc_init(CONFIG_MMC_SUNXI_SLOT);

	return 0;
}
#endif

#if defined(CONFIG_NAND_SUNXI) && defined(CONFIG_NAND)
void sunxi_nand_gpio_init(void)
{
	/* NRB1 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(7), 2);
	/* NRB0 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(6), 2);
	/* NRE# */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(5), 2);
	/* NCE0 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(4), 2);
	/* NCE1 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(3), 2);
	/* NCLE */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(2), 2);
	/* NALE */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(1), 2);
	/* NWE */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(0), 2);
	/* NDQ7 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(15), 2);
	/* NDQ6 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(14), 2);
	/* NDQ5 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(13), 2);
	/* NDQ4 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(12), 2);
	/* NDQ3 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(11), 2);
	/* NDQ2 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(10), 2);
	/* NDQ1 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(9), 2);
	/* NDQ0 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(8), 2);
	/* NCE7 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(22), 2);
	/* NCE7 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(22), 2);
	/* NCE6 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(21), 2);
	/* NCE5 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(20), 2);
	/* NCE4 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(19), 2);
	/* NCE3 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(18), 2);
	/* NCE2 */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(17), 2);
	/* NWP */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(16), 2);
}
#endif

#ifdef CONFIG_SPL_BUILD
void sunxi_board_init(void)
{
	int power_failed = 0;
	int ramsize;

	printf("DRAM:");
	ramsize = sunxi_dram_init();
	if (!ramsize) {
		printf(" ?");
		ramsize = sunxi_dram_init();
	}
	if (!ramsize) {
		printf(" ?");
		ramsize = sunxi_dram_init();
	}
	printf(" %dMB\n", ramsize>>20);
	if (!ramsize)
		hang();

#ifdef CONFIG_AXP152_POWER
	power_failed = axp152_init();
	power_failed |= axp152_set_dcdc2(1400);
	power_failed |= axp152_set_dcdc3(1500);
	power_failed |= axp152_set_dcdc4(1250);
	power_failed |= axp152_set_ldo2(3000);
#endif
#ifdef CONFIG_AXP209_POWER
	power_failed |= axp209_init();
	power_failed |= axp209_set_dcdc2(1400);
	power_failed |= axp209_set_dcdc3(1250);
	power_failed |= axp209_set_ldo2(3000);
	power_failed |= axp209_set_ldo3(2800);
	power_failed |= axp209_set_ldo4(2800);
#endif

	/*
	 * Only clock up the CPU to full speed if we are reasonably
	 * assured it's being powered with suitable core voltage
	 */
	if (!power_failed)
#ifdef CONFIG_SUN7I
		clock_set_pll1(912000000);
#else
		clock_set_pll1(1008000000);
#endif
	else
		printf("Failed to set core voltage!. Can't set CPU frequency\n");

#if defined(CONFIG_NAND_SUNXI) && defined(CONFIG_NAND)
	sunxi_nand_gpio_init();
#endif

#if defined(CONFIG_CMD_NAND)
	printf("NAND:");
	nand_init();
#endif
}

#ifdef CONFIG_SPL_DISPLAY_PRINT
void spl_display_print(void)
{
	printf("Board: %s\n", CONFIG_SYS_BOARD_NAME);
}
#endif

#endif

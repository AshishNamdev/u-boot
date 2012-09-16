/*
 * (C) Copyright 2012 MDSoft Ltd, Mike Dyer <mike.dyer@md-soft.co.uk>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */
#include <common.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc.h>

static struct s5pc110_gpio *s5pc110_gpio;

static void dm9000_pre_init(void)
{
	/* DM9000 is on SROM bank #1 */
	unsigned long tmp;

	s5p_gpio_cfg_pin(&s5pc110_gpio->mp0_1, 1, 0x2);

	tmp = readl(S5PC110_SROMC_BASE);
	tmp = (tmp & ~0xF0) | 0x10;
	writel(tmp, S5PC110_SROMC_BASE);
	writel(((0<<28) | (0<<24) | (5<<16) | (0<<12) | (0<<8) | (0<<4) | (0<<0)), S5PC110_SROMC_BASE + 8);
}

void motherboard_init(void) {

	s5pc110_gpio = (struct s5pc110_gpio *)S5PC110_GPIO_BASE;

#ifdef CONFIG_DRIVER_DM9000
	dm9000_pre_init();
#endif

#ifdef CONFIG_DRIVER_DM9000
int board_eth_init(bd_t *bis)
{
	return dm9000_initialize(bis);
}
#endif

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	int i;
	for (i = 0; i < 7; i++) {
		/* GPG0[0:6] special function 2 */
		s5p_gpio_cfg_pin(&s5pc110_gpio->g0, i, 0x2);
		/* GPG0[0:6] pull disable */
		s5p_gpio_set_pull(&s5pc110_gpio->g0, i, GPIO_PULL_NONE);
		/* GPG0[0:6] drv 4x */
		s5p_gpio_set_drv(&s5pc110_gpio->g0, i, GPIO_DRV_4X);
	}

	return (s5p_mmc_init(0, 4));
}
#endif

}

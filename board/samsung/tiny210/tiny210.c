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

DECLARE_GLOBAL_DATA_PTR;


static struct s5pc110_gpio *s5pc110_gpio;

static inline void delay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n" "bne 1b":"=r" (loops):"0"(loops));
}

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

int board_init(void)
{
	/* Set Initial global variables */
	s5pc110_gpio = (struct s5pc110_gpio *)S5PC110_GPIO_BASE;

	gd->bd->bi_arch_number = CONFIG_MACH_TYPE;
	gd->bd->bi_boot_params = (PHYS_SDRAM_1+0x100);

#ifdef CONFIG_DRIVER_DM9000
	dm9000_pre_init();
#endif
	return 0;
}

#ifdef BOARD_LATE_INIT
int board_late_init (void)
{
	return 0;
}
#endif

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	printf("\nBoard:   %s\n", CONFIG_BOARD_NAME);
	return (0);
}
#endif

int dram_init(void)
{
        gd->ram_size    = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	return 0;
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
}

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


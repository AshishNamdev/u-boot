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
#include <netdev.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc.h>

static struct s5pc110_gpio *s5pc110_gpio;

/*
 * Accessor functions replacing the "weak" functions in
 * drivers/net/smc911x.c
 */
u32 smc911x_reg_read(struct eth_device *dev, u32 offset)
{
	volatile u16 *addr_16;

	//printf("%s: 0x%08x\n", __func__, offset);

	s5p_gpio_set_value(&s5pc110_gpio->g2, 6, 0);
	s5p_gpio_set_value(&s5pc110_gpio->g2, 0, (offset >> 4) & 0x1); /* ADDR4 */
	s5p_gpio_set_value(&s5pc110_gpio->g2, 1, (offset >> 5) & 0x1); /* ADDR5 */
	s5p_gpio_set_value(&s5pc110_gpio->g2, 2, (offset >> 6) & 0x1); /* ADDR6 */
	s5p_gpio_set_value(&s5pc110_gpio->g2, 3, (offset >> 7) & 0x1); /* ADDR7 */
	s5p_gpio_set_value(&s5pc110_gpio->g2, 4, (offset >> 8) & 0x1); /* ADDR8 */
	s5p_gpio_set_value(&s5pc110_gpio->g2, 5, (offset >> 9) & 0x1); /* ADDR9 */

	addr_16 = (u16 *)(dev->iobase + offset);
	return ((*addr_16 & 0x0000ffff) | (*(addr_16 + 1) << 16));
}

void smc911x_reg_write(struct eth_device *dev, u32 offset, u32 val)
{
	//printf("%s: 0x%08x\n", __func__, offset);
	s5p_gpio_set_value(&s5pc110_gpio->g2, 6, 0);
	s5p_gpio_set_value(&s5pc110_gpio->g2, 0, (offset >> 4) & 0x1); /* ADDR4 */
	s5p_gpio_set_value(&s5pc110_gpio->g2, 1, (offset >> 5) & 0x1); /* ADDR5 */
	s5p_gpio_set_value(&s5pc110_gpio->g2, 2, (offset >> 6) & 0x1); /* ADDR6 */
	s5p_gpio_set_value(&s5pc110_gpio->g2, 3, (offset >> 7) & 0x1); /* ADDR7 */
	s5p_gpio_set_value(&s5pc110_gpio->g2, 4, (offset >> 8) & 0x1); /* ADDR8 */
	s5p_gpio_set_value(&s5pc110_gpio->g2, 5, (offset >> 9) & 0x1); /* ADDR9 */

	*(volatile u16 *)(dev->iobase + offset) = (u16)val;
	*(volatile u16 *)(dev->iobase + offset + 2) = (u16)(val >> 16);
}

void pkt_data_push(struct eth_device *dev, u32 offset, u32 val)
{
	//printf("%s: 0x%08x\n", __func__, offset);
	s5p_gpio_set_value(&s5pc110_gpio->g2, 6, 1);
	*(volatile u16 *)(dev->iobase + offset) = (u16)val;
	*(volatile u16 *)(dev->iobase + offset + 2) = (u16)(val >> 16);

	return;
}

u32 pkt_data_pull(struct eth_device *dev, u32 offset)
{
	volatile u16 *addr_16;

	//printf("%s: 0x%08x\n", __func__, offset);
	s5p_gpio_set_value(&s5pc110_gpio->g2, 6, 1);
	addr_16 = (u16 *)(dev->iobase + offset);
	return ((*addr_16 & 0x0000ffff) | (*(addr_16 + 1) << 16));
}

int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_SMC911X
	rc = smc911x_initialize(0, CONFIG_SMC911X_BASE);
#endif
	return rc;
}

void lan9311_preinit(void) 
{
	unsigned long tmp;
	/* Set up GPIOs for Ethernet */
	s5p_gpio_cfg_pin(&s5pc110_gpio->g2, 0, GPIO_OUTPUT); /* ADDR4 */
	s5p_gpio_cfg_pin(&s5pc110_gpio->g2, 1, GPIO_OUTPUT); /* ADDR5 */
	s5p_gpio_cfg_pin(&s5pc110_gpio->g2, 2, GPIO_OUTPUT); /* ADDR6 */
	s5p_gpio_cfg_pin(&s5pc110_gpio->g2, 3, GPIO_OUTPUT); /* ADDR7 */
	s5p_gpio_cfg_pin(&s5pc110_gpio->g2, 4, GPIO_OUTPUT); /* ADDR8 */
	s5p_gpio_cfg_pin(&s5pc110_gpio->g2, 5, GPIO_OUTPUT); /* ADDR9 */
	s5p_gpio_cfg_pin(&s5pc110_gpio->g2, 6, GPIO_OUTPUT); /* FIFO_SEL */

	s5p_gpio_cfg_pin(&s5pc110_gpio->mp0_1, 1, 0x2);	     /* CS1 */


	s5p_gpio_cfg_pin(&s5pc110_gpio->mp0_1, 1, 0x2);

	tmp = readl(S5PC110_SROMC_BASE);
	tmp = (tmp & ~0xF0) | 0x10;
	writel(tmp, S5PC110_SROMC_BASE);
	writel((  (0 << 28) /* Tacs */
		| (0 << 24) /* Tcos */
		| (4 << 16) /* Tacc */
		| (0 << 12) /* Tcoh */
		| (2 << 8)  /* Tcah */
		| (4 << 4)  /* Tacp */
		| (0 << 0)), S5PC110_SROMC_BASE + 8);
}

void motherboard_init(void) 
{
	s5pc110_gpio = (struct s5pc110_gpio *)S5PC110_GPIO_BASE;

	lan9311_preinit();
}


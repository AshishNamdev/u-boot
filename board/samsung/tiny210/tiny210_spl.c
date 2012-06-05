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
#include <asm/arch/cpu.h>

typedef u32(*copy_sd_mmc_to_mem)
(u32 channel, u32 start_block, u16 block_size, u32 *trg, u32 init);

void __spin(unsigned long d) {
	int loops;
	for (; d > 0; d --) {
		for (loops = 500; loops > 0; loops --)
			asm("nop");
	}
}

void serial_puts(const char *s) {
	while (*s != 0) {
		serial_putc(*s++);
	}
}

void serial_putc(const char c)
{
	volatile char *txh = (char *)0xE2900020;
	if (c == '\n')
		*txh = '\r';
	*txh = c;
}

void copy_mmc_to_ram(unsigned int blk_offset, unsigned int blk_count, void *destination)
{
	ulong ch;
	ch = *(volatile u32 *)(0xD0037488);
	copy_sd_mmc_to_mem copy_bl2 =
	    (copy_sd_mmc_to_mem) (*(u32 *) (0xD0037F98));

	u32 ret;
	if (ch == 0xEB000000) {
		ret = copy_bl2(0, blk_offset, blk_count, destination, 0);
	}
	else if (ch == 0xEB200000) {
		ret = copy_bl2(2, blk_offset, blk_count, destination, 0);
	}
}

void board_init_f(unsigned long bootflag)
{
        __attribute__((noreturn)) void (*uboot)(void);

	unsigned int *inform3 = (unsigned int *)0xE010F00C;

	unsigned char *dest = (unsigned char *)CONFIG_SYS_TEXT_BASE;
	int i;

	serial_puts("Copying u-boot to ram\n");
	copy_mmc_to_ram(CONFIG_SPL_UBOOT_OFS, CONFIG_SPL_UBOOT_SIZ, dest);

        /* Jump to U-Boot image */
	*inform3 = 0x0000bead; /* Don't bother to reinit */
        uboot = (void *)CONFIG_SYS_TEXT_BASE;
        (*uboot)();

	while(1);
}

/* Place Holders */
void board_init_r(gd_t *id, ulong dest_addr)
{
	while(1);
}


void hex_char(unsigned char x) {

	serial_puts("0x");

	if ( (x & 0xF0) >= 0xa0)
		serial_putc(( (x >> 4) & 0xF) - 0xa + 'A');
	else
		serial_putc(( (x >> 4) & 0xF) + '0');

	if ( (x & 0xF) >= 0xa)
		serial_putc((x & 0xF) - 0xa + 'A');
	else
		serial_putc((x & 0xF) + '0');
	
}




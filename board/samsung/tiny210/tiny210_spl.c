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


#include <fat.h>
#include <malloc.h>
#include <mmc.h>

DECLARE_GLOBAL_DATA_PTR;

/* Define global data structure pointer to it*/
static gd_t gdata __attribute__ ((section(".data")));
static bd_t bdata __attribute__ ((section(".data")));
unsigned int s5p_cpu_id;

void __udelay(unsigned long delay)
{
	unsigned int loops;
	for (;delay > 0; delay--)
		for (loops = 250; loops > 0; loops --)
			asm ("nop");
}

void board_init_f(unsigned long bootflag)
{
	/*
	 * We call relocate_code() with relocation target same as the
	 * CONFIG_SYS_SPL_TEXT_BASE. This will result in relocation getting
	 * skipped. Instead, only .bss initialization will happen. That's
	 * all we need
	 */
	serial_puts(">>board_init_f()\n");
	relocate_code(CONFIG_SYS_INIT_SP_ADDR & ~7, &gdata, CONFIG_SPL_TEXT_BASE);
	while(1);
}

void hang (void){
	serial_puts("Board Hung...\n");
	while(1);
}

/* Place Holders */
void board_init_r(gd_t *id, ulong dest_addr)
{
	struct mmc *mmc;
	int err, i;
	serial_puts(">>board_init_r()\n");

	unsigned char *hdr;

	gd = &gdata;
	gd->bd = &bdata;
	gd->have_console = 1;
	gd->flags = 0;

	s5p_set_cpu_id();
	printf("CPU is 0x%0x\n", s5p_cpu_id);

	mem_malloc_init(CONFIG_SYS_SPL_MALLOC_START, CONFIG_SYS_SPL_MALLOC_SIZE);

	mmc_initialize(gd->bd);

	mmc = find_mmc_device(0);
	if (!mmc) {
		puts("spl: mmc device not found!!\n");
		hang();
	}

	err = mmc_init(mmc);
	if (err) {
		printf("spl: mmc init failed: err - %d\n", err);
		hang();
	}

	hdr = malloc(512);
	err = mmc->block_dev.block_read(0,
			33, 1,
			(void *)hdr);

	hang();
}


void __serial_puts(const char *s) {
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
	
void serial_puts(const char *s) {
}

#ifdef CONFIG_GENERIC_MMC
void set_mmc_pins(int ch)
{
	unsigned int *gpio = (unsigned int *)S5PC110_GPIO_BASE;
	if (ch == 0) {
		gpio[0x1a0 / 4] = 0x02222222;	/* MMC	    */
		gpio[0x1a8 / 4] = 0x0;		/* No Pull  */
		gpio[0x1ac / 4] = 0x3FFF;	/* 4X Drive */
		printf("gpg0: 0x%08x 0x%08x 0x%08x\n", gpio[0x1a0 / 4], gpio[0x1a8 / 4], gpio[0x1ac / 4]);
	}

	if (ch == 2) {
		gpio[0x1e0 / 4] = 0x02222222;	/* MMC	    */
		gpio[0x1e8 / 4] = 0x0;		/* No Pull  */
		gpio[0x1ec / 4] = 0x3FFF;	/* 4X Drive */
	}
}

int board_mmc_init(bd_t *bis)
{
	ulong ch = *(volatile u32 *)(0xD0037488);
	printf("MMC @ 0x%08x\n", (unsigned int)ch);

	if (ch == 0xeb000000UL) {
		set_mmc_pins(0);
		s5p_mmc_init(0, 4);

	} else if (ch == 0xeb200000UL) {
		set_mmc_pins(2);
		s5p_mmc_init(2, 4);
	} else {
		printf("No MMC!\n");
		return -1;
	}

	return 0;
}
#endif


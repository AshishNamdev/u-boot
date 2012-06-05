/*
 * Copyright (C) 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * Configuation settings for the SAMSUNG Universal (EXYNOS4210) board.
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
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_SAMSUNG			1	/* in a SAMSUNG core */
#define CONFIG_S5P			1	/* which is in a S5P Family */
#define CONFIG_S5PC110			1	/* which is in a S5PC110 SoC */
#define CONFIG_TINY210			1
#define CONFIG_MACH_TYPE		MACH_TYPE_TINY210

/* SPL controls */
#define CONFIG_SPL			1
#define CONFIG_SPL_MMC_SUPPORT		1
#define CONFIG_SPL_LIBCOMMON_SUPPORT	1
#define CONFIG_SPL_LIBGENERIC_SUPPORT	1
#define CONFIG_SPL_LIBDISK_SUPPORT	1

#include <asm/arch/cpu.h>		/* get chip and board defs */

#define CONFIG_ARCH_CPU_INIT
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* Keep L2 Cache Disabled */
#define CONFIG_SYS_L2CACHE_OFF		1
#define CONFIG_SYS_DCACHE_OFF           1

#define CONFIG_SYS_SDRAM_BASE           0x20000000
#define CONFIG_SYS_TEXT_BASE            0x23E00000
#define CONFIG_SPL_TEXT_BASE		0xD0020010
#define CONFIG_SYS_INIT_SP_ADDR 	(CONFIG_SYS_LOAD_ADDR - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_SPL_MALLOC_SIZE	(128 * 1024)
#define CONFIG_SYS_SPL_MALLOC_START	(PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE - CONFIG_SYS_SPL_MALLOC_SIZE)
#define MEMORY_BASE_ADDRESS		CONFIG_SYS_SDRAM_BASE


/* input clock of PLL: MINI210 has 24MHz input clock */
#define CONFIG_SYS_CLK_FREQ		24000000

#define CONFIG_NR_DRAM_BANKS		1
#define SDRAM_BANK_SIZE			0x20000000				/* 512MB */
#define PHYS_SDRAM_1			MEMORY_BASE_ADDRESS
#define PHYS_SDRAM_1_SIZE		SDRAM_BANK_SIZE
#define CONFIG_SYS_MEMTEST_START	MEMORY_BASE_ADDRESS
#define CONFIG_SYS_MEMTEST_END		(MEMORY_BASE_ADDRESS + 0x3E00000)	/* 256 MB in DRAM	*/
#define CONFIG_SYS_LOAD_ADDR		(PHYS_SDRAM_1 + 0x1000000)		/* default load address	*/
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 896 * 1024)


#define CONFIG_SYS_NO_FLASH		1
#undef  CONFIG_CMD_IMLS
#define CONFIG_IDENT_STRING		" for FriendlyARM-TINY210"

#define CONFIG_ENV_IS_NOWHERE		1
#define CONFIG_ENV_SIZE			0x20000					/* 128KB */

#define CONFIG_SYS_HZ			2084375					/* Actual clock rate of PWM4 - anyone calling get_timer() needs it */

/* select serial console configuration */
#define CONFIG_SERIAL_MULTI		1
#define CONFIG_SERIAL0			1					/* use SERIAL 0 */
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP							/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER							/* use "hush" command parser    */
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_PROMPT		"Indicomm # "
#define CONFIG_SYS_CBSIZE		256					/* Console I/O Buffer Size*/
#define CONFIG_SYS_PBSIZE		384					/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS		16					/* max number of command args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define BOARD_LATE_INIT

/* Drivers */
#define CONFIG_PWM			1
#define CONFIG_GENERIC_MMC		1
#define CONFIG_MMC			1
#define CONFIG_DOS_PARTITION		1
#define CONFIG_S5P_MMC			1


/* Command Support */
#include <config_cmd_default.h>
#define CONFIG_CMD_MMC			1
#define CONFIG_CMD_FAT			1
#endif

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

#define CONFIG_MACH_TYPE		MACH_TYPE_INDICOMM_DC
#define MACH_TYPE_INDICOMM_DC		2458
#define CONFIG_TINY210_IDC1		1

#include "tiny210_common.h"

#define TINY210_SPL_BOOT_NAND		1

/* SPL controls */
#define CONFIG_SPL			1
#define CONFIG_SPL_UBOOT_OFS		(16 * 1024)		/* After the SPL */
#define CONFIG_SPL_UBOOT_SIZ		((256 - 16) * 1024)	/* Max 2 sectors for SPL + UBOOT */
#define CONFIG_SPL_NAND_PAGE_SIZ	(2 * 1024)
#define CONFIG_SPL_NAND_SECT_SIZ	(128 * 1024)

#define CONFIG_BOARD_NAME		"Indicomm DC1"
#define CONFIG_IDENT_STRING		" for " CONFIG_BOARD_NAME

/* Ethernet */
#define CONFIG_SMC911X			1
#define CONFIG_SMC911X_32_BIT		1
#define CONFIG_SMC911X_BASE		(0x88000000)

/* Environment */
#define CONFIG_ENV_IS_IN_NAND		1
#define CONFIG_ENV_SIZE			0x20000		/* 128KB */
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_ENV_OFFSET		(256 * 1024)	/* Third Sector */

#define CONFIG_EXTRA_ENV_SETTINGS \
	"mtdids=" DEFAULT_MTDID "\0" \
	"mtdparts=" DEFAULT_MTDPARTS "\0"

/* Turn on... */
#define CONFIG_CMD_PING			1

/* Turn off... */
#undef CONFIG_GENERIC_MMC
#undef CONFIG_MMC
#undef CONFIG_S5P_MMC
#undef CONFIG_DOS_PARTITION

#undef CONFIG_CMD_MMC
#undef CONFIG_CMD_FAT
#undef CONFIG_CMD_EXT2

#endif

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

#define CONFIG_MACH_TYPE		MACH_TYPE_TINY210
#define MACH_TYPE_TINY210		2457
#define CONFIG_TINY210_SDK		1

#include "tiny210_common.h"

#define TINY210_SPL_BOOT_MMC		1

/* SPL controls */
#define CONFIG_SPL			1
#define CONFIG_SPL_UBOOT_OFS		(((512 + (16 * 1024) + (CONFIG_ENV_SIZE)) / 512) + 1)
#define CONFIG_SPL_UBOOT_SIZ		(512 * 1024 / 512)

#define CONFIG_BOARD_NAME		"TINY210SDK"
#define CONFIG_IDENT_STRING		" for Indicomm " CONFIG_BOARD_NAME " Bootstrap"

#define CONFIG_ENV_IS_IN_MMC		1
#define CONFIG_ENV_SIZE			0x20000					/* 128KB */
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_ENV_OFFSET		(512 + (16 * 1024))			/* After the DOS partition table and SPL loader */

/* DM9000 Ethernet */
#define DM9000_16BIT_DATA		1
#define CONFIG_DRIVER_DM9000		1
#define CONFIG_NET_MULTI		1
#define CONFIG_NET_RETRY_COUNT		10
#define CONFIG_DM9000_NO_SROM		1
#define CONFIG_DM9000_BASE		(0x88000000)
#define DM9000_IO			(CONFIG_DM9000_BASE)
#define DM9000_DATA			(CONFIG_DM9000_BASE+0x8)

#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootdelay=0\0" \
	"bootcmd=ext2load mmc 0:1 0x22000000 auto.scr; source 0x22000000\0" \
	"mtdids=" DEFAULT_MTDID "\0" \
	"mtdparts=" DEFAULT_MTDPARTS "\0"


#endif

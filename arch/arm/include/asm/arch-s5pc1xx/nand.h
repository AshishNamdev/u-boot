/*
 * (C) Copyright 2012 MDSoft Ltd
 * Mike Dyer <mike.dyer@md-soft.co.uk>
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
 * Note: This file contains the register description for the NAND subsystem
 */
#ifndef __S5PNAND_H
#define __S5PNAND_H

#define NFCONF_ECC_MLC			(1<<24)
#define NFCONF_ECC_1BIT			(0<<23)
#define NFCONF_ECC_4BIT			(2<<23)
#define NFCONF_ECC_8BIT			(1<<23)

#define NFCONT_ECC_ENC			(1<<18)
#define NFCONT_WP			(1<<16)
#define NFCONT_MECCLOCK			(1<<7)
#define NFCONT_SECCLOCK			(1<<6)
#define NFCONT_INITMECC			(1<<5)
#define NFCONT_INITSECC			(1<<4)
#define NFCONT_INITECC			(NFCONT_INITMECC | NFCONT_INITSECC)
#define NFCONT_CS			(1<<1)
#define NFCONT_ENABLE			(1<<0)

#define NFSTAT_ECCENCDONE		(1<<25)
#define NFSTAT_ECCDECDONE		(1<<24)
#define NFSTAT_RnB			(1<<0)

#define NFESTAT0_ECCBUSY		(1<<31)

#ifndef __ASSEMBLY__
struct s5p_nand {
	unsigned int nfconf;		/* 0x00 */
	unsigned int nfcont;		/* 0x04 */
	unsigned int nfcmd;		/* 0x08 */
	unsigned int nfaddr;		/* 0x0c */
	unsigned int nfdata;		/* 0x10 */
	unsigned int nfmeccd[2];	/* 0x14 */
	unsigned int nfseccd;		/* 0x1c */
	unsigned int nfsblk;		/* 0x20 */
	unsigned int nfeblk;		/* 0x24 */
	unsigned int nfstat;		/* 0x28 */
	unsigned int nfeccerr[2];	/* 0x2c */
	unsigned int nfmecc[2];		/* 0x34 */
	unsigned int nfsecc;		/* 0x3c */
	unsigned int nfmlcbitpt;	/* 0x40 */
	unsigned int __resv0[32751];	/* 0x44 */

	unsigned int nfeccconf;		/* 0x20000 */
	unsigned int __resv1[7];	/* 0x20004 */
	unsigned int nfecccont;		/* 0x20020 */
	unsigned int __resv2[3];	/* 0x20024 */
	unsigned int nfeccstat;		/* 0x20030 */
	unsigned int __resv3[3];	/* 0x20034 */
	unsigned int nfeccsecstat;	/* 0x20040 */
	unsigned int __resv4[19];	/* 0x20044 */
	unsigned int nfeccprgecc[7];	/* 0x20090 */
	unsigned int __resv5[5];	/* 0x200AC */
	unsigned int nfeccerl[8];	/* 0x200C0 */
	unsigned int __resv6[4];	/* 0x200E0 */
	unsigned int nfeccerp[4];	/* 0x200F0 */
	unsigned int __resv7[4];	/* 0x20100 */
	unsigned int nfeccconecc[7];	/* 0x20110 */
};
#endif

#endif

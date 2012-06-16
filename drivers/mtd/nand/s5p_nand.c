/*
 * (C) Copyright 2012 MDSoft Limited
 *     Modified for S5P nand driver
 *
 * (C) Copyright 2006 DENX Software Engineering
 *
 * Implementation for U-Boot 1.1.6 by Samsung
 *
 * (C) Copyright 2008
 * Guennadi Liakhovetki, DENX Software Engineering, <lg@denx.de>
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

#include <common.h>

#include <nand.h>
#include <linux/mtd/nand.h>

#include <asm/arch/cpu.h>
#include <asm/arch/nand.h>

#include <asm/io.h>
#include <asm/errno.h>

#define MAX_CHIPS	2
static int nand_cs[MAX_CHIPS] = {0, 1};

#ifdef CONFIG_NAND_SPL
#define printf(arg...) do {} while (0)
#endif

static struct s5p_nand *s5p_nand_regs = NULL;

static struct nand_ecclayout s5p_64_8bit = {
	.eccbytes = 52,
	.eccpos = {
		   12, 13, 14, 15,
		   16, 17, 18, 19, 20, 21, 22, 23,
		   24, 25, 26, 27, 28, 29, 30, 31,
		   32, 33, 34, 35, 36, 37, 38, 39,
		   40, 41, 42, 43, 44, 45, 46, 47,
		   48, 49, 50, 51, 52, 53, 54, 55,
		   56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = {
		{.offset = 2,
		 .length = 10}}
};

/* Nand flash definition values by jsgood */
#ifdef S5P_NAND_DEBUG
/*
 * Function to print out oob buffer for debugging
 * Written by jsgood
 */
static void print_oob(const char *header, struct mtd_info *mtd)
{
	int i;
	struct nand_chip *chip = mtd->priv;

	printf("%s:\t", header);

	for (i = 0; i < 64; i++)
		printf("%02x ", chip->oob_poi[i]);

	printf("\n");
}
#endif /* S5P_NAND_DEBUG */

static void s5p_nand_select_chip(struct mtd_info *mtd, int chip)
{
	int ctrl = readl(&s5p_nand_regs->nfcont);

	switch (chip) {
	case -1:
		ctrl |= 6;
		break;
	case 0:
		ctrl &= ~2;
		break;
	case 1:
		ctrl &= ~4;
		break;
	default:
		return;
	}

	writel(ctrl, &s5p_nand_regs->nfcont);
}

/*
 * Hardware specific access to control-lines function
 * Written by jsgood
 */
static void s5p_nand_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *this = mtd->priv;
	
	if (ctrl & NAND_CTRL_CHANGE) {
		if (ctrl & NAND_CLE)
			this->IO_ADDR_W = (void __iomem *)&s5p_nand_regs->nfcmd;
		else if (ctrl & NAND_ALE)
			this->IO_ADDR_W = (void __iomem *)&s5p_nand_regs->nfaddr;
		else
			this->IO_ADDR_W = (void __iomem *)&s5p_nand_regs->nfdata;
		if (ctrl & NAND_NCE)
			s5p_nand_select_chip(mtd, *(int *)this->priv);
		else
			s5p_nand_select_chip(mtd, -1);
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, this->IO_ADDR_W);
}

/*
 * Function for checking device ready pin
 * Written by jsgood
 */
static int s5p_nand_device_ready(struct mtd_info *mtdinfo)
{
	return !!(readl(&s5p_nand_regs->nfstat) & NFSTAT_RnB);
}

#ifdef CONFIG_SYS_S5P_NAND_HWECC
static int curr_ecc_mode = -1;

static void s5p_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	u32 nfecconf, nfeccont, nfcont, nfecstat;

	curr_ecc_mode = mode;

	/* Clear done bits */
	nfecstat = readl(&s5p_nand_regs->nfeccstat);
	nfecstat |= (1 << 24) | (1 << 25);
	writel(nfecstat, &s5p_nand_regs->nfeccstat);

	nfecconf = readl(&s5p_nand_regs->nfeccconf);
	nfecconf &= ~0x3FF0007;
	nfecconf |=  0x1FF0003;	/* 512 byte message, 8bit ECC */

	writel(nfecconf, &s5p_nand_regs->nfeccconf);
	/* printf("NFECCCONF: 0x%08x\n", nfecconf); */

	/* Initialise ECC */
	nfeccont = readl(&s5p_nand_regs->nfecccont);
	nfeccont |= (1 << 2);
	if (mode == NAND_ECC_WRITE)
		nfeccont |= (1 << 16);
	else
		nfeccont &= ~(1 << 16);

	writel(nfeccont, &s5p_nand_regs->nfecccont);
	/* printf("NFECCCONT: 0x%08x\n", nfeccont); */


	/* Unlock */
	nfcont = readl(&s5p_nand_regs->nfcont);
	nfcont &= ~NFCONT_MECCLOCK;
	writel(nfcont, &s5p_nand_regs->nfcont);
	/* printf("NFCONT: 0x%08x\n", nfcont); */

}

/*
 * Function for checking ECCEncDone in NFSTAT
 */
static void s5p_nand_wait_enc(void)
{
	u32 timeo = (CONFIG_SYS_HZ * 20) / 1000;
	u32 time_start;

	time_start = get_timer(0);

	/* wait until command is processed or timeout occures */
	while (get_timer(time_start) < timeo) {
		if (readl(&s5p_nand_regs->nfeccstat) & (1 << 25)) {
			/* printf("Encode complete\n"); */
			break;
		}
	}
}

/*
 * Function for checking ECCDecDone in NFSTAT
 */
static void s5p_nand_wait_dec(void)
{
	u32 timeo = (CONFIG_SYS_HZ * 20) / 1000;
	u32 time_start;

	time_start = get_timer(0);

	/* wait until command is processed or timeout occures */
	while (get_timer(time_start) < timeo) {
		if (readl(&s5p_nand_regs->nfeccstat) & (1 << 24)) {
			/* printf("Decode complete\n"); */
			break;
		}
	}
}


/*
 * This function is called immediately after encoding ecc codes.
 * This function returns encoded ecc codes.
 * Written by jsgood
 */
static int s5p_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat,
				  u_char *ecc_code)
{
	u_long nfcont, parity;
	int i;

	/* Lock */
	nfcont = readl(&s5p_nand_regs->nfcont);
	nfcont |= NFCONT_MECCLOCK;
	writel(nfcont, &s5p_nand_regs->nfcont);

	if (curr_ecc_mode ==  NAND_ECC_READ) {
		s5p_nand_wait_dec();
		return 0;
	}

	s5p_nand_wait_enc();

	for (i = 0; i <= 12; i += 4) {
		parity = readl(&s5p_nand_regs->nfeccprgecc[i / 4]);
		ecc_code[i] = parity & 0xff;
		ecc_code[i + 1] = (parity >> 8) & 0xff;
		if (i < 12) {
			ecc_code[i + 2] = (parity >> 16) & 0xff;
			ecc_code[i + 3] = (parity >> 24) & 0xff;
		}
	}

	/*
	printf("ECC: 0x%02x 0x%02x 0x%02x 0x%02x\n", ecc_code[0], ecc_code[1], ecc_code[2], ecc_code[3]);
	printf("ECC: 0x%02x 0x%02x 0x%02x 0x%02x\n", ecc_code[4], ecc_code[5], ecc_code[6], ecc_code[7]);
	printf("ECC: 0x%02x 0x%02x 0x%02x 0x%02x\n", ecc_code[8], ecc_code[9], ecc_code[10], ecc_code[11]);
	printf("ECC: 0x%02x 0x%02x\n", ecc_code[12], ecc_code[13]);
	*/
	return 0;
}

/*
 * This function determines whether read data is good or not.
 * If SLC, must write ecc codes to controller before reading status bit.
 * If MLC, status bit is already set, so only reading is needed.
 * If status bit is good, return 0.
 * If correctable errors occured, do that.
 * If uncorrectable errors occured, return -1.
 * Written by jsgood
 */
static int s5p_nand_correct_data(struct mtd_info *mtd, u_char *dat,
				 u_char *read_ecc, u_char *calc_ecc)
{
	u32 nfecstat, nfecsecstat;
	int errors;
 	int i;

	/*
	printf("RdECC: 0x%02x 0x%02x 0x%02x 0x%02x\n", read_ecc[0], read_ecc[1], read_ecc[2], read_ecc[3]);
	printf("RdECC: 0x%02x 0x%02x 0x%02x 0x%02x\n", read_ecc[4], read_ecc[5], read_ecc[6], read_ecc[7]);
	printf("RdECC: 0x%02x 0x%02x 0x%02x 0x%02x\n", read_ecc[8], read_ecc[9], read_ecc[10], read_ecc[11]);
	printf("RdECC: 0x%02x 0x%02x\n", read_ecc[12], read_ecc[13]);
	*/

	nfecsecstat = readl(&s5p_nand_regs->nfeccsecstat);

	errors = nfecsecstat & 0x1F;
	if (errors == 0) {
		/* printf("No ECC Errors\n"); */
		return 0;
	}

	if (errors > 8) {
		printf("Uncorrectable ECC errors\n");
		return -1;
	}

	printf("%d ECC correctable errors!\n", errors);

	for (i = 0; i < 16; i++) {
		if ( ((nfecsecstat >> (8 + i)) & 0x01) != 0) {
			/* This location is valid */
			int loc = (readl(&s5p_nand_regs->nfeccerl[i / 2]) >> ((i % 2) * 16)) & 0x3FF;
			int pat = (readl(&s5p_nand_regs->nfeccerp[i / 4]) >> ((i % 4) * 8)) & 0xFF;
			dat[loc] ^= pat;
		}
	}

	return 0;
}

/**
 * nand_read_page_hwecc_oob_first - [REPLACABLE] hw ecc, read oob first
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	buffer to store read data
 * @page:	page number to read
 *
 * For 8bit ECC decode, we need all the parity read first, so we can write it in
 * to the decoding engine.
 */
static int nand_read_page_hwecc_oob_first(struct mtd_info *mtd,
	struct nand_chip *chip, uint8_t *buf, int page)
{
	int i, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *p = buf;
	uint8_t *ecc_code = chip->buffers->ecccode;
	uint32_t *eccpos = chip->ecc.layout->eccpos;
	uint8_t *ecc_calc = chip->buffers->ecccalc;

	/* Read the OOB area first */
	chip->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);
	chip->cmdfunc(mtd, NAND_CMD_READ0, 0, page);

	for (i = 0; i < chip->ecc.total; i++)
		ecc_code[i] = chip->oob_poi[eccpos[i]];

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		int stat;

		chip->ecc.hwctl(mtd, NAND_ECC_READ);
		chip->read_buf(mtd, p, eccsize);
		chip->write_buf(mtd, chip->oob_poi + eccpos[0] + ((chip->ecc.steps - eccsteps) * eccbytes), eccbytes);
		chip->ecc.calculate(mtd, p, &ecc_calc[i]);

		stat = chip->ecc.correct(mtd, p, &ecc_code[i], NULL);
		if (stat < 0)
			mtd->ecc_stats.failed++;
		else
			mtd->ecc_stats.corrected += stat;
	}
	return 0;
}

#endif /* CONFIG_SYS_S5P_NAND_HWECC */

void dump_addr()
{
	printf("\n");
	printf("nfconf\t\t%p\n", &s5p_nand_regs->nfconf);
	printf("nfcont\t\t%p\n", &s5p_nand_regs->nfcont);
	printf("nfcmd\t\t%p\n", &s5p_nand_regs->nfcmd);
	printf("nfaddr\t\t%p\n", &s5p_nand_regs->nfaddr);
	printf("nfdata\t\t%p\n", &s5p_nand_regs->nfdata);
	printf("nfmeccd\t\t%p\n", &s5p_nand_regs->nfmeccd);
	printf("nfseccd\t\t%p\n", &s5p_nand_regs->nfseccd);
	printf("nfsblk\t\t%p\n", &s5p_nand_regs->nfsblk);
	printf("nfeblk\t\t%p\n", &s5p_nand_regs->nfeblk);
	printf("nfstat\t\t%p\n", &s5p_nand_regs->nfstat);
	printf("nfeccerr\t%p\n", &s5p_nand_regs->nfeccerr);
	printf("nfmecc\t\t%p\n", &s5p_nand_regs->nfmecc);
	printf("nfsecc\t\t%p\n", &s5p_nand_regs->nfsecc);
	printf("nfmlcbitpt\t%p\n", &s5p_nand_regs->nfmlcbitpt);
	printf("nfeccconf\t%p\n", &s5p_nand_regs->nfeccconf);
	printf("nfecccont\t%p\n", &s5p_nand_regs->nfecccont);
	printf("nfeccstat\t%p\n", &s5p_nand_regs->nfeccstat);
	printf("nfeccsecstat\t%p\n", &s5p_nand_regs->nfeccsecstat);
	printf("nfeccprgecc\t%p\n", &s5p_nand_regs->nfeccprgecc);
	printf("nfeccerl\t%p\n", &s5p_nand_regs->nfeccerl);
	printf("nfeccerp\t%p\n", &s5p_nand_regs->nfeccerp);
	printf("nfeccconecc\t%p\n", &s5p_nand_regs->nfeccconecc);
}

/*
 * Board-specific NAND initialization. The following members of the
 * argument are board-specific (per include/linux/mtd/nand.h):
 * - IO_ADDR_R?: address to read the 8 I/O lines of the flash device
 * - IO_ADDR_W?: address to write the 8 I/O lines of the flash device
 * - hwcontrol: hardwarespecific function for accesing control-lines
 * - dev_ready: hardwarespecific function for  accesing device ready/busy line
 * - enable_hwecc?: function to enable (reset)  hardware ecc generator. Must
 *   only be provided if a hardware ECC is available
 * - eccmode: mode of ecc, see defines
 * - chip_delay: chip dependent delay for transfering data from array to
 *   read regs (tR)
 * - options: various chip options. They can partly be set to inform
 *   nand_scan about special functionality. See the defines for further
 *   explanation
 * Members with a "?" were not set in the merged testing-NAND branch,
 * so they are not set here either.
 */
int board_nand_init(struct nand_chip *nand)
{
	static int chip_n;
	unsigned int nfcont;

	if (chip_n >= MAX_CHIPS)
		return -ENODEV;

	s5p_nand_regs = (struct s5p_nand *)samsung_get_base_nand();

	if (s5p_nand_regs == NULL)
		return -ENODEV;

	/* dump_addr(); */

	nfcont = readl(&s5p_nand_regs->nfcont);
	nfcont = ((nfcont & ~NFCONT_WP) | NFCONT_ENABLE | 0x6);
	writel(nfcont, &s5p_nand_regs->nfcont);

	nand->IO_ADDR_R		= (void __iomem *)&s5p_nand_regs->nfdata;
	nand->IO_ADDR_W		= (void __iomem *)&s5p_nand_regs->nfdata;
	nand->cmd_ctrl		= s5p_nand_hwcontrol;
	nand->dev_ready		= s5p_nand_device_ready;
	nand->select_chip	= s5p_nand_select_chip;
	nand->options		= 0;
#ifdef CONFIG_SPL_BUILD
	nand->read_byte		= nand_read_byte;
	nand->write_buf		= nand_write_buf;
	nand->read_buf		= nand_read_buf;
#endif

#ifdef CONFIG_SYS_S5P_NAND_HWECC
	nand->ecc.hwctl		= s5p_nand_enable_hwecc;
	nand->ecc.calculate	= s5p_nand_calculate_ecc;
	nand->ecc.correct	= s5p_nand_correct_data;
	nand->ecc.layout	= &s5p_64_8bit;
	nand->ecc.read_page 	= nand_read_page_hwecc_oob_first;

	/*
	 * If you get more than 1 NAND-chip with different page-sizes on the
	 * board one day, it will get more complicated...
	 */
	nand->ecc.mode		= NAND_ECC_HW_OOB_FIRST;
	nand->ecc.size		= CONFIG_SYS_NAND_ECCSIZE;
	nand->ecc.bytes		= CONFIG_SYS_NAND_ECCBYTES;
#else
	nand->ecc.mode		= NAND_ECC_SOFT;
#endif /* ! CONFIG_SYS_S5P_NAND_HWECC */

	nand->priv		= nand_cs + chip_n++;

	return 0;
}

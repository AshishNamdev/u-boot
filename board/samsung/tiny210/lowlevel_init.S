/*
 * Low Level Init
 *
 * Copyright (C) 2012 MDSoft Ltd, Mike Dyer <mike.dyer@md-soft.co.uk>
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


#include <config.h>
#include <version.h>
#include <asm/arch-s5pc1xx/cpu.h>
#include "regs.h"


_TEXT_BASE:
	.word	CONFIG_SYS_TEXT_BASE
	.globl lowlevel_init

lowlevel_init:	
	push	{lr}

	/* If this is u-boot re-initing after SPL, don't bother */
	ldr	r0, =0xE010F00C @INFORM6
	ldr	r1, =0x0000bead
	ldr	r2, [r0]
	cmp	r1, r2
	ldr	r1, =0
	str	r1, [r0]
	popeq	{pc}

	bl	leds_on
	bl	uart_asm_init
	bl	clocks_to_max
	bl	init_dram

	adr	r0, boot_msg
	bl	uart_asm_puts
	
	pop	{pc}

leds_on:
	ldr	r0, =S5PC110_GPIO_BASE
	ldr	r1, [r0, #0x280]
	ldr	r2, =0xFFFF
	bic	r1, r2
	ldr	r2, =0x1111
	orr	r1, r1, r2
	str	r1, [r0, #0x280]

	ldr	r1, [r0, #0x284]
	bic	r1, #0x0F
	str	r1, [r0, #0x284]
	mov	pc, lr

clocks_to_max:
	push	{lr}

	/* Wait for previous stuff to settle */
	ldr	r1, =0x1000000
1:	subs	r1, #1
	bne	1b

	ldr	r0, =S5PC110_CLOCK_BASE

	/* Bypass PLLs */
	ldr	r1, =0
	str	r1, [r0, #0x200]

	/* APLL 1GHz */
	ldr	r1, =(1 << 31 | 125 << 16 | 3 << 8 | 1)
	str	r1, [r0, #0x100]
	ldr	r1, =0
	str	r1, [r0, #0x104]

	/* MPLL 667MHz */
	ldr	r1, =(1 << 31 | 667 << 16 | 12 << 8 | 1)
	str	r1, [r0, #0x108]

	/* System dividers */
	ldr	r1, =0x14131440
	str	r1, [r0, #0x300]

	/* Stabilize */
	ldr	r1, =0x10000
1:	subs	r1, #1
	bne	1b

	/* Un-bypass PLLs */
	ldr	r1, =0x10001111
	str	r1, [r0, #0x200]

	/* Stabilize */
	ldr	r1, =0x10000
1:	subs	r1, #1
	bne	1b

#ifdef DEBUG
	adr	r0, clk_msg
	bl	uart_asm_puts
#endif

	pop	{pc}
	
uart_asm_init:
	push	{lr}

	/* Mux out the UART pins */
	ldr	r0, =S5PC110_GPIO_BASE
	ldr	r1, =0x22222222
	str	r1, [r0, #0x00]
	ldr	r1, =0x2222
	str	r1, [r0, #0x20]
		
	ldr	r0, =S5PC110_UART_BASE
	ldr	r1, =0x3
	str	r1, [r0, #0x00]
	ldr	r1, =0x305
	str	r1, [r0, #0x04]
	ldr	r1, =0x1
	str	r1, [r0, #0x08]
	ldr	r1, =34
	str	r1, [r0, #0x28]
	ldr	r1, =0xDDDD
	str	r1, [r0, #0x2c]

#ifdef DEBUG
	adr	r0, spl_msg
	bl	uart_asm_puts
#endif

	pop	{pc}

uart_asm_puts:
	ldr	r1, =S5PC110_UART_BASE
	
1:	ldrb	r2, [r0], #1
	cmp	r2, #0
	moveq	pc, lr
	and	r2, r2, #0xFF
	str	r2, [r1, #0x20]
	b	1b

#define DMC0_CONCONTROL		0x00
#define DMC0_MEMCONTROL 	0x04
#define DMC0_MEMCONFIG0 	0x08
#define DMC0_MEMCONFIG1 	0x0c
#define DMC0_DIRECTCMD		0x10
#define DMC0_PRECHCONFIG	0x14
#define DMC0_PHYCONTROL0	0x18
#define DMC0_PHYCONTROL1	0x1c
#define DMC0_PHYSTATUS		0x40
#define DMC0_TIMINGAREF		0x30
#define DMC0_TIMINGROW		0x34
#define DMC0_TIMINGDATA		0x38
#define DMC0_TIMINGPOWER	0x3C
#define DMC0_PWRDNCONFIG	0x28

init_dram:
	push	{lr}

	ldr	r0, =S5PC110_DMC0_BASE
	
	ldr	r1, =0x00101000			@PhyControl0 DLL parameter setting, manual 0x00101000
	str	r1, [r0, #DMC0_PHYCONTROL0]

	ldr	r1, =0x00000086			@PhyControl1 DLL parameter setting, LPDDR/LPDDR2 Case
	str	r1, [r0, #DMC0_PHYCONTROL1]

	ldr	r1, =0x00101002			@PhyControl0 DLL on
	str	r1, [r0,  #DMC0_PHYCONTROL0]

	ldr	r1, =0x00101003			@PhyControl0 DLL start
	str	r1, [r0,  #DMC0_PHYCONTROL0]

1:	ldr	r1, [r0, #DMC0_PHYSTATUS]	@Load Phystatus register value
	and	r2, r1, #0x7
	cmp	r2, #0x7			@Loop until DLL is locked
	bne	1b

	and	r1, #0x3fc0
	mov	r2, r1, LSL #18
	orr	r2, r2, #0x100000
	orr	r2 ,r2, #0x1000

	orr	r1, r2, #0x3			@Force Value locking

	str	r1, [r0, #DMC0_PHYCONTROL0]

	/* setting DDR2 */
	ldr	r1, =0x0FFF2010			@ConControl auto refresh off
	str	r1, [r0, #0x00]

	ldr	r1, =0x00202400			@MemControl BL=4, 1 chip, DDR2 type, dynamic self refresh, force precharge, dynamic power down off
	str	r1, [r0, #DMC0_MEMCONTROL]

	ldr	r1, =0x20E00323			@MemConfig0 512MB config, 8 banks,13 rows, 10 cols
	str	r1, [r0, #DMC0_MEMCONFIG0]

	ldr	r1, =0xFF000000			@PrechConfig
	str	r1, [r0, #DMC0_PRECHCONFIG]

	ldr	r1, =0x00000618			@TimingAref	7.8us*200MHz=0x618
	str	r1, [r0, #DMC0_TIMINGAREF]

	ldr	r1, =0x2B34438A			@TimingRow	for @200MHz
	str	r1, [r0, #DMC0_TIMINGROW]

	ldr	r1, =0x24240000			@TimingData	CL=4
	str	r1, [r0, #DMC0_TIMINGDATA]

	ldr	r1, =0x0BDC0343			@TimingPower
	str	r1, [r0, #DMC0_TIMINGPOWER]

	ldr	r1, =0x07000000			@DirectCmd	chip0 Deselect
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x01000000			@DirectCmd	chip0 PALL
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x00020000			@DirectCmd	chip0 EMRS2
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x00030000			@DirectCmd	chip0 EMRS3
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x00010400			@DirectCmd	chip0 EMRS1 (MEM DLL on, DQS# disable)
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x00000542			@DirectCmd	chip0 MRS (MEM DLL reset) CL=4, BL=4
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x01000000			@DirectCmd	chip0 PALL
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x05000000			@DirectCmd	chip0 REFA
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x05000000			@DirectCmd	chip0 REFA
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x00000442			@DirectCmd	chip0 MRS (MEM DLL unreset)
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x00010780			@DirectCmd	chip0 EMRS1 (OCD default)
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x00010400			@DirectCmd	chip0 EMRS1 (OCD exit)
	str	r1, [r0, #DMC0_DIRECTCMD]

	ldr	r1, =0x0FF02030			@ConControl	auto refresh on
	str	r1, [r0, #DMC0_CONCONTROL]

	ldr	r1, =0xFFFF00FF			@PwrdnConfig
	str	r1, [r0, #DMC0_PWRDNCONFIG]

#ifdef DEBUG
	adr	r0, ram_msg
	bl	uart_asm_puts
#endif
	pop	{pc}

minidram_test:
	push	{lr}

	ldr	r0, =0x20000000
	ldr	r1, =0xaa55aa55

1:	str	r1, [r0], #4
	ldr	r2, =0x20000100
	cmp	r0, r2
	bne	1b

	ldr	r0, =0x20000000
2:	ldr	r2, [r0], #4
	cmp	r1, r2
	bne	3f
	ldr	r2, =0x20000100
	cmp	r0, r2
	bne	2b

	adr	r0, ok_msg
	bl	uart_asm_puts
	b	4f

3:	
	adr	r0, err_msg
	bl	uart_asm_puts
4:	pop	{pc}
	
spin:	b	spin

boot_msg:
	.asciz "Booty, Booty Boot!\r\n"
	.align
spl_msg:
	.asciz "UART Initialised\r\n"
	.align
clk_msg:
	.asciz "APLL 1GHz, MPLL 667MHz\r\n"
	.align
ram_msg:
	.asciz "DRAM initialised\r\n"
	.align
ok_msg:
	.asciz "DRAM Mini-Test OK\r\n"
	.align
err_msg:
	.asciz "DRAM Mini-Test Failed\r\n"
	.align

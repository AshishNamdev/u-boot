#include <common.h>
#include <s5pc110.h>
#include <nand.h>

#include <asm/arch/gpio.h>
#include <asm/io.h>

#ifdef DEBUG_SPL
void s5p_puts(const char * s) {
	static u32 txh = ELFIN_UART_CONSOLE_BASE + UTXH_OFFSET;
	volatile int i, j;

	while (*s != 0x00) {
		if (*s == '\n') {
			writel('\r', txh);
			for (i = 0; i < 1000; i++) j*=2;
		}
		writel(*s, txh);
		for (i = 0; i < 1000; i++) j*=2;
		s ++;
	}
}

void s5p_put_hex(u32 n)
{
	char s[11];
	int i, j;
	s[0] = '0';
	s[1] = 'x';
	
	for (i = 0; i < 8; i++)
	{
		j = (n >> (28 - (4 * i))) & 0xF;
		if (j <= 9)
			s[2 + i] = '0' + j;
		else
			s[2 + i] = 'a' + j - 0xa;
	}
	s[10] = '\0';
	s5p_puts(s);
}
#endif

void board_init_f(unsigned long bootflag)
{
	writeb(~0x01, GPJ2DAT);

	nand_init();

	writeb(~0x02, GPJ2DAT);

	nand_boot();

	while(1);
}

/* Place Holders */
void board_init_r(gd_t *id, ulong dest_addr)
{
	/* Function attribute is no-return */
	/* This Function never executes */
	while (1);
}

void save_boot_params(u32 r0, u32 r1, u32 r2, u32 r3) {}



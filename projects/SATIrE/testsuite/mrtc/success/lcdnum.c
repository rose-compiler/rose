/* MDH WCET BENCHMARK SUITE. File version $Id: lcdnum.c,v 1.3 2005/11/11 10:31:13 ael01 Exp $ */

/***********************************************************************
 *  FILE: lcdnum.c
 *
 *  PURPOSE: demonstrate effect of flow facts for straight loops
 *
 *  IDEA: reading from an in port mapped to a ten-item buffer,
 *        send first five characters to an LCD as numbers
 *
 ***********************************************************************/


 /*
  * Changes: JG 2005/12/23: Changed type of main to int Inserted prototypes.
                            Indented program.
  */

unsigned char   num_to_lcd(unsigned char a);

unsigned char 
num_to_lcd(unsigned char a)
{
	/*
	 * -0-            1            01 1   2         2   4        02  04
	 * -3-    i.e.    8     i.e.   08 4   5        16   32       10  20
	 * -6-           64            40
	 * 
	 */
	switch (a) {
		case 0x00:return 0;
	case 0x01:
		return 0x24;
	case 0x02:
		return 1 + 4 + 8 + 16 + 64;
	case 0x03:
		return 1 + 4 + 8 + 32 + 64;
	case 0x04:
		return 2 + 4 + 8 + 32;
	case 0x05:
		return 1 + 4 + 8 + 16 + 64;
	case 0x06:
		return 1 + 2 + 8 + 16 + 32 + 64;
	case 0x07:
		return 1 + 4 + 32;
	case 0x08:
		return 0x7F;	/* light all */
	case 0x09:
		return 0x0F + 32 + 64;
	case 0x0A:
		return 0x0F + 16 + 32;
	case 0x0B:
		return 2 + 8 + 16 + 32 + 64;
	case 0x0C:
		return 1 + 2 + 16 + 64;
	case 0x0D:
		return 4 + 8 + 16 + 32 + 64;
	case 0x0E:
		return 1 + 2 + 8 + 16 + 64;
	case 0x0F:
		return 1 + 2 + 8 + 16;
	}
	return 0;
}

volatile unsigned char IN;
volatile unsigned char OUT;

int 
main(void)
{
	int             i;
	unsigned char   a;

	for (i = 0; i < 10; i++) {
		a = IN;		/* scan port */
		if (i < 5) {
			a = a & 0x0F;
			OUT = num_to_lcd(a);
		}
	}
	return 0;
}

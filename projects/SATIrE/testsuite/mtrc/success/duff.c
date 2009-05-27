/* MDH WCET BENCHMARK SUITE. File version $Id: duff.c,v 1.4 2005/12/21 09:43:07 jgn Exp $ */

/*----------------------------------------------------------------------
 *  WCET Benchmark created by Jakob Engblom, Uppsala university,
 *  February 2000.
 *
 *  The purpose of this benchmark is to force the compiler to emit an
 *  unstructured loop, which is usually problematic for WCET tools to
 *  handle.
 *
 *  The execution time should be constant.
 *
 *  The original code is "Duff's Device", see the Jargon File, e.g. at
 *  http://www.tf.hut.fi/cgi-bin/jargon.  Created in the early 1980s
 *  as a way to express loop unrolling in C.
 *
 *----------------------------------------------------------------------*/

 /*
  * Changes: JG 2005/12/21: Changed type of main to int
                            Indented program.
  */

#define ARRAYSIZE  100
#define INVOCATION_COUNT 43	/* exec time depends on this one! */


void 
duffcopy(char *to, char *from, int count)
{
	int             n = (count + 7) / 8;
	switch (count % 8) {
	case 0:
		do {
			*to++ = *from++;
	case 7:
			*to++ = *from++;
	case 6:
			*to++ = *from++;
	case 5:
			*to++ = *from++;
	case 4:
			*to++ = *from++;
	case 3:
			*to++ = *from++;
	case 2:
			*to++ = *from++;
	case 1:
			*to++ = *from++;
		} while (--n > 0);
	}
}


void 
initialize(char *arr, int length)
{
	int             i;
	for (i = 0; i < length; i++) {
		arr[i] = length - i;
	}
}


char            source[ARRAYSIZE];
char            target[ARRAYSIZE];

int 
main(void)
{
	initialize(source, ARRAYSIZE);
	duffcopy(source, target, INVOCATION_COUNT);
	return 0;
}


/*------------------------------------------------------------
 * $Id: duff.c,v 1.4 2005/12/21 09:43:07 jgn Exp $
 *------------------------------------------------------------
 * $Log: duff.c,v $
 * Revision 1.4  2005/12/21 09:43:07  jgn
 * Changes...
 *
 * Revision 1.3  2005/11/11 10:29:45  ael01
 * updated
 *
 * Revision 1.2  2005/04/04 11:34:58  csg
 * again
 *
 * Revision 1.1  2005/03/29 09:34:13  jgn
 * New file.
 *
 * Revision 1.8  2000/10/16 07:48:15  jakob
 * *** empty log message ***
 *
 * Revision 1.7  2000/05/22 11:02:18  jakob
 * Fixed minor errors.
 *
 * Revision 1.6  2000/02/27 16:56:52  jakob
 * *** empty log message ***
 *
 * Revision 1.5  2000/02/15 14:09:24  jakob
 * no message
 *
 * Revision 1.2  2000/02/15 13:32:16  jakob
 * Added duff's device to benchmark suite for LCTES 00 paper.
 *
 *
 *------------------------------------------------------------*/

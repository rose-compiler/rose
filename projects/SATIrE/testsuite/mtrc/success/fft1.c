/* MDH WCET BENCHMARK SUITE. File version $Id: fft1.c,v 1.1 2005/11/11 10:15:38 ael01 Exp $ */

/*************************************************************************/
/*                                                                       */
/*   SNU-RT Benchmark Suite for Worst Case Timing Analysis               */
/*   =====================================================               */
/*                              Collected and Modified by S.-S. Lim      */
/*                                           sslim@archi.snu.ac.kr       */
/*                                         Real-Time Research Group      */
/*                                        Seoul National University      */
/*                                                                       */
/*                                                                       */
/*        < Features > - restrictions for our experimental environment   */
/*                                                                       */
/*          1. Completely structured.                                    */
/*               - There are no unconditional jumps.                     */
/*               - There are no exit from loop bodies.                   */
/*                 (There are no 'break' or 'return' in loop bodies)     */
/*          2. No 'switch' statements.                                   */
/*          3. No 'do..while' statements.                                */
/*          4. Expressions are restricted.                               */
/*               - There are no multiple expressions joined by 'or',     */
/*                'and' operations.                                      */
/*          5. No library calls.                                         */
/*               - All the functions needed are implemented in the       */
/*                 source file.                                          */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*  FILE: fft1.c                                                         */
/*  SOURCE : Turbo C Programming for Engineering by Hyun Soon Ahn        */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     FFT using Cooly-Turkey algorithm.                                 */
/*     There are two inputs, ar[] and ai[]. ar[] is real number parts    */
/*     of input array and the ai[] is imaginary number parts of input.   */
/*     The function fft1 process FFT or inverse FFT according to the    .*/
/*     parameter flag. (FFT with flag=0, inverse FFT with flag=1).       */
/*                                                                       */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/

 /*
  * Changes: JG 2005/12/23: Changed type of main to int. Removed parameter to
  * log. Indented program.
  */

#define PI 3.14159
#define M_PI 3.14159

double          ar[8];
double          ai[8] = {0.,};

int             fft1(int n, int flag);

static double 
fabs(double n)
{
	double          f;

	if (n >= 0)
		f = n;
	else
		f = -n;
	return f;
}

static double 
log(void)
{
	return (4.5);
}


static double 
sin(rad)
	double          rad;
{
	double          app;

	double          diff;
	int             inc = 1;

	while (rad > 2 * PI)
		rad -= 2 * PI;
	while (rad < -2 * PI)
		rad += 2 * PI;
	app = diff = rad;
	diff = (diff * (-(rad * rad))) /
		((2.0 * inc) * (2.0 * inc + 1.0));
	app = app + diff;
	inc++;
	while (fabs(diff) >= 0.00001) {
		diff = (diff * (-(rad * rad))) /
			((2.0 * inc) * (2.0 * inc + 1.0));
		app = app + diff;
		inc++;
	}

	return (app);
}


static double 
cos(double rad)
{
	double          sin();

	return (sin(PI / 2.0 - rad));
}


int 
main(void)
{

	int             i, n = 8, flag, chkerr;


	/* ar  */
	for (i = 0; i < n; i++)
		ar[i] = cos(2 * M_PI * i / n);

	/* forward fft */
	flag = 0;
	chkerr = fft1(n, flag);

	/* inverse fft */
	flag = 1;
	chkerr = fft1(n, flag);
	return 0;

}



int 
fft1(int n, int flag)
{

	int             i, j, k, it, xp, xp2, j1, j2, iter;
	double          sign, w, wr, wi, dr1, dr2, di1, di2, tr, ti, arg;

	if (n < 2)
		return (999);
	iter = log( /* (double)n */ ) / log( /* 2.0 */ );
	j = 1;
#ifdef DEBUG
	printf("iter=%d\n", iter);
#endif
	for (i = 0; i < iter; i++)
		j *= 2;
	if (fabs(n - j) > 1.0e-6)
		return (1);

	/* Main FFT Loops  */
	sign = ((flag == 1) ? 1.0 : -1.0);
	xp2 = n;
	for (it = 0; it < iter; it++) {
		xp = xp2;
		xp2 /= 2;
		w = PI / xp2;
#ifdef DEBUG
		printf("xp2=%d\n", xp2);
#endif
		for (k = 0; k < xp2; k++) {
			arg = k * w;
			wr = cos(arg);
			wi = sign * sin(arg);
			i = k - xp;
			for (j = xp; j <= n; j += xp) {
				j1 = j + i;
				j2 = j1 + xp2;
				dr1 = ar[j1];
				dr2 = ar[j2];
				di1 = ai[j1];
				di2 = ai[j2];
				tr = dr1 - dr2;
				ti = di1 - di2;
				ar[j1] = dr1 + dr2;
				ai[j1] = di1 + di2;
				ar[j2] = tr * wr - ti * wi;
				ai[j2] = ti * wr + tr * wi;
			}
		}
	}

	/* Digit Reverse Counter  */

	j1 = n / 2;
	j2 = n - 1;
	j = 1;
#ifdef DEBUG
	printf("j2=%d\n", j2);
#endif
	for (i = 1; i <= j2; i++) {
		if (i < j) {
			tr = ar[j - 1];
			ti = ai[j - 1];
			ar[j - 1] = ar[i - 1];
			ai[j - 1] = ai[i - 1];
			ar[i - 1] = tr;
			ai[i - 1] = ti;
		}
		k = j1;
		while (k < j) {
			j -= k;
			k /= 2;
		}
		j += k;
	}
	if (flag == 0)
		return (0);
	w = n;
	for (i = 0; i < n; i++) {
		ar[i] /= w;
		ai[i] /= w;
	}
	return (0);
}

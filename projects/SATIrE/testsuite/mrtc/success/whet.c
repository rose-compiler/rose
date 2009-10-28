/*
 *      Whetstone benchmark in C.  This program is a translation of the
 *	original Algol version in "A Synthetic Benchmark" by H.J. Curnow
 *      and B.A. Wichman in Computer Journal, Vol  19 #1, February 1976.
 *
 *	Used to test compiler optimization and floating point performance.
 *
 *	Compile by:		cc -O -s -o whet whet.c
 *	or:			cc -O -DPOUT -s -o whet whet.c
 *	if output is desired.
 */

#define ITERATIONS	10 /* 1 Million Whetstone instructions */

#include "math.h"

double		x1, x2, x3, x4, x, y, z, t, t1, t2;
double 		e1[4];
int		i, j, k, l, n1, n2, n3, n4, n6, n7, n8, n9, n10, n11;

main()
{

	/* initialize constants */

	t   =   0.499975;
	t1  =   0.50025;
	t2  =   2.0;

	/* set values of module weights */

	n1  =   0 * ITERATIONS;
	n2  =  12 * ITERATIONS;
	n3  =  14 * ITERATIONS;
	n4  = 345 * ITERATIONS;
	n6  = 210 * ITERATIONS;
	n7  =  32 * ITERATIONS;
	n8  = 899 * ITERATIONS;
	n9  = 616 * ITERATIONS;
	n10 =   0 * ITERATIONS;
	n11 =  93 * ITERATIONS;

/* MODULE 1:  simple identifiers */

	x1 =  1.0;
	x2 = x3 = x4 = -1.0;

	for(i = 1; i <= n1; i += 1) {
		x1 = ( x1 + x2 + x3 - x4 ) * t;
		x2 = ( x1 + x2 - x3 - x4 ) * t;
		x3 = ( x1 - x2 + x3 + x4 ) * t;
		x4 = (-x1 + x2 + x3 + x4 ) * t;
	}
#ifdef POUT
	pout(n1, n1, n1, x1, x2, x3, x4);
#endif


/* MODULE 2:  array elements */

	e1[0] =  1.0;
	e1[1] = e1[2] = e1[3] = -1.0;

	for (i = 1; i <= n2; i +=1) {
		e1[0] = ( e1[0] + e1[1] + e1[2] - e1[3] ) * t;
		e1[1] = ( e1[0] + e1[1] - e1[2] + e1[3] ) * t;
		e1[2] = ( e1[0] - e1[1] + e1[2] + e1[3] ) * t;
		e1[3] = (-e1[0] + e1[1] + e1[2] + e1[3] ) * t;
	}
#ifdef POUT
	pout(n2, n3, n2, e1[0], e1[1], e1[2], e1[3]);
#endif

/* MODULE 3:  array as parameter */

	for (i = 1; i <= n3; i += 1)
		pa(e1);
#ifdef POUT
	pout(n3, n2, n2, e1[0], e1[1], e1[2], e1[3]);
#endif

/* MODULE 4:  conditional jumps */

	j = 1;
	for (i = 1; i <= n4; i += 1) {
		if (j == 1)
			j = 2;
		else
			j = 3;

		if (j > 2)
			j = 0;
		else
			j = 1;

		if (j < 1 )
			j = 1;
		else
			j = 0;
	}
#ifdef POUT
	pout(n4, j, j, x1, x2, x3, x4);
#endif

/* MODULE 5:  omitted */

/* MODULE 6:  integer arithmetic */

	j = 1;
	k = 2;
	l = 3;

	for (i = 1; i <= n6; i += 1) {
		j = j * (k - j) * (l -k);
		k = l * k - (l - j) * k;
		l = (l - k) * (k + j);

		e1[l - 2] = j + k + l;		/* C arrays are zero based */
		e1[k - 2] = j * k * l;
	}
#ifdef POUT
	pout(n6, j, k, e1[0], e1[1], e1[2], e1[3]);
#endif

/* MODULE 7:  trig. functions */

	x = y = 0.5;

	for(i = 1; i <= n7; i +=1) {
		x = t * atan(t2*sin(x)*cos(x)/(cos(x+y)+cos(x-y)-1.0));
		y = t * atan(t2*sin(y)*cos(y)/(cos(x+y)+cos(x-y)-1.0));
	}
#ifdef POUT
	pout(n7, j, k, x, x, y, y);
#endif

/* MODULE 8:  procedure calls */

	x = y = z = 1.0;

	for (i = 1; i <= n8; i +=1)
		p3(x, y, &z);
#ifdef POUT
	pout(n8, j, k, x, y, z, z);
#endif

/* MODULE9:  array references */

	j = 1;
	k = 2;
	l = 3;

	e1[0] = 1.0;
	e1[1] = 2.0;
	e1[2] = 3.0;

	for(i = 1; i <= n9; i += 1)
		p0();
#ifdef POUT
	pout(n9, j, k, e1[0], e1[1], e1[2], e1[3]);
#endif

/* MODULE10:  integer arithmetic */

	j = 2;
	k = 3;

	for(i = 1; i <= n10; i +=1) {
		j = j + k;
		k = j + k;
		j = k - j;
		k = k - j - j;
	}
#ifdef POUT
	pout(n10, j, k, x1, x2, x3, x4);
#endif

/* MODULE11:  standard functions */

	x = 0.75;
	for(i = 1; i <= n11; i +=1)
		x = sqrt( exp( log(x) / t1));

#ifdef POUT
	pout(n11, j, k, x, x, x, x);
#endif
}

pa(e)
double e[4];
{
	register int j;

	j = 0;
     lab:
	e[0] = (  e[0] + e[1] + e[2] - e[3] ) * t;
	e[1] = (  e[0] + e[1] - e[2] + e[3] ) * t;
	e[2] = (  e[0] - e[1] + e[2] + e[3] ) * t;
	e[3] = ( -e[0] + e[1] + e[2] + e[3] ) / t2;
	j += 1;
	if (j < 6)
		goto lab;
}


p3(x, y, z)
double x, y, *z;
{
	x  = t * (x + y);
	y  = t * (x + y);
	*z = (x + y) /t2;
}


p0()
{
	e1[j] = e1[k];
	e1[k] = e1[l];
	e1[l] = e1[j];
}

#ifdef POUT
pout(n, j, k, x1, x2, x3, x4)
int n, j, k;
double x1, x2, x3, x4;
{
	printf("%6d%6d%6d  %5e  %5e  %5e  %5e\n",
		n, j, k, x1, x2, x3, x4);
}
#endif

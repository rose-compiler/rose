/* MDH WCET BENCHMARK SUITE. File version $Id: minmax.c,v 1.1 2005/11/11 10:18:31 ael01 Exp $ */


 /*
  * Changes: JG 2005/12/23: Changed type of main to int, added prototypes.
                            Indented program.
  */

void            swap(int *a, int *b);
int             min(int a, int b, int c);
int             max(int a, int b, int c);

void 
swap(int *a, int *b)
{
	int             tmp = *a;
	*a = *b;
	*b = tmp;
}

int 
min(int a, int b, int c)
{
	int             m;
	if (a <= b) {
		if (a <= c)
			m = a;
		else
			m = c;
	} else
		m = (b <= c) ? b : c;
	return m;
}

int 
max(int a, int b, int c)
{
	if (a <= b)
		swap(&a, &b);
	if (a <= c)
		swap(&a, &c);
	return a;
}

int 
main(void)
{
	int             x = 10;
	int             y = 2;
	int             z = 1;
	if (x <= y)
		swap(&x, &y);
	else if (x <= z)
		x += min(x, y, z);
	else
		z *= max(z, y, x);
	return (y <= z ? y + z : y - z);
}

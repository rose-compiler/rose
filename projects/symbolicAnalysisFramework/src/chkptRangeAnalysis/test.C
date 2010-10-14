#include <stdio.h>
#include <stdlib.h>
#define abs(a) (a<0?0-a:a)

// GCD implementation from Wikipedia
static long gcd(long u, long v)
{
	int shift;
	
	/* GCD(0,x) := x */
	if (u == 0 || v == 0)
		return u | v;
	
	/* Let shift := lg K, where K is the greatest power of 2
	dividing both u and v. */
	for (shift = 0; ((u | v) & 1) == 0; ++shift) {
		u >>= 1;
		v >>= 1;
	}
	
	while ((u & 1) == 0)
		u >>= 1;
	
	/* From here on, u is always odd. */
	do {
		while ((v & 1) == 0)  /* Loop X */
		v >>= 1;
		
		/* Now u and v are both odd, so diff(u, v) is even.
		Let u = min(u, v), v = diff(u, v)/2. */
		if (u <= v) {
			v -= u;
		} else {
			int diff = u - v;
			u = v;
			v = diff;
		}
		v >>= 1;
	} while (v != 0);
	
	return u << shift;
}

void normalize(int a, int b, int c)
{
	int div, rem;
	div = c/a;
	rem = abs(c%a);
	printf("div=%d rem=%d\n", div, rem);
	
	printf("normalize(%d, %d, %d) ==> ", a, b, c);
	if(a==b)
	{
		c=c/a;
		a=1;
		b=1;
	}
	printf("(%d, %d, %d)\n", a, b, c);
}

void normalize2(int a, int b, int c)
{
	bool modified = false;
	printf("normalize(%d, %d, %d)\n", a, b, c);
	if(c == 0)
	{
		
		int gcd_ab = gcd(abs(a), abs(b));
		modified = (a == a/gcd_ab) &&
		           (b == b/gcd_ab);

		a = a/gcd_ab;
		b = b/gcd_ab;
		printf("        B(%d, %d, %d)\n", a, b, c);
	}
	else
	{
		/*// first, if |c| > |b| and |b|>1, then cut |b| down to be smaller than |b|
		if(abs(c) > abs(b) && abs(b)>1)
		{
			int oldB = b;
			b=b+c/b;
			c=c%oldB;
		}
		printf("        C(%d, %d, %d)\n", a, b, c);*/
		int gcd_ab = gcd(abs(a), abs(b));
		int gcd_abc = gcd(gcd_ab, abs(c));
		modified = (a == a/gcd_abc) &&
                 (b == b/gcd_abc) &&
                 (c == c/gcd_abc);
		
		a = a/gcd_abc;
		b = b/gcd_abc;
		c = c/gcd_abc;
		printf("        C(%d, %d, %d)\n", a, b, c);
	}
	
	// Now consider the fact that x and y are integers. Thus, if 
	// if a=l*m, b=l*n, c=l*p+q (q>0)
	// then [x*a <= y*b + c] implies [x*l*m <= y*l*n + l*p] == [x*m <= y*n + p]
	long gcd_ab = gcd(a,b);
	int div, rem;
	div = c/gcd_ab;
	rem = c%gcd_ab;
	//printf("gcd_ab=%d div=%d rem=%d, c-((gcd_ab+rem)%%gcd_ab)=%d\n", gcd_ab, div, rem, c-((gcd_ab+rem)%gcd_ab));
	
	//printf("normalize(%d, %d, %d) ==> ", a, b, c);
	if(gcd_ab>1)
	{
		modified = (a == a/gcd_ab) &&
                 (b == b/gcd_ab) &&
                 (c == (c-((gcd_ab+rem)%gcd_ab))/gcd_ab);
		
		a=a/gcd_ab;
		b=b/gcd_ab;
		c = (c-((gcd_ab+rem)%gcd_ab))/gcd_ab;
	}
	//printf("(%d, %d, %d)\n", a, b, c);
}

int main(int argc, char** argv)
{
	
/*	printf("13/5=%d,  13\%5=%d\n", 13/5, 13%5);
	printf("13/-5=%d, 13\%-5=%d\n", 13/-5, 13%-5);
	printf("-13/5=%d, -13\%5=%d\n", -13/5, -13%5);
	printf("-13/-5=%d, -13\%-5=%d\n", -13/-5, -13%-5);*/
	int a = atoi(argv[1]);
	int b = atoi(argv[2]);
	int c = atoi(argv[3]);
	
	//normalize( a, b, c);
	normalize2(a, b, c);
	
	return 0;
}

/* MDH WCET BENCHMARK SUITE. File version $Id: expint.c,v 1.3 2005/11/11 10:29:56 ael01 Exp $ */

/************************************************************************
 * FROM:
 *   http://sron9907.sron.nl/manual/numrecip/c/expint.c
 *
 * FEATURE:
 *   One loop depends on a loop-invariant value to determine
 *   if it run or not.
 *
 ***********************************************************************/

 /*
  * Changes: JG 2005/12/23: Changed type of main to int, added prototypes.
                            Indented program.
  */

long int        foo(long int x);
long int        expint(int n, long int x);

int 
main(void)
{
	expint(50, 1);
	/* with  expint(50,21) as argument, runs the short path */
	/* in expint.   expint(50,1)  gives the longest execution time */
	return 0;
}

long int 
foo(long int x)
{
	return x * x + (8 * x) << (4 - x);
}


/* Function with same flow, different data types,
   nonsensical calculations */
long int 
expint(int n, long int x)
{
	int             i, ii, nm1;
	long int        a, b, c, d, del, fact, h, psi, ans;

	nm1 = n - 1;		/* arg=50 --> 49 */

	if (x > 1) {		/* take this leg? */
		b = x + n;
		c = 2e6;
		d = 3e7;
		h = d;

		for (i = 1; i <= 100; i++) {	/* MAXIT is 100 */
			a = -i * (nm1 + i);
			b += 2;
			d = 10 * (a * d + b);
			c = b + a / c;
			del = c * d;
			h *= del;
			if (del < 10000) {
				ans = h * -x;
				return ans;
			}
		}
	} else {		/* or this leg? */
		/* For the current argument, will always take */
		/* '2' path here: */
		ans = nm1 != 0 ? 2 : 1000;
		fact = 1;
		for (i = 1; i <= 100; i++) {	/* MAXIT */
			fact *= -x / i;
			if (i != nm1)	/* depends on parameter n */
				del = -fact / (i - nm1);
			else {	/* this fat piece only runs ONCE *//* runs on
				 * iter 49 */
				psi = 0x00FF;
				for (ii = 1; ii <= nm1; ii++)	/* */
					psi += ii + nm1;
				del = psi + fact * foo(x);
			}
			ans += del;
			/* conditional leave removed */
		}

	}
	return ans;
}





/*  #define MAXIT 100 */
/*  #define EULER 0.5772156649 */
/*  #define FPMIN 1.0e-30 */
/*  #define EPS 1.0e-7 */
/*  float expint(int n, float x) */
/*  { */
/*    void nrerror(char error_text[]); */
/*    int i,ii,nm1; */
/*    float a,b,c,d,del,fact,h,psi,ans; */

/*    nm1=n-1; */
/*    if (n < 0 || x < 0.0 || (x==0.0 && (n==0 || n==1))) */
/*    nrerror("bad arguments in expint");                */
/*     else {  */
/*    if (n == 0)  */
/*      ans=exp(-x)/x; */
/*    else  */
/*      { */
/*        if (x == 0.0)  */
/*          ans=1.0/nm1; */
/*        else  */
/*          { */
/*            if (x > 1.0) { */
/*              b=x+n; */
/*              c=1.0/FPMIN; */
/*              d=1.0/b; */
/*              h=d; */

/*              for (i=1;i<=MAXIT;i++)  */
/*              { */
/*                a = -i*(nm1+i); */
/*                b += 2.0; */
/*                d=1.0/(a*d+b); */
/*                c=b+a/c; */
/*                del=c*d; */
/*                h *= del; */
/*                if (fabs(del-1.0) < EPS)  */
/*                  { */
/*                    ans=h*exp(-x); */
/*                    return ans; */
/*                  } */
/*              } */

/*            nrerror("continued fraction failed in expint");*/
/*          }  */
/*          else  */
/*            { */
/*              ans = (nm1!=0 ? 1.0/nm1 : -log(x)-EULER); */
/*            fact=1.0; */
/*            for (i=1;i<=MAXIT;i++) { */
/*              fact *= -x/i; */
/*              if (i != nm1) del = -fact/(i-nm1); */
/*              else { */
/*                psi = -EULER; */
/*                for (ii=1;ii<=nm1;ii++) psi += 1.0/ii; */
/*                del=fact*(-log(x)+psi); */
/*              } */
/*              ans += del; */
/*              if (fabs(del) < fabs(ans)*EPS) return ans; */
/*            } */
/*            nrerror("series failed in expint"); */
/*          } */
/*        } */
/*      } */
/*    } */
/*    return ans; */
/*  } */

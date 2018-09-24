/* static void foobar(int x1,int x2,int x3,int ss,int c,double f,long *larray, int(*pf)()) */

void foobar(x1,x2,x3, ss, c, f, larray, pf)
     /* order should not matter, no argument declaration defaults to int */
	int (*pf)();
	long *larray;
	double f;
   {
     long l;

  /* This unparses to x2[l] instaed of larray[l] */
     larray[l];

     *larray++;

     (*pf)();
   }

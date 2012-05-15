
	#define S1(zT0,zT1,zT2,zT3,i,j)	x1[i]=x1[i]+a[i][j]*y_1[j];
	#define S2(zT0,zT1,zT2,zT3,i,j)	x2[i]=x2[i]+a[j][i]*y_2[j];

		int t0, t1, t2, t3, t4, t5;

	register int lb, ub, lb1, ub1, lb2, ub2;
	register int lbv, ubv;

/* Generated from PLUTO-produced CLooG file by CLooG v0.14.1 64 bits in 0.03s. */
	lb1=0;
	ub1=floord(N-1,256);
#pragma omp parallel for shared(lb1,ub1) private(t0,t1,t2,t3,t4,t5)
	for (t0=lb1; t0<=ub1; t0++) {
  for (t1=0;t1<=floord(N-1,256);t1++) {
    for (t2=max(8*t0,0);t2<=min(8*t0+7,floord(N-1,32));t2++) {
      for (t3=max(8*t1,0);t3<=min(8*t1+7,floord(N-1,32));t3++) {
        for (t4=max(32*t3,0);t4<=min(N-1,32*t3+31);t4++) {
{
	lbv=max(32*t2,0); 	ubv=min(N-1,32*t2+31);
#pragma ivdep
#pragma vector always
	for (t5=lbv; t5<=ubv; t5++) {
            S1(t0,t1,t2,t3,t5,t4) ;
            S2(t0,t1,t2,t3,t5,t4) ;
          }
}
        }
      }
    }
  }
}
/* End of CLooG code */

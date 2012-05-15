
	#define S1(zT0,zT1,zT2,t,i,j)	a[i][j]=(a[i-1][j-1]+a[i-1][j]+a[i-1][j+1]+a[i][j-1]+a[i][j]+a[i][j+1]+a[i+1][j-1]+a[i+1][j]+a[i+1][j+1])/9.0;

		int t0, t1, t2, t3, t4, t5;

	register int lb, ub, lb1, ub1, lb2, ub2;
	register int lbv, ubv;

/* Generated from PLUTO-produced CLooG file by CLooG v0.14.1 64 bits in 0.01s. */
if (N >= 3) {
  for (t0=-1;t0<=floord(5*T+2*N-9,30);t0++) {
	lb1=max(max(ceild(10*t0-13,25),0),ceild(10*t0-T+1,10));
	ub1=min(min(floord(10*t0+9,10),floord(T+N-3,15)),floord(10*t0+N+7,25));
#pragma omp parallel for shared(t0,lb1,ub1) private(t1,t2,t3,t4,t5)
	for (t1=lb1; t1<=ub1; t1++) {
      for (t2=max(max(max(max(0,ceild(15*t1-13,15)),ceild(30*t1-N-11,15)),ceild(10*t0+5*t1-13,15)),ceild(20*t0-20*t1-12,15));t2<=min(min(min(min(floord(10*t0+5*t1+N+21,15),floord(2*T+2*N-6,15)),floord(30*t1+N+25,15)),floord(20*t0-20*t1+2*N+14,15)),floord(15*t1+T+N+11,15));t2++) {
        for (t3=max(max(max(max(0,ceild(15*t2-2*N+4,2)),-15*t1+15*t2-N-12),10*t0-10*t1),15*t1-N+2);t3<=min(min(min(min(T-1,10*t0-10*t1+9),15*t1+13),floord(15*t2+12,2)),-15*t1+15*t2+13);t3++) {
          for (t4=max(max(t3+1,15*t1),15*t2-t3-N+2);t4<=min(min(t3+N-2,15*t2-t3+13),15*t1+14);t4++) {
            for (t5=max(t3+t4+1,15*t2);t5<=min(t3+t4+N-2,15*t2+14);t5++) {
              S1(t0-t1,t1,t2,t3,-t3+t4,-t3-t4+t5) ;
            }
          }
        }
      }
    }
  }
}
/* End of CLooG code */

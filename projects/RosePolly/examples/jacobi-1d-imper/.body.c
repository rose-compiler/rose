
	#define S1(zT0,zT1,t,i)	b[i]=0.33333*(a[i-1]+a[i]+a[i+1]);
	#define S2(zT0,zT1,t,j)	a[j]=b[j];

		int t0, t1, t2, t3, t4;

	register int lb, ub, lb1, ub1, lb2, ub2;
	register int lbv, ubv;

/* Generated from PLUTO-produced CLooG file by CLooG v0.14.1 64 bits in 0.04s. */
for (t0=-1;t0<=floord(3*T+N-4,1024);t0++) {
	lb1=max(max(ceild(2048*t0-1021,3072),ceild(1024*t0-T+1,1024)),0);
	ub1=min(min(floord(1024*t0+1023,1024),floord(2*T+N-3,1024)),floord(2048*t0+N+2045,3072));
#pragma omp parallel for shared(t0,lb1,ub1) private(t1,t2,t3,t4)
	for (t1=lb1; t1<=ub1; t1++) {
    if ((t0 <= floord(3072*t1-N+1,2048)) && (t1 >= ceild(N-1,1024))) {
      if ((-N+1)%2 == 0) {
        if ((-N+1)%2 == 0) {
          S2(t0-t1,t1,(1024*t1-N+1)/2,N-2) ;
        }
      }
    }
    for (t2=max(max(ceild(1024*t1-N+2,2),1024*t0-1024*t1),0);t2<=min(min(min(512*t1-2,T-1),1024*t0-1024*t1+1023),floord(1024*t1-N+1024,2));t2++) {
      for (t3=1024*t1;t3<=2*t2+N-2;t3++) {
        S1(t0-t1,t1,t2,-2*t2+t3) ;
        S2(t0-t1,t1,t2,-2*t2+t3-1) ;
      }
      S2(t0-t1,t1,t2,N-2) ;
    }
    for (t2=max(max(512*t1-1,1024*t0-1024*t1),0);t2<=min(min(T-1,1024*t0-1024*t1+1023),floord(1024*t1-N+1024,2));t2++) {
      S1(t0-t1,t1,t2,2) ;
      for (t3=2*t2+3;t3<=2*t2+N-2;t3++) {
        S1(t0-t1,t1,t2,-2*t2+t3) ;
        S2(t0-t1,t1,t2,-2*t2+t3-1) ;
      }
      S2(t0-t1,t1,t2,N-2) ;
    }
    for (t2=max(max(1024*t0-1024*t1,0),ceild(1024*t1-N+1025,2));t2<=min(min(T-1,1024*t0-1024*t1+1023),512*t1-2);t2++) {
      for (t3=1024*t1;t3<=1024*t1+1023;t3++) {
        S1(t0-t1,t1,t2,-2*t2+t3) ;
        S2(t0-t1,t1,t2,-2*t2+t3-1) ;
      }
    }
    for (t2=max(max(max(512*t1-1,1024*t0-1024*t1),0),ceild(1024*t1-N+1025,2));t2<=min(min(T-1,512*t1+510),1024*t0-1024*t1+1023);t2++) {
      S1(t0-t1,t1,t2,2) ;
      for (t3=2*t2+3;t3<=1024*t1+1023;t3++) {
        S1(t0-t1,t1,t2,-2*t2+t3) ;
        S2(t0-t1,t1,t2,-2*t2+t3-1) ;
      }
    }
  }
}
/* End of CLooG code */

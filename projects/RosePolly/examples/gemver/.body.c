
	#define S1(zT0,zT1,zT2,zT3,i,j)	B[i][j]=A[i][j]+u1[i]*v1[j]+u2[i]*v2[j];
	#define S2(zT0,zT1,zT2,zT3,i,j)	x[i]=x[i]+beta*B[j][i]*y[j];
	#define S3(zT0,zT1,zT2,zT3,i)	x[i]=x[i]+z[i];
	#define S4(zT0,zT1,zT2,zT3,i,j)	w[i]=w[i]+alpha*B[i][j]*x[j];

		int t0, t1, t2, t3, t4, t5, t6, t7;

	register int lb, ub, lb1, ub1, lb2, ub2;
	register int lbv, ubv;

/* Generated from PLUTO-produced CLooG file by CLooG v0.14.1 64 bits in 0.03s. */
	lb1=0;
	ub1=floord(N-1,8000);
#pragma omp parallel for shared(t0,lb1,ub1) private(t1,t2,t3,t4,t5,t6,t7)
	for (t1=lb1; t1<=ub1; t1++) {
  for (t2=0;t2<=floord(N-1,256);t2++) {
    for (t3=max(20*t1,0);t3<=min(20*t1+19,floord(N-1,400));t3++) {
      for (t4=max(0,16*t2);t4<=min(16*t2+15,floord(N-1,16));t4++) {
        for (t5=max(0,16*t4);t5<=min(N-1,16*t4+15);t5++) {
{
	lbv=max(0,400*t3); 	ubv=min(N-1,400*t3+399);
#pragma ivdep
#pragma vector always
	for (t6=lbv; t6<=ubv; t6++) {
            S1(t1,t2,t3,t4,t5,t6) ;
            S2(t1,t2,t3,t4,t6,t5) ;
          }
}
        }
      }
    }
  }
}
	lb1=0;
	ub1=floord(N-1,8000);
#pragma omp parallel for shared(t0,lb1,ub1) private(t1,t2,t3,t4,t5,t6,t7)
	for (t1=lb1; t1<=ub1; t1++) {
  for (t3=max(20*t1,0);t3<=min(20*t1+19,floord(N-1,400));t3++) {
    for (t4=max(0,0);t4<=min(0,15);t4++) {
{
	lbv=max(0,400*t3); 	ubv=min(N-1,400*t3+399);
#pragma ivdep
#pragma vector always
	for (t6=lbv; t6<=ubv; t6++) {
        S3(t1,0,t3,t4,t6) ;
      }
}
    }
  }
}
	lb1=0;
	ub1=floord(N-1,8000);
#pragma omp parallel for shared(t0,lb1,ub1) private(t1,t2,t3,t4,t5,t6,t7)
	for (t1=lb1; t1<=ub1; t1++) {
  for (t2=0;t2<=floord(N-1,256);t2++) {
    for (t3=max(20*t1,0);t3<=min(20*t1+19,floord(N-1,400));t3++) {
      for (t4=max(16*t2,0);t4<=min(floord(N-1,16),16*t2+15);t4++) {
        for (t5=max(0,16*t4);t5<=min(N-1,16*t4+15);t5++) {
{
	lbv=max(0,400*t3); 	ubv=min(N-1,400*t3+399);
#pragma ivdep
#pragma vector always
	for (t6=lbv; t6<=ubv; t6++) {
            S4(t1,t2,t3,t4,t6,t5) ;
          }
}
        }
      }
    }
  }
}
/* End of CLooG code */

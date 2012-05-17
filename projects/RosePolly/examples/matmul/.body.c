
	#define S1(zT0,zT1,zT2,zT3,zT4,zT5,i,j,k)	C[i][j]=beta*C[i][j]+alpha*A[i][k]*B[k][j];

		int t0, t1, t2, t3, t4, t5, t6, t6t, newlb_t6, newub_t6, t7, t7t, newlb_t7, newub_t7, t8;

	register int lb, ub, lb1, ub1, lb2, ub2;
	register int lbv, ubv;

/* Generated from PLUTO-produced CLooG file by CLooG v0.14.1 64 bits in 2.79s. */
	lb1=0;
	ub1=floord(M-1,128);
#pragma omp parallel for shared(lb1,ub1) private(t0,t1,t2,t3,t4,t5,t6,t7,t8)
	for (t0=lb1; t0<=ub1; t0++) {
  for (t1=0;t1<=floord(N-1,256);t1++) {
    for (t2=0;t2<=floord(K-1,128);t2++) {
      for (t3=max(0,16*t0);t3<=min(16*t0+15,floord(M-1,8));t3++) {
        for (t4=max(0,2*t1);t4<=min(2*t1+1,floord(N-1,128));t4++) {
          for (t5=max(16*t2,0);t5<=min(16*t2+15,floord(K-1,8));t5++) {
/*@ begin Loop(
	transform RegTile(loops=['t6','t7'], ufactors=[8,8])
            for (t6=max(8*t3,0);t6<=min(M-1,8*t3+7);t6++) 
              for (t7=max(8*t5,0);t7<=min(K-1,8*t5+7);t7++) 
{
{
	lbv=max(128*t4,0); 	ubv=min(N-1,128*t4+127);
#pragma ivdep
#pragma vector always
	for (t8=lbv; t8<=ubv; t8++) {
                  S1(t0,t1,t2,t3,t4,t5,t6,t8,t7) ;
                }
}
}
) @*/{
  for (t6t=max(8*t3,0); t6t<=min(M-1,8*t3+7)-7; t6t=t6t+8) {
    for (t7t=max(8*t5,0); t7t<=min(K-1,8*t5+7)-7; t7t=t7t+8) {
{
	lbv=max(128*t4,0); 	ubv=min(N-1,128*t4+127);
#pragma ivdep
#pragma vector always
	for (t8=lbv; t8<=ubv; t8++) {
        S1(t0,t1,t2,t3,t4,t5,t6t,t8,t7t);
        S1(t0,t1,t2,t3,t4,t5,t6t,t8,(t7t+1));
        S1(t0,t1,t2,t3,t4,t5,t6t,t8,(t7t+2));
        S1(t0,t1,t2,t3,t4,t5,t6t,t8,(t7t+3));
        S1(t0,t1,t2,t3,t4,t5,t6t,t8,(t7t+4));
        S1(t0,t1,t2,t3,t4,t5,t6t,t8,(t7t+5));
        S1(t0,t1,t2,t3,t4,t5,t6t,t8,(t7t+6));
        S1(t0,t1,t2,t3,t4,t5,t6t,t8,(t7t+7));
        S1(t0,t1,t2,t3,t4,t5,(t6t+1),t8,t7t);
        S1(t0,t1,t2,t3,t4,t5,(t6t+1),t8,(t7t+1));
        S1(t0,t1,t2,t3,t4,t5,(t6t+1),t8,(t7t+2));
        S1(t0,t1,t2,t3,t4,t5,(t6t+1),t8,(t7t+3));
        S1(t0,t1,t2,t3,t4,t5,(t6t+1),t8,(t7t+4));
        S1(t0,t1,t2,t3,t4,t5,(t6t+1),t8,(t7t+5));
        S1(t0,t1,t2,t3,t4,t5,(t6t+1),t8,(t7t+6));
        S1(t0,t1,t2,t3,t4,t5,(t6t+1),t8,(t7t+7));
        S1(t0,t1,t2,t3,t4,t5,(t6t+2),t8,t7t);
        S1(t0,t1,t2,t3,t4,t5,(t6t+2),t8,(t7t+1));
        S1(t0,t1,t2,t3,t4,t5,(t6t+2),t8,(t7t+2));
        S1(t0,t1,t2,t3,t4,t5,(t6t+2),t8,(t7t+3));
        S1(t0,t1,t2,t3,t4,t5,(t6t+2),t8,(t7t+4));
        S1(t0,t1,t2,t3,t4,t5,(t6t+2),t8,(t7t+5));
        S1(t0,t1,t2,t3,t4,t5,(t6t+2),t8,(t7t+6));
        S1(t0,t1,t2,t3,t4,t5,(t6t+2),t8,(t7t+7));
        S1(t0,t1,t2,t3,t4,t5,(t6t+3),t8,t7t);
        S1(t0,t1,t2,t3,t4,t5,(t6t+3),t8,(t7t+1));
        S1(t0,t1,t2,t3,t4,t5,(t6t+3),t8,(t7t+2));
        S1(t0,t1,t2,t3,t4,t5,(t6t+3),t8,(t7t+3));
        S1(t0,t1,t2,t3,t4,t5,(t6t+3),t8,(t7t+4));
        S1(t0,t1,t2,t3,t4,t5,(t6t+3),t8,(t7t+5));
        S1(t0,t1,t2,t3,t4,t5,(t6t+3),t8,(t7t+6));
        S1(t0,t1,t2,t3,t4,t5,(t6t+3),t8,(t7t+7));
        S1(t0,t1,t2,t3,t4,t5,(t6t+4),t8,t7t);
        S1(t0,t1,t2,t3,t4,t5,(t6t+4),t8,(t7t+1));
        S1(t0,t1,t2,t3,t4,t5,(t6t+4),t8,(t7t+2));
        S1(t0,t1,t2,t3,t4,t5,(t6t+4),t8,(t7t+3));
        S1(t0,t1,t2,t3,t4,t5,(t6t+4),t8,(t7t+4));
        S1(t0,t1,t2,t3,t4,t5,(t6t+4),t8,(t7t+5));
        S1(t0,t1,t2,t3,t4,t5,(t6t+4),t8,(t7t+6));
        S1(t0,t1,t2,t3,t4,t5,(t6t+4),t8,(t7t+7));
        S1(t0,t1,t2,t3,t4,t5,(t6t+5),t8,t7t);
        S1(t0,t1,t2,t3,t4,t5,(t6t+5),t8,(t7t+1));
        S1(t0,t1,t2,t3,t4,t5,(t6t+5),t8,(t7t+2));
        S1(t0,t1,t2,t3,t4,t5,(t6t+5),t8,(t7t+3));
        S1(t0,t1,t2,t3,t4,t5,(t6t+5),t8,(t7t+4));
        S1(t0,t1,t2,t3,t4,t5,(t6t+5),t8,(t7t+5));
        S1(t0,t1,t2,t3,t4,t5,(t6t+5),t8,(t7t+6));
        S1(t0,t1,t2,t3,t4,t5,(t6t+5),t8,(t7t+7));
        S1(t0,t1,t2,t3,t4,t5,(t6t+6),t8,t7t);
        S1(t0,t1,t2,t3,t4,t5,(t6t+6),t8,(t7t+1));
        S1(t0,t1,t2,t3,t4,t5,(t6t+6),t8,(t7t+2));
        S1(t0,t1,t2,t3,t4,t5,(t6t+6),t8,(t7t+3));
        S1(t0,t1,t2,t3,t4,t5,(t6t+6),t8,(t7t+4));
        S1(t0,t1,t2,t3,t4,t5,(t6t+6),t8,(t7t+5));
        S1(t0,t1,t2,t3,t4,t5,(t6t+6),t8,(t7t+6));
        S1(t0,t1,t2,t3,t4,t5,(t6t+6),t8,(t7t+7));
        S1(t0,t1,t2,t3,t4,t5,(t6t+7),t8,t7t);
        S1(t0,t1,t2,t3,t4,t5,(t6t+7),t8,(t7t+1));
        S1(t0,t1,t2,t3,t4,t5,(t6t+7),t8,(t7t+2));
        S1(t0,t1,t2,t3,t4,t5,(t6t+7),t8,(t7t+3));
        S1(t0,t1,t2,t3,t4,t5,(t6t+7),t8,(t7t+4));
        S1(t0,t1,t2,t3,t4,t5,(t6t+7),t8,(t7t+5));
        S1(t0,t1,t2,t3,t4,t5,(t6t+7),t8,(t7t+6));
        S1(t0,t1,t2,t3,t4,t5,(t6t+7),t8,(t7t+7));
      }
}
    }
    for (t7=t7t; t7<=min(K-1,8*t5+7); t7=t7+1) {
{
	lbv=max(128*t4,0); 	ubv=min(N-1,128*t4+127);
#pragma ivdep
#pragma vector always
	for (t8=lbv; t8<=ubv; t8++) {
        S1(t0,t1,t2,t3,t4,t5,t6t,t8,t7);
        S1(t0,t1,t2,t3,t4,t5,(t6t+1),t8,t7);
        S1(t0,t1,t2,t3,t4,t5,(t6t+2),t8,t7);
        S1(t0,t1,t2,t3,t4,t5,(t6t+3),t8,t7);
        S1(t0,t1,t2,t3,t4,t5,(t6t+4),t8,t7);
        S1(t0,t1,t2,t3,t4,t5,(t6t+5),t8,t7);
        S1(t0,t1,t2,t3,t4,t5,(t6t+6),t8,t7);
        S1(t0,t1,t2,t3,t4,t5,(t6t+7),t8,t7);
      }
}
    }
  }
  for (t6=t6t; t6<=min(M-1,8*t3+7); t6=t6+1) {
    for (t7t=max(8*t5,0); t7t<=min(K-1,8*t5+7)-7; t7t=t7t+8) {
{
	lbv=max(128*t4,0); 	ubv=min(N-1,128*t4+127);
#pragma ivdep
#pragma vector always
	for (t8=lbv; t8<=ubv; t8++) {
        S1(t0,t1,t2,t3,t4,t5,t6,t8,t7t);
        S1(t0,t1,t2,t3,t4,t5,t6,t8,(t7t+1));
        S1(t0,t1,t2,t3,t4,t5,t6,t8,(t7t+2));
        S1(t0,t1,t2,t3,t4,t5,t6,t8,(t7t+3));
        S1(t0,t1,t2,t3,t4,t5,t6,t8,(t7t+4));
        S1(t0,t1,t2,t3,t4,t5,t6,t8,(t7t+5));
        S1(t0,t1,t2,t3,t4,t5,t6,t8,(t7t+6));
        S1(t0,t1,t2,t3,t4,t5,t6,t8,(t7t+7));
      }
}
    }
    for (t7=t7t; t7<=min(K-1,8*t5+7); t7=t7+1) {
{
	lbv=max(128*t4,0); 	ubv=min(N-1,128*t4+127);
#pragma ivdep
#pragma vector always
	for (t8=lbv; t8<=ubv; t8++) {
        S1(t0,t1,t2,t3,t4,t5,t6,t8,t7);
      }
}
    }
  }
}
/*@ end @*/
          }
        }
      }
    }
  }
}
/* End of CLooG code */

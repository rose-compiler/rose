	#define S1(zT0,zT1,zT2,zT3,zT4,zT5,k,j)	a[k][j]=a[k][j]/a[k][k];
	#define S2(zT0,zT1,zT2,zT3,zT4,zT5,k,i,j)	a[i][j]=a[i][j]-a[i][k]*a[k][j];

		int t0, t1, t2, t3, t4, t5, t6, t7, t8;

	register int lbv, ubv;

/* Generated from PLUTO-produced CLooG file by CLooG v0.14.1 64 bits in 5.26s. */
for (t0=0;t0<=floord(N-2,256);t0++) {
  for (t1=max(0,ceild(128*t0-127,128));t1<=floord(N-1,256);t1++) {
    for (t2=max(ceild(256*t0-255,256),0);t2<=floord(N-1,256);t2++) {
      for (t3=max(16*t0,0);t3<=min(min(min(floord(128*t1+127,8),floord(256*t2+255,16)),floord(N-2,16)),16*t0+15);t3++) {
        for (t4=max(max(0,2*t1),ceild(8*t3-63,64));t4<=min(floord(N-1,128),2*t1+1);t4++) {
          for (t5=max(max(16*t2,0),ceild(16*t3-15,16));t5<=min(floord(N-1,16),16*t2+15);t5++) {
            for (t6=max(0,16*t3);t6<=min(min(128*t4+126,16*t3+15),16*t5-1);t6++) {
              for (t7=16*t5;t7<=min(N-1,16*t5+15);t7++) {
{
	lbv=max(t6+1,128*t4); 	ubv=min(128*t4+127,N-1);
#pragma ivdep
#pragma vector always
	for (t8=lbv; t8<=ubv; t8++) {
                  S2(t0,t1,t2,t3,t4,t5,t6,t7,t8) ;
                }
}
              }
            }
            for (t6=max(max(16*t3,0),16*t5);t6<=min(min(min(N-2,16*t5+14),128*t4+126),16*t3+15);t6++) {
{
	lbv=max(t6+1,128*t4); 	ubv=min(128*t4+127,N-1);
#pragma ivdep
#pragma vector always
	for (t8=lbv; t8<=ubv; t8++) {
                S1(t0,t1,t2,t3,t4,t5,t6,t8) ;
              }
}
              for (t7=t6+1;t7<=min(16*t5+15,N-1);t7++) {
{
	lbv=max(t6+1,128*t4); 	ubv=min(128*t4+127,N-1);
#pragma ivdep
#pragma vector always
	for (t8=lbv; t8<=ubv; t8++) {
                  S2(t0,t1,t2,t3,t4,t5,t6,t7,t8) ;
                }
}
              }
            }
            if ((t3 >= t5) && (t4 >= ceild(16*t5-111,128)) && (t5 <= floord(N-17,16))) {
{
	lbv=max(128*t4,16*t5+16); 	ubv=min(N-1,128*t4+127);
#pragma ivdep
#pragma vector always
	for (t8=lbv; t8<=ubv; t8++) {
                S1(t0,t1,t2,t3,t4,t5,16*t5+15,t8) ;
              }
}
            }
          }
        }
      }
    }
  }
}
/* End of CLooG code */


	#define S1(zT0,zT1,zT2,t,i,j)	b[i][j]=0.2*(a[i][j]+a[i][j-1]+a[i][1+j]+a[1+i][j]+a[i-1][j]);
	#define S2(zT0,zT1,zT2,t,i,j)	a[i][j]=b[i][j];

		int t0, t1, t2, t3, t4, t5;

	register int lb, ub, lb1, ub1, lb2, ub2;
	register int lbv, ubv;

/* Generated from PLUTO-produced CLooG file by CLooG v0.14.1 64 bits in 0.47s. */
for (t0=-1;t0<=floord(3*T+N-4,32);t0++) {
	lb1=max(max(ceild(64*t0-29,96),ceild(32*t0-T+1,32)),0);
	ub1=min(min(floord(32*t0+31,32),floord(64*t0+N+61,96)),floord(2*T+N-3,32));
#pragma omp parallel for shared(t0,lb1,ub1) private(t1,t2,t3,t4,t5)
	for (t1=lb1; t1<=ub1; t1++) {
    for (t2=max(max(ceild(64*t0-64*t1-29,32),ceild(32*t1-N-27,32)),0);t2<=min(min(floord(32*t1+N+27,32),floord(2*T+N-3,32)),floord(64*t0-64*t1+N+61,32));t2++) {
      if ((t0 <= floord(96*t1-N+1,64)) && (t1 >= max(t2,ceild(N-1,32)))) {
        if ((-N+1)%2 == 0) {
          for (t5=max(32*t2,32*t1-N+4);t5<=min(32*t1,32*t2+31);t5++) {
            if ((-N+1)%2 == 0) {
              S2(t0-t1,t1,t2,(32*t1-N+1)/2,N-2,-32*t1+t5+N-2) ;
            }
          }
        }
      }
      if ((t0 <= floord(64*t1+32*t2-N+1,64)) && (t1 <= floord(32*t2-1,32)) && (t2 >= ceild(N-1,32))) {
        if ((-N+1)%2 == 0) {
          for (t4=max(32*t2-N+4,32*t1);t4<=min(32*t1+31,32*t2);t4++) {
            if ((-N+1)%2 == 0) {
              S2(t0-t1,t1,t2,(32*t2-N+1)/2,-32*t2+t4+N-2,N-2) ;
            }
          }
        }
      }
      for (t3=max(max(max(0,ceild(32*t1-N+2,2)),ceild(32*t2-N+2,2)),32*t0-32*t1);t3<=min(min(min(floord(32*t2-N+32,2),32*t0-32*t1+31),T-1),floord(32*t1-N+32,2));t3++) {
        for (t4=32*t1;t4<=2*t3+N-2;t4++) {
          for (t5=32*t2;t5<=2*t3+N-2;t5++) {
            S1(t0-t1,t1,t2,t3,-2*t3+t4,-2*t3+t5) ;
            S2(t0-t1,t1,t2,t3,-2*t3+t4-1,-2*t3+t5-1) ;
          }
          S2(t0-t1,t1,t2,t3,-2*t3+t4-1,N-2) ;
        }
        for (t5=32*t2;t5<=2*t3+N-1;t5++) {
          S2(t0-t1,t1,t2,t3,N-2,-2*t3+t5-1) ;
        }
      }
      for (t3=max(max(max(32*t0-32*t1,ceild(32*t2-N+2,2)),0),ceild(32*t1-N+33,2));t3<=min(min(min(T-1,32*t0-32*t1+31),16*t1-2),floord(32*t2-N+32,2));t3++) {
        for (t4=32*t1;t4<=32*t1+31;t4++) {
          for (t5=32*t2;t5<=2*t3+N-2;t5++) {
            S1(t0-t1,t1,t2,t3,-2*t3+t4,-2*t3+t5) ;
            S2(t0-t1,t1,t2,t3,-2*t3+t4-1,-2*t3+t5-1) ;
          }
          S2(t0-t1,t1,t2,t3,-2*t3+t4-1,N-2) ;
        }
      }
      for (t3=max(max(max(ceild(32*t2-N+33,2),32*t0-32*t1),0),ceild(32*t1-N+2,2));t3<=min(min(min(16*t2-2,32*t0-32*t1+31),T-1),floord(32*t1-N+32,2));t3++) {
        for (t4=32*t1;t4<=2*t3+N-2;t4++) {
          for (t5=32*t2;t5<=32*t2+31;t5++) {
            S1(t0-t1,t1,t2,t3,-2*t3+t4,-2*t3+t5) ;
            S2(t0-t1,t1,t2,t3,-2*t3+t4-1,-2*t3+t5-1) ;
          }
        }
        for (t5=32*t2;t5<=32*t2+31;t5++) {
          S2(t0-t1,t1,t2,t3,N-2,-2*t3+t5-1) ;
        }
      }
      for (t3=max(max(max(16*t1-1,32*t0-32*t1),ceild(32*t2-N+2,2)),0);t3<=min(min(min(T-1,16*t1+14),32*t0-32*t1+31),floord(32*t2-N+32,2));t3++) {
        for (t5=32*t2;t5<=2*t3+N-2;t5++) {
          S1(t0-t1,t1,t2,t3,2,-2*t3+t5) ;
        }
        for (t4=2*t3+3;t4<=32*t1+31;t4++) {
          for (t5=32*t2;t5<=2*t3+N-2;t5++) {
            S1(t0-t1,t1,t2,t3,-2*t3+t4,-2*t3+t5) ;
            S2(t0-t1,t1,t2,t3,-2*t3+t4-1,-2*t3+t5-1) ;
          }
          S2(t0-t1,t1,t2,t3,-2*t3+t4-1,N-2) ;
        }
      }
      for (t3=max(max(max(16*t2-1,0),ceild(32*t1-N+2,2)),32*t0-32*t1);t3<=min(min(min(32*t0-32*t1+31,16*t2+14),T-1),floord(32*t1-N+32,2));t3++) {
        for (t4=32*t1;t4<=2*t3+N-2;t4++) {
          S1(t0-t1,t1,t2,t3,-2*t3+t4,2) ;
          for (t5=2*t3+3;t5<=32*t2+31;t5++) {
            S1(t0-t1,t1,t2,t3,-2*t3+t4,-2*t3+t5) ;
            S2(t0-t1,t1,t2,t3,-2*t3+t4-1,-2*t3+t5-1) ;
          }
        }
        for (t5=2*t3+3;t5<=32*t2+31;t5++) {
          S2(t0-t1,t1,t2,t3,N-2,-2*t3+t5-1) ;
        }
      }
      for (t3=max(max(max(32*t0-32*t1,0),ceild(32*t2-N+33,2)),ceild(32*t1-N+33,2));t3<=min(min(min(T-1,32*t0-32*t1+31),16*t1-2),16*t2-2);t3++) {
        for (t4=32*t1;t4<=32*t1+31;t4++) {
          for (t5=32*t2;t5<=32*t2+31;t5++) {
            S1(t0-t1,t1,t2,t3,-2*t3+t4,-2*t3+t5) ;
            S2(t0-t1,t1,t2,t3,-2*t3+t4-1,-2*t3+t5-1) ;
          }
        }
      }
      for (t3=max(max(max(16*t1-1,ceild(32*t2-N+33,2)),32*t0-32*t1),0);t3<=min(min(min(T-1,16*t1+14),32*t0-32*t1+31),16*t2-2);t3++) {
        for (t5=32*t2;t5<=32*t2+31;t5++) {
          S1(t0-t1,t1,t2,t3,2,-2*t3+t5) ;
        }
        for (t4=2*t3+3;t4<=32*t1+31;t4++) {
          for (t5=32*t2;t5<=32*t2+31;t5++) {
            S1(t0-t1,t1,t2,t3,-2*t3+t4,-2*t3+t5) ;
            S2(t0-t1,t1,t2,t3,-2*t3+t4-1,-2*t3+t5-1) ;
          }
        }
      }
      for (t3=max(max(max(32*t0-32*t1,0),16*t2-1),ceild(32*t1-N+33,2));t3<=min(min(min(T-1,32*t0-32*t1+31),16*t2+14),16*t1-2);t3++) {
        for (t4=32*t1;t4<=32*t1+31;t4++) {
          S1(t0-t1,t1,t2,t3,-2*t3+t4,2) ;
          for (t5=2*t3+3;t5<=32*t2+31;t5++) {
            S1(t0-t1,t1,t2,t3,-2*t3+t4,-2*t3+t5) ;
            S2(t0-t1,t1,t2,t3,-2*t3+t4-1,-2*t3+t5-1) ;
          }
        }
      }
      for (t3=max(max(max(16*t1-1,32*t0-32*t1),0),16*t2-1);t3<=min(min(min(T-1,16*t1+14),32*t0-32*t1+31),16*t2+14);t3++) {
        for (t5=2*t3+2;t5<=32*t2+31;t5++) {
          S1(t0-t1,t1,t2,t3,2,-2*t3+t5) ;
        }
        for (t4=2*t3+3;t4<=32*t1+31;t4++) {
          S1(t0-t1,t1,t2,t3,-2*t3+t4,2) ;
          for (t5=2*t3+3;t5<=32*t2+31;t5++) {
            S1(t0-t1,t1,t2,t3,-2*t3+t4,-2*t3+t5) ;
            S2(t0-t1,t1,t2,t3,-2*t3+t4-1,-2*t3+t5-1) ;
          }
        }
      }
    }
  }
}
/* End of CLooG code */

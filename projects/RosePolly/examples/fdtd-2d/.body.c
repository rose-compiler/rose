	#define S1(t,j)	ey[0][j]=t;
	#define S2(t,i,j)	ey[i][j]=ey[i][j]-0.5*(hz[i][j]-hz[i-1][j]);
	#define S3(t,i,j)	ex[i][j]=ex[i][j]-0.5*(hz[i][j]-hz[i][j-1]);
	#define S4(t,i,j)	hz[i][j]=hz[i][j]-0.7*(ex[i][j+1]-ex[i][j]+ey[i+1][j]-ey[i][j]);

		int t0, t1, t2;

/* Generated from PLUTO-produced CLooG file by CLooG v0.14.1 64 bits in 0.02s. */
if ((ny >= 2) && (nx >= 2)) {
  for (t0=0;t0<=tmax-1;t0++) {
    S1(t0,0) ;
    for (t2=t0+1;t2<=t0+nx-1;t2++) {
      S2(t0,-t0+t2,0) ;
    }
    for (t1=t0+1;t1<=t0+ny-1;t1++) {
      S3(t0,0,-t0+t1) ;
      S1(t0,-t0+t1) ;
      for (t2=t0+1;t2<=t0+nx-1;t2++) {
        S4(t0,-t0+t2-1,-t0+t1-1) ;
        S2(t0,-t0+t2,-t0+t1) ;
        S3(t0,-t0+t2,-t0+t1) ;
      }
      S4(t0,nx-1,-t0+t1-1) ;
    }
    for (t2=t0+1;t2<=t0+nx;t2++) {
      S4(t0,-t0+t2-1,ny-1) ;
    }
  }
}
if ((ny == 1) && (nx >= 2)) {
  for (t0=0;t0<=tmax-1;t0++) {
    S1(t0,0) ;
    for (t2=t0+1;t2<=t0+nx-1;t2++) {
      S2(t0,-t0+t2,0) ;
    }
    for (t2=t0+1;t2<=t0+nx;t2++) {
      S4(t0,-t0+t2-1,0) ;
    }
  }
}
if ((ny >= 2) && (nx == 1)) {
  for (t0=0;t0<=tmax-1;t0++) {
    S1(t0,0) ;
    for (t1=t0+1;t1<=t0+ny-1;t1++) {
      S3(t0,0,-t0+t1) ;
      S1(t0,-t0+t1) ;
      S4(t0,0,-t0+t1-1) ;
    }
    S4(t0,0,ny-1) ;
  }
}
if ((ny == 1) && (nx == 1)) {
  for (t0=0;t0<=tmax-1;t0++) {
    S1(t0,0) ;
    S4(t0,0,0) ;
  }
}
/* End of CLooG code */

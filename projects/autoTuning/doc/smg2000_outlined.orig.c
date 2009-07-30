// A prototype of the outlined function is inserted at the beginning of the code
void OUT__1__6755__(int *Aip__,int *xip__,int *rip__,double **App__,double **xpp__,double **rpp__,int *loopip__,int *loopjp__,int *loopkp__,int *hypre__sx1p__,int *hypre__sy1p__,int *hypre__sz1p__,int *hypre__sx2p__,int *hypre__sy2p__,int *hypre__sz2p__,int *hypre__sx3p__,int *hypre__sy3p__,int *hypre__sz3p__,int *hypre__nxp__,int *hypre__nyp__,int *hypre__nzp__);


// The target loop is replaced by a call to the outlined function
OUT__1__6755__(&Ai,&xi,&ri,&Ap,&xp,&rp,&loopi,&loopj,&loopk,&hypre__sx1,&hypre__sy1,&hypre__sz1,&hypre__sx2,&hypre__sy2,&hypre__sz2,&hypre__sx3,&hypre__sy3,&hypre__sz3,&hypre__nx,&hypre__ny,&hypre__nz);

// The outlined function generated from the target loop
void OUT__1__6755__(int *Aip__,int *xip__,int *rip__,double **App__,double **xpp__,double **rpp__,int *loopip__,int *loopjp__,int *loopkp__,int *hypre__sx1p__,int *hypre__sy1p__,int *hypre__sz1p__,int *hypre__sx2p__,int *hypre__sy2p__,int *hypre__sz2p__,int *hypre__sx3p__,int *hypre__sy3p__,int *hypre__sz3p__,int *hypre__nxp__,int *hypre__nyp__,int *hypre__nzp__)
{
  int Ai =  *((int *)Aip__);
  int xi =  *((int *)xip__);
  int ri =  *((int *)rip__);
  double *Ap =  *((double **)App__);
  double *xp =  *((double **)xpp__);
  double *rp =  *((double **)rpp__);
  int loopi =  *((int *)loopip__);
  int loopj =  *((int *)loopjp__);
  int loopk =  *((int *)loopkp__);
  int hypre__sx1 =  *((int *)hypre__sx1p__);
  int hypre__sy1 =  *((int *)hypre__sy1p__);
  int hypre__sz1 =  *((int *)hypre__sz1p__);
  int hypre__sx2 =  *((int *)hypre__sx2p__);
  int hypre__sy2 =  *((int *)hypre__sy2p__);
  int hypre__sz2 =  *((int *)hypre__sz2p__);
  int hypre__sx3 =  *((int *)hypre__sx3p__);
  int hypre__sy3 =  *((int *)hypre__sy3p__);
  int hypre__sz3 =  *((int *)hypre__sz3p__);
  int hypre__nx =  *((int *)hypre__nxp__);
  int hypre__ny =  *((int *)hypre__nyp__);
  int hypre__nz =  *((int *)hypre__nzp__);
  for (loopk = 0; loopk < hypre__nz; loopk++) {
    for (loopj = 0; loopj < hypre__ny; loopj++) {
      for (loopi = 0; loopi < hypre__nx; loopi++) {{
          rp[ri] -= ((Ap[Ai]) * (xp[xi]));
        }
        Ai += hypre__sx1;
        xi += hypre__sx2;
        ri += hypre__sx3;
      }
      Ai += (hypre__sy1 - (hypre__nx * hypre__sx1));
      xi += (hypre__sy2 - (hypre__nx * hypre__sx2));
      ri += (hypre__sy3 - (hypre__nx * hypre__sx3));
    }
    Ai += (hypre__sz1 - (hypre__ny * hypre__sy1));
    xi += (hypre__sz2 - (hypre__ny * hypre__sy2));
    ri += (hypre__sz3 - (hypre__ny * hypre__sy3));
// end for
  }
   *((int *)hypre__nzp__) = hypre__nz;
   *((int *)hypre__nyp__) = hypre__ny;
   *((int *)hypre__nxp__) = hypre__nx;
   *((int *)hypre__sz3p__) = hypre__sz3;
   *((int *)hypre__sy3p__) = hypre__sy3;
   *((int *)hypre__sx3p__) = hypre__sx3;
   *((int *)hypre__sz2p__) = hypre__sz2;
   *((int *)hypre__sy2p__) = hypre__sy2;
   *((int *)hypre__sx2p__) = hypre__sx2;
   *((int *)hypre__sz1p__) = hypre__sz1;
   *((int *)hypre__sy1p__) = hypre__sy1;
   *((int *)hypre__sx1p__) = hypre__sx1;
   *((int *)loopkp__) = loopk;
   *((int *)loopjp__) = loopj;
   *((int *)loopip__) = loopi;
   *((double **)rpp__) = rp;
   *((double **)xpp__) = xp;
   *((double **)App__) = Ap;
   *((int *)rip__) = ri;
   *((int *)xip__) = xi;
   *((int *)Aip__) = Ai;
}
 


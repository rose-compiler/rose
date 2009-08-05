// A prototype of the outlined function is inserted at the beginning of the code
void OUT__1__6755__(void **__out_argv);


// The target loop is replaced by a call to the outlined function
// with parameter wrapping statements 
void *__out_argv1__1527__[21];
 *(__out_argv1__1527__ + 0) = ((void *)(&hypre__nz));
 *(__out_argv1__1527__ + 1) = ((void *)(&hypre__ny));
 *(__out_argv1__1527__ + 2) = ((void *)(&hypre__nx));
 *(__out_argv1__1527__ + 3) = ((void *)(&hypre__sz3));
 *(__out_argv1__1527__ + 4) = ((void *)(&hypre__sy3));
 *(__out_argv1__1527__ + 5) = ((void *)(&hypre__sx3));
 *(__out_argv1__1527__ + 6) = ((void *)(&hypre__sz2));
 *(__out_argv1__1527__ + 7) = ((void *)(&hypre__sy2));
 *(__out_argv1__1527__ + 8) = ((void *)(&hypre__sx2));
 *(__out_argv1__1527__ + 9) = ((void *)(&hypre__sz1));
 *(__out_argv1__1527__ + 10) = ((void *)(&hypre__sy1));
 *(__out_argv1__1527__ + 11) = ((void *)(&hypre__sx1));
 *(__out_argv1__1527__ + 12) = ((void *)(&loopk));
 *(__out_argv1__1527__ + 13) = ((void *)(&loopj));
 *(__out_argv1__1527__ + 14) = ((void *)(&loopi));
 *(__out_argv1__1527__ + 15) = ((void *)(&rp));
 *(__out_argv1__1527__ + 16) = ((void *)(&xp));
 *(__out_argv1__1527__ + 17) = ((void *)(&Ap));
 *(__out_argv1__1527__ + 18) = ((void *)(&ri));
 *(__out_argv1__1527__ + 19) = ((void *)(&xi));
 *(__out_argv1__1527__ + 20) = ((void *)(&Ai));

OUT__1__6755__(__out_argv1__1527__);

// The outlined function generated from the target loop
// Saved into a separated file named OUT__1__6755__.c

void OUT__1__6755__(void **__out_argv)
{
  int Ai =  *((int *)(__out_argv[20]));
  int xi =  *((int *)(__out_argv[19]));
  int ri =  *((int *)(__out_argv[18]));
  double *Ap =  *((double **)(__out_argv[17]));
  double *xp =  *((double **)(__out_argv[16]));
  double *rp =  *((double **)(__out_argv[15]));
  int loopi =  *((int *)(__out_argv[14]));
  int loopj =  *((int *)(__out_argv[13]));
  int loopk =  *((int *)(__out_argv[12]));
  int hypre__sx1 =  *((int *)(__out_argv[11]));
  int hypre__sy1 =  *((int *)(__out_argv[10]));
  int hypre__sz1 =  *((int *)(__out_argv[9]));
  int hypre__sx2 =  *((int *)(__out_argv[8]));
  int hypre__sy2 =  *((int *)(__out_argv[7]));
  int hypre__sz2 =  *((int *)(__out_argv[6]));
  int hypre__sx3 =  *((int *)(__out_argv[5]));
  int hypre__sy3 =  *((int *)(__out_argv[4]));
  int hypre__sz3 =  *((int *)(__out_argv[3]));
  int hypre__nx =  *((int *)(__out_argv[2]));
  int hypre__ny =  *((int *)(__out_argv[1]));
  int hypre__nz =  *((int *)(__out_argv[0]));
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
  }
   *((int *)(__out_argv[0])) = hypre__nz;
   *((int *)(__out_argv[1])) = hypre__ny;
   *((int *)(__out_argv[2])) = hypre__nx;
   *((int *)(__out_argv[3])) = hypre__sz3;
   *((int *)(__out_argv[4])) = hypre__sy3;
   *((int *)(__out_argv[5])) = hypre__sx3;
   *((int *)(__out_argv[6])) = hypre__sz2;
   *((int *)(__out_argv[7])) = hypre__sy2;
   *((int *)(__out_argv[8])) = hypre__sx2;
   *((int *)(__out_argv[9])) = hypre__sz1;
   *((int *)(__out_argv[10])) = hypre__sy1;
   *((int *)(__out_argv[11])) = hypre__sx1;
   *((int *)(__out_argv[12])) = loopk;
   *((int *)(__out_argv[13])) = loopj;
   *((int *)(__out_argv[14])) = loopi;
   *((double **)(__out_argv[15])) = rp;
   *((double **)(__out_argv[16])) = xp;
   *((double **)(__out_argv[17])) = Ap;
   *((int *)(__out_argv[18])) = ri;
   *((int *)(__out_argv[19])) = xi;
   *((int *)(__out_argv[20])) = Ai;
}
 

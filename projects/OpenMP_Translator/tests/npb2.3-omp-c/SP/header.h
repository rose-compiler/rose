/*--------------------------------------------------------------------
c-------------------------------------------------------------------*/


/*--------------------------------------------------------------------
c The following include file is generated automatically by the
c "setparams" utility. It defines 
c      problem_size:  12, 64, 102, 162 (for class T, A, B, C)
c      dt_default:    default time step for this problem size if no
c                     config file
c      niter_default: default number of iterations for this problem size
c-------------------------------------------------------------------*/

#include "npbparams.h"

/* common /global */
static int grid_points[3];

/* common /constants/ */
static double tx1, tx2, tx3, ty1, ty2, ty3, tz1, tz2, tz3, 
              dx1, dx2, dx3, dx4, dx5, dy1, dy2, dy3, dy4, 
              dy5, dz1, dz2, dz3, dz4, dz5, dssp, dt, 
              ce[13][5], dxmax, dymax, dzmax, xxcon1, xxcon2, 
              xxcon3, xxcon4, xxcon5, dx1tx1, dx2tx1, dx3tx1,
              dx4tx1, dx5tx1, yycon1, yycon2, yycon3, yycon4,
              yycon5, dy1ty1, dy2ty1, dy3ty1, dy4ty1, dy5ty1,
              zzcon1, zzcon2, zzcon3, zzcon4, zzcon5, dz1tz1, 
              dz2tz1, dz3tz1, dz4tz1, dz5tz1, dnxm1, dnym1, 
              dnzm1, c1c2, c1c5, c3c4, c1345, conz1, c1, c2, 
              c3, c4, c5, c4dssp, c5dssp, dtdssp, dttx1, bt,
              dttx2, dtty1, dtty2, dttz1, dttz2, c2dttx1, 
              c2dtty1, c2dttz1, comz1, comz4, comz5, comz6, 
              c3c4tx3, c3c4ty3, c3c4tz3, c2iv, con43, con16;


#define	IMAX	PROBLEM_SIZE
#define	JMAX	PROBLEM_SIZE
#define	KMAX	PROBLEM_SIZE

/*--------------------------------------------------------------------
c   To improve cache performance, first two dimensions padded by 1 
c   for even number sizes only
c-------------------------------------------------------------------*/

/* common /fields/ */
static double u       [5][IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
	      us      [IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
              vs      [IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
              ws      [IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
              qs      [IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
              ainv    [IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
              rho_i   [IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
              speed   [IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
	      square  [IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
              rhs     [5][IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
              forcing [5][IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1],
              lhs     [15][IMAX/2*2+1][JMAX/2*2+1][KMAX/2*2+1];

/* common /work_1d/ */
static double cv[PROBLEM_SIZE], rhon[PROBLEM_SIZE],
              rhos[PROBLEM_SIZE], rhoq[PROBLEM_SIZE],
              cuf[PROBLEM_SIZE], q[PROBLEM_SIZE],
              ue[5][PROBLEM_SIZE], buf[5][PROBLEM_SIZE];

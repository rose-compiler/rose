void *_pp_globalobject;
void *__ompc_lock_critical;
static void **_thdprv_buf;
static void **_thdprv_ue;
static void **_thdprv_q;
static void **_thdprv_cuf;
/*--------------------------------------------------------------------
  
  NAS Parallel Benchmarks 2.3 OpenMP C versions - BT
  This benchmark is an OpenMP C version of the NPB BT code.
  
  The OpenMP C versions are developed by RWCP and derived from the serial
  Fortran versions in "NPB 2.3-serial" developed by NAS.
  Permission to use, copy, distribute and modify this software for any
  purpose with or without fee is hereby granted.
  This software is provided "as is" without express or implied warranty.
  
  Send comments on the OpenMP C versions to pdp-openmp@rwcp.or.jp
  Information on OpenMP activities at RWCP is available at:
           http://pdplab.trc.rwcp.or.jp/pdperf/Omni/
  
  Information on NAS Parallel Benchmarks 2.3 is available at:
  
           http://www.nas.nasa.gov/NAS/NPB/
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
  Authors: R. Van der Wijngaart
           T. Harris
           M. Yarrow
  OpenMP C version: S. Satoh
  
--------------------------------------------------------------------*/
#include "npb-C.h"
/* global variables */
#include "header.h"
/* function declarations */
#include "ompcLib.h" 
static void add();
static void adi();
static void error_norm(double rms[5UL]);
static void rhs_norm(double rms[5UL]);
static void exact_rhs();
static void exact_solution(double xi,double eta,double zeta,double dtemp[5UL]);
static void initialize();
static void lhsinit();
static void lhsx();
static void lhsy();
static void lhsz();
static void compute_rhs();
static void set_constants();
static void verify(int no_time_steps,char *cclass,boolean *verified);
static void x_solve();
static void x_backsubstitute();
static void x_solve_cell();
static void matvec_sub(double ablock[5UL][5UL],double avec[5UL],double bvec[5UL]);
static void matmul_sub(double ablock[5UL][5UL],double bblock[5UL][5UL],double cblock[5UL][5UL]);
static void binvcrhs(double lhs[5UL][5UL],double c[5UL][5UL],double r[5UL]);
static void binvrhs(double lhs[5UL][5UL],double r[5UL]);
static void y_solve();
static void y_backsubstitute();
static void y_solve_cell();
static void z_solve();
static void z_backsubstitute();
static void z_solve_cell();

static void __ompc_func_1(void **__ompc_args)
{
  int *_pp_niter;
  _pp_niter = ((int *)(__ompc_args[0]));
  int *_pp_nthreads;
  _pp_nthreads = ((int *)(__ompc_args[1]));
  int _p_niter =  *_pp_niter;
  int _p_step;
{
    for (_p_step = 1; _p_step <= _p_niter; _p_step++) {
      if (((_p_step % 20) == 0) || (_p_step == 1)) {
        if (_ompc_is_master()) {
          printf(" Time step %4d\n",_p_step);
        }
        else {
        }
      }
      else {
      }
      adi();
    }
#if defined(_OPENMP)
    if (_ompc_is_master()) {
       *_pp_nthreads = omp_get_num_threads();
    }
    else {
    }
#endif /* _OPENMP */
  }
}


static void __ompc_func_2(void **__ompc_args)
{
{
    initialize();
    lhsinit();
    exact_rhs();
/*--------------------------------------------------------------------
c      do one time step to touch all code, and reinitialize
c-------------------------------------------------------------------*/
    adi();
    initialize();
  }
}

/*--------------------------------------------------------------------
      program BT
c-------------------------------------------------------------------*/

int main(int argc,char **argv)
{
  int status = 0;
  _ompc_init(argc,argv);
  int niter;
  int step;
  int n3;
  int nthreads = 1;
  double navg;
  double mflops;
  double tmax;
  boolean verified;
  char cclass;
  FILE *fp;
/*--------------------------------------------------------------------
c      Root node reads input file (if it exists) else takes
c      defaults from parameters
c-------------------------------------------------------------------*/
  printf("\n\n NAS Parallel Benchmarks 2.3 OpenMP C version - BT Benchmark\n\n");
  fp = fopen("inputbt.data","r");
  if (fp != ((0))) {
    printf(" Reading from input file inputbt.data");
    fscanf(fp,"%d",&niter);
    while(fgetc(fp) != ('\n')){
    }
    fscanf(fp,"%lg",&dt);
    while(fgetc(fp) != ('\n')){
    }
    fscanf(fp,"%d%d%d",(grid_points + 0),(grid_points + 1),(grid_points + 2));
    fclose(fp);
  }
  else {
    printf(" No input file inputbt.data. Using compiled defaults\n");
    niter = 60;
    dt = 0.010;
    grid_points[0] = 12;
    grid_points[1] = 12;
    grid_points[2] = 12;
  }
  printf(" Size: %3dx%3dx%3d\n",(grid_points[0]),(grid_points[1]),(grid_points[2]));
  printf(" Iterations: %3d   dt: %10.6f\n",niter,dt);
  if ((((grid_points[0]) > 12) || ((grid_points[1]) > 12)) || ((grid_points[2]) > 12)) {
    printf(" %dx%dx%d\n",(grid_points[0]),(grid_points[1]),(grid_points[2]));
    printf(" Problem size too big for compiled array sizes\n");
    exit(1);
  }
  else {
  }
  set_constants();
{
    _ompc_do_parallel(__ompc_func_2,0);
  }
  timer_clear(1);
  timer_start(1);
{
    void *__ompc_argv[2UL];
     *(__ompc_argv + 0) = ((void *)(&niter));
     *(__ompc_argv + 1) = ((void *)(&nthreads));
    _ompc_do_parallel(__ompc_func_1,__ompc_argv);
  }
  timer_stop(1);
  tmax = timer_read(1);
  verify(niter,&cclass,&verified);
  n3 = (((grid_points[0]) * (grid_points[1])) * (grid_points[2]));
  navg = (((((grid_points[0]) + (grid_points[1])) + (grid_points[2]))) / 3.0);
  if (tmax != 0.0) {
    mflops = (((1.0e-6 * ((double )niter)) * (((3478.8 * ((double )n3)) - (17655.7 * (navg * navg))) + (28023.7 * navg))) / tmax);
  }
  else {
    mflops = 0.0;
  }
  c_print_results(("BT"),cclass,(grid_points[0]),(grid_points[1]),(grid_points[2]),niter,nthreads,tmax,mflops,("          floating point"),verified,("2.3"),("17 Aug 2006"),("/home/liao6/OpenMPtranslation/roseomp"),("$(CC)"),("-L$(omniInstallPath)/lib/openmp/lib -lompc ..."),("-I../common -I/home/liao6/OpenMPtranslation"),("-D_OPENMP"),("-lm"),("(none)"));
  _ompc_terminate(status);
}

/*--------------------------------------------------------------------
c-------------------------------------------------------------------*/

static void add()
{
/*--------------------------------------------------------------------
c     addition of update to the vector u
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
  int m;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          for (m = 0; m < 5; m++) {
            (((u[_p_loop_index])[j])[k])[m] = (((((u[_p_loop_index])[j])[k])[m]) + ((((rhs[_p_loop_index])[j])[k])[m]));
          }
        }
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void adi()
{
  compute_rhs();
  x_solve();
  y_solve();
  z_solve();
  add();
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void error_norm(double *rms)
{
/*--------------------------------------------------------------------
c     this function computes the norm of the difference between the
c     computed solution and the exact solution
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
  int m;
  int d;
  double xi;
  double eta;
  double zeta;
  double u_exact[5UL];
  double add;
  for (m = 0; m < 5; m++) {
    rms[m] = 0.0;
  }
  for (i = 0; i < (grid_points[0]); i++) {
    xi = (((double )i) * dnxm1);
    for (j = 0; j < (grid_points[1]); j++) {
      eta = (((double )j) * dnym1);
      for (k = 0; k < (grid_points[2]); k++) {
        zeta = (((double )k) * dnzm1);
        exact_solution(xi,eta,zeta,u_exact);
        for (m = 0; m < 5; m++) {
          add = (((((u[i])[j])[k])[m]) - (u_exact[m]));
          rms[m] = ((rms[m]) + (add * add));
        }
      }
    }
  }
  for (m = 0; m < 5; m++) {
    for (d = 0; d <= 2; d++) {
      rms[m] = ((rms[m]) / ((double )((grid_points[d]) - 2)));
    }
    rms[m] = sqrt((rms[m]));
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void rhs_norm(double *rms)
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
  int i;
  int j;
  int k;
  int d;
  int m;
  double add;
  for (m = 0; m < 5; m++) {
    rms[m] = 0.0;
  }
  for (i = 1; i < ((grid_points[0]) - 1); i++) {
    for (j = 1; j < ((grid_points[1]) - 1); j++) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (m = 0; m < 5; m++) {
          add = ((((rhs[i])[j])[k])[m]);
          rms[m] = ((rms[m]) + (add * add));
        }
      }
    }
  }
  for (m = 0; m < 5; m++) {
    for (d = 0; d <= 2; d++) {
      rms[m] = ((rms[m]) / ((double )((grid_points[d]) - 2)));
    }
    rms[m] = sqrt((rms[m]));
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void exact_rhs()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     compute the right hand side based on exact solution
c-------------------------------------------------------------------*/
  double dtemp[5UL];
  double xi;
  double eta;
  double zeta;
  double dtpp;
  int m;
  int i;
  int j;
  int k;
  int ip1;
  int im1;
  int jp1;
  int jm1;
  int km1;
  int kp1;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[0]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 0; j < (grid_points[1]); j++) {
        for (k = 0; k < (grid_points[2]); k++) {
          for (m = 0; m < 5; m++) {
            (((forcing[_p_loop_index])[j])[k])[m] = 0.0;
          }
        }
      }
    }
    _ompc_barrier();
  }
{
    double (*_ppthd_buf)[12UL][5UL];
    double (*_ppthd_ue)[12UL][5UL];
    double (*_ppthd_q)[12UL];
    double (*_ppthd_cuf)[12UL];
    _ppthd_cuf = ((double (*)[12UL])(_ompc_get_thdprv(&_thdprv_cuf,sizeof(cuf),cuf)));
    _ppthd_q = ((double (*)[12UL])(_ompc_get_thdprv(&_thdprv_q,sizeof(q),q)));
    _ppthd_ue = ((double (*)[12UL][5UL])(_ompc_get_thdprv(&_thdprv_ue,sizeof(ue),ue)));
    _ppthd_buf = ((double (*)[12UL][5UL])(_ompc_get_thdprv(&_thdprv_buf,sizeof(buf),buf)));
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[1]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      eta = (((double )_p_loop_index) * dnym1);
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        zeta = (((double )k) * dnzm1);
        for (i = 0; i < (grid_points[0]); i++) {
          xi = (((double )i) * dnxm1);
          exact_solution(xi,eta,zeta,dtemp);
          for (m = 0; m < 5; m++) {
            (( *_ppthd_ue)[i])[m] = (dtemp[m]);
          }
          dtpp = (1.0 / (dtemp[0]));
          for (m = 1; m <= 4; m++) {
            (( *_ppthd_buf)[i])[m] = (dtpp * (dtemp[m]));
          }
          ( *_ppthd_cuf)[i] = (((( *_ppthd_buf)[i])[1]) * ((( *_ppthd_buf)[i])[1]));
          (( *_ppthd_buf)[i])[0] = (((( *_ppthd_cuf)[i]) + (((( *_ppthd_buf)[i])[2]) * ((( *_ppthd_buf)[i])[2]))) + (((( *_ppthd_buf)[i])[3]) * ((( *_ppthd_buf)[i])[3])));
          ( *_ppthd_q)[i] = (0.5 * (((((( *_ppthd_buf)[i])[1]) * ((( *_ppthd_ue)[i])[1])) + (((( *_ppthd_buf)[i])[2]) * ((( *_ppthd_ue)[i])[2]))) + (((( *_ppthd_buf)[i])[3]) * ((( *_ppthd_ue)[i])[3]))));
        }
        for (i = 1; i < ((grid_points[0]) - 1); i++) {
          im1 = (i - 1);
          ip1 = (i + 1);
          (((forcing[i])[_p_loop_index])[k])[0] = ((((((forcing[i])[_p_loop_index])[k])[0]) - (tx2 * (((( *_ppthd_ue)[ip1])[1]) - ((( *_ppthd_ue)[im1])[1])))) + (dx1tx1 * ((((( *_ppthd_ue)[ip1])[0]) - (2.0 * ((( *_ppthd_ue)[i])[0]))) + ((( *_ppthd_ue)[im1])[0]))));
          (((forcing[i])[_p_loop_index])[k])[1] = (((((((forcing[i])[_p_loop_index])[k])[1]) - (tx2 * (((((( *_ppthd_ue)[ip1])[1]) * ((( *_ppthd_buf)[ip1])[1])) + (c2 * (((( *_ppthd_ue)[ip1])[4]) - (( *_ppthd_q)[ip1])))) - ((((( *_ppthd_ue)[im1])[1]) * ((( *_ppthd_buf)[im1])[1])) + (c2 * (((( *_ppthd_ue)[im1])[4]) - (( *_ppthd_q)[im1]))))))) + (xxcon1 * ((((( *_ppthd_buf)[ip1])[1]) - (2.0 * ((( *_ppthd_buf)[i])[1]))) + ((( *_ppthd_buf)[im1])[1])))) + (dx2tx1 * ((((( *_ppthd_ue)[ip1])[1]) - (2.0 * ((( *_ppthd_ue)[i])[1]))) + ((( *_ppthd_ue)[im1])[1]))));
          (((forcing[i])[_p_loop_index])[k])[2] = (((((((forcing[i])[_p_loop_index])[k])[2]) - (tx2 * ((((( *_ppthd_ue)[ip1])[2]) * ((( *_ppthd_buf)[ip1])[1])) - (((( *_ppthd_ue)[im1])[2]) * ((( *_ppthd_buf)[im1])[1]))))) + (xxcon2 * ((((( *_ppthd_buf)[ip1])[2]) - (2.0 * ((( *_ppthd_buf)[i])[2]))) + ((( *_ppthd_buf)[im1])[2])))) + (dx3tx1 * ((((( *_ppthd_ue)[ip1])[2]) - (2.0 * ((( *_ppthd_ue)[i])[2]))) + ((( *_ppthd_ue)[im1])[2]))));
          (((forcing[i])[_p_loop_index])[k])[3] = (((((((forcing[i])[_p_loop_index])[k])[3]) - (tx2 * ((((( *_ppthd_ue)[ip1])[3]) * ((( *_ppthd_buf)[ip1])[1])) - (((( *_ppthd_ue)[im1])[3]) * ((( *_ppthd_buf)[im1])[1]))))) + (xxcon2 * ((((( *_ppthd_buf)[ip1])[3]) - (2.0 * ((( *_ppthd_buf)[i])[3]))) + ((( *_ppthd_buf)[im1])[3])))) + (dx4tx1 * ((((( *_ppthd_ue)[ip1])[3]) - (2.0 * ((( *_ppthd_ue)[i])[3]))) + ((( *_ppthd_ue)[im1])[3]))));
          (((forcing[i])[_p_loop_index])[k])[4] = (((((((((forcing[i])[_p_loop_index])[k])[4]) - (tx2 * ((((( *_ppthd_buf)[ip1])[1]) * ((c1 * ((( *_ppthd_ue)[ip1])[4])) - (c2 * (( *_ppthd_q)[ip1])))) - (((( *_ppthd_buf)[im1])[1]) * ((c1 * ((( *_ppthd_ue)[im1])[4])) - (c2 * (( *_ppthd_q)[im1]))))))) + ((0.5 * xxcon3) * ((((( *_ppthd_buf)[ip1])[0]) - (2.0 * ((( *_ppthd_buf)[i])[0]))) + ((( *_ppthd_buf)[im1])[0])))) + (xxcon4 * (((( *_ppthd_cuf)[ip1]) - (2.0 * (( *_ppthd_cuf)[i]))) + (( *_ppthd_cuf)[im1])))) + (xxcon5 * ((((( *_ppthd_buf)[ip1])[4]) - (2.0 * ((( *_ppthd_buf)[i])[4]))) + ((( *_ppthd_buf)[im1])[4])))) + (dx5tx1 * ((((( *_ppthd_ue)[ip1])[4]) - (2.0 * ((( *_ppthd_ue)[i])[4]))) + ((( *_ppthd_ue)[im1])[4]))));
        }
/*--------------------------------------------------------------------
c     Fourth-order dissipation                         
c-------------------------------------------------------------------*/
        for (m = 0; m < 5; m++) {
          i = 1;
          (((forcing[i])[_p_loop_index])[k])[m] = (((((forcing[i])[_p_loop_index])[k])[m]) - (dssp * (((5.0 * ((( *_ppthd_ue)[i])[m])) - (4.0 * ((( *_ppthd_ue)[i + 1])[m]))) + ((( *_ppthd_ue)[i + 2])[m]))));
          i = 2;
          (((forcing[i])[_p_loop_index])[k])[m] = (((((forcing[i])[_p_loop_index])[k])[m]) - (dssp * (((((-4.0) * ((( *_ppthd_ue)[i - 1])[m])) + (6.0 * ((( *_ppthd_ue)[i])[m]))) - (4.0 * ((( *_ppthd_ue)[i + 1])[m]))) + ((( *_ppthd_ue)[i + 2])[m]))));
        }
        for (m = 0; m < 5; m++) {
          for (i = (1 * 3); i <= (((grid_points[0]) - (3 * 1)) - 1); i++) {
            (((forcing[i])[_p_loop_index])[k])[m] = (((((forcing[i])[_p_loop_index])[k])[m]) - (dssp * ((((((( *_ppthd_ue)[i - 2])[m]) - (4.0 * ((( *_ppthd_ue)[i - 1])[m]))) + (6.0 * ((( *_ppthd_ue)[i])[m]))) - (4.0 * ((( *_ppthd_ue)[i + 1])[m]))) + ((( *_ppthd_ue)[i + 2])[m]))));
          }
        }
        for (m = 0; m < 5; m++) {
          i = ((grid_points[0]) - 3);
          (((forcing[i])[_p_loop_index])[k])[m] = (((((forcing[i])[_p_loop_index])[k])[m]) - (dssp * (((((( *_ppthd_ue)[i - 2])[m]) - (4.0 * ((( *_ppthd_ue)[i - 1])[m]))) + (6.0 * ((( *_ppthd_ue)[i])[m]))) - (4.0 * ((( *_ppthd_ue)[i + 1])[m])))));
          i = ((grid_points[0]) - 2);
          (((forcing[i])[_p_loop_index])[k])[m] = (((((forcing[i])[_p_loop_index])[k])[m]) - (dssp * ((((( *_ppthd_ue)[i - 2])[m]) - (4.0 * ((( *_ppthd_ue)[i - 1])[m]))) + (5.0 * ((( *_ppthd_ue)[i])[m])))));
        }
      }
    }
    _ompc_barrier();
  }
{
    double (*_ppthd_buf)[12UL][5UL];
    double (*_ppthd_ue)[12UL][5UL];
    double (*_ppthd_q)[12UL];
    double (*_ppthd_cuf)[12UL];
    _ppthd_cuf = ((double (*)[12UL])(_ompc_get_thdprv(&_thdprv_cuf,sizeof(cuf),cuf)));
    _ppthd_q = ((double (*)[12UL])(_ompc_get_thdprv(&_thdprv_q,sizeof(q),q)));
    _ppthd_ue = ((double (*)[12UL][5UL])(_ompc_get_thdprv(&_thdprv_ue,sizeof(ue),ue)));
    _ppthd_buf = ((double (*)[12UL][5UL])(_ompc_get_thdprv(&_thdprv_buf,sizeof(buf),buf)));
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      xi = (((double )_p_loop_index) * dnxm1);
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        zeta = (((double )k) * dnzm1);
        for (j = 0; j < (grid_points[1]); j++) {
          eta = (((double )j) * dnym1);
          exact_solution(xi,eta,zeta,dtemp);
          for (m = 0; m < 5; m++) {
            (( *_ppthd_ue)[j])[m] = (dtemp[m]);
          }
          dtpp = (1.0 / (dtemp[0]));
          for (m = 1; m <= 4; m++) {
            (( *_ppthd_buf)[j])[m] = (dtpp * (dtemp[m]));
          }
          ( *_ppthd_cuf)[j] = (((( *_ppthd_buf)[j])[2]) * ((( *_ppthd_buf)[j])[2]));
          (( *_ppthd_buf)[j])[0] = (((( *_ppthd_cuf)[j]) + (((( *_ppthd_buf)[j])[1]) * ((( *_ppthd_buf)[j])[1]))) + (((( *_ppthd_buf)[j])[3]) * ((( *_ppthd_buf)[j])[3])));
          ( *_ppthd_q)[j] = (0.5 * (((((( *_ppthd_buf)[j])[1]) * ((( *_ppthd_ue)[j])[1])) + (((( *_ppthd_buf)[j])[2]) * ((( *_ppthd_ue)[j])[2]))) + (((( *_ppthd_buf)[j])[3]) * ((( *_ppthd_ue)[j])[3]))));
        }
        for (j = 1; j < ((grid_points[1]) - 1); j++) {
          jm1 = (j - 1);
          jp1 = (j + 1);
          (((forcing[_p_loop_index])[j])[k])[0] = ((((((forcing[_p_loop_index])[j])[k])[0]) - (ty2 * (((( *_ppthd_ue)[jp1])[2]) - ((( *_ppthd_ue)[jm1])[2])))) + (dy1ty1 * ((((( *_ppthd_ue)[jp1])[0]) - (2.0 * ((( *_ppthd_ue)[j])[0]))) + ((( *_ppthd_ue)[jm1])[0]))));
          (((forcing[_p_loop_index])[j])[k])[1] = (((((((forcing[_p_loop_index])[j])[k])[1]) - (ty2 * ((((( *_ppthd_ue)[jp1])[1]) * ((( *_ppthd_buf)[jp1])[2])) - (((( *_ppthd_ue)[jm1])[1]) * ((( *_ppthd_buf)[jm1])[2]))))) + (yycon2 * ((((( *_ppthd_buf)[jp1])[1]) - (2.0 * ((( *_ppthd_buf)[j])[1]))) + ((( *_ppthd_buf)[jm1])[1])))) + (dy2ty1 * ((((( *_ppthd_ue)[jp1])[1]) - (2.0 * ((( *_ppthd_ue)[j])[1]))) + ((( *_ppthd_ue)[jm1])[1]))));
          (((forcing[_p_loop_index])[j])[k])[2] = (((((((forcing[_p_loop_index])[j])[k])[2]) - (ty2 * (((((( *_ppthd_ue)[jp1])[2]) * ((( *_ppthd_buf)[jp1])[2])) + (c2 * (((( *_ppthd_ue)[jp1])[4]) - (( *_ppthd_q)[jp1])))) - ((((( *_ppthd_ue)[jm1])[2]) * ((( *_ppthd_buf)[jm1])[2])) + (c2 * (((( *_ppthd_ue)[jm1])[4]) - (( *_ppthd_q)[jm1]))))))) + (yycon1 * ((((( *_ppthd_buf)[jp1])[2]) - (2.0 * ((( *_ppthd_buf)[j])[2]))) + ((( *_ppthd_buf)[jm1])[2])))) + (dy3ty1 * ((((( *_ppthd_ue)[jp1])[2]) - (2.0 * ((( *_ppthd_ue)[j])[2]))) + ((( *_ppthd_ue)[jm1])[2]))));
          (((forcing[_p_loop_index])[j])[k])[3] = (((((((forcing[_p_loop_index])[j])[k])[3]) - (ty2 * ((((( *_ppthd_ue)[jp1])[3]) * ((( *_ppthd_buf)[jp1])[2])) - (((( *_ppthd_ue)[jm1])[3]) * ((( *_ppthd_buf)[jm1])[2]))))) + (yycon2 * ((((( *_ppthd_buf)[jp1])[3]) - (2.0 * ((( *_ppthd_buf)[j])[3]))) + ((( *_ppthd_buf)[jm1])[3])))) + (dy4ty1 * ((((( *_ppthd_ue)[jp1])[3]) - (2.0 * ((( *_ppthd_ue)[j])[3]))) + ((( *_ppthd_ue)[jm1])[3]))));
          (((forcing[_p_loop_index])[j])[k])[4] = (((((((((forcing[_p_loop_index])[j])[k])[4]) - (ty2 * ((((( *_ppthd_buf)[jp1])[2]) * ((c1 * ((( *_ppthd_ue)[jp1])[4])) - (c2 * (( *_ppthd_q)[jp1])))) - (((( *_ppthd_buf)[jm1])[2]) * ((c1 * ((( *_ppthd_ue)[jm1])[4])) - (c2 * (( *_ppthd_q)[jm1]))))))) + ((0.5 * yycon3) * ((((( *_ppthd_buf)[jp1])[0]) - (2.0 * ((( *_ppthd_buf)[j])[0]))) + ((( *_ppthd_buf)[jm1])[0])))) + (yycon4 * (((( *_ppthd_cuf)[jp1]) - (2.0 * (( *_ppthd_cuf)[j]))) + (( *_ppthd_cuf)[jm1])))) + (yycon5 * ((((( *_ppthd_buf)[jp1])[4]) - (2.0 * ((( *_ppthd_buf)[j])[4]))) + ((( *_ppthd_buf)[jm1])[4])))) + (dy5ty1 * ((((( *_ppthd_ue)[jp1])[4]) - (2.0 * ((( *_ppthd_ue)[j])[4]))) + ((( *_ppthd_ue)[jm1])[4]))));
        }
/*--------------------------------------------------------------------
c     Fourth-order dissipation                      
c-------------------------------------------------------------------*/
        for (m = 0; m < 5; m++) {
          j = 1;
          (((forcing[_p_loop_index])[j])[k])[m] = (((((forcing[_p_loop_index])[j])[k])[m]) - (dssp * (((5.0 * ((( *_ppthd_ue)[j])[m])) - (4.0 * ((( *_ppthd_ue)[j + 1])[m]))) + ((( *_ppthd_ue)[j + 2])[m]))));
          j = 2;
          (((forcing[_p_loop_index])[j])[k])[m] = (((((forcing[_p_loop_index])[j])[k])[m]) - (dssp * (((((-4.0) * ((( *_ppthd_ue)[j - 1])[m])) + (6.0 * ((( *_ppthd_ue)[j])[m]))) - (4.0 * ((( *_ppthd_ue)[j + 1])[m]))) + ((( *_ppthd_ue)[j + 2])[m]))));
        }
        for (m = 0; m < 5; m++) {
          for (j = (1 * 3); j <= (((grid_points[1]) - (3 * 1)) - 1); j++) {
            (((forcing[_p_loop_index])[j])[k])[m] = (((((forcing[_p_loop_index])[j])[k])[m]) - (dssp * ((((((( *_ppthd_ue)[j - 2])[m]) - (4.0 * ((( *_ppthd_ue)[j - 1])[m]))) + (6.0 * ((( *_ppthd_ue)[j])[m]))) - (4.0 * ((( *_ppthd_ue)[j + 1])[m]))) + ((( *_ppthd_ue)[j + 2])[m]))));
          }
        }
        for (m = 0; m < 5; m++) {
          j = ((grid_points[1]) - 3);
          (((forcing[_p_loop_index])[j])[k])[m] = (((((forcing[_p_loop_index])[j])[k])[m]) - (dssp * (((((( *_ppthd_ue)[j - 2])[m]) - (4.0 * ((( *_ppthd_ue)[j - 1])[m]))) + (6.0 * ((( *_ppthd_ue)[j])[m]))) - (4.0 * ((( *_ppthd_ue)[j + 1])[m])))));
          j = ((grid_points[1]) - 2);
          (((forcing[_p_loop_index])[j])[k])[m] = (((((forcing[_p_loop_index])[j])[k])[m]) - (dssp * ((((( *_ppthd_ue)[j - 2])[m]) - (4.0 * ((( *_ppthd_ue)[j - 1])[m]))) + (5.0 * ((( *_ppthd_ue)[j])[m])))));
        }
      }
    }
    _ompc_barrier();
  }
{
    double (*_ppthd_buf)[12UL][5UL];
    double (*_ppthd_ue)[12UL][5UL];
    double (*_ppthd_q)[12UL];
    double (*_ppthd_cuf)[12UL];
    _ppthd_cuf = ((double (*)[12UL])(_ompc_get_thdprv(&_thdprv_cuf,sizeof(cuf),cuf)));
    _ppthd_q = ((double (*)[12UL])(_ompc_get_thdprv(&_thdprv_q,sizeof(q),q)));
    _ppthd_ue = ((double (*)[12UL][5UL])(_ompc_get_thdprv(&_thdprv_ue,sizeof(ue),ue)));
    _ppthd_buf = ((double (*)[12UL][5UL])(_ompc_get_thdprv(&_thdprv_buf,sizeof(buf),buf)));
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      xi = (((double )_p_loop_index) * dnxm1);
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        eta = (((double )j) * dnym1);
        for (k = 0; k < (grid_points[2]); k++) {
          zeta = (((double )k) * dnzm1);
          exact_solution(xi,eta,zeta,dtemp);
          for (m = 0; m < 5; m++) {
            (( *_ppthd_ue)[k])[m] = (dtemp[m]);
          }
          dtpp = (1.0 / (dtemp[0]));
          for (m = 1; m <= 4; m++) {
            (( *_ppthd_buf)[k])[m] = (dtpp * (dtemp[m]));
          }
          ( *_ppthd_cuf)[k] = (((( *_ppthd_buf)[k])[3]) * ((( *_ppthd_buf)[k])[3]));
          (( *_ppthd_buf)[k])[0] = (((( *_ppthd_cuf)[k]) + (((( *_ppthd_buf)[k])[1]) * ((( *_ppthd_buf)[k])[1]))) + (((( *_ppthd_buf)[k])[2]) * ((( *_ppthd_buf)[k])[2])));
          ( *_ppthd_q)[k] = (0.5 * (((((( *_ppthd_buf)[k])[1]) * ((( *_ppthd_ue)[k])[1])) + (((( *_ppthd_buf)[k])[2]) * ((( *_ppthd_ue)[k])[2]))) + (((( *_ppthd_buf)[k])[3]) * ((( *_ppthd_ue)[k])[3]))));
        }
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          km1 = (k - 1);
          kp1 = (k + 1);
          (((forcing[_p_loop_index])[j])[k])[0] = ((((((forcing[_p_loop_index])[j])[k])[0]) - (tz2 * (((( *_ppthd_ue)[kp1])[3]) - ((( *_ppthd_ue)[km1])[3])))) + (dz1tz1 * ((((( *_ppthd_ue)[kp1])[0]) - (2.0 * ((( *_ppthd_ue)[k])[0]))) + ((( *_ppthd_ue)[km1])[0]))));
          (((forcing[_p_loop_index])[j])[k])[1] = (((((((forcing[_p_loop_index])[j])[k])[1]) - (tz2 * ((((( *_ppthd_ue)[kp1])[1]) * ((( *_ppthd_buf)[kp1])[3])) - (((( *_ppthd_ue)[km1])[1]) * ((( *_ppthd_buf)[km1])[3]))))) + (zzcon2 * ((((( *_ppthd_buf)[kp1])[1]) - (2.0 * ((( *_ppthd_buf)[k])[1]))) + ((( *_ppthd_buf)[km1])[1])))) + (dz2tz1 * ((((( *_ppthd_ue)[kp1])[1]) - (2.0 * ((( *_ppthd_ue)[k])[1]))) + ((( *_ppthd_ue)[km1])[1]))));
          (((forcing[_p_loop_index])[j])[k])[2] = (((((((forcing[_p_loop_index])[j])[k])[2]) - (tz2 * ((((( *_ppthd_ue)[kp1])[2]) * ((( *_ppthd_buf)[kp1])[3])) - (((( *_ppthd_ue)[km1])[2]) * ((( *_ppthd_buf)[km1])[3]))))) + (zzcon2 * ((((( *_ppthd_buf)[kp1])[2]) - (2.0 * ((( *_ppthd_buf)[k])[2]))) + ((( *_ppthd_buf)[km1])[2])))) + (dz3tz1 * ((((( *_ppthd_ue)[kp1])[2]) - (2.0 * ((( *_ppthd_ue)[k])[2]))) + ((( *_ppthd_ue)[km1])[2]))));
          (((forcing[_p_loop_index])[j])[k])[3] = (((((((forcing[_p_loop_index])[j])[k])[3]) - (tz2 * (((((( *_ppthd_ue)[kp1])[3]) * ((( *_ppthd_buf)[kp1])[3])) + (c2 * (((( *_ppthd_ue)[kp1])[4]) - (( *_ppthd_q)[kp1])))) - ((((( *_ppthd_ue)[km1])[3]) * ((( *_ppthd_buf)[km1])[3])) + (c2 * (((( *_ppthd_ue)[km1])[4]) - (( *_ppthd_q)[km1]))))))) + (zzcon1 * ((((( *_ppthd_buf)[kp1])[3]) - (2.0 * ((( *_ppthd_buf)[k])[3]))) + ((( *_ppthd_buf)[km1])[3])))) + (dz4tz1 * ((((( *_ppthd_ue)[kp1])[3]) - (2.0 * ((( *_ppthd_ue)[k])[3]))) + ((( *_ppthd_ue)[km1])[3]))));
          (((forcing[_p_loop_index])[j])[k])[4] = (((((((((forcing[_p_loop_index])[j])[k])[4]) - (tz2 * ((((( *_ppthd_buf)[kp1])[3]) * ((c1 * ((( *_ppthd_ue)[kp1])[4])) - (c2 * (( *_ppthd_q)[kp1])))) - (((( *_ppthd_buf)[km1])[3]) * ((c1 * ((( *_ppthd_ue)[km1])[4])) - (c2 * (( *_ppthd_q)[km1]))))))) + ((0.5 * zzcon3) * ((((( *_ppthd_buf)[kp1])[0]) - (2.0 * ((( *_ppthd_buf)[k])[0]))) + ((( *_ppthd_buf)[km1])[0])))) + (zzcon4 * (((( *_ppthd_cuf)[kp1]) - (2.0 * (( *_ppthd_cuf)[k]))) + (( *_ppthd_cuf)[km1])))) + (zzcon5 * ((((( *_ppthd_buf)[kp1])[4]) - (2.0 * ((( *_ppthd_buf)[k])[4]))) + ((( *_ppthd_buf)[km1])[4])))) + (dz5tz1 * ((((( *_ppthd_ue)[kp1])[4]) - (2.0 * ((( *_ppthd_ue)[k])[4]))) + ((( *_ppthd_ue)[km1])[4]))));
        }
/*--------------------------------------------------------------------
c     Fourth-order dissipation                        
c-------------------------------------------------------------------*/
        for (m = 0; m < 5; m++) {
          k = 1;
          (((forcing[_p_loop_index])[j])[k])[m] = (((((forcing[_p_loop_index])[j])[k])[m]) - (dssp * (((5.0 * ((( *_ppthd_ue)[k])[m])) - (4.0 * ((( *_ppthd_ue)[k + 1])[m]))) + ((( *_ppthd_ue)[k + 2])[m]))));
          k = 2;
          (((forcing[_p_loop_index])[j])[k])[m] = (((((forcing[_p_loop_index])[j])[k])[m]) - (dssp * (((((-4.0) * ((( *_ppthd_ue)[k - 1])[m])) + (6.0 * ((( *_ppthd_ue)[k])[m]))) - (4.0 * ((( *_ppthd_ue)[k + 1])[m]))) + ((( *_ppthd_ue)[k + 2])[m]))));
        }
        for (m = 0; m < 5; m++) {
          for (k = (1 * 3); k <= (((grid_points[2]) - (3 * 1)) - 1); k++) {
            (((forcing[_p_loop_index])[j])[k])[m] = (((((forcing[_p_loop_index])[j])[k])[m]) - (dssp * ((((((( *_ppthd_ue)[k - 2])[m]) - (4.0 * ((( *_ppthd_ue)[k - 1])[m]))) + (6.0 * ((( *_ppthd_ue)[k])[m]))) - (4.0 * ((( *_ppthd_ue)[k + 1])[m]))) + ((( *_ppthd_ue)[k + 2])[m]))));
          }
        }
        for (m = 0; m < 5; m++) {
          k = ((grid_points[2]) - 3);
          (((forcing[_p_loop_index])[j])[k])[m] = (((((forcing[_p_loop_index])[j])[k])[m]) - (dssp * (((((( *_ppthd_ue)[k - 2])[m]) - (4.0 * ((( *_ppthd_ue)[k - 1])[m]))) + (6.0 * ((( *_ppthd_ue)[k])[m]))) - (4.0 * ((( *_ppthd_ue)[k + 1])[m])))));
          k = ((grid_points[2]) - 2);
          (((forcing[_p_loop_index])[j])[k])[m] = (((((forcing[_p_loop_index])[j])[k])[m]) - (dssp * ((((( *_ppthd_ue)[k - 2])[m]) - (4.0 * ((( *_ppthd_ue)[k - 1])[m]))) + (5.0 * ((( *_ppthd_ue)[k])[m])))));
        }
      }
    }
    _ompc_barrier();
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          for (m = 0; m < 5; m++) {
            (((forcing[_p_loop_index])[j])[k])[m] = ((-1.0) * ((((forcing[_p_loop_index])[j])[k])[m]));
          }
        }
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void exact_solution(double xi,double eta,double zeta,double *dtemp)
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     this function returns the exact solution at point xi, eta, zeta  
c-------------------------------------------------------------------*/
  int m;
  for (m = 0; m < 5; m++) {
    dtemp[m] = (((((ce[m])[0]) + (xi * (((ce[m])[1]) + (xi * (((ce[m])[4]) + (xi * (((ce[m])[7]) + (xi * ((ce[m])[10]))))))))) + (eta * (((ce[m])[2]) + (eta * (((ce[m])[5]) + (eta * (((ce[m])[8]) + (eta * ((ce[m])[11]))))))))) + (zeta * (((ce[m])[3]) + (zeta * (((ce[m])[6]) + (zeta * (((ce[m])[9]) + (zeta * ((ce[m])[12])))))))));
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void initialize()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     This subroutine initializes the field variable u using 
c     tri-linear transfinite interpolation of the boundary values     
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
  int m;
  int ix;
  int iy;
  int iz;
  double xi;
  double eta;
  double zeta;
  double Pface[2UL][3UL][5UL];
  double Pxi;
  double Peta;
  double Pzeta;
  double temp[5UL];
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = 12;
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 0; j < 12; j++) {
        for (k = 0; k < 12; k++) {
          for (m = 0; m < 5; m++) {
            (((u[_p_loop_index])[j])[k])[m] = 1.0;
          }
        }
      }
    }
    _ompc_barrier();
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[0]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      xi = (((double )_p_loop_index) * dnxm1);
      for (j = 0; j < (grid_points[1]); j++) {
        eta = (((double )j) * dnym1);
        for (k = 0; k < (grid_points[2]); k++) {
          zeta = (((double )k) * dnzm1);
          for (ix = 0; ix < 2; ix++) {
            exact_solution(((double )ix),eta,zeta,(((Pface[ix])[0]) + 0));
          }
          for (iy = 0; iy < 2; iy++) {
            exact_solution(xi,((double )iy),zeta,(((Pface[iy])[1]) + 0));
          }
          for (iz = 0; iz < 2; iz++) {
            exact_solution(xi,eta,((double )iz),(((Pface[iz])[2]) + 0));
          }
          for (m = 0; m < 5; m++) {
            Pxi = ((xi * (((Pface[1])[0])[m])) + ((1.0 - xi) * (((Pface[0])[0])[m])));
            Peta = ((eta * (((Pface[1])[1])[m])) + ((1.0 - eta) * (((Pface[0])[1])[m])));
            Pzeta = ((zeta * (((Pface[1])[2])[m])) + ((1.0 - zeta) * (((Pface[0])[2])[m])));
            (((u[_p_loop_index])[j])[k])[m] = ((((((Pxi + Peta) + Pzeta) - (Pxi * Peta)) - (Pxi * Pzeta)) - (Peta * Pzeta)) + ((Pxi * Peta) * Pzeta));
          }
        }
      }
    }
    _ompc_barrier();
  }
/*--------------------------------------------------------------------
c     now store the exact values on the boundaries        
c-------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     west face                                                  
c-------------------------------------------------------------------*/
  i = 0;
  xi = 0.0;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[1]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      eta = (((double )_p_loop_index) * dnym1);
      for (k = 0; k < (grid_points[2]); k++) {
        zeta = (((double )k) * dnzm1);
        exact_solution(xi,eta,zeta,temp);
        for (m = 0; m < 5; m++) {
          (((u[i])[_p_loop_index])[k])[m] = (temp[m]);
        }
      }
    }
  }
/*--------------------------------------------------------------------
c     east face                                                      
c-------------------------------------------------------------------*/
  i = ((grid_points[0]) - 1);
  xi = 1.0;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[1]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      eta = (((double )_p_loop_index) * dnym1);
      for (k = 0; k < (grid_points[2]); k++) {
        zeta = (((double )k) * dnzm1);
        exact_solution(xi,eta,zeta,temp);
        for (m = 0; m < 5; m++) {
          (((u[i])[_p_loop_index])[k])[m] = (temp[m]);
        }
      }
    }
    _ompc_barrier();
  }
/*--------------------------------------------------------------------
c     south face                                                 
c-------------------------------------------------------------------*/
  j = 0;
  eta = 0.0;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[0]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      xi = (((double )_p_loop_index) * dnxm1);
      for (k = 0; k < (grid_points[2]); k++) {
        zeta = (((double )k) * dnzm1);
        exact_solution(xi,eta,zeta,temp);
        for (m = 0; m < 5; m++) {
          (((u[_p_loop_index])[j])[k])[m] = (temp[m]);
        }
      }
    }
  }
/*--------------------------------------------------------------------
c     north face                                    
c-------------------------------------------------------------------*/
  j = ((grid_points[1]) - 1);
  eta = 1.0;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[0]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      xi = (((double )_p_loop_index) * dnxm1);
      for (k = 0; k < (grid_points[2]); k++) {
        zeta = (((double )k) * dnzm1);
        exact_solution(xi,eta,zeta,temp);
        for (m = 0; m < 5; m++) {
          (((u[_p_loop_index])[j])[k])[m] = (temp[m]);
        }
      }
    }
    _ompc_barrier();
  }
/*--------------------------------------------------------------------
c     bottom face                                       
c-------------------------------------------------------------------*/
  k = 0;
  zeta = 0.0;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[0]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      xi = (((double )_p_loop_index) * dnxm1);
      for (j = 0; j < (grid_points[1]); j++) {
        eta = (((double )j) * dnym1);
        exact_solution(xi,eta,zeta,temp);
        for (m = 0; m < 5; m++) {
          (((u[_p_loop_index])[j])[k])[m] = (temp[m]);
        }
      }
    }
  }
/*--------------------------------------------------------------------
c     top face     
c-------------------------------------------------------------------*/
  k = ((grid_points[2]) - 1);
  zeta = 1.0;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[0]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      xi = (((double )_p_loop_index) * dnxm1);
      for (j = 0; j < (grid_points[1]); j++) {
        eta = (((double )j) * dnym1);
        exact_solution(xi,eta,zeta,temp);
        for (m = 0; m < 5; m++) {
          (((u[_p_loop_index])[j])[k])[m] = (temp[m]);
        }
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void lhsinit()
{
  int i;
  int j;
  int k;
  int m;
  int n;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[0]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 0; j < (grid_points[1]); j++) {
        for (k = 0; k < (grid_points[2]); k++) {
          for (m = 0; m < 5; m++) {
            for (n = 0; n < 5; n++) {
              (((((lhs[_p_loop_index])[j])[k])[0])[m])[n] = 0.0;
              (((((lhs[_p_loop_index])[j])[k])[1])[m])[n] = 0.0;
              (((((lhs[_p_loop_index])[j])[k])[2])[m])[n] = 0.0;
            }
          }
        }
      }
    }
    _ompc_barrier();
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[0]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 0; j < (grid_points[1]); j++) {
        for (k = 0; k < (grid_points[2]); k++) {
          for (m = 0; m < 5; m++) {
            (((((lhs[_p_loop_index])[j])[k])[1])[m])[m] = 1.0;
          }
        }
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void lhsx()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     This function computes the left hand side in the xi-direction
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[1]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (i = 0; i < (grid_points[0]); i++) {
          tmp1 = (1.0 / ((((u[i])[_p_loop_index])[k])[0]));
          tmp2 = (tmp1 * tmp1);
          tmp3 = (tmp1 * tmp2);
/*--------------------------------------------------------------------
c     
c-------------------------------------------------------------------*/
          ((((fjac[i])[_p_loop_index])[k])[0])[0] = 0.0;
          ((((fjac[i])[_p_loop_index])[k])[0])[1] = 1.0;
          ((((fjac[i])[_p_loop_index])[k])[0])[2] = 0.0;
          ((((fjac[i])[_p_loop_index])[k])[0])[3] = 0.0;
          ((((fjac[i])[_p_loop_index])[k])[0])[4] = 0.0;
          ((((fjac[i])[_p_loop_index])[k])[1])[0] = ((-((((((u[i])[_p_loop_index])[k])[1]) * tmp2) * ((((u[i])[_p_loop_index])[k])[1]))) + (((c2 * 0.5) * (((((((u[i])[_p_loop_index])[k])[1]) * ((((u[i])[_p_loop_index])[k])[1])) + (((((u[i])[_p_loop_index])[k])[2]) * ((((u[i])[_p_loop_index])[k])[2]))) + (((((u[i])[_p_loop_index])[k])[3]) * ((((u[i])[_p_loop_index])[k])[3])))) * tmp2));
          ((((fjac[i])[_p_loop_index])[k])[1])[1] = ((2.0 - c2) * (((((u[i])[_p_loop_index])[k])[1]) / ((((u[i])[_p_loop_index])[k])[0])));
          ((((fjac[i])[_p_loop_index])[k])[1])[2] = ((-c2) * (((((u[i])[_p_loop_index])[k])[2]) * tmp1));
          ((((fjac[i])[_p_loop_index])[k])[1])[3] = ((-c2) * (((((u[i])[_p_loop_index])[k])[3]) * tmp1));
          ((((fjac[i])[_p_loop_index])[k])[1])[4] = c2;
          ((((fjac[i])[_p_loop_index])[k])[2])[0] = ((-(((((u[i])[_p_loop_index])[k])[1]) * ((((u[i])[_p_loop_index])[k])[2]))) * tmp2);
          ((((fjac[i])[_p_loop_index])[k])[2])[1] = (((((u[i])[_p_loop_index])[k])[2]) * tmp1);
          ((((fjac[i])[_p_loop_index])[k])[2])[2] = (((((u[i])[_p_loop_index])[k])[1]) * tmp1);
          ((((fjac[i])[_p_loop_index])[k])[2])[3] = 0.0;
          ((((fjac[i])[_p_loop_index])[k])[2])[4] = 0.0;
          ((((fjac[i])[_p_loop_index])[k])[3])[0] = ((-(((((u[i])[_p_loop_index])[k])[1]) * ((((u[i])[_p_loop_index])[k])[3]))) * tmp2);
          ((((fjac[i])[_p_loop_index])[k])[3])[1] = (((((u[i])[_p_loop_index])[k])[3]) * tmp1);
          ((((fjac[i])[_p_loop_index])[k])[3])[2] = 0.0;
          ((((fjac[i])[_p_loop_index])[k])[3])[3] = (((((u[i])[_p_loop_index])[k])[1]) * tmp1);
          ((((fjac[i])[_p_loop_index])[k])[3])[4] = 0.0;
          ((((fjac[i])[_p_loop_index])[k])[4])[0] = ((((c2 * (((((((u[i])[_p_loop_index])[k])[1]) * ((((u[i])[_p_loop_index])[k])[1])) + (((((u[i])[_p_loop_index])[k])[2]) * ((((u[i])[_p_loop_index])[k])[2]))) + (((((u[i])[_p_loop_index])[k])[3]) * ((((u[i])[_p_loop_index])[k])[3])))) * tmp2) - (c1 * (((((u[i])[_p_loop_index])[k])[4]) * tmp1))) * (((((u[i])[_p_loop_index])[k])[1]) * tmp1));
          ((((fjac[i])[_p_loop_index])[k])[4])[1] = (((c1 * ((((u[i])[_p_loop_index])[k])[4])) * tmp1) - (((0.5 * c2) * ((((3.0 * ((((u[i])[_p_loop_index])[k])[1])) * ((((u[i])[_p_loop_index])[k])[1])) + (((((u[i])[_p_loop_index])[k])[2]) * ((((u[i])[_p_loop_index])[k])[2]))) + (((((u[i])[_p_loop_index])[k])[3]) * ((((u[i])[_p_loop_index])[k])[3])))) * tmp2));
          ((((fjac[i])[_p_loop_index])[k])[4])[2] = (((-c2) * (((((u[i])[_p_loop_index])[k])[2]) * ((((u[i])[_p_loop_index])[k])[1]))) * tmp2);
          ((((fjac[i])[_p_loop_index])[k])[4])[3] = (((-c2) * (((((u[i])[_p_loop_index])[k])[3]) * ((((u[i])[_p_loop_index])[k])[1]))) * tmp2);
          ((((fjac[i])[_p_loop_index])[k])[4])[4] = (c1 * (((((u[i])[_p_loop_index])[k])[1]) * tmp1));
          ((((njac[i])[_p_loop_index])[k])[0])[0] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[0])[1] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[0])[2] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[0])[3] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[0])[4] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[1])[0] = ((((-con43) * c3c4) * tmp2) * ((((u[i])[_p_loop_index])[k])[1]));
          ((((njac[i])[_p_loop_index])[k])[1])[1] = ((con43 * c3c4) * tmp1);
          ((((njac[i])[_p_loop_index])[k])[1])[2] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[1])[3] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[1])[4] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[2])[0] = (((-c3c4) * tmp2) * ((((u[i])[_p_loop_index])[k])[2]));
          ((((njac[i])[_p_loop_index])[k])[2])[1] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[2])[2] = (c3c4 * tmp1);
          ((((njac[i])[_p_loop_index])[k])[2])[3] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[2])[4] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[3])[0] = (((-c3c4) * tmp2) * ((((u[i])[_p_loop_index])[k])[3]));
          ((((njac[i])[_p_loop_index])[k])[3])[1] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[3])[2] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[3])[3] = (c3c4 * tmp1);
          ((((njac[i])[_p_loop_index])[k])[3])[4] = 0.0;
          ((((njac[i])[_p_loop_index])[k])[4])[0] = ((((((-((con43 * c3c4) - c1345)) * tmp3) * (((((u[i])[_p_loop_index])[k])[1]) * ((((u[i])[_p_loop_index])[k])[1]))) - (((c3c4 - c1345) * tmp3) * (((((u[i])[_p_loop_index])[k])[2]) * ((((u[i])[_p_loop_index])[k])[2])))) - (((c3c4 - c1345) * tmp3) * (((((u[i])[_p_loop_index])[k])[3]) * ((((u[i])[_p_loop_index])[k])[3])))) - ((c1345 * tmp2) * ((((u[i])[_p_loop_index])[k])[4])));
          ((((njac[i])[_p_loop_index])[k])[4])[1] = ((((con43 * c3c4) - c1345) * tmp2) * ((((u[i])[_p_loop_index])[k])[1]));
          ((((njac[i])[_p_loop_index])[k])[4])[2] = (((c3c4 - c1345) * tmp2) * ((((u[i])[_p_loop_index])[k])[2]));
          ((((njac[i])[_p_loop_index])[k])[4])[3] = (((c3c4 - c1345) * tmp2) * ((((u[i])[_p_loop_index])[k])[3]));
          ((((njac[i])[_p_loop_index])[k])[4])[4] = (c1345 * tmp1);
        }
/*--------------------------------------------------------------------
c     now jacobians set, so form left hand side in x direction
c-------------------------------------------------------------------*/
        for (i = 1; i < ((grid_points[0]) - 1); i++) {
          tmp1 = (dt * tx1);
          tmp2 = (dt * tx2);
          (((((lhs[i])[_p_loop_index])[k])[0])[0])[0] = ((((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[0])[0])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[0])[0]))) - (tmp1 * dx1));
          (((((lhs[i])[_p_loop_index])[k])[0])[0])[1] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[0])[1])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[0])[1])));
          (((((lhs[i])[_p_loop_index])[k])[0])[0])[2] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[0])[2])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[0])[2])));
          (((((lhs[i])[_p_loop_index])[k])[0])[0])[3] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[0])[3])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[0])[3])));
          (((((lhs[i])[_p_loop_index])[k])[0])[0])[4] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[0])[4])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[0])[4])));
          (((((lhs[i])[_p_loop_index])[k])[0])[1])[0] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[1])[0])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[1])[0])));
          (((((lhs[i])[_p_loop_index])[k])[0])[1])[1] = ((((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[1])[1])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[1])[1]))) - (tmp1 * dx2));
          (((((lhs[i])[_p_loop_index])[k])[0])[1])[2] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[1])[2])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[1])[2])));
          (((((lhs[i])[_p_loop_index])[k])[0])[1])[3] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[1])[3])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[1])[3])));
          (((((lhs[i])[_p_loop_index])[k])[0])[1])[4] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[1])[4])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[1])[4])));
          (((((lhs[i])[_p_loop_index])[k])[0])[2])[0] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[2])[0])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[2])[0])));
          (((((lhs[i])[_p_loop_index])[k])[0])[2])[1] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[2])[1])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[2])[1])));
          (((((lhs[i])[_p_loop_index])[k])[0])[2])[2] = ((((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[2])[2])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[2])[2]))) - (tmp1 * dx3));
          (((((lhs[i])[_p_loop_index])[k])[0])[2])[3] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[2])[3])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[2])[3])));
          (((((lhs[i])[_p_loop_index])[k])[0])[2])[4] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[2])[4])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[2])[4])));
          (((((lhs[i])[_p_loop_index])[k])[0])[3])[0] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[3])[0])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[3])[0])));
          (((((lhs[i])[_p_loop_index])[k])[0])[3])[1] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[3])[1])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[3])[1])));
          (((((lhs[i])[_p_loop_index])[k])[0])[3])[2] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[3])[2])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[3])[2])));
          (((((lhs[i])[_p_loop_index])[k])[0])[3])[3] = ((((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[3])[3])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[3])[3]))) - (tmp1 * dx4));
          (((((lhs[i])[_p_loop_index])[k])[0])[3])[4] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[3])[4])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[3])[4])));
          (((((lhs[i])[_p_loop_index])[k])[0])[4])[0] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[4])[0])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[4])[0])));
          (((((lhs[i])[_p_loop_index])[k])[0])[4])[1] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[4])[1])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[4])[1])));
          (((((lhs[i])[_p_loop_index])[k])[0])[4])[2] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[4])[2])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[4])[2])));
          (((((lhs[i])[_p_loop_index])[k])[0])[4])[3] = (((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[4])[3])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[4])[3])));
          (((((lhs[i])[_p_loop_index])[k])[0])[4])[4] = ((((-tmp2) * (((((fjac[i - 1])[_p_loop_index])[k])[4])[4])) - (tmp1 * (((((njac[i - 1])[_p_loop_index])[k])[4])[4]))) - (tmp1 * dx5));
          (((((lhs[i])[_p_loop_index])[k])[1])[0])[0] = ((1.0 + ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[0])[0]))) + ((tmp1 * 2.0) * dx1));
          (((((lhs[i])[_p_loop_index])[k])[1])[0])[1] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[0])[1]));
          (((((lhs[i])[_p_loop_index])[k])[1])[0])[2] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[0])[2]));
          (((((lhs[i])[_p_loop_index])[k])[1])[0])[3] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[0])[3]));
          (((((lhs[i])[_p_loop_index])[k])[1])[0])[4] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[0])[4]));
          (((((lhs[i])[_p_loop_index])[k])[1])[1])[0] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[1])[0]));
          (((((lhs[i])[_p_loop_index])[k])[1])[1])[1] = ((1.0 + ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[1])[1]))) + ((tmp1 * 2.0) * dx2));
          (((((lhs[i])[_p_loop_index])[k])[1])[1])[2] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[1])[2]));
          (((((lhs[i])[_p_loop_index])[k])[1])[1])[3] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[1])[3]));
          (((((lhs[i])[_p_loop_index])[k])[1])[1])[4] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[1])[4]));
          (((((lhs[i])[_p_loop_index])[k])[1])[2])[0] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[2])[0]));
          (((((lhs[i])[_p_loop_index])[k])[1])[2])[1] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[2])[1]));
          (((((lhs[i])[_p_loop_index])[k])[1])[2])[2] = ((1.0 + ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[2])[2]))) + ((tmp1 * 2.0) * dx3));
          (((((lhs[i])[_p_loop_index])[k])[1])[2])[3] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[2])[3]));
          (((((lhs[i])[_p_loop_index])[k])[1])[2])[4] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[2])[4]));
          (((((lhs[i])[_p_loop_index])[k])[1])[3])[0] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[3])[0]));
          (((((lhs[i])[_p_loop_index])[k])[1])[3])[1] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[3])[1]));
          (((((lhs[i])[_p_loop_index])[k])[1])[3])[2] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[3])[2]));
          (((((lhs[i])[_p_loop_index])[k])[1])[3])[3] = ((1.0 + ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[3])[3]))) + ((tmp1 * 2.0) * dx4));
          (((((lhs[i])[_p_loop_index])[k])[1])[3])[4] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[3])[4]));
          (((((lhs[i])[_p_loop_index])[k])[1])[4])[0] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[4])[0]));
          (((((lhs[i])[_p_loop_index])[k])[1])[4])[1] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[4])[1]));
          (((((lhs[i])[_p_loop_index])[k])[1])[4])[2] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[4])[2]));
          (((((lhs[i])[_p_loop_index])[k])[1])[4])[3] = ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[4])[3]));
          (((((lhs[i])[_p_loop_index])[k])[1])[4])[4] = ((1.0 + ((tmp1 * 2.0) * (((((njac[i])[_p_loop_index])[k])[4])[4]))) + ((tmp1 * 2.0) * dx5));
          (((((lhs[i])[_p_loop_index])[k])[2])[0])[0] = (((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[0])[0])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[0])[0]))) - (tmp1 * dx1));
          (((((lhs[i])[_p_loop_index])[k])[2])[0])[1] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[0])[1])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[0])[1])));
          (((((lhs[i])[_p_loop_index])[k])[2])[0])[2] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[0])[2])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[0])[2])));
          (((((lhs[i])[_p_loop_index])[k])[2])[0])[3] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[0])[3])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[0])[3])));
          (((((lhs[i])[_p_loop_index])[k])[2])[0])[4] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[0])[4])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[0])[4])));
          (((((lhs[i])[_p_loop_index])[k])[2])[1])[0] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[1])[0])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[1])[0])));
          (((((lhs[i])[_p_loop_index])[k])[2])[1])[1] = (((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[1])[1])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[1])[1]))) - (tmp1 * dx2));
          (((((lhs[i])[_p_loop_index])[k])[2])[1])[2] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[1])[2])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[1])[2])));
          (((((lhs[i])[_p_loop_index])[k])[2])[1])[3] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[1])[3])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[1])[3])));
          (((((lhs[i])[_p_loop_index])[k])[2])[1])[4] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[1])[4])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[1])[4])));
          (((((lhs[i])[_p_loop_index])[k])[2])[2])[0] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[2])[0])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[2])[0])));
          (((((lhs[i])[_p_loop_index])[k])[2])[2])[1] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[2])[1])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[2])[1])));
          (((((lhs[i])[_p_loop_index])[k])[2])[2])[2] = (((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[2])[2])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[2])[2]))) - (tmp1 * dx3));
          (((((lhs[i])[_p_loop_index])[k])[2])[2])[3] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[2])[3])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[2])[3])));
          (((((lhs[i])[_p_loop_index])[k])[2])[2])[4] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[2])[4])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[2])[4])));
          (((((lhs[i])[_p_loop_index])[k])[2])[3])[0] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[3])[0])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[3])[0])));
          (((((lhs[i])[_p_loop_index])[k])[2])[3])[1] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[3])[1])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[3])[1])));
          (((((lhs[i])[_p_loop_index])[k])[2])[3])[2] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[3])[2])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[3])[2])));
          (((((lhs[i])[_p_loop_index])[k])[2])[3])[3] = (((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[3])[3])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[3])[3]))) - (tmp1 * dx4));
          (((((lhs[i])[_p_loop_index])[k])[2])[3])[4] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[3])[4])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[3])[4])));
          (((((lhs[i])[_p_loop_index])[k])[2])[4])[0] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[4])[0])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[4])[0])));
          (((((lhs[i])[_p_loop_index])[k])[2])[4])[1] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[4])[1])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[4])[1])));
          (((((lhs[i])[_p_loop_index])[k])[2])[4])[2] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[4])[2])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[4])[2])));
          (((((lhs[i])[_p_loop_index])[k])[2])[4])[3] = ((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[4])[3])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[4])[3])));
          (((((lhs[i])[_p_loop_index])[k])[2])[4])[4] = (((tmp2 * (((((fjac[i + 1])[_p_loop_index])[k])[4])[4])) - (tmp1 * (((((njac[i + 1])[_p_loop_index])[k])[4])[4]))) - (tmp1 * dx5));
        }
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void lhsy()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     This function computes the left hand side for the three y-factors   
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 0; j < (grid_points[1]); j++) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          tmp1 = (1.0 / ((((u[_p_loop_index])[j])[k])[0]));
          tmp2 = (tmp1 * tmp1);
          tmp3 = (tmp1 * tmp2);
          ((((fjac[_p_loop_index])[j])[k])[0])[0] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[0])[1] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[0])[2] = 1.0;
          ((((fjac[_p_loop_index])[j])[k])[0])[3] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[0])[4] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[1])[0] = ((-(((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[2]))) * tmp2);
          ((((fjac[_p_loop_index])[j])[k])[1])[1] = (((((u[_p_loop_index])[j])[k])[2]) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[1])[2] = (((((u[_p_loop_index])[j])[k])[1]) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[1])[3] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[1])[4] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[2])[0] = ((-((((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[2])) * tmp2)) + ((0.5 * c2) * ((((((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[1])) + (((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[2]))) + (((((u[_p_loop_index])[j])[k])[3]) * ((((u[_p_loop_index])[j])[k])[3]))) * tmp2)));
          ((((fjac[_p_loop_index])[j])[k])[2])[1] = (((-c2) * ((((u[_p_loop_index])[j])[k])[1])) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[2])[2] = (((2.0 - c2) * ((((u[_p_loop_index])[j])[k])[2])) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[2])[3] = (((-c2) * ((((u[_p_loop_index])[j])[k])[3])) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[2])[4] = c2;
          ((((fjac[_p_loop_index])[j])[k])[3])[0] = ((-(((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[3]))) * tmp2);
          ((((fjac[_p_loop_index])[j])[k])[3])[1] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[3])[2] = (((((u[_p_loop_index])[j])[k])[3]) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[3])[3] = (((((u[_p_loop_index])[j])[k])[2]) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[3])[4] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[4])[0] = (((((c2 * (((((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[1])) + (((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[2]))) + (((((u[_p_loop_index])[j])[k])[3]) * ((((u[_p_loop_index])[j])[k])[3])))) * tmp2) - ((c1 * ((((u[_p_loop_index])[j])[k])[4])) * tmp1)) * ((((u[_p_loop_index])[j])[k])[2])) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[4])[1] = ((((-c2) * ((((u[_p_loop_index])[j])[k])[1])) * ((((u[_p_loop_index])[j])[k])[2])) * tmp2);
          ((((fjac[_p_loop_index])[j])[k])[4])[2] = (((c1 * ((((u[_p_loop_index])[j])[k])[4])) * tmp1) - ((0.5 * c2) * ((((((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[1])) + ((3.0 * ((((u[_p_loop_index])[j])[k])[2])) * ((((u[_p_loop_index])[j])[k])[2]))) + (((((u[_p_loop_index])[j])[k])[3]) * ((((u[_p_loop_index])[j])[k])[3]))) * tmp2)));
          ((((fjac[_p_loop_index])[j])[k])[4])[3] = (((-c2) * (((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[3]))) * tmp2);
          ((((fjac[_p_loop_index])[j])[k])[4])[4] = ((c1 * ((((u[_p_loop_index])[j])[k])[2])) * tmp1);
          ((((njac[_p_loop_index])[j])[k])[0])[0] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[0])[1] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[0])[2] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[0])[3] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[0])[4] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[1])[0] = (((-c3c4) * tmp2) * ((((u[_p_loop_index])[j])[k])[1]));
          ((((njac[_p_loop_index])[j])[k])[1])[1] = (c3c4 * tmp1);
          ((((njac[_p_loop_index])[j])[k])[1])[2] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[1])[3] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[1])[4] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[2])[0] = ((((-con43) * c3c4) * tmp2) * ((((u[_p_loop_index])[j])[k])[2]));
          ((((njac[_p_loop_index])[j])[k])[2])[1] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[2])[2] = ((con43 * c3c4) * tmp1);
          ((((njac[_p_loop_index])[j])[k])[2])[3] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[2])[4] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[3])[0] = (((-c3c4) * tmp2) * ((((u[_p_loop_index])[j])[k])[3]));
          ((((njac[_p_loop_index])[j])[k])[3])[1] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[3])[2] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[3])[3] = (c3c4 * tmp1);
          ((((njac[_p_loop_index])[j])[k])[3])[4] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[4])[0] = ((((((-(c3c4 - c1345)) * tmp3) * (((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[1]))) - ((((con43 * c3c4) - c1345) * tmp3) * (((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[2])))) - (((c3c4 - c1345) * tmp3) * (((((u[_p_loop_index])[j])[k])[3]) * ((((u[_p_loop_index])[j])[k])[3])))) - ((c1345 * tmp2) * ((((u[_p_loop_index])[j])[k])[4])));
          ((((njac[_p_loop_index])[j])[k])[4])[1] = (((c3c4 - c1345) * tmp2) * ((((u[_p_loop_index])[j])[k])[1]));
          ((((njac[_p_loop_index])[j])[k])[4])[2] = ((((con43 * c3c4) - c1345) * tmp2) * ((((u[_p_loop_index])[j])[k])[2]));
          ((((njac[_p_loop_index])[j])[k])[4])[3] = (((c3c4 - c1345) * tmp2) * ((((u[_p_loop_index])[j])[k])[3]));
          ((((njac[_p_loop_index])[j])[k])[4])[4] = (c1345 * tmp1);
        }
      }
    }
    _ompc_barrier();
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          tmp1 = (dt * ty1);
          tmp2 = (dt * ty2);
          (((((lhs[_p_loop_index])[j])[k])[0])[0])[0] = ((((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[0])[0])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[0])[0]))) - (tmp1 * dy1));
          (((((lhs[_p_loop_index])[j])[k])[0])[0])[1] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[0])[1])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[0])[1])));
          (((((lhs[_p_loop_index])[j])[k])[0])[0])[2] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[0])[2])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[0])[2])));
          (((((lhs[_p_loop_index])[j])[k])[0])[0])[3] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[0])[3])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[0])[3])));
          (((((lhs[_p_loop_index])[j])[k])[0])[0])[4] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[0])[4])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[0])[4])));
          (((((lhs[_p_loop_index])[j])[k])[0])[1])[0] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[1])[0])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[1])[0])));
          (((((lhs[_p_loop_index])[j])[k])[0])[1])[1] = ((((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[1])[1])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[1])[1]))) - (tmp1 * dy2));
          (((((lhs[_p_loop_index])[j])[k])[0])[1])[2] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[1])[2])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[1])[2])));
          (((((lhs[_p_loop_index])[j])[k])[0])[1])[3] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[1])[3])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[1])[3])));
          (((((lhs[_p_loop_index])[j])[k])[0])[1])[4] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[1])[4])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[1])[4])));
          (((((lhs[_p_loop_index])[j])[k])[0])[2])[0] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[2])[0])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[2])[0])));
          (((((lhs[_p_loop_index])[j])[k])[0])[2])[1] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[2])[1])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[2])[1])));
          (((((lhs[_p_loop_index])[j])[k])[0])[2])[2] = ((((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[2])[2])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[2])[2]))) - (tmp1 * dy3));
          (((((lhs[_p_loop_index])[j])[k])[0])[2])[3] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[2])[3])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[2])[3])));
          (((((lhs[_p_loop_index])[j])[k])[0])[2])[4] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[2])[4])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[2])[4])));
          (((((lhs[_p_loop_index])[j])[k])[0])[3])[0] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[3])[0])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[3])[0])));
          (((((lhs[_p_loop_index])[j])[k])[0])[3])[1] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[3])[1])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[3])[1])));
          (((((lhs[_p_loop_index])[j])[k])[0])[3])[2] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[3])[2])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[3])[2])));
          (((((lhs[_p_loop_index])[j])[k])[0])[3])[3] = ((((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[3])[3])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[3])[3]))) - (tmp1 * dy4));
          (((((lhs[_p_loop_index])[j])[k])[0])[3])[4] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[3])[4])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[3])[4])));
          (((((lhs[_p_loop_index])[j])[k])[0])[4])[0] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[4])[0])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[4])[0])));
          (((((lhs[_p_loop_index])[j])[k])[0])[4])[1] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[4])[1])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[4])[1])));
          (((((lhs[_p_loop_index])[j])[k])[0])[4])[2] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[4])[2])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[4])[2])));
          (((((lhs[_p_loop_index])[j])[k])[0])[4])[3] = (((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[4])[3])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[4])[3])));
          (((((lhs[_p_loop_index])[j])[k])[0])[4])[4] = ((((-tmp2) * (((((fjac[_p_loop_index])[j - 1])[k])[4])[4])) - (tmp1 * (((((njac[_p_loop_index])[j - 1])[k])[4])[4]))) - (tmp1 * dy5));
          (((((lhs[_p_loop_index])[j])[k])[1])[0])[0] = ((1.0 + ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[0])[0]))) + ((tmp1 * 2.0) * dy1));
          (((((lhs[_p_loop_index])[j])[k])[1])[0])[1] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[0])[1]));
          (((((lhs[_p_loop_index])[j])[k])[1])[0])[2] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[0])[2]));
          (((((lhs[_p_loop_index])[j])[k])[1])[0])[3] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[0])[3]));
          (((((lhs[_p_loop_index])[j])[k])[1])[0])[4] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[0])[4]));
          (((((lhs[_p_loop_index])[j])[k])[1])[1])[0] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[1])[0]));
          (((((lhs[_p_loop_index])[j])[k])[1])[1])[1] = ((1.0 + ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[1])[1]))) + ((tmp1 * 2.0) * dy2));
          (((((lhs[_p_loop_index])[j])[k])[1])[1])[2] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[1])[2]));
          (((((lhs[_p_loop_index])[j])[k])[1])[1])[3] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[1])[3]));
          (((((lhs[_p_loop_index])[j])[k])[1])[1])[4] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[1])[4]));
          (((((lhs[_p_loop_index])[j])[k])[1])[2])[0] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[2])[0]));
          (((((lhs[_p_loop_index])[j])[k])[1])[2])[1] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[2])[1]));
          (((((lhs[_p_loop_index])[j])[k])[1])[2])[2] = ((1.0 + ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[2])[2]))) + ((tmp1 * 2.0) * dy3));
          (((((lhs[_p_loop_index])[j])[k])[1])[2])[3] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[2])[3]));
          (((((lhs[_p_loop_index])[j])[k])[1])[2])[4] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[2])[4]));
          (((((lhs[_p_loop_index])[j])[k])[1])[3])[0] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[3])[0]));
          (((((lhs[_p_loop_index])[j])[k])[1])[3])[1] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[3])[1]));
          (((((lhs[_p_loop_index])[j])[k])[1])[3])[2] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[3])[2]));
          (((((lhs[_p_loop_index])[j])[k])[1])[3])[3] = ((1.0 + ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[3])[3]))) + ((tmp1 * 2.0) * dy4));
          (((((lhs[_p_loop_index])[j])[k])[1])[3])[4] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[3])[4]));
          (((((lhs[_p_loop_index])[j])[k])[1])[4])[0] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[4])[0]));
          (((((lhs[_p_loop_index])[j])[k])[1])[4])[1] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[4])[1]));
          (((((lhs[_p_loop_index])[j])[k])[1])[4])[2] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[4])[2]));
          (((((lhs[_p_loop_index])[j])[k])[1])[4])[3] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[4])[3]));
          (((((lhs[_p_loop_index])[j])[k])[1])[4])[4] = ((1.0 + ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[4])[4]))) + ((tmp1 * 2.0) * dy5));
          (((((lhs[_p_loop_index])[j])[k])[2])[0])[0] = (((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[0])[0])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[0])[0]))) - (tmp1 * dy1));
          (((((lhs[_p_loop_index])[j])[k])[2])[0])[1] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[0])[1])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[0])[1])));
          (((((lhs[_p_loop_index])[j])[k])[2])[0])[2] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[0])[2])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[0])[2])));
          (((((lhs[_p_loop_index])[j])[k])[2])[0])[3] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[0])[3])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[0])[3])));
          (((((lhs[_p_loop_index])[j])[k])[2])[0])[4] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[0])[4])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[0])[4])));
          (((((lhs[_p_loop_index])[j])[k])[2])[1])[0] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[1])[0])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[1])[0])));
          (((((lhs[_p_loop_index])[j])[k])[2])[1])[1] = (((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[1])[1])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[1])[1]))) - (tmp1 * dy2));
          (((((lhs[_p_loop_index])[j])[k])[2])[1])[2] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[1])[2])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[1])[2])));
          (((((lhs[_p_loop_index])[j])[k])[2])[1])[3] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[1])[3])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[1])[3])));
          (((((lhs[_p_loop_index])[j])[k])[2])[1])[4] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[1])[4])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[1])[4])));
          (((((lhs[_p_loop_index])[j])[k])[2])[2])[0] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[2])[0])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[2])[0])));
          (((((lhs[_p_loop_index])[j])[k])[2])[2])[1] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[2])[1])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[2])[1])));
          (((((lhs[_p_loop_index])[j])[k])[2])[2])[2] = (((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[2])[2])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[2])[2]))) - (tmp1 * dy3));
          (((((lhs[_p_loop_index])[j])[k])[2])[2])[3] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[2])[3])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[2])[3])));
          (((((lhs[_p_loop_index])[j])[k])[2])[2])[4] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[2])[4])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[2])[4])));
          (((((lhs[_p_loop_index])[j])[k])[2])[3])[0] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[3])[0])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[3])[0])));
          (((((lhs[_p_loop_index])[j])[k])[2])[3])[1] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[3])[1])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[3])[1])));
          (((((lhs[_p_loop_index])[j])[k])[2])[3])[2] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[3])[2])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[3])[2])));
          (((((lhs[_p_loop_index])[j])[k])[2])[3])[3] = (((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[3])[3])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[3])[3]))) - (tmp1 * dy4));
          (((((lhs[_p_loop_index])[j])[k])[2])[3])[4] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[3])[4])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[3])[4])));
          (((((lhs[_p_loop_index])[j])[k])[2])[4])[0] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[4])[0])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[4])[0])));
          (((((lhs[_p_loop_index])[j])[k])[2])[4])[1] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[4])[1])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[4])[1])));
          (((((lhs[_p_loop_index])[j])[k])[2])[4])[2] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[4])[2])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[4])[2])));
          (((((lhs[_p_loop_index])[j])[k])[2])[4])[3] = ((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[4])[3])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[4])[3])));
          (((((lhs[_p_loop_index])[j])[k])[2])[4])[4] = (((tmp2 * (((((fjac[_p_loop_index])[j + 1])[k])[4])[4])) - (tmp1 * (((((njac[_p_loop_index])[j + 1])[k])[4])[4]))) - (tmp1 * dy5));
        }
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void lhsz()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     This function computes the left hand side for the three z-factors   
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = 0; k < (grid_points[2]); k++) {
          tmp1 = (1.0 / ((((u[_p_loop_index])[j])[k])[0]));
          tmp2 = (tmp1 * tmp1);
          tmp3 = (tmp1 * tmp2);
          ((((fjac[_p_loop_index])[j])[k])[0])[0] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[0])[1] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[0])[2] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[0])[3] = 1.0;
          ((((fjac[_p_loop_index])[j])[k])[0])[4] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[1])[0] = ((-(((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[3]))) * tmp2);
          ((((fjac[_p_loop_index])[j])[k])[1])[1] = (((((u[_p_loop_index])[j])[k])[3]) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[1])[2] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[1])[3] = (((((u[_p_loop_index])[j])[k])[1]) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[1])[4] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[2])[0] = ((-(((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[3]))) * tmp2);
          ((((fjac[_p_loop_index])[j])[k])[2])[1] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[2])[2] = (((((u[_p_loop_index])[j])[k])[3]) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[2])[3] = (((((u[_p_loop_index])[j])[k])[2]) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[2])[4] = 0.0;
          ((((fjac[_p_loop_index])[j])[k])[3])[0] = ((-((((((u[_p_loop_index])[j])[k])[3]) * ((((u[_p_loop_index])[j])[k])[3])) * tmp2)) + ((0.5 * c2) * ((((((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[1])) + (((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[2]))) + (((((u[_p_loop_index])[j])[k])[3]) * ((((u[_p_loop_index])[j])[k])[3]))) * tmp2)));
          ((((fjac[_p_loop_index])[j])[k])[3])[1] = (((-c2) * ((((u[_p_loop_index])[j])[k])[1])) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[3])[2] = (((-c2) * ((((u[_p_loop_index])[j])[k])[2])) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[3])[3] = (((2.0 - c2) * ((((u[_p_loop_index])[j])[k])[3])) * tmp1);
          ((((fjac[_p_loop_index])[j])[k])[3])[4] = c2;
          ((((fjac[_p_loop_index])[j])[k])[4])[0] = ((((c2 * (((((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[1])) + (((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[2]))) + (((((u[_p_loop_index])[j])[k])[3]) * ((((u[_p_loop_index])[j])[k])[3])))) * tmp2) - (c1 * (((((u[_p_loop_index])[j])[k])[4]) * tmp1))) * (((((u[_p_loop_index])[j])[k])[3]) * tmp1));
          ((((fjac[_p_loop_index])[j])[k])[4])[1] = (((-c2) * (((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[3]))) * tmp2);
          ((((fjac[_p_loop_index])[j])[k])[4])[2] = (((-c2) * (((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[3]))) * tmp2);
          ((((fjac[_p_loop_index])[j])[k])[4])[3] = ((c1 * (((((u[_p_loop_index])[j])[k])[4]) * tmp1)) - ((0.5 * c2) * ((((((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[1])) + (((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[2]))) + ((3.0 * ((((u[_p_loop_index])[j])[k])[3])) * ((((u[_p_loop_index])[j])[k])[3]))) * tmp2)));
          ((((fjac[_p_loop_index])[j])[k])[4])[4] = ((c1 * ((((u[_p_loop_index])[j])[k])[3])) * tmp1);
          ((((njac[_p_loop_index])[j])[k])[0])[0] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[0])[1] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[0])[2] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[0])[3] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[0])[4] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[1])[0] = (((-c3c4) * tmp2) * ((((u[_p_loop_index])[j])[k])[1]));
          ((((njac[_p_loop_index])[j])[k])[1])[1] = (c3c4 * tmp1);
          ((((njac[_p_loop_index])[j])[k])[1])[2] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[1])[3] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[1])[4] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[2])[0] = (((-c3c4) * tmp2) * ((((u[_p_loop_index])[j])[k])[2]));
          ((((njac[_p_loop_index])[j])[k])[2])[1] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[2])[2] = (c3c4 * tmp1);
          ((((njac[_p_loop_index])[j])[k])[2])[3] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[2])[4] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[3])[0] = ((((-con43) * c3c4) * tmp2) * ((((u[_p_loop_index])[j])[k])[3]));
          ((((njac[_p_loop_index])[j])[k])[3])[1] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[3])[2] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[3])[3] = (((con43 * c3) * c4) * tmp1);
          ((((njac[_p_loop_index])[j])[k])[3])[4] = 0.0;
          ((((njac[_p_loop_index])[j])[k])[4])[0] = ((((((-(c3c4 - c1345)) * tmp3) * (((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[1]))) - (((c3c4 - c1345) * tmp3) * (((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[2])))) - ((((con43 * c3c4) - c1345) * tmp3) * (((((u[_p_loop_index])[j])[k])[3]) * ((((u[_p_loop_index])[j])[k])[3])))) - ((c1345 * tmp2) * ((((u[_p_loop_index])[j])[k])[4])));
          ((((njac[_p_loop_index])[j])[k])[4])[1] = (((c3c4 - c1345) * tmp2) * ((((u[_p_loop_index])[j])[k])[1]));
          ((((njac[_p_loop_index])[j])[k])[4])[2] = (((c3c4 - c1345) * tmp2) * ((((u[_p_loop_index])[j])[k])[2]));
          ((((njac[_p_loop_index])[j])[k])[4])[3] = ((((con43 * c3c4) - c1345) * tmp2) * ((((u[_p_loop_index])[j])[k])[3]));
          ((((njac[_p_loop_index])[j])[k])[4])[4] = (c1345 * tmp1);
        }
      }
    }
    _ompc_barrier();
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          tmp1 = (dt * tz1);
          tmp2 = (dt * tz2);
          (((((lhs[_p_loop_index])[j])[k])[0])[0])[0] = ((((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[0])[0])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[0])[0]))) - (tmp1 * dz1));
          (((((lhs[_p_loop_index])[j])[k])[0])[0])[1] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[0])[1])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[0])[1])));
          (((((lhs[_p_loop_index])[j])[k])[0])[0])[2] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[0])[2])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[0])[2])));
          (((((lhs[_p_loop_index])[j])[k])[0])[0])[3] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[0])[3])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[0])[3])));
          (((((lhs[_p_loop_index])[j])[k])[0])[0])[4] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[0])[4])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[0])[4])));
          (((((lhs[_p_loop_index])[j])[k])[0])[1])[0] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[1])[0])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[1])[0])));
          (((((lhs[_p_loop_index])[j])[k])[0])[1])[1] = ((((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[1])[1])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[1])[1]))) - (tmp1 * dz2));
          (((((lhs[_p_loop_index])[j])[k])[0])[1])[2] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[1])[2])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[1])[2])));
          (((((lhs[_p_loop_index])[j])[k])[0])[1])[3] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[1])[3])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[1])[3])));
          (((((lhs[_p_loop_index])[j])[k])[0])[1])[4] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[1])[4])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[1])[4])));
          (((((lhs[_p_loop_index])[j])[k])[0])[2])[0] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[2])[0])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[2])[0])));
          (((((lhs[_p_loop_index])[j])[k])[0])[2])[1] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[2])[1])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[2])[1])));
          (((((lhs[_p_loop_index])[j])[k])[0])[2])[2] = ((((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[2])[2])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[2])[2]))) - (tmp1 * dz3));
          (((((lhs[_p_loop_index])[j])[k])[0])[2])[3] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[2])[3])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[2])[3])));
          (((((lhs[_p_loop_index])[j])[k])[0])[2])[4] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[2])[4])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[2])[4])));
          (((((lhs[_p_loop_index])[j])[k])[0])[3])[0] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[3])[0])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[3])[0])));
          (((((lhs[_p_loop_index])[j])[k])[0])[3])[1] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[3])[1])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[3])[1])));
          (((((lhs[_p_loop_index])[j])[k])[0])[3])[2] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[3])[2])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[3])[2])));
          (((((lhs[_p_loop_index])[j])[k])[0])[3])[3] = ((((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[3])[3])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[3])[3]))) - (tmp1 * dz4));
          (((((lhs[_p_loop_index])[j])[k])[0])[3])[4] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[3])[4])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[3])[4])));
          (((((lhs[_p_loop_index])[j])[k])[0])[4])[0] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[4])[0])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[4])[0])));
          (((((lhs[_p_loop_index])[j])[k])[0])[4])[1] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[4])[1])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[4])[1])));
          (((((lhs[_p_loop_index])[j])[k])[0])[4])[2] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[4])[2])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[4])[2])));
          (((((lhs[_p_loop_index])[j])[k])[0])[4])[3] = (((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[4])[3])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[4])[3])));
          (((((lhs[_p_loop_index])[j])[k])[0])[4])[4] = ((((-tmp2) * (((((fjac[_p_loop_index])[j])[k - 1])[4])[4])) - (tmp1 * (((((njac[_p_loop_index])[j])[k - 1])[4])[4]))) - (tmp1 * dz5));
          (((((lhs[_p_loop_index])[j])[k])[1])[0])[0] = ((1.0 + ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[0])[0]))) + ((tmp1 * 2.0) * dz1));
          (((((lhs[_p_loop_index])[j])[k])[1])[0])[1] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[0])[1]));
          (((((lhs[_p_loop_index])[j])[k])[1])[0])[2] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[0])[2]));
          (((((lhs[_p_loop_index])[j])[k])[1])[0])[3] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[0])[3]));
          (((((lhs[_p_loop_index])[j])[k])[1])[0])[4] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[0])[4]));
          (((((lhs[_p_loop_index])[j])[k])[1])[1])[0] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[1])[0]));
          (((((lhs[_p_loop_index])[j])[k])[1])[1])[1] = ((1.0 + ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[1])[1]))) + ((tmp1 * 2.0) * dz2));
          (((((lhs[_p_loop_index])[j])[k])[1])[1])[2] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[1])[2]));
          (((((lhs[_p_loop_index])[j])[k])[1])[1])[3] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[1])[3]));
          (((((lhs[_p_loop_index])[j])[k])[1])[1])[4] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[1])[4]));
          (((((lhs[_p_loop_index])[j])[k])[1])[2])[0] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[2])[0]));
          (((((lhs[_p_loop_index])[j])[k])[1])[2])[1] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[2])[1]));
          (((((lhs[_p_loop_index])[j])[k])[1])[2])[2] = ((1.0 + ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[2])[2]))) + ((tmp1 * 2.0) * dz3));
          (((((lhs[_p_loop_index])[j])[k])[1])[2])[3] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[2])[3]));
          (((((lhs[_p_loop_index])[j])[k])[1])[2])[4] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[2])[4]));
          (((((lhs[_p_loop_index])[j])[k])[1])[3])[0] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[3])[0]));
          (((((lhs[_p_loop_index])[j])[k])[1])[3])[1] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[3])[1]));
          (((((lhs[_p_loop_index])[j])[k])[1])[3])[2] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[3])[2]));
          (((((lhs[_p_loop_index])[j])[k])[1])[3])[3] = ((1.0 + ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[3])[3]))) + ((tmp1 * 2.0) * dz4));
          (((((lhs[_p_loop_index])[j])[k])[1])[3])[4] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[3])[4]));
          (((((lhs[_p_loop_index])[j])[k])[1])[4])[0] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[4])[0]));
          (((((lhs[_p_loop_index])[j])[k])[1])[4])[1] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[4])[1]));
          (((((lhs[_p_loop_index])[j])[k])[1])[4])[2] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[4])[2]));
          (((((lhs[_p_loop_index])[j])[k])[1])[4])[3] = ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[4])[3]));
          (((((lhs[_p_loop_index])[j])[k])[1])[4])[4] = ((1.0 + ((tmp1 * 2.0) * (((((njac[_p_loop_index])[j])[k])[4])[4]))) + ((tmp1 * 2.0) * dz5));
          (((((lhs[_p_loop_index])[j])[k])[2])[0])[0] = (((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[0])[0])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[0])[0]))) - (tmp1 * dz1));
          (((((lhs[_p_loop_index])[j])[k])[2])[0])[1] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[0])[1])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[0])[1])));
          (((((lhs[_p_loop_index])[j])[k])[2])[0])[2] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[0])[2])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[0])[2])));
          (((((lhs[_p_loop_index])[j])[k])[2])[0])[3] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[0])[3])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[0])[3])));
          (((((lhs[_p_loop_index])[j])[k])[2])[0])[4] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[0])[4])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[0])[4])));
          (((((lhs[_p_loop_index])[j])[k])[2])[1])[0] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[1])[0])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[1])[0])));
          (((((lhs[_p_loop_index])[j])[k])[2])[1])[1] = (((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[1])[1])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[1])[1]))) - (tmp1 * dz2));
          (((((lhs[_p_loop_index])[j])[k])[2])[1])[2] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[1])[2])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[1])[2])));
          (((((lhs[_p_loop_index])[j])[k])[2])[1])[3] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[1])[3])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[1])[3])));
          (((((lhs[_p_loop_index])[j])[k])[2])[1])[4] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[1])[4])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[1])[4])));
          (((((lhs[_p_loop_index])[j])[k])[2])[2])[0] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[2])[0])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[2])[0])));
          (((((lhs[_p_loop_index])[j])[k])[2])[2])[1] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[2])[1])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[2])[1])));
          (((((lhs[_p_loop_index])[j])[k])[2])[2])[2] = (((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[2])[2])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[2])[2]))) - (tmp1 * dz3));
          (((((lhs[_p_loop_index])[j])[k])[2])[2])[3] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[2])[3])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[2])[3])));
          (((((lhs[_p_loop_index])[j])[k])[2])[2])[4] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[2])[4])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[2])[4])));
          (((((lhs[_p_loop_index])[j])[k])[2])[3])[0] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[3])[0])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[3])[0])));
          (((((lhs[_p_loop_index])[j])[k])[2])[3])[1] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[3])[1])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[3])[1])));
          (((((lhs[_p_loop_index])[j])[k])[2])[3])[2] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[3])[2])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[3])[2])));
          (((((lhs[_p_loop_index])[j])[k])[2])[3])[3] = (((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[3])[3])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[3])[3]))) - (tmp1 * dz4));
          (((((lhs[_p_loop_index])[j])[k])[2])[3])[4] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[3])[4])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[3])[4])));
          (((((lhs[_p_loop_index])[j])[k])[2])[4])[0] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[4])[0])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[4])[0])));
          (((((lhs[_p_loop_index])[j])[k])[2])[4])[1] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[4])[1])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[4])[1])));
          (((((lhs[_p_loop_index])[j])[k])[2])[4])[2] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[4])[2])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[4])[2])));
          (((((lhs[_p_loop_index])[j])[k])[2])[4])[3] = ((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[4])[3])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[4])[3])));
          (((((lhs[_p_loop_index])[j])[k])[2])[4])[4] = (((tmp2 * (((((fjac[_p_loop_index])[j])[k + 1])[4])[4])) - (tmp1 * (((((njac[_p_loop_index])[j])[k + 1])[4])[4]))) - (tmp1 * dz5));
        }
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void compute_rhs()
{
  int i;
  int j;
  int k;
  int m;
  double rho_inv;
  double uijk;
  double up1;
  double um1;
  double vijk;
  double vp1;
  double vm1;
  double wijk;
  double wp1;
  double wm1;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[0]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 0; j < (grid_points[1]); j++) {
        for (k = 0; k < (grid_points[2]); k++) {
          rho_inv = (1.0 / ((((u[_p_loop_index])[j])[k])[0]));
          ((rho_i[_p_loop_index])[j])[k] = rho_inv;
          ((us[_p_loop_index])[j])[k] = (((((u[_p_loop_index])[j])[k])[1]) * rho_inv);
          ((vs[_p_loop_index])[j])[k] = (((((u[_p_loop_index])[j])[k])[2]) * rho_inv);
          ((ws[_p_loop_index])[j])[k] = (((((u[_p_loop_index])[j])[k])[3]) * rho_inv);
          ((square[_p_loop_index])[j])[k] = ((0.5 * (((((((u[_p_loop_index])[j])[k])[1]) * ((((u[_p_loop_index])[j])[k])[1])) + (((((u[_p_loop_index])[j])[k])[2]) * ((((u[_p_loop_index])[j])[k])[2]))) + (((((u[_p_loop_index])[j])[k])[3]) * ((((u[_p_loop_index])[j])[k])[3])))) * rho_inv);
          ((qs[_p_loop_index])[j])[k] = ((((square[_p_loop_index])[j])[k]) * rho_inv);
        }
      }
    }
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 0;
    _p_loop_upper = (grid_points[0]);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 0; j < (grid_points[1]); j++) {
        for (k = 0; k < (grid_points[2]); k++) {
          for (m = 0; m < 5; m++) {
            (((rhs[_p_loop_index])[j])[k])[m] = ((((forcing[_p_loop_index])[j])[k])[m]);
          }
        }
      }
    }
    _ompc_barrier();
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          uijk = (((us[_p_loop_index])[j])[k]);
          up1 = (((us[_p_loop_index + 1])[j])[k]);
          um1 = (((us[_p_loop_index - 1])[j])[k]);
          (((rhs[_p_loop_index])[j])[k])[0] = ((((((rhs[_p_loop_index])[j])[k])[0]) + (dx1tx1 * ((((((u[_p_loop_index + 1])[j])[k])[0]) - (2.0 * ((((u[_p_loop_index])[j])[k])[0]))) + ((((u[_p_loop_index - 1])[j])[k])[0])))) - (tx2 * (((((u[_p_loop_index + 1])[j])[k])[1]) - ((((u[_p_loop_index - 1])[j])[k])[1]))));
          (((rhs[_p_loop_index])[j])[k])[1] = (((((((rhs[_p_loop_index])[j])[k])[1]) + (dx2tx1 * ((((((u[_p_loop_index + 1])[j])[k])[1]) - (2.0 * ((((u[_p_loop_index])[j])[k])[1]))) + ((((u[_p_loop_index - 1])[j])[k])[1])))) + ((xxcon2 * con43) * ((up1 - (2.0 * uijk)) + um1))) - (tx2 * (((((((u[_p_loop_index + 1])[j])[k])[1]) * up1) - (((((u[_p_loop_index - 1])[j])[k])[1]) * um1)) + ((((((((u[_p_loop_index + 1])[j])[k])[4]) - (((square[_p_loop_index + 1])[j])[k])) - ((((u[_p_loop_index - 1])[j])[k])[4])) + (((square[_p_loop_index - 1])[j])[k])) * c2))));
          (((rhs[_p_loop_index])[j])[k])[2] = (((((((rhs[_p_loop_index])[j])[k])[2]) + (dx3tx1 * ((((((u[_p_loop_index + 1])[j])[k])[2]) - (2.0 * ((((u[_p_loop_index])[j])[k])[2]))) + ((((u[_p_loop_index - 1])[j])[k])[2])))) + (xxcon2 * (((((vs[_p_loop_index + 1])[j])[k]) - (2.0 * (((vs[_p_loop_index])[j])[k]))) + (((vs[_p_loop_index - 1])[j])[k])))) - (tx2 * ((((((u[_p_loop_index + 1])[j])[k])[2]) * up1) - (((((u[_p_loop_index - 1])[j])[k])[2]) * um1))));
          (((rhs[_p_loop_index])[j])[k])[3] = (((((((rhs[_p_loop_index])[j])[k])[3]) + (dx4tx1 * ((((((u[_p_loop_index + 1])[j])[k])[3]) - (2.0 * ((((u[_p_loop_index])[j])[k])[3]))) + ((((u[_p_loop_index - 1])[j])[k])[3])))) + (xxcon2 * (((((ws[_p_loop_index + 1])[j])[k]) - (2.0 * (((ws[_p_loop_index])[j])[k]))) + (((ws[_p_loop_index - 1])[j])[k])))) - (tx2 * ((((((u[_p_loop_index + 1])[j])[k])[3]) * up1) - (((((u[_p_loop_index - 1])[j])[k])[3]) * um1))));
          (((rhs[_p_loop_index])[j])[k])[4] = (((((((((rhs[_p_loop_index])[j])[k])[4]) + (dx5tx1 * ((((((u[_p_loop_index + 1])[j])[k])[4]) - (2.0 * ((((u[_p_loop_index])[j])[k])[4]))) + ((((u[_p_loop_index - 1])[j])[k])[4])))) + (xxcon3 * (((((qs[_p_loop_index + 1])[j])[k]) - (2.0 * (((qs[_p_loop_index])[j])[k]))) + (((qs[_p_loop_index - 1])[j])[k])))) + (xxcon4 * (((up1 * up1) - ((2.0 * uijk) * uijk)) + (um1 * um1)))) + (xxcon5 * (((((((u[_p_loop_index + 1])[j])[k])[4]) * (((rho_i[_p_loop_index + 1])[j])[k])) - ((2.0 * ((((u[_p_loop_index])[j])[k])[4])) * (((rho_i[_p_loop_index])[j])[k]))) + (((((u[_p_loop_index - 1])[j])[k])[4]) * (((rho_i[_p_loop_index - 1])[j])[k]))))) - (tx2 * ((((c1 * ((((u[_p_loop_index + 1])[j])[k])[4])) - (c2 * (((square[_p_loop_index + 1])[j])[k]))) * up1) - (((c1 * ((((u[_p_loop_index - 1])[j])[k])[4])) - (c2 * (((square[_p_loop_index - 1])[j])[k]))) * um1))));
        }
      }
    }
    _ompc_barrier();
  }
/*--------------------------------------------------------------------
c     add fourth order xi-direction dissipation               
c-------------------------------------------------------------------*/
  i = 1;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[1]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (m = 0; m < 5; m++) {
          (((rhs[i])[_p_loop_index])[k])[m] = (((((rhs[i])[_p_loop_index])[k])[m]) - (dssp * (((5.0 * ((((u[i])[_p_loop_index])[k])[m])) - (4.0 * ((((u[i + 1])[_p_loop_index])[k])[m]))) + ((((u[i + 2])[_p_loop_index])[k])[m]))));
        }
      }
    }
  }
  i = 2;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[1]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (m = 0; m < 5; m++) {
          (((rhs[i])[_p_loop_index])[k])[m] = (((((rhs[i])[_p_loop_index])[k])[m]) - (dssp * (((((-4.0) * ((((u[i - 1])[_p_loop_index])[k])[m])) + (6.0 * ((((u[i])[_p_loop_index])[k])[m]))) - (4.0 * ((((u[i + 1])[_p_loop_index])[k])[m]))) + ((((u[i + 2])[_p_loop_index])[k])[m]))));
        }
      }
    }
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 3;
    _p_loop_upper = ((grid_points[0]) - 3);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          for (m = 0; m < 5; m++) {
            (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * ((((((((u[_p_loop_index - 2])[j])[k])[m]) - (4.0 * ((((u[_p_loop_index - 1])[j])[k])[m]))) + (6.0 * ((((u[_p_loop_index])[j])[k])[m]))) - (4.0 * ((((u[_p_loop_index + 1])[j])[k])[m]))) + ((((u[_p_loop_index + 2])[j])[k])[m]))));
          }
        }
      }
    }
  }
  i = ((grid_points[0]) - 3);
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[1]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (m = 0; m < 5; m++) {
          (((rhs[i])[_p_loop_index])[k])[m] = (((((rhs[i])[_p_loop_index])[k])[m]) - (dssp * (((((((u[i - 2])[_p_loop_index])[k])[m]) - (4.0 * ((((u[i - 1])[_p_loop_index])[k])[m]))) + (6.0 * ((((u[i])[_p_loop_index])[k])[m]))) - (4.0 * ((((u[i + 1])[_p_loop_index])[k])[m])))));
        }
      }
    }
  }
  i = ((grid_points[0]) - 2);
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[1]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (m = 0; m < 5; m++) {
          (((rhs[i])[_p_loop_index])[k])[m] = (((((rhs[i])[_p_loop_index])[k])[m]) - (dssp * ((((((u[i - 2])[_p_loop_index])[k])[m]) - (4.0 * ((((u[i - 1])[_p_loop_index])[k])[m]))) + (5.0 * ((((u[i])[_p_loop_index])[k])[m])))));
        }
      }
    }
    _ompc_barrier();
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          vijk = (((vs[_p_loop_index])[j])[k]);
          vp1 = (((vs[_p_loop_index])[j + 1])[k]);
          vm1 = (((vs[_p_loop_index])[j - 1])[k]);
          (((rhs[_p_loop_index])[j])[k])[0] = ((((((rhs[_p_loop_index])[j])[k])[0]) + (dy1ty1 * ((((((u[_p_loop_index])[j + 1])[k])[0]) - (2.0 * ((((u[_p_loop_index])[j])[k])[0]))) + ((((u[_p_loop_index])[j - 1])[k])[0])))) - (ty2 * (((((u[_p_loop_index])[j + 1])[k])[2]) - ((((u[_p_loop_index])[j - 1])[k])[2]))));
          (((rhs[_p_loop_index])[j])[k])[1] = (((((((rhs[_p_loop_index])[j])[k])[1]) + (dy2ty1 * ((((((u[_p_loop_index])[j + 1])[k])[1]) - (2.0 * ((((u[_p_loop_index])[j])[k])[1]))) + ((((u[_p_loop_index])[j - 1])[k])[1])))) + (yycon2 * (((((us[_p_loop_index])[j + 1])[k]) - (2.0 * (((us[_p_loop_index])[j])[k]))) + (((us[_p_loop_index])[j - 1])[k])))) - (ty2 * ((((((u[_p_loop_index])[j + 1])[k])[1]) * vp1) - (((((u[_p_loop_index])[j - 1])[k])[1]) * vm1))));
          (((rhs[_p_loop_index])[j])[k])[2] = (((((((rhs[_p_loop_index])[j])[k])[2]) + (dy3ty1 * ((((((u[_p_loop_index])[j + 1])[k])[2]) - (2.0 * ((((u[_p_loop_index])[j])[k])[2]))) + ((((u[_p_loop_index])[j - 1])[k])[2])))) + ((yycon2 * con43) * ((vp1 - (2.0 * vijk)) + vm1))) - (ty2 * (((((((u[_p_loop_index])[j + 1])[k])[2]) * vp1) - (((((u[_p_loop_index])[j - 1])[k])[2]) * vm1)) + ((((((((u[_p_loop_index])[j + 1])[k])[4]) - (((square[_p_loop_index])[j + 1])[k])) - ((((u[_p_loop_index])[j - 1])[k])[4])) + (((square[_p_loop_index])[j - 1])[k])) * c2))));
          (((rhs[_p_loop_index])[j])[k])[3] = (((((((rhs[_p_loop_index])[j])[k])[3]) + (dy4ty1 * ((((((u[_p_loop_index])[j + 1])[k])[3]) - (2.0 * ((((u[_p_loop_index])[j])[k])[3]))) + ((((u[_p_loop_index])[j - 1])[k])[3])))) + (yycon2 * (((((ws[_p_loop_index])[j + 1])[k]) - (2.0 * (((ws[_p_loop_index])[j])[k]))) + (((ws[_p_loop_index])[j - 1])[k])))) - (ty2 * ((((((u[_p_loop_index])[j + 1])[k])[3]) * vp1) - (((((u[_p_loop_index])[j - 1])[k])[3]) * vm1))));
          (((rhs[_p_loop_index])[j])[k])[4] = (((((((((rhs[_p_loop_index])[j])[k])[4]) + (dy5ty1 * ((((((u[_p_loop_index])[j + 1])[k])[4]) - (2.0 * ((((u[_p_loop_index])[j])[k])[4]))) + ((((u[_p_loop_index])[j - 1])[k])[4])))) + (yycon3 * (((((qs[_p_loop_index])[j + 1])[k]) - (2.0 * (((qs[_p_loop_index])[j])[k]))) + (((qs[_p_loop_index])[j - 1])[k])))) + (yycon4 * (((vp1 * vp1) - ((2.0 * vijk) * vijk)) + (vm1 * vm1)))) + (yycon5 * (((((((u[_p_loop_index])[j + 1])[k])[4]) * (((rho_i[_p_loop_index])[j + 1])[k])) - ((2.0 * ((((u[_p_loop_index])[j])[k])[4])) * (((rho_i[_p_loop_index])[j])[k]))) + (((((u[_p_loop_index])[j - 1])[k])[4]) * (((rho_i[_p_loop_index])[j - 1])[k]))))) - (ty2 * ((((c1 * ((((u[_p_loop_index])[j + 1])[k])[4])) - (c2 * (((square[_p_loop_index])[j + 1])[k]))) * vp1) - (((c1 * ((((u[_p_loop_index])[j - 1])[k])[4])) - (c2 * (((square[_p_loop_index])[j - 1])[k]))) * vm1))));
        }
      }
    }
    _ompc_barrier();
  }
/*--------------------------------------------------------------------
c     add fourth order eta-direction dissipation         
c-------------------------------------------------------------------*/
  j = 1;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (m = 0; m < 5; m++) {
          (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * (((5.0 * ((((u[_p_loop_index])[j])[k])[m])) - (4.0 * ((((u[_p_loop_index])[j + 1])[k])[m]))) + ((((u[_p_loop_index])[j + 2])[k])[m]))));
        }
      }
    }
  }
  j = 2;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (m = 0; m < 5; m++) {
          (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * (((((-4.0) * ((((u[_p_loop_index])[j - 1])[k])[m])) + (6.0 * ((((u[_p_loop_index])[j])[k])[m]))) - (4.0 * ((((u[_p_loop_index])[j + 1])[k])[m]))) + ((((u[_p_loop_index])[j + 2])[k])[m]))));
        }
      }
    }
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 3; j < ((grid_points[1]) - 3); j++) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          for (m = 0; m < 5; m++) {
            (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * ((((((((u[_p_loop_index])[j - 2])[k])[m]) - (4.0 * ((((u[_p_loop_index])[j - 1])[k])[m]))) + (6.0 * ((((u[_p_loop_index])[j])[k])[m]))) - (4.0 * ((((u[_p_loop_index])[j + 1])[k])[m]))) + ((((u[_p_loop_index])[j + 2])[k])[m]))));
          }
        }
      }
    }
  }
  j = ((grid_points[1]) - 3);
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (m = 0; m < 5; m++) {
          (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * (((((((u[_p_loop_index])[j - 2])[k])[m]) - (4.0 * ((((u[_p_loop_index])[j - 1])[k])[m]))) + (6.0 * ((((u[_p_loop_index])[j])[k])[m]))) - (4.0 * ((((u[_p_loop_index])[j + 1])[k])[m])))));
        }
      }
    }
  }
  j = ((grid_points[1]) - 2);
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (m = 0; m < 5; m++) {
          (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * ((((((u[_p_loop_index])[j - 2])[k])[m]) - (4.0 * ((((u[_p_loop_index])[j - 1])[k])[m]))) + (5.0 * ((((u[_p_loop_index])[j])[k])[m])))));
        }
      }
    }
    _ompc_barrier();
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          wijk = (((ws[_p_loop_index])[j])[k]);
          wp1 = (((ws[_p_loop_index])[j])[k + 1]);
          wm1 = (((ws[_p_loop_index])[j])[k - 1]);
          (((rhs[_p_loop_index])[j])[k])[0] = ((((((rhs[_p_loop_index])[j])[k])[0]) + (dz1tz1 * ((((((u[_p_loop_index])[j])[k + 1])[0]) - (2.0 * ((((u[_p_loop_index])[j])[k])[0]))) + ((((u[_p_loop_index])[j])[k - 1])[0])))) - (tz2 * (((((u[_p_loop_index])[j])[k + 1])[3]) - ((((u[_p_loop_index])[j])[k - 1])[3]))));
          (((rhs[_p_loop_index])[j])[k])[1] = (((((((rhs[_p_loop_index])[j])[k])[1]) + (dz2tz1 * ((((((u[_p_loop_index])[j])[k + 1])[1]) - (2.0 * ((((u[_p_loop_index])[j])[k])[1]))) + ((((u[_p_loop_index])[j])[k - 1])[1])))) + (zzcon2 * (((((us[_p_loop_index])[j])[k + 1]) - (2.0 * (((us[_p_loop_index])[j])[k]))) + (((us[_p_loop_index])[j])[k - 1])))) - (tz2 * ((((((u[_p_loop_index])[j])[k + 1])[1]) * wp1) - (((((u[_p_loop_index])[j])[k - 1])[1]) * wm1))));
          (((rhs[_p_loop_index])[j])[k])[2] = (((((((rhs[_p_loop_index])[j])[k])[2]) + (dz3tz1 * ((((((u[_p_loop_index])[j])[k + 1])[2]) - (2.0 * ((((u[_p_loop_index])[j])[k])[2]))) + ((((u[_p_loop_index])[j])[k - 1])[2])))) + (zzcon2 * (((((vs[_p_loop_index])[j])[k + 1]) - (2.0 * (((vs[_p_loop_index])[j])[k]))) + (((vs[_p_loop_index])[j])[k - 1])))) - (tz2 * ((((((u[_p_loop_index])[j])[k + 1])[2]) * wp1) - (((((u[_p_loop_index])[j])[k - 1])[2]) * wm1))));
          (((rhs[_p_loop_index])[j])[k])[3] = (((((((rhs[_p_loop_index])[j])[k])[3]) + (dz4tz1 * ((((((u[_p_loop_index])[j])[k + 1])[3]) - (2.0 * ((((u[_p_loop_index])[j])[k])[3]))) + ((((u[_p_loop_index])[j])[k - 1])[3])))) + ((zzcon2 * con43) * ((wp1 - (2.0 * wijk)) + wm1))) - (tz2 * (((((((u[_p_loop_index])[j])[k + 1])[3]) * wp1) - (((((u[_p_loop_index])[j])[k - 1])[3]) * wm1)) + ((((((((u[_p_loop_index])[j])[k + 1])[4]) - (((square[_p_loop_index])[j])[k + 1])) - ((((u[_p_loop_index])[j])[k - 1])[4])) + (((square[_p_loop_index])[j])[k - 1])) * c2))));
          (((rhs[_p_loop_index])[j])[k])[4] = (((((((((rhs[_p_loop_index])[j])[k])[4]) + (dz5tz1 * ((((((u[_p_loop_index])[j])[k + 1])[4]) - (2.0 * ((((u[_p_loop_index])[j])[k])[4]))) + ((((u[_p_loop_index])[j])[k - 1])[4])))) + (zzcon3 * (((((qs[_p_loop_index])[j])[k + 1]) - (2.0 * (((qs[_p_loop_index])[j])[k]))) + (((qs[_p_loop_index])[j])[k - 1])))) + (zzcon4 * (((wp1 * wp1) - ((2.0 * wijk) * wijk)) + (wm1 * wm1)))) + (zzcon5 * (((((((u[_p_loop_index])[j])[k + 1])[4]) * (((rho_i[_p_loop_index])[j])[k + 1])) - ((2.0 * ((((u[_p_loop_index])[j])[k])[4])) * (((rho_i[_p_loop_index])[j])[k]))) + (((((u[_p_loop_index])[j])[k - 1])[4]) * (((rho_i[_p_loop_index])[j])[k - 1]))))) - (tz2 * ((((c1 * ((((u[_p_loop_index])[j])[k + 1])[4])) - (c2 * (((square[_p_loop_index])[j])[k + 1]))) * wp1) - (((c1 * ((((u[_p_loop_index])[j])[k - 1])[4])) - (c2 * (((square[_p_loop_index])[j])[k - 1]))) * wm1))));
        }
      }
    }
    _ompc_barrier();
  }
/*--------------------------------------------------------------------
c     add fourth order zeta-direction dissipation                
c-------------------------------------------------------------------*/
  k = 1;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (m = 0; m < 5; m++) {
          (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * (((5.0 * ((((u[_p_loop_index])[j])[k])[m])) - (4.0 * ((((u[_p_loop_index])[j])[k + 1])[m]))) + ((((u[_p_loop_index])[j])[k + 2])[m]))));
        }
      }
    }
  }
  k = 2;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (m = 0; m < 5; m++) {
          (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * (((((-4.0) * ((((u[_p_loop_index])[j])[k - 1])[m])) + (6.0 * ((((u[_p_loop_index])[j])[k])[m]))) - (4.0 * ((((u[_p_loop_index])[j])[k + 1])[m]))) + ((((u[_p_loop_index])[j])[k + 2])[m]))));
        }
      }
    }
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = 3; k < ((grid_points[2]) - 3); k++) {
          for (m = 0; m < 5; m++) {
            (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * ((((((((u[_p_loop_index])[j])[k - 2])[m]) - (4.0 * ((((u[_p_loop_index])[j])[k - 1])[m]))) + (6.0 * ((((u[_p_loop_index])[j])[k])[m]))) - (4.0 * ((((u[_p_loop_index])[j])[k + 1])[m]))) + ((((u[_p_loop_index])[j])[k + 2])[m]))));
          }
        }
      }
    }
  }
  k = ((grid_points[2]) - 3);
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (m = 0; m < 5; m++) {
          (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * (((((((u[_p_loop_index])[j])[k - 2])[m]) - (4.0 * ((((u[_p_loop_index])[j])[k - 1])[m]))) + (6.0 * ((((u[_p_loop_index])[j])[k])[m]))) - (4.0 * ((((u[_p_loop_index])[j])[k + 1])[m])))));
        }
      }
    }
  }
  k = ((grid_points[2]) - 2);
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (m = 0; m < 5; m++) {
          (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (dssp * ((((((u[_p_loop_index])[j])[k - 2])[m]) - (4.0 * ((((u[_p_loop_index])[j])[k - 1])[m]))) + (5.0 * ((((u[_p_loop_index])[j])[k])[m])))));
        }
      }
    }
    _ompc_barrier();
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[1]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
        for (m = 0; m < 5; m++) {
          for (i = 1; i < ((grid_points[0]) - 1); i++) {
            (((rhs[i])[_p_loop_index])[k])[m] = (((((rhs[i])[_p_loop_index])[k])[m]) * dt);
          }
        }
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void set_constants()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
  (ce[0])[0] = 2.0;
  (ce[0])[1] = 0.0;
  (ce[0])[2] = 0.0;
  (ce[0])[3] = 4.0;
  (ce[0])[4] = 5.0;
  (ce[0])[5] = 3.0;
  (ce[0])[6] = 0.5;
  (ce[0])[7] = 0.02;
  (ce[0])[8] = 0.010;
  (ce[0])[9] = 0.03;
  (ce[0])[10] = 0.5;
  (ce[0])[11] = 0.4;
  (ce[0])[12] = 0.3;
  (ce[1])[0] = 1.0;
  (ce[1])[1] = 0.0;
  (ce[1])[2] = 0.0;
  (ce[1])[3] = 0.0;
  (ce[1])[4] = 1.0;
  (ce[1])[5] = 2.0;
  (ce[1])[6] = 3.0;
  (ce[1])[7] = 0.010;
  (ce[1])[8] = 0.03;
  (ce[1])[9] = 0.02;
  (ce[1])[10] = 0.4;
  (ce[1])[11] = 0.3;
  (ce[1])[12] = 0.5;
  (ce[2])[0] = 2.0;
  (ce[2])[1] = 2.0;
  (ce[2])[2] = 0.0;
  (ce[2])[3] = 0.0;
  (ce[2])[4] = 0.0;
  (ce[2])[5] = 2.0;
  (ce[2])[6] = 3.0;
  (ce[2])[7] = 0.04;
  (ce[2])[8] = 0.03;
  (ce[2])[9] = 0.05;
  (ce[2])[10] = 0.3;
  (ce[2])[11] = 0.5;
  (ce[2])[12] = 0.4;
  (ce[3])[0] = 2.0;
  (ce[3])[1] = 2.0;
  (ce[3])[2] = 0.0;
  (ce[3])[3] = 0.0;
  (ce[3])[4] = 0.0;
  (ce[3])[5] = 2.0;
  (ce[3])[6] = 3.0;
  (ce[3])[7] = 0.03;
  (ce[3])[8] = 0.05;
  (ce[3])[9] = 0.04;
  (ce[3])[10] = 0.2;
  (ce[3])[11] = 0.1;
  (ce[3])[12] = 0.3;
  (ce[4])[0] = 5.0;
  (ce[4])[1] = 4.0;
  (ce[4])[2] = 3.0;
  (ce[4])[3] = 2.0;
  (ce[4])[4] = 0.1;
  (ce[4])[5] = 0.4;
  (ce[4])[6] = 0.3;
  (ce[4])[7] = 0.05;
  (ce[4])[8] = 0.04;
  (ce[4])[9] = 0.03;
  (ce[4])[10] = 0.1;
  (ce[4])[11] = 0.3;
  (ce[4])[12] = 0.2;
  c1 = 1.4;
  c2 = 0.4;
  c3 = 0.1;
  c4 = 1.0;
  c5 = 1.4;
  dnxm1 = (1.0 / ((double )((grid_points[0]) - 1)));
  dnym1 = (1.0 / ((double )((grid_points[1]) - 1)));
  dnzm1 = (1.0 / ((double )((grid_points[2]) - 1)));
  c1c2 = (c1 * c2);
  c1c5 = (c1 * c5);
  c3c4 = (c3 * c4);
  c1345 = (c1c5 * c3c4);
  conz1 = (1.0 - c1c5);
  tx1 = (1.0 / (dnxm1 * dnxm1));
  tx2 = (1.0 / (2.0 * dnxm1));
  tx3 = (1.0 / dnxm1);
  ty1 = (1.0 / (dnym1 * dnym1));
  ty2 = (1.0 / (2.0 * dnym1));
  ty3 = (1.0 / dnym1);
  tz1 = (1.0 / (dnzm1 * dnzm1));
  tz2 = (1.0 / (2.0 * dnzm1));
  tz3 = (1.0 / dnzm1);
  dx1 = 0.75;
  dx2 = 0.75;
  dx3 = 0.75;
  dx4 = 0.75;
  dx5 = 0.75;
  dy1 = 0.75;
  dy2 = 0.75;
  dy3 = 0.75;
  dy4 = 0.75;
  dy5 = 0.75;
  dz1 = 1.0;
  dz2 = 1.0;
  dz3 = 1.0;
  dz4 = 1.0;
  dz5 = 1.0;
  dxmax = ((dx3 > dx4)?dx3:dx4);
  dymax = ((dy2 > dy4)?dy2:dy4);
  dzmax = ((dz2 > dz3)?dz2:dz3);
  dssp = (0.25 * (((dx1 > (((dy1 > dz1)?dy1:dz1)))?dx1:(((dy1 > dz1)?dy1:dz1)))));
  c4dssp = (4.0 * dssp);
  c5dssp = (5.0 * dssp);
  dttx1 = (dt * tx1);
  dttx2 = (dt * tx2);
  dtty1 = (dt * ty1);
  dtty2 = (dt * ty2);
  dttz1 = (dt * tz1);
  dttz2 = (dt * tz2);
  c2dttx1 = (2.0 * dttx1);
  c2dtty1 = (2.0 * dtty1);
  c2dttz1 = (2.0 * dttz1);
  dtdssp = (dt * dssp);
  comz1 = dtdssp;
  comz4 = (4.0 * dtdssp);
  comz5 = (5.0 * dtdssp);
  comz6 = (6.0 * dtdssp);
  c3c4tx3 = (c3c4 * tx3);
  c3c4ty3 = (c3c4 * ty3);
  c3c4tz3 = (c3c4 * tz3);
  dx1tx1 = (dx1 * tx1);
  dx2tx1 = (dx2 * tx1);
  dx3tx1 = (dx3 * tx1);
  dx4tx1 = (dx4 * tx1);
  dx5tx1 = (dx5 * tx1);
  dy1ty1 = (dy1 * ty1);
  dy2ty1 = (dy2 * ty1);
  dy3ty1 = (dy3 * ty1);
  dy4ty1 = (dy4 * ty1);
  dy5ty1 = (dy5 * ty1);
  dz1tz1 = (dz1 * tz1);
  dz2tz1 = (dz2 * tz1);
  dz3tz1 = (dz3 * tz1);
  dz4tz1 = (dz4 * tz1);
  dz5tz1 = (dz5 * tz1);
  c2iv = 2.5;
  con43 = (4.0 / 3.0);
  con16 = (1.0 / 6.0);
  xxcon1 = ((c3c4tx3 * con43) * tx3);
  xxcon2 = (c3c4tx3 * tx3);
  xxcon3 = ((c3c4tx3 * conz1) * tx3);
  xxcon4 = ((c3c4tx3 * con16) * tx3);
  xxcon5 = ((c3c4tx3 * c1c5) * tx3);
  yycon1 = ((c3c4ty3 * con43) * ty3);
  yycon2 = (c3c4ty3 * ty3);
  yycon3 = ((c3c4ty3 * conz1) * ty3);
  yycon4 = ((c3c4ty3 * con16) * ty3);
  yycon5 = ((c3c4ty3 * c1c5) * ty3);
  zzcon1 = ((c3c4tz3 * con43) * tz3);
  zzcon2 = (c3c4tz3 * tz3);
  zzcon3 = ((c3c4tz3 * conz1) * tz3);
  zzcon4 = ((c3c4tz3 * con16) * tz3);
  zzcon5 = ((c3c4tz3 * c1c5) * tz3);
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void verify(int no_time_steps,char *cclass,boolean *verified)
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c  verification routine                         
c-------------------------------------------------------------------*/
  double xcrref[5UL];
  double xceref[5UL];
  double xcrdif[5UL];
  double xcedif[5UL];
  double epsilon;
  double xce[5UL];
  double xcr[5UL];
  double dtref;
  int m;
/*--------------------------------------------------------------------
c   tolerance level
c-------------------------------------------------------------------*/
  epsilon = 1.0e-08;
/*--------------------------------------------------------------------
c   compute the error norm and the residual norm, and exit if not printing
c-------------------------------------------------------------------*/
  error_norm(xce);
  compute_rhs();
  rhs_norm(xcr);
  for (m = 0; m < 5; m++) {
    xcr[m] = ((xcr[m]) / dt);
  }
   *cclass = 'U';
   *verified = (1);
  for (m = 0; m < 5; m++) {
    xcrref[m] = 1.0;
    xceref[m] = 1.0;
  }
/*--------------------------------------------------------------------
c    reference data for 12X12X12 grids after 100 time steps, with DT = 1.0d-02
c-------------------------------------------------------------------*/
  if (((((grid_points[0]) == 12) && ((grid_points[1]) == 12)) && ((grid_points[2]) == 12)) && (no_time_steps == 60)) 
{
     *cclass = 'S';
    dtref = 0.010;
/*--------------------------------------------------------------------
c  Reference values of RMS-norms of residual.
c-------------------------------------------------------------------*/
    xcrref[0] = 1.7034283709541311e-01;
    xcrref[1] = 1.2975252070034097e-02;
    xcrref[2] = 3.2527926989486055e-02;
    xcrref[3] = 2.6436421275166801e-02;
    xcrref[4] = 1.9211784131744430e-01;
/*--------------------------------------------------------------------
c  Reference values of RMS-norms of solution error.
c-------------------------------------------------------------------*/
    xceref[0] = 4.9976913345811579e-04;
    xceref[1] = 4.5195666782961927e-05;
    xceref[2] = 7.3973765172921357e-05;
    xceref[3] = 7.3821238632439731e-05;
    xceref[4] = 8.9269630987491446e-04;
/*--------------------------------------------------------------------
c    reference data for 24X24X24 grids after 200 time steps, with DT = 0.8d-3
c-------------------------------------------------------------------*/
  }
  else {
    if (((((grid_points[0]) == 24) && ((grid_points[1]) == 24)) && ((grid_points[2]) == 24)) && (no_time_steps == 200)) 
{
       *cclass = 'W';
      dtref = 0.8e-3;
/*--------------------------------------------------------------------
c  Reference values of RMS-norms of residual.
c-------------------------------------------------------------------*/
      xcrref[0] = 0.1125590409344e+03;
      xcrref[1] = 0.1180007595731e+02;
      xcrref[2] = 0.2710329767846e+02;
      xcrref[3] = 0.2469174937669e+02;
      xcrref[4] = 0.2638427874317e+03;
/*--------------------------------------------------------------------
c  Reference values of RMS-norms of solution error.
c-------------------------------------------------------------------*/
      xceref[0] = 0.4419655736008e+01;
      xceref[1] = 0.4638531260002e+00;
      xceref[2] = 0.1011551749967e+01;
      xceref[3] = 0.9235878729944e+00;
      xceref[4] = 0.1018045837718e+02;
/*--------------------------------------------------------------------
c    reference data for 64X64X64 grids after 200 time steps, with DT = 0.8d-3
c-------------------------------------------------------------------*/
    }
    else {
      if (((((grid_points[0]) == 64) && ((grid_points[1]) == 64)) && ((grid_points[2]) == 64)) && (no_time_steps == 200)) 
{
         *cclass = 'A';
        dtref = 0.8e-3;
/*--------------------------------------------------------------------
c  Reference values of RMS-norms of residual.
c-------------------------------------------------------------------*/
        xcrref[0] = 1.0806346714637264e+02;
        xcrref[1] = 1.1319730901220813e+01;
        xcrref[2] = 2.5974354511582465e+01;
        xcrref[3] = 2.3665622544678910e+01;
        xcrref[4] = 2.5278963211748344e+02;
/*--------------------------------------------------------------------
c  Reference values of RMS-norms of solution error.
c-------------------------------------------------------------------*/
        xceref[0] = 4.2348416040525025e+00;
        xceref[1] = 4.4390282496995698e-01;
        xceref[2] = 9.6692480136345650e-01;
        xceref[3] = 8.8302063039765474e-01;
        xceref[4] = 9.7379901770829278e+00;
/*--------------------------------------------------------------------
c    reference data for 102X102X102 grids after 200 time steps,
c    with DT = 3.0d-04
c-------------------------------------------------------------------*/
      }
      else {
        if (((((grid_points[0]) == 102) && ((grid_points[1]) == 102)) && ((grid_points[2]) == 102)) && (no_time_steps == 200)) 
{
           *cclass = 'B';
          dtref = 3.0e-4;
/*--------------------------------------------------------------------
c  Reference values of RMS-norms of residual.
c-------------------------------------------------------------------*/
          xcrref[0] = 1.4233597229287254e+03;
          xcrref[1] = 9.9330522590150238e+01;
          xcrref[2] = 3.5646025644535285e+02;
          xcrref[3] = 3.2485447959084092e+02;
          xcrref[4] = 3.2707541254659363e+03;
/*--------------------------------------------------------------------
c  Reference values of RMS-norms of solution error.
c-------------------------------------------------------------------*/
          xceref[0] = 5.2969847140936856e+01;
          xceref[1] = 4.4632896115670668e+00;
          xceref[2] = 1.3122573342210174e+01;
          xceref[3] = 1.2006925323559144e+01;
          xceref[4] = 1.2459576151035986e+02;
/*--------------------------------------------------------------------
c    reference data for 162X162X162 grids after 200 time steps,
c    with DT = 1.0d-04
c-------------------------------------------------------------------*/
        }
        else {
          if (((((grid_points[0]) == 162) && ((grid_points[1]) == 162)) && ((grid_points[2]) == 162)) && (no_time_steps == 200)) {
             *cclass = 'C';
            dtref = 1.0e-4;
/*--------------------------------------------------------------------
c  Reference values of RMS-norms of residual.
c-------------------------------------------------------------------*/
            xcrref[0] = 0.62398116551764615e+04;
            xcrref[1] = 0.50793239190423964e+03;
            xcrref[2] = 0.15423530093013596e+04;
            xcrref[3] = 0.13302387929291190e+04;
            xcrref[4] = 0.11604087428436455e+05;
/*--------------------------------------------------------------------
c  Reference values of RMS-norms of solution error.
c-------------------------------------------------------------------*/
            xceref[0] = 0.16462008369091265e+03;
            xceref[1] = 0.11497107903824313e+02;
            xceref[2] = 0.41207446207461508e+02;
            xceref[3] = 0.37087651059694167e+02;
            xceref[4] = 0.36211053051841265e+03;
          }
          else {
             *verified = (0);
          }
        }
      }
    }
  }
/*--------------------------------------------------------------------
c    verification test for residuals if gridsize is either 12X12X12 or 
c    64X64X64 or 102X102X102 or 162X162X162
c-------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c    Compute the difference of solution values and the known reference values.
c-------------------------------------------------------------------*/
  for (m = 0; m < 5; m++) {
    xcrdif[m] = fabs((((xcr[m]) - (xcrref[m])) / (xcrref[m])));
    xcedif[m] = fabs((((xce[m]) - (xceref[m])) / (xceref[m])));
  }
/*--------------------------------------------------------------------
c    Output the comparison of computed results to known cases.
c-------------------------------------------------------------------*/
  if ((( *cclass)) != ('U')) {
    printf(" Verification being performed for class %1c\n",(( *cclass)));
    printf(" accuracy setting for epsilon = %20.13e\n",epsilon);
    if (fabs((dt - dtref)) > epsilon) {
       *verified = (0);
       *cclass = 'U';
      printf(" DT does not match the reference value of %15.8e\n",dtref);
    }
    else {
    }
  }
  else {
    printf(" Unknown class\n");
  }
  if ((( *cclass)) != ('U')) {
    printf(" Comparison of RMS-norms of residual\n");
  }
  else {
    printf(" RMS-norms of residual\n");
  }
  for (m = 0; m < 5; m++) {
    if ((( *cclass)) == ('U')) {
      printf("          %2d%20.13e\n",m,(xcr[m]));
    }
    else {
      if ((xcrdif[m]) > epsilon) {
         *verified = (0);
        printf(" FAILURE: %2d%20.13e%20.13e%20.13e\n",m,(xcr[m]),(xcrref[m]),(xcrdif[m]));
      }
      else {
        printf("          %2d%20.13e%20.13e%20.13e\n",m,(xcr[m]),(xcrref[m]),(xcrdif[m]));
      }
    }
  }
  if ((( *cclass)) != ('U')) {
    printf(" Comparison of RMS-norms of solution error\n");
  }
  else {
    printf(" RMS-norms of solution error\n");
  }
  for (m = 0; m < 5; m++) {
    if ((( *cclass)) == ('U')) {
      printf("          %2d%20.13e\n",m,(xce[m]));
    }
    else {
      if ((xcedif[m]) > epsilon) {
         *verified = (0);
        printf(" FAILURE: %2d%20.13e%20.13e%20.13e\n",m,(xce[m]),(xceref[m]),(xcedif[m]));
      }
      else {
        printf("          %2d%20.13e%20.13e%20.13e\n",m,(xce[m]),(xceref[m]),(xcedif[m]));
      }
    }
  }
  if ((( *cclass)) == ('U')) {
    printf(" No reference values provided\n");
    printf(" No verification performed\n");
  }
  else {
    if ( *verified == 1) {
      printf(" Verification Successful\n");
    }
    else {
      printf(" Verification failed\n");
    }
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void x_solve()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     
c     Performs line solves in X direction by first factoring
c     the block-tridiagonal matrix into an upper triangular matrix, 
c     and then performing back substitution to solve for the unknow
c     vectors of each line.  
c     
c     Make sure we treat elements zero to cell_size in the direction
c     of the sweep.
c     
c-------------------------------------------------------------------*/
  lhsx();
  x_solve_cell();
  x_backsubstitute();
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void x_backsubstitute()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     back solve: if last cell, then generate U(isize)=rhs[isize)
c     else assume U(isize) is loaded in un pack backsub_info
c     so just use it
c     after call u(istart) will be sent to next cell
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
  int m;
  int n;
  for (i = ((grid_points[0]) - 2); i >= 0; i--) {{
      int _p_loop_index;
      int _p_loop_lower;
      int _p_loop_upper;
      int _p_loop_stride;
      _p_loop_lower = 1;
      _p_loop_upper = ((grid_points[1]) - 1);
      _p_loop_stride = 1;
      _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
      for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          for (m = 0; m < 5; m++) {
            for (n = 0; n < 5; n++) {
              (((rhs[i])[_p_loop_index])[k])[m] = (((((rhs[i])[_p_loop_index])[k])[m]) - (((((((lhs[i])[_p_loop_index])[k])[2])[m])[n]) * ((((rhs[i + 1])[_p_loop_index])[k])[n])));
            }
          }
        }
      }
      _ompc_barrier();
    }
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void x_solve_cell()
{
/*--------------------------------------------------------------------
c     performs guaussian elimination on this cell.
c     
c     assumes that unpacking routines for non-first cells 
c     preload C' and rhs' from previous cell.
c     
c     assumed send happens outside this routine, but that
c     c'(IMAX) and rhs'(IMAX) will be sent to next cell
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
  int isize;
  isize = ((grid_points[0]) - 1);
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[1]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
/*--------------------------------------------------------------------
c     multiply c(0,j,k) by b_inverse and copy back to c
c     multiply rhs(0) by b_inverse(0) and copy to rhs
c-------------------------------------------------------------------*/
        binvcrhs(((((lhs[0])[_p_loop_index])[k])[1]),((((lhs[0])[_p_loop_index])[k])[2]),(((rhs[0])[_p_loop_index])[k]));
      }
    }
    _ompc_barrier();
  }
/*--------------------------------------------------------------------
c     begin inner most do loop
c     do all the elements of the cell unless last 
c-------------------------------------------------------------------*/
  for (i = 1; i < isize; i++) {{
      int _p_loop_index;
      int _p_loop_lower;
      int _p_loop_upper;
      int _p_loop_stride;
      _p_loop_lower = 1;
      _p_loop_upper = ((grid_points[1]) - 1);
      _p_loop_stride = 1;
      _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
      for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
/*--------------------------------------------------------------------
c     rhs(i) = rhs(i) - A*rhs(i-1)
c-------------------------------------------------------------------*/
          matvec_sub(((((lhs[i])[_p_loop_index])[k])[0]),(((rhs[i - 1])[_p_loop_index])[k]),(((rhs[i])[_p_loop_index])[k]));
/*--------------------------------------------------------------------
c     B(i) = B(i) - C(i-1)*A(i)
c-------------------------------------------------------------------*/
          matmul_sub(((((lhs[i])[_p_loop_index])[k])[0]),((((lhs[i - 1])[_p_loop_index])[k])[2]),((((lhs[i])[_p_loop_index])[k])[1]));
/*--------------------------------------------------------------------
c     multiply c(i,j,k) by b_inverse and copy back to c
c     multiply rhs(1,j,k) by b_inverse(1,j,k) and copy to rhs
c-------------------------------------------------------------------*/
          binvcrhs(((((lhs[i])[_p_loop_index])[k])[1]),((((lhs[i])[_p_loop_index])[k])[2]),(((rhs[i])[_p_loop_index])[k]));
        }
      }
      _ompc_barrier();
    }
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[1]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
/*--------------------------------------------------------------------
c     rhs(isize) = rhs(isize) - A*rhs(isize-1)
c-------------------------------------------------------------------*/
        matvec_sub(((((lhs[isize])[_p_loop_index])[k])[0]),(((rhs[isize - 1])[_p_loop_index])[k]),(((rhs[isize])[_p_loop_index])[k]));
/*--------------------------------------------------------------------
c     B(isize) = B(isize) - C(isize-1)*A(isize)
c-------------------------------------------------------------------*/
        matmul_sub(((((lhs[isize])[_p_loop_index])[k])[0]),((((lhs[isize - 1])[_p_loop_index])[k])[2]),((((lhs[isize])[_p_loop_index])[k])[1]));
/*--------------------------------------------------------------------
c     multiply rhs() by b_inverse() and copy to rhs
c-------------------------------------------------------------------*/
        binvrhs(((((lhs[i])[_p_loop_index])[k])[1]),(((rhs[i])[_p_loop_index])[k]));
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void matvec_sub(double (*ablock)[5UL],double *avec,double *bvec)
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     subtracts bvec=bvec - ablock*avec
c-------------------------------------------------------------------*/
  int i;
  for (i = 0; i < 5; i++) {
/*--------------------------------------------------------------------
c            rhs(i,ic,jc,kc,ccell) = rhs(i,ic,jc,kc,ccell) 
c     $           - lhs[i,1,ablock,ia,ja,ka,acell)*
c-------------------------------------------------------------------*/
    bvec[i] = ((((((bvec[i]) - (((ablock[i])[0]) * (avec[0]))) - (((ablock[i])[1]) * (avec[1]))) - (((ablock[i])[2]) * (avec[2]))) - (((ablock[i])[3]) * (avec[3]))) - (((ablock[i])[4]) * (avec[4])));
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void matmul_sub(double (*ablock)[5UL],double (*bblock)[5UL],double (*cblock)[5UL])
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     subtracts a(i,j,k) X b(i,j,k) from c(i,j,k)
c-------------------------------------------------------------------*/
  int j;
  for (j = 0; j < 5; j++) {
    (cblock[0])[j] = (((((((cblock[0])[j]) - (((ablock[0])[0]) * ((bblock[0])[j]))) - (((ablock[0])[1]) * ((bblock[1])[j]))) - (((ablock[0])[2]) * ((bblock[2])[j]))) - (((ablock[0])[3]) * ((bblock[3])[j]))) - (((ablock[0])[4]) * ((bblock[4])[j])));
    (cblock[1])[j] = (((((((cblock[1])[j]) - (((ablock[1])[0]) * ((bblock[0])[j]))) - (((ablock[1])[1]) * ((bblock[1])[j]))) - (((ablock[1])[2]) * ((bblock[2])[j]))) - (((ablock[1])[3]) * ((bblock[3])[j]))) - (((ablock[1])[4]) * ((bblock[4])[j])));
    (cblock[2])[j] = (((((((cblock[2])[j]) - (((ablock[2])[0]) * ((bblock[0])[j]))) - (((ablock[2])[1]) * ((bblock[1])[j]))) - (((ablock[2])[2]) * ((bblock[2])[j]))) - (((ablock[2])[3]) * ((bblock[3])[j]))) - (((ablock[2])[4]) * ((bblock[4])[j])));
    (cblock[3])[j] = (((((((cblock[3])[j]) - (((ablock[3])[0]) * ((bblock[0])[j]))) - (((ablock[3])[1]) * ((bblock[1])[j]))) - (((ablock[3])[2]) * ((bblock[2])[j]))) - (((ablock[3])[3]) * ((bblock[3])[j]))) - (((ablock[3])[4]) * ((bblock[4])[j])));
    (cblock[4])[j] = (((((((cblock[4])[j]) - (((ablock[4])[0]) * ((bblock[0])[j]))) - (((ablock[4])[1]) * ((bblock[1])[j]))) - (((ablock[4])[2]) * ((bblock[2])[j]))) - (((ablock[4])[3]) * ((bblock[3])[j]))) - (((ablock[4])[4]) * ((bblock[4])[j])));
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void binvcrhs(double (*lhs)[5UL],double (*c)[5UL],double *r)
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
  double pivot;
  double coeff;
/*--------------------------------------------------------------------
c     
c-------------------------------------------------------------------*/
  pivot = (1.0 / ((lhs[0])[0]));
  (lhs[0])[1] = (((lhs[0])[1]) * pivot);
  (lhs[0])[2] = (((lhs[0])[2]) * pivot);
  (lhs[0])[3] = (((lhs[0])[3]) * pivot);
  (lhs[0])[4] = (((lhs[0])[4]) * pivot);
  (c[0])[0] = (((c[0])[0]) * pivot);
  (c[0])[1] = (((c[0])[1]) * pivot);
  (c[0])[2] = (((c[0])[2]) * pivot);
  (c[0])[3] = (((c[0])[3]) * pivot);
  (c[0])[4] = (((c[0])[4]) * pivot);
  r[0] = ((r[0]) * pivot);
  coeff = ((lhs[1])[0]);
  (lhs[1])[1] = (((lhs[1])[1]) - (coeff * ((lhs[0])[1])));
  (lhs[1])[2] = (((lhs[1])[2]) - (coeff * ((lhs[0])[2])));
  (lhs[1])[3] = (((lhs[1])[3]) - (coeff * ((lhs[0])[3])));
  (lhs[1])[4] = (((lhs[1])[4]) - (coeff * ((lhs[0])[4])));
  (c[1])[0] = (((c[1])[0]) - (coeff * ((c[0])[0])));
  (c[1])[1] = (((c[1])[1]) - (coeff * ((c[0])[1])));
  (c[1])[2] = (((c[1])[2]) - (coeff * ((c[0])[2])));
  (c[1])[3] = (((c[1])[3]) - (coeff * ((c[0])[3])));
  (c[1])[4] = (((c[1])[4]) - (coeff * ((c[0])[4])));
  r[1] = ((r[1]) - (coeff * (r[0])));
  coeff = ((lhs[2])[0]);
  (lhs[2])[1] = (((lhs[2])[1]) - (coeff * ((lhs[0])[1])));
  (lhs[2])[2] = (((lhs[2])[2]) - (coeff * ((lhs[0])[2])));
  (lhs[2])[3] = (((lhs[2])[3]) - (coeff * ((lhs[0])[3])));
  (lhs[2])[4] = (((lhs[2])[4]) - (coeff * ((lhs[0])[4])));
  (c[2])[0] = (((c[2])[0]) - (coeff * ((c[0])[0])));
  (c[2])[1] = (((c[2])[1]) - (coeff * ((c[0])[1])));
  (c[2])[2] = (((c[2])[2]) - (coeff * ((c[0])[2])));
  (c[2])[3] = (((c[2])[3]) - (coeff * ((c[0])[3])));
  (c[2])[4] = (((c[2])[4]) - (coeff * ((c[0])[4])));
  r[2] = ((r[2]) - (coeff * (r[0])));
  coeff = ((lhs[3])[0]);
  (lhs[3])[1] = (((lhs[3])[1]) - (coeff * ((lhs[0])[1])));
  (lhs[3])[2] = (((lhs[3])[2]) - (coeff * ((lhs[0])[2])));
  (lhs[3])[3] = (((lhs[3])[3]) - (coeff * ((lhs[0])[3])));
  (lhs[3])[4] = (((lhs[3])[4]) - (coeff * ((lhs[0])[4])));
  (c[3])[0] = (((c[3])[0]) - (coeff * ((c[0])[0])));
  (c[3])[1] = (((c[3])[1]) - (coeff * ((c[0])[1])));
  (c[3])[2] = (((c[3])[2]) - (coeff * ((c[0])[2])));
  (c[3])[3] = (((c[3])[3]) - (coeff * ((c[0])[3])));
  (c[3])[4] = (((c[3])[4]) - (coeff * ((c[0])[4])));
  r[3] = ((r[3]) - (coeff * (r[0])));
  coeff = ((lhs[4])[0]);
  (lhs[4])[1] = (((lhs[4])[1]) - (coeff * ((lhs[0])[1])));
  (lhs[4])[2] = (((lhs[4])[2]) - (coeff * ((lhs[0])[2])));
  (lhs[4])[3] = (((lhs[4])[3]) - (coeff * ((lhs[0])[3])));
  (lhs[4])[4] = (((lhs[4])[4]) - (coeff * ((lhs[0])[4])));
  (c[4])[0] = (((c[4])[0]) - (coeff * ((c[0])[0])));
  (c[4])[1] = (((c[4])[1]) - (coeff * ((c[0])[1])));
  (c[4])[2] = (((c[4])[2]) - (coeff * ((c[0])[2])));
  (c[4])[3] = (((c[4])[3]) - (coeff * ((c[0])[3])));
  (c[4])[4] = (((c[4])[4]) - (coeff * ((c[0])[4])));
  r[4] = ((r[4]) - (coeff * (r[0])));
  pivot = (1.0 / ((lhs[1])[1]));
  (lhs[1])[2] = (((lhs[1])[2]) * pivot);
  (lhs[1])[3] = (((lhs[1])[3]) * pivot);
  (lhs[1])[4] = (((lhs[1])[4]) * pivot);
  (c[1])[0] = (((c[1])[0]) * pivot);
  (c[1])[1] = (((c[1])[1]) * pivot);
  (c[1])[2] = (((c[1])[2]) * pivot);
  (c[1])[3] = (((c[1])[3]) * pivot);
  (c[1])[4] = (((c[1])[4]) * pivot);
  r[1] = ((r[1]) * pivot);
  coeff = ((lhs[0])[1]);
  (lhs[0])[2] = (((lhs[0])[2]) - (coeff * ((lhs[1])[2])));
  (lhs[0])[3] = (((lhs[0])[3]) - (coeff * ((lhs[1])[3])));
  (lhs[0])[4] = (((lhs[0])[4]) - (coeff * ((lhs[1])[4])));
  (c[0])[0] = (((c[0])[0]) - (coeff * ((c[1])[0])));
  (c[0])[1] = (((c[0])[1]) - (coeff * ((c[1])[1])));
  (c[0])[2] = (((c[0])[2]) - (coeff * ((c[1])[2])));
  (c[0])[3] = (((c[0])[3]) - (coeff * ((c[1])[3])));
  (c[0])[4] = (((c[0])[4]) - (coeff * ((c[1])[4])));
  r[0] = ((r[0]) - (coeff * (r[1])));
  coeff = ((lhs[2])[1]);
  (lhs[2])[2] = (((lhs[2])[2]) - (coeff * ((lhs[1])[2])));
  (lhs[2])[3] = (((lhs[2])[3]) - (coeff * ((lhs[1])[3])));
  (lhs[2])[4] = (((lhs[2])[4]) - (coeff * ((lhs[1])[4])));
  (c[2])[0] = (((c[2])[0]) - (coeff * ((c[1])[0])));
  (c[2])[1] = (((c[2])[1]) - (coeff * ((c[1])[1])));
  (c[2])[2] = (((c[2])[2]) - (coeff * ((c[1])[2])));
  (c[2])[3] = (((c[2])[3]) - (coeff * ((c[1])[3])));
  (c[2])[4] = (((c[2])[4]) - (coeff * ((c[1])[4])));
  r[2] = ((r[2]) - (coeff * (r[1])));
  coeff = ((lhs[3])[1]);
  (lhs[3])[2] = (((lhs[3])[2]) - (coeff * ((lhs[1])[2])));
  (lhs[3])[3] = (((lhs[3])[3]) - (coeff * ((lhs[1])[3])));
  (lhs[3])[4] = (((lhs[3])[4]) - (coeff * ((lhs[1])[4])));
  (c[3])[0] = (((c[3])[0]) - (coeff * ((c[1])[0])));
  (c[3])[1] = (((c[3])[1]) - (coeff * ((c[1])[1])));
  (c[3])[2] = (((c[3])[2]) - (coeff * ((c[1])[2])));
  (c[3])[3] = (((c[3])[3]) - (coeff * ((c[1])[3])));
  (c[3])[4] = (((c[3])[4]) - (coeff * ((c[1])[4])));
  r[3] = ((r[3]) - (coeff * (r[1])));
  coeff = ((lhs[4])[1]);
  (lhs[4])[2] = (((lhs[4])[2]) - (coeff * ((lhs[1])[2])));
  (lhs[4])[3] = (((lhs[4])[3]) - (coeff * ((lhs[1])[3])));
  (lhs[4])[4] = (((lhs[4])[4]) - (coeff * ((lhs[1])[4])));
  (c[4])[0] = (((c[4])[0]) - (coeff * ((c[1])[0])));
  (c[4])[1] = (((c[4])[1]) - (coeff * ((c[1])[1])));
  (c[4])[2] = (((c[4])[2]) - (coeff * ((c[1])[2])));
  (c[4])[3] = (((c[4])[3]) - (coeff * ((c[1])[3])));
  (c[4])[4] = (((c[4])[4]) - (coeff * ((c[1])[4])));
  r[4] = ((r[4]) - (coeff * (r[1])));
  pivot = (1.0 / ((lhs[2])[2]));
  (lhs[2])[3] = (((lhs[2])[3]) * pivot);
  (lhs[2])[4] = (((lhs[2])[4]) * pivot);
  (c[2])[0] = (((c[2])[0]) * pivot);
  (c[2])[1] = (((c[2])[1]) * pivot);
  (c[2])[2] = (((c[2])[2]) * pivot);
  (c[2])[3] = (((c[2])[3]) * pivot);
  (c[2])[4] = (((c[2])[4]) * pivot);
  r[2] = ((r[2]) * pivot);
  coeff = ((lhs[0])[2]);
  (lhs[0])[3] = (((lhs[0])[3]) - (coeff * ((lhs[2])[3])));
  (lhs[0])[4] = (((lhs[0])[4]) - (coeff * ((lhs[2])[4])));
  (c[0])[0] = (((c[0])[0]) - (coeff * ((c[2])[0])));
  (c[0])[1] = (((c[0])[1]) - (coeff * ((c[2])[1])));
  (c[0])[2] = (((c[0])[2]) - (coeff * ((c[2])[2])));
  (c[0])[3] = (((c[0])[3]) - (coeff * ((c[2])[3])));
  (c[0])[4] = (((c[0])[4]) - (coeff * ((c[2])[4])));
  r[0] = ((r[0]) - (coeff * (r[2])));
  coeff = ((lhs[1])[2]);
  (lhs[1])[3] = (((lhs[1])[3]) - (coeff * ((lhs[2])[3])));
  (lhs[1])[4] = (((lhs[1])[4]) - (coeff * ((lhs[2])[4])));
  (c[1])[0] = (((c[1])[0]) - (coeff * ((c[2])[0])));
  (c[1])[1] = (((c[1])[1]) - (coeff * ((c[2])[1])));
  (c[1])[2] = (((c[1])[2]) - (coeff * ((c[2])[2])));
  (c[1])[3] = (((c[1])[3]) - (coeff * ((c[2])[3])));
  (c[1])[4] = (((c[1])[4]) - (coeff * ((c[2])[4])));
  r[1] = ((r[1]) - (coeff * (r[2])));
  coeff = ((lhs[3])[2]);
  (lhs[3])[3] = (((lhs[3])[3]) - (coeff * ((lhs[2])[3])));
  (lhs[3])[4] = (((lhs[3])[4]) - (coeff * ((lhs[2])[4])));
  (c[3])[0] = (((c[3])[0]) - (coeff * ((c[2])[0])));
  (c[3])[1] = (((c[3])[1]) - (coeff * ((c[2])[1])));
  (c[3])[2] = (((c[3])[2]) - (coeff * ((c[2])[2])));
  (c[3])[3] = (((c[3])[3]) - (coeff * ((c[2])[3])));
  (c[3])[4] = (((c[3])[4]) - (coeff * ((c[2])[4])));
  r[3] = ((r[3]) - (coeff * (r[2])));
  coeff = ((lhs[4])[2]);
  (lhs[4])[3] = (((lhs[4])[3]) - (coeff * ((lhs[2])[3])));
  (lhs[4])[4] = (((lhs[4])[4]) - (coeff * ((lhs[2])[4])));
  (c[4])[0] = (((c[4])[0]) - (coeff * ((c[2])[0])));
  (c[4])[1] = (((c[4])[1]) - (coeff * ((c[2])[1])));
  (c[4])[2] = (((c[4])[2]) - (coeff * ((c[2])[2])));
  (c[4])[3] = (((c[4])[3]) - (coeff * ((c[2])[3])));
  (c[4])[4] = (((c[4])[4]) - (coeff * ((c[2])[4])));
  r[4] = ((r[4]) - (coeff * (r[2])));
  pivot = (1.0 / ((lhs[3])[3]));
  (lhs[3])[4] = (((lhs[3])[4]) * pivot);
  (c[3])[0] = (((c[3])[0]) * pivot);
  (c[3])[1] = (((c[3])[1]) * pivot);
  (c[3])[2] = (((c[3])[2]) * pivot);
  (c[3])[3] = (((c[3])[3]) * pivot);
  (c[3])[4] = (((c[3])[4]) * pivot);
  r[3] = ((r[3]) * pivot);
  coeff = ((lhs[0])[3]);
  (lhs[0])[4] = (((lhs[0])[4]) - (coeff * ((lhs[3])[4])));
  (c[0])[0] = (((c[0])[0]) - (coeff * ((c[3])[0])));
  (c[0])[1] = (((c[0])[1]) - (coeff * ((c[3])[1])));
  (c[0])[2] = (((c[0])[2]) - (coeff * ((c[3])[2])));
  (c[0])[3] = (((c[0])[3]) - (coeff * ((c[3])[3])));
  (c[0])[4] = (((c[0])[4]) - (coeff * ((c[3])[4])));
  r[0] = ((r[0]) - (coeff * (r[3])));
  coeff = ((lhs[1])[3]);
  (lhs[1])[4] = (((lhs[1])[4]) - (coeff * ((lhs[3])[4])));
  (c[1])[0] = (((c[1])[0]) - (coeff * ((c[3])[0])));
  (c[1])[1] = (((c[1])[1]) - (coeff * ((c[3])[1])));
  (c[1])[2] = (((c[1])[2]) - (coeff * ((c[3])[2])));
  (c[1])[3] = (((c[1])[3]) - (coeff * ((c[3])[3])));
  (c[1])[4] = (((c[1])[4]) - (coeff * ((c[3])[4])));
  r[1] = ((r[1]) - (coeff * (r[3])));
  coeff = ((lhs[2])[3]);
  (lhs[2])[4] = (((lhs[2])[4]) - (coeff * ((lhs[3])[4])));
  (c[2])[0] = (((c[2])[0]) - (coeff * ((c[3])[0])));
  (c[2])[1] = (((c[2])[1]) - (coeff * ((c[3])[1])));
  (c[2])[2] = (((c[2])[2]) - (coeff * ((c[3])[2])));
  (c[2])[3] = (((c[2])[3]) - (coeff * ((c[3])[3])));
  (c[2])[4] = (((c[2])[4]) - (coeff * ((c[3])[4])));
  r[2] = ((r[2]) - (coeff * (r[3])));
  coeff = ((lhs[4])[3]);
  (lhs[4])[4] = (((lhs[4])[4]) - (coeff * ((lhs[3])[4])));
  (c[4])[0] = (((c[4])[0]) - (coeff * ((c[3])[0])));
  (c[4])[1] = (((c[4])[1]) - (coeff * ((c[3])[1])));
  (c[4])[2] = (((c[4])[2]) - (coeff * ((c[3])[2])));
  (c[4])[3] = (((c[4])[3]) - (coeff * ((c[3])[3])));
  (c[4])[4] = (((c[4])[4]) - (coeff * ((c[3])[4])));
  r[4] = ((r[4]) - (coeff * (r[3])));
  pivot = (1.0 / ((lhs[4])[4]));
  (c[4])[0] = (((c[4])[0]) * pivot);
  (c[4])[1] = (((c[4])[1]) * pivot);
  (c[4])[2] = (((c[4])[2]) * pivot);
  (c[4])[3] = (((c[4])[3]) * pivot);
  (c[4])[4] = (((c[4])[4]) * pivot);
  r[4] = ((r[4]) * pivot);
  coeff = ((lhs[0])[4]);
  (c[0])[0] = (((c[0])[0]) - (coeff * ((c[4])[0])));
  (c[0])[1] = (((c[0])[1]) - (coeff * ((c[4])[1])));
  (c[0])[2] = (((c[0])[2]) - (coeff * ((c[4])[2])));
  (c[0])[3] = (((c[0])[3]) - (coeff * ((c[4])[3])));
  (c[0])[4] = (((c[0])[4]) - (coeff * ((c[4])[4])));
  r[0] = ((r[0]) - (coeff * (r[4])));
  coeff = ((lhs[1])[4]);
  (c[1])[0] = (((c[1])[0]) - (coeff * ((c[4])[0])));
  (c[1])[1] = (((c[1])[1]) - (coeff * ((c[4])[1])));
  (c[1])[2] = (((c[1])[2]) - (coeff * ((c[4])[2])));
  (c[1])[3] = (((c[1])[3]) - (coeff * ((c[4])[3])));
  (c[1])[4] = (((c[1])[4]) - (coeff * ((c[4])[4])));
  r[1] = ((r[1]) - (coeff * (r[4])));
  coeff = ((lhs[2])[4]);
  (c[2])[0] = (((c[2])[0]) - (coeff * ((c[4])[0])));
  (c[2])[1] = (((c[2])[1]) - (coeff * ((c[4])[1])));
  (c[2])[2] = (((c[2])[2]) - (coeff * ((c[4])[2])));
  (c[2])[3] = (((c[2])[3]) - (coeff * ((c[4])[3])));
  (c[2])[4] = (((c[2])[4]) - (coeff * ((c[4])[4])));
  r[2] = ((r[2]) - (coeff * (r[4])));
  coeff = ((lhs[3])[4]);
  (c[3])[0] = (((c[3])[0]) - (coeff * ((c[4])[0])));
  (c[3])[1] = (((c[3])[1]) - (coeff * ((c[4])[1])));
  (c[3])[2] = (((c[3])[2]) - (coeff * ((c[4])[2])));
  (c[3])[3] = (((c[3])[3]) - (coeff * ((c[4])[3])));
  (c[3])[4] = (((c[3])[4]) - (coeff * ((c[4])[4])));
  r[3] = ((r[3]) - (coeff * (r[4])));
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void binvrhs(double (*lhs)[5UL],double *r)
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
  double pivot;
  double coeff;
/*--------------------------------------------------------------------
c     
c-------------------------------------------------------------------*/
  pivot = (1.0 / ((lhs[0])[0]));
  (lhs[0])[1] = (((lhs[0])[1]) * pivot);
  (lhs[0])[2] = (((lhs[0])[2]) * pivot);
  (lhs[0])[3] = (((lhs[0])[3]) * pivot);
  (lhs[0])[4] = (((lhs[0])[4]) * pivot);
  r[0] = ((r[0]) * pivot);
  coeff = ((lhs[1])[0]);
  (lhs[1])[1] = (((lhs[1])[1]) - (coeff * ((lhs[0])[1])));
  (lhs[1])[2] = (((lhs[1])[2]) - (coeff * ((lhs[0])[2])));
  (lhs[1])[3] = (((lhs[1])[3]) - (coeff * ((lhs[0])[3])));
  (lhs[1])[4] = (((lhs[1])[4]) - (coeff * ((lhs[0])[4])));
  r[1] = ((r[1]) - (coeff * (r[0])));
  coeff = ((lhs[2])[0]);
  (lhs[2])[1] = (((lhs[2])[1]) - (coeff * ((lhs[0])[1])));
  (lhs[2])[2] = (((lhs[2])[2]) - (coeff * ((lhs[0])[2])));
  (lhs[2])[3] = (((lhs[2])[3]) - (coeff * ((lhs[0])[3])));
  (lhs[2])[4] = (((lhs[2])[4]) - (coeff * ((lhs[0])[4])));
  r[2] = ((r[2]) - (coeff * (r[0])));
  coeff = ((lhs[3])[0]);
  (lhs[3])[1] = (((lhs[3])[1]) - (coeff * ((lhs[0])[1])));
  (lhs[3])[2] = (((lhs[3])[2]) - (coeff * ((lhs[0])[2])));
  (lhs[3])[3] = (((lhs[3])[3]) - (coeff * ((lhs[0])[3])));
  (lhs[3])[4] = (((lhs[3])[4]) - (coeff * ((lhs[0])[4])));
  r[3] = ((r[3]) - (coeff * (r[0])));
  coeff = ((lhs[4])[0]);
  (lhs[4])[1] = (((lhs[4])[1]) - (coeff * ((lhs[0])[1])));
  (lhs[4])[2] = (((lhs[4])[2]) - (coeff * ((lhs[0])[2])));
  (lhs[4])[3] = (((lhs[4])[3]) - (coeff * ((lhs[0])[3])));
  (lhs[4])[4] = (((lhs[4])[4]) - (coeff * ((lhs[0])[4])));
  r[4] = ((r[4]) - (coeff * (r[0])));
  pivot = (1.0 / ((lhs[1])[1]));
  (lhs[1])[2] = (((lhs[1])[2]) * pivot);
  (lhs[1])[3] = (((lhs[1])[3]) * pivot);
  (lhs[1])[4] = (((lhs[1])[4]) * pivot);
  r[1] = ((r[1]) * pivot);
  coeff = ((lhs[0])[1]);
  (lhs[0])[2] = (((lhs[0])[2]) - (coeff * ((lhs[1])[2])));
  (lhs[0])[3] = (((lhs[0])[3]) - (coeff * ((lhs[1])[3])));
  (lhs[0])[4] = (((lhs[0])[4]) - (coeff * ((lhs[1])[4])));
  r[0] = ((r[0]) - (coeff * (r[1])));
  coeff = ((lhs[2])[1]);
  (lhs[2])[2] = (((lhs[2])[2]) - (coeff * ((lhs[1])[2])));
  (lhs[2])[3] = (((lhs[2])[3]) - (coeff * ((lhs[1])[3])));
  (lhs[2])[4] = (((lhs[2])[4]) - (coeff * ((lhs[1])[4])));
  r[2] = ((r[2]) - (coeff * (r[1])));
  coeff = ((lhs[3])[1]);
  (lhs[3])[2] = (((lhs[3])[2]) - (coeff * ((lhs[1])[2])));
  (lhs[3])[3] = (((lhs[3])[3]) - (coeff * ((lhs[1])[3])));
  (lhs[3])[4] = (((lhs[3])[4]) - (coeff * ((lhs[1])[4])));
  r[3] = ((r[3]) - (coeff * (r[1])));
  coeff = ((lhs[4])[1]);
  (lhs[4])[2] = (((lhs[4])[2]) - (coeff * ((lhs[1])[2])));
  (lhs[4])[3] = (((lhs[4])[3]) - (coeff * ((lhs[1])[3])));
  (lhs[4])[4] = (((lhs[4])[4]) - (coeff * ((lhs[1])[4])));
  r[4] = ((r[4]) - (coeff * (r[1])));
  pivot = (1.0 / ((lhs[2])[2]));
  (lhs[2])[3] = (((lhs[2])[3]) * pivot);
  (lhs[2])[4] = (((lhs[2])[4]) * pivot);
  r[2] = ((r[2]) * pivot);
  coeff = ((lhs[0])[2]);
  (lhs[0])[3] = (((lhs[0])[3]) - (coeff * ((lhs[2])[3])));
  (lhs[0])[4] = (((lhs[0])[4]) - (coeff * ((lhs[2])[4])));
  r[0] = ((r[0]) - (coeff * (r[2])));
  coeff = ((lhs[1])[2]);
  (lhs[1])[3] = (((lhs[1])[3]) - (coeff * ((lhs[2])[3])));
  (lhs[1])[4] = (((lhs[1])[4]) - (coeff * ((lhs[2])[4])));
  r[1] = ((r[1]) - (coeff * (r[2])));
  coeff = ((lhs[3])[2]);
  (lhs[3])[3] = (((lhs[3])[3]) - (coeff * ((lhs[2])[3])));
  (lhs[3])[4] = (((lhs[3])[4]) - (coeff * ((lhs[2])[4])));
  r[3] = ((r[3]) - (coeff * (r[2])));
  coeff = ((lhs[4])[2]);
  (lhs[4])[3] = (((lhs[4])[3]) - (coeff * ((lhs[2])[3])));
  (lhs[4])[4] = (((lhs[4])[4]) - (coeff * ((lhs[2])[4])));
  r[4] = ((r[4]) - (coeff * (r[2])));
  pivot = (1.0 / ((lhs[3])[3]));
  (lhs[3])[4] = (((lhs[3])[4]) * pivot);
  r[3] = ((r[3]) * pivot);
  coeff = ((lhs[0])[3]);
  (lhs[0])[4] = (((lhs[0])[4]) - (coeff * ((lhs[3])[4])));
  r[0] = ((r[0]) - (coeff * (r[3])));
  coeff = ((lhs[1])[3]);
  (lhs[1])[4] = (((lhs[1])[4]) - (coeff * ((lhs[3])[4])));
  r[1] = ((r[1]) - (coeff * (r[3])));
  coeff = ((lhs[2])[3]);
  (lhs[2])[4] = (((lhs[2])[4]) - (coeff * ((lhs[3])[4])));
  r[2] = ((r[2]) - (coeff * (r[3])));
  coeff = ((lhs[4])[3]);
  (lhs[4])[4] = (((lhs[4])[4]) - (coeff * ((lhs[3])[4])));
  r[4] = ((r[4]) - (coeff * (r[3])));
  pivot = (1.0 / ((lhs[4])[4]));
  r[4] = ((r[4]) * pivot);
  coeff = ((lhs[0])[4]);
  r[0] = ((r[0]) - (coeff * (r[4])));
  coeff = ((lhs[1])[4]);
  r[1] = ((r[1]) - (coeff * (r[4])));
  coeff = ((lhs[2])[4]);
  r[2] = ((r[2]) - (coeff * (r[4])));
  coeff = ((lhs[3])[4]);
  r[3] = ((r[3]) - (coeff * (r[4])));
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void y_solve()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     Performs line solves in Y direction by first factoring
c     the block-tridiagonal matrix into an upper triangular matrix][ 
c     and then performing back substitution to solve for the unknow
c     vectors of each line.  
c     
c     Make sure we treat elements zero to cell_size in the direction
c     of the sweep.
c-------------------------------------------------------------------*/
  lhsy();
  y_solve_cell();
  y_backsubstitute();
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void y_backsubstitute()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     back solve: if last cell][ then generate U(jsize)=rhs(jsize)
c     else assume U(jsize) is loaded in un pack backsub_info
c     so just use it
c     after call u(jstart) will be sent to next cell
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
  int m;
  int n;
  for (j = ((grid_points[1]) - 2); j >= 0; j--) {{
      int _p_loop_index;
      int _p_loop_lower;
      int _p_loop_upper;
      int _p_loop_stride;
      _p_loop_lower = 1;
      _p_loop_upper = ((grid_points[0]) - 1);
      _p_loop_stride = 1;
      _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
      for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
          for (m = 0; m < 5; m++) {
            for (n = 0; n < 5; n++) {
              (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (((((((lhs[_p_loop_index])[j])[k])[2])[m])[n]) * ((((rhs[_p_loop_index])[j + 1])[k])[n])));
            }
          }
        }
      }
      _ompc_barrier();
    }
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void y_solve_cell()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     performs guaussian elimination on this cell.
c     
c     assumes that unpacking routines for non-first cells 
c     preload C' and rhs' from previous cell.
c     
c     assumed send happens outside this routine, but that
c     c'(JMAX) and rhs'(JMAX) will be sent to next cell
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
  int jsize;
  jsize = ((grid_points[1]) - 1);
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
/*--------------------------------------------------------------------
c     multiply c(i,0,k) by b_inverse and copy back to c
c     multiply rhs(0) by b_inverse(0) and copy to rhs
c-------------------------------------------------------------------*/
        binvcrhs(((((lhs[_p_loop_index])[0])[k])[1]),((((lhs[_p_loop_index])[0])[k])[2]),(((rhs[_p_loop_index])[0])[k]));
      }
    }
    _ompc_barrier();
  }
/*--------------------------------------------------------------------
c     begin inner most do loop
c     do all the elements of the cell unless last 
c-------------------------------------------------------------------*/
  for (j = 1; j < jsize; j++) {{
      int _p_loop_index;
      int _p_loop_lower;
      int _p_loop_upper;
      int _p_loop_stride;
      _p_loop_lower = 1;
      _p_loop_upper = ((grid_points[0]) - 1);
      _p_loop_stride = 1;
      _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
      for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
        for (k = 1; k < ((grid_points[2]) - 1); k++) {
/*--------------------------------------------------------------------
c     subtract A*lhs_vector(j-1) from lhs_vector(j)
c     
c     rhs(j) = rhs(j) - A*rhs(j-1)
c-------------------------------------------------------------------*/
          matvec_sub(((((lhs[_p_loop_index])[j])[k])[0]),(((rhs[_p_loop_index])[j - 1])[k]),(((rhs[_p_loop_index])[j])[k]));
/*--------------------------------------------------------------------
c     B(j) = B(j) - C(j-1)*A(j)
c-------------------------------------------------------------------*/
          matmul_sub(((((lhs[_p_loop_index])[j])[k])[0]),((((lhs[_p_loop_index])[j - 1])[k])[2]),((((lhs[_p_loop_index])[j])[k])[1]));
/*--------------------------------------------------------------------
c     multiply c(i,j,k) by b_inverse and copy back to c
c     multiply rhs(i,1,k) by b_inverse(i,1,k) and copy to rhs
c-------------------------------------------------------------------*/
          binvcrhs(((((lhs[_p_loop_index])[j])[k])[1]),((((lhs[_p_loop_index])[j])[k])[2]),(((rhs[_p_loop_index])[j])[k]));
        }
      }
      _ompc_barrier();
    }
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (k = 1; k < ((grid_points[2]) - 1); k++) {
/*--------------------------------------------------------------------
c     rhs(jsize) = rhs(jsize) - A*rhs(jsize-1)
c-------------------------------------------------------------------*/
        matvec_sub(((((lhs[_p_loop_index])[jsize])[k])[0]),(((rhs[_p_loop_index])[jsize - 1])[k]),(((rhs[_p_loop_index])[jsize])[k]));
/*--------------------------------------------------------------------
c     B(jsize) = B(jsize) - C(jsize-1)*A(jsize)
c     call matmul_sub(aa,i,jsize,k,c,
c     $              cc,i,jsize-1,k,c,BB,i,jsize,k)
c-------------------------------------------------------------------*/
        matmul_sub(((((lhs[_p_loop_index])[jsize])[k])[0]),((((lhs[_p_loop_index])[jsize - 1])[k])[2]),((((lhs[_p_loop_index])[jsize])[k])[1]));
/*--------------------------------------------------------------------
c     multiply rhs(jsize) by b_inverse(jsize) and copy to rhs
c-------------------------------------------------------------------*/
        binvrhs(((((lhs[_p_loop_index])[jsize])[k])[1]),(((rhs[_p_loop_index])[jsize])[k]));
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void z_solve()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     Performs line solves in Z direction by first factoring
c     the block-tridiagonal matrix into an upper triangular matrix, 
c     and then performing back substitution to solve for the unknow
c     vectors of each line.  
c     
c     Make sure we treat elements zero to cell_size in the direction
c     of the sweep.
c-------------------------------------------------------------------*/
  lhsz();
  z_solve_cell();
  z_backsubstitute();
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void z_backsubstitute()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     back solve: if last cell, then generate U(ksize)=rhs(ksize)
c     else assume U(ksize) is loaded in un pack backsub_info
c     so just use it
c     after call u(kstart) will be sent to next cell
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
  int m;
  int n;
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
        for (k = ((grid_points[2]) - 2); k >= 0; k--) {
          for (m = 0; m < 5; m++) {
            for (n = 0; n < 5; n++) {
              (((rhs[_p_loop_index])[j])[k])[m] = (((((rhs[_p_loop_index])[j])[k])[m]) - (((((((lhs[_p_loop_index])[j])[k])[2])[m])[n]) * ((((rhs[_p_loop_index])[j])[k + 1])[n])));
            }
          }
        }
      }
    }
    _ompc_barrier();
  }
}

/*--------------------------------------------------------------------
--------------------------------------------------------------------*/

static void z_solve_cell()
{
/*--------------------------------------------------------------------
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
c     performs guaussian elimination on this cell.
c     
c     assumes that unpacking routines for non-first cells 
c     preload C' and rhs' from previous cell.
c     
c     assumed send happens outside this routine, but that
c     c'(KMAX) and rhs'(KMAX) will be sent to next cell.
c-------------------------------------------------------------------*/
  int i;
  int j;
  int k;
  int ksize;
  ksize = ((grid_points[2]) - 1);
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
/*--------------------------------------------------------------------
c     multiply c(i,j,0) by b_inverse and copy back to c
c     multiply rhs(0) by b_inverse(0) and copy to rhs
c-------------------------------------------------------------------*/
        binvcrhs(((((lhs[_p_loop_index])[j])[0])[1]),((((lhs[_p_loop_index])[j])[0])[2]),(((rhs[_p_loop_index])[j])[0]));
      }
    }
    _ompc_barrier();
  }
/*--------------------------------------------------------------------
c     begin inner most do loop
c     do all the elements of the cell unless last 
c-------------------------------------------------------------------*/
  for (k = 1; k < ksize; k++) {{
      int _p_loop_index;
      int _p_loop_lower;
      int _p_loop_upper;
      int _p_loop_stride;
      _p_loop_lower = 1;
      _p_loop_upper = ((grid_points[0]) - 1);
      _p_loop_stride = 1;
      _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
      for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
        for (j = 1; j < ((grid_points[1]) - 1); j++) {
/*--------------------------------------------------------------------
c     subtract A*lhs_vector(k-1) from lhs_vector(k)
c     
c     rhs(k) = rhs(k) - A*rhs(k-1)
c-------------------------------------------------------------------*/
          matvec_sub(((((lhs[_p_loop_index])[j])[k])[0]),(((rhs[_p_loop_index])[j])[k - 1]),(((rhs[_p_loop_index])[j])[k]));
/*--------------------------------------------------------------------
c     B(k) = B(k) - C(k-1)*A(k)
c     call matmul_sub(aa,i,j,k,c,cc,i,j,k-1,c,BB,i,j,k)
c-------------------------------------------------------------------*/
          matmul_sub(((((lhs[_p_loop_index])[j])[k])[0]),((((lhs[_p_loop_index])[j])[k - 1])[2]),((((lhs[_p_loop_index])[j])[k])[1]));
/*--------------------------------------------------------------------
c     multiply c(i,j,k) by b_inverse and copy back to c
c     multiply rhs(i,j,1) by b_inverse(i,j,1) and copy to rhs
c-------------------------------------------------------------------*/
          binvcrhs(((((lhs[_p_loop_index])[j])[k])[1]),((((lhs[_p_loop_index])[j])[k])[2]),(((rhs[_p_loop_index])[j])[k]));
        }
      }
      _ompc_barrier();
    }
  }
{
    int _p_loop_index;
    int _p_loop_lower;
    int _p_loop_upper;
    int _p_loop_stride;
    _p_loop_lower = 1;
    _p_loop_upper = ((grid_points[0]) - 1);
    _p_loop_stride = 1;
    _ompc_default_sched(&_p_loop_lower,&_p_loop_upper,&_p_loop_stride);
    for (_p_loop_index = _p_loop_lower; _p_loop_index < _p_loop_upper; _p_loop_index += _p_loop_stride) {
      for (j = 1; j < ((grid_points[1]) - 1); j++) {
/*--------------------------------------------------------------------
c     rhs(ksize) = rhs(ksize) - A*rhs(ksize-1)
c-------------------------------------------------------------------*/
        matvec_sub(((((lhs[_p_loop_index])[j])[ksize])[0]),(((rhs[_p_loop_index])[j])[ksize - 1]),(((rhs[_p_loop_index])[j])[ksize]));
/*--------------------------------------------------------------------
c     B(ksize) = B(ksize) - C(ksize-1)*A(ksize)
c     call matmul_sub(aa,i,j,ksize,c,
c     $              cc,i,j,ksize-1,c,BB,i,j,ksize)
c-------------------------------------------------------------------*/
        matmul_sub(((((lhs[_p_loop_index])[j])[ksize])[0]),((((lhs[_p_loop_index])[j])[ksize - 1])[2]),((((lhs[_p_loop_index])[j])[ksize])[1]));
/*--------------------------------------------------------------------
c     multiply rhs(ksize) by b_inverse(ksize) and copy to rhs
c-------------------------------------------------------------------*/
        binvrhs(((((lhs[_p_loop_index])[j])[ksize])[1]),(((rhs[_p_loop_index])[j])[ksize]));
      }
    }
    _ompc_barrier();
  }
}


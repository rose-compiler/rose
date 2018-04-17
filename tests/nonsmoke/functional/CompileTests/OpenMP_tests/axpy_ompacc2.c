// Add example of writing multiple device code using OpenMP 4.0
//
/* change this to do saxpy or daxpy : single precision or double precision*/
#include <omp.h>
#define REAL double

#ifdef __cplusplus
extern "C" {
#endif

/* both the omp version and ompacc version */
extern void axpy_omp(REAL* x, REAL* y, int n, REAL a); 
extern void axpy_ompacc(REAL* x, REAL* y, int n, REAL a); 
extern double read_timer(); /* in second */
extern double read_timer_ms(); /* in ms */
#ifdef __cplusplus
 }
#endif

/* standard one-dev support */
void axpy_ompacc(REAL* x, REAL* y, int n, REAL a) {
  int i;
   /* this one defines both the target device name and data environment to map to,
      I think here we need mechanism to tell the compiler the device type (could be multiple) so that compiler can generate the codes of different versions; 
      we also need to let the runtime know what the target device is so the runtime will chose the right function to call if the code are generated 
      #pragma omp target device (gpu0) map(x, y) 
   */
  #pragma omp target device (gpu0) map(tofrom: y[0:n]) map(to: x[0:n],a,n)
  #pragma omp parallel for shared(x, y, n, a) private(i)
  for (i = 0; i < n; ++i)
    y[i] += a * x[i];
}

/* version 1: use omp parallel, i.e. each host thread responsible for one dev */
void axpy_mdev_v1(REAL* x, REAL* y, int n, REAL a) {
  int ndev = omp_get_num_devices(); /* standard omp call, see ticket 167 */
  #pragma omp parallel num_threads(ndev)
  {
        int i;
        /* chunking it for each device */
        int devid = omp_get_thread_num();
        int remain = n % ndev;
        int esize = n / ndev;
        int partsize, starti, endi;
        if (devid < remain) { /* each of the first remain dev has one more element */
                partsize = esize+1;
                starti = partsize*devid;
        } else {
                partsize = esize;
                starti = esize*devid+remain;
        }
        endi=starti + partsize;

#pragma omp target device (devid) map(tofrom: y[starti:endi]) map(to: x[starti:endi],a,partsize)
#pragma omp parallel for shared(x, y, partsize, a) private(i)
        for (i = 0; i < partsize; ++i)
          y[i] += a * x[i];
  }
}

#include "npbparams.h"


/*
c If processor array is 1x1 -> 0D grid decomposition


c Cache blocking params. These values are good for most
c RISC processors.  
c FFT parameters:
c  fftblock controls how many ffts are done at a time. 
c  The default is appropriate for most cache-based machines
c  On vector machines, the FFT can be vectorized with vector
c  length equal to the block size, so the block size should
c  be as large as possible. This is the size of the smallest
c  dimension of the problem: 128 for class A, 256 for class B and
c  512 for class C.
*/

#define	FFTBLOCK_DEFAULT	16
#define	FFTBLOCKPAD_DEFAULT	18

#define FFTBLOCK	FFTBLOCK_DEFAULT
#define FFTBLOCKPAD	FFTBLOCKPAD_DEFAULT

/* COMMON block: blockinfo */
int fftblock;
int fftblockpad;
      
/*
c we need a bunch of logic to keep track of how
c arrays are laid out. 


c Note: this serial version is the derived from the parallel 0D case
c of the ft NPB.
c The computation proceeds logically as

c set up initial conditions
c fftx(1)
c transpose (1->2)
c ffty(2)
c transpose (2->3)
c fftz(3)
c time evolution
c fftz(3)
c transpose (3->2)
c ffty(2)
c transpose (2->1)
c fftx(1)
c compute residual(1)

c for the 0D, 1D, 2D strategies, the layouts look like xxx
c        
c            0D        1D        2D
c 1:        xyz       xyz       xyz
c 2:        xyz       xyz       yxz
c 3:        xyz       zyx       zxy

c the array dimensions are stored in dims(coord, phase)
*/

/* COMMON block: layout */
static int dims[3][3];
static int xstart[3];
static int ystart[3];
static int zstart[3];
static int xend[3];
static int yend[3];
static int zend[3];

#define	T_TOTAL		0
#define	T_SETUP		1
#define	T_FFT		2
#define	T_EVOLVE	3
#define	T_CHECKSUM	4
#define	T_FFTLOW	5
#define	T_FFTCOPY	6
#define	T_MAX		7

#define	TIMERS_ENABLED	FALSE

/* other stuff */

#define	SEED	314159265.0
#define	A	1220703125.0
#define	PI	3.141592653589793238
#define	ALPHA	1.0e-6

#define	EXPMAX	(NITER_DEFAULT*(NX*NX/4+NY*NY/4+NZ*NZ/4))

/* COMMON block: excomm */
static double ex[EXPMAX+1];	/* ex(0:expmax) */

/*
c roots of unity array
c relies on x being largest dimension?
*/

/* COMMON block: ucomm */
static dcomplex u[NX];

/* for checksum data */

/* COMMON block: sumcomm */
static dcomplex sums[NITER_DEFAULT+1]; /* sums(0:niter_default) */

/* number of iterations*/

/* COMMON block: iter */
static int niter;


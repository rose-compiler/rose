#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#include "instrument.h"

/* Default problem size. */
#ifndef CZ
# define CZ 256
#endif
#ifndef CYM
# define CYM 256
#endif
#ifndef CXM
# define CXM 256
#endif

/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif

/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
DATA_TYPE ch;
DATA_TYPE mui;
#ifndef POLYBENCH_TEST_MALLOC
DATA_TYPE Ex[CZ + 1][CYM + 1][CXM + 1];
DATA_TYPE Ey[CZ + 1][CYM + 1][CXM + 1];
DATA_TYPE Bza[CZ + 1][CYM + 1][CXM + 1];
DATA_TYPE Hz[CZ + 1][CYM + 1][CXM + 1];
DATA_TYPE clf[CZ + 1][CYM + 1];
DATA_TYPE tmp[CZ + 1][CYM + 1];
DATA_TYPE Ry[CZ + 1][CYM + 1];
DATA_TYPE Ax[CZ + 1][CYM + 1];
DATA_TYPE cymh[CYM + 1];
DATA_TYPE cyph[CYM + 1];
DATA_TYPE cxmh[CXM + 1];
DATA_TYPE cxph[CXM + 1];
DATA_TYPE czm[CZ + 1];
DATA_TYPE czp[CZ + 1];
#else
DATA_TYPE* czm = (DATA_TYPE*)malloc((CZ + 1) * sizeof(DATA_TYPE));
DATA_TYPE* czp = (DATA_TYPE*)malloc((CZ + 1) * sizeof(DATA_TYPE));
DATA_TYPE* cxmh = (DATA_TYPE*)malloc((CXM + 1) * sizeof(DATA_TYPE));
DATA_TYPE* cxph = (DATA_TYPE*)malloc((CXM + 1) * sizeof(DATA_TYPE));
DATA_TYPE* cymh = (DATA_TYPE*)malloc((CYM + 1) * sizeof(DATA_TYPE));
DATA_TYPE* cymp = (DATA_TYPE*)malloc((CYM + 1) * sizeof(DATA_TYPE));
DATA_TYPE** clf = (DATA_TYPE**)malloc((CZ + 1) * sizeof(DATA_TYPE*));
DATA_TYPE** tmp = (DATA_TYPE**)malloc((CZ + 1) * sizeof(DATA_TYPE*));
DATA_TYPE** Ry = (DATA_TYPE**)malloc((CZ + 1) * sizeof(DATA_TYPE*));
DATA_TYPE** Ax = (DATA_TYPE**)malloc((CZ + 1) * sizeof(DATA_TYPE*));
DATA_TYPE*** Hz = (DATA_TYPE***)malloc((CZ + 1) * sizeof(DATA_TYPE**));
DATA_TYPE*** Bza = (DATA_TYPE***)malloc((CZ + 1) * sizeof(DATA_TYPE**));
DATA_TYPE*** Ex = (DATA_TYPE***)malloc((CZ + 1) * sizeof(DATA_TYPE**));
DATA_TYPE*** Ey = (DATA_TYPE***)malloc((CZ + 1) * sizeof(DATA_TYPE**));
{
  int i, j;
  for (i = 0; i <= CZ; ++i)
    {
      clf[i] = (DATA_TYPE*)malloc((CYM + 1) * sizeof(DATA_TYPE));
      tmp[i] = (DATA_TYPE*)malloc((CYM + 1) * sizeof(DATA_TYPE));
      Ry[i] = (DATA_TYPE*)malloc((CYM + 1) * sizeof(DATA_TYPE));
      Ax[i] = (DATA_TYPE*)malloc((CYM + 1) * sizeof(DATA_TYPE));
      Ex[i] = (DATA_TYPE**)malloc((CYM + 1) * sizeof(DATA_TYPE*));
      Ey[i] = (DATA_TYPE**)malloc((CYM + 1) * sizeof(DATA_TYPE*));
      Bza[i] = (DATA_TYPE**)malloc((CYM + 1) * sizeof(DATA_TYPE*));
      Hz[i] = (DATA_TYPE**)malloc((CYM + 1) * sizeof(DATA_TYPE*));
      for (j = 0; j <= CYM; ++j)
	{
	  Ex[i][j] = (DATA_TYPE*)malloc((CXM + 1) * sizeof(DATA_TYPE));
	  Ey[i][j] = (DATA_TYPE*)malloc((CXM + 1) * sizeof(DATA_TYPE));
	  Bza[i][j] = (DATA_TYPE*)malloc((CXM + 1) * sizeof(DATA_TYPE));
	  Hz[i][j] = (DATA_TYPE*)malloc((CXM + 1) * sizeof(DATA_TYPE));
	}
    }
}
#endif

inline
void init_array()
{
  int i, j, k;

  mui = 2341;
  ch = 42;
  for (i = 0; i <= CZ; i++)
    {
      czm[i] = ((DATA_TYPE) i + 1) / CXM;
      czp[i] = ((DATA_TYPE) i + 2) / CXM;
    }
  for (i = 0; i <= CXM; i++)
    {
      cxmh[i] = ((DATA_TYPE) i + 3) / CXM;
      cxph[i] = ((DATA_TYPE) i + 4) / CXM;
    }
  for (i = 0; i <= CYM; i++)
    {
      cymh[i] = ((DATA_TYPE) i + 5) / CXM;
      cyph[i] = ((DATA_TYPE) i + 6) / CXM;
    }

  for (i = 0; i <= CZ; i++)
    for (j = 0; j <= CYM; j++)
      {
	Ry[i][j] = ((DATA_TYPE) i*j + 10) / CYM;
	Ax[i][j] = ((DATA_TYPE) i*j + 11) / CYM;
	for (k = 0; k <= CXM; k++)
	  {
	    Ex[i][j][k] = ((DATA_TYPE) i*j + k + 1) / CXM;
	    Ey[i][j][k] = ((DATA_TYPE) i*j + k + 2) / CYM;
	    Hz[i][j][k] = ((DATA_TYPE) i*j + k + 3) / CZ;
	  }
      }
}

/* Define the live-out variables. Code is not executed unless
   POLYBENCH_DUMP_ARRAYS is defined. */
inline
void print_array(int argc, char** argv)
{
  int i, j, k;
#ifndef POLYBENCH_DUMP_ARRAYS
  if (argc > 42 && ! strcmp(argv[0], ""))
#endif
    {
      for (i = 0; i <= CZ; i++)
	for (j = 0; j <= CYM; j++)
	  for (k = 0; k <= CXM; k++) {
	    fprintf(stderr, DATA_PRINTF_MODIFIER, Bza[i][j][k]);
	    fprintf(stderr, DATA_PRINTF_MODIFIER, Ex[i][j][k]);
	    fprintf(stderr, DATA_PRINTF_MODIFIER, Ey[i][j][k]);
	    fprintf(stderr, DATA_PRINTF_MODIFIER, Hz[i][j][k]);
	    if ((i * CXM + j) % 80 == 20) fprintf(stderr, "\n");
	  }
      fprintf(stderr, "\n");
    }
}


int main(int argc, char** argv)
{
  int iz, iy, ix;
  int Cz = CZ;
  int Cym = CYM;
  int Cxm = CXM;

  /* Initialize array. */
  init_array();

  /* Start timer. */
  polybench_start_instruments;

#pragma scop
#pragma live-out Bza, Hz

  for (iz = 0; iz < Cz; iz++)
    {
      for (iy = 0; iy < Cym; iy++)
	{
	  for (ix = 0; ix < Cxm; ix++)
	    {
	      clf[iz][iy] = Ex[iz][iy][ix] - Ex[iz][iy+1][ix] + Ey[iz][iy][ix+1] - Ey[iz][iy][ix];
	      tmp[iz][iy] = (cymh[iy] / cyph[iy]) * Bza[iz][iy][ix] - (ch / cyph[iy]) * clf[iz][iy];
	      Hz[iz][iy][ix] = (cxmh[ix] /cxph[ix]) * Hz[iz][iy][ix]
		+ (mui * czp[iz] / cxph[ix]) * tmp[iz][iy]
		- (mui * czm[iz] / cxph[ix]) * Bza[iz][iy][ix];
	      Bza[iz][iy][ix] = tmp[iz][iy];
	    }
	  clf[iz][iy] = Ex[iz][iy][Cxm] - Ex[iz][iy+1][Cxm] + Ry[iz][iy] - Ey[iz][iy][Cxm];
	  tmp[iz][iy] = (cymh[iy] / cyph[iy]) * Bza[iz][iy][Cxm] - (ch / cyph[iy]) * clf[iz][iy];
	  Hz[iz][iy][Cxm]=(cxmh[Cxm] / cxph[Cxm]) * Hz[iz][iy][Cxm]
	    + (mui * czp[iz] / cxph[Cxm]) * tmp[iz][iy]
	    - (mui * czm[iz] / cxph[Cxm]) * Bza[iz][iy][Cxm];
	  Bza[iz][iy][Cxm] = tmp[iz][iy];
	}
      for (ix = 0; ix < Cxm; ix++)
	{
	  clf[iz][iy] = Ex[iz][Cym][ix] - Ax[iz][ix] + Ey[iz][Cym][ix+1] - Ey[iz][Cym][ix];
	  tmp[iz][iy] = (cymh[Cym] / cyph[iy]) * Bza[iz][iy][ix] - (ch / cyph[iy]) * clf[iz][iy];
	  Hz[iz][Cym][ix] = (cxmh[ix] / cxph[ix]) * Hz[iz][Cym][ix]
	    + (mui * czp[iz] / cxph[ix]) * tmp[iz][iy]
	    - (mui * czm[iz] / cxph[ix]) * Bza[iz][Cym][ix];
	  Bza[iz][Cym][ix] = tmp[iz][iy];
	}
      clf[iz][iy] = Ex[iz][Cym][Cxm] - Ax[iz][Cxm] + Ry[iz][Cym] - Ey[iz][Cym][Cxm];
      tmp[iz][iy] = (cymh[Cym] / cyph[Cym]) * Bza[iz][Cym][Cxm] - (ch / cyph[Cym]) * clf[iz][iy];
      Hz[iz][Cym][Cxm] = (cxmh[Cxm] / cxph[Cxm]) * Hz[iz][Cym][Cxm]
	+ (mui * czp[iz] / cxph[Cxm]) * tmp[iz][iy]
	- (mui * czm[iz] / cxph[Cxm]) * Bza[iz][Cym][Cxm];
      Bza[iz][Cym][Cxm] = tmp[iz][iy];
    }

#pragma endscop

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  print_array(argc, argv);

  return 0;
}

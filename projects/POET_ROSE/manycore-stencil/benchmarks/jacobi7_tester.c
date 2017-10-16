#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef RANDSEED
#define RANDSEED 1
#endif


/* routine to test*/
void jacobi7(const int nx,const int ny,int nz,float * A0,const int timesteps,const float * B,const int ldb,float * Anext,const int ldc) 
;
/* macro for the value of routine parameter */

#ifndef TIME
#define TIME 1
#endif

#ifndef SIZE
#define SIZE 4
#endif
#define Index3D(_nx,_ny,_i,_j,_k) ((_i)+_nx*((_j)+_ny*(_k)))
void jacobi7_ref(const int nx,const int ny,int nz,float * A0,const int timesteps,const float * B,const int ldb,float * Anext,const int ldc) 
{
   float fac;
   float* temp_ptr;
   int i,j,k,t;
   fac = 6.0/(A0[0]*A0[0]);
   float* l0,* lnext;
   for (t=0; t<timesteps; t+=1) 
     {
        for (k=1; k<nz-1; k+=1) 
          {
             for (j=1; j<ny-1; j+=1) 
               {
                  for (i=1; i<nx-1; i+=1) 
                    {
                       if (t%2==0)  
                         {
                            l0 = A0;
                            lnext = Anext;
                         }
                       else 
                         {
                            lnext = A0;
                            l0 = Anext;
                         }
                       lnext[Index3D(nx,ny,i,j,k)] = l0[Index3D(nx,ny,i,j,k+1)]+l0[Index3D(nx,ny,i,j,k-1)]+l0[Index3D(nx,ny,i,j+1,k)]+l0[Index3D(nx,ny,i,j-1,k)]+l0[Index3D(nx,ny,i+1,j,k)]+l0[Index3D(nx,ny,i-1,j,k)]-l0[Index3D(nx,ny,i,j,k)]*fac;
                    }
               }
          }
     }
}

int main(int argc, char **argv) 
{
  
  /* induction variables */
  int __pt_i0, __pt_i1, __pt_i2;
  
  
  
  /* Declaring parameters of the routine */
  int nx;
  int ny;
  int nz;
  float* A0_comp;
  float* A0;
  int timesteps;
  float* B;
  int ldb;
  float* Anext_comp;
  float* Anext;
  int ldc;
  float* Anext_buf;
  int Anext_size;
  float* A0_buf;
  int A0_size;
  float* A0_comp_buf;
  int A0_comp_size;
  float* Anext_comp_buf;
  int Anext_comp_size;
  
  /* parameter initializations */
  srand(RANDSEED);
  ldc = 0;
  ldb = 0;
  B = 0;
  timesteps = TIME;
  nz = SIZE;
  ny = SIZE;
  nx = SIZE;
  Anext_size=nx*ny*nx;
  Anext_buf = (float*)calloc(Anext_size, sizeof(float));
  A0_size=Index3D(nx,ny,nx-1,ny-1,nz-1);
  A0_buf = (float*)calloc(A0_size, sizeof(float));
  A0_comp_size=Index3D(nx,ny,nx-1,ny-1,nz-1);
  A0_comp_buf = (float*)calloc(A0_comp_size, sizeof(float));
  A0_comp=A0_comp_buf;
  Anext_comp_size=Index3D(nx,ny,nx-1,ny-1,nz-1+1);
  Anext_comp_buf = (float*)calloc(Anext_comp_size, sizeof(float));
  Anext_comp = Anext_comp_buf;
  for (__pt_i0=0; __pt_i0<Anext_size; ++__pt_i0)
  {
    Anext_buf[__pt_i0] = rand();; 
  }
  Anext = Anext_buf;
  for (__pt_i0=0; __pt_i0<A0_size; ++__pt_i0)
  {
    A0_buf[__pt_i0] = rand();; 
  }
  A0 = A0_buf;
  Anext_comp = Anext_comp_buf;
  for (__pt_i0=0; __pt_i0<A0_size; ++__pt_i0)
  {
    A0_comp_buf[__pt_i0] = A0_buf[__pt_i0];
  }
  for (__pt_i0=0; __pt_i0<Anext_size; ++__pt_i0)
  {
    Anext_comp_buf[__pt_i0] = Anext_buf[__pt_i0];
  }
  jacobi7 (nx,ny,nz,A0_comp,timesteps,B,ldb,Anext_comp,ldc);
  

  for (__pt_i0=0; __pt_i0<Anext_size; ++__pt_i0)
  {
    A0_comp_buf[__pt_i0] = Anext[__pt_i0];
  }

  jacobi7_ref (nx,ny,nz,A0,timesteps,B,ldb,Anext,ldc);
  
  {
  int diff_flag = 0;
  for (__pt_i0=0; __pt_i0<Index3D(nx,ny,nx-1,ny-1,nz-1); ++__pt_i0)
  {
    if(Anext_comp_buf[__pt_i0] != Anext_buf[__pt_i0]) {
      diff_flag = 1;
      printf("Position %d (%f) and Position %d (%f) differ by %.15f; original value at the same position is %.15f \n", __pt_i0,
          Anext_comp_buf[__pt_i0], __pt_i0, Anext_buf[__pt_i0],
      	fabs(Anext_comp_buf[__pt_i0]-Anext_buf[__pt_i0]),A0_comp_buf[__pt_i0]);
    }
    /*else {
      printf("Identical output at index %d\n", __pt_i0);
    } */
  }
  if(diff_flag) {
    printf("Output differs\n");
  }else {
    printf("Output is identical\n");
  }
  }
  return(0);
}

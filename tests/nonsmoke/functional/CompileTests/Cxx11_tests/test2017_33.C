// #include "sw4.h"
#define float_sw4 double

//#include <iostream>
using namespace std;
#include <stdio.h>
#include <sched.h>
#include "RAJA/RAJA.hpp"
using namespace RAJA;

// restrict qualifier does not seem to help 
void rhs4sg_nolambda( int ifirst, int ilast, int jfirst, int jlast, int kfirst, int klast,
	     int nk, int* onesided, const float_sw4* __restrict__ a_acof, const float_sw4* __restrict__ a_bope, const float_sw4* __restrict__ a_ghcof,
	     float_sw4* __restrict__ a_lu, const float_sw4* __restrict__ a_u, const float_sw4* __restrict__ a_mu, const float_sw4* __restrict__ a_lambda, 
	     float_sw4 h, const float_sw4* __restrict__ a_strx, const float_sw4* __restrict__ a_stry, const float_sw4* __restrict__ a_strz  )
{

  // This would work to create multi-dimensional C arrays:
  //   float_sw4** b_ar=(float_sw4*)malloc(ni*nj*sizeof(float_sw4*));
  //   for( int j=0;j<nj;j++)
  //      b_ar[j] = &a_lu[j-1+ni*(1-1)];
  //#define ar(i,j) b_ar[j][i];

  // Direct reuse of fortran code by these macro definitions:
#define mu(i,j,k)     a_mu[base+i+ni*(j)+nij*(k)]
#define la(i,j,k) a_lambda[base+i+ni*(j)+nij*(k)]
#define u(c,i,j,k)   a_u[base3+c+3*(i)+nic*(j)+nijc*(k)]   
#define lu(c,i,j,k) a_lu[base3+c+3*(i)+nic*(j)+nijc*(k)]   
#define strx(i) a_strx[i-ifirst0]
#define stry(j) a_stry[j-jfirst0]
#define strz(k) a_strz[k-kfirst0]
#define acof(i,j,k) a_acof[(i-1)+6*(j-1)+48*(k-1)]
#define bope(i,j) a_bope[i-1+6*(j-1)]
#define ghcof(i) a_ghcof[i-1]

  const float_sw4 a1   = 0;
  const float_sw4 i6   = 1.0/6;
  const float_sw4 i12  = 1.0/12;
  const float_sw4 i144 = 1.0/144;
  const float_sw4 tf   = 0.75;

  const int ni    = ilast-ifirst+1;
  const int nij   = ni*(jlast-jfirst+1);
  const int base  = -(ifirst+ni*jfirst+nij*kfirst);
  const int base3 = 3*base-1;
  const int nic  = 3*ni;
  const int nijc = 3*nij;
  const int ifirst0 = ifirst;
  const int jfirst0 = jfirst;
  const int kfirst0 = kfirst;

  int k1, k2; // kb;
  //int i, j, k, qb, mb;


  float_sw4 cof = 1.0/(h*h);


  k1 = kfirst+2;
  if( onesided[4] == 1 )
    k1 = 7;
  k2 = klast-2;
  if( onesided[5] == 1 )
    k2 = nk-6;
  {
    int j,k;
    for( k= k1; k <= k2 ; k++ )
    {
      for( j=jfirst+2; j <= jlast-2 ; j++ )
      {
        RangeSegment iter(ifirst+2,ilast-1);
        int ibegin = *(std::begin(iter));
        int iend = *(std::end(iter));
#pragma simd
#pragma ivdep
        for (size_t i = ibegin ; i < iend; ++i)
        {
          float_sw4 mux1, mux2, mux3, mux4, muy1, muy2, muy3, muy4, muz1, muz2, muz3, muz4;
          float_sw4 r1;
          /* from inner_loop_4a, 28x3 = 84 ops */
          mux1 = mu(i-1,j,k)*strx(i-1)-
            tf*(mu(i,j,k)*strx(i)+mu(i-2,j,k)*strx(i-2));
          mux2 = mu(i-2,j,k)*strx(i-2)+mu(i+1,j,k)*strx(i+1)+
            3*(mu(i,j,k)*strx(i)+mu(i-1,j,k)*strx(i-1));
          mux3 = mu(i-1,j,k)*strx(i-1)+mu(i+2,j,k)*strx(i+2)+
            3*(mu(i+1,j,k)*strx(i+1)+mu(i,j,k)*strx(i));
          mux4 = mu(i+1,j,k)*strx(i+1)-
            tf*(mu(i,j,k)*strx(i)+mu(i+2,j,k)*strx(i+2));

          muy1 = mu(i,j-1,k)*stry(j-1)-
            tf*(mu(i,j,k)*stry(j)+mu(i,j-2,k)*stry(j-2));
          muy2 = mu(i,j-2,k)*stry(j-2)+mu(i,j+1,k)*stry(j+1)+
            3*(mu(i,j,k)*stry(j)+mu(i,j-1,k)*stry(j-1));
          muy3 = mu(i,j-1,k)*stry(j-1)+mu(i,j+2,k)*stry(j+2)+
            3*(mu(i,j+1,k)*stry(j+1)+mu(i,j,k)*stry(j));
          muy4 = mu(i,j+1,k)*stry(j+1)-
            tf*(mu(i,j,k)*stry(j)+mu(i,j+2,k)*stry(j+2));

          muz1 = mu(i,j,k-1)*strz(k-1)-
            tf*(mu(i,j,k)*strz(k)+mu(i,j,k-2)*strz(k-2));
          muz2 = mu(i,j,k-2)*strz(k-2)+mu(i,j,k+1)*strz(k+1)+
            3*(mu(i,j,k)*strz(k)+mu(i,j,k-1)*strz(k-1));
          muz3 = mu(i,j,k-1)*strz(k-1)+mu(i,j,k+2)*strz(k+2)+
            3*(mu(i,j,k+1)*strz(k+1)+mu(i,j,k)*strz(k));
          muz4 = mu(i,j,k+1)*strz(k+1)-
            tf*(mu(i,j,k)*strz(k)+mu(i,j,k+2)*strz(k+2));
          /* xx, yy, and zz derivatives:*/
          /* 75 ops */
          r1 = i6*( strx(i)*( (2*mux1+la(i-1,j,k)*strx(i-1)-
                  tf*(la(i,j,k)*strx(i)+la(i-2,j,k)*strx(i-2)))*
                (u(1,i-2,j,k)-u(1,i,j,k))+
                (2*mux2+la(i-2,j,k)*strx(i-2)+la(i+1,j,k)*strx(i+1)+
                 3*(la(i,j,k)*strx(i)+la(i-1,j,k)*strx(i-1)))*
                (u(1,i-1,j,k)-u(1,i,j,k))+ 
                (2*mux3+la(i-1,j,k)*strx(i-1)+la(i+2,j,k)*strx(i+2)+
                 3*(la(i+1,j,k)*strx(i+1)+la(i,j,k)*strx(i)))*
                (u(1,i+1,j,k)-u(1,i,j,k))+
                (2*mux4+ la(i+1,j,k)*strx(i+1)-
                 tf*(la(i,j,k)*strx(i)+la(i+2,j,k)*strx(i+2)))*
                (u(1,i+2,j,k)-u(1,i,j,k)) ) + stry(j)*(
                  muy1*(u(1,i,j-2,k)-u(1,i,j,k)) + 
                  muy2*(u(1,i,j-1,k)-u(1,i,j,k)) + 
                  muy3*(u(1,i,j+1,k)-u(1,i,j,k)) +
                  muy4*(u(1,i,j+2,k)-u(1,i,j,k)) ) + strz(k)*(
                  muz1*(u(1,i,j,k-2)-u(1,i,j,k)) + 
                  muz2*(u(1,i,j,k-1)-u(1,i,j,k)) + 
                  muz3*(u(1,i,j,k+1)-u(1,i,j,k)) +
                  muz4*(u(1,i,j,k+2)-u(1,i,j,k)) ) );

          /* 9 ops */
          lu(1,i,j,k) = a1*lu(1,i,j,k) + cof*r1;
        }
      }
    }
  }
#undef mu
#undef la
#undef u
#undef lu
#undef strx
#undef stry
#undef strz
}

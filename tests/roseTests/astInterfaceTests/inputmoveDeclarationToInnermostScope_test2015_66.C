
// DQ (1/22/2015): Added ";" to the macro definition.
#define MY_MACRO_1(a,k1) ( a[k1] );

// DQ (1/22/2015): Added ";" to the macro definition.
#define MY_MACRO_2(aa,bb,cc,k1,dd,ee,ff,k2,gg,hh,ii,k3,jj,kk,ll,k4) \
        dx1 = aa[k1] + dd[k2] - gg[k3] - jj[k4] ;  \
        dy1 = bb[k1] + ee[k2] - hh[k3] - kk[k4] ;  \
        va += ax*dx3 + ay*dy3 + az*dz3 ;


void foobar()
   {
     int i, i1, i2, i3, i4 ;
     double *x, *xs, *ys, *zs ;
     double x4c, y4c, z4c ;
     double dx1, dy1, dz1 ;
     double dx2, dy2, dz2 ;
     double dx3, dy3, dz3 ;
     double ax, ay, az, va ;

     for ( i = 0 ; i <= 42 ; i++ ) 
        {
          x4c = 1.0 * MY_MACRO_1(xs,i1) ;

          MY_MACRO_2( xs,ys,zs,i1, xs,ys,zs,i4, xs,ys,zs,i3, xs,ys,zs,i2 ) ;

        }
   }


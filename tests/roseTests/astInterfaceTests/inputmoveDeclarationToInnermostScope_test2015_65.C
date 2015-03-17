
// DQ (1/22/2015): Added ";" to the macro definition.
#define MY_MACRO_1(a,k1,b,k2,c,k3,d,k4,e,k5,f,k6,g,k7,h,k8)     \
        ( a[k1] + b[k2] + c[k3] + d[k4] + e[k5] + f[k6] + g[k7] + h[k8] );

// DQ (1/22/2015): Added ";" to the macro definition.
#define MY_MACRO_2(aa,bb,cc,k1,dd,ee,ff,k2,gg,hh,ii,k3,jj,kk,ll,k4) \
        x1 = aa[k1] + dd[k2] - gg[k3] - jj[k4] ;  \
        y1 = bb[k1] + ee[k2] - hh[k3] - kk[k4] ;  \
        z1 = cc[k1] + ff[k2] - ii[k3] - ll[k4] ;  \
        x1 = aa[k1] + dd[k2] - gg[k3] - jj[k4] ;  \
        y1 = bb[k1] + ee[k2] - hh[k3] - kk[k4] ;  \
        z1 = cc[k1] + ff[k2] - ii[k3] - ll[k4] ;  \
        x1 = aa[k1] + dd[k2] - gg[k3] - jj[k4] ;  \
        y1 = bb[k1] + ee[k2] - hh[k3] - kk[k4] ;  \
        z1 = cc[k1] + ff[k2] - ii[k3] - ll[k4] ;  \
        ax  = y1 + z2 - z1 + y2 ; \
        ay  = z1 + x2 - x1 + z2 ; \
        az  = x1 + y2 - y1 + x2 ; \
        abc += ax + x3 + ay + y3 + az + z3;

void foobar()
   {
     int i, i1, i2, i3, i4 ;
     double *x, *xs, *ys, *zs ;
     double x4;
     double x1, y1, z1 ;
     double x2, y2, z2 ;
     double x3, y3, z3 ;
     double ax, ay, az, abc ;

     for ( i = 0 ; i <= 42 ; i++ ) 
        {
          x4 = 1.0 * MY_MACRO_1(xs,i1,xs,i2,x,i2,x,i1,xs,i4,xs,i3,x,i3,x,i4) ;

          MY_MACRO_2( xs,ys,zs,i1, xs,ys,zs,i4, xs,ys,zs,i3, xs,ys,zs,i2 ) ;

        }
   }


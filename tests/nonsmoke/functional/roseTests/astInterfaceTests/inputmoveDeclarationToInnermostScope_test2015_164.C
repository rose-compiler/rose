#define MACRO_A(a,k1,b,k2,c,k3,d,k4,e,k5,f,k6,g,k7,h,k8)     \
        ( a[k1] + b[k2] + c[k3] + d[k4] + e[k5] + f[k6] + g[k7] + h[k8] ) ;

                                                           /* SUM8 7 flops */

#if 1
#define MACRO_B(aa,bb,cc,k1,dd,ee,ff,k2,gg,hh,ii,k3,jj,kk,ll,k4) \
        dx1 = aa[k1] + dd[k2] - gg[k3] - jj[k4] ;  \
        dy1 = bb[k1] + ee[k2] - hh[k3] - kk[k4] ;  \
        dz1 = cc[k1] + ff[k2] - ii[k3] - ll[k4] ;  \
        dx2 = dd[k2] + gg[k3] - jj[k4] - aa[k1] ;  \
        dy2 = ee[k2] + hh[k3] - kk[k4] - bb[k1] ;  \
        dz2 = ff[k2] + ii[k3] - ll[k4] - cc[k1] ;  \
        dx3 = x4c - (aa[k1] + dd[k2] + gg[k3] + jj[k4]) ; \
        dy3 = y4c - (bb[k1] + ee[k2] + hh[k3] + kk[k4]) ; \
        dz3 = z4c - (cc[k1] + ff[k2] + ii[k3] + ll[k4]) ; \
        ax  = dy1*dz2 - dz1*dy2 ; \
        ay  = dz1*dx2 - dx1*dz2 ; \
        az  = dx1*dy2 - dy1*dx2 ; \
        va += ax*dx3 + ay*dy3 + az*dz3 ;            /* VOLPYRAMID 45 flops */

#else
#define MACRO_B(aa,bb,cc,k1,dd,ee,ff,k2,gg,hh,ii,k3,jj,kk,ll,k4) \
        double dx1 = aa[k1] + dd[k2] - gg[k3] - jj[k4] ;  \
        double dy1 = bb[k1] + ee[k2] - hh[k3] - kk[k4] ;  \
        double dz1 = cc[k1] + ff[k2] - ii[k3] - ll[k4] ;  \
        double dx2 = dd[k2] + gg[k3] - jj[k4] - aa[k1] ;  \
        double dy2 = ee[k2] + hh[k3] - kk[k4] - bb[k1] ;  \
        double dz2 = ff[k2] + ii[k3] - ll[k4] - cc[k1] ;  \
        double dx3 = x4c - (aa[k1] + dd[k2] + gg[k3] + jj[k4]) ; \
        double dy3 = y4c - (bb[k1] + ee[k2] + hh[k3] + kk[k4]) ; \
        double dz3 = z4c - (cc[k1] + ff[k2] + ii[k3] + ll[k4]) ; \
        double ax  = dy1*dz2 - dz1*dy2 ; \
        double ay  = dz1*dx2 - dx1*dz2 ; \
        double az  = dx1*dy2 - dy1*dx2 ; \
        va += ax*dx3 + ay*dy3 + az*dz3 ;            /* VOLPYRAMID 45 flops */

#endif

#define NPNL 2

void foobar( double *dvol, double scale, int ip, int jp, int kp, int imax, int jmax, int kmax )
{
   int i, j, k, ii, jj, i1, i2, i3, i4, off, nsld, offn, offs, offset ;
   int min1, min2, max1, max2 ;
   int *j1, *j2, *j3, *j4, *ndx ;
   double *x, *y, *z, *xs, *ys, *zs ;
   double x4c, y4c, z4c ;
   double dx1, dy1, dz1 ;
   double dx2, dy2, dz2 ;
   double dx3, dy3, dz3 ;
   double ax, ay, az, va, sign, dv, volold, volnew ;
   const double vnormc = 0.0208333333333333 ;             /* comment */
   double scalec ;

   /* comment comment */

   x  = 0L;
   y  = 0L;
   z  = 0L;
   xs = 0L;
   ys = 0L; // comment 1
   zs = 0L; // comment 2

   double *vol = 0L;

   /* comment comment */

   // COMMENT COMMENT
   for ( k = NPNL ; k < kmax ; k++ ) {

      for ( j = NPNL ; j < jmax ; j++ ) {

         off = j * jp + k * kp ;

         for ( i = NPNL ; i <= imax ; i++ ) {

            i1 = i * ip + off ;
            i2 = i1 + jp ;
            i3 = i1 + jp + kp ;
            i4 = i1 + kp ;

            /* comment comment comment A */

            va = 0.0 ;

            /* comment comment comment B */

            x4c = 0.5 * MACRO_A(xs,i1,xs,i2,x,i2,x,i1,xs,i4,xs,i3,x,i3,x,i4) ;
         // y4c = 0.5 * MACRO_A(ys,i1,ys,i2,y,i2,y,i1,ys,i4,ys,i3,y,i3,y,i4) ;
         // z4c = 0.5 * MACRO_A(zs,i1,zs,i2,z,i2,z,i1,zs,i4,zs,i3,z,i3,z,i4) ;

                                                               /* comment 42 */

            /* comment comment comment C */

            MACRO_B( xs,ys,zs,i1, xs,ys,zs,i4, xs,ys,zs,i3, xs,ys,zs,i2 ) ;
         // MACRO_B( x, y, z, i2, x, y, z, i3, x, y, z, i4, x, y, z, i1 ) ;
         }

      }

   }

}



!-------------------------------------------------------------------------!
!                                                                         !
!        N  A  S     P A R A L L E L     B E N C H M A R K S  2.3         !
!                                                                         !
!                     S E R I A L     V E R S I O N S                     !
!                                                                         !
!                                   C G                                   !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    This benchmark is a serial version of the NPB CG code.               !
!                                                                         !
!    Permission to use, copy, distribute and modify this software         !
!    for any purpose with or without fee is hereby granted.  We           !
!    request, however, that all derived work reference the NAS            !
!    Parallel Benchmarks 2.3. This software is provided "as is"           !
!    without express or implied warranty.                                 !
!                                                                         !
!    Information on NPB 2.3, including the technical report, the          !
!    original specifications, source code, results and information        !
!    on how to submit new results, is available at:                       !
!                                                                         !
!           http://www.nas.nasa.gov/NAS/NPB/                              !
!                                                                         !
!    Send comments or suggestions to  npb@nas.nasa.gov                    !
!    Send bug reports to              npb-bugs@nas.nasa.gov               !
!                                                                         !
!          NAS Parallel Benchmarks Group                                  !
!          NASA Ames Research Center                                      !
!          Mail Stop: T27A-1                                              !
!          Moffett Field, CA   94035-1000                                 !
!                                                                         !
!          E-mail:  npb@nas.nasa.gov                                      !
!          Fax:     (415) 604-3957                                        !
!                                                                         !
!-------------------------------------------------------------------------!


c---------------------------------------------------------------------
c      NPB CG serial version      
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c
c Authors: M. Yarrow
c          C. Kuszmaul
c
c---------------------------------------------------------------------


c---------------------------------------------------------------------
c---------------------------------------------------------------------
      program cg
c---------------------------------------------------------------------
c---------------------------------------------------------------------


      implicit none

      include 'npbparams.h'

c---------------------------------------------------------------------
c  Note: please observe that in the routine conj_grad three 
c  implementations of the sparse matrix-vector multiply have
c  been supplied.  The default matrix-vector multiply is not
c  loop unrolled.  The alternate implementations are unrolled
c  to a depth of 2 and unrolled to a depth of 8.  Please
c  experiment with these to find the fastest for your particular
c  architecture.  If reporting timing results, any of these three may
c  be used without penalty.
c---------------------------------------------------------------------


c---------------------------------------------------------------------
c  Class specific parameters: 
c  It appears here for reference only.
c  These are their values, however, this info is imported in the npbparams.h
c  include file, which is written by the sys/setparams.c program.
c---------------------------------------------------------------------

C----------
C  Class S:
C----------
CC       parameter( na=1400, 
CC      >           nonzer=7, 
CC      >           shift=10., 
CC      >           niter=15,
CC      >           rcond=1.0d-1 )
C----------
C  Class W:
C----------
CC       parameter( na=7000,
CC      >           nonzer=8, 
CC      >           shift=12., 
CC      >           niter=15,
CC      >           rcond=1.0d-1 )
C----------
C  Class A:
C----------
CC       parameter( na=14000,
CC      >           nonzer=11, 
CC      >           shift=20., 
CC      >           niter=15,
CC      >           rcond=1.0d-1 )
C----------
C  Class B:
C----------
CC       parameter( na=75000, 
CC      >           nonzer=13, 
CC      >           shift=60., 
CC      >           niter=75,
CC      >           rcond=1.0d-1 )
C----------
C  Class C:
C----------
CC       parameter( na=150000, 
CC      >           nonzer=15, 
CC      >           shift=110., 
CC      >           niter=75,
CC      >           rcond=1.0d-1 )



      integer    nz
      parameter( nz = na*(nonzer+1)*(nonzer+1)
     >              + na*(nonzer+2) )


			      	
      common / partit_size  / 	 naa, nzz, 
     >                        	 firstrow, 
     >                           lastrow, 
     >                           firstcol, 
     >                           lastcol
      integer                 	 naa, nzz, 
     >                        	 firstrow, 
     >                           lastrow, 
     >                           firstcol, 
     >                           lastcol
			      	
			      	
      common / main_int_mem / 	 colidx,     rowstr,
     >                        	 iv,         arow,     acol
      integer                 	 colidx(nz), rowstr(na+1),
     >                        	 iv(2*na+1), arow(nz), acol(nz)
			      	
			      	
      common / main_flt_mem / 	 v,       aelt,     a,
     >                        	 x,
     >                        	 z,
     >                           p,
     >                           q,
     >                           r,
     >                           w
      double precision           v(na+1), aelt(nz), a(nz),
     >                        	 x(na+2),
     >                        	 z(na+2),
     >                        	 p(na+2),
     >                        	 q(na+2),
     >                        	 r(na+2),
     >                        	 w(na+2)
			      	
			      	
      common /urando/         	 amult, tran
      double precision           amult, tran




      integer            i, j, k, it

      double precision   zeta, randlc
      external           randlc
      double precision   rnorm
      double precision   norm_temp1(2)

      double precision   t, mflops
      external           timer_read
      double precision   timer_read
      character          class
      logical            verified
      double precision   zeta_verify_value, epsilon


      firstrow = 1
      lastrow  = na
      firstcol = 1
      lastcol  = na


      if( na .eq. 1400 .and. 
     &    nonzer .eq. 7 .and. 
     &    niter .eq. 15 .and.
     &    shift .eq. 10. ) then
         class = 'S'
         zeta_verify_value = 8.5971775078648d0
      else if( na .eq. 7000 .and. 
     &         nonzer .eq. 8 .and. 
     &         niter .eq. 15 .and.
     &         shift .eq. 12. ) then
         class = 'W'
         zeta_verify_value = 10.362595087124d0
      else if( na .eq. 14000 .and. 
     &         nonzer .eq. 11 .and. 
     &         niter .eq. 15 .and.
     &         shift .eq. 20. ) then
         class = 'A'
         zeta_verify_value = 17.130235054029d0
      else if( na .eq. 75000 .and. 
     &         nonzer .eq. 13 .and. 
     &         niter .eq. 75 .and.
     &         shift .eq. 60. ) then
         class = 'B'
         zeta_verify_value = 22.712745482631d0
      else if( na .eq. 150000 .and. 
     &         nonzer .eq. 15 .and. 
     &         niter .eq. 75 .and.
     &         shift .eq. 110. ) then
         class = 'C'
         zeta_verify_value = 28.973605592845d0
      else
         class = 'U'
      endif

      write( *,1000 ) 
      write( *,1001 ) na
      write( *,1002 ) niter
 1000 format(//,' NAS Parallel Benchmarks 2.3-serial version',
     >          ' - CG Benchmark', /)
 1001 format(' Size: ', i10 )
 1002 format(' Iterations: ', i5 )



      naa = na
      nzz = nz


c---------------------------------------------------------------------
c  Inialize random number generator
c---------------------------------------------------------------------
      tran    = 314159265.0D0
      amult   = 1220703125.0D0
      zeta    = randlc( tran, amult )

c---------------------------------------------------------------------
c  
c---------------------------------------------------------------------
      call makea(naa, nzz, a, colidx, rowstr, nonzer,
     >           firstrow, lastrow, firstcol, lastcol, 
     >           rcond, arow, acol, aelt, v, iv, shift)



c---------------------------------------------------------------------
c  Note: as a result of the above call to makea:
c        values of j used in indexing rowstr go from 1 --> lastrow-firstrow+1
c        values of colidx which are col indexes go from firstcol --> lastcol
c        So:
c        Shift the col index vals from actual (firstcol --> lastcol ) 
c        to local, i.e., (1 --> lastcol-firstcol+1)
c---------------------------------------------------------------------
      do j=1,lastrow-firstrow+1
         do k=rowstr(j),rowstr(j+1)-1
            colidx(k) = colidx(k) - firstcol + 1
         enddo
      enddo

c---------------------------------------------------------------------
c  set starting vector to (1, 1, .... 1)
c---------------------------------------------------------------------
      do i = 1, na+1
         x(i) = 1.0D0
      enddo

      zeta  = 0.0d0

c---------------------------------------------------------------------
c---->
c  Do one iteration untimed to init all code and data page tables
c---->                    (then reinit, start timing, to niter its)
c---------------------------------------------------------------------
      do it = 1, 1

c---------------------------------------------------------------------
c  The call to the conjugate gradient routine:
c---------------------------------------------------------------------
         call conj_grad ( colidx,
     >                    rowstr,
     >                    x,
     >                    z,
     >                    a,
     >                    p,
     >                    q,
     >                    r,
     >                    w,
     >                    rnorm )

c---------------------------------------------------------------------
c  zeta = shift + 1/(x.z)
c  So, first: (x.z)
c  Also, find norm of z
c  So, first: (z.z)
c---------------------------------------------------------------------
         norm_temp1(1) = 0.0d0
         norm_temp1(2) = 0.0d0
         do j=1, lastcol-firstcol+1
            norm_temp1(1) = norm_temp1(1) + x(j)*z(j)
            norm_temp1(2) = norm_temp1(2) + z(j)*z(j)
         enddo

         norm_temp1(2) = 1.0d0 / sqrt( norm_temp1(2) )


c---------------------------------------------------------------------
c  Normalize z to obtain x
c---------------------------------------------------------------------
         do j=1, lastcol-firstcol+1      
            x(j) = norm_temp1(2)*z(j)    
         enddo                           


      enddo                              ! end of do one iteration untimed


c---------------------------------------------------------------------
c  set starting vector to (1, 1, .... 1)
c---------------------------------------------------------------------
c
c  
c
      do i = 1, na+1
         x(i) = 1.0D0
      enddo

      zeta  = 0.0d0


      call timer_clear( 1 )
      call timer_start( 1 )

c---------------------------------------------------------------------
c---->
c  Main Iteration for inverse power method
c---->
c---------------------------------------------------------------------
      do it = 1, niter

c---------------------------------------------------------------------
c  The call to the conjugate gradient routine:
c---------------------------------------------------------------------
         call conj_grad ( colidx,
     >                    rowstr,
     >                    x,
     >                    z,
     >                    a,
     >                    p,
     >                    q,
     >                    r,
     >                    w,
     >                    rnorm )


c---------------------------------------------------------------------
c  zeta = shift + 1/(x.z)
c  So, first: (x.z)
c  Also, find norm of z
c  So, first: (z.z)
c---------------------------------------------------------------------
         norm_temp1(1) = 0.0d0
         norm_temp1(2) = 0.0d0
         do j=1, lastcol-firstcol+1
            norm_temp1(1) = norm_temp1(1) + x(j)*z(j)
            norm_temp1(2) = norm_temp1(2) + z(j)*z(j)
         enddo


         norm_temp1(2) = 1.0d0 / sqrt( norm_temp1(2) )


         zeta = shift + 1.0d0 / norm_temp1(1)
         if( it .eq. 1 ) write( *,9000 )
         write( *,9001 ) it, rnorm, zeta

 9000    format( /,'   iteration           ||r||                 zeta' )
 9001    format( 4x, i5, 7x, e20.14, f20.13 )

c---------------------------------------------------------------------
c  Normalize z to obtain x
c---------------------------------------------------------------------
         do j=1, lastcol-firstcol+1      
            x(j) = norm_temp1(2)*z(j)    
         enddo                           


      enddo                              ! end of main iter inv pow meth

      call timer_stop( 1 )

c---------------------------------------------------------------------
c  End of timed section
c---------------------------------------------------------------------

      t = timer_read( 1 )


      write(*,100)
 100  format(' Benchmark completed ')

      epsilon = 1.d-10
      if (class .ne. 'U') then

         if( abs( zeta - zeta_verify_value ) .le. epsilon ) then
            verified = .TRUE.
            write(*, 200)
            write(*, 201) zeta
            write(*, 202) zeta - zeta_verify_value
 200        format(' VERIFICATION SUCCESSFUL ')
 201        format(' Zeta is    ', E20.12)
 202        format(' Error is   ', E20.12)
         else
            verified = .FALSE.
            write(*, 300) 
            write(*, 301) zeta
            write(*, 302) zeta_verify_value
 300        format(' VERIFICATION FAILED')
 301        format(' Zeta                ', E20.12)
 302        format(' The correct zeta is ', E20.12)
         endif
      else
         verified = .FALSE.
         write (*, 400)
         write (*, 401)
 400     format(' Problem size unknown')
 401     format(' NO VERIFICATION PERFORMED')
      endif


      if( t .ne. 0. ) then
         mflops = float( 2*niter*na )
     &               * ( 3.+float( nonzer*(nonzer+1) )
     &                 + 25.*(5.+float( nonzer*(nonzer+1) ))
     &                 + 3. ) / t / 1000000.0
      else
         mflops = 0.0
      endif


         call print_results('CG', class, na, 0, 0,
     >                      niter, t,
     >                      mflops, '          floating point', 
     >                      verified, npbversion, compiletime,
     >                      cs1, cs2, cs3, cs4, cs5, cs6, cs7)



 600  format( i4, 2e19.12)




      end                              ! end main



c---------------------------------------------------------------------
c---------------------------------------------------------------------
      subroutine conj_grad ( colidx,
     >                       rowstr,
     >                       x,
     >                       z,
     >                       a,
     >                       p,
     >                       q,
     >                       r,
     >                       w,
     >                       rnorm )
c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c  Floaging point arrays here are named as in NPB1 spec discussion of 
c  CG algorithm
c---------------------------------------------------------------------
 
      implicit none


			      	
      common / partit_size  / 	 naa, nzz, 
     >                        	 firstrow, 
     >                           lastrow, 
     >                           firstcol, 
     >                           lastcol
      integer                 	 naa, nzz, 
     >                        	 firstrow, 
     >                           lastrow, 
     >                           firstcol, 
     >                           lastcol
			      	


      double precision   x(*),
     >                   z(*),
     >                   a(nzz)
      integer            colidx(nzz), rowstr(naa+1)

      double precision   p(*),
     >                   q(*),
     >                   r(*),               
     >                   w(*)                ! used as work temporary


      integer   i, j, k
      integer   cgit, cgitmax
      data      cgitmax / 25 /

      double precision   d, sum, rho, rho0, alpha, beta, rnorm

      external         timer_read
      double precision timer_read



c---------------------------------------------------------------------
c  Initialize the CG algorithm:
c---------------------------------------------------------------------
      do j=1,naa+1
         q(j) = 0.0d0
         z(j) = 0.0d0
         r(j) = x(j)
         p(j) = r(j)
         w(j) = 0.0d0                 
      enddo


c---------------------------------------------------------------------
c  rho = r.r
c  Now, obtain the norm of r: First, sum squares of r elements locally...
c---------------------------------------------------------------------
      sum = 0.0d0
      do j=1, lastcol-firstcol+1
         sum = sum + r(j)*r(j)
      enddo

      rho = sum



c---------------------------------------------------------------------
c---->
c  The conj grad iteration loop
c---->
c---------------------------------------------------------------------
      do cgit = 1, cgitmax


c---------------------------------------------------------------------
c  q = A.p
c  The partition submatrix-vector multiply: use workspace w
c---------------------------------------------------------------------
C
C  NOTE: this version of the multiply is actually (slightly: maybe %5) 
C        faster on the sp2 on 16 nodes than is the unrolled-by-2 version 
C        below.   On the Cray t3d, the reverse is true, i.e., the 
C        unrolled-by-two version is some 10% faster.  
C        The unrolled-by-8 version below is significantly faster
C        on the Cray t3d - overall speed of code is 1.5 times faster.
C
         do j=1,lastrow-firstrow+1
            sum = 0.d0
            do k=rowstr(j),rowstr(j+1)-1
               sum = sum + a(k)*p(colidx(k))
            enddo
            w(j) = sum
         enddo

CC          do j=1,lastrow-firstrow+1
CC             i = rowstr(j) 
CC             iresidue = mod( rowstr(j+1)-i, 2 )
CC             sum1 = 0.d0
CC             sum2 = 0.d0
CC             if( iresidue .eq. 1 )
CC      &          sum1 = sum1 + a(i)*p(colidx(i))
CC             do k=i+iresidue, rowstr(j+1)-2, 2
CC                sum1 = sum1 + a(k)  *p(colidx(k))
CC                sum2 = sum2 + a(k+1)*p(colidx(k+1))
CC             enddo
CC             w(j) = sum1 + sum2
CC          enddo

CC          do j=1,lastrow-firstrow+1
CC             i = rowstr(j) 
CC             iresidue = mod( rowstr(j+1)-i, 8 )
CC             sum = 0.d0
CC             do k=i,i+iresidue-1
CC                sum = sum +  a(k)*p(colidx(k))
CC             enddo
CC             do k=i+iresidue, rowstr(j+1)-8, 8
CC                sum = sum + a(k  )*p(colidx(k  ))
CC      &                   + a(k+1)*p(colidx(k+1))
CC      &                   + a(k+2)*p(colidx(k+2))
CC      &                   + a(k+3)*p(colidx(k+3))
CC      &                   + a(k+4)*p(colidx(k+4))
CC      &                   + a(k+5)*p(colidx(k+5))
CC      &                   + a(k+6)*p(colidx(k+6))
CC      &                   + a(k+7)*p(colidx(k+7))
CC             enddo
CC             w(j) = sum
CC          enddo
            


         do j=1,lastcol-firstcol+1
            q(j) = w(j)
         enddo


c---------------------------------------------------------------------
c  Clear w for reuse...
c---------------------------------------------------------------------
         do j=1, lastcol-firstcol+1
            w(j) = 0.0d0
         enddo
         

c---------------------------------------------------------------------
c  Obtain p.q
c---------------------------------------------------------------------
         sum = 0.0d0
         do j=1, lastcol-firstcol+1
            sum = sum + p(j)*q(j)
         enddo

         d = sum


c---------------------------------------------------------------------
c  Obtain alpha = rho / (p.q)
c---------------------------------------------------------------------
         alpha = rho / d

c---------------------------------------------------------------------
c  Save a temporary of rho
c---------------------------------------------------------------------
         rho0 = rho

c---------------------------------------------------------------------
c  Obtain z = z + alpha*p
c  and    r = r - alpha*q
c---------------------------------------------------------------------
         do j=1, lastcol-firstcol+1
            z(j) = z(j) + alpha*p(j)
            r(j) = r(j) - alpha*q(j)
         enddo
            
c---------------------------------------------------------------------
c  rho = r.r
c  Now, obtain the norm of r: First, sum squares of r elements locally...
c---------------------------------------------------------------------
         sum = 0.0d0
         do j=1, lastcol-firstcol+1
            sum = sum + r(j)*r(j)
         enddo

         rho = sum

c---------------------------------------------------------------------
c  Obtain beta:
c---------------------------------------------------------------------
         beta = rho / rho0

c---------------------------------------------------------------------
c  p = r + beta*p
c---------------------------------------------------------------------
         do j=1, lastcol-firstcol+1
            p(j) = r(j) + beta*p(j)
         enddo



      enddo                             ! end of do cgit=1,cgitmax


c---------------------------------------------------------------------
c  Compute residual norm explicitly:  ||r|| = ||x - A.z||
c  First, form A.z
c  The partition submatrix-vector multiply
c---------------------------------------------------------------------
      do j=1,lastrow-firstrow+1
         sum = 0.d0
         do k=rowstr(j),rowstr(j+1)-1
            sum = sum + a(k)*z(colidx(k))
         enddo
         w(j) = sum
      enddo



      do j=1,lastcol-firstcol+1
         r(j) = w(j)
      enddo


c---------------------------------------------------------------------
c  At this point, r contains A.z
c---------------------------------------------------------------------
         sum = 0.0d0
         do j=1, lastcol-firstcol+1
            d   = x(j) - r(j)         
            sum = sum + d*d
         enddo
         
      d = sum


      rnorm = sqrt( d )



      return
      end                               ! end of routine conj_grad



c---------------------------------------------------------------------
c---------------------------------------------------------------------
      subroutine makea( n, nz, a, colidx, rowstr, nonzer,
     >                  firstrow, lastrow, firstcol, lastcol,
     >                  rcond, arow, acol, aelt, v, iv, shift )
c---------------------------------------------------------------------
c---------------------------------------------------------------------

      integer             n, nz
      integer             firstrow, lastrow, firstcol, lastcol
      integer             colidx(nz), rowstr(n+1)
      integer             iv(2*n+1), arow(nz), acol(nz)
      double precision    v(n+1), aelt(nz)
      double precision    rcond, a(nz), shift

c---------------------------------------------------------------------
c       generate the test problem for benchmark 6
c       makea generates a sparse matrix with a
c       prescribed sparsity distribution
c
c       parameter    type        usage
c
c       input
c
c       n            i           number of cols/rows of matrix
c       nz           i           nonzeros as declared array size
c       rcond        r*8         condition number
c       shift        r*8         main diagonal shift
c
c       output
c
c       a            r*8         array for nonzeros
c       colidx       i           col indices
c       rowstr       i           row pointers
c
c       workspace
c
c       iv, arow, acol i
c       v, aelt        r*8
c---------------------------------------------------------------------

      integer i, nnza, iouter, ivelt, ivelt1, irow, nzv, NONZER

c---------------------------------------------------------------------
c      nonzer is approximately  (int(sqrt(nnza /n)));
c---------------------------------------------------------------------

      double precision  size, ratio, scale
      external          sparse, sprnvc, vecset

      size = 1.0D0
      ratio = rcond ** (1.0D0 / dfloat(n))
      nnza = 0

c---------------------------------------------------------------------
c  Initialize colidx(n+1 .. 2n) to zero.
c  Used by sprnvc to mark nonzero positions
c---------------------------------------------------------------------

      do i = 1, n
	   colidx(n+i) = 0
      enddo
      do iouter = 1, n
         nzv = nonzer
         call sprnvc( n, nzv, v, iv, colidx(1), colidx(n+1) )
         call vecset( n, v, iv, nzv, iouter, .5D0 )
         do ivelt = 1, nzv
              jcol = iv(ivelt)
              if (jcol.ge.firstcol .and. jcol.le.lastcol) then
	         scale = size * v(ivelt)
                 do ivelt1 = 1, nzv
	            irow = iv(ivelt1)
                    if (irow.ge.firstrow .and. irow.le.lastrow) then
	               nnza = nnza + 1
                       if (nnza .gt. nz) goto 9999
	               acol(nnza) = jcol
	               arow(nnza) = irow
	               aelt(nnza) = v(ivelt1) * scale
                    endif
                 enddo
              endif
         enddo
         size = size * ratio
      enddo


c---------------------------------------------------------------------
c       ... add the identity * rcond to the generated matrix to bound
c           the smallest eigenvalue from below by rcond
c---------------------------------------------------------------------
	do i = firstrow, lastrow
           if (i.ge.firstcol .and. i.le.lastcol) then
              iouter = n + i
	      nnza = nnza + 1
              if (nnza .gt. nz) goto 9999
	      acol(nnza) = i
	      arow(nnza) = i
	      aelt(nnza) = rcond - shift
	   endif
        enddo


c---------------------------------------------------------------------
c       ... make the sparse matrix from list of elements with duplicates
c           (v and iv are used as  workspace)
c---------------------------------------------------------------------
      call sparse( a, colidx, rowstr, n, arow, acol, aelt,
     >             firstrow, lastrow,
     >             v, iv(1), iv(n+1), nnza )
      return

 9999 continue
      write(*,*) 'Space for matrix elements exceeded in makea'
      write(*,*) 'nnza, nzmax = ',nnza, nz
      write(*,*) ' iouter = ',iouter

      stop
      end
c-------end   of makea------------------------------

c---------------------------------------------------------------------
c---------------------------------------------------------------------
      subroutine sparse( a, colidx, rowstr, n, arow, acol, aelt,
     >                   firstrow, lastrow,
     >                   x, mark, nzloc, nnza )
c---------------------------------------------------------------------
c---------------------------------------------------------------------

      implicit           logical (a-z)
      integer            colidx(1), rowstr(1)
      integer            firstrow, lastrow
      integer            n, arow(1), acol(1), nnza
      double precision   a(1), aelt(1)

c---------------------------------------------------------------------
c       rows range from firstrow to lastrow
c       the rowstr pointers are defined for nrows = lastrow-firstrow+1 values
c---------------------------------------------------------------------
      integer            nzloc(n), nrows
      double precision   x(n)
      logical            mark(n)

c---------------------------------------------------
c       generate a sparse matrix from a list of
c       [col, row, element] tri
c---------------------------------------------------

      integer            i, j, jajp1, nza, k, nzrow
      double precision   xi

c---------------------------------------------------------------------
c    how many rows of result
c---------------------------------------------------------------------
	nrows = lastrow - firstrow + 1

c---------------------------------------------------------------------
c     ...count the number of triples in each row
c---------------------------------------------------------------------
      do j = 1, n
         rowstr(j) = 0
         mark(j) = .false.
      enddo
      rowstr(n+1) = 0

      do nza = 1, nnza
         j = (arow(nza) - firstrow + 1) + 1
         rowstr(j) = rowstr(j) + 1
      enddo

      rowstr(1) = 1
      do j = 2, nrows+1
         rowstr(j) = rowstr(j) + rowstr(j-1)
      enddo


c---------------------------------------------------------------------
c     ... rowstr(j) now is the location of the first nonzero
c           of row j of a
c---------------------------------------------------------------------


c---------------------------------------------------------------------
c     ... do a bucket sort of the triples on the row index
c---------------------------------------------------------------------
      do nza = 1, nnza
         j = arow(nza) - firstrow + 1
         k = rowstr(j)
         a(k) = aelt(nza)
         colidx(k) = acol(nza)
         rowstr(j) = rowstr(j) + 1
      enddo


c---------------------------------------------------------------------
c       ... rowstr(j) now points to the first element of row j+1
c---------------------------------------------------------------------
      do j = nrows, 1, -1
          rowstr(j+1) = rowstr(j)
      enddo
      rowstr(1) = 1


c---------------------------------------------------------------------
c       ... generate the actual output rows by adding elements
c---------------------------------------------------------------------
      nza = 0
      do i = 1, n
          x(i)    = 0.0
          mark(i) = .false.
      enddo

      jajp1 = rowstr(1)
      do j = 1, nrows
         nzrow = 0

c---------------------------------------------------------------------
c          ...loop over the jth row of a
c---------------------------------------------------------------------
         do k = jajp1 , rowstr(j+1)-1
            i = colidx(k)
            x(i) = x(i) + a(k)
            if ( (.not. mark(i)) .and. (x(i) .ne. 0.D0)) then
             mark(i) = .true.
             nzrow = nzrow + 1
             nzloc(nzrow) = i
            endif
         enddo

c---------------------------------------------------------------------
c          ... extract the nonzeros of this row
c---------------------------------------------------------------------
         do k = 1, nzrow
            i = nzloc(k)
            mark(i) = .false.
            xi = x(i)
            x(i) = 0.D0
            if (xi .ne. 0.D0) then
             nza = nza + 1
             a(nza) = xi
             colidx(nza) = i
            endif
         enddo
         jajp1 = rowstr(j+1)
         rowstr(j+1) = nza + rowstr(1)
      enddo
CC       write (*, 11000) nza
      return
11000   format ( //,'final nonzero count in sparse ',
     1            /,'number of nonzeros       = ', i16 )
      end
c-------end   of sparse-----------------------------


c---------------------------------------------------------------------
c---------------------------------------------------------------------
      subroutine sprnvc( n, nz, v, iv, nzloc, mark )
c---------------------------------------------------------------------
c---------------------------------------------------------------------

      implicit           logical (a-z)
      double precision   v(1)
      integer            n, nz, iv(1), nzloc(n), nn1
      integer mark(n)
      common /urando/    amult, tran
      double precision   amult, tran


c---------------------------------------------------------------------
c       generate a sparse n-vector (v, iv)
c       having nzv nonzeros
c
c       mark(i) is set to 1 if position i is nonzero.
c       mark is all zero on entry and is reset to all zero before exit
c       this corrects a performance bug found by John G. Lewis, caused by
c       reinitialization of mark on every one of the n calls to sprnvc
c---------------------------------------------------------------------

        integer            nzrow, nzv, ii, i, icnvrt

        external           randlc, icnvrt
        double precision   randlc, vecelt, vecloc


        nzv = 0
        nzrow = 0
        nn1 = 1
 50     continue
          nn1 = 2 * nn1
          if (nn1 .lt. n) goto 50

c---------------------------------------------------------------------
c    nn1 is the smallest power of two not less than n
c---------------------------------------------------------------------

100     continue
        if (nzv .ge. nz) goto 110
         vecelt = randlc( tran, amult )

c---------------------------------------------------------------------
c   generate an integer between 1 and n in a portable manner
c---------------------------------------------------------------------
         vecloc = randlc(tran, amult)
         i = icnvrt(vecloc, nn1) + 1
         if (i .gt. n) goto 100

c---------------------------------------------------------------------
c  was this integer generated already?
c---------------------------------------------------------------------
         if (mark(i) .eq. 0) then
            mark(i) = 1
            nzrow = nzrow + 1
            nzloc(nzrow) = i
            nzv = nzv + 1
            v(nzv) = vecelt
            iv(nzv) = i
         endif
         goto 100
110      continue
      do ii = 1, nzrow
         i = nzloc(ii)
         mark(i) = 0
      enddo
      return
      end
c-------end   of sprnvc-----------------------------


c---------------------------------------------------------------------
c---------------------------------------------------------------------
      function icnvrt(x, ipwr2)
c---------------------------------------------------------------------
c---------------------------------------------------------------------

      implicit           logical (a-z)
      double precision   x
      integer            ipwr2, icnvrt

c---------------------------------------------------------------------
c    scale a double precision number x in (0,1) by a power of 2 and chop it
c---------------------------------------------------------------------
      icnvrt = int(ipwr2 * x)

      return
      end
c-------end   of icnvrt-----------------------------


c---------------------------------------------------------------------
c---------------------------------------------------------------------
      subroutine vecset(n, v, iv, nzv, i, val)
c---------------------------------------------------------------------
c---------------------------------------------------------------------

      implicit           logical (a-z)
      integer            n, iv(1), nzv, i, k
      double precision   v(1), val

c---------------------------------------------------------------------
c       set ith element of sparse vector (v, iv) with
c       nzv nonzeros to val
c---------------------------------------------------------------------

      logical set

      set = .false.
      do k = 1, nzv
         if (iv(k) .eq. i) then
            v(k) = val
            set  = .true.
         endif
      enddo
      if (.not. set) then
         nzv     = nzv + 1
         v(nzv)  = val
         iv(nzv) = i
      endif
      return
      end
c-------end   of vecset-----------------------------




!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

       program eqstat

!***********************************************************************
!
!     calculate coefficients for POP polynomial equation of state option
!
!     THIS WAS TAKEN FROM THE GFDL MOM MODEL and extensively modified
!       to improve readability (February (1999)
!
!       This program calculates the 9 coefficients of a third order
!     polynomial approximation to the equation of state for sea water.
!     The coefficients are calculated by first sampling a range of
!     temperature and salinity at each depth.  The density is then
!     computed at each of the sampled points using a full equation
!     of state.  A least squares method is used to fit a polynomial
!     (cubic in temp and salinity) to the sampled points.  More 
!     specifically, the densities calculated from the polynomial 
!     formula are in the form of sigma anomalies.  The method is
!     taken from that described by Bryan & Cox (1972).  
!       By default, the program uses the equation of state set by the
!     Joint Panel on Oceanographic Tables & Standards (UNESCO, 1981)
!     an described by Gill (1982).
!       Subroutine lsqsl2 performs the iterative least-squares
!     polynomial fitting for the overdetermined system.  The algorithm
!     is outlined by Hanson and Lawson (1969), and the code is a
!     slightly modified (to get rid of old Fortran IV stuff) version
!     of the original routine.
!
!     ACCURACY IMPROVEMENTS (March 1999)
!
!       There are two improvements: (a)  An more accurate depth-
!     to-pressure conversion based on the Levitus_94 climatology,
!     and (b) Use of a more recent formula for the computation of 
!     potential temperature (Bryden, 1973) in place of an older
!     algorithm (Fofonoff, 1962).
!
!     references:
!        Bryan, K. & M. Cox, An approximate equation of state
!          for numerical models of ocean circulation, J. Phys.
!          Oceanogr., 2, 510-514, 1972.
!        Bryden, H.L., New polynomials for thermal expansion, adiabatic
!          temperature gradient and potential temperature of sea water,
!          Deap-Sea Res., 20, 401-408, 1973.
!        Fofonoff, N., The Sea: Vol 1, (ed. M. Hill). Interscience,
!          New York, 1962, pp 3-30.
!        Gill, A., Atmosphere-Ocean Dynamics: International Geophysical
!          Series No. 30. Academic Press, London, 1982, pp 599-600.
!        Hanson, R., & C. Lawson, Extensions and applications of the
!          Householder algorithm for solving linear least squares
!          problems. Math. Comput., 23, 787-812, 1969.
!        UNESCO, 10th report of the joint panel on oceanographic tables
!          and standards. UNESCO Tech. Papers in Marine Sci. No. 36,
!          Paris, 1981.
!
!    inputs:
!      The user needs only to specify the number of model levels km
!    and the model layer thicknesses [cm] dz(1..km).
!
!-----------------------------------------------------------------------

      implicit none

      integer, parameter :: km=20

      double precision, parameter ::
     &  c0 = 0.0,
     &  c1 = 1.0,
     &  c2 = 2.0,
     &  p5 = 0.5

      double precision, dimension(km) ::
     &  dzt             ! model layer thicknesses (in cm)

      integer, parameter ::
     &  nsample_salt = 5,              ! number of pts to sample in S 
     &  nsample_temp = 2*nsample_salt, ! number of pts in sample in T
     &  nsample_all  = nsample_salt*nsample_temp, ! total sample points
     &  itmax = 4,                     ! max iters for least squares
     &  sbdim = nsample_all+72,        ! array dims for work arrays 
     &  rdim = nsample_all+36          !  in lsq routine

      double precision, parameter ::
     &   mpercm = 1.0d-2    ! meters per cm

      integer ::
     &  i,j,k,n,        ! dummy loop counters
     &  nsample,        ! sample number
     &  irank,          ! used by lsqsl2 routine
     &  in,             ! used by lsqsl2 routine
     &  it,             ! used by lsqsl2 routine
     &  ieq,            ! used by lsqsl2 routine
     &  nlines,         ! number of data lines in km output
     &  nwords_last_line ! num data words in last line of km output

      double precision
     &  dtemp, dsalt,   ! T,S increments for curve fit
     &  temp_sample,    ! sample temperature in temp range
     &  salt_sample,    ! sample salinity    in salt range
     &  density,        ! density in MKS units
     &  tanom, sanom,   ! T,S anomalies (from level average)
     &  enorm           ! norm of least squares residual

      double precision, dimension(km) ::
     &  zmeters,        ! model depth in meters at layer midpoints
     &  tmin, tmax,     ! temperature range for each vertical level
     &  smin, smax,     ! salinity    range for each vertical level
     &  avg_temp,       ! average of sample temps      for level k
     &  avg_salt,       ! average of sample salinity   for level k
     &  avg_theta,      ! average of sample pot. temps for level k
     &  avg_sigma       ! average of sample densities  for level k

      double precision, dimension(nsample_all) ::
     &  tsamp,          ! temperature     at each sample point 
     &  ssamp,          ! salinity        at each sample point
     &  thsamp,         ! potential temp  at each sample point
     &  sigma,          ! density         at each sample point
     &  sigman          ! density anomaly at each sample point

      double precision, dimension(nsample_all,9) ::
     &  lsqarray,       ! least squares array to find coeffs
     &  lsqc            ! lsq work array

      double precision, dimension(9) ::
     &  lsqcoeffs,      ! polynomial coeffs returned by lsq routine
     &  homog           ! solution to homogeneous eqns (used by lsq)

      double precision, dimension(sbdim) ::
     &  lsqsb           ! lsq work array

      double precision, dimension(rdim) ::
     &  lsqr            ! lsq work array

      double precision, dimension(9,km) ::
     &   all_coeffs     ! polynomial coeffs for all levels

      character (60) :: 
     &   depths_file    ! input file with model layer thicknesses

      !***
      !*** bounds for polynomial fit using a reference model of 
      !*** 33 levels from surface to 8000m at z=(k-1)*250 meters
      !*** The user should review the appropriateness of the 
      !*** reference values set below, and modify them if the 
      !*** intended modelling application could be expected to yield 
      !*** temperature and salinity values outside of the ranges set 
      !*** by default.
      !***

      double precision, dimension(33) ::
     &  trefmin = ( / -2.0, -2.0, -2.0, -2.0, -1.0, -1.0,
     &                -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 
     &                -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 
     &                -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  
     &                 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  
     &                 0.0,  0.0,  0.0 / ),
     &  trefmax = ( / 29.0, 19.0, 14.0, 11.0,  9.0,  7.0, 
     &                 7.0,  7.0,  7.0,  7.0,  7.0,  7.0, 
     &                 7.0,  7.0,  7.0,  7.0,  7.0,  7.0, 
     &                 7.0,  7.0,  7.0,  7.0,  7.0,  7.0, 
     &                 7.0,  7.0,  7.0,  7.0,  7.0,  7.0, 
     &                 7.0,  7.0,  7.0 / ),
     &  srefmin = ( / 28.5, 33.7, 34.0, 34.1, 34.2, 34.4,
     &                34.5, 34.5, 34.6, 34.6, 34.6, 34.6, 
     &                34.6, 34.6, 34.6, 34.6, 34.6, 34.6, 
     &                34.6, 34.6, 34.6, 34.6, 34.6, 34.7, 
     &                34.7, 34.7, 34.7, 34.7, 34.7, 34.7, 
     &                34.7, 34.7, 34.7 / ),
     &  srefmax = ( / 37.0, 36.6, 35.8, 35.7, 35.3, 35.1, 
     &                35.1, 35.0, 35.0, 35.0, 35.0, 35.0, 
     &                35.0, 35.0, 35.0, 35.0, 35.0, 35.0, 
     &                35.0, 35.0, 35.0, 35.0, 35.0, 35.0, 
     &                35.0, 35.0, 35.0, 35.0, 35.0, 35.0, 
     &                35.0, 35.0, 35.0 / )

!-----------------------------------------------------------------------
!
!     read in depth profile 
!
!-----------------------------------------------------------------------
 
      write(6,*) 'Input file for model layer thicknesses:'
      read(5,1000) depths_file
 1000 format(a60)

      open(7,file=depths_file,status='unknown')
      do k = 1,km
         read(7,*) dzt(k)
      enddo
      close(7)

!-----------------------------------------------------------------------
!
!     calculate depths of level midpoints from dzt (converting dzt 
!     to meters) - the maximum allowable depth is 8000 meters
!
!-----------------------------------------------------------------------

      zmeters(1) = p5 * dzt(1) * mpercm
      do k=2,km
        zmeters(k) = zmeters(k-1) + p5 * (dzt(k)+dzt(k-1)) * mpercm
      end do

!-----------------------------------------------------------------------
!
!     set the temperature and salinity ranges to be used for each
!     model level when performing the polynomial fitting
!
!-----------------------------------------------------------------------

      do k=1,km
        i = int(zmeters(k)/250.0) + 1
        tmin(k) = trefmin(i)
        tmax(k) = trefmax(i)
        smin(k) = srefmin(i)
        smax(k) = srefmax(i)
      end do

!-----------------------------------------------------------------------
!
!  loop over all model levels
!
!-----------------------------------------------------------------------

      do k=1,km

!-----------------------------------------------------------------------
!
!       sample the temperature range with nsample_temp points
!       and the salinity range with nsample_salt points and
!       create an array of possible combinations of T,S samples
!
!-----------------------------------------------------------------------

        dtemp = (tmax(k)-tmin(k)) / (nsample_temp-c1)
        dsalt = (smax(k)-smin(k)) / (nsample_salt-c1)

        nsample = 0
        do i=1,nsample_temp
          temp_sample = tmin(k) + (i-1)*dtemp
          do j=1,nsample_salt
            nsample = nsample + 1
            salt_sample = smin(k) + (j-1)*dsalt
            tsamp(nsample) = temp_sample
            ssamp(nsample) = salt_sample
          end do
        end do

!-----------------------------------------------------------------------
!
!       loop over the number of samples
!
!-----------------------------------------------------------------------

        !***
        !*** initialize averaging sums
        !***

        avg_temp(k)  = c0
        avg_salt(k)  = c0
        avg_sigma(k) = c0
        avg_theta(k) = c0

        do n=1,nsample_all

!-----------------------------------------------------------------------
!
!         calculate density (sigma) for each t,s combintion at
!         this depth using unesco equation of state
!         unesco returns density (kg per m**3)
!
!-----------------------------------------------------------------------

          call unesco(tsamp(n),ssamp(n),zmeters(k),density)

          sigma(n) = density - 1.0d3 + 2.5d-2

!-----------------------------------------------------------------------
!
!         calculate potential temp. from from insitu temperature,
!         salinity, and depth (pressure)
!
!-----------------------------------------------------------------------

          call potem(tsamp(n),ssamp(n),zmeters(k),thsamp(n))

!-----------------------------------------------------------------------
!
!         accumulate level averages and end loop over samples
!
!-----------------------------------------------------------------------

          avg_temp(k)  = avg_temp(k)  + tsamp(n)
          avg_salt(k)  = avg_salt(k)  + ssamp(n)
          avg_sigma(k) = avg_sigma(k) + sigma(n)
          avg_theta(k) = avg_theta(k) + thsamp(n)

        end do ! loop over samples

!-----------------------------------------------------------------------
!
!       complete layer averages
!
!-----------------------------------------------------------------------

        avg_temp(k)  = avg_temp(k) /real(nsample_all)
        avg_salt(k)  = avg_salt(k) /real(nsample_all)
        avg_theta(k) = avg_theta(k)/real(nsample_all)
        avg_sigma(k) = avg_sigma(k)/real(nsample_all)

!-----------------------------------------------------------------------
!
!       recompute average (reference) density based on level average 
!       values of T, S, and pressure (depth).
!       use average potential temperature in place of average temp
!
!-----------------------------------------------------------------------

        call unesco (avg_temp(k), avg_salt(k), zmeters(k), density)

        avg_sigma(k) = density - 1.0d3 + 2.5d-2
        avg_temp(k) = avg_theta(k)

!-----------------------------------------------------------------------
!
!       fill array for least squares routine with anomalies
!       and their products (the terms in the desired cubic
!       polynomial.
!
!-----------------------------------------------------------------------

        do n=1,nsample_all

          tsamp(n) = thsamp(n)   !*** replace temp with potential temp

          tanom = tsamp(n) - avg_temp(k)
          sanom = ssamp(n) - avg_salt(k)
          sigman(n) = sigma(n) - avg_sigma(k)

          lsqarray(n,1) = tanom
          lsqarray(n,2) = sanom
          lsqarray(n,3) = tanom * tanom
          lsqarray(n,4) = tanom * sanom
          lsqarray(n,5) = sanom * sanom
          lsqarray(n,6) = tanom * tanom * tanom
          lsqarray(n,7) = sanom * sanom * tanom
          lsqarray(n,8) = tanom * tanom * sanom
          lsqarray(n,9) = sanom * sanom * sanom

        end do

!-----------------------------------------------------------------------
!
!       LSQL2 is  a Jet Propulsion Laboratory subroutine that
!       computes the least squares fit in an iterative manner for
!       overdetermined systems. it is called here to iteratively
!       determine polynomial coefficients for cubic polynomials
!       that will best fit the density at the sampled points
!
!-----------------------------------------------------------------------

        irank = 0
        in = 1
        it = 0
        ieq = 2

        call lsqsl2 (nsample_all, lsqarray, nsample_all, 9, sigman, 
     &               lsqcoeffs, irank, 1, itmax, 0, 2, enorm, 1.0d-7, 
     &               9, homog, lsqc, lsqr, lsqsb)

!-----------------------------------------------------------------------
!
!       store coefficients
!
!-----------------------------------------------------------------------

        all_coeffs(:,k) = lsqcoeffs

!-----------------------------------------------------------------------
!
!       end of loop over levels
!
!-----------------------------------------------------------------------

      end do

!-----------------------------------------------------------------------
!
!     rescale some of the coefficients for correct units
!
!-----------------------------------------------------------------------

      do k=1,km
        avg_salt (k) = 1.e-3 * avg_salt (k) - 0.035

        all_coeffs(1,k) = 1.e-3 * all_coeffs(1,k)
        all_coeffs(3,k) = 1.e-3 * all_coeffs(3,k)
        all_coeffs(5,k) = 1.e+3 * all_coeffs(5,k)
        all_coeffs(6,k) = 1.e-3 * all_coeffs(6,k)
        all_coeffs(7,k) = 1.e+3 * all_coeffs(7,k)
        all_coeffs(9,k) = 1.e+6 * all_coeffs(9,k)
      end do

!-----------------------------------------------------------------------
!
!     write reference T,S at 5 words per line
!
!-----------------------------------------------------------------------

      open(50, file='state_coeffs.dat', access='direct',
     &         form='unformatted', recl=km*8,status='unknown')
      write(50,rec=1) avg_temp
      write(50,rec=2) avg_salt
      write(50,rec=3) avg_sigma

!-----------------------------------------------------------------------
!
!     write polynomial coefficients
!
!-----------------------------------------------------------------------

      do i = 1,9
        write(50,rec=i+3) all_coeffs(i,:)
      end do

!-----------------------------------------------------------------------
!
!     write valid ranges for polynomial fit
!
!-----------------------------------------------------------------------

      write (50,rec=13) tmin
      write (50,rec=14) tmax
      write (50,rec=15) smin
      write (50,rec=16) smax

!-----------------------------------------------------------------------
!
!     close file
!
!-----------------------------------------------------------------------

      close (50)
      write (6,*) 'New file has been written in state_coeffs.dat'

!-----------------------------------------------------------------------

      end program eqstat

!***********************************************************************

      subroutine lsqsl2 (ndim,a,d,w,b,x,irank,in,itmax,it,ieq,enorm,
     &                   eps1,nhdim,h,aa,r,s)

!-----------------------------------------------------------------------
!
!     this routine is a modification of lsqsol. march,1968. r. hanson.
!     linear least squares solution
!
!     this routine finds x such that the euclidean length of
!     (*) ax-b is a minimum.
!
!     here a has k rows and n columns, while b is a column vector with
!     k components.
!
!     an orthogonal matrix q is found so that qa is zero below
!     the main diagonal.
!     suppose that rank (a)=r
!     an orthogonal matrix s is found such that
!     qas=t is an r x n upper triangular matrix whose last n-r columns
!     are zero.
!     the system tz=c (c the first r components of qb) is then
!     solved. with w=sz, the solution may be expressed
!     as x = w + sy, where w is the solution of (*) of minimum euclid-
!     ean length and y is any solution to (qas)y=ty=0.
!
!     iterative improvements are calculated using residuals and
!     the above procedures with b replaced by b-ax, where x is an
!     approximate solution.
!
!-----------------------------------------------------------------------

      implicit none

!-----------------------------------------------------------------------
!
!      input variables
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!      output variables
!
!-----------------------------------------------------------------------
!      subroutine lsqsl2 (ndim,a,d,w,b,x,irank,in,itmax,it,ieq,enorm,
!     &                   eps1,nhdim,h,aa,r,s)
!-----------------------------------------------------------------------
!
!     local variables
!
!-----------------------------------------------------------------------

       integer ndim,d,w,irank,in,itmax,it,ieq,nhdim

       double precision a,b,x,enorm,eps1,h,aa,r,s

      double precision sj,dp,up,bp,aj
      logical erm

!
!     in=1 for first entry.
!                   a is decomposed and saved. ax-b is solved.
!     in = 2 for subsequent entries with a new vector b.
!     in=3 to restore a from the previous entry.
!     in=4 to continue the iterative improvement for this system.
!     in = 5 to calculate solutions to ax=0, then store in the array h.
!     in  =  6   do not store a  in aa.  obtain  t = qas, where t is
!     min(k,n) x min(k,n) and upper triangular. now return.do not obtain
!     a solution.
!     no scaling or column interchanges are performed.
!     in  =  7   same as with  in = 6  except that soln. of min. length
!                is placed into x. no iterative refinement.  now return.
!     column interchanges are performed. no scaling is performed.
!     in  = 8    set addresses. now return.
!
!     options for computing  a matrix product   y*h  or  h*y are
!     available with the use of the entry points  myh and mhy.
!     use of these options in these entry points allow a great saving in
!     storage required.
!
!
      dimension a(d,w),b(1),aa(d,w),s(1), x(1),h(nhdim,nhdim),r(1)


      integer i,j,k,l,m,n,isw,ip,lm,irp1,irm1,n1,ns,k1,k2

      double precision eps2,am,a1,sp,top,top1,top2,enm1,a2

      integer j1, j2, j3, j4, j5, j6, j7, j8, j9

!     d = depth of matrix.
!     w = width of matrix.

      k=d
      n=w

!-----------------------------------------------------------------------
!
!     if it=0 on entry, the possible error message will be suppressed.
!
!-----------------------------------------------------------------------

      if (it /= 0) then
        erm = .true.
      else
        erm = .false.
      endif

!-----------------------------------------------------------------------
!
!     set desired relative accuracy (eps1  controls (eps) rank)
!
!-----------------------------------------------------------------------

      eps2 = 1.d-16

!-----------------------------------------------------------------------
!
!     ieq = 2      if column scaling by least max. column length is
!     to be performed.
!
!     ieq = 1       if scaling of all components is to be done with
!     the scalar max(abs(aij))/k*n.
!
!     ieq = 3 if column scaling as with in =2 will be retained in
!     rank deficient cases.
!
!     the array s must contain at least max(k,n) + 4n + 4min(k,n) cells
!        the   array r must contain k+4n s.p. cells.
!
!-----------------------------------------------------------------------

      isw = 1
      l   = min(k,n)
      m   = max(k,n)
      j1  = m
      j2  = j1 + n
      j3  = j2 + n
      j4  = j3 + l
      j5  = j4 + l
      j6  = j5 + l
      j7  = j6 + l
      j8  = j7 + n
      j9  = j8 + n

      if (irank >= 1 .and. irank <= l) then
        lm = irank
      else
        lm  = l
      endif

      select case (in)
      case (1)
        goto 10
      case (2)
        goto 360
      case (3)
        goto 810
      case (4)
        goto 390
      case (5)
        goto 830
      case (6)
        lm = l
        goto 10
      case (7)
        goto 10
      case (8)
        return
      case default
        stop 'unknown choice for in in lsqsl2'
      end select

!-----------------------------------------------------------------------
!
!     equilibrate columns of a (1)-(2).
!
!-----------------------------------------------------------------------

   10 continue

!-----------------------------------------------------------------------
!
!     save data when in = 1.
!
!-----------------------------------------------------------------------

      if (in == 1) then
        aa = a
      endif

      if (ieq /= 1) then
        do j=1,n
          am = 0.d0
          do i=1,k
            am = max(am,abs(a(i,j)))
          end do

          !*** s(m+n+1)-s(m+2n) contains scaling for output variables.

          if (in == 6) am = 1.d0
          s(j2+j) = 1.d0/am
          do i=1,k
            a(i,j) = a(i,j)*s(j2+j)
          end do
        end do
      else
        am = 0.d0
        do j=1,n
          do i=1,k
            am = max(am,abs(a(i,j)))
          end do
        end do
        am = am/real(k*n)
        if (in == 6) am = 1.d0
        do j=1,n
          s(j2+j) = 1.d0/am
          do i=1,k
            a(i,j) = a(i,j)*s(j2+j)
          end do
        end do
      endif

!-----------------------------------------------------------------------
!
!     compute column lengths
!
!-----------------------------------------------------------------------

      do j=1,n
        s(j7+j) = s(j2+j)
      end do

!-----------------------------------------------------------------------
!
!      s(m+1)-s(m+ n) contains variable permutations.
!     set permutation to identity.
!
!-----------------------------------------------------------------------

      do j=1,n
        s(j1+j) = j
      end do

!-----------------------------------------------------------------------
!
!     begin elimination on the matrix a with orthogonal matrices .
!     ip=pivot row
!
!-----------------------------------------------------------------------

      do 250 ip=1,lm

        dp = 0.d0
        k2 = ip
        do j=ip,n
          sj = 0.d0
          do i=ip,k
            sj = sj + a(i,j)**2
          end do
          if (sj >= dp) then
            dp = sj
            k2 = j
            if (in == 6) go to 160
          endif
        end do

!-----------------------------------------------------------------------
!
!       maximize (sigma)**2 by column interchange.
!        supress column interchanges when in=6.
!        exchange columns if necessary.
!
!-----------------------------------------------------------------------

        if (k2 == ip) go to 160
        do i=1,k
          a1      = a(i,ip)
          a(i,ip) = a(i,k2)
          a(i,k2) = a1
        end do

!-----------------------------------------------------------------------
!
!       record permutation and exchange squares of column lengths.
!
!-----------------------------------------------------------------------

        a1       = s(j1+k2)
        s(j1+k2) = s(j1+ip)
        s(j1+ip) = a1

        a1       = s(j7+k2)
        s(j7+k2) = s(j7+ip)
        s(j7+ip) = a1

  160   if (ip /= 1) then
          a1 = 0.d0
          do i=1,ip-1
            a1 = a1 + a(i,ip)**2
          end do
          if (a1 > 0.d0) go to 190
        end if
        if (dp > 0.d0) go to 200

!-----------------------------------------------------------------------
!
!       test for rank deficiency.
!
!-----------------------------------------------------------------------

  190   if (sqrt(dp/a1) > eps1 .or. in == 6) go to 200
        if (erm) write (6,1140) irank,eps1,ip-1,ip-1
        irank = ip-1
        erm = .false.
        go to 260

!-----------------------------------------------------------------------
!
!       (eps1) rank is deficient.
!
!-----------------------------------------------------------------------

  200   sp = sqrt(dp)

!-----------------------------------------------------------------------
!
!       begin front elimination on column ip.
!       sp=sqroot(sigma**2).
!
!-----------------------------------------------------------------------

        bp = 1.d0/(dp+sp*abs(a(ip,ip)))

!-----------------------------------------------------------------------
!
!       store beta in s(3n+1)-s(3n+l).
!
!-----------------------------------------------------------------------

        if (ip == k) bp = 0.d0
        r(k+2*n+ip) = bp
        up = sign(sp + abs(a(ip,ip)), a(ip,ip))
        if (ip < k) then
          if (ip < n) then
            do j=ip+1,n
              sj = 0.d0
              do i=ip+1,k
                sj=sj+a(i,j)*a(i,ip)
              end do
              sj = sj + up*a(ip,j)
              sj = bp*sj  !*** sj=yj now
  
              do i=ip+1,k
                a(i,j) = a(i,j) - a(i,ip)*sj
              end do
              a(ip,j) = a(ip,j) - sj*up
            end do
          endif
          a(ip,ip)    = -sign(sp,a(ip,ip))
          r(k+3*n+ip) = up
        endif

  250 continue
      irank = lm
  260 irp1 = irank+1
      irm1 = irank-1

      if (irank == 0 .or. irank == n) go to 360

!-----------------------------------------------------------------------
!
!     begin back processing for rank deficiency case
!      if irank is less than n.
!
!-----------------------------------------------------------------------

      if (ieq /= 3) then
        do j=1,n
          l = min(j,irank)

          !*** unscale columns for rank deficient matrices when ieq /= 3
          do i=1,l
            a(i,j) = a(i,j)/s(j7+j)
          end do

          s(j7+j) = 1.d0
          s(j2+j) = 1.d0
        end do
      endif

      ip = irank

  300 sj = 0.d0
      do j=irp1,n
        sj = sj + a(ip,j)**2
      end do
      sj = sj + a(ip,ip)**2
      aj = sqrt(sj)
      up = sign(aj+abs(a(ip,ip)), a(ip,ip))

!-----------------------------------------------------------------------
!
!     ip th element of u vector calculated.
!     bp = 2/length of u squared.
!
!-----------------------------------------------------------------------

      bp = 1.d0/(sj+abs(a(ip,ip))*aj)

      if (ip-1 > 0) then
        do i=1,ip-1
          dp = a(i,ip)*up
          do j=irp1,n
            dp = dp + a(i,j)*a(ip,j)
          end do
          dp = dp/(sj+abs(a(ip,ip))*aj)

          !*** calc. (aj,u), where aj=jth row of a

          a(i,ip) = a(i,ip) - up*dp

          !*** modify array a.

          do j=irp1,n
            a(i,j) = a(i,j) - a(ip,j)*dp
          end do
        end do
      endif

      a(ip,ip) = -sign(aj,a(ip,ip))

!-----------------------------------------------------------------------
!
!     calc. modified pivot.
!     save beta and ip th element of u vector in r array.
!
!-----------------------------------------------------------------------

      r(k+ip)   = bp
      r(k+n+ip) = up

!-----------------------------------------------------------------------
!
!     test for end of back processing.
!
!-----------------------------------------------------------------------

      if (ip-1 > 0) then
        ip=ip-1
        go to 300
      endif

  360 if (in == 6) return

      do j=1,k
        r(j) = b(j)
      end do
      it=0

!-----------------------------------------------------------------------
!
!     set initial x vector to zero.
!
!-----------------------------------------------------------------------

      do j=1,n
        x(j) = 0.d0
      end do
      if (irank == 0) go to 690

!-----------------------------------------------------------------------
!
!     apply q to rt. hand side.
!
!-----------------------------------------------------------------------

  390 do ip=1,irank
        sj = r(k+3*n+ip)*r(ip)
        if (ip+1 <= k) then
          do i=ip+1,k
            sj = sj + a(i,ip)*r(i)
          end do
        endif
        bp = r(k+2*n+ip)
        if (ip+1 <= k) then
          do i=ip+1,k
            r(i) = r(i) - bp*a(i,ip)*sj
          end do
        endif
        r(ip) = r(ip) - bp*r(k+3*n+ip)*sj
      end do

      do j=1,irank
        s(j) = r(j)
      end do

      enorm = 0.d0
      if (irp1.gt.k) go to 510
      do j=irp1,k
        enorm = enorm + r(j)**2
      end do
      enorm = sqrt(enorm)
      go to 510

  460 do j=1,n
        sj = 0.d0
        ip = s(j1+j)
        do i=1,k
          sj = sj + r(i)*aa(i,ip)
        end do

        !*** apply to rt. hand side.  apply scaling.
c
        r(k+n+j) = sj*s(j2+ip)
      end do

      s(1) = r(k+n+1)/a(1,1)
      if (n /= 1) then
        do j=2,n
          sj = 0.d0
          do i=1,j-1
            sj = sj + a(i,j)*s(i)
          end do
          s(j) = (r(k+j+n)-sj)/a(j,j)
        end do
      endif

!-----------------------------------------------------------------------
!
!     entry to continue iterating.  solves tz = c = 1st irank
!     components of qb .
!
!-----------------------------------------------------------------------

  510 s(irank) = s(irank)/a(irank,irank)
      if (irm1 /= 0) then
        do j=1,irm1
          n1 = irank-j
          sj = 0.
          do i=n1+1,irank
            sj = sj + a(n1,i)*s(i)
          end do
          s(n1) = (s(n1)-sj)/a(n1,n1)
        end do
      endif

!-----------------------------------------------------------------------
!
!     z calculated.  compute x = sz.
!
!-----------------------------------------------------------------------

      if (irank /= n) then
        do j=irp1,n
          s(j) = 0.d0
        end do
        do i=1,irank
          sj = r(k+n+i)*s(i)
          do j=irp1,n
            sj = sj + a(i,j)*s(j)
          end do
          do j=irp1,n
            s(j) = s(j) - a(i,j)*r(k+i)*sj
          end do
          s(i) = s(i) - r(k+i)*r(k+n+i)*sj
        end do
      endif

!-----------------------------------------------------------------------
!
!     increment for x of minimal length calculated.
!
!-----------------------------------------------------------------------

      do i=1,n
        x(i) = x(i) + s(i)
      end do

      if (in.eq.7) go to 750

!-----------------------------------------------------------------------
!
!     calc. sup norm of increment and residuals
!
!-----------------------------------------------------------------------

      top1 = 0.d0
      do j=1,n
        top1 = max(top1,abs(s(j))*s(j7+j))
      end do

      do i=1,k
        sj = 0.d0
        do j=1,n
          ip = s(j1+j)
          sj = sj + aa(i,ip)*x(j)*s(j2+ip)
        end do
        r(i) = b(i) - sj
      end do

      if (itmax <= 0) go to 750

!-----------------------------------------------------------------------
!
!     calc. sup norm of x.
!
!-----------------------------------------------------------------------

      top = 0.d0
      do j=1,n
        top = max(top,abs(x(j))*s(j7+j))
      end do

!-----------------------------------------------------------------------
!
!     compare relative change in x with tolerance eps .
!
!-----------------------------------------------------------------------

      if (top1-top*eps2 > 0) then
        if (it-itmax < 0) then
          it = it+1
          if (it /= 1) then
            if (top1 > .25*top2) go to 690
          endif
          top2 = top1
          if (isw == 1) then
            go to 390
          else if (isw ==2) then
            go to 460
          endif
        endif
        it=0
      endif
  690 sj=0.d0
      do j=1,k
        sj = sj + r(j)**2
      end do
      enorm = sqrt(sj)

      if (irank == n .and. isw == 1) then
        enm1 = enorm

        !*** save x array

        do j=1,n
          r(k+j) = x(j)
        end do
        isw = 2
        it = 0
        go to 460
      endif

!-----------------------------------------------------------------------
!
!     choose best solution
!
!-----------------------------------------------------------------------

      if (irank >= n .and. enorm > enm1) then
        do j=1,n
          x(j) = r(k+j)
        end do
        enorm = enm1
      endif

!-----------------------------------------------------------------------
!
!     norm of ax - b located in the cell enorm .
!     rearrange variables.
!
!-----------------------------------------------------------------------

  750 do j=1,n
        s(j) = s(j1+j)
      end do
      do j=1,n
        do i=j,n
          ip = s(i)
          if (j == ip) go to 780
        end do
  780   s(i) = s(j)
        s(j) = j
        sj   = x(j)
        x(j) = x(i)
        x(i) = sj
      end do

!-----------------------------------------------------------------------
!
!     scale variables.
!
!-----------------------------------------------------------------------

      do j=1,n
        x(j) = x(j)*s(j2+j)
      end do
      return

!-----------------------------------------------------------------------
!
!     restore a.
!
!-----------------------------------------------------------------------

  810 do j=1,n
        do i=1,k
          a(i,j) = aa(i,j)
        end do
      end do
      return

!-----------------------------------------------------------------------
!
!     generate solutions to the homogeneous equation ax = 0.
!
!-----------------------------------------------------------------------

  830 if (irank == n) return
      ns = n-irank
      do i=1,n
        do j=1,ns
          h(i,j) = 0.d0
        end do
      end do

      do j=1,ns
        h(irank+j,j) = 1.d0
      end do

      if (irank == 0) return

      do j=1,irank
        do i=1,ns
          sj = r(k+n+j)*h(j,i)
          do k1=irp1,n
            sj = sj + h(k1,i)*a(j,k1)
          end do
          bp = r(k+j)
          dp = bp*r(k+n+j)*sj
          a1 = dp
          a2 = dp-a1
          h(j,i) = h(j,i)-(a1+2.*a2)
          do k1=irp1,n
            dp = bp*a(j,k1)*sj
            a1 = dp
            a2 = dp - a1
            h(k1,i)=h(k1,i)-(a1+2.*a2)
          end do
        end do
      end do

!-----------------------------------------------------------------------
!
!     rearrange rows of solution matrix.
!
!-----------------------------------------------------------------------

      do j=1,n
        s(j) = s(j1+j)
      end do
      do j=1,n
        do i=j,n
          ip = s(i)
          if (j == ip) go to 900
        end do
  900   s(i) = s(j)
        s(j) = j
        do k1=1,ns
          a1      = h(j,k1)
          h(j,k1) = h(i,k1)
          h(i,k1) = a1
        end do
      end do

!-----------------------------------------------------------------------

 1140 format (/'warning. irank has been set to',i4,'  but(',1pe10.3,
     & ') rank is',i4,'.  irank is now taken as ',i4)

!-----------------------------------------------------------------------

      end subroutine lsqsl2

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
!***********************************************************************

      subroutine potem (temp, salt, depth, theta)

!-----------------------------------------------------------------------
!
!     this subroutine calculates potential temperature as a function
!     of in-situ temperature, salinity, and pressure.
!
!     references:
!        Bryden, H.L., New polynomials for thermal expansion, adiabatic
!           temperature gradient and potential temperature of sea water,
!           Deap-Sea Res., 20, 401-408, 1973.
!
!-----------------------------------------------------------------------

      implicit none

!-----------------------------------------------------------------------
!
!     input variables
!
!-----------------------------------------------------------------------

      double precision, intent(in) ::
     &  temp,           ! in-situ temperature [degrees centigrade]
     &  salt,           ! salinity [per mil]
     &  depth           ! depth [meters]

!-----------------------------------------------------------------------
!
!     output variables
!
!-----------------------------------------------------------------------

      double precision, intent(out) ::
     &  theta           ! potential temperature [degrees centigrade]

!-----------------------------------------------------------------------
!
!     local variables
!
!-----------------------------------------------------------------------

      double precision :: pbars, prss2, prss3, potmp,
     &  press           ! pressure [decibars]

!-----------------------------------------------------------------------
!
!     set up constant and convert depth to pressure in decibars 
!     and then to pressure in bars
!     compute powers of several variables
!
!-----------------------------------------------------------------------

      call depth_press (depth, press)
      pbars = press * 0.1d0

      prss2 = pbars*pbars
      prss3 = prss2*pbars

!-----------------------------------------------------------------------
!
!     compute potential temperature from polynomial
!
!-----------------------------------------------------------------------

      potmp = pbars*(3.6504d-4 + temp*(8.3198d-5 + 
     &        temp*(-5.4065d-7 + temp*4.0274d-9))) - 
     &        pbars*(salt - 35.0d0)*(1.7439d-5 -
     &        temp*2.9778d-7) - prss2*(8.9309d-7 + 
     &        temp*(-3.1628d-8 + temp*2.1987d-10)) + 
     &        4.1057d-9*prss2*(salt - 35.0d0) -
     &        prss3*(-1.6056d-10 + temp*5.0484d-12)

      theta = temp - potmp

!-----------------------------------------------------------------------

      end subroutine potem

!***********************************************************************

      subroutine unesco (temp, salt, depth, rho)

!-----------------------------------------------------------------------
!
!     this subroutine calculates the density of seawater using the
!     standard equation of state recommended by unesco (1981).
!
!     references:
!        Gill, A., Atmosphere-Ocean Dynamics: International Geophysical
!         Series No. 30. Academic Press, London, 1982, pp 599-600.
!        UNESCO, 10th report of the joint panel on oceanographic tables
!          and standards. UNESCO Tech. Papers in Marine Sci. No. 36,
!          Paris, 1981.
!
!-----------------------------------------------------------------------

      implicit none

!-----------------------------------------------------------------------
!
!     input variables
!
!-----------------------------------------------------------------------

      double precision, intent(in) ::
     &  temp,           ! in-situ temperature [degrees centigrade]
     &  salt,           ! salinity [practical salinity units]
     &  depth           ! depth [meters]

!-----------------------------------------------------------------------
!
!     output variables
!
!-----------------------------------------------------------------------

      double precision, intent(out) ::
     &  rho             ! density in kilograms per cubic meter

!-----------------------------------------------------------------------
!
!     local variables
!
!-----------------------------------------------------------------------

      double precision :: rw, rsto, xkw, xksto, xkstp, pbars,
     &                    tem2, tem3, tem4, tem5, slt2, st15, pbar2, 
     &  press           ! pressure [decibars]

!-----------------------------------------------------------------------
!
!     set up constant and convert depth to pressure in decibars 
!     and then to pressure in bars
!     compute powers of several variables
!
!-----------------------------------------------------------------------

      call depth_press (depth, press)
      pbars = press * 0.1d0

      tem2 = temp**2
      tem3 = temp**3
      tem4 = temp**4
      tem5 = temp**5
      slt2 = salt**2
      st15 = salt**(1.5d0) 
      pbar2 = pbars**2

!-----------------------------------------------------------------------
!
!     compute density
!
!-----------------------------------------------------------------------

      rw =     9.99842594d+2 + 6.793952d-2*temp - 
     &         9.095290d-3*tem2 + 1.001685d-4*tem3 - 
     &         1.120083d-6*tem4 + 6.536332d-9*tem5

      rsto =   rw + 
     &         (8.24493d-1 - 
     &          4.0899d-3*temp + 7.6438d-5*tem2
     &        - 8.2467d-7*tem3 + 5.3875d-9*tem4)*salt
     &      + (-5.72466d-3 + 1.0227d-4*temp - 
     &          1.6546d-6*tem2)*st15 + 4.8314d-4*slt2

      xkw =    1.965221d+4 + 
     &         1.484206d+2*temp - 2.327105d+0*tem2 +
     &         1.360477d-2*tem3 - 5.155288d-5*tem4

      xksto =  xkw + 
     &    (5.46746d+1      - 6.03459d-1*temp + 
     &     1.09987d-2*tem2 - 6.1670d-5*tem3)*salt
     &  + (7.944d-2  + 
     &     1.6483d-2*temp - 5.3009d-4*tem2)*st15

      xkstp =  xksto + 
     &   (3.239908d+0     + 1.43713d-3*temp + 
     &    1.16092d-4*tem2 - 5.77905d-7*tem3)*pbars
     & + (2.2838d-3       - 1.0981d-5*temp - 
     &    1.6078d-6 *tem2)*pbars*salt
     & + 1.91075d-4       *pbars*st15
     & + (8.50935d-5      - 6.12293d-6*temp + 
     &                      5.2787d-8 *tem2)*pbar2
     & + (-9.9348d-7      + 2.0816d-8 *temp + 
     &    9.1697d-10*tem2)*pbar2*salt

      rho =   rsto / (1.d0 - pbars/xkstp)

!-----------------------------------------------------------------------

      end subroutine unesco

!***********************************************************************

      subroutine depth_press (depth, press)

!-----------------------------------------------------------------------
!
!     this subroutine converts depth in meters to pressure in decibars
!     by using a mean density derived from depth-dependent global 
!     average temperatures and salinities from Levitus_94, and 
!     integrating using hydrostatic balance.
!
!     references:
!        Levitus, S., R. Burgett, and T.P. Boyer, World Ocean Atlas 
!          1994, Volume 3: Salinity, NOAA Atlas NESDIS 3, US Dept. of 
!          Commerce, 1994.
!        Levitus, S. and T.P. Boyer, World Ocean Atlas 1994, 
!          Volume 4: Temperature, NOAA Atlas NESDIS 4, US Dept. of 
!          Commerce, 1994.
!
!-----------------------------------------------------------------------

      implicit none

!-----------------------------------------------------------------------
!
!     input variable
!
!-----------------------------------------------------------------------

      double precision, intent(in) :: depth    ! depth in meters

!-----------------------------------------------------------------------
!
!     output variable
!
!-----------------------------------------------------------------------

      double precision, intent(out) :: press   ! pressure [decibars]

!-----------------------------------------------------------------------
!
!     convert depth in meters to pressure in decibars
!
!-----------------------------------------------------------------------

      press = 0.9806d0*(1.3211526d0*(exp(-0.0025d0*depth) - 1.d0) 
     &        + (1.02818d0 + 2.24586d-6*depth)*depth)

!-----------------------------------------------------------------------

      end subroutine depth_press

!***********************************************************************

      subroutine GSMCAMRPOP(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,rhs
     & ,irhslo0,irhslo1
     & ,irhshi0,irhshi1
     & ,icoloredboxlo0,icoloredboxlo1
     & ,icoloredboxhi0,icoloredboxhi1
     & ,dx
     & ,alpha
     & ,beta
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1)
      integer irhslo0,irhslo1
      integer irhshi0,irhshi1
      REAL*8 rhs(
     & irhslo0:irhshi0,
     & irhslo1:irhshi1)
      integer icoloredboxlo0,icoloredboxlo1
      integer icoloredboxhi0,icoloredboxhi1
      REAL*8 dx
      REAL*8 alpha
      REAL*8 beta
      REAL*8 lambda, dxinv, sum_b, lphi
      integer i,j
      integer idir
      dxinv = (1.0d0)/(dx*dx)
      sum_b = 0.0
      do idir = 0, 2 -1
         sum_b = sum_b + (2.0d0)*dxinv
      enddo
      lambda = (1.0d0)/(alpha - beta*sum_b)
      chiterations=(1+icoloredBoxhi1-icoloredBoxlo1)*(1+icoloredBoxhi0-i
     &coloredBoxlo0)*1
      do j = icoloredBoxlo1,icoloredBoxhi1,2
      do i = icoloredBoxlo0,icoloredBoxhi0,2
        lphi =
     & ( phi(i+1,j )
     & + phi(i-1,j )
     $ -(2.0d0)*phi(i ,j ))
     $ +( phi(i ,j+1)
     & + phi(i ,j-1)
     $ -(2.0d0)*phi(i ,j ))
        lphi = lphi*dxinv
        phi(i,j) =
     $ phi( i,j) +
     & lambda*( rhs( i,j) - lphi)
      enddo
      enddo
      chloads=chloads+chiterations*2*8
      chstores=chstores+chiterations*8
      chflops=chflops+chiterations*(3*2 +4)
      return
      end
      subroutine GSRBHELMHOLTZ(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,rhs
     & ,irhslo0,irhslo1
     & ,irhshi0,irhshi1
     & ,nrhscomp
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,dx
     & ,alpha
     & ,beta
     & ,redBlack
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer nphicomp
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1,
     & 0:nphicomp-1)
      integer nrhscomp
      integer irhslo0,irhslo1
      integer irhshi0,irhshi1
      REAL*8 rhs(
     & irhslo0:irhshi0,
     & irhslo1:irhshi1,
     & 0:nrhscomp-1)
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      REAL*8 dx
      REAL*8 alpha
      REAL*8 beta
      integer redBlack
      REAL*8 lambda, dxinv, sum_b, lphi, helmop
      integer i,j
      integer n,ncomp,idir,indtot,imin,imax
      dxinv = (1.0d0)/(dx*dx)
      sum_b = 0.0
      do idir = 0, 2 -1
         sum_b = sum_b + (2.0d0)*dxinv
      enddo
      lambda = -(1.0d0)/(alpha - beta*sum_b)
      ncomp = nphicomp
      if(ncomp .ne. nrhscomp) then
         call MAYDAYERROR()
      endif
      chiterations=0
      do n = 0, ncomp - 1
          do j=iregionlo1, iregionhi1
            imin = iregionlo0
            indtot = imin + j
            imin = imin + abs(mod(indtot + redBlack, 2))
            imax = iregionhi0
            chiterations=chiterations+(imax-imin)/2
            do i = imin, imax, 2
              lphi = (phi(i+1,j,n)
     & + phi(i-1,j,n)
     & + phi(i,j+1,n)
     & + phi(i,j-1,n)
     & -(2.0d0)*2*phi(i,j,n))*dxinv
              helmop = alpha*phi(i,j,n) + beta*lphi
              phi(i,j,n) = phi(i,j,n) +
     & lambda*(helmop - rhs(i,j,n))
            enddo
          enddo
      enddo
      chloads=chloads+chiterations*2*8
      chstores=chstores+chiterations*8
      chflops=chflops+chiterations*(2*2 +9)
      return
      end
      subroutine GSRBLAPLACIAN(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,rhs
     & ,irhslo0,irhslo1
     & ,irhshi0,irhshi1
     & ,nrhscomp
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,dx
     & ,redBlack
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer nphicomp
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1,
     & 0:nphicomp-1)
      integer nrhscomp
      integer irhslo0,irhslo1
      integer irhshi0,irhshi1
      REAL*8 rhs(
     & irhslo0:irhshi0,
     & irhslo1:irhshi1,
     & 0:nrhscomp-1)
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      REAL*8 dx
      integer redBlack
      REAL*8 lambda, dxinv, sum_b, lphi, lap
      integer i,j
      integer n,ncomp,idir,indtot,imin,imax
      dxinv = (1.0d0)/(dx*dx)
      sum_b = 0.0
      do idir = 0, 2 -1
         sum_b = sum_b + (2.0d0)*dxinv
      enddo
      lambda = -(1.0d0)/sum_b
      chiterations=0
      ncomp = nphicomp
      if(ncomp .ne. nrhscomp) then
         call MAYDAYERROR()
      endif
      do n = 0, ncomp - 1
          do j=iregionlo1, iregionhi1
            imin = iregionlo0
            indtot = imin + j
            imin = imin + abs(mod(indtot + redBlack, 2))
            imax = iregionhi0
            chiterations=chiterations+(imax-imin)/2
            do i = imin, imax, 2
              lphi = (
     & phi(i+1,j,n)
     & + phi(i-1,j,n)
     & + phi(i,j+1,n)
     & + phi(i,j-1,n)
     & ) * dxinv
              phi(i,j,n) = lambda*(rhs(i,j,n)-lphi)
            enddo
          enddo
      enddo
      chloads=chloads+chiterations*2*8
      chstores=chstores+chiterations*1*8
      chflops=chflops+chiterations*(2*2 +2)
      return
      end
      subroutine GSRBLAZY(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,lphi
     & ,ilphilo0,ilphilo1
     & ,ilphihi0,ilphihi1
     & ,rhs
     & ,irhslo0,irhslo1
     & ,irhshi0,irhshi1
     & ,icoloredboxlo0,icoloredboxlo1
     & ,icoloredboxhi0,icoloredboxhi1
     & ,alpha
     & ,beta
     & ,dx
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1)
      integer ilphilo0,ilphilo1
      integer ilphihi0,ilphihi1
      REAL*8 lphi(
     & ilphilo0:ilphihi0,
     & ilphilo1:ilphihi1)
      integer irhslo0,irhslo1
      integer irhshi0,irhshi1
      REAL*8 rhs(
     & irhslo0:irhshi0,
     & irhslo1:irhshi1)
      integer icoloredboxlo0,icoloredboxlo1
      integer icoloredboxhi0,icoloredboxhi1
      REAL*8 alpha
      REAL*8 beta
      REAL*8 dx
      integer i,j, idir
      REAL*8 dxinv, sum_b, lambda
      dxinv = (1.0d0)/(dx*dx)
      sum_b = 0.0
      do idir = 0, 2 -1
         sum_b = sum_b + (2.0d0)*dxinv
      enddo
      lambda = -(1.0d0)/(alpha - beta*sum_b)
      chiterations=(1+icoloredBoxhi1-icoloredBoxlo1)*(1+icoloredBoxhi0-i
     &coloredBoxlo0)*1
      do j = icoloredBoxlo1,icoloredBoxhi1,2
      do i = icoloredBoxlo0,icoloredBoxhi0,2
      phi(i,j) =
     $ phi( i,j) -
     & lambda*(
     $ rhs( i,j) -
     $ lphi( i,j))
      enddo
      enddo
      chloads=chloads+chiterations*3*8
      chstores=chstores+chiterations*1*8
      chflops=chflops+chiterations*3
      return
      end
      subroutine AMRPMULTICOLOR(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,rhs
     & ,irhslo0,irhslo1
     & ,irhshi0,irhshi1
     & ,weight
     & ,alpha
     & ,beta
     & ,dx
     & ,icoloredboxlo0,icoloredboxlo1
     & ,icoloredboxhi0,icoloredboxhi1
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1)
      integer irhslo0,irhslo1
      integer irhshi0,irhshi1
      REAL*8 rhs(
     & irhslo0:irhshi0,
     & irhslo1:irhshi1)
      REAL*8 weight
      REAL*8 alpha
      REAL*8 beta
      REAL*8 dx(0:1)
      integer icoloredboxlo0,icoloredboxlo1
      integer icoloredboxhi0,icoloredboxhi1
      integer i,j
      REAL*8 laplphi, dx0,dx1
      dx0 = beta/(dx(0) * dx(0))
                dx1 = beta/(dx(1) * dx(1))
      chiterations=(1+icoloredBoxhi1-icoloredBoxlo1)*(1+icoloredBoxhi0-i
     &coloredBoxlo0)*1
      do j = icoloredBoxlo1,icoloredBoxhi1,2
      do i = icoloredBoxlo0,icoloredBoxhi0,2
        laplphi =
     & ( phi(i+1,j )
     & + phi(i-1,j )
     $ -(2.0d0)*phi(i ,j ))*dx0
     $ +( phi(i ,j+1)
     & + phi(i ,j-1)
     $ -(2.0d0)*phi(i ,j ))*dx1
        laplphi = laplphi + alpha * phi(i,j)
        phi(i,j) = phi(i,j) +
     & weight*(rhs(i,j) - laplphi)
      enddo
      enddo
      return
      end
      subroutine OPERATORLAP(
     & lofphi
     & ,ilofphilo0,ilofphilo1
     & ,ilofphihi0,ilofphihi1
     & ,nlofphicomp
     & ,phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,dx
     & ,alpha
     & ,beta
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer nlofphicomp
      integer ilofphilo0,ilofphilo1
      integer ilofphihi0,ilofphihi1
      REAL*8 lofphi(
     & ilofphilo0:ilofphihi0,
     & ilofphilo1:ilofphihi1,
     & 0:nlofphicomp-1)
      integer nphicomp
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1,
     & 0:nphicomp-1)
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      REAL*8 dx
      REAL*8 alpha
      REAL*8 beta
      REAL*8 dxinv, lap
      integer n,ncomp
      integer i,j
      ncomp = nphicomp
      if(ncomp .ne. nlofphicomp) then
         call MAYDAYERROR()
      endif
      dxinv = (1.0d0)/(dx*dx)
      do n = 0, ncomp-1
      chiterations=(1+iregionhi1-iregionlo1)*(1+iregionhi0-iregionlo0)*1
      do j = iregionlo1,iregionhi1
      do i = iregionlo0,iregionhi0
          lap = ( phi(i+1,j ,n)
     & + phi(i-1,j ,n)
     & + phi(i ,j+1,n)
     & + phi(i ,j-1,n)
     & -(2*2)*phi(i,j,n) )
     & * dxinv
          lofphi(i,j,n) = alpha*phi(i,j,n)+beta*lap
      enddo
      enddo
      enddo
      chloads=chloads+chiterations*8
      chstores=chstores+chiterations*8
      chflops=chflops+chiterations*(3*2 +6)
      return
      end
      subroutine OPERATORLAPRES(
     & r
     & ,irlo0,irlo1
     & ,irhi0,irhi1
     & ,nrcomp
     & ,phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,rhs
     & ,irhslo0,irhslo1
     & ,irhshi0,irhshi1
     & ,nrhscomp
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,dx
     & ,alpha
     & ,beta
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer nrcomp
      integer irlo0,irlo1
      integer irhi0,irhi1
      REAL*8 r(
     & irlo0:irhi0,
     & irlo1:irhi1,
     & 0:nrcomp-1)
      integer nphicomp
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1,
     & 0:nphicomp-1)
      integer nrhscomp
      integer irhslo0,irhslo1
      integer irhshi0,irhshi1
      REAL*8 rhs(
     & irhslo0:irhshi0,
     & irhslo1:irhshi1,
     & 0:nrhscomp-1)
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      REAL*8 dx
      REAL*8 alpha
      REAL*8 beta
      REAL*8 dxinv, lap
      integer n,ncomp
      integer i,j
      ncomp = nphicomp
      dxinv = (1.0d0)/(dx*dx)
      do n = 0, ncomp-1
      chiterations=(1+iregionhi1-iregionlo1)*(1+iregionhi0-iregionlo0)*1
      do j = iregionlo1,iregionhi1
      do i = iregionlo0,iregionhi0
          lap = ( phi(i+1,j ,n)
     & + phi(i-1,j ,n)
     & + phi(i ,j+1,n)
     & + phi(i ,j-1,n)
     & -(2*2)*phi(i,j,n) )
     & * dxinv
         r(i,j,n) = -alpha*phi(i,j,n) -beta*lap +
     & rhs(i,j,n)
      enddo
      enddo
      enddo
      chloads=chloads+chiterations*2*8
      chstores=chstores+chiterations*8
      chflops=chflops+chiterations*(3*2 +7)
      return
      end
      subroutine RESTRICTRES(
     & res
     & ,ireslo0,ireslo1
     & ,ireshi0,ireshi1
     & ,nrescomp
     & ,phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,rhs
     & ,irhslo0,irhslo1
     & ,irhshi0,irhshi1
     & ,nrhscomp
     & ,alpha
     & ,beta
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,dx
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer nrescomp
      integer ireslo0,ireslo1
      integer ireshi0,ireshi1
      REAL*8 res(
     & ireslo0:ireshi0,
     & ireslo1:ireshi1,
     & 0:nrescomp-1)
      integer nphicomp
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1,
     & 0:nphicomp-1)
      integer nrhscomp
      integer irhslo0,irhslo1
      integer irhshi0,irhshi1
      REAL*8 rhs(
     & irhslo0:irhshi0,
     & irhslo1:irhshi1,
     & 0:nrhscomp-1)
      REAL*8 alpha
      REAL*8 beta
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      REAL*8 dx
      REAL*8 denom,dxinv,lofphi
      integer n,ncomp
      integer i,j
      integer ii,jj
      ncomp = nphicomp
      dxinv = (1.0d0) / (dx*dx)
      denom = 2 *2
      do n = 0, ncomp-1
      chiterations=(1+iregionhi1-iregionlo1)*(1+iregionhi0-iregionlo0)*1
      do j = iregionlo1,iregionhi1
      do i = iregionlo0,iregionhi0
          ii = i/2
          jj = j/2
          lofphi = alpha * phi(i,j,n)
     & + beta *
     & ( phi(i+1,j ,n)
     & + phi(i-1,j ,n)
     & + phi(i ,j+1,n)
     & + phi(i ,j-1,n)
     & - phi(i ,j ,n) * 2 * 2
     & ) * dxinv
          res(ii,jj,n) = res(ii,jj,n)
     & + (rhs(i,j,n) - lofphi) / denom
          chiterations=chiterations+1
      enddo
      enddo
      enddo
      chloads=chloads+chiterations*2*8
      chstores=chstores+chiterations*8
      chflops=chflops+chiterations*(3*2 +7)
      return
      end
      subroutine PROLONG(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,coarse
     & ,icoarselo0,icoarselo1
     & ,icoarsehi0,icoarsehi1
     & ,ncoarsecomp
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,m
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer nphicomp
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1,
     & 0:nphicomp-1)
      integer ncoarsecomp
      integer icoarselo0,icoarselo1
      integer icoarsehi0,icoarsehi1
      REAL*8 coarse(
     & icoarselo0:icoarsehi0,
     & icoarselo1:icoarsehi1,
     & 0:ncoarsecomp-1)
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      integer m
      INTEGER ncomp, n
      integer i,j
      integer ii,jj
      ncomp = nphicomp
      do n = 0, ncomp-1
      chiterations=(1+iregionhi1-iregionlo1)*(1+iregionhi0-iregionlo0)*1
      do j = iregionlo1,iregionhi1
      do i = iregionlo0,iregionhi0
          ii = i/m
          jj = j/m
          phi(i,j,n) = phi(i,j,n) +
     & coarse(ii,jj,n)
      enddo
      enddo
      enddo
      chflops=chflops+chiterations
      chloads=chloads+chiterations
      chstores=chstores+chiterations
      return
      end
      subroutine PROLONG_2(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,coarse
     & ,icoarselo0,icoarselo1
     & ,icoarsehi0,icoarsehi1
     & ,ncoarsecomp
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,m
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer nphicomp
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1,
     & 0:nphicomp-1)
      integer ncoarsecomp
      integer icoarselo0,icoarselo1
      integer icoarsehi0,icoarsehi1
      REAL*8 coarse(
     & icoarselo0:icoarsehi0,
     & icoarselo1:icoarsehi1,
     & 0:ncoarsecomp-1)
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      integer m
      INTEGER ncomp, n
      integer i,j
      integer offs(2)
      integer ic,jc
      REAL*8 f0, den, fx(2)
      den = (1.0d0)/(4**2)
      fx(1) = (3.0d0)*den
      fx(2) = (3.0d0)**2*den
      f0 = (1.0d0)*den
      ncomp = nphicomp
      chiterations=(1+iregionhi1-iregionlo1)*(1+iregionhi0-iregionlo0)*1
      do j = iregionlo1,iregionhi1
      do i = iregionlo0,iregionhi0
        ic = i/m
        jc = j/m
        offs(1) = 2*mod(i,2) - 1
        offs(2) = 2*mod(j,2) - 1
        do n = 0, ncomp-1
          phi(i,j,n) = phi(i,j,n)
     $ + fx(2)*
     $ coarse(ic,jc,n)
     $ + f0*coarse(ic+offs(1),jc+offs(2),n)
          phi(i,j,n) = phi(i,j,n)
     $ + fx(2 -1)*
     $ (
     $ coarse(ic+offs(1),jc,n)
     $ + coarse(ic,jc+offs(2),n) )
# 768 "p/2d.Linux.64.g++.gfortran.DEBUG/AMRPoissonOpF.C"
        enddo
      enddo
      enddo
      chflops=chflops+chiterations*2
      chloads=chloads+chiterations
      chstores=chstores+chiterations
      return
      end
      subroutine NEWGETFLUX(
     & flux
     & ,ifluxlo0,ifluxlo1
     & ,ifluxhi0,ifluxhi1
     & ,nfluxcomp
     & ,phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,iboxlo0,iboxlo1
     & ,iboxhi0,iboxhi1
     & ,beta_dx
     & ,a_idir
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer CHF_ID(0:5,0:5)
      data CHF_ID/ 1,0,0,0,0,0 ,0,1,0,0,0,0 ,0,0,1,0,0,0 ,0,0,0,1,0,0 ,0
     &,0,0,0,1,0 ,0,0,0,0,0,1 /
      integer nfluxcomp
      integer ifluxlo0,ifluxlo1
      integer ifluxhi0,ifluxhi1
      REAL*8 flux(
     & ifluxlo0:ifluxhi0,
     & ifluxlo1:ifluxhi1,
     & 0:nfluxcomp-1)
      integer nphicomp
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1,
     & 0:nphicomp-1)
      integer iboxlo0,iboxlo1
      integer iboxhi0,iboxhi1
      REAL*8 beta_dx
      integer a_idir
      INTEGER ncomp,n
      integer ii, jj
      integer i , j
      ncomp = nphicomp
      ii = CHF_ID(a_idir, 0)
      jj = CHF_ID(a_idir, 1)
      do n = 0, ncomp-1
      chiterations=(1+iboxhi1-iboxlo1)*(1+iboxhi0-iboxlo0)*1
      do j = iboxlo1,iboxhi1
      do i = iboxlo0,iboxhi0
          flux(i,j,n) =
     & (phi(i,j,n)-
     & phi(i-ii,j-jj,n))*beta_dx
      enddo
      enddo
      enddo
      chloads=chloads+chiterations*2*8
      chstores=chstores+chiterations*8
      chflops=chflops+chiterations*2
      return
      end

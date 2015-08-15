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
!      chloads=chloads+chiterations*2*8
!      chstores=chstores+chiterations*8
!      chflops=chflops+chiterations*(3*2 +4)
      return
      end

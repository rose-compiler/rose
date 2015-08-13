      subroutine INTERPCENTRALSLOPE(
     & slope
     & ,islopelo0,islopelo1
     & ,islopehi0,islopehi1
     & ,nslopecomp
     & ,state
     & ,istatelo0,istatelo1
     & ,istatehi0,istatehi1
     & ,nstatecomp
     & ,iblo0,iblo1
     & ,ibhi0,ibhi1
     & ,dir
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
      integer nslopecomp
      integer islopelo0,islopelo1
      integer islopehi0,islopehi1
      REAL*8 slope(
     & islopelo0:islopehi0,
     & islopelo1:islopehi1,
     & 0:nslopecomp-1)
      integer nstatecomp
      integer istatelo0,istatelo1
      integer istatehi0,istatehi1
      REAL*8 state(
     & istatelo0:istatehi0,
     & istatelo1:istatehi1,
     & 0:nstatecomp-1)
      integer iblo0,iblo1
      integer ibhi0,ibhi1
      integer dir
      integer i0,i1
      integer ii0,ii1
      integer var
      ii0=CHF_ID(0, dir)
      ii1=CHF_ID(1, dir)
      do var = 0, nstatecomp - 1
      chiterations=(1+ibhi1-iblo1)*(1+ibhi0-iblo0)*1
      do i1 = iblo1,ibhi1
      do i0 = iblo0,ibhi0
          slope (i0,i1,var) = (0.500d0) * (
     & state (i0+ii0,i1+ii1,var) -
     & state (i0-ii0,i1-ii1,var) )
      enddo
      enddo
       end do
       chflops=chflops+2*chiterations
       chloads=chloads+chiterations
       chstores=chstores+chiterations
      return
      end


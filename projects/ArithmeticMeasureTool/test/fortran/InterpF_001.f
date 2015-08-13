      subroutine INTERPCONSTANT(
     & fine
     & ,ifinelo0,ifinelo1
     & ,ifinehi0,ifinehi1
     & ,nfinecomp
     & ,coarse
     & ,icoarselo0,icoarselo1
     & ,icoarsehi0,icoarsehi1
     & ,ncoarsecomp
     & ,iblo0,iblo1
     & ,ibhi0,ibhi1
     & ,ref_ratio
     & ,ibreflo0,ibreflo1
     & ,ibrefhi0,ibrefhi1
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer nfinecomp
      integer ifinelo0,ifinelo1
      integer ifinehi0,ifinehi1
      REAL*8 fine(
     & ifinelo0:ifinehi0,
     & ifinelo1:ifinehi1,
     & 0:nfinecomp-1)
      integer ncoarsecomp
      integer icoarselo0,icoarselo1
      integer icoarsehi0,icoarsehi1
      REAL*8 coarse(
     & icoarselo0:icoarsehi0,
     & icoarselo1:icoarsehi1,
     & 0:ncoarsecomp-1)
      integer iblo0,iblo1
      integer ibhi0,ibhi1
      integer ref_ratio
      integer ibreflo0,ibreflo1
      integer ibrefhi0,ibrefhi1
      integer var
      integer ic0,ic1
      integer if0,if1
      integer ii0,ii1
      do var = 0, ncoarsecomp - 1
      chiterations=(1+ibhi1-iblo1)*(1+ibhi0-iblo0)*1
      do ic1 = iblo1,ibhi1
      do ic0 = iblo0,ibhi0
      chiterations=(1+ibrefhi1-ibreflo1)*(1+ibrefhi0-ibreflo0)*1
      do ii1 = ibreflo1,ibrefhi1
      do ii0 = ibreflo0,ibrefhi0
               if0 = ic0*ref_ratio + ii0
               if1 = ic1*ref_ratio + ii1
               fine(if0,if1,var) = coarse(ic0,ic1,var)
      enddo
      enddo
      enddo
      enddo
      end do
!      chloads=chloads+chiterations+chiterations/(ref_ratio*2)
!      chstores=chstores+chiterations
      return
      end

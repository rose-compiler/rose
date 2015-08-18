      SUBROUTINE AVERAGECODIM(coarse,icoarselo0,icoarselo1,icoarsehi0,icoarsehi1,ncoarsecomp,fine,&
     &ifinelo0,ifinelo1,ifinehi0,ifinehi1,nfinecomp,iboxlo0,iboxlo1,iboxhi0,iboxhi1,refRatio,&
     &refFactor,refDim,ibreflo0,ibreflo1,ibrefhi0,ibrefhi1)
      IMPLICIT NONE
      INTEGER(kind=8) :: chiterations
      INTEGER(kind=8) :: chflops, chloads, chstores
      COMMON / chiterations / chiterations
      COMMON / chflops / chflops,chloads,chstores
      external :: CHCOMMON
      INTEGER :: ncoarsecomp
      INTEGER :: icoarselo0, icoarselo1
      INTEGER :: icoarsehi0, icoarsehi1
      REAL(kind=8), DIMENSION(icoarselo0:icoarsehi0,icoarselo1:icoarsehi1,0:(ncoarsecomp - 1)) :: coarse
      INTEGER :: nfinecomp
      INTEGER :: ifinelo0, ifinelo1
      INTEGER :: ifinehi0, ifinehi1
      REAL(kind=8), DIMENSION(ifinelo0:ifinehi0,ifinelo1:ifinehi1,0:(nfinecomp - 1)) :: fine
      INTEGER :: iboxlo0, iboxlo1
      INTEGER :: iboxhi0, iboxhi1
      INTEGER :: refRatio
      INTEGER :: refFactor
      INTEGER :: refDim
      INTEGER :: ibreflo0, ibreflo1
      INTEGER :: ibrefhi0, ibrefhi1
      INTEGER :: var
      INTEGER :: ic0, ic1
      INTEGER :: ip0, ip1
      INTEGER :: ii0, ii1
      REAL(kind=8) :: refScale, coarseSum
      refScale = (1.0d0) / (refFactor ** refDim)
      DO var = 0, ncoarsecomp - 1
      chiterations = (1 + iboxhi1 - iboxlo1) * (1 + iboxhi0 - iboxlo0) * 1
      DO ic1 = iboxlo1, iboxhi1
      DO ic0 = iboxlo0, iboxhi0
      ip0 = ic0 * refRatio
      ip1 = ic1 * refRatio
      coarseSum = (0.0d0)
      chiterations = (1 + ibrefhi1 - ibreflo1) * (1 + ibrefhi0 - ibreflo0) * 1
      DO ii1 = ibreflo1, ibrefhi1
      DO ii0 = ibreflo0, ibrefhi0
      coarseSum = coarseSum + fine(ip0 + ii0,ip1 + ii1,var)
      END DO
      END DO
      coarse(ic0,ic1,var) = coarseSum * refScale
      END DO
      END DO
      END DO
      RETURN
      END SUBROUTINE 

      SUBROUTINE AVERAGECODIMHARMONIC(coarse,icoarselo0,icoarselo1,icoarsehi0,icoarsehi1,ncoarsecomp,fine,&
     &ifinelo0,ifinelo1,ifinehi0,ifinehi1,nfinecomp,iboxlo0,iboxlo1,iboxhi0,iboxhi1,refRatio,refFactor,&
     &refDim,ibreflo0,ibreflo1,ibrefhi0,ibrefhi1)
      IMPLICIT NONE
      INTEGER(kind=8) :: chiterations
      INTEGER(kind=8) :: chflops, chloads, chstores
      COMMON / chiterations / chiterations
      COMMON / chflops / chflops,chloads,chstores
      external :: CHCOMMON
      INTEGER :: ncoarsecomp
      INTEGER :: icoarselo0, icoarselo1
      INTEGER :: icoarsehi0, icoarsehi1
      REAL(kind=8), DIMENSION(icoarselo0:icoarsehi0,icoarselo1:icoarsehi1,0:(ncoarsecomp - 1)) :: coarse
      INTEGER :: nfinecomp
      INTEGER :: ifinelo0, ifinelo1
      INTEGER :: ifinehi0, ifinehi1
      REAL(kind=8), DIMENSION(ifinelo0:ifinehi0,ifinelo1:ifinehi1,0:(nfinecomp - 1)) :: fine
      INTEGER :: iboxlo0, iboxlo1
      INTEGER :: iboxhi0, iboxhi1
      INTEGER :: refRatio
      INTEGER :: refFactor
      INTEGER :: refDim
      INTEGER :: ibreflo0, ibreflo1
      INTEGER :: ibrefhi0, ibrefhi1
      INTEGER :: var
      INTEGER :: ic0, ic1
      INTEGER :: ip0, ip1
      INTEGER :: ii0, ii1
      REAL(kind=8) :: refScale, coarseSum
      refScale = (1.0d0) / (refFactor ** refDim)
      DO var = 0, ncoarsecomp - 1
      chiterations = (1 + iboxhi1 - iboxlo1) * (1 + iboxhi0 - iboxlo0) * 1
      DO ic1 = iboxlo1, iboxhi1
      DO ic0 = iboxlo0, iboxhi0
      ip0 = ic0 * refRatio
      ip1 = ic1 * refRatio
      coarseSum = (0.0d0)
      chiterations = (1 + ibrefhi1 - ibreflo1) * (1 + ibrefhi0 - ibreflo0) * 1
      DO ii1 = ibreflo1, ibrefhi1
      DO ii0 = ibreflo0, ibrefhi0
      coarseSum = coarseSum + (1.0d0) / fine(ip0 + ii0,ip1 + ii1,var)
      END DO
      END DO
      coarse(ic0,ic1,var) = (1.0d0) / (coarseSum * refScale)
      END DO
      END DO
      END DO
      RETURN
      END SUBROUTINE 


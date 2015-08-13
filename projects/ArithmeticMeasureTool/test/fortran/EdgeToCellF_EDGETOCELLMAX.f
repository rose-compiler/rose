      subroutine EDGETOCELLMAX(
     & edgeData
     & ,iedgeDatalo0,iedgeDatalo1
     & ,iedgeDatahi0,iedgeDatahi1
     & ,cellData
     & ,icellDatalo0,icellDatalo1
     & ,icellDatahi0,icellDatahi1
     & ,icellBoxlo0,icellBoxlo1
     & ,icellBoxhi0,icellBoxhi1
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
      integer iedgeDatalo0,iedgeDatalo1
      integer iedgeDatahi0,iedgeDatahi1
      REAL*8 edgeData(
     & iedgeDatalo0:iedgeDatahi0,
     & iedgeDatalo1:iedgeDatahi1)
      integer icellDatalo0,icellDatalo1
      integer icellDatahi0,icellDatahi1
      REAL*8 cellData(
     & icellDatalo0:icellDatahi0,
     & icellDatalo1:icellDatahi1)
      integer icellBoxlo0,icellBoxlo1
      integer icellBoxhi0,icellBoxhi1
      integer dir
      integer i,j
      integer ii,jj
      chiterations=(1+icellBoxhi1-icellBoxlo1)*(1+icellBoxhi0-icellBoxlo
     &0)*1
      do j = icellBoxlo1,icellBoxhi1
      do i = icellBoxlo0,icellBoxhi0
      ii = i+CHF_ID(0,dir)
      jj = j+CHF_ID(1,dir)
      cellData(i,j) = max(
     & edgeData(i,j),
     & edgeData(ii,jj))
      enddo
      enddo
      chflops=chflops+chiterations
      chloads=chloads+chiterations*2
      chstores=chstores+chiterations
      return
      end


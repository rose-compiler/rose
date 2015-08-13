# 1 "p/2d.Linux.64.g++.gfortran.DEBUG/EdgeToCellF.C"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "p/2d.Linux.64.g++.gfortran.DEBUG/EdgeToCellF.C"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/REAL.H" 1
# 2 "p/2d.Linux.64.g++.gfortran.DEBUG/EdgeToCellF.C" 2
# 1 "./SPACE.H" 1
# 22 "./SPACE.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CH_assert.H" 1
# 18 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CH_assert.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/BaseNamespaceHeader.H" 1
# 19 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CH_assert.H" 2
# 54 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CH_assert.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/BaseNamespaceFooter.H" 1
# 55 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CH_assert.H" 2
# 23 "./SPACE.H" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/NamespaceHeader.H" 1
# 25 "./SPACE.H" 2
# 66 "./SPACE.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/NamespaceFooter.H" 1
# 67 "./SPACE.H" 2
# 3 "p/2d.Linux.64.g++.gfortran.DEBUG/EdgeToCellF.C" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CONSTANTS.H" 1
# 13 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CONSTANTS.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/BaseNamespaceHeader.H" 1
# 14 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CONSTANTS.H" 2
# 141 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CONSTANTS.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/BaseNamespaceFooter.H" 1
# 142 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CONSTANTS.H" 2
# 4 "p/2d.Linux.64.g++.gfortran.DEBUG/EdgeToCellF.C" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/BoxTools/../../src/BaseTools/CONSTANTS.H" 1
# 6 "p/2d.Linux.64.g++.gfortran.DEBUG/EdgeToCellF.C" 2
      subroutine EDGETOCELL(
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
      cellData(i,j) = (0.500d0)*(
     & edgeData(i,j)
     & +edgeData(ii,jj))
      enddo
      enddo
      chflops=chflops+chiterations*2
      chloads=chloads+chiterations*2
      chstores=chstores+chiterations
      return
      end
      subroutine EDGETOINCREMENTCELL(
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
      integer i0,i1
      integer ii0,ii1
      ii0=CHF_ID(0, dir)
      ii1=CHF_ID(1, dir)
      chiterations=(1+icellBoxhi1-icellBoxlo1)*(1+icellBoxhi0-icellBoxlo
     &0)*1
      do i1 = icellBoxlo1,icellBoxhi1
      do i0 = icellBoxlo0,icellBoxhi0
         cellData(i0,i1) = cellData(i0,i1) + (0.500d0)*(
     & edgeData(i0,i1) + edgeData(i0+ii0,i1+ii1))
      enddo
      enddo
      chflops=chflops+chiterations*3
      chloads=chloads+chiterations*2
      chstores=chstores+chiterations
      return
      end
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

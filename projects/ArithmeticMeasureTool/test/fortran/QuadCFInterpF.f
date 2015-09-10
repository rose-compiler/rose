# 1 "p/2d.Linux.64.g++.gfortran.DEBUG/QuadCFInterpF.C"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "p/2d.Linux.64.g++.gfortran.DEBUG/QuadCFInterpF.C"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/REAL.H" 1
# 2 "p/2d.Linux.64.g++.gfortran.DEBUG/QuadCFInterpF.C" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BoxTools/SPACE.H" 1
# 22 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BoxTools/SPACE.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CH_assert.H" 1
# 18 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CH_assert.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/BaseNamespaceHeader.H" 1
# 19 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CH_assert.H" 2
# 54 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CH_assert.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/BaseNamespaceFooter.H" 1
# 55 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CH_assert.H" 2
# 23 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BoxTools/SPACE.H" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/NamespaceHeader.H" 1
# 25 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BoxTools/SPACE.H" 2
# 66 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BoxTools/SPACE.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/NamespaceFooter.H" 1
# 67 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BoxTools/SPACE.H" 2
# 3 "p/2d.Linux.64.g++.gfortran.DEBUG/QuadCFInterpF.C" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 1
# 13 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/BaseNamespaceHeader.H" 1
# 14 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 2
# 141 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/BaseNamespaceFooter.H" 1
# 142 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 2
# 4 "p/2d.Linux.64.g++.gfortran.DEBUG/QuadCFInterpF.C" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 1
# 6 "p/2d.Linux.64.g++.gfortran.DEBUG/QuadCFInterpF.C" 2
      subroutine QUADINTERP(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,phistar
     & ,iphistarlo0,iphistarlo1
     & ,iphistarhi0,iphistarhi1
     & ,nphistarcomp
     & ,iboxlo0,iboxlo1
     & ,iboxhi0,iboxhi1
     & ,ihilo
     & ,h
     & ,idir
     & ,scomp
     & ,ecomp
     & ,nref
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
      integer nphicomp
      integer iphilo0,iphilo1
      integer iphihi0,iphihi1
      REAL*8 phi(
     & iphilo0:iphihi0,
     & iphilo1:iphihi1,
     & 0:nphicomp-1)
      integer nphistarcomp
      integer iphistarlo0,iphistarlo1
      integer iphistarhi0,iphistarhi1
      REAL*8 phistar(
     & iphistarlo0:iphistarhi0,
     & iphistarlo1:iphistarhi1,
     & 0:nphistarcomp-1)
      integer iboxlo0,iboxlo1
      integer iboxhi0,iboxhi1
      integer ihilo
      REAL*8 h
      integer idir
      integer scomp
      integer ecomp
      integer nref
      integer i0,i1
      integer ii0,ii1
      integer n
      REAL*8 x, pa, pb, ps, a, b, frac, denom, xsquared
      REAL*8 mult, invh
      frac = (2.0d0)/(h*h)
      denom = nref*nref + 4*nref + 3
      mult = frac/denom
      invh = (1.0d0) / h
      x = (2.0d0) * h
      xsquared = (4.0d0) * h*h
      ii0= ihilo*CHF_ID(0, idir)
      ii1= ihilo*CHF_ID(1, idir)
      do n=scomp,ecomp
      chiterations=(1+iboxhi1-iboxlo1)*(1+iboxhi0-iboxlo0)*1
      do i1 = iboxlo1,iboxhi1
      do i0 = iboxlo0,iboxhi0
            pa = phi(i0 -2*ii0,i1 -2*ii1,n)
            pb = phi(i0 -ii0,i1 -ii1,n)
            ps = phistar(i0 +ii0,i1 +ii1,n)
            a = mult*((2.0d0)*ps + (nref+1)*pa - (nref+3)*pb)
            b = (pb-pa)*invh - a*h
            phi(i0,i1,n) = xsquared*a + b*x + pa
      enddo
      enddo
      enddo
      chflops=chflops+chiterations*13
      chloads=chloads+chiterations*2*8
      chstores=chstores+chiterations*8
      return
      end
      subroutine PHISTAR(
     & fPhiStar
     & ,ifPhiStarlo0,ifPhiStarlo1
     & ,ifPhiStarhi0,ifPhiStarhi1
     & ,nfPhiStarcomp
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,phic
     & ,iphiclo0,iphiclo1
     & ,iphichi0,iphichi1
     & ,nphiccomp
     & ,coarslope
     & ,icoarslopelo0,icoarslopelo1
     & ,icoarslopehi0,icoarslopehi1
     & ,ncoarslopecomp
     & ,coarcurva
     & ,icoarcurvalo0,icoarcurvalo1
     & ,icoarcurvahi0,icoarcurvahi1
     & ,ncoarcurvacomp
     & ,coarmixed
     & ,icoarmixedlo0,icoarmixedlo1
     & ,icoarmixedhi0,icoarmixedhi1
     & ,ncoarmixedcomp
     & ,dxf
     & ,ivar
     & ,dir
     & ,sign
     & ,nRef
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
      integer nfPhiStarcomp
      integer ifPhiStarlo0,ifPhiStarlo1
      integer ifPhiStarhi0,ifPhiStarhi1
      REAL*8 fPhiStar(
     & ifPhiStarlo0:ifPhiStarhi0,
     & ifPhiStarlo1:ifPhiStarhi1,
     & 0:nfPhiStarcomp-1)
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      integer nphiccomp
      integer iphiclo0,iphiclo1
      integer iphichi0,iphichi1
      REAL*8 phic(
     & iphiclo0:iphichi0,
     & iphiclo1:iphichi1,
     & 0:nphiccomp-1)
      integer ncoarslopecomp
      integer icoarslopelo0,icoarslopelo1
      integer icoarslopehi0,icoarslopehi1
      REAL*8 coarslope(
     & icoarslopelo0:icoarslopehi0,
     & icoarslopelo1:icoarslopehi1,
     & 0:ncoarslopecomp-1)
      integer ncoarcurvacomp
      integer icoarcurvalo0,icoarcurvalo1
      integer icoarcurvahi0,icoarcurvahi1
      REAL*8 coarcurva(
     & icoarcurvalo0:icoarcurvahi0,
     & icoarcurvalo1:icoarcurvahi1,
     & 0:ncoarcurvacomp-1)
      integer ncoarmixedcomp
      integer icoarmixedlo0,icoarmixedlo1
      integer icoarmixedhi0,icoarmixedhi1
      REAL*8 coarmixed(
     & icoarmixedlo0:icoarmixedhi0,
     & icoarmixedlo1:icoarmixedhi1,
     & 0:ncoarmixedcomp-1)
      REAL*8 dxf
      integer ivar
      integer dir
      integer sign
      integer nRef
      REAL*8 xf1, xc1, xf2, xc2, x1, x2, dxc
      REAL*8 aa, update1, update2, update3
      integer i0,i1
      integer ii0,ii1
      integer ir0,ir1
      integer ic(0:2 -1)
      integer ivf(0:2 -1)
      integer YOU(1:2, 0:2), you1, you2
      data YOU / 1, 2, 0, 2, 0, 1 /
      dxc = nRef * dxf
      you1 = YOU(1,dir)
      you2 = YOU(2,dir)
      ii0= sign*CHF_ID(0, dir)
      ii1= sign*CHF_ID(1, dir)
      chiterations=(1+iregionhi1-iregionlo1)*(1+iregionhi0-iregionlo0)*1
      do ir1 = iregionlo1,iregionhi1
      do ir0 = iregionlo0,iregionhi0
         ic(0)=ir0/nRef
         ic(1)=ir1/nRef
         ivf(0)=ir0
         ivf(1)=ir1
         i0=ir0+ii0
         i1=ir1+ii1
         xf1 = (ivf(you1)+(0.500d0))*dxf
         xc1 = ( ic(you1)+(0.500d0))*dxc
         xf2 = (ivf(you2)+(0.500d0))*dxf
         xc2 = ( ic(you2)+(0.500d0))*dxc
         x1 = xf1-xc1
         x2 = xf2-xc2
         aa= phic(ic(0),ic(1),ivar)
         update1=x1*coarslope(ic(0),ic(1),you1) +
     & (0.500d0)*x1*x1*coarcurva(ic(0),ic(1),you1)
         update2=x2*coarslope(ic(0),ic(1),you2) +
     & (0.500d0)*x2*x2*coarcurva(ic(0),ic(1),you2)
         update3=x1*x2*coarmixed(ic(0),ic(1),0)
         fPhiStar(i0,i1,ivar) = aa+update1+update2+update3
      enddo
      enddo
      chflops=chflops+chiterations*(3*2 +16)
      chloads=chloads+chiterations*2*8
      chstores=chstores+chiterations*8
      return
      end

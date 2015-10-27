# 1 "p/2d.Linux.64.g++.gfortran.DEBUG/InterpF.C"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "p/2d.Linux.64.g++.gfortran.DEBUG/InterpF.C"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/REAL.H" 1
# 2 "p/2d.Linux.64.g++.gfortran.DEBUG/InterpF.C" 2
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
# 3 "p/2d.Linux.64.g++.gfortran.DEBUG/InterpF.C" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 1
# 13 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/BaseNamespaceHeader.H" 1
# 14 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 2
# 141 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/BaseNamespaceFooter.H" 1
# 142 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 2
# 4 "p/2d.Linux.64.g++.gfortran.DEBUG/InterpF.C" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 1
# 6 "p/2d.Linux.64.g++.gfortran.DEBUG/InterpF.C" 2
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
      chloads=chloads+chiterations+chiterations/(ref_ratio*2)
      chstores=chstores+chiterations
      return
      end
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
      subroutine INTERPHISIDESLOPE(
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
          slope (i0,i1,var) =
     & state ( i0+ii0,i1+ii1, var)
     & - state ( i0,i1, var)
      enddo
      enddo
       enddo
       chflops=chflops+chiterations
       chloads=chloads+chiterations
       chstores=chstores+chiterations
      return
      end
      subroutine INTERPLOSIDESLOPE(
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
      integer i0,i1, var
      integer ii0,ii1
      ii0=CHF_ID(0, dir)
      ii1=CHF_ID(1, dir)
      do var = 0, nstatecomp - 1
      chiterations=(1+ibhi1-iblo1)*(1+ibhi0-iblo0)*1
      do i1 = iblo1,ibhi1
      do i0 = iblo0,ibhi0
         slope (i0,i1,var) =
     & state ( i 0, i 1, var) -
     & state ( i0-ii0, i1-ii1, var)
      enddo
      enddo
       end do
       chflops=chflops+chiterations
       chloads=chloads+chiterations
       chstores=chstores+chiterations
      return
      end
      subroutine INTERPLIMIT(
     & islope
     & ,iislopelo0,iislopelo1
     & ,iislopehi0,iislopehi1
     & ,nislopecomp
     & ,jslope
     & ,ijslopelo0,ijslopelo1
     & ,ijslopehi0,ijslopehi1
     & ,njslopecomp
     & ,kslope
     & ,ikslopelo0,ikslopelo1
     & ,ikslopehi0,ikslopehi1
     & ,nkslopecomp
     & ,state
     & ,istatelo0,istatelo1
     & ,istatehi0,istatehi1
     & ,nstatecomp
     & ,ibcoarselo0,ibcoarselo1
     & ,ibcoarsehi0,ibcoarsehi1
     & ,ibnlo0,ibnlo1
     & ,ibnhi0,ibnhi1
     & ,iphysdomainlo0,iphysdomainlo1
     & ,iphysdomainhi0,iphysdomainhi1
     & )
      implicit none
      integer*8 chiterations
      integer*8 chflops,chloads,chstores
      common /chiterations/ chiterations
      common /chflops/ chflops,chloads,chstores
      EXTERNAL CHCOMMON
      integer nislopecomp
      integer iislopelo0,iislopelo1
      integer iislopehi0,iislopehi1
      REAL*8 islope(
     & iislopelo0:iislopehi0,
     & iislopelo1:iislopehi1,
     & 0:nislopecomp-1)
      integer njslopecomp
      integer ijslopelo0,ijslopelo1
      integer ijslopehi0,ijslopehi1
      REAL*8 jslope(
     & ijslopelo0:ijslopehi0,
     & ijslopelo1:ijslopehi1,
     & 0:njslopecomp-1)
      integer nkslopecomp
      integer ikslopelo0,ikslopelo1
      integer ikslopehi0,ikslopehi1
      REAL*8 kslope(
     & ikslopelo0:ikslopehi0,
     & ikslopelo1:ikslopehi1,
     & 0:nkslopecomp-1)
      integer nstatecomp
      integer istatelo0,istatelo1
      integer istatehi0,istatehi1
      REAL*8 state(
     & istatelo0:istatehi0,
     & istatelo1:istatehi1,
     & 0:nstatecomp-1)
      integer ibcoarselo0,ibcoarselo1
      integer ibcoarsehi0,ibcoarsehi1
      integer ibnlo0,ibnlo1
      integer ibnhi0,ibnhi1
      integer iphysdomainlo0,iphysdomainlo1
      integer iphysdomainhi0,iphysdomainhi1
      integer i0, i1, var
      integer ii0, ii1
      integer in0, in1
      REAL*8 statemax, statemin, deltasum, eta
      do var = 0, nislopecomp - 1
      chiterations=(1+ibcoarsehi1-ibcoarselo1)*(1+ibcoarsehi0-ibcoarselo
     &0)*1
      do i1 = ibcoarselo1,ibcoarsehi1
      do i0 = ibcoarselo0,ibcoarsehi0
             statemax = state ( i0,i1, var )
             statemin = state ( i0,i1, var )
      chiterations=(1+ibnhi1-ibnlo1)*(1+ibnhi0-ibnlo0)*1
      do ii1 = ibnlo1,ibnhi1
      do ii0 = ibnlo0,ibnhi0
                 in0 = i0 + ii0
                 in1 = i1 + ii1
                 if (
     & in0 .ge. istatelo0 .and.
     & in0 .le. istatehi0
     & .and.
     & in1 .ge. istatelo1 .and.
     & in1 .le. istatehi1
     & ) then
                    statemax = max ( statemax, state(in0,in1,var))
                    statemin = min ( statemin, state(in0,in1,var))
                 endif
      enddo
      enddo
             deltasum = (0.500d0) * (
     & abs ( islope ( i0,i1, var ) )
     & +
     & abs ( jslope ( i0,i1, var ) )
     & )
              eta = min(statemax - state(i0,i1,var),
     & state(i0,i1,var) - statemin)
              if( eta .le. 1.e-9*abs(statemax) ) then
                 eta = (0.0d0)
              else
              if (deltasum .gt. eta) then
                eta = eta/deltasum
              else
                eta = (1.0d0)
              endif
              endif
              islope ( i0,i1, var ) =
     & eta * islope ( i0,i1, var )
              jslope ( i0,i1, var ) =
     & eta * jslope ( i0,i1, var )
              chflops=chflops+2
              chloads=chloads+2
              chstores=chstores+2
      enddo
      enddo
      end do
      return
      end
      subroutine INTERPLINEAR(
     & fine
     & ,ifinelo0,ifinelo1
     & ,ifinehi0,ifinehi1
     & ,nfinecomp
     & ,slope
     & ,islopelo0,islopelo1
     & ,islopehi0,islopehi1
     & ,nslopecomp
     & ,iblo0,iblo1
     & ,ibhi0,ibhi1
     & ,dir
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
      integer nslopecomp
      integer islopelo0,islopelo1
      integer islopehi0,islopehi1
      REAL*8 slope(
     & islopelo0:islopehi0,
     & islopelo1:islopehi1,
     & 0:nslopecomp-1)
      integer iblo0,iblo1
      integer ibhi0,ibhi1
      integer dir
      integer ref_ratio
      integer ibreflo0,ibreflo1
      integer ibrefhi0,ibrefhi1
      integer ic0,ic1
      integer if0,if1
      integer ii0,ii1
      integer var, id
      REAL*8 dxf
      do var = 0, nfinecomp - 1
      chiterations=(1+ibhi1-iblo1)*(1+ibhi0-iblo0)*1
      do ic1 = iblo1,ibhi1
      do ic0 = iblo0,ibhi0
      chiterations=(1+ibrefhi1-ibreflo1)*(1+ibrefhi0-ibreflo0)*1
      do ii1 = ibreflo1,ibrefhi1
      do ii0 = ibreflo0,ibrefhi0
                  if0 = ic0*ref_ratio + ii0
                  if1 = ic1*ref_ratio + ii1
                  if (dir .eq. 0) then
                      id = ii0
                  else if (dir .eq. 1) then
                      id = ii1
                  endif
                  dxf = -(0.500d0) + ( (id+(0.500d0)) / ref_ratio )
                  fine( if0,if1,var) =
     & fine( if0,if1,var) +
     & dxf * slope ( ic 0, ic 1, var )
      enddo
      enddo
      enddo
      enddo
      end do
      chflops=chflops+chiterations*(2 +5)
      chloads=chloads+chiterations*2
      chstores=chstores+chiterations
      return
      end
      subroutine INTERPHOMO_OLD(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,x1
     & ,dxCrse
     & ,idir
     & ,ihilo
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
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      REAL*8 x1
      REAL*8 dxCrse
      integer idir
      integer ihilo
      REAL*8 x2, denom, idenom, x, xsquared, m1, m2
      REAL*8 q1, q2
      REAL*8 pa, pb, a, b
      INTEGER ncomp, n
      INTEGER ii0,ii1
      INTEGER i0,i1
      x2 = (0.500d0)*((3.0d0)*x1+dxCrse)
      denom = (1.0d0)-((x1+x2)/x1)
      idenom = (1.0d0)/(denom)
      x = (2.0d0)*x1
      xsquared = x*x
      m1 = (1.0d0)/(x1*x1)
      m2 = (1.0d0)/(x1*(x1-x2))
      q1 = (1.0d0)/(x1-x2)
      q2 = x1+x2
      ihilo = ihilo*(-1)
      ncomp = nphicomp
      ii0= ihilo*CHF_ID(0, idir)
      ii1= ihilo*CHF_ID(1, idir)
      do n = 0, ncomp-1
      chiterations=(1+iregionhi1-iregionlo1)*(1+iregionhi0-iregionlo0)*1
      do i1 = iregionlo1,iregionhi1
      do i0 = iregionlo0,iregionhi0
          pa=phi(i0+2*ii0,i1+2*ii1,n)
          pb=phi(i0+ii0,i1+ii1,n)
          a=((pb-pa)*m1 - (pb)*m2)*idenom
          b=(pb)*q1 - a*q2
          phi(i0,i1,n) = a*xsquared + b*x + pa
      enddo
      enddo
      enddo
      chflops=chflops+chiterations*12
      chloads=chloads+chiterations
      chstores=chstores+chiterations
      return
      end
      subroutine INTERPHOMO(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,x1
     & ,dxCrse
     & ,idir
     & ,ihilo
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
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      REAL*8 x1
      REAL*8 dxCrse
      integer idir
      integer ihilo
      REAL*8 c1, c2
      REAL*8 pa, pb
      INTEGER ncomp, n
      INTEGER ii0,ii1
      INTEGER i0,i1
      c1=(2.0d0)*(dxCrse-x1)/(dxCrse+x1)
      c2= -(dxCrse-x1)/(dxCrse+(3.0d0)*x1)
      ihilo = ihilo*(-1)
      ncomp = nphicomp
      ii0= ihilo*CHF_ID(0, idir)
      ii1= ihilo*CHF_ID(1, idir)
      do n = 0, ncomp-1
      chiterations=(1+iregionhi1-iregionlo1)*(1+iregionhi0-iregionlo0)*1
      do i1 = iregionlo1,iregionhi1
      do i0 = iregionlo0,iregionhi0
          pa=phi(i0+ii0,i1+ii1,n)
          pb=phi(i0+2*ii0,i1+2*ii1,n)
          phi(i0,i1,n) = c1*pa + c2*pb
      enddo
      enddo
      enddo
      chflops=chflops+3*chiterations
      chloads=chloads+chiterations
      chstores=chstores+chiterations
      return
      end
      subroutine INTERPHOMOLINEAR(
     & phi
     & ,iphilo0,iphilo1
     & ,iphihi0,iphihi1
     & ,nphicomp
     & ,iregionlo0,iregionlo1
     & ,iregionhi0,iregionhi1
     & ,x1
     & ,dxCrse
     & ,idir
     & ,ihilo
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
      integer iregionlo0,iregionlo1
      integer iregionhi0,iregionhi1
      REAL*8 x1
      REAL*8 dxCrse
      integer idir
      integer ihilo
      INTEGER ncomp, n
      INTEGER ii0,ii1
      INTEGER i0,i1
      REAL*8 pa, factor
      ihilo = ihilo*(-1)
      ncomp = nphicomp
      ii0= ihilo*CHF_ID(0, idir)
      ii1= ihilo*CHF_ID(1, idir)
      factor = (1.0d0) - (2.0d0)*x1/(x1+dxCrse)
      do n = 0, ncomp-1
      chiterations=(1+iregionhi1-iregionlo1)*(1+iregionhi0-iregionlo0)*1
      do i1 = iregionlo1,iregionhi1
      do i0 = iregionlo0,iregionhi0
          pa=phi(i0+ii0,i1+ii1,n)
          phi(i0,i1,n) = factor*pa
      enddo
      enddo
      enddo
      chflops=chflops+chiterations
      chloads=chloads+chiterations
      chstores=chstores+chiterations
      return
      end

# 1 "p/2d.Linux.64.identityTranslator.identityTranslator.DEBUG/InterpFace.C"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "p/2d.Linux.64.identityTranslator.identityTranslator.DEBUG/InterpFace.C"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/REAL.H" 1
# 2 "p/2d.Linux.64.identityTranslator.identityTranslator.DEBUG/InterpFace.C" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BoxTools/SPACE.H" 1
# 22 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BoxTools/SPACE.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CH_assert.H" 1
# 18 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CH_assert.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/BaseNamespaceHeader.H" 1
# 19 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CH_assert.H" 2
# 54 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CH_assert.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/BaseNamespaceFooter.H" 1
# 55 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CH_assert.H" 2
# 23 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BoxTools/SPACE.H" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/NamespaceHeader.H" 1
# 25 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BoxTools/SPACE.H" 2
# 66 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BoxTools/SPACE.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/NamespaceFooter.H" 1
# 67 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BoxTools/SPACE.H" 2
# 3 "p/2d.Linux.64.identityTranslator.identityTranslator.DEBUG/InterpFace.C" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 1
# 13 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/BaseNamespaceHeader.H" 1
# 14 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 2
# 141 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H"
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/BaseNamespaceFooter.H" 1
# 142 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 2
# 4 "p/2d.Linux.64.identityTranslator.identityTranslator.DEBUG/InterpFace.C" 2
# 1 "/global/project/projectdirs/rosecompiler/rose-project-workspace/ExReDi-rose/lib/src/AMRTools/../../src/BaseTools/CONSTANTS.H" 1
# 6 "p/2d.Linux.64.identityTranslator.identityTranslator.DEBUG/InterpFace.C" 2
      subroutine INTERPFACECONSTANT(
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
     & ,dir
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
&ifinelo1:ifinehi1,
     & 0:nfinecomp-1)
      integer ncoarsecomp
      integer icoarselo0,icoarselo1
      integer icoarsehi0,icoarsehi1
      REAL*8 coarse(
     & icoarselo0:icoarsehi0,
&icoarselo1:icoarsehi1,
     & 0:ncoarsecomp-1)
      integer iblo0,iblo1
      integer ibhi0,ibhi1
      integer ref_ratio
      integer ibreflo0,ibreflo1
      integer ibrefhi0,ibrefhi1
      integer dir
      integer var
      integer ic0,ic1
      integer ifine0,ifine1
      integer ii0,ii1
      do var = 0, ncoarsecomp - 1
      chiterations=(1+ibhi1-iblo1)*(1+ibhi0-iblo0)*1
      do ic1 = iblo1,ibhi1
      do ic0 = iblo0,ibhi0
      chiterations=(1+ibrefhi1-ibreflo1)*(1+ibrefhi0-ibreflo0)*1
      do ii1 = ibreflo1,ibrefhi1
      do ii0 = ibreflo0,ibrefhi0
               ifine0 = ic0*ref_ratio + ii0
               ifine1 = ic1*ref_ratio + ii1
               fine(ifine0,ifine1,var) = coarse(ic0,ic1,var)
      enddo
      enddo
      enddo
      enddo
      end do
      return
      end
      subroutine INTERPLINEARFACE(
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
     & ,ibreffacelo0,ibreffacelo1
     & ,ibreffacehi0,ibreffacehi1
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
&ifinelo1:ifinehi1,
     & 0:nfinecomp-1)
      integer nslopecomp
      integer islopelo0,islopelo1
      integer islopehi0,islopehi1
      REAL*8 slope(
     & islopelo0:islopehi0,
&islopelo1:islopehi1,
     & 0:nslopecomp-1)
      integer iblo0,iblo1
      integer ibhi0,ibhi1
      integer dir
      integer ref_ratio
      integer ibreffacelo0,ibreffacelo1
      integer ibreffacehi0,ibreffacehi1
      integer ic0,ic1
      integer ifine0,ifine1
      integer ii0,ii1
      integer var, id
      REAL*8 dxf
      do var = 0, nfinecomp - 1
      chiterations=(1+ibhi1-iblo1)*(1+ibhi0-iblo0)*1
      do ic1 = iblo1,ibhi1
      do ic0 = iblo0,ibhi0
      chiterations=(1+ibreffacehi1-ibreffacelo1)*(1+ibreffacehi0-ibreffa
     &celo0)*1
      do ii1 = ibreffacelo1,ibreffacehi1
      do ii0 = ibreffacelo0,ibreffacehi0
                  ifine0 = ic0*ref_ratio + ii0
                  ifine1 = ic1*ref_ratio + ii1
                  if (dir .eq. 0) then
                      id = ii0
                  else if (dir .eq. 1) then
                      id = ii1
                  endif
                  dxf = -(0.500d0) + ( (id+(0.500d0)) / ref_ratio )
                  fine( ifine0,ifine1,var) =
     & fine( ifine0,ifine1,var) +
     & dxf * slope ( ic 0, ic 1, var )
      enddo
      enddo
      enddo
      enddo
      end do
      return
      end
      subroutine INTERPLINEARINTERIORFACE(
     & fine
     & ,ifinelo0,ifinelo1
     & ,ifinehi0,ifinehi1
     & ,nfinecomp
     & ,ibcoarselo0,ibcoarselo1
     & ,ibcoarsehi0,ibcoarsehi1
     & ,ref_ratio
     & ,facedir
     & ,iinteriorrefboxlo0,iinteriorrefboxlo1
     & ,iinteriorrefboxhi0,iinteriorrefboxhi1
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
      integer nfinecomp
      integer ifinelo0,ifinelo1
      integer ifinehi0,ifinehi1
      REAL*8 fine(
     & ifinelo0:ifinehi0,
&ifinelo1:ifinehi1,
     & 0:nfinecomp-1)
      integer ibcoarselo0,ibcoarselo1
      integer ibcoarsehi0,ibcoarsehi1
      integer ref_ratio
      integer facedir
      integer iinteriorrefboxlo0,iinteriorrefboxlo1
      integer iinteriorrefboxhi0,iinteriorrefboxhi1
      integer ic0,ic1
      integer ifine0,ifine1
      integer ii0,ii1
      integer iloface0,iloface1
      integer ihiface0,ihiface1
      integer var, id
      REAL*8 dxf, diff
      REAL*8 loval, hival
      do var=0, nfinecomp -1
      chiterations=(1+ibcoarsehi1-ibcoarselo1)*(1+ibcoarsehi0-ibcoarselo
     &0)*1
      do ic1 = ibcoarselo1,ibcoarsehi1
      do ic0 = ibcoarselo0,ibcoarsehi0
      chiterations=(1+iinteriorrefboxhi1-iinteriorrefboxlo1)*(1+iinterio
     &rrefboxhi0-iinteriorrefboxlo0)*1
      do ii1 = iinteriorrefboxlo1,iinteriorrefboxhi1
      do ii0 = iinteriorrefboxlo0,iinteriorrefboxhi0
              ifine0 = ic0*ref_ratio + ii0
              ifine1 = ic1*ref_ratio + ii1
              iloface0 = ic0*ref_ratio + (1-CHF_ID(0,facedir))*ii0
              iloface1 = ic1*ref_ratio + (1-CHF_ID(1,facedir))*ii1
              ihiface0 = iloface0 + ref_ratio*CHF_ID(0,facedir)
              ihiface1 = iloface1 + ref_ratio*CHF_ID(1,facedir)
              if (facedir .eq. 0) then
                 id = ii0
              else if (facedir .eq. 1) then
                 id = ii1
              endif
              dxf = float(id)/ref_ratio
              diff = fine(ihiface0,ihiface1,var)
     & -fine(iloface0,iloface1,var)
              fine( ifine0,ifine1,var) =
     & fine(iloface0,iloface1,var)
     & +dxf*diff
      enddo
      enddo
      enddo
      enddo
       enddo
       return
       end
      subroutine INTERPLIMITFACE(
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
     & ,lslope
     & ,ilslopelo0,ilslopelo1
     & ,ilslopehi0,ilslopehi1
     & ,nlslopecomp
     & ,mslope
     & ,imslopelo0,imslopelo1
     & ,imslopehi0,imslopehi1
     & ,nmslopecomp
     & ,nslope
     & ,inslopelo0,inslopelo1
     & ,inslopehi0,inslopehi1
     & ,nnslopecomp
     & ,state
     & ,istatelo0,istatelo1
     & ,istatehi0,istatehi1
     & ,nstatecomp
     & ,iblo0,iblo1
     & ,ibhi0,ibhi1
     & ,ibnlo0,ibnlo1
     & ,ibnhi0,ibnhi1
     & ,ivalidBoxlo0,ivalidBoxlo1
     & ,ivalidBoxhi0,ivalidBoxhi1
     & ,normaldir
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
      integer nislopecomp
      integer iislopelo0,iislopelo1
      integer iislopehi0,iislopehi1
      REAL*8 islope(
     & iislopelo0:iislopehi0,
&iislopelo1:iislopehi1,
     & 0:nislopecomp-1)
      integer njslopecomp
      integer ijslopelo0,ijslopelo1
      integer ijslopehi0,ijslopehi1
      REAL*8 jslope(
     & ijslopelo0:ijslopehi0,
&ijslopelo1:ijslopehi1,
     & 0:njslopecomp-1)
      integer nkslopecomp
      integer ikslopelo0,ikslopelo1
      integer ikslopehi0,ikslopehi1
      REAL*8 kslope(
     & ikslopelo0:ikslopehi0,
&ikslopelo1:ikslopehi1,
     & 0:nkslopecomp-1)
      integer nlslopecomp
      integer ilslopelo0,ilslopelo1
      integer ilslopehi0,ilslopehi1
      REAL*8 lslope(
     & ilslopelo0:ilslopehi0,
&ilslopelo1:ilslopehi1,
     & 0:nlslopecomp-1)
      integer nmslopecomp
      integer imslopelo0,imslopelo1
      integer imslopehi0,imslopehi1
      REAL*8 mslope(
     & imslopelo0:imslopehi0,
&imslopelo1:imslopehi1,
     & 0:nmslopecomp-1)
      integer nnslopecomp
      integer inslopelo0,inslopelo1
      integer inslopehi0,inslopehi1
      REAL*8 nslope(
     & inslopelo0:inslopehi0,
&inslopelo1:inslopehi1,
     & 0:nnslopecomp-1)
      integer nstatecomp
      integer istatelo0,istatelo1
      integer istatehi0,istatehi1
      REAL*8 state(
     & istatelo0:istatehi0,
&istatelo1:istatehi1,
     & 0:nstatecomp-1)
      integer iblo0,iblo1
      integer ibhi0,ibhi1
      integer ibnlo0,ibnlo1
      integer ibnhi0,ibnhi1
      integer ivalidBoxlo0,ivalidBoxlo1
      integer ivalidBoxhi0,ivalidBoxhi1
      integer normaldir
      integer i0,i1, var
      integer ii0,ii1
      integer in0,in1
      REAL*8 statemax, statemin, deltasum, etamax, etamin, eta
      REAL*8 tempone, tempzero
      tempone = (1.0d0)
      tempzero = (0.0d0)
      do var = 0, nislopecomp - 1
      chiterations=(1+ibhi1-iblo1)*(1+ibhi0-iblo0)*1
      do i1 = iblo1,ibhi1
      do i0 = iblo0,ibhi0
             statemax = state ( i0,i1, var )
             statemin = state ( i0,i1, var )
      chiterations=(1+ibnhi1-ibnlo1)*(1+ibnhi0-ibnlo0)*1
      do ii1 = ibnlo1,ibnhi1
      do ii0 = ibnlo0,ibnhi0
                 in0 = i0 + ii0
                 in1 = i1 + ii1
                 if (
     & in0 .ge. ivalidBoxlo0 .and.
     & in0 .le. ivalidBoxhi0
     & .and.
     & in1 .ge. ivalidBoxlo1 .and.
     & in1 .le. ivalidBoxhi1
     & )
     & then
                    statemax = max ( statemax, state(in0,in1,var))
                    statemin = min ( statemin, state(in0,in1,var))
                 endif
      enddo
      enddo
             deltasum = (0.500d0) * (
     & (1-CHF_ID(normaldir,0))*abs(islope(i0,i1,var))
     & +
     & (1-CHF_ID(normaldir,1))*abs(jslope(i0,i1,var))
     & )
             if ( deltasum .gt. (0.0d0) ) then
                etamax = ( statemax - state ( i0,i1, var ) )
     & / deltasum
                etamin = ( state ( i0,i1, var ) - statemin )
     & / deltasum
                eta = max ( min ( etamin, etamax, tempone ), tempzero )
                islope ( i0,i1, var ) =
     & eta * islope ( i0,i1, var )
                jslope ( i0,i1, var ) =
     & eta * jslope ( i0,i1, var )
             end if
      enddo
      enddo
      end do
      return
      end

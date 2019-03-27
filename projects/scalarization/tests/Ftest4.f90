#define FUSE_LOOPS
#define SCALARIZE
module gvars
  
  implicit none
  
  save
  integer,parameter :: vecsize = 1024576
  real(8),dimension(vecsize,3,10),target :: glo1
  real(8),dimension(vecsize) :: glo3
  real(8),dimension(vecsize) :: &
       px1,px2,px3,px4,px5,px6,px7,px8, &
       py1,py2,py3,py4,py5,py6,py7,py8, &
       pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8
  real(8),dimension(vecsize,3,10) :: glo2
  real(8),dimension(vecsize) :: glo4
  real(8),dimension(vecsize,3,10) :: glo6
  real(8),dimension(vecsize) :: glo5
  real(8),dimension(vecsize) :: glo7
  integer :: vecn 
  real(8) :: gs1,gs2
end module gvars

subroutine sub1 (fff,e_s,var1,var2,var3)
     
  use gvars, only: glo1,glo3, &
       px1,px2,px3,px4,px5,px6,px7,px8, &
       py1,py2,py3,py4,py5,py6,py7,py8, &
       pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
       glo2,glo4,glo6,glo5,glo7, &
       vecn,gs1,vecsize,gs2

  implicit none

  real(8) :: e_s,var1,var2,var3
  real(8),dimension(12,vecsize) :: fff

  integer :: i,j,k
  real(8) :: scal1,scal2,scal3,scal4
!$rose scalarization
  real(8),dimension(vecsize) :: fac,&
       sv11,sv21,sv31,sv41,sv51,sv61,sv71,sv81, &
       sv12,sv22,sv32,sv42,sv52,sv62,sv72,sv82, &
       sv13,sv23,sv33,sv43,sv53,sv63,sv73,sv83, &
       sv14,sv24,sv34,sv44,sv54,sv64,sv74,sv84, &
       qqx1,qqx2,qqx3,qqx4,qqy1,qqy2,qqy3,qqy4,qqz1,qqz2,qqz3,qqz4, &
       hx1,hx2,hx3,hx4,hy1,hy2,hy3,hy4,hz1,hz2,hz3,hz4, &
       hx1v,hx2v,hx3v,hx4v,hy1v,hy2v,hy3v,hy4v,hz1v,hz2v,hz3v,hz4v, &
       qqwk
  if(var1 <= 0.0d0 .and. var2 <= 0.0d0) then
!$acc parallel loop gang vector collapse(3) async(1)
!$omp target teams distribute parallel do collapse(3) 
    do k=1,8
      do j=1,3
        do i=1,vecn
          glo6(i,j,k) = 0.0d0
        enddo
      enddo
    enddo
    return
  endif

  scal4 = 0.0d0
!$acc data  async(1) create( &
!$acc&      fac, &
!$acc&      sv11,sv21,sv31,sv41,sv51,sv61,sv71,sv81, &
!$acc&      sv12,sv22,sv32,sv42,sv52,sv62,sv72,sv82, &
!$acc&      sv13,sv23,sv33,sv43,sv53,sv63,sv73,sv83, &
!$acc&      sv14,sv24,sv34,sv44,sv54,sv64,sv74,sv84, &
!$acc&      qqx1,qqx2,qqx3,qqx4,qqy1,qqy2,qqy3,qqy4,qqz1,qqz2,qqz3,qqz4, &
!$acc&      hx1,hx2,hx3,hx4,hy1,hy2,hy3,hy4,hz1,hz2,hz3,hz4, &
!$acc&      hx1v,hx2v,hx3v,hx4v,hy1v,hy2v,hy3v,hy4v,hz1v,hz2v,hz3v,hz4v, &
!$acc&      qqwk)
!$omp target data  map(alloc: &
!$omp&      fac, &
!$omp&      sv11,sv21,sv31,sv41,sv51,sv61,sv71,sv81, &
!$omp&      sv12,sv22,sv32,sv42,sv52,sv62,sv72,sv82, &
!$omp&      sv13,sv23,sv33,sv43,sv53,sv63,sv73,sv83, &
!$omp&      sv14,sv24,sv34,sv44,sv54,sv64,sv74,sv84, &
!$omp&      qqx1,qqx2,qqx3,qqx4,qqy1,qqy2,qqy3,qqy4,qqz1,qqz2,qqz3,qqz4, &
!$omp&      hx1,hx2,hx3,hx4,hy1,hy2,hy3,hy4,hz1,hz2,hz3,hz4, &
!$omp&      hx1v,hx2v,hx3v,hx4v,hy1v,hy2v,hy3v,hy4v,hz1v,hz2v,hz3v,hz4v, &
!$omp&      qqwk)

!$acc parallel loop gang vector  async(1)
!$omp target teams distribute parallel do 

  do i=1,vecn
    hx1(i) = glo1(i,1,3)-glo1(i,1,4)-glo1(i,1,7)+glo1(i,1,8)
    hx2(i) = glo1(i,1,2)-glo1(i,1,3)-glo1(i,1,5)+glo1(i,1,8)
    hx3(i) = glo1(i,1,1)-glo1(i,1,4)-glo1(i,1,6)+glo1(i,1,7)
    hx4(i) = glo1(i,1,1)-glo1(i,1,2)-glo1(i,1,5)+glo1(i,1,6)
    hy1(i) = glo1(i,2,3)-glo1(i,2,4)-glo1(i,2,7)+glo1(i,2,8)
    hy2(i) = glo1(i,2,2)-glo1(i,2,3)-glo1(i,2,5)+glo1(i,2,8)
    hy3(i) = glo1(i,2,1)-glo1(i,2,4)-glo1(i,2,6)+glo1(i,2,7)
    hy4(i) = glo1(i,2,1)-glo1(i,2,2)-glo1(i,2,5)+glo1(i,2,6)
    hz1(i) = glo1(i,3,3)-glo1(i,3,4)-glo1(i,3,7)+glo1(i,3,8)
    hz2(i) = glo1(i,3,2)-glo1(i,3,3)-glo1(i,3,5)+glo1(i,3,8)
    hz3(i) = glo1(i,3,1)-glo1(i,3,4)-glo1(i,3,6)+glo1(i,3,7)
    hz4(i) = glo1(i,3,1)-glo1(i,3,2)-glo1(i,3,5)+glo1(i,3,6)

    scal3  = 1.0d0/glo4(i)
    qqx1(i) = (hx3(i)-hx2(i))*scal3
    qqx2(i) = (hx3(i)+hx2(i))*scal3
    qqx3(i) = (hx4(i)-hx1(i))*scal3
    qqx4(i) = (hx4(i)+hx1(i))*scal3
    qqy1(i) = (hy3(i)-hy2(i))*scal3
    qqy2(i) = (hy3(i)+hy2(i))*scal3
    qqy3(i) = (hy4(i)-hy1(i))*scal3
    qqy4(i) = (hy4(i)+hy1(i))*scal3
    qqz1(i) = (hz3(i)-hz2(i))*scal3
    qqz2(i) = (hz3(i)+hz2(i))*scal3
    qqz3(i) = (hz4(i)-hz1(i))*scal3
    qqz4(i) = (hz4(i)+hz1(i))*scal3

    sv11(i) =  1.0d0-qqx1(i)*px1(i)-qqy1(i)*py1(i)-qqz1(i)*pz1(i)
    sv21(i) = -1.0d0-qqx1(i)*px2(i)-qqy1(i)*py2(i)-qqz1(i)*pz2(i)
    sv31(i) =  1.0d0-qqx1(i)*px3(i)-qqy1(i)*py3(i)-qqz1(i)*pz3(i)
    sv41(i) = -1.0d0-qqx1(i)*px4(i)-qqy1(i)*py4(i)-qqz1(i)*pz4(i)
    sv51(i) =  1.0d0-qqx1(i)*px5(i)-qqy1(i)*py5(i)-qqz1(i)*pz5(i)
    sv61(i) = -1.0d0-qqx1(i)*px6(i)-qqy1(i)*py6(i)-qqz1(i)*pz6(i)
    sv71(i) =  1.0d0-qqx1(i)*px7(i)-qqy1(i)*py7(i)-qqz1(i)*pz7(i)
    sv81(i) = -1.0d0-qqx1(i)*px8(i)-qqy1(i)*py8(i)-qqz1(i)*pz8(i)

    sv12(i) =  1.0d0-qqx2(i)*px1(i)-qqy2(i)*py1(i)-qqz2(i)*pz1(i)
    sv22(i) =  1.0d0-qqx2(i)*px2(i)-qqy2(i)*py2(i)-qqz2(i)*pz2(i)
    sv32(i) = -1.0d0-qqx2(i)*px3(i)-qqy2(i)*py3(i)-qqz2(i)*pz3(i)
    sv42(i) = -1.0d0-qqx2(i)*px4(i)-qqy2(i)*py4(i)-qqz2(i)*pz4(i)
    sv52(i) = -1.0d0-qqx2(i)*px5(i)-qqy2(i)*py5(i)-qqz2(i)*pz5(i)
    sv62(i) = -1.0d0-qqx2(i)*px6(i)-qqy2(i)*py6(i)-qqz2(i)*pz6(i)
    sv72(i) =  1.0d0-qqx2(i)*px7(i)-qqy2(i)*py7(i)-qqz2(i)*pz7(i)
    sv82(i) =  1.0d0-qqx2(i)*px8(i)-qqy2(i)*py8(i)-qqz2(i)*pz8(i)

    sv13(i) =  1.0d0-qqx3(i)*px1(i)-qqy3(i)*py1(i)-qqz3(i)*pz1(i)
    sv23(i) = -1.0d0-qqx3(i)*px2(i)-qqy3(i)*py2(i)-qqz3(i)*pz2(i)
    sv33(i) = -1.0d0-qqx3(i)*px3(i)-qqy3(i)*py3(i)-qqz3(i)*pz3(i)
    sv43(i) =  1.0d0-qqx3(i)*px4(i)-qqy3(i)*py4(i)-qqz3(i)*pz4(i)
    sv53(i) = -1.0d0-qqx3(i)*px5(i)-qqy3(i)*py5(i)-qqz3(i)*pz5(i)
    sv63(i) =  1.0d0-qqx3(i)*px6(i)-qqy3(i)*py6(i)-qqz3(i)*pz6(i)
    sv73(i) =  1.0d0-qqx3(i)*px7(i)-qqy3(i)*py7(i)-qqz3(i)*pz7(i)
    sv83(i) = -1.0d0-qqx3(i)*px8(i)-qqy3(i)*py8(i)-qqz3(i)*pz8(i)

    sv14(i) =  1.0d0-qqx4(i)*px1(i)-qqy4(i)*py1(i)-qqz4(i)*pz1(i)
    sv24(i) = -1.0d0-qqx4(i)*px2(i)-qqy4(i)*py2(i)-qqz4(i)*pz2(i)
    sv34(i) =  1.0d0-qqx4(i)*px3(i)-qqy4(i)*py3(i)-qqz4(i)*pz3(i)
    sv44(i) = -1.0d0-qqx4(i)*px4(i)-qqy4(i)*py4(i)-qqz4(i)*pz4(i)
    sv54(i) = -1.0d0-qqx4(i)*px5(i)-qqy4(i)*py5(i)-qqz4(i)*pz5(i)
    sv64(i) =  1.0d0-qqx4(i)*px6(i)-qqy4(i)*py6(i)-qqz4(i)*pz6(i)
    sv74(i) = -1.0d0-qqx4(i)*px7(i)-qqy4(i)*py7(i)-qqz4(i)*pz7(i)
    sv84(i) =  1.0d0-qqx4(i)*px8(i)-qqy4(i)*py8(i)-qqz4(i)*pz8(i)

    hx1v(i) = glo2(i,1,1)*sv11(i)+glo2(i,1,2)*sv21(i)+glo2(i,1,3)*sv31(i)+glo2(i,1,4)*sv41(i) &
            + glo2(i,1,5)*sv51(i)+glo2(i,1,6)*sv61(i)+glo2(i,1,7)*sv71(i)+glo2(i,1,8)*sv81(i)
    hx2v(i) = glo2(i,1,1)*sv12(i)+glo2(i,1,2)*sv22(i)+glo2(i,1,3)*sv32(i)+glo2(i,1,4)*sv42(i) &
            + glo2(i,1,5)*sv52(i)+glo2(i,1,6)*sv62(i)+glo2(i,1,7)*sv72(i)+glo2(i,1,8)*sv82(i)
    hx3v(i) = glo2(i,1,1)*sv13(i)+glo2(i,1,2)*sv23(i)+glo2(i,1,3)*sv33(i)+glo2(i,1,4)*sv43(i) &
            + glo2(i,1,5)*sv53(i)+glo2(i,1,6)*sv63(i)+glo2(i,1,7)*sv73(i)+glo2(i,1,8)*sv83(i)
    hx4v(i) = glo2(i,1,1)*sv14(i)+glo2(i,1,2)*sv24(i)+glo2(i,1,3)*sv34(i)+glo2(i,1,4)*sv44(i) &
            + glo2(i,1,5)*sv54(i)+glo2(i,1,6)*sv64(i)+glo2(i,1,7)*sv74(i)+glo2(i,1,8)*sv84(i)

    hy1v(i) = glo2(i,2,1)*sv11(i)+glo2(i,2,2)*sv21(i)+glo2(i,2,3)*sv31(i)+glo2(i,2,4)*sv41(i) &
            + glo2(i,2,5)*sv51(i)+glo2(i,2,6)*sv61(i)+glo2(i,2,7)*sv71(i)+glo2(i,2,8)*sv81(i)
    hy2v(i) = glo2(i,2,1)*sv12(i)+glo2(i,2,2)*sv22(i)+glo2(i,2,3)*sv32(i)+glo2(i,2,4)*sv42(i) &
            + glo2(i,2,5)*sv52(i)+glo2(i,2,6)*sv62(i)+glo2(i,2,7)*sv72(i)+glo2(i,2,8)*sv82(i)
    hy3v(i) = glo2(i,2,1)*sv13(i)+glo2(i,2,2)*sv23(i)+glo2(i,2,3)*sv33(i)+glo2(i,2,4)*sv43(i) &
            + glo2(i,2,5)*sv53(i)+glo2(i,2,6)*sv63(i)+glo2(i,2,7)*sv73(i)+glo2(i,2,8)*sv83(i)
    hy4v(i) = glo2(i,2,1)*sv14(i)+glo2(i,2,2)*sv24(i)+glo2(i,2,3)*sv34(i)+glo2(i,2,4)*sv44(i) &
            + glo2(i,2,5)*sv54(i)+glo2(i,2,6)*sv64(i)+glo2(i,2,7)*sv74(i)+glo2(i,2,8)*sv84(i)

    hz1v(i) = glo2(i,3,1)*sv11(i)+glo2(i,3,2)*sv21(i)+glo2(i,3,3)*sv31(i)+glo2(i,3,4)*sv41(i) &
            + glo2(i,3,5)*sv51(i)+glo2(i,3,6)*sv61(i)+glo2(i,3,7)*sv71(i)+glo2(i,3,8)*sv81(i)
    hz2v(i) = glo2(i,3,1)*sv12(i)+glo2(i,3,2)*sv22(i)+glo2(i,3,3)*sv32(i)+glo2(i,3,4)*sv42(i) &
            + glo2(i,3,5)*sv52(i)+glo2(i,3,6)*sv62(i)+glo2(i,3,7)*sv72(i)+glo2(i,3,8)*sv82(i)
    hz3v(i) = glo2(i,3,1)*sv13(i)+glo2(i,3,2)*sv23(i)+glo2(i,3,3)*sv33(i)+glo2(i,3,4)*sv43(i) &
            + glo2(i,3,5)*sv53(i)+glo2(i,3,6)*sv63(i)+glo2(i,3,7)*sv73(i)+glo2(i,3,8)*sv83(i)
    hz4v(i) = glo2(i,3,1)*sv14(i)+glo2(i,3,2)*sv24(i)+glo2(i,3,3)*sv34(i)+glo2(i,3,4)*sv44(i) &
            + glo2(i,3,5)*sv54(i)+glo2(i,3,6)*sv64(i)+glo2(i,3,7)*sv74(i)+glo2(i,3,8)*sv84(i)
  enddo

  if (var1 > 0.0d0) then

    scal1 = .25d0*var1*gs2*var3
!$acc parallel loop gang vector async(1) private(scal2)
!$omp target teams distribute parallel do private(scal2)
    do i=1,vecn
      scal2 = px1(i)**2+px2(i)**2+px3(i)**2+px4(i)**2 &
           + py1(i)**2+py2(i)**2+py3(i)**2+py4(i)**2 &
           + pz1(i)**2+pz2(i)**2+pz3(i)**2+pz4(i)**2 &
           + px5(i)**2+px6(i)**2+px7(i)**2+px8(i)**2 &
           + py5(i)**2+py6(i)**2+py7(i)**2+py8(i)**2 &
           + pz5(i)**2+pz6(i)**2+pz7(i)**2+pz8(i)**2
      fac(i) = scal1*scal2/glo4(i)

      fff(1,i)  = fff(1,i)  + fac(i)*hx1v(i)
      fff(2,i)  = fff(2,i)  + fac(i)*hx2v(i)
      fff(3,i)  = fff(3,i)  + fac(i)*hx3v(i)
      fff(4,i)  = fff(4,i)  + fac(i)*hx4v(i)
      fff(5,i)  = fff(5,i)  + fac(i)*hy1v(i)
      fff(6,i)  = fff(6,i)  + fac(i)*hy2v(i)
      fff(7,i)  = fff(7,i)  + fac(i)*hy3v(i)
      fff(8,i)  = fff(8,i)  + fac(i)*hy4v(i)
      fff(9,i)  = fff(9,i)  + fac(i)*hz1v(i)
      fff(10,i) = fff(10,i) + fac(i)*hz2v(i)
      fff(11,i) = fff(11,i) + fac(i)*hz3v(i)
      fff(12,i) = fff(12,i) + fac(i)*hz4v(i)
    enddo
    if(var2 <= 0.0d0) then
!$acc parallel loop gang vector  async(1)
!$omp target teams distribute parallel do 
      do i=1,vecn
        hx1(i) = fff(1,i)
        hx2(i) = fff(2,i)
        hx3(i) = fff(3,i)
        hx4(i) = fff(4,i)
        hy1(i) = fff(5,i)
        hy2(i) = fff(6,i)
        hy3(i) = fff(7,i)
        hy4(i) = fff(8,i)
        hz1(i) = fff(9,i)
        hz2(i) = fff(10,i)
        hz3(i) = fff(11,i)
        hz4(i) = fff(12,i)
      enddo
    else
      scal1 = -0.25d0*var2*gs1
!$acc parallel loop gang vector  async(1)
!$omp target teams distribute parallel do 
      do i=1,vecn
        fac(i) = glo7(i)*scal1*glo3(i)*glo3(i)/glo5(i)
        
        hx1(i) = fac(i)*hx1v(i) + fff(1,i)
        hx2(i) = fac(i)*hx2v(i) + fff(2,i)
        hx3(i) = fac(i)*hx3v(i) + fff(3,i)
        hx4(i) = fac(i)*hx4v(i) + fff(4,i)
        hy1(i) = fac(i)*hy1v(i) + fff(5,i)
        hy2(i) = fac(i)*hy2v(i) + fff(6,i)
        hy3(i) = fac(i)*hy3v(i) + fff(7,i)
        hy4(i) = fac(i)*hy4v(i) + fff(8,i)
        hz1(i) = fac(i)*hz1v(i) + fff(9,i)
        hz2(i) = fac(i)*hz2v(i) + fff(10,i)
        hz3(i) = fac(i)*hz3v(i) + fff(11,i)
        hz4(i) = fac(i)*hz4v(i) + fff(12,i)
      enddo
    endif

  elseif(var2 > 0.0d0) then

    scal1 = -0.25d0*var2*gs1
!$acc parallel loop gang vector  async(1)
!$omp target teams distribute parallel do 
    do i=1,vecn
      fac(i) = glo7(i)*scal1*glo3(i)*glo3(i)/glo5(i)

      hx1(i) = fac(i)*hx1v(i)
      hx2(i) = fac(i)*hx2v(i)
      hx3(i) = fac(i)*hx3v(i)
      hx4(i) = fac(i)*hx4v(i)
      hy1(i) = fac(i)*hy1v(i)
      hy2(i) = fac(i)*hy2v(i)
      hy3(i) = fac(i)*hy3v(i)
      hy4(i) = fac(i)*hy4v(i)
      hz1(i) = fac(i)*hz1v(i)
      hz2(i) = fac(i)*hz2v(i)
      hz3(i) = fac(i)*hz3v(i)
      hz4(i) = fac(i)*hz4v(i)
    enddo

  endif

!$acc parallel loop gang vector  async(1) reduction(+:scal4)
!$omp target teams distribute parallel do  reduction(+:scal4)
  do i=1,vecn
    glo6(i,1,1) = -hx1(i)*sv11(i)-hx2(i)*sv12(i)-hx3(i)*sv13(i)-hx4(i)*sv14(i)
    glo6(i,1,2) = -hx1(i)*sv21(i)-hx2(i)*sv22(i)-hx3(i)*sv23(i)-hx4(i)*sv24(i)
    glo6(i,1,3) = -hx1(i)*sv31(i)-hx2(i)*sv32(i)-hx3(i)*sv33(i)-hx4(i)*sv34(i)
    glo6(i,1,4) = -hx1(i)*sv41(i)-hx2(i)*sv42(i)-hx3(i)*sv43(i)-hx4(i)*sv44(i)
    glo6(i,1,5) = -hx1(i)*sv51(i)-hx2(i)*sv52(i)-hx3(i)*sv53(i)-hx4(i)*sv54(i)
    glo6(i,1,6) = -hx1(i)*sv61(i)-hx2(i)*sv62(i)-hx3(i)*sv63(i)-hx4(i)*sv64(i)
    glo6(i,1,7) = -hx1(i)*sv71(i)-hx2(i)*sv72(i)-hx3(i)*sv73(i)-hx4(i)*sv74(i)
    glo6(i,1,8) = -hx1(i)*sv81(i)-hx2(i)*sv82(i)-hx3(i)*sv83(i)-hx4(i)*sv84(i)

    glo6(i,2,1) = -hy1(i)*sv11(i)-hy2(i)*sv12(i)-hy3(i)*sv13(i)-hy4(i)*sv14(i)
    glo6(i,2,2) = -hy1(i)*sv21(i)-hy2(i)*sv22(i)-hy3(i)*sv23(i)-hy4(i)*sv24(i)
    glo6(i,2,3) = -hy1(i)*sv31(i)-hy2(i)*sv32(i)-hy3(i)*sv33(i)-hy4(i)*sv34(i)
    glo6(i,2,4) = -hy1(i)*sv41(i)-hy2(i)*sv42(i)-hy3(i)*sv43(i)-hy4(i)*sv44(i)
    glo6(i,2,5) = -hy1(i)*sv51(i)-hy2(i)*sv52(i)-hy3(i)*sv53(i)-hy4(i)*sv54(i)
    glo6(i,2,6) = -hy1(i)*sv61(i)-hy2(i)*sv62(i)-hy3(i)*sv63(i)-hy4(i)*sv64(i)
    glo6(i,2,7) = -hy1(i)*sv71(i)-hy2(i)*sv72(i)-hy3(i)*sv73(i)-hy4(i)*sv74(i)
    glo6(i,2,8) = -hy1(i)*sv81(i)-hy2(i)*sv82(i)-hy3(i)*sv83(i)-hy4(i)*sv84(i)

    glo6(i,3,1) = -hz1(i)*sv11(i)-hz2(i)*sv12(i)-hz3(i)*sv13(i)-hz4(i)*sv14(i)
    glo6(i,3,2) = -hz1(i)*sv21(i)-hz2(i)*sv22(i)-hz3(i)*sv23(i)-hz4(i)*sv24(i)
    glo6(i,3,3) = -hz1(i)*sv31(i)-hz2(i)*sv32(i)-hz3(i)*sv33(i)-hz4(i)*sv34(i)
    glo6(i,3,4) = -hz1(i)*sv41(i)-hz2(i)*sv42(i)-hz3(i)*sv43(i)-hz4(i)*sv44(i)
    glo6(i,3,5) = -hz1(i)*sv51(i)-hz2(i)*sv52(i)-hz3(i)*sv53(i)-hz4(i)*sv54(i)
    glo6(i,3,6) = -hz1(i)*sv61(i)-hz2(i)*sv62(i)-hz3(i)*sv63(i)-hz4(i)*sv64(i)
    glo6(i,3,7) = -hz1(i)*sv71(i)-hz2(i)*sv72(i)-hz3(i)*sv73(i)-hz4(i)*sv74(i)
    glo6(i,3,8) = -hz1(i)*sv81(i)-hz2(i)*sv82(i)-hz3(i)*sv83(i)-hz4(i)*sv84(i)

    qqwk(i) = &
         hx1(i)*hx1v(i) + hx2(i)*hx2v(i) + hx3(i)*hx3v(i) + hx4(i)*hx4v(i) &
         + hy1(i)*hy1v(i) + hy2(i)*hy2v(i) + hy3(i)*hy3v(i) + hy4(i)*hy4v(i) &
         + hz1(i)*hz1v(i) + hz2(i)*hz2v(i) + hz3(i)*hz3v(i) + hz4(i)*hz4v(i)
    scal4 = scal4 + qqwk(i)
  enddo
  e_s = e_s - gs2*scal4
     
!$acc end data
!$omp end target data
end subroutine sub1
   
subroutine sub1_fuse (fff,e_s,var1,var2,var3)

  use gvars, only: glo1,glo3, &
       px1,px2,px3,px4,px5,px6,px7,px8, &
       py1,py2,py3,py4,py5,py6,py7,py8, &
       pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
       glo2,glo4,glo6,glo5,glo7, &
       vecn,gs1,vecsize,gs2

  implicit none

  real(8) :: e_s,var1,var2,var3
  real(8),dimension(12,vecsize) :: fff

  integer :: i,j,k
  real(8) :: scal1,scal2,scal3,scal4
  real(8),dimension(vecsize) :: fac,&
       sv11,sv21,sv31,sv41,sv51,sv61,sv71,sv81, &
       sv12,sv22,sv32,sv42,sv52,sv62,sv72,sv82, &
       sv13,sv23,sv33,sv43,sv53,sv63,sv73,sv83, &
       sv14,sv24,sv34,sv44,sv54,sv64,sv74,sv84, &
       qqx1,qqx2,qqx3,qqx4,qqy1,qqy2,qqy3,qqy4,qqz1,qqz2,qqz3,qqz4, &
       hx1,hx2,hx3,hx4,hy1,hy2,hy3,hy4,hz1,hz2,hz3,hz4, &
       hx1v,hx2v,hx3v,hx4v,hy1v,hy2v,hy3v,hy4v,hz1v,hz2v,hz3v,hz4v, &
       qqwk
  if(var1 <= 0.0d0 .and. var2 <= 0.0d0) then
!$acc parallel loop gang vector collapse(3) async(1)
!$omp target teams distribute parallel do collapse(3) 
    do k=1,8
      do j=1,3
        do i=1,vecn
          glo6(i,j,k) = 0.0d0
        enddo
      enddo
    enddo
    return
  endif

  scal4 = 0.0d0
!$acc data  async(1) create( &
!$acc&      fac, &
!$acc&      sv11,sv21,sv31,sv41,sv51,sv61,sv71,sv81, &
!$acc&      sv12,sv22,sv32,sv42,sv52,sv62,sv72,sv82, &
!$acc&      sv13,sv23,sv33,sv43,sv53,sv63,sv73,sv83, &
!$acc&      sv14,sv24,sv34,sv44,sv54,sv64,sv74,sv84, &
!$acc&      qqx1,qqx2,qqx3,qqx4,qqy1,qqy2,qqy3,qqy4,qqz1,qqz2,qqz3,qqz4, &
!$acc&      hx1,hx2,hx3,hx4,hy1,hy2,hy3,hy4,hz1,hz2,hz3,hz4, &
!$acc&      hx1v,hx2v,hx3v,hx4v,hy1v,hy2v,hy3v,hy4v,hz1v,hz2v,hz3v,hz4v, &
!$acc&      qqwk)
!$omp target data  map(alloc: &
!$omp&      fac, &
!$omp&      sv11,sv21,sv31,sv41,sv51,sv61,sv71,sv81, &
!$omp&      sv12,sv22,sv32,sv42,sv52,sv62,sv72,sv82, &
!$omp&      sv13,sv23,sv33,sv43,sv53,sv63,sv73,sv83, &
!$omp&      sv14,sv24,sv34,sv44,sv54,sv64,sv74,sv84, &
!$omp&      qqx1,qqx2,qqx3,qqx4,qqy1,qqy2,qqy3,qqy4,qqz1,qqz2,qqz3,qqz4, &
!$omp&      hx1,hx2,hx3,hx4,hy1,hy2,hy3,hy4,hz1,hz2,hz3,hz4, &
!$omp&      hx1v,hx2v,hx3v,hx4v,hy1v,hy2v,hy3v,hy4v,hz1v,hz2v,hz3v,hz4v, &
!$omp&      qqwk)

!$acc parallel loop gang vector  async(1) &
#ifdef FUSE_LOOPS
!$acc& reduction(+:scal4) &
#endif
!$acc
!$omp target teams distribute parallel do &
#ifdef FUSE_LOOPS
!$omp& reduction(+:scal4) &
#endif
!$omp
  do i=1,vecn
    hx1(i) = glo1(i,1,3)-glo1(i,1,4)-glo1(i,1,7)+glo1(i,1,8)
    hx2(i) = glo1(i,1,2)-glo1(i,1,3)-glo1(i,1,5)+glo1(i,1,8)
    hx3(i) = glo1(i,1,1)-glo1(i,1,4)-glo1(i,1,6)+glo1(i,1,7)
    hx4(i) = glo1(i,1,1)-glo1(i,1,2)-glo1(i,1,5)+glo1(i,1,6)
    hy1(i) = glo1(i,2,3)-glo1(i,2,4)-glo1(i,2,7)+glo1(i,2,8)
    hy2(i) = glo1(i,2,2)-glo1(i,2,3)-glo1(i,2,5)+glo1(i,2,8)
    hy3(i) = glo1(i,2,1)-glo1(i,2,4)-glo1(i,2,6)+glo1(i,2,7)
    hy4(i) = glo1(i,2,1)-glo1(i,2,2)-glo1(i,2,5)+glo1(i,2,6)
    hz1(i) = glo1(i,3,3)-glo1(i,3,4)-glo1(i,3,7)+glo1(i,3,8)
    hz2(i) = glo1(i,3,2)-glo1(i,3,3)-glo1(i,3,5)+glo1(i,3,8)
    hz3(i) = glo1(i,3,1)-glo1(i,3,4)-glo1(i,3,6)+glo1(i,3,7)
    hz4(i) = glo1(i,3,1)-glo1(i,3,2)-glo1(i,3,5)+glo1(i,3,6)

    scal3  = 1.0d0/glo4(i)
    qqx1(i) = (hx3(i)-hx2(i))*scal3
    qqx2(i) = (hx3(i)+hx2(i))*scal3
    qqx3(i) = (hx4(i)-hx1(i))*scal3
    qqx4(i) = (hx4(i)+hx1(i))*scal3
    qqy1(i) = (hy3(i)-hy2(i))*scal3
    qqy2(i) = (hy3(i)+hy2(i))*scal3
    qqy3(i) = (hy4(i)-hy1(i))*scal3
    qqy4(i) = (hy4(i)+hy1(i))*scal3
    qqz1(i) = (hz3(i)-hz2(i))*scal3
    qqz2(i) = (hz3(i)+hz2(i))*scal3
    qqz3(i) = (hz4(i)-hz1(i))*scal3
    qqz4(i) = (hz4(i)+hz1(i))*scal3

    sv11(i) =  1.0d0-qqx1(i)*px1(i)-qqy1(i)*py1(i)-qqz1(i)*pz1(i)
    sv21(i) = -1.0d0-qqx1(i)*px2(i)-qqy1(i)*py2(i)-qqz1(i)*pz2(i)
    sv31(i) =  1.0d0-qqx1(i)*px3(i)-qqy1(i)*py3(i)-qqz1(i)*pz3(i)
    sv41(i) = -1.0d0-qqx1(i)*px4(i)-qqy1(i)*py4(i)-qqz1(i)*pz4(i)
    sv51(i) =  1.0d0-qqx1(i)*px5(i)-qqy1(i)*py5(i)-qqz1(i)*pz5(i)
    sv61(i) = -1.0d0-qqx1(i)*px6(i)-qqy1(i)*py6(i)-qqz1(i)*pz6(i)
    sv71(i) =  1.0d0-qqx1(i)*px7(i)-qqy1(i)*py7(i)-qqz1(i)*pz7(i)
    sv81(i) = -1.0d0-qqx1(i)*px8(i)-qqy1(i)*py8(i)-qqz1(i)*pz8(i)

    sv12(i) =  1.0d0-qqx2(i)*px1(i)-qqy2(i)*py1(i)-qqz2(i)*pz1(i)
    sv22(i) =  1.0d0-qqx2(i)*px2(i)-qqy2(i)*py2(i)-qqz2(i)*pz2(i)
    sv32(i) = -1.0d0-qqx2(i)*px3(i)-qqy2(i)*py3(i)-qqz2(i)*pz3(i)
    sv42(i) = -1.0d0-qqx2(i)*px4(i)-qqy2(i)*py4(i)-qqz2(i)*pz4(i)
    sv52(i) = -1.0d0-qqx2(i)*px5(i)-qqy2(i)*py5(i)-qqz2(i)*pz5(i)
    sv62(i) = -1.0d0-qqx2(i)*px6(i)-qqy2(i)*py6(i)-qqz2(i)*pz6(i)
    sv72(i) =  1.0d0-qqx2(i)*px7(i)-qqy2(i)*py7(i)-qqz2(i)*pz7(i)
    sv82(i) =  1.0d0-qqx2(i)*px8(i)-qqy2(i)*py8(i)-qqz2(i)*pz8(i)

    sv13(i) =  1.0d0-qqx3(i)*px1(i)-qqy3(i)*py1(i)-qqz3(i)*pz1(i)
    sv23(i) = -1.0d0-qqx3(i)*px2(i)-qqy3(i)*py2(i)-qqz3(i)*pz2(i)
    sv33(i) = -1.0d0-qqx3(i)*px3(i)-qqy3(i)*py3(i)-qqz3(i)*pz3(i)
    sv43(i) =  1.0d0-qqx3(i)*px4(i)-qqy3(i)*py4(i)-qqz3(i)*pz4(i)
    sv53(i) = -1.0d0-qqx3(i)*px5(i)-qqy3(i)*py5(i)-qqz3(i)*pz5(i)
    sv63(i) =  1.0d0-qqx3(i)*px6(i)-qqy3(i)*py6(i)-qqz3(i)*pz6(i)
    sv73(i) =  1.0d0-qqx3(i)*px7(i)-qqy3(i)*py7(i)-qqz3(i)*pz7(i)
    sv83(i) = -1.0d0-qqx3(i)*px8(i)-qqy3(i)*py8(i)-qqz3(i)*pz8(i)

    sv14(i) =  1.0d0-qqx4(i)*px1(i)-qqy4(i)*py1(i)-qqz4(i)*pz1(i)
    sv24(i) = -1.0d0-qqx4(i)*px2(i)-qqy4(i)*py2(i)-qqz4(i)*pz2(i)
    sv34(i) =  1.0d0-qqx4(i)*px3(i)-qqy4(i)*py3(i)-qqz4(i)*pz3(i)
    sv44(i) = -1.0d0-qqx4(i)*px4(i)-qqy4(i)*py4(i)-qqz4(i)*pz4(i)
    sv54(i) = -1.0d0-qqx4(i)*px5(i)-qqy4(i)*py5(i)-qqz4(i)*pz5(i)
    sv64(i) =  1.0d0-qqx4(i)*px6(i)-qqy4(i)*py6(i)-qqz4(i)*pz6(i)
    sv74(i) = -1.0d0-qqx4(i)*px7(i)-qqy4(i)*py7(i)-qqz4(i)*pz7(i)
    sv84(i) =  1.0d0-qqx4(i)*px8(i)-qqy4(i)*py8(i)-qqz4(i)*pz8(i)

    hx1v(i) = glo2(i,1,1)*sv11(i)+glo2(i,1,2)*sv21(i)+glo2(i,1,3)*sv31(i)+glo2(i,1,4)*sv41(i) &
            + glo2(i,1,5)*sv51(i)+glo2(i,1,6)*sv61(i)+glo2(i,1,7)*sv71(i)+glo2(i,1,8)*sv81(i)
    hx2v(i) = glo2(i,1,1)*sv12(i)+glo2(i,1,2)*sv22(i)+glo2(i,1,3)*sv32(i)+glo2(i,1,4)*sv42(i) &
            + glo2(i,1,5)*sv52(i)+glo2(i,1,6)*sv62(i)+glo2(i,1,7)*sv72(i)+glo2(i,1,8)*sv82(i)
    hx3v(i) = glo2(i,1,1)*sv13(i)+glo2(i,1,2)*sv23(i)+glo2(i,1,3)*sv33(i)+glo2(i,1,4)*sv43(i) &
            + glo2(i,1,5)*sv53(i)+glo2(i,1,6)*sv63(i)+glo2(i,1,7)*sv73(i)+glo2(i,1,8)*sv83(i)
    hx4v(i) = glo2(i,1,1)*sv14(i)+glo2(i,1,2)*sv24(i)+glo2(i,1,3)*sv34(i)+glo2(i,1,4)*sv44(i) &
            + glo2(i,1,5)*sv54(i)+glo2(i,1,6)*sv64(i)+glo2(i,1,7)*sv74(i)+glo2(i,1,8)*sv84(i)

    hy1v(i) = glo2(i,2,1)*sv11(i)+glo2(i,2,2)*sv21(i)+glo2(i,2,3)*sv31(i)+glo2(i,2,4)*sv41(i) &
            + glo2(i,2,5)*sv51(i)+glo2(i,2,6)*sv61(i)+glo2(i,2,7)*sv71(i)+glo2(i,2,8)*sv81(i)
    hy2v(i) = glo2(i,2,1)*sv12(i)+glo2(i,2,2)*sv22(i)+glo2(i,2,3)*sv32(i)+glo2(i,2,4)*sv42(i) &
            + glo2(i,2,5)*sv52(i)+glo2(i,2,6)*sv62(i)+glo2(i,2,7)*sv72(i)+glo2(i,2,8)*sv82(i)
    hy3v(i) = glo2(i,2,1)*sv13(i)+glo2(i,2,2)*sv23(i)+glo2(i,2,3)*sv33(i)+glo2(i,2,4)*sv43(i) &
            + glo2(i,2,5)*sv53(i)+glo2(i,2,6)*sv63(i)+glo2(i,2,7)*sv73(i)+glo2(i,2,8)*sv83(i)
    hy4v(i) = glo2(i,2,1)*sv14(i)+glo2(i,2,2)*sv24(i)+glo2(i,2,3)*sv34(i)+glo2(i,2,4)*sv44(i) &
            + glo2(i,2,5)*sv54(i)+glo2(i,2,6)*sv64(i)+glo2(i,2,7)*sv74(i)+glo2(i,2,8)*sv84(i)

    hz1v(i) = glo2(i,3,1)*sv11(i)+glo2(i,3,2)*sv21(i)+glo2(i,3,3)*sv31(i)+glo2(i,3,4)*sv41(i) &
            + glo2(i,3,5)*sv51(i)+glo2(i,3,6)*sv61(i)+glo2(i,3,7)*sv71(i)+glo2(i,3,8)*sv81(i)
    hz2v(i) = glo2(i,3,1)*sv12(i)+glo2(i,3,2)*sv22(i)+glo2(i,3,3)*sv32(i)+glo2(i,3,4)*sv42(i) &
            + glo2(i,3,5)*sv52(i)+glo2(i,3,6)*sv62(i)+glo2(i,3,7)*sv72(i)+glo2(i,3,8)*sv82(i)
    hz3v(i) = glo2(i,3,1)*sv13(i)+glo2(i,3,2)*sv23(i)+glo2(i,3,3)*sv33(i)+glo2(i,3,4)*sv43(i) &
            + glo2(i,3,5)*sv53(i)+glo2(i,3,6)*sv63(i)+glo2(i,3,7)*sv73(i)+glo2(i,3,8)*sv83(i)
    hz4v(i) = glo2(i,3,1)*sv14(i)+glo2(i,3,2)*sv24(i)+glo2(i,3,3)*sv34(i)+glo2(i,3,4)*sv44(i) &
            + glo2(i,3,5)*sv54(i)+glo2(i,3,6)*sv64(i)+glo2(i,3,7)*sv74(i)+glo2(i,3,8)*sv84(i)
#ifndef FUSE_LOOPS
  enddo
#endif

  if (var1 > 0.0d0) then

    scal1 = .25d0*var1*gs2*var3
#ifndef FUSE_LOOPS
!$acc parallel loop gang vector async(1) private(scal2)
!$omp target teams distribute parallel do private(scal2)
    do i=1,vecn
#endif
      scal2 = px1(i)**2+px2(i)**2+px3(i)**2+px4(i)**2 &
           + py1(i)**2+py2(i)**2+py3(i)**2+py4(i)**2 &
           + pz1(i)**2+pz2(i)**2+pz3(i)**2+pz4(i)**2 &
           + px5(i)**2+px6(i)**2+px7(i)**2+px8(i)**2 &
           + py5(i)**2+py6(i)**2+py7(i)**2+py8(i)**2 &
           + pz5(i)**2+pz6(i)**2+pz7(i)**2+pz8(i)**2
      fac(i) = scal1*scal2/glo4(i)

      fff(1,i)  = fff(1,i)  + fac(i)*hx1v(i)
      fff(2,i)  = fff(2,i)  + fac(i)*hx2v(i)
      fff(3,i)  = fff(3,i)  + fac(i)*hx3v(i)
      fff(4,i)  = fff(4,i)  + fac(i)*hx4v(i)
      fff(5,i)  = fff(5,i)  + fac(i)*hy1v(i)
      fff(6,i)  = fff(6,i)  + fac(i)*hy2v(i)
      fff(7,i)  = fff(7,i)  + fac(i)*hy3v(i)
      fff(8,i)  = fff(8,i)  + fac(i)*hy4v(i)
      fff(9,i)  = fff(9,i)  + fac(i)*hz1v(i)
      fff(10,i) = fff(10,i) + fac(i)*hz2v(i)
      fff(11,i) = fff(11,i) + fac(i)*hz3v(i)
      fff(12,i) = fff(12,i) + fac(i)*hz4v(i)
#ifndef FUSE_LOOPS
    enddo
#endif
    if(var2 <= 0.0d0) then
#ifndef FUSE_LOOPS
!$acc parallel loop gang vector  async(1)
!$omp target teams distribute parallel do 
      do i=1,vecn
#endif
        hx1(i) = fff(1,i)
        hx2(i) = fff(2,i)
        hx3(i) = fff(3,i)
        hx4(i) = fff(4,i)
        hy1(i) = fff(5,i)
        hy2(i) = fff(6,i)
        hy3(i) = fff(7,i)
        hy4(i) = fff(8,i)
        hz1(i) = fff(9,i)
        hz2(i) = fff(10,i)
        hz3(i) = fff(11,i)
        hz4(i) = fff(12,i)
#ifndef FUSE_LOOPS
      enddo
#endif
    else
      scal1 = -0.25d0*var2*gs1
#ifndef FUSE_LOOPS
!$acc parallel loop gang vector  async(1)
!$omp target teams distribute parallel do 
      do i=1,vecn
#endif
        fac(i) = glo7(i)*scal1*glo3(i)*glo3(i)/glo5(i)
        
        hx1(i) = fac(i)*hx1v(i) + fff(1,i)
        hx2(i) = fac(i)*hx2v(i) + fff(2,i)
        hx3(i) = fac(i)*hx3v(i) + fff(3,i)
        hx4(i) = fac(i)*hx4v(i) + fff(4,i)
        hy1(i) = fac(i)*hy1v(i) + fff(5,i)
        hy2(i) = fac(i)*hy2v(i) + fff(6,i)
        hy3(i) = fac(i)*hy3v(i) + fff(7,i)
        hy4(i) = fac(i)*hy4v(i) + fff(8,i)
        hz1(i) = fac(i)*hz1v(i) + fff(9,i)
        hz2(i) = fac(i)*hz2v(i) + fff(10,i)
        hz3(i) = fac(i)*hz3v(i) + fff(11,i)
        hz4(i) = fac(i)*hz4v(i) + fff(12,i)
#ifndef FUSE_LOOPS
      enddo
#endif
    endif

  elseif(var2 > 0.0d0) then

    scal1 = -0.25d0*var2*gs1
#ifndef FUSE_LOOPS
!$acc parallel loop gang vector  async(1)
!$omp target teams distribute parallel do 
    do i=1,vecn
#endif
      fac(i) = glo7(i)*scal1*glo3(i)*glo3(i)/glo5(i)

      hx1(i) = fac(i)*hx1v(i)
      hx2(i) = fac(i)*hx2v(i)
      hx3(i) = fac(i)*hx3v(i)
      hx4(i) = fac(i)*hx4v(i)
      hy1(i) = fac(i)*hy1v(i)
      hy2(i) = fac(i)*hy2v(i)
      hy3(i) = fac(i)*hy3v(i)
      hy4(i) = fac(i)*hy4v(i)
      hz1(i) = fac(i)*hz1v(i)
      hz2(i) = fac(i)*hz2v(i)
      hz3(i) = fac(i)*hz3v(i)
      hz4(i) = fac(i)*hz4v(i)
#ifndef FUSE_LOOPS
    enddo
#endif

  endif

#ifndef FUSE_LOOPS
!$acc parallel loop gang vector  async(1) reduction(+:scal4)
!$omp target teams distribute parallel do  reduction(+:scal4)
  do i=1,vecn
#endif
    glo6(i,1,1) = -hx1(i)*sv11(i)-hx2(i)*sv12(i)-hx3(i)*sv13(i)-hx4(i)*sv14(i)
    glo6(i,1,2) = -hx1(i)*sv21(i)-hx2(i)*sv22(i)-hx3(i)*sv23(i)-hx4(i)*sv24(i)
    glo6(i,1,3) = -hx1(i)*sv31(i)-hx2(i)*sv32(i)-hx3(i)*sv33(i)-hx4(i)*sv34(i)
    glo6(i,1,4) = -hx1(i)*sv41(i)-hx2(i)*sv42(i)-hx3(i)*sv43(i)-hx4(i)*sv44(i)
    glo6(i,1,5) = -hx1(i)*sv51(i)-hx2(i)*sv52(i)-hx3(i)*sv53(i)-hx4(i)*sv54(i)
    glo6(i,1,6) = -hx1(i)*sv61(i)-hx2(i)*sv62(i)-hx3(i)*sv63(i)-hx4(i)*sv64(i)
    glo6(i,1,7) = -hx1(i)*sv71(i)-hx2(i)*sv72(i)-hx3(i)*sv73(i)-hx4(i)*sv74(i)
    glo6(i,1,8) = -hx1(i)*sv81(i)-hx2(i)*sv82(i)-hx3(i)*sv83(i)-hx4(i)*sv84(i)

    glo6(i,2,1) = -hy1(i)*sv11(i)-hy2(i)*sv12(i)-hy3(i)*sv13(i)-hy4(i)*sv14(i)
    glo6(i,2,2) = -hy1(i)*sv21(i)-hy2(i)*sv22(i)-hy3(i)*sv23(i)-hy4(i)*sv24(i)
    glo6(i,2,3) = -hy1(i)*sv31(i)-hy2(i)*sv32(i)-hy3(i)*sv33(i)-hy4(i)*sv34(i)
    glo6(i,2,4) = -hy1(i)*sv41(i)-hy2(i)*sv42(i)-hy3(i)*sv43(i)-hy4(i)*sv44(i)
    glo6(i,2,5) = -hy1(i)*sv51(i)-hy2(i)*sv52(i)-hy3(i)*sv53(i)-hy4(i)*sv54(i)
    glo6(i,2,6) = -hy1(i)*sv61(i)-hy2(i)*sv62(i)-hy3(i)*sv63(i)-hy4(i)*sv64(i)
    glo6(i,2,7) = -hy1(i)*sv71(i)-hy2(i)*sv72(i)-hy3(i)*sv73(i)-hy4(i)*sv74(i)
    glo6(i,2,8) = -hy1(i)*sv81(i)-hy2(i)*sv82(i)-hy3(i)*sv83(i)-hy4(i)*sv84(i)

    glo6(i,3,1) = -hz1(i)*sv11(i)-hz2(i)*sv12(i)-hz3(i)*sv13(i)-hz4(i)*sv14(i)
    glo6(i,3,2) = -hz1(i)*sv21(i)-hz2(i)*sv22(i)-hz3(i)*sv23(i)-hz4(i)*sv24(i)
    glo6(i,3,3) = -hz1(i)*sv31(i)-hz2(i)*sv32(i)-hz3(i)*sv33(i)-hz4(i)*sv34(i)
    glo6(i,3,4) = -hz1(i)*sv41(i)-hz2(i)*sv42(i)-hz3(i)*sv43(i)-hz4(i)*sv44(i)
    glo6(i,3,5) = -hz1(i)*sv51(i)-hz2(i)*sv52(i)-hz3(i)*sv53(i)-hz4(i)*sv54(i)
    glo6(i,3,6) = -hz1(i)*sv61(i)-hz2(i)*sv62(i)-hz3(i)*sv63(i)-hz4(i)*sv64(i)
    glo6(i,3,7) = -hz1(i)*sv71(i)-hz2(i)*sv72(i)-hz3(i)*sv73(i)-hz4(i)*sv74(i)
    glo6(i,3,8) = -hz1(i)*sv81(i)-hz2(i)*sv82(i)-hz3(i)*sv83(i)-hz4(i)*sv84(i)

    qqwk(i) = &
         hx1(i)*hx1v(i) + hx2(i)*hx2v(i) + hx3(i)*hx3v(i) + hx4(i)*hx4v(i) &
         + hy1(i)*hy1v(i) + hy2(i)*hy2v(i) + hy3(i)*hy3v(i) + hy4(i)*hy4v(i) &
         + hz1(i)*hz1v(i) + hz2(i)*hz2v(i) + hz3(i)*hz3v(i) + hz4(i)*hz4v(i)
    scal4 = scal4 + qqwk(i)
  enddo
  e_s = e_s - gs2*scal4
     
!$acc end data
!$omp end target data
end subroutine sub1_fuse

subroutine sub1_fuse_scalarize (fff,e_s,var1,var2,var3)

  use gvars, only: glo1,glo3, &
       px1,px2,px3,px4,px5,px6,px7,px8, &
       py1,py2,py3,py4,py5,py6,py7,py8, &
       pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
       glo2,glo4,glo6,glo5,glo7, &
       vecn,gs1,vecsize,gs2

  implicit none

  real(8) :: e_s,var1,var2,var3
  real(8),dimension(12,vecsize) :: fff

  integer :: i,j,k
  real(8) :: scal1,scal2,scal3,scal4
  real(8) :: fac,&
       sv11,sv21,sv31,sv41,sv51,sv61,sv71,sv81, &
       sv12,sv22,sv32,sv42,sv52,sv62,sv72,sv82, &
       sv13,sv23,sv33,sv43,sv53,sv63,sv73,sv83, &
       sv14,sv24,sv34,sv44,sv54,sv64,sv74,sv84, &
       qqx1,qqx2,qqx3,qqx4,qqy1,qqy2,qqy3,qqy4,qqz1,qqz2,qqz3,qqz4, &
       hx1,hx2,hx3,hx4,hy1,hy2,hy3,hy4,hz1,hz2,hz3,hz4, &
       hx1v,hx2v,hx3v,hx4v,hy1v,hy2v,hy3v,hy4v,hz1v,hz2v,hz3v,hz4v, &
       qqwk
  if(var1 <= 0.0d0 .and. var2 <= 0.0d0) then
!$acc parallel loop gang vector collapse(3) async(1)
!$omp target teams distribute parallel do collapse(3) 
    do k=1,8
      do j=1,3
        do i=1,vecn
          glo6(i,j,k) = 0.0d0
        enddo
      enddo
    enddo
    return
  endif

  scal4 = 0.0d0
#ifndef SCALARIZE
!$acc data  async(1) create( &
!$acc&      fac, &
!$acc&      sv11,sv21,sv31,sv41,sv51,sv61,sv71,sv81, &
!$acc&      sv12,sv22,sv32,sv42,sv52,sv62,sv72,sv82, &
!$acc&      sv13,sv23,sv33,sv43,sv53,sv63,sv73,sv83, &
!$acc&      sv14,sv24,sv34,sv44,sv54,sv64,sv74,sv84, &
!$acc&      qqx1,qqx2,qqx3,qqx4,qqy1,qqy2,qqy3,qqy4,qqz1,qqz2,qqz3,qqz4, &
!$acc&      hx1,hx2,hx3,hx4,hy1,hy2,hy3,hy4,hz1,hz2,hz3,hz4, &
!$acc&      hx1v,hx2v,hx3v,hx4v,hy1v,hy2v,hy3v,hy4v,hz1v,hz2v,hz3v,hz4v, &
!$acc&      qqwk)
!$omp target data  map(alloc: &
!$omp&      fac, &
!$omp&      sv11,sv21,sv31,sv41,sv51,sv61,sv71,sv81, &
!$omp&      sv12,sv22,sv32,sv42,sv52,sv62,sv72,sv82, &
!$omp&      sv13,sv23,sv33,sv43,sv53,sv63,sv73,sv83, &
!$omp&      sv14,sv24,sv34,sv44,sv54,sv64,sv74,sv84, &
!$omp&      qqx1,qqx2,qqx3,qqx4,qqy1,qqy2,qqy3,qqy4,qqz1,qqz2,qqz3,qqz4, &
!$omp&      hx1,hx2,hx3,hx4,hy1,hy2,hy3,hy4,hz1,hz2,hz3,hz4, &
!$omp&      hx1v,hx2v,hx3v,hx4v,hy1v,hy2v,hy3v,hy4v,hz1v,hz2v,hz3v,hz4v, &
!$omp&      qqwk)
#endif

!$acc parallel loop gang vector  async(1) &
#ifdef FUSE_LOOPS
!$acc& reduction(+:scal4) &
#endif
!$acc
!$omp target teams distribute parallel do &
#ifdef FUSE_LOOPS
!$omp& reduction(+:scal4) &
#endif
!$omp
  do i=1,vecn
    hx1 = glo1(i,1,3)-glo1(i,1,4)-glo1(i,1,7)+glo1(i,1,8)
    hx2 = glo1(i,1,2)-glo1(i,1,3)-glo1(i,1,5)+glo1(i,1,8)
    hx3 = glo1(i,1,1)-glo1(i,1,4)-glo1(i,1,6)+glo1(i,1,7)
    hx4 = glo1(i,1,1)-glo1(i,1,2)-glo1(i,1,5)+glo1(i,1,6)
    hy1 = glo1(i,2,3)-glo1(i,2,4)-glo1(i,2,7)+glo1(i,2,8)
    hy2 = glo1(i,2,2)-glo1(i,2,3)-glo1(i,2,5)+glo1(i,2,8)
    hy3 = glo1(i,2,1)-glo1(i,2,4)-glo1(i,2,6)+glo1(i,2,7)
    hy4 = glo1(i,2,1)-glo1(i,2,2)-glo1(i,2,5)+glo1(i,2,6)
    hz1 = glo1(i,3,3)-glo1(i,3,4)-glo1(i,3,7)+glo1(i,3,8)
    hz2 = glo1(i,3,2)-glo1(i,3,3)-glo1(i,3,5)+glo1(i,3,8)
    hz3 = glo1(i,3,1)-glo1(i,3,4)-glo1(i,3,6)+glo1(i,3,7)
    hz4 = glo1(i,3,1)-glo1(i,3,2)-glo1(i,3,5)+glo1(i,3,6)

    scal3  = 1.0d0/glo4(i)
    qqx1 = (hx3-hx2)*scal3
    qqx2 = (hx3+hx2)*scal3
    qqx3 = (hx4-hx1)*scal3
    qqx4 = (hx4+hx1)*scal3
    qqy1 = (hy3-hy2)*scal3
    qqy2 = (hy3+hy2)*scal3
    qqy3 = (hy4-hy1)*scal3
    qqy4 = (hy4+hy1)*scal3
    qqz1 = (hz3-hz2)*scal3
    qqz2 = (hz3+hz2)*scal3
    qqz3 = (hz4-hz1)*scal3
    qqz4 = (hz4+hz1)*scal3

    sv11 =  1.0d0-qqx1*px1(i)-qqy1*py1(i)-qqz1*pz1(i)
    sv21 = -1.0d0-qqx1*px2(i)-qqy1*py2(i)-qqz1*pz2(i)
    sv31 =  1.0d0-qqx1*px3(i)-qqy1*py3(i)-qqz1*pz3(i)
    sv41 = -1.0d0-qqx1*px4(i)-qqy1*py4(i)-qqz1*pz4(i)
    sv51 =  1.0d0-qqx1*px5(i)-qqy1*py5(i)-qqz1*pz5(i)
    sv61 = -1.0d0-qqx1*px6(i)-qqy1*py6(i)-qqz1*pz6(i)
    sv71 =  1.0d0-qqx1*px7(i)-qqy1*py7(i)-qqz1*pz7(i)
    sv81 = -1.0d0-qqx1*px8(i)-qqy1*py8(i)-qqz1*pz8(i)

    sv12 =  1.0d0-qqx2*px1(i)-qqy2*py1(i)-qqz2*pz1(i)
    sv22 =  1.0d0-qqx2*px2(i)-qqy2*py2(i)-qqz2*pz2(i)
    sv32 = -1.0d0-qqx2*px3(i)-qqy2*py3(i)-qqz2*pz3(i)
    sv42 = -1.0d0-qqx2*px4(i)-qqy2*py4(i)-qqz2*pz4(i)
    sv52 = -1.0d0-qqx2*px5(i)-qqy2*py5(i)-qqz2*pz5(i)
    sv62 = -1.0d0-qqx2*px6(i)-qqy2*py6(i)-qqz2*pz6(i)
    sv72 =  1.0d0-qqx2*px7(i)-qqy2*py7(i)-qqz2*pz7(i)
    sv82 =  1.0d0-qqx2*px8(i)-qqy2*py8(i)-qqz2*pz8(i)

    sv13 =  1.0d0-qqx3*px1(i)-qqy3*py1(i)-qqz3*pz1(i)
    sv23 = -1.0d0-qqx3*px2(i)-qqy3*py2(i)-qqz3*pz2(i)
    sv33 = -1.0d0-qqx3*px3(i)-qqy3*py3(i)-qqz3*pz3(i)
    sv43 =  1.0d0-qqx3*px4(i)-qqy3*py4(i)-qqz3*pz4(i)
    sv53 = -1.0d0-qqx3*px5(i)-qqy3*py5(i)-qqz3*pz5(i)
    sv63 =  1.0d0-qqx3*px6(i)-qqy3*py6(i)-qqz3*pz6(i)
    sv73 =  1.0d0-qqx3*px7(i)-qqy3*py7(i)-qqz3*pz7(i)
    sv83 = -1.0d0-qqx3*px8(i)-qqy3*py8(i)-qqz3*pz8(i)

    sv14 =  1.0d0-qqx4*px1(i)-qqy4*py1(i)-qqz4*pz1(i)
    sv24 = -1.0d0-qqx4*px2(i)-qqy4*py2(i)-qqz4*pz2(i)
    sv34 =  1.0d0-qqx4*px3(i)-qqy4*py3(i)-qqz4*pz3(i)
    sv44 = -1.0d0-qqx4*px4(i)-qqy4*py4(i)-qqz4*pz4(i)
    sv54 = -1.0d0-qqx4*px5(i)-qqy4*py5(i)-qqz4*pz5(i)
    sv64 =  1.0d0-qqx4*px6(i)-qqy4*py6(i)-qqz4*pz6(i)
    sv74 = -1.0d0-qqx4*px7(i)-qqy4*py7(i)-qqz4*pz7(i)
    sv84 =  1.0d0-qqx4*px8(i)-qqy4*py8(i)-qqz4*pz8(i)

    hx1v = glo2(i,1,1)*sv11+glo2(i,1,2)*sv21+glo2(i,1,3)*sv31+glo2(i,1,4)*sv41 &
            + glo2(i,1,5)*sv51+glo2(i,1,6)*sv61+glo2(i,1,7)*sv71+glo2(i,1,8)*sv81
    hx2v = glo2(i,1,1)*sv12+glo2(i,1,2)*sv22+glo2(i,1,3)*sv32+glo2(i,1,4)*sv42 &
            + glo2(i,1,5)*sv52+glo2(i,1,6)*sv62+glo2(i,1,7)*sv72+glo2(i,1,8)*sv82
    hx3v = glo2(i,1,1)*sv13+glo2(i,1,2)*sv23+glo2(i,1,3)*sv33+glo2(i,1,4)*sv43 &
            + glo2(i,1,5)*sv53+glo2(i,1,6)*sv63+glo2(i,1,7)*sv73+glo2(i,1,8)*sv83
    hx4v = glo2(i,1,1)*sv14+glo2(i,1,2)*sv24+glo2(i,1,3)*sv34+glo2(i,1,4)*sv44 &
            + glo2(i,1,5)*sv54+glo2(i,1,6)*sv64+glo2(i,1,7)*sv74+glo2(i,1,8)*sv84

    hy1v = glo2(i,2,1)*sv11+glo2(i,2,2)*sv21+glo2(i,2,3)*sv31+glo2(i,2,4)*sv41 &
            + glo2(i,2,5)*sv51+glo2(i,2,6)*sv61+glo2(i,2,7)*sv71+glo2(i,2,8)*sv81
    hy2v = glo2(i,2,1)*sv12+glo2(i,2,2)*sv22+glo2(i,2,3)*sv32+glo2(i,2,4)*sv42 &
            + glo2(i,2,5)*sv52+glo2(i,2,6)*sv62+glo2(i,2,7)*sv72+glo2(i,2,8)*sv82
    hy3v = glo2(i,2,1)*sv13+glo2(i,2,2)*sv23+glo2(i,2,3)*sv33+glo2(i,2,4)*sv43 &
            + glo2(i,2,5)*sv53+glo2(i,2,6)*sv63+glo2(i,2,7)*sv73+glo2(i,2,8)*sv83
    hy4v = glo2(i,2,1)*sv14+glo2(i,2,2)*sv24+glo2(i,2,3)*sv34+glo2(i,2,4)*sv44 &
            + glo2(i,2,5)*sv54+glo2(i,2,6)*sv64+glo2(i,2,7)*sv74+glo2(i,2,8)*sv84

    hz1v = glo2(i,3,1)*sv11+glo2(i,3,2)*sv21+glo2(i,3,3)*sv31+glo2(i,3,4)*sv41 &
            + glo2(i,3,5)*sv51+glo2(i,3,6)*sv61+glo2(i,3,7)*sv71+glo2(i,3,8)*sv81
    hz2v = glo2(i,3,1)*sv12+glo2(i,3,2)*sv22+glo2(i,3,3)*sv32+glo2(i,3,4)*sv42 &
            + glo2(i,3,5)*sv52+glo2(i,3,6)*sv62+glo2(i,3,7)*sv72+glo2(i,3,8)*sv82
    hz3v = glo2(i,3,1)*sv13+glo2(i,3,2)*sv23+glo2(i,3,3)*sv33+glo2(i,3,4)*sv43 &
            + glo2(i,3,5)*sv53+glo2(i,3,6)*sv63+glo2(i,3,7)*sv73+glo2(i,3,8)*sv83
    hz4v = glo2(i,3,1)*sv14+glo2(i,3,2)*sv24+glo2(i,3,3)*sv34+glo2(i,3,4)*sv44 &
            + glo2(i,3,5)*sv54+glo2(i,3,6)*sv64+glo2(i,3,7)*sv74+glo2(i,3,8)*sv84
#ifndef FUSE_LOOPS
  enddo
#endif

  if (var1 > 0.0d0) then

    scal1 = .25d0*var1*gs2*var3
#ifndef FUSE_LOOPS
!$acc parallel loop gang vector async(1) private(scal2)
!$omp target teams distribute parallel do private(scal2)
    do i=1,vecn
#endif
      scal2 = px1(i)**2+px2(i)**2+px3(i)**2+px4(i)**2 &
           + py1(i)**2+py2(i)**2+py3(i)**2+py4(i)**2 &
           + pz1(i)**2+pz2(i)**2+pz3(i)**2+pz4(i)**2 &
           + px5(i)**2+px6(i)**2+px7(i)**2+px8(i)**2 &
           + py5(i)**2+py6(i)**2+py7(i)**2+py8(i)**2 &
           + pz5(i)**2+pz6(i)**2+pz7(i)**2+pz8(i)**2
      fac = scal1*scal2/glo4(i)

      fff(1,i)  = fff(1,i)  + fac*hx1v
      fff(2,i)  = fff(2,i)  + fac*hx2v
      fff(3,i)  = fff(3,i)  + fac*hx3v
      fff(4,i)  = fff(4,i)  + fac*hx4v
      fff(5,i)  = fff(5,i)  + fac*hy1v
      fff(6,i)  = fff(6,i)  + fac*hy2v
      fff(7,i)  = fff(7,i)  + fac*hy3v
      fff(8,i)  = fff(8,i)  + fac*hy4v
      fff(9,i)  = fff(9,i)  + fac*hz1v
      fff(10,i) = fff(10,i) + fac*hz2v
      fff(11,i) = fff(11,i) + fac*hz3v
      fff(12,i) = fff(12,i) + fac*hz4v
#ifndef FUSE_LOOPS
    enddo
#endif
    if(var2 <= 0.0d0) then
#ifndef FUSE_LOOPS
!$acc parallel loop gang vector  async(1)
!$omp target teams distribute parallel do 
      do i=1,vecn
#endif
        hx1 = fff(1,i)
        hx2 = fff(2,i)
        hx3 = fff(3,i)
        hx4 = fff(4,i)
        hy1 = fff(5,i)
        hy2 = fff(6,i)
        hy3 = fff(7,i)
        hy4 = fff(8,i)
        hz1 = fff(9,i)
        hz2 = fff(10,i)
        hz3 = fff(11,i)
        hz4 = fff(12,i)
#ifndef FUSE_LOOPS
      enddo
#endif
    else
      scal1 = -0.25d0*var2*gs1
#ifndef FUSE_LOOPS
!$acc parallel loop gang vector  async(1)
!$omp target teams distribute parallel do 
      do i=1,vecn
#endif
        fac = glo7(i)*scal1*glo3(i)*glo3(i)/glo5(i)
        
        hx1 = fac*hx1v + fff(1,i)
        hx2 = fac*hx2v + fff(2,i)
        hx3 = fac*hx3v + fff(3,i)
        hx4 = fac*hx4v + fff(4,i)
        hy1 = fac*hy1v + fff(5,i)
        hy2 = fac*hy2v + fff(6,i)
        hy3 = fac*hy3v + fff(7,i)
        hy4 = fac*hy4v + fff(8,i)
        hz1 = fac*hz1v + fff(9,i)
        hz2 = fac*hz2v + fff(10,i)
        hz3 = fac*hz3v + fff(11,i)
        hz4 = fac*hz4v + fff(12,i)
#ifndef FUSE_LOOPS
      enddo
#endif
    endif

  elseif(var2 > 0.0d0) then

    scal1 = -0.25d0*var2*gs1
#ifndef FUSE_LOOPS
!$acc parallel loop gang vector  async(1)
!$omp target teams distribute parallel do 
    do i=1,vecn
#endif
      fac = glo7(i)*scal1*glo3(i)*glo3(i)/glo5(i)

      hx1 = fac*hx1v
      hx2 = fac*hx2v
      hx3 = fac*hx3v
      hx4 = fac*hx4v
      hy1 = fac*hy1v
      hy2 = fac*hy2v
      hy3 = fac*hy3v
      hy4 = fac*hy4v
      hz1 = fac*hz1v
      hz2 = fac*hz2v
      hz3 = fac*hz3v
      hz4 = fac*hz4v
#ifndef FUSE_LOOPS
    enddo
#endif

  endif

#ifndef FUSE_LOOPS
!$acc parallel loop gang vector  async(1) reduction(+:scal4)
!$omp target teams distribute parallel do  reduction(+:scal4)
  do i=1,vecn
#endif
    glo6(i,1,1) = -hx1*sv11-hx2*sv12-hx3*sv13-hx4*sv14
    glo6(i,1,2) = -hx1*sv21-hx2*sv22-hx3*sv23-hx4*sv24
    glo6(i,1,3) = -hx1*sv31-hx2*sv32-hx3*sv33-hx4*sv34
    glo6(i,1,4) = -hx1*sv41-hx2*sv42-hx3*sv43-hx4*sv44
    glo6(i,1,5) = -hx1*sv51-hx2*sv52-hx3*sv53-hx4*sv54
    glo6(i,1,6) = -hx1*sv61-hx2*sv62-hx3*sv63-hx4*sv64
    glo6(i,1,7) = -hx1*sv71-hx2*sv72-hx3*sv73-hx4*sv74
    glo6(i,1,8) = -hx1*sv81-hx2*sv82-hx3*sv83-hx4*sv84

    glo6(i,2,1) = -hy1*sv11-hy2*sv12-hy3*sv13-hy4*sv14
    glo6(i,2,2) = -hy1*sv21-hy2*sv22-hy3*sv23-hy4*sv24
    glo6(i,2,3) = -hy1*sv31-hy2*sv32-hy3*sv33-hy4*sv34
    glo6(i,2,4) = -hy1*sv41-hy2*sv42-hy3*sv43-hy4*sv44
    glo6(i,2,5) = -hy1*sv51-hy2*sv52-hy3*sv53-hy4*sv54
    glo6(i,2,6) = -hy1*sv61-hy2*sv62-hy3*sv63-hy4*sv64
    glo6(i,2,7) = -hy1*sv71-hy2*sv72-hy3*sv73-hy4*sv74
    glo6(i,2,8) = -hy1*sv81-hy2*sv82-hy3*sv83-hy4*sv84

    glo6(i,3,1) = -hz1*sv11-hz2*sv12-hz3*sv13-hz4*sv14
    glo6(i,3,2) = -hz1*sv21-hz2*sv22-hz3*sv23-hz4*sv24
    glo6(i,3,3) = -hz1*sv31-hz2*sv32-hz3*sv33-hz4*sv34
    glo6(i,3,4) = -hz1*sv41-hz2*sv42-hz3*sv43-hz4*sv44
    glo6(i,3,5) = -hz1*sv51-hz2*sv52-hz3*sv53-hz4*sv54
    glo6(i,3,6) = -hz1*sv61-hz2*sv62-hz3*sv63-hz4*sv64
    glo6(i,3,7) = -hz1*sv71-hz2*sv72-hz3*sv73-hz4*sv74
    glo6(i,3,8) = -hz1*sv81-hz2*sv82-hz3*sv83-hz4*sv84

    qqwk = &
         hx1*hx1v + hx2*hx2v + hx3*hx3v + hx4*hx4v &
         + hy1*hy1v + hy2*hy2v + hy3*hy3v + hy4*hy4v &
         + hz1*hz1v + hz2*hz2v + hz3*hz3v + hz4*hz4v
    scal4 = scal4 + qqwk
  enddo
  e_s = e_s - gs2*scal4
     
#ifndef SCALARIZE
!$acc end data
!$omp end target data
#endif
end subroutine sub1_fuse_scalarize

subroutine init (fff,e_s,var1,var2,var3)

  use gvars, only: glo1,glo3, &
       px1,px2,px3,px4,px5,px6,px7,px8, &
       py1,py2,py3,py4,py5,py6,py7,py8, &
       pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
       glo2,glo4,glo6,glo5,glo7, &
       vecn,gs1,vecsize,gs2

  implicit none

  real(8) :: e_s,var1,var2,var3
  real(8),dimension(12,vecsize) :: fff

  fff=1.0d0
  e_s = 0.0d0
  var1 = 1.0d0
  var2 = 1.0d0
  var3 = 1.0d0

  glo1(:,:,1)=-1
  glo1(:,:,2)=1
  glo1(:,:,3)=-2
  glo1(:,:,4)=2
  glo1(:,:,5)=-3
  glo1(:,:,6)=3
  glo1(:,:,7)=-4
  glo1(:,:,8)=4

  glo3 = 1.0d0
  px1 = 1.0d0
  px2 = 1.0d0
  px3 = 1.0d0
  px4 = 1.0d0
  px5 = 1.0d0
  px6 = 1.0d0
  px7 = 1.0d0
  px8 = 1.0d0
  py1 = 1.0d0
  py2 = 1.0d0
  py3 = 1.0d0
  py4 = 1.0d0
  py5 = 1.0d0
  py6 = 1.0d0
  py7 = 1.0d0
  py8 = 1.0d0
  pz1 = 1.0d0
  pz2 = 1.0d0
  pz3 = 1.0d0
  pz4 = 1.0d0
  pz5 = 1.0d0
  pz6 = 1.0d0
  pz7 = 1.0d0
  pz8 = 1.0d0

  glo2 = 1.0d0
  glo4 = 1.0d0
  glo6 = 1.0d0
  glo5 = 1.0d0
  glo7 = 1.0d0
  gs1 = 1.0d0
  gs2 = 1.0d0
end subroutine init
   
program omptest

  use gvars, only: glo1,glo3, &
       px1,px2,px3,px4,px5,px6,px7,px8, &
       py1,py2,py3,py4,py5,py6,py7,py8, &
       pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
       glo2,glo4,glo6,glo5,glo7, &
       vecn,gs1,vecsize,gs2

  implicit none

  real(8) :: e_s,var1,var2,var3
  real(8),dimension(12,vecsize) :: fff

  real(8) :: answer

  vecn = vecsize
  answer = -4611168d0*vecn

  call init (fff,e_s,var1,var2,var3)

!$acc enter data create(fff, &
!$acc& glo1,glo3, &
!$acc& px1,px2,px3,px4,px5,px6,px7,px8, &
!$acc& py1,py2,py3,py4,py5,py6,py7,py8, &
!$acc& pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
!$acc& glo2,glo4,glo6,glo5,glo7)

!$acc update device(fff, &
!$acc& glo1,glo3, &
!$acc& px1,px2,px3,px4,px5,px6,px7,px8, &
!$acc& py1,py2,py3,py4,py5,py6,py7,py8, &
!$acc& pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
!$acc& glo2,glo4,glo6,glo5,glo7)

!$omp target enter data map(alloc: fff, &
!$omp& glo1,glo3, &
!$omp& px1,px2,px3,px4,px5,px6,px7,px8, &
!$omp& py1,py2,py3,py4,py5,py6,py7,py8, &
!$omp& pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
!$omp& glo2,glo4,glo6,glo5,glo7)

!$omp target update to(fff, &
!$omp& glo1,glo3, &
!$omp& px1,px2,px3,px4,px5,px6,px7,px8, &
!$omp& py1,py2,py3,py4,py5,py6,py7,py8, &
!$omp& pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
!$omp& glo2,glo4,glo6,glo5,glo7)

  call sub1 (fff,e_s,var1,var2,var3)

  print *, e_s

  if (e_s == answer) then
    print *,'sub1 passed'
  else
    print *,'sub1 failed'
  endif

  call init (fff,e_s,var1,var2,var3)
!$acc update device(fff, &
!$acc& glo1,glo3, &
!$acc& px1,px2,px3,px4,px5,px6,px7,px8, &
!$acc& py1,py2,py3,py4,py5,py6,py7,py8, &
!$acc& pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
!$acc& glo2,glo4,glo6,glo5,glo7)

!$omp target update to(fff, &
!$omp& glo1,glo3, &
!$omp& px1,px2,px3,px4,px5,px6,px7,px8, &
!$omp& py1,py2,py3,py4,py5,py6,py7,py8, &
!$omp& pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
!$omp& glo2,glo4,glo6,glo5,glo7)

  call sub1_fuse (fff,e_s,var1,var2,var3)

  print *, e_s

  if (e_s == answer) then
    print *,'sub1_fuse passed'
  else
    print *,'sub1_fuse failed'
  endif

  call init (fff,e_s,var1,var2,var3)
!$acc update device(fff, &
!$acc& glo1,glo3, &
!$acc& px1,px2,px3,px4,px5,px6,px7,px8, &
!$acc& py1,py2,py3,py4,py5,py6,py7,py8, &
!$acc& pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
!$acc& glo2,glo4,glo6,glo5,glo7)

!$omp target update to(fff, &
!$omp& glo1,glo3, &
!$omp& px1,px2,px3,px4,px5,px6,px7,px8, &
!$omp& py1,py2,py3,py4,py5,py6,py7,py8, &
!$omp& pz1,pz2,pz3,pz4,pz5,pz6,pz7,pz8, &
!$omp& glo2,glo4,glo6,glo5,glo7)

  call sub1_fuse_scalarize (fff,e_s,var1,var2,var3)

  print *, e_s

  if (e_s == answer) then
    print *,'sub1_fuse_scalarize passed'
  else
    print *,'sub1_fuse_scalarize failed'
  endif

end program omptest

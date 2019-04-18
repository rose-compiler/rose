subroutine sub1 (fff,e_s,var1,var2,var3)
  implicit none
     
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
     
end subroutine sub1

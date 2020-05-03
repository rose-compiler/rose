module trans
  implicit none
  integer,parameter :: max_size = 16*1024
contains
  
  subroutine sub_orig(N)
    integer N

    common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
    real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8

    common /flags/ f1,f2
    logical f1,f2

    integer i
    real,dimension(N) :: t1,t2,t3,t4

!$acc data present(c1,c2,c3,c4,c5,c6,c7,c8,f1,f2) create(t1,t2,t3,t4)

!$acc parallel loop gang vector if(flags%f1) async(1) &
    do i=1,N
      t1(i)=c1(i)*c2(i)
      t2(i)=c3(i)*c4(i)
      t3(i)=t1(i)*t2(i)+c6(i)
      t4(i)=c1(i)-c2(i)+t3(i)
      c5(i)=t1(i)+t2(i)+c7(i)
    enddo

    call sub1_orig(N)

    if (f1) then
!$acc parallel loop async
      do i=1,N
        c4(i)=t1(i)-t2(i)
        c5(i)=t1(i)
      enddo
    else
!$acc parallel loop async
      do i=1,N
        c4(i)=c6(i)+t3(i)
        c5(i)=c7(i)+t1(i)*t4(i)
      enddo
    endif

!$acc end data
    
  end subroutine sub_orig
  
  subroutine sub1_orig(N)
    integer N

    common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
    real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8

    common /flags/ f1,f2
    logical f1,f2

    integer i
    real,dimension(N) :: t5,t6

!$acc data present(c1,c2,c3,c4,c5,c6,c7,c8,f1,f2) create(t5,t6)

!$acc parallel loop async
    do i=1,N
      t5(i)=c3(i)+c4(i)
      c6(i)=t5(i)+c1(i)
      t6(i)=c8(i)*t5(i)
    enddo
    if (f2) then
!$acc parallel loop async
      do i=1,N
        c3(i)=t5(i)*t6(i)+c1(i)
      enddo
    else
!$acc parallel loop async
      do i=1,N
        c3(i)=c2(i)+t6(i)-c8(i)
      enddo
    endif
!$acc parallel loop async
    do i=1,N
      c5(i)=t5(i)+c1(i)+c8(i)
    enddo

!$acc end data

  end subroutine sub1_orig

  subroutine sub_inline(N)
    integer N

    common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
    real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8

    common /flags/ f1,f2
    logical f1,f2

    integer i
    real,dimension(N) :: t1,t2,t3,t4
    real,dimension(N) :: t5,t6

!$acc data present(c1,c2,c3,c4,c5,c6,c7,c8,f1,f2) create(t1,t2,t3,t4,t5,t6)

!$acc parallel loop async
    do i=1,N
      t1(i)=c1(i)*c2(i)
      t2(i)=c3(i)*c4(i)
      t3(i)=t1(i)*t2(i)+c6(i)
      t4(i)=c1(i)-c2(i)+t3(i)
      c5(i)=t1(i)+t2(i)+c7(i)
    enddo

    ! sub1_orig inlined
!$acc parallel loop async
    do i=1,N
      t5(i)=c3(i)+c4(i)
      c6(i)=t5(i)+c1(i)
      t6(i)=c8(i)*t5(i)
    enddo

    if (f2) then
!$acc parallel loop async
      do i=1,N
        c3(i)=t5(i)*t6(i)+c1(i)
      enddo
    else
!$acc parallel loop async
      do i=1,N
        c3(i)=c2(i)+t6(i)-c8(i)
      enddo
    endif
!$acc parallel loop async
    do i=1,N
      c5(i)=t5(i)+c1(i)+c8(i)
    enddo
    ! end sub1_orig inlined

    if (f1) then
!$acc parallel loop async
      do i=1,N
        c4(i)=t1(i)-t2(i)
        c5(i)=t1(i)
      enddo
    else
!$acc parallel loop async
      do i=1,N
        c4(i)=c6(i)+t3(i)
        c5(i)=c7(i)+t1(i)*t4(i)
      enddo
    endif

!$acc end data
    
  end subroutine sub_inline
    
  subroutine sub_inline_fused(N)
    integer N

    common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
    real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8

    common /flags/ f1,f2
    logical f1,f2

    integer i
    real,dimension(N) :: t1,t2,t3,t4
    real,dimension(N) :: t5,t6

!$acc data present(c1,c2,c3,c4,c5,c6,c7,c8,f1,f2) create(t1,t2,t3,t4,t5,t6)

!$acc parallel loop async
    do i=1,N
      t1(i)=c1(i)*c2(i)
      t2(i)=c3(i)*c4(i)
      t3(i)=t1(i)*t2(i)+c6(i)
      t4(i)=c1(i)-c2(i)+t3(i)
      c5(i)=t1(i)+t2(i)+c7(i)

    ! sub1_orig inlined
      t5(i)=c3(i)+c4(i)
      c6(i)=t5(i)+c1(i)
      t6(i)=c8(i)*t5(i)
      if (f2) then
        c3(i)=t5(i)*t6(i)+c1(i)
      else
        c3(i)=c2(i)+t6(i)-c8(i)
      endif
      c5(i)=t5(i)+c1(i)+c8(i)
    ! end sub1_orig inlined

      if (f1) then
        c4(i)=t1(i)-t2(i)
        c5(i)=t1(i)
      else
        c4(i)=c6(i)+t3(i)
        c5(i)=c7(i)+t1(i)*t4(i)
      endif
    
    enddo
!$acc end data

  end subroutine sub_inline_fused
    
  subroutine sub_inline_fused_scalarized(N)
    integer N

    common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
    real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8

    common /flags/ f1,f2
    logical f1,f2

    integer i
    real              :: t1,t2,t3,t4
    real              :: t5,t6

!$acc data present(c1,c2,c3,c4,c5,c6,c7,c8,f1,f2)

!$acc parallel loop private(t1,t2,t3,t4,t5,t6) async
    do i=1,N
      t1=c1(i)*c2(i)
      t2=c3(i)*c4(i)
      t3=t1*t2+c6(i)
      t4=c1(i)-c2(i)+t3
      c5(i)=t1+t2+c7(i)

    ! sub1_orig inlined
      t5=c3(i)+c4(i)
      c6(i)=t5+c1(i)
      t6=c8(i)*t5
      if (f2) then
        c3(i)=t5*t6+c1(i)
      else
        c3(i)=c2(i)+t6-c8(i)
      endif
      c5(i)=t5+c1(i)+c8(i)
    ! end sub1_orig inlined

      if (f1) then
        c4(i)=t1-t2
        c5(i)=t1
      else
        c4(i)=c6(i)+t3
        c5(i)=c7(i)+t1*t4
      endif
    
    enddo

!$acc end data

  end subroutine sub_inline_fused_scalarized
    
  subroutine sub_fused(N)
    integer N

    common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
    real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8

    common /flags/ f1,f2
    logical f1,f2

    integer i
    real,dimension(N) :: t1,t2,t3,t4

    do i=1,N
      t1(i)=c1(i)*c2(i)
      t2(i)=c3(i)*c4(i)
      t3(i)=t1(i)*t2(i)+c6(i)
      t4(i)=c1(i)-c2(i)+t3(i)
      c5(i)=t1(i)+t2(i)+c7(i)
    enddo

    call sub1_fused(N)
    
    do i=1,N
      if (f1) then
        c4(i)=t1(i)-t2(i)
        c5(i)=t1(i)
      else
        c4(i)=c6(i)+t3(i)
        c5(i)=c7(i)+t1(i)*t4(i)
      endif
    enddo
    
  end subroutine sub_fused
  
  subroutine sub1_fused(N)
    integer N

    common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
    real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8

    common /flags/ f1,f2
    logical f1,f2

    integer i
    real,dimension(N) :: t5,t6
    do i=1,N
      t5(i)=c3(i)+c4(i)
      c6(i)=t5(i)+c1(i)
      t6(i)=c8(i)*t5(i)
      if (f2) then
        c3(i)=t5(i)*t6(i)+c1(i)
      else
        c3(i)=c2(i)+t6(i)-c8(i)
      endif
      c5(i)=t5(i)+c1(i)+c8(i)
    enddo

  end subroutine sub1_fused

  subroutine sub_fused_scalarized(N)
    integer N

    common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
    real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8

    common /flags/ f1,f2
    logical f1,f2

    integer i
    real,dimension(N) :: t1,t2,t3,t4

    do i=1,N
      t1(i)=c1(i)*c2(i)
      t2(i)=c3(i)*c4(i)
      t3(i)=t1(i)*t2(i)+c6(i)
      t4(i)=c1(i)-c2(i)+t3(i)
      c5(i)=t1(i)+t2(i)+c7(i)
    enddo

    call sub1_fused_scalarized(N)
    
    do i=1,N
      if (f1) then
        c4(i)=t1(i)-t2(i)
        c5(i)=t1(i)
      else
        c4(i)=c6(i)+t3(i)
        c5(i)=c7(i)+t1(i)*t4(i)
      endif
    enddo
    
  end subroutine sub_fused_scalarized
  
  subroutine sub1_fused_scalarized(N)
    integer N

    common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
    real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8

    common /flags/ f1,f2
    logical f1,f2

    integer i
    real            :: t5,t6
    do i=1,N
      t5=c3(i)+c4(i)
      c6(i)=t5+c1(i)
      t6=c8(i)*t5
      if (f2) then
        c3(i)=t5*t6+c1(i)
      else
        c3(i)=c2(i)+t6-c8(i)
      endif
      c5(i)=t5+c1(i)+c8(i)
    enddo

  end subroutine sub1_fused_scalarized

end module trans

program test
  
  use trans
  implicit none
    
  common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
  real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8
  
  common /flags/ f1,f2
  logical f1,f2
  
  integer i


  c1(:)=0.0d0
  c2(:)=0.0d0
  c3(:)=0.0d0
  c4(:)=0.0d0
  c5(:)=0.0d0
  c6(:)=0.0d0
  c7(:)=0.0d0
  c8(:)=0.0d0

  f1=.true.
  f2=.true.

!$acc enter data copyin(c1,c2,c3,c4,c5,c6,c7,c8,f1,f2)

  do i=1,2
    call sub_orig(max_size)
    call sub_inline(max_size)
    call sub_inline_fused(max_size)
    call sub_inline_fused_scalarized(max_size)
  enddo

end program test

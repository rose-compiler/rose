! to compile using pgi : pgfortran -acc -c thisfile.F90
module transv2
  implicit none
  integer,parameter :: max_size = 16*1024

  type mytype
      logical:: flag
      real*8 :: a(3)
  end type mytype

contains

  subroutine sub_inline_fused(N)
    integer N

    common /arrays/c1,c2,c3,c4,c5,c6,c7,c8
    real,dimension(max_size) :: c1,c2,c3,c4,c5,c6,c7,c8

    common /flags/ f1,f2
    logical f1,f2
    type (mytype) var

    integer i
    real,dimension(N) :: t1,t2,t3,t4
    real,dimension(N) :: t5,t6

!  !$acc data present(c1,c2,c3,c4,c5,c6,c7,c8,f1,f2) create(t1,t2,t3,t4,t5,t6)
!$acc parallel loop gang vector if(var%flag) async(1) &
!$acc& present(c1,c2,c3,c4,c5,&
!$acc& c6,c7,c8,f1,f2)
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
! !$acc end data

  end subroutine sub_inline_fused
end module transv2


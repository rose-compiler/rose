module badcomment
! OK 1
  implicit none
! OK 2
contains
! OK 3
  subroutine suba
! OK 4
    implicit none

! OK 5
    write(13,fmt='( &
         &3x,"Damping work: ")')
    write(13,fmt='( &
         &3x,"Friction work: ")')
! BAD 1 This comment comes one line later
  end subroutine suba

  subroutine subb
! BAD 2 This comment comes one line later
    implicit none
! BAD 3 This comment comes one line later
    integer :: ii,jj,j

!$acc OK 6 parallel loop gang vector async(1) default(present) &
!$acc& BAD 4 one line later private(vd) reduction(+:dwork,fwork)
    iiloop1: do ii=1,10
      jj = j
    enddo iiloop1
! BAD 5 This comment comes one line later
  end subroutine subb
! BAD 6 This line is missing
end module badcomment

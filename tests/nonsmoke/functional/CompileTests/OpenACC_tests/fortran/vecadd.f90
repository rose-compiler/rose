module mpoint
type point
integer :: n
real, dimension(:), allocatable :: x, y, z
end type
contains

subroutine move_to_device( A )
type(point) :: A
!$acc enter data copyin(A)
!$acc enter data create(A%x, A%y, A%z)
end subroutine

subroutine move_off_device( A )
type(point) :: A
!$acc exit data copyout(A%x, A%y, A%z)
!$acc exit data delete(A)
end subroutine
end module

subroutine vecaddgpu( A, base )
use mpoint
type(point) :: A
real :: base(:)
integer :: i
!$acc parallel loop present(A,base)
do i = 1, n
A%x(i) = base(i)
A%y(i) = sqrt( base(i) )
A%z(i) = 0
enddo
end subroutine

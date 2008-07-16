type node    ! define a "recursive" type
   integer :: value = 0
   type(node), pointer :: next_node => null()
end type node

type(node), target :: head  ! automatically initialized
type(node), pointer :: current, temp ! declare pointers
integer :: ioem, k

current => head  ! current point to head of list
do 
   read(*,*,iostat=ioem) k   ! Read next value, if any
   if(ioem /= 0) exit
   allocate(temp)            ! create new cell each iteration
   temp%value = k            ! assign value to cell
   current%next_node => temp ! attach new cell to list
   current => temp           ! current points to new end of list
end do

current => head
do
   if(.not. associated (current%next_node)) exit
   current => current%next_node
   write (*,*) current%value
end do

end


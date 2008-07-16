! this is the first (and only) example from C.3.2
type :: list
   type(list), pointer :: next=>null()
   class(*), allocatable :: item
end type list

type(list), pointer :: in_list, list_copy =>null()
type(list), pointer :: in_walk, new_tail

if(associated(in_list)) then
   in_walk => in_list
   allocate(list_copy)
   new_tail => list_copy
   do
      allocate(new_tail%item, source=in_walk%item)
      in_walk => in_walk%next
      if(.not. associated(in_walk)) exit
      allocate(new_tail%next)
      new_tail => new_tail%next
   end do
end if

end

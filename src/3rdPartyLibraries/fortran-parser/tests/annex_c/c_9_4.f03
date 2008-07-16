module callback_list_module
!  
! Type for users to extend with their own data, if they so desire
!
abstract interface
   subroutine callback_procedure(data)
     import callback_data
     class(callback_data), optional :: data
   end subroutine callback_procedure
 end interface 
!
! The callback list type.
!
type callback_list
   private
   class(callback_record), pointer :: first=>NULL()
end type callback_list
!
! Internal: each callback registration creates one of these
!
type, private :: callback_record
   procedure(callback_procedure), pointer, nopass :: proc
   class(callback_record),pointer::next
   class(callback_data),pointer :: data => NULL();
end type callback_record
private invoke, forward_invoke
contains
!
! Register a callback procedure with optional data
!
subroutine register_callback(list, entry, data)
  type(callback_list), intent(inout) :: list
  procedure(callback_procedure) :: entry
  class(callback_data), optional :: data
  type(callback_record), pointer :: new,last
  allocate(new)
  new%proc => entry
  if (present(data)) allocate(new%data, source=data)
  new%next => list%first
  list%first => new
end subroutine register_callback
! 
! Internal: Invoke a single callback and destroy its record
!
subroutine invoke(callback)
  type(callback_record),pointer :: callback
  if(associated(callback%data)) then
     call callback%proc(list%first%data)
     deallocate(callback%data)
  else
     call callback%proc
  end if
  deallocate(callback)
end subroutine invoke
!
! Call the procedures in reverse order of registration
!
subroutine invoke_callback_reverse(list)
  type(callback_list),intent(inout) :: list
  type(callback_record),pointer :: next,current
  current=>list%first
  nullify(list%first)
  do while (associated(current))
     next => current%next
     call invoke(current)
     current => next
  end do
end subroutine invoke_callback_reverse
! 
! Internal: Forward mode invocation
!
recursive subroutine forward_invoke(callback)
  if (associated(callback%next)) call forward_invoke(callback%next)
  call invoke(callback)
end subroutine forward_invoke
!
! Call the procedures in forward order of registration
!
subroutine invoke_callback_forward(list)
  type(callback_list),intent(inout) :: list
  if (associated(list%first)) call forward_invoke(list%first)
end subroutine invoke_callback_forward
end


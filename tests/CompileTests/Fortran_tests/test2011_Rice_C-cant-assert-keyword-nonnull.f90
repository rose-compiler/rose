! Rice test
! In 'c_action_actual_arg_spec', assertion 'keyword' != NULL is incorrect.
! CASE C: rule for 'actual_arg_spec'.

program c_cant_assert_keyword_nonnull
  call g(k = 0)  ! assertion failure: 'keyword' is null
contains
  subroutine g(k)
    integer :: k
  end subroutine	
end program

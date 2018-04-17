! Rice test
! In 'c_action_actual_arg_spec', assertion 'keyword' != NULL is incorrect.
! CASE B: rule for 'substr_range_or_arg_list_suffix', alternative 2.
!
! An alternate return specifier as the second or following argument
! in an actual argument list causes the front end to fail an assertion.

 subroutine g(k, *)
   integer :: k
 end subroutine

program b_cant_assert_keyword_nonnull
    call g(0, *100)  ! assertion failure: 'keyword' is null
100 continue
end program

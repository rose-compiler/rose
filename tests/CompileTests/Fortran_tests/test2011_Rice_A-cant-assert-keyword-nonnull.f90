! Rice test
! In 'c_action_actual_arg_spec', assertion 'keyword' != NULL is incorrect.
! CASE A: rule for 'substring_range_or_arg_list', alternative 5.
!
! An alternate return specifier as the first argument in an actual
! argument list causes the front end to fail an assertion.

program a_cant_assert_keyword_nonull
    call g(*100)  ! assertion failure: 'keyword' is null
 100 continue
 
contains
 subroutine g(*)
 end subroutine
end program

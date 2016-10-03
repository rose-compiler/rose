! S08A-cant-assert-keyword-nonnull.f90
! In 'c_action_actual_arg_spec', assertion 'keyword' != NULL is incorrect.
! CASE A: rule for 'substring_range_or_arg_list', alternative 5.
!
! An alternate return specifier as the first argument in an actual
! argument list causes the front end to fail an assertion.

! The subroutine definition for reference below is commented out because
! it causes front end to fail another assertion before showing this bug.
! subroutine g(*)
! end subroutine

program p
    call g(*100)  ! assertion failure: 'keyword' is null
100 continue
end program

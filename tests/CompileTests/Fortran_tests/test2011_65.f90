! S08C-cant-assert-keyword-nonnull.f90
! In 'c_action_actual_arg_spec', assertion 'keyword' != NULL is incorrect.
! CASE C: rule for 'actual_arg_spec'.
!
! An alternate return specifier following a keyword argument in an
! actual argument list causes the front end to fail an assertion.

! The subroutine definition for reference below is commented out because
! it causes front end to fail another assertion before showing this bug.
subroutine g(k, *)
   integer :: k
end subroutine

program p
    ! The following call is incorrect because F90 requires that
    ! positional arguments come before keyword arguments.
    ! However, OFP accepts it and ROSE fails an assertion on it.
    call g(k=0, *100)  ! assertion failure: 'keyword' is null
100 continue
end program

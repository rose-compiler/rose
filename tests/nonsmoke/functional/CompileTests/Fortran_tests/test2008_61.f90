! Danger with Optional Arguments
! In this example an optional argument is used to determine if a header is printed.

         subroutine print_char(this,header)
         character(len=*), intent (in) :: this
         logical, optional, intent (in) :: header
  ! THIS IS THE WRONG WAY
         if (present(header) .and. header) then
             print *, 'This is the header '
         endif
         print *, this
         end subroutine print_char

         subroutine print_char(this,header)
         character(len=*), intent (in) :: this
         logical, optional, intent (in) :: header
  ! THIS IS THE RIGHT WAY
         if (present(header)) then
            if (header) print *, 'This is the header '
         endif
         print *, this
         end subroutine print_char

! Explanation
! The first method is not safe because the compiler is allowed to evaluate the header argument before the present function is evaluated. If the header argument is not in fact present an out of bounds memory reference could occur, which could cause a failure.

subroutine where_example()
   ! integer :: logical_kind

   ! Note that "logical_kind" is an implicit variable
   ! logical(logical_kind) :: i

   ! This works only if -rose:skip_syntax_check is used since it is non-standard F90 syntax.
   ! logical*1 :: i1

     logical(kind=1) :: i2

     logical(1) :: i3

end subroutine where_example

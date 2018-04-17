 ! This works fine...
   character(len=2) :: a

   type t
    ! This works when compiled separately ... but fails when compiled with the statement below.
    ! character (5) :: Xarray (7)

    ! This is not unparsed correctly: "integer, DIMENSION(2) :: arr = (/(/"a","ab"/)/)"
    ! character (1) :: arr (2) = [ character(len=4) :: "a", "ab" ]
      character (1) :: arr (2)
   end type t

end

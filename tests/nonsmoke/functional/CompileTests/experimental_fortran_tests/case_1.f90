program main
  integer :: i
  integer :: j

  i = 3

  SELECT CASE (i)
     CASE(:3, 8:10)
        j = i
        j = 8
     CASE(4)
        j = 5
     CASE(6,7)
        j = 8
     CASE(11:)
        j = 500
     CASE DEFAULT
        j = 0
  END SELECT

end program

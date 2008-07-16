! Test inquire-stmt
!      inquire-stmt  is  INQUIRE ( inquire-spec-list )
!                    or  INQUIRE ( IOLENGTH = scalar-int-variable ) 
!                          output-item-list
!
! Not tested here: inquire-spec-list, scalar-int-variable, and
!  output-item-list.
inquire(10)
inquire(10, file='foo.txt', blank=my_blank_variable)
inquire(iolength=x) a, b, c

end

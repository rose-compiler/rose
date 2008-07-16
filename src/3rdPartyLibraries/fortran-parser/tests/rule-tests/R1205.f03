! Test interface-body
!      interface-body  is  function-stmt
!                            [ specification-part ]
!                          end-function-stmt
!                      or  subroutine-stmt
!                            [ specification-part ]
!                          end-subroutine-stmt
!
! Not tested here: function-stmt, specification-part, end-function-stmt, 
! subroutine-stmt, and end-subroutine-stmt.
!
! interface-body tested as part of an interface-block.

! Test functions.
interface
   function foo()
   end function foo
end interface

! Test subroutines.
interface
   subroutine sub()
   end subroutine sub
end interface

! Test both together; this is not a test of an interface-body, but actually
! a test of interface-specification, which allows for multiple interface-body.
interface
   subroutine sub()
   end subroutine sub
   function foo()
   end function foo
end interface

end

   

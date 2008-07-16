! Test module
!      module  is  module-stmt
!                    [ specification-part ]
!                    [ module-subprogram-part ]
!                  end-module-stmt
!
!      module-stmt  is  MODULE module-name
!
!      end-module-stmt  is  END [ MODULE [ module-name ] ]
!
!      module-subprogram-part  is  contains-stmt 
!                                    module-subprogram
!                                    [ module-subprogram ] ...
!
!      module-subprogram  is  function-subprogram 
!                         or  subroutine-subprogram
!
! Not tested here: specification-part, function-subprogram, and 
! subroutine-subprogram.

! None of the optional parts included
module a
end

! Include the optional MODULE in end-module-stmt
module a
end module

! Include optional MODULE and module-name in end-module-stmt.
module a
end module a

! Include an optional specification-part
module a
  integer i
end module a

! Include an optional module-subprogram-part
module a
contains
  subroutine sub()
  end subroutine sub
  function foo()
  end function foo
end module a

! Include all optional parts
module a
  integer i
contains
  subroutine sub()
  end subroutine sub
  function foo()
  end function foo
end module a


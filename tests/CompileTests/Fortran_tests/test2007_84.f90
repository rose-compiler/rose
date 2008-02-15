subroutine sub

end subroutine sub

subroutine subpte

end subroutine subpte

!subroutine example
     implicit none
     external sub
!    external subpte
     pointer (subptr,subpte)
     subptr = loc(sub)
!    call subpte()
!end subroutine example

end




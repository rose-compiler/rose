subroutine suba
    implicit none
    write(13,fmt='( &
         &3x,"Damping work: ")')
! good comment
    write(13,fmt='( &
         &3x,"Friction work: ")')
! BAD 1 This comment comes one line later
end subroutine suba

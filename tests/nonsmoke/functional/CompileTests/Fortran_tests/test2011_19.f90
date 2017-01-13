   integer, parameter :: kmax0 = 1
   integer, parameter :: kmax1 = 1
   integer, parameter :: kmax2 = 1
!  integer, dimension(2,kmax0) :: X(2,kmax0) = reshape( (/5,6/), (/2,kmax0/) )
!  integer, dimension(2,kmax0) :: X(2,kmax0)
!  integer, dimension(2) :: X(kmax0)

 ! This is correct in the AST, but not unparsed correctly.
 ! integer, dimension(2,4) :: X(kmax0,5)

 ! This is correct in the AST, but not unparsed correctly.
 ! integer, dimension(2,4) :: X(kmax0,5) = 7

   integer, dimension(2,kmax0) :: X(2,kmax1) = reshape( (/5,6/), (/2,kmax2/) )
end


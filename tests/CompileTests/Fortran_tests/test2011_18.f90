! Simplied example from test2010_136.f90
!     integer, parameter :: kmax0 = 1
!     integer, dimension(2,kmax0) :: X(2,kmax0) = reshape( (/5,6/), (/2,kmax0/) )
!     integer, dimension(2,3) :: X(4,5) = reshape( (/6,7/), (/8,9/) )
!     integer, dimension(2,3) :: X(4,5) = 7
!     integer, dimension(2,3) :: X(4,5)
      integer, dimension(2) :: X(3)
      integer, dimension(2),pointer :: Y(:)

    ! This is correct in the AST, but not unparsed correctly!
      integer, dimension(2),pointer :: Z(:)

end

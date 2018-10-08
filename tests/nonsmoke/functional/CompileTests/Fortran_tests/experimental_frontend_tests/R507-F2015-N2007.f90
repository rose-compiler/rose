!! R507-F2015-N2007 form-team-stmt
!    is FORM TEAM ( team-id , team-variable [, form-team-spec-list ] )
!
program main
  use, intrinsic :: ISO_FORTRAN_ENV
  type(TEAM_TYPE) :: initial, block
  integer :: me, p2

  call GET_TEAM(initial)
  me = THIS_IMAGE()
  P2 = NUM_IMAGES()/2

  FORM TEAM(1+(ME-1)/p2, block)

end program

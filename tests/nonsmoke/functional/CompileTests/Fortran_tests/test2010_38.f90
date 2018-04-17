! Bug report from Laksono Adhianto
! Just in case you haven't seen this bug, I found out that Rose unparses allocatable character into a character:
module testch
! Unparses as: CHARACTER :: cckwrk(:)
! This causes an error when we allocate the variable since the variable is not allocatable anymore.
  character*16, allocatable, dimension(:) :: cckwrk   !chemkin character work array
end module


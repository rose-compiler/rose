! S02-abort-leaves-broken-rmod-file.f90
! Running this file through 'testTranslator' will produce a broken
! ".rmod" file. This file contains an error which causes ROSE to abort,
! and the broken .rmod file is not cleaned up.

! Note: this file demonstrates its bug by exploiting another bug,
! the one in which ROSE fails a assertion when it sees an empty string constant.
! If anyone fixes that bug, then this test file will no longer fail.

module m
contains
  subroutine g
    character :: c = ""  ! ROSE fails here and leaves a bad rmod file
  end subroutine
end module

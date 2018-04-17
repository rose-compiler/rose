! { dg-do compile }
! This tests the fix for PRs 26834, 25669 and 18803, in which
! shape information for the lbound and ubound intrinsics was not
! transferred to the scalarizer.  For this reason, an ICE would
! ensue, whenever these functions were used in temporaries.
!
! The tests are lifted from the PRs and some further checks are
! done to make sure that nothing is broken.
!
! This is PR26834
subroutine gfcbug34 ()
  implicit none
  type t
     integer, pointer :: i (:) => NULL ()
   ! integer, pointer :: i (:)
  end type t
!  type(t), save :: gf
!  allocate (gf%i(20))
!  write(*,*) 'ubound:', ubound (gf% i)
!  write(*,*) 'lbound:', lbound (gf% i)
end subroutine gfcbug34

end


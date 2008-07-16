! Test rewind-stmt
!      rewind-stmt  is  REWIND file-unit-number
!                   or  REWIND ( position-spec-list )
!
! Not tested here: file-unit-number and position-spec-list.
rewind 10
rewind (10, iostat=n, err=4)

end

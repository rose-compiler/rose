! Test endfile-stmt
!      endfile-stmt  is  ENDFILE file-unit-number
!                    or  ENDFILE ( position-spec-list )
!
! Not tested here: file-unit-number and position-spec-list.
endfile 10
endfile (10, iostat=n, err=4)

end


! Test 
!      backspace-stmt  is  BACKSPACE file-unit-number
!                      or  BACKSPACE ( position-spec-list )
!
! Not tested here: file-unit-number and position-spec-list.
backspace 10
backspace (10, iostat=n)

end

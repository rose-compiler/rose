! Test stop-stmt
!      stop-stmt  is  STOP [ stop-code ]
!  
!      stop-code  is  scalar-char-constant
!                 or  digit [ digit [ digit [ digit [ digit ] ] ] ] 
!
! Not tested here: scalar-char-constant
stop
stop 'end of program'
stop 1
stop 12
stop 123
stop 1234
stop 12345

end

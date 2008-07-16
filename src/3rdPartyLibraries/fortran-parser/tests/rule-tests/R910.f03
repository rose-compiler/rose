! Test read-stmt
!      read-stmt  is  READ (io-control-spec-list ) [ input-item-list ]
!                 or  READ format [, input-item-list ]
!
! Not tested here: io-control-spec-list, input-item-list, and format
read(10)
read(10, *) a, b

read 5, a, b, c

5 format (2e16.3, i5)

end


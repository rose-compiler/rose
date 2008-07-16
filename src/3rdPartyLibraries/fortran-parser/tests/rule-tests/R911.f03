! Test write-stmt
!      write-stmt  is  WRITE ( io-control-spec-list ) [ output-item-list ]
!
! Not tested here: io-control-spec-list and output-item-list.
write(5)
write(6,10)
write(blank=" ", end=12, 10) a, b, c

end

! Test flush-stmt
!      flush-stmt  is  FLUSH file-unit-number
!                  or  FLUSH ( flush-spec-list )
!
! Not tested here: file-unit-number.
flush 10
flush (10, iostat=n, iomsg=var, err=10)
flush (unit=10, iostat=n, iomsg=var, err=10)

end


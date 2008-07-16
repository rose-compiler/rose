

! Bunch of tests for io-_control_spec_list.
! Ripped off from web page of examples
write (*,*) a
write (unit=6,*) b
write (10,fmt=*) c 
write (unit=n,fmt=*,iostat=ios) d
write (*,err=hula) e
write (*,size=14,err=hula,iostat=3,rec=2) f
end


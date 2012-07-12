program if_stop
  if (.true.) stop  ! assertion failure
  if (.true.) stop 2  ! 2 becomes "2" in translation
  if (n .ne. 2) then
    stop
  endif
  if (.true.) then
    stop 2
  endif
  if (.false.) then
    n = 1   
  else
    stop
  endif
  if (n .eq. 1) then
    stop 'Done #123' 
  else
    stop 2
  endif
end program


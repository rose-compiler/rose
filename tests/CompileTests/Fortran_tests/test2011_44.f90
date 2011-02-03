program main
  real, target :: a, b
  real, pointer :: p, q
  nullify(p, q)
  if(associated(p)) then
     print *, "failed"
  end if
  a=3.
  p => a
  if(.NOT. (associated(p, a))) then
     print *, "failed"
  end if
  q => b
  nullify(q)
  if(associated(q)) then
     print *, "failed"
  else
     print *, "OK"
  end if
end program main

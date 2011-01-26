! I think this is using F03 syntax for procedure pointers.
! So this might really be a FFortran 2003 test code 
! (syntax fails for gfortran, but is part of the gfortran test suite).

  procedure(prc_is_allowed), pointer :: fptr

  interface
     function prc_is_allowed (flv, hel, col) result (is_allowed)
       logical :: is_allowed
       integer, intent(in) :: flv, hel, col
     end function prc_is_allowed
  end interface

  fptr => prc_is_allowed

end


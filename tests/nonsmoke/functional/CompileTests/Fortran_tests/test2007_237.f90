! implied do loop for variable initialization
! I think this is an OFP bug.
program main
  real, dimension(10):: tab =(/(0.0, I=1,10)/)
end program

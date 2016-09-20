! unary-operator_1: Bug from Rice (8/5/2010)
! This appears to be a bug in OFP (something being worked on currently).

! the result should be "0"
! after go through OFP/ROSE, the result is "-2"

program foo

  integer i
  i = 1
  print *,-i+1

end

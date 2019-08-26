program test_format
  write(13,'("line1:")')
  write(13,fmt='(/, &
        10x,"eq:0 - no blending of new & old topology weights",/, &
        10x,"gt:0 - weights are blended over this # of steps")')
end program

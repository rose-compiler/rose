! This test code tests the correct handling of labels on the if-stmt.
  integer i,m,n
  do 20 m=1,n
     i = m
20   if (.true.) i = 0
end



! Testing data-stmt-object (R526), data-implied-do (R527), data-i-do-object (R528),
! and data-i-do-variable (R528)
data truth, beauty, ugly /3,14,15/
data a, (z%d, (B(i,j) j=1,i), i=1,100,foo()) /3*1.0/    ! LEADS TO PARSER ERROR, is this correct? 
data myname /person(21,'john')/
data a%b/jj/
end

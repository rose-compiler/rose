! Test io-implied-do
!      io-implied-do  is  ( io-implied-do-object-list, io-implied-do-control )
!
! Not tested here: io-implied-do-object-list and io-implied-do-control.
!
! io-implied-do tested as an output-itme for a print-stmt.
print *, (a, b, c, d=1,2)
print *, (a, b, c, d=1,10,2)

end

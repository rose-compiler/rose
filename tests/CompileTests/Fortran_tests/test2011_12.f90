program foo
   integer, parameter :: numdims = 4
   integer :: localcount(numdims), localmap(numdims)

   localCount = (/ 2, 3, 5, 7 /)

!  localMap   (:numDims  ) = (/ 1, (product(localCount(:j)), j = 1, numDims - 1) /)
!  localMap   (:numDims  ) = (/ 1, (product(localCount(:j)), j = 5, 12) /)

   localMap   (:numDims  ) = (/ 1, (product(localCount(:j)), j = 1, numDims - 1) /)
   print *, "localcount=", localcount
   print *, "localmap=", localmap
end

   implicit none

   integer nsize
 ! **** Use the intrinsic F90 random number generator *****
 ! initialize f90 random number generator

 ! Call this function in three different ways (testing).
   call random_seed
   call random_seed()
   call random_seed(size=nsize)
end

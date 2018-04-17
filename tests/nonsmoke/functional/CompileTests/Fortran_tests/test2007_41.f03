module test2007_41_test
   interface test_interface
      subroutine foo (i)
         import
         integer :: i 
    ! This is not reported as a bug in gfortran 4.1.2, but 
    ! is correctly reported as a bug in later versions of gfortran.
    ! end subroutine sub
      end subroutine foo
   end interface test_interface
end module test2007_41_test

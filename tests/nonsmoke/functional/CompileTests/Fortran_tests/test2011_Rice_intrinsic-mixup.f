      subroutine foo ()
      LP = index('fortran', 'r') 
      end

      subroutine bar ()
         index = 4  ! ROSE assertion failure due to mixup with intrinsic function INDEX
      end

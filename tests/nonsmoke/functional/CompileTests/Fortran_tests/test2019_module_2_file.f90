 module testmod2
        implicit none

 contains

      subroutine shape_function_weight()

        implicit none

          call outerproduct()

        return
        
      end subroutine  shape_function_weight


!***************************************************************************************

      subroutine shape_function_weightm()

        use testmod ! use types and functions defined in this module
 
        implicit none
        real output  
        
        output = outerproduct(0.) 

        return
        
      end subroutine  shape_function_weightm


!***************************************************************************************

       subroutine outerproduct()
        implicit none
        return
      end subroutine outerproduct


 end module testmod2


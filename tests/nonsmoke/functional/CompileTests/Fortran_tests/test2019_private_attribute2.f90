  module test2019_private_attribute2
  private dot_product3

contains
  function dot_product3(a,b)
!$acc routine seq
    real :: dot_product3
    real :: a,b
     
  end function dot_product3
  end module test2019_private_attribute2

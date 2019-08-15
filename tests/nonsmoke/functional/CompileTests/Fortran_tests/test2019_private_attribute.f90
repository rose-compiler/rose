  module test2019_private_attribute

  contains
  subroutine s1()
    use test2019_private_attribute2
    real ::  r
     r = dot_product3(1.0,2.0) 

    contains
      function dot_product3(a,b)
      real :: dot_product3
      real :: a,b
      end function dot_product3
  end subroutine s1
  
  end module test2019_private_attribute

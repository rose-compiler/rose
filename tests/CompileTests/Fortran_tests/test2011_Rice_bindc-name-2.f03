program test_bindc_name2

   type :: T
   end type 

  type(T), bind(C, name="foo") :: bar

end program

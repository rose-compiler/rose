program test_bindc_name
  integer, bind(C, name="foo") :: bar
end program

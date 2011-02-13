! Bug report from Jean (ANL) nameListDecl_2.f90
module m

   implicit none
   save

   logical  ::  &
      iage_on, dye_on

   namelist /passive_tracers_on_nml/  &
      iage_on, dye_on
end module 

program p 
  use m
  print *,'OK' ! parser test for namelist only
end program

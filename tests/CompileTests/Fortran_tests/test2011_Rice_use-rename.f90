  module work_mod_1
  implicit none
   real work
  end module
  
  subroutine use_rename_1
  use work_mod_1, ke => work 
  implicit none 

  ke = 0.5
  end subroutine

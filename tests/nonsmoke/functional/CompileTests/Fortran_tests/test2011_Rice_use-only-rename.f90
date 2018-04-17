  module work_mod
  implicit none
   real work
  end module
  
  subroutine use_only_rename
  use work_mod, only: ke => work 
  implicit none 

  ke = 0.5
  end subroutine

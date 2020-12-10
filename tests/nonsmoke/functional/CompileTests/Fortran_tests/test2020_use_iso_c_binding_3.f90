!! This tests for issue PAR-88
!
!
module test2020_use_iso_c_binding_3
! contains memory pool-related routines

  interface
    
    subroutine my_free(ptr) bind(C,name="free")
      use iso_c_binding
    end subroutine my_free
    
  end interface

end module test2020_use_iso_c_binding_3

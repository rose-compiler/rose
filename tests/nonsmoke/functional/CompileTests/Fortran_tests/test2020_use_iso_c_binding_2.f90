!! This tests for issue ROSE-2567
!
!
module memory_pool_subs
! contains memory pool-related routines

contains

  subroutine free_memory(cptr)
    use iso_c_binding
    implicit none

    ! incoming

    type(c_ptr)               :: cptr


  end subroutine free_memory

end module memory_pool_subs

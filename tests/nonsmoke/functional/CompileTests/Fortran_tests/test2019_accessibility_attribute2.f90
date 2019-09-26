module test2019_accessibility_attribute2
   implicit none
   private
   save
   type, public :: distrb  ! distribution data type
      integer  :: &
         nprocs            ,&! number of processors in this dist
         communicator        ! communicator to use in this dist

      integer , dimension(:), pointer :: &
         proc              ,&! processor location for this block
         local_block         ! block position in local array on proc
   end type
end module
module test2019_accessibility_attribute3
   use test2019_accessibility_attribute2
  implicit none
  private
  save
  type(distrb) :: dist 
end module

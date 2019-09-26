   subroutine loadcurve_label2num
!
! Convert loadcurve labels to their internal number for options that
! do not call an initialization routine (usually in initlz).
!
!     use mod_assembly
!
     implicit none
!
   integer, parameter:: singI = kind(0)
   type fric_damp_data_T
   integer   :: lc_damp_dev   = 0
   end type fric_damp_data_T
!
   type part_T
   type(fric_damp_data_T),pointer :: fric_damp_data => Null()
   end type part_T
!
   type assembly_T
   type(part_T), dimension(:),pointer,contiguous :: part => Null()
   end type assembly_T
!  
   type(assembly_T),save,target :: assembly
   integer(singI) :: lc_num
     type(part_T), pointer :: part
!
   type(fric_damp_data_T), pointer :: tmp
!
     part => assembly%part(1)
       tmp = part%fric_damp_data
!     lc_num = part%fric_damp_data%lc_damp_dev
!
   end subroutine loadcurve_label2num

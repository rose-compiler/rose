module patch

  type patch_type
    integer :: color
  end type patch_type
  
contains

   subroutine setup_patch(patch_data, color)
     implicit none
     class(patch_type), intent(out) :: patch_data ! Auto deallocation of all type components
     integer,           intent(in)  :: color
   end subroutine setup_patch
      
 end module patch

   type :: distrbutionType  ! distribution data type
      integer, dimension(:), pointer :: proc
   end type

   type (distrbutionType) :: distribution

   integer bcount
   bcount = 0
   do n=1,size(distribution%proc)
      if (distribution%proc(n) == 1) bcount = 0
   end do
end

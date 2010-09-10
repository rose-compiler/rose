!  function nf90_put_var_1D_text(ncid, varid, values, start, count, stride, map)
   function nf90_put_var_1D_text()
!     integer,                         intent( in) :: ncid, varid
!     character (len = *), dimension(:), &
!                                      intent( in) :: values
!     integer, dimension(:), optional, intent( in) :: start, count, stride, map
     integer                                      :: nf90_put_var_1D_text
 
     integer, parameter :: &
        nf90_max_dims     = 1024 !, nf90_max_var_dims = nf90_max_dims

     integer, parameter                :: numDims = 1
     integer, dimension(nf90_max_dims) :: localStart, localCount, localStride, localMap
     integer                           :: counter
 
   ! Set local arguments to default values
   ! localStart (:         ) = 1
   ! localCount ( :numDims+1) = (/ len(values(1)), shape(values) /)
   ! localCount (numDims+2:) = 0
   ! localStride(:         ) = 1
!    localMap   (:numDims  ) = (/ 1, (product(localCount(:counter)), counter = 1, numDims - 1) /)
!    localMap   (:numDims  ) = (/ 1, (localCount(:counter), counter = 1, numDims - 1) /)
!    localMap   (:numDims  ) = (/ 1, (localCount(0), counter = 1, numDims - 1) /)
     localMap   (:numDims  ) = (/ 1, (localCount(0), counter = 1, numDims - 1) /)

!    localMap   (:numDims  ) = 0

   end function nf90_put_var_1D_text

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module io

!BOP
! !MODULE: io
!
! !DESCRIPTION:
!  This module provides a generic parallel input/output interface
!  for writing arrays.
!
! !REVISION HISTORY:
!  CVS:$Id: io.F90,v 1.12 2002/05/07 17:40:29 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use communicate
   use broadcast
   use exit_mod
   use domain
   use constants
   use io_netcdf
   use io_binary
   use io_types

   implicit none
   public  ! to get io_types without having to explicitly use io_types
           ! module directly
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: data_set

!EOP
!BOC
!EOC
!***********************************************************************

contains

!***********************************************************************
!BOP
! !IROUTINE: data_set
! !INTERFACE:

 subroutine data_set (data_file, operation, io_field)

! !DESCRIPTION:
!  This routine is the main interface for array and file io functions,
!  including read, write, open, close.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent (in)   :: operation

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent (inout)  :: data_file
   type (io_field_desc), intent (inout), optional :: io_field

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------


!-----------------------------------------------------------------------
!
!  select operation to perform
!
!-----------------------------------------------------------------------

   select case (trim(operation))

!-----------------------------------------------------------------------
!
!  open for reading
!
!-----------------------------------------------------------------------

   case ('open_read')

      if (data_file%data_format=='bin') then
         call open_read_binary(data_file)
      else if (data_file%data_format=='nc') then
         call open_read_netcdf(data_file)
      endif

!-----------------------------------------------------------------------
!
!  Open means open for write.  We also at this time write any global
!  attributes.
!
!-----------------------------------------------------------------------

   case ('open')

      if (data_file%data_format=='bin') then
         call open_binary(data_file)
      else if (data_file%data_format=='nc') then
         call open_netcdf(data_file)
      endif

!-----------------------------------------------------------------------
!
!  close a data file
!
!-----------------------------------------------------------------------

   case ('close')

      if (data_file%data_format=='bin') then
         call close_binary(data_file)
      else if (data_file%data_format=='nc') then
         call close_netcdf(data_file)
      endif

!-----------------------------------------------------------------------
!
!  define an io field
!
!-----------------------------------------------------------------------

   case ('define')

      if (.not.present(io_field)) then
         call exit_POP(sigAbort, &
                       'data_file define: missing io_field arg')
      end if

      if (data_file%data_format=='bin') then
         call define_field_binary(data_file,io_field)
      else if (data_file%data_format=='nc') then
         call define_field_netcdf(data_file,io_field)
      endif

!-----------------------------------------------------------------------
!
!  write an io field
!
!-----------------------------------------------------------------------

   case ('write')

      if (.not.present(io_field)) then
         call exit_POP(sigAbort,'data_file write: missing io_field arg')
      end if

      if (data_file%data_format=='bin') then
         call write_field_binary(data_file,io_field)
      else if (data_file%data_format=='nc') then
         call write_field_netcdf(data_file,io_field)
      endif

!-----------------------------------------------------------------------
!
!  read an io field
!
!-----------------------------------------------------------------------

   case ('read')

      if (.not.present(io_field)) then
         call exit_POP(sigAbort,'data_file read: missing io_field arg')
      end if

      if (data_file%data_format=='bin') then
         call read_field_binary(data_file,io_field)
      else if (data_file%data_format=='nc') then
         call read_field_netcdf(data_file,io_field)
      endif

!-----------------------------------------------------------------------
!
!  unknown operation
!
!-----------------------------------------------------------------------

   case default

      if (my_task == master_task) &
         write(stdout,*) 'data_set operation: ',trim(operation)
      call exit_POP(sigAbort,'data_set: Unknown operation')

   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine data_set

!***********************************************************************

 end module io

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

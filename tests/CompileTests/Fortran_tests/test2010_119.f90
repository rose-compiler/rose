
program gfcbug51
  implicit none

  character(len=12) :: cdate(3)      ! yyyymmddhhmm

  type year_t
    integer :: year = 0
  end type year_t

  type(year_t) :: time(3)

  cdate = (/'200612231200', '200712231200', &
            '200812231200'/)

!  call date_to_year (cdate)
!  if (any (time%year .ne. (/2006, 2007, 2008/))) call abort ()

!  call month_to_date ((/8, 9, 10/), cdate)
!  if ( any (cdate .ne. (/'200608231200', '200709231200', &
!                         '200810231200'/))) call abort ()

contains

  subroutine date_to_year (d)
    character(len=12) :: d(3)
  ! Before being fixed: This statement leaves and expression on the stack and this causes problems later.
  ! read (cdate(:)(1:4),'(i4)')  time%year
    read (cdate(:)(1:4),'(i4)')  time%year
  end subroutine

!  subroutine month_to_date (m, d)
!    character(len=12) :: d(3)
!    integer :: m(:)
!!    write (cdate(:)(5:6),'(i2.2)')  m
!  end subroutine month_to_date

end program gfcbug51

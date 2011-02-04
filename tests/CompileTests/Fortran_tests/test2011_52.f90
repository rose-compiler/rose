! from http://www.cs.rpi.edu/~szymansk/OOF90/F90_Objects.html

      module fft1r_module
      integer, save, private :: saved_indx = -1
      integer, dimension(:), allocatable, save, private :: mixup
      complex, dimension(:), allocatable, save, private :: sct
      contains
         subroutine fft1r_init(indx)
! initialization call
         integer, intent(in) :: indx
         integer :: nx, nxh, ierr, isign=0
! allocate f and t: old, ugly fft requires them as arguments
         real, dimension(2**indx) :: f
         complex, dimension(2**(indx-1)) :: t
         if (indx.lt.0) then
            print *,'indx must be non-negative'
            stop
         endif
         nx = 2**indx ; nxh = nx/2 ; saved_indx = indx
         if (allocated(mixup)) deallocate(mixup)
         if (allocated(sct)) deallocate(sct)
         allocate(mixup(nxh),sct(nxh),stat=ierr)
         if (ierr.ne.0) then
            print *,'allocation error'
            stop
         endif
! call old, ugly but fast fft here
!         call lib_fft1r(f,t,isign,mixup,sct,saved_indx,nx,nxh)
         end subroutine fft1r_init
!
         subroutine fft1r_end
! deallocate internal data
         saved_indx = -1
         deallocate(mixup,sct)
         end subroutine fft1r_end
!
         subroutine fft1r(f,isign)
         real, dimension(:), intent(inout) :: f
         integer, intent(in) :: isign
         integer :: nx, nxh
         complex, dimension(size(f)/2) :: t
         nx = size(f) ; nxh = nx/2
! do nothing if isign is invalid
         if (isign.eq.0) return
         if (saved_indx.lt.0) then
            print *,'fft tables not initialized!'
            stop
         endif
! call old, ugly but fast fft here
!         call lib_fft1r(f,t,isign,mixup,sct,saved_indx,nx,nxh)
         end subroutine fft1r
      end module fft1r_module
	  
	  
	  

      program main
      use fft1r_module
      implicit none
      integer :: indx, nx, i
      real, dimension(:), allocatable :: f
! write prompt without linefeed
!      write (6,'(a)',advance='no') 'enter indx: '
! obtain indx from input device
!      read (5,*) indx
       indx=4
! allocate array f
      nx = 2**indx
      allocate(f(nx))
! initialize data using array constructor
      f = (/(i,i=1,nx)/)
! initialize fft
      call fft1r_init(indx)
! call fft
      call fft1r(f,-1)
! terminate fft
      call fft1r_end
      print *, "OK"
      end

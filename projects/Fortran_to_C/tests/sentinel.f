      subroutine test()
c ----Declarations      
      integer i
      integer j
c ----Begin loop
c$omp parallel
! Do 'do' loops make sense in C?
!$omp do    
      do i = 1, 10
         j = j+1
      enddo
! An ampersand at the end allows continuation of a statement, but each line must
! still being with the sentinel.  These are parsed to a '\\\n' in C.  Comments
! at the end of a sentinel are NOT supported.
*$omp end &  
c The next statement is parsed as null and deleted
!$omp &
!$omp parallel for    
      return
      end

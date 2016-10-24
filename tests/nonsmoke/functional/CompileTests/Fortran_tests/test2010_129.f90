!     implicit real*8 (a-h,o-z)
      implicit none
      integer JPL

!      CHARACTER*50   FNAME
!      CHARACTER*1    DUMMY
!      data JPL / 01 /
!      OPEN(JPL,ERR=999,FILE=FNAME,STATUS='OLD')
!    1    READ(JPL,'(a1)') DUMMY
!     IF( DUMMY.EQ.'#' ) GO TO 1

      backspace JPL
      backspace (unit=JPL,err=999)
      backspace (JPL,err=999)
  999 CONTINUE
end


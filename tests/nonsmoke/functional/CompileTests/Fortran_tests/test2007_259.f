! This is a bug in gfortran (version 4.1.2) (gfortran asks that the error be reported)
! It however correctly handled in OFP, as I understand it.
!      READ (fmt=1) A

! This fails and does not report the presence of the "*" in OFP
!      READ *

! This is the format option (though no action in OFP makes this detectable, same as above)
!      READ *, A

! This works fine (and it interpreted as unit=*)
!      READ (*) A

! This is the fomat specifier, but is handled properly in OFP.
      READ 1, A

! This is interpeted properly as unit specifier.
!      READ (1) A

    ! This a comment even though not starting in column 1
      WRITE (*,1) A

! This is a print
      PRINT 1, A

 1    FORMAT ()
      END

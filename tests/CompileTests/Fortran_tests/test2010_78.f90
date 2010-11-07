! Hi Dan:
! Thank you for informing us about your upcoming 2-week "disappearing act."
! We were able to svn merge the rose-rice branch to the rose main trunk.
! However, we encountered quite a few problems when we ran our own benchmark test suite.
! There are two problems right now that we have not been able to resolve nor found a workaround.
! Attached are three simple input files, m1.f90, m2.f90 and prog1.f90 to illustrate the bugs.
! 1.  Running the testTranslator utility with m1.f90, m2.f90 in that order produces incorrect unparsed code for m2.f90.  
! The output file rose_m2.f90 has an include statement at the top and looks like this:
! include "m2.f90"
! MODULE m2
! INTEGER :: i2
! END MODULE m2
! 2.  Running testTranslator with m1.f90 as the sole input first, 
! then running testTranslator again with prog1.f90 as input produces the following incorrect output  (NOTE that the assignment statement in prog1.f90 is gone):
! PROGRAM prog1
! USE m1
! END PROGRAM 
! We must resolve these problems before we can proceed with our testing and complete the merge.
! Your assistance in debugging these problems is greatly appreciated.
! Zung

module m1
  integer :: i1
end module


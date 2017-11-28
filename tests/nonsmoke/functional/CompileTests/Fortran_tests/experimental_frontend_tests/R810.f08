!! R810 critical-construct
!    is critical-stmt
!          block
!       end-critical-stmt
!
! from NOTE 8.5
10 CRITICAL
      GLOBAL_COUNTER[1] = GLOBAL_COUNTER[1] + 1
11 END CRITICAL

 CRITICAL
 END CRITICAL

end

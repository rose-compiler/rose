! Test nonblock-do-construct
!      nonblock-do-construct  is  action-term-do-construct
!                             or  outer-shared-do-construct
!
!      action-term-do-construct  is  label-do-stmt
!                                      do-body
!                                    do-term-action-stmt
!
!      do-body  is  [ execution-part-construct ] ...
!
!      do-term-action-stmt  is  action-stmt
!
!      outer-shared-do-construct  is  label-do-stmt
!                                       do-body
!                                     shared-term-do-construct
!
!      shared-term-do-construct  is  outer-shared-do-construct
!                                or  inner-shared-do-construct
!
!      inner-shared-do-construct  is  label-do-stmt
!                                       do-body
!                                     do-term-shared-stmt
!
!      do-term-shared-stmt  is  action-stmt
!
! Tested here: nonblock-do-construct, action-term-do-construct, 
! outer-shared-do-construct, outer-shared-do-construct, 
! shared-term-do-construct, inner-shared-do-construct.
!
! Not tested here: label-do-stmt, do-body, do-term-action-stmt, and 
! do-term-shared-stmt.

! Test the action-term-do-construct form first.
do 10 i = 1, 10
   x = 2
10 print *, 'end of do loop 10'

! Test the outer-shared-do-construct
do 20 i = 1, 10 ! outer-shared-do-construct
   x = 2
   do 20 j = 1, 10 ! outer-shared-do-construct
      x = 3
      do 20 k = 1, 10 ! inner-shared-do-construct
         x = 4
20       print *, "end of do loop 20"

end


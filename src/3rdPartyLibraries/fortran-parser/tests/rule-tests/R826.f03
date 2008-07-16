! Test block-do-construct
!      block-do-construct  is  do-stmt
!                                do-block
!                              end-do
!
!      do-stmt  is  label-do-stmt
!               or  nonlabel-do-stmt
!
!      label-do-stmt  is  [ do-construct-name : ] DO label [ loop-control ]
!
!      nonlabel-do-stmt  is  [ do-construct-name : ] DO [ loop-control ]
!
!      loop-control  is  [,] do-variable = scalar-int-expr, scalar-int-expr
!                          [ , scalar-int-expr ]
!                    or  [,] WHILE ( scalar-logical-expr )
!
!      do-variable  is  scalar-int-variable
!
!      do-block  is  block
!
!      end-do  is  end-do-stmt
!              or  continue-stmt
!
!      end-do-stmt  is  END DO [ do-construct-name ]
!
! Tested here: block-do-construct, do-stmt, end-do, label-do-stmt, 
! nonlabel-do-stmt, do-construct-name, loop-control, and end-do-stmt.
! 
! Not tested here: do-block, do-variable, scalar-int-expr, 
! scalar-logical-expr, block, and continue-stmt.

! Test the label and nonlabel stmts
do 10 
   x = 1
10 end do 

do 10, i = 1,10
   x = 1
10 end do 

! Test the loop-control forms
do i = 1,10
   x = 1
end do 

do i = 1,10,2
   x = 1
end do 

do i = 1,10,2
   x = 1
end do 

do while(x > 2)
   i = 1
end do 

do, while(x > 2)
   x = 1
end do 

! Test the end-do forms
do i = 1,10
   x = 1
end do

do 10 i = 1,10
   x = 1
10 continue

! Test the allowance of a do-construct-name
my_do: do i = 1, 10
   x = 2
end do my_do

end

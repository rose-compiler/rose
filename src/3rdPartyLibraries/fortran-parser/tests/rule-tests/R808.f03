! Test case-construct
!      case-construct  is  select-case-stmt
!                            [ case-stmt
!                                block ] ...
!                          end-select-stmt
!
!      select-case-stmt  is  [ case-construct-name: ] SELECT CASE ( case-expr )
!
!      case-stmt  is  CASE case-selector [ case-construct-name ]
!  
!      end-select-stmt  is  END SELECT [ case-construct-name ]
!
!      case-expr  is  scalar-int-expr
!                 or  scalar-char-expr
!                 or  scalar-logical-expr
!
!      case-selector  is  ( case-value-range-list )
!                     or  DEFAULT
!
!      case-value-range  is  case-value
!                        or  case-value :
!                        or  : case-value
!                        or  case-value : case-value
! 
!      case-value  is  scalar-int-initialization-expr
!                  or  scalar-char-initialization-expr
!                  or  scalar-logical-initialization-expr
!
! Tested here: case-construct, select-case-stmt, case-stmt, end-select-stmt,
! case-expr, case-selector, case-value-range, case-value-range-list,  
! case-value, and case-construct-name.
!
! Not tested here are: scalar-int-expr, scalar-char-expr, scalar-logical-expr, 
! scalar-int-initialization-expr, scalar-char-initialization-expr,  
! scalar-logical-initialization-expr, and block.
select case(x) 
   case (1)
      x = 1
   case (2:)
      x = 2
   case (:3)
      x = 3
   case (5:8)
      x = 8
   case (10:12, 14:15, 22)
      x = 15
   case default 
      x = 0
end select

select case('a') 
   case ('a')
      y = 'a'
   case default 
      y = 'b'
end select

select case(x > 2) 
   case (.true.)
      flag = .true.
   case default 
      flag = .false.
end select

my_case: select case(x > 2) 
   case (.true.) my_case
      flag = .true.
   case default my_case
      flag = .false.
end select my_case

end

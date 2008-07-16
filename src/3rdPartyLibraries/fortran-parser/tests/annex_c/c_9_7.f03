type(point) :: t2
type(color_point) :: t3
class(point) :: p2
class(color_point) :: p3
! Dummy argument is polymorphic and actual argument is of fixed type
! i added the 'end subroutine ...'
subroutine sub2 ( x2 ); class(point) :: x2; end subroutine sub2
subroutine sub3 ( x3 ); class(color_point) :: x3; end subroutine sub3

call sub2 ( t2 ) ! Valid -- The declared type of t2 is the same as the
                 !          declared type of x2.
call sub2 ( t3 ) ! Valid -- The declared type of t3 is extended from 
                 !          the declared type of x2.
call sub3 ( t2 ) ! Invalid -- The declared type of t2 is neither the 
                 !            same as nor extended from the declared type 
                 !            of x3.
call sub3 ( t3 ) ! Valid -- The declared type of t3 is the same as the 
                 !          declared type of x3.
! Actual argument is polymorphic and dummy argument is of fixed type
! i added the 'end subroutine ...'
subroutine tub2 ( d2 ); type(point) :: d2; end subroutine tub2
subroutine tub3 ( d3 ); type(color_point) :: d3; end subroutine tub3

call tub2 ( p2 ) ! Valid -- The declared type of p2 is the same as the
                 !          declared type of d2.
call tub2 ( p3 ) ! Invalid -- The decared type of p3 differs from the
                 !            declared type of d2.
call tub2 ( p3%point ) ! Valid alternative to the above
call tub3 ( p2 ) ! Invalid -- The declared type of p2 differs from the
                 !            declared type of d3.
select type ( p2 ) ! Valid conditional alternative to the above
class is ( color_point ) ! Works if the dynamic type of p2 is the same
  call tub3 ( p2 )       ! as the declared type of d3, or a type 
                         ! extended therefrom.
class default
                         ! Cannot work if not.
end select
call tub3 ( p3 ) ! Valid -- The declared type of p3 is the same as the 
                 !          declared type of d3.
! Both the actual and dummy arguments are of polymorphic type.
call sub2 ( p2 ) ! Valid -- The declared type of p2 is the same as the
                 !          declared type of x2.
call sub2 ( p3 ) ! Valid -- The declared type of p3 is extended from 
                 !          the declared type of x2.
call sub3 ( p2 ) ! Invalid -- The declared type of p2 is neither the
                 !            same as nor extended from the declared 
                 !            type of x3.
select type ( p2 ) ! Valid conditional alternative to the above
class is ( color_point ) ! Works if the dynamic type of p2 is the
  call sub3 ( p2 )       ! same as the declared type of x3, or a 
                         ! type extended therefrom.
class default
                         ! Cannot work if not.
end select
call sub3 ( p3 ) ! Valid -- The declared type of p3 is the same as the
                 ! declared type of x3.

end  ! i added this.


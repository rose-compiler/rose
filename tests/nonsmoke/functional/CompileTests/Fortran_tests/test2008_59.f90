! Example from Craig (9/24/2008)
!
! 1. I don't think the class SgColonShapeExp is needed.  This is just a part of an SgSubscriptExpression (and is used elsewhere as such, for example, in section-subscript).  I've changed case ArraySpecElement_colon with the following:
!
!                // expression = new SgColonShapeExp();
!                // setSourcePosition(expression);

!                // CER (9/24/2008): I think SgSubscriptExpression is the correct thing to do here
!                    expression = buildSubscriptExpression(false, false, false, false);
!and it seems to work.
!2. I couldn't get case ArraySpecElement_expr_colon_asterisk to work.  I suggest that the SgAsteriskShapeExp constructor take a lowerBound expression to allow for
!     real :: A(1:)
! I'm attaching a file that shows exhibits the various cases.
!
! Several examples of array declarations
!
subroutine bambino(A,B,C,D,E,F)
    ! ArraySpecElement_expr (700): explicit shape with bounds 1:10
    real :: A(11)

    ! ArraySpecElement_expr_colon (701): assumed shape with lower of 0
    real :: B(0:)

    ! ArraySpecElement_expr_colon_expr (702): explicit shape with bounds 0:10
    real :: C(0:12)    

    ! ArraySpecElement_expr_colon_asterisk (703): assumed size with lower bounds of 0
!    real :: D(0:*)    

    ! ArraySpecElement_asterisk (704): assumed size
    real :: E(*)    

    ! ArraySpecElement_colon (705): assumed shape
    real :: F(:)    
end subroutine bambino

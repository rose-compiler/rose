module rational_numbers
  implicit none
  private
  type, public :: rational
     private
     integer n, d
   contains
     ! ordinary type-bound procedure
     procedure :: real => rat_to_real
     ! specific type-bound procedures for generic support
     procedure, private :: rat_asgn_i, rat_plus_rat, rat_plus_i
     procedure, private, pass(b) :: i_plus_rat
     ! generic type-bound procedures
     generic :: assignment(=) => rat_asgn_i
     generic :: operator(+) => rat_plus_rat, rat_plus_i, i_plus_rat
  end type rational
contains
  elemental real function rat_to_real(this) result(r)
    class(rational), intent(in) :: this
    r = real(this%n)/this%d
  end function rat_to_real
  elemental subroutine rat_asgn_i(a,b)
    class(rational), intent(out) :: a
    integer, intent(in) :: b
    a%n = b
    a%d = 1
  end subroutine rat_asgn_i
  elemental type(rational) function rat_plus_i(a,b) result(r)
    class(rational), intent(in) :: a
    integer,intent(in) :: b
    r%n = a%n + b*a%d
    r%d = a%d
  end function rat_plus_i
  elemental type(rational) function i_plus_rat(a,b) result(r)
    integer,intent(in) :: a
    class(rational),intent(in) :: b
    r%n = b%n + a*b%d
    r%d = b%d
  end function i_plus_rat
  elemental type(rational) function rat_plus_rat(a,b) result(r)
    class(rational),intent(in) :: a,b
    r%n = a%n*b%d + b%n*a%d
    r%d = a%d*b%d
  end function rat_plus_rat
end 

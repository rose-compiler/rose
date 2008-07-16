module integer_sets
! This moudle is intended to illustrate use of the module facility
! to define a new type, along with suitable operators.

integer, parameter :: max_set_card = 200

type set               ! Define SET type
   private
   integer card
   integer element (max_set_card)
end type set

interface operator (.in.)
   module procedure element
end interface operator (.in.)

interface operator (<=)
   module procedure subset
end interface operator (<=)

interface operator (+)
   module procedure union
end interface operator (+)

interface operator (-)
   module procedure difference
end interface operator(-)

interface operator (*)
   module procedure intersection
end interface operator (*)

contains

  integer function cardinality (a)    ! Returns cardinality of set A
    type(set), intent(in) :: a
    cardinality = a % card
  end function cardinality

  logical function element(x, a)   ! Determines if element x is in set A
    integer, intent(in) :: x
    type(set), intent(in) :: a
    element = any (a%element(1:a%card) == x)
  end function element

  function union (a, b)            ! union of sets a and b
    type(set) union
    type(set), intent(in) :: a, b
    integer j
    union = a
    do j = 1, b%card
       if(.not. (b%element(j) .in. a)) then
          if (union%card < max_set_card) then
             union%card = union%card + 1
             union%element(union%card) = &
                  b%element(j)
          else
             ! Maximum set size exceeded
          end if
       end if
    end do
  end function union

  function difference (a,b)  ! Difference of sets A and B
    type(set) difference
    type(set), intent(in) :: A, B
    integer j, x
    difference%card = 0      ! The empty set
    do j = 1, A%card
       x = a%element(j)
       if (.not. (x .in. b)) difference = difference + set(1, x)
    end do
  end function difference

  function intersection (a, b)   ! Intersection of sets A and B
    type(set) intersection
    type(set), intent(in) :: a, b
    intersection = a - (a - b)
  end function intersection

  logical function subset(a, b)   ! Determines if set A is a subset of set B
    type(set), intent(in) :: a, b
    integer i
    subset = a%card <= b%card
    if (.not. subset) return      ! For efficiency
    do i = 1, a%card
       subset = subset .and. (a%element(i) .in. b)
    end do
  end function subset

  ! Transfer function between a vector of elements and a set of elements 
  ! removing duplicate elements
  type(set) function setf(v) 
    integer v(:)
    integer j
    setf%card = 0
    do j = 1, size(v)
       if(.not. (v (j) .in. setf)) then
          if(setf%card < max_set_card) then
             setf%card = setf%card+1
             setf%element(setf%card) = v(j)
          else
             ! Maximum set size exceeded....
          end if
       end if
    end do
  end function setf

  ! Transfer the values of set A into a vecotr in ascending order
  function vector(a)
    type(set), intent(in) :: A
    integer, pointer :: vector(:)
    integer i, j, k
    allocate (vector(a%card))
    vector = a%element(1:a%card)
    do i = 1, a%card - 1 ! use a better sort if a%card is large
       do j = i+1, a%card
          if(vector(i) > vector(j)) then
             k = vector(j); vector(j) = vector(i); vector(i) = k
          end if
       end do
    end do
  end function vector

end module integer_sets

! Check to see if A ha more than 10 elements
! added the print to complete the stmt
if(cardinality(a) > 10) then 
   print *, 'A has more than 10'
end if

! Check for X an element of A but not of B
! added the print to complete the stmt
if(x .in. (a-b)) then 
   print *, 'x is an element of a but not b'
end if

! C is the union of A and the result of B intersected
! with the integers 1 to 100
c = a + b * setf((/ (i, i = 1, 100) /))

! does a have any even numbers in the range 1:100?
! added the print to complete the stmt
if(cardinality (a*setf((/(i,I=2, 100, 2) /))) > 0) then 
   print *, 'even numbers...'
end if

print *, vector(b) ! Print out the elements of set b, in ascending order

end

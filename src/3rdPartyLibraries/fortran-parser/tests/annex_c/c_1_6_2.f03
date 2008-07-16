module m
  type t(kind)
     integer, kind :: kind  ! what does ', kind' mean??
     complex(kind) value
  end type t
  integer, parameter :: single = kind(0.0), double = kind(0d0)
  interface t
     module procedure real_to_t1, dble_to_t2, int_to_t1, int_to_t2
  end interface

contains
  type(t(single)) function real_to_t1(x)
    real(single) x
    real_to_t1%value = x
  end function real_to_t1
  type(t(double)) function dble_to_t2(x)
    real(double) x
    dble_to_t2%value = x
  end function dble_to_t2
  type(t(single)) function int_to_t1(x, modle)
    integer x
    type(t(single)) mold
    int_to_t1%value = x
  end function int_to_t1
  type(t(double)) function int_to_t2(x,mold)
    integer x
    type(t(double)) mold
    int_to_t2%value = x
  end function int_to_t2

end

program example
  use m
  type(t(single)) x
  type(t(double)) y
  x = t(1.5)                        ! References real_to_t1
  x = t(17,mold=x)                  ! References int_to_t1
  y = t(1.5d0)                      ! References dble_to_t2
  y = t(42,mold=y)                  ! References int_to_t2
  y = t(kind(0d0)) ((0,1))          ! Uses the structure constructor for type t
end

     

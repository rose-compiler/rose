module iso_c_binding

  INTEGER,PARAMETER :: c_int = 1
  INTEGER,PARAMETER :: c_short = 2
  INTEGER,PARAMETER :: c_long = 3
  INTEGER,PARAMETER :: c_long_long = 4
  INTEGER,PARAMETER :: c_signed_char = 5
  INTEGER,PARAMETER :: c_size_t = 6
  INTEGER,PARAMETER :: c_intmax_t = 7
  INTEGER,PARAMETER :: c_intptr_t = 8

  INTEGER,PARAMETER :: c_int8_t = 9
  INTEGER,PARAMETER :: c_int16_t = 10
  INTEGER,PARAMETER :: c_int32_t = 11
  INTEGER,PARAMETER :: c_int64_t = 12
  INTEGER,PARAMETER :: c_int_least8_t = 13
  INTEGER,PARAMETER :: c_int_least16_t = 14
  INTEGER,PARAMETER :: c_int_least32_t = 15
  INTEGER,PARAMETER :: c_int_least64_t = 16
  INTEGER,PARAMETER :: c_int_fast8_t = 17
  INTEGER,PARAMETER :: c_int_fast16_t = 18
  INTEGER,PARAMETER :: c_int_fast32_t = 19
  INTEGER,PARAMETER :: c_int_fast64_t = 20

! Kind parameter values for the C integral types int8_t to int_fast64_t. These are all kind values for type INTEGER.

  INTEGER,PARAMETER :: c_float = 21
  INTEGER,PARAMETER :: c_double = 22
  INTEGER,PARAMETER :: c_long_double = 23

! Kind parameter values for the C floating-point types float, double and long double. These are all kind values for type REAL.

  INTEGER,PARAMETER :: c_float_complex = c_float
  INTEGER,PARAMETER :: c_double_complex = c_double
  INTEGER,PARAMETER :: c_long_double_complex = c_long_double

! Kind parameter values for the C _Complex family of types. These always have exactly the same values as c_float et al, and are included only for unnecessary redundancy.

  INTEGER,PARAMETER :: c_bool = 24

! Kind parameter value for the C type _Bool, for use with the LOGICAL type.

  INTEGER,PARAMETER :: c_char = 25

! Kind parameter value for the C type char, for use with the CHARACTER type.
! Character Constants
! The following parameters give Fortran values for all of the C ``backslash'' escape sequences.
! Note that "achar()" is the Character in ASCII collating sequence.
  CHARACTER,PARAMETER :: c_null_char = char(0)         ! C '\0' 
  CHARACTER,PARAMETER :: c_alert = achar(7)            ! C '\a'
  CHARACTER,PARAMETER :: c_backspace = achar(8)        ! C '\b'
  CHARACTER,PARAMETER :: c_form_feed = achar(12)       ! C '\f'
  CHARACTER,PARAMETER :: c_new_line = achar(10)        ! C '\n'
  CHARACTER,PARAMETER :: c_carriage_return = achar(13) ! C '\r'
  CHARACTER,PARAMETER :: c_horizontal_tab = achar(9)   ! C '\t'
  CHARACTER,PARAMETER :: c_vertical_tab = achar(11)    ! C '\v'

! Pointer Constants

  TYPE(c_ptr),PARAMETER :: c_null_ptr = c_ptr()

! This is a C null pointer, equivalent to (void *)0 in C.

  TYPE(c_funptr),PARAMETER :: c_null_funptr = c_funptr()

! This is a C null function pointer.
! Type Definitions

  TYPE c_funptr
    PRIVATE
  END TYPE

! This type represents a C function pointer, and is used when passing procedure arguments
! to a C routine. The interface to the C routine is declared with a TYPE(c_funptr) dummy 
! argument, and values of this type can be created by using the function c_funloc on a 
! procedure name (see below for restrictions).

  TYPE c_ptr
    PRIVATE
  END TYPE

! This type represents a ``(void *)'' C data pointer, and is used when passing pointer 
! arguments to a C routine. The interface to the C routine is declared with a TYPE(c_ptr) 
! dummy argument; values of this type are created using the c_loc function (Fortran)
! pointer or target (see below for restrictions). A C pointer can be turned into a 
! Fortran pointer using the c_f_pointer function (see below for the full description).
! Procedure Descriptions

! All the procedures provided are generic and not specific. None of the procedures are pure.

! In the descriptions below, TYPE(*) means any type (including intrinsic types), and INTEGER(*) means any kind of INTEGER type.

  INTERFACE c_associated
    LOGICAL FUNCTION c_associated(c_ptr1,c_ptr2) ! Specific name not visible
      TYPE(c_ptr),INTENT(IN) :: c_ptr1,c_ptr2
      OPTIONAL c_ptr2
    END
    LOGICAL FUNCTION c_associated(c_ptr1,c_ptr2) ! Specific name not visible
      TYPE(c_funptr),INTENT(IN):: c_ptr1,c_ptr2
      OPTIONAL c_ptr2
    END
  END INTERFACE

! Returns true if and only if c_ptr1 is not a null pointer and, if c_ptr2 is present, the same as c_ptr2.

  INTERFACE c_f_pointer
    SUBROUTINE c_f_pointer(cptr,fptr) ! Specific name not visible
      TYPE(c_ptr),INTENT(IN) :: cptr

    ! DQ (10/26/2010): Not clear how to represent this tyoe(*).
    ! TYPE(*),INTENT(OUT),POINTER :: fptr
    END
    SUBROUTINE c_f_pointer(cptr,fptr,shape) ! Specific name not visible
      TYPE(c_ptr),INTENT(IN) :: cptr

    ! DQ (10/26/2010): Not clear how to represent this tyoe(*).
    ! TYPE(*),INTENT(OUT),POINTER :: fptr()
    ! INTEGER(*),INTENT(IN) :: shape(:)
    END
  END INTERFACE

! Converts a C address to a Fortran pointer. If fptr is an array, shape must be an array whose size is equal to the rank of fptr.

  INTERFACE c_f_procpointer
  END INTERFACE

! This procedure is not available in release 5.1. It converts TYPE(c_funptr) into Fortran procedure pointers.

  INTERFACE c_funloc
  TYPE(c_funptr) FUNCTION c_funloc(x) ! Specific name not visible
      EXTERNAL x
    END
  END INTERFACE

! Returns the C address of a Fortran procedure, which must be a dummy procedure, external procedure or module procedure, and must have the BIND(C) attribute.

! Note that since Fortran procedure pointers are not available in release 5.1, invoking a procedure through a TYPE(c_funptr) value can only be done from C.

  INTERFACE c_loc
    TYPE(c_ptr) FUNCTION c_loc(x) ! Specific name not visible

  ! DQ (10/26/2010): Not clear how to represent this type(*).
  ! TYPE(*),TARGET :: x
    END
  END INTERFACE

end module iso_c_binding

! I think this is using F03 syntax for procedure pointers.
! So this might really be a FFortran 2003 test code 
! (syntax fails for gfortran, but is part of the gfortran test suite).

PROGRAM test_prog

  PROCEDURE(add), POINTER :: forig, fset

  forig => add

  CALL set_ptr(forig,fset)

  if (forig(1,2) /= fset(1,2)) call abort()

CONTAINS

 SUBROUTINE set_ptr(f1,f2)
   PROCEDURE(add), POINTER :: f1, f2
    f2 => f1
  END SUBROUTINE set_ptr

  FUNCTION add(a,b)
    INTEGER :: a,b,add
    add = a+b

  END FUNCTION add
 
END PROGRAM test_prog


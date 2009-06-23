// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2:foldmethod=syntax
// test MITRE CWE-587, assignment of a fixed address to a pointer
#include <stdio.h>
#include <bits/wordsize.h>


#if __WORDSIZE == 64
  typedef long s_ptr;
#else
  typedef int s_ptr;
#endif



s_ptr return_const() {
  return 42;
}


int main() {

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Positive matches

  // fn ptr example direct from MITRE, except with added cast
  int (*pt2fn_fixed_addr) (float, char, char) = 
    (int (*)(float, char, char)) 0x08040000;

  int (*pt2fn_fixed_addr_nocast)() = (int (*)()) 0x08040000;

  int (*pt2fn_arith_addr) (float, char, char) = 
    (int (*)(float, char, char)) (0x08040000 + 0x200);

  int (*pt2fn_assgn_fixed_addr) (float, char, char);
  pt2fn_assgn_fixed_addr = (int (*)(float, char, char)) 0x08040000;


  // arrays of function pointers

  typedef int (*fnptr)(void);
  fnptr f[2] = {
    (fnptr) 0x08010000,
    (fnptr) 0x08020000
  };
  int (*pt2fn_array_fixed_addr[2])(void) = {
    (int (*)(void)) 0x08010000,
    (int (*)(void)) 0x08020000
  };

  typedef fnptr ind_fnptr;
  fnptr f_indirect[2] = {
    (fnptr) 0x08010000,
    (fnptr) 0x08020000
  };


  int (*pt2fn_arith_implicit_cast_addr) (float, char, char) = 
    (int (*)(float, char, char)) 0x08010000 + 0x10;

  int* pint_plus_zero = (int *) 0x2 + 0;


  /* False negative

  int (*pt2fn_arith_fn_addr) (float, char, char) = 
    (int (*)(float, char, char)) (0x08040000 + return_const());
   // */


  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Negative matches

  s_ptr x = getchar();

  // legal constant array
  int arr[2] = {
    0x01,
    0x02
  };

  // legal since not a fixed addr
  int (*pt2fn_arith_input_addr) (float, char, char) = 
    (int (*)(float, char, char)) (0x08040000 + x);


  // NULL & 0 are the only legal exceptions.
  int (*pt2fn_null) (float, char, char) = NULL;
  int (*pt2fn_zero) (float, char, char) = 0;

  // legal since lhs not a pointer
  int y = 0x08040000;
  int z;
  z = 0x08050000;

  // legal since rhs is not fixed
  s_ptr (*p2fn_foldable)() = &return_const;

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  return 0;
}

/* Expected Output:

DoNotAssignPointerToFixedAddress: doNotAssignPointerToFixedAddressTest1.c:27.3: Pointer pt2fn_fixed_addr is initialized to a fixed address.
DoNotAssignPointerToFixedAddress: doNotAssignPointerToFixedAddressTest1.c:30.3: Pointer pt2fn_fixed_addr_nocast is initialized to a fixed address.
DoNotAssignPointerToFixedAddress: doNotAssignPointerToFixedAddressTest1.c:32.3: Pointer pt2fn_arith_addr is initialized to a fixed address.
DoNotAssignPointerToFixedAddress: doNotAssignPointerToFixedAddressTest1.c:36.3-66: Pointer is assigned to a fixed address.
DoNotAssignPointerToFixedAddress: doNotAssignPointerToFixedAddressTest1.c:42.3: Array of pointers f has an element initialized to a fixed address.
DoNotAssignPointerToFixedAddress: doNotAssignPointerToFixedAddressTest1.c:46.3: Array of pointers pt2fn_array_fixed_addr has an element initialized to a fixed address.
DoNotAssignPointerToFixedAddress: doNotAssignPointerToFixedAddressTest1.c:52.3: Array of pointers f_indirect has an element initialized to a fixed address.
DoNotAssignPointerToFixedAddress: doNotAssignPointerToFixedAddressTest1.c:58.3: Pointer pt2fn_arith_implicit_cast_addr is initialized to a fixed address.
DoNotAssignPointerToFixedAddress: doNotAssignPointerToFixedAddressTest1.c:61.3: Pointer pint_plus_zero is initialized to a fixed address.

*/

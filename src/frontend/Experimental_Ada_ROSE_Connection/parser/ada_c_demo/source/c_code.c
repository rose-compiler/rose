#include <stdio.h>
#include "ada_code.h"

void c_func () {
  printf ("c_func: Begin\n");
  printf ("c_func: End\n");
}

// "Ada_Proc ();" call below does not link in
//    ada_c_main.adb OR ada_c_ada_main.adb. Error is:
//  ld: libada_c_demo.a(c_code.o): in function `c_ada_caller':
//  c_code.c:(.text+0x2e): undefined reference to `Ada_Proc'
// "Ada_Proc;" does link in ada_c_ada_main.adb, but of course
// does not get called (It's an expression returning the function,
// not a function call.)

void c_ada_caller () {
  printf ("c_ada_caller: Calling Ada_Proc\n");
  printf ("c_ada_caller: INCOMPLETE (see source code)\n");
//  Ada_Proc ();
  printf ("c_ada_caller: Returned from Ada_Proc\n");
}


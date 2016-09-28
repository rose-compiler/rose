// t0342.cc
// test 5.3.1 paras 6--9

struct S {};

void foo()
{
  char c;
  int i;
  unsigned u;
  long L;
  float f;
  double d;
  int *p;
  S s;

  // 5.3.1 para 6
  __checkType(+c, (int)0);
  __checkType(+i, (int)0);
  __checkType(+u, (unsigned int)0);
  __checkType(+L, (long)0);
  __checkType(+f, (float)0);
  __checkType(+d, (double)0);
  __checkType(+p, (int *)0);
  //ERROR(1): +s;    // '+' to struct
  //ERROR(2): +foo;  // '+' to function

  // 5.3.1 para 7
  __checkType(-c, (int)0);
  __checkType(-i, (int)0);
  __checkType(-u, (unsigned int)0);
  __checkType(-L, (long)0);
  __checkType(-f, (float)0);
  __checkType(-d, (double)0);
  //ERROR(3): -p;    // '-' to pointer
  //ERROR(4): -s;    // '-' to struct
  //ERROR(5): -foo;  // '-' to function

  // 5.3.1 para 8
  __checkType(!c, (bool)0);
  __checkType(!i, (bool)0);
  __checkType(!u, (bool)0);
  __checkType(!L, (bool)0);
  __checkType(!f, (bool)0);
  __checkType(!d, (bool)0);
  __checkType(!p, (bool)0);
  //ERROR(6): !s;    // '!' to struct
  // Elsa allows converting a function to bool, and I think that is
  // right, but I'm not sure so I am not going to test it.

  // 5.3.1 para 9
  __checkType(~c, (int)0);
  __checkType(~i, (int)0);
  __checkType(~u, (unsigned int)0);
  __checkType(~L, (long)0);
  //ERROR(7): ~f;    // '~' to floating point
  //ERROR(8): ~d;    // '~' to floating point
  //ERROR(9): ~p;    // '~' to pointer
  //ERROR(10): ~s;   // '~' to struct
  //ERROR(11): ~foo; // '~' to function
  
  // see also t0343.cc
}

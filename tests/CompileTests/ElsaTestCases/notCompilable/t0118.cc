// t0118.cc
// test getImplicitConversion

// copied from stdconv.h
enum StandardConversion {
  SC_IDENTITY        = 0x00,  // types are identical

  // conversion group 1 (comes first)
  SC_LVAL_TO_RVAL    = 0x01,  // 4.1: int& -> int
  SC_ARRAY_TO_PTR    = 0x02,  // 4.2: char[] -> char*
  SC_FUNC_TO_PTR     = 0x03,  // 4.3: int ()(int) -> int (*)(int)
  SC_GROUP_1_MASK    = 0x03,

  // conversion group 3 (comes last conceptually)
  SC_QUAL_CONV       = 0x04,  // 4.4: int* -> int const*
  SC_GROUP_3_MASK    = 0x04,

  // conversion group 2 (goes in the middle)
  SC_INT_PROM        = 0x10,  // 4.5: int... -> int..., no info loss possible
  SC_FLOAT_PROM      = 0x20,  // 4.6: float -> double, no info loss possible
  SC_INT_CONV        = 0x30,  // 4.7: int... -> int..., info loss possible
  SC_FLOAT_CONV      = 0x40,  // 4.8: float... -> float..., info loss possible
  SC_FLOAT_INT_CONV  = 0x50,  // 4.9: int... <-> float..., info loss possible
  SC_PTR_CONV        = 0x60,  // 4.10: 0 -> Foo*, Child* -> Parent*
  SC_PTR_MEMB_CONV   = 0x70,  // 4.11: int Child::* -> int Parent::*
  SC_BOOL_CONV       = 0x80,  // 4.12: various types <-> bool
  SC_GROUP_2_MASK    = 0xF0,

  SC_ERROR           = 0xFF,  // cannot convert
};

// copied from implconv.h
enum Kind {
  IC_NONE,             // no conversion possible
  IC_STANDARD,         // 13.3.3.1.1: standard conversion sequence
  IC_USER_DEFINED,     // 13.3.3.1.2: user-defined conversion sequence
  IC_ELLIPSIS,         // 13.3.3.1.3: ellipsis conversion sequence
  IC_AMBIGUOUS,        // 13.3.3.1 para 10
  NUM_KINDS
} kind;


struct A {
};

struct B {
  B(A const &);                    // line 47
};

struct C : A {   operator int();   // line 50
};


struct D {
  operator int();      // line 55
  operator float();    // line 56
};

struct E {
  operator char();     // line 60
  operator float();    // line 61
};

struct F {
  operator bool ();               // line 65
  operator char ();               // line 66
  operator signed char ();        // line 67
  operator unsigned char ();      // line 68
  operator short ();              // line 69
  operator unsigned short ();     // line 70
  operator int ();                // line 71
  operator unsigned int ();       // line 72
  operator long ();               // line 73
  operator unsigned long ();      // line 74
  operator float ();              // line 75
  operator double ();             // line 76
  operator long double ();        // line 77
};

class G {
  operator A* ();                 // line 81
  operator C* ();                 // line 82
};

class H {
  operator A* ();                 // line 86
  operator B* ();                 // line 87
};

class I {
  operator int ();                // line 91
  operator int& ();               // line 92
}; 

class J {
  J(int);                         // line 96
  J(long);                        // line 97
};

class L;
class K {
  K(L const &);                   // line 102
};

class L {
  operator K ();                  // line 106
};

class L2;
class K2 {
  K2(L2 const &);                 // line 111
};

class L2 {
  operator K2 ();                 // line 115
  operator K2& ();                // line 116
};

// __getImplicitConversion(
//   <expression with source type>,
//   <expression with destination type>,
//   <implicit conversion kind>,
//   <standard conversion 1>,
//   <line number of user-defined conversion, or 0 if none>,
//   <standard conversion 2, or 0 if none>
// )


void f()
{
  // elementary conversions
  __getImplicitConversion((int)0, (int)0,
                          IC_STANDARD, SC_IDENTITY, 0, 0);
  __getImplicitConversion((int)0, (int const &)0,
                          IC_STANDARD, SC_IDENTITY,0,0);

  // constructor conversions
  A a;
  B b(a);
  C c;
  __getImplicitConversion((A)a, (B)b,
                          IC_USER_DEFINED, SC_IDENTITY, 47, SC_IDENTITY);
  __getImplicitConversion((A&)a, (B)b,
                          IC_USER_DEFINED, SC_IDENTITY, 47, SC_IDENTITY);
  __getImplicitConversion((C&)c, (B)b,
                          IC_USER_DEFINED, SC_PTR_CONV, 47, SC_IDENTITY);
  __getImplicitConversion(c, (int)0,
                          IC_USER_DEFINED, SC_IDENTITY, 50, SC_IDENTITY);

  // operator conversions
  D d;
  __getImplicitConversion(d, (int)0,
                          IC_USER_DEFINED, SC_IDENTITY, 55, SC_IDENTITY);
  __getImplicitConversion(d, (float)0,
                          IC_USER_DEFINED, SC_IDENTITY, 56, SC_IDENTITY);

  // int->char and float->char are both conversions, therefore it's ambiguous
  __getImplicitConversion(d, (char)0,
                          IC_AMBIGUOUS, 0,0,0);

  E e;
  // char->int is a promotion, therefore preferred
  __getImplicitConversion(e, (int)0,
                          IC_USER_DEFINED, SC_IDENTITY, 60, SC_INT_PROM);
  // float->double also a promotion
  __getImplicitConversion(e, (double)0,
                          IC_USER_DEFINED, SC_IDENTITY, 61, SC_FLOAT_PROM);

  F f;
  __getImplicitConversion(f, (bool)0,
                          IC_USER_DEFINED, SC_IDENTITY, 65, SC_IDENTITY);
  __getImplicitConversion(f, (unsigned short)0,
                          IC_USER_DEFINED, SC_IDENTITY, 70, SC_IDENTITY);
  __getImplicitConversion(f, (double)0,
                          IC_USER_DEFINED, SC_IDENTITY, 76, SC_IDENTITY);
                          
  G g;
  __getImplicitConversion(g, (A*)0,
                          IC_USER_DEFINED, SC_IDENTITY, 81, SC_IDENTITY);
  __getImplicitConversion(g, (B*)0,
                          IC_NONE, 0,0,0);
  __getImplicitConversion(g, (C*)0,
                          IC_USER_DEFINED, SC_IDENTITY, 82, SC_IDENTITY);
  __getImplicitConversion(g, (void*)0,
                          IC_USER_DEFINED, SC_IDENTITY, 81, SC_PTR_CONV);

  H h;
  __getImplicitConversion(h, (A*)0,
                          IC_USER_DEFINED, SC_IDENTITY, 86, SC_IDENTITY);
  __getImplicitConversion(h, (B*)0,
                          IC_USER_DEFINED, SC_IDENTITY, 87, SC_IDENTITY);
  __getImplicitConversion(h, (void*)0,
                          IC_AMBIGUOUS, 0,0,0);

  I i;
  int anInt;
  __getImplicitConversion(i, anInt /*int&*/,
                          IC_USER_DEFINED, SC_IDENTITY, 92, SC_IDENTITY);

  J j(1);
  __getImplicitConversion((int)0, (J)j,
                          IC_USER_DEFINED, SC_IDENTITY, 96, SC_IDENTITY);
  __getImplicitConversion((long)0, (J)j,
                          IC_USER_DEFINED, SC_IDENTITY, 97, SC_IDENTITY);
  __getImplicitConversion((float)0, (J)j,
                          IC_AMBIGUOUS, 0,0,0);

  // ambiguity between conversion operator and conversion constructor
  L ell;
  K k(ell);
  __getImplicitConversion(ell, (K)k,
                          IC_AMBIGUOUS, 0,0,0);

  // ambiguity among 3: ambiguous conversion operator, and conversion constructor
  L2 ell2;
  K2 k2(ell2);
  __getImplicitConversion(ell2, (K2)k2,
                          IC_AMBIGUOUS, 0,0,0);

}

// t0117.cc
// test 'getStandardConversion' in stdconv.cc

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

class Foo {};
class Incomplete;
enum Enum {};
enum Enum2 {};

class Base {};
class Ambiguous {};
class Amb1 : public Ambiguous {};     // workaround for inherent ambiguity
class Amb2 : public Ambiguous {};     // of directly inheriting same class twice
class Virtual {};
class Derived : public Base, public Amb1, public Amb2,
                virtual public Virtual {};

void f()
{
  // identity
  __getStandardConversion((int)0, (int)0, SC_IDENTITY);
  __getStandardConversion((Foo)0, (Foo)0, SC_IDENTITY);
  __getStandardConversion((Foo &)0, (Foo &)0, SC_IDENTITY);

  // lval to rval
  __getStandardConversion((int &)0, (int)0, SC_LVAL_TO_RVAL);
  __getStandardConversion((int const &)0, (int)0, SC_LVAL_TO_RVAL);
  __getStandardConversion((Foo &)0, (Foo)0, SC_LVAL_TO_RVAL);
  __getStandardConversion((Foo const &)0, (Foo const)0, SC_LVAL_TO_RVAL);

  // I can't tell if this is really supposed to be an error.. and
  // for now my implementation doesn't classify it as such
  //__getStandardConversion((Foo const &)0, (Foo)0, SC_ERROR);

  // this now fails with a different error mode than SC_ERROR
  //ERROR(1): __getStandardConversion((Incomplete &)0, (Incomplete)0, SC_ERROR);

  // binding to references
  __getStandardConversion((int)0, (int const &)0, SC_IDENTITY);
  __getStandardConversion((int*)0, (int * const &)0, SC_IDENTITY);
  __getStandardConversion((Foo)0, (Foo const &)0, SC_IDENTITY);
  __getStandardConversion((Foo &)0, (Foo const &)0, SC_IDENTITY);
  __getStandardConversion((Derived)0, (Derived const &)0, SC_IDENTITY);
  __getStandardConversion((Derived)0, (Base const &)0, SC_PTR_CONV);
                                                            
  // binding an rvalue to a non-const reference
  __getStandardConversion((Derived)0, (Derived &)0, SC_ERROR);
  __getStandardConversion((Derived)0, (Base &)0, SC_ERROR);

  // I'm inferring the following from the second example in
  // section 13.3.3.2 para 3
  __getStandardConversion((int&)0, (int const&)0, SC_IDENTITY);  // not SC_QUAL_CONV!
  __getStandardConversion((int const&)0, (int&)0, SC_ERROR);
  __getStandardConversion((int const&)0, (int const&)0, SC_IDENTITY);
  __getStandardConversion((int &)0, (int &)0, SC_IDENTITY);

  // array to pointer
  extern int intArr[];
  extern int const intArrC[];
  __getStandardConversion((int [3])0, (int*)0, SC_ARRAY_TO_PTR);
  __getStandardConversion(intArr, (int*)0, SC_ARRAY_TO_PTR);
  __getStandardConversion("abc", (char const*)0, SC_ARRAY_TO_PTR);
  __getStandardConversion(L"abc", (wchar_t const*)0, SC_ARRAY_TO_PTR);
  __getStandardConversion("abc", (char*)0, SC_ARRAY_TO_PTR|SC_QUAL_CONV);

  __getStandardConversion(intArrC, (int*)0, SC_ERROR);
  __getStandardConversion((int (&)[])0, (int*)0, SC_ARRAY_TO_PTR);

  // function to pointer
  typedef int func(int*);
  __getStandardConversion((func)0, (int (*)(int*))0, SC_FUNC_TO_PTR);
  __getStandardConversion((func)0, (int (*)(int const*))0, SC_ERROR);

  // qualification conversions
  __getStandardConversion((int *)0, (int const *)0, SC_QUAL_CONV);
  __getStandardConversion((int **)0, (int const * const *)0, SC_QUAL_CONV);
  __getStandardConversion((int * const *)0, (int const * const *)0, SC_QUAL_CONV);
  __getStandardConversion((int const **)0, (int const * const *)0, SC_QUAL_CONV);

  __getStandardConversion((int **)0, (int const **)0, SC_ERROR);
  __getStandardConversion((int const * const *)0, (int const **)0, SC_ERROR);
  __getStandardConversion((int const * const *)0, (int * const *)0, SC_ERROR);
  __getStandardConversion((int const * const *)0, (int **)0, SC_ERROR);

  __getStandardConversion((int Foo::*)0, (int const Foo::*)0, SC_QUAL_CONV);
  __getStandardConversion((int Foo::* *)0, (int const Foo::* const *)0, SC_QUAL_CONV);
  __getStandardConversion((int * Foo::*)0, (int const * const Foo::*)0, SC_QUAL_CONV);
  __getStandardConversion((int * Foo::*)0, (int const * Foo::*)0, SC_ERROR);

  // integral promotions
  __getStandardConversion((char)0, (int)0, SC_INT_PROM);
  __getStandardConversion((signed char)0, (int)0, SC_INT_PROM);
  __getStandardConversion((unsigned char)0, (int)0, SC_INT_PROM);
  __getStandardConversion((short)0, (int)0, SC_INT_PROM);
  __getStandardConversion((unsigned short)0, (int)0, SC_INT_PROM);
  __getStandardConversion((wchar_t)0, (int)0, SC_INT_PROM);
  __getStandardConversion((Enum)0, (int)0, SC_INT_PROM);
  __getStandardConversion((bool)0, (int)0, SC_INT_PROM);

  __getStandardConversion((int)0, (Enum)0, SC_ERROR);
  __getStandardConversion((Enum2)0, (Enum)0, SC_ERROR);
  __getStandardConversion((Enum)0, (Enum)0, SC_IDENTITY);

  // TODO: test bitfields once they're implemented as distinct types

  // floating point promotion
  __getStandardConversion((float)0, (double)0, SC_FLOAT_PROM);

  // integral conversions
  __getStandardConversion((short)0, (unsigned)0, SC_INT_CONV);
  __getStandardConversion((short)0, (long)0, SC_INT_CONV);
  __getStandardConversion((long)0, (int)0, SC_INT_CONV);
  __getStandardConversion((short)0, (char)0, SC_INT_CONV);
  __getStandardConversion((short)0, (unsigned short)0, SC_INT_CONV);
  __getStandardConversion((char)0, (signed char)0, SC_INT_CONV);
  __getStandardConversion((bool)0, (char)0, SC_INT_CONV);

  // floating point conversions
  __getStandardConversion((float)0, (long double)0, SC_FLOAT_CONV);
  __getStandardConversion((double)0, (float)0, SC_FLOAT_CONV);

  // floating-integral conversions
  __getStandardConversion((short)0, (float)0, SC_FLOAT_INT_CONV);
  __getStandardConversion((float)0, (short)0, SC_FLOAT_INT_CONV);
  __getStandardConversion((bool)0, (float)0, SC_FLOAT_INT_CONV);

  // pointer conversions
  __getStandardConversion(0, (int *)0, SC_PTR_CONV);
  __getStandardConversion(0, (int const *)0, SC_PTR_CONV);
  __getStandardConversion(0, (Foo *)0, SC_PTR_CONV);
  __getStandardConversion(0, (Foo const * const *)0, SC_PTR_CONV);
  __getStandardConversion(1, (int *)0, SC_ERROR);

  __getStandardConversion((int *)0, (void *)0, SC_PTR_CONV);
  __getStandardConversion((int const *)0, (void const *)0, SC_PTR_CONV);
  __getStandardConversion((Foo *)0, (void *)0, SC_PTR_CONV);
  __getStandardConversion((Foo const *)0, (void const *)0, SC_PTR_CONV);

  __getStandardConversion((int const *)0, (void *)0, SC_ERROR);
  __getStandardConversion((void *)0, (int *)0, SC_ERROR);
  __getStandardConversion((Foo *)0, (int *)0, SC_ERROR);
  __getStandardConversion((float *)0, (int *)0, SC_ERROR);

  __getStandardConversion((Derived *)0, (Base *)0, SC_PTR_CONV);
  __getStandardConversion((Derived const *)0, (Base const *)0, SC_PTR_CONV);
  __getStandardConversion((Derived *)0, (Base const *)0, SC_PTR_CONV|SC_QUAL_CONV);
  __getStandardConversion((Derived *)0, (Virtual *)0, SC_PTR_CONV);

  __getStandardConversion((Base *)0, (Derived *)0, SC_ERROR);
  __getStandardConversion((Derived *)0, (Ambiguous *)0, SC_ERROR);

  // pointer to member conversions
  __getStandardConversion(0, (int Foo::*)0, SC_PTR_MEMB_CONV);
  __getStandardConversion(0, (int const Foo::*)0, SC_PTR_MEMB_CONV);

  __getStandardConversion((int Base::*)0, (int Derived::*)0, SC_PTR_MEMB_CONV);
  __getStandardConversion((int Base::*)0, (int const Derived::*)0, SC_PTR_MEMB_CONV|SC_QUAL_CONV);

  __getStandardConversion((int Base::* *)0, (int Derived::* *)0, SC_ERROR);
  __getStandardConversion((int Base::*)0, (float Derived::*)0, SC_ERROR);
  __getStandardConversion((int Derived::*)0, (int Base::*)0, SC_ERROR);
  __getStandardConversion((int Ambiguous::*)0, (int Derived::*)0, SC_ERROR);
  __getStandardConversion((int Virtual::*)0, (int Derived::*)0, SC_ERROR);

  // boolean conversions
  __getStandardConversion((int)0, (bool)0, SC_BOOL_CONV);
  __getStandardConversion((float)0, (bool)0, SC_BOOL_CONV);
  __getStandardConversion((Enum)0, (bool)0, SC_BOOL_CONV);
  __getStandardConversion((int *)0, (bool)0, SC_BOOL_CONV);
  __getStandardConversion((int Foo::*)0, (bool)0, SC_BOOL_CONV);

  __getStandardConversion((Foo)0, (bool)0, SC_ERROR);

}

// attr02.c
// these tests focus on the placement of attribute specifier lists,
// largely ignoring the syntax of what's in the (( and ))

// http://gcc.gnu.org/onlinedocs/gcc-3.1/gcc/Attribute-Syntax.html


// an attribute specifier list may appear af the colon following a label
void f()
{
  label: __attribute__((unused))
    goto label;
}


// An attribute specifier list may appear as part of a struct, union
// or enum specifier.

  // It may go either immediately after the struct, union or enum keyword,
  struct __attribute__((blah)) Struct1 {};
  union __attribute__((blah)) Union1 {};
  enum __attribute__((blah)) Enum1 { enum1 };

  // or after the closing brace.
  struct Struct2 {} __attribute__((blah));
  struct Struct2a {} __attribute__((blah)) some_declarator;
  union Union2 {} __attribute__((blah));
  enum Enum2 { enum2 } __attribute__((blah));

  // subsequent text indicates it is allowed (though ignored) after
  // the keyword in an elaborated type specifier (no "{}")
  struct __attribute__((blah)) Struct1 *s1p;
  union __attribute__((blah)) Union1 *u1p;
  enum __attribute__((blah)) Enum1 *e1p;


// Otherwise, an attribute specifier appears as part of a declaration,
// counting declarations of unnamed parameters and type names
// [sm: so, they regard parameters as being "declarations"]
//
// Any list of specifiers and qualifiers at the start of a declaration
// may contain attribute specifiers, whether or not such a list may in
// that context contain storage class specifiers.
// [sm: what does "contain" main?  arbitrarily intermixed?  seems so...]
// 
// my speculation on the places that a "declaration" can occur:
//   - toplevel
//   - function scope
//   - struct member list
//   - function parameter list
            

// toplevel:
        __attribute__((blah)) int                              x1;
        __attribute__((blah)) int __attribute__((blah))        x2;
static  __attribute__((blah)) int __attribute__((blah))        x3;
        __attribute__((blah)) int __attribute__((blah)) static x4;
typedef __attribute__((blah)) int __attribute__((blah))        x5;

// function scope:
void g()
{
          __attribute__((blah)) int                              x1;
          __attribute__((blah)) int __attribute__((blah))        x2;
  static  __attribute__((blah)) int __attribute__((blah))        x3;
          __attribute__((blah)) int __attribute__((blah)) static x4;
  typedef __attribute__((blah)) int __attribute__((blah))        x5;
}

// struct member list (only in gcc >= 3):
struct Struct3 {
  #if defined(__GNUC__) && __GNUC__ >= 3
          __attribute__((blah)) int                              x1;
          __attribute__((blah)) int __attribute__((blah))        x2;
  short   __attribute__((blah)) int __attribute__((blah))        x3;
  #endif
          int                              x4   __attribute__((blah)) ;
};

// function parameter list (declaration declarator)
int f1(__attribute((blah)) int x);
int f2(short __attribute((blah)) int x);
int f3(__attribute((blah)) int x, __attribute((blah)) int y);

// and definition declarators
int g1(__attribute((blah)) int x) {}
int g2(short __attribute((blah)) int x) {}
int g3(__attribute((blah)) int x, __attribute((blah)) int y) {}


// In the obsolescent usage where a type of int is implied by the
// absence of type specifiers, such a list of specifiers and
// qualifiers may be an attribute specifier list with no other
// specifiers or qualifiers.
//
// I have commented this out because Elsa does not have implicit-int
// support.
//         __attribute__((blah)) /*implicit-int*/                   x6;


// An attribute specifier list may appear immediately before a
// declarator (other than the first) in a comma-separated list of
// declarators in a declaration of more than one identifier using a
// single list of specifiers and qualifiers.
int a1, __attribute__((blah)) a2;
int b1, __attribute__((blah)) *b2;
int c1, * __attribute__((blah)) c2;     // nested declarators?  guess so...
int * __attribute__((blah)) d1;         // nested declarator on *first* one
int ( __attribute__((blah)) e1);        // nested declarator inside paren
int ( __attribute__((blah)) h1), ( __attribute__((blah)) h2);
//int ( i1 __attribute__((blah)) );       // end of parenthesized?  NO

// mixed in with cv-qualifiers?  only in gcc >= 3
int c1c, * const __attribute__((blah)) c2c;
#if defined(__GNUC__) && __GNUC__ >= 3
int c1cv, * const __attribute__((blah)) volatile c2cv;
#endif

// try this in function scope too
void h()
{
  int a1, __attribute__((blah)) a2;
  int b1, __attribute__((blah)) *b2;
  int c1, * __attribute__((blah)) c2;     // nested declarator
  int * __attribute__((blah)) d1;         // nested declarator on *first* one
  int ( __attribute__((blah)) e1);        // nested declarator inside paren
  int ( __attribute__((blah)) h1), ( __attribute__((blah)) h2);
}

// example from the manual; only works in gcc >= 3
#if defined(__GNUC__) && __GNUC__ >= 3
__attribute__((noreturn)) void ex_d0 (void),
         __attribute__((format(printf, 1, 2))) ex_d1 (const char *, ...),
          ex_d2 (void)   ;   // why was the semicolon missing?
#endif


// An attribute specifier list may appear immediately before the
// comma, = or semicolon terminating the declaration of an identifier
// other than a function definition.
int aa1 __attribute__((blah));
int bb1 __attribute__((blah)) = 2;
int cc1 __attribute__((blah)), cc2;
int dd1 __attribute__((blah)), dd2 __attribute__((blah));

int ee1(int) __attribute__((blah));
//int ff1(int) __attribute__((blah)) {}    // this one isn't allowed

// example from the manual
void (****gg1)(void) __attribute__((noreturn));


// again, testing attributes *after* declarator, in a number of contexts:

// toplevel:
        int y1 __attribute__((blah));
typedef int y2 __attribute__((blah));

// function scope:
void yg()
{
          int y1 __attribute__((blah));
  typedef int y2 __attribute__((blah));
}

// struct member list
struct yStruct3 {
  int x1 __attribute__((blah)) ;
  int x2 : 2 __attribute__((blah)) ;
};

// function parameter list (declaration declarator)
int yf1(int x __attribute((blah)));
int yf3(int x __attribute((blah)), int y __attribute((blah)));

// and definition declarators
int yg1(int x __attribute((blah))) {}
int yg3(int x __attribute((blah)), int y __attribute((blah))) {}


// a few more nested declarator examples from the manual
void (__attribute__((noreturn)) ****hh1) (void);
char *__attribute__((aligned(8))) *ii1;


// NOTE: I did not test the thing about qualifiers inside [].


// EOF

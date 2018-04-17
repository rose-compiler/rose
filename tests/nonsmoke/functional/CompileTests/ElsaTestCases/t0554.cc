// t0554.cc
// explicit specialization of static data member

// see 14.5.1.3 (ordinary defns) and 14.7.3 (explicit specs)


template <class T>
struct A {
  static int a;
  static int b;
  static int c;

  static T t;
  static T u;
  static T v;
};


// ordinary definition w/o initializer
template <class T>
int A<T>::a;

// repeated definition
//ERROR(1): template <class T>
//ERROR(1): int A<T>::a;


// ordinary definition with initializer
template <class T>
int A<T>::b = 3;

// repeated
//ERROR(2): template <class T>
//ERROR(2): int A<T>::b = 3;


// ordinary definition of member with dependent return type
template <class T>
T A<T>::t;


// ordinary definition, initialized by ctor call
template <class T>
T A<T>::u(1,2,3);


// another ordinary definition; this will not be explicitly
// specialized, but will also not prevent A<int> from being
// instantiated because 'v' will not be used anywhere
template <class T>
T A<T>::v(1,2,3,4,5);


// explicit specialization *declaration*
template <>
int A<int>::a;

// not an error!  just another declaration
template <>
int A<int>::a;

// explicit specialization *definition*
template <>
int A<int>::a = 1;

// duplicate definition
//ERROR(3): template <>
//ERROR(3): int A<int>::a = 1;
                        

// another explicit spec
template <>
int A<int>::b = 2;


// another, using a typedef (see also in/t0555.cc)
typedef A<int> A_int;
template <>
int A_int::c = 3;


// explicit specialization with dependent type and ctor init
template <>           //ERRORIFMISSING(4): required
int A<int>::u(1);     //ERRORIFMISSING(4): required


void foo()
{
  // this must find the explicit spec of 'u' because otherwise
  // the normal definition will try to call int(int,int,int)
  A<int> a;
  
  // 14.7.1p1: 'u' must be used in some way to trigger the
  // instantiation of its definition, needed for error 4
  a.u;
}



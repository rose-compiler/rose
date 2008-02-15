// t0452.cc
// coverage: template stuff

//ERROR(1): template <class T>
//ERROR(1): int A<T>::foo() { return 2; }

template <class T>
struct A {
  template <class U>
  struct B {
    int foo();
    int foo2();

    typedef int INT;
    INT bar();
  };
  
  int f();
};

template <class T>
template <class U>
int A<T>::B<U>::foo() 
{
  int x;
  //ERROR(6): x = sizeof(typename INT);
  return x;
}

template <class T>
template <class U>
typename           //ERRORIFMISSING(2): required
          A<T>::
                template                 //ERRORIFMISSING(3): required
                          B<U>::INT  A<T>::
                                            //template    // required? no
                                                      B<U>::bar() { return 3; }

// Is the second 'template' required?  14.2p4 is where the requirement
// to specify 'template' is stated, but it is hard to interpret.  icc
// accepts it but does not require it; gcc rejects it.  For now, Elsa
// behaves like icc, but I will not test that behavior.
//
// Thinking more, I believe icc is right:
//   - p4 says it is *required* only when the member template name
//     is dependent.  But A<T>::B<U> is not dependent on what T is;
//     we simply match A<T> with existing specializations, and the
//     result of that match is used regardless of what value T might
//     have in a particular instantiation.
//   - p5 says it is *illegal* if either
//     (1) it does not precede a member template, but here A<T>::B<U>
//         is of course a member template, or
//     (2) it is not in the scope of a template.
//     Now, it is a little unclear what "in the scope of a template"
//     means, since we are in the declarator portion of a template.
//     But notice that the 'template' keyword is clearly required
//     in the type specifier, and if that is "in the scope" then
//     the declarator is too.  I think "in the scope" simply means
//     in the scope of template *parameters*, as opposed to the scope
//     of a particular template.

// so here is one where 'template' *is* used, and I believe allowed
// (gcc rejects)
template <class T>
template <class U>
int A<T>::template B<U>::foo2() { return 3; }

//ERROR(4): template <class T>
//ERROR(4): template <class U>
//ERROR(4): int AA<T>::BB<U>::foo() { return 3; }

// wrong template arg
//ERROR(5): template <class T1>
//ERROR(5): int A<T2>::f() { return 4; }



struct S {
  int x;
};

//ERROR(7): template <>
//ERROR(7): struct S<int> { int y; };


//ERROR(8): template <>
//ERROR(8): struct A<nonexist> { int z; };


template <class T>
struct C {
  static int x;
};

// specialize a non-function
template <>
int C<int>::x = 5;

// specialize a non-member non-function
//ERROR(9): template <>
//ERROR(9): int y<int>;


template <class T>
void dependentPTM(T *t)
{
  &T::x;     // dependent PTM type
}




// EOF

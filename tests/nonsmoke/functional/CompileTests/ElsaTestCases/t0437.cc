// t0437.cc
// type specifier PseudoInstantiations

template <class T>
struct A {
  A* f1();
  A<T>* f2();

  static A *p3;
  static A<T> *p4;
};

template <class T1>
A<T1>* A<T1>::f1()
{
  return this;
}

template <class T2>
A<T2>* A<T2>::f2()
{
  return this;
}

template <class T3>
A<T3>* A<T3>::p3 = 0;

template <class T4>
A<T4>* A<T4>::p4 = 0;


void inst_A()
{
  A<int> a;
  a.f1();
  a.f2();
}


template <class S, class T>
struct B {
  B* f1();
  B<S,T> *f2();
  B<T,S> *f3();
  B<T,S> *f4();
  
  static B* p5;
  static B<S,T> *p6;
  static B<T,S> *p7;
  static B<T,S> *p8;
};

template <class S1, class T1>
B<S1,T1>* B<S1,T1>::f1()
{
  return this;
}

template <class S2, class T2>
B<S2,T2>* B<S2,T2>::f2()
{
  return this;
}

template <class S3, class T3>
B<T3,S3>* B<S3,T3>::f3()
{
  return new B<T3,S3>;
}

//ERROR(1): template <class S4, class T4>
//ERROR(1): B<S4,T4>* B<S4,T4>::f4()
//ERROR(1): {
//ERROR(1):   return new B<T4,S4>;
//ERROR(1): }

template <class S5, class T5>
B<S5,T5>* B<S5,T5>::p5 = 0;

template <class S6, class T6>
B<S6,T6>* B<S6,T6>::p6 = 0;

template <class S7, class T7>
B<T7,S7>* B<S7,T7>::p7 = 0;

//ERROR(2): template <class S8, class T8>
//ERROR(2): B<S8,T8>* B<S8,T8>::p8 = 0;



void inst_B()
{
  B<int,float> b;
  b.f1();
  b.f2();
  b.f3();
}



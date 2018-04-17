// #include <iostream>
// using namespace std;

template<class T, class U, int I> struct X
{ void f() { /* cout << "Primary template" << endl; */ } };

template<class T, int I> struct X<T, T*, I>
{ void f() { /* cout << "Partial specialization 1" << endl; */
  } };

#if 0
template<class T, class U, int I> struct X<T*, U, I>
  { void f() { cout << "Partial specialization 2" << endl;
  } };

template<class T> struct X<int, T*, 10>
  { void f() { cout << "Partial specialization 3" << endl;
  } };

template<class T, class U, int I> struct X<T, U*, I>
  { void f() { cout << "Partial specialization 4" << endl;
  } };
#endif

#if 0
// Added classes...
template<int I, int J> class X { };

// Invalid partial specialization
template<int I> class X <I * 4, I + 3> { };

// Valid partial specialization
template <int I> class X <I, I> { }; 
#endif

int main() {
   X<int, int, 10> a;
   X<int, int*, 5> b;
   a.f(); 
   b.f();

#if 0
   X<int*, float, 10> c;
   X<int, char*, 10> d;
   X<float, int*, 10> e;
//   X<int, int*, 10> f;
   a.f(); b.f(); c.f(); d.f(); e.f();
#endif

}


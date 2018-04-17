// t0532.cc
// using template arguments to obtain scopes for arg-dep lookup


namespace N1 {
  template <class T>
  struct B {};

  namespace M {
    struct A {};

    int f(B<A>*);       // the function to find
  }

  int f(int,int);

  int foo(B<M::A> *b)
  {
    return f(b);
  }
}


namespace N2 {
  namespace M {
    struct A {
      friend int f(A*);
    };
  }

  int foo(M::A *a)
  {
    return f(a);
  }
}



namespace N3 {
  namespace M {
    struct A {
      // neither GCC nor ICC find this ...
      static int f(A*);
    };
  }

  int foo(M::A *a)
  {
    //ERROR(1): return f(a);
  }
}



namespace N4 {
  namespace M {
    struct A {
    };
    
    // GCC rejects, ICC accepts; I guess I will reject
    //ERROR(2): int f;
  }       
  
  int f(M::A *);

  int foo(M::A *a)
  {
    return f(a);
  }
}




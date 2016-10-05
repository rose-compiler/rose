// t0511.cc
// testing mtype module

// 2005-08-02: The tests in this file alone are able to drive mtype to
// 100% statement coverage except for the MF_POLYMORPHIC stuff.  The
// tests in this file were mostly constructed specifically (and only)
// to obtain that coverage level.


// copied from mflags.h
enum MatchFlags {
  // complete equality; this is the default; note that we are
  // checking for *type* equality, rather than equality of the
  // syntax used to denote it, so we do *not* compare (e.g.)
  // function parameter names or typedef usage
  MF_NONE            = 0x0000,

  // ----- basic behaviors -----
  // when comparing function types, do not check whether the
  // return types are equal
  MF_IGNORE_RETURN   = 0x0001,

  // when comparing function types, if one is a nonstatic member
  // function and the other is not, then do not (necessarily)
  // call them unequal
  MF_STAT_EQ_NONSTAT = 0x0002,    // static can equal nonstatic

  // when comparing function types, only compare the explicit
  // (non-receiver) parameters; this does *not* imply
  // MF_STAT_EQ_NONSTAT
  MF_IGNORE_IMPLICIT = 0x0004,

  // In the C++ type system, cv qualifiers on parameters are not part
  // of the type [8.3.5p3], so by default mtype ignores them.  If you
  // set this flag, then they will *not* be ignored.  It is provided
  // only for completeness; Elsa does not use it.
  MF_RESPECT_PARAM_CV= 0x0008,

  // ignore the topmost cv qualification of the two types compared
  MF_IGNORE_TOP_CV   = 0x0010,

  // when comparing function types, compare the exception specs;
  // by default such specifications are not compared because the
  // exception spec is not part of the "type" [8.3.5p4]
  MF_COMPARE_EXN_SPEC= 0x0020,
  
  // allow the cv qualifications to differ up to the first type
  // constructor that is not a pointer or pointer-to-member; this
  // is cppstd 4.4 para 4 "similar"; implies MF_IGNORE_TOP_CV
  MF_SIMILAR         = 0x0040,

  // when the second type in the comparison is polymorphic (for
  // built-in operators; this is not for templates), and the first
  // type is in the set of types described, say they're equal;
  // note that polymorhism-enabled comparisons therefore are not
  // symmetric in their arguments
  MF_POLYMORPHIC     = 0x0080,

  // for use by the matchtype module: this flag means we are trying
  // to deduce function template arguments, so the variations
  // allowed in 14.8.2.1 are in effect (for the moment I don't know
  // what propagation properties this flag should have)
  MF_DEDUCTION       = 0x0100,

  // this is another flag for MatchTypes, and it means that template
  // parameters should be regarded as unification variables only if
  // they are *not* associated with a specific template
  MF_UNASSOC_TPARAMS = 0x0200,

  // ignore the cv qualification on the array element, if the
  // types being compared are arrays
  MF_IGNORE_ELT_CV   = 0x0400,

  // enable matching/substitution with template parameters
  MF_MATCH           = 0x0800,

  // do not allow new bindings to be created; but existing bindings
  // can continue to be used
  MF_NO_NEW_BINDINGS = 0x1000,

  // when combined with MF_MATCH, it means we can bind variables in
  // the pattern only to other variables in the "concrete" type, and
  // that the binding function must be injective (no two pattern
  // variables can be bound to the same concrete variable); this
  // is used to compare two templatized signatures for equivalence
  MF_ISOMORPHIC      = 0x2000,

  // ----- combined behaviors -----
  // all flags set to 1
  MF_ALL             = 0x3FFF,

  // number of 1 bits in MF_ALL
  MF_NUM_FLAGS       = 14,

  // signature equivalence for the purpose of detecting whether
  // two declarations refer to the same entity (as opposed to two
  // overloaded entities)
  MF_SIGNATURE       = (
    MF_IGNORE_RETURN |       // can't overload on return type
    MF_STAT_EQ_NONSTAT       // can't overload on static vs. nonstatic
  ),

  // ----- combinations used by the mtype implementation -----
  // this is the set of flags that allow CV variance within the
  // current type constructor
  MF_OK_DIFFERENT_CV = (MF_IGNORE_TOP_CV | MF_SIMILAR),

  // this is the set of flags that automatically propagate down
  // the type tree equality checker; others are suppressed once
  // the first type constructor looks at them
  MF_PROP = (
    MF_RESPECT_PARAM_CV |
    MF_POLYMORPHIC      |
    MF_UNASSOC_TPARAMS  |
    MF_MATCH            |
    MF_NO_NEW_BINDINGS  |
    MF_ISOMORPHIC       
    
    // Note: MF_COMPARE_EXN_SPEC is *not* propagated.  It is used only
    // when the compared types are FunctionTypes, to compare those
    // toplevel exn specs, but any FunctionTypes appearing underneath
    // are compared just as types (not objects), and hence their exn
    // specs are irrelevant.
  ),

  // these flags are propagated below ptr and ptr-to-member
  MF_PTR_PROP = (
    MF_PROP            |
    MF_SIMILAR         |
    MF_DEDUCTION
  )
};


struct B {
  int x;
  
  static int f_stat();
  int f_nonstat();
  
  typedef int INT;
  typedef int const INTC;
};

struct B2 {
  int x;
};


template <class T>
struct C {
  T t;
};


template <class U, class V>
struct Pair {
  U u;
  V v;
};

template <class U, class V>
struct Pair2 {
  U u;
  V v;
};


template <int m>
struct Num {};
             

int const global_const1 = 1;
int const global_const2 = 2;


int f_nonvararg(int);
int f_vararg(int, ...);

int f_throws_int(int) throw(int);
int f_throws_float(int) throw(float);



int global_n;
int global_m;

template <class T, int &intref>
struct TakesIntRef {};

template <class T, int *intptr>
struct TakesIntPtr {};
                   

template <class T, int B::*ptm>
struct TakesPTM {};


struct D {
  template <class T>
  struct E {
    typedef int INT;
  };

  // partial spec
  template <class T>
  struct E<T*> {};

  // explicit full spec
  template <>
  struct E<float*> {};
  
  struct F {
    typedef int INT;
  };
  
  typedef int INT;
};


template <class S, class T, int n, int n2>
struct A {
  void f()
  {
    // very simple
    __test_mtype((int*)0, (T*)0, MF_MATCH,
                 "T", (int)0);
    __test_mtype((int*)0, (float*)0, MF_MATCH, false);


    // CVAtomicType
    __test_mtype((int const volatile)0, (T const)0, MF_MATCH,
                 "T", (int volatile)0);

    // PointerType
    __test_mtype((int const * const)0, (T * const)0, MF_MATCH,
                 "T", (int const)0);
    __test_mtype((int const * const)0, (T *)0, MF_MATCH,
                 false);

    // ReferenceType
    __test_mtype((int &)0, (T &)0, MF_MATCH,
                 "T", (int)0);

    // FunctionType
    __test_mtype((int (*)())0, (T (*)())0, MF_MATCH,
                 "T", (int)0);

    // ArrayType
    __test_mtype((int (*)[3])0,
                 (T   (*)[3])0, MF_MATCH,
                 "T", (int)0);
    __test_mtype((int (*)[])0,
                 (T   (*)[])0, MF_MATCH,
                 "T", (int)0);
    __test_mtype((int (*)[3])0,
                 (T   (*)[4])0, MF_MATCH,
                 false);


    // testing binding of variables directly to atomics
    __test_mtype((int B::*)0, (int T::*)0, MF_MATCH,
                 "T", (B)0);

    __test_mtype((int (*)(B const *, int B::*))0,
                 (int (*)(T       *, int T::*))0, MF_MATCH,
                 "T", (B const)0);

    __test_mtype((int (*)(B const *, int B::*, B const *))0,
                 (int (*)(T       *, int T::*, T       *))0, MF_MATCH,
                 "T", (B const)0);

    __test_mtype((int (*)(int B::*, B const *))0,
                 (int (*)(int T::*, T       *))0, MF_MATCH,
                 "T", (B const)0);

    __test_mtype((int (*)(int B::*, B const *, int B::*))0,
                 (int (*)(int T::*, T       *, int T::*))0, MF_MATCH,
                 "T", (B const)0);

    __test_mtype((int (*)(int B::*, B const *, B const volatile *))0,
                 (int (*)(int T::*, T       *, T       volatile *))0, MF_MATCH,
                 "T", (B const)0);

    __test_mtype((int (*)(int B::*, B const          *))0,
                 (int (*)(int T::*, T       volatile *))0, MF_MATCH,
                 false);

    __test_mtype((int (*)(int B::*, B const *, B const          *))0,
                 (int (*)(int T::*, T       *, T       volatile *))0, MF_MATCH,
                 false);

    // second binding is not to an atomic
    __test_mtype((int (*)(int B::*, B const * *))0,
                 (int (*)(int T::*, T         *))0, MF_MATCH,
                 false);

    // wrong atomic
    __test_mtype((int (*)(int B::*, B2 const *))0,
                 (int (*)(int T::*, T        *))0, MF_MATCH,
                 false);

    // first binding is not an atomic
    __test_mtype((int (*)(B const * *, int B::*))0,
                 (int (*)(T         *, int T::*))0, MF_MATCH,
                 false);

    // wrong atomic
    __test_mtype((int (*)(B2 const *, int B::*))0,
                 (int (*)(T        *, int T::*))0, MF_MATCH,
                 false);

    // wrong AtomicType
    __test_mtype((int (*)(int B2::*, int B::*))0,
                 (int (*)(int T ::*, int T::*))0, MF_MATCH,
                 false);

    // not a type (causes an early error diagnosis)
    //__test_mtype((int (*)(Num<3>, int B::*))0,
    //             (int (*)(Num<n>, int n::*))0, MF_MATCH,
    //             false);


    // multiple occurrences of variables in patterns
    __test_mtype((int (*)(int,int))0,
                 (int (*)(T,T))0, MF_MATCH,
                 "T", (int)0);

    __test_mtype((int (*)(int,float))0,
                 (int (*)(T,T))0, MF_MATCH,
                 false);


    // match instantiation with PseudoInstantiation
    __test_mtype((Pair<int,int>*)0,
                 (Pair<T,T>*)0, MF_MATCH,
                 "T", (int)0);
    __test_mtype((Pair<int,int>*)0,
                 (Pair2<T,T>*)0, MF_MATCH,
                 false);


    // match template param with itself
    __test_mtype((T*)0,
                 (T*)0, MF_NONE);
    __test_mtype((T*)0,
                 (S*)0, MF_NONE, false);

    // Q: should this yield a binding?  for now it does...
    __test_mtype((T*)0,
                 (T*)0, MF_MATCH,
                 "T", (T)0);

    // PseudoInstantiation
    __test_mtype((Pair<T,int>*)0,
                 (Pair<T,int>*)0, MF_NONE);
    __test_mtype((Pair<T,int>*)0,
                 (Pair<T,int const>*)0, MF_NONE, false);
    __test_mtype((Pair<T,int>*)0,
                 (Pair2<T,int>*)0, MF_NONE, false);

    // DependentQType
    __test_mtype((typename T::Foo*)0,
                 (typename T::Foo*)0, MF_NONE);
    __test_mtype((typename T::Foo::Bar*)0,
                 (typename T::Foo::Bar*)0, MF_NONE);
    __test_mtype((typename T::Foo::template Baz<3>*)0,
                 (typename T::Foo::template Baz<3>*)0, MF_NONE);
    __test_mtype((typename T::Foo::template Baz<3>*)0,
                 (typename T::Foo::template Baz<4>*)0, MF_NONE, false);


    // match with an integer
    __test_mtype((Num<3>*)0,
                 (Num<n>*)0, MF_MATCH,
                 "n", 3);

    // ask the infrastructure for an unbound value
    //ERROR(1): __test_mtype((Num<3>*)0,
    //ERROR(1):              (Num<n>*)0, MF_MATCH,
    //ERROR(1):              "nn", 3);

    // attempt to compare different kinds of atomics
    __test_mtype((B*)0, (int*)0, MF_NONE, false);
    
    // Mix different binding kinds together.
    //
    // These are currently triggering match failures in mtype.cc, as
    // intended (to improve coverage), but it is possible that a
    // future change to the tcheck code might diagnose them as syntax
    // errors.  If that happens, these can just be commented out I
    // suppose.  
    __test_mtype((Pair<int, Num<3> >*)0,
                 (Pair<T,   Num<T> >*)0, MF_MATCH, false);

    __test_mtype((Pair<Num<3>, int>*)0,
                 (Pair<Num<T>, T  >*)0, MF_MATCH, false);

    __test_mtype((Pair<int, Num<3> >*)0,
                 (Pair<n,   Num<n> >*)0, MF_MATCH, false);

    __test_mtype((Pair<Num<3>, int>*)0,
                 (Pair<Num<n>, n  >*)0, MF_MATCH, false);

    // nontype bound to 3 then 4
    __test_mtype((Pair<Num<3>, Num<4> >*)0,
                 (Pair<Num<n>, Num<n> >*)0, MF_MATCH, false);

    // cover a specific line in mtype.cc ...
    __test_mtype((Num<3>*)0,
                 (Num<n>*)0, MF_MATCH|MF_NO_NEW_BINDINGS, false);
    __test_mtype((int*)0,
                 (T*)0, MF_MATCH|MF_NO_NEW_BINDINGS, false);
    __test_mtype((int B::*)0,
                 (int T::*)0, MF_MATCH|MF_NO_NEW_BINDINGS, false);

    // DQTs with mismatching leading atomics
    __test_mtype((typename C<T>::Foo*)0,
                 (typename Pair<T,T>::Foo*)0, MF_NONE, false);
                 
    // and mismatching PQName kinds
    __test_mtype((typename C<T>::template Foo<3>*)0,
                 (typename C<T>::Foo*)0, MF_NONE, false);

    // differing PQ_qualifier names
    __test_mtype((typename C<T>::Foo::Baz*)0,
                 (typename C<T>::Bar::Baz*)0, MF_NONE, false);

    // differing template arguments to PQ_qualifier
    __test_mtype((typename C<T>::template Foo<1>::Baz*)0,
                 (typename C<T>::template Foo<2>::Baz*)0, MF_NONE, false);


    // different Type kinds
    __test_mtype((int)0,
                 (int*)0, MF_NONE, false);
                 
    // cv-flags in the pattern aren't present in concrete
    __test_mtype((int const)0,
                 (T volatile)0, MF_MATCH, false);
    __test_mtype((int const)0,
                 (T const)0, MF_MATCH,
                 "T", (int)0);
                 
    // FunctionType with differing return type
    __test_mtype((int (*)())0,
                 (float (*)())0, MF_NONE, false);
                 
    // static vs. non-static
    __test_mtype(B::f_stat,
                 B::f_nonstat, MF_NONE, false);
    __test_mtype(B::f_stat,
                 B::f_nonstat, MF_STAT_EQ_NONSTAT);
    __test_mtype(B::f_nonstat,
                 B::f_stat, MF_STAT_EQ_NONSTAT);

    // vararg vs. non-vararg
    __test_mtype(f_vararg, f_nonvararg, MF_NONE, false);
    
    // throw vs. non-throw
    __test_mtype(f_nonvararg, f_throws_int, MF_COMPARE_EXN_SPEC, false);

    // differing exception specs
    __test_mtype(f_throws_float, f_throws_int, MF_COMPARE_EXN_SPEC, false);
    __test_mtype(f_throws_int, f_throws_int, MF_COMPARE_EXN_SPEC);
    
    // ArrayTypes and MF_IGNORE_ELT_CV
    //
    // I am abusing the cast syntax here because MF_IGNORE_ELT_CV does
    // not propagate below type constructors... at some point Elsa
    // might be modified to reject these invalid casts altogether, in
    // which case these tests can just be commented out.
    __test_mtype((int const [2])0,
                 (int       [2])0, MF_NONE, false);
    __test_mtype((int const [2])0,
                 (int       [2])0, MF_IGNORE_ELT_CV);
    __test_mtype((int const [2][3])0,
                 (int       [2][3])0, MF_IGNORE_ELT_CV);
                 
    // expression comparison
    __test_mtype((Num<n+3>*)0,
                 (Num<n+3>*)0, MF_NONE);

    __test_mtype((Num<(n+3)>*)0,
                 (Num< n+3 >*)0, MF_NONE);

    __test_mtype((Num< n+3 >*)0,
                 (Num<(n+3)>*)0, MF_NONE);

    __test_mtype((Num<(n+3)>*)0,
                 (Num<(n+3)>*)0, MF_NONE);

    __test_mtype((Num<-n>*)0,
                 (Num<-n>*)0, MF_NONE);

    __test_mtype((Num<n+3>*)0,
                 (Num<-n>*)0, MF_NONE, false);

    __test_mtype((Num<n+(int)true>*)0,
                 (Num<n+(int)true>*)0, MF_NONE);

    __test_mtype((Num<n+static_cast<int>(true)>*)0,
                 (Num<n+static_cast<int>(true)>*)0, MF_NONE);

    __test_mtype((Num<n+'a'>*)0,
                 (Num<n+'a'>*)0, MF_NONE);

    __test_mtype((Num<n >*)0,
                 (Num<n2>*)0, MF_NONE, false);

    __test_mtype((Num<n+global_const1>*)0,
                 (Num<n+global_const2>*)0, MF_NONE, false);

    __test_mtype((Num<n+sizeof(int)>*)0,
                 (Num<n+sizeof(int)>*)0, MF_NONE);

    __test_mtype((Num<n+sizeof(n)>*)0,
                 (Num<n+sizeof(n)>*)0, MF_NONE);

    __test_mtype((Num< n? 1 : 2 >*)0,
                 (Num< n? 1 : 2 >*)0, MF_NONE);
    __test_mtype((Num< n? 1 : 2 >*)0,
                 (Num< n? 1 : 3 >*)0, MF_NONE, false);
                 
    __test_mtype((TakesIntRef<T, global_n>*)0,
                 (TakesIntRef<T, global_n>*)0, MF_NONE);

    __test_mtype((TakesIntRef<T, global_n>*)0,
                 (TakesIntRef<T, global_m>*)0, MF_NONE, false);

    __test_mtype((TakesIntPtr<T, &global_n>*)0,
                 (TakesIntPtr<T, &global_n>*)0, MF_NONE);

    __test_mtype((TakesPTM<T, &B::x>*)0,
                 (TakesPTM<T, &B::x>*)0, MF_NONE);


    // testing resolution of DQTs (see also in/t0487.cc)
    __test_mtype((void (*)(B *, int            , int))0,
                 (void (*)(T *, typename T::INT, int))0, MF_MATCH,
                 "T", (B)0);

    // bind directly to AtomicType rather than CVAtomicType
    __test_mtype((void (*)(int B::*, int            , int))0,
                 (void (*)(int T::*, typename T::INT, int))0, MF_MATCH,
                 "T", (B)0);

    // failure because the initial binding doesn't work
    __test_mtype((void (*)(B2 *, int            , int))0,
                 (void (*)(T  *, typename T::INT, int))0, MF_MATCH,
                 false);

    // final name component is PQ_template
    __test_mtype((void (*)(D *, D::E<int>                  , int))0,
                 (void (*)(T *, typename T::template E<int>, int))0, MF_MATCH,
                 "T", (D)0);

    // mismatching variant
    __test_mtype((void (*)(D *, D::E<int>                    , int))0,
                 (void (*)(T *, typename T::template E<float>, int))0, MF_MATCH,
                 false);

    // PQ_template, but the name doesn't name a template
    __test_mtype((void (*)(D *, D::E<int>                    , int))0,
                 (void (*)(T *, typename T::template INT<int>, int))0, MF_MATCH,
                 false);

    // exercise the partial specialization
    __test_mtype((void (*)(D *, D::E<int*>                  , int))0,
                 (void (*)(T *, typename T::template E<int*>, int))0, MF_MATCH,
                 "T", (D)0);

    // exercise the full specialization
    __test_mtype((void (*)(D *, D::E<float*>                  , int))0,
                 (void (*)(T *, typename T::template E<float*>, int))0, MF_MATCH,
                 "T", (D)0);

    // PQ_qualifier without template args
    __test_mtype((void (*)(D *, int               , int))0,
                 (void (*)(T *, typename T::F::INT, int))0, MF_MATCH,
                 "T", (D)0);

    // bad PQ_qualifier
    __test_mtype((void (*)(D *, int                      , int))0,
                 (void (*)(T *, typename T::NONEXIST::INT, int))0, MF_MATCH,
                 false);

    // does not name a CompoundType
    __test_mtype((void (*)(B *, int                 , int))0,
                 (void (*)(T *, typename T::INT::INT, int))0, MF_MATCH,
                 false);

    // PQ_qualifier *with* template args
    //
    // this does not work, see notes near the xunimp it triggers
    //__test_mtype((void (*)(D *, int                             , int))0,
    //             (void (*)(T *, typename T::template E<int>::INT, int))0, MF_MATCH,
    //             "T", (D)0);
    
    // from t0487b.cc
    __test_mtype((void (*)(B *, int const *       , int))0,
                 (void (*)(T *, typename T::INTC *, int))0, MF_MATCH,
                 "T", (B)0);

    __test_mtype((void (*)(B *, int       *       , int))0,
                 (void (*)(T *, typename T::INTC *, int))0, MF_MATCH,
                 false);

    __test_mtype((void (*)(B *, int const *       , int))0,
                 (void (*)(T *, typename T::INT  *, int))0, MF_MATCH,
                 false);
                 
    // from in/t0315.cc
    __test_mtype((int     *)0,
                 (T const *)0, MF_MATCH|MF_DEDUCTION,
                 "T", (int)0);
                 
    // from in/t0462.cc; the 'const' is *not* deduced for T
    __test_mtype((int const)0,
                 (T        )0, MF_MATCH|MF_DEDUCTION,
                 "T", (int)0);
                 
    // from in/t0486.cc
    __test_mtype((typename T::INT (*)(typename T::INT))0,
                 (S               (*)(S              ))0, MF_MATCH,
                 "S", (typename T::INT)0);
  }
};

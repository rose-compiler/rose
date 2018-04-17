///////////////////////////////////////////////////////////////////////////
//
// INCLUDE FILES
//
///////////////////////////////////////////////////////////////////////////

// For basic definitions.
// i n c l u d e "Config.h"
#if defined(USE_EXPRESSION_TEMPLATES)

// The COPY option is required since it results in
// more readily optimized internal loops and results in
// half the compile time (70 minutes for single statement vs. 35 minutes)
// It might be that a pointer (using the restrict keyword) would be
// best since it would avoid the internal copy (at runtime) we will
// try this out at some point.
#define USE_EXPRESSION_TEMPLATE_COPY_OPTION

// The use of default member functions within classes causes them
// to be inlined (according to the C++ standard).  It is believed
// that this is a principle cause of the excessive compile times
// for expression templates.  So to check this we have implemented
// the member functions which would otherwise be built inline by the 
// compiler.  We hope that this option within the expression template
// option within A++/P++ will result in a significant improvement in the 
// compile times.
// define DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED

#if defined(__KCC) || defined(SGI)
#define HAS_MEMBER_TEMPLATES
#define HAS_FORTRAN
#define HANDLES_TEMPLATES_IN_CCFILES
#else
// The Sun C++ compiler does not know about the static_cast keyword
// define static_cast
#define BARE_BONES_EXPRESSION_TEMPLATES
#endif

// Test implementation of expression templates without the static_cast keyword (with KCC compiler)
// The results of the test are that the static_cast is required to work with the KCC compiler
// at least this is my experience so far.  A better implementation might be able to work around the
// problem.  So far the Sun C++ compiler will compile A++ and some examples using expressions
// if this is turned ON  (i.e. defined).
// define BARE_BONES_EXPRESSION_TEMPLATES

// I can't get the non member template option to work -- I think this just requires more work
// I susspect there is a problem in my implementation since it does not work with the KCC compiler 
// (the sun C++ compiler is the real target of this part of the work and that is VERY close to working)
// undef HAS_MEMBER_TEMPLATES

#if !defined(__GNUC__) && !defined(typename)
#define typename
#endif

/* Real numbers */
// Type 'Real' should correspond to a Fortran real.
typedef float Real;

// Type 'Double' should correspond to a Fortran doubleprecision.
typedef double Double;

/* Integers (at least four bytes long) */
// Type 'Int' should correspond to a Fortran integer.
typedef int Int;

/* bools */
#if 0
#if defined(SGI)
typedef int bool;
#define true 1
#define false 0
#endif
#endif

///////////////////////////////////////////////////////////////////////////
//
// MACRO AND INLINE DEFINITIONS
//
///////////////////////////////////////////////////////////////////////////

/* Null pointers */

#define Null 0

/* Mathematical operations */

inline Real pow2(Real x)
{
  return x * x;
}
inline Double pow2(Double x)
{
  return x * x;
}
inline Int pow2(Int x)
{
  return x * x;
}

inline Real pow3(Real x)
{
  Real x2 = x * x;
  return x * x2;
}
inline Double pow3(Double x)
{
  Double x2 = x * x;
  return x * x2;
}
inline Int pow3(Int x)
{
  Int x2 = x * x;
  return x * x2;
}

inline Real pow4(Real x)
{
  Real x2 = x * x;
  return pow2(x2);
}
inline Double pow4(Double x)
{
  Double x2 = x * x;
  return pow2(x2);
}
inline Int pow4(Int x)
{
  Int x2 = x * x;
  return pow2(x2);
}

#if !defined(_WIN32)

inline Real abs(Real x)
{
  return fabs(x);
}

// inline Real fabs(Real x)
// {
//   return fabs(x);
// }

#endif

#if !defined(HP700) && !defined(_WIN32)

inline Double abs(Double x)
{
  return fabs(x);
}

// inline Double fabs(Double x)
// {
//   return fabs(x);
// }

#endif

inline float max(float x, float y)
{
  return x > y ? x : y;
}

inline double max(double x, double y)
{
  return x > y ? x : y;
}

inline int max(int x, int y)
{
  return x > y ? x : y;
}

inline float min(float x, float y)
{
  return x < y ? x : y;
}

inline double min(double x, double y)
{
  return x < y ? x : y;
}

inline int min(int x, int y)
{
  return x < y ? x : y;
}

/* Null string pointer */

const char * const NullCStr = "";

// For operations.
///////////////////////////////////////////////////////////////////////////
//
// PROMOTION TRAIT CLASSES
//
///////////////////////////////////////////////////////////////////////////

template <class A1, class A2>
struct Promote {
};

struct Promote<Int, Int> {
  typedef Int RetT;
};

struct Promote<Real, Real> {
  typedef Real RetT;
};

struct Promote<Double, Double> {
  typedef Double RetT;
};

struct Promote<Int,Real> {
  typedef Real RetT;
};

struct Promote<Int,Double> {
  typedef Double RetT;
};

struct Promote<Real,Int> {
  typedef Real RetT;
};

struct Promote<Real,Double> {
  typedef Double RetT;
};

struct Promote<Double,Int> {
  typedef Double RetT;
};

struct Promote<Double,Real> {
  typedef Double RetT;
};


///////////////////////////////////////////////////////////////////////////
//
// BINARY OPERATION AND FUNCTION CLASSES
//
///////////////////////////////////////////////////////////////////////////
//
// A possible optimization would be to add the restrict keywork to the
// apply functions.  Usage is as in:
// static inline RetT apply(const T1 &a, const T2 &b) restrict 
// Steve Karmisen showed me this cool trick (I have not tried it yet)
// Well now that I have tried it -- I can't make it work.
// I suspect that it does not work with a static member function
//

#if !defined(BARE_BONES_EXPRESSION_TEMPLATES)
#define DefineInBinOp(ap,op)                                             \
template <class T1, class T2>                                            \
struct ap {                                                              \
  typedef typename Promote<T1, T2>::RetT RetT;                           \
  static inline RetT apply(const T1 &a, const T2 &b)                     \
    { return a op b; }                                                   \
};
#else
#define DefineInBinOp(ap,op)                                             \
template <class T1, class T2>                                            \
struct ap {                                                              \
  typedef T1 RetT;                                                       \
  static inline RetT apply(const T1 &a, const T2 &b)                     \
    { return a op b; }                                                   \
};
#endif

#define DefineInBinFn(ap,fn)                                             \
template <class T1, class T2>                                            \
struct ap {                                                              \
  typedef typename Promote<T1, T2>::RetT RetT;                           \
  static inline RetT apply(const T1 &a, const T2 &b)                     \
    { return fn(a, b); }                                                 \
};

#define DefineInBoolOp(ap,op)                                            \
template <class T1, class T2>                                            \
struct ap {                                                              \
  typedef int RetT;                                                      \
  static inline RetT apply(const T1 &a, const T2 &b)                     \
    { return a op b; }                                                   \
};

// Arithmetic operators:

DefineInBinOp(Plus, +)
DefineInBinOp(Minus, -)
DefineInBinOp(Times, *)
DefineInBinOp(Divide, /)

// 2 argument functions:

DefineInBinFn(Atan2Funct, atan2)
DefineInBinFn(PowFunct, pow)
DefineInBinFn(FmodFunct, fmod)

// bool operators:
// Return type in these operators was changed to int
// to be consistant with A++/P++ (at least for now)

DefineInBoolOp(Equal, ==)
DefineInBoolOp(NotEqual, !=)
DefineInBoolOp(GrThan, >)
DefineInBoolOp(LessThan, <)
DefineInBoolOp(GrThanEq, >=)
DefineInBoolOp(LessThanEq, <=)
DefineInBoolOp(LogicalOr, ||)
DefineInBoolOp(LogicalAnd,&&)


///////////////////////////////////////////////////////////////////////////
//
// UNARY OPERATION AND FUNCTION CLASSES
//
///////////////////////////////////////////////////////////////////////////

#define DefineInUnOp(ap,op)                                              \
template <class T>                                                       \
struct ap {                                                              \
  typedef T RetT;                                                        \
  static inline RetT apply(const T &a) { return op(a); }                 \
};

// Unary operators

DefineInUnOp(UnaryPlus, +)
DefineInUnOp(UnaryMinus, -)
DefineInUnOp(UnaryNot, !)

// Unary functions

DefineInUnOp(ExpFunct, exp)
DefineInUnOp(LogFunct, log)
DefineInUnOp(Log10Funct, log10)
DefineInUnOp(SinFunct, sin)
DefineInUnOp(CosFunct, cos)
DefineInUnOp(TanFunct, tan)
DefineInUnOp(AsinFunct, asin)
DefineInUnOp(AcosFunct, acos)
DefineInUnOp(AtanFunct, atan)
DefineInUnOp(SinhFunct, sinh)
DefineInUnOp(CoshFunct, cosh)
DefineInUnOp(TanhFunct, tanh)
DefineInUnOp(Pow2Funct, pow2)
DefineInUnOp(Pow3Funct, pow3)
DefineInUnOp(Pow4Funct, pow4)
DefineInUnOp(SqrtFunct, sqrt)
DefineInUnOp(CeilFunct, ceil)
DefineInUnOp(FloorFunct, floor)
DefineInUnOp(CexpFunct, cexp)
DefineInUnOp(CsqrtFunct, cexp)
DefineInUnOp(RealXFunct, real)
DefineInUnOp(ImagFunct, imag)
DefineInUnOp(AbsFunct, abs)
// Added new function
DefineInUnOp(FabsFunct, fabs)
DefineInUnOp(ConjgFunct, conjg)


///////////////////////////////////////////////////////////////////////////
//
// ASSIGNMENT OPERATORS
//
///////////////////////////////////////////////////////////////////////////

#define DefineInAsOp(ap,op)                                              \
template <class T1, class T2>                                            \
struct ap {                                                              \
  static inline void apply(T1 &a, const T2 &b)                           \
    { a op b; }                                                          \
};

// Assignment operators

DefineInAsOp(AssignEqu, =)
DefineInAsOp(AssignPlus, +=)
DefineInAsOp(AssignMinus, -=)
DefineInAsOp(AssignTimes, *=)
DefineInAsOp(AssignDivide, /=)

// Specializations used for bool operations (where arithmetic is not d efined).

struct AssignPlus<bool,bool> {
  static inline void apply(bool &, const bool &) { }
};

struct AssignMinus<bool,bool> {
  static inline void apply(bool &, const bool &) { }
};

struct AssignTimes<bool,bool> {
  static inline void apply(bool &, const bool &) { }
};

struct AssignDivide<bool,bool> {
  static inline void apply(bool &, const bool &) { }
};

///////////////////////////////////////////////////////////////////////////
//
// CLASS NAME
//    InAssign
//
// DESCRIPTION
//    Hack to work around a non-implementation of member templates.
//
// NOTE
//    If member templates are not available, the ultimate type of the
//    expression on the RHS of an assignment operator (subject to the
//    usual promotion rules) MUST be the same as the element type of
//    the array or array section.
//
///////////////////////////////////////////////////////////////////////////

#if !defined(HAS_MEMBER_TEMPLATES)

template<class T>
class InAssign {

public:

  virtual void assignEqu(T *data, const Array_Domain_Type *s) const = 0;
  virtual void assignPlus(T *data, const Array_Domain_Type *s) const = 0;
  virtual void assignMinus(T *data, const Array_Domain_Type *s) const = 0;
  virtual void assignTimes(T *data, const Array_Domain_Type *s) const = 0;
  virtual void assignDivide(T *data, const Array_Domain_Type *s) const = 0;
  
  virtual const Array_Domain_Type &getShape() const = 0;
};

#define INHERIT_FROM_INASSIGN : public InAssign<Ret>

#define DefineInAsOps(ap,op)                                             \
virtual void ap(Ret *data, const Array_Domain_Type *s) const         \
{                                                                        \
    Int i, n = s->size();                                                \
    if (s->continuousData())                                             \
      for (i = 0; i < n; i++)                                            \
        op<Ret,Ret>::apply(data[s->offset0(i)], (*this)[i]);             \
    else                                                                 \
      for (i = 0; i < n; i++)                                            \
        op<Ret,Ret>::apply(data[s->expression_index(i)], (*this)[i]);    \
}

#else

#define INHERIT_FROM_INASSIGN

#endif


///////////////////////////////////////////////////////////////////////////
//
// CLASS NAME
//    Indexable
//
// DESCRIPTION
//    An envelope class made for containing generic expressions. 
//
///////////////////////////////////////////////////////////////////////////

// Forward reference
class BaseArray;

template<class Ret, class A>
class Indexable INHERIT_FROM_INASSIGN {

public:
#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~Indexable();
   Indexable();
   Indexable (const Indexable & X);
   Indexable & operator= (const Indexable & X);
#endif
  
#if !defined(BARE_BONES_EXPRESSION_TEMPLATES)
  const A &data() const { return *static_cast<const A*>(this); }
#else
  const A &data() const { return *((const A*) (this)); }
#endif

#if 0
  Ret operator[](Int i) const 
  { 
    printf ("Inside of Indexable::operator[](i=%d) \n",i);
#if !defined(BARE_BONES_EXPRESSION_TEMPLATES)
    return static_cast<const A*>(this)->operator[](i);
#else
    return this->operator[](i); 
#endif
  }
#endif
  
  bool usesIndirectAddressing() const 
  {
 // printf ("Inside of Indexable::usesIndirectAddressing() \n");
    return static_cast<const A*>(this)->usesIndirectAddressing(); 
  }
  
  int internalNumberOfDimensions () const
     {
    // printf ("Inside of Indexable::internalNumberOfDimensions() \n");
       return static_cast<const A*>(this)->internalNumberOfDimensions(); 
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       if (APP_DEBUG > 2)
            printf ("Inside of Indexable::containsArrayID(int lhs_id = %d) \n",lhs_id);
       return static_cast<const A*>(this)->containsArrayID(lhs_id);
     }

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
  { static_cast<const A*>(this)->buildListOfArrayOperands(arrayList); }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
  { static_cast<const A*>(this)->buildListOfArrayOperands(arrayList); }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
  { static_cast<const A*>(this)->buildListOfArrayOperands(arrayList); }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
  { static_cast<const A*>(this)->buildListOfArrayOperands(arrayList); }

  void setupIndirectAddressingSupport() const
  { static_cast<const A*>(this)->setupIndirectAddressingSupport(); }

  Ret expand(int i) const 
  { return static_cast<const A*>(this)->expand(i); }
  Ret indirect_expand_1D(int i) const 
  { return static_cast<const A*>(this)->indirect_expand_1D(i); }
  
  Ret expand(int i, int j) const 
  { return static_cast<const A*>(this)->expand(i,j); }
  Ret indirect_expand_2D(int i) const 
  { return static_cast<const A*>(this)->indirect_expand_2D(i); }
  
  Ret expand(int i, int j, int k) const 
  { return static_cast<const A*>(this)->expand(i,j,k); }
  Ret indirect_expand_3D(int i) const 
  { return static_cast<const A*>(this)->indirect_expand_3D(i); }
  
  Ret expand(int i, int j, int k, int l) const 
  { return static_cast<const A*>(this)->expand(i,j,k,l); }
  Ret indirect_expand_4D(int i) const 
  { return static_cast<const A*>(this)->indirect_expand_4D(i); }
  
  Ret expand(int i, int j, int k, int l, int m) const 
  { return static_cast<const A*>(this)->expand(i,j,k,l,m); }
  Ret indirect_expand_5D(int i) const 
  { return static_cast<const A*>(this)->indirect_expand_5D(i); }
  
  Ret expand(int i, int j, int k, int l, int m, int n) const 
  { return static_cast<const A*>(this)->expand(i,j,k,l,m,n); }
  Ret indirect_expand_6D(int i) const 
  { return static_cast<const A*>(this)->indirect_expand_6D(i); }
  
  // const ExprShape &shape() const 
  const Array_Domain_Type &shape() const 
     { 
    // printf ("Inside of Indexable::shape() const \n");
       return static_cast<const A*>(this)->shape(); 
     }
  // const ExprShape *shapePtr() const
  const Array_Domain_Type *shapePtr() const
  { return static_cast<const A*>(this)->shapePtr(); }
  
#if !defined(HAS_MEMBER_TEMPLATES)

  DefineInAsOps(assignEqu, AssignEqu)
  DefineInAsOps(assignPlus, AssignPlus)
  DefineInAsOps(assignMinus, AssignMinus)
  DefineInAsOps(assignTimes, AssignTimes)
  DefineInAsOps(assignDivide, AssignDivide)
  
  virtual const Array_Domain_Type &getShape() const { return shape(); }

#undef DefineInAsOps

#endif
};

// Output of an arbitrary indexable object.

template <class T, class A>
ostream &operator<<(ostream &os, const Indexable<T,A> &a);

#undef INHERIT_FROM_INASSIGN


///////////////////////////////////////////////////////////////////////////
//
// OPERATORS & FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// CLASS NAME
//    InUnOp
//
// DESCRIPTION
//    Class used to represent generic unary operations 
//    involving Indexable objects. This includes both unary functions 
//    (like 'exp') and unary operators (like negation).
//
///////////////////////////////////////////////////////////////////////////

template<class A, class Op>
class InUnOp : public Indexable<typename Op::RetT, InUnOp<A, Op> > {

public:
   typedef typename A::RetT RetT;

#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~InUnOp();
   InUnOp();
   InUnOp (const InUnOp & X);
   InUnOp & operator= (const InUnOp & X);
#endif

#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  InUnOp(A a, const Array_Domain_Type *sptr): iter(a), s(sptr) { }
#else
  InUnOp(const A &a, const Array_Domain_Type *sptr): iter(a), s(sptr) { }
#endif

#if 0
  typename Op::RetT operator[](Int i) const
  { 
    printf ("Inside of InUnOp::operator[](i=%d) \n",i);
    return Op::apply(iter[i]); 
  }
#endif

  bool usesIndirectAddressing() const 
     { 
    // printf ("Inside of InUnOp::usesIndirectAddressing() \n");
       return iter.usesIndirectAddressing(); 
     }

  int internalNumberOfDimensions () const
     {
    // printf ("Inside of InUnOp::internalNumberOfDimensions() \n");
       return iter.internalNumberOfDimensions(); 
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       if (APP_DEBUG > 2)
            printf ("Inside of InUnOp::containsArrayID(int lhs_id = %d) \n",lhs_id);
       return iter.containsArrayID(lhs_id);
     }

#if 0
  const BaseArray *getDerivedObject () const
     { 
       printf ("Inside of InUnOp::getDerivedObject() \n");
       return iter.getDerivedObject();
     }
#endif

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void setupIndirectAddressingSupport() const
  { iter.setupIndirectAddressingSupport(); }

  typename Op::RetT expand(int i) const
  { return Op::apply(iter.expand(i)); }
  typename Op::RetT indirect_expand_1D(int i) const
  { return Op::apply(iter.indirect_expand_1D(i)); }

  typename Op::RetT expand(int i, int j) const
  { return Op::apply(iter.expand(i,j)); }
  typename Op::RetT indirect_expand_2D(int i) const
  { return Op::apply(iter.indirect_expand_2D(i)); }

  typename Op::RetT expand(int i, int j, int k) const
  { return Op::apply(iter.expand(i,j,k)); }
  typename Op::RetT indirect_expand_3D(int i) const
  { return Op::apply(iter.indirect_expand_3D(i)); }

  typename Op::RetT expand(int i, int j, int k, int l) const
  { return Op::apply(iter.expand(i,j,k,l)); }
  typename Op::RetT indirect_expand_4D(int i) const
  { return Op::apply(iter.indirect_expand_4D(i)); }

  typename Op::RetT expand(int i, int j, int k, int l, int m) const
  { return Op::apply(iter.expand(i,j,k,l,m)); }
  typename Op::RetT indirect_expand_5D(int i) const
  { return Op::apply(iter.indirect_expand_5D(i)); }

  typename Op::RetT expand(int i, int j, int k, int l, int m, int n) const
  { return Op::apply(iter.expand(i,j,k,l,m,n)); }
  typename Op::RetT indirect_expand_6D(int i) const
  { return Op::apply(iter.indirect_expand_6D(i)); }

  const Array_Domain_Type &shape() const { return *s; }
  const Array_Domain_Type *shapePtr() const { return s; }
  
  const InUnOp<A, Op> &data() const { return *this; }

private:

#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  A iter;
#else
  const A & iter;
#endif
  const Array_Domain_Type *s;
};


///////////////////////////////////////////////////////////////////////////
//
// CLASS NAME
//    InInBinOp, LitInBinOp, InLitBinOp
//
// DESCRIPTION
//    Classes used to represent generic binary operations involving 
//    Indexable objects. This includes binary operations and binary 
//    functions (like 'atan2').
//
///////////////////////////////////////////////////////////////////////////

template<class A, class B, class Op>
class InInBinOp : public Indexable<typename Op::RetT,InInBinOp<A, B, Op> > {

public:
   typedef typename A::RetT RetT;

#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~InInBinOp();
   InInBinOp();
   InInBinOp (const InInBinOp & X);
   InInBinOp & operator= (const InInBinOp & X);
#endif

// Re force the return type to be an int for all logical operations.  This allows
// the interface to be consistant with the current A++/P++ interface.
#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  InInBinOp(A a, B b, const Array_Domain_Type *sptr)
  : iter1(a), iter2(b), s(sptr) { }
#else
  InInBinOp(const A &a, const B &b, const Array_Domain_Type *sptr)
  : iter1(a), iter2(b), s(sptr) { }
#endif

#if 0
// Part of a test to debug a missing constructor!
  InInBinOp(A a, B b)
  : iter1(a), iter2(b), s(a.shapePtr()) { }
#endif

#if 0
  typename Op::RetT operator[](Int i) const
  { 
    printf ("Inside of InInBinOp::operator[](i=%d) \n",i);
    return Op::apply(iter1[i], iter2[i]); 
  }
#endif

  bool usesIndirectAddressing() const 
     { 
    // printf ("Inside of InInBinOp::usesIndirectAddressing() \n");
       return iter1.usesIndirectAddressing() || iter2.usesIndirectAddressing(); 
     }

  int internalNumberOfDimensions () const
     {
    // return the maximum dimension of eacher branch
    // printf ("Inside of InInBinOp::internalNumberOfDimensions() \n");
       int temp1 = iter1.internalNumberOfDimensions(); 
       int temp2 = iter2.internalNumberOfDimensions(); 
       return (temp1 > temp2) ? temp1 : temp2;
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       if (APP_DEBUG > 2)
            printf ("Inside of InInBinOp::containsArrayID(int lhs_id = %d) \n",lhs_id);
       return iter1.containsArrayID(lhs_id) || iter2.containsArrayID(lhs_id);
     }

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
  { 
    iter1.buildListOfArrayOperands(arrayList); 
    iter2.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
  { 
    iter1.buildListOfArrayOperands(arrayList); 
    iter2.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
  { 
    iter1.buildListOfArrayOperands(arrayList); 
    iter2.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
  { 
    iter1.buildListOfArrayOperands(arrayList); 
    iter2.buildListOfArrayOperands(arrayList); 
  }

  void setupIndirectAddressingSupport() const
  { 
    iter1.setupIndirectAddressingSupport(); 
    iter2.setupIndirectAddressingSupport(); 
  }

  typename Op::RetT expand(int i) const
  { return Op::apply(iter1.expand(i),iter2.expand(i)); }
  typename Op::RetT indirect_expand_1D(int i) const
  { return Op::apply(iter1.indirect_expand_1D(i),iter2.indirect_expand_1D(i)); }

  typename Op::RetT expand(int i, int j) const
  { return Op::apply(iter1.expand(i,j),iter2.expand(i,j)); }
  typename Op::RetT indirect_expand_2D(int i) const
  { return Op::apply(iter1.indirect_expand_2D(i),iter2.indirect_expand_2D(i)); }

  typename Op::RetT expand(int i, int j, int k) const
  { return Op::apply(iter1.expand(i,j,k),iter2.expand(i,j,k)); }
  typename Op::RetT indirect_expand_3D(int i) const
  { return Op::apply(iter1.indirect_expand_3D(i),iter2.indirect_expand_3D(i)); }

  typename Op::RetT expand(int i, int j, int k, int l) const
  { return Op::apply(iter1.expand(i,j,k,l),iter2.expand(i,j,k,l)); }
  typename Op::RetT indirect_expand_4D(int i) const
  { return Op::apply(iter1.indirect_expand_4D(i),iter2.indirect_expand_4D(i)); }

  typename Op::RetT expand(int i, int j, int k, int l, int m) const
  { return Op::apply(iter1.expand(i,j,k,l,m),iter2.expand(i,j,k,l,m)); }
  typename Op::RetT indirect_expand_5D(int i) const
  { return Op::apply(iter1.indirect_expand_5D(i),iter2.indirect_expand_5D(i)); }

  typename Op::RetT expand(int i, int j, int k, int l, int m, int n) const
  { return Op::apply(iter1.expand(i,j,k,l,m,n),iter2.expand(i,j,k,l,m,n)); }
  typename Op::RetT indirect_expand_6D(int i) const
  { return Op::apply(iter1.indirect_expand_6D(i),iter2.indirect_expand_6D(i)); }

  const Array_Domain_Type &shape() const { return *s; }
  const Array_Domain_Type *shapePtr() const { return s; }

  const InInBinOp<A,B,Op> &data() const { return *this; }

private:

#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  A iter1;
  B iter2;
#else
  const A & iter1;
  const B & iter2;
#endif
  const Array_Domain_Type *s;
};

template<class A, class B, class Op>
class LitInBinOp : 
  public Indexable<typename Op::RetT,LitInBinOp<A, B, Op> > {

public:
   typedef typename B::RetT RetT;

#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~LitInBinOp();
   LitInBinOp();
   LitInBinOp (const LitInBinOp & X);
   LitInBinOp & operator= (const LitInBinOp & X);
#endif

#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  LitInBinOp(const A &a, B b, const Array_Domain_Type *sptr)
  : lit(a), iter(b), s(sptr) { }
#else
  LitInBinOp(const A &a, const B &b, const Array_Domain_Type *sptr)
  : lit(a), iter(b), s(sptr) { }
#endif

#if 0
  typename Op::RetT operator[](Int i) const
  { 
    printf ("Inside of LitInBinOp::operator[](i=%d) \n",i);
    return Op::apply(lit, iter[i]); 
  }
#endif

  bool usesIndirectAddressing() const 
     { 
    // printf ("Inside of LitInBinOp::usesIndirectAddressing() \n");
       return iter.usesIndirectAddressing(); 
     }

  int internalNumberOfDimensions () const
     {
    // printf ("Inside of LitInBinOp::internalNumberOfDimensions() \n");
       return iter.internalNumberOfDimensions(); 
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       if (APP_DEBUG > 0)
            printf ("Inside of LitInBinOp::containsArrayID(int lhs_id = %d) \n",lhs_id);
       return iter.containsArrayID(lhs_id);
     }

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void setupIndirectAddressingSupport() const
  { 
    iter.setupIndirectAddressingSupport(); 
  }

  typename Op::RetT expand(int i) const
  { return Op::apply(lit,iter.expand(i)); }
  typename Op::RetT indirect_expand_1D(int i) const
  { return Op::apply(lit,iter.indirect_expand_1D(i)); }

  typename Op::RetT expand(int i, int j) const
  { return Op::apply(lit,iter.expand(i,j)); }
  typename Op::RetT indirect_expand_2D(int i) const
  { return Op::apply(lit,iter.indirect_expand_2D(i)); }

  typename Op::RetT expand(int i, int j, int k) const
  { return Op::apply(lit,iter.expand(i,j,k)); }
  typename Op::RetT indirect_expand_3D(int i) const
  { return Op::apply(lit,iter.indirect_expand_3D(i)); }

  typename Op::RetT expand(int i, int j, int k, int l) const
  { return Op::apply(lit,iter.expand(i,j,k,l)); }
  typename Op::RetT indirect_expand_4D(int i) const
  { return Op::apply(lit,iter.indirect_expand_4D(i)); }

  typename Op::RetT expand(int i, int j, int k, int l, int m) const
  { return Op::apply(lit,iter.expand(i,j,k,l,m)); }
  typename Op::RetT indirect_expand_5D(int i) const
  { return Op::apply(lit,iter.indirect_expand_5D(i)); }

  typename Op::RetT expand(int i, int j, int k, int l, int m, int n) const
  { return Op::apply(lit,iter.expand(i,j,k,l,m,n)); }
  typename Op::RetT indirect_expand_6D(int i) const
  { return Op::apply(lit,iter.indirect_expand_6D(i)); }

  const Array_Domain_Type &shape() const { return *s; }
  const Array_Domain_Type *shapePtr() const { return s; }

  const LitInBinOp<A,B,Op> &data() const { return *this; }

private:

  A lit;
#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  B iter;
#else
  const B & iter;
#endif
  const Array_Domain_Type *s;
};

template<class A, class B, class Op>
class InLitBinOp : 
  public Indexable<typename Op::RetT,InLitBinOp<A, B, Op> > {

public:
   typedef typename A::RetT RetT;

#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~InLitBinOp();
   InLitBinOp();
   InLitBinOp (const InLitBinOp & X);
   InLitBinOp & operator= (const InLitBinOp & X);
#endif

// We have modified the "const A &a" parameter so it avoids the call to the copy constructor!
// It was previously "A a" which forced a call to the copy constructor for the Array_Descriptor_Type.
#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  InLitBinOp( A a, const B &b, const Array_Domain_Type *sptr)
  : iter(a), lit(b), s(sptr) { }
#else
  InLitBinOp( const A &a, const B &b, const Array_Domain_Type *sptr)
  : iter(a), lit(b), s(sptr) { }
#endif

#if 0
  typename Op::RetT operator[](Int i) const
  { 
    printf ("Inside of InLitBinOp::operator[](i=%d) \n",i);
    return Op::apply(iter[i], lit); 
  }
#endif

  bool usesIndirectAddressing() const 
     { 
    // printf ("Inside of InLitBinOp::usesIndirectAddressing() \n");
       return iter.usesIndirectAddressing(); 
     }

  int internalNumberOfDimensions () const
     {
    // printf ("Inside of InLitBinOp::internalNumberOfDimensions() \n");
       return iter.internalNumberOfDimensions(); 
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       if (APP_DEBUG > 2)
            printf ("Inside of InLitBinOp::containsArrayID(int lhs_id = %d) \n",lhs_id);
       return iter.containsArrayID(lhs_id);
     }

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
  { 
    iter.buildListOfArrayOperands(arrayList); 
  }

  void setupIndirectAddressingSupport() const
  { 
    iter.setupIndirectAddressingSupport(); 
  }

  typename Op::RetT expand(int i) const
  { return Op::apply(iter.expand(i),lit); }
  typename Op::RetT indirect_expand_1D(int i) const
  { return Op::apply(iter.indirect_expand_1D(i),lit); }

  typename Op::RetT expand(int i, int j) const
  { return Op::apply(iter.expand(i,j),lit); }
  typename Op::RetT indirect_expand_2D(int i) const
  { return Op::apply(iter.indirect_expand_2D(i),lit); }

  typename Op::RetT expand(int i, int j, int k) const
  { return Op::apply(iter.expand(i,j,k),lit); }
  typename Op::RetT indirect_expand_3D(int i) const
  { return Op::apply(iter.indirect_expand_3D(i),lit); }

  typename Op::RetT expand(int i, int j, int k, int l) const
  { return Op::apply(iter.expand(i,j,k,l),lit); }
  typename Op::RetT indirect_expand_4D(int i) const
  { return Op::apply(iter.indirect_expand_4D(i),lit); }

  typename Op::RetT expand(int i, int j, int k, int l, int m) const
  { return Op::apply(iter.expand(i,j,k,l,m),lit); }
  typename Op::RetT indirect_expand_5D(int i) const
  { return Op::apply(iter.indirect_expand_5D(i),lit); }

  typename Op::RetT expand(int i, int j, int k, int l, int m, int n) const
  { return Op::apply(iter.expand(i,j,k,l,m,n),lit); }
  typename Op::RetT indirect_expand_6D(int i) const
  { return Op::apply(iter.indirect_expand_6D(i),lit); }

  const Array_Domain_Type &shape()    const { return *s; }
  const Array_Domain_Type *shapePtr() const { return s; }

  const InLitBinOp<A,B,Op> &data() const { return *this; }

private:

// We have to make this a const reference to permit the initializing parameter to be
// passed in as a const reference.  This helps to avoid the unnecessary call to the
// Array_Descriptor_Type copy constructor which contributes to some significant
// overhead (I think).  We need to do this within the rest of the expression template
// implementation.
#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  A iter;
#else
  const A & iter;
#endif
  B lit;
  const Array_Domain_Type *s;
};


///////////////////////////////////////////////////////////////////////////
//
// CLASS NAME
//    Outer2UnFn
//
// DESCRIPTION
//    Template class used to implement a generalized outer product.
//    This class is optimized for array sizes that are multiples of two.
//
///////////////////////////////////////////////////////////////////////////

template<class A, class B, class Op>
class Outer2UnFn : public Indexable<typename Op::RetT,Outer2UnFn<A,B,Op> > {

public:                                                                  
   typedef typename A::RetT RetT;
                                                                         
#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~Outer2UnFn();
   Outer2UnFn();
   Outer2UnFn (const Outer2UnFn & X);
   Outer2UnFn & operator= (const Outer2UnFn & X);
#endif

#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  Outer2UnFn( A a, B b, 
             const Array_Domain_Type *s1, 
             const Array_Domain_Type *s2)
  : iter1(a), iter2(b), s(*s1 & *s2)
  {
    n = s1->size() - 1;

    Int mm = s2->size();
    m = 0;
    while (mm > 1)
      {
	m++;
	mm /= 2;
      }
  }
#else
  Outer2UnFn(const A &a, const B &b,
             const Array_Domain_Type *s1,
             const Array_Domain_Type *s2)
  : iter1(a), iter2(b), s(*s1 & *s2)
  {
    n = s1->size() - 1;

    Int mm = s2->size();
    m = 0;
    while (mm > 1)
      {
        m++;
        mm /= 2;
      }
  }
#endif
  
#if 0
  typename Op::RetT operator[](Int i) const
  { 
    printf ("Inside of Outer2UnFn::operator[](i=%d) \n",i);
    return Op::apply(iter1[i & n], iter2[i >> m]); 
  }
#endif

  bool usesIndirectAddressing() const 
     { 
       printf ("Inside of Outer2UnFn::usesIndirectAddressing() \n");
       APP_ABORT();
       return FALSE; 
     }

  int internalNumberOfDimensions () const
     {
       printf ("Inside of Outer2UnFn::internalNumberOfDimensions() \n");
       APP_ABORT();
       return 0;
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       printf ("Inside of Outer2UnFn::containsArrayID(int lhs_id) --- Exiting ... \n");
       APP_ABORT();
       return FALSE;
     }

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
  { 
    iter1.buildListOfArrayOperands(arrayList); 
    iter2.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
  { 
    iter1.buildListOfArrayOperands(arrayList); 
    iter2.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
  { 
    iter1.buildListOfArrayOperands(arrayList); 
    iter2.buildListOfArrayOperands(arrayList); 
  }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
  { 
    iter1.buildListOfArrayOperands(arrayList); 
    iter2.buildListOfArrayOperands(arrayList); 
  }

  void setupIndirectAddressingSupport() const
  { 
    iter1.setupIndirectAddressingSupport(); 
    iter2.setupIndirectAddressingSupport(); 
  }

  typename Op::RetT expand(int i) const
  { 
    printf ("Inside of Outer2UnFn::expand(int i) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.expand(i),iter2.expand(i)); 
  }
  typename Op::RetT indirect_expand_1D(int i) const
  { 
    printf ("Inside of Outer2UnFn::indirect_expand_1D(int i) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.indirect_expand_1D(i),iter2.indirect_expand_1D(i)); 
  }

  typename Op::RetT expand(int i, int j) const
  { 
    printf ("Inside of Outer2UnFn::expand(int i, int j) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.expand(i,j),iter2.expand(i,j)); 
  }
  typename Op::RetT indirect_expand_2D(int i) const
  { 
    printf ("Inside of Outer2UnFn::indirect_expand_2D(int i) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.indirect_expand_2D(i),iter2.indirect_expand_2D(i)); 
  }

  typename Op::RetT expand(int i, int j, int k) const
  { 
    printf ("Inside of Outer2UnFn::expand(int i, int j, int k) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.expand(i,j,k),iter2.expand(i,j,k)); 
  }
  typename Op::RetT indirect_expand_3D(int i) const
  { 
    printf ("Inside of Outer2UnFn::indirect_expand_3D(int i) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.indirect_expand_3D(i),iter2.indirect_expand_3D(i)); 
  }

  typename Op::RetT expand(int i, int j, int k, int l) const
  { 
    printf ("Inside of Outer2UnFn::expand(int i, int j, int k, int l) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.expand(i,j,k,l),iter2.expand(i,j,k,l)); 
  }
  typename Op::RetT indirect_expand_4D(int i, int j, int k, int l) const
  { 
    printf ("Inside of Outer2UnFn::indirect_expand_4D(int i) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.indirect_expand_4D(i),iter2.indirect_expand_4D(i)); 
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m) const
  { 
    printf ("Inside of Outer2UnFn::expand(int i, int j, int k, int l, int m) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.expand(i,j,k,l,m),iter2.expand(i,j,k,l,m)); 
  }
  typename Op::RetT indirect_expand_5D(int i) const
  { 
    printf ("Inside of Outer2UnFn::indirect_expand_5D(int i) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.indirect_expand_5D(i),iter2.indirect_expand_5D(i)); 
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m, int n) const
  { 
    printf ("Inside of Outer2UnFn::expand(int i, int j, int k, int l, int m, int n) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.expand(i,j,k,l,m,n),iter2.expand(i,j,k,l,m,n)); 
  }
  typename Op::RetT indirect_expand_6D(int i) const
  { 
    printf ("Inside of Outer2UnFn::indirect_expand_6D(int i) --- Exiting ... \n");
    APP_ABORT();
    return Op::apply(iter1.indirect_expand_6D(i),iter2.indirect_expand_6D(i)); 
  }

  const Array_Domain_Type &shape() const { return s; }                         
  const Array_Domain_Type *shapePtr() const { return &s; }                       

  const Outer2UnFn<A,B,Op> &data() const { return *this; }
                                                                         
private:

#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  A iter1;
  B iter2;
#else
  const A & iter1;
  const B & iter2;
#endif
  Int n, m;
  Array_Domain_Type s;
};


#if 1
///////////////////////////////////////////////////////////////////////////
//
// CLASS NAME
//    IotaUnFn
//
// DESCRIPTION
//    Template class used to implement a one-dimensional array constructor.
//    For example, iota(dx, n) will return the sequence 0, dx, 2 * dx, ...
//    (n-1) * dx.
//
///////////////////////////////////////////////////////////////////////////

template<class T>                                               
class IotaUnFn : public Indexable< T,IotaUnFn<T> >{

public:                                                                  
   typedef typename T RetT;
                                                                         
#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~IotaUnFn();
   IotaUnFn();
   IotaUnFn (const IotaUnFn & X);
   IotaUnFn & operator= (const IotaUnFn & X);
#endif

  IotaUnFn(const T &strt, const T &a, Int n)
  : start(strt), atom(a), s(n) { }

#if 0
  T operator[](Int i) const
  { 
    printf ("Inside of IotaUnFn::operator[](i=%d) \n",i);
    return start + i * atom; 
  }
#endif

  bool usesIndirectAddressing() const 
     { 
       printf ("Inside of IotaUnFn::usesIndirectAddressing() \n");
       APP_ABORT();
       return FALSE; 
     }

  int internalNumberOfDimensions () const
     {
       printf ("Inside of IotaUnFn::internalNumberOfDimensions() \n");
       APP_ABORT();
       return 0;
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       printf ("Inside of IotaUnFn::containsArrayID(int lhs_id) --- Exiting ... \n");
       APP_ABORT();
       return FALSE;
     }

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
  { 
    printf ("Inside of buildListOfArrayOperands( list<doubleArray*> & arrayList ) const \n");
  }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
  { 
    printf ("Inside of buildListOfArrayOperands( list<floatArray*> & arrayList ) const \n");
  }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
  { 
    printf ("Inside of buildListOfArrayOperands( list<intArray*> & arrayList ) const \n");
  }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
  { 
    printf ("Inside of buildListOfArrayOperands( list<BaseArray*> & arrayList ) const \n");
  }

  void setupIndirectAddressingSupport() const
  { 
    printf ("Inside of setupIndirectAddressingSupport(void) const \n");
  }

  typename Op::RetT expand(int i) const
  {
    printf ("Inside of IotaUnFn::expand(int i) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }
  typename Op::RetT indirect_expand_1D(int i) const
  {
    printf ("Inside of IotaUnFn::indirect_expand_1D(int i) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }

  typename Op::RetT expand(int i, int j) const
  { 
    printf ("Inside of IotaUnFn::expand(int i, int j) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }
  typename Op::RetT indirect_expand_2D(int i) const
  { 
    printf ("Inside of IotaUnFn::indirect_expand_2D(int i) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }

  typename Op::RetT expand(int i, int j, int k) const
  { 
    printf ("Inside of IotaUnFn::expand(int i, int j, int k) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }
  typename Op::RetT indirect_expand_3D(int i) const
  { 
    printf ("Inside of IotaUnFn::indirect_expand_3D(int i) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }

  typename Op::RetT expand(int i, int j, int k, int l) const
  { 
    printf ("Inside of IotaUnFn::expand(int i, int j, int k, int l) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }
  typename Op::RetT indirect_expand_4D(int i) const
  { 
    printf ("Inside of IotaUnFn::indirect_expand_4D(int i) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m) const
  { 
    printf ("Inside of IotaUnFn::expand(int i, int j, int k, int l, int m) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }
  typename Op::RetT indirect_expand_5D(int i) const
  { 
    printf ("Inside of IotaUnFn::indirect_expand_5D(int i) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m, int n) const
  { 
    printf ("Inside of IotaUnFn::expand(int i, int j, int k, int l, int m, int n) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }
  typename Op::RetT indirect_expand_6D(int i) const
  { 
    printf ("Inside of IotaUnFn::indirect_expand_6D(int i) --- Exiting ... \n");
    APP_ABORT();
    return start + i * atom;
  }

// I'm not clear that we need this for this object!
  const Array_Domain_Type &shape() const { return s; }                         
  const Array_Domain_Type *shapePtr() const { return &s; }                       

  const IotaUnFn<T> &data() const { return *this; }
                                                                         
private:                                                                 
                                                                         
  T start, atom;                                                               
  Array_Domain_Type s;                                                   
};                                                                       
#endif

///////////////////////////////////////////////////////////////////////////
//
// CLASS NAME
//    InInWhOp, LitInWhOp, InLitWhOp
//
// DESCRIPTION
//    Equivalent to ?: for arrays. Takes three arguments. The first is an
//    Array<bool> or an bool expression. An element-by-element evaluation
//    is performed and the result of the function is the 2nd argument if
//    that is true and the 3rd element if it is false.
//
///////////////////////////////////////////////////////////////////////////

template<class W, class A, class B, class Op>
class InInWhOp : 
  public Indexable<typename Op::RetT,InInWhOp<W, A, B, Op> > {

public:
   typedef typename A::RetT RetT;

#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~InInWhOp();
   InInWhOp();
   InInWhOp (const InInWhOp & X);
   InInWhOp & operator= (const InInWhOp & X);
#endif

#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  InInWhOp(W w, A a, B b, const Array_Domain_Type *sptr)
  : wdec(w), iter1(a), iter2(b), s(sptr) { }
#else
  InInWhOp(W w, const A &a, const B &b, const Array_Domain_Type *sptr)
  : wdec(w), iter1(a), iter2(b), s(sptr) { }
#endif

#if 0
  typename Op::RetT operator[](Int i) const
  { 
    printf ("Inside of InInWhOp::operator[](i=%d) \n",i);
    return wdec[i] ? iter1[i] : iter2[i]; 
  }
#endif

  bool usesIndirectAddressing() const 
     { 
       printf ("Inside of InInWhOp::usesIndirectAddressing() \n");
       APP_ABORT();
       return FALSE; 
     }

  int internalNumberOfDimensions () const
     {
       printf ("Inside of InInWhOp::internalNumberOfDimensions() \n");
       APP_ABORT();
       return 0; 
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       printf ("Inside of InInWhOp::containsArrayID(int lhs_id) --- Exiting ... \n");
       APP_ABORT();
       return FALSE;
     }

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
  {
    iter1.buildListOfArrayOperands(arrayList);
    iter2.buildListOfArrayOperands(arrayList);
  }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
  {
    iter1.buildListOfArrayOperands(arrayList);
    iter2.buildListOfArrayOperands(arrayList);
  }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
  {
    iter1.buildListOfArrayOperands(arrayList);
    iter2.buildListOfArrayOperands(arrayList);
  }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
  {
    iter1.buildListOfArrayOperands(arrayList);
    iter2.buildListOfArrayOperands(arrayList);
  }

  void setupIndirectAddressingSupport() const
  { 
    iter1.setupIndirectAddressingSupport();
    iter2.setupIndirectAddressingSupport();
  }

  typename Op::RetT expand(int i) const
  {
    printf ("Inside of InInWhOp::expand(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }
  typename Op::RetT indirect_expand_1D(int i) const
  {
    printf ("Inside of InInWhOp::indirect_expand_1D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }

  typename Op::RetT expand(int i, int j) const
  {
    printf ("Inside of InInWhOp::expand(int i, int j) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }
  typename Op::RetT indirect_expand_2D(int i) const
  {
    printf ("Inside of InInWhOp::indirect_expand_2D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }

  typename Op::RetT expand(int i, int j, int k) const
  {
    printf ("Inside of InInWhOp::expand(int i, int j, int k) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }
  typename Op::RetT indirect_expand_3D(int i) const
  {
    printf ("Inside of InInWhOp::indirect_expand_3D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }

  typename Op::RetT expand(int i, int j, int k, int l) const
  {
    printf ("Inside of InInWhOp::expand(int i, int j, int k, int l) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }
  typename Op::RetT indirect_expand_4D(int i) const
  {
    printf ("Inside of InInWhOp::indirect_expand_4D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m) const
  {
    printf ("Inside of InInWhOp::expand(int i, int j, int k, int l, int m) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }
  typename Op::RetT indirect_expand_5D(int i) const
  {
    printf ("Inside of InInWhOp::indirect_expand_5D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m, int n) const
  {
    printf ("Inside of InInWhOp::expand(int i, int j, int k, int l, int m, int n) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }
  typename Op::RetT indirect_expand_6D(int i) const
  {
    printf ("Inside of InInWhOp::indirect_expand_6D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter1[i] : iter2[i];
  }

  const Array_Domain_Type &shape() const { return *s; }
  const Array_Domain_Type *shapePtr() const { return s; }

  const InInWhOp<W,A,B,Op> &data() const { return *this; }

private:

  W wdec;
#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  A iter1;
  B iter2;
#else
  const A & iter1;
  const B & iter2;
#endif
  const Array_Domain_Type *s;
};

template<class W, class B, class Op>
class LitInWhOp : 
  public Indexable<typename Op::RetT,LitInWhOp<W,B,Op> > {

  typedef typename Op::RetT Ret;
  
public:
   typedef typename B::RetT RetT;

#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~LitInWhOp();
   LitInWhOp();
   LitInWhOp (const LitInWhOp & X);
   LitInWhOp & operator= (const LitInWhOp & X);
#endif

#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  LitInWhOp(W w, const Ret &a, B b, const Array_Domain_Type *sptr)
  : wdec(w), lit(a), iter(b), s(sptr) { }
#else
  LitInWhOp(W w, const Ret &a, const B & b, const Array_Domain_Type *sptr)
  : wdec(w), lit(a), iter(b), s(sptr) { }
#endif

#if 0
  typename Op::RetT operator[](Int i) const
  { 
    printf ("Inside of LitInWhOp::operator[](i=%d) \n",i);
    return wdec[i] ? lit : iter[i]; 
  }
#endif

  bool usesIndirectAddressing() const 
     { 
       printf ("Inside of LitInWhOp::usesIndirectAddressing() \n");
       APP_ABORT();
       return FALSE; 
     }

  int internalNumberOfDimensions () const
     {
       printf ("Inside of LitInWhOp::internalNumberOfDimensions() \n");
       APP_ABORT();
       return 0; 
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       printf ("Inside of LitInWhOp::containsArrayID(int lhs_id) --- Exiting ... \n");
       APP_ABORT();
       return FALSE;
     }

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
  {
    iter.buildListOfArrayOperands(arrayList);
  }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
  {
    iter.buildListOfArrayOperands(arrayList);
  }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
  {
    iter.buildListOfArrayOperands(arrayList);
  }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
  {
    iter.buildListOfArrayOperands(arrayList);
  }

  void setupIndirectAddressingSupport() const
  { 
    iter.setupIndirectAddressingSupport();
  }

  typename Op::RetT expand(int i) const
  {
    printf ("Inside of LitInWhOp::expand(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }
  typename Op::RetT indirect_expand_1D(int i) const
  {
    printf ("Inside of LitInWhOp::indirect_expand_1D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }

  typename Op::RetT expand(int i, int j) const
  {
    printf ("Inside of LitInWhOp::expand(int i, int j) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }
  typename Op::RetT indirect_expand_2D(int i) const
  {
    printf ("Inside of LitInWhOp::indirect_expand_2D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }

  typename Op::RetT expand(int i, int j, int k) const
  {
    printf ("Inside of LitInWhOp::expand(int i, int j, int k) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }
  typename Op::RetT indirect_expand_3D(int i) const
  {
    printf ("Inside of LitInWhOp::indirect_expand_3D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }

  typename Op::RetT expand(int i, int j, int k, int l) const
  {
    printf ("Inside of LitInWhOp::expand(int i, int j, int k, int l) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }
  typename Op::RetT indirect_expand_4D(int i) const
  {
    printf ("Inside of LitInWhOp::indirect_expand_4D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m) const
  {
    printf ("Inside of LitInWhOp::expand(int i, int j, int k, int l, int m) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }
  typename Op::RetT indirect_expand_5D(int i) const
  {
    printf ("Inside of LitInWhOp::indirect_expand_5D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m, int n) const
  {
    printf ("Inside of LitInWhOp::expand(int i, int j, int k, int l, int m, int n) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }
  typename Op::RetT indirect_expand_6D(int i) const
  {
    printf ("Inside of LitInWhOp::indirect_expand_6D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit : iter[i];
  }

  const Array_Domain_Type &shape() const { return *s; }
  const Array_Domain_Type *shapePtr() const { return s; }

  const LitInWhOp<W,B,Op> &data() const { return *this; }

private:

  W wdec;
  Ret lit;
#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  B iter;
#else
  const B & iter;
#endif
  const Array_Domain_Type *s;
};

template<class W, class A, class Op>
class InLitWhOp : 
  public Indexable<typename Op::RetT,InLitWhOp<W, A, Op> > {

  typedef typename Op::RetT Ret;

public:
   typedef typename A::RetT RetT;

#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~InLitWhOp();
   InLitWhOp();
   InLitWhOp (const InLitWhOp & X);
   InLitWhOp & operator= (const InLitWhOp & X);
#endif

#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  InLitWhOp(W w, A a, const Ret &b, const Array_Domain_Type *sptr)
  : wdec(w), iter(a), lit(b), s(sptr) { }
#else
  InLitWhOp(W w, const A &a, const Ret &b, const Array_Domain_Type *sptr)
  : wdec(w), iter(a), lit(b), s(sptr) { }
#endif

#if 0
  typename Op::RetT operator[](Int i) const
  { 
    printf ("Inside of InLitWhOp::operator[](i=%d) \n",i);
    return wdec[i] ? iter[i] : lit; 
  }
#endif

  bool usesIndirectAddressing() const 
     { 
       printf ("Inside of InLitWhOp::usesIndirectAddressing() \n");
       APP_ABORT();
       return FALSE; 
     }

  int internalNumberOfDimensions () const
     {
       printf ("Inside of InLitWhOp::internalNumberOfDimensions() \n");
       APP_ABORT();
       return 0;
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       printf ("Inside of InLitWhOp::containsArrayID(int lhs_id) --- Exiting ... \n");
       APP_ABORT();
       return FALSE;
     }

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
  {
    iter.buildListOfArrayOperands(arrayList);
  }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
  {
    iter.buildListOfArrayOperands(arrayList);
  }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
  {
    iter.buildListOfArrayOperands(arrayList);
  }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
  {
    iter.buildListOfArrayOperands(arrayList);
  }

  void setupIndirectAddressingSupport() const
  { 
    iter.setupIndirectAddressingSupport();
  }

  typename Op::RetT expand(int i) const
  {
    printf ("Inside of InLitWhOp::expand(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }
  typename Op::RetT indirect_expand_1D(int i) const
  {
    printf ("Inside of InLitWhOp::indirect_expand_1D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }

  typename Op::RetT expand(int i, int j) const
  {
    printf ("Inside of InLitWhOp::expand(int i, int j) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }
  typename Op::RetT indirect_expand_2D(int i) const
  {
    printf ("Inside of InLitWhOp::indirect_expand_2D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }

  typename Op::RetT expand(int i, int j, int k) const
  {
    printf ("Inside of InLitWhOp::expand(int i, int j, int k) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }
  typename Op::RetT indirect_expand_3D(int i) const
  {
    printf ("Inside of InLitWhOp::indirect_expand_3D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }

  typename Op::RetT expand(int i, int j, int k, int l) const
  {
    printf ("Inside of InLitWhOp::expand(int i, int j, int k, int l) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }
  typename Op::RetT indirect_expand_4D(int i) const
  {
    printf ("Inside of InLitWhOp::indirect_expand_4D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m) const
  {
    printf ("Inside of InLitWhOp::expand(int i, int j, int k, int l, int m) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }
  typename Op::RetT indirect_expand_5D(int i) const
  {
    printf ("Inside of InLitWhOp::indirect_expand_5D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m, int n) const
  {
    printf ("Inside of InLitWhOp::expand(int i, int j, int k, int l, int m, int n) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }
  typename Op::RetT indirect_expand_6D(int i) const
  {
    printf ("Inside of InLitWhOp::indirect_expand_6D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? iter[i] : lit;
  }

  const Array_Domain_Type &shape() const { return *s; }
  const Array_Domain_Type *shapePtr() const { return s; }

  const InLitWhOp<W,A,Op> &data() const { return *this; }

private:

  W wdec;
#if defined(USE_EXPRESSION_TEMPLATE_COPY_OPTION)
  A iter;
#else
  const A & iter;
#endif
  Ret lit;
  const Array_Domain_Type *s;
};

template<class W, class Op>
class LitLitWhOp : 
  public Indexable<typename Op::RetT,LitLitWhOp<W,Op> > {

  typedef typename Op::RetT Ret;

public:
   typedef typename W::RetT RetT;

#if DEFAULT_MEMBER_FUNCTIONS_IMPLEMENTED
  ~LitLitWhOp();
   LitLitWhOp();
   LitLitWhOp (const LitLitWhOp & X);
   LitLitWhOp & operator= (const LitLitWhOp & X);
#endif

  LitLitWhOp(W w, const Ret &a, const Ret &b, const Array_Domain_Type *sptr)
  : wdec(w), lit1(a), lit2(b), s(sptr) { }

#if 0
  typename Op::RetT operator[](Int i) const
  { 
    printf ("Inside of LitLitWhOp::operator[](i=%d) \n",i);
    return wdec[i] ? lit1 : lit2; 
  }
#endif

  bool usesIndirectAddressing() const 
     { 
       printf ("Inside of LitLitWhOp::usesIndirectAddressing() \n");
       APP_ABORT();
       return FALSE; 
     }

  int internalNumberOfDimensions () const
     {
       printf ("Inside of LitLitWhOp::internalNumberOfDimensions() \n");
       APP_ABORT();
       return 0;
     }

  bool containsArrayID ( int lhs_id ) const
     { 
       printf ("Inside of LitLitWhOp::containsArrayID(int lhs_id) --- Exiting ... \n");
       APP_ABORT();
       return FALSE;
     }

  void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
     {
       printf ("Inside of LitLitWhOp::buildListOfArrayOperands(list<doubleArray*> & arrayList) \n");
       APP_ABORT();
     }

  void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
     {
       printf ("Inside of LitLitWhOp::buildListOfArrayOperands(list<floatArray*> & arrayList) \n");
       APP_ABORT();
     }

  void buildListOfArrayOperands( list<intArray*> & arrayList ) const
     {
       printf ("Inside of LitLitWhOp::buildListOfArrayOperands(list<intArray*> & arrayList) \n");
       APP_ABORT();
     }

  void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
     {
       printf ("Inside of LitLitWhOp::buildListOfArrayOperands(list<BaseArray*> & arrayList) \n");
       APP_ABORT();
     }

  void setupIndirectAddressingSupport() const
     { 
       printf ("Inside of LitLitWhOp::setupIndirectAddressingSupport() \n");
       APP_ABORT();
     }

  typename Op::RetT expand(int i) const
  {
    printf ("Inside of LitLitWhOp::expand(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }
  typename Op::RetT indirect_expand_1D(int i) const
  {
    printf ("Inside of LitLitWhOp::indirect_expand_1D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }

  typename Op::RetT expand(int i, int j) const
  {
    printf ("Inside of LitLitWhOp::expand(int i, int j) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }
  typename Op::RetT indirect_expand_2D(int i) const
  {
    printf ("Inside of LitLitWhOp::indirect_expand_2D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }

  typename Op::RetT expand(int i, int j, int k) const
  {
    printf ("Inside of LitLitWhOp::expand(int i, int j, int k) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }
  typename Op::RetT indirect_expand_3D(int i) const
  {
    printf ("Inside of LitLitWhOp::indirect_expand_3D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }

  typename Op::RetT expand(int i, int j, int k, int l) const
  {
    printf ("Inside of LitLitWhOp::expand(int i, int j, int k, int l) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }
  typename Op::RetT indirect_expand_4D(int i) const
  {
    printf ("Inside of LitLitWhOp::indirect_expand_4D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m) const
  {
    printf ("Inside of LitLitWhOp::expand(int i, int j, int k, int l, int m) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }
  typename Op::RetT indirect_expand_5D(int i) const
  {
    printf ("Inside of LitLitWhOp::indirect_expand_5D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }

  typename Op::RetT expand(int i, int j, int k, int l, int m, int n) const
  {
    printf ("Inside of LitLitWhOp::expand(int i, int j, int k, int l, int m, int n) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }
  typename Op::RetT indirect_expand_6D(int i) const
  {
    printf ("Inside of LitLitWhOp::indirect_expand_6D(int i) --- Exiting ... \n");
    APP_ABORT();
    return wdec[i] ? lit1 : lit2;
  }

  const Array_Domain_Type &shape() const { return *s; }
  const Array_Domain_Type *shapePtr() const { return s; }

  const LitLitWhOp<W,Op> &data() const { return *this; }

private:

  W wdec;
  Ret lit1, lit2;
  const Array_Domain_Type *s;
};


///////////////////////////////////////////////////////////////////////////
//
// MACRO NAME
//    DefineInUnOps
//
// DESCRIPTION
//    Macro used to d e f i n e unary operators involving Indexable objects.
//
///////////////////////////////////////////////////////////////////////////

#define DefineInUnOps(ap, op)                                            \
template<class T, class A>                                               \
inline InUnOp< A,ap<T> >                                                 \
op(const Indexable<T,A> &a)                                              \
{                                                                        \
  return InUnOp< A,ap<T> >(a.data(),a.shapePtr());                       \
}

// Public macro.

#define ExprTemplatizeFn1Arg(fn)                                         \
  DefineInUnOp(fn##UserFunct,fn)                                         \
  DefineInUnOps(fn##UserFunct,fn)

// Unary operators

DefineInUnOps(UnaryPlus, operator+)
DefineInUnOps(UnaryMinus, operator-)
DefineInUnOps(UnaryNot, operator!)

// Unary functions

DefineInUnOps(ExpFunct, exp)
DefineInUnOps(LogFunct, log)
DefineInUnOps(Log10Funct, log10)
DefineInUnOps(SinFunct, sin)
DefineInUnOps(CosFunct, cos)
DefineInUnOps(TanFunct, tan)
DefineInUnOps(AsinFunct, asin)
DefineInUnOps(AcosFunct, acos)
DefineInUnOps(AtanFunct, atan)
DefineInUnOps(SinhFunct, sinh)
DefineInUnOps(CoshFunct, cosh)
DefineInUnOps(TanhFunct, tanh)
DefineInUnOps(Pow2Funct, pow2)
DefineInUnOps(Pow3Funct, pow3)
DefineInUnOps(Pow4Funct, pow4)
DefineInUnOps(SqrtFunct, sqrt)
DefineInUnOps(CeilFunct, ceil)
DefineInUnOps(FloorFunct, floor)
DefineInUnOps(CexpFunct, cexp)
DefineInUnOps(CsqrtFunct, csqrt)
DefineInUnOps(RealXFunct, real)
DefineInUnOps(ImagFunct, imag)
DefineInUnOps(AbsFunct, abs)
DefineInUnOps(FabsFunct, fabs)
DefineInUnOps(ConjgFunct, conjg)


///////////////////////////////////////////////////////////////////////////
//
// MACRO NAME
//    DefineInBinOps
//
// DESCRIPTION
//    Macro used to d e f i n e binary operator functions involving Indexable
//    objects.
//
///////////////////////////////////////////////////////////////////////////

// Line taken from InInBinOp
// return InInBinOp< A, B, ap<T1, T2> >(a.data(), b.data(), a.shapePtr()); 

#define DefineInBinOps(ap, op)                                           \
template<class T1, class T2, class A, class B>                           \
inline InInBinOp< A, B, ap<T1, T2> >                                     \
op(const Indexable<T1,A> &a, const Indexable<T2,B> &b)                   \
{                                                                        \
  assert(a.shape().conformsTo(b.shape()));                               \
  return InInBinOp< A, B, ap<T1, T2> >(a.data(), b.data(), a.shapePtr()); \
}                                                                        \
template<class T, class A>                                               \
inline LitInBinOp<T,A,ap<T, T> >                                         \
op(const T &a, const Indexable<T,A> &b)                                  \
{                                                                        \
  return LitInBinOp< T,A,ap<T, T> >                                      \
    (a, b.data(), b.shapePtr());                                         \
}                                                                        \
template<class T, class A>                                               \
inline InLitBinOp< A,T,ap<T, T> >                                        \
op(const Indexable<T,A> &a, const T &b)                                  \
{                                                                        \
  return InLitBinOp< A,T,ap<T, T> >                                      \
    (a.data(), b, a.shapePtr());                                         \
}

// The actual operators:

DefineInBinOps(Plus, operator+)
DefineInBinOps(Minus, operator-)
DefineInBinOps(Times, operator*)
DefineInBinOps(Divide, operator/)

// 2 argument functions:

DefineInBinOps(Atan2Funct, atan2)
DefineInBinOps(PowFunct, pow)
DefineInBinOps(FmodFunct, fmod)

// bool operators:

DefineInBinOps(Equal, operator==)
DefineInBinOps(NotEqual, operator!=)
DefineInBinOps(GrThan, operator>)
DefineInBinOps(LessThan, operator<)
DefineInBinOps(GrThanEq, operator>=)
DefineInBinOps(LessThanEq, operator<=)
DefineInBinOps(LogicalOr, operator||)
DefineInBinOps(LogicalAnd, operator&&)


///////////////////////////////////////////////////////////////////////////
//
// TEMPLATE NAME
//    outer2
//
// DESCRIPTION
//    Functions used to create generalized outer products 
//    (power of 2 version).
//
///////////////////////////////////////////////////////////////////////////

template <class T1, class T2, class A, class B>
inline Outer2UnFn< A, B, Times<T1, T2> >
outer2(const Indexable<T1,A> &a, const Indexable<T2,B> &b)
{
  return Outer2UnFn< A, B,
    Times<T1, T2> >(a.data(), b.data(), a.shapePtr(), b.shapePtr());
}

template <class T1, class T2, class A, class B, class Op>
inline Outer2UnFn< A, B, Op >
outer2(const Indexable<T1,A> &a, const Indexable<T2,B> &b,
  const Op &)
{
  return Outer2UnFn< A, B, Op >
    (a.data(), b.data(), a.shapePtr(), b.shapePtr());
}


#if 1
///////////////////////////////////////////////////////////////////////////
//
// TEMPLATE NAME
//    iota
//
// DESCRIPTION
//    Function used to implement a one-dimensional array constructor.
//    For example, iota(dx, n) will return the sequence 0, dx, 2 * dx, ...
//    (n-1) * dx.
//
///////////////////////////////////////////////////////////////////////////

template <class T>
inline IotaUnFn<T>
iota(const T &c, Int n)
{
  typedef T RetT;
  return IotaUnFn<T>(T(0), c, n);
}

template <class T>
inline IotaUnFn<T>
iota(const T &s, const T &e, Int n)
{
  typedef T RetT;
  return IotaUnFn<T>(s, (e-s)/T(n <= 1 ? 1 : n - 1), n);
}
#endif


///////////////////////////////////////////////////////////////////////////
//
// TEMPLATE NAME
//    where
//
// DESCRIPTION
//    Function used to implement the 'where' expression.
//
///////////////////////////////////////////////////////////////////////////

template<class T1, class T2, class T3, class W, class A, class B>
inline InInWhOp< W, A, B,Promote<T2,T3> >
where(const Indexable<T1,W> &w, 
  const Indexable<T2,A> &a, const Indexable<T3,B> &b)
{
  assert(w.shape().conformsTo(a.shape()));
  assert(w.shape().conformsTo(b.shape()));
  return InInWhOp< W, A, B,Promote<T2,T3> >
    (w.data(), a.data(), b.data(), w.shapePtr());
}
template<class T1, class T2, class W, class A>
inline LitInWhOp< W,A,Promote<T2,T2> >
where(const Indexable<T1,W> &w, const T2 &a, const Indexable<T2,A> &b)
{
  assert(w.shape().conformsTo(b.shape()));
  return LitInWhOp< W,A,Promote<T2,T2> >
    (w.data(), a, b.data(), w.shapePtr());
}
template<class T1, class T2, class W, class A>
inline InLitWhOp< W,A,Promote<T2,T2> >
where(const Indexable<T1,W> &w, const Indexable<T2,A> &a, const T2 &b)
{
  assert(w.shape().conformsTo(a.shape()));
  return InLitWhOp< W,A,Promote<T2,T2> >
    (w.data(), a.data(), b, w.shapePtr());
}
template<class T1, class W>
inline LitLitWhOp< W,Promote<T1,T1> >
where(const Indexable<T1,W> &w, const T1 &a, const T1 &b)
{
  return LitLitWhOp< W,Promote<T1,T1> >
    (w.data(), a, b, w.shapePtr());
}


///////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME
//    sum/product/min/max
//
// DESCRIPTION
//    Returns the sum/product/min/max of an expression.
//
///////////////////////////////////////////////////////////////////////////

#if 0
// These functions are in inline_expression_templates.h_m4
template<class T, class A>
inline T sum(const Indexable<T,A> &rhs)
{
  Int i, n = rhs.shape().size();
  T acc = 0;

  for (i = 0; i < n; i++)
    acc += rhs[i];
    
  return acc;
}

template<class T, class A>
inline T product(const Indexable<T,A> &rhs)
{
  Int i, n = rhs.shape().size();
  T acc = 1;

  for (i = 0; i < n; i++)
    acc *= rhs[i];
    
  return acc;
}

template<class T, class A>
inline T max(const Indexable<T,A> &rhs)
{
  Int i, n = rhs.shape().size();
  // const T *d = rhs;
  const Indexable<T,A> d = rhs;
  T val = d[0];

  for (i = 1; i < n; i++)
    val = (d[i] > val) ? d[i] : val;
    
  return val;
}

template<class T, class A>
inline T min(const Indexable<T,A> &rhs)
{
  Int i, n = rhs.shape().size();
  // const T *d = &rhs;
  // const T *d = rhs;
  // const Indexable<T,A> *d = rhs;
  const Indexable<T,A> d = rhs;
  T val = d[0];

  for (i = 1; i < n; i++)
    val = (d[i] < val) ? d[i] : val;
    
  return val;
}
#endif


///////////////////////////////////////////////////////////////////////////
//
// INCLUDE CC FILE IF TEMPLATE INSTANTIATION SYSTEM IS STUPID
//
///////////////////////////////////////////////////////////////////////////
/*
#if !defined(HANDLES_TEMPLATES_IN_CCFILES)
#include "Expressions.cc"
#endif
*/

///////////////////////////////////////////////////////////////////////////
//
// END OF FILE
//
///////////////////////////////////////////////////////////////////////////

   // if defined(USE_EXPRESSION_TEMPLATES)
#endif 


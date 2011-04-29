// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file rosehpct/util/types.hh
 *  \brief Type support, adapted from Alexandrescu (2001).
 *  \ingroup ROSEHPCT_UTIL
 *
 *  $Id: types.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#if !defined(INC_UTIL_TYPES_HH)
//! util/types.hh included
#define INC_UTIL_TYPES_HH

#include <map>

/*!
 *  \brief Advanced compile-time type support, adapted from utility
 *  routines written by Alexandrescu (2001) for the Loki library.
 */
namespace Types
{
  //! \addtogroup ROSEHPCT_UTIL
  //@{

  /*!
   *  Generate distinct types from an integer (i.e., Int2Type<0> is
   *  distinct from Int2Type<1>) for use in type disambiguation.
   *
   *  \note See Alexandrescu (2001), Section 2.4.
   */
  template <int v>
  struct Int2Type
  {
    enum {value = v};
  };

  /*!
   *  Generate a distinct type from a type name (i.e., Type2Type<int>
   *  is distinct from Type2Type<MyClass>) for use in function
   *  disambiguation and simulation of partial template function
   *  specialization.
   *
   *  \note See Alexandrescu (2001), Section 2.5.
   */
  template <typename T>
  class Type2Type
  {
    typedef T OriginalType;
  };

  /*!
   *  \name Compile-time on-the-fly selection of a type T if flag is
   *  true, or U otherwise.
   *
   *  The following example selects a container's value type to be a
   *  pointer if the object is specified as being polymorphic (in a
   *  template parameter), or a value type otherwise.
   *  \code
   *  template <typename T, bool isPoly>
   *  class Container
   *  {
   *    // Value is a pointer if polymorphic, or value type otherwise.
   *    typedef typename Select<isPoly, T*, T>::Result ValueType;
   *  };
   *  \endcode
   *
   *  \note See Alexandrescu (2001), Section 2.6.
   */
  /*@{*/
  //! Selects T by default (flag is true)
  template <bool flag, typename T, typename U>
  struct Select
  {
    typedef T Result;
  };

  //! Selects U when flag is false
  template <typename T, typename U>
  struct Select<false, T, U>
  {
    typedef U Result;
  };
  /*@}*/

  /*!
   *  Compile-time test for the existence of automatic conversion
   *  between two types.
   *
   *  \note See Alexandrescu (2001), Section 2.7.
   */
  template <class T, class U>
  class Conversion
  {
    //! \name Two distinct types.
    /*@{*/
    //! A basic type.
    typedef char Small;
    //! A type whose size is guaranteed to differ from Small.
    class Big { char dummy[2]; };
    /*@}*/

    static Small Test (U); //!< "Called" if conversion exists 
    static Big Test (...); //!< "Called" if no conversion exists 
    static T MakeT (); //!< Generates an object of type T 

  public:
    //! Uses sizeof trick to distinguish between Small and Big
    enum {exists = sizeof (Test (MakeT ())) == sizeof (Small) };

    //! Avoids g++ warning
    virtual void dummy (void) = 0;
  };

  //! Invalid type, to mark end of a type list or 'type not found'.
  class NullType {};

  //! Empty type
  struct EmptyType {};

  /*!
   *  \name Typelist support.
   *
   *  \note See Alexandrescu (2001), Chapter 3.
   */
  /*@{*/
  //! Typelist building block
  template <class T, class U>
  struct Typelist
  {
    typedef T Head; //!< First type in the pair 
    typedef U Tail; //!< Last type in the pair; also, sublists. 
  };

  //! \name Linearized typelists of various lengths.
  /*@{*/
#define TYPELIST_1(T1)  \
          Types::Typelist<T1, Types::NullType>
#define TYPELIST_2(T1, T2)  \
          Types::Typelist<T1, TYPELIST_1(T2) >
#define TYPELIST_3(T1, T2, T3)  \
          Types::Typelist<T1, TYPELIST_2(T2, T3) >
#define TYPELIST_4(T1, T2, T3, T4)  \
          Types::Typelist<T1, TYPELIST_3(T2, T3, T4) >
#define TYPELIST_5(T1, T2, T3, T4, T5) \
          Types::Typelist<T1, TYPELIST_4(T2, T3, T4, T5) >
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) \
          Types::Typelist<T1, TYPELIST_5(T2, T3, T4, T5, T6) >
#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) \
          Types::Typelist<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >
#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) \
          Types::Typelist<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >
#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
          Types::Typelist<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >
#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) \
          Types::Typelist<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >
  /*@}*/

  /* Computes the length of a typelist. */
  template <class TList> struct Length;
  //! Zero-length typelist.
  template <> struct Length<NullType>
  {
    enum { value = 0 };
  };
  //! n-length typelist.
  template <class T, class U>
  struct Length< Typelist<T, U> >
  {
    enum { value = 1 + Length<U>::value };
  };

  /* Indexed access to a typelist. */
  template <class TList, unsigned int index> struct TypeAt;

  //! Return the first element in a typelist.
  template <class Head, class Tail>
  struct TypeAt<Typelist<Head, Tail>, 0>
  {
    typedef Head Result; //!< Head type 
  };

  //! Returns the i-th element in a typelist.
  template <class Head, class Tail, unsigned int i>
  struct TypeAt<Typelist<Head, Tail>, i>
  {
    typedef typename TypeAt<Tail, i-1>::Result Result; //!< Recursive call 
  };
  /*@}*/

  /* Typelist search. */
  template <class TList, class T> struct IndexOf;

  //! Type not found
  template <class T>
  struct IndexOf<NullType, T>
  {
    enum {value = -1};
  };

  //! Type found
  template <class T, class Tail>
  struct IndexOf<Typelist<T, Tail>, T>
  {
    enum {value = 0};
  };

  //! Search for type T
  template <class Head, class Tail, class T>
  struct IndexOf<Typelist<Head, Tail>, T>
  {
  private:
    enum {temp = IndexOf<Tail, T>::value};
  public:
    enum {value = (temp < 0) ? -1 : 1 + temp};
  };

  /*!
   *  Search a typelist Tlist for a type T, and return the boolean
   *  result in 'found'.
   */
  template <class TList, class T>
  struct SearchTypes
  {
  public:
    enum {found = IndexOf<TList, T>::value >= 0};
  };

  /*!
   *  Compile-time detection of basic type properties.
   *
   *  \note See Alexandrescu (2001), Section 2.10.
   *
   *  \todo Conditionally-compile 'long long' in the list of
   *  fundamental types.
   *  \todo What happens to 'enum' types in TypeTraits?
   */
  template <typename T>
  class TypeTraits
  {
    //! \name Detect pointer types.
    /*@{*/
  private:
    //! Signal a non-pointer type.
    template <class U>
    struct PointerTraits
    {
      enum { result = false }; //!< false ==> non-pointer type 
      typedef NullType PointeeType; //!< Non-pointer type points to nothing 
    };

    /*!
     *  Signal a pointer type, and indicate the type to which it
     *  points.
     */
    template <class U>
    struct PointerTraits<U*>
    {
      enum { result = true }; //!< Is a pointer type.
      typedef U PointeeType; //!< Points to type U.
    };

  public:
    //! True <==> T is a pointer type
    enum { isPointer = PointerTraits<T>::result };

    //! Type to which T points if isPointer, or NullType otherwise.
    typedef typename PointerTraits<T>::PointeeType PointeeType;
    /*@}*/

    //! \name Detect reference types.
    /*@{*/
  private:
    //! Signal a non-reference type.
    template <class U>
    struct ReferenceTraits
    {
      enum { result = false }; //!< Is not a reference type. 
      typedef U ReferredType; //!< Type referred to is U 
    };

    /*!
     *  Signal a referenced type, and indicate the type to which it
     *  refers.
     */
    template <class U>
    struct ReferenceTraits<U&>
    {
      enum { result = true }; //!< Is a reference type. 
      typedef U ReferredType; //!< Type referred to is U. 
    };

  public:
    enum { isReference = ReferenceTraits<T>::result };
    typedef typename ReferenceTraits<T>::ReferredType ReferredType;
    /*@}*/

    //! \name Detect pointers-to-members
    /*@{*/
  private:
    //! Signal a type is not a pointer to a member.
    template <class U>
    struct PToMTraits
    {
      enum { result = false }; //!< Is not a pointer to a member. 
    };

    //! Signal a type is a pointer to a member.
    template <class U, class V>
    struct PToMTraits<U V::*>
    {
      enum { result = true }; //!< Is a pointer to a member. 
    };

  public:
    //! True if type T is a pointer to a member
    enum { isMemberPointer = PToMTraits<T>::result };
    /*@}*/

    //! \name Remove 'const' qualifier
    /*@{*/
  private:
    //! Non-const version of 'U' is U.
    template <class U>
    struct UnConst
    {
      typedef U Result; //! Non-const version of a non-const type
    };

    //! Non-const version of 'const U' is U.
    template <class U>
    struct UnConst<const U>
    {
      typedef U Result; //!< Non-const version of a const type
    };

  public:
    //! Non-const version of the type T.
    typedef typename UnConst<T>::Result NonConstType;
    /*@}*/

  public:
    //! Unsigned integer types
    typedef TYPELIST_4(unsigned char, unsigned short int, unsigned int, unsigned long int) UnsignedInts;

    //! Signed integer types
    typedef TYPELIST_4(char, short int, int, long int) SignedInts;

    //! Other integral types
    typedef TYPELIST_3(bool, char, wchar_t) OtherInts;

    /*! (Real) floating-point types */
    typedef TYPELIST_3(float, double, long double) Floats;

    //! True if type T is an unsigned fundamental integer type
    enum { isStdUnsignedInt = SearchTypes<UnsignedInts, T>::found };

    //! True if type T is a signed fundamental integer type
    enum { isStdSignedInt = SearchTypes<SignedInts, T>::found };

    //! True if type T is an 'other' ordinal type
    enum { isOtherInt = SearchTypes<OtherInts, T>::found };

    //! True if type T is any fundamental ordinal type
    enum { isStdIntegral = isStdUnsignedInt || isStdSignedInt || isOtherInt };

    //! True if type T is a fundamental floating-point (real) type
    enum { isStdFloat = SearchTypes<Floats, T>::found };

    //! True if type T is an arithmetic type
    enum { isStdArith = isStdIntegral || isStdFloat };

  public:
    /*!
     *  Automatically selects the most-efficient way to pass type T as
     *  a parameter (i.e., either by value or by reference.
     *
     *  \note Does not work for enums?
     */
    typedef typename Select<isStdArith || isPointer || isMemberPointer,
                            T, ReferredType&>::Result  ParameterType;
  }; /* END TypeTraits */

  /*!
   *  \brief Provides support for creating Singleton classes, in the
   *  spirit of the implementation provided in Loki by Alexandrescu
   *  (2001), Chapter 6.
   *
   *  This implementation provides only a small fraction of the
   *  functionality provided in Loki.
   */
  template <class T>
  class SingletonHolder
  {
  public:
    //! Returns the single instance of T
    static T& instance (void);
    
  private:
    //! \name Private members to prevent instantiation.
    /*@{*/
    SingletonHolder (void);
    SingletonHolder (const SingletonHolder<T>& sh);
    ~SingletonHolder (void);
    /*@}*/
  };

  /* ---------- BEGIN Template member function bodies ---------- */
  template <class T>
  T&
  SingletonHolder<T>::instance (void)
  {
    static T obj;
    return obj;
  }

  template <class T>
  SingletonHolder<T>::SingletonHolder (void)
  {
  }

  template <class T>
  SingletonHolder<T>::SingletonHolder (const SingletonHolder<T> &)
  {
  }

  template <class T>
  SingletonHolder<T>::~SingletonHolder (void)
  {
  }
  /* ---------- END Template member function bodies ---------- */

  /*!
   *  \brief Provides support for creating object factories, adapted
   *  from the implementation in Alexandrescu (2001), Chapter 8.
   */
  template <class ProdType, typename IdType, typename ProdCreator>
  class Factory
  {
  public:
    virtual ~Factory () {}

    //! Add a new type to the factory
    bool registerType (const IdType& id, ProdCreator creator);
    //! Remove a type from the factory
    bool unregisterType (const IdType& id);
    //! Create an object of the desired type
    ProdType* createObject (const IdType& id);

  protected:
    //! Calls a particular objects registered 'create' routine
    virtual ProdType* callCreate (const IdType& id, ProdCreator creator)
      const = 0;

  private:
    //! Map for registered type lookup.
    typedef std::map<IdType, ProdCreator> AssocMapType;
    //! Registered type lookup table.
    AssocMapType product_table_;
  };

  /* ---------- BEGIN Template member function bodies ---------- */
  template <class ProdType, typename IdType, typename ProdCreator>
  bool
  Factory<ProdType, IdType, ProdCreator>::registerType (const IdType& id,
                                                        ProdCreator creator)
  {
    return product_table_.insert (typename AssocMapType::value_type(id, creator)).second;
  }

  template <class ProdType, typename IdType, typename ProdCreator>
  bool
  Factory<ProdType, IdType, ProdCreator>::unregisterType (const IdType& id)
  {
    return product_table_.erase (id) == 1;
  }

  template <class ProdType, typename IdType, typename ProdCreator>
  ProdType *
  Factory<ProdType, IdType, ProdCreator>::createObject (const IdType& id)
  {
    typename AssocMapType::const_iterator i = product_table_.find (id);
    if (i != product_table_.end ())
      return callCreate (id, i->second);
    return NULL;
  }
  /* ---------- END Template member function bodies ---------- */

  //@}
}

#endif

/* eof */

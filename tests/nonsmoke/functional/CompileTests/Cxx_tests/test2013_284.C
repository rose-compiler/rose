//------------------------------------------------------------------------------
//
/// @class   GenericObject
/// @ingroup GenericsPackage
///
/// This class provides for a generic object. This is only intended for as a
/// base class from which other classes are expected to be derived. The only
/// real function is to supply an "id number".
///
/// @note This class is actually indended as an abstract base class. There
///       isn't really such a thing as a GenericObject. Thus the constructor
///       is protected. Traditionally a pure virtual function would be used
///       to make for an abstract base class but there isn't really a natural
///       method for this here.
///
/// @since Generics 1.0
///
/// @author Jim Reus
///
/// @date 23Nov2008
//
//------------------------------------------------------------------------------

class GenericObject
{
  public:
                                GenericObject ( );
                                GenericObject ( const GenericObject& );
           virtual             ~GenericObject ( );
           GenericObject&       operator= ( const GenericObject& );

           virtual const char*  debugName ( ) const;
           static const char*   debugNameOf ( const GenericObject* );
           int                  id ( ) const;
           virtual const char*  typeName ( ) const;

  private:
           int                  m_id;
           static int           m_unique;
};

//------------------------------------------------------------------------------
//
/// Object id.
///
/// This public method returns the id number of this object. It is expected
/// that derived classes will make use of this method.
///
/// @return  A non-negative integer is always returned.
///
/// @since Generics 1.0
///
/// @author Jim Reus
///
/// @date 01Aug2008
//
//------------------------------------------------------------------------------

inline int GenericObject::id ( ) const
{
   return m_id;
}

//------------------------------------------------------------------------------
//
/// @class   RawHashTable
/// @ingroup GenericsPackage
///
/// This abstract base class provides for a very generic hash table. It is
/// used for associating generic objects (values) with generic objects (keys).
/// Such object-object associations are known as key-value pairs or simply pairs.
/// A hash table may hold zero or more key-value pairs. This class provides only
/// the most basic operations such as insertion and removal of key-value pairs.
/// Derived classes are expected to provide operations involving specific types
/// of keys such as key hashing, key comparison, key copying, and key deletion.
///
/// @note Such hash tables do not "own" the values (object) to which they
///       refer. The caller is expected to manage the objects. For convinience
///       a "removal handler" may be supplied when a RawHashTable object
///       is created. If such a handler is supplied, it will be called whenever
///       a key-value pair is actually removed from the table. It is expected
///       that this function will actually delete the value in an appropriate
///       fashion. Of course such a function is only needed when the table
///       "owns" the values. Be careful, disaster of biblical proprtions may
///       occur if objects "in" a hash table are destroyed before they are
///       removed from the table.
///
/// @since Generics 2.0
///
/// @author Jim Reus
///
/// @date 09Oct2010
//
//------------------------------------------------------------------------------

class RawHashTable : public GenericObject
{
  public:
             class Iterator;
             class SortedIterator;
  private:
             class Pair;

  friend class Iterator;
  friend class Pair;
  friend class SortedIterator;

  public:
             /// @nosubgrouping
             /// @name Constructors
             //@{
                                          RawHashTable ( );
                                          RawHashTable ( int sz );
                                          RawHashTable ( void (*f)(const void*,void*) );
                                          RawHashTable ( int sz, void (*f)(const void*,void*) );
             //@}
             /// @name Destructor
             //@{
                  virtual                ~RawHashTable ( );
             //@}
             /// @name Methods supplied by RawHashTable
             //@{
                  int                     check ( ) const;
                  virtual const char*     debugName ( ) const;
                  int                     doesNotHaveKey ( const void* key ) const;
                  int                     hash ( const void* key ) const;
                  int                     hasKey ( const void* key ) const;
                  int                     insert ( const void* key, void* value );
                  int                     isEmpty ( ) const;
                  int                     isNotEmpty ( ) const;
                  int                     numberOfPairs ( ) const;
                  int                     numberOfDeletedPairs ( ) const;
                  int                     remove ( const void* key );
                  int                     removeAll ( );
                  int                     size ( ) const;
                  int                     sortByKey ( );
                  virtual const char*     typeName ( ) const;
                  void*                   valueOf ( const void* key ) const;
             //@}
             /// @name Functions supplied by RawHashTable
             //@{
                  static int              compare ( const char* a, const char* b );
                  static char*            copyOf ( const char* s );
                  static const char*      debugNameOf ( const char* s );
                  static const char*      debugNameOf ( const void* p );
                  static const char*      debugNameOf ( const RawHashTable* h );
                  static int              equals ( const char* a, const char* b );
                  static int              lengthOf ( const char* s );
                  static void             setErrorCallerData ( void* callerData );
                  static void             setErrorHandler ( void (*f)(const char* msg, void*) );
                  static void             setErrorHandler ( void (*f)(const char* msg, void*), void* callerData );
             //@}

  protected:
             /// @name Protected functions supplied by RawHashTable
             //@{
                  static void             detectedError ( const char* msg );
             //@}

             /// @name Protected methods to be supplied by derived classes
             //@{
                                          //--------------------------------------
                                          //
                                          /// Compare keys.
                                          ///
                                          /// This method is used to compare two keys expressed as
                                          /// two generic pointers. Note that the derived class is
                                          /// expected to cast these pointers to meaningful pointer
                                          /// types and perform a comparison of the keys to which
                                          /// these useful pointer refer.
                                          ///
                                          /// @param a  A pointer to the first or left key in the
                                          ///           comparison.
                                          ///
                                          /// @param b  A pointer to the second or right key in the
                                          ///           comparison.
                                          ///
                                          /// @return An integer value is returned whose sign indicates the
                                          ///         order releationship between the two given keys. A
                                          ///         negative return value indicates that the first key
                                          ///         is "less than" the second key, a zero return value
                                          ///         indicates that the first key is "equal to" the second
                                          ///         key (not the pointers but the keys to which the pointers
                                          ///         refer), a positive value indicates that the first key
                                          ///         is "greater than" the second key.
                                          ///
                                          /// @since Generics 2.0
                                          ///
                                          /// @author Jim Reus
                                          ///
                                          /// @date 10Oct2010
                                          //
                                          //--------------------------------------
                  virtual int             compareKeys ( const void* a, const void* b ) const = 0;
                                          //--------------------------------------
                                          //
                                          /// Copy a key.
                                          ///
                                          /// This method is used to copy a key expressed as a generic
                                          /// pointer. Note that the derived class is expected to cast
                                          /// this pointer to a meaningful pointer type and properly
                                          /// copy the key which it refers.
                                          ///
                                          /// @param key  A non-nil pointer to the key to be copied.
                                          ///
                                          /// @return A generic pointer to newly allocated storage
                                          ///         representing a copy of the given key. Note
                                          ///         that the deleteKey() method should be used to
                                          ///         release this storage. A nil return value generally
                                          ///         indicates that sufficient memory could not be
                                          ///         allocated.
                                          ///
                                          /// @since Generics 2.0
                                          ///
                                          /// @author Jim Reus
                                          ///
                                          /// @date 10Oct2010
                                          //
                                          //--------------------------------------
                  virtual void*           copyKey ( const void* key ) const = 0;
                                          //--------------------------------------
                                          //
                                          /// Delete a key.
                                          ///
                                          /// This method is used to release the storage used to
                                          /// represent a key expressed as a generic pointer. Note
                                          /// that the derived class is expected to cast this pointer
                                          /// to a meaningful pointer type and properly release the
                                          /// key which it refers.
                                          ///
                                          /// @param key  A non-nil pointer to the key to be released.
                                          ///
                                          /// @return A generic nil pointer is always returned.
                                          ///
                                          /// @since Generics 2.0
                                          ///
                                          /// @author Jim Reus
                                          ///
                                          /// @date 10Oct2010
                                          //
                                          //--------------------------------------
                  virtual void*           deleteKey ( void* key ) const = 0;
                                          //--------------------------------------
                                          //
                                          /// Hash a key.
                                          ///
                                          /// This method is used to compute an integer hash value
                                          /// for the key expressed as a generic pointer. Note that
                                          /// the derived class is expected to cast this pointer
                                          /// to a meaningful pointer type and properly compute an
                                          /// appropriate hash value for the key which it refers.
                                          ///
                                          /// @param key  A non-nil pointer to the key to be examined.
                                          ///
                                          /// @return A non-negative hash value. It is up to the
                                          ///         caller to use a modulo operator to force this
                                          ///         hash value into the proper range.
                                          ///
                                          /// @since Generics 2.0
                                          ///
                                          /// @author Jim Reus
                                          ///
                                          /// @date 10Oct2010
                                          //
                                          //--------------------------------------
                  virtual int             hashKey ( const void* key ) const = 0;
                                          //--------------------------------------
                                          //
                                          /// Printable string for a key.
                                          ///
                                          /// This method returns a printable string representing
                                          /// the given key (expressed as a generic pointer). Note
                                          /// that the derived class is expected to cast this pointer
                                          /// to a meaningful pointer type and properly form a
                                          /// printable representation of the key which it refers.
                                          ///
                                          /// @param key  A non-nil pointer to the key to be examined.
                                          ///
                                          /// @return A pointer to a printable C-style NULL-terminated
                                          ///         string is always returned. Note that this string
                                          ///         should be considered static with a limited life
                                          ///         and must not be released by the caller.
                                          ///
                                          /// @since Generics 2.0
                                          ///
                                          /// @author Jim Reus
                                          ///
                                          /// @date 10Oct2010
                                          //
                                          //--------------------------------------
                  virtual const char*     keyName ( const void* key ) const = 0;
                                          //--------------------------------------
                                          //
                                          /// Check if key is valid.
                                          ///
                                          /// This method is used to examine the given key to determine
                                          /// if it is a valid key. Note that the derived class is
                                          /// expected to cast this pointer to a meaningful pointer
                                          /// type and properly examine the contents for validity.
                                          ///
                                          /// @param key  A non-nil pointer to the key to be examined.
                                          ///
                                          /// @return A value of true (non-zero) is returned if the
                                          ///         $key appears to be a valid key, otherwise a value
                                          ///         of false (zero) is returned.
                                          ///
                                          /// @since Generics 2.0
                                          ///
                                          /// @author Jim Reus
                                          ///
                                          /// @date 10Oct2010
                                          //
                                          //--------------------------------------
                  virtual int             validKey ( const void* key ) const = 0;
             //@}

  public:
                  //------------------------------------------------------------------------------
                  //
                  /// An iterator for a generic hash table.
                  ///
                  /// This concrete (interior) class intended to serve the generic hash table
                  /// class. Using such an iterator the caller can navigate though a generic
                  /// hash table.
                  //
                  //------------------------------------------------------------------------------

                  class Iterator : public GenericObject
                  {
                    public:
                               /// @nosubgrouping
                               /// @name Constructors
                               //@{
                                                             Iterator ( RawHashTable& forTable );
                                                             Iterator ( const Iterator& given );
                               //@}
                               /// @name Destructor
                               //@{
                                    virtual                 ~Iterator ( );
                               //@}
                               /// @name Methods provided by RawHashTable::Iterator
                               //@{
                                    int                      check ( ) const;
                                    virtual const char*      debugName ( ) const;
                                    int                      direction ( ) const;
                                    int                      first ( );
                                    int                      isFirst ( ) const;
                                    int                      isLast ( ) const;
                                    int                      isNotValid ( ) const;
                                    int                      isValid ( ) const;
                                    const void*              key ( ) const;
                                    int                      last ( );
                                    int                      next ( );
                                    int                      previous ( );
                                    int                      remove ( );
                                    virtual const char*      typeName ( ) const;
                                    void*                    value ( ) const;
                                    Iterator&                operator= ( const Iterator& rhs );
                                    int                      operator== ( const Iterator& rhs ) const;
                                    int                      operator!= ( const Iterator& rhs ) const;
                               //@}
                               /// @name Functions provided by RawHashTable::Iterator
                               //@{
                                    static const char*       debugNameOf ( const Iterator* i );
                               //@}

                    private:
                                                             Iterator ( );
                                                           //Iterator ( RawHashTable&, Pair* );

                                    RawHashTable&            m_whichTable;
                                    Pair*                    m_whichPair;
                                    int                      m_direction;

                    friend class Pair;
                    friend class RawHashTable;
                  };

                  //------------------------------------------------------------------------------
                  //
                  /// A sorted iterator for a generic hash table.
                  ///
                  /// This concrete (interior) class intended to serve the generic hash table
                  /// class. Using such a sorted iterator the caller can navigate though a
                  /// generic hash table. Unlike a "normal" hash table iterator, when created
                  /// a sorted iterator takes a "snapshot" of the contents of the hash table
                  /// and sorts it. It then permits the client to traverse this sorted list of
                  /// pairs. While not intended for public use, it has proven useful in the past.
                  //
                  //------------------------------------------------------------------------------

                  class SortedIterator : public GenericObject
                  {
                    public:
                               /// @nosubgrouping
                               /// @name Constructors
                               //@{
                                                             SortedIterator ( RawHashTable& forTable );
                                                             SortedIterator ( const SortedIterator& given );
                               //@}
                               /// @name Destructor
                               //@{
                                    virtual                 ~SortedIterator ( );
                               //@}
                               /// @name Methods provided by RawHashTable::SortedIterator
                               //@{
                                    int                      check ( ) const;
                                    virtual const char*      debugName ( ) const;
                                    int                      first ( );
                                    int                      isNotValid ( ) const;
                                    int                      isValid ( ) const;
                                    const void*              key ( ) const;
                                    int                      last ( );
                                    int                      next ( );
                                    int                      previous ( );
                                    int                      remove ( );
                                    void                     reset ( );
                                    virtual const char*      typeName ( ) const;
                                    void*                    value ( ) const;
                                    SortedIterator&          operator= ( const SortedIterator& rhs );
                                    int                      operator== ( const Iterator& rhs ) const;
                                    int                      operator!= ( const Iterator& rhs ) const;
                               //@}
                               /// @name Functions provided by RawHashTable::SortedIterator
                               //@{
                                    static const char*       debugNameOf ( const SortedIterator* i );
                               //@}

                    private:
                                                             SortedIterator ( );
                                                           //SortedIterator ( RawHashTable&, Pair* );

                                    void                     sortPairs ( );
                                    void                     sortPairs ( int, int );

                                    RawHashTable&            m_whichTable;
                                    int                      m_whichPair;
                                    Pair**                   m_pairList;
                                    int                      m_pairListLength;

                    friend class Pair;
                    friend class RawHashTable;
                  };

  private:
                  //------------------------------------------------------------------------------
                  //
                  /// A key-value pair.
                  ///
                  /// This concrete (interior) class is used to represent a key-value pair
                  /// in the hash table. These private (hidden) objects link together to
                  /// form the chains. Each pair carries a private copy of the key string and
                  /// the pointer to the value. The key is owned by the table but the value
                  /// is owned by the caller.
                  //
                  //------------------------------------------------------------------------------

                  class Pair : public GenericObject
                  {
                    public:
                               /// @nosubgrouping
                               /// @name Constructors
                               //@{
                                                            Pair ( const void* key, void* value, RawHashTable&, int, Pair* );
                               //@}
                               /// @name Destructor
                               //@{
                                    virtual                ~Pair ( );
                               //@}
                               /// @name Methods provided by RawHashTable::Pair
                               //@{
                                    virtual const char*     debugName ( ) const;
                                    const void*             keyOf ( ) const;
                                    virtual const char*     typeName ( ) const;
                               //@}
                               /// @name Functions provided by RawHashTable::Pair
                               //@{
                                    static const char*      debugNameOf ( const Pair* p );
                               //@}

                    private:
                                    void                    destroy ( );

                                    const void*             m_keyPointer;
                                    void*                   m_valuePointer;
                                    RawHashTable&           m_whichTable;
                                    int                     m_hashValue;
                                    Pair*                   m_prevInChain;
                                    Pair*                   m_nextInChain;
                                    Pair*                   m_prevInTable;
                                    Pair*                   m_nextInTable;
                                    int                     m_markedForDeletion;
                                    int                     m_refCount;

                                                            Pair ( );
                                                            Pair ( const Pair& );
                                    Pair&                   operator= ( const Pair& );

                    friend class RawHashTable;
                    friend class RawHashTable::Iterator;
                    friend class RawHashTable::SortedIterator;
                  };

                  //------------------------------------------------------------------------------
                  //
                  /// A key-pair set.
                  ///
                  /// This private concrete (interior) structure is used in support of the
                  /// sorted iterator. The idea is that an array of these key-pair sets is
                  /// formed by traversing the hash table. Then the array is sorted using the
                  /// key after which the hash table's pair traversal chain is reordered to
                  /// match.
                  //
                  //------------------------------------------------------------------------------

                  struct KeyPairSetTag
                  {
                     const void* key;
                     const void* pair;
                  };

                                          RawHashTable ( const RawHashTable& );
                                          RawHashTable ( const RawHashTable* );
                  RawHashTable&           operator= ( const RawHashTable& );

                  void                    grow ( );
                  Pair*                   pairWithKey ( const void* key, int* Pslot = 0 ) const;
                  void                    quick ( KeyPairSetTag* keyPairSets, int a, int b ) const;
                  void                    setup ( int sz );
                  int                     setupChains ( );
                  void                    sortKeyPairSets ( int N, KeyPairSetTag* keyPairSets ) const;
                  void                    unsetupChains ( );

                  static void             defaultErrorHandler ( const char* msg, void* callerData );

                  int                     m_vecSize;
                  int*                    m_countVec;
                  Pair**                  m_firstInChain;
                  Pair**                  m_lastInChain;
                  Pair*                   m_firstInTable;
                  Pair*                   m_lastInTable;
                  int                     m_numberOfMarkedPairs;
                  int                     m_numberOfPairs;
                  int                     m_numberOfIterators;
                  int                     m_numberOfSortedIterators;
                  int                     m_markedForDeletion;
                  void                  (*m_removeHandler)(const void* key, void* value);

                  static const int        m_numberOfPrimes;
                  static const int        m_prime[28];
                  static void           (*m_errorHandler)(const char* msg, void* callerData);
                  static void*            m_callerData;
};


// **************************************

class GenericHashTable : public RawHashTable
{
  public:
           class Iterator;
           class SortedIterator;

  friend class Iterator;
  friend class SortedIterator;

  public:
             /// @nosubgrouping
             /// @name Constructors
             //@{
                                          GenericHashTable ( );
                                          GenericHashTable ( int sz );
                                          GenericHashTable ( void (*f)(const char*,void*) );
                                          GenericHashTable ( int sz, void (*f)(const char*,void*) );
             //@}
             /// @name Destructor
             //@{
                  virtual                ~GenericHashTable ( );
             //@}
             /// @name Methods supplied by GenericHashTable
             //@{
                  virtual const char*     debugName ( ) const;
                  int                     doesNotHaveKey ( const char* key ) const;
                  int                     hash ( const char* key ) const;
                  int                     hasKey ( const char* key ) const;
                  int                     insert ( const char* key, void* value );
                  int                     remove ( const char* key );
                  virtual const char*     typeName ( ) const;
                  void*                   valueOf ( const char* key ) const;
             //@}
             /// @name Functions supplied by GenericHashTable
             //@{
                  static const char*      debugNameOf ( const GenericHashTable* h );
                  static void             setErrorCallerData ( void* callerData );
                  static void             setErrorHandler ( void (*f)(const char*,void*) );
                  static void             setErrorHandler ( void (*f)(const char*,void*), void* callerData );
             //@}

  protected:
             /// @name Methods needed to make this a concrete class.
             //@{
                  int                     compareKeys ( const void* a, const void* b ) const;
                  void*                   copyKey ( const void* key ) const;
                  void*                   deleteKey ( void* key ) const;
                  int                     hashKey ( const void* key ) const;
                  const char*             keyName ( const void* key ) const;
                  int                     validKey ( const void* key ) const;
             //@}

  public:
                  //------------------------------------------------------------------------------
                  //
                  /// An iterator for a generic hash table.
                  ///
                  /// This concrete (interior) class intended to serve the generic hash table
                  /// class. Using such an iterator the caller can navigate though a generic
                  /// hash table. As expected this iterator provides no additional state vs.
                  /// RawHashTable::Iterator but simply smooths-over type differences.
                  //
                  //------------------------------------------------------------------------------

                  class Iterator : public RawHashTable::Iterator
                  {
                    public:
                               /// @nosubgrouping
                               /// @name Constructors
                               //@{
                                                             Iterator ( GenericHashTable& forTable );
                                                             Iterator ( const Iterator& given );
                               //@}
                               /// @name Destructor
                               //@{
                                    virtual                 ~Iterator ( );
                               //@}
                               /// @name Methods provided by GenericHashTable::Iterator
                               //@{
                                    virtual const char*      debugName ( ) const;
                                    const char*              key ( ) const;
                                    virtual const char*      typeName ( ) const;
                               //@}
                               /// @name Functions provided by GenericHashTable::Iterator
                               //@{
                                    static const char*       debugNameOf ( const Iterator* i );
                               //@}

                    private:
                                                             Iterator ( );

                    friend class GenericHashTable;
                  };

                  //------------------------------------------------------------------------------
                  //
                  /// A sorted iterator for a generic hash table.
                  ///
                  /// This concrete (interior) class intended to serve the generic hash table
                  /// class. Using such a sorted iterator the caller can navigate though a
                  /// generic hash table. Unlike a "normal" hash table iterator, when created a
                  /// sorted iterator takes a "snapshot" of the contents of the hash table and
                  /// sorts it. It then permits the client to traverse this sorted list of pairs.
                  /// While not intended for public use, it has proven useful in the past. As
                  /// expected this iterator provides no additional state vs. the RawHashTable::SortedIterator
                  /// but simply smooths-over type differences.
                  //
                  //------------------------------------------------------------------------------

                  class SortedIterator : public RawHashTable::SortedIterator
                  {
                    public:
                               /// @nosubgrouping
                               /// @name Constructors
                               //@{
                                                             SortedIterator ( GenericHashTable& forTable );
                                                             SortedIterator ( const SortedIterator& given );
                               //@}
                               /// @name Destructor
                               //@{
                                    virtual                 ~SortedIterator ( );
                               //@}
                               /// @name Methods provided by GenericHashTable::SortedIterator
                               //@{
                                    virtual const char*      debugName ( ) const;
                                    const char*              key ( ) const;
                                    virtual const char*      typeName ( ) const;
                               //@}
                               /// @name Functions provided by GenericHashTable::SortedIterator
                               //@{
                                    static const char*       debugNameOf ( const SortedIterator* i );
                               //@}

                    private:
                                                             SortedIterator ( );

                    friend class GenericHashTable;
                  };

  private:

                                          GenericHashTable ( const GenericHashTable& );
                                          GenericHashTable ( const GenericHashTable* );
                  GenericHashTable&       operator= ( const GenericHashTable& );

                  static void             detectedError ( const char* msg );

                  static void           (*m_errorHandler)(const char*,void*);
                  static void*            m_callerData;
};



// *********************************************



typedef long globalID ;

//------------------------------------------------------------------------------
//
// Some private stuff...
//
//------------------------------------------------------------------------------

struct SurfaceFacetTag
{
   globalID Gnodes[4];
   globalID Gelem;
   int Lnodes[4];
   int Lelem;
};

typedef struct SurfaceFacetTag  SurfaceFacetPkt;
typedef struct SurfaceFacetTag *SurfaceFacet;

//-------------------------------------------------------------------------- - -
//
/// @class   SurfaceFacetHashTable
///
/// This concrete class is derived from the GenericHashTable class. It simply
/// provides the type checking of values inserted into and extracted from
/// a untyped GenericHashTable. Almost all of the real functionality is
/// provided by the GenericHashTable class.
///
/// @author Jim Reus
///
/// @date 22Oct2007
//
//-------------------------------------------------------------------------- - -

class SurfaceFacetHashTable : public GenericHashTable
{
  public:
           class Iterator;

  friend class Iterator;

  public:
                                          SurfaceFacetHashTable ( int Ndim );
                                          SurfaceFacetHashTable ( int Ndim, int sz );
                  virtual                ~SurfaceFacetHashTable ( );
                  int                     dim ( ) const;
                  int                     insert ( const char* key, SurfaceFacetPkt* value );
                  static long             numberOf ( );
                  void                    processLocalFacet2D ( globalID Gnodes[2]
                                                              , globalID Gelem
                                                              , int Lnodes[2]
                                                              , int Lelem
                                                              );
                  void                    processLocalFacet3D ( globalID Gnodes[4]
                                                              , globalID Gelem
                                                              , int Lnodes[4]
                                                              , int Lelem
                                                              );
                  void                    processOtherFacet2D ( globalID Gnodes[2] );
                  void                    processOtherFacet3D ( globalID Gnodes[4] );
                  int                     removeFacet2D ( globalID Gnodes[2] );
                  int                     removeFacet3D ( globalID Gnodes[4] );
                  const char*             typeName ( ) const;
                  SurfaceFacetPkt*        valueOf ( const char* key ) const;

                  ///
                  /// An iterator over SurfaceFacetHashTables.
                  ///
                  class Iterator : public GenericHashTable::Iterator
                  {
                    public:
                                                             Iterator ( SurfaceFacetHashTable& theTable );
                                                             Iterator ( const Iterator& given );
                                    virtual                 ~Iterator ( );
                                    const char*              typeName ( ) const;
                                    SurfaceFacetPkt*         value ( ) const;
                                    static long              numberOf ( );
                                    Iterator&                operator = ( const Iterator& rhs );

                    private:
                                    ///
                                    /// Default constructor should not be called.
                                    ///
                                                             Iterator ( );

                                    static long              m_objCount;

                    friend class SurfaceFacetHashTable;
                  };

  private:

                                          SurfaceFacetHashTable ( );
                                          SurfaceFacetHashTable ( const SurfaceFacetHashTable& );
                                          SurfaceFacetHashTable ( const SurfaceFacetHashTable* );
                  SurfaceFacetHashTable&  operator= ( const SurfaceFacetHashTable& );

                  int                     m_Ndims;
                  static long             m_objCount;
};


// Bug: base class is unparsed as:  
//      Iterator( *((class GenericHashTable *)(&theTable)))
// and should have been unparsed with name qualification as: 
//      GenericHashTable::Iterator(*((GenericHashTable*)(&theTable)))
SurfaceFacetHashTable::Iterator::Iterator ( SurfaceFacetHashTable& theTable ) :
            GenericHashTable::Iterator(*((GenericHashTable*)(&theTable)))
{
   m_objCount += 1;
}

#if 0
SurfaceFacetHashTable::Iterator::Iterator ( const SurfaceFacetHashTable::Iterator& given ) :
             GenericHashTable::Iterator(*((GenericHashTable::Iterator*)(&given)))
{
   m_objCount += 1;
}
#endif


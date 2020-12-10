
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


inline int GenericObject::id ( ) const
{
   return m_id;
}

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
                  static void             detectedError ( const char* msg );

                  virtual int             compareKeys ( const void* a, const void* b ) const = 0;
                  virtual void*           copyKey ( const void* key ) const = 0;
                  virtual void*           deleteKey ( void* key ) const = 0;
                  virtual int             hashKey ( const void* key ) const = 0;
                  virtual const char*     keyName ( const void* key ) const = 0;
                  virtual int             validKey ( const void* key ) const = 0;

  public:
                  class Iterator : public GenericObject
                  {
                    public:
                                                             Iterator ( RawHashTable& forTable );
                                                             Iterator ( const Iterator& given );
                                    virtual                 ~Iterator ( );
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
                                    static const char*       debugNameOf ( const Iterator* i );

                    private:
                                                             Iterator ( );
                                                           //Iterator ( RawHashTable&, Pair* );

                                    RawHashTable&            m_whichTable;
                                    Pair*                    m_whichPair;
                                    int                      m_direction;

                    friend class Pair;
                    friend class RawHashTable;
                  };

                  class SortedIterator : public GenericObject
                  {
                    public:
                                                             SortedIterator ( RawHashTable& forTable );
                                                             SortedIterator ( const SortedIterator& given );
                                    virtual                 ~SortedIterator ( );
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
                                    static const char*       debugNameOf ( const SortedIterator* i );

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
                  class Pair : public GenericObject
                  {
                    public:
                                                            Pair ( const void* key, void* value, RawHashTable&, int, Pair* );
                                    virtual                ~Pair ( );
                                    virtual const char*     debugName ( ) const;
                                    const void*             keyOf ( ) const;
                                    virtual const char*     typeName ( ) const;
                                    static const char*      debugNameOf ( const Pair* p );

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


class GenericHashTable : public RawHashTable
{
  public:
           class Iterator;
           class SortedIterator;

  friend class Iterator;
  friend class SortedIterator;

  public:
                                          GenericHashTable ( );
                                          GenericHashTable ( int sz );
                                          GenericHashTable ( void (*f)(const char*,void*) );
                                          GenericHashTable ( int sz, void (*f)(const char*,void*) );
                  virtual                ~GenericHashTable ( );
                  virtual const char*     debugName ( ) const;
                  int                     doesNotHaveKey ( const char* key ) const;
                  int                     hash ( const char* key ) const;
                  int                     hasKey ( const char* key ) const;
                  int                     insert ( const char* key, void* value );
                  int                     remove ( const char* key );
                  virtual const char*     typeName ( ) const;
                  void*                   valueOf ( const char* key ) const;
                  static const char*      debugNameOf ( const GenericHashTable* h );
                  static void             setErrorCallerData ( void* callerData );
                  static void             setErrorHandler ( void (*f)(const char*,void*) );
                  static void             setErrorHandler ( void (*f)(const char*,void*), void* callerData );

  protected:
                  int                     compareKeys ( const void* a, const void* b ) const;
                  void*                   copyKey ( const void* key ) const;
                  void*                   deleteKey ( void* key ) const;
                  int                     hashKey ( const void* key ) const;
                  const char*             keyName ( const void* key ) const;
                  int                     validKey ( const void* key ) const;

  public:
                  class Iterator : public RawHashTable::Iterator
                  {
                    public:
                                                             Iterator ( GenericHashTable& forTable );
                                                             Iterator ( const Iterator& given );
                                    virtual                 ~Iterator ( );
                                    virtual const char*      debugName ( ) const;
                                    const char*              key ( ) const;
                                    virtual const char*      typeName ( ) const;
                                    static const char*       debugNameOf ( const Iterator* i );

                    private:
                                                             Iterator ( );

                    friend class GenericHashTable;
                  };

                  class SortedIterator : public RawHashTable::SortedIterator
                  {
                    public:
                                                             SortedIterator ( GenericHashTable& forTable );
                                                             SortedIterator ( const SortedIterator& given );
                                    virtual                 ~SortedIterator ( );
                                    virtual const char*      debugName ( ) const;
                                    const char*              key ( ) const;
                                    virtual const char*      typeName ( ) const;
                                    static const char*       debugNameOf ( const SortedIterator* i );

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



typedef long globalID ;

struct SurfaceFacetTag
{
   globalID Gnodes[4];
   globalID Gelem;
   int Lnodes[4];
   int Lelem;
};

typedef struct SurfaceFacetTag  SurfaceFacetPkt;
typedef struct SurfaceFacetTag *SurfaceFacet;

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


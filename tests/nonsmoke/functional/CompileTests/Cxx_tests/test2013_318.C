/// pair holds two objects of arbitrary type.
template<class _T1, class _T2>
struct pair
   {
     typedef _T1 first_type;    ///<  @c first_type is the first bound type
     typedef _T2 second_type;   ///<  @c second_type is the second bound type

     _T1 first;                 ///< @c first is a copy of the first object
     _T2 second;                ///< @c second is a copy of the second object

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 265.  std::pair::pair() effects overly restrictive
      /** The default constructor creates @c first and @c second using their
       *  respective default constructors.  */
     pair() : first(), second() { }

      /** Two objects may be passed to a @c pair constructor to be copied.  */
     pair(const _T1& __a, const _T2& __b) : first(__a), second(__b) { }

      /** There is also a templated copy ctor for the @c pair class itself.  */
      template<class _U1, class _U2> pair(const pair<_U1, _U2>& __p) : first(__p.first), second(__p.second) { }
    };


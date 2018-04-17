
template<class _T1, class _T2>
struct pair
   {
  // typedef _T1 first_type;    ///<  @c first_type is the first bound type
  // typedef _T2 second_type;   ///<  @c second_type is the second bound type

     _T1 first;                 ///< @c first is a copy of the first object
  // _T2 second;                ///< @c second is a copy of the second object

  // pair() : first(), second() { }
  // pair(const _T1& __a, const _T2& __b) : first(__a), second(__b) { }

     template<class _U1, class _U2> pair(const pair<_U1, _U2>& __p) : first(__p.first) /* , second(__p.second) */ { }
   };

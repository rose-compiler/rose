template<class _T1, class _T2>
struct pair
   {
     typedef _T1 first_type;
     typedef _T2 second_type;

     _T1 first;
     _T2 second;
   };

template<class _T1, class _T2>
inline bool operator==(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) { return true; };

//  { return __x.first == __y.first && __x.second == __y.second; }

  /// <http://gcc.gnu.org/onlinedocs/libstdc++/20_util/howto.html#pairlt>
template<class _T1, class _T2>
inline bool operator<(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) { return true; };

//  { return __x.first < __y.first || (!(__y.first < __x.first) && __x.second < __y.second); }


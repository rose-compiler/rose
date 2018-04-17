
#if 1

   #include <vector>
// #include <pair>

#else

#include <initializer_list>
namespace std
   {
  // class string { public: string(const char* s); };
     template <typename T1, typename T2> class pair { public: pair(T1 a, T2 b); };
  // template<typename _Tp> struct vector { public: vector(const _Tp & x); };
     template<typename _Tp> struct vector { public: vector(std::initializer_list<int> list); };

  // template <class Key, class T> class map { public: map(const pair<Key,T> & x); };
   }

#endif

// std::vector<std::pair<int,double>> abc = { {42, 42.0} };
// std::vector<std::pair<int,int>> abc = { {42, 42} };
std::vector<std::pair<int,int>> abc = { {42, 43}, {52, 53} };

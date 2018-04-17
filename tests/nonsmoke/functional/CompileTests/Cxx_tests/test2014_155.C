// #include <set>

namespace std {
template < typename T, typename S >
class set
   {
     public:
          set(S);
   };
}


bool compareSolutions(int x);

void foobar()
   {
 // Simplist failing case.
    ::std::set<int, bool(*)(int)> XXX_triedSolutions(compareSolutions);

  // Works fine.
  // ::std::set<int, bool(*)(int)> XXX_triedSolutions();
   }


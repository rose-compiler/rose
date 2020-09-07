// RC-107
// A fix for this is available, but it forces test2020_94.C to fail
// (which is a copy of Cxx_tests/test2018_51.C).  More specifically,
// about seven of the tests in Cxx_tests directory fail when using 
// this fix, all for seemingly similar reasons, and so we need a 
// better solution.  The fix for this test code is in edgRose.C
// and is marked as FIX_FOR_RC_107 in the source code.

// DQ (9/6/2020):
// ROSE currently has intermediate work on RC-107 (assertion in buildSpeculativeFunctionRefExp()).

// More details:
// An assertion error is triggered after calling the getDeclarationScope() 
// function (in edgRose.c).  An issue appears to be that EDG has two scope for 
// something previously processed and the scope_cache that is maintained can 
// not be used to lookup the ROSE scope associated with the dsecond EDG scope 
// (that was not previously processed).  In these cases we force the 
// processing of the previously unseen scope (specifically the type that 
// that scope represents).  In test2020_92.C this causes a second version 
// of a function to be built.  The fix works great and uses the ROSE AST 
// to resolve scopes that have not previoously been seen within EDG,
// however this fix causes test2020_94.C to fail. At this point I am
// checking in intermediate work on RC-107 so that Tristand and I might 
// be able to work on it together, and I can move on to other work.
// See Cxx11_tests/test2020_91.C an examples of this.  Also  test2020_92.C, 
// and test2020_93.C demonstrate simplare examples of this bug that pass, 
// but for which the AST has an additional template function declaration.

class Arena 
   {
      template <typename Key, typename T>
      friend class Map;
   };

template <typename Key, typename T>
class MapPair {};

template <typename Key, typename T>
struct Map 
   {
     typedef MapPair<Key, T> * pointer;

     template <typename U>
     struct MapAllocator {};

     class KeyValuePair {};
     typedef MapAllocator<KeyValuePair> Allocator;

     struct iterator
        {
          int abcdefghijklmnop;
          pointer operator->() const;
        };

     void foo(iterator pos) 
        {
          delete pos.operator->();
        }
   };


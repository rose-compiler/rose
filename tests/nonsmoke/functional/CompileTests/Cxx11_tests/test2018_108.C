// This is reproducer ROSE-38

// Small reproducer (originally 236967 lines):

template <typename... TALL>
class TypedIndexSet;

template <typename T0, typename... TREST>
class TypedIndexSet<T0, TREST...> {
public:
  template <typename BODY, typename... ARGS>
  void segmentCall(BODY &&body) const  {}
};

struct Struct1 {};

void func1() {
  TypedIndexSet< int, int >* local2;
  local2->segmentCall(Struct1{});
}

 
#if 0
// gets translated into:

template < typename ... TALL >
class TypedIndexSet;

template < typename T0, typename ... TREST >
class TypedIndexSet < T0, TREST ... > {
public :
  template < typename BODY, typename ... ARGS >
  void segmentCall ( BODY && body ) const { }
};

struct Struct1
{
};


void func1()
{
  class TypedIndexSet< int  , int  > *local2;
  local2 -> segmentCall({});
}
#endif

 
#if 0
// which gets error message:
rose_ROSE-38.cc(18): error: no instance of function template "TypedIndexSet<T0, TREST...>::segmentCall [with T0=int, TREST=<int>]" matches the argument list
            argument types are: ({...})
            object type is: TypedIndexSet<int, int>
    local2 -> segmentCall({});
              ^
#endif
 

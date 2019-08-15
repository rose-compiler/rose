// This is reproducer ROSE-38

// Small reproducer (originally 236967 lines):

template <typename... TALL> class TypedIndexSet;

template <typename T0, typename... TREST>
class TypedIndexSet<T0, TREST...> 
   {
     public:
          template <typename BODY, typename... ARGS>
          void segmentCall(BODY &&body) const  {}
   };

struct Struct1 {};

void func1() 
   {
     TypedIndexSet< int, int >* local2;
  // TypedIndexSet< int >* local2;

  // initialization of an unnamed temporary with a braced-init-list
  // Unparsed as: local2 -> segmentCall({});
     local2->segmentCall(Struct1{});
   }


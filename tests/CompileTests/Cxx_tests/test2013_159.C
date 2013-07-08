class string {};

template <typename Element>
class RepeatedPtrField
   {
     public:
         class TypeHandler {};
   };

// This demonstrates a bug.  Fails assertions in build API: templateArgumentsList->size() > 0
template <> class RepeatedPtrField<string>::TypeHandler {};


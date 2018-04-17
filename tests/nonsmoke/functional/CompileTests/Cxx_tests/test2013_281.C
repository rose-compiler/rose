class new_allocator
   {
     public:
//        struct rebind {};
          struct rebind;
   };

#if 1
// The scope of the declaration for "class rebind" in "class allocator" is the class definition of "class new_allocator"
// so the scope represented by the class definition of "class allocator" has a symbol for which the declaration's scope 
// does not match.
class allocator : public new_allocator 
   {
  // struct rebind {};
     struct rebind;
     struct rebind {};
   };
#endif


template<class Key, class Value, class Hash, class Pred, class Allocator>
class hash_map 
   {
  // DQ (8/18/2020): I think this is C++11, or C++14 code that does not wotk with GNU 4.9 using C++11 support.
  // [[no_unique_address]] Hash hasher;
  // [[no_unique_address]] Pred pred;
  // [[no_unique_address]] Allocator alloc;
     Hash hasher;
     Pred pred;
     Allocator alloc;
     int* buckets;
   };


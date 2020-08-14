template<class Key, class Value, class Hash, class Pred, class Allocator>
class hash_map 
   {
     [[no_unique_address]] Hash hasher;
     [[no_unique_address]] Pred pred;
     [[no_unique_address]] Allocator alloc;
     int* buckets;
   };


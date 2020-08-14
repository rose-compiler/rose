// class Allocator {};

template<class Allocator>
class hash_map 
   {
     [[no_unique_address]] Allocator alloc;
   };


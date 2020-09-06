// RC-107
#define SIMPLIFY 1

#if !SIMPLIFY
class Arena 
   {
      template <typename Key, typename T>
      friend class Map;
   };
#endif

template <typename Key, typename T>
class MapPair {};

template <typename Key, typename T>
struct Map 
   {
     typedef MapPair<Key, T> * pointer;

#if !SIMPLIFY
     template <typename U>
     struct MapAllocator {};

     class KeyValuePair {};
     typedef MapAllocator<KeyValuePair> Allocator;
#endif

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


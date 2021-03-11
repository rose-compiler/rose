// RC-107 (simplified)

template <typename Key, typename T>
class MapPair {};

template <typename Key, typename T>
struct Map 
   {
     typedef MapPair<Key, T> * pointer;

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


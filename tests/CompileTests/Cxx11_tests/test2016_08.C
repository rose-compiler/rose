
typedef long unsigned int size_t;

namespace std 
   {
     template<class _E>
     class initializer_list
        {
          public:
               typedef const _E* const_iterator;
          private:
            // constexpr initializer_list(const_iterator __a, size_t __l);
               initializer_list(const_iterator __a, size_t __l);
        };
   }

class X
   {
     public:
          X (std::initializer_list<int> list);
   };

X abcdefg = { 42 };


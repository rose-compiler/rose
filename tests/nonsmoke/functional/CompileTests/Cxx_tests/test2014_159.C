namespace std
   {
     template<typename _Tp> class allocator {};

     template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
     class vector
        {
          public:
               vector();
        };
   }

namespace XXX
   {
     void foobar(const int def[3]);

     namespace
        {
          const unsigned size = 3;

          struct YYY {};
          std::vector<YYY*> boundary_data;
        }

      void foobar( const int def[size] ) {}
   }


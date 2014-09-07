#if 1
namespace A
   {
#if 1
     template <typename T> bool operator== (T,T);
#endif
   }
#endif

namespace A
   {
#if 1
     template <typename T> bool operator== (T,int);
#endif

     class X
        {
          public:
            // template <typename T> friend bool operator== (T,T);
        };

#if 1
     template <typename T>
     bool operator== (T,T)
        {
          return false;
        }
#endif
   }


#if 0
namespace A
   {
#if 0
     template <typename T>
     bool operator== (T,T);
#endif
   }
#endif

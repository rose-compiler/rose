#define DEFINE_AS_FRIEND_FUNCTION 1

namespace std
   {
     class X
        {
          public:
#if DEFINE_AS_FRIEND_FUNCTION
               template <typename T> friend bool operator== (X,int);
#else
               template <typename T> bool operator== (int);
#endif
        };
#if DEFINE_AS_FRIEND_FUNCTION
     template <typename T> bool operator== (X,int)
        {
          return false;
        }
#else
     template <typename T> bool X::operator== (int)
        {
          return false;
        }
#endif

   }

#define DEFINE_AS_FRIEND_FUNCTION 1

namespace std
// namespace X___std
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

// This will cause an error in the weaving of CCP directives back into the AST.
#if DEFINE_AS_FRIEND_FUNCTION
     template <typename T> bool operator== (X,int)
#else
     template <typename T> bool X::operator== (int)
#endif
        {
          return false;
        }

   }

// The test code is used to test the source positions at the 
// end of the defining declarations (should match that of the 
// associated end of definitions.

class X;

#if 1
class X
#endif
   {
   };


void foobar();

#if 1
void foobar()
#endif
   {
   }

#define DEFINE_AS_FRIEND_FUNCTION 1

namespace std
{
class Y
   {
     public:
#if DEFINE_AS_FRIEND_FUNCTION
       // template <typename T> friend bool operator== (Y,int);
#else
          template <typename T> bool operator== (int);
#endif
   };

// This will cause an error in the weaving of CCP directives back into the AST.
#if DEFINE_AS_FRIEND_FUNCTION
template <typename T> bool operator== (Y,int)
#else
template <typename T> bool Y::operator== (int)
#endif
   {
     return false;
   }
}


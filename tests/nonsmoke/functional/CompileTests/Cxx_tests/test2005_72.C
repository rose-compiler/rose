// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

template<class U>
class A
   {
     public:
          int x21;

      // templates defined in the class are preserved in the output template declaration
         template<class T> void g(int y, T u) 
            {
              y = y+u;
              x21+=u;
            }

       // non-template member functions are not represented in the templated class 
       // (even though we have specified FRIEND_AND_MEMBER_DEFINITIONS_MAY_BE_MOVED_OUT_OF_CLASS 
       // to be FALSE in EDG).
          void f(int x)
             {
               x++;
             }
          int x22;
   };

int main()
   {
     A<int> a;
     int x = 42;
     a.f(x);
     a.g<int>(x,x);
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif


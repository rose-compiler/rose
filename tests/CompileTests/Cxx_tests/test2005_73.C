template<class U>
class A
   {
     public:
       // U xyz;
          int x21;

      // templates defined in the class are preserved in the output template declaration
         template<class T> void g(int y, T u) 
            {
              y   = y + u;
              x21 += u;
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

#if 1

// Template instantiation directive (for a class)
#if (__GNUC__ == 3)
#if (__GNUC_MINOR__ == 4)
// TOO (2/15/2011): template A<int>; is error for Thrifty (gcc 3.4.4)
template class A<int>;
#else
template A<int>;
#endif
#else
// DQ (2/20/2010): this is a error for g++ 4.x compilers (at least g++ 4.2).
// template<> A<int>;
#endif

#else
// Template instantiation directive (for a function)
template inline void A<int>::f(int x);
#endif

// Template instantiation directives (for a function)
// template inline void A<int>::g<int>(int y,int u);
// DQ (6/4/2011): This unparses as: "template void g < int > (int y,int u);"
template inline void A<int>::g<int>(int y,int u);

int foo()
   {
     return 0;
   }

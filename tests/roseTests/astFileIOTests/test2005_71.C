template<class U>
class A
   {
     public:
          int x21;

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
   }


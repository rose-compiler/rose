// This is a test code to be used with the default translator built using the make rule "installcheck"
template <class T>
class X
   {
     public:
          T data;
          void foo() 
             {
               X<int> variable;
             }
   };

int main()
   {
     X<int> variable;
     return 0;
   }

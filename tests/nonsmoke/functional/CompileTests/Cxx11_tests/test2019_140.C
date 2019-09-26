class foo
   {
     public:
          void bar_function();
   };

int main()
   {
  // This is a variable declaration for a member function of class foo.
     void (foo::*X)() = &foo::bar_function;
   }



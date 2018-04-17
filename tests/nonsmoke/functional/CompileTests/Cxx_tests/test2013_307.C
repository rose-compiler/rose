class foo
   {
     public:
          int bar_variable;
          void bar_function();
   };

// Examples of function prototype with function paramters 
// similar to how we want to represent template parameters.
void foobar(void (foo::*X)(),int foo::*Y);

template <typename T>
struct X
   {
  // In the AST, this function declaration has 2 arguments, instead of one
     friend void bar( X<T> & i) { }
   };

void foo()
   {
     X<int> y;
     bar(y);
   }


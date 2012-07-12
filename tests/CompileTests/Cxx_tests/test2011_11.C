/*
In the example below, the function bar() has a single formal argument. However, the number of formal arguments present
in the AST is 2, instead of one. The same argument i is repeated twice in a row. This can easily be seen in the AST
graph; also, (barDeclaration.get_args().size() == 2) is true.

This bug seems to be the result of the function declaration for bar() having two parents. The SgFunctionDeclaration
node for bar() is a child of SgGlobal as well as a child of SgTemplateInstantiationDefn. Curiously enough the consistency
tests pass.

-George

incorrectFormalArgumentCount.C
--------------------------
template <typename T>
struct X
{
    //In the AST, this function declaration has 2 arguments, instead of one
    friend void bar( X<T> & i)
    { }
};

void foo()
{
    X<int> y;
    bar(y);
}
--------------------------- 
*/

template <typename T>
struct X
   {
  // In the AST, this function declaration has 2 arguments, instead of one
     friend void bar( X<T> & i)
        { }
   };

void foo()
   {
     X<int> y;
     bar(y);
   }

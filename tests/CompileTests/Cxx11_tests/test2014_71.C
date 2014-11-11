
template <typename T>
struct A 
{
};

template <typename T>
struct B 
{
};

template <typename T>
struct C
{
};


A<B<int>> ab; // works
A<B<C<int>>> abc; // fails

#if 0
"/home/dquinlan/ROSE/git-dq-edg49-rc/tests/CompileTests/Cxx11_tests/test2014_71.C", line 19: error: 
          expected a ">"
  A<B<C<int>>> abc; // fails
           ^

"/home/dquinlan/ROSE/git-dq-edg49-rc/tests/CompileTests/Cxx11_tests/test2014_71.C", line 19: error: 
          expected a ">"
  A<B<C<int>>> abc; // fails
                  ^

"/home/dquinlan/ROSE/git-dq-edg49-rc/tests/CompileTests/Cxx11_tests/test2014_71.C", line 19: error: 
          expected a ">"
  A<B<C<int>>> abc; // fails
                  ^

DONE: frontend called (frontendErrorLevel = 2) 
frontendErrorLevel = 2 
terminate called after throwing an instance of 'rose_exception'
  what():  Errors in Processing: (frontend_failed)
Aborted (core dumped)
#endif

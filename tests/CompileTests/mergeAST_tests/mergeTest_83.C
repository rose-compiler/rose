// Example of bug using 2 namespaces with a declaration of the same function in each
namespace std
{
  void foo(void);
}

namespace std
{
  void foo(void);
}



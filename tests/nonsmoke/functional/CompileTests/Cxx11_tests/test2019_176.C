#include <typeinfo>

int n_;
void f_() { ++n_; }
auto g_() -> decltype( f_() ) { return f_(); }

void foobar()
   {
  // typeid(g_());
   }

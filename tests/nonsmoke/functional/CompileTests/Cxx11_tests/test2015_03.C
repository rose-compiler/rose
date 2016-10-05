#include <iostream>
#include <utility>
#include <vector>
 
// void may_throw();
// void no_throw() noexcept;

// auto lmay_throw = []{};

// auto lno_throw = []() noexcept {};

#if 0
class T{
public:
  ~T(){} // dtor prevents move ctor
         // copy ctor is noexcept
};
class U{
public:
  ~U(){} // dtor prevents move ctor
         // copy ctor is noexcept(false)
  std::vector<int> v;
};
class V{
public:
  std::vector<int> v;
};
 
int main()
{
 T t;
 U u;
 V v;
}
#endif

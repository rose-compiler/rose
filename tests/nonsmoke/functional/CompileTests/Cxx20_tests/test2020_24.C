#include <iostream>
 
auto make_function(int& x) {
  return [&]{ std::cout << x << '\n'; };
}
 
int main() {
  int i = 3;
  auto f = make_function(i); // the use of x in f binds directly to i
  i = 5;
  f(); // OK; prints 5
}



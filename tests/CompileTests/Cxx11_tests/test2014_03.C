#include <iostream>
#include <initializer_list>

using namespace std;

class R {
  public:
    int v;
    // R(std::initializer_list<int> l) {}
    R() {v = 3;}
};

int main(int argc, char* argv[]) {
  
  auto a = R();

  cout << a.v << endl;
  return 0;
}

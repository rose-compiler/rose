
// Example of C++ initializer use from: http://en.cppreference.com/w/cpp/language/list_initialization

// Added required include file.
#include <initializer_list>

struct X {
    X() = default;
    X(const X&) = default;
};


struct Q {
    Q() = default;
    Q(Q const&) = default;
    Q(std::initializer_list<Q>) {}
};
 
int main() {
  X x;
  X x2 = X { x }; // copy-constructor (not aggregate initialization)
  Q q;
  Q q2 = Q { q }; // initializer-list constructor (not copy constructor)
}


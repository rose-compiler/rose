// Modernizing using-declarations

// This is required.
#include <utility>

// setprecision example
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision

template <typename... T>
struct Overloader : T... /* […] */ {
    // […]
};

template <typename... T>
constexpr auto make_overloader(T&&... t) {
  return Overloader<T...>{std::forward<T>(t) ... };
}

int main() {
    auto o = make_overloader([] (auto const& a) {std::cout << a;},
                             [] (float f) {std::cout << std::setprecision(3) << f;});
}



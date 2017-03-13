template<typename T>
class A {
          T component;
        };

// This is called an alias template (C++11 specific).
template<typename V>
using Alias = A<V>;


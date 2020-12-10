struct s2 {
  double ohseven = .007;
  auto f() { // nearest enclosing function for the following two lambdas
    return [this] { // capture the enclosing s2 by reference
      return [*this] { // capture the enclosing s2 by copy (C++17)
         return ohseven; // OK
       }
     }();
  }
  auto g() {
     return []{ // capture nothing
         return [*this]{}; // error: *this not captured by outer lambda-expression
      }();
   }
};



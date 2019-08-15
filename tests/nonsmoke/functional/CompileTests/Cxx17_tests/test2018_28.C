// Lambda Capture of *this by Value as [=,*this]

struct s2 {
     double ohseven = .007;
     auto f()
        {
          return [this]{
               return [*this]{
                    return ohseven; // OK
                    };
               }();
        }

auto g() {
     return []{
       // return [*this]{}; // error: *this not captured by outer lambda-expression
          }();
     }
};


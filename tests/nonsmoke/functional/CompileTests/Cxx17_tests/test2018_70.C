// Aggregate initialization with inheritance: Extension to aggregate initialization

struct base { int a1, a2; };
struct derived : base { int b1; };

derived d1{{1, 2}, 3};      // full explicit initialization
derived d2{{}, 1};          // the base is value initialized


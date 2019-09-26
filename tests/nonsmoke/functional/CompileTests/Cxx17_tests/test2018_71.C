// Aggregate initialization with inheritance: Extension to aggregate initialization

struct base1 { int b1, b2 = 42; };
struct base2 { 

 // Typo in original example code?
 // B() {
    base2() {
        b3 = 42;
    }
    int b3;
};
struct derived : base1, base2 {
    int d;
};

derived d1{{1, 2}, {}, 4};  // full initialization
derived d2{{}, {}, 4};      // value-initialized bases

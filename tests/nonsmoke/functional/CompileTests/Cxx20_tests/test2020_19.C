struct S2 { void f(int i); };

void S2::f(int i)
{
    [&]{};          // OK: by-reference capture default
    [&, i]{};       // OK: by-reference capture, except i is captured by copy

 // DQ (7/21/2020): Failing case should not be tested
 // [&, &i] {};     // Error: by-reference capture when by-reference is the default

    [&, this] {};   // OK, equivalent to [&]
    [&, this, i]{}; // OK, equivalent to [&, i]
}



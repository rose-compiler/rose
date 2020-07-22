void f3() {
    float x, &r = x;
    [=]
    { // x and r are not captured (appearance in a decltype operand is not an odr-use)
        decltype(x) y1; // y1 has type float
        decltype((x)) y2 = y1; // y2 has type float const& because this lambda
                               // is not mutable and x is an lvalue
        decltype(r) r1 = y1;   // r1 has type float& (transformation not considered)

     // DQ (7/21/2020): This appears to be an error in EDG 6.0.
        decltype((r)) r2 = y2; // r2 has type float const&
    };
}



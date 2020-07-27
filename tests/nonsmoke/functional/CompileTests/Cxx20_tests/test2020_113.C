
int x = 4;

// DQ (7/21/2020): I think this can can be in global scope (test2020_21.C tests it in a function).
auto y = [&r = x, x = x + 1]()->int
    {
        r += 2;
        return x * x;
    }(); // updates ::x to 6 and initializes y to 25.

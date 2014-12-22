// declaring_lambda_expressions1.cpp
// compile with: /EHsc /W4
// #include <functional>
#include <iostream>

void foo()
{
#if 0
    using namespace std;

    // Assign the lambda expression that adds two numbers to an auto variable.
    auto f1 = [](int x, int y) { return x + y; };

//  cout << f1(2, 3) << endl;

    // Assign the same lambda expression to a function object.
    function<int(int, int)> f2 = [](int x, int y) { return x + y; };

//  cout << f2(3, 4) << endl;
#endif
}

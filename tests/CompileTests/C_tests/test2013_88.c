// Test a pragma (Beata's bug report)

#pragma pragma test 7;

#pragma pragma test 8
int foo()
// #pragma pragma test 9
   {
#pragma pragma test 10
#pragma pragma test 11
     int x = 42;
#pragma pragma test 19
// #pragma pragma test 34
     return x;
#pragma pragma test 35
   }
#pragma pragma test 36
#pragma pragma test 36


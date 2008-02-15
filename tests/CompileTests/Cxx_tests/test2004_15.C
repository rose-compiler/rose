// Test a pragma (Beata's bug report)

#pragma pragma test 1
class ABC
// Pragma not allowed here
#pragma pragma test 2
   {
#pragma pragma test 3
     int a;
#pragma pragma test 4
     int b;
#pragma pragma test 5
   };
#pragma pragma test 6

#pragma pragma test 7;

#pragma pragma test 8
int foo()
#pragma pragma test 9
   {
#pragma pragma test 10
#pragma pragma test 11
     int x = 42;
#pragma pragma test 12
     for (int i=0; i < 10; i++)
// Pragma not allowed here
// #pragma pragma test 13
        {
#pragma pragma test 14
           x = x + 
#pragma pragma test 15
               x + 
#pragma pragma test 16
               x + 
#pragma pragma test 17
               x;
#pragma pragma test 18
        }
#pragma pragma test 19
     switch (x)
// Pragma not allowed here
// #pragma pragma test 20
        {
#pragma pragma test 21
          case 1:
#pragma pragma test 22
               x = 42;
#pragma pragma test 23
               break;
#pragma pragma test 24
          case 2:
#pragma pragma test 25
             {
#pragma pragma test 26
               x = 42;
#pragma pragma test 27
               break;
#pragma pragma test 28
             }
#pragma pragma test 29
          default:
#pragma pragma test 30
             {
#pragma pragma test 31
               x = 42;
#pragma pragma test 32
             }
#pragma pragma test 33
        }
#pragma pragma test 34
     return x;
#pragma pragma test 35
   }
#pragma pragma test 36
#pragma pragma test 36


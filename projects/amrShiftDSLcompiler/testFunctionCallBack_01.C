#include <stdio.h>
#include <functional>

// using cb1_t = std::function<void()>;

int x = 0;
int y = 0;

void foo1() { x += 7; }
void foo2(int i) { y += 42; }

std::function<void()> f1 = std::bind(&foo1);
std::function<void()> f2 = std::bind(&foo2, 5);

int main(int argc, char* argv[])
   {
     f1(); // Invoke foo1()
     f2(); // Invoke foo2(5)

     printf ("x = %d y = %d \n",x,y);

     return 0;
   }

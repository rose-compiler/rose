// using cb1_t = std::function<void()>;

#include <stdio.h>
#include <vector>
#include <functional>

std::vector< std::function<void()> > callbacks;

int x = 0;
int y = 0;

void foo1() { x += 7; }
void foo2(int i) { y += 42; }

int main(int argc, char* argv[])
   {
     std::function<void()> f1 = std::bind(&foo1);
     callbacks.push_back(f1);

     int n = 15;
     std::function<void()> f2 = std::bind(&foo2, std::ref(n));
     callbacks.push_back(f2);

  // Invoke the functions
     for(auto& fun : callbacks)
        {
	  fun();
        }

     printf ("x = %d y = %d \n",x,y);

     return 0;
   }

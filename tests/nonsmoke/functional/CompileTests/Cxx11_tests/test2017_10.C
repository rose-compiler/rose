#include <iostream>
#include <vector>

int main(void)
{
   std::vector<int> vec{3,1,2};

   for(auto v : vec)
     std::cout << v << " ";
   std::cout << std::endl;

   return 0;
}

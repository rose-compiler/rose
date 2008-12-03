//test mixed element access member functions
#include <vector>
int main(void)
{
  int i;
  std::vector <int> v1(100);
  for (i=1; i< v1.size(); i++)
//  for (i=0; i<100; i++)
    v1.at(i) = v1[i] +1; 
  return 0;  
}

// vector constructors
// semantics
// implementation: 
#include <vector>
#include <iostream>

using namespace std;

int main (void)
{
  vector <int> v1; // minimum exe time
  cout<<"Capacity is:"<<v1.capacity()<<endl;
  cout<<"Max size is:"<<v1.max_size()<<endl;
  // is:            1073741823
  //vector <int> v1(500000000,56);  //stmt 2.
   vector <int> v2(673741823); // the same execution time as stmt 2!
  return 0;
}

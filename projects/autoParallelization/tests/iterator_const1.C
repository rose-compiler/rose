// any use for const_iterator?
// how to handle list 
#include <list>
#include <iostream>
using namespace std;

list<int> v2;

int main()
{
  int i;
  //sequential initialization
  for (i=0; i<10; i++)
    v2.push_back(i);
  
  list<int>::const_iterator citer;
  int sum=0;

  cout<<"size of v2 is "<<v2.size()<<endl;
  for (citer=v2.begin(); citer!=v2.end(); citer++)
  {
     sum = sum + *citer;
//     cout<<"sum is "<<sum<<endl;
  }
  cout<<"sum is "<<sum<<endl;
  return 0; 
}

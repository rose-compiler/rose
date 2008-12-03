// omp task candidates
#include <list>
#include <iostream>

int foo(int i) {return i;}

//using std::list;
using namespace std;

int bar(const std::list<int>& slist)
{  
  int sum=0;
  for (std::list<int>::const_iterator i=slist.begin();i!=slist.end();i++)
  {  
   sum = sum + foo(*i);
  }
  return sum;
}
int main()
{
  list<int> L;
  L.push_back(0);
  L.push_front(1);
  L.insert(++L.begin(), 2);
  //copy(L.begin(), L.end(), ostream_iterator<int>(cout, " "));
  // The values that are printed are 1 2 0
  cout<<bar(L)<<endl;; 
} 




// vector<>::iterator can be replaced by address calculation
// 
// for (iter=v1.begin();iter!=v1.end();iter++) -->
//  for (i=0;i<v1.size();i++) 
//
// *iter --> v1[i]
//
// iter[n] -> v1[i+n]
//
#include <vector>
#include <iostream>
#include <iterator>
//extern int rand();
using namespace std;
vector<float> v1(100);
int main()
{
  vector<float>::iterator iter;
  vector<float>::difference_type diff;
  for (iter=v1.begin();iter!=v1.end()-1;iter++)
  {
    *iter = *iter +10;
    //iter[0] = iter[1] +10; // only for random access iterators
    //iter[0] = iter[0] +10;
    //*iter = rand();      
  } 
  diff = distance(v1.begin(),v1.end());
  cout<<"Distance is "<<diff<<endl;
  //cout<<"Distance type is "<<distance_type(v1.begin())<<endl;
  return 0;
}

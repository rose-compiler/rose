#include <map>
#include <vector>
using namespace std;

//C++11 brace-init
int a{0};
string s{"hello"};
string s2{s}; //copy construction
vector <string> vs{"alpha", "beta", "gamma"};
map<string, string> stars
 { {"Superman", "+1 (212) 545-7890"},
  {"Batman", "+1 (212) 545-0987"}};
double *pd= new double [3] {0.5, 1.2, 12.99};
class C
{
int x[4];
public:
C(): x{0,1,2,3} {}
};


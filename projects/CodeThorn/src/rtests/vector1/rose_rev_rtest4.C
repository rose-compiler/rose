namespace std
{
}
#include <vector>
using std::vector;

void func()
{
std::vector< int  , class std::allocator< int  >  > v;
v . push_back(1);
v . push_back(2);
v . push_back(3);
}

int main()
{
func();
return 0;
}

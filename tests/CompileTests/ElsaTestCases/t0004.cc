// cc.in4
// testcase for some sharing

typedef int x;
int z;
typedef int t;

int main()
{
  x *y = (t*)z;
  
  // left side: multiplication or declaration
  // right side: unambiguous cast
  // goal: share the "cast" node on the RHS, not merely the typeid and expr
}

// test unparser to have space between : and ::std
// Liao, 2/16/2009
#include <string>
using namespace std;
void foo()
{
  bool first;
  string s;
  s += first ? "" : string(",\n");
}

#include <vector>
#include <list>

void g()
{
  std::vector<int> v;
  v.insert(v.end(), 1);

  v.resize(v.size() + 0x01, 1);

  std::list<int>* vv = new std::list<int>();
  vv->insert(vv->begin(), 1);

}

// Liao, 2/27/2009
// A test for 'const' used in function parameter list
#include <vector>
using namespace std;
class CFGEdge;
class CFGNode
{
 std::vector<CFGEdge> outEdges() const;
};

class CFGPath{
  public:
 CFGNode target() const ;
};

void makeClosure(CFGNode (CFGPath::*otherSide)() const)
{
   vector<CFGPath> rawEdges;
  for (vector<CFGPath>::const_iterator i = rawEdges.begin(); i != rawEdges.end(); ++i)
      ((*i).*otherSide)();

}
void foo()
{
  CFGPath path;
  makeClosure(&CFGPath::target);

}


// This test is deriv3ed from input327.C 
// It isolates a failing case in ROSE 4.x work: Case not handled! inner_kind = eok_dot_pm_call

// using namespace std;

class CFGNode
   {
   };

class CFGPath
   {
     public:
          CFGNode target() const ;
   };

void makeClosure(CFGNode (CFGPath::*otherSide)() const)
   {
     CFGPath x;
     (x.*otherSide)();
   }


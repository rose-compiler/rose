#include <general.h>
#include <PrivatizeScalar.h>
#include <LoopInfoInterface.h>
#include <DefUseChain.h>
#include <CommandOptions.h>


// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a header file it is OK here!
#define Boolean int

string PrivatizeScalar:: cmdline_help() 
   { 
    return "-pv : move scalar-assignments to innermost privatizable loops before loop transformation";
   }
Boolean PrivatizeScalar:: cmdline_configure()
{
    return CmdOptions::GetInstance()->HasOption("-pv") != 0;
}

AstNodePtr PrivatizeScalar::
operator()( LoopTransformInterface& la, const AstNodePtr& root)
{
  AstInterface& fa = la;
  DefaultDUchain defuse;
  defuse.build(root, &la.getAliasInterface(), la.getSideEffectInterface());
  for (DefaultDUchain::NodeIterator nodes = defuse.GetNodeIterator(); 
       !nodes.ReachEnd(); ++nodes) {
     DefUseChainNode *n = *nodes;
     if (!n->is_definition())
        continue;
     AstNodePtr lhs;
     if (!fa.IsVarRef(n->get_ref()) || !fa.IsAssignment(n->get_stmt(), &lhs)
         || lhs != n->get_ref())
        continue;
     AstNodePtr l = 0;
     for (DefaultDUchain::EdgeIterator uses = 
             defuse.GetNodeEdgeIterator(n,GraphAccess::EdgeOut);
         !uses.ReachEnd(); ++uses) { 
        DefUseChainNode *u  = defuse.GetEdgeEndPoint(*uses, GraphAccess::EdgeIn);
        DefaultDUchain::EdgeIterator defs = 
             defuse.GetNodeEdgeIterator(u,GraphAccess::EdgeIn); 
        ++defs;
        if (defs.ReachEnd()) {
           if (l == 0)
              l = GetEnclosingLoop(u->get_stmt(), fa);
           else if (l != GetEnclosingLoop(u->get_stmt(),fa)) {
              l = 0; break;   
           }
        }   
        else {
           l = 0; break;
        }
     }
     if (l != 0) {
        AstNodePtr body;
        if (!fa.IsLoop(l, 0,0,0,&body))
            assert(false);
        fa.RemoveStmt(n->get_stmt());
        fa.BasicBlockPrependStmt(body, n->get_stmt()); 
     }
  }
  return root;
}

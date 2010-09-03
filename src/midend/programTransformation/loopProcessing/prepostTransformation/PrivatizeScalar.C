

#include <PrivatizeScalar.h>
#include <LoopInfoInterface.h>
#include <DefUseChain.h>
#include <CommandOptions.h>


std::string PrivatizeScalar:: cmdline_help() 
   { 
    return "-pv : move scalar-assignments to innermost privatizable loops before loop transformation";
   }
bool PrivatizeScalar:: cmdline_configure()
{
    return CmdOptions::GetInstance()->HasOption("-pv") != 0;
}

AstNodePtr PrivatizeScalar::
operator()( LoopTransformInterface& la, const AstNodePtr& root)
{
  AstInterface& fa = la;
  DefaultDUchain defuse;
  defuse.build(fa, root, &la.getAliasInterface(), la.getSideEffectInterface());
  for (DefaultDUchain::NodeIterator nodes = defuse.GetNodeIterator(); 
       !nodes.ReachEnd(); ++nodes) {
     DefUseChainNode *n = *nodes;
     if (!n->is_definition())
        continue;
     AstNodePtr lhs;
     if (!fa.IsVarRef(n->get_ref()) || !fa.IsAssignment(n->get_stmt(), &lhs)
         || lhs != n->get_ref())
        continue;
     AstNodePtr l = AST_NULL;
     for (DefaultDUchain::EdgeIterator uses = 
             defuse.GetNodeEdgeIterator(n,GraphAccess::EdgeOut);
         !uses.ReachEnd(); ++uses) { 
        DefUseChainNode *u  = defuse.GetEdgeEndPoint(*uses, GraphAccess::EdgeIn);
        DefaultDUchain::EdgeIterator defs = 
             defuse.GetNodeEdgeIterator(u,GraphAccess::EdgeIn); 
        ++defs;
        if (defs.ReachEnd()) {
           if (l == AST_NULL)
              l = GetEnclosingLoop(u->get_stmt(), fa);
           else if (l != GetEnclosingLoop(u->get_stmt(),fa)) {
              l = AST_NULL; break;   
           }
        }   
        else {
           l = AST_NULL; break;
        }
     }
     if (l != AST_NULL) {
        AstNodePtr body;
        if (!fa.IsLoop(l, 0,0,0,&body))
            assert(false);
        fa.RemoveStmt(n->get_stmt());
        fa.BlockPrependStmt(body, n->get_stmt()); 
     }
  }
  return root; // Does not seem to be used, but Qing suggests this as the correct return value
}

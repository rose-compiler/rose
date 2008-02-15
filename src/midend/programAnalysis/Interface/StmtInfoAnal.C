#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using namespace std;

#include <iostream>
#include <stdlib.h>

#include <outsideInterface/AstInterface.h>
#include <StmtInfoAnal.h>

Boolean CollectStmtInfoTraverse :: 
ProcessTree( AstInterface &fa, const AstNodePtr& s, 
                       AstInterface::TraversalVisitType t) 
{
 if (t == AstInterface::PreVisit) {
   if (fa.IsAssignment(s)) {
     AstNodePtr lhs = fa.GetAssignmentLHS(s);
     if (col->GetCollectType() & StmtInfoCollect::LOCAL_MOD_VAR) {
        col->Append( lhs, StmtInfoCollect::LOCAL_MOD_VAR );
     }
     SkipOnly(lhs);
   }
   else if (fa.IsIOInputStmt(s)) {
     AstInterface::AstNodeList args = fa.GetIOInputVarList(s);
     fa.ListReverse(args);
     for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(args);
           !p.ReachEnd(); ++p) {
           AstNodePtr cur = *p;
           if (col->GetCollectType() & StmtInfoCollect::LOCAL_MOD_VAR) 
               col->Append(cur, StmtInfoCollect::LOCAL_MOD_VAR);
           SkipOnly(cur);
     }
   }
   else if (fa.IsInvocation(s)) {
      if ( col->GetCollectType() & StmtInfoCollect::FUNC_CALL)
         col->Append(s, StmtInfoCollect::FUNC_CALL);
   }
   else if ( (fa.IsVarRef(s) || fa.IsSubscriptedRef(s)) && 
            (col->GetCollectType() & StmtInfoCollect::LOCAL_READ_VAR)) {
      col->Append(s, StmtInfoCollect::LOCAL_READ_VAR);
   }
 }
 return true;
}

#include "defUseQuery.h"

/*************************************************
 ******************** MemObj *********************
 *************************************************/
// bool MemObj::isAccessedByPointer() 
// { 
//   return pointerAccessed; 
// }

// void MemObj::insert(VariableId _vid)
// {
//   set.insert(_vid);
// }

// void MemObj::setVariableIdSet(VariableIdSet that)
// {
//   set = that;
// }

void ProcessOperands::visit(SgVarRefExp* sgn)
{
  // set.insert(vidm.variableId(sgn));
}

void ProcessOperands::visit(SgPntrArrRefExp* sgn)
{  
}

void ProcessOperands::visit(SgPointerDerefExp* sgn)
{
  set = fipa.getMemModByPointer();
}

MemObj getDefMemObj(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa)
{
  MemObj rMemObj;
  // handle only expressions for now
  try 
  {
    if(!isSgExpression(sgn))
      throw;
  }
  catch(...)
  {
    std::cerr <<  "error: argument is not an expression\n";
  }

  ProcessQuery pq;
  // how should we handle function call expression here ?
  MatchResult matches = pq("$OP=SgAssignOp", sgn);

  // return empty object if no writes happening
  if(matches.size() == 0)
    return rMemObj;

  for(MatchResult::iterator it = matches.begin(); it != matches.end(); ++it)
  {
    // get lhs_operand
    // apply visitor pattern on lhs to identify memory location assigned
    SgNode* op = (*it)["$OP"];
    SgNode* lhs_op = isSgAssignOp(op)->get_lhs_operand();
    ProcessOperands po(fipa);
    lhs_op->accept(po);   
  }

  return rMemObj;  
}

// MemObj getUseMemObj(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa)
// {
//   MemObj rMemObj;
//   return rMemObj;
// }

#include "sage3basic.h"
#include "live_dead_analysis.h"
#include "compose.h"
#include <boost/make_shared.hpp>

using namespace std;
using namespace dbglog;
namespace fuse {
int liveDeadAnalysisDebugLevel=0;

// ###############################
// ##### LiveDeadMemAnalysis #####
// ###############################

LiveDeadMemAnalysis::LiveDeadMemAnalysis(funcSideEffectUses* fseu): 
fseu(fseu)
{
}

// Initializes the state of analysis lattices at the given function, part and edge into our out of the part
// by setting initLattices to refer to freshly-allocated Lattice objects.
void LiveDeadMemAnalysis::genInitLattice(PartPtr part, PartEdgePtr pedge, 
                                         std::vector<Lattice*>& initLattices)
{
  AbstractObjectSet* s = new AbstractObjectSet(part->outEdgeToAny(), getComposer(), this, AbstractObjectSet::may);
  
  // If this part is the return statement of main(), make sure that its return value is live
  if(SgReturnStmt* returnStmt = part->maySgNodeAny<SgReturnStmt>()) {
    SgFunctionDefinition* funcD = SageInterface::getEnclosingFunctionDefinition(returnStmt);
    if(funcD == SageInterface::findMain(SageInterface::getGlobalScope(returnStmt))->get_definition()) {
      // Get the memory location of the return statement's operand. Note that although this is a backward
      // analysis, the edge passed to OperandExpr2MemLoc() is the edge that comes into part. This is because
      // this edge denotes the set of executions that terminates at the return statement part and executions
      // always run forwards.
      // MemLocObjectPtrPair p(composer->OperandExpr2MemLoc(returnStmt, returnStmt->get_expression(), part->inEdgeFromAny(), this));
      // s->insert(p.expr? p.expr : p.mem);
      MemLocObjectPtr p(composer->OperandExpr2MemLoc(returnStmt, returnStmt->get_expression(), part->inEdgeFromAny(), this));
      s->insert(p);
    }
  }
  
  initLattices.push_back(s);
}

/// Visits live expressions - helper to LiveDeadVarsTransfer
class LDMAExpressionTransfer : public ROSE_VisitorPatternDefaultBase
{
    LiveDeadMemTransfer &ldmt;

public:
    // Should only be called on expressions
    void visit(SgNode *) { assert(0); }
    // Catch up any other expressions that are not yet handled
    void visit(SgExpression * expr)
    {
        // Function Reference
        // !!! CURRENTLY WE HAVE NO NOTION OF VARIABLES THAT IDENTIFY FUNCTIONS, SO THIS CASE IS EXCLUDED FOR NOW
        /*} else if(isSgFunctionRefExp(sgn)) {*/
        /*} else if(isSgMemberFunctionRefExp(sgn)) {*/

        // !!! DON'T KNOW HOW TO HANDLE THESE
        /*} else if(isSgStatementExpression(sgn)) {(*/

        // Typeid
        // !!! DON'T KNOW WHAT TO DO HERE SINCE THE RETURN VALUE IS A TYPE AND THE ARGUMENT'S VALUE IS NOT USED
        /*} else if(isSgTypeIdOp(sgn)) {*/
        // Var Args
        // !!! DON'T HANDLE THESE RIGHT NOW. WILL HAVE TO IN THE FUTURE
        /*  SgVarArgOp 
            SgExpression *  get_operand_expr () const 
            SgVarArgCopyOp
            SgExpression *  get_lhs_operand () const
            SgExpression *  get_rhs_operand () const  
            SgVarArgEndOp 
            SgExpression *  get_operand_expr 
            SgVarArgStartOneOperandOp 
            SgExpression *  get_operand_expr () const 
            SgVarArgStartOp 
            SgExpression *  get_lhs_operand () const
            SgExpression *  get_rhs_operand () const */
        // !!! WHAT IS THIS?
        /*  SgVariantExpression*/


        // TODO: Make this assert(0), because unhandled expression types are likely to give wrong results
      assert(0);
    }
    // Plain assignment: lhs = rhs
    void visit(SgAssignOp *sgn) {
      ldmt.assign(sgn, sgn->get_lhs_operand());
      
      // If this expression or the value it writes are live, make the rhs live
      if(ldmt.isMemLocLive(sgn) || ldmt.isMemLocLive(sgn, sgn->get_lhs_operand())) {
        // If the lhs of the assignment is a complex expression (i.e. it refers to a variable that may be live) OR
        // if is a known expression that is known to may-be-live
        // THIS CODE ONLY APPLIES TO RHSs THAT ARE SIDE-EFFECT-FREE AND WE DON'T HAVE AN ANALYSIS FOR THAT YET
        /*if(!isVarExpr(sgn->get_lhs_operand()) || 
          (isVarExpr(sgn->get_lhs_operand()) && 
          liveLat->isLiveVar(SgExpr2Var(sgn->get_lhs_operand()))))
          { */
        ldmt.use(sgn, sgn->get_rhs_operand());
      }
    }
    // Initializer for a variable
    void visit(SgAssignInitializer *sgn) {
      // Only make the operand(s) live if the expression is live
      if(!ldmt.isMemLocLive(sgn)) return;
      
      if(liveDeadAnalysisDebugLevel>=1) {
        dbg << "LDMAExpressionTransfer::visit(SgAssignInitializer *sgn)"<<endl;
        dbg << "&nbsp;&nbsp;&nbsp;&nbsp;sgn="<<SgNode2Str(sgn)<<endl;
        dbg << "&nbsp;&nbsp;&nbsp;&nbsp;sgn->get_operand()="<<SgNode2Str(sgn->get_operand())<<endl;
      }
      ldmt.use(sgn, sgn->get_operand());
    }
    // Initializer for a function arguments
    void visit(SgConstructorInitializer *sgn) {
      // Only make the operand(s) live if the expression is live
      if(!ldmt.isMemLocLive(sgn)) return;
      
      SgExprListExp* exprList = sgn->get_args();
      for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
          expr!=exprList->get_expressions().end(); expr++)
        ldmt.use(sgn, *expr);
    }
    // Initializer that captures internal stucture of structs or arrays ("int x[2] = {1,2};", it is the "1,2")
    // NOTE: Should this use abstractMemory interface ?
    void visit(SgAggregateInitializer *sgn) {
      // Only make the operand(s) live if the expression is live
      if(!ldmt.isMemLocLive(sgn)) return;
      
      SgExprListExp* exprList = sgn->get_initializers();
      for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
          expr!=exprList->get_expressions().end(); expr++)
        ldmt.use(sgn, *expr);
    }
    // Designated Initializer 
    void visit(SgDesignatedInitializer *sgn) {
      // Only make the operand(s) live if the expression is live
      if(!ldmt.isMemLocLive(sgn)) return;
      
      SgExprListExp* exprList = sgn->get_designatorList();
      for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
          expr!=exprList->get_expressions().end(); expr++)
        ldmt.use(sgn, *expr);
    }
    // Variable access
    void visit(SgVarRefExp *sgn) {
//  Liao, 4/5/2012. We cannot decide if a SgVarRefExp is read or written
//    without its context information: for example, in  a = b; both a and b are represented as
//    SgVarRefExp. But a is written and b is read.
//    We should let the ancestor node (like SgAssignOp) decide on the READ/Written of SgVarRefExp.
//  GB 2012-09-04: We don't need to care about read/write access for this because the only way that a
//    variable reference can be live is if it is used, not assigned by its parent expression.
      
      //ldmt.useMem(sgn);
    }
    void visit(SgFunctionRefExp* sgn) {
      // TODO: This should be handled when we properly support CodeLocObjects
    }
    // Array access
    void visit(SgPntrArrRefExp *sgn) {
      //if(ldmt.isMemLocLive(sgn)) {
        if(liveDeadAnalysisDebugLevel>=1) dbg << "visit(SgPntrArrRefExp *sgn)"<<endl;
        // The only way for this SgPntrArrRefExp to appear s if it is used by its parent expression
        ldmt.useMem(sgn);
        // Both the lhs and rhs are used to identify the memory location being accessed
        if(liveDeadAnalysisDebugLevel>=1) dbg << "LHS"<<endl;
        ldmt.use(sgn, sgn->get_lhs_operand());
        if(liveDeadAnalysisDebugLevel>=1) dbg << "RHS"<<endl;
        ldmt.use(sgn, sgn->get_rhs_operand());
      //}
    }
    // Binary Operations
    void visit(SgBinaryOp *sgn) {
      /* GB 2012-09-05 : The liveness of the operand is not changed as a result of an auto-update
      // Self-update expressions, where the lhs is assigned
      if(isSgCompoundAssignOp(sgn))
        ldmt.assign(sgn, sgn->get_lhs_operand());*/
      
      if(liveDeadAnalysisDebugLevel>=1) dbg << "LiveDead: visit("<<SgNode2Str(sgn)<<") ldmt.isMemLocLive(sgn)="<<ldmt.isMemLocLive(sgn)<<endl;
      
      // If this expression is live or writes writes to a live memory location, make the operands live
      if(ldmt.isMemLocLive(sgn) || (isSgCompoundAssignOp(sgn) && ldmt.isMemLocLive(sgn, sgn->get_lhs_operand()))) {
        // Both the lhs and rhs are used
        ldmt.use(sgn, sgn->get_lhs_operand());
        ldmt.use(sgn, sgn->get_rhs_operand());
      }
    }
    // Unary Operations
    void visit(SgUnaryOp *sgn) {
      /* GB 2012-09-05 : The liveness of the operand is not changed as a result of an auto-update
       * // If this is an auto-update operation
      if(isSgMinusMinusOp(sgn) || isSgPlusPlusOp(sgn)) {
        // The argument is defined
        ldmt.assign(sgn, sgn->get_operand());
      }*/
        
        // If this expression is live or writes writes to a live memory location, make the operands live
      if(ldmt.isMemLocLive(sgn) || 
         ((isSgMinusMinusOp(sgn) || isSgPlusPlusOp(sgn)) && ldmt.isMemLocLive(sgn, sgn->get_operand())))
        // The argument is used
        ldmt.use(sgn, sgn->get_operand());
    }
    
    // Conditionals (condE ? trueE : falseE)
    void visit(SgConditionalExp *sgn) {
      // Only make the operand(s) live if the expression is live
      if(!ldmt.isMemLocLive(sgn)) return;
      
      // The arguments are used
      ldmt.use(sgn, sgn->get_conditional_exp());
      ldmt.use(sgn, sgn->get_true_exp());
      ldmt.use(sgn, sgn->get_false_exp());
    }
    // Delete
    void visit(SgDeleteExp *sgn) {
      // GB: 2012-09-05 - I don't think we need to process these in live-dead because a deleted memory location
      //      definitely cannot be used afterwards and must be dead at deletion time.
      
      // Delete expressions return nothing
      // The arguments are used
      //ldmt.use(sgn, sgn->get_variable());
    }
    // New
    void visit(SgNewExp *sgn) {
      // Only make the operand(s) live if the expression is live
      if(!ldmt.isMemLocLive(sgn)) return;
      
      // The placement arguments are used
      SgExprListExp* exprList = sgn->get_placement_args();
      // NOTE: placement args are optional
      // exprList could be NULL
      // check for NULL before adding to used set
      if(exprList) {
        for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
          expr!=exprList->get_expressions().end(); expr++)
          ldmt.use(sgn, *expr);
      }
                              
      // The placement arguments are used
      // check for NULL before adding to used set
      // not sure if this check is required for get_constructor_args()
      exprList = sgn->get_constructor_args()->get_args();
      if(exprList) {
        for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
          expr!=exprList->get_expressions().end(); expr++)
          ldmt.use(sgn, *expr);
      }
                              
      // The built-in arguments are used (DON'T KNOW WHAT THESE ARE!)
      // check for NULL before adding to used set
      // not sure if this check is required for get_builtin_args()
      if(sgn->get_builtin_args()) {
        ldmt.use(sgn, sgn->get_builtin_args());
      }
    }
    // Function Calls
    void visit(SgFunctionCallExp *sgn) {
      // The side-effects of calls to functions for which source is available are processed by inter-procedural 
      // analyses. Here we need to consider the variable that identifies the function being called and the 
      // side-effects of functions for which source is not available.
      
      // !!! CURRENTLY WE HAVE NO NOTION OF VARIABLES THAT IDENTIFY FUNCTIONS, SO THIS CASE IS EXCLUDED FOR NOW
      /*// The expression that identifies the called function is used
        ldmt.use(sgn, sgn->get_function());*/
      
      /*
      // If the function's source code is not available
      Function func(sgn);
      if(liveDeadAnalysisDebugLevel>=1) dbg << "LiveDeadAnalysis::visit(SgFunctionCallExp *sgn) call to function "<<func.get_name().getString()<<"()"<<endl;
      if(!func.get_definition()) {
        if(liveDeadAnalysisDebugLevel>=1) dbg << "LiveDeadAnalysis::visit(SgFunctionCallExp *sgn) Function "<<func.get_name().getString()<<"() with no definition."<<endl;
      
        // The function call's arguments are used
        SgExprListExp* exprList = sgn->get_args();
        for(SgExpressionPtrList::iterator expr=exprList->get_expressions().begin();
            expr!=exprList->get_expressions().end(); expr++)
            ldmt.use(sgn, *expr);

        / * // If this function has no definition and the user provided a class to provide 
        // the variables that are used by such functions
        if(sgn->getAssociatedFunctionDeclaration() && 
           sgn->getAssociatedFunctionDeclaration()->get_definition()==NULL &&
           ldmt.fseu) {
            set<MemLocObjectPtr> funcUsedVars = ldmt.fseu->usedVarsInFunc(Function(sgn->getAssociatedFunctionDeclaration()), ldmt.part, ldmt.nodeState);
            //ldmt.use(sgn, funcUsedVars.begin(), funcUsedVars.end());
            for(set<MemLocObjectPtr>::iterator used=funcUsedVars.begin(); used!=funcUsedVars.end(); used++)
            ldmt.use(sgn, *used);
        }* /
      }*/
    }
    
    // Sizeof
    void visit(SgSizeOfOp *sgn) {
      // Only make the operand(s) live if the expression is live
      if(!ldmt.isMemLocLive(sgn)) return;
      
      // XXX: The argument is NOT used, but its type is
      // NOTE: get_operand_expr() returns NULL when sizeof(type)
      // FIX: use get_operand_expr() only when sizeof() involves expr
      // ldmt.use(sgn, sgn->get_operand_expr());
      if(sgn->get_operand_expr()) {
        ldmt.use(sgn, sgn->get_operand_expr());
      }
    }
    // This
    void visit(SgThisExp *sgn) {}
    // Literal values
    void visit(SgValueExp* sgn) {}
    
    LDMAExpressionTransfer(LiveDeadMemTransfer &ldmt)
    : ldmt(ldmt)
    { }
};

LiveDeadMemTransfer::LiveDeadMemTransfer(PartPtr part, CFGNode cn, NodeState &s, 
                    std::map<PartEdgePtr, std::vector<Lattice*> > &dfInfo, 
                    LiveDeadMemAnalysis* ldma,
                    Composer* composer, funcSideEffectUses *fseu)
    : DFTransferVisitor(part, cn, s, dfInfo),
    ldma(ldma), 
    composer(composer),
    modified(false), 
    assigned(part->inEdgeFromAny(), composer, (ComposedAnalysis*)ldma, AbstractObjectSet::may), 
    used(part->inEdgeFromAny(), composer, (ComposedAnalysis*)ldma, AbstractObjectSet::may), 
    part(part), 
    fseu(fseu)
{
  assert(dfInfo.find(PartEdgePtr()) != dfInfo.end());
  liveLat = dynamic_cast<AbstractObjectSet*>(*(dfInfo[PartEdgePtr()].begin()));

  if(liveDeadAnalysisDebugLevel>=1) {
    dbg << "LiveDeadMemTransfer: liveLat=";
    indent ind(liveDeadAnalysisDebugLevel, 1);
    dbg << liveLat->str("")<<endl;
  }
  // Make sure that all the lattice is initialized
  liveLat->initialize();
}

// Note that the variable corresponding to this expression is assigned
void LiveDeadMemTransfer::assign(SgNode *sgn, SgExpression* operand)
{
  //dbg << "LiveDeadMemTransfer::assign(sgn="<<SgNode2Str(sgn)<<" operand="<<SgNode2Str(operand)<<endl;
  // MemLocObjectPtrPair p(composer->OperandExpr2MemLoc(sgn, operand, part->outEdgeToAny(), ldma)/*ceml->Expr2Obj(sgn)*/);
  MemLocObjectPtr p(composer->OperandExpr2MemLoc(sgn, operand, part->outEdgeToAny(), ldma)/*ceml->Expr2Obj(sgn)*/);
  //return boost::dynamic_pointer_cast<AbstractObject>(cpMap->get(ml));
  // if(p.expr) assigned.insert(p.expr);
  // // GB 2012-09-05 : Need this to handle lhs of SgAssignOps where the lhs is a memory location.
  // if(p.mem) assigned.insert(p.mem);
  assigned.insert(p);
}
/*void LiveDeadMemTransfer::assign(AbstractObjectPtr mem)
{
    assigned.insert(mem);
}*/

// Note that the variable corresponding to this expression is used
void LiveDeadMemTransfer::use(SgNode *sgn, SgExpression* operand)
{
  //dbg << "part->outEdgeToAny()="<<part->outEdgeToAny()->str()<<endl;
  // MemLocObjectPtrPair p = composer->OperandExpr2MemLoc(sgn, operand, part->outEdgeToAny(), ldma);//ceml->Expr2Obj(sgn);
  scope reg(txt()<<"LiveDeadMemTransfer::use(sgn="<<SgNode2Str(sgn)<<", operand="<<SgNode2Str(operand)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  MemLocObjectPtr p = composer->OperandExpr2MemLoc(sgn, operand, part->outEdgeToAny(), ldma);//ceml->Expr2Obj(sgn);
  //dbg << "LiveDeadMemTransfer::use(sgn=["<<escape(sgn->unparseToString())<<" | "<<sgn->class_name()<<"]"<<endl;
  //dbg << "p="<<p->str()<<endl;
  // In almost all cases we only need expressions to use their operands, which are also expressions.
  // if(p.expr) used.insert(p.expr);
  // At statement boundaries SgVarRefExp and SgArrPntrRefExp refer to real memory locations that were written by prior
  // statements. 
  // if((isSgVarRefExp(operand) || isSgPntrArrRefExp(operand)) && p.mem)  used.insert(p.mem);
  // #SA
  // expression is either memory or temporary neither both
  // add the object returned by OperandExpr2MemLoc to used ??
  // 
  if(p) used.insert(p);
}
// Note that the memory location denoted by the corresponding SgInitializedName is used
void LiveDeadMemTransfer::useMem(SgInitializedName* name)
{
  scope reg(txt()<<"LiveDeadMemTransfer::useMem(name="<<SgNode2Str(name)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  dbg << "name="<<SgNode2Str(name)<<endl;
  MemLocObjectPtr p = composer->Expr2MemLoc(name, part->outEdgeToAny(), ldma);
  if(liveDeadAnalysisDebugLevel>=1) dbg << "LiveDeadMemTransfer::useMem(SgInitializedName)("<<SgNode2Str(name)<<")"<<endl;
  // used.insert(p.mem);
  used.insert(p);
}
// Note that the memory location denoted by the corresponding SgVarRefExp is used
void LiveDeadMemTransfer::useMem(SgVarRefExp* sgn)
{
  scope reg(txt()<<"LiveDeadMemTransfer::useMem(name="<<SgNode2Str(sgn)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  // MemLocObjectPtrPair p = composer->Expr2MemLoc(sgn, part->outEdgeToAny(), ldma);//ceml->Expr2Obj(sgn);
  MemLocObjectPtr p = composer->Expr2MemLoc(sgn, part->outEdgeToAny(), ldma);//ceml->Expr2Obj(sgn);
  if(liveDeadAnalysisDebugLevel>=1) dbg << "LiveDeadMemTransfer::useMem(SgVarRefExp)("<<SgNode2Str(sgn)<<")"<<endl;
  // used.insert(p.mem);
  used.insert(p);
}
// Note that the memory location denoted by the corresponding SgPntrArrRefExp is used
void LiveDeadMemTransfer::useMem(SgPntrArrRefExp* sgn)
{
  scope reg(txt()<<"LiveDeadMemTransfer::useMem(sgn="<<SgNode2Str(sgn)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  // We use the SgPntrArrRefExp itself as well as all of its parent SgPntrArrRefExp because to reach
  // this index we need to access all the indexes that precede it in the expression, as well as the root 
  // (array in array[1][2][3]) that identifies the base pointer.
  
  // !!! NOTE: This is not the best solution for two reasons:
  // !!! - In statically-allocated multi-dimensional arrays you don't actually use one dimension to access another
  // !!! - A more general treatment of this would build up referents of SgPntrArrRefExps incrementally, meaning that
  // !!!   this SgPntrArrRefExp would only use its immediate parent. However, to pull this off we'd need for
  // !!!   OrthogonalArrayAnalysis to also manage SgPntrArrRefExps incrementally, rather than a base address and a 
  // !!!   multi-dimensional offset. This should be resolved when we move to an object-offset representation for 
  // !!!   MemLocs that makes it easy to not explicitly maintain array indexes.

  if(liveDeadAnalysisDebugLevel>=1) {
    dbg << "LiveDeadMemTransfer::useMem(SgPntrArrRefExp)("<<SgNode2Str(sgn)<<")"<<endl;
    dbg << "LHS="<<SgNode2Str(sgn->get_lhs_operand())<<endl;
    dbg << "RHS="<<SgNode2Str(sgn->get_rhs_operand())<<endl;
  }
  MemLocObjectPtr p = composer->Expr2MemLoc(sgn, part->outEdgeToAny(), ldma);
  // If a memory object is available, insert it. Not all SgPntrArrRefExps correspond to a real memory location.
  // e.g. in array2d[a][b] the expression array2D[a] doesn't denote a memory location if array2d is allocated statically.
  if(p) used.insert(p);
  
  /*do {
    if(liveDeadAnalysisDebugLevel>=1) { dbg << "LHS="<<SgNode2Str(sgn->get_lhs_operand())<<endl; }
    MemLocObjectPtr p = composer->Expr2MemLoc(sgn->get_lhs_operand(), part->outEdgeToAny(), ldma);
    // If a memory object is available, insert it. Not all SgPntrArrRefExps correspond to a real memory location.
    // e.g. in array2d[a][b] the expression array2D[a] doesn't denote a memory location if array2d is allocated statically.
    if(p) used.insert(p);
    sgn = isSgPntrArrRefExp(sgn->get_lhs_operand());
  } while(sgn);*/
}

/*void LiveDeadMemTransfer::use(AbstractObjectPtr mem)
{
    used.insert(mem);
}*/

// Returns true if the given expression is currently live and false otherwise
bool LiveDeadMemTransfer::isMemLocLive(SgExpression* sgn, SgExpression* operand) {
  // MemLocObjectPtrPair p = composer->OperandExpr2MemLoc(sgn, operand, part->outEdgeToAny(), ldma);//ceml->Expr2Obj(expr);
  // return (p.expr ? liveLat->containsMay(p.expr) : false) ||
  //        (p.mem  ? liveLat->containsMay(p.mem)  : false);
  MemLocObjectPtr p = composer->OperandExpr2MemLoc(sgn, operand, part->outEdgeToAny(), ldma);//ceml->Expr2Obj(expr);
  return (p ? liveLat->containsMay(p) : false);

}

// Returns true if the given expression is currently live and false otherwise
bool LiveDeadMemTransfer::isMemLocLive(SgExpression* sgn) {
  // MemLocObjectPtrPair p = composer->Expr2MemLoc(sgn, part->outEdgeToAny(), ldma);//ceml->Expr2Obj(expr);
  // return (p.expr ? liveLat->containsMay(p.expr) : false) ||
  //        (p.mem  ? liveLat->containsMay(p.mem)  : false);
  MemLocObjectPtr p = composer->Expr2MemLoc(sgn, part->outEdgeToAny(), ldma);//ceml->Expr2Obj(expr);
  return (p ? liveLat->containsMay(p) : false);
}

void LiveDeadMemTransfer::visit(SgExpression *sgn)
{
  scope reg(txt()<<"LiveDeadMemTransfer::visit(sgn="<<SgNode2Str(sgn)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  
  //AbstractMemoryObject::ObjSet* objset = SgExpr2ObjSet(sgn);
  // MemLocObjectPtrPair p = composer->Expr2MemLoc(sgn, part->outEdgeToAny(), ldma);//ceml->Expr2Obj(sgn);
  MemLocObjectPtr p = composer->Expr2MemLoc(sgn, part->outEdgeToAny(), ldma);//ceml->Expr2Obj(sgn);
  LDMAExpressionTransfer helper(*this);
  sgn->accept(helper);
  
  // Remove the expression object itself since it has no uses above itself.
  // Do not remove the memory location object since it has just been used.
  /*if(!isSgVarRefExp(sgn) && !isSgPntrArrRefExp(sgn)) // Liao 4/5/2012, we should not remove SgVarRef or SgPntrArrRefExp since it may have uses above itself
  {
    //if(liveDeadAnalysisDebugLevel>=1) dbg << "   Removing "<< mem.str("         ") <<endl;
    //modified = liveLat->remove(mem) || modified;
    if(assigned.insert(mem); /// ????
  }*/
  // if(p.expr) assigned.insert(p.expr);
  // #SA
  // Should we discard expressions that are memory here ?
  // # GB
  // No, we should not. Fixed.
  if(!MemLocObject::isMemExpr(sgn) && p) assigned.insert(p);
}

void LiveDeadMemTransfer::visit(SgInitializedName *sgn) {
  scope reg(txt()<<"LiveDeadMemTransfer::visit(sgn="<<SgNode2Str(sgn)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  /*SgVarRefExp* exp = SageBuilder::buildVarRefExp(sgn);
  dbg << "LiveDeadMemTransfer::visit(SgInitializedName: sgn=["<<escape(sgn->unparseToString())<<" | "<<sgn->class_name()<<"]"<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;exp="<<exp<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;exp=["<<escape(exp->unparseToString())<<" | "<<exp->class_name()<<"]"<<endl;*/
  // MemLocObjectPtrPair p = composer->Expr2MemLoc(sgn, part->outEdgeToAny(), ldma); //ceml->Expr2Obj(exp);
  MemLocObjectPtr p = composer->Expr2MemLoc(sgn, part->outEdgeToAny(), ldma); //ceml->Expr2Obj(exp);
  assert(p);
  bool isLive = (p  ? liveLat->containsMay(p)  : false);
  /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;isLive="<<isLive<<endl;
  if(liveDeadAnalysisDebugLevel>=1)
    dbg << indent << (isLive ? "Live Expression" : "Dead Expression") <<endl;*/

  // If this is a live variable and this is the instance of SgInitializedName that occurs immediately after 
  // the declaration's initializer AND this declaration has an initializer, add it as a use
  if(isLive && sgn->get_initializer())
    use(sgn, sgn->get_initializer());
  //if(p.expr) assigned.insert(p.expr);
  //if(p.mem)  assigned.insert(p.mem);
  // assigned.insert(p.mem);
  assigned.insert(p);
}

void LiveDeadMemTransfer::visit(SgReturnStmt *sgn) {
  scope reg(txt()<<"LiveDeadMemTransfer::visit(sgn="<<SgNode2Str(sgn)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
/*  
  WE CURRENTLY ASSUME THAT THE EXPRESSION OF A RETURN STATEMENT IS ALWAYS USED
  SHOULD ONLY ASSUME THIS FOR RETURN STA```TEMENT OF main()*/
  
  use(sgn, sgn->get_expression());
}
void LiveDeadMemTransfer::visit(SgExprStatement *sgn) {
    /*use(sgn, sgn->get_expression());*/
}
void LiveDeadMemTransfer::visit(SgSwitchStatement *sgn) {
  // The operand of a control-flow statement is automatically live only
  // if the control-flow statement leads to multiple control options
  // This is always true for switch statements since they can always skip all the cases.
  // However, in the future may want to check for whether the value of the switch is a constant.
  // GB 2012-09-19 : Which object corresponds to the expression that chooses the case?
  assert(0);
  //use(sgn, sgn->get_item_selector());
}
void LiveDeadMemTransfer::visit(SgCaseOptionStmt *sgn) {
  scope reg(txt()<<"LiveDeadMemTransfer::visit(sgn="<<SgNode2Str(sgn)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  use(sgn, sgn->get_key());
  use(sgn, sgn->get_key_range_end());
}
void LiveDeadMemTransfer::visit(SgIfStmt *sgn) {
  // The operand of a control-flow statement is automatically live only
  // if the control-flow statement leads to multiple control options
  // This is always true for if statements since there's a difference between the true body executing and not executing
  // even if there is no false body.
  // However, in the future may want to check for whether the value of the switch is a constant.
  
  assert(isSgExprStatement(sgn->get_conditional()));
  use(sgn, isSgExprStatement(sgn->get_conditional())->get_expression());
}

void LiveDeadMemTransfer::visit(SgForStatement *sgn) {
  scope reg(txt()<<"LiveDeadMemTransfer::visit(sgn="<<SgNode2Str(sgn)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  //dbg << "test="<<escape(sgn->get_test()->unparseToString()) << " | " << sgn->get_test()->class_name()<<endl;
  //dbg << "increment="<<escape(sgn->get_increment()->unparseToString()) << " | " << sgn->get_increment()->class_name()<<endl;
  
  // The operands of a for statement are automatically live since loops have an unknown
  // number of iterations that is decided based on these operands
  assert(isSgExprStatement(sgn->get_test()));
  use(sgn, isSgExprStatement(sgn->get_test())->get_expression());
  use(sgn, sgn->get_increment());
}
void LiveDeadMemTransfer::visit(SgWhileStmt *sgn) {
  scope reg(txt()<<"LiveDeadMemTransfer::visit(sgn="<<SgNode2Str(sgn)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  // The operands of a while statement are automatically live since loops have an unknown
  // number of iterations that is decided based on these operands
  assert(isSgExprStatement(sgn->get_condition()));
  //dbg << "condition="<<escape(sgn->get_condition()->unparseToString()) << " | " << sgn->get_condition()->class_name()<<endl;
  use(sgn, isSgExprStatement(sgn->get_condition())->get_expression());
}
void LiveDeadMemTransfer::visit(SgDoWhileStmt *sgn) {
  scope reg(txt()<<"LiveDeadMemTransfer::visit(sgn="<<SgNode2Str(sgn)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  // The operands of a do-while statement are automatically live since loops have an unknown
  // number of iterations that is decided based on these operands
  assert(isSgExprStatement(sgn->get_condition()));
  //dbg << "condition="<<escape(sgn->get_condition()->unparseToString()) << " | " << sgn->get_condition()->class_name()<<endl;
  use(sgn, isSgExprStatement(sgn->get_condition())->get_expression());
}
void LiveDeadMemTransfer::visit(SgFunctionDefinition* def) {
  scope reg(txt()<<"LiveDeadMemTransfer::visit(sgn="<<SgNode2Str(def)<<")", scope::medium, liveDeadAnalysisDebugLevel, 1);
  Function func(def);
  dbg << "Definition "<<SgNode2Str(def)<<" attributeExists="<<def->attributeExists("fuse:UnknownSideEffects")<<endl;
  
  // The function's parameters are used
  SgFunctionParameterList* params = func.get_params();
  for(SgInitializedNamePtrList::iterator p=params->get_args().begin(); p!=params->get_args().end(); p++)
    useMem(*p);
}
bool LiveDeadMemTransfer::finish()
{
  scope reg(txt()<<"LiveDeadMemTransfer::finish()", scope::medium, liveDeadAnalysisDebugLevel, 1);
  
  // First process assignments, then uses since we may assign and use the same variable
  // and in the end we want to first remove it and then re-insert it.   
  if(liveDeadAnalysisDebugLevel>=1) {
    dbg << "used="<<endl;
    { indent ind(analysisDebugLevel, 1);
    for(AbstractObjectSet::const_iterator asgn=used.begin(); asgn!=used.end(); asgn++) {
      dbg << (*asgn)->str()<<endl;
    } }
    
    dbg << "assigned="<<endl;
    { indent ind(analysisDebugLevel, 1);
    for(AbstractObjectSet::const_iterator asgn=assigned.begin(); asgn!=assigned.end(); asgn++) {
      dbg << (*asgn)->str() << endl;
    } }
    
    dbg << "liveLat="<<endl;
    {indent ind(analysisDebugLevel, 1);
     dbg << liveLat->str("")<<endl;}
  }

  /* Live-In (node) = use(node) + (Live-Out (node) - Assigned (b))  
   * Live-Out (node) is the lattice after merging ???
   * */         
  // Record for each assigned expression:
  //    If the expression corresponds to a variable, record that the variable is dead.
  //    Otherwise, record that the expression that computes the assigned memory location is live
  for(AbstractObjectSet::const_iterator asgn=assigned.begin(); asgn!=assigned.end(); asgn++) {
    // If the lhs is a variable reference, remove it from live variables unless we also use this variable
    if(!used.containsMay(*asgn)) // if not found in use, then remove it, Liao 4/5/2012
    {
      // if(liveDeadAnalysisDebugLevel>=1) {
      //     dbg << indent << "    removing assigned expr <" << (*asgn)->class_name() <<":"<<(*asgn)->unparseToString();
      //     dbg << ">"<<endl;
      // }
      modified = liveLat->remove(*asgn) || modified; 
    }
    else 
    {
      modified = liveLat->insert(*asgn) || modified;
      // if(liveDeadAnalysisDebugLevel>=1) {
      //     dbg << indent << "    add assigned expr as live <" << (*asgn)->class_name() <<":"<<(*asgn)->unparseToString();
      // }
    }
  } // end for

  // Record that the used variables are live
  for(AbstractObjectSet::const_iterator var=used.begin(); var!=used.end(); var++)
    modified = liveLat->insert(*var) || modified;
  
  return modified;
}

/*******************************
 ***** LiveDeadMemAnalysis *****
 *******************************/

// Maps the given SgNode to an implementation of the MemLocObject abstraction.
MemLocObjectPtr LiveDeadMemAnalysis::Expr2MemLoc(SgNode* n, PartEdgePtr pedge)
{
  // MemLocObjectPtrPair p = composer->Expr2MemLoc(n, pedge, this);
  // dbg << "LiveDeadMemAnalysis::Expr2MemLoc() p="<<p.strp(pedge)<<endl;
  // if(p.mem) return createLDMemLocObjectCategory(n, p.mem, this);
  // else      return p.expr;
  MemLocObjectPtr p = composer->Expr2MemLoc(n, pedge, this);
  if(liveDeadAnalysisDebugLevel>=1) dbg << "LiveDeadMemAnalysis::Expr2MemLoc() p="<<p->strp(pedge)<<endl;
  // #SA
  // createLDMemLocObject for objects returned by composer for now
  return boost::make_shared<LDMemLocObject>(n, p, this);
}

/**************************
 ***** LDMemLocObject *****
 **************************/
LDMemLocObject::LDMemLocObject(SgNode* n, MemLocObjectPtr parent_, LiveDeadMemAnalysis* ldma)
  : MemLocObject(n), parent(parent_), ldma(ldma)
{
}

LDMemLocObject::LDMemLocObject(const LDMemLocObject& that) : 
    MemLocObject(that), parent(that.parent), ldma(that.ldma) 
{}

bool LDMemLocObject::mayEqualML(MemLocObjectPtr o, PartEdgePtr pedge)
{
  LDMemLocObjectPtr that = boost::dynamic_pointer_cast<LDMemLocObject>(o);
  bool isThisLive = isLiveML(pedge);
  bool isThatLive = that->isLiveML(pedge);

  /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;LDMemLocObject::mayEqual"<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;this (live="<<isThisLive<<")="<<str("")<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;that (live="<<isThatLive<<")="<<o->str("")<<endl;*/
  
  if(!that) { /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;FALSE"<<endl;*/ return false; }
  //dbg << "    isThisLive="<<isThisLive<<" isThisLive="<<isThisLive<<endl;
  // If both objects may be live, use the parents' equality operator
  if(isThisLive && isThatLive) {
    bool tmp=parent->mayEqual(that->parent, pedge, ldma->getComposer(), ldma);
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;"<<(tmp?"TRUE":"FALSE")<<endl; 
    return tmp;
  // If both objects are definitely not live, they're counted as being equal
  } else if(!isThisLive && !isThatLive) {
     //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;TRUE"<<endl; 
     return true;
  // Otherwise, they're in different classes and thus unequal
  } else
  { /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;FALSE"<<endl;*/ return false; }
}

bool LDMemLocObject::mustEqualML(MemLocObjectPtr o, PartEdgePtr pedge)
{
  LDMemLocObjectPtr that = boost::dynamic_pointer_cast<LDMemLocObject>(o);
  bool isThisLive = isLiveML(pedge);
  bool isThatLive = that->isLiveML(pedge);
        
  /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;LDMemLocObject::mustEqual"<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;this (live="<<isThisLive<<")="<<str("")<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;that (live="<<isThatLive<<")="<<o->str("")<<endl;/*/
        
  if(!that) { /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;FALSE"<<endl;*/ return false; }
  // If both objects may be live, use the parents' equality operator
  if(isThisLive && isThisLive) {
    bool tmp=parent->mustEqual(that->parent, pedge, ldma->getComposer(), ldma);
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;"<<(tmp?"TRUE":"FALSE")<<endl; 
    return tmp;
  // If both objects are definitely not live, they're counted as being equal
  } else if(!isThisLive && !isThatLive) {
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;TRUE"<<endl; 
    return true;
  // Otherwise, they're in different classes and thus unequal
  } else
  { /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;FALSE"<<endl;*/ return false; }
}

// Returns whether the two abstract objects denote the same set of concrete objects
bool LDMemLocObject::equalSet(AbstractObjectPtr o, PartEdgePtr pedge)
{
  LDMemLocObjectPtr that = boost::dynamic_pointer_cast<LDMemLocObject>(o);
  bool isThisLive = isLiveML(pedge);
  bool isThatLive = that->isLiveML(pedge);
        
  if(!that) { /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;FALSE"<<endl;*/ return false; }
  // If both objects may be live, use the parents' equality operator
  if(isThisLive && isThisLive) {
    bool tmp=parent->equalSet(that->parent, pedge, ldma->getComposer(), ldma);
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;"<<(tmp?"TRUE":"FALSE")<<endl; 
    return tmp;
  // If both objects are definitely not live, they're counted as being equal
  } else if(!isThisLive && !isThatLive) {
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;TRUE"<<endl; 
    return true;
  // Otherwise, they're in different classes and thus unequal
  } else
  { /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;FALSE"<<endl;*/ return false; }
}

// Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
// by the given abstract object.
bool LDMemLocObject::subSet(AbstractObjectPtr o, PartEdgePtr pedge) {
  LDMemLocObjectPtr that = boost::dynamic_pointer_cast<LDMemLocObject>(o);
  bool isThisLive = isLiveML(pedge);
  bool isThatLive = that->isLiveML(pedge);
        
  if(!that) { /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;FALSE"<<endl;*/ return false; }
  // If both objects may be live, use the parents' equality operator
  if(isThisLive && isThisLive) {
    bool tmp=parent->subSet(that->parent, pedge, ldma->getComposer(), ldma);
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;"<<(tmp?"TRUE":"FALSE")<<endl; 
    return tmp;
  // If both objects are definitely not live, they're counted as being equal
  } else if(!isThisLive && !isThatLive) {
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;TRUE"<<endl; 
    return true;
  // Otherwise, they're in different classes and thus disjoint
  } else
  { /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;==&gt;FALSE"<<endl;*/ return false; }
}

// Returns true if this object is live at the given part and false otherwise
bool LDMemLocObject::isLiveML(PartEdgePtr pedge)
{ 
  //dbg << "LDMemLocObject::isLive() pedge="<<pedge->str()<<endl;
  //dbg << "parent str="<<parent->str()<<endl;
  //dbg << "parent strp="<<parent->strp(pedge)<<endl;
  //dbg << "live="<<isLiveMay(parent, ldma, pedge, "")<<endl;
  //dbg << "state="<<NodeState::getNodeState(ldma, pedge->target())->str(ldma)<<endl;*/
  
  // The MemLocObject for the SgFunctionDefinition is special since it denotes the return values of a function
  // It should always be live.
  /*MemLocObject funcDeclML = 
          
          getComposer()->Expr2MemLoc(SageInterface::getEnclosingProcedure(parent)
          func.get_declaration()->search_for_symbol_from_symbol_table(), part->inEdgeFromAny(), analysis),
  
  if(parent->isFunctionMemLoc()) return true;
  else */
  
  bool live = isLiveMay(parent, ldma, pedge, "");
  return live;
}

// Computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool LDMemLocObject::meetUpdateML(MemLocObjectPtr o, PartEdgePtr pedge)
{
  LDMemLocObjectPtr that = boost::dynamic_pointer_cast<LDMemLocObject>(o);
  assert(that); 
  return parent->meetUpdate(that->parent, pedge, ldma->getComposer(), ldma);
}

// Returns whether this AbstractObject denotes the set of all possible execution prefixes.
bool LDMemLocObject::isFull(PartEdgePtr pedge)
{
  return parent->isFull(pedge, ldma->getComposer(), ldma);
}

// Returns whether this AbstractObject denotes the empty set.
bool LDMemLocObject::isEmpty(PartEdgePtr pedge)
{
  return parent->isEmpty(pedge, ldma->getComposer(), ldma);
}

// pretty print for the object
std::string LDMemLocObject::str(std::string indent) const
{
  ostringstream oss;
  
  // Choose the string description to use based on the sub-type of this LDMemLocObject
  string name = "LDML";
  /*     if(dynamic_cast<const LDScalar*>(this))         name = "LDScalar";
  else if(dynamic_cast<const LDFunctionMemLoc*>(this)) name = "LDFunctionMemLoc";
  else if(dynamic_cast<const LDArray*>(this))          name = "LDArray";
  else if(dynamic_cast<const LDPointer*>(this))        name = "LDPointer";*/
  
  oss << "["<<name<<": "<<parent->str("    ")<<"]";
  return oss.str();
}

std::string LDMemLocObject::strp(PartEdgePtr pedge, std::string indent)
{
  ostringstream oss;
  if(isLiveML(pedge))
    oss << "[LDMemLocObject: LIVE: "<<parent->str("    ")<<"]";
  else
    oss << "[LDMemLocObject: DEAD]";
  return oss.str();
}

// Allocates a copy of this object and returns a pointer to it
MemLocObjectPtr LDMemLocObject::copyML() const
{
  return boost::make_shared<LDMemLocObject>(*this);
}

/*
// Creates an instance of an LDMemLocObject that belongs to one of the MemLocObject categories
// (LDMemLocObject sub-classes): LDScalar, LDFunctionMemLoc, LDLabeledAggregate, LDArray or LDPointer.
LDMemLocObjectPtr createLDMemLocObjectCategory(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma)
{
  if(parent->isScalar()) {
    //dbg << "LiveDeadMemAnalysis::createLDMemLocObjectCategory() Scalar"<<endl;
    return boost::make_shared<LDScalar>(n, parent, ldma);
  } else if(parent->isFunctionMemLoc()) {
    //dbg << "LiveDeadMemAnalysis::createLDMemLocObjectCategory() FunctionMemLoc"<<endl;
    return boost::make_shared<LDFunctionMemLoc>(n, parent, ldma);
  } else if(parent->isLabeledAggregate()) {
    //dbg << "LiveDeadMemAnalysis::createLDMemLocObjectCategory() LabeledAggregate"<<endl;
    return boost::make_shared<LDLabeledAggregate>(n, parent, ldma);
  } else if(parent->isArray()) {
    //dbg << "LiveDeadMemAnalysis::createLDMemLocObjectCategory() Array"<<endl;
    return boost::make_shared<LDArray>(n, parent, ldma);
  } else if(parent->isPointer()) {
    //dbg << "LiveDeadMemAnalysis::createLDMemLocObjectCategory() Pointer"<<endl;
    return boost::make_shared<LDPointer>(n, parent, ldma);
  }
  // Control should not reach here
  dbg << "<font color=\"#ff0000\">parent="<<parent->str()<<"</font>"<<endl;
  assert(0);
}

LDScalar::LDScalar(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma)
  : MemLocObject(n), LDMemLocObject(n, parent, ldma)
{ / *dbg << "LDScalar::LDScalar(parent="<<(parent ? parent->str("") : "NULL")<<")"<<endl;* / }

LDFunctionMemLoc::LDFunctionMemLoc(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma)
  :MemLocObject(n),  LDMemLocObject(n, parent, ldma)
{ / *dbg << "LDFunctionMemLoc::LDFunctionMemLoc(parent="<<(parent ? parent->str("") : "NULL")<<")"<<endl;* / }

LDLabeledAggregate::LDLabeledAggregate(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma)
  : MemLocObject(n), LDMemLocObject(n, parent, ldma)
{/ * dbg << "LDLabeledAggregate::LDLabeledAggregate(parent="<<(parent ? parent->str("") : "NULL")<<")"<<endl;* / }

size_t LDLabeledAggregate::fieldCount(PartEdgePtr pedge)
{
  //if(isLiveMay(parent, ldma, part, *NodeState::getNodeState(ldma, part), "")) 
    return parent->isLabeledAggregate()->fieldCount(pedge);
  //return 0;
}

std::list<LabeledAggregateFieldPtr> LDLabeledAggregate::getElements(PartEdgePtr pedge) const
{
  //if(isLiveMay(parent, ldma, part, *NodeState::getNodeState(ldma, part), "")) 
    return parent->isLabeledAggregate()->getElements(pedge);
  / *else {
    std::vector<boost::shared_ptr<LabeledAggregateField> > ret;
    return ret;
  }* /
}

LDArray::LDArray(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma)
  : MemLocObject(n), LDMemLocObject(n, parent, ldma)
{ / *dbg << "LDArray::LDArray(parent="<<(parent ? parent->str("") : "NULL")<<")"<<endl;* / }

// Returns a memory object that corresponds to all the elements in the given array
MemLocObjectPtr LDArray::getElements(PartEdgePtr pedge)
{
  //if(isLiveMay(parent, ldma, part, *NodeState::getNodeState(ldma, part), "")) 
    return createLDMemLocObjectCategory(NULL, parent->isArray()->getElements(pedge), ldma);
  //else return MemLocObjectPtr();
}

// Returns the memory object that corresponds to the elements described by the given abstract index, 
// which represents one or more indexes within the array
MemLocObjectPtr LDArray::getElements(IndexVectorPtr ai, PartEdgePtr pedge)
{
  //if(isLiveMay(parent, ldma, part, *NodeState::getNodeState(ldma, part), "")) 
    return createLDMemLocObjectCategory(NULL, parent->isArray()->getElements(ai, pedge), ldma);
  //else return MemLocObjectPtr();
}

// number of dimensions of the array
size_t LDArray::getNumDims(PartEdgePtr pedge)
{
  if(isLive(pedge)) return parent->isArray()->getNumDims(pedge);
  else return 0;
}

//--- pointer like semantics
// support dereference of array object, similar to the dereference of pointer
// Return the element object: array[0]
MemLocObjectPtr LDArray::getDereference(PartEdgePtr pedge)
{
  //if(isLiveMay(parent, ldma, part, *NodeState::getNodeState(ldma, part), "")) 
    return createLDMemLocObjectCategory(NULL, parent->isArray()->getDereference(pedge), ldma);
  //else return MemLocObjectPtr();
}

LDPointer::LDPointer(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma)
  : MemLocObject(n), LDMemLocObject(n, parent, ldma)
{ / *dbg << "LDPointer::LDPointer(parent="<<(parent ? parent->str("") : "NULL")<<")"<<endl;* / }

MemLocObjectPtr LDPointer::getDereference(PartEdgePtr pedge)
{
  //if(isLiveMay(parent, ldma, part, *NodeState::getNodeState(ldma, part), "")) 
  return createLDMemLocObjectCategory(NULL, parent->isPointer()->getDereference(pedge), ldma);
  //else return MemLocObjectPtr();
}
*/

// Initialize vars to hold all the variables and expressions that are live at PartEdgePtr pedge
void getAllLiveMemAt(LiveDeadMemAnalysis* ldma, PartEdgePtr pedge, const NodeState& state, set<AbstractObjectPtr>& vars, string indent)
{
  //dbg << "getAllLiveVarsAt() n="<<escape(n.getNode()->unparseToString()) << " | " << n.getNode()->class_name()<<" | "<<n.getIndex()<<endl;
  //dbg << "    state.getLatticeAbove(ldma): #="<<state.getLatticeAbove(ldma).size()<<endl;
  //for(vector<Lattice*>::const_iterator lat=state.getLatticeAbove(ldma).begin(); lat!=state.getLatticeAbove(ldma).end(); lat++) {
  //      dbg <<"        lat="<<*lat<<endl;
  //      dbg <<"        lat="<<(*lat)->str("            ")<<endl;
  //}
  //dbg << "    state.getLatticeBelow(ldma): #="<<state.getLatticeBelow(ldma).size()<<endl;
  //for(vector<Lattice*>::const_iterator lat=state.getLatticeBelow(ldma).begin(); lat!=state.getLatticeBelow(ldma).end(); lat++) {
  //      dbg <<"        lat="<<*lat<<endl;
  //      dbg <<"        lat="<<(*lat)->str("            ")<<endl;
  //}
  //dbg << "    state = "<<state.str(ldma, "        ")<<endl;
  //dbg.flush();
      
  /*AbstractObjectSet* liveLAbove = dynamic_cast<AbstractObjectSet*>(*(state.getLatticeAbove(ldma).begin()));
  AbstractObjectSet* liveLBelow = dynamic_cast<AbstractObjectSet*>(*(state.getLatticeBelow(ldma).begin()));

  // The set of live vars AT this node is the union of vars that are live above it and below it
  for(AbstractObjectSet::const_iterator var=liveLAbove->begin(); var!=liveLAbove->end(); var++)
    vars.insert(*var);
  for(AbstractObjectSet::const_iterator var=liveLBelow->begin(); var!=liveLBelow->end(); var++)
    vars.insert(*var);*/
  
  AbstractObjectSet* liveL = dynamic_cast<AbstractObjectSet*>(state.getLatticeAbove(ldma, pedge, 0));
  assert(liveL);
  for(AbstractObjectSet::const_iterator var=liveL->begin(); var!=liveL->end(); var++)
    vars.insert(*var);
}

// Returns the set of variables and expressions that are live at PartEdgePtr pedge
set<AbstractObjectPtr> getAllLiveMemAt(LiveDeadMemAnalysis* ldma, PartEdgePtr pedge, const NodeState& state, string indent)
{
    set<AbstractObjectPtr> vars;
    getAllLiveMemAt(ldma, pedge, state, vars, indent);
    return vars;
}

// Returns true if the given MemLocObject must be live at the given PartEdgePtr pedge
bool isLiveMust(MemLocObjectPtr mem, LiveDeadMemAnalysis* ldma, PartEdgePtr pedge, NodeState& state, string indent)
{
  /*AbstractObjectSet* liveLAbove = dynamic_cast<AbstractObjectSet*>(*(state.getLatticeAbove(ldma).begin()));
  AbstractObjectSet* liveLBelow = dynamic_cast<AbstractObjectSet*>(*(state.getLatticeBelow(ldma).begin()));
  
  //dbg << "isLiveMust: liveLAbove="<<liveLAbove->str("")<<endl;
  //dbg << "isLiveMust: liveLBelow="<<liveLAbove->str("")<<endl;
  
  if(liveLAbove->containsMust(mem)) return true;
  if(liveLBelow->containsMust(mem)) return true;*/
  AbstractObjectSet* liveL = dynamic_cast<AbstractObjectSet*>(state.getLatticeAbove(ldma, pedge, 0));
  assert(liveL);
  if(liveL->containsMust(mem)) return true;
  
  return false;
}

// Returns true if the given MemLocObject may be live at the given PartEdgePtr pedge
bool isLiveMay(MemLocObjectPtr mem, LiveDeadMemAnalysis* ldma, PartEdgePtr pedge, string indent)
{
  scope reg("isLiveMay()", scope::medium, liveDeadAnalysisDebugLevel, 1);
  if(liveDeadAnalysisDebugLevel>=1) {
    dbg << "mem="<<mem->str("")<<endl;
    dbg << "pedge="<<pedge->str()<<endl;
  }
  
  // If this is not a wildcard edge, check if mem is live along it
  if(pedge->source() && pedge->target()) {
    NodeState* state = NodeState::getNodeState(ldma, pedge->target());
    //dbg << "state="<<state->str(ldma)<<endl;
    
    AbstractObjectSet* liveL = dynamic_cast<AbstractObjectSet*>(state->getLatticeAbove(ldma, pedge, 0)); assert(liveL);
    if(liveDeadAnalysisDebugLevel>=1) dbg << "isLiveMay: liveLAbove="<<liveL->str("")<<endl;
    if(liveL->containsMay(mem)) {
      if(liveDeadAnalysisDebugLevel>=1) dbg << "<b>LIVE</b>"<<endl;
      return true;
    }
  // If the source of this edge is a wildcard, mem is live if it is live along any incoming edge or 
  // the outgoing edge (for SgInitializedName we want to say that it is live immediately before its
  // declaration so that clients can ask for its MemLocObject at edges coming into the declaration).
  } else if(pedge->target()) {
    NodeState* state = NodeState::getNodeState(ldma, pedge->target());
    //dbg << "state="<<state->str(ldma)<<endl;
    
    {scope regAbv("Checking containment above", scope::low, liveDeadAnalysisDebugLevel, 1);
    map<PartEdgePtr, std::vector<Lattice*> >& e2lats = state->getLatticeAboveAllMod(ldma);
    assert(e2lats.size()>=1);
    for(map<PartEdgePtr, std::vector<Lattice*> >::iterator lats=e2lats.begin(); lats!=e2lats.end(); lats++) {
      PartEdge* p = lats->first.get();
      assert(p->target() == pedge.get()->target());
      AbstractObjectSet* liveL = dynamic_cast<AbstractObjectSet*>(*(lats->second.begin())); assert(liveL);
      scope regAbvEdge(txt()<<"edge="<<p->str(), scope::low, liveDeadAnalysisDebugLevel, 1);
      if(liveDeadAnalysisDebugLevel>=1) dbg << "liveLAbove="<<liveL->str("")<<endl;
      
      if(liveL->containsMay(mem)) {
        if(liveDeadAnalysisDebugLevel>=1) dbg << "<b>LIVE</b>"<<endl;
        return true;
      }
    }}

    { scope regAbv("Checking containment below", scope::low, liveDeadAnalysisDebugLevel, 1);
    std::vector<Lattice*>& lats = state->getLatticeBelowMod(ldma);
    AbstractObjectSet* liveL = dynamic_cast<AbstractObjectSet*>(*(lats.begin())); assert(liveL);
    if(liveDeadAnalysisDebugLevel>=1) dbg << "isLiveMay: liveLBelow="<<liveL->str("")<<endl;
    //dbg << "isLiveMay: liveLBelow="<<liveL->str("")<<endl;
    if(liveL->containsMay(mem)) {
      if(liveDeadAnalysisDebugLevel>=1) dbg << "<b>LIVE</b>"<<endl;
      return true;
    }}
  // If the target of this edge is a wildcard, mem is live if it is live along any outgoing edge
  } else if(pedge->source()) {
    NodeState* state = NodeState::getNodeState(ldma, pedge->source());
    //dbg << "isLiveMay: state="<<state->str(ldma)<<endl;
    
    std::vector<Lattice*>& lats = state->getLatticeBelowMod(ldma);
    AbstractObjectSet* liveL = dynamic_cast<AbstractObjectSet*>(*(lats.begin()));
    assert(liveL);
    if(liveDeadAnalysisDebugLevel>=1) dbg << "isLiveMay: liveLBelow="<<liveL->str("")<<endl;
    if(liveL->containsMay(mem)) {
      if(liveDeadAnalysisDebugLevel>=1) dbg << "<b>LIVE</b>"<<endl;
      return true;
    }
  }
  
  if(liveDeadAnalysisDebugLevel>=1) dbg << "<b>DEAD</b>"<<endl;
  return false;
}

}; // namespace fuse

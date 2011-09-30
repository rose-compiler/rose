#include "dataflow.h"
#include "latticeFull.h"
#include "liveDeadVarAnalysis.h"

#include <vector>

template <class LatticeType>
class VariableStateTransfer : public IntraDFTransferVisitor
{
protected:
  bool modified;
  void updateModified(bool latModified) { modified = latModified || modified; }

  const int debugLevel;

  FiniteVarsExprsProductLattice* prodLat;

  LatticeType *getLattice(const SgExpression *sgn) {
    return sgn ? getLattice(SgExpr2Var(sgn)) : NULL;
  }
  LatticeType *getLattice(varID var) {
    return dynamic_cast<LatticeType *>(prodLat->getVarLattice(var));
  }

  bool getLattices(const SgBinaryOp *sgn, LatticeType* &arg1Lat, LatticeType* &arg2Lat, LatticeType* &resLat) {
    arg1Lat = getLattice(sgn->get_lhs_operand());
    arg2Lat = getLattice(sgn->get_rhs_operand());
    resLat = getLattice(sgn);

    if(isSgCompoundAssignOp(sgn)) {
      if(resLat==NULL && arg1Lat != NULL)
        resLat = arg1Lat;
    }
    //Dbg::dbg << "transfer B, resLat="<<resLat<<"\n";

    return (arg1Lat && arg2Lat && resLat);
  }
  bool getLattices(const SgUnaryOp *sgn,  LatticeType* &arg1Lat, LatticeType* &arg2Lat, LatticeType* &resLat) {
    arg1Lat = getLattice(sgn->get_operand());
    resLat = getLattice(sgn);

    // Unary Update
    if(isSgMinusMinusOp(sgn) || isSgPlusPlusOp(sgn)) {
      arg2Lat = new LatticeType(1);
    }
    //Dbg::dbg << "res="<<res.str()<<" arg1="<<arg1.str()<<" arg1Lat="<<arg1Lat<<", arg2Lat="<<arg2Lat<<"\n";
    //Dbg::dbg << "transfer B, resLat="<<resLat<<"\n";

    return (arg1Lat && arg2Lat && resLat);
  }

public:
  VariableStateTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo, const int &debugLevel_)
    : IntraDFTransferVisitor(func, n, state, dfInfo), modified(false), debugLevel(debugLevel_), prodLat(dynamic_cast<FiniteVarsExprsProductLattice*>(*(dfInfo.begin())))
  {
    //Dbg::dbg << "transfer A prodLat="<<prodLat<<"="<<prodLat->str("    ")<<"\n";
    // Make sure that all the lattices are initialized
    //prodLat->initialize();
    const std::vector<Lattice*>& lattices = prodLat->getLattices();
    for(std::vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
      (dynamic_cast<LatticeType *>(*it))->initialize();
  }

  void visit(SgAssignOp *sgn)
  {
    LatticeType *lhsLat, *rhsLat, *resLat;
    getLattices(sgn, lhsLat, rhsLat, resLat);
                
    if(debugLevel>=1) {
      if(resLat) Dbg::dbg << "resLat=\n    "<<resLat->str("    ")<<"\n";
      if(lhsLat) Dbg::dbg << "lhsLat=\n    "<<lhsLat->str("    ")<<"\n";
      if(rhsLat) Dbg::dbg << "rhsLat=\n    "<<rhsLat->str("    ")<<"\n";
    }

    // Copy the lattice of the right-hand-side to both the left-hand-side variable and to the assignment expression itself
    if(resLat) // If the left-hand-side contains a live expression or variable
      { resLat->copy(rhsLat); modified = true; }
    if(lhsLat) // If the left-hand-side contains a live expression or variable
      { lhsLat->copy(rhsLat); modified = true; }
  }

  void visit(SgAssignInitializer *sgn)
  {
    LatticeType* asgnLat = getLattice(sgn->get_operand());
    LatticeType* resLat = getLattice(sgn);

    if(debugLevel>=1) {
      if(asgnLat) Dbg::dbg << "asgnLat=    "<<asgnLat->str("    ")<<"\n";
      if(resLat) Dbg::dbg << "resLat=    "<<resLat->str("    ")<<"\n";
    }

    // If the result expression is live
    if(resLat) { resLat->copy(asgnLat); modified = true; }
  }

  // XXX: Right now, we take the meet of all of the elements of the
  // initializer. This could be enhanced with an improved memory
  // abstraction to treat each element individually.
  void visit(SgAggregateInitializer *sgn)
  {
    LatticeType *res = getLattice(sgn);
    SgExpressionPtrList &inits = sgn->get_initializers()->get_expressions();
    if (inits.size() > 0) {
      res->copy(getLattice(inits[0]));
      modified = true;
      for (int i = 1; i < inits.size(); ++i)
        res->meetUpdate(getLattice(inits[i]));
    }
  }

  // XXX: This needs to be handled by an inter-procedural analysis
  void visit(SgConstructorInitializer *sgn)
  { }

  // XXX: I don't even know what this is - Phil
  void visit(SgDesignatedInitializer *sgn)
  { }

  void visit(SgInitializedName *initName)
  {
    LatticeType* varLat = getLattice(initName);

    if(varLat) {
      LatticeType* initLat = getLattice(initName->get_initializer());
      // If there was no initializer, leave this in its default 'bottom' state
      if(initLat) {
        varLat->copy(initLat);
        modified = true;
      }
    }
  }

  void visit(SgBinaryOp *sgn) {
    LatticeType *lhs, *rhs, *res;
    getLattices(sgn, lhs, rhs, res);
    if (res) {
      res->copy(lhs);
      res->meetUpdate(rhs);
      modified = true;
    }
  }

  void visit(SgCompoundAssignOp *sgn) {
    LatticeType *lhs, *rhs, *res;
    getLattices(sgn, lhs, rhs, res);
    if (lhs)
      updateModified(lhs->meetUpdate(rhs));
    // Liveness of the result implies liveness of LHS
    if (res) {
      res->copy(lhs);
      modified = true;
    }
  }

  void visit(SgCommaOpExp *sgn)
  {
    LatticeType *lhsLat, *rhsLat, *resLat;
    getLattices(sgn, lhsLat, rhsLat, resLat);
                
    if (resLat) {
      resLat->copy(rhsLat);
      modified = true;
    }
  }

  void visit(SgConditionalExp *sgn)
  {
    LatticeType *condLat = getLattice(sgn->get_conditional_exp()),
                *trueLat = getLattice(sgn->get_true_exp()),
                *falseLat = getLattice(sgn->get_false_exp()),
                *resLat = getLattice(sgn);

    // Liveness of the result implies liveness of the input expressions
    if (resLat) {
      resLat->copy(condLat);
      resLat->meetUpdate(trueLat);
      resLat->meetUpdate(falseLat);
      modified = true;
    }
  }

  void visit(SgScopeOp *)
  {
    // Documentation says this is no longer used, so explicitly fail if we see it
    assert(0);
  }

  void visit(SgBitComplementOp *sgn)
  {
    LatticeType *res = getLattice(sgn);
    if (res) {
      res->copy(getLattice(sgn->get_operand()));
      modified = true;
    }
  }
};

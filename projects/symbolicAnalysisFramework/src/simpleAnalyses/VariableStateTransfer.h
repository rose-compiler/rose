#include "dataflow.h"
#include "latticeFull.h"

template <class LatticeType>
class VariableStateTransfer : public IntraDFTransferVisitor
{
protected:
  bool modified;
  void updateModified(bool latModified) { modified = latModified || modified; }

  const int debugLevel;

  FiniteVarsExprsProductLattice* prodLat;

  LatticeType *getLattice(const SgExpression *sgn) {
    return dynamic_cast<LatticeType *>(prodLat->getVarLattice(SgExpr2Var(sgn)));
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
  VariableStateTransfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo, const int &debugLevel_)
    : IntraDFTransferVisitor(func, n, state, dfInfo), modified(false), debugLevel(debugLevel_), prodLat(dynamic_cast<FiniteVarsExprsProductLattice*>(*(dfInfo.begin())))
  {
    //Dbg::dbg << "transfer A prodLat="<<prodLat<<"="<<prodLat->str("    ")<<"\n";
    // Make sure that all the lattices are initialized
    //prodLat->initialize();
    const vector<Lattice*>& lattices = prodLat->getLattices();
    for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
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

  void visit(SgInitializedName *initName)
  {
    varID var(initName);
    LatticeType* varLat = dynamic_cast<LatticeType *>(prodLat->getVarLattice(var));
		
    // if this is a scalar that we care about, initialize it to Bottom
    if(varLat) {
      // If there was no initializer
      if(initName->get_initializer()==NULL)
        updateModified(varLat->setBot());
      else {
        LatticeType* initLat = getLattice(initName->get_initializer());
        if(initLat) {
          varLat->copy(initLat);
          modified = true;
        }
      }
    }
  }


};

#ifndef DFTRANSFERFUNCTIONS_HH
#define DFTRANSFERFUNCTIONS_HH

template<LatticeType>
class DFTransferFunctions {
public:
  virtual LatticeType transfer(Label lab, LatticeType element);
protected:
  virtual void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, RDLattice& element)=0;
  virtual void transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, RDLattice& element);
  virtual void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, RDLattice& element);
  virtual void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, RDLattice& element);
  virtual void transferDeclaration(Label label, SgVariableDeclaration* decl, RDLattice& element);
  virtual void transferExpression(Label label, SgExpression* expr, RDLattice& element);
};

#endif

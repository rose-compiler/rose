template<typename Domain<Lattice>, typename PropertyState >
class CppExprEvaluatorX : public AstBottomUpProcessing<Lattice> {
 public:
  virtual Lattice evaluateSynthesizedAttribute(SgNode* node, AstBottomUpProcessing<Lattice>::SynthesizedAttributesList clist) {
    switch(node->variantT()) {
    case V_SgIntVal: return domain->intValue(isSgIntVal(node)->get_value());
    case V_SgAddOp:  return domain->add(clist[0],clist[1]);
    case V_SgSubtractOp: return domain->sub(clist[0],clist[1]);
    case V_SgMultiplyOp: return domain->mul(clist[0],clist[1]);
    case V_SgDivideOp: return domain->div(clist[0],clist[1]);
    case V_SgModOp: return domain->mod(clist[0],clist[1]);
    case V_SgMinusOp: return domain->neg(clist[0]);
    case V_SgVarRefExp: {return propertyState->getVarValue(variableIdMapping->variableId(node));}
    case V_SgAssignOp:
    default:
      return Lattice();
    }
    return Lattice();
  }
  void setDomain(Domain<Lattice>* domain) { this->domain=domain; }
  void setPropertyState(Domain<Lattice>* pstate) { this->propertyState=pstate; }
  void setVariableIdMapping(VariableIdMapping variableIdMapping) { this->variableIdMapping=variableIdMapping; }
private:
  Domain<Lattice>* domain;
  PropertyState* propertyState;
  VariableIdMapping* variableIdMapping;
};

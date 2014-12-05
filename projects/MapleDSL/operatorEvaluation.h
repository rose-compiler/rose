
// Forward class declarations.
class DiscretizationFSM;
class OperatorFSM;

class OperatorEvaluation_InheritedAttribute
   {
     private:
          bool isShiftExpression;

     public:
          DiscretizationFSM* discretizationFSM;

          OperatorEvaluation_InheritedAttribute();
          OperatorEvaluation_InheritedAttribute( const OperatorEvaluation_InheritedAttribute & X );

          void set_ShiftExpression(bool value);
          bool get_ShiftExpression();
   };

class OperatorEvaluation_SynthesizedAttribute
   {
     private:
          bool operatorTransformed;

     public:
          SgNode* node;

          DiscretizationFSM* discretizationFSM;

          std::vector<OperatorFSM> operatorList;

       // The Operator to apply and the operand to apply it to (rhs).
          OperatorFSM operatorApplication;
          SgExpression* operatorApplicationOperand;
          SgExpression* operatorApplicationLhs;

     public:
          OperatorEvaluation_SynthesizedAttribute();
       // OperatorEvaluation_SynthesizedAttribute( SgNode* n );
          OperatorEvaluation_SynthesizedAttribute( const OperatorEvaluation_SynthesizedAttribute & X );

          void set_operatorTransformed(bool value);
          bool get_operatorTransformed();
   };

class OperatorEvaluationTraversal : public SgTopDownBottomUpProcessing<OperatorEvaluation_InheritedAttribute,OperatorEvaluation_SynthesizedAttribute>
   {
     private:
#if 0
       // std::vector<SgInitializedName*> initializedNameList;
       // We want a map of vectors of inputs to stencil declarations later.
       // std::map<SgInitializedName*, std::vector<SgVarRef*> > stencilInputList;
       // std::vector<SgVarRefExp*> stencilInputList;
       // std::vector<SgExpression*>      stencilInputExpressionList;
          std::vector<SgInitializedName*> stencilInputInitializedNameList;
          std::vector<SgInitializedName*> stencilOperatorInitializedNameList;
          std::vector<SgFunctionCallExp*> stencilOperatorFunctionCallList;
#endif

       // This is the list of locations in the AST where the operator is called (typically once, but could be many).
          std::vector<SgFunctionCallExp*> operatorFunctionCallList;

       // This is the map of stencil offsets, keys are generated from the names of the variables.
       // The simple rules for the specification of stencils should not allow "Point" variable
       // names to shadow one another in nested scopes (else we would have to use more complex
       // keys with names generated from name qualification, of using the unique name generator,
       // etc., for now we keep it simple).
          std::map<std::string,DiscretizationFSM*> discretizationMap;

       // This is the map of all stencils (there can be more than one).
       // The key is the name associated with the variable that is built of type Stencil.
          std::map<std::string,OperatorFSM*> operatorMap;

     public:
       // Functions required to overload the pure virtual functions in the abstract base class.
          OperatorEvaluation_InheritedAttribute   evaluateInheritedAttribute   (SgNode* astNode, OperatorEvaluation_InheritedAttribute inheritedAttribute );
          OperatorEvaluation_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, OperatorEvaluation_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

       // This traversal takes no operands.
          OperatorEvaluationTraversal();

          void displayOperator(const std::string & label);

          std::vector<SgFunctionCallExp*> & get_operatorFunctionCallList();

          std::map<std::string,OperatorFSM*> & get_operatorMap();
   };


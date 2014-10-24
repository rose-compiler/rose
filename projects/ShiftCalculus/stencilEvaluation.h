// Forward class declarations.
class StencilOffsetFSM;
class StencilFSM;

class StencilEvaluation_InheritedAttribute
   {
     private:
          bool isShiftExpression;

     public:
          StencilOffsetFSM* stencilOffsetFSM;
          double stencilCoeficientValue;

          StencilEvaluation_InheritedAttribute();
          StencilEvaluation_InheritedAttribute( const StencilEvaluation_InheritedAttribute & X );

          void set_ShiftExpression(bool value);
          bool get_ShiftExpression();
   };

class StencilEvaluation_SynthesizedAttribute
   {
     private:
          bool stencilOperatorTransformed;


     public:
          SgNode* node;

          StencilOffsetFSM* stencilOffsetFSM;
          double stencilCoeficientValue;

     public:
          StencilEvaluation_SynthesizedAttribute();
          StencilEvaluation_SynthesizedAttribute( SgNode* n );
          StencilEvaluation_SynthesizedAttribute( const StencilEvaluation_SynthesizedAttribute & X );

          void set_stencilOperatorTransformed(bool value);
          bool get_stencilOperatorTransformed();
   };

class StencilEvaluationTraversal : public SgTopDownBottomUpProcessing<StencilEvaluation_InheritedAttribute,StencilEvaluation_SynthesizedAttribute>
   {
     private:
       // std::vector<SgInitializedName*> initializedNameList;
       // We want a map of vectors of inputs to stencil declarations later.
       // std::map<SgInitializedName*, std::vector<SgVarRef*> > stencilInputList;
       // std::vector<SgVarRefExp*> stencilInputList;
       // std::vector<SgExpression*>      stencilInputExpressionList;
          std::vector<SgInitializedName*> stencilInputInitializedNameList;
          std::vector<SgInitializedName*> stencilOperatorInitializedNameList;
          std::vector<SgFunctionCallExp*> stencilOperatorFunctionCallList;

       // This is the map of stencil offsets, keys are generated from the names of the variables.
       // The simple rules for the specification of stencils should not allow "Point" variable
       // names to shadow one another in nested scopes (else we would have to use more complex
       // keys with names generated from name qualification, of using the unique name generator,
       // etc., for now we keep it simple).
          std::map<std::string,StencilOffsetFSM*> StencilOffsetMap;

       // This is the map of all stencils (there can be more than one).
       // The key is the name associated with the variable that is built of type Stencil.
          std::map<std::string,StencilFSM*> stencilMap;


     public:
       // Functions required to overload the pure virtual functions in the abstract base class.
          StencilEvaluation_InheritedAttribute   evaluateInheritedAttribute   (SgNode* astNode, StencilEvaluation_InheritedAttribute inheritedAttribute );
          StencilEvaluation_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, StencilEvaluation_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

       // This traversal takes the result of the previous traversal.
          StencilEvaluationTraversal(DetectionTraversal & result);

          void displayStencil(const std::string & label);

          std::vector<SgFunctionCallExp*> & get_stencilOperatorFunctionCallList();

          std::map<std::string,StencilFSM*> & get_stencilMap();

   };


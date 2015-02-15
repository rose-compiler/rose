class Detection_InheritedAttribute
   {
     private:
          bool isShiftExpression;

          bool isStencilDeclaration;
          bool isStencilOperatorDeclaration;
          bool isStencilOperatorRef;

     public:
          Detection_InheritedAttribute();
          Detection_InheritedAttribute( const Detection_InheritedAttribute & X );

          void set_ShiftExpression(bool value);
          bool get_ShiftExpression();

          void set_StencilDeclaration(bool value);
          bool get_StencilDeclaration();
          void set_StencilOperatorDeclaration(bool value);
          bool get_StencilOperatorDeclaration();

          void set_StencilOperatorRef(bool value);
          bool get_StencilOperatorRef();
   };

class Detection_SynthesizedAttribute
   {
     private:
          bool stencilOperatorTransformed;

     public:
          SgNode* node;

     public:
          Detection_SynthesizedAttribute();
          Detection_SynthesizedAttribute( SgNode* n );
          Detection_SynthesizedAttribute( const Detection_SynthesizedAttribute & X );

          void set_stencilOperatorTransformed(bool value);
          bool get_stencilOperatorTransformed();
   };

class DetectionTraversal : public SgTopDownBottomUpProcessing<Detection_InheritedAttribute,Detection_SynthesizedAttribute>
   {
     private:
          std::vector<SgInitializedName*> initializedNameList;
       // We want a map of vectors of inputs to stencil declarations later.
       // std::map<SgInitializedName*, std::vector<SgVarRef*> > stencilInputList;
       // std::vector<SgVarRefExp*> stencilInputList;
          std::vector<SgExpression*>      stencilInputExpressionList;
          std::vector<SgInitializedName*> stencilInputInitializedNameList;

       // This is a list of the stencil objects found (there could be more than one).
          std::vector<SgInitializedName*> stencilInitializedNameList;
          std::vector<SgInitializedName*> stencilOperatorInitializedNameList;
          std::vector<SgFunctionCallExp*> stencilOperatorFunctionCallList;

     public:
       // Functions required to overload the pure virtual functions in the abstract base class.
          Detection_InheritedAttribute   evaluateInheritedAttribute   (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute );
          Detection_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

          DetectionTraversal();

          std::vector<SgInitializedName*> & get_stencilInputInitializedNameList();
          std::vector<SgInitializedName*> & get_stencilOperatorInitializedNameList();

          std::vector<SgFunctionCallExp*> & get_stencilOperatorFunctionCallList();

       // Output debugging data.
          void display();
   };


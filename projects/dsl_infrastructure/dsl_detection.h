
// We define a traversal here to detect the relevant attributes in the
// input application's AST and add attributes as required.

class Detection_InheritedAttribute
   {
     private:
       // These are DSL abstraction specific, see if we can work without them.
       // bool isShiftExpression;
       // bool isStencilDeclaration;
       // bool isStencilOperatorDeclaration;
       // bool isStencilOperatorRef;

     public:
       // DQ (2/18/2015): Added to support debugging by restricting traversals to main function.
          bool inFunction;
          bool inDSL_Function;

     public:
          Detection_InheritedAttribute();
          Detection_InheritedAttribute( const Detection_InheritedAttribute & X );

#if 0
          void set_ShiftExpression(bool value);
          bool get_ShiftExpression();

          void set_StencilDeclaration(bool value);
          bool get_StencilDeclaration();
          void set_StencilOperatorDeclaration(bool value);
          bool get_StencilOperatorDeclaration();

          void set_StencilOperatorRef(bool value);
          bool get_StencilOperatorRef();
#endif
   };

class Detection_SynthesizedAttribute
   {
     private:
          bool stencilOperatorTransformed;

     public:
          SgNode* node;

       // std::vector<SgVarRefExp*> boxVarRefExpList;
       // std::vector<SgVarRefExp*> rectMDArrayVarRefExpList;
       // std::vector<SgVarRefExp*> stencilVarRefExpList;

          std::vector<SgNode*> dslChildren;

     public:
          Detection_SynthesizedAttribute();
          Detection_SynthesizedAttribute( SgNode* n );
          Detection_SynthesizedAttribute( const Detection_SynthesizedAttribute & X );

       // void set_stencilOperatorTransformed(bool value);
       // bool get_stencilOperatorTransformed();
   };

class DetectionTraversal : public SgTopDownBottomUpProcessing<Detection_InheritedAttribute,Detection_SynthesizedAttribute>
   {
     private:
#if 0
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

          std::vector<SgInitializedName*> pointInitializedNameList;
          std::vector<SgInitializedName*> arrayInitializedNameList;
          std::vector<SgInitializedName*> RectMDArrayInitializedNameList;
          std::vector<SgInitializedName*> boxInitializedNameList;
#endif
       // Root of the AST being traversed to support detection of DSL. Required to support 
       // recursive traversals over the whole of the AST to find constant expressions identified 
       // with variable used in DSL expressions.  I think that the variable numbering might 
       // eliminate or simplify this step.
          SgNode* root;

     public:
       // Functions required to overload the pure virtual functions in the abstract base class.
          Detection_InheritedAttribute   evaluateInheritedAttribute   (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute );
          Detection_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

          void findAndAddAttributeToConstantExpression ( SgFunctionCallExp* functionCallExp );

       // DQ: added new version of this function.
          void findAndAddAttributeToConstantExpression ( SgConstructorInitializer* constructorInializer );

#if 0
          static const std::string PointVariable;
          static const std::string BoxVariable;
          static const std::string ShiftVariable;
          static const std::string ArrayVariable;
          static const std::string StencilVariable;
          static const std::string RectMDArrayVariable;
          static const std::string BLIteratorVariable;


       // static const std::string scalar_multiply_name;
          static const std::string PointOperatorMultiply;
          static const std::string ScalarShiftOperatorMultiply;
          static const std::string PointShiftOperatorMultiply;
          static const std::string OperatorZero;
          static const std::string OperatorOnes;
          static const std::string OperatorUnit;
          static const std::string ForLoopOperator;
          static const std::string GrowOperator;
          static const std::string ConstantExpression;
          static const std::string OperatorShiftVec;

          static const std::string MakePairConstructor;
          static const std::string StencilApplyOperator;
          static const std::string StencilOrEqualOperator;

          static const std::string StencilOperatorParenthesis;
          static const std::string StencilOperatorPlus;
          static const std::string OperatorSquareBrackets;
          static const std::string GetBox;
          static const std::string OperatorStar;
          static const std::string OperatorMinusEqual;
          static const std::string OperatorPlusEqual;

          static const std::string IntegerValue;
          static const std::string DoubleValue;
#endif

          DetectionTraversal(SgNode* root);

#if 0
          std::vector<SgInitializedName*> & get_stencilInputInitializedNameList();
          std::vector<SgInitializedName*> & get_stencilOperatorInitializedNameList();

          std::vector<SgFunctionCallExp*> & get_stencilOperatorFunctionCallList();
#endif

       // Output debugging data.
          void display();
   };




class InheritedAttribute
   {
          bool isSharedTypeExpression;

     public:
          InheritedAttribute();
          InheritedAttribute( const InheritedAttribute & X );

          void set_SharedTypeExpression(bool value);
          bool get_SharedTypeExpression();
   };

class SynthesizedAttribute
   {
     private:
          bool isSharedTypeExpression;
          bool isMarkedForTransformation;

     public:
          SgNode* node;

     public:
          SynthesizedAttribute();
          SynthesizedAttribute( SgNode* n );
          SynthesizedAttribute( const SynthesizedAttribute & X );

          void set_SharedTypeExpression(bool value);
          bool get_SharedTypeExpression();
          void set_MarkedForTransformation(bool value);
          bool get_MarkedForTransformation();
   };

class Traversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     private:
          std::vector<SgNode*> nodeListWithTypesToModify;

          SgVariableDeclaration* supportingOffset;

     public:
       // Functions required to overload the pure virtual functions in the abstract base class.
          InheritedAttribute evaluateInheritedAttribute (SgNode* astNode, InheritedAttribute inheritedAttribute );
          SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

          Traversal();

          std::vector<SgNode*> & get_nodeListWithTypesToModify();
          bool isSharedType(SgType* type);
          void transformType(SgType* type);
       // SgExpression* transformExpression(SgVarRefExp* varRefExp);
          SgExpression* transformExpression(SgExpression* exp);
   };





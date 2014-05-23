

class InheritedAttribute
   {
     public:
          InheritedAttribute();
   };

class SynthesizedAttribute
   {
     public:
          SynthesizedAttribute();
   };

class Traversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     private:
          std::vector<SgNode*> nodeListWithTypesToModify;

     public:
       // Functions required to overload the pure virtual functions in the abstract base class.
          InheritedAttribute evaluateInheritedAttribute (SgNode* astNode, InheritedAttribute inheritedAttribute );
          SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

          std::vector<SgNode*> & get_nodeListWithTypesToModify();
          bool isSharedType(SgType* type);
          void transformType(SgType* type);
       // SgExpression* transformExpression(SgVarRefExp* varRefExp);
          SgExpression* transformExpression(SgExpression* exp);
   };





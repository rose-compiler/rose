class TypeTraversalInheritedAttribute
   {
     public:
          TypeTraversalInheritedAttribute();
   };

class TypeTraversalSynthesizedAttribute
   {
     public:
          TypeTraversalSynthesizedAttribute();
   };

class TypeTraversal : public SgTopDownBottomUpProcessing<TypeTraversalInheritedAttribute,TypeTraversalSynthesizedAttribute>
   {
     private:
          std::vector<SgNode*> nodeListWithTypesToModify;

     public:
       // Functions required to overload the pure virtual functions in the abstract base class.
          TypeTraversalInheritedAttribute evaluateInheritedAttribute (SgNode* astNode, TypeTraversalInheritedAttribute inheritedAttribute );
          TypeTraversalSynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, TypeTraversalInheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

          void transformType(SgType* type);
   };


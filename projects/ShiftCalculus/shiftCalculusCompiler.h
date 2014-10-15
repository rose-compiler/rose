
class InheritedAttribute
   {
     private:
          bool isShiftExpression;

     public:
          InheritedAttribute();
          InheritedAttribute( const InheritedAttribute & X );

          void set_ShiftExpression(bool value);
          bool get_ShiftExpression();
   };

class SynthesizedAttribute
   {
     private:
          bool isShiftExpression;

     public:
          SgNode* node;

     public:
          SynthesizedAttribute();
          SynthesizedAttribute( SgNode* n );
          SynthesizedAttribute( const SynthesizedAttribute & X );

          void set_ShiftExpression(bool value);
          bool get_ShiftExpression();
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

          SgExpression* transformExpression(SgExpression* exp);
   };



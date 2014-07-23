

class InheritedAttribute
   {
     private:
          bool isSharedTypeExpression;

       // DQ (7/16/2014): Shared pointers in function parameters should suppress
       // the transformation of shared pointers in function call expressions.
       // Similarly shared pointer initialization should supress their transformations (see test2014_59.c)
          bool suppressTransformation;

       // For function call parameters of shared types, we need to supress transformations.  so we need to 
       // record in the inheritedAttribute if this is from a function call expression so that we can 
       // evaluate the associated types of the expressions in the associated function type for where we
       // need to supress the transformations.
          bool inFunctionCallArgumentList;

     public:
          InheritedAttribute();
          InheritedAttribute( const InheritedAttribute & X );

          void set_SharedTypeExpression(bool value);
          bool get_SharedTypeExpression();
          void set_suppressTransformation(bool value);
          bool get_suppressTransformation();
          void set_inFunctionCallArgumentList(bool value);
          bool get_inFunctionCallArgumentList();
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





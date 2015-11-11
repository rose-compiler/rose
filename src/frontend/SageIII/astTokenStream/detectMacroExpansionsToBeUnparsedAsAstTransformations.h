
// Header file

#if 1
class DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute
   {
  // I don't think there is anything to do for this case (but I have implemented this as a 
  // SgTopDownBottomUpProcessing in case there is a requirement for the inherited attribute.
     public:
          DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute();
          DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute( const DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute & X );
   };


class DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute
   {
     private:

     public:
          SgNode* node;

          DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute();
          DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute( SgNode* n );
          DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute( const DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute & X );
   };


class DetectMacroExpansionsToBeUnparsedAsAstTransformations 
   : public SgTopDownBottomUpProcessing<DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute,
                                        DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute>
   {
     public:
          std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap;

       // DetectMacroExpansionsToBeUnparsedAsAstTransformations(SgSourceFile* sourceFile);
          DetectMacroExpansionsToBeUnparsedAsAstTransformations( std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & input_tokenStreamSequenceMap );

       // virtual function must be defined
          DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute evaluateInheritedAttribute(SgNode* n, DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute inheritedAttribute);

       // virtual function must be defined
          DetectMacroExpansionsToBeUnparsedAsAstTransformationsSynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, DetectMacroExpansionsToBeUnparsedAsAstTransformationsInheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );
   };
#endif


#if 0
class DetectMacroExpansionsToBeUnparsedAsAstTransformations : public AstSimpleProcessing
   {
     public:
       // virtual function must be defined
          void visit( SgNode* n );
   };
#endif


// Main API function to call the AST traversals
void detectMacroExpansionsToBeUnparsedAsAstTransformations ( SgSourceFile* sourceFile );

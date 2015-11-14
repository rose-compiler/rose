
// Header file

class DetectMacroOrIncludeFileExpansionsInheritedAttribute
   {
  // I don't think there is anything to do for this case (but I have implemented this as a 
  // SgTopDownBottomUpProcessing in case there is a requirement for the inherited attribute.
     public:
          DetectMacroOrIncludeFileExpansionsInheritedAttribute();
          DetectMacroOrIncludeFileExpansionsInheritedAttribute( const DetectMacroOrIncludeFileExpansionsInheritedAttribute & X );
   };

class DetectMacroOrIncludeFileExpansionsSynthesizedAttribute
   {
     private:

     public:
          SgNode* node;

          DetectMacroOrIncludeFileExpansionsSynthesizedAttribute();
          DetectMacroOrIncludeFileExpansionsSynthesizedAttribute( SgNode* n );
          DetectMacroOrIncludeFileExpansionsSynthesizedAttribute( const DetectMacroOrIncludeFileExpansionsSynthesizedAttribute & X );
   };

class DetectMacroOrIncludeFileExpansions 
   : public SgTopDownBottomUpProcessing<DetectMacroOrIncludeFileExpansionsInheritedAttribute,
                                        DetectMacroOrIncludeFileExpansionsSynthesizedAttribute>
   {
     public:
          std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap;

       // DetectMacroOrIncludeFileExpansions(SgSourceFile* sourceFile);
          DetectMacroOrIncludeFileExpansions( std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & input_tokenStreamSequenceMap );

       // virtual function must be defined
          DetectMacroOrIncludeFileExpansionsInheritedAttribute evaluateInheritedAttribute(SgNode* n, DetectMacroOrIncludeFileExpansionsInheritedAttribute inheritedAttribute);

       // virtual function must be defined
          DetectMacroOrIncludeFileExpansionsSynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, DetectMacroOrIncludeFileExpansionsInheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );
   };



// Main API function to call the AST traversals
void detectMacroOrIncludeFileExpansions ( SgSourceFile* sourceFile );

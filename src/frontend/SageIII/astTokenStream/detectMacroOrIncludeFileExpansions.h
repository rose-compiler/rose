
// Header file

class MacroExpansion
   {
     public:
       // The name of the macro that is expanded.
          std::string macro_name;

       // Mark this as a shared macro expansion if multiple statements are associated with it.
          bool shared;

       // Save the position of the macro expansion so that we can distinquish between different macro expansions.
          int line;
          int column;

       // The starting and ending positions of the macro call in the token sequence.
          int token_start;
          int token_end;

       // Use a vector as a container for the associated IR nodes for this macro expansion when it is shared.
          std::vector<SgStatement*> associatedStatementVector;

       // Mark the macro expansion as transformed if all of it's statements have been marked as transformed 
       // as part of processing in SageInterface::resetInternalMapsForTargetStatement().
          bool isTransformed;

          MacroExpansion(const std::string & name);

   };


class DetectMacroOrIncludeFileExpansionsInheritedAttribute
   {
  // I don't think there is anything to do for this case (but I have implemented this as a 
  // SgTopDownBottomUpProcessing in case there is a requirement for the inherited attribute).
     public:
          MacroExpansion* macroExpansion;

          DetectMacroOrIncludeFileExpansionsInheritedAttribute();
          DetectMacroOrIncludeFileExpansionsInheritedAttribute( const DetectMacroOrIncludeFileExpansionsInheritedAttribute & X );
   };

class DetectMacroOrIncludeFileExpansionsSynthesizedAttribute
   {
     private:

     public:
          SgNode* node;
          MacroExpansion* macroExpansion;

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
          SgSourceFile* sourceFile;

          std::vector<MacroExpansion*> macroExpansionStack;

       // DetectMacroOrIncludeFileExpansions(SgSourceFile* sourceFile);
       // DetectMacroOrIncludeFileExpansions( std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & input_tokenStreamSequenceMap );
          DetectMacroOrIncludeFileExpansions( SgSourceFile* input_sourceFile, std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & input_tokenStreamSequenceMap );

       // virtual function must be defined
          DetectMacroOrIncludeFileExpansionsInheritedAttribute evaluateInheritedAttribute(SgNode* n, DetectMacroOrIncludeFileExpansionsInheritedAttribute inheritedAttribute);

       // virtual function must be defined
          DetectMacroOrIncludeFileExpansionsSynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, DetectMacroOrIncludeFileExpansionsInheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

       // Support function
          MacroExpansion* isPartOfMacroExpansion( SgStatement* statement, std::string & name, int & startingToken, int & endingToken );
   };



// Main API function to call the AST traversals
void detectMacroOrIncludeFileExpansions ( SgSourceFile* sourceFile );

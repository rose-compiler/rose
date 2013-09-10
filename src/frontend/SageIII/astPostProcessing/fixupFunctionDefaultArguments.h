#ifndef FIXUP_FUNCTION_DEFAULT_ARGUMENTS_H
#define FIXUP_FUNCTION_DEFAULT_ARGUMENTS_H

void fixupFunctionDefaultArguments( SgNode* node );

//! Inherited attribute required for FixupFunctionDefaultArguments class.
class FixupFunctionDefaultArgumentsInheritedAttribute
   {
     public:
          FixupFunctionDefaultArgumentsInheritedAttribute();
   };

// DQ (4/24/2013): This class controls the output of template declarations in the generated code (by the unparser).
class FixupFunctionDefaultArguments
   : public SgTopDownProcessing<FixupFunctionDefaultArgumentsInheritedAttribute>
   {
     public:
      //! Saved reference to SgFile (where the template instatiation data is held)
          SgSourceFile* currentFile;

          class SetStructure
             {
               public:
                  int distanceInSourceSequence;
                  SgFunctionDeclaration* associatedFunctionDeclaration;
                  std::set<SgFunctionDeclaration*> setOfFunctionDeclarations;
             };

          std::map<SgFunctionDeclaration*,SetStructure*> mapOfSetsOfFunctionDeclarations;

      //! Constructor to provide access to file's backend specific template instantiation options
          FixupFunctionDefaultArguments(SgSourceFile* file);
 
      //! Required traversal function
          FixupFunctionDefaultArgumentsInheritedAttribute
               evaluateInheritedAttribute ( SgNode* node, FixupFunctionDefaultArgumentsInheritedAttribute inheritedAttribute );
   };


// endif for FIXUP_FUNCTION_DEFAULT_ARGUMENTS_H
#endif

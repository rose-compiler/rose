// This AST postprocessing fixes some known way in which include files
// can be used to initialized variable declarations.
//
//  For example: 
//     int array[] = 
//     #include<numbers.h>
//     ;
//
//  See Cxx_tests/test2020_46.C for an example.
//

#ifndef FIXUP_INITIALIZERS_H
#define FIXUP_INITIALIZERS_H

// DQ (8/25/2020):
/*! \brief Fixup known ways in which inlcude files are used to initialized variable.

    \implementation This focused on #include files used to initialize variables.
 */
// void fixupInitializersUsingIncludeFiles (SgNode* node);
void fixupInitializersUsingIncludeFiles (SgProject* node);



//! Inherited attribute required for FixupInitializers class.
class FixupInitializersUsingIncludeFilesInheritedAttribute
   {
     public:
          bool isInsideVariableDeclaration;
          SgVariableDeclaration* variableDeclaration;

          bool isInsideInitializer;
          SgInitializedName* initializedName;

          FixupInitializersUsingIncludeFilesInheritedAttribute();
          FixupInitializersUsingIncludeFilesInheritedAttribute( const FixupInitializersUsingIncludeFilesInheritedAttribute & X );
   };

class FixupInitializersUsingIncludeFilesSynthesizedAttribute
   {
     public:
          FixupInitializersUsingIncludeFilesSynthesizedAttribute();
          FixupInitializersUsingIncludeFilesSynthesizedAttribute( const FixupInitializersUsingIncludeFilesSynthesizedAttribute & X );
   };




/*! \brief Fixup known ways in which inlcude files are used to initialized variable.

    This is lower level support for the fixupInitializersUsingIncludeFiles(SgNode*) function.

 */
class FixupInitializersUsingIncludeFilesTraversal 
   : public SgTopDownBottomUpProcessing<FixupInitializersUsingIncludeFilesInheritedAttribute,FixupInitializersUsingIncludeFilesSynthesizedAttribute>
   {
     public:
          FixupInitializersUsingIncludeFilesTraversal();

       // refactored code.
          void findAndRemoveMatchingInclude(SgStatement* statement, SgExpression* expression, PreprocessingInfo::RelativePositionType location_to_search);

       // Required traversal function
          FixupInitializersUsingIncludeFilesInheritedAttribute
               evaluateInheritedAttribute ( SgNode* node, FixupInitializersUsingIncludeFilesInheritedAttribute inheritedAttribute );

       // Required traversal function
          FixupInitializersUsingIncludeFilesSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             FixupInitializersUsingIncludeFilesInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// endif for FIXUP_INITIALIZERS_H
#endif


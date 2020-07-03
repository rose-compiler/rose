#ifndef ADD_INCLUDE_DIRECTIVES_H
#define ADD_INCLUDE_DIRECTIVES_H

// DQ (5/7/2020): Iterate of the AST and add the include directives associated with 
// SgIncludeFiles that were collected in the generation of the AST.


// DQ (5/7/2020):
/*! \brief Add include directives to the AST.

    Include files are collected in the EDG/ROSE translation, the include files are not 
    in the AST unless we traverse the AST and add then explicitly.  The goal is to simplify 
    the handling of include directives seperately from other CPP directives.

   \internal This post processing needs to be run before unparsing.

   \todo Need to consider how this might effect unparsing of header files.
 */

// DQ (5/7/2020): Make the EDG_ROSE_Translation namespace available so that we can access 
// the the map of include files.
namespace EDG_ROSE_Translation
   {
     extern std::map<std::string, SgIncludeFile*> edg_include_file_map;
   };


void addIncludeDirectives( SgNode* node );

//! Inherited attribute required for AddIncludeDirectives class.
class AddIncludeDirectivesInheritedAttribute
   {
     public:
          bool xxx;
          bool foundStatementFromIncludeFile;

       // AddIncludeDirectivesInheritedAttribute() : xxx(false) {}
          AddIncludeDirectivesInheritedAttribute();
          AddIncludeDirectivesInheritedAttribute( const AddIncludeDirectivesInheritedAttribute & X );
   };

class AddIncludeDirectivesSynthesizedAttribute
   {
     public:
          AddIncludeDirectivesSynthesizedAttribute();
          AddIncludeDirectivesSynthesizedAttribute( const AddIncludeDirectivesSynthesizedAttribute & X );
   };


// class AddIncludeDirectivesTraversal : public SgTopDownProcessing<AddIncludeDirectivesInheritedAttribute>
class AddIncludeDirectivesTraversal
   : public SgTopDownBottomUpProcessing<AddIncludeDirectivesInheritedAttribute,AddIncludeDirectivesSynthesizedAttribute>
   {
     public:
       // std::map<std::string, SgIncludeFile*> include_file_map;
          SgIncludeFile* include_file;

          SgStatement* previous_statement;

          bool includeDirectiveHasBeenInserted;

       // AddIncludeDirectivesTraversal(std::map<std::string, SgIncludeFile*> & input_include_file_map);
          AddIncludeDirectivesTraversal(SgIncludeFile* input_include_file);

       // Required traversal function
          AddIncludeDirectivesInheritedAttribute
               evaluateInheritedAttribute ( SgNode* node, AddIncludeDirectivesInheritedAttribute inheritedAttribute );

       // Required traversal function
          AddIncludeDirectivesSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             AddIncludeDirectivesInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// endif for ADD_INCLUDE_DIRECTIVES_H
#endif


// This skeleton is the basis for modifications to make it useful to
// connect the automatically generated ATerm constructor functions to the
// hand-written SageBuilder API.

// It is not clear to me if there is a better way since at some point
// the language specific semantics of specific IR nodes must be dealt with
// by hand.  but is a weak point of this Aterm process that we need this
// moderately manual translation process (a portion of the code is generated
// manually, but then the Aterm tranalation would be automated).  This is an
// example of why it is difficult to automate the addition of new IR nodes to
// an existing compiler (though this piece could be a function that the user
// would have to specify to support the addition of IR nodes in a semi-automated
// mechanism (e.g in Rosebud).

#include "sage3basic.h"

// The header file ("rose_config.h") should only be included by source files that require it.
#include "rose_config.h"

using namespace std;

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNode* SgNode::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgNode::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNode* returnNode = NULL;

     return returnNode;
   }


SgSupport* SgSupport::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgSupport::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSupport* returnNode = NULL;

     return returnNode;
   }


SgModifier* SgModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgModifier* returnNode = NULL;

     return returnNode;
   }


SgModifierNodes* SgModifierNodes::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgModifierNodes::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgModifierNodes* returnNode = NULL;

     return returnNode;
   }


SgConstVolatileModifier* SgConstVolatileModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgConstVolatileModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgConstVolatileModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 90 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgStorageModifier* SgStorageModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgStorageModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgStorageModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 108 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAccessModifier* SgAccessModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAccessModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAccessModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 126 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionModifier* SgFunctionModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgFunctionModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 144 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUPC_AccessModifier* SgUPC_AccessModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgUPC_AccessModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUPC_AccessModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 162 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSpecialFunctionModifier* SgSpecialFunctionModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgSpecialFunctionModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSpecialFunctionModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 180 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgElaboratedTypeModifier* SgElaboratedTypeModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgElaboratedTypeModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgElaboratedTypeModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 198 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLinkageModifier* SgLinkageModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgLinkageModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLinkageModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 216 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBaseClassModifier* SgBaseClassModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgBaseClassModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBaseClassModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 234 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeModifier* SgTypeModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 252 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDeclarationModifier* SgDeclarationModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgDeclarationModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDeclarationModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 270 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOpenclAccessModeModifier* SgOpenclAccessModeModifier::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgOpenclAccessModeModifier::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOpenclAccessModeModifier* returnNode = NULL;

     return returnNode;
   }


/* #line 288 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgName* SgName::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgName::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgName* returnNode = NULL;

     return returnNode;
   }


/* #line 306 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSymbolTable* SgSymbolTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgSymbolTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSymbolTable* returnNode = NULL;

     return returnNode;
   }


/* #line 324 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAttribute* SgAttribute::build_node_from_nonlist_children ( std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgAttribute::build_node_from_nonlist_children (std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAttribute* returnNode = NULL;

     return returnNode;
   }


/* #line 342 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPragma* SgPragma::build_node_from_nonlist_children ( std::string name, Sg_File_Info* startOfConstruct, Sg_File_Info* endOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgPragma::build_node_from_nonlist_children (std::string name, Sg_File_Info* startOfConstruct, Sg_File_Info* endOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPragma* returnNode = NULL;

     return returnNode;
   }


/* #line 360 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBitAttribute* SgBitAttribute::build_node_from_nonlist_children ( std::string name, unsigned long int bitflag )
   {
#if 0
  // debugging information!
     printf ("In SgBitAttribute::build_node_from_nonlist_children (std::string name, unsigned long int bitflag): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBitAttribute* returnNode = NULL;

     return returnNode;
   }


/* #line 378 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFuncDecl_attr* SgFuncDecl_attr::build_node_from_nonlist_children ( std::string name, unsigned long int bitflag )
   {
#if 0
  // debugging information!
     printf ("In SgFuncDecl_attr::build_node_from_nonlist_children (std::string name, unsigned long int bitflag): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFuncDecl_attr* returnNode = NULL;

     return returnNode;
   }


/* #line 396 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgClassDecl_attr* SgClassDecl_attr::build_node_from_nonlist_children ( std::string name, unsigned long int bitflag )
   {
#if 0
  // debugging information!
     printf ("In SgClassDecl_attr::build_node_from_nonlist_children (std::string name, unsigned long int bitflag): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgClassDecl_attr* returnNode = NULL;

     return returnNode;
   }


/* #line 414 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
Sg_File_Info* Sg_File_Info::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In Sg_File_Info::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     Sg_File_Info* returnNode = NULL;

     return returnNode;
   }


/* #line 432 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFile* SgFile::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgFile::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFile* returnNode = NULL;

     return returnNode;
   }


/* #line 450 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSourceFile* SgSourceFile::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgSourceFile::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSourceFile* returnNode = NULL;

     returnNode = new SgSourceFile();
     ROSE_ASSERT(returnNode != NULL);

     return returnNode;
   }


/* #line 468 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBinaryComposite* SgBinaryComposite::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgBinaryComposite::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBinaryComposite* returnNode = NULL;

     return returnNode;
   }


/* #line 486 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUnknownFile* SgUnknownFile::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgUnknownFile::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnknownFile* returnNode = NULL;

     return returnNode;
   }


/* #line 504 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgProject* SgProject::build_node_from_nonlist_children ( SgFileList* fileList_ptr )
   {
#if 0
  // debugging information!
     printf ("In SgProject::build_node_from_nonlist_children (SgFileList* fileList_ptr): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgProject* returnNode = NULL;

     return returnNode;
   }


/* #line 522 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOptions* SgOptions::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgOptions::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOptions* returnNode = NULL;

     return returnNode;
   }


/* #line 540 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUnparse_Info* SgUnparse_Info::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgUnparse_Info::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnparse_Info* returnNode = NULL;

     return returnNode;
   }


/* #line 558 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBaseClass* SgBaseClass::build_node_from_nonlist_children ( SgClassDeclaration* base_class, bool isDirectBaseClass )
   {
#if 0
  // debugging information!
     printf ("In SgBaseClass::build_node_from_nonlist_children (SgClassDeclaration* base_class, bool isDirectBaseClass): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBaseClass* returnNode = NULL;

     return returnNode;
   }


/* #line 576 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgExpBaseClass* SgExpBaseClass::build_node_from_nonlist_children ( SgClassDeclaration* base_class, bool isDirectBaseClass, SgExpression* base_class_exp )
   {
#if 0
  // debugging information!
     printf ("In SgExpBaseClass::build_node_from_nonlist_children (SgClassDeclaration* base_class, bool isDirectBaseClass, SgExpression* base_class_exp): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgExpBaseClass* returnNode = NULL;

     return returnNode;
   }


/* #line 594 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypedefSeq* SgTypedefSeq::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypedefSeq::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypedefSeq* returnNode = NULL;

     return returnNode;
   }


/* #line 612 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateParameter* SgTemplateParameter::build_node_from_nonlist_children ( SgTemplateParameter::template_parameter_enum parameterType, SgType* type, SgType* defaultTypeParameter, SgExpression* expression, SgExpression* defaultExpressionParameter, SgDeclarationStatement* templateDeclaration, SgDeclarationStatement* defaultTemplateDeclarationParameter, SgInitializedName* initializedName )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateParameter::build_node_from_nonlist_children (SgTemplateParameter::template_parameter_enum parameterType, SgType* type, SgType* defaultTypeParameter, SgExpression* expression, SgExpression* defaultExpressionParameter, SgDeclarationStatement* templateDeclaration, SgDeclarationStatement* defaultTemplateDeclarationParameter, SgInitializedName* initializedName): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateParameter* returnNode = NULL;

     return returnNode;
   }


/* #line 630 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateArgument* SgTemplateArgument::build_node_from_nonlist_children ( SgTemplateArgument::template_argument_enum argumentType, bool isArrayBoundUnknownType, SgType* type, SgExpression* expression, SgDeclarationStatement* templateDeclaration, bool explicitlySpecified )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateArgument::build_node_from_nonlist_children (SgTemplateArgument::template_argument_enum argumentType, bool isArrayBoundUnknownType, SgType* type, SgExpression* expression, SgDeclarationStatement* templateDeclaration, bool explicitlySpecified): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateArgument* returnNode = NULL;

     return returnNode;
   }


/* #line 648 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDirectory* SgDirectory::build_node_from_nonlist_children ( std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgDirectory::build_node_from_nonlist_children (std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDirectory* returnNode = NULL;

     return returnNode;
   }


/* #line 666 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFileList* SgFileList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgFileList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFileList* returnNode = NULL;

     return returnNode;
   }


/* #line 684 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDirectoryList* SgDirectoryList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgDirectoryList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDirectoryList* returnNode = NULL;

     return returnNode;
   }


/* #line 702 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionParameterTypeList* SgFunctionParameterTypeList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgFunctionParameterTypeList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionParameterTypeList* returnNode = NULL;

     return returnNode;
   }


/* #line 720 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgQualifiedName* SgQualifiedName::build_node_from_nonlist_children ( SgScopeStatement* scope )
   {
#if 0
  // debugging information!
     printf ("In SgQualifiedName::build_node_from_nonlist_children (SgScopeStatement* scope): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgQualifiedName* returnNode = NULL;

     return returnNode;
   }


/* #line 738 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateArgumentList* SgTemplateArgumentList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateArgumentList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateArgumentList* returnNode = NULL;

     return returnNode;
   }


/* #line 756 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateParameterList* SgTemplateParameterList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateParameterList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateParameterList* returnNode = NULL;

     return returnNode;
   }


/* #line 774 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgGraph* SgGraph::build_node_from_nonlist_children ( std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgGraph::build_node_from_nonlist_children (std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgGraph* returnNode = NULL;

     return returnNode;
   }


/* #line 792 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIncidenceDirectedGraph* SgIncidenceDirectedGraph::build_node_from_nonlist_children ( std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgIncidenceDirectedGraph::build_node_from_nonlist_children (std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIncidenceDirectedGraph* returnNode = NULL;

     return returnNode;
   }


/* #line 810 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBidirectionalGraph* SgBidirectionalGraph::build_node_from_nonlist_children ( std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgBidirectionalGraph::build_node_from_nonlist_children (std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBidirectionalGraph* returnNode = NULL;

     return returnNode;
   }


/* #line 828 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgStringKeyedBidirectionalGraph* SgStringKeyedBidirectionalGraph::build_node_from_nonlist_children ( std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgStringKeyedBidirectionalGraph::build_node_from_nonlist_children (std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgStringKeyedBidirectionalGraph* returnNode = NULL;

     return returnNode;
   }


/* #line 846 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIntKeyedBidirectionalGraph* SgIntKeyedBidirectionalGraph::build_node_from_nonlist_children ( std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgIntKeyedBidirectionalGraph::build_node_from_nonlist_children (std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIntKeyedBidirectionalGraph* returnNode = NULL;

     return returnNode;
   }


/* #line 864 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIncidenceUndirectedGraph* SgIncidenceUndirectedGraph::build_node_from_nonlist_children ( std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgIncidenceUndirectedGraph::build_node_from_nonlist_children (std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIncidenceUndirectedGraph* returnNode = NULL;

     return returnNode;
   }


/* #line 882 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgGraphNode* SgGraphNode::build_node_from_nonlist_children ( std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgGraphNode::build_node_from_nonlist_children (std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgGraphNode* returnNode = NULL;

     return returnNode;
   }


/* #line 900 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgGraphEdge* SgGraphEdge::build_node_from_nonlist_children ( SgGraphNode* node_A, SgGraphNode* node_B, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgGraphEdge::build_node_from_nonlist_children (SgGraphNode* node_A, SgGraphNode* node_B, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgGraphEdge* returnNode = NULL;

     return returnNode;
   }


/* #line 918 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDirectedGraphEdge* SgDirectedGraphEdge::build_node_from_nonlist_children ( SgGraphNode* node_A, SgGraphNode* node_B, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgDirectedGraphEdge::build_node_from_nonlist_children (SgGraphNode* node_A, SgGraphNode* node_B, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDirectedGraphEdge* returnNode = NULL;

     return returnNode;
   }


/* #line 936 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUndirectedGraphEdge* SgUndirectedGraphEdge::build_node_from_nonlist_children ( SgGraphNode* node_A, SgGraphNode* node_B, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgUndirectedGraphEdge::build_node_from_nonlist_children (SgGraphNode* node_A, SgGraphNode* node_B, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUndirectedGraphEdge* returnNode = NULL;

     return returnNode;
   }


/* #line 954 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgGraphNodeList* SgGraphNodeList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgGraphNodeList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgGraphNodeList* returnNode = NULL;

     return returnNode;
   }


/* #line 972 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgGraphEdgeList* SgGraphEdgeList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgGraphEdgeList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgGraphEdgeList* returnNode = NULL;

     return returnNode;
   }


/* #line 990 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeTable* SgTypeTable::build_node_from_nonlist_children ( SgSymbolTable* type_table )
   {
#if 0
  // debugging information!
     printf ("In SgTypeTable::build_node_from_nonlist_children (SgSymbolTable* type_table): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeTable* returnNode = NULL;

     return returnNode;
   }


/* #line 1008 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNameGroup* SgNameGroup::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgNameGroup::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNameGroup* returnNode = NULL;

     return returnNode;
   }


/* #line 1026 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDimensionObject* SgDimensionObject::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgDimensionObject::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDimensionObject* returnNode = NULL;

     return returnNode;
   }


/* #line 1044 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFormatItem* SgFormatItem::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgFormatItem::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFormatItem* returnNode = NULL;

     return returnNode;
   }


/* #line 1062 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFormatItemList* SgFormatItemList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgFormatItemList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFormatItemList* returnNode = NULL;

     return returnNode;
   }


/* #line 1080 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDataStatementGroup* SgDataStatementGroup::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgDataStatementGroup::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDataStatementGroup* returnNode = NULL;

     return returnNode;
   }


/* #line 1098 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDataStatementObject* SgDataStatementObject::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgDataStatementObject::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDataStatementObject* returnNode = NULL;

     return returnNode;
   }


/* #line 1116 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDataStatementValue* SgDataStatementValue::build_node_from_nonlist_children ( SgDataStatementValue::data_statement_value_enum data_initialization_format )
   {
#if 0
  // debugging information!
     printf ("In SgDataStatementValue::build_node_from_nonlist_children (SgDataStatementValue::data_statement_value_enum data_initialization_format): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDataStatementValue* returnNode = NULL;

     return returnNode;
   }


/* #line 1134 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaImportStatementList* SgJavaImportStatementList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgJavaImportStatementList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaImportStatementList* returnNode = NULL;

     return returnNode;
   }


/* #line 1152 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaClassDeclarationList* SgJavaClassDeclarationList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgJavaClassDeclarationList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaClassDeclarationList* returnNode = NULL;

     return returnNode;
   }


/* #line 1170 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgType* SgType::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgType::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgType* returnNode = NULL;

     return returnNode;
   }


/* #line 1188 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeUnknown* SgTypeUnknown::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeUnknown::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeUnknown* returnNode = NULL;

     return returnNode;
   }


/* #line 1206 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeChar* SgTypeChar::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeChar::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeChar* returnNode = NULL;

     return returnNode;
   }


// DQ (3/23/2018): Added to support new char16 and char32 types.
SgTypeChar16* SgTypeChar16::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeChar16::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeChar16* returnNode = NULL;

     return returnNode;
   }


// DQ (3/23/2018): Added to support new char16 and char32 types.
SgTypeChar32* SgTypeChar32::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeChar32::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeChar32* returnNode = NULL;

     return returnNode;
   }


/* #line 1224 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeSignedChar* SgTypeSignedChar::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeSignedChar::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeSignedChar* returnNode = NULL;

     return returnNode;
   }


/* #line 1242 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeUnsignedChar* SgTypeUnsignedChar::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeUnsignedChar::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeUnsignedChar* returnNode = NULL;

     return returnNode;
   }


/* #line 1260 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeShort* SgTypeShort::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeShort::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeShort* returnNode = NULL;

     return returnNode;
   }


/* #line 1278 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeSignedShort* SgTypeSignedShort::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeSignedShort::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeSignedShort* returnNode = NULL;

     return returnNode;
   }


/* #line 1296 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeUnsignedShort* SgTypeUnsignedShort::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeUnsignedShort::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeUnsignedShort* returnNode = NULL;

     return returnNode;
   }


/* #line 1314 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeInt* SgTypeInt::build_node_from_nonlist_children ( int field_size )
   {
#if 0
  // debugging information!
     printf ("In SgTypeInt::build_node_from_nonlist_children (int field_size): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeInt* returnNode = NULL;

  // Note that the input field_size is not used since the field_size must be an expression 
  // in more complex cases and it is a annotation to the SgInitializedName and not a proper 
  // part of the type system.  This is a relic from the initial design of the ROSE IR.

     returnNode = SageBuilder::buildIntType();
     ROSE_ASSERT(returnNode != NULL);

     return returnNode;
   }


/* #line 1332 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeSignedInt* SgTypeSignedInt::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeSignedInt::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeSignedInt* returnNode = NULL;

     return returnNode;
   }


/* #line 1350 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeUnsignedInt* SgTypeUnsignedInt::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeUnsignedInt::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeUnsignedInt* returnNode = NULL;

     return returnNode;
   }


/* #line 1368 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeLong* SgTypeLong::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeLong::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeLong* returnNode = NULL;

     returnNode = SageBuilder::buildLongType();
     ROSE_ASSERT(returnNode != NULL);

     return returnNode;
   }


/* #line 1386 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeSignedLong* SgTypeSignedLong::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeSignedLong::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeSignedLong* returnNode = NULL;

     return returnNode;
   }


/* #line 1404 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeUnsignedLong* SgTypeUnsignedLong::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeUnsignedLong::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeUnsignedLong* returnNode = NULL;

     return returnNode;
   }


/* #line 1422 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeVoid* SgTypeVoid::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeVoid::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeVoid* returnNode = NULL;

     return returnNode;
   }


/* #line 1440 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeGlobalVoid* SgTypeGlobalVoid::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeGlobalVoid::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeGlobalVoid* returnNode = NULL;

     return returnNode;
   }


/* #line 1458 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeWchar* SgTypeWchar::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeWchar::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeWchar* returnNode = NULL;

     return returnNode;
   }


/* #line 1476 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeFloat* SgTypeFloat::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeFloat::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeFloat* returnNode = NULL;

     returnNode = SageBuilder::buildFloatType();
     ROSE_ASSERT(returnNode != NULL);

     return returnNode;
   }


/* #line 1494 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeDouble* SgTypeDouble::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeDouble::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeDouble* returnNode = NULL;

     returnNode = SageBuilder::buildDoubleType();
     ROSE_ASSERT(returnNode != NULL);

     return returnNode;
   }

SgTypeFloat80* SgTypeFloat80::build_node_from_nonlist_children (  )
   {
     SgTypeFloat80* returnNode = SageBuilder::buildFloat80Type();
     ROSE_ASSERT(returnNode != NULL);
     return returnNode;
   }

SgTypeFloat128* SgTypeFloat128::build_node_from_nonlist_children (  )
   {
     SgTypeFloat128* returnNode = SageBuilder::buildFloat128Type();
     ROSE_ASSERT(returnNode != NULL);
     return returnNode;
   }


/* #line 1512 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeLongLong* SgTypeLongLong::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeLongLong::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeLongLong* returnNode = NULL;

     return returnNode;
   }


/* #line 1530 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeSignedLongLong* SgTypeSignedLongLong::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeSignedLongLong::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeSignedLongLong* returnNode = NULL;

     return returnNode;
   }


/* #line 1548 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeUnsignedLongLong* SgTypeUnsignedLongLong::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeUnsignedLongLong::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeUnsignedLongLong* returnNode = NULL;

     return returnNode;
   }


/* #line 1566 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeSigned128bitInteger* SgTypeSigned128bitInteger::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeSigned128bitInteger::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeSigned128bitInteger* returnNode = NULL;

     return returnNode;
   }


/* #line 1584 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeUnsigned128bitInteger* SgTypeUnsigned128bitInteger::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeUnsigned128bitInteger::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeUnsigned128bitInteger* returnNode = NULL;

     return returnNode;
   }


/* #line 1602 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeLongDouble* SgTypeLongDouble::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeLongDouble::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeLongDouble* returnNode = NULL;

     returnNode = SageBuilder::buildLongDoubleType();
     ROSE_ASSERT(returnNode != NULL);

     return returnNode;
   }


/* #line 1620 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeString* SgTypeString::build_node_from_nonlist_children ( SgExpression* lengthExpression )
   {
#if 0
  // debugging information!
     printf ("In SgTypeString::build_node_from_nonlist_children (SgExpression* lengthExpression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeString* returnNode = NULL;

     return returnNode;
   }


/* #line 1638 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeBool* SgTypeBool::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeBool::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeBool* returnNode = NULL;

     return returnNode;
   }


/* #line 1656 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPointerType* SgPointerType::build_node_from_nonlist_children ( SgType* base_type )
   {
#if 0
  // debugging information!
     printf ("In SgPointerType::build_node_from_nonlist_children (SgType* base_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPointerType* returnNode = NULL;

     ROSE_ASSERT(base_type != NULL);

     returnNode = SageBuilder::buildPointerType(base_type);
     ROSE_ASSERT(returnNode != NULL);

     return returnNode;
   }


/* #line 1674 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPointerMemberType* SgPointerMemberType::build_node_from_nonlist_children ( SgType* base_type, SgType* class_type )
   {
#if 0
  // debugging information!
     printf ("In SgPointerMemberType::build_node_from_nonlist_children (SgType* base_type, SgType* class_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPointerMemberType* returnNode = NULL;

     return returnNode;
   }


/* #line 1692 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgReferenceType* SgReferenceType::build_node_from_nonlist_children ( SgType* base_type )
   {
#if 0
  // debugging information!
     printf ("In SgReferenceType::build_node_from_nonlist_children (SgType* base_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgReferenceType* returnNode = NULL;

     return returnNode;
   }


/* #line 1710 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNamedType* SgNamedType::build_node_from_nonlist_children ( SgDeclarationStatement* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgNamedType::build_node_from_nonlist_children (SgDeclarationStatement* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNamedType* returnNode = NULL;

     return returnNode;
   }


/* #line 1728 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgClassType* SgClassType::build_node_from_nonlist_children ( SgDeclarationStatement* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgClassType::build_node_from_nonlist_children (SgDeclarationStatement* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgClassType* returnNode = NULL;

     return returnNode;
   }


/* #line 1746 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaParameterType* SgJavaParameterType::build_node_from_nonlist_children ( SgDeclarationStatement* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgJavaParameterType::build_node_from_nonlist_children (SgDeclarationStatement* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaParameterType* returnNode = NULL;

     return returnNode;
   }


/* #line 1764 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaParameterizedType* SgJavaParameterizedType::build_node_from_nonlist_children ( SgDeclarationStatement* declaration, SgNamedType* raw_type, SgTemplateParameterList* type_list )
   {
#if 0
  // debugging information!
     printf ("In SgJavaParameterizedType::build_node_from_nonlist_children (SgDeclarationStatement* declaration, SgNamedType* raw_type, SgTemplateParameterList* type_list): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaParameterizedType* returnNode = NULL;

     return returnNode;
   }


/* #line 1782 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaQualifiedType* SgJavaQualifiedType::build_node_from_nonlist_children ( SgDeclarationStatement* declaration, SgNamedType * parent_type, SgNamedType * type )
   {
#if 0
  // debugging information!
     printf ("In SgJavaQualifiedType::build_node_from_nonlist_children (SgDeclarationStatement* declaration, SgNamedType * parent_type, SgNamedType * type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaQualifiedType* returnNode = NULL;

     return returnNode;
   }


/* #line 1800 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEnumType* SgEnumType::build_node_from_nonlist_children ( SgDeclarationStatement* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgEnumType::build_node_from_nonlist_children (SgDeclarationStatement* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEnumType* returnNode = NULL;

     return returnNode;
   }


/* #line 1818 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypedefType* SgTypedefType::build_node_from_nonlist_children ( SgDeclarationStatement* declaration, SgSymbol* parent_scope )
   {
#if 0
  // debugging information!
     printf ("In SgTypedefType::build_node_from_nonlist_children (SgDeclarationStatement* declaration, SgSymbol* parent_scope): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypedefType* returnNode = NULL;

     return returnNode;
   }


/* #line 1836 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaWildcardType* SgJavaWildcardType::build_node_from_nonlist_children ( SgDeclarationStatement* declaration, SgType* bound_type, bool is_unbound, bool has_extends, bool has_super )
   {
#if 0
  // debugging information!
     printf ("In SgJavaWildcardType::build_node_from_nonlist_children (SgDeclarationStatement* declaration, SgType* bound_type, bool is_unbound, bool has_extends, bool has_super): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaWildcardType* returnNode = NULL;

     return returnNode;
   }


/* #line 1854 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgModifierType* SgModifierType::build_node_from_nonlist_children ( SgType* base_type )
   {
#if 0
  // debugging information!
     printf ("In SgModifierType::build_node_from_nonlist_children (SgType* base_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgModifierType* returnNode = NULL;

     return returnNode;
   }


/* #line 1872 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionType* SgFunctionType::build_node_from_nonlist_children ( SgType* return_type, bool has_ellipses )
   {
#if 0
  // debugging information!
     printf ("In SgFunctionType::build_node_from_nonlist_children (SgType* return_type, bool has_ellipses): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionType* returnNode = NULL;

     return returnNode;
   }


/* #line 1890 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMemberFunctionType* SgMemberFunctionType::build_node_from_nonlist_children ( SgType* return_type, bool has_ellipses, SgType* class_type, unsigned int mfunc_specifier, unsigned int ref_qualifier )
   {
#if 0
  // debugging information!
     printf ("In SgMemberFunctionType::build_node_from_nonlist_children (SgType* return_type, bool has_ellipses, SgType* class_type, unsigned int mfunc_specifier): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMemberFunctionType* returnNode = NULL;

     return returnNode;
   }


/* #line 1908 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPartialFunctionType* SgPartialFunctionType::build_node_from_nonlist_children ( SgType* return_type, bool has_ellipses, SgType* class_type, unsigned int mfunc_specifier, unsigned int ref_qualifier )
   {
#if 0
  // debugging information!
     printf ("In SgPartialFunctionType::build_node_from_nonlist_children (SgType* return_type, bool has_ellipses, SgType* class_type, unsigned int mfunc_specifier): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPartialFunctionType* returnNode = NULL;

     return returnNode;
   }


/* #line 1926 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPartialFunctionModifierType* SgPartialFunctionModifierType::build_node_from_nonlist_children ( SgType* return_type, bool has_ellipses, SgType* class_type, unsigned int mfunc_specifier, unsigned int ref_qualifier )
   {
#if 0
  // debugging information!
     printf ("In SgPartialFunctionModifierType::build_node_from_nonlist_children (SgType* return_type, bool has_ellipses, SgType* class_type, unsigned int mfunc_specifier): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPartialFunctionModifierType* returnNode = NULL;

     return returnNode;
   }


/* #line 1944 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgArrayType* SgArrayType::build_node_from_nonlist_children ( SgType* base_type, SgExpression* index )
   {
#if 0
  // debugging information!
     printf ("In SgArrayType::build_node_from_nonlist_children (SgType* base_type, SgExpression* index): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgArrayType* returnNode = NULL;

     return returnNode;
   }


/* #line 1962 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeEllipse* SgTypeEllipse::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeEllipse::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeEllipse* returnNode = NULL;

     return returnNode;
   }


/* #line 1980 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateType* SgTemplateType::build_node_from_nonlist_children ( SgName name )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateType::build_node_from_nonlist_children (SgName name, int template_parameter_position): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateType* returnNode = NULL;

     return returnNode;
   }


/* #line 1998 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgQualifiedNameType* SgQualifiedNameType::build_node_from_nonlist_children ( SgType* base_type )
   {
#if 0
  // debugging information!
     printf ("In SgQualifiedNameType::build_node_from_nonlist_children (SgType* base_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgQualifiedNameType* returnNode = NULL;

     return returnNode;
   }


/* #line 2016 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeComplex* SgTypeComplex::build_node_from_nonlist_children ( SgType* base_type )
   {
#if 0
  // debugging information!
     printf ("In SgTypeComplex::build_node_from_nonlist_children (SgType* base_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeComplex* returnNode = NULL;

     return returnNode;
   }


/* #line 2034 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeImaginary* SgTypeImaginary::build_node_from_nonlist_children ( SgType* base_type )
   {
#if 0
  // debugging information!
     printf ("In SgTypeImaginary::build_node_from_nonlist_children (SgType* base_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeImaginary* returnNode = NULL;

     return returnNode;
   }


/* #line 2052 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeDefault* SgTypeDefault::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeDefault::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeDefault* returnNode = NULL;

     return returnNode;
   }


/* #line 2070 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeCAFTeam* SgTypeCAFTeam::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeCAFTeam::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeCAFTeam* returnNode = NULL;

     return returnNode;
   }


/* #line 2088 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeCrayPointer* SgTypeCrayPointer::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeCrayPointer::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeCrayPointer* returnNode = NULL;

     return returnNode;
   }


/* #line 2106 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeLabel* SgTypeLabel::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeLabel::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeLabel* returnNode = NULL;

     return returnNode;
   }


/* #line 2124 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaUnionType* SgJavaUnionType::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgJavaUnionType::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaUnionType* returnNode = NULL;

     return returnNode;
   }


/* #line 2142 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgRvalueReferenceType* SgRvalueReferenceType::build_node_from_nonlist_children ( SgType* base_type )
   {
#if 0
  // debugging information!
     printf ("In SgRvalueReferenceType::build_node_from_nonlist_children (SgType* base_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgRvalueReferenceType* returnNode = NULL;

     return returnNode;
   }


/* #line 2160 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeNullptr* SgTypeNullptr::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgTypeNullptr::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeNullptr* returnNode = NULL;

     return returnNode;
   }


/* #line 2178 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDeclType* SgDeclType::build_node_from_nonlist_children ( SgExpression* base_expression, SgType* base_type )
   {
#if 0
  // debugging information!
     printf ("In SgDeclType::build_node_from_nonlist_children (SgExpression* base_expression, SgType* base_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDeclType* returnNode = NULL;

     return returnNode;
   }


/* #line 2196 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLocatedNode* SgLocatedNode::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgLocatedNode::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLocatedNode* returnNode = NULL;

     return returnNode;
   }


/* #line 2214 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgToken* SgToken::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string lexeme_string, unsigned int classification_code )
   {
#if 0
  // debugging information!
     printf ("In SgToken::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string lexeme_string, unsigned int classification_code): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgToken* returnNode = NULL;

     return returnNode;
   }


/* #line 2232 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLocatedNodeSupport* SgLocatedNodeSupport::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgLocatedNodeSupport::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLocatedNodeSupport* returnNode = NULL;

     return returnNode;
   }


/* #line 2250 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCommonBlockObject* SgCommonBlockObject::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgCommonBlockObject::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCommonBlockObject* returnNode = NULL;

     return returnNode;
   }


/* #line 2268 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
// SgInitializedName* SgInitializedName::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
SgInitializedName* SgInitializedName::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgType* type)
   {
#if 1
  // debugging information!
      printf ("In SgInitializedName::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): startOfConstruct = %p name = %s type = %p \n",startOfConstruct,name.str(),type);
#endif

     SgInitializedName* returnNode = NULL;

  // Calling SgInitializedName* buildInitializedName(const SgName & name, SgType* type, SgInitializer* init = NULL);

     ROSE_ASSERT(startOfConstruct != NULL);
     ROSE_ASSERT(type != NULL);

     SgInitializer* initializer = NULL;
     returnNode = SageBuilder::buildInitializedName(name,type,initializer);
     ROSE_ASSERT(returnNode != NULL);

     return returnNode;
   }


/* #line 2286 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgInterfaceBody* SgInterfaceBody::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName function_name, SgFunctionDeclaration* functionDeclaration, bool use_function_name )
   {
#if 0
  // debugging information!
     printf ("In SgInterfaceBody::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName function_name, SgFunctionDeclaration* functionDeclaration, bool use_function_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgInterfaceBody* returnNode = NULL;

     return returnNode;
   }


/* #line 2304 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgHeaderFileBody* SgHeaderFileBody::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgHeaderFileBody::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgHeaderFileBody* returnNode = NULL;

     return returnNode;
   }


/* #line 2322 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgRenamePair* SgRenamePair::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName local_name, SgName use_name )
   {
#if 0
  // debugging information!
     printf ("In SgRenamePair::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName local_name, SgName use_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgRenamePair* returnNode = NULL;

     return returnNode;
   }


/* #line 2340 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaMemberValuePair* SgJavaMemberValuePair::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgJavaMemberValuePair::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaMemberValuePair* returnNode = NULL;

     return returnNode;
   }


/* #line 2358 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpClause* SgOmpClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgOmpClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2376 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpOrderedClause* SgOmpOrderedClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgOmpOrderedClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpOrderedClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2394 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpNowaitClause* SgOmpNowaitClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgOmpNowaitClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpNowaitClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2412 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpUntiedClause* SgOmpUntiedClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgOmpUntiedClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpUntiedClause* returnNode = NULL;

     return returnNode;
   }

SgOmpMergeableClause* SgOmpMergeableClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
     SgOmpMergeableClause* returnNode = NULL;

     return returnNode;
   }



/* #line 2430 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpDefaultClause* SgOmpDefaultClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgOmpClause::omp_default_option_enum data_sharing )
   {
#if 0
  // debugging information!
     printf ("In SgOmpDefaultClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgOmpClause::omp_default_option_enum data_sharing): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpDefaultClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2448 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpExpressionClause* SgOmpExpressionClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgOmpExpressionClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpExpressionClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2466 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpCollapseClause* SgOmpCollapseClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgOmpCollapseClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpCollapseClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2484 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpIfClause* SgOmpIfClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgOmpIfClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpIfClause* returnNode = NULL;

     return returnNode;
   }

SgOmpFinalClause* SgOmpFinalClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
     SgOmpFinalClause* returnNode = NULL;

     return returnNode;
   }


SgOmpPriorityClause* SgOmpPriorityClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
     SgOmpPriorityClause* returnNode = NULL;

     return returnNode;
   }



/* #line 2502 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpNumThreadsClause* SgOmpNumThreadsClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgOmpNumThreadsClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpNumThreadsClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2520 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpDeviceClause* SgOmpDeviceClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgOmpDeviceClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpDeviceClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2538 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpSafelenClause* SgOmpSafelenClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgOmpSafelenClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpSafelenClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2556 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpVariablesClause* SgOmpVariablesClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExprListExp* variables)
   {
#if 0
  // debugging information!
     printf ("In SgOmpVariablesClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpVariablesClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2574 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpCopyprivateClause* SgOmpCopyprivateClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExprListExp* variables)
   {
#if 0
  // debugging information!
     printf ("In SgOmpCopyprivateClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpCopyprivateClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2592 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpPrivateClause* SgOmpPrivateClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExprListExp* variables)
   {
#if 0
  // debugging information!
     printf ("In SgOmpPrivateClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpPrivateClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2610 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpFirstprivateClause* SgOmpFirstprivateClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExprListExp* variables)
   {
#if 0
  // debugging information!
     printf ("In SgOmpFirstprivateClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpFirstprivateClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2628 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpSharedClause* SgOmpSharedClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExprListExp* variables)
   {
#if 0
  // debugging information!
     printf ("In SgOmpSharedClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpSharedClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2646 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpCopyinClause* SgOmpCopyinClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExprListExp* variables )
   {
#if 0
  // debugging information!
     printf ("In SgOmpCopyinClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpCopyinClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2664 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpLastprivateClause* SgOmpLastprivateClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExprListExp* variables )
   {
#if 0
  // debugging information!
     printf ("In SgOmpLastprivateClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpLastprivateClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2682 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpReductionClause* SgOmpReductionClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExprListExp* variables , SgOmpClause::omp_reduction_operator_enum operation )
   {
#if 0
  // debugging information!
     printf ("In SgOmpReductionClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgOmpClause::omp_reduction_operator_enum operation): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpReductionClause* returnNode = NULL;

     return returnNode;
   }

SgOmpDependClause* SgOmpDependClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExprListExp* variables , SgOmpClause::omp_dependence_type_enum operation )
   {
     SgOmpDependClause* returnNode = NULL;

     return returnNode;
   }



/* #line 2700 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpMapClause* SgOmpMapClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExprListExp* variables, SgOmpClause::omp_map_operator_enum operation )
   {
#if 0
  // debugging information!
     printf ("In SgOmpMapClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgOmpClause::omp_map_operator_enum operation): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpMapClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2718 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpLinearClause* SgOmpLinearClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExprListExp* variables, SgExpression* step)
   {
#if 0
  // debugging information!
     printf ("In SgOmpLinearClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpLinearClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2736 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpUniformClause* SgOmpUniformClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExprListExp* variables)
   {
#if 0
  // debugging information!
     printf ("In SgOmpUniformClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpUniformClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2754 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpAlignedClause* SgOmpAlignedClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct , SgExprListExp* variables, SgExpression* alignment)
   {
#if 0
  // debugging information!
     printf ("In SgOmpAlignedClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpAlignedClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2772 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpScheduleClause* SgOmpScheduleClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgOmpClause::omp_schedule_kind_enum kind, SgExpression* chunk_size )
   {
#if 0
  // debugging information!
     printf ("In SgOmpScheduleClause::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgOmpClause::omp_schedule_kind_enum kind, SgExpression* chunk_size): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpScheduleClause* returnNode = NULL;

     return returnNode;
   }


/* #line 2790 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedNode* SgUntypedNode::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedNode::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedNode* returnNode = NULL;

     return returnNode;
   }


/* #line 2808 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedExpression* SgUntypedExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 2826 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedUnaryOperator* SgUntypedUnaryOperator::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum, std::string operator_name, SgUntypedExpression* operand )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedUnaryOperator::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum, std::string operator_name, SgUntypedExpression* operand): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedUnaryOperator* returnNode = NULL;

     return returnNode;
   }


/* #line 2844 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedBinaryOperator* SgUntypedBinaryOperator::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum, std::string operator_name, SgUntypedExpression* lhs_operand, SgUntypedExpression* rhs_operand )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedBinaryOperator::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum, std::string operator_name, SgUntypedExpression* lhs_operand, SgUntypedExpression* rhs_operand): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedBinaryOperator* returnNode = NULL;

     return returnNode;
   }


/* #line 2862 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedValueExpression* SgUntypedValueExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum, std::string value_string, SgUntypedType* type )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedValueExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum, std::string value_string, SgUntypedType* type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedValueExpression* returnNode = NULL;

     return returnNode;
   }


// Static function for building IR nodes for Aterm support (like a constructor).
SgUntypedArrayReferenceExpression* SgUntypedArrayReferenceExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum, std::string name, SgUntypedExpression* array_subscripts, SgUntypedExpression* coarray_subscripts)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedArrayReferenceExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum, std::string name, SgUntypedExpression* array_subscripts, SgUntypedExpression* coarray_subscripts): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedArrayReferenceExpression* returnNode = NULL;

     return returnNode;
   }


// Static function for building IR nodes for Aterm support (like a constructor).
SgUntypedSubscriptExpression* SgUntypedSubscriptExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum, SgUntypedExpression* lower_bound, SgUntypedExpression* upper_bound, SgUntypedExpression* stride )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedSubscriptExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum, SgUntypedExpression* lower_bound, SgUntypedExpression* upper_bound, SgUntypedExpression* stride): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedSubscriptExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 2898 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedOtherExpression* SgUntypedOtherExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedOtherExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedOtherExpression* returnNode = NULL;

     return returnNode;
   }


SgUntypedNamedExpression* SgUntypedNamedExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum, std::string expression_name, SgUntypedExpression* expression)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedNamedExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum, std::string expression_name, SgUntypedExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedNamedExpression* returnNode = NULL;

     return returnNode;
   }


SgUntypedNullExpression* SgUntypedNullExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedNullExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedNullExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 2916 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedFunctionCallOrArrayReferenceExpression* SgUntypedFunctionCallOrArrayReferenceExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedFunctionCallOrArrayReferenceExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedFunctionCallOrArrayReferenceExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 2934 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedReferenceExpression* SgUntypedReferenceExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedReferenceExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedReferenceExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 2952 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedStatement* SgUntypedStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedDeclarationStatement* SgUntypedDeclarationStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedDeclarationStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedDeclarationStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedDirectiveDeclaration* SgUntypedDirectiveDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int statement_enum, std::string statement_name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedDirectiveDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum, std::string statement_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedDirectiveDeclaration* returnNode = NULL;

     return returnNode;
   }

SgUntypedEnumDeclaration* SgUntypedEnumDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string enum_name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedEnumDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string enum_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedEnumDeclaration* returnNode = NULL;

     return returnNode;
   }

SgUntypedLabelStatement* SgUntypedLabelStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedLabelStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedLabelStatement* returnNode = NULL;

     return returnNode;
   }

SgUntypedNameListDeclaration* SgUntypedNameListDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int statement_enum )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedNameListDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedNameListDeclaration* returnNode = NULL;

     return returnNode;
   }

SgUntypedInitializedNameListDeclaration* SgUntypedInitializedNameListDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int statement_enum )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedInitializedNameListDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedInitializedNameListDeclaration* returnNode = NULL;

     return returnNode;
   }

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedImplicitDeclaration* SgUntypedImplicitDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedImplicitDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedImplicitDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 3006 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedVariableDeclaration* SgUntypedVariableDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgUntypedType* type )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedVariableDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgUntypedType* type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedVariableDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 3024 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedFunctionDeclaration* SgUntypedFunctionDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedFunctionDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedFunctionDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 3042 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedProgramHeaderDeclaration* SgUntypedProgramHeaderDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedProgramHeaderDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedProgramHeaderDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 3060 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedSubroutineDeclaration* SgUntypedSubroutineDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedSubroutineDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedSubroutineDeclaration* returnNode = NULL;

     return returnNode;
   }


SgUntypedInterfaceDeclaration* SgUntypedInterfaceDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string name, int statement_enum )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedInterfaceDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name, int statement_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedInterfaceDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 3078 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedModuleDeclaration* SgUntypedModuleDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedModuleDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedModuleDeclaration* returnNode = NULL;

     return returnNode;
   }


// Rasmussen (8/16/2017): Added UntypedSubmoduleDeclaration IR node (a submodule extends an existing module)
SgUntypedSubmoduleDeclaration* SgUntypedSubmoduleDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string name, std::string submodule_ancestor, std::string submodule_parent )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedSubmoduleDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name, std::string submodule_ancestor, std::string submodule_parent): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedSubmoduleDeclaration* returnNode = NULL;

     return returnNode;
   }


// Rasmussen (8/17/2017): Added new IR node to represent a Fortran block-data program unit
SgUntypedBlockDataDeclaration* SgUntypedBlockDataDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedBlockDataDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedBlockDataDeclaration* returnNode = NULL;

     return returnNode;
   }


// Rasmussen (10/04/2017): Added several functions below for new untyped nodes related to general language support.
//
SgUntypedExceptionHandlerDeclaration* SgUntypedExceptionHandlerDeclaration::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedExceptionHandlerDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedExceptionHandlerDeclaration* returnNode = NULL;

     return returnNode;
   }


SgUntypedDelayRelativeStatement* SgUntypedDelayRelativeStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedDelayRelativeStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedDelayRelativeStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedForStatement* SgUntypedForStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedExpression* initialization, SgUntypedExpression* bound, SgUntypedExpression* increment, SgUntypedStatement* body, std::string do_construct_name)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedForStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedExpression* initialization, SgUntypedExpression* bound, SgUntypedExpression* increment, SgUntypedStatement* body, std::string do_construct_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedForStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedForAllStatement* SgUntypedForAllStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedType* type, SgUntypedExprListExpression* iterates, SgUntypedExprListExpression* local, SgUntypedExpression* mask, std::string do_construct_name)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedForAllStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedType* type, SgUntypedExprListExpression* iterates, SgUntypedExprListExpression* local, SgUntypedExpression* mask, std::string do_construct_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedForAllStatement* returnNode = NULL;

     return returnNode;
   }


// Rasmussen (4/18/2018): Added target_expression so that the goto statement has a place to go to.
SgUntypedGotoStatement* SgUntypedGotoStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, std::string target_label)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedGotoStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string target_label): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedGotoStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedLoopStatement* SgUntypedLoopStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedLoopStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedLoopStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedAcceptStatement* SgUntypedAcceptStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedAcceptStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedAcceptStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedIfStatement* SgUntypedIfStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedExpression* conditional, SgUntypedStatement* true_body, SgUntypedStatement* false_body)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedIfStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedExpression* conditional, SgUntypedStatement* true_body, SgUntypedStatement* false_body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedIfStatement* returnNode = NULL;

     return returnNode;
   }


SgDeclarationScope* SgDeclarationScope::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgDeclarationScope::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDeclarationScope* returnNode = NULL;

     return returnNode;
   }


SgUntypedRequeueStatement* SgUntypedRequeueStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedRequeueStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedRequeueStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedCaseStatement* SgUntypedCaseStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedExpression* expression, SgUntypedStatement* body, std::string case_name)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedCaseStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedExpression* expression, SgUntypedStatement* body, std::string case_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedCaseStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedExtendedReturnStatement* SgUntypedExtendedReturnStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedExtendedReturnStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedExtendedReturnStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedNullStatement* SgUntypedNullStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedNullStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedNullStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedNullDeclaration* SgUntypedNullDeclaration::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedNullDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedNullDeclaration* returnNode = NULL;

     return returnNode;
   }


SgUntypedReturnStatement* SgUntypedReturnStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, SgUntypedExpression* expression)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedReturnStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgUntypedExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedReturnStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedStopStatement* SgUntypedStopStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, SgUntypedExpression* expression)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedStopStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgUntypedExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedStopStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedTaskDeclaration* SgUntypedTaskDeclaration::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, std::string name)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedTaskDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedTaskDeclaration* returnNode = NULL;

     return returnNode;
   }


SgUntypedUnitDeclaration* SgUntypedUnitDeclaration::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, std::string name)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedUnitDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedUnitDeclaration* returnNode = NULL;

     return returnNode;
   }


SgUntypedConditionalEntryCallStatement* SgUntypedConditionalEntryCallStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedConditionalEntryCallStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedConditionalEntryCallStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedAbortStatement* SgUntypedAbortStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedAbortStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedAbortStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedRaiseStatement* SgUntypedRaiseStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedRaiseStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedRaiseStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedDelayUntilStatement* SgUntypedDelayUntilStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedDelayUntilStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedDelayUntilStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedExceptionDeclaration* SgUntypedExceptionDeclaration::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedExceptionDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedExceptionDeclaration* returnNode = NULL;

     return returnNode;
   }


SgUntypedExitStatement* SgUntypedExitStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedExitStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedExitStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedAsynchronousSelectStatement* SgUntypedAsynchronousSelectStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedAsynchronousSelectStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedAsynchronousSelectStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedWhileStatement* SgUntypedWhileStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, SgUntypedExpression* condition, SgUntypedStatement* body)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedWhileStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgUntypedExpression* condition, SgUntypedStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedWhileStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedTerminateAlternativeStatement* SgUntypedTerminateAlternativeStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedTerminateAlternativeStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedTerminateAlternativeStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedProcedureCallStatement* SgUntypedProcedureCallStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedProcedureCallStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedProcedureCallStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedSelectiveAcceptStatement* SgUntypedSelectiveAcceptStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedSelectiveAcceptStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedSelectiveAcceptStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedPackageDeclaration* SgUntypedPackageDeclaration::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, std::string name)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedPackageDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedPackageDeclaration* returnNode = NULL;

     return returnNode;
   }


SgUntypedStructureDeclaration* SgUntypedStructureDeclaration::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, std::string name)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedStructureDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedStructureDeclaration* returnNode = NULL;

     return returnNode;
   }


SgUntypedTimedEntryCallStatement* SgUntypedTimedEntryCallStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedTimedEntryCallStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedTimedEntryCallStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedEntryCallStatement* SgUntypedEntryCallStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedEntryCallStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedEntryCallStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedCodeStatement* SgUntypedCodeStatement::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedCodeStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedCodeStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3096 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedAssignmentStatement* SgUntypedAssignmentStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgUntypedExpression* lhs_operand, SgUntypedExpression* rhs_operand )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedAssignmentStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgUntypedExpression* lhs_operand, SgUntypedExpression* rhs_operand): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedAssignmentStatement* returnNode = NULL;

     return returnNode;
   }


// Static function for building IR nodes for Aterm support (like a constructor).
SgUntypedBlockStatement* SgUntypedBlockStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedBlockStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedBlockStatement* returnNode = NULL;

     return returnNode;
   }


// Static function for building IR nodes for Aterm support (like a constructor).
SgUntypedFunctionCallStatement* SgUntypedFunctionCallStatement::build_node_from_nonlist_children (Sg_File_Info*, int, SgUntypedExpression*, SgUntypedExprListExpression*)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedFunctionCallStatement::build_node_from_nonlist_children (Sg_File_Info*, int, SgUntypedExpression*, SgUntypedExprListExpression*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedFunctionCallStatement* returnNode = NULL;

     return returnNode;
   }


// Static function for building IR nodes for Aterm support (like a constructor).
SgUntypedImageControlStatement* SgUntypedImageControlStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedExpression* variable, SgUntypedExpression* expression, SgUntypedExprListExpression* status_list)
   {
#if 0
  // debugging information!
     printf ("In SgUntypedImageControlStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedExpression* variable, SgUntypedExpression* expression, SgUntypedExprListExpression* status_list): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedImageControlStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3150 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedNamedStatement* SgUntypedNamedStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int statement_enum, std::string statement_name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedNamedStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum, std::string statement_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedNamedStatement* returnNode = NULL;

     return returnNode;
   }

SgUntypedExpressionStatement* SgUntypedExpressionStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedExpression* statement_expression )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedExpressionStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum, SgUntypedExpression* statement_expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedExpressionStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3168 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedOtherStatement* SgUntypedOtherStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int statement_enum )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedOtherStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int statement_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedOtherStatement* returnNode = NULL;

     return returnNode;
   }


SgUntypedUseStatement* SgUntypedUseStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string module_name, SgToken::ROSE_Fortran_Keywords module_nature, bool isOnlyList )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedUseStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string module_name, SgToken::ROSE_Fortran_Keywords module_nature, bool isOnlyList): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedUseStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3186 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedScope* SgUntypedScope::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedScope::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedScope* returnNode = NULL;

     return returnNode;
   }


/* #line 3204 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedFunctionScope* SgUntypedFunctionScope::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedFunctionScope::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedFunctionScope* returnNode = NULL;

     return returnNode;
   }


/* #line 3222 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedModuleScope* SgUntypedModuleScope::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedModuleScope::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedModuleScope* returnNode = NULL;

     return returnNode;
   }


/* #line 3240 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedGlobalScope* SgUntypedGlobalScope::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedGlobalScope::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedGlobalScope* returnNode = NULL;

     return returnNode;
   }


/* #line 3258 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedType* SgUntypedType::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string type_name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedType::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string type_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedType* returnNode = NULL;

     return returnNode;
   }


/* #line 3276 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedArrayType* SgUntypedArrayType::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string type_name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedArrayType::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string type_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedArrayType* returnNode = NULL;

     return returnNode;
   }


/* #line 3294 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedAttribute* SgUntypedAttribute::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgToken::ROSE_Fortran_Keywords type_name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedAttribute::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgToken::ROSE_Fortran_Keywords type_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedAttribute* returnNode = NULL;

     return returnNode;
   }


/* #line 3312 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedInitializedName* SgUntypedInitializedName::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgUntypedType* type, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedInitializedName::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgUntypedType* type, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedInitializedName* returnNode = NULL;

     return returnNode;
   }


/* #line 3330 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedFile* SgUntypedFile::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedFile::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedFile* returnNode = NULL;

     return returnNode;
   }


/* #line 3348 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedStatementList* SgUntypedStatementList::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedStatementList::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedStatementList* returnNode = NULL;

     return returnNode;
   }


/* #line 3366 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedDeclarationStatementList* SgUntypedDeclarationStatementList::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedDeclarationStatementList::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedDeclarationStatementList* returnNode = NULL;

     return returnNode;
   }


/* #line 3384 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedFunctionDeclarationList* SgUntypedFunctionDeclarationList::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedFunctionDeclarationList::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedFunctionDeclarationList* returnNode = NULL;

     return returnNode;
   }


/* #line 3402 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedInitializedNameList* SgUntypedInitializedNameList::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedInitializedNameList::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedInitializedNameList* returnNode = NULL;

     return returnNode;
   }


// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedName* SgUntypedName::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedName::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedName* returnNode = NULL;

     return returnNode;
   }

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedNameList* SgUntypedNameList::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedNameList::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedNameList* returnNode = NULL;

     return returnNode;
   }

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedToken* SgUntypedToken::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string lexeme_string, unsigned int classification_code )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedToken::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string lexeme_string, unsigned int classification_code): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedToken* returnNode = NULL;

     return returnNode;
   }

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedTokenList* SgUntypedTokenList::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedTokenList::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedTokenList* returnNode = NULL;

     return returnNode;
   }


SgUntypedTokenPair* SgUntypedTokenPair::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string lexeme_string_1, unsigned int classification_code_1, std::string lexeme_string_2, unsigned int classification_code_2 )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedTokenPair::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string lexeme_string, unsigned int classification_code): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedTokenPair* returnNode = NULL;

     return returnNode;
   }

SgUntypedTokenPairList* SgUntypedTokenPairList::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedTokenPairList::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedTokenPairList* returnNode = NULL;

     return returnNode;
   }

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUntypedExprListExpression* SgUntypedExprListExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int expression_enum )
   {
#if 0
  // debugging information!
     printf ("In SgUntypedExprListExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int expression_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUntypedExprListExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 3420 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgStatement* SgStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgStatement* returnNode = NULL;

     return returnNode;
   }


// Generated static function for building IR nodes for Aterm support (like a constructor).
SgScopeStatement* SgScopeStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgScopeStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgScopeStatement* returnNode = NULL;

     return returnNode;
   }



// Generated static function for building IR nodes for Aterm support (like a constructor).
SgGlobal* SgGlobal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 1
  // debugging information!
      printf ("In SgGlobal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): \n");
#endif

     SgGlobal* returnNode = new SgGlobal();
     ROSE_ASSERT(returnNode != NULL);

     ROSE_ASSERT(startOfConstruct != NULL);
     returnNode->set_startOfConstruct(startOfConstruct);


// DQ (10/13/2014): We can't access the AtermSupport namespace under Windows.
#ifdef ROSE_USE_ROSE_ATERM_SUPPORT

     string id = AtermSupport::uniqueId(returnNode);

#if 1
     printf ("In SgGlobal::build_node_from_nonlist_children(): id = %s \n",id.c_str());
#endif

     if (AtermSupport::translationScopeMap.find(id) != AtermSupport::translationScopeMap.end())
        {
          ROSE_ASSERT(AtermSupport::translationScopeMap[id] != NULL);
#if 1
          printf ("In SgGlobal::build_node_from_nonlist_children(): id = %s in translationScopeMap: returnNode = %p = %s \n",id.c_str(),returnNode,returnNode->class_name().c_str());
#endif
          printf ("Error: The global scope should not already exist in the map. \n");
          ROSE_ASSERT(false);
        }
       else
        {
#if 1
          printf ("In SgGlobal::build_node_from_nonlist_children(): Node not found in translationNodeMap: adding scope to translationScopeMap \n");
#endif
          AtermSupport::translationScopeMap[id] = returnNode;
        }
#endif

#if 0
     printf ("Exiting as a test in SgGlobal::build_node_from_nonlist_children() \n");
     ROSE_ASSERT(false);
#endif

     return returnNode;
   }


/* #line 3474 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBasicBlock* SgBasicBlock::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgBasicBlock::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBasicBlock* returnNode = NULL;

     ROSE_ASSERT(startOfConstruct != NULL);

     returnNode = SageBuilder::buildBasicBlock_nfi();
     ROSE_ASSERT(returnNode != NULL);

     returnNode->set_startOfConstruct(startOfConstruct);

     return returnNode;
   }


/* #line 3492 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIfStmt* SgIfStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* conditional, SgStatement* true_body, SgStatement* false_body )
   {
#if 0
  // debugging information!
     printf ("In SgIfStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* conditional, SgStatement* true_body, SgStatement* false_body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIfStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 3510 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgForStatement* SgForStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* test, SgExpression* increment, SgStatement* loop_body )
   {
#if 0
  // debugging information!
     printf ("In SgForStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* test, SgExpression* increment, SgStatement* loop_body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgForStatement* returnNode = NULL;

     return returnNode;
   }

// DQ (3/28/2018): Added support for new IR nodes specific to range-based for loop in C++11.
SgRangeBasedForStatement* SgRangeBasedForStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgVariableDeclaration* iterator_declaration, SgVariableDeclaration* range_declaration, SgVariableDeclaration* begin_declaration, SgVariableDeclaration* end_declaration, SgExpression* not_equal_expression, SgExpression* increment_expression, SgStatement* loop_body )
   {
#if 0
  // debugging information!
     printf ("In SgRangeBasedForStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgVariableDeclaration* iterator_declaration, SgVariableDeclaration* range_declaration, SgVariableDeclaration* begin_declaration, SgVariableDeclaration* end_declaration, SgExpression* not_equal_expression, SgExpression* increment_expression, SgStatement* loop_body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgRangeBasedForStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3528 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionDefinition* SgFunctionDefinition::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgBasicBlock* body )
   {
#if 0
  // debugging information!
     printf ("In SgFunctionDefinition::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgBasicBlock* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionDefinition* returnNode = NULL;

     return returnNode;
   }


/* #line 3546 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateFunctionDefinition* SgTemplateFunctionDefinition::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgBasicBlock* body )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateFunctionDefinition::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgBasicBlock* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateFunctionDefinition* returnNode = NULL;

     return returnNode;
   }


/* #line 3564 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgClassDefinition* SgClassDefinition::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgClassDefinition::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgClassDefinition* returnNode = NULL;

     return returnNode;
   }


/* #line 3582 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateInstantiationDefn* SgTemplateInstantiationDefn::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateInstantiationDefn::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateInstantiationDefn* returnNode = NULL;

     return returnNode;
   }


/* #line 3600 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateClassDefinition* SgTemplateClassDefinition::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateClassDefinition::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateClassDefinition* returnNode = NULL;

     return returnNode;
   }


/* #line 3618 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgWhileStmt* SgWhileStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* condition, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgWhileStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* condition, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgWhileStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 3636 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDoWhileStmt* SgDoWhileStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body, SgStatement* condition )
   {
#if 0
  // debugging information!
     printf ("In SgDoWhileStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body, SgStatement* condition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDoWhileStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 3654 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSwitchStatement* SgSwitchStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* item_selector, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgSwitchStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* item_selector, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSwitchStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3672 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCatchOptionStmt* SgCatchOptionStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgVariableDeclaration* condition, SgStatement* body, SgTryStmt* trystmt )
   {
#if 0
  // debugging information!
     printf ("In SgCatchOptionStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgVariableDeclaration* condition, SgStatement* body, SgTryStmt* trystmt): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCatchOptionStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 3690 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNamespaceDefinitionStatement* SgNamespaceDefinitionStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgNamespaceDeclarationStatement* namespaceDeclaration )
   {
#if 0
  // debugging information!
     printf ("In SgNamespaceDefinitionStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgNamespaceDeclarationStatement* namespaceDeclaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNamespaceDefinitionStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3708 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBlockDataStatement* SgBlockDataStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgBlockDataStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBlockDataStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3726 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAssociateStatement* SgAssociateStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgAssociateStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAssociateStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3744 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFortranDo* SgFortranDo::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* initialization, SgExpression* bound, SgExpression* increment, SgBasicBlock* body )
   {
#if 0
  // debugging information!
     printf ("In SgFortranDo::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* initialization, SgExpression* bound, SgExpression* increment, SgBasicBlock* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFortranDo* returnNode = NULL;

     return returnNode;
   }


// Static function for building IR nodes for Aterm support (like a constructor).
SgFortranNonblockedDo* SgFortranNonblockedDo::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* initialization, SgExpression* bound, SgExpression* increment, SgBasicBlock* body, SgStatement* end_statement )
   {
#if 0
  // debugging information!
     printf ("In SgFortranNonblockedDo::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* initialization, SgExpression* bound, SgExpression* increment, SgBasicBlock* body, SgStatement* end_statement): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFortranNonblockedDo* returnNode = NULL;

     return returnNode;
   }


// Static function for building IR nodes for Aterm support (like a constructor).
SgForAllStatement* SgForAllStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExprListExp* forall_header, SgBasicBlock* body )
   {
#if 0
  // debugging information!
     printf ("In SgForAllStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExprListExp* forall_header, SgBasicBlock* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgForAllStatement* returnNode = NULL;

     return returnNode;
   }


// Jovial statements
//

SgJovialForThenStatement* SgJovialForThenStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* initialization, SgExpression* then_expression, SgExpression* where_expression, SgBasicBlock* loop_body)
   {
#if 0
  // debugging information!
     printf ("In SgJovialForThenStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* initialization, SgExpression* then_expression, SgExpression* where_expression, SgBasicBlock* loop_body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJovialForThenStatement* returnNode = NULL;

     return returnNode;
   }

SgJovialDefineDeclaration* SgJovialDefineDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string define_string)
   {
#if 0
  // debugging information!
     printf ("In SgJovialDefineDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string define_string): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJovialDefineDeclaration* returnNode = NULL;

     return returnNode;
   }

SgJovialDirectiveStatement* SgJovialDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string directive_content, SgJovialDirectiveStatement::directive_types directive_type)
   {
#if 0
  // debugging information!
     printf ("In SgJovialDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string directive_content, SgJovialDirectiveStatement::directive_types directive_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJovialDirectiveStatement* returnNode = NULL;

     return returnNode;
   }

SgJovialCompoolStatement* SgJovialCompoolStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name)
   {
#if 0
  // debugging information!
     printf ("In SgJovialCompoolStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJovialCompoolStatement* returnNode = NULL;

     return returnNode;
   }

SgJovialTableStatement* SgJovialTableStatement::build_node_from_nonlist_children (Sg_File_Info*, SgName, SgClassDeclaration::class_types, SgClassType*, SgClassDefinition*)
   {
#if 0
  // debugging information!
     printf ("In SgJovialTableStatement::build_node_from_nonlist_children (Sg_File_Info*, SgName, SgClassDeclaration::class_types, SgClassType*, SgClassDefinition*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJovialTableStatement* returnNode = NULL;

     return returnNode;
   }

/* #line 3798 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUpcForAllStatement* SgUpcForAllStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* test, SgExpression* increment, SgExpression* affinity, SgStatement* loop_body )
   {
#if 0
  // debugging information!
     printf ("In SgUpcForAllStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* test, SgExpression* increment, SgExpression* affinity, SgStatement* loop_body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUpcForAllStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3816 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCAFWithTeamStatement* SgCAFWithTeamStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgVarRefExp* teamId, SgBasicBlock* body )
   {
#if 0
  // debugging information!
     printf ("In SgCAFWithTeamStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgVarRefExp* teamId, SgBasicBlock* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCAFWithTeamStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3834 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaForEachStatement* SgJavaForEachStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgVariableDeclaration* element, SgExpression* collection, SgStatement* loop_body )
   {
#if 0
  // debugging information!
     printf ("In SgJavaForEachStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgVariableDeclaration* element, SgExpression* collection, SgStatement* loop_body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaForEachStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3852 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaLabelStatement* SgJavaLabelStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName label, SgStatement* statement )
   {
#if 0
  // debugging information!
     printf ("In SgJavaLabelStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName label, SgStatement* statement): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaLabelStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3870 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionTypeTable* SgFunctionTypeTable::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgSymbolTable* function_type_table )
   {
#if 0
  // debugging information!
     printf ("In SgFunctionTypeTable::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgSymbolTable* function_type_table): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionTypeTable* returnNode = NULL;

     return returnNode;
   }


/* #line 3888 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDeclarationStatement* SgDeclarationStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgDeclarationStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDeclarationStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3906 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionParameterList* SgFunctionParameterList::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 1
  // debugging information!
     printf ("In SgFunctionParameterList::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct) \n");
#endif

     SgFunctionParameterList* returnNode = NULL;

     returnNode = SageBuilder::buildFunctionParameterList_nfi();
     ROSE_ASSERT(returnNode != NULL);

     ROSE_ASSERT(startOfConstruct != NULL);
     returnNode->set_startOfConstruct(startOfConstruct);

     return returnNode;
   }


/* #line 3924 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVariableDeclaration* SgVariableDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgVariableDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVariableDeclaration* returnNode = NULL;

     ROSE_ASSERT(startOfConstruct != NULL);

     returnNode = new SgVariableDeclaration(startOfConstruct);
     ROSE_ASSERT(returnNode != NULL);

     return returnNode;
   }


/* #line 3942 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateVariableDeclaration* SgTemplateVariableDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateVariableDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateVariableDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 3960 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVariableDefinition* SgVariableDefinition::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgInitializedName* vardefn, SgExpression* bitfield )
   {
#if 0
  // debugging information!
     printf ("In SgVariableDefinition::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgInitializedName* vardefn, SgValueExp* bitfield): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVariableDefinition* returnNode = NULL;

     return returnNode;
   }


/* #line 3978 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgClinkageDeclarationStatement* SgClinkageDeclarationStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgClinkageDeclarationStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgClinkageDeclarationStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 3996 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgClinkageStartStatement* SgClinkageStartStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgClinkageStartStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgClinkageStartStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4014 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgClinkageEndStatement* SgClinkageEndStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgClinkageEndStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgClinkageEndStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4032 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEnumDeclaration* SgEnumDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgEnumType* type )
   {
#if 0
  // debugging information!
     printf ("In SgEnumDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgEnumType* type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEnumDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 4050 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmStmt* SgAsmStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgAsmStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 4068 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAttributeSpecificationStatement* SgAttributeSpecificationStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgAttributeSpecificationStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAttributeSpecificationStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4086 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFormatStatement* SgFormatStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgFormatItemList* format_item_list )
   {
#if 0
  // debugging information!
     printf ("In SgFormatStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgFormatItemList* format_item_list): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFormatStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4104 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateDeclaration* SgTemplateDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 4122 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateInstantiationDirectiveStatement* SgTemplateInstantiationDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgDeclarationStatement* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateInstantiationDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgDeclarationStatement* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateInstantiationDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4140 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUseStatement* SgUseStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, bool only_option )
   {
#if 0
  // debugging information!
     printf ("In SgUseStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, bool only_option): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUseStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4158 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgParameterStatement* SgParameterStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgParameterStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgParameterStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4176 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNamespaceDeclarationStatement* SgNamespaceDeclarationStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgNamespaceDefinitionStatement* definition, bool isUnnamedNamespace )
   {
#if 0
  // debugging information!
     printf ("In SgNamespaceDeclarationStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgNamespaceDefinitionStatement* definition, bool isUnnamedNamespace): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNamespaceDeclarationStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4194 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEquivalenceStatement* SgEquivalenceStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgEquivalenceStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEquivalenceStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4212 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgInterfaceStatement* SgInterfaceStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgInterfaceStatement::generic_spec_enum generic_spec )
   {
#if 0
  // debugging information!
     printf ("In SgInterfaceStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgInterfaceStatement::generic_spec_enum generic_spec): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgInterfaceStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4230 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNamespaceAliasDeclarationStatement* SgNamespaceAliasDeclarationStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgNamespaceDeclarationStatement* namespaceDeclaration )
   {
#if 0
  // debugging information!
     printf ("In SgNamespaceAliasDeclarationStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgNamespaceDeclarationStatement* namespaceDeclaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNamespaceAliasDeclarationStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4248 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCommonBlock* SgCommonBlock::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgCommonBlock::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCommonBlock* returnNode = NULL;

     return returnNode;
   }


/* #line 4266 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypedefDeclaration* SgTypedefDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgType* base_type, SgTypedefType* type, SgDeclarationStatement* declaration, SgSymbol* parent_scope )
   {
#if 0
  // debugging information!
     printf ("In SgTypedefDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgType* base_type, SgTypedefType* type, SgDeclarationStatement* declaration, SgSymbol* parent_scope): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypedefDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 4284 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgStatementFunctionStatement* SgStatementFunctionStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgFunctionDeclaration* function, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgStatementFunctionStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgFunctionDeclaration* function, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgStatementFunctionStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4302 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCtorInitializerList* SgCtorInitializerList::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgCtorInitializerList::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCtorInitializerList* returnNode = NULL;

     return returnNode;
   }


/* #line 4320 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPragmaDeclaration* SgPragmaDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgPragma* pragma )
   {
#if 0
  // debugging information!
     printf ("In SgPragmaDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgPragma* pragma): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPragmaDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 4338 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUsingDirectiveStatement* SgUsingDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgNamespaceDeclarationStatement* namespaceDeclaration )
   {
#if 0
  // debugging information!
     printf ("In SgUsingDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgNamespaceDeclarationStatement* namespaceDeclaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUsingDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4356 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgClassDeclaration* SgClassDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgClassDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgClassDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 4374 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateClassDeclaration* SgTemplateClassDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateClassDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateClassDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 4392 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateInstantiationDecl* SgTemplateInstantiationDecl::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition, SgTemplateClassDeclaration* templateDeclaration, SgTemplateArgumentPtrList templateArguments )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateInstantiationDecl::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition, SgTemplateClassDeclaration* templateDeclaration, SgTemplateArgumentPtrList templateArguments): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateInstantiationDecl* returnNode = NULL;

     return returnNode;
   }


/* #line 4410 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDerivedTypeStatement* SgDerivedTypeStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgDerivedTypeStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDerivedTypeStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4428 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgModuleStatement* SgModuleStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgModuleStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgModuleStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4446 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaPackageDeclaration* SgJavaPackageDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgJavaPackageDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgClassDeclaration::class_types class_type, SgClassType* type, SgClassDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaPackageDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 4464 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgImplicitStatement* SgImplicitStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, bool implicit_none )
   {
#if 0
  // debugging information!
     printf ("In SgImplicitStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, bool implicit_none): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgImplicitStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4482 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUsingDeclarationStatement* SgUsingDeclarationStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgDeclarationStatement* declaration, SgInitializedName* initializedName )
   {
#if 0
  // debugging information!
     printf ("In SgUsingDeclarationStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgDeclarationStatement* declaration, SgInitializedName* initializedName): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUsingDeclarationStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4500 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNamelistStatement* SgNamelistStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgNamelistStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNamelistStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4518 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgImportStatement* SgImportStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgImportStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgImportStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4536 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionDeclaration* SgFunctionDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct,  SgName name, SgFunctionType* type, SgFunctionDefinition* definition )
   {
     printf ("In SgFunctionDeclaration::build_node_from_nonlist_children(): default function not used \n");
     ROSE_ASSERT(false);

     return NULL;
   }

// DQ (10/11/2014): Adding special case support for aterm translation to ROSE AST.
SgFunctionDeclaration* SgFunctionDeclaration::build_node_from_nonlist_children(SgType* return_type, SgFunctionParameterList* parameterList, SgScopeStatement* scope, Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition)
   {
#if 0
  // debugging information!
     printf ("In SgFunctionDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionDeclaration* returnNode = NULL;

     if (definition == NULL)
        {
       // Trying to use: SgFunctionDeclaration* buildNondefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, 
       //             SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList);
       // But we don't have all of the inputs.
       // I need a build function that takes the inputs to this function, or I need to change the inputs to this function by altering which data members are
       // marked as constructor inputs.

       // SgScopeStatement* scope         = NULL;
          SgExprListExp* decoratorList    = NULL;
          bool buildTemplateInstantiation = false;
          SgTemplateArgumentPtrList* templateArgumentsList = NULL;

          ROSE_ASSERT(scope != NULL);

          returnNode = SageBuilder::buildNondefiningFunctionDeclaration (name,return_type,parameterList,scope,decoratorList,buildTemplateInstantiation,templateArgumentsList);
        }
       else
        {
          printf ("In SgFunctionDeclaration::build_node_from_nonlist_children(): definition != NULL (not implemented yet) \n");
          ROSE_ASSERT(false);
        }

     return returnNode;
   }


/* #line 4554 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateFunctionDeclaration* SgTemplateFunctionDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition )
// SgTemplateFunctionDeclaration* SgTemplateFunctionDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionParameterList*, SgFunctionType* type, SgFunctionDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateFunctionDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateFunctionDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 4572 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMemberFunctionDeclaration* SgMemberFunctionDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition )
// SgMemberFunctionDeclaration* SgMemberFunctionDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionParameterList*, SgFunctionType* type, SgFunctionDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgMemberFunctionDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMemberFunctionDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 4590 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateMemberFunctionDeclaration* SgTemplateMemberFunctionDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition )
// SgTemplateMemberFunctionDeclaration* SgTemplateMemberFunctionDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionParameterList*, SgFunctionType* type, SgFunctionDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateMemberFunctionDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateMemberFunctionDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 4608 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateInstantiationMemberFunctionDecl* SgTemplateInstantiationMemberFunctionDecl::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition, SgTemplateMemberFunctionDeclaration* templateDeclaration, SgTemplateArgumentPtrList templateArguments )
// SgTemplateInstantiationMemberFunctionDecl* SgTemplateInstantiationMemberFunctionDecl::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionParameterList*, SgFunctionType* type, SgFunctionDefinition* definition, SgTemplateMemberFunctionDeclaration* templateDeclaration, SgTemplateArgumentPtrList templateArguments )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateInstantiationMemberFunctionDecl::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition, SgTemplateMemberFunctionDeclaration* templateDeclaration, SgTemplateArgumentPtrList templateArguments): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateInstantiationMemberFunctionDecl* returnNode = NULL;

     return returnNode;
   }


/* #line 4626 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
// SgTemplateInstantiationFunctionDecl* SgTemplateInstantiationFunctionDecl::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionParameterList*, SgFunctionType* type, SgFunctionDefinition* definition, SgTemplateFunctionDeclaration* templateDeclaration, SgTemplateArgumentPtrList templateArguments )
SgTemplateInstantiationFunctionDecl* SgTemplateInstantiationFunctionDecl::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition, SgTemplateFunctionDeclaration* templateDeclaration, SgTemplateArgumentPtrList templateArguments )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateInstantiationFunctionDecl::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition, SgTemplateFunctionDeclaration* templateDeclaration, SgTemplateArgumentPtrList templateArguments): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateInstantiationFunctionDecl* returnNode = NULL;

     return returnNode;
   }


/* #line 4644 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgProgramHeaderStatement* SgProgramHeaderStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition )
// SgProgramHeaderStatement* SgProgramHeaderStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionParameterList*, SgFunctionType* type, SgFunctionDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgProgramHeaderStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgProgramHeaderStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4662 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgProcedureHeaderStatement* SgProcedureHeaderStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition )
// SgProcedureHeaderStatement* SgProcedureHeaderStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionParameterList*, SgFunctionType* type, SgFunctionDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgProcedureHeaderStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgProcedureHeaderStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4680 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEntryStatement* SgEntryStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition )
// SgEntryStatement* SgEntryStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgFunctionParameterList*, SgFunctionType* type, SgFunctionDefinition* definition )
   {
#if 0
  // debugging information!
     printf ("In SgEntryStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgFunctionType* type, SgFunctionDefinition* definition): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEntryStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4698 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgContainsStatement* SgContainsStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgContainsStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgContainsStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4716 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgC_PreprocessorDirectiveStatement* SgC_PreprocessorDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgC_PreprocessorDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgC_PreprocessorDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4734 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIncludeDirectiveStatement* SgIncludeDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgIncludeDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIncludeDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4752 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDefineDirectiveStatement* SgDefineDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgDefineDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDefineDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4770 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUndefDirectiveStatement* SgUndefDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUndefDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUndefDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4788 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIfdefDirectiveStatement* SgIfdefDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgIfdefDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIfdefDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4806 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIfndefDirectiveStatement* SgIfndefDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgIfndefDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIfndefDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4824 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIfDirectiveStatement* SgIfDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgIfDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIfDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4842 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDeadIfDirectiveStatement* SgDeadIfDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgDeadIfDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDeadIfDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4860 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgElseDirectiveStatement* SgElseDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgElseDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgElseDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4878 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgElseifDirectiveStatement* SgElseifDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgElseifDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgElseifDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4896 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEndifDirectiveStatement* SgEndifDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgEndifDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEndifDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4914 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLineDirectiveStatement* SgLineDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgLineDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLineDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4932 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgWarningDirectiveStatement* SgWarningDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgWarningDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgWarningDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4950 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgErrorDirectiveStatement* SgErrorDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgErrorDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgErrorDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4968 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEmptyDirectiveStatement* SgEmptyDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgEmptyDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEmptyDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 4986 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIncludeNextDirectiveStatement* SgIncludeNextDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgIncludeNextDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIncludeNextDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5004 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIdentDirectiveStatement* SgIdentDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgIdentDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIdentDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5022 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLinemarkerDirectiveStatement* SgLinemarkerDirectiveStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgLinemarkerDirectiveStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLinemarkerDirectiveStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5040 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpThreadprivateStatement* SgOmpThreadprivateStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgOmpThreadprivateStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpThreadprivateStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5058 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFortranIncludeLine* SgFortranIncludeLine::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string filename )
   {
#if 0
  // debugging information!
     printf ("In SgFortranIncludeLine::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string filename): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFortranIncludeLine* returnNode = NULL;

     return returnNode;
   }


/* #line 5076 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaImportStatement* SgJavaImportStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName path, bool containsWildCard )
   {
#if 0
  // debugging information!
     printf ("In SgJavaImportStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName path, bool containsWildCard): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaImportStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5094 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaPackageStatement* SgJavaPackageStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name )
   {
#if 0
  // debugging information!
     printf ("In SgJavaPackageStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaPackageStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5112 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgStmtDeclarationStatement* SgStmtDeclarationStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* statement )
   {
#if 0
  // debugging information!
     printf ("In SgStmtDeclarationStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* statement): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgStmtDeclarationStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5130 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgStaticAssertionDeclaration* SgStaticAssertionDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* condition, SgName string_literal )
   {
#if 0
  // debugging information!
     printf ("In SgStaticAssertionDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* condition, SgName string_literal): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgStaticAssertionDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 5148 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMicrosoftAttributeDeclaration* SgMicrosoftAttributeDeclaration::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName attribute_string )
   {
#if 0
  // debugging information!
     printf ("In SgMicrosoftAttributeDeclaration::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName attribute_string): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMicrosoftAttributeDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 5166 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgExprStatement* SgExprStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgExprStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgExprStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5184 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLabelStatement* SgLabelStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName label, SgStatement* statement )
   {
#if 0
  // debugging information!
     printf ("In SgLabelStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName label, SgStatement* statement): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLabelStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5202 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCaseOptionStmt* SgCaseOptionStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* key, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgCaseOptionStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* key, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCaseOptionStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 5220 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTryStmt* SgTryStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgTryStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTryStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 5238 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDefaultOptionStmt* SgDefaultOptionStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgDefaultOptionStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDefaultOptionStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 5256 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBreakStmt* SgBreakStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgBreakStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBreakStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 5274 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgContinueStmt* SgContinueStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgContinueStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgContinueStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 5292 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgReturnStmt* SgReturnStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgReturnStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgReturnStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 5310 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgGotoStatement* SgGotoStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgLabelStatement* label )
   {
#if 0
  // debugging information!
     printf ("In SgGotoStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgLabelStatement* label): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgGotoStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5328 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSpawnStmt* SgSpawnStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgFunctionCallExp* the_func )
   {
#if 0
  // debugging information!
     printf ("In SgSpawnStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgFunctionCallExp* the_func): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSpawnStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 5346 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNullStatement* SgNullStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgNullStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNullStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5364 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVariantStatement* SgVariantStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgVariantStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVariantStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5382 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgForInitStatement* SgForInitStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgForInitStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgForInitStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5400 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCatchStatementSeq* SgCatchStatementSeq::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgCatchStatementSeq::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCatchStatementSeq* returnNode = NULL;

     return returnNode;
   }


/* #line 5418 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgStopOrPauseStatement* SgStopOrPauseStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* code )
   {
#if 0
  // debugging information!
     printf ("In SgStopOrPauseStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* code): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgStopOrPauseStatement* returnNode = NULL;

     return returnNode;
   }


// Static function for building IR nodes for Aterm support (like a constructor).
SgSyncAllStatement* SgSyncAllStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgSyncAllStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSyncAllStatement* returnNode = NULL;

     return returnNode;
   }

// Static function for building IR nodes for Aterm support (like a constructor).
SgSyncImagesStatement* SgSyncImagesStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* image_set )
   {
#if 0
  // debugging information!
     printf ("In SgSyncImagesStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* image_set): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSyncImagesStatement* returnNode = NULL;

     return returnNode;
   }

// Static function for building IR nodes for Aterm support (like a constructor).
SgSyncMemoryStatement* SgSyncMemoryStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgSyncMemoryStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSyncMemoryStatement* returnNode = NULL;

     return returnNode;
   }

// Static function for building IR nodes for Aterm support (like a constructor).
SgSyncTeamStatement* SgSyncTeamStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* team_value )
   {
#if 0
  // debugging information!
     printf ("In SgSyncTeamStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* team_value): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSyncTeamStatement* returnNode = NULL;

     return returnNode;
   }

// Static function for building IR nodes for Aterm support (like a constructor).
SgLockStatement* SgLockStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lock_variable )
   {
#if 0
  // debugging information!
     printf ("In SgLockStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lock_variable): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLockStatement* returnNode = NULL;

     return returnNode;
   }

// Static function for building IR nodes for Aterm support (like a constructor).
SgUnlockStatement* SgUnlockStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lock_variable )
   {
#if 0
  // debugging information!
     printf ("In SgUnlockStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lock_variable): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnlockStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5436 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIOStatement* SgIOStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgIOStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIOStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5454 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPrintStatement* SgPrintStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgPrintStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPrintStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5472 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgReadStatement* SgReadStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgReadStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgReadStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5490 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgWriteStatement* SgWriteStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgWriteStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgWriteStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5508 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOpenStatement* SgOpenStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgOpenStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOpenStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5526 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCloseStatement* SgCloseStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgCloseStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCloseStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5544 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgInquireStatement* SgInquireStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgInquireStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgInquireStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5562 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFlushStatement* SgFlushStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgFlushStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFlushStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5580 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBackspaceStatement* SgBackspaceStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgBackspaceStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBackspaceStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5598 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgRewindStatement* SgRewindStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgRewindStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgRewindStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5616 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEndfileStatement* SgEndfileStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgEndfileStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEndfileStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5634 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgWaitStatement* SgWaitStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgWaitStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgWaitStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5652 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgWhereStatement* SgWhereStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* condition, SgBasicBlock* body, SgElseWhereStatement* elsewhere )
   {
#if 0
  // debugging information!
     printf ("In SgWhereStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* condition, SgBasicBlock* body, SgElseWhereStatement* elsewhere): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgWhereStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5670 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgElseWhereStatement* SgElseWhereStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* condition, SgBasicBlock* body, SgElseWhereStatement* elsewhere )
   {
#if 0
  // debugging information!
     printf ("In SgElseWhereStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* condition, SgBasicBlock* body, SgElseWhereStatement* elsewhere): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgElseWhereStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5688 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNullifyStatement* SgNullifyStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgNullifyStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNullifyStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5706 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgArithmeticIfStatement* SgArithmeticIfStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* conditional, SgLabelRefExp* less_label, SgLabelRefExp* equal_label, SgLabelRefExp* greater_label )
   {
#if 0
  // debugging information!
     printf ("In SgArithmeticIfStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* conditional, SgLabelRefExp* less_label, SgLabelRefExp* equal_label, SgLabelRefExp* greater_label): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgArithmeticIfStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5724 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAssignStatement* SgAssignStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgLabelRefExp* label, SgExpression* value )
   {
#if 0
  // debugging information!
     printf ("In SgAssignStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgLabelRefExp* label, SgExpression* value): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAssignStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5742 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgComputedGotoStatement* SgComputedGotoStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExprListExp* labelList, SgExpression* label_index )
   {
#if 0
  // debugging information!
     printf ("In SgComputedGotoStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExprListExp* labelList, SgExpression* label_index): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgComputedGotoStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5760 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAssignedGotoStatement* SgAssignedGotoStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExprListExp* targets )
   {
#if 0
  // debugging information!
     printf ("In SgAssignedGotoStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExprListExp* targets): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAssignedGotoStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5778 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAllocateStatement* SgAllocateStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgAllocateStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAllocateStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5796 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDeallocateStatement* SgDeallocateStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgDeallocateStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDeallocateStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5814 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUpcNotifyStatement* SgUpcNotifyStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* notify_expression )
   {
#if 0
  // debugging information!
     printf ("In SgUpcNotifyStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* notify_expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUpcNotifyStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5832 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUpcWaitStatement* SgUpcWaitStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* wait_expression )
   {
#if 0
  // debugging information!
     printf ("In SgUpcWaitStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* wait_expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUpcWaitStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5850 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUpcBarrierStatement* SgUpcBarrierStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* barrier_expression )
   {
#if 0
  // debugging information!
     printf ("In SgUpcBarrierStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* barrier_expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUpcBarrierStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5868 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUpcFenceStatement* SgUpcFenceStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUpcFenceStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUpcFenceStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5886 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpBarrierStatement* SgOmpBarrierStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgOmpBarrierStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpBarrierStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5904 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpTaskwaitStatement* SgOmpTaskwaitStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgOmpTaskwaitStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpTaskwaitStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5922 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpFlushStatement* SgOmpFlushStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgOmpFlushStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpFlushStatement* returnNode = NULL;

     return returnNode;
   }

SgOmpDeclareSimdStatement* SgOmpDeclareSimdStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
     SgOmpDeclareSimdStatement* returnNode = NULL;

     return returnNode;
   }



/* #line 5940 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpBodyStatement* SgOmpBodyStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpBodyStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpBodyStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5958 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpAtomicStatement* SgOmpAtomicStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpAtomicStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpAtomicStatement* returnNode = NULL;

     return returnNode;
   }

// Rasmussen (05/24/2017): not actually generated but should be
// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpAtomicClause* SgOmpAtomicClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgOmpClause::omp_atomic_clause_enum clause_enum )
   {
#if 0
  // debugging information!
     printf ("In SgOmpAtomicClause::build_node_from_nonlist_children (Sg_File_Info*, SgOmpClause::omp_atomic_clause_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpAtomicClause* returnNode = NULL;

     return returnNode;
   }

// Rasmussen (05/24/2017): not actually generated but should be
// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpProcBindClause* SgOmpProcBindClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgOmpClause::omp_proc_bind_policy_enum policy_enum)
   {
#if 0
  // debugging information!
     printf ("In SgOmpProcBindClause::build_node_from_nonlist_children (Sg_File_Info*, SgOmpClause::omp_proc_bind_policy_enum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpProcBindClause* returnNode = NULL;

     return returnNode;
   }

// Rasmussen (05/25/2017): not actually generated but should be
// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpNotinbranchClause* SgOmpNotinbranchClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgOmpNotinbranchClause::build_node_from_nonlist_children (Sg_File_Info*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpNotinbranchClause* returnNode = NULL;

     return returnNode;
   }

// Rasmussen (05/25/2017): not actually generated but should be
// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpInbranchClause* SgOmpInbranchClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgOmpInbranchClause::build_node_from_nonlist_children (Sg_File_Info*): this = %p = %s \n",this,this->class_name().c_str());
#endif

      SgOmpInbranchClause* returnNode = NULL;

      return returnNode;
}

// Rasmussen (05/25/2017): not actually generated but should be
// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpSimdlenClause* SgOmpSimdlenClause::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expr )
   {
#if 0
  // debugging information!
     printf ("In SgOmpSimdlenClause::build_node_from_nonlist_children (Sg_File_Info*, SgExpression*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpSimdlenClause* returnNode = NULL;

     return returnNode;
   }

/* #line 5976 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpMasterStatement* SgOmpMasterStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpMasterStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpMasterStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 5994 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpOrderedStatement* SgOmpOrderedStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpOrderedStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpOrderedStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6012 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpCriticalStatement* SgOmpCriticalStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body, SgName name )
   {
#if 0
  // debugging information!
     printf ("In SgOmpCriticalStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body, SgName name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpCriticalStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6030 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpSectionStatement* SgOmpSectionStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpSectionStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpSectionStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6048 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpWorkshareStatement* SgOmpWorkshareStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpWorkshareStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpWorkshareStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6066 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpClauseBodyStatement* SgOmpClauseBodyStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpClauseBodyStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpClauseBodyStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6084 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpParallelStatement* SgOmpParallelStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpParallelStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpParallelStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6102 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpSingleStatement* SgOmpSingleStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpSingleStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpSingleStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6120 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpTaskStatement* SgOmpTaskStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpTaskStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpTaskStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6138 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpForStatement* SgOmpForStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpForStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpForStatement* returnNode = NULL;

     return returnNode;
   }
SgOmpForSimdStatement* SgOmpForSimdStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpForSimdStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpForSimdStatement* returnNode = NULL;

     return returnNode;
   }



/* #line 6156 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpDoStatement* SgOmpDoStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpDoStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpDoStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6174 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpSectionsStatement* SgOmpSectionsStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpSectionsStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpSectionsStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6192 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpTargetStatement* SgOmpTargetStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpTargetStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpTargetStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6210 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpTargetDataStatement* SgOmpTargetDataStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpTargetDataStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpTargetDataStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6228 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOmpSimdStatement* SgOmpSimdStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgOmpSimdStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOmpSimdStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6246 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSequenceStatement* SgSequenceStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgSequenceStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSequenceStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6264 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgWithStatement* SgWithStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgWithStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgWithStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6282 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPythonPrintStmt* SgPythonPrintStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* destination, SgExprListExp* values )
   {
#if 0
  // debugging information!
     printf ("In SgPythonPrintStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* destination, SgExprListExp* values): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPythonPrintStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 6300 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPassStatement* SgPassStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgPassStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPassStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6318 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAssertStmt* SgAssertStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* test )
   {
#if 0
  // debugging information!
     printf ("In SgAssertStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* test): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAssertStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 6336 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgExecStatement* SgExecStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* executable, SgExpression* globals, SgExpression* locals )
   {
#if 0
  // debugging information!
     printf ("In SgExecStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* executable, SgExpression* globals, SgExpression* locals): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgExecStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6354 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPythonGlobalStmt* SgPythonGlobalStmt::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgPythonGlobalStmt::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPythonGlobalStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 6372 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaThrowStatement* SgJavaThrowStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgThrowOp* throwOp )
   {
#if 0
  // debugging information!
     printf ("In SgJavaThrowStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgThrowOp* throwOp): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaThrowStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6390 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaSynchronizedStatement* SgJavaSynchronizedStatement::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression, SgStatement* body )
   {
#if 0
  // debugging information!
     printf ("In SgJavaSynchronizedStatement::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression, SgStatement* body): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaSynchronizedStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 6408 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgExpression* SgExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 6426 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUnaryOp* SgUnaryOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgUnaryOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnaryOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6444 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgExpressionRoot* SgExpressionRoot::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgExpressionRoot::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgExpressionRoot* returnNode = NULL;

     return returnNode;
   }


/* #line 6462 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMinusOp* SgMinusOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgMinusOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMinusOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6480 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUnaryAddOp* SgUnaryAddOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgUnaryAddOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnaryAddOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6498 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNotOp* SgNotOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgNotOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNotOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6516 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPointerDerefExp* SgPointerDerefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgPointerDerefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPointerDerefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 6534 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAddressOfOp* SgAddressOfOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgAddressOfOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAddressOfOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6552 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMinusMinusOp* SgMinusMinusOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgMinusMinusOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMinusMinusOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6570 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPlusPlusOp* SgPlusPlusOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgPlusPlusOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPlusPlusOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6588 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBitComplementOp* SgBitComplementOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgBitComplementOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBitComplementOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6606 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCastExp* SgCastExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type, SgCastExp::cast_type_enum cast_type )
   {
#if 0
  // debugging information!
     printf ("In SgCastExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type, SgCastExp::cast_type_enum cast_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCastExp* returnNode = NULL;

     return returnNode;
   }


/* #line 6624 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgThrowOp* SgThrowOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type, SgThrowOp::e_throw_kind throwKind )
   {
#if 0
  // debugging information!
     printf ("In SgThrowOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type, SgThrowOp::e_throw_kind throwKind): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgThrowOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6642 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgRealPartOp* SgRealPartOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgRealPartOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgRealPartOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6660 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgImagPartOp* SgImagPartOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgImagPartOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgImagPartOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6678 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgConjugateOp* SgConjugateOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgConjugateOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgConjugateOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6696 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUserDefinedUnaryOp* SgUserDefinedUnaryOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type, SgName operator_name, SgFunctionSymbol* symbol )
   {
#if 0
  // debugging information!
     printf ("In SgUserDefinedUnaryOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type, SgName operator_name, SgFunctionSymbol* symbol): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUserDefinedUnaryOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6714 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBinaryOp* SgBinaryOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgBinaryOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBinaryOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6732 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgArrowExp* SgArrowExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgArrowExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgArrowExp* returnNode = NULL;

     return returnNode;
   }


/* #line 6750 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDotExp* SgDotExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgDotExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDotExp* returnNode = NULL;

     return returnNode;
   }


/* #line 6768 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDotStarOp* SgDotStarOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgDotStarOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDotStarOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6786 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgArrowStarOp* SgArrowStarOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgArrowStarOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgArrowStarOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6804 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEqualityOp* SgEqualityOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgEqualityOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEqualityOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6822 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLessThanOp* SgLessThanOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgLessThanOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLessThanOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6840 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgGreaterThanOp* SgGreaterThanOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgGreaterThanOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgGreaterThanOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6858 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNotEqualOp* SgNotEqualOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgNotEqualOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNotEqualOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6876 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLessOrEqualOp* SgLessOrEqualOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgLessOrEqualOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLessOrEqualOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6894 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgGreaterOrEqualOp* SgGreaterOrEqualOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgGreaterOrEqualOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgGreaterOrEqualOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6912 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAddOp* SgAddOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgAddOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAddOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6930 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSubtractOp* SgSubtractOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgSubtractOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSubtractOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6948 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMultiplyOp* SgMultiplyOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgMultiplyOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMultiplyOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6966 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDivideOp* SgDivideOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgDivideOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDivideOp* returnNode = NULL;

     return returnNode;
   }


/* #line 6984 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIntegerDivideOp* SgIntegerDivideOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgIntegerDivideOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIntegerDivideOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7002 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgModOp* SgModOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgModOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgModOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7020 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAndOp* SgAndOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgAndOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAndOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7038 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgOrOp* SgOrOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgOrOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgOrOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7056 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBitXorOp* SgBitXorOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgBitXorOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBitXorOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7074 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBitAndOp* SgBitAndOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgBitAndOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBitAndOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7092 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBitOrOp* SgBitOrOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgBitOrOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBitOrOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7110 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCommaOpExp* SgCommaOpExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgCommaOpExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCommaOpExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7128 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLshiftOp* SgLshiftOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgLshiftOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLshiftOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7146 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgRshiftOp* SgRshiftOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgRshiftOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgRshiftOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7164 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPntrArrRefExp* SgPntrArrRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgPntrArrRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPntrArrRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7182 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgScopeOp* SgScopeOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgScopeOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgScopeOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7200 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAssignOp* SgAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7218 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgExponentiationOp* SgExponentiationOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgExponentiationOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgExponentiationOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7236 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaUnsignedRshiftOp* SgJavaUnsignedRshiftOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgJavaUnsignedRshiftOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaUnsignedRshiftOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7254 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgConcatenationOp* SgConcatenationOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgConcatenationOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgConcatenationOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7272 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPointerAssignOp* SgPointerAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgPointerAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPointerAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7290 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUserDefinedBinaryOp* SgUserDefinedBinaryOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type, SgName operator_name, SgFunctionSymbol* symbol )
   {
#if 0
  // debugging information!
     printf ("In SgUserDefinedBinaryOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type, SgName operator_name, SgFunctionSymbol* symbol): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUserDefinedBinaryOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7308 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCompoundAssignOp* SgCompoundAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgCompoundAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCompoundAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7326 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPlusAssignOp* SgPlusAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgPlusAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPlusAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7344 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMinusAssignOp* SgMinusAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgMinusAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMinusAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7362 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAndAssignOp* SgAndAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgAndAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAndAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7380 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIorAssignOp* SgIorAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgIorAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIorAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7398 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMultAssignOp* SgMultAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgMultAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMultAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7416 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDivAssignOp* SgDivAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgDivAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDivAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7434 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgModAssignOp* SgModAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgModAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgModAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7452 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgXorAssignOp* SgXorAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgXorAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgXorAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7470 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLshiftAssignOp* SgLshiftAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgLshiftAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLshiftAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7488 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgRshiftAssignOp* SgRshiftAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgRshiftAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgRshiftAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7506 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaUnsignedRshiftAssignOp* SgJavaUnsignedRshiftAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgJavaUnsignedRshiftAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaUnsignedRshiftAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7524 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIntegerDivideAssignOp* SgIntegerDivideAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgIntegerDivideAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIntegerDivideAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7542 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgExponentiationAssignOp* SgExponentiationAssignOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgExponentiationAssignOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgExponentiationAssignOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7560 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMembershipOp* SgMembershipOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgMembershipOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMembershipOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7578 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNonMembershipOp* SgNonMembershipOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgNonMembershipOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNonMembershipOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7596 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIsOp* SgIsOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgIsOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIsOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7614 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIsNotOp* SgIsNotOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgIsNotOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand_i, SgExpression* rhs_operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIsNotOp* returnNode = NULL;

     return returnNode;
   }


/* #line 7632 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgExprListExp* SgExprListExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgExprListExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgExprListExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7650 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgListExp* SgListExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgListExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgListExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7668 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTupleExp* SgTupleExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgTupleExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTupleExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7686 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVarRefExp* SgVarRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgVariableSymbol* symbol )
   {
#if 0
  // debugging information!
     printf ("In SgVarRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgVariableSymbol* symbol): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVarRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7704 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgClassNameRefExp* SgClassNameRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgClassSymbol* symbol )
   {
#if 0
  // debugging information!
     printf ("In SgClassNameRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgClassSymbol* symbol): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgClassNameRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7722 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionRefExp* SgFunctionRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgFunctionSymbol* symbol_i, SgFunctionType* function_type )
   {
#if 0
  // debugging information!
     printf ("In SgFunctionRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgFunctionSymbol* symbol_i, SgFunctionType* function_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7740 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMemberFunctionRefExp* SgMemberFunctionRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgMemberFunctionSymbol* symbol_i, int virtual_call, SgFunctionType* function_type, int need_qualifier )
   {
#if 0
  // debugging information!
     printf ("In SgMemberFunctionRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgMemberFunctionSymbol* symbol_i, int virtual_call, SgFunctionType* function_type, int need_qualifier): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMemberFunctionRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7758 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgValueExp* SgValueExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgValueExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgValueExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7776 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgBoolValExp* SgBoolValExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int value )
   {
#if 0
  // debugging information!
     printf ("In SgBoolValExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int value): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgBoolValExp* returnNode = NULL;

     return returnNode;
   }


/* #line 7794 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgStringVal* SgStringVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, std::string value )
   {
#if 0
  // debugging information!
     printf ("In SgStringVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, std::string value): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgStringVal* returnNode = NULL;

     return returnNode;
   }


/* #line 7812 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgShortVal* SgShortVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, short value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgShortVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, short value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgShortVal* returnNode = NULL;

     return returnNode;
   }


/* #line 7830 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCharVal* SgCharVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, char value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgCharVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, char value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCharVal* returnNode = NULL;

     return returnNode;
   }

// DQ (3/23/2018): Added to support new char16 and char32 types.
SgChar16Val* SgChar16Val::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, unsigned short value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgChar16Val::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, unsigned short value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgChar16Val* returnNode = NULL;

     return returnNode;
   }

// DQ (3/23/2018): Added to support new char16 and char32 types.
SgChar32Val* SgChar32Val::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, unsigned int value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgChar32Val::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, unsigned int value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgChar32Val* returnNode = NULL;

     return returnNode;
   }


/* #line 7848 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUnsignedCharVal* SgUnsignedCharVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, unsigned char value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgUnsignedCharVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, unsigned char value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnsignedCharVal* returnNode = NULL;

     return returnNode;
   }


/* #line 7866 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgWcharVal* SgWcharVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, unsigned long valueUL, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgWcharVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, unsigned long valueUL, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgWcharVal* returnNode = NULL;

     return returnNode;
   }


/* #line 7884 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUnsignedShortVal* SgUnsignedShortVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, unsigned short value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgUnsignedShortVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, unsigned short value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnsignedShortVal* returnNode = NULL;

     return returnNode;
   }


/* #line 7902 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIntVal* SgIntVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgIntVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIntVal* returnNode = NULL;

     return returnNode;
   }


/* #line 7920 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEnumVal* SgEnumVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int value, SgEnumDeclaration* declaration, SgName name )
   {
#if 0
  // debugging information!
     printf ("In SgEnumVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int value, SgEnumDeclaration* declaration, SgName name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEnumVal* returnNode = NULL;

     return returnNode;
   }


/* #line 7938 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUnsignedIntVal* SgUnsignedIntVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, unsigned int value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgUnsignedIntVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, unsigned int value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnsignedIntVal* returnNode = NULL;

     return returnNode;
   }


/* #line 7956 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLongIntVal* SgLongIntVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, long int value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgLongIntVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, long int value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLongIntVal* returnNode = NULL;

     return returnNode;
   }


/* #line 7974 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLongLongIntVal* SgLongLongIntVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, long long int value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgLongLongIntVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, long long int value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLongLongIntVal* returnNode = NULL;

     return returnNode;
   }


/* #line 7992 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUnsignedLongLongIntVal* SgUnsignedLongLongIntVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, unsigned long long int value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgUnsignedLongLongIntVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, unsigned long long int value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnsignedLongLongIntVal* returnNode = NULL;

     return returnNode;
   }


/* #line 8010 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUnsignedLongVal* SgUnsignedLongVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, unsigned long value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgUnsignedLongVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, unsigned long value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnsignedLongVal* returnNode = NULL;

     return returnNode;
   }


/* #line 8028 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFloatVal* SgFloatVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, float value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgFloatVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, float value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFloatVal* returnNode = NULL;

     return returnNode;
   }


/* #line 8046 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDoubleVal* SgDoubleVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, double value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgDoubleVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, double value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDoubleVal* returnNode = NULL;

     return returnNode;
   }


/* #line 8064 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLongDoubleVal* SgLongDoubleVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, long double value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgLongDoubleVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, long double value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLongDoubleVal* returnNode = NULL;

     return returnNode;
   }


/* #line 8082 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgComplexVal* SgComplexVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgValueExp* real_value, SgValueExp* imaginary_value, SgType* precisionType, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgComplexVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgValueExp* real_value, SgValueExp* imaginary_value, SgType* precisionType, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgComplexVal* returnNode = NULL;

     return returnNode;
   }


/* #line 8100 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUpcThreads* SgUpcThreads::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgUpcThreads::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUpcThreads* returnNode = NULL;

     return returnNode;
   }


/* #line 8118 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUpcMythread* SgUpcMythread::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int value, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgUpcMythread::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int value, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUpcMythread* returnNode = NULL;

     return returnNode;
   }


/* #line 8136 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateParameterVal* SgTemplateParameterVal::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int template_parameter_position, std::string valueString )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateParameterVal::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int template_parameter_position, std::string valueString): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateParameterVal* returnNode = NULL;

     return returnNode;
   }


/* #line 8154 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNullptrValExp* SgNullptrValExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgNullptrValExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNullptrValExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8172 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCallExpression* SgCallExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* function, SgExprListExp* args, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgCallExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* function, SgExprListExp* args, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCallExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8190 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionCallExp* SgFunctionCallExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* function, SgExprListExp* args, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgFunctionCallExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* function, SgExprListExp* args, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionCallExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8208 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCudaKernelCallExp* SgCudaKernelCallExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* function, SgExprListExp* args, SgType* expression_type, SgCudaKernelExecConfig* exec_config )
   {
#if 0
  // debugging information!
     printf ("In SgCudaKernelCallExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* function, SgExprListExp* args, SgType* expression_type, SgCudaKernelExecConfig* exec_config): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCudaKernelCallExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8226 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSizeOfOp* SgSizeOfOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* operand_type, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgSizeOfOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* operand_type, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSizeOfOp* returnNode = NULL;

     return returnNode;
   }


/* #line 8244 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUpcLocalsizeofExpression* SgUpcLocalsizeofExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression, SgType* operand_type, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgUpcLocalsizeofExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression, SgType* operand_type, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUpcLocalsizeofExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8262 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUpcBlocksizeofExpression* SgUpcBlocksizeofExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression, SgType* operand_type, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgUpcBlocksizeofExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression, SgType* operand_type, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUpcBlocksizeofExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8280 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUpcElemsizeofExpression* SgUpcElemsizeofExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression, SgType* operand_type, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgUpcElemsizeofExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression, SgType* operand_type, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUpcElemsizeofExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8298 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaInstanceOfOp* SgJavaInstanceOfOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* operand_type, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgJavaInstanceOfOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* operand_type, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaInstanceOfOp* returnNode = NULL;

     return returnNode;
   }


/* #line 8316 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSuperExp* SgSuperExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgClassSymbol* class_symbol, int pobj_super )
   {
#if 0
  // debugging information!
     printf ("In SgSuperExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgClassSymbol* class_symbol, int pobj_super): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSuperExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8334 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeIdOp* SgTypeIdOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* operand_type )
   {
#if 0
  // debugging information!
     printf ("In SgTypeIdOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* operand_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeIdOp* returnNode = NULL;

     return returnNode;
   }


/* #line 8352 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgConditionalExp* SgConditionalExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* conditional_exp, SgExpression* true_exp, SgExpression* false_exp, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgConditionalExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* conditional_exp, SgExpression* true_exp, SgExpression* false_exp, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgConditionalExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8370 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNewExp* SgNewExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgType* specified_type, SgExprListExp* placement_args, SgConstructorInitializer* constructor_args, SgExpression* builtin_args, short need_global_specifier, SgFunctionDeclaration* newOperatorDeclaration )
   {
#if 0
  // debugging information!
     printf ("In SgNewExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgType* specified_type, SgExprListExp* placement_args, SgConstructorInitializer* constructor_args, SgExpression* builtin_args, short need_global_specifier, SgFunctionDeclaration* newOperatorDeclaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNewExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8388 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDeleteExp* SgDeleteExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* variable, short is_array, short need_global_specifier, SgFunctionDeclaration* deleteOperatorDeclaration )
   {
#if 0
  // debugging information!
     printf ("In SgDeleteExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* variable, short is_array, short need_global_specifier, SgFunctionDeclaration* deleteOperatorDeclaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDeleteExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8406 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgThisExp* SgThisExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgClassSymbol* class_symbol, SgNonrealSymbol* nonreal_symbol, int pobj_this )
   {
#if 0
  // debugging information!
     printf ("In SgThisExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgClassSymbol* class_symbol, int pobj_this): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgThisExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8424 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgRefExp* SgRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgType* type_name )
   {
#if 0
  // debugging information!
     printf ("In SgRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgType* type_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8442 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgInitializer* SgInitializer::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgInitializer::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgInitializer* returnNode = NULL;

     return returnNode;
   }


/* #line 8460 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAggregateInitializer* SgAggregateInitializer::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExprListExp* initializers, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgAggregateInitializer::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExprListExp* initializers, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAggregateInitializer* returnNode = NULL;

     return returnNode;
   }


/* #line 8478 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCompoundInitializer* SgCompoundInitializer::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExprListExp* initializers, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgCompoundInitializer::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExprListExp* initializers, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCompoundInitializer* returnNode = NULL;

     return returnNode;
   }


/* #line 8496 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgConstructorInitializer* SgConstructorInitializer::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgMemberFunctionDeclaration* declaration, SgExprListExp* args, SgType* expression_type, bool need_name, bool need_qualifier, bool need_parenthesis_after_name, bool associated_class_unknown )
   {
#if 0
  // debugging information!
     printf ("In SgConstructorInitializer::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgMemberFunctionDeclaration* declaration, SgExprListExp* args, SgType* expression_type, bool need_name, bool need_qualifier, bool need_parenthesis_after_name, bool associated_class_unknown): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgConstructorInitializer* returnNode = NULL;

     return returnNode;
   }


/* #line 8514 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAssignInitializer* SgAssignInitializer::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgAssignInitializer::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_i, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAssignInitializer* returnNode = NULL;

     ROSE_ASSERT(startOfConstruct != NULL);
     ROSE_ASSERT(operand_i != NULL);

  // Note that this is an older data member that is no longer used in all but a few SgExpression IR nodes.
     ROSE_ASSERT(expression_type == NULL);

     returnNode = SageBuilder::buildAssignInitializer_nfi(operand_i);
     ROSE_ASSERT(returnNode != NULL);

     returnNode->set_startOfConstruct(startOfConstruct);

     return returnNode;
   }


/* #line 8532 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDesignatedInitializer* SgDesignatedInitializer::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExprListExp* designatorList, SgInitializer* memberInit )
   {
#if 0
  // debugging information!
     printf ("In SgDesignatedInitializer::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExprListExp* designatorList, SgInitializer* memberInit): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDesignatedInitializer* returnNode = NULL;

     return returnNode;
   }


/* #line 8550 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVarArgStartOp* SgVarArgStartOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand, SgExpression* rhs_operand, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgVarArgStartOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand, SgExpression* rhs_operand, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVarArgStartOp* returnNode = NULL;

     return returnNode;
   }


/* #line 8568 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVarArgOp* SgVarArgOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgVarArgOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVarArgOp* returnNode = NULL;

     return returnNode;
   }


/* #line 8586 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVarArgEndOp* SgVarArgEndOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgVarArgEndOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVarArgEndOp* returnNode = NULL;

     return returnNode;
   }


/* #line 8604 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVarArgCopyOp* SgVarArgCopyOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lhs_operand, SgExpression* rhs_operand, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgVarArgCopyOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lhs_operand, SgExpression* rhs_operand, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVarArgCopyOp* returnNode = NULL;

     return returnNode;
   }


/* #line 8622 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVarArgStartOneOperandOp* SgVarArgStartOneOperandOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgVarArgStartOneOperandOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVarArgStartOneOperandOp* returnNode = NULL;

     return returnNode;
   }


/* #line 8640 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNullExpression* SgNullExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgNullExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNullExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8658 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVariantExpression* SgVariantExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgVariantExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVariantExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8676 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSubscriptExpression* SgSubscriptExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* lowerBound, SgExpression* upperBound, SgExpression* stride )
   {
#if 0
  // debugging information!
     printf ("In SgSubscriptExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* lowerBound, SgExpression* upperBound, SgExpression* stride): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSubscriptExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8694 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgColonShapeExp* SgColonShapeExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgColonShapeExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgColonShapeExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8712 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsteriskShapeExp* SgAsteriskShapeExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgAsteriskShapeExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsteriskShapeExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8730 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgImpliedDo* SgImpliedDo::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* do_var_initialization, SgExpression* last_val, SgExpression* increment, SgExprListExp* object_list, SgScopeStatement* implied_do_scope )
   {
#if 0
  // debugging information!
     printf ("In SgImpliedDo::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* do_var_initialization, SgExpression* last_val, SgExpression* increment, SgExprListExp* object_list, SgScopeStatement* implied_do_scope): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgImpliedDo* returnNode = NULL;

     return returnNode;
   }


/* #line 8748 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIOItemExpression* SgIOItemExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name, SgExpression* io_item )
   {
#if 0
  // debugging information!
     printf ("In SgIOItemExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name, SgExpression* io_item): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIOItemExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8766 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgStatementExpression* SgStatementExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgStatement* statement )
   {
#if 0
  // debugging information!
     printf ("In SgStatementExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgStatement* statement): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgStatementExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8784 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmOp* SgAsmOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgAsmOp::asm_operand_constraint_enum constraint, SgAsmOp::asm_operand_modifier_enum modifiers, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgAsmOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgAsmOp::asm_operand_constraint_enum constraint, SgAsmOp::asm_operand_modifier_enum modifiers, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmOp* returnNode = NULL;

     return returnNode;
   }


/* #line 8802 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLabelRefExp* SgLabelRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgLabelSymbol* symbol )
   {
#if 0
  // debugging information!
     printf ("In SgLabelRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgLabelSymbol* symbol): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLabelRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8820 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgActualArgumentExpression* SgActualArgumentExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName argument_name, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgActualArgumentExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName argument_name, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgActualArgumentExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8838 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgUnknownArrayOrFunctionReference* SgUnknownArrayOrFunctionReference::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgUnknownArrayOrFunctionReference::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgUnknownArrayOrFunctionReference* returnNode = NULL;

     return returnNode;
   }


/* #line 8856 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgPseudoDestructorRefExp* SgPseudoDestructorRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgType* object_type )
   {
#if 0
  // debugging information!
     printf ("In SgPseudoDestructorRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgType* object_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgPseudoDestructorRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8874 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCAFCoExpression* SgCAFCoExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgVarRefExp* teamId, SgExpression* teamRank, SgExpression* referData )
   {
#if 0
  // debugging information!
     printf ("In SgCAFCoExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgVarRefExp* teamId, SgExpression* teamRank, SgExpression* referData): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCAFCoExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 8892 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCudaKernelExecConfig* SgCudaKernelExecConfig::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* grid, SgExpression* blocks, SgExpression* shared, SgExpression* stream )
   {
#if 0
  // debugging information!
     printf ("In SgCudaKernelExecConfig::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* grid, SgExpression* blocks, SgExpression* shared, SgExpression* stream): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCudaKernelExecConfig* returnNode = NULL;

     return returnNode;
   }


/* #line 8910 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLambdaRefExp* SgLambdaRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgFunctionDeclaration* functionDeclaration )
   {
#if 0
  // debugging information!
     printf ("In SgLambdaRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgFunctionDeclaration* functionDeclaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLambdaRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8928 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDictionaryExp* SgDictionaryExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgDictionaryExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDictionaryExp* returnNode = NULL;

     return returnNode;
   }


/* #line 8946 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgKeyDatumPair* SgKeyDatumPair::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* key, SgExpression* datum )
   {
#if 0
  // debugging information!
     printf ("In SgKeyDatumPair::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* key, SgExpression* datum): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgKeyDatumPair* returnNode = NULL;

     return returnNode;
   }


/* #line 8964 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgComprehension* SgComprehension::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* target, SgExpression* iter, SgExprListExp* filters )
   {
#if 0
  // debugging information!
     printf ("In SgComprehension::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* target, SgExpression* iter, SgExprListExp* filters): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgComprehension* returnNode = NULL;

     return returnNode;
   }


/* #line 8982 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgListComprehension* SgListComprehension::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* element, SgExprListExp* generators )
   {
#if 0
  // debugging information!
     printf ("In SgListComprehension::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* element, SgExprListExp* generators): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgListComprehension* returnNode = NULL;

     return returnNode;
   }


/* #line 9000 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSetComprehension* SgSetComprehension::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* element, SgExprListExp* generators )
   {
#if 0
  // debugging information!
     printf ("In SgSetComprehension::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* element, SgExprListExp* generators): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSetComprehension* returnNode = NULL;

     return returnNode;
   }


/* #line 9018 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDictionaryComprehension* SgDictionaryComprehension::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgKeyDatumPair* element, SgExprListExp* generators )
   {
#if 0
  // debugging information!
     printf ("In SgDictionaryComprehension::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgKeyDatumPair* element, SgExprListExp* generators): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDictionaryComprehension* returnNode = NULL;

     return returnNode;
   }


/* #line 9036 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNaryOp* SgNaryOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgNaryOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNaryOp* returnNode = NULL;

     return returnNode;
   }


/* #line 9054 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNaryBooleanOp* SgNaryBooleanOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgNaryBooleanOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNaryBooleanOp* returnNode = NULL;

     return returnNode;
   }


/* #line 9072 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNaryComparisonOp* SgNaryComparisonOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct )
   {
#if 0
  // debugging information!
     printf ("In SgNaryComparisonOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNaryComparisonOp* returnNode = NULL;

     return returnNode;
   }


/* #line 9090 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgStringConversion* SgStringConversion::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* expression )
   {
#if 0
  // debugging information!
     printf ("In SgStringConversion::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* expression): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgStringConversion* returnNode = NULL;

     return returnNode;
   }


/* #line 9108 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgYieldExpression* SgYieldExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* value )
   {
#if 0
  // debugging information!
     printf ("In SgYieldExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* value): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgYieldExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 9126 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateFunctionRefExp* SgTemplateFunctionRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgTemplateFunctionSymbol* symbol_i )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateFunctionRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgTemplateFunctionSymbol* symbol_i): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateFunctionRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 9144 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateMemberFunctionRefExp* SgTemplateMemberFunctionRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgTemplateMemberFunctionSymbol* symbol_i, int virtual_call, int need_qualifier )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateMemberFunctionRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgTemplateMemberFunctionSymbol* symbol_i, int virtual_call, int need_qualifier): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateMemberFunctionRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 9162 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAlignOfOp* SgAlignOfOp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* operand_type, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgAlignOfOp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgExpression* operand_expr, SgType* operand_type, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAlignOfOp* returnNode = NULL;

     return returnNode;
   }


/* #line 9180 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeTraitBuiltinOperator* SgTypeTraitBuiltinOperator::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgName name )
   {
#if 0
  // debugging information!
     printf ("In SgTypeTraitBuiltinOperator::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgName name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeTraitBuiltinOperator* returnNode = NULL;

     return returnNode;
   }


/* #line 9198 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCompoundLiteralExp* SgCompoundLiteralExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgVariableSymbol* symbol )
   {
#if 0
  // debugging information!
     printf ("In SgCompoundLiteralExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgVariableSymbol* symbol): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCompoundLiteralExp* returnNode = NULL;

     return returnNode;
   }


/* #line 9216 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaAnnotation* SgJavaAnnotation::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgJavaAnnotation::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaAnnotation* returnNode = NULL;

     return returnNode;
   }


/* #line 9234 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaMarkerAnnotation* SgJavaMarkerAnnotation::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgJavaMarkerAnnotation::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaMarkerAnnotation* returnNode = NULL;

     return returnNode;
   }


/* #line 9252 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaSingleMemberAnnotation* SgJavaSingleMemberAnnotation::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgType* expression_type, SgExpression* value )
   {
#if 0
  // debugging information!
     printf ("In SgJavaSingleMemberAnnotation::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgType* expression_type, SgExpression* value): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaSingleMemberAnnotation* returnNode = NULL;

     return returnNode;
   }


/* #line 9270 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaNormalAnnotation* SgJavaNormalAnnotation::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgType* expression_type )
   {
#if 0
  // debugging information!
     printf ("In SgJavaNormalAnnotation::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgType* expression_type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaNormalAnnotation* returnNode = NULL;

     return returnNode;
   }


/* #line 9288 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaTypeExpression* SgJavaTypeExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgType* type )
   {
#if 0
  // debugging information!
     printf ("In SgJavaTypeExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgType* type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaTypeExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 9306 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypeExpression* SgTypeExpression::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgType* type )
   {
#if 0
  // debugging information!
     printf ("In SgTypeExpression::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgType* type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypeExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 9324 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgClassExp* SgClassExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, SgClassSymbol* class_symbol, int pobj_class )
   {
#if 0
  // debugging information!
     printf ("In SgClassExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, SgClassSymbol* class_symbol, int pobj_class): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgClassExp* returnNode = NULL;

     return returnNode;
   }


/* #line 9342 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionParameterRefExp* SgFunctionParameterRefExp::build_node_from_nonlist_children ( Sg_File_Info* startOfConstruct, int parameter_number, int parameter_levels_up )
   {
#if 0
  // debugging information!
     printf ("In SgFunctionParameterRefExp::build_node_from_nonlist_children (Sg_File_Info* startOfConstruct, int parameter_number, int parameter_levels_up): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionParameterRefExp* returnNode = NULL;

     return returnNode;
   }


/* #line 9360 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgSymbol* SgSymbol::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgSymbol::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9378 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgVariableSymbol* SgVariableSymbol::build_node_from_nonlist_children ( SgInitializedName* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgVariableSymbol::build_node_from_nonlist_children (SgInitializedName* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgVariableSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9396 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateVariableSymbol* SgTemplateVariableSymbol::build_node_from_nonlist_children ( SgInitializedName* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateVariableSymbol::build_node_from_nonlist_children (SgInitializedName* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateVariableSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9414 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionSymbol* SgFunctionSymbol::build_node_from_nonlist_children ( SgFunctionDeclaration* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgFunctionSymbol::build_node_from_nonlist_children (SgFunctionDeclaration* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9432 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgMemberFunctionSymbol* SgMemberFunctionSymbol::build_node_from_nonlist_children ( SgFunctionDeclaration* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgMemberFunctionSymbol::build_node_from_nonlist_children (SgFunctionDeclaration* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgMemberFunctionSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9450 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateMemberFunctionSymbol* SgTemplateMemberFunctionSymbol::build_node_from_nonlist_children ( SgFunctionDeclaration* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateMemberFunctionSymbol::build_node_from_nonlist_children (SgFunctionDeclaration* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateMemberFunctionSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9468 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateFunctionSymbol* SgTemplateFunctionSymbol::build_node_from_nonlist_children ( SgFunctionDeclaration* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateFunctionSymbol::build_node_from_nonlist_children (SgFunctionDeclaration* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateFunctionSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9486 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgRenameSymbol* SgRenameSymbol::build_node_from_nonlist_children ( SgFunctionDeclaration* declaration, SgSymbol* original_symbol, SgName new_name )
   {
#if 0
  // debugging information!
     printf ("In SgRenameSymbol::build_node_from_nonlist_children (SgFunctionDeclaration* declaration, SgSymbol* original_symbol, SgName new_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgRenameSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9504 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgFunctionTypeSymbol* SgFunctionTypeSymbol::build_node_from_nonlist_children ( SgName name, SgType* type )
   {
#if 0
  // debugging information!
     printf ("In SgFunctionTypeSymbol::build_node_from_nonlist_children (SgName name, SgType* type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFunctionTypeSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9522 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgClassSymbol* SgClassSymbol::build_node_from_nonlist_children ( SgClassDeclaration* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgClassSymbol::build_node_from_nonlist_children (SgClassDeclaration* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgClassSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9540 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateClassSymbol* SgTemplateClassSymbol::build_node_from_nonlist_children ( SgClassDeclaration* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateClassSymbol::build_node_from_nonlist_children (SgClassDeclaration* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateClassSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9558 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTemplateSymbol* SgTemplateSymbol::build_node_from_nonlist_children ( SgTemplateDeclaration* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgTemplateSymbol::build_node_from_nonlist_children (SgTemplateDeclaration* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9576 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEnumSymbol* SgEnumSymbol::build_node_from_nonlist_children ( SgEnumDeclaration* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgEnumSymbol::build_node_from_nonlist_children (SgEnumDeclaration* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEnumSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9594 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgEnumFieldSymbol* SgEnumFieldSymbol::build_node_from_nonlist_children ( SgInitializedName* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgEnumFieldSymbol::build_node_from_nonlist_children (SgInitializedName* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgEnumFieldSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9612 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgTypedefSymbol* SgTypedefSymbol::build_node_from_nonlist_children ( SgTypedefDeclaration* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgTypedefSymbol::build_node_from_nonlist_children (SgTypedefDeclaration* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTypedefSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9630 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgLabelSymbol* SgLabelSymbol::build_node_from_nonlist_children ( SgLabelStatement* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgLabelSymbol::build_node_from_nonlist_children (SgLabelStatement* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLabelSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9648 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgDefaultSymbol* SgDefaultSymbol::build_node_from_nonlist_children ( SgType* type )
   {
#if 0
  // debugging information!
     printf ("In SgDefaultSymbol::build_node_from_nonlist_children (SgType* type): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgDefaultSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9666 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgNamespaceSymbol* SgNamespaceSymbol::build_node_from_nonlist_children ( SgName name, SgNamespaceDeclarationStatement* declaration, SgNamespaceAliasDeclarationStatement* aliasDeclaration, bool isAlias )
   {
#if 0
  // debugging information!
     printf ("In SgNamespaceSymbol::build_node_from_nonlist_children (SgName name, SgNamespaceDeclarationStatement* declaration, SgNamespaceAliasDeclarationStatement* aliasDeclaration, bool isAlias): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgNamespaceSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9684 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgIntrinsicSymbol* SgIntrinsicSymbol::build_node_from_nonlist_children ( SgInitializedName* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgIntrinsicSymbol::build_node_from_nonlist_children (SgInitializedName* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgIntrinsicSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9702 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgModuleSymbol* SgModuleSymbol::build_node_from_nonlist_children ( SgModuleStatement* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgModuleSymbol::build_node_from_nonlist_children (SgModuleStatement* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgModuleSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9720 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgInterfaceSymbol* SgInterfaceSymbol::build_node_from_nonlist_children ( SgInterfaceStatement* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgInterfaceSymbol::build_node_from_nonlist_children (SgInterfaceStatement* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgInterfaceSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9738 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgCommonSymbol* SgCommonSymbol::build_node_from_nonlist_children ( SgInitializedName* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgCommonSymbol::build_node_from_nonlist_children (SgInitializedName* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgCommonSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9756 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAliasSymbol* SgAliasSymbol::build_node_from_nonlist_children ( SgSymbol* alias, bool isRenamed, SgName new_name )
   {
#if 0
  // debugging information!
     printf ("In SgAliasSymbol::build_node_from_nonlist_children (SgSymbol* alias, bool isRenamed, SgName new_name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAliasSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9774 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryAddressSymbol* SgAsmBinaryAddressSymbol::build_node_from_nonlist_children ( SgName address_name, SgAsmInstruction* address )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryAddressSymbol::build_node_from_nonlist_children (SgName address_name, SgAsmInstruction* address): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryAddressSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9792 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryDataSymbol* SgAsmBinaryDataSymbol::build_node_from_nonlist_children ( SgName variable_name, SgAsmInstruction* address )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryDataSymbol::build_node_from_nonlist_children (SgName variable_name, SgAsmInstruction* address): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryDataSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9810 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgJavaLabelSymbol* SgJavaLabelSymbol::build_node_from_nonlist_children ( SgJavaLabelStatement* declaration )
   {
#if 0
  // debugging information!
     printf ("In SgJavaLabelSymbol::build_node_from_nonlist_children (SgJavaLabelStatement* declaration): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgJavaLabelSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 9828 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNode* SgAsmNode::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNode::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNode* returnNode = NULL;

     return returnNode;
   }


/* #line 9846 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmStatement* SgAsmStatement::build_node_from_nonlist_children ( rose_addr_t address )
   {
#if 0
  // debugging information!
     printf ("In SgAsmStatement::build_node_from_nonlist_children (rose_addr_t address): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmStatement* returnNode = NULL;

     return returnNode;
   }


/* #line 9864 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmSynthesizedDeclaration* SgAsmSynthesizedDeclaration::build_node_from_nonlist_children ( rose_addr_t address )
   {
#if 0
  // debugging information!
     printf ("In SgAsmSynthesizedDeclaration::build_node_from_nonlist_children (rose_addr_t address): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmSynthesizedDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 9882 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmSynthesizedDataStructureDeclaration* SgAsmSynthesizedDataStructureDeclaration::build_node_from_nonlist_children ( rose_addr_t address )
   {
#if 0
  // debugging information!
     printf ("In SgAsmSynthesizedDataStructureDeclaration::build_node_from_nonlist_children (rose_addr_t address): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmSynthesizedDataStructureDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 9900 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmFunction* SgAsmFunction::build_node_from_nonlist_children ( rose_addr_t address, std::string name, unsigned reason, SgAsmFunction::function_kind_enum function_kind )
   {
#if 0
  // debugging information!
     printf ("In SgAsmFunction::build_node_from_nonlist_children (rose_addr_t address, std::string name, unsigned reason, SgAsmFunction::function_kind_enum function_kind): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmFunction* returnNode = NULL;

     return returnNode;
   }


/* #line 9918 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmSynthesizedFieldDeclaration* SgAsmSynthesizedFieldDeclaration::build_node_from_nonlist_children ( rose_addr_t address )
   {
#if 0
  // debugging information!
     printf ("In SgAsmSynthesizedFieldDeclaration::build_node_from_nonlist_children (rose_addr_t address): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmSynthesizedFieldDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 9936 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBlock* SgAsmBlock::build_node_from_nonlist_children ( rose_addr_t address )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBlock::build_node_from_nonlist_children (rose_addr_t address): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBlock* returnNode = NULL;

     return returnNode;
   }


/* #line 9954 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmInstruction* SgAsmInstruction::build_node_from_nonlist_children ( rose_addr_t address, std::string mnemonic )
   {
#if 0
  // debugging information!
     printf ("In SgAsmInstruction::build_node_from_nonlist_children (rose_addr_t address, std::string mnemonic): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmInstruction* returnNode = NULL;

     return returnNode;
   }


/* #line 9972 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmX86Instruction* SgAsmX86Instruction::build_node_from_nonlist_children(rose_addr_t address, std::string mnemonic,
                                                                           Rose::BinaryAnalysis::X86InstructionKind kind,
                                                                           Rose::BinaryAnalysis::X86InstructionSize baseSize,
                                                                           Rose::BinaryAnalysis::X86InstructionSize operandSize,
                                                                           Rose::BinaryAnalysis::X86InstructionSize addressSize)
   {
#if 0
  // debugging information!
     printf ("In SgAsmX86Instruction::build_node_from_nonlist_children (rose_addr_t address, std::string mnemonic, X86InstructionKind kind, X86InstructionSize baseSize, X86InstructionSize operandSize, X86InstructionSize addressSize): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmX86Instruction* returnNode = NULL;

     return returnNode;
   }


/* #line 9990 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmArmInstruction* SgAsmArmInstruction::build_node_from_nonlist_children(rose_addr_t address, std::string mnemonic,
                                                                           Rose::BinaryAnalysis::ArmInstructionKind kind,
                                                                           Rose::BinaryAnalysis::ArmInstructionCondition condition,
                                                                           int positionOfConditionInMnemonic)
   {
#if 0
  // debugging information!
     printf ("In SgAsmArmInstruction::build_node_from_nonlist_children (rose_addr_t address, std::string mnemonic, ArmInstructionKind kind, ArmInstructionCondition condition, int positionOfConditionInMnemonic): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmArmInstruction* returnNode = NULL;

     return returnNode;
   }


/* #line 10008 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPowerpcInstruction* SgAsmPowerpcInstruction::build_node_from_nonlist_children(rose_addr_t address, std::string mnemonic,
                                                                                   Rose::BinaryAnalysis::PowerpcInstructionKind kind)
   {
#if 0
  // debugging information!
     printf ("In SgAsmPowerpcInstruction::build_node_from_nonlist_children (rose_addr_t address, std::string mnemonic, PowerpcInstructionKind kind): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPowerpcInstruction* returnNode = NULL;

     return returnNode;
   }


/* #line 10026 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmMipsInstruction* SgAsmMipsInstruction::build_node_from_nonlist_children(rose_addr_t address, std::string mnemonic,
                                                                             Rose::BinaryAnalysis::MipsInstructionKind kind)
   {
#if 0
  // debugging information!
     printf ("In SgAsmMipsInstruction::build_node_from_nonlist_children (rose_addr_t address, std::string mnemonic, MipsInstructionKind kind): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmMipsInstruction* returnNode = NULL;

     return returnNode;
   }


/* #line 10044 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmM68kInstruction* SgAsmM68kInstruction::build_node_from_nonlist_children(rose_addr_t address, std::string mnemonic,
                                                                             Rose::BinaryAnalysis::M68kInstructionKind kind)
   {
#if 0
  // debugging information!
     printf ("In SgAsmM68kInstruction::build_node_from_nonlist_children (rose_addr_t address, std::string mnemonic, M68kInstructionKind kind): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmM68kInstruction* returnNode = NULL;

     return returnNode;
   }


/* #line 10062 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmStaticData* SgAsmStaticData::build_node_from_nonlist_children ( rose_addr_t address )
   {
#if 0
  // debugging information!
     printf ("In SgAsmStaticData::build_node_from_nonlist_children (rose_addr_t address): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmStaticData* returnNode = NULL;

     return returnNode;
   }


/* #line 10080 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmExpression* SgAsmExpression::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmExpression::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10098 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmValueExpression* SgAsmValueExpression::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmValueExpression::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmValueExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10116 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmConstantExpression* SgAsmConstantExpression::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmConstantExpression::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmConstantExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10134 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmIntegerValueExpression* SgAsmIntegerValueExpression::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmIntegerValueExpression::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmIntegerValueExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10152 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmFloatValueExpression* SgAsmFloatValueExpression::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmFloatValueExpression::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmFloatValueExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10170 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryExpression* SgAsmBinaryExpression::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryExpression::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10188 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryAdd* SgAsmBinaryAdd::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryAdd::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryAdd* returnNode = NULL;

     return returnNode;
   }


/* #line 10206 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinarySubtract* SgAsmBinarySubtract::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinarySubtract::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinarySubtract* returnNode = NULL;

     return returnNode;
   }


/* #line 10224 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryMultiply* SgAsmBinaryMultiply::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryMultiply::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryMultiply* returnNode = NULL;

     return returnNode;
   }


/* #line 10242 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryDivide* SgAsmBinaryDivide::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryDivide::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryDivide* returnNode = NULL;

     return returnNode;
   }


/* #line 10260 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryMod* SgAsmBinaryMod::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryMod::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryMod* returnNode = NULL;

     return returnNode;
   }


/* #line 10278 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryAddPreupdate* SgAsmBinaryAddPreupdate::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryAddPreupdate::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryAddPreupdate* returnNode = NULL;

     return returnNode;
   }


/* #line 10296 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinarySubtractPreupdate* SgAsmBinarySubtractPreupdate::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinarySubtractPreupdate::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinarySubtractPreupdate* returnNode = NULL;

     return returnNode;
   }


/* #line 10314 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryAddPostupdate* SgAsmBinaryAddPostupdate::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryAddPostupdate::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryAddPostupdate* returnNode = NULL;

     return returnNode;
   }


/* #line 10332 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinarySubtractPostupdate* SgAsmBinarySubtractPostupdate::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinarySubtractPostupdate::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinarySubtractPostupdate* returnNode = NULL;

     return returnNode;
   }


/* #line 10350 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryLsl* SgAsmBinaryLsl::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryLsl::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryLsl* returnNode = NULL;

     return returnNode;
   }


/* #line 10368 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryLsr* SgAsmBinaryLsr::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryLsr::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryLsr* returnNode = NULL;

     return returnNode;
   }


/* #line 10386 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryAsr* SgAsmBinaryAsr::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryAsr::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryAsr* returnNode = NULL;

     return returnNode;
   }


/* #line 10404 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBinaryRor* SgAsmBinaryRor::build_node_from_nonlist_children ( SgAsmExpression* lhs, SgAsmExpression* rhs )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBinaryRor::build_node_from_nonlist_children (SgAsmExpression* lhs, SgAsmExpression* rhs): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBinaryRor* returnNode = NULL;

     return returnNode;
   }


/* #line 10422 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmUnaryExpression* SgAsmUnaryExpression::build_node_from_nonlist_children ( SgAsmExpression* operand )
   {
#if 0
  // debugging information!
     printf ("In SgAsmUnaryExpression::build_node_from_nonlist_children (SgAsmExpression* operand): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmUnaryExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10440 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmUnaryPlus* SgAsmUnaryPlus::build_node_from_nonlist_children ( SgAsmExpression* operand )
   {
#if 0
  // debugging information!
     printf ("In SgAsmUnaryPlus::build_node_from_nonlist_children (SgAsmExpression* operand): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmUnaryPlus* returnNode = NULL;

     return returnNode;
   }


/* #line 10458 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmUnaryMinus* SgAsmUnaryMinus::build_node_from_nonlist_children ( SgAsmExpression* operand )
   {
#if 0
  // debugging information!
     printf ("In SgAsmUnaryMinus::build_node_from_nonlist_children (SgAsmExpression* operand): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmUnaryMinus* returnNode = NULL;

     return returnNode;
   }


/* #line 10476 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmUnaryRrx* SgAsmUnaryRrx::build_node_from_nonlist_children ( SgAsmExpression* operand )
   {
#if 0
  // debugging information!
     printf ("In SgAsmUnaryRrx::build_node_from_nonlist_children (SgAsmExpression* operand): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmUnaryRrx* returnNode = NULL;

     return returnNode;
   }


/* #line 10494 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmUnaryArmSpecialRegisterList* SgAsmUnaryArmSpecialRegisterList::build_node_from_nonlist_children ( SgAsmExpression* operand )
   {
#if 0
  // debugging information!
     printf ("In SgAsmUnaryArmSpecialRegisterList::build_node_from_nonlist_children (SgAsmExpression* operand): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmUnaryArmSpecialRegisterList* returnNode = NULL;

     return returnNode;
   }


/* #line 10512 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmMemoryReferenceExpression* SgAsmMemoryReferenceExpression::build_node_from_nonlist_children ( SgAsmExpression* address, SgAsmExpression* segment )
   {
#if 0
  // debugging information!
     printf ("In SgAsmMemoryReferenceExpression::build_node_from_nonlist_children (SgAsmExpression* address, SgAsmExpression* segment): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmMemoryReferenceExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10530 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmRegisterReferenceExpression* SgAsmRegisterReferenceExpression::build_node_from_nonlist_children ( Rose::BinaryAnalysis::RegisterDescriptor descriptor )
   {
#if 0
  // debugging information!
     printf ("In SgAsmRegisterReferenceExpression::build_node_from_nonlist_children (RegisterDescriptor descriptor): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmRegisterReferenceExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10548 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDirectRegisterExpression* SgAsmDirectRegisterExpression::build_node_from_nonlist_children ( Rose::BinaryAnalysis::RegisterDescriptor descriptor )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDirectRegisterExpression::build_node_from_nonlist_children (RegisterDescriptor descriptor): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDirectRegisterExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10566 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmIndirectRegisterExpression* SgAsmIndirectRegisterExpression::build_node_from_nonlist_children ( Rose::BinaryAnalysis::RegisterDescriptor descriptor, Rose::BinaryAnalysis::RegisterDescriptor stride, Rose::BinaryAnalysis::RegisterDescriptor offset, size_t index, size_t modulus )
   {
#if 0
  // debugging information!
     printf ("In SgAsmIndirectRegisterExpression::build_node_from_nonlist_children (RegisterDescriptor descriptor, RegisterDescriptor stride, RegisterDescriptor offset, size_t index, size_t modulus): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmIndirectRegisterExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10584 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmControlFlagsExpression* SgAsmControlFlagsExpression::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmControlFlagsExpression::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmControlFlagsExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10602 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmCommonSubExpression* SgAsmCommonSubExpression::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmCommonSubExpression::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmCommonSubExpression* returnNode = NULL;

     return returnNode;
   }


/* #line 10620 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmExprListExp* SgAsmExprListExp::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmExprListExp::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmExprListExp* returnNode = NULL;

     return returnNode;
   }


/* #line 10638 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmRegisterNames* SgAsmRegisterNames::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmRegisterNames::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmRegisterNames* returnNode = NULL;

     return returnNode;
   }


/* #line 10656 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmInterpretation* SgAsmInterpretation::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmInterpretation::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmInterpretation* returnNode = NULL;

     return returnNode;
   }


/* #line 10674 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmOperandList* SgAsmOperandList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmOperandList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmOperandList* returnNode = NULL;

     return returnNode;
   }


/* #line 10692 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmType* SgAsmType::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmType::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmType* returnNode = NULL;

     return returnNode;
   }


/* #line 10710 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmScalarType* SgAsmScalarType::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmScalarType::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmScalarType* returnNode = NULL;

     return returnNode;
   }


/* #line 10728 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmIntegerType* SgAsmIntegerType::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmIntegerType::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmIntegerType* returnNode = NULL;

     return returnNode;
   }


/* #line 10746 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmFloatType* SgAsmFloatType::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmFloatType::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmFloatType* returnNode = NULL;

     return returnNode;
   }


/* #line 10764 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmVectorType* SgAsmVectorType::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmVectorType::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmVectorType* returnNode = NULL;

     return returnNode;
   }


/* #line 10782 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmExecutableFileFormat* SgAsmExecutableFileFormat::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmExecutableFileFormat::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmExecutableFileFormat* returnNode = NULL;

     return returnNode;
   }


/* #line 10800 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericDLL* SgAsmGenericDLL::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericDLL::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericDLL* returnNode = NULL;

     return returnNode;
   }


/* #line 10818 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericFormat* SgAsmGenericFormat::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericFormat::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericFormat* returnNode = NULL;

     return returnNode;
   }


/* #line 10836 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericDLLList* SgAsmGenericDLLList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericDLLList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericDLLList* returnNode = NULL;

     return returnNode;
   }


/* #line 10854 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfEHFrameEntryFD* SgAsmElfEHFrameEntryFD::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfEHFrameEntryFD::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfEHFrameEntryFD* returnNode = NULL;

     return returnNode;
   }


/* #line 10872 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericFile* SgAsmGenericFile::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericFile::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericFile* returnNode = NULL;

     return returnNode;
   }


/* #line 10890 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericSection* SgAsmGenericSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericSection* returnNode = NULL;

     return returnNode;
   }


/* #line 10908 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericHeader* SgAsmGenericHeader::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericHeader::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericHeader* returnNode = NULL;

     return returnNode;
   }


/* #line 10926 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEFileHeader* SgAsmPEFileHeader::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEFileHeader::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEFileHeader* returnNode = NULL;

     return returnNode;
   }


/* #line 10944 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmLEFileHeader* SgAsmLEFileHeader::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmLEFileHeader::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmLEFileHeader* returnNode = NULL;

     return returnNode;
   }


/* #line 10962 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNEFileHeader* SgAsmNEFileHeader::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNEFileHeader::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNEFileHeader* returnNode = NULL;

     return returnNode;
   }


/* #line 10980 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDOSFileHeader* SgAsmDOSFileHeader::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDOSFileHeader::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDOSFileHeader* returnNode = NULL;

     return returnNode;
   }


/* #line 10998 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfFileHeader* SgAsmElfFileHeader::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfFileHeader::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfFileHeader* returnNode = NULL;

     return returnNode;
   }


/* #line 11016 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSection* SgAsmElfSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11034 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymbolSection* SgAsmElfSymbolSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymbolSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymbolSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11052 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfRelocSection* SgAsmElfRelocSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfRelocSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfRelocSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11070 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfDynamicSection* SgAsmElfDynamicSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfDynamicSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfDynamicSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11088 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfStringSection* SgAsmElfStringSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfStringSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfStringSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11106 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfNoteSection* SgAsmElfNoteSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfNoteSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfNoteSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11124 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfEHFrameSection* SgAsmElfEHFrameSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfEHFrameSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfEHFrameSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11142 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverSection* SgAsmElfSymverSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11160 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverDefinedSection* SgAsmElfSymverDefinedSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverDefinedSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverDefinedSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11178 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverNeededSection* SgAsmElfSymverNeededSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverNeededSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverNeededSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11196 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSectionTable* SgAsmElfSectionTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSectionTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSectionTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11214 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSegmentTable* SgAsmElfSegmentTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSegmentTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSegmentTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11232 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPESection* SgAsmPESection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPESection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPESection* returnNode = NULL;

     return returnNode;
   }


/* #line 11250 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEImportSection* SgAsmPEImportSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEImportSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEImportSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11268 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEExportSection* SgAsmPEExportSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEExportSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEExportSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11286 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEStringSection* SgAsmPEStringSection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEStringSection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEStringSection* returnNode = NULL;

     return returnNode;
   }


/* #line 11304 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPESectionTable* SgAsmPESectionTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPESectionTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPESectionTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11322 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDOSExtendedHeader* SgAsmDOSExtendedHeader::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDOSExtendedHeader::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDOSExtendedHeader* returnNode = NULL;

     return returnNode;
   }


/* #line 11340 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmCoffSymbolTable* SgAsmCoffSymbolTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmCoffSymbolTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmCoffSymbolTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11358 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNESection* SgAsmNESection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNESection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNESection* returnNode = NULL;

     return returnNode;
   }


/* #line 11376 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNESectionTable* SgAsmNESectionTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNESectionTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNESectionTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11394 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNENameTable* SgAsmNENameTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNENameTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNENameTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11412 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNEModuleTable* SgAsmNEModuleTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNEModuleTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNEModuleTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11430 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNEStringTable* SgAsmNEStringTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNEStringTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNEStringTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11448 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNEEntryTable* SgAsmNEEntryTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNEEntryTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNEEntryTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11466 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNERelocTable* SgAsmNERelocTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNERelocTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNERelocTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11484 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmLESection* SgAsmLESection::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmLESection::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmLESection* returnNode = NULL;

     return returnNode;
   }


/* #line 11502 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmLESectionTable* SgAsmLESectionTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmLESectionTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmLESectionTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11520 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmLENameTable* SgAsmLENameTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmLENameTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmLENameTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11538 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmLEPageTable* SgAsmLEPageTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmLEPageTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmLEPageTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11556 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmLEEntryTable* SgAsmLEEntryTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmLEEntryTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmLEEntryTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11574 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmLERelocTable* SgAsmLERelocTable::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmLERelocTable::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmLERelocTable* returnNode = NULL;

     return returnNode;
   }


/* #line 11592 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericSymbol* SgAsmGenericSymbol::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericSymbol::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 11610 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmCoffSymbol* SgAsmCoffSymbol::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmCoffSymbol::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmCoffSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 11628 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymbol* SgAsmElfSymbol::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymbol::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymbol* returnNode = NULL;

     return returnNode;
   }


/* #line 11646 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericStrtab* SgAsmGenericStrtab::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericStrtab::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericStrtab* returnNode = NULL;

     return returnNode;
   }


/* #line 11664 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfStrtab* SgAsmElfStrtab::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfStrtab::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfStrtab* returnNode = NULL;

     return returnNode;
   }


/* #line 11682 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmCoffStrtab* SgAsmCoffStrtab::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmCoffStrtab::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmCoffStrtab* returnNode = NULL;

     return returnNode;
   }


/* #line 11700 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericSymbolList* SgAsmGenericSymbolList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericSymbolList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericSymbolList* returnNode = NULL;

     return returnNode;
   }


/* #line 11718 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericSectionList* SgAsmGenericSectionList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericSectionList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericSectionList* returnNode = NULL;

     return returnNode;
   }


/* #line 11736 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericHeaderList* SgAsmGenericHeaderList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericHeaderList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericHeaderList* returnNode = NULL;

     return returnNode;
   }


/* #line 11754 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericString* SgAsmGenericString::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericString::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericString* returnNode = NULL;

     return returnNode;
   }


/* #line 11772 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmBasicString* SgAsmBasicString::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmBasicString::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmBasicString* returnNode = NULL;

     return returnNode;
   }


/* #line 11790 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmStoredString* SgAsmStoredString::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmStoredString::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmStoredString* returnNode = NULL;

     return returnNode;
   }


/* #line 11808 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSectionTableEntry* SgAsmElfSectionTableEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSectionTableEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSectionTableEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 11826 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSegmentTableEntry* SgAsmElfSegmentTableEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSegmentTableEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSegmentTableEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 11844 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymbolList* SgAsmElfSymbolList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymbolList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymbolList* returnNode = NULL;

     return returnNode;
   }


/* #line 11862 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfRelocEntry* SgAsmElfRelocEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfRelocEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfRelocEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 11880 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfRelocEntryList* SgAsmElfRelocEntryList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfRelocEntryList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfRelocEntryList* returnNode = NULL;

     return returnNode;
   }


/* #line 11898 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEExportEntry* SgAsmPEExportEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEExportEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEExportEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 11916 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEExportEntryList* SgAsmPEExportEntryList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEExportEntryList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEExportEntryList* returnNode = NULL;

     return returnNode;
   }


/* #line 11934 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfDynamicEntry* SgAsmElfDynamicEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfDynamicEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfDynamicEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 11952 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfDynamicEntryList* SgAsmElfDynamicEntryList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfDynamicEntryList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfDynamicEntryList* returnNode = NULL;

     return returnNode;
   }


/* #line 11970 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSegmentTableEntryList* SgAsmElfSegmentTableEntryList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSegmentTableEntryList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSegmentTableEntryList* returnNode = NULL;

     return returnNode;
   }


/* #line 11988 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmStringStorage* SgAsmStringStorage::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmStringStorage::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmStringStorage* returnNode = NULL;

     return returnNode;
   }


/* #line 12006 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfNoteEntry* SgAsmElfNoteEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfNoteEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfNoteEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 12024 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfNoteEntryList* SgAsmElfNoteEntryList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfNoteEntryList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfNoteEntryList* returnNode = NULL;

     return returnNode;
   }


/* #line 12042 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverEntry* SgAsmElfSymverEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 12060 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverEntryList* SgAsmElfSymverEntryList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverEntryList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverEntryList* returnNode = NULL;

     return returnNode;
   }


/* #line 12078 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverDefinedEntry* SgAsmElfSymverDefinedEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverDefinedEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverDefinedEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 12096 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverDefinedEntryList* SgAsmElfSymverDefinedEntryList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverDefinedEntryList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverDefinedEntryList* returnNode = NULL;

     return returnNode;
   }


/* #line 12114 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverDefinedAux* SgAsmElfSymverDefinedAux::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverDefinedAux::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverDefinedAux* returnNode = NULL;

     return returnNode;
   }


/* #line 12132 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverDefinedAuxList* SgAsmElfSymverDefinedAuxList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverDefinedAuxList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverDefinedAuxList* returnNode = NULL;

     return returnNode;
   }


/* #line 12150 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverNeededEntry* SgAsmElfSymverNeededEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverNeededEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverNeededEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 12168 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverNeededEntryList* SgAsmElfSymverNeededEntryList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverNeededEntryList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverNeededEntryList* returnNode = NULL;

     return returnNode;
   }


/* #line 12186 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverNeededAux* SgAsmElfSymverNeededAux::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverNeededAux::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverNeededAux* returnNode = NULL;

     return returnNode;
   }


/* #line 12204 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfSymverNeededAuxList* SgAsmElfSymverNeededAuxList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfSymverNeededAuxList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfSymverNeededAuxList* returnNode = NULL;

     return returnNode;
   }


/* #line 12222 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEImportDirectory* SgAsmPEImportDirectory::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEImportDirectory::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEImportDirectory* returnNode = NULL;

     return returnNode;
   }


/* #line 12240 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPESectionTableEntry* SgAsmPESectionTableEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPESectionTableEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPESectionTableEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 12258 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEExportDirectory* SgAsmPEExportDirectory::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEExportDirectory::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEExportDirectory* returnNode = NULL;

     return returnNode;
   }


/* #line 12276 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPERVASizePair* SgAsmPERVASizePair::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPERVASizePair::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPERVASizePair* returnNode = NULL;

     return returnNode;
   }


/* #line 12294 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmCoffSymbolList* SgAsmCoffSymbolList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmCoffSymbolList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmCoffSymbolList* returnNode = NULL;

     return returnNode;
   }


/* #line 12312 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPERVASizePairList* SgAsmPERVASizePairList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPERVASizePairList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPERVASizePairList* returnNode = NULL;

     return returnNode;
   }


/* #line 12330 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfEHFrameEntryCI* SgAsmElfEHFrameEntryCI::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfEHFrameEntryCI::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfEHFrameEntryCI* returnNode = NULL;

     return returnNode;
   }


/* #line 12348 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEImportDirectoryList* SgAsmPEImportDirectoryList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEImportDirectoryList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEImportDirectoryList* returnNode = NULL;

     return returnNode;
   }


/* #line 12366 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNEEntryPoint* SgAsmNEEntryPoint::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNEEntryPoint::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNEEntryPoint* returnNode = NULL;

     return returnNode;
   }


/* #line 12384 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNERelocEntry* SgAsmNERelocEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNERelocEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNERelocEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 12402 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmNESectionTableEntry* SgAsmNESectionTableEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmNESectionTableEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmNESectionTableEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 12420 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfEHFrameEntryCIList* SgAsmElfEHFrameEntryCIList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfEHFrameEntryCIList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfEHFrameEntryCIList* returnNode = NULL;

     return returnNode;
   }


/* #line 12438 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmLEPageTableEntry* SgAsmLEPageTableEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmLEPageTableEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmLEPageTableEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 12456 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmLEEntryPoint* SgAsmLEEntryPoint::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmLEEntryPoint::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmLEEntryPoint* returnNode = NULL;

     return returnNode;
   }


/* #line 12474 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmLESectionTableEntry* SgAsmLESectionTableEntry::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmLESectionTableEntry::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmLESectionTableEntry* returnNode = NULL;

     return returnNode;
   }


/* #line 12492 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmElfEHFrameEntryFDList* SgAsmElfEHFrameEntryFDList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmElfEHFrameEntryFDList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmElfEHFrameEntryFDList* returnNode = NULL;

     return returnNode;
   }


/* #line 12510 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfInformation* SgAsmDwarfInformation::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfInformation::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfInformation* returnNode = NULL;

     return returnNode;
   }


/* #line 12528 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfMacro* SgAsmDwarfMacro::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfMacro::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfMacro* returnNode = NULL;

     return returnNode;
   }


/* #line 12546 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfMacroList* SgAsmDwarfMacroList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfMacroList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfMacroList* returnNode = NULL;

     return returnNode;
   }


/* #line 12564 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfLine* SgAsmDwarfLine::build_node_from_nonlist_children ( uint64_t address, int file_id, int line, int column )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfLine::build_node_from_nonlist_children (uint64_t address, int file_id, int line, int column): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfLine* returnNode = NULL;

     return returnNode;
   }


/* #line 12582 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfLineList* SgAsmDwarfLineList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfLineList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfLineList* returnNode = NULL;

     return returnNode;
   }


/* #line 12600 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfCompilationUnitList* SgAsmDwarfCompilationUnitList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfCompilationUnitList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfCompilationUnitList* returnNode = NULL;

     return returnNode;
   }


/* #line 12618 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfConstruct* SgAsmDwarfConstruct::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfConstruct::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfConstruct* returnNode = NULL;

     return returnNode;
   }


/* #line 12636 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfArrayType* SgAsmDwarfArrayType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfArrayType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfArrayType* returnNode = NULL;

     return returnNode;
   }


/* #line 12654 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfClassType* SgAsmDwarfClassType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfClassType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfClassType* returnNode = NULL;

     return returnNode;
   }


/* #line 12672 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfEntryPoint* SgAsmDwarfEntryPoint::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfEntryPoint::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfEntryPoint* returnNode = NULL;

     return returnNode;
   }


/* #line 12690 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfEnumerationType* SgAsmDwarfEnumerationType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfEnumerationType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfEnumerationType* returnNode = NULL;

     return returnNode;
   }


/* #line 12708 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfFormalParameter* SgAsmDwarfFormalParameter::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfFormalParameter::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfFormalParameter* returnNode = NULL;

     return returnNode;
   }


/* #line 12726 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfImportedDeclaration* SgAsmDwarfImportedDeclaration::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfImportedDeclaration::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfImportedDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 12744 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfLabel* SgAsmDwarfLabel::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfLabel::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfLabel* returnNode = NULL;

     return returnNode;
   }


/* #line 12762 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfLexicalBlock* SgAsmDwarfLexicalBlock::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfLexicalBlock::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfLexicalBlock* returnNode = NULL;

     return returnNode;
   }


/* #line 12780 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfMember* SgAsmDwarfMember::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfMember::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfMember* returnNode = NULL;

     return returnNode;
   }


/* #line 12798 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfPointerType* SgAsmDwarfPointerType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfPointerType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfPointerType* returnNode = NULL;

     return returnNode;
   }


/* #line 12816 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfReferenceType* SgAsmDwarfReferenceType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfReferenceType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfReferenceType* returnNode = NULL;

     return returnNode;
   }


/* #line 12834 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfCompilationUnit* SgAsmDwarfCompilationUnit::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfCompilationUnit::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfCompilationUnit* returnNode = NULL;

     return returnNode;
   }


/* #line 12852 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfStringType* SgAsmDwarfStringType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfStringType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfStringType* returnNode = NULL;

     return returnNode;
   }


/* #line 12870 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfStructureType* SgAsmDwarfStructureType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfStructureType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfStructureType* returnNode = NULL;

     return returnNode;
   }


/* #line 12888 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfSubroutineType* SgAsmDwarfSubroutineType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfSubroutineType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfSubroutineType* returnNode = NULL;

     return returnNode;
   }


/* #line 12906 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfTypedef* SgAsmDwarfTypedef::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfTypedef::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfTypedef* returnNode = NULL;

     return returnNode;
   }


/* #line 12924 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfUnionType* SgAsmDwarfUnionType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfUnionType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfUnionType* returnNode = NULL;

     return returnNode;
   }


/* #line 12942 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfUnspecifiedParameters* SgAsmDwarfUnspecifiedParameters::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfUnspecifiedParameters::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfUnspecifiedParameters* returnNode = NULL;

     return returnNode;
   }


/* #line 12960 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfVariant* SgAsmDwarfVariant::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfVariant::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfVariant* returnNode = NULL;

     return returnNode;
   }


/* #line 12978 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfCommonBlock* SgAsmDwarfCommonBlock::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfCommonBlock::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfCommonBlock* returnNode = NULL;

     return returnNode;
   }


/* #line 12996 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfCommonInclusion* SgAsmDwarfCommonInclusion::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfCommonInclusion::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfCommonInclusion* returnNode = NULL;

     return returnNode;
   }


/* #line 13014 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfInheritance* SgAsmDwarfInheritance::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfInheritance::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfInheritance* returnNode = NULL;

     return returnNode;
   }


/* #line 13032 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfInlinedSubroutine* SgAsmDwarfInlinedSubroutine::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfInlinedSubroutine::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfInlinedSubroutine* returnNode = NULL;

     return returnNode;
   }


/* #line 13050 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfModule* SgAsmDwarfModule::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfModule::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfModule* returnNode = NULL;

     return returnNode;
   }


/* #line 13068 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfPtrToMemberType* SgAsmDwarfPtrToMemberType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfPtrToMemberType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfPtrToMemberType* returnNode = NULL;

     return returnNode;
   }


/* #line 13086 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfSetType* SgAsmDwarfSetType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfSetType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfSetType* returnNode = NULL;

     return returnNode;
   }


/* #line 13104 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfSubrangeType* SgAsmDwarfSubrangeType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfSubrangeType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfSubrangeType* returnNode = NULL;

     return returnNode;
   }


/* #line 13122 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfWithStmt* SgAsmDwarfWithStmt::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfWithStmt::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfWithStmt* returnNode = NULL;

     return returnNode;
   }


/* #line 13140 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfAccessDeclaration* SgAsmDwarfAccessDeclaration::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfAccessDeclaration::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfAccessDeclaration* returnNode = NULL;

     return returnNode;
   }


/* #line 13158 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfBaseType* SgAsmDwarfBaseType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfBaseType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfBaseType* returnNode = NULL;

     return returnNode;
   }


/* #line 13176 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfCatchBlock* SgAsmDwarfCatchBlock::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfCatchBlock::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfCatchBlock* returnNode = NULL;

     return returnNode;
   }


/* #line 13194 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfConstType* SgAsmDwarfConstType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfConstType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfConstType* returnNode = NULL;

     return returnNode;
   }


/* #line 13212 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfConstant* SgAsmDwarfConstant::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfConstant::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfConstant* returnNode = NULL;

     return returnNode;
   }


/* #line 13230 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfEnumerator* SgAsmDwarfEnumerator::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfEnumerator::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfEnumerator* returnNode = NULL;

     return returnNode;
   }


/* #line 13248 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfFileType* SgAsmDwarfFileType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfFileType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfFileType* returnNode = NULL;

     return returnNode;
   }


/* #line 13266 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfFriend* SgAsmDwarfFriend::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfFriend::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfFriend* returnNode = NULL;

     return returnNode;
   }


/* #line 13284 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfNamelist* SgAsmDwarfNamelist::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfNamelist::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfNamelist* returnNode = NULL;

     return returnNode;
   }


/* #line 13302 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfNamelistItem* SgAsmDwarfNamelistItem::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfNamelistItem::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfNamelistItem* returnNode = NULL;

     return returnNode;
   }


/* #line 13320 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfPackedType* SgAsmDwarfPackedType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfPackedType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfPackedType* returnNode = NULL;

     return returnNode;
   }


/* #line 13338 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfSubprogram* SgAsmDwarfSubprogram::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfSubprogram::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfSubprogram* returnNode = NULL;

     return returnNode;
   }


/* #line 13356 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfTemplateTypeParameter* SgAsmDwarfTemplateTypeParameter::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfTemplateTypeParameter::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfTemplateTypeParameter* returnNode = NULL;

     return returnNode;
   }


/* #line 13374 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfTemplateValueParameter* SgAsmDwarfTemplateValueParameter::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfTemplateValueParameter::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfTemplateValueParameter* returnNode = NULL;

     return returnNode;
   }


/* #line 13392 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfThrownType* SgAsmDwarfThrownType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfThrownType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfThrownType* returnNode = NULL;

     return returnNode;
   }


/* #line 13410 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfTryBlock* SgAsmDwarfTryBlock::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfTryBlock::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfTryBlock* returnNode = NULL;

     return returnNode;
   }


/* #line 13428 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfVariantPart* SgAsmDwarfVariantPart::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfVariantPart::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfVariantPart* returnNode = NULL;

     return returnNode;
   }


/* #line 13446 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfVariable* SgAsmDwarfVariable::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfVariable::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfVariable* returnNode = NULL;

     return returnNode;
   }


/* #line 13464 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfVolatileType* SgAsmDwarfVolatileType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfVolatileType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfVolatileType* returnNode = NULL;

     return returnNode;
   }


/* #line 13482 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfDwarfProcedure* SgAsmDwarfDwarfProcedure::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfDwarfProcedure::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfDwarfProcedure* returnNode = NULL;

     return returnNode;
   }


/* #line 13500 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfRestrictType* SgAsmDwarfRestrictType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfRestrictType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfRestrictType* returnNode = NULL;

     return returnNode;
   }


/* #line 13518 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfInterfaceType* SgAsmDwarfInterfaceType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfInterfaceType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfInterfaceType* returnNode = NULL;

     return returnNode;
   }


/* #line 13536 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfNamespace* SgAsmDwarfNamespace::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfNamespace::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfNamespace* returnNode = NULL;

     return returnNode;
   }


/* #line 13554 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfImportedModule* SgAsmDwarfImportedModule::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfImportedModule::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfImportedModule* returnNode = NULL;

     return returnNode;
   }


/* #line 13572 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfUnspecifiedType* SgAsmDwarfUnspecifiedType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfUnspecifiedType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfUnspecifiedType* returnNode = NULL;

     return returnNode;
   }


/* #line 13590 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfPartialUnit* SgAsmDwarfPartialUnit::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfPartialUnit::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfPartialUnit* returnNode = NULL;

     return returnNode;
   }


/* #line 13608 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfImportedUnit* SgAsmDwarfImportedUnit::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfImportedUnit::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfImportedUnit* returnNode = NULL;

     return returnNode;
   }


/* #line 13626 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfMutableType* SgAsmDwarfMutableType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfMutableType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfMutableType* returnNode = NULL;

     return returnNode;
   }


/* #line 13644 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfCondition* SgAsmDwarfCondition::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfCondition::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfCondition* returnNode = NULL;

     return returnNode;
   }


/* #line 13662 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfSharedType* SgAsmDwarfSharedType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfSharedType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfSharedType* returnNode = NULL;

     return returnNode;
   }


/* #line 13680 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfFormatLabel* SgAsmDwarfFormatLabel::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfFormatLabel::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfFormatLabel* returnNode = NULL;

     return returnNode;
   }


/* #line 13698 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfFunctionTemplate* SgAsmDwarfFunctionTemplate::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfFunctionTemplate::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfFunctionTemplate* returnNode = NULL;

     return returnNode;
   }


/* #line 13716 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfClassTemplate* SgAsmDwarfClassTemplate::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfClassTemplate::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfClassTemplate* returnNode = NULL;

     return returnNode;
   }


/* #line 13734 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfUpcSharedType* SgAsmDwarfUpcSharedType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfUpcSharedType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfUpcSharedType* returnNode = NULL;

     return returnNode;
   }


/* #line 13752 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfUpcStrictType* SgAsmDwarfUpcStrictType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfUpcStrictType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfUpcStrictType* returnNode = NULL;

     return returnNode;
   }


/* #line 13770 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfUpcRelaxedType* SgAsmDwarfUpcRelaxedType::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfUpcRelaxedType::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfUpcRelaxedType* returnNode = NULL;

     return returnNode;
   }


/* #line 13788 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfUnknownConstruct* SgAsmDwarfUnknownConstruct::build_node_from_nonlist_children ( int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfUnknownConstruct::build_node_from_nonlist_children (int nesting_level, uint64_t offset, uint64_t overall_offset): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfUnknownConstruct* returnNode = NULL;

     return returnNode;
   }


/* #line 13806 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmDwarfConstructList* SgAsmDwarfConstructList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmDwarfConstructList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmDwarfConstructList* returnNode = NULL;

     return returnNode;
   }


/* #line 13824 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEImportItem* SgAsmPEImportItem::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEImportItem::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEImportItem* returnNode = NULL;

     return returnNode;
   }


/* #line 13842 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmPEImportItemList* SgAsmPEImportItemList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmPEImportItemList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmPEImportItemList* returnNode = NULL;

     return returnNode;
   }


/* #line 13860 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmInterpretationList* SgAsmInterpretationList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmInterpretationList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmInterpretationList* returnNode = NULL;

     return returnNode;
   }


/* #line 13878 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAsmGenericFileList* SgAsmGenericFileList::build_node_from_nonlist_children (  )
   {
#if 0
  // debugging information!
     printf ("In SgAsmGenericFileList::build_node_from_nonlist_children (): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsmGenericFileList* returnNode = NULL;

     return returnNode;
   }


/* #line 13896 "../../../src/frontend/SageIII//Cxx_GrammarAtermNodeBuildFunctions.C" */

/* #line 1 "/home/dquinlan/ROSE/git-dq-edg49-fortran-rc/src/ROSETTA/Grammar/grammarAtermConstructorDefinitionMacros.macro" */

// Generated static function for building IR nodes for Aterm support (like a constructor).
SgAterm* SgAterm::build_node_from_nonlist_children ( std::string name )
   {
#if 0
  // debugging information!
     printf ("In SgAterm::build_node_from_nonlist_children (std::string name): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAterm* returnNode = NULL;

     return returnNode;
   }

// DQ (12/11/2014): Added these which were missing as a result of new C++11 support.
SgLambdaCapture* SgLambdaCapture::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgExpression*, bool, bool, bool)
   {
#if 0
  // debugging information!
     printf ("In SgLambdaCapture::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgExpression*, bool, bool, bool): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLambdaCapture* returnNode = NULL;

     return returnNode;
   }

// DQ (12/11/2014): Added these which were missing as a result of new C++11 support.
SgTemplateTypedefDeclaration* SgTemplateTypedefDeclaration::build_node_from_nonlist_children(Sg_File_Info*, SgName, SgType*, SgTypedefType*, SgDeclarationStatement*, SgSymbol*)
   {
#if 0
  // debugging information!
     printf ("In SgTemplateTypedefDeclaration::build_node_from_nonlist_children(Sg_File_Info*, SgName, SgType*, SgTypedefType*, SgDeclarationStatement*, SgSymbol*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateTypedefDeclaration* returnNode = NULL;

     return returnNode;
   }

// DQ (12/11/2014): Added these which were missing as a result of new C++11 support.
SgFinishStmt* SgFinishStmt::build_node_from_nonlist_children(Sg_File_Info*, SgStatement*)
   {
#if 0
  // debugging information!
     printf ("In SgFinishStmt::build_node_from_nonlist_children(Sg_File_Info*, SgStatement*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgFinishStmt* returnNode = NULL;

     return returnNode;
   }

// DQ (12/11/2014): Added these which were missing as a result of new C++11 support.
SgAtStmt* SgAtStmt::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgStatement*)
   {
#if 0
  // debugging information!
     printf ("In SgAtStmt::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgStatement*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAtStmt* returnNode = NULL;

     return returnNode;
   }

// DQ (12/11/2014): Added these which were missing as a result of new C++11 support.
SgHereExp* SgHereExp::build_node_from_nonlist_children(Sg_File_Info*, SgType*, SgExpression*)
   {
#if 0
  // debugging information!
     printf ("In SgHereExp::build_node_from_nonlist_children(Sg_File_Info*, SgType*, SgExpression*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgHereExp* returnNode = NULL;

     return returnNode;
   }

// DQ (12/11/2014): Added these which were missing as a result of new C++11 support.
SgLambdaCaptureList* SgLambdaCaptureList::build_node_from_nonlist_children(Sg_File_Info*)
   {
#if 0
  // debugging information!
     printf ("In SgLambdaCaptureList::build_node_from_nonlist_children(Sg_File_Info*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLambdaCaptureList* returnNode = NULL;

     return returnNode;
   }

// DQ (12/11/2014): Added these which were missing as a result of new C++11 support.
SgTemplateTypedefSymbol* SgTemplateTypedefSymbol::build_node_from_nonlist_children(SgTypedefDeclaration*)
   {
#if 0
  // debugging information!
     printf ("In SgTemplateTypedefSymbol::build_node_from_nonlist_children(SgTypedefDeclaration*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateTypedefSymbol* returnNode = NULL;

     return returnNode;
   }

// DQ (12/11/2014): Added these which were missing as a result of new C++11 support.
SgTemplateInstantiationTypedefDeclaration* SgTemplateInstantiationTypedefDeclaration::build_node_from_nonlist_children(Sg_File_Info*, SgName, SgType*, SgTypedefType*, SgDeclarationStatement*, SgSymbol*, SgTemplateTypedefDeclaration*, std::vector<SgTemplateArgument*, std::allocator<SgTemplateArgument*> >)
   {
#if 0
  // debugging information!
     printf ("In SgTemplateInstantiationTypedefDeclaration::build_node_from_nonlist_children(Sg_File_Info*, SgName, SgType*, SgTypedefType*, SgDeclarationStatement*, SgSymbol*, SgTemplateTypedefDeclaration*, std::vector<SgTemplateArgument*, std::allocator<SgTemplateArgument*> >): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgTemplateInstantiationTypedefDeclaration* returnNode = NULL;

     return returnNode;
   }

// DQ (12/11/2014): Added these which were missing as a result of new C++11 support.
SgLambdaExp* SgLambdaExp::build_node_from_nonlist_children(Sg_File_Info*, SgLambdaCaptureList*, SgClassDeclaration*, SgFunctionDeclaration*)
   {
#if 0
  // debugging information!
     printf ("In SgLambdaExp::build_node_from_nonlist_children(Sg_File_Info*, SgLambdaCaptureList*, SgClassDeclaration*, SgFunctionDeclaration*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLambdaExp* returnNode = NULL;

     return returnNode;
   }

// DQ (12/11/2014): Added these which were missing as a result of new C++11 support.
SgAsyncStmt* SgAsyncStmt::build_node_from_nonlist_children(Sg_File_Info*, SgStatement*)
   {
#if 0
  // debugging information!
     printf ("In SgAsyncStmt::build_node_from_nonlist_children(Sg_File_Info*, SgStatement*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgAsyncStmt* returnNode = NULL;

     return returnNode;
   }

#if 0
  // DQ (1/18/2016): Need to add more complete support for newer IR nodes (Matlab, Asm, and OpenMP nodes).
     SgLeftDivideOp
     SgElementwiseDivideOp
     SgElementwisePowerOp
     SgElementwiseLeftDivideOp
     SgElementwiseAddOp
     SgOmpEndClause
     SgRangeExp
     SgOmpBeginClause
     SgMatrixTransposeOp
     SgNoexceptOp
     SgPowerOp
     SgAsmRiscOperation
     SgMatrixExp
     SgElementwiseSubtractOp
     SgMagicColonExp
     SgElementwiseMultiplyOp

  // Support for statement:
     SgMatlabForStatement

  // Support for types:
     SgTypeMatrix
     SgTypeOfType
     SgTypeTuple
#endif

// SgLeftDivideOp* SgLeftDivideOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgLeftDivideOp* SgLeftDivideOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgType*)
   {
#if 0
  // debugging information!
     printf ("In SgLeftDivideOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*): this = %p = %s \n",this,this->class_name().c_str());
#endif

     SgLeftDivideOp* returnNode = NULL;

     return returnNode;
   }

// SgElementwiseDivideOp* SgElementwiseDivideOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgElementwiseDivideOp* SgElementwiseDivideOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgType*)
   {
     SgElementwiseDivideOp* returnNode = NULL;
     return returnNode;
   }

// SgElementwisePowerOp* SgElementwisePowerOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgElementwisePowerOp* SgElementwisePowerOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgType*)
   {
     SgElementwisePowerOp* returnNode = NULL;
     return returnNode;
   }

// SgElementwiseLeftDivideOp* SgElementwiseLeftDivideOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgElementwiseLeftDivideOp* SgElementwiseLeftDivideOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgType*)
   {
     SgElementwiseLeftDivideOp* returnNode = NULL;
     return returnNode;
   }

// SgElementwiseAddOp* SgElementwiseAddOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgElementwiseAddOp* SgElementwiseAddOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgType*)
   {
     SgElementwiseAddOp* returnNode = NULL;
     return returnNode;
   }

// SgOmpEndClause* SgOmpEndClause::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgOmpEndClause* SgOmpEndClause::build_node_from_nonlist_children(Sg_File_Info*)
   {
     SgOmpEndClause* returnNode = NULL;
     return returnNode;
   }

// SgRangeExp* SgRangeExp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgRangeExp* SgRangeExp::build_node_from_nonlist_children(Sg_File_Info*)
   {
     SgRangeExp* returnNode = NULL;
     return returnNode;
   }

// SgOmpBeginClause* SgOmpBeginClause::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgOmpBeginClause* SgOmpBeginClause::build_node_from_nonlist_children(Sg_File_Info*)
   {
     SgOmpBeginClause* returnNode = NULL;
     return returnNode;
   }

// SgMatrixTransposeOp* SgMatrixTransposeOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgMatrixTransposeOp* SgMatrixTransposeOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgType*)
   {
     SgMatrixTransposeOp* returnNode = NULL;
     return returnNode;
   }

// SgNoexceptOp* SgNoexceptOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgNoexceptOp* SgNoexceptOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
   {
     SgNoexceptOp* returnNode = NULL;
     return returnNode;
   }

// SgPowerOp* SgPowerOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgPowerOp* SgPowerOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgType*)
   {
     SgPowerOp* returnNode = NULL;
     return returnNode;
   }

#if 0
// SgAsmRiscOperation* SgAsmRiscOperation::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgAsmRiscOperation* SgAsmRiscOperation::build_node_from_nonlist_children((SgAsmRiscOperation::RiscOperator)
   {
     SgAsmRiscOperation* returnNode = NULL;
     return returnNode;
   }
#endif

SgAsmRiscOperation* SgAsmRiscOperation::build_node_from_nonlist_children(SgAsmRiscOperation::RiscOperator riscOperator )
   {
     SgAsmRiscOperation* returnNode = NULL;
     return returnNode;
   }


// SgMatrixExp* SgMatrixExp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgMatrixExp* SgMatrixExp::build_node_from_nonlist_children(Sg_File_Info*)
   {
     SgMatrixExp* returnNode = NULL;
     return returnNode;
   }

// SgElementwiseSubtractOp* SgElementwiseSubtractOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgElementwiseSubtractOp* SgElementwiseSubtractOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgType*)
   {
     SgElementwiseSubtractOp* returnNode = NULL;
     return returnNode;
   }

// SgMagicColonExp* SgMagicColonExp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgMagicColonExp* SgMagicColonExp::build_node_from_nonlist_children(Sg_File_Info*)
   {
     SgMagicColonExp* returnNode = NULL;
     return returnNode;
   }

// SgElementwiseMultiplyOp* SgElementwiseMultiplyOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*)
SgElementwiseMultiplyOp* SgElementwiseMultiplyOp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgType*)
   {
     SgElementwiseMultiplyOp* returnNode = NULL;
     return returnNode;
   }

// SgMatlabForStatement* SgMatlabForStatement::build_node_from_nonlist_children(Sg_File_Info*, SgStatement*)
SgMatlabForStatement* SgMatlabForStatement::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgBasicBlock*)
   {
     SgMatlabForStatement* returnNode = NULL;
     return returnNode;
   }

// SgTypeMatrix* SgTypeMatrix::build_node_from_nonlist_children(Sg_File_Info*, SgType*)
SgTypeMatrix* SgTypeMatrix::build_node_from_nonlist_children()
   {
     SgTypeMatrix* returnNode = NULL;
     return returnNode;
   }

// SgTypeOfType* SgTypeOfType::build_node_from_nonlist_children(Sg_File_Info*, SgType*)
SgTypeOfType* SgTypeOfType::build_node_from_nonlist_children(SgExpression*, SgType*)
   {
     SgTypeOfType* returnNode = NULL;
     return returnNode;
   }

// SgTypeTuple* SgTypeTuple::build_node_from_nonlist_children(Sg_File_Info*, SgType*)
SgTypeTuple* SgTypeTuple::build_node_from_nonlist_children()
   {
     SgTypeTuple* returnNode = NULL;
     return returnNode;
   }


// DQ (1/26/2017): Manually added member functions for what are a few new IR nodes added in the last year of so.
SgFunctionParameterScope* SgFunctionParameterScope::build_node_from_nonlist_children(Sg_File_Info*)
   {
     SgFunctionParameterScope* returnNode = NULL;
     return returnNode;
   }

SgDotDotExp* SgDotDotExp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgExpression*, SgType*)
   {
     SgDotDotExp* returnNode = NULL;
     return returnNode;
   }

SgAtomicStmt* SgAtomicStmt::build_node_from_nonlist_children(Sg_File_Info*, SgStatement*)
   {
     SgAtomicStmt* returnNode = NULL;
     return returnNode;
   }

SgFinishExp* SgFinishExp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgStatement*)
   {
     SgFinishExp* returnNode = NULL;
     return returnNode;
   }

SgAtExp* SgAtExp::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgStatement*)
   {
     SgAtExp* returnNode = NULL;
     return returnNode;
   }

SgBracedInitializer* SgBracedInitializer::build_node_from_nonlist_children(Sg_File_Info*, SgExprListExp*, SgType*)
   {
     SgBracedInitializer* returnNode = NULL;
     return returnNode;
   }

SgWhenStmt* SgWhenStmt::build_node_from_nonlist_children(Sg_File_Info*, SgExpression*, SgStatement*)
   {
     SgWhenStmt* returnNode = NULL;
     return returnNode;
   }

// DQ (12/2/2018: Added support for new IR node.
SgIncludeFile* SgIncludeFile::build_node_from_nonlist_children ( SgName name )
   {
     SgIncludeFile* returnNode = NULL;

     return returnNode;
   }

SgNonrealRefExp* SgNonrealRefExp::build_node_from_nonlist_children(Sg_File_Info*, SgNonrealSymbol*)
   {
     SgNonrealRefExp* returnNode = NULL;
     return returnNode;
   }

SgNonrealBaseClass* SgNonrealBaseClass::build_node_from_nonlist_children(SgClassDeclaration*, bool, SgNonrealDecl*)
   {
     SgNonrealBaseClass* returnNode = NULL;
     return returnNode;
   }

SgNonrealType* SgNonrealType::build_node_from_nonlist_children(SgDeclarationStatement*)
   {
     SgNonrealType* returnNode = NULL;
     return returnNode;
   }

SgNonrealSymbol* SgNonrealSymbol::build_node_from_nonlist_children(SgNonrealDecl*)
   {
     SgNonrealSymbol* returnNode = NULL;
     return returnNode;
   }

SgNonrealDecl* SgNonrealDecl::build_node_from_nonlist_children(Sg_File_Info*, SgName)
   {
     SgNonrealDecl* returnNode = NULL;
     return returnNode;
   }

SgAutoType* SgAutoType::build_node_from_nonlist_children()
   {
     SgAutoType* returnNode = NULL;
     return returnNode;
   }

SgFloat80Val* SgFloat80Val::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, long double value, std::string valueString)
   {
     SgFloat80Val* returnNode = NULL;
     return returnNode;
   }

SgFloat128Val* SgFloat128Val::build_node_from_nonlist_children(Sg_File_Info* startOfConstruct, long double value, std::string valueString)
   {
     SgFloat128Val* returnNode = NULL;
     return returnNode;
   }


SgVoidVal* SgVoidVal::build_node_from_nonlist_children (Sg_File_Info*)
   {
     SgVoidVal* returnNode = NULL;

     return returnNode;
   }

SgEmptyDeclaration* SgEmptyDeclaration::build_node_from_nonlist_children (Sg_File_Info*)
   {
     SgEmptyDeclaration* returnNode = NULL;

     return returnNode;
   }


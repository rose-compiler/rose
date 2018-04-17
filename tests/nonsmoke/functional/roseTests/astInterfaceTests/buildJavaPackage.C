// Demonstrate how to build a the initial Java package support.
//
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utility tools 
//-------------------------------------------------------------------
#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;
using namespace std;

int main (int argc, char *argv[])
   {
     SgProject* project = frontend (argc, argv);
     ROSE_ASSERT(project != NULL);

  // SgSourceFile* sourceFile = isSgSourceFile(project->get_fileList_ptr()->operator[](0));
     SgSourceFile* sourceFile = isSgSourceFile(project->operator[](0));
     ROSE_ASSERT(sourceFile != NULL);

     SgGlobal *globalScope = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

     SgName javaLangPackage = "java_lang";

  // Depricated function you were trying to call...
  // SgClassDeclaration *declaration = SageBuilder::buildDefiningClassDeclaration(javaLangPackage, globalScope)

  // New API function that should be called.
  // SgClassDeclaration* declaration = buildClassDeclaration_nfi(const SgName& XXX_name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgClassDeclaration* nonDefiningDecl , bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList )

     SgClassDeclaration* nonDefiningDecl              = NULL;
     bool buildTemplateInstantiation                  = false;
     SgTemplateArgumentPtrList* templateArgumentsList = NULL;

     SgClassDeclaration* declaration = buildClassDeclaration_nfi(javaLangPackage, SgClassDeclaration::e_class, globalScope, nonDefiningDecl, buildTemplateInstantiation, templateArgumentsList);
     ROSE_ASSERT(declaration != NULL);

     return 0;
   }


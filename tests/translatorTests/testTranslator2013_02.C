// This test code demonstrates a warning that will not happen in the 
// construction of the AST from the EDG/ROSE translation.

// The warning is that the non-defining and defining declaration will 
// not have the same scope.  This happens because the class "rebind"
// (in the associated input code) is in the base class "new_allocate"
// and thus appears in the symbol table of the class "allocate"

// This is related to a problem pointed out for Java, but was fixed
// when the implementation of the import statement in Java was
// fixed to not redundantly incert the associated package's symbols
// into the scope.  This combined with the new organization of the 
// Java AST to include a SgGlobal (global scope) in the SgProject
// was redundnat and the root cause of the problem.  Still we incert
// SgAliasSymbols into the derived class definition for each symbol
// in the base class's class definition.

#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:void visit(SgNode* astNode);
     bool done;
   };

void SimpleInstrumentation::visit(SgNode* astNode)
   {
     printf ("In SimpleInstrumentation::visit(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());

  // SgFile *file=isSgFile(astNode);
     SgClassDefinition* classdef = isSgClassDefinition(astNode);
     SgClassDeclaration* target_class_declaration = NULL;

     if (classdef != NULL)
        {
          target_class_declaration = classdef->get_declaration();
          ROSE_ASSERT(target_class_declaration != NULL);
        }
     
     if (target_class_declaration != NULL && target_class_declaration->get_name() == "allocator" && !done)
        {
          ROSE_ASSERT(classdef != NULL);
          printf ("Found SgClassDefinition: classdef->get_declaration()->get_name() = %s \n",classdef->get_declaration()->get_name().str());

       // SgScopeStatement *scope = getGlobalScope(classdef);
          SgScopeStatement *scope = classdef;
#if 1
       // SgClassDeclaration* buildClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgClassDeclaration* nonDefiningDecl, bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList);
       // SgClassDeclaration *class_defn = buildDefiningClassDeclaration(SgName("testClass"),scope);
          SgName new_class_name = "rebind";
          SgClassDeclaration *class_decl = buildClassDeclaration_nfi(new_class_name,SgClassDeclaration::e_class,scope,NULL,false,NULL);

       // prependStatement(class_decl, scope);
          appendStatement(class_decl, scope);
#endif
          done = true;
        }
   }

int main(int argc, char *argv[])
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project!=NULL);

     SimpleInstrumentation treeTraversal;
     treeTraversal.done = false;
     treeTraversal.traverseInputFiles(project, preorder);

  // Output an optional graph of the AST (just the tree, when active)
     generateDOT ( *project );

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

     return backend(project);
   }



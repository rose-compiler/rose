// Investigation of the repeated representation of getline in the AST from iostream header file processing.
// Not clear if this is a real problem or not, but I would like to justify it's number of occurences in the AST.
// Plus this appears to be linked to why the name qualification is trying to report that it needs one level
// of name qualification, when it appears that it sould not.  It might point to somehting in the name qualification
// support that need to be more sophisticated than it is currently.

#include "rose.h"

// using namespace SageBuilder;
// using namespace SageInterface;
using namespace std;

class SimpleTransformation : public SgSimpleProcessing
   {
     public:
          void visit(SgNode* astNode);
   };

void SimpleTransformation::visit(SgNode* astNode)
   {
  // Visit function to iterate over the AST and transform all functions.
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
     if (functionDeclaration != NULL)
        {
          string name = functionDeclaration->get_name();
          std::size_t pos = name.find("getline");
          if (pos != string::npos)
             {
               printf ("found getline function: pos = %zu \n",pos);
               printf ("   --- function name = %s \n",name.c_str());
               printf ("   --- functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());
               SgTemplateInstantiationFunctionDecl*       templateInstantiationFunctionDecl       = isSgTemplateInstantiationFunctionDecl(functionDeclaration);
               SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(functionDeclaration);

               if (templateInstantiationFunctionDecl != NULL)
                  {
                    SgTemplateFunctionDeclaration* templateFunctionDeclaration = isSgTemplateFunctionDeclaration(templateInstantiationFunctionDecl->get_templateDeclaration());
                    ROSE_ASSERT(templateFunctionDeclaration != NULL);
                    printf ("   --- templateFunctionDeclaration = %p = %s \n",templateFunctionDeclaration,templateFunctionDeclaration->class_name().c_str());
                    printf ("   --- templateFunctionDeclaration->get_file_info()->get_filename() = %s \n",templateFunctionDeclaration->get_file_info()->get_filename());
                    printf ("   --- templateFunctionDeclaration->get_file_info()->get_line()     = %d \n",templateFunctionDeclaration->get_file_info()->get_line());
                  }
                 else
                  {
                    if (templateInstantiationMemberFunctionDecl != NULL)
                       {
                         SgTemplateMemberFunctionDeclaration* templateMemberFunctionDeclaration = isSgTemplateMemberFunctionDeclaration(templateInstantiationMemberFunctionDecl->get_templateDeclaration());
                         ROSE_ASSERT(templateMemberFunctionDeclaration != NULL);
                         printf ("   --- templateMemberFunctionDeclaration = %p = %s \n",templateMemberFunctionDeclaration,templateMemberFunctionDeclaration->class_name().c_str());
                         printf ("   --- templateMemberFunctionDeclaration->get_file_info()->get_filename() = %s \n",templateMemberFunctionDeclaration->get_file_info()->get_filename());
                         printf ("   --- templateMemberFunctionDeclaration->get_file_info()->get_line()     = %d \n",templateMemberFunctionDeclaration->get_file_info()->get_line());
                       }
                      else
                       {
                         printf ("   --- functionDeclaration->get_file_info()->get_filename() = %s \n",functionDeclaration->get_file_info()->get_filename());
                         printf ("   --- functionDeclaration->get_file_info()->get_line()     = %d \n",functionDeclaration->get_file_info()->get_line());
                       }
                  }

               printf ("   --- functionDeclaration->isCompilerGenerated() = %s \n",functionDeclaration->isCompilerGenerated() ? "true" : "false");
               printf ("   --- functionDeclaration->isTransformation()    = %s \n",functionDeclaration->isTransformation()    ? "true" : "false");
            // printf ("   --- function: %s \n",functionDeclaration->unparseToString().c_str());
               if (functionDeclaration->isCompilerGenerated() == true)
                  {
                    ROSE_ASSERT(functionDeclaration->isTransformation() == false);

                    functionDeclaration->setTransformation();
                    functionDeclaration->setOutputInCodeGeneration();

                    printf ("   --- (before unparse) functionDeclaration->isTransformation()    = %s \n",functionDeclaration->isTransformation()    ? "true" : "false");

                    printf ("   --- function: %s \n",functionDeclaration->unparseToString().c_str());

                    functionDeclaration->unsetOutputInCodeGeneration();
                    functionDeclaration->unsetTransformation();
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
                 // printf ("   --- (non-compiler-generated) function: %s \n",functionDeclaration->unparseToString().c_str());
                    printf ("   --- function: %s \n",functionDeclaration->unparseToString().c_str());
                  }
             }
        }
   }

int main(int argc, char *argv[])
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project!=NULL);

     SimpleTransformation treeTraversal;
  // treeTraversal.traverseInputFiles(project, preorder);
     treeTraversal.traverse(project, preorder);

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     generateDOT ( *project );

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

#if 1
     return backend(project);
#else
     return 0;
#endif
   }





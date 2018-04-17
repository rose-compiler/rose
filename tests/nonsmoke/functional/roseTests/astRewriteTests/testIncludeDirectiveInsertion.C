#include "rose.h"

using namespace std;

int
main( int argc, char * argv[] )
   {
  // This test code tests the AST rewrite mechanism to add include directives to the AST.

  // SgProject project(argc,argv);
     SgProject* project = frontend(argc,argv);

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
  // generatePDF(*project);

  // Output the source code file (as represented by the SAGE AST) as a DOT file (graph)
  // generateDOT(*project);

  // Allow compiler options to influence if we operate on the AST
     if ( project->get_skip_transformation() == false )
        {
       // NOTE: There can be multiple files on the command line and each file has a global scope

          MiddleLevelRewrite::ScopeIdentifierEnum   scope           = MidLevelCollectionTypedefs::StatementScope;
          MiddleLevelRewrite::PlacementPositionEnum locationInScope = MidLevelCollectionTypedefs::TopOfCurrentScope;

       // list<SgNode*> globalScopeList = NodeQuery::querySubTree (project,V_SgGlobal);
       // for (list<SgNode*>::iterator i = globalScopeList.begin(); i != globalScopeList.end(); i++)
          NodeQuerySynthesizedAttributeType globalScopeList = NodeQuery::querySubTree (project,V_SgGlobal);
          for (NodeQuerySynthesizedAttributeType::iterator i = globalScopeList.begin(); i != globalScopeList.end(); i++)
             {
            // Add a TAU include directive here

            // Need to converst *i from SgNode to at least a SgStatement
               ROSE_ASSERT ( (*i) != NULL);
               SgGlobal* globalScope = isSgGlobal(*i);
               ROSE_ASSERT (globalScope != NULL);

            // TAU does not seem to compile using EDG or g++ (need to sort this out with Brian)
            // MiddleLevelRewrite::insert(globalScope,"#define PROFILING_ON \n#include<TAU.h> \n",scope,locationInScope);
               MiddleLevelRewrite::insert(globalScope,"#include<tauProtos.h> \n",scope,locationInScope);
             }
#if 0
          list<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgFunctionDeclaration);
          for (list<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
               ROSE_ASSERT(functionDeclaration != NULL);
               SgFunctionDeclaration* definingFunctionDeclaration = isSgFunctionDeclaration(functionDeclaration->get_definingDeclaration());

            // The defining declaration is unique and we only want to process the function body once
               if (definingFunctionDeclaration == functionDeclaration)
                  {
                    ROSE_ASSERT(functionDeclaration->get_definition() != NULL);
                    SgBasicBlock* functionBody = functionDeclaration->get_definition()->get_body();
                    ROSE_ASSERT(functionBody != NULL);

                    string tauInstumentationString = "TauTimer tauTimerInstrumentation;\n";
                    MiddleLevelRewrite::insert(functionBody,tauInstumentationString,scope,locationInScope);
                  }
             }
#endif
        }
       else
        {
          printf ("project->get_skip_transformation() == true \n");
        }

  // Call the ROSE backend (unparse to generate transformed 
  // source code and compile it with vendor compiler).
     return backend(project);
   }


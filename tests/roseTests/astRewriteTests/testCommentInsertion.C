#include "rose.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

int
main( int argc, char * argv[] )
   {
  // This test code tests the AST rewrite mechanism to add comments to the AST.

  // SgProject project(argc,argv);
     SgProject* project = frontend(argc,argv);

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
  // generatePDF(*project);

  // Output the source code file (as represented by the SAGE AST) as a DOT file (graph)
  // generateDOT(*project);

  // Allow compiler options to influence if we operate on the AST
     if ( project->get_skip_transformation() == false )
        {
       // list<SgNode*> forLoopList = NodeQuery::querySubTree (project,V_SgForStatement);
       // for (list<SgNode*>::iterator i = forLoopList.begin(); i != forLoopList.end(); i++)
          NodeQuerySynthesizedAttributeType forLoopList = NodeQuery::querySubTree (project,V_SgForStatement);
          for (NodeQuerySynthesizedAttributeType::iterator i = forLoopList.begin(); i != forLoopList.end(); i++)
             {
            // Add a comment here
               MiddleLevelRewrite::ScopeIdentifierEnum scope = 
                    MidLevelCollectionTypedefs::SurroundingScope;
               MiddleLevelRewrite::PlacementPositionEnum locationInScope = 
                    MidLevelCollectionTypedefs::BeforeCurrentPosition;

               ROSE_ASSERT ( (*i) != NULL);
               SgForStatement* forStatement = isSgForStatement(*i);
               ROSE_ASSERT (forStatement != NULL);

// DQ (7/12/2004): Inserting a comment currently requires insertion the comment with a valid statement
#define INSERT_COMMENT 0
#if INSERT_COMMENT
               MiddleLevelRewrite::insert(forStatement,"// important for loop?",scope,locationInScope);
#else
               MiddleLevelRewrite::insert(forStatement,"// important for loop? \nint z;",scope,locationInScope);
#endif
             }
        }
       else
        {
          printf ("project->get_skip_transformation() == true \n");
        }
     

  // Call the ROSE backend (unparse to generate transformed 
  // source code and compile it with vendor compiler).
     return backend(project);
   }


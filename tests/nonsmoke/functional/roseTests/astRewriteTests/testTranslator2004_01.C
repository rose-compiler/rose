// This bug was also reported by Brian Miller, it is now fixed.
// The fix requires a change to the code below.  The member function
// has been modified so that the relative insert can also include
// the insertion of code into the target statement and not just
// into the region surrounding it (what was all that was previously 
// possible):
//        static void MiddleLevelRewrite::insert (
//                           SgStatement* target,
//                           const string & transformationString,
//                           ScopeIdentifierEnum inputRelativeScope = SurroundingScope,
//                           PlacementPositionEnum locationInScope = BeforeCurrentPosition );
// What was previously called "LocalScope" is not called "SurroundingScope"
// and a new scope enum value has been introduced called "StatementScope"
// which only make sense when the target statement is a SgScopeStatement.
//
// test input:
// int i;

///////////////////////////////////////////////////////////////////////////////
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE

#include "rose.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

class MyTraversal
   : public SgSimpleProcessing
   {
     public:
          void visit(SgNode* astNode);
   };

void
MyTraversal::visit ( SgNode* astNode )
   {
     SgGlobal *sg=isSgGlobal(astNode);

  // Make this test code harder and include only a CPP directive (or any comment)
  // string fixme="\n int ignore_me;";
     string fixme="\n #include<rose.h>";
     if (sg!=NULL)
        {
          MiddleLevelRewrite::insert(sg,fixme,
               MidLevelCollectionTypedefs::StatementScope,
               MidLevelCollectionTypedefs::TopOfCurrentScope);
        }
   }

int
main( int argc, char * argv[] )
   {
     SgProject* project= frontend (argc,argv);
     MyTraversal myTraversal;
     myTraversal.traverseInputFiles (project, postorder);
     project->unparse();
     return 0;
   }
/// end rose.C
//////////////////////////////


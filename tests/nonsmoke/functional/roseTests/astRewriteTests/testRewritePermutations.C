// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

using namespace std;
using namespace Rose;

#include "testNewRewrite.h"

void transformation ( SgProject* project )
   {
  // cast away const because the MyInheritedAttributeType constructor takes a pointer
  // SgProject & sageProject = (SgProject &) project;

  // printf ("Project = %s \n",project->unparseToString().c_str());

  // Build the inherited attribute
  // MyInheritedAttributeType inheritedAttribute (&sageProject);
     MyInheritedAttributeType inheritedAttribute;

  // Initialize the project pointer so that it can be passed to the nested tree traversal
  // inheritedAttribute.projectPointer = &sageProject;

  // default setting for debugging AST rewrite mechanism
     inheritedAttribute.targetCodeString = "int x;";

     string newCodeTemplateString = "int y_$SCOPE_NAME_$POSITION_IN_SCOPE_NAME_$NEW_SCOPE_TEST;";

#if 1
  // Iterate over all possible scopes
     int i;
     for (i = HighLevelInterfaceNodeCollection::SurroundingScope;
          i < HighLevelInterfaceNodeCollection::LAST_SCOPE_TAG;
          i++)
        {
       // Iterate over all possible positions in a given scope
          int j;

       // DQ (10/17/2004): Debugging current issues resetict where we are testing
       // for (j = HighLevelInterfaceNodeCollection::TopOfScope;
          for (j = HighLevelInterfaceNodeCollection::BeforeCurrentPosition;
               j < HighLevelInterfaceNodeCollection::LAST_PLACEMENT_TAG;
               j++)
             {
            // Test the the use of strings inserted into newScopes and not inserted into new scopes
               int trueFalseIndex = 0;

            // The replacement is overly destructive and thus more difficult to test (use another
            // test code for this case).
               if (j == HighLevelInterfaceNodeCollection::ReplaceCurrentPosition)
                    continue;

            // Set the bound to 1 for only false case (bound should be set to 2 to test false and true cases).
               for (trueFalseIndex = 0; trueFalseIndex < 1; trueFalseIndex++)
                  {
                 // First insert the new string
                    inheritedAttribute.removeTargetCode = false;
                    inheritedAttribute.scope            = HighLevelInterfaceNodeCollection::ScopeIdentifierEnum(i);
                    inheritedAttribute.location         = HighLevelInterfaceNodeCollection::PlacementPositionEnum(j);

                 // inheritedAttribute.newCodeString = "/* comment only */";
                    inheritedAttribute.newCodeString = newCodeTemplateString;
                    printf ("inheritedAttribute.newCodeString = -->%s<-- \n",inheritedAttribute.newCodeString.c_str());

                 // Control the creation of strings in a new scope or not
                    inheritedAttribute.buildInNewScope = (trueFalseIndex != 0);

                    testPermutation( project, inheritedAttribute );

                    string projectString = project->unparseToString();
                    string newCodeString = inheritedAttribute.newCodeString;

#if 1
                    printf ("################################################################### \n");
                    printf ("newCodeString = -->%s<-- \n",newCodeString.c_str());
                    printf ("project->unparseToString() = \n%s\n",projectString.c_str());
                    printf ("################################################################### \n");
#endif

                    bool insertionVerified = StringUtility::isContainedIn(projectString,newCodeString);

                 // Transformations inserted into the preamble don't appear in the final AST 
                 // (only in the intermediate file generated to compile the transformation 
                 // strings and generate the AST fragements that are edited into place in 
                 // the application's AST.
                    ROSE_ASSERT ((insertionVerified == true) || (i == HighLevelInterfaceNodeCollection::Preamble));

                 // printf ("\n\n At base of transformation() \n\n\n");
                  }
             }
        }

#if 0
  // Testing the existing of strings after all strings have been added should not 
  // be required.  Each new addition of a string should have an associated test 
  // for it's existand (so put the test into the base of the for loop above!).

  // Iterate over all possible scopes
     for (i = HighLevelInterfaceNodeCollection::SurroundingScope;
          i < HighLevelInterfaceNodeCollection::LAST_SCOPE_TAG;
          i++)
        {
       // Iterate over all possible positions in a given scope
          int j;
          for (j = HighLevelInterfaceNodeCollection::TopOfScope;
               j < HighLevelInterfaceNodeCollection::LAST_PLACEMENT_TAG;
               j++)
             {
            // Test the the use of strings inserted into newScopes and not inserted into new scopes
               int trueFalseIndex = 0;

            // The replacement is overly destructive and thus more difficult to test (use another
            // test code for this case).
               if (j == HighLevelInterfaceNodeCollection::ReplaceCurrentPosition)
                    continue;

            // Set the bound to 1 for only false case (bound should be set to 2 to test false and
            // true cases).
               for (trueFalseIndex = 0; trueFalseIndex < 1; trueFalseIndex++)
                  {
                 // First insert the new string
                    printf ("inheritedAttribute.newCodeString = -->%s<-- \n",inheritedAttribute.newCodeString.c_str());

                 // Control the creation of strings in a new scope or not
                    inheritedAttribute.buildInNewScope = (trueFalseIndex != 0);

                    testPermutation( project, inheritedAttribute );

                    string projectString = project->unparseToString().c_str();
                    string newCodeString = inheritedAttribute.newCodeString;

                    printf ("################################################################### \n");
                    printf ("newCodeString = -->%s<-- \n",newCodeString.c_str());
                    printf ("project->unparseToString() = \n%s\n",projectString.c_str());
                    printf ("################################################################### \n");

                    bool insertionVerified = StringUtility::isContainedIn(projectString,newCodeString);
                    ROSE_ASSERT (insertionVerified == true);
                    printf ("\n\n At base of transformation() \n\n\n");
                  }
             }
        }
#endif

#else
  // Special test of any problems

  // First insert the new string
     inheritedAttribute.removeTargetCode = false;

  // inheritedAttribute.scope    = HighLevelInterfaceNodeCollection::ParentScope;
  // inheritedAttribute.scope    = HighLevelInterfaceNodeCollection::SurroundingScope;
  // inheritedAttribute.location = HighLevelInterfaceNodeCollection::TopOfScope;
  // inheritedAttribute.location = HighLevelInterfaceNodeCollection::BottomOfScope;
  // inheritedAttribute.scope    = HighLevelInterfaceNodeCollection::FileScope;
  // inheritedAttribute.location = HighLevelInterfaceNodeCollection::TopOfScope;
  // inheritedAttribute.scope    = HighLevelInterfaceNodeCollection::NestedLoopScope;
  // inheritedAttribute.location = HighLevelInterfaceNodeCollection::BeforeCurrentPosition;

     inheritedAttribute.scope    = HighLevelInterfaceNodeCollection::SurroundingScope;
     inheritedAttribute.location = HighLevelInterfaceNodeCollection::BeforeCurrentPosition;
//   inheritedAttribute.location = HighLevelInterfaceNodeCollection::AfterCurrentPosition;

  // Control the creation of strings in a new scope or not
     inheritedAttribute.buildInNewScope = false;

     inheritedAttribute.newCodeString = "/* comment only (before) */";

     testPermutation( project, inheritedAttribute );
#endif
   }
     

int
main( int argc, char * argv[] )
   {
  // This test code tests the AST rewrite mechanism using an arbitrary targetCodeString string and
  // an arbitrary newCodeString string.  The targetCodeString is searched for in the AST (the
  // recognition is dependent upon the unparsed AST matching the target string, so this would be
  // problematic if this were not a test code).  The new code (newCodeString) is then inserted into
  // the AST at the relative position specified (scope and position in scope). All possible
  // positions are tested by this test code.

  // SgProject project(argc,argv);
     SgProject* project = frontend(argc,argv);

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
     generatePDF(*project);

  // Output the source code file (as represented by the SAGE AST) as a DOT file (with bookmarks)
     generateDOT(*project);

  // Allow compiler options to influence if we operate on the AST
     if ( project->get_skip_transformation() == false )
        {
          printf ("Calling transformation \n");
          transformation ( project );
        }
       else
        {
          printf ("project->get_skip_transformation() == true \n");
        }
     

  // Call the ROSE backend (unparse to generate transformed 
  // source code and compile it with vendor compiler).
     return backend(project);
   }

















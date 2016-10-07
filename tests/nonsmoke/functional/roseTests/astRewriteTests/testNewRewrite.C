// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

// Extra header for customizing the rewrite mechanism
#include "rewriteTemplateImpl.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace rose;

#include "testNewRewrite.h"

MyInheritedAttributeType::MyInheritedAttributeType ()
   {
  // default values
     scope            = HighLevelInterfaceNodeCollection::unknownScope;
     location         = HighLevelInterfaceNodeCollection::unknownPositionInScope;
     removeTargetCode = false;
   }

MySynthesizedAttributeType::MySynthesizedAttributeType ()
   {
  // Nothing to do here
   }

MyTraversal::~MyTraversal ()
   {
  // Nothing to do here
   }

MyTraversal::MyTraversal ()
   {
  // Nothing to do here
   }

MyInheritedAttributeType 
MyTraversal::evaluateRewriteInheritedAttribute (
   SgNode* astNode,
   MyInheritedAttributeType inheritedAttribute )
   {
     return inheritedAttribute;
   }

MySynthesizedAttributeType
MyTraversal::evaluateRewriteSynthesizedAttribute (
   SgNode* astNode,
   MyInheritedAttributeType inheritedAttribute,
   SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     MySynthesizedAttributeType returnAttribute;

  // printf ("In MyTraversal::evaluateRewriteSynthesizedAttribute (%s) \n",astNode->sage_class_name());

     if (astNode->variantT() == V_SgVariableDeclaration)
        {
          SgStatement* statement = isSgStatement(astNode);
          ROSE_ASSERT (statement != NULL);

       // Look for a target statement ("int x;") so that we avoid adding more than one string
          string currentDeclarationStatement = astNode->unparseToString();
       // printf ("Found a declaration: currentDeclarationStatement = %s \n",currentDeclarationStatement.c_str());
          if (currentDeclarationStatement == inheritedAttribute.targetCodeString)
             {
            // printf ("Matching current statement = %p = %s (parent = %p) \n",astNode,currentDeclarationStatement.c_str(),astNode->get_parent());

               string newSourceString = inheritedAttribute.newCodeString;
            // printf ("Found a matching target statement (insert newSourceString = %s) \n",newSourceString.c_str());

#if 0
               printf ("Exiting before call to insert ... \n");
               ROSE_ABORT();
#endif

               ROSE_ASSERT (inheritedAttribute.removeTargetCode == false);

#if 0
            // Test insert and replace functions with code string
               if (inheritedAttribute.location == HighLevelInterfaceNodeCollection::ReplaceCurrentPosition)
                  {
                 // printf ("Calling replace! \n");
                    returnAttribute.replace (statement,newSourceString,inheritedAttribute.scope);
                  }
                 else
                  {
                    returnAttribute.insert (
                                    statement,
                                    newSourceString,
                                    inheritedAttribute.scope,
                                    inheritedAttribute.location);
                  }
#else
            // Test insert and replace functions with comment strings and code string

            // This case currently fails to pass one of the tests!!!

#if 0
            // This fails because the verification phase triedto find the input string 
            // in the unparsed output, but the unparse to string skips comments so the 
            // test fails.
            // Insert a comment before the current statement within the specified scope
               returnAttribute.insert (
                                    statement,
                                    "/* comment before */",
                                    inheritedAttribute.scope,
                                    HighLevelInterfaceNodeCollection::BeforeCurrentPosition);
#endif

#if 0
            // Insert an include directive before the current statement within the specified scope
            // This fails because it seems that the assert.h header file can not be included in all scopes
               returnAttribute.insert (
                                    statement,
                                    "#include<assert.h>",
                                    inheritedAttribute.scope,
                                    HighLevelInterfaceNodeCollection::BeforeCurrentPosition);
#endif

#if 1
  #if 1
            // Test the replace and insert functions
               if (inheritedAttribute.location == HighLevelInterfaceNodeCollection::ReplaceCurrentPosition)
                  {
                 // printf ("Calling replace! \n");
                    returnAttribute.replace (statement,newSourceString,inheritedAttribute.scope);
                  }
                 else
                  {
                    returnAttribute.insert (
                                    statement,
                                    newSourceString,
                                    inheritedAttribute.scope,
                                    inheritedAttribute.location);
                  }
  #else
            // Test the insert function even for case of (location == ReplaceCurrentPosition)
               returnAttribute.insert (
                                    statement,
                                    newSourceString,
                                    inheritedAttribute.scope,
                                    inheritedAttribute.location);
  #endif
#endif
#if 0
            // Insert a comment after the current statement within the specified scope
               returnAttribute.insert (
                                    statement,
                                    "/* comment after */",
                                    inheritedAttribute.scope,
                                    HighLevelInterfaceNodeCollection::AfterCurrentPosition);
#endif
#endif

            // returnAttribute.display("display called after addTransformationString() \n");
             }
        }

     return returnAttribute;
   }

void
testPermutation( 
   SgProject* project, 
   MyInheritedAttributeType & inheritedAttribute )
   {
     HighLevelInterfaceNodeCollection::ScopeIdentifierEnum   scope           = inheritedAttribute.scope;
     HighLevelInterfaceNodeCollection::PlacementPositionEnum positionInScope = inheritedAttribute.location;

     bool buildInNewScope = inheritedAttribute.buildInNewScope;

#if 1
     printf ("In testPermutation(): scope = %s location = %s buildInNewScope = %s \n",
          HighLevelInterfaceNodeCollection::getRelativeScopeString(HighLevelRewrite::ScopeIdentifierEnum(scope)).c_str(),
          HighLevelInterfaceNodeCollection::getRelativeLocationString(HighLevelInterfaceNodeCollection::PlacementPositionEnum(positionInScope)).c_str(),
          (buildInNewScope == true) ? "true" : "false");
#endif

     ROSE_ASSERT (scope           != HighLevelInterfaceNodeCollection::unknownScope);
     ROSE_ASSERT (positionInScope != HighLevelInterfaceNodeCollection::unknownPositionInScope);

#if 0
  // The replacement is overly destructive and thus more difficult to test (use another
  // test code for this case).
     if (positionInScope == HighLevelInterfaceNodeCollection::ReplaceCurrentPosition)
          return;
#endif

  // Template used to construct new strings based on their scope and location in scope
  // string newCodeTemplateString = "/* int y_$SCOPE_NAME_$POSITION_IN_SCOPE_NAME_$NEW_SCOPE_TEST; */";
     string newCodeTemplateString = inheritedAttribute.newCodeString;

  // Build and edit the new code string (if the new code string contains $SCOPE_NAME, 
  // $POSITION_IN_SCOPE_NAME, and $NEW_SCOPE_TEST string substitution variables.
     string tempString = newCodeTemplateString;
     tempString = StringUtility::copyEdit (
                       tempString,"$SCOPE_NAME",
                       HighLevelInterfaceNodeCollection::getRelativeScopeString(HighLevelInterfaceNodeCollection::ScopeIdentifierEnum(scope)));
     tempString = StringUtility::copyEdit (
                       tempString,"$POSITION_IN_SCOPE_NAME",
                       HighLevelInterfaceNodeCollection::getRelativeLocationString(HighLevelInterfaceNodeCollection::PlacementPositionEnum(positionInScope)));
     tempString = StringUtility::copyEdit (
                       tempString,"$NEW_SCOPE_TEST",
                       (buildInNewScope == true) ? "BuildInNewScope_true" : "BuildInNewScope_false");

  // Set the new source code string
     inheritedAttribute.newCodeString = tempString;

//   ROSE_ASSERT (inheritedAttribute.projectPointer != NULL);

  // The traversal uses the AST rewrite mechanism which requires the SgProject object to retrive the
  // command line for compilation of the intermeditate files (from strings to AST fragments) before
  // patching them into the application's AST.
//   MyTraversal myTraversal(*(inheritedAttribute.projectPointer));
     MyTraversal myTraversal;

  // Error checking
  // printf ("inheritedAttribute.newCodeString = %s \n",inheritedAttribute.newCodeString.c_str());

  // Clear the flags after modifying the tree processing the tree
  // AstClearVisitFlags cleanTreeFlags;

  // printf ("In testRewriteReplacementPermutations (test code): clear internal visit flags \n");
  // cleanTreeFlags.traverse(project);

     myTraversal.traverseInputFiles(project,inheritedAttribute);

  // printf ("In testRewriteReplacementPermutations (test code): clear internal visit flags \n");
  // cleanTreeFlags.traverse(project);

#if 0
     printf ("Exiting after traversal ... \n");
     ROSE_ABORT();
#endif
   }












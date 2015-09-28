
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "nameQuery.h"
#include "rewrite.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void
HighLevelRewrite::generatePrefixAndSuffix ( 
     SgNode* astNode,
     string & globalPrefixString, 
     string & localPrefixString, 
     string & suffixString,
     bool generateIncludeDirectives,
     bool prefixIncludesCurrentStatement )
   {
  // Definition of the generatePrefixAndSuffix() function in the HighLevelRewrite differs 
  // slightly from the definition in the MidLevelInterface. Mostly it adds a ";" to make the current
  // function a prototype.

     if (isSgStatement(astNode) != NULL)
        {
#if 0
          printf ("\n\n*************************************************************\n");
          printf ("In HighLevelRewrite::generatePrefixAndSuffixStrings: astNode is a %s \n",astNode->sage_class_name());
          printf ("     astNode = %s \n",astNode->unparseToString().c_str());
#endif

       // DQ (7/25/2005): Removed use of AstClearVisitFlags
       // Part of a temporary fix to explicitly clear all the flags
       // AstClearVisitFlags cleanTreeFlags;

          ROSE_ASSERT (astNode != NULL);
          SgProject* project = TransformationSupport::getProject(astNode);
          ROSE_ASSERT (project != NULL);

       // printf ("Temporary Fix: Clearing internal visit flags in HighLevelRewrite::generatePrefixAndSuffix() \n");
       // cleanTreeFlags.traverse(project);

          PrefixInheritedAttribute inheritedAttribute;
          PrefixSuffixGenerationTraversal traversal (generateIncludeDirectives);

       // Record the current statment so that we can determine if the previous 
       // statement (if it is sought) is in the same scope or not.
          SgStatement* currentStatement = TransformationSupport::getStatement(astNode);
          ROSE_ASSERT (currentStatement != NULL);
          traversal.previousScope = currentStatement->get_scope();
          ROSE_ASSERT (traversal.previousScope != NULL);

          SgNode* startingStatementForPrefixGeneration = astNode;
          if (prefixIncludesCurrentStatement == false) 
             {
               SgStatement* currentStatement = TransformationSupport::getStatement(astNode);
               ROSE_ASSERT (currentStatement != NULL);
               startingStatementForPrefixGeneration = rose::getPreviousStatement(currentStatement);
               ROSE_ASSERT (startingStatementForPrefixGeneration != NULL);
             }

          traversal.traverse(startingStatementForPrefixGeneration,inheritedAttribute);

       // printf ("Temporary Fix: Clearing internal visit flags in HighLevelRewrite::generatePrefixAndSuffix() \n");
       // cleanTreeFlags.traverse(project);

          globalPrefixString += traversal.generatePrefixStringGlobalDeclarations() + ";";

          localPrefixString  += traversal.generatePrefixStringLocalDeclarations();

          suffixString = traversal.generateSuffixString();

#if 0
          printf ("Leaving HighLevelRewrite::generatePrefixAndSuffixStrings: astNode is a %s \n",astNode->sage_class_name());
          printf ("************************************************************* \n\n");
#endif
        }
       else
        {
          printf ("ERROR: generatePrefixAndSuffixStrings() only works on SgStatements \n");
          ROSE_ABORT();
        }

#if 0
     printf ("Exiting in HighLevelRewrite<ASTNodeCollection>::generatePrefixAndSuffix() \n");
     ROSE_ABORT();
#endif
   }

void
HighLevelRewrite::remove  ( SgStatement* target )
   {
     MiddleLevelRewrite::remove(target);
   }














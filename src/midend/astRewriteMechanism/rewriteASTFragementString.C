
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// ************************************************************
//               Source Code String Class
// ************************************************************

// template <class T>
template<>
void
TransformationStringTemplatedType<HighLevelCollectionTypedefs>::preconditions(
   ScopeIdentifierEnum   & relativeScope,
   PlacementPositionEnum & relativeLocation,
   bool                  & inputBuildInNewScope )
   {
  // Note that we automatically change some of the input specifications to permit the 
  // simplified automatic testing of all the permutations associated with new strings 
  // added to all locations in a scope and for all scopes.

  // Disallow use of anything but TopOfScope if associated with Preamble (scope)
     if (relativeScope == HighLevelInterfaceNodeCollection::Preamble)
        {
       // If Preamble (scope) then use PreamblePositionInScope (used to be TopOfScope)
          if (relativeLocation != HighLevelInterfaceNodeCollection::PreamblePositionInScope)
             {
            // printf ("Warning: Use of Preamble requires specification of PreamblePositionInScope (reset to PreamblePositionInScope) \n");
            // Change the position of scope to PreamblePositionInScope in this case
               relativeLocation = HighLevelInterfaceNodeCollection::PreamblePositionInScope;
             }
        }

  // Disallow use of TopOfIncludeRegion and BottomOfIncludeRegion if not associated with GlobalScope or FileScope
     if ( (relativeScope != HighLevelInterfaceNodeCollection::GlobalScope) && 
          (relativeScope != HighLevelInterfaceNodeCollection::FileScope) )
        {
       // If GlobalScope is not specified then test for use of TopOfIncludeRegion or BottomOfIncludeRegion
          if ( (relativeLocation == HighLevelInterfaceNodeCollection::TopOfIncludeRegion) || 
               (relativeLocation == HighLevelInterfaceNodeCollection::BottomOfIncludeRegion) )
             {
            // printf ("Warning: Use of (TopOfIncludeRegion || BottomOfIncludeRegion) not permitted for global or file scopes (reset to TopOfScope) \n");
            // Change the position and scope to GlobalScope & TopOfScope in this case
               relativeScope    = HighLevelInterfaceNodeCollection::GlobalScope;
//             relativeLocation = HighLevelInterfaceNodeCollection::TopOfScope;

               printf ("WARNING: case of relativeScope set to GlobalScope or FileScope relativeLocation set to TopOfIncludeRegion or BottomOfIncludeRegion \n");
//             ROSE_ABORT();
             }
        }

  // Disallow use of (buildInNewScope == true) in global or file scopes
     if ( (relativeScope == HighLevelInterfaceNodeCollection::GlobalScope) || 
          (relativeScope == HighLevelInterfaceNodeCollection::FileScope) )
        {
       // If GlobalScope or FileScope is specified then warn about (buildInNewScope == true) which makes no sense.
          if (buildInNewScope == true)
             {
            // printf ("Warning: Use of (buildInNewScope == true) not permitted for global or file scopes (reset to false) \n");
            // Change value of buildInNewScope to false
               buildInNewScope = false;

//             printf ("Exiting in case of relativeScope set to GlobalScope or FileScope and buildInNewScope == true \n");
//             ROSE_ABORT();
             }
        }

#if 0
  // DQ (1/15/2003): Fix for Brian's Bug (added concept of statement scope
  // Disallow use of anything but TopOfScope or BottomOfScope if associated with StatementScope
     if (relativeScope == HighLevelInterfaceNodeCollection::StatementScope)
        {
           printf ("It is an error to use the StatementScope within the HighLevelInterface \n");
           ROSE_ASSERT (1 == 2);

#if 0
       // If StatementScope then check position in scope (default to: TopOfScope)
          if ( !((relativeLocation == HighLevelInterfaceNodeCollection::TopOfScope) ||
                 (relativeLocation == HighLevelInterfaceNodeCollection::BottomOfScope)
             {
               printf ("Warning: Use of StatementScope requires specification of TopOfScope or BottomOfScope (reset to TopOfScope) \n");
            // Change the position of scope to PreamblePositionInScope in this case
               relativeLocation = HighLevelInterfaceNodeCollection::TopOfScope;
             }
#endif
        }
#endif
   }


template<>
void
TransformationStringTemplatedType<MidLevelCollectionTypedefs>::preconditions(
   ScopeIdentifierEnum   & relativeScope,
   PlacementPositionEnum & relativeLocation,
   bool                  & inputBuildInNewScope )
   {
  // Note that we automatically change some of the input specifications to permit the 
  // simplified automatic testing of all the permutations associated with new strings 
  // added to all locations in a scope and for all scopes.

  // Disallow use of anything but TopOfScope if associated with Preamble (scope)
     if (relativeScope == MidLevelInterfaceNodeCollection::Preamble)
        {
       // If Preamble (scope) then use PreamblePositionInScope (used to be TopOfScope)
          if (relativeLocation != MidLevelInterfaceNodeCollection::PreamblePositionInScope)
             {
            // printf ("Warning: Use of Preamble requires specification of PreamblePositionInScope (reset to PreamblePositionInScope) \n");
            // Change the position of scope to PreamblePositionInScope in this case
               relativeLocation = MidLevelInterfaceNodeCollection::PreamblePositionInScope;
             }
        }

  // DQ (1/15/2003): Fix for Brian's Bug (added concept of statement scope
  // Disallow use of anything but TopOfScope or BottomOfScope if associated with StatementScope
     if (relativeScope == MidLevelInterfaceNodeCollection::StatementScope)
        {
       // StatementScope forces resetting the target to either before the 
       // first statment or after the last statement.
          if ( !( (relativeLocation == MidLevelInterfaceNodeCollection::BeforeCurrentPosition) ||
                  (relativeLocation == MidLevelInterfaceNodeCollection::AfterCurrentPosition) ) )
             {
               printf ("Error: Use of StatementScope requires specification of BeforeCurrentPosition or AfterCurrentPosition \n");
               ROSE_ASSERT (1 == 2);

            // Change the position of scope to PreamblePositionInScope in this case
               relativeLocation = MidLevelInterfaceNodeCollection::TopOfCurrentScope;
             }
        }
   }

template<>
TransformationStringTemplatedType<MidLevelCollectionTypedefs>::
TransformationStringTemplatedType(
   SgNode* astNode,
   const string & X,
   ScopeIdentifierEnum inputRelativeScope,
   PlacementPositionEnum inputRelativeLocation,
   bool inputBuildInNewScope )
   {
  // This constructor uses default arguments
     relativeScope            = inputRelativeScope;
     relativeLocation         = inputRelativeLocation;

  // This pointer will be reset before existing this function
     associatedASTNode        = NULL;
     buildInNewScope          = inputBuildInNewScope;
     sourceCode               = X;

  // DQ (1/15/2004): Uncommented call to preconditions()
  // Enforce simple constraints on the values of the input variables
     preconditions(relativeScope,relativeLocation,buildInNewScope);

#if 0
     printf ("At base of TransformationStringTemplatedType constructor: relativeScope    = %s \n",getRelativeScopeString().c_str());
     printf ("At base of TransformationStringTemplatedType constructor: relativeLocation = %s \n",getRelativeLocationString().c_str());
#endif
   }

#if 0
// DQ (8/19/2004): Moved to ROSE/src/util/stringSupport/string_functions.C
// DQ: 09/23/03
// We require a global function for getting the string associated 
// with the definition of a variant (which is a global enum).
string getVariantName ( VariantT v )
   {
     ROSE_ASSERT( int(v) < int(V_SgNumVariants));

  // This code is far simpler (if the function is static)

  // DQ (4/8/2004): Modified code to use new global list of sage 
  // class names (simpler this way)
  // return string(SgTreeTraversal<int,int>::get_variantName(v));
     extern const char* roseGlobalVariantNameList[];
     return string(roseGlobalVariantNameList[v]);
   }
#endif

template<>
SgScopeStatement*
TransformationStringTemplatedType<HighLevelCollectionTypedefs>::getScope (
// SgStatement* inputStatement, 
   SgNode* astNode,
   ScopeIdentifierEnum relativeScope )
   {
     ROSE_ASSERT (astNode != NULL);

  // printf ("Inside of TransformationStringTemplatedType<HighLevelCollectionTypedefs>::getScope() \n");

  // Find the statement in which the astNode is in node of it's subtree
     SgStatement* inputStatement = TransformationSupport::getStatement(astNode);
     ROSE_ASSERT (inputStatement != NULL);

     SgStatement* parentStatement = inputStatement;
     SgScopeStatement* targetScope = NULL;
     switch (relativeScope)
        {
       // Consider global scope, file scope, and the Preample to be the same for now
          case HighLevelCollectionTypedefs::Preamble:
          case HighLevelCollectionTypedefs::FileScope:
          case HighLevelCollectionTypedefs::GlobalScope:
             {
//             printf ("In case HighLevelCollectionTypedefs::GlobalScope \n");

            // scopeFunctionPointer = &AST_Rewrite::InheritedAttribute::getGlobalScope;
            // targetScope = isSgScopeStatement(findEnclosingStatement(astNode,V_SgGlobal));
               while ( (parentStatement->get_parent() != NULL) && (isSgGlobal(parentStatement) == NULL) )
                  {
//                  printf ("parentStatement->sage_class_name() = %s \n",parentStatement->sage_class_name());
                    parentStatement = isSgStatement(parentStatement->get_parent());
                    ROSE_ASSERT (parentStatement != NULL);
                  }

               targetScope = isSgScopeStatement(parentStatement);
               ROSE_ASSERT (targetScope != NULL);
               break;
             }

          case HighLevelCollectionTypedefs::SurroundingScope:
             {
            // scopeFunctionPointer = &AST_Rewrite::InheritedAttribute::getLocalScope;

//             printf ("In case HighLevelCollectionTypedefs::LocalScope \n");

            // Avoid getting back the current astNode in the case where it might be a scope (e.g. SgForStatement)
            // targetScope = isSgScopeStatement(findEnclosingStatement(astNode->get_parent(),V_SgScopeStatement));
               while ( (parentStatement->get_parent() != NULL) && (isSgScopeStatement(parentStatement) == NULL) )
                  {
//                  printf ("parentStatement->sage_class_name() = %s \n",parentStatement->sage_class_name());
                    parentStatement = isSgStatement(parentStatement->get_parent());
                    ROSE_ASSERT (parentStatement != NULL);
                  }

               targetScope = isSgScopeStatement(parentStatement);
               ROSE_ASSERT (targetScope != NULL);
               break;
             }

          case HighLevelCollectionTypedefs::ParentScope:
             {
            // Find the local scope of the current statement (often a basic block)
               parentStatement = isSgStatement(parentStatement->get_parent());
               ROSE_ASSERT (parentStatement != NULL);
            // printf ("Before loop 1: parentStatement->sage_class_name() = %s \n",parentStatement->sage_class_name());
            // Find the next outer scope (often a for loop)
               parentStatement = isSgStatement(parentStatement->get_parent());
               ROSE_ASSERT (parentStatement != NULL);
            // printf ("Before loop 2: parentStatement->sage_class_name() = %s \n",parentStatement->sage_class_name());
               while ( (parentStatement->get_parent() != NULL) && 
                       ( (isSgScopeStatement(parentStatement) == NULL) ||
                         ( (isSgForStatement(parentStatement) != NULL) ||
                           (isSgWhileStmt(parentStatement) != NULL)    ||
                           (isSgDoWhileStmt(parentStatement) != NULL) ) ) )
                  {
                 // printf ("parentStatement->sage_class_name() = %s \n",parentStatement->sage_class_name());
                    parentStatement = isSgStatement(parentStatement->get_parent());
                    ROSE_ASSERT (parentStatement != NULL);
                 // printf ("Next: parentStatement->sage_class_name() = %s \n",parentStatement->sage_class_name());
                  }

               targetScope = isSgScopeStatement(parentStatement);
               ROSE_ASSERT (targetScope != NULL);

            // printf ("case ParentScope: targetScope->sage_class_name() = %s \n",targetScope->sage_class_name());

               break;
             }

          case HighLevelCollectionTypedefs::NestedLoopScope:
             {
            // Find the for statment
            // targetScope = isSgScopeStatement(findEnclosingStatement(astNode->get_parent(),V_SgForStatement));
            // Then get the scope containing the for statement
            // targetScope = isSgScopeStatement(findEnclosingStatement(targetScope->get_parent(),V_SgScopeStatement));

//             printf ("Sorry, NestedLoopScope case not implemented \n");
//             ROSE_ABORT();

            // Once to get the scope of the current statement (basic block)
               parentStatement = isSgStatement(parentStatement->get_parent());
            // Again to get the loop structure (for loop)
               if ( (isSgForStatement(parentStatement->get_parent()) != NULL) ||
                    (isSgWhileStmt(parentStatement->get_parent())    != NULL) ||
                    (isSgDoWhileStmt(parentStatement->get_parent())  != NULL) )
                    parentStatement = isSgStatement(parentStatement->get_parent());

               ROSE_ASSERT (parentStatement != NULL);

            // Loop other any nested for, while, or dowhile loops
               while ( (parentStatement->get_parent() != NULL) && 
                       ( (isSgScopeStatement(parentStatement) == NULL) ||
                         ( (isSgForStatement(parentStatement) != NULL) ||
                           (isSgWhileStmt(parentStatement) != NULL)    ||
                           (isSgDoWhileStmt(parentStatement) != NULL) ) ) )
                  {
//                  printf ("parentStatement->sage_class_name() = %s \n",parentStatement->sage_class_name());
                    parentStatement = isSgStatement(parentStatement->get_parent());
                    ROSE_ASSERT (parentStatement != NULL);
                 // If the number of statements in the scope is 1 then look for another nesting of for loop
                    SgBasicBlock* basicBlock = isSgBasicBlock(parentStatement);
                    if ( (basicBlock != NULL) && (basicBlock->get_statements().size() == 1) )
                       {
                      // look for the next outer level of the nested loop
                         parentStatement = isSgStatement(parentStatement->get_parent());
                       }
                  }

               targetScope = isSgScopeStatement(parentStatement);
               ROSE_ASSERT (targetScope != NULL);
               break;
             }

          case HighLevelCollectionTypedefs::NestedConditionalScope:
             {
            // Find the conditional statment (e.g. if)
            // targetScope = isSgScopeStatement(findEnclosingStatement(astNode->get_parent(),V_SgIfStmt));
            // Then get the scope containing the for statement
            // targetScope = isSgScopeStatement(findEnclosingStatement(targetScope->get_parent(),V_SgScopeStatement));

//             printf ("Sorry, NestedConditionalScope case not implemented \n");
//             ROSE_ABORT();

            // Once to get the scope of the current statement (basic block)
               parentStatement = isSgStatement(parentStatement->get_parent());
            // Again to get the loop structure (if conditional)
               if (isSgIfStmt(parentStatement->get_parent()) != NULL)
                    parentStatement = isSgStatement(parentStatement->get_parent());

               ROSE_ASSERT (parentStatement != NULL);

            // Loop other any nested conditional statements
               while ( (parentStatement->get_parent() != NULL) && 
                       ( (isSgScopeStatement(parentStatement) == NULL) ||
                         (isSgIfStmt(parentStatement) != NULL) ) )
                  {
//                  printf ("parentStatement->sage_class_name() = %s \n",parentStatement->sage_class_name());
                    parentStatement = isSgStatement(parentStatement->get_parent());
                    ROSE_ASSERT (parentStatement != NULL);
                 // If the number of statements in the scope is 1 then look for another nesting of for loop
                    SgBasicBlock* basicBlock = isSgBasicBlock(parentStatement);
                    if ( (basicBlock != NULL) && (basicBlock->get_statements().size() == 1) )
                       {
                      // look for the next outer level of the nested loop
                         parentStatement = isSgStatement(parentStatement->get_parent());
                       }
                  }

               targetScope = isSgScopeStatement(parentStatement);
               ROSE_ASSERT (targetScope != NULL);
               break;
             }

          case HighLevelCollectionTypedefs::FunctionScope:
             {
//             printf ("Sorry, FunctionScope case not implemented \n");
//             ROSE_ABORT();

               while ( (parentStatement->get_parent() != NULL) && (isSgFunctionDefinition(parentStatement) == NULL) )
                  {
//                  printf ("parentStatement->sage_class_name() = %s \n",parentStatement->sage_class_name());
                    parentStatement = isSgStatement(parentStatement->get_parent());
                    ROSE_ASSERT (parentStatement != NULL);
                  }

               targetScope = isSgScopeStatement(parentStatement);
               ROSE_ASSERT (targetScope != NULL);

               SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(targetScope);
               ROSE_ASSERT(functionDefinition != NULL);

            // Then get the basic block associated with the function definition
               targetScope = functionDefinition->get_body();

               break;
             }

          case HighLevelCollectionTypedefs::unknownScope:
             {
               printf ("ERROR: unknownScope case found is switch statement \n");
               ROSE_ABORT();
               break;
             }

          default:
             {
               printf ("default case found is switch statement (to set member function pointer) \n");
               ROSE_ABORT();
               break;
             }
        }

     ROSE_ASSERT (targetScope != NULL);
     return targetScope;
   }


template<>
SgStatement*
TransformationStringTemplatedType<HighLevelCollectionTypedefs>::getAssociatedStatement (
   SgNode* astNode,
   ScopeIdentifierEnum relativeScope,
   PlacementPositionEnum relativeLocation )
   {
// With each position in scope where an operation is appiled we store an associated node
// where the operation is properly defined, where the insertion is triggered); examples include:
//    1) BeforeCurrentPosition: the statement where it was specified
//    2) TopOfScope: the scope of any statment where it was specified (requested)
//    3) BottomOfIncludeRegion: the first function in the global scope (not implemented currently)

  // Find the statement in which the astNode is in node of it's subtree
     SgStatement* currentStatement = TransformationSupport::getStatement(astNode);
     ROSE_ASSERT (currentStatement != NULL);

#if 0
  // Debugging output
     printf ("Output the different scopes for currentStatement = %s \n",currentStatement->sage_class_name());
     printf ("     currentStatement = %s \n",currentStatement->unparseToString().c_str());
     for (int i = HighLevelCollectionTypedefs::LocalScope; i < HighLevelCollectionTypedefs::LAST_SCOPE_TAG; i++)
        {
          SgScopeStatement* tempScope = getScope(currentStatement,ScopeIdentifierEnum(i));
          printf ("   i = %s tempScope = %s at line %d \n",
               HighLevelCollectionTypedefs::getRelativeScopeString(ScopeIdentifierEnum(i)).c_str(),
               tempScope->sage_class_name(),
               rose::getLineNumber(tempScope));
          printf ("\n");
        }

     printf ("Exiting after output of all possible scopes: currentStatement = %s \n",
          currentStatement->sage_class_name(),rose::getLineNumber(currentStatement));
     printf ("     currentStatement->unparseToString() = \n%s\n",currentStatement->unparseToString().c_str());
     ROSE_ABORT();
#endif


     SgScopeStatement* scope = getScope(currentStatement,relativeScope);

#if 0
  // Debugging output
     printf ("After call to getScope(): relativeScope = %s scope = %s at line %d \n",
          HighLevelCollectionTypedefs::getRelativeScopeString(relativeScope).c_str(),
          scope->sage_class_name(),
          rose::getLineNumber(scope));
#endif

     SgStatement* associatedASTNode = NULL;
     switch (relativeLocation)
        {
       // Actions we want to occur when we traverse the scope containing the statements
          case HighLevelCollectionTypedefs::PreamblePositionInScope:
            // This should be the first declaration in the AST
               associatedASTNode = scope;
               ROSE_ASSERT (isSgScopeStatement(associatedASTNode) != NULL);
               break;

          case HighLevelCollectionTypedefs::TopOfScope:
          case HighLevelCollectionTypedefs::TopOfIncludeRegion:
          case HighLevelCollectionTypedefs::BottomOfIncludeRegion:
          case HighLevelCollectionTypedefs::BottomOfScope:
               associatedASTNode = scope;
               ROSE_ASSERT (isSgScopeStatement(associatedASTNode) != NULL);
               break;

       // Actions we want to occur when we traverse the statements in a scope
          case HighLevelCollectionTypedefs::BeforeCurrentPosition:
          case HighLevelCollectionTypedefs::ReplaceCurrentPosition:
          case HighLevelCollectionTypedefs::AfterCurrentPosition:
             {
            // Since these cases are relative to the current statement we have to more specific than
            // pinpointing the current scope or the currentStatement.  We have to find the current 
            // statement or one of its parents in the current scope.
//             associatedASTNode = currentStatement;

            // Get the parent of the current statement, usually a scope of some sort (e.g. basic block)
//             SgStatement* parentStatement = isSgStatement(currentStatement->get_parent());
               SgStatement* parentStatement = currentStatement;
               ROSE_ASSERT (parentStatement != NULL);
               bool foundTargetScope = false;
            // SgScopeStatement* targetScope            = isSgScopeStatement(parentStatement);
               SgStatement*      statementInTargetScope = isSgStatement(parentStatement);
               while ( (foundTargetScope == false) && (parentStatement->get_parent() != NULL) )
                  {
//                  printf ("parentStatement->sage_class_name() = %s \n",parentStatement->sage_class_name());
                    statementInTargetScope = isSgStatement(parentStatement);
                    parentStatement = isSgStatement(parentStatement->get_parent());
//                  targetScope = isSgScopeStatement(parentStatement);
                    ROSE_ASSERT (parentStatement != NULL);
                    if (parentStatement == scope)
                       {
                      // found required scope of curent statement
//                       printf ("Found the relavant scope! \n");
                         foundTargetScope = true;
                       }
                  }

               ROSE_ASSERT (statementInTargetScope != NULL);
//             printf ("statementInTargetScope = %s \n",statementInTargetScope->sage_class_name());
//             printf ("statementInTargetScope = %s \n",statementInTargetScope->unparseToString().c_str());

            // Now find the current statement in the list of statements in the current scope
               associatedASTNode = statementInTargetScope;
               ROSE_ASSERT (dynamic_cast<SgGlobal*>(associatedASTNode) == NULL);
               break;
             }

          case HighLevelCollectionTypedefs::unknownPositionInScope:
            // For this case build a new SgBasicBlock and return a pointer to it
               printf ("ERROR: case of unknownPositionInScope not implemented \n");
               ROSE_ABORT();
               break;

          default:
               printf ("default case found is switch statement (to set associatedASTNode) value = %d \n",relativeLocation);
               ROSE_ABORT();
               break;
        }

     ROSE_ASSERT (associatedASTNode != NULL);

#if 0
     printf ("associatedASTNode = %p associatedASTNode->sage_class_name() = %s \n",
              associatedASTNode,associatedASTNode->sage_class_name());
#endif

     return associatedASTNode;
   }


// template <class T>
template<>
TransformationStringTemplatedType<HighLevelCollectionTypedefs>::
TransformationStringTemplatedType (
   SgNode* astNode,
   const string & X,
   ScopeIdentifierEnum inputRelativeScope,
   PlacementPositionEnum inputRelativeLocation,
   bool inputBuildInNewScope )
   {
  // In this constructor we use the input astNode as the current position 
  // from which to compute the absolute position using the relative offset.

  // This constructor uses default arguments
     relativeScope            = inputRelativeScope;
     relativeLocation         = inputRelativeLocation;

  // This pointer will be reset before existing this function
     associatedASTNode        = NULL;

     buildInNewScope          = inputBuildInNewScope;

  // Enforce simple constraints on the values of the input variables
     preconditions(relativeScope,relativeLocation,buildInNewScope);

#if 0
     printf ("At top of TransformationStringTemplatedType<HighLevelCollectionTypedefs> constructor: relativeScope    = %s \n",getRelativeScopeString().c_str());
     printf ("At top of TransformationStringTemplatedType<HighLevelCollectionTypedefs> constructor: relativeLocation = %s \n",getRelativeLocationString().c_str());
#endif

  // No processing of the input string is require (within new version of rewrite mechanism)
     sourceCode               = X;

#if 0
  // The AST fragements need to be associated with a specified scope but depending upon the
  // specified location in the scope we want to attach the AST fragements relative to either the
  // current statement (at some scope) or a specific scope itself.  This switch statement sets up
  // two pointers which another switch statement will use one or the other of to associate an AST
  // fratements (currently just a string since we have not processed it yet) with a position for
  // insertion into the application program's AST.

     SgStatement* currentStatement = isSgStatement(astNode);
     ROSE_ASSERT (currentStatement != NULL);
     SgScopeStatement* targetScope = getScope (currentStatement,relativeScope);
     ROSE_ASSERT (targetScope != NULL);

  // Depending upon the position in the scope we will attach the AST fragments (once the strings are
  // gathered, sorted and converted into AST fragments) to either the scopes or the statements
  // themselves.
#endif

     SgStatement* currentStatement = isSgStatement(astNode);
     ROSE_ASSERT (currentStatement != NULL);

#if 0
  // Debugging output
     printf ("Output the different scopes and relative positions in scope for currentStatement = %s \n",currentStatement->sage_class_name());
     printf ("     currentStatement = %s \n",currentStatement->unparseToString().c_str());
     for (int i = HighLevelCollectionTypedefs::LocalScope; i < HighLevelCollectionTypedefs::LAST_SCOPE_TAG; i++)
        {
          for (int j = HighLevelCollectionTypedefs::PreamblePositionInScope; j < HighLevelCollectionTypedefs::LAST_PLACEMENT_TAG; j++)
             {
               SgStatement* tempStatement = 
                    getAssociatedStatement(currentStatement,ScopeIdentifierEnum(i),PlacementPositionEnum(j));
               printf ("   i = %s j = %s tempStatement = %s at line %d \n",
                    HighLevelCollectionTypedefs::getRelativeScopeString(ScopeIdentifierEnum(i)).c_str(),
                    HighLevelCollectionTypedefs::getRelativeLocationString(PlacementPositionEnum(j)).c_str(),
                    tempStatement->sage_class_name(),
                    rose::getLineNumber(tempStatement));
            // printf ("\n");
             }
        }

     printf ("\n");
     printf ("Exiting after output of all possible scopes and locations in each scope: currentStatement = %s \n",
          currentStatement->sage_class_name(),rose::getLineNumber(currentStatement));
     printf ("     currentStatement->unparseToString() = \n%s\n",currentStatement->unparseToString().c_str());
     ROSE_ABORT();
#endif

  // The associated node is the location where within the traversal the transformation will be triggered
     associatedASTNode = getAssociatedStatement(currentStatement,relativeScope,relativeLocation);
     ROSE_ASSERT (associatedASTNode != NULL);

  // error checking
     if (dynamic_cast<SgStatement*>(associatedASTNode) == NULL)
        {
          printf ("ERROR: associatedASTNode should be a Statement: associatedASTNode->sage_class_name() = %s \n",associatedASTNode->sage_class_name());
        }
     ROSE_ASSERT (dynamic_cast<SgStatement*>(associatedASTNode) != NULL);

#if 0
     printf ("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& \n");
     printf ("At base of TransformationStringTemplatedType<HighLevelCollectionTypedefs> constructor \n");
     printf ("   input relativeScope    = %s \n",getRelativeScopeString().c_str());
     printf ("   input relativeLocation = %s \n",getRelativeLocationString().c_str());
     printf ("   input string           = %s \n",X.c_str());
     printf ("   TransformationStringTemplatedType associatedASTNode->sage_class_name() = %s \n",associatedASTNode->sage_class_name());
     printf ("   TransformationStringTemplatedType associatedASTNode = %s \n",associatedASTNode->unparseToString().c_str());
     printf ("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& \n");
#endif
   }

















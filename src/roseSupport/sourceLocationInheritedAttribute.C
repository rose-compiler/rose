
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sourceLocationInheritedAttribute.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// ************************************************************
//               Inherited Attribute Base Class
// ************************************************************

void
SourceLocationInheritedAttribute::initialize()
   {
  // Initialize all pointers to NULL
     project                        = NULL;
     file                           = NULL;
     globalScope                    = NULL;
     currentFunctionDefinitionScope = NULL;
     loopNestParentScope            = NULL;
     conditionalParentScope         = NULL;
     localScope                     = NULL;

  // This is some what redundent since the the funtion body is available from the
  // currentFunctionDefinition.
     currentFunctionBasicBlockScope = NULL;

  // current statement and expression being evaluated
  // currentDeclarationStatement    = NULL;
     currentStatement               = NULL;
     currentExpression              = NULL;
     currentNode                    = NULL;
   }

#if 0
#if 0
SourceLocationInheritedAttribute::SourceLocationInheritedAttribute()
   {
  // Only the first instance of a SourceLocationInheritedAttribute object should use the default constructor!
     printf ("SourceLocationInheritedAttribute default constructor should only be used for initial creation of object! \n");
     initialize();
   }
#else
// Use a special function to generate the initial instance of a 
// SourceLocationInheritedAttribute object for a traversal so avoid
// having the default constructor being called by accedent due to 
// user error.

SourceLocationInheritedAttribute
SourceLocationInheritedAttribute::initialInstance()
   {
  // Only the first instance of a SourceLocationInheritedAttribute object should use the default constructor!
     printf ("SourceLocationInheritedAttribute default constructor should only be used for initial creation of object! \n");
     initialize();

     return *this;
   }
#endif
#endif

SourceLocationInheritedAttribute::~SourceLocationInheritedAttribute()
   {
  // Reset everything to NULL
     initialize();
   }

SourceLocationInheritedAttribute &
SourceLocationInheritedAttribute::
operator= ( const SourceLocationInheritedAttribute & X )
   {
  // printf ("In SourceLocationInheritedAttribute::operator=()  from old one scope = %p \n",globalScope);

  // Initialize all pointers to NULL
     project                        = X.project;
     file                           = X.file;
     globalScope                    = X.globalScope;
     currentFunctionDefinitionScope = X.currentFunctionDefinitionScope;
     loopNestParentScope            = X.loopNestParentScope;
     conditionalParentScope         = X.conditionalParentScope;
     localScope                     = X.localScope;

  // This is redundent data
     currentFunctionBasicBlockScope = X.currentFunctionBasicBlockScope;

  // current statement and expression being evaluated
  // currentDeclarationStatement    = X.currentDeclarationStatement;
     currentStatement               = X.currentStatement;
     currentExpression              = X.currentExpression;
     currentNode                    = X.currentNode;

  // Assignment for STL vector of scopes (SgScopeStatement pointers)
     scopeList                      = X.scopeList;
     statementList                  = X.statementList;

     return *this;
   }

SourceLocationInheritedAttribute::
SourceLocationInheritedAttribute ( SgNode* astNode )
   {
     ROSE_ASSERT (astNode != NULL);

     initialize();

  // Refine the information with a classification of the current node
     refineClassification(astNode);
   }

SourceLocationInheritedAttribute::
SourceLocationInheritedAttribute ( const SourceLocationInheritedAttribute & X )
   {
  // After the first instance, all subsequent object should be build using the copy constructor!

  // The usual implementation of the copy constructor
     operator=(X);
   }

SourceLocationInheritedAttribute::
SourceLocationInheritedAttribute(
     const SourceLocationInheritedAttribute & X, SgNode* astNode )
   {
     ROSE_ASSERT (astNode != NULL);

  // Save the current information
     operator=(X);

  // Refine the information with a classification of the current node
     refineClassification(astNode);
   }

void
SourceLocationInheritedAttribute::
refineClassification ( SgNode* astNode )
   {
     ROSE_ASSERT (astNode != NULL);
  // printf ("Inside of SourceLocationInheritedAttribute::refineClassification() astNode->sage_class_name() = %s \n",astNode->sage_class_name());

  // Always update this variable
     currentNode = astNode;

  // Only update the currentStatement if the current node is a SgStatement node
     if (isSgStatement(astNode) != NULL)
        {
          currentStatement = isSgStatement(astNode);

       // With this stack we never have to pop anything off the stack
          statementList.push_back(currentStatement);

       // IDEA: Store the current statement with the scope as we push the scope onto the stack
       // Then we can more accurately associate the statement with a given scope.
       // if (isSgScopeStatement(astNode) != NULL)
       // if ( (isSgScopeStatement(astNode) != NULL) && (astNode->get_traversalSuccessorNamesContainer().size() > 0) )

       // Only save the scoep if it is a global scope or basic block
          if ( (isSgGlobal(astNode) != NULL) ||
               (isSgBasicBlock(astNode) != NULL) )
             {
               SgScopeStatement* scope = isSgScopeStatement(astNode);
               scopeList.push_back(scope);
             }

       // printf ("Need to store the current statement with the scope (or something like that) \n");
        }

#if ROSE_INTERNAL_DEBUG
  // error checking
     unsigned int i;
     for (i = 0; i < scopeList.size(); i++)
        {
          ROSE_ASSERT (scopeList[i] != NULL);
        }
     for (i = 0; i < statementList.size(); i++)
        {
          ROSE_ASSERT (statementList[i] != NULL);
        }
#endif

  // Only update the currentExpression if the current node is a SgExpression node
     if (isSgExpression(astNode) != NULL)
          currentExpression = isSgExpression(astNode);

  // currentFunctionBasicBlockScope = NULL;

  // Refine the information with a classification of the current node
     ROSE_ASSERT (astNode != NULL);
     switch (astNode->variantT())
        {
          case V_SgProject:
               project = isSgProject(astNode);
               break;

       // case V_SgFile:
          case V_SgSourceFile:
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
          case V_SgBinaryComposite:
#endif
               file = isSgFile(astNode);
               break;

          case V_SgGlobal:
               globalScope = isSgGlobal(astNode);

            // This will be reset when we hit a SgBasicBlock (but should be set here
            // so that the local scope can be refered to even in the global scope).
               localScope  = globalScope;

            // With this stack we never have to pop anything off the stack
            // scopeList.push_back(globalScope);
               break;

          case V_SgFunctionDeclaration:
#if 0
               currentFunctionDeclaration = isSgFunctionDeclaration(astNode);
#endif
               break;

          case V_SgFunctionDefinition:
             {
               currentFunctionDefinitionScope = isSgFunctionDefinition(astNode);

            // With this stack we heve have to pop anything off the stack (don't add function definition to scope list)
            // scopeList.push_back(currentFunctionDefinitionScope);
#if 0
            // Maybe the function scope should be set when we process the basic block?
               SgFunctionDefenition functionDefinition = isSgFunctionDefinition(astNode);
               if (functionDefinition != NULL)
                    currentFunctionScope = functionDefinition->get_block();
#endif
               break;
             }

          case V_SgBasicBlock:
             {
            // printf ("Case V_SgBasicBlock \n");
               localScope = isSgBasicBlock(astNode);
               if ( isSgFunctionDefinition(localScope->get_parent()) != NULL )
                  {
                    currentFunctionBasicBlockScope = isSgBasicBlock(astNode);
                    ROSE_ASSERT ( currentFunctionBasicBlockScope == currentFunctionDefinitionScope->get_body() );
                  }

            // loop back through the loop nest to recover the parent scope of the loop nest
               SgForStatement* forStatement = isSgForStatement(localScope->get_parent());
               while ( forStatement != NULL )
                  {
                    SgStatement* parentStatement = isSgStatement(forStatement->get_parent());
                    loopNestParentScope = isSgBasicBlock(parentStatement);
                    ROSE_ASSERT (loopNestParentScope != NULL);
                    forStatement = isSgForStatement(loopNestParentScope->get_parent());
                  }

            // don't worry about nested conditionals (not yet at least)
               SgIfStmt* ifStatement = isSgIfStmt(localScope->get_parent());
               if ( ifStatement != NULL )
                  {
                    SgStatement* parentStatement = isSgStatement(ifStatement->get_parent());
                    conditionalParentScope = isSgBasicBlock(parentStatement);
                    ROSE_ASSERT (conditionalParentScope != NULL);
                  }

            // With this stack we never have to pop anything off the stack
            // scopeList.push_back(localScope);
               break;
             }

       // For now all these cases are the same
          case V_SgCatchOptionStmt:
          case V_SgClassDefinition:
          case V_SgDoWhileStmt:
          case V_SgForStatement:
          case V_SgIfStmt:
          case V_SgSwitchStatement:
          case V_SgWhileStmt:
             {
               SgScopeStatement* scope = isSgScopeStatement(astNode);
               ROSE_ASSERT (scope != NULL);

            // With this stack we never have to pop anything off the stack
            // scopeList.push_back(scope);
               break;
             }

          default:
            // astNode is some other type of AST node which we don't keep track of in this inherited
            // attribute.
#if 0
               printf ("Default case inside of SourceLocationInheritedAttribute::refineClassification(astNode = %p) \n",astNode);
               ROSE_ABORT();
#endif
               break;
        }

  // assertValidPointers();

#if 0
     printf ("Inside of SourceLocationInheritedAttribute::refineClassification(astNode = %p) \n",astNode);
     display("Inside of SourceLocationInheritedAttribute::refineClassification()");
#endif
#if 0
     printf ("Inside of SourceLocationInheritedAttribute::refineClassification(astNode = %p) \n",astNode);
     ROSE_ABORT();
#endif
   }


void
SourceLocationInheritedAttribute::
display( const string s ) const
   {
     printf ("Inside of SourceLocationInheritedAttribute::display(%s) \n",s.c_str());
     printf ("     project                        = %p \n",project);
     printf ("     file                           = %p \n",file);
     printf ("     globalScope                    = %p \n",globalScope);
  // printf ("     currentFunctionDeclaration     = %p \n",currentFunctionDeclaration);
     printf ("     currentFunctionDefinitionScope = %p \n",currentFunctionDefinitionScope);
  // printf ("     currentFunctionScope           = %p \n",currentFunctionScope);
     printf ("     loopNestParentScope            = %p \n",loopNestParentScope);
     printf ("     conditionalParentScope         = %p \n",conditionalParentScope);
     printf ("     localScope                     = %p \n",localScope);
     printf ("     currentFunctionBasicBlockScope = %p \n",currentFunctionBasicBlockScope);

     printf ("     scopeList (number of scopes = %" PRIuPTR "): \n",scopeList.size());
     unsigned int i = 0;
     for (i = 0; i < scopeList.size(); i++)
        {
          ROSE_ASSERT (scopeList[i] != NULL);
          printf ("          vector[%d] = %p  Scope Name: %s \n",i,scopeList[i],scopeList[i]->sage_class_name());
        }
     printf ("\n");

     printf ("     statementList (number of statements = %" PRIuPTR "): \n",statementList.size());
     for (i = 0; i < statementList.size(); i++)
        {
          ROSE_ASSERT (statementList[i] != NULL);
          printf ("          vector[%d] = %p  Statement Name: %s \n",i,statementList[i],statementList[i]->sage_class_name());
        }
     printf ("\n");

  // assertValidPointers();
   }


// SgGlobal*
SgNode*
SourceLocationInheritedAttribute::getGlobalScope() const
   {
     SgGlobal* scope = NULL;
     scope = globalScope;

     ROSE_ASSERT (scope != NULL);
     ROSE_ASSERT (isSgGlobal(scope) != NULL);
     return scope;
   }

SgNode*
SourceLocationInheritedAttribute::
getCurrentStatementInScope( SgScopeStatement* targetScope ) const
   {
     ROSE_ASSERT (targetScope != NULL);

#if 1
     printf ("At top of SourceLocationInheritedAttribute::getCurrentStatementInScope(): targetScope = %p targetScope->sage_class_name() = %s \n",
          targetScope,targetScope->sage_class_name());
#endif

     int scopeDepth = -1;
     unsigned int i;

  // Search the scopeList for the target scope. We no longer use the index of the 
  // scope to compute the index into the statement list. This would be complicated 
  // since the lists are different sizes debending on the details of the traversal 
  // of the AST (what is a child vs. what is a new sort of Sage node).
     for (i = 0; i < scopeList.size(); i++)
        {
          ROSE_ASSERT (scopeList[i] != NULL);
          if (scopeList[i] == targetScope)
               scopeDepth = i;
        }

#if 1
  // error checking (redundent with assert below, but provides better user interface) 
  // printf ("scopeDepth = %d \n",scopeDepth);
     if (scopeDepth < 0)
        {
       // Report an error since no scope was found
          printf ("ERROR: target scope was not found targetScope = %p targetScope->unparseToString() = %s \n",
               targetScope,targetScope->unparseToString().c_str());
          ROSE_ABORT();
        }
#endif

     ROSE_ASSERT (scopeDepth >= 0);

  // Now get the depth of the associated target statement
  // int targetStatementDepth = scopeDepth+1;
  // increment depends upon number of functions (could be more than one with local class definitions).
  // int targetStatementDepth = scopeDepth+1;

#if 1
     printf ("In SourceLocationInheritedAttribute::getCurrentStatementInScope(): scopeDepth = %d  scopeList.size()     = %" PRIuPTR " \n",scopeDepth,scopeList.size());
     printf ("In SourceLocationInheritedAttribute::getCurrentStatementInScope(): scopeDepth = %d  statementList.size() = %" PRIuPTR " \n",scopeDepth,statementList.size());
     printf ("##### Find the associated current statement in the target scope: \n");
#endif

  // If this is not a query about the deepest scope then we can find the current statement 
  // in scopeList by looking at the next scope.  Else we have to look at the last statement in the 
  // statement list (to find the current statement in the deepest (currently local) scope).
     SgStatement* targetStatement = NULL;
     if (scopeDepth < (int)(scopeList.size()-1))
        {
          printf ("     use the NEXT statement in the scope list \n");
#if 0
          targetStatement = scopeList[scopeDepth+1];
#else
       // We need to find the target statement in the targe scope, this might not be in the
       // list of scopes since only some scopes are saved to avoid redundent accumulation of 
       // multiple nodes that can represent only a single scope (e.g. functions).  The scope
       // that we store are not always the ones that represent the first node representing 
       // that scope (e.g. functions). But the targetStaement must be initialized to the 
       // statement that appears in the target scope (because the insertion mechanism requires 
       // this).
          int indexOfTargetStatement = -1;
          for (i = 0; i < statementList.size(); i++)
             {
               ROSE_ASSERT (statementList[i] != NULL);
               if (statementList[i] == targetScope)
                    indexOfTargetStatement = i+1;
             }
          ROSE_ASSERT (indexOfTargetStatement >= 0);
          ROSE_ASSERT (indexOfTargetStatement < (int)statementList.size());
          targetStatement = statementList[indexOfTargetStatement];
#endif
        }
       else
        {
          printf ("     use the LAST statement in the statement list \n");
          targetStatement = *(statementList.rbegin());
        }

     ROSE_ASSERT (targetStatement != NULL);

#if 0
     if (isSgScopeStatement( *(statementList.rbegin()) ) != NULL)
          targetStatementDepth = scopeDepth;
       else
          targetStatementDepth = scopeDepth+1;
#endif

#if 1
     printf ("targetScope     = %p targetScope->sage_class_name()     = %s \n",
          targetScope,targetScope->sage_class_name());
     printf ("targetScope = %p targetScope->unparseToString() = %s \n",
          targetScope,targetScope->unparseToString().c_str());
     printf ("targetStatement = %p targetStatement->sage_class_name() = %s \n",
          targetStatement,targetStatement->sage_class_name());
     printf ("targetStatement = %p targetStatement->unparseToString() = %s \n",
          targetStatement,targetStatement->unparseToString().c_str());
#endif

#if ROSE_INTERNAL_DEBUG
  // Error checking: search for targetStatement within the targetScope
     bool found = false;
     if (targetScope->containsOnlyDeclarations() == true)
        {
          printf ("Looking in a scope containing only declarations ... \n");
          SgDeclarationStatementPtrList & declarationList = targetScope->getDeclarationList();
          ROSE_ASSERT (declarationList.size() > 0);
          SgDeclarationStatementPtrList::iterator j;
          for (j = declarationList.begin(); j != declarationList.end(); j++)
             {
               ROSE_ASSERT ((*j) != NULL);
#if 0
               printf ("Testing against declaration: (*j) = %p (*j)->unparseToString() = %s \n",
                    (*j),(*j)->unparseToString().c_str());
#endif
               if ( (*j) == targetStatement )
                    found = true;
             }
        }
       else
        {
          printf ("Looking in a scope containing any statement nodes ... \n");
          SgStatementPtrList & statementList = targetScope->getStatementList();
          ROSE_ASSERT (statementList.size() > 0);
          SgStatementPtrList::iterator j;
          for (j = statementList.begin(); j != statementList.end(); j++)
             {
               ROSE_ASSERT ((*j) != NULL);
#if 0
               printf ("Testing against statement: (*j) = %p (*j)->unparseToString() = %s \n",
                    (*j),(*j)->unparseToString().c_str());
#endif
               if ( (*j) == targetStatement )
                    found = true;
             }
        }

     if (found == false)
          display("At base of SourceLocationInheritedAttribute::getCurrentStatementInScope()");

     ROSE_ASSERT (found == true);
#endif

#if 0
     printf ("Exiting at base of SourceLocationInheritedAttribute::getCurrentStatementInScope() \n");
     ROSE_ABORT();
#endif

     ROSE_ASSERT (targetStatement != NULL);

     return targetStatement;
   }

// SgFunctionDefinition*
SgNode*
SourceLocationInheritedAttribute::getCurrentFunctionScope() const
   {
     SgFunctionDefinition* scope = NULL;
     scope = currentFunctionDefinitionScope;

     ROSE_ASSERT (scope != NULL);
     return scope;
   }

// SgScopeStatement*
SgNode*
SourceLocationInheritedAttribute::getLocalScope() const
   {
     SgScopeStatement* scope = NULL;
     int numberOfScopes = scopeList.size();

  // printf ("In SourceLocationInheritedAttribute::getLocalScope(): numberOfScopes = %d \n",numberOfScopes);
  // ROSE_ASSERT (numberOfScopes > 0);
     if (numberOfScopes > 0)
          scope = scopeList[numberOfScopes-1];
       else
          scope = isSgGlobal( getGlobalScope() );

     ROSE_ASSERT (scope != NULL);
     return scope;
   }

// SgScopeStatement*
SgNode*
SourceLocationInheritedAttribute::getLoopNestParentScope() const
   {
     SgScopeStatement* scope = NULL;
     scope = loopNestParentScope;

  // ROSE_ASSERT (scope != NULL);
     return scope;
   }

// SgScopeStatement*
SgNode*
SourceLocationInheritedAttribute::getConditionalParentScope() const
   {
     SgScopeStatement* scope = NULL;
     scope = conditionalParentScope;

  // ROSE_ASSERT (scope != NULL);
     return scope;
   }

// SgScopeStatement*
SgNode*
SourceLocationInheritedAttribute::getParentScope() const
   {
     SgScopeStatement* scope = NULL;
     int numberOfScopes = scopeList.size();
  // ROSE_ASSERT (numberOfScopes > 0);
  // printf ("In SourceLocationInheritedAttribute::getParentScope(): numberOfScopes = %d \n",numberOfScopes);
     if (numberOfScopes > 1)
        {
          scope = scopeList[numberOfScopes-2];

       // We don't want to return a for loop statement as a parent scope statement since insert_statement
       // is not well defined for such scope statements.

       // if (scope->variantT() == V_SgForStatement)
          int i = 1;
          while (scope->variantT() == V_SgForStatement)
             {
               ROSE_ASSERT (numberOfScopes-(2+i) >= 0);
               scope = scopeList[numberOfScopes-(2+i)];
               i++;
             }
        }
       else
        {
          scope = isSgGlobal( getGlobalScope() );
        }

     printf ("In SourceLocationInheritedAttribute::getParentScope(): parent scope is %s \n",scope->sage_class_name());

     ROSE_ASSERT (scope->variantT() != V_SgForStatement);

     ROSE_ASSERT (scope != NULL);
     return scope;
   }

// SgBasicBlock*
SgNode*
SourceLocationInheritedAttribute::getCurrentFunctionBasicBlockScope() const
   {
     SgBasicBlock* scope = NULL;
     scope = currentFunctionBasicBlockScope;

     ROSE_ASSERT (scope != NULL);
     ROSE_ASSERT (isSgScopeStatement(scope) != NULL);
     return scope;
   }

// SgStatement*
SgNode*
SourceLocationInheritedAttribute::getCurrentStatement() const
   {
     SgStatement* stmt = NULL;
     stmt = currentStatement;

     ROSE_ASSERT (stmt != NULL);
     return stmt;
   }

// SgExpression*
SgNode*
SourceLocationInheritedAttribute::getCurrentExpression() const
   {
     SgExpression* expr = NULL;
     expr = currentExpression;

     ROSE_ASSERT (expr != NULL);
     return expr;
   }

// SgNode*
SgNode*
SourceLocationInheritedAttribute::getCurrentNode() const
   {
     SgNode* node = NULL;
     node = currentNode;

     ROSE_ASSERT (node != NULL);
     return node;
   }

#if 0
SgNode*
SourceLocationInheritedAttribute::getPositionInGlobalScope() const
   {
     SgNode* node = NULL;
     node = currentNode;

     ROSE_ASSERT (node != NULL);
     return node;
   }

SgNode*
SourceLocationInheritedAttribute::getPositionInLocalScope() const
   {
     SgNode* node = NULL;
     node = currentNode;

     ROSE_ASSERT (node != NULL);
     return node;
   }

SgNode*
SourceLocationInheritedAttribute::getPositionInLoopNestParentScoped() const
   {
     SgNode* node = NULL;
     node = currentNode;

     ROSE_ASSERT (node != NULL);
     return node;
   }

SgNode*
SourceLocationInheritedAttribute::getPositionInFunctionBasicBlockScope() const
   {
     SgNode* node = NULL;
     node = currentNode;

     ROSE_ASSERT (node != NULL);
     return node;
   }

SgNode*
SourceLocationInheritedAttribute::getPositionInConditionalParentScope() const
   {
     SgNode* node = NULL;
     node = currentNode;

     ROSE_ASSERT (node != NULL);
     return node;
   }
#endif

























// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "rewrite.h"


// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

SgStatement*
returnFirstOrLastStatementFromCurrentFile ( 
     SgStatement* target,
     const SgStatementPtrList & statementList,
     bool findFirstStatement )
   {
  // printf ("Inside of returnFirstOrLastStatement \n");
  // Find the first statement from the current file being processed
     SgStatement* statement = NULL;
     if (findFirstStatement)
        {
          SgStatementPtrList::const_iterator statementIterator = statementList.begin();
          while (statementIterator != statementList.end())
             {
               statement = *statementIterator;
               ROSE_ASSERT (statement != NULL);
               Sg_File_Info* fileInfo = statement->get_file_info();
               ROSE_ASSERT (fileInfo != NULL);
               string filename = fileInfo->get_filename();
               if (filename == ROSE::getFileNameByTraversalBackToFileNode(target))
                  {
                 // printf ("Found the first statement in this file = %s \n",filename.c_str());
                 // printf ("First statement = %s \n",statement->unparseToCompleteString().c_str());
                    break;
                  }
               statementIterator++;
             }
        }
       else
        {
          SgStatementPtrList::const_reverse_iterator statementIterator = statementList.rbegin();
          while (statementIterator != statementList.rend())
             {
               statement = *statementIterator;
               ROSE_ASSERT (statement != NULL);
               Sg_File_Info* fileInfo = statement->get_file_info();
               ROSE_ASSERT (fileInfo != NULL);
               string filename = fileInfo->get_filename();
               if (filename == ROSE::getFileNameByTraversalBackToFileNode(target))
                  {
                 // printf ("Found the last statement in this file = %s \n",filename.c_str());
                 // printf ("First statement = %s \n",statement->unparseToCompleteString().c_str());
                    break;
                  }
               statementIterator++;
             }
        }

     ROSE_ASSERT (statement != NULL);
     return statement;
   }


void
resetTargetStatementAndLocation ( 
     SgStatement* & target,
     const SgStatementPtrList & statementList,
     MidLevelCollectionTypedefs::PlacementPositionEnum & locationInScope )
   {
     if (statementList.size() > 0)
        {
       // Get the first statement
          if (locationInScope == MidLevelCollectionTypedefs::TopOfCurrentScope)
             {
            // printf ("Case of StatementScope and TopOfCurrentScope \n");
               target = returnFirstOrLastStatementFromCurrentFile(target,statementList,true);
               locationInScope = MidLevelCollectionTypedefs::BeforeCurrentPosition;
             }

       // Get the last statement
          if (locationInScope == MidLevelCollectionTypedefs::BottomOfCurrentScope)
             {
            // printf ("Case of StatementScope and BottomOfCurrentScope \n");
            // declarationStatement = *(declarationList.rbegin());
               target = returnFirstOrLastStatementFromCurrentFile(target,statementList,false);
               locationInScope = MidLevelCollectionTypedefs::AfterCurrentPosition;
             }
        }
       else 
        {
          printf ("Sorry, not implemented: insertion into empty scope \n");
          ROSE_ASSERT (1 == 2);
        }
   }


template<>
bool
MidLevelRewrite<MidLevelInterfaceNodeCollection>::
insertUsingStatementScopeSupported ( SgStatement* astNode )
   {
  // This function identifies those IR nodes which contain a list and for 
  // which the StatementScope may be specified within the insert function.
     bool containsList = (isSgGlobal(astNode)          != NULL) ||
                         (isSgClassDefinition(astNode) != NULL) ||
                         (isSgBasicBlock(astNode)      != NULL);

  // printf ("In supportsInsertUsingStatementScope: containsList = %s \n",containsList ? "true" : "false");
     return containsList;
   }

template<>
bool
MidLevelRewrite<MidLevelInterfaceNodeCollection>::
insertSupported ( SgStatement* astNode, MidLevelCollectionTypedefs::ScopeIdentifierEnum inputRelativeScope )
   {
  // This function filters out a few unsupported IR nodes which are not 
  // currently supported by the AST rewrite mechanism.
     bool skip = // (isSgGlobal(astNode)                           != NULL) ||
//               (isSgForStatement(astNode->get_parent())       != NULL) ||
//               (isSgSwitchStatement(astNode->get_parent())    != NULL) ||
//               (isSgFunctionDefinition(astNode->get_parent()) != NULL) ||
                 (isSgClassDefinition(astNode)                  != NULL) ||
                 (isSgFunctionParameterList(astNode)            != NULL) ||
                 (isSgCtorInitializerList(astNode)              != NULL) ||
                 (isSgCaseOptionStmt(astNode)                   != NULL) ||
                 (isSgDefaultOptionStmt(astNode)                != NULL);

     bool currentlySupported = !skip;

  // printf ("In insertCurrentlySupported: currentlySupported = %s \n",currentlySupported ? "true" : "false");

     bool supported = currentlySupported;

  // Extra test required when (inputRelativeScope == StatementScope)
     if ( (currentlySupported == true) && (inputRelativeScope == MidLevelCollectionTypedefs::StatementScope) )
        {
          supported = insertUsingStatementScopeSupported(astNode);
        }

     return supported;
   }


template<>
ROSE_DLL_API
void
MidLevelRewrite<MidLevelInterfaceNodeCollection>::
insert (
   SgStatement* target,
   const string & transformationString,
   ScopeIdentifierEnum inputRelativeScope,
   PlacementPositionEnum locationInScope )
   {
     ROSE_ASSERT (target != NULL);

#if 0
     printf ("MidLevelRewrite<MidLevelInterfaceNodeCollection>::insert(): inputRelativeScope = %s \n",
          MidLevelCollectionTypedefs::getRelativeScopeString(inputRelativeScope).c_str());
#endif

  // Error Reporting
  // Test to see if this is a supported statement (a few are not supported in the rewrite mechanism)
     if ( insertSupported(target,inputRelativeScope) == false )
        {
          printf ("ERROR (MidLevelRewrite<MidLevelInterfaceNodeCollection>::insert): \n");
          printf ("     This %s statement is not currently supported (or not supported \n",target->sage_class_name());
          printf ("     in its position relative to other statements) within the AST Rewrite Mechanism. \n");
          printf ("Work Around: \n");
          printf ("     Use the parent node as a target instead (and specify corresponding appropriate (longer) string. \n");
          printf ("     exiting ... \n");
          ROSE_ABORT();
        }

  // Use a more general interface to allow relative strings to be used in the high level interface 
  // even though they are not required mid level interface.
     MidLevelInterfaceNodeCollection stringAndNodeCollection;

  // DQ (1/15/2003): Needed to add handling of StatementScope insert handling.
  // The specification for scope can be either surrounding scope of statement scope
  // If it is statement scope then the target must contain a list of statements.
     ScopeIdentifierEnum scope = inputRelativeScope;

     if (scope == MidLevelCollectionTypedefs::StatementScope)
        {
       // Not clear if this is a sufficent test
          ROSE_ASSERT (isSgScopeStatement(target) != NULL);

       // Now specify a new target (use any statement inside the scope pointer to by target)
       // printf ("Before resetting target is a: %s \n",target->sage_class_name());

       // Not clear if this works for ALL scope statements!          
          SgScopeStatement* scope = isSgScopeStatement(target);
          ROSE_ASSERT (scope != NULL);
          SgStatementPtrList statementList = scope->generateStatementList();
          resetTargetStatementAndLocation (target,statementList,locationInScope);

       // Reset the target to a statement in the scope pointed to by the target
          ROSE_ASSERT (target != NULL);

       // printf ("Reset target is a: %s \n",target->sage_class_name());
       // printf ("Reset target location is: %s \n",MidLevelCollectionTypedefs::getRelativeLocationString(locationInScope).c_str());
        }

     bool buildInNewScope = false;
     TransformationStringTemplatedType<MidLevelCollectionTypedefs> 
          transformation (target,transformationString,scope,locationInScope,buildInNewScope);

#if 0
     transformation.display("In mid level insert");
#endif
#if 0
     printf ("After display ... exiting ... \n");
     ROSE_ASSERT (false);
#endif

     stringAndNodeCollection.addString(target,transformation);

#if 0
  // PreamblePositionInScope = 1 /*!< Source code to be placed at the top of a specified scope */ ,
  // TopOfCurrentScope       = 2 /*!< Top of scope (current location must be a scope)  */ ,
  // BeforeCurrentPosition   = 3 /*!< Before  */ ,
  // ReplaceCurrentPosition  = 4 /*!< Replace */ ,
  // AfterCurrentPosition    = 5 /*!< After   */ ,
  // BottomOfCurrentScope    = 6 /*!< Bottom of scope (current location must be a scope)  */ ,
#endif

  // Be careful to include/exclude the current statement when generating the prefix!
     bool prefixIncludesCurrentStatement = true;
     switch(locationInScope)
        {
          case MidLevelCollectionTypedefs::TopOfCurrentScope:
          case MidLevelCollectionTypedefs::BeforeCurrentPosition:
          case MidLevelCollectionTypedefs::ReplaceCurrentPosition:
               prefixIncludesCurrentStatement = false;
               break;

          case MidLevelCollectionTypedefs::AfterCurrentPosition:
          case MidLevelCollectionTypedefs::BottomOfCurrentScope:
               prefixIncludesCurrentStatement = true;
               break;

          default:
               printf ("Error, default reached in MidLevelRewrite<MidLevelInterfaceNodeCollection>::insert() \n");
               ROSE_ASSERT (false);
        }

#if 0
     printf ("In MidLevelRewrite<MidLevelInterfaceNodeCollection>::insert() prefixIncludesCurrentStatement = %s \n",
          (prefixIncludesCurrentStatement == true) ? "true" : "false");
#endif

     stringAndNodeCollection.writeAllChangeRequests(target,prefixIncludesCurrentStatement);

#if 0
     printf ("Exiting in MidLevelRewrite<MidLevelInterfaceNodeCollection>::insert() \n");
     ROSE_ABORT();
#endif
   }















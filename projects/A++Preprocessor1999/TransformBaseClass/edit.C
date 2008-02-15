// Include rose.h so we can manipulate ROSE objects to recognize and do the transformations
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rose.h>
#include "TransformationBase.h"

Boolean
ROSE_TransformationBase::transformationIntroducedInThisScope ()
   {
  // We need to know if the transformation has been introduced globally
     return FALSE;
   }

Boolean
ROSE_TransformationBase::transformationIntroducedInGlobalScope ()
   {
  // We need to know if the transformation has already been introduced in the current scope
     return FALSE;
   }

int
ROSE_TransformationBase::numberOfRhsOperands()
   {
  // We will implement this later
     printf ("Sorry not implemented (return value = 1 for now), 'SimpleArrayAssignment::numberOfRhsOperands()' \n");
     return 1;
   }

SgStatement*
ROSE_TransformationBase::getInnerLoopBody ( SgStatement* stmt )
   {
  // This function returns the loop body of the inner most loop in a loop nest

     int LOCAL_ROSE_DEBUG = ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) ) ? TRUE : FALSE;
     ROSE_ASSERT (stmt != NULL);

     SgStatement* returnInnerLoopBody = NULL;
     SgStatementPtrList::iterator statementIterator;

     char* notImplemented = "not implemented in SimpleArrayAssignment::getInnerLoopBody(stmt)";
     switch (stmt->variant())
        {
          case EXPR_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "EXPR_STMT " << notImplemented << endl;
               break;
          case FUNC_DECL_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "MFUNC_DECL_STMT " << notImplemented << endl;
               break;
          case MFUNC_DECL_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "MFUNC_DECL_STMT " << notImplemented << endl;
               break;
          case BASIC_BLOCK_STMT:
             {
            // os << "BASIC_BLOCK_STMT not implemented" << endl;
               SgBasicBlock *basicBlock = isSgBasicBlock(stmt);
               ROSE_ASSERT (basicBlock != NULL);
               for (statementIterator = basicBlock->get_statements().begin(); 
                    statementIterator != basicBlock->get_statements().end();
                    statementIterator++)
                  {
                 // Get the SgStatement from the SgStatementPrt using the irep() member function
                 // showSgStatement (os, (*statementIterator).irep(), depth+1);
                    if (returnInnerLoopBody == NULL)
                       {
#ifdef USE_SAGE3
                         returnInnerLoopBody = getInnerLoopBody (*statementIterator);
#else
                         returnInnerLoopBody = getInnerLoopBody ((*statementIterator).irep());
#endif
                       }
                      else
                       {
                         printf ("Additional Loop Body found in SimpleArrayAssignment::getInnerLoopBody(stmt) ignored! \n");
                      // ROSE_ABORT();
                       }
                  }
               break;
             }
          case FOR_STMT:
             {
            // cout << "FOR_STMT (test) " << notImplemented << endl;
               SgForStatement *forStatement = isSgForStatement ( stmt );
               ROSE_ASSERT (forStatement != NULL);

#if 0
               printf ("#################################################################### \n");
               printf ("#################################################################### \n");
               printf ("##################  forStatement INNER LOOP (BEGIN)  ############### \n");
               printf ("#################################################################### \n");
               printf ("#################################################################### \n");

               showSgStatement ( cout, forStatement );

               printf ("#################################################################### \n");
               printf ("#################################################################### \n");
               printf ("######################  ORIGINAL INNER LOOP (END)  ################# \n");
               printf ("#################################################################### \n");
               printf ("#################################################################### \n");
#endif
            // Find the body (list of statements) for the "for" loop
               ROSE_ASSERT (forStatement->get_loop_body() != NULL);
            // SgBasicBlock *basicBlock = forStatement->get_loop_body();
               SgBasicBlock *basicBlock = isSgBasicBlock(forStatement->get_loop_body());
               ROSE_ASSERT (basicBlock != NULL);
               if (basicBlock != NULL)
                  {
                    ROSE_ASSERT (basicBlock != NULL);
                    ROSE_ASSERT (basicBlock->variant() == BASIC_BLOCK_STMT );
                 // showSgStatement( os, basicBlock, depth+1 );
                    SgStatement *tempInnerForLoopBody = getInnerLoopBody (basicBlock);

                    if (tempInnerForLoopBody == NULL)
                         returnInnerLoopBody = basicBlock;
                      else
                         returnInnerLoopBody = tempInnerForLoopBody;
                  }
               break;
             }
          case DECL_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "DECL_STMT " << notImplemented << endl;
               break;
          case SCOPE_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "SCOPE_STMT " << notImplemented << endl;
               break;
          case FUNC_TBL_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "FUNC_TBL_STMT " << notImplemented << endl;
               break;
          case GLOBAL_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "GLOBAL_STMT " << notImplemented << endl;
               break;
          case IF_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout<< "IF_STMT " << notImplemented << endl;
               break;
          case FUNC_DEFN_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "FUNC_DEFN_STMT " << notImplemented << endl;
               break;
          case VAR_DECL_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "VAR_DECL_STMT " << notImplemented << endl;
               break;
          case VAR_DEFN_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "VAR_DEFN_STMT " << notImplemented << endl;
               break;
          case CLASS_DECL_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "CLASS_DECL_STMT " << notImplemented << endl;
               break;
          case CLASS_DEFN_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "CLASS_DEFN_STMT " << notImplemented << endl;
               break;
          case ENUM_DECL_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "ENUM_DECL_STMT " << notImplemented << endl;
               break;
          case LABEL_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "LABEL_STMT " << notImplemented << endl;
               break;
          case WHILE_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "WHILE_STMT " << notImplemented << endl;
               break;
          case DO_WHILE_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "DO_WHILE_STMT " << notImplemented << endl;
               break;
          case SWITCH_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "SWITCH_STMT " << notImplemented << endl;
               break;
          case CASE_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "CASE_STMT " << notImplemented << endl;
               break;
          case TRY_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "TRY_STMT " << notImplemented << endl;
               break;
          case CATCH_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "CATCH_STMT " << notImplemented << endl;
               break;
          case DEFAULT_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "DEFAULT_STMT " << notImplemented << endl;
               break;
          case BREAK_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "BREAK_STMT " << notImplemented << endl;
               break;
          case CONTINUE_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "CONTINUE_STMT " << notImplemented << endl;
               break;
          case RETURN_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "RETURN_STMT " << notImplemented << endl;
               break;
          case GOTO_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "GOTO_STMT " << notImplemented << endl;
               break;
          case ASM_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "ASM_STMT " << notImplemented << endl;
               break;
          case SPAWN_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "SPAWN_STMT " << notImplemented << endl;
               break;
          case PAR_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "PAR_STMT " << notImplemented << endl;
               break;
          case PARFOR_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "PARFOR_STMT " << notImplemented << endl;
               break;
          case TYPEDEF_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "TYPEDEF_STMT " << notImplemented << endl;
               break;
          case TEMPLATE_DECL_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "TEMPLATE_DECL_STMT " << notImplemented << endl;
               break;
#if USE_SAGE3
          case PRAGMA_STMT:
               if (LOCAL_ROSE_DEBUG > 0) cout << "PRAGMA_STMT " << notImplemented << endl;
               break;
#endif
          default:
               cout << "transformation_1.C, SimpleArrayAssignment::getInnerLoopBody ( SgStatement* stmt ) -- default reached" << endl;
               ROSE_ABORT();
        }

     if (returnInnerLoopBody != NULL)
          ROSE_ASSERT ( returnInnerLoopBody->variant() == BASIC_BLOCK_STMT );

     return returnInnerLoopBody;
   }


void
ROSE_TransformationBase::editInnerLoopBodyToMatchCurrentStatement ( 
   SgStatement *innerLoopBody, SgStatement *transformedStatement )
   {
  // In the first implementation we are actually editing the current statement 
  // to build a SgStatement to use for the inner loop!
     ROSE_ASSERT (innerLoopBody != NULL);
     ROSE_ASSERT (transformedStatement != NULL);

#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 5)
        {
          printf ("Inside of ROSE_TransformationBase::editInnerLoopBodyToMatchCurrentStatement(SgStatement*,SgStatement*) \n");
        }
#endif

     ROSE_ASSERT (innerLoopBody->variant()        == BASIC_BLOCK_STMT);
     ROSE_ASSERT (transformedStatement->variant() == EXPR_STMT);

  // showSgStatement (cout,innerLoopBody);

  // We use "LAST_TAG" as an indication that no operand is required (the fact 
  // that the rhs index expression is NULL says the same thing)
  // SgExpression *lhsOperand = buildLhsOperand("lhs",T_DOUBLE,"j",LAST_TAG,NULL);
 
  // Now output the display of the just built program tree
  // showSgExpression (cout,lhsOperand);

#if 0
     printf ("#################################################################### \n");
     printf ("#################################################################### \n");
     printf ("####################  TRANSFORMED STATEMENT (BEGIN)  ############### \n");
     printf ("#################################################################### \n");
     printf ("#################################################################### \n");

  // showSgStatement ( cout, currentStatement );
     showSgStatement ( cout, transformedStatement, "SimpleArrayAssignment::editInnerLoopBodyToMatchCurrentStatement(): AFTER call to change()" );

     printf ("#################################################################### \n");
     printf ("#################################################################### \n");
     printf ("##################  MODIFIED CURRENT STATEMENT (END)  ############## \n");
     printf ("#################################################################### \n");
     printf ("#################################################################### \n");

     printf ("#################################################################### \n");
     printf ("################  CALL UNPARSE ON PARENT  ########################## \n");
     printf ("#################################################################### \n");

  // currentStatement->logical_unparse(NO_UNPARSE_INFO,cout);
     transformedStatement->logical_unparse(NO_UNPARSE_INFO,cout);

     cout << endl << endl;
     cout << "####################################################################" << endl;
     cout << "####################################################################" << endl;

  // printf ("Exiting AFTER CHANGE (NEW COPY OF STATEMENT) in SimpleArrayAssignment::editInnerLoopBodyToMatchCurrentStatement(SgStatement*,SgStatement*) \n");
  // ROSE_ABORT();
#endif

#if 1
     printf ("#################################################################### \n");
     printf ("#################################################################### \n");
     printf ("###################### IN EDIT ORIGINAL INNER LOOP (BEGIN)  ############### \n");
     printf ("#################################################################### \n");
     printf ("#################################################################### \n");

     showSgStatement ( cout, innerLoopBody, "SimpleArrayAssignment::editInnerLoopBodyToMatchCurrentStatement(): AFTER call to change()" );

     printf ("#################################################################### \n");
     printf ("#################################################################### \n");
     printf ("######################  ORIGINAL INNER LOOP (END)  ################# \n");
     printf ("#################################################################### \n");
     printf ("#################################################################### \n");
#endif

  // Now replace the first statement in the loop body with the edited currentStatement
     SgStatementPtrList::iterator i;
     ROSE_ASSERT (innerLoopBody != NULL);
     SgBasicBlock* basicBlock = isSgBasicBlock(innerLoopBody);
     ROSE_ASSERT (basicBlock != NULL);
     SgStatementPtrList & innerLoopBodyList = basicBlock->get_statements();

     printf ("In BASECLASS:editInnerLoopBodyToMatchCurrentStatement(): innerLoopBodyList.size() = %d \n",
          innerLoopBodyList.size());
     ROSE_ASSERT (innerLoopBodyList.size() > 0);

  // We want only a single statement in the inner loop body (we can make this more general later!)
     ROSE_ASSERT (innerLoopBodyList.size() == 1);

#if 0
     printf ("Editing in edit.C \n");
     ROSE_ABORT();
#endif

     for (i = innerLoopBodyList.begin(); i != innerLoopBodyList.end(); i++)
        {
       // This function does not yet provide a deep enough copy!
       // SgStatement *newStatement = copyinnerLoopStatement ((*i).irep());
       // SgStatement *oldStatement = (*i).irep();
       // ROSE_ASSERT (oldStatement != NULL);

       // Boolean inFront = TRUE;

#if 1
          printf ("#################################################################### \n");
          printf ("#################################################################### \n");
          printf ("#####################  PARENT OF INNER LOOP (BEGIN)  ############### \n");
          printf ("#################################################################### \n");
          printf ("#################################################################### \n");

       // showSgStatement ( cout, innerLoopBody->get_parent(), 
       //    "Called from editInnerLoopBodyToMatchCurrentStatement(SgStatement*,SgStatement*)" );
          showSgStatement ( cout, *i,
             "Called from editInnerLoopBodyToMatchCurrentStatement(SgStatement*,SgStatement*)" );

          printf ("#################################################################### \n");
          printf ("#################################################################### \n");
          printf ("###################  PARENT OF INNER LOOP (END)  ################### \n");
          printf ("#################################################################### \n");
          printf ("#################################################################### \n");
#endif

       // We can't use this because the global scope is the for loop (not the SgBasicBlock)
       // and so the replacement is not so simple.  It is easier to just use the iterator's
       // replace member function
       // innerLoopBody->get_parent()->replace_statement(innerLoopBody,newStatement);
          ROSE_ASSERT (innerLoopBody->get_parent()->variant() == FOR_STMT);
       // printf ("Replace the statement in the loop body with a the modified current statement (now a loop body) \n");

#if USE_SAGE3
       // printf ("ERROR: SAGE 3 version of this code not yet implemented! \n");
       // ROSE_ABORT();

       // SgStatement* result = (*i)->replace (transformedStatement);
          (*i) = transformedStatement;
#else
          SgStatement* result = (*i).replace (transformedStatement);
          ROSE_ASSERT (result != NULL);
#endif
        }

#if 0
     printf ("#################################################################### \n");
     printf ("#################################################################### \n");
     printf ("######################  MODIFIED INNER LOOP (BEGIN)  ############### \n");
     printf ("#################################################################### \n");
     printf ("#################################################################### \n");

     showSgStatement ( cout, innerLoopBody, "CAll from ROSE_TransformationBase::editInnerLoopBodyToMatchCurrentStatement(SgStatement*,SgStatement*)" );

     printf ("#################################################################### \n");
     printf ("#################################################################### \n");
     printf ("######################  MODIFIED INNER LOOP (END)  ################# \n");
     printf ("#################################################################### \n");
     printf ("#################################################################### \n");
     printf ("\n\n");

     printf ("#################################################################### \n");
     printf ("################  CALL UNPARSE ON PARENT  ########################## \n");
     printf ("#################################################################### \n");

  // currentStatement->logical_unparse(NO_UNPARSE_INFO,cout);
     innerLoopBody->logical_unparse(NO_UNPARSE_INFO,cout);

     cout << endl << endl;
     cout << "####################################################################" << endl;
     cout << "####################################################################" << endl;

#endif

#if 0
     printf ("Exiting at BASE of ROSE_TransformationBase::editInnerLoopBodyToMatchCurrentStatement(SgStatement*,SgStatement*) \n");
     ROSE_ABORT();
#endif
   }


void
ROSE_TransformationBase::modifySymbol ( SgSymbol* symbol )
   {
     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("Inside of SimpleArrayAssignment::modifySymbol ( SgSymbol* symbol ); \n");

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          showSgSymbol ( cout, symbol, "SimpleArrayAssignment::modifySymbol()");

  // SgExpression *lhsOperand = buildLhsOperand("lhs",T_DOUBLE,"j",LAST_TAG,NULL);

     SgVariableSymbol* variableSymbol = isSgVariableSymbol( symbol );
     ROSE_ASSERT (variableSymbol != NULL);

     SgInitializedName* initializedName = variableSymbol->get_declaration();
     ROSE_ASSERT (initializedName != NULL);

     modifyInitializedName(initializedName);

  // printf ("Exiting inside of SimpleArrayAssignment::modifySymbol ( SgSymbol* symbol ) \n");
  // ROSE_ABORT();
   }


void
ROSE_TransformationBase::modifyInitializedName ( SgInitializedName* initializedName )
   {
     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("Inside of SimpleArrayAssignment::modifyInitializedName ( SgInitializedName* initializedName ); \n");

#if 0
     cout << "BEFORE MODIFICATION: Variable Name = " << initializedName->get_name().str() << endl;
     showSgInitializedName ( cout, initializedName, "SimpleArrayAssignment::modifyInitializedName()");
#endif

  // os << "variable name: ";
     ROSE_ASSERT (initializedName != NULL);
     const char* oldName = initializedName->get_name().str();
     char* copyOfOldName = new char [128];
     char* result = strcpy(copyOfOldName,oldName);
     ROSE_ASSERT (result != NULL);

  // Search for "_rosePrimativeVariable" in the existing name to 
  // indicate that this variable has already been transformed!
     char* newNameString = copyOfOldName;
     int i = 0;
     int strLength = strlen(copyOfOldName);
     char* testString = "_rosePrimativeVariable";
     int testStrLength = strlen(testString);
     Boolean stringFound = FALSE;
     while ( (i < strLength) && (stringFound == FALSE) )
        {
          int j = 0;
          if (copyOfOldName[i] == testString[j])
             {
               int k = i;
               Boolean found = TRUE;
	       while ((j < testStrLength) && (k < strLength) && (found == TRUE))
                  {
                    if (copyOfOldName[k] != testString[j]) 
                         found = FALSE;
                    k++;
		    j++;
                  }
               stringFound = found;
             }
          i++;
        }

     Boolean containsTransformedName = stringFound;
     if (containsTransformedName == FALSE)
        {
          newNameString = strcat (copyOfOldName,"_rosePrimativeVariable");

       // os << "variable type: " << endl;
       // SgType* type = name->get_type();

          SgTypeDouble* doubleType = new SgTypeDouble ();
          ROSE_ASSERT (doubleType != NULL);
     
       // Modify the variable name
          SgName newNameObject (newNameString);

       // Build a new SgInitializedName and reset the exiting on using operator=
          *initializedName = 
               SgInitializedName
                  ( newNameObject,
                    doubleType,
                 // initializedName->get_type(),
                    initializedName->get_initializer(),
                    initializedName->get_declaration(),
                    initializedName->get_named_item(),
                    initializedName->get_prev_decl_item());
	}
     
#if 0
     cout << "AFTER MODIFICATION: Variable Name = " << initializedName->get_name().str() << endl;
     showSgInitializedName ( cout, initializedName, "SimpleArrayAssignment::modifyInitializedName");
#endif

 // printf ("Exiting inside of SimpleArrayAssignment::modifyInitializedName ( SgInitializedName* initializedName ) \n");
 // ROSE_ABORT();
   }

